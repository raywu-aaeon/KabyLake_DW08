/** @file

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

#include "PeiSkylakeURvp7InitLib.h"
#include <GpioPinsSklLp.h>
#include "SkylakeURvp7HsioPtssTables.h"

#include <ConfigBlock.h>
#include <ConfigBlock/SwitchableGraphicsConfig.h>   // for PCIE RTD3 GPIO
#include <PlatformBoardConfig.h>                    // for USB 20 AFE & Root Port Clk Info
#include <Library/PeiPlatformHookLib.h>             // For Recovery Mode GPIO

extern UINT16 RcompResistorSklRvp2[];
extern UINT16 RcompTargetSklRvp2[];
extern UINT16 RcompResistorSklRvp1[];
extern UINT16 RcompTargetSklRvp1[];
extern UINT8 mSkylakeRvp3Spd110[];
extern UINT16 mSkylakeRvp3Spd110Size;
extern UINT8  DqByteMapSklRvp5[2][6][2];
extern UINT16 DqByteMapSklRvp5Size;
extern UINT8 DqsMapCpu2DramSklRvp5[2][8];
extern UINT8 mSkylakeRvp16Spd[];
extern UINT16 mSkylakeRvp16SpdSize;
extern UINT8 mSpdLpddr32133[];
extern UINT16 mSpdLpddr32133Size;
extern UINT8 mKblRSpdLpddr32133[];
extern UINT16 mKblRSpdLpddr32133Size;

//
// RVP7 Init Table
//
BOARD_INIT_TABLE mRvp7BoardInitPreMem [] = {
  {BoardIdSkylakeURvp7, PeiSkylakeURvp7InitPreMem},
  {BoardIdKabylakeRDdr4, PeiSkylakeURvp7InitPreMem},
  {BoardIdKabylakeRLpddr3, PeiSkylakeURvp7InitPreMem},
  {BoardIdKabylakeKcDdr3, PeiSkylakeURvp7InitPreMem},
  {BoardIdSkylakeURvp7Ppv, PeiSkylakeURvp7PpvInitPreMem},
  { 0, 0 }
};

EFI_STATUS
GetBoardId (
  OUT UINT16 *BoardId,
  OUT UINT16 *BoardIdOrgValue
  );

/**
  SkylaeURvp7 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
PeiSkylakeURvp7CommonInitPreMem (
  IN VOID *Content
  )
{
  PLATFORM_INFO *PlatformInfo;
  PCD64_BLOB RootPort;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;
  EFI_STATUS Status;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);

  PlatformInfo = (PLATFORM_INFO *) Content;

  PcdSet8S (PcdSaMiscUserBd, 5);

  if (BoardIdOrgValue == BoardIdKabylakeRDdr4) {
    PcdSet8S (PcdMrcCaVrefConfig, 2); // DDR4 boards
  }

  if (BoardIdOrgValue == BoardIdKabylakeRLpddr3){
      PcdSet8S(PcdMrcSpdAddressTable0, DIMM_SMB_SPD_P0C0D0_STP);
    PcdSet8S(PcdMrcSpdAddressTable1, DIMM_SMB_SPD_P0C0D1_STP);
    PcdSet8S(PcdMrcSpdAddressTable2, DIMM_SMB_SPD_P0C0D0_STP);
    PcdSet8S(PcdMrcSpdAddressTable3, DIMM_SMB_SPD_P0C0D1_STP);

    PcdSet32S(PcdMrcDqByteMap, (UINTN)DqByteMapSklRvp5);
    PcdSet16S(PcdMrcDqByteMapSize, DqByteMapSklRvp5Size);
    PcdSet32S(PcdMrcDqsMapCpu2Dram, (UINTN)DqsMapCpu2DramSklRvp5);
    PcdSet16S(PcdMrcDqsMapCpu2DramSize, sizeof(DqsMapCpu2DramSklRvp5));

    PcdSet32S(PcdMrcRcompResistor, (UINTN)RcompResistorSklRvp1);
    PcdSet32S(PcdMrcRcompTarget, (UINTN)RcompTargetSklRvp1);

#ifdef MEM_DOWN_FLAG
    if (PlatformInfo->BoardBomId == 0x0) { // Micron MT52L512M32D2PF, 8Gb, DDP, 2133
      PcdSet32S(PcdMrcSpdData, (UINTN)mKblRSpdLpddr32133);
      PcdSet16S(PcdMrcSpdDataSize, mKblRSpdLpddr32133Size);
    }
    if (!PlatformInfo->SpdPresent) {
      PcdSet8S(PcdMrcSpdAddressTable0, 0x0);
      PcdSet8S(PcdMrcSpdAddressTable1, 0x0);
      PcdSet8S(PcdMrcSpdAddressTable2, 0x0);
      PcdSet8S(PcdMrcSpdAddressTable3, 0x0);
    }
#endif
    PcdSet8S(PcdSaMiscUserBd, 5); // MrcBoardType btUser4 for ULT platform
    PcdSet8S(PcdMrcCaVrefConfig, 0); // All DDR3L/LPDDR3 boards

  } else if (BoardIdOrgValue == BoardIdKabylakeKcDdr3) { // KBL-KC RVP MRC parameters
      PcdSet8S (PcdMrcSpdAddressTable0, DIMM_SMB_SPD_P0C0D0_STP);
      PcdSet8S (PcdMrcSpdAddressTable1, DIMM_SMB_SPD_P0C0D1_STP);
      PcdSet8S (PcdMrcSpdAddressTable2, DIMM_SMB_SPD_P0C0D0_STP);
      PcdSet8S (PcdMrcSpdAddressTable3, DIMM_SMB_SPD_P0C0D1_STP);

      PcdSet32S (PcdMrcDqByteMap, (UINTN) DqByteMapSklRvp5);
      PcdSet16S (PcdMrcDqByteMapSize, DqByteMapSklRvp5Size);
      PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklRvp5);
      PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklRvp5));

      PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp1);
      PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp1);

#ifdef MEM_DOWN_FLAG
      if (PlatformInfo->BoardBomId == 0x0) { // Micron MT52L512M32D2PF, 4Gb, DDP, 2133
        PcdSet32S(PcdMrcSpdData, (UINTN)mSpdLpddr32133);
        PcdSet16S(PcdMrcSpdDataSize,mSpdLpddr32133Size);
      }
      if (!PlatformInfo->SpdPresent) {
        PcdSet8S (PcdMrcSpdAddressTable0, 0x0);
        PcdSet8S (PcdMrcSpdAddressTable1, 0x0);
        PcdSet8S (PcdMrcSpdAddressTable2, 0x0);
        PcdSet8S (PcdMrcSpdAddressTable3, 0x0);
      }
#endif


      PcdSet8S (PcdSaMiscUserBd, 5); // MrcBoardType btUser4 for ULT platform

      PcdSet8S (PcdMrcCaVrefConfig, 0); // All DDR3L/LPDDR3 boards
  } else {
    PcdSetBoolS (PcdMrcDqPinsInterleavedControl, TRUE);
    PcdSetBoolS (PcdMrcDqPinsInterleaved, TRUE);
    PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklRvp2);
    PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklRvp2);
  }

  if ((BoardIdOrgValue == BoardIdKabylakeRDdr4) || (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
    PcdSet8S(PcdRootPortDev, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1);
    PcdSet8S(PcdRootPortFunc, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1);
  } else {
    PcdSet8S(PcdRootPortDev, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2);
    PcdSet8S(PcdRootPortFunc, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9);
  }

  PcdSet8S  (PcdPcie0GpioSupport, I2CGpio);
  PcdSet32S (PcdPcie0WakeGpioNo, 0);
  PcdSet8S  (PcdPcie0HoldRstExpanderNo, 0);
  PcdSet32S (PcdPcie0HoldRstGpioNo, 8);
  PcdSetBoolS (PcdPcie0HoldRstActive, TRUE);
  PcdSet8S  (PcdPcie0PwrEnableExpanderNo, 0);
  PcdSet32S (PcdPcie0PwrEnableGpioNo, 16);
  PcdSetBoolS (PcdPcie0PwrEnableActive, FALSE);
  PcdSet8S  (PcdPcie0ClkReqNumber, 8);

  //
  // Root Port Clk Info
  //
  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) || (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 0;
    PcdSet64S (PcdRootPort0ClkInfo, RootPort.Blob); //TBT

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 2;
    PcdSet64S (PcdRootPort5ClkInfo, RootPort.Blob); //Wifi

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 3;
    PcdSet64S (PcdRootPort6ClkInfo, RootPort.Blob); //x1 slot

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 4;
    PcdSet64S (PcdRootPort11ClkInfo, RootPort.Blob); //WWAN

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 5;
    PcdSet64S (PcdRootPort8ClkInfo, RootPort.Blob); //SSD

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber    = 1;
    PcdSet64S (PcdRootPortLanClkInfo, RootPort.Blob); //LAN

  } else {
    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 5;
    PcdSet64S (PcdRootPort2ClkInfo, RootPort.Blob);

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 2;
    PcdSet64S (PcdRootPort3ClkInfo, RootPort.Blob);

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 3;
    PcdSet64S (PcdRootPort4ClkInfo, RootPort.Blob);

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 4;
    PcdSet64S (PcdRootPort5ClkInfo, RootPort.Blob);

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 1;
    PcdSet64S (PcdRootPort8ClkInfo, RootPort.Blob);

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber    = 2;
    PcdSet64S (PcdRootPortLanClkInfo, RootPort.Blob);
  }
  if (BoardIdOrgValue == BoardIdKabylakeKcDdr3) { //KBL-KC PCIe root port CLK for TBT.
    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 1;
    PcdSet64S (PcdRootPort4ClkInfo, RootPort.Blob); //TR1

    RootPort.Blob = 0;
    RootPort.Info.ClkReqSupported = TRUE;
    RootPort.Info.ClkReqNumber = 2;
    PcdSet64S (PcdRootPort8ClkInfo, RootPort.Blob); //TR2
  }
  PcdSetBoolS (PcdIoExpanderPresent, TRUE);
  return EFI_SUCCESS;
}

/**
  SkylaeURvp7 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeURvp7InitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;
  PCD64_BLOB PcdData;
  UINT16 BoardIdOrgValue;
  UINT16 BoardId;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);

  PlatformInfo = (PLATFORM_INFO *) Content;

  Status =  PeiSkylakeURvp7CommonInitPreMem (Content);
  PcdSet32S (PcdSpecificLpHsioPtssTable1,     (UINTN) PchLpHsioPtss_Bx_SkylakeURvp7);
  PcdSet16S (PcdSpecificLpHsioPtssTable1Size, (UINTN) PchLpHsioPtss_Bx_SkylakeURvp7_Size);
  PcdSet32S (PcdSpecificLpHsioPtssTable2,     (UINTN) PchLpHsioPtss_Cx_SkylakeURvp7);
  PcdSet16S (PcdSpecificLpHsioPtssTable2Size, (UINTN) PchLpHsioPtss_Cx_SkylakeURvp7_Size);

  if ((BoardIdOrgValue == BoardIdKabylakeRDdr4) || (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
    PcdSet8S(PcdRootPortDev, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1);
    PcdSet8S(PcdRootPortFunc, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1);
  } else {
    PcdSet8S(PcdRootPortDev, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2);
    PcdSet8S(PcdRootPortFunc, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9);
  }

  PcdSet8S  (PcdPcie0GpioSupport, I2CGpio);
  PcdSet32S (PcdPcie0WakeGpioNo, 0);
  PcdSet8S  (PcdPcie0HoldRstExpanderNo, 0);
  PcdSet32S (PcdPcie0HoldRstGpioNo, 8);
  PcdSetBoolS (PcdPcie0HoldRstActive, TRUE);
  PcdSet8S  (PcdPcie0PwrEnableExpanderNo, 0);
  PcdSet32S (PcdPcie0PwrEnableGpioNo, 16);
  PcdSetBoolS (PcdPcie0PwrEnableActive, FALSE);
  PcdSet8S  (PcdPcie0ClkReqNumber, 8);

  // OC Map for USB2 Ports
  PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPin0);
  PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin2);
  PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPin1);
  PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin2);
  PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPin2);
  PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPinSkip);

  // OC Map for USB3 Ports
  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
    PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
  } else {
    PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin0);
    PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
    PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPin1);
    PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin2);
    PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPinSkip);
  }

  if (BoardIdOrgValue == BoardIdKabylakeKcDdr3) {
    PcdSet32S (PcdGpioGroupToGpeDw0, GPIO_SKL_LP_GROUP_GPP_C);
    PcdSet32S (PcdGpioGroupToGpeDw1, GPIO_SKL_LP_GROUP_GPP_G);
    PcdSet32S (PcdGpioGroupToGpeDw2, GPIO_SKL_LP_GROUP_GPP_E);
  }
  //
  // RecoveryMode GPIO
  //
  PcdData.Blob = 0;
  PcdData.BoardGpioConfig.Type = BoardGpioTypeExpander;
  PcdData.BoardGpioConfig.u.Expander.Section = IO_EXPANDER_0;
  PcdData.BoardGpioConfig.u.Expander.Pin = IO_EXPANDER_GPIO_11;
  PcdSet64S (PcdRecoveryModeGpio, PcdData.Blob);

  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
  PcdSetBoolS (PcdIvCamInitPresent, FALSE);
  }
  else{
  PcdSetBoolS (PcdIvCamInitPresent, TRUE);
  }

  return EFI_SUCCESS;
}

/**
  SkylaeURvp7 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeURvp7PpvInitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS Status;

  Status = PeiSkylakeURvp7InitPreMem (Content);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PcdSet32S (PcdSpecificLpHsioPtssTable1,     (UINTN) PchLpHsioPtss_Bx_SkylakeURvp7Ppv);
  PcdSet16S (PcdSpecificLpHsioPtssTable1Size, (UINTN) PchLpHsioPtss_Bx_SkylakeURvp7Ppv_Size);
  PcdSet32S (PcdSpecificLpHsioPtssTable2,     (UINTN) PchLpHsioPtss_Cx_SkylakeURvp7Ppv);
  PcdSet16S (PcdSpecificLpHsioPtssTable2Size, (UINTN) PchLpHsioPtss_Cx_SkylakeURvp7Ppv_Size);

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
PeiSkylakeURvp7BoardInitPreMemLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp7BoardInitPreMem, TABLE_LIST_NUM);
}

