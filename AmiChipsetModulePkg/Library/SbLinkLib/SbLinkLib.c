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
//*****************************************************************************
/** @file SbLinkLib.c
    South Bridge Library for ELINK functions

**/
//*************************************************************************

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <PiPei.h>
#include <Setup.h>
#include <SbSetupData.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

//-------------------------------------------------------------------------
// Variable and External Declaration(s)
//-------------------------------------------------------------------------
// Variable Declaration(s)


/**
    This function returns SB Chipset setup data from system SetupData
    variable

    @param *Services    - Pointer to PeiServices or RuntimeServices
        structure
    @param *SbSetupData - Pointer to custom setup data to return
    @param *SetupData   - Pointer to system setup data.
    @param Pei          - Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/
VOID SbSetupCallbacks (
  IN VOID              *Services,
  IN OUT SB_SETUP_DATA *SbSetupData,
  IN SETUP_DATA        *SetupData,
  IN BOOLEAN           Pei )
{
  DEBUG ((DEBUG_ERROR, "SbSetupCallbacks starts.....\n"));

  DEBUG ((DEBUG_ERROR, "SbSetupCallbacks ends.....\n"));
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
