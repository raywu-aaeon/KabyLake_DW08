/** @file
  Source code file for board configuration Pcd Init Pre-Memory PEI module

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <PlatformInfo.h>
#include <Board/BoardInitTable.h>
#include <Board/BoardConfigInit.h>

/**
  Init the Board Init PCD Table List.

  @param  TableList               Pointer to array of the Board Init Table.

  @retval  EFI_SUCCESS            Table initialization successfully.
**/
EFI_STATUS
InitBoardInitTableList (
  VOID
)
{
  VOID *Pointer;

  Pointer = (VOID *) (UINTN) PcdGet32 (PcdPeiBoardInitTableList);
  if (Pointer == NULL) {
    Pointer = (VOID *) AllocateZeroPool (sizeof (VOID *) * TABLE_LIST_NUM);
    if (Pointer == NULL) {
      DEBUG ((DEBUG_ERROR, "Can't allocate enough memory for PeiBoardInitTableList\n"));
      return EFI_OUT_OF_RESOURCES;
    }
    PcdSet32S (PcdPeiBoardInitTableList, (UINTN) Pointer);
  }

  return EFI_SUCCESS;
}

/**
  Scan the array of the Board Init Table list to find a free member.

  @param  Table                 Pointer to the array of the Board Init Table pointers.
  @param  FreeSlot              The free slot for new Board Init Table list..

  @retval EFI_SUCCESS           Table initialization successfully.
          EFI_OUT_OF_RESOURCE   All slots are occupied without free one.
**/
EFI_STATUS
GetFreeTableSlot (
  IN VOID **Table,
  OUT UINT8 *FreeSlot
  )
{
  UINT8 Slot;

  for (Slot = DEFAULT_INIT_SLOT_POSTMEM + 1; Slot <  TABLE_LIST_NUM; Slot ++) {
    if (Table [Slot] == NULL) {
      break;
    }
  }

  if (Slot == TABLE_LIST_NUM) {
    DEBUG ((DEBUG_ERROR, "Can't Found free slot in Board Init Table List\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  *FreeSlot = Slot;
  return EFI_SUCCESS;
}

/**
  Common Board Lib contructor.

  @param  BoardInitTable        Pointer to the array of the Board Init Table.

  @retval EFI_SUCCESS           Table initialization successfully.
          EFI_OUT_OF_RESOURCE   All slots are occupied without free one.
**/
EFI_STATUS
CommonInitInternalLibConstructor (
  IN BOARD_INIT_TABLE *BoardInitTable,
  IN UINTN FixedSlot
  )
{
  EFI_STATUS Status;
  VOID **TablePtr;
  UINT8 Slot;

  Status = InitBoardInitTableList ();

  if (EFI_ERROR (Status)) {
    return Status;
  }

  TablePtr = (VOID **) (UINTN) PcdGet32 (PcdPeiBoardInitTableList);
  if (TablePtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (FixedSlot >= TABLE_LIST_NUM) {
    Slot = 0;
    Status = GetFreeTableSlot (TablePtr, &Slot);

    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    Slot = (UINT8) FixedSlot;
  }

  TablePtr [Slot] = (VOID *) BoardInitTable;

  return EFI_SUCCESS;
}

EFI_STATUS
GetBoardId (
  OUT UINT16 *BoardId,
  OUT UINT16 *BoardIdOrgValue
  )
{
  BOARD_PRIVATE_DATA *BoardInfo;

  BoardInfo = (BOARD_PRIVATE_DATA *) PcdGetPtr (PcdBoardPrivateData);

  *BoardId = BoardInfo->BoardId;
  *BoardIdOrgValue = BoardInfo->BoardIdOrgValue;

  return EFI_SUCCESS;
}
