//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: 
//
// $Revision: 
//
// $Date: 
//*************************************************************************
// Revision History
// ----------------
// $Log: 
// 
//*************************************************************************
/** @file IntelSaGopProtocol.h
    GOP Specific Protocol

**/
//*************************************************************************

#ifndef _SA_GOP_POLICY_PROTOCOL_H_
#define _SA_GOP_POLICY_PROTOCOL_H_

#define GOP_DISPLAY_BRIGHTNESS_PROTOCOL_GUID \
  { 0x6ff23f1d, 0x877c, 0x4b1b, 0x93, 0xfc, 0xf1, 0x42, 0xb2, 0xee, 0xa6, 0xa7 }

#define GOP_DISPLAY_BIST_PROTOCOL_GUID \
  { 0xf51dd33a, 0xe57f, 0x4020, 0xb4, 0x66, 0xf4, 0xc1, 0x71, 0xc6, 0xe4, 0xf7 }

#define GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01     0x01

#define GOP_DISPLAY_BIST_PROTOCOL_REVISION_01           0x01


#pragma pack(1)

typedef enum {
   Docked,
   UnDocked,
   DockStatusMax
} DOCK_STATUS;

typedef struct _GOP_DISPLAY_BRIGHTNESS_PROTOCOL     GOP_DISPLAY_BRIGHTNESS_PROTOCOL;
typedef struct _GOP_DISPLAY_BIST_PROTOCOL           GOP_DISPLAY_BIST_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *GET_PLATFORM_DOCK_STATUS) (
   OUT DOCK_STATUS CurrentDockStatus
);

typedef
EFI_STATUS
(EFIAPI *GET_MAXIMUM_BRIGHTNESS_LEVEL) (
   IN GOP_DISPLAY_BRIGHTNESS_PROTOCOL *This,
   OUT UINT32 *MaxBrightnessLevel
);

typedef
EFI_STATUS
(EFIAPI *GET_CURRENT_BRIGHTNESS_LEVEL) (
   IN GOP_DISPLAY_BRIGHTNESS_PROTOCOL *This,
   OUT UINT32 *CurrentBrightnessLevel
);

typedef
EFI_STATUS
(EFIAPI *SET_BRIGHTNESS_LEVEL) (
   IN GOP_DISPLAY_BRIGHTNESS_PROTOCOL *This,
   IN UINT32 BrightnessLevel
);

typedef
EFI_STATUS
(EFIAPI *ENABLE_BIST) (
   IN GOP_DISPLAY_BIST_PROTOCOL *This
);

typedef
EFI_STATUS
(EFIAPI *DISABLE_BIST) (
   IN GOP_DISPLAY_BIST_PROTOCOL *This
);

#pragma pack()

typedef struct _GOP_DISPLAY_BRIGHTNESS_PROTOCOL {
  UINT32                             Revision;
  GET_MAXIMUM_BRIGHTNESS_LEVEL       GetMaxBrightnessLevel;
  GET_CURRENT_BRIGHTNESS_LEVEL       GetCurrentBrightnessLevel;
  SET_BRIGHTNESS_LEVEL               SetBrightnessLevel;
} GOP_DISPLAY_BRIGHTNESS_PROTOCOL;

typedef struct _GOP_DISPLAY_BIST_PROTOCOL{
  UINT32                             Revision;
  ENABLE_BIST                        EnableBist;
  DISABLE_BIST                       DisableBist;
} GOP_DISPLAY_BIST_PROTOCOL;

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
