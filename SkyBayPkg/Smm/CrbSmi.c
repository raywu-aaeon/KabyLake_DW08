//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
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
// $Header: 
//
// $Revision: 
//
// $Date: 
//*************************************************************************
/** @file CrbSmi.c
    This file contains code for all CRB SMI events

**/
//*************************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmVariable.h>
#include <Library/SmmServicesTableLib.h>

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
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

/**
    This is a template CRB software SMI Handler for Porting.

    @param DispatchHandle  - EFI Handle
    @param DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval VOID
**/
#if defined CRB_AMI_GET_TSEG_SIZE_SUPPORT && CRB_AMI_GET_TSEG_SIZE_SUPPORT == 1
#define AMI_TSEG_INFO_GUID \
    {0x202f0384, 0xaa82, 0x4a04, 0xae, 0x64, 0x2c, 0x21, 0xf4, 0x61, 0xa5, 0xc}
#endif

EFI_STATUS CRBSwSmiHandler (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL)
{
#if defined CRB_AMI_GET_TSEG_SIZE_SUPPORT && CRB_AMI_GET_TSEG_SIZE_SUPPORT == 1
    // Porting if needed
    EFI_STATUS                   Status;
    UINTN                        i = 0, UnusedTsegPageSize = 0, DataSize;
    EFI_PHYSICAL_ADDRESS         SmramBuffer;
    EFI_SMM_VARIABLE_PROTOCOL    *mSmmVariable;
    EFI_GUID                     gAmiTsegInfoGuid = AMI_TSEG_INFO_GUID;

    while (1) {        
        Status = gSmst->SmmAllocatePages (
                      AllocateAnyPages,
                      EfiRuntimeServicesData,
                      UnusedTsegPageSize,
                      &SmramBuffer
                      );
        if( !EFI_ERROR(Status) ) {
            gSmst->SmmFreePages (SmramBuffer, UnusedTsegPageSize);
            UnusedTsegPageSize++;
        } else {
            Status = gSmst->SmmLocateProtocol(&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&mSmmVariable);
            if (EFI_ERROR(Status)) return Status;
            
            DataSize = sizeof (UINT32);
            Status = mSmmVariable->SmmSetVariable (
                                     L"GetTsegInfo",
                                     &gAmiTsegInfoGuid,
                                     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                     DataSize,
                                     &UnusedTsegPageSize
                                     );
            if (EFI_ERROR(Status)) return Status;
            break;
        }
    }
#endif    
    return EFI_SUCCESS;
}

/**
    This is a template CRB Sx SMI Handler for Porting.

    @param DispatchHandle  - EFI Handle
    @param DispatchContext - Pointer to the EFI_SMM_SX_DISPATCH_CONTEXT

    @retval VOID
**/

EFI_STATUS CRBSxSmiHandler (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    // Porting if needed
    return EFI_SUCCESS;    
}

#if defined POWER_BUTTON_DRIVER_SUPPORT && POWER_BUTTON_DRIVER_SUPPORT == 1
/**
    This is a template CRB Power Button SMI Handler for Porting.

    @param DispatchHandle  - EFI Handle
    @param DispatchContext - Pointer to the
    @param EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT

    @retval VOID
**/

EFI_STATUS CRBPowerButtonSmiHandler (
    IN EFI_HANDLE    DispatchHandle,
    IN CONST VOID    *Context OPTIONAL,
    IN OUT VOID      *CommBuffer OPTIONAL,
    IN OUT UINTN     *CommBufferSize OPTIONAL)
{
    // Porting if needed
    return EFI_SUCCESS;
}
#endif

/**
    Installs CRB SMM Child Dispatcher Handler.

    @param ImageHandle - Image handle
        *SystemTable - Pointer to the system table

        EFI_STATUS
    @retval EFI_NOT_FOUND The SMM Base protocol is not found.
    @retval EFI_SUCCESS Installs CRB SMM Child Dispatcher Handler
        successfully.
**/

EFI_STATUS InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                              Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL           *pSwDispatch;
    EFI_SMM_SX_DISPATCH2_PROTOCOL           *pSxDispatch;
#if defined POWER_BUTTON_DRIVER_SUPPORT && POWER_BUTTON_DRIVER_SUPPORT == 1    
    EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *pPwrDispatch;
#endif    
    EFI_SMM_SW_REGISTER_CONTEXT             SwContext = {CRB_SWSMI};
    EFI_SMM_SX_REGISTER_CONTEXT             SxContext = {SxS3, SxEntry};
    EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT   PwrContext = {EfiPowerButtonEntry};    
    EFI_HANDLE                              Handle;
    
    Status = InitAmiSmmLib (ImageHandle, SystemTable);
    if (EFI_ERROR(Status)) return Status;    

    Status = pSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch);
    if (!EFI_ERROR(Status)) {
        Status  = pSwDispatch->Register (pSwDispatch, \
                                         CRBSwSmiHandler, \
                                         &SwContext, \
                                         &Handle);
    }

    Status = pSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid, NULL, &pSxDispatch);    
    if (!EFI_ERROR(Status)) {
        Status  = pSxDispatch->Register (pSxDispatch, \
                                         CRBSxSmiHandler, \
                                         &SxContext, \
                                         &Handle);
    }
#if defined POWER_BUTTON_DRIVER_SUPPORT && POWER_BUTTON_DRIVER_SUPPORT == 1
    Status = pSmst->SmmLocateProtocol ( 
                       &gEfiSmmPowerButtonDispatch2ProtocolGuid, 
                       NULL,
                       &pPwrDispatch);
    if (!EFI_ERROR(Status)) {
        Status  = pPwrDispatch->Register (pPwrDispatch, \
                                          CRBPowerButtonSmiHandler, \
                                          &PwrContext, \
                                          &Handle);
    }
#endif    

    return EFI_SUCCESS;
}

/**
    Installs CRB SMM Child Dispatcher Handler.

    @param ImageHandle - Image handle
    @param *SystemTable - Pointer to the system table


    @retval EFI_NOT_FOUND The SMM Base protocol is not found.
    @retval EFI_SUCCESS Installs CRB SMM Child Dispatcher Handler
        successfully.
**/

EFI_STATUS EFIAPI InitializeCrbSmm (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
