/** @file
  Implementation of Fsp Security Policy Initialization.

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
  Performs FSP Security PEI Policy initialization.

  @param[in][out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSecurityPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  EFI_STATUS                    Status;
  SI_PREMEM_POLICY_PPI          *SiPreMemPolicyPpi;
  CPU_SECURITY_PREMEM_CONFIG    *CpuSecurityPreMemConfig;
  CPU_TXT_PREMEM_CONFIG         *CpuTxtPreMemConfig;

  DEBUG ((DEBUG_INFO | DEBUG_INIT, "FSP Update SecurityPolicy Pre-Mem Start\n"));

  //
  // Locate SiPreMemPolicyPpi
  //
  SiPreMemPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuTxtPreMemConfigGuid, (VOID *) &CpuTxtPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  ///
  ///Production Policies
  ///
  FspmUpd->FspmConfig.BiosGuard              = (UINT8)CpuSecurityPreMemConfig->BiosGuard;
  FspmUpd->FspmConfig.EnableSgx              = (UINT8)CpuSecurityPreMemConfig->EnableSgx;
  FspmUpd->FspmConfig.Txt                    = (UINT8)CpuSecurityPreMemConfig->Txt;
  FspmUpd->FspmConfig.FlashWearOutProtection = (UINT8)CpuSecurityPreMemConfig->FlashWearOutProtection;
  FspmUpd->FspmConfig.PrmrrSize              = CpuSecurityPreMemConfig->PrmrrSize;

  //
  //Value is produced premem (value initialized in TXT_INFO_HOB premem).  It is not consumed until DXE.
  //In theory, could be produced in postmem - will be done during TXT enablement
  //
  FspmUpd->FspmConfig.SinitMemorySize   = CpuTxtPreMemConfig->SinitMemorySize;
  FspmUpd->FspmConfig.TxtHeapMemorySize = CpuTxtPreMemConfig->TxtHeapMemorySize;
  FspmUpd->FspmConfig.TxtDprMemoryBase  = CpuTxtPreMemConfig->TxtDprMemoryBase;
  FspmUpd->FspmConfig.TxtDprMemorySize  = CpuTxtPreMemConfig->TxtDprMemorySize;

  ///
  ///Test RC Policies
  ///
  FspmUpd->FspmTestConfig.BiosSize          = (UINT16)CpuSecurityPreMemConfig->BiosSize;
  FspmUpd->FspmTestConfig.BiosAcmBase       = CpuTxtPreMemConfig->BiosAcmBase;
  FspmUpd->FspmTestConfig.BiosAcmSize       = CpuTxtPreMemConfig->BiosAcmSize;
  FspmUpd->FspmTestConfig.TgaSize           = CpuTxtPreMemConfig->TgaSize;
  FspmUpd->FspmTestConfig.TxtLcpPdBase      = CpuTxtPreMemConfig->TxtLcpPdBase;
  FspmUpd->FspmTestConfig.TxtLcpPdSize      = CpuTxtPreMemConfig->TxtLcpPdSize;

  DEBUG ((DEBUG_INFO | DEBUG_INIT, "FSP Update SecurityPolicy Pre-Mem End\n"));

  return EFI_SUCCESS;
}

/**
  Performs FSP Security PEI Policy post memory initialization.

  @param[in][out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSecurityPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  EFI_STATUS                Status;
// AMI_OVERRIDE_START - Remove one ';'
  SI_POLICY_PPI             *SiPolicyPpi;
// AMI_OVERRIDE_END - Remove one ';'
  BIOS_GUARD_CONFIG         *BiosGuardConfig;
  CPU_SGX_CONFIG            *CpuSgxConfig;
  UINTN                     Index;

  DEBUG ((DEBUG_INFO | DEBUG_INIT, "FSP Update SecurityPolicy\n"));
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

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gBiosGuardConfigGuid, (VOID *) &BiosGuardConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuSgxConfigGuid, (VOID *) &CpuSgxConfig);
  ASSERT_EFI_ERROR (Status);
  ///
  ///Production RC Policies
  ///

  for (Index = 0; Index < 4; Index++) {
    FspsUpd->FspsConfig.BgpdtHash[Index] = BiosGuardConfig->BgpdtHash[Index];
  }
  FspsUpd->FspsConfig.BiosGuardAttr = BiosGuardConfig->BiosGuardAttr;
  FspsUpd->FspsConfig.BiosGuardModulePtr = BiosGuardConfig->BiosGuardModulePtr;
  FspsUpd->FspsConfig.SendEcCmd = BiosGuardConfig->SendEcCmd;
  FspsUpd->FspsConfig.EcCmdProvisionEav = BiosGuardConfig->EcCmdProvisionEav;
  FspsUpd->FspsConfig.EcCmdLock = BiosGuardConfig->EcCmdLock;

  FspsUpd->FspsConfig.SgxEpoch0           = (UINT64)CpuSgxConfig->SgxEpoch0;
  FspsUpd->FspsConfig.SgxEpoch1           = (UINT64)CpuSgxConfig->SgxEpoch1;
  FspsUpd->FspsConfig.SgxSinitNvsData     = (UINT8)CpuSgxConfig->SgxSinitNvsData;


  ///
  ///Test RC Policies
  ///
  FspsUpd->FspsTestConfig.SgxSinitDataFromTpm = (UINT8)CpuSgxConfig->SgxSinitDataFromTpm;

  ///
  ///
  return EFI_SUCCESS;
}
