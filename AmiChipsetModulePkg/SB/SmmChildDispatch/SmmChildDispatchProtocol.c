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

/** @file SmmChildDispatchProtocol.c
    SMM Child Dispatch protocol functions implementation

**/
#include <Token.h>
#include <AmiDxeLib.h>
#include "SmmChildDispatch.h"
#include "SmmChildDispatchProtocol.h"

SMM_CHILD_DISPATCHER SmmHandler[] = {
//SwSmmHandler
    EfiSmmSwSmi,
    { SmmSwAddHandler, SmmSwRemoveHandler, SmmSwVerifyContext, SmmSwGetContext, SmmSwDispatchSmi },
    { 0, NULL, NULL },

//SxSmmHandler
    EfiSmmSxSmi,
    { SmmSxAddHandler, SmmSxRemoveHandler, SmmSxVerifyContext, SmmSxGetContext, SmmSxDispatchSmi },
    { 0, NULL, NULL },

//PeriodicTimerSmmHandler
    EfiSmmPeriodicTimerSmi,
    { SmmTimerAddHandler, SmmTimerRemoveHandler, SmmTimerVerifyContext, SmmTimerGetContext, SmmTimerDispatchSmi },
    { 0, NULL, NULL },

//UsbSmmHandler
    EfiSmmUsbSmi,
    { SmmUsbAddHandler, SmmUsbRemoveHandler, SmmUsbVerifyContext, SmmUsbGetContext, SmmUsbDispatchSmi },
    { 0, NULL, NULL },

//UsbSmmHandler
    EfiSmmGpiSmi,
    { SmmGpiAddHandler, SmmGpiRemoveHandler, SmmGpiVerifyContext, SmmGpiGetContext, SmmGpiDispatchSmi },
    { 0, NULL, NULL },

//SButtonSmmHandler
    EfiSmmStandbyButtonSmi,
    { SmmSButtonAddHandler, SmmSButtonRemoveHandler, SmmSButtonVerifyContext, SmmSButtonGetContext, SmmSButtonDispatchSmi },
    { 0, NULL, NULL },

//PButtonSmmHandler
    EfiSmmPowerButtonSmi,
    { SmmPButtonAddHandler, SmmPButtonRemoveHandler, SmmPButtonVerifyContext, SmmPButtonGetContext, SmmPButtonDispatchSmi },
    { 0, NULL, NULL },

//IoTrapSmmHandler
    EfiSmmIoTrapSmi,
    { SmmIoTrapAddHandler, SmmIoTrapRemoveHandler, SmmIoTrapVerifyContext, SmmIoTrapGetContext, SmmIoTrapDispatchSmi },
    { 0, NULL, NULL },

//************************ PUT ADDITIONAL HANDLERS HERE *****************************************
//************************ PUT ADDITIONAL HANDLERS HERE *****************************************
//************************ PUT ADDITIONAL HANDLERS HERE *****************************************

//Terminator record
    EfiSmmMaxSmi,
    { NULL,     NULL,       NULL,       NULL,       NULL },
    { 0, NULL, NULL }
};

EFI_SMM_SW_DISPATCH2_PROTOCOL gEfiSmmSwDispatch2Protocol = \
                                                { EfiSmmSwRegister, \
                                                  EfiSmmSwUnregister, \
                                                  MAX_SW_SMI_INPUT_VALUE };

EFI_SMM_SX_DISPATCH2_PROTOCOL gEfiSmmSxDispatch2Protocol = \
                                  { EfiSmmSxRegister, EfiSmmSxUnregister };

EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL \
                                    gEfiSmmPeriodicTimerDispatch2Protocol = \
                                       { EfiSmmTimerRegister, \
                                         EfiSmmTimerUnregister, \
                                         EfiSmmTimerGetNextShorterInterval };
EFI_SMM_USB_DISPATCH2_PROTOCOL gEfiSmmUsbDispatch2Protocol = \
                                 { EfiSmmUsbRegister, EfiSmmUsbUnregister };
EFI_SMM_GPI_DISPATCH2_PROTOCOL gEfiSmmGpiDispatch2Protocol = \
                                                    { EfiSmmGpiRegister, \
                                                      EfiSmmGpiUnregister, \
                                                      SUPPORTED_GPIS };
EFI_SMM_STANDBY_BUTTON_DISPATCH2_PROTOCOL \
                                    gEfiSmmStandbyButtonDispatch2Protocol = \
                         { EfiSmmSButtonRegister, EfiSmmSButtonUnregister };
EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL gEfiSmmPowerButonDispatch2Protocol = \
                        { EfiSmmPButtonRegister, EfiSmmPButtonUnregister };
EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL gEfiSmmIoTrapDispatch2Protocol = \
                          { EfiSmmIoTrapRegister, EfiSmmIoTrapUnregister };


extern UINT64 SupportedIntervals[];

//---------------------------------------------------------------------------
//                          S/W SMI Handler functions
//---------------------------------------------------------------------------

/**
    EFI_SMM_SW_DISPATCH(2)_PROTOCOL Register function.

    @param *This    - Pointer to EFI_SMM_SW_DISPATCH(2)_PROTOCOL
    @param Function - Pointer to callback function
    @param *Context - Pointer to callback context
    @param *Handle  - Pointer to store registered handle

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmSwRegister (
    IN  CONST EFI_SMM_SW_DISPATCH2_PROTOCOL     *This,
    IN        EFI_SMM_HANDLER_ENTRY_POINT2      Function,
    IN  CONST EFI_SMM_SW_REGISTER_CONTEXT       *Context,
    OUT       EFI_HANDLE                        *Handle )
{
    return RegisterHandler( EfiSmmSwSmi, 
                            Function, 
                            Context, 
                            sizeof(AMI_SMM_SW_CONTEXT),
                            Handle );
}

/**
    EFI_SMM_SW_DISPATCH(2)_PROTOCOL Unregister function.

    @param *This  - Pointer to the EFI_SMM_SW_DISPATCH(2)_PROTOCOL
    @param Handle - Handle to unregister

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmSwUnregister(
    IN CONST EFI_SMM_SW_DISPATCH2_PROTOCOL      *This,
    IN       EFI_HANDLE                         Handle )
{
    return UnregisterHandler( EfiSmmSwSmi, Handle );
}

//-----------------------------------------------------------------------
//              SX SMI Handler functions
//-----------------------------------------------------------------------

/**
    EFI_SMM_SX_DISPATCH(2)_PROTOCOL Register function.

    @param *This    - Pointer to EFI_SMM_SX_DISPATCH(2)_PROTOCOL
    @param Function - Pointer to callback function
    @param *Context - Pointer to callback context
    @param *Handle  - Pointer to store registered handle

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmSxRegister(
    IN  CONST EFI_SMM_SX_DISPATCH2_PROTOCOL     *This,
    IN        EFI_SMM_HANDLER_ENTRY_POINT2      Function,
    IN  CONST EFI_SMM_SX_REGISTER_CONTEXT       *Context,
    OUT       EFI_HANDLE                        *Handle )
{
    return RegisterHandler( EfiSmmSxSmi, 
                            Function, 
                            Context, 
                            sizeof(AMI_SMM_SX_CONTEXT),
                            Handle );
}

/**
    EFI_SMM_SW_DISPATCH(2)_PROTOCOL Unregister function.

    @param *This  - Pointer to the EFI_SMM_SX_DISPATCH(2)_PROTOCOL
    @param Handle - Handle to unregister

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmSxUnregister (
    IN CONST EFI_SMM_SX_DISPATCH2_PROTOCOL *This,
    IN       EFI_HANDLE                    Handle )
{
    return UnregisterHandler( EfiSmmSxSmi, Handle );
}

//-----------------------------------------------------------------------
//              Periodic timer SMI Handler functions
//-----------------------------------------------------------------------

/**
    EFI_SMM_PERIODIC_TIMER_DISPATCH(2)_PROTOCOL Register function.

    @param *This    - Pointer to EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL
    @param Function - Pointer to callback function
    @param *Context - Pointer to callback context
    @param *Handle  - Pointer to store registered handle

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmTimerRegister(
    IN  CONST EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL *This,
    IN        EFI_SMM_HANDLER_ENTRY_POINT2              Function,
    IN  CONST EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT   *Context,
    OUT       EFI_HANDLE                                *Handle )
{
    EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT ExContext;

    ExContext.Period = Context->Period;
    ExContext.SmiTickInterval = Context->SmiTickInterval;
    ExContext.ElapsedTime = 0;

    return RegisterHandler( EfiSmmPeriodicTimerSmi, 
                            Function, 
                            Context, 
                            sizeof(EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT),
                            Handle );
}

/**
    EFI_SMM_PERIODIC_TIMER_DISPATCH(2)_PROTOCOL Unregister function.

    @param *This  - Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH(2)_PROTOCOL
    @param Handle - Handle to unregister

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmTimerUnregister(
    IN CONST EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL *This,
    IN       EFI_HANDLE                                Handle )
{
    return UnregisterHandler( EfiSmmPeriodicTimerSmi, Handle );
}

/**
    EFI_SMM_PERIODIC_TIMER_DISPATCH(2)_PROTOCOL GetNextShorterInterval
    function.

    @param *This - Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH(2)_PROTOCOL
    @param **SmiTickInterval - Pointer to store pointer to next interval

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmTimerGetNextShorterInterval(
    IN CONST EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL *This,
    IN OUT   UINT64                                    **SmiTickInterval )
{
    UINT64 *Result = *SmiTickInterval;

    if(Result == NULL)
        Result = SupportedIntervals;
    else
        Result++;

    *SmiTickInterval = (*Result == 0) ? NULL : Result;
    return EFI_SUCCESS;
}

//-----------------------------------------------------------------------
//              Usb SMI Handler functions
//-----------------------------------------------------------------------

/**
    EFI_SMM_USB_DISPATCH(2)_PROTOCOL Register function.

    @param *This    - Pointer to EFI_SMM_USB_DISPATCH(2)_PROTOCOL
    @param Function - Pointer to callback function
    @param *Context - Pointer to callback context
    @param *Handle  - Pointer to store registered handle

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmUsbRegister(
    IN  CONST EFI_SMM_USB_DISPATCH2_PROTOCOL    *This,
    IN        EFI_SMM_HANDLER_ENTRY_POINT2      Function,
    IN  CONST EFI_SMM_USB_REGISTER_CONTEXT      *Context,
    OUT       EFI_HANDLE                        *Handle )
{
    return RegisterHandler( EfiSmmUsbSmi, 
                            Function, 
                            Context, 
                            sizeof(AMI_SMM_USB_CONTEXT),
                            Handle );
}

/**
    EFI_SMM_USB_DISPATCH(2)_PROTOCOL Unregister function.

    @param *This  - Pointer to the EFI_SMM_USB_DISPATCH(2)_PROTOCOL
    @param Handle - Handle to unregister

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmUsbUnregister(
    IN CONST EFI_SMM_USB_DISPATCH2_PROTOCOL  *This,
    IN       EFI_HANDLE                      Handle )
{
    return UnregisterHandler( EfiSmmUsbSmi, Handle );
}

//-----------------------------------------------------------------------
//              Gpi SMI Handler functions
//-----------------------------------------------------------------------

/**
    EFI_SMM_GPI_DISPATCH(2)_PROTOCOL Register function.

    @param *This    - Pointer to EFI_SMM_GPI_DISPATCH(2)_PROTOCOL
    @param Function - Pointer to callback function
    @param *Context - Pointer to callback context
    @param *Handle  - Pointer to store registered handle

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmGpiRegister (
    IN CONST EFI_SMM_GPI_DISPATCH2_PROTOCOL *This,
    IN       EFI_SMM_HANDLER_ENTRY_POINT2   Function,
    IN CONST EFI_SMM_GPI_REGISTER_CONTEXT   *Context,
    OUT      EFI_HANDLE                     *Handle )
{
    return RegisterHandler( EfiSmmGpiSmi, 
                            Function, 
                            Context, 
                            sizeof(AMI_SMM_GPI_CONTEXT),
                            Handle );
}

/**
    EFI_SMM_GPI_DISPATCH(2)_PROTOCOL Unregister function.

    @param *This  - Pointer to the EFI_SMM_GPI_DISPATCH(2)_PROTOCOL
    @param Handle - Handle to unregister

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmGpiUnregister(
    IN CONST EFI_SMM_GPI_DISPATCH2_PROTOCOL  *This,
    IN       EFI_HANDLE                      Handle )
{
    return UnregisterHandler( EfiSmmGpiSmi, Handle );
}

//-----------------------------------------------------------------------
//              Standby button SMI Handler functions
//-----------------------------------------------------------------------

/**
    EFI_SMM_STANDBY_BUTTON_DISPATCH(2)_PROTOCOL Register function.

    @param *This    - Pointer to EFI_SMM_STANDBY_BUTTON_DISPATCH(2)_PROTOCOL
    @param Function - Pointer to callback function
    @param *Context - Pointer to callback context
    @param *Handle  - Pointer to store registered handle

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmSButtonRegister (
    IN CONST EFI_SMM_STANDBY_BUTTON_DISPATCH2_PROTOCOL *This,
    IN       EFI_SMM_HANDLER_ENTRY_POINT2              Function,
    IN CONST EFI_SMM_STANDBY_BUTTON_REGISTER_CONTEXT   *Context,
    OUT      EFI_HANDLE                                *Handle )
{
    return RegisterHandler( EfiSmmStandbyButtonSmi, 
                            Function, 
                            Context, 
                            sizeof(AMI_SMM_STANDBY_BUTTON_CONTEXT),
                            Handle );
}

/**
    EFI_SMM_STANDBY_BUTTON_DISPATCH(2)_PROTOCOL Unregister function.

    @param *This - Pointer to the EFI_SMM_STANDBY_BUTTON_DISPATCH(2)_PROTOCOL
    @param Handle - Handle to unregister

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmSButtonUnregister (
    IN CONST EFI_SMM_STANDBY_BUTTON_DISPATCH2_PROTOCOL *This,
    IN       EFI_HANDLE                                Handle )
{
    return UnregisterHandler( EfiSmmStandbyButtonSmi, Handle );
}

//-----------------------------------------------------------------------
//              Power button SMI Handler functions
//-----------------------------------------------------------------------

/**
    EFI_SMM_POWER_BUTTON_DISPATCH(2)_PROTOCOL Register function.

    @param *This    - Pointer to EFI_SMM_POWER_BUTTON_DISPATCH(2)_PROTOCOL
    @param Function - Pointer to callback function
    @param *Context - Pointer to callback context
    @param *Handle  - Pointer to store registered handle

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmPButtonRegister(
    IN CONST EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *This,
    IN       EFI_SMM_HANDLER_ENTRY_POINT2            Function,
    IN CONST EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT   *Context,
    OUT      EFI_HANDLE                              *Handle )
{
    return RegisterHandler( EfiSmmPowerButtonSmi, 
                            Function, 
                            Context, 
                            sizeof(AMI_SMM_POWER_BUTTON_CONTEXT),
                            Handle );
}

/**
    EFI_SMM_POWER_BUTTON_DISPATCH(2)_PROTOCOL Unregister function.

    @param *This  - Pointer to the EFI_SMM_POWER_BUTTON_DISPATCH(2)_PROTOCOL
    @param Handle - Handle to unregister

    @retval EFI_STATUS

**/
EFI_STATUS EfiSmmPButtonUnregister(
    IN CONST EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *This,
    IN       EFI_HANDLE                              Handle )
{
    return UnregisterHandler( EfiSmmPowerButtonSmi, Handle );
}

//---------------------------------------------------------------------------
//                       I/O Trap SMI Handler functions
//---------------------------------------------------------------------------

/**
    EFI_SMM_IO_TRAP_DISPATCH(2)_PROTOCOL Register function.

    @param *This    - Pointer to EFI_SMM_IO_TRAP_DISPATCH(2)_PROTOCOL
    @param Function - Pointer to callback function
    @param *Context - Pointer to callback context
    @param *Handle  - Pointer to store registered handle

    @retval EFI_STATUS
**/

EFI_STATUS EfiSmmIoTrapRegister (
    IN CONST EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL *This,
    IN       EFI_SMM_HANDLER_ENTRY_POINT2       Function,
    IN OUT   EFI_SMM_IO_TRAP_REGISTER_CONTEXT   *Context,
    OUT      EFI_HANDLE                         *Handle )
{
     return RegisterHandler( EfiSmmIoTrapSmi, \
                             Function, \
                             Context, \
                             sizeof(AMI_SMM_IO_TRAP_CONTEXT), \
                             Handle );
}

/**
    EFI_SMM_IO_TRAP_DISPATCH(2)_PROTOCOL Unregister function.

    @param *This  - Pointer to the EFI_SMM_IO_TRAP_DISPATCH(2)_PROTOCOL
    @param Handle - Handle to unregister

    @retval EFI_STATUS
**/

EFI_STATUS EfiSmmIoTrapUnregister (
    IN CONST EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL *This,
    IN       EFI_HANDLE                         Handle )
{
    return UnregisterHandler( EfiSmmIoTrapSmi, Handle );
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
