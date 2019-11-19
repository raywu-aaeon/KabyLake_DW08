/**@file
Implementation of AMT Setup Initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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

#include <AmtSetup.h>
#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/AmtWrapperProtocol.h>

extern EFI_GUID gSetupVariableGuid;

EFI_STATUS
EFIAPI
InitializeAmtSetup (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  SETUP_AMT_FEATURES    SetupAmtFeatures;
  EFI_STATUS            Status;
  UINTN                 VariableSize;
  UINTN                 Update = 0;
  UINT8                 GrayOut;
  AMT_WRAPPER_PROTOCOL  *pAmtWrapper = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiAmtWrapperProtocolGuid,
                  NULL,
                  (VOID **) &pAmtWrapper
                  );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof(SETUP_AMT_FEATURES);
  Status = gRT->GetVariable (
                  L"SetupAmtFeatures",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupAmtFeatures
                  );

  if (pAmtWrapper->ActiveManagementIsStorageRedirectionEnabled () ||
      pAmtWrapper->ActiveManagementIsSolEnabled () ||
      pAmtWrapper->ActiveManagementIsKvmEnabled ()) {
    GrayOut = 1;
  } else {
    GrayOut = 0;
  }

  if (SetupAmtFeatures.GrayOut != GrayOut) {
    SetupAmtFeatures.GrayOut = GrayOut;
    Update = 1;
  }

  if (Update || EFI_ERROR(Status)) {
    Status = gRT->SetVariable (
                    L"SetupAmtFeatures",
                    &gSetupVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(SETUP_AMT_FEATURES),
                    &SetupAmtFeatures
                    );
    ASSERT_EFI_ERROR(Status);
  }

  return Status;
}
