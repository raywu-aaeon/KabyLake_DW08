/** @file
  Implementation of Fsp Me Policy Initialization.

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
  Performs FSP ME PEI Policy pre mem initialization.

  @param[in][out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspMePolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  EFI_STATUS                Status;
  SI_PREMEM_POLICY_PPI      *SiPreMemPolicy;
  ME_PEI_PREMEM_CONFIG      *MePeiPreMemConfig;

  DEBUG ((DEBUG_INFO, "PeiFspMePolicyInitPreMem\n"));

  //
  // Locate gSiPreMemPolicyPpi
  //
  SiPreMemPolicy = NULL;
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicy
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gMePeiPreMemConfigGuid, (VOID *) &MePeiPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  FspmUpd->FspmConfig.HeciTimeouts                      = (UINT8) MePeiPreMemConfig->HeciTimeouts;

  //
  // Test policies
  //
  FspmUpd->FspmTestConfig.DidInitStat                   = (UINT8) MePeiPreMemConfig->DidInitStat;
  FspmUpd->FspmTestConfig.DisableCpuReplacedPolling     = (UINT8) MePeiPreMemConfig->DisableCpuReplacedPolling;
  FspmUpd->FspmTestConfig.SendDidMsg                    = (UINT8) MePeiPreMemConfig->SendDidMsg;
  FspmUpd->FspmTestConfig.DisableHeciRetry              = (UINT8) MePeiPreMemConfig->DisableHeciRetry;
  FspmUpd->FspmTestConfig.DisableMessageCheck           = (UINT8) MePeiPreMemConfig->DisableMessageCheck;
  FspmUpd->FspmTestConfig.SkipMbpHob                    = (UINT8) MePeiPreMemConfig->SkipMbpHob;

  FspmUpd->FspmTestConfig.HeciCommunication2            = (UINT8) MePeiPreMemConfig->HeciCommunication2;
  FspmUpd->FspmTestConfig.KtDeviceEnable                = (UINT8) MePeiPreMemConfig->KtDeviceEnable;
  FspmUpd->FspmTestConfig.IderDeviceEnable              = (UINT8) MePeiPreMemConfig->IderDeviceEnable;


  FspmUpd->FspmConfig.Heci1BarAddress                   = MePeiPreMemConfig->Heci1BarAddress;
  FspmUpd->FspmConfig.Heci2BarAddress                   = MePeiPreMemConfig->Heci2BarAddress;
  FspmUpd->FspmConfig.Heci3BarAddress                   = MePeiPreMemConfig->Heci3BarAddress;

  return EFI_SUCCESS;
}

/**
  Performs FSP ME PEI Policy initialization.

  @param[in][out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspMePolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  EFI_STATUS                Status;
  SI_POLICY_PPI             *SiPolicyPpi;
  ME_PEI_CONFIG             *MePeiConfig;

  DEBUG ((DEBUG_INFO, "PeiFspMePolicyInit \n"));
  //
  // Locate gSiPolicyPpiGuid
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

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gMePeiConfigGuid, (VOID *) &MePeiConfig);
  ASSERT_EFI_ERROR (Status);

  FspsUpd->FspsConfig.Heci3Enabled                  = (UINT8) MePeiConfig->Heci3Enabled;
  FspsUpd->FspsConfig.MeUnconfigOnRtcClear          = (UINT8) MePeiConfig->MeUnconfigOnRtcClear;
  FspsUpd->FspsConfig.MeUnconfigIsValid             = (UINT8) MePeiConfig->MeUnconfigIsValid;

  //
  // Test policies
  //
  FspsUpd->FspsTestConfig.EndOfPostMessage          = (UINT8) MePeiConfig->EndOfPostMessage;
  FspsUpd->FspsTestConfig.DisableD0I3SettingForHeci = (UINT8) MePeiConfig->DisableD0I3SettingForHeci;

  return EFI_SUCCESS;
}
