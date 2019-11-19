//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************

/** @file SleepSmi.c
    Provide functions to register and handle Sleep SMI
    functionality.  

**/

#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <SbSetupData.h>
#include <Library/PchInfoLib.h>
#include <Library/AmiSbMiscLib.h>

#define AMI_SMM_SX_DISPATCH_PROTOCOL EFI_SMM_SX_DISPATCH2_PROTOCOL
#define AMI_SMM_SX_DISPATCH_CONTEXT  EFI_SMM_SX_REGISTER_CONTEXT
#define SMM_CHILD_DISPATCH_SUCCESS   EFI_SUCCESS

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

BOOLEAN gPchWakeOnLan   = FALSE;

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)


// Function declarations
#if defined(USB_S5_WAKEUP_SUPPORT) && (USB_S5_WAKEUP_SUPPORT == 1)
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: UsbSbEnablePme
//
// Description: The function enable usb PME.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
UsbSbEnablePme(VOID)
{   
  // Clear PME_B0_STS and PME_STS, then Set PME_B0_EN and PME_EN
  WRITE_IO16_PM(R_PCH_ACPI_GPE0_STS_127_96, 0xffff);       // 0x8c
  SET_IO16_PM(R_PCH_ACPI_GPE0_EN_127_96, BIT13 + BIT11);   // 0x9c          
}
#endif

VOID ChipsetSleepWorkaround(
    VOID
);

/**
    This function will be called by EfiSmmSxDispatch when a Sleep
    SMI occurs and the sleep state is S1.

    PI 0.91, 1.0
    @param DispatchHandle   - SMI dispatcher handle
    @param *DispatchContext - Pointer to the dispatch context
        PI 1.1, 1.2
    @param DispatchHandle  - SMI dispatcher handle
    @param *DispatchContext- Points to an optional Sx SMI context
    @param CommBuffer      - Points to the optional communication
        buffer
    @param CommBufferSize  - Points to the size of the optional

    @retval EFI_STATUS if the new SMM PI is applied.

    @note  This function does not need to put the system to sleep.  This is
              handled by PutToSleep.

**/

EFI_STATUS 
EFIAPI
S1SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    return SMM_CHILD_DISPATCH_SUCCESS;
}


/**
    This function will be called by EfiSmmSxDispatch when a Sleep
    SMI occurs and the sleep state is S3.

    PI 0.91, 1.0
    @param DispatchHandle   - SMI dispatcher handle
    @param *DispatchContext - Pointer to the dispatch context
        PI 1.1, 1.2
    @param DispatchHandle  - SMI dispatcher handle
    @param *DispatchContext- Points to an optional Sx SMI context
    @param CommBuffer      - Points to the optional communication
        buffer
    @param CommBufferSize  - Points to the size of the optional

    @retval EFI_STATUS if the new SMM PI is applied.

    @note  This function does not need to put the system to sleep.  This is
              handled by PutToSleep.

**/

EFI_STATUS 
EFIAPI
S3SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{


    return SMM_CHILD_DISPATCH_SUCCESS;
}


/**
    This function will be called by EfiSmmSxDispatch when a Sleep
    SMI occurs and the sleep state is S4.

    PI 0.91, 1.0
    @param DispatchHandle   - SMI dispatcher handle
    @param *DispatchContext - Pointer to the dispatch context
        PI 1.1, 1.2
    @param DispatchHandle  - SMI dispatcher handle
    @param *DispatchContext- Points to an optional Sx SMI context
    @param CommBuffer      - Points to the optional communication
        buffer
    @param CommBufferSize  - Points to the size of the optional
        communication buffer

    @retval EFI_STATUS if the new SMM PI is applied.

    @note  This function does not need to put the system to sleep.  This is
              handled by PutToSleep.

**/

EFI_STATUS 
EFIAPI
S4SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
  
    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function will be called by EfiSmmSxDispatch when a Sleep
    SMI occurs and the sleep state is S1.

    PI 0.91, 1.0
    @param DispatchHandle   - SMI dispatcher handle
    @param *DispatchContext - Pointer to the dispatch context
        PI 1.1, 1.2
    @param DispatchHandle  - SMI dispatcher handle
    @param *DispatchContext- Points to an optional Sx SMI context
    @param CommBuffer      - Points to the optional communication
        buffer
    @param CommBufferSize  - Points to the size of the optional
        communication buffer

    @retval EFI_STATUS if the new SMM PI is applied.

    @note  This function does not need to put the system to sleep.  This is
              handled by PutToSleep.

**/

EFI_STATUS 
EFIAPI
S5SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
#if defined(USB_S5_WAKEUP_SUPPORT) && (USB_S5_WAKEUP_SUPPORT == 1)    
    UsbSbEnablePme();
#endif   
 
    return SMM_CHILD_DISPATCH_SUCCESS;
}


/**
    This function executes chipset workaround that is needed.  It is 
    necessary for the system to go to S3 - S5.

    @param VOID

    @retval VOID
**/
VOID ChipsetSleepWorkaround( 
    VOID 
)   
{
    ClearMeWakeSts();
}

/** @file
    Install Sleep SMI Handlers for south bridge.

    @param ImageHandle  - Image handle
    @param *SystemTable - Pointer to the system table

    @retval EFI_STATUS
**/

EFI_STATUS InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS                      Status;
    EFI_HANDLE                      hS1Smi;
    EFI_HANDLE                      hS3Smi;
    EFI_HANDLE                      hS4Smi;
    EFI_HANDLE                      hS5Smi;
    AMI_SMM_SX_DISPATCH_PROTOCOL    *SxDispatch;
    AMI_SMM_SX_DISPATCH_CONTEXT     S1DispatchContext = {SxS1, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S3DispatchContext = {SxS3, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S4DispatchContext = {SxS4, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S5DispatchContext = {SxS5, SxEntry};

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;
    Status = pSmst->SmmLocateProtocol( &gEfiSmmSxDispatch2ProtocolGuid , \
                                       NULL, \
                                       (VOID **)&SxDispatch );

    if (EFI_ERROR(Status)) return Status;

    // Register Sleep SMI Handlers
    Status = SxDispatch->Register( SxDispatch, \
                                   S1SleepSmiOccurred, \
                                   &S1DispatchContext, \
                                   &hS1Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S3SleepSmiOccurred, \
                                   &S3DispatchContext, \
                                   &hS3Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S4SleepSmiOccurred, \
                                   &S4DispatchContext, \
                                   &hS4Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S5SleepSmiOccurred, \
                                   &S5DispatchContext, \
                                   &hS5Smi );
    return Status;
}
/**
    This function Registers Sleep SMI functionality.

    @param ImageHandle Handle for this FFS image
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/
EFI_STATUS EFIAPI InitSleepSmi(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS                      Status;
    SB_SETUP_DATA                   *SbSetupData = NULL;
    UINTN                           VariableSize = sizeof(SB_SETUP_DATA);

    InitAmiLib( ImageHandle, SystemTable );

    // Put the Setup Vairable to SMM if needed.
    Status = pBS->AllocatePool( EfiBootServicesData, \
                                VariableSize, \
                                (VOID **)&SbSetupData );
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }

    GetSbSetupData( pRS, SbSetupData, FALSE );

    gPchWakeOnLan = (SbSetupData->PchWakeOnLan == 1) ? TRUE : FALSE;
    pBS->FreePool( SbSetupData );

    return InitSmmHandler( ImageHandle, SystemTable, InSmmFunction, NULL );
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
