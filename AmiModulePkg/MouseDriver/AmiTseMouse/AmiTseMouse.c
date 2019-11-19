//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiTseMouse.c
    Provides AMITSE respective Mouse hook functions. 

**/

//---------------------------------------------------------------------------

#include "AmiTseMouse.h"

//---------------------------------------------------------------------------

#if MINISETUP_MOUSE_SUPPORT 
#include <Protocol/AMIPostMgr.h>

#if TSE_STYLE_GTSE_BIN_SUPPORT
    #include "EDK/MiniSetup/GTSE/gtseConfig.h"
#endif
#if CHANGE_TIMER_TICK_INTERVAL
    #include <Protocol/Timer.h>
#endif
extern UINTN gPostStatus;
    BOOLEAN  gNumericSoftkbdActivated = FALSE;
    
#if defined TSE_BUILD && TSE_BUILD >= 0x1255
extern BOOLEAN SoftKbdRefresh;
#endif
    
#if CHANGE_TIMER_TICK_INTERVAL
    EFI_TIMER_ARCH_PROTOCOL    *gTimer = NULL;
    UINT64   gTimerTickDefaultDuration = 0;
#endif

#if OVERRIDE_TSEMouseInit

/**
    Function to initialize the mouse

    @param  VOID

    @retval VOID

**/
VOID MemCopy( VOID *dest, VOID *src, UINTN size );
extern DXE_MOUSE_PROTOCOL FakeTSEMouse;
EFI_STATUS TSE_SOFTKBD_GetButtonStatus (DXE_MOUSE_PROTOCOL *This, INT32 *ButtonStatus );
VOID 
TSEMouseInit(VOID)
{
    EFI_STATUS      Status;
    UINTN           HorizontalResolution = 0;
    UINTN           VerticalResolution = 0;
    UINTN           MaxRows=0;
    UINTN           MaxColumns=0;

    Status = gBS->LocateProtocol(&gMouseDriverGuid, NULL, (VOID**) &TSEMouse);
    if (EFI_ERROR(Status)) { 
        TSEMouse = NULL;
        return;
    } else {
        //Initialize the Mouse to Full Screen Resolution in case of QuietBoot
        if(TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN == gPostStatus) {
            GetScreenResolution(&HorizontalResolution, &VerticalResolution);
            if (TSEMouse) {
#if OLD_MOUSE_DRIVER_SUPPORT 
                TSEMouse->Initialize(
                         (UINT32)0,
                         (UINT32)0,
                         (UINT32)HorizontalResolution,
                         (UINT32)VerticalResolution
                         );
#else
                TSEMouse->InitializePointingDevice(TSEMouse,
                         (UINT32)0,
                         (UINT32)0,
                         (UINT32)HorizontalResolution,
                         (UINT32)VerticalResolution
                         );
#endif
            }
        } else {
            Status = gST->ConOut->QueryMode(gST->ConOut,
                    gST->ConOut->Mode->Mode,
                    &MaxColumns,
                    &MaxRows);

            if (EFI_ERROR( Status ))
                TSEMouse = NULL;

            Status = GetScreenResolution(&HorizontalResolution, &VerticalResolution);

            if (EFI_ERROR( Status ))
                  TSEMouse = NULL;

            if (TSEMouse) {

#if TSE_STYLE_GTSE_BIN_SUPPORT
                {
                    GTSESetupConfigData* Data;
                    Data = GTSEGetConfigData();
                    Screen_Top =  Data->StartY ;
                    Screen_Left = Data->StartX ;
                } 
#else
                Screen_Top = (INT32)(VerticalResolution - (MaxRows*HiiGetGlyphHeight()) ) /2;
                Screen_Left = (INT32)(HorizontalResolution - (MaxColumns*HiiGetGlyphWidth()) ) /2;
#endif

#if OLD_MOUSE_DRIVER_SUPPORT 
                TSEMouse->Initialize(
                            (UINT32)Screen_Left,
                            (UINT32)Screen_Top,
                            (UINT32)(Screen_Left + (MaxColumns*HiiGetGlyphWidth())),
                            (UINT32)(Screen_Top + (MaxRows*HiiGetGlyphHeight()))
                            );
#else
                //Initializing the Mouse (CURSOR,BOUNDARY,MAPPING etc)
                TSEMouse->InitializePointingDevice(TSEMouse,
                            (INT32)Screen_Left,
                            (INT32)Screen_Top,
                            (INT32)(Screen_Left + (MaxColumns*HiiGetGlyphWidth())),
                            (INT32)(Screen_Top + (MaxRows*HiiGetGlyphHeight()))
                            );

#endif
            }
        }
#if CHANGE_TIMER_TICK_INTERVAL 
    // Locate TimerArchProtocol in case of not located yet.
    if(gTimer == NULL) {
        Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &gTimer);
        if (Status != EFI_SUCCESS) { 
            gTimer = NULL;
            ASSERT_EFI_ERROR(Status);
        }
    } 
    if (gTimer != NULL)  {
        // Saving the default Timer Value before setting timer with TIMER_TICK_INTERVAL_ON_SETUP.
        Status = gTimer->GetTimerPeriod(gTimer,&gTimerTickDefaultDuration);
        if (!EFI_ERROR(Status)) {
            // Setting the timer with TIMER_TICK_INTERVAL_ON_SETUP
            Status = gTimer->SetTimerPeriod(gTimer,TIMER_TICK_INTERVAL_ON_SETUP);
            ASSERT_EFI_ERROR(Status);
        }
    }
#endif
    
#if AMITSE_SOFTKBD_SUPPORT
    
    //In POST, SoftKbd consumes all mouse clicks.Hence, Fake mouse protocol is supported only after POST.[EIP 206420]
    if ( TSEMouse && ( TSE_POST_STATUS_IN_TSE == gPostStatus)) {
       MemCopy (&FakeTSEMouse, TSEMouse, sizeof (DXE_MOUSE_PROTOCOL));
       FakeTSEMouse.GetButtonStatus = TSE_SOFTKBD_GetButtonStatus;
        
      Status = gBS->LocateProtocol(&gSoftKbdGuid, NULL, &TSESoftKbd_n);
      if (!EFI_ERROR (Status)) {
          TSESoftKbd_n->UpdateMouseInterface (TSESoftKbd_n, &FakeTSEMouse);
      }
    }       
#endif
    }
    TSEMouseInitHook();
}
#endif

#if OVERRIDE_TSEMouseStop

/**
    Function to handle the mouse stop

    @param  VOID

    @retval VOID

**/

VOID 
TSEMouseStop(VOID)
{
    //Stop mouse pointer draw
    if (TSEMouse != NULL)
    {
#if OLD_MOUSE_DRIVER_SUPPORT 
        TSEMouse->Stop();
#else
        TSEMouse->StopUpdatingMouseCursor(TSEMouse);
#endif
     }

    TSEMouseStopHook();
}
#endif

#if OVERRIDE_TSEIsMouseClickedonSoftkbd

/**
    Function to handle the mouse Clicked on Softkbd

    @param  VOID

    @retval VOID

**/

BOOLEAN 
TSEIsMouseClickedonSoftkbd(VOID)
{
#if AMITSE_SOFTKBD_SUPPORT  
    EFI_STATUS Status;
    INT32 x = 0,y = 0,z = 0;
    UINT32 SoftKbdStartX = 0, SoftKbdStartY = 0, SoftKbdWidth = 0, SoftKbdHeight = 0;

    Status = gBS->LocateProtocol(&gSoftKbdGuid, NULL, (VOID**) &TSESoftKbd_n);
    if (EFI_ERROR(Status)) { 
       TSESoftKbd_n=NULL;
       return FALSE;
    }
    if ( NULL == TSEMouse ) {
        return FALSE;
    }
    TSEMouse->GetCoordinates(TSEMouse,&x, &y, &z);
    TSESoftKbd_n->GetPosition(TSESoftKbd_n, FALSE , &SoftKbdStartY, &SoftKbdStartX);
    TSESoftKbd_n->GetDimension(TSESoftKbd_n, &SoftKbdWidth, &SoftKbdHeight);
    //
    //EIP 83753 : SoftKbd flickers when positioned near Top/Left end of the screen display. 
    //
    if((( (INT32)x > (INT32)(SoftKbdStartX-HiiGetGlyphHeight()))&&
         ((INT32)y > (INT32)(SoftKbdStartY-(2*HiiGetGlyphHeight()))))&&
        (((INT32)x < (INT32)(SoftKbdStartX+SoftKbdWidth))&&
         ((INT32)y < (INT32)(SoftKbdStartY + SoftKbdHeight))) ) {
        return TRUE;
    } else {
        return FALSE;
    }
#endif
return FALSE;
}
#endif

#if OVERRIDE_TSEMouseRefresh

/**
    Function to handle the mouse Refresh

    @param  VOID

    @retval VOID

**/

VOID 
TSEMouseRefresh(VOID)
{
    if (TSEMouse == NULL) {
        return;
    }
    if ((TRUE == TSEMouseIgnoreMouseActionHook()) &&
         (TRUE == TSEIsMouseClickedonSoftkbd())) {
        TSEMouse->StopUpdatingMouseCursor(TSEMouse);
    }
#if defined TSE_BUILD && TSE_BUILD >= 0x1255
    if(SoftKbdRefresh == TRUE ) {              // EIP214829: Avoid SoftKbdRefresh for Popup case in Setup 
#endif	    	    
    TSEMouseRefreshHook();
#if defined TSE_BUILD && TSE_BUILD >= 0x1255
    }
    SoftKbdRefresh = TRUE;
#endif
    
    //
    //refresh mouse pointer
    //
    if (TSEMouse != NULL) {
#if OLD_MOUSE_DRIVER_SUPPORT 
        TSEMouse->Refresh();
#else
    //
    // Do not display Mouse Pointer when hidden by SoftKbd
    //
    if (!gTseMousePointerHide && !SoftKbdPointerHide()) {
        TSEMouse->UpdateMousePointerProperty(TSEMouse, MOUSE_POINTER_PROPERTY_NORMAL);
    }
    TSEMouse->RefreshMouseCursor(TSEMouse);
#endif
    }
}
#endif

#if OVERRIDE_TSEMouseStart

/**
    Function to handle the mouse Startd

    @param  VOID

    @retval VOID

**/

VOID 
TSEMouseStart(VOID)
{
    TSEMouseStartHook();

    if (TSEMouse != NULL)
#if OLD_MOUSE_DRIVER_SUPPORT 
        TSEMouse->Start();
#else
        TSEMouse->StartUpdatingMouseCursor(TSEMouse);
#endif
}
#endif

#if OVERRIDE_TSEMouseFreeze

/**
    Function to handle the mouse Destory

    @param  VOID

    @retval VOID

**/

VOID 
TSEMouseFreeze(VOID)
{
    //
    //Stop mouse pointer draw
    //
    if (TSEMouse != NULL)
#if OLD_MOUSE_DRIVER_SUPPORT 
        return;
#else
    //
    // Do not display Mouse Pointer when hidden by SoftKbd
    //
    if (!gTseMousePointerHide && !SoftKbdPointerHide()) {
        TSEMouse->UpdateMousePointerProperty(TSEMouse ,MOUSE_POINTER_PROPERTY_FREEZE);
    }
#endif

}
#endif
/**
    Function to handle the mouse pointer hide

    @param  BOOLEAN Hide 
            TRUE - Hide mouse cursor
            FALSE - Do not hide Mouse cursor

    @retval VOID

**/
VOID TSEMouseHide(BOOLEAN Hide)
{
    if(TSEMouse!=NULL) {
#if OLD_MOUSE_DRIVER_SUPPORT 
        return;
#else
        if (Hide) {
            if (!gTseMousePointerHide) {
                TSEMouse->UpdateMousePointerProperty(TSEMouse ,MOUSE_POINTER_PROPERTY_HIDE);
                gTseMousePointerHide = TRUE;
            }
        } else {
            if (gTseMousePointerHide) {
                TSEMouse->UpdateMousePointerProperty(TSEMouse ,MOUSE_POINTER_PROPERTY_NORMAL);
                gTseMousePointerHide = FALSE;
            }
        }
#endif
    }

}

/**
    Checks whether mouse pointer hidden by SoftKbd

    @param  BOOLEAN Hide 
            TRUE - mouse pointer hidden by softkbd
            FALSE - mouse is not hidden by softkbd

    @retval VOID

**/
BOOLEAN SoftKbdPointerHide(VOID)
{
#if AMITSE_SOFTKBD_SUPPORT
    #if  MOUSE_POINTER_BEHAVIOR == 3 
        return TRUE;
    #else
        return FALSE;
    #endif
#else
        return FALSE; 
#endif
}

#if OVERRIDE_TSEGetCoordinates

/**
    Function to handle the mouse Destory

    @param  VOID

    @retval EFI_STATUS

**/

EFI_STATUS 
TSEGetCoordinates (INT32 *x, INT32 *y, INT32 *z)
{
    //
    //Stop mouse pointer draw
    //
    if (TSEMouse != NULL)
#if OLD_MOUSE_DRIVER_SUPPORT 
        return;
#else
        TSEMouse->GetCoordinates(TSEMouse , x, y, z);
#endif
    return EFI_SUCCESS;
}
#endif

#if OVERRIDE_TSEMouseReadInfo

/**
    Function to read the information using mouse

    @param  MOUSE_INFO *MouseInfo 

    @retval EFI_STATUS

**/

EFI_STATUS 
TSEMouseReadInfo(MOUSE_INFO *MouseInfo)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    INT32 Button_Status = 0;
    INT32 x = 0,y = 0, z = 0;
    UINT32 SoftKbdStartX = 0, SoftKbdStartY = 0, SoftKbdWidth = 0, SoftKbdHeight = 0;

    if (NULL == TSEMouse)
        return EFI_UNSUPPORTED;

    TSEMouse->MousePoll(TSEMouse);
    TSEMouse->GetCoordinates(TSEMouse, &x, &y, &z);

    // If mouse action are consumed by Modules like SoftKeyBoard.
    if (TSEMouseIgnoreMouseActionHook() == TRUE) {

#if AMITSE_SOFTKBD_SUPPORT
     Status = gBS->LocateProtocol(&gSoftKbdGuid, NULL, (VOID**) &TSESoftKbd_n);
     if (EFI_ERROR(Status)) { 
         TSESoftKbd_n=NULL;
         return Status;
     }
    
     TSESoftKbd_n->GetPosition(TSESoftKbd_n,FALSE, &SoftKbdStartY,&SoftKbdStartX);
     TSESoftKbd_n->GetDimension(TSESoftKbd_n,&SoftKbdWidth, &SoftKbdHeight);

     if ((( x > (INT32)SoftKbdStartX) && (y > (INT32)SoftKbdStartY)) &&
          (( x <((INT32)SoftKbdStartX + (INT32)SoftKbdWidth))&&
          ( y <((INT32)SoftKbdStartY + (INT32)SoftKbdHeight))) ) {
         
            // Updating the Mouse Interfaces to Softkeyboard with FakeTSEMouse instance in TSEMouseInit function.
            // So commenting here for this issue purpose[EIP 103757].
         
            // TSESoftKbd_n->UpdateMouseInterface(TSESoftKbd_n, TSEMouse);
            //
            // Hide Mouse Pointer when pointer is on SoftKbd
            //
            #if MOUSE_POINTER_BEHAVIOR == 2
                TSEMouseHide(TRUE);
            #endif
            return EFI_NOT_FOUND;
        } //else
           // TSESoftKbd_n->UpdateMouseInterface(TSESoftKbd_n, NULL);
#endif
           // return EFI_NOT_FOUND;
    }
#if OLD_MOUSE_DRIVER_SUPPORT 
    //
    //Getting Mouse Button Status
    //
    TSEMouse->GetButtonStatus(&Button_Status);
    //
    //Getting Mouse Pointer Position
    //
    TSEMouse->GetPosition((INT32*)&x, (INT32*)&y);
#else
    //
    // Display Mouse Pointer when pointer is not on SoftKbd
    //
    #if MOUSE_POINTER_BEHAVIOR == 2
       TSEMouseHide(FALSE);
    #endif
   TSEMouse->GetButtonStatus(TSEMouse, &Button_Status);
   // 
   //Getting Mouse Pointer Position
   //
   TSEMouse->GetCoordinates(TSEMouse, &x, &y, &z);
#endif

    y -= Screen_Top;
    x -= Screen_Left;

    //Converting Graphics Resolution to Text Mode Location
    MouseInfo->Left = (UINT16)(x/HiiGetGlyphWidth());
    MouseInfo->Top = (UINT16)(y/HiiGetGlyphHeight());

    //Setting the Calculated SCREEN_TOP and SCREEN_LEFT Rather than Hardcoding
//    MouseInfo->Top-=Screen_Top;
//    MouseInfo->Left-=Screen_Left;
    MouseInfo->ButtonStatus = Button_Status;

    return EFI_SUCCESS;
}
#endif

#if OVERRIDE_TSEMouseDestroy

/**
    Function to handle the mouse Destroy

    @param VOID

    @retval VOID

**/

VOID 
TSEMouseDestroy(VOID)
{

#if CHANGE_TIMER_TICK_INTERVAL
    EFI_STATUS Status;
#endif
    //
    //Stop mouse pointer draw
    //
    if (TSEMouse != NULL) {
#if OLD_MOUSE_DRIVER_SUPPORT 
        TSEMouse->Stop();
#else    
        TSEMouse->StopUpdatingMouseCursor(TSEMouse);
        TSEMouse->StopPointingDevice(TSEMouse);    //EIP-84150 
#endif
    }

#if CHANGE_TIMER_TICK_INTERVAL

    // Locate TimerArchProtocol in case if its not located yet.
    if(gTimer == NULL) {
        Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &gTimer);
        if (Status != EFI_SUCCESS) { 
            gTimer = NULL;
            ASSERT_EFI_ERROR(Status);
        }
    }

    // Restoring the Old Timer Value before exiting from SETUP.
    if((gTimer != NULL) && (gTimerTickDefaultDuration != 0)) {
        Status = gTimer->SetTimerPeriod(gTimer,gTimerTickDefaultDuration);
        ASSERT_EFI_ERROR(Status);
    }
#endif

    TSEMouseDestroyHook();
}
#endif

#if OVERRIDE_TSEGetactualScreentop
//EIP 79962 : START
// Mouse pointer corruption in GTSE  
/**
    Function to return the actual Screen Top value

    @param VOID

    @retval INT32

**/

INT32 
TSEGetactualScreentop(VOID)
{
    return Screen_Top;
}
//EIP 79962 : END
#endif

#if AMITSE_SOFTKBD_SUPPORT
#if OVERRIDE_TSENumericSoftKbdInit

/**
    Function to display the numeric softkbd

    @param VOID

    @retval INT32

**/

VOID 
TSENumericSoftKbdInit(VOID)
{
    EFI_STATUS Status;

    Status = gBS->LocateProtocol(&gSoftKbdGuid, NULL, (VOID**) &TSESoftKbd_n);
    if (EFI_ERROR(Status)) { 
        TSESoftKbd_n = NULL;
        return;
    } 
    TSESoftKbd_n->SetKbdLayout(TSESoftKbd_n, SOFT_KEY_LAYOUT_NUMERIC);
   
    // From AMITSE label 1255, TSEStringReadLoopEntryHook() is called from Popup controls in AMITSE 
    if (TSE_BUILD < 0x1255) {
        TSEStringReadLoopEntryHook();        
    }
    gNumericSoftkbdActivated  = TRUE;
}
#endif

#if OVERRIDE_TSENumericSoftKbdExit

/**
    Function to destroy the numeric softkbd display

    @param VOID

    @retval INT32

**/

VOID 
TSENumericSoftKbdExit (VOID)
{
    if (NULL == TSESoftKbd_n)
        return;

    if (gNumericSoftkbdActivated) {
        TSEStringReadLoopExitHook();
        TSESoftKbd_n->SetKbdLayout(TSESoftKbd_n, SOFT_KEY_LAYOUT_FULL_US_KEYBOARD);
        gNumericSoftkbdActivated = FALSE;
    }
}
#endif

/**
    Returns the Mouse Pointer width, height.

    @param MousePointerWidth 
    @param MousePointerHeight 

         
    @retval VOID

**/

VOID 
TSEGetMousePointerDimensions (
  OUT  INT32     *MousePointerWidth,
  OUT  INT32     *MousePointerHeight
)
{
    *MousePointerWidth = CURSOR_WIDTH;
    *MousePointerHeight = CURSOR_HEIGHT;
    return;
}

#endif //AMITSE_SOFTKBD_SUPPORT
#endif //MINISETUP_MOUSE_SUPPORT

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
