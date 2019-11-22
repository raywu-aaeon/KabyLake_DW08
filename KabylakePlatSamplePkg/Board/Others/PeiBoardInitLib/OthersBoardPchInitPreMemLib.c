/** @file
 Source code for the others board PCH configuration Pcd init functions for Pre-Mmeory Init phase.

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

#include <GpioPinsSklH.h>               // for GPIO definition
#include <PlatformBoardConfig.h>        // for USB 20 AFE & Root Port Clk Info

/**
  Others Board Root Port Clock Info configuration init function for PEI pre-memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersRootPortClkInfoInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  PCD64_BLOB *RootPort;
  PCD64_BLOB Lan;

  RootPort = AllocateZeroPool (32 * sizeof (PCD64_BLOB));
  ASSERT (RootPort != NULL);
  if (RootPort == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Lan.Blob = 0;

  ///
  /// Assign ClkReq signal to root port. (Base 0)
  /// For LP, Set 0 - 5
  /// For H,  Set 0 - 15
  /// Note that if GbE is enabled, ClkReq assigned to GbE will not be available for Root Port.
  ///
  switch (BoardId) {
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
      RootPort[2].Info.ClkReqSupported = TRUE;
      RootPort[2].Info.ClkReqNumber = 5;
      RootPort[3].Info.ClkReqSupported = TRUE;
      RootPort[3].Info.ClkReqNumber = 2;
      RootPort[4].Info.ClkReqSupported = TRUE;
      RootPort[4].Info.ClkReqNumber = 3;
      RootPort[5].Info.ClkReqSupported = TRUE;
      RootPort[5].Info.ClkReqNumber = 4;
      RootPort[6].Info.ClkReqSupported = TRUE;
      RootPort[6].Info.ClkReqNumber = 0;
      RootPort[8].Info.ClkReqSupported = TRUE;
      RootPort[8].Info.ClkReqNumber = 1;
      Lan.Info.ClkReqSupported = TRUE;
      Lan.Info.ClkReqNumber    = 2;
      break;

    case BoardIdSkylakeSds:
    case BoardIdSkylakeStarbrook:
      RootPort[1].Info.ClkReqSupported = TRUE;
      RootPort[1].Info.ClkReqNumber = 1;
      RootPort[2].Info.ClkReqSupported = TRUE;
      RootPort[2].Info.ClkReqNumber = 5;
      RootPort[3].Info.ClkReqSupported = TRUE;
      RootPort[3].Info.ClkReqNumber = 4;
      RootPort[4].Info.ClkReqSupported = TRUE;
      RootPort[4].Info.ClkReqNumber = 3;
      RootPort[8].Info.ClkReqSupported = TRUE;
      RootPort[8].Info.ClkReqNumber = 2;
      break;

    case BoardIdSkylakePhabletPoc: // ClockReq to Clock Source -- needs to be done in Softstrap
      RootPort[2].Info.ClkReqSupported = TRUE; //Root port 2 in Bios corresponds to 3 in Schematic and is connected to WLAN
      RootPort[9].Info.ClkReqSupported = TRUE; //Root port 9 in Bios corresponds to 10 in Schematic and is connected to WIGIG
      //In softstrap connect ClkReqNumber = 2 to ClockSource = 5 -- For WLAN
      RootPort[2].Info.ClkReqNumber = 2; //WLAN is mapped to root port 2 in Bios (3 in schematic) is connected to ClkReqNumber 2
      //In softstrap connect ClkReqNumber = 3 to ClockSource = 3 -- For WIGIG
      RootPort[9].Info.ClkReqNumber = 3; //WIGIG is mapped to root port 9 in Bios (10 in schematic) is connected to ClkReqNumber
      break;

    case BoardIdSkylakePantherMtn:
      RootPort[2].Info.ClkReqSupported = TRUE;
      RootPort[2].Info.ClkReqNumber = 5;
      break;

    case BoardIdKabylakeGrizzlyMtn:
      RootPort[2].Info.ClkReqSupported = TRUE;
      RootPort[2].Info.ClkReqNumber = 5;
      RootPort[4].Info.ClkReqSupported = TRUE;
      RootPort[4].Info.ClkReqNumber = 3;
      break;

    case BoardIdKabylakeCoyoteMtn:
      RootPort[2].Info.ClkReqSupported = TRUE; //WLAN
      RootPort[2].Info.ClkReqNumber = 5;
      RootPort[3].Info.ClkReqSupported = TRUE; //PCIE Modem
      RootPort[3].Info.ClkReqNumber = 4;
      RootPort[4].Info.ClkReqSupported = TRUE; //SSD
      RootPort[4].Info.ClkReqNumber = 3;
      break;

    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Erb:
    case BoardIdSkylakeSlpfPpv:
      if ((BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvCrb) || (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb)) {
        RootPort[3].Info.ClkReqSupported = TRUE;   // PCIe port 4
        RootPort[3].Info.ClkReqNumber = 2;
        RootPort[4].Info.ClkReqSupported = TRUE;
        RootPort[4].Info.ClkReqNumber = 1;

        RootPort[20].Info.ClkReqSupported = TRUE;   // PCIe port 21
        RootPort[20].Info.ClkReqNumber = 10;
      }
      else {
        RootPort[3].Info.ClkReqSupported = TRUE;   // PCIe port 4
        RootPort[3].Info.ClkReqNumber = 1;
        RootPort[4].Info.ClkReqSupported = TRUE;
        RootPort[4].Info.ClkReqNumber = 2;
      }


      RootPort[ 5].Info.ClkReqSupported = TRUE;
      RootPort[ 5].Info.ClkReqNumber = 3;
      RootPort[ 6].Info.ClkReqSupported = TRUE;
      RootPort[ 6].Info.ClkReqNumber = 4;
      RootPort[ 7].Info.ClkReqSupported = TRUE;
      RootPort[ 7].Info.ClkReqNumber = 5;
      RootPort[ 8].Info.ClkReqSupported = TRUE;
      RootPort[ 8].Info.ClkReqNumber = 6;
      RootPort[16].Info.ClkReqSupported = TRUE;   // PCIe port 17
      RootPort[16].Info.ClkReqNumber = 7;
      Lan.Info.ClkReqSupported = TRUE;
      Lan.Info.ClkReqNumber    = 2;
      break;
    case BoardIdSkylakeSdlBrk:
      RootPort[ 5].Info.ClkReqSupported = TRUE; // PCIe port 6 / M.2 WIFI
      RootPort[ 5].Info.ClkReqNumber    = 0;
      RootPort[12].Info.ClkReqSupported = TRUE; // PCIe port 13 / M.2 SSD
      RootPort[12].Info.ClkReqNumber    = 1;
      Lan.Info.ClkReqSupported = TRUE;
      Lan.Info.ClkReqNumber    = 3;
      break;

    default:
      break;
  }

  PcdSet64S (PcdRootPort0ClkInfo, RootPort[ 0].Blob);
  PcdSet64S (PcdRootPort1ClkInfo, RootPort[ 1].Blob);
  PcdSet64S (PcdRootPort2ClkInfo, RootPort[ 2].Blob);
  PcdSet64S (PcdRootPort3ClkInfo, RootPort[ 3].Blob);
  PcdSet64S (PcdRootPort4ClkInfo, RootPort[ 4].Blob);
  PcdSet64S (PcdRootPort5ClkInfo, RootPort[ 5].Blob);
  PcdSet64S (PcdRootPort6ClkInfo, RootPort[ 6].Blob);
  PcdSet64S (PcdRootPort7ClkInfo, RootPort[ 7].Blob);
  PcdSet64S (PcdRootPort8ClkInfo, RootPort[ 8].Blob);
  PcdSet64S (PcdRootPort9ClkInfo, RootPort[ 9].Blob);
  PcdSet64S (PcdRootPort10ClkInfo, RootPort[10].Blob);
  PcdSet64S (PcdRootPort11ClkInfo, RootPort[11].Blob);
  PcdSet64S (PcdRootPort12ClkInfo, RootPort[12].Blob);
  PcdSet64S (PcdRootPort13ClkInfo, RootPort[13].Blob);
  PcdSet64S (PcdRootPort14ClkInfo, RootPort[14].Blob);
  PcdSet64S (PcdRootPort15ClkInfo, RootPort[15].Blob);
  PcdSet64S (PcdRootPort16ClkInfo, RootPort[16].Blob);
  PcdSet64S (PcdRootPort17ClkInfo, RootPort[17].Blob);
  PcdSet64S (PcdRootPort18ClkInfo, RootPort[18].Blob);
  PcdSet64S (PcdRootPort19ClkInfo, RootPort[19].Blob);
  PcdSet64S (PcdRootPort20ClkInfo, RootPort[20].Blob);
  PcdSet64S (PcdRootPort21ClkInfo, RootPort[21].Blob);
  PcdSet64S (PcdRootPort22ClkInfo, RootPort[22].Blob);
  PcdSet64S (PcdRootPort23ClkInfo, RootPort[23].Blob);
  PcdSet64S (PcdRootPort24ClkInfo, RootPort[24].Blob);
  PcdSet64S (PcdRootPort25ClkInfo, RootPort[25].Blob);
  PcdSet64S (PcdRootPort26ClkInfo, RootPort[26].Blob);
  PcdSet64S (PcdRootPort27ClkInfo, RootPort[27].Blob);
  PcdSet64S (PcdRootPort28ClkInfo, RootPort[28].Blob);
  PcdSet64S (PcdRootPort29ClkInfo, RootPort[29].Blob);
  PcdSet64S (PcdRootPort30ClkInfo, RootPort[30].Blob);
  PcdSet64S (PcdRootPort31ClkInfo, RootPort[31].Blob);
  PcdSet64S (PcdRootPortLanClkInfo, Lan.Blob);

  return EFI_SUCCESS;
}

/**
  Others Board USB related configuration init function for PEI pre-memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.
  @param  BoardId       BoardId enum value.
  @param  OrgBoardId    Original BoardId enum value.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersUsbConfigInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 OrgBoardId
  )
{
  PCD32_BLOB *UsbPort20Afe;

  UsbPort20Afe = AllocateZeroPool (PCH_MAX_USB2_PORTS * sizeof (PCD32_BLOB));
  ASSERT (UsbPort20Afe != NULL);
  if (UsbPort20Afe == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // USB2 AFE settings.
  //
  if (PlatformInfo->PlatformType == TypeUltUlx) {

    UsbPort20Afe[0].Info.Petxiset     = 7;
    UsbPort20Afe[0].Info.Txiset       = 0;
    UsbPort20Afe[0].Info.Predeemp     = 2;
    UsbPort20Afe[0].Info.Pehalfbit    = 1;

    UsbPort20Afe[1].Info.Petxiset     = 7;
    UsbPort20Afe[1].Info.Txiset       = 0;
    UsbPort20Afe[1].Info.Predeemp     = 2;
    UsbPort20Afe[1].Info.Pehalfbit    = 1;

    UsbPort20Afe[2].Info.Petxiset     = 7;
    UsbPort20Afe[2].Info.Txiset       = 0;
    UsbPort20Afe[2].Info.Predeemp     = 2;
    UsbPort20Afe[2].Info.Pehalfbit    = 1;

    UsbPort20Afe[3].Info.Petxiset     = 7;
    UsbPort20Afe[3].Info.Txiset       = 0;
    UsbPort20Afe[3].Info.Predeemp     = 2;
    UsbPort20Afe[3].Info.Pehalfbit    = 1;

    UsbPort20Afe[4].Info.Petxiset     = 7;
    UsbPort20Afe[4].Info.Txiset       = 0;
    UsbPort20Afe[4].Info.Predeemp     = 2;
    UsbPort20Afe[4].Info.Pehalfbit    = 1;

    UsbPort20Afe[5].Info.Petxiset     = 7;
    UsbPort20Afe[5].Info.Txiset       = 0;
    UsbPort20Afe[5].Info.Predeemp     = 2;
    UsbPort20Afe[5].Info.Pehalfbit    = 1;

    UsbPort20Afe[6].Info.Petxiset     = 7;
    UsbPort20Afe[6].Info.Txiset       = 0;
    UsbPort20Afe[6].Info.Predeemp     = 2;
    UsbPort20Afe[6].Info.Pehalfbit    = 1;

    UsbPort20Afe[7].Info.Petxiset     = 7;
    UsbPort20Afe[7].Info.Txiset       = 0;
    UsbPort20Afe[7].Info.Predeemp     = 2;
    UsbPort20Afe[7].Info.Pehalfbit    = 1;

    UsbPort20Afe[8].Info.Petxiset     = 7;
    UsbPort20Afe[8].Info.Txiset       = 0;
    UsbPort20Afe[8].Info.Predeemp     = 2;
    UsbPort20Afe[8].Info.Pehalfbit    = 1;

    UsbPort20Afe[9].Info.Petxiset     = 7;
    UsbPort20Afe[9].Info.Txiset       = 0;
    UsbPort20Afe[9].Info.Predeemp     = 2;
    UsbPort20Afe[9].Info.Pehalfbit    = 1;

  } else if (PlatformInfo->PlatformType == TypeTrad) {

    UsbPort20Afe[0].Info.Petxiset     = 7;
    UsbPort20Afe[0].Info.Txiset       = 0;
    UsbPort20Afe[0].Info.Predeemp     = 2;
    UsbPort20Afe[0].Info.Pehalfbit    = 1;

    UsbPort20Afe[1].Info.Petxiset     = 7;
    UsbPort20Afe[1].Info.Txiset       = 0;
    UsbPort20Afe[1].Info.Predeemp     = 2;
    UsbPort20Afe[1].Info.Pehalfbit    = 1;

    UsbPort20Afe[2].Info.Petxiset     = 7;
    UsbPort20Afe[2].Info.Txiset       = 0;
    UsbPort20Afe[2].Info.Predeemp     = 2;
    UsbPort20Afe[2].Info.Pehalfbit    = 1;

    UsbPort20Afe[3].Info.Petxiset     = 7;
    UsbPort20Afe[3].Info.Txiset       = 0;
    UsbPort20Afe[3].Info.Predeemp     = 2;
    UsbPort20Afe[3].Info.Pehalfbit    = 1;

    UsbPort20Afe[4].Info.Petxiset     = 7;
    UsbPort20Afe[4].Info.Txiset       = 0;
    UsbPort20Afe[4].Info.Predeemp     = 2;
    UsbPort20Afe[4].Info.Pehalfbit    = 1;

    UsbPort20Afe[5].Info.Petxiset     = 7;
    UsbPort20Afe[5].Info.Txiset       = 0;
    UsbPort20Afe[5].Info.Predeemp     = 2;
    UsbPort20Afe[5].Info.Pehalfbit    = 1;

    UsbPort20Afe[6].Info.Petxiset     = 7;
    UsbPort20Afe[6].Info.Txiset       = 0;
    UsbPort20Afe[6].Info.Predeemp     = 2;
    UsbPort20Afe[6].Info.Pehalfbit    = 1;

    UsbPort20Afe[7].Info.Petxiset     = 7;
    UsbPort20Afe[7].Info.Txiset       = 0;
    UsbPort20Afe[7].Info.Predeemp     = 2;
    UsbPort20Afe[7].Info.Pehalfbit    = 1;

    UsbPort20Afe[8].Info.Petxiset     = 7;
    UsbPort20Afe[8].Info.Txiset       = 0;
    UsbPort20Afe[8].Info.Predeemp     = 2;
    UsbPort20Afe[8].Info.Pehalfbit    = 1;

    UsbPort20Afe[9].Info.Petxiset     = 7;
    UsbPort20Afe[9].Info.Txiset       = 0;
    UsbPort20Afe[9].Info.Predeemp     = 2;
    UsbPort20Afe[9].Info.Pehalfbit    = 1;

    UsbPort20Afe[10].Info.Petxiset    = 7;
    UsbPort20Afe[10].Info.Txiset      = 0;
    UsbPort20Afe[10].Info.Predeemp    = 2;
    UsbPort20Afe[10].Info.Pehalfbit   = 1;

    UsbPort20Afe[11].Info.Petxiset    = 7;
    UsbPort20Afe[11].Info.Txiset      = 0;
    UsbPort20Afe[11].Info.Predeemp    = 2;
    UsbPort20Afe[11].Info.Pehalfbit   = 1;

    UsbPort20Afe[12].Info.Petxiset    = 7;
    UsbPort20Afe[12].Info.Txiset      = 0;
    UsbPort20Afe[12].Info.Predeemp    = 2;
    UsbPort20Afe[12].Info.Pehalfbit   = 1;

    UsbPort20Afe[13].Info.Petxiset    = 7;
    UsbPort20Afe[13].Info.Txiset      = 0;
    UsbPort20Afe[13].Info.Predeemp    = 2;
    UsbPort20Afe[13].Info.Pehalfbit   = 1;

    UsbPort20Afe[14].Info.Petxiset    = 7;
    UsbPort20Afe[14].Info.Txiset      = 0;
    UsbPort20Afe[14].Info.Predeemp    = 2;
    UsbPort20Afe[14].Info.Pehalfbit   = 1;

    UsbPort20Afe[15].Info.Petxiset    = 7;
    UsbPort20Afe[15].Info.Txiset      = 0;
    UsbPort20Afe[15].Info.Predeemp    = 2;
    UsbPort20Afe[15].Info.Pehalfbit   = 1;
  }

  PcdSet32S (PcdUsb20Port0Afe,  UsbPort20Afe[ 0].Blob);
  PcdSet32S (PcdUsb20Port1Afe,  UsbPort20Afe[ 1].Blob);
  PcdSet32S (PcdUsb20Port2Afe,  UsbPort20Afe[ 2].Blob);
  PcdSet32S (PcdUsb20Port3Afe,  UsbPort20Afe[ 3].Blob);
  PcdSet32S (PcdUsb20Port4Afe,  UsbPort20Afe[ 4].Blob);
  PcdSet32S (PcdUsb20Port5Afe,  UsbPort20Afe[ 5].Blob);
  PcdSet32S (PcdUsb20Port6Afe,  UsbPort20Afe[ 6].Blob);
  PcdSet32S (PcdUsb20Port7Afe,  UsbPort20Afe[ 7].Blob);
  PcdSet32S (PcdUsb20Port8Afe,  UsbPort20Afe[ 8].Blob);
  PcdSet32S (PcdUsb20Port9Afe,  UsbPort20Afe[ 9].Blob);
  PcdSet32S (PcdUsb20Port10Afe, UsbPort20Afe[10].Blob);
  PcdSet32S (PcdUsb20Port11Afe, UsbPort20Afe[11].Blob);
  PcdSet32S (PcdUsb20Port12Afe, UsbPort20Afe[12].Blob);
  PcdSet32S (PcdUsb20Port13Afe, UsbPort20Afe[13].Blob);
  PcdSet32S (PcdUsb20Port14Afe, UsbPort20Afe[14].Blob);
  PcdSet32S (PcdUsb20Port15Afe, UsbPort20Afe[15].Blob);

  //
  // USB Port Over Current Pin
  //

  PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort14, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb20OverCurrentPinPort15, PchUsbOverCurrentPinMax);

  PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort6, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort7, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort8, PchUsbOverCurrentPinMax);
  PcdSet8S (PcdUsb30OverCurrentPinPort9, PchUsbOverCurrentPinMax);

  switch (BoardId) {
    case BoardIdSkylakeDtRvp8Crb:
//ray_override / [XI-BringUp] Bring Up Porting / Tuning Board Override / USB Port OC Mapping / Modified >>
//      if ((OrgBoardId == BoardIdKabyLakeSDdr4UdimmEvErb) ||
//        (OrgBoardId == BoardIdKabyLakeSDdr4UdimmEvCrb) ||
//        (OrgBoardId == BoardIdKabyLakeSDdr4UdimmCrb)) {
//        // Based on KBL-S schematic
//
//        // OC Map for USB2 Ports
//        PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin0);
//        PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin0); // Port unused on KBL-S
//        PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPin3);
//        PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPin4);
//        PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPin5);
//        PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin6);
//        PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPin4);
//        PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPin2);
//        PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPinSkip);
//        PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPin2);
//        PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPin1);
//        PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPin5);
//        PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPin1);
//        PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);
//
//        // OC Map for USB3 Ports
//        PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
//        PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin1);
//        PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin1);
//        PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
//        PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin0); // Port unused on KBL-S
//        PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPin3);
//        PcdSet8S (PcdUsb30OverCurrentPinPort6, PchUsbOverCurrentPin2);
//        PcdSet8S (PcdUsb30OverCurrentPinPort7, PchUsbOverCurrentPin2);
//        PcdSet8S (PcdUsb30OverCurrentPinPort8, PchUsbOverCurrentPin6);
//        PcdSet8S (PcdUsb30OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
//      } else {
//        // Based on SKL AIO RVP8 schematic
//
//        // OC Map for USB2 Ports
//        PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin2);
//        PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin5);
//        PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPin4);
//        PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPin4);
//        PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPin1);
//        PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin1);
//        PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
//        PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPin0);
//        PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPin5);
//        PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPin2);
//        PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
//        PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPin6);
//        PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPin3);
//        PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPin6);
//
//        // OC Map for USB3 Ports
//        PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPinSkip);
//        PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin1);
//        PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin1);
//        PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin0);
//        PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin0);
//        PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
//        PcdSet8S (PcdUsb30OverCurrentPinPort6, PchUsbOverCurrentPin2);
//        PcdSet8S (PcdUsb30OverCurrentPinPort7, PchUsbOverCurrentPin2);
//        PcdSet8S (PcdUsb30OverCurrentPinPort8, PchUsbOverCurrentPin3);
//        PcdSet8S (PcdUsb30OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
//      }
        // OC Map for USB2 Ports
        PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin0);
        PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin0);
        PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPin1);
        PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPin1);
        PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPin4);
        PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin4);
        PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPin4);
        PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPin4);
        PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);
        // OC Map for USB3 Ports
        PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
        PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin0);
        PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin1);
        PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin1);
        PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb30OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb30OverCurrentPinPort7, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb30OverCurrentPinPort8, PchUsbOverCurrentPinSkip);
        PcdSet8S (PcdUsb30OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
//ray_override / [XI-BringUp] Bring Up Porting / Tuning Board Override / USB Port OC Mapping / Modified <<
      break;

    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Erb:
      // Based on SKL AIO RVP9/10 schematic

      // OC Map for USB2 Ports
      PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin5);
      PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin5);
      PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPin4);
      PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPin4);
      PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);

      // OC Map for USB3 Ports
      PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort6, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb30OverCurrentPinPort7, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb30OverCurrentPinPort8, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
      break;


    case BoardIdSkylakeSds:
    case BoardIdSkylakePantherMtn:
    case BoardIdSkylakeStarbrook:
    case BoardIdKabylakeGrizzlyMtn:
      // Based on SKL SDS Schematic
      // Default connection:
      // USB2_P1 and USB3_P1  connected to USB_OC_0_OTG
      // USB2_P2 and USB3_P2  connected to USB_OC_1_WALKUP

      // OC Map for USB2 Ports
      PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);

      // OC Map for USB3 Ports
      PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
      break;

    case BoardIdKabylakeCoyoteMtn:
      // Based on SKL SDS Schematic
      // Default connection:
      // USB2_P1 and USB3_P1  connected to USB_OC_0_OTG
      // USB2_P2 and USB3_P2  connected to USB_OC_1_WALKUP

      // OC Map for USB2 Ports
      PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);

      // OC Map for USB3 Ports
      PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
      break;

    case BoardIdZumbaBeachServerEv:
      PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPin3);
      PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPin4);
      PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPin6);
      PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPin5);
      PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPin6);
      PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPin7);

      PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin4);
      PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin5);
      PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin3);
      PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPin7);
      break;

    case BoardIdZumbaBeachServerCrb:
      PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPin3);
      PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPin4);
      PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPin6);
      PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPin5);
      PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPin6);
      PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin4);
      PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin5);
      PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin3);
      PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
      break;

    case BoardIdSkylakeSdlBrk:
      // Based on Saddlebrook schematic

      // OC Map for USB2 Ports
      PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPin3);
      PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPin3);
      PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPin4);
      PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPin4);
      PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);

      // OC Map for USB3 Ports
      PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin0);
      PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin1);
      PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPin2);
      PcdSet8S (PcdUsb30OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort7, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort8, PchUsbOverCurrentPinSkip);
      PcdSet8S (PcdUsb30OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
      break;
  }

  return EFI_SUCCESS;
}

/**
  Others Board GPIO Group Tier configuration init function for PEI pre-memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
OthersGpioGroupTierInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId
  )
{
  //
  // GPIO Group Tier
  //

  switch (BoardId) {
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10Erb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeSlpfPpv:
    case BoardIdZumbaBeachServerEv:
    case BoardIdZumbaBeachServerCrb:
    case BoardIdSkylakeSdlBrk:
      PcdSet32S (PcdGpioGroupToGpeDw0, GPIO_SKL_H_GROUP_GPP_G);
      PcdSet32S (PcdGpioGroupToGpeDw1, GPIO_SKL_H_GROUP_GPP_D);
      PcdSet32S (PcdGpioGroupToGpeDw2, GPIO_SKL_H_GROUP_GPP_E);
      break;
    default:
      PcdSet32S (PcdGpioGroupToGpeDw0, 0);
      PcdSet32S (PcdGpioGroupToGpeDw1, 0);
      PcdSet32S (PcdGpioGroupToGpeDw2, 0);
      break;
  }

  return EFI_SUCCESS;
}