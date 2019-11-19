/** @file
  This file contains SKL specific GPIO information

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2016 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#include "GpioLibrary.h"

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO mPchLpGpioGroupInfo[] = {
  {PID_GPIOCOM0, R_PCH_LP_PCR_GPIO_GPP_A_PAD_OWN, R_PCH_PCR_GPIO_GPP_A_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_A_GPI_IS,    R_PCH_PCR_GPIO_GPP_A_GPI_IE,    R_PCH_PCR_GPIO_GPP_A_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_A_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_A_SMI_STS,  R_PCH_PCR_GPIO_GPP_A_SMI_EN,   R_PCH_PCR_GPIO_GPP_A_NMI_STS,  R_PCH_PCR_GPIO_GPP_A_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_A_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_A_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_A_PADCFG_OFFSET,    V_PCH_GPIO_GPP_A_PAD_MAX},    //SKL PCH-LP GPP_A
  {PID_GPIOCOM0, R_PCH_LP_PCR_GPIO_GPP_B_PAD_OWN, R_PCH_PCR_GPIO_GPP_B_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_B_GPI_IS,    R_PCH_PCR_GPIO_GPP_B_GPI_IE,    R_PCH_PCR_GPIO_GPP_B_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_B_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_B_SMI_STS,  R_PCH_PCR_GPIO_GPP_B_SMI_EN,   R_PCH_PCR_GPIO_GPP_B_NMI_STS,  R_PCH_PCR_GPIO_GPP_B_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_B_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_B_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_B_PADCFG_OFFSET,    V_PCH_GPIO_GPP_B_PAD_MAX},    //SKL PCH-LP GPP_B
  {PID_GPIOCOM1, R_PCH_LP_PCR_GPIO_GPP_C_PAD_OWN, R_PCH_PCR_GPIO_GPP_C_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_C_GPI_IS,    R_PCH_PCR_GPIO_GPP_C_GPI_IE,    R_PCH_PCR_GPIO_GPP_C_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_C_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_C_SMI_STS,  R_PCH_PCR_GPIO_GPP_C_SMI_EN,   R_PCH_PCR_GPIO_GPP_C_NMI_STS,  R_PCH_PCR_GPIO_GPP_C_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_C_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_C_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET,    V_PCH_GPIO_GPP_C_PAD_MAX},    //SKL PCH-LP GPP_C
  {PID_GPIOCOM1, R_PCH_LP_PCR_GPIO_GPP_D_PAD_OWN, R_PCH_PCR_GPIO_GPP_D_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_D_GPI_IS,    R_PCH_PCR_GPIO_GPP_D_GPI_IE,    R_PCH_PCR_GPIO_GPP_D_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_D_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_D_SMI_STS,  R_PCH_PCR_GPIO_GPP_D_SMI_EN,   R_PCH_PCR_GPIO_GPP_D_NMI_STS,  R_PCH_PCR_GPIO_GPP_D_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_D_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_D_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_D_PADCFG_OFFSET,    V_PCH_GPIO_GPP_D_PAD_MAX},    //SKL PCH-LP GPP_D
  {PID_GPIOCOM1, R_PCH_LP_PCR_GPIO_GPP_E_PAD_OWN, R_PCH_PCR_GPIO_GPP_E_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_E_GPI_IS,    R_PCH_PCR_GPIO_GPP_E_GPI_IE,    R_PCH_PCR_GPIO_GPP_E_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_E_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_E_SMI_STS,  R_PCH_PCR_GPIO_GPP_E_SMI_EN,   R_PCH_PCR_GPIO_GPP_E_NMI_STS,  R_PCH_PCR_GPIO_GPP_E_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_E_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_E_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_E_PADCFG_OFFSET,    V_PCH_LP_GPIO_GPP_E_PAD_MAX}, //SKL PCH-LP GPP_E
  {PID_GPIOCOM3, R_PCH_LP_PCR_GPIO_GPP_F_PAD_OWN, R_PCH_LP_PCR_GPIO_GPP_F_HOSTSW_OWN, R_PCH_LP_PCR_GPIO_GPP_F_GPI_IS, R_PCH_LP_PCR_GPIO_GPP_F_GPI_IE, R_PCH_LP_PCR_GPIO_GPP_F_GPI_GPE_STS,R_PCH_LP_PCR_GPIO_GPP_F_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      R_PCH_LP_PCR_GPIO_GPP_F_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_F_PADCFGLOCKTX, R_PCH_LP_PCR_GPIO_GPP_F_PADCFG_OFFSET, V_PCH_GPIO_GPP_F_PAD_MAX},    //SKL PCH-LP GPP_F
  {PID_GPIOCOM3, R_PCH_LP_PCR_GPIO_GPP_G_PAD_OWN, R_PCH_LP_PCR_GPIO_GPP_G_HOSTSW_OWN, R_PCH_LP_PCR_GPIO_GPP_G_GPI_IS, R_PCH_LP_PCR_GPIO_GPP_G_GPI_IE, R_PCH_LP_PCR_GPIO_GPP_G_GPI_GPE_STS,R_PCH_LP_PCR_GPIO_GPP_G_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      R_PCH_LP_PCR_GPIO_GPP_G_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_G_PADCFGLOCKTX, R_PCH_LP_PCR_GPIO_GPP_G_PADCFG_OFFSET, V_PCH_LP_GPIO_GPP_G_PAD_MAX}, //SKL PCH-LP GPP_G
  {PID_GPIOCOM2, R_PCH_LP_PCR_GPIO_GPD_PAD_OWN,   R_PCH_PCR_GPIO_GPD_HOSTSW_OWN,      R_PCH_PCR_GPIO_GPD_GPI_IS,      R_PCH_PCR_GPIO_GPD_GPI_IE,      R_PCH_PCR_GPIO_GPD_GPI_GPE_STS,     R_PCH_PCR_GPIO_GPD_GPI_GPE_EN,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      R_PCH_LP_PCR_GPIO_GPD_PADCFGLOCK,   R_PCH_LP_PCR_GPIO_GPD_PADCFGLOCKTX,   R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET,      V_PCH_GPIO_GPD_PAD_MAX},      //SKL PCH-LP GPD
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO mPchHGpioGroupInfo[] = {
  {PID_GPIOCOM0, R_PCH_H_PCR_GPIO_GPP_A_PAD_OWN,  R_PCH_PCR_GPIO_GPP_A_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_A_GPI_IS,    R_PCH_PCR_GPIO_GPP_A_GPI_IE,    R_PCH_PCR_GPIO_GPP_A_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_A_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_A_SMI_STS,  R_PCH_PCR_GPIO_GPP_A_SMI_EN,   R_PCH_PCR_GPIO_GPP_A_NMI_STS,  R_PCH_PCR_GPIO_GPP_A_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_A_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_A_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_A_PADCFG_OFFSET,    V_PCH_GPIO_GPP_A_PAD_MAX},    //SKL PCH-H GPP_A
  {PID_GPIOCOM0, R_PCH_H_PCR_GPIO_GPP_B_PAD_OWN,  R_PCH_PCR_GPIO_GPP_B_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_B_GPI_IS,    R_PCH_PCR_GPIO_GPP_B_GPI_IE,    R_PCH_PCR_GPIO_GPP_B_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_B_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_B_SMI_STS,  R_PCH_PCR_GPIO_GPP_B_SMI_EN,   R_PCH_PCR_GPIO_GPP_B_NMI_STS,  R_PCH_PCR_GPIO_GPP_B_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_B_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_B_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_B_PADCFG_OFFSET,    V_PCH_GPIO_GPP_B_PAD_MAX},    //SKL PCH-H GPP_B
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_C_PAD_OWN,  R_PCH_PCR_GPIO_GPP_C_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_C_GPI_IS,    R_PCH_PCR_GPIO_GPP_C_GPI_IE,    R_PCH_PCR_GPIO_GPP_C_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_C_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_C_SMI_STS,  R_PCH_PCR_GPIO_GPP_C_SMI_EN,   R_PCH_PCR_GPIO_GPP_C_NMI_STS,  R_PCH_PCR_GPIO_GPP_C_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_C_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_C_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET,    V_PCH_GPIO_GPP_C_PAD_MAX},    //SKL PCH-H GPP_C
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_D_PAD_OWN,  R_PCH_PCR_GPIO_GPP_D_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_D_GPI_IS,    R_PCH_PCR_GPIO_GPP_D_GPI_IE,    R_PCH_PCR_GPIO_GPP_D_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_D_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_D_SMI_STS,  R_PCH_PCR_GPIO_GPP_D_SMI_EN,   R_PCH_PCR_GPIO_GPP_D_NMI_STS,  R_PCH_PCR_GPIO_GPP_D_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_D_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_D_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_D_PADCFG_OFFSET,    V_PCH_GPIO_GPP_D_PAD_MAX},    //SKL PCH-H GPP_D
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_E_PAD_OWN,  R_PCH_PCR_GPIO_GPP_E_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_E_GPI_IS,    R_PCH_PCR_GPIO_GPP_E_GPI_IE,    R_PCH_PCR_GPIO_GPP_E_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_E_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_E_SMI_STS,  R_PCH_PCR_GPIO_GPP_E_SMI_EN,   R_PCH_PCR_GPIO_GPP_E_NMI_STS,  R_PCH_PCR_GPIO_GPP_E_NMI_EN,   R_PCH_H_PCR_GPIO_GPP_E_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_E_PADCFGLOCKTX,  R_PCH_PCR_GPIO_GPP_E_PADCFG_OFFSET,    V_PCH_H_GPIO_GPP_E_PAD_MAX},  //SKL PCH-H GPP_E
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_F_PAD_OWN,  R_PCH_H_PCR_GPIO_GPP_F_HOSTSW_OWN,  R_PCH_H_PCR_GPIO_GPP_F_GPI_IS,  R_PCH_H_PCR_GPIO_GPP_F_GPI_IE,  R_PCH_H_PCR_GPIO_GPP_F_GPI_GPE_STS, R_PCH_H_PCR_GPIO_GPP_F_GPI_GPE_EN,  R_PCH_H_PCR_GPIO_GPP_F_SMI_STS,R_PCH_H_PCR_GPIO_GPP_F_SMI_EN, R_PCH_H_PCR_GPIO_GPP_F_NMI_STS,R_PCH_H_PCR_GPIO_GPP_F_NMI_EN, R_PCH_H_PCR_GPIO_GPP_F_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_F_PADCFGLOCKTX,  R_PCH_H_PCR_GPIO_GPP_F_PADCFG_OFFSET,  V_PCH_GPIO_GPP_F_PAD_MAX},    //SKL PCH-H GPP_F
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_G_PAD_OWN,  R_PCH_H_PCR_GPIO_GPP_G_HOSTSW_OWN,  R_PCH_H_PCR_GPIO_GPP_G_GPI_IS,  R_PCH_H_PCR_GPIO_GPP_G_GPI_IE,  R_PCH_H_PCR_GPIO_GPP_G_GPI_GPE_STS, R_PCH_H_PCR_GPIO_GPP_G_GPI_GPE_EN,  R_PCH_H_PCR_GPIO_GPP_G_SMI_STS,R_PCH_H_PCR_GPIO_GPP_G_SMI_EN, R_PCH_H_PCR_GPIO_GPP_G_NMI_STS,R_PCH_H_PCR_GPIO_GPP_G_NMI_EN, R_PCH_H_PCR_GPIO_GPP_G_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_G_PADCFGLOCKTX,  R_PCH_H_PCR_GPIO_GPP_G_PADCFG_OFFSET,  V_PCH_H_GPIO_GPP_G_PAD_MAX},  //SKL PCH-H GPP_G
  {PID_GPIOCOM1, R_PCH_H_PCR_GPIO_GPP_H_PAD_OWN,  R_PCH_H_PCR_GPIO_GPP_H_HOSTSW_OWN,  R_PCH_H_PCR_GPIO_GPP_H_GPI_IS,  R_PCH_H_PCR_GPIO_GPP_H_GPI_IE,  R_PCH_H_PCR_GPIO_GPP_H_GPI_GPE_STS, R_PCH_H_PCR_GPIO_GPP_H_GPI_GPE_EN,  R_PCH_H_PCR_GPIO_GPP_H_SMI_STS,R_PCH_H_PCR_GPIO_GPP_H_SMI_EN, R_PCH_H_PCR_GPIO_GPP_H_NMI_STS,R_PCH_H_PCR_GPIO_GPP_H_NMI_EN, R_PCH_H_PCR_GPIO_GPP_H_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_H_PADCFGLOCKTX,  R_PCH_H_PCR_GPIO_GPP_H_PADCFG_OFFSET,  V_PCH_H_GPIO_GPP_H_PAD_MAX},  //SKL PCH-H GPP_H
  {PID_GPIOCOM3, R_PCH_H_PCR_GPIO_GPP_I_PAD_OWN,  R_PCH_H_PCR_GPIO_GPP_I_HOSTSW_OWN,  R_PCH_H_PCR_GPIO_GPP_I_GPI_IS,  R_PCH_H_PCR_GPIO_GPP_I_GPI_IE,  R_PCH_H_PCR_GPIO_GPP_I_GPI_GPE_STS, R_PCH_H_PCR_GPIO_GPP_I_GPI_GPE_EN,  R_PCH_H_PCR_GPIO_GPP_I_SMI_STS,R_PCH_H_PCR_GPIO_GPP_I_SMI_EN, R_PCH_H_PCR_GPIO_GPP_I_NMI_STS,R_PCH_H_PCR_GPIO_GPP_I_NMI_EN, R_PCH_H_PCR_GPIO_GPP_I_PADCFGLOCK,  R_PCH_H_PCR_GPIO_GPP_I_PADCFGLOCKTX,  R_PCH_H_PCR_GPIO_GPP_I_PADCFG_OFFSET,  V_PCH_H_GPIO_GPP_I_PAD_MAX},  //SKL PCH-H GPP_I
  {PID_GPIOCOM2, R_PCH_H_PCR_GPIO_GPD_PAD_OWN,    R_PCH_PCR_GPIO_GPD_HOSTSW_OWN,      R_PCH_PCR_GPIO_GPD_GPI_IS,      R_PCH_PCR_GPIO_GPD_GPI_IE,      R_PCH_PCR_GPIO_GPD_GPI_GPE_STS,     R_PCH_PCR_GPIO_GPD_GPI_GPE_EN,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      R_PCH_H_PCR_GPIO_GPD_PADCFGLOCK,    R_PCH_H_PCR_GPIO_GPD_PADCFGLOCKTX,    R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET,      V_PCH_GPIO_GPD_PAD_MAX}       //SKL PCH-H GPD
};

//
// I2C controller pins
// I2C[controller number][pin: SDA/SCL]
//
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchLpI2cGpio [PCH_LP_SERIALIO_MAX_I2C_CONTROLLERS][PCH_SERIAL_IO_PINS_PER_I2C_CONTROLLER]=
{
  {{GPIO_SKL_LP_GPP_C16, GpioPadModeNative1}, {GPIO_SKL_LP_GPP_C17, GpioPadModeNative1}},
  {{GPIO_SKL_LP_GPP_C18, GpioPadModeNative1}, {GPIO_SKL_LP_GPP_C19, GpioPadModeNative1}},
  {{GPIO_SKL_LP_GPP_F4,  GpioPadModeNative1}, {GPIO_SKL_LP_GPP_F5 , GpioPadModeNative1}},
  {{GPIO_SKL_LP_GPP_F6,  GpioPadModeNative1}, {GPIO_SKL_LP_GPP_F7 , GpioPadModeNative1}},
  {{GPIO_SKL_LP_GPP_F8,  GpioPadModeNative1}, {GPIO_SKL_LP_GPP_F9 , GpioPadModeNative1}},
  {{GPIO_SKL_LP_GPP_F10, GpioPadModeNative1}, {GPIO_SKL_LP_GPP_F11, GpioPadModeNative1}}
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchHI2cGpio [PCH_H_SERIALIO_MAX_I2C_CONTROLLERS][PCH_SERIAL_IO_PINS_PER_I2C_CONTROLLER]=
{
  {{GPIO_SKL_H_GPP_C16, GpioPadModeNative1}, {GPIO_SKL_H_GPP_C17, GpioPadModeNative1}}, //I2C0
  {{GPIO_SKL_H_GPP_C18, GpioPadModeNative1}, {GPIO_SKL_H_GPP_C19, GpioPadModeNative1}}, //I2C1
  {{GPIO_SKL_H_GPP_D13, GpioPadModeNative3}, {GPIO_SKL_H_GPP_D14, GpioPadModeNative3}}, //I2C2
  {{GPIO_SKL_H_GPP_D4,  GpioPadModeNative2}, {GPIO_SKL_H_GPP_D23, GpioPadModeNative2}}  //I2C3
};

//
// UART controller pins
// UART[controller number][pin: RXD/TXD/RTSB/CTSB]
//
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchLpUartGpio [PCH_SERIALIO_MAX_UART_CONTROLLERS][PCH_SERIAL_IO_PINS_PER_UART_CONTROLLER]=
{
  { // UART0
    {GPIO_SKL_LP_GPP_C8,  GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C9,  GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C10, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C11, GpioPadModeNative1}
  },
  { // UART1
    {GPIO_SKL_LP_GPP_C12, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C13, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C14, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C15, GpioPadModeNative1}
  },
  { // UART2
    {GPIO_SKL_LP_GPP_C20, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C21, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C22, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_C23, GpioPadModeNative1}
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchHUartGpio [PCH_SERIALIO_MAX_UART_CONTROLLERS][PCH_SERIAL_IO_PINS_PER_UART_CONTROLLER]=
{
  { // UART0
    {GPIO_SKL_H_GPP_C8,  GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C9,  GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C10, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C11, GpioPadModeNative1}
  },
  { // UART1
    {GPIO_SKL_H_GPP_C12, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C13, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C14, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C15, GpioPadModeNative1}
  },
  { // UART2
    {GPIO_SKL_H_GPP_C20, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C21, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C22, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_C23, GpioPadModeNative1}
  }
};

//
// SPI controller pins
// SPI[controller number][pin: CSB/CLK/MISO/MOSI]
//
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchLpSpiGpio [PCH_SERIALIO_MAX_SPI_CONTROLLERS][PCH_SERIAL_IO_PINS_PER_SPI_CONTROLLER]=
{
  { // SPI0
    {GPIO_SKL_LP_GPP_B15, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_B16, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_B17, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_B18, GpioPadModeNative1}
  },
  { // SPI1
    {GPIO_SKL_LP_GPP_B19, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_B20, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_B21, GpioPadModeNative1},
    {GPIO_SKL_LP_GPP_B22, GpioPadModeNative1}
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchHSpiGpio [PCH_SERIALIO_MAX_SPI_CONTROLLERS][PCH_SERIAL_IO_PINS_PER_SPI_CONTROLLER]=
{
  { // SPI0
    {GPIO_SKL_H_GPP_B15, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_B16, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_B17, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_B18, GpioPadModeNative1}
  },
  { // SPI1
    {GPIO_SKL_H_GPP_B19, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_B20, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_B21, GpioPadModeNative1},
    {GPIO_SKL_H_GPP_B22, GpioPadModeNative1}
  }
};

//
// SATA reset port to GPIO pin mapping
// SATAGP_x -> GPIO pin y
//
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchLpSataPortResetToGpioMap[PCH_LP_AHCI_MAX_PORTS] =
{
  {GPIO_SKL_LP_GPP_E0, GpioPadModeNative2},
  {GPIO_SKL_LP_GPP_E1, GpioPadModeNative2},
  {GPIO_SKL_LP_GPP_E2, GpioPadModeNative2}
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchHSataPortResetToGpioMap[PCH_H_AHCI_MAX_PORTS]  =
{
  {GPIO_SKL_H_GPP_E0, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_E1, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_E2, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F0, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F1, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F2, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F3, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F4, GpioPadModeNative2}
};

//
// SATADevSlpPin to GPIO pin mapping
// SATA_DEVSLP_x -> GPIO pin y
//
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchLpSataDevSlpPinToGpioMap[PCH_LP_AHCI_MAX_PORTS] =
{
  {GPIO_SKL_LP_GPP_E4, GpioPadModeNative1},
  {GPIO_SKL_LP_GPP_E5, GpioPadModeNative1},
  {GPIO_SKL_LP_GPP_E6, GpioPadModeNative1}
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_PAD_NATIVE_FUNCTION mPchHSataDevSlpPinToGpioMap[PCH_H_AHCI_MAX_PORTS] =
{
  {GPIO_SKL_H_GPP_E4, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_E5, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_E6, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F5, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F6, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F7, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F8, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F9, GpioPadModeNative1}
};
