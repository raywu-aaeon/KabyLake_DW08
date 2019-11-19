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
/** @file AmiSyncSetupData.c

**/
#include <Token.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <SetupVariable.h>
#include <PchAccess.h>
#include <Library/PttHeciLib.h>

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
EFI_GUID      SetupGuid = SETUP_GUID;

SETUP_DATA    gSetupData;

VOID 
SyncDisplaySpiInformation(VOID)
{
    UINT32          FlashComponents;
    UINT32          PchSpiBar0;
    UINT32          Data32;
    
    PchSpiBar0 = MmioRead32 (MmPciBase (
                                    DEFAULT_PCI_BUS_NUMBER_PCH,
                                    PCI_DEVICE_NUMBER_PCH_SPI,
                                    PCI_FUNCTION_NUMBER_PCH_SPI)
                                    + R_PCH_SPI_BAR0) &~(B_PCH_SPI_BAR0_MASK);
    
    // Read Descriptor offset 0x14 - To get number of components
    Data32 = 0x0;
    Data32 = Data32 | V_PCH_SPI_FDOC_FDSS_FSDM | 0x4;  // Signature section 0x0000 + offset 0x4 which points to Descriptor offset 0x14
    MmioWrite32 (PchSpiBar0 + R_PCH_SPI_FDOC, Data32);
        
    FlashComponents  = MmioRead32 (PchSpiBar0 + R_PCH_SPI_FDOD);
    DEBUG ((DEBUG_INFO, "FlashComponents = 0x%.8x\n\n", FlashComponents));
       
    Data32 = (((FlashComponents) & (B_PCH_SPI_FDBAR_NC )) >> N_PCH_SPI_FDBAR_NC);
        
    switch (Data32) {
        case 0:
          gSetupData.TwoComponents = 0;
          break;
        case 1:
          gSetupData.TwoComponents = 1;
          break;
        default:
          break;
    }
}

/**
    

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID AmiSyncSetupDataEvent (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{ 
    EFI_STATUS      Status;
    UINTN           SetupVariableSize;
    UINT32          Attributes = 0;
    BOOLEAN         PttCurrentState;
    
    DEBUG((DEBUG_INFO, "AmiSyncSetupDataEvent Enter\n"));
    
    SetupVariableSize = sizeof (SETUP_DATA);
    Status = gRT->GetVariable (
                 L"Setup",
                 &SetupGuid,
                 &Attributes,
                 &SetupVariableSize,
                 &gSetupData
             );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
    }
    
    PttHeciGetState (&PttCurrentState);
    DEBUG((DEBUG_INFO, "PttCurrentState = %x\n", PttCurrentState));
    gSetupData.TpmDeviceSelection = PttCurrentState;    
    
    SyncDisplaySpiInformation();
    
    Status = gRT->SetVariable (
                 L"Setup",
                 &SetupGuid,
                 Attributes,
                 SetupVariableSize,
                 &gSetupData
             );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
    }
    
    DEBUG((DEBUG_INFO, "AmiSyncSetupDataEvent Exist\n"));
}

EFI_STATUS
EFIAPI
AmiSyncSetupDataEntry (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
	EFI_STATUS		Status;
	EFI_EVENT       EndOfDxeEvent;

    DEBUG((DEBUG_INFO, "[AmiSyncSetupData.c] AmiSyncSetupData Entry\n"));   

    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    AmiSyncSetupDataEvent,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    ASSERT_EFI_ERROR(Status);    

    return Status;
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
