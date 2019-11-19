//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/**
 * @file AmiCapsuleUpdateLib.c
 * @brief Implementation of AmiCapsuleUpdate default library
 */

#include <Token.h>
#include <Capsule.h>
#include <AmiFwCapsuleGuids.h>
#include <Library/BaseMemoryLib.h>

#ifndef BDS_FW_UPDATE_CAPSULE_LOAD
#define BDS_FW_UPDATE_CAPSULE_LOAD 0
#endif

/**
 * @brief Returns payload from capsule
 *
 * @param Capsule Pointer to capsule
 * @param CapsuleSize Capsule size
 * @param Payload Pointer where to return pointer to payload
 * @param PayloadSize Pointer where to return payload size
 *
 * @return Status of operation
 */
EFI_STATUS AmiPreprocessCapsule (
    IN EFI_CAPSULE_HEADER *Capsule,
    IN UINT32 CapsuleSize,
    OUT VOID **Payload OPTIONAL,
    OUT UINT32 *PayloadSize OPTIONAL
    )
{
    static EFI_GUID gESRTCapsuleGuid = W8_FW_UPDATE_IMAGE_CAPSULE_GUID;
    static EFI_GUID gAmiFwCapsuleGuid   = APTIO_FW_CAPSULE_GUID;
    UINT8 *p = (UINT8 *)Capsule;
    UINT32 sz = CapsuleSize;

    //we're skipping capsule header in two cases:
    //1. ESRT-based firmware update
    //2. AFU-based firmware update with capsule processing in SMM
    if (CompareGuid (&Capsule->CapsuleGuid, &gESRTCapsuleGuid) ||
        (CompareGuid (&Capsule->CapsuleGuid, &gAmiFwCapsuleGuid) && BDS_FW_UPDATE_CAPSULE_LOAD)) {
        p = p + Capsule->HeaderSize;
        sz = CapsuleSize - Capsule->HeaderSize;
    }

    if (Payload)
        *Payload = p;
    if (PayloadSize)
        *PayloadSize = sz;

    return EFI_SUCCESS;
}

/**
 * @brief Checks if capsule is main firmware update capsule
 *
 * @param Capsule Pointer to capsule
 *
 * @retval TRUE - Capsule is the main firmware update capsule
 * @retval FALSE - Capsule is not the main firmware update capsule
 */
BOOLEAN AmiIsFwUpdateCapsule (
    IN EFI_CAPSULE_HEADER *Capsule
    )
{
    UINT32 i;
    for (i = 0; i < AmiFwCapsuleGuidListSize; i++) {
        if (CompareGuid (&Capsule->CapsuleGuid, AmiFwCapsuleGuidList + i))
        return TRUE;
    }
    return FALSE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
