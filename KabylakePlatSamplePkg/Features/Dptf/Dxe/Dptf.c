/** @file
  This DXE driver configures and supports Dptf.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2018 Intel Corporation.

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

#include "Dptf.h"

GLOBAL_REMOVE_IF_UNREFERENCED SETUP_DATA              mSetupData;
//
// KblGBegin
//
typedef enum {
  NotKblG = 0,
  XLSku,
  XTSku,
  RadeonProSku,
} KBLG_SKU;

UINT32
FindDgpuDeviceId (
  VOID
  ) 
{
  EFI_HANDLE                *HandleBuffer;
  UINTN                     HandleCount;
  UINTN                     Index;
  EFI_STATUS                Status;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  PCI_DEVICE_INDEPENDENT_REGION  PciHeader;

  ///
  /// Get all PCI IO protocols
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    return 0;
  }
    
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
                      
      if (EFI_ERROR (Status)) {
        continue;
      }               


    PciIo->Pci.Read (
             PciIo,
             EfiPciIoWidthUint32,
             0,
             (sizeof (PciHeader.VendorId) + sizeof (PciHeader.DeviceId)) / sizeof (UINT32),
             &PciHeader
             );
    DEBUG((DEBUG_INFO, "PciHeader.DeviceId = %x\n", PciHeader.DeviceId));
    
    if ((PciHeader.VendorId == 0x1002) && (PciHeader.DeviceId == 0x694E)) {
      return XLSku;   
    }
    if ((PciHeader.VendorId == 0x1002) && (PciHeader.DeviceId == 0x694F)) {
      return RadeonProSku;   
    }
    if ((PciHeader.VendorId == 0x1002) && (PciHeader.DeviceId == 0x694C)) {
      return XTSku;
    }
  }
    return 0;
}
//
// KblGEnd
//
EFI_STATUS
EFIAPI
InitializeDptf (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
/**
@brief
  This procedure does all the DPTF initialization and loads the ACPI tables.

  @param[in] ImageHandle   - The firmware allocated handle to the Driver Image
  @param[in] SystemTable   - Pointer to the EFI System Table

  @retval EFI_SUCCESS - The driver installed/initialized correctly.
**/
{
  EFI_STATUS         Status;
  UINTN              VariableSize;
//
// KblGBegin
//
  MSR_REGISTER       TempMsr;
  KBLG_SKU           KblGSku;
//
// KblGEnd
//

  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &mSetupData
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Check if Dptf is enabled and load the ACPI SSDT.
  ///
  if (mSetupData.EnableDptf == 1) {
    ///
    /// Load the SSDT ACPI Tables.
    ///
    if (PcdGetBool (PcdSdsDptfTablePresent)) {
        DEBUG ((DEBUG_INFO, "DPTF: LoadAcpiTablesSds\n"));
        LoadAcpiTablesSds ();
    } else {
        DEBUG ((DEBUG_INFO, "DPTF: LoadAcpiTables\n"));
        LoadAcpiTables ();
    }
//
// KblGBegin
//
    if (IsKblGPackage()) {
      KblGSku = FindDgpuDeviceId(); //Determine the SKU of KBLG before changing power limit values
      ///
      /// @todo In the scenarios where SKU can be differentiated early in boot, these MSR writes must be done in Silicon RC
      /// Only XT SKU needs PLx overrides.
      /// Because for XT SKU of KBLG, DPTF's power share policy needs the PL1 value higher for PID algorithm to work optimally
      ///
      if (KblGSku == XTSku) {   
        DEBUG ((DEBUG_INFO, "DPTF:: Found out XT Sku PL1 and PL2 values will be changed\n")); 
        TempMsr.Qword = AsmReadMsr64(MSR_PACKAGE_POWER_LIMIT);

        TempMsr.Dwords.Low &= ~POWER_LIMIT_MASK;                  /// (0x7FFF)    
        TempMsr.Dwords.Low |= (UINT32)PL1_OVERRIDE_VALUE_XT_SKU;  /// (0x1E0 for PL1 = 60W)

        TempMsr.Dwords.High &= ~POWER_LIMIT_MASK;                 /// (0x7FFF)    
        TempMsr.Dwords.High |= (UINT32)PL2_OVERRIDE_VALUE_XT_SKU; /// (0x280 for PL2 = 80W)

        DEBUG ((DEBUG_INFO,"MSR(610h)=%08X%08X\n", TempMsr.Dwords.High, TempMsr.Dwords.Low));
        AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, TempMsr.Qword);
      }
      LoadGDDVTables ();
    }
//
// KblGEnd
//
  }

  return Status;
}

EFI_STATUS
EFIAPI
LoadAcpiTables(
  VOID
  )
/**
@brief
  This procedure loads the ACPI SSDT tables.

**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  BOOLEAN                       LoadTable;
// AMI_OVERRIDE_START - When setup item "_TMP1 Object" or "_TMP2 Object" is enable, the method "TMPX" change to "_TMP".
  EFI_ACPI_DESCRIPTION_HEADER   *DptfAcpiTable;
  UINT8                         TMPCount;
  UINT8                         *CurrPtr;
  UINT8                         *EndPtr;
  UINT32                        *Signature;
  UINTN                         VariableSize = sizeof (SETUP_DATA);
// AMI_OVERRIDE_END - When setup item "_TMP1 Object" or "_TMP2 Object" is enable, the method "TMPX" change to "_TMP".
///
///

  FwVol = NULL;
  Table = NULL;

  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "DPTF: No Efi Firmware Volume Protocol available.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "DPTF: Efi Firmware Volume Protocol is loaded.\n"));
  }
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolume2ProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    if ((Status == EFI_SUCCESS) && (FwVol != NULL)) {
      ///
      /// See if it has the ACPI storage file
      ///
      Size      = 0;
      FvStatus  = 0;
      Status = FwVol->ReadFile (
                      FwVol,
                      &gDptfAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

      ///
      /// If we found it, then we are done
      ///
      if (Status == EFI_SUCCESS) {
        DEBUG ((DEBUG_INFO, "DPTF: Dptf Acpi Table Storage for RVP is found.\n"));
        break;
      }
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Find the Table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                          FwVol,
                          &gDptfAcpiTableStorageGuid,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &Table,
                          &Size,
                          &FvStatus
                          );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the table size is at least as large as an EFI_ACPI_COMMON_HEADER
      ///
      if (Size < sizeof (EFI_ACPI_COMMON_HEADER)) {
        return EFI_BUFFER_TOO_SMALL;
      }

      LoadTable = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      switch (((EFI_ACPI_DESCRIPTION_HEADER*) TableHeader)->OemTableId) {

      case SIGNATURE_64 ('D', 'p', 't', 'f', 'T', 'a', 'b', 'l'):
        ///
        /// This is Dptf SSDT. Dptf should be enabled if we reach here so load the table.
        ///
        LoadTable = TRUE;
        DEBUG ((DEBUG_INFO, "DPTF: Found Dptf SSDT signature.\n"));
// AMI_OVERRIDE_START - When setup item "_TMP1 Object" or "_TMP2 Object" is enable, the method "TMPX" change to "_TMP".      
        Status = gRT->GetVariable (
                          L"Setup",
                          &gSetupVariableGuid,
                          NULL,
                          &VariableSize,
                          &mSetupData
                          );
        ASSERT_EFI_ERROR (Status);
        if( mSetupData.Tmp1Object != 1 || mSetupData.Tmp2Object != 1){
            DptfAcpiTable = NULL;
			TMPCount = 0;
                DptfAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) TableHeader;
                CurrPtr = (UINT8 *) DptfAcpiTable;
                EndPtr  = CurrPtr + DptfAcpiTable->Length;
                for (; CurrPtr <= EndPtr; CurrPtr++) {
                     Signature = (UINT32 *) (CurrPtr + 3);
                     if (*Signature == SIGNATURE_32 ('T', 'M', 'P', 'X')) {
                	      // _TMP1 Enable
                          if( TMPCount == 0 && mSetupData.Tmp1Object == 0){     
        	               DEBUG ((DEBUG_INFO, "DPTF: Found Dptf TMPX(1) and _TMP1 is enable.\n"));
        	               *Signature  = SIGNATURE_32 ('_', 'T', 'M', 'P'); 
                          }
                          // _TMP2 Enable
                          if( TMPCount == 1 && mSetupData.Tmp2Object == 0){     
                               DEBUG ((DEBUG_INFO, "DPTF: Found Dptf TMPX(2) and _TMP2 is enable.\n"));
		                      *Signature  = SIGNATURE_32 ('_', 'T', 'M', 'P'); 
		                  }
                          TMPCount ++;
                     }
                }
        }
// AMI_OVERRIDE_END - When setup item "_TMP1 Object" or "_TMP2 Object" is enable, the method "TMPX" change to "_TMP".
        break;
		
      case SIGNATURE_64 ('G', 'd', 'd', 'v', 'D', 'u', 'm', 'm'):
//
// KblGBegin
//
        if(IsKblGPackage()){  //If this is a KBLG package, SKU specific GDDVs will be loaded by LoadGDDVTables
          DEBUG ((DEBUG_INFO, "DPTF: KBlG detected\n"));
          LoadTable = FALSE;
        }
        else {
//
// KblGEnd
//
          LoadTable = TRUE;
          DEBUG ((DEBUG_INFO, "DPTF: Loading empty data vault for documentation purposes\n"));
        }   // KblGContent
        break;

      default:
        break;
      }

      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        ///
        /// Check the length field isn't larger than the size read in section
        ///
        if (Table->Length > Size) {
          return EFI_BAD_BUFFER_SIZE;
        }
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  Table,
                                  Table->Length,
                                  &TableHandle
                                  );
      }

      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LoadAcpiTablesSds(
  VOID
  )
/**
@brief
  This procedure loads the ACPI SSDT tables for the SDS.

**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  BOOLEAN                       LoadTable;
///
///

  FwVol = NULL;
  Table = NULL;

  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "DPTF: No Efi Firmware Volume Protocol available.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "DPTF: Efi Firmware Volume Protocol is loaded.\n"));
  }
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolume2ProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    if ((Status == EFI_SUCCESS) && (FwVol != NULL)) {
      ///
      /// See if it has the ACPI storage file
      ///
      Size      = 0;
      FvStatus  = 0;
      Status = FwVol->ReadFile (
                      FwVol,
                      &gDptfAcpiTableStorageGuidSds,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

      ///
      /// If we found it, then we are done
      ///
      if (Status == EFI_SUCCESS) {
        DEBUG ((DEBUG_INFO, "DPTF: Dptf Acpi Table Storage for SDS is found.\n"));
        break;
      }
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Find the Table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                          FwVol,
                          &gDptfAcpiTableStorageGuidSds,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &Table,
                          &Size,
                          &FvStatus
                          );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the table size is at least as large as an EFI_ACPI_COMMON_HEADER
      ///
      if (Size < sizeof (EFI_ACPI_COMMON_HEADER)) {
       return EFI_BUFFER_TOO_SMALL;
      }

      LoadTable = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      switch (((EFI_ACPI_DESCRIPTION_HEADER*) TableHeader)->OemTableId) {

      case SIGNATURE_64 ('D', 'p', 't', 'f', 'S', 'd', 's','_'):
        ///
        /// This is Dptf SSDT. Dptf should be enabled if we reach here so load the table.
        ///
        LoadTable = TRUE;
        DEBUG ((DEBUG_INFO, "DPTF: Found DptfSds_ SSDT signature.\n"));
        break;

      default:
        break;
      }

      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        ///
        /// Check the length field isn't larger than the size read in section
        ///
        if (Table->Length > Size) {
          return EFI_BAD_BUFFER_SIZE;
        }
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  Table,
                                  Table->Length,
                                  &TableHandle
                                  );
      }

      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
  return EFI_SUCCESS;
}
//
// KblGBegin
//
EFI_STATUS
EFIAPI
LoadGDDVTables(
  VOID
  )
/**
@brief
  This procedure loads the GDDV SSDT tables for KBL G. based on SKU detected

**/
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        FvStatus;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  BOOLEAN                       LoadTable;
  KBLG_SKU                      KblGSku;

  FwVol = NULL;
  Table = NULL;
  KblGSku = 0;
  ///
  /// Locate FV protocol.
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "DPTF: No Efi Firmware Volume Protocol available.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "DPTF: Efi Firmware Volume Protocol is loaded.\n"));
  }
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {

    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolume2ProtocolGuid,
                  (VOID **) &FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    if ((Status == EFI_SUCCESS) && (FwVol != NULL)) {
      ///
      /// See if it has the ACPI storage file
      ///
      Size      = 0;
      FvStatus  = 0;
      Status = FwVol->ReadFile (
                      FwVol,
                      &gDptfAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

      ///
      /// If we found it, then we are done
      ///
      if (Status == EFI_SUCCESS) {
        DEBUG ((DEBUG_INFO, "DPTF: Dptf GDDV Tables for KBL G is found.\n"));
        break;
      }
    }
  }

  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Find the Table protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                          FwVol,
                          &gDptfAcpiTableStorageGuid,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &Table,
                          &Size,
                          &FvStatus
                          );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the table size is at least as large as an EFI_ACPI_COMMON_HEADER
      ///
      if (Size < sizeof (EFI_ACPI_COMMON_HEADER)) {
        return EFI_BUFFER_TOO_SMALL;
      }

      LoadTable = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
      
      KblGSku = FindDgpuDeviceId(); //Determine the SKU of KBLG before loading GDDV
      DEBUG ((DEBUG_INFO, "KblGSku = %x\n", KblGSku));
      
      switch (((EFI_ACPI_DESCRIPTION_HEADER*) TableHeader)->OemTableId) {
          
        case SIGNATURE_64('G', 'd', 'd', 'v', 'X', 'L', 0, 0):
          if (KblGSku == XLSku) {
            DEBUG ((DEBUG_INFO, "DPTF:: Found out GDDV SSDT: KBLG XL Sku\n"));
            LoadTable = TRUE;
          }
          
          if (KblGSku == RadeonProSku) {
            DEBUG ((DEBUG_INFO, "DPTF:: Found out GDDV SSDT: KBLG RadeonPro Sku\n"));
            LoadTable = TRUE;
          }
          break;
        
        case SIGNATURE_64('G', 'd', 'd', 'v', 'X', 'T', 0, 0):
          if (KblGSku == XTSku) {
            DEBUG ((DEBUG_INFO, "DPTF:: Found out SSDT: KBLG XT Sku\n"));
            LoadTable = TRUE;
          }
         break;

        default:
          break;
      }
      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        ///
        /// Check the length field isn't larger than the size read in section
        ///
        if (Table->Length > Size) {
          return EFI_BAD_BUFFER_SIZE;
        }
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  Table,
                                  Table->Length,
                                  &TableHandle
                                  );
      }

      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
  return EFI_SUCCESS;
}
//
// KblGEnd
//