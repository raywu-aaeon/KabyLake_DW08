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
#ifndef _GPIO_TABLE_ZUMBABEACH_H_
#define _GPIO_TABLE_ZUMBABEACH_H_

static GPIO_INIT_CONFIG upZumbaBeachGpioDefinitionTable[] =
{
    {GPIO_SKL_H_GPP_A0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_0_RCINB_ESPI_ALERT1B
    {GPIO_SKL_H_GPP_A1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_1_LAD_0_ESPI_IO_0
    {GPIO_SKL_H_GPP_A2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_2_LAD_1_ESPI_IO_1
    {GPIO_SKL_H_GPP_A3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_3_LAD_2_ESPI_IO_2
    {GPIO_SKL_H_GPP_A4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_4_LAD_3_ESPI_IO_3
    {GPIO_SKL_H_GPP_A5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_5_LFRAMEB_ESPI_CS0B
    {GPIO_SKL_H_GPP_A6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_6_SERIRQ_ESPI_CS1B
    {GPIO_SKL_H_GPP_A7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_7_PIRQAB_ESPI_ALERT0B
    {GPIO_SKL_H_GPP_A8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_8_CLKRUNB
    {GPIO_SKL_H_GPP_A9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_9_CLKOUT_LPC_0_ESPI_CLK
    {GPIO_SKL_H_GPP_A10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_10_CLKOUT_LPC_1
    {GPIO_SKL_H_GPP_A11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_11_PMEB
    {GPIO_SKL_H_GPP_A12, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci,    GpioPlatformReset,  GpioTermNone}},//GPP_A_12_BM_BUSYB_ISH_GP_6_SX_EXIT_HOLDOFFB
    {GPIO_SKL_H_GPP_A13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_13_SUSWARNB_SUSPWRDNACK
    {GPIO_SKL_H_GPP_A14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_14_SUS_STATB_ESPI_RESETB
    {GPIO_SKL_H_GPP_A15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_15_SUSACKB
    {GPIO_SKL_H_GPP_A16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_16_CLKOUT_48
    {GPIO_SKL_H_GPP_A17, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_17_ISH_GP_7
    {GPIO_SKL_H_GPP_A18, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_A_18_ISH_GP_0
    {GPIO_SKL_H_GPP_A20, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_20_ISH_GP_2
    {GPIO_SKL_H_GPP_A21, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_21_ISH_GP_3
    {GPIO_SKL_H_GPP_A22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_22_ISH_GP_4
    {GPIO_SKL_H_GPP_A23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_A_23_ISH_GP_5
    {GPIO_SKL_H_GPP_B0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_0
    {GPIO_SKL_H_GPP_B1,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_1
    {GPIO_SKL_H_GPP_B2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_2_VRALERTB
    {GPIO_SKL_H_GPP_B3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_3_CPU_GP2
    {GPIO_SKL_H_GPP_B4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_4_CPU_GP3
    {GPIO_SKL_H_GPP_B5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_5_SRCCLKREQB_0
    {GPIO_SKL_H_GPP_B6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_B_6_SRCCLKREQB_1
    {GPIO_SKL_H_GPP_B7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_7_SRCCLKREQB_2
    {GPIO_SKL_H_GPP_B8,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_8_SRCCLKREQB_3
    {GPIO_SKL_H_GPP_B9,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_9_SRCCLKREQB_4
    {GPIO_SKL_H_GPP_B10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_10_SRCCLKREQB_5
    {GPIO_SKL_H_GPP_B11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_11_EXT_PWR_GATEB
    {GPIO_SKL_H_GPP_B12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_12_SLP_S0B
    {GPIO_SKL_H_GPP_B13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_13_PLTRSTB
    {GPIO_SKL_H_GPP_B14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_B_14_SPKR
    {GPIO_SKL_H_GPP_B15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_15_GSPI0_CSB
    {GPIO_SKL_H_GPP_B16, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_16_GSPI0_CLK
    {GPIO_SKL_H_GPP_B17, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_17_GSPI0_MISO
    {GPIO_SKL_H_GPP_B18, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_18_GSPI0_MOSI
    {GPIO_SKL_H_GPP_B19, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_19_GSPI1_CSB
    {GPIO_SKL_H_GPP_B20, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_B_20_GSPI1_CLK
    {GPIO_SKL_H_GPP_B21, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_21_GSPI1_MISO
    {GPIO_SKL_H_GPP_B22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_22_GSPI1_MOSI
    {GPIO_SKL_H_GPP_B23, {GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_B_23_SML1ALERTB_PCHHOTB
    {GPIO_SKL_H_GPP_C8,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_C_8_UART0_RXD
    {GPIO_SKL_H_GPP_C9,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_C_9_UART0_TXD
    {GPIO_SKL_H_GPP_C10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_C_10_UART0_RTSB
    {GPIO_SKL_H_GPP_C11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_C_11_UART0_CTSB
    {GPIO_SKL_H_GPP_C12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_C_12_UART1_RXD_ISH_UART1_RXD
    {GPIO_SKL_H_GPP_C13, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_C_13_UART1_TXD_ISH_UART1_TXD
    {GPIO_SKL_H_GPP_C14, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci,    GpioPlatformReset,  GpioTermNone}},//GPP_C_14_UART1_RTSB_ISH_UART1_RTSB
    {GPIO_SKL_H_GPP_C15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_C_15_UART1_CTSB_ISH_UART1_CTSB
    {GPIO_SKL_H_GPP_C16, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_C_16_I2C0_SDA
    {GPIO_SKL_H_GPP_C17, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_C_17_I2C0_SCL
    {GPIO_SKL_H_GPP_C18, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_C_18_I2C1_SDA
    {GPIO_SKL_H_GPP_C19, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_C_19_I2C1_SCL
    {GPIO_SKL_H_GPP_C22, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSmi,    GpioPlatformReset,  GpioTermNone}},//GPP_C_22_UART2_RTSB
    {GPIO_SKL_H_GPP_C23, {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSmi,    GpioPlatformReset,  GpioTermNone}},//GPP_C_23_UART2_CTSB
    {GPIO_SKL_H_GPP_D0,  {GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntNmi,    GpioPlatformReset,  GpioTermNone}},//GPP_D_0_SPI1_CSB
    {GPIO_SKL_H_GPP_D1,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_1_SPI1_CLK
    {GPIO_SKL_H_GPP_D2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_2_SPI1_MISO_IO_1
    {GPIO_SKL_H_GPP_D3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_3_SPI1_MOSI_IO_0
    {GPIO_SKL_H_GPP_D4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_D_4_ISH_I2C2_SDA_I2C3_SDA
    {GPIO_SKL_H_GPP_D5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_5_SSP0_SFRM
    {GPIO_SKL_H_GPP_D6,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_6_SSP0_TXD
    {GPIO_SKL_H_GPP_D7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_7_SSP0_RXD
    {GPIO_SKL_H_GPP_D8,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_8_SSP0_SCLK
    {GPIO_SKL_H_GPP_D9,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_9_ISH_SPI_CSB
    {GPIO_SKL_H_GPP_D10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_10_ISH_SPI_CLK
    {GPIO_SKL_H_GPP_D11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_11_ISH_SPI_MISO
    {GPIO_SKL_H_GPP_D12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_12_ISH_SPI_MOSI
    {GPIO_SKL_H_GPP_D13, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_13_ISH_UART0_RXD_SML0BDATA_I2C2_SDA
    {GPIO_SKL_H_GPP_D14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_14_ISH_UART0_TXD_SML0BCLK_I2C2_SCL
    {GPIO_SKL_H_GPP_D15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_15_ISH_UART0_RTSB
    {GPIO_SKL_H_GPP_D16, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_16_ISH_UART0_CTSB_SML0BALERTB
    {GPIO_SKL_H_GPP_D17, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_17_DMIC_CLK_1
    {GPIO_SKL_H_GPP_D18, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermWpd20K}},//GPP_D_18_DMIC_DATA_1
    {GPIO_SKL_H_GPP_D19, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_D_19_DMIC_CLK_0
    {GPIO_SKL_H_GPP_D20, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermWpd20K}},//GPP_D_20_DMIC_DATA_0
    {GPIO_SKL_H_GPP_D21, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_21_SPI1_IO_2
    {GPIO_SKL_H_GPP_D22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_22_SPI1_IO_3
    {GPIO_SKL_H_GPP_D23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_D_23_ISH_I2C2_SCL_I2C3_SCL
    {GPIO_SKL_H_GPP_E0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_0_SATAXPCIE_0_SATAGP_0
    {GPIO_SKL_H_GPP_E1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_1_SATAXPCIE_1_SATAGP_1
    {GPIO_SKL_H_GPP_E2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_2_SATAXPCIE_2_SATAGP_2
    {GPIO_SKL_H_GPP_E3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_3_CPU_GP0
    {GPIO_SKL_H_GPP_E4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_4_SATA_DEVSLP_0
    {GPIO_SKL_H_GPP_E5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_5_SATA_DEVSLP_1
    {GPIO_SKL_H_GPP_E6,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_6_SATA_DEVSLP_2
    {GPIO_SKL_H_GPP_E7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_7_CPU_GP1
    {GPIO_SKL_H_GPP_E8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_8_SATA_LEDB
    {GPIO_SKL_H_GPP_E9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_9_USB2_OCB_0
    {GPIO_SKL_H_GPP_E10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_10_USB2_OCB_1
    {GPIO_SKL_H_GPP_E11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_11_USB2_OCB_2
    {GPIO_SKL_H_GPP_E12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_E_12_USB2_OCB_3
    {GPIO_SKL_H_GPP_F0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_0_SATAXPCIE_3_SATAGP_3
    {GPIO_SKL_H_GPP_F1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_1_SATAXPCIE_4_SATAGP_4
    {GPIO_SKL_H_GPP_F2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_2_SATAXPCIE_5_SATAGP_5
    {GPIO_SKL_H_GPP_F3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_3_SATAXPCIE_6_SATAGP_6
    {GPIO_SKL_H_GPP_F4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_4_SATAXPCIE_7_SATAGP_7
    {GPIO_SKL_H_GPP_F5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic,    GpioPlatformReset,  GpioTermNone}},//GPP_F_5_SATA_DEVSLP_3
    {GPIO_SKL_H_GPP_F6,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_F_6_SATA_DEVSLP_4
    {GPIO_SKL_H_GPP_F7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_F_7_SATA_DEVSLP_5
    {GPIO_SKL_H_GPP_F8,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_F_8_SATA_DEVSLP_6
    {GPIO_SKL_H_GPP_F9,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_F_9_SATA_DEVSLP_7
    {GPIO_SKL_H_GPP_F10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_10_SATA_SCLOCK
    {GPIO_SKL_H_GPP_F11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_11_SATA_SLOAD
    {GPIO_SKL_H_GPP_F12, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_12_SATA_SDATAOUT1
    {GPIO_SKL_H_GPP_F13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_13_SATA_SDATAOUT0
    {GPIO_SKL_H_GPP_F14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_14
    {GPIO_SKL_H_GPP_F15, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_15_USB2_OCB_4
    {GPIO_SKL_H_GPP_F16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_16_USB2_OCB_5
    {GPIO_SKL_H_GPP_F17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_17_USB2_OCB_6
    {GPIO_SKL_H_GPP_F18, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_18_USB2_OCB_7
    {GPIO_SKL_H_GPP_F19, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_19_L_VDDEN
    {GPIO_SKL_H_GPP_F20, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_20_L_BKLTEN
    {GPIO_SKL_H_GPP_F21, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_21_L_BKLTCTL
    {GPIO_SKL_H_GPP_F22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_22
    {GPIO_SKL_H_GPP_F23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_F_23
    {GPIO_SKL_H_GPP_G0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_0_FAN_TACH_0
    {GPIO_SKL_H_GPP_G1,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_1_FAN_TACH_1
    {GPIO_SKL_H_GPP_G2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_2_FAN_TACH_2
    {GPIO_SKL_H_GPP_G3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_3_FAN_TACH_3
    {GPIO_SKL_H_GPP_G4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_4_FAN_TACH_4
    {GPIO_SKL_H_GPP_G5,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_5_FAN_TACH_5
    {GPIO_SKL_H_GPP_G6,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_6_FAN_TACH_6
    {GPIO_SKL_H_GPP_G7,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_7_FAN_TACH_7
    {GPIO_SKL_H_GPP_G8,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_8_FAN_PWM_0
    {GPIO_SKL_H_GPP_G9,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_9_FAN_PWM_1
    {GPIO_SKL_H_GPP_G10, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_10_FAN_PWM_2
    {GPIO_SKL_H_GPP_G11, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_11_FAN_PWM_3
    {GPIO_SKL_H_GPP_G12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_12_GSXDOUT
    {GPIO_SKL_H_GPP_G13, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_13_GSXSLOAD
    {GPIO_SKL_H_GPP_G14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_14_GSXDIN
    {GPIO_SKL_H_GPP_G15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_15_GSXSRESETB
    {GPIO_SKL_H_GPP_G16, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_16_GSXCLK
    {GPIO_SKL_H_GPP_G17, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_17_ADR_COMPLETE
    {GPIO_SKL_H_GPP_G19, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_19_SMIB
    {GPIO_SKL_H_GPP_G23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_G_23
    {GPIO_SKL_H_GPP_H0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_0_SRCCLKREQB_6
    {GPIO_SKL_H_GPP_H2,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_2_SRCCLKREQB_8
    {GPIO_SKL_H_GPP_H3,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_3_SRCCLKREQB_9
    {GPIO_SKL_H_GPP_H10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_10_SML2CLK
    {GPIO_SKL_H_GPP_H11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_11_SML2DATA
    {GPIO_SKL_H_GPP_H12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_12_SML2ALERTB
    {GPIO_SKL_H_GPP_H13, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_13_SML3CLK
    {GPIO_SKL_H_GPP_H14, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_14_SML3DATA
    {GPIO_SKL_H_GPP_H15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_15_SML3ALERTB
    {GPIO_SKL_H_GPP_H16, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_16_SML4CLK
    {GPIO_SKL_H_GPP_H17, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_17_SML4DATA
    {GPIO_SKL_H_GPP_H18, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_H_18_SML4ALERTB
    {GPIO_SKL_H_GPP_H19, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_H_19
    {GPIO_SKL_H_GPP_H20, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_H_20
    {GPIO_SKL_H_GPP_H21, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_H_21
    {GPIO_SKL_H_GPP_H22, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_H_22
    {GPIO_SKL_H_GPP_H23, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_H_23
    {GPIO_SKL_H_GPP_I0,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_0_DDSP_HPD_0
    {GPIO_SKL_H_GPP_I1,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_1_DDSP_HPD_1
    {GPIO_SKL_H_GPP_I2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_2_DDSP_HPD_2
    {GPIO_SKL_H_GPP_I3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_3_DDSP_HPD_3
    {GPIO_SKL_H_GPP_I4,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_4_EDP_HPD
    {GPIO_SKL_H_GPP_I5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_5_DDPB_CTRLCLK
    {GPIO_SKL_H_GPP_I6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_6_DDPB_CTRLDATA
    {GPIO_SKL_H_GPP_I7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_7_DDPC_CTRLCLK
    {GPIO_SKL_H_GPP_I8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_8_DDPC_CTRLDATA
    {GPIO_SKL_H_GPP_I9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_9_DDPD_CTRLCLK
    {GPIO_SKL_H_GPP_I10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioHostDeepReset,  GpioTermNone}},//GPP_I_10_DDPD_CTRLDATA
    {GPIO_SKL_H_GPD0,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_0_BATLOWB
    {GPIO_SKL_H_GPD1,  {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_1_ACPRESENT
    {GPIO_SKL_H_GPD2,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_2_LAN_WAKEB
    {GPIO_SKL_H_GPD3,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_3_PWRBTNB
    {GPIO_SKL_H_GPD4,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_4_SLP_S3B
    {GPIO_SKL_H_GPD5,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_5_SLP_S4B
    {GPIO_SKL_H_GPD6,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_6_SLP_AB
    {GPIO_SKL_H_GPD7,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_7_USB2_WAKEOUTB
    {GPIO_SKL_H_GPD8,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_8_SUSCLK
    {GPIO_SKL_H_GPD9,  {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_9_SLP_WLANB
    {GPIO_SKL_H_GPD10, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_10_SLP_S5B
    {GPIO_SKL_H_GPD11, {GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,    GpioDswReset,  GpioTermNone}},//GPD_11_LANPHYPC

};
#endif
