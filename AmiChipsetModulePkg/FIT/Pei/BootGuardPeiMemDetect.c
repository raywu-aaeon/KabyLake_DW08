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
/** @file BootGuardPeiMemDetect.c
    Chain of trust for Pei

**/
//*************************************************************************

#include "BootGuardPei.h"
#include <CpuRegs.h>
#include <Library/TimerLib.h>
#include <AmiCspFlashLib.h>
#include <CryptLib.h>
#include <AmiRomLayout.h>
#include <Library/MtrrLib.h>
#include <SaRegs.h>
#include <AmiCspLib.h>

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
    
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP) && (FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP == 1)
#define FV_BB_AFTER_RE_BASE_GUID {0x54b7581d, 0x1387, 0x4fd5, {0xa5, 0x51, 0xc6, 0x69, 0x2c, 0x2f, 0x31, 0x7d}}
EFI_GUID   gFvBbAfterMemoryReBasePpiGuid = FV_BB_AFTER_RE_BASE_GUID;

static EFI_PEI_PPI_DESCRIPTOR gFvBbAfterMemoryReBasePpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gFvBbAfterMemoryReBasePpiGuid,
    NULL};
#endif

EFI_GUID    ReserveBootGuardFvBbAfterMemHashKeyGuid = RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY_FFS_FILE_RAW_GUID;
EFI_GUID    FvBbAfterMemInfoFfsGuid = {0x201B32CB, 0x21AD, 0x4AB3, 0xB2, 0x1D, 0xCE, 0xDD, 0x1A, 0x20, 0x53, 0xF7};
#define     FvInfoAlignment 4*1024

EFI_STATUS
BootGuardVerificationForPeiToPeiAfterMem(CONST EFI_PEI_SERVICES **PeiServices);
  
#endif

struct crypto_rsa_key *BtGcrypto_import_rsa2048_public_key(const UINT8 *KeyN, size_t lenN,
                        const UINT8 *KeyE, size_t lenE);

int BtGcrypto_rsa_exptmod(const UINT8 *in, size_t inlen, UINT8 *out, size_t *outlen,
                       struct crypto_rsa_key *key, int use_private);

EFI_STATUS
EFIAPI
PeiBtGCryptLibAllocate (
  // IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
EFIAPI
PeiBtGCryptLibFree (
  // IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

/**
    Find Guid in given Range with alignment

    @retval Addr returns a address to found location
**/
UINT32
FindGuidInRangeWithAlignment(
    UINT32      Base,
    UINT32      Length,
    EFI_GUID    *Guid,
    UINT32      Alignment
)
{
    UINT32      Addr;
    
    for(Addr = (Base + Alignment); Addr < (Base + Length); Addr += Alignment){
        if(CompareGuid( (EFI_GUID *)(Addr-sizeof(EFI_FFS_FILE_HEADER)), Guid ))
            return Addr;
    }
    return 0;
}

/**
    Loads binary from RAW section of X firwmare volume


    @retval Buffer returns a pointer to allocated memory. Caller
        must free it when done.
    @retval Size returns the size of the binary loaded into the
        buffer.

**/
EFI_STATUS
LocateBootGuardHashKey (
    IN CONST EFI_PEI_SERVICES   **PpSv,
    IN EFI_GUID                 *Guid,
    IN OUT VOID                 **Buffer
)
{
    EFI_STATUS                    Status;
    EFI_FIRMWARE_VOLUME_HEADER    *pFV;
    UINTN                         FvNum=0;
    EFI_FFS_FILE_HEADER           *ppFile=NULL;
    BOOLEAN                       Found = FALSE;

    Status = (*PpSv)->FfsFindNextVolume (PpSv, FvNum, &pFV);

    while ( TRUE ) {
        Status = (*PpSv)->FfsFindNextVolume( PpSv, FvNum, &pFV );
        if ( EFI_ERROR( Status ) ) {
            return Status;
        }

        ppFile = NULL;

        while ( TRUE ) {
            Status = (*PpSv)->FfsFindNextFile( PpSv,
                                               EFI_FV_FILETYPE_FREEFORM,
                                               pFV,
                                               &ppFile );

            if ( Status == EFI_NOT_FOUND ) {
                break;
            }
            if (CompareGuid( &ppFile->Name, Guid )) {
                Found = TRUE;
                break;
            }
        }

        if ( Found ) {
            break;
        } else {
            FvNum++;
        }
    }

    Status = (*PpSv)->FfsFindSectionData( PpSv,
                                          EFI_SECTION_RAW,
                                          ppFile,
                                          Buffer );

    if ( EFI_ERROR( Status ) ) {
        return EFI_NOT_FOUND;
    }

    return Status;
}

/**
    ACM_Success
    Check if Boot Guard ACM Verify and Measured Boot success.

    @retval EFI_STATUS
**/

EFI_STATUS 
ACM_Success(
)
{
    UINT32  MsrValue;

    MsrValue = (UINT32) AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);
    if ( ((MsrValue & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == B_BOOT_GUARD_SACM_INFO_NEM_ENABLED)  &&
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) == B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT)  &&
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) == B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) ) {
        DEBUG((DEBUG_INFO, "[BootGuardPei.c] : ACM_TPMSuccess : EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    }

    DEBUG((DEBUG_INFO,"[BootGuardPei.c] : ACM_TPMSuccess : EFI_UNSUPPORTED\n"));
    return EFI_UNSUPPORTED;
}

/**
    ACM_MeasuredBootS3Success
    Check if Boot Guard ACM Measured Boot success.

    @retval EFI_STATUS
**/

EFI_STATUS 
ACM_MeasuredBootSuccess(
)
{
    UINT32  MsrValue;

    MsrValue = (UINT32) AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);
    if ( ((MsrValue & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) == B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT)  &&
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) == B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) ) {
        DEBUG((DEBUG_INFO, "[BootGuardPei.c] : ACM_TPMS3Success : EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    }

    DEBUG((DEBUG_INFO,"[BootGuardPei.c] : ACM_TPMS3Success : EFI_UNSUPPORTED\n"));
    return EFI_UNSUPPORTED;
}

/**
    ACM_PostSuccess
    Check if Boot Guard ACM Verify Boot success.

    @retval EFI_STATUS
**/

EFI_STATUS 
ACM_VerifyBootSuccess(
)
{
    UINT32  MsrValue;

    MsrValue = (UINT32) AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);
    if ((MsrValue & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) {
        DEBUG((DEBUG_INFO, "[BootGuardPei.c] : ACM_VerifyBootSuccess : EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    }

    DEBUG((DEBUG_INFO,"[BootGuardPei.c] : ACM_VerifyBootSuccess : EFI_UNSUPPORTED\n"));
    return EFI_UNSUPPORTED;
}

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
/**    
    Action for verify fail

    @param PeiServices - PeiServices
**/
void
VerifyFailAction(
    CONST EFI_PEI_SERVICES          **PeiServices
)
{
    (*PeiServices)->ReportStatusCode (
            PeiServices,
            EFI_ERROR_CODE,                                          // Type
            INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FV_AFTER_MEMORY_FAILED_POSTCODE,  // Value
            0,
            NULL,
            NULL
    );
    PostCode (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FV_AFTER_MEMORY_FAILED_POSTCODE);
    CpuDeadLoop ();
}
#endif

/**    
    Swap byte reverse.

    @param Key - pointer to the public key
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
    CONST EFI_PEI_SERVICES    **PeiServices,
    UINT8                     *PubKey,
    UINTN                     unTpmPubKeySize, // should be 256 byte.
    UINT8                     *messageHash,
    UINT8                     *SignValue,
    UINTN                     unSignSize // should be 256 byte.    
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINT8                   *pDecData;
    UINTN                   pDecDataSize = 0x100;
    CRYPTO_RSA_KEY          *pRsaPubKey = NULL;
    const UINT32            expE = 0x10001;
    UINT8                   *pPubModulusKey = NULL;
    UINTN                   PubKeySize = 0x100;
    UINT8                   *pEncData = SignValue;
    UINTN                   unEncDataSize = unSignSize;
    
    Status = ((*PeiServices)->AllocatePool) (PeiServices, PubKeySize, &pPubModulusKey);
    if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] BIOS_Verify256 :: AllocatePool is fail for pPubModulusKey\n"));
        return   Status;
    }
    
    CopyMem(pPubModulusKey, PubKey, PubKeySize );

    SwapByteReverse( pPubModulusKey);
    
    Status = ((*PeiServices)->AllocatePool) (PeiServices, pDecDataSize, &pDecData);
    if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] BIOS_Verify256 :: AllocatePool is fail for pDecData\n"));
        return   Status;
    }
    
    ResetCRmm();
    
    pEncData = SignValue;
    unEncDataSize = unSignSize;
    
    Status = ((*PeiServices)->AllocatePool) (PeiServices, sizeof(CRYPTO_RSA_KEY), &pRsaPubKey);
    if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] BIOS_Verify256 :: AllocatePool is fail for pRsaPubKey\n"));
        return   Status;
    }
    
    pRsaPubKey = BtGcrypto_import_rsa2048_public_key( pPubModulusKey, PubKeySize, (UINT8*)&expE, 3);
    Status = BtGcrypto_rsa_exptmod( (UINT8*)pEncData, unEncDataSize, pDecData, &pDecDataSize, pRsaPubKey, 0);
    if (EFI_ERROR(Status)) {
        goto Verify_RSASSAPKCS1v15_SHA1_end;
    }
    
    // Compare the Hash and Sign Hash
    if( 0 == CompareMem( &pDecData[0x100-SHA256_DIGEST_SIZE], messageHash, SHA256_DIGEST_SIZE) ) {
        Status = EFI_SUCCESS;
    } else {
        Status = EFI_UNSUPPORTED;
    }

    if( pDecData ) {
        FreePool(pDecData);
    }

    if( pPubModulusKey ) {
        FreePool(pPubModulusKey);
    }

Verify_RSASSAPKCS1v15_SHA1_end:
    return Status;
}

/**
    BootGuardVerificationForPeiToDxeHandoffEndOfPei at end of Pei
    handler.

    @retval PeiServices Pointer to PEI Services Table.
**/
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
EFI_STATUS
EFIAPI
BootGuardVerificationForPeiToPeiAfterMem(CONST EFI_PEI_SERVICES **PeiServices)
{
    EFI_STATUS                                     Status;
    EFI_BOOT_MODE                                  BootMode;
    UINTN                                          BootGuardHashDataSize = 0;
    UINTN                                          FvAfterMemHashDataSize = 0;
    UINTN                                          FvInfoHashDataSize = 0;
    VOID                                           *BootGuardSha256Context;
    VOID                                           *FvAfterMemSha256Context;
    VOID                                           *FvInfoHashSha256Context;
    UINT8                                          PubKeyHash[32];
    UINT8                                          FvAfterMemHash[32];
    UINT8                                          FvInfoHash[32];
    UINT8                                          *BootGuardOrgFvBbAfterMemHash256;
    RESERVE_FV_REGION_INFO                         *FvRegionInfo;
    RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_INFO        *ReserveBootGuardFvBbAfterMemInfo;
    UINT32                                         ReBaseOffset;
    BOOLEAN                                        Verification = TRUE;
    UINT32                                         DataAddress;
    UINTN                                          FlashRegionAddress;
    UINTN                                          FlashRegionSize;
    MTRR_SETTINGS                                  MtrrSetting, MtrrSetting_Backup;
    BOOLEAN                                        MtrrModified = FALSE;
    UINT64                                         MaxLowMemoryLength;

    Status = PeiServicesGetBootMode (&BootMode);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] Get Boot Mode is fail\n"));
        return   Status;
    }
    
    Status = ACM_VerifyBootSuccess();
    if(EFI_ERROR(Status))
        Verification = FALSE;  // if ACM verify IBB failed, skip this verification.
#if (defined INTEL_BOOT_GUARD_S3_VERIFY_ON_ACM_SKIP) && (INTEL_BOOT_GUARD_S3_VERIFY_ON_ACM_SKIP == 1)
    // During S3 resume, verification of IBB is only enforced on every 16th S3 resume. 
    // BIOS should ignore MSR 0x13A NEMEnable bit until Verified Boot is enforced. 
    if((BootMode == BOOT_ON_S3_RESUME) && (EFI_ERROR(Status)))
        Verification = TRUE;  // if ACM don't verify IBB on S3, still perform this verification.
#endif
    if(Verification) {
        if(MtrrGetMemoryAttribute(AMI_ROM_LAYOUT_FV_BB_ADDRESS) != CacheWriteProtected) {
            MtrrModified = TRUE;
        }
        if(MtrrGetMemoryAttribute(FV_BB_AFTER_MEMORY_BASE) != CacheWriteProtected) {
            MtrrModified = TRUE;
        }
        if(MtrrModified) {
            // Get Mtrr Setting
            MtrrGetAllMtrrs(&MtrrSetting);
            // Backup original Mtrr Setting
            MtrrGetAllMtrrs(&MtrrSetting_Backup);
            // Cache all flash region
            FlashRegionSize = FLASH_SIZE;
            FlashRegionAddress = (UINTN) (0x100000000ULL - FLASH_SIZE);
            Status = MtrrSetMemoryAttributeInMtrrSettings (
                          &MtrrSetting,
                          FlashRegionAddress,
                          FlashRegionSize,
                          CacheWriteProtected
                          );

            MaxLowMemoryLength = READ_PCI32(SA_MC_BUS, 0, 0, R_SA_TOLUD) & B_SA_TOLUD_TOLUD_MASK;
            //
            // Set 1M - Tolud as WB.
            //
            MaxLowMemoryLength = (MaxLowMemoryLength + 0xFFFFFFF) & 0xF0000000;
            Status = MtrrSetMemoryAttributeInMtrrSettings (
                        &MtrrSetting,
                        0x100000,
                        MaxLowMemoryLength - 0x100000,
                        CacheWriteBack
                        );
            ASSERT_EFI_ERROR (Status);
            MtrrSetAllMtrrs (&MtrrSetting);
        }

        // Locate Hash key & Fv Info in FvBB
        Status  = LocateBootGuardHashKey(PeiServices ,&ReserveBootGuardFvBbAfterMemHashKeyGuid, &BootGuardOrgFvBbAfterMemHash256);

        if (EFI_ERROR( Status ) ) {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: Locate Hash Key & Fv Info in FvBB fail\n"));
            VerifyFailAction(PeiServices);
        }
    
        ReserveBootGuardFvBbAfterMemInfo = (RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_INFO*)BootGuardOrgFvBbAfterMemHash256;

        // Locate Public key & Signature in Fv After Memory
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP) && (FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP == 1)
        Status = (*PeiServices)->LocatePpi(
                     PeiServices,
                     &gFvBbAfterMemoryReBasePpiGuid,
                     0, NULL,
                     NULL);
       if ( EFI_ERROR( Status ) ) {
#endif
            // FvRegionInfo = (RESERVE_FV_REGION_INFO*)(FV_BB_AFTER_MEMORY_BASE + ReserveBootGuardFvBbAfterMemInfo->BootGuardFvRegionInfoOffset);
            DataAddress = FindGuidInRangeWithAlignment(FV_BB_AFTER_MEMORY_BASE, FV_BB_AFTER_MEMORY_SIZE, &FvBbAfterMemInfoFfsGuid, FvInfoAlignment);
            FvRegionInfo = (RESERVE_FV_REGION_INFO*)DataAddress;
            ReBaseOffset = 0;
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP) && (FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP == 1)
        }else{
            // FvRegionInfo = (RESERVE_FV_REGION_INFO*)(FV_AFTER_MEMORY_BACKUP_ADDRESS + ReserveBootGuardFvBbAfterMemInfo->BootGuardFvRegionInfoOffset);
            DataAddress = FindGuidInRangeWithAlignment(FV_AFTER_MEMORY_BACKUP_ADDRESS, FV_BB_AFTER_MEMORY_SIZE, &FvBbAfterMemInfoFfsGuid, FvInfoAlignment);
            FvRegionInfo = (RESERVE_FV_REGION_INFO*)DataAddress;
            if(FvRegionInfo->Flag && BIT0) {
                // Build Time Backup
                ReBaseOffset = 0;
            } else {
                // Run Time Backup
                ReBaseOffset = FV_BB_AFTER_MEMORY_BASE - FV_AFTER_MEMORY_BACKUP_ADDRESS;
            }
        }
#endif
        if(!DataAddress){
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: Can NOT find the FvRegionInfo\n"));
            goto ResetMtrr;
        }
        
        // Calculate Hash of Public Key in Fv After Memory
        BootGuardHashDataSize = Sha256GetContextSize ();
        Status = ((*PeiServices)->AllocatePool) (PeiServices, BootGuardHashDataSize, &BootGuardSha256Context);
        if ( EFI_ERROR( Status ) ) {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: AllocatePool is fail for BootGuardSha256Context\n"));
            goto ResetMtrr;
        }

        Sha256Init (BootGuardSha256Context);
        Sha256Update (BootGuardSha256Context, (UINT8 *)FvRegionInfo->FvRegionSignature.Key.Modulus, RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT);
        Sha256Final (BootGuardSha256Context,  PubKeyHash);

        if(CompareMem(PubKeyHash, ReserveBootGuardFvBbAfterMemInfo->BootGuardFvRegionHashKey, 32)) {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: Public Key verify failed\n"));
            VerifyFailAction(PeiServices);
        }
        
        // Calculate Hash of Fv After Memory
        FvAfterMemHashDataSize = Sha256GetContextSize ();
        Status = ((*PeiServices)->AllocatePool) (PeiServices, FvAfterMemHashDataSize, &FvAfterMemSha256Context);
        if ( EFI_ERROR( Status ) ) {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: AllocatePool is fail for FvAfterMemSha256Context\n"));
            goto ResetMtrr;
        }

        Sha256Init (FvAfterMemSha256Context);
        Sha256Update (FvAfterMemSha256Context, (UINT8 *)FvRegionInfo->BootGuardFvAreaSegmentBase[0] - ReBaseOffset, FvRegionInfo->BootGuardFvAreaSegmentSize[0]);
        Sha256Update (FvAfterMemSha256Context, (UINT8 *)FvRegionInfo->BootGuardFvAreaSegmentBase[1] - ReBaseOffset, FvRegionInfo->BootGuardFvAreaSegmentSize[1]);            
        Sha256Final (FvAfterMemSha256Context,  FvAfterMemHash);
        
        if(CompareMem(FvAfterMemHash, FvRegionInfo->FvHash.HashBuffer, 32)) {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: Hash of Fv After Memory verify failed\n"));
            VerifyFailAction(PeiServices);
        }

        // Calculate Hash of Fv Info (Hash of Fv After Memory) for providing signature verify (messageHash)
        FvInfoHashDataSize = Sha256GetContextSize ();
        Status = ((*PeiServices)->AllocatePool) (PeiServices, FvInfoHashDataSize, &FvInfoHashSha256Context);
        if ( EFI_ERROR( Status ) ) {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: AllocatePool is fail for FvInfoHashSha256Context\n"));
            goto ResetMtrr;
        }

        Sha256Init (FvInfoHashSha256Context);
        // Sha256Update (FvInfoHashSha256Context, (UINT8 *)FvAfterMemHash, 32);
        Sha256Update (FvInfoHashSha256Context, (UINT8 *)FvRegionInfo, sizeof(RESERVE_FV_REGION_INFO) - sizeof(KEY_SIGNATURE_STRUCT));
        Sha256Final (FvInfoHashSha256Context,  FvInfoHash);


        // Verify Signature
        Status = BIOS_Verify256(
                                PeiServices, 
                                (UINT8 *)FvRegionInfo->FvRegionSignature.Key.Modulus, 
                                0x100,
                                FvInfoHash,
                                FvRegionInfo->FvRegionSignature.Signature.Signature, 
                                0x100
                                );
        if ( EFI_ERROR( Status ) ) {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: Signature Failed!!!\n"));
            VerifyFailAction(PeiServices);
        } else {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: Verify Pass!!!\n"));
        }
    }
ResetMtrr:
    if(MtrrModified)
        MtrrSetAllMtrrs (&MtrrSetting_Backup);

    return   Status;
}
#endif

EFI_STATUS
EFIAPI
BootGuardPeiMemDetectEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    
    if ( IsBootGuardSupported() == FALSE ) {
        return   Status;
    }

    if ( (UINT32)AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) == 0 ) {
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] Boot Guard is disabled by Boot Guard Profile Configuration in the Intel Fitc\n"));
        return   Status;
    }
    
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1) 
    PeiBtGCryptLibAllocate(PeiServices);
    Status = BootGuardVerificationForPeiToPeiAfterMem(PeiServices);
    PeiBtGCryptLibFree(PeiServices);
#endif

    return   Status;
}

BOOLEAN
BtGFtFvBeforePublishing (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT AMI_ROM_AREA  *Area,
    IN     UINT32        FvType
)
{
    EFI_STATUS                             Status = EFI_SUCCESS;
    // cppcheck-suppress unreadVariable
    const UINTN                            FvRecoveryBase = FV_BB_AFTER_MEMORY_BASE;
    // cppcheck-suppress unreadVariable
    EFI_FIRMWARE_VOLUME_HEADER             *FvRecoveryHdr = (EFI_FIRMWARE_VOLUME_HEADER*)FV_BB_AFTER_MEMORY_BASE;
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
    EFI_GUID                               FvAfterMemoryGuid = {0x14E428FA, 0x1A12, 0x4875, {0xB6, 0x37, 0x8B, 0x3C, 0xC8, 0x7F, 0xDF, 0x07}};
    EFI_GUID                               FvAfterMemoryBackupGuid = {0x5b08a058, 0x784f, 0x4938, {0x9a, 0x49, 0x15, 0x88, 0xaa, 0x05, 0xf4, 0xb9}};
#endif

    if ( IsBootGuardSupported() == FALSE ) {
        return TRUE;
    }

    if ( (UINT32)AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) == 0 ) {
        DEBUG ((DEBUG_INFO, "[BootGuardPeiMemoryDetect.c] Boot Guard is disabled by Boot Guard Profile Configuration in the Intel Fitc\n"));
        return TRUE;
    }

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
    ///
    /// Verify FV_BB_AFTER_MEMORY
    ///
    if (CompareGuid(&FvAfterMemoryGuid, &Area->Guid) || CompareGuid(&FvAfterMemoryBackupGuid, &Area->Guid)) {
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP) && (FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP == 1)
        if (BUILD_TIME_BACKUP) {
            if (!IsTopSwapOn()) {
                goto EndOfRebasePPI;
            }
            // update address of recovery FV to its backup copy
            if (Area->Address == FvRecoveryBase) {
                (*PeiServices)->InstallPpi(PeiServices, &gFvBbAfterMemoryReBasePpi);
            }
        } else {
            if (!IsTopSwapOn() && (FvRecoveryHdr->Signature == EFI_FVH_SIGNATURE)) { //TopSwap if OFF - do nothing
                goto EndOfRebasePPI;
            }
            
            // update address of recovery FV to its backup copy
            if (Area->Address == FvRecoveryBase) {
                if (FvRecoveryHdr->Signature != EFI_FVH_SIGNATURE) {
                    (*PeiServices)->InstallPpi(PeiServices, &gFvBbAfterMemoryReBasePpi);
                }
            }
        }
EndOfRebasePPI:
#endif
        PeiBtGCryptLibAllocate(PeiServices);
        Status = BootGuardVerificationForPeiToPeiAfterMem(PeiServices);
        PeiBtGCryptLibFree(PeiServices);
    }          
#endif //FV_BB_VERIFY_FV_AFTER_MEMORY
   return TRUE;
        
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
