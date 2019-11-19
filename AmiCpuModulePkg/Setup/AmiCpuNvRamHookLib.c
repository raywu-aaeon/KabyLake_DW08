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

/** @file AmiCpuNvRamHookLib.c
    Cpu hook function in GetVariableHook Elink.

**/

#include <Efi.h>
#include <Token.h>
#include <Setup.h>
#include <SetupVariable.h>
#include <AmiDxeLib.h>
#include <Library/DebugLib.h>

static EFI_GUID  gSetupGuid = SETUP_GUID;

EFI_STATUS DxeGetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data
);

EFI_STATUS DxeSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
);

/**
    This function will hook get CpuSetup variable.

	@param VariableName String name
	@param VendorGuid Guid
	@param Attributes Attributes
	@param DataSize Data Size
	@param Data Pointer to data buffer

    @retval EFI_STATUS
**/

EFI_STATUS DxeCpuGetVariable(
  IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
  OUT UINT32 *Attributes OPTIONAL,
  IN OUT UINTN *DataSize, OUT VOID *Data
)
{
  EFI_STATUS    Status;
      
  if ((!StrCmp (VariableName, L"CpuSetup")) &&
      (!guidcmp (VendorGuid, &gCpuSetupVariableGuid))) 
  {
    Status = DxeGetVariable(
               L"Setup", 
               &gSetupGuid, 
               Attributes,
               DataSize, 
               Data);
    //DEBUG ((DEBUG_INFO, "DxeCpuGetVariable Status = %r.\n", Status));    
    return Status; 
  }

  return EFI_UNSUPPORTED;    
}

EFI_STATUS DxeCpuSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
  EFI_STATUS    Status;
    
  if ((!StrCmp (VariableName, L"CpuSetup")) &&
      (!guidcmp (VendorGuid, &gCpuSetupVariableGuid))) 
  {
    Status  = DxeSetVariable( 
                L"Setup", 
                &gSetupGuid, 
                Attributes, 
                DataSize, 
                Data);
    //DEBUG ((DEBUG_INFO, "DxeCpuSetVariable Status = %r.\n", Status));    
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
