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

/** @file BiosGuardVerifyRomImage.c
   

**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Setup.h>
#include "Token.h"
#include "AmiCertificate.h"
#include <FlashUpd.h>
#include <AmiHobs.h>
#include <Library/AmiBiosGuardVerifyRomPeiLib.h>
#include <AmiBiosGuard.h>
#include <Capsule.h>

//
// GUID definition
//
EFI_GUID FWCapsuleGuid                     = APTIO_FW_CAPSULE_GUID;
static EFI_GUID FwCapFfsGuid               = AMI_FW_CAPSULE_FFS_GUID;
static EFI_GUID FwCapSectionGuid           = AMI_FW_CAPSULE_SECTION_GUID;

#if defined(BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)
/**    
    Function to call SecureFlash to verify ME image.
    
    @param  PeiServices - Pointer to pointer to EFI_PEI_SERVICES structure.
    @param  MeImageAddress - Me Image address.
    @param  MeImageSize - ME image size.

    @retval  EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
VerifyMeImage(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PHYSICAL_ADDRESS       MeImageAddress,
  IN UINT32                     MeImageSize
)
{
    EFI_STATUS  Status;
    UINT32      FailedStage;
    
    Status = VerifyFwImage(PeiServices, (VOID**)&MeImageAddress, (UINT32*)&MeImageSize,(UINT32*)&FailedStage ); 
    
    return Status;
}

/**    
    Function to verify ME image.
    
    @param  PeiServices - Pointer to pointer to EFI_PEI_SERVICES structure

    @retval  EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
VerifyMe(
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
    EFI_STATUS              Status;    
    EFI_PEI_HOB_POINTERS    HobPointer;
    UINT8                   *RecoveryRomAddress;
    UINTN                   i;
    UINT8*                  MePtr;
    EFI_PHYSICAL_ADDRESS    TempAddress = 0;
    UINTN                   HeaderSize;
#if defined (BIOS_GUARD_ECFW_UPDATE_SUPPORT) && (BIOS_GUARD_ECFW_UPDATE_SUPPORT == 1)
    UINT8*                  EcPtr;
    EFI_PHYSICAL_ADDRESS    EcTempAddress = 0;
#endif
    
    HobPointer.Raw = GetHobList ();
    while ( (HobPointer.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, HobPointer.Raw)) != NULL ) {
        RecoveryRomAddress = (UINT8 *)HobPointer.Capsule->BaseAddress;
        
        DEBUG((DEBUG_INFO, "Capsule Length = %x\n", HobPointer.Capsule->Length));

#if defined (FWCAPSULE_FILE_FORMAT) && (FWCAPSULE_FILE_FORMAT == 1)
        if (HobPointer.Capsule->Length == (FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE)) {
            HeaderSize = ((APTIO_FW_CAPSULE_HEADER*)RecoveryRomAddress)->CapHdr.HeaderSize;
            DEBUG((DEBUG_INFO, "HeaderSize = %x\n", HeaderSize));
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize);
            Status = VerifyMeImage(PeiServices, (EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize, BIOSGUARD_ME_ALIGNMENT_SIZE);
            DEBUG ((DEBUG_INFO, "Verify ME Image Status(%r)\n", Status));
            if (EFI_ERROR(Status)) {            
                for (i = 0; i < 0x100; i++) {
                    *MePtr = 0xFF;
                    MePtr++;
                }
                return Status;
            }
            
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (FV_MEFW_CAPSULE_SIZE), &TempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }
            
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize + ((APTIO_FW_CAPSULE_HEADER *)(RecoveryRomAddress + FLASH_SIZE + HeaderSize))->RomImageOffset);
            CopyMem((UINT8*)TempAddress, MePtr, FV_MEFW_CAPSULE_SIZE);
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize);
            CopyMem(MePtr, (UINT8*)TempAddress, FV_MEFW_CAPSULE_SIZE);
        } else if (HobPointer.Capsule->Length == (FWCAPSULE_MAX_HDR_SIZE + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE)) {
            HeaderSize = ((APTIO_FW_CAPSULE_HEADER*)RecoveryRomAddress)->CapHdr.HeaderSize;
            DEBUG((DEBUG_INFO, "HeaderSize = %x\n", HeaderSize));
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize);
            Status = VerifyMeImage(PeiServices, (EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize, BIOSGUARD_ME_ALIGNMENT_SIZE);
            DEBUG ((DEBUG_INFO, "Verify ME Image Status(%r)\n", Status));
            if (EFI_ERROR(Status)) {            
                for (i = 0; i < 0x100; i++) {
                    *MePtr = 0xFF;
                    MePtr++;
                }
                return Status;
            }
            
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (FV_MEFW_CAPSULE_SIZE), &TempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }
            
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize + ((APTIO_FW_CAPSULE_HEADER *)(RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize))->RomImageOffset);
            CopyMem((UINT8*)TempAddress, MePtr, FV_MEFW_CAPSULE_SIZE);
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize);
            CopyMem(MePtr, (UINT8*)TempAddress, FV_MEFW_CAPSULE_SIZE);
        }
#if defined (BIOS_GUARD_ECFW_UPDATE_SUPPORT) && (BIOS_GUARD_ECFW_UPDATE_SUPPORT == 1)
        else if (HobPointer.Capsule->Length == (FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE)) {
            HeaderSize = ((APTIO_FW_CAPSULE_HEADER*)RecoveryRomAddress)->CapHdr.HeaderSize;
            DEBUG((DEBUG_INFO, "HeaderSize = %x\n", HeaderSize));
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize);
            Status = VerifyMeImage(PeiServices, (EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize, BIOSGUARD_ME_ALIGNMENT_SIZE);
            DEBUG ((DEBUG_INFO, "Verify ME Image Status(%r)\n", Status));
            if (EFI_ERROR(Status)) {            
                for (i = 0; i < 0x100; i++) {
                    *MePtr = 0xFF;
                    MePtr++;
                }
                return Status;
            }
            
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (FV_MEFW_CAPSULE_SIZE), &TempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }

            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (EC_FIRMWARE_BIN_SIZE), &EcTempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }

            
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize + ((APTIO_FW_CAPSULE_HEADER *)(RecoveryRomAddress + FLASH_SIZE + HeaderSize))->RomImageOffset);
            CopyMem((UINT8*)TempAddress, MePtr, FV_MEFW_CAPSULE_SIZE);
            EcPtr = (UINT8 *)(MePtr + FV_MEFW_CAPSULE_SIZE);
            CopyMem((UINT8*)EcTempAddress, EcPtr, EC_FIRMWARE_BIN_SIZE);
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize);
            EcPtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize + FV_MEFW_CAPSULE_SIZE);
            CopyMem(MePtr, (UINT8*)TempAddress, FV_MEFW_CAPSULE_SIZE);
            CopyMem(EcPtr, (UINT8*)TempAddress, EC_FIRMWARE_BIN_SIZE);         
        } else if (HobPointer.Capsule->Length == (FWCAPSULE_MAX_HDR_SIZE + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE)) {
            HeaderSize = ((APTIO_FW_CAPSULE_HEADER*)RecoveryRomAddress)->CapHdr.HeaderSize;
            DEBUG((DEBUG_INFO, "HeaderSize = %x\n", HeaderSize));
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize);
            Status = VerifyMeImage(PeiServices, (EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize, BIOSGUARD_ME_ALIGNMENT_SIZE);
            DEBUG ((DEBUG_INFO, "Verify ME Image Status(%r)\n", Status));
            if (EFI_ERROR(Status)) {            
                for (i = 0; i < 0x100; i++) {
                    *MePtr = 0xFF;
                    MePtr++;
                }
                return Status;
            }
            
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (FV_MEFW_CAPSULE_SIZE), &TempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }

            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (EC_FIRMWARE_BIN_SIZE), &EcTempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }

            
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize + ((APTIO_FW_CAPSULE_HEADER *)(RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize))->RomImageOffset);
            CopyMem((UINT8*)TempAddress, MePtr, FV_MEFW_CAPSULE_SIZE);
            EcPtr = (UINT8 *)(MePtr + FV_MEFW_CAPSULE_SIZE);
            CopyMem((UINT8*)EcTempAddress, EcPtr, EC_FIRMWARE_BIN_SIZE);
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize);
            EcPtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE + HeaderSize + FV_MEFW_CAPSULE_SIZE);
            CopyMem(MePtr, (UINT8*)TempAddress, FV_MEFW_CAPSULE_SIZE);
            CopyMem(EcPtr, (UINT8*)TempAddress, EC_FIRMWARE_BIN_SIZE);         
        }
#endif
#else
        if (HobPointer.Capsule->Length == (FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE)) {
            HeaderSize = ((APTIO_FW_CAPSULE_HEADER*)RecoveryRomAddress)->CapHdr.HeaderSize;
            DEBUG((DEBUG_INFO, "HeaderSize = %x\n", HeaderSize));
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize);
            Status = VerifyMeImage(PeiServices, (EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize, BIOSGUARD_ME_ALIGNMENT_SIZE);
            DEBUG ((DEBUG_INFO, "Verify ME Image Status(%r)\n", Status));
            if (EFI_ERROR(Status)) {            
                for (i = 0; i < 0x100; i++) {
                    *MePtr = 0xFF;
                    MePtr++;
                }
                return Status;
            }
            
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (FV_MEFW_CAPSULE_SIZE), &TempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }
            
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize + ((APTIO_FW_CAPSULE_HEADER *)(RecoveryRomAddress + FLASH_SIZE + HeaderSize))->RomImageOffset);
            CopyMem((UINT8*)TempAddress, MePtr, FV_MEFW_CAPSULE_SIZE);
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize);
            CopyMem(MePtr, (UINT8*)TempAddress, FV_MEFW_CAPSULE_SIZE);
        } 
#if defined (BIOS_GUARD_ECFW_UPDATE_SUPPORT) && (BIOS_GUARD_ECFW_UPDATE_SUPPORT == 1)
        else if (HobPointer.Capsule->Length == (FWCAPSULE_MAX_HDR_SIZE + FLASH_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE)) {
            HeaderSize = ((APTIO_FW_CAPSULE_HEADER*)RecoveryRomAddress)->CapHdr.HeaderSize;
            DEBUG((DEBUG_INFO, "HeaderSize = %x\n", HeaderSize));
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize);
            Status = VerifyMeImage(PeiServices, (EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize, BIOSGUARD_ME_ALIGNMENT_SIZE);
            DEBUG ((DEBUG_INFO, "Verify ME Image Status(%r)\n", Status));
            if (EFI_ERROR(Status)) {            
                for (i = 0; i < 0x100; i++) {
                    *MePtr = 0xFF;
                    MePtr++;
                }
                return Status;
            }
            
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (FV_MEFW_CAPSULE_SIZE), &TempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }

            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (EC_FIRMWARE_BIN_SIZE), &EcTempAddress);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }

            
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize + ((APTIO_FW_CAPSULE_HEADER *)(RecoveryRomAddress + FLASH_SIZE + HeaderSize))->RomImageOffset);
            CopyMem((UINT8*)TempAddress, MePtr, FV_MEFW_CAPSULE_SIZE);
            EcPtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize + BIOSGUARD_ME_ALIGNMENT_SIZE);
            CopyMem((UINT8*)EcTempAddress, EcPtr, EC_FIRMWARE_BIN_SIZE);
            MePtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize);
            EcPtr = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + FLASH_SIZE + HeaderSize + FV_MEFW_CAPSULE_SIZE);
            CopyMem(MePtr, (UINT8*)TempAddress, FV_MEFW_CAPSULE_SIZE);
            CopyMem(EcPtr, (UINT8*)EcTempAddress, EC_FIRMWARE_BIN_SIZE);
        }
#endif
#endif
        
        HobPointer.Raw = GET_NEXT_HOB (HobPointer);
    }  
    
    return EFI_SUCCESS;
}
#endif

/**    
    Function to read FW Cap Sig data from Ffs.
    
    @param  pCapsule - pointer to data buffer to read from.
    @param  pFfsData - pointer to output buffer.

    @retval  EFI_SUCCESS - EFI_SUCCESS if Capsule Hdr with Signature is retrieved.
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

/**    
    When verify capsule image signature fail, set CapsuleImageSize to 0xFFFFFFFF
    to let VerifyFwImage() to return EFI_SECURITY_VIOLATION
    
    @param  PeiServices Pointer to pointer to EFI_PEI_SERVICES structure
    
**/
VOID CancelFlashUpdate(
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
    EFI_PEI_HOB_POINTERS    HobPointer;
    EFI_CAPSULE_HEADER      *BiosGuardCapsuleHeader;
    EFI_PHYSICAL_ADDRESS    CapsuleAddress;
    APTIO_FW_CAPSULE_HEADER *FWCapsuleHdr;
    //EFI_GUID                EraseGuid = { 0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};
    
    HobPointer.Raw = GetHobList ();
    while ( (HobPointer.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, HobPointer.Raw)) != NULL ) {
        BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)HobPointer.Capsule->BaseAddress);
        //CopyMem(&BiosGuardCapsuleHeader->CapsuleGuid, &EraseGuid, sizeof(EFI_GUID));
        CapsuleAddress = HobPointer.Capsule->BaseAddress + (EFI_PHYSICAL_ADDRESS)BiosGuardCapsuleHeader->HeaderSize;
        
        if(CompareGuid((EFI_GUID*)&BiosGuardCapsuleHeader->CapsuleGuid, &FWCapsuleGuid)) {
            FWCapsuleHdr = (APTIO_FW_CAPSULE_HEADER *)BiosGuardCapsuleHeader;
        } else {
            if (EFI_ERROR(BiosGuardFindCapHdrFFS((VOID *)(CapsuleAddress), (UINT8**)&FWCapsuleHdr))) {
                HobPointer.Raw = GET_NEXT_HOB (HobPointer);
                continue;
            }
        }
        
        // Let VerifyFwImage() to return EFI_SECURITY_VIOLATION
        FWCapsuleHdr->CapHdr.CapsuleImageSize = 0xFFFFFFFF;        
        HobPointer.Raw = GET_NEXT_HOB (HobPointer);
    }
}

/**    
    This function verify BiosGuard capsule image signature.
    
    @param  PeiServices Pointer to pointer to EFI_PEI_SERVICES structure

    @retval  EFI_SUCCESS - BiosGuard capsule image signature verify pass.
**/
EFI_STATUS
EFIAPI
BiosGuardVerifyCapsule(
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
    EFI_STATUS              Status;    
    EFI_GUID                BiosGuardRecoveryImageHobGuid = AMI_BIOSGUARD_RECOVERY_IMAGE_HOB_GUID;
    RECOVERY_IMAGE_HOB      *RecoveryHob = NULL;
    EFI_HOB_GENERIC_HEADER  *Hobs = NULL;
    UINT32                  ImageSize = 0;
    
    Status = (*PeiServices)->GetHobList(PeiServices, (VOID**)&Hobs);
    if(EFI_ERROR(Status))
        return Status;

    RecoveryHob = GetNextGuidHob(&BiosGuardRecoveryImageHobGuid, (VOID*)Hobs);
    if(RecoveryHob == NULL) {
        return EFI_NOT_FOUND;
    }
    
    DEBUG((DEBUG_INFO, "RecoveryHob->ImageSize = %x\n", RecoveryHob->ImageSize));
    
    switch (RecoveryHob->ImageSize) {
        //EIP429536 >>
        case BIOSGUARD_MEUD_BIOS_SIZE:
            ImageSize = BIOSGUARD_MEUD_BIOS_SIZE;
            break;
        //EIP429536 <<
        case BIOSGUARD_RECOVERY_IMAGE_SIZE:
            ImageSize = BIOSGUARD_RECOVERY_IMAGE_SIZE;
            break;
        
        case (BIOSGUARD_MEUD_BIOS_SIZE + FV_MEFW_CAPSULE_SIZE):        
            ImageSize = BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE;
            break;
        
        case (BIOSGUARD_MEUD_BIOS_SIZE + FV_MEFW_CAPSULE_SIZE + EC_FIRMWARE_BIN_SIZE):
            ImageSize = BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE;
            break;
        
        case (BIOSGUARD_MEUD_BIOS_SIZE + EC_FIRMWARE_BIN_SIZE):
            ImageSize = BIOSGUARD_MEUD_BIOS_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE;
            break;
        
        default:    
            ImageSize = 0;
    }
    
    if (ImageSize == 0) {
        DEBUG((DEBUG_ERROR, "BiosGuardVerifyCapsule : ImageSize can not be zero.\n"));
        CancelFlashUpdate(PeiServices);
        return EFI_INVALID_PARAMETER;
    }
    
    Status = AmiBiosGuardVerifiyBiosImage((EFI_PEI_SERVICES **)PeiServices, RecoveryHob->Address, ImageSize);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "Verify BIOS image fail(%r)\n", Status));
        CancelFlashUpdate(PeiServices);
    }
    DEBUG((DEBUG_ERROR, "AmiBiosGuardVerifiyBiosImage(%r)\n", Status));
    return Status;
}

/** 
    This is the entry point of the BiosGuardVerifyRomImage driver.

    @param FileHandle  Handle of this PEIM
    @param PeiServices Pointer to pointer to EFI_PEI_SERVICES structure

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
BiosGuardVerifyRomImageEntry (
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
    
    Status = PeiServicesGetBootMode (&BootMode);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((DEBUG_ERROR, "[BootGuardPei.c] Get Boot Mode is fail\n"));
        return   Status;
    }
    
    if ( SetupData.BiosGuard == 1 && BootMode == BOOT_ON_FLASH_UPDATE) {
        BiosGuardVerifyCapsule(PeiServices);
#if defined(BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)          
        VerifyMe(PeiServices);
#endif        
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
