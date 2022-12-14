/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2016 Intel Corporation.

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

//
// You can add below line to INF file:
// [libraries.common]      <== Select one of below stub
// ModuleDebugEdkIIGlueLib <== EdkIIGlueLib
// ModuleDebugPeiLib       <== PeiLib
// ModuleDebugDxeLib       <== EfiDriverLib
// ModuleDebugRuntimeLib   <== EfiRuntimeLib
// ModuleDebugSmmLib       <== SmmDriverLib
//
// [nmake.common]
// C_FLAGS = $(C_FLAGS) /FIModuleDebugLib.h
//

#ifndef _MODULE_DEBUG_H_
#define _MODULE_DEBUG_H_

#include "Tiano.h"
#include "Pei.h"

#define M_DEBUG(arg)            ModuleDebugPrint arg
#define M_ASSERT(assertion)     if(!(assertion)) ModuleDebugAssert(__FILE__, __LINE__, #assertion)

#define M_PEI_DEBUG(arg)            PeiModuleDebugPrint arg
#define M_PEI_ASSERT(assertion)     if(!(assertion)) PeiModuleDebugAssert(__FILE__, __LINE__, #assertion)

VOID
ModuleDebugPrint (
  IN  UINTN        ErrorLevel,
  IN  CONST CHAR8  *Format,
  ...
  );

VOID
ModuleDebugAssert (
  IN CONST CHAR8  *FileName,
  IN UINTN        LineNumber,
  IN CONST CHAR8  *Description
  );

VOID
PeiModuleDebugAssert (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN CHAR8              *FileName,
  IN INTN               LineNumber,
  IN CHAR8              *Description
  );

VOID
PeiModuleDebugPrint (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN  UINTN             ErrorLevel,
  IN  CHAR8             *Format,
  ...
  );

#endif
