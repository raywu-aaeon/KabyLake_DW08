/** @file
  Source code for the Kabylake DDR4 RVP17 configuration init function in DXE init phase.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 - 2019 Intel Corporation.

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

#include "KabylakeDdr4Rvp17InitLib.h"

#include <Library/GpioLib.h>
#include <PchAccess.h>
#include <Library/PchSerialIoLib.h>
#include <PlatformBoardConfig.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>

//
// Kabylake DDR4 RVP17 Init Table
//
BOARD_INIT_TABLE mKabylakeDdr4Rvp17Init [] = {
  {BoardIdKabylakeDdr4Rvp17, KabylakeDdr4Rvp17Init},
  { 0, 0 }
};

CHAR16 *KblRvp17BoardName = L"Kabylake DDR4 RVP17";

//SKL PCH H GPIO Expander Number
#define IO_EXPANDER_0            0
#define IO_EXPANDER_1            1

//SKL PCH H GPIO Pin Mapping
#define IO_EXPANDER_GPIO_5        5   // P05
#define IO_EXPANDER_GPIO_11       11  // P13

/**
  Init Platform Board Config Block for ACPI platform.

  @param[in]  PlatformInfo      a pointer to Platform Info.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
KabylakeDdr4Rvp17AcpiConfigBlockInit (
  IN PLATFORM_INFO *PlatformInfo
)
{
  EFI_STATUS                   Status;
  UINT16                       BoardId;
  UINT16                       BoardIdOrgValue;

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Update OEM table ID
  //
  PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'r', 'v', 'p', '1', '1'));

  // Modify Preferred_PM_Profile field based on Board SKU's. Default is set to Mobile
  //
  PcdSet8S (PcdPreferredPmProfile, EFI_ACPI_2_0_PM_PROFILE_MOBILE);

  PcdSetBoolS (PcdTherSdsTableEnable, FALSE);

  PcdSet8S (PcdSpecificIoExpanderBus, IO_EXPANDER_DISABLED);

  //
  // Assign FingerPrint, Gnss, BLuetooth & TouchPanel relative GPIO.
  //
  PcdSet32S (PcdFingerPrintIrqGpio  , GPIO_SKL_H_GPP_A22);// DRDY
  PcdSet32S (PcdFingerPrintSleepGpio, GPIO_SKL_H_GPP_A12);// FSLP
  PcdSet32S (PcdGnssResetGpio       , GPIO_SKL_H_GPP_A0);//not connected
  PcdSet32S (PcdBluetoothWakeGpio   , GPIO_SKL_H_GPP_A0);//not connected
  PcdSet32S (PcdBluetoothRfKillGpio , GPIO_SKL_H_GPP_B3);
  PcdSet32S (PcdBluetoothIrqGpio    , GPIO_SKL_H_GPP_B5);
  PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_H_GPP_F9);
  PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_H_GPP_E7);

  PcdSet32S(PcdPchAudioPowerEnable, GPIO_SKL_H_GPP_H10);
  PcdSet32S(PcdPcieSlot2Wake, GPIO_SKL_H_GPP_G18);
  PcdSet32S(PcdM2WlanReset, GPIO_SKL_H_GPP_F2);
  PcdSet32S(PcdUsbP8Power, GPIO_SKL_H_GPP_H7);
  PcdSet32S(PcdM2PcieSsd2Reset, GPIO_SKL_H_GPP_F18);
  PcdSet32S(PcdM2PcieSsd2Power, GPIO_SKL_H_GPP_G9);
  PcdSetBoolS (PcdEInkDfuDevice, FALSE);

  //
  // Board Specific Init
  //
  PcdSetBoolS (PcdRvp11Rtd3TableEnable, TRUE);

  //
  // Updating Channel Slot Map For SMBIOS Table Type 17
  //
  PcdSet8S(PcdChannelASlotMap, 0x03);
  PcdSet8S(PcdChannelBSlotMap, 0x03);

  return EFI_SUCCESS;
}

/**
  Init Common Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
KabylakeDdr4Rvp17CommonConfigBlockInit (
  IN PLATFORM_INFO *PlatformInfo
)
{
  EFI_STATUS                   Status;
  UINT16                       BoardId;
  UINT16                       BoardIdOrgValue;
  PCD64_BLOB                   Data64;

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Board Name
  //
  PcdSet64S (PcdSmbiosFabBoardName, (UINTN) KblRvp17BoardName);

  //
  // HID I2C Interrupt GPIO.
  //
  PcdSet32S (PcdHidI2cIntPad, GPIO_SKL_LP_GPP_E7);

  //
  // HaloMd2 Specific Init.
  //

  PcdSetBoolS (PcdHaloMd2Present, TRUE);

  //
  // DDISelection
  //

  PcdSet8S (PcdDDISelection, FALSE);

  //
  //Ssd turn off in Sx
  //

  PcdSetBoolS(PcdSsdPwrOverrideInSx, TRUE);

  //
  // Sensor Hub
  //

  PcdSetBoolS (PcdSensorHubSupport, FALSE);

  //
  // GFX Detect
  //
  //RVP8,9,10,11,16, SaddleBrook
  //GPIO_SKL_H_GPP_F13 - SATA_SDATAOUT0/GFX_CRB_DETECT for SPT-H and GPIO_SKL_H_GPP_D23 for uSFF
  Data64.BoardGpioConfig.Type = BoardGpioTypePch;
  Data64.BoardGpioConfig.u.Pin = GPIO_SKL_H_GPP_F13;
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
  PcdSetBoolS(PcdH8S2113SIO, TRUE);

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
  // PBOK
  //
  PcdSetBoolS (PcdEnablePbok, TRUE);

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
  PcdSet8S (PcdZPODD, 3);

  //
  //  DPTF: VSCT table check for Virtual Participants.
  //
  PcdSetBoolS (PcdVsctTableForSens, FALSE);  // load SEN taget in VSCT table for KBL-R.

  //
  // SMC Runtime Sci Pin
  //
  PcdSet32S(PcdSmcRuntimeSciPin, (UINT32)GPIO_SKL_H_GPP_G3);
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
  PcdSetBoolS(PcdVirtualButtonHomeButtonSupport, FALSE);
  PcdSetBoolS(PcdVirtualButtonRotationLockSupport, FALSE);

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
  PcdSet32S(PcdPmPowerButtonGpioPin, 0x00);

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
  KabylakeDdr4Rvp17 board configuration init function for DXE memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI
KabylakeDdr4Rvp17Init (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformInfo = (PLATFORM_INFO *) Content;

  Status = KabylakeDdr4Rvp17AcpiConfigBlockInit (PlatformInfo);
  ASSERT_EFI_ERROR(Status);

  Status = KabylakeDdr4Rvp17CommonConfigBlockInit (PlatformInfo);
  ASSERT_EFI_ERROR(Status);


  return EFI_SUCCESS;
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
DxeKabylakeDdr4Rvp17InitLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return CommonInitInternalLibConstructor (mKabylakeDdr4Rvp17Init, TABLE_LIST_NUM);
}

