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

/** @file CpuBoardLib.c
    Platform CPU Lib C source file

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include "Cpu.h"
#include "CpuBoardLib.h"

EFI_GUID gAmiSetupGuid = SETUP_GUID;

typedef struct {
    SETUP_DATA *SetupData;
} PRIVATE_CPU_SETUP_LIB;

/**
    Initialize DXE Init Platform Cpu Lib.

    @param Bs A pointer to boot service.
    @param Rs A pointer to runtime service.
    @param Handle Setup handle.

    @retval EFI_STATUS Return EFI status.
**/

EFI_STATUS DxeInitPlatformCpuLib(
    IN EFI_BOOT_SERVICES       *Bs,
    IN EFI_RUNTIME_SERVICES    *Rs,
	OUT VOID **Handle
)
{
    UINTN VariableSize = sizeof(SETUP_DATA);
    PRIVATE_CPU_SETUP_LIB   *Private;
    EFI_STATUS              Status;
    
    *Handle = NULL;
    
    Status = Bs->AllocatePool(EfiBootServicesData, sizeof(PRIVATE_CPU_SETUP_LIB), &Private);
    if (EFI_ERROR(Status)) return Status;
    
    Status = Bs->AllocatePool(EfiBootServicesData, sizeof(SETUP_DATA), &Private->SetupData);
    if (EFI_ERROR(Status)) return Status;
    
    VariableSize = sizeof(SETUP_DATA);
    Status = Rs->GetVariable(
        L"Setup",
        &gAmiSetupGuid,
        NULL,
        &VariableSize,
        Private->SetupData
    );
    if (EFI_ERROR(Status)) return Status;
    
    *Handle = (VOID*)Private;

    return EFI_SUCCESS;
}

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
