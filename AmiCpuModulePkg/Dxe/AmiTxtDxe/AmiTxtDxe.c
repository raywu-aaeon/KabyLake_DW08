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

/** @file AmiTxtDxe.c
    Creat Ami txt information protocol and define Tpm Nvram.

**/

#include "AmiTxtDxe.h"
#include "Txt.h"

AMI_TXT_INFO_PROTOCOL gAmiTxtInfoProtocol = {0};
AMI_TXT_INFO_DATA     gAmiTxtInfoData = {0};

EFI_GUID gHobListGuid               = HOB_LIST_GUID;
EFI_GUID gAmiTxtInfoProtocolGuid    = AMI_TXT_INFO_PROTOCOL_GUID;

EFI_GUID gSetupGuid = SETUP_GUID;

#if defined SinitAcm_SUPPORT && SinitAcm_SUPPORT == 1
/**
    This routine copy SINIT ACM to SINIT meory for BIOS-Base SINI 
    AC Module implementation.
 
    @param Event Event that was triggered
    @param Context Point to calling context

    @retval VOID
**/

VOID
EventUpdateSinitAcmToMemory (
    IN EFI_EVENT      Event,
    IN VOID           *Context
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_HANDLE      *FvHandle;
    UINTN           Number, i, SinitacmSize = 0;
    UINT32          Authentication, TxtMemBase;
    VOID            *pSinitacmPtr = NULL;
    EFI_FIRMWARE_VOLUME2_PROTOCOL	*Fv;
	EFI_FV_FILETYPE               	FileType;
	EFI_FV_FILE_ATTRIBUTES        	FileAttributes;
    EFI_GUID        SinitAcmFfsGuid = \
    { 0x7fbfe0ea, 0x0e9d, 0x406b, 0xa2, 0x2, 0xca, 0x6a, 0x7d, 0x0e, 0x0f, 0x1e};
    EFI_GUID  	    gEfiFirmwareVolume2ProtocolGuid 	= 
    { 0x220e73b6, 0x6bdb, 0x4413, 0x84, 0x5, 0xb9, 0x74, 0xb1, 0x8, 0x61, 0x9a };


    pBS->CloseEvent (Event);
    Status = pBS->LocateHandleBuffer (ByProtocol, \
                                      &gEfiFirmwareVolume2ProtocolGuid, \
                                      NULL, \
                                      &Number, \
                                      &FvHandle);
    if (EFI_ERROR(Status)) return;

    for (i = 0; i < Number; i++) {
        Status = pBS->HandleProtocol (\
                    FvHandle[i], &gEfiFirmwareVolume2ProtocolGuid, (VOID **)&Fv);
        if (EFI_ERROR(Status)) continue;
        
        // Read SINIT AC module from Firmware Volume
        SinitacmSize = 0;        
        pSinitacmPtr = NULL;
									
		Status = Fv->ReadFile (
                    Fv,
                    &SinitAcmFfsGuid,
                    &pSinitacmPtr,
                    &SinitacmSize,
                    &FileType,
                    &FileAttributes,
                    &Authentication
                    );
		
        if (!EFI_ERROR(Status)) {    
            // SINIT Memory Base = TXT Public Space + 270h
            TxtMemBase = *(UINT32*)0xfed30270;
            // copy SINIT ACM to SINIT memory
            pBS->CopyMem ((VOID*)TxtMemBase, pSinitacmPtr, SinitacmSize);
            // BiosToOS Region Base = TXT Public Space + 300h
            TxtMemBase = *(UINT32*)0xfed30300;
            // Update SINIT ACM size to BiosToOS region
            *(UINT32*)(TxtMemBase + 12) = (UINT32)SinitacmSize; 
            pBS->FreePool (pSinitacmPtr);
            break;
        }
    }
    pBS->FreePool (FvHandle);
}
#endif
/**
    Get Txt information

    @param VOID

    @retval VOID
**/

EFI_STATUS GetTxtInfo (
	IN AMI_TXT_INFO_PROTOCOL  *AMI_TXT_INFO_PROTOCOL,
    OUT VOID                    **AmiTxtInfoData
){
    *AmiTxtInfoData = (VOID*)(&gAmiTxtInfoData);
    return 0;
}

/**
    Initialize Txt information

    @param VOID

    @retval VOID
**/

VOID InitAmiTxtInfo() 
{
    EFI_STATUS   Status;
    VOID         *HobList;
    TXT_INFO_HOB *TxtInfoHob;
    UINT32       RegEax, RegEbx, RegEcx, RegEdx;    
	UINT32		 *TxtErrCode  = (UINT32*)(TXT_PUBLIC_BASE + 0x0030);
	

    HobList = GetEfiConfigurationTable(pST,&gHobListGuid);
    if (!HobList) return;    
    
    //Find RC Txt Info Hob.
    TxtInfoHob = (TXT_INFO_HOB*)HobList;
    while (!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, &TxtInfoHob))) {
        if (guidcmp(&TxtInfoHob->EfiHobGuidType.Name, &gTxtInfoHobGuid) == 0) break;
    }
    if (EFI_ERROR(Status)) return;
	
	//Initialize AmiTxtInfoProtocol
	gAmiTxtInfoProtocol.ProtocolVer = AMI_TXT_INFO_PROTOCOL_VERSION;
	gAmiTxtInfoProtocol.GetTxtInfo = GetTxtInfo;	
    
    //Initialize AMI TXT info data from HOB created by Intel Txt RC
    gAmiTxtInfoData.ChipsetIsTxtCapable = (TxtInfoHob->Data.ChipsetIsTxtCapable) ? TRUE : FALSE;
    
    //Initializing ChipsetIsProduction default value
    gAmiTxtInfoData.ChipsetIsProduction = (*(UINT32 *) (TXT_PUBLIC_BASE + 0x200) & BIT31) ? TRUE : FALSE;
    
    //Check ACM is production or not
    if ( TxtInfoHob->Data.BiosAcmBase != 0)
    	gAmiTxtInfoData.BiosAcmIsProduction = (((ACM_HEADER*)((UINT32)TxtInfoHob->Data.BiosAcmBase))->ModuleID & BIT31) ? FALSE : TRUE;
    
    //CPUID funciton 1 ECX[6], Secure Mode Extensions check
    AsmCpuid(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    gAmiTxtInfoData.CpuIsTxtCapable = (RegEcx & BIT6) ? TRUE : FALSE;
    
    //TXT.CRASH register, TXT_PUBLIC_BASE + 0x30
    gAmiTxtInfoData.TxtErrCode = *TxtErrCode;
    
    Status = pBS->InstallProtocolInterface(
                    &TheImageHandle,
                    &gAmiTxtInfoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gAmiTxtInfoProtocol
                );   

	if (EFI_ERROR(Status))
		DEBUG ((DEBUG_ERROR, "AmiTXTDxe : Install Protocol error, Status = %x\n", Status));
}
/**
    DXE Entry Point for Ami TXT Driver.
 
    @param ImageHandle Image handle of the loaded driver
    @param EFI_SYSTEM_TABLE SystemTable - Pointer to the System Table 

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS
EFIAPI
AmiTxtDxeEntry (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
)
{
	InitAmiLib(ImageHandle, SystemTable);   			
	
    InitAmiTxtInfo();

#if defined SinitAcm_SUPPORT && SinitAcm_SUPPORT == 1
	{
        // BIOS-Based SINIT AC module support.
		EFI_STATUS		Status;
        EFI_EVENT       SinitEvent;
        UINTN			VariableSize;
        SETUP_DATA		SetupData;
        UINT8       	*TpmBaseAddr = (UINT8*)0xfed40000;

        VariableSize = sizeof(SETUP_DATA);
        Status = pRS->GetVariable ( L"Setup", \
							&gSetupGuid, \
							NULL, \
							&VariableSize, \
							&SetupData );\

		//If TPM device is present and Txt is enabled
		if ( (!EFI_ERROR(Status)) && (SetupData.Txt != 0) && (*TpmBaseAddr != 0xff) ) { 
			CreateReadyToBootEvent(TPL_CALLBACK, \
	                               EventUpdateSinitAcmToMemory, \
	                               NULL, \
	                               &SinitEvent );
		}
	}	                                    
#endif	
	
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
