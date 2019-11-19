//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file BootGuardTPMPei.h
    TPM Initialization Flow for Boot Guard

**/
//**********************************************************************

#ifndef _BOOT_GUARD_TPM_PEI_H_
#define _BOOT_GUARD_TPM_PEI_H_

//
// Define macros to build data structure signatures from characters.
//
#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#define EFI_SIGNATURE_64(A, B, C, D, E, F, G, H) \
    (EFI_SIGNATURE_32 (A, B, C, D) | ((UINT64) (EFI_SIGNATURE_32 (E, F, G, H)) << 32))

#define IBB_ENTRYPOINT_M  0xFFFFFFC0
#define ACM_STATUS (*(UINT32*)0xFED30328)

// The LocateTcgPPi(...) entry is call the AmiTcgPlatformPeiLib.obj
EFI_STATUS LocateTcgPpi(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_TPM_PPI      **gTpmDevicePpi,
    IN PEI_TCG_PPI      **gTcgPpi
);

#pragma pack(1)
typedef struct _TCG_PEI_CALLBACK_CONTEXT
{
    PEI_TPM_PPI      *TpmDevice;
    EFI_PEI_SERVICES **PeiServices;
} TCG_PEI_CALLBACK_CONTEXT;

typedef struct _BOOT_POLICY
{
    UINT8            RSTR;
    UINT8            TYPE;
    UINT16           ACM_SVN;
    UINT8            ACM_Signature[256];
    UINT8            Key_Manifest_Signature[256];
    UINT8            Boot_Policy_Manifest_Signature[256];
    UINT8            Digest_of_Hashed_IBB_Segment[32];
} BOOT_POLICY;

typedef struct _FIT_ENTRY
{
    UINT64           TblAddress;
    UINT32           TblSIZE;
    UINT16           TblVer;
    UINT8            TblType;
    UINT8            TblChkSum;
} FIT_ENTRY;

//
// Manifest definition
//
#define TPM_ALG_SHA1        0x4
#define TPM_ALG_SHA256      0xB
#define SHA1_DIGEST_SIZE    20
#define SHA256_DIGEST_SIZE  32

typedef struct {
    UINT16   HashAlg;
    UINT16   Size;
    UINT8    HashBuffer[SHA256_DIGEST_SIZE];
} HASH_STRUCTURE;

#define RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT   (RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT/8)
#define RSA_PUBLIC_KEY_STRUCT_KEY_EXPONENT_DEFAULT  0x10001 // NOT 0x10001
typedef struct {
    UINT8    Version;
    UINT16   KeySize;
    UINT32   Exponent;
    UINT8    Modulus[RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT];
} RSA_PUBLIC_KEY_STRUCT;

#define RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT   (RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT/8)
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
    UINT8              StructureID[8];
    UINT8              StructVersion;
    UINT8              HdrStructVersion;
    UINT8              PMBPMVersion;
    UINT8              BPSVN_BPM;
    UINT8              ACMSVN_BPM;
    UINT8              Reserved;
    UINT16             NEMDataStack;
} BOOT_POLICY_MANIFEST_HEADER;

typedef struct {
    UINT16   Reserved;
    UINT16   Flags;
    UINT32   Base;
    UINT32   Size;
} IBB_SEGMENT_ELEMENT;

typedef struct {
    UINT8               StructureID[8];
    UINT8               StructVersion;
    UINT8               SetNumber;
    UINT8               Reserved;
    UINT8               PBETValue;
    UINT32              Flags;
    UINT64              IBB_MCHBAR;
    UINT64              VTD_BAR;
    UINT32              PMRL_Base;
    UINT32              PMRL_Limit;
    UINT64              PMRH_Base;
    UINT64              PMRH_Limit;
    HASH_STRUCTURE      PostIbbHash;
    UINT32              EntryPoint;
    HASH_STRUCTURE      Digest;
    UINT8               SegmentCount; // 1 ~ 8
    IBB_SEGMENT_ELEMENT IBBSegment[4];
} IBB_ELEMENT;

typedef struct {
    UINT8    StructureID[8];
    UINT8    StructVersion;
    UINT16   PMDataSize;
//UINT8    PMData[PMDataSize];
} PLATFORM_MANUFACTURER_ELEMENT;

typedef struct {
    UINT8                 StructureID[8];
    UINT8                 StructVersion;
    KEY_SIGNATURE_STRUCT  KeySignature;
} BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT;

typedef struct {
    BOOT_POLICY_MANIFEST_HEADER                   Bpm_Header;
    IBB_ELEMENT                                   Ibb_Element;
    //PLATFORM_MANUFACTURER_ELEMENT                 Platform_Manufacture_Element;
    BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT        Bpm_Signature_Element;
} BpmStruct;

typedef struct {
    UINT8                  StructureID[8];
    UINT8                  StructVersion;
    UINT8                  KeyManifestVersion;
    UINT8                  KMSVN;
    UINT8                  KeyManifestID;
    HASH_STRUCTURE         BPKey;
    KEY_SIGNATURE_STRUCT   KeyManifestSignature;
} KEY_MANIFEST_STRAUCTURE;

typedef struct {
  UINT32 ModuleType;
  UINT32 HeaderLen;
  UINT32 HeaderVersion;
  UINT16 ChipsetId;
  UINT16 Flags;
  UINT32 ModuleVendor;
  UINT32 Date;
  UINT32 Size;
  UINT16 AcmSvn;
  UINT16 Reserved1;
  UINT32 CodeControl;
  UINT32 ErrorEntryPoint;
  UINT32 GdtLimit;
  UINT32 GdtBasePtr;
  UINT32 SegSel;
  UINT32 EntryPoint;
  UINT8  Reserved2[64];
  UINT32 KeySize;
  UINT32 ScratchSize;
  UINT8  RsaPubKey[64 * 4];
  UINT32 RsaPubExp;
  UINT8  RsaSig[256];
} ACM_FORMAT;

#define ACM_HEADER_FLAG_DEBUG_SIGNED     BIT15
#define ACM_NPW_SVN                      0x2

//
// Boot Policy Restrictions definition
//
typedef union {
  struct {
    UINT8 Facb     : 1;
    UINT8 Dcd      : 1;
    UINT8 Dbi      : 1;
    UINT8 Pbe      : 1;
    UINT8 Bbp      : 1;
    UINT8 Reserved : 2;
    UINT8 BpInvd   : 1;
  } Bits;
  UINT8 Data;
} BP_RSTR_LOW;

//
// Boot Policy Type definition
//
typedef union {
  struct {
    UINT8 MeasuredBoot : 1;
    UINT8 VerifiedBoot : 1;
    UINT8 Hap          : 1;
    UINT8 Reserved     : 5;
  } Bits;
  UINT8    Data;
} BP_TYPE_LOW;

//
// ACM definition
//
#define MMIO_ACM_STATUS  (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS)

#pragma pack()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
