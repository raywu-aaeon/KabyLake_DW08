/** @file
  This file is SampleCode for Intel PEI Platform Policy initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2017 Intel Corporation.

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

#include "PeiPolicyInit.h"
// AMI_OVERRIDE_START - SiPolicy update for chipset porting. 
#include <Library/PchHookPeiLib.h>
#include <Library/SystemAgentHookPeiLib.h>

EFI_STATUS
EFIAPI
UpdatePeiCpuPlatformPolicy (
  IN OUT  SI_POLICY_PPI *SiPolicyPpi
  );
// AMI_OVERRIDE_END - SiPolicy update for chipset porting. 

/**
  Initialize Intel PEI Platform Policy

  @param[in] PeiServices            General purpose services available to every PEIM.
  @param[in] FirmwareConfiguration  It uses to skip specific policy init that depends
                                    on the 'FirmwareConfiguration' varaible.
**/
VOID
EFIAPI
PeiPolicyInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN UINT8                     FirmwareConfiguration
  )
{
  EFI_STATUS                   Status;
  SI_POLICY_PPI                *SiPolicyPpi;

  //
  // Call SiCreateConfigBlocks to initialize Silicon Policy structure
  // and get all Intel default policy settings.
  //
  Status = SiCreateConfigBlocks (&SiPolicyPpi);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }
#ifndef MINTREE_FLAG
  if (PcdGetBool (PcdDumpDefaultSiliconPolicy)) {
    DEBUG ((DEBUG_INFO, "Dump Default Silicon Policy...\n"));
    DumpSiPolicy (SiPolicyPpi);
  }

  //
  // Update policy by board configuration
  //
  UpdatePeiSiPolicyBoardConfig (SiPolicyPpi);
  UpdatePeiPchPolicyBoardConfig (SiPolicyPpi);
  UpdatePeiSaPolicyBoardConfig (SiPolicyPpi);
  UpdatePeiCpuPolicyBoardConfig (SiPolicyPpi);
#ifdef AMT_SUPPORT
  UpdatePeiAmtPolicyBoardConfig (SiPolicyPpi);
#endif
  UpdatePeiMePolicyBoardConfig (SiPolicyPpi);
#endif // MINTREE_FLAG

  if (FirmwareConfiguration != FwConfigDefault) {
    //
    // Update and override all platform related and customized settings below.
    //
    UpdatePeiPchPolicy (SiPolicyPpi);
    UpdatePeiSaPolicy (SiPolicyPpi);
#ifndef MINTREE_FLAG
    UpdatePeiCpuPolicy (SiPolicyPpi);
#ifdef AMT_SUPPORT
    UpdatePeiAmtPolicy (SiPolicyPpi);
#endif
    UpdatePeiMePolicy (SiPolicyPpi);
#endif // MINTREE_FLAG
    if ((FirmwareConfiguration >= FwConfigTest) && (FirmwareConfiguration < FwConfigMax)) {
      // Update Debug Policies
#ifndef MINTREE_FLAG
      UpdatePeiPchPolicyDebug (SiPolicyPpi);
      UpdatePeiSaPolicyDebug (SiPolicyPpi);
      UpdatePeiCpuPolicyDebug (SiPolicyPpi);
#endif // MINTREE_FLAG
    }
// AMI_OVERRIDE_START - SiPolicy update for chipset porting.
    AmiUpdatePeiPchPolicy (SiPolicyPpi);
    AmiUpdatePeiSaPolicy (SiPolicyPpi);
    UpdatePeiCpuPlatformPolicy (SiPolicyPpi);
// AMI_OVERRIDE_END - SiPolicy update for chipset porting. 
  }
  //
  // Install SiPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SiInstallPolicyPpi (SiPolicyPpi);
  ASSERT_EFI_ERROR (Status);
}
