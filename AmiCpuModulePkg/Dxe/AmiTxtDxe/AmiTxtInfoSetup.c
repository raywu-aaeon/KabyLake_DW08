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

/** @file AmiTxtInfoSetup.c
    AmiTxtInfo Setup hooks

**/

#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiHobs.h>
#include "Include/Protocol/AmiTxtInfo.h"

EFI_GUID gAmiTxtInfoProtocolGuid    = AMI_TXT_INFO_PROTOCOL_GUID;

AMI_TXT_INFO_PROTOCOL *gAmiTxtInfoProt;

#pragma pack(push, 1)

typedef struct {
    STRING_REF ChipsetFuse;
    STRING_REF BiosAcmFuse;
    STRING_REF ChipsetTxtsupport;
    STRING_REF CpuTxtsupport;
    STRING_REF ErrorCode;
	STRING_REF ClassCode;
	STRING_REF MajorCode;
	STRING_REF MinorCode;
} SKT_STR_TOK;

#pragma pack()

static SKT_STR_TOK gSktStrTok[] = {
{   STRING_TOKEN(STR_CPUTXT_CHIP_FUSE_VALUE),
    STRING_TOKEN(STR_CPUTXT_BIOSACM_FUSE_VALUE),
    STRING_TOKEN(STR_CPUTXT_CHIP_SUPPORT_VALUE),
    STRING_TOKEN(STR_CPUTXT_CPU_SUPPORT_VALUE),
    STRING_TOKEN(STR_CPUTXT_ERROR_CODE_VALUE),
	STRING_TOKEN(STR_CPUTXT_CLASS_CODE_VALUE),
	STRING_TOKEN(STR_CPUTXT_MAJOR_CODE_VALUE),
	STRING_TOKEN(STR_CPUTXT_MINOR_CODE_VALUE)
}};


/**
    Initialize CPU strings.

    @param HiiHandle Efi Hii Handle
    @param Class Form set class

    @retval VOID
**/

VOID InitAmiTxtInfoStrings(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{
    EFI_STATUS  Status;	
	AMI_TXT_INFO_DATA     *AmiTxtInfoData = NULL;	   
	
	if (Class != ADVANCED_FORM_SET_CLASS) return; 
	
	Status = pBS->LocateProtocol (&gAmiTxtInfoProtocolGuid, NULL, &gAmiTxtInfoProt);
	if (EFI_ERROR(Status)) return;
	
	//Stop if protocl version incorrect
	if (gAmiTxtInfoProt->ProtocolVer != AMI_TXT_INFO_PROTOCOL_VERSION) return;	
	
	//Get AmiTxt info data
	gAmiTxtInfoProt->GetTxtInfo(gAmiTxtInfoProt, &AmiTxtInfoData);
	
	//Update setup string
	if (AmiTxtInfoData->ChipsetIsProduction) {
	    InitString(
			HiiHandle,
			gSktStrTok[0].ChipsetFuse,
			L"%a",
			"Production Fused"
		);	
    }
    
	if (AmiTxtInfoData->BiosAcmIsProduction) {
	    InitString(
			HiiHandle,
			gSktStrTok[0].BiosAcmFuse,
			L"%a",
			"Production Fused"
		);	
    }
    
    if (AmiTxtInfoData->ChipsetIsTxtCapable) {
	    InitString(
			HiiHandle,
			gSktStrTok[0].ChipsetTxtsupport,
			L"%a",
			"Supported"
		);	
    }
    
    if (AmiTxtInfoData->CpuIsTxtCapable) {
	    InitString(
			HiiHandle,
			gSktStrTok[0].CpuTxtsupport,
			L"%a",
			"Supported"
		);	
    }
    
    if (AmiTxtInfoData->TxtErrCode) { 
		InitString(
			HiiHandle,
			gSktStrTok[0].ErrorCode,
			L"%x",
			AmiTxtInfoData->TxtErrCode
		);
		
		InitString(
			HiiHandle,
			gSktStrTok[0].ClassCode,
			L"%x",
			(AmiTxtInfoData->TxtErrCode >> 4) & 0x3f //ErrorCode[9:4]
		);
		
		InitString(
			HiiHandle,
			gSktStrTok[0].MajorCode,
			L"%x",
			(AmiTxtInfoData->TxtErrCode >> 10) & 0x1f //ErrorCode[14:10]
		);
		
		InitString(
			HiiHandle,
			gSktStrTok[0].MinorCode,
			L"%x",
			(AmiTxtInfoData->TxtErrCode >> 16) & 0x1ff //ErrorCode[24:16]
		);
    }
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
