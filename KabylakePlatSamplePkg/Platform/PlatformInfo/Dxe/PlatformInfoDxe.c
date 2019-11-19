/** @file
  Platform Info driver to install protocol for DXE and
  creat a PlatformInfo varible.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2017 Intel Corporation.

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

#include "PlatformInfoDxe.h"

/**
  Create a variable for PlatformInfo if it is not existing.

  @param[in]  Event     Event whose notification function is being invoked.
  @param[in]  Context   Pointer to the notification function's context.
**/
VOID
EFIAPI
CreatePlatformInfoVar (
  IN EFI_EVENT          Event,
  IN VOID               *Context
  )
{
  EFI_STATUS            Status;
  PLATFORM_INFO         *PlatformInfo;
  PLATFORM_INFO         PlatformInfoVariable;
  UINTN                 DataSize;
  EFI_PEI_HOB_POINTERS  HobList;

  ///
  /// Get the HOB list. If it is not present, then ASSERT.
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Fail to get the HOB list.\n"));
    ASSERT_EFI_ERROR (Status);
    return;
  }

  ///
  /// Search for the Platform Info PEIM GUID HOB.
  ///
  HobList.Raw = GetNextGuidHob (&gPlatformInfoHobGuid, HobList.Raw);
  if (HobList.Raw == NULL) {
    DEBUG ((DEBUG_INFO, "PlatfromInfo HOB is not found.\n"));
    Status = EFI_NOT_FOUND;
    ASSERT_EFI_ERROR (Status);
    return;
  }
  PlatformInfo = (PLATFORM_INFO *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));

  if (PlatformInfo->UpdateData == TRUE) {
    DataSize = sizeof(PLATFORM_INFO);
    CopyMem (&PlatformInfoVariable, PlatformInfo, DataSize);
    Status = gRT->SetVariable (
                    PLATFORM_INFO_VARIABLE_NAME,
                    &gPlatformInfoHobGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    DataSize,
                    &PlatformInfoVariable
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Fail to set variable for PlatformInfo.\n"));
      ASSERT_EFI_ERROR (Status);
    }
  }
}


/**
  Entry point for the driver.

  This routine gets info from the PlatformInfo and produces a protocol
  to be consumed by DXE drivers to identify platforms.

  @param[in]  ImageHandle  Image Handle.
  @param[in]  SystemTable  EFI System Table.

  @retval     EFI_SUCCESS  Function has completed successfully.
  @retval     Others       All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
PlatformInfoInit (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                  Status;
  PLATFORM_INFO               *PlatformInfo;
  UINTN                       DataSize;
  EFI_PEI_HOB_POINTERS        HobList;
  VOID                        *Registration;
  EFI_EVENT                   CreatePlatformInfoVarEvent;
  UINT8                       DataBuffer[2];
  DXE_BOARD_CONFIG_INIT       BoardConfigInit;

  ///
  /// Get the HOB list.  If it is not present, then ASSERT.
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Search for the Platform Info PEIM GUID HOB.
  ///
  HobList.Raw = GetNextGuidHob (&gPlatformInfoHobGuid, HobList.Raw);
  if (HobList.Raw == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = (PLATFORM_INFO *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
  DataSize  = HobList.Header->HobLength - sizeof (EFI_HOB_GUID_TYPE);

  ///
  /// Fill the updated data
  ///
  if ((PlatformInfo->BoardType == BoardTypeSds) && (PlatformInfo->DockAttached == 1) && (PlatformInfo->EcPresent == 1)) {
    Status = GetBaseEcFwVersion (DataBuffer);
    if (Status == EFI_SUCCESS) {
      PlatformInfo->BaseEcFwMajorVersion = DataBuffer[0];
      PlatformInfo->BaseEcFwMinorVersion = DataBuffer[1];
    }
    Status = GetEc2EcProtocolVersion (DataBuffer);
    if (Status == EFI_SUCCESS) {
      PlatformInfo->EcProtocolVersion     = DataBuffer[0];
      PlatformInfo->BaseEcProtocolVersion = DataBuffer[1];
    }
  }

//AMI_OVERRIDE_START - Print debug trace for debugging  
  DEBUG ((DEBUG_ERROR, "PlatformInfoInit : BoardID: 0x%x\n", PlatformInfo->BoardID));
//AMI_OVERRIDE_END - Print debug trace for debugging    
  
  ///
  /// Install the PlatformInfo Protocol.
  ///
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gPlatformInfoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  PlatformInfo
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  CreatePlatformInfoVar,
                  NULL,
                  &CreatePlatformInfoVarEvent
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->RegisterProtocolNotify (
                  &gEfiVariableWriteArchProtocolGuid,
                  CreatePlatformInfoVarEvent,
                  &Registration
                  );
  ASSERT_EFI_ERROR (Status);
//AMI_OVERRIDE_START - Need to signal to ensure this event can be triggered
  gBS->SignalEvent (CreatePlatformInfoVarEvent);
//AMI_OVERRIDE_END - Need to signal to ensure this event can be triggered

  //
  // Init Board Config.
  //
  BoardConfigInit = (DXE_BOARD_CONFIG_INIT) (UINTN) PcdGet64 (PcdFuncBoardConfigInit);
  ASSERT (BoardConfigInit != NULL);
  if (BoardConfigInit == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = BoardConfigInit ((VOID *)PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
