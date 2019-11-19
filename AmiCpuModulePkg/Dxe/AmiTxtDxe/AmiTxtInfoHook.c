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

/** @file AmiTxtInfoHook.c
    AmiTxtInfoHook Rountines

**/

#include <Token.h>
#include <AMIVfr.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/AMIPostMgr.h>
#include <Library/PrintLib.h>
#include "Include/Protocol/AmiTxtInfo.h"

extern EFI_BOOT_SERVICES      *gBS;
extern EFI_SYSTEM_TABLE       *gST;
extern EFI_RUNTIME_SERVICES   *gRT;

/**
    Show Txt info.

    @param VOID

    @retval EFI_STATUS Return EFI status.
**/

VOID
AmiTxtHook (VOID) 
{
    EFI_STATUS  Status;
	EFI_GUID AmiTseSetupguid = AMITSESETUP_GUID;
	EFI_GUID AmiTxtInfoProtocolGuid    = AMI_TXT_INFO_PROTOCOL_GUID;
	EFI_GUID gAmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
	AMI_POST_MANAGER_PROTOCOL *AmiPostMgr = NULL;
	AMI_TXT_INFO_PROTOCOL *AmiTxtInfoProt;
	AMI_TXT_INFO_DATA     *AmiTxtInfoData = NULL;    
	UINTN 		Index;
	EFI_INPUT_KEY 	Key = {0};
	AMITSESETUP TSEVar;
    UINTN       Size;
	CHAR16		Str[30];

	Size = sizeof (AMITSESETUP);

    Status = gRT->GetVariable(L"AMITSESetup", \
                               &AmiTseSetupguid, \
                               NULL, \
                               &Size, \
                               &TSEVar );
	if (EFI_ERROR(Status)) return;

	Status = gBS->LocateProtocol( &gAmiPostManagerProtocolGuid, NULL, &AmiPostMgr );
	if (EFI_ERROR(Status)) return;

	Status = gBS->LocateProtocol (&AmiTxtInfoProtocolGuid, NULL, &AmiTxtInfoProt);
	if (EFI_ERROR(Status)) return;

	//Get AmiTxt info data
	AmiTxtInfoProt->GetTxtInfo(AmiTxtInfoProt, &AmiTxtInfoData);

	//Only display txtinfo when error occur
//	if (!AmiTxtInfoData->TxtErrCode) return;

	if (AmiTxtInfoData->TxtErrCode) {
		UINT32 ClassCode = (AmiTxtInfoData->TxtErrCode >> 4) & 0x3f; //ErrorCode[9:4]
		UINT32 MajorCode = (AmiTxtInfoData->TxtErrCode >> 10) & 0x1f; //ErrorCode[14:10]
		
		//Check if it's progress code
		if ((ClassCode == 0) && (MajorCode == 0)) return;
	}

	//If it's silent boot, change to post screen
	if(TSEVar.AMISilentBoot != 0) AmiPostMgr->SwitchToPostScreen();

	AmiPostMgr->DisplayPostMessage(L"Intel Txt Information:");

	if (AmiTxtInfoData->ChipsetIsProduction)
		AmiPostMgr->DisplayPostMessage(L"Chipset Production Fused");
	else
		AmiPostMgr->DisplayPostMessage(L"Chipset Debug Fused");
	
	if (AmiTxtInfoData->BiosAcmIsProduction)
		AmiPostMgr->DisplayPostMessage(L"BiosAcm Production Fused");
	else
		AmiPostMgr->DisplayPostMessage(L"BiosAcm Debug Fused");

	if (AmiTxtInfoData->ChipsetIsTxtCapable)
		AmiPostMgr->DisplayPostMessage(L"Chipset Txt Supported");
	else
		AmiPostMgr->DisplayPostMessage(L"Chipset Txt Don't Support");
	
	if (AmiTxtInfoData->CpuIsTxtCapable)   //CPUID eax=1 ecx[6]
		AmiPostMgr->DisplayPostMessage(L"Cpu Txt Supported");
	else
		AmiPostMgr->DisplayPostMessage(L"Cpu Txt don't Support");
	
	if (AmiTxtInfoData->TxtErrCode) {
		UINT32 ClassCode = (AmiTxtInfoData->TxtErrCode >> 4) & 0x3f; //ErrorCode[9:4]
		UINT32 MajorCode = (AmiTxtInfoData->TxtErrCode >> 10) & 0x1f; //ErrorCode[14:10]
		UINT32 MinorCode = (AmiTxtInfoData->TxtErrCode >> 16) & 0x1ff; //ErrorCode[24:16]
		
		UnicodeSPrint(Str, sizeof(Str), L"Error Code %x", AmiTxtInfoData->TxtErrCode);
		AmiPostMgr->DisplayPostMessage(Str);
		
		UnicodeSPrint(Str, sizeof(Str), L" Class Code %x", ClassCode);
		AmiPostMgr->DisplayPostMessage(Str);

		UnicodeSPrint(Str, sizeof(Str), L"  Major Code %x", MajorCode);
		AmiPostMgr->DisplayPostMessage(Str);
		
		UnicodeSPrint(Str, sizeof(Str), L"  Minor Code %x", MinorCode);
		AmiPostMgr->DisplayPostMessage(Str);
    } else {
		AmiPostMgr->DisplayPostMessage(L"Error Code None");
		AmiPostMgr->DisplayPostMessage(L"  Class Code None");
		AmiPostMgr->DisplayPostMessage(L"  Major Code None");
		AmiPostMgr->DisplayPostMessage(L"  Minor Code None");
	}

	AmiPostMgr->DisplayPostMessage(L"Intel Trusted Execution Technology function fail, please enter setup to disable it.");
	AmiPostMgr->DisplayPostMessage(L"Press anykey to continue");

	while(1){
		gBS->WaitForEvent(1, &(gST->ConIn->WaitForKey), &Index);
		gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
		if((Key.UnicodeChar != 0) || (Key.ScanCode != 0)) break;
	}
					
	return; 
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
