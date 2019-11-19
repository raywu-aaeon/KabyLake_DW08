/** @file
  This header file provides platform specific definitions used
  by other modules for platform specific initialization.
  This is not suitable for consumption by ASL or VRF files.

@copyright
 Copyright (c) 2004 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _AMI_CPU_INIT_DATA_HOB_LIB_H_
#define _AMI_CPU_INIT_DATA_HOB_LIB_H_

VOID AmiUpdateCpuInitDataHob (
	    IN OUT  VOID       *Policy,
		IN      UINT16     PolicySize,
		IN OUT  VOID       *PeiRcPolicyHobPtr
);

#endif
