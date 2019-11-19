/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016-2017 Intel Corporation.

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

#include "PeiSkylakeHaloDdr4Rvp11InitLib.h"

#include "SkylakeHaloDdr4Rvp11HsioPtssTables.h"

#include "GpioTableHaloRvp11.h"

#include <ConfigBlock.h>
#include <ConfigBlock/SwitchableGraphicsConfig.h>   // for PCIE RTD3 GPIO
#include <PlatformBoardConfig.h>                    // for USB 20 AFE & Root Port Clk Info
#include <Library/PeiPlatformHookLib.h>             // For Recovery Mode GPIO
#include <SaAccess.h>                               // For SA_PEG10_DEV_NUM definition

extern UINT16 RcompResistorSklDtHalo[];
extern UINT16 RcompTargetSklDtHalo[];

//
// RVP11 Init Table
//
BOARD_INIT_TABLE mRvp11BoardInitPreMem [] = {
  {BoardIdSkylakeHaloDdr4Rvp11, PeiSkylakeHaloDdr4Rvp11InitPreMem},
  {BoardIdSkylakeHaloDdr4Rvp11Ppv, PeiSkylakeHaloDdr4Rvp11PpvInitPreMem},
  { 0, 0 }
};

/**
  SkylaeHaloDdr4Rvp11 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
PeiSkylakeHaloDdr4Rvp11CommonInitPreMem (
  IN VOID *Content
  )
{
  PLATFORM_INFO *PlatformInfo;
  PCD64_BLOB RootPort;

  EFI_STATUS Status;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;

  Status = GetBoardId(&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR(Status);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformInfo = (PLATFORM_INFO *) Content;

  PcdSetBoolS (PcdPegGpioResetControl, FALSE);
  if (BoardIdOrgValue == BoardIdHaloDdr4uSFF) {
    PcdSet32S (PcdPegResetGpioPad, GPIO_SKL_H_GPP_B20);
  } else {
    PcdSet32S (PcdPegResetGpioPad, GPIO_SKL_H_GPP_F22);
  }
  PcdSetBoolS (PcdPegResetGpioActive, FALSE);


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

  //
  // Root Port Clk Info
  //
  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE; // PCIe port 6
  RootPort.Info.ClkReqNumber    = 1;
  PcdSet64S (PcdRootPort5ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE; // PCIe port 7
  RootPort.Info.ClkReqNumber    = 2;
  PcdSet64S (PcdRootPort6ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE; // PCIe port 8
  RootPort.Info.ClkReqNumber    = 3;
  PcdSet64S (PcdRootPort7ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE; // PCIe port 9-12 (x4)
  RootPort.Info.ClkReqNumber    = 4;
  RootPort.Info.DeviceResetPad  = GPIO_SKL_H_GPP_F4;
  RootPort.Info.DeviceResetPadActiveHigh = 1; // Active high
  PcdSet64S (PcdRootPort8ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE; // PCIe port 13-16(x4)
  RootPort.Info.ClkReqNumber    = 5;
  PcdSet64S (PcdRootPort12ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE; // PCIe port 17
  RootPort.Info.ClkReqNumber    = 7;
  PcdSet64S (PcdRootPort16ClkInfo, RootPort.Blob);

  RootPort.Blob = 0;
  RootPort.Info.ClkReqSupported = TRUE;
  RootPort.Info.ClkReqNumber    = 9;
  PcdSet64S (PcdRootPortLanClkInfo, RootPort.Blob);

  // OC Map for USB2 Ports
  PcdSet8S (PcdUsb20OverCurrentPinPort0, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort1, PchUsbOverCurrentPin3);
  PcdSet8S (PcdUsb20OverCurrentPinPort2, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort4, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort5, PchUsbOverCurrentPin0);
  PcdSet8S (PcdUsb20OverCurrentPinPort6, PchUsbOverCurrentPin0);
  PcdSet8S (PcdUsb20OverCurrentPinPort7, PchUsbOverCurrentPin2);
  PcdSet8S (PcdUsb20OverCurrentPinPort8, PchUsbOverCurrentPin2);
  PcdSet8S (PcdUsb20OverCurrentPinPort9, PchUsbOverCurrentPin1);
  PcdSet8S (PcdUsb20OverCurrentPinPort10, PchUsbOverCurrentPin1);
  PcdSet8S (PcdUsb20OverCurrentPinPort11, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb20OverCurrentPinPort12, PchUsbOverCurrentPin4);
  PcdSet8S (PcdUsb20OverCurrentPinPort13, PchUsbOverCurrentPin4);

  // OC Map for USB3 Ports
  PcdSet8S (PcdUsb30OverCurrentPinPort0, PchUsbOverCurrentPin5);
  PcdSet8S (PcdUsb30OverCurrentPinPort1, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb30OverCurrentPinPort2, PchUsbOverCurrentPin3);
  PcdSet8S (PcdUsb30OverCurrentPinPort3, PchUsbOverCurrentPinSkip);
  PcdSet8S (PcdUsb30OverCurrentPinPort4, PchUsbOverCurrentPin1);
  PcdSet8S (PcdUsb30OverCurrentPinPort5, PchUsbOverCurrentPin0);
  PcdSet8S (PcdUsb30OverCurrentPinPort6, PchUsbOverCurrentPin0);
  PcdSet8S (PcdUsb30OverCurrentPinPort7, PchUsbOverCurrentPin2);
  PcdSet8S (PcdUsb30OverCurrentPinPort8, PchUsbOverCurrentPin2);
  PcdSet8S (PcdUsb30OverCurrentPinPort9, PchUsbOverCurrentPin1);

  if (BoardIdOrgValue == BoardIdHaloDdr4uSFF) {
    PcdSet32S (PcdGpioGroupToGpeDw0, GPIO_SKL_H_GROUP_GPP_C);
  } else {
    PcdSet32S (PcdGpioGroupToGpeDw0, GPIO_SKL_H_GROUP_GPP_G);
  }
  PcdSet32S (PcdGpioGroupToGpeDw1, GPIO_SKL_H_GROUP_GPP_D);
  PcdSet32S (PcdGpioGroupToGpeDw2, GPIO_SKL_H_GROUP_GPP_E);

  PcdSetBoolS (PcdPc8374SioKbcPresent, FALSE);

  return EFI_SUCCESS;
}

/**
  SkylaeHaloDdr4Rvp11 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeHaloDdr4Rvp11InitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;
  //PCD64_BLOB RootPort;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = PeiSkylakeHaloDdr4Rvp11CommonInitPreMem  (Content);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PlatformInfo = (PLATFORM_INFO *) Content;

  PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeHaloDdr4Rvp11);
  PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeHaloDdr4Rvp11_Size);
  PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeHaloDdr4Rvp11);
  PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeHaloDdr4Rvp11_Size);

  PcdSet8S (PcdSaMiscUserBd, 1);

  PcdSetBoolS (PcdMrcDqPinsInterleavedControl, TRUE);
  PcdSetBoolS (PcdMrcDqPinsInterleaved, TRUE);
  PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklDtHalo);
  PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklDtHalo);

  PcdSet8S (PcdMrcCaVrefConfig, 2); // DDR4 boards

  return EFI_SUCCESS;
}

/**
  SkylaeHaloDdr4Rvp11 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeHaloDdr4Rvp11PpvInitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS Status;

  Status = PeiSkylakeHaloDdr4Rvp11InitPreMem (Content);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeHaloDdr4Rvp11Ppv);
  PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeHaloDdr4Rvp11Ppv_Size);
  PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeHaloDdr4Rvp11Ppv);
  PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeHaloDdr4Rvp11Ppv_Size);

  PcdSet32S (PcdBoardGpioTablePreMem, (UINTN) mGpioTableHaloRvp11PreMem);
  PcdSet16S (PcdBoardGpioTablePreMemSize, mGpioTableHaloRvp11PreMemSize);

  return EFI_SUCCESS;
}

/**
  The constructor function for SkylakeHaloDdr4Rvp11 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiSkylakeHaloDdr4Rvp11BoardInitPreMemLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp11BoardInitPreMem, TABLE_LIST_NUM);
}

