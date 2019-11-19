/** @file
  The PEIM implements the SA PEI Initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2019 Intel Corporation.

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
#include <Library/PeiServicesLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MmPciLib.h>
#include <Uefi/UefiBaseType.h>
#include <PcieRegs.h>
#include <CpuRegs.h>
#include <CpuAccess.h>
#include <Ppi/SiPolicy.h>
#include <SaDataHob.h>
#include <Private/SaPegHob.h>
#include <Private/SaConfigHob.h>
#include <Library/SaPlatformLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/PchPcieRpLib.h>
#include <Library/PchInfoLib.h>
#include <Private/Library/SaInitLib.h>
#include <Private/Library/SwitchableGraphicsInitLib.h>
#include <Private/Library/GraphicsInitLib.h>
#include <Private/Library/SaPcieInitLib.h>
#include <Private/Library/SaDmiInitLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/ConfigBlockLib.h>
#include <Library/PostCodeLib.h>
#include <Private/Library/SaOcInitLib.h>


/**
  Check some SA policies are valid for debugging unexpected problem if these
  values are not initialized or assigned incorrect resource.

  @param[in] SiPreMemPolicyPpi    The SI PreMem Policy instance

**/
VOID
SaValidatePolicy (
  IN  SI_PREMEM_POLICY_PPI  *SiPreMemPolicyPpi
  )
{
  //
  // KblGBegin
  //
  UINT8                           Index;
  EFI_STATUS                      Status;
  PCIE_PEI_PREMEM_CONFIG          *PciePeiPreMemConfig;

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaPciePeiPreMemConfigGuid, (VOID *) &PciePeiPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  if (IsKblGPackage()) {
    //
    // Validate the PciExpress Configuration for G Pkg
    //
    DEBUG ((DEBUG_INFO, "Pcie dGPU validate Config for KBLG\n"));
    // Enable Peg0 and set Peg1 and Peg2 to Auto
    if (PciePeiPreMemConfig->Peg0Enable != 1) {
      DEBUG ((DEBUG_INFO, "PciePeiPreMemConfig->Peg0Enable != 1\n"));
    }

    // Disable hot plug for function 0
    if (PciePeiPreMemConfig->PegRootPortHPE[0] != 0) {
      DEBUG ((DEBUG_INFO, "PciePeiPreMemConfig->PegRootPortHPE[0] != 0\n"));
    }

    // Program the first PEG0 lanes
    for (Index = 0; Index < (SA_PEG_MAX_LANE/2); Index++) {
      if (PciePeiPreMemConfig->PegGen3RootPortPreset[Index] != 4) {
        DEBUG ((DEBUG_INFO, "PciePeiPreMemConfig->PegGen3RootPortPreset[Index] != 4\n"));
      }
      if (PciePeiPreMemConfig->PegGen3EndPointPreset[Index] != 6){
        DEBUG ((DEBUG_INFO, "PciePeiPreMemConfig->PegGen3EndPointPreset[Index] != 6\n"));
      }
      if (PciePeiPreMemConfig->PegGen3EndPointHint[Index] != 6) {
        DEBUG ((DEBUG_INFO, "PciePeiPreMemConfig->PegGen3EndPointHint[Index] != 6\n"));
      }
    }
  }
  //
  // KblGEnd
  //
  ASSERT (SiPreMemPolicyPpi->TableHeader.Header.Revision == SI_PREMEM_POLICY_REVISION);
}

/**
  This function enumerate all downstream bridge.

  @param[in] BusNum  - Primary bus number of current bridge.

  @retval BusNum: return current bus number if current bus is an enpoint device.
  @retval SubBus: return subordinate bus number if current bus is a bridge.
**/
UINT8
EnumerateDownstream (
  IN UINT8  BusNum
  )
{
  UINTN   DeviceBaseAddress;
  UINT8   DevNum;
  UINT16  Buffer16;
  UINT8   SubBus;
  UINT8   SecBus;

  SubBus  = 0;

  SecBus  = BusNum;

  for (DevNum = 0; DevNum < 32; DevNum++) {
    ///
    /// Read Vendor ID to check if device exists
    /// if no device exists, then check next device
    ///
    DeviceBaseAddress = MmPciBase (BusNum, DevNum, 0);
    if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      continue;
    }

    Buffer16 = MmioRead16 (DeviceBaseAddress + R_PCI_SCC_OFFSET);
    ///
    /// Check for PCI/PCI Bridge Device Base Class 6 with subclass 4
    ///
    if (Buffer16 == 0x0604) {
      SecBus++;
      MmioWrite8 (DeviceBaseAddress + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, BusNum);
      MmioWrite8 (DeviceBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET, SecBus);
      ///
      /// Assign temporary subordinate bus number so that device behind this bridge can be seen
      ///
      MmioWrite8 (DeviceBaseAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET, 0xFF);

      ///
      /// A config write is required in order for the device to re-capture the Bus number,
      /// according to PCI Express Base Specification, 2.2.6.2
      /// Write to a read-only register VendorID to not cause any side effects.
      ///
      MmioWrite16 (MmPciBase (SecBus, 0, 0) + PCI_VENDOR_ID_OFFSET, 0);

      ///
      /// Enumerate bus behind this bridge by calling this function recursively
      ///
      SubBus = EnumerateDownstream (SecBus);
      ///
      /// Update the correct subordinate bus number
      ///
      MmioWrite8 (DeviceBaseAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET, SubBus);
      SecBus = SubBus;
    }
  }

  if (SubBus == 0) {
    return BusNum;
  } else {
    return SubBus;
  }
}

/**
  Find the MMIO size that a given PCI device requires

  @param[in] BusNum     - Bus number of the device
  @param[in] DevNum     - device Number of the device
  @param[in] FunNum     - Function number of the device
  @param[out] MmioLength - MMIO Length in bytes
**/
VOID
FindPciDeviceMmioLength (
  IN UINT32  BusNum,
  IN UINT32  DevNum,
  IN UINT32  FunNum,
  OUT UINT32 *MmioLength
  )
{
  UINT32  CurrentMmioLength;
  UINT32  SavedBAR;
  UINT32  i;
  UINT64  BarAlign;
  UINT8   ClassCode;
  UINTN   DeviceBaseAddress;

  *MmioLength = 0;
  BarAlign    = PCI_BAR_OLD_ALIGN;

  DeviceBaseAddress = MmPciBase (BusNum, DevNum, FunNum);
  ClassCode         = MmioRead8 (DeviceBaseAddress + R_PCI_BCC_OFFSET);
  if (ClassCode == PCI_CLASS_BRIDGE) {
    return;
  }

  for (i = R_BASE_ADDRESS_OFFSET_0; i <= R_BASE_ADDRESS_OFFSET_5; i += 4) {
    SavedBAR = MmioRead32 (DeviceBaseAddress + i);
    ///
    /// Check BAR is read-only or not
    ///
    MmioAnd32 (DeviceBaseAddress + i, (UINT32) PCI_BAR_NOCHANGE);
    MmioOr32  (DeviceBaseAddress + i, (UINT32) BarAlign);
    if (SavedBAR == MmioRead32 (DeviceBaseAddress + i)) {
      ///
      /// Restore BAR as original value
      ///
      MmioWrite32 (DeviceBaseAddress + i, SavedBAR);
      continue;
    }
    ///
    /// If BAR is not memory map, skip it
    ///
    if ((SavedBAR & BIT0) != 0) {
      ///
      /// Restore BAR as original value
      ///
      MmioWrite32 (DeviceBaseAddress + i, SavedBAR);
      continue;
    }
    ///
    /// Calculate the MMIO length through BAR
    ///
    CurrentMmioLength = ~(MmioRead32 (DeviceBaseAddress + i) &~0xF) + 1;
    *MmioLength += CurrentMmioLength;

    ///
    /// Restore BAR as original value
    ///
    MmioWrite32 (DeviceBaseAddress + i, SavedBAR);
    ///
    /// Skip next index if BAR is 64bit address
    ///
    if ((SavedBAR & (BIT1 + BIT2)) == 0x4) {
      i += 4;
    }
  }
}

/**
  Read OpROM Headers and determine if the OpROM only provides legacy VBIOS

  @param[in] RomSize                - Size of the OpROM memory mapped area
  @param[in] OpRomScanTempMmioBar   - Temporary BAR to MMIO map OpROMs during VGA scanning
  @param[in] OpRomScanTempMmioLimit - Limit address for OpROM MMIO range

  @retval TRUE if end point has a legacy only OpROM
  @retval FALSE if end point has a UEFI OpROM, Hybrid OpROM, or no OpROM
**/
BOOLEAN
ParseOpRomHeadersForLegacyOnly (
  IN UINT32   RomSize,
  IN UINT32   OpRomScanTempMmioBar,
  IN UINT32   OpRomScanTempMmioLimit
  )
{
  PCI_EXPANSION_ROM_HEADER  *RomHeader;
  PCI_DATA_STRUCTURE        *RomPcir;
  UINT32                    CurrentImageOffset;
  UINT32                    RomImageSize;
  UINT16                    OffsetPcir;
  UINT16                    EfiMachineType;
  BOOLEAN                   FirstImage;
  BOOLEAN                   FoundEfiImage;
  BOOLEAN                   FoundLegacyImage;
  UINT8                     Indicator;

  FirstImage          = TRUE;
  FoundEfiImage       = FALSE;
  FoundLegacyImage    = FALSE;
  Indicator           = 0;
  CurrentImageOffset  = 0;
  RomImageSize        = 0;
  do {
    RomHeader = (PCI_EXPANSION_ROM_HEADER*) (UINTN) (OpRomScanTempMmioBar + CurrentImageOffset);
    if (RomHeader->Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
      if (FirstImage) {
        DEBUG ((DEBUG_INFO, "Invalid OpROM\n"));
        return FALSE;
      } else {
        CurrentImageOffset += 512;
        RomImageSize       += 512;
        continue;
      }
    }
    FirstImage = FALSE;
    OffsetPcir = RomHeader->PcirOffset;
    if ((OffsetPcir == 0) || ((OffsetPcir & 3) != 0) ||
        ((RomImageSize + OffsetPcir + sizeof (PCI_DATA_STRUCTURE)) > RomSize)) {
      DEBUG ((DEBUG_INFO, "PCIR offset invalid\n"));
      break;
    }

    RomPcir = (PCI_DATA_STRUCTURE*) (UINTN) (OpRomScanTempMmioBar + CurrentImageOffset + OffsetPcir);
    if (RomPcir->Signature != PCI_DATA_STRUCTURE_SIGNATURE) {
      DEBUG ((DEBUG_INFO, "Invalid PCIR signature\n"));
      break;
    }
    if ((RomImageSize + (RomPcir->ImageLength * 512)) > RomSize) {
      DEBUG ((DEBUG_INFO, "Image size exceeds total ROM size\n"));
      break;
    }
    if (RomPcir->CodeType == PCI_CODE_TYPE_PCAT_IMAGE) {
      DEBUG ((DEBUG_INFO, "Legacy OpROM Found: Offset[0x%x]\n", CurrentImageOffset));
      FoundLegacyImage = TRUE;
    } else if (RomPcir->CodeType == PCI_CODE_TYPE_EFI_IMAGE) {
      EfiMachineType = ((EFI_PCI_EXPANSION_ROM_HEADER*) RomHeader)->EfiMachineType;
      switch (EfiMachineType) {
        case EFI_IMAGE_MACHINE_IA32:
          DEBUG ((DEBUG_INFO, "UEFI OpROM Found: Type[IA32] Offset[0x%x]\n", CurrentImageOffset));
          break;
        case EFI_IMAGE_MACHINE_IA64:
          DEBUG ((DEBUG_INFO, "UEFI OpROM Found: Type[IPF] Offset[0x%x]\n", CurrentImageOffset));
          break;
        case EFI_IMAGE_MACHINE_EBC:
          DEBUG ((DEBUG_INFO, "UEFI OpROM Found: Type[EBC] Offset[0x%x]\n", CurrentImageOffset));
          break;
        case EFI_IMAGE_MACHINE_X64:
          DEBUG ((DEBUG_INFO, "UEFI OpROM Found: Type[X64] Offset[0x%x]\n", CurrentImageOffset));
          break;
        case EFI_IMAGE_MACHINE_ARMTHUMB_MIXED:
          DEBUG ((DEBUG_INFO, "UEFI OpROM Found: Type[ARM] Offset[0x%x]\n", CurrentImageOffset));
          break;
        case EFI_IMAGE_MACHINE_AARCH64:
          DEBUG ((DEBUG_INFO, "UEFI OpROM Found: Type[ARM64] Offset[0x%x]\n", CurrentImageOffset));
          break;
        default:
          DEBUG ((DEBUG_INFO, "UEFI OpROM Found: Type[Unknown] Offset[0x%x]\n", CurrentImageOffset));
          break;
      }
      ///
      /// GOP Image Machine Type must be X64 or EBC to work on this platform
      ///
      if ((EfiMachineType == EFI_IMAGE_MACHINE_X64) || (EfiMachineType == EFI_IMAGE_MACHINE_EBC)) {
        FoundEfiImage = TRUE;
      } else {
        DEBUG ((DEBUG_INFO, "UEFI OpROM Machine Type not supported on this platform\n"));
      }
    } else {
      DEBUG ((DEBUG_INFO, "Unknown OpROM Found: Type = %d Offset[%x]\n", (UINTN) RomPcir->CodeType, CurrentImageOffset));
    }
    if ((RomPcir->ImageLength % 2) == 1) {
      DEBUG ((DEBUG_INFO, "OpROM Size: %d.5KB\n", RomPcir->ImageLength / 2));
    } else {
      DEBUG ((DEBUG_INFO, "OpROM Size: %dKB\n", RomPcir->ImageLength / 2));
    }

    Indicator           = RomPcir->Indicator;
    RomImageSize       += RomPcir->ImageLength * 512;
    CurrentImageOffset += RomPcir->ImageLength * 512;
  } while (((Indicator & BIT7) == 0x0) && (CurrentImageOffset < RomSize));

  if (!FoundEfiImage && FoundLegacyImage) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Traverse depth first to find end point in the PCI topology and determine its OpROM

  @param[in] ParentBus              - Bus number for parent device
  @param[in] ParentDev              - Device number for parent device
  @param[in] ParentFunc             - Function number for parent device
  @param[in] OpRomScanTempMmioBar   - Temporary BAR to MMIO map OpROMs during VGA scanning
  @param[in] OpRomScanTempMmioLimit - Limit address for OpROM MMIO range
  @param[in] ParentSecondary        - Secondary Bus number for the parent PCI/PCI bridge
  @param[in] ParentSubordinate      - Subordinate Bus number for the parent PCI/PCI bridge
  @param[in] EpBus                  - Bus number for end point to check
  @param[in] EpDev                  - Device number for end point to check
  @param[in] EpFunc                 - Function number for end point to check

  @retval TRUE if end point has a legacy only OpROM
  @retval FALSE if end point has a UEFI OpROM, Hybrid OpROM, or no OpROM
**/
BOOLEAN
ScanDownstreamForLegacyOpRom (
  IN      UINT8     ParentBus,
  IN      UINTN     ParentDev,
  IN      UINTN     ParentFunc,
  IN      UINT32    OpRomScanTempMmioBar,
  IN      UINT32    OpRomScanTempMmioLimit,
  IN      UINT8     ParentSecondary,
  IN      UINT8     ParentSubordinate,
  IN      UINT8     EpBus,
  IN      UINT8     EpDev,
  IN      UINT8     EpFunc,
  IN OUT  BOOLEAN   *FoundEp
  )
{
  UINTN     ParentBaseAddress;
  UINTN     DeviceBaseAddress;
  UINT32    RomSize;
  UINT16    ClassCode;
  UINT8     Dev;
  UINT8     Func;
  UINT8     MaxFunc;
  UINT8     ChildSecondary;
  UINT8     ChildSubordinate;
  UINT8     HeaderType;
  BOOLEAN   FoundLegacyOpRom;
  BOOLEAN   TraversedBridge;

  FoundLegacyOpRom  = FALSE;
  TraversedBridge   = FALSE;
  ParentBaseAddress = MmPciBase ((UINT32) ParentBus, (UINT32) ParentDev, (UINT32) ParentFunc);

  ///
  /// Forward MMIO access from the primary interface to the secondary interface
  ///
  MmioWrite32 (ParentBaseAddress + R_PCI_BRIDGE_MBL, ((OpRomScanTempMmioLimit & 0xFFFF0000) | (OpRomScanTempMmioBar >> 16)));
  MmioOr8 (ParentBaseAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// If we have reached the correct bus, there are no more bridges to traverse
  ///
  if (ParentSecondary == EpBus) {
    DeviceBaseAddress = MmPciBase ((UINT32) EpBus, (UINT32) EpDev, (UINT32) EpFunc);
    if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      (*FoundEp) = FALSE;
    } else {
      ///
      /// Found the Primary Display device
      ///
      (*FoundEp) = TRUE;
      ///
      /// Determine if an OpROM is present and its size
      ///
      MmioWrite32 (DeviceBaseAddress + PCI_EXPANSION_ROM_BASE, 0xFFFFFFFE);
      RomSize   = MmioRead32 (DeviceBaseAddress + PCI_EXPANSION_ROM_BASE);
      RomSize  &= 0xFFFFF800;  ///< Bits[10:1] are reserved
      if ((RomSize == 0) || (RomSize == 0xFFFFF800)) {
        ///
        /// Device doesn't have an OpROM, hence it doesn't have a legacy OpROM
        ///
        FoundLegacyOpRom = FALSE;
      } else {
        RomSize = ((~RomSize) + 1);
        DEBUG ((DEBUG_INFO, "Found %dKB OpROM\n", (UINTN) (RomSize / 1024)));
        ///
        /// Enable MMIO to the OpROM
        ///
        MmioWrite32 (DeviceBaseAddress + PCI_EXPANSION_ROM_BASE, (OpRomScanTempMmioBar | BIT0));
        MmioOr8 (DeviceBaseAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
        ///
        /// Parse the OpROM headers to determine if it is legacy only
        ///
        FoundLegacyOpRom = ParseOpRomHeadersForLegacyOnly (RomSize, OpRomScanTempMmioBar, OpRomScanTempMmioLimit);
        ///
        /// Disable MMIO to the OpROM
        ///
        MmioAnd8 (DeviceBaseAddress + PCI_COMMAND_OFFSET, (UINT8) ~(EFI_PCI_COMMAND_MEMORY_SPACE));
        MmioWrite32 (DeviceBaseAddress + PCI_EXPANSION_ROM_BASE, 0);
      }
    }
  } else {
    ///
    /// Scan Parent Secondary Bus for more bridges
    ///
    for (Dev = 0; Dev < 32; Dev++) {
      DeviceBaseAddress = MmPciBase ((UINT32) ParentSecondary, (UINT32) Dev, 0);
      if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
        continue;
      }
      HeaderType = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE_OFFSET);
      if ((HeaderType & HEADER_TYPE_MULTI_FUNCTION) != 0) {
        MaxFunc = 7;
      } else {
        MaxFunc = 0;
      }
      for (Func = 0; Func <= MaxFunc; Func++) {
        if (Func != 0) {
          DeviceBaseAddress = MmPciBase ((UINT32) ParentSecondary, (UINT32) Dev, (UINT32) Func);
          if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
            continue;
          }
        }
        ///
        /// Search for the next PCI/PCI Bridge
        ///
        ClassCode = MmioRead16 (DeviceBaseAddress + R_PCI_SCC_OFFSET);
        if (ClassCode == 0x0604) {
          ChildSecondary    = MmioRead8 (DeviceBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
          ChildSubordinate  = MmioRead8 (DeviceBaseAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
          if ((EpBus >= ChildSecondary) && (EpBus <= ChildSubordinate)) {
            DEBUG ((
                     DEBUG_INFO,
                     "Traversing PCI/PCI Bridge at (%x:%x:%x) Sec[%d], Sub[%d]\n",
                     (UINTN) ParentSecondary,
                     (UINTN) Dev,
                     (UINTN) Func,
                     (UINTN) ChildSecondary,
                     (UINTN) ChildSubordinate
                     ));
            TraversedBridge   = TRUE;
            FoundLegacyOpRom  = ScanDownstreamForLegacyOpRom (
                                  ParentSecondary,
                                  Dev,
                                  Func,
                                  OpRomScanTempMmioBar,
                                  OpRomScanTempMmioLimit,
                                  ChildSecondary,
                                  ChildSubordinate,
                                  EpBus,
                                  EpDev,
                                  EpFunc,
                                  FoundEp
                                  );
            break;
          }
        }
      }
      if (TraversedBridge) {
        break;
      }
    }
  }
  ///
  /// Disable MMIO access now that we are done
  ///
  MmioAnd8 (ParentBaseAddress + PCI_COMMAND_OFFSET, (UINT8) ~(EFI_PCI_COMMAND_MEMORY_SPACE));
  MmioWrite32 (ParentBaseAddress + R_PCI_BRIDGE_MBL, 0);
  return FoundLegacyOpRom;
}

/**
  Check if device has a legacy only OpROM (CSM required to dispatch device's OpROM)

  @param[in] OpRomScanTempMmioBar   - Temporary BAR to MMIO map OpROMs during VGA scanning
  @param[in] OpRomScanTempMmioLimit - Limit address for OpROM MMIO range
  @param[in] EpBus                  - Bus number for end point to check
  @param[in] EpDev                  - Device number for end point to check
  @param[in] EpFunc                 - Function number for end point to check

  @retval TRUE if end point has a legacy only OpROM
  @retval FALSE if end point has a UEFI OpROM, Hybrid OpROM, or no OpROM
**/
BOOLEAN
CheckForLegacyOnlyOpRom (
  IN UINT32   OpRomScanTempMmioBar,
  IN UINT32   OpRomScanTempMmioLimit,
  IN UINT8    EpBus,
  IN UINT8    EpDev,
  IN UINT8    EpFunc
  )
{
  EFI_STATUS  Status;
  UINTN       RpDev;
  UINTN       RpFunc;
  UINTN       RpIndex;
  UINTN       RpBaseAddress;
  CPU_FAMILY  CpuFamilyId;
  BOOLEAN     FoundEp;
  BOOLEAN     FoundLegacyOpRom;
  UINT8       SecondaryBus;
  UINT8       SubordinateBus;

  RpDev       = SA_PEG_DEV_NUM;
  CpuFamilyId = GetCpuFamily ();

  ///
  /// Check that correct alignment and range is provided
  ///
  ASSERT ((OpRomScanTempMmioBar % 0x1000000) == 0);
  ASSERT ((OpRomScanTempMmioLimit - OpRomScanTempMmioBar) >= 0x1000000);
  ///
  /// First determine the root port the device is connected to, check PEG first
  ///
  DEBUG ((DEBUG_INFO, "Detecting OpROM for (%x:%x:%x)\n", (UINTN) EpBus, (UINTN) EpDev, (UINTN) EpFunc));
  if (IsPchLinkDmi (CpuFamilyId)) {
    for (RpFunc = 0; RpFunc < SA_PEG_MAX_FUN; RpFunc++) {
      RpBaseAddress = MmPciBase (SA_PEG_BUS_NUM, (UINT32) RpDev, (UINT32) RpFunc);
      if (MmioRead16 (RpBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
        continue;
      }
      SecondaryBus    = MmioRead8 (RpBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      SubordinateBus  = MmioRead8 (RpBaseAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
      DEBUG ((
               DEBUG_INFO,
               "PCIe RP (%x:%x:%x) Sec[%d], Sub[%d]\n",
               SA_PEG_BUS_NUM,
               RpDev,
               RpFunc,
               (UINTN) SecondaryBus,
               (UINTN) SubordinateBus
               ));
      if ((EpBus >= SecondaryBus) && (EpBus <= SubordinateBus)) {
        DEBUG ((DEBUG_INFO, "Found correct RP, traversing downstream...\n"));
        FoundEp           = FALSE;
        FoundLegacyOpRom  = ScanDownstreamForLegacyOpRom (
                              SA_PEG_BUS_NUM,
                              RpDev,
                              RpFunc,
                              OpRomScanTempMmioBar,
                              OpRomScanTempMmioLimit,
                              SecondaryBus,
                              SubordinateBus,
                              EpBus,
                              EpDev,
                              EpFunc,
                              &FoundEp
                              );
        if (FoundEp) {
          DEBUG ((DEBUG_INFO, "Legacy Only VBIOS: %d\n", (UINTN) FoundLegacyOpRom));
          return FoundLegacyOpRom;
        }
      }
    }
  }
  for (RpIndex = 0; RpIndex < GetPchMaxPciePortNum (); RpIndex++) {
    ///
    /// Check if root port exists
    ///
    Status = GetPchPcieRpDevFun (RpIndex, &RpDev, &RpFunc);
    ASSERT_EFI_ERROR (Status);

    RpBaseAddress = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDev, (UINT32) RpFunc);
    if (MmioRead16 (RpBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      continue;
    }
    SecondaryBus    = MmioRead8 (RpBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    SubordinateBus  = MmioRead8 (RpBaseAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
    DEBUG ((
             DEBUG_INFO,
             "PCIe RP (%x:%x:%x) Sec[%d], Sub[%d]\n",
             DEFAULT_PCI_BUS_NUMBER_PCH,
             RpDev,
             RpFunc,
             (UINTN) SecondaryBus,
             (UINTN) SubordinateBus
             ));
    if ((EpBus >= SecondaryBus) && (EpBus <= SubordinateBus)) {
      DEBUG ((DEBUG_INFO, "Found correct RP, traversing downstream...\n"));
      FoundEp           = FALSE;
      FoundLegacyOpRom  = ScanDownstreamForLegacyOpRom (
                            DEFAULT_PCI_BUS_NUMBER_PCH,
                            RpDev,
                            RpFunc,
                            OpRomScanTempMmioBar,
                            OpRomScanTempMmioLimit,
                            SecondaryBus,
                            SubordinateBus,
                            EpBus,
                            EpDev,
                            EpFunc,
                            &FoundEp
                            );
      if (FoundEp) {
        DEBUG ((DEBUG_INFO, "Legacy Only VBIOS: %d\n", (UINTN) FoundLegacyOpRom));
        return FoundLegacyOpRom;
      }
    }
  }
  DEBUG ((DEBUG_WARN, "Primary Graphics device not found!\n"));
  return FALSE;
}

/**
  CheckOffboardPcieVga: Check if off board PCIe graphics Card is present

  @param[in, out] PchPcieMmioLength       - Total PCIe MMIO length on all PCH root ports
  @param[in, out] PrimaryDisplay          - Primary Display - default is IGD
  @param[in]      OpRomScanTempMmioBar    - Temporary BAR to MMIO map OpROMs during VGA scanning
  @param[in]      OpRomScanTempMmioLimit  - Limit address for OpROM MMIO range
  @param[in]      ScanForLegacyOpRom      - TRUE to scan for legacy only VBIOS, FALSE otherwise
  @param[out]     FoundLegacyOpRom        - If legacy only VBIOS found, returns TRUE
**/
VOID
CheckOffboardPcieVga (
  IN OUT   UINT32                       *PchPcieMmioLength,
  IN OUT   DISPLAY_DEVICE               *PrimaryDisplay,
  IN       UINT32                       OpRomScanTempMmioBar,
  IN       UINT32                       OpRomScanTempMmioLimit,
  IN       BOOLEAN                      ScanForLegacyOpRom,
  OUT      BOOLEAN                      *FoundLegacyOpRom
  )
{
  EFI_STATUS  Status;
  UINTN       RpDev;
  UINTN       RpFunc;
  UINTN       RpIndex;
  UINTN       DeviceBaseAddress;
  UINT32      PcieBusNum;
  UINT8       Bus;
  UINT8       Dev;
  UINT8       Func;
  UINT8       MaxFunction;
  UINT8       SubBusNum;
  UINT8       HeaderType;
  UINT16      Buffer16;
  BOOLEAN     CardDetect;
  UINT32      MmioLength;

  MmioLength = 0;

  ///
  /// Initialize Secondary and Subordinate bus number for first Pcie root port
  ///
  PcieBusNum  = 0x00010100;

  SubBusNum   = 0;

  CardDetect  = FALSE;

  for (RpIndex = 0; RpIndex < GetPchMaxPciePortNum (); RpIndex++) {
    ///
    /// Check if root port exists
    ///
    Status = GetPchPcieRpDevFun (RpIndex, &RpDev, &RpFunc);
    ASSERT_EFI_ERROR (Status);

    DeviceBaseAddress = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDev, (UINT32) RpFunc);
    if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      continue;
    }

    MmioWrite32 (DeviceBaseAddress + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, PcieBusNum);
    Bus = MmioRead8 (DeviceBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);

    ///
    /// Assign temporary subordinate bus number so that device this bridge can be seen
    ///
    MmioWrite8 (DeviceBaseAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET, 0xFF);

    ///
    /// A config write is required in order for the device to re-capture the Bus number,
    /// according to PCI Express Base Specification, 2.2.6.2
    /// Write to a read-only register VendorID to not cause any side effects.
    ///
    MmioWrite16 (MmPciBase (Bus, 0, 0) + PCI_VENDOR_ID_OFFSET, 0);

    SubBusNum                       = EnumerateDownstream (Bus);
    ///
    /// Update the actual subordinate bus number
    ///
    MmioWrite8 (DeviceBaseAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET, SubBusNum);
    PcieBusNum = (SubBusNum + 1) << 8;
  }

  for (Bus = 1; Bus <= SubBusNum; Bus++) {
    for (Dev = 0; Dev < 32; Dev++) {
      ///
      /// Read Vendor ID to check if device exists
      /// if no device exists, then check next device
      ///
      DeviceBaseAddress = MmPciBase (Bus, Dev, 0);
      if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
        continue;
      }
      ///
      /// Check for a multifunction device
      ///
      HeaderType = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE_OFFSET);
      if ((HeaderType & HEADER_TYPE_MULTI_FUNCTION) != 0) {
        MaxFunction = 7;
      } else {
        MaxFunction = 0;
      }

      for (Func = 0; Func <= MaxFunction; Func++) {
        if (MmioRead16 (MmPciBase (Bus, Dev, Func) + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
          continue;
        }

        FindPciDeviceMmioLength (Bus, Dev, Func, &MmioLength);
        *PchPcieMmioLength += MmioLength;

        ///
        /// Video cards can have Base Class 0 with Sub-class 1
        /// or Base Class 3.
        ///
        if (MmioRead16 (MmPciBase (Bus, Dev, Func) + R_PCI_SCC_OFFSET) == 0x0300) {
          if (CardDetect != TRUE) {
            *PrimaryDisplay = PCI;
            DEBUG ((DEBUG_INFO, "PCH PCIe Graphics Card enabled.\n"));
            CardDetect = TRUE;
            if (ScanForLegacyOpRom) {
              (*FoundLegacyOpRom) = CheckForLegacyOnlyOpRom (OpRomScanTempMmioBar, OpRomScanTempMmioLimit, Bus, Dev, Func);
            }
          }
        }
      }
    }
  }
  ///
  /// Clear bus number on all the bridges that we have opened so far.
  /// We have to do it in the reverse Bus number order.
  ///
  for (Bus = SubBusNum; Bus >= 1; Bus--) {
    for (Dev = 0; Dev < 32; Dev++) {
      ///
      /// Read Vendor ID to check if device exists
      /// if no device exists, then check next device
      ///
      DeviceBaseAddress = MmPciBase (Bus, Dev, 0);
      if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
        continue;
      }

      Buffer16 = MmioRead16 (DeviceBaseAddress + R_PCI_SCC_OFFSET);
      ///
      /// Clear Bus Number for PCI/PCI Bridge Device
      ///
      if (Buffer16 == 0x0604) {
        MmioWrite32 (DeviceBaseAddress + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0);
      }
    }
  }

  for (RpIndex = 0; RpIndex < GetPchMaxPciePortNum (); RpIndex++) {
    ///
    /// Clear bus numbers so that PCIe slots are hidden
    ///
    Status = GetPchPcieRpDevFun (RpIndex, &RpDev, &RpFunc);
    ASSERT_EFI_ERROR (Status);

    DeviceBaseAddress = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDev, (UINT32) RpFunc);
    MmioWrite32 (DeviceBaseAddress + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0);
  }
}

/**
  Program flow control credits for ULT/UlX CPUs
**/
VOID
FlowControlCreditProgrammingUltUlx (
  VOID
  )
{
}

/**
  SA early PEI Initialization.

  @retval EFI_SUCCESS           Function executed successfully
  @retval EFI_NOT_FOUND         The required PPI could not be located.
**/
EFI_STATUS
EFIAPI
SaOnPolicyInstalled (
  VOID
  )
{
  EFI_STATUS                    Status;
  SI_PREMEM_POLICY_PPI          *SiPreMemPolicyPpi;
  SA_MISC_PEI_PREMEM_CONFIG     *MiscPeiPreMemConfig;
  GRAPHICS_PEI_PREMEM_CONFIG    *GtConfig;
  MEMORY_CONFIGURATION          *MemConfig;
  PCIE_PEI_PREMEM_CONFIG        *PciePeiPreMemConfig;
  OVERCLOCKING_PREMEM_CONFIG    *SaOverclockingPreMemConfig;
  SWITCHABLE_GRAPHICS_CONFIG    *SgGpioData;
  MEMORY_CONFIG_NO_CRC          *MemConfigNoCrc;
  SA_DATA_HOB                   *SaDataHob;
  SA_CONFIG_HOB                 *SaConfigHob;
  SA_PEG_HOB                    *SaPegHob;
  CPU_FAMILY                    CpuFamilyId;
  DISPLAY_DEVICE                PrimaryDisplay;
  UINT32                        PchPcieMmioLength;
  UINT32                        PegMmioLength;
  UINT32                        IGfxMmioLength;
  UINT32                        TotalMmioLength;
  UINTN                         McD0BaseAddress;
  BOOLEAN                       ScanForLegacyOpRom;
  BOOLEAN                       FoundLegacyOpRom;
  BOOLEAN                       TempFoundLegacyOpRom;
  UINT32                        ResMemLimit1;
  UINT32                        AddMem;
  UINT32                        MchBar;
  CPU_SKU                       CpuSku;

  DEBUG ((DEBUG_INFO, "SaOnPolicyInstalled () - Start\n"));
  DEBUG ((DEBUG_INFO, "Pre-Mem SaInit Entry\n"));
  PostCode (0xA00);

  PchPcieMmioLength    = 0;
  PegMmioLength        = 0;
  IGfxMmioLength       = 0;
  FoundLegacyOpRom     = FALSE;
  TempFoundLegacyOpRom = FALSE;
  PrimaryDisplay       = IGD;
  CpuFamilyId          = GetCpuFamily ();
  CpuSku               = GetCpuSku ();

  ///
  /// Get policy settings through the SaInitConfigBlock PPI
  ///
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);
  if ((Status != EFI_SUCCESS) || (SiPreMemPolicyPpi == NULL)) {
    return EFI_NOT_FOUND;
  }

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *) &MiscPeiPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gGraphicsPeiPreMemConfigGuid, (VOID *) &GtConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMemoryConfigGuid, (VOID *) &MemConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaPciePeiPreMemConfigGuid, (VOID *) &PciePeiPreMemConfig);
  ASSERT_EFI_ERROR (Status);


  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSwitchableGraphicsConfigGuid, (VOID *) &SgGpioData);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMemoryConfigNoCrcGuid, (VOID *) &MemConfigNoCrc);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaOverclockingPreMemConfigGuid, (VOID *) &SaOverclockingPreMemConfig);
  ASSERT_EFI_ERROR (Status);


  ///
  /// Program SA Bar Registers
  ///
  DEBUG ((DEBUG_INFO, "Programming SA Bars\n"));
  PostCode (0xA06);
  ProgramSaBars (MiscPeiPreMemConfig);

  ///
  /// Install SA HOBs
  ///
  DEBUG ((DEBUG_INFO, "Install SA HOBs\n"));
  PostCode (0xA08);
  InstallSaHob (MiscPeiPreMemConfig, &SaDataHob, &SaConfigHob, &SaPegHob);

  ///
  /// Report SA PCIe code version
  ///
  DEBUG ((DEBUG_INFO, "Reporting SA PCIe code version\n"));
  PostCode (0xA0A);
  ReportPcieVersion (MiscPeiPreMemConfig );

  if (IsPchLinkDmi (CpuFamilyId)) {
    ///
    /// Initialize DMI
    ///
    DEBUG ((DEBUG_INFO, "Initializing DMI\n"));
    PostCode (0xA10);
    DmiInit (
      PciePeiPreMemConfig
      );
  }

  ///
  /// Initialize DMI/OPI Max PayLoad Size
  ///
  DEBUG ((DEBUG_INFO, "Initializing DMI/OPI Max PayLoad Size\n"));
  PostCode (0xA1F);
  MaxPayloadSizeInit (PciePeiPreMemConfig);

  ///
  /// Set the VGA Decode bits to a good known starting point where both PEG and
  /// IGD VGA Decode Bits are both disabled.
  ///
  McD0BaseAddress   = MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN);
  MmioOr16 (McD0BaseAddress + R_SA_GGC, B_SA_GGC_IVD_MASK);

  ///
  /// Initialize SwitchableGraphics
  ///
  DEBUG ((DEBUG_INFO, "Initializing SwitchableGraphics\n"));
  PostCode (0xA20);
  SwitchableGraphicsInit (SgGpioData, MiscPeiPreMemConfig);

  if (IsPchLinkDmi (CpuFamilyId)) {
    ///
    /// Initialize SA PCIe
    ///
    DEBUG ((DEBUG_INFO, "Initializing SA PCIe\n"));
    PostCode (0xA30);
    PciExpressInit (
      MiscPeiPreMemConfig,
      PciePeiPreMemConfig
      );
  } else {
    if ((CpuSku == EnumCpuUlx) || (CpuSku == EnumCpuUlt)) {
      ///
      /// Program PEG credit with the values for ULT and ULX CPUs
      ///
      DEBUG ((DEBUG_INFO, "Programming PEG credit values Start\n"));
      PostCode (0xA3F);
      FlowControlCreditProgrammingUltUlx ();
    }
  }

  ///
  /// Initialize DMI Tc/Vc mapping setting
  ///
  DEBUG ((DEBUG_INFO, "Initializing DMI Tc/Vc mapping\n"));
  PostCode (0xA40);
  SaDmiTcVcInit (PciePeiPreMemConfig);

  ///
  /// Graphics Detection (IGD/PEG/PCH)
  ///
  DEBUG ((DEBUG_INFO, "Graphics detection\n"));
  if (MiscPeiPreMemConfig->SkipExtGfxScan == DISABLED) {
    if (MiscPeiPreMemConfig->ScanExtGfxForLegacyOpRom == 0) {
      ScanForLegacyOpRom = FALSE;
    } else {
      ScanForLegacyOpRom = TRUE;
    }
    FoundLegacyOpRom = FALSE;
    DEBUG ((DEBUG_INFO, "CheckOffboardPcieVga\n"));
    PostCode (0xA42);
    CheckOffboardPcieVga (
      &PchPcieMmioLength,
      &PrimaryDisplay,
      MiscPeiPreMemConfig->OpRomScanTempMmioBar,
      MiscPeiPreMemConfig->OpRomScanTempMmioLimit,
      ScanForLegacyOpRom,
      &TempFoundLegacyOpRom
      );
    FoundLegacyOpRom |= TempFoundLegacyOpRom;
    DEBUG ((DEBUG_INFO, "CheckAndInitializePegVga\n"));
    PostCode (0xA44);
    CheckAndInitializePegVga (
      &PrimaryDisplay,
      (UINT8) GtConfig->PrimaryDisplay,
      &PegMmioLength,
      MiscPeiPreMemConfig->OpRomScanTempMmioBar,
      MiscPeiPreMemConfig->OpRomScanTempMmioLimit,
      ScanForLegacyOpRom,
      &TempFoundLegacyOpRom
      );
    FoundLegacyOpRom |= TempFoundLegacyOpRom;
    if (FoundLegacyOpRom) {
      SaDataHob->LegacyOnlyVgaOpRomDetected = 1;
    } else {
      SaDataHob->LegacyOnlyVgaOpRomDetected = 0;
    }
  }

  ///
  /// Initialize Graphics (IGD)
  ///
  DEBUG ((DEBUG_INFO, "Initializing Graphics\n"));
  PostCode (0xA50);
  GraphicsInit (GtConfig, &PrimaryDisplay, &IGfxMmioLength);

  ///
  /// Initialize System Agent Overclocking
  ///
  DEBUG ((DEBUG_INFO, "Initializing System Agent Overclocking\n"));
  PostCode (0xA52);
  SaOcInit(SaOverclockingPreMemConfig);

  ///
  /// Determine MMIO Size for Dynamic Tolud
  ///
  TotalMmioLength = PchPcieMmioLength + PegMmioLength + IGfxMmioLength;
  DEBUG ((DEBUG_INFO, "TotalMmioLength:   0x%08X bytes\n", TotalMmioLength));
  if (MiscPeiPreMemConfig->MmioSize == 0x00) {
    /**
      Dynamic MmioSize will calculate the value basing on MmioLength found from main consumers. (Video cards)
      Since PCIe and some other Chipset MMIO BAR occupied Top MMIO region, we need to enlarge MmioSize.
      By default 0xE0000000 ~ 0xFFFFFFFF are not available as MMIO resource so add 512MB in all cases.
      MiscPeiPreMemConfig->MmioSizeAdjustment will be used later if needed to increase or decrease the final MmioSize.
    **/

    ///
    /// Get PCI Express Base address and calculate the memory occupied on Top MMIO region by that.
    ///
    ResMemLimit1 = PcdGet32 (PcdPciReservedMemLimit);
    if (ResMemLimit1 == 0) {
      ResMemLimit1 = ((UINTN) MmPciBase (0, 0, 0));
    }

    AddMem = ((MAX_ADDRESS_32BIT - ResMemLimit1) + 1) >> 20;
    ///
    /// if total MMIO need 1GB or over
    ///
    if (TotalMmioLength >= 0x40000000) {
      MiscPeiPreMemConfig->MmioSize = 0x800 + (UINT16) AddMem;
    }
    ///
    /// if total MMIO need 728MB~1GB
    ///
    else if (TotalMmioLength >= 0x30000000) {
      MiscPeiPreMemConfig->MmioSize = 0x700 + (UINT16) AddMem;
    }
    ///
    /// if total MMIO need 512MB~728MB
    ///
    else if (TotalMmioLength >= 0x20000000) {
      MiscPeiPreMemConfig->MmioSize = 0x600 + (UINT16) AddMem;
    }
    ///
    /// if total MMIO need 256MB~512MB
    ///
    else if (TotalMmioLength >= 0x10000000) {
      MiscPeiPreMemConfig->MmioSize = 0x500 + (UINT16) AddMem;
    }
    ///
    /// if total MMIO need less than 256MB
    ///
    else {
      MiscPeiPreMemConfig->MmioSize = 0x400 + (UINT16) AddMem;
    }
    //
    // Increase or Decrease MMIO size basing on platform requirement: MmioSizeAdjustment and it can be negative or positive.
    // If MmioSize not in reasonable range (Minimal should be larger than 0MB and Maximal 3840MB.), reset to safe default 1GB.
    //
    MiscPeiPreMemConfig->MmioSize = (INT32) (MiscPeiPreMemConfig->MmioSize) + MiscPeiPreMemConfig->MmioSizeAdjustment;
    if ((MiscPeiPreMemConfig->MmioSize == 0) || (MiscPeiPreMemConfig->MmioSize > 0xF00)) {
      DEBUG ((DEBUG_WARN, "MmioSize 0x%x is not reasonable, reset to default 1024MB!\n"));
      MiscPeiPreMemConfig->MmioSize = 0x400;
    }
  }

  MchBar = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR) &~BIT0;

  ///
  /// Configure MchBar+ R_SA_MCHBAR_HDAUDRID_IMPH [7:3] = 0x1F and set Bit31.
  ///
  MmioAndThenOr32 ((UINTN) MchBar + R_SA_MCHBAR_HDAUDRID_IMPH, (UINT32) ~B_SA_MCHBAR_HDAUDRID_IMPH_DEVNUM_MASK, (UINT32) (0x1F << N_SA_MCHBAR_HDAUDRID_IMPH_DEVNUM_OFFSET) | BIT31);
  ///
  /// Set MchBar+0x701C Bit 22 to 1
  ///
  MmioOr32 ((UINTN) MchBar + 0x701C, BIT22);
  DEBUG((DEBUG_INFO, "Pre-Mem SaInit Exit\n"));
  PostCode (0xA7F);
  DEBUG ((DEBUG_INFO, "SaOnPolicyInstalled () - End\n"));

  return EFI_SUCCESS;
}

/**
  Programs SA Bars

  @param[in] MiscPeiPreMemConfig - Instance of SA_MISC_PEI_PREMEM_CONFIG
**/
VOID
ProgramSaBars (
  IN    SA_MISC_PEI_PREMEM_CONFIG      *MiscPeiPreMemConfig
  )
{
  UINTN                              McD0BaseAddress;

  ///
  /// Program SA MchBar, DmiBar and EpBar
  ///
  McD0BaseAddress = MmPciBase (SA_MC_BUS, 0, 0);
  MmioWrite32 (McD0BaseAddress + R_SA_MCHBAR + 4, (UINT32)RShiftU64 (MiscPeiPreMemConfig->MchBar,32));
  MmioWrite32(McD0BaseAddress + R_SA_MCHBAR, (UINT32)(MiscPeiPreMemConfig->MchBar | BIT0));

  MmioWrite32 (McD0BaseAddress + R_SA_DMIBAR + 4, (UINT32)RShiftU64 (MiscPeiPreMemConfig->DmiBar, 32));
  MmioWrite32(McD0BaseAddress + R_SA_DMIBAR, (UINT32)(MiscPeiPreMemConfig->DmiBar | BIT0));

  MmioWrite32 (McD0BaseAddress + R_SA_PXPEPBAR + 4, (UINT32)RShiftU64(MiscPeiPreMemConfig->EpBar, 32));
  MmioWrite32(McD0BaseAddress + R_SA_PXPEPBAR, (UINT32)(MiscPeiPreMemConfig->EpBar | BIT0));

  ///
  /// Program SA GdxcBar
  ///
  MmioWrite32 ((UINTN)(MiscPeiPreMemConfig->MchBar) + R_SA_MCHBAR_GDXCBAR_OFFSET + 4, (UINT32)RShiftU64 (MiscPeiPreMemConfig->GdxcBar, 32));
  MmioWrite32((UINTN)(MiscPeiPreMemConfig->MchBar) + R_SA_MCHBAR_GDXCBAR_OFFSET, (UINT32)(MiscPeiPreMemConfig->GdxcBar | BIT0));

  ///
  /// Detect EDRAM
  ///
  if (AsmReadMsr64 (MSR_PLATFORM_INFO) & B_PLATFORM_INFO_EDRAM_EN) {
    ///
    /// Program SA EdramBar
    ///
    MmioWrite32 ((UINTN) (MiscPeiPreMemConfig->MchBar) + R_SA_MCHBAR_EDRAMBAR_OFFSET + 4, (UINT32)RShiftU64 (MiscPeiPreMemConfig->EdramBar, 32));
    MmioWrite32 ((UINTN) (MiscPeiPreMemConfig->MchBar) + R_SA_MCHBAR_EDRAMBAR_OFFSET, (UINT32)(MiscPeiPreMemConfig->EdramBar | BIT0));
  }
}


/**
  Init and Install SA Hob

  @param[in]  MiscPeiPreMemConfig - Instance of SA_MISC_PEI_PREMEM_CONFIG
  @param[out] SaDataHob           - SA_DATA_HOB instance installed by this function
  @param[out] SaConfigHob         - SA_CONFIG_HOB instance installed by this function
  @param[out] SaPegHob            - SA_PEG_HOB instance installed by this function
  @retval EFI_SUCCESS
**/
EFI_STATUS
InstallSaHob (
  IN       SA_MISC_PEI_PREMEM_CONFIG   *MiscPeiPreMemConfig,
  OUT      SA_DATA_HOB                 **SaDataHobOut,
  OUT      SA_CONFIG_HOB               **SaConfigHobOut,
  OUT      SA_PEG_HOB                  **SaPegHobOut
  )
{
  EFI_STATUS                   Status;
  SA_DATA_HOB                  *SaDataHob;
  SA_PEG_HOB                   *SaPegHob;
  SA_CONFIG_HOB                *SaConfigHob;
  SI_PREMEM_POLICY_PPI         *SiPreMemPolicy;
  CPU_SECURITY_PREMEM_CONFIG   *CpuSecurityPreMemConfig;
  CPU_TXT_PREMEM_CONFIG        *CpuTxtPreMemConfig;
  UINT16                      BiosMemSizeInMb;

  BiosMemSizeInMb           = 0;


  ///
  /// Create HOB for SA Data
  ///
  Status = PeiServicesCreateHob (
             EFI_HOB_TYPE_GUID_EXTENSION,
             sizeof (SA_DATA_HOB),
             (VOID **) &SaDataHob
             );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Create HOB for SA PEG
  ///
  Status = PeiServicesCreateHob (
             EFI_HOB_TYPE_GUID_EXTENSION,
             sizeof (SA_PEG_HOB),
             (VOID **) &SaPegHob
             );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Create HOB for SA Config
  ///
  Status = PeiServicesCreateHob (
             EFI_HOB_TYPE_GUID_EXTENSION,
             sizeof (SA_CONFIG_HOB),
             (VOID **) &SaConfigHob
             );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initialize default HOB data
  ///
  SaDataHob->EfiHobGuidType.Name = gSaDataHobGuid;
  DEBUG ((DEBUG_INFO, "SaDataHob->EfiHobGuidType.Name: %g\n", &SaDataHob->EfiHobGuidType.Name));
  ZeroMem (&(SaDataHob->IedSize), sizeof (SA_DATA_HOB) - sizeof (EFI_HOB_GUID_TYPE));

  SaConfigHob->EfiHobGuidType.Name = gSaConfigHobGuid;
  DEBUG ((DEBUG_INFO, "SaConfigHob->EfiHobGuidType.Name: %g\n", &SaConfigHob->EfiHobGuidType.Name));
  ZeroMem (&(SaConfigHob->DprDirectory[0]), sizeof (SA_CONFIG_HOB) - sizeof (EFI_HOB_GUID_TYPE));

  SaPegHob->EfiHobGuidType.Name = gSaPegHobGuid;
  DEBUG ((DEBUG_INFO, "SaPegHob->EfiHobGuidType.Name: %g\n", &SaPegHob->EfiHobGuidType.Name));
  ZeroMem (&(SaPegHob->PegData), sizeof (SA_PEG_DATA));


  DEBUG ((DEBUG_INFO, "SaDataHob @ %X\n", SaDataHob));
  DEBUG ((DEBUG_INFO, "&(SaDataHob->PegPlatformResetRequired) @ %X\n", &(SaDataHob->PegPlatformResetRequired)));
  DEBUG ((DEBUG_INFO, "SaDataHobSize - HobHeader: %X\n", sizeof (SA_DATA_HOB) - sizeof (EFI_HOB_GUID_TYPE)));
  DEBUG ((DEBUG_INFO, "SaDataHobSize: %X\n", sizeof (SA_DATA_HOB)));


  DEBUG ((DEBUG_INFO, "SaPegHob @ %X\n", SaPegHob));
  DEBUG ((DEBUG_INFO, "&(SaPegHob->PegData) @ %X\n", &(SaPegHob->PegData)));
  DEBUG ((DEBUG_INFO, "SaPegHob - HobHeader: %X\n", sizeof (SA_PEG_HOB) - sizeof (EFI_HOB_GUID_TYPE)));
  DEBUG ((DEBUG_INFO, "SaPegData: %X\n", sizeof (SA_PEG_DATA)));

  DEBUG ((DEBUG_INFO, "SaDataHob->1: %X\n", SaDataHob->PegPlatformResetRequired));
  DEBUG ((DEBUG_INFO, "SaDataHob->2: %X\n", SaDataHob->LegacyOnlyVgaOpRomDetected));
  DEBUG ((DEBUG_INFO, "SaDataHob->3: %X\n", SaDataHob->IedSize));

  DEBUG ((DEBUG_INFO, "SaConfigHob->1: %X\n", SaConfigHob->SaRtd3.Rtd3Pcie0Gpio.HoldRst.ExpanderNo));
  DEBUG ((DEBUG_INFO, "SaConfigHob->2: %X\n", SaConfigHob->SaRtd3.Rtd3Pcie0Gpio.HoldRst.GpioNo));
  DEBUG ((DEBUG_INFO, "SaConfigHob->3: %X\n", SaConfigHob->SaRtd3.Rtd3Pcie0Gpio.HoldRst.Active));

  ///
  /// Update SA Data HOB
  ///
  SaDataHob->IedSize = MiscPeiPreMemConfig->IedSize;

  ///
  /// Update SA Config HOB
  ///
  Status = PeiServicesLocatePpi(
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicy
             );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gCpuTxtPreMemConfigGuid, (VOID *) &CpuTxtPreMemConfig);
  ///
  /// TXT DPR Directory Entry
  ///
    SaConfigHob->DprDirectory[EnumDprDirectoryTxt].Type = DPR_DIRECTORY_TYPE_TXT;
    if (CpuSecurityPreMemConfig->Txt) {
      SaConfigHob->DprDirectory[EnumDprDirectoryTxt].Size = (UINT8) RShiftU64 ((UINT64) CpuTxtPreMemConfig->TxtDprMemorySize, 20);
    }

  ///
  /// BIOS Guard Directory Entry
  ///
  SaConfigHob->DprDirectory[EnumDprDirectoryBiosGuard].Type = DPR_DIRECTORY_TYPE_BIOSGUARD;
  if (CpuSecurityPreMemConfig->BiosGuard == TRUE) {
    ///
    /// Check if FlashWearoutProtection policy is enabled
    /// If Enabled, the size allocated to BIOS Guard in DPR is 0MB, BGUP data will be stored in TSEG
    /// Otherwise, BGUP data will use the memory allocated within DPR
    ///
    if (CpuSecurityPreMemConfig->FlashWearOutProtection == FALSE) {
      ///
      /// The size allocated for DPR needs to account for the size of the BIOS ROM plus the size of the
      /// BGUP data: BGUP Header, BGUP Script,and BGUP Certificate.
      /// The size allocated needs to aligned to 1MB due to 1MB granularity of DPR.
      ///
      if ((CpuSecurityPreMemConfig->BiosSize % 1024) == 0) {
        BiosMemSizeInMb = (CpuSecurityPreMemConfig->BiosSize / 1024) + 1;
      } else {
        ///
        /// For a BiosSize that is not exactly 1MB, additional space will need to be added
        /// With consideration of the BGUP data, we can add an additional 2MB for a BIOS
        /// with greater than X.5MB, where X represents whole number value of the BiosSize
        ///
        if ((CpuSecurityPreMemConfig->BiosSize % 1024) < 512) {
          BiosMemSizeInMb = (CpuSecurityPreMemConfig->BiosSize / 1024) + 1;
        } else {
          BiosMemSizeInMb = (CpuSecurityPreMemConfig->BiosSize / 1024) + 2;
        }
      }
      ///
      /// Upon MTRR shortage, BiosMemSizeInMb can be 2MB aligned at the expense of losing memory.
      ///
      if ((BiosMemSizeInMb % 2) == 1) {
        BiosMemSizeInMb = BiosMemSizeInMb + 1;
      }
    }

    SaConfigHob->DprDirectory[EnumDprDirectoryBiosGuard].Size = (UINT8) BiosMemSizeInMb;
    DEBUG ((DEBUG_INFO, "BIOS Guard DPR Directory Size: %x MB\n", BiosMemSizeInMb));
  }

  (*SaDataHobOut) = SaDataHob;
  (*SaConfigHobOut) = SaConfigHob;
  (*SaPegHobOut) = SaPegHob;
  DEBUG ((DEBUG_INFO, "SA HOBs installed\n"));

  return EFI_SUCCESS;
}

/**
  Report the SA PCIe initialization code version.

  @param[in] MiscPeiPreMemConfig - Instance of SA_MISC_PEI_PREMEM_CONFIG

  @retval EFI_SUCCESS
**/
EFI_STATUS
ReportPcieVersion (
  IN    SA_MISC_PEI_PREMEM_CONFIG      *MiscPeiPreMemConfig
  )
{
  UINT32             Version;

  DEBUG ((DEBUG_INFO, "***************** System Agent PCIe code version *****************\n"));
  DEBUG ((DEBUG_INFO, "** Major version number is: %3d                                 **\n", PcdGet8 (PcdSiliconInitVersionMajor)));
  DEBUG ((DEBUG_INFO, "** Minor version number is: %3d                                 **\n", PcdGet8 (PcdSiliconInitVersionMinor)));
  DEBUG ((DEBUG_INFO, "** Rev version number is:   %3d                                 **\n", PcdGet8 (PcdSiliconInitVersionRevision)));
  DEBUG ((DEBUG_INFO, "** Build number is:         %3d                                 **\n", PcdGet8 (PcdSiliconInitVersionBuild)));
  DEBUG ((DEBUG_INFO, "******************************************************************\n"));

  Version = (((UINT32) PcdGet8 (PcdSiliconInitVersionMajor)) << 24) |
            (((UINT32) PcdGet8 (PcdSiliconInitVersionMinor)) << 16) |
            (((UINT32) PcdGet8 (PcdSiliconInitVersionRevision)) << 8) |
            (((UINT32) PcdGet8 (PcdSiliconInitVersionBuild)));

  ///
  /// Store SA Reference Code version and SA PCIe code version in scrachpad registers
  ///
  MmioWrite32 ((UINTN) (MiscPeiPreMemConfig->DmiBar) + R_SA_DMIBAR_SCRATCHPAD0_OFFSET, Version);
  MmioWrite32 ((UINTN) (MiscPeiPreMemConfig->DmiBar) + R_SA_DMIBAR_SCRATCHPAD1_OFFSET, Version);
  return EFI_SUCCESS;
}

