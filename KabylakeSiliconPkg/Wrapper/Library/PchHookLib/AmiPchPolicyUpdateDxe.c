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
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//*****************************************************************************

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Setup.h>
#include <AmiCspLib.h>


//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------

/**
    UpdateDxePchPlatformPolicy

    @param DxePchPlatformPolicy 
  
    @retval OUT DXE_PCH_PLATFORM_POLICY_PROTOCOL  *DxePchPlatformPolicy
    @retval OUT EFI_SUCCESS
                     
**/
EFI_STATUS
EFIAPI
UpdateDxePchPlatformPolicy (
  IN OUT  DXE_PCH_PLATFORM_POLICY_PROTOCOL  *DxePchPlatformPolicy
  )
/*++
 
Routine Description:
 
  This function installs Dxe System Agent Platform Policy Protocol
  
Arguments:
 
  DxePchPlatformPolicyPtr          The SA Platform Policy protocol instance
 
Returns:
 
  EFI_SUCCESS                     Initialization complete.
  EFI_UNSUPPORTED                 The chipset is unsupported by this driver.
  EFI_OUT_OF_RESOURCES            Do not have enough resources to initialize the driver.
  EFI_DEVICE_ERROR                Device error, driver exits abnormally.
 
--*/
{
  return EFI_SUCCESS;
}
