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

/** @file PauseKey.c
   Implements the functions of PauseKey driver.
    
    This driver registers a notify function for pause key stroke that notify
    function stops the currently running process by checking the WaitForKey 
    event's signaled state continuously and displays a message on screen.
    On other key press, paused process will be resumed.

**/

#include <AmiDxeLib.h>
#include <Protocol/SimpleTextInEx.h>
#include <Library/HiiLib.h>
#include <Protocol/AMIPostMgr.h>
#include "Token.h"
#include <Protocol/SimpleTextInEx.h>

// GUID for Pause key strings package
#define PAUSEKEY_STRING_PACKAGE_GUID\
       { 0x4edd1379, 0x7bbe, 0x4d32, { 0xb4, 0x5d, 0x85, 0xd3, 0xe5, 0xc5, 0x91, 0x65 } }

// module global variable for the Ami Post Manager Protocol
AMI_POST_MANAGER_PROTOCOL   *gPostMgr = NULL;

// global variable for ST->ConInHandle's  SimpleTextInEx protocol pointer
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *gSimpleEx = NULL;   

// Global pointer for retrieved unicode string from the HII database for Pause Message 
CHAR16          *gString;

// Global pointer for retrieved unicode string from the HII database to erase Pause Message 
CHAR16          *gBlankString;

// Sets the pause key scan code, unicode and key state values
EFI_KEY_DATA    gPauseKeyData = { { SCAN_PAUSE, CHAR_NULL} , { 0, 0} };

// Handle for string resources that are part of the module
EFI_HII_HANDLE gHiiHandle = 0;

// Unique handle to be assigned for registered pause key notification.
VOID            *gPauseKeyNotifyHandle;

// Semaphore to avoid re-entrancy even if UnregisterKeyNotify() fails
BOOLEAN         gPauseKeyAtomicLock = FALSE;
BOOLEAN         gPausePressedAgain = FALSE;

EFI_STATUS
PauseKeyNotificationFunction(
    IN EFI_KEY_DATA *RegisteredKeyData
    );

VOID PromptToUnlockTheHalt(VOID);

VOID PauseKey_ConInInstalledCallback(IN EFI_EVENT Event, IN VOID *Context);

//----------------------------------------------------------------------------

/**
   This is the driver entrypoint.  
   
    This driver installs the string packages that are connected to 
    the driver image.  After installing the package, it gets the two strings
    uased in the driver.  It then installs a callback on the SimpleTextInEx Protocol

  @param ImageHandle  Handle of the driver image
  @param SystemTable  Pointer to the system table
  
  @return EFI_STATUS
  @retval EFI_SUCCESS   
**/
EFI_STATUS
PauseKeyDriverEntryPoint(    
        IN EFI_HANDLE       ImageHandle,
        IN EFI_SYSTEM_TABLE *SystemTable
        )
{
#if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))
    EFI_STATUS                          Status;
    EFI_HII_HANDLE                      gPauseKeyStringPackHandle = NULL;
    EFI_GUID gPausekeyStringPackageGuid = PAUSEKEY_STRING_PACKAGE_GUID;
#endif
    EFI_EVENT                           Event;
    VOID                                *pRegistration;

    InitAmiLib(ImageHandle, SystemTable);

#if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))
    

    // Load the Language strings for use when creating the strings into the Hii database
    Status = LoadStrings(ImageHandle, &gHiiHandle);
    ASSERT(Status == EFI_SUCCESS);
    if (EFI_ERROR(Status))
        return Status;


    // Gets the "STR_BLANK_LINE_STRING" string from HII database 
    gBlankString = HiiGetString(gHiiHandle, STRING_TOKEN(STR_BLANK_LINE_STRING), NULL);

    // Gets the "STR_PAUSEKEY_PRESSED_MSG" string from HII database 
    gString = HiiGetString(gHiiHandle, STRING_TOKEN(STR_PAUSEKEY_PRESSED_MSG), NULL);

#endif  // end of #if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))

    
    // register a callback for ConIn to be installed
    RegisterProtocolCallback(&gEfiSimpleTextInputExProtocolGuid, PauseKey_ConInInstalledCallback, NULL, &Event,&pRegistration);
    return EFI_SUCCESS;   
}

/**
    SimpleTextInputEx Protocol Callback which installs the event and notification used to 
    handle the Pause Key processing.
    
    First it checks to see in the SystemTable->ConIn Pointer has been updated,
    This check is to see if the Consplitter has been installed.  If it has,
    and the ConInHandle is valid, it is used to handle the SimpleTextInEx Protocol.  
    If the ConInHandle is not valid, then try the ConOutHandle.  The core uses the 
    same handle for both.  Once the Handle is found, the SimpleTextInEx Event is 
    closed, because no more processing is needed.
    Once the handle is defined, the system calls RegisterKeyNotify to get a 
    callback on a Pause key press. An event is also created for un-pausing the 
    system.

    @param Event The pointer created to identify this callback
    @param Context data passed in from the create event call for use in the function
    
    @return None
 */
VOID PauseKey_ConInInstalledCallback(IN EFI_EVENT Event, IN VOID *Context)
{
    EFI_STATUS      Status;
    EFI_HANDLE      ConsoleInHandle;
    
    // If the SystemTable ConIn Pointer is NULL exit out and wait until ConIn is defined
    if (pST->ConIn == NULL)
        return;
    else 
    {
        // neither handle has been defined yet. wait until the next call, we can't do anything without the handle
        if ((pST->ConsoleInHandle == NULL) && (pST->ConsoleOutHandle == NULL))
            return; 
        // Both use the same handle, but the call may have happened before the pST->ConInHandle is set
        else if (pST->ConsoleInHandle == NULL)
            ConsoleInHandle = pST->ConsoleOutHandle;
        else 
            ConsoleInHandle = pST->ConsoleInHandle;
    }
    
    // Get the SimpleTextInEx Protocol from the ConInHandle in the System Table
    Status = pBS->HandleProtocol (
            ConsoleInHandle,
            &gEfiSimpleTextInputExProtocolGuid,
            (VOID**)&gSimpleEx
            );
    if (EFI_ERROR(Status))
    {
        // if the SimpleTextInputEx is not on the handle, exit and wait for the next call
        DEBUG((-1, "SimpleTxtInEx not found.\n\r"));
        ASSERT_EFI_ERROR (Status); 
        return;
    }
        
    //register RegisterKeyNotify
    Status = gSimpleEx->RegisterKeyNotify(
            gSimpleEx,
            &gPauseKeyData,
            PauseKeyNotificationFunction,
            &gPauseKeyNotifyHandle
            );

    if (EFI_ERROR(Status))
    {
        // The RegisterKeyNotify did not happen as expected, exit here and wait for next call
        DEBUG((-1, "RegisterKeyNotify Failed.\n\r"));
        ASSERT_EFI_ERROR (Status); 
        return;
    }

    // we no longer need this event all registrations have completed properly
    pBS->CloseEvent(Event);

}

/**
  This function checks for any keypress and if it isn't the Pause key the 
  system is allowed to continue running
  
  This function handles the display of the message strings and the detection
  of a keypress to end the Pause.  First it sets the TPL to TPL_APPLICATION
  so no other timer events or callbacks will be blocked by this event.  Next 
  it displays a message on the screen and waits for a keypress using the 
  WaitForKeyEx function.  If any other key other than pause is pressed, then
  the system clears the key buffer, clears the message and re-registerKeyNotify 
  for the Pause key.  Finally, the semaphore is cleared and the TPL is returned
  to its inital value.
  
 
  @return VOID
  @retval none
*/

VOID PromptToUnlockTheHalt(VOID)
{
#if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))
    EFI_STATUS                  Status;
    UINTN                       PostStatus = TSE_POST_STATUS_IN_TSE;       
    UINTN                       CurrRow;
    UINTN                       CurrColumn;
#endif // end of #if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))
    EFI_KEY_DATA                ReadKeyData;  
    EFI_TPL                     OriginalTpl;    
    UINTN                       Index = 0;


    // For stability purpose, we are going to handle all the TPLs while polling to break the pause
    OriginalTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    pBS->RestoreTPL(TPL_APPLICATION);

#if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))
    
    if (gPostMgr == NULL)
    {
        // Locates the AMI_POST_MANAGER_PROTOCOL interface
        Status = pBS->LocateProtocol(
                            &gAmiPostManagerProtocolGuid, 
                            NULL,
                            (void**) &gPostMgr
                            );
        if (EFI_ERROR(Status))
        {
            ASSERT_EFI_ERROR(Status);
            gPausePressedAgain = FALSE;
            pBS->RaiseTPL(TPL_HIGH_LEVEL);
            gPauseKeyAtomicLock = FALSE;
            pBS->RestoreTPL(OriginalTpl);
            return;
        }

    }

    // Gets the current post status 
    PostStatus = gPostMgr->GetPostStatus();

    // Handle messaging actions if it is in Test-mode
    if ((PostStatus != TSE_POST_STATUS_IN_TSE) && (PostStatus != TSE_POST_STATUS_PROCEED_TO_BOOT))
    {
      
        // Gets the cursor's current position
        gPostMgr->GetCurPos(&CurrColumn, &CurrRow);
        gPostMgr->SetCurPos(0, CurrRow + 1);
        if (EFI_ERROR(Status))
        {
            gPostMgr->SetCurPos(0, CurrRow);
        }
            
        // Displays the message on next line 
        gPostMgr->DisplayPostMessageEx(gString, PM_EX_DONT_ADVANCE_TO_NEXT_LINE);
        gPostMgr->SetCurPos(CurrColumn, CurrRow);

    }   
    else if (PostStatus == TSE_POST_STATUS_PROCEED_TO_BOOT)
    {
      
        // Gets the cursor's current position
        CurrRow = pST->ConOut->Mode->CursorRow;
        CurrColumn = pST->ConOut->Mode->CursorColumn;

        Status = pST->ConOut->SetCursorPosition(pST->ConOut, 0, CurrRow + 1);
            
        if (Status == EFI_UNSUPPORTED)
        {
            // at the bottom of the screen
            Status = pST->ConOut->SetCursorPosition(pST->ConOut, 0, CurrRow);
        }
        // Displays the message on next line 
        pST->ConOut->OutputString(pST->ConOut, gString);
        pST->ConOut->SetCursorPosition(pST->ConOut, CurrColumn, CurrRow);
    }
#endif  // end of #if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))
  

    while (TRUE) {
        // Wait for the keypress
        pBS->WaitForEvent( 1, &gSimpleEx->WaitForKeyEx, &Index );
        
		// check if the RegisterKeyNotify for the Pause key has been called or not
        if (gPausePressedAgain == FALSE)
        {
			// if not, we should break the loop and exit
            break;
        }
        else 
        {
			// if the pause key was pressed, clear the buffer, clear the pause key notification and continue to wait
            while( gSimpleEx->ReadKeyStrokeEx(gSimpleEx, &ReadKeyData ) == EFI_SUCCESS );
            gPausePressedAgain = FALSE;
        }
    }    

#if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))  
    
    if ((PostStatus != TSE_POST_STATUS_IN_TSE) && (PostStatus != TSE_POST_STATUS_PROCEED_TO_BOOT))
    {        
        // Clears the printed message, by printing a blank line string
        // Go to beginning of line
        gPostMgr->SetCurPos(0, CurrRow + 1);
        gPostMgr->DisplayPostMessageEx(gBlankString, PM_EX_DONT_ADVANCE_TO_NEXT_LINE);
        
        // Sets the cursor position to previous position 
        gPostMgr->SetCurPos(CurrColumn, CurrRow);
    }
    else if (PostStatus == TSE_POST_STATUS_PROCEED_TO_BOOT)
    {
        Status = pST->ConOut->SetCursorPosition(pST->ConOut, 0, CurrRow + 1);
        if (Status == EFI_UNSUPPORTED)
        {
            // at the bottom of the screen
            Status = pST->ConOut->SetCursorPosition(pST->ConOut, 0, CurrRow);
        }
        // Displays the message on next line 
        pST->ConOut->OutputString(pST->ConOut, gBlankString);
        pST->ConOut->SetCursorPosition(pST->ConOut, CurrColumn, CurrRow);
    }
#endif // end of #if (defined(SUPPRESS_PAUSE_MESSAGE) && (SUPPRESS_PAUSE_MESSAGE == 0))

    // restore the TPL level and clear the semaphore
    pBS->RaiseTPL(TPL_HIGH_LEVEL);
    gPauseKeyAtomicLock = FALSE;
    pBS->RestoreTPL(OriginalTpl);
    gPausePressedAgain = FALSE;  // clear the pause key notification variable just to be sure
}


/**
   This is the registered notify function for pause key stroke.
    
    Once the pause key is pressed this function will be invoked. It checks 
    for the possibility of re-entry, if not, then set the semaphore and 
    unregisters the PauseKeyNotificationFunction for pause key which  
    was registered in ConInStartedProtocolCallback(). Finally it calls 
    PromptToUnlockTheHalt that will check for keypress to end the pause.
    
  @param RegisteredKeyData - Registered key data

  @return EFI_STATUS
  @retval EFI_SUCCESS     - If this function is executed successfully.
    
**/

EFI_STATUS 
PauseKeyNotificationFunction (
    IN EFI_KEY_DATA *RegisteredKeyData 
)
{
    EFI_STATUS                          Status = EFI_SUCCESS;
    EFI_TPL                             OriginalTpl;
    EFI_KEY_DATA                ReadKeyData;

    // Semaphore PauseKeyAtomicLock will act as shield if 
    // SimpleEx->UnregisterKeyNotify() in the upcoming code fails
    if(gPauseKeyAtomicLock) {
	// on a second entry, set this variable to notify the rest of the code that the key press was a pause and not a key to exit the pause
        gPausePressedAgain = TRUE;  
        return EFI_SUCCESS;
    }

    // block the timer interrupt while setting semaphore
    OriginalTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    gPauseKeyAtomicLock = TRUE;  
    pBS->RestoreTPL(OriginalTpl);    
    
    // we need to process the pause keypress.  There is a case in shell where this 
    // doesn't happen and then the pause key driver reads the key
    gSimpleEx->ReadKeyStrokeEx(gSimpleEx, &ReadKeyData );

    // Schedule an event to simulate halt 
    PromptToUnlockTheHalt();
    return Status;
}




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
