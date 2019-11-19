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

/** @file BiosGuardCpuPolicyOverride.c
    Code listing file for BiosGuardCpuPolicyOverride

**/

#include "BiosGuardCpuPolicyOverride.h"

EFI_GUID BiosGuardPubKeyGuid = BIOS_GUARD_PUB_KEY_FFS_FILE_RAW_GUID;

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mBiosGuardCpuPolicyOverrideNotifyDesc = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gSiPolicyPpiGuid,
    BiosGuardCpuPolicyOverride
};

//
// Private GUID for BIOS Guard initializes
//
extern EFI_GUID gBiosGuardModuleGuid;

/**
    Loads binary from RAW section of X firwmare volume

    @param PpSv 
    @param Buffer Returns a pointer to allocated memory. Caller
        must free it when done.

    @retval EFI_STATUS

**/
EFI_STATUS
LocateBiosGuardPubKey (
    IN CONST EFI_PEI_SERVICES       **PpSv,
    IN OUT VOID                     **Buffer
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
        if ( EFI_ERROR( Status )) {
            return Status;
        }

        ppFile = NULL;

        while ( TRUE ) {
            Status = (*PpSv)->FfsFindNextFile( PpSv,
                                               EFI_FV_FILETYPE_FREEFORM,
                                               pFV,
                                               &ppFile );

            if ( Status == EFI_NOT_FOUND ) break;            

            if ( CompareGuid( &ppFile->Name, &BiosGuardPubKeyGuid ) ) {             
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
  Get the BIOS Guard Module pointer.

  @param[in, out] ModulePtr - Input is a NULL pointer,
                              and output points BIOS Guard module address if found.

  @retval EFI_SUCCESS       - BIOS Guard Module found.
  @retval EFI_NOT_FOUND     - BIOS Guard Module not found.
**/
EFI_STATUS
UpdateBiosGuardModulePtr (
  IN OUT EFI_PHYSICAL_ADDRESS   *ModulePtr
  )
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME_HEADER    *FvHeader;
  EFI_FFS_FILE_HEADER           *FfsFile;
  EFI_GUID                      *BiosGuardModuleGuidPtr;
  UINT32                        ModuleAddr;

  BiosGuardModuleGuidPtr      = &gBiosGuardModuleGuid;
  FfsFile                     = NULL;
  Status                      = EFI_SUCCESS;

  ///
  /// Locate Firmware Volume header
  ///
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) FV_BB_BASE;  

  while (TRUE) {
    ///
    /// Locate Firmware File System file within Firmware Volume
    ///
    Status = PeiServicesFfsFindNextFile (EFI_FV_FILETYPE_RAW, FvHeader, (VOID **)&FfsFile);

    if (EFI_ERROR(Status)) {
      return EFI_NOT_FOUND;
    }

    ///
    /// Validate that the found Firmware File System file is the BIOS Guard Module
    ///
    if (CompareGuid (&(FfsFile->Name), BiosGuardModuleGuidPtr)) {
      ModuleAddr = (UINT32)((UINT8 *) FfsFile + sizeof (EFI_FFS_FILE_HEADER));
      DEBUG(( DEBUG_INFO, "BIOS Guard Module Location: %x\n", ModuleAddr));
      *ModulePtr = (EFI_PHYSICAL_ADDRESS) ModuleAddr;
      return Status;
    }
  }
}

/**
    BiosGuardCpuPolicyOverride at end of PeiCpuPlatformPolicy
    handler.

    @param PeiServices Pointer to PEI Services Table.
    @param NotifyDesc Pointer to the descriptor for the Notification
        event that caused this function to execute.
    @param Ppi Pointer to the PPI data associated with
        this function.

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
BiosGuardCpuPolicyOverride (
    IN CONST EFI_PEI_SERVICES       **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
    IN VOID                         *Ppi
)
{
    EFI_STATUS                     Status;
    SI_POLICY_PPI                  *SiPolicyPpi;    
    SI_PREMEM_POLICY_PPI           *SiPreMemPolicyPpi;    
    BIOS_GUARD_CONFIG              *CpuBiosGuardConfig;
    CPU_SECURITY_PREMEM_CONFIG     *CpuSecurityPreMemConfig;
    VOID                           *BiosGuardSha256Context;
    VOID                           *BiosGuardData;
    UINT8                          BiosGuardSfamCount,  AmiSfamOemCount =0, NumSpiComponents;
    UINT8                          BiosGuardHash256Val[32];
    UINT8                          *Sha256PubKeyDigest;
    UINT32                         TotalROMSize = 0, BIOS_GUARD_BIOS_START_ROM_ADDRESS = 0, PchSpiBase;
    UINTN                          BiosGuardHashDataSize = 0;
    SFAM_DATA                      AmiSfamOemData[] = { AMI_BIOS_GUARD_SFAM_OEM_1, 
                                                        AMI_BIOS_GUARD_SFAM_OEM_2, 
                                                        AMI_BIOS_GUARD_SFAM_OEM_3, 
                                                        AMI_BIOS_GUARD_SFAM_OEM_4 };
    EFI_PEI_HOB_POINTERS           HobList;
    BIOSGUARD_HOB                  *BiosGuardHobPtr;

    DEBUG ((DEBUG_INFO, "BiosGuardCpuPolicyOverride\n"));

    Status = PeiServicesLocatePpi (&gSiPolicyPpiGuid, 0, NULL, &SiPolicyPpi);
    ASSERT_EFI_ERROR (Status);
    
    Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gBiosGuardConfigGuid, (VOID *) &CpuBiosGuardConfig);
    ASSERT_EFI_ERROR (Status);  
    
    PeiServicesGetHobList (&HobList.Raw);
    BiosGuardHobPtr = NULL;
    HobList.Raw = GetNextGuidHob (&gBiosGuardHobGuid, HobList.Raw);
    if (HobList.Raw == NULL) {
      DEBUG ((DEBUG_ERROR, "Error! Can not get BiosGuard HOB!!!\n"));
      return EFI_NOT_FOUND;
    }
    
    BiosGuardHobPtr  = (BIOSGUARD_HOB *) ((UINT8 *) (&HobList.Guid->Header));
    
    BiosGuardHashDataSize = Sha256GetContextSize ();
    Status = ((*PeiServices)->AllocatePool) (PeiServices, BiosGuardHashDataSize, &BiosGuardSha256Context);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Status = LocateBiosGuardPubKey(PeiServices ,&Sha256PubKeyDigest);
    ASSERT_EFI_ERROR (Status);
    
    //
    // If it fails it might be in non BGUPC mode
    //
    if (!EFI_ERROR (Status)) {
        BiosGuardHobPtr->BgupHeader.PkgAttributes = 0;
        CopyMem (&BiosGuardHobPtr->Bgpdt.PkeySlot0, Sha256PubKeyDigest, 32);
    }

    ///
    /// Select to Flash Map 0 Register to get the number of flash Component
    ///
    PchSpiBase = MmioRead32 (MmPciBase (
                              DEFAULT_PCI_BUS_NUMBER_PCH,
                              PCI_DEVICE_NUMBER_PCH_SPI,
                              PCI_FUNCTION_NUMBER_PCH_SPI)
                              + R_PCH_SPI_BAR0) & ~B_PCH_SPI_BAR0_MASK;
    ASSERT (PchSpiBase != 0);

    MmioAndThenOr32 (
      PchSpiBase + R_PCH_SPI_FDOC,
      (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
      (UINT32) (V_PCH_SPI_FDOC_FDSS_FSDM | R_PCH_SPI_FDBAR_FLASH_MAP0)
    );
    /**
     Copy Zero based Number Of Components
     Valid Bit Settings:
       - 00 : 1 Component
       - 01 : 2 Components
       - All other settings : Reserved
    **/
     
    NumSpiComponents = (UINT8) ((MmioRead16 (PchSpiBase + R_PCH_SPI_FDOD) & B_PCH_SPI_FDBAR_NC) >> N_PCH_SPI_FDBAR_NC);

    ///
    /// Select to Flash Components Register to get Components Density
    ///
    MmioAndThenOr32 (
      PchSpiBase + R_PCH_SPI_FDOC,
      (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
      (UINT32) (V_PCH_SPI_FDOC_FDSS_COMP | R_PCH_SPI_FCBA_FLCOMP)
    );

    ///
    /// Calculate TotalFlashSize from Descriptor information
    ///
    BIOS_GUARD_BIOS_START_ROM_ADDRESS = (UINT8) MmioRead32 (PchSpiBase + R_PCH_SPI_FDOD);
    TotalROMSize = (SPI_SIZE_BASE_512KB << ((UINT8) (BIOS_GUARD_BIOS_START_ROM_ADDRESS & B_PCH_SPI_FLCOMP_COMP0_MASK)));
    if ( NumSpiComponents == 1 ) {
        TotalROMSize += (SPI_SIZE_BASE_512KB << ((UINT8) ((BIOS_GUARD_BIOS_START_ROM_ADDRESS & B_PCH_SPI_FLCOMP_COMP1_MASK) >> 4)));
    }
    BIOS_GUARD_BIOS_START_ROM_ADDRESS = TotalROMSize - FLASH_SIZE;

    //
    // We only protect the ME, FV_MAIN, FV_DATA and FV_BB and protect regions must to be signed.
    // SfamData[0] : BIOS Guard protects the ME Region from 0 to BIOS Starting address.
    //
    BiosGuardSfamCount = 0;
    BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = 0;
    BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = BIOS_GUARD_BIOS_START_ROM_ADDRESS - 1;

    //
    // SfamData[1] : BIOS Guard protects the FV_MAIN Region from FV_MAIN to FV_MAIN + FV_MainLength.
    //
    BiosGuardSfamCount++;
    BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = BIOS_GUARD_BIOS_START_ROM_ADDRESS + FV_MainStartingAddress;
    BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = BIOS_GUARD_BIOS_START_ROM_ADDRESS + FV_MainStartingAddress + FV_MainLength - 1;
   
#if defined INTEL_FIT_SUPPORT && INTEL_FIT_SUPPORT    
    //
    // SfamData[2] : BIOS Guard protects the FV_DATA Region from FV_DATA to FV_DATA + FV_DataLength.
    //
    BiosGuardSfamCount++;
    BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = BIOS_GUARD_BIOS_START_ROM_ADDRESS + FV_DataStartingAddress;
    BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = BIOS_GUARD_BIOS_START_ROM_ADDRESS + FV_DataStartingAddress + FV_DataLength - 1;
#endif
    
    //
    // SfamData[3] : BIOS Guard protects the FV_BB Region from FV_BB to FV_BB + FV_BBLength.
    //
    BiosGuardSfamCount++;
    BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = BIOS_GUARD_BIOS_START_ROM_ADDRESS + FV_BBStartingAddress;
    BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = BIOS_GUARD_BIOS_START_ROM_ADDRESS + FV_BBStartingAddress + FV_BBLength - 1;        
    
    for ( AmiSfamOemCount = 0; AmiSfamOemCount < 4; AmiSfamOemCount++  ) {
        if ( AmiSfamOemData[AmiSfamOemCount].LastByte != 0 ) {
            //
            // SfamData[4~7] : BIOS Guard protects the OEM Region from OEM to OEM + OEMLength.
            //
            BiosGuardSfamCount++;
            BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = BIOS_GUARD_BIOS_START_ROM_ADDRESS + AmiSfamOemData[AmiSfamOemCount].FirstByte;
            BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = BIOS_GUARD_BIOS_START_ROM_ADDRESS + AmiSfamOemData[AmiSfamOemCount].FirstByte + AmiSfamOemData[AmiSfamOemCount].LastByte -1;
        }
    }
    
    //
    // LastSfam : Number of SfamData
    //
    BiosGuardHobPtr->Bgpdt.LastSfam = BiosGuardSfamCount;
    if (BiosGuardHobPtr->Bgpdt.LastSfam > (MAX_SFAM_COUNT - 1)) {
        BiosGuardHobPtr->Bgpdt.LastSfam = MAX_SFAM_COUNT - 1;
    }

    for (BiosGuardSfamCount = 0; BiosGuardSfamCount <= BiosGuardHobPtr->Bgpdt.LastSfam; BiosGuardSfamCount++) {
        DEBUG ((DEBUG_INFO, "[BiosGuardCpuPolicyOverride.c] BiosGuardConfig->Bgpdt.SfamData[%d].FirstByte: %X \n", BiosGuardSfamCount, BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte));
        DEBUG ((DEBUG_INFO, "[BiosGuardCpuPolicyOverride.c] BiosGuardConfig->Bgpdt.SfamData[%d].LastByte: %X \n", BiosGuardSfamCount, BiosGuardHobPtr->Bgpdt.SfamData[BiosGuardSfamCount].LastByte));
    }
    
    //Status = UpdateBiosGuardModulePtr(&CpuBiosGuardConfig->BiosGuardModulePtr);
    //ASSERT_EFI_ERROR (Status);

    BiosGuardHobPtr->Bgpdt.BgpdtSize = (sizeof (BGPDT) - sizeof (BiosGuardHobPtr->Bgpdt.SfamData) + ((BiosGuardHobPtr->Bgpdt.LastSfam + 1) * sizeof (SFAM_DATA)));
    //
    // Defined values in the token
    //
    BiosGuardHobPtr->Bgpdt.BiosSvn = BIOS_SVN;
    BiosGuardHobPtr->BgupHeader.BiosSvn = BiosGuardHobPtr->Bgpdt.BiosSvn;
    
    Status = PeiServicesLocatePpi (
                  &gSiPreMemPolicyPpiGuid,
                  0,
                  NULL,
                  (VOID **) &SiPreMemPolicyPpi
                  );
    ASSERT_EFI_ERROR (Status);

    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
    ASSERT_EFI_ERROR (Status);
    
    // Now is KB alignment, Not MB
    //CpuSecurityPreMemConfig->TotalFlashSize = (UINT16) RShiftU64(TotalROMSize, KBShift);
    CpuSecurityPreMemConfig->BiosSize = (UINT16) RShiftU64(FLASH_SIZE, KBShift);    
    
    if (CpuSecurityPreMemConfig->FlashWearOutProtection) {
        BiosGuardHobPtr->Bgpdt.BiosGuardAttr |= EnumFlashwearoutProtection;
    } else {
        BiosGuardHobPtr->Bgpdt.BiosGuardAttr &= ~EnumFlashwearoutProtection;
    }    
    CpuBiosGuardConfig->BiosGuardAttr = BiosGuardHobPtr->Bgpdt.BiosGuardAttr;

    //
    // Hash the Bgpdt to BiosGuardConfig->BgpdtHash
    //
    BiosGuardHashDataSize = BiosGuardHobPtr->Bgpdt.BgpdtSize;
    BiosGuardData = (VOID *) &BiosGuardHobPtr->Bgpdt;
    Sha256Init (BiosGuardSha256Context);
    Sha256Update (BiosGuardSha256Context, BiosGuardData, BiosGuardHashDataSize);
    Sha256Final (BiosGuardSha256Context,  BiosGuardHash256Val);
    CopyMem (&CpuBiosGuardConfig->BgpdtHash[0], &BiosGuardHash256Val[0], 8);
    CopyMem (&CpuBiosGuardConfig->BgpdtHash[1], &BiosGuardHash256Val[8], 8);
    CopyMem (&CpuBiosGuardConfig->BgpdtHash[2], &BiosGuardHash256Val[16], 8);
    CopyMem (&CpuBiosGuardConfig->BgpdtHash[3], &BiosGuardHash256Val[24], 8);

    return Status;
}

EFI_STATUS 
EFIAPI
BiosGuardCpuPolicyOverrideEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS                       Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
    SETUP_DATA                       SetupData;
    UINTN                            VariableSize= sizeof (SETUP_DATA);
    EFI_GUID                         gAmiSetupGuid = SETUP_GUID;

    Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    Status = ReadOnlyVariable->GetVariable (
                                 ReadOnlyVariable,
                                 L"Setup",
                                 &gAmiSetupGuid,
                                 NULL,
                                 &VariableSize,
                                 &SetupData
                                 );

    if ( EFI_ERROR( Status ) ) {
        // After Bios Update, SetupData size is change.
        Status = PeiServicesNotifyPpi (&mBiosGuardCpuPolicyOverrideNotifyDesc);
        return Status;
    }

    if ( SetupData.BiosGuard == Enable ) {
        Status = PeiServicesNotifyPpi (&mBiosGuardCpuPolicyOverrideNotifyDesc);
        if ( EFI_ERROR( Status ) ) {
            return   Status;
        }
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
