//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************

/** @file SmmChildDispatchMain.c
    This file contains implementation of module entry point,
    generic RegisterHandler and UnregisterHandler routines 
    and main dispatcher loop.

**/

#include <AmiDxeLib.h>
#include <Smm.h>
#include "SmmChildDispatch.h"
#include "SmmChildDispatchProtocol.h"

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>

#define SMI_HANDLER_SIGNATURE 0x48494d53    //SMIH

EFI_SMM_CPU_PROTOCOL    *gEfiSmmCpuProtocol;

EFI_SMM_SMI_CONTEXT SmiContext;

extern SMM_CHILD_DISPATCHER SmmHandler[];

extern EFI_SMM_SW_DISPATCH2_PROTOCOL gEfiSmmSwDispatch2Protocol;
extern EFI_SMM_SX_DISPATCH2_PROTOCOL gEfiSmmSxDispatch2Protocol;
extern EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL \
                                       gEfiSmmPeriodicTimerDispatch2Protocol;
extern EFI_SMM_USB_DISPATCH2_PROTOCOL gEfiSmmUsbDispatch2Protocol;
extern EFI_SMM_GPI_DISPATCH2_PROTOCOL gEfiSmmGpiDispatch2Protocol;
extern EFI_SMM_STANDBY_BUTTON_DISPATCH2_PROTOCOL \
                                      gEfiSmmStandbyButtonDispatch2Protocol;
extern EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL \
                                          gEfiSmmPowerButonDispatch2Protocol;
extern EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL gEfiSmmIoTrapDispatch2Protocol;


/**
    This function registers SMI handler and returns registered handle

    @param EFI_SMM_SMI Type - type of SMI handler
    @param SMI_GENERIC_CALLBACK Function - pointer to callback function
    @param VOID *Context - pointer to callback context
    @param UINTN ContextSize - callback context size
    @param EFI_HANDLE *Handle - pointer to store registered handle

    @retval EFI_STATUS

**/
EFI_STATUS RegisterHandler(
    IN  EFI_SMM_SMI           Type,
    IN  SMI_GENERIC_CALLBACK  Function,
    IN  VOID                  *Context,
    IN  UINTN                 ContextSize,
    OUT EFI_HANDLE            *Handle )
{
    EFI_STATUS Status;
    HANDLER_LINK *HandlerLink;

    Status = SmmHandler[Type].HandlerProtocol.VerifyContext(Context);
    if(EFI_ERROR(Status))
        return Status;

    Status = pSmst->SmmAllocatePool(0,
                                 sizeof(HANDLER_LINK) + ContextSize -1,
                                 (VOID **)&HandlerLink);
    if(EFI_ERROR(Status))
        return Status;

    HandlerLink->Callback = Function;
    HandlerLink->Signature = SMI_HANDLER_SIGNATURE;
    MemCpy(HandlerLink->Context, Context, ContextSize);

    DListAdd(&(SmmHandler[Type].RegisteredCallbacks), (DLINK *)HandlerLink);

    SmmHandler[Type].HandlerProtocol.AddHandler(HandlerLink->Context);
    *Handle = HandlerLink;

    return EFI_SUCCESS;
}

/**
    This function unregisters SMI handler with given handle

    @param EFI_SMM_SMI Type - type of SMI handler
    @param EFI_HANDLE Handle - handle of registered handler

    @retval EFI_STATUS

**/
EFI_STATUS UnregisterHandler(
    IN EFI_SMM_SMI  Type,
    IN EFI_HANDLE   Handle
    )
{
    HANDLER_LINK *HandlerLink = (HANDLER_LINK *)Handle;

    if(HandlerLink->Signature != SMI_HANDLER_SIGNATURE)
        return EFI_INVALID_PARAMETER;

    SmmHandler[Type].HandlerProtocol.RemoveHandler(HandlerLink->Context);

    DListDelete(&(SmmHandler[Type].RegisteredCallbacks), (DLINK *)HandlerLink);
    pSmst->SmmFreePool(HandlerLink);

    return EFI_SUCCESS;
}

/**
    This function implements main SMI dispatcher loop

    @param EFI_HANDLE SmmImageHandle - SMM image handle
    @param VOID *CommunicationBuffer - pointer to optional communication buffer
    @param UINTN *SourceSize - pointer to size of communication buffer

    @retval EFI_STATUS

**/
EFI_STATUS ChildDispatcher(
//#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A) // [ EIP427935 ]
    IN EFI_HANDLE           DispatchHandle,
	IN CONST VOID           *DispatchContext OPTIONAL,
	IN OUT VOID             *CommBuffer OPTIONAL,
	IN OUT UINTN            *CommBufferSize OPTIONAL
//#else
//    IN      EFI_HANDLE      SmmImageHandle,
//    IN OUT  VOID            *CommunicationBuffer OPTIONAL,
//    IN OUT  UINTN           *SourceSize OPTIONAL
//#endif
)
{
    UINT32 Index;
    BOOLEAN HandledSmi;

    do
    {
        HandledSmi = FALSE;
        for(Index = EfiSmmSwSmi; Index < EfiSmmMaxSmi; Index++)
        {
            if(SmmHandler[Index].HandlerProtocol.GetContext != NULL &&
               SmmHandler[Index].HandlerProtocol.GetContext())
            {
                SmmHandler[Index].HandlerProtocol.DispatchSmi();
                HandledSmi = TRUE;
            }
        }
    } while (HandledSmi);

    ClearAllSmi();

    return EFI_HANDLER_SUCCESS;
}

/**
    This function initializes Child dispatcher in SMM mode

    @param EFI_HANDLE ImageHandle - Image handle
    @param EFI_SYSTEM_TABLE *SystemTable - pointer to system table

    @retval EFI_STATUS

**/
EFI_STATUS InSmmFunction(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS Status;
    EFI_HANDLE Handle = NULL;

//#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A) // [ EIP427935 ]
    EFI_HANDLE      Handle2 = NULL;
    EFI_HANDLE      RootHandle = NULL;

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;
//#endif

    DisableAllSmi();

//#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A) // [ EIP427935 ]
    // Locate SMM CPU Protocols
    Status = pSmst->SmmLocateProtocol( &gEfiSmmCpuProtocolGuid, \
                                       NULL, \
                                       &gEfiSmmCpuProtocol );

    Status = pSmst->SmmInstallProtocolInterface( \
                                            &Handle2, \
                                            &gEfiSmmSwDispatch2ProtocolGuid, \
                                            EFI_NATIVE_INTERFACE, \
                                            &gEfiSmmSwDispatch2Protocol );
    ASSERT_EFI_ERROR(Status);

    Status = pSmst->SmmInstallProtocolInterface( \
                                            &Handle2, \
                                            &gEfiSmmSxDispatch2ProtocolGuid, \
                                            EFI_NATIVE_INTERFACE, \
                                            &gEfiSmmSxDispatch2Protocol );
    ASSERT_EFI_ERROR(Status);

    Status = pSmst->SmmInstallProtocolInterface( \
                                 &Handle2, \
                                 &gEfiSmmPeriodicTimerDispatch2ProtocolGuid, \
                                 EFI_NATIVE_INTERFACE, \
                                 &gEfiSmmPeriodicTimerDispatch2Protocol );
    ASSERT_EFI_ERROR(Status);

    Status = pSmst->SmmInstallProtocolInterface( \
                                        &Handle2, \
                                        &gEfiSmmUsbDispatch2ProtocolGuid, \
                                        EFI_NATIVE_INTERFACE, \
                                        &gEfiSmmUsbDispatch2Protocol );
    ASSERT_EFI_ERROR(Status);

    Status = pSmst->SmmInstallProtocolInterface( \
                                        &Handle2, \
                                        &gEfiSmmGpiDispatch2ProtocolGuid, \
                                        EFI_NATIVE_INTERFACE, \
                                        &gEfiSmmGpiDispatch2Protocol );
    ASSERT_EFI_ERROR(Status);

    Status = pSmst->SmmInstallProtocolInterface( \
                                &Handle2, \
                                &gEfiSmmStandbyButtonDispatch2ProtocolGuid, \
                                EFI_NATIVE_INTERFACE, \
                                &gEfiSmmStandbyButtonDispatch2Protocol );
    ASSERT_EFI_ERROR(Status);

    Status = pSmst->SmmInstallProtocolInterface( \
                                    &Handle2, \
                                    &gEfiSmmPowerButtonDispatch2ProtocolGuid, \
                                    EFI_NATIVE_INTERFACE, \
                                    &gEfiSmmPowerButonDispatch2Protocol );
    ASSERT_EFI_ERROR(Status);

    Status = pSmst->SmmInstallProtocolInterface( \
                                    &Handle2, \
                                    &gEfiSmmIoTrapDispatch2ProtocolGuid, \
                                    EFI_NATIVE_INTERFACE, \
                                    &gEfiSmmIoTrapDispatch2Protocol );
    ASSERT_EFI_ERROR(Status);
    Status  = pSmst->SmiHandlerRegister( ChildDispatcher, \
                                         NULL, \
                                         &RootHandle );
//#else
//    //Install Protocols
//    Status = pBS->InstallMultipleProtocolInterfaces(&Handle,
//        &gEfiSmmSwDispatchProtocolGuid,             &gEfiSmmSwDispatchProtocol,
//        &gEfiSmmSxDispatchProtocolGuid,             &gEfiSmmSxDispatchProtocol,
//        &gEfiSmmPeriodicTimerDispatchProtocolGuid,  &gEfiSmmPeriodicTimerDispatchProtocol,
//        &gEfiSmmUsbDispatchProtocolGuid,            &gEfiSmmUsbDispatchProtocol,
//        &gEfiSmmGpiDispatchProtocolGuid,            &gEfiSmmGpiDispatchProtocol,
//        &gEfiSmmStandbyButtonDispatchProtocolGuid,  &gEfiSmmStandbyButtonDispatchProtocol,
//        &gEfiSmmPowerButtonDispatchProtocolGuid,    &gEfiSmmPowerButonDispatchProtocol,
//        &gEfiSmmIoTrapDispatchProtocolGuid,         &gEfiSmmIoTrapDispatchProtocol,
//        NULL
//        );
//    ASSERT_EFI_ERROR(Status);
//
//    Status = pSmmBase->RegisterCallback(pSmmBase, ImageHandle, ChildDispatcher, FALSE, FALSE);
//    ASSERT_EFI_ERROR(Status);
//#endif

    return Status;
}

/**
    SMM Child Dispatcher module entry point

    @param EFI_HANDLE ImageHandle - Image handle
    @param EFI_SYSTEM_TABLE *SystemTable - pointer to system table

    @retval EFI_STATUS

**/
EFI_STATUS SmmChildDispatchEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS      Status = EFI_SUCCESS;

    InitAmiLib(ImageHandle,SystemTable);

    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
