//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#include <Efi.h>
#include <AmiPeiLib.h>
#include <AmiTxtTcgPeim.h>

#define CopyMem AuxCopyMem

#pragma pack(1)

typedef struct {
    TPM2_COMMAND_HEADER     Header;
    UINT16                  bytesRequested;
} TPM2_GetRandom_COMMAND;

typedef struct {
    TPM2_RESPONSE_HEADER      Header;
    TPM2B_DIGEST              randomBytes;
} TPM2_GetRandom_RESPONSE;

#pragma pack()

EFI_STATUS Tpm2GetRandom(
    UINTN                   unRequest,
    UINT8*                  pOutBuf
)
{
    EFI_STATUS              Status = EFI_SUCCESS;

    TPM2_GetRandom_COMMAND  GetRandom_cmd;
    TPM2_GetRandom_RESPONSE GetRandom_ret;
    UINT32                  RetSize;

    // Check the Request Size
    if( unRequest > sizeof(TPM2B_DIGEST) - sizeof(UINT16) )
    {
        //DEBUG(( -1, "[%d]: Tpm2GetRandom Error. Request too large\n", __LINE__ ));
        return EFI_BUFFER_TOO_SMALL;
    }

    MemSet( &GetRandom_cmd, sizeof(GetRandom_cmd), 0 );
    MemSet( &GetRandom_ret, sizeof(GetRandom_ret), 0 );

    GetRandom_cmd.Header.tag = SwapBytes16(TPM_ST_NO_SESSIONS);
    GetRandom_cmd.Header.commandCode = SwapBytes32(TPM_CC_GetRandom);
    GetRandom_cmd.Header.paramSize = SwapBytes32( sizeof(GetRandom_cmd) );
    GetRandom_cmd.bytesRequested = SwapBytes16(unRequest );

    RetSize = sizeof(GetRandom_ret);
    Status = TpmOemSubmitCommand ( sizeof(GetRandom_cmd), (UINT8*)&GetRandom_cmd, &RetSize, (UINT8*)&GetRandom_ret );
    if (EFI_ERROR (Status)) {
        //DEBUG(( -1, "[%d]: Tpm2GetRandom.Error\n", __LINE__ ));
        return Status;
    }

    if (SwapBytes32(GetRandom_ret.Header.responseCode) != TPM_RC_SUCCESS) {
        //DEBUG ((-1, "[%d]: Tpm2GetRandom: Response Code error! 0x%08x\r\n", __LINE__, SwapBytes32(GetRandom_ret.Header.responseCode) ));
        return EFI_DEVICE_ERROR;
    }

    CopyMem( pOutBuf, &GetRandom_ret.randomBytes.buffer[0], unRequest );

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
