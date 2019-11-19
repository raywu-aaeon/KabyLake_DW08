//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
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
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/LynxPoint/Intel Pch SB Chipset/SB Protocols/SBPlatformData.h 1     2/08/12 8:26a Yurenlai $
//
// $Revision: 1 $
//
// $Date: 2/08/12 8:26a $
//*************************************************************************
/** @file SBPlatformData.h
    SB Specific Setup Variables and Structures

**/
//**********************************************************************
#ifndef _SB_PLATFORM_DATA_H_
#define _SB_PLATFORM_DATA_H_
#ifdef __cplusplus
extern "C" {
#endif

  #pragma pack(1)

typedef struct _SETUP_PLATFORM_DATA
{
    UINT8  GbePciePortNum;
    UINT8  PlatformFlavor;
    UINT8  PcieSBDE;
//    UINT8  IGFXAvailable1;
//    UINT8  VTdAvailable1;
    UINT8  PlatformSupportCppc;
    UINT8  PlatformSupportRtD3;
    UINT8  PchHotLevelPresent;
    UINT8  LPMSupport;
    UINT8  LPTType;
    UINT8  HideDeepSx;
    UINT8  PchRid;
//    UINT8   PegPresent[4];
//    UINT8   DimmPresent[4];
}SETUP_PLATFORM_DATA;
/*
  typedef struct _SB_PLATFORM_DATA
  {
    UINT8 PchHotLevelPresent;
    UINT8 GbePciePortNum;
    UINT8 PcieSBDE;
    UINT8 PchRid;
    UINT8 LPTType;
    UINT8 HideDeepSx;
  }SB_PLATFORM_DATA;
*/
  #pragma pack()

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
