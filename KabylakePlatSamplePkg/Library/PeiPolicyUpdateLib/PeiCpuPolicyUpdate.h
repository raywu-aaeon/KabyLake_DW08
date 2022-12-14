/** @file
  Header file for PEI CpuPolicyUpdate.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2017 Intel Corporation.

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
#ifndef _PEI_CPU_POLICY_UPDATE_H_
#define _PEI_CPU_POLICY_UPDATE_H_

#include <PiPei.h>
#include <PlatformInfo.h>
#include <Ppi/SiPolicy.h>
#include <Ppi/Wdt.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Library/PeiPlatformLib.h>
#include <Library/PeiPlatformHookLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseCryptLib.h>
#include <SetupVariable.h>
#include <PchAccess.h>
#include <Register/Cpuid.h>
#include <Register/Msr.h>
#include <CpuAccess.h>
#include <Ppi/MasterBootMode.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include "PeiPchPolicyUpdate.h"
#include <Library/EcLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/MmPciLib.h>
#include <SoftwareGuardSetupData.h>

EFI_STATUS
EFIAPI
InitCpuPmConfigBySetupValues (
  IN OUT  SI_POLICY_PPI            *SiPolicyPpi,
  IN SETUP_DATA                    *SetupData,
  IN CPU_SETUP                     *CpuSetup
  );

#endif
