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

/** @file SbHdaVerbTableLib.h
    This header file define South bridge HDA Verb Table Override  
    function and the necessary structures

*/
#ifndef  _AMI_SB_HDA_VERB_TABLE_LIB_H   //To Avoid this header get compiled twice
#define  _AMI_SB_HDA_VERB_TABLE_LIB_H
#include "HDAVBTBL.h"

EFI_STATUS
SbHdaVerbTableOverride (
    IN EFI_PEI_SERVICES       **PeiServices,
    IN OUT SB_HDA_VERB_TABLE  **VerbTable,
    OUT UINT8                 *TotalVerbTableNumber
);

VOID AmiInstallOemHdaVerbTables (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN SI_POLICY_PPI     	*SiPolicy
);

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
