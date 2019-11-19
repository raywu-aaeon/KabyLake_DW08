/** @file
  This file is SampleCode of the library for Intel CPU PEI Debug Policy initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2018 Intel Corporation.

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
#include "PeiPolicyDebug.h"
#include <Library/ConfigBlockLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/PcdLib.h>
#include <Library/MmPciLib.h>
#include <Library/IoLib.h>

/**
  This function performs CPU PEI Debug Policy initialization.

  @param[in] SiPolicyPpi          The SI Policy PPI instance

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
**/
EFI_STATUS
EFIAPI
UpdatePeiCpuPolicyDebug (
  IN OUT  SI_POLICY_PPI  *SiPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  CPU_SETUP                       CpuSetupDataBuffer;
  CPU_SETUP                       *CpuSetup;
  CPU_TEST_CONFIG                 *CpuTestConfig;
  CPU_POWER_MGMT_TEST_CONFIG      *CpuPowerMgmtTestConfig;
  UINT8                           CpuSku;
  CPU_FAMILY                      CpuFamily;
  UINT32                          CpuDid;

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuTestConfigGuid, (VOID *) &CpuTestConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuPowerMgmtTestConfigGuid, (VOID *) &CpuPowerMgmtTestConfig);
  ASSERT_EFI_ERROR (Status);

  CpuDid = MmioRead16 (MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN) + R_SA_MC_DEVICE_ID);

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
                               &CpuSetupDataBuffer
                               );
  DEBUG(( DEBUG_INFO, "Get Variable status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);

  CpuSetup = &CpuSetupDataBuffer;

  CpuTestConfig->ApHandoffManner                      = CpuSetup->ApHandoffManner;
  CpuTestConfig->ProcTraceOutputScheme                = CpuSetup->ProcTraceOutputScheme;
  CpuTestConfig->ProcTraceEnable                      = CpuSetup->ProcTraceEnable;
  CpuTestConfig->DebugInterfaceEnable                 = CpuSetup->DebugInterfaceEnable;
  CpuTestConfig->DebugInterfaceLockEnable             = CpuSetup->DebugInterfaceLockEnable;
  CpuTestConfig->ApIdleManner                         = CpuSetup->ApIdleManner;
  CpuTestConfig->ProcTraceMemSize                     = CpuSetup->ProcTraceMemSize;
  CpuTestConfig->MachineCheckEnable                   = CpuSetup->MachineCheck;
  CpuTestConfig->MonitorMwaitEnable                   = CpuSetup->MonitorMwait;
  CpuTestConfig->MlcStreamerPrefetcher                = CpuSetup->HwPrefetcher;
  CpuTestConfig->MlcSpatialPrefetcher                 = CpuSetup->AclPrefetch;

  PcdSet8S (PcdCpuApLoopMode, (UINT8) CpuTestConfig->ApIdleManner);

  if(CpuSetup->VoltageOptimization == 2){ // AUTO
  CpuSku = GetCpuSku();
    if ((CpuSku == EnumCpuUlx) || (CpuSku == EnumCpuUlt)){
      CpuTestConfig->VoltageOptimization = TRUE;
     } else {
      CpuTestConfig->VoltageOptimization = FALSE;
    }
  } else {
    CpuTestConfig->VoltageOptimization                  = CpuSetup->VoltageOptimization;
  }
  CpuTestConfig->ThreeStrikeCounterDisable            = CpuSetup->ThreeStrikeCounterDisable;
  CpuPowerMgmtTestConfig->CstCfgCtrIoMwaitRedirection = CpuSetup->PmgCstCfgCtrIoMwaitRedirection;
  CpuPowerMgmtTestConfig->PpmIrmSetting               = CpuSetup->InterruptRedirectMode;
  CpuPowerMgmtTestConfig->Cx                          = CpuSetup->EnableCx;
  CpuPowerMgmtTestConfig->Eist                        = CpuSetup->EnableGv;
  CpuPowerMgmtTestConfig->EnergyEfficientPState       = CpuSetup->EnergyEfficientPState;
  CpuPowerMgmtTestConfig->CStatePreWake               = CpuSetup->CStatePreWake;
  CpuPowerMgmtTestConfig->TimedMwait                  = CpuSetup->TimedMwait;
  CpuPowerMgmtTestConfig->PmgCstCfgCtrlLock           = CpuSetup->PmgCstCfgCtrlLock;
  CpuPowerMgmtTestConfig->ProcHotLock                 = CpuSetup->ProcHotLock;
  CpuPowerMgmtTestConfig->BiProcHot                   = CpuSetup->EnableProcHot;
  CpuPowerMgmtTestConfig->DisableProcHotOut           = CpuSetup->DisableProcHotOut;
  CpuPowerMgmtTestConfig->DisableVrThermalAlert       = CpuSetup->DisableVrThermalAlert;
  CpuPowerMgmtTestConfig->ProcHotResponse             = CpuSetup->ProcHotResponse;
  CpuPowerMgmtTestConfig->AutoThermalReporting        = CpuSetup->AutoThermalReporting;
  CpuPowerMgmtTestConfig->TStates                     = CpuSetup->TStatesEnable;
  CpuPowerMgmtTestConfig->ThermalMonitor              = CpuSetup->EnableThermalMonitor;
  CpuPowerMgmtTestConfig->ConfigTdpLevel              = CpuSetup->ConfigTdpLevel;
  CpuPowerMgmtTestConfig->RaceToHalt                  = CpuSetup->RaceToHalt;
  CpuPowerMgmtTestConfig->PkgCStateLimit              = CpuSetup->PkgCStateLimit;
  if (CpuSetup->EnergyEfficientTurbo < 2) { ///< If setup is not AUTO, otherwise override with Si init values.
    CpuPowerMgmtTestConfig->EnergyEfficientTurbo      = CpuSetup->EnergyEfficientTurbo;
  }
  if (CpuPowerMgmtTestConfig->Cx) {
    CpuPowerMgmtTestConfig->C1e            = CpuSetup->EnableC1e;
    //
    // Clear C1 & C3 Auto demotion policy
    //
    CpuPowerMgmtTestConfig->C1AutoDemotion = FALSE;
    CpuPowerMgmtTestConfig->C3AutoDemotion = FALSE;

    switch (CpuSetup->CStateAutoDemotion) {
    case 0:
      //
      // Disable C1 and C3 Auto-demotion
      //
      break;

    case 1:
      //
      // Enable C3/C6/C7 Auto-demotion to C1
      //
      CpuPowerMgmtTestConfig->C1AutoDemotion = TRUE;
      break;

    case 2:
      //
      // Enable C6/C7 Auto-demotion to C3
      //
      CpuPowerMgmtTestConfig->C3AutoDemotion = TRUE;
      break;

    default:

    case 3:
      //
      // Enable C6/C7 Auto-demotion to C1 and C3
      //
      CpuPowerMgmtTestConfig->C1AutoDemotion = TRUE;
      CpuPowerMgmtTestConfig->C3AutoDemotion = TRUE;
      break;
    }
    //
    // Configure Un-demotion.
    //
      CpuPowerMgmtTestConfig->C1UnDemotion   = FALSE;
      CpuPowerMgmtTestConfig->C3UnDemotion   = FALSE;

    switch (CpuSetup->CStateUnDemotion) {
      case 0:
        //
        // Disable C1 and C3 Un-demotion
        //
        break;

      case 1:
        //
        // Enable C1 Un-demotion
        //
        CpuPowerMgmtTestConfig->C1UnDemotion = TRUE;
        break;

      case 2:
        //
        // Enable C3 Un-demotion
        //
        CpuPowerMgmtTestConfig->C3UnDemotion = TRUE;
        break;

      case 3:
        //
        // Enable C1 and C3 Un-demotion
        //
        CpuPowerMgmtTestConfig->C1UnDemotion = TRUE;
        CpuPowerMgmtTestConfig->C3UnDemotion = TRUE;
        break;

      default:
        break;
    }
    //
    // Pkg C-state Demotion/Un Demotion
    //
    CpuFamily = GetCpuFamily ();
    if(CpuSetup->PkgCStateDemotion != 0x2) {
      CpuPowerMgmtTestConfig->PkgCStateDemotion = CpuSetup->PkgCStateDemotion;
    } else {
      if((CpuFamily == EnumCpuSklUltUlx) || (CpuFamily == EnumCpuSklDtHalo)){
        CpuPowerMgmtTestConfig->PkgCStateDemotion = 0x1;
      } else if((CpuFamily == EnumCpuKblUltUlx) || (CpuFamily == EnumCpuKblDtHalo)) {
        CpuPowerMgmtTestConfig->PkgCStateDemotion = 0x0;
      }
    }
    if(CpuSetup->PkgCStateUnDemotion != 0x2) {
      CpuPowerMgmtTestConfig->PkgCStateUnDemotion = CpuSetup->PkgCStateUnDemotion;
    } else {
      if((CpuFamily == EnumCpuSklUltUlx) || (CpuFamily == EnumCpuSklDtHalo)){
        CpuPowerMgmtTestConfig->PkgCStateUnDemotion = 0x1;
      } else if((CpuFamily == EnumCpuKblUltUlx) || (CpuFamily == EnumCpuKblDtHalo)) {
        CpuPowerMgmtTestConfig->PkgCStateUnDemotion = 0x0;
      }
    }
    //
    // Update the Latency values for msr 0x60a, 0x60b, 0x60c
    //
    CpuPowerMgmtTestConfig->CstateLatencyControl0TimeUnit      = CpuSetup->CstateLatencyControl0TimeUnit;
    CpuPowerMgmtTestConfig->CstateLatencyControl1TimeUnit      = CpuSetup->CstateLatencyControl1TimeUnit;
    CpuPowerMgmtTestConfig->CstateLatencyControl2TimeUnit      = CpuSetup->CstateLatencyControl2TimeUnit;
    CpuPowerMgmtTestConfig->CstateLatencyControl0Irtl          = CpuSetup->CstateLatencyControl0Irtl;
    CpuPowerMgmtTestConfig->CstateLatencyControl1Irtl          = CpuSetup->CstateLatencyControl1Irtl;
    CpuPowerMgmtTestConfig->CstateLatencyControl2Irtl          = CpuSetup->CstateLatencyControl2Irtl;
    CpuPowerMgmtTestConfig->CstateLatencyControl3TimeUnit      = CpuSetup->CstateLatencyControl3TimeUnit;
    CpuPowerMgmtTestConfig->CstateLatencyControl4TimeUnit      = CpuSetup->CstateLatencyControl4TimeUnit;
    CpuPowerMgmtTestConfig->CstateLatencyControl5TimeUnit      = CpuSetup->CstateLatencyControl5TimeUnit;
    CpuPowerMgmtTestConfig->CstateLatencyControl3Irtl          = CpuSetup->CstateLatencyControl3Irtl;
    CpuPowerMgmtTestConfig->CstateLatencyControl4Irtl          = CpuSetup->CstateLatencyControl4Irtl;
    CpuPowerMgmtTestConfig->CstateLatencyControl5Irtl          = CpuSetup->CstateLatencyControl5Irtl;
  }

  return EFI_SUCCESS;
}