/** @file
  Implementation of Fsp AMT Policy Initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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

#include <PeiFspPolicyInitLib.h>
#include <Ppi/SiPolicy.h>

/**
  Performs FSP AMT PEI Policy initialization.

  @param[in][out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspAmtPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  EFI_STATUS                Status;
  SI_POLICY_PPI             *SiPolicyPpi;
  AMT_PEI_CONFIG            *AmtPeiConfig;

  DEBUG ((DEBUG_INFO, "PeiFspAmtPolicyInit \n"));
  //
  // Locate SiPolicyPpi
  //
  SiPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (
             &gSiPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPolicyPpi
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gAmtPeiConfigGuid, (VOID *) &AmtPeiConfig);
  ASSERT_EFI_ERROR (Status);

  FspsUpd->FspsConfig.AmtEnabled        = (UINT8) AmtPeiConfig->AmtEnabled;
  FspsUpd->FspsConfig.WatchDog          = (UINT8) AmtPeiConfig->WatchDog;
  FspsUpd->FspsConfig.AsfEnabled        = (UINT8) AmtPeiConfig->AsfEnabled;
  FspsUpd->FspsConfig.ManageabilityMode = (UINT8) AmtPeiConfig->ManageabilityMode;
  FspsUpd->FspsConfig.FwProgress        = (UINT8) AmtPeiConfig->FwProgress;
  FspsUpd->FspsConfig.AmtSolEnabled     = (UINT8) AmtPeiConfig->AmtSolEnabled;
  FspsUpd->FspsConfig.WatchDogTimerOs   = (UINT16) AmtPeiConfig->WatchDogTimerOs;
  FspsUpd->FspsConfig.WatchDogTimerBios = (UINT16) AmtPeiConfig->WatchDogTimerBios;

  return EFI_SUCCESS;
}
