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

/** @file AmiTseMouse.h
    Provides AMITSE respective Mouse hook related definitions. 

**/

#ifndef __AMITSEMOUSE__H__
#define __AMITSEMOUSE__H__
#ifdef __cplusplus
extern "C" {
#endif

#include "Token.h"

#if MINISETUP_MOUSE_SUPPORT

#include <Include/AmiDxeLib.h>
#include "commonoem.h"

#if OLD_MOUSE_DRIVER_SUPPORT 
#include "Include/Protocol/MouseLib/MouseLib.h"
#else 
#include "Include/Protocol/MouseProtocol.h"
#endif 

#if AMITSE_SOFTKBD_SUPPORT
#include "Include/Protocol/SoftKbdProtocol.h"
#endif

#define CURSOR_WIDTH   11
#define CURSOR_HEIGHT  20

BOOLEAN  gTseMousePointerHide = FALSE;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE  *gST;

//
// MouseDriver related extern variables
//
extern DXE_MOUSE_PROTOCOL *TSEMouse;
extern EFI_GUID           gMouseDriverGuid;

//
// SoftKbd related extern variables
//
#if AMITSE_SOFTKBD_SUPPORT
extern SOFT_KBD_PROTOCOL *TSESoftKbd_n;
extern EFI_GUID          gSoftKbdGuid;
#endif

//
// Resolution and Glyph related functions
//
extern INT32 Screen_Top;
extern INT32 Screen_Left;       //Calculated Setup Screen Top and Left 

EFI_STATUS GetScreenResolution(UINTN *ResX, UINTN *ResY);
UINTN      HiiGetGlyphWidth(VOID);
UINTN      HiiGetGlyphHeight(VOID);
UINTN      StyleGetStdMaxRows(VOID);

//
// SoftKbd related hook functions
//
VOID    TSEMouseInit(VOID);
VOID    TSEStringReadLoopEntryHook(VOID);
VOID    TSEStringReadLoopExitHook(VOID);
VOID    TSEMouseInitHook(VOID);
VOID    TSEMouseStopHook(VOID);
VOID    TSEMouseRefreshHook(VOID);
VOID    TSEMouseStartHook(VOID);
BOOLEAN TSEMouseIgnoreMouseActionHook(VOID);
VOID    TSEMouseDestroyHook(VOID);
VOID    TSEMouseHide(BOOLEAN Hide);
BOOLEAN SoftKbdPointerHide();

#endif
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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
