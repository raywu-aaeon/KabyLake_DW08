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
#include <AmiCspLib.h>
#include <SetupVariable.h>
#include <Library/DebugLib.h>

static EFI_GUID   gSetupGuid = SETUP_GUID;

#ifdef PEI_BUILD
#include <AmiPeiLib.h>
#include <Pei.h>
#include <Library/PeiServicesTablePointerLib.h>

EFI_STATUS PeiPchGetVariable(
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VendorGuid,
  OUT UINT32 *Attributes, IN OUT UINTN *DataSize, OUT VOID *Data
)
{
  EFI_STATUS    Status;
    
  if ((!StrCmp (VariableName, L"PchSetup")) &&
      (!guidcmp ((EFI_GUID *) VendorGuid, &gPchSetupVariableGuid))) 
  {
    Status = This->GetVariable (
                     This,
                     L"Setup", 
                     &gSetupGuid, 
                     Attributes,
                     DataSize, 
                     Data
                     );
    //DEBUG ((DEBUG_INFO, GetPeiServicesTablePointer(), "PeiPchGetVariable Status = %r.\n", Status));     
    return Status;   
  }
   
  return EFI_UNSUPPORTED;
}

#else //#ifdef PEI_BUILD

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
   
EFI_STATUS DxePchGetVariable(
  IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
  OUT UINT32 *Attributes OPTIONAL,
  IN OUT UINTN *DataSize, OUT VOID *Data
)
{
  EFI_STATUS    Status;
      
  if ((!StrCmp (VariableName, L"PchSetup")) &&
      (!guidcmp (VendorGuid, &gPchSetupVariableGuid))) 
  {
    Status = DxeGetVariable(
               L"Setup", 
               &gSetupGuid, 
               Attributes,
               DataSize, 
               Data);
    DEBUG ((DEBUG_INFO, "DxePchGetVariable Status = %r.\n", Status));    
    return Status; 
  }

  return EFI_UNSUPPORTED;    
}

#if defined SB_SETUP_SUPPORT && SB_SETUP_SUPPORT == 1

EFI_STATUS DxePchSetVariable(
  IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
  IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
  EFI_STATUS    Status;
  
  if ((!StrCmp (VariableName, L"PchSetup")) &&
      (!guidcmp (VendorGuid, &gPchSetupVariableGuid))) 
  {
    Status  = DxeSetVariable( 
                L"Setup", 
                &gSetupGuid, 
                Attributes, 
                DataSize, 
                Data);
    //DEBUG ((DEBUG_INFO, "DxePchSetVariable Status = %r.\n", Status));    
    return Status;
  }
  
  return EFI_UNSUPPORTED;    
}
#endif
#endif // #ifdef PEI_BUILD
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
