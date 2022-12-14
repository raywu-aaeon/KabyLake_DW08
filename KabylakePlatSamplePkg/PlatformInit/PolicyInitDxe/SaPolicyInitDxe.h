/** @file
  Header file for the SaPolicyInitDxe Driver.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2017 Intel Corporation.

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
#ifndef _SA_POLICY_INIT_DXE_H_
#define _SA_POLICY_INIT_DXE_H_

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <FirwmareConfigurations.h>             // AdvancedFeaturesContent
#include <Protocol/SaPolicy.h>
#include <Library/DxeSaPolicyLib.h>
#include <Library/DxePolicyBoardConfigLib.h>    // AdvancedFeaturesContent
#include <Library/DxeSaPolicyUpdateLib.h>       // AdvancedFeaturesContent

#include <SaAccess.h>

extern UINT8 mFirmwareConfiguration;            // AdvancedFeaturesContent

/**
  <b>SA DXE Policy Driver Entry Point</b> \n
  - <b>Introduction</b> \n
    System Agent DXE drivers behavior can be controlled by platform policy without modifying reference code directly.
    Platform policy Protocol is initialized with default settings in this funciton.
    This policy Protocol has to be initialized prior to System Agent initialization DXE drivers execution.

  - @pre
    - Runtime variable service should be ready if policy initialization required.

  - @result
    SA_POLICY_PROTOCOL will be installed successfully and ready for System Agent reference code use.

  - <b>Porting Recommendations</b> \n
    Policy should be initialized basing on platform design or user selection (like BIOS Setup Menu)

  @param[in] ImageHandle - Image handle of this driver.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED    The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
SaPolicyInitDxe (
  IN EFI_HANDLE           ImageHandle
  );

#endif
