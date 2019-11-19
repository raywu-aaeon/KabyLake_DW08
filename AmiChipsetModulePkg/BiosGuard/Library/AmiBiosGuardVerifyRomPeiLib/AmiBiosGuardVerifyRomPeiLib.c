//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiBiosGuardVerifyRomPeiLib.c


*/

#include <Token.h>
#include <Uefi.h>
#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <BiosGuard.h>
#include <Protocol/AmiDigitalSignature.h>
#include "BaseCryptLib.h"
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <AmiBiosGuard.h>


/**    
    Swap byte reverse.

    @param Key - pointer to the public key
    
    @retval  EFI_SUCCESS - Set Capsule Update EFI Variable success.
**/
VOID SwapByteReverse(
    UINT8           *Key
)
{
    UINTN       unKeySize = 0x100;
    UINTN       unIdx, unLastOfst;
    UINT8       u8TmpVal;

    for( unIdx=0, unLastOfst=0xff; unIdx < unKeySize/2; ++unIdx, --unLastOfst ) {
        u8TmpVal = Key[unIdx];
        Key[unIdx] = Key[unLastOfst];
        Key[unLastOfst] = u8TmpVal;
    }
}

/**    
    Verify Image hash value with signature.

    @param Pubkey - pointer to the public key
    @param unTpmPubKeySize - size of the public key
    @param messageHash - verify Image hash
    @param SignValue - pointer to the signature
    @param unSignSize - size of the signature

    @retval  EFI_SUCCESS - Set Capsule Update EFI Variable success.
**/
EFI_STATUS BIOS_Verify256(
    EFI_PEI_SERVICES    **PeiServices,
    UINT8               *Pubkey,
    UINTN               unTpmPubKeySize,
    UINT8               *messageHash,
    UINT8               *SignValue,
    UINTN               unSignSize
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINT8                   *pDecData;
    UINTN                   pDecDataSize = 0x100;
    struct crypto_rsa_key*  pPubKey = NULL;
    const UINT32            expE = 0x10001;
    UINT8                   *pPubkey = NULL;
    UINTN                   PubKeySize = 0x100;
    UINT8                   *pEncData = SignValue;
    UINTN                   unEncDataSize = unSignSize;

    Status = ((*PeiServices)->AllocatePool)(PeiServices, pDecDataSize, &pPubkey);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "Fail to allocate memory\n"));
        return Status;
    }
    CopyMem( pPubkey, Pubkey, PubKeySize );
    
    SwapByteReverse( pPubkey );

    Status = ((*PeiServices)->AllocatePool)(PeiServices, pDecDataSize, &pDecData);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "Fail to allocate memory1\n"));
        return Status;
    }

    ResetCRmm();

    pEncData = SignValue;
    unEncDataSize = unSignSize;

    pPubKey = crypto_import_rsa2048_public_key( pPubkey, PubKeySize, (UINT8*)&expE, 3);
    Status = crypto_rsa_exptmod( (UINT8*)pEncData, unEncDataSize, pDecData, &pDecDataSize, pPubKey, 0);
    if (EFI_ERROR(Status)) {
        goto Verify_RSASSAPKCS1v15_SHA1_end;
    }
    
    // Compare the Hash and Sign Hash
    if( 0 == CompareMem( &pDecData[0x100-SHA256_DIGEST_SIZE], messageHash, SHA256_DIGEST_SIZE) ) {
        Status = EFI_SUCCESS;
    } else {
        Status = EFI_UNSUPPORTED;
    }

Verify_RSASSAPKCS1v15_SHA1_end:
    //
    // security concern need to clear the key data.
    //
    SetMem(pPubkey, PubKeySize, 0xff);
    SetMem(pDecData, SHA256_DIGEST_SIZE, 0xff);

    return Status;
}

/**    
    This function is use to verify whole Image in PEI phase
    
    @param OriBiosGuardImageAddr - pointer to the verify image
    @param ImageSize - size of the verify image

    @retval  EFI_SUCCESS - Image is verify pass.
**/
EFI_STATUS
AmiBiosGuardVerifiyBiosImage(
  EFI_PEI_SERVICES         **PeiServices,
  EFI_PHYSICAL_ADDRESS     OriBiosGuardImageAddr,
  UINT32                   ImageSize
)
{
    EFI_STATUS          Status;
    BIOSGUARD_HOB       *BiosGuardHobPtr;
    UINT8               *Pubkey;
    UINT32              AmiBiosGuardHeaderLength = 0;
    BGUP_HEADER         *BgupHdr;
    VOID                *BiosGuardSha256Context;
    UINTN               BiosGuardHashDataSize = 0;
    UINT8               BiosGuardHash256Val[32];
    UINT8               *PubKeyHash;
    
    // Get stored BiosGuard public key hash
    BiosGuardHobPtr = GetFirstGuidHob (&gBiosGuardHobGuid);
    if (BiosGuardHobPtr == NULL) {        
        DEBUG ((DEBUG_ERROR, "Error! Can not get BiosGuard HOB!!!\n"));
        return EFI_NOT_FOUND;
    } 
    PubKeyHash = BiosGuardHobPtr->Bgpdt.PkeySlot0;   
    
    AmiBiosGuardHeaderLength = *(UINT32*)OriBiosGuardImageAddr;
    BgupHdr = (BGUP_HEADER*)(OriBiosGuardImageAddr + AmiBiosGuardHeaderLength);
    AmiBiosGuardHeaderLength += (BgupHdr->ScriptSectionSize + sizeof(BGUP_HEADER) + BgupHdr->DataSectionSize);    
    Pubkey = (UINT8 *)(OriBiosGuardImageAddr + AmiBiosGuardHeaderLength + 8);
    
    BiosGuardHashDataSize = Sha256GetContextSize ();
    Status = ((*PeiServices)->AllocatePool)(PeiServices, BiosGuardHashDataSize, &BiosGuardSha256Context);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "Error! Fail to allocate memory!!!\n"));
        return Status;
    }
    
    // Get the hash value of the public key
    Sha256Init (BiosGuardSha256Context);
    Sha256Update (BiosGuardSha256Context, (VOID *)Pubkey, VKEYMOD_SIZE + VKEYEXP_SIZE);
    Sha256Final (BiosGuardSha256Context,  BiosGuardHash256Val);
    
    // Verify BiosGuard public key
    if( CompareMem( BiosGuardHash256Val, PubKeyHash, SHA256_DIGEST_SIZE) ) {
        DEBUG((DEBUG_ERROR, "Error! Invalid key.\n"));
        return EFI_ABORTED;
    }
    
    // Get the hash value of the image
    Sha256Init (BiosGuardSha256Context);
    Sha256Update (BiosGuardSha256Context, (VOID *)OriBiosGuardImageAddr, ImageSize - SIGNATURE_SIZE);
    Sha256Final (BiosGuardSha256Context,  BiosGuardHash256Val);
    
    // Verify Image
    Status = BIOS_Verify256(PeiServices, Pubkey, VKEYMOD_SIZE, BiosGuardHash256Val, (UINT8 *)(OriBiosGuardImageAddr + ImageSize - SIGNATURE_SIZE), SIGNATURE_SIZE);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "Error! Image verify fail.\n"));
        return EFI_ABORTED;
    }
    
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
