/** @file
 Source code for the others board configuration Pcd init functions.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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

#include "OthersBoardInitPreMemLib.h"

#include <PlatformBoardConfig.h>
#include <Library/PeiPlatformHookLib.h>   // For Recovery Mode GPIO
#include <Library/PchInfoLib.h>

//
// HSIO PTSS
//

#include "PchHHsioPtssTablesBx.h"
#include "PchHHsioPtssTablesDx.h"
#include "PchLpHsioPtssTablesBx.h"
#include "PchLpHsioPtssTablesCx.h"
#include "KabylakePchHHsioPtssTablesAx.h"

EFI_STATUS
OthersInitInternalLibConstructor (
  IN BOARD_INIT_TABLE *BoardInitTable
  );

//
// Others Board Init Table
//
BOARD_INIT_TABLE mOthersBoardInitPreMem [] = {
  {COMMON_BOARD_ID, OthersBoardInitPreMem},
  { 0, 0 }
};

/**
  Others HSIO PTSS init function for PEI pre-memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersBoardMiscInitPreMem (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  PCD64_BLOB PcdData;

  //
  // IoExpander Present
  //
  switch (BoardId) {
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
      PcdSetBoolS (PcdIoExpanderPresent, TRUE);
      break;
    default:
      PcdSetBoolS (PcdIoExpanderPresent, FALSE);
      break;
  }

  //
  // RecoveryMode GPIO
  //
  PcdData.Blob = 0;
  PcdData.BoardGpioConfig.Type = BoardGpioTypeNotSupported;
  PcdSet64S (PcdRecoveryModeGpio, PcdData.Blob);

  //
  // IvCam Present
  //
  if (BoardId == BoardIdKabylakeUDdr3lRvp7 || BoardId == BoardIdSkylakeSds) {
    PcdSetBoolS (PcdIvCamInitPresent, TRUE);
  }
  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
    PcdSetBoolS (PcdIvCamInitPresent, FALSE);
  }

  //
  // OddPower Init
  //
  if (BoardId == BoardIdSkylakeAioRvp10Crb || BoardId == BoardIdSkylakeAioRvp10Evp) {
    PcdSetBoolS (PcdOddPowerInitEnable, TRUE);
  } else {
    PcdSetBoolS (PcdOddPowerInitEnable, FALSE);
  }

  //
  // Pc8374SioKbc Present
  //
  if ((BoardId != BoardIdSkylakeAioRvp10Crb) &&
      (BoardId != BoardIdSkylakeAioRvp10CrbPpv) &&
      (BoardId != BoardIdSkylakeAioRvp10Erb) &&
      (BoardId != BoardIdSkylakeAioRvp10Evp) &&
      (BoardId != BoardIdSkylakeSlpfPpv) &&
      (BoardId != BoardIdSkylakeHaloLpddr3Rvp16)) {
    PcdSetBoolS (PcdPc8374SioKbcPresent, TRUE);
  } else {
    PcdSetBoolS (PcdPc8374SioKbcPresent, FALSE);
  }

  //
  // Vboost Enable
  //
  if (BoardId == BoardIdSkylakeDtRvp8Crb) {
    PcdSetBoolS (PcdCpuVboostEnable, TRUE);
  }

  if ((BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvCrb) || (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb)) {
    //
    // @todo - Enable PCD when KSC Firmware is enabled SX Entry commands completely.
    //
    PcdSetBoolS(PcdUsbcEcPdNegotiation, FALSE);
  } else {
    PcdSetBoolS(PcdUsbcEcPdNegotiation, FALSE);
  }


 return  EFI_SUCCESS;
}

/**
  Others HSIO PTSS init function for PEI pre-memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersHsioPtssInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId
  )
{
  //
  // PCH-H HSIO PTSS Table Init
  //
  PcdSet32S (PcdUnknowHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_Unknown1);
  PcdSet16S (PcdUnknowHHsioPtssTable1Size, PchHHsioPtss_Bx_Unknown1_Size);
  PcdSet32S (PcdUnknowHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_Unknown1);
  PcdSet16S (PcdUnknowHHsioPtssTable2Size, PchHHsioPtss_Dx_Unknown1_Size);

  switch (BoardId) {
    case BoardIdSkylakeDtRvp8Crb:
      if (GetPchGeneration () == KblPch) {
        PcdSet32S (PcdSpecificHHsioPtssTable1, 0);
        PcdSet16S (PcdSpecificHHsioPtssTable1Size, 0);
        PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Ax_KabylakeRvp8);
        PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Ax_KabylakeRvp8_Size);
      } else {
        PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeDtRvp8Crb);
        PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeDtRvp8Crb_Size);
        PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeDtRvp8Crb);
        PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeDtRvp8Crb_Size);
      }
      break;
    case BoardIdSkylakeAioRvp9Crb:
      PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeAioRvp9Crb);
      PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeAioRvp9Crb_Size);
      PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeAioRvp9Crb);
      PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeAioRvp9Crb_Size);
      break;
    case BoardIdSkylakeAioRvp10Crb:
      PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeAioRvp10Crb);
      PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeAioRvp10Crb_Size);
      PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeAioRvp10Crb);
      PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeAioRvp10Crb_Size);
      break;
    case BoardIdSkylakeAioRvp10Erb:
      PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeAioRvp10Erb);
      PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeAioRvp10Erb_Size);
      PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeAioRvp10Erb);
      PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeAioRvp10Erb_Size);
      break;
    case BoardIdPineValley:
      PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_PineValley);
      PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_PineValley_Size);
      PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_PineValley);
      PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_PineValley_Size);
      break;
    case BoardIdSkylakeAioRvp10CrbPpv:
      PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeAioRvp10CrbPpv);
      PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeAioRvp10CrbPpv_Size);
      PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeAioRvp10CrbPpv);
      PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeAioRvp10CrbPpv_Size);
      break;
    case BoardIdSkylakeSdlBrk:
      PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeSdlBrk);
      PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeSdlBrk_Size);
      PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeSdlBrk);
      PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeSdlBrk_Size);
      break;

    default:
      break;
  }

  //
  // PCH-LP HSIO PTSS Table Init
  //

  PcdSet32S (PcdUnknowLpHsioPtssTable1, (UINTN) PchLpHsioPtss_Bx_Unknown1);
  PcdSet32S (PcdUnknowLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_Unknown1);
  PcdSet16S (PcdUnknowLpHsioPtssTable1Size, PchLpHsioPtss_Bx_Unknown1_Size);
  PcdSet16S (PcdUnknowLpHsioPtssTable2Size, PchLpHsioPtss_Cx_Unknown1_Size);

  switch (BoardId) {
    case BoardIdSkylakeULpddr3Rvp5:
      PcdSet32S (PcdSpecificLpHsioPtssTable1, (UINTN) PchLpHsioPtss_Bx_SkylakeULpddr3Rvp5);
      PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_SkylakeULpddr3Rvp5);
      PcdSet16S (PcdSpecificLpHsioPtssTable1Size, PchLpHsioPtss_Bx_SkylakeULpddr3Rvp5_Size);
      PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_SkylakeULpddr3Rvp5_Size);
      break;
    case BoardIdSkylakeULpddr3Rvp5Ppv:
      PcdSet32S (PcdSpecificLpHsioPtssTable1, (UINTN) PchLpHsioPtss_Bx_SkylakeULpddr3Rvp5Ppv);
      PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_SkylakeULpddr3Rvp5Ppv);
      PcdSet16S (PcdSpecificLpHsioPtssTable1Size, PchLpHsioPtss_Bx_SkylakeULpddr3Rvp5Ppv_Size);
      PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_SkylakeULpddr3Rvp5Ppv_Size);
      break;
    case BoardIdSkylakeA0Ddr4Rvp2:
      PcdSet32S (PcdSpecificLpHsioPtssTable1, (UINTN) PchLpHsioPtss_Bx_SkylakeA0Ddr4Rvp2);
      PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_SkylakeA0Ddr4Rvp2);
      PcdSet16S (PcdSpecificLpHsioPtssTable1Size, PchLpHsioPtss_Bx_SkylakeA0Ddr4Rvp2_Size);
      PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_SkylakeA0Ddr4Rvp2_Size);
      break;
    case BoardIdKarkomUlt:
      PcdSet32S (PcdSpecificLpHsioPtssTable1, (UINTN) PchLpHsioPtss_Bx_KarkomUlt);
      PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_KarkomUlt);
      PcdSet16S (PcdSpecificLpHsioPtssTable1Size, PchLpHsioPtss_Bx_KarkomUlt_Size);
      PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_KarkomUlt_Size);
      break;
    case BoardIdSkylakeSds:
      PcdSet32S (PcdSpecificLpHsioPtssTable1, 0); //PchLpHsioPtss_Bx_SkylakeSds;
      PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_SkylakeSds);
      PcdSet16S (PcdSpecificLpHsioPtssTable1Size, 0); //PchLpHsioPtss_Bx_SkylakeSds_Size;
      PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_SkylakeSds_Size);
      break;
    case BoardIdKarkomA0:
      PcdSet32S (PcdSpecificLpHsioPtssTable1, (UINTN)PchLpHsioPtss_Bx_KarkomA0);
      PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN)PchLpHsioPtss_Cx_KarkomA0);
      PcdSet16S (PcdSpecificLpHsioPtssTable1Size, PchLpHsioPtss_Bx_KarkomA0_Size);
      PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_KarkomA0_Size);
      break;

    default:
      break;
  }

  return EFI_SUCCESS;
}


/**
  Others board init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
OthersBoardInitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;

  PlatformInfo = (PLATFORM_INFO *) Content;
  ASSERT (PlatformInfo != NULL);

  if (PlatformInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  if (EFI_ERROR (Status)) {
    BoardId = COMMON_BOARD_ID;
  }

  Status = OthersHsioPtssInit (PlatformInfo, BoardId);
  Status = OthersMrcConfigInit (PlatformInfo, BoardId);
  Status = OthersSaGpioConfigInit (PlatformInfo, BoardId, BoardIdOrgValue);
  Status = OthersSaMiscConfigInit (PlatformInfo, BoardId, BoardIdOrgValue);
  Status = OthersRootPortClkInfoInit(PlatformInfo, BoardId, BoardIdOrgValue);
  Status = OthersUsbConfigInit (PlatformInfo, BoardId, BoardIdOrgValue);
  Status = OthersGpioGroupTierInit (PlatformInfo, BoardId);
  Status = OthersBoardMiscInitPreMem (PlatformInfo, BoardId, BoardIdOrgValue);
  Status = OthersBoardFunctionInitPreMem (PlatformInfo, BoardId, BoardIdOrgValue);

  return EFI_SUCCESS;
}

/**
  The constructor function for SkylakeA0Rvp3 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
**/
EFI_STATUS
EFIAPI
PeiOthersBoardInitPreMemLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mOthersBoardInitPreMem, DEFAULT_INIT_SLOT_PREMEM);
}
