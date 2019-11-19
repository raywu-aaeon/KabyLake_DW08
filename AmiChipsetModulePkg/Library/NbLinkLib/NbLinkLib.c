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
/** @file NbLinkLib.c
    North Bridge Library for ELINK functions

**/
//*************************************************************************

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <PiPei.h>
#include <Setup.h>
#include <NbSetupData.h>
#include <Library/DebugLib.h>

//-------------------------------------------------------------------------
// Variable and External Declaration(s)
//-------------------------------------------------------------------------
// Variable Declaration(s)


/**
    This function returns NB Chipset setup data from system SetupData
    variable 

    @param Services Pointer to PeiServices or RuntimeServices
        structure  
    @param NbSetupData Pointer to custom setup data to return
    @param SetupData Pointer to system setup data.
    @param Pei Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/
VOID NbSetupCallbacks (
    IN VOID              *Services,
    IN OUT NB_SETUP_DATA *NbSetupData,
    IN SETUP_DATA        *SetupData,
    IN BOOLEAN           Pei )
{
#if defined NB_SETUP_SUPPORT && NB_SETUP_SUPPORT == 1
    if (SetupData != NULL) {
        // Porting Start
        
        // Porting End
    }
    DEBUG ((DEBUG_INFO, "NbSetupCallbacks ends.....\n"));
#endif
}

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
