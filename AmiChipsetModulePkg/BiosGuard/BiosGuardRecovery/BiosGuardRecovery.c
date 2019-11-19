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

#include <Pei.h>
#include <AmiPeiLib.h>
#include <Token.h>
#include <Ppi/Stall.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Hob.h>
#include <AmiHobs.h>
#include <BiosGuard.h>
#include <Library/DebugLib.h>
#include <AmiCspLib.h>
#include "CpuRegs.h"
#include <Library/AmiBiosGuardMiscLib.h>
#include <Library/MmPciLib.h>
#include <Library/IoLib.h>
#include <Library/AmiBiosGuardVerifyRomPeiLib.h>
#include <AmiBiosGuard.h>
#include <Library/PciSegmentLib.h>
#include <Register/PchRegsSpi.h>
#include <PchAccess.h>

#define BIOSGUARD_PACKAGE_CERT   524

//----------------------------------------------------------------------------
// Local prototypes
typedef struct
{
    EFI_PEI_DEVICE_RECOVERY_MODULE_PPI          *pDRM_Ppi;
    EFI_PEI_DEVICE_LOAD_RECOVERY_CAPSULE        SavedLoadCapsule;
} DEVICE_RECOVERY_MODULE_INFO;

//----------------------------------------------------------------------------
// Local Variables
DEVICE_RECOVERY_MODULE_INFO gDeviceRecoveryModuleInfo[MAX_DEVICE_RECOVERY_MODULE];

EFI_PHYSICAL_ADDRESS
FindAvailableMemory (
    IN EFI_PEI_SERVICES      **PeiServices
)
{
    VOID                    *p;
    UINT8                   i;
    EFI_PHYSICAL_ADDRESS    TopOfMemory = 0xffffffff;

    for ((*PeiServices)->GetHobList(PeiServices, &p), i = 0; \
            !(FindNextHobByType(EFI_HOB_TYPE_MEMORY_ALLOCATION, &p));	i++) {
        if ((TopOfMemory > ((EFI_HOB_MEMORY_ALLOCATION*)p)->AllocDescriptor.MemoryBaseAddress) && 
                (((EFI_HOB_MEMORY_ALLOCATION*)p)->AllocDescriptor.MemoryBaseAddress > BIOSGUARD_RECOVERY_IMAGE_SIZE))
            TopOfMemory = ((EFI_HOB_MEMORY_ALLOCATION*)p)->AllocDescriptor.MemoryBaseAddress;
    }

    return (TopOfMemory - (BIOSGUARD_RECOVERY_IMAGE_SIZE & 0xFFF00000));
}

VOID
ReportBiosGuardRecoverySize (
    EFI_PEI_SERVICES **PeiServices,
    UINT32 Size
)
{
    static EFI_GUID RecoveryHobGuid = AMI_RECOVERY_IMAGE_HOB_GUID;
    EFI_STATUS Status;
    RECOVERY_IMAGE_HOB *RecoveryHob;
    
    Status = (*PeiServices)->GetHobList(PeiServices, (VOID **)&RecoveryHob);
    if(!EFI_ERROR(Status)) {
        Status = FindNextHobByGuid(&RecoveryHobGuid, (VOID **)&RecoveryHob);
        if(EFI_ERROR(Status))
            return;
    }
    
    RecoveryHob->ImageSize = Size;
}

VOID
GetRecoveryImageSize (
    EFI_PEI_SERVICES **PeiServices,
    UINT32 *Size
)
{
    static EFI_GUID RecoveryHobGuid = AMI_RECOVERY_IMAGE_HOB_GUID;
    EFI_STATUS Status;
    RECOVERY_IMAGE_HOB *RecoveryHob;
    
    Status = (*PeiServices)->GetHobList(PeiServices, (VOID **)&RecoveryHob);
    if(!EFI_ERROR(Status)) {
        Status = FindNextHobByGuid(&RecoveryHobGuid, (VOID **)&RecoveryHob);
        if(EFI_ERROR(Status))
            return;
    }
    
    *Size = RecoveryHob->ImageSize;
}

EFI_STATUS
BiosGuardRecoveryFileLoaded (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN RECOVERY_IMAGE_HOB       *RecoveryHob
)
{
    UINT8                   AmiBiosGuardSign[] = "_AMIPFAT";
    UINT8                   *RecoveryRomAddress;
    UINT32                  CurrentBGUPAddress = 0;
    EFI_STATUS              Status;
    EFI_PHYSICAL_ADDRESS    Buffer = 0, SavedBiosGuardImage = 0;
    EFI_BOOT_MODE           BootMode;
    RECOVERY_IMAGE_HOB      *BiosGuardRecoveryImageHob;
    EFI_GUID                gBiosGuardRecoveryImageHobGuid = AMI_BIOSGUARD_RECOVERY_IMAGE_HOB_GUID;
    UINT32                  RecoveryImageSize;
    UINT8                   *FWCapsuleHdr;
    UINT32                  PchSpiBase;
    UINT32                  BiosGuardBiosStartRomAddress = 0;
    BOOLEAN                 MeUpdate = FALSE;
    BOOLEAN                 EcUpdate = FALSE;
    BOOLEAN                 BiosUpdate = FALSE;  //EIP429536
    
    DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : BiosGuardRecoveryFileLoaded Start\n"));
    
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

    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    if ( (EFI_ERROR(Status)) || !(BootMode == BOOT_ON_FLASH_UPDATE || BootMode == BOOT_IN_RECOVERY_MODE) ) return Status;

    //
    // To avoid out of memory resource with AllocatePages in PEI phase, find out the top of available memory for collecting ROM.
    //
    Buffer = FindAvailableMemory(PeiServices);
    DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : FindAvailableMemory =%x\n", Buffer));

    RecoveryRomAddress = (UINT8*)RecoveryHob->Address;
    DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : RecoveryRomAddress =%x\n", RecoveryRomAddress));
    RecoveryImageSize = RecoveryHob->ImageSize;
    DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : ImageSize =%x\n", RecoveryHob->ImageSize));

    //
    // Checking the recovery ROM whether is AMI BIOS GUARD ROM or Not.
    //
    if (MemCmp (RecoveryRomAddress + 8, AmiBiosGuardSign, 8)) {
        DEBUG ((DEBUG_ERROR, "BiosGuardRecovery.c : This is Not BIOS Guard ROM.\n"));
        ASSERT_EFI_ERROR (EFI_UNSUPPORTED);
        return EFI_SECURITY_VIOLATION;
    }
    
    // Verify bios image
    Status = AmiBiosGuardVerifiyBiosImage(PeiServices, (EFI_PHYSICAL_ADDRESS)RecoveryRomAddress, RecoveryImageSize);
    if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "BiosGuardRecovery.c : Verify BiosGuard image fail.\n"));
        return EFI_SECURITY_VIOLATION;
    }

    AmiBiosGuardReOrgnizeRomImage((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress, Buffer, RecoveryImageSize, &CurrentBGUPAddress);
    
    switch (RecoveryImageSize) {
        //EIP429536
        case BIOSGUARD_MEUD_BIOS_SIZE:
            BiosUpdate = TRUE;
            break;
        //EIP429536 <<

        case BIOSGUARD_RECOVERY_IMAGE_SIZE:
            break;
            
        case (BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE):
            MeUpdate = TRUE;
            break;
            
        case (BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE):
            MeUpdate = TRUE;
            EcUpdate = TRUE;
            break;
            
        case (BIOSGUARD_MEUD_BIOS_SIZE + EC_FIRMWARE_BIN_ALIGNMENT_SIZE):
            EcUpdate = TRUE;
            break;
            
        default:    
            DEBUG ((DEBUG_ERROR, "RecoveryImageSize not match, Stop doing BiosGuard recovery.\n"));
            return EFI_UNSUPPORTED;
    }

    //
    // Allocate memory for saving BiosGuard image.
    //
    Status = (*PeiServices)->AllocatePages ( \
             PeiServices, \
             EfiBootServicesData, \
             (RecoveryImageSize >> 12) + 1, \
             &SavedBiosGuardImage);
    ASSERT_EFI_ERROR(Status);
    if ( EFI_ERROR( Status ) ) {
        ASSERT_EFI_ERROR (Status);
        return Status;
    }
    if (!EFI_ERROR(Status)) {
        DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : Saved BiosGuard Image %lx\n", SavedBiosGuardImage));

        //
        // Save BiosGuard image to HOB.
        //
        (*PeiServices)->CopyMem ((UINT8*)SavedBiosGuardImage, (UINT8*)RecoveryRomAddress, RecoveryImageSize);
        Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof(RECOVERY_IMAGE_HOB), &BiosGuardRecoveryImageHob);
        if ( !EFI_ERROR(Status) ) {
            //
            // Create BiosGuard recovery Image HOB for DXE used.
            //
            BiosGuardRecoveryImageHob->Header.Name = gBiosGuardRecoveryImageHobGuid;
            BiosGuardRecoveryImageHob->Address = SavedBiosGuardImage;
            BiosGuardRecoveryImageHob->FailedStage = 0;
            if (MeUpdate) {
                BiosGuardRecoveryImageHob->ImageSize = BIOSGUARD_MEUD_BIOS_SIZE + FV_MEFW_CAPSULE_SIZE;
                if (EcUpdate) {
                    BiosGuardRecoveryImageHob->ImageSize = BIOSGUARD_MEUD_BIOS_SIZE + FV_MEFW_CAPSULE_SIZE + EC_FIRMWARE_BIN_SIZE;
                }
            } else {
                if (EcUpdate) {
                    BiosGuardRecoveryImageHob->ImageSize = BIOSGUARD_MEUD_BIOS_SIZE + EC_FIRMWARE_BIN_SIZE;
                } else {
                    //EIP429536 >>
                    if (BiosUpdate) 
                        BiosGuardRecoveryImageHob->ImageSize = BIOSGUARD_MEUD_BIOS_SIZE;
                    else
                    //EIP429536 <<
                        BiosGuardRecoveryImageHob->ImageSize = BIOSGUARD_RECOVERY_IMAGE_SIZE;
                }
            }            
        }
    }

    if (FWCAPSULE_FILE_FORMAT) {
        FWCapsuleHdr = (UINT8*)(RecoveryRomAddress + RecoveryImageSize - FWCAPSULE_IMAGE_ALLIGN - SIGNATURE_SIZE);
        
        //
        // Skip ME Region if needed (CurrentBGUPAddress = Original image size).
        //
        (*PeiServices)->CopyMem ((UINT8*)RecoveryHob->Address, FWCapsuleHdr, FWCAPSULE_IMAGE_ALLIGN );

        if (MeUpdate || EcUpdate || BiosUpdate) {  //EIP429536
            (*PeiServices)->CopyMem ((UINT8*)RecoveryHob->Address + FWCAPSULE_IMAGE_ALLIGN, (UINT8*)(Buffer), FLASH_SIZE );
        } else {
            (*PeiServices)->CopyMem ((UINT8*)RecoveryHob->Address + FWCAPSULE_IMAGE_ALLIGN, (UINT8*)(Buffer + BiosGuardBiosStartRomAddress), FLASH_SIZE );
        }
        // Copy EC bin
        if (EcUpdate) {
            if (MeUpdate) {
                (*PeiServices)->CopyMem ((UINT8*)(RecoveryHob->Address + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE), (UINT8*)(SavedBiosGuardImage + BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE), EC_FIRMWARE_BIN_SIZE );
            } else {
                (*PeiServices)->CopyMem ((UINT8*)(RecoveryHob->Address + FWCAPSULE_IMAGE_ALLIGN + FLASH_SIZE), (UINT8*)(SavedBiosGuardImage + BIOSGUARD_MEUD_BIOS_SIZE), EC_FIRMWARE_BIN_SIZE );
            }
        }
    } else {
        if (MeUpdate || EcUpdate || BiosUpdate) {  //EIP429536
            (*PeiServices)->CopyMem ((UINT8*)RecoveryHob->Address, (UINT8*)(Buffer), FLASH_SIZE );
        } else {
            (*PeiServices)->CopyMem ((UINT8*)RecoveryHob->Address, (UINT8*)(Buffer + BiosGuardBiosStartRomAddress), FLASH_SIZE );
        }
        // Copy EC bin
        if (EcUpdate) {
            if (MeUpdate) {
                (*PeiServices)->CopyMem ((UINT8*)(RecoveryHob->Address + FLASH_SIZE), (UINT8*)(SavedBiosGuardImage + BIOSGUARD_MEUD_BIOS_SIZE + BIOSGUARD_ME_ALIGNMENT_SIZE), EC_FIRMWARE_BIN_SIZE );
            } else {
                (*PeiServices)->CopyMem ((UINT8*)(RecoveryHob->Address + FLASH_SIZE), (UINT8*)(SavedBiosGuardImage + BIOSGUARD_MEUD_BIOS_SIZE), EC_FIRMWARE_BIN_SIZE );
            }
        }
    }    

    ReportBiosGuardRecoverySize (PeiServices, FLASH_SIZE);
    return EFI_SUCCESS;
}

/**
    LoadRecoveryCapsuleHook function invoke the Original LoadRecoveryCapsule 
    procedure, and check if the loaded ROM is BiosGuard ROM then reconstruct 
    the BIOS image.
        
    @param PeiServices - pointer to PeiServices Structure 
    @param This - pointer to the PPI structure
    @param CapsuleInstance - value indicating the instance of the PPI
    @param Buffer - contains information read from the block device

         
    @retval EFI_SUCCESS-  File read from recovery media
    @retval EFI_INVALID_PARAMETER - Buffer is a NULL pointer
    @retval EFI_NOT_FOUND - asking for a 1 or greater instance of the PPI
    @retval Other return error values from LocatePpi or FsRecoveryRead
**/
EFI_STATUS
LoadRecoveryCapsuleHook (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI *This,
    IN UINTN                              CapsuleInstance,
    OUT VOID                              *Buffer
)
{
    UINT8                                 NumberOfImageProviders = 0;
    RECOVERY_IMAGE_HOB                    RecoveryHob;
    EFI_STATUS                            Status = EFI_LOAD_ERROR;

    DEBUG ((DEBUG_INFO, "\nBiosGuardRecovery.c : LoadRecoveryCapsuleHook Start\n"));

    for (NumberOfImageProviders = 0; gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi != NULL; NumberOfImageProviders++) {
        //
        // Checking "This" for identfiy the LoadRecoveryCapsule call.
        //
        if (This != gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi) continue;

        //
        // Invoke the Original LoadRecoveryCapsule procedure
        //
        Status = gDeviceRecoveryModuleInfo[NumberOfImageProviders].SavedLoadCapsule ( PeiServices, This, CapsuleInstance, Buffer );
        if (EFI_ERROR(Status)) {
            DEBUG ((DEBUG_ERROR, "BiosGuardRecovery.c : Invoke the Original LoadRecoveryCapsule procedure is fail.\n"));
            return Status;
        }
        
        //
        // Recovery image loaded, check if BiosGuard ROM, reconstruct the BIOS image.
        //
        RecoveryHob.Address = (EFI_PHYSICAL_ADDRESS)Buffer;
        GetRecoveryImageSize(PeiServices, &RecoveryHob.ImageSize);

        DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : Ready to do BIOS Guard recovery.\n"));
        Status = BiosGuardRecoveryFileLoaded(PeiServices, &RecoveryHob);
        break;
    }
    return Status;
}

EFI_STATUS 
EFIAPI
BiosGuardRecoveryEntry (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    UINTN                                 NumberOfImageProviders = 0;
    EFI_STATUS                            Status;
    EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *pRecoveryDevice;
    EFI_PEI_PPI_DESCRIPTOR                *pDummy;

    DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : BiosGuardRecoveryEntry Start\n"));

    if ( !((ReadMsr (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (ReadMsr (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        //
        // BIOS Guard is disabled or not supported
        //
        DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : BIOS Guard is disabled or not supported\n"));
        return EFI_SUCCESS;
    }

    //
    // When enable the BIOS_GUARD_MEUD_SUPPORT, that could be happen two recovery size is same.
    // So for avoid this happen we add the RESERVE 0x20000 for BIOS+ME.
    //                             add the RESERVE 0x10000 for IFWI (full image) 
    // That had case that BIOS+ME will big than IFWI. 
    // Here we need check which one is big. than use this for the PcdRecoveryImageSize.
    //
#if defined (BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)
    if (BIOSGUARD_RECOVERY_IMAGE_SIZE < BIOSGUARD_MEUD_RECOVERY_IMAGE_SIZE) {
        PcdSet32S(PcdRecoveryImageSize, BIOSGUARD_MEUD_RECOVERY_IMAGE_SIZE);
    } else
#endif
    PcdSet32S(PcdRecoveryImageSize, BIOSGUARD_RECOVERY_IMAGE_SIZE);

    //
    // Initialize gDeviceRecoveryModuleInfo structure
    //
    for(NumberOfImageProviders = 0; NumberOfImageProviders < MAX_DEVICE_RECOVERY_MODULE; NumberOfImageProviders++) {
        gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi = NULL;
        gDeviceRecoveryModuleInfo[NumberOfImageProviders].SavedLoadCapsule = NULL;
    }

    NumberOfImageProviders = 0;
    do {
        //
        // Discover the Device Recovery Module PPIs for hooking the LoadRecoveryCapule procedure.
        //
        Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiDeviceRecoveryModulePpiGuid, NumberOfImageProviders, &pDummy, &pRecoveryDevice);
        if (!EFI_ERROR(Status)) {
            //
            // Save Ppi pointer and LoadRecoveryCapsule procedure for Hook used.
            //
            gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi = pRecoveryDevice;
            gDeviceRecoveryModuleInfo[NumberOfImageProviders].SavedLoadCapsule = pRecoveryDevice->LoadRecoveryCapsule;
            NumberOfImageProviders++;
            //
            // Hook the LaodRecoveryCapsule procedure.
            //
            pRecoveryDevice->LoadRecoveryCapsule = LoadRecoveryCapsuleHook;
        }
    } while( !EFI_ERROR(Status) );

    DEBUG ((DEBUG_INFO, "BiosGuardRecovery.c : BiosGuardRecoveryEntry End\n"));
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
