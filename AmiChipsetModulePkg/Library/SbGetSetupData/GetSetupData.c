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
#include <Setup.h>
#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <SbSetupData.h>
#include <AmiCspLib.h>
#include <SbElinks.h>
#include <Library/BaseMemoryLib.h>
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

// Function Definition(s)

// Function Prototype(s)

// External Declaration(s)

extern SB_OEM_SETUP_CALLBACK SB_OEM_SETUP_CALLBACK_LIST EndOfList;

// Variable Declaration(s)

SB_OEM_SETUP_CALLBACK* SbOemSetupCallbackList[] = \
                                            {SB_OEM_SETUP_CALLBACK_LIST NULL};

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
SbOemSetupCallbacks(
    IN VOID              *Services,
    IN OUT SB_SETUP_DATA *SbSetupData,
    IN SETUP_DATA        *SetupData,
    IN BOOLEAN           Pei
)
{
    UINT32 Index;
 
    for (Index = 0; SbOemSetupCallbackList[Index] != NULL; Index++) {
        SbOemSetupCallbackList[Index]( Services, SbSetupData, SetupData, Pei); 
    }
}

/**
    This function returns custom setup data from system SetupData
    variable 

    @param *Services         - Pointer to PeiServices or RuntimeServices
        structure  
    @param *SbSetupData      - Pointer to custom setup data to return
    @param Pei               - Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID GetSbSetupData (
    IN VOID                 *Services,
    IN OUT SB_SETUP_DATA    *SbSetupData,
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
  //  Update SB_SETUP_DATA according to the default values.
  
  ZeroMem(SbSetupData, sizeof(SB_SETUP_DATA));

  if (Pei) {
    PeiServices = (EFI_PEI_SERVICES **)Services;
    Status = (*PeiServices)->LocatePpi( \
                               (CONST EFI_PEI_SERVICES **)PeiServices, \
                               &gEfiPeiReadOnlyVariable2PpiGuid, \
                               0, \
                               NULL, \
                               (VOID **)&ReadOnlyVariable );

    if (!EFI_ERROR(Status)) {
      Status = ReadOnlyVariable->GetVariable( \
		                           ReadOnlyVariable, \
                                   L"Setup", \
                                   &gSetupGuid, \
                                   NULL, \
                                   &VariableSize, \
                                   &SetupData );
    }
  } else {
    RunServices = (EFI_RUNTIME_SERVICES *)Services;
    Status = RunServices->GetVariable( \
		                     L"Setup", \
                             &gSetupGuid, \
                             NULL, \
                             &VariableSize, \
                             &SetupData );
  }

  if (EFI_ERROR(Status)) {
    SbOemSetupCallbacks( Services, SbSetupData, NULL, Pei );
  } else {
    SbOemSetupCallbacks( Services, SbSetupData, &SetupData, Pei );
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
