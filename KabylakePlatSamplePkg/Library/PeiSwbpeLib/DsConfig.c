/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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
DsBreakpointEvent (
  DS_CONFIG_SWBPE_ID        Id,
  EFI_PHYSICAL_ADDRESS      BaseAddress,
  UINTN                     Size,
  EFI_STATUS                *Status
  )
/*++

Routine Description:

  This function generates a SWBPE event.  It will create a DS_CONFIG_SWBPE_MAILBOX structure
  with the DS_CONFIG_SWBPE_GUID and the input arguments and trigger an implementation specific
  SWBPE.

Arguments:

  Id -
  BaseAddress -
  Size -

Returns:

  EFI_SUCCESS       -  The function event was intercepted by the debugger and handled successfully
  EFI_UNSUPPORTED   -  The function event was not intercepted by the debugger and no

--*/
{
  SWBPE_MAILBOX               Mailbox     = {0, 0, 0, 0, 0, 0};
  DS_CONFIG_SWBPE_MAILBOX     DsMailbox   = {DS_CONFIG_SWBPE_GUID, 0, 0, 0, 0};

  //
  // Setup DS-specific data for the debugger
  //
  DsMailbox.Id               = Id;
  DsMailbox.BaseAddress      = BaseAddress;
  DsMailbox.Size             = Size;

  //
  // Setup generic SWBPE data for a DS event
  //
  Mailbox.EventType         = DS_CONFIG_SWBPE_TYPE;
  Mailbox.EventAddress      = DS_CONFIG_SWBPE_ADDRESS;
  Mailbox.EventData         = DS_CONFIG_SWBPE_DATA;
  Mailbox.EventDataWidth    = DS_CONFIG_SWBPE_DATA_WIDTH ;
  Mailbox.DebuggerArgs      = (EFI_PHYSICAL_ADDRESS) (UINTN) (&DsMailbox);

  //
  // Invoke a DS SWBPE
  //
  SoftwareBreakpointEvent (&Mailbox);

  //
  // Return the debugger status to the caller
  //
  *Status = Mailbox.DebuggerStatus;
}

