/** @file
  This file contains Processor Power Management ACPI related functions for
  processors.

  <b>Acronyms:</b>
     - PPM: Processor Power Management
     - TM:  Thermal Monitor
     - IST: Intel(R) Speedstep technology
     - HT:  Hyper-Threading Technology

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2012 - 2019 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/

#include "PowerMgmtInit.h"


/**
  This will perform Miscellaneous Power Management related programming.
**/
VOID
InitMiscFeatures (
  VOID
  )
{
  MSR_REGISTER TempMsr;
  UINT8        CpuConfigTdpLevels;

  ///
  /// Get the number of configurable TDP Levels supported.
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  TempMsr.Qword &= V_CONFIG_TDP_NUM_LEVELS_MASK;
  CpuConfigTdpLevels = (UINT8) RShiftU64 (TempMsr.Qword, N_MSR_PLATFORM_INFO_CONFIG_TDP_NUM_LEVELS_OFFSET);

  ///
  /// Configure Package Turbo Power Limits if ConfigTDP is supported.
  ///
  if ((CpuConfigTdpLevels != 0) && (gCpuGlobalNvsAreaProtocol != NULL)) {
    CtdpS3Save ();
  }
}

/**
  Update the SSDT table pointers and config DWORD CFGD with the PpmFlags current configuration value
**/
VOID
PatchCpuSsdtTable (
  VOID
  )
{
  UINT8       *CurrPtr;
  UINT32      *Signature;
  SSDT_LAYOUT *SsdtPackage;

  ///
  /// Locate the SSDT package
  ///
  SsdtPackage = NULL;
  CurrPtr     = (UINT8 *) mCpuSsdtTable;
  for (CurrPtr = (UINT8 *) mCpuSsdtTable; CurrPtr <= ((UINT8 *) mCpuSsdtTable + mCpuSsdtTable->Length); CurrPtr++) {
    Signature = (UINT32 *) (CurrPtr + 1);
    if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('S', 'S', 'D', 'T')) {
      ///
      /// Update the SSDT table pointers for dynamically loaded tables
      ///
      SsdtPackage = (SSDT_LAYOUT *) CurrPtr;
      ///
      /// Set the P-State SSDT table information
      ///
      SsdtPackage->Cpu0IstAddr  = (UINT32) (UINTN) mCpu0IstTable;
      SsdtPackage->Cpu0IstLen   = mCpu0IstTable->Length;
      SsdtPackage->ApIstAddr    = (UINT32) (UINTN) mApIstTable;
      SsdtPackage->ApIstLen     = mApIstTable->Length;
      ///
      /// Set the C-State SSDT table information
      ///
      SsdtPackage->Cpu0CstAddr  = (UINT32) (UINTN) mCpu0CstTable;
      SsdtPackage->Cpu0CstLen   = mCpu0CstTable->Length;
      SsdtPackage->ApCstAddr    = (UINT32) (UINTN) mApCstTable;
      SsdtPackage->ApCstLen     = mApCstTable->Length;
      ///
      /// Save Cpu0Hwp and Ap0Hwp address to load SSDT Dynamically in ASL
      ///
      if (mCpu0HwpTable != NULL) {
        SsdtPackage->Cpu0HwpAddr  = (UINT32) (UINTN) mCpu0HwpTable;
        SsdtPackage->Cpu0HwpLen   = mCpu0HwpTable->Length;
        if (mApHwpTable != NULL) {
          SsdtPackage->ApHwpAddr  = (UINT32) (UINTN) mApHwpTable;
          SsdtPackage->ApHwpLen   = mApHwpTable->Length;
        }
        if (mHwpLvtTable != NULL) {
          SsdtPackage->HwpLvtAddr = (UINT32) (UINTN) mHwpLvtTable;
          SsdtPackage->HwpLvtLen  = mHwpLvtTable->Length;
        }
      }
      DEBUG ((DEBUG_INFO, "\n Cpu0Ist       :%x   :%x \n",SsdtPackage->Cpu0IstAddr,SsdtPackage->Cpu0IstLen));
      DEBUG ((DEBUG_INFO, "\n Cpu0Cst       :%x   :%x \n",SsdtPackage->Cpu0CstAddr,SsdtPackage->Cpu0CstLen));
      DEBUG ((DEBUG_INFO, "\n Cpu0Hwp       :%x   :%x \n",SsdtPackage->Cpu0HwpAddr,SsdtPackage->Cpu0HwpLen));
      DEBUG ((DEBUG_INFO, "\n ApHwp         :%x   :%x \n",SsdtPackage->ApHwpAddr,SsdtPackage->ApHwpLen));
      DEBUG ((DEBUG_INFO, "\n HwpLvt        :%x   :%x \n",SsdtPackage->HwpLvtAddr,SsdtPackage->HwpLvtLen));
    }
    ///
    /// Update the CPU GlobalNvs area
    ///
    if ((*CurrPtr == AML_EXT_REGION_OP) && *Signature == SIGNATURE_32 ('P', 'N', 'V', 'S')) {
      ASSERT (*(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) == 0xFFFF0000);
      ASSERT (*(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA55);
      ///
      /// CPU Global NVS Area address
      ///
      *(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) = (UINT32) (UINTN) gCpuGlobalNvsAreaProtocol->Area;
      ///
      /// CPU Global NVS Area size
      ///
      *(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) = sizeof (CPU_GLOBAL_NVS_AREA);
      break;
    }
  }
  //
  // Assert if we didn't update the SSDT table
  //
  ASSERT (SsdtPackage != NULL);

  return;
}

/**
  Locate the PPM ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

  @retval EFI_SUCCESS - On success
  @retval EFI_NOT_FOUND - Required firmware volume not found
  @retval EFI_OUT_OF_RESOURCES - No enough resoruces (such as out of memory).
  @retval - Appropiate failure code on error
**/
EFI_STATUS
InitCpuAcpiTable (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   *HandleBuffer;
  UINTN                        NumberOfHandles;
  EFI_FV_FILETYPE              FileType;
  UINT32                       FvStatus;
  EFI_FV_FILE_ATTRIBUTES       Attributes;
  UINTN                        Size;
  UINTN                        i;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  INTN                         Instance;
  EFI_ACPI_TABLE_VERSION       Version;
  EFI_ACPI_COMMON_HEADER       *CurrentTable;
  EFI_ACPI_DESCRIPTION_HEADER  *TempTable;
  UINTN                        AcpiTableHandle;
  FwVol = NULL;

  ///
  /// Locate Firmware volume protocol.
  /// There is little chance we can't find an FV protocol
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Look for FV with ACPI storage file
  ///
  FwVol = NULL;
  for (i = 0; i < NumberOfHandles; i++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);
    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gCpuAcpiTableStorageGuid,
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
      break;
    }
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);
  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol != NULL);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }
  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
  ///
  /// Read tables from the storage file.
  ///
  Instance      = 0;
  CurrentTable  = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gCpuAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      ///
      /// Check the table ID to modify the table
      ///
      switch (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId) {
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'I', 's', 't', 0)) :
          mCpu0IstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EIST) {
            ///
            /// Patch the native _PSS package with the EIST values
            ///
            Status = AcpiPatchPss ();
            if (EFI_ERROR (Status)) {
              return Status;
            }
          }
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'C', 's', 't', 0)) :
          mCpu0CstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'T', 's', 't', 0)) :
          mCpu0TstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'I', 's', 't', 0, 0, 0)) :
          mApIstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'C', 's', 't', 0, 0, 0)) :
          mApCstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'T', 's', 't', 0, 0, 0)) :
          mApTstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', 'S', 's', 'd', 't', 0)) :
          mCpuSsdtTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 't', 'd', 'p', 'B', 0, 0, 0)) :
          mCtdpTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'H', 'w', 'p', 0)) :
          mCpu0HwpTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'H', 'w', 'p', 0, 0, 0)) :
          mApHwpTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('H', 'w', 'p', 'L', 'v', 't', 0, 0)) :
          mHwpLvtTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;

        default:
          break;
      }
      Instance++; // Increment the instance
      CurrentTable = NULL;
    }
  }
  ///
  /// Copy CpuoIst SSDT if EIST is enabled
  ///
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EIST) {
    TempTable = AllocateReservedPool (mCpu0IstTable->Length);
    if (TempTable == NULL) {
      ASSERT (TempTable != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mCpu0IstTable, mCpu0IstTable->Length);
    FreePool (mCpu0IstTable);
    mCpu0IstTable = TempTable;
    AcpiChecksum (mCpu0IstTable, mCpu0IstTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
  }
  ///
  /// Copy Cpu0Hwp & HwpLvt SSDT to reserved location and checksum them
  ///
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_HWP) {
    TempTable = AllocateReservedPool (mCpu0HwpTable->Length);
    if (TempTable == NULL) {
      ASSERT (FALSE);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mCpu0HwpTable, mCpu0HwpTable->Length);
    FreePool (mCpu0HwpTable);
    mCpu0HwpTable = TempTable;
    AcpiChecksum (mCpu0HwpTable, mCpu0HwpTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_HWP_LVT) {
      TempTable = AllocateReservedPool (mHwpLvtTable->Length);
      if (TempTable == NULL) {
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (TempTable, mHwpLvtTable, mHwpLvtTable->Length);
      FreePool (mHwpLvtTable);
      mHwpLvtTable = TempTable;
      AcpiChecksum (mHwpLvtTable, mHwpLvtTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    }
  }

  ///
  /// If we are CMP, then the PPM tables are dynamically loaded:
  ///   We need to publish the CpuPm table to the ACPI tables, and move the CST
  ///   tables that are dynamically loaded to a separate location so that we can fix the
  ///   addresses in the CpuPm table.
  /// Otherwise (non-CMP):
  ///   We need to publish CPU 0 tables only, and CST tables only if CST is enabled
  ///
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_CMP) {
    //
    // Copy tables to our own location and checksum them
    //
    TempTable = AllocateReservedPool (mApIstTable->Length);
    if (TempTable == NULL) {
      ASSERT (TempTable != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mApIstTable, mApIstTable->Length);
    FreePool (mApIstTable);
    mApIstTable = TempTable;
    AcpiChecksum (mApIstTable, mApIstTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    TempTable = AllocateReservedPool (mCpu0CstTable->Length);
    if (TempTable == NULL) {
      ASSERT (TempTable != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mCpu0CstTable, mCpu0CstTable->Length);
    FreePool (mCpu0CstTable);
    mCpu0CstTable = TempTable;
    AcpiChecksum (mCpu0CstTable, mCpu0CstTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    TempTable = AllocateReservedPool (mApCstTable->Length);
    if (TempTable == NULL) {
      ASSERT (TempTable != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mApCstTable, mApCstTable->Length);
    FreePool (mApCstTable);
    mApCstTable = TempTable;
    AcpiChecksum (mApCstTable, mApCstTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    ///
    /// Copy Ap0Hwp SSDT to reserved location and checksum them
    ///
    if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_HWP) {
      TempTable = AllocateReservedPool (mApHwpTable->Length);
      if (TempTable == NULL) {
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (TempTable, mApHwpTable, mApHwpTable->Length);
      FreePool (mApHwpTable);
      mApHwpTable = TempTable;
      AcpiChecksum (mApHwpTable, mApHwpTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    }
  } else {
    //
    // CMP disabled, so statically load the tables
    //
    // Add CST SSDT if C states are enabled
    //
    if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C_STATES) {
      AcpiTableHandle = 0;
      Status = mAcpiTable->InstallAcpiTable (
                             mAcpiTable,
                             mCpu0CstTable,
                             mCpu0CstTable->Length,
                             &AcpiTableHandle
                             );
    }
    ///
    /// Since we are UP, there is no need for the CPU 1 tables
    ///
    ///
    /// Free all tables, since they have been copied into ACPI tables by ACPI support protocol
    ///
    FreePool (mCpu0CstTable);
    FreePool (mApIstTable);
    FreePool (mApCstTable);
  }
  ///
  /// Update the CpuSsdt SSDT table in the ACPI tables.
  ///
  PatchCpuSsdtTable ();
  AcpiTableHandle = 0;
  Status = mAcpiTable->InstallAcpiTable (
                         mAcpiTable,
                         mCpuSsdtTable,
                         mCpuSsdtTable->Length,
                         &AcpiTableHandle
                         );
  FreePool (mCpuSsdtTable);
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TSTATES) {
    ///
    /// Load the Cpu0Tst SSDT table in the ACPI tables
    ///
    AcpiTableHandle = 0;
    Status = mAcpiTable->InstallAcpiTable (
                           mAcpiTable,
                           mCpu0TstTable,
                           mCpu0TstTable->Length,
                           &AcpiTableHandle
                           );
    FreePool (mCpu0TstTable);
    ///
    /// If the CMP is enabled then load the ApTst SSDT table in the ACPI tables
    ///
    if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_CMP) {
      AcpiTableHandle = 0;
      Status = mAcpiTable->InstallAcpiTable (
                             mAcpiTable,
                             mApTstTable,
                             mApTstTable->Length,
                             &AcpiTableHandle
                             );
    }
  }
  FreePool (mApTstTable);
  ///
  /// Load Ctdp SSDT
  ///
  if (mCpuConfigData->ConfigTdpBios == 1) {
    AcpiTableHandle = 0;
    Status = mAcpiTable->InstallAcpiTable (
                           mAcpiTable,
                           mCtdpTable,
                           mCtdpTable->Length,
                           &AcpiTableHandle
                           );
    FreePool (mCtdpTable);
  }

  return Status;
}

//
// Update ACPI IdleStates tables
//

/**
  Configure the FACP for C state support
**/
VOID
ConfigureFadtCStates (
  VOID
  )
{
  EFI_STATUS                                Status;
  EFI_ACPI_DESCRIPTION_HEADER               *Table;
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *FadtPointer;
  UINTN                                     Handle;

  ///
  /// Locate table with matching ID
  ///
  InitializeAslUpdateLib ();

  Handle = 0;
  Status = LocateAcpiTableBySignature (
             EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
             (EFI_ACPI_DESCRIPTION_HEADER **) &Table,
             &Handle
             );

  //
  // Can't have ACPI without FADT, so safe to assert
  //
  if ((EFI_ERROR (Status)) ||
      (Table == NULL) ||
      (Table->Signature != EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE)
      ) {
    ASSERT (FALSE);
    return;
  }
  FadtPointer = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *) Table;

  //
  // Verify expected state.  Should be initialized to off during build.
  //
  ASSERT (FadtPointer->PLvl3Lat >= FADT_C3_LATENCY_DISABLED);

  ///
  /// Configure C states
  ///
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C3) {
    ///
    /// Enable C3 in FADT.
    ///
    FadtPointer->PLvl3Lat = FADT_C3_LATENCY;
  }

  ///
  /// Update the table
  ///
// AMI_OVERRIDE_START >>> EIP433988 - We have to uninstall the acpi table before installing modified it. 
  Status = mAcpiTable->UninstallAcpiTable(
                         mAcpiTable,
                         Handle
                         );
  Handle = 0;
// AMI_OVERRIDE_END <<< EIP433988 - We have to uninstall the acpi table before installing modified it.
  Status = mAcpiTable->InstallAcpiTable (
                         mAcpiTable,
                         Table,
                         Table->Length,
                         &Handle
                         );
  FreePool (Table);

  return;
}
