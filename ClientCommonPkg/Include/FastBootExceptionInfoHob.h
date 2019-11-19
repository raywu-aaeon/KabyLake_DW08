/**@file

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

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be 
  modified, except as allowed by additional terms of your license agreement.

@par Specification Reference:

**/

#ifndef __FAST_BOOT_EXCEPTION_INFO_HOB_H__
#define __FAST_BOOT_EXCEPTION_INFO_HOB_H__

#include <Pi/PiHob.h>
#include <FastBootDataDef.h>

typedef struct _FAST_BOOT_EXCEPTION_INFO_HOB {
  EFI_HOB_GUID_TYPE             Header;
  EFI_GUID                      GuidName;
  FAST_BOOT_EXCEPTION_TYPE      FbExceptionType;
  FAST_BOOT_EXCEPTION_CATEGORY  FbExceptionCategory;
} FAST_BOOT_EXCEPTION_INFO_HOB;

extern EFI_GUID gFastBootExceptionInfoHobGuid;

#endif // __FAST_BOOT_EXCEPTION_INFO_HOB_H__
