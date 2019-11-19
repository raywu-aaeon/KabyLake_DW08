/** @file
  GPIO definition table for Skylake A0 RVP3

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

@par Specification
**/
#ifndef _SKYLAKE_A0_RVP3_GPIO_TABLE_H_
#define _SKYLAKE_A0_RVP3_GPIO_TABLE_H_

#include <GpioPinsSklLp.h>
#include <Library/GpioLib.h>
#include <GpioConfig.h>
#include <Library/PeiPlatformHookLib.h>

#define END_OF_GPIO_TABLE 0xFFFFFFFF

GPIO_INIT_CONFIG mGpioTableLpDdr3Rvp3[] =
{
//skip for eSPI function  {GPIO_SKL_LP_GPP_A0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//H_RCIN_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD0_ESPI_IO0
//skip for eSPI function  {GPIO_SKL_LP_GPP_A2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD1_ESPI_IO1
//skip for eSPI function  {GPIO_SKL_LP_GPP_A3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD2_ESPI_IO2
//skip for eSPI function  {GPIO_SKL_LP_GPP_A4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD3_ESPI_IO3
//skip for eSPI function  {GPIO_SKL_LP_GPP_A5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//LPC_FRAME_ESPI_CS_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//INT_SERIRQ
  {GPIO_SKL_LP_GPP_A7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_SLP_S0ix_R_N
// skip for PM_CLKRUN_N {GPIO_SKL_LP_GPP_A8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_CLKRUN_N
//skip for eSPI function    {GPIO_SKL_LP_GPP_A9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_CLK_ESPI_CLK
// skip for PCH_CLK_PCI_TPM {GPIO_SKL_LP_GPP_A10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//PCH_CLK_PCI_TPM
  {GPIO_SKL_LP_GPP_A11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//EC_HID_INTR
  {GPIO_SKL_LP_GPP_A12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioResumeReset,  GpioTermNone}},//M.2_WWAN_GNSS_UART_RST_N
//skip for SUS_PWR_ACK_R  {GPIO_SKL_LP_GPP_A13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SUS_PWR_ACK_R
//skip for eSPI function    {GPIO_SKL_LP_GPP_A14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_SUS_STAT_ESPI_RST_N
//skip for SUSACK_R_N  {GPIO_SKL_LP_GPP_A15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SUSACK_R_N
  {GPIO_SKL_LP_GPP_A16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_1P8_SEL
  {GPIO_SKL_LP_GPP_A17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_PWR_EN_N
  {GPIO_SKL_LP_GPP_A18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_0_SENSOR
  {GPIO_SKL_LP_GPP_A19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_1_SENSOR
  {GPIO_SKL_LP_GPP_A20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_2_SENSOR
  {GPIO_SKL_LP_GPP_A21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GNSS_CHUB_IRQ
  {GPIO_SKL_LP_GPP_A22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//FPS_SLP_N
  {GPIO_SKL_LP_GPP_A23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//FPS_DRDY
  {GPIO_SKL_LP_GPP_B0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//V0.85A_VID0
  {GPIO_SKL_LP_GPP_B1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//V0.85A_VID1
  {GPIO_SKL_LP_GPP_B2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GP_VRALERTB
  {GPIO_SKL_LP_GPP_B3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioPlatformReset,  GpioTermNone}},//TCH_PAD_INTR_R_N
  {GPIO_SKL_LP_GPP_B4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_RF_KILL_N
  {GPIO_SKL_LP_GPP_B5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//M.2_BT_UART_WAKE_N
  // {GPIO_SKL_LP_GPP_B6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_SLOT1_N
  // {GPIO_SKL_LP_GPP_B7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_SLOT2_LAN_N
  // {GPIO_SKL_LP_GPP_B8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_SSD_SLOT3_N
  // {GPIO_SKL_LP_GPP_B9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_WIGIG_N
  // {GPIO_SKL_LP_GPP_B10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_WLAN_N
  {GPIO_SKL_LP_GPP_B11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MPHY_EXT_PWR_GATEB
  {GPIO_SKL_LP_GPP_B12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_SLP_S0_N
  {GPIO_SKL_LP_GPP_B13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PLT_RST_N
  {GPIO_SKL_LP_GPP_B14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//TCH_PNL_PWREN
  // {GPIO_SKL_LP_GPP_B15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_NFC_DFU, NOT OWNED BY BIOS
  {GPIO_SKL_LP_GPP_B16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}},//M.2_WLAN_WIFI_WAKE_N
  {GPIO_SKL_LP_GPP_B17, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntSci, GpioPlatformReset,  GpioTermWpu20K}},//TBT_CIO_PLUG_EVENT_N
  {GPIO_SKL_LP_GPP_B19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,   GpioTermNone}},//FPS_GSPI1_CS_R1_N
  {GPIO_SKL_LP_GPP_B20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_CLK_R1
  {GPIO_SKL_LP_GPP_B21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_MISO_R1
  {GPIO_SKL_LP_GPP_B22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_MOSI_R1
  {GPIO_SKL_LP_GPP_B23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DISCRETE_GNSS_RESET_N
  {GPIO_SKL_LP_GPP_C0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SMB_CLK
  {GPIO_SKL_LP_GPP_C1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SMB_DATA
  {GPIO_SKL_LP_GPP_C2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SKIN_THRM_SNSR_ALERT_N
  {GPIO_SKL_LP_GPP_C3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML0_CLK
  {GPIO_SKL_LP_GPP_C4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML0_DATA
  {GPIO_SKL_LP_GPP_C5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermWpd20K}},//M.2_WIGIG_WAKE_N
  {GPIO_SKL_LP_GPP_C6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML1_CLK, OWNED BY ME
  {GPIO_SKL_LP_GPP_C7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SML1_DATA, OWNED BY ME
  {GPIO_SKL_LP_GPP_C8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RXD
  {GPIO_SKL_LP_GPP_C9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_TXD
  {GPIO_SKL_LP_GPP_C10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RTS_N
  {GPIO_SKL_LP_GPP_C11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_CTS_N
  {GPIO_SKL_LP_GPP_C12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_RXD
  {GPIO_SKL_LP_GPP_C13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_TXD
  {GPIO_SKL_LP_GPP_C14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_RTS_N
  {GPIO_SKL_LP_GPP_C15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_CTS_N
  {GPIO_SKL_LP_GPP_C16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C0_SDA
  {GPIO_SKL_LP_GPP_C17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C0_SCL
  {GPIO_SKL_LP_GPP_C18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C1_SDA
  {GPIO_SKL_LP_GPP_C19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C1_SCL
  {GPIO_SKL_LP_GPP_C20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RXD
  {GPIO_SKL_LP_GPP_C21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_TXD
  {GPIO_SKL_LP_GPP_C22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RTS_N
  {GPIO_SKL_LP_GPP_C23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_CTS_N
  {GPIO_SKL_LP_GPP_D0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_CS_N
  {GPIO_SKL_LP_GPP_D1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_CLK
  {GPIO_SKL_LP_GPP_D2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_MISO
  {GPIO_SKL_LP_GPP_D3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_MOSI
  {GPIO_SKL_LP_GPP_D4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CSI2_FLASH_STROBE
  {GPIO_SKL_LP_GPP_D5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C0_SDA
  {GPIO_SKL_LP_GPP_D6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C0_SCL
  {GPIO_SKL_LP_GPP_D7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C1_SDA
  {GPIO_SKL_LP_GPP_D8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C1_SCL
  {GPIO_SKL_LP_GPP_D10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SCREEN_LOCK_PCH
  {GPIO_SKL_LP_GPP_D11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//VOL_UP_PCH
  {GPIO_SKL_LP_GPP_D12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//VOL_DOWN_PCH
  {GPIO_SKL_LP_GPP_D13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_RXD_SML0B_DATA
  {GPIO_SKL_LP_GPP_D14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_TXD_SML0B_CLK
  {GPIO_SKL_LP_GPP_D15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_RTS_N
  {GPIO_SKL_LP_GPP_D16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_CTS_SML0B_ALERT_N
  {GPIO_SKL_LP_GPP_D17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_CLK_1
  {GPIO_SKL_LP_GPP_D18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DMIC_DATA_1
  {GPIO_SKL_LP_GPP_D19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_CLK_0
  {GPIO_SKL_LP_GPP_D20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DMIC_DATA_0
  {GPIO_SKL_LP_GPP_D21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_IO2
  {GPIO_SKL_LP_GPP_D22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_IO3
  {GPIO_SKL_LP_GPP_D23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP_MCLK
  {GPIO_SKL_LP_GPP_E0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//SPI_TPM_HDR_IRQ_N
  {GPIO_SKL_LP_GPP_E1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA_ODD_PRSNT_N
  {GPIO_SKL_LP_GPP_E2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntLvlEdgDis | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//M.2_SSD_SATA2_PCIE3_DET_N
  {GPIO_SKL_LP_GPP_E3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResumeReset,  GpioTermNone}},//EINK_SSR_DFU_N
  {GPIO_SKL_LP_GPP_E4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_NFC_RESET
  {GPIO_SKL_LP_GPP_E5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA1_PHYSLP1_DIRECT_R
  // {GPIO_SKL_LP_GPP_E6,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA2_PHYSLP2_M.2SSD_R, NOT OWNED BY BIOS
  {GPIO_SKL_LP_GPP_E8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_SATA_LED_N
  {GPIO_SKL_LP_GPP_E9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_0_WP1_OTG_N
  {GPIO_SKL_LP_GPP_E10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_1_WP4_N
  {GPIO_SKL_LP_GPP_E11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_2_WP2_WP3_WP5_R_N
  // {GPIO_SKL_LP_GPP_E12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_NFC_IRQ, NOT OWNED BY BIOS
  {GPIO_SKL_LP_GPP_E13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI1_HPD_Q
  {GPIO_SKL_LP_GPP_E14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI2_HPD_Q
  {GPIO_SKL_LP_GPP_E15, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntSmi, GpioHostDeepReset,  GpioTermNone}},//SMC_EXTSMI_R_N
  {GPIO_SKL_LP_GPP_E16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}},//SMC_RUNTIME_SCI_R_N
  {GPIO_SKL_LP_GPP_E17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EDP_HPD
  {GPIO_SKL_LP_GPP_E18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI1_CTRL_CLK
  {GPIO_SKL_LP_GPP_E19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DDI1_CTRL_DATA
  {GPIO_SKL_LP_GPP_E20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI2_CTRL_CLK
  {GPIO_SKL_LP_GPP_E21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DDI2_CTRL_DATA
  {GPIO_SKL_LP_GPP_E22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_CODEC_IRQ
  {GPIO_SKL_LP_GPP_E23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//TCH_PNL_RST_N
  {GPIO_SKL_LP_GPP_F0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_SCLK
  {GPIO_SKL_LP_GPP_F1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_SFRM
  {GPIO_SKL_LP_GPP_F2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_TXD
  {GPIO_SKL_LP_GPP_F3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_RXD
  {GPIO_SKL_LP_GPP_F4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SDA
  {GPIO_SKL_LP_GPP_F5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SCL
  {GPIO_SKL_LP_GPP_F6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SDA
  {GPIO_SKL_LP_GPP_F7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SCL
  {GPIO_SKL_LP_GPP_F8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C4_SDA
  {GPIO_SKL_LP_GPP_F9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C4_SCL
  {GPIO_SKL_LP_GPP_F10, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C5_ISH_12C2_SDA
  {GPIO_SKL_LP_GPP_F11, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C5_ISH_12C2_SCL
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
  {GPIO_SKL_LP_GPP_F23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_M.2_WWAN_UIM_SIM_DET
  {GPIO_SKL_LP_GPP_G0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CMD
  {GPIO_SKL_LP_GPP_G1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA0
  {GPIO_SKL_LP_GPP_G2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA1
  {GPIO_SKL_LP_GPP_G3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA2
  {GPIO_SKL_LP_GPP_G4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA3
  {GPIO_SKL_LP_GPP_G5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CDB
  {GPIO_SKL_LP_GPP_G6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CLK
  {GPIO_SKL_LP_GPP_G7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_WP
  {GPIO_SKL_LP_GPD0,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PM_BATLOW_R_N
  {GPIO_SKL_LP_GPD1,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//AC_PRESENT_R
  {GPIO_SKL_LP_GPD2,   {GpioPadModeNative1, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSci, GpioDswReset,  GpioTermNone}},//LANWAKE_SMC_WAKE_SCI_N
  {GPIO_SKL_LP_GPD3,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermWpu20K}},//PM_PWRBTN_R_N
  {GPIO_SKL_LP_GPD4,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S3_R_N
  {GPIO_SKL_LP_GPD5,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S4_R_N
  {GPIO_SKL_LP_GPD6,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_M_R_N
  {GPIO_SKL_LP_GPD7,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//USB_WAKEOUT_INTRUDET_N
  {GPIO_SKL_LP_GPD8,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SUS_CLK
  {GPIO_SKL_LP_GPD9,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PCH_SLP_WLAN_N
  {GPIO_SKL_LP_GPD10,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S5_R_N
  {GPIO_SKL_LP_GPD11,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PM_LANPHY_ENABLE
  {END_OF_GPIO_TABLE,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};

UINT16 mGpioTableLpDdr3Rvp3Size = sizeof (mGpioTableLpDdr3Rvp3) / sizeof (GPIO_INIT_CONFIG) - 1;

GPIO_INIT_CONFIG mGpioTableSkyLakeYLpddr3Rvp3[] =
{
  { GPIO_SKL_LP_GPP_B18, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset, GpioTermWpu20K } },//PCH_SLOT1_WAKE_N
  { GPIO_SKL_LP_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset, GpioTermNone } },//HOME_BTN
};

UINT16 mGpioTableSkyLakeYLpddr3Rvp3Size = sizeof (mGpioTableSkyLakeYLpddr3Rvp3) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTableKabyLakeYLpddr3Rvp3[] =
{
  { GPIO_SKL_LP_GPP_A12, { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis, GpioResumeReset, GpioTermNone } },//REALSENSE_ISH_WAKE
  { GPIO_SKL_LP_GPP_A20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone } },//IRIS_PROXI_INTR
  { GPIO_SKL_LP_GPP_D10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,   GpioOutDefault, GpioIntEdge | GpioIntApic, GpioHostDeepReset, GpioTermNone } },//SD_CARD_WAKE
  { GPIO_SKL_LP_GPP_D11, { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone } },//TYPEC_P1_DCI_CLK
  { GPIO_SKL_LP_GPP_D12, { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone, GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone } },//TYPEC_P1_DCI_DATA
  { GPIO_SKL_LP_GPP_B15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,  GpioOutHigh,    GpioIntDis, GpioPlatformReset, GpioTermNone } },//TBT CIO POWER EN/ RTD3 Power En
};

UINT16 mGpioTableKabyLakeYLpddr3Rvp3Size = sizeof (mGpioTableKabyLakeYLpddr3Rvp3) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTableKabyLakeYLpDdr3Rvp3PcieSlot1[] =
{
  { GPIO_SKL_LP_GPP_B18, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset, GpioTermWpu20K } }, //PCH_SLOT1_WAKE_N
  { GPIO_SKL_LP_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,                GpioResumeReset,   GpioTermNone} },    //M.2_WWAN_GNSS_UART_RST_N
};

UINT16 mGpioTableKabyLakeYLpDdr3Rvp3PcieSlot1Size = sizeof (mGpioTableKabyLakeYLpDdr3Rvp3PcieSlot1) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTableKabyLakeYLpDdr3Rvp3PcieWwan[] =
{
  { GPIO_SKL_LP_GPP_B18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset, GpioTermWpu20K } }, //PCH_SLOT1_WAKE_N
  { GPIO_SKL_LP_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,                GpioResumeReset,   GpioTermNone} },   //M.2_WWAN_GNSS_UART_RST_N
};

UINT16 mGpioTableKabyLakeYLpDdr3Rvp3PcieWwanSize = sizeof (mGpioTableKabyLakeYLpDdr3Rvp3PcieWwan) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTable1AmberLakeY42LpDdr3Rvp3[] =
{
//skip for eSPI function  {GPIO_SKL_LP_GPP_A0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//H_RCIN_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD0_ESPI_IO0
//skip for eSPI function  {GPIO_SKL_LP_GPP_A2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD1_ESPI_IO1
//skip for eSPI function  {GPIO_SKL_LP_GPP_A3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD2_ESPI_IO2
//skip for eSPI function  {GPIO_SKL_LP_GPP_A4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD3_ESPI_IO3
//skip for eSPI function  {GPIO_SKL_LP_GPP_A5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//LPC_FRAME_ESPI_CS_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//INT_SERIRQ
  {GPIO_SKL_LP_GPP_A7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_SLP_S0ix_R_N
// skip for PM_CLKRUN_N {GPIO_SKL_LP_GPP_A8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_CLKRUN_N
//skip for eSPI function    {GPIO_SKL_LP_GPP_A9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_CLK_ESPI_CLK
// skip for PCH_CLK_PCI_TPM {GPIO_SKL_LP_GPP_A10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//PCH_CLK_PCI_TPM
  {GPIO_SKL_LP_GPP_A11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//EC_HID_INTR
// Skip For AML Y42  {GPIO_SKL_LP_GPP_A12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioResumeReset,  GpioTermNone}},//M.2_WWAN_GNSS_UART_RST_N
//skip for SUS_PWR_ACK_R  {GPIO_SKL_LP_GPP_A13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SUS_PWR_ACK_R
//skip for eSPI function    {GPIO_SKL_LP_GPP_A14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_SUS_STAT_ESPI_RST_N
//skip for SUSACK_R_N  {GPIO_SKL_LP_GPP_A15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SUSACK_R_N
  {GPIO_SKL_LP_GPP_A16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_1P8_SEL
  {GPIO_SKL_LP_GPP_A17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_PWR_EN_N
  {GPIO_SKL_LP_GPP_A18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_0_SENSOR
  {GPIO_SKL_LP_GPP_A19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_1_SENSOR
// Skip For AML Y42  {GPIO_SKL_LP_GPP_A20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_2_SENSOR
  {GPIO_SKL_LP_GPP_A21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GNSS_CHUB_IRQ
  {GPIO_SKL_LP_GPP_A22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//FPS_SLP_N
  {GPIO_SKL_LP_GPP_A23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//FPS_DRDY
  {GPIO_SKL_LP_GPP_B0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//V0.85A_VID0
  {GPIO_SKL_LP_GPP_B1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//V0.85A_VID1
  {GPIO_SKL_LP_GPP_B2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GP_VRALERTB
  {GPIO_SKL_LP_GPP_B3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioPlatformReset,  GpioTermNone}},//TCH_PAD_INTR_R_N
  {GPIO_SKL_LP_GPP_B4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_RF_KILL_N
  {GPIO_SKL_LP_GPP_B5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//M.2_BT_UART_WAKE_N
  // {GPIO_SKL_LP_GPP_B6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_SLOT1_N
  // {GPIO_SKL_LP_GPP_B7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_SLOT2_LAN_N
  // {GPIO_SKL_LP_GPP_B8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_SSD_SLOT3_N
  // {GPIO_SKL_LP_GPP_B9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_WIGIG_N
  // {GPIO_SKL_LP_GPP_B10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_WLAN_N
  {GPIO_SKL_LP_GPP_B11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MPHY_EXT_PWR_GATEB
  {GPIO_SKL_LP_GPP_B12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_SLP_S0_N
  {GPIO_SKL_LP_GPP_B13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PLT_RST_N
  {GPIO_SKL_LP_GPP_B14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//TCH_PNL_PWREN
  // {GPIO_SKL_LP_GPP_B15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_NFC_DFU, NOT OWNED BY BIOS
  {GPIO_SKL_LP_GPP_B16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}},//M.2_WLAN_WIFI_WAKE_N
  {GPIO_SKL_LP_GPP_B17, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntSci, GpioPlatformReset,  GpioTermWpu20K}},//TBT_CIO_PLUG_EVENT_N
  {GPIO_SKL_LP_GPP_B19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,   GpioTermNone}},//FPS_GSPI1_CS_R1_N
  {GPIO_SKL_LP_GPP_B20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_CLK_R1
  {GPIO_SKL_LP_GPP_B21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_MISO_R1
  {GPIO_SKL_LP_GPP_B22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_MOSI_R1
  {GPIO_SKL_LP_GPP_B23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DISCRETE_GNSS_RESET_N
  {GPIO_SKL_LP_GPP_C0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SMB_CLK
  {GPIO_SKL_LP_GPP_C1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SMB_DATA
  {GPIO_SKL_LP_GPP_C2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SKIN_THRM_SNSR_ALERT_N
  {GPIO_SKL_LP_GPP_C3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML0_CLK
  {GPIO_SKL_LP_GPP_C4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML0_DATA
  {GPIO_SKL_LP_GPP_C5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermWpd20K}},//M.2_WIGIG_WAKE_N
  {GPIO_SKL_LP_GPP_C6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML1_CLK, OWNED BY ME
  {GPIO_SKL_LP_GPP_C7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SML1_DATA, OWNED BY ME
  {GPIO_SKL_LP_GPP_C8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RXD
  {GPIO_SKL_LP_GPP_C9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_TXD
  {GPIO_SKL_LP_GPP_C10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RTS_N
  {GPIO_SKL_LP_GPP_C11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_CTS_N
// Skip For AML Y42  {GPIO_SKL_LP_GPP_C12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_RXD
// Skip For AML Y42  {GPIO_SKL_LP_GPP_C13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_TXD
// Skip For AML Y42  {GPIO_SKL_LP_GPP_C14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_RTS_N
// Skip For AML Y42  {GPIO_SKL_LP_GPP_C15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_CTS_N
  {GPIO_SKL_LP_GPP_C16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C0_SDA
  {GPIO_SKL_LP_GPP_C17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C0_SCL
  {GPIO_SKL_LP_GPP_C18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C1_SDA
  {GPIO_SKL_LP_GPP_C19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C1_SCL
  {GPIO_SKL_LP_GPP_C20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RXD
  {GPIO_SKL_LP_GPP_C21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_TXD
  {GPIO_SKL_LP_GPP_C22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RTS_N
  {GPIO_SKL_LP_GPP_C23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_CTS_N
  {GPIO_SKL_LP_GPP_D0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_CS_N
  {GPIO_SKL_LP_GPP_D1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_CLK
  {GPIO_SKL_LP_GPP_D2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_MISO
  {GPIO_SKL_LP_GPP_D3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_MOSI
  {GPIO_SKL_LP_GPP_D4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CSI2_FLASH_STROBE
  {GPIO_SKL_LP_GPP_D5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C0_SDA
  {GPIO_SKL_LP_GPP_D6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C0_SCL
  {GPIO_SKL_LP_GPP_D7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C1_SDA
  {GPIO_SKL_LP_GPP_D8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C1_SCL
// Skip For AML Y42  {GPIO_SKL_LP_GPP_D10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SCREEN_LOCK_PCH
// Skip For AML Y42  {GPIO_SKL_LP_GPP_D11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//VOL_UP_PCH
// Skip For AML Y42  {GPIO_SKL_LP_GPP_D12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//VOL_DOWN_PCH
  {GPIO_SKL_LP_GPP_D13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_RXD_SML0B_DATA
  {GPIO_SKL_LP_GPP_D14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_TXD_SML0B_CLK
  {GPIO_SKL_LP_GPP_D15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_RTS_N
  {GPIO_SKL_LP_GPP_D16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_CTS_SML0B_ALERT_N
  {GPIO_SKL_LP_GPP_D17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_CLK_1
  {GPIO_SKL_LP_GPP_D18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DMIC_DATA_1
  {GPIO_SKL_LP_GPP_D19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_CLK_0
  {GPIO_SKL_LP_GPP_D20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DMIC_DATA_0
  {GPIO_SKL_LP_GPP_D21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_IO2
  {GPIO_SKL_LP_GPP_D22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_IO3
  {GPIO_SKL_LP_GPP_D23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP_MCLK
  {GPIO_SKL_LP_GPP_E0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//SPI_TPM_HDR_IRQ_N
  {GPIO_SKL_LP_GPP_E1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA_ODD_PRSNT_N
  {GPIO_SKL_LP_GPP_E2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntLvlEdgDis | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//M.2_SSD_SATA2_PCIE3_DET_N
  {GPIO_SKL_LP_GPP_E3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResumeReset,  GpioTermNone}},//EINK_SSR_DFU_N
  {GPIO_SKL_LP_GPP_E4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_NFC_RESET
  {GPIO_SKL_LP_GPP_E5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA1_PHYSLP1_DIRECT_R
  // {GPIO_SKL_LP_GPP_E6,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA2_PHYSLP2_M.2SSD_R, NOT OWNED BY BIOS
  {GPIO_SKL_LP_GPP_E8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_SATA_LED_N
  {GPIO_SKL_LP_GPP_E9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_0_WP1_OTG_N
  {GPIO_SKL_LP_GPP_E10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_1_WP4_N
  {GPIO_SKL_LP_GPP_E11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_2_WP2_WP3_WP5_R_N
  // {GPIO_SKL_LP_GPP_E12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_NFC_IRQ, NOT OWNED BY BIOS
  {GPIO_SKL_LP_GPP_E13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI1_HPD_Q
  {GPIO_SKL_LP_GPP_E14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI2_HPD_Q
  {GPIO_SKL_LP_GPP_E15, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntSmi, GpioHostDeepReset,  GpioTermNone}},//SMC_EXTSMI_R_N
  {GPIO_SKL_LP_GPP_E16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}},//SMC_RUNTIME_SCI_R_N
  {GPIO_SKL_LP_GPP_E17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EDP_HPD
  {GPIO_SKL_LP_GPP_E18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI1_CTRL_CLK
  {GPIO_SKL_LP_GPP_E19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DDI1_CTRL_DATA
  {GPIO_SKL_LP_GPP_E20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI2_CTRL_CLK
  {GPIO_SKL_LP_GPP_E21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DDI2_CTRL_DATA
  {GPIO_SKL_LP_GPP_E22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_CODEC_IRQ
  {GPIO_SKL_LP_GPP_E23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//TCH_PNL_RST_N
  {GPIO_SKL_LP_GPP_F0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_SCLK
  {GPIO_SKL_LP_GPP_F1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_SFRM
  {GPIO_SKL_LP_GPP_F2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_TXD
  {GPIO_SKL_LP_GPP_F3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_RXD
  {GPIO_SKL_LP_GPP_F4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SDA
  {GPIO_SKL_LP_GPP_F5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SCL
  {GPIO_SKL_LP_GPP_F6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SDA
  {GPIO_SKL_LP_GPP_F7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SCL
  {GPIO_SKL_LP_GPP_F8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C4_SDA
  {GPIO_SKL_LP_GPP_F9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C4_SCL
// Skip For AML Y42  {GPIO_SKL_LP_GPP_F10, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C5_ISH_12C2_SDA
// Skip For AML Y42  {GPIO_SKL_LP_GPP_F11, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C5_ISH_12C2_SCL
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
  {GPIO_SKL_LP_GPP_F23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_M.2_WWAN_UIM_SIM_DET
  {GPIO_SKL_LP_GPP_G0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CMD
  {GPIO_SKL_LP_GPP_G1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA0
  {GPIO_SKL_LP_GPP_G2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA1
  {GPIO_SKL_LP_GPP_G3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA2
  {GPIO_SKL_LP_GPP_G4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA3
  {GPIO_SKL_LP_GPP_G5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CDB
  {GPIO_SKL_LP_GPP_G6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CLK
  {GPIO_SKL_LP_GPP_G7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_WP
  {GPIO_SKL_LP_GPD0,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PM_BATLOW_R_N
  {GPIO_SKL_LP_GPD1,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//AC_PRESENT_R
  {GPIO_SKL_LP_GPD2,   {GpioPadModeNative1, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSci, GpioDswReset,  GpioTermNone}},//LANWAKE_SMC_WAKE_SCI_N
  {GPIO_SKL_LP_GPD3,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermWpu20K}},//PM_PWRBTN_R_N
  {GPIO_SKL_LP_GPD4,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S3_R_N
  {GPIO_SKL_LP_GPD5,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S4_R_N
  {GPIO_SKL_LP_GPD6,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_M_R_N
  {GPIO_SKL_LP_GPD7,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//USB_WAKEOUT_INTRUDET_N
  {GPIO_SKL_LP_GPD8,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SUS_CLK
  {GPIO_SKL_LP_GPD9,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PCH_SLP_WLAN_N
  {GPIO_SKL_LP_GPD10,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S5_R_N
  {GPIO_SKL_LP_GPD11,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PM_LANPHY_ENABLE
  {END_OF_GPIO_TABLE,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};

UINT16 mGpioTable1AmberLakeY42LpDdr3Rvp3Size = sizeof (mGpioTable1AmberLakeY42LpDdr3Rvp3) / sizeof (GPIO_INIT_CONFIG) - 1;

GPIO_INIT_CONFIG mGpioTable2AmberLakeY42LpDdr3Rvp3[] =
{
  {GPIO_SKL_LP_GPP_A12, { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioResumeReset,   GpioTermNone }}, //REALSENSE_ISH_WAKE
  {GPIO_SKL_LP_GPP_A20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone }}, //IRIS_PROXI_INTR
  {GPIO_SKL_LP_GPP_B15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioPlatformReset, GpioTermNone }}, //TBT CIO POWER EN/ RTD3 Power En
  {GPIO_SKL_LP_GPP_B18, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioHostDeepReset,  GpioTermWpu20K }}, //PCH_SLOT1_WAKE_N
  {GPIO_SKL_LP_GPP_C12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,                GpioHostDeepReset,  GpioTermNone   }}, //PCH_SLOT1_RST_N
  {GPIO_SKL_LP_GPP_C13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,                GpioHostDeepReset,  GpioTermNone   }}, //M.2_WWAN_PER_RST_N
  {GPIO_SKL_LP_GPP_C14, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioHostDeepReset,  GpioTermWpu20K }}, //WWAN_WAKE
  {GPIO_SKL_LP_GPP_C15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,                GpioHostDeepReset,  GpioTermNone   }}, //WWAN_WAKE_CTRL
  {GPIO_SKL_LP_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,                GpioResumeReset,    GpioTermNone   }}, //M.2_WWAN_GNSS_UART_RST_N
  {GPIO_SKL_LP_GPP_D10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntEdge | GpioIntApic, GpioHostDeepReset,  GpioTermNone   }}, //SD_CARD_WAKE
  {GPIO_SKL_LP_GPP_F10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}}, //M.2_WIFI_RST_R_N
  {GPIO_SKL_LP_GPP_F11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}}, //M.2_SSD_RST_R_N
};

UINT16 mGpioTable2AmberLakeY42LpDdr3Rvp3Size = sizeof (mGpioTable2AmberLakeY42LpDdr3Rvp3) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTableAmberLakeY42LpDdr3Rvp3PcieWwan[] =
{
  {GPIO_SKL_LP_GPP_C13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,                GpioHostDeepReset,  GpioTermNone   }}, //M.2_WWAN_PER_RST_N
  {GPIO_SKL_LP_GPP_C14, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioHostDeepReset,  GpioTermWpu20K }}, //WWAN_WAKE
  {GPIO_SKL_LP_GPP_C15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,                GpioHostDeepReset,  GpioTermNone   }}, //WWAN_WAKE_CTRL
  {GPIO_SKL_LP_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,                GpioResumeReset,    GpioTermNone   }}, //M.2_WWAN_GNSS_UART_RST_N
};

UINT16 mGpioTableAmberLakeY42LpDdr3Rvp3PcieWwanSize = sizeof (mGpioTableAmberLakeY42LpDdr3Rvp3PcieWwan) / sizeof (GPIO_INIT_CONFIG);

// Gpio Table for AML Y42 AEP

GPIO_INIT_CONFIG mGpioTable1AmberLakeY42LpDdr3Cff[] =
{
//skip for eSPI function  {GPIO_SKL_LP_GPP_A0, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//H_RCIN_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A1, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD0_ESPI_IO0
//skip for eSPI function  {GPIO_SKL_LP_GPP_A2, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD1_ESPI_IO1
//skip for eSPI function  {GPIO_SKL_LP_GPP_A3, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD2_ESPI_IO2
//skip for eSPI function  {GPIO_SKL_LP_GPP_A4, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_AD3_ESPI_IO3
//skip for eSPI function  {GPIO_SKL_LP_GPP_A5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//LPC_FRAME_ESPI_CS_N
//skip for eSPI function  {GPIO_SKL_LP_GPP_A6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//INT_SERIRQ
//{GPIO_SKL_LP_GPP_A7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_SLP_S0ix_R_N
// skip for PM_CLKRUN_N {GPIO_SKL_LP_GPP_A8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_CLKRUN_N
//skip for eSPI function    {GPIO_SKL_LP_GPP_A9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//LPC_CLK_ESPI_CLK
// skip for PCH_CLK_PCI_TPM {GPIO_SKL_LP_GPP_A10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//PCH_CLK_PCI_TPM
//{GPIO_SKL_LP_GPP_A11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//EC_HID_INTR
// Skip For AML Y42  {GPIO_SKL_LP_GPP_A12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis, GpioResumeReset,  GpioTermNone}},//M.2_WWAN_GNSS_UART_RST_N
//skip for SUS_PWR_ACK_R  {GPIO_SKL_LP_GPP_A13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SUS_PWR_ACK_R
//skip for eSPI function    {GPIO_SKL_LP_GPP_A14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PM_SUS_STAT_ESPI_RST_N
//skip for SUSACK_R_N  {GPIO_SKL_LP_GPP_A15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SUSACK_R_N
//  {GPIO_SKL_LP_GPP_A16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_1P8_SEL
//  {GPIO_SKL_LP_GPP_A17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_PWR_EN_N
// {GPIO_SKL_LP_GPP_A18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_0_SENSOR
//  {GPIO_SKL_LP_GPP_A19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_1_SENSOR
// Skip For AML Y42  {GPIO_SKL_LP_GPP_A20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_GP_2_SENSOR
//  {GPIO_SKL_LP_GPP_A21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GNSS_CHUB_IRQ
//  {GPIO_SKL_LP_GPP_A22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//FPS_SLP_N
//  {GPIO_SKL_LP_GPP_A23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//FPS_DRDY
//{GPIO_SKL_LP_GPP_B0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//V0.85A_VID0
//{GPIO_SKL_LP_GPP_B1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//V0.85A_VID1
//{GPIO_SKL_LP_GPP_B2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//GP_VRALERTB
//{GPIO_SKL_LP_GPP_B3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioPlatformReset,  GpioTermNone}},//TCH_PAD_INTR_R_N
  {GPIO_SKL_LP_GPP_B4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//BT_RF_KILL_N
//{GPIO_SKL_LP_GPP_B5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//M.2_BT_UART_WAKE_N
  // {GPIO_SKL_LP_GPP_B6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_SLOT1_N
  // {GPIO_SKL_LP_GPP_B7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_SLOT2_LAN_N
  // {GPIO_SKL_LP_GPP_B8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_SSD_SLOT3_N
  // {GPIO_SKL_LP_GPP_B9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_WIGIG_N
  // {GPIO_SKL_LP_GPP_B10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CLK_REQ_M.2_WLAN_N
  {GPIO_SKL_LP_GPP_B11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//MPHY_EXT_PWR_GATEB
  {GPIO_SKL_LP_GPP_B12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_SLP_S0_N
  {GPIO_SKL_LP_GPP_B13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PLT_RST_N
//  {GPIO_SKL_LP_GPP_B14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//TCH_PNL_PWREN
  // {GPIO_SKL_LP_GPP_B15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_NFC_DFU, NOT OWNED BY BIOS
//  {GPIO_SKL_LP_GPP_B16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}},//M.2_WLAN_WIFI_WAKE_N
//  {GPIO_SKL_LP_GPP_B17, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntSci, GpioPlatformReset,  GpioTermWpu20K}},//TBT_CIO_PLUG_EVENT_N
  {GPIO_SKL_LP_GPP_B19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,   GpioTermNone}},//FPS_GSPI1_CS_R1_N
  {GPIO_SKL_LP_GPP_B20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_CLK_R1
  {GPIO_SKL_LP_GPP_B21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_MISO_R1
  {GPIO_SKL_LP_GPP_B22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//FPS_GSPI1_MOSI_R1
//{GPIO_SKL_LP_GPP_B23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DISCRETE_GNSS_RESET_N
  {GPIO_SKL_LP_GPP_C0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SMB_CLK
  {GPIO_SKL_LP_GPP_C1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SMB_DATA
//{GPIO_SKL_LP_GPP_C2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SKIN_THRM_SNSR_ALERT_N
//{GPIO_SKL_LP_GPP_C3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML0_CLK
//  {GPIO_SKL_LP_GPP_C4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML0_DATA
//{GPIO_SKL_LP_GPP_C5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermWpd20K}},//M.2_WIGIG_WAKE_N
//  {GPIO_SKL_LP_GPP_C6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SML1_CLK, OWNED BY ME
//{GPIO_SKL_LP_GPP_C7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//SML1_DATA, OWNED BY ME
//  {GPIO_SKL_LP_GPP_C8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RXD
//  {GPIO_SKL_LP_GPP_C9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_TXD
//  {GPIO_SKL_LP_GPP_C10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_RTS_N
//  {GPIO_SKL_LP_GPP_C11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART0_CTS_N
// Skip For AML Y42  {GPIO_SKL_LP_GPP_C12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_RXD
// Skip For AML Y42  {GPIO_SKL_LP_GPP_C13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_TXD
// Skip For AML Y42  {GPIO_SKL_LP_GPP_C14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_RTS_N
// Skip For AML Y42  {GPIO_SKL_LP_GPP_C15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART1_ISH_UART1_CTS_N
  {GPIO_SKL_LP_GPP_C16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C0_SDA
  {GPIO_SKL_LP_GPP_C17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C0_SCL
  {GPIO_SKL_LP_GPP_C18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C1_SDA
  {GPIO_SKL_LP_GPP_C19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_I2C1_SCL
  {GPIO_SKL_LP_GPP_C20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RXD
  {GPIO_SKL_LP_GPP_C21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_TXD
//  {GPIO_SKL_LP_GPP_C22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_RTS_N
//{GPIO_SKL_LP_GPP_C23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SERIALIO_UART2_CTS_N
  {GPIO_SKL_LP_GPP_D0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_CS_N
  {GPIO_SKL_LP_GPP_D1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_CLK
  {GPIO_SKL_LP_GPP_D2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_MISO
  {GPIO_SKL_LP_GPP_D3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_MOSI
//{GPIO_SKL_LP_GPP_D4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//CSI2_FLASH_STROBE
  {GPIO_SKL_LP_GPP_D5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C0_SDA
  {GPIO_SKL_LP_GPP_D6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C0_SCL
//  {GPIO_SKL_LP_GPP_D7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C1_SDA
//  {GPIO_SKL_LP_GPP_D8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_I2C1_SCL
// Skip For AML Y42  {GPIO_SKL_LP_GPP_D10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SCREEN_LOCK_PCH
// Skip For AML Y42  {GPIO_SKL_LP_GPP_D11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//VOL_UP_PCH
// Skip For AML Y42  {GPIO_SKL_LP_GPP_D12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//VOL_DOWN_PCH
//{GPIO_SKL_LP_GPP_D13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_RXD_SML0B_DATA
//{GPIO_SKL_LP_GPP_D14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_TXD_SML0B_CLK
//{GPIO_SKL_LP_GPP_D15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_RTS_N
//{GPIO_SKL_LP_GPP_D16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//ISH_UART0_CTS_SML0B_ALERT_N
//{GPIO_SKL_LP_GPP_D17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_CLK_1
//{GPIO_SKL_LP_GPP_D18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DMIC_DATA_1
  {GPIO_SKL_LP_GPP_D19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DMIC_CLK_0
  {GPIO_SKL_LP_GPP_D20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DMIC_DATA_0
  {GPIO_SKL_LP_GPP_D21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_IO2
  {GPIO_SKL_LP_GPP_D22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPI1_TCHPNL_IO3
//{GPIO_SKL_LP_GPP_D23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP_MCLK
//  {GPIO_SKL_LP_GPP_E0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//SPI_TPM_HDR_IRQ_N
  {GPIO_SKL_LP_GPP_E1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA_ODD_PRSNT_N
//  {GPIO_SKL_LP_GPP_E2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntLvlEdgDis | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//M.2_SSD_SATA2_PCIE3_DET_N
//  {GPIO_SKL_LP_GPP_E3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioResumeReset,  GpioTermNone}},//EINK_SSR_DFU_N
//  {GPIO_SKL_LP_GPP_E4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_NFC_RESET
  {GPIO_SKL_LP_GPP_E5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA1_PHYSLP1_DIRECT_R
  // {GPIO_SKL_LP_GPP_E6,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SATA2_PHYSLP2_M.2SSD_R, NOT OWNED BY BIOS
//  {GPIO_SKL_LP_GPP_E8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//PCH_SATA_LED_N
  {GPIO_SKL_LP_GPP_E9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_0_WP1_OTG_N
  {GPIO_SKL_LP_GPP_E10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_1_WP4_N
  {GPIO_SKL_LP_GPP_E11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_2_WP2_WP3_WP5_R_N
  {GPIO_SKL_LP_GPP_E12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//USB_OC_3
  {GPIO_SKL_LP_GPP_E13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI1_HPD_Q
  {GPIO_SKL_LP_GPP_E14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI2_HPD_Q
//  {GPIO_SKL_LP_GPP_E15, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge | GpioIntSmi, GpioHostDeepReset,  GpioTermNone}},//SMC_EXTSMI_R_N
//{GPIO_SKL_LP_GPP_E16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}},//SMC_RUNTIME_SCI_R_N
  {GPIO_SKL_LP_GPP_E17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EDP_HPD
  {GPIO_SKL_LP_GPP_E18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI1_CTRL_CLK
  {GPIO_SKL_LP_GPP_E19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DDI1_CTRL_DATA
  {GPIO_SKL_LP_GPP_E20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//DDI2_CTRL_CLK
  {GPIO_SKL_LP_GPP_E21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//DDI2_CTRL_DATA
//{GPIO_SKL_LP_GPP_E22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_CODEC_IRQ
//{GPIO_SKL_LP_GPP_E23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//TCH_PNL_RST_N
//{GPIO_SKL_LP_GPP_F0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_SCLK
//{GPIO_SKL_LP_GPP_F1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_SFRM
//{GPIO_SKL_LP_GPP_F2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_TXD
//{GPIO_SKL_LP_GPP_F3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SSP2_RXD
//{GPIO_SKL_LP_GPP_F4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SDA
//{GPIO_SKL_LP_GPP_F5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C2_SCL
  {GPIO_SKL_LP_GPP_F6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SDA
  {GPIO_SKL_LP_GPP_F7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C3_SCL
//{GPIO_SKL_LP_GPP_F8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C4_SDA
//{GPIO_SKL_LP_GPP_F9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C4_SCL
// Skip For AML Y42  {GPIO_SKL_LP_GPP_F10, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C5_ISH_12C2_SDA
// Skip For AML Y42  {GPIO_SKL_LP_GPP_F11, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//SERIALIO_I2C5_ISH_12C2_SCL
//{GPIO_SKL_LP_GPP_F12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_CMD
//{GPIO_SKL_LP_GPP_F13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA0
//{GPIO_SKL_LP_GPP_F14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA1
//{GPIO_SKL_LP_GPP_F15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA2
//{GPIO_SKL_LP_GPP_F16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA3
//{GPIO_SKL_LP_GPP_F17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA4
//{GPIO_SKL_LP_GPP_F18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA5
//{GPIO_SKL_LP_GPP_F19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA6
//{GPIO_SKL_LP_GPP_F20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_DATA7
//{GPIO_SKL_LP_GPP_F21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_RCLK
//{GPIO_SKL_LP_GPP_F22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//EMMC_CLK
//{GPIO_SKL_LP_GPP_F23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//PCH_M.2_WWAN_UIM_SIM_DET
//  {GPIO_SKL_LP_GPP_G0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CMD
//{GPIO_SKL_LP_GPP_G1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA0
//{GPIO_SKL_LP_GPP_G2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA1
//{GPIO_SKL_LP_GPP_G3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA2
//{GPIO_SKL_LP_GPP_G4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_DATA3
//{GPIO_SKL_LP_GPP_G5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CDB
//{GPIO_SKL_LP_GPP_G6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_CLK
//{GPIO_SKL_LP_GPP_G7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_WP
  {GPIO_SKL_LP_GPD0,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PM_BATLOW_R_N
  {GPIO_SKL_LP_GPD1,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//AC_PRESENT_R
  {GPIO_SKL_LP_GPD2,   {GpioPadModeNative1, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSci, GpioDswReset,  GpioTermNone}},//LANWAKE_SMC_WAKE_SCI_N
  {GPIO_SKL_LP_GPD3,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermWpu20K}},//PM_PWRBTN_R_N
  {GPIO_SKL_LP_GPD4,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S3_R_N
  {GPIO_SKL_LP_GPD5,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S4_R_N
  {GPIO_SKL_LP_GPD6,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_M_R_N
  {GPIO_SKL_LP_GPD7,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//USB_WAKEOUT_INTRUDET_N
  {GPIO_SKL_LP_GPD8,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SUS_CLK
//{GPIO_SKL_LP_GPD9,   {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PCH_SLP_WLAN_N
  {GPIO_SKL_LP_GPD10,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//SLP_S5_R_N
//{GPIO_SKL_LP_GPD11,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//PM_LANPHY_ENABLE
  {END_OF_GPIO_TABLE,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis, GpioDswReset,  GpioTermNone}},//Marking End of Table
};

UINT16 mGpioTable1AmberLakeY42LpDdr3CffSize = sizeof (mGpioTable1AmberLakeY42LpDdr3Cff) / sizeof (GPIO_INIT_CONFIG) - 1;

GPIO_INIT_CONFIG mGpioTable2AmberLakeY42LpDdr3Cff[] =
{
  {GPIO_SKL_LP_GPP_A7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv,  GpioOutDefault, GpioIntEdge | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//SPI_TPM_INT_N
  {GPIO_SKL_LP_GPP_A12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv,  GpioOutDefault,  GpioIntEdge | GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//SAR_DPR_PCH
  {GPIO_SKL_LP_GPP_A15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SPK_PD_N
  //{GPIO_SKL_LP_GPP_A16, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WWAN_RST_N
  {GPIO_SKL_LP_GPP_A17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInInv,  GpioOutDefault, GpioIntEdge | GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//HUMAN_PRESENCE_INT_N
  {GPIO_SKL_LP_GPP_A18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,   GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SAR_NIRQ_PCH
  {GPIO_SKL_LP_GPP_A19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirInInv,  GpioOutDefault, GpioIntEdge | GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//ALS_INT_N
  {GPIO_SKL_LP_GPP_A20, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,     GpioOutDefault, GpioIntEdge | GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//INT_SHARED
  {GPIO_SKL_LP_GPP_A21, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,     GpioOutDefault, GpioIntEdge | GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//HALL_SENSOR_INT
  {GPIO_SKL_LP_GPP_A22, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,     GpioOutDefault, GpioIntEdge | GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//ACCEL2_INT1
  {GPIO_SKL_LP_GPP_A23, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,     GpioOutDefault, GpioIntEdge | GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//ACCEL1_INT1
  {GPIO_SKL_LP_GPP_B7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,   GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//AR1_CLKREQ2_N
  {GPIO_SKL_LP_GPP_B9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,   GpioOutDefault, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WWAN_CLKREQ_N
  {GPIO_SKL_LP_GPP_B14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermWpd20K}},//WWAN_DISABLE_N
  {GPIO_SKL_LP_GPP_B15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//TBT_RST_N
  {GPIO_SKL_LP_GPP_B16, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv,  GpioOutDefault, GpioIntEdge | GpioIntSci,  GpioPlatformReset,  GpioTermWpu20K}},//AR_CIO_PLUG_EVENT_N
  {GPIO_SKL_LP_GPP_B17, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioPlatformReset,  GpioTermWpu20K}},//AR1_USB_PWR_EN
  {GPIO_SKL_LP_GPP_C4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutLow,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//AR_TBT_FORCE_PWR_R
  //{GPIO_SKL_LP_GPP_C6,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WWAN_PERST_N
  {GPIO_SKL_LP_GPP_C8,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInInv,  GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset,  GpioTermNone}},//GPPC_C8_CODEC_INT_N
  {GPIO_SKL_LP_GPP_C9,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//GPPC_H10_IVCAM_PWREN
  {GPIO_SKL_LP_GPP_C10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//EC_SLP_S0IX_N
  {GPIO_SKL_LP_GPP_C11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//WIFI_PWR_EN
  {GPIO_SKL_LP_GPP_C12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//SSD_RST_BGA_N
  {GPIO_SKL_LP_GPP_C13, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//TBT_WAKE_MUX_SEL_N
  {GPIO_SKL_LP_GPP_C15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioHostDeepReset,  GpioTermNone}},//M2_SSD_PWREN
  //{GPIO_SKL_LP_GPP_C22, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh, GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WWAN_FCP_OFF_N
  {GPIO_SKL_LP_GPP_D7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,     GpioOutDefault, GpioIntEdge | GpioIntApic, GpioPlatformReset,  GpioTermNone}},//FPS_INT_N
  {GPIO_SKL_LP_GPP_D8,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioPlatformReset,  GpioTermNone}},//FPS_RST_N
  {GPIO_SKL_LP_GPP_D9, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis,  GpioHostDeepReset,    GpioTermWpd20K } },//TCH_PNL2_RST_R_N
//  {GPIO_SKL_LP_GPP_D10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntEdge | GpioIntApic, GpioPlatformReset,  GpioTermNone}},//TCH_PNL2_INT_R_N
  {GPIO_SKL_LP_GPP_D11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//SD_SSD_RST_N
  {GPIO_SKL_LP_GPP_D12, {GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntEdge | GpioIntApic, GpioPlatformReset,  GpioTermNone}},//CLICK_PAD_INT_N
  {GPIO_SKL_LP_GPP_E0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WLAN_PERST_N
//  {GPIO_SKL_LP_GPP_E2,  {GpioPadModeGpio,  GpioHostOwnGpio, GpioDirOut,    GpioOutDefault, GpioIntDis, GpioPlatformReset,  GpioTermNone}},//AR_CIO_PWR_EN
  {GPIO_SKL_LP_GPP_E3,  {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv,  GpioOutDefault, GpioIntEdge | GpioIntSmi, GpioHostDeepReset,  GpioTermNone}},//EC_SMI_LP_N
  {GPIO_SKL_LP_GPP_E4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh,    GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WIFI_DISABLE_N
  {GPIO_SKL_LP_GPP_E8,  {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv,  GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermNone}},//WiFi Wake
  {GPIO_SKL_LP_GPP_E15, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv,  GpioOutDefault, GpioIntEdge | GpioIntSci, GpioHostDeepReset, GpioTermWpu20K}},//RTD3_TBT_WAKE_N
  {GPIO_SKL_LP_GPP_F8,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,     GpioOutHigh,   GpioIntDis, GpioHostDeepReset, GpioTolerance1v8 | GpioTermNone}},//TCH_PNL2_EN
  //{GPIO_SKL_LP_GPP_G0,  {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv,  GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermWpu20K}},//WWAN_WAKE_N
};

UINT16 mGpioTable2AmberLakeY42LpDdr3CffSize = sizeof (mGpioTable2AmberLakeY42LpDdr3Cff) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTableAmberLakeY42LpDdr3CffPcieWwan[] =
{
  {GPIO_SKL_LP_GPP_A16, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WWAN_RST_N
  {GPIO_SKL_LP_GPP_C6,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WWAN_PERST_N
  {GPIO_SKL_LP_GPP_C22, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,  GpioOutHigh,     GpioIntDis, GpioHostDeepReset,  GpioTermNone}},//WWAN_FCP_OFF_N
  {GPIO_SKL_LP_GPP_G0,  {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioPlatformReset,  GpioTermWpu20K}},//WWAN_WAKE_N
};

UINT16 mGpioTableAmberLakeY42LpDdr3CffPcieWwanSize = sizeof (mGpioTableAmberLakeY42LpDdr3CffPcieWwan) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTableAmberLakeY42LpDdr3CffPcieSlot1[] =
{
  {GPIO_SKL_LP_GPP_E2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutLow, GpioIntDis, GpioPlatformReset,  GpioTermNone}},//AR_CIO_PWR_EN
};

UINT16 mGpioTableAmberLakeY42LpDdr3CffPcieSlot1Size = sizeof (mGpioTableAmberLakeY42LpDdr3CffPcieSlot1) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTableAmberLakeY42LpDdr3CffPcieSlot1Tbt[] =
{
  {GPIO_SKL_LP_GPP_E2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,    GpioOutHigh, GpioIntDis, GpioPlatformReset,  GpioTermNone}},//AR_CIO_PWR_EN
};

UINT16 mGpioTableAmberLakeY42LpDdr3CffPcieSlot1TbtSize = sizeof (mGpioTableAmberLakeY42LpDdr3CffPcieSlot1Tbt) / sizeof (GPIO_INIT_CONFIG);


GPIO_INIT_CONFIG mGpioTableLpddr3Rvp3UcmcDevice[] =
{
    { GPIO_SKL_LP_GPP_B0, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermNone } }, //GPP_B0
    { GPIO_SKL_LP_GPP_B1, { GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntLevel | GpioIntApic, GpioHostDeepReset, GpioTermNone } }, //GPP_B1
};

UINT16 mGpioTableLpddr3Rvp3UcmcDeviceSize = sizeof (mGpioTableLpddr3Rvp3UcmcDevice) / sizeof (GPIO_INIT_CONFIG);

GPIO_INIT_CONFIG mGpioTableLpDdr3Rvp3Touchpanel =
  {GPIO_SKL_LP_GPP_E7, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic, GpioPlatformReset,  GpioTermNone}};

GPIO_INIT_CONFIG mGpioTableAmberLakeY42LpDdr3CffTouchpanel =
  {GPIO_SKL_LP_GPP_D10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutDefault, GpioIntLevel | GpioIntApic, GpioPlatformReset,  GpioTermNone}};//TCH_PNL2_INT_R_N

GPIO_INIT_CONFIG mGpioTableLpDdr3Rvp3SdhcSidebandCardDetect =
  {GPIO_SKL_LP_GPP_B17, {GpioPadModeGpio,   GpioHostOwnGpio, GpioDirIn,   GpioOutDefault, GpioIntBothEdge, GpioHostDeepReset,  GpioTermNone}}; //SD_CDB D3

//IO Expander Table for SKL RVP3, RVP7, RVP13 and RVP15
IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpander[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_3.3_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SNSR_HUB_DFU_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SATA_PWR_EN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WLAN_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//GFX_CRB_DET_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//MFG_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//FLIP_TO_TABLET_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_8,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_SLOT1_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_9,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB3_CAM_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD_TESTMODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_11, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//BIOS_REC_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_12, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//EINK_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_13, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TBT_FORCE_PWR_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_14, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIFI_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_15, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//DGPU_PRSNT_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB2_CAM_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_18, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//IMAGING_DFU_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_19, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SW_GFX_PWERGD_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_20, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_21, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TP_IOEXP1_P26
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_23, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TP_IOEXP1_P27
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP4_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_OTG_WP1_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP2_WP3_WP5_PWREN_R_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_AUDIO_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_GNSS_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED}//M.2_WIGIG_PWREN_IOEXP
};

UINT16 mGpioTableIoExpanderSize = sizeof (mGpioTableIoExpander) / sizeof (IO_EXPANDER_GPIO_CONFIG);

//IO Expander Table for KBL -Refresh
IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRDdr4[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_3.3_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SNSR_HUB_DFU_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SATA_PWR_EN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WLAN_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//GFX_CRB_DET_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//MFG_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//FLIP_TO_TABLET_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_8,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_SLOT1_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_9,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB3_CAM_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD_TESTMODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_11, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//BIOS_REC_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_12, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RTD3_CIO_PWR_EN_R
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_13, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TBT_FORCE_PWR_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_14, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIFI_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_15, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RTD3_USB_PD1_PWR_EN
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB2_CAM_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_18, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//IMAGING_DFU_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_19, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//HRESET_PD1_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_20, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_21, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_RST_IOEXP_N
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_RST_CNTRL_R
  // We want the initial state to be high.
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_RST_CNTRL_R
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_23, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_WAKE_CTRL_R_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP2_WP3_WP5_PWREN_R_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_AUDIO_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_GNSS_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_PWREN_IOEXP
};
UINT16 mGpioTableIoExpanderSizeKabylakeRDdr4 = sizeof (mGpioTableIoExpanderKabylakeRDdr4) / sizeof (IO_EXPANDER_GPIO_CONFIG);

//IO Expander Table for KBL -Refresh PCIe x4 Slot enable GPIO definition. This must be high when TBT RTD3 is enabled and the board has F22 rework.
IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRDdr4PcieSlot1[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
};
UINT16 mGpioTableIoExpanderSizeKabylakeRDdr4PcieSlot1 = sizeof (mGpioTableIoExpanderKabylakeRDdr4PcieSlot1) / sizeof (IO_EXPANDER_GPIO_CONFIG);

//IO Expander Table for KBL -kc
IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeKcDdr3[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_3.3_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SNSR_HUB_DFU_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SATA_PWR_EN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WLAN_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//GFX_CRB_DET_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//MFG_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//FLIP_TO_TABLET_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_8,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_SLOT1_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_9,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB3_CAM_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD_TESTMODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_11, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//BIOS_REC_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_12, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//EINK_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_13, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TBT_FORCE_PWR_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_14, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIFI_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_15, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB2_CAM_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_18, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//IMAGING_DFU_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_19, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_20, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_21, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TP_IOEXP1_P26
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_23, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TP_IOEXP1_P27
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP4_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_OTG_WP1_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP2_WP3_WP5_PWREN_R_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_AUDIO_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_GNSS_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_8,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//FPS_LOCK_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_9,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_FLEX_PWREN
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB_UART_SEL
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_11, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_DOCK_PWREN_IOEXP_R
};
UINT16 mGpioTableIoExpanderSizeKabylakeKcDdr3 = sizeof (mGpioTableIoExpanderKabylakeKcDdr3) / sizeof (IO_EXPANDER_GPIO_CONFIG);
//IO Expander Table Full table for KBL RVP3
IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRvp3[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_3.3_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SNSR_HUB_DFU_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SATA_PWR_EN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WLAN_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//GFX_CRB_DET_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//MFG_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//FLIP_TO_TABLET_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_9,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB3_CAM_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD_TESTMODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_11, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//BIOS_REC_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_12, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//EINK_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_13, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TBT_FORCE_PWR_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_14, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIFI_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_15, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//DGPU_PRSNT_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED },//SW_GFX_DGPU_SEL (KBL_RVP3_BOARD)
//{IO_EXPANDER_0, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB2_CAM_PWREN_IOEXP (SKL_RVP3_BOARD)
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_18, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//IMAGING_DFU_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_19, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SW_GFX_PWERGD_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_20, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_21, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TP_IOEXP1_P26
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_23, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TP_IOEXP1_P27
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP4_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED },//Not Connected (KBK_RVP3_BOARD)
//{IO_EXPANDER_1, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_OTG_WP1_PWREN_IOEXP (SKL_RVP3_BOARD)
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP2_WP3_WP5_PWREN_R_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_AUDIO_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_GNSS_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB2_CAM_PWREN (KBL_RVP3_BOARD)
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//FPS_LOCK_N  (KBL_RVP3_BOARD)
};

UINT16 mGpioTableIoExpanderKabylakeRvp3Size = sizeof (mGpioTableIoExpanderKabylakeRvp3) / sizeof (IO_EXPANDER_GPIO_CONFIG);

//IO Expander Table Full table for AML Y42 RVP3
IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderAmberLakeY42Rvp3[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_3.3_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SNSR_HUB_DFU_IOEXP_N
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SATA_PWR_EN_IOEXP
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WLAN_WAKE_CTRL_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//GFX_CRB_DET_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//MFG_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//FLIP_TO_TABLET_MODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_9,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB3_CAM_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//RSVD_TESTMODE_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_11, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//BIOS_REC_IOEXP
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_12, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//EINK_PWREN_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_13, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TBT_FORCE_PWR_IOEXP
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_14, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIFI_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_15, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//DGPU_PRSNT_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED },//SW_GFX_DGPU_SEL (KBL_RVP3_BOARD)
//{IO_EXPANDER_0, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB2_CAM_PWREN_IOEXP (SKL_RVP3_BOARD)
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_18, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//IMAGING_DFU_IOEXP
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_19, IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//SW_GFX_PWERGD_IOEXP
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_20, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WIGIG_WAKE_CTRL_IOEXP_N
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_21, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_SSD_RST_IOEXP_N
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TP_IOEXP1_P26
  //{IO_EXPANDER_0, IO_EXPANDER_GPIO_23, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//TP_IOEXP1_P27
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_0,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_1,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_2,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP4_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_INPUT,  IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED },//Not Connected (KBK_RVP3_BOARD)
//{IO_EXPANDER_1, IO_EXPANDER_GPIO_3,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_OTG_WP1_PWREN_IOEXP (SKL_RVP3_BOARD)
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_4,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB32_WP2_WP3_WP5_PWREN_R_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_5,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_AUDIO_PWREN_IOEXP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_6,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_GNSS_DISABLE_IOEXP_N
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_7,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//M.2_WWAN_FP
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//CRD_CAM_PWREN
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_17, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//USB2_CAM_PWREN (KBL_RVP3_BOARD)
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_18, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//CAM_CLK_EN
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_19, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//CAM_SYNC_IN
  {IO_EXPANDER_1, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//FPS_LOCK_N  (KBL_RVP3_BOARD)
};

UINT16 mGpioTableIoExpanderAmberLakeY42Rvp3Size = sizeof (mGpioTableIoExpanderAmberLakeY42Rvp3) / sizeof (IO_EXPANDER_GPIO_CONFIG);

IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRvp3PcieSlot1[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_8,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_SLOT1_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
};

UINT16 mGpioTableIoExpanderKabylakeRvp3PcieSlot1Size = sizeof (mGpioTableIoExpanderKabylakeRvp3PcieSlot1) / sizeof (IO_EXPANDER_GPIO_CONFIG);

IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderAmberLakeY42Rvp3PcieSlot1[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
};

UINT16 mGpioTableIoExpanderAmberLakeY42Rvp3PcieSlot1Size = sizeof (mGpioTableIoExpanderKabylakeRvp3PcieSlot1) / sizeof (IO_EXPANDER_GPIO_CONFIG);

IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRvp3PcieWwan[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_8,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_SLOT1_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
};

UINT16 mGpioTableIoExpanderKabylakeRvp3PcieWwanSize = sizeof (mGpioTableIoExpanderKabylakeRvp3PcieWwan) / sizeof (IO_EXPANDER_GPIO_CONFIG);

IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderAmberLakeY42Rvp3PcieWwanTbt[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
};

UINT16 mGpioTableIoExpanderAmberLakeY42Rvp3PcieWwanTbtSize = sizeof (mGpioTableIoExpanderAmberLakeY42Rvp3PcieWwanTbt) / sizeof (IO_EXPANDER_GPIO_CONFIG);

IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRvp3AmlPcieSlot1Tbt[] =
{
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_8,  IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_LOW,  IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCH_SLOT1_RST_IOEXP_N
  {IO_EXPANDER_0, IO_EXPANDER_GPIO_16, IO_EXPANDER_GPIO_OUTPUT, IO_EXPANDER_GPO_LEVEL_HIGH, IO_EXPANDER_GPI_INV_DISABLED, IO_EXPANDER_GPIO_RESERVED},//PCIE_SLOT1_PWREN_WKCTRL_IOEXP_N
};

UINT16 mGpioTableIoExpanderKabylakeRvp3AmlPcieSlot1TbtSize = sizeof (mGpioTableIoExpanderKabylakeRvp3AmlPcieSlot1Tbt) / sizeof (IO_EXPANDER_GPIO_CONFIG);

#endif // _SKYLAKE_A0_RVP3_GPIO_TABLE_H_
