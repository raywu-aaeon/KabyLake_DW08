/** @file
  This file is SampleCode of the library for Intel PCH PEI Debug Policy initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2017 Intel Corporation.

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
#include <Library/PeiPolicyUpdateLib/PeiPchPolicyUpdate.h>
#include <PiPei.h>
#include <SetupVariable.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <PchAccess.h>
#include <Ppi/SiPolicy.h>
#include <Library/PchInfoLib.h>
// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#include <SbSetupData.h>
#include <Library/PeiServicesTablePointerLib.h>
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.

/**
  This function performs PCH PEI Debug Policy initialization.

  @param[in, out] SiPolicy   The SI Policy PPI instance

  @retval EFI_SUCCESS        The PPI is installed and initialized.
  @retval EFI ERRORS         The PPI is not successfully installed.
**/
EFI_STATUS
EFIAPI
UpdatePeiPchPolicyDebug (
  IN OUT  SI_POLICY_PPI  *SiPolicy
  )
{
  EFI_STATUS                      Status;
// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#if 0  
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  SETUP_DATA                      SetupData;
  UINT8                           Index;
  PCH_SETUP                       PchSetup;
#else
  SB_SETUP_DATA                   PchSetup = {0};
  CONST EFI_PEI_SERVICES          **PeiServices;
  UINT8                           Index;
#endif 
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
  PCH_PCIE_CONFIG                 *PcieRpConfig;
  PCH_SATA_CONFIG                 *SataConfig;
  PCH_LOCK_DOWN_CONFIG            *LockDownConfig;
  PCH_PM_CONFIG                   *PmConfig;
  PCH_P2SB_CONFIG                 *P2sbConfig;
  PCH_DMI_CONFIG                  *DmiConfig;
// AMI_OVERRIDE_START - EIP283878: Provide customers a mechanism to adjust SB PCIE LTR for TBT host
#if defined Thunderbolt_SUPPORT && Thunderbolt_SUPPORT == 1
  SETUP_DATA                      TbtSetup;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  BOOLEAN                         SkipPcieRpPolicyUpdate = FALSE;
#endif


#if defined Thunderbolt_SUPPORT && Thunderbolt_SUPPORT == 1
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &TbtSetup
                               );
  ASSERT_EFI_ERROR (Status);

  if (TbtSetup.TbtSupport == 1 && TbtSetup.TbtSelector <= 24) {
    SkipPcieRpPolicyUpdate = TRUE;
  }
#endif
// AMI_OVERRIDE_END - EIP283878: Provide customers a mechanism to adjust SB PCIE LTR for TBT host

  Status = GetConfigBlock ((VOID *) SiPolicy, &gPcieRpConfigGuid, (VOID *) &PcieRpConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gSataConfigGuid, (VOID *) &SataConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gLockDownConfigGuid, (VOID *) &LockDownConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gPmConfigGuid, (VOID *) &PmConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gP2sbConfigGuid, (VOID *) &P2sbConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gDmiConfigGuid, (VOID *) &DmiConfig);
  ASSERT_EFI_ERROR (Status);

// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#if 0
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

  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                  VariableServices,
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupData
                  );
  ASSERT_EFI_ERROR (Status);
#else
  PeiServices = GetPeiServicesTablePointer ();
  GetSbSetupData ((VOID*)PeiServices, &PchSetup, TRUE);
  Status = EFI_SUCCESS;
#endif 
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.

  //
  // PCI express config
  //
  PcieRpConfig->EnablePort8xhDecode           = PchSetup.PcieRootPort8xhDecode;
  PcieRpConfig->PchPciePort8xhDecodePortIndex = PchSetup.Pcie8xhDecodePortIndex;

  //
  // SATA Config
  //
  SataConfig->TestMode = PchSetup.SataTestMode;

  //
  // LockDown
  //
  LockDownConfig->RtcLock       = PchSetup.PchRtcLock;

  //
  // Misc PM Config
  //
  PmConfig->DisableEnergyReport           = PchSetup.PchDisableEnergyReport;

  if(PchSetup.PchP2sbUnlock) {
    P2sbConfig->SbiUnlock = 1;
    P2sbConfig->SbAccessUnlock = 1;
  } else {
    P2sbConfig->SbiUnlock = 0;
    P2sbConfig->SbAccessUnlock = 0;
  }

  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    if (PchSetup.PchPcieLtrEnable[Index] == TRUE) {
// AMI_OVERRIDE_START - EIP283878: Provide customers a mechanism to adjust SB PCIE LTR for TBT host
#if defined Thunderbolt_SUPPORT && Thunderbolt_SUPPORT == 1
//AMI_OVERRIDE_START - EIP296758 Implement Intel document 563778, version 2
      //
      // TBT PCIE Bus, do not update them with the setup value
      // Override LTR at PeiPchPolicyUpdate.c, this is Thunderbolt Workaround version #1.
      //
      if (SkipPcieRpPolicyUpdate && Index == TbtSetup.TbtSelector - 1 && TbtSetup.TbtWaSwitch == 1) {
//AMI_OVERRIDE_END - EIP296758 Implement Intel document 563778, version 2
        DEBUG ((DEBUG_INFO, "Skip PCI-E #%d\n", Index + 1));
        continue;
      }
#endif
// AMI_OVERRIDE_END - EIP283878: Provide customers a mechanism to adjust SB PCIE LTR for TBT host
      PcieRpConfig->RootPort[Index].SnoopLatencyOverrideMode          = PchSetup.PchPcieSnoopLatencyOverrideMode[Index];
      PcieRpConfig->RootPort[Index].SnoopLatencyOverrideMultiplier    = PchSetup.PchPcieSnoopLatencyOverrideMultiplier[Index];
      PcieRpConfig->RootPort[Index].NonSnoopLatencyOverrideMode       = PchSetup.PchPcieNonSnoopLatencyOverrideMode[Index];
      PcieRpConfig->RootPort[Index].NonSnoopLatencyOverrideMultiplier = PchSetup.PchPcieNonSnoopLatencyOverrideMultiplier[Index];
      PcieRpConfig->RootPort[Index].SnoopLatencyOverrideValue         = PchSetup.PchPcieSnoopLatencyOverrideValue[Index];
      PcieRpConfig->RootPort[Index].NonSnoopLatencyOverrideValue      = PchSetup.PchPcieNonSnoopLatencyOverrideValue[Index];
      PcieRpConfig->RootPort[Index].ForceLtrOverride                  = !!PchSetup.PchPcieForceLtrOverride[Index];
      DmiConfig->PwrOptEnable = TRUE;
    }
  }

  return Status;
}
