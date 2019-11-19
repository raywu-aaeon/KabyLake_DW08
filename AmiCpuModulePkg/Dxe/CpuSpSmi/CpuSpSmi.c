//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CpuSpSmi.c
    Provide functions to CPU specific SMI

**/

#include "CpuSpSmi.h"

UINTN
EFIAPI
MpMtrrSynchUpEntry (
  VOID
 );
 
VOID
EFIAPI
MpMtrrSynchUpExit (
  UINTN Cr4
  );
 
#define AMI_INTERNAL_UCODE_HOB_GUID \
    {0x94567c6f, 0xf7a9, 0x4229, 0x13, 0x30, 0xfe, 0x11, 0xcc, 0xab, 0x3a, 0x11}

typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;    
	UINT32  uCodeAddr;
} AMI_INTERNAL_UCODE_HOB;

EFI_GUID gAmiInternaluCodeHobGuid = AMI_INTERNAL_UCODE_HOB_GUID;
EFI_GUID gAmiIntCodeVarDataGuid = AMI_INT_MICROCODE_VAR_DATA_GUID;

UINT32   gMicrocodeStart = 0;
//
// MSR table for S3 resume
//
EFI_MSR_VALUES       mFixedMtrrValues[] = {
  { MTRR_FIX_64K_00000,          0 },
  { MTRR_FIX_16K_80000,          0 },
  { MTRR_FIX_16K_A0000,          0 },
  { MTRR_FIX_4K_C0000,           0 },
  { MTRR_FIX_4K_C8000,           0 },
  { MTRR_FIX_4K_D0000,           0 },
  { MTRR_FIX_4K_D8000,           0 },
  { MTRR_FIX_4K_E0000,           0 },
  { MTRR_FIX_4K_E8000,           0 },
  { MTRR_FIX_4K_F0000,           0 },
  { MTRR_FIX_4K_F8000,           0 },
  { 0,      0 }
};

EFI_MSR_VALUES       mVariableMtrrValues[] = {
  { MTRR_PHYS_BASE_0,             0 },
  { MTRR_PHYS_MASK_0,             0 },
  { MTRR_PHYS_BASE_1,             0 },
  { MTRR_PHYS_MASK_1,             0 },
  { MTRR_PHYS_BASE_2,             0 },
  { MTRR_PHYS_MASK_2,             0 },
  { MTRR_PHYS_BASE_3,             0 },
  { MTRR_PHYS_MASK_3,             0 },
  { MTRR_PHYS_BASE_4,             0 },
  { MTRR_PHYS_MASK_4,             0 },
  { MTRR_PHYS_BASE_5,             0 },
  { MTRR_PHYS_MASK_5,             0 },
  { MTRR_PHYS_BASE_6,             0 },
  { MTRR_PHYS_MASK_6,             0 },
  { MTRR_PHYS_BASE_7,             0 },
  { MTRR_PHYS_MASK_7,             0 },
  { MTRR_PHYS_BASE_8,             0 },
  { MTRR_PHYS_MASK_8,             0 },
  { MTRR_PHYS_BASE_9,             0 },
  { MTRR_PHYS_MASK_9,             0 }
};

#define MiscMsrCount 1

EFI_MSR_VALUES		mSmmFeatureCtrl = { 0x4e0, 0};

/**
    This function saves the CPU MSRs for S3 resume usage.

    @param DispatchHandle Handle to the Dispatcher
    @param DispatchContext SW SMM dispatcher context

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS 
SaveCpuMsr (
    IN  EFI_HANDLE                  DispatchHandle,
	IN CONST VOID                   *Context OPTIONAL,
	IN OUT VOID                     *CommBuffer OPTIONAL,
	IN OUT UINTN                    *CommBufferSize OPTIONAL
)
{
    UINT32  i;
    if (Shr64(ReadMsr(0x17d),57)  & 0x03){
        if (!(ReadMsr(mSmmFeatureCtrl.Index) & 0x01))
            WriteMsr(mSmmFeatureCtrl.Index, ReadMsr(mSmmFeatureCtrl.Index) | 0x01);
		mSmmFeatureCtrl.Value = ReadMsr(mSmmFeatureCtrl.Index);
    }

    //Save Fixed MTRR
    for(i=0; i < NUM_OF_FIXED_MTRRS; i++)
    {
        mFixedMtrrValues[i].Value = ReadMsr(mFixedMtrrValues[i].Index);
    }
    
    //Save variable MTRR
    for (i = 0; i < ((UINT8)(ReadMsr(MSR_IA32_MTRR_CAP) & VCNT_MASK)); i++)
    {
        mVariableMtrrValues[i * 2].Value = ReadMsr(mVariableMtrrValues[i * 2].Index);        
        mVariableMtrrValues[i * 2 + 1].Value = ReadMsr(mVariableMtrrValues[i * 2 + 1].Index);    
    }

    return EFI_SUCCESS;
}

/**
    This function restores the CPU MSRs during S3 resume.

    @param VOID

    @retval VOID
**/

VOID
RestoreMsrOnCpu(IN VOID *data)
{
    UINT32  i;
    UINTN   Cr4;
    
    Cr4 = MpMtrrSynchUpEntry();
    
    //Restore Fixed MTRR
    for(i=0; i < NUM_OF_FIXED_MTRRS; i++)
    {
        WriteMsr(mFixedMtrrValues[i].Index, mFixedMtrrValues[i].Value );
    }
    
    //Restore variable MTRR
    for (i = 0; i < ((UINT8)(ReadMsr(MSR_IA32_MTRR_CAP) & VCNT_MASK)); i++)
    {
        WriteMsr(mVariableMtrrValues[i * 2].Index, mVariableMtrrValues[i * 2].Value );
        WriteMsr(mVariableMtrrValues[i * 2 + 1].Index, mVariableMtrrValues[i * 2 + 1].Value );     
    }
    
    MpMtrrSynchUpExit (Cr4);    
}

/**
    This function restores the CPU MSRs during S3 resume.

    @param DispatchHandle Handle to the Dispatcher
    @param DispatchContext SW SMM dispatcher context

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS 
RestoreCpuMsr (
    IN  EFI_HANDLE                  DispatchHandle,
	IN CONST VOID                   *Context OPTIONAL,
	IN OUT VOID                     *CommBuffer OPTIONAL,
	IN OUT UINTN                    *CommBufferSize OPTIONAL
)
{
    UINT32  i;
    UINTN   Cr4;    
        
	if (Shr64(ReadMsr(0x17d),57)  & 0x03){
        if (!(ReadMsr(mSmmFeatureCtrl.Index) & 0x01)){
            WriteMsr(mSmmFeatureCtrl.Index, (mSmmFeatureCtrl.Value & ~0x01));
			WriteMsr(mSmmFeatureCtrl.Index, ReadMsr(mSmmFeatureCtrl.Index) | 0x01);
		}
    }

    for (i = 0; i < pSmst->NumberOfCpus; ++i) {
        pSmst->SmmStartupThisAp(RestoreMsrOnCpu, i, NULL);
    }
    
    Cr4 = MpMtrrSynchUpEntry();	
    RestoreMsrOnCpu(NULL);    
    MpMtrrSynchUpExit (Cr4);
	
    return EFI_SUCCESS;
}

/**
    Called from InstallSmiHandler

    @param ImageHandle - Handle assigned to this driver.
    @param SystemTable - Efi System table.

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS InSmmFunction(
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
 )
{
    EFI_STATUS  Status;
    EFI_HANDLE  Handle;
    EFI_SMM_BASE2_PROTOCOL      *gSmmBase2;
    EFI_SMM_SW_DISPATCH2_PROTOCOL    *gSwDispatch = NULL;	
    EFI_SMM_SW_REGISTER_CONTEXT Save_CPU_MSR = {SW_SMI_SAVE_MSR};
    EFI_SMM_SW_REGISTER_CONTEXT Restore_CPU_MSR = {SW_SMI_RESTORE_MSR};    
    EFI_PHYSICAL_ADDRESS   TsegUcodeAddr;
    UINT32                 MicrocodeSize = 0;
    AMI_INT_MICROCODE_DATA  AmiIntUcodeData;
    EDKII_VARIABLE_LOCK_PROTOCOL    *VariableLockProtocol = NULL;
    
    if (gMicrocodeStart != 0 && ((MICROCODE_HEADER*)gMicrocodeStart)->HeaderVersion == 1){  //microcode header version valid?         
		//Copy microcode into SMRAM       
		MicrocodeSize = ((MICROCODE_HEADER*)gMicrocodeStart)->TotalSize;
		Status = pSmst->SmmAllocatePages(AllocateAnyPages, EfiRuntimeServicesData, EFI_SIZE_TO_PAGES (MicrocodeSize), &TsegUcodeAddr);
	    if ( !EFI_ERROR(Status) ) {
			MemCpy((UINT8 *)TsegUcodeAddr, (UINT8 *)gMicrocodeStart, MicrocodeSize);

			//Save the ucode address for S3 resume
			AmiIntUcodeData.Version = 1;
			AmiIntUcodeData.UcodeAddr = (UINT32)TsegUcodeAddr;
			AmiIntUcodeData.Rsv0 = 0;
			AmiIntUcodeData.Rsv1 = 0;
            {
                //If BIOS is updated with preserving NRAM, variable should be deleted first, because the attribute is different.
                UINT32      Attributes = 0;
                UINTN       DataSize;
                UINTN       *OldAmiIntUcodeData;
                  
                DataSize = 0;
                Status = pRS->GetVariable(
                                AMI_INT_MICROCODE_ADDR_VARIABLE,
                                &gAmiIntCodeVarDataGuid,
                                &Attributes,
                                &DataSize,
                                NULL
                                );
                if (Status == EFI_BUFFER_TOO_SMALL) {
                  Status = pBS->AllocatePool(EfiBootServicesData, DataSize, (VOID**)&OldAmiIntUcodeData);
                  if (!EFI_ERROR(Status)) {
                    Status = pRS->GetVariable(
                                  AMI_INT_MICROCODE_ADDR_VARIABLE,
                                  &gAmiIntCodeVarDataGuid,
                                  &Attributes,
                                  &DataSize,
                                  (VOID*)OldAmiIntUcodeData
                                  );
                    if ((!EFI_ERROR(Status)) && (Attributes != (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS))) {
                        //Attribute is different, delete first.
                        Status = pRS->SetVariable (
                                      AMI_INT_MICROCODE_ADDR_VARIABLE,
                                      &gAmiIntCodeVarDataGuid,
                                      0,
                                      0,
                                      NULL
                                      ); 
                        DEBUG((DEBUG_INFO, "CpuSpSmi: delete AMI_INT_MICROCODE_ADDR_VARIABLE Variable. Status = %r.\n", Status));
                       ASSERT_EFI_ERROR(Status);
                    }
                    pBS->FreePool (OldAmiIntUcodeData);
                  }
                }
            }
			Status = pRS->SetVariable(
			   AMI_INT_MICROCODE_ADDR_VARIABLE,
			   &gAmiIntCodeVarDataGuid,
			   EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
			   sizeof(AMI_INT_MICROCODE_DATA), 
			   (VOID*) &AmiIntUcodeData
		    );
			ASSERT_EFI_ERROR(Status);
			Status = pBS->LocateProtocol (
                      &gEdkiiVariableLockProtocolGuid,
                      NULL,
                      (VOID **) &VariableLockProtocol
                      );
			if ( !EFI_ERROR(Status) ) {
			    Status = VariableLockProtocol->RequestToLock (
                                VariableLockProtocol,
                                AMI_INT_MICROCODE_ADDR_VARIABLE,
                                &gAmiIntCodeVarDataGuid
                                );
                DEBUG((DEBUG_INFO, "CpuSpSmi: Lock AMI_INT_MICROCODE_ADDR_VARIABLE Variable. Status = %r.\n", Status));
                ASSERT_EFI_ERROR (Status);
			}
		}
    }    
    
    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &gSmmBase2);
    
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmmBase->GetSmstLocation (gSmmBase2, &pSmst);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmst->SmmLocateProtocol( \
                        &gEfiSmmSwDispatch2ProtocolGuid, NULL, &gSwDispatch);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = gSwDispatch->Register(
        gSwDispatch,
        SaveCpuMsr,
        &Save_CPU_MSR,
        &Handle
    );
    ASSERT_EFI_ERROR(Status);    
   
    Status = gSwDispatch->Register(
        gSwDispatch,
        RestoreCpuMsr,
        &Restore_CPU_MSR,
        &Handle
    );    
    ASSERT_EFI_ERROR(Status);
    
    return EFI_SUCCESS;
}

/**
    Initializes CPU specific SMM Drivers.

    @param ImageHandle - Handle assigned to this driver.
    @param SystemTable - Efi System table.

    @retval EFI_STATUS Return EFI status

    @note Here is the control flow of this function
**/

EFI_STATUS EFIAPI CpuSpSmiInit(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
 )
{
    EFI_STATUS Status;
    AMI_INTERNAL_UCODE_HOB    *uCodeHob = NULL;
    EFI_GUID HobListGuid = HOB_LIST_GUID;    
    
    InitAmiLib(ImageHandle,SystemTable);
    uCodeHob = (AMI_INTERNAL_UCODE_HOB*)GetEfiConfigurationTable(pST,&HobListGuid);
    
    if (uCodeHob != NULL) {
        Status = FindNextHobByGuid(&gAmiInternaluCodeHobGuid,(VOID**)&uCodeHob);
        if (Status == EFI_SUCCESS && uCodeHob->uCodeAddr != 0 && uCodeHob->uCodeAddr != 0xffff) {            
            gMicrocodeStart = uCodeHob->uCodeAddr;            //Microcode hob found, save microcode address
        }
    }
    
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

