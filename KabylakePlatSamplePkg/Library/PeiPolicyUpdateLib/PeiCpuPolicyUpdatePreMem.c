/** @file
  This file is SampleCode of the library for Intel CPU PEI Policy initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2018 Intel Corporation.

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

#include "PeiCpuPolicyUpdate.h"
#include <CmosMap.h>
#include <Library/ConfigBlockLib.h>

extern EFI_GUID gEpcOsDataGuid;


/**
  Check on the processor if SGX is supported.

  @retval True if SGX supported or FALSE if not
**/
BOOLEAN
IsSgxCapSupported (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegs;

  ///
  /// Processor support SGX feature by reading CPUID.(EAX=7,ECX=0):EBX[2]
  ///
  AsmCpuidEx (CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS, 0, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);

  ///
  /// SGX feature is supported only on SKL and later,
  /// with CPUID.(EAX=7,ECX=0):EBX[2]=1
  /// PRMRR configuration enabled, MSR IA32_MTRRCAP (FEh) [12] == 1
  ///
  if ((CpuidRegs.RegEbx & BIT2) && (AsmReadMsr64 (MSR_IA32_MTRRCAP) & BIT12)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Calculate the smallest supported PRMRR size that will be
  reported in response to a given EPC size request from OS

  @param[in] EpcSize           - EPC size

  @retval    UINT32            - PRMRR size that is just bigger than the requested EPC
**/
UINT32
ConvertEpcToPrmrr (
  UINT32 EpcSize
  )
{
  UINT32 EpcAlignedPowerOfTwo;
  UINT32 SupportedPrmrr;

  if (EpcSize == 0) {
    return 0;
  }

  ///
  /// Round EPC size to the nearest power of two
  ///
  EpcAlignedPowerOfTwo = GetPowerOfTwo32(EpcSize);

  ///
  /// If given EpcSize is not power of two
  ///
  if (EpcAlignedPowerOfTwo < EpcSize) {
    EpcAlignedPowerOfTwo = (UINT32) (UINTN) LShiftU64(EpcAlignedPowerOfTwo,1);
  }

  /*
    Bits 0 through 31 indicate which sizes are supported. Each bit
    represents one size. A bit at position n indicates whether
    size 2^(n+20) bytes is supported
  */
  SupportedPrmrr = (UINT32) (UINTN) LShiftU64 (AsmReadMsr64 (MSR_PRMRR_VALID_CONFIG),20);

  ///
  /// Removing sizes that are lower then EPC round size
  ///
  SupportedPrmrr = SupportedPrmrr & ~(EpcAlignedPowerOfTwo - 1);

  ///
  /// If EPC rounding is bigger than highest supported PRMRR size,
  /// then EPC size that was set by OS is not supported
  ///
  if (SupportedPrmrr < EpcAlignedPowerOfTwo) {
    DEBUG ((DEBUG_INFO, "ConvertEpcToPrmrr: Requested EPC size (rounded up to be aligned to power of two) by OS 0x%016llX, is bigger than supported PRMRRs size 0x%016llX\n" ,EpcAlignedPowerOfTwo ,SupportedPrmrr));
    return 0;
  }

  ///
  /// If PRMRR supported size is bigger then EPC size rounded to two
  /// then return the next lowest PRMRR size supported
  ///
  if ((SupportedPrmrr & EpcAlignedPowerOfTwo) == 0) {
    return (UINT32) (UINTN) (LShiftU64(BIT0,LowBitSet32(SupportedPrmrr)));
  }
  return (SupportedPrmrr & EpcAlignedPowerOfTwo);
}

/**
  Function will update the policy for PRMRR size and enable SGX
  (In case PRMRR size is not zero), by reading the setup option
  value, SGX UEFI variable if exist and if SGX enabling is
  software controlled.

  Function may initiate a cold reset upon the occurring of the
  next scenarios

  1.SGX need to be enabled(PRMRR size is bigger than 0) but MSR
    FEATURE_CONTROL[0x3a] is already locked[0] and SGX bit[18]
    was not set.

  2.PRMRR size need to bigger than it was in previous boot and
    MSR FEATURE_CONTROL[0x3a] is locked[0]

  3.SGX need to be disabled(PRMRR size is 0), but MSR
    FEATURE_CONTROL[0x3a] is already locked[0] and SGX bit[18]
    was set.

  @param[in]  VariableServices
  @param[in]  CpuSetup
  @param[in, out]  CpuSecurityPreMemConfig - will update
                   CpuSecurityPreMemConfig->EnableSgx and
                   CpuSecurityPreMemConfig->PrmrrSize
**/
VOID
EFIAPI
UpdatePrmrrSize (
  IN EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices,
  IN CPU_SETUP                       *CpuSetup,
  IN OUT CPU_SECURITY_PREMEM_CONFIG  *CpuSecurityPreMemConfig
  )
{
  EFI_STATUS                        Status;
  MSR_IA32_FEATURE_CONTROL_REGISTER Msr;
  EPC_OS_CONFIG                     mEpcOsConfiguration;
  UINTN                             mEpcOsConfigurationSize;
  BOOLEAN                           SgxFeatureEnabled;
  BOOLEAN                           FeatureMsrLocked;
  EFI_BOOT_MODE                     BootMode;

  CpuSecurityPreMemConfig->PrmrrSize = 0;
  mEpcOsConfigurationSize = sizeof(EPC_OS_CONFIG);

  Msr.Uint64 = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);
  SgxFeatureEnabled = (BOOLEAN) (Msr.Bits.SgxEnable == 1);
  FeatureMsrLocked  = (BOOLEAN) (Msr.Bits.Lock == 1);

  if (CpuSetup->EnableSgx == SGX_SOFTWARE_CONTROL) {
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 EPC_OS_CTRL,
                                 &gEpcOsDataGuid,
                                 NULL,
                                 &mEpcOsConfigurationSize,
                                 &mEpcOsConfiguration
                                 );

    ///
    /// OS SGX variable was found
    ///
    if (Status == EFI_SUCCESS) {
      ///
      /// BIOS will enable SGX by OS request only if the EPC size that was asked is a valid one
      ///
      mEpcOsConfiguration.RequestedEpcSize = ConvertEpcToPrmrr(mEpcOsConfiguration.RequestedEpcSize);

      Status = PeiServicesGetBootMode (&BootMode);
      ASSERT_EFI_ERROR (Status);

      ///
      /// If EPC size is supported and Boot Mode is valid for SGX Configuration
      ///
      if ((mEpcOsConfiguration.RequestedEpcSize != 0) && (BootMode != BOOT_ON_S3_RESUME) && (BootMode != BOOT_ON_S4_RESUME)) {
        DEBUG ((DEBUG_INFO, "SGX is enabled by OS\n"));

        ///
        /// If feature MSR(0x3a) is locked and SGX feature is not set, the cold reset
        /// need to be done, in order to remove feature control lock and to enable SGX
        /// This scenario may occur if in the previous boot
        /// - SGX was set to software control but there were no UEFI OS variable, so SGX was disabled.
        /// - Previous power cycle was a warm reset, so lock bits were not cleaned.
        /// Now there is such variable, but, so we need to perform a cold reset
        ///
        if (FeatureMsrLocked && (SgxFeatureEnabled == FALSE)) {
          DEBUG ((DEBUG_INFO, "In order to enable SGX, system will perform a cold reset now\n"));
          (*GetPeiServicesTablePointer())->ResetSystem2 (EfiResetCold, EFI_SUCCESS, 0, NULL);
        }

        CpuSecurityPreMemConfig->EnableSgx = CPU_FEATURE_ENABLE;

        DEBUG ((DEBUG_INFO, "PRMRR size that was defined by OS: 0x%X\n", mEpcOsConfiguration.RequestedEpcSize));
        DEBUG ((DEBUG_INFO, "PRMRR size that was defined by setup: 0x%X\n", CpuSetup->PrmrrSize));

        ///
        /// BIOS is setting PRMRR to be the be MAX(EPC that been asked by OS (and converted to PRMRR),PRMRR size in setup options)
        ///
        if (CpuSetup->PrmrrSize < mEpcOsConfiguration.RequestedEpcSize) {
          CpuSetup->PrmrrSize = mEpcOsConfiguration.RequestedEpcSize;
          ///
          /// If PRMRR size need to be changed from what it was in the previous boot
          /// but previous power cycle was a warm one, so need to initiate a cold reset.
          ///
          if (FeatureMsrLocked && SgxFeatureEnabled) {
            DEBUG ((DEBUG_INFO, "In order to change PRMRR size, system will perform a cold reset now\n"));
            (*GetPeiServicesTablePointer())->ResetSystem2 (EfiResetCold, EFI_SUCCESS, 0, NULL);
          }
        }
      } else if (BootMode == BOOT_ON_S3_RESUME) {
        DEBUG ((DEBUG_WARN, "SGX Configuration changes cannot be made on a S3 flow\n"));
      } else if (BootMode == BOOT_ON_S4_RESUME) {
        DEBUG ((DEBUG_WARN, "SGX Configuration changes cannot be made on a S4 flow\n"));
      } else {
        DEBUG ((DEBUG_INFO, "EpcSize of 0x%X is not supported\n", mEpcOsConfiguration.RequestedEpcSize));
      }
    } else {
      DEBUG ((DEBUG_WARN, "OS SGX variable was not found\n"));
    }

    ///
    /// if OS variable was not found or EPC request size was not valid
    ///
    if (CpuSecurityPreMemConfig->EnableSgx == CPU_FEATURE_DISABLE) {

      ///
      /// If SGX was enabled in previous boot, then PRMRR setup option will have a valid size
      ///
      if (CpuSetup->PrmrrSize != 0) {
        CpuSecurityPreMemConfig->EnableSgx = CPU_FEATURE_ENABLE;
      } else {
        CpuSecurityPreMemConfig->EnableSgx = CPU_FEATURE_DISABLE;
      }
    }
  } else {
    DEBUG ((DEBUG_INFO, "SGX is setup controlled\n"));
    CpuSecurityPreMemConfig->EnableSgx = CpuSetup->EnableSgx;
  }

  if (CpuSecurityPreMemConfig->EnableSgx != CPU_FEATURE_ENABLE) {

    ///
    /// If feature MSR(0x3a) is locked and SGX feature is set, then a cold reset
    /// need to be done in order to remove feature control lock and to disable SGX
    /// This Scenario may occur if:
    /// - In Previous boot SGX was enabled
    /// - SGX is software controlled
    /// - Software asked for unsupported EPC size
    /// Need to remove SGX bit, and lock bit, so need to initiate a cold reset
    ///
    if (FeatureMsrLocked && SgxFeatureEnabled) {
      DEBUG ((DEBUG_INFO, "In order to disable SGX, system will perform a cold reset now\n"));
      (*GetPeiServicesTablePointer())->ResetSystem2 (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }
// AMI_OVERRIDE_START >>> EIP494036 - Sgx will not enable successfully when feature control is locked.
  else {
    ///
    /// If feature MSR(0x3a) is locked and SGX feature is not set, the cold reset
    /// need to be done, in order to remove feature control lock and to enable SGX
    ///
    if (FeatureMsrLocked && (SgxFeatureEnabled == FALSE) ) {
      DEBUG ((DEBUG_INFO, "In order to enable SGX, system will perform a cold reset now\n"));
      (*GetPeiServicesTablePointer())->ResetSystem2 (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }
// AMI_OVERRIDE_END <<< EIP494036 - Sgx will not enable successfully when feature control is locked.

  ///
  /// If PRMRR is supported
  /// PRMRR configuration enabled, MSR IA32_MTRRCAP (FEh) [12] == 1
  ///
  if ((AsmReadMsr64(MSR_IA32_MTRRCAP) & BIT12) != 0) {
    if (CpuSecurityPreMemConfig->EnableSgx == CPU_FEATURE_ENABLE) {
      CpuSecurityPreMemConfig->PrmrrSize = CpuSetup->PrmrrSize;

    }
  }

  ///
  /// Check if PRMRR size is not supported.
  /// Bits 0 through 31 indicate which sizes are supported. Each bit represents one size.
  /// A bit at position n indicates whether size 2^(n+20) bytes is supported
  ///
  if ((LShiftU64 (AsmReadMsr64 (MSR_PRMRR_VALID_CONFIG),20) & CpuSecurityPreMemConfig->PrmrrSize) == 0x0) {
    DEBUG ((DEBUG_INFO, "InitializePrm: PRMRR setup size 0x%X\n", CpuSecurityPreMemConfig->PrmrrSize));
    CpuSecurityPreMemConfig->PrmrrSize = 0;
  }
}

/**
  This function performs CPU PEI Policy initialization in Pre-memory.

  @param[in] SiPreMemPolicyPpi     The SI Pre-Mem Policy PPI instance

  @retval EFI_SUCCESS              The PPI is installed and initialized.
  @retval EFI ERRORS               The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES     Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
UpdatePeiCpuPolicyPreMem (
  IN OUT  SI_PREMEM_POLICY_PPI *SiPreMemPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  CPU_SETUP                       CpuSetupBuffer;
  CPU_SETUP                       *CpuSetup;
  EFI_BOOT_MODE                   BootMode;
  UINT32                          PmConA;
  UINT32                          PmConB;
  UINTN                           PmcBaseAddress;
  CPU_SECURITY_PREMEM_CONFIG      *CpuSecurityPreMemConfig;
  CPU_TXT_PREMEM_CONFIG           *CpuTxtPreMemConfig;
  CPU_CONFIG_LIB_PREMEM_CONFIG    *CpuConfigLibPreMemConfig;
  CPU_OVERCLOCKING_PREMEM_CONFIG  *CpuOverClockingPreMemConfig;
  SETUP_DATA                      SetupDataBuffer;
  UINTN                           PchSpiPciBase;
  UINT32                          PchSpiBar0;

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuTxtPreMemConfigGuid, (VOID *) &CpuTxtPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuConfigLibPreMemConfigGuid, (VOID *) &CpuConfigLibPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuOverclockingPreMemConfigGuid, (VOID *) &CpuOverClockingPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "UpdatePeiCpuPolicyPreMem Start\n"));

  //
  // Make sure ReadOnlyVariablePpi is available
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Get Setup Variable
  //
  VariableSize = sizeof (CPU_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"CpuSetup",
                               &gCpuSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &CpuSetupBuffer
                               );
// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.  
#if 0  
  ASSERT_EFI_ERROR (Status);
#else  
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.  

  //
  // Get current boot mode
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  CpuSetup = &CpuSetupBuffer;

  //
  // Get Setup Variable
  //
  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SetupDataBuffer
                               );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Function will update PrmrrSize
  /// If Runtime Memory interface is enabled and OS variable exist, the PRMRR size will be defined by OS
  /// If Runtime Memory interface is not enabled or OS asked for not supported PRMRR size, PRMRR size will be updated by
  /// PRMRR size from setup options
  /// Function will issue a cold reset if such is needed, in order to enable/disable SGX
  ///
  UpdatePrmrrSize(VariableServices, CpuSetup, CpuSecurityPreMemConfig
                   );
  //
  // Update TXT Platform Policy
  //
// AMI_OVERRIDE_START - not use.  
#if 0 
  CpuSecurityPreMemConfig->Txt          = CpuSetup->Txt;
  CpuTxtPreMemConfig->TxtDprMemorySize  = CpuSetup->DprSize * 0x100000;
#endif
// AMI_OVERRIDE_END - not use.

  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );
  PmConA  = MmioRead32 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_A);
  PmConB  = MmioRead32 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B);

  //
  // Check GEN_PMCON_B (PMC PCI offset 0A4) bit[1] - PWR FLR
  //
  if (PmConB & B_PCH_PMC_GEN_PMCON_B_PWR_FLR) {
    //
    // Write 1 to clear PWR_FLR
    //
    MmioOr8 (
      PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B,
      B_PCH_PMC_GEN_PMCON_B_PWR_FLR
      );
  }

  //
  // When the SUS_PWR_FLR bit is set, it indicates VRMPWRGD signal from the CPU VRM went low.
  // Software must clear this bit if set.
  //
  if (PmConB & B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR) {
    //
    // BIOS clears this bit by writing a '1' to it.
    // Take care of keepping HOST_RST_STS bit as '0' when write, so we don't clear it out accidentally.
    //
    MmioAndThenOr8 (
      PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B + 1,
      (UINT8) ~(B_PCH_PMC_GEN_PMCON_B_HOST_RST_STS >> 8),
      B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR >> 8
      );
  }

  if (CpuSetup->SkipStopPbet == 1) {
    CpuSecurityPreMemConfig->SkipStopPbet = TRUE;
  } else {
    CpuSecurityPreMemConfig->SkipStopPbet = FALSE;
  }

  ///
  /// Set Flash Wear-Out Protection policy
  ///
  if ((BootMode == BOOT_ON_FLASH_UPDATE) || (BootMode == BOOT_IN_RECOVERY_MODE)){
    CpuSecurityPreMemConfig->FlashWearOutProtection = CPU_FEATURE_DISABLE;
  } else {
    CpuSecurityPreMemConfig->FlashWearOutProtection = CpuSetup->FlashWearOutProtection;
  }


  CpuSecurityPreMemConfig->BiosGuard  = CpuSetup->BiosGuard;

  PchSpiPciBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_SPI,
                    PCI_FUNCTION_NUMBER_PCH_SPI
                    );
  PchSpiBar0 = MmioRead32 (PchSpiPciBase + R_PCH_SPI_BAR0);
  PchSpiBar0 &= ~(B_PCH_SPI_BAR0_MASK);

  if (PchSpiBar0 == 0) {
    PchSpiBar0 = PCH_SPI_BASE_ADDRESS;
    MmioWrite32 (PchSpiPciBase + R_PCH_SPI_BAR0, PCH_SPI_BASE_ADDRESS);
    MmioOr32 (PchSpiPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
  }

  if ((MmioRead32 (PchSpiBar0 + R_PCH_SPI_HSFSC) & B_PCH_SPI_HSFSC_SAF_MODE_ACTIVE)) {
    DEBUG ((DEBUG_INFO, "SAF mode is active. BIOS Guard has been disabled due to incompatibilty with SAF Mode\n"));
    CpuSecurityPreMemConfig->BiosGuard = CPU_FEATURE_DISABLE;
  }

  CpuSecurityPreMemConfig->ResetAux   = CpuSetup->ResetAux;

  //
  //  Update CpuConfigLibPreMem Config Block data
  //
  CpuConfigLibPreMemConfig->HyperThreading          = CpuSetup->HyperThreading;
  CpuConfigLibPreMemConfig->BootFrequency           = CpuSetup->BootFrequency;
  CpuConfigLibPreMemConfig->ActiveCoreCount         = CpuSetup->ActiveCoreCount;
  CpuConfigLibPreMemConfig->FClkFrequency           = CpuSetup->EpocFclkFreq;
  CpuConfigLibPreMemConfig->JtagC10PowerGateDisable = CpuSetup->JtagC10PowerGateDisable;
  CpuConfigLibPreMemConfig->BistOnReset             = CpuSetup->BistOnReset;
  CpuConfigLibPreMemConfig->VmxEnable               = CpuSetup->VT;
  CpuConfigLibPreMemConfig->CpuRatioOverride        = CpuSetup->CpuRatioOverride;
  CpuConfigLibPreMemConfig->CpuRatio                = CpuSetup->CpuRatio;
  CpuConfigLibPreMemConfig->PeciSxReset             = CpuSetup->PeciSxReset;
  CpuConfigLibPreMemConfig->PeciC10Reset            = CpuSetup->PeciC10Reset;
  //
  //  Update CpuOverclockingPreMemConfig Config  Block data
  //
  CpuOverClockingPreMemConfig->OcSupport  = CpuSetup->OverclockingSupport;
  CpuOverClockingPreMemConfig->OcLock     = CpuSetup->OverclockingLock;

  if (CpuSetup->OverclockingSupport) {
    //
    //  IA Core
    //
    CpuOverClockingPreMemConfig->CoreVoltageMode = CpuSetup->CoreVoltageMode;

    if (CpuOverClockingPreMemConfig->CoreVoltageMode == OC_LIB_OFFSET_ADAPTIVE) {
      CpuOverClockingPreMemConfig->CoreVoltageOverride = 0;
      CpuOverClockingPreMemConfig->CoreVoltageAdaptive = CpuSetup->CoreExtraTurboVoltage;
    } else if (CpuOverClockingPreMemConfig->CoreVoltageMode == OC_LIB_OFFSET_OVERRIDE) {
      CpuOverClockingPreMemConfig->CoreVoltageOverride = CpuSetup->CoreVoltageOverride;
      CpuOverClockingPreMemConfig->CoreVoltageAdaptive = 0;
    } else {
      CpuOverClockingPreMemConfig->CoreVoltageOverride = 0;
      CpuOverClockingPreMemConfig->CoreVoltageAdaptive = 0;
    }

    if (CpuSetup->CoreVoltageOffsetPrefix == 1) {
      // Offset is negative, need to convert
      CpuOverClockingPreMemConfig->CoreVoltageOffset = (INT16)(~CpuSetup->CoreVoltageOffset +1);
    } else {
      CpuOverClockingPreMemConfig->CoreVoltageOffset = CpuSetup->CoreVoltageOffset;
    }
    CpuOverClockingPreMemConfig->CoreMaxOcRatio         = CpuSetup->CoreMaxOcRatio;
    CpuOverClockingPreMemConfig->CorePllVoltageOffset   = CpuSetup->CorePllVoltageOffset;
    CpuOverClockingPreMemConfig->GtPllVoltageOffset     = CpuSetup->GtPllVoltageOffset;
    CpuOverClockingPreMemConfig->RingPllVoltageOffset   = CpuSetup->RingPllVoltageOffset;
    CpuOverClockingPreMemConfig->SaPllVoltageOffset     = CpuSetup->SaPllVoltageOffset;
    CpuOverClockingPreMemConfig->McPllVoltageOffset     = CpuSetup->McPllVoltageOffset;
    CpuOverClockingPreMemConfig->RingMaxOcRatio         = CpuSetup->RingMaxOcRatio;
    CpuOverClockingPreMemConfig->RingMinOcRatio         = CpuSetup->RingMinOcRatio;
    CpuOverClockingPreMemConfig->RingDownBin            = CpuSetup->RingDownBin;
    CpuOverClockingPreMemConfig->Avx2RatioOffset        = CpuSetup->Avx2RatioOffset;
    CpuOverClockingPreMemConfig->BclkAdaptiveVoltage    = CpuSetup->BclkAdaptiveVoltageEnable;
    CpuOverClockingPreMemConfig->TvbRatioClipping       = CpuSetup->TvbRatioClipping;
    CpuOverClockingPreMemConfig->TvbVoltageOptimization = CpuSetup->TvbVoltageOptimization;
    CpuOverClockingPreMemConfig->TjMaxOffset            = CpuSetup->TjMaxOffset;
  }

  return EFI_SUCCESS;
}
