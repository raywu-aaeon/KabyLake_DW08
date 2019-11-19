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
// $Header:  $
//
// $Revision:  $
//
// $Date: $
//**********************************************************************

/** @file SmiHandlerGeneric.c
    This file contains implementation of generic SMI handler
    functions

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <AmiSmm.h>
#include "SmmChildDispatch.h"

extern SMM_CHILD_DISPATCHER SmmHandler[];
extern EFI_SMM_SMI_CONTEXT SmiContext;

static UINT32 gEnabledIoTrapSmi = 0;
static UINT64 CurrentInterval = 0xffffffffffffffff;
static UINT16 EnabledUsbSmi = 0;
static UINT16 ActiveUsbSmi = 0;
UINT16 EnabledGpiSmi = 0;

extern UINT64 SupportedIntervals[];


//-----------------------------------------------------------------------
//              SW SMI Handler functions
//-----------------------------------------------------------------------

/**
    This function adds SW SMI handler

    @param VOID *Context - pointer to SMI context

    @retval EFI_STATUS

**/
EFI_STATUS SmmSwAddHandler(
    IN VOID  *Context
)
{
    if(SmmHandler[EfiSmmSwSmi].RegisteredCallbacks.Size == 1)
        SwSmiEnable();

    return EFI_SUCCESS;
}

/**
    This function removes SW SMI handler

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS SmmSwRemoveHandler(
    IN VOID  *Context
)
{
    if(SmmHandler[EfiSmmSwSmi].RegisteredCallbacks.Size == 0)
        SwSmiDisable();

    return EFI_SUCCESS;
}

/**
    This function verifies SW SMI context

    @param VOID *Context - pointer to context

    @retval EFI_INVALID_PARAMETER Given context is invalid
    @retval EFI_SUCCESS Context verified

**/
EFI_STATUS SmmSwVerifyContext(
    IN VOID  *Context
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmSwSmi].RegisteredCallbacks.pHead;
    AMI_SMM_SW_CONTEXT *SwContext = (AMI_SMM_SW_CONTEXT *)Context;
    AMI_SMM_SW_CONTEXT *RegisteredSwContext;

//first check if we already registered handler for this value
    while(Handler != NULL)
    {
        RegisteredSwContext = (AMI_SMM_SW_CONTEXT *)Handler->Context;
        if(SwContext->SwSmiInputValue == RegisteredSwContext->SwSmiInputValue)
            return EFI_INVALID_PARAMETER;       //handler with this value already registered

        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

//second check if given value is extended SMI value
    if((SwContext->SwSmiInputValue & 0xff) == EXTENDED_SMI)     //check the lowest byte 
        return EFI_SUCCESS;     //accept value of UINTN size

//third check if given value is in default range
    return (SwContext->SwSmiInputValue > MAX_SW_SMI_INPUT_VALUE) ? EFI_INVALID_PARAMETER : EFI_SUCCESS;
}

/**
    This function verifies SW SMI event and sets SW SMI context

    @param VOID

    @retval TRUE SW SMI occured, context saved
    @retval FALSE There was no SW SMI

**/
BOOLEAN SmmSwGetContext(
    VOID
)
{
    UINT16 SwSmiNumber;
    BOOLEAN SwSmiDetected;

    SwSmiDetected = SwSmiDetect(&SwSmiNumber);

    if(SwSmiDetected)
    {
        if(SwSmiNumber == EXTENDED_SMI)     //get the actual number from EAX register
            SmiContext.SwContext.SwSmiInputValue = GetEAX();
        else
            SmiContext.SwContext.SwSmiInputValue = SwSmiNumber;
    }
    return SwSmiDetected;
}

/**
    This function dispatches SW SMI event based on context

    @param VOID

    @retval VOID

**/
VOID SmmSwDispatchSmi(
    VOID
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmSwSmi].RegisteredCallbacks.pHead;
    AMI_SMM_SW_CONTEXT *SwContext;
    EFI_SMM_SW_CONTEXT          CommBuffer;
    UINTN                       CommBufferSize;
    UINT16                      i;
    EFI_GUID                    SwSmiCpuTriggerGuid = SW_SMI_CPU_TRIGGER_GUID;
    SW_SMI_CPU_TRIGGER          *SwSmiCpuTrigger;
    UINTN                       Cpu = pSmst->CurrentlyExecutingCpu - 1; //default cpu #

    for (i = 0; i < pSmst->NumberOfTableEntries; i++) {
        if (guidcmp(&(pSmst->SmmConfigurationTable[i].VendorGuid), &SwSmiCpuTriggerGuid) == 0)
            break;
    }

    //If found table, check for the CPU that caused the software Smi.
    if (i != pSmst->NumberOfTableEntries) {
        SwSmiCpuTrigger = pSmst->SmmConfigurationTable[i].VendorTable;
        Cpu = SwSmiCpuTrigger->Cpu;
    }

    CommBuffer.SwSmiCpuIndex = Cpu;
    CommBuffer.CommandPort = IoRead8(SW_SMI_IO_ADDRESS);
    CommBuffer.DataPort = IoRead8(SW_SMI_IO_ADDRESS + 1);
    CommBufferSize = sizeof(CommBuffer);

    while(Handler != NULL)
    {
        SwContext = (AMI_SMM_SW_CONTEXT *)Handler->Context;
        if(SwContext->SwSmiInputValue == SmiContext.SwContext.SwSmiInputValue) {
            Handler->Callback(Handler, SwContext, &CommBuffer, &CommBufferSize);
        }
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }
    SwSmiClear();
}

//-----------------------------------------------------------------------
//              SX SMI Handler functions
//-----------------------------------------------------------------------

/**
    This function adds SX SMI handler

    @param VOID *Context - pointer to SMI context

    @retval EFI_STATUS

**/
EFI_STATUS SmmSxAddHandler(
    IN VOID  *Context
)
{
    if(SmmHandler[EfiSmmSxSmi].RegisteredCallbacks.Size == 1)
        SxSmiEnable();

    return EFI_SUCCESS;
}

/**
    This function removes SX SMI handler

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS SmmSxRemoveHandler(
    IN VOID  *Context
)
{
    if(SmmHandler[EfiSmmSxSmi].RegisteredCallbacks.Size == 0)
        SxSmiDisable();

    return EFI_SUCCESS;
}

/**
    This function verifies SX SMI context

    @param VOID *Context - pointer to context

    @retval EFI_INVALID_PARAMETER Given context is invalid
    @retval EFI_SUCCESS Context verified
    @retval EFI_UNSUPPORTED Context is not supported

**/
EFI_STATUS SmmSxVerifyContext(
    IN VOID  *Context
)
{
    AMI_SMM_SX_CONTEXT *SxContext = (AMI_SMM_SX_CONTEXT *)Context;

    if(SxContext->Type >= EfiMaximumSleepType || SxContext->Phase >= EfiMaximumPhase)
        return EFI_INVALID_PARAMETER;

    return (SxContext->Phase != SxEntry) ? EFI_UNSUPPORTED : EFI_SUCCESS;
}

/**
    This function verifies SX SMI event and sets SX SMI context

    @param VOID

    @retval TRUE SX SMI occured, context saved
    @retval FALSE There was no SX SMI

**/
BOOLEAN SmmSxGetContext(
    VOID
)
{
    UINT16 SxSleepState;
    BOOLEAN SxSmiDetected;

    SxSmiDetected = SxSmiDetect(&SxSleepState);

    SmiContext.SxContext.Type = SxSleepState;
    SmiContext.SxContext.Phase = SxEntry;

    return SxSmiDetected;
}

/**
    This function dispatches SX SMI event based on context

    @param VOID

    @retval VOID

**/
VOID SmmSxDispatchSmi(
    VOID
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmSxSmi].RegisteredCallbacks.pHead;
    AMI_SMM_SX_CONTEXT *SxContext;

    while(Handler != NULL)
    {
        SxContext = (AMI_SMM_SX_CONTEXT *)Handler->Context;

        if(SxContext->Type == SmiContext.SxContext.Type &&
           SxContext->Phase == SmiContext.SxContext.Phase) {
            Handler->Callback(Handler, SxContext, NULL, NULL);
        }

        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    SxSmiClear();

    if (SmiContext.SxContext.Type == SxS0) return;

    PutToSleep( &(SmiContext.SxContext) );

    // Control returns here on S1.

    SxSmiClear();
}

//-----------------------------------------------------------------------
//              Periodic timer SMI Handler functions
//-----------------------------------------------------------------------

/**
    This function adds Periodic timer SMI handler

    @param VOID *Context - pointer to SMI context

    @retval EFI_STATUS

**/
EFI_STATUS SmmTimerAddHandler(  
    IN VOID  *Context
)
{
    EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT *TimerContext = (EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT *)Context;

    if(SmmHandler[EfiSmmPeriodicTimerSmi].RegisteredCallbacks.Size == 1)
    {
        CurrentInterval = TimerContext->SmiTickInterval;
        TimerSetInterval(TimerContext->SmiTickInterval);
        TimerSmiEnable();
        return EFI_SUCCESS;
    }

    if(CurrentInterval > TimerContext->SmiTickInterval)
    {
        CurrentInterval = TimerContext->SmiTickInterval;
        TimerSetInterval(TimerContext->SmiTickInterval);
    }
    return EFI_SUCCESS;
}

/**
    This function removes Periodic timer SMI handler

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS SmmTimerRemoveHandler( 
    IN VOID  *Context 
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmPeriodicTimerSmi].RegisteredCallbacks.pHead;
    EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT *TimerContext;
    UINT64 Interval = 0xffffffffffffffff;

    if(SmmHandler[EfiSmmPeriodicTimerSmi].RegisteredCallbacks.Size == 0)
    {
        CurrentInterval = 0xffffffffffffffff;
        TimerSmiDisable();
        return EFI_SUCCESS;
    }

    while(Handler != NULL)
    {    
        TimerContext = (EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT *)Handler->Context;
        Interval = (Interval > TimerContext->SmiTickInterval) ? TimerContext->SmiTickInterval : Interval;
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if(CurrentInterval < Interval) //this means lowest rate timer no longer active
    {
        CurrentInterval = Interval;
        TimerSetInterval(Interval);
    }
    return EFI_SUCCESS;
}

/**
    This function verifies Periodic timer SMI context

    @param VOID *Context - pointer to context

    @retval EFI_INVALID_PARAMETER Given context is invalid
    @retval EFI_SUCCESS Context verified

**/
EFI_STATUS SmmTimerVerifyContext( IN VOID  *Context )
{
    EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT *TimerContext = (EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT *)Context;
    UINT64 *Interval = SupportedIntervals;

    while(*Interval != 0)
    {
        if(*Interval == TimerContext->SmiTickInterval)
            return EFI_SUCCESS;
        Interval++;
    }
    return EFI_INVALID_PARAMETER;
}

/**
    This function verifies Periodic timer SMI event and sets Periodic timer SMI context

    @param VOID

    @retval TRUE Periodic timer SMI occured, context saved
    @retval FALSE There was no Periodic timer SMI

**/
BOOLEAN SmmTimerGetContext( VOID )
{
    UINT16 TimerType;
    BOOLEAN TimerSmiDetected;

    TimerSmiDetected = TimerSmiDetect(&TimerType);
    
    SmiContext.TimerContext.SmiTickInterval = CurrentInterval;
    return TimerSmiDetected;
}

/**
    This function dispatches Periodic timer SMI event based on context

    @param VOID

    @retval VOID

**/
VOID SmmTimerDispatchSmi( VOID )
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmPeriodicTimerSmi].RegisteredCallbacks.pHead;
    EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT *TimerContext;

    while(Handler != NULL)
    {
        TimerContext = (EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT *)Handler->Context;
        TimerContext->ElapsedTime += SmiContext.TimerContext.SmiTickInterval;

        if(TimerContext->ElapsedTime >= TimerContext->Period)
        {
            Handler->Callback(Handler, TimerContext, NULL, NULL);
            TimerContext->ElapsedTime = 0;
        }

        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }
    TimerSmiClear();
}

//-----------------------------------------------------------------------
//              USB SMI Handler functions
//-----------------------------------------------------------------------

/**
    This function adds USB SMI handler

    @param VOID *Context - pointer to SMI context

    @retval EFI_STATUS

**/
EFI_STATUS SmmUsbAddHandler(    
    IN VOID  *Context
)
{
    AMI_SMM_USB_CONTEXT *UsbContext = (AMI_SMM_USB_CONTEXT *)Context;
    UINT16 ControllerType;
    VOID *NewDp;
    EFI_STATUS Status;
//
// Save USB device path protocol into SMM memory
//

    UINTN Length = DPLength(UsbContext->Device);
    Status = pSmst->SmmAllocatePool(0, Length, (VOID **)&NewDp);
    if(EFI_ERROR(Status))
        return Status;
    MemCpy(NewDp, UsbContext->Device, Length);
    UsbContext->Device = (EFI_DEVICE_PATH_PROTOCOL *)NewDp;

    ControllerType = GetControllerType(UsbContext->Device);
    if((ControllerType & EnabledUsbSmi) == 0)
    {
        EnabledUsbSmi |= ControllerType;
        UsbSmiSet(EnabledUsbSmi);
    }
    return EFI_SUCCESS;
}

/**
    This function removes USB SMI handler

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS SmmUsbRemoveHandler( 
    IN VOID  *Context 
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmUsbSmi].RegisteredCallbacks.pHead;
    AMI_SMM_USB_CONTEXT *UsbContext = (AMI_SMM_USB_CONTEXT *)Context;
    UINT16 ControllerType = 0;

    pSmst->SmmFreePool(UsbContext->Device);

    if(SmmHandler[EfiSmmUsbSmi].RegisteredCallbacks.Size == 0)
    {
        EnabledUsbSmi = 0;
        UsbSmiSet(EnabledUsbSmi);
        return EFI_SUCCESS;
    }

    while(Handler != NULL)
    {
        UsbContext = (AMI_SMM_USB_CONTEXT *)Handler->Context;
        ControllerType |= GetControllerType(UsbContext->Device);
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if(ControllerType != EnabledUsbSmi)
    {
        EnabledUsbSmi = ControllerType;
        UsbSmiSet(EnabledUsbSmi);
    }

    return EFI_SUCCESS;
}

/**
    This function verifies USB SMI context

    @param VOID *Context - pointer to context

    @retval EFI_INVALID_PARAMETER Given context is invalid
    @retval EFI_SUCCESS Context verified
    @retval EFI_UNSUPPORTED Context is not supported

**/
EFI_STATUS SmmUsbVerifyContext( 
    IN VOID  *Context 
)
{
    AMI_SMM_USB_CONTEXT *UsbContext = (AMI_SMM_USB_CONTEXT *)Context;
    UINT16 ControllerType;

    ControllerType = GetControllerType(UsbContext->Device);
    if((ControllerType & 3) == 0 || UsbContext->Type > UsbWake)
        return EFI_INVALID_PARAMETER;

    return (UsbContext->Type > UsbLegacy) ? EFI_UNSUPPORTED : EFI_SUCCESS;
}

/**
    This function verifies USB SMI event and sets USB SMI context

    @param VOID

    @retval TRUE USB SMI occured, context saved
    @retval FALSE There was no USB SMI

**/
BOOLEAN SmmUsbGetContext( 
    VOID 
)
{
    BOOLEAN UsbSmiDetected;

    UsbSmiDetected = UsbSmiDetect(&ActiveUsbSmi);
    SmiContext.UsbContext.Type = UsbLegacy;

    return UsbSmiDetected;
}

/**
    This function dispatches USB SMI event based on context

    @param VOID

    @retval VOID

**/
VOID SmmUsbDispatchSmi( 
    VOID 
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmUsbSmi].RegisteredCallbacks.pHead;
    AMI_SMM_USB_CONTEXT *UsbContext;
    UINT16 ControllerType;

    while(Handler != NULL)
    {
        UsbContext = (AMI_SMM_USB_CONTEXT *)Handler->Context;
        ControllerType = GetControllerType(UsbContext->Device);

        if((ControllerType & ActiveUsbSmi) != 0 && UsbContext->Type == SmiContext.UsbContext.Type)
        {
            Handler->Callback(Handler, UsbContext, NULL, NULL);
        }

        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }
    UsbSmiClear(ActiveUsbSmi);
    ActiveUsbSmi = 0;
}

//-----------------------------------------------------------------------
//              GPI SMI Handler functions
//-----------------------------------------------------------------------

/**
    This function adds GPI SMI handler

    @param VOID *Context - pointer to SMI context

    @retval EFI_STATUS

**/
EFI_STATUS SmmGpiAddHandler(
    IN VOID  *Context
)
{
    AMI_SMM_GPI_CONTEXT *GpiContext = (AMI_SMM_GPI_CONTEXT *)Context;


    if((GpiContext->GpiNum & EnabledGpiSmi) != GpiContext->GpiNum)
    {
        EnabledGpiSmi |= GpiContext->GpiNum;
        GpiSmiSet(EnabledGpiSmi);
    }
    return EFI_SUCCESS;
}

/**
    This function removes GPI SMI handler

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS SmmGpiRemoveHandler( 
    IN VOID  *Context 
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmGpiSmi].RegisteredCallbacks.pHead;
    AMI_SMM_GPI_CONTEXT *GpiContext;
    UINT16 CurrentlyEnabledGpiSmi = 0;

    if(SmmHandler[EfiSmmGpiSmi].RegisteredCallbacks.Size == 0)
    {
        EnabledGpiSmi = 0;
        GpiSmiSet(EnabledGpiSmi);
        return EFI_SUCCESS;
    }

    while(Handler != NULL)
    {
        GpiContext = (AMI_SMM_GPI_CONTEXT *)Handler->Context;
        CurrentlyEnabledGpiSmi |= GpiContext->GpiNum;
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if(CurrentlyEnabledGpiSmi != EnabledGpiSmi)
    {
        EnabledGpiSmi = CurrentlyEnabledGpiSmi;
        GpiSmiSet(EnabledGpiSmi);
    }

    return EFI_SUCCESS;
}

/**
    This function verifies GPI SMI context

    @param VOID *Context - pointer to context

    @retval EFI_INVALID_PARAMETER Given context is invalid
    @retval EFI_SUCCESS Context verified

**/
EFI_STATUS SmmGpiVerifyContext( 
    IN VOID  *Context 
)
{
    AMI_SMM_GPI_CONTEXT *GpiContext = (AMI_SMM_GPI_CONTEXT *)Context;

#if (GPI_DISPATCH_BY_BITMAP != 0)
    if ((GpiContext->GpiNum & (UINT32)SUPPORTED_GPIS) == 0)
#else
    if (((UINT32)(1 << GpiContext->GpiNum) & (UINT32)SUPPORTED_GPIS) == 0)
#endif
        return EFI_INVALID_PARAMETER;

    return EFI_SUCCESS;
}

/**
    This function verifies GPI SMI event and sets GPI SMI context

    @param VOID

    @retval TRUE GPI SMI occured, context saved
    @retval FALSE There was no GPI SMI

**/
BOOLEAN SmmGpiGetContext( 
    VOID 
)
{
    BOOLEAN GpiSmiDetected;
    UINT16  GpiSmiNum;

    GpiSmiDetected = GpiSmiDetect(&GpiSmiNum);
#if (GPI_DISPATCH_BY_BITMAP != 0)
    SmiContext.GpiContext.GpiNum = GpiSmiNum;
#else
    if (GpiSmiDetected) {
        while ((GpiSmiNum % 2) == 0) {
            GpiSmiNum /= 2;
            Index++;
        }
        SmiContext.GpiContext.GpiNum = Index;
    }
#endif

    return GpiSmiDetected;
}

/**
    This function dispatches GPI SMI event based on context

    @param VOID

    @retval VOID

**/
VOID SmmGpiDispatchSmi( 
    VOID 
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmGpiSmi].RegisteredCallbacks.pHead;
    AMI_SMM_GPI_CONTEXT *GpiContext;

    while(Handler != NULL)
    {
        GpiContext = (AMI_SMM_GPI_CONTEXT *)Handler->Context;

#if (GPI_DISPATCH_BY_BITMAP != 0)
        if ((SmiContext.GpiContext.GpiNum & GpiContext->GpiNum) != 0) {
#else
        if (SmiContext.GpiContext.GpiNum == GpiContext->GpiNum) {
#endif
            Handler->Callback( Handler, GpiContext, NULL, NULL );
        }

        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

#if (GPI_DISPATCH_BY_BITMAP != 0)
    GpiSmiClear( (UINT16)SmiContext.GpiContext.GpiNum );
#else
    GpiSmiClear( (UINT16)( 1 << SmiContext.GpiContext.GpiNum) );
#endif
}

//-----------------------------------------------------------------------
//              Standby button SMI Handler functions
//-----------------------------------------------------------------------

/**
    This function adds Standby button SMI handler

    @param VOID *Context - pointer to SMI context

    @retval EFI_STATUS

**/
EFI_STATUS SmmSButtonAddHandler(
    IN VOID  *Context
)
{
    if(SmmHandler[EfiSmmStandbyButtonSmi].RegisteredCallbacks.Size == 1)
        SButtonSmiEnable();

    return EFI_SUCCESS;
}

/**
    This function removes Standby button SMI handler

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS SmmSButtonRemoveHandler( 
    IN VOID  *Context 
)
{
    if(SmmHandler[EfiSmmStandbyButtonSmi].RegisteredCallbacks.Size == 0)
        SButtonSmiDisable();

    return EFI_SUCCESS;
}

#define Entry EfiStandbyButtonEntry 
#define Exit  EfiStandbyButtonExit

/**
    This function verifies Standby button SMI context

    @param VOID *Context - pointer to context

    @retval EFI_INVALID_PARAMETER Given context is invalid
    @retval EFI_SUCCESS Context verified
    @retval EFI_UNSUPPORTED Context is not supported

**/
EFI_STATUS SmmSButtonVerifyContext( 
    IN VOID  *Context 
)
{
    AMI_SMM_STANDBY_BUTTON_CONTEXT *SButtonContext = (AMI_SMM_STANDBY_BUTTON_CONTEXT *)Context;

    if(SButtonContext->Phase > Exit)
        return EFI_INVALID_PARAMETER;

    return (SButtonContext->Phase > Entry) ? EFI_UNSUPPORTED : EFI_SUCCESS;
}

/**
    This function verifies Standby button SMI event and sets Standby button SMI context

    @param VOID

    @retval TRUE Standby button SMI occured, context saved
    @retval FALSE There was no Standby button SMI

**/
BOOLEAN SmmSButtonGetContext( 
    VOID 
)
{
    UINT16 Dummy;
    BOOLEAN SButtonSmiDetected;

    SButtonSmiDetected = SButtonSmiDetect(&Dummy);

    SmiContext.SBtnContext.Phase = Entry;

    return SButtonSmiDetected;
}

/**
    This function dispatches Standby button SMI event based on context

    @param VOID

    @retval VOID

**/
VOID SmmSButtonDispatchSmi( 
    VOID 
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmStandbyButtonSmi].RegisteredCallbacks.pHead;
    AMI_SMM_STANDBY_BUTTON_CONTEXT *SButtonContext;

    while(Handler != NULL)
    {
        SButtonContext = (AMI_SMM_STANDBY_BUTTON_CONTEXT *)Handler->Context;

        if(SButtonContext->Phase == SmiContext.SBtnContext.Phase) {
            Handler->Callback( Handler, SButtonContext, NULL, NULL );
        }

        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }
    SButtonSmiClear();
}

//-----------------------------------------------------------------------
//              Power button SMI Handler functions
//-----------------------------------------------------------------------

/**
    This function adds Power button SMI handler

    @param VOID *Context - pointer to SMI context

    @retval EFI_STATUS

**/
EFI_STATUS SmmPButtonAddHandler(
    IN VOID  *Context
)
{
    if(SmmHandler[EfiSmmPowerButtonSmi].RegisteredCallbacks.Size == 1)
        PButtonSmiEnable();

    return EFI_SUCCESS;
}

/**
    This function removes Power button SMI handler

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS SmmPButtonRemoveHandler( 
    IN VOID  *Context 
)
{
    if(SmmHandler[EfiSmmPowerButtonSmi].RegisteredCallbacks.Size == 0)
        PButtonSmiDisable();

    return EFI_SUCCESS;
}

#define PowerButtonEntry EfiPowerButtonEntry
#define PowerButtonExit  EfiPowerButtonExit

/**
    This function verifies Power button SMI context

    @param VOID *Context - pointer to context

    @retval EFI_INVALID_PARAMETER Given context is invalid
    @retval EFI_SUCCESS Context verified
    @retval EFI_UNSUPPORTED Context is not supported

**/
EFI_STATUS SmmPButtonVerifyContext( 
    IN VOID  *Context 
)
{
    AMI_SMM_POWER_BUTTON_CONTEXT *PButtonContext = (AMI_SMM_POWER_BUTTON_CONTEXT *)Context;

    if(PButtonContext->Phase > PowerButtonExit)
        return EFI_INVALID_PARAMETER;

    return (PButtonContext->Phase > PowerButtonEntry) ? \
                                               EFI_UNSUPPORTED : EFI_SUCCESS;
}

/**
    This function verifies Power button SMI event and sets Power button SMI context

    @param VOID

    @retval TRUE Power button SMI occured, context saved
    @retval FALSE There was no Power button SMI

**/
BOOLEAN SmmPButtonGetContext( 
    VOID 
)
{
    UINT16 Dummy;
    BOOLEAN PButtonSmiDetected;

    PButtonSmiDetected = PButtonSmiDetect(&Dummy);

    SmiContext.PBtnContext.Phase = PowerButtonEntry;

    return PButtonSmiDetected;
}

/**
    This function dispatches Power button SMI event based on context

    @param VOID

    @retval VOID

**/
VOID SmmPButtonDispatchSmi( 
    VOID 
)
{
    HANDLER_LINK *Handler = (HANDLER_LINK *)SmmHandler[EfiSmmPowerButtonSmi].RegisteredCallbacks.pHead;
    AMI_SMM_POWER_BUTTON_CONTEXT *PButtonContext;

    while(Handler != NULL)
    {
        PButtonContext = (AMI_SMM_POWER_BUTTON_CONTEXT *)Handler->Context;

        if(PButtonContext->Phase == SmiContext.PBtnContext.Phase) {
            Handler->Callback( Handler, PButtonContext, NULL, NULL );
        }

        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }
    PButtonSmiClear();

    SbLib_Shutdown();
}

//---------------------------------------------------------------------------
//                      I/O Trap SMI Handler functions
//---------------------------------------------------------------------------

/**
    This function adds I/O Trap SMI handler

    @param *Context - Pointer to SMI context

    @retval EFI_SUCCESS

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmmIoTrapAddHandler (
    IN VOID             *Context )
{
/*
    HANDLER_LINK                        *Handler;
    SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE    *RegisteredContext;
    SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE    *IoTrapContext;
    UINTN                               i = 0;
    UINT32                              TrapRegIndex = 0;

    IoTrapContext = (SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE *)Context;

    Handler = (HANDLER_LINK *)SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.pHead;

    while (Handler != NULL) {
        RegisteredContext = (SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE *)Handler->Context;
        if (RegisteredContext->IoTrapContext.Address == IoTrapContext->IoTrapContext.Address) {
            TrapRegIndex = RegisteredContext->TrapRegIndex;
            i++;
        }
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if (i > 1) {
        IoTrapContext->TrapRegIndex = TrapRegIndex;
        return EFI_SUCCESS;
    }

    IoTrapSmiSet( IoTrapContext );

    gEnabledIoTrapSmi |= (1 << (UINT32)(IoTrapContext->TrapRegIndex));

    if (SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.Size == 1)
        IoTrapSmiEnable();

    return EFI_SUCCESS;
*/
    HANDLER_LINK                        *Handler;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *RegedContext;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *IoTrapContext;
    UINT32                              TrapRegIndex = 0;
    UINT32                              i = 0;

    IoTrapContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Context;

    Handler = \
        (HANDLER_LINK *)SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.pHead;

    while (Handler != NULL) {
        RegedContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Handler->Context;
        if ((RegedContext->Address == IoTrapContext->Address) && \
            (RegedContext->Type == IoTrapContext->Type) && \
            (RegedContext->Length == IoTrapContext->Length)) {
            i++;
        }
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if (i > 1) return EFI_SUCCESS;

    IoTrapSmiSet( IoTrapContext );

    gEnabledIoTrapSmi |= (UINT32)(1 << TrapRegIndex);

    if (SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.Size == 1)
        IoTrapSmiEnable();

    return EFI_SUCCESS;
}

/**
    This function removes I/O Trap SMI handler

    @param *Context - Pointer to SMI context

    @retval EFI_SUCCESS

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmmIoTrapRemoveHandler (
    IN VOID             *Context )
{
/*
    HANDLER_LINK                        *Handler;
    SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE    *RegisteredContext;
    UINTN       i = 0;
    UINT32      RemoveIoTrapRegIndex = (((SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE *)Context)->TrapRegIndex);

    Handler = (HANDLER_LINK *)SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.pHead;

    while (Handler != NULL) {
        RegisteredContext = (SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE *)Handler->Context;
        if (RegisteredContext->TrapRegIndex == RemoveIoTrapRegIndex) 
            i++;
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if (i > 1) return EFI_SUCCESS;

    IoTrapSmiReset( Context );
   
    gEnabledIoTrapSmi &= ~(1 << RemoveIoTrapRegIndex);

    if (gEnabledIoTrapSmi == 0) IoTrapSmiDisable();

    return EFI_SUCCESS;
*/
    HANDLER_LINK                        *Handler;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *RegedContext;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *RemoveContext;
    UINT32                              i = 0;
    UINT32                              TrapRegIndex = 0;

    RemoveContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Context;

    Handler = \
        (HANDLER_LINK *)SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.pHead;

    while (Handler != NULL) {
        RegedContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Handler->Context;
        if ((RegedContext->Address == RemoveContext->Address) && \
            (RegedContext->Type == RemoveContext->Type) && \
            (RegedContext->Length == RemoveContext->Length)) i++;
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if (i > 1) return EFI_SUCCESS;

    IoTrapSmiReset( Context );
   
    gEnabledIoTrapSmi &= ~(1 << TrapRegIndex);

    if (gEnabledIoTrapSmi == 0) IoTrapSmiDisable();

    return EFI_SUCCESS;

}

/**
    This function verifies I/O Trap SMI context

    @param *Context - Pointer to SMI context

        
    @retval EFI_SUCCESS Context verified
    @retval EFI_INVALID_PARAMETER Given context is invalid
    @retval EFI_UNSUPPORTED Context is not supported
    @retval EFI_OUT_OF_RESOURCES There is no I/O Trap register
        available

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmmIoTrapVerifyContext (
    IN VOID                 *Context )
{
/*
    HANDLER_LINK                        *Handler;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *RegisteredContext;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *IoTrapContext;

    IoTrapContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Context;
    if ( IoTrapContext->Length > MAX_SUPPORTED_IOTRAP_LENGTH)
        return EFI_INVALID_PARAMETER;

    Handler = (HANDLER_LINK *)SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.pHead;

    while (Handler != NULL) {
        RegisteredContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Handler->Context;
        if (RegisteredContext->Address == IoTrapContext->Address) {
            return EFI_SUCCESS;
        }
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if (gEnabledIoTrapSmi >= ((1 << MAX_SUPPORTED_IOTRAP_REGS) - 1))
        return EFI_OUT_OF_RESOURCES;


    return EFI_SUCCESS;
*/
    HANDLER_LINK                        *Handler;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *RegedContext;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *IoTrapContext;

    Handler = \
        (HANDLER_LINK *)SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.pHead;

    IoTrapContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Context;

    while (Handler != NULL) {
        RegedContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Handler->Context;
        if ((RegedContext->Address == IoTrapContext->Address) && \
            (RegedContext->Type == IoTrapContext->Type)) {
            if ( IoTrapContext->Length > MAX_SUPPORTED_IOTRAP_LENGTH)
                return EFI_INVALID_PARAMETER;
            return EFI_SUCCESS;
        }
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    if (gEnabledIoTrapSmi >= ((1 << MAX_SUPPORTED_IOTRAP_REGS) - 1))
        return EFI_OUT_OF_RESOURCES;

    if ( IoTrapContext->Length > MAX_SUPPORTED_IOTRAP_LENGTH)
        return EFI_INVALID_PARAMETER;

    return EFI_SUCCESS;

}

/**
    This function verifies I/O Trap SMI event and sets 
    I/O Trap SMI context.

    @param VOID

        
    @retval TRUE I/O Trap SMI occured, context saved
    @retval FALSE There was no I/O Trap SMI

    @note  GENERALLY NO PORTING REQUIRED
**/

BOOLEAN SmmIoTrapGetContext (VOID)
{
    return IoTrapSmiDetect( &SmiContext.IoTrapContext );
}

/**
    This function dispatches I/O Trap SMI event based on context.

    @param VOID

    @retval VOID

    @note  GENERALLY NO PORTING REQUIRED
**/

VOID SmmIoTrapDispatchSmi (VOID)
{
/*
    HANDLER_LINK                            *Handler;
    SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE    *IoTrapContext;
    UINT32 TrapRegIndex;

//get TrapRegIndex from Context
    IoTrapContext = (SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE *)(&SmiContext.IoTrapContext);
    TrapRegIndex = IoTrapContext->TrapRegIndex;

    Handler = (HANDLER_LINK *)SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.pHead;
    while (Handler != NULL) {
        IoTrapContext = (SMM_IO_TRAP_REGISTER_CONTEXT_PRIVATE *)Handler->Context;

        if ( (IoTrapContext->TrapRegIndex == TrapRegIndex) &&
             ( gEnabledIoTrapSmi & (1 << IoTrapContext->TrapRegIndex))) {
                if (Handler->Callback != NULL)
                    Handler->Callback(Handler, &SmiContext.IoTrapContext);
                if (Handler->Callback2 != NULL)
                    Handler->Callback2(Handler,&SmiContext.IoTrapContext, \
                                NULL, NULL);
            }


        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    IoTrapSmiClear();
*/
    HANDLER_LINK                        *Handler;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *IoTrapContext;
    volatile UINT16                     MaxAddress;

    Handler = \
        (HANDLER_LINK *)SmmHandler[EfiSmmIoTrapSmi].RegisteredCallbacks.pHead;
    while (Handler != NULL) {
        IoTrapContext = (EFI_SMM_IO_TRAP_REGISTER_CONTEXT *)Handler->Context;
        MaxAddress = IoTrapContext->Address + IoTrapContext->Length;
        if ((IoTrapContext->Address <= SmiContext.IoTrapContext.Address) && \
            (MaxAddress > SmiContext.IoTrapContext.Address)) {
            if ((IoTrapContext->Type == ReadWriteTrap) || \
               (IoTrapContext->Type == SmiContext.IoTrapContext.Type)) {
                Handler->Callback( Handler, &SmiContext.IoTrapContext, \
                                   NULL, NULL );
            }
        }
        Handler = (HANDLER_LINK *)Handler->Link.pNext;
    }

    IoTrapSmiClear();
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
