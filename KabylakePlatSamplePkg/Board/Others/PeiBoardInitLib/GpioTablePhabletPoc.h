/** @file
  GPIO definition table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 Intel Corporation.

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
#ifndef _GPIO_TABLE_PHABLET_POC_H_
#define _GPIO_TABLE_PHABLET_POC_H_

static GPIO_INIT_CONFIG mGpioTablePhabletPoc[] =
{
//skip for eSPI function  {GPIO_SKL_LP_GPP_A0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//H_RCIN_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD0_ESPI_IO0
//skip for eSPI function  {GPIO_SKL_LP_GPP_A2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD1_ESPI_IO1
//skip for eSPI function  {GPIO_SKL_LP_GPP_A3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD2_ESPI_IO2
//skip for eSPI function  {GPIO_SKL_LP_GPP_A4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD3_ESPI_IO3
//skip for eSPI function  {GPIO_SKL_LP_GPP_A5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//LPC_FRAME_ESPI_CS_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//INT_SERIRQ
//skip for eSPI function  {GPIO_SKL_LP_GPP_A7, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_SLP_S0ix_R_N
  {GPIO_SKL_LP_GPP_A8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_CLKRUN_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_CLK_ESPI_CLK
//  {GPIO_SKL_LP_GPP_A10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//NC/TP - TP_LPC_CLK1 for SKL phablet
  {GPIO_SKL_LP_GPP_A11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//EC_HID_INTR
//  {GPIO_SKL_LP_GPP_A12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
  {GPIO_SKL_LP_GPP_A13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SUS_PWR_ACK_R
//skip for eSPI function  {GPIO_SKL_LP_GPP_A14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_SUS_STAT_ESPI_RST_N
  {GPIO_SKL_LP_GPP_A15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SUSACK_R_N
//  {GPIO_SKL_LP_GPP_A16, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
//  {GPIO_SKL_LP_GPP_A17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
  {GPIO_SKL_LP_GPP_A18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ACCEL_GYRO_INT_N
  {GPIO_SKL_LP_GPP_A19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//COMPASS_DRDY
  {GPIO_SKL_LP_GPP_A20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ALS_INT_N
  {GPIO_SKL_LP_GPP_A21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_ISH_IRQ
  {GPIO_SKL_LP_GPP_A22, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GPS2AP_INT_N -- NC OUTPUT as per arun suggestion
  {GPIO_SKL_LP_GPP_A23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet OUTPUT as per arun suggestion
//  {GPIO_SKL_LP_GPP_B0, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
//  {GPIO_SKL_LP_GPP_B1, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
  {GPIO_SKL_LP_GPP_B2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GP_VRALERTB
  {GPIO_SKL_LP_GPP_B3, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WLAN_PERST_N
//  {GPIO_SKL_LP_GPP_B4, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
//  {GPIO_SKL_LP_GPP_B5, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
  // {GPIO_SKL_LP_GPP_B6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
  {GPIO_SKL_LP_GPP_B7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLKREQ_WLAN_N
  {GPIO_SKL_LP_GPP_B8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLKREQ_WIGIG_N
  // {GPIO_SKL_LP_GPP_B9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
  // {GPIO_SKL_LP_GPP_B10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet
  {GPIO_SKL_LP_GPP_B11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MPHY_EXT_PWR_GATEB
  {GPIO_SKL_LP_GPP_B12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_SLP_S0_N
  {GPIO_SKL_LP_GPP_B13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PLT_RST_N
  {GPIO_SKL_LP_GPP_B14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//TCH_PNL_PWREN
  {GPIO_SKL_LP_GPP_B15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GSSPIO_CS_N - AudioCodec
  {GPIO_SKL_LP_GPP_B16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GSSPIO_CLK - AudioCodec
  {GPIO_SKL_LP_GPP_B17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//GSSPIO_MISO - AudioCodec
  {GPIO_SKL_LP_GPP_B18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//GSSPIO_MOSI - AudioCodec
  {GPIO_SKL_LP_GPP_B19, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh, GpioIntDis, GpioHostDeepReset,   GpioTermNone}},//WIGIG_DISABLE_RFKILL - Disabled RFKILL by default
  {GPIO_SKL_LP_GPP_B20, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CAP_BTN_LED_EN - Disabled CAPSENSE by default
  {GPIO_SKL_LP_GPP_B21, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WIGIG_RST_N - Disabled WIGIG by default
  {GPIO_SKL_LP_GPP_B22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_MOSI_R1
  {GPIO_SKL_LP_GPP_B23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntApic | GpioIntSci, GpioHostDeepReset,  GpioTermWpd20K}},//I2C_LID_BASE_EC_IRQ_R
//  {GPIO_SKL_LP_GPP_C0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//NC for SKL phablet - TP_SMB_CLK
//  {GPIO_SKL_LP_GPP_C1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//NC for SKL phablet - TP_SMB_DATA
//  {GPIO_SKL_LP_GPP_C2, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//NC for SKL phablet - TLS_CONF_PU
  {GPIO_SKL_LP_GPP_C3, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CODEC_RST_N
  {GPIO_SKL_LP_GPP_C4, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_TO_CODEC_EN
// {GPIO_SKL_LP_GPP_C5, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermWpd20K}},//WLAN_TIME_SYNC_INT -- NC - arun suggestion
  {GPIO_SKL_LP_GPP_C6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//I2C_LID_BASE_EC_CLK_R
  {GPIO_SKL_LP_GPP_C7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//I2C_LID_BASE_EC_DAT_R
  {GPIO_SKL_LP_GPP_C8, {GpioPadModeGpio,  GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//VOLUMEUP_BTN_N
  {GPIO_SKL_LP_GPP_C9, {GpioPadModeGpio,  GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//VOLUMEDOWN_BTN_N
  {GPIO_SKL_LP_GPP_C10, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_RSVD_GPIO1
  {GPIO_SKL_LP_GPP_C11, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_TXPWRTH
  {GPIO_SKL_LP_GPP_C12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_RXD - GNSS
  {GPIO_SKL_LP_GPP_C13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_TXD - GNSS
  {GPIO_SKL_LP_GPP_C14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_RTS_N - GNSS
  {GPIO_SKL_LP_GPP_C15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_CTS_N - GNSS
  {GPIO_SKL_LP_GPP_C16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C0_SDA - AUDIO I2C
  {GPIO_SKL_LP_GPP_C17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C0_SCL - AUDIO I2C
  {GPIO_SKL_LP_GPP_C18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C1_SDA - TOUCH PNL I2C
  {GPIO_SKL_LP_GPP_C19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C1_SCL - TOUCH PNL I2C
  {GPIO_SKL_LP_GPP_C20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RXD - PCH DEBUG UART
  {GPIO_SKL_LP_GPP_C21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_TXD - PCH DEBUG UART
//  {GPIO_SKL_LP_GPP_C22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RTS_N - NC for SKL phablet
//  {GPIO_SKL_LP_GPP_C23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_CTS_N - NC for SKL phablet
  {GPIO_SKL_LP_GPP_D0, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_GSMPULSE
  {GPIO_SKL_LP_GPP_D1, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//FLASH_MODEM_EN -- Not used make it input
  {GPIO_SKL_LP_GPP_D2, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//CAP_BTN_INT_N
  {GPIO_SKL_LP_GPP_D3, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_RST_OUT_N -- Configured as input - arun suggestion
  {GPIO_SKL_LP_GPP_D4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CSI2_FLASH_STROBE
  {GPIO_SKL_LP_GPP_D5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C0_SDA
  {GPIO_SKL_LP_GPP_D6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C0_SCL
//  {GPIO_SKL_LP_GPP_D7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C1_SDA - NC for SKL phablet
//  {GPIO_SKL_LP_GPP_D8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C1_SCL - NC for SKL phablet
  {GPIO_SKL_LP_GPP_D9, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//HOME_BTN
  {GPIO_SKL_LP_GPP_D10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//LNP_WLAN_EN
  {GPIO_SKL_LP_GPP_D11, {GpioPadModeNative1,    GpioHostOwnGpio, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BSSB_CLK in phablet
  {GPIO_SKL_LP_GPP_D12, {GpioPadModeNative1,    GpioHostOwnGpio, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BSSB_DI in phablet
  {GPIO_SKL_LP_GPP_D13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_RXD_SML0B_DATA - UART LTE WWAN
  {GPIO_SKL_LP_GPP_D14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_TXD_SML0B_CLK - UART LTE WWAN
  {GPIO_SKL_LP_GPP_D15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_RTS_N - UART LTE WWAN
  {GPIO_SKL_LP_GPP_D16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_CTS_SML0B_ALERT_N - UART LTE WWAN
  {GPIO_SKL_LP_GPP_D17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_CLK_1 - DMIC - Bottom Mic's
  {GPIO_SKL_LP_GPP_D18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DMIC_DATA_1 - DMIC - Bottom Mic's
  {GPIO_SKL_LP_GPP_D19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_CLK_0 - DMIC - Top Mic's
  {GPIO_SKL_LP_GPP_D20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DMIC_DATA_0 - DMIC - Top Mic's
  {GPIO_SKL_LP_GPP_D21, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_FM_ENABLE -- Disabled by default
  {GPIO_SKL_LP_GPP_D22, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//AP2GPS_INT_N  -- NC for phablet -- configured as output as per arun suggestion
  {GPIO_SKL_LP_GPP_D23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP_MCLK
//  {GPIO_SKL_LP_GPP_E0, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SATAXPCIE_0 - Will be configured by soft-trap in Phablet
//  {GPIO_SKL_LP_GPP_E1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATAXPCIE_1 - Will be configured by soft-trap in Phablet
//  {GPIO_SKL_LP_GPP_E2, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntLvlEdgDis | GpioIntApic | GpioIntSci, GpioHostDeepReset,  GpioTermNone}},SATAXPCIE_2 - Will be configured by soft-trap in Phablet
  {GPIO_SKL_LP_GPP_E3, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CAM_PMIC_RST
  {GPIO_SKL_LP_GPP_E4, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis | GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCIE_WAKE_PCH_N
  {GPIO_SKL_LP_GPP_E5, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MIPI_RESET_N
  {GPIO_SKL_LP_GPP_E6, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_USB_SEL -- this is used for modem flashing
  {GPIO_SKL_LP_GPP_E8, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MIPI_IO_PWREN1 -- //Nikhil Changed during Power-On
  {GPIO_SKL_LP_GPP_E9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_0_WP1_OTG_N
  {GPIO_SKL_LP_GPP_E10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_1_WP4_N
  {GPIO_SKL_LP_GPP_E11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MIPI_CORE_PWREN2 --- Nikhil changed during PowerOn
  {GPIO_SKL_LP_GPP_E12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WIGIG_WAKE_N
  {GPIO_SKL_LP_GPP_E13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI1_HPD_Q
  {GPIO_SKL_LP_GPP_E14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI2_HPD_Q
  {GPIO_SKL_LP_GPP_E15, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntSmi, GpioHostDeepReset,  GpioTermNone}},//SMC_EXTSMI_R_N
  {GPIO_SKL_LP_GPP_E16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}},//SMC_RUNTIME_SCI_R_N
  {GPIO_SKL_LP_GPP_E17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EDP_HPD
//  {GPIO_SKL_LP_GPP_E18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI1_CTRL_CLK - NC in SKL Phablet
//  {GPIO_SKL_LP_GPP_E19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DDI1_CTRL_DATA - set by soft strap
//  {GPIO_SKL_LP_GPP_E20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI2_CTRL_CLK - set by soft strap
//  {GPIO_SKL_LP_GPP_E21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DDI2_CTRL_DATA - NC in SKL Phablet
  {GPIO_SKL_LP_GPP_E22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_CODEC_IRQ
  {GPIO_SKL_LP_GPP_E23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut, GpioOutLow, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//TOUCH_PNL_RST_N
//  {GPIO_SKL_LP_GPP_F0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_SCLK - NC in SKL Phablet
//  {GPIO_SKL_LP_GPP_F1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_SFRM - NC in SKL Phablet
//  {GPIO_SKL_LP_GPP_F2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_TXD - NC in SKL Phablet
//  {GPIO_SKL_LP_GPP_F3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_RXD - NC in SKL Phablet
  {GPIO_SKL_LP_GPP_F4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SDA -- I2C for Camera/PMIC
  {GPIO_SKL_LP_GPP_F5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SCL -- I2C for Camera/PMIC
  {GPIO_SKL_LP_GPP_F6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SDA -- I2C for Capsense
  {GPIO_SKL_LP_GPP_F7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SCL -- I2C for Capsense
  {GPIO_SKL_LP_GPP_F8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C4_SDA -- I2C for eDP MIPI Bridge Chip
  {GPIO_SKL_LP_GPP_F9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C4_SCL -- I2C for eDP MIPI Bridge Chip
  {GPIO_SKL_LP_GPP_F10, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C5_ISH_12C2_SDA -- I2C for LTE WWAN
  {GPIO_SKL_LP_GPP_F11, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C5_ISH_12C2_SCL -- I2C for LTE WWAN
  {GPIO_SKL_LP_GPP_F12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_CMD
  {GPIO_SKL_LP_GPP_F13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA0
  {GPIO_SKL_LP_GPP_F14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA1
  {GPIO_SKL_LP_GPP_F15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA2
  {GPIO_SKL_LP_GPP_F16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA3
  {GPIO_SKL_LP_GPP_F17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA4
  {GPIO_SKL_LP_GPP_F18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA5
  {GPIO_SKL_LP_GPP_F19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA6
  {GPIO_SKL_LP_GPP_F20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA7
  {GPIO_SKL_LP_GPP_F21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_RCLK
  {GPIO_SKL_LP_GPP_F22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_CLK
 // Commenting as this Interrupt doenot seem to be a Sharable interrupt ... Gives a strange error in Bios when configuring it as a Interrupt.
 // {GPIO_SKL_LP_GPP_F23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_M.2_WWAN_UIM_SIM_DET
  {GPIO_SKL_LP_GPP_G0, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GNSS_RESET_N
  {GPIO_SKL_LP_GPP_G1, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_RST_N
  {GPIO_SKL_LP_GPP_G2, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_RST_N
  {GPIO_SKL_LP_GPP_G3, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_RESET_BB_PCH_N
  {GPIO_SKL_LP_GPP_G4, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_PWR_ON
  {GPIO_SKL_LP_GPP_G5, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,    GpioIntDis | GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//MODEM_RSVD_GPIO2
  {GPIO_SKL_LP_GPP_G6, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MODEM_CORE_DUMP
//  {GPIO_SKL_LP_GPP_G7, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_WWAN_WAKE_N -- NC in SKL Phablet
  {GPIO_SKL_LP_GPD0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PM_BATLOW_R_N
  {GPIO_SKL_LP_GPD1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//AC_PRESENT_R
  {GPIO_SKL_LP_GPD2, {GpioPadModeNative1, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSci, GpioDswReset,  GpioTermNone}},//LANWAKE_SMC_WAKE_SCI_N
  {GPIO_SKL_LP_GPD3, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,  GpioOutDefault, GpioIntBothEdge | GpioIntSci, GpioDswReset,  GpioTermWpu20K}},//PM_PWRBTN_R_N
  {GPIO_SKL_LP_GPD4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S3_R_N
  {GPIO_SKL_LP_GPD5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S4_R_N
  {GPIO_SKL_LP_GPD6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_M_R_N
  {GPIO_SKL_LP_GPD7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//USB_WAKEOUT_INTRUDET_N
  {GPIO_SKL_LP_GPD8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SUS_CLK
  {GPIO_SKL_LP_GPD9, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntDis, GpioDswReset,  GpioTermNone}},//USB32_LOAD_DETECT_N
  {GPIO_SKL_LP_GPD10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S5_R_N
  {GPIO_SKL_LP_GPD11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioDswReset,  GpioTermNone}},//USB_WALKUP_PWREN
};

static GPIO_INIT_CONFIG mGpioTablePhabletPocTouchpanel =
  {GPIO_SKL_LP_GPP_E7, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}};//TOUCH_PANEL_INTR_N_CMC

#endif
