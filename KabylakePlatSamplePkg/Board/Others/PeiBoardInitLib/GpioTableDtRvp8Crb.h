/** @file
  GPIO definition table.
  This file was automatically generated. Modify at your own risk.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2015 Intel Corporation.

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
#ifndef _GPIO_TABLE_DT_RVP8_CRB_H
#define _GPIO_TABLE_DT_RVP8_CRB_H

static GPIO_INIT_CONFIG mGpioTableDtRvp8Crb[] =
{
  {GPIO_SKL_H_GPP_A0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //RCINB__ESPI_ALERT1B
  {GPIO_SKL_H_GPP_A1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K  }}, //LPC_AD0_ESPI_IO0
  {GPIO_SKL_H_GPP_A2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K  }}, //LPC_AD1_ESPI_IO1
  {GPIO_SKL_H_GPP_A3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K  }}, //LPC_AD2_ESPI_IO2
  {GPIO_SKL_H_GPP_A4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K  }}, //LPC_AD3_ESPI_IO3
  {GPIO_SKL_H_GPP_A5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //LPC_FRAME_ESPI_CS0_N
  {GPIO_SKL_H_GPP_A6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //INT_SERIRQ_ESPI_CS1_N
  {GPIO_SKL_H_GPP_A7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //PIRQAB_ESPI_ALERT0_N/Not Used
  {GPIO_SKL_H_GPP_A8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //PM_CLKRUN_N
  {GPIO_SKL_H_GPP_A9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermWpd20K  }}, //LPC_CLK_ESPI_CLK
  {GPIO_SKL_H_GPP_A10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermWpd20K  }}, //PCH_CLK_PCI_TPM
  {GPIO_SKL_H_GPP_A11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //PMEB/NOT USED //ray_todo : PME function porting
  {GPIO_SKL_H_GPP_A12, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //BM_BUSYB_ISH_GP_6/NOT USED
  {GPIO_SKL_H_GPP_A13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //SUS_PWR_ACK_R
  {GPIO_SKL_H_GPP_A14, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //PM_SUS_STAT_ESPI_RST_N
  {GPIO_SKL_H_GPP_A15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermWpu20K  }}, //SUSACK_R_N
  {GPIO_SKL_H_GPP_A16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //CLKOUT_48/NOT USED
  {GPIO_SKL_H_GPP_A17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_GP_7/NOT USED
  {GPIO_SKL_H_GPP_A18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_A19, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_A20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_GP_2/ NOT USED
  {GPIO_SKL_H_GPP_A21, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_A22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_A23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B0,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B1,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B2,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_B3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //CPU_GP2/BT_RF_KILL_R_N
//  {GPIO_SKL_H_GPP_B4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //CPU_GP3/ EXTTS_SNI_DRV1_PCH
//  {GPIO_SKL_H_GPP_B5,  {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioHostDeepReset   , GpioTermNone }}, //SRCCLKREQB_0/ M.2_BT_UART_WAKE_N
  {GPIO_SKL_H_GPP_B3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B6,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B8,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_4 / PCIEX1_SLOT1_PRSNT2_R_N
  {GPIO_SKL_H_GPP_B10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_5 / PCIEX1_SLOT2_PRSNT2_R_N
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  // {GPIO_SKL_H_GPP_B6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_1/ CLK_REQ1_M.2_WIFI_N
  // {GPIO_SKL_H_GPP_B7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //CLK_REQ2_LAN_N
  // {GPIO_SKL_H_GPP_B8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //CLK_REQ3_M.2_WIGIG_SNK_N
  // {GPIO_SKL_H_GPP_B9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_4 / PCIEX1_SLOT1_PRSNT2_R_N
  // {GPIO_SKL_H_GPP_B10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_5 / PCIEX1_SLOT2_PRSNT2_R_N
  {GPIO_SKL_H_GPP_B11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //EXT_PWR_GATEB / NOT USED
  {GPIO_SKL_H_GPP_B12, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //PCH_SLP_S0_N
  {GPIO_SKL_H_GPP_B13, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //PLT_RST_N
  {GPIO_SKL_H_GPP_B14, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermWpd20K  }}, //SPKR
  {GPIO_SKL_H_GPP_B15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //GSPI0_CLK / NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_B17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //GSPI0_MISO / WIFI_RF_KILL_R_N
  {GPIO_SKL_H_GPP_B17,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_B18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B19, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B21, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_B22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_B23, {GpioPadModeNative2, GpioHostOwnDefault, GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermWpd20K  }}, //SML1ALERTB_PCHHOTB/ PCHHOT_N
  {GPIO_SKL_H_GPP_B23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_C0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SMB_CLK
  {GPIO_SKL_H_GPP_C1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SMB_DATA
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_C2,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //SMBALERTB/PCH_PORT80_LED--FP LED
//  {GPIO_SKL_H_GPP_C3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SML0_CLK
//  {GPIO_SKL_H_GPP_C4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SML0_DATA
//  {GPIO_SKL_H_GPP_C5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SML0ALERTB/NOT USED
//  {GPIO_SKL_H_GPP_C6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SML1_CLK
//  {GPIO_SKL_H_GPP_C7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //SML1_DATA
//  {GPIO_SKL_H_GPP_C8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART0_RXD
//  {GPIO_SKL_H_GPP_C9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART0_TXD
//  {GPIO_SKL_H_GPP_C10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART0_RTS_N
//  {GPIO_SKL_H_GPP_C11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART0_CTS_N
  {GPIO_SKL_H_GPP_C2, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C3, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C4, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C5, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C6, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C7, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C8, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C9, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_C12, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART1_ISH_UART1_RXD / NOT USED
  {GPIO_SKL_H_GPP_C13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART1_ISH_UART1_TXD / NOT USED
  {GPIO_SKL_H_GPP_C14, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART1_ISH_UART1_RTS_N / NOT USED
  {GPIO_SKL_H_GPP_C15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART1_ISH_UART1_CTS_N / NOT USED
  {GPIO_SKL_H_GPP_C16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_I2C0_SDA / NOT USED
  {GPIO_SKL_H_GPP_C17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_I2C0_SCL / NOT USED
  {GPIO_SKL_H_GPP_C18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C19, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_C20, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART2_RXD
//  {GPIO_SKL_H_GPP_C21, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART2_TXD
//  {GPIO_SKL_H_GPP_C22, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART2_RTS_N
//  {GPIO_SKL_H_GPP_C23, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SERIALIO_UART2_CTS_N
  {GPIO_SKL_H_GPP_C20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C21, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_C23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_D0,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D1,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D2,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_D5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SSP0_SFRM
//  {GPIO_SKL_H_GPP_D6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SSP0_TXD
//  {GPIO_SKL_H_GPP_D7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SSP0_RXD
//  {GPIO_SKL_H_GPP_D8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut   , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SSP0_SCLK
  {GPIO_SKL_H_GPP_D5, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D6, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D7, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D8, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_D9,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_SPI_CSB/NOT USED
  {GPIO_SKL_H_GPP_D10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_SPI_CLK/NOT USED
  {GPIO_SKL_H_GPP_D11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_SPI_MISO/NOT USED
  {GPIO_SKL_H_GPP_D12, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_SPI_MOSI/NOT USED
  {GPIO_SKL_H_GPP_D13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_UART0_RXD_SML0BDATA_I2C2_SDA/NOT USED
  {GPIO_SKL_H_GPP_D14, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_UART0_TXD_SML0BCLK_I2C2_SCL/NOT USED
  {GPIO_SKL_H_GPP_D15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_UART0_RTS_N/NOT USED
  {GPIO_SKL_H_GPP_D16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_UART0_CTS_SML0B_ALERT_N/NOT USED
  {GPIO_SKL_H_GPP_D17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D19, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D21, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_D23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_E0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K  }}, //SATAE_IFDET
//  {GPIO_SKL_H_GPP_E1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K  }}, //SATAE_IFDET
  {GPIO_SKL_H_GPP_E0,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_E1,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_E2,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_E3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //CPU_GP0_VISA2CH1_D2/EXTTS_SNI_DRV0_PCH
//  {GPIO_SKL_H_GPP_E4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATA_DEVSLP_0
  {GPIO_SKL_H_GPP_E3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_E4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_E5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_E6,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_E7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_E8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATA_LEDB_VISA2CH1_D7/PCH_SATA_LED_N(All SKU)
  {GPIO_SKL_H_GPP_E9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB2_OCB_0_VISA2CH2_D0/USB_OC0_R_N
  {GPIO_SKL_H_GPP_E10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB2_OCB_1_VISA2CH2_D1/USB_OC1_R_N
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_E11, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB2_OCB_2_VISA2CH2_D2/USB_OC2_R_N
//  {GPIO_SKL_H_GPP_E12, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB2_OCB_3__VISA2CH2_D3/USB_OC3_R_N
  {GPIO_SKL_H_GPP_E11,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_E12,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_F0,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_F1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K  }}, //SATAXPCIE_4_SATAGP_4_VISA2CH2_D5
  {GPIO_SKL_H_GPP_F1,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_F2,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATAXPCIE_5_SATAGP_5_VISA2CH2_D6/NOT USED
  {GPIO_SKL_H_GPP_F3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATAXPCIE_6_SATAGP_6_VISA2CH2_D7/NOT USED
  {GPIO_SKL_H_GPP_F4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATAXPCIE_7_SATAGP_7_VISA2CH2_CLK/NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_F5,  {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirIn      , GpioOutDefault, GpioIntEdge      | GpioIntSci, GpioHostDeepReset   , GpioTermNone }}, //SATA_DEVSLP_3/PCH_CODEC_IRQ
//  {GPIO_SKL_H_GPP_F6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATA_DEVSLP_4/M.2_SSD_DEVSLP
  {GPIO_SKL_H_GPP_F5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_F6,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_F7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATA_DEVSLP_5/NOT USED
  {GPIO_SKL_H_GPP_F8,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATA_DEVSLP_6/NOT USED
  {GPIO_SKL_H_GPP_F9,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATA_DEVSLP_7/NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_F10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic , GpioPlatformReset , GpioTermNone }}, //SATA_SCLOCK/PCH_CONFIG_JUMPER
//  {GPIO_SKL_H_GPP_F11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SATA_SLOAD/RSVD
//  {GPIO_SKL_H_GPP_F12, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntApic , GpioPlatformReset , GpioTermNone }}, //SATA_SDATAOUT1/SV_ADVANCE
//  {GPIO_SKL_H_GPP_F13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic , GpioPlatformReset , GpioTermNone }}, //SATA_SDATAOUT0/GFX_CRB_DETECT
  {GPIO_SKL_H_GPP_F10,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_F11,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_F12,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_F13,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_F14, {GpioPadModeGpio   , GpioHostOwnDefault, GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioHostDeepReset   , GpioTermNone }}, //H_SKTOCC_N
  {GPIO_SKL_H_GPP_F15, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_OC4_R_N
  {GPIO_SKL_H_GPP_F16, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_OC5_R_N
  {GPIO_SKL_H_GPP_F17, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioHostDeepReset   , GpioTermNone }}, //USB_OC6_R_N
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_F18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //USB2_OCB_7/GPIO_PCIE_RESET
  {GPIO_SKL_H_GPP_F18,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_F19, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_F20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_F21, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_F22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //GPIO_PEG_RESET
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_F23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //VCORE_VBOOST_CTRL
//  {GPIO_SKL_H_GPP_G0,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //FAN_TACH_0/WIGIG_SNK_POWERGATE
//  {GPIO_SKL_H_GPP_G1,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //FAN_TACH_1/SATA_ODD_PWRGT_R
//  {GPIO_SKL_H_GPP_G2,  {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioPlatformReset , GpioTermNone }}, //FAN_TACH_2/SATA_ODD_DA_INT_N
//  {GPIO_SKL_H_GPP_G3,  {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioPlatformReset , GpioTermNone }}, //FAN_TACH_3/SMC_RUNTIME_SCI_N
//  {GPIO_SKL_H_GPP_G4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FAN_TACH_4/M.2_WIGIG_PGPIO
//  {GPIO_SKL_H_GPP_G5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioPlatformReset , GpioTermNone }}, //FAN_TACH_5/SV_DETECT
//  {GPIO_SKL_H_GPP_G6,  {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntEdge      | GpioIntSci, GpioPlatformReset , GpioTermNone }}, //FAN_TACH_6/WIGIG_SGPIO_SW
//  {GPIO_SKL_H_GPP_G7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //FAN_TACH_7/M.2_WIGIG_DISABLE_R_N
  {GPIO_SKL_H_GPP_F23,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G0, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G1, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G2, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G3, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G4, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G5, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G6, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G7, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_G8,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //FAN_PWM_0/NOT USED
  {GPIO_SKL_H_GPP_G9,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //FAN_PWM_1/NOT USED
  {GPIO_SKL_H_GPP_G10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //FAN_PWM_2/NOT USED
  {GPIO_SKL_H_GPP_G11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //FAN_PWM_3/NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_G12, {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntEdge      | GpioIntSci, GpioPlatformReset   , GpioTermWpd20K }}, //GSXDOUT/TBCIO_PLUG_EVENT
//  {GPIO_SKL_H_GPP_G13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //GSXSLOAD/IVCAM_PWRGT_R
//  {GPIO_SKL_H_GPP_G14, {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntEdge      | GpioIntSci, GpioPlatformReset , GpioTermNone }}, //GSXDIN/M.2_WIGIG_WGPIO
//  {GPIO_SKL_H_GPP_G15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //GSXSRESETB/M.2_WIGIG_WAKE_CTRL_N
//  {GPIO_SKL_H_GPP_G16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDis      , GpioPlatformReset , GpioTermWpd20K    }}, //GSXCLK/TB_FRC_PWR
  {GPIO_SKL_H_GPP_G12, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G14, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_G17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ADR_COMPLETE/NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_G18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioPlatformReset , GpioTermNone }}, //NMIB/2X4_POWER_DETECT
//  {GPIO_SKL_H_GPP_G19, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SMIB/PCH_SMI_N
//  {GPIO_SKL_H_GPP_G20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioPlatformReset , GpioTermNone }}, //TEST_SETUP_MENU
//  {GPIO_SKL_H_GPP_G21, {GpioPadModeGpio   , GpioHostOwnAcpi   , GpioDirInInv   , GpioOutDefault, GpioIntEdge      | GpioIntSci, GpioHostDeepReset   , GpioTermNone }}, //P_INTF_N (SPI TPM PIRQ)
//  {GPIO_SKL_H_GPP_G22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //PCH_PEGSLOT1_PWREN
//  {GPIO_SKL_H_GPP_G23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //IVCAM_DFU_R
  {GPIO_SKL_H_GPP_G18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G19, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G21, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_G23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  // {GPIO_SKL_H_GPP_H0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_6/PCIEX4_SLOT3_PRSNT2_R_N
//  // {GPIO_SKL_H_GPP_H1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_7/CLK_REQ7_M.2_SSD_SLOT_N
//  {GPIO_SKL_H_GPP_H2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_8/PCIEX16_PRSNT2_N
  {GPIO_SKL_H_GPP_H0, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_H1, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_H2, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  // {GPIO_SKL_H_GPP_H3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_9/CLK_REQ9_SATAE_N
  {GPIO_SKL_H_GPP_H4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_10/NOT USED
  {GPIO_SKL_H_GPP_H5,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_11/NOT USED
  {GPIO_SKL_H_GPP_H6,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_12/NOT USED
  {GPIO_SKL_H_GPP_H7,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_13/NOT USED
  {GPIO_SKL_H_GPP_H8,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_14/NOT USED
  {GPIO_SKL_H_GPP_H9,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SRCCLKREQB_15/NOT USED
  {GPIO_SKL_H_GPP_H10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SML2CLK
  {GPIO_SKL_H_GPP_H11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SML2DATA
  {GPIO_SKL_H_GPP_H12, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //SML2ALERTB/NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_H13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioPlatformReset , GpioTermNone }}, //SML3CLK/HOME_BTN_R2
//  {GPIO_SKL_H_GPP_H14, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioPlatformReset , GpioTermNone }}, //SML3DATA/VOL_UP_R_R1
//  {GPIO_SKL_H_GPP_H15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntLevel     | GpioIntApic, GpioPlatformReset , GpioTermNone }}, //SML3ALERTB/VOL_DOWN_R_R1
  {GPIO_SKL_H_GPP_H13, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_H14, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_H15, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_H16, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //Not Used
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_H17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutHigh   , GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //SML4DATA/M.2_WIGIG_RST_R_N
  {GPIO_SKL_H_GPP_H17, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_H18, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_H19, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_H20, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_H21, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_I2C1_SCL/NOT USED
  {GPIO_SKL_H_GPP_H22, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //ISH_I2C1_SDA/NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_H23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDis      , GpioHostDeepReset   , GpioTermNone }}, //LED_DRIVE
  {GPIO_SKL_H_GPP_H23, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_I0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //DDSP_HPD_0
  {GPIO_SKL_H_GPP_I1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //DDSP_HPD_1
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_I2,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //DDSP_HPD_2
//  {GPIO_SKL_H_GPP_I3,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirInInv   , GpioOutDefault, GpioIntEdge      | GpioIntSmi, GpioPlatformReset , GpioTermNone }}, //DDSP_HPD_3/SMC_EXTSMI_R_N
  {GPIO_SKL_H_GPP_I2, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_I3, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPP_I4,  {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_I5,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //DDPB_CTRLCLK
  {GPIO_SKL_H_GPP_I6,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpd20K  }}, //DDPB_CTRLDATA
  {GPIO_SKL_H_GPP_I7,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //DDPC_CTRLCLK
  {GPIO_SKL_H_GPP_I8,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpd20K  }}, //DDPC_CTRLDATA
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPP_I9,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //DDPD_CTRLCLK
//  {GPIO_SKL_H_GPP_I10, {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpd20K  }}, //DDPD_CTRLDATA
  {GPIO_SKL_H_GPP_I9, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
  {GPIO_SKL_H_GPP_I10, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPD0,   {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //PM_BATLOW_R_N/NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPD1,   {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirOut     , GpioOutLow    , GpioIntDis      , GpioDswReset, GpioTermNone }}, //AC_PRESENT_R/USB_DSW_OVERRIDE_GPI0
  {GPIO_SKL_H_GPD1, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPD2,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntLevel     | GpioIntSci, GpioDswReset, GpioTermNative  }}, //LANWAKE_SMC_WAKE_SCI_N
  {GPIO_SKL_H_GPD3,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioDswReset, GpioTermWpu20K  }}, //PM_PWRBTN_R_N
  {GPIO_SKL_H_GPD4,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioDswReset, GpioTermNone }}, //SLP_S3_R_N
  {GPIO_SKL_H_GPD5,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioDswReset, GpioTermNone }}, //SLP_S4_R_N
  {GPIO_SKL_H_GPD6,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioDswReset, GpioTermNone }}, //SLP_M_R_N
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPD7,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioDswReset, GpioTermNone }}, //USB_WAKEOUT_N
  {GPIO_SKL_H_GPD7, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPD8,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioDswReset, GpioTermNone }}, //SUS_CLK
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPD9,   {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioDswReset, GpioTermNone }}, //PCH_SLP_WLAN_N
  {GPIO_SKL_H_GPD9, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
  {GPIO_SKL_H_GPD10,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioDswReset, GpioTermNone }}, //SLP_S5_R_N
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//  {GPIO_SKL_H_GPD11,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut     , GpioOutDefault, GpioIntDis      , GpioDswReset, GpioTermNone }}, //PM_LANPHY_ENABLE
  {GPIO_SKL_H_GPD11, {GpioPadModeGpio   , GpioHostOwnGpio   , GpioDirIn      , GpioOutDefault, GpioIntDis      , GpioPlatformReset , GpioTermNone }}, //NOT USED
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
};


#endif  // _GPIO_TABLE_DT_RVP8_CRB_H
