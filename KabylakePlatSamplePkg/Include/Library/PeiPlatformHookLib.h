/** @file
  @todo ADD DESCRIPTION

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

@par Specification Reference:
**/
#ifndef _PEI_PLATFORM_HOOK_LIB_H_
#define _PEI_PLATFORM_HOOK_LIB_H_

#include <PlatformInfo.h>
#include <Library/PeiServicesLib.h>
#include <SetupVariable.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>

//ZUMBA BEACH SKU ID
#define UP_B_ID_BIT0_GPIO_PAD                           GPIO_SKL_H_GPP_G12
#define UP_B_ID_BIT1_GPIO_PAD                           GPIO_SKL_H_GPP_G13
#define UP_B_ID_BIT2_GPIO_PAD                           GPIO_SKL_H_GPP_G14
//ZUMBA BEACH GPIO REV_ID
#define UP_REV_ID_BIT0_GPIO_PAD                         GPIO_SKL_H_GPP_G15
#define UP_REV_ID_BIT1_GPIO_PAD                         GPIO_SKL_H_GPP_G16

//Grizzly Mountain specific init GPIOs
#define GM_MIPI_RESET_N                                 GPIO_SKL_LP_GPP_E6
#define GM_MIPI_IO_PWREN1                               GPIO_SKL_LP_GPP_E8
#define GM_MIPI_CORE_PWREN2                             GPIO_SKL_LP_GPP_E11
#define GM_DISP_PWR_EN                                  GPIO_SKL_LP_GPP_E12
#define GM_SSD_RESET_N                                  GPIO_SKL_LP_GPP_G6

#ifdef __GNUC__
#pragma GCC diagnostic warning "-Wunused-variable"
#endif
static GPIO_INIT_CONFIG UpZumbaBeachGpioIdTable[] =
{
    {GPIO_SKL_H_GPP_G12, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_12_GSXDOUT
    {GPIO_SKL_H_GPP_G13, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_13_GSXSLOAD
    {GPIO_SKL_H_GPP_G14, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_14_GSXDIN
    {GPIO_SKL_H_GPP_G15, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_15_GSXSRESETB
    {GPIO_SKL_H_GPP_G16, {GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,    GpioPlatformReset,  GpioTermNone}},//GPP_G_16_GSXCLK

};

typedef struct {
  UINT32 IoExpanderNumber   : 1;  // IO Expander Number (0/1)
  UINT32 GpioPinNumber      : 5;  // GPIO Pin Number (0 to 23)
  UINT32 GpioDirection      : 1;  // GPIO Pin Direction (Input/Output)
  UINT32 GpioLevel          : 1;  // GPIO Pin Output Level (High/Low)
  UINT32 GpioInversion     : 1;  // GPIO Pin Inversion (Enabled/Disabled)
  UINT32 Reserved           : 23; // Reserved
} IO_EXPANDER_GPIO_CONFIG;

//SKL PCH LP GPIO Expander Number
#define IO_EXPANDER_0            0
#define IO_EXPANDER_1            1

//SKL PCH LP GPIO Pin Mapping
#define IO_EXPANDER_GPIO_0        0   // P00
#define IO_EXPANDER_GPIO_1        1   // P01
#define IO_EXPANDER_GPIO_2        2   // P02
#define IO_EXPANDER_GPIO_3        3   // P03
#define IO_EXPANDER_GPIO_4        4   // P04
#define IO_EXPANDER_GPIO_5        5   // P05
#define IO_EXPANDER_GPIO_6        6   // P06
#define IO_EXPANDER_GPIO_7        7   // P07
#define IO_EXPANDER_GPIO_8        8   // P10
#define IO_EXPANDER_GPIO_9        9   // P11
#define IO_EXPANDER_GPIO_10       10  // P12
#define IO_EXPANDER_GPIO_11       11  // P13
#define IO_EXPANDER_GPIO_12       12  // P14
#define IO_EXPANDER_GPIO_13       13  // P15
#define IO_EXPANDER_GPIO_14       14  // P16
#define IO_EXPANDER_GPIO_15       15  // P17
#define IO_EXPANDER_GPIO_16       16  // P20
#define IO_EXPANDER_GPIO_17       17  // P21
#define IO_EXPANDER_GPIO_18       18  // P22
#define IO_EXPANDER_GPIO_19       19  // P23
#define IO_EXPANDER_GPIO_20       20  // P24
#define IO_EXPANDER_GPIO_21       21  // P25
#define IO_EXPANDER_GPIO_22       22  // P26
#define IO_EXPANDER_GPIO_23       23  // P27

//SKL PCH LP GPIO Expander GPIO Direction
#define IO_EXPANDER_GPIO_OUTPUT   0
#define IO_EXPANDER_GPIO_INPUT    1

//SKL PCH LP GPIO Expaner GPIO Output Level
#define IO_EXPANDER_GPO_LEVEL_LOW    0
#define IO_EXPANDER_GPO_LEVEL_HIGH   1

//SKL PCH LP GPIO Expaner GPIO Inversion Status
#define IO_EXPANDER_GPI_INV_DISABLED  0
#define IO_EXPANDER_GPI_INV_ENABLED   1
#define IO_EXPANDER_GPIO_RESERVED     0x00

//GPIO Table Terminator
#define END_OF_GPIO_TABLE 0xFFFFFFFF

//EC Command to provide one byte of debug indication
#define BSSB_DEBUG_INDICATION 0xAE
#define COYOTE_MTN_TYPEC_HRESET 0x39
#ifndef MINTREE_FLAG
/**
  Configure EC for specific devices

  @param[in] PchLan       - The PchLan of PCH_SETUP variable.
  @param[in] BootMode     - The current boot mode.
**/
VOID
EcInit (
  IN UINT8                PchLan,
  IN EFI_BOOT_MODE        BootMode
  );

/**
  Pmic Voltage Selection as directed by User

  @param[in]  SystemConfiguration
**/
VOID
PmicVoltageSelect (
  IN SETUP_DATA                 *SetupData
  );

/**
  Checks if Premium PMIC present

  @retval  TRUE  if present
  @retval  FALSE it discrete/other PMIC
**/
BOOLEAN
IsPremiumPmicPresent (
  VOID
  );

/**
  Pmic Programming to supprort LPAL Feature

  @retval     NONE
**/
VOID
PremiumPmicDisableSlpS0Voltage (
  VOID
  );

/**
  Program the changes required for Thunderbolt(TM)

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor
  @param[in]  Interface

  @retval     EFI_SUCCESS       Operation success.
**/
EFI_STATUS
EFIAPI
ThunderboltCcUpdateNotify (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  );
#endif

/**
  Do platform specific programming pre-memory. For example, EC init, Chipset programming

  @retval  Status
**/
EFI_STATUS
PlatformSpecificInitPreMem (
  VOID
  );
#ifndef MINTREE_FLAG
/**
  Do platform specific programming post-memory.

  @retval  Status
**/
EFI_STATUS
PlatformSpecificInit (
  VOID
  );
#endif

/**
  figure out the CSM configutation and post it to PcdCsmEnable.

  @param[in]  Event     Event whose notification function is being invoked.
  @param[in]  Context   Pointer to the notification function's context.
**/
VOID
EFIAPI
GetCsmControlStatus (
    VOID
  );

/**
  Get platform information

  @param[out]  PlatformInfo   Get platforminfo from HOB

**/
EFI_STATUS
GetPlatformInfo (
  OUT PLATFORM_INFO                 **PlatformInfo
  );

/**
  Configure GPIO and SIO Before Memory is ready.

  @retval  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
BoardInitPreMem (
  VOID
  );

/**
  Configure GPIO and SIO

  @retval  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
BoardInit (
  VOID
  );
#ifndef MINTREE_FLAG
/**
Voltage Margining Routine

@retval  EFI_SUCCESS   Operation success
**/
EFI_STATUS
VoltageMarginingRoutine(
  VOID
  );
#endif

/**
  Detect recovery mode

  @retval  EFI_SUCCESS       System in Recovery Mode
  @retval  EFI_UNSUPPORTED   System doesn't support Recovery Mode
  @retval  EFI_NOT_FOUND     System is not in Recovery Mode
**/
EFI_STATUS
IsRecoveryMode (
  VOID
  );

#endif
