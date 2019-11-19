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
#include <Library/PeiServicesLib.h>
#include <Guid/MemoryTypeInformation.h>
#include <Guid/AmiGlobalVariable.h>
#include <Library/HobLib.h>
///
/// Memory Reserved should be between 125% to 150% of the Current required memory
/// otherwise BdsMisc.c would do a reset to make it 125% to avoid s4 resume issues.
///
GLOBAL_REMOVE_IF_UNREFERENCED EFI_MEMORY_TYPE_INFORMATION mDefaultMemoryTypeInformation[] = {
  { EfiACPIReclaimMemory, 0x65 },       // ASL
  { EfiACPIMemoryNVS, 0x400 },          // ACPI NVS (including S3 related)
#ifndef MDEPKG_NDEBUG
  { EfiReservedMemoryType, 0x188B },    // BIOS Reserved (including S3 related)
#else  // MDEPKG_NDEBUG
  { EfiReservedMemoryType, 0x402 },     // BIOS Reserved (including S3 related)
#endif // MDEPKG_NDEBUG
  { EfiRuntimeServicesData, 0x500 },    // Runtime Service Data
#ifndef MDEPKG_NDEBUG
  { EfiRuntimeServicesCode, 0x80 },     // Runtime Service Code
#else
  { EfiRuntimeServicesCode, 0x25 },     // Runtime Service Code
#endif
  { EfiBootServicesCode,    0x600 },    // Please refer AmiDxeIpl.c
  { EfiBootServicesData,    0x1500 },   // Please refer AmiDxeIpl.c
  { EfiMaxMemoryType, 0 }
};

EFI_STATUS PeiSaGetVariable (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VendorGuid,
  OUT UINT32 *Attributes, IN OUT UINTN *DataSize, OUT VOID *Data
)
{
  EFI_STATUS    Status;
  
  if ((!StrCmp (VariableName, L"SaSetup")) &&
      (!guidcmp ((EFI_GUID *) VendorGuid, &gSaSetupVariableGuid))) 
  { 
    Status = This->GetVariable (
                     This,
                     L"Setup", 
                     &gSetupGuid, 
                     Attributes,
                     DataSize, 
                     Data
                     );
  
    return Status;   
  }
  
  if ((!StrCmp (VariableName, EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME)) &&
      (!guidcmp ((EFI_GUID *) VendorGuid, &gEfiMemoryTypeInformationGuid))) 
  { 
	CONST EFI_PEI_SERVICES  **PeiServices;
	EFI_BOOT_MODE          BootMode;
	  
	PeiServices = GetPeiServicesTablePointer ();
	PeiServicesGetBootMode (&BootMode);
	
    Status = This->GetVariable (
                     This,
                     (BootMode == BOOT_ON_S4_RESUME)
                     ? L"PreviousMemoryTypeInformation" 
                     : EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME, 
                     &gAmiGlobalVariableGuid, 
                     Attributes,
                     DataSize, 
                     Data
                     );
    if (EFI_ERROR (Status)) {
    	EFI_PEI_HOB_POINTERS  Hob;
        Hob.Raw = GetFirstHob (EFI_HOB_TYPE_UNUSED);
        while ((Hob.Raw != NULL) && (!END_OF_HOB_LIST (Hob))) {
          if (!guidcmp (&gEfiMemoryTypeInformationGuid, &Hob.Guid->Name)) {
             return Status;
          }
          Hob.Raw = GET_NEXT_HOB (Hob);
          Hob.Raw = GetNextHob (EFI_HOB_TYPE_UNUSED, Hob.Raw);
        }
       *DataSize = sizeof (mDefaultMemoryTypeInformation);
       (*PeiServices)->CopyMem (Data, mDefaultMemoryTypeInformation, *DataSize);
       return EFI_SUCCESS;
    } else { 
    	return Status;   
    }
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
   
EFI_STATUS DxeSaGetVariable(
  IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
  OUT UINT32 *Attributes OPTIONAL,
  IN OUT UINTN *DataSize, OUT VOID *Data
)
{
  EFI_STATUS    Status;
      
  if ((!StrCmp (VariableName, L"SaSetup")) &&
      (!guidcmp (VendorGuid, &gSaSetupVariableGuid))) 
  { 
    Status = DxeGetVariable(
               L"Setup", 
               &gSetupGuid, 
               Attributes,
               DataSize, 
               Data);
    //DEBUG ((DEBUG_INFO, "DxeSaGetVariable Status = %r.\n", Status));    
    return Status; 
  }
  
  return EFI_UNSUPPORTED;    
}

EFI_STATUS DxeSaSetVariable(
  IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
  IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
  EFI_STATUS    Status;
  
  if ((!StrCmp (VariableName, L"SaSetup")) &&
      (!guidcmp (VendorGuid, &gSaSetupVariableGuid))) 
  {
    Status  = DxeSetVariable( 
                L"Setup", 
                &gSetupGuid, 
                Attributes, 
                DataSize, 
                Data);
    DEBUG ((DEBUG_INFO, "DxeSaSetVariable Status = %r.\n", Status));    
    return Status;
  }

  return EFI_UNSUPPORTED;
}
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
