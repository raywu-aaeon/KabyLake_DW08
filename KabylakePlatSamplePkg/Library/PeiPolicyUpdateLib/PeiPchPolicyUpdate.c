/** @file
  This file is SampleCode of the library for Intel PCH PEI Policy initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2004 - 2019 Intel Corporation.

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

#include "PeiPchPolicyUpdate.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
// AMI_OVERRIDE_START - For AMI Setup. 
#include <Library/PeiServicesTablePointerLib.h>
// AMI_OVERRIDE_END - For AMI Setup. 
#include <Guid/GlobalVariable.h>
#include <Guid/PlatformEmmcHs400Info.h> // AdvancedFeaturesContent
#include <Library/PchGbeLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchHsioLib.h>
#include <Library/PchSerialIoLib.h>
#include <Library/PchPcieRpLib.h>
#include <Ppi/Spi.h>
#include <Board/BoardConfigInit.h>
#ifndef MINTREE_FLAG
#include <Library/SerialPortParameterLib.h>
#endif //MINTREE_FLAG
#include <GpioConfig.h>
#include <GpioPinsSklH.h>
#include <Library/DebugLib.h>
#include <PlatformBoardConfig.h>
#include <Library/BoardConfigLib.h>
#include <PlatformBoardId.h>
#include <Library/PchGbeLib.h>
// AMI_OVERRIDE_START - For AMI Setup.
#include <SbSetupData.h>
// AMI_OVERRIDE_END - For AMI Setup. 

// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#define CpuSetup PchSetup
#define SetupVariables PchSetup
#define SaSetup PchSetup  
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.

#define PCI_CLASS_NETWORK             0x02
#define PCI_CLASS_NETWORK_ETHERNET    0x00
#define PCI_CLASS_NETWORK_OTHER       0x80

GLOBAL_REMOVE_IF_UNREFERENCED PCH_PCIE_DEVICE_OVERRIDE mPcieDeviceTable[] = {
  //
  // Intel PRO/Wireless
  //
  { 0x8086, 0x422b, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x422c, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x4238, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x4239, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel WiMAX/WiFi Link
  //
  { 0x8086, 0x0082, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0085, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0083, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0084, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0086, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0087, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0088, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0089, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x008F, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0090, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Crane Peak WLAN NIC
  //
  { 0x8086, 0x08AE, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x08AF, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Crane Peak w/BT WLAN NIC
  //
  { 0x8086, 0x0896, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0897, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Kelsey Peak WiFi, WiMax
  //
  { 0x8086, 0x0885, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0886, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 105
  //
  { 0x8086, 0x0894, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0895, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 135
  //
  { 0x8086, 0x0892, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0893, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 2200
  //
  { 0x8086, 0x0890, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0891, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 2230
  //
  { 0x8086, 0x0887, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x0888, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel Centrino Wireless-N 6235
  //
  { 0x8086, 0x088E, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x088F, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel CampPeak 2 Wifi
  //
  { 0x8086, 0x08B5, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  { 0x8086, 0x08B6, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },
  //
  // Intel WilkinsPeak 1 Wifi
  //
  { 0x8086, 0x08B3, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0158, 0x0000000F, 0, 0, 0, 0, 0 },
  { 0x8086, 0x08B4, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0158, 0x0000000F, 0, 0, 0, 0, 0 },
  //
  // Intel Wilkins Peak 2 Wifi
  //
  { 0x8086, 0x08B1, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0158, 0x0000000F, 0, 0, 0, 0, 0 },
  { 0x8086, 0x08B2, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2AndL1SubstatesOverride, 0x0158, 0x0000000F, 0, 0, 0, 0, 0 },
  //
  // Intel Wilkins Peak PF Wifi
  //
  { 0x8086, 0x08B0, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1, PchPcieL1L2Override, 0, 0, 0, 0, 0, 0, 0 },

// AMI_OVERRIDE_START - Create a Pcie device override table for OEM/ODM porting.
#include "AmiPcieDeviceOverrideTable.h"
// AMI_OVERRIDE_END - Create a Pcie device override table for OEM/ODM porting.
  
  //
  // End of Table
  //
  { 0 }
};

/**
  Add verb table helper function.
  This function calculates verbtable number and shows verb table information.

  @param[in,out] VerbTableEntryNum      Input current VerbTable number and output the number after adding new table
  @param[in,out] VerbTableArray         Pointer to array of VerbTable
  @param[in]     VerbTable              VerbTable which is going to add into array
**/
STATIC
VOID
InternalAddVerbTable (
  IN OUT  UINT8                   *VerbTableEntryNum,
  IN OUT  UINT32                  *VerbTableArray,
  IN      HDAUDIO_VERB_TABLE      *VerbTable
  )
{
  if (VerbTable == NULL) {
    DEBUG ((DEBUG_ERROR, "InternalAddVerbTable wrong input: VerbTable == NULL\n"));
    return;
  }

  VerbTableArray[*VerbTableEntryNum] = (UINT32) VerbTable;
  *VerbTableEntryNum += 1;

  DEBUG ((DEBUG_INFO,
    "Add verb table for vendor = 0x%04X devId = 0x%04X (size = %d DWords)\n",
    VerbTable->Header.VendorId,
    VerbTable->Header.DeviceId,
    VerbTable->Header.DataDwords)
    );
}

enum HDAUDIO_CODEC_SELECT {
  PchHdaCodecPlatformOnboard = 0,
  PchHdaCodecExternalKit     = 1
};

/**
  Add verb table function.
  This function update the verb table number and verb table ptr of policy.

  @param[in] HdAudioConfig            HDAudie config block
  @param[in] CodecType                Platform codec type indicator
  @param[in] AudioConnectorType       Platform audio connector type
**/
STATIC
VOID
InternalAddPlatformVerbTables (
  IN  PCH_HDAUDIO_CONFIG              *HdAudioConfig,
  IN  UINT8                           CodecType,
  IN  UINT8                           AudioConnectorType
  )
{
  EFI_STATUS                      Status;
  UINT8                           VerbTableEntryNum;
  UINT32                          VerbTableArray[32];
  UINT32                          *VerbTablePtr;
  BOARD_CONFIG_BLOCK_PEI_POSTMEM  *PlatformBoardConfig;

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPostMemGuid, (VOID **) &PlatformBoardConfig);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }

  VerbTableEntryNum = 0;

  InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->DisplayAudioVerbTable);

  if (CodecType == PchHdaCodecPlatformOnboard) {
    DEBUG ((DEBUG_INFO, "HDA Policy: Onboard codec selected\n"));
    if (PlatformBoardConfig->VerbTable2[0] != NULL) {
      if (AudioConnectorType == 0) { //Type-C Audio connector selected in Bios Setup menu
        InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable2[0]);
        InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable2[1]);
        InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable2[2]);
        DEBUG ((DEBUG_INFO, "HDA: Type-C Audio connector selected!\n"));
      } else { //Stacked Jack Audio connector selected in Bios Setup menu
        InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable1[0]);
        InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable1[1]);
        InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable1[2]);
        DEBUG ((DEBUG_INFO, "HDA: Stacked-Jack Audio connector selected!\n"));
      }
    } else {
      InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable1[0]);
      InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable1[1]);
      InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->VerbTable1[2]);
    }
  } else {
    DEBUG ((DEBUG_INFO, "HDA Policy: External codec kit selected\n"));
    InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->CommonVerbTable[0]);
    InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->CommonVerbTable[1]);
    InternalAddVerbTable (&VerbTableEntryNum, VerbTableArray, PlatformBoardConfig->CommonVerbTable[2]);
  }

  HdAudioConfig->VerbTableEntryNum = VerbTableEntryNum;

  VerbTablePtr = (UINT32 *) AllocateZeroPool (sizeof (UINT32) * VerbTableEntryNum);
  CopyMem (VerbTablePtr, VerbTableArray, sizeof (UINT32) * VerbTableEntryNum);
  HdAudioConfig->VerbTablePtr = (UINT32) VerbTablePtr;
}

/**
  Detect if there is a capsule with CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flag set.

  @retval TRUE        Such capsule is detected.
  @retval FALSE       No such capsules there.

**/
BOOLEAN
IsPopulateCapsuleInSystemTable (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS        HobPointer;
  EFI_CAPSULE_HEADER          *CapsuleHeader;

  //
  // Search all capsule images from hob
  //
  HobPointer.Raw = GetHobList ();
  while ((HobPointer.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, HobPointer.Raw)) != NULL) {
    CapsuleHeader = (EFI_CAPSULE_HEADER *) (UINTN) HobPointer.Capsule->BaseAddress;
    if ((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) != 0) {
      //
      // SPI should keep locked if the capusle is intended to pass info to OS (via system table) rather than update firmware
      //
      DEBUG((DEBUG_INFO, "A Capsule with CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flag is detected.\n"));
      return TRUE;
    }
    HobPointer.Raw = GET_NEXT_HOB (HobPointer);
  }

  return FALSE;
}

/**
  This function performs PCH PEI Policy initialization.

  @param[in, out] SiPolicy        The SI Policy PPI instance

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
UpdatePeiPchPolicy (
  IN OUT      SI_POLICY_PPI     *SiPolicy
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#if 0
  SETUP_DATA                      SetupVariables;
#endif
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
  PCH_SERIES                      PchSeries;

  PCH_GENERAL_CONFIG              *PchGeneralConfig;
  PCH_PCIE_CONFIG                 *PcieRpConfig;
  PCH_SATA_CONFIG                 *SataConfig;
  PCH_IOAPIC_CONFIG               *IoApicConfig;
  PCH_CIO2_CONFIG                 *Cio2Config;
  PCH_DMI_CONFIG                  *DmiConfig;
  PCH_FLASH_PROTECTION_CONFIG     *FlashProtectionConfig;
  PCH_HDAUDIO_CONFIG              *HdAudioConfig;
  PCH_INTERRUPT_CONFIG            *InterruptConfig;
  PCH_ISH_CONFIG                  *IshConfig;
  PCH_LAN_CONFIG                  *LanConfig;
  PCH_LOCK_DOWN_CONFIG            *LockDownConfig;
  PCH_PM_CONFIG                   *PmConfig;
  PCH_PORT61H_SMM_CONFIG          *Port61hConfig;
  PCH_SCS_CONFIG                  *ScsConfig;
  PCH_SERIAL_IO_CONFIG            *SerialIoConfig;
  PCH_LPC_SIRQ_CONFIG             *SerialIrqConfig;
  PCH_THERMAL_CONFIG              *ThermalConfig;
  PCH_USB_CONFIG                  *UsbConfig;
#ifndef MINTREE_FLAG
  UINT8                           Index;
// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#if 0  
  SA_SETUP                        SaSetup;
  CPU_SETUP                       CpuSetup;
  PCH_SETUP                       PchSetup;
#endif
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.  
// AMI_OVERRIDE_START - For AMI TBT support
#if defined Thunderbolt_SUPPORT && Thunderbolt_SUPPORT == 1
  UINT8                           TbtSelector;
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0  
  UINT8                           TbtSelector1;
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.  
#endif
// AMI_OVERRIDE_END - For AMI TBT support  
// AMI_OVERRIDE_START - For AMI Setup.
#if 0  
  PCH_SPI_PPI                     *SpiPpi;
  UINT32                          BaseAddr;
  UINT32                          RegionSize;
#endif
// AMI_OVERRIDE_END - For AMI Setup.  
  UINT32                          CycleRouterNum;
  BOOLEAN                         FlashProtectionEnabled;
  UINT8                           MaxSataPorts;
  UINTN                           MaxPciePorts;
  SATA_THERMAL_THROTTLE           *SataTT;
  THERMAL_THROTTLE_LEVELS         *TTLevels;
  DMI_HW_WIDTH_CONTROL            *DmiHaAWC;
  PLATFORM_EMMC_TUNING_DATA       Hs400TuningInfo;
  BOARD_CONFIG_BLOCK_PEI_POSTMEM  *PlatformBoardConfigPostMem;
  SI_PREMEM_POLICY_PPI            *SiPreMemPolicyPpi;
  CPU_SECURITY_PREMEM_CONFIG      *CpuSecurityPreMemConfig;
  BOARD_PRIVATE_DATA              *BoardInfo;
#endif // MINTREE_FLAG
  HYBRID_STORAGE_CONFIG           *HybridStorageConfig;

// AMI_OVERRIDE_START - For AMI Setup.
  CONST EFI_PEI_SERVICES          **PeiServices;
  EFI_BOOT_MODE                   BootMode;
  SB_SETUP_DATA                   PchSetup = {0};
// AMI_OVERRIDE_START - Fix TBT PCI RP setting may not be applied
  SETUP_DATA                      Setup = {0};
// AMI_OVERRIDE_END - Fix TBT PCI RP setting may not be applied
  
  DEBUG ((DEBUG_INFO, "UpdatePeiPchPolicy() - Start\n"));
  
  PeiServices = GetPeiServicesTablePointer ();  
  GetSbSetupData ((VOID*)PeiServices, &PchSetup, TRUE);
  
  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "Now Boot Mode is %x\n", BootMode));
// AMI_OVERRIDE_END - For AMI Setup.

  Status = EFI_SUCCESS;

  Status = GetConfigBlock ((VOID *) SiPolicy, &gPchGeneralConfigGuid, (VOID *) &PchGeneralConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gPcieRpConfigGuid, (VOID *) &PcieRpConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gSataConfigGuid, (VOID *) &SataConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gIoApicConfigGuid, (VOID *) &IoApicConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gCio2ConfigGuid, (VOID *) &Cio2Config);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gDmiConfigGuid, (VOID *) &DmiConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gFlashProtectionConfigGuid, (VOID *) &FlashProtectionConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gHdAudioConfigGuid, (VOID *) &HdAudioConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gInterruptConfigGuid, (VOID *) &InterruptConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gIshConfigGuid, (VOID *) &IshConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gLanConfigGuid, (VOID *) &LanConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gLockDownConfigGuid, (VOID *) &LockDownConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gPmConfigGuid, (VOID *) &PmConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gPort61ConfigGuid, (VOID *) &Port61hConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gScsConfigGuid, (VOID *) &ScsConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gSerialIoConfigGuid, (VOID *) &SerialIoConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gSerialIrqConfigGuid, (VOID *) &SerialIrqConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gThermalConfigGuid, (VOID *) &ThermalConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gUsbConfigGuid, (VOID *) &UsbConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gHybridStorageConfigGuid, (VOID *) &HybridStorageConfig);
  ASSERT_EFI_ERROR (Status);
#ifndef MINTREE_FLAG
  Status = PeiServicesLocatePpi (
                &gSiPreMemPolicyPpiGuid,
                0,
                NULL,
                (VOID **) &SiPreMemPolicyPpi
                );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
  ASSERT_EFI_ERROR (Status);
#endif // MINTREE_FLAG
  //
  // Retrieve Setup variable
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices       // PPI
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VariableSize,
// AMI_OVERRIDE_START - Fix TBT PCI RP setting may not be applied							   
                               &Setup
// AMI_OVERRIDE_END - Fix TBT PCI RP setting may not be applied							   
                               );
  ASSERT_EFI_ERROR (Status);

// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#if 0
#ifndef MINTREE_FLAG
  VariableSize = sizeof (SA_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"SaSetup",
                               &gSaSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SaSetup
                               );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (CPU_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"CpuSetup",
                               &gCpuSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &CpuSetup
                               );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VariableSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &PchSetup
                               );
  ASSERT_EFI_ERROR (Status);
#endif // MINTREE_FLAG
#endif
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.

  PchSeries = GetPchSeries ();

  //
  // DefaultSvidSid Config
  //
  PchGeneralConfig->SubSystemVendorId = V_PCH_INTEL_VENDOR_ID;
  PchGeneralConfig->SubSystemId       = V_PCH_DEFAULT_SID;

#ifndef MINTREE_FLAG
  //
  // LAN Config
  // Check Gbe Region to determine if hide LAN controller setup option
  //
  if (!PchIsGbeAvailable ()) {
    PchSetup.PchLan = 0;
  }
  LanConfig->Enable        = PchSetup.PchLan;
  LanConfig->K1OffEnable   = PchSetup.PchLanK1Off;

  //
  // DeviceEnables
  //
  SataConfig->Enable       = PchSetup.PchSata;
  PmConfig->LpcClockRun    = PchSetup.PchLpcClockRun;
  PchGeneralConfig->Crid   = PchSetup.PchCrid;

  DEBUG_CODE_BEGIN ();
  //
  //  Kabylake OC board should disable LpcClockRun on debug builds
  //  to resolve debug bios log corruption issue
  //
  BoardInfo = (BOARD_PRIVATE_DATA *) PcdGetPtr (PcdBoardPrivateData);
  if (BoardInfo->BoardIdOrgValue == BoardIdKabyLakeOc) {
    PmConfig->LpcClockRun    = 0;
  }
  DEBUG_CODE_END ();

  //
  // SATA Config
  //
  MaxSataPorts = GetPchMaxSataPortNum ();
  MaxPciePorts = GetPchMaxPciePortNum ();

  SataConfig->SataMode = PchSetup.SataInterfaceMode;

  for (Index = 0; Index < MaxSataPorts; Index++) {
    if (PchSetup.SataTestMode == TRUE) {
      SataConfig->PortSettings[Index].Enable = TRUE;
    } else {
      SataConfig->PortSettings[Index].Enable = PchSetup.SataPort[Index];
    }
    SataConfig->PortSettings[Index].HotPlug          = PchSetup.SataHotPlug[Index];
    SataConfig->PortSettings[Index].SpinUp           = PchSetup.SataSpinUp[Index];
    SataConfig->PortSettings[Index].DevSlp           = PchSetup.PxDevSlp[Index];
    SataConfig->PortSettings[Index].EnableDitoConfig = PchSetup.EnableDitoConfig[Index];
    SataConfig->PortSettings[Index].DmVal            = PchSetup.DmVal[Index];
    SataConfig->PortSettings[Index].DitoVal          = PchSetup.DitoVal[Index];
    SataConfig->PortSettings[Index].SolidStateDrive  = PchSetup.SataType[Index];
  }
  if (PchSeries == PchLp) {
    //
    // Only Mobile boards has interlock switches in SATA Port 0, 1 and 2
    //
    for (Index = 0; Index < MaxSataPorts; Index++) {
      if (SataConfig->PortSettings[Index].HotPlug == TRUE) {
        SataConfig->PortSettings[Index].InterlockSw = PchSetup.SataMechanicalSw[Index];
      }
    }
  }
  SataConfig->Rst.RaidDeviceId       = PchSetup.RaidDeviceId;
  SataConfig->Rst.LegacyOrom         = PchSetup.SataLegacyOrom;
  SataConfig->Rst.Raid0              = PchSetup.SataRaidR0;
  SataConfig->Rst.Raid1              = PchSetup.SataRaidR1;
  SataConfig->Rst.Raid10             = PchSetup.SataRaidR10;
  SataConfig->Rst.Raid5              = PchSetup.SataRaidR5;
  SataConfig->Rst.Irrt               = PchSetup.SataRaidIrrt;
  SataConfig->Rst.OromUiBanner       = PchSetup.SataRaidOub;
  SataConfig->Rst.HddUnlock          = PchSetup.SataHddlk;
  SataConfig->Rst.LedLocate          = PchSetup.SataLedl;
  SataConfig->Rst.IrrtOnly           = PchSetup.SataRaidIooe;
  SataConfig->Rst.SmartStorage       = PchSetup.SataRaidSrt;
  SataConfig->Rst.OromUiDelay        = PchSetup.SataRaidOromDelay;
  SataConfig->Rst.OptaneMemory       = PchSetup.SataRstOptaneMemory;
  SataConfig->Rst.CpuAttachedStorage = PchSetup.SataRstCpuAttachedStorage;

  SataConfig->SalpSupport         = PchSetup.SataSalp;
// AMI_OVERRIDE_START - Add the SATA Controller Speed.
  SataConfig->SpeedLimit          = PchSetup.SataControllerSpeed;
// AMI_OVERRIDE_END - Add the SATA Controller Speed.

  if (PchSetup.PchSata == TRUE && PchSetup.SataInterfaceMode == SATA_MODE_RAID) {
    for (Index = 0; Index < MaxPciePorts; Index++) {
      CycleRouterNum = RstGetCycleRouterNumber (Index);
      if (PchSetup.RstPcieRemapEnabled[Index] && CycleRouterNum != RST_PCIE_STORAGE_CR_INVALID) {
        SataConfig->RstPcieStorageRemap[CycleRouterNum].Enable             = 1;
        SataConfig->RstPcieStorageRemap[CycleRouterNum].RstPcieStoragePort = Index + 1;
      }
    }
  } else {
    for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
      SataConfig->RstPcieStorageRemap[Index].Enable = FALSE;
    }
  }

  //
  // Initiate DMI Configuration
  //
  DmiConfig->DmiAspm = PchSetup.PchDmiAspm;
  if (PchSetup.PchLegacyIoLowLatency) {
    DmiConfig->DmiAspm = 0;
    DmiConfig->LegacyIoLowLatency = TRUE;
  }

  //
  // PCI express config
  //
  Status = GetBoardConfigBlock (&gPlatformBoardConfigPostMemGuid, (VOID **) &PlatformBoardConfigPostMem);
// AMI_OVERRIDE_START - For AMI TBT support
#if defined Thunderbolt_SUPPORT && Thunderbolt_SUPPORT == 1 
// AMI_OVERRIDE_START - Fix TBT PCI RP setting may not be applied
  //
  // "TbtDefaultSelectorInfo" will not be changed via SetupUtility. It only fixed from SDL file.
  // So...........we get TBT selector from "SETUP_DATA" instead of "gPlatformBoardConfigPostMemGuid"
  //
  //TbtSelector = (PlatformBoardConfigPostMem-> TbtDefaultSelectorInfo) - 1;
  TbtSelector = Setup.TbtSelector - 1;
// AMI_OVERRIDE_END - Fix TBT PCI RP setting may not be applied
// AMI_OVERRIDE_START - EIP283878: Provide customers a mechanism to adjust SB PCIE LTR for TBT host
  if ((SetupVariables.TbtSupport == 1) && (TbtSelector <= 24)) {
    PcieRpConfig->RootPort[TbtSelector].MaxPayload = PchPcieMaxPayload128;
    PcieRpConfig->RootPort[TbtSelector].EnableHotplugSmi = FALSE;
    PcieRpConfig->RootPort[TbtSelector].EnableHotPlugSci = FALSE;
    DEBUG ((DEBUG_INFO, "PCIe RootPort[%x]= %x\n",TbtSelector,PcieRpConfig->RootPort[TbtSelector].MaxPayload));
//AMI_OVERRIDE_START - EIP296758 Implement Intel document 563778, version 2
    if ((PchSetup.PchPcieLtrEnable[TbtSelector] == 1 && Setup.TbtWaSwitch == 1)) {
//AMI_OVERRIDE_END - EIP296758 Implement Intel document 563778, version 2
      PcieRpConfig->RootPort[TbtSelector].ForceLtrOverride                  = PcdGet32 (PcdTbtForceLtrOverride);
      PcieRpConfig->RootPort[TbtSelector].SnoopLatencyOverrideMode          = PcdGet8 (PcdTbtSnoopLatencyOverrideMode);
      PcieRpConfig->RootPort[TbtSelector].SnoopLatencyOverrideMultiplier    = PcdGet8 (PcdTbtSnoopLatencyOverrideMultiplier);
      PcieRpConfig->RootPort[TbtSelector].SnoopLatencyOverrideValue         = PcdGet16 (PcdTbtSnoopLatencyOverrideValue);
      PcieRpConfig->RootPort[TbtSelector].NonSnoopLatencyOverrideMode       = PcdGet8 (PcdTbtNonSnoopLatencyOverrideMode);
      PcieRpConfig->RootPort[TbtSelector].NonSnoopLatencyOverrideMultiplier = PcdGet8 (PcdTbtNonSnoopLatencyOverrideMultiplier);
      PcieRpConfig->RootPort[TbtSelector].NonSnoopLatencyOverrideValue      = PcdGet16 (PcdTbtNonSnoopLatencyOverrideValue);
    }
  }
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  if (SetupVariables.Multicontroller) {
    TbtSelector1 = (PlatformBoardConfigPostMem-> TbtDefaultSelectorInfo1) - 1;
    if ((SetupVariables.TbtSupport == 1) && (TbtSelector1 <=20)) {
      PcieRpConfig->RootPort[TbtSelector1].MaxPayload = PchPcieMaxPayload128;
      DEBUG ((DEBUG_INFO, "PCIe RootPort[%x]= %x\n",TbtSelector1,PcieRpConfig->RootPort[TbtSelector1].MaxPayload));
      if ((PchSetup.PchPcieLtrEnable[TbtSelector1] == 1)) {
        PcieRpConfig->RootPort[TbtSelector1].ForceLtrOverride = 1;
        PcieRpConfig->RootPort[TbtSelector1].SnoopLatencyOverrideMode = 1;
        PcieRpConfig->RootPort[TbtSelector1].SnoopLatencyOverrideMultiplier = 2;
        PcieRpConfig->RootPort[TbtSelector1].SnoopLatencyOverrideValue = 85;
        PcieRpConfig->RootPort[TbtSelector1].NonSnoopLatencyOverrideMode = 1;
        PcieRpConfig->RootPort[TbtSelector1].NonSnoopLatencyOverrideMultiplier = 2;
        PcieRpConfig->RootPort[TbtSelector1].NonSnoopLatencyOverrideValue = 85;
      }
    }
  }

// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
#endif // End of Thunderbolt_SUPPORT
// AMI_OVERRIDE_END - For AMI TBT support 
#endif
  PcieRpConfig->DisableRootPortClockGating    = PchSetup.PcieClockGatingDisabled;
  PcieRpConfig->EnablePeerMemoryWrite         = PchSetup.PcieRootPortPeerMemoryWriteEnable;
  PcieRpConfig->ComplianceTestMode            = PchSetup.PcieComplianceTestMode;
  PcieRpConfig->RpFunctionSwap                = PchSetup.RpFunctionSwap;

  for (Index = 0; Index < MaxPciePorts; Index++) {
    PcieRpConfig->RootPort[Index].ClkReqDetect                   = TRUE;
    PcieRpConfig->RootPort[Index].Aspm                           = PchSetup.PcieRootPortAspm[Index];
    PcieRpConfig->RootPort[Index].L1Substates                    = PchSetup.PcieRootPortL1SubStates[Index];
    PcieRpConfig->RootPort[Index].PcieSpeed                      = PchSetup.PcieRootPortSpeed[Index];
    PcieRpConfig->RootPort[Index].Gen3EqPh3Method                = PchSetup.PcieRootPortEqPh3Method[Index];
    PcieRpConfig->RootPort[Index].AcsEnabled                     = PchSetup.PcieRootPortACS[Index];
    PcieRpConfig->RootPort[Index].PmSci                          = PchSetup.PcieRootPortPMCE[Index];
    PcieRpConfig->RootPort[Index].HotPlug                        = PchSetup.PcieRootPortHPE[Index];
    PcieRpConfig->RootPort[Index].AdvancedErrorReporting         = PchSetup.PcieRootPortAER[Index];
    PcieRpConfig->RootPort[Index].UnsupportedRequestReport       = PchSetup.PcieRootPortURE[Index];
    PcieRpConfig->RootPort[Index].FatalErrorReport               = PchSetup.PcieRootPortFEE[Index];
    PcieRpConfig->RootPort[Index].NoFatalErrorReport             = PchSetup.PcieRootPortNFE[Index];
    PcieRpConfig->RootPort[Index].CorrectableErrorReport         = PchSetup.PcieRootPortCEE[Index];
    PcieRpConfig->RootPort[Index].SystemErrorOnFatalError        = PchSetup.PcieRootPortSFE[Index];
    PcieRpConfig->RootPort[Index].SystemErrorOnNonFatalError     = PchSetup.PcieRootPortSNE[Index];
    PcieRpConfig->RootPort[Index].SystemErrorOnCorrectableError  = PchSetup.PcieRootPortSCE[Index];
    PcieRpConfig->RootPort[Index].TransmitterHalfSwing           = PchSetup.PcieRootPortTHS[Index];
    PcieRpConfig->RootPort[Index].Uptp                           = PchSetup.PcieRootPortUptp[Index];
    PcieRpConfig->RootPort[Index].Dptp                           = PchSetup.PcieRootPortDptp[Index];
    PcieRpConfig->RootPort[Index].LtrConfigLock                  = PchSetup.PchPcieLtrConfigLock[Index];
    PcieRpConfig->RootPort[Index].LtrEnable                      = PchSetup.PchPcieLtrEnable[Index];
    PcieRpConfig->RootPort[Index].DetectTimeoutMs                = PchSetup.PcieDetectTimeoutMs[Index];
  }

  for (Index = 0; Index < MaxPciePorts; ++Index) {
    PcieRpConfig->EqPh3LaneParam[Index].Cm  = PchSetup.PcieLaneCm[Index];
    PcieRpConfig->EqPh3LaneParam[Index].Cp  = PchSetup.PcieLaneCp[Index];
  }
  if (PchSetup.PcieSwEqOverride) {
    for (Index = 0; Index < PCH_PCIE_SWEQ_COEFFS_MAX; Index++) {
      PcieRpConfig->SwEqCoeffList[Index].Cm     = PchSetup.PcieSwEqCoeffCm[Index];
      PcieRpConfig->SwEqCoeffList[Index].Cp     = PchSetup.PcieSwEqCoeffCp[Index];
    }
  }
#endif // MINTREE_FLAG

  //
  // CLKREQ# mapping override
  //
#ifndef MINTREE_FLAG
  for (Index = 0; Index < MaxPciePorts; Index++) {
    if (PchSetup.PchPcieClkReqOverride[Index] == 1) {
      DEBUG ((DEBUG_INFO, "PCIe RootPort[%d] CLKREQ# mapping override, DISABLED\n", Index));
      PcieRpConfig->RootPort[Index].ClkReqSupported = FALSE;
    }
    if (PchSetup.PchPcieClkReqOverride[Index] == 2 &&
        (((PchSeries == PchLp) && (PchSetup.PchPcieClkReqNumber[Index] < PCH_LP_PCIE_MAX_CLK_REQ)) ||
         ((PchSeries == PchH)  && (PchSetup.PchPcieClkReqNumber[Index] < PCH_H_PCIE_MAX_CLK_REQ))))
    {
      PcieRpConfig->RootPort[Index].ClkReqSupported = TRUE;
      PcieRpConfig->RootPort[Index].ClkReqNumber = PchSetup.PchPcieClkReqNumber[Index];
      DEBUG ((DEBUG_INFO, "PCIe RootPort[%d] CLKREQ# mapping override, number = %d\n", Index, PcieRpConfig->RootPort[Index].ClkReqNumber));
    }
    if (PcieRpConfig->RootPort[Index].ClkReqSupported == TRUE) {
      PcieRpConfig->RootPort[Index].ClkSrcNumber = PcieRpConfig->RootPort[Index].ClkReqNumber;
    }
    PcieRpConfig->RootPort[Index].ForceClkDisableWhenRpDisable = FALSE;
  }
#endif // MINTREE_FLAG
  PcieRpConfig->PcieDeviceOverrideTablePtr = (UINT32) mPcieDeviceTable;

  //
  // HdAudioConfig
  //
#ifndef MINTREE_FLAG
  HdAudioConfig->Enable               = PchSetup.PchHdAudio;
  HdAudioConfig->DspEnable            = PchSetup.PchHdAudioDsp;
  HdAudioConfig->DspUaaCompliance     = PchSetup.PchHdAudioDspUaaCompliance;
  HdAudioConfig->Pme                  = PchSetup.PchHdAudioPme;
  HdAudioConfig->IoBufferOwnership    = PchSetup.PchHdAudioIoBufferOwnership;
  HdAudioConfig->IoBufferVoltage      = PchSetup.PchHdAudioIoBufferVoltage;
  HdAudioConfig->HdAudioLinkFrequency = PchSetup.PchHdaHdAudioLinkFreq;
  HdAudioConfig->IDispLinkFrequency   = PchSetup.PchHdaIDisplayLinkFreq;
  HdAudioConfig->IDispLinkTmode       = PchSetup.PchHdaIDisplayLinkTmode;
  HdAudioConfig->IDispCodecDisconnect = PchSetup.PchHdaIDisplayCodecDisconnect;

  if (HdAudioConfig->DspUaaCompliance) {
    HdAudioConfig->DspEndpointDmic      = FALSE;
    HdAudioConfig->DspEndpointBluetooth = FALSE;
    HdAudioConfig->DspEndpointI2s       = FALSE;
  } else {
    HdAudioConfig->DspEndpointDmic      = PchSetup.PchHdAudioNhltEndpointDmic;
    HdAudioConfig->DspEndpointBluetooth = PchSetup.PchHdAudioNhltEndpointBluetooth;
    HdAudioConfig->DspEndpointI2s       = PchSetup.PchHdAudioNhltEndpointI2s;
  }

  for (Index = 0; Index < HDAUDIO_FEATURES; Index++) {
    HdAudioConfig->DspFeatureMask |= (UINT32)(PchSetup.PchHdAudioFeature[Index] ? (1 << Index) : 0);
  }

  //
  // Add Verb Table
  //
  if (PchSetup.PchHdAudio) {
// AMI_OVERRIDE_START - Install OEM Verb Table
#ifndef OEM_HDA_VERB_TABLE_INSTALL
    InternalAddPlatformVerbTables (HdAudioConfig, PchSetup.PchHdAudioCodecSelect, SetupVariables.AudioConnector);
#endif
// AMI_OVERRIDE_END - Install OEM Verb Table
  }
#else
  InternalAddPlatformVerbTables (HdAudioConfig, PchHdaCodecPlatformOnboard, SetupVariables.AudioConnector);
#endif // MINTREE_FLAG

#ifndef MINTREE_FLAG
  //
  // LockDown
  //
  FlashProtectionEnabled = PchSetup.FprrEnable;
  if (CpuSecurityPreMemConfig->BiosGuard) {
    DEBUG ((DEBUG_INFO, "Enable InSMM.STS (EISS) and BIOS Lock Enable (BLE) since BIOS Guard is enabled.\n"));
    LockDownConfig->SpiEiss  = TRUE;
    LockDownConfig->BiosLock = TRUE;
  } else {
    LockDownConfig->BiosLock = PchSetup.PchBiosLock;
    LockDownConfig->SpiEiss  = PchSetup.PchBiosLock;
  }

// AMI_OVERRIDE_START - Keep BIOS protection enabled for security.
#if 0
  if ((GetBootModeHob () == BOOT_ON_FLASH_UPDATE) && (!IsPopulateCapsuleInSystemTable ())) {
    DEBUG((DEBUG_INFO,"Disable Flash Protection Range Registers for Capsule Update Process\n"));
    FlashProtectionEnabled = FALSE;
    if (!CpuSecurityPreMemConfig->BiosGuard) {
      DEBUG((DEBUG_INFO,"Disabling BIOS protection for Capsule Update Process\n"));
      LockDownConfig->SpiEiss  = FALSE;
      LockDownConfig->BiosLock = FALSE;
    }
  }
#endif
// AMI_OVERRIDE_END - Keep BIOS protection enabled for security.
// AMI_OVERRIDE_START - Disable to program Flash Protection Range Registers.
#if 0

  //
  // Flash Protection Range Register initialization
  //
  if (FlashProtectionEnabled) {
    Status = PeiServicesLocatePpi (
               &gPchSpiPpiGuid,
               0,
               NULL,
               (VOID **)&SpiPpi
               );

    DEBUG ((DEBUG_INFO, "Enabling Flash Protection Range: LocatePpi gPchSpiPpiGuid = %r\n", Status));
    if (!EFI_ERROR (Status)) {
      //
      // Find the base address for BIOS region
      //
      Status = SpiPpi->GetRegionAddress (SpiPpi, FlashRegionBios, &BaseAddr, &RegionSize);
      DEBUG ((DEBUG_INFO, "BIOS Region: %r - Start Address: 0x%x - Size: 0x%x\n", Status, BaseAddr, RegionSize));
      ASSERT_EFI_ERROR (Status);
      if (!EFI_ERROR (Status)) {
        //
        // Initialize flash protection
        //
        FlashProtectionConfig->ProtectRange[0].WriteProtectionEnable  = TRUE;
        FlashProtectionConfig->ProtectRange[0].ReadProtectionEnable   = FALSE;
        //
        // Assign FPRR ranges
        //
        FlashProtectionConfig->ProtectRange[0].ProtectedRangeBase    = (UINT16)((BaseAddr + FixedPcdGet32 (PcdFlashNvStorageSize)) >> 12);
        FlashProtectionConfig->ProtectRange[0].ProtectedRangeLimit   = (UINT16)(((BaseAddr + RegionSize) - 1) >> 12);
        DEBUG ((DEBUG_INFO, "FlashProtectionConfig->ProtectRange[0].ProtectedRangeBase  %x\n", FlashProtectionConfig->ProtectRange[0].ProtectedRangeBase));
        DEBUG ((DEBUG_INFO, "FlashProtectionConfig->ProtectRange[0].ProtectedRangeLimit %x\n", FlashProtectionConfig->ProtectRange[0].ProtectedRangeLimit));
      }
    }
  }
#endif
// AMI_OVERRIDE_END - Disable to program Flash Protection Range Registers.

  //
  // IOAPIC Config
  //
  IoApicConfig->IoApicEntry24_119 = PchSetup.PchIoApic24119Entries;
  IoApicConfig->BdfValid          = 1;
  IoApicConfig->BusNumber         = 0xF0;
  IoApicConfig->DeviceNumber      = 0x1F;
  IoApicConfig->FunctionNumber    = 0;

  //
  // SerialIo Config
  //
  for (Index=0; Index<PCH_SERIALIO_MAX_CONTROLLERS; Index++) {
    if (PchSetup.PchSerialIoDevice[Index] == 0) {
      SerialIoConfig->DevMode[Index] = PchSerialIoDisabled;
    } else if (PchSetup.PchSerialIoDevice[Index] == 1) {
      SerialIoConfig->DevMode[Index] = PchSerialIoPci;
    } else if (PchSetup.PchSerialIoDevice[Index] == 2) {
      SerialIoConfig->DevMode[Index] = PchSerialIoAcpi;
    }
  }

  SerialIoConfig->Gpio          = PchSetup.PchGpio;
  InterruptConfig->GpioIrqRoute = PchSetup.PchGpioIrqRoute;

  for (Index=0; Index<PCH_SERIALIO_MAX_I2C_CONTROLLERS; Index++) {
    SerialIoConfig->I2cVoltage[Index] = PchSetup.PchI2cVoltageSelect[Index];
  }

  //
  // SPI0 chip select polarity should be adjusted to connected peripheral device, but since we don't have any devices defined yet, let it depend on setup option
  //
  SerialIoConfig->SpiCsPolarity[0] = PchSetup.PchSpiCsPolaritySelect[0];
  //
  // SPI1 chip select polarity must be adjusted to connected peripheral device
  //
  switch (SetupVariables.PchSpi1SensorDevice) {
    case 1: //FPC1011
    case 2: //FPC1020
    case 6: //FPC1021
    default:
      SerialIoConfig->SpiCsPolarity[1] = PchSerialIoCsActiveLow;
      break;
  }

  for (Index=0; Index<PCH_SERIALIO_MAX_UART_CONTROLLERS; Index++) {
    SerialIoConfig->UartHwFlowCtrl[Index] = PchSetup.PchUartHwFlowCtrl[Index];
  }

  if (PcdGetBool (PcdIoExpanderPresent)) {
    //
    // This I2C controller will be used as connection for IO Expander
    //
    SerialIoConfig->DevMode[PchSerialIoIndexI2C4] = PchSerialIoAcpiHidden;
  }

  //
  // I2C4 and I2C5 don't exist in SPT-H chipset
  //
  if (GetPchSeries () == PchH) {
    SerialIoConfig->DevMode[PchSerialIoIndexI2C4] = PchSerialIoDisabled;
    SerialIoConfig->DevMode[PchSerialIoIndexI2C5] = PchSerialIoDisabled;
  }

  if (PchSetup.OsDebugPort != 0) {
    //
    // This UART controller will be used as OS debug port
    //
    SerialIoConfig->DevMode[PchSerialIoIndexUart0 + PchSetup.OsDebugPort - 1] = PchSerialIoLegacyUart;

    SerialIoConfig->DebugUartNumber          = PchSetup.OsDebugPort - 1;
    SerialIoConfig->EnableDebugUartAfterPost = TRUE;
  }
#endif // MINTREE_FLAG

  //
  // If SerialIO UART is disabled in policy but used for debug, then
  // override the policy setting
  //
DEBUG_CODE_BEGIN();
#ifndef MINTREE_FLAG
  if ((PcdGet8 (PcdSerialIoUartDebugEnable) == 1) &&
      SerialIoConfig->DevMode[PchSerialIoIndexUart0 + PcdGet8 (PcdSerialIoUartNumber)] == PchSerialIoDisabled &&
      (GetDebugInterface () & STATUS_CODE_USE_SERIALIO) ) {
#else
if ((PcdGet8 (PcdSerialIoUartDebugEnable) == 1) &&
      SerialIoConfig->DevMode[PchSerialIoIndexUart0 + PcdGet8 (PcdSerialIoUartNumber)] == PchSerialIoDisabled ) {
#endif //MINTREE_FLAG
    SerialIoConfig->DevMode[PchSerialIoIndexUart0 + PcdGet8 (PcdSerialIoUartNumber)] = PchSerialIoLegacyUart;
  }
DEBUG_CODE_END();
#ifndef MINTREE_FLAG
  //
  // Misc PM Config
  //
  PmConfig->PchDeepSxPol                          = PchSetup.DeepSxMode;
  PmConfig->WakeConfig.WolEnableOverride          = PchSetup.PchWakeOnLan;
  PmConfig->WakeConfig.WoWlanEnable               = PchSetup.PchWakeOnWlan;
  PmConfig->WakeConfig.WoWlanDeepSxEnable         = PchSetup.PchWakeOnWlanDeepSx;
  PmConfig->WakeConfig.LanWakeFromDeepSx          = PchSetup.LanWakeFromDeepSx;
  PmConfig->SlpLanLowDc                           = PchSetup.SlpLanLowDc;
  if (SetupVariables.TenSecondPowerButtonEnable == 1) { //Power Button Override Period (PBOP): refer to EDS for detail
                                                        //Encoding:
                                                        //000b - 4 seconds
                                                        //011b - 10 seconds
                                                        //power button behavior will depend on this setting and other features
                                                        //such as power button SMI which will shutdown system immediately when it is enabled in pre-OS.
    PmConfig->PwrBtnOverridePeriod              = 0x3; // 10sec
  } else {
    PmConfig->PwrBtnOverridePeriod              = 0;   // 4sec
  }
  PmConfig->DisableDsxAcPresentPulldown           = PchSetup.DisableDsxAcPresentPulldown;
  PmConfig->EnableTcoTimer                        = PchSetup.EnableTcoTimer;
  PmConfig->SlpS0VmEnable                         = SetupVariables.SlpS0VmSupport;

  //
  // Thermal Config
  //
  if ((SaSetup.MemoryThermalManagement != FALSE) &&
      ((SaSetup.ExttsViaTsOnBoard != FALSE) || (SaSetup.ExttsViaTsOnDimm != FALSE))) {
    ThermalConfig->MemoryThrottling.Enable                                     = TRUE;
    ThermalConfig->MemoryThrottling.TsGpioPinSetting[TsGpioC].PmsyncEnable     = TRUE;
    ThermalConfig->MemoryThrottling.TsGpioPinSetting[TsGpioD].PmsyncEnable     = TRUE;
    ThermalConfig->MemoryThrottling.TsGpioPinSetting[TsGpioC].C0TransmitEnable = TRUE;
    ThermalConfig->MemoryThrottling.TsGpioPinSetting[TsGpioD].C0TransmitEnable = TRUE;
    ThermalConfig->MemoryThrottling.TsGpioPinSetting[TsGpioC].PinSelection     = 1;
    ThermalConfig->MemoryThrottling.TsGpioPinSetting[TsGpioD].PinSelection     = 0;
  } else {
    ThermalConfig->MemoryThrottling.Enable = FALSE;
  }

  ThermalConfig->ThermalDeviceEnable = SetupVariables.ThermalDeviceEnable;

  //
  // Update Hybrid Storage Policies
  //
  HybridStorageConfig->HybridStorageMode         = PchSetup.HybridStorageMode;

  //
  // Program Thermal Throttling Level
  //
  TTLevels = &ThermalConfig->TTLevels;
  TTLevels->SuggestedSetting   = PchSetup.PchTtLevelSuggestSet;
  TTLevels->PchCrossThrottling = PchSetup.PchCrossThrottling;
  TTLevels->TTLock             = PchSetup.PchThrmTtLock;
  TTLevels->TTState13Enable    = PchSetup.PchThrmTtState13Enable;
  TTLevels->TTEnable           = PchSetup.PchThrmTtEnable;
  TTLevels->T2Level            = PchSetup.PchThrmT2Level;
  TTLevels->T1Level            = PchSetup.PchThrmT1Level;
  TTLevels->T0Level            = PchSetup.PchThrmT0Level;

  //
  // Program DMI Thermal Throttling
  //
  DmiHaAWC = &ThermalConfig->DmiHaAWC;
  DmiHaAWC->SuggestedSetting  = PchSetup.PchDmiTsSuggestSet;
  DmiHaAWC->TS3TW             = PchSetup.PchTs3Width;
  DmiHaAWC->TS2TW             = PchSetup.PchTs2Width;
  DmiHaAWC->TS1TW             = PchSetup.PchTs1Width;
  DmiHaAWC->TS0TW             = PchSetup.PchTs0Width;
  DmiHaAWC->DmiTsawEn         = PchSetup.PchDmiTsawEn;

  //
  // Program SATA Thermal Throttling
  //
  SataTT                    = &ThermalConfig->SataTT;
  SataTT->SuggestedSetting  = PchSetup.PchSataTsSuggestSet;
  SataTT->P1TDispFinit      = PchSetup.PchP1TDispFinit;
  SataTT->P1Tinact          = PchSetup.PchP1Tinact;
  SataTT->P1TDisp           = PchSetup.PchP1TDisp;
  SataTT->P1T3M             = PchSetup.PchP1T3M;
  SataTT->P1T2M             = PchSetup.PchP1T2M;
  SataTT->P1T1M             = PchSetup.PchP1T1M;
  SataTT->P0TDispFinit      = PchSetup.PchP0TDispFinit;
  SataTT->P0Tinact          = PchSetup.PchP0Tinact;
  SataTT->P0TDisp           = PchSetup.PchP0TDisp;
  SataTT->P0T3M             = PchSetup.PchP0T3M;
  SataTT->P0T2M             = PchSetup.PchP0T2M;
  SataTT->P0T1M             = PchSetup.PchP0T1M;

  //
  // Initialize Serial IRQ Config
  //
  if (PchSetup.PchSirqMode == 0) {
    SerialIrqConfig->SirqMode = PchQuietMode;
  } else {
    SerialIrqConfig->SirqMode = PchContinuousMode;
  }

  //
  // Port 61h emulation
  //
  Port61hConfig->Enable = PchSetup.PchPort61hSmm;

  //
  // USB configuration
  //
  UsbConfig->DisableComplianceMode = PchSetup.DisableComplianceMode;

  for (Index = 0; Index < GetPchXhciMaxUsb2PortNum (); Index++) {
    UsbConfig->PortUsb20[Index].Enable = PchSetup.PchUsbHsPort[Index];
  }
  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {
    UsbConfig->PortUsb30[Index].Enable = PchSetup.PchUsbSsPort[Index];
  }

  UsbConfig->SsicConfig.SsicPort[0].Enable = PchSetup.UsbSsicEnable[0];
  UsbConfig->SsicConfig.SsicPort[1].Enable = PchSetup.UsbSsicEnable[1];

  //
  // xDCI (USB device) related settings from setup variable
  //
  UsbConfig->XdciConfig.Enable = PchSetup.PchXdciSupport;

  if (PcdGet8 (PcdWwanUsbConfig) && (SetupVariables.WWANEnable == 0)) {
    //
    // Disable High speed port for GNSS disable on WWAN card
    // PcdWwanUsbConfig indicates which port has WWAN (1 based); 0 value indicates no WWAN.
    //
    UsbConfig->PortUsb20[PcdGet8 (PcdWwanUsbConfig) - 1].Enable = FALSE;
  }

  //
  // CIO2 Configuration
  //
  Cio2Config->DeviceEnable             = PchSetup.PchSkyCamCio2Enable;  // AdvancedFeaturesContent
  Cio2Config->SkyCamPortATermOvrEnable = SetupVariables.PchSkyCamPortATermOverride;
  Cio2Config->SkyCamPortBTermOvrEnable = SetupVariables.PchSkyCamPortBTermOverride;
  Cio2Config->SkyCamPortCTermOvrEnable = SetupVariables.PchSkyCamPortCTermOverride;
  Cio2Config->SkyCamPortDTermOvrEnable = SetupVariables.PchSkyCamPortDTermOverride;
  Cio2Config->PortATrimEnable          = SetupVariables.PchSkyCamPortATrimEnable;
  Cio2Config->PortBTrimEnable          = SetupVariables.PchSkyCamPortBTrimEnable;
  Cio2Config->PortCTrimEnable          = SetupVariables.PchSkyCamPortCTrimEnable;
  Cio2Config->PortDTrimEnable          = SetupVariables.PchSkyCamPortDTrimEnable;
  Cio2Config->PortACtleEnable          = SetupVariables.PchSkyCamPortACtleEnable;
  Cio2Config->PortBCtleEnable          = SetupVariables.PchSkyCamPortBCtleEnable;
  Cio2Config->PortCDCtleEnable         = SetupVariables.PchSkyCamPortCDCtleEnable;
  Cio2Config->PortADataTrimValue       = SetupVariables.PchSkyCamPortADataTrimValue;
  Cio2Config->PortBDataTrimValue       = SetupVariables.PchSkyCamPortBDataTrimValue;
  Cio2Config->PortCDDataTrimValue      = SetupVariables.PchSkyCamPortCDDataTrimValue;
  Cio2Config->PortACtleCapValue        = SetupVariables.PchSkyCamPortACtleCapValue;
  Cio2Config->PortBCtleCapValue        = SetupVariables.PchSkyCamPortBCtleCapValue;
  Cio2Config->PortCDCtleCapValue       = SetupVariables.PchSkyCamPortCDCtleCapValue;
  Cio2Config->PortACtleResValue        = SetupVariables.PchSkyCamPortACtleResValue;
  Cio2Config->PortBCtleResValue        = SetupVariables.PchSkyCamPortBCtleResValue;
  Cio2Config->PortCDCtleResValue       = SetupVariables.PchSkyCamPortCDCtleResValue;
  Cio2Config->PortAClkTrimValue        = SetupVariables.PchSkyCamPortAClkTrimValue;
  Cio2Config->PortBClkTrimValue        = SetupVariables.PchSkyCamPortBClkTrimValue;
  Cio2Config->PortCClkTrimValue        = SetupVariables.PchSkyCamPortCClkTrimValue;
  Cio2Config->PortDClkTrimValue        = SetupVariables.PchSkyCamPortDClkTrimValue;

  //
  // SCS Configuration
  //
  ScsConfig->ScsEmmcEnabled      = PchSetup.PchScsEmmcEnabled;
  ScsConfig->ScsSdSwitch         = PchSetup.PchScsSdCardEnabled ? PchScsSdcardMode : PchScsSdDisabled;
  ScsConfig->ScsEmmcHs400Enabled = PchSetup.PchScsEmmcHs400Enabled;

  if ((ScsConfig->ScsEmmcEnabled == TRUE) &&
      (ScsConfig->ScsEmmcHs400Enabled == TRUE)) {

    ScsConfig->ScsEmmcHs400DriverStrength = PchSetup.PchScsEmmcHs400DriverStrength;

    //
    // First Boot or CMOS clear, system boot with Default settings, set tuning required
    //
    // Subsequent Boots, Get Variable 'Hs400TuningData'
    //    - if failed to get variable, set tuning required
    //    - if passed, retrieve Hs400DataValid, Hs400RxStrobe1Dll and Hs400TxDataDll from variable. Set tuning not required.
    //
// AMI_OVERRIDE_START -
#if 0
    if (GetBootModeHob () == BOOT_WITH_DEFAULT_SETTINGS) {
#else
    if (BootMode == BOOT_WITH_DEFAULT_SETTINGS) {
#endif
// AMI_OVERRIDE_END -
      ScsConfig->ScsEmmcHs400TuningRequired = TRUE;
//    ScsConfig->ScsEmmcHs400DllDataValid   = FALSE;
//    ScsConfig->ScsEmmcHs400RxStrobeDll1   = 0;
//    ScsConfig->ScsEmmcHs400TxDataDll      = 0;
    } else {
      VariableSize = sizeof (PLATFORM_EMMC_TUNING_DATA);
      Status = VariableServices->GetVariable (
                                   VariableServices,
                                   HS400_TUNING_DATA_VAR,
                                   &gPlatformEmmcHs400TuningInfoGuid,
                                   NULL,
                                   &VariableSize,
                                   &Hs400TuningInfo
                                   );
      if (Status == EFI_SUCCESS) {
        if (Hs400TuningInfo.Hs400DriverStrength == ScsConfig->ScsEmmcHs400DriverStrength) {
          ScsConfig->ScsEmmcHs400TuningRequired = FALSE;
        } else {
          DEBUG ((DEBUG_INFO, "eMMC: HS400 Driver Strength changed - retuning required!\n"));
          ScsConfig->ScsEmmcHs400TuningRequired = TRUE;
        }
        ScsConfig->ScsEmmcHs400DllDataValid   = Hs400TuningInfo.Hs400DataValid;
        ScsConfig->ScsEmmcHs400RxStrobeDll1   = Hs400TuningInfo.Hs400RxStrobe1Dll;
        ScsConfig->ScsEmmcHs400TxDataDll      = Hs400TuningInfo.Hs400TxDataDll;
      } else {
        ScsConfig->ScsEmmcHs400TuningRequired = TRUE;
//      ScsConfig->ScsEmmcHs400DllDataValid   = FALSE;
//      ScsConfig->ScsEmmcHs400RxStrobeDll1   = 0;
//      ScsConfig->ScsEmmcHs400TxDataDll      = 0;
      }
    }
  }

  //
  // ISH Configuration
  //
  IshConfig->Enable          = PchSetup.PchIshEnable;
  IshConfig->SpiGpioAssign   = PchSetup.PchIshSpiGpioAssign;
  IshConfig->Uart0GpioAssign = PchSetup.PchIshUart0GpioAssign;
  IshConfig->Uart1GpioAssign = PchSetup.PchIshUart1GpioAssign;
  IshConfig->I2c0GpioAssign  = PchSetup.PchIshI2c0GpioAssign;
  IshConfig->I2c1GpioAssign  = PchSetup.PchIshI2c1GpioAssign;
  IshConfig->I2c2GpioAssign  = PchSetup.PchIshI2c2GpioAssign;
  IshConfig->Gp0GpioAssign   = PchSetup.PchIshGp0GpioAssign;
  IshConfig->Gp1GpioAssign   = PchSetup.PchIshGp1GpioAssign;
  IshConfig->Gp2GpioAssign   = PchSetup.PchIshGp2GpioAssign;
  IshConfig->Gp3GpioAssign   = PchSetup.PchIshGp3GpioAssign;
  IshConfig->Gp4GpioAssign   = PchSetup.PchIshGp4GpioAssign;
  IshConfig->Gp5GpioAssign   = PchSetup.PchIshGp5GpioAssign;
  IshConfig->Gp6GpioAssign   = PchSetup.PchIshGp6GpioAssign;
  IshConfig->Gp7GpioAssign   = PchSetup.PchIshGp7GpioAssign;
  IshConfig->PdtUnlock       = PchSetup.PchIshPdtUnlock;

  if ((PchSeries == PchH) && (SerialIoConfig->DevMode[PchSerialIoIndexI2C2] != PchSerialIoDisabled)) {
    //
    // ISH UART0 use the same GPIO pins as SerialIo I2C2 on PCH-H
    //
    IshConfig->Uart0GpioAssign = FALSE;
  }

  if (SerialIoConfig->DevMode[PchSerialIoIndexUart1] != PchSerialIoDisabled) {
    //
    // ISH UART1 use the same GPIO pins as SerialIo UART1
    //
    IshConfig->Uart1GpioAssign = FALSE;
  }

  if (((PchSeries == PchLp) && (SerialIoConfig->DevMode[PchSerialIoIndexI2C5] != PchSerialIoDisabled)) ||
      ((PchSeries == PchH)  && (SerialIoConfig->DevMode[PchSerialIoIndexI2C3] != PchSerialIoDisabled))) {
    //
    // ISH I2C2 use the same GPIO pins as SerialIo I2C5 on PCH-LP / I2C3 on PCH-H
    //
    IshConfig->I2c2GpioAssign = FALSE;
  }
#endif // MINTREE_FLAG

  return Status;
}

