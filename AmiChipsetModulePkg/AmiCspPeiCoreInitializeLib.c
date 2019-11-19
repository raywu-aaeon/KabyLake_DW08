//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  This file contains library functions for AmiCspPeiCoreInitialize.

**/
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
AmiCspPeiPeiFreePages(
    IN CONST EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PHYSICAL_ADDRESS       Memory,
    IN UINTN                      Pages
)
{
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmiCspPeiCoreInitializeLib(
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_PEI_SERVICES     **ModifyPeiServices;
    
    ModifyPeiServices = (EFI_PEI_SERVICES   **)PeiServices;

    // (*PeiServices)->FreePages is a new function in PI 1.6. 
    // It will be in EFI_PEI_SERVICES because we update to MdePkg_15.
    // Foundation_13 adds this function but we don't have the plan to update this module in KBL.
    // The system will hang if someone calls (*PeiServices)->FreePages.
    // Update the function pointer of (*PeiServices)->FreePages to a null function 
    // to avoid the system might be crashed.
    (*ModifyPeiServices)->FreePages = AmiCspPeiPeiFreePages;
    
    return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

