/** @file

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

#include "PeiMePolicyUpdate.h"
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <SetupVariable.h>
#include <Library/ConfigBlockLib.h>

/**
  Update the ME Policy Library

  @param[in] SiPreMemPolicyPpi  The pointer to SiPreMemPolicyPpi

  @retval EFI_SUCCESS           Update complete.
**/
EFI_STATUS
UpdatePeiMePolicyPreMem (
  IN OUT SI_PREMEM_POLICY_PPI     *SiPreMemPolicyPpi
  )
{
  EFI_STATUS                      Status;
  ME_SETUP                        MeSetup;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  UINTN                           VariableSize;
  ME_PEI_PREMEM_CONFIG            *MePeiPreMemConfig;

  DEBUG ((DEBUG_INFO, "UpdatePeiMePolicyPreMem\n"));

  Status = EFI_SUCCESS;
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &Variable
             );
  if (!EFI_ERROR (Status)) {
    VariableSize = sizeof (ME_SETUP);
    Status = Variable->GetVariable (
                         Variable,
                         L"MeSetup",
                         &gMeSetupVariableGuid,
                         NULL,
                         &VariableSize,
                         &MeSetup
                         );
    if (!EFI_ERROR (Status)) {
      Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMePeiPreMemConfigGuid, (VOID *) &MePeiPreMemConfig);
      ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_START - Force TpmDeviceSelection to 0 without PTT_SUPPORT.
#if PTT_SUPPORT
      MePeiPreMemConfig->TpmDeviceSelection = MeSetup.TpmDeviceSelection;
#else
      MePeiPreMemConfig->TpmDeviceSelection = 0;
#endif      
// AMI_OVERRIDE_END - Force TpmDeviceSelection to 0 without PTT_SUPPORT.

      MePeiPreMemConfig->HeciTimeouts = MeSetup.HeciTimeouts;

      // <Test policies>
      MePeiPreMemConfig->DidInitStat = MeSetup.DidInitStat;
      MePeiPreMemConfig->DisableCpuReplacedPolling = MeSetup.DisableCpuReplacedPolling;
      MePeiPreMemConfig->SendDidMsg = MeSetup.SendDidMsg;
      MePeiPreMemConfig->DisableHeciRetry = MeSetup.DisableHeciRetry;
      MePeiPreMemConfig->DisableMessageCheck = MeSetup.DisableMessageCheck;
      MePeiPreMemConfig->SkipMbpHob = MeSetup.SkipMbpHob;

      MePeiPreMemConfig->HeciCommunication2 = MeSetup.HeciCommunication2;
      MePeiPreMemConfig->KtDeviceEnable = MeSetup.KtDeviceEnable;
      MePeiPreMemConfig->IderDeviceEnable = MeSetup.IderDeviceEnable;
    }
  }

  return Status;
}

