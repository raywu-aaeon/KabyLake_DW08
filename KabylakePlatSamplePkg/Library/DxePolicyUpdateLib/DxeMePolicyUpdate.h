/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2008 - 2015 Intel Corporation.

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

#ifndef _DXE_ME_POLICY_UPDATE_H_
#define _DXE_ME_POLICY_UPDATE_H_

#include <PiDxe.h>
#include <SetupVariable.h>
#include <Setup.h>
#include <HeciRegs.h>
#include <Setup/MeSetup.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>
#include <IndustryStandard/Acpi10.h>
#ifdef PERFORMANCE_FLAG
#include <Protocol/FpdtFirmwarePerformance.h>
#endif
#include <Library/BaseMemoryLib.h>
#include <Protocol/HeciProtocol.h>
#include <Protocol/MePolicy.h>
#include <Library/HobLib.h>
#include <MeBiosPayloadHob.h>
#include <Library/ConfigBlockLib.h>
#include <ConfigBlock/MePeiConfig.h>
#include <Library/DxeMeLib.h>

#define PLATFORM_BOOT_TABLE_PTR_TYPE   0x1001
#define PLATFORM_BOOT_RECORD_TYPE      0x1022
//
// Timeout values based on HPET
//
#define HECI_MSG_DELAY                 2000000  ///< show warning msg and stay for 2 seconds.
#define CONVERSION_MULTIPLIER          1000000  ///< msec to nanosec multiplier
#define PLATFORM_BOOT_TABLE_SIGNATURE  SIGNATURE_32 ('P', 'B', 'P', 'T')

extern EFI_GUID                        gMeInfoSetupGuid;

//
// Platform Boot Performance Table Record
//

typedef struct {
  UINT16 Type;
  UINT8  Length;
  UINT8  Revision;
  UINT32 Reserved;
  UINT64 TimestampDelta1;
  UINT64 TimestampDelta2;
  UINT64 TimestampDelta3;
} PLATFORM_BOOT_TABLE_RECORD;

//
// Platform boot Performance Table
//

typedef struct {
  EFI_ACPI_COMMON_HEADER     Header;
  PLATFORM_BOOT_TABLE_RECORD PlatformBoot;
} PLATFORM_BOOT_PERFORMANCE_TABLE;

/**
  Update ME Policy while MePlatformProtocol is installed.

  @param[in] MePolicyInstance     Instance of ME Policy Protocol

**/
VOID
UpdateMePolicyFromMeSetup (
  IN ME_POLICY_PROTOCOL           *MePolicyInstance
  );

/**
  Update ME Policy if Setup variable exists.

  @param[in, out] MePolicyInstance     Instance of ME Policy Protocol

**/
VOID
UpdateMePolicyFromSetup (
  IN OUT ME_POLICY_PROTOCOL     *MePolicyInstance
  );

/**
  Functions performs HECI exchange with FW to update MePolicy settings.

  @param[in] Event         A pointer to the Event that triggered the callback.
  @param[in] Context       A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
UpdateMeSetupCallback (
  IN  EFI_EVENT                       Event,
  IN  VOID                            *Context
  );

#endif
