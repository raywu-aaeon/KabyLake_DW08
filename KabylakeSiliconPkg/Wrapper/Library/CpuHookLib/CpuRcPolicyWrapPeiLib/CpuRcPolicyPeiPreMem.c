//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1987-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file  CpuPolicyInitPei.c
	PeiCpuPolicyInitList eLink interface & its default child procedure
	for setting up PEI_CPU_PLATFORM_POLICY_PPI.
**/

#include <Token.h>
#include <Efi.h>
#include <Pei.h>
#include "Setup.h"
#include <Library/DebugLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Txt.h>
#include <Ppi/SiPolicy.h>
#include "CpuRcPolicyPeiPreMem.h"
#include <CpuCspLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CpuPlatformLib.h>

//#define MSR_FLEX_RATIO                  0x194
//#define MSR_TURBO_RATIO_LIMIT           0x1AD
#define MSR_VR_CURRENT_CONFIG           0x601
#ifndef _CPU_REGS_H_
#define MSR_PACKAGE_POWER_LIMIT         0x610
#define MSR_PACKAGE_POWER_SKU           0x614
#endif // _CPU_REGS_H_

// Type Definition(s)
typedef VOID (CPU_POLICY_INIT_FUNC) (  
    IN CONST EFI_PEI_SERVICES   **PeiServices,
	IN SETUP_DATA               *SetupData,
    IN OUT SI_PREMEM_POLICY_PPI *PeiCpuPolicyPpi);

// External Declaration(s)
extern CPU_POLICY_INIT_FUNC PEI_PRE_MEM_CPU_POLICY_INIT_LIST EndOfList;

CPU_POLICY_INIT_FUNC* PeiPreMemCpuPolicyInitListTable[] = \
                                            {PEI_PRE_MEM_CPU_POLICY_INIT_LIST NULL};

/**
    Find the BiosAcm address.

    @param Fv - Firmware Volume

    @retval VOID 0 if not found or the BiosAcm Address.
**/

EFI_PHYSICAL_ADDRESS FindBiosAcmInFv(IN VOID *Fv)
{
    UINT8  *pEndOfFv = (UINT8*)Fv + ((EFI_FIRMWARE_VOLUME_HEADER*)Fv)->FvLength - 1;
    UINT8  *pFfs = (UINT8*)Fv + ((EFI_FIRMWARE_VOLUME_HEADER*)Fv)->HeaderLength;
    UINT32 FfsSize;
    EFI_PHYSICAL_ADDRESS BiosAcmBase;

    //Check for corrupt firmware volume.
    if (((EFI_FIRMWARE_VOLUME_HEADER*)Fv)->Signature != EFI_FVH_SIGNATURE) return 0;
    if (pEndOfFv <= pFfs) return 0;

    while(TRUE) {
        if (pFfs >= pEndOfFv) return 0;

        FfsSize = *(UINT32*)&((EFI_FFS_FILE_HEADER*)pFfs)->Size & 0xffffff;

        //Find BiosAcm file
        if (CompareGuid(&((EFI_FFS_FILE_HEADER*)pFfs)->Name, &gTxtBiosAcmPeiFileGuid))
            break;
        if ((pFfs + FfsSize) <= pFfs) return 0; //Corruption?
        pFfs +=  FfsSize;  //Next file.
        pFfs = (UINT8*)(((UINT32)pFfs + 7) & ~7);  //Align on 8 byte boundary.
    }

    //Find BiosAcm
    BiosAcmBase = (EFI_PHYSICAL_ADDRESS)(UINT32)(pFfs + sizeof(EFI_FFS_FILE_HEADER));
    return BiosAcmBase;
}

/**
    Ami override policy function. 

    @param PeiServices pointer to PEI services
    @param SiCpuPolicyPpi pointer to CPU Pet policy

    @retval EFI_STATUS Return EFI status.
**/

VOID AmiPeiPreMemCpuPolicyInit (	
    IN CONST EFI_PEI_SERVICES   **PeiServices,
	IN SETUP_DATA               *SetupData,
    IN OUT SI_PREMEM_POLICY_PPI *SiPreMemPolicyPpi)
{
  EFI_STATUS                  Status;    
  CPU_SECURITY_PREMEM_CONFIG  *CpuSecurityPreMemConfig;
  CPU_TXT_PREMEM_CONFIG       *CpuTxtPreMemConfig;  
  CPU_CONFIG_LIB_PREMEM_CONFIG	*CpuConfigLibPreMemConfig;
  UINT32	                  RegEax, RegEbx, RegEcx, RegEdx;
  UINT8                       TotLogicalCpus = 0;
  UINT8                       LogicalCpusPerCore = 0;
  UINT8                       CpuSupportedCores = 0;
   
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
  if (EFI_ERROR (Status)){
    ASSERT_EFI_ERROR (Status);
  }
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuTxtPreMemConfigGuid, (VOID *) &CpuTxtPreMemConfig);
  if (EFI_ERROR (Status)){
    ASSERT_EFI_ERROR (Status);  
  }
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuConfigLibPreMemConfigGuid, (VOID *) &CpuConfigLibPreMemConfig);
  if (EFI_ERROR (Status)){
    ASSERT_EFI_ERROR (Status); 
  }

  //
  // TXT Policy overwrite
  //  
  if (SetupData != NULL) {
    CpuSecurityPreMemConfig->ResetAux = SetupData->ResetAux;
    CpuSecurityPreMemConfig->TxtAcheckRequest = 0;
#if INTEL_FIT_SUPPORT
    CpuTxtPreMemConfig->BiosAcmBase = FindBiosAcmInFv((VOID*)FV_DATA_BASE);
#endif
#if TCG_SUPPORT || TCG2Support
    if (SetupData->Txt && SetupData->TpmSupport && SetupData->TpmDeviceSelection == 0) {
      CpuSecurityPreMemConfig->Txt = 1;
      CpuTxtPreMemConfig->TxtDprMemorySize = 0x400000;          
    }
#else
    CpuSecurityPreMemConfig->Txt = 0;
#endif
    
    AsmCpuidEx (0xb, 1, &RegEax, &RegEbx, &RegEcx, &RegEdx);
	TotLogicalCpus  = (UINT8)RegEbx;
	AsmCpuidEx (0xb, 0, &RegEax, &RegEbx, &RegEcx, &RegEdx);
	LogicalCpusPerCore  = (UINT8)RegEbx;
	CpuSupportedCores = TotLogicalCpus / LogicalCpusPerCore;
    
    if (SetupData->ActiveCoreCount != 0 && SetupData->ActiveCoreCount <= CpuSupportedCores)
		CpuConfigLibPreMemConfig->ActiveCoreCount = SetupData->ActiveCoreCount;
	else
		CpuConfigLibPreMemConfig->ActiveCoreCount = 0;
	
	if (CpuSecurityPreMemConfig->Txt)
		CpuConfigLibPreMemConfig->VmxEnable = 1;  // VMX must enable when TXT enable
  }
  
  if (IsSgxSupported()) {
	  if (((CpuSecurityPreMemConfig->EnableSgx == 1) && (CpuSecurityPreMemConfig->PrmrrSize == 0)) || 
		  ((CpuSecurityPreMemConfig->EnableSgx == 1) && (CpuSecurityPreMemConfig->EnableC6Dram) && (CpuSecurityPreMemConfig->PrmrrSize == SIZE_1MB))) {
		  UINT32 PrmrrSupportSize = (LShiftU64(AsmReadMsr64(MSR_PRMRR_VALID_CONFIG), 20)) & 0xE000000;
		  
		  switch (PrmrrSupportSize) {
				case 0xE000000: 
				    CpuSecurityPreMemConfig->PrmrrSize = 0x8000000;
					break;
				case 0x6000000: 
				    CpuSecurityPreMemConfig->PrmrrSize = 0x4000000;
					break;
				default:
				    CpuSecurityPreMemConfig->PrmrrSize = 0x2000000;
					break;
		  }
	  }
  }
}

/**
    This function will call CPU PEI pre-memory policy override function. 

    @param PeiServices pointer to PEI services
    @param SiCpuPolicyPpi pointer to CPU Pet policy

    @retval EFI_STATUS Return EFI status.
**/

EFI_STATUS
EFIAPI
UpdatePeiPreMemCpuPlatformPolicy (
  IN OUT  SI_PREMEM_POLICY_PPI *SiPreMemPolicyPpi
  )
{
  UINTN                       i;
  EFI_STATUS                  Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *ReadOnlyVariable; 
  EFI_GUID                    gEfiSetupGuid = SETUP_GUID;
  SETUP_DATA                  SetupDataPtr;
  SETUP_DATA                  *SetupData = NULL;
  UINTN                       VariableSize= sizeof (SETUP_DATA);
  CONST EFI_PEI_SERVICES      **PeiServices;
  
  PeiServices = GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi (
      	                       PeiServices,
      	                       &gEfiPeiReadOnlyVariable2PpiGuid,
      	                       0,
      	                       NULL,
      	                       (VOID **) &ReadOnlyVariable
      	                       );

  if (!EFI_ERROR(Status)) {
    Status = ReadOnlyVariable->GetVariable (
      	                       ReadOnlyVariable,
      	    	    		   L"Setup",
      	                       &gEfiSetupGuid,
      	                       NULL,
      	                       &VariableSize,
      	                       &SetupDataPtr
      	                       );
  }

  if(!EFI_ERROR(Status)) SetupData = &SetupDataPtr;

  for (i = 0; PeiPreMemCpuPolicyInitListTable[i] != NULL; i++) 
	PeiPreMemCpuPolicyInitListTable[i](PeiServices, SetupData, SiPreMemPolicyPpi);

  return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1987-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
