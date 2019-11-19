/** @file
  Instance of Fsp Policy Initialization Library.

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

#include <PeiFspPolicyInitLib.h>

/**
  Performs FSP PEI Policy Pre-memory initialization.

  @param[in] FspmUpdDataPtr       Pointer to FSPM UPD data.
**/
VOID
EFIAPI
FspPolicyInitPreMem (
  IN FSPM_UPD           *FspmUpdDataPtr
  )
{
  EFI_STATUS            Status;

  //
  // SI Pei Fsp Policy Initialization
  //
  Status = PeiFspSiPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - SI Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // PCH Pei Fsp Policy Initialization
  //
  Status = PeiFspPchPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - PCH Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // Cpu Pei Fsp Policy Initialization
  //
  Status = PeiFspCpuPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - CPU Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

#ifndef MINTREE_FLAG
  //
  // Security Pei Fsp Policy Initialization
  //
  Status = PeiFspSecurityPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - Security Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }
#endif

  //
  // ME Pei Fsp Policy Initialization
  //
  Status = PeiFspMePolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - ME Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // SystemAgent Pei Fsp Policy Initialization
  //
  Status = PeiFspSaPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - SystemAgent Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // Other Upd Initialization
  //
  Status = PeiFspMiscUpdInitPreMem (FspmUpdDataPtr);

}

/**
  Performs FSP PEI Policy initialization.

  @param[in][out] FspsUpd  Pointer UPD data region

**/
VOID
EFIAPI
FspPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  EFI_STATUS            Status;

  //
  // PCH Pei Fsp Policy Initialization
  //
  Status = PeiFspPchPolicyInit (FspsUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - PCH Pei Fsp Policy iInitialization fail, Status = %r\n", Status));
  }

  //
  // ME Pei Fsp Policy Initialization
  //
  Status = PeiFspMePolicyInit (FspsUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - ME Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }

  //
  // SystemAgent Pei Fsp Policy Initialization
  //
  Status = PeiFspSaPolicyInit (FspsUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - SystemAgent Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }

#ifdef AMT_SUPPORT
    //
    // AMT Pei Fsp Policy Initialization
    //
    Status = PeiFspAmtPolicyInit (FspsUpd);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_WARN, "ERROR - AMT Pei Fsp Policy Initialization fail, Status = %r\n", Status));
    }
#endif

  //
  // Cpu Pei Fsp Policy Initialization
  //
  Status = PeiFspCpuPolicyInit (FspsUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - CPU Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }

#ifndef MINTREE_FLAG
  //
  // Security Pei Fsp Policy Initialization
  //
  Status = PeiFspSecurityPolicyInit (FspsUpd);
  if (EFI_ERROR(Status)) {
  DEBUG((DEBUG_WARN, "ERROR - Security Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }
#endif
}
