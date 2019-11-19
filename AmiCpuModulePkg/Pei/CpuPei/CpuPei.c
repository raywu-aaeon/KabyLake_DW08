//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CpuPei.c
    This file is the main CPU PEI component file. This component utilizes
    CPU I/O & PCI CFG PPI to publish early CPU Init PPI which can be used
    by NB PEI to load itself.  Also this file contains a CPU init routine
    to be executed in permanent memory present environment. This is handled
    by issuing a notifyPPI on permanent memory PPI.

**/

#include "CpuPei.h"
#include "CpuPeiInterface.h"

typedef struct _CPU_BIST_HOB {
    EFI_HOB_GENERIC_HEADER   Header;
    EFI_GUID                 Name;
    UINT32                   ApicId;
    UINT32                   BIST;    
} CPU_BIST_HOB;

// Setup GUID variables for installing, locating and notifying PPIs
EFI_GUID gPeiCachePpiGuid               = PEI_CACHE_PPI_GUID;
EFI_GUID gEfiPeiEndOfPeiPhasePpiGuid    = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;
EFI_GUID gAmiCpuinfoHobGuid             = AMI_CPUINFO_HOB_GUID;
EFI_GUID gAmiStatusCodeCpuBistDataGuid  = AMI_STATUS_CODE_CPU_BIST_DATA_GUID;
EFI_GUID gAmiInternaluCodeHobGuid = AMI_INTERNAL_UCODE_HOB_GUID;
EFI_GUID gPeiReadOnlyVariable2PpiGuid = EFI_PEI_READ_ONLY_VARIABLE2_PPI_GUID;
EFI_GUID gAmiIntCodeVarDataGuid = AMI_INT_MICROCODE_VAR_DATA_GUID;

EFI_STATUS NotifyAtPeiEnd (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *Ppi
);

EFI_PEI_NOTIFY_DESCRIPTOR CpuNotifyDescs = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiEndOfPeiPhasePpiGuid,
    NotifyAtPeiEnd
};

/**
    Report the BIST Status Code.

    @param PeiServices pointer to PEI services.
    @param CpuinfoHob A pointer to cpu information HOB.
    @param NumCpus Number of cpu.

    @retval VOID
**/

VOID ReportBistStatusCodes(IN EFI_PEI_SERVICES **PeiServices, IN CPUINFO_HOB *CpuInfoHob, IN UINT32 NumCpus)
{
    UINT32 i;
    AMI_STATUS_CODE_CPU_BIST_DATA BistData;

    BistData.DataHeader.HeaderSize = sizeof(EFI_STATUS_CODE_DATA);
    BistData.DataHeader.Size = sizeof(UINT32);
    MemCpy(&BistData.DataHeader.Type, &gAmiStatusCodeCpuBistDataGuid, sizeof(EFI_GUID));

    for (i = 0; i < NumCpus; ++i) {        
        if ( CpuInfoHob->Cpuinfo[i].BIST != 0 ) {
            DEBUG ((DEBUG_INFO, "CpuInfo[%x].BIST = %x \n", i, CpuInfoHob->Cpuinfo[i].BIST));
            BistData.Bist = CpuInfoHob->Cpuinfo[i].BIST;

            (*PeiServices)->ReportStatusCode(
                PeiServices,
                EFI_ERROR_CODE| EFI_ERROR_MAJOR,
                PEI_CPU_SELF_TEST_FAILED,
                CpuInfoHob->Cpuinfo[i].ApicId,
                NULL,
                (EFI_STATUS_CODE_DATA*)&BistData
            );
        }
    }
}

/**
    Create CPU Hob and fill in default data.

    @param PeiServices pointer to PEI services.
    @param NumCpus Number of cpu.

    @retval CPUINFO_HOB Cpu information hob.
**/

CPUINFO_HOB * CreateCpuHobWithDefaults(IN EFI_PEI_SERVICES **PeiServices, IN UINT8 NumCpus)
{
    CPUINFO_HOB *CpuinfoHob;
    EFI_STATUS  Status;
    UINT32      RegEax;
    UINT32      RegEbx;
    UINT32      RegEcx;
    UINT32      RegEdx;
    UINT8       i;
    VOID        *FirstHob;
    CPU_BIST_HOB *SecBistHob;

    //Create hob for storing Cpu Data
    Status = (**PeiServices).CreateHob(PeiServices,
        EFI_HOB_TYPE_GUID_EXTENSION,
        //NOTE: sizeof(CPUINFO_HOB) already includes size of one CPUINFO structure
        sizeof(CPUINFO_HOB) + (NumCpus - 1) * sizeof(CPUINFO),
        &CpuinfoHob
    );
    ASSERT_EFI_ERROR (Status);

    CpuinfoHob->EfiHobGuidType.Name = gAmiCpuinfoHobGuid;
    CpuinfoHob->CpuCount = NumCpus;
    CpuinfoHob->NodeCount = NUMBER_CPU_SOCKETS;
    CpuinfoHob->CacheLineSize = 64;
    
    CpuinfoHob->TsegAddress = NBGetTsegBase();
    CpuinfoHob->TsegSize = TSEG_SIZE;

    for(i = 0; i < NumCpus; ++i) {
        CpuinfoHob->Cpuinfo[i].Valid    = FALSE;
        CpuinfoHob->Cpuinfo[i].Disabled = FALSE;
        CpuinfoHob->Cpuinfo[i].BIST     = 0;
		CpuinfoHob->Cpuinfo[i].ApicVer = (UINT8)MemRead32((UINT32*)(UINTN)(LOCAL_APIC_BASE + APIC_VERSION_REGISTER));
    }
//Save BSP features to CpuinfoHob
    CPULib_CpuID(0x01, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    CpuinfoHob->CpuFeatures.FeatureEcx = RegEcx;
    CpuinfoHob->CpuFeatures.FeatureEdx = RegEdx;

    CPULib_CpuID(0x80000001, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    CpuinfoHob->CpuFeatures.ExtFeatureEax = RegEax;
    CpuinfoHob->CpuFeatures.ExtFeatureEbx = RegEbx;
    CpuinfoHob->CpuFeatures.ExtFeatureEcx = RegEcx;
    CpuinfoHob->CpuFeatures.ExtFeatureEdx = RegEdx;

    (*PeiServices)->GetHobList(PeiServices, &FirstHob);
    if (!FirstHob) ASSERT_EFI_ERROR (EFI_NOT_FOUND);
    
    SecBistHob = (CPU_BIST_HOB *)FirstHob;
	while ( !EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, &SecBistHob)) ) {
		if ( guidcmp(&((EFI_HOB_GUID_TYPE*)SecBistHob)->Name, &gEfiHtBistHobGuid) == 0 ) {
			break;
		}
	}
	
	if ( !EFI_ERROR(Status) ) {
        CpuinfoHob->Cpuinfo[SecBistHob->ApicId].BIST = SecBistHob->BIST;
    }
    
    return CpuinfoHob;
}

/**
    According to resource descriptor HOBs to config system cache.

    @param PeiServices pointer to PEI services.
    @param NotifyDescriptor The notification 
        structure this PEIM registered on install.
    @param Ppi The memory discovered PPI.  Not used.

    @retval EFI_STATUS Return EFI status.
**/
EFI_STATUS NotifyAtPeiEnd (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *Ppi
)
{
    EFI_STATUS          Status;
    EFI_BOOT_MODE       BootMode;

    //
    //Get bootmode
    //
    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    
    if ( BootMode == BOOT_ON_S3_RESUME ){    	
    	DEBUG ((DEBUG_INFO, "CPUPEI boot mode is S3 in end of PEI, restore MSR\n"));
         //Trigger SWSMI to save CPU fixed & varible MTRRs
        IoWrite8(SW_SMI_IO_ADDRESS, SW_SMI_RESTORE_MSR);
    }
    
    return Status;
}

/**
    According to resource descriptor HOBs to config system cache.

    @param PeiServices pointer to PEI services.
    @param BootMode Boot mode

    @retval VOID
**/
#if 0
VOID SetupCache (IN EFI_PEI_SERVICES **PeiServices, IN EFI_BOOT_MODE BootMode)
{
    EFI_STATUS  Status;   
    PEI_CACHE_PPI *CachePpi;    
    UINT64 MemoryLength;
    UINT64 MemoryLengthUc;
    UINT64 MaxMemoryLength;
    UINT64 CurrentBaseAddress;
    UINT64 PowerTwo;
    UINT64 PowerTwoEnd;
    UINT8 MtrrCount = 0;

    //
    // Load Cache PPI
    //    
    Status = (*PeiServices)->LocatePpi(
                PeiServices,
                &gPeiCachePpiGuid,
                0,
                NULL,
                &CachePpi
                ) ;
    ASSERT_EFI_ERROR (Status);
           
    //
    // Clear the CAR Settings
    //
    CachePpi->ResetCache(
                CachePpi
                );
                
                
    //
    // Parse the HOB list and determine the amount of memory installed
    // The first 1MB will be set until overridden by the CSM.
    // The above 4G memory length will be calculated separately.
    //   
    
    MemoryLength = (UINT64)NBGetTsegBase() + TSEG_SIZE;
    DEBUG ((DEBUG_INFO, "Memory Length below 4G= %lx.\n", MemoryLength));
    
    //
    // Record the current address
    //
    CurrentBaseAddress = 0;
    //
    // Set WB loop first
    //

    for (PowerTwo = SIZE_2G, PowerTwoEnd = SIZE_64M; PowerTwo >= PowerTwoEnd; PowerTwo = Shr64 (PowerTwo, 1)) {
        if ( MemoryLength >= PowerTwo && MtrrCount < 8 ) {
        	DEBUG ((DEBUG_INFO, "WB Memory Length = %08lx at %09lx.\n", PowerTwo, CurrentBaseAddress));
            CachePpi->SetCache (CachePpi, CurrentBaseAddress, PowerTwo, EfiCacheTypeWriteBack);
            MtrrCount++;
            CurrentBaseAddress += PowerTwo;
            MemoryLength -= PowerTwo;
        }
    }
    //
    // Get Max Address for WB/UC
    //
    if ( MemoryLength == GetPowerOfTwo64 (MemoryLength) ) {
        MaxMemoryLength = MemoryLength;
    } else {
        MaxMemoryLength = GetPowerOfTwo64 (Shl64 (MemoryLength, 1));
    }
    //
    // Set the MAX memory range as WB
    //
    if ( MaxMemoryLength != 0 && MtrrCount < 8 ) {
    	DEBUG ((DEBUG_INFO, "WB Memory Length = %08lx at %09lx.\n", MaxMemoryLength, CurrentBaseAddress));
        CachePpi->SetCache (CachePpi, CurrentBaseAddress, MaxMemoryLength, EfiCacheTypeWriteBack);
        MtrrCount++;
    }
    //
    // Start Setting UC here
    //
    while ( MaxMemoryLength != MemoryLength ) {
        if ( MtrrCount == 8 ) break;
        MemoryLengthUc = GetPowerOfTwo64 (MaxMemoryLength - MemoryLength);
        DEBUG ((DEBUG_INFO, "UC Memory Length = %08lx at %09lx.\n", MemoryLengthUc, CurrentBaseAddress + MaxMemoryLength - MemoryLengthUc));
        CachePpi->SetCache (
                    CachePpi,
                    CurrentBaseAddress + MaxMemoryLength - MemoryLengthUc,
                    MemoryLengthUc,
                    EfiCacheTypeUncacheable
                    );
        MtrrCount++;
        MaxMemoryLength -= MemoryLengthUc;
    }    

    //
    // Programm fix MTRRs WB from 0 to A0000
    //
    DEBUG ((DEBUG_INFO, "WB Memory Length = %08x at %09x.\n", 640*1024, 0));
    CachePpi->SetCache(
                CachePpi,
                0,
                640*1024,
                EfiCacheTypeWriteBack
            );
    //
    // Cache Flash Area
    //
    MaxMemoryLength = GetPowerOfTwo64 (FLASH_SIZE);
    if ( MaxMemoryLength < FLASH_SIZE ) {
        MaxMemoryLength = Shl64 (MaxMemoryLength, 1);
    }
    
    Status = CachePpi->SetCache (
                        CachePpi,
                        (SIZE_4G - MaxMemoryLength),
                        MaxMemoryLength,
                        EfiCacheTypeWriteProtected
                    );
                    
    if (Status) DEBUG ((DEBUG_ERROR, PeiServices, "Flash Area Caching Error!!\n"));

    //
    //Disable NEM, Update MTRR setting from MTRR buffer
    //
    CachePpi->ActivateCache (CachePpi);
    
    //Call back to disable caching flash at end of PEI.
    (*PeiServices)->NotifyPpi(
        PeiServices,
        &CpuNotifyDescs
    ); 
}
#endif
/**
    AMI CPU PEI driver entry

    @param FfsHeader pointer to image FFS file
    @param PeiServices pointer to PEI services

    @retval EFI_STATUS Return EFI status.
**/

EFI_STATUS CpuPeiEntry(
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_PEI_READ_ONLY_VARIABLE2_PPI	*ReadOnlyVariable2;
    EFI_BOOT_MODE       BootMode;
    EFI_STATUS          Status;
    CPUINFO_HOB         *CpuinfoHob;
    UINT32              NumCpus; 
    VOID    *MicrocodeAddr = NULL;
    UINT32  MicrocodeSize = 0;
    EFI_PHYSICAL_ADDRESS  	MicrocodeBuffer;
    AMI_INTERNAL_UCODE_HOB *uCodeHob;
    UINTN VarSize = sizeof(AMI_INT_MICROCODE_DATA);
    AMI_INT_MICROCODE_DATA  AmiIntUcodeData;   
    
    PEI_PROGRESS_CODE(PeiServices, PEI_CPU_INIT);

    Status = (*PeiServices)->LocatePpi(
        PeiServices, &gPeiReadOnlyVariable2PpiGuid, 0, NULL, &ReadOnlyVariable2       
    );
    ASSERT_EFI_ERROR (Status);
    
    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    ASSERT_EFI_ERROR (Status);

	NumCpus = (UINT32)((UINT8)ReadMsr(MSR_CORE_THREAD_COUNT));	
	
    CpuinfoHob = CreateCpuHobWithDefaults(PeiServices, NumCpus);

    CpuinfoHob->BspNo = 0;

    ReportBistStatusCodes(PeiServices, CpuinfoHob, NumCpus);
  
    if ( BootMode != BOOT_ON_S3_RESUME ){
		//Copy CPU microcode from ROM to RAM, when flash ragne is still cached
		//and create hob to record the microcode address
		//Create CPU microcode HOB
		MicrocodeAddr = CPULib_FindMicrocode();
	
		if( MicrocodeAddr != NULL ){
			DEBUG ((DEBUG_INFO, "Cpu uCode ID - %x\n",((MICROCODE_HEADER*)MicrocodeAddr)->CpuSignature ));
			MicrocodeSize = ((MICROCODE_HEADER*)MicrocodeAddr)->TotalSize;
				
			Status = (*PeiServices)->AllocatePages (
										PeiServices,
										EfiBootServicesData, 
										EFI_SIZE_TO_PAGES (MicrocodeSize),
										&MicrocodeBuffer );
			if( !EFI_ERROR(Status) ){
				// Copy Microcode from ROM to RAM
				(*PeiServices)->CopyMem (
								(VOID*)MicrocodeBuffer, 
								(VOID*)MicrocodeAddr,
								(UINTN)MicrocodeSize );
				
				Status = (*PeiServices)->CreateHob(
							PeiServices,
							EFI_HOB_TYPE_GUID_EXTENSION,
							sizeof(AMI_INTERNAL_UCODE_HOB),
							&uCodeHob );
							
				if (!EFI_ERROR(Status)){
					MemCpy(&uCodeHob->EfiHobGuidType.Name, &gAmiInternaluCodeHobGuid, sizeof(EFI_GUID));                        
					uCodeHob->uCodeAddr = (UINT32)MicrocodeBuffer;
					DEBUG ((DEBUG_INFO, "Cpu MicrocodeBuffer - %x\n",(UINT32)MicrocodeBuffer));
				}
			}
		}
    } else {
        
        //
        // In s3 resume, try to get microcode address from variable
        //
        Status = ReadOnlyVariable2->GetVariable(
                         ReadOnlyVariable2,
                         AMI_INT_MICROCODE_ADDR_VARIABLE,
                         &gAmiIntCodeVarDataGuid,
                         NULL,
                         &VarSize,
                         &AmiIntUcodeData
                         );
                         
        if( !EFI_ERROR(Status) ) {
            MicrocodeAddr =  (VOID*)(AmiIntUcodeData.UcodeAddr);            
            Status = (*PeiServices)->CreateHob(
                        PeiServices,
                        EFI_HOB_TYPE_GUID_EXTENSION,
                        sizeof(AMI_INTERNAL_UCODE_HOB),
                        &uCodeHob );
            
            if( !EFI_ERROR(Status) ) {
                MemCpy(&uCodeHob->EfiHobGuidType.Name, &gAmiInternaluCodeHobGuid, sizeof(EFI_GUID));                        
			    uCodeHob->uCodeAddr = (UINT32)MicrocodeAddr;
            }
        }
    }    

    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

