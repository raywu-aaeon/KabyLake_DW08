//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  This file contains FV before publishing hook

**/
#include <Token.h>
#include <AmiCspLib.h>
#include <AmiRomLayout.h>
#include "FitPeiProcessFvBeforePublishing.h"

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
EFI_GUID ReserveBootGuardFvBbAfterMemHashKeyGuid = RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY_FFS_FILE_RAW_GUID;
#endif

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
EFI_GUID ReserveBootGuardFvMainHashKeyGuid = RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY_FFS_FILE_RAW_GUID;
#endif

//**********************************************************************
/**
    Loads binary from RAW section of X firwmare volume


    @retval Buffer returns a pointer to allocated memory. Caller
        must free it when done.
    @retval Size returns the size of the binary loaded into the
        buffer.

**/
//**********************************************************************
EFI_STATUS
LocateBootGuardHashKey (
    IN EFI_PEI_SERVICES  **PpSv,
    IN EFI_GUID          *Guid,
    IN OUT VOID          **Buffer
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
//**********************************************************************
/**
    BootGuardVerificationForPeiToPeiAfterMemory
    handler.

    @retval PeiServices Pointer to PEI Services Table.
    @retval NotifyDesc Pointer to the descriptor for the Notification
        event that caused this function to execute.
    @retval Ppi Pointer to the PPI data associated with
        this function.

**/
//**********************************************************************
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1) 
BOOLEAN
BootGuardVerificationForPeiToPeiAfterMemory (
        IN      EFI_PEI_SERVICES          **PeiServices
)
{
    EFI_STATUS                             Status;
    UINTN                                  BootGuardHashDataSize = 0;
    UINTN                                  i;
    UINT8                                  CurrentBootGuardFvBbAfterMemHash256Val[32];    
    VOID                                   *BootGuardSha256Context;
    UINT8                                  *BootGuardOrgFvBbAfterMemHash256;
    RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY    *ReserveBootGuardFvBbAfterMemHashKey;

    BootGuardHashDataSize = Sha256GetContextSize ();
    Status = ((*PeiServices)->AllocatePool) (PeiServices, BootGuardHashDataSize, &BootGuardSha256Context);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: AllocatePool is fail for BootGuardSha256Context\n"));
        return   Status;
    }

    BootGuardOrgFvBbAfterMemHash256 = AllocateZeroPool (sizeof(RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY));
    if (BootGuardOrgFvBbAfterMemHash256 == NULL) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: AllocateZeroPool is fail for RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY\n"));
        return Status;
    }

    Status  = LocateBootGuardHashKey(PeiServices ,&ReserveBootGuardFvBbAfterMemHashKeyGuid, &BootGuardOrgFvBbAfterMemHash256);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: LocateBootGuardHashKey is fail\n"));
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

    ReserveBootGuardFvBbAfterMemHashKey = (RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY*)BootGuardOrgFvBbAfterMemHash256;

    for ( i = 0; i < sizeof(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey0); i++ ) {
        DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemHashKey0[%x]= %x.\n", i, ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey0[i]));
    }
    DEBUG ((DEBUG_INFO, "\n[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemSegmentBase0= %x.\n", ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentBase0));
    DEBUG ((DEBUG_INFO, "\n[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemSegmentSize0= %x.\n", ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentSize0));

    Sha256Init (BootGuardSha256Context);
    Sha256Update (BootGuardSha256Context, (UINT8 *)(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentBase0), (UINTN)ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentSize0);
    Sha256Final (BootGuardSha256Context,  CurrentBootGuardFvBbAfterMemHash256Val);

    for ( i = 0; i < sizeof (CurrentBootGuardFvBbAfterMemHash256Val); i++ ) {
        DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: CurrentBootGuardFvBbAfterMemHash256Val[%x]= %x.\n", i, CurrentBootGuardFvBbAfterMemHash256Val[i]));
    }

    if ( CompareMem(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey0, CurrentBootGuardFvBbAfterMemHash256Val, 32) ) {
        DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] FvAfterMemory BootGuardFvBbAfterMemHashKey0 Verify Fail.\n"));
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP) && (FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP == 1) 
        for ( i = 0; i < sizeof(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey1); i++ ) {
            DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemHashKey1[%x]= %x.\n", i, ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey1[i]));
        }
        DEBUG ((DEBUG_INFO, "\n[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemSegmentBase1= %x.\n", ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentBase1));
        DEBUG ((DEBUG_INFO, "\n[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemSegmentSize1= %x.\n", ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentSize1));

        Sha256Init (BootGuardSha256Context);
        Sha256Update (BootGuardSha256Context, (UINT8 *)(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentBase1), (UINTN)ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentSize1);
        Sha256Final (BootGuardSha256Context,  CurrentBootGuardFvBbAfterMemHash256Val);

        for ( i = 0; i < sizeof (CurrentBootGuardFvBbAfterMemHash256Val); i++ ) {
            DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] VerificationForPeiToPeiAfterMemory :: [1]CurrentBootGuardFvBbAfterMemHash256Val[%x]= %x.\n", i, CurrentBootGuardFvBbAfterMemHash256Val[i]));
        }

        if ( CompareMem(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey1, CurrentBootGuardFvBbAfterMemHash256Val, 32) ) {
            DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] FvAfterMemory BootGuardFvBbAfterMemHashKey1 Verify Fail.\n"));
            (*PeiServices)->ReportStatusCode (
                  PeiServices,
                  EFI_ERROR_CODE,                                          // Type
                  INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FV_AFTER_MEMORY_FAILED_POSTCODE,  // Value
                  0,
                  NULL,
                  NULL
            );
            PostCode (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FV_AFTER_MEMORY_FAILED_POSTCODE);
            DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] BootGuardFvBbAfterMemHashKey1 CpuDeadLoop.\n"));
            CpuDeadLoop ();
        } else {
            DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] BootGuardFvBbAfterMemHash Verify Pass.\n"));
			return   TRUE;
        }
#endif    
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
    
    return   TRUE;
}
#endif
//**********************************************************************
/**
    BootGuardVerificationForPeiToDxeHandoffEndOfPei at end of Pei
    handler.

    @retval PeiServices Pointer to PEI Services Table.
    @retval NotifyDesc Pointer to the descriptor for the Notification
        event that caused this function to execute.
    @retval Ppi Pointer to the PPI data associated with
        this function.

**/
//**********************************************************************
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
BOOLEAN
BootGuardVerificationForPeiToDxeHandoffEndOfPei (
        IN      EFI_PEI_SERVICES          **PeiServices
)
{
    EFI_STATUS                             Status;
    UINTN                                  BootGuardHashDataSize = 0, i;
    UINT8                                  CurrentBootGuardFvMainHash256Val[32];
    VOID                                   *BootGuardSha256Context;
    UINT8                                  *BootGuardOrgFvMainHash256;
    EFI_BOOT_MODE                          BootMode;
    RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY    *ReserveBootGuardFvMainHashKey;
            
    Status = PeiServicesGetBootMode (&BootMode);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[FitPeiProcessFvBeforePublishing.c] Get Boot Mode is fail\n"));
        return   Status;
    }

    if ( BootMode == BOOT_ON_S3_RESUME ) {
        DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] In the BOOT_ON_S3_RESUME\n"));
        return   Status;
    }
   
    BootGuardHashDataSize = Sha256GetContextSize ();
    Status = ((*PeiServices)->AllocatePool) (PeiServices, BootGuardHashDataSize, &BootGuardSha256Context);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[FitPeiProcessFvBeforePublishing.c] AllocatePool is fail for BootGuardSha256Context\n"));
        return   FALSE;
    }

    BootGuardOrgFvMainHash256 = AllocateZeroPool (sizeof(RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY));
    if (BootGuardOrgFvMainHash256 == NULL) {
        DEBUG ((DEBUG_ERROR, "[FitPeiProcessFvBeforePublishing.c] AllocateZeroPool is fail for RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY\n"));
        return FALSE;
    }

    Status  = LocateBootGuardHashKey(PeiServices ,&ReserveBootGuardFvMainHashKeyGuid, &BootGuardOrgFvMainHash256);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[FitPeiProcessFvBeforePublishing.c] LocateBootGuardHashKey is fail\n"));
        return   FALSE; 
    }    
    ReserveBootGuardFvMainHashKey = (RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY*)BootGuardOrgFvMainHash256;

    for ( i = 0; i < sizeof(ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey); i++ ) {
        DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] BootGuardFvMainHashKey[%x]= %x.\n", i, ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey[i]));
    }
    Sha256Init (BootGuardSha256Context);
    for( i = 0; i < NUMBER_FVMAIN_SEGMAMENT; i++) {
        DEBUG ((DEBUG_INFO, "\n[BootGuardPei.c] BootGuardFvMainSegmentBase[%x]= %x.\n", i, ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentBase[i]));
        DEBUG ((DEBUG_INFO, "\n[BootGuardPei.c] BootGuardFvMainSegmentSize[%x]= %x.\n", i, ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentSize[i]));
        if ((ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentSize[i] != 0) && (ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentBase[i] != 0xFFFFFFFF)) {
            Sha256Update (BootGuardSha256Context, (UINT8 *)(ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentBase[i]), (UINTN)ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentSize[i]);
        }
    }
    Sha256Final (BootGuardSha256Context,  CurrentBootGuardFvMainHash256Val);

    for ( i = 0; i < sizeof (CurrentBootGuardFvMainHash256Val); i++ ) {
            DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] CurrentBootGuardFvMainHash256Val[%x]= %x.\n", i, CurrentBootGuardFvMainHash256Val[i]));
    }

    if ( CompareMem(ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey, CurrentBootGuardFvMainHash256Val, 32) ) {
        DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] FvMainHashKey Verify Fail.\n"));
        return   FALSE;
    }
    return   TRUE;
}
#endif

/**
  This function 

  @param PeiServices Pointer to the Pointer to the Pei Services Table
  @param Area        Pointer to the RomArea being checked
  @param FvType      The attributes of the RomArea that is being checked

  @retval TRUE       Volume should be published

**/
BOOLEAN
FitPeiProcessFvBeforePublishing (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT AMI_ROM_AREA  *Area,
    IN     UINT32        FvType
)
{

    EFI_STATUS                             Status;
    EFI_BOOT_MODE                          BootMode;
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
    EFI_GUID                               AmiBootGuardHobGuid = AMI_BOOT_GUARD_HOB_GUID;
    AMI_BOOT_GUARD_HOB                     *AmiBootGuardHobPtr;
#endif
    
    if ( IsBootGuardSupported() == FALSE ) {
        return TRUE;
    }

    if ( (UINT32)AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) == 0 ) {
        DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] Boot Guard is disabled by Boot Guard Profile Configuration in the Intel Fitc\n"));
        return TRUE;
    }
        
    Status = PeiServicesGetBootMode (&BootMode);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[FitPeiProcessFvBeforePublishing.c] Get Boot Mode is fail\n"));
        return TRUE;
    }
    
    ///
    /// Verify Boot Code when first call
    ///
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
    if(GetFirstGuidHob (&AmiBootGuardHobGuid) == NULL){    
        Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof (AMI_BOOT_GUARD_HOB), (VOID **) &AmiBootGuardHobPtr);
        if ( EFI_ERROR( Status ) ) {
            return   Status;
        }
        AmiBootGuardHobPtr->EfiHobGuidType.Name = AmiBootGuardHobGuid;
        AmiBootGuardHobPtr->AmiBootGuardVerificationFlag = 0; 
    } else {
        AmiBootGuardHobPtr = GetFirstGuidHob (&AmiBootGuardHobGuid);
    }
#endif
/*
// Move to BootGuardPeiMemDetect.c
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
    ///
    /// Verify FV_BB_AFTER_MEMORY
    /// 
    if(AmiBootGuardHobPtr->AmiBootGuardVerificationFlag == 0x11) return TRUE;
    if(!BootGuardVerificationForPeiToPeiAfterMemory(PeiServices)) return  FALSE;
    DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] Verify FV_AFTER_BB pass.\n"));            
    AmiBootGuardHobPtr->AmiBootGuardVerificationFlag = (AmiBootGuardHobPtr->AmiBootGuardVerificationFlag) | (0x10);
#endif //FV_BB_VERIFY_FV_AFTER_MEMORY
*/  
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
    ///
    /// Verify FV_MAIN
    ///    
    if(AmiBootGuardHobPtr->AmiBootGuardVerificationFlag == 0x01) return TRUE;
    if(!BootGuardVerificationForPeiToDxeHandoffEndOfPei(PeiServices)) return FALSE;
    DEBUG ((DEBUG_INFO, "[FitPeiProcessFvBeforePublishing.c] Verify FV_MAIN pass.\n"));
    AmiBootGuardHobPtr->AmiBootGuardVerificationFlag = (AmiBootGuardHobPtr->AmiBootGuardVerificationFlag) | (0x01);    
#endif //INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT
    
    return TRUE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

