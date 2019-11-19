//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiNbMiscLib.h
  This file contains North Bridge chipset porting library Miscellaneous 
  functions and data structure definitions for both PEI & DXE stages.
  
*/

#ifndef __AMI_NB_MISC_LIB_H__
#define __AMI_NB_MISC_LIB_H__

#include <Uefi.h>
#include "Token.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    UINT8     ChannelNumber;  //< Zero based channel number.
    UINT8     DimmNumber;     //< Zero based DIMM number.
    UINT8     Rank;           //< Zero based rank number.
    UINT8     Bank;           //< Zero based bank number.
    UINT16    Cas;            //< Zero based CAS number.
    UINT16    Ras;            //< Zero based RAS number.
} NB_ADDRESS_DECODE;

VOID NbResetCpuOnly (
    VOID
);

UINT32 NBGetTsegBase (
    VOID
);

BOOLEAN NbCheckVtdSupport (
    VOID
);

BOOLEAN NbIsDualChannel (
    IN UINT8              MemoryChannelType
);

EFI_STATUS NbLockPavpc (
    IN BOOLEAN            MODE
);

EFI_STATUS NbGetDimmLocInfo (
    IN CONST UINTN        MemoryAddr,
    OUT NB_ADDRESS_DECODE *NbAddressDecode
);

UINT32 NbFindCapPtr(
    IN UINT64  PciAddress,
    IN UINT8   CapId
);

#ifdef __cplusplus
}
#endif
#endif


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
