/** @file
  Internal header file for Fsp Policy Initialization Library.

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

@par Specification Reference:
**/

#ifndef _PEI_FSP_POLICY_INIT_LIB_H_
#define _PEI_FSP_POLICY_INIT_LIB_H_

#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/ConfigBlockLib.h>

#include <FspEas.h>
#include <FspmUpd.h>
#include <FspsUpd.h>

extern EFI_GUID gIntelPeiGraphicsVbtGuid;
extern EFI_GUID gTianoLogoGuid;

/**
  Performs FSP SI PEI Policy initialization.

  @param[in][out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSiPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  );

/**
  Performs FSP PCH PEI Policy pre mem initialization.

  @param[in][out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspPchPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  );

/**
  Performs FSP PCH PEI Policy initialization.

  @param[in][out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspPchPolicyInit (
  IN OUT FSPS_UPD     *FspsUpd
  );

/**
  Performs FSP CPU PEI Policy initialization.

  @param[in][out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspCpuPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  );

#ifndef MINTREE_FLAG
/**
Performs FSP Security PEI Policy initialization.

@param[in][out]  FspmUpd             Pointer to FSP UPD Data.

@retval          EFI_SUCCESS         FSP UPD Data is updated.
@retval          EFI_NOT_FOUND       Fail to locate required PPI.
@retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSecurityPolicyInitPreMem(
IN OUT FSPM_UPD    *FspmUpd
);
#endif

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
  );

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
  IN OUT FSPS_UPD     *FspsUpd
  );

/**
  Performs FSP SA PEI Policy initialization in pre-memory.

  @param[in][out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSaPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  );

/**
  Performs FSP SA PEI Policy initialization.

  @param[in][out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSaPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  );

#ifdef AMT_SUPPORT
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
  );
#endif

/**
  Performs FSP CPU PEI Policy post memory initialization.

  @param[in][out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspCpuPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  );

#ifndef MINTREE_FLAG
/**
Performs FSP Security PEI Policy post memory initialization.

@param[in][out]  FspsUpd             Pointer to FSP UPD Data.

@retval          EFI_SUCCESS         FSP UPD Data is updated.
@retval          EFI_NOT_FOUND       Fail to locate required PPI.
@retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSecurityPolicyInit(
IN OUT FSPS_UPD    *FspsUpd
);
#endif

/**
  PeiGetSectionFromFv finds the file in FV and gets file Address and Size

  @param[in] NameGuid              - File GUID
  @param[out] Address              - Pointer to the File Address
  @param[out] Size                 - Pointer to File Size

  @retval EFI_SUCCESS                Successfull in reading the section from FV
**/
EFI_STATUS
EFIAPI
PeiGetSectionFromFv (
  IN CONST  EFI_GUID        NameGuid,
  OUT VOID                  **Address,
  OUT UINT32               *Size
  );

/**
  Performs FSP Misc UPD initialization.

  @param[in][out]  FspmUpd             Pointer to FSPM_UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
**/
EFI_STATUS
EFIAPI
PeiFspMiscUpdInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  );

#endif // _PEI_FSP_POLICY_INIT_LIB_H_
