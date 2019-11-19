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
#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AmiCapsuleUpdateLib.h>
#include <Guid/FmpCapsule.h>
#include <Capsule.h>

EFI_STATUS EFIAPI SupportCapsuleImage(
    IN EFI_CAPSULE_HEADER *CapsuleHeader
){
    static EFI_GUID gW8ScreenImageGuid = W8_SCREEN_IMAGE_CAPSULE_GUID;
    static EFI_GUID* ExtraSupportedCapsuleGuid[] = {
        &gW8ScreenImageGuid, &gEfiFmpCapsuleGuid,
        NULL
    };
    UINTN i;

    if (AmiIsFwUpdateCapsule (CapsuleHeader))
        return EFI_SUCCESS;

    for(i = 0; ExtraSupportedCapsuleGuid[i] != NULL; i++){
        if (CompareGuid (ExtraSupportedCapsuleGuid[i], &CapsuleHeader->CapsuleGuid))
            return EFI_SUCCESS;
    }
    return EFI_UNSUPPORTED;
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
