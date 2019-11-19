/** @file
  CPU PEI Policy Update & initialization.

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
// AMI_OVERRIDE_START - AMI don't support SecurityPkg.
#if 0
#include <Library/Tpm2CommandLib.h>
#include <Library/Tpm12CommandLib.h>
#endif
// AMI_OVERRIDE_END - AMI don't support SecurityPkg.
#include <Library/ConfigBlockLib.h>
#include <Library/HobLib.h>
#include <Board/BoardConfigInit.h>
#include <Library/PcdLib.h>
#include "PlatformBoardId.h"

/**
  Update Cpu Power Management Policy settings according to the related BIOS Setup options

  @param[in] SiPolicyPpi           The SI Policy PPI instance
  @param[in] CpuSetup              The Setup variables instance

  @retval EFI_SUCCESS              The PPI is installed and initialized.
  @retval EFI ERRORS               The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES     Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
InitCpuPmConfigBySetupValues (
  IN OUT  SI_POLICY_PPI            *SiPolicyPpi,
  IN SETUP_DATA                    *SetupData,
  IN CPU_SETUP                     *CpuSetup
  )
{
  UINT8                            Index;
  UINT8                            MinRatio;
  BOARD_PRIVATE_DATA               *BoardInfo;
  CPU_POWER_MGMT_BASIC_CONFIG      *CpuPowerMgmtBasicConfig;
  CPU_POWER_MGMT_VR_CONFIG         *CpuPowerMgmtVrConfig;
  CPU_POWER_MGMT_CUSTOM_CONFIG     *CpuPowerMgmtCustomConfig;
  CPU_POWER_MGMT_PSYS_CONFIG       *CpuPowerMgmtPsysConfig;
  CPU_OVERCLOCKING_PREMEM_CONFIG   *CpuOverClockingConfig;
  SI_PREMEM_POLICY_PPI             *SiPreMemPolicyPpi;
  EFI_STATUS                       Status;
  UINT8                            MaxRatio;

  Status = PeiServicesLocatePpi (
                &gSiPreMemPolicyPpiGuid,
                0,
                NULL,
                (VOID **) &SiPreMemPolicyPpi
                );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuPowerMgmtBasicConfigGuid, (VOID *) &CpuPowerMgmtBasicConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuPowerMgmtVrConfigGuid, (VOID *) &CpuPowerMgmtVrConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuPowerMgmtCustomConfigGuid, (VOID *) &CpuPowerMgmtCustomConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuPowerMgmtPsysConfigGuid, (VOID *) &CpuPowerMgmtPsysConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuOverclockingPreMemConfigGuid, (VOID *) &CpuOverClockingConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Get Maximum Efficiency ratio(Minimum Ratio) from Platform Info MSR Bits[47:40]
  //
// AMI_OVERRIDE_START - fixed build error when OPTIMIZATION = 0.
  MinRatio  = (UINT8) ((RShiftU64 (AsmReadMsr64 (MSR_PLATFORM_INFO), 40) & 0xFF));
// AMI_OVERRIDE_END - fixed build error when OPTIMIZATION = 0.

  CpuPowerMgmtBasicConfig->TurboMode        = CpuSetup->TurboMode;
  CpuPowerMgmtBasicConfig->PowerLimit2      = CpuSetup->PowerLimit2;
  CpuPowerMgmtCustomConfig->ConfigTdpLock   = CpuSetup->ConfigTdpLock;
// AMI_OVERRIDE_START - Use TOKEN to support Dptf_SUPPORT enabled/disabled.
#if defined (Dptf_SUPPORT) && Dptf_SUPPORT == 1   
  if (CpuSetup->ConfigTdpLock == 1 || SetupData->EnableDptf == 1) {
#else
  if (CpuSetup->ConfigTdpLock == 1) {
#endif
// AMI_OVERRIDE_END - Use TOKEN to support Dptf_SUPPORT enabled/disabled.
    CpuPowerMgmtCustomConfig->ConfigTdpBios          = 0;
  } else {
    CpuPowerMgmtCustomConfig->ConfigTdpBios          = CpuSetup->ConfigTdpBios;
  }

  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[0].CustomPowerLimit1 = (UINT16) (CpuSetup->Custom1PowerLimit1Power / 125);
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[0].CustomPowerLimit2 = (UINT16) (CpuSetup->Custom1PowerLimit2Power / 125);
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[0].CustomPowerLimit1Time = CpuSetup->Custom1PowerLimit1Time;
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[0].CustomTurboActivationRatio = CpuSetup->Custom1TurboActivationRatio;
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[1].CustomPowerLimit1 = (UINT16) (CpuSetup->Custom2PowerLimit1Power / 125);
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[1].CustomPowerLimit2 = (UINT16) (CpuSetup->Custom2PowerLimit2Power / 125);
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[1].CustomPowerLimit1Time = CpuSetup->Custom2PowerLimit1Time;
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[1].CustomTurboActivationRatio = CpuSetup->Custom2TurboActivationRatio;
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[2].CustomPowerLimit1 = (UINT16) (CpuSetup->Custom3PowerLimit1Power / 125);
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[2].CustomPowerLimit2 = (UINT16) (CpuSetup->Custom3PowerLimit2Power / 125);
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[2].CustomPowerLimit1Time = CpuSetup->Custom3PowerLimit1Time;
  CpuPowerMgmtCustomConfig->CustomConfigTdpTable[2].CustomTurboActivationRatio = CpuSetup->Custom3TurboActivationRatio;

  //
  // Turbo Mode setting
  //
  if (CpuSetup->LongDurationPwrLimitOverride) {
    CpuPowerMgmtBasicConfig->PowerLimit1      = (UINT16) (CpuSetup->PowerLimit1 / 125);
    CpuPowerMgmtBasicConfig->PowerLimit1Time  = CpuSetup->PowerLimit1Time;
  }

  CpuPowerMgmtBasicConfig->PowerLimit2Power     = (UINT16) (CpuSetup->PowerLimit2Power / 125);
  CpuPowerMgmtBasicConfig->TurboPowerLimitLock  = CpuSetup->TurboPowerLimitLock;

  if (CpuSetup->PowerLimit3Override) {
    CpuPowerMgmtBasicConfig->PowerLimit3           = (UINT16) (CpuSetup->PowerLimit3 / 125);
    CpuPowerMgmtBasicConfig->PowerLimit3Time       = CpuSetup->PowerLimit3Time;
    CpuPowerMgmtBasicConfig->PowerLimit3DutyCycle  = CpuSetup->PowerLimit3DutyCycle;
    CpuPowerMgmtBasicConfig->PowerLimit3Lock       = CpuSetup->PowerLimit3Lock;
  }

  if (CpuSetup->PowerLimit4Override) {
    CpuPowerMgmtBasicConfig->PowerLimit4           = (UINT16) (CpuSetup->PowerLimit4 / 125);
    CpuPowerMgmtBasicConfig->PowerLimit4Lock       = CpuSetup->PowerLimit4Lock;
  }

  CpuPowerMgmtPsysConfig->PsysPowerLimit1        = CpuSetup->PlatformPowerLimit1Enable;
  CpuPowerMgmtPsysConfig->PsysPowerLimit2        = CpuSetup->PlatformPowerLimit2Enable;

  CpuPowerMgmtPsysConfig->PsysPowerLimit1Power   = (UINT16) (CpuSetup->PlatformPowerLimit1Power / 125);
  CpuPowerMgmtPsysConfig->PsysPowerLimit1Time    = CpuSetup->PlatformPowerLimit1Time;
  CpuPowerMgmtPsysConfig->PsysPowerLimit2Power   = (UINT16) (CpuSetup->PlatformPowerLimit2Power / 125);

  //
  // Customize HDC
  //
  CpuPowerMgmtBasicConfig->HdcControl     = CpuSetup->HdcControl;

  //
  // Thermal Function Enables / Configuration
  //
  CpuPowerMgmtBasicConfig->Hwp            = CpuSetup->EnableHwp;

  //
  // Custom VID table
  //
  if (CpuSetup->StateRatio[0] < MinRatio) {
    CpuPowerMgmtCustomConfig->CustomRatioTable.MaxRatio = MinRatio;
  } else {
    CpuPowerMgmtCustomConfig->CustomRatioTable.MaxRatio = CpuSetup->StateRatio[0];
  }

  CpuPowerMgmtCustomConfig->CustomRatioTable.NumberOfEntries = CpuSetup->NumOfCustomPStates;

  for (Index = 0; Index < CpuSetup->NumOfCustomPStates; Index++) {
    if (CpuSetup->StateRatio[Index] < MinRatio) {
      CpuPowerMgmtCustomConfig->CustomRatioTable.StateRatio[Index] = MinRatio;
    } else {
      CpuPowerMgmtCustomConfig->CustomRatioTable.StateRatio[Index] = CpuSetup->StateRatio[Index];
    }
  }

  //
  // Update Turbo Ratio limit override table
  //
  MaxRatio  = (UINT8) ((AsmReadMsr64 (MSR_PLATFORM_INFO) >> 8) & 0xFF);

  if (CpuSetup->RatioLimit1 >= MaxRatio) {
    CpuPowerMgmtBasicConfig->OneCoreRatioLimit = CpuSetup->RatioLimit1;
  }
  if (CpuSetup->RatioLimit2 >= MaxRatio) {
    CpuPowerMgmtBasicConfig->TwoCoreRatioLimit = CpuSetup->RatioLimit2;
  }
  if (CpuSetup->RatioLimit3 >= MaxRatio) {
    CpuPowerMgmtBasicConfig->ThreeCoreRatioLimit = CpuSetup->RatioLimit3;
  }
  if (CpuSetup->RatioLimit4 >= MaxRatio) {
    CpuPowerMgmtBasicConfig->FourCoreRatioLimit = CpuSetup->RatioLimit4;
  }
  if (CpuSetup->RatioLimit5 >= MaxRatio) {
    CpuPowerMgmtBasicConfig->FiveCoreRatioLimit = CpuSetup->RatioLimit5;
  }
  if (CpuSetup->RatioLimit6 >= MaxRatio) {
    CpuPowerMgmtBasicConfig->SixCoreRatioLimit = CpuSetup->RatioLimit6;
  }
  if (CpuSetup->RatioLimit7 >= MaxRatio) {
    CpuPowerMgmtBasicConfig->SevenCoreRatioLimit = CpuSetup->RatioLimit7;
  }
  if (CpuSetup->RatioLimit8 >= MaxRatio) {
    CpuPowerMgmtBasicConfig->EightCoreRatioLimit = CpuSetup->RatioLimit8;
  }

  //
  // Skylake VR Configuration
  //
  CpuPowerMgmtVrConfig->PsysSlope    = CpuSetup->PsysSlope;
  CpuPowerMgmtVrConfig->PsysOffset   = CpuSetup->PsysOffset;
  CpuPowerMgmtPsysConfig->PsysPmax   = CpuSetup->PsysPmax;

  for (Index = 0; Index < MAX_NUM_VRS; Index++) {
    //
    //  Only update if the user wants to override VR settings
    //
    if (CpuSetup->VrConfigEnable[Index] != 0) {
      CpuPowerMgmtVrConfig->VrConfigEnable[Index]  = CpuSetup->VrConfigEnable[Index];
      CpuPowerMgmtVrConfig->Psi1Threshold[Index]   = CpuSetup->Psi1Threshold[Index];
      CpuPowerMgmtVrConfig->Psi2Threshold[Index]   = CpuSetup->Psi2Threshold[Index];
      CpuPowerMgmtVrConfig->Psi3Threshold[Index]   = CpuSetup->Psi3Threshold[Index];
      CpuPowerMgmtVrConfig->Psi3Enable[Index]      = CpuSetup->Psi3Enable[Index];
      CpuPowerMgmtVrConfig->Psi4Enable[Index]      = CpuSetup->Psi4Enable[Index];
      CpuPowerMgmtVrConfig->ImonSlope[Index]       = CpuSetup->ImonSlope[Index];
      if (CpuSetup->ImonOffsetPrefix[Index] == 1) {
        //
        // Number is negative, need to convert to 2's complement representation
        //
        CpuPowerMgmtVrConfig->ImonOffset[Index] = ~CpuSetup->ImonOffset[Index] + 1;
      } else {
        CpuPowerMgmtVrConfig->ImonOffset[Index] = CpuSetup->ImonOffset[Index];
      }
      CpuPowerMgmtVrConfig->VrVoltageLimit[Index]  = CpuSetup->VrVoltageLimit[Index];
      //
      // Only update if IccMax is non-zero. This is to distinguish between the default EDS override.
      //
      if (CpuSetup->IccMax[Index] != 0) {
        CpuPowerMgmtVrConfig->IccMax[Index]          = CpuSetup->IccMax[Index];
      }
      //
      // Only update TDC if power limit is non-zero. TDC Lock should be separated to allow
      // locking of TDC feature.
      //
      if (CpuSetup->TdcPowerLimit[Index] != 0) {
        CpuPowerMgmtVrConfig->TdcPowerLimit[Index]   = CpuSetup->TdcPowerLimit[Index];
        CpuPowerMgmtVrConfig->TdcTimeWindow[Index]   = CpuSetup->TdcTimeWindow[Index];
      }
      CpuPowerMgmtVrConfig->TdcEnable[Index]       = CpuSetup->TdcEnable[Index];
      CpuPowerMgmtVrConfig->TdcLock[Index]         = CpuSetup->TdcLock[Index];

    if ((CpuOverClockingConfig->OcSupport == 1) && (CpuSetup->TdcPowerLimit[Index] == 0)) {
      CpuPowerMgmtVrConfig->TdcEnable[Index]       = 0;
      CpuPowerMgmtVrConfig->TdcPowerLimit[Index]   = 0;
    }

    }
    // Need to check if the user intends to override through CpuSetup to distinguish
    // between the default EDS override.
    if (CpuSetup->AcLoadline[Index] != 0) {
      CpuPowerMgmtVrConfig->AcLoadline[Index]      = CpuSetup->AcLoadline[Index];
    }
    if (CpuSetup->DcLoadline[Index] != 0) {
      CpuPowerMgmtVrConfig->DcLoadline[Index]      = CpuSetup->DcLoadline[Index];
    }
  }

  //
  // Skylake VR Acoustic Noise Mitigation
  //
  CpuPowerMgmtVrConfig->AcousticNoiseMitigation = CpuSetup->AcousticNoiseMitigation;
  CpuPowerMgmtVrConfig->FastPkgCRampDisableIa   = CpuSetup->FastPkgCRampDisableIa;
  CpuPowerMgmtVrConfig->FastPkgCRampDisableGt   = CpuSetup->FastPkgCRampDisableGt;
  CpuPowerMgmtVrConfig->FastPkgCRampDisableSa   = CpuSetup->FastPkgCRampDisableSa;
  CpuPowerMgmtVrConfig->SlowSlewRateForIa       = CpuSetup->SlowSlewRateForIa;
  CpuPowerMgmtVrConfig->SlowSlewRateForGt       = CpuSetup->SlowSlewRateForGt;
  CpuPowerMgmtVrConfig->SlowSlewRateForSa       = CpuSetup->SlowSlewRateForSa;

  //
  // VR Commands from Setup option
  //
  CpuPowerMgmtVrConfig->SendVrMbxCmd1           = CpuSetup->VrMbxCmd;
  CpuPowerMgmtVrConfig->IslVrCmd                = CpuSetup->IslVrCmd;
  CpuPowerMgmtVrConfig->VrPowerDeliveryDesign   = CpuSetup->VrPowerDeliveryDesign;

  //
  // Get the board ID to properly update the VrPowerDeliveryDesign item
  // VrPowerDeliveryDesign is only needed on Coffeelake-S boards.
  //
  if (CpuPowerMgmtVrConfig->VrPowerDeliveryDesign == 0) {
    BoardInfo = (BOARD_PRIVATE_DATA *) PcdGetPtr (PcdBoardPrivateData);
    switch (BoardInfo->BoardId) {
      case BoardIdSkylakeDtRvp8Crb:
      case BoardIdSkylakeAioRvp10Crb:
      case BoardIdSkylakeAioRvp10CrbPpv:
      case BoardIdSkylakeAioRvp10Evp:
      case BoardIdSkylakeAioRvp10Erb:
      case BoardIdKabyLakeSDdr4UdimmEvCrb:
      case BoardIdKabyLakeSDdr4UdimmCrb:
      case BoardIdKabylakeSUdimmCpv:
      case BoardIdKabyLakeOc:    
        CpuPowerMgmtVrConfig->VrPowerDeliveryDesign = EnumCflS95Watt62PowerDesign;
        break;

      default:
        CpuPowerMgmtVrConfig->VrPowerDeliveryDesign = EnumUnknownCpuVrPowerDesign;
        break;
    }
  }

  return EFI_SUCCESS;
}

/**
  This function performs CPU PEI Policy initialization.

  @param[in] SiPolicyPpi           The SI Policy PPI instance

  @retval EFI_SUCCESS              The PPI is installed and initialized.
  @retval EFI ERRORS               The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES     Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
UpdatePeiCpuPolicy (
  IN OUT  SI_POLICY_PPI   *SiPolicyPpi
  )
{
  EFI_STATUS                       Status;
  CPU_CONFIG                       *CpuConfig;
  CPU_SGX_CONFIG                   *CpuSgxConfig;
  CPU_SECURITY_PREMEM_CONFIG       *CpuSecurityPreMemConfig;
  CPU_POWER_MGMT_BASIC_CONFIG      *CpuPowerMgmtBasicConfig;
  SI_PREMEM_POLICY_PPI             *SiPreMemPolicyPpi;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  UINTN                            VariableSize;
  SETUP_DATA                       SetupDataBuffer;
  SETUP_DATA                       *SetupData;
  CPU_SETUP                        CpuSetupBuffer;
  CPU_SETUP                        *CpuSetup;

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

  VariableSize = sizeof (CPU_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"CpuSetup",
                               &gCpuSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &CpuSetupBuffer
                               );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Status = PeiServicesLocatePpi (
                &gSiPreMemPolicyPpiGuid,
                0,
                NULL,
                (VOID **) &SiPreMemPolicyPpi
                );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuConfigGuid, (VOID *) &CpuConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuPowerMgmtBasicConfigGuid, (VOID *) &CpuPowerMgmtBasicConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuSgxConfigGuid, (VOID *) &CpuSgxConfig);
  ASSERT_EFI_ERROR (Status);

  SetupData = &SetupDataBuffer;
  CpuSetup  = &CpuSetupBuffer;

// AMI_OVERRIDE_START - AES will be defined to 1 in the Tpm20.h.
#if 0
  CpuConfig->AesEnable                                 = CpuSetup->AES;
#else
  CpuConfig->AesEnable                                 = CpuSetup->AesEnable;
#endif
// AMI_OVERRIDE_END - AES will be defined to 1 in the Tpm20.h.
  CpuConfig->EnableDts                                 = CpuSetup->EnableDigitalThermalSensor;
// AMI_OVERRIDE_START - Fix to build error when it doesn't support OverClock.
#if defined(OVER_CLOCK_SUPPORT) && (OVER_CLOCK_SUPPORT == 1)
// AMI_OVERRIDE_END - Fix to build error when it doesn't support OverClock.
  CpuConfig->EnableRsr                                 = CpuSetup->EnableRsr;
// AMI_OVERRIDE_START - Fix to build error when it doesn't support OverClock.
#endif
// AMI_OVERRIDE_END - Fix to build error when it doesn't support OverClock.
  CpuConfig->TxtEnable                                 = CpuSetup->Txt;

  CpuPowerMgmtBasicConfig->TccActivationOffset         = CpuSetup->TCCActivationOffset;
  CpuPowerMgmtBasicConfig->TccOffsetTimeWindowForRatl  = CpuSetup->TccOffsetTimeWindow;
  CpuPowerMgmtBasicConfig->TccOffsetClamp              = CpuSetup->TccOffsetClamp;
  CpuPowerMgmtBasicConfig->TccOffsetLock               = CpuSetup->TccOffsetLock;

  //
  // Init Power Management Policy Variables based on setup values
  //
  InitCpuPmConfigBySetupValues (SiPolicyPpi, SetupData, CpuSetup);

  if (CpuSecurityPreMemConfig->EnableSgx == CPU_FEATURE_ENABLE) {
    DEBUG ((DEBUG_INFO, "SGX policy was enabled, updating EPOCHS values \n"));
    CpuSgxConfig->SgxEpoch0          = CpuSetup->SgxEpoch0;
    CpuSgxConfig->SgxEpoch1          = CpuSetup->SgxEpoch1;
  }

  return EFI_SUCCESS;
}
