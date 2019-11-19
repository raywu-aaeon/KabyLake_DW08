/** @file
  This file is PeiCpuPolicy library.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2019 Intel Corporation.

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

@par Specification
**/
#include "PeiCpuPolicyLibrary.h"
#include <Library/MmPciLib.h>
#include <Library/ConfigBlockLib.h>
#include <Library/PostCodeLib.h>
#include <Library/SaPlatformLib.h>

/**
  Load Config block default

  @param[in] ConfigBlockPointer         Pointer to config block
**/
#ifndef MINTREE_FLAG
VOID
LoadCpuSecurityPreMemConfigDefault (
  IN VOID          *ConfigBlockPointer
  )
{
  CPU_SECURITY_PREMEM_CONFIG  *CpuSecurityPreMemConfig;
  CpuSecurityPreMemConfig = ConfigBlockPointer;

  DEBUG ((DEBUG_INFO, "CpuSecurityPreMemConfig->Header.GuidHob.Name = %g\n", &CpuSecurityPreMemConfig->Header.GuidHob.Name));
  DEBUG ((DEBUG_INFO, "CpuSecurityPreMemConfig->Header.GuidHob.Header.HobLength = 0x%x\n", CpuSecurityPreMemConfig->Header.GuidHob.Header.HobLength));

  /********************************
    CPU Security PreMem configuration
  ********************************/
  //
  // Set PrmrrSize to 1 MB as default.
  //
  CpuSecurityPreMemConfig->PrmrrSize              = SIZE_1MB;
  CpuSecurityPreMemConfig->BiosSize               = (7 * 1024);          /// BIOS Size in KB
  CpuSecurityPreMemConfig->BiosGuard              = CPU_FEATURE_ENABLE;
  CpuSecurityPreMemConfig->FlashWearOutProtection = CPU_FEATURE_ENABLE;
}

/**
  Load Config block default

  @param[in] ConfigBlockPointer         Pointer to config block
**/
VOID
LoadCpuTxtPreMemConfigDefault (
  IN VOID          *ConfigBlockPointer
  )
{
  CPU_TXT_PREMEM_CONFIG  *CpuTxtPreMemConfig;
  CPUID_VERSION_INFO_ECX      Ecx;
  UINT32                      TxtCapableChipset;
  CpuTxtPreMemConfig = ConfigBlockPointer;

  DEBUG ((DEBUG_INFO, "CpuTxtPreMemConfig->Header.GuidHob.Name = %g\n", &CpuTxtPreMemConfig->Header.GuidHob.Name));
  DEBUG ((DEBUG_INFO, "CpuTxtPreMemConfig->Header.GuidHob.Header.HobLength = 0x%x\n", CpuTxtPreMemConfig->Header.GuidHob.Header.HobLength));

  /********************************
    CPU TXT PreMem configuration
  ********************************/
  ///
  /// Execute CPUID and GETSEC[CAPABILITIES] to check both processor and chipset are TXT capable.
  ///
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, &Ecx.Uint32, NULL);
  TxtCapableChipset = CheckSmxCapabilities();
  if ((Ecx.Bits.SMX == 1) && (TxtCapableChipset & BIT0)) {
     CpuTxtPreMemConfig->SinitMemorySize    = TXT_SINIT_MEMORY_SIZE;
     CpuTxtPreMemConfig->TxtHeapMemorySize  = TXT_HEAP_MEMORY_SIZE;
     CpuTxtPreMemConfig->TgaSize            = TXT_TGA_MEMORY_SIZE;
     CpuTxtPreMemConfig->TxtLcpPdBase       = TXT_LCP_PD_BASE;
     CpuTxtPreMemConfig->TxtLcpPdSize       = TXT_LCP_PD_SIZE;
  }
}
#endif

/**
  Load Config block default

  @param[in] ConfigBlockPointer         Pointer to config block
**/
VOID
LoadCpuConfigLibPreMemConfigDefault (
  IN VOID          *ConfigBlockPointer
  )
{
  CPU_CONFIG_LIB_PREMEM_CONFIG  *CpuConfigLibPreMemConfig;
  CPU_FAMILY  CpuFamily;
  CPU_SKU     CpuSku;
  BOOLEAN     PegDisabled;
  UINT64      MchBar;

  CpuConfigLibPreMemConfig = ConfigBlockPointer;

  DEBUG ((DEBUG_INFO, "CpuConfigLibPreMemConfig->Header.GuidHob.Name = %g\n", &CpuConfigLibPreMemConfig->Header.GuidHob.Name));
  DEBUG ((DEBUG_INFO, "CpuConfigLibPreMemConfig->Header.GuidHob.Header.HobLength = 0x%x\n", CpuConfigLibPreMemConfig->Header.GuidHob.Header.HobLength));

  /********************************
    CPU Config Lib PreMem configuration
  ********************************/
  CpuConfigLibPreMemConfig->HyperThreading          = CPU_FEATURE_ENABLE;
  CpuConfigLibPreMemConfig->BootFrequency           = 1;    // Maximum non-turbo Performance
  CpuConfigLibPreMemConfig->ActiveCoreCount         = 0;    // All cores active
  CpuConfigLibPreMemConfig->VmxEnable               = CPU_FEATURE_ENABLE;
  CpuConfigLibPreMemConfig->CpuRatio = RShiftU64 (AsmReadMsr64 (MSR_PLATFORM_INFO), N_PLATFORM_INFO_MAX_RATIO) & B_PLATFORM_INFO_RATIO_MASK;

  ///
  /// FCLK Frequency
  ///
  CpuFamily  = GetCpuFamily();
  CpuSku     = GetCpuSku();
  MchBar = MmioRead32 (MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN) + R_SA_MCHBAR) &~BIT0;
  if (IsPchLinkDmi (CpuFamily) && (MmioRead16 (MmPciBase (SA_PEG_BUS_NUM, SA_PEG_DEV_NUM, SA_PEG10_FUN_NUM) + PCI_VENDOR_ID_OFFSET) != 0xFFFF)) {
    PegDisabled = MmioRead32 ((UINTN) MchBar + R_SA_MCHBAR_BIOS_RESET_CPL_OFFSET) & BIT3;
  } else {
    PegDisabled = 1;
  }

  ///
  /// DT/Halo FCLK = 1GHz
  /// Ulx/Ult FCLK = 800MHz
  ///
  if (((CpuSku == EnumCpuHalo) && (!PegDisabled)) || (CpuSku == EnumCpuTrad)) {
    CpuConfigLibPreMemConfig->FClkFrequency = 1;  // 1Ghz
  } else {
    CpuConfigLibPreMemConfig->FClkFrequency = 0;  // 800MHz
  }

  CpuConfigLibPreMemConfig->PeciC10Reset = 1;  // Disables Peci Reset on C10 exit
}

/**
  Load Overclocking pre-mem Config block default

  @param[in] ConfigBlockPointer         Pointer to config block
**/
VOID
LoadCpuOverclockingPreMemConfigDefault (
  IN VOID          *ConfigBlockPointer
  )
{
  CPU_OVERCLOCKING_PREMEM_CONFIG  *CpuOverclockingPreMemConfig;
  CpuOverclockingPreMemConfig = ConfigBlockPointer;

  DEBUG ((DEBUG_INFO, "CpuOverclockingPreMemConfig->Header.GuidHob.Name = %g\n", &CpuOverclockingPreMemConfig->Header.GuidHob.Name));
  DEBUG ((DEBUG_INFO, "CpuOverclockingPreMemConfig->Header.GuidHob.Header.HobLength = 0x%x\n", CpuOverclockingPreMemConfig->Header.GuidHob.Header.HobLength));

  /********************************
    CPU Overclocking PreMem configuration
  ********************************/
  CpuOverclockingPreMemConfig->RingDownBin  = CPU_FEATURE_ENABLE;
}


static COMPONENT_BLOCK_ENTRY  mCpuIpBlocksPreMem [] = {
#ifndef MINTREE_FLAG
  {&gCpuSecurityPreMemConfigGuid,     sizeof (CPU_SECURITY_PREMEM_CONFIG),      CPU_SECURITY_PREMEM_CONFIG_REVISION,    LoadCpuSecurityPreMemConfigDefault},
  {&gCpuTxtPreMemConfigGuid,          sizeof (CPU_TXT_PREMEM_CONFIG),           CPU_TXT_PREMEM_CONFIG_REVISION,         LoadCpuTxtPreMemConfigDefault},
#endif
  {&gCpuConfigLibPreMemConfigGuid,    sizeof (CPU_CONFIG_LIB_PREMEM_CONFIG),    CPU_CONFIG_LIB_PREMEM_CONFIG_REVISION,  LoadCpuConfigLibPreMemConfigDefault},
  {&gCpuOverclockingPreMemConfigGuid, sizeof (CPU_OVERCLOCKING_PREMEM_CONFIG),  CPU_OVERCLOCKING_PREMEM_CONFIG_REVISION,LoadCpuOverclockingPreMemConfigDefault},
};

/**
  Get CPU PREMEM config block table total size.

  @retval Size of CPU PREMEM config block table
**/
UINT16
EFIAPI
CpuGetPreMemConfigBlockTotalSize (
  VOID
  )
{
  return GetComponentConfigBlockTotalSize (&mCpuIpBlocksPreMem[0], sizeof (mCpuIpBlocksPreMem) / sizeof (COMPONENT_BLOCK_ENTRY));
}

/**
  CpuAddPreMemConfigBlocks add all CPU PREMEM config blocks.

  @param[in] ConfigBlockTableAddress    The pointer to add CPU PREMEM config blocks

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
CpuAddPreMemConfigBlocks (
  IN     VOID      *ConfigBlockTableAddress
  )
{
  EFI_STATUS Status;
  DEBUG((DEBUG_INFO, "CPU Pre-Mem Entry \n"));
  PostCode (0xC00);

  Status = AddComponentConfigBlocks (ConfigBlockTableAddress, &mCpuIpBlocksPreMem[0], sizeof (mCpuIpBlocksPreMem) / sizeof (COMPONENT_BLOCK_ENTRY));
  DEBUG((DEBUG_INFO, "CpuAddPreMemConfigBlocks Done \n"));
  PostCode (0xC0F);

  return Status;
}

