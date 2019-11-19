//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
/** @file FitPeiProcessFvBeforePublishing.h
    Header file for FitPeiProcessFvBeforePublishing

**/
//*************************************************************************

#ifndef _FIT_PEI_PROCESS_FV_BEFORE_PUBLISHING_H_
#define _FIT_PEI_PROCESS_FV_BEFORE_PUBLISHING_H_

#include <Ppi/EndOfPeiPhase.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <BaseCryptLib.h>

#include <CpuAccess.h>
#include <Library/BootGuardLib.h>
#include <Token.h>
#include <Library/PostCodeLib.h>
#include <Library/PcdLib.h>

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
#define RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY_FFS_FILE_RAW_GUID \
    {0x98db68e0, 0x5ab6, 0x4a48, 0x80, 0xc8, 0xea, 0xc6, 0xc5, 0x11, 0x80, 0xfc}
#endif

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)      
#define NUMBER_FVMAIN_SEGMAMENT 3
#define RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY_FFS_FILE_RAW_GUID \
    {0xcbc91f44, 0xa4bc, 0x4a5b, 0x86, 0x96, 0x70, 0x34, 0x51, 0xd0, 0xb0, 0x53}
#endif

#define AMI_BOOT_GUARD_HOB_GUID \
    {0xb60ab175, 0x498d, 0x429d, 0xad, 0xba, 0xa, 0x62, 0x2c, 0x58, 0x16, 0xe2}

#if defined(BUILD_WITH_GLUELIB)
#undef SetMem
VOID *
SetMem (
    OUT VOID *Buffer,
    IN UINTN Length,
    IN UINT8 Value
)
{
    return GlueSetMem (Buffer, Length, Value);
}

#undef CopyMem
VOID *
CopyMem (
    OUT VOID       *DestinationBuffer,
    IN CONST VOID  *SourceBuffer,
    IN UINTN       Length
)
{
    return GlueCopyMem (DestinationBuffer, SourceBuffer, Length);
}
#endif



#pragma pack (1)

typedef struct {
    EFI_HOB_GUID_TYPE EfiHobGuidType;
    UINT8             AmiBootGuardVerificationFlag;
} AMI_BOOT_GUARD_HOB;

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1) 
typedef struct {
    UINT8             BootGuardFvBbAfterMemHashKey0[32];
    UINT32            BootGuardFvBbAfterMemSegmentBase0;
    UINT32            BootGuardFvBbAfterMemSegmentSize0;
#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP) && (FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP == 1) 
    UINT8             BootGuardFvBbAfterMemHashKey1[32];
    UINT32            BootGuardFvBbAfterMemSegmentBase1;
    UINT32            BootGuardFvBbAfterMemSegmentSize1;
#endif        
} RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY;
#endif

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)      
typedef struct {
    UINT8             BootGuardFvMainHashKey[32];
    UINT32            BootGuardFvMainSegmentBase[NUMBER_FVMAIN_SEGMAMENT];
    UINT32            BootGuardFvMainSegmentSize[NUMBER_FVMAIN_SEGMAMENT];
} RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY;
#endif

#pragma pack ()

#endif

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
