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
#ifndef _GPIO_TABLE_COYOTE_MTN_H_
#define _GPIO_TABLE_COYOTE_MTN_H_

static GPIO_INIT_CONFIG mGpioTableCoyoteMtn[] =
{
  {GPIO_SKL_LP_GPP_G6, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//SSD_RST_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//H_RCIN_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//LPC_AD0_ESPI_IO0
//skip for eSPI function  {GPIO_SKL_LP_GPP_A2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//LPC_AD1_ESPI_IO1
//skip for eSPI function  {GPIO_SKL_LP_GPP_A3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//LPC_AD2_ESPI_IO2
//skip for eSPI function  {GPIO_SKL_LP_GPP_A4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//LPC_AD3_ESPI_IO3
//skip for eSPI function  {GPIO_SKL_LP_GPP_A5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPC_FRAME_ESPI_CS_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//INT_SERIRQ
//skip for eSPI function  {GPIO_SKL_LP_GPP_A7, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//PM_SLP_S0ix_R_N
  {GPIO_SKL_LP_GPP_A8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//PM_CLKRUN_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//LPC_CLK_ESPI_CLK
  {GPIO_SKL_LP_GPP_A10, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh, GpioIntDis, GpioResetDeep,  GpioTermNone}},//CAM_2_CLK_R
  {GPIO_SKL_LP_GPP_A11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioResetDeep,  GpioTermNone}},//EC_HID_INTR
//  {GPIO_SKL_LP_GPP_A12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
  {GPIO_SKL_LP_GPP_A13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SUS_PWR_ACK_R
//skip for eSPI function  {GPIO_SKL_LP_GPP_A14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//PM_SUS_STAT_ESPI_RST_N Native?
  {GPIO_SKL_LP_GPP_A15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//SUSACK_R_N
  {GPIO_SKL_LP_GPP_A16, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,   GpioOutDefault,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//VOLUMEUP_3P3_BTN_N
  {GPIO_SKL_LP_GPP_A17, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//VOLUMEDOWN_3P3_BTN_N
  {GPIO_SKL_LP_GPP_A18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//ACCEL_GYRO_INT_N
  {GPIO_SKL_LP_GPP_A19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//COMPASS_DRDY
  {GPIO_SKL_LP_GPP_A20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//ALS_INT_N
  {GPIO_SKL_LP_GPP_A21, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//MODEM_ISH_IRQ
  //{GPIO_SKL_LP_GPP_A22, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//MODEM_DISABLE_RADIO_N
  //{GPIO_SKL_LP_GPP_A23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SSD_PWREN
//  {GPIO_SKL_LP_GPP_B0, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_B1, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
  {GPIO_SKL_LP_GPP_B2, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//DP_X2MODE_N KKP is it input?
  {GPIO_SKL_LP_GPP_B3, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//WLAN_PERST_N
  {GPIO_SKL_LP_GPP_B4, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//BT_RFKILL_N
//  {GPIO_SKL_LP_GPP_B5, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
  // {GPIO_SKL_LP_GPP_B6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_B7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
  {GPIO_SKL_LP_GPP_B8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SSD_CLK_REQ_N
  {GPIO_SKL_LP_GPP_B9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//MDM_PCIE_CLKREQ_N
  {GPIO_SKL_LP_GPP_B10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//CLK_REQ_WLAN_N
  {GPIO_SKL_LP_GPP_B11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//MPHY_EXT_PWR_GATEB
  {GPIO_SKL_LP_GPP_B12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//PCH_SLP_S0_N
  {GPIO_SKL_LP_GPP_B13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//PLT_RST_N
  {GPIO_SKL_LP_GPP_B14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//GPP_B_14_SPKR might be native?
//  {GPIO_SKL_LP_GPP_B15, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_B16, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_B17, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_B18, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP for Grizzly Mtn
  {GPIO_SKL_LP_GPP_B19, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone  }},//TYPEC_SPI_CS_R_N for Coyote Mtn
  {GPIO_SKL_LP_GPP_B20, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone  }},//TYPEC_SPI_CLK_R  for Coyote Mtn
  {GPIO_SKL_LP_GPP_B21, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone  }},//TYPEC_SPI_MISO_R for Coyote Mtn
  {GPIO_SKL_LP_GPP_B22, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//TYPEC_SPI_MOSI_R for Coyote Mtn
//  {GPIO_SKL_LP_GPP_B23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntApic | GpioIntSci, GpioResetDeep,  GpioTermWpd20K}},//I2C_LID_BASE_EC_IRQ_R not connected
  {GPIO_SKL_LP_GPP_C0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SMB_CLK
  {GPIO_SKL_LP_GPP_C1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//SMB_DATA
//  {GPIO_SKL_LP_GPP_C2, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//NC for SKL phablet - TLS_CONF_PU
  {GPIO_SKL_LP_GPP_C3, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh, GpioIntDis, GpioResetDeep,  GpioTermNone}},//CODEC_RST_N
//  {GPIO_SKL_LP_GPP_C4, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP
 {GPIO_SKL_LP_GPP_C5, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone}},//MDM_PCIE_WAKE_N
  {GPIO_SKL_LP_GPP_C6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//I2C_LID_BASE_EC_CLK_R
  {GPIO_SKL_LP_GPP_C7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//I2C_LID_BASE_EC_DAT_R
  {GPIO_SKL_LP_GPP_C8, {GpioPadModeNative1,  GpioHostOwnGpio, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_UART0_RXD
  {GPIO_SKL_LP_GPP_C9, {GpioPadModeNative1,  GpioHostOwnGpio, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_UART0_TXD
  {GPIO_SKL_LP_GPP_C10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_UART0_RTS
  {GPIO_SKL_LP_GPP_C11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_UART0_CTS
  {GPIO_SKL_LP_GPP_C12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_GNSS_ISH_UART1_RXD
  {GPIO_SKL_LP_GPP_C13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_GNSS_ISH_UART1_TXD
  {GPIO_SKL_LP_GPP_C14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_GNSS_ISH_UART1_RTS_N
  {GPIO_SKL_LP_GPP_C15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_GNSS_ISH_UART1_CTS_N
  {GPIO_SKL_LP_GPP_C16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_MIPI_DIS_I2C0_SDA
  {GPIO_SKL_LP_GPP_C17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//LPSS_MIPI_DIS_I2C0_SCL
  {GPIO_SKL_LP_GPP_C18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SERIALIO_I2C1_SDA - TOUCH PNL I2C
  {GPIO_SKL_LP_GPP_C19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SERIALIO_I2C1_SCL - TOUCH PNL I2C
  {GPIO_SKL_LP_GPP_C20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_UART_RXD
  {GPIO_SKL_LP_GPP_C21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_UART_TXD
  {GPIO_SKL_LP_GPP_C22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_UART_RTS
  {GPIO_SKL_LP_GPP_C23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_UART_CTS
  {GPIO_SKL_LP_GPP_D0, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//MODEM_GSMPULSE
//  {GPIO_SKL_LP_GPP_D1, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_D2, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TOUCH_PANEL_INT_N
  {GPIO_SKL_LP_GPP_D3, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//MODEM_RST_OUT_N
//  {GPIO_SKL_LP_GPP_D4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP_PCH_CAM_FLASH_STROBE_OUT
  {GPIO_SKL_LP_GPP_D5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//ISH_I2C0_SDA
  {GPIO_SKL_LP_GPP_D6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//ISH_I2C0_SCL
  {GPIO_SKL_LP_GPP_D7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//ISH_SENSOR_I2C1_SDA
  {GPIO_SKL_LP_GPP_D8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//ISH_SENSOR_I2C1_SCL
  {GPIO_SKL_LP_GPP_D9, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioResetDeep,  GpioTermNone}},//MODEM_TXPWRTH
  {GPIO_SKL_LP_GPP_D10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//WIFI_BT_EN
//  {GPIO_SKL_LP_GPP_D11, {GpioPadModeNative1,    GpioHostOwnGpio, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_D12, {GpioPadModeNative1,    GpioHostOwnGpio, GpioDirNone,    GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP for Grizzly Mtn
  {GPIO_SKL_LP_GPP_D13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WLAN_MFUART_RXD
  {GPIO_SKL_LP_GPP_D14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WLAN_MFUART_TXD
  {GPIO_SKL_LP_GPP_D15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WLAN_MFUART_RTS
  {GPIO_SKL_LP_GPP_D16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WLAN_MFUART_CTS
  {GPIO_SKL_LP_GPP_D17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//DMIC_CLK_1 - DMIC
  {GPIO_SKL_LP_GPP_D18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//DMIC_DATA_1 - DMIC
  {GPIO_SKL_LP_GPP_D19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//DMIC_CLK_0 - DMIC - Top Mic's
  {GPIO_SKL_LP_GPP_D20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//DMIC_DATA_0 - DMIC - Top Mic's
  {GPIO_SKL_LP_GPP_D21, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_FM_ENABLE -- Disabled by default
  {GPIO_SKL_LP_GPP_D22, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioResetDeep,  GpioTermNone}},//MDM_PERST_N
  {GPIO_SKL_LP_GPP_D23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP_MCLK
  {GPIO_SKL_LP_GPP_E0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SATAXPCIE_0 - Will be configured by soft-trap in Phablet
  {GPIO_SKL_LP_GPP_E1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SATAXPCIE_1 - Will be configured by soft-trap in Phablet
  {GPIO_SKL_LP_GPP_E2, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SATAXPCIE_2 - Will be configured by soft-trap in Phablet
  {GPIO_SKL_LP_GPP_E3, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//CAM_PMIC_RST
  {GPIO_SKL_LP_GPP_E4, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis | GpioIntLevel | GpioIntApic, GpioResetDeep,  GpioTermNone}},//PCIE_WAKE_PCH_N
  {GPIO_SKL_LP_GPP_E5, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutLow, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP_GPP_E_5_SATA_DEVSLP_1
  {GPIO_SKL_LP_GPP_E6, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut, GpioOutLow, GpioIntDis, GpioResetDeep, GpioTermNone }},//MIPI_RESET_N
  {GPIO_SKL_LP_GPP_E8, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioResetDeep,  GpioTermNone}},//MIPI_CORE_PWREN2
  {GPIO_SKL_LP_GPP_E9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//USB_OC_0_WP1_OTG_N
  {GPIO_SKL_LP_GPP_E10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//USB_OC_1_WP4_N
  {GPIO_SKL_LP_GPP_E11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioResetDeep,  GpioTermNone}},//MIPI_IO_PWREN1
  {GPIO_SKL_LP_GPP_E12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioResetDeep,  GpioTermNone}},//DISP_PWR_EN
  {GPIO_SKL_LP_GPP_E13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//DDI1_HPD_Q
  {GPIO_SKL_LP_GPP_E14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//DDI2_HPD_Q
  {GPIO_SKL_LP_GPP_E15, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntSmi, GpioResetDeep,  GpioTermNone}},//SMC_EXTSMI_R_N
  {GPIO_SKL_LP_GPP_E16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioResetNormal,  GpioTermNone}},//SMC_RUNTIME_SCI_R_N
  {GPIO_SKL_LP_GPP_E17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//EDP_HPD
//  {GPIO_SKL_LP_GPP_E18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//DDI1_CTRL_CLK - NC in SKL Phablet
  {GPIO_SKL_LP_GPP_E19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//DDI1_CTRL_DATA - set by soft strap
//  {GPIO_SKL_LP_GPP_E20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//DDI2_CTRL_CLK - set by soft strap
  {GPIO_SKL_LP_GPP_E21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermWpd20K}},//DDI2_CTRL_DATA - NC in SKL Phablet
//  {GPIO_SKL_LP_GPP_E22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntApic, GpioResetDeep,  GpioTermNone}},//TP for Grizzly Mtn
  {GPIO_SKL_LP_GPP_E22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_CODEC_IRQ
  {GPIO_SKL_LP_GPP_E23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut, GpioOutLow, GpioIntDis, GpioResetDeep,  GpioTermNone}},//TOUCH_PNL_RST_N
  {GPIO_SKL_LP_GPP_F0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//I2S_BT_CLK
  {GPIO_SKL_LP_GPP_F1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//I2S_BT_FS
  {GPIO_SKL_LP_GPP_F2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//I2S_BT_TXD
  {GPIO_SKL_LP_GPP_F3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//I2S_BT_RXD
  {GPIO_SKL_LP_GPP_F4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SDA -- I2C for Camera/PMIC
  {GPIO_SKL_LP_GPP_F5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SCL -- I2C for Camera/PMIC
  {GPIO_SKL_LP_GPP_F6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SDA -- I2C for Capsense
  {GPIO_SKL_LP_GPP_F7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SCL -- I2C for Capsense
  {GPIO_SKL_LP_GPP_F8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTolerance1v8 | GpioTermNone}},//UF_CAM_I2C4_SDA
  {GPIO_SKL_LP_GPP_F9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTolerance1v8 | GpioTermNone}},//UF_CAM_I2C4_SCL
  {GPIO_SKL_LP_GPP_F10, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTolerance1v8 | GpioTermNone}},//WWAN_ISH_I2C2_SDA
  {GPIO_SKL_LP_GPP_F11, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep, GpioTolerance1v8 | GpioTermNone}},//WWAN_ISH_I2C2_SCL
  {GPIO_SKL_LP_GPP_F12, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//CAP_BTN_INT_N
  {GPIO_SKL_LP_GPP_F13, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//CAM_GPIO1
  {GPIO_SKL_LP_GPP_F14, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//CAM_GPIO2
  {GPIO_SKL_LP_GPP_F15, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//CAM_GPIO3
//  {GPIO_SKL_LP_GPP_F16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_F17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_F18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_F19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_F20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_F21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
//  {GPIO_SKL_LP_GPP_F22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetDeep,  GpioTermNone}},//NC for Grizzly Mtn
 // Commenting as this Interrupt doenot seem to be a Sharable interrupt ... Gives a strange error in Bios when configuring it as a Interrupt.
  {GPIO_SKL_LP_GPP_F23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioResetDeep,  GpioTermNone}},//SIM1_CD
  {GPIO_SKL_LP_GPP_G0, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//GNSS_RST_N
  {GPIO_SKL_LP_GPP_G1, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntDis | GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//BT_WAKEUP_HOST
  //{GPIO_SKL_LP_GPP_G2, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//TP_MODEM_RST_N
  {GPIO_SKL_LP_GPP_G3, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//MODEM_RESET_BB_PCH_N
  {GPIO_SKL_LP_GPP_G4, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//MODEM_PWR_ON
  {GPIO_SKL_LP_GPP_G5, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//TX_CUTBACK
  //{GPIO_SKL_LP_GPP_G6, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//SSD_RST_N
  {GPIO_SKL_LP_GPP_G7, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,    GpioIntDis, GpioResetDeep,  GpioTermNone}},//MODEM_CORE_DUMP
  {GPIO_SKL_LP_GPD0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//PM_BATLOW_R_N
  {GPIO_SKL_LP_GPD1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//AC_PRESENT_R
  {GPIO_SKL_LP_GPD2, {GpioPadModeNative1, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSci, GpioResetPwrGood,  GpioTermNone}},//LANWAKE_SMC_WAKE_SCI_N
  {GPIO_SKL_LP_GPD3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermWpu20K}},//PM_PWRBTN_R_N
  {GPIO_SKL_LP_GPD4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//SLP_S3_R_N
  {GPIO_SKL_LP_GPD5, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//SLP_S4_R_N
  {GPIO_SKL_LP_GPD6, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//SLP_M_R_N
  {GPIO_SKL_LP_GPD7, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//USB_WAKEOUT_INTRUDET_N
  {GPIO_SKL_LP_GPD8, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//SUS_CLK
  {GPIO_SKL_LP_GPD9, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//PCH_SLP_WLAN_N
  {GPIO_SKL_LP_GPD10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//SLP_S5_R_N
  {GPIO_SKL_LP_GPD11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResetPwrGood,  GpioTermNone}},//USB_WALKUP_PWREN
  {GPIO_SKL_LP_GPP_A23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh, GpioIntDis, GpioResetDeep,  GpioTermNone}},//SSD_PWREN
};

static GPIO_INIT_CONFIG mGpioTableCoyoteMtnTouchpanel =
  {GPIO_SKL_LP_GPP_D2, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntLevel | GpioIntApic, GpioResetDeep,  GpioTermNone}}; //TOUCH_PANEL_INT_N

#endif
