/** @file
  Header file for Save Previous Memory Configuration Driver.

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _SAVE_MEMORY_CONFIG_DRIVER_H
#define _SAVE_MEMORY_CONFIG_DRIVER_H

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//

#include <Base.h>
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Protocol/VariableLock.h>

#define PCIE_CFG_ADDR(bus,dev,func,reg) \
  ((VOID*) (UINTN)(PcdGet64 (PcdPciExpressBaseAddress) + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

//
// PCI Register Definitions
//
#define PM_CONFIG_SPACE_DEV             0x1F
#define R_PCH_LPC_GEN_PMCON_2           0xA2
#define B_PCH_LPC_GEN_PMCON_DRAM_INIT   0x80

//
// ME DRAM_INIT_DONE BIOS Action definitions
// Refer to ME BWG: section 8.4.2.2
//
#define ME_DRAM_INIT_DONE_CONTINUE_POST   0
#define ME_DRAM_INIT_DONE_GLOBAL_RESET    1
#define ME_DRAM_INIT_DONE_NONE_PCR        2
#define ME_DRAM_INIT_DONE_PCR             3

/**
  This is the standard EFI driver point that detects whether there is a
  MemoryConfigurationData HOB and, if so, saves its data to nvRAM.

  @param[in] ImageHandle    Handle for the image of this driver
  @param[in] SystemTable    Pointer to the EFI System Table

  @retval    EFI_UNSUPPORTED
**/
EFI_STATUS
EFIAPI
SaveMemoryConfigEntryPoint(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );
#endif
