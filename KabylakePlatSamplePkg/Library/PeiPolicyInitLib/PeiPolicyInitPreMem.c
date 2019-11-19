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
// AMI_OVERRIDE_START - SiPreMemPolicy update for chipset porting. 
#include <Library/PchHookPeiLib.h>
#include <Library/SystemAgentHookPeiLib.h>

EFI_STATUS
EFIAPI
UpdatePeiPreMemCpuPlatformPolicy (
  IN OUT  SI_PREMEM_POLICY_PPI  *SiPreMemPolicyPpi
);
// AMI_OVERRIDE_END - SiPreMemPolicy update for chipset porting. 

/**
  Initialize Intel PEI Platform Policy

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.
**/
VOID
EFIAPI
PeiPolicyInitPreMem (
  IN UINT8                     FirmwareConfiguration
  )
{
  EFI_STATUS                   Status;
  SI_PREMEM_POLICY_PPI         *SiPreMemPolicyPpi;

  DEBUG ((DEBUG_INFO, "Silicon PEI Policy Initialization Start in Pre-Memory...\n"));
  //
  // Call SiCreatePreMemConfigBlocks to initialize platform policy structure
  // and get all intel default policy settings.
  //
  Status = SiCreatePreMemConfigBlocks (&SiPreMemPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Update policy by board configuration
  //
#ifndef MINTREE_FLAG
  UpdatePeiPchPolicyBoardConfigPreMem (SiPreMemPolicyPpi);
  UpdatePeiMePolicyBoardConfigPreMem (SiPreMemPolicyPpi);
  UpdatePeiSaPolicyBoardConfigPreMem (SiPreMemPolicyPpi);
  UpdatePeiCpuPolicyBoardConfigPreMem (SiPreMemPolicyPpi);
#endif //MINTREE_FLAG

  if (FirmwareConfiguration != FwConfigDefault) {
    //
    // Update and override all platform related and customized settings below.
    //
    UpdatePeiPchPolicyPreMem (SiPreMemPolicyPpi);
    UpdatePeiSaPolicyPreMem (SiPreMemPolicyPpi);
#ifndef MINTREE_FLAG
    UpdatePeiMePolicyPreMem (SiPreMemPolicyPpi);
    UpdatePeiCpuPolicyPreMem (SiPreMemPolicyPpi);

    if ((FirmwareConfiguration >= FwConfigTest) && (FirmwareConfiguration < FwConfigMax)) {
      // Update Debug Policies
      UpdatePeiPchPolicyDebugPreMem (SiPreMemPolicyPpi);
      UpdatePeiSaPolicyDebugPreMem (SiPreMemPolicyPpi);
      UpdatePeiCpuPolicyDebugPreMem (SiPreMemPolicyPpi);
    }
// AMI_OVERRIDE_START - SiPreMemPolicy update for chipset porting. 
    AmiUpdatePeiPchPolicyPreMem (SiPreMemPolicyPpi);  
    AmiUpdatePeiSaPolicyPreMem (SiPreMemPolicyPpi);
    UpdatePeiPreMemCpuPlatformPolicy (SiPreMemPolicyPpi);
// AMI_OVERRIDE_END - SiPreMemPolicy update for chipset porting. 
#endif // MINTREE_FLAG

  }
  //
  // Install SiPreMemPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SiPreMemInstallPolicyPpi (SiPreMemPolicyPpi);
  ASSERT_EFI_ERROR (Status);

#ifndef MINTREE_FLAG
  //
  // Work around to reset a PCIe card which requires more delay than the standard delay as per PCIe spec.
  //
#if 0 // AMI_OVERRIDE_START - Do the power sequence by SgTpv module.
  PcieCardResetWorkAround (SiPreMemPolicyPpi);
#endif // AMI_OVERRIDE_END - Do the power sequence by SgTpv module.
#endif

  DEBUG ((DEBUG_INFO, "Silicon PEI Policy Initialization Done in Pre-Memory\n"));
}
