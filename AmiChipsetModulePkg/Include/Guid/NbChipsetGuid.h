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
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/IntelGopDriver/IntelSaGopDriverProtocol/IntelSaGopDriver.h 3     4/23/13 8:00a Ireneyang $
//
// $Revision: 3 $
//
// $Date: 4/23/13 8:00a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/IntelGopDriver/IntelSaGopDriverProtocol/IntelSaGopDriver.h $
// 
//*************************************************************************
/** @file NbChipsetGuid.h
    NorthBridge Specific Guid

**/
//*************************************************************************

#ifndef __AMI_CHIPSET_GUID_H__
#define __AMI_CHIPSET_GUID_H__

#define AMI_CHIPSET_VARIABLE_GUID \
  { 0x40312829, 0x7891, 0x4abd, {0xb2, 0x00, 0x0c, 0x54, 0x1b, 0x06, 0x19, 0x39 } }

#define AMI_NB_PEG_INFO_GUID \
  { 0x7d971640, 0x6815, 0x408a, {0xac, 0x4f, 0xbc, 0xb7, 0xa3, 0xbd, 0x93, 0x36 } }

extern EFI_GUID gAmiChipsetVariableGuid;
extern EFI_GUID gAmiNbPegInfoGuid;

#endif
