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
#include <PlatformBoardId.h>
#include <PlatformInfo.h>
#include <Board/BoardInitTable.h>
#include <Board/BoardConfigInit.h>

EFI_STATUS
GetPlatformInfo (
  IN OUT PLATFORM_INFO **PlatformInfo
  );

/**
  Board Config Init dispatch routine.

  @param[in]  PlatformInfo         Pointer to PLATFORM_INFO data structure.

  @retval     EFI_SUCCESS          The function completes successfully.
  @retval     EFI_NOT_FOUND        The BoardInitTableList is not init properly.
**/
EFI_STATUS
BoardConfigInit (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  VOID **Table;
  BOARD_INIT_TABLE *BoardInitTable;
  PLATFORM_INFO *PlatformInfo;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;
  UINT8 Slot;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  if (EFI_ERROR (Status)) {
    BoardId = COMMON_BOARD_ID;
  }

  Table = (VOID *) (UINTN) PcdGet64 (PcdDxeBoardInitTableList);

  Status = EFI_SUCCESS;

  if (Table == NULL) {
    return Status;
  }

  //
  // Scan and Dispatch Board Init function if BoardId matched.
  //
  for (Slot = 0; Slot <  TABLE_LIST_NUM; Slot ++) {
    DEBUG ((DEBUG_INFO, "\nTable [%d] = 0x%x\n", Slot, Table [Slot]));
    BoardInitTable = (BOARD_INIT_TABLE *) Table [Slot];
    if (Table [Slot] == NULL) {
      continue;
    }
    while (TRUE) {
      DEBUG ((DEBUG_INFO, " |-- BoardId = 0x%04x Init = 0x%x\n", BoardInitTable->BoardId, BoardInitTable->BoardInit));
      if (BoardInitTable->BoardId == 0 && BoardInitTable->BoardInit == NULL) {
        break;
      }
      if ((BoardInitTable->BoardId == BoardId || BoardInitTable->BoardId == COMMON_BOARD_ID) && BoardInitTable->BoardInit != NULL) {
        DEBUG ((DEBUG_INFO, " |--- Launch PEI Board Init Function | Board Id = 0x%x in 0x%x\n", BoardInitTable->BoardId, BoardInitTable->BoardInit));
        Status = BoardInitTable->BoardInit (Content);
      }
      BoardInitTable ++;
    }
  }

  ClearTableSlot (Table); // clear all slots after init.
  PcdSet64S(PcdFuncBoardConfigInit, 0);

  return Status;
}

/**
  Entry point for the driver.

  This routine gets info from the PlatformInfo and produces a protocol
  to be consumed by DXE drivers to identify platforms.

  @param[in]  ImageHandle  Image Handle.
  @param[in]  SystemTable  EFI System Table.

  @retval     EFI_SUCCESS  Function has completed successfully.
  @retval     Others       All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
BoardConfigInitEntryPoint (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  //UINTN Pointer;

  //
  // Init Board Config Init Post-Mem function is called in Post-Mem phase.
  //
  //Pointer = (UINTN) PcdGet64 (PcdFuncBoardConfigInit);

  //if (Pointer == 0) {
  PcdSet64S(PcdFuncBoardConfigInit, (UINTN) BoardConfigInit);
  //}

  return EFI_SUCCESS;;
}
