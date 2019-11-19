/** @file
  ACPI Platform Driver

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2019 Intel Corporation.

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

#include <Base.h>
#include <Uefi.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Protocol/FirmwareVolume2.h>
#include <PlatformBoardId.h>  // AdvancedFeaturesContent
#include <PlatformInfo.h>
#include <IndustryStandard/AcpiAml.h>
#include <IndustryStandard/Pci30.h>
#include "AcpiPlatform.h"
#include <Protocol/CpuGlobalNvsArea.h>
#include <Library/MmPciLib.h>
#include <Library/GpioExpanderLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcieRpLib.h>
#include <Library/PchSerialIoLib.h>
#include <Library/GpioLib.h>
#include <Library/BoardConfigLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <MeBiosPayloadHob.h> // AdvancedFeaturesContent
#include <PlatformBoardConfig.h>
#include <Board/BoardHook.h>
#include <Library/CpuPlatformLib.h>
#include <Library/MemoryAllocationLib.h>
#ifndef MINTREE_FLAG
#include <Library/TbtCommonLib.h>
#include <Register/PchRegsCam.h>
#endif // MINTREE_FLAG


// AMI_OVERRIDE_START - For AMI Chipset Setup
#include <Acpi50.h>
#include <AcpiGetSetupData/AcpiSetupData.h>
#include <NbSetupData.h>
#include <SbSetupData.h>
// AMI_OVERRIDE_END - For AMI Chipset Setup

// AMI_OVERRIDE_START - EIP204108
#include <Protocol/CpuGlobalNvsArea.h>  
// AMI_OVERRIDE_END - EIP204108

// AMI_OVERRIDE_START - For the implemtation of AMI's FACP update
#ifndef FADT_C3_LATENCY       // EIP204108 it should be defined at PowerMgmtInit.h
#define FADT_C3_LATENCY    57
#endif
// AMI_OVERRIDE_END - For the implemtation of AMI's FACP update

//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_INFO                             *mPlatformInfo;
GLOBAL_REMOVE_IF_UNREFERENCED VOID                                      *mGfxNvsAddress;
// AMI_OVERRIDE_START - For AMI Chipset Setup 
// GLOBAL_REMOVE_IF_UNREFERENCED SYSTEM_CONFIGURATION                      mSystemConfiguration;
ACPI_SETUP_DATA                                                         mSystemConfiguration = {0};
// AMI_OVERRIDE_END - For AMI Chipset Setup 
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA_PROTOCOL              mGlobalNvsArea;
GLOBAL_REMOVE_IF_UNREFERENCED CPU_GLOBAL_NVS_AREA_PROTOCOL              mCpuGlobalNvsArea;

GLOBAL_REMOVE_IF_UNREFERENCED EFI_CPU_IO2_PROTOCOL                      *mCpuIo;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_CPU_ID_ORDER_MAP                      mCpuApicIdOrderTable[MAX_CPU_NUM];
GLOBAL_REMOVE_IF_UNREFERENCED EFI_CPU_APIC_ID_REORDER_MAP               mCpuApicIdReorderTable[MAX_CPU_NUM];

GLOBAL_REMOVE_IF_UNREFERENCED EFI_CPU_PACKAGE_INFO                      mSbspPackageInfo;
#ifndef MINTREE_FLAG
GLOBAL_REMOVE_IF_UNREFERENCED ME_SETUP                                  mMeSetup;
// AMI_OVERRIDE_START - For AMI Chipset Setup 
#if 0
GLOBAL_REMOVE_IF_UNREFERENCED PCH_SETUP                                 mPchSetup;
#else
SB_SETUP_DATA                                                           mPchSetup = {0};
#endif
// AMI_OVERRIDE_END - For AMI Chipset Setup 
GLOBAL_REMOVE_IF_UNREFERENCED TBT_INFO_HOB                              *gTbtInfoHob;
#endif // MINTREE_FLAG
// AMI_OVERRIDE_START - Replace the DSDT GUID to AMI's definition
extern EFI_GUID gAmiAslDsdtFileGuid;
// AMI_OVERRIDE_END - Replace the DSDT GUID to AMI's definition

//
// Function implementations
//
#ifndef MINTREE_FLAG
/**
  Install Xhci ACPI Table

**/
VOID
InstallXhciAcpiTable (
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

// AMI_OVERRIDE_START - Replace the DSDT GUID to AMI's definition
//  EfiAcpiMultiTableStorageGuid = gEfiCallerIdGuid;
  EfiAcpiMultiTableStorageGuid = gRcAcpiTableStorageGuid;  
// AMI_OVERRIDE_END - Replace the DSDT GUID to AMI's definition
     
  HandleBuffer  = 0;
  Instance      = 0;
  TableHandle   = 0;
  CurrentTable  = NULL;
  FwVol         = NULL;

  DEBUG ((DEBUG_INFO, "InstallXhciAcpiTable\n"));

  //
  // Update OEM table ID
  //
  if (PcdGet64 (PcdXhciAcpiTableSignature) == 0) {
    return;
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

      if (TableHeader->OemTableId == PcdGet64 (PcdXhciAcpiTableSignature)) {
        DEBUG ((DEBUG_INFO, "Install xhci table: %x\n", TableHeader->OemTableId));
        //
        // Add the table
        //
        TableHandle = 0;

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
}

VOID
PublishOverClockingAcpiTable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;

  FwVol = NULL;
  Table = NULL;

  DEBUG ((DEBUG_INFO, "PublishOverClockingAcpiTable\n"));

  //
  // Locate FV protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Look for FV with ACPI storage file
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {

    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolume2ProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    //
    // See if it has the OverClocking Acpi Table file
    //
    Status = FwVol->ReadSection (
                      FwVol,
                      &gOcAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      0,
                      (VOID **) &Table,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  //
  // Free any allocated buffers
  //
  FreePool (HandleBuffer);

  //
  // Not find the table, it's fine the over clocking support is not enabled
  //
  if (Table == NULL) {
    return;
  }

  //
  // By default, publish it in all ACPI table versions.
  //
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
  if (CompareMem (&TableHeader->OemTableId, "PerfTune", 8) == 0) {
    TableHandle = 0;
    Status = AcpiTable->InstallAcpiTable (
                          AcpiTable,
                          Table,
                          Table->Length,
                          &TableHandle
                          );
  }
}
#endif // MINTREE_FLAG

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

/*
  This function will determine whether Native ASPM is supported on the platform.

  @param[in] SystemConfiguration     System configuration data

  @return TRUE if Native ASPM is supported, FALSE otherwise
*/
BOOLEAN
IsNativeAspmSupported (
// AMI_OVERRIDE_START - For AMI Chipset Setup
  CONST ACPI_SETUP_DATA* SystemConfiguration
// AMI_OVERRIDE_END - For AMI Chipset Setup
  )
{
  if (SystemConfiguration->PciExpNative == 0) {
    return FALSE;
  }
  switch (SystemConfiguration->NativeAspmEnable) {
  case 0:
    return FALSE;
  case 1:
    return TRUE;
  case 2: /* AUTO */
    return PchIsPcieNativeAspmSupported ();
  default:
    ASSERT (0);
  }
  return TRUE;
}

/**
  This function will update any runtime platform specific information.
  This currently includes:
    Setting OEM table values, ID, table ID, creator ID and creator revision.
    Enabling the proper processor entries in the APIC tables
  It also indicates with which ACPI table version the table belongs.

  @param[in] Table        The table to update
  @param[in] Version      Where to install this table

  @retval EFI_SUCCESS     Updated tables commplete.
**/
EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN OUT EFI_ACPI_TABLE_VERSION       *Version
  )
{
  EFI_ACPI_DESCRIPTION_HEADER                 *TableHeader;
  UINT8                                       *CurrPtr;
  UINT8                                       *TmpDsdtPointer;
  UINT8                                       *EndPtr;
  UINT8                                       *TempPtr;
  EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE *ApicPtr;
  UINT8                                       CurrProcessor;
  EFI_STATUS                                  Status;
  EFI_MP_SERVICES_PROTOCOL                    *MpService;
  UINTN                                       NumberOfCPUs;
  UINT8                                       MaximumDsdtPointLength;
  UINTN                                       NumberOfEnabledCPUs;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL             *PciRootBridgeIo;
  UINT32                                      HpetBaseAdress;
  UINT8                                       LocalApicCounter;
  //UINT32                                      GcsRegister;

#ifndef MINTREE_FLAG
  EFI_PEI_HOB_POINTERS                        Hob;
#endif
  CPU_GENERATION                              CpuGeneration;

  CurrPtr           = NULL;
  EndPtr            = NULL;
  ApicPtr           = NULL;
  TempPtr           = NULL;
  CurrProcessor     = 0;
  NumberOfCPUs      = 1;
  LocalApicCounter  = 0;
  MaximumDsdtPointLength = 20;
  CpuGeneration = GetCpuGeneration ();

  //
  // This will be accurate except for FACS structure
  //
  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

  //
  // Update the OEM and creator information for every table except FACS.
  //
// AMI_OVERRIDE_START - keep DSDT OemId
  if (Table->Signature != EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE &&
      Table->Signature != EFI_ACPI_1_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
// AMI_OVERRIDE_END - keep DSDT OemId.
    *(UINT32 *) (TableHeader->OemId)      = 'I' + ('N' << 8) + ('T' << 16) + ('E' << 24);
    *(UINT16 *) (TableHeader->OemId + 4)  = 'L' + (' ' << 8);

    //
    // Skip OEM table ID and creator information for DSDT, SSDT and PSDT tables, since these are
    // created by an ASL compiler and the creator information is useful.
    //
    if (Table->Signature != EFI_ACPI_1_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
        Table->Signature != EFI_ACPI_1_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
        Table->Signature != EFI_ACPI_1_0_PERSISTENT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE
        ) {

      //
      // Update OEM table ID
      //
      // Check if the silicon is KBL
#ifndef MINTREE_FLAG
      if (CpuGeneration == EnumKblCpu) {
        switch (mPlatformInfo->PlatformType) {
        case TypeTrad:
          TableHeader->OemTableId = ACPI_OEM_TABLE_ID_KBL;
          break;

        case TypeUltUlx:
          if (mPlatformInfo->BoardType == BoardTypeSds) {
            TableHeader->OemTableId = ACPI_OEM_TABLE_ID_KBL_SDS;
          } else {
            TableHeader->OemTableId = ACPI_OEM_TABLE_ID_KBL_ULT;
          }
          break;

        default:
          TableHeader->OemTableId = ACPI_OEM_TABLE_ID_KBL;
          break;
        }

      } else {

        switch (mPlatformInfo->PlatformType) {
        case TypeTrad:
          TableHeader->OemTableId = ACPI_OEM_TABLE_ID_SKL;
          break;

        case TypeUltUlx:
          if (mPlatformInfo->BoardType == BoardTypeSds) {
           TableHeader->OemTableId = ACPI_OEM_TABLE_ID_SKL_SDS;
          } else {
           TableHeader->OemTableId = ACPI_OEM_TABLE_ID_SKL_ULT;
          }
          break;

        default:
          TableHeader->OemTableId = ACPI_OEM_TABLE_ID_SKL;
          break;
        }
      }
#else
      TableHeader->OemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
#endif
      //
      // Update creator information
      //
      TableHeader->CreatorId        = EFI_ACPI_CREATOR_ID;
      TableHeader->CreatorRevision  = EFI_ACPI_CREATOR_REVISION;
    }
  }

  //
  // Locate the PCI IO protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID **) &PciRootBridgeIo
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Locate the MP services protocol
  // Find the MP Protocol. This is an MP platform, so MP protocol must be there.
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpService
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Determine the number of processors
  //
  MpService->GetNumberOfProcessors (
               MpService,
               &NumberOfCPUs,
               &NumberOfEnabledCPUs
               );

  //
  // By default, a table belongs in all ACPI table versions published.
  // Some tables will override this because they have different versions of the table.
  //
  *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  //
  // Update the various table types with the necessary updates
  //
  switch (Table->Signature) {

  case EFI_ACPI_1_0_APIC_SIGNATURE:
    //
    // if not MP and not APIC then don't publish the APIC tables.
    //
      //@todo mSystemConfiguration.ApicEnabled default value is 1. So, the following logic is not executed.
      //if (mSystemConfiguration.ApicEnabled == 0) {
      //  *Version = EFI_ACPI_TABLE_VERSION_NONE;
      //  break;
      //}
    //
    // Call for Local APIC ID Reorder
    //
    SortCpuLocalApicInTable (
      MpService,
      NumberOfCPUs,
      NumberOfEnabledCPUs
      );

    CurrPtr = (UINT8 *) &((EFI_ACPI_DESCRIPTION_HEADER *) Table)[1];
    CurrPtr = CurrPtr + 8;
    //
    // Size of Local APIC Address & Flag
    //
    EndPtr  = (UINT8 *) Table;
    EndPtr  = EndPtr + Table->Length;

    while (CurrPtr < EndPtr) {
      ApicPtr = (EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE *) CurrPtr;

      //
      // Check table entry type
      //
      if (ApicPtr->Type == EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC) {
        AppendCpuMapTableEntry (
          ApicPtr,
          NumberOfCPUs,
          NumberOfEnabledCPUs,
          LocalApicCounter
          );
        LocalApicCounter++;
      }

      //
      // Go to the next structure in the APIC table
      //
      CurrPtr = CurrPtr + ApicPtr->Length;
    }
    break;

#if 0  // AMI_OVERRIDE_START - FACP table is installed by ACPI module. Update these in AmiUpdateFacpTable().
  case EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
    //
    // Fix up all FACP Table values if configuration requires it.
    // This code fixes up the following Table values:
    // (1) C2/C3/CST Enable FACP values
    // (2) RTC S4 Flag
    //
    {
      EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *FadtPointer;

      FadtPointer = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *) Table;

      //
      // Check the version of the table
      //
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
      if (FadtPointer->Header.Revision == EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
        *Version = EFI_ACPI_TABLE_VERSION_NONE;
      } else if (FadtPointer->Header.Revision == EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
        *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0;
      } else if (FadtPointer->Header.Revision == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
        *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_3_0;
      } else if (FadtPointer->Header.Revision == EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
        *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_5_0;
      }
#ifndef MINTREE_FLAG
      //
      // Modify MSI capability based on setup
      //
      if((mSystemConfiguration.TbtSupport == 1) && (mSystemConfiguration.EnableMsiInFadt == 0)) {
        FadtPointer->IaPcBootArch |= (1 << 3);
      }
#endif

      //
      // Modify Preferred_PM_Profile field based on Board SKU's. Default is set to Mobile
      //
      FadtPointer->PreferredPmProfile = PcdGet8 (PcdPreferredPmProfile);

      //
      // if Native ASPM is disabled, set FACP table to skip Native ASPM
      //
      if (!IsNativeAspmSupported (&mSystemConfiguration)) {
        FadtPointer->IaPcBootArch |= 0x10;
      }

      //
      //PME WAKE supported, set PCI_EXP_WAK, BIT14 of Fixed feature flags.
      //
      FadtPointer->Flags |= (EFI_ACPI_6_0_PCI_EXP_WAK);

      if (mSystemConfiguration.LowPowerS0Idle) {
        // The Flags field within the FADT (offset 112)
        //   1) will have a new Low Power S0 Idle Capable ACPI flag (bit offset 21).
        FadtPointer->Flags = (BIT21 | FadtPointer->Flags);
        //Only passive docking available in Conected Standby mode. Clear Docking capability Bit
        FadtPointer->Flags &= ~BIT9;
      }
      //
      // Modify FADT Fixed Feature Flag to support 10 sec Power button.
      // If 10sec Power button is enabled: Set PWR_BUTTON(bit4) in Fixed Feature Flag(FACP offset 112)
      //
      if ((mSystemConfiguration.LowPowerS0Idle == 1) && (mSystemConfiguration.TenSecondPowerButtonEnable == 1)) {
        FadtPointer->Flags |= EFI_ACPI_2_0_PWR_BUTTON; // Set Fixed Power Button flag, means enable PB as a Control Method
      } else {
        FadtPointer->Flags &= ~(EFI_ACPI_2_0_PWR_BUTTON); // Clear fixed Power Button flag, means enable Power Button as Fixed PB
      }

      //
      // 1. set header revision.
      //
      FadtPointer->Header.Revision = EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION;

      //
      // 2. set all GAR register AccessSize to valid value.
      //
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->ResetReg.AccessSize    = EFI_ACPI_5_0_BYTE;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->XPm1aEvtBlk.AccessSize = EFI_ACPI_5_0_WORD;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->XPm1bEvtBlk.AccessSize = EFI_ACPI_5_0_WORD;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->XPm1aCntBlk.AccessSize = EFI_ACPI_5_0_WORD;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->XPm1bCntBlk.AccessSize = EFI_ACPI_5_0_WORD;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->XPm2CntBlk.AccessSize  = EFI_ACPI_5_0_BYTE;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->XPmTmrBlk.AccessSize   = EFI_ACPI_5_0_DWORD;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->XGpe0Blk.AccessSize    = EFI_ACPI_5_0_BYTE;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->XGpe1Blk.AccessSize    = EFI_ACPI_5_0_BYTE;

      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepControlReg.AddressSpaceId    = 0x1;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepControlReg.RegisterBitWidth  = 0x8;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepControlReg.RegisterBitOffset = 0;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepControlReg.AccessSize        = EFI_ACPI_5_0_DWORD;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepControlReg.Address           = EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 4;

      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepStatusReg.AddressSpaceId     = 0x1;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepStatusReg.RegisterBitWidth   = 0x8;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepStatusReg.RegisterBitOffset  = 0;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepStatusReg.AccessSize         = EFI_ACPI_5_0_DWORD;
      ((EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)FadtPointer)->SleepStatusReg.Address            = EFI_ACPI_PM1A_EVT_BLK_ADDRESS;

        //@todo RtcS4WakeEnable and PmTimerEnable default value is 1. So, the following logic is not executed.
        ////
        //// Check if RTC S4 is enabled
        ////
        //if (!mSystemConfiguration.RtcS4WakeEnable) {

        //  //
        //  // Clear the S4 RTC flag
        //  //
        //  FadtPointer->Flags &= ~EFI_ACPI_2_0_RTC_S4;
        //}

        ////
        //// Check if PM timer is enabled
        ////
        //if (!mSystemConfiguration.PmTimerEnable) {

        //  //
        //  // Clear the PM timer flag
        //  //
        //  FadtPointer->Flags &= ~EFI_ACPI_3_0_USE_PLATFORM_CLOCK;
        //}
    }
    break;
#endif  // AMI_OVERRIDE_END - FACP table is installed by ACPI module. Update these in AmiUpdateFacpTable().

  case EFI_ACPI_1_0_PERSISTENT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    //
    // These items only belong in the 1.0 tables.
    //
    *Version = EFI_ACPI_TABLE_VERSION_1_0B;
    break;

  case EFI_ACPI_2_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    //
    // Fix up the AML code in the DSDT affected by end user options.
    // Fix up the following ASL Code:
    // (1)  ACPI Global NVS Memory Base and Size.
    // (2)  ACPI Graphics NVS Memory Base and Size.
    // (3)  SMBus I/O Base.
    // (4)  Thermal Management Methods.
    //
    {
      UINT8   *DsdtPointer;
      UINT32  *Signature;
      UINT8   *Operation;
      UINT32  *Address;
      UINT8   *Value;
      UINT16  *Size;
      BOOLEAN EnterDock = FALSE;
// AMI_OVERRIDE_START - For the implemtation of AMI's FACP update
      UINTN   AcpiHandle = 0;
      EFI_ACPI_DESCRIPTION_HEADER *FacpTable;
      
      // Do not install the DSDT table. It is handled by ACPI module.
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
      
      Status = LocateAcpiTableBySignature (
                  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
                  (EFI_ACPI_DESCRIPTION_HEADER **)  &FacpTable,
                  &AcpiHandle
                  );
      ASSERT_EFI_ERROR (Status);
            
      //
      // Loop through the ASL looking for values that we must fix up.
      //
      //CurrPtr = (UINT8 *) Table;
      CurrPtr = (VOID*)(UINTN)((EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *)FacpTable)->Dsdt;
// AMI_OVERRIDE_END - For the implemtation of AMI's FACP update
      for (DsdtPointer = CurrPtr;
           DsdtPointer < (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);
           DsdtPointer++
          ) {
        Signature = (UINT32 *) DsdtPointer;
        switch (*Signature) {
        //
        // GNVS operation region
        //
        case (SIGNATURE_32 ('G', 'N', 'V', 'S')):
          //
          // Conditional match.  For Region Objects, the Operator will always be the
          // byte immediately before the specific name.  Therefore, subtract 1 to check
          // the Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_EXT_REGION_OP) {
// AMI_OVERRIDE_START - Print debug trace for debugging
            DEBUG((DEBUG_INFO, "ACPI Global NVS Ptr=0x%X, Length=0x%X.\n", (UINT32)(UINTN) mGlobalNvsArea.Area, sizeof (EFI_GLOBAL_NVS_AREA)));
// AMI_OVERRIDE_END - Print debug trace for debugging
            Address   = (UINT32 *) (DsdtPointer + 6);
            *Address  = (UINT32) (UINTN) mGlobalNvsArea.Area;
            Size      = (UINT16 *) (DsdtPointer + 11);
            *Size     = sizeof (EFI_GLOBAL_NVS_AREA);
          }
          break;

        //
        // _AC0 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '0')):
          //
          // Conditional match.  _AC0 is >63 and <4095 bytes, so the package length is 2 bytes.
          // Therefore, subtract 3 to check the Operator.
          //
          Operation = DsdtPointer - 3;
          if (*Operation == AML_METHOD_OP) {
            //
            // Check if we want _AC0 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '0');
            }
          }
          break;

        //
        // _AL0 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '0')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL0 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '0');
            }
          }
          break;

        //
        // _AC1 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '1')):
          //
          // Conditional match.  _AC1 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC1 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '1');
            }
          }
          break;

        //
        // _AL1 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '1')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL1 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '1');
            }
          }
          break;

        //
        // _AC2 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '2')):
          //
          // Conditional match.  _AC2 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC2 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '2');
            }
          }
          break;

        //
        // _AL2 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '2')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL2 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '2');
            }
          }
          break;

        //
        // _AC3 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '3')):
          //
          // Conditional match.  _AC3 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC3 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '3');
            }
          }
          break;

        //
        // _AL3 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '3')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL3 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '3');
            }
          }
          break;

        //
        // _AC4 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '4')):
          //
          // Conditional match.  _AC4 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC4 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '4');
            }
          }
          break;

        //
        // _AL4 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '4')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL4 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '4');
            }
          }
          break;

        //
        // _AC5 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '5')):
          //
          // Conditional match.  _AC5 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC5 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '5');
            }
          }
          break;

        //
        // _AL5 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '5')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL5 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '5');
            }
          }
          break;

        //
        // _AC6 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '6')):
          //
          // Conditional match.  _AC6 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC6 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '6');
            }
          }
          break;

        //
        // _AL6 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '6')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL6 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '6');
            }
          }
          break;

        //
        // _AC7 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '7')):
          //
          // Conditional match.  _AC7 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC7 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '7');
            }
          }
          break;

        //
        // _AL7 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '7')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL7 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '7');
            }
          }
          break;

        //
        // _AC8 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '8')):
          //
          // Conditional match.  _AC8 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC8 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '8');
            }
          }
          break;

        //
        // _AL8 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '8')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL8 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '8');
            }
          }
          break;

        //
        // _AC9 method
        //
        case (SIGNATURE_32 ('_', 'A', 'C', '9')):
          //
          // Conditional match.  _AC9 is < 63 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _AC9 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'C', '9');
            }
          }
          break;

        //
        // _AL9 method
        //
        case (SIGNATURE_32 ('_', 'A', 'L', '9')):
          //
          // Conditional match.  For Name Objects, the Operator will always be the byte
          // immediately before the specific name. Therefore, subtract 1 to check the
          // Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {

            //
            // Check if we want _AL9 enabled
            //
            if (mSystemConfiguration.DisableActiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'A', 'L', '9');
            }
          }
          break;

        //
        // _PSL method
        //
        case (SIGNATURE_32 ('_', 'P', 'S', 'L')):
          //
          // Conditional match.  _PSL is < 256 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 3;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _PSL enabled
            //
            if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'P', 'S', 'L');
            }
          }
          break;

        //
        // _PSV method
        //
        case (SIGNATURE_32 ('_', 'P', 'S', 'V')):
          //
          // Conditional match.  _PSV is < 256 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 3;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _PSV enabled
            //
            if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'P', 'S', 'V');
            }
          }
          break;

        //
        // _CRT method
        //
        case (SIGNATURE_32 ('_', 'C', 'R', 'T')):
          //
          // Conditional match.  _CRT is < 256 bytes, so the package length is 1 byte.
          // Subtract 3 to check the Operator for CRB, subract 2 for Harris Beach.
          //
          Operation = DsdtPointer - 3;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _CRT enabled
            //
            if (mSystemConfiguration.DisableCriticalTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'C', 'R', 'T');
            }
          }
          break;

        //
        // _TC1 method
        //
        case (SIGNATURE_32 ('_', 'T', 'C', '1')):
          //
          // Conditional match.  _TC1 is < 256 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _TC1 enabled
            //
            if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'T', 'C', '1');
            }
          }
          break;

        //
        // _TC2 method
        //
        case (SIGNATURE_32 ('_', 'T', 'C', '2')):
          //
          // Conditional match.  _TC2 is < 256 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _TC2 enabled
            //
            if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'T', 'C', '2');
            }
          }
          break;

        //
        // _TSP method
        //
        case (SIGNATURE_32 ('_', 'T', 'S', 'P')):
          //
          // Conditional match.  _TSP is < 256 bytes, so the package length is 1 byte.
          // Therefore, subtract 2 to check the Operator.
          //
          Operation = DsdtPointer - 2;
          if (*Operation == AML_METHOD_OP) {

            //
            // Check if we want _TSP enabled
            //
            if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'T', 'S', 'P');
            }
          }
          break;

#if 0  // AMI_OVERRIDE_START - Print debug trace for debugging
        //
        // Update SS3 Name with Setup value
        //
        case (SIGNATURE_32 ('S', 'S', '3', '_')):
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {
            Value   = (UINT8 *) DsdtPointer + 4;
            *Value  = (UINT8)mSystemConfiguration.AcpiSleepState;
          }
          break;
        //
        // Update SS4 Name with Setup value
        //
        case (SIGNATURE_32 ('S', 'S', '4', '_')):
          Operation = DsdtPointer - 1;
          if (*Operation == AML_NAME_OP) {
            Value   = (UINT8 *) DsdtPointer + 4;
            *Value  = mSystemConfiguration.AcpiHibernate;
          }
          break;
#endif  // AMI_OVERRIDE_END - Print debug trace for debugging
        //
        // _EJ0 method
        //
        case (SIGNATURE_32 ('_', 'E', 'J', '0')):
          if (mSystemConfiguration.LowPowerS0Idle) {
            //
            // Remove _EJ0 for SOC
            //
            if (*(DsdtPointer-3) == AML_METHOD_OP) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'E', 'J', '0');
              EnterDock = TRUE;
            }
          }
          break;
        //
        // _STA method for Device (\_SB.PCI0.DOCK)
        //
        case (SIGNATURE_32 ('_', 'S', 'T', 'A')):
          if (mSystemConfiguration.LowPowerS0Idle) {
            //
            // Remove _STA in (\_SB.PCI0.DOCK) for SOC
            //
            if ((*(DsdtPointer-3) == AML_METHOD_OP) && (EnterDock)) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'S', 'T', 'A');
              EnterDock = FALSE;
            }
          }
          break;
        //
        // _UPC method for Device (\_SB.PCI0.XHC.RHUB)
        //
        case (SIGNATURE_32('H', 'S', '1', '3')):
          for (TmpDsdtPointer = DsdtPointer;
               TmpDsdtPointer <= DsdtPointer + MaximumDsdtPointLength;
               TmpDsdtPointer++){
            Signature = (UINT32 *) TmpDsdtPointer;
            switch (*Signature) {
              case(SIGNATURE_32('U', 'P', 'C', 'P')):
                Value   = (UINT8 *)((UINT32 *)TmpDsdtPointer + 2);
                break;
              default:
                //
                // Do nothing.
                //
                break;
            }
          }
        break;


        //
        // _DCK method
        //
        case (SIGNATURE_32 ('_', 'D', 'C', 'K')):
          if (mSystemConfiguration.LowPowerS0Idle) {
            //
            // Remove _DCK for SOC
            //
            if (*(DsdtPointer-3) == AML_METHOD_OP) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'D', 'C', 'K');
            }
          }
          break;

        //
        // mask _DEP from CPU's scope if CS disabled.
        //
        case (SIGNATURE_32 ('P', 'R', '0', '0')):
        case (SIGNATURE_32 ('P', 'R', '0', '1')):
        case (SIGNATURE_32 ('P', 'R', '0', '2')):
        case (SIGNATURE_32 ('P', 'R', '0', '3')):
        case (SIGNATURE_32 ('P', 'R', '0', '4')):
        case (SIGNATURE_32 ('P', 'R', '0', '5')):
        case (SIGNATURE_32 ('P', 'R', '0', '6')):
        case (SIGNATURE_32 ('P', 'R', '0', '7')):
        case (SIGNATURE_32 ('P', 'R', '0', '8')):
        case (SIGNATURE_32 ('P', 'R', '0', '9')):
        case (SIGNATURE_32 ('P', 'R', '1', '0')):
        case (SIGNATURE_32 ('P', 'R', '1', '1')):
        case (SIGNATURE_32 ('P', 'R', '1', '2')):
        case (SIGNATURE_32 ('P', 'R', '1', '3')):
        case (SIGNATURE_32 ('P', 'R', '1', '4')):
        case (SIGNATURE_32 ('P', 'R', '1', '5')):

          if (mSystemConfiguration.LowPowerS0Idle == 0) {
            for (TmpDsdtPointer = DsdtPointer; TmpDsdtPointer <= DsdtPointer + MaximumDsdtPointLength; TmpDsdtPointer++){
              Signature = (UINT32 *) TmpDsdtPointer;
              switch (*Signature) {
                case(SIGNATURE_32('_', 'D', 'E', 'P')):
                  *(UINT8 *) TmpDsdtPointer = 'X';
                  break;
                default:
                  //
                  // Do nothing.
                  //
                  break;
              }
            }
          }
          break;

        //
        // _EDL name
        //
        case (SIGNATURE_32 ('_', 'E', 'D', 'L')):
          if (mSystemConfiguration.LowPowerS0Idle) {
            //
            // Remove _EDL for SOC
            //
            if (*(DsdtPointer-1) == AML_NAME_OP) {
              Signature   = (UINT32 *) DsdtPointer;
              *Signature  = SIGNATURE_32 ('X', 'E', 'D', 'L');
            }
          }
          break;
#ifndef MINTREE_FLAG
        //
        // TBT hotplug handler name
        //
        case (SIGNATURE_32 ('Y', 'T', 'B', 'T')):
          DEBUG((DEBUG_INFO, "(Tbt) YTBT found \n"));
          if (mSystemConfiguration.TbtSupport == 1) {
            Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &Hob.Raw);
            if (!EFI_ERROR (Status)) {
              for (; !END_OF_HOB_LIST(Hob); Hob.Raw = GET_NEXT_HOB(Hob)) {
                if ((GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_GUID_EXTENSION) && (CompareGuid(&Hob.Guid->Name, &gTbtInfoHobGuid))) {
                  if(mGlobalNvsArea.Area->Checkfor2tier == 0) {
// AMI_OVERRIDE_START - Add a DEBUG message when XTBT obj is found in ASL.
                    DEBUG((DEBUG_INFO, "(Tbt) update Thunderbolt GPE event \n"));
// AMI_OVERRIDE_END - Add a DEBUG message when XTBT obj is found in ASL.
                    *Signature = ((TBT_INFO_HOB *)Hob.Guid)->CioPlugEventGpio.AcpiGpeSignature;
                  }
                  break;
                }
              }
            }
          }
          break;

        case (SIGNATURE_32 ('T', 'D', 'M', 'A')):
          DEBUG((DEBUG_INFO, "(Tbt) TDMA found \n"));

          if (*(DsdtPointer-1) == AML_NAME_OP) {
            Address   = (UINT32 *) (DsdtPointer + 5);
            DEBUG((DEBUG_INFO, "Address of TDMA value pointer = 0x%lx \n", Address));
            DEBUG((DEBUG_INFO, "Initial TDMA value = 0x%lx \n", *Address));

            if (mSystemConfiguration.TbtSupport == 1) {
              *Address  = mSystemConfiguration.TbtDebugBaseAddressMem;
              DEBUG((DEBUG_INFO, "Final TDMA value = 0x%lx \n", *Address));
            }
          }
          break;

        case (SIGNATURE_32 ('T', 'D', 'P', 'G')):
          DEBUG((DEBUG_INFO, "(Tbt) TDPG found \n"));

          if (*(DsdtPointer-1) == AML_NAME_OP) {
            Address   = (UINT32 *) (DsdtPointer + 5);
            DEBUG((DEBUG_INFO, "Address of TDPG value pointer = 0x%lx \n", Address));
            DEBUG((DEBUG_INFO, "Initial TDPG value = 0x%lx \n", *Address));
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
            if ((mSystemConfiguration.TbtSupport == 1) && (mSystemConfiguration.TbtXConnectSupport == 1)) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
              *Address  = mSystemConfiguration.TbtDgpuPciBaseAddressMem;
              DEBUG((DEBUG_INFO, "Final TDPG value = 0x%lx \n", *Address));
            }
          }
          break;

        case (SIGNATURE_32 ('T', 'D', 'T', 'I')):
          DEBUG((DEBUG_INFO, "(Tbt) TDTI found \n"));

          if (*(DsdtPointer-1) == AML_NAME_OP) {
            Address   = (UINT32 *) (DsdtPointer + 5);
            DEBUG((DEBUG_INFO, "Address of TDTI value pointer = 0x%lx \n", Address));
            DEBUG((DEBUG_INFO, "Initial TDTI value = 0x%lx \n", *Address));

//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
            if ((mSystemConfiguration.TbtSupport == 1) && (mSystemConfiguration.TbtXConnectSupport == 1)) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
              *Address  = mSystemConfiguration.TbtDeviceTreeBuffer;
              DEBUG((DEBUG_INFO, "Final TDTI value = 0x%lx \n", *Address));
            }
          }
          break;
#endif // MINTREE_FLAG

        default:
          //
          // Do nothing.
          //
          break;
        }
      }
    }
    break;

  case EFI_ACPI_3_0_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE:
// AMI_OVERRIDE_START - Print debug trace for debugging
    DEBUG((DEBUG_INFO, "Update HPET Table.\n"));
// AMI_OVERRIDE_END - Print debug trace for debugging
    //
    // If HPET is disabled in setup, don't publish the table.
    //
//
// AdvancedFeaturesBegin
//
    if (mPchSetup.Hpet == FALSE) {
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
    } else {
//
// AdvancedFeaturesEnd
//
      //
      // Get HPET base address
      //
      PchHpetBaseGet (&HpetBaseAdress);
      //
      // Adjust HPET Table to correct the Base Address
      //
      ((EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER*) Table)->BaseAddressLower32Bit.Address = HpetBaseAdress;
    }  // AdvancedFeaturesContent
    break;

/* @todo Need to uncomment once MSFT fully implements uPEP
  case SIGNATURE_32 ('L', 'P', 'I', 'T'):
    //
    // If L0s is disabled in setup, don't publish the table.
    //
    if (mSystemConfiguration.LowPowerS0Idle == 0) {
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
    }
    break;
*/
  case EFI_ACPI_1_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
#ifndef MINTREE_FLAG
  {
#if defined CRB_THERMAL_ASL_SUPPORT && CRB_THERMAL_ASL_SUPPORT 	// AMI_OVERRIDE_START - Add CRB_THERMAL_ASL_SUPPORT token
    UINT8   *TablePointer;
    UINT32  *Signature;
    UINT8   *Operation;
#endif                                                          // AMI_OVERRIDE_END - Add CRB_THERMAL_ASL_SUPPORT token
#endif // MINTREE_FLAG
    //
    // Do not load the xHCI table. It is handled by separate function.
    //
    if (CompareMem (&TableHeader->OemTableId, "xh_", 3) == 0) {
      DEBUG((DEBUG_INFO, "TableHeader->OemTableId = %x\n ", TableHeader->OemTableId));
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
    }
#ifndef MINTREE_FLAG
    //
    // Load SSDT tables for the platforms based on boardID; default to RVP tables.
    // Load Ther_Sds for Skylake SDS, PantherMtn, or StarBrook.
    // Load Ther_Rvp for all others.
    //
// AMI_OVERRIDE_START - Add CRB_THERMAL_ASL_SUPPORT token
#if defined CRB_THERMAL_ASL_SUPPORT && CRB_THERMAL_ASL_SUPPORT   
// AMI_OVERRIDE_END - Add CRB_THERMAL_ASL_SUPPORT token
    if (CompareMem (&TableHeader->OemTableId, "Ther_Sds", 8) == 0) {
      if (!PcdGetBool (PcdTherSdsTableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_NONE;
      } else {
        CurrPtr = (UINT8 *) Table;
        for (TablePointer = CurrPtr;TablePointer < (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);TablePointer++)
        {
          Signature = (UINT32 *) TablePointer;
          switch (*Signature) {
          //
          // _AC0 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '0')):
            //
            // Conditional match.  _AC0 is >63 and <4095 bytes, so the package length is 2 bytes.
            // Therefore, subtract 3 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _AC0 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '0');
              }
            }
            break;
          //
          // _AL0 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '0')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {

              //
              // Check if we want _AL0 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '0');
              }
            }
            break;
          //
          // _AC1 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '1')):
            //
            // Conditional match.  _AC0 is >63 and <4095 bytes, so the package length is 2 bytes.
            // Therefore, subtract 3 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {

              //
              // Check if we want _AC1 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '1');
              }
            }
            break;
          //
          // _AL1 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '1')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {

              //
              // Check if we want _AL1 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '1');
              }
            }
            break;
          //
          // _AC2 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '2')):
            //
            // Conditional match.  _AC0 is >63 and <4095 bytes, so the package length is 2 bytes.
            // Therefore, subtract 3 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {

              //
              // Check if we want _AC2 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '2');
              }
            }
            break;
          //
          // _AL2 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '2')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {

              //
              // Check if we want _AL2 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '2');
              }
            }
            break;
          //
          // _AC3 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '3')):
            //
            // Conditional match.  _AC0 is >63 and <4095 bytes, so the package length is 2 bytes.
            // Therefore, subtract 3 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _AC3 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '3');
              }
            }
            break;
          //
          // _AL3 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '3')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {
              //
              // Check if we want _AL3 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '3');
              }
            }
            break;
          // _PSL method
          //
          case (SIGNATURE_32 ('_', 'P', 'S', 'L')):
            //
            // Conditional match.  _PSL is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _PSL enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'P', 'S', 'L');
              }
            }
            break;
          //
          // _PSV method
          //
          case (SIGNATURE_32 ('_', 'P', 'S', 'V')):
            //
            // Conditional match.  _PSV is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _PSV enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'P', 'S', 'V');
              }
            }
            break;
          //
          // _CRT method
          //
          case (SIGNATURE_32 ('_', 'C', 'R', 'T')):
            //
            // Conditional match.  _CRT is < 256 bytes, so the package length is 1 byte.
            // Subtract 3 to check the Operator for CRB, subract 2 for Harris Beach.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _CRT enabled
              //
              if (mSystemConfiguration.DisableCriticalTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'C', 'R', 'T');
              }
            }
            break;
          //
          // _TC1 method
          //
          case (SIGNATURE_32 ('_', 'T', 'C', '1')):
            //
            // Conditional match.  _TC1 is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _TC1 enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'T', 'C', '1');
              }
            }
            break;
          //
          // _TC2 method
          //
          case (SIGNATURE_32 ('_', 'T', 'C', '2')):
            //
            // Conditional match.  _TC2 is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _TC2 enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'T', 'C', '2');
              }
            }
            break;
          //
          // _TSP method
          //
          case (SIGNATURE_32 ('_', 'T', 'S', 'P')):
            //
            // Conditional match.  _TSP is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _TSP enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'T', 'S', 'P');
              }
            }
            break;
          }
        }
      }
    }

// AMI_OVERRIDE_START - Add CRB_THERMAL_ASL_SUPPORT token	
#else
    if (CompareMem (&TableHeader->OemTableId, "Ther_Sds", 8) == 0) {
            *Version = EFI_ACPI_TABLE_VERSION_NONE;
    }    
#endif
    
#if defined CRB_THERMAL_ASL_SUPPORT && CRB_THERMAL_ASL_SUPPORT
// AMI_OVERRIDE_END - Add CRB_THERMAL_ASL_SUPPORT token
    if (CompareMem (&TableHeader->OemTableId, "Ther_Rvp", 8) == 0) {
      if (PcdGetBool (PcdTherSdsTableEnable)) {
        *Version = EFI_ACPI_TABLE_VERSION_NONE;
      } else {
        CurrPtr = (UINT8 *) Table;
        for (TablePointer = CurrPtr;TablePointer < (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);TablePointer++)
        {
          Signature = (UINT32 *) TablePointer;
          switch (*Signature) {
          //
          // _AC0 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '0')):
            //
            // Conditional match.  _AC0 is >63 and <4095 bytes, so the package length is 2 bytes.
            // Therefore, subtract 3 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _AC0 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '0');
              }
            }
            break;
          //
          // _AL0 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '0')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {
              //
              // Check if we want _AL0 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '0');
              }
            }
            break;
          //
          // _AC1 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '1')):
            //
            // Conditional match.  _AC1 is < 63 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _AC1 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '1');
              }
            }
            break;
          //
          // _AL1 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '1')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {
              //
              // Check if we want _AL1 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '1');
              }
            }
            break;
          //
          // _AC2 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '2')):
            //
            // Conditional match.  _AC2 is < 63 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _AC2 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '2');
              }
            }
            break;
          //
          // _AL2 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '2')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {
              //
              // Check if we want _AL2 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '2');
              }
            }
            break;
          //
          // _AC3 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '3')):
            //
            // Conditional match.  _AC3 is < 63 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _AC3 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '3');
              }
            }
            break;
          //
          // _AL3 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '3')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {
              //
              // Check if we want _AL3 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '3');
              }
            }
            break;
          //
          // _AC4 method
          //
          case (SIGNATURE_32 ('_', 'A', 'C', '4')):
            //
            // Conditional match.  _AC4 is < 63 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _AC4 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'C', '4');
              }
            }
            break;
          //
          // _AL4 method
          //
          case (SIGNATURE_32 ('_', 'A', 'L', '4')):
            //
            // Conditional match.  For Name Objects, the Operator will always be the byte
            // immediately before the specific name. Therefore, subtract 1 to check the
            // Operator.
            //
            Operation = TablePointer - 1;
            if (*Operation == AML_NAME_OP) {
              //
              // Check if we want _AL4 enabled
              //
              if (mSystemConfiguration.DisableActiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'A', 'L', '4');
              }
            }
            break;
          // _PSL method
          //
          case (SIGNATURE_32 ('_', 'P', 'S', 'L')):
            //
            // Conditional match.  _PSL is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _PSL enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'P', 'S', 'L');
              }
            }
            break;
          //
          // _PSV method
          //
          case (SIGNATURE_32 ('_', 'P', 'S', 'V')):
            //
            // Conditional match.  _PSV is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _PSV enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'P', 'S', 'V');
              }
            }
            break;
          //
          // _CRT method
          //
          case (SIGNATURE_32 ('_', 'C', 'R', 'T')):
            //
            // Conditional match.  _CRT is < 256 bytes, so the package length is 1 byte.
            // Subtract 3 to check the Operator for CRB, subract 2 for Harris Beach.
            //
            Operation = TablePointer - 3;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _CRT enabled
              //
              if (mSystemConfiguration.DisableCriticalTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'C', 'R', 'T');
              }
            }
            break;
          //
          // _TC1 method
          //
          case (SIGNATURE_32 ('_', 'T', 'C', '1')):
            //
            // Conditional match.  _TC1 is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _TC1 enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'T', 'C', '1');
              }
            }
            break;
          //
          // _TC2 method
          //
          case (SIGNATURE_32 ('_', 'T', 'C', '2')):
            //
            // Conditional match.  _TC2 is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _TC2 enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'T', 'C', '2');
              }
            }
            break;
          //
          // _TSP method
          //
          case (SIGNATURE_32 ('_', 'T', 'S', 'P')):
            //
            // Conditional match.  _TSP is < 256 bytes, so the package length is 1 byte.
            // Therefore, subtract 2 to check the Operator.
            //
            Operation = TablePointer - 2;
            if (*Operation == AML_METHOD_OP) {
              //
              // Check if we want _TSP enabled
              //
              if (mSystemConfiguration.DisablePassiveTripPoints == 0) {
                Signature   = (UINT32 *) TablePointer;
                *Signature  = SIGNATURE_32 ('X', 'T', 'S', 'P');
              }
            }
            break;
          }
        }
      }
    }
// AMI_OVERRIDE_START - Add CRB_THERMAL_ASL_SUPPORT token	
#else    
    if (CompareMem (&TableHeader->OemTableId, "Ther_Rvp", 8) == 0) {
    	    *Version = EFI_ACPI_TABLE_VERSION_NONE;
    }
#endif
// AMI_OVERRIDE_END - Add CRB_THERMAL_ASL_SUPPORT token	

    //
    // Load RTD3 SSDT table for SDS, RVP3, RVP7, RVP13
    //
    if ((CompareMem (&TableHeader->OemTableId, "RVP3Rtd3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "RV31Rtd3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "RVP7Rtd3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "RVPRRtd3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "RVKcRtd3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "RV10Rtd3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "RV11Rtd3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "SDBKRtd3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "SDS_Rtd3", 8) == 0) ||
// AMI_OVERRIDE_START - Report ACPI RTD3 of other board
        (CompareMem (&TableHeader->OemTableId, "OEM_RTD3", 8) == 0) ||
// AMI_OVERRIDE_END - Report ACPI RTD3 of other board
        (CompareMem (&TableHeader->OemTableId, "RV3Rd3AR", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "AMLD3TBT", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "AMLY42D3", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "AML42TBT", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "AMAEPTBT", 8) == 0) ||
        (CompareMem (&TableHeader->OemTableId, "RV3Rd3WW", 8) == 0)) {
      //
      // set default = none
      //
      *Version = EFI_ACPI_TABLE_VERSION_NONE;

      //
      // check RTD3 enabled in setup
      //
      if (mSystemConfiguration.Rtd3Support == 1) {
        //
        // match table ID and BoardId
        //

        // load RTD3 table for RVP3/RVP13 if AR AIC and PCIe WWAN support is not enabled.
        if ((CompareMem (&TableHeader->OemTableId, "RVP3Rtd3", 8) == 0) &&
            (mSystemConfiguration.AicArSupport == 0) &&
            (PcdGet8 (PcdWwanInterface) != 1)) {
          if (PcdGetBool (PcdRvp3Rtd3TableEnable) || PcdGetBool (PcdAmlRtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for AML Y42 if AR AIC and PCIe WWAN support is not enabled.
        if ((CompareMem (&TableHeader->OemTableId, "AMLY42D3", 8) == 0) &&
            (mSystemConfiguration.AicArSupport == 0)) {
          if (PcdGetBool (PcdAmlY42Rtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for RVP7
        if (CompareMem (&TableHeader->OemTableId, "RVP7Rtd3", 8) == 0) {
          if (PcdGetBool (PcdRvp7Rtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for RVPR
        if (CompareMem (&TableHeader->OemTableId, "RVPRRtd3", 8) == 0) {
          if (PcdGetBool (PcdRvpRRtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for RVPKC
        if (CompareMem (&TableHeader->OemTableId, "RVKcRtd3", 8) == 0) {
          if (PcdGetBool (PcdRvpKcRtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for RVP10
        if (CompareMem (&TableHeader->OemTableId, "RV10Rtd3", 8) == 0) {
          if (PcdGetBool (PcdRvp10Rtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for RVP11
        if (CompareMem (&TableHeader->OemTableId, "RV11Rtd3", 8) == 0) {
          if (PcdGetBool (PcdRvp11Rtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for Saddlebrook
        if (CompareMem (&TableHeader->OemTableId, "SDBKRtd3", 8) == 0) {
          if (PcdGetBool (PcdSdlBrkRtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 |EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for SDS, PantherMtn, or StarBrook
        if (CompareMem (&TableHeader->OemTableId, "SDS_Rtd3", 8) == 0) {
          if (PcdGetBool (PcdSdsRtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for RVP3 TBT AIC if AR AIC support is enabled.
        if ((CompareMem (&TableHeader->OemTableId, "RV3Rd3AR", 8) == 0) &&
            (mSystemConfiguration.AicArSupport == 1) &&
            (PcdGet8 (PcdWwanInterface) != 1)) {
          if (PcdGetBool (PcdRvp3Rtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for AML TBT AIC if AR AIC support is enabled.
        if ((CompareMem (&TableHeader->OemTableId, "AMLD3TBT", 8) == 0) &&
            (mSystemConfiguration.AicArSupport == 1) &&
            (PcdGet8 (PcdWwanInterface) != 1)) {
          if (PcdGetBool (PcdAmlRtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for AML Y42 TBT AIC if AR AIC support is enabled.
        if ((CompareMem (&TableHeader->OemTableId, "AML42TBT", 8) == 0) &&
            (mSystemConfiguration.AicArSupport == 1)) {
          if (PcdGetBool (PcdAmlY42Rtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }
        // load RTD3 table for AML Y42 Aep TBT AIC if AR AIC support is enabled.
        if ((CompareMem (&TableHeader->OemTableId, "AMAEPTBT", 8) == 0)) {
          if (PcdGetBool (PcdAmlY42CffRtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }

        // load RTD3 table for RVP3 PCIe WWAN if WWAN interface is PCIe only.
        if ((CompareMem (&TableHeader->OemTableId, "RV3Rd3WW", 8) == 0) &&
            (PcdGet8 (PcdWwanInterface) == 1)) {
          if (PcdGetBool (PcdRvp3Rtd3TableEnable) || PcdGetBool (PcdAmlRtd3TableEnable)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
          }
        }
// AMI_OVERRIDE_START - Report ACPI RTD3 of other board
        if (CompareMem (&TableHeader->OemTableId, "OEM_RTD3", 8) == 0) {
          if ((mPlatformInfo->BoardID != BoardIdSkylakeSds)              &&
              (mPlatformInfo->BoardID != BoardIdSkylakePhabletPoc)       &&
              (mPlatformInfo->BoardID != BoardIdSkylakePantherMtn)       &&
              (mPlatformInfo->BoardID != BoardIdSkylakeStarbrook)        &&
              (mPlatformInfo->BoardID != BoardIdSkylakeHaloDdr4Rvp11)    &&
              (mPlatformInfo->BoardID != BoardIdSkylakeHaloDdr4Rvp11Ppv) &&
              (mPlatformInfo->BoardID != BoardIdSkylakeHaloLpddr3Rvp16)  &&
              (mPlatformInfo->BoardID != BoardIdSkylakeAioRvp10Crb)      &&
              (mPlatformInfo->BoardID != BoardIdSkylakeAioRvp10Evp)      &&
              (mPlatformInfo->BoardID != BoardIdSkylakeAioRvp10CrbPpv)   &&
              (mPlatformInfo->BoardID != BoardIdSkylakeURvp7)            &&
              (mPlatformInfo->BoardID != BoardIdSkylakeURvp7Ppv)         &&
              (mPlatformInfo->BoardID != BoardIdSkylakeURvp15)           &&
              (mPlatformInfo->BoardID != BoardIdSkylakeA0Rvp3)           &&
              (mPlatformInfo->BoardID != BoardIdSkylakeA0Rvp3Ppv)        &&
              (mPlatformInfo->BoardID != BoardIdSkylakeYRvp13)) {
            *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 |EFI_ACPI_TABLE_VERSION_3_0;
            DEBUG((DEBUG_INFO, "Update SSDT OEM_RTD3 Table.\n"));        
          }
        }
// AMI_OVERRIDE_END - Report ACPI RTD3 of other board
      } // Rtd3Support
    } // Load RTD3 SSDT table for SDS, RVP3, RVP13
  }
#endif // MINTREE_FLAG
    break;

  default:
    break;
  }
  return EFI_SUCCESS;
}

/**
  This function calculates RCR based on PCI Device ID and Vendor ID from the devices
  available on the platform.
  It also includes other instances of BIOS change to calculate CRC and provides as
  HWSignature filed in FADT table.
**/
VOID
IsHardwareChange (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT32                        CRC;
  UINT32                        *HWChange;
  UINTN                         HWChangeSize;
  UINT32                        PciId;
  UINTN                         Handle;
  EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *FacsPtr;
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE    *pFADT;

  HandleCount  = 0;
  HandleBuffer = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return; // PciIO protocol not installed yet!
  }

  //
  // Allocate memory for HWChange and add additional entrie for
  // pFADT->XDsdt
  //
  HWChangeSize = HandleCount + 1;
  HWChange = AllocateZeroPool( sizeof(UINT32) * HWChangeSize );
  ASSERT( HWChange != NULL );

  if (HWChange == NULL) return;

  //
  // add HWChange inputs: PCI devices
  //
  for (Index = 0; HandleCount > 0; HandleCount--) {
    PciId = 0;
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID **) &PciIo);
    if (!EFI_ERROR (Status)) {
      Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
      if (EFI_ERROR (Status)) {
        continue;
      }
      HWChange[Index++] = PciId;
    }
  }

  //
  // Locate FACP Table
  //
  Handle = 0;
  Status = LocateAcpiTableBySignature (
              EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
              (EFI_ACPI_DESCRIPTION_HEADER **) &pFADT,
              &Handle
              );
  if (EFI_ERROR (Status) || (pFADT == NULL)) {
    return;  //Table not found or out of memory resource for pFADT table
  }

  //
  // add HWChange inputs: others
  //
  HWChange[Index++] = (UINT32)pFADT->XDsdt;

  //
  // Calculate CRC value with HWChange data.
  //
  Status = gBS->CalculateCrc32(HWChange, HWChangeSize, &CRC);
  DEBUG((DEBUG_INFO, "CRC = %x and Status = %r\n", CRC, Status));

  //
  // Set HardwareSignature value based on CRC value.
  //
  FacsPtr = (EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)pFADT->FirmwareCtrl;
  FacsPtr->HardwareSignature = CRC;
  FreePool( HWChange );
}

EFI_STATUS
PublishAcpiTablesFromFv (
  IN EFI_GUID gEfiAcpiMultiTableStorageGuid
  )
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINT32                        FvStatus;
  UINTN                         Size;
  EFI_ACPI_TABLE_VERSION        Version;
  UINTN                         TableHandle;
  INTN                          Instance;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;

  Instance      = 0;
  TableHandle   = 0;
  CurrentTable  = NULL;
  FwVol         = NULL;

  //
  // Find the AcpiSupport protocol
  //
  Status = LocateSupportProtocol (
            &gEfiAcpiTableProtocolGuid,
            gEfiAcpiMultiTableStorageGuid,
            (VOID **) &AcpiTable,
            FALSE
            );
  ASSERT_EFI_ERROR (Status);

  //
  // Locate the firmware volume protocol
  //
  Status = LocateSupportProtocol (
            &gEfiFirmwareVolume2ProtocolGuid,
            gEfiAcpiMultiTableStorageGuid,
            (VOID **) &FwVol,
            TRUE
            );

  /* TODO: Hang at native mode. Need to find the cause
   * when the related drivers are finished for native support. */
  //ASSERT_EFI_ERROR (Status);

  //
  // Read tables from the storage file.
  //

  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gEfiAcpiMultiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {

      //
      // Perform any table specific updates.
      //
      PlatformUpdateTables (CurrentTable, &Version);

      //
      // Add the table
      //
      TableHandle = 0;

      if (Version != EFI_ACPI_TABLE_VERSION_NONE) {
        Status = AcpiTable->InstallAcpiTable (
                              AcpiTable,
                              CurrentTable,
                              CurrentTable->Length,
                              &TableHandle
                              );
      }

      ASSERT_EFI_ERROR (Status);

      //
      // Increment the instance
      //
      Instance++;
      CurrentTable = NULL;
    }
  }

  //
  // Finished
  //
  return EFI_SUCCESS;
}


VOID
EFIAPI
AcpiEndOfDxeEvent (
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
  )
{
//
// AdvancedFeaturesBegin
//
  COMA_V_DATA     ComaData;
  COMA_NV_DATA    ComaNvData;
  COMB_NV_DATA    CombNvData;
  EFI_GUID        SioGuid = SIO_VAR_GUID;
  UINTN           Size;
  EFI_STATUS      Status;
//
// AdvancedFeaturesEnd
//

  if (Event != NULL) {
    gBS->CloseEvent(Event);
  }

// AMI_OVERRIDE_START - For the implemtation of AMI's FACP update
  AmiUpdateFacpTable ();
// AMI_OVERRIDE_END - For the implemtation of AMI's FACP update
//
// AdvancedFeaturesBegin
//
  //
  // According to real COM settings to update global NV area
  //
  Size = sizeof (COMA_V_DATA);
  ZeroMem (&ComaData, Size);
  Status = gRT->GetVariable (
                  L"PNP0501_0_VV",
                  &SioGuid,
                  NULL,
                  &Size,
                  &ComaData
                  );
  if (!EFI_ERROR (Status) && ComaData.ComImplemented != 0) {
    mGlobalNvsArea.Area->SMSC1007 = GLOBAL_NVS_DEVICE_ENABLE;
  } else {
    mGlobalNvsArea.Area->SMSC1007 = GLOBAL_NVS_DEVICE_DISABLE;
  }

  Size = sizeof (COMA_NV_DATA);
  ZeroMem (&ComaNvData, Size);
  Status = gRT->GetVariable (
                  L"PNP0501_0_NV",
                  &SioGuid,
                  NULL,
                  &Size,
                  &ComaNvData
                  );
  if (!EFI_ERROR (Status) && ComaNvData.ComEnable != 0) {
    mGlobalNvsArea.Area->OnboardCom = GLOBAL_NVS_DEVICE_ENABLE;
  } else {
    mGlobalNvsArea.Area->OnboardCom = GLOBAL_NVS_DEVICE_DISABLE;
  }
  Size = sizeof (COMB_NV_DATA);
  ZeroMem (&CombNvData, Size);
  Status = gRT->GetVariable (
                  L"PNP0510_1_NV",
                  &SioGuid,
                  NULL,
                  &Size,
                  &CombNvData
                  );
  if (!EFI_ERROR (Status) && CombNvData.ComEnable != 0) {
    mGlobalNvsArea.Area->OnboardComCir = GLOBAL_NVS_DEVICE_ENABLE;
  } else {
    mGlobalNvsArea.Area->OnboardComCir = GLOBAL_NVS_DEVICE_DISABLE;
  }
//
// AdvancedFeaturesEnd
//

  //
  // Calculate Hardware Signature value based on current platform configurations
  //
  IsHardwareChange();
}

/**
  Allocate MemoryType below 4G memory address.

  @param[in] Size     - Size of memory to allocate.
  @param[in] Buffer   - Allocated address for output.

  @retval EFI_SUCCESS - Memory successfully allocated.
  @retval Other       - Other errors occur.

**/
EFI_STATUS
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
}

/**
  Convert string containing GUID in the canonical form:
  "aabbccdd-eeff-gghh-iijj-kkllmmnnoopp"
  where aa - pp are unicode hexadecimal digits
  to the buffer format to be used in ACPI, byte ordering:
  [Byte 0] gg, hh, ee, ff, aa, bb, cc, dd [Byte 7]
  [Byte 8] pp, oo, nn, mm, ll, kk, jj, ii [Byte 16]

  @param[in]  GuidString    - GUID String null terminated (aligned on a 16-bit boundary)
  @param[out] AcpiGuidPart1 - First half of buffer  (bytes 0 - 7)
  @param[out] AcpiGuidPart2 - Second half of buffer (bytes 8 - 16)

  @retval EFI_SUCCESS     - String converted successfully.
  @retval EFI_UNSUPPORTED - Wrong input string format.

**/
EFI_STATUS
GuidStringToAcpiBuffer (
  IN  CHAR16 *GuidString,
  OUT UINT64 *AcpiGuidPart1,
  OUT UINT64 *AcpiGuidPart2
  )
{
  UINT32 GuidTempPart32 = 0;
  UINT16 GuidTempPart16 = 0;
  UINT8  GuidPartIndex;

  DEBUG((DEBUG_INFO,"GuidStringToAcpiBuffer() - GUID = %s\n", GuidString));

  for (GuidPartIndex = 0; GuidPartIndex < 4; GuidPartIndex++) {
    switch (GuidPartIndex){
      case 0:
        GuidTempPart32 = SwapBytes32((UINT32)StrHexToUint64(GuidString));
        *AcpiGuidPart1 = ((UINT64)GuidTempPart32 << 0x20);
        break;
      case 1:
        GuidTempPart16 = SwapBytes16((UINT16)StrHexToUint64(GuidString));
        *AcpiGuidPart1 += ((UINT64)GuidTempPart16 << 0x10);
        break;
      case 2:
        GuidTempPart16 = SwapBytes16((UINT16)StrHexToUint64(GuidString));
        *AcpiGuidPart1 += ((UINT64)GuidTempPart16);
        break;
      case 3:
        GuidTempPart16 = (UINT16)StrHexToUint64(GuidString);
        break;
      default:
        return EFI_UNSUPPORTED;
    }

    while ((*GuidString != L'-') && (*GuidString != L'\0')) {
      GuidString++;
    }

    if (*GuidString == L'-') {
      GuidString++;
    } else {
      return EFI_UNSUPPORTED;
    }
  }

  *AcpiGuidPart2 = ((UINT64)GuidTempPart16 << 0x30) + StrHexToUint64(GuidString);

  // Switch endianess because it will be swapped again in ACPI Buffer object
  *AcpiGuidPart1 = SwapBytes64(*AcpiGuidPart1);
  *AcpiGuidPart2 = SwapBytes64(*AcpiGuidPart2);

  return EFI_SUCCESS;
}

/**
  Save Acpi Setup relatived variables.
**/
#if 0  // AMI_OVERRIDE_START - For AMI Chipset Setup
EFI_STATUS
SetAcpiSetupVariables (
  VOID
  )
{
  EFI_STATUS                   Status;
  UINTN                        Handle;
  SETUP_DATA                   SetupData;
  UINTN                        VariableSize;
  UINT32                       VariableAttributes;
  EFI_ACPI_DESCRIPTION_HEADER  *Table;

  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &SetupData
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Locate table with matching ID
  ///
  Handle = 0;
  Table  = NULL;
  Status = LocateAcpiTableBySignature (
             EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
             (EFI_ACPI_DESCRIPTION_HEADER **) &Table,
             &Handle
             );
  if (EFI_ERROR (Status) || (Table == NULL)) {
    return Status;
  }

  SetupData.AcpiTableRevision = Table->Revision;
  Status = gRT->SetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  VariableAttributes,
                  VariableSize,
                  &SetupData
                  );
  ASSERT_EFI_ERROR(Status);
  DEBUG ((DEBUG_INFO, "Acpi table revision: 0x%x, saved successfully.\n", Table->Revision));

  return Status;
}
#endif  //AMI_OVERRIDE_END - For AMI Chipset Setup

/**
  ACPI Platform driver installation function.

  @param[in] ImageHandle     Handle for this drivers loaded image protocol.
  @param[in] SystemTable     EFI system table.

  @retval EFI_SUCCESS        The driver installed without error.
  @retval EFI_ABORTED        The driver encountered an error and could not complete installation of
                             the ACPI tables.

**/
EFI_STATUS
EFIAPI
InstallAcpiPlatform (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                    Status;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINTN                         TableHandle;
  UINTN                         VarDataSize;
  EFI_HANDLE                    Handle;
  UINT8                         PortData;
  //UINT8                         BatteryNum;
  EFI_CPUID_REGISTER            CpuidRegs;
  EFI_MP_SERVICES_PROTOCOL      *MpService;
  UINTN                         NumberOfCPUs;
  UINTN                         NumberOfEnabledCPUs;
  UINTN                         Index;
  UINT8                         CtdpLevels; // AdvancedFeaturesContent
  UINT8                         LpmSupport; // AdvancedFeaturesContent
  PCH_SERIES                    PchSeries;
  EFI_EVENT                     EndOfDxeEvent;
#ifndef MINTREE_FLAG
// AMI_OVERRIDE_START - For AMI Chipset Setup
#if 0
  SA_SETUP                      SaSetup;
#else
  NB_SETUP_DATA                 SaSetup;
#endif
// AMI_OVERRIDE_END - For AMI Chipset Setup
  CPU_SETUP                     CpuSetup;
  UINT64                        AcpiGuidPart1;
  UINT64                        AcpiGuidPart2;
  CHAR16                        LocalGuidString[GUID_CHARS_NUMBER];
  BOOLEAN                       LanControllerAvailable;
  UINTN                         GbePciBase;
  BOOLEAN                       TbtHostRouterRtd3Capability;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  UINTN                         TbtRpPrimaryBusOffsetAddress;
  UINT32                        OriginalTbtRpPrimaryBus;
  UINT16                        HrDeviceId;
  BOOLEAN                       SkyCamSensorsAvailable;
  UINTN                         Cio2PciBase;
#endif // MINTREE_FLAG


  UINT16                        ModuleName[SKYCAM_MODULE_NAME_LENGTH]; // AdvancedFeaturesContent
  ME_BIOS_PAYLOAD_HOB           *MbpHob = NULL; // AdvancedFeaturesContent
  GPIO_INIT_CONFIG              *UcmcGpioTable = NULL;
  UINT16                        UcmcGpioTableSize = 0;
  EFI_PEI_HOB_POINTERS          HobList;


  Instance      = 0;
  CurrentTable  = NULL;
  TableHandle   = 0;
  Handle        = NULL;

  AsmCpuid (
    1,
    &CpuidRegs.RegEax,
    &CpuidRegs.RegEbx,
    &CpuidRegs.RegEcx,
    &CpuidRegs.RegEdx
    );

// AMI_OVERRIDE_START - No use currently
#if 0
  PublishOverClockingAcpiTable (); // AdvancedFeaturesContent
#endif
// AMI_OVERRIDE_END - No use currently

#if 0  // AMI_OVERRIDE_START - for AMI Chipset Setup   
  //
  // Locate the setup configuration data
  // We assert if it is not found because we have a dependency on PlatformCpu,
  // published by the platform setup driver.
  // This should ensure that we only get called after the setup has updated the
  // variable.
  //
  VarDataSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VarDataSize,
                  &mSystemConfiguration
                  );

//#ifndef MINTREE_FLAG  // AMI_OVERRIDE
  VarDataSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  NULL,
                  &VarDataSize,
                  &SaSetup
                  );
#endif  // AMI_OVERRIDE_END - for AMI Chipset Setup

  VarDataSize = sizeof (ME_SETUP);
  Status = gRT->GetVariable (
                  L"MeSetup",
                  &gMeSetupVariableGuid,
                  NULL,
                  &VarDataSize,
                  &mMeSetup
                  );

  VarDataSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VarDataSize,
                  &CpuSetup
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
// AMI_OVERRIDE_START >>> for AMI Chipset Setup
#if 0
  VarDataSize = sizeof (PCH_SETUP);
  Status = gRT->GetVariable (
                  L"PchSetup",
                  &gPchSetupVariableGuid,
                  NULL,
                  &VarDataSize,
                  &mPchSetup
                  );
#else 
  GetAcpiSetupData (gRT, &mSystemConfiguration);
  GetNbSetupData (gRT, &SaSetup, FALSE);
  GetSbSetupData (gRT, &mPchSetup, FALSE);
#endif
//#endif // MINTREE_FLAG
// AMI_OVERRIDE_END - for AMI Chipset Setup

  //
  // Locate the MP services protocol
  // Find the MP Protocol. This is an MP platform, so MP protocol must be there.
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpService
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Determine the number of processors
  //
  MpService->GetNumberOfProcessors (
              MpService,
              &NumberOfCPUs,
              &NumberOfEnabledCPUs
              );

  //
  // Allocate and initialize the NVS area for SMM and ASL communication.
  //
  AcpiGnvsInit((VOID **) &mGlobalNvsArea.Area);

  //
  // Update global NVS area for ASL and SMM init code to use
  //
#ifndef MINTREE_FLAG
  //mGlobalNvsArea.Area->EnableThermalOffset        = mSystemConfiguration.EnableThermalOffset;
  mGlobalNvsArea.Area->Ac1TripPoint                 = mSystemConfiguration.Ac1TripPoint;
  mGlobalNvsArea.Area->Ac0TripPoint                 = mSystemConfiguration.Ac0TripPoint;
  mGlobalNvsArea.Area->Ac1FanSpeed                  = mSystemConfiguration.Ac1FanSpeed;
  mGlobalNvsArea.Area->Ac0FanSpeed                  = mSystemConfiguration.Ac0FanSpeed;
  mGlobalNvsArea.Area->PassiveThermalTripPoint      = mSystemConfiguration.PassiveThermalTripPoint;
  mGlobalNvsArea.Area->PassiveTc1Value              = mSystemConfiguration.PassiveTc1Value;
  mGlobalNvsArea.Area->PassiveTc2Value              = mSystemConfiguration.PassiveTc2Value;
  mGlobalNvsArea.Area->PassiveTspValue              = mSystemConfiguration.PassiveTspValue;
  mGlobalNvsArea.Area->CriticalThermalTripPoint     = mSystemConfiguration.CriticalThermalTripPoint;
  mGlobalNvsArea.Area->EmaEnable                    = 0; //mSystemConfiguration.EmaEnable; //todo
#endif
  mGlobalNvsArea.Area->ThreadCount                  = (UINT8)NumberOfEnabledCPUs;
  mGlobalNvsArea.Area->MefEnable                    = 0; //mSystemConfiguration.MefEnable; //todo // AdvancedFeaturesContent

  //
  // DPTF Devices and trip points
  //
  mGlobalNvsArea.Area->EnableDptf                   = mSystemConfiguration.EnableDptf;
#ifndef MINTREE_FLAG
  if (mSystemConfiguration.EnableDptf == 1) {

    mGlobalNvsArea.Area->EnableDCFG                   = mSystemConfiguration.EnableDCFG;
    mGlobalNvsArea.Area->EnableSaDevice               = mSystemConfiguration.EnableSaDevice;
    mGlobalNvsArea.Area->ActiveThermalTripPointSA     = mSystemConfiguration.ActiveThermalTripPointSA;
    mGlobalNvsArea.Area->PassiveThermalTripPointSA    = mSystemConfiguration.PassiveThermalTripPointSA;
    mGlobalNvsArea.Area->CriticalThermalTripPointSA   = mSystemConfiguration.CriticalThermalTripPointSA;
    mGlobalNvsArea.Area->CriticalThermalTripPointSaS3 = mSystemConfiguration.CriticalThermalTripPointSaS3;
    mGlobalNvsArea.Area->HotThermalTripPointSA        = mSystemConfiguration.HotThermalTripPointSA;
    mGlobalNvsArea.Area->ThermalSamplingPeriodSA      = mSystemConfiguration.ThermalSamplingPeriodSA;
    mGlobalNvsArea.Area->PpccStepSize                 = mSystemConfiguration.PpccStepSize;
    mGlobalNvsArea.Area->MinPowerLimit0               = mSystemConfiguration.MinPowerLimit0;
    mGlobalNvsArea.Area->MinPowerLimit1               = mSystemConfiguration.MinPowerLimit1;
    mGlobalNvsArea.Area->MinPowerLimit2               = mSystemConfiguration.MinPowerLimit2;

    //
    // DPTF Policies
    //
    CtdpLevels = (UINT8) (RShiftU64(AsmReadMsr64(MSR_PLATFORM_INFO),33) & 0x03);
    if ((CtdpLevels == 1) || (CtdpLevels == 2)) {
      mGlobalNvsArea.Area->EnableCtdpPolicy             = mSystemConfiguration.EnableCtdpPolicy;
    } else {
      mGlobalNvsArea.Area->EnableCtdpPolicy             = 0;
    }

    LpmSupport = (UINT8) (RShiftU64(AsmReadMsr64(MSR_PLATFORM_INFO),32) & 0x01);
    if (LpmSupport == 1) {
      mGlobalNvsArea.Area->EnableLpmPolicy              = mSystemConfiguration.EnableLpmPolicy;
    } else {
      mGlobalNvsArea.Area->EnableLpmPolicy              = 0;
    }
    mGlobalNvsArea.Area->CurrentLowPowerMode            = mSystemConfiguration.CurrentLowPowerMode;
    mGlobalNvsArea.Area->EnableCurrentExecutionUnit     = mSystemConfiguration.EnableCurrentExecutionUnit;
    mGlobalNvsArea.Area->TargetGfxFreq                  = mSystemConfiguration.TargetGfxFreq;

    //
    // DPTF Devices and trip points
    //
    mGlobalNvsArea.Area->EnableFan1Device             = mSystemConfiguration.EnableFan1Device;
    mGlobalNvsArea.Area->EnableFan2Device             = mSystemConfiguration.EnableFan2Device;

    //
    // DPTF Policies
    //
    mGlobalNvsArea.Area->EnableActivePolicy           = mSystemConfiguration.EnableActivePolicy;
    mGlobalNvsArea.Area->EnablePassivePolicy          = mSystemConfiguration.EnablePassivePolicy;
    mGlobalNvsArea.Area->EnableCriticalPolicy         = mSystemConfiguration.EnableCriticalPolicy;
    mGlobalNvsArea.Area->EnableCoolingModePolicy      = mSystemConfiguration.EnableCoolingModePolicy;
    mGlobalNvsArea.Area->TrtRevision                  = mSystemConfiguration.TrtRevision;
    mCpuGlobalNvsArea.Area->EnableHdcPolicy           = mSystemConfiguration.EnableHDCPolicy;
    mGlobalNvsArea.Area->EnableAPPolicy               = mSystemConfiguration.EnableAPPolicy;
    mGlobalNvsArea.Area->EnablePIDPolicy              = mSystemConfiguration.EnablePIDPolicy;
    mGlobalNvsArea.Area->EnablePowerSharePolicy       = mSystemConfiguration.EnablePowerSharePolicy;  // KblGContent

    mGlobalNvsArea.Area->OemDesignVariable0           = mSystemConfiguration.OemDesignVariable0;
    mGlobalNvsArea.Area->OemDesignVariable1           = mSystemConfiguration.OemDesignVariable1;
    mGlobalNvsArea.Area->OemDesignVariable2           = mSystemConfiguration.OemDesignVariable2;
    mGlobalNvsArea.Area->OemDesignVariable3           = mSystemConfiguration.OemDesignVariable3;
    mGlobalNvsArea.Area->OemDesignVariable4           = mSystemConfiguration.OemDesignVariable4;
    mGlobalNvsArea.Area->OemDesignVariable5           = mSystemConfiguration.OemDesignVariable5;

    //
    // CLPO (Current Logical Processor Off lining Setting)
    //
    mGlobalNvsArea.Area->LPOEnable                    = mSystemConfiguration.LPOEnable;
    mGlobalNvsArea.Area->LPOStartPState               = mSystemConfiguration.LPOStartPState;
    mGlobalNvsArea.Area->LPOStepSize                  = mSystemConfiguration.LPOStepSize;
    mGlobalNvsArea.Area->LPOPowerControlSetting       = mSystemConfiguration.LPOPowerControlSetting;
    mGlobalNvsArea.Area->LPOPerformanceControlSetting = mSystemConfiguration.LPOPerformanceControlSetting;

    mGlobalNvsArea.Area->EnableDisplayParticipant     = mSystemConfiguration.EnableDisplayParticipant;
    mGlobalNvsArea.Area->DisplayDepthLowerLimit       = mSystemConfiguration.DisplayDepthLowerLimit;
    mGlobalNvsArea.Area->DisplayDepthUpperLimit       = mSystemConfiguration.DisplayDepthUpperLimit;

    mGlobalNvsArea.Area->EnableChargerParticipant     = mSystemConfiguration.EnableChargerParticipant;
    mGlobalNvsArea.Area->EnablePowerParticipant       = mSystemConfiguration.EnablePowerParticipant;
    mGlobalNvsArea.Area->PowerParticipantPollingRate  = mSystemConfiguration.PowerParticipantPollingRate;
    mGlobalNvsArea.Area->EnablePowerBossPolicy        = mSystemConfiguration.EnablePowerBossPolicy;

    mGlobalNvsArea.Area->EnablePerCParticipant          = mSystemConfiguration.EnablePerCParticipant;
    mGlobalNvsArea.Area->ActiveThermalTripPointPerC     = mSystemConfiguration.ActiveThermalTripPointPerC;
    mGlobalNvsArea.Area->PassiveThermalTripPointPerC    = mSystemConfiguration.PassiveThermalTripPointPerC;
    mGlobalNvsArea.Area->CriticalThermalTripPointPerC   = mSystemConfiguration.CriticalThermalTripPointPerC;
    mGlobalNvsArea.Area->CriticalThermalTripPointPerCS3 = mSystemConfiguration.CriticalThermalTripPointPerCS3;
    mGlobalNvsArea.Area->HotThermalTripPointPerC        = mSystemConfiguration.HotThermalTripPointPerC;

    mGlobalNvsArea.Area->EnableIvCamParticipant          = mSystemConfiguration.EnableIvCamParticipant;
    mGlobalNvsArea.Area->ActiveThermalTripPointIvCam     = mSystemConfiguration.ActiveThermalTripPointIvCam;
    mGlobalNvsArea.Area->PassiveThermalTripPointIvCam    = mSystemConfiguration.PassiveThermalTripPointIvCam;
    mGlobalNvsArea.Area->CriticalThermalTripPointIvCam   = mSystemConfiguration.CriticalThermalTripPointIvCam;
    mGlobalNvsArea.Area->CriticalThermalTripPointIvCamS3 = mSystemConfiguration.CriticalThermalTripPointIvCamS3;
    mGlobalNvsArea.Area->HotThermalTripPointIvCam        = mSystemConfiguration.HotThermalTripPointIvCam;

    mGlobalNvsArea.Area->EnableVSPolicy               = mSystemConfiguration.EnableVSPolicy;

    mGlobalNvsArea.Area->EnableVS1Participant          = mSystemConfiguration.EnableVS1Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointVS1     = mSystemConfiguration.ActiveThermalTripPointVS1;
    mGlobalNvsArea.Area->PassiveThermalTripPointVS1    = mSystemConfiguration.PassiveThermalTripPointVS1;
    mGlobalNvsArea.Area->CriticalThermalTripPointVS1   = mSystemConfiguration.CriticalThermalTripPointVS1;
    mGlobalNvsArea.Area->CriticalThermalTripPointVS1S3 = mSystemConfiguration.CriticalThermalTripPointVS1S3;
    mGlobalNvsArea.Area->HotThermalTripPointVS1        = mSystemConfiguration.HotThermalTripPointVS1;

    mGlobalNvsArea.Area->EnableVS2Participant          = mSystemConfiguration.EnableVS2Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointVS2     = mSystemConfiguration.ActiveThermalTripPointVS2;
    mGlobalNvsArea.Area->PassiveThermalTripPointVS2    = mSystemConfiguration.PassiveThermalTripPointVS2;
    mGlobalNvsArea.Area->CriticalThermalTripPointVS2   = mSystemConfiguration.CriticalThermalTripPointVS2;
    mGlobalNvsArea.Area->CriticalThermalTripPointVS2S3 = mSystemConfiguration.CriticalThermalTripPointVS2S3;
    mGlobalNvsArea.Area->HotThermalTripPointVS2        = mSystemConfiguration.HotThermalTripPointVS2;

    mGlobalNvsArea.Area->EnableStorageParticipant       = mSystemConfiguration.EnableStorageParticipant;
    mGlobalNvsArea.Area->ActiveThermalTripPointStrg     = mSystemConfiguration.ActiveThermalTripPointStrg;
    mGlobalNvsArea.Area->PassiveThermalTripPointStrg    = mSystemConfiguration.PassiveThermalTripPointStrg;
    mGlobalNvsArea.Area->CriticalThermalTripPointStrg   = mSystemConfiguration.CriticalThermalTripPointStrg;
    mGlobalNvsArea.Area->CriticalThermalTripPointStrgS3 = mSystemConfiguration.CriticalThermalTripPointStrgS3;
    mGlobalNvsArea.Area->HotThermalTripPointStrg        = mSystemConfiguration.HotThermalTripPointStrg;


    mGlobalNvsArea.Area->EnableWWANParticipant        = mSystemConfiguration.EnableWWANParticipant;
    mGlobalNvsArea.Area->ActiveThermalTripPointWWAN   = mSystemConfiguration.ActiveThermalTripPointWWAN;
    mGlobalNvsArea.Area->PassiveThermalTripPointWWAN  = mSystemConfiguration.PassiveThermalTripPointWWAN;
    mGlobalNvsArea.Area->CriticalThermalTripPointWWAN = mSystemConfiguration.CriticalThermalTripPointWWAN;
    mGlobalNvsArea.Area->CriticalThermalTripPointWwanS3 = mSystemConfiguration.CriticalThermalTripPointWwanS3;
    mGlobalNvsArea.Area->HotThermalTripPointWWAN      = mSystemConfiguration.HotThermalTripPointWWAN;

    mGlobalNvsArea.Area->EnableWifiParticipant        = mSystemConfiguration.EnableWifiParticipant;
    mGlobalNvsArea.Area->ActiveThermalTripPointWifi   = mSystemConfiguration.ActiveThermalTripPointWifi;
    mGlobalNvsArea.Area->PassiveThermalTripPointWifi  = mSystemConfiguration.PassiveThermalTripPointWifi;
    mGlobalNvsArea.Area->CriticalThermalTripPointWifi = mSystemConfiguration.CriticalThermalTripPointWifi;
    mGlobalNvsArea.Area->CriticalThermalTripPointWifiS3 = mSystemConfiguration.CriticalThermalTripPointWifiS3;
    mGlobalNvsArea.Area->HotThermalTripPointWifi      = mSystemConfiguration.HotThermalTripPointWifi;
    mGlobalNvsArea.Area->ThermalSamplingPeriodWifi    = mSystemConfiguration.ThermalSamplingPeriodWifi;

    mGlobalNvsArea.Area->EnableSen1Participant        = mSystemConfiguration.EnableSen1Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointSen1   = mSystemConfiguration.ActiveThermalTripPointSen1;
    mGlobalNvsArea.Area->PassiveThermalTripPointSen1  = mSystemConfiguration.PassiveThermalTripPointSen1;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen1 = mSystemConfiguration.CriticalThermalTripPointSen1;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen1S3 = mSystemConfiguration.CriticalThermalTripPointSen1S3;
    mGlobalNvsArea.Area->HotThermalTripPointSen1      = mSystemConfiguration.HotThermalTripPointSen1;
    mGlobalNvsArea.Area->SensorSamplingPeriodSen1     = mSystemConfiguration.SensorSamplingPeriodSen1;

    mGlobalNvsArea.Area->EnableSen2Participant        = mSystemConfiguration.EnableSen2Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointSen2   = mSystemConfiguration.ActiveThermalTripPointSen2;
    mGlobalNvsArea.Area->PassiveThermalTripPointSen2  = mSystemConfiguration.PassiveThermalTripPointSen2;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen2 = mSystemConfiguration.CriticalThermalTripPointSen2;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen2S3 = mSystemConfiguration.CriticalThermalTripPointSen2S3;
    mGlobalNvsArea.Area->HotThermalTripPointSen2      = mSystemConfiguration.HotThermalTripPointSen2;
    mGlobalNvsArea.Area->SensorSamplingPeriodSen2     = mSystemConfiguration.SensorSamplingPeriodSen2;

    mGlobalNvsArea.Area->EnableSen3Participant        = mSystemConfiguration.EnableSen3Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointSen3   = mSystemConfiguration.ActiveThermalTripPointSen3;
    mGlobalNvsArea.Area->PassiveThermalTripPointSen3  = mSystemConfiguration.PassiveThermalTripPointSen3;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen3 = mSystemConfiguration.CriticalThermalTripPointSen3;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen3S3 = mSystemConfiguration.CriticalThermalTripPointSen3S3;
    mGlobalNvsArea.Area->HotThermalTripPointSen3      = mSystemConfiguration.HotThermalTripPointSen3;
    mGlobalNvsArea.Area->SensorSamplingPeriodSen3     = mSystemConfiguration.SensorSamplingPeriodSen3;

    mGlobalNvsArea.Area->EnableSen4Participant        = mSystemConfiguration.EnableSen4Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointSen4   = mSystemConfiguration.ActiveThermalTripPointSen4;
    mGlobalNvsArea.Area->PassiveThermalTripPointSen4  = mSystemConfiguration.PassiveThermalTripPointSen4;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen4 = mSystemConfiguration.CriticalThermalTripPointSen4;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen4S3 = mSystemConfiguration.CriticalThermalTripPointSen4S3;
    mGlobalNvsArea.Area->HotThermalTripPointSen4      = mSystemConfiguration.HotThermalTripPointSen4;
    mGlobalNvsArea.Area->SensorSamplingPeriodSen4     = mSystemConfiguration.SensorSamplingPeriodSen4;

    mGlobalNvsArea.Area->EnableSen5Participant        = mSystemConfiguration.EnableSen5Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointSen5   = mSystemConfiguration.ActiveThermalTripPointSen5;
    mGlobalNvsArea.Area->PassiveThermalTripPointSen5  = mSystemConfiguration.PassiveThermalTripPointSen5;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen5 = mSystemConfiguration.CriticalThermalTripPointSen5;
    mGlobalNvsArea.Area->CriticalThermalTripPointSen5S3 = mSystemConfiguration.CriticalThermalTripPointSen5S3;
    mGlobalNvsArea.Area->HotThermalTripPointSen5      = mSystemConfiguration.HotThermalTripPointSen5;
    mGlobalNvsArea.Area->SensorSamplingPeriodSen5     = mSystemConfiguration.SensorSamplingPeriodSen5;

    mGlobalNvsArea.Area->EnableGen1Participant        = mSystemConfiguration.EnableGen1Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointGen1   = mSystemConfiguration.ActiveThermalTripPointGen1;
    mGlobalNvsArea.Area->PassiveThermalTripPointGen1  = mSystemConfiguration.PassiveThermalTripPointGen1;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen1 = mSystemConfiguration.CriticalThermalTripPointGen1;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen1S3 = mSystemConfiguration.CriticalThermalTripPointGen1S3;
    mGlobalNvsArea.Area->HotThermalTripPointGen1      = mSystemConfiguration.HotThermalTripPointGen1;
    mGlobalNvsArea.Area->ThermistorSamplingPeriodGen1 = mSystemConfiguration.ThermistorSamplingPeriodGen1;

    mGlobalNvsArea.Area->EnableGen2Participant        = mSystemConfiguration.EnableGen2Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointGen2   = mSystemConfiguration.ActiveThermalTripPointGen2;
    mGlobalNvsArea.Area->PassiveThermalTripPointGen2  = mSystemConfiguration.PassiveThermalTripPointGen2;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen2 = mSystemConfiguration.CriticalThermalTripPointGen2;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen2S3 = mSystemConfiguration.CriticalThermalTripPointGen2S3;
    mGlobalNvsArea.Area->HotThermalTripPointGen2      = mSystemConfiguration.HotThermalTripPointGen2;
    mGlobalNvsArea.Area->ThermistorSamplingPeriodGen2 = mSystemConfiguration.ThermistorSamplingPeriodGen2;

    mGlobalNvsArea.Area->EnableGen3Participant        = mSystemConfiguration.EnableGen3Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointGen3   = mSystemConfiguration.ActiveThermalTripPointGen3;
    mGlobalNvsArea.Area->PassiveThermalTripPointGen3  = mSystemConfiguration.PassiveThermalTripPointGen3;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen3 = mSystemConfiguration.CriticalThermalTripPointGen3;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen3S3 = mSystemConfiguration.CriticalThermalTripPointGen3S3;
    mGlobalNvsArea.Area->HotThermalTripPointGen3      = mSystemConfiguration.HotThermalTripPointGen3;
    mGlobalNvsArea.Area->ThermistorSamplingPeriodGen3 = mSystemConfiguration.ThermistorSamplingPeriodGen3;

    mGlobalNvsArea.Area->EnableGen4Participant        = mSystemConfiguration.EnableGen4Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointGen4   = mSystemConfiguration.ActiveThermalTripPointGen4;
    mGlobalNvsArea.Area->PassiveThermalTripPointGen4  = mSystemConfiguration.PassiveThermalTripPointGen4;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen4 = mSystemConfiguration.CriticalThermalTripPointGen4;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen4S3 = mSystemConfiguration.CriticalThermalTripPointGen4S3;
    mGlobalNvsArea.Area->HotThermalTripPointGen4      = mSystemConfiguration.HotThermalTripPointGen4;
    mGlobalNvsArea.Area->ThermistorSamplingPeriodGen4 = mSystemConfiguration.ThermistorSamplingPeriodGen4;

    mGlobalNvsArea.Area->EnableGen5Participant        = mSystemConfiguration.EnableGen5Participant;
    mGlobalNvsArea.Area->ActiveThermalTripPointGen5   = mSystemConfiguration.ActiveThermalTripPointGen5;
    mGlobalNvsArea.Area->PassiveThermalTripPointGen5  = mSystemConfiguration.PassiveThermalTripPointGen5;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen5 = mSystemConfiguration.CriticalThermalTripPointGen5;
    mGlobalNvsArea.Area->CriticalThermalTripPointGen5S3 = mSystemConfiguration.CriticalThermalTripPointGen5S3;
    mGlobalNvsArea.Area->HotThermalTripPointGen5      = mSystemConfiguration.HotThermalTripPointGen5;
    mGlobalNvsArea.Area->ThermistorSamplingPeriodGen5 = mSystemConfiguration.ThermistorSamplingPeriodGen5;

//
// KblGBegin
//
    // Discrete Graphics Power Share Particpants
    mGlobalNvsArea.Area->EnableDGFCParticipant             = mSystemConfiguration.EnableDGFCParticipant;
    mGlobalNvsArea.Area->ActiveThermalTripPointDGFC     = mSystemConfiguration.ActiveThermalTripPointDGFC;
    mGlobalNvsArea.Area->PassiveThermalTripPointDGFC    = mSystemConfiguration.PassiveThermalTripPointDGFC;
    mGlobalNvsArea.Area->CriticalThermalTripPointDGFC   = mSystemConfiguration.CriticalThermalTripPointDGFC;
    mGlobalNvsArea.Area->CriticalThermalTripPointDGFCS3 = mSystemConfiguration.CriticalThermalTripPointDGFCS3;
    mGlobalNvsArea.Area->HotThermalTripPointDGFC        = mSystemConfiguration.HotThermalTripPointDGFC;
    mGlobalNvsArea.Area->SensorSamplingPeriodDGFC       = mSystemConfiguration.SensorSamplingPeriodDGFC;
    mGlobalNvsArea.Area->PpccStepSizeDGFC               = mSystemConfiguration.PpccStepSizeDGFC;
    mGlobalNvsArea.Area->MinPowerLimit0DGFC             = mSystemConfiguration.MinPowerLimit0DGFC;

    mGlobalNvsArea.Area->EnableDGHMParticipant             = mSystemConfiguration.EnableDGHMParticipant;
    mGlobalNvsArea.Area->ActiveThermalTripPointDGHM      = mSystemConfiguration.ActiveThermalTripPointDGHM;
    mGlobalNvsArea.Area->PassiveThermalTripPointDGHM     = mSystemConfiguration.PassiveThermalTripPointDGHM;
    mGlobalNvsArea.Area->CriticalThermalTripPointDGHM    = mSystemConfiguration.CriticalThermalTripPointDGHM;
    mGlobalNvsArea.Area->CriticalThermalTripPointDGHMS3  = mSystemConfiguration.CriticalThermalTripPointDGHMS3;
    mGlobalNvsArea.Area->HotThermalTripPointDGHM         = mSystemConfiguration.HotThermalTripPointDGHM;
    mGlobalNvsArea.Area->SensorSamplingPeriodDGHM        = mSystemConfiguration.SensorSamplingPeriodDGHM;

    mGlobalNvsArea.Area->EnableMCPParticipant             = mSystemConfiguration.EnableMCPParticipant;
    mGlobalNvsArea.Area->PpccStepSizeMCP                  = mSystemConfiguration.PpccStepSizeMCP;
    mGlobalNvsArea.Area->MinPowerLimit0MCP                = mSystemConfiguration.MinPowerLimit0MCP;
//
// KblGEnd
//
  }

  mGlobalNvsArea.Area->HebcValue               = mSystemConfiguration.HebcValue;

  //
  // BIOS only version of CTDP. (CTDP without using DPTF)
  //
  if (CpuSetup.ConfigTdpLock == 1 || mSystemConfiguration.EnableDptf == 1) {
    mGlobalNvsArea.Area->ConfigTdpBios      = 0;
  } else {
    mGlobalNvsArea.Area->ConfigTdpBios      = CpuSetup.ConfigTdpBios;
  }

  //
  // PECI Access Method
  //
  mGlobalNvsArea.Area->PeciAccessMethod           = mSystemConfiguration.PeciAccessMethod;
#endif
  //
  // Wireless support
  //
  mGlobalNvsArea.Area->PowerSharingManagerEnable  = mSystemConfiguration.PowerSharingManagerEnable;
#ifndef MINTREE_FLAG
  mGlobalNvsArea.Area->PsmSplcDomainType1         = mSystemConfiguration.PsmSplcDomainType1;
  mGlobalNvsArea.Area->PsmSplcPowerLimit1         = mSystemConfiguration.PsmSplcPowerLimit1;
  mGlobalNvsArea.Area->PsmSplcTimeWindow1         = mSystemConfiguration.PsmSplcTimeWindow1;
  mGlobalNvsArea.Area->PsmSplcDomainType2         = mSystemConfiguration.PsmSplcDomainType2;
  mGlobalNvsArea.Area->PsmSplcPowerLimit2         = mSystemConfiguration.PsmSplcPowerLimit2;
  mGlobalNvsArea.Area->PsmSplcTimeWindow2         = mSystemConfiguration.PsmSplcTimeWindow2;

  mGlobalNvsArea.Area->PsmDplcDomainType1         = mSystemConfiguration.PsmDplcDomainType1;
  mGlobalNvsArea.Area->PsmDplcDomainPreference1   = mSystemConfiguration.PsmDplcDomainPreference1;
  mGlobalNvsArea.Area->PsmDplcPowerLimitIndex1    = mSystemConfiguration.PsmDplcPowerLimitIndex1;
  mGlobalNvsArea.Area->PsmDplcDefaultPowerLimit1  = mSystemConfiguration.PsmDplcDefaultPowerLimit1;
  mGlobalNvsArea.Area->PsmDplcDefaultTimeWindow1  = mSystemConfiguration.PsmDplcDefaultTimeWindow1;
  mGlobalNvsArea.Area->PsmDplcMinimumPowerLimit1  = mSystemConfiguration.PsmDplcMinimumPowerLimit1;
  mGlobalNvsArea.Area->PsmDplcMaximumPowerLimit1  = mSystemConfiguration.PsmDplcMaximumPowerLimit1;
  mGlobalNvsArea.Area->PsmDplcMaximumTimeWindow1  = mSystemConfiguration.PsmDplcMaximumTimeWindow1;

  mGlobalNvsArea.Area->PsmDplcDomainType2         = mSystemConfiguration.PsmDplcDomainType2;
  mGlobalNvsArea.Area->PsmDplcDomainPreference2   = mSystemConfiguration.PsmDplcDomainPreference2;
  mGlobalNvsArea.Area->PsmDplcPowerLimitIndex2    = mSystemConfiguration.PsmDplcPowerLimitIndex2;
  mGlobalNvsArea.Area->PsmDplcDefaultPowerLimit2  = mSystemConfiguration.PsmDplcDefaultPowerLimit2;
  mGlobalNvsArea.Area->PsmDplcDefaultTimeWindow2  = mSystemConfiguration.PsmDplcDefaultTimeWindow2;
  mGlobalNvsArea.Area->PsmDplcMinimumPowerLimit2  = mSystemConfiguration.PsmDplcMinimumPowerLimit2;
  mGlobalNvsArea.Area->PsmDplcMaximumPowerLimit2  = mSystemConfiguration.PsmDplcMaximumPowerLimit2;
  mGlobalNvsArea.Area->PsmDplcMaximumTimeWindow2  = mSystemConfiguration.PsmDplcMaximumTimeWindow2;
#endif

  mGlobalNvsArea.Area->WifiEnable                = mSystemConfiguration.WifiEnable;
#ifndef MINTREE_FLAG
  if (mSystemConfiguration.WifiEnable == 1) {
    mGlobalNvsArea.Area->WifiDomainType1         = mSystemConfiguration.WifiDomainType1;
    mGlobalNvsArea.Area->WifiPowerLimit1         = mSystemConfiguration.WifiPowerLimit1;
    mGlobalNvsArea.Area->WifiTimeWindow1         = mSystemConfiguration.WifiTimeWindow1;
    mGlobalNvsArea.Area->WifiDomainType2         = mSystemConfiguration.WifiDomainType2;
    mGlobalNvsArea.Area->WifiPowerLimit2         = mSystemConfiguration.WifiPowerLimit2;
    mGlobalNvsArea.Area->WifiTimeWindow2         = mSystemConfiguration.WifiTimeWindow2;
    mGlobalNvsArea.Area->WifiDomainType3         = mSystemConfiguration.WifiDomainType3;
    mGlobalNvsArea.Area->WifiPowerLimit3         = mSystemConfiguration.WifiPowerLimit3;
    mGlobalNvsArea.Area->WifiTimeWindow3         = mSystemConfiguration.WifiTimeWindow3;
    mGlobalNvsArea.Area->TRxDelay0               = mSystemConfiguration.TRxDelay0;
    mGlobalNvsArea.Area->TRxCableLength0         = mSystemConfiguration.TRxCableLength0;
    mGlobalNvsArea.Area->TRxDelay1               = mSystemConfiguration.TRxDelay1;
    mGlobalNvsArea.Area->TRxCableLength1         = mSystemConfiguration.TRxCableLength1;
    mGlobalNvsArea.Area->WrddDomainType1         = mSystemConfiguration.WrddDomainType1;
    mGlobalNvsArea.Area->WrddCountryIndentifier1 = mSystemConfiguration.WrddCountryIndentifier1;
    mGlobalNvsArea.Area->WrddDomainType2         = mSystemConfiguration.WrddDomainType2;
    mGlobalNvsArea.Area->WrddCountryIndentifier2 = mSystemConfiguration.WrddCountryIndentifier2;
    mGlobalNvsArea.Area->WrdsWiFiSarEnable       = mSystemConfiguration.WrdsWiFiSarEnable;
    if (mGlobalNvsArea.Area->WrdsWiFiSarEnable == 1) {
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit1  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit1;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit2  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit2;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit3  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit3;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit4  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit4;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit5  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit5;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit6  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit6;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit7  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit7;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit8  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit8;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit9  = mSystemConfiguration.WrdsWiFiSarTxPowerLimit9;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit10 = mSystemConfiguration.WrdsWiFiSarTxPowerLimit10;
    } else {
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit1  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit2  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit3  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit4  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit5  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit6  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit7  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit8  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit9  = 0x0;
      mGlobalNvsArea.Area->WrdsWiFiSarTxPowerLimit10 = 0x0;
    }

    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup1PowerMax1    = mSystemConfiguration.WgdsWiFiSarDeltaGroup1PowerMax1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup1PowerChainA1 = mSystemConfiguration.WgdsWiFiSarDeltaGroup1PowerChainA1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup1PowerChainB1 = mSystemConfiguration.WgdsWiFiSarDeltaGroup1PowerChainB1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup1PowerMax2    = mSystemConfiguration.WgdsWiFiSarDeltaGroup1PowerMax2;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup1PowerChainA2 = mSystemConfiguration.WgdsWiFiSarDeltaGroup1PowerChainA2;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup1PowerChainB2 = mSystemConfiguration.WgdsWiFiSarDeltaGroup1PowerChainB2;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup2PowerMax1    = mSystemConfiguration.WgdsWiFiSarDeltaGroup2PowerMax1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup2PowerChainA1 = mSystemConfiguration.WgdsWiFiSarDeltaGroup2PowerChainA1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup2PowerChainB1 = mSystemConfiguration.WgdsWiFiSarDeltaGroup2PowerChainB1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup2PowerMax2    = mSystemConfiguration.WgdsWiFiSarDeltaGroup2PowerMax2;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup2PowerChainA2 = mSystemConfiguration.WgdsWiFiSarDeltaGroup2PowerChainA2;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup2PowerChainB2 = mSystemConfiguration.WgdsWiFiSarDeltaGroup2PowerChainB2;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup3PowerMax1    = mSystemConfiguration.WgdsWiFiSarDeltaGroup3PowerMax1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup3PowerChainA1 = mSystemConfiguration.WgdsWiFiSarDeltaGroup3PowerChainA1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup3PowerChainB1 = mSystemConfiguration.WgdsWiFiSarDeltaGroup3PowerChainB1;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup3PowerMax2    = mSystemConfiguration.WgdsWiFiSarDeltaGroup3PowerMax2;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup3PowerChainA2 = mSystemConfiguration.WgdsWiFiSarDeltaGroup3PowerChainA2;
    mGlobalNvsArea.Area->WgdsWiFiSarDeltaGroup3PowerChainB2 = mSystemConfiguration.WgdsWiFiSarDeltaGroup3PowerChainB2;

    mGlobalNvsArea.Area->WigigRfe = mSystemConfiguration.WigigRfe;
    if (mGlobalNvsArea.Area->WigigRfe == 1) {
      mGlobalNvsArea.Area->WiGigRfeCh1 = mSystemConfiguration.WiGigRfeCh1;
      mGlobalNvsArea.Area->WiGigRfeCh2 = mSystemConfiguration.WiGigRfeCh2;
      mGlobalNvsArea.Area->WiGigRfeCh3 = mSystemConfiguration.WiGigRfeCh3;
      mGlobalNvsArea.Area->WiGigRfeCh4 = mSystemConfiguration.WiGigRfeCh4;
    } else {
      mGlobalNvsArea.Area->WiGigRfeCh1 = 0x0;
      mGlobalNvsArea.Area->WiGigRfeCh2 = 0x0;
      mGlobalNvsArea.Area->WiGigRfeCh3 = 0x0;
      mGlobalNvsArea.Area->WiGigRfeCh4 = 0x0;
    }
    mGlobalNvsArea.Area->AwvClassIndex      = mSystemConfiguration.AwvClassIndex;
    mGlobalNvsArea.Area->WakeOnWiGigSupport = mSystemConfiguration.WakeOnWiGigSupport;

    mGlobalNvsArea.Area->BluetoothSar = mSystemConfiguration.BluetoothSar;
    if (mGlobalNvsArea.Area->BluetoothSar == 1) {
      mGlobalNvsArea.Area->BluetoothSarBr     = mSystemConfiguration.BluetoothSarBr;
      mGlobalNvsArea.Area->BluetoothSarEdr2   = mSystemConfiguration.BluetoothSarEdr2;
      mGlobalNvsArea.Area->BluetoothSarEdr3   = mSystemConfiguration.BluetoothSarEdr3;
      mGlobalNvsArea.Area->BluetoothSarLe     = mSystemConfiguration.BluetoothSarLe;
      mGlobalNvsArea.Area->BluetoothSarLe2Mhz = mSystemConfiguration.BluetoothSarLe2Mhz;
      mGlobalNvsArea.Area->BluetoothSarLeLr   = mSystemConfiguration.BluetoothSarLeLr;
    } else {
      mGlobalNvsArea.Area->BluetoothSarBr     = 0x0;
      mGlobalNvsArea.Area->BluetoothSarEdr2   = 0x0;
      mGlobalNvsArea.Area->BluetoothSarEdr3   = 0x0;
      mGlobalNvsArea.Area->BluetoothSarLe     = 0x0;
    }
  }

  mGlobalNvsArea.Area->SystemTimeAndAlarmSource = mSystemConfiguration.SystemTimeAndAlarmSource;
  mGlobalNvsArea.Area->WirelessCharging = mSystemConfiguration.WirelessCharging;
  mGlobalNvsArea.Area->XdciFnEnable = mPchSetup.PchXdciSupport;
  mGlobalNvsArea.Area->SerialPortAcpiDebug        = mSystemConfiguration.AcpiDebug && mSystemConfiguration.SerialPortAcpiDebug;
#endif
  //
  // Initilize platform ID in Global NVS Area
  //
  //
  // Get the HOB list. If it is not present, then ASSERT.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to get the HOB list.\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  HobList.Raw = GetNextGuidHob (&gPlatformInfoHobGuid, HobList.Raw);
  if (HobList.Raw != NULL) {
    mPlatformInfo = (PLATFORM_INFO *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
    mGlobalNvsArea.Area->GenerationId   = mPlatformInfo->PlatformGeneration;
    mGlobalNvsArea.Area->PlatformFlavor = mPlatformInfo->PlatformFlavor;
    mGlobalNvsArea.Area->BoardRev       = mPlatformInfo->BoardRev;
    mGlobalNvsArea.Area->BoardType      = mPlatformInfo->BoardType;
  }
#ifndef MINTREE_FLAG
  mGlobalNvsArea.Area->Rtd3Support                = mSystemConfiguration.Rtd3Support;
  mGlobalNvsArea.Area->Rtd3P0dl                   = mSystemConfiguration.Rtd3P0dl;
  mGlobalNvsArea.Area->Rtd3P3dl                   = mSystemConfiguration.Rtd3P3dl;
  mGlobalNvsArea.Area->Rtd3AudioDelay             = mSystemConfiguration.Rtd3AudioDelay;
  mGlobalNvsArea.Area->Rtd3SensorHub              = mSystemConfiguration.Rtd3SensorHub;
  mGlobalNvsArea.Area->Rtd3TouchPanelDelay        = mSystemConfiguration.Rtd3TouchPanelDelay;
  mGlobalNvsArea.Area->Rtd3TouchPadDelay          = mSystemConfiguration.Rtd3TouchPadDelay;
  mGlobalNvsArea.Area->Rtd3I2C0ControllerPS0Delay = mSystemConfiguration.Rtd3I2C0ControllerPS0Delay;
  mGlobalNvsArea.Area->Rtd3I2C1ControllerPS0Delay = mSystemConfiguration.Rtd3I2C1ControllerPS0Delay;
  mGlobalNvsArea.Area->VRStaggeringDelay          = mSystemConfiguration.VRStaggeringDelay;
  mGlobalNvsArea.Area->VRRampUpDelay              = mSystemConfiguration.VRRampUpDelay;
  mGlobalNvsArea.Area->PstateCapping              = mSystemConfiguration.PstateCapping;
  mGlobalNvsArea.Area->UsbPowerResourceTest       = (mSystemConfiguration.Rtd3UsbPt2 << 4)
                                                    | mSystemConfiguration.Rtd3UsbPt1;
  mGlobalNvsArea.Area->Rtd3I2C0SensorHub          = mSystemConfiguration.Rtd3I2C0SensorHub;
  mGlobalNvsArea.Area->SensorStandby              = mSystemConfiguration.SensorStandby;
  mGlobalNvsArea.Area->Rtd3Config0                = ((mSystemConfiguration.WWANEnable && (mSystemConfiguration.Rtd3WWAN != 0) ? 1:0) << 7)
                                                    | (mSystemConfiguration.Rtd3I2CTouchPanel << 4) // Applicable for SKL SDS RTD3 SIP only
                                                    | (mSystemConfiguration.Rtd3PcieNvme << 2)  // Applicable for SKL SDS RTD3
                                                    | (mSystemConfiguration.Rtd3Camera << 1)
                                                    | mSystemConfiguration.Rtd3ZPODD;
  mGlobalNvsArea.Area->Rtd3Config1                = (mSystemConfiguration.Rtd3RaidVolumes << 9)
                                                    | (mSystemConfiguration.Rtd3RemapCR3 << 8)
                                                    | (mSystemConfiguration.Rtd3RemapCR2 << 7)
                                                    | (mSystemConfiguration.Rtd3RemapCR1 << 6)
                                                    // BIT5 reserved for Port5
                                                    | (mSystemConfiguration.Rtd3SataPort4 << 4)
                                                    | (mSystemConfiguration.Rtd3SataPort3 << 3)
                                                    | (mSystemConfiguration.Rtd3SataPort2 << 2)
                                                    | (mSystemConfiguration.Rtd3SataPort1 << 1)
                                                    |  mSystemConfiguration.Rtd3SataPort0;

  mGlobalNvsArea.Area->EnableModernStandby        = mSystemConfiguration.ConsolidatedPR;
  mGlobalNvsArea.Area->Rtd3TbtSupport             = mSystemConfiguration.Rtd3Tbt;         // TBT AR RTD3 Enable.
  mGlobalNvsArea.Area->Rtd3TbtOffDelay            = mSystemConfiguration.Rtd3TbtOffDelay; // TBT AR RTD3 Off delay in ms.
  mGlobalNvsArea.Area->Rtd3TbtClkReq              = mSystemConfiguration.Rtd3TbtClkReq;   // TBT AR RTD3 ClkReq Mask Enable.
  mGlobalNvsArea.Area->Rtd3TbtClkReqDelay         = mSystemConfiguration.Rtd3TbtClkReqDelay; // TBT AR RTD3 ClkReq mask delay in ms.
  mGlobalNvsArea.Area->TbtEnable                  = mSystemConfiguration.TbtSupport;         // TBT Enabled/Disabled
  mGlobalNvsArea.Area->Rtd3WwanDsm                = mSystemConfiguration.Rtd3WwanDsm;

  TbtHostRouterRtd3Capability = FALSE;
  //
  // Locating Thunderbolt(TM) Hob
  //
  if (mSystemConfiguration.TbtSupport == 1) {
    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &gTbtInfoHob);
    if (!EFI_ERROR (Status)) {
      gTbtInfoHob = GetNextGuidHob (&gTbtInfoHobGuid, gTbtInfoHob);
      if (gTbtInfoHob == NULL) {
        DEBUG ((DEBUG_INFO, "TBT HOB not found\n"));
      }
    }
  }

  if (mSystemConfiguration.Rtd3Support && mSystemConfiguration.TbtSupport && mSystemConfiguration.Rtd3Tbt) {
    if (gTbtInfoHob != NULL) {
      Status = GetTbtRpDevFun (gTbtInfoHob->TbtDefaultSelectorInfo - 1, &RpDev, &RpFunc);
      TbtRpPrimaryBusOffsetAddress = MmPciBase ((UINT32) 0x00, (UINT32) RpDev ,(UINT32)  RpFunc) + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET;
      OriginalTbtRpPrimaryBus = MmioRead32 (TbtRpPrimaryBusOffsetAddress);
// AMI_OVERRIDE_START - Avoid this address for OEM used.      
#if 0      
      MmioWrite32(TbtRpPrimaryBusOffsetAddress, 0x00F0F000); // Store Temp Bus
      HrDeviceId  = MmioRead16 (MmPciBase (0xF0, 0x00, 0x00) + PCI_DEVICE_ID_OFFSET);
#else
      MmioWrite32(TbtRpPrimaryBusOffsetAddress, 0x003F3F00); // Store Temp Bus
      HrDeviceId  = MmioRead16 (MmPciBase (0x3F, 0x00, 0x00) + PCI_DEVICE_ID_OFFSET);
#endif
// AMI_OVERRIDE_START - Avoid this address for OEM used.      
      MmioWrite32(TbtRpPrimaryBusOffsetAddress, OriginalTbtRpPrimaryBus); // Restore Original Bus
      TbtHostRouterRtd3Capability |= IsTbtHostRouter(HrDeviceId);
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
      if (mSystemConfiguration.Multicontroller) {
        Status = GetTbtRpDevFun (gTbtInfoHob->TbtDefaultSelectorInfo1 - 1, &RpDev, &RpFunc);
        TbtRpPrimaryBusOffsetAddress = MmPciBase((UINT32)0x00, (UINT32)RpDev, (UINT32)RpFunc) + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET;
        OriginalTbtRpPrimaryBus = MmioRead32(TbtRpPrimaryBusOffsetAddress);
        MmioWrite32(TbtRpPrimaryBusOffsetAddress, 0x00F0F000); // Store Temp Bus
        HrDeviceId = MmioRead16(MmPciBase(0xF0, 0x00, 0x00) + PCI_DEVICE_ID_OFFSET);
        MmioWrite32(TbtRpPrimaryBusOffsetAddress, OriginalTbtRpPrimaryBus); // Restore Original Bus
        TbtHostRouterRtd3Capability |= IsTbtHostRouter(HrDeviceId);
      }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
    }
  }
#endif // MINTREE_FLAG

  //
  // Enable PowerState
  //
  mGlobalNvsArea.Area->PowerState = 1; // AC =1; for mobile platform, will update this value in SmmPlatform.c
/*
  if (mSystemConfiguration.IgdLcdIBia == 0) {
    //
    // Re-change the ALS to disable according to setup.
    //
    mGlobalNvsArea.Area->AlsEnable = 0;
  } else if (mSystemConfiguration.IgdLcdIBia < 6) {
    //
    // Re-change the ALS to enable (2 accepted in ACPI) according to setup.
    //
    mGlobalNvsArea.Area->AlsEnable = 2;
    mGlobalNvsArea.Area->AlsAdjustmentFactor = mSystemConfiguration.IgdLcdIBia * 20;
  } else if (mSystemConfiguration.IgdLcdIBia == 6) {
    //
    // The default value from VBT
    //
    mGlobalNvsArea.Area->AlsAdjustmentFactor = 100;
  }
*/

  //
  // These don't have setup controls yet
  //
  mGlobalNvsArea.Area->NumberOfBatteries        = 0;
  mGlobalNvsArea.Area->BatteryCapacity0         = 100;
  mGlobalNvsArea.Area->BatteryStatus0           = 84;

  mGlobalNvsArea.Area->OnboardCom               = 1;//mSystemConfiguration.OnboardCom; // todo
  mGlobalNvsArea.Area->OnboardComCir            = 0;//mSystemConfiguration.OnboardCir; // todo
  mGlobalNvsArea.Area->IdeMode                  = 0;

  if (mPlatformInfo->EcPresent == TRUE) {
    //
    // GNVS->NumberOfBatteries = bit 0 for battery 1 status and bit 1 for battery 2
    // Bit 7 and 6 are Battery present indicator bits from EC

    //
    // Commenting out this part of battery bit initializer code as it will be initialized in ASL code to save boot time.
    //

    //
    //BatteryNum = 0;
    //Status = GetBatteryNumber (&BatteryNum);
    //if (Status == EFI_SUCCESS) {
    //  BatteryNum &= EC_REAL_BAT_PRESENT_MASK;
    //  BatteryNum = BatteryNum >> 6;
    //  mGlobalNvsArea.Area->NumberOfBatteries = BatteryNum;
    //}
    //

    //
    // Call EC lib to get PCIe dock status
    //
    PortData = 0;
    Status = GetPcieDockStatus (&PortData);

    //
    // the bit0 is PCIe Dock Status, 1 = docked
    //
    mGlobalNvsArea.Area->PcieDockStatus = (PortData & 1);
  }

  mGlobalNvsArea.Area->NativePCIESupport        = mSystemConfiguration.PciExpNative;
  mGlobalNvsArea.Area->PlatformCpuId            = (CpuidRegs.RegEax & 0x0FFFFF);

  //
  // PCIe LTR Configuration
  //
  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    mGlobalNvsArea.Area->LtrEnable[Index]     = mPchSetup.PchPcieLtrEnable[Index];  // AdvancedFeaturesContent
  }

  //
  // Create an End of DXE event.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  AcpiEndOfDxeEvent,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // SIO related option
  //
  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **) &mCpuIo);
  ASSERT_EFI_ERROR (Status);

  mGlobalNvsArea.Area->WPCN381U = GLOBAL_NVS_DEVICE_DISABLE;
  mGlobalNvsArea.Area->SMSC1000 = GLOBAL_NVS_DEVICE_DISABLE;
  //
  // Smsc1007, Smsc1000, SIO Base Address
  //
  mGlobalNvsArea.Area->LpcSioPort1  = 0x164E;
  mGlobalNvsArea.Area->LpcSioPort2  = 0x4E;
  mGlobalNvsArea.Area->LpcSioPmeBar = 0x0680;

  mGlobalNvsArea.Area->DockedSioPresent = GLOBAL_NVS_DEVICE_DISABLE;
  mGlobalNvsArea.Area->DockComA = GLOBAL_NVS_DEVICE_DISABLE;
  mGlobalNvsArea.Area->DockComB = GLOBAL_NVS_DEVICE_DISABLE;
  mGlobalNvsArea.Area->DockLpt  = GLOBAL_NVS_DEVICE_DISABLE;
  mGlobalNvsArea.Area->DockFdc  = GLOBAL_NVS_DEVICE_DISABLE;

  //
  // Check ID for NS87393 SIO, which is connected to Thimble Peak.
  //
  PortData = 0x20;
  Status = mCpuIo->Io.Write (
                        mCpuIo,
                        EfiCpuIoWidthUint8,
                        NAT_CONFIG_INDEX,
                        1,
                        &PortData
                        );
  ASSERT_EFI_ERROR (Status);

  Status = mCpuIo->Io.Read (
                        mCpuIo,
                        EfiCpuIoWidthUint8,
                        NAT_CONFIG_DATA,
                        1,
                        &PortData
                        );
  ASSERT_EFI_ERROR (Status);

  if (PortData == 0xEA) {
    mGlobalNvsArea.Area->DockedSioPresent = GLOBAL_NVS_DEVICE_ENABLE;
    mGlobalNvsArea.Area->DockComA         = GLOBAL_NVS_DEVICE_ENABLE;
    mGlobalNvsArea.Area->DockComB         = GLOBAL_NVS_DEVICE_ENABLE;
  }

  if (mGlobalNvsArea.Area->DockedSioPresent != GLOBAL_NVS_DEVICE_ENABLE) {
    //
    // Check ID for SIO WPCN381U
    //
    Status = mCpuIo->Io.Read (
                          mCpuIo,
                          EfiCpuIoWidthUint8,
                          WPCN381U_CONFIG_INDEX,
                          1,
                          &PortData
                          );
    ASSERT_EFI_ERROR (Status);
    if (PortData != 0xFF) {
      PortData = 0x20;
      Status = mCpuIo->Io.Write (
                            mCpuIo,
                            EfiCpuIoWidthUint8,
                            WPCN381U_CONFIG_INDEX,
                            1,
                            &PortData
                            );
      ASSERT_EFI_ERROR (Status);
      Status = mCpuIo->Io.Read (
                            mCpuIo,
                            EfiCpuIoWidthUint8,
                            WPCN381U_CONFIG_DATA,
                            1,
                            &PortData
                            );
      ASSERT_EFI_ERROR (Status);
      if ((PortData == WPCN381U_CHIP_ID) || (PortData == WDCP376_CHIP_ID)) {
        mGlobalNvsArea.Area->WPCN381U = GLOBAL_NVS_DEVICE_ENABLE;
        mGlobalNvsArea.Area->OnboardCom = GLOBAL_NVS_DEVICE_ENABLE;
        mGlobalNvsArea.Area->OnboardComCir = GLOBAL_NVS_DEVICE_DISABLE;
      }
    }
  }

  //
  // Enable APIC
  //
  mGlobalNvsArea.Area->ApicEnable = GLOBAL_NVS_DEVICE_ENABLE;

  //
  // EC function
  //
  mGlobalNvsArea.Area->EcAvailable                 = mPlatformInfo->EcPresent;
  mGlobalNvsArea.Area->EcLowPowerMode              = mSystemConfiguration.EcLowPowerMode; // AdvancedFeaturesContent
#ifndef MINTREE_FLAG
  mGlobalNvsArea.Area->RunTimeVmControl            = mSystemConfiguration.SlpS0VmSupport ? mPlatformInfo->RunTimeVmControl : 0;
  mGlobalNvsArea.Area->RunTimeVmVrGpio             = PcdGet32 (PcdRunTimeVmVrGpio);
  mGlobalNvsArea.Area->EcSmiGpioPin = PcdGet32(PcdEcSmiGpio);
  mGlobalNvsArea.Area->EcLowPowerModeGpioPin = PcdGet32(PcdEcLowPowerExitGpio);
#endif

#ifndef MINTREE_FLAG
  //
  // Comms Hub Support Enable/Disable
  //
  mGlobalNvsArea.Area->CommsHubEnable  = mMeSetup.CommsHubEnable;

  mGlobalNvsArea.Area->IuerButtonEnable = SaSetup.IuerButtonEnable;

  //
  // Enable 10sec Power Button Override only if CS is enabled.
  //
  if ((mSystemConfiguration.LowPowerS0Idle == 1) && (mSystemConfiguration.TenSecondPowerButtonEnable == 1)) {
    mGlobalNvsArea.Area->TenSecondPowerButtonEnable |= BIT0;    // Enable 10sec Power Button OVR.
  } else {
    mGlobalNvsArea.Area->TenSecondPowerButtonEnable &= ~(BIT0); // Disable 10sec Power Button OVR.
  }
  mGlobalNvsArea.Area->TenSecondPowerButtonEnable |= (SaSetup.SlateIndicatorRT << 3);
  mGlobalNvsArea.Area->TenSecondPowerButtonEnable |= (SaSetup.DockIndicatorRT << 4);
  mGlobalNvsArea.Area->IuerConvertibleEnable = SaSetup.IuerConvertibleEnable;
  mGlobalNvsArea.Area->IuerDockEnable = SaSetup.IuerDockEnable;

  mGlobalNvsArea.Area->VirtualGpioButtonSxBitmask  = 0x00;
  mGlobalNvsArea.Area->VirtualGpioButtonSxBitmask |=  SaSetup.SlateIndicatorSx << 6;
  mGlobalNvsArea.Area->VirtualGpioButtonSxBitmask |=  SaSetup.DockIndicatorSx << 7;

  GbePciBase = MmPciBase(
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_LAN,
    PCI_FUNCTION_NUMBER_PCH_LAN
  );

  if (MmioRead32(GbePciBase) != 0xFFFFFFFF) {
    LanControllerAvailable = TRUE;
  }

  SkyCamSensorsAvailable = FALSE; // Assigning to False by default
  Cio2PciBase = MmPciBase (
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_CIO2,
    PCI_FUNCTION_NUMBER_PCH_CIO2
  );
  if (MmioRead32 (Cio2PciBase) != 0xFFFFFFFF) {
    SkyCamSensorsAvailable = TRUE;
  }

  mGlobalNvsArea.Area->WwanRtd3Option = mSystemConfiguration.Rtd3WWAN;
#endif // MINTREE_FLAG


  //
  // Low Power S0 Idle - Enabled/Disabled
  //
  mGlobalNvsArea.Area->LowPowerS0Idle = mSystemConfiguration.LowPowerS0Idle;

  //
  // Micro-PEP constraint list
  //
#ifndef MINTREE_FLAG
  mGlobalNvsArea.Area->LowPowerS0IdleConstraint = ((mSystemConfiguration.PepGbe && LanControllerAvailable)                              <<  0) | // Bit[0]   - En/Dis LAN(GBE)
                                                  ((SaSetup.SaImguEnable && mSystemConfiguration.PepIsp)                                <<  1) | // Bit[1]   - En/Dis CSIO
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexUart0] && mSystemConfiguration.PepUart) <<  2) | // Bit[2]   - En/Dis UART0
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexUart1] && mSystemConfiguration.PepUart) <<  3) | // Bit[3]   - En/Dis UART1
                                                  ((mSystemConfiguration.PepTbt && TbtHostRouterRtd3Capability)                         <<  4) | // Bit[4]   - En/Dis TBT RP
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexI2C0] && mSystemConfiguration.PepI2c0)  <<  5) | // Bit[5]   - En/Dis I2C0
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexI2C1] && mSystemConfiguration.PepI2c1)  <<  6) | // Bit[6]   - En/Dis I2C1
                                                  ( mSystemConfiguration.PepXhci                                                        <<  7) | // Bit[7]   - En/Dis XHCI
                                                  (((mPchSetup.PchHdAudio && mSystemConfiguration.PepAudio))                            <<  8) | // Bit[8]   - En/Dis HD Audio (includes ADSP)
                                                  ( mSystemConfiguration.PepGfx                                                         <<  9) | // Bit[9]   - En/Dis GFX
                                                  ( mSystemConfiguration.PepCpu                                                         << 10) | // Bit[10]  - En/Dis CPU
                                                  ((mPchSetup.PchScsEmmcEnabled && mSystemConfiguration.PepEmmc)                        << 11) | // Bit[11]  - En/Dis EMMC
                                                  ((mPchSetup.PchScsSdCardEnabled && mSystemConfiguration.PepSdxc)                      << 12) | // Bit[12]  - En/Dis SDXC
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexI2C2] && mSystemConfiguration.PepI2c2)  << 13) | // Bit[13]  - En/Dis I2C2
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexI2C3] && mSystemConfiguration.PepI2c3)  << 14) | // Bit[14]  - En/Dis I2C3
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexI2C4] && mSystemConfiguration.PepI2c4)  << 15) | // Bit[15]  - En/Dis I2C4
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexI2C5] && mSystemConfiguration.PepI2c5)  << 16) | // Bit[16]  - En/Dis I2C5
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexUart2] && mSystemConfiguration.PepUart) << 17) | // Bit[17]  - En/Dis UART2
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexSpi0] && mSystemConfiguration.PepSpi)   << 18) | // Bit[18]  - En/Dis SPI0
                                                  ((mPchSetup.PchSerialIoDevice[PchSerialIoIndexSpi1] && mSystemConfiguration.PepSpi)   << 19) | // Bit[19]  - En/Dis SPI1
                                                  ( mSystemConfiguration.PepStorageControllers                                          << 20) | // Bit[20]  - En/Dis SATA Controllers
                                                  ( mSystemConfiguration.PepSataRaidVol0                                                << 21) | // Bit[1]   - En/Dis SATA RAID Volume 0
                                                  ( mSystemConfiguration.PepSataPort0                                                   << 22) | // Bit[2]   - En/Dis SATA PORT0
                                                  ( mSystemConfiguration.PepSataPort1                                                   << 23) | // Bit[3]   - En/Dis SATA PORT1
                                                  ( mSystemConfiguration.PepSataPort2                                                   << 24) | // Bit[4]   - En/Dis SATA PORT2
                                                  ( mSystemConfiguration.PepSataPort3                                                   << 25) | // Bit[5]   - En/Dis SATA PORT3
                                                  ( mSystemConfiguration.PepSataPort4                                                   << 26) | // Bit[6]   - En/Dis SATA PORT4
                                                  ( mSystemConfiguration.PepSataPort5                                                   << 27) | // Bit[7]   - En/Dis SATA PORT5
                                                  ( mSystemConfiguration.PepSataNvm1                                                    << 28) | // Bit[8]   - En/Dis SATA NVM1
                                                  ( mSystemConfiguration.PepSataNvm2                                                    << 29) | // Bit[9]   - En/Dis SATA NVM2
                                                  ( mSystemConfiguration.PepSataNvm3                                                    << 30) | // Bit[10]  - En/Dis SATA NVM3
                                                  ( mSystemConfiguration.PepCsme                                                        << 31);  // Bit[31]  - En/Dis CSME

  DEBUG((DEBUG_INFO, "ACPI NVS, LowPowerS0IdleConstraint(Micro-pep constraints) = 0x%X \n", mGlobalNvsArea.Area->LowPowerS0IdleConstraint));

  mGlobalNvsArea.Area->LowPowerS0IdleConstraint1 = ((mSystemConfiguration.PepCio2 && SkyCamSensorsAvailable)                             <<  0);  // Bit[0]   - En/Dis SkyCamSensors

  DEBUG((DEBUG_INFO, "ACPI NVS, LowPowerS0IdleConstraint 01 (Micro-pep constraints) = 0x%X \n", mGlobalNvsArea.Area->LowPowerS0IdleConstraint1));
#endif // MINTREE_FLAG

  //
  // Disable / Enable PEP F1 constraints for WiGig device based on user settings
  //
  mGlobalNvsArea.Area->PepWiGigF1 = mSystemConfiguration.PepWiGigF1;

  //
  // Update SATA port connect status for PEPD.  This needs to be done only when CS is enabled.
  // Note: We cannot do this inside ASL code since SATA device has a dependency to PEPD device and PEPD device
  // needs to know SATA device connect status. Deadlock situation between SATA and PEPD.
  //
  if (mSystemConfiguration.LowPowerS0Idle == 1) {
    //
    // Read SATA device PCS field for SATA Port Present status bits if SATA Controller is present.
    //
    if (MmioRead16 (MmPciBase (0, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA) + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {
      PchSeries   = GetPchSeries ();
      if (PchSeries == PchH) {
        mGlobalNvsArea.Area->SataPortState = MmioRead8 (MmPciBase (0, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA) + R_PCH_H_SATA_PCS + 2);
      } else {
        mGlobalNvsArea.Area->SataPortState = MmioRead8 (MmPciBase (0, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA) + R_PCH_LP_SATA_PCS + 1) & 0x0F;
      }
    }
  }

  mGlobalNvsArea.Area->CSNotifyEC      = mSystemConfiguration.CSNotifyEC;
#ifndef MINTREE_FLAG
  mGlobalNvsArea.Area->CSDebugLightEC  = mSystemConfiguration.CSDebugLightEC;

  mGlobalNvsArea.Area->PL1LimitCS      = mSystemConfiguration.PL1LimitCS;
  mGlobalNvsArea.Area->PL1LimitCSValue = mSystemConfiguration.PL1LimitCSValue;

  mGlobalNvsArea.Area->SDS0           = mSystemConfiguration.PchI2c0SensorDevice;
  mGlobalNvsArea.Area->SDM0           = mSystemConfiguration.PchI2c0SensorIrqMode;
  mGlobalNvsArea.Area->TPDB           = mSystemConfiguration.PchI2c0TouchpadBusAddress;
  mGlobalNvsArea.Area->TPDH           = mSystemConfiguration.PchI2c0TouchpadHidAddress;
  mGlobalNvsArea.Area->TPDS           = mSystemConfiguration.PchI2c0TouchpadSpeed;
#endif
  mGlobalNvsArea.Area->SDS1           = mSystemConfiguration.PchI2c1SensorDevice;
#ifndef MINTREE_FLAG
  mGlobalNvsArea.Area->SDM1           = mSystemConfiguration.PchI2c1SensorIrqMode;
  mGlobalNvsArea.Area->TPLB           = mSystemConfiguration.PchI2c1TouchpanelBusAddress;
  mGlobalNvsArea.Area->TPLH           = mSystemConfiguration.PchI2c1TouchpanelHidAddress;
#endif

  if (mSystemConfiguration.PchI2c1SensorDevice == 6) {
    mGlobalNvsArea.Area->TPLS           = mSystemConfiguration.PchI2c1WacompanelSpeed;
  } else {
    mGlobalNvsArea.Area->TPLS           = mSystemConfiguration.PchI2c1TouchpanelSpeed;
  }

#ifndef MINTREE_FLAG
  mGlobalNvsArea.Area->SDS2           = mSystemConfiguration.PchI2c2SensorDevice;
  mGlobalNvsArea.Area->SDS3           = mSystemConfiguration.PchI2c3SensorDevice;
  mGlobalNvsArea.Area->SDS4           = mSystemConfiguration.PchI2c4SensorDevice;
  mGlobalNvsArea.Area->SDS5           = mSystemConfiguration.PchI2c5SensorDevice;

  //mGlobalNvsArea.Area->SDS6           = mSystemConfiguration.PchSpi0SensorDevice;
  //mGlobalNvsArea.Area->SDM6           = mSystemConfiguration.PchI2c0SensorIrqMode;

  //
  // Get Mbp Hob
  //
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (PcdGetBool (PcdPchFpsSupport) &&
      (((MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType != IntelMeCorporateFw) && (mSystemConfiguration.PchFpsEnable == 2)) ||
      (mSystemConfiguration.PchFpsEnable == 0))) {
    mGlobalNvsArea.Area->SDS7         = 0;
  } else {
    mGlobalNvsArea.Area->SDS7         = mSystemConfiguration.PchSpi1SensorDevice;
  }
  mGlobalNvsArea.Area->SDM7           = mSystemConfiguration.PchSpi1SensorIrqMode;

  mGlobalNvsArea.Area->SDS8           = mSystemConfiguration.PchUart0SensorDevice;
  mGlobalNvsArea.Area->SDM8           = mSystemConfiguration.PchUart0SensorIrqMode;

  //mGlobalNvsArea.Area->SDS9           = mSystemConfiguration.PchUart1SensorDevice;
  mGlobalNvsArea.Area->GnssModel      = mSystemConfiguration.GnssModel;
  mGlobalNvsArea.Area->GnssConnection = mSystemConfiguration.GnssConnection;

  //mGlobalNvsArea.Area->SDSA           = mSystemConfiguration.PchUart2SensorDevice;
  //mGlobalNvsArea.Area->SDMA           = mSystemConfiguration.PchI2c0SensorIrqMode;

  mGlobalNvsArea.Area->WTVX           = mSystemConfiguration.PchI2cWittVersion;
  mGlobalNvsArea.Area->WITX           = mSystemConfiguration.PchI2cWittDevice;
  mGlobalNvsArea.Area->UTKX           = mSystemConfiguration.PchUartUtkDevice;
  mGlobalNvsArea.Area->GDBT           = mPchSetup.PchGpioDebounce;
  mGlobalNvsArea.Area->GPTD           = mPchSetup.PchGpioTestDevices;
  mGlobalNvsArea.Area->SPTD           = mPchSetup.PchAdditionalSerialIoDevices;
#endif // MINTREE_FLAG

  //
  // UCMC support
  //
  mGlobalNvsArea.Area->UCSI           = mPchSetup.PchUcsiUcmcDevice;            // AdvancedFeaturesContent

  UcmcGpioTable = (GPIO_INIT_CONFIG *) (UINTN) PcdGet32 (PcdBoardUcmcGpioTable);
  UcmcGpioTableSize = PcdGet16 (PcdBoardUcmcGpioTableSize);

  if (UcmcGpioTableSize) {
    ASSERT (UcmcGpioTableSize == 2);
    mGlobalNvsArea.Area->UcmcPort1Gpio = UcmcGpioTable[0].GpioPad;
    mGlobalNvsArea.Area->UcmcPort2Gpio = UcmcGpioTable[1].GpioPad;
  }
#ifndef MINTREE_FLAG
  //
  // RVP5 and RVP15 boards are also having fix IO expander location in I2C4
  //
  mGlobalNvsArea.Area->GEXN           = PcdGet8 (PcdSpecificIoExpanderBus);
  mGlobalNvsArea.Area->USTP           = mSystemConfiguration.PchSerialIoUseTimingParameters;
  mGlobalNvsArea.Area->SSHI           = mSystemConfiguration.PchSerialIoTimingSSHI;
  mGlobalNvsArea.Area->SSLI           = mSystemConfiguration.PchSerialIoTimingSSLI;
  mGlobalNvsArea.Area->SSDI           = mSystemConfiguration.PchSerialIoTimingSSDI;
  mGlobalNvsArea.Area->FMHI           = mSystemConfiguration.PchSerialIoTimingFMHI;
  mGlobalNvsArea.Area->FMLI           = mSystemConfiguration.PchSerialIoTimingFMLI;
  mGlobalNvsArea.Area->FMDI           = mSystemConfiguration.PchSerialIoTimingFMDI;
  mGlobalNvsArea.Area->FPHI           = mSystemConfiguration.PchSerialIoTimingFPHI;
  mGlobalNvsArea.Area->FPLI           = mSystemConfiguration.PchSerialIoTimingFPLI;
  mGlobalNvsArea.Area->FPDI           = mSystemConfiguration.PchSerialIoTimingFPDI;
  mGlobalNvsArea.Area->M0CI           = mSystemConfiguration.PchSerialIoTimingM0CI;
  mGlobalNvsArea.Area->M1CI           = mSystemConfiguration.PchSerialIoTimingM1CI;
  mGlobalNvsArea.Area->M0CS           = mSystemConfiguration.PchSerialIoTimingM0CS;
  mGlobalNvsArea.Area->M1CS           = mSystemConfiguration.PchSerialIoTimingM1CS;
  mGlobalNvsArea.Area->M0CU           = mSystemConfiguration.PchSerialIoTimingM0CU;
  mGlobalNvsArea.Area->M1CU           = mSystemConfiguration.PchSerialIoTimingM1CU;

  mGlobalNvsArea.Area->FingerPrintSleepGpio = PcdGet32 (PcdFingerPrintSleepGpio);
  mGlobalNvsArea.Area->FingerPrintIrqGpio   = PcdGet32 (PcdFingerPrintIrqGpio);
  mGlobalNvsArea.Area->GnssResetGpio        = PcdGet32 (PcdGnssResetGpio);
  mGlobalNvsArea.Area->BluetoothWakeGpio    = PcdGet32 (PcdBluetoothWakeGpio);
  mGlobalNvsArea.Area->BluetoothRfKillGpio  = PcdGet32 (PcdBluetoothRfKillGpio);
  mGlobalNvsArea.Area->BluetoothIrqGpio     = PcdGet32 (PcdBluetoothIrqGpio);
  mGlobalNvsArea.Area->TouchpadIrqGpio      = PcdGet32 (PcdTouchpadIrqGpio);
  mGlobalNvsArea.Area->TouchpanelIrqGpio    = PcdGet32 (PcdTouchpanelIrqGpio);

//
// AdvancedFeaturesBegin
//
  // Power Meter Support
  mGlobalNvsArea.Area->PowerMeterEnable                   = mSystemConfiguration.PowerMeterEnable;
  mGlobalNvsArea.Area->PAC193xPowerMeterSupport           = PcdGetBool (PcdPAC193xPowerMeterSupport);
  mGlobalNvsArea.Area->PowerMeterI2cControllerNumber      = PcdGet8 (PcdPowerMeterI2cControllerNumber);

  // S3 Wake Capability LID Switch GPE0 Number
  mGlobalNvsArea.Area->LidSwitchGpe0Number                = PcdGet8 (PcdLidSwitchGpe0Number);
//
// AdvancedFeaturesEnd
//

#endif

  mGlobalNvsArea.Area->Revision           = GLOBAL_NVS_AREA_REVISION;
// AMI_OVERRIDE_START - Always report PS2MouseEnable.
#if 0
  if (mSystemConfiguration.FastBoot == 0) {
    mGlobalNvsArea.Area->Ps2MouseEnable  = IsPs2MouseConnected ();
  } else {
    mGlobalNvsArea.Area->Ps2MouseEnable  = 1;
  }
#endif
// AMI_OVERRIDE_END - Always report PS2MouseEnable.
#ifndef MINTREE_FLAG
  if(PcdGetBool (PcdPs2SupportDisable)) {
    mGlobalNvsArea.Area->Ps2KbMsEnable = 0;
  } else {
#endif
    mGlobalNvsArea.Area->Ps2KbMsEnable = mSystemConfiguration.Ps2KbMsEnable;
  } // AdvancedFeaturesContent
#ifndef MINTREE_FLAG
  //
  // Feature Specific RVP Details
  //
  if ((PcdGetBool(PcdUsbTypeCSupport)) && (mSystemConfiguration.UsbTypeCAicSupport == 1)) {
    mGlobalNvsArea.Area->UsbTypeCSupport = 1;
  }
  else {
    mGlobalNvsArea.Area->UsbTypeCSupport = 0;
  }
  mGlobalNvsArea.Area->UsbTypeCMultiPortSupport = PcdGetBool(PcdUsbTypeCMultiPortSupport);
  mGlobalNvsArea.Area->TsOnDimmTemperature      = PcdGetBool (PcdTsOnDimmTemperature);
  mGlobalNvsArea.Area->PcdDsEndpointCamera      = PcdGetBool(PcdDsEndpointCamera);
  mGlobalNvsArea.Area->PcdPercepDs4Camera       = PcdGetBool(PcdPercepDs4Camera);
  mGlobalNvsArea.Area->PcdIvCamera              = PcdGetBool(PcdIvCamera);
  mGlobalNvsArea.Area->PcdRealBattery1Control   = PcdGet8(PcdRealBattery1Control);
  mGlobalNvsArea.Area->PcdRealBattery2Control   = PcdGet8(PcdRealBattery2Control);
  mGlobalNvsArea.Area->PcdSkyCamSensor          = PcdGetBool(PcdSkyCamSensor);
  mGlobalNvsArea.Area->PcdHDAudioI2S            = PcdGetBool(PcdHDAudioI2S);
  mGlobalNvsArea.Area->PcdNCT6776FCOM           = PcdGetBool(PcdNCT6776FCOM);
  mGlobalNvsArea.Area->PcdNCT6776FSIO           = PcdGetBool(PcdNCT6776FSIO);
  mGlobalNvsArea.Area->PcdNCT6776FHWMON         = PcdGetBool(PcdNCT6776FHWMON);
  mGlobalNvsArea.Area->PcdH8S2113SIO            = PcdGetBool(PcdH8S2113SIO);
  mGlobalNvsArea.Area->PcdPowerParticipant      = PcdGetBool(PcdPowerParticipant);
  mGlobalNvsArea.Area->PcdEnablePbok            = PcdGetBool (PcdEnablePbok); // KblGContent
  mGlobalNvsArea.Area->PcdSensorHubApp          = PcdGetBool(PcdSensorHubApp);
  mGlobalNvsArea.Area->PcdEInkApp               = PcdGetBool(PcdEInkApp);
  mGlobalNvsArea.Area->PcdZPODD                 = PcdGet8(PcdZPODD);
  mGlobalNvsArea.Area->PcdVsctTableForSens      = PcdGetBool(PcdVsctTableForSens);

  mGlobalNvsArea.Area->PcdSmcRuntimeSciPin                  = PcdGet32(PcdSmcRuntimeSciPin);
  mGlobalNvsArea.Area->PcdConvertableDockSupport            = PcdGetBool(PcdConvertableDockSupport);
  mGlobalNvsArea.Area->PcdEcHotKeyF3Support                 = PcdGet8(PcdEcHotKeyF3Support);
  mGlobalNvsArea.Area->PcdEcHotKeyF4Support                 = PcdGet8(PcdEcHotKeyF4Support);
  mGlobalNvsArea.Area->PcdEcHotKeyF5Support                 = PcdGet8(PcdEcHotKeyF5Support);
  mGlobalNvsArea.Area->PcdEcHotKeyF6Support                 = PcdGet8(PcdEcHotKeyF6Support);
  mGlobalNvsArea.Area->PcdEcHotKeyF7Support                 = PcdGet8(PcdEcHotKeyF7Support);
  mGlobalNvsArea.Area->PcdEcHotKeyF8Support                 = PcdGet8(PcdEcHotKeyF8Support);
  mGlobalNvsArea.Area->PcdVirtualButtonVolumeUpSupport      = PcdGetBool(PcdVirtualButtonVolumeUpSupport);
  mGlobalNvsArea.Area->PcdVirtualButtonVolumeDownSupport    = PcdGetBool(PcdVirtualButtonVolumeDownSupport);
  mGlobalNvsArea.Area->PcdVirtualButtonHomeButtonSupport    = PcdGetBool(PcdVirtualButtonHomeButtonSupport);
  mGlobalNvsArea.Area->PcdVirtualButtonRotationLockSupport  = PcdGetBool(PcdVirtualButtonRotationLockSupport);
  mGlobalNvsArea.Area->PcdSlateModeSwitchSupport            = PcdGetBool(PcdSlateModeSwitchSupport);
  mGlobalNvsArea.Area->PcdAcDcAutoSwitchSupport             = PcdGetBool(PcdAcDcAutoSwitchSupport);
  mGlobalNvsArea.Area->PcdPmPowerButtonGpioPin              = PcdGet32(PcdPmPowerButtonGpioPin);
  mGlobalNvsArea.Area->PcdAcpiEnableAllButtonSupport        = PcdGetBool(PcdAcpiEnableAllButtonSupport);
  mGlobalNvsArea.Area->PcdAcpiHidDriverButtonSupport        = PcdGetBool(PcdAcpiHidDriverButtonSupport);
  mGlobalNvsArea.Area->PcdPowerButtonSciDisableSupport      = PcdGetBool(PcdPowerButtonSciDisableSupport);


  //Battery present
  mGlobalNvsArea.Area->BatteryPresent = PcdGet8 (PcdBatteryPresent);
#endif // MINTREE_FLAG

  //
  // Always report PS2MouseEnable for non-EC board
  //
// AMI_OVERRIDE_START - Always report PS2MouseEnable.
#if 0
  if (mGlobalNvsArea.Area->EcAvailable == GLOBAL_NVS_DEVICE_DISABLE) {
    mGlobalNvsArea.Area->Ps2MouseEnable = 1;
  }

#else
  mGlobalNvsArea.Area->Ps2MouseEnable = 1;
#endif
// AMI_OVERRIDE_END - Always report PS2MouseEnable.    
#ifndef MINTREE_FLAG
  mGlobalNvsArea.Area->UsbSensorHub    = mSystemConfiguration.UsbSensorHub;

  //
  // Camera Specific Initializations
  //
  mGlobalNvsArea.Area->IvcamPresent = PcdGet8 (PcdIvcamPresent);
  mGlobalNvsArea.Area->IvcamEpAddress = PcdGet8 (PcdIvcamEpAddress);

  mGlobalNvsArea.Area->CameraType   = mSystemConfiguration.CameraType;
  mGlobalNvsArea.Area->IvcamDfuSupport = mSystemConfiguration.IvcamDfuSupport;
  mGlobalNvsArea.Area->IvcamDfuGpio = PcdGet64 (PcdIvcamDfuGpio);
  mGlobalNvsArea.Area->IvcamWakeSupport = mSystemConfiguration.IvcamWakeSupport;

  mGlobalNvsArea.Area->IvcamPowerOnDelay = mSystemConfiguration.IvcamPowerOnDelay;
  mGlobalNvsArea.Area->IvcamPowerOffDelay = mSystemConfiguration.IvcamPowerOffDelay;

  mGlobalNvsArea.Area->IvcamRotation = mSystemConfiguration.IvcamRotation;
  mGlobalNvsArea.Area->IvcamPwrGpio = PcdGet64 (PcdIvcamPwrGpio);

  mGlobalNvsArea.Area->DsPowerOnDelay = mSystemConfiguration.DsPowerOnDelay;
  mGlobalNvsArea.Area->DsPowerOffDelay = mSystemConfiguration.DsPowerOffDelay;
  if (PcdGet8 (PcdDsPresent) != 0) {
    mGlobalNvsArea.Area->DsPresent = PcdGet8 (PcdDsPresent);
    mGlobalNvsArea.Area->DsEpAddress = PcdGet8 (PcdDsEpAddress);
  }
  mGlobalNvsArea.Area->DsRotation = mSystemConfiguration.DsRotation;
  mGlobalNvsArea.Area->DsPwrGpio = PcdGet64 (PcdDsPwrGpio);

  //
  // EInk DFU device enable/disable
  //
  if (PcdGetBool (PcdEInkDfuDevice)) {
    mGlobalNvsArea.Area->EInkDfuEnable = mSystemConfiguration.EnableEInkDfu;
  } else {
    mGlobalNvsArea.Area->EInkDfuEnable = 0;
  }
  if (PcdGetBool (PcdEInkDfuSpecialPowerCycleEnable)) {
    // Toggle EInk DFU GPIO from LOW to HIGH to properly complete Power Cycle and enable EInk Device
    GpioSetOutputValue (GPIO_SKL_LP_GPP_A21, V_PCH_GPIO_TX_STATE_HIGH);
  }

  //
  // SkyCam sensor configuration
  //
  if (mPchSetup.PchSkyCamCio2Enable) {
    mGlobalNvsArea.Area->SkyCamControlLogic0 = mSystemConfiguration.SkyCam_ControlLogic0;
    mGlobalNvsArea.Area->SkyCamControlLogic1 = mSystemConfiguration.SkyCam_ControlLogic1;
    mGlobalNvsArea.Area->SkyCamControlLogic2 = mSystemConfiguration.SkyCam_ControlLogic2;
    mGlobalNvsArea.Area->SkyCamControlLogic3 = mSystemConfiguration.SkyCam_ControlLogic3;
    mGlobalNvsArea.Area->SkyCamLink0Enabled = mSystemConfiguration.SkyCam_Link0;
    mGlobalNvsArea.Area->SkyCamLink1Enabled = mSystemConfiguration.SkyCam_Link1;
    mGlobalNvsArea.Area->SkyCamLink2Enabled = mSystemConfiguration.SkyCam_Link2;
    mGlobalNvsArea.Area->SkyCamLink3Enabled = mSystemConfiguration.SkyCam_Link3;

    mGlobalNvsArea.Area->SkyCamLanesClkDiv = mSystemConfiguration.SkyCam_LanesClkDiv;

    // SkyCam Control Logic 0 specific
    mGlobalNvsArea.Area->SkyCamCtrlLogic0Version = 1;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0_Type = mSystemConfiguration.SkyCam_ControlLogic0_Type;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0_CrdVersion = mSystemConfiguration.SkyCam_ControlLogic0_CrdVersion;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioPinsEnabled = mSystemConfiguration.SkyCam_ControlLogic0_GpioPinsEnabled;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0_I2cBus = mSystemConfiguration.SkyCam_ControlLogic0_I2cChannel;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0_I2cAddress = mSystemConfiguration.SkyCam_ControlLogic0_I2cAddress;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0_Pld = mSystemConfiguration.SkyCam_ControlLogic0_Pld;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0Wled1FlashMaxCurrent = mSystemConfiguration.SkyCamControlLogic0Wled1FlashMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0Wled1TorchMaxCurrent = mSystemConfiguration.SkyCamControlLogic0Wled1TorchMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0Wled2FlashMaxCurrent = mSystemConfiguration.SkyCamControlLogic0Wled2FlashMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0Wled2TorchMaxCurrent = mSystemConfiguration.SkyCamControlLogic0Wled2TorchMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0Wled1Type = mSystemConfiguration.SkyCamControlLogic0Wled1Type;
    mGlobalNvsArea.Area->SkyCamCtrlLogic0Wled2Type = mSystemConfiguration.SkyCamControlLogic0Wled2Type;

    for (Index = 0; Index < SKYCAM_GPIO_COUNT; Index++) {
      if (Index < mSystemConfiguration.SkyCam_ControlLogic0_GpioPinsEnabled) {
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioGroupPadNumber[Index] = mSystemConfiguration.SkyCam_ControlLogic0_GpioGroupPadNumber[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioGroupNumber[Index] = mSystemConfiguration.SkyCam_ControlLogic0_GpioGroupNumber[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioFunction[Index] = mSystemConfiguration.SkyCam_ControlLogic0_GpioFunction[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioActiveValue[Index] = mSystemConfiguration.SkyCam_ControlLogic0_GpioActiveValue[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioInitialValue[Index] = mSystemConfiguration.SkyCam_ControlLogic0_GpioInitialValue[Index];
      } else {
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioGroupPadNumber[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioGroupNumber[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioFunction[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioActiveValue[Index] = 1;
        mGlobalNvsArea.Area->SkyCamCtrlLogic0_GpioInitialValue[Index] = 0;
      }
    }

    // SkyCam Control Logic 1 specific
    mGlobalNvsArea.Area->SkyCamCtrlLogic1Version = 1;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1_Type = mSystemConfiguration.SkyCam_ControlLogic1_Type;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1_CrdVersion = mSystemConfiguration.SkyCam_ControlLogic1_CrdVersion;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioPinsEnabled = mSystemConfiguration.SkyCam_ControlLogic1_GpioPinsEnabled;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1_I2cBus = mSystemConfiguration.SkyCam_ControlLogic1_I2cChannel;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1_I2cAddress = mSystemConfiguration.SkyCam_ControlLogic1_I2cAddress;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1_Pld = mSystemConfiguration.SkyCam_ControlLogic1_Pld;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1Wled1FlashMaxCurrent = mSystemConfiguration.SkyCamControlLogic1Wled1FlashMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1Wled1TorchMaxCurrent = mSystemConfiguration.SkyCamControlLogic1Wled1TorchMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1Wled2FlashMaxCurrent = mSystemConfiguration.SkyCamControlLogic1Wled2FlashMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1Wled2TorchMaxCurrent = mSystemConfiguration.SkyCamControlLogic1Wled2TorchMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1Wled1Type = mSystemConfiguration.SkyCamControlLogic1Wled1Type;
    mGlobalNvsArea.Area->SkyCamCtrlLogic1Wled2Type = mSystemConfiguration.SkyCamControlLogic1Wled2Type;

    for (Index = 0; Index < SKYCAM_GPIO_COUNT; Index++) {
      if (Index < mSystemConfiguration.SkyCam_ControlLogic1_GpioPinsEnabled) {
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioGroupPadNumber[Index] = mSystemConfiguration.SkyCam_ControlLogic1_GpioGroupPadNumber[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioGroupNumber[Index] = mSystemConfiguration.SkyCam_ControlLogic1_GpioGroupNumber[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioFunction[Index] = mSystemConfiguration.SkyCam_ControlLogic1_GpioFunction[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioActiveValue[Index] = mSystemConfiguration.SkyCam_ControlLogic1_GpioActiveValue[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioInitialValue[Index] = mSystemConfiguration.SkyCam_ControlLogic1_GpioInitialValue[Index];
      } else {
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioGroupPadNumber[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioGroupNumber[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioFunction[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioActiveValue[Index] = 1;
        mGlobalNvsArea.Area->SkyCamCtrlLogic1_GpioInitialValue[Index] = 0;
      }
    }

    // SkyCam Control Logic 2 specific
    mGlobalNvsArea.Area->SkyCamCtrlLogic2Version = 1;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2_Type = mSystemConfiguration.SkyCam_ControlLogic2_Type;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2_CrdVersion = mSystemConfiguration.SkyCam_ControlLogic2_CrdVersion;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioPinsEnabled = mSystemConfiguration.SkyCam_ControlLogic2_GpioPinsEnabled;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2_I2cBus = mSystemConfiguration.SkyCam_ControlLogic2_I2cChannel;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2_I2cAddress = mSystemConfiguration.SkyCam_ControlLogic2_I2cAddress;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2_Pld = mSystemConfiguration.SkyCam_ControlLogic2_Pld;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2Wled1FlashMaxCurrent = mSystemConfiguration.SkyCamControlLogic2Wled1FlashMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2Wled1TorchMaxCurrent = mSystemConfiguration.SkyCamControlLogic2Wled1TorchMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2Wled2FlashMaxCurrent = mSystemConfiguration.SkyCamControlLogic2Wled2FlashMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2Wled2TorchMaxCurrent = mSystemConfiguration.SkyCamControlLogic2Wled2TorchMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2Wled1Type = mSystemConfiguration.SkyCamControlLogic2Wled1Type;
    mGlobalNvsArea.Area->SkyCamCtrlLogic2Wled2Type = mSystemConfiguration.SkyCamControlLogic2Wled2Type;


    for (Index = 0; Index < SKYCAM_GPIO_COUNT; Index++) {
      if (Index < mSystemConfiguration.SkyCam_ControlLogic2_GpioPinsEnabled) {
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioGroupPadNumber[Index] = mSystemConfiguration.SkyCam_ControlLogic2_GpioGroupPadNumber[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioGroupNumber[Index] = mSystemConfiguration.SkyCam_ControlLogic2_GpioGroupNumber[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioFunction[Index] = mSystemConfiguration.SkyCam_ControlLogic2_GpioFunction[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioActiveValue[Index] = mSystemConfiguration.SkyCam_ControlLogic2_GpioActiveValue[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioInitialValue[Index] = mSystemConfiguration.SkyCam_ControlLogic2_GpioInitialValue[Index];
      } else {
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioGroupPadNumber[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioGroupNumber[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioFunction[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioActiveValue[Index] = 1;
        mGlobalNvsArea.Area->SkyCamCtrlLogic2_GpioInitialValue[Index] = 0;
      }
    }

    // SkyCam Control Logic 3 specific
    mGlobalNvsArea.Area->SkyCamCtrlLogic3Version = 1;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3_Type = mSystemConfiguration.SkyCam_ControlLogic3_Type;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3_CrdVersion = mSystemConfiguration.SkyCam_ControlLogic3_CrdVersion;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioPinsEnabled = mSystemConfiguration.SkyCam_ControlLogic3_GpioPinsEnabled;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3_I2cBus = mSystemConfiguration.SkyCam_ControlLogic3_I2cChannel;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3_I2cAddress = mSystemConfiguration.SkyCam_ControlLogic3_I2cAddress;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3_Pld = mSystemConfiguration.SkyCam_ControlLogic3_Pld;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3Wled1FlashMaxCurrent = mSystemConfiguration.SkyCamControlLogic3Wled1FlashMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3Wled1TorchMaxCurrent = mSystemConfiguration.SkyCamControlLogic3Wled1TorchMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3Wled2FlashMaxCurrent = mSystemConfiguration.SkyCamControlLogic3Wled2FlashMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3Wled2TorchMaxCurrent = mSystemConfiguration.SkyCamControlLogic3Wled2TorchMaxCurrent;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3Wled1Type = mSystemConfiguration.SkyCamControlLogic3Wled1Type;
    mGlobalNvsArea.Area->SkyCamCtrlLogic3Wled2Type = mSystemConfiguration.SkyCamControlLogic3Wled2Type;

    for (Index = 0; Index < SKYCAM_GPIO_COUNT; Index++) {
      if (Index < mSystemConfiguration.SkyCam_ControlLogic3_GpioPinsEnabled) {
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioGroupPadNumber[Index] = mSystemConfiguration.SkyCam_ControlLogic3_GpioGroupPadNumber[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioGroupNumber[Index] = mSystemConfiguration.SkyCam_ControlLogic3_GpioGroupNumber[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioFunction[Index] = mSystemConfiguration.SkyCam_ControlLogic3_GpioFunction[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioActiveValue[Index] = mSystemConfiguration.SkyCam_ControlLogic3_GpioActiveValue[Index];
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioInitialValue[Index] = mSystemConfiguration.SkyCam_ControlLogic3_GpioInitialValue[Index];
      } else {
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioGroupPadNumber[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioGroupNumber[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioFunction[Index] = 0;
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioActiveValue[Index] = 1;
        mGlobalNvsArea.Area->SkyCamCtrlLogic3_GpioInitialValue[Index] = 0;
      }
    }

    // SkyCam Link0 specific
    mGlobalNvsArea.Area->SkyCamLink0SensorModel = mSystemConfiguration.SkyCam_Link0_SensorModel;
    mGlobalNvsArea.Area->SkyCamLink0Pld = mSystemConfiguration.SkyCam_Link0_CameraPhysicalLocation;
    ZeroMem(ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    // Need to copy to temporary alligned buffer due to pack(1) in SetupVariables.h
    CopyMem(ModuleName,mSystemConfiguration.SkyCam_Link0_ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    UnicodeStrToAsciiStrS (ModuleName, (CHAR8 *) mGlobalNvsArea.Area->SkyCamLink0ModuleName, sizeof (mGlobalNvsArea.Area->SkyCamLink0ModuleName));
    ZeroMem(ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    CopyMem(ModuleName,mSystemConfiguration.SkyCam_Link0_UserHid, sizeof(UINT16) * SKYCAM_HID_LENGTH);
    UnicodeStrToAsciiStrS (ModuleName, (CHAR8 *) mGlobalNvsArea.Area->SkyCamLink0UserHid, sizeof (mGlobalNvsArea.Area->SkyCamLink0UserHid));
    mGlobalNvsArea.Area->SkyCamLink0I2cDevicesEnabled = mSystemConfiguration.SkyCam_Link0_I2cDevicesEnabled;
    mGlobalNvsArea.Area->SkyCamLink0I2cBus = mSystemConfiguration.SkyCam_Link0_I2cChannel;

    // SkyCam Link0 I2C Devices
    for (Index = 0; Index < SKYCAM_I2C_DEVICES_COUNT; Index++) {
      mGlobalNvsArea.Area->SkyCamLink0I2cAddrDev[Index] = mSystemConfiguration.SkyCam_Link0_I2cAddress[Index];
      mGlobalNvsArea.Area->SkyCamLink0I2cDeviceType[Index] = mSystemConfiguration.SkyCam_Link0_I2cDeviceType[Index];
    }
    mGlobalNvsArea.Area->SkyCamLink0DD_Version = 1;
    mGlobalNvsArea.Area->SkyCamLink0DD_CrdVersion = mSystemConfiguration.SkyCam_Link0_DriverData_CrdVersion;
    mGlobalNvsArea.Area->SkyCamLink0DD_LinkUsed = 0;
    mGlobalNvsArea.Area->SkyCamLink0DD_LaneUsed = mSystemConfiguration.SkyCam_Link0_DriverData_LaneUsed;
    mGlobalNvsArea.Area->SkyCamLink0DD_EepromType = mSystemConfiguration.SkyCam_Link0_DriverData_EepromType;
    mGlobalNvsArea.Area->SkyCamLink0DD_VcmType = mSystemConfiguration.SkyCam_Link0_DriverData_VcmType;
    mGlobalNvsArea.Area->SkyCamLink0DD_Mclk = mSystemConfiguration.SkyCam_Link0_DriverData_Mclk;
    mGlobalNvsArea.Area->SkyCamLink0DD_ControlLogic = mSystemConfiguration.SkyCam_Link0_DriverData_ControlLogic;
    mGlobalNvsArea.Area->SkyCamLink0DD_FlashSupport = mSystemConfiguration.SkyCam_Link0_DriverData_FlashSupport;
    mGlobalNvsArea.Area->SkyCamLink0DDPrivacyLed = mSystemConfiguration.SkyCamLink0DriverDataPrivacyLed;
    mGlobalNvsArea.Area->SkyCamLink0DD_Degree = mSystemConfiguration.SkyCam_Link0_DriverData_Degree;
    mGlobalNvsArea.Area->SkyCamLink0DDPmicPosition = mSystemConfiguration.SkyCamLink0DriverDataPmicPosition;
    mGlobalNvsArea.Area->SkyCamLink0DDVoltageRail = mSystemConfiguration.SkyCamLink0DriverDataVoltageRail;

    // SkyCam Link1 specific
    mGlobalNvsArea.Area->SkyCamLink1SensorModel = mSystemConfiguration.SkyCam_Link1_SensorModel;
    mGlobalNvsArea.Area->SkyCamLink1Pld = mSystemConfiguration.SkyCam_Link1_CameraPhysicalLocation;
    ZeroMem(ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    // Need to copy to temporary alligned buffer due to pack(1) in SetupVariables.h
    CopyMem(ModuleName,mSystemConfiguration.SkyCam_Link1_ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    UnicodeStrToAsciiStrS (ModuleName, (CHAR8 *) mGlobalNvsArea.Area->SkyCamLink1ModuleName, sizeof (mGlobalNvsArea.Area->SkyCamLink1ModuleName));
    ZeroMem(ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    CopyMem(ModuleName,mSystemConfiguration.SkyCam_Link1_UserHid, sizeof(UINT16) * SKYCAM_HID_LENGTH);
    UnicodeStrToAsciiStrS (ModuleName, (CHAR8 *) mGlobalNvsArea.Area->SkyCamLink1UserHid, sizeof (mGlobalNvsArea.Area->SkyCamLink1UserHid));
    mGlobalNvsArea.Area->SkyCamLink1I2cDevicesEnabled = mSystemConfiguration.SkyCam_Link1_I2cDevicesEnabled;
    mGlobalNvsArea.Area->SkyCamLink1I2cBus = mSystemConfiguration.SkyCam_Link1_I2cChannel;

    // SkyCam Link1 I2C Devices
    for (Index = 0; Index < SKYCAM_I2C_DEVICES_COUNT; Index++) {
      mGlobalNvsArea.Area->SkyCamLink1I2cAddrDev[Index] = mSystemConfiguration.SkyCam_Link1_I2cAddress[Index];
      mGlobalNvsArea.Area->SkyCamLink1I2cDeviceType[Index] = mSystemConfiguration.SkyCam_Link1_I2cDeviceType[Index];
    }
    mGlobalNvsArea.Area->SkyCamLink1DD_Version = 1;
    mGlobalNvsArea.Area->SkyCamLink1DD_CrdVersion = mSystemConfiguration.SkyCam_Link1_DriverData_CrdVersion;
    mGlobalNvsArea.Area->SkyCamLink1DD_LinkUsed = 1;
    mGlobalNvsArea.Area->SkyCamLink1DD_LaneUsed = mSystemConfiguration.SkyCam_Link1_DriverData_LaneUsed;
    mGlobalNvsArea.Area->SkyCamLink1DD_EepromType = mSystemConfiguration.SkyCam_Link1_DriverData_EepromType;
    mGlobalNvsArea.Area->SkyCamLink1DD_VcmType = mSystemConfiguration.SkyCam_Link1_DriverData_VcmType;
    mGlobalNvsArea.Area->SkyCamLink1DD_Mclk = mSystemConfiguration.SkyCam_Link1_DriverData_Mclk;
    mGlobalNvsArea.Area->SkyCamLink1DD_ControlLogic = mSystemConfiguration.SkyCam_Link1_DriverData_ControlLogic;
    mGlobalNvsArea.Area->SkyCamLink1DD_FlashSupport = mSystemConfiguration.SkyCam_Link1_DriverData_FlashSupport;
    mGlobalNvsArea.Area->SkyCamLink1DDPrivacyLed = mSystemConfiguration.SkyCamLink1DriverDataPrivacyLed;
    mGlobalNvsArea.Area->SkyCamLink1DD_Degree = mSystemConfiguration.SkyCam_Link1_DriverData_Degree;
    mGlobalNvsArea.Area->SkyCamLink1DDPmicPosition = mSystemConfiguration.SkyCamLink1DriverDataPmicPosition;
    mGlobalNvsArea.Area->SkyCamLink1DDVoltageRail = mSystemConfiguration.SkyCamLink1DriverDataVoltageRail;

    // SkyCam Link2 specific
    mGlobalNvsArea.Area->SkyCamLink2SensorModel = mSystemConfiguration.SkyCam_Link2_SensorModel;
    mGlobalNvsArea.Area->SkyCamLink2Pld = mSystemConfiguration.SkyCam_Link2_CameraPhysicalLocation;
    ZeroMem(ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    // Need to copy to temporary alligned buffer due to pack(1) in SetupVariables.h
    CopyMem(ModuleName,mSystemConfiguration.SkyCam_Link2_ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    UnicodeStrToAsciiStrS (ModuleName, (CHAR8 *) mGlobalNvsArea.Area->SkyCamLink2ModuleName, sizeof (mGlobalNvsArea.Area->SkyCamLink2ModuleName));
    ZeroMem(ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    CopyMem(ModuleName,mSystemConfiguration.SkyCam_Link2_UserHid, sizeof(UINT16) * SKYCAM_HID_LENGTH);
    UnicodeStrToAsciiStrS (ModuleName, (CHAR8 *) mGlobalNvsArea.Area->SkyCamLink2UserHid, sizeof (mGlobalNvsArea.Area->SkyCamLink2UserHid));
    mGlobalNvsArea.Area->SkyCamLink2I2cDevicesEnabled = mSystemConfiguration.SkyCam_Link2_I2cDevicesEnabled;
    mGlobalNvsArea.Area->SkyCamLink2I2cBus = mSystemConfiguration.SkyCam_Link2_I2cChannel;

    // SkyCam Link2 I2C Devices
    for (Index = 0; Index < SKYCAM_I2C_DEVICES_COUNT; Index++) {
      mGlobalNvsArea.Area->SkyCamLink2I2cAddrDev[Index] = mSystemConfiguration.SkyCam_Link2_I2cAddress[Index];
      mGlobalNvsArea.Area->SkyCamLink2I2cDeviceType[Index] = mSystemConfiguration.SkyCam_Link2_I2cDeviceType[Index];
    }
    mGlobalNvsArea.Area->SkyCamLink2DD_Version = 1;
    mGlobalNvsArea.Area->SkyCamLink2DD_CrdVersion = mSystemConfiguration.SkyCam_Link2_DriverData_CrdVersion;
    mGlobalNvsArea.Area->SkyCamLink2DD_LinkUsed = 2;
    mGlobalNvsArea.Area->SkyCamLink2DD_LaneUsed = mSystemConfiguration.SkyCam_Link2_DriverData_LaneUsed;
    mGlobalNvsArea.Area->SkyCamLink2DD_EepromType = mSystemConfiguration.SkyCam_Link2_DriverData_EepromType;
    mGlobalNvsArea.Area->SkyCamLink2DD_VcmType = mSystemConfiguration.SkyCam_Link2_DriverData_VcmType;
    mGlobalNvsArea.Area->SkyCamLink2DD_Mclk = mSystemConfiguration.SkyCam_Link2_DriverData_Mclk;
    mGlobalNvsArea.Area->SkyCamLink2DD_ControlLogic = mSystemConfiguration.SkyCam_Link2_DriverData_ControlLogic;
    mGlobalNvsArea.Area->SkyCamLink2DD_FlashSupport = mSystemConfiguration.SkyCam_Link2_DriverData_FlashSupport;
    mGlobalNvsArea.Area->SkyCamLink2DDPrivacyLed = mSystemConfiguration.SkyCamLink2DriverDataPrivacyLed;
    mGlobalNvsArea.Area->SkyCamLink2DD_Degree = mSystemConfiguration.SkyCam_Link2_DriverData_Degree;
    mGlobalNvsArea.Area->SkyCamLink2DDPmicPosition = mSystemConfiguration.SkyCamLink2DriverDataPmicPosition;
    mGlobalNvsArea.Area->SkyCamLink2DDVoltageRail = mSystemConfiguration.SkyCamLink2DriverDataVoltageRail;

    // SkyCam Link3 specific
    mGlobalNvsArea.Area->SkyCamLink3SensorModel = mSystemConfiguration.SkyCam_Link3_SensorModel;
    mGlobalNvsArea.Area->SkyCamLink3Pld = mSystemConfiguration.SkyCam_Link3_CameraPhysicalLocation;
    ZeroMem(ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    // Need to copy to temporary alligned buffer due to pack(1) in SetupVariables.h
    CopyMem(ModuleName,mSystemConfiguration.SkyCam_Link3_ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    UnicodeStrToAsciiStrS (ModuleName, (CHAR8 *) mGlobalNvsArea.Area->SkyCamLink3ModuleName, sizeof (mGlobalNvsArea.Area->SkyCamLink3ModuleName));
    ZeroMem(ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    CopyMem(ModuleName,mSystemConfiguration.SkyCam_Link3_UserHid, sizeof(UINT16) * SKYCAM_HID_LENGTH);
    UnicodeStrToAsciiStrS (ModuleName, (CHAR8 *) mGlobalNvsArea.Area->SkyCamLink3UserHid, sizeof (mGlobalNvsArea.Area->SkyCamLink3UserHid));
    mGlobalNvsArea.Area->SkyCamLink3I2cDevicesEnabled = mSystemConfiguration.SkyCam_Link3_I2cDevicesEnabled;
    mGlobalNvsArea.Area->SkyCamLink3I2cBus = mSystemConfiguration.SkyCam_Link3_I2cChannel;

    // SkyCam Link3 I2C Devices
    for (Index = 0; Index < SKYCAM_I2C_DEVICES_COUNT; Index++) {
      mGlobalNvsArea.Area->SkyCamLink3I2cAddrDev[Index] = mSystemConfiguration.SkyCam_Link3_I2cAddress[Index];
      mGlobalNvsArea.Area->SkyCamLink3I2cDeviceType[Index] = mSystemConfiguration.SkyCam_Link3_I2cDeviceType[Index];
    }
    mGlobalNvsArea.Area->SkyCamLink3DD_Version = 1;
    mGlobalNvsArea.Area->SkyCamLink3DD_CrdVersion = mSystemConfiguration.SkyCam_Link3_DriverData_CrdVersion;
    mGlobalNvsArea.Area->SkyCamLink3DD_LinkUsed = 3;
    mGlobalNvsArea.Area->SkyCamLink3DD_LaneUsed = mSystemConfiguration.SkyCam_Link3_DriverData_LaneUsed;
    mGlobalNvsArea.Area->SkyCamLink3DD_EepromType = mSystemConfiguration.SkyCam_Link3_DriverData_EepromType;
    mGlobalNvsArea.Area->SkyCamLink3DD_VcmType = mSystemConfiguration.SkyCam_Link3_DriverData_VcmType;
    mGlobalNvsArea.Area->SkyCamLink3DD_Mclk = mSystemConfiguration.SkyCam_Link3_DriverData_Mclk;
    mGlobalNvsArea.Area->SkyCamLink3DD_ControlLogic = mSystemConfiguration.SkyCam_Link3_DriverData_ControlLogic;
    mGlobalNvsArea.Area->SkyCamLink3DD_FlashSupport = mSystemConfiguration.SkyCam_Link3_DriverData_FlashSupport;
    mGlobalNvsArea.Area->SkyCamLink3DDPrivacyLed = mSystemConfiguration.SkyCamLink3DriverDataPrivacyLed;
    mGlobalNvsArea.Area->SkyCamLink3DD_Degree = mSystemConfiguration.SkyCam_Link3_DriverData_Degree;
    mGlobalNvsArea.Area->SkyCamLink3DDPmicPosition = mSystemConfiguration.SkyCamLink3DriverDataPmicPosition;
    mGlobalNvsArea.Area->SkyCamLink3DDVoltageRail = mSystemConfiguration.SkyCamLink3DriverDataVoltageRail;
  }

  //
  // HDAudio Configuration
  //
  mGlobalNvsArea.Area->I2SC = mPchSetup.PchHdAudioI2sCodecSelect;

  for (Index = 0; Index < HDAUDIO_PP_MODULES; Index++) {
    mGlobalNvsArea.Area->HdaDspPpModuleMask |= (UINT32)(mPchSetup.PchHdAudioPostProcessingMod[Index] ? (1 << Index) : 0);
  }
  DEBUG((DEBUG_INFO,"HDA: HdaDspPpModuleMask [ADPM] = 0x%08x\n", mGlobalNvsArea.Area->HdaDspPpModuleMask));

  if (mPchSetup.PchHdAudioPostProcessingMod[29]){
    DEBUG((DEBUG_INFO,"HDA: AudioDSP Pre/Post-Processing custom module 'Alpha' enabled (BIT29)\n"));

    CopyMem(LocalGuidString, mPchSetup.PchHdAudioPostProcessingModCustomGuid1, GUID_CHARS_NUMBER * sizeof(CHAR16));
    GuidStringToAcpiBuffer (LocalGuidString, &AcpiGuidPart1, &AcpiGuidPart2);
    mGlobalNvsArea.Area->HdaDspPpModCustomGuid1Low  = AcpiGuidPart1;
    mGlobalNvsArea.Area->HdaDspPpModCustomGuid1High = AcpiGuidPart2;
    DEBUG((DEBUG_INFO,"HdaDspPpModCustomGuid1Low  = 0x%016Lx\nHdaDspPpModCustomGuid2High = 0x%016Lx\n",
            mGlobalNvsArea.Area->HdaDspPpModCustomGuid1Low, mGlobalNvsArea.Area->HdaDspPpModCustomGuid1High));
  }

  if (mPchSetup.PchHdAudioPostProcessingMod[30]){
    DEBUG((DEBUG_INFO,"HDA: AudioDSP Pre/Post-Processing custom module 'Beta' enabled (BIT30)\n"));

    CopyMem(LocalGuidString, mPchSetup.PchHdAudioPostProcessingModCustomGuid2, GUID_CHARS_NUMBER * sizeof(CHAR16));
    GuidStringToAcpiBuffer (LocalGuidString, &AcpiGuidPart1, &AcpiGuidPart2);
    mGlobalNvsArea.Area->HdaDspPpModCustomGuid2Low  = AcpiGuidPart1;
    mGlobalNvsArea.Area->HdaDspPpModCustomGuid2High = AcpiGuidPart2;
    DEBUG((DEBUG_INFO,"HdaDspPpModCustomGuid2Low  = 0x%016Lx\nHdaDspPpModCustomGuid2High = 0x%016Lx\n",
            mGlobalNvsArea.Area->HdaDspPpModCustomGuid2Low, mGlobalNvsArea.Area->HdaDspPpModCustomGuid2High));
  }

  if (mPchSetup.PchHdAudioPostProcessingMod[31]){
    DEBUG((DEBUG_INFO,"HDA: AudioDSP Pre/Post-Processing custom module 'Gamma' enabled (BIT31)\n"));

    CopyMem(LocalGuidString, mPchSetup.PchHdAudioPostProcessingModCustomGuid3, GUID_CHARS_NUMBER * sizeof(CHAR16));
    GuidStringToAcpiBuffer (LocalGuidString, &AcpiGuidPart1, &AcpiGuidPart2);
    mGlobalNvsArea.Area->HdaDspPpModCustomGuid3Low  = AcpiGuidPart1;
    mGlobalNvsArea.Area->HdaDspPpModCustomGuid3High = AcpiGuidPart2;
    DEBUG((DEBUG_INFO,"HdaDspPpModCustomGuid3Low  = 0x%016Lx\nHdaDspPpModCustomGuid3High = 0x%016Lx\n",
            mGlobalNvsArea.Area->HdaDspPpModCustomGuid3Low, mGlobalNvsArea.Area->HdaDspPpModCustomGuid3High));
  }

  //
  // SCS Configuration
  //
  mGlobalNvsArea.Area->SDWE = mPchSetup.PchScsSdCardSidebandEventsSupport;

  //
  // Thunderbolt(TM) configuration
  //
  if (mSystemConfiguration.TbtSupport == 1 && gTbtInfoHob != NULL) {
    if (GpioCheckFor2Tier (gTbtInfoHob->CioPlugEventGpio.GpioNumber)) {
      mGlobalNvsArea.Area->Checkfor2tier = 1;
    } else {
      mGlobalNvsArea.Area->Checkfor2tier = 0;
    }
  }

  mGlobalNvsArea.Area->PciDelayOptimizationEcr = mSystemConfiguration.PciDelayOptimizationEcr;

  mGlobalNvsArea.Area->TbtSxWakeSwitchLogicEnable            = PcdGetBool(PcdTbtSxWakeSwitchLogicEnable);

  //
  // HID Event Filter Enable/Disable
  //
  mGlobalNvsArea.Area->HidEventFilterEnable = mSystemConfiguration.HidEventFilterDriverEnable;

  mGlobalNvsArea.Area->PcdWwanGnssUartResetGpio = PcdGet32(PcdWwanGnssUartReset);
  mGlobalNvsArea.Area->PcdPchAudioPowerEnable = PcdGet32(PcdPchAudioPowerEnable);
  mGlobalNvsArea.Area->PcdPcieSlot2Wake = PcdGet32(PcdPcieSlot2Wake);
  mGlobalNvsArea.Area->PcdM2WlanReset = PcdGet32(PcdM2WlanReset);
  mGlobalNvsArea.Area->PcdUsbP8Power = PcdGet32(PcdUsbP8Power);
  mGlobalNvsArea.Area->PcdM2PcieSsd2Reset = PcdGet32(PcdM2PcieSsd2Reset);
  mGlobalNvsArea.Area->PcdM2PcieSsd2Power = PcdGet32(PcdM2PcieSsd2Power);
  mGlobalNvsArea.Area->UcsiRetryEC = mSystemConfiguration.UcsiRetryEcEnable;
#endif // MINTREE_FLAG


  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiGlobalNvsAreaProtocolGuid,
                  &mGlobalNvsArea,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate NVS area for VBIOS. This is not currently used.
  //
  Status = AllocateMemoryBelow4G (
             EfiACPIMemoryNVS,
             0x10000,
             &mGfxNvsAddress
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Platform ACPI Tables
  //
// AMI_OVERRIDE_START - Replace the DSDT GUID to AMI's definition
#if 0
  PublishAcpiTablesFromFv (gEfiCallerIdGuid);
#else
  PublishAcpiTablesFromFv(gAmiAslDsdtFileGuid);   
#endif
// AMI_OVERRIDE_END - Replace the DSDT GUID to AMI's definition

  //
  // Reference Code ACPI Tables
  //
  PublishAcpiTablesFromFv (gRcAcpiTableStorageGuid);
#ifndef MINTREE_FLAG
  //
  // Install xHCI ACPI Table
  //
  InstallXhciAcpiTable ();
#endif

  //
  // Save Acpi Setup Variables
  //
#if 0  //AMI_OVERRIDE_START - For AMI Chipset Setup
  Status = SetAcpiSetupVariables ();
  ASSERT_EFI_ERROR (Status);
#endif  //AMI_OVERRIDE_END - For AMI Chipset Setup

// AMI_OVERRIDE_START - EIP251482 
{ 
  UINTN AmiSleepEntryS3Address = (UINTN)&mGlobalNvsArea.Area->AmiSleepEntryS3;
  
  Status = gRT->SetVariable (
                  L"AmiEntryS3Addr",
                  &gEfiGlobalNvsAreaProtocolGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (UINTN),
                  &AmiSleepEntryS3Address
                  );
}  
// AMI_OVERRIDE_END - EIP251482   
  //
  // Finished
  //
  return EFI_SUCCESS;
}

/**
  Sort ordering of CPUs according to the priorities of cores and threads.
  ** ASSUMPTION: 1) single CPU package systsem wide; 2) two threads per core

  @param[in] MpService             The MP servicde protocol (used to retrievel MP info)
  @param[in] NumberOfCPUs          Number of CPUs
  @param[in] NumberOfEnabledCPUs   Number of Enabled CPUs.

  @retval EFI_SUCCESS              The driver installed without error.

**/
EFI_STATUS
SortCpuLocalApicInTable (
  IN  EFI_MP_SERVICES_PROTOCOL          *MpService,
  IN  UINTN                             NumberOfCPUs,
  IN  UINTN                             NumberOfEnabledCPUs
  )
{
  EFI_PROCESSOR_INFORMATION                 *MpContext;
  UINTN                                     BufferSize;
  UINT8                                     Index1;
  UINT8                                     Index;
  UINT8                                     CpuThreadIndex;
  UINTN                                     BspIndex;
  UINT8                                     CurrProcessor;
  EFI_STATUS                                Status;
  UINT8                                     MaxCpuCore;
  UINT8                                     MaxCpuThread;

  BufferSize                    = 0;
  MpContext                     = NULL;
  MaxCpuCore                    = 0;
  MaxCpuThread                  = 0;

  Status = MpService->WhoAmI (
                        MpService,
                        &BspIndex
                        );

  //
  // Fill mCpuApicIdOrderTable
  //
  for (CurrProcessor = 0; CurrProcessor < NumberOfCPUs; CurrProcessor++) {

    MpContext = AllocatePool (sizeof(EFI_PROCESSOR_INFORMATION));
    ASSERT (MpContext != NULL);
    Status = MpService->GetProcessorInfo (
                          MpService,
                          CurrProcessor,
                          MpContext
                          );
    ASSERT_EFI_ERROR (Status);

    if (MpContext == NULL) {
      return Status;
    }
    mCpuApicIdOrderTable[CurrProcessor].ApicId  = (UINT8) MpContext->ProcessorId;
    DEBUG((DEBUG_INFO, "The CurrProcessor 0x%x ApicId is 0x%x\n", CurrProcessor, mCpuApicIdOrderTable[CurrProcessor].ApicId));
    mCpuApicIdOrderTable[CurrProcessor].Flags   = (MpContext->StatusFlag | PROCESSOR_ENABLED_BIT)? 1: 0;
    mCpuApicIdOrderTable[CurrProcessor].Package = (UINT8) MpContext->Location.Package;
    mCpuApicIdOrderTable[CurrProcessor].Die     = (UINT8) 0;
    mCpuApicIdOrderTable[CurrProcessor].Core    = (UINT8) MpContext->Location.Core;
    mCpuApicIdOrderTable[CurrProcessor].Thread  = (UINT8) MpContext->Location.Thread;

    if (MaxCpuThread < (UINT8) MpContext->Location.Thread) {
      MaxCpuThread = (UINT8) MpContext->Location.Thread;
    }

    if (MaxCpuCore < (UINT8) MpContext->Location.Core) {
      MaxCpuCore = (UINT8) MpContext->Location.Core;
    }
  }

  //
  // Do some statistics about the SBSP package
  //
  for (CurrProcessor = 0; CurrProcessor < NumberOfCPUs; CurrProcessor++) {
    if (CurrProcessor == BspIndex) {
      mSbspPackageInfo.BspApicId = mCpuApicIdOrderTable[CurrProcessor].ApicId;
      mSbspPackageInfo.PackageNo   = mCpuApicIdOrderTable[CurrProcessor].Package;
      mSbspPackageInfo.TotalThreads = 0;
      mSbspPackageInfo.CoreNo = 0;
      mSbspPackageInfo.LogicalThreadNo = 0;
    }
  }

  for (CurrProcessor = 0; CurrProcessor < NumberOfCPUs; CurrProcessor++) {
    if (mCpuApicIdOrderTable[CurrProcessor].Package == mSbspPackageInfo.PackageNo) {
      mSbspPackageInfo.TotalThreads++;
      if (mCpuApicIdOrderTable[CurrProcessor].Thread == 0) {
        mSbspPackageInfo.CoreNo++;
      } else {
        mSbspPackageInfo.LogicalThreadNo++;
      }
    }
  }

  //
  // Output debug info
  //
  for (Index = 0; Index < NumberOfCPUs; Index++) {
    DEBUG((DEBUG_INFO, "Package = %x,  Die = %x,  Core = %x,  Thread = %x,  ApicId = %x\n", \
      mCpuApicIdOrderTable[Index].Package, \
      mCpuApicIdOrderTable[Index].Die, \
      mCpuApicIdOrderTable[Index].Core, \
      mCpuApicIdOrderTable[Index].Thread, \
      mCpuApicIdOrderTable[Index].ApicId));
  }

  DEBUG((DEBUG_INFO, "MaxCpuCore    = %x\n", MaxCpuCore));
  DEBUG((DEBUG_INFO, "MaxCpuThread    = %x\n\n", MaxCpuThread));
  DEBUG((DEBUG_INFO, "mSbspPackageInfo.BspApicId        = %x\n", mSbspPackageInfo.BspApicId));
  DEBUG((DEBUG_INFO, "mSbspPackageInfo.TotalThreads     = %x\n", mSbspPackageInfo.TotalThreads));
  DEBUG((DEBUG_INFO, "mSbspPackageInfo.PackageNo        = %x\n", mSbspPackageInfo.PackageNo));
  DEBUG((DEBUG_INFO, "mSbspPackageInfo.CoreNo           = %x\n", mSbspPackageInfo.CoreNo));
  DEBUG((DEBUG_INFO, "mSbspPackageInfo.LogicalThreadNo  = %x\n", mSbspPackageInfo.LogicalThreadNo));

  //
  // First entry is always SBSP
  //
  CurrProcessor = 0;
  mCpuApicIdReorderTable[CurrProcessor].ApicId = mSbspPackageInfo.BspApicId;
  mCpuApicIdReorderTable[CurrProcessor].Package = mSbspPackageInfo.PackageNo;
  mCpuApicIdReorderTable[CurrProcessor].Flags = 1;
  CurrProcessor++;

  //
  // Reorder Core and threads
  //
  for (Index1 = 0; Index1 <= MaxCpuThread; Index1 ++) {
    for (Index = 0; Index <= MaxCpuCore; Index++) {
      for (CpuThreadIndex = 0; CpuThreadIndex < NumberOfCPUs; CpuThreadIndex++) {
        if ((mCpuApicIdOrderTable[CpuThreadIndex].Package == mSbspPackageInfo.PackageNo) &&
            (mCpuApicIdOrderTable[CpuThreadIndex].Thread == Index1) &&
            (mCpuApicIdOrderTable[CpuThreadIndex].Core == Index) &&
            (mCpuApicIdOrderTable[CpuThreadIndex].ApicId != mCpuApicIdReorderTable[0].ApicId)) {
          mCpuApicIdReorderTable[CurrProcessor].ApicId = mCpuApicIdOrderTable[CpuThreadIndex].ApicId;
          mCpuApicIdReorderTable[CurrProcessor].Package = mSbspPackageInfo.PackageNo;
          mCpuApicIdReorderTable[CurrProcessor].Flags = mCpuApicIdOrderTable[CpuThreadIndex].Flags;
          CurrProcessor++;
          break;
        }
      }
    }
  }

  //
  // Output debug info
  //
  for (Index = 0; Index < NumberOfCPUs; Index++) {
    DEBUG((DEBUG_INFO, "Index = %x,  ApicId = %x,  Package = %x\n", \
      Index, \
      mCpuApicIdReorderTable[Index].ApicId, \
      mCpuApicIdReorderTable[Index].Package));
  }

  FreePool (MpContext);

  return  EFI_SUCCESS;
}


EFI_STATUS
AppendCpuMapTableEntry (
  IN  EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE   *AcpiLocalApic,
  IN  UINTN                                         NumberOfCPUs,
  IN  UINTN                                         NumberOfEnabledCPUs,
  IN  UINT8                                         LocalApicCounter
  )
{
  if (LocalApicCounter < NumberOfCPUs) {
    AcpiLocalApic->Flags           = mCpuApicIdReorderTable[LocalApicCounter].Flags;
    AcpiLocalApic->ApicId          = mCpuApicIdReorderTable[LocalApicCounter].ApicId;
    //
    // Make AcpiProcessorIds 1-based, matching the ones in Processor() definitions
    //
    AcpiLocalApic->AcpiProcessorId = LocalApicCounter + 1;
  } else {
    AcpiLocalApic->Flags           = 0;
    AcpiLocalApic->ApicId          = 0xFF;
    //
    // Make AcpiProcessorIds 1-based, matching the ones in Processor() definitions
    //
    AcpiLocalApic->AcpiProcessorId = LocalApicCounter + 1;
  }

  //
  // Display the results
  //
  DEBUG((DEBUG_INFO, "AcpiLocalApic: AcpiProcessorId=%x, ApicId=%x, Flags=%x\n", \
  AcpiLocalApic->AcpiProcessorId, \
  AcpiLocalApic->ApicId, \
  AcpiLocalApic->Flags));

  return EFI_SUCCESS;
}

/**
  I/O work flow to wait input buffer empty in given time.

  @param[in] Timeout       Wating time.

  @retval EFI_TIMEOUT      if input is still not empty in given time.
  @retval EFI_SUCCESS      input is empty.
**/
EFI_STATUS
WaitInputEmpty (
  IN UINTN Timeout
  )
{
  UINTN Delay;
  UINT8 Data;

  Delay = Timeout / 50;

  do {
    Data = IoRead8 (KBC_CMD_STS_PORT);

    //
    // Check keyboard controller status bit 0 and 1 (Output Buffer and Input Buffer status)
    //
    if ((Data & KBC_OUTB) != 0) {
      IoRead8 (KBC_DATA_PORT);
    } else if ((Data & KBC_INPB) == 0) {
      break;
    }

    gBS->Stall (50);
    Delay--;
  } while (Delay != 0);

  if (Delay == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  I/O work flow to wait output buffer full in given time.

  @param[in] Timeout       given time

  @retval EFI_TIMEOUT      output is not full in given time
  @retval EFI_SUCCESS      output is full in given time.
**/
EFI_STATUS
WaitOutputFull (
  IN UINTN Timeout
  )
{
  UINTN Delay;
  UINT8 Data;

  Delay = Timeout / 50;

  do {
    Data = IoRead8 (KBC_CMD_STS_PORT);

    //
    // Check keyboard controller status bit 0(output buffer status)
    //  & bit5(output buffer for auxiliary device)
    //
    if ((Data & (KBC_OUTB | KBC_AUXB)) == (KBC_OUTB | KBC_AUXB)) {
      break;
    }

    gBS->Stall (50);
    Delay--;
  } while (Delay != 0);

  if (Delay == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  I/O work flow of in 8042 Aux data.

  @param Data    Buffer holding return value.

  @retval EFI_SUCCESS Success to excute I/O work flow
  @retval EFI_TIMEOUT Keyboard controller time out.
**/
EFI_STATUS
In8042AuxData (
  IN OUT UINT8 *Data
  )
{
  EFI_STATUS Status;

  //
  // wait for output data
  //
  Status = WaitOutputFull (BAT_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Data = IoRead8 (KBC_DATA_PORT);

  return EFI_SUCCESS;
}

/**
  I/O work flow of outing 8042 Aux command.

  @param Command Aux I/O command

  @retval EFI_SUCCESS Success to excute I/O work flow
  @retval EFI_TIMEOUT Keyboard controller time out.
**/
EFI_STATUS
Out8042AuxCommand (
  IN UINT8 Command
  )
{
  EFI_STATUS  Status;
  UINT8       Data;

  //
  // Wait keyboard controller input buffer empty
  //
  Status = WaitInputEmpty (TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Send write to auxiliary device command
  //
  Data = WRITE_AUX_DEV;
  IoWrite8 (KBC_CMD_STS_PORT, Data);

  Status = WaitInputEmpty (TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Send auxiliary device command
  //
  IoWrite8 (KBC_DATA_PORT, Command);

  //
  // Read return code
  //
  Status = In8042AuxData (&Data);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Data == PS2_ACK) {
    //
    // Receive mouse acknowledge, command send success
    //
    return EFI_SUCCESS;

  } else if (Data == PS2_RESEND) {
    //
    // Resend command
    //
    Status = Out8042AuxCommand (Command);
    if (EFI_ERROR (Status)) {
      return Status;
    }

  } else {
    //
    // Invalid return code
    //
    return EFI_DEVICE_ERROR;

  }

  return EFI_SUCCESS;
}

/**
  Issue command to enable Ps2 mouse.

  @return Status of command issuing.
**/
EFI_STATUS
PS2MouseEnable (
  VOID
  )
{
  //
  // Send auxiliary command to enable mouse
  //
  return Out8042AuxCommand (ENABLE_CMD);
}

/**
  Check whether there is Ps/2 mouse device in system

  @retval TRUE      - Keyboard in System.
  @retval FALSE     - Keyboard not in System.

**/
BOOLEAN
IsPs2MouseConnected (
  VOID
  )
{
  EFI_STATUS Status;

  Status = PS2MouseEnable ();

  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}

// AMI_OVERRIDE_START - For the implemtation of AMI's FACP update.
EFI_STATUS
AmiUpdateFacpTable()
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_ACPI_TABLE_PROTOCOL       *gAcpiTable = NULL;
  EFI_ACPI_SDT_PROTOCOL         *gAcpiSdtProtocol = NULL;
  CPU_GLOBAL_NVS_AREA_PROTOCOL  *gCpuGlobalNvsAreaProtocol = NULL;
  EFI_ACPI_SDT_HEADER           *Table = NULL;
  PFACP_50                      FadtPointer = NULL;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_ACPI_TABLE_VERSION        DoneVersion[2] = {0, 0}; // rev.2 and non-rev.2
  UINTN                         Handle = 0;
  UINTN                         Index = 0;
  
  DEBUG((DEBUG_INFO, "AMI Update ACPI FACP table\n"));
  
  Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, &gAcpiTable);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol(&gEfiAcpiSdtProtocolGuid, NULL, &gAcpiSdtProtocol);
  ASSERT_EFI_ERROR (Status);
  
  do {
    Status = gAcpiSdtProtocol->GetAcpiTable(Index, &Table, &Version, &Handle);
    
    if (Status == EFI_NOT_FOUND){
      DEBUG((DEBUG_INFO, "AcpiPlatform: Not Found FACP table breaking search loop...\n"));
      break;
    }
    
    if (Table->Signature == FACP_SIG){
      DEBUG((DEBUG_INFO, "AcpiPlatform: Find FACP table version:%x | Handle: %x\n", Table->Revision, Handle));
      FadtPointer = (PFACP_50) Table;

// AMI_OVERRIDE_START - Skip TBT function when disabling Thunderbolt_SUPPORT
#if defined(Thunderbolt_SUPPORT) && Thunderbolt_SUPPORT
      //
      // Modify MSI capability based on setup
      //
      if ((mSystemConfiguration.TbtSupport == 1) && (mSystemConfiguration.EnableMsiInFadt == 0)) {
        FadtPointer->IAPC_BOOT_ARCH |= (1 << 3); 
      }
#endif
// AMI_OVERRIDE_END - Skip TBT function when disabling Thunderbolt_SUPPORT
      
      //
      // if Native ASPM is disabled, set FACP table to skip Native ASPM
      //
      if (!IsNativeAspmSupported (&mSystemConfiguration)) {
        DEBUG((DEBUG_INFO, "AcpiPlatform: Update IAPC_BOOT_ARCH\n"));
        FadtPointer->IAPC_BOOT_ARCH |= 0x10;
      }
      
      if (FACP_FLAG_PCI_EXP_WAK) {
          //
          // PME WAKE supported, set PCI_EXP_WAK, BIT14 of Fixed feature flags.
          //
          FadtPointer->FLAGS |= BIT14; //(EFI_ACPI_6_0_PCI_EXP_WAK)
      }

#ifdef AMI_CRB_EC_SUPPORT_FLAG
      //
      // Modify Preferred_PM_Profile field based on Board SKU's. Default is set to Mobile
      //
      if((mPlatformInfo->BoardID == BoardIdSkylakeDtRvp8Crb) ||
         (mPlatformInfo->BoardID == BoardIdSkylakeAioRvp9Crb) ||
         (mPlatformInfo->BoardID == BoardIdSkylakeAioRvp10Erb) ||
         (mPlatformInfo->BoardID == BoardIdSkylakeAioRvp10Crb) ||
         (mPlatformInfo->BoardID == BoardIdSkylakeAioRvp10CrbPpv)) {
        FadtPointer->PM_PPROF = EFI_ACPI_2_0_PM_PROFILE_DESKTOP;
      }
#else
      FadtPointer->PM_PPROF = ACPI_PM_PROFILE;
#endif
      
      if (mSystemConfiguration.LowPowerS0Idle) {
        DEBUG((DEBUG_INFO, "AcpiPlatform: Update FACP flag for S0Idle\n"));
        // The Flags field within the FADT (offset 112)
        //   1) will have a new Low Power S0 Idle Capable ACPI flag (bit offset 21).
        FadtPointer->FLAGS |= BIT21;
        //Only passive docking available in Conected Standby mode. Clear Docking capability Bit
        FadtPointer->FLAGS &= ~BIT9;
      } else {
        FadtPointer->FLAGS &= ~BIT21;
      }
      
      //
      // Modify FADT Fixed Feature Flag to support 10 sec Power button.
      // If 10sec Power button is enabled: Set PWR_BUTTON(bit4) in Fixed Feature Flag(FACP offset 112)
      //
      if ((mSystemConfiguration.LowPowerS0Idle == 1) && (mSystemConfiguration.TenSecondPowerButtonEnable == 1)) {
        FadtPointer->FLAGS |= EFI_ACPI_2_0_PWR_BUTTON; // Set Fixed Power Button flag (Disabling Fixed Power button and enabling Control Method PB)
      } else {
        FadtPointer->FLAGS &= ~(EFI_ACPI_2_0_PWR_BUTTON); // Default clear fixed Power Button flag
      }
      
      //
      // 2. set all GAR register AccessSize to valid value.
      //
      if (FadtPointer->Header.Revision == 5){
        FadtPointer->RESET_REG.AccessSize      = EFI_ACPI_3_0_BYTE;
        FadtPointer->X_PM1a_EVT_BLK.AccessSize = EFI_ACPI_3_0_WORD;
        FadtPointer->X_PM1b_EVT_BLK.AccessSize = EFI_ACPI_3_0_WORD;
        FadtPointer->X_PM1a_CNT_BLK.AccessSize = EFI_ACPI_3_0_WORD;
        FadtPointer->X_PM1b_CNT_BLK.AccessSize = EFI_ACPI_3_0_WORD;
        FadtPointer->X_PM2_CNT_BLK.AccessSize  = EFI_ACPI_3_0_BYTE;
        FadtPointer->X_PM_TMR_BLK.AccessSize   = EFI_ACPI_3_0_DWORD;
        FadtPointer->X_GPE0_BLK.AccessSize     = EFI_ACPI_3_0_BYTE;
        FadtPointer->X_GPE1_BLK.AccessSize     = EFI_ACPI_3_0_BYTE;
		
// AMI_OVERRIDE_START - EIP240597: SleepSmi wasn't trigger while shutdown in VMware ESXi.		
#if HW_REDUCED_ACPI
        // It refer to AcpiBaseLib.c for HW_REDUCED_ACPI.
        //
        FadtPointer->FLAGS |= 1<<20;
        FadtPointer->SLEEP_CONTROL_REG.AddrSpcID   = SLEEP_CONTROL_REG_TYPE;
        FadtPointer->SLEEP_CONTROL_REG.RegBitWidth = SLEEP_CONTROL_REG_BITWIDTH;
        FadtPointer->SLEEP_CONTROL_REG.RegBitOffs  = SLEEP_CONTROL_REG_BITOFFSET;
        FadtPointer->SLEEP_CONTROL_REG.AccessSize  = SLEEP_CONTROL_REG_ACCESS_SIZE;
        FadtPointer->SLEEP_CONTROL_REG.Address     = SLEEP_CONTROL_REG_ADDRESS;

        FadtPointer->SLEEP_STATUS_REG.AddrSpcID    = SLEEP_STATUS_REG_TYPE;
        FadtPointer->SLEEP_STATUS_REG.RegBitWidth  = SLEEP_STATUS_REG_BITWIDTH;
        FadtPointer->SLEEP_STATUS_REG.RegBitOffs   = SLEEP_STATUS_REG_BITOFFSET;
        FadtPointer->SLEEP_STATUS_REG.AccessSize   = SLEEP_STATUS_REG_ACCESS_SIZE;
        FadtPointer->SLEEP_STATUS_REG.Address      = SLEEP_STATUS_REG_ADDRESS;
#else
// AMI_OVERRIDE_END - EIP240597: SleepSmi wasn't trigger while shutdown in VMware ESXi.	
        FadtPointer->SLEEP_CONTROL_REG.AddrSpcID   = 0x1;
        FadtPointer->SLEEP_CONTROL_REG.RegBitWidth = 0x8;
        FadtPointer->SLEEP_CONTROL_REG.RegBitOffs  = 0;
        FadtPointer->SLEEP_CONTROL_REG.AccessSize  = EFI_ACPI_3_0_DWORD;
        FadtPointer->SLEEP_CONTROL_REG.Address     = FadtPointer->PM1a_EVT_BLK + 4;
        FadtPointer->SLEEP_STATUS_REG.AddrSpcID    = 0x1;
        FadtPointer->SLEEP_STATUS_REG.RegBitWidth  = 0x8;
        FadtPointer->SLEEP_STATUS_REG.RegBitOffs   = 0;
        FadtPointer->SLEEP_STATUS_REG.AccessSize   = EFI_ACPI_3_0_DWORD;
        FadtPointer->SLEEP_STATUS_REG.Address      = FadtPointer->PM1a_EVT_BLK;
// AMI_OVERRIDE_START - EIP240597: SleepSmi wasn't trigger while shutdown in VMware ESXi.	
#endif 
// AMI_OVERRIDE_END - EIP240597: SleepSmi wasn't trigger while shutdown in VMware ESXi.
      }
      
      ///
      /// Locate CPU GlobalNvs Protocol.
      ///
      Status = gBS->LocateProtocol (&gCpuGlobalNvsAreaProtocolGuid, NULL, (VOID **) &gCpuGlobalNvsAreaProtocol);
      
      if (!EFI_ERROR (Status)){
        if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C3) {
          FadtPointer->P_LVL3_LAT = FADT_C3_LATENCY;
        }
      }
      
      if ((Table->Revision == EFI_ACPI_TABLE_VERSION_1_0B && DoneVersion[0] == 0) || 
          (Table->Revision != EFI_ACPI_TABLE_VERSION_1_0B && DoneVersion[1] == 0))
      {
        if (Table->Revision == EFI_ACPI_TABLE_VERSION_1_0B) DoneVersion[0] = 1;
        
        if (Table->Revision != EFI_ACPI_TABLE_VERSION_1_0B) DoneVersion[1] = 1;
// AMI_OVERRIDE_START >>> EIP433988 - We have to uninstall the acpi table before installing modified it. 
        Status = gAcpiTable->UninstallAcpiTable (gAcpiTable,
                                                Handle );                                   
        Handle = 0;
// AMI_OVERRIDE_END <<< EIP433988 - We have to uninstall the acpi table before installing modified it. 
        Status = gAcpiTable->InstallAcpiTable (gAcpiTable,
                                               Table,
                                               Table->Length,
                                               &Handle );
        Index = 0;
      } else Index++;
        
      if (DoneVersion[0] && DoneVersion[1]) break;
      
    } else Index++;
  } while(1);
  
  return EFI_SUCCESS;
}
// AMI_OVERRIDE_END - For the implemtation of AMI's FACP update.
