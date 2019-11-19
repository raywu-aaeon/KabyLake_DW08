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

/** @file PowerButton.c
    Provide functions to register and handle Powerbutton
    functionality.

**/

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Token.h>
#include <AmiCspLib.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <PchAccess.h>
#include <Library/GpioLib.h>
#include <Library/DebugLib.h>

EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT	DispatchContext = {EfiPowerButtonEntry};
EFI_SMM_BASE2_PROTOCOL                  *pSmmBase2;
EFI_SMM_SYSTEM_TABLE2                   *pSmst2;

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

/**
    If the power button is pressed, then this function is called.

        
    @param DispatchHandle 
    @param DispatchContext 

    @retval VOID

**/
EFI_STATUS
EFIAPI
SbPwrBtnHandler(
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL
)
{   
    DEBUG((DEBUG_INFO, "PowerButtonActivated Start\n"));
    
#if defined(USB_S5_WAKEUP_SUPPORT) && (USB_S5_WAKEUP_SUPPORT == 1)    
    UsbSbEnablePme();
#endif 
    
    DEBUG((DEBUG_INFO, "PowerButtonActivated End\n"));
    return EFI_SUCCESS;
}

/**
    Installs South Bridge Power Button SMI Handler

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/

EFI_STATUS InSmmFunction (
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
)
{
    EFI_STATUS                              Status;
    EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *PowerButton;
    EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT   DispatchContext = {EfiPowerButtonEntry};
    EFI_HANDLE                              Handle = NULL;

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;

    Status = pSmmBase2->GetSmstLocation( pSmmBase2, &pSmst2 );
    if (EFI_ERROR(Status)) return Status;

    Status = pSmst2->SmmLocateProtocol(
                      &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                      NULL,
                      (VOID **)&PowerButton
                      );

    if (EFI_ERROR(Status)) return Status;

    Status = PowerButton->Register (
                            PowerButton,
                            SbPwrBtnHandler,
                            &DispatchContext,
                            &Handle
                            );

    return Status;
}

/**
    This is the entrypoint of the Power button driver.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/
EFI_STATUS EFIAPI PowerButtonEntry(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS    Status;

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&pSmmBase2);
    if (EFI_ERROR(Status)) return Status;

    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
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
