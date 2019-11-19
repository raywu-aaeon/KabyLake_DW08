/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2016 Intel Corporation.

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

#ifndef _DS_CONFIG_H_
#define _DS_CONFIG_H_

//
// This is the GUID that is used to uniquely identify DS Config SWBPEs
// to the debugger
//
#define DS_CONFIG_SWBPE_GUID   \
    { 0x1a237f6e, 0xfa50, 0x4338, { 0xae, 0x36, 0x6c, 0x89, 0x38, 0xa7, 0xf2, 0xc2 } }

//
// These are the ID values for DS SWBPEs
//
typedef enum {
  DsConfigSwbpeIdUndefined         = 0,
  DsConfigSwBpeIdCheck             = 1,
  DsConfigSwBpeIdNemStart          = 2,
  DsConfigSwBpeIdNemStop           = 3,
  DsConfigSwBpeIdDramStart         = 4,
  DsConfigSwBpeIdDramStop          = 5,
  DsConfigSwBpeIdMax               = 6
} DS_CONFIG_SWBPE_ID;


//
// This is the DS-specific data that firmware will
// pass to the debugger via the DebuggerArgs field of
// SWBPE_MAILBOX.
//
#pragma pack (push, 1)
typedef struct {
    EFI_GUID                    Guid;
    UINT64                      Id;
    EFI_PHYSICAL_ADDRESS        BaseAddress;
    UINT64                      Size;
    EFI_STATUS                  *Status;
} DS_CONFIG_SWBPE_MAILBOX;
#pragma pack (pop)


VOID
DsBreakpointEvent (
  IN  DS_CONFIG_SWBPE_ID        Id,
  IN  EFI_PHYSICAL_ADDRESS      BaseAddress, OPTIONAL
  IN  UINTN                     Size, OPTIONAL
  OUT EFI_STATUS                *Status
  )
/*++

Routine Description:

  This function generates a SWBPE event.  It will create a DS_CONFIG_SWBPE_MAILBOX structure
  with the DS_CONFIG_SWBPE_GUID and the input arguments and trigger an implementation specific
  SWBPE.

Arguments:

  Id                - The event being signalled to the debugger
  BaseAddress       - The address of the buffer.  This is not used for some ID.
  Size              - The size of the buffer.  This is not used for some ID.
  Status            - The status returned by the debugger.

Returns:

  EFI_SUCCESS       -  The function event was intercepted by the debugger and handled successfully
  EFI_UNSUPPORTED   -  The function event was not intercepted by the debugger and not handled

--*/
;

#endif // _DS_CONFIG_H_
