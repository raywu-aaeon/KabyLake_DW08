/** @file
  This file is a wrapper for Platform Policy driver. Get Setup
  Value to initialize Intel DXE Platform Policy.

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

#include "PolicyInitDxe.h"
#include <CpuSmm.h> // AdvancedFeaturesContent

GLOBAL_REMOVE_IF_UNREFERENCED UINT8            mFirmwareConfiguration = 0; // AdvancedFeaturesContent

#ifndef MINTREE_FLAG
/**
  Update the enhanced CPU SMM feature PCDs
**/
VOID
UpdateCpuSmmEnhancedPolicy (
  VOID
 )
{
  EFI_STATUS              Status;
  UINTN                   Size;
  CPU_SMM                 CpuSmmSetup;

  Size = sizeof (CPU_SMM);
  Status = gRT->GetVariable (L"CpuSmm", &gCpuSmmGuid, NULL, &Size, &CpuSmmSetup);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UpdateCpuSmmEnhancedPolicy: CpuSmmSetup GetVariable Status - %x\n", Status));
  } else {
    PcdSetBoolS (PcdCpuSmmCodeAccessCheckEnable,  (BOOLEAN)CpuSmmSetup.CpuSmmCodeAccessCheckEnable);
    PcdSetBoolS (PcdCpuSmmMsrSaveStateEnable,  (BOOLEAN)CpuSmmSetup.CpuSmmMsrSaveStateEnable);
    PcdSetBoolS (PcdCpuSmmUseDelayIndication,  (BOOLEAN)CpuSmmSetup.CpuSmmUseDelayIndication);
    PcdSetBoolS (PcdCpuSmmUseBlockIndication,  (BOOLEAN)CpuSmmSetup.CpuSmmUseBlockIndication);
    PcdSetBoolS (PcdCpuSmmUseSmmEnableIndication,  (BOOLEAN)CpuSmmSetup.CpuSmmUseSmmEnableIndication);
  }
}
#endif //MINTREE_FLAG

/**
  Initialize  DXE Platform Policy

  @param[in] ImageHandle       Image handle of this driver.
  @param[in] SystemTable       Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
PolicyInitDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS           Status;
#ifndef MINTREE_FLAG
  UINTN                VarDataSize;
  SETUP_DATA           *SystemConfiguration = NULL;

  VarDataSize = sizeof (SETUP_DATA);

  SystemConfiguration = (SETUP_DATA *)AllocateZeroPool (VarDataSize);

  mFirmwareConfiguration = 0;
  if (SystemConfiguration == NULL) {
    DEBUG ((DEBUG_WARN, "Fail to allocate memory to get System Configuration!\n"));
  } else {
    Status = gRT->GetVariable (
                    L"Setup",
                    &gSetupVariableGuid,
                    NULL,
                    &VarDataSize,
                    SystemConfiguration
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_WARN, "Fail to get System Configuration to identify the initial policy mode!\n"));
    } else {
      mFirmwareConfiguration = SystemConfiguration->FirmwareConfiguration;
    }
  }
#endif //MINTREE_FLAG

  //
  // SystemAgent Dxe Platform Policy Initialization
  //
  Status = SaPolicyInitDxe (ImageHandle);
  DEBUG ((DEBUG_INFO, "SystemAgent Dxe Platform Policy Initialization done\n"));
  ASSERT_EFI_ERROR (Status);

#ifndef MINTREE_FLAG
  //
  // ME DXE Policy Initialization
  //
  Status = MePolicyInitDxe (ImageHandle);
  DEBUG ((DEBUG_INFO, "ME DXE Policy Initialization done\n"));
  ASSERT_EFI_ERROR (Status);

  //
  // AMT DXE Policy Initialization
  //
#ifdef AMT_SUPPORT
  Status = AmtPolicyInitDxe (ImageHandle);
  DEBUG ((DEBUG_INFO, "AMT DXE Policy Initialization done\n"));
#endif

  //
  // Silicon Dxe Platform Policy Initialization
  //
  Status = SiliconPolicyInitDxe (ImageHandle);
  DEBUG ((DEBUG_INFO, "Silicon Dxe Platform Policy Initialization done\n"));
  ASSERT_EFI_ERROR (Status);
#endif //MINTREE_FLAG
  if (PcdGetBool(PcdIntelGopEnable)) {
    //
    // GOP Dxe Policy Initialization
    //
    Status = GopPolicyInitDxe(ImageHandle);
    DEBUG((DEBUG_INFO, "GOP Dxe Policy Initialization done\n"));
    ASSERT_EFI_ERROR(Status);
  }

#ifndef MINTREE_FLAG
  //
  // Update CPU SMM Enhanced Policy Initialization
  //
  UpdateCpuSmmEnhancedPolicy();
#endif //MINTREE_FLAG
  return Status;

}
