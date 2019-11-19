/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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

@par Specification Reference:
**/
// AMI_OVERRIDE_START - No use in AMI code base.
#if 0

#ifndef _FLASH_AREA_H_
#define _FLASH_AREA_H_

#include <Library/HobLib.h>
//
// Definition for flash map GUIDed HOBs
//
typedef UINT32  EFI_FLASH_AREA_ATTRIBUTES;

#define EFI_FLASH_AREA_FV           0x0001
#define EFI_FLASH_AREA_SUBFV        0x0002
#define EFI_FLASH_AREA_MEMMAPPED_FV 0x0004
#define EFI_FLASH_AREA_REQUIRED     0x0008
#define EFI_FLASH_AREA_CORRUPT      0x0010

typedef UINT8   EFI_FLASH_AREA_TYPE;

#define EFI_FLASH_AREA_RECOVERY_BIOS  0x0   // Recovery code
#define EFI_FLASH_AREA_MAIN_BIOS      0x1   // Regular BIOS code
#define EFI_FLASH_AREA_PAL_B          0x2   // PAL-B
#define EFI_FLASH_AREA_RESERVED_03    0x3   // Reserved for backwards compatibility
#define EFI_FLASH_AREA_RESERVED_04    0x4   // Reserved for backwards compatibility
#define EFI_FLASH_AREA_DMI_FRU        0x5   // DMI FRU information
#define EFI_FLASH_AREA_OEM_BINARY     0x6   // OEM Binary Code/data
#define EFI_FLASH_AREA_RESERVED_07    0x7   // Reserved for backwards compatibility
#define EFI_FLASH_AREA_RESERVED_08    0x8   // Reserved for backwards compatibility
#define EFI_FLASH_AREA_RESERVED_09    0x9   // Reserved for backwards compatibility
#define EFI_FLASH_AREA_RESERVED_0A    0x0a  // Reserved for backwards compatibility
#define EFI_FLASH_AREA_EFI_VARIABLES  0x0b  // EFI variables
#define EFI_FLASH_AREA_MCA_LOG        0x0c  // MCA error log
#define EFI_FLASH_AREA_SMBIOS_LOG     0x0d  // SMBIOS error log
#define EFI_FLASH_AREA_FTW_BACKUP     0x0e  // A backup block during FTW operations
#define EFI_FLASH_AREA_FTW_STATE      0x0f  // State information during FTW operations
#define EFI_FLASH_AREA_UNUSED         0x0fd // Not used
#define EFI_FLASH_AREA_GUID_DEFINED   0x0fe // Usage defined by a GUID

#pragma pack(1)
//
// An individual sub-area Entry.
// A single flash area may consist of  more than one sub-area.
//
typedef struct {
  EFI_FLASH_AREA_ATTRIBUTES Attributes;
  UINT32                    Reserved;
  EFI_PHYSICAL_ADDRESS      Base;
  EFI_PHYSICAL_ADDRESS      Length;
  EFI_GUID                  FileSystem;
} EFI_FLASH_SUBAREA_ENTRY;

typedef struct {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  UINT8                   Reserved[3];
  EFI_FLASH_AREA_TYPE     AreaType;
  EFI_GUID                AreaTypeGuid;
  UINT32                  NumEntries;
  EFI_FLASH_SUBAREA_ENTRY Entries[1];
} EFI_HOB_FLASH_MAP_ENTRY_TYPE;

//
// Internal definitions
//
typedef struct {
  UINT8                   Reserved[3];
  EFI_FLASH_AREA_TYPE     AreaType;
  EFI_GUID                AreaTypeGuid;
  UINT32                  NumberOfEntries;
  EFI_FLASH_SUBAREA_ENTRY SubAreaData;
} EFI_FLASH_AREA_HOB_DATA;

typedef struct {
  UINTN                     Base;
  UINTN                     Length;
  EFI_FLASH_AREA_ATTRIBUTES Attributes;
  EFI_FLASH_AREA_TYPE       AreaType;
  UINT8                     Reserved[3];
  EFI_GUID                  AreaTypeGuid;
} EFI_FLASH_AREA_DATA;

#pragma pack()

#define EFI_FLASH_AREA_DATA_DEFINITION \
  /* FVMAIN2 region */\
  {\
    (UINTN) FixedPcdGet32 (PcdFlashFvMain2Base),\
    (UINTN) FixedPcdGet32 (PcdFlashFvMain2Size),\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_MAIN_BIOS,\
    { 0, 0, 0 },\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
  },\
  /* FVMAIN region */\
  {\
    (UINTN) FixedPcdGet32 (PcdFlashFvMainBase),\
    (UINTN) FixedPcdGet32 (PcdFlashFvMainSize),\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_MAIN_BIOS,\
    { 0, 0, 0 },\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
  },\
  /* NVSTORAGE region */\
  {\
    (UINTN) FixedPcdGet32 (PcdFlashNvStorageVariableBase),\
    (UINTN) (FixedPcdGet32 (PcdFlashNvStorageVariableSize) + FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize) + FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize)),\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_GUID_DEFINED,\
    { 0, 0, 0 },\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
  },\
  /* BIOSGUARD_MODULE region */\
  {\
    (UINTN) FixedPcdGet32 (PcdFlashBiosGuardModuleFvBase),\
    (UINTN) FixedPcdGet32 (PcdFlashBiosGuardModuleFvSize),\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_GUID_DEFINED,\
    { 0, 0, 0 },\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
  },\
  /* MICROCODE region */\
  {\
    (UINTN) FixedPcdGet32 (PcdFlashMicrocodeFvBase),\
    (UINTN) FixedPcdGet32 (PcdFlashMicrocodeFvSize),\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_MAIN_BIOS,\
    { 0, 0, 0 },\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
  },\
  /* FV_RECOVERY3 region */\
  {\
    (UINTN) FixedPcdGet32 (PcdFlashFvRecovery3Base),\
    (UINTN) FixedPcdGet32 (PcdFlashFvRecovery3Size),\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_RECOVERY_BIOS,\
    { 0, 0, 0 },\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
  },\
  /* FV_RECOVERY2 region */\
  {\
    (UINTN) FixedPcdGet32 (PcdFlashFvRecovery2Base),\
    (UINTN) FixedPcdGet32 (PcdFlashFvRecovery2Size),\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_RECOVERY_BIOS,\
    { 0, 0, 0 },\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
  },\
  /* FV_RECOVERY region */\
  {\
    (UINTN) FixedPcdGet32 (PcdFlashFvRecoveryBase),\
    (UINTN) FixedPcdGet32 (PcdFlashFvRecoverySize),\
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,\
    EFI_FLASH_AREA_RECOVERY_BIOS,\
    { 0, 0, 0 },\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
  },\


//
// EFI_HOB_FLASH_MAP_ENTRY_TYPE definition
//
#define EFI_HOB_FLASH_MAP_ENTRY_TYPE_DATA_DEFINITION \
  /* NVSTORAGE.NV_VARIABLE_STORE Subregion */\
  {\
    {\
      EFI_HOB_TYPE_GUID_EXTENSION,\
      sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE ),\
      0 \
    },\
    EFI_FLASH_MAP_HOB_GUID, \
    { 0, 0, 0 },\
    EFI_FLASH_AREA_EFI_VARIABLES,\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
    1,\
    {\
      {\
        EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,\
        0,\
        (UINTN) FixedPcdGet32 (PcdFlashNvStorageVariableBase),\
        (UINTN) FixedPcdGet32 (PcdFlashNvStorageVariableSize),\
        { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
      },\
    },\
  }, \
  /* NVSTORAGE.NV_FTW_WORKING Subregion */\
  {\
    {\
      EFI_HOB_TYPE_GUID_EXTENSION,\
      sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE ),\
      0,\
    },\
    EFI_FLASH_MAP_HOB_GUID, \
    { 0, 0, 0 },\
    EFI_FLASH_AREA_FTW_STATE,\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
    1,\
    {\
      {\
        EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,\
        0,\
        (UINTN) FixedPcdGet32 (PcdFlashNvStorageFtwWorkingBase),\
        (UINTN) FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize),\
        { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
      },\
    },\
  }, \
  /* NVSTORAGE.NV_FTW_SPARE Subregion */\
  {\
    {\
      EFI_HOB_TYPE_GUID_EXTENSION,\
      sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE ),\
      0,\
    },\
    EFI_FLASH_MAP_HOB_GUID, \
    { 0, 0, 0 },\
    EFI_FLASH_AREA_FTW_BACKUP,\
    { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
    1,\
    {\
      {\
        EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,\
        0,\
        (UINTN) FixedPcdGet32 (PcdFlashNvStorageFtwSpareBase),\
        (UINTN) FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize),\
        { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },\
      },\
    },\
  },


#endif
#endif
// AMI_OVERRIDE_END - No use in AMI code base.