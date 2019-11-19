/** @file
 Header file for Board initial table definition

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

#ifndef _BOARD_INIT_TABLE_H_
#define _BOARD_INIT_TABLE_H_

#define COMMON_BOARD_ID             0xf000

typedef
EFI_STATUS
(EFIAPI *BOARD_INIT) (
  IN VOID *Content
  );

typedef struct {
  UINT16 BoardId;
  BOARD_INIT BoardInit;
} BOARD_INIT_TABLE;

VOID
ClearTableSlot (
  IN VOID **Table
  );

EFI_STATUS
CommonInitInternalLibConstructor (
  IN BOARD_INIT_TABLE *BoardInitTable,
  IN UINTN FixedSlot
  );

//
// Size of Board Init Table List entry.
//
#define DEFAULT_INIT_SLOT_PREMEM 0x0
#define DEFAULT_INIT_SLOT_POSTMEM 0x01
#define DEFAULT_INIT_SLOT_DXE 0x0
#define TABLE_LIST_NUM 0x10

EFI_STATUS
GetFreeTableSlot (
  VOID **Table,
  UINT8 *FreeSlot
);

EFI_STATUS
InitBoardInitTableList (
  VOID
);

EFI_STATUS
GetBoardId (
  OUT UINT16 *BoardId,
  OUT UINT16 *BoardIdOrgValue
  );

#endif // _BOARD_INIT_LIB_H_
