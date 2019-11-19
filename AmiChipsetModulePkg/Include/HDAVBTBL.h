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
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/LynxPoint/Intel Pch SB Board/HDAVBTBL.h 1     2/08/12 8:22a Yurenlai $
//
// $Revision: 1 $
//
// $Date: 2/08/12 8:22a $
//*************************************************************************
// 
//*************************************************************************
/** @file HDAVBTBL.h
    HD Audio Verb Table

**/
//*************************************************************************

#ifndef _HDAVBTBL_H // To Avoid this header get compiled twice
#define _HDAVBTBL_H
#ifdef __cplusplus
extern "C" {
#endif

#include <ConfigBlock.h>
#include <ConfigBlock/HdAudioConfig.h>
#include <Ppi/SiPolicy.h>

typedef struct {
  PCH_HDA_VERB_TABLE_HEADER  VerbTableHeader;  ///< The header information in Azalia verb table.
  UINT32                     *VerbPtr;         ///< Pointer to the buffer containing verb tables data provided by platform.
} SB_HDA_VERB_TABLE;

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
