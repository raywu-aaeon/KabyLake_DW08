/** @file
  This header file provides platform specific definitions used
  by other modules for platform specific initialization.
  This is not suitable for consumption by ASL or VRF files.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2004 - 2016 Intel Corporation.

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

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "CommonDefinitions.h"
#include "PchAccess.h"
#include "SaAccess.h"
// AMI_OVERRIDE_START - For AMI Token.h.
#include <Token.h>
// AMI_OVERRIDE_END - For AMI Token.h.


// AMI_OVERRIDE_START - For AMI Token setting.
#ifndef FLASH_BLOCK_SIZE
#define FLASH_BLOCK_SIZE          0x10000
#endif
// AMI_OVERRIDE_END - For AMI Token setting.

#define CPU_EXTERNAL_CLOCK_FREQ   0x64
#define CPU_FREQUENCY_MODE_100    0x64
#define FREQUENCY_RESOLUTION_3182 0xc6e
#define NDIVIDER_BASE_VALUE       0x19d
#define MDIVIDER_VALUE_13         0xd

#endif
