/** @file
  Platform Info Driver.

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

#ifndef _PLATFORM_INIT_DXE_H_
#define _PLATFORM_INIT_DXE_H_

#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <PlatformInfo.h>
#include <PchAccess.h>
#include <Library/HobLib.h>
#include <SetupVariable.h>
#include <Library/MmPciLib.h>
#include <Library/DebugLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/BootGuardRevocationLib.h>// AdvancedFeaturesContent
#include <Library/MmPciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Uefi/UefiBaseType.h>
#include <Library/HobLib.h>
#include <OemSetup.h>
#include <Protocol/MemInfo.h>
#include <MemInfoHob.h>
#include <Library/PchCycleDecodingLib.h>
#include <Base.h>
#include <PlatformBoardConfig.h>
#ifdef SINITBIN_FLAG
#include <TxtInfoHob.h>
#include <Txt.h>
#include <Protocol/FirmwareVolume2.h>
extern EFI_GUID gSinitModuleGuid;
#endif

extern EFI_GUID                   gEpcBiosDataGuid; // AdvancedFeaturesContent
extern EFI_GUID                   gEpcOsDataGuid; // AdvancedFeaturesContent
extern EFI_GUID                   gBdsAllDriversConnectedProtocolGuid;
extern EFI_GUID                   gSinitSvnGuid; // AdvancedFeaturesContent
extern EFI_GUID                   gSgxPolicyStatusGuid; // AdvancedFeaturesContent

#define  HbPciD0F0RegBase    (UINTN) PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (0 << 15) + (UINT32) (0 << 12) // AdvancedFeaturesContent
//----------------------------------------------------------------------------
// VT-d Engine Configuration Register Details
//----------------------------------------------------------------------------
#define SA_VTD_RMRR_USB_LENGTH  0x20000

VOID
PchPlatformInitDxe(
  VOID
  );

VOID
SaPlatformInitDxe (
  VOID
  );
#ifndef MINTREE_FLAG
VOID
EFIAPI
CheckLidBaseEcProtocolVersion (
  VOID
  );

VOID
UpdateDimmPopulation(
  PLATFORM_INFO                *PlatformInfo
  );
#endif //MINTREE_FLAG
#ifdef CSM_FLAG
VOID
InitializeGfxInt15 (
  VOID
  );

VOID
UpdateCmosDisplayRegisters(
  PLATFORM_INFO                *PlatformInfo
  );
#endif

VOID
Callback1394BootScript (
  VOID
  );

#ifdef SINITBIN_FLAG
/**

  This function looks for SINIT ACM in FVs and updates TXT HOB
  with SINIT ACM Base and Size.

  @retval EFI_SUCCESS     - SINIT ACM found and copied.
  @retval EFI_NOT_FOUND   - If TxtInfoHob is not found
  @retval EFI_UNSUPPORTED - TXT Device memory not available.

**/
EFI_STATUS
TxtSinitAcmLoad (
  VOID
  );
#endif

#endif
