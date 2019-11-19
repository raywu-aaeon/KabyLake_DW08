/** @file
  Source code for the Skylake-U RVP7 configuration init function in DXE init phase.

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

#include "SkylakeURvp7InitLib.h"

#include <Library/PchSerialIoLib.h>
#include <PlatformBoardConfig.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>

//
// Others Board Init Table
//
BOARD_INIT_TABLE mSkylakeURvp7Init [] = {
  {BoardIdSkylakeURvp7, SkylakeURvp7Init},
  { BoardIdKabylakeRDdr4, SkylakeURvp7Init },
  { BoardIdKabylakeRLpddr3, SkylakeURvp7Init },
  { BoardIdKabylakeKcDdr3, SkylakeURvp7Init },
  {BoardIdSkylakeURvp7Ppv, SkylakeURvp7PpvInit},
  { 0, 0 }
};

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
SkylakeURvp7AcpiConfigBlockInit (
  IN PLATFORM_INFO *PlatformInfo
)
{
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;
  EFI_STATUS Status;
  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);
  //
  // Update OEM table ID
  //

  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) { //Customized change needed for KBL R RVP in contrast to SKL RVP7
    PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'k', 'b', 'l', '-', 'r'));
  } else {
    PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'r', 'v', 'p', '0', '7'));
  }

  //
  // Modify Preferred_PM_Profile field based on Board SKU's. Default is set to Mobile
  //
  PcdSet8S (PcdPreferredPmProfile, EFI_ACPI_2_0_PM_PROFILE_MOBILE);

  //
  // TherSds or TherRvp table enable.
  //
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

  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {//Customized change needed for KBL R RVP in contrast to SKL RVP7
    PcdSet32S (PcdBluetoothIrqGpio  , GPIO_SKL_LP_GPP_E3);
  } else if(BoardIdOrgValue != BoardIdKabylakeKcDdr3){ //KBL-kc does not have Bluetooth IRQ.
    PcdSet32S (PcdBluetoothIrqGpio  , GPIO_SKL_LP_GPP_B5);
  }

  PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_LP_GPP_B3);
  PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_LP_GPP_E7);
  PcdSetBoolS (PcdEInkDfuDevice, TRUE);

  //
  // VSCT table, load SEN targets for KBL R
  //
  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
    PcdSetBoolS (PcdVsctTableForSens, TRUE);    // load SEN taget in VSCT table
  } else {
    PcdSetBoolS (PcdVsctTableForSens, FALSE);   // load GEN taget in VSCT table
  }

  //
  // Board Specific Init
  //
  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) { //Customized change needed for KBL R RVP in contrast to SKL RVP7
    PcdSetBoolS (PcdRvpRRtd3TableEnable, TRUE);
  } else if(BoardIdOrgValue == BoardIdKabylakeKcDdr3){ //KBL-kc does not have Bluetooth IRQ.
    PcdSetBoolS (PcdRvpKcRtd3TableEnable, TRUE);
  } else {
    PcdSetBoolS (PcdRvp7Rtd3TableEnable, TRUE);
  }
  return EFI_SUCCESS;
}

/**
  Init Common Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
SkylakeURvp7CommonConfigBlockInit (
  IN PLATFORM_INFO *PlatformInfo
)
{
  PCD64_BLOB Data64;
  UINTN Size;
  EFI_GUID ZeroGuid;
  Size = sizeof (EFI_GUID);

  //
  // Enable EC SMI# for SMI
  //
  PcdSet32S (PcdEcSmiGpio, GPIO_SKL_LP_GPP_E15);
  PcdSet32S (PcdEcLowPowerExitGpio, GPIO_SKL_LP_GPP_A7);

  //
  // ESRT Firmware ID.
  //
  ZeroMem (&ZeroGuid, sizeof (EFI_GUID));
  PcdSetPtrS(PcdEsrtFirmwareId, &Size, &ZeroGuid);

  //
  // HID I2C Interrupt GPIO.
  //
  PcdSet32S (PcdHidI2cIntPad, GPIO_SKL_LP_GPP_E7);

  PcdSetBoolS (PcdDetectPs2KbOnCmdAck, FALSE);

  PcdSetBoolS (PcdSpdAddressOverride, FALSE);

  //
  // DDISelection
  //
  PcdSet8S (PcdDDISelection, TRUE);

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
  PcdSetBoolS(PcdDsEndpointCamera, FALSE);

  //
  // Percep Ds 4 Camera
  //
  PcdSetBoolS(PcdPercepDs4Camera, FALSE);

  //
  // Disable IVCAM before going into Sx state
  //
  PcdSetBoolS(PcdIvCamera, 0);

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
  PcdSetBoolS(PcdH8S2113SIO, FALSE);

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
  PcdSet8S (PcdZPODD, 1);

  //
  // SMC Runtime Sci Pin
  //
  PcdSet32S(PcdSmcRuntimeSciPin, (UINT32) GPIO_SKL_LP_GPP_E16);

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
  PcdSetBoolS(PcdAcpiHidDriverButtonSupport, TRUE);


  return EFI_SUCCESS;
}


/**
  SkylaeURvp7 board configuration init function for DXE memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI
SkylakeURvp7Init (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformInfo = (PLATFORM_INFO *) Content;

  Status = SkylakeURvp7AcpiConfigBlockInit (PlatformInfo);
  ASSERT_EFI_ERROR(Status);

  Status = SkylakeURvp7CommonConfigBlockInit (PlatformInfo);
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SkylakeURvp7PpvInit (
  IN VOID *Content
  )
{
  return SkylakeURvp7Init (Content);
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
DxeSkylakeURvp7InitLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return CommonInitInternalLibConstructor (mSkylakeURvp7Init, TABLE_LIST_NUM);
}

