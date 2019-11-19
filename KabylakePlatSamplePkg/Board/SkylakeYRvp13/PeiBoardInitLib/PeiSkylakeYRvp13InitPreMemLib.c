/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2018 Intel Corporation.

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

#include "PeiSkylakeYRvp13InitLib.h"

#include "SkylakeYRvp13HsioPtssTables.h"

#include <PlatformBoardConfig.h>                    // for USB 20 AFE & Root Port Clk Info

//
// RVP13 Init Table
//
BOARD_INIT_TABLE mRvp13BoardInitPreMem[] = {
  {BoardIdSkylakeYRvp13, PeiSkylakeYRvp13InitPreMem},
  {BoardIdAmberLakeLpddr3Rvp13, PeiSkylakeYRvp13InitPreMem},
  { 0, 0 }
};

#ifdef MEM_DOWN_FLAG
//
// This is SPD for BoardIdAmberLakeLpddr3Rvp13
//
extern const UINT8 mAmberLakeLpddr32133Spd[];
extern const UINT16 mAmberLakeLpddr32133SpdSize;
//
// This is SPD for BoardIdSkylakeYRvp13
//
extern const UINT8 mSkylakeRvp3Spd110[];
extern const UINT16 mSkylakeRvp3Spd110Size;
#endif

//
// RVP13 depends on RVP3 config
//
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeA0Rvp3InitPreMem (
  IN VOID *Content
  );

EFI_STATUS
PeiSkylakeYRvp13SpecificInit (
  IN PLATFORM_INFO *PlatformInfo
  );

/**
  SkylaeYRvp13 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeYRvp13InitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS    Status;
  PLATFORM_INFO *PlatformInfo;
  PCD32_BLOB    UsbPort20Afe;
  PCD64_BLOB    PcdData;

  PlatformInfo = (PLATFORM_INFO *) Content;

  Status = PeiSkylakeA0Rvp3InitPreMem (Content);  // depends on RVP3.

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PcdSet32S (PcdSpecificLpHsioPtssTable1, (UINTN) PchLpHsioPtss_Bx_SkylakeYRvp13);
  PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_SkylakeYRvp13);
  PcdSet16S (PcdSpecificLpHsioPtssTable1Size, PchLpHsioPtss_Bx_SkylakeYRvp13_Size);
  PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_SkylakeYRvp13_Size);


  //
  // RecoveryMode GPIO
  //
  PcdData.Blob = 0;
  PcdData.BoardGpioConfig.Type = BoardGpioTypeNotSupported;
  PcdSet64S (PcdRecoveryModeGpio, PcdData.Blob);

  PcdSetBoolS (PcdIvCamInitPresent, FALSE);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port8Afe,  UsbPort20Afe.Blob);

#ifdef MEM_DOWN_FLAG
  if (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeLpddr3Rvp13) {
    PcdSet32S (PcdMrcSpdData, (UINTN) mAmberLakeLpddr32133Spd);
    PcdSet16S (PcdMrcSpdDataSize, mAmberLakeLpddr32133SpdSize);
  } else if (PlatformInfo->BoardIDOrgvalue == BoardIdSkylakeYRvp13) {
    PcdSet32S (PcdMrcSpdData, (UINTN) mSkylakeRvp3Spd110);
    PcdSet16S (PcdMrcSpdDataSize, mSkylakeRvp3Spd110Size);
  }
#endif

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
PeiSkylakeYRvp13BoardInitPreMemLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp13BoardInitPreMem, TABLE_LIST_NUM);
}

