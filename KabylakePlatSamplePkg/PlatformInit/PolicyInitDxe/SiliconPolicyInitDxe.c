/** @file
  This file is SampleCode for Intel Silicon DXE Platform Policy initialzation.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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

@par Specification
**/
#include <SiliconPolicyInitDxe.h>
#include <Library/BaseLib.h>

DXE_SI_POLICY_PROTOCOL mSiPolicyData  = { 0 };

/**
  Initilize Intel Cpu DXE Platform Policy

  @param[in] ImageHandle        Image handle of this driver.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED    The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
SiliconPolicyInitDxe (
  IN EFI_HANDLE       ImageHandle
  )
{
  EFI_STATUS Status;

  mSiPolicyData.Revision                         = DXE_SI_POLICY_PROTOCOL_REVISION;
  mSiPolicyData.SmbiosOemTypeFirmwareVersionInfo = (UINT8) PcdGet8 (PcdSmbiosOemTypeFirmwareVersionInfo);
  ///
  /// Install the DXE_SI_POLICY_PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gDxeSiPolicyProtocolGuid,
                  &mSiPolicyData,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  return Status;
}
