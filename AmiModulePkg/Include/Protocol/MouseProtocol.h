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

/** @file MouseLib.h
    Mouse Driver protocol definition header file
**/

#ifndef _EFI_MOUSE_PROTOCOL_H_
#define _EFI_MOUSE_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#include <Protocol/GraphicsOutput.h>

//---------------------------------------------------------------------------

#define EFI_MOUSE_DRIVER_PROTOCOL_GUID \
{0xc7a7030c, 0xc3d8, 0x45ee, 0xbe, 0xd9, 0x5d, 0x9e, 0x76, 0x76, 0x29, 0x53}

#define EFI_SOFTKEY_ACTIVATE_PROTOCOL_GUID \
{0x1172381f, 0x7ae6, 0x4652, 0x8d, 0x85, 0xb6, 0x51, 0x69, 0x7b, 0xe3, 0xcf}

#define EFI_SOFTKEY_DEACTIVATE_PROTOCOL_GUID \
{0xd3431c06, 0x2b4c, 0x4337, 0x93, 0x34, 0xff, 0xd9, 0xc0, 0x55, 0x15, 0x21}

#define EFI_PROCESS_ESC_KEY_PROTOCOL_GUID \
{0xd78ebe23, 0x7413, 0x43b9, 0x9d, 0x94, 0x44, 0x21, 0x6f, 0x4, 0xbb, 0x51}

#define EFI_PROCESS_POST_KEY_PROTOCOL_GUID \
{0x8a6e4fcf, 0x6e69, 0x4929,0xa6,0x29, 0xa2, 0xca, 0xd6, 0xc5, 0xfc, 0x56}

//For CLICK Event
#define     TSEMOUSE_NULL_CLICK         0
#define     TSEMOUSE_RIGHT_CLICK        1
#define     TSEMOUSE_LEFT_CLICK         2
#define     TSEMOUSE_MIDDLE_CLICK       3
#define     TSEMOUSE_LEFT_DCLICK        4
#define     TSEMOUSE_RIGHT_DOWN         5
#define     TSEMOUSE_LEFT_DOWN          6
#define     TSEMOUSE_MIDDLE_DOWN        7
#define     TSEMOUSE_RIGHT_UP           8
#define     TSEMOUSE_LEFT_UP            9
#define     TSEMOUSE_MIDDLE_UP         10

typedef struct _DXE_MOUSE_PROTOCOL DXE_MOUSE_PROTOCOL;

typedef enum _MOUSE_POINTER_PROPERTY {
    MOUSE_POINTER_PROPERTY_NORMAL = 0,	// Show Mouse pointer and report actions 
    MOUSE_POINTER_PROPERTY_FREEZE,	// freeze the Mouse pointer and don't update the movement and update the actions.		
    MOUSE_POINTER_PROPERTY_HIDE  	// Hide the Mouse pointer but report the movement and actions
} MOUSE_POINTER_PROPERTY;


typedef struct {
    UINT32 ActivateKeyStartX;
    UINT32 ActivateKeyEndX;
    UINT32 ActivateKeyStartY;
    UINT32 ActivateKeyEndY;
    UINT32 DeActivateKeyStartX;
    UINT32 DeActivateKeyEndX;
    UINT32 DeActivateKeyStartY;
    UINT32 DeActivateKeyEndY;
    UINT32 EscKeyStartX;
    UINT32 EscKeyEndX;
    UINT32 EscKeyStartY;
    UINT32 EscKeyEndY;
    UINT32 SetupKeyStartX;
    UINT32 SetupKeyEndX;
    UINT32 SetupKeyStartY;
    UINT32 SetupKeyEndY;
    UINT32 BbsKeyStartX;
    UINT32 BbsKeyEndX;
    UINT32 BbsKeyStartY;
    UINT32 BbsKeyEndY;
}SOFT_KEY_DATA;

typedef struct _MOUSE_TRANSLATION_TABLE {
    UINT32  MinSpeed;
    UINT32  MaxSpeed;
    UINT32  TranslationMultiplier;
} MOUSE_TRANSLATION_TABLE;

/**
    Updates Mouse Pointer Property value 

    @param  This Pointer to the mouse protocol.
    @param  Property MousePointer property
    
    @retval  EFI_STATUS

**/

typedef EFI_STATUS (*EFI_FUNCTION_UPDATE_MOUSE_POINTER_PROPERTY)(
    IN  DXE_MOUSE_PROTOCOL      *This,
    IN  MOUSE_POINTER_PROPERTY  Property
);

/**
    Polling function to get mouse movement and action

    @param  This - Mouse Protocol Pointer

    @retval EFI_SUCCESS

**/

typedef EFI_STATUS (*EFI_FUNCTION_POLL)(
    IN  DXE_MOUSE_PROTOCOL	*This
);

/**
    Initializes mouse movement related events and restrict its movement to a 
    defined area.

    @param  This - Mouse Protocol Pointer
    @param  StartX - Min Row for the mouse movement
    @param  StartY - Min Coloum for the mouse movement 
    @param  EndX - Maximum Row for the mouse movement
    @param  EndY - Maximum Coloum for the mouse movement

          
    @retval EFI_SUCCESS Initialized mouse movement successfully

**/

typedef EFI_STATUS (*EFI_FUNCTION_INITIALIZE)(
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  INT32               StartX,
    IN  INT32               StartY,
    IN  INT32               EndX,
    IN  INT32               EndY
);

/**
    Starts updating the mouse pointer movement.

    @param  This - Mouse Protocol Pointer

    @retval EFI_SUCCESS
 
**/

typedef EFI_STATUS (*EFI_FUNCTION_START_UPDATING_MOUSE_CURSOR)(
    IN  DXE_MOUSE_PROTOCOL  *This
);

/**
    Stops updating the mouse pointer movement and removes pointer from the screen.

    @param  This - Mouse Protocol Pointer

    @retval EFI_SUCCESS
 
**/

typedef EFI_STATUS (*EFI_FUNCTION_STOP_UPDATING_MOUSE_CURSOR)(
    IN  DXE_MOUSE_PROTOCOL  *This
);

/**
    Redraws the mouse pointer on the screen if it is corrupted.

    @param  This - Mouse Protocol Pointer

    @retval EFI_SUCCESS
 
**/

typedef EFI_STATUS (*EFI_FUNCTION_REFRESH_MOUSE_CURSOR)(
    IN  DXE_MOUSE_PROTOCOL  *This
);

/**
    Returns the current mouse position.

    @param  This - Mouse Protocol Pointer
    @param  PointerX - current X-axis position.
    @param  PointerY - current Y-axis position.
    @param  PointerZ - current Z-axis position.

    @retval EFI_SUCCESS

**/

typedef EFI_STATUS (*EFI_FUNCTION_GETPOSITION)(
    IN  DXE_MOUSE_PROTOCOL   *This,
    OUT INT32                *PointerX,
    OUT INT32                *PointerY,
    OUT INT32                *PointerZ
);

/**
    Sets the mouse pointer position.

    @param  This - Mouse Protocol Pointer
    @param  PointerX - X-axis position
    @param  PointerY - Y-axis position

          
    @retval EFI_SUCCESS Coordinate values updated successfully
    @retval EFI_INVALID_PARAMETER Invalid input values

**/

typedef EFI_STATUS (*EFI_FUNCTION_SETPOSITION)(
    IN  DXE_MOUSE_PROTOCOL	*This,
    IN  INT32                PointerX,
    IN  INT32                PointerY
);

/**
    Sets the Speed of mouse movement

    @param  This - Mouse Protocol Pointer
    @param  Value - Speed of the move movement

          
    @retval EFI_SUCCESS driver stopped successfully
    @retval EFI_UNSUPPORTED Invalid input values.
 
**/

typedef EFI_STATUS (*EFI_FUNCTION_SETSPEED)(
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  UINT32              Value
);

/**
    Returns the mouse button status.

    @param  This Mouse Protocol Pointer
    @param  ButtonStatus Device Button status

    @retval EFI_STATUS 

    @note  
      If there is no mouse click then
      *ButtonStatus = TSEMOUSE_NULL_CLICK

**/

typedef EFI_STATUS (*EFI_FUNCTION_GETBUTTONSTATUS)(
    IN  DXE_MOUSE_PROTOCOL  *This,
    OUT INT32               *Button_Status
);

/**
    Sets and restricts the mouse movement viewpoint.

    @param  This - Mouse Protocol Pointer
    @param  MouseViewStartRow - Start row position
    @param  MouseViewStartColoum - Start coloum position
    @param  MouseViewEndRow - End row position
    @param  MouseViewEndColoum - End coloum position

         
    @retval EFI_SUCCESS driver stopped successfully
 
**/

typedef EFI_STATUS (EFIAPI *EFI_FUNCTION_SETVIEWPOINT)(
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  INT32   MouseViewStartRow,
    IN  INT32   MouseViewStartColoum,
    IN  INT32   MouseViewEndRow,
    IN  INT32   MouseViewEndColoum
);

/**
    Sets Softkeyboard Activate, Deactivate Button and ESC Icon co-ordinates. 
    So Mouse driver can inform others when there is click on Activate and 
    Deactivate button.

    @param  This Mouse Protocol Pointer
    @param  SoftKeyData Structure that contains SoftKbd related
            button Co-ordinates.

    @retval EFI_SUCCESS
 
**/

typedef EFI_STATUS (*EFI_FUNCTION_SOFTKEY)(
    IN  DXE_MOUSE_PROTOCOL  *This,
    SOFT_KEY_DATA           *SoftKeyData
); 

/**
    Stops updating the mouse pointer movement and removes pointer from the screen.

    @param  This - Mouse Protocol Pointer

    @retval EFI_SUCCESS
 
**/

typedef EFI_STATUS (*EFI_FUNCTION_STOP)(
    IN  DXE_MOUSE_PROTOCOL  *This
);

/**
    This function is provided to allow overriding of the default mouse 
    speed translation table.

    @param  This - Mouse Protocol pointer
    @param  NewTable - Pointer to new a speed 
            translation table
    @param  NewTableSize - New table size.

    @retval EFI_STATUS

**/

typedef EFI_STATUS (*EFI_FUNCTION_SET_SPEED_TRANSLATION_TABLE)(
    IN  DXE_MOUSE_PROTOCOL       *This,
    IN  MOUSE_TRANSLATION_TABLE  *NewTable,
    IN  UINT32                   NewTableSize
);

/**
    This function returns the current speed of the mouse device.

    @param  This - pointer to the mouse protocol
    @param  MouseVelocity - contains the current mouse speed upon return

    @retval EFI_STATUS

**/

typedef EFI_STATUS (*EFI_FUNCTION_GET_VELOCITY)(
    IN  DXE_MOUSE_PROTOCOL   *This,
    OUT UINT32              *MouseVelocity
);

/**
    Updates the Mouse pointer shape with the input pointer value.

    @param  This - Pointer to the mouse protocol.
    @param  MousePointerBlt - Blt buffer of new Mouse pointer.
    @param  Width - Width in pixels of new Mousepointer.
    @param  Height - Height in pixels of new Mousepointer.
    @param  TransparentPixel - Pixel color in the input mouse pointer buffer that is transparent (screen background)
      
**/

typedef EFI_STATUS (*EFI_FUNCTION_UPDATE_POINTER)(
    IN  DXE_MOUSE_PROTOCOL	         *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *MousePointerBlt,
    IN  UINT32                       Width,
    IN  UINT32                       Height,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *TransparentPixel );

/**
    Updates GOP instance from other application 

    @param  This
    @param  RestoreOptimization

    @retval EFI_STATUS

**/

typedef EFI_STATUS (*EFI_FUNCTION_SET_MOUSE_GOP)(
    IN  DXE_MOUSE_PROTOCOL               *This,
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL     *GOP,
    IN  BOOLEAN                          RestoreOptimization
);

/**
    Returns the mouse image over the passed background buffer

         
    @param  This DXE_MOUSE_PROTOCOL instance
    @param  BkgBuffer Buffer for background
            of mouse image
    @param  MouseBuffer Mouse cursor image on
            top of background buffer
            
    @retval EFI_STATUS status
            EFI_INVALID_PARAMETER

**/

typedef EFI_STATUS (*EFI_FUNCTION_GET_MOUSE_CURSOR)(
    IN  DXE_MOUSE_PROTOCOL               *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *BkgBuffer,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   **MouseBuffer
);

typedef struct _DXE_MOUSE_PROTOCOL {
    UINT8                                                   Revision;
    EFI_FUNCTION_POLL                                       MousePoll;
    EFI_FUNCTION_INITIALIZE                                 InitializePointingDevice;
    EFI_FUNCTION_START_UPDATING_MOUSE_CURSOR                StartUpdatingMouseCursor;
    EFI_FUNCTION_STOP_UPDATING_MOUSE_CURSOR                 StopUpdatingMouseCursor;
    EFI_FUNCTION_REFRESH_MOUSE_CURSOR                       RefreshMouseCursor;
    EFI_FUNCTION_GETPOSITION                                GetCoordinates;
    EFI_FUNCTION_SETPOSITION                                SetCoordinates;
    EFI_FUNCTION_SETSPEED                                   SetMouseSpeed;
    EFI_FUNCTION_GETBUTTONSTATUS                            GetButtonStatus;
    EFI_FUNCTION_SETVIEWPOINT                               SetMouseCoordinates;
    EFI_FUNCTION_SOFTKEY                                    SetSoftKeyData;
    EFI_FUNCTION_STOP                                       StopPointingDevice;
    EFI_FUNCTION_SET_SPEED_TRANSLATION_TABLE                SetSpeedTranslationTable;
    EFI_FUNCTION_GET_VELOCITY                               GetMouseVelocity;
    EFI_FUNCTION_UPDATE_POINTER                             UpdateMousePointer;
    EFI_FUNCTION_UPDATE_MOUSE_POINTER_PROPERTY              UpdateMousePointerProperty;
    EFI_FUNCTION_SET_MOUSE_GOP                              SetMouseGOP;
    EFI_FUNCTION_GET_MOUSE_CURSOR                           GetMouseCursorImage;
} DXE_MOUSE_PROTOCOL;


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

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
//**********************************************************************
