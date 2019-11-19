/** @file
  ACPI Features Driver

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2018 Intel Corporation.

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

#include "AcpiFeatures.h"
#include <PlatformBoardId.h>
#include <PlatformInfo.h>
#include <SetupVariable.h>
#include <Guid/TcoWdtHob.h>
#include <Library/PcdLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA_PROTOCOL              *mGlobalNvsArea;
GLOBAL_REMOVE_IF_UNREFERENCED SETUP_DATA                                mSystemConfiguration;
GLOBAL_REMOVE_IF_UNREFERENCED PCH_SETUP                                 mPchSetup;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                                   mSkipLpiFromPublish = FALSE;

/**
  Install Debug Port ACPI Table

  @param[in] OsDebugPort      Parameter to tell which debug port is used by the OS
**/
VOID
InstallDebugPortAcpiTable (
  IN  UINT8                                 OsDebugPort
  )
{
  BOOLEAN                                       HasDebugPort;
  UINTN                                         AcpiTableKey;
  EFI_ACPI_TABLE_PROTOCOL                       *AcpiTable;
  EFI_ACPI_DEBUG_PORT_DESCRIPTION_TABLE         *DebugPortTable;
  EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE       *DebugPortTable2;
  EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT *Dbg2DeviceTablePtr;
  EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE        *Dbg2DeviceGasPtr;
  UINT32                                        *Dbg2DeviceAdrSizePtr;
  UINT8                                         *Dbg2DeviceNameSpacePtr;
  CHAR8                                         *Dbg2DeviceNameSpaceStr;
  UINT16                                        Dbg2DeviceNameSpaceStrSize;
  UINT32                                        DebugPortTable2Size;
  UINT32                                        Dbg2DevNum;
  UINT16                                        DebugDevInfoStructSize;
  EFI_STATUS                                    Status;

  DebugPortTable    = NULL;
  AcpiTableKey      = 0;

  //
  // Locate ACPI support protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate Memory for Debug Port Table
  //
  DebugPortTable = AllocatePool (sizeof(EFI_ACPI_DEBUG_PORT_DESCRIPTION_TABLE));
  ASSERT (DebugPortTable != NULL);

  if (DebugPortTable == NULL) return;

  DebugPortTable->Header.Signature = EFI_ACPI_3_0_DEBUG_PORT_TABLE_SIGNATURE;
  DebugPortTable->Header.Length = sizeof(EFI_ACPI_DEBUG_PORT_DESCRIPTION_TABLE);
  DebugPortTable->Header.Revision = EFI_ACPI_DEBUG_PORT_TABLE_REVISION;
  DebugPortTable->Header.Checksum = 0x00;
  CopyMem (DebugPortTable->Header.OemId, EFI_ACPI_OEM_ID, 6);
  DebugPortTable->Header.OemTableId = 0x00;
  DebugPortTable->Header.OemRevision = PcdGet32 (PcdAcpiDefaultOemRevision);
  DebugPortTable->Header.CreatorId = EFI_ACPI_CREATOR_ID;
  DebugPortTable->Header.CreatorRevision = EFI_ACPI_CREATOR_REVISION;
  DebugPortTable->InterfaceType = EFI_ACPI_DBGP_INTERFACE_TYPE_FULL_16550;

  //
  // Initialize the number of device for DBG2
  //
  Dbg2DevNum = 1;

  //
  // Initialize the DBG2 debug device's name space string and size
  //
  Dbg2DeviceNameSpaceStr = ACPI_DBG2_DEFAULT_NAME_SPACE;
  if (OsDebugPort == 1) {
    Dbg2DeviceNameSpaceStr = ACPI_DBG2_SERIALIO_UART0_NAME_SPACE;
  }
  if (OsDebugPort == 2) {
    Dbg2DeviceNameSpaceStr = ACPI_DBG2_SERIALIO_UART1_NAME_SPACE;
  }
  if (OsDebugPort == 3) {
    Dbg2DeviceNameSpaceStr = ACPI_DBG2_SERIALIO_UART2_NAME_SPACE;
  }
  Dbg2DeviceNameSpaceStrSize = (UINT16) AsciiStrSize(Dbg2DeviceNameSpaceStr);

  //
  // Initialize the DBG2 total size
  //
  DebugDevInfoStructSize = (UINT16)((sizeof(EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) +        // Debug Device Information Structure
                                     sizeof(EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE) +               // Base Address Register - Assume 1 BAR
                                     4 +                                                            // Address Size
                                     Dbg2DeviceNameSpaceStrSize +                                   // Name Space String
                                     0) *                                                           // Oem Data - Assume none
                                    Dbg2DevNum);
  DebugPortTable2Size = sizeof(EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE) + DebugDevInfoStructSize;   // DBG2 table size

  //
  // Allocate memory for DBG2
  //
  DebugPortTable2 = AllocatePool (DebugPortTable2Size);
  ASSERT (DebugPortTable2 != NULL);

  if (DebugPortTable2 == NULL) return;

  //
  // Populate data for DBG2
  //
  DebugPortTable2->Header.Signature       = EFI_ACPI_5_0_DEBUG_PORT_2_TABLE_SIGNATURE;
  DebugPortTable2->Header.Length          = DebugPortTable2Size;
  DebugPortTable2->Header.Revision        = EFI_ACPI_DEBUG_PORT_2_TABLE_REVISION;
  DebugPortTable2->Header.Checksum        = 0x00;
  CopyMem (DebugPortTable2->Header.OemId, EFI_ACPI_OEM_ID, 6);
  DebugPortTable2->Header.OemTableId      = 0x00;
  DebugPortTable2->Header.OemRevision     = PcdGet32 (PcdAcpiDefaultOemRevision);
  DebugPortTable2->Header.CreatorId       = EFI_ACPI_CREATOR_ID;
  DebugPortTable2->Header.CreatorRevision = EFI_ACPI_CREATOR_REVISION;
  DebugPortTable2->OffsetDbgDeviceInfo    = sizeof(EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE);
  DebugPortTable2->NumberDbgDeviceInfo    = Dbg2DevNum;

  //
  // Set pointer to DBG2 Device Info Structure Offset
  //
  Dbg2DeviceTablePtr = (EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT *)((UINT8 *)DebugPortTable2 + DebugPortTable2->OffsetDbgDeviceInfo);

  //
  // Populate DBG2 Device Info Structure
  //
  Dbg2DeviceTablePtr[0].Revision                        = 0x00;
  Dbg2DeviceTablePtr[0].Length                          = DebugDevInfoStructSize;
  Dbg2DeviceTablePtr[0].NumberofGenericAddressRegisters = 1;
  Dbg2DeviceTablePtr[0].NameSpaceStringLength           = Dbg2DeviceNameSpaceStrSize;
  Dbg2DeviceTablePtr[0].NameSpaceStringOffset           = sizeof(EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) + sizeof(EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE) + 4;
  Dbg2DeviceTablePtr[0].OemDataLength                   = 0;
  Dbg2DeviceTablePtr[0].OemDataOffset                   = 0;
  Dbg2DeviceTablePtr[0].PortType                        = EFI_ACPI_DBG2_PORT_TYPE_SERIAL;
  Dbg2DeviceTablePtr[0].PortSubtype                     = EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_FULL_16550;
  Dbg2DeviceTablePtr[0].Reserved[0]                     = 0;
  Dbg2DeviceTablePtr[0].Reserved[1]                     = 0;
  Dbg2DeviceTablePtr[0].BaseAddressRegisterOffset       = sizeof(EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT);
  Dbg2DeviceTablePtr[0].AddressSizeOffset               = sizeof(EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) + sizeof(EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE);

  //
  // Set pointer to DBG2 Device Base Address Offset
  //
  Dbg2DeviceGasPtr = (EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE *)((UINT8 *)Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].BaseAddressRegisterOffset);

  //
  // Set pointer to DBG2 Device Address Size Offset and populate value
  //
  Dbg2DeviceAdrSizePtr = (UINT32 *)((UINT8 *)Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].AddressSizeOffset);
// AMI_OVERRIDE_START - BQA Ubuntu Firmware Test Live dbg2: DBG2 (Debug Port Table 2) test
#if 0
  Dbg2DeviceAdrSizePtr[0] = 32;
#else
  Dbg2DeviceAdrSizePtr[0] = sizeof(EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE);
#endif 
// AMI_OVERRIDE_END - BQA Ubuntu Firmware Test Live dbg2: DBG2 (Debug Port Table 2) test

  //
  // Set pointer to DBG2 Device Name Space Offset and populate value
  //
  Dbg2DeviceNameSpacePtr = (UINT8 *)((UINT8 *)Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].NameSpaceStringOffset);
  CopyMem (Dbg2DeviceNameSpacePtr, Dbg2DeviceNameSpaceStr, Dbg2DeviceNameSpaceStrSize);

  //
  // Check which port is used as Debug Port based on policy
  // case 0 : Legacy UART
  // case 1 : Serial IO UART 0
  // case 2 : Serial IO UART 1
  // case 3 : Serial IO UART 2
  //
  HasDebugPort      = FALSE;
  switch (OsDebugPort) {
    case 0:
      DebugPortTable->BaseAddress.AddressSpaceId = EFI_ACPI_2_0_SYSTEM_IO;
      DebugPortTable->BaseAddress.RegisterBitWidth = 8;
      DebugPortTable->BaseAddress.RegisterBitOffset = 0;
      DebugPortTable->BaseAddress.Reserved = 0;
      DebugPortTable->BaseAddress.Address = PcdGet64 (PcdSerialRegisterBase);
      //
      // Update DBG2 Device Base Address
      //
      Dbg2DeviceGasPtr[0].AddressSpaceId = EFI_ACPI_5_0_SYSTEM_IO;
      Dbg2DeviceGasPtr[0].RegisterBitWidth = 8;
      Dbg2DeviceGasPtr[0].RegisterBitOffset = 0;
      Dbg2DeviceGasPtr[0].AccessSize = EFI_ACPI_5_0_UNDEFINED;
      Dbg2DeviceGasPtr[0].Address = PcdGet64 (PcdSerialRegisterBase);
      HasDebugPort = TRUE;
      break;
    case 1:
    case 2:
    case 3:
      DebugPortTable->BaseAddress.AddressSpaceId = EFI_ACPI_2_0_SYSTEM_MEMORY;
      DebugPortTable->BaseAddress.RegisterBitWidth = 8;
      DebugPortTable->BaseAddress.RegisterBitOffset = 0;
      DebugPortTable->BaseAddress.Reserved = 0;
      DebugPortTable->BaseAddress.Address  = FindSerialIoBar(OsDebugPort-1+PchSerialIoIndexUart0, 0);
      //
      // Update DBG2 Device Base Address
      //
      Dbg2DeviceGasPtr[0].AddressSpaceId = EFI_ACPI_5_0_SYSTEM_MEMORY;
      Dbg2DeviceGasPtr[0].RegisterBitWidth = 8;
      Dbg2DeviceGasPtr[0].RegisterBitOffset = 0;
      Dbg2DeviceGasPtr[0].AccessSize = EFI_ACPI_5_0_UNDEFINED;
      Dbg2DeviceGasPtr[0].Address = FindSerialIoBar(OsDebugPort-1+PchSerialIoIndexUart0, 0);
      HasDebugPort = TRUE;
      break;
    default:
      ASSERT (FALSE);
      break;
  }

  if (HasDebugPort) {
    Status = AcpiTable->InstallAcpiTable (AcpiTable, DebugPortTable, DebugPortTable->Header.Length, &AcpiTableKey);
    ASSERT_EFI_ERROR (Status);
    //
    // Publish DBG2
    //
// AMI_OVERRIDE_START - Fix DebugPortTable would be override issue
      AcpiTableKey = 0;
// AMI_OVERRIDE_END - Fix DebugPortTable would be override issue   
    Status = AcpiTable->InstallAcpiTable (AcpiTable, DebugPortTable2, DebugPortTable2->Header.Length, &AcpiTableKey);
    ASSERT_EFI_ERROR (Status);
  }

  FreePool (DebugPortTable);
  FreePool (DebugPortTable2);
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
PlatformFeaturesUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN OUT EFI_ACPI_TABLE_VERSION       *Version
  )
{
  EFI_STATUS                                  Status;
  EFI_ACPI_DESCRIPTION_HEADER                 *TableHeader;
  UINT8                                       *WatchdogStatus;
  EFI_PEI_HOB_POINTERS                        Hob;
  EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY *WdatActionEntry;
  UINT8                                       Index;
  CPU_GENERATION                              CpuGeneration;
  PLATFORM_INFO                               *PlatformInfo;

  CpuGeneration = GetCpuGeneration();

  Status = gBS->LocateProtocol (
                  &gPlatformInfoProtocolGuid,
                  NULL,
                  (VOID **) &PlatformInfo
                  );

  ASSERT_EFI_ERROR (Status);

  //
  // This will be accurate except for FACS structure
  //
  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

  //
  // Update the OEM and creator information for every table except FACS.
  //
  if (Table->Signature != EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
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
      if (CpuGeneration == EnumKblCpu) {
        switch (PlatformInfo->PlatformType) {
        case TypeTrad:
          TableHeader->OemTableId = ACPI_OEM_TABLE_ID_KBL;
          break;

        case TypeUltUlx:
          if (PlatformInfo->BoardType == BoardTypeSds) {
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

        switch (PlatformInfo->PlatformType) {
        case TypeTrad:
          TableHeader->OemTableId = ACPI_OEM_TABLE_ID_SKL;
          break;

        case TypeUltUlx:
          if (PlatformInfo->BoardType == BoardTypeSds) {
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

      //
      // Update creator information
      //
      TableHeader->CreatorId        = EFI_ACPI_CREATOR_ID;
      TableHeader->CreatorRevision  = EFI_ACPI_CREATOR_REVISION;
    }
  }

  //
  // By default, a table belongs in all ACPI table versions published.
  // Some tables will override this because they have different versions of the table.
  //
  *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  //
  // Update the various table types with the necessary updates
  //
  switch (Table->Signature) {

  case EFI_ACPI_1_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:

    //
    // Do not load the PTID SSDT table if it is not enabled in the BIOS setup
    //
    if (CompareMem (&TableHeader->OemTableId, "PtidDevc", 8) == 0) {
      if (mSystemConfiguration.PtidSupport == 0) {
        *Version = EFI_ACPI_TABLE_VERSION_NONE;
      }
    }
// AMI_OVERRIDE_START - EIP236974 : ZPODD ASL code implement  
    //
    // Do not load the ZPODD SSDT table if it is not enabled in the BIOS setup
    //
    if (CompareMem (&TableHeader->OemTableId, "zpodd", 5) == 0) {
      if (mSystemConfiguration.ZpOddSupport == 0) {
        *Version = EFI_ACPI_TABLE_VERSION_NONE;
      }
    }
// AMI_OVERRIDE_END - EIP236974 : ZPODD ASL code implement  

// AMI_OVERRIDE_START - Customer request not load EInkApp if it not enabled 
    //
    // Do not load EInkApp if it not enabled, Check PcdEInkApp
    //
    if (CompareMem (&TableHeader->OemTableId, "EInkApp", 7) == 0) {
      if ( (!PcdGetBool(PcdEInkApp)) && (!mGlobalNvsArea->Area->EInkDfuEnable)) {
        *Version = EFI_ACPI_TABLE_VERSION_NONE;
      }
    }
// AMI_OVERRIDE_END - Customer request not load EInkApp if it not enabled.
// AMI_OVERRIDE_START - EIP345352 : _UPC & _PLD setting of original TbtTypeCWrapper.asl is for Intel CRB desing, customer's board design is different to CRB. They can include its ASL file for its _PLD/__UPC.
    //
    // Do not load EInkApp if it not enabled, Check PcdEInkApp
    //
    if (CompareMem (&TableHeader->OemTableId, "TbtTypeC", 8) == 0) {
        #if defined (TbtTypeC_Wrapper_ASL_SUPPORT) && (TbtTypeC_Wrapper_ASL_SUPPORT == 0) 
            *Version = EFI_ACPI_TABLE_VERSION_NONE;
        #endif
    }    
// AMI_OVERRIDE_END - EIP345352 : _UPC & _PLD setting of original TbtTypeCWrapper.asl is for Intel CRB desing, customer's board design is different to CRB. They can include its ASL file for its _PLD/__UPC.    

    break;
  case ACPI_LOW_POWER_IDLE_TABLE_SIGNATURE:
    {
      UINT8                                  LpitStateEntries = 0;
      UINT32                                 PchPwrmBase;
      EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE SetResidencyCounter[2] = { ACPI_LPI_RES_SLP_S0_COUNTER, ACPI_LPI_RES_C10_COUNTER };
      UINT64                                 ResidencyCounterFrequency = 0;
      UINT8                                  LpitResidencyCounter = 0; // 0 - SLP_S0 Based , 1 - C10 Counter

      LpitStateEntries = (UINT8)(((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(EFI_ACPI_MWAIT_LPI_STATE_DESCRIPTOR);
      if(LpitStateEntries != 0) {
        //
        // 1. Publish Dual LPI entry in ACPI table when Windows RS2 LPIT WA is needed
        // 2. Publish Single LPI entry in ACPI table when Windows RS2 LPIT WA is not needed (windows RS3)
        //
        if (mSystemConfiguration.LpitRs2Wa == TRUE && LpitStateEntries == 2) {
          mSkipLpiFromPublish = FALSE;
          LpitResidencyCounter = mSystemConfiguration.LpitResidencyCounter;
          //
          // LpitResidencyCounter : 0 - SLP_S0 Based , 1 - C10 Counter ,
          //
          if (LpitResidencyCounter == 0) {
            //
            // Read PWRM Base Address to fill in Residency counter Address Space
            //
            PchPwrmBaseGet(&PchPwrmBase);
            SetResidencyCounter[LpitResidencyCounter].Address = (UINT64)PchPwrmBase + R_PCH_PWRM_SLP_S0_RESIDENCY_COUNTER;
            ResidencyCounterFrequency = 9580; //Counter runs at 104us granularity which implies 9.58KHz frequency (9580Hz)
          } else {
            SetResidencyCounter[LpitResidencyCounter].Address = C10_RESIDENCY_COUNTER_MSR;
            ResidencyCounterFrequency = 0; //Counter runs at TSC frequency
          }
        } else if (mSystemConfiguration.LpitRs2Wa == FALSE && LpitStateEntries == 1) {
          mSkipLpiFromPublish = FALSE;
          //
          // LpitResidencyCounter : 0 - Always SLP_S0 Based Counter for RS3 Windows Onwards
          //
          PchPwrmBaseGet(&PchPwrmBase);
          SetResidencyCounter[LpitResidencyCounter].Address = (UINT64)PchPwrmBase + R_PCH_PWRM_SLP_S0_RESIDENCY_COUNTER;
          ResidencyCounterFrequency = 9580; //Counter runs at 104us granularity which implies 9.58KHz frequency (9580Hz)
        } else {
          mSkipLpiFromPublish = TRUE;
        }

        (((ACPI_LOW_POWER_IDLE_TABLE *)Table)->LpiStates[LpitStateEntries - 1].ResidencyCounter) = SetResidencyCounter[LpitResidencyCounter];
        (((ACPI_LOW_POWER_IDLE_TABLE *)Table)->LpiStates[LpitStateEntries - 1].ResidencyCounterFrequency) = ResidencyCounterFrequency;
      }
    }
    break;
  case ACPI_WSMT_SIGNATURE:
    {
       (((ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_TABLE *)Table)->ProtectionFlags.Flags) = (UINT32 ) (WSMT_PROTECTION_FLAG & PcdGet32(PcdWsmtProtectionFlags));
    }
    break;
  case EFI_ACPI_3_0_WATCHDOG_ACTION_TABLE_SIGNATURE:
    //
    // If TCO timer is disabled in setup, don't publish the table.
    //
    if (mPchSetup.EnableTcoTimer == FALSE) {
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
      break;
    }

    //
    // WDAT table exist, clear NO REBOOT bit
    //
    // @todo SKL PCH: Update with relevant programming
    /*
    GcsRegister = MmioRead32 (RootComplexBar + R_PCH_RCRB_GCS);
    GcsRegister &= (UINT32)~B_PCH_RCRB_GCS_NR;
    MmioWrite32 (RootComplexBar + R_PCH_RCRB_GCS, GcsRegister);
    */

    //
    // Allocate one byte of EfiRuntimeServicesData, use it to store TCO reboot status
    // so that OS can get it through EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_WATCHDOG_STATUS
    //
    WatchdogStatus = NULL;
    Status = (gBS->AllocatePool) (EfiRuntimeServicesData, 0x01, (VOID **) &WatchdogStatus);
    ASSERT_EFI_ERROR (Status);
    *WatchdogStatus = 0;

    //
    // Read the TCO reboot status from HOB
    //
    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &Hob.Raw);
    if (EFI_ERROR (Status)) {
      break; // Not able to get HobList.
    }
    for (; !END_OF_HOB_LIST(Hob); Hob.Raw = GET_NEXT_HOB(Hob)) {
      if ((GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_GUID_EXTENSION) && (CompareGuid(&Hob.Guid->Name, &gTcoWdtHobGuid))) {
        *WatchdogStatus = ((TCO_WDT_HOB *)Hob.Guid)->TcoRebootHappened;
      }
    }

    WdatActionEntry = (EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY *)((UINT8 *) Table + sizeof(EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE));
    for (Index = 0; Index < ((EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE *) Table)->NumberWatchdogInstructionEntries; Index ++) {
      if (WdatActionEntry[Index].WatchdogAction == EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_WATCHDOG_STATUS) {
        WdatActionEntry[Index].RegisterRegion.AddressSpaceId = EFI_ACPI_2_0_SYSTEM_MEMORY;
        WdatActionEntry[Index].RegisterRegion.RegisterBitWidth = 0x01;
        WdatActionEntry[Index].RegisterRegion.RegisterBitOffset = 0x00;
        WdatActionEntry[Index].RegisterRegion.Reserved = 0x01;
        WdatActionEntry[Index].RegisterRegion.Address = (UINT64)WatchdogStatus;
        WdatActionEntry[Index].Value = 1;
        WdatActionEntry[Index].Mask = 1;
      }
      if (WdatActionEntry[Index].WatchdogAction == EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_WATCHDOG_STATUS) {
        WdatActionEntry[Index].RegisterRegion.AddressSpaceId = EFI_ACPI_2_0_SYSTEM_MEMORY;
        WdatActionEntry[Index].RegisterRegion.RegisterBitWidth = 0x01;
        WdatActionEntry[Index].RegisterRegion.RegisterBitOffset = 0x00;
        WdatActionEntry[Index].RegisterRegion.Reserved = 0x01;
        WdatActionEntry[Index].RegisterRegion.Address = (UINT64)WatchdogStatus;
        WdatActionEntry[Index].Value = 0;
        WdatActionEntry[Index].Mask = 1;
      }
    }
    break;

  default:
    break;
  }
  return EFI_SUCCESS;
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
      PlatformFeaturesUpdateTables (CurrentTable, &Version);
      //
      // 1. Skip to Publish Single LPI entry in ACPI table when Windows RS2 LPIT WA is needed
      // 2. Skip to Publish Dual LPI entry in ACPI table when Windows RS2 LPIT WA is not needed (windows RS3)
      //
      if (mSkipLpiFromPublish) {
        mSkipLpiFromPublish = FALSE; 
      } else {
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
      }
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
  ACPI Features driver installation function.

  @param[in] ImageHandle     Handle for this drivers loaded image protocol.
  @param[in] SystemTable     EFI system table.

  @retval EFI_SUCCESS        The driver installed without error.
  @retval EFI_ABORTED        The driver encountered an error and could not complete installation of
                             the ACPI tables.

**/
EFI_STATUS
EFIAPI
InstallAcpiPlatformFeatures (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                    Status;
  UINTN                         VarDataSize;

  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &mGlobalNvsArea
                  );
  ASSERT_EFI_ERROR (Status);

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

  VarDataSize = sizeof (PCH_SETUP);
  Status = gRT->GetVariable (
                  L"PchSetup",
                  &gPchSetupVariableGuid,
                  NULL,
                  &VarDataSize,
                  &mPchSetup
                  );

  //
  // Platform Features ACPI Tables
  //
  PublishAcpiTablesFromFv(gEfiCallerIdGuid);

  //
  // Placeholder for SKL CRB code
  // Install DBGP Table
  //
  InstallDebugPortAcpiTable (mPchSetup.OsDebugPort);

  if (mSystemConfiguration.IrmtConfiguration) {
    //
    // Call Irmt Dxe
    //
    IrmtDxeEntryPoint(ImageHandle, SystemTable);
    //
    // Initialize the Irmt configuration in ACPI GNVS
    //
#if 0  // AMI_OVERRIDE_START - Use setup item to enable/disable Irmt table.Change iRMT asl code.
    mGlobalNvsArea->Area->IrmtCfg = BIT0;  // Irmt Enabled
#else
    mGlobalNvsArea->Area->IrmtCfg = (mSystemConfiguration.IrmtHwNotification << 1) | (mSystemConfiguration.IrmtHwNotification);  // Irmt Enabled
    DEBUG((DEBUG_ERROR, "mGlobalNvsArea.Area->IrmtCfg: 0x%x\n", mGlobalNvsArea->Area->IrmtCfg));
#endif // AMI_OVERRIDE_END - Use setup item to enable/disable Irmt table.Change iRMT asl code.
  }

  //
  // Finished
  //
  return EFI_SUCCESS;
}
