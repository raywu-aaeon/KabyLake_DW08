//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiCpuFeaturesDxe.c
    Creat Cpu features information for cpu setup use.

**/

#include "AmiCpuFeaturesDxe.h"
#include "CpuSetupFeature.h"

typedef struct {
	UINT8	AfterFlashFlag;
	UINT8   Enable;
	UINT8	Update;
	UINT64  Ep0;
	UINT64  Ep1;
	UINT32  PrSize;
} EP_DATA;

#define AMI_EP_GUID \
    {0x73dad563, 0x8f27, 0x42af, 0x91, 0x8f, 0x86, 0x51, 0xeb, 0x0a, 0x93, 0xef}

#define	ENCRYPTION_SET1 0xc485b3d9d4a309b5
#define	ENCRYPTION_SET2 0x63b4ef2c1d3da345

EFI_GUID gAmiSetupGuid = SETUP_GUID;

AMICPU_SETUP_FEATURES  gCpuSetupFeatures;

/**
    This event is for if SGX setup items is not change in setup, then we will need to update 
    variable.
 
    @param Event Event that was triggered
    @param Context Point to calling context

    @retval VOID
**/

VOID
EventUpdateVariable (
    IN EFI_EVENT      Event,
    IN VOID           *Context
)
{
	EFI_STATUS  	Status;
	SETUP_DATA    	SetupData;
	EFI_GUID      	gAmiEpGuid = AMI_EP_GUID;
	EP_DATA			EpData;
	UINTN         	SetupSize = sizeof(SETUP_DATA);
	UINTN        	EpSize = sizeof(EP_DATA);
	UINT32      	EpAttributes = 0;
	UINT64			EncryptSet1 = ENCRYPTION_SET1;
	UINT64			EncryptSet2 = ENCRYPTION_SET2;
	
	pBS->CloseEvent (Event);

	Status = pRS->GetVariable(L"Setup", &gAmiSetupGuid, NULL, &SetupSize, &SetupData);
	
	if(EFI_ERROR(Status)) return;
	
	Status = pRS->GetVariable(L"Ep", &gAmiEpGuid, &EpAttributes, &EpSize, &EpData);
	
	if(!(EFI_ERROR(Status))) {
		//If SGX setup items is changed in DXE driver instead of in setup.
		if ((EpData.Enable != SetupData.EnableSgx) || ((EpData.Ep0 ^ EncryptSet1) != SetupData.SgxEpoch0) ||
		   ((EpData.Ep1 ^ EncryptSet2) != SetupData.SgxEpoch1) || (EpData.Update != SetupData.EpochUpdate) ||
		   (EpData.PrSize != SetupData.PrmrrSize)) {
			EpData.Enable = SetupData.EnableSgx;
			EpData.Ep0 = SetupData.SgxEpoch0 ^ EncryptSet1;
			EpData.Ep1 = SetupData.SgxEpoch1 ^ EncryptSet2;
			EpData.Update = SetupData.EpochUpdate;
			EpData.PrSize = SetupData.PrmrrSize;
			pRS->SetVariable(L"Ep", &gAmiEpGuid, EpAttributes, sizeof(EP_DATA), &EpData);
		}
	}
	else {
		//Create EP variable. 
		//EnableSGX is change by RC code, so we don't get EP variable which will create when user change setup item.
		//Cause SGX is changed to enabled, we'll need to create EP variable.
		EpData.AfterFlashFlag = 0;
		EpData.Enable = SetupData.EnableSgx;
		EpData.Ep0 = SetupData.SgxEpoch0 ^ EncryptSet1;
		EpData.Ep1 = SetupData.SgxEpoch1 ^ EncryptSet2;
		EpData.Update = SetupData.EpochUpdate;
		EpData.PrSize = SetupData.PrmrrSize;
		Status = pRS->SetVariable (
				 L"Ep",
				 &gAmiEpGuid,
				 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
				 EpSize,
				 &EpData
		);
		ASSERT_EFI_ERROR (Status);
	}
}

/**
    Fill in Sgx features data. 
 
    @param VOID

    @retval VOID
**/

VOID InitSgxFeatures()
{
	EFI_STATUS  	Status;
	SETUP_DATA    	SetupData;
	EFI_GUID      	gAmiEpGuid = AMI_EP_GUID;
	EP_DATA			EpData;
	UINTN         	SetupSize = sizeof(SETUP_DATA);
	UINTN        	EpSize = sizeof(EP_DATA);
	UINT32      	SetupAttributes = 0;
	UINT32      	EpAttributes = 0;
	UINT64			EncryptSet1 = ENCRYPTION_SET1;
  	UINT64			EncryptSet2 = ENCRYPTION_SET2;
  	EFI_EVENT		Event;
	
	Status = pRS->GetVariable(L"Setup", &gAmiSetupGuid, &SetupAttributes, &SetupSize, &SetupData);
	
	if( !EFI_ERROR(Status) ) {
		Status = pRS->GetVariable(L"Ep", &gAmiEpGuid, &EpAttributes, &EpSize, &EpData);
		if(!(EFI_ERROR(Status)) && (EpData.AfterFlashFlag == 1)) {
			//After flash ROM, restore SGX setting.
			if ((SetupData.EnableSgx != EpData.Enable) || (SetupData.SgxEpoch0 != (EpData.Ep0 ^ EncryptSet1)) ||
			   (SetupData.SgxEpoch1 != (EpData.Ep1 ^ EncryptSet2)) || (SetupData.EpochUpdate != EpData.Update) ||
			   (SetupData.PrmrrSize != EpData.PrSize)) {
				SetupData.EnableSgx = EpData.Enable;
				SetupData.SgxEpoch0 = EpData.Ep0 ^ EncryptSet1;
				SetupData.SgxEpoch1 = EpData.Ep1 ^ EncryptSet2;
				SetupData.EpochUpdate = EpData.Update;
				SetupData.PrmrrSize = EpData.PrSize;
				Status = pRS->SetVariable(
						L"Setup",
						&gAmiSetupGuid,
						SetupAttributes,
						sizeof(SETUP_DATA),
						&SetupData);
				//Reset to zero
				EpData.AfterFlashFlag = 0;
				pRS->SetVariable(L"Ep", &gAmiEpGuid, EpAttributes, sizeof(EP_DATA), &EpData);
			}
		}
		
		Status = CreateReadyToBootEvent(TPL_CALLBACK, \
										EventUpdateVariable, \
										NULL, \
										&Event );
	}
}

/**
    Fill in cpu features data. 
 
    @param VOID

    @retval VOID
**/

VOID InitCpuFeatures()
{
    
    EFI_STATUS  Status;
    UINT8 		PowerUnit;
    EFI_GUID 	gHobListGuid = HOB_LIST_GUID;
    EFI_GUID	gAmiCpuInternalInfoHobGuid = AMI_CPU_INTERNAL_INFO_HOB_GUID;
    VOID		*FirstHob;
    AMI_CPU_INTERNAL_INFO_HOB *CpuInternalHob;

    pBS->SetMem(&gCpuSetupFeatures, sizeof(gCpuSetupFeatures), 0);     //Initialize features to 0.      
    //   
    //Init Cpu features for setup item usage
    //    
    gCpuSetupFeatures.XDBitAvailable = IsXDSupported();
    gCpuSetupFeatures.HTAvailable = NumSupportedThreadsPerCore() > 1;
    gCpuSetupFeatures.MultiCoreAvailable = NumSupportedCpuCores() > 1;
    gCpuSetupFeatures.VTAvailable = IsVmxSupported();
    gCpuSetupFeatures.LimitCpuidAvailable = IsLimitCpuidSupported();
    gCpuSetupFeatures.MultiSocketAvailable = 0;
    gCpuSetupFeatures.MultiSocketPopulated = 0;
    gCpuSetupFeatures.LocalX2ApicAvailable = FALSE;
    gCpuSetupFeatures.NumCores = NumCpuCores();
    gCpuSetupFeatures.CpuGroup =  GetCpuGroup();   
    gCpuSetupFeatures.SmxAvailable = IsSmxSupported();
    gCpuSetupFeatures.XECoreRatioLimitAvailable = IsXECoreRatioLimitSupported();
    gCpuSetupFeatures.CurrentLimitAvailable = FALSE;
    gCpuSetupFeatures.PwrLimitAvailable = IsXeTdpLimitSupported();
    gCpuSetupFeatures.TccActivationAvailable = IsTccActProgSupported();
    gCpuSetupFeatures.EistAvailable = IsEistSupported();
    gCpuSetupFeatures.TurboModeAvailable = IsTurboModeSupported();
    gCpuSetupFeatures.C3Available = IsCxStateSupported(C3_STATE);
    gCpuSetupFeatures.C6Available = IsCxStateSupported(C6_STATE);
    gCpuSetupFeatures.C7Available = IsCxStateSupported(C7_STATE);   
    gCpuSetupFeatures.C8Available = IsCxStateSupported(C8_STATE);
    gCpuSetupFeatures.C9Available = IsCxStateSupported(C9_STATE);
    gCpuSetupFeatures.C10Available = IsCxStateSupported(C10_STATE);
    gCpuSetupFeatures.AesAvailable = IsAesSupported();
    gCpuSetupFeatures.MaximumTurboFrequency = GetMaximumTurboFrequency();

    //Because Cpuid function 1 ecx[25] Aes feature will change with Aes disable/enable status, get Aes feature before RC override it.
    FirstHob = GetEfiConfigurationTable(pST, &gHobListGuid);
	if ( FirstHob ) {
		CpuInternalHob = (AMI_CPU_INTERNAL_INFO_HOB*)FirstHob;
	
		//Find CpuInternal Hob
		while ( !EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION,&CpuInternalHob)) ) {
			if ( guidcmp(&CpuInternalHob->EfiHobGuidType.Name,&gAmiCpuInternalInfoHobGuid) == 0 ) break;
		}
		
		if ( !EFI_ERROR(Status) ) {
			gCpuSetupFeatures.AesAvailable = CpuInternalHob->AesAvailable;
		}
	}

    gCpuSetupFeatures.cTDPAvailable = IsCtdpSupported();
    gCpuSetupFeatures.PkgCStateDemotionAvailable = TRUE;
	gCpuSetupFeatures.FivrAvailable = FALSE;
	gCpuSetupFeatures.SgxSupported = AmiIsSgxSupported();
	gCpuSetupFeatures.PrmrrSupportSize = GetPrmrrSupportSize();
	gCpuSetupFeatures.HwpAvailable = IsHwpSupported();
    
    if ( !((ReadMsr (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
               (ReadMsr (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        gCpuSetupFeatures.DebugInterfaceSelection = TRUE;
    } else {
        gCpuSetupFeatures.DebugInterfaceSelection = FALSE;    
    }
    
    gCpuSetupFeatures.TjMax = (UINT8)(ReadMsr (MSR_TEMPERATURE_TARGET) >> 16 & 0xff);
    
    PowerUnit=(UINT8)(ReadMsr(MSR_PACKAGE_POWER_SKU_UNIT) & 0xf);    
    gCpuSetupFeatures.CpuTdp = (UINT16)((ReadMsr (MSR_PACKAGE_POWER_SKU) & 0x7fff) >> PowerUnit );


    Status = pRS->SetVariable(
                L"AmiCpuSetupFeatures",
                &gAmiSetupGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof(gCpuSetupFeatures),
                &gCpuSetupFeatures
            );

	if( EFI_ERROR(Status) )
		DEBUG ((DEBUG_ERROR, "Set Ami Cpu feature error = %r\n", Status));
}    

/**
    DXE Entry Point for Ami CpuFeatures Driver.
 
    @param ImageHandle Image handle of the loaded driver
    @param EFI_SYSTEM_TABLE SystemTable - Pointer to the System Table 

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS EFIAPI AmiCpuFeaturesDxeEntry(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{ 
    InitAmiLib(ImageHandle,SystemTable);
    InitCpuFeatures();
    InitSgxFeatures();
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
