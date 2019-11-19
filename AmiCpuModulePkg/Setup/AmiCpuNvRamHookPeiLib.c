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

/** @file AmiCpuNvRamHookPeiLib.c
    Cpu hook function in PeiGetVariableHook Elink.

**/

#include <Efi.h>
#include <Token.h>
#include <Setup.h>
#include <SetupVariable.h>
#include <AmiPeiLib.h>
#include <Pei.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>

static EFI_GUID  gSetupGuid = SETUP_GUID;

/**
    This function will hook get CpuSetup variable.
 
    @param This Pointer to Read only variable2 ppi service.
	@param VariableName String name
	@param VendorGuid Guid
	@param Attributes Attributes
	@param DataSize Data Size
	@param Data Pointer to data buffer

    @retval EFI_STATUS
**/

EFI_STATUS PeiCpuGetVariable(
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VendorGuid,
  OUT UINT32 *Attributes, IN OUT UINTN *DataSize, OUT VOID *Data
)
{
  EFI_STATUS    Status;
    
  if ((!StrCmp (VariableName, L"CpuSetup")) &&
      (!guidcmp (VendorGuid, &gCpuSetupVariableGuid))) 
  {
    Status = This->GetVariable (
                     This,
                     L"Setup", 
                     &gSetupGuid, 
                     Attributes,
                     DataSize, 
                     Data
                     );
    //DEBUG ((DEBUG_INFO, "PeiCpuGetVariable Status = %r.\n", Status));     
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
