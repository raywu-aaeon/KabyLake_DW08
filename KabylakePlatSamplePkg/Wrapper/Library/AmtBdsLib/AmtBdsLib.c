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

//**********************************************************************
/** @file AmtBdsLib.c

**/
//**********************************************************************
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <AmiDxeLib.h>
#include <Protocol/ActiveManagement.h>

//----------------------------------------------------------------------------
// Function Externs
ACTIVE_MANAGEMENT_PROTOCOL      *mActiveManagement = NULL;
EFI_INSTALL_PROTOCOL_INTERFACE  gSavedInstallProtocol;

VOID InstallConInStartedProtocol(VOID);
extern EFI_GUID ConInStartedProtocolGuid;

//----------------------------------------------------------------------------
// Local prototypes
/**
    Save Protocol Interface
**/
EFI_STATUS
EFIAPI
PrivateInstallProtocolInterface (
  IN OUT EFI_HANDLE     *UserHandle,
  IN EFI_GUID           *Protocol,
  IN EFI_INTERFACE_TYPE InterfaceType,
  IN VOID               *Interface
  )
{
  if (!guidcmp(Protocol, &ConInStartedProtocolGuid)) return EFI_SUCCESS;
  return gSavedInstallProtocol(UserHandle, Protocol, InterfaceType, Interface);
}

/**
    Check Sol Status

    @param VOID

        BOOLEAN
    @retval TRUE SOL Enable
    @retval FALSE SOL Disable

**/
BOOLEAN IsSolEnabled (VOID) 
{
  EFI_STATUS          Status;    
  static BOOLEAN      CurrentState = 0xEE;
  
  if (mActiveManagement == NULL) {
      Status = pBS->LocateProtocol (&gActiveManagementProtocolGuid, \
                                      NULL, (VOID **) &mActiveManagement);
      if (EFI_ERROR(Status)) return FALSE;                                
  }
  
  if (CurrentState == 0xEE) 
      mActiveManagement->GetSolState (mActiveManagement, &CurrentState);
  
  return CurrentState;
}

/**
    if USER Password enabled with SOL enabled, the Password is checked if 
    ConOut,ConIn and ConsoleControll Protocols are instatlled, but 
    when the Terminal and PciSerial are not connected yet, so, here we 
    hooks the pBS->InstallProtocolInterface to postpone the 
    ConInStartedProtocol to be installed after "ConnectEverything" hook.

    @param VOID

    @retval VOID

**/
VOID AmtSolBeforeConInProtocolHook() 
{
  if (IsSolEnabled() == FALSE) return;
  
  // if USER Password enabled with SOL enabled, the Password is checked if 
  // ConOut,ConIn and ConsoleControll Protocols are instatlled, but 
  // when the Terminal and PciSerial are not connected yet, so, here we 
  // hooks the pBS->InstallProtocolInterface to postpone the 
  // ConInStartedProtocol to be installed after "ConnectEverything" hook.
  gSavedInstallProtocol = pBS->InstallProtocolInterface;
  pBS->InstallProtocolInterface = PrivateInstallProtocolInterface;
}

/**

    @param VOID

    @retval VOID

**/
VOID AmtSolAfterDriverConnectedHook() 
{
  if (IsSolEnabled() == FALSE) return;
  
  pBS->InstallProtocolInterface = gSavedInstallProtocol;
  InstallConInStartedProtocol();
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
