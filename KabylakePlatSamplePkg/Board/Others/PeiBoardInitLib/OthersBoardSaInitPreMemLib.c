/** @file
 Source code for the others board SA configuration Pcd init functions in Pre-Memory init phase.

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

#include "OthersBoardInitLib.h"

#include "BoardSaConfigPreMem.h"

#include <PlatformBoardConfig.h>

/**
  Others MRC configuration init function for PEI pre-memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersSaMiscConfigInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  //
  // UserBd
  //
  switch (BoardId) {
    case BoardIdSkylakeA0Lpddr3Crb:
    case BoardIdSkylakeA0Ddr4Rvp2:
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
    case BoardIdKarkomX0:
    case BoardIdKarkomA0:
    case BoardIdKarkomUlt:
    case BoardIdKarkomMiniUlt:
    case BoardIdSkylakeSds:
    case BoardIdSkylakePantherMtn:
    case BoardIdSkylakePhabletPoc:
    case BoardIdSkylakeStarbrook:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
      //
      // Assign UserBd to 5 which is assigned to MrcInputs->BoardType btUser4 for ULT platforms.
      // This is required to skip Memory voltage programming based on GPIO's in MRC
      //
      PcdSet8S (PcdSaMiscUserBd, 5); // MrcBoardType btUser4 for ULT platform
      break;

    case BoardIdZumbaBeachServerEv:
    case BoardIdZumbaBeachServerCrb:
      PcdSet8S (PcdSaMiscUserBd, 7); // It's a UP Server board (btUpServer)
      PcdSet16S (PcdSaMiscMmioSizeAdjustment, 0x100); // Add 256MB of MMIO
      break;

    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Erb:
    case BoardIdPineValley:
    case BoardIdSkylakeSlpfPpv:
    case BoardIdSkylakeSdlBrk:
      PcdSet8S (PcdSaMiscUserBd, 1);
      if (BoardIdOrgValue == BoardIdKabyLakeOc) {
        PcdSet8S (PcdSaMiscUserBd, 8);
      }
      break;

    default:
      // MiscPeiPreMemConfig.UserBd = 0 by default.
      break;
  }


  return EFI_SUCCESS;
}

/**
  Others Board Memory Init related configuration init function for PEI pre-memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersMrcConfigInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId
  )
{

  //
  // SPD Address Table
  //
  switch (BoardId) {
    case BoardIdSkylakeA0Lpddr3Crb:
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
      PcdSet8S (PcdMrcSpdAddressTable0, DIMM_SMB_SPD_P0C0D0_STP);
      PcdSet8S (PcdMrcSpdAddressTable1, DIMM_SMB_SPD_P0C0D1_STP);
      PcdSet8S (PcdMrcSpdAddressTable2, DIMM_SMB_SPD_P0C0D0_STP);
      PcdSet8S (PcdMrcSpdAddressTable3, DIMM_SMB_SPD_P0C0D1_STP);
      break;
    default:
      PcdSet8S (PcdMrcSpdAddressTable0, 0xA0);
      PcdSet8S (PcdMrcSpdAddressTable1, 0xA2);
      PcdSet8S (PcdMrcSpdAddressTable2, 0xA4);
      PcdSet8S (PcdMrcSpdAddressTable3, 0xA6);
      break;
  }

  //
  // DRAM SPD Data & related configuration
  //
  // Setting the PCD's to default value (SKL RVP3). It will be overriden to board specific settings below.
  PcdSet32S (PcdMrcDqByteMap, (UINTN) mDqByteMapSklRvp3);
  PcdSet16S (PcdMrcDqByteMapSize, sizeof (mDqByteMapSklRvp3));
  PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) mDqsMapCpu2DramSklRvp3);
  PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (mDqsMapCpu2DramSklRvp3));
  PcdSet32S (PcdMrcRcompTarget, (UINTN) mRcompTargetSklRvp3);
  PcdSet32S (PcdMrcRcompResistor, (UINTN) mRcompResistorSklRvp3);

  switch (BoardId) {
    case BoardIdSkylakeA0Lpddr3Crb:
      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);
      break;

    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
      PcdSet32S (PcdMrcDqByteMap, (UINTN) DqByteMapSklRvp5);
      PcdSet16S (PcdMrcDqByteMapSize, DqByteMapSklRvp5Size);
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklRvp5);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklRvp5));

      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);

#ifdef MEM_DOWN_FLAG
      if (PlatformInfo->BoardBomId == 0x6) { // Hynix H9CCNNNBJTMLAR-NUD, 8Gb, DDP, 1867
        PcdSet32S (PcdMrcSpdData, (UINTN) mSkylakeRvp3Spd110);
        PcdSet16S (PcdMrcSpdDataSize, mSkylakeRvp3Spd110Size);
      }
      if (PlatformInfo->BoardBomId == 0x7) { // Samsung K4E6E304EE-EGCF, 4Gb, QDP, 1867
        PcdSet32S (PcdMrcSpdData, (UINTN) mSkylakeRvp16Spd);
        PcdSet16S (PcdMrcSpdDataSize, mSkylakeRvp16SpdSize);
      }
      if (!PlatformInfo->SpdPresent) {
        PcdSet8S (PcdMrcSpdAddressTable0, 0x0);
        PcdSet8S (PcdMrcSpdAddressTable1, 0x0);
        PcdSet8S (PcdMrcSpdAddressTable2, 0x0);
        PcdSet8S (PcdMrcSpdAddressTable3, 0x0);
      }
#endif
      break;


    case BoardIdSkylakeLpdimmDoe:
      PcdSet32S (PcdMrcDqByteMap, (UINTN) DqByteMapSklLpdimmDOE);
      PcdSet16S (PcdMrcDqByteMapSize, sizeof (DqByteMapSklLpdimmDOE));
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklLpdimmDOE);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklLpdimmDOE));

      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);

      PcdSetBoolS (PcdMrcDqPinsInterleavedControl, TRUE);
      PcdSetBoolS (PcdMrcDqPinsInterleaved, FALSE);
      break;

    case BoardIdSkylakeA0Ddr4Rvp2:
      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp2);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp2);
      break;

    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdPineValley:
    case BoardIdZumbaBeachServerEv:
    case BoardIdZumbaBeachServerCrb:
    case BoardIdSkylakeSdlBrk:
      PcdSetBoolS (PcdMrcDqPinsInterleavedControl, TRUE);
      PcdSetBoolS (PcdMrcDqPinsInterleaved, TRUE);
      // Fall through to RVP9/10
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Erb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeSlpfPpv:
      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklDtHalo);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklDtHalo);

      break;



    case BoardIdSkylakeSds:
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklSds);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklSds));

      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);

#ifdef MEM_DOWN_FLAG
      PcdSet8S (PcdMrcSpdAddressTable0, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable1, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable2, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable3, 0x0);
      PcdSet32S (PcdMrcSpdData, (UINTN) SkylakeSdsSpd);
      PcdSet16S (PcdMrcSpdDataSize, sizeof (SkylakeSdsSpd));
#endif
      break;

    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
      PcdSet32S (PcdMrcDqByteMap, (UINTN) DqByteMapSklPantherMtn);
      PcdSet16S (PcdMrcDqByteMapSize, sizeof (DqByteMapSklPantherMtn));
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklPantherMtn);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklPantherMtn));

      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);

#ifdef MEM_DOWN_FLAG
      PcdSet8S (PcdMrcSpdAddressTable0, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable1, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable2, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable3, 0x0);
      PcdSet32S (PcdMrcSpdData, (UINTN) PantherMountainSpd);
      PcdSet16S (PcdMrcSpdDataSize, sizeof (PantherMountainSpd));
#endif
      break;

    case BoardIdSkylakeStarbrook:
      PcdSet32S (PcdMrcDqByteMap, (UINTN) DqByteMapSklStarbrook);
      PcdSet16S (PcdMrcDqByteMapSize, sizeof (DqByteMapSklStarbrook));
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklStarbrook);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklStarbrook));

      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);

#ifdef MEM_DOWN_FLAG
      PcdSet8S (PcdMrcSpdAddressTable0, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable1, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable2, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable3, 0x0);
      PcdSet32S (PcdMrcSpdData, (UINTN) PantherMountainSpd);
      PcdSet16S (PcdMrcSpdDataSize, sizeof (PantherMountainSpd));
#endif
      break;

    case BoardIdSkylakePhabletPoc:
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklPhabletPoc);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklPhabletPoc));

      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);

#ifdef MEM_DOWN_FLAG
      PcdSet8S (PcdMrcSpdAddressTable0, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable1, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable2, 0x0);
      PcdSet8S (PcdMrcSpdAddressTable3, 0x0);
      PcdSet32S (PcdMrcSpdData, (UINTN) PantherMountainSpd);
      PcdSet16S (PcdMrcSpdDataSize, sizeof (PantherMountainSpd));
#endif
      break;

    default:
      break;
  }

  //
  // CA Vref routing: board-dependent
  // 0 - VREF_CA goes to both CH_A and CH_B (LPDDR3/DDR3L)
  // 1 - VREF_CA to CH_A, VREF_DQ_A to CH_B (should not be used)
  // 2 - VREF_CA to CH_A, VREF_DQ_B to CH_B (DDR4)
  //
  switch (BoardId) {
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdZumbaBeachServerEv:
    case BoardIdZumbaBeachServerCrb:
    case BoardIdSkylakeSdlBrk:
      PcdSet8S (PcdMrcCaVrefConfig, 2); // DDR4 boards
      break;

    case BoardIdSkylakeA0Ddr4Rvp2:
      PcdSet8S (PcdMrcCaVrefConfig, 1);
      break;

    default:
      PcdSet8S (PcdMrcCaVrefConfig, 0); // All DDR3L/LPDDR3 boards
      break;
  }

  return EFI_SUCCESS;
}

/**
  Others Board SA related GPIO configuration init function for PEI pre-memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersSaGpioConfigInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  //
  // Update board's GPIO for PEG slot reset
  //
  switch (BoardId) {
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Erb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeSlpfPpv:
    case BoardIdSkylakeSdlBrk:
      PcdSetBoolS (PcdPegGpioResetControl, FALSE);
      PcdSet32S (PcdPegResetGpioPad, GPIO_SKL_H_GPP_F22);
      PcdSetBoolS (PcdPegResetGpioActive, FALSE);
      break;
    case BoardIdZumbaBeachServerEv:
    case BoardIdZumbaBeachServerCrb:
      PcdSetBoolS (PcdPegGpioResetControl, FALSE);
      PcdSet32S (PcdPegResetGpioPad, GPIO_SKL_H_GPP_D18);
      PcdSetBoolS (PcdPegResetGpioActive, FALSE);
      break;
      ///
      /// Unknown: Disable GPIO usage
      ///
    default:
      PcdSetBoolS (PcdPegGpioResetControl, TRUE);
      PcdSetBoolS (PcdPegGpioResetSupoort, FALSE);
      PcdSet32S (PcdPegResetGpioPad, 0);
      PcdSetBoolS (PcdPegResetGpioActive, FALSE);
      break;
  }

  //
  // PCIE RTD3 GPIO
  //
  switch (BoardId) {
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeSlpfPpv:
      PcdSet8S (PcdRootPortDev, SA_PEG10_DEV_NUM);
      PcdSet8S (PcdRootPortFunc, SA_PEG10_FUN_NUM);

      PcdSet8S (PcdPcie0GpioSupport, PchGpio);
      PcdSet32S (PcdPcie0WakeGpioNo, 0);
      PcdSet8S (PcdPcie0HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie0HoldRstGpioNo, GPIO_SKL_H_GPP_F22);
      PcdSetBoolS (PcdPcie0HoldRstActive, FALSE);
      PcdSet8S (PcdPcie0PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie0PwrEnableGpioNo, GPIO_SKL_H_GPP_G22);
      PcdSetBoolS (PcdPcie0PwrEnableActive, TRUE);
      PcdSet8S  (PcdPcie0ClkReqNumber, 8);

      PcdSet8S  (PcdPcie1GpioSupport, NotSupported);
      PcdSet32S (PcdPcie1WakeGpioNo, 0);
      PcdSet8S  (PcdPcie1HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie1HoldRstGpioNo, 0);
      PcdSetBoolS (PcdPcie1HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie1PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie1PwrEnableGpioNo, 0);
      PcdSetBoolS (PcdPcie1PwrEnableActive, FALSE);

      PcdSet8S  (PcdPcie2GpioSupport, NotSupported);
      PcdSet32S (PcdPcie2WakeGpioNo, 0);
      PcdSet8S  (PcdPcie2HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie2HoldRstGpioNo, 0);
      PcdSetBoolS (PcdPcie2HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie2PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie2PwrEnableGpioNo, 0);
      PcdSetBoolS (PcdPcie2PwrEnableActive, FALSE);
      break;
    case BoardIdSkylakeHaloLpddr3Rvp16:
    case BoardIdSkylakeSdlBrk:
      PcdSet8S (PcdRootPortDev, SA_PEG10_DEV_NUM);
      PcdSet8S (PcdRootPortFunc, SA_PEG10_FUN_NUM);

      PcdSet8S  (PcdPcie0GpioSupport, PchGpio);
      PcdSet32S (PcdPcie0WakeGpioNo, GPIO_SKL_H_GPP_F12);
      PcdSet8S  (PcdPcie0HoldRstExpanderNo, 0);
      if (BoardIdOrgValue == BoardIdHaloDdr4uSFF) {
        PcdSet32S(PcdPcie0HoldRstGpioNo, GPIO_SKL_H_GPP_B20);
        PcdSet32S(PcdPcie0PwrEnableGpioNo, GPIO_SKL_H_GPP_C14);
      } else {
        PcdSet32S(PcdPcie0HoldRstGpioNo, GPIO_SKL_H_GPP_F22);
        PcdSet32S(PcdPcie0PwrEnableGpioNo, GPIO_SKL_H_GPP_G22);
      }
      PcdSetBoolS (PcdPcie0HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie0PwrEnableExpanderNo, 0);
      PcdSetBoolS (PcdPcie0PwrEnableActive, TRUE);
      PcdSet8S  (PcdPcie0ClkReqNumber, 8);

      PcdSet8S  (PcdPcie1GpioSupport, NotSupported);
      PcdSet32S (PcdPcie1WakeGpioNo, 0);
      PcdSet8S  (PcdPcie1HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie1HoldRstGpioNo, 0);
      PcdSetBoolS (PcdPcie1HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie1PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie1PwrEnableGpioNo, 0);
      PcdSetBoolS (PcdPcie1PwrEnableActive, FALSE);

      PcdSet8S  (PcdPcie2GpioSupport, NotSupported);
      PcdSet32S (PcdPcie2WakeGpioNo, 0);
      PcdSet8S  (PcdPcie2HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie2HoldRstGpioNo, 0);
      PcdSetBoolS (PcdPcie2HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie2PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie2PwrEnableGpioNo, 0);
      PcdSetBoolS (PcdPcie2PwrEnableActive, FALSE);
      break;
    case BoardIdPineValley:
      PcdSet8S (PcdRootPortDev, SA_PEG10_DEV_NUM);
      PcdSet8S (PcdRootPortFunc, SA_PEG10_FUN_NUM);

      PcdSet8S  (PcdPcie0GpioSupport, PchGpio);
      PcdSet32S (PcdPcie0WakeGpioNo, GPIO_SKL_H_GPP_F10);
      PcdSet8S  (PcdPcie0HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie0HoldRstGpioNo, GPIO_SKL_H_GPP_F23);
      PcdSetBoolS (PcdPcie0HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie0PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie0PwrEnableGpioNo, GPIO_SKL_H_GPP_G8);
      PcdSetBoolS (PcdPcie0PwrEnableActive, TRUE);
      PcdSet8S  (PcdPcie0ClkReqNumber, 8);

      PcdSet8S  (PcdPcie1GpioSupport, PchGpio);
      PcdSet32S (PcdPcie1WakeGpioNo, GPIO_SKL_H_GPP_B11);
      PcdSet8S  (PcdPcie1HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie1HoldRstGpioNo, GPIO_SKL_H_GPP_B14);
      PcdSetBoolS (PcdPcie1HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie1PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie1PwrEnableGpioNo, GPIO_SKL_H_GPP_G21);
      PcdSetBoolS (PcdPcie1PwrEnableActive, TRUE);
      PcdSet8S  (PcdPcie0ClkReqNumber, 8);

      PcdSet8S  (PcdPcie2GpioSupport, PchGpio);
      PcdSet32S (PcdPcie2WakeGpioNo, GPIO_SKL_H_GPP_F11);
      PcdSet8S  (PcdPcie2HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie2HoldRstGpioNo, GPIO_SKL_H_GPP_H12);
      PcdSetBoolS (PcdPcie2HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie2PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie2PwrEnableGpioNo, GPIO_SKL_H_GPP_A18);
      PcdSetBoolS (PcdPcie2PwrEnableActive, TRUE);
      PcdSet8S  (PcdPcie0ClkReqNumber, 8);
      break;
      ///
      /// Unknown: Disable GPIO usage
      ///
    default:
      PcdSet8S (PcdRootPortDev, 0xFF);
      PcdSet8S (PcdRootPortFunc, 0xFF);

      PcdSet8S  (PcdPcie0GpioSupport, NotSupported);
      PcdSet32S (PcdPcie0WakeGpioNo, 0);
      PcdSet8S  (PcdPcie0HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie0HoldRstGpioNo, 0);
      PcdSetBoolS (PcdPcie0HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie0PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie0PwrEnableGpioNo, 0);
      PcdSetBoolS (PcdPcie0PwrEnableActive, FALSE);

      PcdSet8S  (PcdPcie1GpioSupport, NotSupported);
      PcdSet32S (PcdPcie1WakeGpioNo, 0);
      PcdSet8S  (PcdPcie1HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie1HoldRstGpioNo, 0);
      PcdSetBoolS (PcdPcie1HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie1PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie1PwrEnableGpioNo, 0);
      PcdSetBoolS (PcdPcie1PwrEnableActive, FALSE);

      PcdSet8S  (PcdPcie2GpioSupport, NotSupported);
      PcdSet32S (PcdPcie2WakeGpioNo, 0);
      PcdSet8S  (PcdPcie2HoldRstExpanderNo, 0);
      PcdSet32S (PcdPcie2HoldRstGpioNo, 0);
      PcdSetBoolS (PcdPcie2HoldRstActive, FALSE);
      PcdSet8S  (PcdPcie2PwrEnableExpanderNo, 0);
      PcdSet32S (PcdPcie2PwrEnableGpioNo, 0);
      PcdSetBoolS (PcdPcie2PwrEnableActive, FALSE);
      break;
  }

  return EFI_SUCCESS;
}

