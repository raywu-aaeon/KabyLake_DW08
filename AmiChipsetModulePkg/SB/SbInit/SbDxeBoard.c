//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//*************************************************************************
/** @file SbDxeBoard.c
    This file contains DXE stage board component code for
    South Bridge.

**/
//*************************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

// Module specific Includes
#include <Efi.h>
#include <Dxe.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <SbDxeInitElink.h>
#include <AmiCspLib.h>

// Produced Protocols

// Consumed Protocols
#include <PchAccess.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

typedef VOID (SAVE_RESTORE_CALLBACK)( BOOLEAN Save );

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

SAVE_RESTORE_CALLBACK* SaveRestoreCallbackList[] = \
                                          { SAVE_RESTORE_CALLBACK_LIST NULL };

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

extern SAVE_RESTORE_CALLBACK SAVE_RESTORE_CALLBACK_LIST EndOfList;

// Function Definition(s)

//---------------------------------------------------------------------------

/**
    This function initializes the board specific component in
    in the chipset South bridge

    @param ImageHandle - Image handle
    @param SystemTable - Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.
**/

EFI_STATUS SBDXE_BoardInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib( ImageHandle, SystemTable );

    return EFI_SUCCESS;
}

#if defined SB_TIMER_ARCH_PROTOCOL_SUPPORT && SB_TIMER_ARCH_PROTOCOL_SUPPORT == 1
/**
    This function calls registered callbacks to save/restore
    registers value in timer interrupt routine

    @param BOOLEAN Save - if TRUE - save registers, FALSE - restore back

    @retval VOID
**/

VOID SaveRestoreRegisters (
    IN BOOLEAN              Save )
{
    UINTN   i;

    for (i = 0; SaveRestoreCallbackList[i] != NULL; i++) 
        SaveRestoreCallbackList[i]( Save );
}
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
