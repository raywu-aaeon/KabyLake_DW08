/** @file
  Source code for the Skylake-A0 RVP3 configuration init function in DXE init phase.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2019 Intel Corporation.

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

#include "SkylakeA0Rvp3InitLib.h"


#include <Library/PchSerialIoLib.h>
#include <PlatformBoardConfig.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <Board/BoardConfigInit.h>

//
// Others Board Init Table
//
BOARD_INIT_TABLE mSkylakeA0Rvp3Init [] = {
  {BoardIdSkylakeA0Rvp3, SkylakeA0Rvp3Init},
  {BoardIdSkylakeA0Rvp3Ppv, SkylakeA0Rvp3PpvInit},
  { 0, 0 }
};

extern EFI_GUID gSysFwRvp3Guid;

//SKL PCH LP GPIO Expander Number
#define IO_EXPANDER_0            0
#define IO_EXPANDER_1            1

//SKL PCH LP GPIO Pin Mapping
#define IO_EXPANDER_GPIO_5        5   // P05
#define IO_EXPANDER_GPIO_11       11  // P13

/**
  Init Platform Board Config Block for ACPI platform.

  @param[in]  PlatformInfo      a pointer to Platform Info.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
SkylakeA0Rvp3AcpiConfigBlockInit (
  IN PLATFORM_INFO *PlatformInfo
)
{
  BOARD_PRIVATE_DATA      *BoardInfo;

  BoardInfo = (BOARD_PRIVATE_DATA *) PcdGetPtr (PcdBoardPrivateData);
  //
  // Update OEM table ID
  //
  //  case BoardIdSkylakeA0Rvp3:
  //  case BoardIdSkylakeA0Rvp3Ppv:
  if (BoardInfo->BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3) {
    //
    // KBL Y Rvp3 with Type-C
    //
    DEBUG ((DEBUG_INFO, "Acpi for KBL Rvp3 Usb Ports\n"));
    PcdSet64S(PcdXhciAcpiTableSignature, SIGNATURE_64('x', 'h', '_', 'r', 'v', 'p', '0', '3'));
  }  else if (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) {
    //
    // AML 7W Rvp3 with Type-C
    //
    DEBUG((DEBUG_INFO, "Acpi for AML Rvp3 Usb Ports\n"));
    PcdSet64S(PcdXhciAcpiTableSignature, SIGNATURE_64('x', 'h', '_', 'r', 'v', 'p', '3', 'a'));
  } else if (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    //
    // AML Y42 Rvp3 with Type-C
    //
    DEBUG ((DEBUG_INFO, "Acpi for AML Y42 Usb Ports\n"));
    PcdSet64S(PcdXhciAcpiTableSignature, SIGNATURE_64('x', 'h', '_', 'm', 'l', 'y', '4', '2'));
  } else if ((BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
             (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)){
    //
    // AML Y42 AEP with Type-C
    //
    DEBUG((DEBUG_INFO, "Acpi for AML AEP Usb Ports\n"));
    PcdSet64S(PcdXhciAcpiTableSignature, SIGNATURE_64('x', 'h', '_', 'm', 'l', 'a', 'e', 'p'));
  } else {
    //
    // Rvp3 without Type-C
    //
    DEBUG ((DEBUG_INFO, "Acpi for SKL Rvp3 Usb Ports\n"));
    PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'r', 'v', 'p', '3', 's'));
  }

  //
  // Modify Preferred_PM_Profile field based on Board SKU's. Default is set to Mobile
  //
  PcdSet8S (PcdPreferredPmProfile, EFI_ACPI_2_0_PM_PROFILE_MOBILE);
  PcdSetBoolS (PcdTherSdsTableEnable, FALSE);
  PcdSet8S (PcdSpecificIoExpanderBus, PchSerialIoIndexI2C4);

  //
  // Assign FingerPrint, Gnss, BLuetooth & TouchPanel relative GPIO.
  //
  PcdSet32S (PcdFingerPrintSleepGpio, GPIO_SKL_LP_GPP_A22);
  PcdSet32S (PcdFingerPrintIrqGpio  , GPIO_SKL_LP_GPP_A23);
  PcdSet32S (PcdGnssResetGpio       , GPIO_SKL_LP_GPP_B23);
  PcdSet32S (PcdBluetoothWakeGpio   , GPIO_SKL_LP_GPP_C2);//not sure if connected?
  PcdSet32S (PcdBluetoothRfKillGpio , GPIO_SKL_LP_GPP_B4);
  PcdSet32S (PcdBluetoothIrqGpio    , GPIO_SKL_LP_GPP_B5);
  PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_LP_GPP_B3);
  PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_LP_GPP_E7);

  PcdSetBoolS (PcdEInkDfuDevice, TRUE);
  if (BoardInfo->BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3 ||
      BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3 ||
      BoardInfo->BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) {
    PcdSet32S (PcdWwanGnssUartReset, GPIO_SKL_LP_GPP_D9);
  } else if ((BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
             (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)){
    PcdSet32S (PcdWwanGnssUartReset,    GPIO_SKL_LP_GPP_A16);
    PcdSet32S (PcdFingerPrintSleepGpio, 0);
    PcdSet32S (PcdFingerPrintIrqGpio  , GPIO_SKL_LP_GPP_D7);
    PcdSet32S (PcdGnssResetGpio       , 0);
    PcdSet32S (PcdBluetoothWakeGpio   , 0);
    PcdSet32S (PcdBluetoothRfKillGpio , GPIO_SKL_LP_GPP_B4);
    PcdSet32S (PcdBluetoothIrqGpio    , 0);
    PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_LP_GPP_D12);
    PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_LP_GPP_D10);

    PcdSetBoolS (PcdEInkDfuDevice, FALSE);
  } else {
    PcdSet32S (PcdWwanGnssUartReset, GPIO_SKL_LP_GPP_A12);
  }
#ifndef MINTREE_FLAG
  //
  // Board Specific Init
  //
  if (BoardInfo->BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3 ||
      BoardInfo->BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) {
    PcdSetBoolS (PcdAmlRtd3TableEnable, TRUE);
  } else if (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    PcdSetBoolS (PcdAmlY42Rtd3TableEnable, TRUE);
  } else if ((BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
             (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)){
    PcdSetBoolS (PcdAmlY42CffRtd3TableEnable, TRUE);
  } else {
    PcdSetBoolS (PcdRvp3Rtd3TableEnable, TRUE);
  }

//
// AdvancedFeaturesBegin
//

  //
  // Board Specific Power Meter Support Init
  //
  if (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    PcdSetBoolS (PcdPAC193xPowerMeterSupport, TRUE);
    PcdSet8S (PcdPowerMeterI2cControllerNumber, PchSerialIoIndexI2C1);
  } else if ((BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
             (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdSetBoolS (PcdPAC193xPowerMeterSupport, TRUE);
    PcdSet8S (PcdPowerMeterI2cControllerNumber, PchSerialIoIndexI2C1);
  }

//
// AdvancedFeaturesEnd
//

#endif
  return EFI_SUCCESS;
}

/**
  Init Common Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
SkylakeA0Rvp3CommonConfigBlockInit (
  IN PLATFORM_INFO *PlatformInfo
)
{
  BOARD_PRIVATE_DATA              *BoardInfo;
  UINTN                           Size;
  PCD64_BLOB                      Data64;

  BoardInfo = (BOARD_PRIVATE_DATA *) PcdGetPtr (PcdBoardPrivateData);
  Size = sizeof (EFI_GUID);

  //
  // Enable EC SMI# for SMI
  //
  PcdSet32S (PcdEcSmiGpio, GPIO_SKL_LP_GPP_E15);  // SPT-LP, Group E, Pad 15
  PcdSet32S (PcdEcLowPowerExitGpio, GPIO_SKL_LP_GPP_A7);

  if ((BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
      (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdSet32S (PcdEcSmiGpio, 0);
    PcdSet32S (PcdEcLowPowerExitGpio, GPIO_SKL_LP_GPP_E3);
  }

  //
  // ESRT Firmware ID.
  //
  PcdSetPtrS(PcdEsrtFirmwareId, &Size, &gSysFwRvp3Guid);

  //
  // HID I2C Interrupt GPIO.
  //
  PcdSet32S (PcdHidI2cIntPad, GPIO_SKL_LP_GPP_E7);

  if ((BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
      (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdSet32S (PcdHidI2cIntPad, GPIO_SKL_LP_GPP_D10);
  }
  PcdSetBoolS (PcdDetectPs2KbOnCmdAck, FALSE);
  PcdSetBoolS (PcdSpdAddressOverride, TRUE);

  //
  // DDISelection
  //
  PcdSet8S (PcdDDISelection, TRUE);

  //
  // Grizzly Mtn Specific Init
  //
  PcdSetBoolS (PcdMipiDisplayPresent, FALSE);


  //
  // Sensor Hub
  //
  PcdSetBoolS (PcdSensorHubSupport, TRUE);

  //
  // GFX Detect
  //
  //RVP 3,5,7,13,15
  //IO_EXPANDER_GPIO_5 in IO_EXPANDER_0
  Data64.BoardGpioConfig.Type = BoardGpioTypeExpander;
  Data64.BoardGpioConfig.u.Expander.Section = IO_EXPANDER_0;
  Data64.BoardGpioConfig.u.Expander.Pin = IO_EXPANDER_GPIO_5;

  if ((BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
      (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    Data64.Blob = 0;
    Data64.BoardGpioConfig.Type = BoardGpioTypeNotSupported;
  }
  PcdSet64S (PcdGfxCrbDetectGpio, Data64.Blob);

  //
  //Battery Present
  //
  PcdSet8S (PcdBatteryPresent, BOARD_REAL_BATTERY_SUPPORTED | BOARD_VIRTUAL_BATTERY_SUPPORTED);

  //
  // TS-on-DIMM temperature
  //
  PcdSetBoolS (PcdTsOnDimmTemperature, FALSE);

  //
  // Ds End Point Camera
  //
  PcdSetBoolS(PcdDsEndpointCamera, TRUE);

  //
  // Percep Ds 4 Camera
  //
  PcdSetBoolS(PcdPercepDs4Camera, TRUE);

  //
  // Disable IVCAM before going into Sx state
  //
  PcdSetBoolS(PcdIvCamera, FALSE);

  //
  // Real Battery 1 Control & Real Battery 2 Control
  //
  PcdSet8S (PcdRealBattery1Control, 1);
  PcdSet8S (PcdRealBattery2Control, 2);

  //
  // Sky Camera Sensor
  //
  PcdSetBoolS(PcdSkyCamSensor, FALSE);

  //
  // HD Audio I2S
  //
  PcdSetBoolS(PcdHDAudioI2S, FALSE);

  //
  // H8S2113 SIO
  //
  if (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    PcdSetBoolS(PcdH8S2113SIO, TRUE);
  } else {
    PcdSetBoolS(PcdH8S2113SIO, FALSE);
  }

  //
  // NCT6776F COM, SIO & HWMON
  //
  PcdSetBoolS(PcdNCT6776FCOM, FALSE);
  PcdSetBoolS(PcdNCT6776FSIO, FALSE);
  PcdSetBoolS(PcdNCT6776FHWMON, FALSE);

  //
  // Power Participant
  //
  PcdSetBoolS(PcdPowerParticipant, FALSE);

  //
  // Sensor Hub Application
  //
  PcdSetBoolS(PcdSensorHubApp, FALSE);

  //
  // EInk Application
  //
  PcdSetBoolS(PcdEInkApp, FALSE);

  //
  // ZPODD
  //
  PcdSet8S (PcdZPODD, 0);

  //
  // SMC Runtime Sci Pin
  //
  PcdSet32S(PcdSmcRuntimeSciPin, (UINT32) GPIO_SKL_LP_GPP_E16);

  if ((BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
      (BoardInfo->BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
  PcdSet32S(PcdSmcRuntimeSciPin,0);
  }
  //
  // Convertable Dock Support
  //
  PcdSetBoolS(PcdConvertableDockSupport, FALSE);

  //
  // Ec Hotkey F3, F4, F5, F6, F7 and F8 Support
  //
  PcdSet8S (PcdEcHotKeyF3Support, 1);
  PcdSet8S (PcdEcHotKeyF4Support, 1);
  PcdSet8S (PcdEcHotKeyF5Support, 1);
  PcdSet8S (PcdEcHotKeyF6Support, 1);
  PcdSet8S (PcdEcHotKeyF7Support, 1);
  PcdSet8S (PcdEcHotKeyF8Support, 1);

  //
  // Virtual Button Volume Up & Done Support
  // Virtual Button Home Button Support
  // Virtual Button Rotation Lock Support
  //
  PcdSetBoolS(PcdVirtualButtonVolumeUpSupport, TRUE);
  PcdSetBoolS(PcdVirtualButtonVolumeDownSupport, TRUE);
  PcdSetBoolS(PcdVirtualButtonHomeButtonSupport, TRUE);
  PcdSetBoolS(PcdVirtualButtonRotationLockSupport, TRUE);

  //
  // Slate Mode Switch Support
  //
  PcdSetBoolS(PcdSlateModeSwitchSupport, FALSE);

  //
  // Ac Dc Auto Switch Support
  //
  PcdSetBoolS(PcdAcDcAutoSwitchSupport, TRUE);

  //
  // Pm Power Button Gpio Pin
  //
  PcdSet32S(PcdPmPowerButtonGpioPin, (UINT32) GPIO_SKL_LP_GPD3);

  //
  // Acpi Enable All Button Support
  //
  PcdSetBoolS(PcdAcpiEnableAllButtonSupport, TRUE);

  //
  // Acpi Hid Driver Button Support
  //
  PcdSetBoolS (PcdAcpiHidDriverButtonSupport, TRUE);

  return EFI_SUCCESS;
}


/**
  SkylaeA0Rvp3 board configuration init function for DXE memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI
SkylakeA0Rvp3Init (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformInfo = (PLATFORM_INFO *) Content;


  Status = SkylakeA0Rvp3AcpiConfigBlockInit (PlatformInfo);
  ASSERT_EFI_ERROR(Status);

  Status = SkylakeA0Rvp3CommonConfigBlockInit (PlatformInfo);
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SkylakeA0Rvp3PpvInit (
  IN VOID *Content
  )
{
  return SkylakeA0Rvp3Init (Content);
}

/**
  The library constructuor.

  The function does the necessary initialization work for this library
  instance.

  @param[in]  ImageHandle       The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable       A pointer to the EFI system table.

  @retval     EFI_SUCCESS       The function always return EFI_SUCCESS for now.
                                It will ASSERT on error for debug version.
  @retval     EFI_ERROR         Please reference LocateProtocol for error code details.
**/
EFI_STATUS
EFIAPI
DxeSkylakeA0Rvp3InitLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return CommonInitInternalLibConstructor (mSkylakeA0Rvp3Init, TABLE_LIST_NUM);
}

