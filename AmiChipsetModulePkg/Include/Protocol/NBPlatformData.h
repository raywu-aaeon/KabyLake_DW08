//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
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
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Chipset/NB Protocols/NBPlatformData.h 1     2/08/12 4:35a Yurenlai $
//
// $Revision: 1 $
//
// $Date: 2/08/12 4:35a $
//*************************************************************************
/** @file NBPlatformData.h
    NB Specific Setup Variables and Structures

**/
//*************************************************************************
#ifndef _NB_PLATFORM_DATA_H_
#define _NB_PLATFORM_DATA_H_
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef struct _NB_PLATFORM_DATA
{
  UINT8  PlatformFlavor;
  UINT8  PlatformType;
  UINT8  BoardType;
  UINT8  IGFXCapability;
  UINT8  IGFXAvailable;
  UINT8  IGFXGopAvailable;
  UINT8  PegAvailable;
  UINT8  VTdAvailable;
  UINT8  UserBoard;
  UINT8  XmpProfile1;
  UINT8  XmpProfile2;
  UINT8  DDR3Type;
  UINT16 CpuFamilyModel;
  UINT16 CpuExtendedFamilyModel;
  UINT8  CpuStepping;
} NB_PLATFORM_DATA;

#pragma pack()

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
