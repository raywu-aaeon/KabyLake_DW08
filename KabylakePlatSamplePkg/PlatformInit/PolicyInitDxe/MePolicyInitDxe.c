/** @file
  This file is SampleCode for Intel ME DXE Policy initialization.

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
#include "MePolicyInitDxe.h"

/**
  Initialize ME DXE Policy

  @param[in] ImageHandle          Image handle of this driver.

  @retval EFI_SUCCESS             Initialization complete.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
MePolicyInitDxe (
  IN EFI_HANDLE                   ImageHandle
  )
{
  EFI_STATUS                Status;
  ME_POLICY_PROTOCOL        *MePolicyProtocol;

  DEBUG ((DEBUG_INFO, "MePolicyInitDxe\n"));
  //
  // Call MeCreatePolicyDefaults to initialize platform policy structure
  // and get all default policy settings.
  //
  Status = MeCreatePolicyDefaults (&MePolicyProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UpdateDxeMePolicyBoardConfig (MePolicyProtocol);

  if (mFirmwareConfiguration != FwConfigDefault) {
    UpdateDxeMePolicy (MePolicyProtocol);
  }
  //
  // Install MeInstallPolicyProtocol.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = MeInstallPolicyProtocol (ImageHandle, MePolicyProtocol);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

