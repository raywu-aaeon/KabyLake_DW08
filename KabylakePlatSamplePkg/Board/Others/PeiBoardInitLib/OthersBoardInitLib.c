/** @file
  Source code for the others board configuration init function in Post Memory init phase.

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

#include "OthersBoardInitLib.h"

#include <TbtBoardInfo.h>

//
// GPIO table header files
//

#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <GpioConfig.h>
#include <Library/PeiPlatformHookLib.h>
#include <Library/CpuPlatformLib.h>

#include "GpioTableLpddr3Rvp5.h"
#include "GpioTableDtRvp8Crb.h"
#include "GpioTableAioRvp9Crb.h"
#include "GpioTableAioRvp10Crb.h"
#include "GpioTableOc.h"
#include "GpioTableSlpfPpv.h"
#include "GpioTableDefault.h"
#include "GpioTableLpdimmDoe.h"
#include "GpioTableDvpPineValley.h"
#include "GpioTableSds.h"
#include "GpioTablePantherMtn.h"
#include "GpioTableGrizzlyMtn.h"
#include "GpioTableCoyoteMtn.h"
#include "GpioTablePhabletPoc.h"
#include "GpioTableLpddr3StarBrook.h"
#include "GpioTableIoExpander.h"
#include "GpioTableZumbaBeach.h"
#include "GpioTableHaloSdlBrk.h"
#include "PchHdaVerbTables.h"

EFI_STATUS
OthersInitInternalLibConstructor (
  IN BOARD_INIT_TABLE *BoardInitTable
  );

EFI_STATUS
OthersBoardFunctionInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId
);

//
// Others Board Init Table
//
BOARD_INIT_TABLE mOthersBoardInit [] = {
  {COMMON_BOARD_ID, OthersBoardInit},
  { 0, 0 }
};

/**
  Others misc. init function for PEI post memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersBoardMiscInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId
  )
{
  BOOLEAN Flag;

  Flag = ((BoardId == BoardIdSkylakeULpddr3Rvp5) || (BoardId == BoardIdSkylakeULpddr3Rvp5Ppv)) ? TRUE : FALSE;
  PcdSetBoolS (PcdDebugUsbUartEnable, Flag);

  Flag = BoardId == BoardIdSkylakeSdlBrk ? TRUE : FALSE;
  PcdSetBoolS (PcdM2GpioEnable, Flag);

  Flag = (BoardId == BoardIdSkylakePantherMtn || BoardId == BoardIdSkylakeSds ||
          BoardId == BoardIdKabylakeGrizzlyMtn || BoardId == BoardIdKabylakeCoyoteMtn) ?
          TRUE : FALSE;
  PcdSetBoolS (PcdSkyCamGpioEnable, Flag);

  Flag = (BoardId == BoardIdSkylakePhabletPoc) ? TRUE : FALSE;
  PcdSetBoolS (PcdPhabletPocGpioEnable, Flag);

  Flag = (BoardId == BoardIdSkylakePantherMtn) ? TRUE : FALSE;
  PcdSetBoolS (PcdPantherMtnGpioEnable, Flag);

  Flag = (BoardId == BoardIdKabylakeGrizzlyMtn) ? TRUE : FALSE;
  PcdSetBoolS(PcdGrizzlyMtnGpioEnable, Flag);
  if (Flag) {
    PcdSetBoolS(PcdMipiBridgeConfigInit, TRUE);
    PcdSet8S(PcdMipiBridgeI2CPort, 0);
  }

  Flag = (BoardId == BoardIdKabylakeCoyoteMtn) ? TRUE : FALSE;
  PcdSetBoolS(PcdCoyoteMtnGpioEnable, Flag);
  if (Flag) {
    PcdSetBoolS(PcdMipiBridgeConfigInit, TRUE);
    PcdSet8S(PcdMipiBridgeI2CPort, 4);
  }

  Flag = (BoardId == BoardIdSkylakeSds) ? TRUE : FALSE;
  PcdSetBoolS (PcdSarSensorResetWA, Flag);



  return EFI_SUCCESS;

}

EFI_STATUS
OthersBoardTbtInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId
  )
{
  switch (BoardId) {
    case BoardIdSkylakeSds:
      //
      // SDS board
      //
      PcdSet32S   (PcdTbtSupportedPortsInfo                    , 0x000100);
      PcdSet8S    (PcdTbtDefaultSelectorInfo                   , 9);

      PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType            , 1);
      PcdSet32S   (PcdTbtForcePwrGpioGpioNumber                , GPIO_SKL_LP_GPP_B0);
      PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel                 , 1);

      PcdSet8S    (PcdTbtCioPlugEventGpioGpioAccessType          , 1);
      PcdSet32S   (PcdTbtCioPlugEventGpioGpioNumber              , GPIO_SKL_LP_GPP_E23);
      PcdSetBoolS (PcdTbtCioPlugEventGpioGpioMode                , 0);

      //PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature        , SIGNATURE_32 ('_', 'E', '1', '7'));
      PcdSetBoolS (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting , 1);
      PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber       , 0x57);
      PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature        , SIGNATURE_32 ('_', 'E', '5', '7'));
      PcdSet16S   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress       , (UINT16)(EFI_ACPI_GPE0_BLK_ADDRESS + (0x57 / 0x08)));
      PcdSet8S    (PcdTbtCioPlugEventGpioAcpiGpeStsBit           , (0x57 % 0x08));
      break;

    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeSlpfPpv:
    case BoardIdSkylakeSdlBrk:
      //
      // DT/AIO/HALO board
      //
      PcdSet32S   (PcdTbtSupportedPortsInfo                    , 0x100100);
      PcdSet8S    (PcdTbtDefaultSelectorInfo                   , 9);

      PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType             , 1);
      PcdSet32S   (PcdTbtForcePwrGpioGpioNumber                 , GPIO_SKL_H_GPP_G16);
      PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel                  , 1);

      PcdSet8S    (PcdTbtCioPlugEventGpioGpioAccessType         , 1);
      PcdSet32S   (PcdTbtCioPlugEventGpioGpioNumber             , GPIO_SKL_H_GPP_G12);
      PcdSetBoolS (PcdTbtCioPlugEventGpioGpioMode               , 0);

      //PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature        , SIGNATURE_32 ('_', 'E', '0', 'C'));
      PcdSetBoolS (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting , 1);                                           //GpioGetGpeNumber(PeiPostMem->CioPlugEventGpio.GpioNumber, &Data32);
      PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber       , 0x0c);                         // Data32;      //Data16Ascii = Data8toAscii((UINT8)Data32);
      PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature        , SIGNATURE_32 ('_', 'E', '0', 'C'));           // SIGNATURE_32 ('_', 'E', ((Data16Ascii>>8) & 0xFF), ((Data16Ascii) & 0xFF));
      PcdSet16S   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress       , (UINT16) (EFI_ACPI_GPE0_BLK_ADDRESS + (0x0c / 0x08))); // (Data32/8));
      PcdSet8S    (PcdTbtCioPlugEventGpioAcpiGpeStsBit           , (0x0c % 0x08));                                          // (Data32%8);
      break;
  }

  return EFI_SUCCESS;
}
/**
  Others GPIO init function for PEI post memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersBoardGpioInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  //
  // GPIO Table Init.
  //

  switch (BoardId) {
    case BoardIdSkylakeSds:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableSds);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableSds) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdSkylakePantherMtn:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTablePantherMtn);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTablePantherMtn) / sizeof (GPIO_INIT_CONFIG));

      break;
    case BoardIdKabylakeGrizzlyMtn:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableGrizzlyMtn);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableGrizzlyMtn) / sizeof (GPIO_INIT_CONFIG));
      break;

    case BoardIdKabylakeCoyoteMtn:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableCoyoteMtn);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableCoyoteMtn) / sizeof (GPIO_INIT_CONFIG));

      break;
    case BoardIdSkylakeStarbrook:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableStarBrook);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableStarBrook) / sizeof (GPIO_INIT_CONFIG));

      break;
    case BoardIdSkylakeA0Lpddr3Crb:
    case BoardIdSkylakeA0Ddr4Rvp2:
      //@todo: Add GPIO table for new board
      //ConfigureGpio (mGpioTableLpDdr3RvpCrb, sizeof (mGpioTableLpDdr3RvpCrb) / sizeof (GPIO_INIT_CONFIG));
      break;

    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableLpddr3Rvp5);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableLpddr3Rvp5) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdSkylakeLpdimmDoe:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableLpdimmDoe);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableLpdimmDoe) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10Erb:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableAioRvp10Crb);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableAioRvp10Crb) / sizeof (GPIO_INIT_CONFIG));
      if (BoardIdOrgValue == BoardIdHaloDdr4uSffCpv) {
        PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTableUsffCpv);
        PcdSet16S(PcdBoardGpioTable2Size, sizeof (mGpioTableUsffCpv) / sizeof (GPIO_INIT_CONFIG));
      }
      break;
    case BoardIdSkylakeSlpfPpv:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableSlpfPpv);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableSlpfPpv) / sizeof (GPIO_INIT_CONFIG));
      break;

    case BoardIdSkylakeAioRvp9Crb:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableAioRvp9Crb);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableAioRvp9Crb) / sizeof (GPIO_INIT_CONFIG));
      break;

    case BoardIdSkylakeDtRvp8Crb:
   if ((BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvErb) ||
       (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvCrb) ||
       (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb)) { //Customized change needed for KBL-S EV ERB/CRB in contrast to SKL RVP9
        PcdSet32S(PcdBoardGpioTable, (UINTN)mGpioTableAioRvp9Crb);
        PcdSet16S(PcdBoardGpioTableSize, sizeof (mGpioTableAioRvp9Crb) / sizeof (GPIO_INIT_CONFIG));
        PcdSet32S(PcdBoardGpioTable2, (UINTN)mGpioTableKabyLakeDtDdr4Rvp9Ev);
        PcdSet16S(PcdBoardGpioTable2Size, sizeof (mGpioTableKabyLakeDtDdr4Rvp9Ev) / sizeof (GPIO_INIT_CONFIG));
   } else if(BoardIdOrgValue == BoardIdKabyLakeOc) {
        PcdSet32S(PcdBoardGpioTable, (UINTN)mGpioTableOc);
        PcdSet16S(PcdBoardGpioTableSize, sizeof(mGpioTableOc) / sizeof(GPIO_INIT_CONFIG));
   } else {
        PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableDtRvp8Crb);
        PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableDtRvp8Crb) / sizeof (GPIO_INIT_CONFIG));
   }
      break;
    case BoardIdPineValley:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableDvpPineValley);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableDvpPineValley) / sizeof (GPIO_INIT_CONFIG));
      break;

    case BoardIdZumbaBeachServerEv:
    case BoardIdZumbaBeachServerCrb:
      DEBUG((DEBUG_INFO, "Initializing Zumba Beach GPIOs!!!!\n"));
      PcdSet32S (PcdBoardGpioTable, (UINTN) upZumbaBeachGpioDefinitionTable);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (upZumbaBeachGpioDefinitionTable) / sizeof (GPIO_INIT_CONFIG));
      break;

    case BoardIdSkylakeSdlBrk:
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableHaloSdlBrk);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableHaloSdlBrk) / sizeof (GPIO_INIT_CONFIG));
      break;

    default:
      DEBUG((DEBUG_INFO, "For Unknown Board ID..Use Default GPIO Table...\n"));
      PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableDefault);
      PcdSet16S (PcdBoardGpioTableSize, sizeof (mGpioTableDefault) / sizeof (GPIO_INIT_CONFIG));

      break;
  }

  return EFI_SUCCESS;
}

/**
  Others Expander GPIO init function for PEI post memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersExpanderGpioInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId
  )
{

  //
  // GpioExpander Init.
  //

  switch (BoardId) {
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
      PcdSet32S (PcdGpioExpanderTable, (UINTN) mGpioTableIoExpanderRvp5);
      PcdSet16S (PcdGpioExpanderTableSize, sizeof (mGpioTableIoExpanderRvp5) / sizeof (IO_EXPANDER_GPIO_CONFIG));
      break;

    default:
      PcdSet32S (PcdGpioExpanderTable, 0);
      PcdSet16S (PcdGpioExpanderTableSize, 0);
      break;
  }

  return EFI_SUCCESS;
}

/**
  Others touch panel GPIO init function for PEI post memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersTouchPanelGpioInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  switch (BoardId) {
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10Erb:
    case BoardIdSkylakeSlpfPpv:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableAioRvp10CrbTouchpanel);
      break;
    case BoardIdSkylakeSds:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableSdsTouchpanel);
      break;
    case BoardIdSkylakePantherMtn:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTablePantherMtnTouchpanel);
      break;
    case BoardIdKabylakeGrizzlyMtn:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableGrizzlyMtnTouchpanel);
      break;
    case BoardIdKabylakeCoyoteMtn:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableCoyoteMtnTouchpanel);
      break;
    case BoardIdSkylakeStarbrook:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableStarBrookTouchpanel);
      break;
    case BoardIdSkylakePhabletPoc:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTablePhabletPocTouchpanel);
      break;
    case BoardIdSkylakeSdlBrk:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableHaloSdlBrkTouchpanel);
      break;
    case BoardIdSkylakeDtRvp8Crb:
      if ((BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvErb) ||
          (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvCrb) ||
          (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb)) {
        PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableAioRvp9CrbTouchpanel);
      }
      break;
    case BoardIdSkylakeAioRvp9Crb:
      PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableAioRvp9CrbTouchpanel);
      break;
    default:
      PcdSet32S (PcdBoardGpioTableTouchPanel, 0);
      break;
  }

  return EFI_SUCCESS;
}

/**
  Others SDHC detect GPIO init function for PEI post memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersSidebandCardDetectGpioInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId
  )
{

  PcdSet32S (PcdBoardGpioTableSdhcSidebandCardDetect, 0);

  return EFI_SUCCESS;
}

/**
  Others HDA VerbTable init function for PEI post memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersHdaVerbTableInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  HDAUDIO_VERB_TABLE *VerbTable;
  HDAUDIO_VERB_TABLE *VerbTable2;
  HDAUDIO_VERB_TABLE *DisplayAudioVerbTable;

  VerbTable  = NULL;
  VerbTable2 = NULL;
  DisplayAudioVerbTable = NULL;

  switch (BoardId) {
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
      VerbTable = &HdaVerbTableAlc298Rvp5;
      break;
    case BoardIdSkylakeDtRvp8Crb:
      if ((BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvErb) ||
          (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvCrb) ||
          (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb)) {
        VerbTable = &HdaVerbTableAlc274Rvp9;
      } else {
        VerbTable = &HdaVerbTableAlc898Rvp8;
      }
      break;
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10Erb:
      VerbTable = &HdaVerbTableAlc298Rvp10;
      break;
    case BoardIdSkylakeA0Lpddr3Crb:
    case BoardIdSkylakeA0Ddr4Rvp2:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeSlpfPpv:
      VerbTable = &HdaVerbTableAlc286S;
      VerbTable2 = &HdaVerbTableAlc298;
      break;
    case BoardIdSkylakeSds:
      VerbTable = &HdaVerbTableAlc298Sds;
      break;
    case BoardIdSkylakePantherMtn:
    case BoardIdSkylakeStarbrook:
      VerbTable = &HdaVerbTableAlc298PantherMtn;
      break;
    case BoardIdKabylakeGrizzlyMtn:
      VerbTable = &HdaVerbTableAlc298GrizzlyMtn;
      break;
  }

  PcdSet32S (PcdHdaVerbTable, (UINTN) VerbTable);
  PcdSet32S (PcdHdaVerbTable2, (UINTN) VerbTable2);

  if (EnumSklCpu == GetCpuGeneration ()) {
    DisplayAudioVerbTable = &HdaVerbTableDisplayAudioSkl;
  } else {
    DisplayAudioVerbTable = &HdaVerbTableDisplayAudio;
  }

  PcdSet32S (PcdDisplayAudioHdaVerbTable, (UINTN) DisplayAudioVerbTable);

  // SKL POR Codecs - Realtek ALC286S, ALC298, ALC888S (external - connected via HDA header)
  VerbTable = &HdaVerbTableAlc286S;
  PcdSet32S (PcdCommonHdaVerbTable1, (UINTN) VerbTable);

  VerbTable = &HdaVerbTableAlc298;
  PcdSet32S (PcdCommonHdaVerbTable2, (UINTN) VerbTable);

  VerbTable = &HdaVerbTableAlc888S;
  PcdSet32S (PcdCommonHdaVerbTable3, (UINTN) VerbTable);

  return EFI_SUCCESS;
}

/**
  SkylaeA0Rvp3 board configuration init function for PEI post memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
OthersBoardInit (
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

  Status = OthersBoardGpioInit (PlatformInfo, BoardId, BoardIdOrgValue);
  Status = OthersExpanderGpioInit (PlatformInfo, BoardId);
  Status = OthersTouchPanelGpioInit (PlatformInfo, BoardId, BoardIdOrgValue);
  Status = OthersSidebandCardDetectGpioInit (PlatformInfo, BoardId);
  Status = OthersHdaVerbTableInit (PlatformInfo, BoardId, BoardIdOrgValue);
  Status = OthersBoardMiscInit (PlatformInfo, BoardId);
  Status = OthersBoardTbtInit (PlatformInfo, BoardId);
  Status = OthersBoardFunctionInit (PlatformInfo, BoardId);

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
PeiOthersBoardInitLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{

  return CommonInitInternalLibConstructor (mOthersBoardInit, DEFAULT_INIT_SLOT_POSTMEM);
}

