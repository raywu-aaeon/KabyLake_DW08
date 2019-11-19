/**@file
  RST one click BIOS support module.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2018 Intel Corporation.

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
**/

#ifndef __RST_ONE_CLICK_ENABLE__
#define __RST_ONE_CLICK_ENABLE__

#include <Uefi.h>
#include <Uefi/UefiMultiPhase.h>

#include <SetupVariable.h>
#include <OemSetup.h>
#include <PchAccess.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <Library/PchInfoLib.h>

#include "RstConfigVariable.h"
//AMI_OVERRIDE_START >>> EIP426038 - Fix build failed with new GCC flags
//GLOBAL_REMOVE_IF_UNREFERENCED EFI_RUNTIME_SERVICES *gRS;
EFI_RUNTIME_SERVICES *gRS;

//GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID  gPchSetupVariableGuid;
extern EFI_GUID  gPchSetupVariableGuid;
//GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID  gSetupVariableGuid;
extern EFI_GUID  gSetupVariableGuid;
//AMI_OVERRIDE_END <<< EIP426038 - Fix build failed with new GCC flags
/**
  Updates system configuration based on RST_CONFIG_VARIABLE recieved from OS

  @param[in] RstConfig  A pointer to an instance of RST_CONFIG_VARIABLE received from OS

  @retval EFI_STATUS  Returns EFI_SUCCESS only if update wasn't necessary, otherwise returns last error
**/
EFI_STATUS
UpdateSystemConfiguration (
  IN RST_CONFIG_VARIABLE  *RstConfig
  );

/**
  Updates PchSetup variable based on information passed in RST_CONFIG_VARIABLE

  @param[in] RstConfig  RST confifg variable

  @retval EFI_STATUS  Returns EFI_SUCCESS if PchSetup was updated successfully, returns last error otherwise
**/
EFI_STATUS
UpdatePchSetupVariable (
  IN RST_CONFIG_VARIABLE  *RstConfig
  );

/**
  Checks if the integrated SATA controller is in RAID mode

  @retval BOOLEAN  True if SATA controller is in RAID mode
**/
BOOLEAN
IsIntegratedSataControllerInRaidMode (
  VOID
  );

/**
  Switches SATA controller to RAID mode

  @param PchSetup  A pointer to PCH_SETUP instance
**/
VOID
SwitchSataControllerToRaid (
  OUT PCH_SETUP*  PchSetup
  );

/**
  Checks if given root port is remap capable based on information in SETUP_VOLATILE_DATA

  @param  SetupData  A pointer to an instance of SETUP_VOLATILE_DATA
  @param  RpNumber  Root port number to check

  @retval  BOOLEAN  Returns TRUE if root port is remap capable, returns false otherwise
**/
BOOLEAN
CheckIfRootPortIsRemapCapable (
  SETUP_VOLATILE_DATA *SetupData,
  UINT32  RpNumber
  );

/**
  Disables NVMe remapping on all remap capable root ports

  @param[in, out] PchSetup  An instance of PchSetup to be updated
**/
VOID
DisableRemapOnAllCapableRp (
  IN OUT PCH_SETUP  *PchSetup
  );

/**
  Enables NVMe remapping on all remap capable root ports

  @param PchSetup  An instance of PchSetup to be updated

  @retval EFI_STATUS  Returns EFI_SUCCESS if remapping has been successfully enabled, returns last error otherwise
**/
VOID
EnableRemapOnAllCapableRp (
  OUT PCH_SETUP  *PchSetup
  );

/**
  Creates an instance of RST_CONFIG_VARIABLE.
  Since this function initializes variable to 0 it can also be used to reseting the variable

  @retval EFI_STATUS  Returns status of SetVariable runtime service
**/
EFI_STATUS
CreateRstConfigVariable (
  VOID
  );

#endif
