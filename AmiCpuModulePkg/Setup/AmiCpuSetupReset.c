//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CpuSetupReset.c
    CPU Setup reset Rountines

**/

#include <Setup.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Protocol/MePlatformGetResetType.h>

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

static EFI_GUID gAmiEpGuid = AMI_EP_GUID;
static EFI_GUID gAmiSetupGuid = SETUP_GUID;

ME_PLATFORM_GET_RESET_TYPE_PROTOCOL mCpuPlatformGetResetType;

/**
    This function is a hook called after some control
    modified in the setup utility by user. This
    function is available as ELINK.

    @param VOID

    @retval VOID

**/

VOID
AmiCpuSavedConfigChanges (
    VOID
)
{
    EFI_STATUS		Status;
    UINTN			Size = sizeof (SETUP_DATA);
    UINT32			EpAttributes = 0;
    SETUP_DATA      SetupData;
    EP_DATA			EpData;
    EFI_GUID      	gAmiEpGuid = AMI_EP_GUID;
    UINTN         	EpSize = sizeof(EP_DATA);
    UINT64			EncryptSet1 = ENCRYPTION_SET1;
    UINT64			EncryptSet2 = ENCRYPTION_SET2;

    Status = pRS->GetVariable (
                 L"Setup",
                 &gAmiSetupGuid,
                 NULL,
                 &Size,
                 &SetupData
             );

    if ((!EFI_ERROR (Status)) && SetupData.EnableSgx != 0) {
		Status = pRS->GetVariable(L"Ep", &gAmiEpGuid, &EpAttributes, &EpSize, &EpData);
		if (!EFI_ERROR (Status)) {
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
		}
    }
    else {
    	Status = pRS->GetVariable(L"Ep", &gAmiEpGuid, &EpAttributes, &EpSize, &EpData);
    	if (!EFI_ERROR (Status)) {
    		pRS->SetVariable (
				 L"Ep",
				 &gAmiEpGuid,
				 EpAttributes,
				 0,
				 &EpData
    		);
    	}
    }
	
}
#if 0
/**
    Return PowerCycleResetReq type.

    @param PlatformResetType

    @retval ME_SPECIAL_RESET_TYPES

**/

ME_SPECIAL_RESET_TYPES
CpuPlatformReset (
  IN  EFI_RESET_TYPE                  PlatformResetType
  )
{  
    return PowerCycleResetReq;
}

/**
    This routine for calling shutdown reset if necessary.

    @param VOID

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS
CpuExitSetupEvent  (VOID)
{
    EFI_STATUS          Status;
    UINTN				Size = sizeof (SETUP_DATA);
    SETUP_DATA      	SetupData; 
    EFI_HANDLE          PlatformResetHandle;
    BOOLEAN				TriggerPowerCycleReset = 0;

    Status = pRS->GetVariable (
    				L"Setup",
    		        &gAmiSetupGuid,
                    NULL,
                    &Size,
                    &SetupData
                 );
    if (EFI_ERROR(Status)) return (Status);
	
/*	if ( SetupData.Txt != 0 ) {
		// Generate Global Reset System if TXT is enabled w/o SMX enabled 
		if ((UINT16)(ReadMsr(0x3A) & 0xFF03) != 0xFF03) {
			TriggerPowerCycleReset = 1;
		}
	} else { 
		// Generate Global Reset System if TXT is disabled with SMX enabled.
		if ((UINT16)(ReadMsr(0x3A) & 0xFF03) == 0xFF03) {
			TriggerPowerCycleReset = 1;
		}
	}*/
	
	if ( TriggerPowerCycleReset ) {
		PlatformResetHandle = NULL;
		mCpuPlatformGetResetType.Revision       = ME_PLATFORM_GET_RESET_TYPE_PROTOCOL_REVISION;
		mCpuPlatformGetResetType.MeGetResetType = CpuPlatformReset;
		pBS->InstallMultipleProtocolInterfaces (
					&PlatformResetHandle,
					&gMePlatformGetResetTypeGuid,  &mCpuPlatformGetResetType,
					NULL
					);
	}

    return EFI_SUCCESS;
}
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
