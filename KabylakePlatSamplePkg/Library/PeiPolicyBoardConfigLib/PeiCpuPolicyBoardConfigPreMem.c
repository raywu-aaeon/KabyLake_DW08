/** @file
 Intel PEI CPU Pre-Memory Policy update by board configuration

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2017 Intel Corporation.

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
#include "PeiPolicyBoardConfig.h"
// AMI_OVERRIDE_START - AMI not use. 
#if 0
#include <CmosMap.h>
#endif
// AMI_OVERRIDE_END - AMI not use. 
#include <Library/ConfigBlockLib.h>
// AMI_OVERRIDE_START - AMI not use. 
#if 0
#include <Library/CmosAccessLib.h>
#endif
// AMI_OVERRIDE_END - AMI not use. 

///
/// BIOS ACM GUID. BIOS ACM interface is described in Intel Trusted Execution Technology BIOS Specification
/// @pre BIOS ACM must be placed in flash part on 4KB boundary
///
extern EFI_GUID gTxtApStartupPeiFileGuid;

/**
Searches PEI firmware volume (FV_BB) for the file with specified GUID through pGuid

@param[in] Guid         - A pointer GUID
@param[in] FileHandle   - Upon exit, points to the found file's handle
or NULL if it could not be found.
@param[in] Module       - A pointer to pointer to variable to hold address

@retval EFI_SUCCESS   - If address has been found
@retval EFI ERROR     - If address has not been found
**/
EFI_STATUS
FindModuleInFlash (
  IN EFI_GUID                    *Guid,
  IN EFI_FIRMWARE_VOLUME_HEADER  *FvHeader,
  OUT UINT32                     **Module
  )
{
  EFI_FFS_FILE_HEADER        *FfsFile;
  EFI_STATUS                 Status;

  ///
  /// Locate Firmware File System file within Firmware Volume
  ///
  Status = PeiServicesFfsFindFileByName (Guid, FvHeader, (VOID **)&FfsFile);

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  *Module = (UINT32 *)((UINT8 *)FfsFile + sizeof (EFI_FFS_FILE_HEADER));

  return Status;
}

// AMI_OVERRIDE_START - AMI not use. 
#if 0
/**
Searches PEI firmware volume (FV_BB) for file containing BIOS ACM.

@param[in] BiosAcBase    - A pointer to pointer to variable to hold found address

@retval EFI_SUCCESS   - If address has been found
@exception EFI_NOT_FOUND     - If address has not been found
**/
EFI_STATUS
FindBiosAcmInVolume (
  OUT UINT32             **BiosAcBase
  )
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME_HEADER    *FvHeader;

  ///
  /// Locate Firmware Volume header
  ///
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FixedPcdGet32 (PcdFlashFvRecovery2Base);

  Status = FindModuleInFlash (&gTxtBiosAcmPeiFileGuid, FvHeader, BiosAcBase);
  ///
  /// If BIOS ACM is not found - don't hang system. Assume that TXT
  /// must be disabled.
  ///
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to locate BIOS ACM\n"));
    return EFI_NOT_FOUND;
  }

  ASSERT (((((UINT32)(*BiosAcBase)) & 0xFFF) == 0) ? TRUE : FALSE);

  return EFI_SUCCESS;
}
#endif
// AMI_OVERRIDE_END - AMI not use. 

/**
Searches PEI firmware volume (FV_BB) for file containing AP Startup code

@param[in] ApStartupBase  - A pointer to pointer to variable to hold address

@retval EFI_SUCCESS   - If address has been found
@exception EFI_NOT_FOUND     - If address has not been found
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
FindApStartupInVolume (
  OUT UINT32             **ApStartupBase
  )
{
  EFI_STATUS                    Status;
  BOOLEAN                       GoodPlacement;
  EFI_FIRMWARE_VOLUME_HEADER    *FvHeader;

  ///
  /// Locate Firmware Volume header
  ///
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FixedPcdGet32 (PcdFlashFvRecoveryBase);
  Status = FindModuleInFlash (&gTxtApStartupPeiFileGuid, FvHeader, ApStartupBase);

  ///
  /// If AP Startup code is not found - don't hang system. Assume that TXT
  /// must be disabled.
  ///
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "Failed to locate AP Startup\n"));
    return EFI_NOT_FOUND;
  }
  ASSERT (((((UINT32)(*ApStartupBase)) & 0xFFF) == 0) ? TRUE : FALSE);

  GoodPlacement = (BOOLEAN)(((UINT32)(*ApStartupBase) >= 0xFFFE0000) && ((UINT32)(*ApStartupBase) <= 0xFFFFF000));
  ASSERT (GoodPlacement);

  return Status;
}

/**
  This function performs PEI CPU Pre-Memory Policy update by board configuration.

  @param[in, out] SiPolicy        The SI PreMem Policy PPI instance

  @retval EFI_SUCCESS             The SI Policy is successfully updated.
  @retval Others                  The SI Policy is not successfully updated.
**/
EFI_STATUS
EFIAPI
UpdatePeiCpuPolicyBoardConfigPreMem (
  IN OUT   SI_PREMEM_POLICY_PPI      *SiPreMemPolicyPpi
  )
{
  EFI_STATUS                      Status;
  CPU_TXT_PREMEM_CONFIG           *CpuTxtPreMemConfig;
  CPU_SECURITY_PREMEM_CONFIG      *CpuSecurityPreMemConfig;

  DEBUG ((DEBUG_INFO, "Updating CPU Policy by board config in Pre Mem\n"));

  Status = GetConfigBlock ((VOID *)SiPreMemPolicyPpi, &gCpuTxtPreMemConfigGuid, (VOID *)&CpuTxtPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *)SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *)&CpuSecurityPreMemConfig);
  ASSERT_EFI_ERROR (Status);

// AMI_OVERRIDE_START - AMI not use. 
#if 0
  CpuSecurityPreMemConfig->TxtAcheckRequest = CmosRead8 (CMOS_TXT_REG) & 1;

  //
  // ACheck Request needs to be reset since we do not want system to run ACheck in the same boot sequence
  //
  CmosWrite8 (CMOS_TXT_REG, (UINT8)0);
  FindBiosAcmInVolume ((UINT32**)&CpuTxtPreMemConfig->BiosAcmBase);
#endif 
// AMI_OVERRIDE_END - AMI not use. 
  FindApStartupInVolume ((UINT32**)&CpuTxtPreMemConfig->ApStartupBase);
  CpuSecurityPreMemConfig->BiosSize = (UINT16)RShiftU64 (FixedPcdGet32 (PcdFlashAreaSize), 10);

  return Status;
}
