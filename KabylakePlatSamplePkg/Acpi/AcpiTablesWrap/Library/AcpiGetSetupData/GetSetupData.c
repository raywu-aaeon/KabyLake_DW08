//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
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
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Board/GetSetupData.c $
// 
//*************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name:        GetSetupData.c
//
// Description: Custom North Bridge setup data behavior implementation
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>

#include <Token.h>
#include <Setup.h>
#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <AcpiSetupData.h>
#include <AmiCspLib.h>
#include <AmiAcpiElinks.h>
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
extern ACPI_OEM_SETUP_CALLBACK ACPI_OEM_SETUP_CALLBACK_LIST EndOfList;

// Variable Declaration(s)

ACPI_OEM_SETUP_CALLBACK* AcpiOemSetupCallbackList[] = \
                                            {ACPI_OEM_SETUP_CALLBACK_LIST NULL};
//---------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AcpiOemSetupCallbacks
//
// Description: This function calls registered callbacks for OEM/custom setup.
//
// Input:       *Services      - Pointer to PeiServices or RuntimeServices
//                               structure  
//              *AcpiSetupData - Pointer to custom setup data to return
//              *SetupData     - Pointer to system setup data.
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID AcpiOemSetupCallbacks (
    IN VOID                   *Services,
    IN OUT ACPI_SETUP_DATA    *AcpiSetupData,
    IN SETUP_DATA             *SetupData
  )
{
  UINT32                  i;
    
  for (i = 0; AcpiOemSetupCallbackList[i] != NULL; i++) 
      AcpiOemSetupCallbackList[i]( Services, AcpiSetupData, SetupData );  

}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetAcpiSetupData
//
// Description: This function returns custom setup data from system SetupData
//              variable 
//
// Input:       *Services      - Pointer to PeiServices or RuntimeServices
//                               structure  
//              *AcpiSetupData - Pointer to custom setup data to return
//
// Output:      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID GetAcpiSetupData (
    IN VOID                   *Services,
    IN OUT ACPI_SETUP_DATA    *AcpiSetupData
)
{
    EFI_STATUS                      Status;
    SETUP_DATA                      SetupData;
    EFI_RUNTIME_SERVICES            *RunServices;
    UINTN                           VariableSize = sizeof(SETUP_DATA);
    
    RunServices = (EFI_RUNTIME_SERVICES *)Services;
    Status = RunServices->GetVariable( L"Setup", \
                            &gSetupGuid, \
                            NULL, \
                            &VariableSize, \
                            &SetupData );

    if (EFI_ERROR(Status)) {
        AcpiOemSetupCallbacks( Services, AcpiSetupData, NULL );
    } else {
        AcpiOemSetupCallbacks( Services, AcpiSetupData, &SetupData );
    }
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
