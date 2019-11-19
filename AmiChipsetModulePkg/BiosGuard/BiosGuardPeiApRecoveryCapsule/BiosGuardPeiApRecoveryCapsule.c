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

/** @file BiosGuardPeiApRecoveryCapsule.c
    This file is used for BIOS Guard flash tool executed "/recovery" and "/capsule".

**/

#include "BiosGuardPeiApRecoveryCapsule.h"

BOOLEAN
IsFlashUpdate (
    EFI_PEI_SERVICES    **PeiServices,
    EFI_BOOT_MODE       *BootMode
) {
    EFI_STATUS                            Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI       *ReadOnlyVariable;
    UINTN                                 BiosGuardCapsuleVarSize = sizeof(AMI_BIOS_GUARD_RECOVERY);
    EFI_GUID                              BiosGuardCapsuleVariableGuid = BIOS_GUARD_CAPSULE_VARIABLE_GUID;
    AMI_BIOS_GUARD_RECOVERY               ApRecoveryAddress;

    Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    Status = ReadOnlyVariable->GetVariable (
                 ReadOnlyVariable,
                 L"BiosGuardCapsuleVariable",
                 &BiosGuardCapsuleVariableGuid,
                 NULL,
                 &BiosGuardCapsuleVarSize,
                 &ApRecoveryAddress
             );

    if ( !EFI_ERROR( Status ) ) {
        if( ApRecoveryAddress.ReoveryMode == BiosGuardRecoveryMode ) {
            *BootMode = BOOT_IN_RECOVERY_MODE;
            DEBUG ((DEBUG_INFO, "AP executed the command of /recovery\n"));
            return TRUE;
        } else if( ApRecoveryAddress.ReoveryMode == BiosGuardCapsuleMode ) {
            *BootMode = BOOT_ON_FLASH_UPDATE;
            DEBUG ((DEBUG_INFO, "AP executed the command of /capsule\n"));
            return TRUE;
        } else {
            DEBUG ((DEBUG_INFO, "ApRecoveryAddress.ReoveryMode = %X\n", ApRecoveryAddress.ReoveryMode));
        }
    }

    return FALSE;
}

EFI_STATUS
EFIAPI
BiosGuardCapsuleChangeBootModeAfterEndofMrc (
    IN EFI_PEI_SERVICES              **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
    IN VOID                          *InvokePpi
)
{
    EFI_STATUS           Status;
    EFI_BOOT_MODE        BootMode;

    Status = PeiServicesGetBootMode (&BootMode);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] Get Boot Mode is fail\n"));
        return   Status;
    }

    DEBUG ((DEBUG_INFO, "BiosGuardRecovery : BootMode = %X\n", BootMode));

    if ( !EFI_ERROR(Status) && (BootMode == BOOT_ON_S3_RESUME) ) {
        //
        // Change the Boot Mode to BOOT_ON_FLASH_UPDATE from S3_RESUME
        //
        if ( IsFlashUpdate(PeiServices, &BootMode) ) {
            return Status;
        }

        if ( BootMode == BOOT_ON_FLASH_UPDATE ) {
            PeiServicesSetBootMode(BootMode);
            DEBUG ((DEBUG_INFO, "BiosGuardRecovery : Change boot mode to BOOT_ON_FLASH_UPDATE\n"));
        }
    }

    return Status;
}

EFI_STATUS
GetBiosGuardCapsuleInfo (
    IN EFI_CAPSULE_BLOCK_DESCRIPTOR   *Desc,
    IN OUT UINTN                      *NumDescriptors OPTIONAL,
    IN OUT UINTN                      *CapsuleSize OPTIONAL,
    IN OUT UINTN                      *CapsuleNumber OPTIONAL
)
{
    UINTN                          Count;
    UINTN                          Size;
    UINTN                          Number;
    UINTN                          ThisCapsuleImageSize;
    EFI_CAPSULE_HEADER             *BiosGuardCapsuleHeader;

    DEBUG ((DEBUG_INFO, "GetBiosGuardCapsuleInfo Enter\n"));

    ASSERT (Desc != NULL);

    Count = 0;
    Size  = 0;
    Number = 0;
    ThisCapsuleImageSize = 0;

    while (Desc->Union.ContinuationPointer != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
        if (Desc->Length == 0) {
            //
            // Descriptor points to another list of block descriptors somewhere
            //
            if ( Desc == (EFI_CAPSULE_BLOCK_DESCRIPTOR  *) (UINTN) Desc->Union.ContinuationPointer ||
                    Desc->Union.ContinuationPointer > MAX_ADDRESS ) {
                Count = 0;
                break;
            }

            Desc = (EFI_CAPSULE_BLOCK_DESCRIPTOR  *) (UINTN) Desc->Union.ContinuationPointer;
        } else {
            //
            // Sanity Check
            // It is needed, because ValidateCapsuleIntegrity() only validate one individual capsule Size.
            // While here we need check all capsules size.
            //
            if (Desc->Length >= (MAX_ADDRESS - Size)) {
                DEBUG ((DEBUG_ERROR, "ERROR: Desc->Length(0x%lx) >= (MAX_ADDRESS - Size(0x%x))\n", Desc->Length, Size));
                return EFI_OUT_OF_RESOURCES;
            }
            Size += (UINTN) Desc->Length;
            Count++;

            //
            // See if this is first capsule's header
            //
            if (ThisCapsuleImageSize == 0) {
                BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)Desc->Union.DataBlock);

                //
                // This has been checked in ValidateCapsuleIntegrity()
                //
                Number++;
                ThisCapsuleImageSize = BiosGuardCapsuleHeader->CapsuleImageSize;
            }

            //
            // This has been checked in ValidateCapsuleIntegrity()
            //
            ASSERT (ThisCapsuleImageSize >= Desc->Length);
            ThisCapsuleImageSize = (UINTN)(ThisCapsuleImageSize - Desc->Length);

            //
            // Move to next
            //
            Desc++;
        }
    }
    //
    // If no descriptors, then fail
    //
    if (Count == 0) {
        DEBUG ((DEBUG_ERROR, "ERROR: Count == 0\n"));
        return EFI_NOT_FOUND;
    }

    //
    // checked in ValidateCapsuleIntegrity()
    //
    ASSERT (ThisCapsuleImageSize == 0);

    if (NumDescriptors != NULL) {
        *NumDescriptors = Count;
    }

    if (CapsuleSize != NULL) {
        *CapsuleSize = Size;
    }

    if (CapsuleNumber != NULL) {
        *CapsuleNumber = Number;
    }

    return EFI_SUCCESS;
}

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

EFI_STATUS
EFIAPI
BiosGuardGetCapsuleImage (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
    IN VOID                               *InvokePpi
)
{
    EFI_STATUS                          Status;
    UINT32                              PchSpiBase;
    UINT32                              BiosGuardBiosStartRomAddress;
    EFI_GUID                            BiosGuardCapsuleVariableGuid = BIOS_GUARD_CAPSULE_VARIABLE_GUID;
    EFI_GUID                            gBiosGuardRecoveryImageHobGuid = AMI_BIOSGUARD_RECOVERY_IMAGE_HOB_GUID;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    UINTN                               BiosGuardCapsuleVarSize = sizeof(AMI_BIOS_GUARD_RECOVERY);
    AMI_BIOS_GUARD_RECOVERY             ApRecoveryAddress;
    EFI_PEI_HOB_POINTERS                HobPointer;
    EFI_CAPSULE_HEADER                  *BiosGuardCapsuleHeader;    
    EFI_CAPSULE_BLOCK_DESCRIPTOR        *BiosGuardCapsuleBlockDesc;
    EFI_PHYSICAL_ADDRESS                SavedBiosGuardImage = 0, SaveCapsuleImage = 0, CapsuleAddress = 0;
    UINT8                               AmiBiosGuardSign[] = "_AMIPFAT";
    UINT8                               *RecoveryRomAddress;    
    UINT32                              BiosImageSize;
    UINT32                              CurrentBGUPAddress = 0;
    UINTN                               CapsuleSize;
    BOOLEAN                             MeUpdate = FALSE;
    UINT8                               *MePayload;   
    BOOLEAN                             EcUpdate = FALSE;
    BOOLEAN                             BiosUpdate = FALSE;  //EIP429536
    UINT8                               *EcPayload;
    APTIO_FW_CAPSULE_HEADER             *FWCapsuleHdr;
    RECOVERY_IMAGE_HOB                  *BiosGuardRecoveryImageHob;
    EFI_PHYSICAL_ADDRESS                TempAddress = 0;
    BOOLEAN                             BgtCapsule = FALSE;
    BOOLEAN                             FoundBiosGuardUpdateImage = FALSE;
#if BIOS_GUARD_DEBUG_MODE
    UINTN                               i, j;
#endif    
    
    PchSpiBase = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (
                                     0,
                                     DEFAULT_PCI_BUS_NUMBER_PCH,
                                     PCI_DEVICE_NUMBER_PCH_SPI,
                                     PCI_FUNCTION_NUMBER_PCH_SPI,
                                     R_PCH_SPI_BAR0
                                     ));
    PchSpiBase &= ~(B_PCH_SPI_BAR0_MASK);
    ASSERT (PchSpiBase != 0);
    
    if (PchSpiBase != 0) {
        BiosGuardBiosStartRomAddress = (UINT32) LShiftU64 ((*(UINT16*)(UINTN) PchSpiBase), 12);
    }

    Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    Status = ReadOnlyVariable->GetVariable (
                 ReadOnlyVariable,
                 L"BiosGuardCapsuleVariable",
                 &BiosGuardCapsuleVariableGuid,
                 NULL,
                 &BiosGuardCapsuleVarSize,
                 &ApRecoveryAddress
             );

    if ( !EFI_ERROR( Status ) && (ApRecoveryAddress.Signature != 0) ) {
#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((DEBUG_INFO, "ApRecoveryAddress = %X \n", ApRecoveryAddress));
        DEBUG ((DEBUG_INFO, "ApRecoveryAddress.Signature = %lX\n", ApRecoveryAddress.Signature));
        DEBUG ((DEBUG_INFO, "ApRecoveryAddress.Version = %X\n", ApRecoveryAddress.Version));
        DEBUG ((DEBUG_INFO, "ApRecoveryAddress.ReoveryMode = %X\n", ApRecoveryAddress.ReoveryMode));
        DEBUG ((DEBUG_INFO, "ApRecoveryAddress.ReoveryAddress = %X\n", ApRecoveryAddress.ReoveryAddress));
#endif
        if ( (ApRecoveryAddress.ReoveryAddress == 0) || (ApRecoveryAddress.ReoveryAddress == 0xFFFFFFF) ) return EFI_SUCCESS;

        BiosGuardCapsuleBlockDesc = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)ApRecoveryAddress.ReoveryAddress;
        BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)BiosGuardCapsuleBlockDesc->Union.DataBlock);
#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((DEBUG_INFO, "BiosGuardCapsuleHeader->CapsuleGuid : %g \n", &BiosGuardCapsuleHeader->CapsuleGuid));
        DEBUG ((DEBUG_INFO, "BiosGuardCapsuleHeader->HeaderSize : %X \n", BiosGuardCapsuleHeader->HeaderSize));
        DEBUG ((DEBUG_INFO, "BiosGuardCapsuleHeader->Flags : %X \n", BiosGuardCapsuleHeader->Flags));
#endif

        if( ApRecoveryAddress.ReoveryMode == BiosGuardRecoveryMode ) {
            if ( !CompareGuid (&BiosGuardRecoveryGuid, &BiosGuardCapsuleHeader->CapsuleGuid) ) {
                DEBUG ((DEBUG_INFO, "Stop to do BIOS GUARD Recovery, BiosGuardRecoveryGuid is Not the same.\n"));
                return EFI_SUCCESS;
            }

            PeiServicesSetBootMode(BOOT_IN_RECOVERY_MODE);
            PeiServicesInstallPpi(&RecoveryModePpi);
            return EFI_SUCCESS;
        } else if( ApRecoveryAddress.ReoveryMode == BiosGuardCapsuleMode ) {
            BgtCapsule = TRUE;
        }
    }

    //
    // Capsule update for BIOS Guard
    //
    HobPointer.Raw = GetHobList ();
    while ( (HobPointer.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, HobPointer.Raw)) != NULL ) {
        BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)HobPointer.Capsule->BaseAddress);
        CapsuleAddress = (EFI_PHYSICAL_ADDRESS)BiosGuardCapsuleHeader;
        RecoveryRomAddress = (UINT8*)(CapsuleAddress + (EFI_PHYSICAL_ADDRESS)BiosGuardCapsuleHeader->HeaderSize);

        //
        // Checking the recovery ROM whether is AMI BIOS GUARD ROM or Not.
        //
        if ( CompareMem ( RecoveryRomAddress + 8, AmiBiosGuardSign, 8 ) ) {
            DEBUG ((DEBUG_INFO, "This is Not BIOS Guard ROM.\n"));
            HobPointer.Raw = GET_NEXT_HOB (HobPointer);
            continue;
        }
        FoundBiosGuardUpdateImage = TRUE;
        CapsuleSize = BiosGuardCapsuleHeader->CapsuleImageSize - BiosGuardCapsuleHeader->HeaderSize;
        DEBUG ((DEBUG_INFO, "CapsuleSize = %X\n", CapsuleSize));            
        if (CapsuleSize > INVALID_IMAGE_SIZE) continue;
        
        //
        // Allocate memory for saving BiosGuard image.
        //
        Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (CapsuleSize), &SavedBiosGuardImage);
        ASSERT_EFI_ERROR(Status);
        if ( EFI_ERROR( Status ) ) {
            ASSERT_EFI_ERROR (Status);
            return Status;
        }

        DEBUG ((DEBUG_INFO, "Saved BiosGuard capsule Image : %lx\n", SavedBiosGuardImage));
        CopyMem((UINT8*)SavedBiosGuardImage, RecoveryRomAddress, CapsuleSize);    
        
        // Move SaveCapsuleImage to CV Hob BaseAddress
        SaveCapsuleImage = HobPointer.Capsule->BaseAddress;
#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((DEBUG_INFO, "SaveCapsuleImage = %x\n\n", SaveCapsuleImage));
#endif            
        RecoveryRomAddress = (UINT8*)SavedBiosGuardImage;
        
        switch (CapsuleSize) {
            //EIP429536 >>
            case BIOSGUARD_MEUD_BIOS_SIZE:
                BiosImageSize = BIOSGUARD_MEUD_BIOS_SIZE;
                BiosUpdate = TRUE;
                break;
            //EIP429536 <<

            case BIOSGUARD_RECOVERY_IMAGE_SIZE:
                BiosImageSize = BIOSGUARD_RECOVERY_IMAGE_SIZE;
                break;
                
            case (BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE):
                MeUpdate = TRUE;
                MePayload = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + BIOSGUARD_MEUD_BIOS_SIZE);
                BiosImageSize = BIOSGUARD_MEUD_BIOS_SIZE;
                break;
                
            case (BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE):
                MeUpdate = TRUE;
                EcUpdate = TRUE;
                MePayload = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + BIOSGUARD_MEUD_BIOS_SIZE);
                EcPayload = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE);
                BiosImageSize = BIOSGUARD_MEUD_BIOS_SIZE;
                break;
                
            case (BIOSGUARD_MEUD_BIOS_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE):
                EcUpdate = TRUE;
                EcPayload = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + BIOSGUARD_MEUD_BIOS_SIZE);
                BiosImageSize = BIOSGUARD_MEUD_BIOS_SIZE;
                break;
                
            default:    
                DEBUG ((DEBUG_ERROR, "CapsuleSize not match, Stop doing BiosGuard Capsule.\n"));
                return EFI_UNSUPPORTED;
        }
        
        // Re-build BIOS image 
        AmiBiosGuardReOrgnizeRomImage((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress, SaveCapsuleImage, BiosImageSize, &CurrentBGUPAddress);
        
        // Get the FWCapsuleHdr
        if (FWCAPSULE_FILE_FORMAT) {
            FWCapsuleHdr = (APTIO_FW_CAPSULE_HEADER*)(RecoveryRomAddress + CapsuleSize - FWCAPSULE_IMAGE_ALLIGN - SIGNATURE_SIZE);            
        } else {                
            if (MeUpdate || EcUpdate || BiosUpdate) {  //EIP429536
                if ( EFI_ERROR(BiosGuardFindCapHdrFFS((VOID*)(SaveCapsuleImage), (UINT8**)&FWCapsuleHdr)) ) {
                    ASSERT_EFI_ERROR (EFI_SECURITY_VIOLATION);
                    return EFI_SECURITY_VIOLATION;
                }
            } else {
                if ( EFI_ERROR(BiosGuardFindCapHdrFFS((VOID*)(SaveCapsuleImage + BiosGuardBiosStartRomAddress), (UINT8**)&FWCapsuleHdr)) ) {
                    ASSERT_EFI_ERROR (EFI_SECURITY_VIOLATION);
                    return EFI_SECURITY_VIOLATION;
                }
            }
        }
        
        // Allocate temp pool for store BIOS+FWCapsuleHdr
        Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (FLASH_SIZE + FWCAPSULE_MAX_HDR_SIZE), &TempAddress);
        ASSERT_EFI_ERROR(Status);
        if ( EFI_ERROR( Status ) ) {
            ASSERT_EFI_ERROR (Status);
            return Status;
        }
        CopyMem ((UINT8*)TempAddress, FWCapsuleHdr, FWCAPSULE_MAX_HDR_SIZE);            
        if ((MeUpdate == FALSE) && (EcUpdate == FALSE)) {
            //EIP429536 >>
            if (BiosUpdate){
                CopyMem ((UINT8*)(TempAddress + FWCAPSULE_MAX_HDR_SIZE), (UINT8*)SaveCapsuleImage, FLASH_SIZE );
            }
            else
            //EIP429536 <<
                CopyMem ((UINT8*)(TempAddress + FWCAPSULE_MAX_HDR_SIZE), (UINT8*)SaveCapsuleImage + BiosGuardBiosStartRomAddress, FLASH_SIZE );
        } else {
            CopyMem ((UINT8*)(TempAddress + FWCAPSULE_MAX_HDR_SIZE), (UINT8*)SaveCapsuleImage, FLASH_SIZE );
        }
        CopyMem ((UINT8*)SaveCapsuleImage, (UINT8*)TempAddress, FLASH_SIZE + FWCAPSULE_MAX_HDR_SIZE);
        
        if (MeUpdate) {
            // Copy ME capsule image to the end of SaveCapsuleImage
            CopyMem ((UINT8*)SaveCapsuleImage + FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE, MePayload, BIOSGUARD_ME_ALIGNMENT_SIZE);
            HobPointer.Capsule->Length = FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE;
            if (EcUpdate) {
                // Copy EC Fw capsule image to the end of SaveCapsuleImage
                CopyMem ((UINT8*)SaveCapsuleImage + FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE, EcPayload, EC_FIRMWARE_BIN_ALIGNMENT_SIZE);
                HobPointer.Capsule->Length = FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE;
            }
        } else {
            // MeUpdate == FALSE
            if (EcUpdate) {
                // Copy EC Fw capsule image to the end of SaveCapsuleImage
                CopyMem ((UINT8*)SaveCapsuleImage + FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE, EcPayload, EC_FIRMWARE_BIN_ALIGNMENT_SIZE);
                HobPointer.Capsule->Length = FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE;
            }
        }             

#if BIOS_GUARD_DEBUG_MODE
        SaveCapsuleImage = HobPointer.Capsule->BaseAddress;

        DEBUG ((DEBUG_INFO, "APTIO_FW_CAPSULE_HEADER\n\n"));
        for ( i = 0; i < FWCAPSULE_MAX_HDR_SIZE/16; i++ ) {
            for ( j = 0; j < 16; j++ ) {
                DEBUG ((DEBUG_INFO, "%02X ", *(UINT8*)SaveCapsuleImage++));
            }
            DEBUG ((DEBUG_INFO, "\n"));
        }

        DEBUG ((DEBUG_INFO, "\nBIOS Data\n"));
        for ( i = 0; i < 16; i++ ) {
            for ( j = 0; j < 16; j++ ) {
                DEBUG ((DEBUG_INFO, "%02X ", *(UINT8*)SaveCapsuleImage++));
            }
            DEBUG ((DEBUG_INFO, "\n"));
        }
#endif
        //
        // Save BiosGuard image to HOB.
        //
        Status = PeiServicesCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, sizeof (RECOVERY_IMAGE_HOB), (VOID **) &BiosGuardRecoveryImageHob);
        if ( EFI_ERROR( Status ) ) {
            ASSERT_EFI_ERROR (Status);
            return Status;
        }

        //
        // Create BiosGuard recovery Image HOB for DXE used.
        //
        BiosGuardRecoveryImageHob->Header.Name = gBiosGuardRecoveryImageHobGuid;
        BiosGuardRecoveryImageHob->Address = SavedBiosGuardImage;
        
        if (MeUpdate) {
            // BIOS+ME
            BiosGuardRecoveryImageHob->ImageSize = BiosImageSize + FV_MEFW_CAPSULE_SIZE;
            if (EcUpdate) {
                // BIOS+ME+EC
                BiosGuardRecoveryImageHob->ImageSize = BiosImageSize + FV_MEFW_CAPSULE_SIZE + EC_FIRMWARE_BIN_SIZE;
            }
        } else {
            if (EcUpdate) {
                // BIOS+EC
                BiosGuardRecoveryImageHob->ImageSize = BiosImageSize + EC_FIRMWARE_BIN_SIZE;
            } else {
                // BIOS
                BiosGuardRecoveryImageHob->ImageSize = BiosImageSize;
            }            
        }
        
        if (BgtCapsule) {
            PeiServicesSetBootMode(BOOT_ON_FLASH_UPDATE);
            PeiServicesInstallPpi(&FlashUpdateModePpi);
        }
        
        break;
    }
    
    if ((FoundBiosGuardUpdateImage == FALSE) && (BgtCapsule)) {
        HobPointer.Raw = GetHobList ();
        while ( (HobPointer.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, HobPointer.Raw)) != NULL ) {
            BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)HobPointer.Capsule->BaseAddress);
            CapsuleAddress = (EFI_PHYSICAL_ADDRESS)BiosGuardCapsuleHeader;
            RecoveryRomAddress = (UINT8*)(CapsuleAddress + (EFI_PHYSICAL_ADDRESS)BiosGuardCapsuleHeader->HeaderSize);            
            CapsuleSize = BiosGuardCapsuleHeader->CapsuleImageSize - BiosGuardCapsuleHeader->HeaderSize;
            DEBUG ((DEBUG_INFO, "CapsuleSize = %X\n", CapsuleSize));
            
            SaveCapsuleImage = HobPointer.Capsule->BaseAddress;
            
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (CapsuleSize), &TempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }
            
            CopyMem((UINT8*)TempAddress, RecoveryRomAddress, CapsuleSize);
            CopyMem((UINT8*)SaveCapsuleImage, (UINT8*)TempAddress, CapsuleSize);
            HobPointer.Capsule->Length = CapsuleSize;
            break;
        }
        
        PeiServicesSetBootMode(BOOT_ON_FLASH_UPDATE);
        PeiServicesInstallPpi(&FlashUpdateModePpi);
    }

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BiosGuardSiPreMemPolicyOverride (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
    IN VOID                               *InvokePpi
)
{
    EFI_STATUS                          Status;
    SI_PREMEM_POLICY_PPI                *SiPreMemPolicyPpi = NULL;
    CPU_SECURITY_PREMEM_CONFIG          *CpuSecurityPreMemConfig = NULL;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    EFI_BOOT_MODE                       BootMode;
    SETUP_DATA                          SetupData;
    EFI_GUID                            gAmiSetupGuid = SETUP_GUID;
    UINT32                              WarmResetFlag = 0;
    UINTN                               VariableSize = sizeof(SETUP_DATA);
    EFI_GUID                            WarmResetGuid = {0xb8cafa84, 0x4593, 0x4aa9, 0xae, 0xf7, 0x8e, 0x68, 0x6e, 0xb0, 0x73, 0x20};
    EFI_GUID                            BiosGuardCapsuleVariableGuid = BIOS_GUARD_CAPSULE_VARIABLE_GUID;
    AMI_BIOS_GUARD_RECOVERY             ApRecoveryAddress;
    
    SiPreMemPolicyPpi = NULL;
    Status = PeiServicesLocatePpi (&gSiPreMemPolicyPpiGuid, 0, NULL, (VOID **)&SiPreMemPolicyPpi);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }  
    
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }  
    
    Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }    

    Status = ReadOnlyVariable->GetVariable ( ReadOnlyVariable, L"Setup", &gAmiSetupGuid, NULL, &VariableSize, &SetupData);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }
    
    if (SetupData.FlashWearOutProtection == 1) {
        (*PeiServices)->GetBootMode( PeiServices, &BootMode );
        
        if (BootMode == BOOT_ON_S3_RESUME) {            
            VariableSize = sizeof(WarmResetFlag);
            Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                                    L"SbWarmResetVar", \
                                                    &WarmResetGuid, \
                                                    NULL, \
                                                    &VariableSize, \
                                                    &WarmResetFlag );
            
            if ((!EFI_ERROR(Status)) && (WarmResetFlag == 'IsWR')) {
                DEBUG((DEBUG_INFO, "Disable FlashWearOutProtection for capsule update\n"));
                CpuSecurityPreMemConfig->FlashWearOutProtection = CPU_FEATURE_DISABLE;
            }          
        }
        
        VariableSize = sizeof(AMI_BIOS_GUARD_RECOVERY);
        Status = ReadOnlyVariable->GetVariable (
                     ReadOnlyVariable,
                     L"BiosGuardCapsuleVariable",
                     &BiosGuardCapsuleVariableGuid,
                     NULL,
                     &VariableSize,
                     &ApRecoveryAddress
                 );
        if (!EFI_ERROR(Status)) {
            // Check if in BGT Capsule/Recovery flow
            if( ApRecoveryAddress.ReoveryMode & 0x03 ) {
                DEBUG((DEBUG_INFO, "Disable FlashWearOutProtection for BGT capsule/recovery flow\n"));
                CpuSecurityPreMemConfig->FlashWearOutProtection = CPU_FEATURE_DISABLE;
            }
        }        
    }    
    
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BiosGuardMeFwBootOnFlashUpdateNotify(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *NullPpi
)
{
#if defined(BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)     
    EFI_PEI_HOB_POINTERS        HobPointer;
    EFI_CAPSULE_HEADER          *FwCapHdr = (EFI_CAPSULE_HEADER*)NULL;
    UINT8                       *p;
    UINT32                      i;
    EFI_GUID mMeFwCapsuleFvGuid = {0x9F8B1DEF, 0xB62B, 0x45F3, 0x82, 0x82, 0xBF, 0xD7, 0xEA, 0x19, 0x80, 0x1B};

    HobPointer.Raw = GetHobList ();
    while ( (HobPointer.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, HobPointer.Raw)) != NULL ) {    
        FwCapHdr = (EFI_CAPSULE_HEADER*)((UINTN)HobPointer.Capsule->BaseAddress);
        
        p = (UINT8*)FwCapHdr;
        if ((FwCapHdr->CapsuleImageSize - FwCapHdr->HeaderSize) == FLASH_SIZE) {
            // BIOSGUARD MEUD by AMIBGT
            for (i = FwCapHdr->CapsuleImageSize; i < (FwCapHdr->CapsuleImageSize + BIOSGUARD_ME_ALIGNMENT_SIZE); i+=16) {
                if( ((EFI_FIRMWARE_VOLUME_HEADER*)(p + \
                                i))->Signature != EFI_FVH_SIGNATURE ) continue;
                if( !CompareMem( (UINT8*)p + i + ((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->ExtHeaderOffset, &mMeFwCapsuleFvGuid, sizeof(EFI_GUID)) ) break;
                i += (UINT32)(((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->FvLength - 16);
            }
            if( i >= (FwCapHdr->CapsuleImageSize + BIOSGUARD_ME_ALIGNMENT_SIZE) ) {
                HobPointer.Raw = GET_NEXT_HOB (HobPointer);
                continue;
            } 
        } else if ((FwCapHdr->CapsuleImageSize - FwCapHdr->HeaderSize) == BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE) {
            // BIOSGUARD MEUD by Windows Capsule
            for (i = FwCapHdr->CapsuleImageSize - BIOSGUARD_ME_ALIGNMENT_SIZE; i < (FwCapHdr->CapsuleImageSize); i+=16) {
                if( ((EFI_FIRMWARE_VOLUME_HEADER*)(p + \
                                i))->Signature != EFI_FVH_SIGNATURE ) continue;
                if( !CompareMem( (UINT8*)p + i + ((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->ExtHeaderOffset, &mMeFwCapsuleFvGuid, sizeof(EFI_GUID)) ) break;
                i += (UINT32)(((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->FvLength - 16);
            }
            if( i >= (FwCapHdr->CapsuleImageSize) ) {
                HobPointer.Raw = GET_NEXT_HOB (HobPointer);
                continue;
            } 
        } 
#if defined (BIOS_GUARD_ECFW_UPDATE_SUPPORT) && (BIOS_GUARD_ECFW_UPDATE_SUPPORT == 1)        
        else if ((FwCapHdr->CapsuleImageSize - FwCapHdr->HeaderSize) == (BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE)) {
            
            for (i = FwCapHdr->CapsuleImageSize - BIOSGUARD_ME_ALIGNMENT_SIZE - EC_FIRMWARE_BIN_ALIGNMENT_SIZE; i < (FwCapHdr->CapsuleImageSize); i+=16) {
                if( ((EFI_FIRMWARE_VOLUME_HEADER*)(p + \
                                i))->Signature != EFI_FVH_SIGNATURE ) continue;
                if( !CompareMem( (UINT8*)p + i + ((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->ExtHeaderOffset, &mMeFwCapsuleFvGuid, sizeof(EFI_GUID)) ) break;
                i += (UINT32)(((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->FvLength - 16);
            }
            if( i >= (FwCapHdr->CapsuleImageSize) ) {
                HobPointer.Raw = GET_NEXT_HOB (HobPointer);
                continue;
            } 
        }
#endif
        else {
            HobPointer.Raw = GET_NEXT_HOB (HobPointer);
            continue;
        }
                                      
        DEBUG((DEBUG_INFO, "BIOSGUARD : HECI interface not work if S3 resume path, so generate a Soft Reset to re-activate HECI.\n"));
        // Clear Wake Status (WAK_STS) and Sleep Type (SLP_TYP)
        IoWrite16( PM_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_WAK );
        IoWrite16( PM_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, \
          (IoRead16( PM_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT ) & ~B_PCH_ACPI_PM1_CNT_SLP_TYP) );
        // Since, HECI interface not work if S3 resume path, to generate a Soft Reset
        // to re-activate HECI.
        IoWrite8( R_PCH_RST_CNT, V_PCH_RST_CNT_HARDSTARTSTATE );
        IoWrite8( R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET );
    }
#endif    
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BiosGuardPeiApRecoveryCapsuleEntry (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS                       Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
    SETUP_DATA                       SetupData;
    UINTN                            VariableSize= sizeof (SETUP_DATA);
    EFI_GUID                         gAmiSetupGuid = SETUP_GUID;
    EFI_BOOT_MODE                    BootMode;

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
        return Status;
    }

    if ( SetupData.BiosGuard == Enable ) {
        PeiServicesNotifyPpi(BiosGuardGetCapsuleImageNotifyList);
        PeiServicesNotifyPpi(BiosGuardCapsuleChangeBootModeAfterEndofMrcNotifyList);   
        PeiServicesNotifyPpi(BiosGuardSiPreMemPolicyOverrideNotifyList);
        
        if (BIOSGUARD_MEUD_WARMRESET) {
            Status = PeiServicesGetBootMode(&BootMode);
            if (!EFI_ERROR(Status) && (BootMode == BOOT_ON_S3_RESUME)) {
                PeiServicesNotifyPpi(BiosGuardMeFwBootOnFlashUpdateNotifyList);
            }
        }
    }

    return Status;
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
