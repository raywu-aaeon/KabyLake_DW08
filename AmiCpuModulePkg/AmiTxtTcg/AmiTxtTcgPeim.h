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

/** @file AmiTxtTcgPeim.h
    Header for the AmiTxtTcgPeim Peim.

**/

#ifndef TCG2_OEM_INIT_H
#define TCG2_OEM_INIT_H

#include "Tpm20.h"
#include "TPM2Interface/TpmTisLib.h"
#include "TPM2Interface/TpmCrbLib.h"
#include "TpmAuxCommLib.h"


#ifdef TPM_H2NS
#undef TPM_H2NS
#endif

#ifdef TPM_H2NL
#undef TPM_H2NL
#endif

#define TPM_H2NS( x ) \
    ( (UINT16)( ((x)<<8) | ((x)>>8) ) )
#define TPM_H2NL( x ) \
    ( (UINT32)((((UINT32)(x)) >> 24)    \
             | (((x) >> 8) & 0xff00)  \
             | ((x) << 24)            \
             | (((x) & 0xff00) << 8)) )

typedef enum {
    UnKnowTpm = 0,
    TpmIs_12,
    TpmIs_20
} TPM_DETECT_DEVICE_TYPE;

/*#if defined(SMDBG_SUPPORT_LIB)
#undef DEBUG
    #define DEBUG(Arguments) SMDbgTrace Arguments
#endif*/

#endif // End define TCG2_OEM_INIT_H
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
