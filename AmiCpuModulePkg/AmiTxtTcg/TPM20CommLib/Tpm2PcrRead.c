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
#include <Library/DebugLib.h>
#include <AmiLib.h>
#include <AmiTxtTcgPeim.h>

#pragma pack(1)
typedef struct
{
    TPM2_COMMAND_HEADER Header;
    TPML_PCR_SELECTION  pcrSelection;
}TPM2_PCR_Read;

typedef struct
{
    TPM2_RESPONSE_HEADER    ResHead;
    UINT32                  pcrUpdateCounter;
    TPML_PCR_SELECTION      pcrSelectionOut;
//    TPML_DIGEST             pcrValues;
}TPM2_PCR_Read_Res;
#pragma pack()

EFI_STATUS Tpm2PCRRead(
    IN TPM_PCRINDEX PCRIndex,
    OUT TPM_DIGEST  *Digest )
{
    EFI_STATUS              Status;
    TPM2_PCR_Read           Cmd;
    UINTN                   unIdx;
    UINT32                  RetBufSize = 0x200;
    UINT8                   Res[0x200];
    TPM2_PCR_Read_Res       *pResCmd;
    TPML_DIGEST*            pRetDigest;

    Cmd.Header.tag         = (TPM_ST) TPM_H2NS(TPM_ST_NO_SESSIONS);
    Cmd.Header.commandCode = (TPM_CC) TPM_H2NL(TPM_CC_PCR_Read);
    
    Cmd.pcrSelection.count = TPM_H2NL(0x00000001);
    Cmd.pcrSelection.pcrSelections[0].hash = TPM_H2NS(TPM_ALG_SHA1);    // SHA-1
    Cmd.pcrSelection.pcrSelections[0].sizeofSelect = (UINT8)PCR_SELECT_MIN;      // PCR 0~24 
    // Assign PCR Index.
    unIdx = PCRIndex / 8;
    Cmd.pcrSelection.pcrSelections[0].pcrSelect[unIdx] = (UINT8)1<<(PCRIndex % 8);
    
    Cmd.Header.paramSize = 
            sizeof(Cmd.Header)+ 
            sizeof(Cmd.pcrSelection.count)+
            sizeof(Cmd.pcrSelection.pcrSelections[0]);
        
    Cmd.Header.paramSize = TPM_H2NL(Cmd.Header.paramSize);
    
    Status = TpmOemSubmitCommand(
            TPM_H2NL(Cmd.Header.paramSize),
            (UINT8*)&Cmd,
            &RetBufSize,
            Res
            );
    if (EFI_ERROR (Status)) {
       //DEBUG((-1,"Tpm2SubmitCommand() Failed: [%r]\n", Status));    
       return Status;
   }
    
    pResCmd = (TPM2_PCR_Read_Res*)Res;
    
    if( pResCmd->ResHead.responseCode )
    {
        //DEBUG((-1,"Tpm2PCRRead(...): ErrorCode[%x]\n", TPM_H2NL(pResCmd->ResHead.responseCode) ));
        return EFI_DEVICE_ERROR;
    }
    
    // Presp
    unIdx = 
            // TPM2_RESPONSE_HEADER
            sizeof(pResCmd->ResHead) 
            // UINT32
            + sizeof(pResCmd->pcrUpdateCounter) 
            // UINT32
            + sizeof(pResCmd->pcrSelectionOut.count);
        
    unIdx += 
            // TPML_PCR_SELECTION->count * sizeof(TPMS_PCR_SELECTION)
            TPM_H2NL(pResCmd->pcrSelectionOut.count) * sizeof(pResCmd->pcrSelectionOut.pcrSelections[0]);
    
    pRetDigest = (TPML_DIGEST*)&Res[unIdx];
        
    AuxCopyMem( Digest, pRetDigest->digests[0].buffer, TPM_H2NS(pRetDigest->digests[0].size));

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
