//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiBiosGuard.h
  
*/

#ifndef __AMI_BIOSGUARD_H__
#define __AMI_BIOSGUARD_H__

//
// Common definition
//
#define VKEYMOD_SIZE    0x100
#define VKEYEXP_SIZE    0x4
#define SIGNATURE_SIZE  0x100

#define INVALID_IMAGE_SIZE  0x10000000

#ifndef BIOSGUARD_MEUD_BIOS_SIZE
#define BIOSGUARD_MEUD_BIOS_SIZE        INVALID_IMAGE_SIZE
#endif

#ifndef BIOSGUARD_ME_ALIGNMENT_SIZE
#define BIOSGUARD_ME_ALIGNMENT_SIZE     INVALID_IMAGE_SIZE
#endif

#ifndef EC_FIRMWARE_BIN_ALIGNMENT_SIZE
#define EC_FIRMWARE_BIN_ALIGNMENT_SIZE  INVALID_IMAGE_SIZE+0x1000000
#endif

#ifndef FWCAPSULE_FILE_FORMAT
#define FWCAPSULE_FILE_FORMAT   0
#endif

#ifndef FWCAPSULE_IMAGE_ALLIGN
#define FWCAPSULE_IMAGE_ALLIGN  4096
#endif

#ifndef FV_MEFW_CAPSULE_SIZE
#define FV_MEFW_CAPSULE_SIZE    INVALID_IMAGE_SIZE
#endif

#ifndef EC_FIRMWARE_BIN_SIZE
#define EC_FIRMWARE_BIN_SIZE    INVALID_IMAGE_SIZE+0x1000000
#endif

//
// Structure definition
//
#pragma pack (1)
typedef struct {
    //
    // AmiBiosGuardRecoveryAddress for Tool Interface
    //
    UINT64                      Signature;                  // _AMIBSGD
    UINT16                      Version;                    // 0
    UINT16                      ReoveryMode;                // 1: Recovery, 2: Capsule
    UINT32                      ReoveryAddress;             // AP pointer;
} AMI_BIOS_GUARD_RECOVERY;
#pragma pack ()

typedef struct {
    EFI_FFS_FILE_HEADER         FfsHdr;
    EFI_COMMON_SECTION_HEADER   SecHdr;
    EFI_GUID                    SectionGuid;
    UINT8                       FwCapHdr[0];
} AMI_FFS_COMMON_SECTION_HEADER;

typedef enum
{
    BiosGuardRecoveryMode = 1,
    BiosGuardCapsuleMode = 2
} BIOS_GUARD_RECOVERY_TYPE;

#endif


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
