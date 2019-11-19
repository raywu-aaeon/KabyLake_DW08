/** @file
  Implements UsbTypeC Dxe driver and publish UsbTypeC Acpi table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2019 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#include <UsbTypeCDxe.h>

#include <Protocol/AcpiTable.h>

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/EcMiscLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/PcdLib.h>
#include <Protocol/GlobalNvsArea.h>
#include <PlatformInfo.h>
#include <PlatformBoardConfig.h>
#include <Library/BoardConfigLib.h>
#include <SetupVariable.h>
// AMI_OVERRIDE_START - Create AmiGetUcsiRevLib that OEM can override it to get rev from EC.
#include <Library/AmiGetUcsiRevLib.h>
// AMI_OVERRIDE_END - Create AmiGetUcsiRevLib that OEM can override it to get rev from EC.
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/FirmwareVolume2.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA           *mGlobalNvsAreaPtr;

#define USBTYPEC_DATA_VAR         L"UsbTypeC"
EFI_GUID UsbTypeCVarGuid      =  { 0xfc876842, 0xd8f0, 0x4844, {0xae, 0x32, 0x1f, 0xf8, 0x43, 0x79, 0x7b, 0x17} };

/**
  Allocate MemoryType below 4G memory address.

  @param[in] Size           Size of memory to allocate.
  @param[in] Buffer         Allocated address for output.

  @retval EFI_SUCCESS       Memory successfully allocated.
  @retval Other             Other errors occur.
**/
static EFI_STATUS
AllocateMemoryBelow4G (
  IN   EFI_MEMORY_TYPE MemoryType,
  IN   UINTN           Size,
  OUT  VOID           **Buffer
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;

  Pages = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  = (gBS->AllocatePages) (
                   AllocateMaxAddress,
                   MemoryType,
                   Pages,
                   &Address
                   );

  *Buffer = (VOID *) (UINTN) Address;

  return Status;
};

static UINT16 GetUcsiRev(VOID)
{
  UINT8         Rev1, Rev2;

  SendEcCommand( 0xE1);
  ReceiveEcData( &Rev1 );
  ReceiveEcData( &Rev2 );
  return ((Rev1 << 8) + Rev2 );
}
/**
  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

  @param[in] Protocol           The protocol to find.
  @param[in] Instance           Return pointer to the first instance of the protocol.
  @param[in] Type               TRUE if the desired protocol is a FV protocol.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_NOT_FOUND         The protocol could not be located.
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources to find the protocol.
**/
EFI_STATUS
LocateSupportProtocol (
  IN     EFI_GUID                      *Protocol,
  IN     EFI_GUID                      gEfiAcpiMultiTableStorageGuid,
     OUT VOID                          **Instance,
  IN     BOOLEAN                       Type
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              *HandleBuffer;
  UINTN                   NumberOfHandles;
  EFI_FV_FILETYPE         FileType;
  UINT32                  FvStatus;
  EFI_FV_FILE_ATTRIBUTES  Attributes;
  UINTN                   Size;
  UINTN                   Index;

  //
  // Locate protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Protocol,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    //
    // Defined errors at this time are not found and out of resources.
    //
    return Status;
  }

  //
  // Looking for FV with ACPI storage file
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {

    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    Protocol,
                    Instance
                    );
    ASSERT_EFI_ERROR (Status);

    if (!Type) {

      //
      // Not looking for the FV protocol, so find the first instance of the
      // protocol.  There should not be any errors because our handle buffer
      // should always contain at least one or LocateHandleBuffer would have
      // returned not found.
      //
      break;
    }

    //
    // See if it has the ACPI storage file
    //
    Size      = 0;
    FvStatus  = 0;
    Status = ((EFI_FIRMWARE_VOLUME2_PROTOCOL *) (*Instance))->ReadFile (
                                                              *Instance,
                                                              &gEfiAcpiMultiTableStorageGuid,
                                                              NULL,
                                                              &Size,
                                                              &FileType,
                                                              &Attributes,
                                                              &FvStatus
                                                              );

    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //
  //
  // Free any allocated buffers
  //
  FreePool (HandleBuffer);

  return Status;
}

/**
  Initialize and publish UsbTypeC ACPI table.

  @retval   EFI_SUCCESS     The UsbTypeC ACPI table is published successfully.
  @retval   Others          The UsbTypeC ACPI table is not published.

**/
EFI_STATUS
PublishAcpiTable (
  VOID
  )
{
  EFI_HANDLE                    *HandleBuffer;
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINT32                        FvStatus;
  UINTN                         Size;
  UINTN                         TableHandle;
  INTN                          Instance;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  EFI_GUID                      EfiAcpiMultiTableStorageGuid;

  EfiAcpiMultiTableStorageGuid = gEfiCallerIdGuid;
  HandleBuffer  = 0;
  Instance      = 0;
  TableHandle   = 0;
  CurrentTable  = NULL;
  FwVol         = NULL;

  DEBUG ((DEBUG_INFO, "InstallUsbTypeCAcpiTable\n"));
  //
  // Update OEM table ID
  //
  if (PcdGet64 (PcdUsbTypeCAcpiTableSignature) == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // Find the AcpiSupport protocol
  //
  Status = LocateSupportProtocol (
            &gEfiAcpiTableProtocolGuid,
            EfiAcpiMultiTableStorageGuid,
            (VOID **) &AcpiTable,
            FALSE
            );
  ASSERT_EFI_ERROR (Status);

  //
  // Locate the firmware volume protocol
  //
  Status = LocateSupportProtocol (
            &gEfiFirmwareVolume2ProtocolGuid,
            EfiAcpiMultiTableStorageGuid,
            (VOID **) &FwVol,
            TRUE
            );
  //
  // Read tables from the storage file.
  //
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &EfiAcpiMultiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;

      if (TableHeader->OemTableId == PcdGet64 (PcdUsbTypeCAcpiTableSignature)) {
        DEBUG ((DEBUG_INFO, "Install Usb Type C table : %x\n", TableHeader->OemTableId));
        //
        // Add the table
        //
        TableHandle = 0;
        CopyMem (TableHeader->OemId, PcdGetPtr (PcdAcpiDefaultOemId),sizeof (TableHeader->OemId));

        Status = AcpiTable->InstallAcpiTable (
                              AcpiTable,
                              CurrentTable,
                              CurrentTable->Length,
                              &TableHandle
                              );

        break;
      }

      //
      // Increment the instance
      //
      Instance++;
      CurrentTable = NULL;
    }
  }

  return Status;
}
/**
  This function is the entry point for this DXE driver.
  It creates ACPI table and publishes it.

  @param[in] ImageHandle       Image handle of this driver.
  @param[in] SystemTable       Global system service table.

  @retval EFI_DEVICE_ERROR     Initialization fails
  @retval EFI_SUCCESS          Initialization completes successfully.

**/
EFI_STATUS
EFIAPI
UsbTypeCEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS          Status;
  USBTYPEC_OPM_BUF    *OpmBuffer;
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.  
  SETUP_DATA          SetupData;
  UINTN               VariableSize;
  UINT32              VariableAttributes;
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#endif
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.  
  DEBUG ((DEBUG_INFO, "UsbTypeC entrypoint.\n"));

  Status = PublishAcpiTable ();
  ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.
  VariableSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable(
                  L"Setup",
                  &gSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &SetupData
                  );

  ASSERT_EFI_ERROR(Status);
  if ((!PcdGetBool (PcdUsbTypeCSupport)) || (!(SetupData.UsbTypeCAicSupport))) {
    DEBUG ((DEBUG_WARN, "UsbTypeC currently only supported on KBL RVP3, SKL RVP16, KBL DT RVP9 and Grizzly Mtn.\n"));
    return EFI_UNSUPPORTED;
  }
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#endif
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;
  //
  // Allocate memory in ACPI NVS
  //
  Status = AllocateMemoryBelow4G (EfiACPIMemoryNVS, 0x1000, (VOID **) &OpmBuffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SetMem (OpmBuffer, sizeof (USBTYPEC_OPM_BUF), 0x0);
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.
  OpmBuffer->Version =  GetUcsiRev();

// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
// AMI_OVERRIDE_START - Create AmiGetUcsiRevLib that OEM can override it to get rev from EC.
#else
  OpmBuffer->Version =  AmiGetUcsiRev();
// AMI_OVERRIDE_END - Create AmiGetUcsiRevLib that OEM can override it to get rev from EC.
#endif
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.
  
  mGlobalNvsAreaPtr->UsbTypeCOpBaseAddr = (UINT32) (UINTN) OpmBuffer;
  DEBUG ((DEBUG_INFO, "UsbTypeC EntryPoint: mGlobalNvsAreaPtr->UsbTypeCOpBaseAddr = 0x%X\n", mGlobalNvsAreaPtr->UsbTypeCOpBaseAddr));

  Status = gRT->SetVariable (
    USBTYPEC_DATA_VAR,
    &UsbTypeCVarGuid,
    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
    sizeof (UINT32),
    &(mGlobalNvsAreaPtr->UsbTypeCOpBaseAddr)
  );

  return Status;
}

