/** @file
  Internal header file for TscTimerLib instances.

@copyright
 Copyright (c) 2011 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains 'Framework Code' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may not be modified, except as allowed by
 additional terms of your license agreement.

@par Specification Reference:
**/

#ifndef _TSC_TIMER_LIB_INTERNAL_H_
#define _TSC_TIMER_LIB_INTERNAL_H_

#include <Base.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BasePlatformTimerLib.h>

/**  Get TSC frequency.

  @return The number of TSC counts per second.

**/
UINT64
InternalGetTscFrequency (
  VOID
  );

/**  Calculate TSC frequency.

  The TSC counting frequency is determined by comparing how far it counts
  during a 1ms period as determined by the ACPI timer. The ACPI timer is
  used because it counts at a known frequency.
  If ACPI I/O space not enabled, this function will enable it. Then the
  TSC is sampled, followed by waiting for 3579 clocks of the ACPI timer, or 1ms.
  The TSC is then sampled again. The difference multiplied by 1000 is the TSC
  frequency. There will be a small error because of the overhead of reading
  the ACPI timer. An attempt is made to determine and compensate for this error.

  @return The number of TSC counts per second.

**/
UINT64
InternalCalculateTscFrequency (
  VOID
  );

#endif
