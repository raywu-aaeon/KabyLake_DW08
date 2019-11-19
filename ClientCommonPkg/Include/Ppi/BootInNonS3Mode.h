/** @file   
  This PPI is installed by the platform PEIM after it detects
  current boot mode is NOT S3.

  This PPI is introduce to resolve S3 performance issue.
  Some modules may NOT need to be dispatched in S3 mode, like
  SignFv verification. Normally, we can handle this in driver
  entrypoint. However, if this module is compressed, PeiCore
  may need spend time on decompress it. The decompressing itself
  may take time. So here we add dependency to let PeiCore not
  process those modules.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2012 - 2016 Intel Corporation.

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

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be 
  modified, except as allowed by additional terms of your license agreement.

@par Specification Reference:
**/

#ifndef __BOOT_IN_NON_S3_MODE_PPI_H__
#define __BOOT_IN_NON_S3_MODE_PPI_H__

#define EFI_PEI_BOOT_IN_NON_S3_MODE_PPI \
  { \
    0x667ef084, 0x74ef, 0x4707, { 0x90, 0xb3, 0x75, 0x14, 0x59, 0x20, 0xb3, 0x9c } \
  }

extern EFI_GUID gEfiPeiBootInNonS3ModePpiGuid;

#endif
