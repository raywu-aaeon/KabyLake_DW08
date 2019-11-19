/** @file
  Functions which applys complex row address swizzling to allow incremental
  cell testing.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2014 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification
**/
#include "DcttWrapper.h"
#include "DcttSwizzleFunctions.h"

//Physical 0123/4567/89AB/CDEF
//Logical  0123/4567/EFCD/AB89
UINT32
Swizzle_GetLogicalAddress01234567EFCDAB89 (
  UINT32 physicalAddress
  )
{
  UINT32 partialSwizzle;

  partialSwizzle = (((physicalAddress & 0xCC00) >> 2) | ((physicalAddress & 0x3300) << 2));

  return (physicalAddress & 0xFF) | ((partialSwizzle & 0xF00) << 4) | ((partialSwizzle & 0xF000) >> 4);
}

UINT32
Swizzle_GetLogicalAddressNoSwizzle (
  UINT32 physicalAddress
  )
{
  return physicalAddress;
}

UINT32
Swizzle_GetLogicalAddress3xor1_3xor2 (
  UINT32 physicalAddress
  )
{
  UINT32 xorMask;

  xorMask = (((physicalAddress & SWIZZLE_BIT3_MASK) >> 1) | ((physicalAddress & SWIZZLE_BIT3_MASK) >> 2));

  return (xorMask ^ physicalAddress);
}
