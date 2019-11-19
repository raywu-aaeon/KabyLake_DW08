/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2019 Intel Corporation.

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

#include "PeiSkylakeA0Rvp3InitLib.h"
#include "SkylakeA0Rvp3GpioTable.h"
#include "SkylakeA0Rvp3HsioPtssTables.h"
#include "SkylakeA0Rvp3SpdTable.h"

#include <ConfigBlock.h>
#ifndef MINTREE_FLAG
#include <ConfigBlock/SwitchableGraphicsConfig.h>   // for PCIE RTD3 GPIO
#endif
#include <PlatformBoardConfig.h>                    // for USB 20 AFE & Root Port Clk Info
#include <Library/PeiPlatformHookLib.h>             // For Recovery Mode GPIO

#ifndef MINTREE_FLAG
#include <Library/PchGbeLib.h>
#endif


//
// Reference RCOMP resistors on motherboard - for SKL RVP1
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT16 RcompResistorSklRvp1[SA_MRC_MAX_RCOMP] = { 200, 81, 162 };
//
// RCOMP target values for RdOdt, WrDS, WrDSCmd, WrDSCtl, WrDSClk - for SKL RVP1
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT16 RcompTargetSklRvp1[SA_MRC_MAX_RCOMP_TARGETS] = { 100, 40, 40, 23, 40 };

//
// RVP3 Init Table
//
BOARD_INIT_TABLE mRvp3BoardInitPreMem[] = {
  {BoardIdSkylakeA0Rvp3,           PeiSkylakeA0Rvp3InitPreMem},
  {BoardIdKabyLakeYLpddr3Rvp3,     PeiSkylakeA0Rvp3InitPreMem},
  {BoardIdAmberLakeY42Lpddr3Rvp3,  PeiSkylakeA0Rvp3InitPreMem},
  {BoardIdAmberLakeLpddr3Rvp3,     PeiSkylakeA0Rvp3InitPreMem},
  {BoardIdAmberLakeY42Lpddr3Cff,   PeiSkylakeA0Rvp3InitPreMem},
  {BoardIdAmberLakeY42Lpddr3CffCc, PeiSkylakeA0Rvp3InitPreMem},
#ifndef MINTREE_FLAG
  {BoardIdSkylakeA0Rvp3Ppv,        PeiSkylakeA0Rvp3PpvInitPreMem},
#endif
  { 0, 0 }
};


// @todo for BoardInit() early Init

EFI_STATUS
PeiSkylakeA0Rvp3CommonInit (
  IN PLATFORM_INFO *PlatformInfo
  );

/**
  SkylaeA0Rvp3 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeA0Rvp3InitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS    Status;
  PLATFORM_INFO *PlatformInfo;
  PCD64_BLOB    RootPort;
  PCD64_BLOB    PcdData;
  PCD32_BLOB    UsbPort20Afe;
  UINT16        BoardId;
  UINT16        BoardIdOrgValue;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);


  PlatformInfo = (PLATFORM_INFO *) Content;
#ifndef MINTREE_FLAG
  PcdSet8S (PcdRootPortDev, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1);
  PcdSet8S (PcdRootPortFunc, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1);

  PcdSet8S  (PcdPcie0GpioSupport, I2CGpio);
  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdSet8S  (PcdPcie0GpioSupport, PchGpio);
  }
#endif
  PcdSet32S (PcdPcie0WakeGpioNo, 0);
  PcdSet8S  (PcdPcie0HoldRstExpanderNo, 0);
  PcdSet32S (PcdPcie0HoldRstGpioNo, 8);
  PcdSetBoolS (PcdPcie0HoldRstActive, TRUE);
  PcdSet8S  (PcdPcie0PwrEnableExpanderNo, 0);
  PcdSet32S (PcdPcie0PwrEnableGpioNo, 16);
  PcdSetBoolS (PcdPcie0PwrEnableActive, FALSE);
  PcdSet8S (PcdPcie0ClkReqNumber, 8);

  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdSet32S (PcdPcie0WakeGpioNo, 0);
    PcdSet8S  (PcdPcie0HoldRstExpanderNo, 0);
    PcdSet32S (PcdPcie0HoldRstGpioNo,GPIO_SKL_LP_GPP_B15);
    PcdSetBoolS (PcdPcie0HoldRstActive, FALSE);
    PcdSet8S  (PcdPcie0PwrEnableExpanderNo, 0);
    PcdSet32S (PcdPcie0PwrEnableGpioNo, GPIO_SKL_LP_GPP_E2);
    PcdSetBoolS (PcdPcie0PwrEnableActive, TRUE);
    PcdSet8S (PcdPcie0ClkReqNumber, 8);
  }
  //
  // HSIO PTSS Table
  //
  PcdSet32S (PcdSpecificLpHsioPtssTable1,     (UINTN) PchLpHsioPtss_Bx_SkylakeA0Rvp3);
  PcdSet16S (PcdSpecificLpHsioPtssTable1Size, (UINTN) PchLpHsioPtss_Bx_SkylakeA0Rvp3_Size);
  PcdSet32S (PcdSpecificLpHsioPtssTable2,     (UINTN) PchLpHsioPtss_Cx_SkylakeA0Rvp3);
  PcdSet16S (PcdSpecificLpHsioPtssTable2Size, (UINTN) PchLpHsioPtss_Cx_SkylakeA0Rvp3_Size);

  //
  // RootPort
  //
  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE;
  RootPort.Info.ClkReqNumber = 2;
  PcdSet64S (PcdRootPort0ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  if (PlatformInfo->BoardBomId == 0x4) {  // BOM SKU 3-1
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 1;
    PcdSet64S (PcdRootPort4ClkInfo, RootPort.Blob);
  } else {
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 3;

#ifndef MINTREE_FLAG
    if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) && (!PchIsGbeAvailable ())) {
// Since on AML Y42, RP05 is connected to M.2 SSD not GBE by default.
// So CLK Assign is CLK_SRC1 to PCIE05.
      RootPort.Info.ClkReqNumber = 1;
    }
    if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
        (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
      RootPort.Info.ClkReqNumber = 1;
    }
#endif

    PcdSet64S (PcdRootPort4ClkInfo, RootPort.Blob);

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 1;
    PcdSet64S (PcdRootPort5ClkInfo, RootPort.Blob);
  }

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE;
// Since at given point of time (based on board rework) either PCIE6 or PCIE8
// will get CLK_SRC1. Hence both ports are assigned same CLKREQ number here.
  RootPort.Info.ClkReqNumber = 1;
  PcdSet64S (PcdRootPort7ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE;
  RootPort.Info.ClkReqNumber = 5;
  PcdSet64S (PcdRootPort8ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE;
  RootPort.Info.ClkReqNumber = 4;
  PcdSet64S (PcdRootPort9ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE;
  RootPort.Info.ClkReqNumber    = 3;
  PcdSet64S (PcdRootPortLanClkInfo, RootPort.Blob);

  //
  // DRAM related definition
  //
  PcdSet8S (PcdSaMiscUserBd, 5);

  PcdSet8S (PcdMrcSpdAddressTable0, 0xA2);
  PcdSet8S (PcdMrcSpdAddressTable1, 0xA0);
  PcdSet8S (PcdMrcSpdAddressTable2, 0xA2);
  PcdSet8S (PcdMrcSpdAddressTable3, 0xA0);

  PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
  PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);

  if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    if ((PlatformInfo->BoardBomId == 0x5) || (PlatformInfo->BoardBomId == 0x6) || (PlatformInfo->BoardBomId == 0x7)) {
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) mDqsMapCpu2DramAml42Rvp3_x64);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (mDqsMapCpu2DramAml42Rvp3_x64));
    } else {
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) mDqsMapCpu2DramAml42Rvp3);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (mDqsMapCpu2DramAml42Rvp3));
    }
    PcdSet32S (PcdMrcDqByteMap, (UINTN) mDqByteMapAml42Rvp3);
    PcdSet16S (PcdMrcDqByteMapSize, sizeof (mDqByteMapAml42Rvp3));
  } else if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) {
    PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) mDqsMapCpu2DramAml42Lpddr3Cff);
    PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (mDqsMapCpu2DramAml42Lpddr3Cff));
    PcdSet32S (PcdMrcDqByteMap, (UINTN) mDqByteMapAml42Lpddr3Cff);
    PcdSet16S (PcdMrcDqByteMapSize, sizeof (mDqByteMapAml42Lpddr3Cff));
  } else if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc) {
    PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) mDqsMapCpu2DramAml42Rvp3);
    PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (mDqsMapCpu2DramAml42Rvp3));
    PcdSet32S (PcdMrcDqByteMap, (UINTN) mDqByteMapAml42Rvp3);
    PcdSet16S (PcdMrcDqByteMapSize, sizeof (mDqByteMapAml42Rvp3));
  } else {
// AMI_OVERRIDE_START - Fix build error. 
#if 0
    PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) mDqsMapCpu2DramSklRvp3);
    PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (mDqsMapCpu2DramSklRvp3));
    PcdSet32S (PcdMrcDqByteMap, (UINTN) mDqByteMapSklRvp3);
    PcdSet16S (PcdMrcDqByteMapSize, sizeof (mDqByteMapSklRvp3));
#else
    PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklRvp3);
    PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklRvp3));
    PcdSet32S (PcdMrcDqByteMap, (UINTN) DqByteMapSklRvp3);
    PcdSet16S (PcdMrcDqByteMapSize, sizeof (DqByteMapSklRvp3));
#endif
// AMI_OVERRIDE_END - Fix build error.   
  }
#ifdef MEM_DOWN_FLAG
  if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    if ((PlatformInfo->BoardBomId == 0x5) || (PlatformInfo->BoardBomId == 0x7)){
      // Samsung memory part. K50369-001
      PcdSet32S (PcdMrcSpdData, (UINTN) mAmlSamsungLpddr31867Spd);
      PcdSet16S (PcdMrcSpdDataSize, mAmlSamsungLpddr31867SpdSize);
    } else if (PlatformInfo->BoardBomId == 0x6) {
      // Micron MT52L512M64D4PQ-093 J68025-001
      PcdSet32S (PcdMrcSpdData, (UINTN) mSpdLpddr32133);
      PcdSet16S (PcdMrcSpdDataSize, mSpdLpddr32133Size);
    } else {
      PcdSet32S (PcdMrcSpdData, (UINTN) mAmberLakeLpddr32133Spd);
      PcdSet16S (PcdMrcSpdDataSize, mAmberLakeLpddr32133SpdSize);
    }
  } else if (BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) {
    PcdSet32S (PcdMrcSpdData, (UINTN) mAmberLakeLpddr32133Spd);
    PcdSet16S (PcdMrcSpdDataSize, mAmberLakeLpddr32133SpdSize);
  } else if (BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3) {
    PcdSet32S (PcdMrcSpdData, (UINTN) mSkylakeRvp3Spd110);
    PcdSet16S (PcdMrcSpdDataSize, mSkylakeRvp3Spd110Size);
  } else if (BoardIdOrgValue == BoardIdSkylakeA0Rvp3) {
    if (PlatformInfo->BoardBomId == 0x6) { // Hynix H9CCNNNBJTMLAR-NUD, 8Gb, DDP, 1867
      PcdSet32S (PcdMrcSpdData, (UINTN) mSkylakeRvp3Spd110);
      PcdSet16S (PcdMrcSpdDataSize, mSkylakeRvp3Spd110Size);
    }
    if (PlatformInfo->BoardBomId == 0x7) { // Samsung K4E6E304EE-EGCF, 4Gb, QDP, 1867
      PcdSet32S (PcdMrcSpdData, (UINTN) mSkylakeRvp16Spd);
      PcdSet16S (PcdMrcSpdDataSize, mSkylakeRvp16SpdSize);
    }
  } else if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) {
      if (PlatformInfo->BoardBomId == 0x1) { // K44922-001, SKYNIX 8GB
        PcdSet32S (PcdMrcSpdData, (UINTN) mAmberLakeLpddr3ODP2133Spd);
        PcdSet16S (PcdMrcSpdDataSize, mAmberLakeLpddr3ODP2133SpdSize);
      } else if (PlatformInfo->BoardBomId == 0x3) { // Micron MT52L512M64D4PQ-093 WT:B 4G
        PcdSet32S (PcdMrcSpdData, (UINTN) mSpdLpddr32133);
        PcdSet16S (PcdMrcSpdDataSize, mSpdLpddr32133Size);
      } else { // BOM ID 0x01 Micron MT52L1G64D8QC-107 WT:B, 8GB
        PcdSet32S (PcdMrcSpdData, (UINTN) mAmberLakeLpddr3ODP1867Spd);
        PcdSet16S (PcdMrcSpdDataSize, mAmberLakeLpddr3ODP1867SpdSize);
      }
  } else if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc) {
    PcdSet32S (PcdMrcSpdData, (UINTN) mAmlSamsungLpddr31867Spd);
    PcdSet16S (PcdMrcSpdDataSize, mAmlSamsungLpddr31867SpdSize);
  }
#ifndef MINTREE_FLAG
  else if (BoardIdOrgValue == BoardIdSkylakeA0Rvp3Ppv) {
    if (PlatformInfo->BoardBomId == 0x1) {
      PcdSet32S (PcdMrcSpdData, (UINTN) mAmberLakeLpddr32133Spd);
      PcdSet16S (PcdMrcSpdDataSize, mAmberLakeLpddr32133SpdSize);
    }
  }
#endif
  if (!PlatformInfo->SpdPresent) {
    PcdSet8S (PcdMrcSpdAddressTable0, 0);
    PcdSet8S (PcdMrcSpdAddressTable1, 0);
    PcdSet8S (PcdMrcSpdAddressTable2, 0);
    PcdSet8S (PcdMrcSpdAddressTable3, 0);
  }
#endif

  //
  // USB 2.0 Port AFE
  //
  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port0Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port1Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port2Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port3Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port4Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port5Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port6Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port7Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 5;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port8Afe,  UsbPort20Afe.Blob);

  UsbPort20Afe.Blob = 0;
  UsbPort20Afe.Info.Petxiset     = 7;
  UsbPort20Afe.Info.Txiset       = 0;
  UsbPort20Afe.Info.Predeemp     = 2;
  UsbPort20Afe.Info.Pehalfbit    = 1;
  PcdSet32S (PcdUsb20Port9Afe,  UsbPort20Afe.Blob);

  //
  // USB 2.0 & 3.0 Port Overcurrent
  //
  if (BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3 ||
      BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) {
    // Based on KabyLake Y Lpddr3 Rvp3 CRB RVP PBA : H35311-101 board schematic
    // OC Map for USB2 Ports
    PcdSet8S (PcdUsb20OverCurrentPinPort0,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort1,  PchUsbOverCurrentPin0);
    PcdSet8S (PcdUsb20OverCurrentPinPort2,  PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort3,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort4,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort5,  PchUsbOverCurrentPin1);
    PcdSet8S (PcdUsb20OverCurrentPinPort6,  PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort7,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort8,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort9,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);
  if (PlatformInfo->BoardRev == 0) {
    //Skip the OC map for Port 1 and Port 5 for KBL RVP3 Fab 0 due to Board/PCH Limitations
    PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
  }

    // OC Map for USB3 Ports
    PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin0);
    PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin1);
    PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPin2);
  } else if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    // Based on AmberLake Y42 Lpddr3 Rvp3 CRB RVP PBA
    // OC Map for USB2 Ports
    PcdSet8S (PcdUsb20OverCurrentPinPort0,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort1,  PchUsbOverCurrentPin0);
    PcdSet8S (PcdUsb20OverCurrentPinPort2,  PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort3,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort4,  PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort5,  PchUsbOverCurrentPin0);
    PcdSet8S (PcdUsb20OverCurrentPinPort6,  PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort7,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort8,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort9,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);

    // OC Map for USB3 Ports
    PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin0);
    PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin1);
    PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPin2);
  } else if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
             (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    // Based on AmberLake Y42 Lpddr3 Rvp3 CRB RVP PBA
    // OC Map for USB2 Ports
    PcdSet8S (PcdUsb20OverCurrentPinPort0,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort1,  PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort2,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort3,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort4,  PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort5,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort6,  PchUsbOverCurrentPin1);
    PcdSet8S (PcdUsb20OverCurrentPinPort7,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort8,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort9,  PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);

    // OC Map for USB3 Ports
    PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
  }else {
    // Based on SKL A0 LPDDR3 CRB RVP PBA : H35311-101 board schematic
    // Default connection:
    // USB_P1_WP1_OTG and USB3_P1_WP1  connected to USB_OC_0
    // USB_P9_WP4     and USB3_P4_WP4  connected to USB_OC_1
    // USB_P5_WP2                      connected to USB_OC_2
    // USB_P2_WP5                      connected to USB_OC_2

    // OC Map for USB2 Ports
    PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin0);
    PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPin1);
    PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);

    // OC Map for USB3 Ports
    PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
    PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin1);
    PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
  }
  PcdSetBoolS (PcdIoExpanderPresent, TRUE);
  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdSetBoolS (PcdIoExpanderPresent, FALSE);
  }
  //
  // RecoveryMode GPIO
  //
  PcdData.Blob = 0;
  PcdData.BoardGpioConfig.Type = BoardGpioTypeExpander;
  PcdData.BoardGpioConfig.u.Expander.Section = IO_EXPANDER_0;
  PcdData.BoardGpioConfig.u.Expander.Pin = IO_EXPANDER_GPIO_11;
  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdData.Blob = 0;
    PcdData.BoardGpioConfig.Type = BoardGpioTypeNotSupported;
  }
  PcdSet64S (PcdRecoveryModeGpio, PcdData.Blob);
#ifndef MINTREE_FLAG
  PcdSetBoolS (PcdIvCamInitPresent, TRUE);

  if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    PcdData.Blob = 0;
    PcdData.BoardGpioConfig.Type = BoardGpioTypeExpander;
    PcdData.BoardGpioConfig.u.Expander.Section = IO_EXPANDER_1;
    PcdData.BoardGpioConfig.u.Expander.Pin = IO_EXPANDER_GPIO_0;
    PcdSet64S (PcdWwanFullCardPowerOffGpio, PcdData.Blob);
    PcdSet32S (PcdWwanBbrstGpio, GPIO_SKL_LP_GPP_D9);
  }
#endif

  return EFI_SUCCESS;
}

#ifndef MINTREE_FLAG
/**
  SkylaeA0Rvp3Ppv board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeA0Rvp3PpvInitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS Status;

  Status = PeiSkylakeA0Rvp3InitPreMem (Content);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PcdSet32S (PcdSpecificLpHsioPtssTable1, (UINTN) PchLpHsioPtss_Bx_SkylakeA0Rvp3Ppv);
  PcdSet32S (PcdSpecificLpHsioPtssTable2, (UINTN) PchLpHsioPtss_Cx_SkylakeA0Rvp3Ppv);
  PcdSet16S (PcdSpecificLpHsioPtssTable1Size, PchLpHsioPtss_Bx_SkylakeA0Rvp3Ppv_Size);
  PcdSet16S (PcdSpecificLpHsioPtssTable2Size, PchLpHsioPtss_Cx_SkylakeA0Rvp3Ppv_Size);

  return EFI_SUCCESS;
}
#endif

/**
  The constructor function for SkylakeA0Rvp3 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiSkylakeA0Rvp3BoardInitPreMemLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp3BoardInitPreMem, TABLE_LIST_NUM);
}

