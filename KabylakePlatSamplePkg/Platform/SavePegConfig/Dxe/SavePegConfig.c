/** @file
  This is the driver that locates the SaData HOB, if it
  exists, and saves the PegData portion of it to NVRAM.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2012 - 2016 Intel Corporation.

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

#include "SavePegConfig.h"
#include <SaDataHob.h>

//
// Variable to save SA PEG Data
//
#define SA_PEG_DATA_GUID \
  { \
    0xb414caf8, 0x8225, 0x4d6f, { 0xb9, 0x18, 0xcd, 0xe5, 0xcb, 0x84, 0xcf, 0x0b } \
  }

GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID  gPegConfigDataGuid  = SA_PEG_DATA_GUID;
GLOBAL_REMOVE_IF_UNREFERENCED CHAR16    PegConfigVariable[] = L"SaPegData";


/**
  This is the standard EFI driver point that detects if there is an
  SaData HOB, and, if so, saves the PegData portion of it to NVRAM.

  @param[in] ImageHandle
  @param[in] SystemTable

  @retval    EFI_UNSUPPORTED
**/
EFI_STATUS
EFIAPI
SavePegConfigEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINTN                           VarSize;
  EFI_STATUS                      Status;
  UINT8                           *SaPegHob;
  SA_DATA_HOB                     *SaDataHob;
  VOID                            *VariableData;
  UINT8                           *PegData;
  EDKII_VARIABLE_LOCK_PROTOCOL    *VariableLock;

  Status = EFI_NOT_FOUND;
  VarSize = 0;
  VariableData = NULL;
  Status = gRT->GetVariable (
                  PegConfigVariable,
                  &gPegConfigDataGuid,
                  NULL,
                  &VarSize,
                  VariableData
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    VariableData = AllocateZeroPool (VarSize);
    Status = gRT->GetVariable (
                    PegConfigVariable,
                    &gPegConfigDataGuid,
                    NULL,
                    &VarSize,
                    VariableData
                    );
  }

  ///
  /// Get SaPegHob HOB
  ///
  SaPegHob = NULL;
  SaPegHob = GetFirstGuidHob (&gSaPegHobGuid);
  if ((Status == EFI_SUCCESS) && (SaPegHob == NULL)) {
    ///
    /// If Hob not present but variable present, delete the variable
    ///
    Status = gRT->SetVariable (
                    PegConfigVariable,
                    &gPegConfigDataGuid,
                    0,
                    0,
                    NULL
                    );
    DEBUG ((DEBUG_INFO, "Delete SaPegData variable: Status=%r\n", Status));
  } else if (SaPegHob != NULL) {
    PegData = (UINT8 *) (SaPegHob + sizeof (EFI_HOB_GUID_TYPE));
    if ((Status == EFI_NOT_FOUND) || CompareMem (VariableData, PegData, (((EFI_HOB_GENERIC_HEADER*) SaPegHob)->HobLength - sizeof (EFI_HOB_GUID_TYPE)))) {
      VarSize = (((EFI_HOB_GENERIC_HEADER*) SaPegHob)->HobLength - sizeof (EFI_HOB_GUID_TYPE));
      Status = gRT->SetVariable (
                      PegConfigVariable,
                      &gPegConfigDataGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                      VarSize,
                      PegData
                      );
      DEBUG ((DEBUG_INFO, "Update SaPegData variable: Size=%X, Status=%r\n", VarSize, Status));
    }
  }

  //
  // Mark SaPegData to read-only if the Variable Lock protocol exists
  //
  Status = gBS->LocateProtocol(&gEdkiiVariableLockProtocolGuid, NULL, (VOID **)&VariableLock);
  if (!EFI_ERROR(Status)) {
    Status = VariableLock->RequestToLock(VariableLock, L"SaPegData", &gPegConfigVariableGuid);
    ASSERT_EFI_ERROR(Status);
  }

  ///
  /// Get SaDataHob HOB
  ///
  SaDataHob = NULL;
  SaDataHob = (SA_DATA_HOB *) GetFirstGuidHob (&gSaDataHobGuid);
  if ((SaDataHob != NULL) && (SaDataHob->PegPlatformResetRequired)) {
    DEBUG ((DEBUG_INFO, "PEG Preset Search indicates platform reset required.\n"));
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  return EFI_UNSUPPORTED;
}
