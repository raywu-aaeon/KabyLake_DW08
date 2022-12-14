/** @file
  Header file for the SaPolicyInitPei PEIM.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2017 Intel Corporation.

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
#ifndef _SA_POLICY_INIT_PEI_H_
#define _SA_POLICY_INIT_PEI_H_
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciLib.h>
#include <Library/PeiSaPolicyLib.h>
#include <Ppi/SiPolicy.h>
#include <SaPolicyCommon.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Library/PeiSiPolicyUpdateLib.h>
#include <Library/PeiPolicyBoardConfigLib.h>
#include <Library/PeiSaPolicyDebugLib.h>
#include <FirwmareConfigurations.h>
#include <Library/TimerLib.h>
#include <Library/GpioLib.h>

//
// Functions
//
/**
PCIe GPIO Write

@param[in] GpioSupport - GPIO Support; 0=Disable, 1=PCH Based, 2=I2C Based
@param[in] Expander    - For I2C Based=Expander No
@param[in] Gpio        - GPIO Number
@param[in] Active      - GPIO Active Information; High/Low
@param[in] Level       - Write GPIO value (0/1)

**/
VOID
PcieGpioWrite(
IN       UINT8                        GpioSupport,
IN       UINT8                        Expander,
IN       UINT32                       Gpio,
IN       BOOLEAN                      Active,
IN       BOOLEAN                      Level
);


/**
PcieCardResetWorkAround performs PCIe Card reset on root port

@param[in out] SiPreMemPolicyPpi       - SI_PREMEM_POLICY_PPI

@retval EFI_SUCCESS              The policy is installed and initialized.
**/
EFI_STATUS
PcieCardResetWorkAround(
IN OUT   SI_PREMEM_POLICY_PPI         *SiPreMemPolicyPpi
);
#endif
