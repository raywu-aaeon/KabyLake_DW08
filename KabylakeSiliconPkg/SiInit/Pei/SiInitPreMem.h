/** @file
  Header file for Silicon Init Pre Memory module.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2016 Intel Corporation.

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
#ifndef _SI_INIT_PRE_MEM_MODULE_H_
#define _SI_INIT_PRE_MEM_MODULE_H_

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/SiPolicy.h>
#include <Private/Library/PchInitLib.h>
#include <Private/Library/SaInitLib.h>
#include <Private/Library/CpuInitLib.h>
#include <Private/Library/MeInitLib.h>
#include <Private/Library/ActiveManagementLib.h>
#include <Private/Library/MemoryInitLib.h>
#include <Private/Library/EvLoaderLib.h>
#include <Private/Library/CpuPowerOnConfigLib.h>

//
// IO/MMIO resource limits
//
#define MIN_IO_SPACE_SIZE   0x10
//
// Set minimum MMIO space to 2MB for PCH XDCI requirement
//
#define MIN_MMIO_SPACE_SIZE 0x200000

//
// Pre Mem Performance GUIDs
//
extern EFI_GUID gPerfPchPrePolicyGuid;
extern EFI_GUID gPerfSiValidateGuid;
extern EFI_GUID gPerfPchValidateGuid;
extern EFI_GUID gPerfCpuValidateGuid;
extern EFI_GUID gPerfMeValidateGuid;
extern EFI_GUID gPerfSaValidateGuid;
extern EFI_GUID gPerfHeciPreMemGuid;
extern EFI_GUID gPerfPchPreMemGuid;
extern EFI_GUID gPerfCpuPreMemGuid;
extern EFI_GUID gPerfMePreMemGuid;
extern EFI_GUID gPerfSaPreMemGuid;
extern EFI_GUID gPerfEvlGuid;
extern EFI_GUID gPerfMemGuid;

#endif // _SI_INIT_PRE_MEM_MODULE_H_
