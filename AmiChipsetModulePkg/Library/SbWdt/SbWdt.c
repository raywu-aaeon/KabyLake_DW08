//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
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

/** @file SbWdt.c
    SB Invokes Intel PCH Watch Dog Timer PPI/Protocol.

**/

#include <Library/DebugLib.h>
#include <AmiCspLib.h>
#include <Protocol/Wdt.h>
#include <Ppi/Wdt.h>
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

//---------------------------------------------------------------------------

/**
    This function invokes WdtReloadAndStart()

    @param *Services         - Pointer to PeiServices or BootServices
        structure  
    @param TimeoutValue      - Time in seconds before WDT times out.
        Supported range = 1 - 1024.
    @param Pei               - Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
EFI_STATUS AmiWdtReloadAndStart
(
  IN  VOID    *Services,
  IN  UINT32  TimeoutValue,
  IN  BOOLEAN Pei
)
{
  EFI_STATUS         Status;
  EFI_PEI_SERVICES   **PeiServices;
  EFI_BOOT_SERVICES  *BootServices;
  WDT_PPI            *WdtPpi;
  WDT_PROTOCOL       *WdtProtocol;
  
  if (Pei) {
    PeiServices = (EFI_PEI_SERVICES **)Services;
    Status = (*PeiServices)->LocatePpi (PeiServices, &gWdtPpiGuid, 0, NULL, (VOID **)&WdtPpi);
    ASSERT_EFI_ERROR (Status);
    return WdtPpi->ReloadAndStart (TimeoutValue);
  } else {
    BootServices = (EFI_BOOT_SERVICES *)Services;
    Status = BootServices->LocateProtocol(&gWdtProtocolGuid, NULL, (VOID **)&WdtProtocol);
    ASSERT_EFI_ERROR (Status);
    return WdtProtocol->ReloadAndStart (TimeoutValue);
  }
}

/**
    This function invokes WdtCheckStatus()

    @param *Services         - Pointer to PeiServices or BootServices
        structure  
    @param Pei               - Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
UINT8 AmiWdtCheckStatus
(
  IN     VOID       *Services,
  IN     BOOLEAN    Pei,
  IN OUT EFI_STATUS *Status
)
{
  EFI_PEI_SERVICES   **PeiServices;
  EFI_BOOT_SERVICES  *BootServices;
  WDT_PPI            *WdtPpi;
  WDT_PROTOCOL       *WdtProtocol;
  
  if (Pei) {
    PeiServices = (EFI_PEI_SERVICES **)Services;
    *Status = (*PeiServices)->LocatePpi (PeiServices, &gWdtPpiGuid, 0, NULL, (VOID **)&WdtPpi);
    ASSERT_EFI_ERROR (*Status);
    return WdtPpi->CheckStatus();
  } else {
    BootServices = (EFI_BOOT_SERVICES *)Services;
    *Status = BootServices->LocateProtocol(&gWdtProtocolGuid, NULL, (VOID **)&WdtProtocol);
    ASSERT_EFI_ERROR (*Status);	
    return WdtProtocol->CheckStatus();
  }
}

/**
    This function invokes WdtDisable()

    @param *Services         - Pointer to PeiServices or BootServices
        structure  
    @param Pei               - Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
EFI_STATUS AmiWdtDisable
(
  IN  VOID    *Services,
  IN  BOOLEAN Pei
)
{
  EFI_STATUS         Status = EFI_SUCCESS;
  EFI_PEI_SERVICES   **PeiServices;
  EFI_BOOT_SERVICES  *BootServices;
  WDT_PPI            *WdtPpi;
  WDT_PROTOCOL       *WdtProtocol;
  
  if (Pei) {
    PeiServices = (EFI_PEI_SERVICES **)Services;
    Status = (*PeiServices)->LocatePpi (PeiServices, &gWdtPpiGuid, 0, NULL, (VOID **)&WdtPpi);
	if ( !EFI_ERROR(Status) )  {
        WdtPpi->Disable();
    } else {
        ASSERT_EFI_ERROR (Status);	
	}	
  } else {
    BootServices = (EFI_BOOT_SERVICES *)Services;
    Status = BootServices->LocateProtocol(&gWdtProtocolGuid, NULL, (VOID **)&WdtProtocol);
	if ( !EFI_ERROR(Status) )  {
        WdtProtocol->Disable();
    } else {
        ASSERT_EFI_ERROR (Status);	
	}	
  }
  
  return Status;
}

/**
    This function invokes WdtAllowKnownReset()

    @param *Services         - Pointer to PeiServices or BootServices
        structure  
    @param Pei               - Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
EFI_STATUS AmiWdtAllowKnownReset
(
  IN  VOID    *Services,
  IN  BOOLEAN Pei
)
{
  EFI_STATUS         Status;
  EFI_PEI_SERVICES   **PeiServices;
  EFI_BOOT_SERVICES  *BootServices;
  WDT_PPI            *WdtPpi;
  WDT_PROTOCOL       *WdtProtocol;
  
  if (Pei) {
    PeiServices = (EFI_PEI_SERVICES **)Services;
    Status = (*PeiServices)->LocatePpi (PeiServices, &gWdtPpiGuid, 0, NULL, (VOID **)&WdtPpi);
    ASSERT_EFI_ERROR (Status);
    if ( !EFI_ERROR(Status) )  {
        WdtPpi->AllowKnownReset();
    } else {
        ASSERT_EFI_ERROR (Status);	
    }
  } else {
    BootServices = (EFI_BOOT_SERVICES *)Services;
    Status = BootServices->LocateProtocol(&gWdtProtocolGuid, NULL, (VOID **)&WdtProtocol);
    if ( !EFI_ERROR(Status) )  {
        WdtProtocol->AllowKnownReset();
    } else {
        ASSERT_EFI_ERROR (Status);	
    }
  }
  
  return Status;
}

/**
    This function invokes IsWdtRequired()

    @param *Services         - Pointer to PeiServices or BootServices
        structure  
    @param Pei               - Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
UINT8 AmiIsWdtRequired
(
  IN      VOID       *Services,
  IN      BOOLEAN    Pei,
  IN  OUT EFI_STATUS *Status
)
{
  EFI_PEI_SERVICES   **PeiServices;
  EFI_BOOT_SERVICES  *BootServices;
  WDT_PPI            *WdtPpi;
  WDT_PROTOCOL       *WdtProtocol;
  
  if (Pei) {
    PeiServices = (EFI_PEI_SERVICES **)Services;
    *Status = (*PeiServices)->LocatePpi (PeiServices, &gWdtPpiGuid, 0, NULL, (VOID **)&WdtPpi);
    if ( !EFI_ERROR(*Status) )  {
      return WdtPpi->IsWdtRequired();
    } else {
      ASSERT_EFI_ERROR (*Status);
    }
  } else {
    BootServices = (EFI_BOOT_SERVICES *)Services;
    *Status = BootServices->LocateProtocol(&gWdtProtocolGuid, NULL, (VOID **)&WdtProtocol);
    if ( !EFI_ERROR(*Status) )  {
      return WdtProtocol->IsWdtRequired();
    } else {
      ASSERT_EFI_ERROR (*Status);
    }
  }
  
  return 0;
}

/**
    This function invokes IsWdtEnabled()

    @param *Services         - Pointer to PeiServices or BootServices
        structure  
    @param Pei               - Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
UINT8 AmiIsWdtEnabled (
  IN      VOID       *Services,
  IN      BOOLEAN    Pei,
  IN  OUT EFI_STATUS *Status
)
{
  EFI_PEI_SERVICES   **PeiServices;
  EFI_BOOT_SERVICES  *BootServices;
  WDT_PPI            *WdtPpi;
  WDT_PROTOCOL       *WdtProtocol;
  
  if (Pei) {
    PeiServices = (EFI_PEI_SERVICES **)Services;
    *Status = (*PeiServices)->LocatePpi (PeiServices, &gWdtPpiGuid, 0, NULL, (VOID **)&WdtPpi);
    ASSERT_EFI_ERROR (*Status);
    return WdtPpi->IsWdtEnabled();
  } else {
    BootServices = (EFI_BOOT_SERVICES *)Services;
    *Status = BootServices->LocateProtocol(&gWdtProtocolGuid, NULL, (VOID **)&WdtProtocol);
    ASSERT_EFI_ERROR (*Status);
    return WdtProtocol->IsWdtEnabled();
  }
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
