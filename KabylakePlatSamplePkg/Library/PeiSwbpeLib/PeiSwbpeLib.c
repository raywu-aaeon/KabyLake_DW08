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


#include <PiPei.h>
#include <Base.h>
#include <Uefi.h>
#include <Library/PeiSwbpeLib.h>

VOID
DoSwbpeTypeIoOut (
  UINTN       PortAddress,
  UINTN       PortData,
  UINTN       PortDataWidth,
  UINTN       MailboxAddress
  );

UINTN
DoSwbpeTypeIoIn (
  UINTN       PortAddress,
  UINTN       PortDataWidth,
  UINTN       MailboxAddress
  );

UINTN
DoSwbpeTypeMemRead (
  UINTN       MemAddress,
  UINTN       MemDataWidth,
  UINTN       MailboxAddress
  );

VOID
DoSwbpeTypeMemWrite (
  UINTN       MemAddress,
  UINTN       MemData,
  UINTN       MemDataWidth,
  UINTN       MailboxAddress
  );

VOID
DoSwbpeTypeInt3 (
  UINTN MailboxAddress
  );

VOID
SoftwareBreakpointEvent (
  SWBPE_MAILBOX *Mailbox
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
{
  //
  // Initialize the mailbox to check for debugger presence
  //
  Mailbox->DebuggerStatus    = EFI_UNSUPPORTED;

  //
  // Determine the type of event, and invoke it
  //
  switch (Mailbox->EventType) {

    case SwbpeTypeIoOut:
      DoSwbpeTypeIoOut (
        (UINTN) Mailbox->EventAddress,
        (UINTN) Mailbox->EventData,
        (UINTN) Mailbox->EventDataWidth,
        (UINTN) Mailbox
        );
       break;

    case SwbpeTypeIoIn:
      Mailbox->EventData = DoSwbpeTypeIoIn (
                             (UINTN) Mailbox->EventAddress,
                             (UINTN) Mailbox->EventDataWidth,
                             (UINTN) Mailbox
                             );
      break;

    case SwbpeTypeMemWrite:
      DoSwbpeTypeMemWrite (
        (UINTN) Mailbox->EventAddress,
        (UINTN) Mailbox->EventData,
        (UINTN) Mailbox->EventDataWidth,
        (UINTN) Mailbox
        );
      break;

    case SwbpeTypeMemRead:
      Mailbox->EventData = DoSwbpeTypeMemRead (
                             (UINTN) Mailbox->EventAddress,
                             (UINTN) Mailbox->EventDataWidth,
                             (UINTN) Mailbox
                              );
      break;

    case SwbpeTypeIntXX:
      DoSwbpeTypeInt3 ((UINTN) Mailbox);
      break;

    default:
      //
      // If type not recognized then just leave EFI_UNSUPPORTED
      //
      break;
  }
}

