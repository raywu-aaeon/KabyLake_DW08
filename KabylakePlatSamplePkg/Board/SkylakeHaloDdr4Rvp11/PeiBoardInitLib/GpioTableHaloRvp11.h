/** @file
  GPIO definition table.
  This file was automatically generated. Modify at your own risk.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2017 Intel Corporation.

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

@par Specification
**/
#ifndef _GPIO_TABLE_HALO_RVP11_H
#define _GPIO_TABLE_HALO_RVP11_H

#include <GpioPinsSklH.h>

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTableHaloRvp11[] =
{
              {GPIO_SKL_H_GPP_A0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //KBRST
              {GPIO_SKL_H_GPP_A1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LAD0
              {GPIO_SKL_H_GPP_A2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LAD1
              {GPIO_SKL_H_GPP_A3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LAD2
              {GPIO_SKL_H_GPP_A4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LAD3
              {GPIO_SKL_H_GPP_A5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //L_FRAME_N
              {GPIO_SKL_H_GPP_A6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //SERIRQ
              {GPIO_SKL_H_GPP_A7,  {GpioPadModeNative2, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //ESPI ALERT
              {GPIO_SKL_H_GPP_A8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //CLKRUN
              {GPIO_SKL_H_GPP_A9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LPC 0 CLOCK
              {GPIO_SKL_H_GPP_A10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LPC 1 CLOCK
              {GPIO_SKL_H_GPP_A11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntEdge      | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //EC_HID_INTR.
              {GPIO_SKL_H_GPP_A12, {GpioPadModeGpio   , GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SX_EXIT_HOLDOFF
              {GPIO_SKL_H_GPP_A13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SUSWARN
              {GPIO_SKL_H_GPP_A14, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //LPCPD
              {GPIO_SKL_H_GPP_A15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SUS PWR ACK
              {GPIO_SKL_H_GPP_A16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SLP_S0IX_N
              {GPIO_SKL_H_GPP_A17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 WLAN power enable
              {GPIO_SKL_H_GPP_A18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //Sensor header
              {GPIO_SKL_H_GPP_A19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //Sensor header
              {GPIO_SKL_H_GPP_A20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //sensor header
              {GPIO_SKL_H_GPP_A21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //GNSS CHUB IRQ
              {GPIO_SKL_H_GPP_A22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //FPS SLP
              {GPIO_SKL_H_GPP_A23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //FPS DRDY
              {GPIO_SKL_H_GPP_B0,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //screen lock
              {GPIO_SKL_H_GPP_B1,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //TCH_PNL_PWREN
  //Not Used  {GPIO_SKL_H_GPP_B2,  {GpioPadMode       , GpioHostOwn       , GpioDir        , GpioOut       , GpioInt          | , GpioReset       , GpioTerm        }}, //VRALERTB- not used
              {GPIO_SKL_H_GPP_B3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //BT_RF-KILL
              {GPIO_SKL_H_GPP_B4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SNI_DRV_PCH
              {GPIO_SKL_H_GPP_B5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//M.2_BT_UART_WAKE_N
              // {GPIO_SKL_H_GPP_B6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //CLKREQ - WIGIG
              // {GPIO_SKL_H_GPP_B7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //CLKREQ - WIFI
              // {GPIO_SKL_H_GPP_B8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //CLKREQ - PCIE X1 SLOT 1
              // {GPIO_SKL_H_GPP_B9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //CLKREQ - PCIE X4 SLOT 2
              // {GPIO_SKL_H_GPP_B10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //CLKREQ  - SATAE, SSD1
              {GPIO_SKL_H_GPP_B11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //MPHY_EXT_PWR_GATEB
              {GPIO_SKL_H_GPP_B12, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //PM_SLP_S0_N
              {GPIO_SKL_H_GPP_B13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //PLT_RST_N
              {GPIO_SKL_H_GPP_B14, {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutHigh   , GpioIntEdge      | GpioIntSmi, GpioHostDeepReset   , GpioTermNone }}, //SMC_EXTSMI
              // {GPIO_SKL_H_GPP_B15, {GpioPadModeGpio   , GpioHostOwnDefault, GpioDirOut     , GpioOutLow    , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, // NFC DFU, NOT OWNED BY BIOS
              {GPIO_SKL_H_GPP_B16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}}, //M.2_WLAN_WIFI_WAKE_N
              {GPIO_SKL_H_GPP_B17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //WIFI_RF_KILL_R_N
              {GPIO_SKL_H_GPP_B18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow   , GpioIntDis      , GpioResetDeep   , GpioTermNone }}, //pcie_slot2_pwren_wkctrl_n
              {GPIO_SKL_H_GPP_B19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FINGERPRINT SENSOR
              {GPIO_SKL_H_GPP_B20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FINGERPRINT SENSOR
              {GPIO_SKL_H_GPP_B21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FINGERPRINT SENSOR
              {GPIO_SKL_H_GPP_B22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FINGERPRINT SENSOR
              {GPIO_SKL_H_GPP_B23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //GNSS DISABLE
              {GPIO_SKL_H_GPP_C0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SMB_CLK_RESUME
              {GPIO_SKL_H_GPP_C1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SMB_DATA_RESUME
              {GPIO_SKL_H_GPP_C2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermWpd20K  }}, //TLS CONFIDENTIALITY/skin thermal sensor alert
              {GPIO_SKL_H_GPP_C3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //smlink 0 clk
              {GPIO_SKL_H_GPP_C4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //smlink 0 data
  //Not Used  {GPIO_SKL_H_GPP_C5,  {GpioPadMode       , GpioHostOwn       , GpioDir        , GpioOut       , GpioInt          | , GpioHostDeepReset   , GpioTermWpd20K  }}, //only strap- not used
              {GPIO_SKL_H_GPP_C6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //smlink 1 clk
              {GPIO_SKL_H_GPP_C7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //smlink 1 data
              {GPIO_SKL_H_GPP_C8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 BT, level shifter added
              {GPIO_SKL_H_GPP_C9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 BT, level shifter added
              {GPIO_SKL_H_GPP_C10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 BT, level shifter added
              {GPIO_SKL_H_GPP_C11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 BT, level shifter added
              {GPIO_SKL_H_GPP_C12, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //used for discreet gnss, level shifter added
              {GPIO_SKL_H_GPP_C13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //used for discreet gnss, level shifter added
              {GPIO_SKL_H_GPP_C14, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //used for discreet gnss, level shifter added
              {GPIO_SKL_H_GPP_C15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //used for discreet gnss, level shifter added
              {GPIO_SKL_H_GPP_C16, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //multiple devices
              {GPIO_SKL_H_GPP_C17, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //multiple devices
              {GPIO_SKL_H_GPP_C18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //touch panel
              {GPIO_SKL_H_GPP_C19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //touch panel
              {GPIO_SKL_H_GPP_C20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //serial port,, header
              {GPIO_SKL_H_GPP_C21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //serial port,, header
              {GPIO_SKL_H_GPP_C22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //serial port,, header
              {GPIO_SKL_H_GPP_C23, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //serial port,, header
              {GPIO_SKL_H_GPP_D0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_CS_N
              {GPIO_SKL_H_GPP_D1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_CLK
              {GPIO_SKL_H_GPP_D2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_MISO
              {GPIO_SKL_H_GPP_D3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_MOSI
              {GPIO_SKL_H_GPP_D4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //comms hub
              {GPIO_SKL_H_GPP_D5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SSP0_SFRM
              {GPIO_SKL_H_GPP_D6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SSP0_TXD
              {GPIO_SKL_H_GPP_D7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SSP0_RXD
              {GPIO_SKL_H_GPP_D8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SSP0_SCLK
  //Not Used  {GPIO_SKL_H_GPP_D9,  {GpioPadMode       , GpioHostOwn       , GpioDir        , GpioOut       , GpioInt          | , GpioHostDeepReset   , GpioTerm        }}, //TP- not used
              {GPIO_SKL_H_GPP_D10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //M.2_WWAN_DISABLE_N
  //Not Used  {GPIO_SKL_H_GPP_D11, {GpioPadMode       , GpioHostOwn       , GpioDir        , GpioOut       , GpioInt          | , GpioHostDeepReset   , GpioTerm        }}, //TP- not used
  //Not Used  {GPIO_SKL_H_GPP_D12, {GpioPadMode       , GpioHostOwn       , GpioDir        , GpioOut       , GpioInt          | , GpioHostDeepReset   , GpioTerm        }}, //TP- not used
              {GPIO_SKL_H_GPP_D13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //ISH UART
              {GPIO_SKL_H_GPP_D14, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //ISH UART
              {GPIO_SKL_H_GPP_D15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //ISH UART
              {GPIO_SKL_H_GPP_D16, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //ISH UART
              {GPIO_SKL_H_GPP_D17, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //DMIC_CLK_1
              {GPIO_SKL_H_GPP_D18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermWpd20K | GpioTolerance1v8 }}, //DMIC_DATA_1
              {GPIO_SKL_H_GPP_D19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //DMIC_CLK_0
              {GPIO_SKL_H_GPP_D20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermWpd20K | GpioTolerance1v8 }}, //DMIC_DATA_0
              {GPIO_SKL_H_GPP_D21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_IO2
              {GPIO_SKL_H_GPP_D22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_IO3
              {GPIO_SKL_H_GPP_D23, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //comms hub
              {GPIO_SKL_H_GPP_E0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //use for IF DETECT for SATAe OR M.2 SSD (1) detect, VISA
              {GPIO_SKL_H_GPP_E1,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirDefault , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //visa only
              {GPIO_SKL_H_GPP_E2,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirDefault , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //visa only
              {GPIO_SKL_H_GPP_E3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //EXTTS_SNI_DRV0, VISA
              {GPIO_SKL_H_GPP_E4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //devslp0 for SATAe direct, M.2 SSD, VISA
            //{GPIO_SKL_H_GPP_E5,  {GpioPadModeNative1, GpioHostOwnGpio   , GpioDirNone    , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone}}, //retain as pch nfc reset, NOT OWNED BY BIOS
              {GPIO_SKL_H_GPP_E6,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirDefault , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //visa only
            //{GPIO_SKL_H_GPP_E7,  {GpioPadModeGpio   , GpioHostOwnDefault, GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioPlatformReset   , GpioTermNone }}, //TCH_PNL_INTR, visa
              {GPIO_SKL_H_GPP_E8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //hard drive LED, visa
              {GPIO_SKL_H_GPP_E9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //OC, VISA
              {GPIO_SKL_H_GPP_E10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //OC, VISA
              {GPIO_SKL_H_GPP_E11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //OC, VISA
              {GPIO_SKL_H_GPP_E12, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //OC, VISA
              {GPIO_SKL_H_GPP_F0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //ODD DETECT, visa
              {GPIO_SKL_H_GPP_F1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //GP-M.2 SSD2, visa
              {GPIO_SKL_H_GPP_F2,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //m.2 wifi reset, visa
              {GPIO_SKL_H_GPP_F3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //tch_pnl_reset, visa
              {GPIO_SKL_H_GPP_F4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //pcie slot 2 reset, visa
              {GPIO_SKL_H_GPP_F5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntEdge      | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //pch codec irq
              {GPIO_SKL_H_GPP_F6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //sata SSD2 devslp
              {GPIO_SKL_H_GPP_F7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //ME PG LED
              {GPIO_SKL_H_GPP_F8,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SATAE CLK DET/ BT DEV WAKE
              {GPIO_SKL_H_GPP_F9,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioPlatformReset   , GpioTermNone }}, //tch pad intr, wwan wake
              // move to pre-memory phase {GPIO_SKL_H_GPP_F10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //BIOS REC
              {GPIO_SKL_H_GPP_F11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //PCH RSVD
              {GPIO_SKL_H_GPP_F12, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioPlatformReset , GpioTermNone }}, //MFG MODE/ PEG X16 WAKE
              {GPIO_SKL_H_GPP_F13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //GFX_CRB_DETECT
              {GPIO_SKL_H_GPP_F14, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //H_SKTOCC_N
              {GPIO_SKL_H_GPP_F15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_OC4_R, VISA
              {GPIO_SKL_H_GPP_F16, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_OC5_R
              {GPIO_SKL_H_GPP_F17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 SSD1 RESET
              {GPIO_SKL_H_GPP_F18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 SSD2 RESET
              {GPIO_SKL_H_GPP_F19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //L_VDDEN
              {GPIO_SKL_H_GPP_F20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //L_BKLTEN
              {GPIO_SKL_H_GPP_F21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //L_BKLTCTL
              {GPIO_SKL_H_GPP_F22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //peg slot reset
              //{GPIO_SKL_H_GPP_F23, {GpioPadModeGpio   , GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //NFC IRQ, NOT OWNED BY BIOS
              {GPIO_SKL_H_GPP_G0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermWpd20K}}, //m.2 wigig wake
              {GPIO_SKL_H_GPP_G1,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SATA_ODD_PWRGT
              {GPIO_SKL_H_GPP_G2,  {GpioPadModeNative1, GpioHostOwnGpio   , GpioDirNone    ,  GpioOutDefault, GpioIntDis     , GpioHostDeepReset   , GpioTermNone}}, //SATA_ODD_DA_INT
              {GPIO_SKL_H_GPP_G3,  {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioPlatformReset , GpioTermNone    }}, //SMC_RUNTIME_SCI_N
              {GPIO_SKL_H_GPP_G4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2_WIGIG_WAKE_CTRL_N
              {GPIO_SKL_H_GPP_G5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //satae reset
              {GPIO_SKL_H_GPP_G6,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2_WWAN_GNSS_UART_RST
              {GPIO_SKL_H_GPP_G7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutLow    , GpioIntLevel     | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //PCH_M.2_WWAN_UIM_SIM_DET
              {GPIO_SKL_H_GPP_G8,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2_SSD1_PWREN
              {GPIO_SKL_H_GPP_G9,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2_SSD2_PWREN
              {GPIO_SKL_H_GPP_G10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SNSR_HUB_DFU
              {GPIO_SKL_H_GPP_G11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SATA_DIRECT_PWREN
              {GPIO_SKL_H_GPP_G12, {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntEdge      | GpioIntSci, GpioPlatformReset   , GpioTermWpd20K  }}, //TBCIO_PLUG_EVENT
              {GPIO_SKL_H_GPP_G13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //IVCAM_PWRGT_R
  //Not Used  {GPIO_SKL_H_GPP_G14, {GpioPadMode       , GpioHostOwn       , GpioDir        , GpioOut       , GpioInt          | , GpioHostDeepReset   , GpioTermNone }}, //pulldown- NOT USED
              {GPIO_SKL_H_GPP_G15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 WWAN PWREN
              {GPIO_SKL_H_GPP_G16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDis      , GpioPlatformReset , GpioTermWpd20K    }}, //GSXCLK/TB_FRC_PWR
              {GPIO_SKL_H_GPP_G17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 WIGIG PWREN
              {GPIO_SKL_H_GPP_G18, {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone }}, //SLOT2 wake
  //Not Used  {GPIO_SKL_H_GPP_G19, {GpioPadMode       , GpioHostOwn       , GpioDir        , GpioOut       , GpioInt          | , GpioHostDeepReset   , GpioTermNone }}, //PU and optional PD- NOT USED
              {GPIO_SKL_H_GPP_G20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //discreet gnss reset
              {GPIO_SKL_H_GPP_G21, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntEdge | GpioIntApic, GpioHostDeepReset,  GpioTermWpd20K}}, //SPI TPM PIRQ
              {GPIO_SKL_H_GPP_G22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset   , GpioTermNone }}, //PEGSLOT_PWREN
              {GPIO_SKL_H_GPP_G23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //IVCAM_DFU_R
              {GPIO_SKL_H_GPP_H0,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //MP_GP_1
           // {GPIO_SKL_H_GPP_H1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //CLK_REQ7_M.2_SSD2_N
              {GPIO_SKL_H_GPP_H2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset   , GpioTermNone }}, // PEG; PCIEX16_PRSNT2_R_N
           // {GPIO_SKL_H_GPP_H3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //CLK_REQ_LAN_n
              {GPIO_SKL_H_GPP_H4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2_WIGIG_RST_n
              {GPIO_SKL_H_GPP_H5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //MP_GP_2
              {GPIO_SKL_H_GPP_H6,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_PWREN_OTG_1
              {GPIO_SKL_H_GPP_H7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_PWREN_P8
              {GPIO_SKL_H_GPP_H8,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_PWREN_P9
              {GPIO_SKL_H_GPP_H9,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FPS_LOCK_R_N
              {GPIO_SKL_H_GPP_H10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //PCH_AUDIO_PWREN
              {GPIO_SKL_H_GPP_H11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_PWREN_P14
  //Not Used  {GPIO_SKL_H_GPP_H12, {GpioPadMode       , GpioHostOwn       , GpioDir        , GpioOut       , GpioInt          | , GpioHostDeepReset   , GpioTerm        }}, //strap - not used
              {GPIO_SKL_H_GPP_H13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //home button
              {GPIO_SKL_H_GPP_H14, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //VOL_UP_R
              {GPIO_SKL_H_GPP_H15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //VOL_DOWN_R
              {GPIO_SKL_H_GPP_H16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_PWREN_P13
              {GPIO_SKL_H_GPP_H17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_PWREN_P11
              {GPIO_SKL_H_GPP_H18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_PWREN_P10
              {GPIO_SKL_H_GPP_H19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //ish i2c
              {GPIO_SKL_H_GPP_H20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //ish i2c
              {GPIO_SKL_H_GPP_H21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //ish i2c
              {GPIO_SKL_H_GPP_H22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //ish i2c
              {GPIO_SKL_H_GPP_H23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2_WLAN_WAKE_CTRL_N
              {GPIO_SKL_H_GPP_I0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD0,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD1,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD2,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioDswReset, GpioTermNative  }}, //LANWAKE_SMC_WAKE_SCI_N
              {GPIO_SKL_H_GPD3,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD4,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD5,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD6,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD7,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD8,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD9,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD10,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD11,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTableHaloRvp11uSff[] =
{
              {GPIO_SKL_H_GPP_A0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //KBRST
              {GPIO_SKL_H_GPP_A1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LAD0
              {GPIO_SKL_H_GPP_A2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LAD1
              {GPIO_SKL_H_GPP_A3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LAD2
              {GPIO_SKL_H_GPP_A4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LAD3
              {GPIO_SKL_H_GPP_A5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //L_FRAME_N
              {GPIO_SKL_H_GPP_A6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //SERIRQ
              {GPIO_SKL_H_GPP_A7,  {GpioPadModeNative2, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //ESPI ALERT
              {GPIO_SKL_H_GPP_A8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //CLKRUN
              {GPIO_SKL_H_GPP_A9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LPC 0 CLOCK
              {GPIO_SKL_H_GPP_A10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioResetDefault, GpioTermNone }}, //LPC 1 CLOCK
              {GPIO_SKL_H_GPP_A11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntEdge      | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //EC_HID_INTR.
              {GPIO_SKL_H_GPP_A12, {GpioPadModeGpio   , GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SX_EXIT_HOLDOFF
              {GPIO_SKL_H_GPP_A13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SUSWARN
              {GPIO_SKL_H_GPP_A14, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //LPCPD
              {GPIO_SKL_H_GPP_A15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SUS PWR ACK
              {GPIO_SKL_H_GPP_A16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SLP_S0IX_N
              {GPIO_SKL_H_GPP_A17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 WLAN power enable
              {GPIO_SKL_H_GPP_A18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //Sensor header
              {GPIO_SKL_H_GPP_A19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //Sensor header
              {GPIO_SKL_H_GPP_A20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //sensor header
              {GPIO_SKL_H_GPP_A21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //GNSS CHUB IRQ
              {GPIO_SKL_H_GPP_A22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //FPS SLP
              {GPIO_SKL_H_GPP_A23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //FPS DRDY
              {GPIO_SKL_H_GPP_B0,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //screen lock
              {GPIO_SKL_H_GPP_B1,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //TCH_PNL_PWREN
              {GPIO_SKL_H_GPP_B3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //BT_RF-KILL
              {GPIO_SKL_H_GPP_B4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SNI_DRV_PCH
              {GPIO_SKL_H_GPP_B5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//M.2_BT_UART_WAKE_N
              {GPIO_SKL_H_GPP_B6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn    , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset , GpioTermNone   }}, //PCIEX16_PRSNT2_R_N
              {GPIO_SKL_H_GPP_B8,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut    , GpioOutHigh, GpioIntDefault  , GpioHostDeepReset , GpioTermNone   }}, //PCH_AUDIO_PWREN
              {GPIO_SKL_H_GPP_B10, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn     , GpioOutDefault, GpioIntDis  , GpioHostDeepReset , GpioTermNone    }}, //H_SKTOCC_N
              {GPIO_SKL_H_GPP_B11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //MPHY_EXT_PWR_GATEB
              {GPIO_SKL_H_GPP_B12, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //PM_SLP_S0_N
              {GPIO_SKL_H_GPP_B13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //PLT_RST_N
              {GPIO_SKL_H_GPP_B14, {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutHigh   , GpioIntEdge      | GpioIntSmi, GpioHostDeepReset   , GpioTermNone }}, //SMC_EXTSMI
              {GPIO_SKL_H_GPP_B15, {GpioPadModeGpio  , GpioHostOwnAcpi, GpioDirInInv   , GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset , GpioTermNone   }}, //slot2 wake
              {GPIO_SKL_H_GPP_B16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}}, //M.2_WLAN_WIFI_WAKE_N
              {GPIO_SKL_H_GPP_B17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //WIFI_RF_KILL_R_N
              {GPIO_SKL_H_GPP_B18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow   , GpioIntDis      , GpioResetDeep   , GpioTermNone }}, //pcie_slot2_pwren_wkctrl_n
              {GPIO_SKL_H_GPP_B19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FINGERPRINT SENSOR
              {GPIO_SKL_H_GPP_B20, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut     , GpioOutHigh, GpioIntDefault, GpioHostDeepReset , GpioTermNone  }}, //GPIO_PCIE_RST
              {GPIO_SKL_H_GPP_B21, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut     , GpioOutLow, GpioIntDefault, GpioHostDeepReset , GpioTermNone  }}, //m.2 wifi reset
              {GPIO_SKL_H_GPP_B22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FINGERPRINT SENSOR
              {GPIO_SKL_H_GPP_B23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //GNSS DISABLE
              {GPIO_SKL_H_GPP_C0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SMB_CLK_RESUME
              {GPIO_SKL_H_GPP_C1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SMB_DATA_RESUME
              {GPIO_SKL_H_GPP_C2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermWpd20K  }}, //TLS CONFIDENTIALITY/skin thermal sensor alert
              {GPIO_SKL_H_GPP_C3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //smlink 0 clk
              {GPIO_SKL_H_GPP_C4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //smlink 0 data
              {GPIO_SKL_H_GPP_C6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //smlink 1 clk
              {GPIO_SKL_H_GPP_C7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //smlink 1 data
              {GPIO_SKL_H_GPP_C8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 BT, level shifter added
              {GPIO_SKL_H_GPP_C9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 BT, level shifter added
              {GPIO_SKL_H_GPP_C10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 BT, level shifter added
              {GPIO_SKL_H_GPP_C11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //M.2 BT, level shifter added
              {GPIO_SKL_H_GPP_C12, {GpioPadModeGpio  , GpioHostOwnGpio  , GpioDirOut   , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset , GpioTermNone   }}, //USB_PWREN_P8
              {GPIO_SKL_H_GPP_C13, {GpioPadModeGpio  , GpioHostOwnGpio  , GpioDirOut   , GpioOutLow   , GpioIntDefault  , GpioHostDeepReset , GpioTermNone   }}, //M.2_WLAN_WAKE_CTRL_N
              {GPIO_SKL_H_GPP_C14, {GpioPadModeGpio  , GpioHostOwnGpio  , GpioDirOut   , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset , GpioTermNone   }}, //M.2_WLAN_WAKE_CTRL_N
              {GPIO_SKL_H_GPP_C15, {GpioPadModeGpio  , GpioHostOwnAcpi  , GpioDirInInv   , GpioOutDefault, GpioIntLevel | GpioIntSci  , GpioPlatformReset , GpioTermNone}}, //SMC_RUNTIME_SCI
              {GPIO_SKL_H_GPP_C16, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //multiple devices
              {GPIO_SKL_H_GPP_C17, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //multiple devices
              {GPIO_SKL_H_GPP_C18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //touch panel
              {GPIO_SKL_H_GPP_C19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //touch panel
              {GPIO_SKL_H_GPP_C20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //serial port,, header
              {GPIO_SKL_H_GPP_C21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //serial port,, header
              {GPIO_SKL_H_GPP_C22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //serial port,, header
              {GPIO_SKL_H_GPP_C23, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //serial port,, header
              {GPIO_SKL_H_GPP_D0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_CS_N
              {GPIO_SKL_H_GPP_D1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_CLK
              {GPIO_SKL_H_GPP_D3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_MOSI
              {GPIO_SKL_H_GPP_D4,  {GpioPadModeGpio  , GpioHostOwnGpio  , GpioDirIn    , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset , GpioTermNone   }}, //BIOS REC
              {GPIO_SKL_H_GPP_D5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SSP0_SFRM
              {GPIO_SKL_H_GPP_D6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SSP0_TXD
              {GPIO_SKL_H_GPP_D7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SSP0_RXD
              {GPIO_SKL_H_GPP_D8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SSP0_SCLK
              {GPIO_SKL_H_GPP_D9,  {GpioPadModeGpio  , GpioHostOwnAcpi  , GpioDirInInv   , GpioDirDefault, GpioIntEdge | GpioIntSci, GpioPlatformReset , GpioTermWpd20K}}, //TBCIO_PLUG_EVENT
              {GPIO_SKL_H_GPP_D10, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioHostDeepReset, GpioTermNone } }, //M.2 SSD2 RESET
              {GPIO_SKL_H_GPP_D11, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutLow, GpioIntDefault, GpioPlatformReset, GpioTermWpd20K } }, //TB_FRC_PWR_R
              {GPIO_SKL_H_GPP_D12, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutHigh, GpioIntDefault, GpioHostDeepReset, GpioTermNone } }, //M.2_SSD2_PWREN
              {GPIO_SKL_H_GPP_D13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //ISH UART
              {GPIO_SKL_H_GPP_D14, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //ISH UART
              {GPIO_SKL_H_GPP_D15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //ISH UART
              {GPIO_SKL_H_GPP_D16, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //ISH UART
              {GPIO_SKL_H_GPP_D17, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //DMIC_CLK_1
              {GPIO_SKL_H_GPP_D18, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermWpd20K | GpioTolerance1v8 }}, //DMIC_DATA_1
              {GPIO_SKL_H_GPP_D19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //DMIC_CLK_0
              {GPIO_SKL_H_GPP_D20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermWpd20K | GpioTolerance1v8 }}, //DMIC_DATA_0
              {GPIO_SKL_H_GPP_D21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_IO2
              {GPIO_SKL_H_GPP_D22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone | GpioTolerance1v8 }}, //SPI1_TCHPNL_IO3
              {GPIO_SKL_H_GPP_D23, {GpioPadModeGpio  , GpioHostOwnGpio  , GpioDirIn    , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset , GpioTermNone   }}, //GFX_CRB_DETECT
              {GPIO_SKL_H_GPP_E0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //use for IF DETECT for SATAe OR M.2 SSD (1) detect, VISA
              {GPIO_SKL_H_GPP_E1,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirDefault , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //visa only
              {GPIO_SKL_H_GPP_E2,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirDefault , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //visa only
              {GPIO_SKL_H_GPP_E3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //EXTTS_SNI_DRV0, VISA
              {GPIO_SKL_H_GPP_E4, {GpioPadModeGpio  , GpioHostOwnGpio  , GpioDirIn    , GpioOutDefault, GpioIntEdge | GpioIntApic , GpioHostDeepReset , GpioTermNone  }}, //pch codec irq
              {GPIO_SKL_H_GPP_E6,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirDefault , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //visa only
              {GPIO_SKL_H_GPP_E8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //hard drive LED, visa
              {GPIO_SKL_H_GPP_E9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //OC, VISA
              {GPIO_SKL_H_GPP_E10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //OC, VISA
              {GPIO_SKL_H_GPP_E11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //OC, VISA
              {GPIO_SKL_H_GPP_E12, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutHigh   , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //OC, VISA
              {GPIO_SKL_H_GPP_F4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //pcie slot 2 reset, visa
              // move to pre-memory phase {GPIO_SKL_H_GPP_F10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //BIOS REC
              {GPIO_SKL_H_GPP_F11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //PCH RSVD
              {GPIO_SKL_H_GPP_F12, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioPlatformReset , GpioTermNone }}, //MFG MODE/ PEG X16 WAKE
              {GPIO_SKL_H_GPP_F13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //GFX_CRB_DETECT
              {GPIO_SKL_H_GPP_F19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //L_VDDEN
              {GPIO_SKL_H_GPP_F20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //L_BKLTEN
              {GPIO_SKL_H_GPP_F21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //L_BKLTCTL
              {GPIO_SKL_H_GPP_I0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPP_I10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD0,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD1,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD2,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioDswReset, GpioTermNative  }}, //LANWAKE_SMC_WAKE_SCI_N
              {GPIO_SKL_H_GPD3,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD4,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD5,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD8,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
              {GPIO_SKL_H_GPD10,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mGpioTableHaloRvp11Size = sizeof (mGpioTableHaloRvp11) / sizeof (GPIO_INIT_CONFIG);
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mGpioTableHaloRvp11uSffSize = sizeof (mGpioTableHaloRvp11uSff) / sizeof (GPIO_INIT_CONFIG);

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTableHaloRvp11Touchpanel =
  {GPIO_SKL_H_GPP_E7, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermNone}};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTableHaloRvp11PreMem[] =
{
                {GPIO_SKL_H_GPP_F10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //BIOS REC
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mGpioTableHaloRvp11PreMemSize = sizeof (mGpioTableHaloRvp11PreMem) / sizeof (GPIO_INIT_CONFIG);

#endif  // _GPIO_TABLE_HALO_RVP11_H
