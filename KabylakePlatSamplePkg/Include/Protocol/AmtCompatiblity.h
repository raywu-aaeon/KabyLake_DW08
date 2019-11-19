//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//**********************************************************************

/** @file AmtCompatiblity.h
    Amt Wrapper Define file

**/
#ifndef _AMT_WRAPPER_H
#define _AMT_WRAPPER_H

#include <Protocol/AmtWrapperProtocol.h>
#include <Protocol/AmtPolicy.h>
#include <MeBiosPayloadHob.h>
#include <MeBiosPayloadData.h>
#include <MeBiosExtensionSetup.h>

//RC Common Protocol
// gDxeAmtPolicyGuid
#define DXE_AMTPOLICY_WRAPPER_POLICY_GUID \
   {0x6725e645, 0x4a7f, 0x9969, {0x82, 0xec, 0xd1, 0x87, 0x21, 0xde, 0x5a, 0x57}}

// gEfiAmtWrapperProtocolGuid
#define AMT_WRAP_PROTOCOL_GUID \
        {0x919383de, 0xebac, 0x4924, {0x01, 0x94, 0x52, 0x59, 0xe0, 0x0d, 0x65, 0x7a}}


//OEM Commem Protocol
#define MBP_WRAP_PROTOCOL_GUID \
        { 0x2f5dd2d, 0xad22, 0x4ad0, { 0x83, 0x88, 0xd1, 0x26, 0x5e, 0xab, 0xdc, 0xb7 }}

///extern EFI_GUID                   gMbpWrapperProtocolGuid;


#endif //_AMT_WRAPPER_H
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
