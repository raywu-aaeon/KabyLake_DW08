//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CpuBoardLib.h
    CPU library.

**/

#ifndef __PLATFORM_CPU_LIB_H__
#define __PLATFORM_CPU_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VFRCOMPILE
#include <Efi.h>
#include <Pei.h>

EFI_STATUS DxeInitPlatformCpuLib(
    IN EFI_BOOT_SERVICES       *Bs,
    IN EFI_RUNTIME_SERVICES    *Rs,
	OUT VOID **Handle
);

#endif

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

