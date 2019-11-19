/** @file
  SMM driver to manage a block of memory used to store confidential data.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2019 Intel Corporation.

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

**/
#include <PiSmm.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Protocol/SmmConfidentialMem.h>

EFI_STATUS
EFIAPI
SmmAllocateConfPool (
  IN      UINTN   Size,
  IN OUT  VOID**  Buffer
  );

EFI_STATUS
EFIAPI
SmmFreeConfPool (
  IN OUT  VOID** Buffer
  );

#define NUMBER_OF_CONF_ENTRIES  128
#define CACHE_LINE_SIZE         64
#define CACHE_LINE_MASK         (~((UINTN) (CACHE_LINE_SIZE - 1)))
#define START_PAD_SIZE          8
#define END_PAD_SIZE            0
#define MAX_CONF_SIZE (CACHE_LINE_SIZE - START_PAD_SIZE - END_PAD_SIZE)

UINT8*  mUsedEntryList = NULL;
UINT8*  mConfMem = NULL;

EDKII_SMM_CONF_MEM_PROTOCOL mSmmConfMemProt = {
  SmmAllocateConfPool,
  SmmFreeConfPool
};

/**
  SmmAllocateConfPool -         Allocates SMM memory for confidential storage

  @param Size                   Requested size in bytes
  @param Buffer                 Buffer address

  @retval EFI_SUCCESS           Requested buffer was allocated successfully.
  @retval EFI_NOT_STARTED       Requested buffer could not be allocated.
  @retval EFI_INVALID_PARAMETER Requested buffer could not be allocated.
  @retval EFI_OUT_OF_RESOURCES  Requested buffer could not be allocated.

**/
EFI_STATUS
EFIAPI
SmmAllocateConfPool (
  IN      UINTN   Size,
  IN OUT  VOID**  Buffer
  )
{
  UINT32  Index;

  DEBUG ((EFI_D_INFO, "%a() - Allocate size %d\n", __FUNCTION__, Size));

  if (mUsedEntryList == NULL || mConfMem == NULL) {
    DEBUG ((EFI_D_ERROR, "%a() - Memory initialization failed\n", __FUNCTION__));
    return EFI_NOT_STARTED;
  }
  if (Size == 0 || Size > MAX_CONF_SIZE || Buffer == NULL) {
    DEBUG ((EFI_D_ERROR, "%a() - Invalid parameter\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  // Find next available entry and mark entry as in use
  *Buffer = NULL;
  for (Index = 0; Index < NUMBER_OF_CONF_ENTRIES; Index++) {
    if (mUsedEntryList[Index] == 0) {
      mUsedEntryList[Index] = 1;
      break;
    }
  }
  if (Index == NUMBER_OF_CONF_ENTRIES) {
    DEBUG ((EFI_D_ERROR, "%a() - No more pool entries available\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Compute the address of the entry
  *Buffer = mConfMem + (CACHE_LINE_SIZE * Index) + START_PAD_SIZE;

  // Clear confidential buffer data region
  ZeroMem (*Buffer, MAX_CONF_SIZE);
  DEBUG ((EFI_D_INFO, "%a() - Allocate index %d\n", __FUNCTION__, Index));

  return EFI_SUCCESS;
}

/**
  SmmFreeConfPool -             Frees previously allocated SMM memory

  @param Buffer                 Buffer address

  @retval EFI_SUCCESS           Buffer was freed successfully.
  @retval EFI_NOT_STARTED       Buffer could not be freed.
  @retval EFI_INVALID_PARAMETER Buffer could not be freed.
  @retval EFI_NOT_FOUND         Buffer could not be freed.

**/
EFI_STATUS
EFIAPI
SmmFreeConfPool (
  IN OUT  VOID** Buffer
  )
{
  UINTN TmpMemBase;
  UINTN TmpConfBase;
  UINTN Index;

  if (mUsedEntryList == NULL || mConfMem == NULL) {
    DEBUG ((EFI_D_ERROR, "%a() - Memory initialization failed\n", __FUNCTION__));
    return EFI_NOT_STARTED;
  }
  if (Buffer == NULL || *Buffer == NULL) {
    DEBUG ((EFI_D_ERROR, "%a() - Invalid parameter\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  // Determine if this buffer is in the confidential memory region
  TmpConfBase = (UINTN) mConfMem;
  TmpMemBase = (UINTN) *Buffer;
  TmpMemBase &= CACHE_LINE_MASK;
  if ((TmpMemBase < TmpConfBase) || (TmpMemBase > (TmpConfBase + ((NUMBER_OF_CONF_ENTRIES - 1) * CACHE_LINE_SIZE)))) {
    DEBUG ((EFI_D_ERROR, "%a() - Invalid address\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  // Clear the memory
  ZeroMem (*Buffer, MAX_CONF_SIZE);

  // Determine the usage entry and mark it as free
  Index = (TmpMemBase - TmpConfBase) / CACHE_LINE_SIZE;
  mUsedEntryList[Index] = 0;
  DEBUG ((EFI_D_INFO, "%a() - Free index %d\n", __FUNCTION__, Index));

  // Now that this memory is free set the source pointer to NULL
  *Buffer = NULL;

  return EFI_SUCCESS;
}

/**
  SmmConfMemEntryPoint -        Allocates SMM pages to be used for confidential storage

  @param ImageHandle
  @param SystemTable

  @retval EFI_SUCCESS           Buffer was freed successfully.
  @retval EFI_OUT_OF_RESOURCES  Buffer could not be allocated.

**/
EFI_STATUS
EFIAPI
SmmConfMemEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_HANDLE Handle;
  EFI_PHYSICAL_ADDRESS SmmData;

  SmmData = 0;
  Status = gSmst->SmmAllocatePages (
    AllocateAnyPages,
    EfiRuntimeServicesData,
    EFI_SIZE_TO_PAGES(NUMBER_OF_CONF_ENTRIES * CACHE_LINE_SIZE),
    &SmmData
    );
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "%a() - Unable to allocate confidential memory pages\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }
  mConfMem = (UINT8*)(UINTN) SmmData;
  SetMem (mConfMem, NUMBER_OF_CONF_ENTRIES * CACHE_LINE_SIZE, 0xFF);
  DEBUG ((EFI_D_INFO, "%a() - Confidential Memory Base: 0x%p\n", __FUNCTION__, mConfMem));

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, NUMBER_OF_CONF_ENTRIES, (void**)&mUsedEntryList);
  if (EFI_ERROR(Status) || mUsedEntryList == NULL) {
    DEBUG ((EFI_D_ERROR, "%a() - Unable to allocate confidential memory usage buffer\n", __FUNCTION__));
    gSmst->SmmFreePages (SmmData, EFI_SIZE_TO_PAGES(NUMBER_OF_CONF_ENTRIES * CACHE_LINE_SIZE));
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem(mUsedEntryList, NUMBER_OF_CONF_ENTRIES);

  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
    &Handle,
    &gEdkiiSmmConfMemProtocolGuid,
    EFI_NATIVE_INTERFACE,
    &mSmmConfMemProt
    );
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "%a() - Failed to install SMM protocol for confidential memory\n", __FUNCTION__));
    gSmst->SmmFreePages (SmmData, EFI_SIZE_TO_PAGES(NUMBER_OF_CONF_ENTRIES * CACHE_LINE_SIZE));
    gSmst->SmmFreePool (mUsedEntryList);
    return Status;
  }

  return EFI_SUCCESS;
}