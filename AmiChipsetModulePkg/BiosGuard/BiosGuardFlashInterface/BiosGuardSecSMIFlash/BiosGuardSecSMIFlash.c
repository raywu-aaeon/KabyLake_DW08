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

/** @file BiosGuardSecSMIFlash.c
    This file contains code for Bios Guard SMM

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/SecSmiFlash.h>
#include <Library/AmiBufferValidationLib.h>
#include <CpuRegs.h>
#include <Capsule.h>
#include <Guid/CapsuleVendor.h>
#include <Protocol/AmiBiosGuardProtocol.h>
#include <Protocol/AmiDigitalSignature.h>
#include "BaseCryptLib.h"
#include <BiosGuard.h>
#include <Library/HobLib.h>
#include <AmiBiosGuard.h>
#include <Sb.h>
#include <Library/AmiBiosGuardVerifyOemBinLib.h>
#include <Library/AmiBiosGuardMiscLib.h>
#include "AmiCertificate.h"
#include <RomLayout.h>
#include <Fid.h>

//
//  GUID
//
static EFI_GUID     gAmiBiosGuardSecSmiFlashProtocolGuid = AMI_BIOSGUARD_SEC_SMI_FLASH_PROTOCOL_GUID;
EFI_GUID            BiosGuardCapsuleGuid = BIOS_GUARD_CAPSULE_GUID;
EFI_GUID            BiosGuardRecoveryGuid = BIOS_GUARD_RECOVERY_GUID;
EFI_GUID            BiosGuardRecoveryAddressGuid = BIOS_GUARD_RECOVERY_ADDRESS_GUID;
EFI_GUID            BiosGuardCapsuleVariableGuid = BIOS_GUARD_CAPSULE_VARIABLE_GUID;
EFI_GUID            SbWarmResetGuid = SB_WARM_RESET_GUID;
extern EFI_GUID     gEfiCapsuleVendorGuid;

//
//  Global Variable
//
static EFI_PHYSICAL_ADDRESS gBiosGuardFwCapsuleBuffer = 0;
UINT32                      gBiosGuardFwCapsuleSize = 0; 
UINT32                      gBiosguardFwCapMaxSize = 0;
static EFI_PHYSICAL_ADDRESS gpFwBiosGuardCapsuleMailbox = 0;
static UINTN                gpFwBiosGuardCapsuleMailboxSize = 0;
static CHAR16               gCapsuleNameBuffer[30];
EFI_CAPSULE_HEADER          *gBiosGuardCapsuleHdr;
EFI_SMM_VARIABLE_PROTOCOL   *mSmmVariable;
UINT8                       *BiosGuardPubKeyHash256Val;


//
//  Function Declaration
//
EFI_STATUS 
EFIAPI
LoadBiosGuardFwImage(
    IN OUT FUNC_BLOCK   *pFuncBlock
);

EFI_STATUS 
EFIAPI
SetBiosGuardFlUpdMethod(
    IN OUT FUNC_FLASH_SESSION_BLOCK  *pSessionBlock
);

EFI_STATUS 
EFIAPI
CheckMeStateDisable(
    VOID
);

AMI_BIOSGUARD_SEC_SMI_FLASH_PROTOCOL    BiosGuardSecSmiFlash = {
    LoadBiosGuardFwImage,
    SetBiosGuardFlUpdMethod,
    CheckMeStateDisable
};

#if defined (BIOS_GUARD_REVISION_CHECK_SUPPORT) && (BIOS_GUARD_REVISION_CHECK_SUPPORT == 1)
#if IGNORE_RUNTIME_UPDATE_IMAGE_REVISION_CHECK == 0

static EFI_PHYSICAL_ADDRESS gBiosGuardReOrgImageBuffer = 0;

EFI_GUID            FWCapsuleGuid = APTIO_FW_CAPSULE_GUID;
static EFI_GUID     FwCapFfsGuid = AMI_FW_CAPSULE_FFS_GUID;
static EFI_GUID     FwCapSectionGuid = AMI_FW_CAPSULE_SECTION_GUID;
static EFI_GUID FidSectionGuid = \
    { 0x2EBE0275, 0x6458, 0x4AF9, 0x91, 0xed, 0xD3, 0xF4, 0xED, 0xB1, 0x00, 0xAA };

const UINT8 *FidSignature = "$FID";

typedef struct _FID_SECTION {
    EFI_GUID   Guid;
    FW_VERSION FwVersion;
} FID_SECTION;

/**    
    Verify project ID.

    @param FwVersionData - pointer to the FwVersionData
    
    @retval  EFI_SUCCESS - Verify project ID success.
    @retval  EFI_SECURITY_VIOLATION - Project ID not match or version check fail.
    @retval  EFI_INCOMPATIBLE_VERSION - Version check fail.
**/
EFI_STATUS
VerifyProjectId (
    IN FW_VERSION      *FwVersionData
)
{
/*
    CHAR8       BiosTag[9];     //BIOS Tag
    EFI_GUID    FirmwareGuid;       //Firmware GUID
    CHAR8       CoreMajorVersion[3];    
    CHAR8       CoreMinorVersion[3];
    CHAR8       ProjectMajorVersion[3];
    CHAR8       ProjectMinorVersion[3];
*/
// Project ID, Major, Minor rev 
    char  *strOrgProjectId = CONVERT_TO_STRING(PROJECT_TAG);
    UINTN  Size = sizeof(CONVERT_TO_STRING(PROJECT_TAG));
#if IGNORE_FID_FW_VERSION_GUID_CHECK == 0    
    EFI_GUID FirmwareGuid = FW_VERSION_GUID;
#endif    
#if 0    
DEBUG_CODE (
    char   strProjectId[sizeof(CONVERT_TO_STRING(PROJECT_TAG))];

    MemSet(&strProjectId, Size, 0);
    MemCpy( strProjectId, FwVersionData->BiosTag, Size-1);
    DEBUG ((DEBUG_INFO, "Org/New BiosTag: %a=%a\nOrg/New ProjVer: %d=%d\nOrg/New FwGuid: %g=%g\n",
        strOrgProjectId, strProjectId,(PROJECT_MAJOR_VERSION*100+PROJECT_MINOR_VERSION),
        (Atoi(FwVersionData->ProjectMajorVersion)*100+Atoi(FwVersionData->ProjectMinorVersion)),
        &FirmwareGuid, &(FwVersionData->FirmwareGuid)
    ));
);
#endif
    if (!Size || MemCmp (strOrgProjectId, FwVersionData->BiosTag, Size-1)) return EFI_SECURITY_VIOLATION;
#if IGNORE_FID_FW_VERSION_GUID_CHECK == 0
    if(guidcmp(&FirmwareGuid, &(FwVersionData->FirmwareGuid)) ) return EFI_SECURITY_VIOLATION;
#endif

#if IGNORE_IMAGE_ROLLBACK == 0
    if((Atoi(FwVersionData->ProjectMajorVersion)*100+Atoi(FwVersionData->ProjectMinorVersion)) <
       (PROJECT_MAJOR_VERSION*100+PROJECT_MINOR_VERSION) 
     ){
#if (defined(REFLASH_INTERACTIVE) && REFLASH_INTERACTIVE==1)
        // Physically present user may still override the roll-back protection block 
        // from the ReFlash Setup screen after the Fw reset on Capsule update flow
        return EFI_INCOMPATIBLE_VERSION;
#else
        return EFI_SECURITY_VIOLATION;
#endif
    }
#endif

    return EFI_SUCCESS;
}

/**    
    Get Fid Data.

    @param pFV - pointer to the firmware volume
    @param Size - Size of the firmware volume
    @param FwVersionData - pointer to the FwVersionData
    
    @retval  TRUE - Found Fid data.
    @retval  FALSE - Fid data not found.
**/
BOOLEAN GetFidData(
    IN  VOID           *pFV,
    IN  UINT32          Size,
    OUT FW_VERSION    **FwVersionData
)
{
//    UINT32 Signature;
    UINT8 *SearchPointer;
    FID_SECTION *Section;

    if (pFV == NULL) return FALSE;

// Simplified search by $FID signature only.
//    SearchPointer = (UINT32 *)((UINT8 *)pFV + sizeof(EFI_GUID));
//    Signature = FidSectionGuid.Data1;
    SearchPointer = (UINT8 *)pFV;

    do {
//        if(*SearchPointer == Signature) {
            Section = (FID_SECTION *)SearchPointer;
            if(!guidcmp(&FidSectionGuid, &(Section->Guid)) && 
               (*((UINT32*)(&Section->FwVersion.FirmwareID[0])) == *(UINT32*)FidSignature)){ 
                *FwVersionData = &Section->FwVersion;
                return TRUE;
            }
//        }
    } while( SearchPointer++ < (UINT8*)((UINTN)pFV+Size));

    return FALSE;
}

/**    
    Verify Fw Revision.

    @param FWCapsuleHdr - Pointer to the firmware capsule header
    @param RomData - Pointer to the Bios data.
    
    @retval  EFI_SUCCESS - Verify Fw Revision pass.
    @retval  EFI_SECURITY_VIOLATION - Verify Fw Revision fail.
**/
EFI_STATUS
VerifyFwRevision (
    IN APTIO_FW_CAPSULE_HEADER  *FWCapsuleHdr,
    IN UINT8 *RomData
)    
{
    ROM_AREA                *Area;
    EFI_PHYSICAL_ADDRESS    FvAddress;
    FW_VERSION             *FwVersionData;

    if (FWCapsuleHdr == NULL) return EFI_INVALID_PARAMETER;
    if (RomData == NULL) return EFI_INVALID_PARAMETER;

    Area = (ROM_AREA *)((UINTN)FWCapsuleHdr+FWCapsuleHdr->RomLayoutOffset);

    for (Area; Area->Size != 0; Area++) {
        if (!(Area->Attributes & ROM_AREA_FV_SIGNED)) 
            continue;
        //  $FID can be in FV with either PEI or DXE
        if (!(Area->Attributes & (ROM_AREA_FV_PEI+ROM_AREA_FV_DXE))) 
            continue;

        FvAddress = (EFI_PHYSICAL_ADDRESS)RomData + (Area->Offset);
        if (GetFidData((UINT8*)FvAddress, Area->Size, &FwVersionData)) {
            return VerifyProjectId(FwVersionData);
        }
    } 
// At least one FW block must be signed OR no $FID structure found in the new FW image
    return EFI_SECURITY_VIOLATION;
}

/**    
    Function to read FW Cap Sig data from Ffs.

    @param pCapsule - pointer to data buffer to read from
    @param pFfsData - Pointer to the firmware capsule header.
    
    @retval  EFI_SUCCESS if Capsule Hdr with Signature is retrieved
**/
EFI_STATUS
BiosGuardFindCapHdrFFS (
    IN  VOID    *pCapsule,
    OUT UINT8   **pFfsData
)
{
    UINT32                          Signature;
    UINT32                          *SearchPointer;
    AMI_FFS_COMMON_SECTION_HEADER   *FileSection;
    APTIO_FW_CAPSULE_HEADER         *pFwCapHdr;
    
    if (pCapsule == NULL) return EFI_INVALID_PARAMETER;

    SearchPointer = (UINT32 *)((UINT8 *)pCapsule - sizeof(AMI_FFS_COMMON_SECTION_HEADER) + FLASH_SIZE);
    Signature = FwCapFfsGuid.Data1;

    do {
        if ( *SearchPointer == Signature ) {
            FileSection = (AMI_FFS_COMMON_SECTION_HEADER *)SearchPointer;
            if ( CompareGuid(&FwCapFfsGuid, &(FileSection->FfsHdr.Name)) && CompareGuid(&FwCapSectionGuid, &(FileSection->SectionGuid)) ) {
                pFwCapHdr = (APTIO_FW_CAPSULE_HEADER*)(FileSection->FwCapHdr);
                // just a sanity check - Cap Size must match the Section size
                if (((*(UINT32 *)FileSection->FfsHdr.Size) & 0xffffff) >= pFwCapHdr->CapHdr.HeaderSize + sizeof(AMI_FFS_COMMON_SECTION_HEADER) &&
                        CompareGuid((EFI_GUID*)&pFwCapHdr->CapHdr.CapsuleGuid, &FWCapsuleGuid)) {
                    *pFfsData = (UINT8*)pFwCapHdr;
                    return EFI_SUCCESS;
                }
            }
        }
    } while ( SearchPointer-- != pCapsule );

    return EFI_NOT_FOUND;
}
#endif
#endif

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
    UINT8           *Pubkey,
    UINTN           unTpmPubKeySize,
    UINT8           *messageHash,
    UINT8           *SignValue,
    UINTN           unSignSize
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

    Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, pDecDataSize, &pPubkey);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    CopyMem( pPubkey, Pubkey, PubKeySize );
    
    SwapByteReverse( pPubkey );

    Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, pDecDataSize, &pDecData);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
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

    if( pDecData ) {
        SetMem(pDecData, pDecDataSize, 0xff);
        gSmst->SmmFreePool(pDecData);
    }

    if( pPubkey ) {
        SetMem(pPubkey, PubKeySize, 0xff);
        gSmst->SmmFreePool(pPubkey);
    }

    return Status;
}

/**    
    This function is use to verify FW revision    

    @retval  EFI_SUCCESS - Verify FW revision pass.
**/
EFI_STATUS 
BiosGuardVerifyFwRevision (VOID)
{
#if defined (BIOS_GUARD_REVISION_CHECK_SUPPORT) && (BIOS_GUARD_REVISION_CHECK_SUPPORT == 1)
#if IGNORE_RUNTIME_UPDATE_IMAGE_REVISION_CHECK == 0
    EFI_STATUS                      Status;
    UINT32                          CurrentBGUPAddress = 0;
    APTIO_FW_CAPSULE_HEADER         *FWCapsuleHdr;
    
    // Re-Org BIOS image.
    AmiBiosGuardReOrgnizeRomImage(gBiosGuardFwCapsuleBuffer, gBiosGuardReOrgImageBuffer, gBiosGuardFwCapsuleSize, &CurrentBGUPAddress);
    
    // Get the FWCapsuleHdr
    if (FWCAPSULE_FILE_FORMAT) {
        FWCapsuleHdr = (APTIO_FW_CAPSULE_HEADER*)(gBiosGuardFwCapsuleBuffer + gBiosGuardFwCapsuleSize - FWCAPSULE_IMAGE_ALLIGN - SIGNATURE_SIZE);            
    } else {
        if ( EFI_ERROR(BiosGuardFindCapHdrFFS((VOID*)(gBiosGuardReOrgImageBuffer + CurrentBGUPAddress - FLASH_SIZE), (UINT8**)&FWCapsuleHdr)) ) {
            ASSERT_EFI_ERROR (EFI_SECURITY_VIOLATION);
            return EFI_SECURITY_VIOLATION;
        }        
    }
    
    Status = VerifyFwRevision(FWCapsuleHdr, (UINT8 *)(gBiosGuardReOrgImageBuffer + CurrentBGUPAddress - FLASH_SIZE));
    if (EFI_ERROR(Status)) {
        return EFI_SECURITY_VIOLATION;
    }
#endif
#endif
    return EFI_SUCCESS;
}

/**    
    This function is use to verify whole Image    

    @retval  EFI_SUCCESS - Image is verify pass.
**/
EFI_STATUS 
VerifyBiosGuardImage (VOID)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    UINT8                           *Pubkey;
    UINT32                          AmiBiosGuardHeaderLength = 0;
    BGUP_HEADER                     *BgupHdr;
    VOID                            *BiosGuardSha256Context;
    UINTN                           BiosGuardHashDataSize = 0;
    UINT8                           BiosGuardHash256Val[32];

    AmiBiosGuardHeaderLength = *(UINT32*)gBiosGuardFwCapsuleBuffer;
    BgupHdr = (BGUP_HEADER*)(gBiosGuardFwCapsuleBuffer + AmiBiosGuardHeaderLength);
    AmiBiosGuardHeaderLength += (BgupHdr->ScriptSectionSize + sizeof(BGUP_HEADER) + BgupHdr->DataSectionSize);    
    Pubkey = (UINT8 *)(gBiosGuardFwCapsuleBuffer + AmiBiosGuardHeaderLength + 8);
    
    BiosGuardHashDataSize = Sha256GetContextSize ();    
    Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, BiosGuardHashDataSize, &BiosGuardSha256Context);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    // Get the hash value of the public key
    Sha256Init (BiosGuardSha256Context);
    Sha256Update (BiosGuardSha256Context, (VOID *)Pubkey, VKEYMOD_SIZE + VKEYEXP_SIZE);
    Sha256Final (BiosGuardSha256Context,  BiosGuardHash256Val);
    
    // Verify BiosGuard public key
    if( CompareMem( BiosGuardHash256Val, BiosGuardPubKeyHash256Val, SHA256_DIGEST_SIZE) ) {
        DEBUG((DEBUG_ERROR, "Error! Invalid key.\n"));
        return EFI_ABORTED;
    }
    
    // Get the hash value of the image
    Sha256Init (BiosGuardSha256Context);
    Sha256Update (BiosGuardSha256Context, (VOID *)gBiosGuardFwCapsuleBuffer, gBiosGuardFwCapsuleSize - SIGNATURE_SIZE);
    Sha256Final (BiosGuardSha256Context,  BiosGuardHash256Val);
    
    // Verify Image
    Status = BIOS_Verify256(Pubkey, VKEYMOD_SIZE, BiosGuardHash256Val, (UINT8 *)(gBiosGuardFwCapsuleBuffer + gBiosGuardFwCapsuleSize - SIGNATURE_SIZE), SIGNATURE_SIZE);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "Error! Image verify fail.\n"));
        return EFI_ABORTED;
    }
    
    gSmst->SmmFreePool(BiosGuardSha256Context);
    
    return EFI_SUCCESS;
}

/**
 * Capsule NVRAM variables are owned by either the runtime Capsule Update service 
 * or by this driver. Each service must not override previously created instances 
 * of the variable and create a new one with n+1 index
 */
CHAR16* GetLastFwCapsuleVariable()
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN Index = 0;
    EFI_PHYSICAL_ADDRESS    IoData=0;
    UINTN                   Size;

    Swprintf_s(gCapsuleNameBuffer, 30, L"%s", EFI_CAPSULE_VARIABLE_NAME);
    // get any NVRAM variable of the format L"CapsuleUpdateDataN" where N is an integer
    while(!EFI_ERROR(Status)) {
        if(Index > 0 )
            Swprintf_s(gCapsuleNameBuffer, 30, L"%s%d", EFI_CAPSULE_VARIABLE_NAME, Index);
        Size = sizeof(EFI_PHYSICAL_ADDRESS);
        Status = mSmmVariable->SmmGetVariable( gCapsuleNameBuffer, &gEfiCapsuleVendorGuid,
                                    NULL,
                                    &Size, 
                                    &IoData);
        DEBUG((DEBUG_INFO, "Get '%S' %r, %lX\n", gCapsuleNameBuffer, Status, IoData));
        if(!EFI_ERROR(Status) && 
           (IoData == gpFwBiosGuardCapsuleMailbox && 
            !EFI_ERROR(AmiValidateSmramBuffer((VOID*)IoData, gpFwBiosGuardCapsuleMailboxSize)))
        ) {
            break;
        }
        Index++;
    }

    return gCapsuleNameBuffer;
}

/**    
    This function prepares Capsule Update EFI Variable

    @param pSessionBlock

    @retval  EFI_SUCCESS - Set Capsule Update EFI Variable success.
**/
EFI_STATUS 
UpdateCapsule (
    IN FUNC_FLASH_SESSION_BLOCK *pSessionBlock
)
{
    EFI_STATUS                      Status;
    EFI_CAPSULE_BLOCK_DESCRIPTOR    *pCapsuleMailboxPtr;
    UINT32                          Attributes;
    CHAR16                          *EfiFwCapsuleVarName = NULL;
    AMI_BIOS_GUARD_RECOVERY         *ApRecoveryAddress;
    UINTN                           BiosGuardRecoveryAddressVarSize = sizeof(UINT32);

    pCapsuleMailboxPtr = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)(UINTN)gpFwBiosGuardCapsuleMailbox;
    
    // Initial gBiosGuardCapsuleHdr
    CopyGuid(&gBiosGuardCapsuleHdr->CapsuleGuid, &BiosGuardCapsuleGuid);
    gBiosGuardCapsuleHdr->HeaderSize = sizeof(EFI_CAPSULE_HEADER);
    gBiosGuardCapsuleHdr->CapsuleImageSize = gBiosGuardFwCapsuleSize + sizeof(EFI_CAPSULE_HEADER);
    gBiosGuardCapsuleHdr->Flags = CAPSULE_FLAGS_PERSIST_ACROSS_RESET;

    pCapsuleMailboxPtr[0].Length = gBiosGuardCapsuleHdr->HeaderSize;
    pCapsuleMailboxPtr[0].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)(UINTN)gBiosGuardCapsuleHdr;
    
    pCapsuleMailboxPtr[1].Length = gBiosGuardFwCapsuleSize;
    pCapsuleMailboxPtr[1].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)(UINTN)gBiosGuardFwCapsuleBuffer;
    
    pCapsuleMailboxPtr[2].Length = 0;
    pCapsuleMailboxPtr[2].Union.DataBlock = 0;

    EfiFwCapsuleVarName = GetLastFwCapsuleVariable();
    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS;

    //Clear old Capsule Update EFI Variable
    mSmmVariable->SmmSetVariable( EfiFwCapsuleVarName, &gEfiCapsuleVendorGuid, 0, 0, NULL);

    //Set Capsule Update EFI Variable
    Status = mSmmVariable->SmmSetVariable( EfiFwCapsuleVarName, 
                                           &gEfiCapsuleVendorGuid, 
                                           Attributes, 
                                           sizeof(EFI_PHYSICAL_ADDRESS), 
                                           (VOID*)&gpFwBiosGuardCapsuleMailbox);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = mSmmVariable->SmmGetVariable( L"BiosGuardRecoveryAddressVariable",
                                           &BiosGuardRecoveryAddressGuid,
                                           NULL,
                                           &BiosGuardRecoveryAddressVarSize,
                                           &ApRecoveryAddress);
    if ( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return Status;
    }

    ApRecoveryAddress->ReoveryMode = BiosGuardCapsuleMode;
    ApRecoveryAddress->ReoveryAddress = (UINT32)gpFwBiosGuardCapsuleMailbox;
    
    return Status;
}

/**    
    This function save update Bios Image to gBiosGuardFwCapsuleBuffer

    @param pFuncBlock

    @retval  EFI_SUCCESS - Load image success.
    @retval  EFI_DEVICE_ERROR - Load image fail.
**/
EFI_STATUS
EFIAPI
LoadBiosGuardFwImage(
    IN OUT FUNC_BLOCK   *pFuncBlock
)
{
#if BIOS_GUARD_DEBUG_MODE
    DEBUG((DEBUG_INFO, "LoadBiosGuardFwImage enter\n"));
#endif    
    
    if(gBiosGuardFwCapsuleBuffer == 0 || pFuncBlock == NULL)
        return EFI_DEVICE_ERROR;

    pFuncBlock->ErrorCode = 1;
    
#if BIOS_GUARD_DEBUG_MODE    
    DEBUG((DEBUG_INFO,"BiosGuard SecSMI. LoadImage at %lX, Size is 0x%X\n",(UINTN)gBiosGuardFwCapsuleBuffer + pFuncBlock->BlockAddr, pFuncBlock->BlockSize));
#endif    
    if(EFI_ERROR(AmiValidateMemoryBuffer(pFuncBlock, sizeof(FUNC_BLOCK))))
        return EFI_DEVICE_ERROR;

    // prevent leaking of the SMM code to the external buffer
    if(EFI_ERROR(AmiValidateMemoryBuffer((VOID*)(pFuncBlock->BufAddr), pFuncBlock->BlockSize)))
        return EFI_DEVICE_ERROR;

    // assuming the address in 0 based offset in new ROM image
    if(((UINT64)pFuncBlock->BlockAddr + pFuncBlock->BlockSize) > (UINT64)gBiosguardFwCapMaxSize) {
        DEBUG((DEBUG_INFO, "Error : pFuncBlock->BlockAddr + pFuncBlock->BlockSize > gBiosguardFwCapMaxSize(%lx)\n", gBiosguardFwCapMaxSize));
        return EFI_DEVICE_ERROR;
    }

    MemCpy((VOID*)((UINTN)gBiosGuardFwCapsuleBuffer + pFuncBlock->BlockAddr), 
            (VOID*)pFuncBlock->BufAddr, pFuncBlock->BlockSize);

    pFuncBlock->ErrorCode = (UINT8)MemCmp(
        (VOID*)((UINTN)gBiosGuardFwCapsuleBuffer + pFuncBlock->BlockAddr), 
        (VOID*)pFuncBlock->BufAddr, pFuncBlock->BlockSize)==0?0:1;
    
    gBiosGuardFwCapsuleSize += pFuncBlock->BlockSize;

    return (pFuncBlock->ErrorCode == 0) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/**    
    This function will set FLASH_UPDATE_VAR

    @param pSessionBlock

    @retval EFI_SUCCESS - FLASH_UPDATE_VAR set success.
    @retval EFI_DEVICE_ERROR - FLASH_UPDATE_VAR set fail.
**/
EFI_STATUS SetFlashUpdateVar (
    IN FUNC_FLASH_SESSION_BLOCK    *pSessionBlock
){
    EFI_STATUS                      Status;
    EFI_GUID                        FlashUpdGuid  = FLASH_UPDATE_GUID;
    UINT32                          Attributes;
    
    if (pSessionBlock == NULL) return EFI_INVALID_PARAMETER;

    if(pSessionBlock->FlUpdBlock.FlashOpType == FlRecovery &&
        pSessionBlock->FlUpdBlock.FwImage.AmiRomFileName[0] == 0) {
        return EFI_DEVICE_ERROR;
    }
    
    Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;    
    mSmmVariable->SmmSetVariable ( FLASH_UPDATE_VAR, &FlashUpdGuid, 0, 0, NULL);
    Status = mSmmVariable->SmmSetVariable ( FLASH_UPDATE_VAR,
                                            &FlashUpdGuid,
                                            Attributes,
                                            sizeof(AMI_FLASH_UPDATE_BLOCK),
                                            (VOID*) &pSessionBlock->FlUpdBlock );
    if ( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return EFI_DEVICE_ERROR;
    }
    
    return EFI_SUCCESS;
}

/**    
    This function will set FLASH_UPDATE_VAR and BiosGuardRecoveryAddressVariable

    @param pSessionBlock

    @retval EFI_SUCCESS - FLASH_UPDATE_VAR set success.
    @retval EFI_DEVICE_ERROR - FLASH_UPDATE_VAR set fail.
**/
EFI_STATUS SetBiosGuardFlashUpdateVar (
    IN FUNC_FLASH_SESSION_BLOCK    *pSessionBlock
){
    EFI_STATUS                      Status;
    EFI_CAPSULE_BLOCK_DESCRIPTOR    *pCapsuleMailboxPtr;
    AMI_BIOS_GUARD_RECOVERY         *ApRecoveryAddress;
    UINTN                           BiosGuardRecoveryAddressVarSize = sizeof(UINT32);
    
    if (pSessionBlock == NULL) return EFI_INVALID_PARAMETER;

    Status = SetFlashUpdateVar(pSessionBlock);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    // Initial gBiosGuardCapsuleHdr
    CopyGuid(&gBiosGuardCapsuleHdr->CapsuleGuid, &BiosGuardRecoveryGuid);
    gBiosGuardCapsuleHdr->HeaderSize = sizeof(EFI_CAPSULE_HEADER);
    gBiosGuardCapsuleHdr->CapsuleImageSize = 0;
    gBiosGuardCapsuleHdr->Flags = 0;
    
    pCapsuleMailboxPtr = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)(UINTN)gpFwBiosGuardCapsuleMailbox;       
    pCapsuleMailboxPtr[0].Length = gBiosGuardCapsuleHdr->HeaderSize;
    pCapsuleMailboxPtr[0].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)(UINTN)gBiosGuardCapsuleHdr;    
    pCapsuleMailboxPtr[1].Length = 0;
    pCapsuleMailboxPtr[1].Union.DataBlock = 0;    
    
    Status = mSmmVariable->SmmGetVariable( L"BiosGuardRecoveryAddressVariable",
                                           &BiosGuardRecoveryAddressGuid,
                                           NULL,
                                           &BiosGuardRecoveryAddressVarSize,
                                           &ApRecoveryAddress);
    if ( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return EFI_DEVICE_ERROR;
    }
    
    ApRecoveryAddress->ReoveryMode = BiosGuardRecoveryMode;
    ApRecoveryAddress->ReoveryAddress = (UINT32)gpFwBiosGuardCapsuleMailbox;
    
    return EFI_SUCCESS;
}

/**    
    This function check gBiosGuardFwCapsuleBuffer is AMI BIOS GUARD ROM format or not.
    
    @param  pSessionBlock - 

    @retval  TRUE  - gBiosGuardFwCapsuleBuffer is AMI BIOS GUARD ROM format.
    @retval  FALSE - gBiosGuardFwCapsuleBuffer is not AMI BIOS GUARD ROM format.
**/
BOOLEAN IsAmiBiosGuardRom(VOID)
{
    UINT8   AmiBiosGuardSign[] = "_AMIPFAT";
    
    if ( CompareMem ( (UINT8 *)(gBiosGuardFwCapsuleBuffer + 8), AmiBiosGuardSign, 8 ) ) {
        return FALSE;
    }
    return TRUE;
}

/**    
    This function set BiosGuard flash update mode(capsule or recovery).
    
    @param  pSessionBlock - 

    @retval  EFI_SUCCESS - set BiosGuard flash update method success
    @retval  EFI_DEVICE_ERROR - set BiosGuard flash update method fail
**/
EFI_STATUS
EFIAPI
SetBiosGuardFlUpdMethod(
    IN OUT FUNC_FLASH_SESSION_BLOCK    *pSessionBlock
)
{
    EFI_STATUS          Status;
#if BIOS_GUARD_DEBUG_MODE    
    DEBUG((DEBUG_INFO, "SetBiosGuardFlUpdMethod enter\n"));
#endif    
    
    Status = EFI_DEVICE_ERROR;
    
    if(EFI_ERROR(AmiValidateMemoryBuffer(pSessionBlock, sizeof(FUNC_FLASH_SESSION_BLOCK))))
        return Status;

#if BIOS_GUARD_DEBUG_MODE    
    DEBUG((DEBUG_INFO, "pSessionBlock->FlUpdBlock.FlashOpType = %x\n", pSessionBlock->FlUpdBlock.FlashOpType));
#endif    
    
    switch(pSessionBlock->FlUpdBlock.FlashOpType)
    {
        case FlRuntime:
            Status = VerifyBiosGuardImage();
            if (!EFI_ERROR(Status)) {
                Status = BiosGuardVerifyFwRevision();
            }
            gBiosGuardFwCapsuleSize = 0;
            DEBUG((DEBUG_INFO, "FlRuntime VerifyBiosGuardImage (%r)\n", Status));
            break;
            
        case FlCapsule:
            if(gBiosGuardFwCapsuleBuffer == 0 || gBiosGuardFwCapsuleSize == 0)
                break;
            if (IsAmiBiosGuardRom()) {
                Status = VerifyBiosGuardImage();
            } else {
                Status = VerifiyOemBinImage((UINT8 *)gBiosGuardFwCapsuleBuffer, gBiosGuardFwCapsuleSize);
            }
            DEBUG((DEBUG_INFO, "FlCapsule VerifyBiosGuardImage (%r)\n", Status));

            if (EFI_ERROR(Status)) {
                gBiosGuardFwCapsuleSize = 0;
            } else {
                Status = UpdateCapsule(pSessionBlock);
#if BIOS_GUARD_DEBUG_MODE            
                DEBUG((DEBUG_INFO, "UpdateCapsule (%r)\n", Status));
#endif              
                if (!EFI_ERROR(Status)) {
                    Status = SetFlashUpdateVar(pSessionBlock);
                }
            }
              
            break;
        case FlRecovery:
            Status = SetBiosGuardFlashUpdateVar (pSessionBlock);
#if BIOS_GUARD_DEBUG_MODE            
            DEBUG((DEBUG_INFO, "SetBiosGuardFlashUpdateVar (%r)\n", Status));
#endif            
            break;
        default:
            Status = EFI_DEVICE_ERROR;
    }

    
    // Set Error Status
    if (Status != EFI_SUCCESS) { 
        //pSessionBlock->FSHandle  = 0;
        //pSessionBlock->ErrorCode = 1;
        return EFI_DEVICE_ERROR;
    }
    
    return EFI_SUCCESS;
}

/**    
  This function set related variable for BiosGuard capsule/recovery
  and change S5 to S3.

  @param[in] Handle    Handle of the callback
  @param[in] Context   The dispatch context

  @retval EFI_SUCCESS  
**/
EFI_STATUS
EFIAPI
BiosGuardRecoveryS5SmiHandler (
    IN EFI_HANDLE                DispatchHandle,
    IN CONST VOID                *DispatchContext OPTIONAL,
    IN OUT VOID                  *CommBuffer OPTIONAL,
    IN OUT UINTN                 *CommBufferSize OPTIONAL
)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    CHAR16                          SbWarmResetVar[] = SB_WARM_RESET_VARIABLE;
    UINT32                          SbWarmResetFlag = SB_WARM_RESET_TAG;
    UINTN                           BiosGuardRecoveryAddressVarSize = sizeof(UINT32);
    AMI_BIOS_GUARD_RECOVERY         *ApRecoveryAddress = NULL;
    EFI_CAPSULE_BLOCK_DESCRIPTOR    *BiosGuardCapsuleBlockDesc;
    EFI_CAPSULE_HEADER              *BiosGuardCapsuleHeader;
    UINTN                           BiosGuardCapsuleAddressVarSize = sizeof(AMI_BIOS_GUARD_RECOVERY);
    EFI_RUNTIME_SERVICES 	        *SmmRuntimeTable = NULL;
    UINT32                          i = 0;
    EFI_GUID                        SmmRsTableGuid = EFI_SMM_RUNTIME_SERVICES_TABLE_GUID;
//    AMI_FLASH_UPDATE_BLOCK          FlashUpdDesc;
//    UINTN                           FlashUpdDescSize = sizeof(AMI_FLASH_UPDATE_BLOCK);
//    EFI_GUID                        FlashUpdGuid  = FLASH_UPDATE_GUID;
    
    DEBUG ((DEBUG_INFO, "BiosGuardRecoveryS5SmiHandler Start\n"));

    Status = mSmmVariable->SmmGetVariable( L"BiosGuardRecoveryAddressVariable",
                                           &BiosGuardRecoveryAddressGuid,
                                           NULL,
                                           &BiosGuardRecoveryAddressVarSize,
                                           &ApRecoveryAddress);
    if ( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return EFI_SUCCESS;
    }
    
    DEBUG ((DEBUG_INFO, "ApRecoveryAddress->ReoveryAddress : %x\n", ApRecoveryAddress->ReoveryAddress));
    if ( (ApRecoveryAddress->ReoveryAddress == 0) || (ApRecoveryAddress->ReoveryAddress == 0xFFFFFFF) ) return EFI_SUCCESS;
    
    BiosGuardCapsuleBlockDesc = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)ApRecoveryAddress->ReoveryAddress;
    DEBUG ((DEBUG_INFO, "Length[0] : %x\n", BiosGuardCapsuleBlockDesc->Length));
    DEBUG ((DEBUG_INFO, "Union[0].DataBlock : %x\n", BiosGuardCapsuleBlockDesc->Union.DataBlock));
    DEBUG ((DEBUG_INFO, "Union[0].ContinuationPointer : %x\n", BiosGuardCapsuleBlockDesc->Union.ContinuationPointer));
    
    BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)BiosGuardCapsuleBlockDesc->Union.DataBlock);
    
    DEBUG ((DEBUG_INFO, "BiosGuardCapsuleHeader->CapsuleGuid : %g\n", &BiosGuardCapsuleHeader->CapsuleGuid));
    DEBUG ((DEBUG_INFO, "BiosGuardCapsuleHeader->HeaderSize : %X\n", BiosGuardCapsuleHeader->HeaderSize));
    DEBUG ((DEBUG_INFO, "BiosGuardCapsuleHeader->Flags : %X\n", BiosGuardCapsuleHeader->Flags));
    DEBUG ((DEBUG_INFO, "BiosGuardCapsuleHeader->CapsuleImageSize : %X\n", BiosGuardCapsuleHeader->CapsuleImageSize));   
    
    if ( CompareGuid (&BiosGuardRecoveryGuid, &BiosGuardCapsuleHeader->CapsuleGuid) ||
         CompareGuid (&BiosGuardCapsuleGuid, &BiosGuardCapsuleHeader->CapsuleGuid) ) {
        DEBUG ((DEBUG_INFO, "Start to do BIOS Guard Capsule\n"));
        
        Status = mSmmVariable->SmmSetVariable ( L"BiosGuardCapsuleVariable",
                                                &BiosGuardCapsuleVariableGuid,
                                                (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                                                BiosGuardCapsuleAddressVarSize,
                                                ApRecoveryAddress);
        if ( EFI_ERROR(Status) ) {
            ASSERT_EFI_ERROR (Status);
            return EFI_SUCCESS;
        }
        for (i = 0; i < gSmst->NumberOfTableEntries; ++i) {
            if (CompareGuid(&gSmst->SmmConfigurationTable[i].VendorGuid,&SmmRsTableGuid)) {
                break;
            }
        }
        if (i != gSmst->NumberOfTableEntries) {
            SmmRuntimeTable = gSmst->SmmConfigurationTable[i].VendorTable;
            SmmRuntimeTable->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
        }
    }
    
    DEBUG ((DEBUG_INFO, "BiosGuardRecoveryS5SmiHandler End\n"));
    return EFI_SUCCESS;
}

/** 
    This is the entry point of the BiosGuardSecSMIFlash driver.

    @param ImageHandle
    @param SystemTable

    @retval EFI_STATUS

**/
EFI_STATUS EFIAPI BiosGuardSecSMIFlashDriverEntryPoint (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS                      Status;
    EFI_HANDLE                      Handle, SxDispatchHandle;
    VOID                            *Memory = NULL;
    EFI_SMM_SX_DISPATCH2_PROTOCOL   *SxDispatch;
    EFI_SMM_SX_REGISTER_CONTEXT     SxContext = {SxS5, SxEntry};
    BIOSGUARD_HOB                   *BiosGuardHobPtr;

    
    DEBUG((DEBUG_INFO, "BiosGuardSecSMIFlashDriverEntryPoint\n"));
    
    if ( !((AsmReadMsr64 (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        DEBUG ((DEBUG_INFO, "BIOS Guard is disabled or not supported \n"));
        return EFI_SUCCESS;
    }
    
    //
    // Get stored BiosGuard public key hash
    //
    BiosGuardHobPtr = GetFirstGuidHob (&gBiosGuardHobGuid);
    if (BiosGuardHobPtr == NULL) {
        DEBUG ((DEBUG_ERROR, "Error! Can not get BiosGuard HOB!!!\n"));
        return EFI_NOT_FOUND;
    }
    
    Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, SHA256_DIGEST_SIZE, &BiosGuardPubKeyHash256Val);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    
    CopyMem( BiosGuardPubKeyHash256Val, &BiosGuardHobPtr->Bgpdt.PkeySlot0, SHA256_DIGEST_SIZE );
    
    //
    // Reserve memory for BiosGuard Capsule/Recovery
    //
    gBiosguardFwCapMaxSize = BIOSGUARD_RECOVERY_IMAGE_SIZE;    
#if defined (BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)     
    if (BIOSGUARD_RECOVERY_IMAGE_SIZE < BIOSGUARD_MEUD_RECOVERY_IMAGE_SIZE) {
        gBiosguardFwCapMaxSize = BIOSGUARD_MEUD_RECOVERY_IMAGE_SIZE;
    } 
#endif
    DEBUG((DEBUG_INFO, "gBiosguardFwCapMaxSize = %x\n", gBiosguardFwCapMaxSize));
    
    gBiosGuardFwCapsuleBuffer = 0xFFFFFFFF;
    Status = gBS->AllocatePages(AllocateMaxAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES(gBiosguardFwCapMaxSize), &gBiosGuardFwCapsuleBuffer);
    DEBUG((DEBUG_INFO, "Allocate BiosGuardCapsuleBuffer : %r\n",Status));
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    DEBUG((DEBUG_INFO, "Allocate BiosGuardCapsuleBuffer %lX,0x%X\n",gBiosGuardFwCapsuleBuffer, gBiosguardFwCapMaxSize));
    
    SetMem((void*)gBiosGuardFwCapsuleBuffer, gBiosguardFwCapMaxSize, 0 );

#if defined (BIOS_GUARD_REVISION_CHECK_SUPPORT) && (BIOS_GUARD_REVISION_CHECK_SUPPORT == 1)
    gBiosGuardReOrgImageBuffer = 0xFFFFFFFF;
    Status = gBS->AllocatePages(AllocateMaxAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES(FULL_BIOS_IMAGE_SIZE), &gBiosGuardReOrgImageBuffer);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
#endif    
    
    //
    // Install BiosGuard Secure SMI Flash Protocol 
    //
    Handle = NULL;
    Status = gSmst->SmmInstallProtocolInterface(
                 &Handle,
                 &gAmiBiosGuardSecSmiFlashProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &BiosGuardSecSmiFlash
             );
    ASSERT_EFI_ERROR(Status);
    
    //
    // Reserve pool in smm runtime memory for capsule's mailbox list
    //
    gpFwBiosGuardCapsuleMailboxSize = 4 * sizeof(EFI_CAPSULE_BLOCK_DESCRIPTOR) + sizeof(EFI_CAPSULE_HEADER); // (4*16)+28
    Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, gpFwBiosGuardCapsuleMailboxSize, &Memory);
    DEBUG((DEBUG_INFO, "Allocate gpFwBiosGuardCapsuleMailbox : %r\n",Status));    
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    gpFwBiosGuardCapsuleMailbox = (EFI_PHYSICAL_ADDRESS)(UINTN)Memory;    
    
    DEBUG((DEBUG_INFO, "Allocate gpFwBiosGuardCapsuleMailbox %lX,0x%X\n", gpFwBiosGuardCapsuleMailbox, gpFwBiosGuardCapsuleMailboxSize));
    SetMem((void*)gpFwBiosGuardCapsuleMailbox, gpFwBiosGuardCapsuleMailboxSize, 0 );
    
    //
    // Reserve pool for BiosGuardCapsuleHeader
    //
    //Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, sizeof(EFI_CAPSULE_HEADER), &gBiosGuardCapsuleHdr);
    Status = gBS->AllocatePool(EfiRuntimeServicesData, sizeof(EFI_CAPSULE_HEADER), &gBiosGuardCapsuleHdr);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    SetMem((void*)gBiosGuardCapsuleHdr, sizeof(EFI_CAPSULE_HEADER), 0 );
    
    //
    // Locate SmmVariableProtocol
    //
    Status = gSmst->SmmLocateProtocol(
                 &gEfiSmmVariableProtocolGuid,
                 NULL,
                 (VOID**)&mSmmVariable
             );
    if ( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return Status;
    }
    
    //
    // Register S5 call back function
    //
    Status = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid, NULL, &SxDispatch);
    ASSERT_EFI_ERROR (Status);
    if (!EFI_ERROR(Status)) {
        Status  = SxDispatch->Register (SxDispatch, \
                                        BiosGuardRecoveryS5SmiHandler, \
                                        &SxContext, \
                                        &SxDispatchHandle);
        ASSERT_EFI_ERROR (Status);
    }
    
    return Status;
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
