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
#include <AmiPeiLib.h>
#include "CpuRcPolicyPei.h"
#include <Library/PeiServicesTablePointerLib.h>
#include <CpuCspLib.h>
#include <Ppi/SiPolicy.h>

#define AMI_SGX_SINIT_DATA_GUID \
    { 0x51f30caa, 0x6f0b, 0x4460, 0x8a, 0x67, 0x91, 0x93, 0xf0, 0xf7, 0xd6,\
      0xf3}

#define MSR_VR_CURRENT_CONFIG           0x601
#ifndef _CPU_REGS_H_
#define MSR_PACKAGE_POWER_LIMIT         0x610
#define MSR_PACKAGE_POWER_SKU           0x614
#endif // _CPU_REGS_H_
// AMI_UP_SERVER_OVERRIDE >>>
#ifdef ZUMBA_BEACH_SUPPORT
#define UpNextHob(Hob,Type) ((Type *) ((UINT8 *) Hob + ((EFI_HOB_GENERIC_HEADER *)Hob)->HobLength))
#endif
// AMI_UP_SERVER_OVERRIDE <<<

// Type Definition(s)
typedef VOID (CPU_POLICY_INIT_FUNC) (  
    IN CONST EFI_PEI_SERVICES   **PeiServices,
	IN SETUP_DATA               *SetupData,
    IN OUT SI_POLICY_PPI        *PeiCpuPolicyPpi);

// External Declaration(s)
extern CPU_POLICY_INIT_FUNC PEI_CPU_POLICY_INIT_LIST EndOfList;

CPU_POLICY_INIT_FUNC* PeiCpuPolicyInitListTable[] = \
                                            {PEI_CPU_POLICY_INIT_LIST NULL};

typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
	UINT32  uCodeAddr;
} AMI_INTERNAL_UCODE_HOB;

typedef struct {
	EFI_HOB_GUID_TYPE   EfiHobGuidType;
	UINT8  SgxSinitDataFromTpm;           ///< SGX SINIT Data from TPM
} SGX_SINIT_DATA_FROM_TPM;

/**
  Get the microcode patch pointer.
  
  @param gMicrocodeStart Microcode address.

  @retval EFI_PHYSICAL_ADDRESS - Address of the microcode patch, or NULL if not found.
**/

EFI_PHYSICAL_ADDRESS
AmiPlatformCpuRetrieveMicrocode (
    IN UINT32 gMicrocodeStart
)
{
    if ((gMicrocodeStart != 0) && (gMicrocodeStart != 0xFFFF)) {	        
    	return (EFI_PHYSICAL_ADDRESS) (UINTN) gMicrocodeStart;
    } else{
        return (EFI_PHYSICAL_ADDRESS) (UINTN) NULL;
    }
}

/**
    Ami override policy function. 

    @param PeiServices pointer to PEI services
    @param SiPolicyPpi pointer to CPU Pet policy

    @retval EFI_STATUS Return EFI status.
**/

VOID AmiPeiCpuPolicyInit (	
    IN CONST EFI_PEI_SERVICES   **PeiServices,
	IN SETUP_DATA               *SetupData,
    IN OUT SI_POLICY_PPI        *SiPolicyPpi )
{
  EFI_STATUS                  Status;
  UINT64                      TempMsr;
  AMI_CPU_INTERNAL_INFO_HOB   *CpuInternalHob;
  VOID        				  *FirstHob;
  AMI_INTERNAL_UCODE_HOB      *uCodeHob = NULL;
  UINT32 					  gMicrocodeStart = 0;
  EFI_GUID 					  gAmiCpuInternalInfoHobGuid = AMI_CPU_INTERNAL_INFO_HOB_GUID;
  EFI_GUID 				 	  gAmiInternaluCodeHobGuid = AMI_INTERNAL_UCODE_HOB_GUID;
  EFI_GUID            		  gSgxSinitDataGuid = AMI_SGX_SINIT_DATA_GUID;
  SI_CONFIG                   *SiConfig;
// AMI_UP_SERVER_OVERRIDE >>>
#ifdef ZUMBA_BEACH_SUPPORT
  EFI_BOOT_MODE               BootMode;
  EFI_HOB_GENERIC_HEADER      *ThisHob;
#endif
// AMI_UP_SERVER_OVERRIDE <<<
  SGX_SINIT_DATA_FROM_TPM 	  *SgxSinitDataFromTpm = NULL;
  CPU_CONFIG                  *CpuConfig;
  CPU_POWER_MGMT_BASIC_CONFIG *CpuPowerMgmtBasicConfig;
  CPU_SGX_CONFIG              *CpuSgxConfig;
  SI_PREMEM_POLICY_PPI        *SiPreMemPolicy;

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuConfigGuid, (VOID *) &CpuConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuPowerMgmtBasicConfigGuid, (VOID *) &CpuPowerMgmtBasicConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gCpuSgxConfigGuid, (VOID *) &CpuSgxConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSiConfigGuid, (VOID *) &SiConfig);
  ASSERT_EFI_ERROR (Status);
  
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gSiPreMemPolicyPpiGuid,
                             0,
                             NULL,
                             (VOID **) &SiPreMemPolicy
                             );  
  ASSERT_EFI_ERROR (Status);

  //
  // Create Cpu internal HOB
  //
  Status = (*PeiServices)->CreateHob(
            PeiServices,
            EFI_HOB_TYPE_GUID_EXTENSION,
            sizeof(AMI_CPU_INTERNAL_INFO_HOB),
            (VOID **) &CpuInternalHob
        );

  CpuInternalHob->EfiHobGuidType.Name = gAmiCpuInternalInfoHobGuid;
  CpuInternalHob->Revision = 1;

  TempMsr = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);

  CpuInternalHob->FactoryTdc = (UINT16)(RShiftU64 (TempMsr, 32) & 0x7fff);
  CpuInternalHob->FactoryTdp = (UINT16)(TempMsr & 0x7fff);
  CpuInternalHob->FactoryTdpLimitTime = (UINT8)(RShiftU64 (TempMsr, 17) & 0x7f);

  TempMsr = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU);
  CpuInternalHob->FactoryPowerConv = 1 << ((UINT8)(TempMsr & 0xf));
  CpuInternalHob->FactoryTimeConv = RShiftU64(TempMsr, 16) & 0xf ;   // 1 / (2 ^ TIME_UINT)

  TempMsr = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  CpuInternalHob->FactoryOneCoreRatioLimit = (UINT8)TempMsr;
  CpuInternalHob->FactoryTwoCoreRatioLimit = (UINT8)(RShiftU64 (TempMsr, 8));
  CpuInternalHob->FactoryThreeCoreRatioLimit = (UINT8)(RShiftU64 (TempMsr, 16));
  CpuInternalHob->FactoryFourCoreRatioLimit = (UINT8)(RShiftU64 (TempMsr, 24));
  CpuInternalHob->FactoryFiveCoreRatioLimit = (UINT8)(RShiftU64 (TempMsr, 32));
  CpuInternalHob->FactorySixCoreRatioLimit = (UINT8)(RShiftU64 (TempMsr, 40));
  CpuInternalHob->FactoryIaCoreCurrentMax = (UINT16)(AsmReadMsr64(MSR_VR_CURRENT_CONFIG) & 0xfff);
  CpuInternalHob->AesAvailable = IsAesSupported();
  
  (*PeiServices)->GetHobList(PeiServices, &FirstHob);
  ASSERT(FirstHob != NULL);
  
// AMI_UP_SERVER_OVERRIDE >>>
#ifdef ZUMBA_BEACH_SUPPORT
  Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
  if(BootMode == BOOT_ON_S3_RESUME) {
     Status = EFI_NOT_FOUND;
     ThisHob = FirstHob;
     while(ThisHob->HobType != EFI_HOB_TYPE_END_OF_HOB_LIST) {
        ThisHob = UpNextHob(ThisHob,EFI_HOB_GENERIC_HEADER);
	    if (ThisHob->HobType==EFI_HOB_TYPE_GUID_EXTENSION) {
          if (guidcmp(&((EFI_HOB_GUID_TYPE*)uCodeHob)->Name, &gAmiInternaluCodeHobGuid) == 0) {
              (VOID*)uCodeHob = ThisHob;
              Status = EFI_SUCCESS;
              break;
          }
        }
     }      
  } else {
#endif
    uCodeHob = (AMI_INTERNAL_UCODE_HOB *)FirstHob;
    while (!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, (VOID **) &uCodeHob))) {
    	if (guidcmp(&((EFI_HOB_GUID_TYPE*)uCodeHob)->Name, &gAmiInternaluCodeHobGuid) == 0) {
    		break;
    	}
#ifdef ZUMBA_BEACH_SUPPORT
    }
#endif
  }
// AMI_UP_SERVER_OVERRIDE <<< 

  if (!EFI_ERROR(Status) && (uCodeHob != NULL)) {
	gMicrocodeStart = uCodeHob->uCodeAddr;
	// uCode in memory is already found, change the RetriveveMicrocode funciton      
	CpuConfig->MicrocodePatchAddress  = AmiPlatformCpuRetrieveMicrocode(gMicrocodeStart);
  }

  CpuConfig->SmmbaseSwSmiNumber	= SMM_FROM_SMBASE_DRIVER;

  // CPU policy configure by setup questions
  if (SetupData != NULL) {
    //Do not set ActiveCoreCount more than CPU supported Cores
  }
  
  (*PeiServices)->GetHobList(PeiServices, &FirstHob);
  ASSERT(FirstHob != NULL);
		
// AMI_UP_SERVER_OVERRIDE >>>
#ifdef ZUMBA_BEACH_SUPPORT
  Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
  if(BootMode == BOOT_ON_S3_RESUME) {
	Status = EFI_NOT_FOUND;
	ThisHob = FirstHob;
	while(ThisHob->HobType != EFI_HOB_TYPE_END_OF_HOB_LIST) {
	  ThisHob = UpNextHob(ThisHob,EFI_HOB_GENERIC_HEADER);
	  if (ThisHob->HobType==EFI_HOB_TYPE_GUID_EXTENSION) {
		if (guidcmp(&((EFI_HOB_GUID_TYPE*)SgxSinitDataFromTpm)->Name, &gSgxSinitDataGuid) == 0) {
		  (VOID*)SgxSinitDataFromTpm = ThisHob;
		  Status = EFI_SUCCESS;
		  break;
		}
	  }
	}      
  } else {
#endif
	SgxSinitDataFromTpm = (SGX_SINIT_DATA_FROM_TPM *) FirstHob;
	while (!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, (VOID **) &SgxSinitDataFromTpm))) {
	  if (guidcmp(&((EFI_HOB_GUID_TYPE*)SgxSinitDataFromTpm)->Name, &gSgxSinitDataGuid) == 0) {
		break;
	  }
#ifdef ZUMBA_BEACH_SUPPORT
	}
#endif
  }

  if ( !EFI_ERROR(Status) ) {
    CpuSgxConfig->SgxSinitDataFromTpm = SgxSinitDataFromTpm->SgxSinitDataFromTpm;
//    DEBUG ((DEBUG_INFO, "SgxSinitDataFromTpm = %X\n", SgxSinitDataFromTpm->SgxSinitDataFromTpm));
  }

// AMI_OVERRIDE_SIPKG0004_START >>> We don't use PcdSiCsmEnable, this code move to the CpuRcPolicyPei.c.
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
  //
  // Check CsmSupport to set CSM flag.
  //
  if (SetupData != NULL && SetupData->CsmSupport) {//AMI_OVERRIDE >>> Fix cpp check error
    SiConfig->CsmFlag = 1;
  } else {
    SiConfig->CsmFlag = 0;
  }
#endif
// AMI_OVERRIDE_SIPKG0004_END <<<
}

/**
    This function will call CPU PEI policy override function. 

    @param PeiServices pointer to PEI services
    @param SiPolicyPpi pointer to CPU Pet policy

    @retval EFI_STATUS Return EFI status.
**/

EFI_STATUS
EFIAPI
UpdatePeiCpuPlatformPolicy (
  IN OUT  SI_POLICY_PPI *SiPolicyPpi
  )
{
  UINTN						i;
  EFI_STATUS                Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *ReadOnlyVariable; 
  EFI_GUID					gEfiSetupGuid = SETUP_GUID;
  SETUP_DATA				SetupDataPtr;
  SETUP_DATA 				*SetupData = NULL;
  UINTN						VariableSize= sizeof (SETUP_DATA);
  CONST EFI_PEI_SERVICES    **PeiServices;
  
  PeiServices = GetPeiServicesTablePointer ();
  
  //
  // Get AMI setup variable to setup PEI CPU Policy
  //
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

  for (i = 0; PeiCpuPolicyInitListTable[i] != NULL; i++) 
	PeiCpuPolicyInitListTable[i](PeiServices, SetupData, SiPolicyPpi);

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
