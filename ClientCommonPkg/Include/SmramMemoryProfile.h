/** @file
 SMRAM memory profile data structure

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

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

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be 
  modified, except as allowed by additional terms of your license agreement.

@par Specification Reference:

**/

#ifndef _SMRAM_MEMORY_PROFILE_H_
#define _SMRAM_MEMORY_PROFILE_H_

//
// ROYAL_PARK_OVERRIDE: RoyalParkOverrideBegin - RPCO-0025
//

#pragma pack(1)

//
// SMRAM profile command
//
#define SMRAM_PROFILE_COMMAND_GET_PROFILE_SIZE         0x1
#define SMRAM_PROFILE_COMMAND_GET_PROFILE_DATA         0x2
//
// Below 2 commands are used by ECP only and only valid before SmmReadyToLock
//
#define SMRAM_PROFILE_COMMAND_REGISTER_IMAGE           0x3
#define SMRAM_PROFILE_COMMAND_UNREGISTER_IMAGE         0x4

typedef struct {
  UINT32                         Command;
  UINT64                         ReturnStatus;
} SMRAM_PROFILE_PARAMETER_HEADER;

typedef struct {
  SMRAM_PROFILE_PARAMETER_HEADER  Header;
  UINT64                          ProfileSize;
} SMRAM_PROFILE_PARAMETER_GET_PROFILE_SIZE;

//
// SMRAM profile layout:
// +---------------------------+
// | CONTEXT                   |
// +---------------------------+
// | DRIVER_INFO(1)            |
// +---------------------------+
// | ALLOC_INFO(1, 1)          |
// +---------------------------+
// | ALLOC_INFO(1, m1)         |
// +---------------------------+
// | DRIVER_INFO(n)            |
// +---------------------------+
// | ALLOC_INFO(n, 1)          |
// +---------------------------+
// | ALLOC_INFO(n, mn)         |
// +---------------------------+
// | FREE_MEMORY_DESCRIPTOR(1) |
// +---------------------------+
// | FREE_MEMORY_DESCRIPTOR(p) |
// +---------------------------+
// | EFI_SMRAM_DESCRIPTOR(1)   |
// +---------------------------+
// | EFI_SMRAM_DESCRIPTOR(q)   |
// +---------------------------+
//
typedef struct {
  SMRAM_PROFILE_PARAMETER_HEADER  Header;
  UINT64                          ProfileSize;
  PHYSICAL_ADDRESS                ProfileBuffer;
} SMRAM_PROFILE_PARAMETER_GET_PROFILE_DATA;

typedef struct {
  SMRAM_PROFILE_PARAMETER_HEADER  Header;
  EFI_GUID                        FileName;
  PHYSICAL_ADDRESS                ImageBuffer;
  UINT64                          NumberOfPage;
} SMRAM_PROFILE_PARAMETER_REGISTER_IMAGE;

typedef struct {
  SMRAM_PROFILE_PARAMETER_HEADER  Header;
  EFI_GUID                        FileName;
  PHYSICAL_ADDRESS                ImageBuffer;
  UINT64                          NumberOfPage;
} SMRAM_PROFILE_PARAMETER_UNREGISTER_IMAGE;

typedef struct {
  UINT64                   CurrentTotalUsage;
  UINT64                   PeakTotalUsage;
  UINT64                   TotalImageSize;
  UINTN                    ImageCount;
  UINT64                   FreeMemoryPages;
  UINTN                    FreeMemoryEntryCount;
  UINTN                    SmramRangeCount;
  UINTN                    SequenceCount;
} SMRAM_PROFILE_CONTEXT;

typedef struct {
  EFI_GUID                 FileName;
  PHYSICAL_ADDRESS         ImageBuffer;
  UINT64                   ImageSize;
  UINT64                   CurrentUsage;
  UINT64                   PeakUsage;
  UINTN                    AllocRecordCount;
} SMRAM_PROFILE_DRIVER_INFO;

typedef enum {
  SmramActionAllocatePages = 1,
  SmramActionFreePages = 2,
  SmramActionAllocatePool = 3,
  SmramActionFreePool = 4,
} SMRAM_PROFILE_ACTION;

typedef struct {
  EFI_PHYSICAL_ADDRESS     CallerAddress;
  UINT64                   SequenceId;
  SMRAM_PROFILE_ACTION     Action;
  EFI_MEMORY_TYPE          MemoryType;
  EFI_PHYSICAL_ADDRESS     Buffer;
  UINT64                   Size;
} SMRAM_PROFILE_ALLOC_INFO;

typedef struct {
  EFI_PHYSICAL_ADDRESS  Address;
  UINT64                Size;
} SMRAM_FREE_MEMORY_DESCRIPTOR;

#pragma pack()

extern EFI_GUID  gSmramProfileGuid;

//
// ROYAL_PARK_OVERRIDE: RoyalParkOverrideEnd
//

#endif // _SMRAM_MEMORY_PROFILE_H_
