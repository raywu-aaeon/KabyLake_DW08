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
/** @file BootGuardPei.c
    Chain of trust for Pei

**/
//*************************************************************************

#include "BootGuardPei.h"
#include <CpuRegs.h>

EFI_GUID ReserveBootGuardFvMainHashKeyGuid = RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY_FFS_FILE_RAW_GUID;
/* Move to BootGuardPeiMemDetect.c
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)

EFI_GUID ReserveBootGuardFvBbAfterMemHashKeyGuid = RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY_FFS_FILE_RAW_GUID;

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  BootGuardVerificationForPeiToPeiAfterMemoryNotifyDesc = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmiPeiEndOfMemDetectGuid,
    BootGuardVerificationForPeiToPeiAfterMemory
};
#endif
*/

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
STATIC EFI_PEI_NOTIFY_DESCRIPTOR  BootGuardVerificationForPeiToDxeHandoffEndOfPeiNotifyDesc = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEndOfSiInitPpiGuid,
    BootGuardVerificationForPeiToDxeHandoffEndOfPei
};
#endif

static EFI_PEI_PPI_DESCRIPTOR HandleTpmS3Failure[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gHandleTpmS3ResumeFailure, \
      NULL }
};

static EFI_PEI_NOTIFY_DESCRIPTOR  mBootModePpi[] =
{
    { (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
      &gEfiPeiMasterBootModePpiGuid,//&gMasterBootModeGuid,
      BootGuardHandleTpmFailure
    },
};

CONST EFI_PEI_SERVICES  **ppPS;
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
    BootGuardVerificationForPeiToDxeHandoffEndOfPei at end of Pei
    handler.

    @retval PeiServices Pointer to PEI Services Table.
    @retval NotifyDesc Pointer to the descriptor for the Notification
        event that caused this function to execute.
    @retval Ppi Pointer to the PPI data associated with
        this function.

**/
/* Move to BootGuardPeiMemDetect.c
//**********************************************************************
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1) 
STATIC
EFI_STATUS
EFIAPI
BootGuardVerificationForPeiToPeiAfterMemory (
    IN      EFI_PEI_SERVICES          **PeiServices,
    IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN      VOID                      *Ppi
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
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: LocateBootGuardHashKey is fail\n"));
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
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemHashKey0[%x]= %x.\n", i, ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey0[i]));
    }
    DEBUG ((DEBUG_INFO, "\n[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemSegmentBase0= %x.\n", ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentBase0));
    DEBUG ((DEBUG_INFO, "\n[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemSegmentSize0= %x.\n", ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentSize0));

    Sha256Init (BootGuardSha256Context);
    Sha256Update (BootGuardSha256Context, (UINT8 *)(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentBase0), (UINTN)ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentSize0);
    Sha256Final (BootGuardSha256Context,  CurrentBootGuardFvBbAfterMemHash256Val);

    for ( i = 0; i < sizeof (CurrentBootGuardFvBbAfterMemHash256Val); i++ ) {
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: [0]CurrentBootGuardFvBbAfterMemHash256Val[%x]= %x.\n", i, CurrentBootGuardFvBbAfterMemHash256Val[i]));
    }

    if ( CompareMem(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey0, CurrentBootGuardFvBbAfterMemHash256Val, 32) ) {
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] FvAfterMemory BootGuardFvBbAfterMemHashKey0 Verify Fail.\n"));
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP) && (FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP == 1) 
        for ( i = 0; i < sizeof(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey1); i++ ) {
            DEBUG ((DEBUG_INFO, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemHashKey1[%x]= %x.\n", i, ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey1[i]));
        }
        DEBUG ((DEBUG_INFO, "\n[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemSegmentBase1= %x.\n", ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentBase1));
        DEBUG ((DEBUG_INFO, "\n[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: BootGuardFvBbAfterMemSegmentSize1= %x.\n", ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentSize1));

        Sha256Init (BootGuardSha256Context);
        Sha256Update (BootGuardSha256Context, (UINT8 *)(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentBase1), (UINTN)ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemSegmentSize1);
        Sha256Final (BootGuardSha256Context,  CurrentBootGuardFvBbAfterMemHash256Val);

        for ( i = 0; i < sizeof (CurrentBootGuardFvBbAfterMemHash256Val); i++ ) {
            DEBUG ((DEBUG_INFO, "[BootGuardPei.c] VerificationForPeiToPeiAfterMemory :: [1]CurrentBootGuardFvBbAfterMemHash256Val[%x]= %x.\n", i, CurrentBootGuardFvBbAfterMemHash256Val[i]));
        }

        if ( CompareMem(ReserveBootGuardFvBbAfterMemHashKey->BootGuardFvBbAfterMemHashKey1, CurrentBootGuardFvBbAfterMemHash256Val, 32) ) {
            DEBUG ((DEBUG_INFO, "[BootGuardPei.c] FvAfterMemory BootGuardFvBbAfterMemHashKey1 Verify Fail.\n"));
            (*PeiServices)->ReportStatusCode (
                  PeiServices,
                  EFI_ERROR_CODE,                                          // Type
                  INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FV_AFTER_MEMORY_FAILED_POSTCODE,  // Value
                  0,
                  NULL,
                  NULL
            );
            PostCode (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FV_AFTER_MEMORY_FAILED_POSTCODE);
            DEBUG ((DEBUG_INFO, "[BootGuardPei.c] BootGuardFvBbAfterMemHashKey1 CpuDeadLoop.\n"));
            CpuDeadLoop ();
        } else {
            DEBUG ((DEBUG_INFO, "[BootGuardPei.c] BootGuardFvBbAfterMemHash Verify Pass.\n"));
            return   Status;
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
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] BootGuardFvBbAfterMemHashKey0 CpuDeadLoop.\n"));
        CpuDeadLoop ();

	}
	
    return   Status;
}
#endif
*/
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
STATIC
EFI_STATUS
EFIAPI
BootGuardVerificationForPeiToDxeHandoffEndOfPei (
    IN      EFI_PEI_SERVICES          **PeiServices,
    IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN      VOID                      *Ppi
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
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] Get Boot Mode is fail\n"));
        return   Status;
    }

    if ( BootMode == BOOT_ON_S3_RESUME ) {
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] In the BOOT_ON_S3_RESUME\n"));
        return   Status;
    }
   
    BootGuardHashDataSize = Sha256GetContextSize ();
    Status = ((*PeiServices)->AllocatePool) (PeiServices, BootGuardHashDataSize, &BootGuardSha256Context);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] AllocatePool is fail for BootGuardSha256Context\n"));
        return   Status;
    }

    BootGuardOrgFvMainHash256 = AllocateZeroPool (sizeof(RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY));
    if (BootGuardOrgFvMainHash256 == NULL) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] AllocateZeroPool is fail for RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY\n"));
        return Status;
    }

    Status  = LocateBootGuardHashKey(PeiServices ,&ReserveBootGuardFvMainHashKeyGuid, &BootGuardOrgFvMainHash256);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] LocateBootGuardHashKey is fail\n"));
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION == 0)
        PeiServicesSetBootMode(BOOT_IN_RECOVERY_MODE);
        PeiServicesInstallPpi(&RecoveryModePpi);
        if(!PcdGetBool (AmiPcdForceShutdownOnFailedRecovery)){
            PcdSetBoolS(AmiPcdForceShutdownOnFailedRecovery,TRUE);            
        }
#endif
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION == 1)
        (*PeiServices)->ReportStatusCode (
                  PeiServices,
                  EFI_ERROR_CODE,                                          // Type
                  INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_POSTCODE,  // Value
                  0,
                  NULL,
                  NULL
        );
        PostCode (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_POSTCODE);
        CpuDeadLoop ();
#endif          
        return   Status;
    }

    ReserveBootGuardFvMainHashKey = (RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY*)BootGuardOrgFvMainHash256;

    for ( i = 0; i < sizeof(ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey); i++ ) {
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] BootGuardFvMainHashKey[%x]= %x.\n", i, ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey[i]));
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
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] CurrentBootGuardFvMainHash256Val[%x]= %x.\n", i, CurrentBootGuardFvMainHash256Val[i]));
    }

    if ( CompareMem(ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey, CurrentBootGuardFvMainHash256Val, 32) ) {
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] FvMainHashKey Verify Fail.\n"));
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION == 0)
        PeiServicesSetBootMode(BOOT_IN_RECOVERY_MODE);
        PeiServicesInstallPpi(&RecoveryModePpi);
        if(!PcdGetBool (AmiPcdForceShutdownOnFailedRecovery)){
            PcdSetBoolS(AmiPcdForceShutdownOnFailedRecovery,TRUE);            
        }
#endif
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION == 1)
        (*PeiServices)->ReportStatusCode (
                  PeiServices,
                  EFI_ERROR_CODE,                                          // Type
                  INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_POSTCODE,  // Value
                  0,
                  NULL,
                  NULL
        );
        PostCode (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_POSTCODE);
        CpuDeadLoop ();
#endif  
        return   EFI_SUCCESS;
    }
           
    return   Status;
}
#endif  

#define AMI_SKIP_TPM_STARTUP_GUID \
    { 0x6ee1b483, 0xa9b8, 0x4eaf, 0x9a, 0xe1, 0x3b, 0x28, 0xc5, 0xcf, 0xf3,\
      0x6b}

EFI_GUID            gSkipTpmStartupGuid = AMI_SKIP_TPM_STARTUP_GUID;

static EFI_PEI_PPI_DESCRIPTOR SkipTPMStartDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gSkipTpmStartupGuid, \
      NULL }
};

EFI_STATUS ACM_PostSuccess(
)
{
    UINT32  MsrValue;

    MsrValue = (UINT32) AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);
    if ( ((MsrValue & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == B_BOOT_GUARD_SACM_INFO_NEM_ENABLED)  &&
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) == B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT)  &&
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) == B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) ) {
        DEBUG((DEBUG_INFO, "[BootGuardPei.c] : ACM_PostSuccess : EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    }

    DEBUG((DEBUG_INFO,"[BootGuardPei.c] : ACM_PostSuccess : EFI_UNSUPPORTED\n"));
    return EFI_UNSUPPORTED;
}

//**********************************************************************
/**
    BootGuardHandleTpmFailure, notify function for MasterBootModePpi

    @retval PeiServices Pointer to PEI Services Table.
    @retval NotifyDesc Pointer to the descriptor for the Notification
        event that caused this function to execute.
    @retval Ppi Pointer to the PPI data associated with
        this function.

**/
//**********************************************************************
STATIC
EFI_STATUS
EFIAPI
BootGuardHandleTpmFailure (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
    IN  VOID                        *Ppi
)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    EFI_BOOT_MODE       BootMode;
    
    Status = PeiServicesGetBootMode (&BootMode);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] Get Boot Mode is fail\n"));
        return Status;
    }
    
    if ( BootMode == BOOT_ON_S3_RESUME ) {
        // Security Solution for BootGuard handle TPM failure.
        CONST UINTN MMIO_ACM_STATUS = 0xFED30328;
        UINT32      AcmStatus = MmioRead32 (MMIO_ACM_STATUS);
        UINT8       ErrorCode = 0;
        
        ErrorCode = (AcmStatus >> 4) & 0x3F;
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c][%d] TPM ERROR Class [0x%x]\n", __LINE__, ErrorCode));
        
        if (    ErrorCode == 0x0B ||        // ANC_TPM_FIFO_STARTUP_LOCALITY_ERROR_CLASS
                ErrorCode == 0x13 ||        // ANC_DTPM20_HIERARCHY_ERROR_CLASS
                ErrorCode == 0x14 ||        // ANC_TPM_CRB_HIERARCHY_ERROR_CLASS
                ErrorCode == 0x15 ||        // ANC_TPM_CRB_STARTUUP_ERROR_CLASS
                ErrorCode == 0x0C           // ANC_TPM_STARTUP_CMD_ERROR_CLASS 
            )
        {
            Status = (*PeiServices)->InstallPpi(PeiServices, &HandleTpmS3Failure[0]);
            
            if (EFI_ERROR(Status))
            {
                DEBUG ((DEBUG_ERROR, "[BootGuardPei.c][%d] Failed to install HandleTpmS3Failure PPI\n", __LINE__));
                return Status;
            }
        }
    }
    return Status;
}

EFI_STATUS
EFIAPI
BootGuardPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    ppPS = PeiServices; 
    
    if ( IsBootGuardSupported() == FALSE ) {
        return   Status;
    }

    if ( (UINT32)AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) == 0 ) {
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c] Boot Guard is disabled by Boot Guard Profile Configuration in the Intel Fitc\n"));
        return   Status;
    }
    
#if BOOT_GUARD_TPM_CHAIN_OF_TRUST   
    Status = ACM_PostSuccess();
    if( !EFI_ERROR(Status) )
    {
        // Install the Skip TpmStartup
        DEBUG ((DEBUG_INFO, "[BootGuardPei.c][%d] Install Skip TPM Startup\n", __LINE__));
        Status = (*PeiServices)->InstallPpi(PeiServices, &SkipTPMStartDesc[0]);
        ASSERT_EFI_ERROR (Status);
        if( EFI_ERROR(Status) )
        {
            DEBUG ((DEBUG_ERROR, "[BootGuardPei.c][%d] Failed to install Skip TPM Startup\n", __LINE__));
        }
    }
    //Install the notify function for MasterBootModePpi to check boot mode when boot mode has been updated.
    Status = (*PeiServices)->NotifyPpi( PeiServices, mBootModePpi);
    if( EFI_ERROR(Status) )
    {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c][%d] Failed to install boot mode notify ppi\n", __LINE__));
    }
#endif
/* Move to BootGuardPeiMemDetect.c
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1) 
    Status = PeiServicesNotifyPpi (&BootGuardVerificationForPeiToPeiAfterMemoryNotifyDesc);
    if ( EFI_ERROR( Status ) ) {
        return   Status;
    }    
#endif    
*/
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
    Status = PeiServicesNotifyPpi (&BootGuardVerificationForPeiToDxeHandoffEndOfPeiNotifyDesc);
    if ( EFI_ERROR( Status ) ) {
        return   Status;
    }
#endif

    return   Status;
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
