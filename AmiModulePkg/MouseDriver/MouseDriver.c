//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file MouseDriver.c
    File has the functions to draw the mouse Pointer and Protocol 
    Functions
**/

//---------------------------------------------------------------------------

#include <Protocol/SimplePointer.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/AbsPointerProtocol.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/DevicePath.h>
#include <Protocol/EdidDiscovered.h>
#include "MouseDriver.h"
#include "MouseDriverElinks.h"

//---------------------------------------------------------------------------

#if SOFTKBD_ICON_SUPPORT || BITLOCKER_SOFTKBD_SUPPORT
#include <Protocol/SoftKbdProtocol.h>
#endif

#define FRACTIONAL_PART_CONVERSION    100

EFI_GUID        gEfiDXEMouseDriverProtocolGuid  = 
                            EFI_MOUSE_DRIVER_PROTOCOL_GUID;
EFI_GUID        gActivateSoftKeyboardProtocolGuid = 
                            EFI_SOFTKEY_ACTIVATE_PROTOCOL_GUID;
EFI_GUID        gDeActivateSoftKeyboardProtocolGuid = 
                            EFI_SOFTKEY_DEACTIVATE_PROTOCOL_GUID;
EFI_GUID        gProcessEscKeyProtocolGuid = 
                            EFI_PROCESS_ESC_KEY_PROTOCOL_GUID;
EFI_GUID        gProcessPostKeyProtocolGuid = 
                            EFI_PROCESS_POST_KEY_PROTOCOL_GUID;
#if SOFTKBD_ICON_SUPPORT || BITLOCKER_SOFTKBD_SUPPORT
EFI_GUID        gEfiSoftKbdProtocolGuid = EFI_SOFT_KBD_PROTOCOL_GUID;
#endif

VOID SignalProtocolEvent(IN EFI_GUID *ProtocolGuid);

MOUSE_TRANSLATION_TABLE gDefaultSpeedTranslationTable[] = 
    { MOUSE_ACCELERATION_ENTRIES {0, 0, 0} };
UINT32 gDefaultSpeedTranslationTableCount = 
    (sizeof(gDefaultSpeedTranslationTable) / sizeof(MOUSE_TRANSLATION_TABLE)) - 1;

DLIST        SimplePointerList;
DLIST        AbsPointerList;

// Resolution for calculation of SpeedTranslation
INT32       CurrentXResolution = 1;
INT32       CurrentYResolution = 1;
INT32       CurrentZResolution = 1;

// To Restrict the Mouse Movement to a Defined Area
INT32       MouseStartRow = 0;
INT32       MouseStartColoum = 0;
INT32       MouseEndRow = 0;
INT32       MouseEndColoum = 0;

// Mouse Pointer Position values
INT32       CurrentXPosition = 0;
INT32       CurrentYPosition = 0;
INT32       CurrentZPosition = 0;
INT32       PreviousXPosition;
INT32       PreviousYPosition;

BOOLEAN     InMousePointDraw = FALSE;

// Buffer to store MousePointer and Background
EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer = NULL;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MousePointerBuffer = NULL;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MouseBltBuffer = NULL;
EFI_EVENT             SimplePointEvent;
EFI_EVENT             AbsPointEvent;
//
// To ensure Graphics Output protocol is located
//
BOOLEAN             gGOPLocated = FALSE;
static EFI_EVENT    gGopProtocolEvent;
VOID               *gGopProtocolEventReg;
void               *gSimplePointerNotifyReg;
void               *gAbsPointerNotifyReg;

// To check whether there is pointer device present
BOOLEAN             gPointerDevicePresent = TRUE;

static EFI_DRIVER_BINDING_PROTOCOL Binding = {
                            MouseDriverSupported,
                          
                            MouseDriverStart,
                            MouseDriverStop,
                            1,
                            NULL,
                            NULL };

typedef struct _MOUSE_DRIVER_PRIVATE {
    //
    // Mouse Protocol functions and variables
    //
    UINT8                                       Revision;
    EFI_FUNCTION_POLL                           MousePoll;
    EFI_FUNCTION_INITIALIZE                     InitializePointingDevice;
    EFI_FUNCTION_START_UPDATING_MOUSE_CURSOR    StartUpdatingMouseCursor;
    EFI_FUNCTION_STOP_UPDATING_MOUSE_CURSOR     StopUpdatingMouseCursor;
    EFI_FUNCTION_REFRESH_MOUSE_CURSOR           RefreshMouseCursor;
    EFI_FUNCTION_GETPOSITION                    GetCoordinates;
    EFI_FUNCTION_SETPOSITION                    SetCoordinates;
    EFI_FUNCTION_SETSPEED                       SetMouseSpeed;
    EFI_FUNCTION_GETBUTTONSTATUS                GetButtonStatus;
    EFI_FUNCTION_SETVIEWPOINT                   SetMouseCoordinates;
    EFI_FUNCTION_SOFTKEY                        SetSoftKeyData;
    EFI_FUNCTION_STOP                           StopPointingDevice;
    EFI_FUNCTION_SET_SPEED_TRANSLATION_TABLE    SetSpeedTranslationTable;
    EFI_FUNCTION_GET_VELOCITY                   GetMouseVelocity;
    EFI_FUNCTION_UPDATE_POINTER                 UpdateMousePointer;
    EFI_FUNCTION_UPDATE_MOUSE_POINTER_PROPERTY  UpdateMousePointerProperty;
    EFI_FUNCTION_SET_MOUSE_GOP                  SetMouseGOP;
    EFI_FUNCTION_GET_MOUSE_CURSOR               GetMouseCursorImage;

    //
    // Simple and Absolute protocol related variables
    //

    EFI_GRAPHICS_OUTPUT_PROTOCOL    *Gop;
#if SOFTKBD_ICON_SUPPORT || BITLOCKER_SOFTKBD_SUPPORT
    SOFT_KBD_PROTOCOL               *SoftKbd;
#endif
    EFI_SIMPLE_POINTER_MODE         *SimpleMode;
    EFI_ABSOLUTE_POINTER_MODE       *AbsMode;
    EFI_SIMPLE_POINTER_STATE        SimplePtrState;
    EFI_ABSOLUTE_POINTER_STATE      AbsState;

    //
    // Mouse Events
    //
    EFI_EVENT                       GetMouseActionEvent;
    EFI_EVENT                       LeftClickEvent;
#if DRAW_MOUSE_ON_HOTPLUG
    EFI_EVENT                       ChecktoDrawMouseEvent;
#endif

    // 
    // Mouse Speed related variables
    //
    UINT32                          MovementSpeed;
    SOFT_KEY_DATA                   SoftKeyboardData;
    MOUSE_ACCELERATION_DATA         AccelerationData;

    //
    // Mouse Pointer related variables
    //
    MOUSE_POINTER_PROPERTY          MousePointerProperty;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL   TransparentPixel;
    UINTN                           CursorWidth;
    UINTN                           CursorHeight;
    UINTN                           PreviousCursorHeight;
    UINTN                           PreviousCursorWidth;
    BOOLEAN                         MouseCursorUpdate;
    BOOLEAN                         MousePointerPresent;
    BOOLEAN                         MousePointerUpdate;
    BOOLEAN                         PositionMousePointer;

    //
    // Mouse Click and Position variables
    //
    UINT8        RightPressCount;
    UINT8        RightReleaseCount;
    UINT8        LeftPressCount;
    UINT8        LeftReleaseCount;
    UINT8        MidPressCount;
    UINT8        MidReleaseCount;
    UINT8        AbsMouseClickCount;
    UINT8        AbsMouseRelCount;
    BOOLEAN      DoubleClick;

    INT32         RightXPosition;
    INT32         RightYPosition;
    INT32         RightXRelPosition;
    INT32         RightYRelPosition;
    INT32         LeftXPosition;
    INT32         LeftYPosition;
    INT32         LeftXRelPosition;
    INT32         LeftYRelPosition;
    INT32         MidXPosition;
    INT32         MidYPosition;
    INT32         MidXRelPosition;
    INT32         MidYRelPosition;
    INT32         AbsXPosition;
    INT32         AbsYPosition;
    INT32         AbsXRelPosition;
    INT32         AbsYRelPosition;
    
    BOOLEAN       GOPUpdated;
    BOOLEAN       ReportedAbsDClick;
#if !MOUSE_SUPPORT_FOR_NT32 
    BOOLEAN       ValidPressOrRelease;             //EIP 168048 Used to Check Sequence of Press & release  
#endif
    
#if HIDE_POINTER_FOR_TOUCH
    BOOLEAN       PointerHideForAbs;
#endif

}MOUSE_DRIVER_PRIVATE;

MOUSE_DRIVER_PRIVATE    *gMDP;

VOID 
DrawMousePointer (
    IN MOUSE_DRIVER_PRIVATE *MDP );

VOID 
ClearMousePointer (
    IN MOUSE_DRIVER_PRIVATE *MDP );

VOID 
GetMouseClick (
    IN MOUSE_DRIVER_PRIVATE *MDP );

VOID 
GetMouseMovement (
    IN MOUSE_DRIVER_PRIVATE *MDP );

EFI_STATUS 
GetAbsDeviceData (    
    IN MOUSE_DRIVER_PRIVATE *MDP );

EFI_STATUS 
GetSimplePointerDeviceData (    
    IN MOUSE_DRIVER_PRIVATE *MDP );

EFI_STATUS 
ResetPointingDevices (    
    IN MOUSE_DRIVER_PRIVATE *MDP );

#if SOFTKBD_ICON_SUPPORT || BITLOCKER_SOFTKBD_SUPPORT
UINT8 
IconClick (
    IN MOUSE_DRIVER_PRIVATE *MDP,
    IN BOOLEAN IconPress );
#endif

VOID 
ConnectSimpleAbsHandles (
    IN  EFI_EVENT    Event,
    IN  VOID         *ProtocolContext );

/**
    Mouse Driver entry point.

    @param ImageHandle Image handle for this driver
    @param SystemTable Pointer to the EFI system table.

    @retval EFI_SUCCESS The function completed successfully.

    @note  
       Here is the control flow of this function:
              1. Initialize Ami Lib
              2. Install Driver Binding Protocol
              3. Return EFI_STATUS.

**/
 
EFI_STATUS
EFIAPI
MouseDriverEntryPoint (
    IN  EFI_HANDLE           ImageHandle,
    IN  EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS        Status;

    //
    // Initialize the EFI driver library
    //
    InitAmiLib(ImageHandle, SystemTable);

    Binding.DriverBindingHandle = ImageHandle;
    Binding.ImageHandle = ImageHandle;

    //
    // Initialize the global device lists
    //
    DListInit(&SimplePointerList);
    DListInit(&AbsPointerList);

    Status = pBS->InstallMultipleProtocolInterfaces(
                                    &Binding.DriverBindingHandle,
                                    &gEfiDriverBindingProtocolGuid, &Binding,
                                    NULL);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    //
    // Create the notification and register callback function on 
    // the Simple Pointer Protocols
    //
    Status = pBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL, 
                    TPL_CALLBACK,
                    ConnectSimpleAbsHandles, 
                    &gSimplePointerNotifyReg, 
                    &SimplePointEvent);

    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    Status = pBS->RegisterProtocolNotify (
                &gEfiSimplePointerProtocolGuid, 
                SimplePointEvent, 
                &gSimplePointerNotifyReg);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    //
    // Create the notification and register callback function on the 
    // Absolute Pointer Protocols
    //
    Status = pBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL, 
                    TPL_CALLBACK,
                    ConnectSimpleAbsHandles, 
                    &gAbsPointerNotifyReg, 
                    &AbsPointEvent);

    if (EFI_ERROR(Status)) { 
        return Status;
    }

    Status = pBS->RegisterProtocolNotify (
                &gEfiAbsolutePointerProtocolGuid, 
                AbsPointEvent, 
                &gAbsPointerNotifyReg);

    return Status;
}

/**

    Checks whether the controller supports EFI_SIMPLE_POINTER_PROTOCOL or EFI_EDID_DISCOVERED_PROTOCOL

    @param  This Pointer to this instance 
            of driver binding protocol
    @param  Controller Controller handle
    @param  RemainingDevicePath pointer to device path

          
    @retval EFI_SUCCESS The function completed successfully.

    @note  
       Here is the control flow:
          1. Checks controller supports EFI_EDID_DISCOVERED_PROTOCOL.
             if supports returns EFI_STATUS.
          2. else checks controller supports EFI_SIMPLE_POINTER_PROTOCOL
             if supports returns EFI_STATUS.
          3. else checks controller supports EFI_ABSOLUTE_POINTER_PROTOCOL
          4. Return EFI_STATUS.
                  

**/

EFI_STATUS 
MouseDriverSupported (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
)
{ 
    EFI_SIMPLE_POINTER_PROTOCOL     *SimplePointer;
    EFI_EDID_DISCOVERED_PROTOCOL    *EdidDiscovered;
    EFI_STATUS      Status;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath;

    //
    // Open EFI_EDID_DISCOVERED_PROTOCOL_GUID BY_DRIVER to get notification 
    // for EFI_GRAPHICS_OUTPUT_PROTOCOL Reinstallation/Uninstallation.
    //
    Status = pBS->OpenProtocol( Controller,
                    &gEfiEdidDiscoveredProtocolGuid,
                    (VOID**)&EdidDiscovered,
                    This->DriverBindingHandle,
                    Controller,   
                    EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (!EFI_ERROR (Status)) {
        Status = pBS->CloseProtocol (
                        Controller,
                        &gEfiEdidDiscoveredProtocolGuid,
                        This->DriverBindingHandle,
                        Controller);
        return Status;
    }

    //
    // Core ver 4641 and above provides SimplePointer splitter support, so 
    // don't handle SimplePointer protocol open BY_DRIVER for valid DevicePath.
    //
    Status = pBS->OpenProtocol( Controller,
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&DevicePath,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL );
#if defined CORE_COMBINED_VERSION && CORE_COMBINED_VERSION > 0x5000b
    //
    // Return Unsupported for Valid DevicePath
    //
    if (!EFI_ERROR(Status)) {
         return EFI_UNSUPPORTED;
    }
#else
    //
    // Return error for Valid DevicePath
    //
    if (!EFI_ERROR(Status)) {
        //  
        // Open SimplePointer protocol BY_DRIVER if it is Absolute device.
        //
        Status = AbsMouseDriverSupported(This,Controller,RemainingDevicePath);
        return Status;
    }
#endif
    Status = pBS->OpenProtocol( Controller,
                    &gEfiSimplePointerProtocolGuid,
                    (VOID **)&SimplePointer,
                    This->DriverBindingHandle,
                    Controller,   
                    EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (EFI_ERROR (Status)) {
        Status = AbsMouseDriverSupported(This,Controller,RemainingDevicePath);
        return Status;        
    }

    Status = pBS->CloseProtocol (
                    Controller,
                    &gEfiSimplePointerProtocolGuid,
                    This->DriverBindingHandle,
                    Controller);

    return  Status;
}

/**

     Installs Mouse Driver protocol EFI_MOUSE_DRIVER_PROTOCOL_GUID and adds 
     node in SIMPLE_POINTER list.

    @param This Pointer to this instance 
        of driver binding protocol
    @param Controller Controller handle
    @param RemainingDevicePath pointer to device path

          
    @retval EFI_SUCCESS The function completed successfully.

**/

EFI_STATUS 
MouseDriverStart (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
)
{
    EFI_SIMPLE_POINTER_PROTOCOL     *SimplePointerProtocol;
    EFI_EDID_DISCOVERED_PROTOCOL    *EdidDiscovered;
    EFI_STATUS      Status;
    SIMPLE_POINTER  *SimplePointerNode;

    //
    // Open EFI_EDID_DISCOVERED_PROTOCOL_GUID BY_DRIVER to get notification 
    // for EFI_GRAPHICS_OUTPUT_PROTOCOL Reinstallation/Uninstallation.
    //
    Status = pBS->OpenProtocol( Controller,
                    &gEfiEdidDiscoveredProtocolGuid,
                    (VOID**)&EdidDiscovered,
                    This->DriverBindingHandle,
                    Controller,   
                    EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (!EFI_ERROR (Status)) {
        LocateGopProtocol(NULL, (VOID*)gMDP); 
        return Status;
    }
    
    Status = pBS->OpenProtocol( Controller,
                    &gEfiSimplePointerProtocolGuid,
                    (VOID **)&SimplePointerProtocol,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (EFI_ERROR (Status)) {
        Status = AbsMouseDriverStart(This, Controller, RemainingDevicePath);
        return Status;   
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(SIMPLE_POINTER), 
                               (VOID**)&SimplePointerNode);

    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    SimplePointerNode->SimplePointer = SimplePointerProtocol;
    SimplePointerNode->Handle = Controller;

    DListAdd(&SimplePointerList, &(SimplePointerNode->Link)); 

    Status = InstallMouseProtocol(Controller);
    
    if (EFI_ERROR (Status)) {
            return Status;   
    }

    DrawMousePointer(gMDP);
    
    return  Status;
 
}

/**
    Removes specific node from SIMPLE_POINTER list and clears Mouse Pointer 
    when no device is present. 

    @param This - pointer to driver binding protocol
    @param ControllerHandle - controller handle to install driver on
    @param NumberOfChildren - number of childs on this handle
    @param ChildHandleBuffer - pointer to child handles array

         
    @retval EFI_SUCCESS driver stopped successfully

**/

EFI_STATUS 
MouseDriverStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  UINTN                            NumberOfChildren,
    IN  EFI_HANDLE                       *ChildHandleBuffer
)
{
    EFI_SIMPLE_POINTER_PROTOCOL     *SimplePointer;
    EFI_EDID_DISCOVERED_PROTOCOL    *EdidDiscovered;
    EFI_STATUS      Status;
    DLINK           *ListPtr;
    SIMPLE_POINTER  *SimplePointerNode;


    //
    // Check whether controller handle supports
    // EFI_EDID_DISCOVERED_PROTOCOL_GUID
    //
    Status = pBS->OpenProtocol( Controller,
                    &gEfiEdidDiscoveredProtocolGuid,
                    (VOID**)&EdidDiscovered,
                    This->DriverBindingHandle,
                    Controller,   
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL );

    if (!EFI_ERROR(Status)) {
        //
        // Update var "gGOPLocated" for GOP uninstallation
        //
        gGOPLocated = FALSE;
        Status = pBS->CloseProtocol (
                        Controller,
                        &gEfiEdidDiscoveredProtocolGuid,
                        This->DriverBindingHandle,
                        Controller);
        return Status;
    }

    Status = pBS->OpenProtocol( Controller,
                    &gEfiSimplePointerProtocolGuid,
                    (VOID **)&SimplePointer,
                    This->DriverBindingHandle,
                    Controller,   
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL );


    if (EFI_ERROR (Status)) {
        Status = AbsMouseDriverStop(This, Controller,
                                    NumberOfChildren, ChildHandleBuffer);
        goto Exit;
    } else {
    
        //
        // Remove SimplePointer 
        //
        ListPtr = SimplePointerList.pHead;
        while ( ListPtr != NULL) {
            SimplePointerNode = OUTTER(ListPtr, Link, SIMPLE_POINTER);
            if ( SimplePointerNode->Handle == Controller) {
                DListDelete(&SimplePointerList, ListPtr);
                Status = pBS->FreePool(SimplePointerNode);
                break;
            }           
            ListPtr = ListPtr->pNext;
        }
    
        Status = pBS->CloseProtocol (
                        Controller,
                        &gEfiSimplePointerProtocolGuid,
                        This->DriverBindingHandle,
                        Controller);
    }

Exit:
    //
    // Remove the Mouse Pointer From Screen if all the SimplePointer 
    // and Absolute Protocol uninstalled.
    //    
    if (SimplePointerList.Size == 0 && AbsPointerList.Size == 0) {
        ClearMousePointer(gMDP);
    }

    return Status;
}

/**
    Checks whether the controller supports EFI_ABSOLUTE_POINTER_PROTOCOL

    @param  This - Pointer to this instance 
            of driver binding protocol
    @param  Controller - Controller handle
    @param  RemainingDevicePath - pointer to device path

    @retval EFI_SUCCESS The function completed successfully.

**/

EFI_STATUS 
AbsMouseDriverSupported (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
)
{ 
    EFI_ABSOLUTE_POINTER_PROTOCOL *AbsolutePointer;
    EFI_STATUS      Status;

    Status = pBS->OpenProtocol( Controller,
                    &gEfiAbsolutePointerProtocolGuid,
                    (VOID **)&AbsolutePointer,
                    This->DriverBindingHandle,
                    Controller,   
                    EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
    }

    Status = pBS->CloseProtocol (
                    Controller,
                    &gEfiAbsolutePointerProtocolGuid,
                    This->DriverBindingHandle,
                    Controller);

    return  Status;
}

/**
    Installs Mouse Driver protocol EFI_MOUSE_DRIVER_PROTOCOL_GUID and adds 
    node in ABSOLUTE_POINTER list.

    @param  This - Pointer to this instance 
            of driver binding protocol
    @param  Controller - Controller handle
    @param  RemainingDevicePath - pointer to device path

    @retval EFI_SUCCESS The function completed successfully.

**/

EFI_STATUS 
AbsMouseDriverStart (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
)
{
    EFI_ABSOLUTE_POINTER_PROTOCOL *AbsolutePointer;
    EFI_STATUS      Status;
    ABSOLUTE_POINTER  *AbsPointerNode;


    Status = pBS->OpenProtocol( Controller,
                    &gEfiAbsolutePointerProtocolGuid,
                    (VOID **)&AbsolutePointer,
                    This->DriverBindingHandle,
                    Controller,   
                    EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(ABSOLUTE_POINTER), (VOID**) &AbsPointerNode);
    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    AbsPointerNode->AbsPointer = AbsolutePointer;
    AbsPointerNode->Handle = Controller;    
    DListAdd(&AbsPointerList, &(AbsPointerNode->Link));

    Status = InstallMouseProtocol(Controller);
#if MOUSE_POINTER_FOR_ABS_DEVICE
    DrawMousePointer(gMDP);
#endif

    return  Status;

}

/**
    Removes specific node from ABSOLUTE_POINTER list and clears Mouse Pointer 
    when no device is present. 

    @param  This - pointer to driver binding protocol
    @param  ControllerHandle - controller handle to install driver on
    @param  NumberOfChildren - number of childs on this handle
    @param  ChildHandleBuffer - pointer to child handles array
 
         
    @retval EFI_SUCCESS driver stopped successfully

**/

EFI_STATUS 
AbsMouseDriverStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  UINTN                            NumberOfChildren,
    IN  EFI_HANDLE                       *ChildHandleBuffer
)
{
    EFI_ABSOLUTE_POINTER_PROTOCOL *AbsolutePointer;
    EFI_STATUS      Status;
    DLINK           *ListPtr;
    ABSOLUTE_POINTER  *AbsPointerNode;

    Status = pBS->OpenProtocol( Controller,
                    &gEfiAbsolutePointerProtocolGuid,
                    (VOID **)&AbsolutePointer,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL );

    if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
    }

    //
    // Remove Absolute Pointer 
    //

    ListPtr = AbsPointerList.pHead;
    while ( ListPtr != NULL)
    {
        AbsPointerNode = OUTTER(ListPtr, Link, ABSOLUTE_POINTER);
        if ( AbsPointerNode->Handle == Controller)
        {
            DListDelete(&AbsPointerList, ListPtr);
            Status = pBS->FreePool(AbsPointerNode);
            break;
        }
        
        ListPtr = ListPtr->pNext;
    }

    Status = pBS->CloseProtocol(
                    Controller,
                    &gEfiAbsolutePointerProtocolGuid,
                    This->DriverBindingHandle,
                    Controller);

    return Status;
}

/**
    Polling function to get mouse movement and action

    @param  This - Mouse Protocol Pointer

    @retval EFI_SUCCESS

**/  

EFI_STATUS 
MousePoll (
    IN  DXE_MOUSE_PROTOCOL       *This
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;

    MDP = gMDP;

    GetMouseAction(NULL, MDP);

    return EFI_SUCCESS;

}

/**
    Initialises mouse movement related events and restict its movement to a 
    defined area.

    @param  This - Mouse Protocol Pointer
    @param  StartX - Min Row for the mouse movement
    @param  StartY - Min Coloum for the mouse movement 
    @param  EndX - Maximim Row for the mouse movement
    @param  EndY - Maximum Coloum for the mouse movement

          
    @retval EFI_SUCCESS Initialised mouse movement successfully

**/
 
EFI_STATUS 
InitializePointingDevice (
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  INT32               StartX,
    IN  INT32               StartY,
    IN  INT32               EndX,
    IN  INT32               EndY
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    EFI_STATUS          Status;
    EFI_HANDLE          *HandleBuffer;
    UINTN                HandleCount;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL TransPixel = O;

    MDP = gMDP;

    //
    //Locating GOP protocol
    //
    if (MDP->Gop == NULL){
        Status = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, \
                                     (VOID**) &MDP->Gop);
        //
        // Update var "gGOPLocated" for GOP installation
        //
        gGOPLocated = TRUE;
        if (EFI_ERROR(Status)) {
            gGOPLocated = FALSE;
            return EFI_NOT_FOUND;
        }
    }
    //
    // Clear mouse pointer if it is already initialized
    //
    ClearMousePointer (MDP);

    //
    //For Displaying the Mouse Cursor when the Driver is Started
    //

    PreviousXPosition = ((EndX - StartX) / 2) + StartX;
    PreviousYPosition = ((EndY - StartY) / 2) + StartY;

    CurrentXPosition = PreviousXPosition;
    CurrentYPosition = PreviousYPosition; 

    //
    //Initialize the mouse speed and pointer property
    //
    MDP->MovementSpeed = MOUSE_BASE_SPEED;
    MDP->MousePointerProperty = MOUSE_POINTER_PROPERTY_NORMAL;
    MDP->MousePointerUpdate = FALSE;

    //
    //Setting the Boundary for the Mouse Movement
    //
    MouseStartRow = StartX;
    MouseStartColoum = StartY;
    MouseEndRow = EndX;
    MouseEndColoum = EndY;

    ResetPointingDevices (MDP);

    MDP->MouseCursorUpdate = TRUE;

    //
    // Initialize Mouse Pointer data
    //
    MDP->CursorWidth = CURSOR_WIDTH;
    MDP->CursorHeight = CURSOR_HEIGHT;
    MDP->PreviousCursorWidth = CURSOR_WIDTH;
    MDP->PreviousCursorHeight = CURSOR_HEIGHT;
    MDP->TransparentPixel = TransPixel;

    if (MousePointerBuffer == NULL) {
        //
        //Allocating Resource to BltBuffer
        //
        Status = pBS->AllocatePool(EfiBootServicesData, 
                                   sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * MDP->CursorWidth \
                                   * MDP->CursorHeight, (VOID**) &MousePointerBuffer);

        if (EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }
    }

    pBS->CopyMem(MousePointerBuffer, MousePointer, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) \
                    * MDP->CursorWidth * MDP->CursorHeight);

    if (BltBuffer == NULL) {
        //
        //Allocating Resource to BltBuffer
        //
        Status = pBS->AllocatePool(EfiBootServicesData, \
                                    sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * MDP->CursorWidth \
                                    * MDP->CursorHeight, (VOID**) &BltBuffer);
        if (EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }
    }

    if (MouseBltBuffer == NULL) {
        //
        //Allocating Resource to BltBuffer
        //
        Status = pBS->AllocatePool(EfiBootServicesData, \
                                    sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * MDP->CursorWidth \
                                    * MDP->CursorHeight, (VOID**) &MouseBltBuffer);
        if (EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }
    }

    if (gMDP->GetMouseActionEvent == NULL) {
        //
        //Event for Double Click
        //
        Status = pBS->CreateEvent(
                        EVT_TIMER|EVT_NOTIFY_SIGNAL,
                        TPL_NOTIFY, GetMouseAction,
                        gMDP, &gMDP->GetMouseActionEvent
                        );
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }

    if (MDP->LeftClickEvent == NULL) {
        //
        //Event for Double Click
        //
        Status = pBS->CreateEvent(
                        EVT_TIMER|EVT_NOTIFY_SIGNAL,
                        TPL_NOTIFY,
                        LeftClickCallBack,
                        MDP,
                        &MDP->LeftClickEvent
                        );
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }
#if DRAW_MOUSE_ON_HOTPLUG
    if (gMDP->ChecktoDrawMouseEvent == NULL) {
        //
        //Event for Displaying or erasing mouse pointer during 
        // hotplug or unplug
        //
        Status = pBS->CreateEvent(
                EVT_TIMER|EVT_NOTIFY_SIGNAL,
                TPL_NOTIFY,
                ChecktoDrawMouse,
                gMDP,
                &gMDP->ChecktoDrawMouseEvent
                );
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }
#endif
    
    if (SimplePointerList.Size != 0) { 
        //  
        //  Find out the Number of handles having Simple Pointer Protocol installed on them.
        //

        Status = pBS->LocateHandleBuffer (
                      ByProtocol,
                      &gEfiSimplePointerProtocolGuid,
                      NULL,
                      &HandleCount,
                      &HandleBuffer
                      );   
        if (EFI_ERROR(Status)) {
            return Status;
        }

        if (HandleCount == 1){
            //
            // If the Handle Count is One; check if that Handle is the Consplitter Handle
            // Dont draw the MousePointer if the Handle is the Consplitter Handle
            //
            if (pST->ConsoleInHandle != HandleBuffer[0]){
                DrawMousePointer(MDP);
            }
           
        } else {
            DrawMousePointer(MDP);
        }

    }
    
#if MOUSE_POINTER_FOR_ABS_DEVICE
    if (AbsPointerList.Size != 0) { 
        //  
        //  Find out the Number of handles having Absolute Pointer Protocol installed on them.
        //
        Status = pBS->LocateHandleBuffer (
                      ByProtocol,
                      &gEfiAbsolutePointerProtocolGuid,
                      NULL,
                      &HandleCount,
                      &HandleBuffer
                      );   
        if (EFI_ERROR(Status)) {
            return Status;
        }

        if (HandleCount == 1){
            //
            // If the Handle Count is One; check if that Handle is the Consplitter Handle
            // Dont draw the MousePointer if the Handle is the Consplitter Handle
            //
            if (pST->ConsoleInHandle != HandleBuffer[0]){
                DrawMousePointer(MDP);
            }
           
        } else {
            DrawMousePointer(MDP);
        }
    }
#endif    

    Status = pBS->SetTimer(MDP->GetMouseActionEvent, TimerPeriodic, \
                                                     TIMER_ONE_SECOND/ MOUSE_TIMER_EVENT_RATE);
    if (EFI_ERROR(Status)) { 
        pBS->CloseEvent(MDP->GetMouseActionEvent);
    }
#if DRAW_MOUSE_ON_HOTPLUG
    Status = pBS->SetTimer(MDP->ChecktoDrawMouseEvent, TimerPeriodic, TIMER_ONE_SECOND );
    if (EFI_ERROR(Status)) { 
        pBS->CloseEvent(MDP->ChecktoDrawMouseEvent);
    }
#endif
    
    return Status;
}

/**
    Starts updating the mouse pointer movement.

    @param  This - Mouse Protocol Pointer

    @retval 
        EFI_SUCCESS
 
**/
 
EFI_STATUS 
StartUpdatingMouseCursor (
    IN  DXE_MOUSE_PROTOCOL  *This
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    
    //
    // Check GOP instance has correct address
    //
    if (!gGOPLocated) {
        return EFI_SUCCESS;
    }

    MDP = gMDP;

    MDP->MouseCursorUpdate = TRUE;
    return EFI_SUCCESS;
}

/**
    Stops updating the mouse pointer movement and removes pointer from the screen.

    @param  This - Mouse Protocol Pointer

    @retval 
        EFI_SUCCESS
 
**/
  
EFI_STATUS 
StopUpdatingMouseCursor (
    IN  DXE_MOUSE_PROTOCOL  *This
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    
    //
    // Check GOP instance has correct address
    //
     if (!gGOPLocated) {
         return EFI_SUCCESS;
     }

    MDP = gMDP;
    
    if (MDP->MouseCursorUpdate) {

        MDP->MouseCursorUpdate = FALSE;

        if (MDP->MousePointerPresent && 
            (MDP->MousePointerProperty != MOUSE_POINTER_PROPERTY_HIDE)) {
            if (!MDP->GOPUpdated) {
                //
                //Write Screen Data Back
                //
                MDP->Gop->Blt(MDP->Gop, BltBuffer, EfiUgaBltBufferToVideo, \
                              0, 0, PreviousXPosition, PreviousYPosition, \
                              MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
            } else {
                //
                // If GOP is overridden send buffer as NULL to hide the Mouse
                //
                MDP->Gop->Blt(MDP->Gop, NULL, EfiUgaBltBufferToVideo, \
                              0, 0, PreviousXPosition, PreviousYPosition, \
                              MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
            }
            pBS->SetMem(MouseBltBuffer, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) \
                            * MDP->CursorWidth * MDP->CursorHeight, 0);
        }
    }
    return EFI_SUCCESS;
}
 
/**
    Redraws the mouse pointer on the screen if it is corrupted.

    @param  This - Mouse Protocol Pointer

    @retval 
        EFI_SUCCESS
 
**/
 
EFI_STATUS 
RefreshMouseCursor (
    IN  DXE_MOUSE_PROTOCOL  *This
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    EFI_STATUS              Status;
    UINT32                  BufferCount = 0;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *tempBltBuffer = NULL;
    UINTN                   i, j;  
    
    //
    // Check GOP instance has correct address
    //
    if (!gGOPLocated) {
        return EFI_SUCCESS;
    }   

    MDP = gMDP;

    if (!MDP->MousePointerPresent) {
//
// When MOUSE_POINTER_FOR_ABS_DEVICE is disabled set 
// MouseCursorUpdate to true here to update further mouseactions
//
#if !MOUSE_POINTER_FOR_ABS_DEVICE
        MDP->MouseCursorUpdate = TRUE;
#endif
        return EFI_SUCCESS;
    }

    if (MDP->MousePointerProperty == MOUSE_POINTER_PROPERTY_HIDE) {
        MDP->MouseCursorUpdate = TRUE;
        return EFI_SUCCESS;
    }


    InMousePointDraw = TRUE;

    if (MDP->MouseCursorUpdate) {
        //
        // Allocating Resource to tempBltBuffer
        //
        Status = pBS->AllocatePool(EfiBootServicesData, \
                                  sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * MDP->CursorWidth \
                                  * MDP->CursorHeight, (VOID**) &tempBltBuffer);

        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            return Status;
         }
        //
        //First Read the Background
        //
        Status = MDP->Gop->Blt(MDP->Gop, tempBltBuffer, EfiUgaVideoToBltBuffer,
                           CurrentXPosition, CurrentYPosition, 0, 0,
                           MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            return Status;
         }
        //
        //Check Mouse Pointer already Present in the Screen.
        //

        while (tempBltBuffer[BufferCount].Red==MouseBltBuffer[BufferCount].Red && 
               tempBltBuffer[BufferCount].Green==MouseBltBuffer[BufferCount].Green && 
               tempBltBuffer[BufferCount].Blue==MouseBltBuffer[BufferCount].Blue) {
            if (BufferCount == (MDP->CursorWidth * MDP->CursorHeight)) {
                break;
            }
            BufferCount++;
        }
    }

    if (BufferCount < (UINT32)(MDP->CursorWidth * MDP->CursorHeight)) {

        Status = MDP->Gop->Blt(MDP->Gop, 
                               MouseBltBuffer, EfiUgaVideoToBltBuffer,
                               CurrentXPosition, CurrentYPosition, 0, 0,
                               MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);

                for (i=0; i<MDP->CursorHeight; i++) {
                    for (j=0; j<MDP->CursorWidth; j++){
                        if ((MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Red != MDP->TransparentPixel.Red) &&
                            (MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Green != MDP->TransparentPixel.Green) &&
                            (MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Blue != MDP->TransparentPixel.Blue)) {

                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Red   =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Red;
                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Green =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Green;
                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Blue  =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Blue;
                        }
                    }
                }

        //
        //Read the Screen Data
        //
        Status = MDP->Gop->Blt(MDP->Gop,
                               BltBuffer, EfiUgaVideoToBltBuffer,
                               CurrentXPosition, CurrentYPosition, 0, 0,
                               MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            return Status;
         }
        //
        //Write Mouse Cursor.
        //
        Status = MDP->Gop->Blt(MDP->Gop,
                               MouseBltBuffer, EfiUgaBltBufferToVideo,
                               0, 0, CurrentXPosition, CurrentYPosition,
                               MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            return Status;
         }
    }
 
    InMousePointDraw = FALSE;


    MDP->MouseCursorUpdate = TRUE;

    //
    // Free the tempBltbuffer
    //
    if (tempBltBuffer != NULL) {
        pBS->FreePool(tempBltBuffer);
        tempBltBuffer = NULL;
    }

    return EFI_SUCCESS; 
}

/**
    Returns the current mouse position.

    @param  This - Mouse Protocol Pointer
    @param  PointerX - current X-axis position.
    @param  PointerY - current Y-axis position.
    @param  PointerZ - current Z-axis position.

    @retval 
        EFI_SUCCESS

**/  

EFI_STATUS  
GetCoordinates (
    IN  DXE_MOUSE_PROTOCOL   *This,
    OUT INT32               *PointerX,
    OUT INT32               *PointerY,
    OUT INT32               *PointerZ
)
{
    *PointerX = CurrentXPosition;
    *PointerY = CurrentYPosition;
    *PointerZ = CurrentZPosition;

    return EFI_SUCCESS;
}

/**
    Sets the mouse pointer position.

    @param  This - Mouse Protocol Pointer
    @param  PointerX - X-axis position
    @param  PointerY - Y-axis position

          
    @retval EFI_SUCCESS Coordinate values updated successfully
    @retval EFI_INVALID_PARAMETER Invalid input values

**/  

EFI_STATUS  
SetCoordinates (
    IN  DXE_MOUSE_PROTOCOL   *This,
    IN  INT32                PointerX,
    IN  INT32                PointerY
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;

    MDP = gMDP;
    //
    // Validate the input coordinates
    //
    if ((PointerX < MouseStartRow || 
         PointerX > MouseEndRow - (INT32)MDP->CursorWidth) ||
        (PointerY < MouseStartColoum || 
         PointerY > MouseEndColoum - (INT32)MDP->CursorHeight)) {
        return EFI_INVALID_PARAMETER;
    }

    CurrentXPosition = PointerX;
    CurrentYPosition = PointerY;

    if (MDP->MousePointerPresent && MDP->MouseCursorUpdate) {
        DrawMousePointer(MDP);
    }
    PreviousXPosition = PointerX;
    PreviousYPosition = PointerY;

    return EFI_SUCCESS;
}
 
/**
    Sets the Speed of mouse movement

    @param  This - Mouse Protocol Pointer
    @param  Value - Speed of the move movement

          
    @retval EFI_SUCCESS driver stopped successfully
    @retval EFI_UNSUPPORTED Invalid input values.
 
**/
  
EFI_STATUS  
SetMouseSpeed (
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  UINT32               Value
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;

    MDP = gMDP;

    if (Value >= 1 && Value <= 5) {
        MDP->MovementSpeed = Value;
    } else {
        return EFI_UNSUPPORTED;
    }
    return EFI_SUCCESS;
}

/**
    Returns the mouse button status.

    @param  This Mouse Protocol Pointer
    @param  ButtonStatus Device Button status

    @retval 
        EFI_STATUS 

    @note  
      If there is no mouse click then
      *ButtonStatus = TSEMOUSE_NULL_CLICK

**/ 
 
EFI_STATUS  
GetButtonStatus (
    IN  DXE_MOUSE_PROTOCOL  *This,
    OUT INT32               *ButtonStatus
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
  
    static INT32    PeriviousXPosition = 0; 
    static INT32    PeriviousYPosition = 0;
    static INT32    PerviousAbsXPostion = 0;
    static INT32    PerviousAbsYPostion = 0;
    static INT32    PreviousButtonStatus = TSEMOUSE_NULL_CLICK;
    INT8            ClickPixelDifference = MOUSE_CLICK_RANGE;
    INT8            SingleTouchPixelDifference;
    INT8            DoubleTouchPixelDifference;
#if SOFTKBD_ICON_SUPPORT || BITLOCKER_SOFTKBD_SUPPORT
    EFI_STATUS      Status;
    UINT8           ClickOnIcon = 0;
#endif
    *ButtonStatus = TSEMOUSE_NULL_CLICK;
    //
    // Check GOP instance has correct address
    //
    if (!gGOPLocated) {
        return EFI_SUCCESS;
    }

    InMousePointDraw = TRUE;

    MDP = gMDP;

    // Calculate the Pixel differece for Abs click/Dclick 
    // based on screen resolution.
    SingleTouchPixelDifference = (MDP->Gop->Mode->Info->HorizontalResolution/DOUBLE_TOUCH_RANGE)/100;
    DoubleTouchPixelDifference = (MDP->Gop->Mode->Info->HorizontalResolution/MOUSE_CLICK_RANGE)/100;
    
    //
    // Process the Softkeyboard Event
    //
#if SOFTKBD_ICON_SUPPORT || BITLOCKER_SOFTKBD_SUPPORT
    if (MDP->LeftPressCount > 0 || MDP->AbsMouseClickCount > 0) {

        ClickOnIcon = IconClick(MDP, TRUE);

        if (ClickOnIcon) {

            if ((MDP->LeftReleaseCount || MDP->AbsMouseRelCount) &&
                //
                // Check Click and Release is within Icon area.
                //
               (ClickOnIcon == IconClick(MDP, FALSE)) ) { 

                if (MDP->SoftKbd == NULL) {
                    Status = pBS->LocateProtocol(&gEfiSoftKbdProtocolGuid, NULL, (VOID**) &MDP->SoftKbd);
                    ASSERT_EFI_ERROR(Status);                   
                }

                switch(ClickOnIcon) {
                    case ESC_ICON:
                        //
                        // Signal a event that Esc key icon pressed by User
                        //
                        SignalProtocolEvent(&gProcessEscKeyProtocolGuid);
                        break;
                    case ACT_ICON:
                        //
                        // When Single Icon for ACT/DEACT this case handles both
                        // activate and deactivate.
                        //
                        if (!MDP->SoftKbd->SoftKbdActive) {
                            //
                            // Initialize Soft Keyboard Layout to FULL US layout
                            //
                            MDP->SoftKbd->SetKbdLayout((SOFT_KBD_PROTOCOL*)MDP->SoftKbd, SOFT_KEY_LAYOUT_FULL_US_KEYBOARD);
                            //
                            //Signal a event that SoftKeyboard activated by User
                            //
                            SignalProtocolEvent(&gActivateSoftKeyboardProtocolGuid);
                        } else {
                            //
                            //Signal a event that SoftKeyboard Deactivated by User
                            //
                            SignalProtocolEvent(&gDeActivateSoftKeyboardProtocolGuid);
                        }
                        break;
                    case DEACT_ICON:
                        //
                        //Signal a event that SoftKeyboard Deactivated by User
                        //
                        SignalProtocolEvent(&gDeActivateSoftKeyboardProtocolGuid);
                        break;
                    case POST_ICON:
                        //
                        //Signal a event when setup icon pressed by User
                        //
                        SignalProtocolEvent(&gProcessPostKeyProtocolGuid);
                        break;     
                    default:
                        break;
                }
                //
                // Clear click and position related values
                //
                MDP->LeftReleaseCount = 0;
                MDP->AbsMouseRelCount = 0;
                MDP->LeftXPosition = 0;
                MDP->LeftXRelPosition = 0;
                MDP->LeftYPosition = 0;
                MDP->LeftYRelPosition = 0;
                MDP->AbsXPosition = 0;
                MDP->AbsXRelPosition = 0;
                MDP->AbsYPosition = 0;
                MDP->AbsYRelPosition = 0;
            }
            MDP->LeftPressCount = 0;
            MDP->AbsMouseClickCount = 0;
            InMousePointDraw = FALSE;
            return EFI_NOT_FOUND;
        }
    }
#endif // #if SOFTKBD_ICON_SUPPORT

    //
    //If Right Mouse Button Pressed
    //
    if (MDP->RightPressCount == 1) {
        if (MDP->RightReleaseCount) {
            if ( MDP->RightXPosition >= (MDP->RightXRelPosition - ClickPixelDifference) &&
                 MDP->RightXPosition <= (MDP->RightXRelPosition + ClickPixelDifference) &&
                 MDP->RightYPosition >= (MDP->RightYRelPosition - ClickPixelDifference) &&
                 MDP->RightYPosition <= (MDP->RightYRelPosition + ClickPixelDifference)) {
                *ButtonStatus = TSEMOUSE_RIGHT_CLICK;
            } else {
                *ButtonStatus = TSEMOUSE_RIGHT_UP;
            }
            MDP->RightReleaseCount = 0;
            PreviousButtonStatus = TSEMOUSE_NULL_CLICK;
        } else {
            PreviousButtonStatus = *ButtonStatus = TSEMOUSE_RIGHT_DOWN;
        }
        MDP->RightPressCount = 0;
    }

/*    //
    //If Right Mouse Button Pressed
    //
    if (MDP->MidPressCount == 1) {
        if (MDP->MidReleaseCount) {
            if (MDP->MidXPosition == MDP->MidXRelPosition &&
                MDP->MidYPosition == MDP->MidYRelPosition) {
                *ButtonStatus = TSEMOUSE_MIDDLE_CLICK;
            } else {
                *ButtonStatus = TSEMOUSE_MIDDLE_UP;
            }
            MDP->MidReleaseCount = 0;
        } else {
            *ButtonStatus = TSEMOUSE_MIDDLE_DOWN;
        }
        MDP->MidPressCount = 0;
    }
*/
    //
    //If Left Mouse Button Pressed
    //
    if (MDP->LeftPressCount == 1) {
        PeriviousXPosition = MDP->LeftXPosition;
        PeriviousYPosition = MDP->LeftYPosition;
        if (MDP->LeftReleaseCount) {
            if ( MDP->LeftXPosition >= (MDP->LeftXRelPosition - ClickPixelDifference) &&
                 MDP->LeftXPosition <= (MDP->LeftXRelPosition + ClickPixelDifference) &&
                 MDP->LeftYPosition >= (MDP->LeftYRelPosition - ClickPixelDifference) &&
                 MDP->LeftYPosition <= (MDP->LeftYRelPosition + ClickPixelDifference)) {
                *ButtonStatus = TSEMOUSE_LEFT_CLICK;
            } else {
                *ButtonStatus = TSEMOUSE_LEFT_UP;
            }
            MDP->LeftReleaseCount = 0;
            PreviousButtonStatus = TSEMOUSE_NULL_CLICK;
        } else {
            PreviousButtonStatus = *ButtonStatus = TSEMOUSE_LEFT_DOWN; 
        }
        MDP->LeftPressCount = 0;
    }

    //
    //If Left Mouse Button Pressed Twice
    //
    if (MDP->LeftPressCount == 2) {
        if (MDP->LeftReleaseCount) {
            if ((PeriviousXPosition >= (MDP->LeftXRelPosition - ClickPixelDifference)) &&
                (PeriviousXPosition <= (MDP->LeftXRelPosition + ClickPixelDifference)) &&
                (PeriviousYPosition >= (MDP->LeftYRelPosition - ClickPixelDifference)) &&
                (PeriviousYPosition <= (MDP->LeftYRelPosition + ClickPixelDifference))) {
                *ButtonStatus = TSEMOUSE_LEFT_DCLICK;
            } else {
               *ButtonStatus = TSEMOUSE_LEFT_UP;
            } 
            MDP->LeftReleaseCount = 0;
            PreviousButtonStatus =TSEMOUSE_NULL_CLICK;
       } else {
                PreviousButtonStatus = *ButtonStatus = TSEMOUSE_LEFT_DOWN; 
       }
       MDP->LeftPressCount = 0;
    }

    //
    //Absolute Pointer Single Click
    //
    if (MDP->AbsMouseClickCount == 1) {
        PerviousAbsXPostion = MDP->AbsXPosition;
        PerviousAbsYPostion = MDP->AbsYPosition;
        if (MDP->AbsMouseRelCount) {
            if ((MDP->AbsXPosition >= (MDP->AbsXRelPosition - SingleTouchPixelDifference) &&
                 MDP->AbsXPosition <= (MDP->AbsXRelPosition + SingleTouchPixelDifference)) &&
                (MDP->AbsYPosition >= (MDP->AbsYRelPosition - SingleTouchPixelDifference) &&
                 MDP->AbsYPosition <= (MDP->AbsYRelPosition + SingleTouchPixelDifference))) {
                *ButtonStatus = TSEMOUSE_LEFT_CLICK;
            } else {
                *ButtonStatus = TSEMOUSE_LEFT_UP;
            }
            MDP->AbsMouseRelCount = 0;
            PreviousButtonStatus = TSEMOUSE_NULL_CLICK;
        } else {
            PreviousButtonStatus = *ButtonStatus = TSEMOUSE_LEFT_DOWN;
        }
        MDP->AbsMouseClickCount=0;
    }

    //
    //If Absolute Pointer 2nd Click. If the 2nd click is within the Perivious 
    //
    if (MDP->AbsMouseClickCount == 2) {
        if ((PerviousAbsXPostion >= (MDP->AbsXRelPosition - DoubleTouchPixelDifference) &&
             PerviousAbsXPostion <= (MDP->AbsXRelPosition + DoubleTouchPixelDifference)) &&
            (PerviousAbsYPostion >= (MDP->AbsYRelPosition - DoubleTouchPixelDifference) &&
             PerviousAbsYPostion <= (MDP->AbsYRelPosition + DoubleTouchPixelDifference))) {
                *ButtonStatus = TSEMOUSE_LEFT_DCLICK;
                MDP->AbsMouseRelCount = 0;
                //
                // DCLICK Reported . Do not consider next click/touch release as new click.
                //
                MDP->ReportedAbsDClick = TRUE;
        } else {
            if (MDP->AbsMouseRelCount) {
                *ButtonStatus = TSEMOUSE_LEFT_CLICK;
                MDP->AbsMouseRelCount = 0;
                PreviousButtonStatus = TSEMOUSE_NULL_CLICK;
            } else {
                PreviousButtonStatus = *ButtonStatus = TSEMOUSE_LEFT_DOWN;
                //
                // DCLICK not Reported. Consider next click/touch release as new click.
                //
                MDP->ReportedAbsDClick = FALSE;
            }
        }
        MDP->AbsMouseClickCount = 0;
    }
    //
    // Report BUTTON_DOWN state when button/touch pressed down 
    // till released without movement. If not then NULL_CLICK will be reported.
    //
    if ( PreviousButtonStatus != TSEMOUSE_NULL_CLICK) {
        *ButtonStatus = PreviousButtonStatus;
    }
    InMousePointDraw = FALSE;


    return EFI_SUCCESS;
}

/**
    Sets and restricts the mouse movement viewpoint.

    @param  This - Mouse Protocol Pointer
    @param  MouseViewStartRow - Start row position
    @param  MouseViewStartColoum - Start coloum position
    @param  MouseViewEndRow - End row position
    @param  MouseViewEndColoum - End coloum position

         
    @retval EFI_SUCCESS driver stopped successfully
 
**/
   
EFI_STATUS  
SetMouseCoordinates (
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  INT32   MouseViewStartRow,
    IN  INT32   MouseViewStartColoum,
    IN  INT32   MouseViewEndRow,
    IN  INT32   MouseViewEndColoum
)
{
    

    MouseStartRow = MouseViewStartRow;
    MouseStartColoum = MouseViewStartColoum;
    MouseEndRow = MouseViewEndRow;
    MouseEndColoum = MouseViewEndColoum;
    return EFI_SUCCESS;
}

/**
    Sets Softkeyboard Activate, Deactivate Button and ESC Icon co-ordinates. 
    So Mouse driver can inform others when there is click on Activate and 
    Deactivate button.

    @param  This Mouse Protocol Pointer
    @param  SoftKeyData Structure that contains SoftKbd related
            button Co-ordinates.

    @retval 
        EFI_SUCCESS
 
**/
   
EFI_STATUS 
SetSoftKeyData (
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  SOFT_KEY_DATA       *SoftKeyData
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;

    MDP = gMDP;

    pBS->CopyMem(&MDP->SoftKeyboardData, SoftKeyData, sizeof(SOFT_KEY_DATA));
    return EFI_SUCCESS;
}

/**
    Stops mouse movement related events and stops accessing SimplePointer 
    and AbsolutePointer devices.

    @param  This - Mouse Protocol Pointer

    @retval 
        EFI_SUCCESS

**/
   
EFI_STATUS  
StopPointingDevice (
    IN  DXE_MOUSE_PROTOCOL  *This
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    EFI_STATUS              Status;

    MDP = gMDP;
     
    if (MDP->GetMouseActionEvent != NULL) {
        Status = pBS->SetTimer(MDP->GetMouseActionEvent, TimerCancel, 0);
        ASSERT_EFI_ERROR(Status);
        pBS->CloseEvent(MDP->GetMouseActionEvent);
        MDP->GetMouseActionEvent = NULL;
    }
         
     if (MDP->LeftClickEvent != NULL) {
        Status = pBS->SetTimer(MDP->LeftClickEvent, TimerCancel, 0);  
        ASSERT_EFI_ERROR(Status);   
        pBS->CloseEvent(MDP->LeftClickEvent);
        MDP->LeftClickEvent=NULL;
    }
#if DRAW_MOUSE_ON_HOTPLUG
    
    if (MDP->ChecktoDrawMouseEvent != NULL) {
        Status = pBS->SetTimer(MDP->ChecktoDrawMouseEvent, TimerCancel, 0);
        ASSERT_EFI_ERROR(Status);
        pBS->CloseEvent(MDP->ChecktoDrawMouseEvent);
        MDP->ChecktoDrawMouseEvent = NULL;
    }
#endif  
    if (BltBuffer != NULL) {
        pBS->FreePool(BltBuffer);
        BltBuffer = NULL;
    }

    if (MouseBltBuffer != NULL) {
        pBS->FreePool(MouseBltBuffer);
        MouseBltBuffer = NULL;
    }

    if (MousePointerBuffer != NULL) {
        pBS->FreePool(MousePointerBuffer);
        MousePointerBuffer = NULL;
    }

    MDP->MousePointerPresent = FALSE;

    return EFI_SUCCESS;
}

/**
    This function is provided to allow overriding of the default mouse 
    speed translation table.

    @param  This - Mouse Protocol pointer
    @param  NewTable - Pointer to new a speed 
            translation table
    @param  NewTableSize - New table size.

    @retval 
        EFI_STATUS

**/

EFI_STATUS 
SetSpeedTranslationTable (
    IN  DXE_MOUSE_PROTOCOL       *This,
    IN  MOUSE_TRANSLATION_TABLE  *NewTable,
    IN  UINT32                   NewTableSize
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    EFI_STATUS              Status = EFI_INVALID_PARAMETER;

    MDP = gMDP;
    
    if (NewTable != NULL && NewTableSize > 0) {
        MDP->AccelerationData.TranslationTable = NewTable;
        MDP->AccelerationData.TableSize = NewTableSize;
        Status = EFI_SUCCESS;
    }
    
    return Status;
}

#if DRAW_MOUSE_ON_HOTPLUG

/**
    This is a timer event to draw or clear mouse pointer based on hot plug or
    unplug event

    @param  Event 
    @param  Context 
    @param  IN MOUSE_DRIVER_PRIVATE *MDP -  Mouse Driver Private structure Pointer

    @retval VOID

**/   

VOID 
ChecktoDrawMouse (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
)
{
    UINTN       CountSP = 0,CountAbs = 0;
    UINTN       PhysicalSP = 0;
    UINTN       PhysicalAbs = 0;
    UINT8       i = 0; 
    EFI_HANDLE  *BufferSP = NULL,*BufferAbs = NULL;
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)Context;  

    //
    // Locates the simple pointer protocol, if count is 1 it means only
    // Consplit has openned it.
    //
    pBS->LocateHandleBuffer(ByProtocol,
                    &gEfiSimplePointerProtocolGuid, NULL, &CountSP, &BufferSP);
   
    // checking for the Physical Simple pointer device   
    for (i = 0; i < CountSP; i++ ) {
        // Ignoring the Consplitter handle if present
        if (pST->ConsoleInHandle == BufferSP[i]){
           continue;
        }
        PhysicalSP++;
    }
    //
    // Locates the absolute pointer protocol only if the
    // MOUSE_POINTER_FOR_ABS_DEVICE is set.
    //
#if MOUSE_POINTER_FOR_ABS_DEVICE
    pBS->LocateHandleBuffer(ByProtocol,
                    &gEfiAbsolutePointerProtocolGuid, NULL, &CountAbs, &BufferAbs);
    
    // checking for the Physical Absolute pointer device
    for (i = 0; i < CountAbs; i++ ) {
        // Ignoring the Consplitter handle if present
       if (pST->ConsoleInHandle == BufferAbs[i]){
           continue;
       }
       PhysicalAbs++;
    }
   
#endif

    //
    // If the sum of number of handles is zero which means only
    // ConSplitter has openned the handle so in that case clear the
    // mouse pointer else draw the mouse pointer
    //
    
    
    if((PhysicalSP + PhysicalAbs) == 0) {
         ClearMousePointer(MDP);
         gPointerDevicePresent = FALSE;
    } else {
         MDP->MouseCursorUpdate = TRUE;
         gPointerDevicePresent = TRUE;
         DrawMousePointer(MDP);         
    }

    //
    // Free the allocated memory
    //
    if ( BufferSP != NULL ){
        pBS->FreePool( BufferSP );
    }
    if ( BufferAbs != NULL ){
        pBS->FreePool( BufferAbs );
    }
    return;

}
#endif
/**
    This function returns the current speed of the mouse device.

    @param  This - pointer to the mouse protocol
    @param  MouseVelocity - contains the current mouse speed upon return

    @retval 
        EFI_STATUS

**/

EFI_STATUS 
GetMouseVelocity (
    IN DXE_MOUSE_PROTOCOL   *This,
    OUT UINT32              *MouseVelocity
)
{
    EFI_STATUS                Status = EFI_SUCCESS;
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    MDP = gMDP;
    //MOUSE_DRIVER_PRIVATE      *Mouse = (MOUSE_DRIVER_PRIVATE*)This;
    //MOUSE_ACCELERATION_DATA   *Adata = &Mouse->AccelerationData;
    

    if (MouseVelocity != NULL){
        //*MouseVelocity = Adata->RealSpeed;
        *MouseVelocity = MDP->AccelerationData.RealSpeed;
    }

    return Status;
}

/**
    Updates the Mouse pointer shape with the input pointer value.

    @param  This - Pointer to the mouse protocol.
    @param  MousePointerBlt - Blt buffer of new Mouse pointer.
    @param  Width - Width in pixels of new Mousepointer.
    @param  Height - Height in pixels of new Mousepointer.
    @param  TransparentPixel - Pixel color in the input mouse pointer buffer that is transparent (screen background)
      
**/ 

EFI_STATUS 
UpdateMousePointer (
    IN  DXE_MOUSE_PROTOCOL             *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MousePointerBlt,
    IN  UINT32                         Width,
    IN  UINT32                         Height,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *TransparentPixel
)
{

    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    EFI_STATUS              Status;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL           TransPixel = O;
    BOOLEAN                 RestorePointer = FALSE;

    MDP = gMDP;

    //
    //If input buffer is NULL, Initialize Mouse Pointer to default
    //
    if ( !MousePointerBlt || !Width || !Height || !TransparentPixel) {
        RestorePointer = TRUE;
    }
    
    if (RestorePointer) {
        Width = CURSOR_WIDTH;
        Height = CURSOR_HEIGHT;
    }
    //
    // Restore the background if mouse pointer is present and not hidden
    //
    if (MDP->MousePointerPresent && MDP->MouseCursorUpdate &&
        MDP->MousePointerProperty != MOUSE_POINTER_PROPERTY_HIDE) {
           if (!MDP->GOPUpdated) {
                //
                // Restore the Screen back
                //
                MDP->Gop->Blt(MDP->Gop, BltBuffer, EfiUgaBltBufferToVideo, \
                          0, 0, PreviousXPosition, PreviousYPosition, \
                          MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
            } else {
                //
                // If GOP is overridden send buffer as NULL to hide the Mouse
                //
                MDP->Gop->Blt(MDP->Gop, NULL, EfiUgaBltBufferToVideo, \
                          0, 0, PreviousXPosition, PreviousYPosition, \
                          MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
            }
    }

    if ((Width != MDP->PreviousCursorWidth) ||
        (Height != MDP->PreviousCursorHeight)) {

        //
        // Reinitialize Buffers with new input size
        //
        if (MouseBltBuffer != NULL) {
            pBS->FreePool(MouseBltBuffer);
        }

        if (BltBuffer != NULL) {
            pBS->FreePool(BltBuffer);
        }

        if (MousePointerBuffer != NULL) {
            pBS->FreePool(MousePointerBuffer);
        }

        Status = pBS->AllocatePool(EfiBootServicesData, 
                                    sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height,
                                    (VOID**) &MousePointerBuffer);
        if (EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }

        Status = pBS->AllocatePool(EfiBootServicesData, 
                                    sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height,
                                    (VOID**) &MouseBltBuffer);
        if (EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }

        Status = pBS->AllocatePool(EfiBootServicesData, 
                                    sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height,
                                    (VOID**) &BltBuffer);
        if (EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }
    }

    if (RestorePointer) {

        pBS->CopyMem(MousePointerBuffer, MousePointer, 
                     sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height);
        MDP->TransparentPixel = TransPixel;

    } else {
        pBS->CopyMem(MousePointerBuffer, MousePointerBlt, 
                     sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height);
        MDP->TransparentPixel.Red = TransparentPixel->Red;
        MDP->TransparentPixel.Blue = TransparentPixel->Blue;
        MDP->TransparentPixel.Green = TransparentPixel->Green;
    }

    MDP->CursorWidth = Width;
    MDP->CursorHeight = Height;
    //
    // Update the Mouse Pointer if it is on screen
    //
    if (MDP->MousePointerPresent && MDP->MouseCursorUpdate) {
        MDP->MousePointerUpdate = TRUE;
        DrawMousePointer(MDP);
    }

    MDP->PreviousCursorWidth = Width;
    MDP->PreviousCursorHeight = Height;
    MDP->MousePointerUpdate = FALSE;

    if (RestorePointer)
        return EFI_INVALID_PARAMETER;

    return EFI_SUCCESS;
}

/**
    Updates Mouse Pointer Property value 
    MOUSE_POINTER_PROPERTY_NORMAL    - Show Mouse pointer and report actions 
    MOUSE_POINTER_PROPERTY_FREEZE    - freeze the Mouse pointer and don't update the movement and update the actions
    MOUSE_POINTER_PROPERTY_HIDE      - Hide the Mouse pointer but report the movement and actions


    @param  This Pointer to the mouse protocol.
    @param  Property MousePointer property
    
    @retval 
        EFI_STATUS

**/ 

EFI_STATUS 
UpdateMousePointerProperty (
    IN  DXE_MOUSE_PROTOCOL       *This,
    IN  MOUSE_POINTER_PROPERTY   Property
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;

    MDP = gMDP;

    if (MDP->MousePointerProperty == Property) {
        return EFI_SUCCESS;
    }
    
#if HIDE_POINTER_FOR_TOUCH    
    if(MDP->PointerHideForAbs == TRUE){
        return EFI_SUCCESS;
    }
#endif
    
    switch (Property)
    {
        case 2:
                MDP->MousePointerProperty = MOUSE_POINTER_PROPERTY_HIDE;
                if (MDP->MousePointerPresent && MDP->MouseCursorUpdate) {
                    if (!MDP->GOPUpdated) {
                        //
                        // Restore the Screen back
                        //
                        MDP->Gop->Blt(MDP->Gop, BltBuffer, EfiUgaBltBufferToVideo, \
                                      0, 0, PreviousXPosition, PreviousYPosition, \
                                      MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
                    } else {
                        //
                        // If GOP is overridden send buffer as NULL to hide the Mouse
                        //
                        MDP->Gop->Blt(MDP->Gop, NULL, EfiUgaBltBufferToVideo, \
                                      0, 0, PreviousXPosition, PreviousYPosition, \
                                      MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
                    }
                    pBS->SetMem(BltBuffer, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                * MDP->CursorWidth * MDP->CursorHeight, 0);
                    pBS->SetMem(MouseBltBuffer, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                * MDP->CursorWidth * MDP->CursorHeight, 0);
                }
                break;
        case 1:
                MDP->MousePointerProperty = MOUSE_POINTER_PROPERTY_FREEZE;
                break;
        case 0:
                if (MDP->MousePointerProperty == MOUSE_POINTER_PROPERTY_FREEZE) {
                    MDP->MousePointerProperty = MOUSE_POINTER_PROPERTY_NORMAL;
                    break;
                }
        default:
                MDP->MousePointerProperty = MOUSE_POINTER_PROPERTY_NORMAL;
                if (MDP->MousePointerPresent && MDP->MouseCursorUpdate) {
                    MDP->MousePointerUpdate = TRUE;
                    DrawMousePointer(MDP);
                    MDP->MousePointerUpdate = FALSE;
                }
                break;
    }
    return EFI_SUCCESS;
}

/**
    Updates GOP instance from other application 

    @param  This
    @param  RestoreOptimization

    @retval 
        EFI_STATUS

**/

EFI_STATUS 
SetMouseGOP (
    IN  DXE_MOUSE_PROTOCOL              *This,
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GOP,
    IN  BOOLEAN                         RestoreOptimization
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    EFI_STATUS Status = EFI_SUCCESS;

    MDP = gMDP;

    if (!GOP) {
        MDP->Gop = NULL;
        MDP->GOPUpdated = FALSE;
    } else {
        MDP->Gop = GOP;
        gGOPLocated = TRUE;
        MDP->GOPUpdated= RestoreOptimization;
    }

    return Status;
}

/**
    Returns the mouse image over the passed background buffer

         
    @param  This DXE_MOUSE_PROTOCOL instance
    @param  BkgBuffer Buffer for background
            of mouse image
    @param  MouseBuffer Mouse cursor image on
            top of background buffer
            
    @retval 
        EFI_STATUS status
        EFI_INVALID_PARAMETER

**/

EFI_STATUS  
GetMouseCursorImage (
    IN  DXE_MOUSE_PROTOCOL              *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BkgBuffer,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   **MouseBuffer
)
{

    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)This;
    EFI_STATUS              Status = 0;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *tempBltBuffer = NULL;
    UINTN                   i, j;   

    MDP = gMDP;

    if (BkgBuffer == NULL){
        return EFI_INVALID_PARAMETER;
    }

    if (MouseBuffer == NULL ||  *MouseBuffer == NULL){
        return EFI_INVALID_PARAMETER;
    }

    //
    // If there is no pointer device present then update Mousebuffer with BkgBuffer
    //
#if DRAW_MOUSE_ON_HOTPLUG    
    if(!gPointerDevicePresent) {
        pBS->CopyMem(*MouseBuffer, BkgBuffer, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) \
                      * MDP->CursorWidth * MDP->CursorHeight);
        return EFI_SUCCESS;
    }
#endif

    //
    //Allocating Resource to tempBltBuffer
    //
    Status = pBS->AllocatePool(EfiBootServicesData, \
                               sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * MDP->CursorWidth \
                               * MDP->CursorHeight, (VOID**) &tempBltBuffer);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    pBS->CopyMem(tempBltBuffer, BkgBuffer, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) \
                  * MDP->CursorWidth * MDP->CursorHeight);

                for (i=0; i<MDP->CursorHeight; i++) {
                    for (j=0; j<MDP->CursorWidth; j++){
                        if ((MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Red != MDP->TransparentPixel.Red) &&
                            (MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Green != MDP->TransparentPixel.Green) &&
                            (MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Blue != MDP->TransparentPixel.Blue)) {

                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Red   =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Red;
                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Green =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Green;
                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Blue  =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Blue;
                        }
                    }
                }

    pBS->CopyMem(*MouseBuffer, tempBltBuffer, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) \
                  * MDP->CursorWidth * MDP->CursorHeight);

    //
    //Free the tempBltbuffer
    //
    if (tempBltBuffer != NULL) {
        pBS->FreePool(tempBltBuffer);
        tempBltBuffer = NULL;
    }

    return Status;
}

/**
    Draws the mouse pointer on screen.

    @param  MDP - Mouse Driver Private structure Pointer

    @retval VOID

**/
 
VOID 
DrawMousePointer (
    IN  MOUSE_DRIVER_PRIVATE *MDP
)
{
    EFI_STATUS          Status;
    INT32               XPosition;
    INT32               YPosition;
    UINTN               i, j;   

    if ( MDP == NULL ) {
        return;
    }

    if (!MDP->MouseCursorUpdate || 
        MDP->MousePointerProperty == MOUSE_POINTER_PROPERTY_HIDE) {
        return;
    }

    if (MDP->MousePointerPresent 
        && !MDP->MousePointerUpdate && !MDP->PositionMousePointer) {
        //
        //No mouse installed, Mouse pointer already present and Mouse not started. 
        //return without drawing the Mouse pointer
        //
        return;
    }
    //
    // If Pointer Update cancel Timer Events
    //
    if (MDP->MousePointerUpdate || MDP->PositionMousePointer) {
        InMousePointDraw = TRUE;
    }

    if (MDP->PositionMousePointer) {
        XPosition = CurrentXPosition;
        YPosition = CurrentYPosition;
    } else {
        XPosition = PreviousXPosition;
        YPosition = PreviousYPosition;
    }

    pBS->SetMem(BltBuffer, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                * MDP->CursorWidth * MDP->CursorHeight, 0);
    // 
    //Read Screen Data from the Centre of the Screen
    //
    Status = MDP->Gop->Blt(MDP->Gop,
                           BltBuffer, EfiUgaVideoToBltBuffer,
                           XPosition, YPosition, 0, 0,
                           MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return ;
    }
    //
    //Draw the Mouse Pointer
    //
    Status = MDP->Gop->Blt(MDP->Gop,
                           MouseBltBuffer, EfiUgaVideoToBltBuffer,
                           XPosition, YPosition, 0, 0,
                           MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return ;
    }
                for (i=0; i<MDP->CursorHeight; i++) {
                    for (j=0; j<MDP->CursorWidth; j++){
                        if ((MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Red != MDP->TransparentPixel.Red) &&
                            (MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Green != MDP->TransparentPixel.Green) &&
                            (MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Blue != MDP->TransparentPixel.Blue)) {

                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Red   =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Red;
                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Green =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Green;
                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Blue  =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Blue;
                        }
                    }
                }
    
    Status = MDP->Gop->Blt(MDP->Gop,
                           MouseBltBuffer, EfiUgaBltBufferToVideo,
                           0, 0, XPosition, YPosition,
                           MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return ;
    }
    MDP->MousePointerPresent = TRUE;

    InMousePointDraw = FALSE;


    return;

}

/**
    Clears the mouse pointer when all the mouse disconnected from the system

    @param  IN  MOUSE_DRIVER_PRIVATE *MDP - Mouse Driver Private structure Pointer

          
    @retval VOID

**/
 
VOID 
ClearMousePointer (
    IN  MOUSE_DRIVER_PRIVATE *MDP
)
{
    if (!MDP->MousePointerPresent ) {
        return;
    }
 
    if (!MDP->GOPUpdated) {
        //
        //Write Screen Data Back
        //
        MDP->Gop->Blt(MDP->Gop, \
                      BltBuffer, EfiUgaBltBufferToVideo, \
                      0, 0, PreviousXPosition, PreviousYPosition, \
                      MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
    } else {
        //
        // If GOP is overridden send buffer as NULL to hide the Mouse
        //
         MDP->Gop->Blt(MDP->Gop, NULL, EfiUgaBltBufferToVideo, \
                       0, 0, PreviousXPosition, PreviousYPosition, \
                       MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
    }

    pBS->SetMem(MouseBltBuffer, 
                sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * MDP->CursorWidth \
                                   * MDP->CursorHeight, 0);

    MDP->MousePointerPresent = FALSE;
    return;
}

/**
    Updates Mouse Movement and Mouse click actions.

    @param  Event 
    @param  Context 

    @retval VOID

**/

VOID 
GetMouseAction (
    IN  EFI_EVENT Event, 
    IN  VOID      *Context
)
{

    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)Context;

    //
    // Check flag to avoid Re-Entry
    //
    if (InMousePointDraw) {
        return;
    }

    InMousePointDraw = TRUE;
    GetMouseClick(MDP);
    GetMouseMovement(MDP);
    InMousePointDraw = FALSE;
    return;

}

/**
    Handling the Mouse Pointer Movement

    @param  MDP - Mouse Driver Private structure Pointer

    @retval VOID

**/ 

VOID 
GetMouseMovement (
    IN  MOUSE_DRIVER_PRIVATE *MDP
)
{
    MOUSE_ACCELERATION_DATA *Adata = &MDP->AccelerationData;
    EFI_STATUS          Status;
    BOOLEAN             PositionChanged = FALSE;
    INT32               Speed;
    UINTN               i, j;

    //
    // Check GOP instance has correct address
    //
    if (!gGOPLocated) {
        return;
    }

    if (MDP->MousePointerProperty == MOUSE_POINTER_PROPERTY_FREEZE) {
         return;
    }


    if (MDP->AbsState.CurrentX || 
        MDP->AbsState.CurrentY || 
        MDP->AbsState.CurrentZ) {
        if (MDP->MouseCursorUpdate 
#if !MOUSE_SUPPORT_FOR_NT32
        		&& MDP->ValidPressOrRelease
#endif		
        ) {
            CurrentXPosition = (UINT32)(MDP->AbsState.CurrentX);
            CurrentYPosition = (UINT32)(MDP->AbsState.CurrentY);
            PositionChanged = TRUE;
        }
    }

    if (MDP->SimplePtrState.RelativeMovementX || 
        MDP->SimplePtrState.RelativeMovementY || 
        MDP->SimplePtrState.RelativeMovementZ ) {
        if (MDP->MouseCursorUpdate) {

            //
            //For Controling the Mouse Speed
            //
            Speed = MDP->SimplePtrState.RelativeMovementX / CurrentXResolution;

            //
            // If RelativeMovement is less than the CurrentResolution then
            // set Speed value to 1 to avoid small movement lose.
            //
            if (!Speed && MDP->SimplePtrState.RelativeMovementX) {
                Speed = 1;
                if (MDP->SimplePtrState.RelativeMovementX < 0) {
                    Speed = -1;
                }
                CurrentXPosition = PreviousXPosition 
                                    + Speed * MDP->MovementSpeed;
            } else {
                CurrentXPosition = PreviousXPosition
                                    + SpeedTranslation( Adata, Speed )
                                    * MDP->MovementSpeed;
            }

            Speed = 
                MDP->SimplePtrState.RelativeMovementY / CurrentYResolution;
            if (!Speed && MDP->SimplePtrState.RelativeMovementY) {
                Speed = 1;
                if (MDP->SimplePtrState.RelativeMovementY < 0) {
                    Speed = -1;
                }
                CurrentYPosition = PreviousYPosition
                                    + Speed * MDP->MovementSpeed;
            } else {
                CurrentYPosition = PreviousYPosition
                                    + SpeedTranslation( Adata, Speed )
                                    * MDP->MovementSpeed;
            }

            PositionChanged = TRUE;
        }
    }
    // Calculate the actual wheel movement
    if ( MDP->SimplePtrState.RelativeMovementZ ) {
        if (MDP->MouseCursorUpdate) {
        CurrentZPosition = ((MDP->SimplePtrState.RelativeMovementZ) 
                        / (INT32)MDP->SimpleMode->ResolutionZ);
        }
    } else {
        CurrentZPosition = 0;
    }

    if (PositionChanged) {

        if (MDP->MousePointerPresent && MDP->MouseCursorUpdate) {
            if (MDP->MousePointerProperty == MOUSE_POINTER_PROPERTY_NORMAL) {
                if (!MDP->GOPUpdated) {
                //
                //Write Screen Data Back
                //
                Status = MDP->Gop->Blt(MDP->Gop, \
                                 BltBuffer, EfiUgaBltBufferToVideo, \
                                 0, 0, PreviousXPosition, PreviousYPosition, \
                                 MDP->CursorWidth, MDP->CursorHeight, (UINTN)NULL);
                if (EFI_ERROR(Status)) {
                    return ;
                }
                pBS->SetMem(BltBuffer, 
                    sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * MDP->CursorWidth * MDP->CursorHeight,
                    0);
                }
            pBS->SetMem(MouseBltBuffer,
                     sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * MDP->CursorWidth * MDP->CursorHeight,
                    0);
            }
        }

        //
        //For Restricting the Mouse Pointer to a Defined Area
        //
        if (CurrentXPosition <= MouseStartRow)
            CurrentXPosition = MouseStartRow;

        if (CurrentXPosition >= (INT32)MouseEndRow)
            CurrentXPosition = MouseEndRow - 1; // For 800 resolution, 0-799 pixels can be drawn using Blt

        if (CurrentXPosition > (INT32)(MouseEndRow - MDP->PreviousCursorWidth)) {
            //
            // Reduce the cursor width towards horizontal end of the screen
            //
            MDP->CursorWidth = MDP->PreviousCursorWidth - 
                              (CurrentXPosition - (MouseEndRow - MDP->PreviousCursorWidth));
        } else {
            MDP->CursorWidth = MDP->PreviousCursorWidth;
        }

        if (CurrentYPosition <= MouseStartColoum)
            CurrentYPosition = MouseStartColoum;

        if (CurrentYPosition >= (INT32)MouseEndColoum)
            CurrentYPosition = MouseEndColoum - 1; // For 600 resolution, 0-599 pixels can be drawn using Blt

        if (CurrentYPosition > (INT32)(MouseEndColoum - MDP->PreviousCursorHeight)){
            //
            // Reduce the cursor height towards verical end of the screen
            //      
            MDP->CursorHeight = MDP->PreviousCursorHeight - 
                                (CurrentYPosition - (MouseEndColoum - MDP->PreviousCursorHeight));
        } else {
            MDP->CursorHeight = MDP->PreviousCursorHeight;
        }

        PreviousXPosition = CurrentXPosition;
        PreviousYPosition = CurrentYPosition;

        if (MDP->MousePointerPresent && MDP->MouseCursorUpdate) {
                
            if (MDP->MousePointerProperty == MOUSE_POINTER_PROPERTY_NORMAL) {
                 //
                //Read Screen Data
                //
                Status = MDP->Gop->Blt(MDP->Gop,
                                       BltBuffer, EfiUgaVideoToBltBuffer,
                                       CurrentXPosition, CurrentYPosition, 0, 0,
                                       MDP->CursorWidth,
                                       MDP->CursorHeight, (UINTN)NULL);
                if (EFI_ERROR(Status)) {
                    ASSERT_EFI_ERROR(Status);
                    return ;
                }
                //First Read the Background
                Status = MDP->Gop->Blt(MDP->Gop,
                                       MouseBltBuffer, EfiUgaVideoToBltBuffer,
                                       CurrentXPosition, CurrentYPosition, 0, 0,
                                       MDP->CursorWidth,
                                       MDP->CursorHeight, (UINTN)NULL);
                if (EFI_ERROR(Status)) {
                    ASSERT_EFI_ERROR(Status);
                    return ;
                }
                //Draw the Mouse Pointer
                for (i=0; i<MDP->CursorHeight; i++) {
                    for (j=0; j<MDP->CursorWidth; j++){
                        if ((MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Red != MDP->TransparentPixel.Red) &&
                            (MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Green != MDP->TransparentPixel.Green) &&
                            (MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Blue != MDP->TransparentPixel.Blue)) {

                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Red   =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Red;
                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Green =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Green;
                            MouseBltBuffer[(i*MDP->CursorWidth)+j].Blue  =  MousePointerBuffer[(i*MDP->PreviousCursorWidth)+j].Blue;
                        }
                    }
                }
     
                Status = MDP->Gop->Blt(MDP->Gop,
                                       MouseBltBuffer, EfiUgaBltBufferToVideo,
                                       0, 0, CurrentXPosition, CurrentYPosition,
                                       MDP->CursorWidth,
                                       MDP->CursorHeight, (UINTN)NULL);
                if (EFI_ERROR(Status)) {
                    ASSERT_EFI_ERROR(Status);
                    return ;
                }
            }
        }
    }

    return;
}

/**
    Resets both Simple Pointer and Absolute Pointer Devices 

    @param  MDP - Mouse Driver Private structure Pointer

    @retval 
        EFI_STATUS

**/   

EFI_STATUS 
ResetPointingDevices (    
    IN  MOUSE_DRIVER_PRIVATE *MDP
)
{
    EFI_STATUS    Status;
    ABSOLUTE_POINTER *AbsPointerNode = 
                      OUTTER(AbsPointerList.pHead, Link, ABSOLUTE_POINTER);
    SIMPLE_POINTER *SimplePointerNode = 
                    OUTTER(SimplePointerList.pHead, Link, SIMPLE_POINTER);

    // we need to loop through all the registered Absolute Pointer Protocol 
    // and call each of their GetState function
    while ( AbsPointerNode != NULL) {
        Status = AbsPointerNode->AbsPointer->Reset(
                                 AbsPointerNode->AbsPointer, TRUE);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        AbsPointerNode = OUTTER(AbsPointerNode->Link.pNext, Link,
                                 ABSOLUTE_POINTER);
    }

    //
    // we need to loop through all the registered Simple Pointer Protocol 
    // and call each of their GetState function
    //
    while ( SimplePointerNode != NULL) {
        Status = SimplePointerNode->SimplePointer->Reset(
                                    SimplePointerNode->SimplePointer, TRUE);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        SimplePointerNode = OUTTER(SimplePointerNode->Link.pNext, Link,
                                     SIMPLE_POINTER);
    }

    return EFI_SUCCESS;
}

/**
    Gets the Absolute device Data.

    @param  MDP -  Mouse Driver Private structure Pointer

    @retval 
        EFI_STATUS

**/   

EFI_STATUS 
GetAbsDeviceData (    
    IN  MOUSE_DRIVER_PRIVATE *MDP
)
{
    EFI_STATUS    Status = EFI_NOT_FOUND;
    ABSOLUTE_POINTER *AbsPointerNode =
                     OUTTER(AbsPointerList.pHead, Link, ABSOLUTE_POINTER);

    if (AbsPointerNode == NULL) {
        return EFI_NOT_FOUND;
    }

    // we need to loop through all the registered Absolute Pointer Protocol 
    // and call each of their GetState function
    while ( AbsPointerNode != NULL) {
        Status = AbsPointerNode->AbsPointer->GetState(
                                 AbsPointerNode->AbsPointer,&MDP->AbsState);
        if (!EFI_ERROR(Status)) {
            MDP->AbsMode = AbsPointerNode->AbsPointer->Mode;
            return Status;
        }

        AbsPointerNode = OUTTER(AbsPointerNode->Link.pNext, Link,
                         ABSOLUTE_POINTER);
    }

    return EFI_NOT_FOUND;
}

/**
    Get the Simple Pointer device Data 

    @param  MDP -  Mouse Driver Private structure Pointer

    @retval 
        EFI_STATUS

**/   

EFI_STATUS 
GetSimplePointerDeviceData (    
    IN  MOUSE_DRIVER_PRIVATE *MDP
)
{
    EFI_STATUS    Status = EFI_NOT_FOUND;
    SIMPLE_POINTER *SimplePointerNode = OUTTER(SimplePointerList.pHead,
                                        Link, SIMPLE_POINTER);

    if (SimplePointerNode == NULL) {
        return EFI_NOT_FOUND;
    }

    //
    // we need to loop through all the registered Simple Pointer Protocol 
    // and call each of their GetState function
    //
    while ( SimplePointerNode != NULL) {
        Status = SimplePointerNode->SimplePointer->GetState( \
                 SimplePointerNode->SimplePointer, &MDP->SimplePtrState);
        if (!EFI_ERROR(Status)) {
            MDP->SimpleMode = SimplePointerNode->SimplePointer->Mode;
            return Status;
        }

        SimplePointerNode = OUTTER(SimplePointerNode->Link.pNext, Link,
                             SIMPLE_POINTER);
    }

    return EFI_NOT_FOUND;
}
 
/**
    This function reads the data from SimplePointer and AbsPointer 
    devices. Monitors the second left Click when left button is 
    pressed once

    @param  MDP -  Mouse Driver Private structure Pointer

          
    @retval VOID

**/   

VOID 
GetMouseClick (    
    IN  MOUSE_DRIVER_PRIVATE *MDP
)
{
    static BOOLEAN      FirstTimeEntry = TRUE;
    static BOOLEAN      RightPressed = FALSE;
    static BOOLEAN      RightReleased = TRUE;
    static BOOLEAN      LeftPressed = FALSE;
    static BOOLEAN      LeftReleased = TRUE;
    static BOOLEAN      AbsPressed = FALSE;
    static BOOLEAN      AbsReleased = TRUE;
    static BOOLEAN      Dclick_Pressed = FALSE;
    static BOOLEAN      CheckAbsPress = FALSE;
    EFI_STATUS          Status;
    static BOOLEAN      AbsFirstTimeEntry = TRUE;
    UINT32              xDivident;
    UINT32              yDivident;
    MOUSE_ACCELERATION_DATA         *Adata = &MDP->AccelerationData;
    
    //
    // Check GOP instance has correct address
    //
    if (!gGOPLocated) {
        return;
    }
    
    if (MDP->LeftPressCount != 0) {
        return;
    }
 
    if (MDP->RightPressCount != 0) { 
        return;
    }

    if (MDP->MidPressCount != 0) { 
        return;
    }

    if (MDP->AbsMouseClickCount != 0) { 
        return;
    }

    //
    //Handle the Absolute Device Movement First
    //
    pBS->SetMem(&MDP->AbsState, sizeof(EFI_ABSOLUTE_POINTER_STATE),0);

    Status = GetAbsDeviceData(MDP);

#if HIDE_POINTER_FOR_TOUCH
    // Hide mouse cursor for ABS device
    if (EFI_SUCCESS == Status){
        UpdateMousePointerProperty ( (DXE_MOUSE_PROTOCOL*) MDP, MOUSE_POINTER_PROPERTY_HIDE);
        MDP->PointerHideForAbs = TRUE;
    }
#endif

#if BITLOCKER_SOFTKBD_SUPPORT
    if ( EFI_SUCCESS == Status ) {
        if ( FALSE == MDP->MouseCursorUpdate ){
            if (MDP->SoftKbd == NULL) {
                Status = pBS->LocateProtocol(&gEfiSoftKbdProtocolGuid, NULL, (VOID**) &MDP->SoftKbd);
                if (EFI_ERROR(Status)) {
                    ASSERT_EFI_ERROR(Status);
                    return ;
                }
            }
            if (MDP->SoftKbd->BitLockerSoftKbdCalled == TRUE) {
                RefreshMouseCursor( (DXE_MOUSE_PROTOCOL*)MDP );
            }
        }
    }
#endif
    if (!EFI_ERROR(Status)) {
#if !MOUSE_SUPPORT_FOR_NT32
	    MDP->ValidPressOrRelease = TRUE;
#endif
        //
        // Convert Absolute device movement in terms of actual screen.
        //
        xDivident=(((UINT32)MDP->AbsMode->AbsoluteMaxX -
                    (UINT32)MDP->AbsMode->AbsoluteMinX)
                    * FRACTIONAL_PART_CONVERSION ) /
                    MDP->Gop->Mode->Info->HorizontalResolution;

        MDP->AbsState.CurrentX=(((UINT32)MDP->AbsState.CurrentX -
                                 (UINT32)MDP->AbsMode->AbsoluteMinX)                                                        
                                 * FRACTIONAL_PART_CONVERSION ) /
                                 ((UINT32)xDivident);

        yDivident=(((UINT32)MDP->AbsMode->AbsoluteMaxY -
                    (UINT32)MDP->AbsMode->AbsoluteMinY)
                    * FRACTIONAL_PART_CONVERSION)/
                    MDP->Gop->Mode->Info->VerticalResolution;

        MDP->AbsState.CurrentY=(((UINT32)MDP->AbsState.CurrentY -
                                 (UINT32)MDP->AbsMode->AbsoluteMinY)
                                 * FRACTIONAL_PART_CONVERSION)/
                                 ((UINT32)yDivident);


#if ABS_CLICK_MODE == 1
        if (MDP->AbsState.ActiveButtons & EFI_ABSP_TouchActive) {
            MDP->AbsMouseClickCount = 1;
            CheckAbsPress = TRUE; 
            // CheckAbsPress variable will ensure that click is consumed only when there is release after press, reported by touch driver
            // Click will not be consumed when only press or only release is reported by touch driver.
        } else {
            if ((( AbsReleased==FALSE && AbsPressed==TRUE) ||
                //
                // DCLICK not Reported. Consider next click/touch release as new click.
                //
                 !MDP->ReportedAbsDClick ) && CheckAbsPress) {
                MDP->AbsMouseClickCount = 1;
                MDP->AbsMouseRelCount = 1;
                MDP->AbsXRelPosition = (INT32)MDP->AbsState.CurrentX;
                MDP->AbsYRelPosition = (INT32)MDP->AbsState.CurrentY;
                AbsReleased = TRUE;
                AbsPressed = FALSE;
                CheckAbsPress = FALSE;
            } 
#if !MOUSE_SUPPORT_FOR_NT32
            else {
                MDP->ValidPressOrRelease = FALSE;                   // EIP 168048 If sequence of press or release is broken
            }
#endif
        }
#endif
#if ABS_CLICK_MODE == 2
        if (MDP->AbsState.ActiveButtons & EFI_ABS_AltActive) {
            MDP->AbsMouseClickCount = 1;  
            MDP->AbsXPosition = (INT32)MDP->AbsState.CurrentX;
            MDP->AbsYPosition = (INT32)MDP->AbsState.CurrentY;  
            CheckAbsPress = TRUE;
            // CheckAbsPress variable will ensure that click is consumed only when there is release after press, reported by touch driver
            // Click will not be consumed when only press or only release is reported by touch driver.
        } else {
            if ((( AbsReleased==FALSE && AbsPressed==TRUE) ||
                //
                // DCLICK not Reported. Consider next click/touch release as new click.
                //
                 !MDP->ReportedAbsDClick ) && CheckAbsPress) {
                MDP->AbsMouseClickCount=1;
                MDP->AbsMouseRelCount = 1;
                MDP->AbsXRelPosition = (INT32)MDP->AbsState.CurrentX;
                MDP->AbsYRelPosition = (INT32)MDP->AbsState.CurrentY;
                AbsReleased = TRUE;
                AbsPressed = FALSE;
                CheckAbsPress = FALSE;
            }
#if !MOUSE_SUPPORT_FOR_NT32
            else {
                MDP->ValidPressOrRelease = FALSE;                    // EIP 168048 If sequence of press or release is broken
            }
#endif
        }
#endif

#if ABS_CLICK_MODE == 1
        if (MDP->AbsState.ActiveButtons & EFI_ABSP_TouchActive){
#else
        if (MDP->AbsState.ActiveButtons & EFI_ABS_AltActive) {
#endif
            if (AbsReleased == TRUE && AbsPressed == FALSE) {
                AbsReleased = FALSE;
                AbsPressed = TRUE;
                if (MDP->DoubleClick == FALSE) {
                    Status = pBS->SetTimer(MDP->LeftClickEvent, TimerRelative, TIMER_ONE_SECOND);
                    MDP->DoubleClick = TRUE;
                    AbsFirstTimeEntry = TRUE; 
                }

                if (AbsFirstTimeEntry == FALSE) {
                    if (MDP->DoubleClick) {
                        MDP->DoubleClick = FALSE;
                        Dclick_Pressed = TRUE;
                    }
                }

                AbsFirstTimeEntry=FALSE;

                if (Dclick_Pressed == TRUE) {
                    MDP->AbsMouseClickCount++;
//                  MDP->AbsMouseRelCount = 1;
                    MDP->AbsXRelPosition = (INT32)MDP->AbsState.CurrentX;
                    MDP->AbsYRelPosition = (INT32)MDP->AbsState.CurrentY;
                    Dclick_Pressed = FALSE;
                    AbsFirstTimeEntry = TRUE; 
                    AbsReleased = TRUE;
                    AbsPressed = FALSE;
                }  
                MDP->AbsXPosition = (INT32)MDP->AbsState.CurrentX;
                MDP->AbsYPosition = (INT32)MDP->AbsState.CurrentY;
            }
        }
        GetMouseMovement(MDP);
        return;
    } else {   

        //
        //Handle the Simple pointer
        //
        pBS->SetMem(&MDP->SimplePtrState, sizeof(EFI_SIMPLE_POINTER_STATE), 0);
    
        Status = GetSimplePointerDeviceData(MDP);

#if HIDE_POINTER_FOR_TOUCH
        // Display cursor for Mouse/Touchpad device
        if (EFI_SUCCESS == Status){
            MDP->PointerHideForAbs = FALSE;
            UpdateMousePointerProperty ( (DXE_MOUSE_PROTOCOL*) MDP, MOUSE_POINTER_PROPERTY_NORMAL);
        }
#endif

#if BITLOCKER_SOFTKBD_SUPPORT
        if ( EFI_SUCCESS == Status ) {
            if ( FALSE == MDP->MouseCursorUpdate ){
                if (MDP->SoftKbd == NULL) {
                    Status = pBS->LocateProtocol(&gEfiSoftKbdProtocolGuid, NULL, (VOID**) &MDP->SoftKbd);
                    if (EFI_ERROR(Status)) {
                        ASSERT_EFI_ERROR(Status);
                        return ;
                    }
                }
                if (MDP->SoftKbd->BitLockerSoftKbdCalled == TRUE) {
                    RefreshMouseCursor( (DXE_MOUSE_PROTOCOL*)MDP );
                }
            }
        }
#endif        
        if (EFI_ERROR(Status)) {
            return;
        }

        //
        // Multiply Relativemovement by ten for not loosing the count less than the resolution
        // and get the actual movement.
        //
        Adata->EndX += ((MDP->SimplePtrState.RelativeMovementX * 10) 
                            / (INT32)MDP->SimpleMode->ResolutionX);

        Adata->EndY += ((MDP->SimplePtrState.RelativeMovementY * 10) 
                            / (INT32)MDP->SimpleMode->ResolutionY);

        CurrentXResolution = (INT32)MDP->SimpleMode->ResolutionX;
        CurrentYResolution = (INT32)MDP->SimpleMode->ResolutionY;
        CurrentZResolution = (INT32)MDP->SimpleMode->ResolutionZ;

        if (++(Adata->CurrentFrame) == MOUSE_SPEED_FRAME_RATE) {
            Adata->EndX /= 10;
            Adata->EndY /= 10;
            CalculateRealSpeed( Adata );
        }

        if (RightReleased == TRUE && RightPressed == FALSE) {
            if (!EFI_ERROR(Status)) {
                if (MDP->SimplePtrState.RightButton == TRUE) {
                    MDP->RightPressCount = 1;
                    RightReleased = FALSE;
                    RightPressed = TRUE;
                    MDP->RightXPosition = CurrentXPosition;
                    MDP->RightYPosition = CurrentYPosition;
                }
            }
        }

        if (RightReleased == FALSE && RightPressed == TRUE) {
            MDP->RightPressCount = 1;
            if (!EFI_ERROR(Status)) {
                if (MDP->SimplePtrState.RightButton == 0) {
                    MDP->RightXRelPosition = CurrentXPosition;
                    MDP->RightYRelPosition = CurrentYPosition;
                    MDP->RightReleaseCount = 1;
                    RightReleased = TRUE; 
                    RightPressed = FALSE;
                }
            }
        }


/*            if (MidReleased == TRUE && MidPressed == FALSE) {
                if (!EFI_ERROR(Status)) {
                    if (MDP->SimplePtrState.MiddleButton == TRUE) {
                        MDP->MidPressCount =1;
                        MidReleased = FALSE;
                        MidPressed = TRUE;
                        MDP->MidXPosition = CurrentXPosition;
                        MDP->MidYPosition = CurrentYPosition;
                    }
                }
            }

            if (MidReleased == FALSE && MidPressed == TRUE) {
                MDP->MidPressCount = 1;
                if (!EFI_ERROR(Status)) {
                    if (MDP->SimplePtrState.MiddleButton == 0) {
                        MDP->MidXRelPosition = CurrentXPosition;
                        MDP->MidYRelPosition = CurrentYPosition;
                        MDP->MidReleaseCount = 1;
                        MidReleased = TRUE; 
                        MidPressed = FALSE;
                    }
                }
            }
*/
        if (LeftPressed == FALSE && LeftReleased == TRUE) {
            if (!EFI_ERROR(Status)) {
                if (MDP->SimplePtrState.LeftButton == TRUE) {
                    if (MDP->DoubleClick == FALSE) {
                        Status = pBS->SetTimer(MDP->LeftClickEvent, TimerRelative, TIMER_ONE_SECOND);
                        MDP->DoubleClick = TRUE;
                        FirstTimeEntry = TRUE; 
                    }
                    
                    if(FirstTimeEntry==FALSE) {
                        if (MDP->DoubleClick) {
                           MDP->DoubleClick = FALSE;
                           Dclick_Pressed=TRUE;
                        }
                    }
                    LeftPressed=TRUE;  
                    LeftReleased=FALSE;
                    MDP->LeftPressCount=1;
                    FirstTimeEntry=FALSE;

                    if (Dclick_Pressed==TRUE) {
                        // Checking if The Second Click also in Same Position as First Click Incase of Double Click. 
                        if ((MDP->LeftXPosition >= (CurrentXPosition - 2)) && (MDP->LeftXPosition <= (CurrentXPosition + 2)) &&
                           (MDP->LeftYPosition >= (CurrentYPosition - 2)) && (MDP->LeftYPosition <= (CurrentYPosition + 2)) ) {
                            Dclick_Pressed=FALSE;
                            FirstTimeEntry=TRUE; 
                        }  else {
                            Status = pBS->SetTimer(MDP->LeftClickEvent, TimerRelative, TIMER_ONE_SECOND);
                            MDP->DoubleClick = TRUE; 
                            FirstTimeEntry = FALSE;
                        }
                    }
                    // Updating the Position where Button is Pressed.
                    MDP->LeftXPosition = CurrentXPosition;
                    MDP->LeftYPosition = CurrentYPosition;
                }
            }
        }
        if (LeftPressed == TRUE && LeftReleased == FALSE) {
            if (FirstTimeEntry == FALSE) {
                MDP->LeftPressCount = 1;
            } else {
                MDP->LeftPressCount = 2;
            }
            if (!EFI_ERROR(Status)) {
                // Check if Button is Released. If released, update the ReleasePosition.
                if (MDP->SimplePtrState.LeftButton == 0) {
                    MDP->LeftXRelPosition = CurrentXPosition;
                    MDP->LeftYRelPosition = CurrentYPosition;
                    MDP->LeftReleaseCount = 1;
                    LeftReleased = TRUE; 
                    LeftPressed = FALSE;
               }
           }
       }
    }
    return;
}

/**
    This function clears the timer set to identify the double click. This 
    timer is set when the first left click is identified and it is cleared 
    after one second through this callback.

    @param  Event 
    @param  Context 
    @param  MDP -  Mouse Driver Private structure Pointer

          
    @retval VOID

**/   

VOID 
LeftClickCallBack (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
)
{
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE*)Context;
    EFI_STATUS Status;

    MDP->DoubleClick = FALSE;
    Status = pBS->SetTimer(MDP->LeftClickEvent, TimerCancel, 0);
    ASSERT_EFI_ERROR(Status);
    return;

}

/**
    Internal function that installs/uninstall protocol with a specified GUID 
    and NULL interface. Such protocols can be used as event signaling mechanism.

    @param  ProtocolGuid - Pointer to the protocol GUID

    @retval VOID

**/

VOID 
SignalProtocolEvent (
    IN  EFI_GUID *ProtocolGuid
)
{
    EFI_HANDLE  Handle = NULL;
    pBS->InstallProtocolInterface (
                            &Handle, ProtocolGuid, EFI_NATIVE_INTERFACE, NULL
                            );
    pBS->UninstallProtocolInterface (
                            Handle, ProtocolGuid, NULL
                            );
    return;
}

/**
    This function returns the integer square root of a 32 bit unsigned 
    number using a simple estimation method.

    @param  Number - the number for which the square root is to be 
            determined (the radicand)

    @retval UINT32 The square root

**/

UINT32 
Sqrt32 (
    IN  UINT32 Number
)  
{  
    UINT32 c = 0x8000;  
    UINT32 g = 0x8000;  

    for (;;) {  
        if ( g*g > Number )
            g ^= c;
        c >>= 1;
        if ( c == 0 )
            return g;
        g |= c;
    }
}

/**
    This function computes the distance between two points using the
    geometric distance formula.

    @param 
        IN INT32 X1 - starting x coordinate
        IN INT32 Y1 - starting y coordinate
        IN INT32 X2 - ending x coordinate
        IN INT32 Y2 - ending y coordinate

         
    @retval UINT32 distance between the points

    @note  
      Distance = ( (X2 - X1)^2 + (Y2 - Y1)^2 )^(1/2)  
**/

UINT32 
PointDistance (
    IN INT32 X1,
    IN INT32 Y1,
    IN INT32 X2,
    IN INT32 Y2
)
{  
    return Sqrt32((UINT32)( ((X2 - X1) * (X2 - X1)) + 
                            ((Y2 - Y1) * (Y2 - Y1)) ));
}

/**
    This function calculates the speed of the cursor based upon the
    distance traveled over a specified number of frames (assuming a 
    constant frame-rate).

    @param  Adata - pointer to the acceleration data structure

          
    @retval VOID

**/

VOID 
CalculateRealSpeed (
    IN OUT MOUSE_ACCELERATION_DATA   *Adata
)
{
    UINT32 Distance = PointDistance( Adata->StartX, 
                                     Adata->StartY, 
                                     Adata->EndX, 
                                     Adata->EndY );
    
    // calculate speed (distance per frame)
    Adata->RealSpeed = Distance / MOUSE_SPEED_FRAME_RATE;

    // current location becomes start location and reset frame count
    Adata->StartX = Adata->EndX;
    Adata->StartY = Adata->EndY;
    Adata->CurrentFrame = 0;
}

/**
    This function multiplies a signed integer times an unsigned integer
    where each integer represents a decimal number with hundredth 
    position precision.

           
    @param  Value first term in product is a signed integer
    @param  Multiplier - second term in product is an unsigned integer

         
    @retval INT32 integer product

    @note  
      To multiply 2.35 times 2 the function call will be 
      FixedMultiplyHundredths(235, 200).
**/

INT32 
FixedMultiplyHundredths (
    IN INT32   Value, 
    IN UINT32  Multiplier )
{
    return ((Value * 100) * (INT32)Multiplier) / 10000;
}

/**
    This function returns a mouse position offset that is either reduced 
    or increased depending on the current mouse speed and the 
    cooresponding translation table multiplier.

    @param  Adata acceleration data structure
    @param  RelativePosition current mouse relative offset position 
            as reported by the hardware

    @retval INT32 the translated offset

**/

INT32 
SpeedTranslation (
    IN  MOUSE_ACCELERATION_DATA  *Adata,
    IN  INT32                    RelativePosition
)
{
    UINT32 i;

    for (i = 0; i < Adata->TableSize; i++) {
        if ((Adata->RealSpeed >= Adata->TranslationTable[i].MinSpeed) &&
            (Adata->RealSpeed <= Adata->TranslationTable[i].MaxSpeed)) {
            break;
        }
    }
    
    if (i < Adata->TableSize)
        return FixedMultiplyHundredths( RelativePosition, 
                            Adata->TranslationTable[i].TranslationMultiplier);
    else
        return RelativePosition;
        
}

/**
    Notification Function which Locates the GOP Protocol whenever it is
    Installed/Reinstalled.

         
    @param  Event 
    @param  Context 

         
    @retval VOID

**/

VOID
LocateGopProtocol (
    IN  EFI_EVENT   Event, 
    IN  VOID        *Context
)
{
    EFI_STATUS               Status;
    MOUSE_DRIVER_PRIVATE    *MDP = (MOUSE_DRIVER_PRIVATE *)Context;

    if (Context==NULL) {
        return;
    }
    Status = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,
                                 NULL, (VOID**) &MDP->Gop);
 
    //
    // Update var "gGOPLocated" for GOP Reinstallation
    //
    gGOPLocated = TRUE;
    if (EFI_ERROR(Status)) {
        gGOPLocated = FALSE;
        MDP->Gop = NULL;
    }

    return;
}
 
/**
    Installs  EFI_MOUSE_DRIVER_PROTOCOL_GUID.

    @param  Controller controller handle

    @retval VOID
              
**/

EFI_STATUS
InstallMouseProtocol (
    IN  EFI_HANDLE   Controller
)
{
    EFI_STATUS               Status;
    static BOOLEAN           ProtocolInstalled = FALSE; 

    if (ProtocolInstalled) {
        return EFI_SUCCESS;
    }

    Status = pBS->AllocatePool (EfiBootServicesData,
                                sizeof(MOUSE_DRIVER_PRIVATE), (VOID**)&gMDP);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    pBS->SetMem(gMDP, sizeof (MOUSE_DRIVER_PRIVATE), 0); 

    gMDP->Revision = MOUSE_PROTOCOL_REVISION;
    gMDP->MousePoll = MousePoll;
    gMDP->InitializePointingDevice = InitializePointingDevice;
    gMDP->StartUpdatingMouseCursor = StartUpdatingMouseCursor;
    gMDP->StopUpdatingMouseCursor  = StopUpdatingMouseCursor;
    gMDP->RefreshMouseCursor       = RefreshMouseCursor;
    gMDP->GetCoordinates           = GetCoordinates;
    gMDP->SetCoordinates           = SetCoordinates;
    gMDP->SetMouseSpeed            = SetMouseSpeed;
    gMDP->GetButtonStatus          = GetButtonStatus;
    gMDP->SetMouseCoordinates      = SetMouseCoordinates;
    gMDP->SetSoftKeyData           = SetSoftKeyData;
    gMDP->StopPointingDevice       = StopPointingDevice;
    gMDP->GetMouseVelocity         = GetMouseVelocity;
    gMDP->SetSpeedTranslationTable = SetSpeedTranslationTable;
    gMDP->UpdateMousePointer       = UpdateMousePointer;
    gMDP->UpdateMousePointerProperty = UpdateMousePointerProperty;
    gMDP->AccelerationData.TranslationTable = gDefaultSpeedTranslationTable;
    gMDP->AccelerationData.TableSize = gDefaultSpeedTranslationTableCount;
    gMDP->SetMouseGOP                = SetMouseGOP;
    gMDP->GetMouseCursorImage        = GetMouseCursorImage;
    //
    //Installing the Mouse Protocol    
    // 
    Status = pBS->InstallProtocolInterface(&Controller,
                                           &gEfiDXEMouseDriverProtocolGuid,
                                           EFI_NATIVE_INTERFACE,
                                           gMDP);
    if (!EFI_ERROR(Status)) {
        Status = RegisterProtocolCallback(&gEfiGraphicsOutputProtocolGuid, 
                                      LocateGopProtocol, 
                                      (VOID*)gMDP, 
                                      &gGopProtocolEvent, 
                                      &gGopProtocolEventReg);
        ProtocolInstalled = TRUE;
    }

    return Status;
}

#if SOFTKBD_ICON_SUPPORT || BITLOCKER_SOFTKBD_SUPPORT
/**
    Checks whether Click/Touch within SoftKbd Icon area.

         
    @param  MDP - Mouse Driver Private structure Pointer
    @param  IconPress 
            TRUE - when checking Click/Touch press
            FALSE - when checking Click/Touch release

         
    @retval 1 Icon Press/Release on ESC Icon
    @retval 2 Icon Press/Release on ACTIVATE Icon
    @retval 3 Icon Press/Release on DEACTIVATE Icon
    @retval 0 No Press/Release on Icon.
              
**/

UINT8 
IconClick (
    IN  MOUSE_DRIVER_PRIVATE *MDP,
    IN  BOOLEAN IconPress
)
{
    UINT32 ClickXPosition;
    UINT32 ClickYPosition;
    UINT32 TouchXPosition;
    UINT32 TouchYPosition;

    if (IconPress) {
        ClickXPosition = MDP->LeftXPosition;
        ClickYPosition = MDP->LeftYPosition;
        TouchXPosition = MDP->AbsXPosition; 
        TouchYPosition = MDP->AbsYPosition; 
    } else {
        ClickXPosition = MDP->LeftXRelPosition;
        ClickYPosition = MDP->LeftYRelPosition;
        TouchXPosition = MDP->AbsXRelPosition;
        TouchYPosition = MDP->AbsYRelPosition; 
    }
#if ESC_ICON_SUPPORT
    //
    // Check whether Left Click is on ESC Icon area
    //
    if ((ClickXPosition > MDP->SoftKeyboardData.EscKeyStartX &&
         ClickXPosition < MDP->SoftKeyboardData.EscKeyEndX) ||
         (TouchXPosition > MDP->SoftKeyboardData.EscKeyStartX &&
          TouchXPosition < MDP->SoftKeyboardData.EscKeyEndX )) {
        if ((ClickYPosition > MDP->SoftKeyboardData.EscKeyStartY && 
             ClickYPosition < MDP->SoftKeyboardData.EscKeyEndY) ||
            (TouchYPosition > MDP->SoftKeyboardData.EscKeyStartY &&
             TouchYPosition < MDP->SoftKeyboardData.EscKeyEndY)) {
             return ESC_ICON;
        }
    }
#endif
    //
    // Check whether Left Click is on SoftKbd Activate Icon area
    //
    if ((ClickXPosition > MDP->SoftKeyboardData.ActivateKeyStartX &&
         ClickXPosition < MDP->SoftKeyboardData.ActivateKeyEndX) ||
        (TouchXPosition > MDP->SoftKeyboardData.ActivateKeyStartX &&
         TouchXPosition < MDP->SoftKeyboardData.ActivateKeyEndX )) {
        if ((ClickYPosition > MDP->SoftKeyboardData.ActivateKeyStartY && 
             ClickYPosition < MDP->SoftKeyboardData.ActivateKeyEndY) ||
            (TouchYPosition > MDP->SoftKeyboardData.ActivateKeyStartY &&
             TouchYPosition < MDP->SoftKeyboardData.ActivateKeyEndY)) {
             return ACT_ICON;
        }
    }
    //
    // Check whether Left Click is on Softkbd Deactivate Icon area
    //
    if ((ClickXPosition > MDP->SoftKeyboardData.DeActivateKeyStartX &&
         ClickXPosition < MDP->SoftKeyboardData.DeActivateKeyEndX) ||
         (TouchXPosition > MDP->SoftKeyboardData.DeActivateKeyStartX &&
          TouchXPosition < MDP->SoftKeyboardData.DeActivateKeyEndX )) {
        if ((ClickYPosition > MDP->SoftKeyboardData.DeActivateKeyStartY && 
             ClickYPosition < MDP->SoftKeyboardData.DeActivateKeyEndY) ||
            (TouchYPosition > MDP->SoftKeyboardData.DeActivateKeyStartY &&
             TouchYPosition < MDP->SoftKeyboardData.DeActivateKeyEndY)) {
             return DEACT_ICON;
        }
    }

    //
    // Check whether Left Click is on Setup Icon area
    //
    if ((ClickXPosition > MDP->SoftKeyboardData.SetupKeyStartX &&
         ClickXPosition < MDP->SoftKeyboardData.SetupKeyEndX) ||
          (TouchXPosition > MDP->SoftKeyboardData.SetupKeyStartX &&
           TouchXPosition < MDP->SoftKeyboardData.SetupKeyEndX )) {
         if ((ClickYPosition > MDP->SoftKeyboardData.SetupKeyStartY && 
              ClickYPosition < MDP->SoftKeyboardData.SetupKeyEndY) ||
             (TouchYPosition > MDP->SoftKeyboardData.SetupKeyStartY &&
              TouchYPosition < MDP->SoftKeyboardData.SetupKeyEndY)) {
              return POST_ICON;
           }
       } 
    //
    // Check whether Left Click is on BBS Icon area
    //
    if ((ClickXPosition > MDP->SoftKeyboardData.BbsKeyStartX &&
         ClickXPosition < MDP->SoftKeyboardData.BbsKeyEndX) ||
          (TouchXPosition > MDP->SoftKeyboardData.BbsKeyStartX &&
           TouchXPosition < MDP->SoftKeyboardData.BbsKeyEndX )) {
         if ((ClickYPosition > MDP->SoftKeyboardData.BbsKeyStartY && 
              ClickYPosition < MDP->SoftKeyboardData.BbsKeyEndY) ||
             (TouchYPosition > MDP->SoftKeyboardData.BbsKeyStartY &&
              TouchYPosition < MDP->SoftKeyboardData.BbsKeyEndY)) {
              return POST_ICON;
           }
       }
    return  FALSE;
}
#endif

/**
    Find all the Simple Pointer Protocol handles and Absolute Pointer 
    Protocol handles and connect the handles to start the MouseDriver 

    @param  Event Event to signal
    @param  Context Event specific context

         
    @retval VOID

**/

VOID 
ConnectSimpleAbsHandles (
    IN  EFI_EVENT    Event,
    IN  VOID         *ProtocolContext
)
{
    EFI_STATUS    Status;
    EFI_HANDLE    *HandleBuffer;
    UINTN         HandleCount;
    UINT8         i;

    if ( ProtocolContext == NULL ) {
        return;
    }

    //
    // Find out the Number of simple Pointer handles
    //
    Status = pBS->LocateHandleBuffer(   ByProtocol,
                                        &gEfiSimplePointerProtocolGuid,
                                        NULL,
                                        &HandleCount,
                                        &HandleBuffer);

    if (!EFI_ERROR(Status)) {  

      for(i=0; i<HandleCount; i++) {
        //
        // Connect the SimplePointerProtocol handles that produced by Consplitter driver
          if(SimplePointEvent == Event && pST->ConsoleInHandle == HandleBuffer[i]) {
            pBS->ConnectController(HandleBuffer[i], NULL, NULL, FALSE);
          }
       }
     pBS->FreePool(HandleBuffer);
    }
    
    //
    // Find out the Number of Absolute Pointer handles
    //
    Status = pBS->LocateHandleBuffer(   ByProtocol,
                                        &gEfiAbsolutePointerProtocolGuid,
                                        NULL,
                                        &HandleCount,
                                        &HandleBuffer);
    if (EFI_ERROR(Status)) {  
        return;
    }

    for(i=0; i<HandleCount; i++) {
#if defined CORE_COMBINED_VERSION && CORE_COMBINED_VERSION > 0x5000b
        //
        // Connect the AbsolutePointerProtocols handles that produced by Consplitter driver. 
        //
        if(AbsPointEvent ==Event && pST->ConsoleInHandle == HandleBuffer[i]) {
            pBS->ConnectController(HandleBuffer[i], NULL, NULL, FALSE);
        }
#else
        // Connect all the absolutePointerProtocol handles. 
        //
        if( AbsPointEvent ==Event ) {
            pBS->ConnectController(HandleBuffer[i], NULL, NULL, FALSE);
        }
#endif     
    }
    pBS->FreePool(HandleBuffer);

    return;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
