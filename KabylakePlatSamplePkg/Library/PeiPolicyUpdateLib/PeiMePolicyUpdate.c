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
#include <Library/PchPmcLib.h>

/**
  Update the ME Policy Library

  @param[in, out] SiPolicyPpi     The pointer to SiPolicyPpi

  @retval EFI_SUCCESS             Update complete.
**/
EFI_STATUS
UpdatePeiMePolicy (
  IN OUT SI_POLICY_PPI            *SiPolicyPpi
  )
{
  EFI_STATUS                      Status;
  ME_SETUP                        MeSetup;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  UINTN                           VariableSize;
  ME_PEI_CONFIG                   *MePeiConfig;

  DEBUG ((DEBUG_INFO, "UpdatePeiMePolicy\n"));

  Status = EFI_SUCCESS;
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &Variable
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gMePeiConfigGuid, (VOID *) &MePeiConfig);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

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
    MePeiConfig->EndOfPostMessage     = MeSetup.EndOfPostMessage;
    MePeiConfig->Heci3Enabled         = MeSetup.Heci3Enabled;
    MePeiConfig->MeUnconfigOnRtcClear = MeSetup.MeUnconfigOnRtcClear;
  }
  if (!PchIsRtcBatteryGood ()) {
    //
    // For non coin battery design, this can be skipped.
    //
    MePeiConfig->MeUnconfigIsValid    = 0;
  }

  return Status;
}

