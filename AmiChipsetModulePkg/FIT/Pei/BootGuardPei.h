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
/** @file BootGuardPei.h
    Header file for BootGuardPei

**/
//*************************************************************************

#ifndef _BOOT_GUARD_PEI_H_
#define _BOOT_GUARD_PEI_H_

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

#define NUMBER_FVMAIN_SEGMAMENT 3

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1)
extern EFI_GUID gAmiPeiEndOfMemDetectGuid;
#define RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_HASH_KEY_FFS_FILE_RAW_GUID \
    {0x98db68e0, 0x5ab6, 0x4a48, 0x80, 0xc8, 0xea, 0xc6, 0xc5, 0x11, 0x80, 0xfc}
#endif

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
#define RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY_FFS_FILE_RAW_GUID \
    {0xcbc91f44, 0xa4bc, 0x4a5b, 0x86, 0x96, 0x70, 0x34, 0x51, 0xd0, 0xb0, 0x53}
#endif

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

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_VERIFY_FAILED_FUNCTION == 0)
static EFI_PEI_PPI_DESCRIPTOR RecoveryModePpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiBootInRecoveryModePpiGuid, NULL
};
#endif

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1) 
STATIC
EFI_STATUS
EFIAPI
BootGuardVerificationForPeiToPeiAfterMemory (
    IN      EFI_PEI_SERVICES          **PeiServices,
    IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN      VOID                      *Ppi
);
#endif

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT == 1)
STATIC
EFI_STATUS
EFIAPI
BootGuardVerificationForPeiToDxeHandoffEndOfPei (
    IN      EFI_PEI_SERVICES          **PeiServices,
    IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN      VOID                      *Ppi
);
#endif

STATIC
EFI_STATUS
EFIAPI
BootGuardHandleTpmFailure (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
    IN  VOID                        *Ppi
);

#pragma pack (1)

#define MAX_NUM_FV_REGION_SEGMAMENT 2
#define SHA256_DIGEST_SIZE  32
#define RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT   (RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT/8)
#define RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT   (RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT/8)

typedef struct crypto_rsa_key {
	int private_key; /* whether private key is set */
	struct bignum *n; /* modulus (p * q) */
	struct bignum *e; /* public exponent */
	/* The following parameters are available only if private_key is set */
//	struct bignum *d; /* private exponent */
//	struct bignum *p; /* prime p (factor of n) */
//	struct bignum *q; /* prime q (factor of n) */
//	struct bignum *dmp1; /* d mod (p - 1); CRT exponent */
//	struct bignum *dmq1; /* d mod (q - 1); CRT exponent */
//	struct bignum *iqmp; /* 1 / q mod p; CRT coefficient */
}CRYPTO_RSA_KEY;

typedef struct {
    UINT16   HashAlg;
    UINT16   Size;
    UINT8    HashBuffer[SHA256_DIGEST_SIZE];
} HASH_STRUCTURE;

typedef struct {
    UINT8    Version;
    UINT16   KeySize;
    UINT32   Exponent;
    UINT8    Modulus[RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT];
} RSA_PUBLIC_KEY_STRUCT;

typedef struct {
    UINT8    Version;
    UINT16   KeySize;
    UINT16   HashAlg;
    UINT8    Signature[RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT];
} RSASSA_SIGNATURE_STRUCT;

typedef struct {
    UINT8                    Version;
    UINT16                   KeyAlg;
    RSA_PUBLIC_KEY_STRUCT    Key;
    UINT16                   SigScheme;
    RSASSA_SIGNATURE_STRUCT  Signature;
} KEY_SIGNATURE_STRUCT;

typedef struct {
    UINT32                 BootGuardFvAreaSegmentBase[MAX_NUM_FV_REGION_SEGMAMENT];
    UINT32                 BootGuardFvAreaSegmentSize[MAX_NUM_FV_REGION_SEGMAMENT];
    UINT32                 Flag; // Bit 0: Backup Region
                                 // Bit 1: Backup Binary
    HASH_STRUCTURE         FvHash;
    KEY_SIGNATURE_STRUCT   FvRegionSignature;
} RESERVE_FV_REGION_INFO;

#if (defined FV_BB_VERIFY_FV_AFTER_MEMORY) && (FV_BB_VERIFY_FV_AFTER_MEMORY == 1) 
typedef struct {
    UINT8             BootGuardFvRegionHashKey[32];
    UINT32            BootGuardFvRegionInfoOffset;
} RESERVE_BOOT_GUARD_FV_BB_AFTER_MEM_INFO;
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
