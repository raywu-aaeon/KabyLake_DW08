/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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

#include "PeiSkylakeURvp15InitLib.h"

#include "SkylakeURvp15HsioPtssTables.h"

#include <PlatformBoardConfig.h>                    // for USB 20 AFE & Root Port Clk Info

//
// RVP13 Init Table
//
BOARD_INIT_TABLE mRvp15BoardInitPreMem[] = {
  {BoardIdSkylakeURvp15, PeiSkylakeURvp15InitPreMem},
  { 0, 0 }
};

//
// RVP13 depends on RVP3 config
//
EFI_STATUS
PeiSkylakeURvp7CommonInitPreMem (
  IN VOID *Content
  );

/**
  SkylaeURvp15 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeURvp15InitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PCD64_BLOB PcdData;

  Status = PeiSkylakeURvp7CommonInitPreMem (Content);  // depends on RVP3.

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PcdSet32S (PcdSpecificLpHsioPtssTable1, (UINTN) PchLpHsioPtss_Bx_SkylakeURvp15);
  PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_SkylakeURvp15);
  PcdSet16S (PcdSpecificLpHsioPtssTable1Size, PchLpHsioPtss_Bx_SkylakeURvp15_Size);
  PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_SkylakeURvp15_Size);

  PcdSet8S (PcdMrcCaVrefConfig, 2);

  //
  // RecoveryMode GPIO
  //
  PcdData.Blob = 0;
  PcdData.BoardGpioConfig.Type = BoardGpioTypeNotSupported;
  PcdSet64S (PcdRecoveryModeGpio, PcdData.Blob);

  PcdSetBoolS (PcdIvCamInitPresent, FALSE);

  return EFI_SUCCESS;
}

/**
  The constructor function for SkylakeA0Rvp3 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiSkylakeURvp15BoardInitPreMemLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp15BoardInitPreMem, TABLE_LIST_NUM);
}

