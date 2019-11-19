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
    Custom North Bridge setup data behavior implementation

**/

#include <Token.h>
#include <Setup.h>
#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <NbSetupData.h>
#include <AmiCspLib.h>
#include <NbElinks.h>
//#include <Library/DebugLib.h>

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
static EFI_GUID gSetupGuid = SETUP_GUID;

// Protocol/Ppi Definition(s)

// External Declaration(s)
extern NB_OEM_SETUP_CALLBACK NB_OEM_SETUP_CALLBACK_LIST EndOfList;

// Variable Declaration(s)

NB_OEM_SETUP_CALLBACK* NbOemSetupCallbackList[] = \
                                            {NB_OEM_SETUP_CALLBACK_LIST NULL};
//---------------------------------------------------------------------------

/**
    This function calls registered callbacks for OEM/custom setup.

    @param Services Pointer to PeiServices or RuntimeServices
        structure  
    @param NbSetupData Pointer to custom setup data to return
    @param SetupData Pointer to system setup data.
    @param Pei Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/

VOID
NbOemSetupCallbacks(
    IN VOID                 *Services,
    IN OUT NB_SETUP_DATA    *NbSetupData,
    IN SETUP_DATA           *SetupData,
    IN BOOLEAN              Pei
)
{
    UINT32	Index;
    
    for (Index = 0; NbOemSetupCallbackList[Index] != NULL; Index++) {
        NbOemSetupCallbackList[Index]( Services, NbSetupData, SetupData, Pei);
    }
}

/**
    This function returns custom setup data from system SetupData
    variable 

    @param Services Pointer to PeiServices or RuntimeServices
        structure  
    @param NbSetupData Pointer to custom setup data to return
    @param Pei Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID GetNbSetupData (
    IN VOID                 *Services,
    IN OUT NB_SETUP_DATA    *NbSetupData,
    IN BOOLEAN              Pei
)
{
    EFI_STATUS                      Status;
    SETUP_DATA                      SetupData;
    EFI_PEI_SERVICES                **PeiServices;
    EFI_RUNTIME_SERVICES            *RunServices;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable = NULL;
    UINTN                           VariableSize = sizeof(SETUP_DATA);
    
// Porting Start
    
    // Update NB_SETUP_DATA according to the default values.
    NbSetupData->NbSetupString         = NB_SETUP_DATA_SIG;
    
// Porting End

    if (Pei) {
        PeiServices = (EFI_PEI_SERVICES **)Services;
        Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                            &gEfiPeiReadOnlyVariable2PpiGuid, \
                                            0, \
                                            NULL, \
                                            (VOID **)&ReadOnlyVariable );

        if (!EFI_ERROR(Status)) {
            Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                                    L"Setup", \
                                                    &gSetupGuid, \
                                                    NULL, \
                                                    &VariableSize, \
                                                    &SetupData );
        }
    } else {
        RunServices = (EFI_RUNTIME_SERVICES *)Services;
        Status = RunServices->GetVariable( L"Setup", \
                                           &gSetupGuid, \
                                           NULL, \
                                           &VariableSize, \
                                           &SetupData );
    }

    if (EFI_ERROR(Status)) {
        NbOemSetupCallbacks( Services, NbSetupData, NULL, Pei );
    } else {
        NbOemSetupCallbacks( Services, NbSetupData, &SetupData, Pei );
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
