//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30071           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CpuSetupPreserve.c
    Cpu setup preserve function.

**/

#include <Efi.h>
#include <Dxe.h>
#include <AmiDxeLib.h>

#define AMI_EP_GUID \
    {0x73dad563, 0x8f27, 0x42af, 0x91, 0x8f, 0x86, 0x51, 0xeb, 0x0a, 0x93, 0xef}

typedef struct {
	UINT8	AfterFlashFlag;
	UINT8   Enable;
	UINT8	Update;
	UINT64  Ep0;
	UINT64  Ep1;
	UINT32  PrSize;
} EP_DATA;

EP_DATA		EpData;
BOOLEAN     EpDataExist;

/**
    This function reserves the Cpu setup variable before erasing NVRAM block

    @param VOID

    @retval VOID

    @note  Modify SmmComputraceNVData and mComputraceVariableSize
**/

VOID
PreserveCpuSetupData(VOID)
{

    EFI_STATUS  Status;
    UINT32      EpAttributes = 0;
    UINTN       EpSize = sizeof(EP_DATA);
    EFI_GUID    gAmiEpGuid = AMI_EP_GUID;
  
    Status = pRS->GetVariable(L"Ep", &gAmiEpGuid, &EpAttributes, &EpSize, &EpData);

    if (!EFI_ERROR(Status)) EpDataExist = TRUE;

}
/**
    This function restore the Cpu setup variable after programming NVRAM block

    @param VOID

    @retval VOID

**/
VOID
RestoreCpuSetupData(VOID)
{
    EFI_GUID    gAmiEpGuid = AMI_EP_GUID;

    if (!EpDataExist) return;
    
    EpData.AfterFlashFlag = 1;

    pRS->SetVariable(
		L"Ep",
		&gAmiEpGuid,
		EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
		sizeof(EP_DATA),
		(VOID*) &EpData
	);

}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30071           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
