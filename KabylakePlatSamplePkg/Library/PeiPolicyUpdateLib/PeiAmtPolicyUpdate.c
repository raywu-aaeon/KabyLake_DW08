/** @file
  Do Platform Stage AMT initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2016 Intel Corporation.

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

#include "PeiAmtPolicyUpdate.h"

#ifdef AMT_SUPPORT
#include <AmtForcePushPetPolicy.h>
#include <MeBiosExtensionSetup.h>
#include <AlertStandardFormat.h>
#include <Library/ConfigBlockLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED ASF_FRAMEWORK_MESSAGE_TYPE mAmtForcePushPetPolicy[] = {
  AsfMessageChassisIntrusion,
  AsfMessageUserAuthenticationInvalid,
  AsfMessageHddAuthenticationInvalid,
};

/**
  Install the Active Management push PET policy

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
InstallAmtForcePushPetPolicy (
  VOID
  )
{
  VOID        *Hob;

  //
  // Build HOB for setup memory information
  //
  Hob = BuildGuidDataHob (
            &gAmtForcePushPetPolicyGuid,
            &mAmtForcePushPetPolicy,
            sizeof (mAmtForcePushPetPolicy)
            );

  //
  // Cannot Build HOB
  //
  ASSERT (Hob != NULL);

  return EFI_SUCCESS;
}

/**
  Install the Active Management Policy Ppi Library

  @param[in, out] SiPolicyPpi   PEI Si Policy

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
UpdatePeiAmtPolicy (
  IN OUT  SI_POLICY_PPI *SiPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  ME_SETUP                        MeSetup;
  ME_BIOS_EXTENSION_SETUP         MeBiosExtensionSetupData;
  AMT_PEI_CONFIG                  *AmtPeiConfig;

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gAmtPeiConfigGuid, (VOID *) &AmtPeiConfig);
  ASSERT_EFI_ERROR (Status);

  Status = EFI_SUCCESS;
  //
  // Locate Variable Ppi
  //
  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);

  //
  // Make sure we have a PPI, if not, just return.
  //
  if (!VariableServices) {
    return EFI_SUCCESS;
  }
  //
  // Get Setup Variable
  //
  VariableSize = sizeof (ME_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"MeSetup",
                               &gMeSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &MeSetup
                               );

  if (Status == EFI_SUCCESS) {
    AmtPeiConfig->AmtEnabled         = MeSetup.Amt;
    AmtPeiConfig->WatchDog           = MeSetup.WatchDog;
    AmtPeiConfig->WatchDogTimerBios  = MeSetup.WatchDogTimerBios;
    AmtPeiConfig->WatchDogTimerOs    = MeSetup.WatchDogTimerOs;
    AmtPeiConfig->AsfEnabled         = MeSetup.Asf;
    AmtPeiConfig->FwProgress         = MeSetup.FwProgress;
  }

  //
  // Get BIOS Sync-up data from MEBx to Amt Policy
  //
  VariableSize = sizeof (MeBiosExtensionSetupData);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
                               &gMeBiosExtensionSetupGuid,
                               NULL,
                               &VariableSize,
                               &MeBiosExtensionSetupData
                               );
  if (Status != EFI_SUCCESS) {
    AmtPeiConfig->ManageabilityMode = 0;
    AmtPeiConfig->AmtSolEnabled = 0;
  } else {
    AmtPeiConfig->ManageabilityMode = MeBiosExtensionSetupData.PlatformMngSel;
    DEBUG ((DEBUG_INFO, "Mebx set ManageabilityMode to %x at previous boot\n", MeBiosExtensionSetupData.PlatformMngSel));
    AmtPeiConfig->AmtSolEnabled = MeBiosExtensionSetupData.AmtSol;
    DEBUG ((DEBUG_INFO, "Mebx set AmtSol to %x at previous boot \n", MeBiosExtensionSetupData.AmtSol));
  }

  InstallAmtForcePushPetPolicy ();

  return Status;
}
#endif
