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

/** @file GetSetupData.c
    Custom South Bridge setup data behavior implementation

**/

#include <Token.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <SbElinks.h>
#include <SbPolicyUpdate.h>
#include <Ppi/SiPolicy.h>
//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------

// GUID Definition(s)

// Protocol/Ppi Definition(s)

// Function Definition(s)

// Function Prototype(s)

// External Declaration(s)

extern SB_OEM_PCH_POLICY_CALLBACK SB_OEM_PCH_POLICY_CALLBACK_LIST EndOfList;

// Variable Declaration(s)

SB_OEM_PCH_POLICY_CALLBACK* SbOemPchPolicyCallbackList[] = \
                                            {SB_OEM_PCH_POLICY_CALLBACK_LIST NULL};

//---------------------------------------------------------------------------

/**
    This function calls registered callbacks for OEM/custom setup.

    @param *Services    - Pointer to PeiServices or RuntimeServices
        structure  
    @param *SbSetupData - Pointer to custom setup data to return
    @param *SetupData   - Pointer to system setup data.
    @param Pei          - Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/

VOID
OemPchPolicyUpdateCallBack(
    IN EFI_PEI_SERVICES        **PeiServices,
    IN SI_POLICY_PPI           *SiPolicy,
    IN UINT16                  BoardId 
)
{
    UINT32    Index;
 
    for (Index = 0; SbOemPchPolicyCallbackList[Index] != NULL; Index++) {
        SbOemPchPolicyCallbackList[Index]( (VOID *)PeiServices, SiPolicy, BoardId);
    }
}

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
