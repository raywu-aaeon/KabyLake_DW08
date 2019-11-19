
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
//
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/CougarPoint_A/Chipset/SBRun.c 11    3/07/11 4:10a Yurenlai $
//
// $Revision: 11 $
//
// $Date: 3/07/11 4:10a $
//
//*****************************************************************************

//*****************************************************************************


/** @file SbRun.c
    This file contains code for SouthBridge runtime
    protocol

**/

// Module specific Includes

#include "Efi.h"
#include "Token.h"
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Library/AmiChipsetRuntimeServiceLib.h>
#include <Library/AmiSbMiscLib.h>
#include <Protocol/Reset.h>
// Produced Protocols
#include <Protocol/Metronome.h>
#include <Protocol/RealTimeClock.h>

#if defined SPS_MePlatformReset_SUPPORT && SPS_MePlatformReset_SUPPORT== 1
#undef MePlatformReset_SUPPORT
#define MePlatformReset_SUPPORT 1
#endif

#ifdef AMI_MODULE_PKG_VERSION
#if AMI_MODULE_PKG_VERSION >= 28
#include <Guid/AmiResetSystemEvent.h>
#endif
#endif

#ifndef MePlatformReset_SUPPORT
#define MePlatformReset_SUPPORT 0
#endif



//----------------------------------------------------------------------------
//          Variable Declaration
//----------------------------------------------------------------------------
EFI_HANDLE mResetProtocolHandle  = NULL;
EFI_RESET_SYSTEM gCallSavedIntelPointer;

EFI_EVENT gAmiSystemResetEvent;
static BOOLEAN gExitBs = FALSE;

#ifdef AMI_MODULE_PKG_VERSION
#if AMI_MODULE_PKG_VERSION < 28
#define AMI_RESET_SYSTEN_EVENT_GUID { 0x62da6a56, 0x13fb, 0x485a, 0xa8, 0xda, 0xa3, 0xdd, 0x79, 0x12, 0xcb, 0x6b }
EFI_GUID gAmiResetSystemEventGuid = AMI_RESET_SYSTEN_EVENT_GUID;
#endif
#endif

// Function Prototypes
EFI_STATUS EFIAPI
WaitForTick (
    IN  EFI_METRONOME_ARCH_PROTOCOL     *This,
    IN  UINT32                          TickNumber
);

// Architectural Protocol Definitions
EFI_METRONOME_ARCH_PROTOCOL mMetronomeProtocol = {
    WaitForTick,
    //
    // The value of TickPeriod in EFI_METRONOME_ARCH_PROTOCOL on any
    // platform must be at least 10 uS and must not exceed 200 uS.
    // The unit of TickPeriod is 100ns.
    // 100 = 10uS/100ns
    // The value of TickPeriod should be between 100 and 2000.
    //
    TICK_PERIOD
};

// Function Definitions

//----------------------------------------------------------------------------
//   USUALLY NO PORTING REQUIRED FOR THE FOLLOWING ROUTINES
//----------------------------------------------------------------------------

/**
    This function calculates the time needed to delay and then
    calls a library function to delay that amount of time

    @param This Pointer to the instance of the Metronome Arch Protocol
    @param TickNumber Number of ticks needed based off of tick period
                      defined in Protocol Definition

    @retval Return Status based on errors that occurred while waiting for
            time to expire.

**/

EFI_STATUS EFIAPI
WaitForTick (
    IN  EFI_METRONOME_ARCH_PROTOCOL     *This,
    IN  UINT32                          TickNumber
    )
{
    EFI_STATUS          Status;
    UINT32              TotalTime;

    // Manipulate TickNumber into a valid value for the library function call
    // the Current Resolution is 10us.
    // The Library uses Microseconds to count delayed time.
    TotalTime = (TickNumber * This->TickPeriod) / 10;

    // Make Library Function call here
    Status = CountTime(TotalTime, PM_BASE_ADDRESS);

    return Status;
}

EFI_STATUS EFIAPI ResetSystemDispatcher (
    IN EFI_RESET_TYPE   ResetType,
    IN EFI_STATUS       ResetStatus,
    IN UINTN            DataSize,
    IN CHAR16           *ResetData OPTIONAL
)
{
	EFI_TPL                 OldTpl;
	
    if (!gExitBs) {
	    pBS->SignalEvent(gAmiSystemResetEvent);
	    
	    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);

	    if (OldTpl > TPL_APPLICATION) {
	        pBS->RestoreTPL(TPL_APPLICATION);
	        pBS->RaiseTPL(OldTpl);
	    } else {
	        pBS->RestoreTPL(OldTpl);
	    }
	    
	    pBS->CloseEvent(gAmiSystemResetEvent);
	}
    
#if SB_RESET_SHUTDOWN_OVERRIDE 		
    if ((gCallSavedIntelPointer == NULL) || \
        (ResetType == EfiResetWarm) || \
        (ResetType == EfiResetShutdown))
#else 
    if ((gCallSavedIntelPointer == NULL) || \
        (ResetType == EfiResetWarm)) 
#endif        
    {
      AmiChipsetResetSystem ( \
              ResetType, \
              ResetStatus, \
              DataSize, \
              ResetData);
    } else {
      gCallSavedIntelPointer( \
              ResetType, \
              ResetStatus, \
              DataSize, \
              ResetData );
    }
    // This should not get here
    return EFI_SUCCESS;
}

/**

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID 
EFIAPI
SaveIntelResetPointer (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context )
{
    gCallSavedIntelPointer = pRS->ResetSystem;
    pRS->ResetSystem = (EFI_RESET_SYSTEM) ResetSystemDispatcher;

    pBS->CloseEvent(Event);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SBRunExitBS
//
// Description:
//
// Input: IN EFI_EVENT Event,
//        IN VOID *Context
//
// Output: None
//
// Notes:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

/**
    Update the global variable ExitBs before exiting boot services.

    @param Event   Event of callback
    @param Context Pointer to context of callback

    @retval VOID

**/

VOID SbExitBs (
    IN EFI_EVENT Event,
    IN VOID *Context )
{
    gExitBs = TRUE;
}

/**
    A dummy function.

    @param Event   Event of callback
    @param Context Pointer to context of callback

    @retval VOID

**/
VOID SbRunDummyFunction (
    IN EFI_EVENT Event,
    IN VOID *Context )
{
}

/**
    Submit runtime services both SMM and runtime.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS
EFIAPI
SbRuntimeServiceInit (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    EFI_STATUS        Status = EFI_SUCCESS;
    EFI_EVENT         Event = NULL;
    VOID              *Registration = NULL;

    pRS->ResetSystem    = (EFI_RESET_SYSTEM) ResetSystemDispatcher;

    gCallSavedIntelPointer = NULL;

#if MePlatformReset_SUPPORT
    Status = RegisterProtocolCallback( \
               &gEfiResetArchProtocolGuid, \
               SaveIntelResetPointer, \
               NULL, \
               &Event, \
               &Registration );
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }
#endif

    pRS->GetTime        = AmiChipsetGetTime;
    pRS->SetTime        = AmiChipsetSetTime;
    pRS->GetWakeupTime  = AmiChipsetGetWakeupTime;
    pRS->SetWakeupTime  = AmiChipsetSetWakeupTime;
    
    return EFI_SUCCESS;
}

/**
    This function is the entry point for this DXE. This function
    installs the runtime services related to SB

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS
EFIAPI
SbRuntimeInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_HANDLE              Handle;
    EFI_EVENT               ExitBsEvent;

    InitAmiRuntimeLib(ImageHandle, SystemTable, NULL, NULL);

    PROGRESS_CODE(DXE_SBRUN_INIT);
    //
    // Install runtime services
    //
    Status = SbRuntimeServiceInit(ImageHandle, SystemTable);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }

    Status = pBS->InstallProtocolInterface (
                    &ImageHandle, \
                    &gEfiMetronomeArchProtocolGuid, \
                    EFI_NATIVE_INTERFACE, \
                    &mMetronomeProtocol
                    );
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }


#if MePlatformReset_SUPPORT == 0
    Status = pBS->InstallProtocolInterface(
                   &mResetProtocolHandle,
                   &gEfiResetArchProtocolGuid,
                   EFI_NATIVE_INTERFACE,
                   NULL);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }

#endif
    
    Status = pBS->CreateEventEx(
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     SbRunDummyFunction,
                     NULL,
                     &gAmiResetSystemEventGuid,
                     &gAmiSystemResetEvent );
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }

    
    Status = pBS->CreateEvent( 
                        EVT_SIGNAL_EXIT_BOOT_SERVICES,
                        TPL_CALLBACK, 
                        SbExitBs, 
                        NULL, 
                        &ExitBsEvent );    
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }


    //
    // This protocol is to notify core that the Runtime Table has been
    // updated, so it can update the runtime table CRC.
    //
    Handle = NULL;
    return pBS->InstallMultipleProtocolInterfaces (
                  &Handle, \
                  &gEfiRealTimeClockArchProtocolGuid, \
                  NULL, \
                  NULL
                  );
}

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
