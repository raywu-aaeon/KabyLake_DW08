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

/** @file MouseDriver.h
    Mouse Driver header file
**/

#ifndef _EFI_DRIVER_NAME_H_
#define _EFI_DRIVER_NAME_H_

//---------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/MouseProtocol.h>
#include <Protocol/DriverBinding.h>

//---------------------------------------------------------------------------

// For Timer Events
#define     TIMER_ONE_SECOND                10000000 
#define     NANOSECOND                      1000000000
#define     ONE_TIMER_EQUALS_NANO           100 

// PIXEL Definitions
#define     MOUSE_COLOR                 EFI_TEXT_ATTR(EFI_WHITE, EFI_WHITE)     // White color
#define     MOUSE_BORDER_COLOR          0                                       // Black color
#define     O                           {50,50,50}                              //Transparent Pixel
#define     W                           {MOUSE_COLOR, MOUSE_COLOR, MOUSE_COLOR}
#define     B                           {MOUSE_BORDER_COLOR, MOUSE_BORDER_COLOR, MOUSE_BORDER_COLOR}
#define     CURSOR_WIDTH                11
#define     CURSOR_HEIGHT               20

#if SOFTKBD_ICON_SUPPORT || BITLOCKER_SOFTKBD_SUPPORT
#define     ESC_ICON        1
#define     ACT_ICON        2
#define     DEACT_ICON      3
#define     POST_ICON       4
#endif

EFI_GRAPHICS_OUTPUT_BLT_PIXEL MousePointer[] =  // Default Mouse Pointer [Arrow]
 {   
  B,O,O,O,O,O,O,O,O,O,O,
  B,B,O,O,O,O,O,O,O,O,O,
  B,W,B,O,O,O,O,O,O,O,O,
  B,W,W,B,O,O,O,O,O,O,O,
  B,W,W,W,B,O,O,O,O,O,O,
  B,W,W,W,W,B,O,O,O,O,O,
  B,W,W,W,W,W,B,O,O,O,O,
  B,W,W,W,W,W,W,B,O,O,O,
  B,W,W,W,W,W,W,W,B,O,O,
  B,W,W,W,W,W,W,W,W,B,O,
  B,W,W,W,W,W,W,B,B,B,B,
  B,W,B,B,W,W,B,O,O,O,O,
  B,B,O,O,B,W,B,O,O,O,O,
  B,O,O,O,O,B,W,B,O,O,O,
  O,O,O,O,O,B,W,B,O,O,O,
  O,O,O,O,O,O,B,W,B,O,O,
  O,O,O,O,O,O,B,W,B,O,O,
  O,O,O,O,O,O,O,B,W,B,O,
  O,O,O,O,O,O,O,B,W,B,O,
  O,O,O,O,O,O,O,O,B,B,B
 };
 
 typedef struct _MOUSE_ACCELERATION_DATA {
    MOUSE_TRANSLATION_TABLE *TranslationTable;
    UINT32                  TableSize;
    INT32                   StartX;
    INT32                   StartY;
    INT32                   EndX;
    INT32                   EndY;
    UINT32                  RealSpeed;
    UINT32                  CurrentFrame;
} MOUSE_ACCELERATION_DATA;

typedef struct _SIMPLE_POINTER {
    DLINK                           Link; //MUST BE THE FIRST FIELD
    EFI_SIMPLE_POINTER_PROTOCOL     *SimplePointer;
    EFI_HANDLE                      Handle;
} SIMPLE_POINTER;

typedef struct _ABSOLUTE_POINTER {
    DLINK                           Link; //MUST BE THE FIRST FIELD
    EFI_ABSOLUTE_POINTER_PROTOCOL   *AbsPointer;
    EFI_HANDLE                      Handle;
} ABSOLUTE_POINTER;

//
//Mouse Driver Protocol Functions
//

EFI_STATUS 
InitializePointingDevice (
    IN DXE_MOUSE_PROTOCOL  *This,
    IN INT32               StartX,
    IN INT32               StartY,
    IN INT32               EndX,
    IN INT32               EndY );

EFI_STATUS  
StartUpdatingMouseCursor (
    IN  DXE_MOUSE_PROTOCOL  *This
);

EFI_STATUS 
StopUpdatingMouseCursor (
    IN  DXE_MOUSE_PROTOCOL  *This
);

EFI_STATUS 
RefreshMouseCursor (
    IN  DXE_MOUSE_PROTOCOL  *This
);

EFI_STATUS  
SetMouseSpeed (
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  UINT32               Value
);

EFI_STATUS
GetButtonStatus (
    IN  DXE_MOUSE_PROTOCOL  *This,
    OUT INT32               *Button_Status
);

EFI_STATUS
GetCoordinates (
    IN  DXE_MOUSE_PROTOCOL  *This,
    OUT INT32               *PointerX,
    OUT INT32               *PointerY,
    OUT INT32               *PointerZ
);

EFI_STATUS  
SetMouseCoordinates (
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  INT32                MouseViewStartRow,
    IN  INT32                MouseViewStartColoum,
    IN  INT32                MouseViewEndRow,
    IN  INT32                MouseViewEndColoum
);

EFI_STATUS 
SetSoftKeyData (
    IN  DXE_MOUSE_PROTOCOL  *This,
    IN  SOFT_KEY_DATA       *SoftKeyData
);

EFI_STATUS  
StopPointingDevice (
    IN  DXE_MOUSE_PROTOCOL  *This
);

EFI_STATUS 
SetSpeedTranslationTable (
    IN  DXE_MOUSE_PROTOCOL       *This,
    IN  MOUSE_TRANSLATION_TABLE  *NewTable,
    IN  UINT32                   NewTableSize
);

EFI_STATUS 
GetMouseVelocity (
    IN  DXE_MOUSE_PROTOCOL   *This,
    OUT UINT32              *MouseVelocity
);

EFI_STATUS 
MousePoll (
    IN  DXE_MOUSE_PROTOCOL    *This
);

EFI_STATUS 
SetCoordinates (
    IN  DXE_MOUSE_PROTOCOL   *This,
    IN  INT32                PointerX,
    IN  INT32                PointerY
);

EFI_STATUS
UpdateMousePointerProperty (
    IN  DXE_MOUSE_PROTOCOL       *This,
    IN  MOUSE_POINTER_PROPERTY   Property
);

EFI_STATUS
UpdateMousePointer (
    IN  DXE_MOUSE_PROTOCOL             *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *MousePointerBlt,
    IN  UINT32                         Width,
    IN  UINT32                         Height,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *TransparentPixel
);

EFI_STATUS 
SetMouseGOP (
    IN  DXE_MOUSE_PROTOCOL              *This,
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GOP,
    IN  BOOLEAN                         RestoreOptimization
);

EFI_STATUS
GetMouseCursorImage (
    IN  DXE_MOUSE_PROTOCOL                *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BkgBuffer,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL    **MouseBuffer
);

//
// Module Private functions
//

EFI_STATUS
MouseDriverEntryPoint (
    IN  EFI_HANDLE           ImageHandle,
    IN  EFI_SYSTEM_TABLE     *SystemTable
);

EFI_STATUS 
MouseDriverSupported (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
);

EFI_STATUS 
MouseDriverStart (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
);

EFI_STATUS 
MouseDriverStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  UINTN                            NumberOfChildren,
    IN  EFI_HANDLE                       *ChildHandleBuffer
);

EFI_STATUS 
AbsMouseDriverSupported (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
);

EFI_STATUS 
AbsMouseDriverStart (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
);

EFI_STATUS 
AbsMouseDriverStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  UINTN                            NumberOfChildren,
    IN  EFI_HANDLE                       *ChildHandleBuffer
);

EFI_STATUS
InstallMouseProtocol (
    IN  EFI_HANDLE   Controller
);

VOID    
GetMouseAction (
    EFI_EVENT Event,
    VOID      *Context
);

#if DRAW_MOUSE_ON_HOTPLUG
VOID    
ChecktoDrawMouse (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
);
#endif

UINT32 
Sqrt32 (
    IN  UINT32 Number
);

UINT32 
PointDistance (
    IN  INT32 X1,
    IN  INT32 Y1,
    IN  INT32 X2,
    IN  INT32 Y2
);

VOID 
CalculateRealSpeed (
    IN  OUT MOUSE_ACCELERATION_DATA   *Adata
);

INT32 
FixedMultiplyHundredths (
    IN  INT32   Value, 
    IN  UINT32  Multiplier
);

INT32 
SpeedTranslation (
    IN  MOUSE_ACCELERATION_DATA  *Adata,
    IN  INT32                    RelativePosition
);

VOID 
LocateGopProtocol (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
);

VOID    
LeftClickCallBack (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
);

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
