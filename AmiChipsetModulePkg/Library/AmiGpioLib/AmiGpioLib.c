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

/** @file AmiGpioLib.c
    South Bridge Chipset GPIO Library implementation

*/

#include <Library/AmiGpioLib.h>
#include "Token.h"
#include <Base.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/GpioLib.h>
#include <Library/AmiChipsetIoLib.h>

#ifndef NO_REGISTER_FOR_PROPERTY
#define NO_REGISTER_FOR_PROPERTY (~0u)
#endif
//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
GLOBAL_REMOVE_IF_UNREFERENCED AMI_GPIO_GROUP_INFO SbLpGpioGroupInfo[] = {
  {PID_GPIOCOM0, R_PCH_LP_PCR_GPIO_GPP_A_PAD_OWN, R_PCH_PCR_GPIO_GPP_A_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_A_GPI_IS,    R_PCH_PCR_GPIO_GPP_A_GPI_IE,    R_PCH_PCR_GPIO_GPP_A_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_A_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_A_SMI_STS,  R_PCH_PCR_GPIO_GPP_A_SMI_EN,   R_PCH_PCR_GPIO_GPP_A_NMI_STS,  R_PCH_PCR_GPIO_GPP_A_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_A_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_A_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_A_PADCFG_OFFSET,    V_PCH_GPIO_GPP_A_PAD_MAX},    //SKL PCH-LP GPP_A
  {PID_GPIOCOM0, R_PCH_LP_PCR_GPIO_GPP_B_PAD_OWN, R_PCH_PCR_GPIO_GPP_B_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_B_GPI_IS,    R_PCH_PCR_GPIO_GPP_B_GPI_IE,    R_PCH_PCR_GPIO_GPP_B_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_B_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_B_SMI_STS,  R_PCH_PCR_GPIO_GPP_B_SMI_EN,   R_PCH_PCR_GPIO_GPP_B_NMI_STS,  R_PCH_PCR_GPIO_GPP_B_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_B_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_B_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_B_PADCFG_OFFSET,    V_PCH_GPIO_GPP_B_PAD_MAX},    //SKL PCH-LP GPP_B
  {PID_GPIOCOM1, R_PCH_LP_PCR_GPIO_GPP_C_PAD_OWN, R_PCH_PCR_GPIO_GPP_C_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_C_GPI_IS,    R_PCH_PCR_GPIO_GPP_C_GPI_IE,    R_PCH_PCR_GPIO_GPP_C_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_C_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_C_SMI_STS,  R_PCH_PCR_GPIO_GPP_C_SMI_EN,   R_PCH_PCR_GPIO_GPP_C_NMI_STS,  R_PCH_PCR_GPIO_GPP_C_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_C_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_C_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET,    V_PCH_GPIO_GPP_C_PAD_MAX},    //SKL PCH-LP GPP_C
  {PID_GPIOCOM1, R_PCH_LP_PCR_GPIO_GPP_D_PAD_OWN, R_PCH_PCR_GPIO_GPP_D_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_D_GPI_IS,    R_PCH_PCR_GPIO_GPP_D_GPI_IE,    R_PCH_PCR_GPIO_GPP_D_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_D_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_D_SMI_STS,  R_PCH_PCR_GPIO_GPP_D_SMI_EN,   R_PCH_PCR_GPIO_GPP_D_NMI_STS,  R_PCH_PCR_GPIO_GPP_D_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_D_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_D_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_D_PADCFG_OFFSET,    V_PCH_GPIO_GPP_D_PAD_MAX},    //SKL PCH-LP GPP_D
  {PID_GPIOCOM1, R_PCH_LP_PCR_GPIO_GPP_E_PAD_OWN, R_PCH_PCR_GPIO_GPP_E_HOSTSW_OWN,    R_PCH_PCR_GPIO_GPP_E_GPI_IS,    R_PCH_PCR_GPIO_GPP_E_GPI_IE,    R_PCH_PCR_GPIO_GPP_E_GPI_GPE_STS,   R_PCH_PCR_GPIO_GPP_E_GPI_GPE_EN,    R_PCH_PCR_GPIO_GPP_E_SMI_STS,  R_PCH_PCR_GPIO_GPP_E_SMI_EN,   R_PCH_PCR_GPIO_GPP_E_NMI_STS,  R_PCH_PCR_GPIO_GPP_E_NMI_EN,   R_PCH_LP_PCR_GPIO_GPP_E_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_E_PADCFGLOCKTX, R_PCH_PCR_GPIO_GPP_E_PADCFG_OFFSET,    V_PCH_LP_GPIO_GPP_E_PAD_MAX}, //SKL PCH-LP GPP_E
  {PID_GPIOCOM3, R_PCH_LP_PCR_GPIO_GPP_F_PAD_OWN, R_PCH_LP_PCR_GPIO_GPP_F_HOSTSW_OWN, R_PCH_LP_PCR_GPIO_GPP_F_GPI_IS, R_PCH_LP_PCR_GPIO_GPP_F_GPI_IE, R_PCH_LP_PCR_GPIO_GPP_F_GPI_GPE_STS,R_PCH_LP_PCR_GPIO_GPP_F_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      R_PCH_LP_PCR_GPIO_GPP_F_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_F_PADCFGLOCKTX, R_PCH_LP_PCR_GPIO_GPP_F_PADCFG_OFFSET, V_PCH_GPIO_GPP_F_PAD_MAX},    //SKL PCH-LP GPP_F
  {PID_GPIOCOM3, R_PCH_LP_PCR_GPIO_GPP_G_PAD_OWN, R_PCH_LP_PCR_GPIO_GPP_G_HOSTSW_OWN, R_PCH_LP_PCR_GPIO_GPP_G_GPI_IS, R_PCH_LP_PCR_GPIO_GPP_G_GPI_IE, R_PCH_LP_PCR_GPIO_GPP_G_GPI_GPE_STS,R_PCH_LP_PCR_GPIO_GPP_G_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      R_PCH_LP_PCR_GPIO_GPP_G_PADCFGLOCK, R_PCH_LP_PCR_GPIO_GPP_G_PADCFGLOCKTX, R_PCH_LP_PCR_GPIO_GPP_G_PADCFG_OFFSET, V_PCH_LP_GPIO_GPP_G_PAD_MAX}, //SKL PCH-LP GPP_G
  {PID_GPIOCOM2, R_PCH_LP_PCR_GPIO_GPD_PAD_OWN,   R_PCH_PCR_GPIO_GPD_HOSTSW_OWN,      R_PCH_PCR_GPIO_GPD_GPI_IS,      R_PCH_PCR_GPIO_GPD_GPI_IE,      R_PCH_PCR_GPIO_GPD_GPI_GPE_STS,     R_PCH_PCR_GPIO_GPD_GPI_GPE_EN,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,      R_PCH_LP_PCR_GPIO_GPD_PADCFGLOCK,   R_PCH_LP_PCR_GPIO_GPD_PADCFGLOCKTX,   R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET,      V_PCH_GPIO_GPD_PAD_MAX},      //SKL PCH-LP GPD
};

GLOBAL_REMOVE_IF_UNREFERENCED AMI_GPIO_GROUP_INFO SbHGpioGroupInfo[] = {
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

UINT32  gGpe0EnBackup = 0;
BOOLEAN gSaved = FALSE;
// GUID Definition(s)

// Protocol/Ppi Definition(s)

// External Declaration(s)

// Function Definition(s)
SB_GPE0_DESC BuildGpe0Desc (
  IN SB_GPE0_TYPE Gpe0Type,
  IN BOOLEAN          Gpe0En
);

BOOLEAN
SbGpioCheckFor2Tier (
    IN AMI_OEM_GPIO Gpio 
);
//---------------------------------------------------------------------------

/**
    Get the enable bit setting in GPE0 per Gpe0Type

    @param Gpe0Type SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval TRUE = Enabled / FALSE = Disabled

**/
BOOLEAN SbGetGpe0En (
  IN SB_GPE0_TYPE Gpe0Type
)
{
    SB_GPE0_DESC SbGpe0Desc;
    
    SbGpe0Desc = BuildGpe0Desc (Gpe0Type, TRUE);
    if (SbGpe0Desc.Buffer32 == 0xFFFFFFFF) return FALSE;
    //if (READ_IO32_PM (SbGpe0Desc.Offset) & SbGpe0Desc.Buffer32)
    if (READ_IO32(PM_BASE_ADDRESS + SbGpe0Desc.Offset) & SbGpe0Desc.Buffer32)
        return TRUE;
    else
        return FALSE;
}

/**
    Set the enable bit in GPE0 per Gpe0Type

    @param Gpe0Type SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID

**/
VOID SbSetGpe0En (
  IN SB_GPE0_TYPE Gpe0Type
)
{
    SB_GPE0_DESC SbGpe0Desc;
    
    SbGpe0Desc = BuildGpe0Desc (Gpe0Type, TRUE);
    if (SbGpe0Desc.Buffer32 == 0xFFFFFFFF) return;
    SET_IO32 (PM_BASE_ADDRESS + SbGpe0Desc.Offset, SbGpe0Desc.Buffer32);
}

/**
    Reset the enable bit in GPE0 per Gpe0Type

    @param Gpe0Type SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID

**/
VOID SbResetGpe0En (
  IN SB_GPE0_TYPE Gpe0Type
)
{
    SB_GPE0_DESC SbGpe0Desc;
    
    SbGpe0Desc = BuildGpe0Desc (Gpe0Type, TRUE);
    if (SbGpe0Desc.Buffer32 == 0xFFFFFFFF) return;
    RESET_IO32 (PM_BASE_ADDRESS + SbGpe0Desc.Offset, SbGpe0Desc.Buffer32);
}

/**
    Save/Restore/ClearAll GPE0 register per Operation

    @param Operation  GPE0_EN_OP. GPE0_EN_OP_SAVE, GPE0_EN_OP_RESTORE,
                      GPE0_EN_OP_CLEAR_ALL.

    @retval VOID

**/
VOID SbGpe0Operation (
  IN GPE0_EN_OP Operation
)
{
    UINT32 PchGpe0EnMask = ( B_PCH_ACPI_GPE0_EN_127_96_HOT_PLUG | \
                             B_PCH_ACPI_GPE0_EN_127_96_SWGPE    | \
                             B_PCH_ACPI_GPE0_EN_127_96_TC0SCI   | \
                             B_PCH_ACPI_GPE0_EN_127_96_RI       | \
                             B_PCH_ACPI_GPE0_EN_127_96_PCI_EXP  | \
                             B_PCH_ACPI_GPE0_EN_127_96_BATLOW   | \
                             B_PCH_ACPI_GPE0_EN_127_96_PME      | \
                             B_PCH_ACPI_GPE0_EN_127_96_ME_SCI   | \
                             B_PCH_ACPI_GPE0_EN_127_96_PME_B0   | \
                             B_PCH_ACPI_GPE0_EN_127_96_WADT);

    switch (Operation) {
        case GPE0_EN_OP_SAVE:
            if (!gSaved) {
                gGpe0EnBackup = READ_IO32 (PM_BASE_ADDRESS + R_PCH_ACPI_GPE0_EN_127_96) & PchGpe0EnMask;
                gSaved = TRUE;
            } // end if
            break;
        case GPE0_EN_OP_RESTORE:
            if (gSaved) {
                RW_IO32 (PM_BASE_ADDRESS + R_PCH_ACPI_GPE0_EN_127_96, gGpe0EnBackup, PchGpe0EnMask);
                gSaved = FALSE;
            } // end if
            break;
        case GPE0_EN_OP_CLEAR_ALL:
            RESET_IO32 (PM_BASE_ADDRESS + R_PCH_ACPI_GPE0_EN_127_96, PchGpe0EnMask);
            break;
        default:
            break;
    } // end of switch
}

/**
    Get the status bit setting in GPE0 per GPE0 Type

    @param Gpe0Type SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval TRUE = Set / FALSE= Clear

**/
BOOLEAN SbGetGpe0Sts (
  IN SB_GPE0_TYPE Gpe0Type
)
{
    SB_GPE0_DESC SbGpe0Desc;
    
    SbGpe0Desc = BuildGpe0Desc (Gpe0Type, FALSE);
    if (SbGpe0Desc.Buffer32 == 0xFFFFFFFF) return FALSE;
    if (READ_IO32 (PM_BASE_ADDRESS + SbGpe0Desc.Offset) & SbGpe0Desc.Buffer32)
        return TRUE;
    else
        return FALSE;
}

/**
    Clear the status bit in GPE0 per GPE0 Type

    @param Gpe0Type SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID 

**/
VOID SbClearGpe0Sts (
  IN SB_GPE0_TYPE Gpe0Type
)
{
    SB_GPE0_DESC SbGpe0Desc;

    SbGpe0Desc = BuildGpe0Desc (Gpe0Type, FALSE);
    if (SbGpe0Desc.Buffer32 == 0xFFFFFFFF) return;
    WRITE_IO32 (PM_BASE_ADDRESS + SbGpe0Desc.Offset, SbGpe0Desc.Buffer32);
}

/**
    Set GPI Pin Enable bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbSetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;

    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    // Check if group has GPI GPE register
    if (GpioGroupInfo[GroupIndex].GpiGpeEnOffset == NO_REGISTER_FOR_PROPERTY) {
        return;
    }
    
    MmioWrite32 (
          PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeEnOffset),
          (UINT32)(BIT0 << Gpio.PinNum)
          );
    
    // Check for 2-tier
    if(SbGpioCheckFor2Tier(Gpio)) {
        SET_IO32(PM_BASE_ADDRESS + R_PCH_ACPI_GPE0_EN_127_96, BIT15);
    }   
}

/**
    Get GPI Pin Enable bit setting in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE = Enabled / FALSE= Disabled

**/
BOOLEAN SbGetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               Data32;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    // Check if group has GPI GPE register
    if (GpioGroupInfo[GroupIndex].GpiGpeEnOffset == NO_REGISTER_FOR_PROPERTY) {
        return FALSE;
    }
    
    Data32 = MmioRead32 (
                PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeEnOffset));
            
    if (Data32 & (BIT0 << Gpio.PinNum)) return TRUE;
        else return FALSE;
}

/**
    Reset GPI Pin Enable bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbResetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               Data32, Mask32;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    // Check if group has GPI GPE register
    if (GpioGroupInfo[GroupIndex].GpiGpeEnOffset == NO_REGISTER_FOR_PROPERTY) {
        return;
    }

    Data32 = MmioRead32 (
                 PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeEnOffset));
    Mask32 = Data32 & (BIT0 << Gpio.PinNum);
    Data32 &= ~Mask32; 
    MmioWrite32 (
              PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeEnOffset),
              Data32 );
}

/**
    Get GPI Pin Status bit setting in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
BOOLEAN SbGetGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32      GroupIndex;
    UINT32      Data32;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }    
    GroupIndex = (Gpio.Group) & 0xFF;
    
    Data32 = MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeStsOffset));
    
    if (Data32 & (BIT0 << Gpio.PinNum)) return TRUE;
    else return FALSE;
}

/**
    Clear GPI Pin Status bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbClearGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;

    MmioWrite32 (
        PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeStsOffset),
        (UINT32)(BIT0 << Gpio.PinNum)
        );    
    
    // Check for 2-tier
    if(SbGpioCheckFor2Tier(Gpio)) {
        WRITE_IO32(PM_BASE_ADDRESS + R_PCH_ACPI_GPE0_STS_127_96, BIT15);
    }   
}

/**
    Get GPIO Use Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE = GPIO MODE / FALSE= NATIVE MODE

**/
BOOLEAN SbGetGpioUseSel (
  IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    
    if (GpioCfgRegVal & (BIT10 | BIT11)){
        // Native MODE
        return FALSE;
    } else {
        // GPIO MODE
        return TRUE;
    }
    
}

/**
    Program GPIO Use Setting

    @param Gpio - Define the group and Pin# of the GPIO
    @param GpioMode - TRUE = GPIO MODE / FALSE= NATIVE MODE
    @param MulFunc - While this pin is a multi-function pin and GpioMode
                     is Native Mode, BIOS will refer to MulFunc to
                     determine which native function this pin will be
                     used.

    @retval VOID

**/
VOID SbProgGpioUseSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      GpioMode,
    IN UINT8        MulFunc
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    
    if (GpioMode){
        GpioCfgRegVal &= ~(BIT10 | BIT11);
        MmioWrite32(
                PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)),
                GpioCfgRegVal);
    }
}

/**
    Get GPIO I/O Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE : Input Mode / FALSE : Output Mode

	KBL/SKL retval : 0 = Output Mode 
	                 1 = Input Mode
	                 2 = InOut Mode 
	                 FF = Error
**/
UINT8 SbGetGpioIoSel (
  IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    
    if(GpioCfgRegVal == 0xFFFFFFFF)
        return 0xFF;
    
    if((GpioCfgRegVal &(BIT8 | BIT9)) == 0){
        return 2;
    } else if (GpioCfgRegVal & BIT8){
        // Input
        return 1;
    } else if (GpioCfgRegVal & BIT9){
        // Output
        return 0;
    } else {
        return 0xFF;
    }
}

/**
    Set GPIO I/O Setting

    @param Gpio - Define the group and Pin# of the GPIO
    @param InputMode - TRUE : Input Mode / FALSE : Output Mode

    @retval VOID

**/
VOID SbSetGpioIoSel (
  IN AMI_OEM_GPIO Gpio,
  IN BOOLEAN  InputMode
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    
    if(GpioCfgRegVal == 0xFFFFFFFF)
        return;
    
    GpioCfgRegVal &= ~(BIT8 | BIT9);
    if (InputMode){
        GpioCfgRegVal |= BIT8;
    } else {
        GpioCfgRegVal |= BIT9;
    }
    
    MmioWrite32(
            PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)),
            GpioCfgRegVal);
}

/**
    Get GPIO Level Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE: High level / FALSE: Low level

**/
BOOLEAN SbGetGpioLvlSel (
  IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    
    if (!(GpioCfgRegVal & BIT8)) {
        // Gpio Output
        if (GpioCfgRegVal & BIT0){
            // Output High
            return TRUE;
        } else {
            // Output Low
            return FALSE;
        }
    }
    
    if (!(GpioCfgRegVal & BIT9)) {
        // Gpio Input
        if (GpioCfgRegVal & BIT1){
            // Output High
            return TRUE;
        } else {
            // Output Low
            return FALSE;
        }
    }
    
    // Unexpected state
    return FALSE;
}

/**
    Set GPIO Level

    @param Gpio - Define the group and Pin# of the GPIO
    @param High - TRUE: Set to High level / FALSE: Set to Low level

    @retval VOID

**/
VOID SbSetGpioLvlSel (
  IN AMI_OEM_GPIO Gpio,
  IN BOOLEAN  High
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    
    if (High){
        GpioCfgRegVal |= BIT0;
    } else {
        GpioCfgRegVal &= ~BIT0;
    }
    
    MmioWrite32(
            PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)),
            GpioCfgRegVal);
}

/**
    Read Alternate GPI SMI Status Register

    @param VOID

    @return The value read from Alternate GPI SMI Status Register

**/
UINT32 SbReadAltGpiSmiSts (
    VOID
)
{
    // not support 
    return 0xFFFFFFFF;
}

/**
    Clear Alternate GPI SMI Status

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbClearAltGpiSmiSts( IN AMI_OEM_GPIO   Gpio )
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioSmiStsRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioSmiStsRegVal = (UINT32) (1 << Gpio.PinNum);
    
    MmioWrite32(
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiStsOffset),
            GpioSmiStsRegVal);
}

/**
    Program Alternate GPI SMI Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param Set - TRUE: Set / FALSE: Clear corresponding Alternate GPI SMI
                 Enable bit.

    @retval VOID

**/
VOID SbProgramAltGpiSmi(
    IN AMI_OEM_GPIO   Gpio,
    IN BOOLEAN  Set
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioSmiEnRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioSmiEnRegVal = MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset));
    
    GpioSmiEnRegVal &= (UINT32)~(1 << Gpio.PinNum);
    if (Set){
        GpioSmiEnRegVal |= (1 << Gpio.PinNum);
    }
    
    MmioWrite32(
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset),
            GpioSmiEnRegVal);
}

/**
    Program GPIO Rout

    @param Gpio - Define the group and Pin# of the GPIO
    @param Mode - NO_EFFECT/SMI_MODE/SCI_MODE/NMI_MODE

    @retval VOID
    
    KBL/SKL Mode : BIT0 - NMI 
                   BIT1 - SMI
                   BIT2 - SCI
                   BIT3 - IOxAPIC
**/
VOID SbProgramGpioRout (
  IN AMI_OEM_GPIO Gpio,
  IN UINT8 Mode
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    
    GpioCfgRegVal &= ~(BIT17 | BIT18 | BIT19 | BIT20);
    
    GpioCfgRegVal |= (UINT32)(Mode << 17);
    
    MmioWrite32(
            PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)),
            GpioCfgRegVal);
}

/**
    Program GPIO Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param RegType - The register type which is going to read
    @param AndData - The value to AND with read value from the GPIO register.
    @param OrData - The value to OR with the result of the AND operation.

    @retval VOID

**/
VOID SbProgramGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType,
    IN UINT32         AndData,
    IN UINT32         OrData
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    if (RegType == PCONF0){
        GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    } else {
        GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum + 4)));
    }
    
    GpioCfgRegVal &= AndData;
    GpioCfgRegVal |= OrData;
    
    if (RegType == PCONF0){
        MmioWrite32(
                PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)),
                GpioCfgRegVal);
    } else {
        MmioWrite32(
                PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum + 4)),
                GpioCfgRegVal);
    }
}

/**
    Read GPIO Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param RegType - The register type which is going to read

    @retval 0xFFFFFFFF - Failed to read the register
    @retval Others - The data read from the register

**/
UINT32 SbReadGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    if (RegType == PCONF0){
        GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    } else {
        GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum + 4)));
    }
    return GpioCfgRegVal;
}

//---------------------------------------------------------------------------
// Extra function for SKL / KBL 

/**
  This procedure is used to convert Gpio group and pad value

  @param[in]  AMI_OEM_GPIO        GPIO pad

  @retval     Gpio GroupPad value
**/
UINT32 ConvertGpioGroupPad (
    IN AMI_OEM_GPIO Gpio)
{
    UINT32 ConvertValue = 0;
    if (Gpio.Group < Gpio_Group_Max)
    {
        ConvertValue = (UINT32)(Gpio.Group << 16);
    } else {
        ConvertValue = 0xFFFFFFFF;
    }
    
    if (ConvertValue != 0xFFFFFFFF){
        ConvertValue |= (UINT32) Gpio.PinNum;
    }
    
    return ConvertValue;
}

/**
  This procedure will get GPE number for provided GpioPad.
  PCH allows to configure mapping between GPIO groups and related GPE (GpioSetGroupToGpeDwX())
  what results in the fact that certain Pad can cause different General Purpose Event. Only three
  GPIO groups can be mapped to cause unique GPE (1-tier), all others groups will be under one common
  event (GPE_111 for 2-tier).

  1-tier:
  Returned GpeNumber is in range <0,95>. GpioGetGpeNumber() can be used
  to determine what _LXX ACPI method would be called on event on selected GPIO pad

  2-tier:
  Returned GpeNumber is 0x6F (111). All GPIO pads which are not mapped to 1-tier GPE
  will be under one master GPE_111 which is linked to _L6F ACPI method. If it is needed to determine
  what Pad from 2-tier has caused the event, _L6F method should check GPI_GPE_STS and GPI_GPE_EN
  registers for all GPIO groups not mapped to 1-tier GPE.

  @param[in]  AMI_OEM_GPIO        GPIO pad
  @param[out] GpeNumber           GPE number

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid group or pad number
**/
EFI_STATUS
GetGpiGpeNumber (
    IN AMI_OEM_GPIO         Gpio,
    OUT UINT32              *GpeNumber )
{
    GPIO_GROUP           GroupToGpeDw0;
    GPIO_GROUP           GroupToGpeDw1;
    GPIO_GROUP           GroupToGpeDw2;
    GPIO_GROUP           GpioGroupLowest;
    GPIO_GROUP           GpioGroupHighest;
    UINT32               GroupIndex;
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;

    if (GetPchSeries() == PchLp){
          GpioGroupInfo = SbLpGpioGroupInfo;
          GpioGroupLowest  = Lp_Gpio_GPP_A;
          GpioGroupHighest = Lp_Gpio_GPD;
    } else {
          GpioGroupInfo = SbHGpioGroupInfo;
          GpioGroupLowest  = H_Gpio_GPP_A;
          GpioGroupHighest = H_Gpio_GPD;
    }
    GroupIndex = (Gpio.Group) & 0xFF;

    //
    // Check if group argument exceeds GPIO group range
    //
    if ((GroupIndex < (GpioGroupLowest & 0xFF)) || (GroupIndex > (GpioGroupHighest & 0xFF))) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Check if legal pad number
    //
    if (Gpio.PinNum >= GpioGroupInfo[GroupIndex].PadPerGroup) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Get GPIO groups mapping to 1-tier GPE
    //
    GpioGetGroupToGpeDwX (&GroupToGpeDw0,&GroupToGpeDw1,&GroupToGpeDw2);

    if (Gpio.Group == GroupToGpeDw0) {
        *GpeNumber = Gpio.PinNum;
    } else if (Gpio.Group == GroupToGpeDw1) {
        *GpeNumber = Gpio.PinNum + 32;
    } else if (Gpio.Group == GroupToGpeDw2) {
        *GpeNumber = Gpio.PinNum + 64;
    } else {
        //
        // If Group number doesn't match any of above then
        // it means than certain pad is routed to 2-tier GPE
        // which all are under GPE_111 (0x6F)
        //
        *GpeNumber = PCH_GPIO_2_TIER_MASTER_GPE_NUMBER;
    }
    
    return EFI_SUCCESS;
}

/**
  This procedure is used to check GPIO inputs belongs to 2 tier or 1 tier architecture

  @param[in]  AMI_OEM_GPIO        GPIO pad

  @retval     Data                0 means 1-tier, 1 means 2-tier
**/
BOOLEAN
SbGpioCheckFor2Tier (
    IN AMI_OEM_GPIO Gpio )
{
    UINT32               Data32;
    
    GetGpiGpeNumber (Gpio, &Data32);
    if(Data32 == PCH_GPIO_2_TIER_MASTER_GPE_NUMBER) {
        return TRUE;
    }
    
    return FALSE;
}

/**
  This procedure is used to program GPIO to enable GPI_GPE_EN

  @param[in]  AMI_OEM_GPIO        GPIO pad

**/
VOID SbProgramGpioGPE (
  IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    UINT32               Mask;
    
    SbProgramGpioRout(Gpio, BIT2);
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }    
    GroupIndex = (Gpio.Group) & 0xFF;
    
    Mask = 0x1 << Gpio.PinNum;
    
    // Clear GPI_IE
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiIeOffset));
    GpioCfgRegVal &= ~Mask;
    MmioWrite32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiIeOffset), GpioCfgRegVal);

    // Clear SMI_EN
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset));
    GpioCfgRegVal &= ~Mask;
    MmioWrite32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset), GpioCfgRegVal);
    
    // Clear NMI_EN
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiEnOffset));
    GpioCfgRegVal &= ~Mask;
    MmioWrite32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiEnOffset), GpioCfgRegVal);      
    
    // Enable GPI_GPE_EN
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeEnOffset));
    GpioCfgRegVal |= Mask;
    MmioWrite32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeEnOffset), GpioCfgRegVal);   
}

/**
  This procedure is used to program GPIO to enable GPI_GPE_EN

  @param[in]  AMI_OEM_GPIO        GPIO pad

**/
VOID SbProgramGpioSMI (
  IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    UINT32               Mask;
    
    SbProgramGpioRout(Gpio, BIT1);
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }    
    GroupIndex = (Gpio.Group) & 0xFF;
    
    Mask = 0x1 << Gpio.PinNum;
    
    // Clear SMI_EN
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset));
    GpioCfgRegVal |= Mask;
    MmioWrite32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset), GpioCfgRegVal);
}

//
// Input : PinNum: GPIO Pin#
// Output: UINT32: Corresponding GPIO Rout Register value.
//
UINT32 SbReadGpioRout (
  IN AMI_OEM_GPIO Gpio
)
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioCfgRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioCfgRegVal = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, (GpioGroupInfo[GroupIndex].PadCfgOffset + 8 * Gpio.PinNum)));
    
    GpioCfgRegVal &= (BIT17 | BIT18 | BIT19 | BIT20);
    
    return (GpioCfgRegVal >> 17);
}

SB_GPE0_DESC BuildGpe0Desc (
  IN SB_GPE0_TYPE Gpe0Type,
  IN BOOLEAN          Gpe0En
)
{
    SB_GPE0_DESC Gpe0Desc;
    
    if (Gpe0En)
        Gpe0Desc.Offset = R_PCH_ACPI_GPE0_EN_127_96;
    else
        Gpe0Desc.Offset = R_PCH_ACPI_GPE0_STS_127_96;

    switch (Gpe0Type) {
        case TYPE_HOT_PLUG:
            Gpe0Desc.Buffer32 = BIT1;
            break;
        case TYPE_SWGPE:
            Gpe0Desc.Buffer32 = BIT2;
            break;
        case TYPE_TCOSCI:
            Gpe0Desc.Buffer32 = BIT6;
            break;
        case TYPE_RI:
            Gpe0Desc.Buffer32 = BIT8;
            break;
        case TYPE_PCI_EXP:
            Gpe0Desc.Buffer32 = BIT9;
            break;
        case TYPE_BATLOW:
            Gpe0Desc.Buffer32 = BIT10;
            break;
        case TYPE_PME:
            Gpe0Desc.Buffer32 = BIT11;
            break;
        case TYPE_ME_SCI:
            Gpe0Desc.Buffer32 = BIT12;
            break;
        case TYPE_PME_B0:
            Gpe0Desc.Buffer32 = BIT13;
            break;
        case TYPE_ESPI_SCI:
            Gpe0Desc.Buffer32 = BIT14;
            break;
        case TYPE_GPIO_TIER2_SCI:
            Gpe0Desc.Buffer32 = BIT15;
            break;
        case TYPE_WADT:
            Gpe0Desc.Buffer32 = BIT18;      
            break;
        default:
            Gpe0Desc.Offset = 0xFFFF;
            Gpe0Desc.Buffer32 = 0xFFFFFFFF;
            break;
    } // end of switch
    
    return Gpe0Desc;
}

/*************************************************
 *
 * Read/Clear ALT-GPI SMI Status
 *
 *************************************************/
UINT32 SbReadGpiGppSmiSts( IN AMI_OEM_GPIO   Gpio )
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioSmiStsRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioSmiStsRegVal = MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiStsOffset));
    
    return GpioSmiStsRegVal;
}

/*************************************************
 *
 * Read/Write/Program ALT-GPI SMI Eanle
 *
 *************************************************/
UINT32 SbReadAltGpiSmi( IN AMI_OEM_GPIO   Gpio )
{
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    UINT32               GroupIndex;
    UINT32               GpioSmiEnRegVal = 0;
    
    if (GetPchSeries() == PchLp){
        GpioGroupInfo = SbLpGpioGroupInfo;
    } else {
        GpioGroupInfo = SbHGpioGroupInfo;
    }
    GroupIndex = (Gpio.Group) & 0xFF;
    
    GpioSmiEnRegVal = MmioRead32 (PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiEnOffset));
    
    return GpioSmiEnRegVal;
}

// End extra function for SKL / KBL
//---------------------------------------------------------------------------


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
