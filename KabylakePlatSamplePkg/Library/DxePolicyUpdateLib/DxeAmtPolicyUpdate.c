/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2012 - 2016 Intel Corporation.

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

#include "DxeAmtPolicyUpdate.h"

/**
  Update AMT Policy while MePlatformProtocol is installed.

  @param[in, out] DxeAmtPolicy       Instance of AMT Policy Protocol

**/
VOID
UpdateAmtPolicyFromMeSetup (
  IN OUT  AMT_POLICY_PROTOCOL   *DxeAmtPolicy
  )
{
  EFI_STATUS                Status;
  ME_SETUP                  MeSetup;
  UINTN                     VariableSize;
  UINT32                    VariableAttributes;
  AMT_DXE_CONFIG            *AmtDxeConfig;

  DEBUG ((DEBUG_INFO, "UpdateAmtPolicyFromMeSetup\n"));

  VariableSize = sizeof (ME_SETUP);

  //
  // Get Management configuration from Setup Data
  //
  Status = gRT->GetVariable (
                  L"MeSetup",
                  &gMeSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &MeSetup
                  );

  if (!EFI_ERROR (Status)) {
    Status = GetConfigBlock ((VOID *) DxeAmtPolicy, &gAmtDxeConfigGuid, (VOID *) &AmtDxeConfig);
    ASSERT_EFI_ERROR (Status);

    AmtDxeConfig->CiraRequest               = MeSetup.AmtCiraRequest;
    AmtDxeConfig->CiraTimeout               = MeSetup.AmtCiraTimeout;
    AmtDxeConfig->UnConfigureMe             = MeSetup.UnConfigureMe;
    AmtDxeConfig->HideUnConfigureMeConfirm  = MeSetup.HideUnConfigureMeConfirm;
    AmtDxeConfig->MebxDebugMsg              = MeSetup.MebxDebugMsg;
    AmtDxeConfig->UsbProvision              = MeSetup.UsbProvision;
    AmtDxeConfig->AmtbxSelectionScreen      = MeSetup.AmtbxSelectionScreen;
    AmtDxeConfig->AmtbxHotkeyPressed        = MeSetup.AmtbxHotKeyPressed;
    AmtDxeConfig->MebxNonUiTextMode         = MeSetup.MebxNonUiTextMode;
    AmtDxeConfig->MebxUiTextMode            = MeSetup.MebxUiTextMode;
    AmtDxeConfig->MebxGraphicsMode          = MeSetup.MebxGraphicsMode;

  }

  DEBUG ((DEBUG_INFO, "UpdateAmtPolicyFromMeSetup done, Status: %r\n", Status));
}

/**
  Install DxeAmtPolicy with setup values

  @param[in, out] DxeAmtPolicy  DXE Amt Policy

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
UpdateDxeAmtPolicy (
  IN OUT  AMT_POLICY_PROTOCOL   *DxeAmtPolicy
  )
{
  UpdateAmtPolicyFromMeSetup (DxeAmtPolicy);

  return EFI_SUCCESS;
}
