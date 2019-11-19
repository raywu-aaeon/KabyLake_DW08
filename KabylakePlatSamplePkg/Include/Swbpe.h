/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2014 Intel Corporation.

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


#ifndef _SWBPE_H_
#define _SWBPE_H_

//
// These values represent events types the SWBPE interface supports.
//
typedef enum {
  SwbpeTypeUndefined          = 0,
  SwbpeTypeIoIn               = 1,
  SwbpeTypeIoOut              = 2,
  SwbpeTypeMemRead            = 3,
  SwbpeTypeMemWrite           = 4,
  SwbpeTypeIntXX              = 5,
  SwbpeTypeMax                = 6
} SWBPE_EVENT_TYPE;

typedef enum {
  SwbpeWidthByte              = 1,
  SwbpeWidthWord              = 2,
  SwbpeWidthDword             = 4
} SWBPE_EVENT_DATA_WIDTH;


//
// This is the data structure the SWBPE interface uses
// to communicate with a debugger.
//
#pragma pack (push, 1)
typedef struct {
  UINT64                    EventType;
  EFI_PHYSICAL_ADDRESS      EventAddress;
  UINT64                    EventData;
  UINT64                    EventDataWidth;
  EFI_PHYSICAL_ADDRESS      DebuggerArgs;
  EFI_STATUS                DebuggerStatus;
} SWBPE_MAILBOX;
#pragma pack (pop)


VOID
SoftwareBreakpointEvent (
  IN OUT SWBPE_MAILBOX    *Mailbox
  )
/*++

Routine Description:

  This function generates an action that can be trapped by an debugger.

Arguments:

  Mailbox  - A pointer to a data structure that contains information about the event and data payload.

Returns:

  EFI_SUCCESS       -  The function event was intercepted by the debugger and handled successfully
  EFI_UNSUPPORTED   -  The function event was not intercepted by the debugger

--*/
;

#endif // _SWBPE_H_
