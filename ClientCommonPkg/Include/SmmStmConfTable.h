/** @file
  GUID definition for the Smm Stm Configuraion Table

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
#ifndef _SMM_STM_CONF_TABLE_H_
#define _SMM_STM_CONF_TABLE_H_

#include <Protocol/SmmTxtResource.h>

extern EFI_GUID gSmmStmConfTableGuid;

///
/// Definitions
///
#define STM_OPT_OUT   0
#define STM_OPT_IN    1
#define STM_INACTIVE  0
#define STM_ACTIVE    2

///
/// STM Configuration Table
///
typedef struct _SMM_ENTRY_INIT_STRUCT SMM_ENTRY_INIT_STRUCT;

typedef
EFI_STATUS
(EFIAPI *STM_ADD_RESOURCE) (
  IN VOID               *ResourceList,
  IN UINT32             NumEntries OPTIONAL,
  IN UINT32             StartOver OPTIONAL
  );

#pragma pack(push, 1)
typedef struct _STM_CONFIGURATION_TABLE {
  UINT64                    StmFlags;
  SMM_TXT_RESOURCE_PROTOCOL *SmmTxtRscProtocol;
  STM_ADD_RESOURCE          StmAddResource;
} STM_CONFIGURATION_TABLE;
#pragma pack(pop)

#endif
