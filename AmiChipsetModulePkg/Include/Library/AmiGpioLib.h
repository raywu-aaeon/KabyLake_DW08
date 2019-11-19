//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiGpioLib.h
  This file contains South Bridge(LPC) chipset porting library GPIO 
  functions and data structure definitions for both PEI & DXE stages.
  
*/

#ifndef __AMI_GPIO_LIB_H__
#define __AMI_GPIO_LIB_H__

#include <Uefi.h>

#ifdef __cplusplus
extern "C" {
#endif

// Extra definition for SKL / KBL >>>
#define SB_LP_GPIO_GROUP_MAX     8
#define SB_H_GPIO_GROUP_MAX      10

//
// Structure for storing information about registers offset, community,
// maximal pad number for available groups
//
typedef struct {
  UINT32 Community;
  UINT32 PadOwnOffset;
  UINT32 HostOwnOffset;
  UINT32 GpiIsOffset;
  UINT32 GpiIeOffset;
  UINT32 GpiGpeStsOffset;
  UINT32 GpiGpeEnOffset;
  UINT32 SmiStsOffset;
  UINT32 SmiEnOffset;
  UINT32 NmiStsOffset;
  UINT32 NmiEnOffset;
  UINT32 PadCfgLockOffset;
  UINT32 PadCfgLockTxOffset;
  UINT32 PadCfgOffset;
  UINT32 PadPerGroup;
} AMI_GPIO_GROUP_INFO;

typedef enum {  
  H_Gpio_GPP_A = 0x0100,
  H_Gpio_GPP_B,
  H_Gpio_GPP_C,
  H_Gpio_GPP_D,
  H_Gpio_GPP_E,
  H_Gpio_GPP_F,
  H_Gpio_GPP_G,
  H_Gpio_GPP_H,
  H_Gpio_GPP_I,
  H_Gpio_GPD,
  Lp_Gpio_GPP_A = 0x0200,
  Lp_Gpio_GPP_B,
  Lp_Gpio_GPP_C,
  Lp_Gpio_GPP_D,
  Lp_Gpio_GPP_E,
  Lp_Gpio_GPP_F,
  Lp_Gpio_GPP_G,
  Lp_Gpio_GPD,
  Gpio_Group_Max
} AMI_GPIO_GROUP_TYPE;

typedef struct {
	UINT16 Offset;
	UINT32 Buffer32;
} SB_GPE0_DESC;
// Extra definition for SKL / KBL <<<

typedef enum {
	TYPE_HOT_PLUG = 0,
	TYPE_SWGPE,
	TYPE_TCOSCI,
	TYPE_RI,
	TYPE_PCI_EXP,
	TYPE_BATLOW,
	TYPE_PME,
	TYPE_ME_SCI,
	TYPE_PME_B0,
	TYPE_ESPI_SCI,
	TYPE_GPIO_TIER2_SCI,
	TYPE_WADT
} SB_GPE0_TYPE;

typedef enum {
  GPE0_EN_OP_SAVE,
  GPE0_EN_OP_RESTORE,
  GPE0_EN_OP_CLEAR_ALL
} GPE0_EN_OP;

//Need adjust >>>
typedef enum {
  PCONF0,
  PCONF1,
} GPIO_REG_TYPE;
//Need adjust <<<

typedef struct _AMI_OEM_GPIO {
  AMI_GPIO_GROUP_TYPE   Group;
  UINT8                 PinNum;
} AMI_OEM_GPIO;


BOOLEAN SbGetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbSetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbResetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbGpe0Operation (
    IN GPE0_EN_OP   Operation
);

BOOLEAN SbGetGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbClearGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbSetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

VOID SbResetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
);

VOID SbClearGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpioUseSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbProgGpioUseSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      GpioMode,
    IN UINT8        MulFunc
);

BOOLEAN SbGetGpioIoSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbSetGpioIoSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      InputMode
);

BOOLEAN SbGetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbSetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      High
);

UINT32 SbReadAltGpiSmiSts (
    VOID
);

VOID SbClearAltGpiSmiSts (
    IN AMI_OEM_GPIO Gpio
);

VOID SbProgramAltGpiSmi (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      Set
);

VOID SbProgramGpioRout (
    IN AMI_OEM_GPIO Gpio,
    IN UINT8        Mode
);

VOID SbProgramGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType,
    IN UINT32         AndData,
    IN UINT32         OrData
);

UINT32 SbReadGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType
);

// Extra function for SKL / KBL >>>
extern AMI_GPIO_GROUP_INFO SbLpGpioGroupInfo[];
extern AMI_GPIO_GROUP_INFO SbHGpioGroupInfo[];

UINT32 ConvertGpioGroupPad (
  IN AMI_OEM_GPIO Gpio
);

UINT32 SbReadAltGpiSmi( 
  IN AMI_OEM_GPIO   Gpio 
);

UINT32 SbReadAltGpiSmi( 
  IN AMI_OEM_GPIO   Gpio 
);

UINT32 SbReadGpiGppSmiSts( 
  IN AMI_OEM_GPIO   Gpio 
);

UINT32 SbReadGpioRout (
  IN AMI_OEM_GPIO Gpio
);

VOID SbProgramGpioGPE (
  IN AMI_OEM_GPIO Gpio
);
// Extra function for SKL / KBL <<<
#ifdef __cplusplus
}
#endif
#endif


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
