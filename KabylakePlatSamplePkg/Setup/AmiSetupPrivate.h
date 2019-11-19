/**@file

@copyright
 Copyright (c) 2010 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef __AmiSetupPrivate__H__
#define __AmiSetupPrivate__H__
#ifdef __cplusplus
extern "C" {
#endif

#ifndef VFRCOMPILE
#include <PlatformInfo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/PciIo.h>
#include <Protocol/MemInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci30.h>

EFI_STATUS HiiLibGetString(
  IN EFI_HII_HANDLE HiiHandle, 
  IN STRING_REF StringId, 
  IN OUT UINTN *StringSize, 
  OUT EFI_STRING String
  );

EFI_STRING
EFIAPI
HiiConstructRequestString (
  IN EFI_STRING      RequestString, OPTIONAL
  IN UINTN           Offset,
  IN UINTN           Width
  );

#define INVALID_HII_HANDLE NULL
#endif

#include <Token.h>
#include <SetupVariable.h>
#include <OemSetup.h>
#include <Library/AmiSetupLibrary.h>
#include <Library/PttHeciLib.h>

extern ME_SETUP         mMeSetup;
extern EFI_GUID         gAmiTseNVRAMUpdateGuid;

// CpuSetup.c
EFI_STATUS
EFIAPI
InitCpuMntrDefault (
  IN VOID
  );

VOID
InitCpuSetupCallback (
  VOID
  );

// MeSetup.c
VOID
InitMeSetupCallback (
  VOID
  );

VOID
MeExtractConfig ();

// IccSetup.c
VOID
InitIccSetupCallback (
  VOID
  );

VOID
IccExtractConfig (
  VOID
  );

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif
