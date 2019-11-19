/** @file
  A helper driver to save information to SMRAM after SMRR is enabled.

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

#include "SmramSaveInfoHandlerSmm.h"

#define SMM_FROM_CPU_DRIVER_SAVE_INFO 0x81

GLOBAL_REMOVE_IF_UNREFERENCED UINT8   mSmiDataRegister;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN mLocked = FALSE;

CPU_INFO_PROTOCOL *mCpuInfoProtocol;
/**
  Dispatch function for a Software SMI handler.

  @param  DispatchHandle        The handle of this dispatch function.
  @param  DispatchContext       The pointer to the dispatch function's context.
                                The SwSmiInputValue field is filled in
                                by the software dispatch driver prior to
                                invoking this dispatch function.
                                The dispatch function will only be called
                                for input values for which it is registered.
**/
VOID
EFIAPI
SmramSaveInfoHandler (
  IN EFI_HANDLE                  DispatchHandle,
  IN EFI_SMM_SW_REGISTER_CONTEXT *DispatchContext
  )
{
  EFI_STATUS                Status;
  UINT64                    VarData[3];
  EFI_PHYSICAL_ADDRESS      SmramDataAddress;

//AMI_OVERRIDE_START >>> security issue fix
  if (DispatchContext->SwSmiInputValue != mCpuInfoProtocol->CpuInfo->SmmbaseSwSmiNumber)
      return;
//AMI_OVERRIDE_END <<< security issue fix
  if (!mLocked && IoRead8 (mSmiDataRegister) == SMM_FROM_CPU_DRIVER_SAVE_INFO) {
    VarData[2] = mCpuInfoProtocol->SmramCpuInfo->LockBoxSize;
    VarData[1] = mCpuInfoProtocol->SmramCpuInfo->SmramCpuData;

      Status = gSmst->SmmAllocatePages (
                 AllocateAnyPages,
                 EfiRuntimeServicesData,
                 EFI_SIZE_TO_PAGES (VarData[2]),
                 &SmramDataAddress
                 );
      ASSERT_EFI_ERROR (Status);
      DEBUG ((DEBUG_INFO, "CPU SMRAM NVS Data - %x\n", SmramDataAddress));
      DEBUG ((DEBUG_INFO, "CPU SMRAM NVS Data size - %x\n", VarData[2]));
//AMI_OVERRIDE_START >>> security issue fix
      if (EFI_ERROR(Status))
          return;
//AMI_OVERRIDE_END <<< security issue fix
      VarData[0] = (UINT64)SmramDataAddress;
      CopyMem (
              (VOID *) (UINTN) (VarData[0]),
              (VOID *) (UINTN) (VarData[1]),
              (UINTN) (VarData[2])
              );
    }

    mLocked = TRUE;
}

/**
  Entry point function of this driver.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
SmramSaveInfoHandlerSmmMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL    *SmmSwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT      SmmSwDispatchContext;
  EFI_HANDLE                       DispatchHandle;

  ///
  /// Get SMI data register
  ///
  mSmiDataRegister = R_PCH_APM_STS;

  ///
  /// Register software SMI handler
  ///
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &SmmSwDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Locate CPU Info Protocol
  ///
  Status = gBS->LocateProtocol (
                  &gCpuInfoProtocolGuid,
                  NULL,
                  (VOID **) &mCpuInfoProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  SmmSwDispatchContext.SwSmiInputValue = mCpuInfoProtocol->CpuInfo->SmmbaseSwSmiNumber;

  Status = SmmSwDispatch->Register (
                            SmmSwDispatch,
                            (EFI_SMM_HANDLER_ENTRY_POINT2)&SmramSaveInfoHandler,
                            &SmmSwDispatchContext,
                            &DispatchHandle
                            );
  ASSERT_EFI_ERROR (Status);
  return Status;
}
