/** @file
  Header file for Silicon Init PEIM.

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
#ifndef _SI_INIT_MODULE_H_
#define _SI_INIT_MODULE_H_

#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/SiInitPpi.h>
#include <Ppi/SiPolicy.h>
#include <Private/Library/PchInitLib.h>
#include <Private/Library/SaInitLib.h>
#include <Private/Library/CpuInitLib.h>
#include <Private/Library/MeInitLib.h>
#include <Private/Library/CpuPowerOnConfigLib.h>
#include <Private/Library/CpuS3Lib.h>
#include <Private/Library/ActiveManagementLib.h>
#include <Private/Library/TxtPeiLib.h>
#include <Private/Library/SmbiosCpuLib.h>
#include <Private/SiConfigHob.h>

//
// GUID Definitions
//
extern EFI_GUID gSiConfigHobGuid;

//
// Post Mem Performance GUIDs
//
extern EFI_GUID gPerfPchPostMemGuid;
extern EFI_GUID gPerfSaPostMemGuid;
extern EFI_GUID gPerfS3CpuInitPostMemGuid;
extern EFI_GUID gPerfSaSecLockPostMemGuid;
extern EFI_GUID gPerfCpuStrapPostMemGuid;
extern EFI_GUID gPerfMpPostMemGuid;
extern EFI_GUID gPerfCpuPostMemGuid;
extern EFI_GUID gPerfSaResetPostMemGuid;
extern EFI_GUID gPerfCpuPowerMgmtGuid;
extern EFI_GUID gPerfMePostMemGuid;
extern EFI_GUID gPerfAmtPostMemGuid;

/**
  Silicon Init End of PEI callback function. This is the last change before entering DXE and OS when S3 resume.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
SiInitOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  );

/**
  Slicon Initializes after PostMem phase Policy PPI produced,
  All required polices must be installed before the callback

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  SiPolicy PPI.

  @retval EFI_SUCCESS             Succeeds.
**/
EFI_STATUS
EFIAPI
SiInitPostMemOnPolicy (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi
  );

#endif // _SI_INIT_MODULE_H_
