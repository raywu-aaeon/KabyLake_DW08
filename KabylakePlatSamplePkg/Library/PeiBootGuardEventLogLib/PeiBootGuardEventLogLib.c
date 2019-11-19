/** @file
  This file contains functions for creating TPM Event Log

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification
**/

#include <PiPei.h>

#include <IndustryStandard/UefiTcgPlatform.h>
#include <Guid/TcgEventHob.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/IoLib.h>
#include <Library/Tpm12CommandLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Ppi/FirmwareVolumeInfoMeasurementExcluded.h>
#include <TrEEConfigNvData.h>
#include <Protocol/TrEEProtocol.h>
#include <Protocol/Tcg2Protocol.h>
#include <Library/MmPciLib.h>
#include <MeChipset.h>
#include <CpuRegs.h>
#include <Library/BootGuardLib.h>
#include <Fit.h>
#include <Library/PeiBootGuardEventLogLib.h>

#define MMIO_ACM_STATUS  (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS) // 0xFED30328

//
// Chipset register, need double check if it is right.
//
#define ACM_KEY_HASH_MMIO_ADDR_0            0xFED30400
#define ACM_KEY_HASH_MMIO_ADDR_1            (ACM_KEY_HASH_MMIO_ADDR_0 + 8)
#define ACM_KEY_HASH_MMIO_ADDR_2            (ACM_KEY_HASH_MMIO_ADDR_0 + 16)
#define ACM_KEY_HASH_MMIO_ADDR_3            (ACM_KEY_HASH_MMIO_ADDR_0 + 24)

//
// Data structure definition
//
#pragma pack (1)

//
// ACM definition
//

#define ACM_PKCS_1_5_RSA_SIGNATURE_SIZE  256
#define ACM_MODULE_TYPE_CHIPSET_ACM      2
#define ACM_MODULE_SUBTYPE_CAPABLE_OF_EXECUTE_AT_RESET  0x1
#define ACM_MODULE_SUBTYPE_ANC_MODULE                   0x2
#define ACM_HEADER_FLAG_DEBUG_SIGNED     BIT15
#define ACM_NPW_SVN                      0x2

typedef struct {
  UINT16     ModuleType;
  UINT16     ModuleSubType;
  UINT32     HeaderLen;
  UINT32     HeaderVersion;
  UINT16     ChipsetID;
  UINT16     Flags;
  UINT32     ModuleVendor;
  UINT32     Date;
  UINT32     Size;
  UINT16     ACM_SVN;
  UINT16     SE_ACM_SVN;
  UINT32     CodeControl;
  UINT32     ErrorEntryPoint;
  UINT32     GDTLimit;
  UINT32     GDTBasePtr;
  UINT32     SegSel;
  UINT32     EntryPoint;
  UINT8      Rsvd2[64];
  UINT32     KeySize; // 64
  UINT32     ScratchSize; // 2 * KeySize + 15
  UINT8      RSAPubKey[64 * 4]; // KeySize * 4
  UINT32     RSAPubExp;
  UINT8      RSASig[256];
  // End of AC module header
//UINT8      Scratch[(64 * 2 + 15) * 4]; // ScratchSize * 4
  // User Area
//UINT8      UserArea[1];
} ACM_FORMAT;

//
// BPM Policy:
//   FIT record type 12 points to Boot Policy Manifest.
//   FIT record type 11 points to Key Manifest.
//

typedef struct {
  UINT16   HashAlg;
  UINT16   Size;
  UINT8    HashBuffer[SHA256_DIGEST_SIZE];
} HASH_STRUCTURE;

typedef struct {
  EFI_GUID               *EventGuid;
  TREE_EVENT_LOG_FORMAT  LogFormat;
} TREE_EVENT_INFO_STRUCT;

#define ALG_RSA             0x1
#define ALG_RSASSA          0x14

#define RSA_PUBLIC_KEY_STRUCT_VERSION  0x10
#define RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT   (RSA_PUBLIC_KEY_STRUCT_KEY_SIZE_DEFAULT/8)
#define RSA_PUBLIC_KEY_STRUCT_KEY_EXPONENT_DEFAULT  0x11 // NOT 0x10001
typedef struct {
  UINT8    Version;
  UINT16   KeySize;
  UINT32   Exponent;
  UINT8    Modulus[RSA_PUBLIC_KEY_STRUCT_KEY_LEN_DEFAULT];
} RSA_PUBLIC_KEY_STRUCT;

#define RSASSA_SIGNATURE_STRUCT_VERSION  0x10
#define RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT  2048
#define RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT   (RSASSA_SIGNATURE_STRUCT_KEY_SIZE_DEFAULT/8)
typedef struct {
  UINT8    Version;
  UINT16   KeySize;
  UINT16   HashAlg;
  UINT8    Signature[RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT];
} RSASSA_SIGNATURE_STRUCT;

#define KEY_SIGNATURE_STRUCT_VERSION  0x10
typedef struct {
  UINT8                    Version;
  UINT16                   KeyAlg;
  RSA_PUBLIC_KEY_STRUCT    Key;
  UINT16                   SigScheme;
  RSASSA_SIGNATURE_STRUCT  Signature;
} KEY_SIGNATURE_STRUCT;

#define BP_KEY_TYPE_BOOT_POLICY_MANIFEST  0
#define BP_KEY_TYPE_KEY_MANIFEST          1

#define BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_ID  (*(UINT64 *)"__ACBP__")
#define BOOT_POLICY_MANIFEST_STRUCTURE_VERSION              0x10
#define BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_VERSION       0x01
typedef struct {
  UINT8              StructureID[8];
  UINT8              StructVersion;
  UINT8              HdrStructVersion;
  UINT8              PMBPMVersion;
  UINT8              BPSVN;
  UINT8              ACMSVN;
  UINT8              Reserved;
  UINT16             NEMDataStack;
} BOOT_POLICY_MANIFEST_HEADER;

#define IBB_SEGMENT_FLAG_IBB     0x0
#define IBB_SEGMENT_FLAG_NON_IBB 0x1
#define IBB_ALIGNMENT            0x3F  // 64 bytes for BootGuard
typedef struct {
  UINT8    Reserved[2];
  UINT16   Flags;
  UINT32   Base;
  UINT32   Size;
} IBB_SEGMENT_ELEMENT;

#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__IBBS__")
#define BOOT_POLICY_MANIFEST_IBB_ELEMENT_VERSION       0x10
#define IBB_FLAG_ENABLE_VTD            0x1
#define IBB_FLAG_INITIAL_MEASURE_LOC3  0x2
#define IBB_FLAG_AUTHORITY_MEASURE     0x4
#define IBB_ENTRYPOINT_MIN   0xFFFFFFC0
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
  IBB_SEGMENT_ELEMENT IBBSegment[1];
} IBB_ELEMENT;

#define BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PMDA__")
#define BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_VERSION       0x10
#define PLATFORM_DATA_ALIGNMENT  0x3  // 4 bytes
typedef struct {
  UINT8    StructureID[8];
  UINT8    StructVersion;
  UINT16   PMDataSize;
//UINT8    PMData[PMDataSize];
} PLATFORM_MANUFACTURER_ELEMENT;

#define BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID  (*(UINT64 *)"__PMSG__")
#define BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_VERSION       0x10
typedef struct {
  UINT8                 StructureID[8];
  UINT8                 StructVersion;
  KEY_SIGNATURE_STRUCT  KeySignature;
} BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT;

#define KEY_MANIFEST_STRUCTURE_ID  (*(UINT64 *)"__KEYM__")
#define KEY_MANIFEST_VERSION       0x10
typedef struct {
  UINT8                  StructureID[8];
  UINT8                  StructVersion;
  UINT8                  KeyManifestVersion;
  UINT8                  KMSVN;
  UINT8                  KeyManifestID;
  HASH_STRUCTURE         BPKey;
  KEY_SIGNATURE_STRUCT   KeyManifestSignature;
} KEY_MANIFEST_STRAUCTURE;

//
// Detail PCR data
//
typedef struct {
  UINT8  BP_RSTR_Low;
  UINT8  BP_TYPE_Low;
  UINT16 ACM_SVN;
  UINT8  AcmRsaSignature[ACM_PKCS_1_5_RSA_SIGNATURE_SIZE];
  UINT8  KmRsaSignature[RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT];
  UINT8  BpmRsaSignature[RSASSA_SIGNATURE_STRUCT_KEY_LEN_DEFAULT];
  UINT8  IbbHash[SHA256_DIGEST_SIZE];
} DETAIL_PCR_DATA;

//
// Authority PCR data
//
typedef struct {
  UINT8  BP_RSTR_Low;
  UINT8  BP_TYPE_Low;
  UINT16 ACM_SVN;
  UINT8  AcmKeyHash[SHA256_DIGEST_SIZE];
  UINT8  BpKeyHash[SHA256_DIGEST_SIZE];
  UINT8  BpmKeyHashFromKm[SHA256_DIGEST_SIZE];
} AUTHORITY_PCR_DATA;

//
// Efi Startup Locality Event Data
//
typedef struct {
  UINT8   Signature[16];
  UINT8   StartupLocality;
} TCG_EFI_STARTUP_LOCALITY_EVENT;

typedef union {
  struct {
    UINT8  FACB: 1;
    UINT8  DCD: 1;
    UINT8  DBI: 1;
    UINT8  PBE: 1;
    UINT8  BBP: 1;
    UINT8  Rsvd: 2;
    UINT8  BPInvd: 1;
  } Bits;
  UINT8    Data;
} BP_RSTR_LOW;

typedef union {
  struct {
    UINT8  MeasuredBoot: 1;
    UINT8  VerifiedBoot: 1;
    UINT8  HAP: 1;
    UINT8  Rsvd: 5;
  } Bits;
  UINT8    Data;
} BP_TYPE_LOW;

#pragma pack ()

TREE_EVENT_INFO_STRUCT mTreeEventInfo[] = {
  {&gTcgEventEntryHobGuid,             EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2},
  {&gTcgEvent2EntryHobGuid,            EFI_TCG2_EVENT_LOG_FORMAT_TCG_2},
};

//
// This PPI means a FV does not need to be extended to PCR by TCG modules.
//
typedef struct {
  UINT32                                                Count;
  EFI_PEI_FIRMWARE_VOLUME_INFO_MEASUREMENT_EXCLUDED_FV  Fv[3];
} FIRMWARE_VOLUME_INFO_MEASUREMENT_EXCLUDED_PPI_2;

FIRMWARE_VOLUME_INFO_MEASUREMENT_EXCLUDED_PPI_2 mPeiFirmwareVolumeInfoMeasurementExcludedPpi = {
  3,
  {
    {
      FixedPcdGet32 (PcdFlashFvRecoveryBase),
      FixedPcdGet32 (PcdFlashFvRecoverySize)
    },
    {
      FixedPcdGet32 (PcdFlashFvRecovery2Base),
      FixedPcdGet32 (PcdFlashFvRecovery2Size)
    },
  }
};

EFI_PEI_PPI_DESCRIPTOR  mPeiFirmwareVolumeInfoMeasurementExcludedPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiFirmwareVolumeInfoMeasurementExcludedPpiGuid,
  &mPeiFirmwareVolumeInfoMeasurementExcludedPpi
};
/**
  Dump raw data.

  @param[in]  Data  raw data
  @param[in]  Size  raw data size

**/
VOID
InternalDumpData (
  IN UINT8  *Data,
  IN UINTN  Size
  )
{
  UINTN  Index;
  for (Index = 0; Index < Size; Index++) {
    DEBUG ((DEBUG_INFO, "%02x", (UINTN)Data[Index]));
  }
}

/**
  Dump raw data with colume format.

  @param[in]  Data  raw data
  @param[in]  Size  raw data size

**/
VOID
InternalDumpHex (
  IN UINT8  *Data,
  IN UINTN  Size
  )
{
  UINTN   Index;
  UINTN   Count;
  UINTN   Left;

#define COLUME_SIZE  (16 * 2)

  Count = Size / COLUME_SIZE;
  Left  = Size % COLUME_SIZE;
  for (Index = 0; Index < Count; Index++) {
    DEBUG ((DEBUG_INFO, "%04x: ", Index * COLUME_SIZE));
    InternalDumpData (Data + Index * COLUME_SIZE, COLUME_SIZE);
    DEBUG ((DEBUG_INFO, "\n"));
  }

  if (Left != 0) {
    DEBUG ((DEBUG_INFO, "%04x: ", Index * COLUME_SIZE));
    InternalDumpData (Data + Index * COLUME_SIZE, Left);
    DEBUG ((DEBUG_INFO, "\n"));
  }
}

/**
  Dump DetailPcr data

  @param[in] DetailPcrData DetailPcr data
**/
VOID
DumpDetailPcr (
  IN DETAIL_PCR_DATA                          *DetailPcrData
  )
{
  DEBUG ((DEBUG_INFO, "DetailPcrData:\n"));
  //InternalDumpHex ((UINT8 *)DetailPcrData, sizeof(*DetailPcrData));
  DEBUG ((DEBUG_INFO, "BP_RSTR_Low      - 0x%02x\n", DetailPcrData->BP_RSTR_Low));
  DEBUG ((DEBUG_INFO, "BP_TYPE_Low      - 0x%02x\n", DetailPcrData->BP_TYPE_Low));
  DEBUG ((DEBUG_INFO, "ACM_SVN          - 0x%04x\n", DetailPcrData->ACM_SVN));
  DEBUG ((DEBUG_INFO, "AcmRsaSignature  - \n"));
  InternalDumpHex ((UINT8 *)DetailPcrData->AcmRsaSignature, sizeof(DetailPcrData->AcmRsaSignature));
  DEBUG ((DEBUG_INFO, "KmRsaSignature   - \n"));
  InternalDumpHex ((UINT8 *)DetailPcrData->KmRsaSignature, sizeof(DetailPcrData->KmRsaSignature));
  DEBUG ((DEBUG_INFO, "BpmRsaSignature  - \n"));
  InternalDumpHex ((UINT8 *)DetailPcrData->BpmRsaSignature, sizeof(DetailPcrData->BpmRsaSignature));
  DEBUG ((DEBUG_INFO, "IbbHash          - "));
  InternalDumpData ((UINT8 *)DetailPcrData->IbbHash, sizeof(DetailPcrData->IbbHash));
  DEBUG ((DEBUG_INFO, "\n"));
}

/**
  Dump AuthorityPCR data

  @param[in] AuthorityPcrData AuthorityPCR data
**/
VOID
DumpAuthorityPcr (
  IN AUTHORITY_PCR_DATA                       *AuthorityPcrData
  )
{
  DEBUG ((DEBUG_INFO, "AuthorityPcrData:\n"));
  //InternalDumpHex ((UINT8 *)AuthorityPcrData, sizeof(*AuthorityPcrData));
  DEBUG ((DEBUG_INFO, "BP_RSTR_Low      - 0x%02x\n", AuthorityPcrData->BP_RSTR_Low));
  DEBUG ((DEBUG_INFO, "BP_TYPE_Low      - 0x%02x\n", AuthorityPcrData->BP_TYPE_Low));
  DEBUG ((DEBUG_INFO, "ACM_SVN          - 0x%04x\n", AuthorityPcrData->ACM_SVN));
  DEBUG ((DEBUG_INFO, "AcmKeyHash       - "));
  InternalDumpData ((UINT8 *)AuthorityPcrData->AcmKeyHash, sizeof(AuthorityPcrData->AcmKeyHash));
  DEBUG ((DEBUG_INFO, "\n"));
  DEBUG ((DEBUG_INFO, "BpKeyHash        - "));
  InternalDumpHex ((UINT8 *)AuthorityPcrData->BpKeyHash, sizeof(AuthorityPcrData->BpKeyHash));
  DEBUG ((DEBUG_INFO, "\n"));
  DEBUG ((DEBUG_INFO, "BpmKeyHashFromKm - "));
  InternalDumpData ((UINT8 *)AuthorityPcrData->BpmKeyHashFromKm, sizeof(AuthorityPcrData->BpmKeyHashFromKm));
  DEBUG ((DEBUG_INFO, "\n"));
}

/**
  Check if buffer is all zero.

  @param[in] Buffer      Buffer to be checked.
  @param[in] BufferSize  Size of buffer to be checked.

  @retval TRUE  Buffer is all zero.
  @retval FALSE Buffer is not all zero.
**/
BOOLEAN
IsZeroBuffer (
  IN VOID  *Buffer,
  IN UINTN BufferSize
  )
{
  UINT8 *BufferData;
  UINTN Index;

  BufferData = Buffer;
  for (Index = 0; Index < BufferSize; Index++) {
    if (BufferData[Index] != 0) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
  Get TPML_DIGEST_VALUES data size.

  @param[in]     DigestList    TPML_DIGEST_VALUES data.

  @return TPML_DIGEST_VALUES data size.
**/
UINT32
GetDigestListSize (
  IN TPML_DIGEST_VALUES             *DigestList
  )
{
  UINTN  Index;
  UINT16 DigestSize;
  UINT32 TotalSize;

  TotalSize = sizeof(DigestList->count);
  for (Index = 0; Index < DigestList->count; Index++) {
    DigestSize = GetHashSizeFromAlgo (DigestList->digests[Index].hashAlg);
    TotalSize += sizeof(DigestList->digests[Index].hashAlg) + DigestSize;
  }

  return TotalSize;
}

/**
  Copy TPML_DIGEST_VALUES into a buffer

  @param[in,out] Buffer        Buffer to hold TPML_DIGEST_VALUES.
  @param[in]     DigestList    TPML_DIGEST_VALUES to be copied.

  @return The end of buffer to hold TPML_DIGEST_VALUES.
**/
VOID *
CopyDigestListToBuffer (
  IN OUT VOID                       *Buffer,
  IN TPML_DIGEST_VALUES             *DigestList
  )
{
  UINTN  Index;
  UINT16 DigestSize;

  CopyMem (Buffer, &DigestList->count, sizeof(DigestList->count));
  Buffer = (UINT8 *)Buffer + sizeof(DigestList->count);
  for (Index = 0; Index < DigestList->count; Index++) {
    CopyMem (Buffer, &DigestList->digests[Index].hashAlg, sizeof(DigestList->digests[Index].hashAlg));
    Buffer = (UINT8 *)Buffer + sizeof(DigestList->digests[Index].hashAlg);
    DigestSize = GetHashSizeFromAlgo (DigestList->digests[Index].hashAlg);
    CopyMem (Buffer, &DigestList->digests[Index].digest, DigestSize);
    Buffer = (UINT8 *)Buffer + DigestSize;
  }

  return Buffer;
}

/**
  Get digest from digest list.

  @param[in] HashAlg    digest algorithm
  @param[in] DigestList digest list
  @param[in] Digest     digest

  @retval EFI_SUCCESS   Sha1Digest is found and returned.
  @retval EFI_NOT_FOUND Sha1Digest is not found.
**/
EFI_STATUS
Tpm2GetDigestFromDigestList (
  IN TPMI_ALG_HASH      HashAlg,
  IN TPML_DIGEST_VALUES *DigestList,
  IN VOID               *Digest
  )
{
  UINTN  Index;
  UINT16 DigestSize;

  DigestSize = GetHashSizeFromAlgo (HashAlg);
  for (Index = 0; Index < DigestList->count; Index++) {
    if (DigestList->digests[Index].hashAlg == HashAlg) {
      CopyMem (
        Digest,
        &DigestList->digests[Index].digest,
        DigestSize
        );
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Add a new entry to the Event Log.

  @param[in] DigestList    Pointer to a TPML_DIGEST_VALUES structure.
  @param[in] NewEventHdr   Pointer to a TCG_PCR_EVENT_HDR data structure.
  @param[in] NewEventData  Pointer to the new event data.

  @retval EFI_SUCCESS           The new event log entry was added.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
**/
EFI_STATUS
LogHashEvent (
  IN      TPML_DIGEST_VALUES        *DigestList,
  IN      TCG_PCR_EVENT_HDR         *NewEventHdr,
  IN      UINT8                     *NewEventData
  )
{
  VOID                              *HobData;
  EFI_STATUS                        Status;
  UINTN                             Index;
  EFI_STATUS                        RetStatus;
  TCG_PCR_EVENT2                    *TcgPcrEvent2;
  UINT8                             *DigestBuffer;
  UINT32                            SupportedEventLogs;

  SupportedEventLogs = EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2 | EFI_TCG2_EVENT_LOG_FORMAT_TCG_2;
  DEBUG ((EFI_D_INFO, "SupportedEventLogs - 0x%08x\n", SupportedEventLogs));

  RetStatus = EFI_SUCCESS;
  for (Index = 0; Index < sizeof(mTreeEventInfo)/sizeof(mTreeEventInfo[0]); Index++) {
    if ((SupportedEventLogs & mTreeEventInfo[Index].LogFormat) != 0) {
      DEBUG ((EFI_D_INFO, "  LogFormat - 0x%08x\n", mTreeEventInfo[Index].LogFormat));
      switch (mTreeEventInfo[Index].LogFormat) {
      case EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2:
        Status = Tpm2GetDigestFromDigestList (TPM_ALG_SHA1, DigestList, &NewEventHdr->Digest);
        if (!EFI_ERROR (Status)) {
          HobData = BuildGuidHob (
                     &gTcgEventEntryHobGuid,
                     sizeof (*NewEventHdr) + NewEventHdr->EventSize
                     );
          if (HobData == NULL) {
            RetStatus = EFI_OUT_OF_RESOURCES;
            break;
          }

          CopyMem (HobData, NewEventHdr, sizeof (*NewEventHdr));
          HobData = (VOID *) ((UINT8*)HobData + sizeof (*NewEventHdr));
          CopyMem (HobData, NewEventData, NewEventHdr->EventSize);
        }
        break;

      case EFI_TCG2_EVENT_LOG_FORMAT_TCG_2:
        HobData = BuildGuidHob (
                   &gTcgEvent2EntryHobGuid,
                   sizeof(TcgPcrEvent2->PCRIndex) + sizeof(TcgPcrEvent2->EventType) + GetDigestListSize (DigestList) + sizeof(TcgPcrEvent2->EventSize) + NewEventHdr->EventSize
                   );
        if (HobData == NULL) {
          RetStatus = EFI_OUT_OF_RESOURCES;
          break;
        }

        TcgPcrEvent2 = HobData;
        TcgPcrEvent2->PCRIndex = NewEventHdr->PCRIndex;
        TcgPcrEvent2->EventType = NewEventHdr->EventType;
        DigestBuffer = (UINT8 *)&TcgPcrEvent2->Digest;
        DigestBuffer = CopyDigestListToBuffer (DigestBuffer, DigestList);
        CopyMem (DigestBuffer, &NewEventHdr->EventSize, sizeof(TcgPcrEvent2->EventSize));
        DigestBuffer = DigestBuffer + sizeof(TcgPcrEvent2->EventSize);
        CopyMem (DigestBuffer, NewEventData, NewEventHdr->EventSize);
        break;
      }
    }
  }

  return RetStatus;
}

/**
  Find FIT Entry address data by type

  @param[in] Type   FIT Entry type

  @return FIT entry address data
**/
VOID *
FindFitEntryData (
  IN UINT8   Type
  )
{
  FIRMWARE_INTERFACE_TABLE_ENTRY  *FitEntry;
  UINT32                          EntryNum;
  UINT64                          FitTableOffset;
  UINT32                          Index;

  FitTableOffset = *(UINT64 *)(UINTN)(BASE_4GB - 0x40);
  FitEntry = (FIRMWARE_INTERFACE_TABLE_ENTRY *)(UINTN)FitTableOffset;
  if (FitEntry[0].Address != *(UINT64 *)"_FIT_   ") {
    return NULL;
  }
  if (FitEntry[0].Type != FIT_TABLE_TYPE_HEADER) {
    return NULL;
  }
  EntryNum = *(UINT32 *)(&FitEntry[0].Size[0]) & 0xFFFFFF;
  for (Index = 0; Index < EntryNum; Index++) {
    if (FitEntry[Index].Type == Type) {
      return (VOID *)(UINTN)FitEntry[Index].Address;
    }
  }

  return NULL;
}

/**
  Find ACM address

  @return ACM address
**/
VOID *
FindAcm (
  VOID
  )
{
  return FindFitEntryData (FIT_TABLE_TYPE_STARTUP_ACM);
}

/**
  Find BPM address

  @return BPM address
**/
VOID *
FindBpm (
  VOID
  )
{
  return FindFitEntryData (FIT_TABLE_TYPE_BOOT_POLICY_MANIFEST);
}

/**
  Find KM address

  @return KM address
**/
VOID *
FindKm (
  VOID
  )
{
  return FindFitEntryData (FIT_TABLE_TYPE_KEY_MANIFEST);
}

/**
  Find BPM element by structureID

  @param[in] Bpm           BPM address
  @param[in] StructureID   BPM element StructureID

  @return BPM element
**/
VOID *
FindBpmElement (
  IN BOOT_POLICY_MANIFEST_HEADER              *Bpm,
  IN UINT64                                   StructureID
  )
{
  BOOT_POLICY_MANIFEST_HEADER             *BpmHeader;
  IBB_ELEMENT                             *IbbElement;
  PLATFORM_MANUFACTURER_ELEMENT           *PmElement;
  BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT  *BpmSignatureElement;
  UINT8                                   *Buffer;

  Buffer = (UINT8 *)Bpm;

  BpmHeader = (BOOT_POLICY_MANIFEST_HEADER *)Buffer;
  if (*(UINT64 *)BpmHeader->StructureID != BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_ID) {
    return NULL;
  }
  if (StructureID == BOOT_POLICY_MANIFEST_HEADER_STRUCTURE_ID) {
    return Buffer;
  }
  Buffer += sizeof(BOOT_POLICY_MANIFEST_HEADER);

  IbbElement = (IBB_ELEMENT *)Buffer;
  if (*(UINT64 *)IbbElement->StructureID != BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID) {
    return NULL;
  }
  if (StructureID == BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID) {
    return Buffer;
  }
  Buffer += sizeof(IBB_ELEMENT) + sizeof(IBB_SEGMENT_ELEMENT) * (IbbElement->SegmentCount - 1);

  PmElement = (PLATFORM_MANUFACTURER_ELEMENT *)Buffer;
  while (*(UINT64 *)PmElement->StructureID == BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_STRUCTURE_ID) {
    if (StructureID == BOOT_POLICY_MANIFEST_PLATFORM_MANUFACTURER_ELEMENT_STRUCTURE_ID) {
      return Buffer;
    }
    Buffer += sizeof(PLATFORM_MANUFACTURER_ELEMENT) + PmElement->PMDataSize;
    PmElement = (PLATFORM_MANUFACTURER_ELEMENT *)Buffer;
  }

  BpmSignatureElement = (BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT *)Buffer;
  if (*(UINT64 *)BpmSignatureElement->StructureID != BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID) {
    return NULL;
  }
  if (StructureID == BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID) {
    return Buffer;
  }
  return NULL;
}

/**
  Find BPM IBB element

  @param[in] Bpm           BPM address

  @return BPM IBB element
**/
VOID *
FindBpmIbb (
  IN BOOT_POLICY_MANIFEST_HEADER              *Bpm
  )
{
  return FindBpmElement (Bpm, BOOT_POLICY_MANIFEST_IBB_ELEMENT_STRUCTURE_ID);
}

/**
  Find BPM Signature element

  @param[in] Bpm           BPM address

  @return BPM Signature element
**/
VOID *
FindBpmSignature (
  IN BOOT_POLICY_MANIFEST_HEADER              *Bpm
  )
{
  return FindBpmElement (Bpm, BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT_STRUCTURE_ID);
}

/**
  Check if this NPW ACM

  @retval TRUE  This is NPW ACM
  @retval FALSE This is NOT NPW ACM
**/
BOOLEAN
IsNpwAcm (
  VOID
  )
{
  ACM_FORMAT  *Acm;

  Acm = FindAcm ();
  ASSERT (Acm != NULL);
  if (Acm == NULL) {
    return FALSE;
  }

  if (((Acm->Flags & ACM_HEADER_FLAG_DEBUG_SIGNED) == 0) && (Acm->ACM_SVN < ACM_NPW_SVN)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Check if this is VerifiedBoot

  @retval TRUE  This is VerifiedBoot
  @retval FALSE This is NOT VerifiedBoot
**/
BOOLEAN
IsVerifiedBoot (
  VOID
  )
{
  if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Check if this is MeasuredBoot

  @retval TRUE  This is MeasuredBoot
  @retval FALSE This is NOT MeasuredBoot
**/
BOOLEAN
IsMeasuredBoot (
  VOID
  )
{
  if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Get BP.RSTR low byte

  @return BP.RSTR low byte
**/
UINT8
GetBPRSTRLow (
  VOID
  )
{
  BP_RSTR_LOW   BpRstr;
  UINT32        AcmStatus;
  UINT64        SacmInfo;

  //
  // Calculate BP.RSTR by ourselves, because we can NOT access hardware register.
  //

  AcmStatus = MmioRead32 (MMIO_ACM_STATUS);
  SacmInfo  = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);

  BpRstr.Bits.FACB = (UINT8)((SacmInfo & BIT4) >> 4);
  BpRstr.Bits.DCD = (UINT8)((AcmStatus & BIT21) >> 21);
  BpRstr.Bits.DBI = (UINT8)((AcmStatus & BIT22) >> 22);
  BpRstr.Bits.PBE = (UINT8)((AcmStatus & BIT23) >> 23);
  BpRstr.Bits.BBP = (UINT8)((AcmStatus & BIT24) >> 24);
  BpRstr.Bits.Rsvd = 0x2;
  BpRstr.Bits.BPInvd = 0;

  return BpRstr.Data;
}

/**
  Get BP.TYPE low byte

  @return BP.TYPE low byte
**/
UINT8
GetBPTYPELow (
  VOID
  )
{
  BP_TYPE_LOW   BpType;
  UINT32        AcmStatus;
  UINT64        SacmInfo;

  //
  // Calculate BP.TYPE by ourselves, because we can NOT access hardware register.
  //

  AcmStatus = MmioRead32 (MMIO_ACM_STATUS);
  SacmInfo  = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);

  BpType.Bits.MeasuredBoot = (UINT8)((SacmInfo & BIT5) >> 5);
  BpType.Bits.VerifiedBoot = (UINT8)((SacmInfo & BIT6) >> 6);
  BpType.Bits.HAP = (UINT8)((AcmStatus & BIT20) >> 20);
  BpType.Bits.Rsvd = 0;

  return BpType.Data;
}

/**
  Calculate SHA1 Hash

  @param[in]  Data   data
  @param[in]  Size   data size
  @param[out] Digest SHA1 digest
**/
VOID
CreateSha1Hash (
  IN  UINT8     *Data,
  IN  UINTN     Size,
  OUT UINT8     *Digest
  )
{
  UINTN       CtxSize;
  VOID        *HashCtx;

  CtxSize = Sha1GetContextSize ();
  HashCtx = AllocatePool (CtxSize);
  ASSERT (HashCtx != NULL);
  Sha1Init (HashCtx);
  Sha1Update (HashCtx, Data, Size);
  Sha1Final (HashCtx, Digest);
  FreePool (HashCtx);

  DEBUG((DEBUG_INFO, "Sha1 Hash  - \n"));
  InternalDumpHex((UINT8 *)(Digest), 20);

  return ;
}

/**
  Calculate SHA256 Hash

  @param[in]  Data   data
  @param[in]  Size   data size
  @param[out] Digest SHA256 digest
**/
VOID
CreateSha256Hash (
  IN  UINT8     *Data,
  IN  UINTN     Size,
  OUT UINT8     *Digest
  )
{
  UINTN       CtxSize;
  VOID        *HashCtx;

  CtxSize = Sha256GetContextSize ();
  HashCtx = AllocatePool (CtxSize);
  ASSERT (HashCtx != NULL);
  Sha256Init (HashCtx);
  Sha256Update (HashCtx, Data, Size);
  Sha256Final (HashCtx, Digest);
  FreePool (HashCtx);

  DEBUG((DEBUG_INFO, "Sha256 Hash  - \n"));
  InternalDumpHex((UINT8 *)(Digest), 32);
  return ;
}

/**
  Calculate IBB Hash

  @param[in]  BpmIbb BPM IBB element
  @param[out] Digest IBB digest
**/
VOID
CreateIbbHash (
  IN  IBB_ELEMENT *BpmIbb,
  OUT UINT8       *Digest
  )
{
  UINTN       CtxSize;
  VOID        *HashCtx;
  UINTN       Index;

  CtxSize = Sha256GetContextSize ();
  HashCtx = AllocatePool (CtxSize);
  ASSERT (HashCtx != NULL);
  Sha256Init (HashCtx);

  for (Index = 0; Index < BpmIbb->SegmentCount; Index++) {
    if (BpmIbb->IBBSegment[Index].Flags == IBB_SEGMENT_FLAG_IBB) {
      Sha256Update (HashCtx, (VOID *)(UINTN)BpmIbb->IBBSegment[Index].Base, BpmIbb->IBBSegment[Index].Size);
    }
  }
  Sha256Final (HashCtx, Digest);
  FreePool (HashCtx);

  return ;
}

/**
  Calculate DetailPCR extend value

  @param[out] SHA1   DetailPCR digest
  @param[out] SHA256 DetailPCR digest
**/
VOID
CalculateDetailPCRExtendValue (
  OUT UINT8 *Sha1Digest,
  OUT UINT8 *Sha256Digest
  )
{
  ACM_FORMAT                               *Acm;
  KEY_MANIFEST_STRAUCTURE                  *Km;
  BOOT_POLICY_MANIFEST_HEADER              *Bpm;
  IBB_ELEMENT                              *BpmIbb;
  BOOT_POLICY_MANIFEST_SIGNATURE_ELEMENT   *BpmSignature;
  DETAIL_PCR_DATA                          DetailPcrData;

  Acm = FindAcm ();
  ASSERT (Acm != NULL);
  if (Acm == NULL) return;

  Km = FindKm ();
  ASSERT (Km != NULL);
  if (Km == NULL) return;

  Bpm = FindBpm ();
  ASSERT (Bpm != NULL);
  if (Bpm == NULL) return;

  BpmIbb = FindBpmIbb (Bpm);
  ASSERT (BpmIbb != NULL);
  if (BpmIbb == NULL) return;

  BpmSignature = FindBpmSignature (Bpm);
  ASSERT (BpmSignature != NULL);
  if (BpmSignature == NULL) return;

  DetailPcrData.BP_RSTR_Low = GetBPRSTRLow ();
  DetailPcrData.BP_TYPE_Low = GetBPTYPELow ();
  DetailPcrData.ACM_SVN = Acm->ACM_SVN;
  CopyMem (&DetailPcrData.AcmRsaSignature, &Acm->RSASig, sizeof(DetailPcrData.AcmRsaSignature));
  CopyMem (&DetailPcrData.KmRsaSignature, &Km->KeyManifestSignature.Signature.Signature, sizeof(DetailPcrData.KmRsaSignature));
  CopyMem (&DetailPcrData.BpmRsaSignature, &BpmSignature->KeySignature.Signature.Signature, sizeof(DetailPcrData.BpmRsaSignature));
  if (IsVerifiedBoot ()) {
    CopyMem (&DetailPcrData.IbbHash, &BpmIbb->Digest.HashBuffer, sizeof(DetailPcrData.IbbHash));
  } else {
    //
    // Calculate IBB hash by ourselves, because it is NOT verified boot, we can not trust BpmIbb->Digest.
    //
    CreateIbbHash (BpmIbb, (UINT8 *)&DetailPcrData.IbbHash);
  }

  DumpDetailPcr (&DetailPcrData);

  CreateSha1Hash ((UINT8 *)&DetailPcrData, sizeof(DetailPcrData), (UINT8 *)Sha1Digest);
  CreateSha256Hash ((UINT8 *)&DetailPcrData, sizeof(DetailPcrData), (UINT8 *)Sha256Digest);
}

/**
  Calculate AuthorityPCR extend value

  @param[out] SHA1   Authority PCR digest
  @param[out] SHA256 Authority PCR digest
**/
VOID
CalculateAuthorityPCRExtendValue (
  OUT UINT8 *Sha1Digest,
  OUT UINT8 *Sha256Digest
  )
{
  ACM_FORMAT                               *Acm;
  KEY_MANIFEST_STRAUCTURE                  *Km;
  AUTHORITY_PCR_DATA                       AuthorityPcrData;

  Acm = FindAcm ();
  ASSERT (Acm != NULL);
  if (Acm == NULL) return;

  Km = FindKm ();
  ASSERT (Km != NULL);
  if (Km == NULL) return;

  AuthorityPcrData.BP_RSTR_Low = GetBPRSTRLow ();
  AuthorityPcrData.BP_TYPE_Low = GetBPTYPELow ();
  AuthorityPcrData.ACM_SVN = Acm->ACM_SVN;

  //
  // Calculate ACM Key hash by ourselves, or access hardware register?
  //
  if (1) {
    *(UINT64 *)&AuthorityPcrData.AcmKeyHash[0]  = MmioRead64 (ACM_KEY_HASH_MMIO_ADDR_0);
    *(UINT64 *)&AuthorityPcrData.AcmKeyHash[8]  = MmioRead64 (ACM_KEY_HASH_MMIO_ADDR_1);
    *(UINT64 *)&AuthorityPcrData.AcmKeyHash[16] = MmioRead64 (ACM_KEY_HASH_MMIO_ADDR_2);
    *(UINT64 *)&AuthorityPcrData.AcmKeyHash[24] = MmioRead64 (ACM_KEY_HASH_MMIO_ADDR_3);
  } else {
    CreateSha256Hash ((UINT8 *)&Acm->RSAPubKey, sizeof(Acm->RSAPubKey), (UINT8 *)&AuthorityPcrData.AcmKeyHash);
  }
  //
  // Calculate BP Key hash by ourselves, because we can NOT access hardware register.
  //
  CreateSha256Hash ((UINT8 *)&Km->KeyManifestSignature.Key.Modulus, sizeof(Km->KeyManifestSignature.Key.Modulus), (UINT8 *)&AuthorityPcrData.BpKeyHash);

  CopyMem (&AuthorityPcrData.BpmKeyHashFromKm, &Km->BPKey.HashBuffer, sizeof(AuthorityPcrData.BpmKeyHashFromKm));

  DumpAuthorityPcr (&AuthorityPcrData);

  CreateSha1Hash ((UINT8 *)&AuthorityPcrData, sizeof(AuthorityPcrData), (UINT8 *)Sha1Digest);
  CreateSha256Hash ((UINT8 *)&AuthorityPcrData, sizeof(AuthorityPcrData), (UINT8 *)Sha256Digest);
}

/**
  Check if we need AuthorityPCR measurement

  @retval TRUE  Need AuthorityPCR measurement
  @retval FALSE Do NOT need AuthorityPCR measurement
**/
BOOLEAN
NeedAuthorityMeasure (
  VOID
  )
{
  BOOT_POLICY_MANIFEST_HEADER              *Bpm;
  IBB_ELEMENT                              *BpmIbb;

  Bpm = FindBpm ();
  ASSERT (Bpm != NULL);
  if (Bpm == NULL) return FALSE;

  BpmIbb = FindBpmIbb (Bpm);
  ASSERT (BpmIbb != NULL);
  if (BpmIbb == NULL) return FALSE;

  if ((BpmIbb->Flags & IBB_FLAG_AUTHORITY_MEASURE) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Check if we need to add event for locality 3 startup

  @retval TRUE  Need Locality Event
  @retval FALSE Do NOT need Locality Event
**/
BOOLEAN
NeedLocalityEvent (
  VOID
  )
{
  BOOT_POLICY_MANIFEST_HEADER              *Bpm;
  IBB_ELEMENT                              *BpmIbb;

  Bpm = FindBpm ();
  ASSERT (Bpm != NULL);
  if (Bpm == NULL) return FALSE;

  BpmIbb = FindBpmIbb (Bpm);
  ASSERT (BpmIbb != NULL);
  if (BpmIbb == NULL) return FALSE;
  DEBUG ((EFI_D_INFO, "BpmIbb->Flags %x\n", BpmIbb->Flags));

  if ((BpmIbb->Flags & IBB_FLAG_INITIAL_MEASURE_LOC3) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Create DetailPCR event log

  @param[in] TpmType        TPM type
  @param[in] ActivePcrBanks Active PCR Banks
**/
VOID
CreateDetailPCREvent (
  IN UINT8              TpmType,
  IN UINT32             ActivePcrBanks
  )
{
  TCG_PCR_EVENT_HDR         NewEventHdr;
  UINT8                     Sha1[SHA1_DIGEST_SIZE];
  UINT8                     Sha256[SHA256_DIGEST_SIZE];
  TPML_DIGEST_VALUES        DigestList;

  NewEventHdr.PCRIndex  = 0;
  NewEventHdr.EventType = EV_S_CRTM_CONTENTS;
  CalculateDetailPCRExtendValue (Sha1, Sha256);

  ZeroMem (&DigestList, sizeof(DigestList));
  if ((ActivePcrBanks & TREE_BOOT_HASH_ALG_SHA1) != 0) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA1;
    CopyMem (DigestList.digests[DigestList.count].digest.sha1, Sha1, SHA1_DIGEST_SIZE);
    DigestList.count ++;
  }
  if ((ActivePcrBanks & TREE_BOOT_HASH_ALG_SHA256) != 0) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA256;
    CopyMem (DigestList.digests[DigestList.count].digest.sha256, Sha256, SHA256_DIGEST_SIZE);
    DigestList.count ++;
  }

  if (IsNpwAcm()) {
    NewEventHdr.EventSize = sizeof ("Boot Guard Debug Measured S-CRTM");
    LogHashEvent (&DigestList, &NewEventHdr, (UINT8 *) "Boot Guard Debug Measured S-CRTM");   // RPPO-KBL-0043
  } else {
    NewEventHdr.EventSize = sizeof ("Boot Guard Measured S-CRTM");
    LogHashEvent (&DigestList, &NewEventHdr, (UINT8 *) "Boot Guard Measured S-CRTM");   // RPPO-KBL-0043
  }
}

/**
  Create AuthorityPCR event log

  @param[in] TpmType        TPM type
  @param[in] ActivePcrBanks Active PCR Banks
**/
VOID
CreateAuthorityPCREvent (
  IN UINT8              TpmType,
  IN UINT32             ActivePcrBanks
  )
{
  TCG_PCR_EVENT_HDR         NewEventHdr;
  UINT8                     Sha1[SHA1_DIGEST_SIZE];
  UINT8                     Sha256[SHA256_DIGEST_SIZE];
  TPML_DIGEST_VALUES        DigestList;

  if (NeedAuthorityMeasure() && IsVerifiedBoot()) {
    if (TpmType == TPM_DEVICE_DTPM_1_2) {
      NewEventHdr.PCRIndex  = 6;
    } else {
      NewEventHdr.PCRIndex  = 7;
    }
    NewEventHdr.EventType = EV_EFI_VARIABLE_DRIVER_CONFIG;
    CalculateAuthorityPCRExtendValue (Sha1, Sha256);

    ZeroMem (&DigestList, sizeof(DigestList));
    if ((ActivePcrBanks & TREE_BOOT_HASH_ALG_SHA1) != 0) {
      DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA1;
      CopyMem (DigestList.digests[DigestList.count].digest.sha1, Sha1, SHA1_DIGEST_SIZE);
      DigestList.count ++;
    }
    if ((ActivePcrBanks & TREE_BOOT_HASH_ALG_SHA256) != 0) {
      DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA256;
      CopyMem (DigestList.digests[DigestList.count].digest.sha256, Sha256, SHA256_DIGEST_SIZE);
      DigestList.count ++;
    }

    if (IsNpwAcm()) {
      NewEventHdr.EventSize = sizeof (L"Boot Guard Debug Measured S-CRTM");
      LogHashEvent (&DigestList, &NewEventHdr, (UINT8 *)L"Boot Guard Debug Measured S-CRTM");
    } else {
      NewEventHdr.EventSize = sizeof (L"Boot Guard Measured S-CRTM");
      LogHashEvent (&DigestList, &NewEventHdr, (UINT8 *)L"Boot Guard Measured S-CRTM");
    }
  }
}

/**
  Create Locality Startup event entry

  @param[in] TpmType        TPM type
  @param[in] ActivePcrBanks Active PCR Banks
**/
VOID
CreateLocalityStartupEvent (
  IN UINT8              TpmType,
  IN UINT32             ActivePcrBanks
  )
{
  TCG_PCR_EVENT_HDR                 NewEventHdr;
  UINT8                            Sha1[SHA1_DIGEST_SIZE];
  UINT8                            Sha256[SHA256_DIGEST_SIZE];
  TPML_DIGEST_VALUES               DigestList;
  TCG_EFI_STARTUP_LOCALITY_EVENT   LocalityEventData;

  if (NeedLocalityEvent()) {
  ZeroMem (&Sha1, SHA1_DIGEST_SIZE);
  ZeroMem (&Sha256, SHA256_DIGEST_SIZE);

  NewEventHdr.PCRIndex  = 0;
  NewEventHdr.EventType = EV_NO_ACTION;

  ZeroMem (&DigestList, sizeof(DigestList));

  if ((ActivePcrBanks & TREE_BOOT_HASH_ALG_SHA1) != 0) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA1;
    CopyMem (DigestList.digests[DigestList.count].digest.sha1, Sha1, SHA1_DIGEST_SIZE);
    DigestList.count ++;
  }

  if ((ActivePcrBanks & TREE_BOOT_HASH_ALG_SHA256) != 0) {
    DigestList.digests[DigestList.count].hashAlg = TPM_ALG_SHA256;
    CopyMem (DigestList.digests[DigestList.count].digest.sha256, Sha256, SHA256_DIGEST_SIZE);
    DigestList.count ++;
  }

    NewEventHdr.EventSize = sizeof (TCG_EFI_STARTUP_LOCALITY_EVENT);

    CopyMem (LocalityEventData.Signature, (UINT8 *) "StartupLocality", sizeof ("StartupLocality"));
    LocalityEventData.StartupLocality = 0x03;

    LogHashEvent (&DigestList, &NewEventHdr,(UINT8 *) &LocalityEventData);

  }
}


/**
  Get Active PCR Banks.

  @retval UINT32  Number of PCR Banks.
**/

UINT32
GetActivePcrBanks (
  VOID
  )
{
  EFI_STATUS           Status;
  UINT32               ActivePcrBanks;
  TPML_PCR_SELECTION   Pcrs;
  UINTN                Index;

  DEBUG ((EFI_D_ERROR, "GetActivePcrBank!\n"));

  Status = Tpm2GetCapabilityPcrs (&Pcrs);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2GetCapabilityPcrs fail!\n"));
    ActivePcrBanks = TREE_BOOT_HASH_ALG_SHA1;
  } else {
    DEBUG ((EFI_D_INFO, "Tpm2GetCapabilityPcrs Count - %08x\n", Pcrs.count));
    ActivePcrBanks = 0;
    for (Index = 0; Index < Pcrs.count; Index++) {
      DEBUG ((EFI_D_INFO, "hash - %x\n", Pcrs.pcrSelections[Index].hash));
      switch (Pcrs.pcrSelections[Index].hash) {
      case TPM_ALG_SHA1:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= TREE_BOOT_HASH_ALG_SHA1;
        }
        break;
      case TPM_ALG_SHA256:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= TREE_BOOT_HASH_ALG_SHA256;
        }
        break;
      case TPM_ALG_SHA384:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= TREE_BOOT_HASH_ALG_SHA384;
        }
        break;
      case TPM_ALG_SHA512:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= TREE_BOOT_HASH_ALG_SHA512;
        }
        break;
      case TPM_ALG_SM3_256:
        if (!IsZeroBuffer (Pcrs.pcrSelections[Index].pcrSelect, Pcrs.pcrSelections[Index].sizeofSelect)) {
          ActivePcrBanks |= EFI_TCG2_BOOT_HASH_ALG_SM3_256;
        }
        break;
      }
    }
  }
  return ActivePcrBanks;
}

/**
  Create Boot Guard TPM event log

  @param[in] TpmType TPM type
**/
VOID
CreateTpmEventLog (
  IN UINT8              TpmType
  )
{
  UINT32               ActivePcrBanks;
  EFI_STATUS           Status;
  //
  // Initialize TPM device
  //
  Status = Tpm2RequestUseTpm ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "TPM2 not detected!\n"));
  }

  ActivePcrBanks = GetActivePcrBanks ();

  if (IsMeasuredBoot()) {
    CreateLocalityStartupEvent (TpmType, ActivePcrBanks);
    CreateDetailPCREvent (TpmType, ActivePcrBanks);
    CreateAuthorityPCREvent (TpmType, ActivePcrBanks);
  }
}

/**
  If Boot Guard already measured IBB, we do not need let TPM measure it again.
  So we need register this PPI to let TPM driver know this Fv should be
  excluded. This data should be consistent as the data in BiosInfo driver.
**/
VOID
ExcludeIbbFv (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "Excluding FV recovery as Boot Guard did Measurement of PEI FV\n"));
  PeiServicesInstallPpi (&mPeiFirmwareVolumeInfoMeasurementExcludedPpiList);
}

/**
  Create various events in the event log as required for Bootguard Enabled Boot

  @retval EFI_SUCCES             Events created successfully
  @retval Others                 Bootguard not supported
**/
EFI_STATUS
EFIAPI
CreateBootguardEventLogEntries (
  VOID
  )
{
  EFI_STATUS                      Status;
  BOOT_GUARD_INFO                 BootGuardInfo;
  UINT32                          MeFwSts4;

  Status = EFI_SUCCESS;
  ZeroMem( &BootGuardInfo, sizeof(BOOT_GUARD_INFO));

  GetBootGuardInfo(&BootGuardInfo);
  if (BootGuardInfo.BootGuardCapability == TRUE) {
    //
    // Skip TPM event log if TPM 1.2 device is deactivated.
    //
    MeFwSts4 = MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_ME_HFS_4);
    if ((MeFwSts4 & B_TPM1_2_DEACTIVATED) == 0) {
      //
      // Create TPM event log if not Sx Resume Type - S3/Deep-S3/FFS Resume
      // Note: S4->S0 is similar to S5->S0
      //
      if(BootGuardInfo.ByPassTpmEventLog != TRUE) {
        DEBUG ((DEBUG_INFO, "BIOS is Creating Event Log for ACM measurements\n"));
        CreateTpmEventLog (BootGuardInfo.TpmType);
        //
        // If Boot Guard already measured IBB, we do not need let TPM measure it again.
        //
        if (IsMeasuredBoot()) {
          ExcludeIbbFv();
        }
      }
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
