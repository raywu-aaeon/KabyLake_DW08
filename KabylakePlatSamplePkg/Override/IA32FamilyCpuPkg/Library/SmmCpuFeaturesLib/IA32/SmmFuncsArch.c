/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file
  SMM CPU misc functions for IA32 arch specific.

Copyright (c) 2014 - 2015, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include <PiSmm.h>
#include <Library/BaseLib.h>

extern IA32_DESCRIPTOR  gSmmFeatureSmiHandlerIdtr;

UINT32  gProtModeIdtr = 0;

/**
  Initialize SMM Protected Mode IDT table.
  
  @param[in]  Cr3      CR3 value.
**/
VOID
InitProtectedModeIdt (
  IN UINT32  Cr3
  )
{
  if (gProtModeIdtr == 0) {
    gProtModeIdtr = (UINT32)(UINTN)&gSmmFeatureSmiHandlerIdtr;
  }
}
