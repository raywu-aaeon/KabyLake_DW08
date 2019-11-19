//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiCpuFeaturesDxe.h
    Ami cpu features dxe header file.

**/

#ifndef __AMI_CPUFEATURES_DXE_H__
#define __AMI_CPUFEATURES_DXE_H__

#include <Token.h>
#include <Dxe.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <CpuCspLib.h>
#include <CpuInfoHob.h>
#include <CpuBoardLib.h>
#include <Cpu.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL                           BIT35

#define MSR_PLAT_FRMW_PROT_CTRL                                       0x00000110
#define B_MSR_PLAT_FRMW_PROT_CTRL_LK                                  BIT0
#define B_MSR_PLAT_FRMW_PROT_CTRL_EN                                  BIT1
#define B_MSR_PLAT_FRMW_PROT_CTRL_S1                                  BIT2

#define MSR_TEMPERATURE_TARGET                                        0x000001A2
#define MSR_PACKAGE_POWER_SKU_UNIT                                    0x606
#define MSR_PACKAGE_POWER_SKU                                         0x614

//Cstate
#define C3_STATE   3
#define C6_STATE   6
#define C7_STATE   7
#define C8_STATE   8
#define C9_STATE   9
#define C10_STATE 10

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
