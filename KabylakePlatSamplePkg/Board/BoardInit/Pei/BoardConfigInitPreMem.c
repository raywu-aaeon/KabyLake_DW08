/** @file
  Source code file for board configuration Pcd Init Pre-Memory PEI module

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <PlatformBoardId.h>
#include <PlatformInfo.h>
#include <Board/BoardInitTable.h>
#include <Board/BoardConfigInit.h>

/**
  Platform Init before memory PEI module entry point

  @param[in]  PlatformInfo         Pointer to PLATFORM_INFO data structure.

  @retval     EFI_SUCCESS          The function completes successfully.
  @retval     EFI_NOT_FOUND        The BoardInitTableList is not init properly.
**/
EFI_STATUS
BoardConfigInitPreMem (
  IN PLATFORM_INFO *PlatformInfo
  )
{
  EFI_STATUS Status;
  VOID **Table;
  BOARD_INIT_TABLE *BoardInitTable;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;
  UINT8 Slot;

  if (PlatformInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  if (EFI_ERROR (Status)) {
    BoardId = COMMON_BOARD_ID;
  }

  Table = (VOID *) (UINTN) PcdGet32 (PcdPeiBoardInitTableList);

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
        DEBUG ((DEBUG_INFO, " |--- Launch PEI Premem Board Init Function | Board Id = 0x%x in 0x%x\n", BoardInitTable->BoardId, BoardInitTable->BoardInit));
        Status = BoardInitTable->BoardInit ((VOID *) PlatformInfo);
      }
      BoardInitTable ++;
    }
  }

  PcdSet32S (PcdPeiBoardInitTableList, 0);

  return Status;
}


/**
  Platform Init before memory PEI module entry point

  @param[in]  FileHandle           Not used.
  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_SUCCESS          The function completes successfully
  @retval     EFI_OUT_OF_RESOURCES Insufficient resources to create database
**/
EFI_STATUS
EFIAPI
BoardConfigInitPreMemEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINTN Pointer;

  //
  // Init Board Config Init Pre-Mem function is called in Pre-Mem phase.
  //
  Pointer = (UINTN) PcdGet32 (PcdFuncBoardConfigInitPreMem);

  if (Pointer == 0) {
    PcdSet32S (PcdFuncBoardConfigInitPreMem, (UINTN) BoardConfigInitPreMem);
  }

  return EFI_SUCCESS;;
}
