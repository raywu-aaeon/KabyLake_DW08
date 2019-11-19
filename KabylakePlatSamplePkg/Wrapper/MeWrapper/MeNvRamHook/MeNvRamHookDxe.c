//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#include <Efi.h>
#include <Token.h>
#include <Setup.h>
#include <SetupVariable.h>
#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>

static EFI_GUID   gSetupGuid = SETUP_GUID;


#include <AmiDxeLib.h>

EFI_STATUS DxeGetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data
);

EFI_STATUS DxeSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
);

EFI_STATUS DxeMEGetVariable(
  IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
  OUT UINT32 *Attributes OPTIONAL,
  IN OUT UINTN *DataSize, OUT VOID *Data
)
{
  EFI_STATUS    Status;
      
  if ((!StrCmp (VariableName, L"MeSetup")) &&
      (!guidcmp (VendorGuid, &gMeSetupVariableGuid))) 
  {
    Status = DxeGetVariable(
               L"Setup", 
               &gSetupGuid, 
               Attributes,
               DataSize, 
               Data);
    //DEBUG ((DEBUG_INFO, "DxeMEGetVariable Status = %r.\n", Status));    
    return Status; 
  }

  return EFI_UNSUPPORTED;    
}

EFI_STATUS DxeMESetVariable(
  IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
  IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
  EFI_STATUS    Status;

  if ((!StrCmp (VariableName, L"MeSetup")) &&
      (!guidcmp (VendorGuid, &gMeSetupVariableGuid))) 
  {
    Status  = DxeSetVariable( 
                L"Setup", 
                &gSetupGuid, 
                Attributes, 
                DataSize, 
                Data);
    //DEBUG ((DEBUG_INFO, "DxeMESetVariable Status = %r.\n", Status));    
    return Status;
  }
  
  return EFI_UNSUPPORTED;    
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
