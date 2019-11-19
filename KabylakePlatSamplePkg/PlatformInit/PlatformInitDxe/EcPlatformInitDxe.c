/** @file
  Source code file for the Platform Init DXE module

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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
/**
  Check Lid and Base EC Protocol Version
**/

#include "PlatformInitDxe.h"
#include <Include/Protocol/SimpleTextIn.h>
#include <Include/Library/UefiLib.h>


/**
  Verifies LID base EC protocol version

**/
VOID
EFIAPI
CheckLidBaseEcProtocolVersion (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_INPUT_KEY     Key;
  CHAR16            *StringBuffer1 = NULL;
  CHAR16            *StringBuffer2 = NULL;
  CHAR16            *StringBuffer3 = NULL;
  CHAR16            *StringBuffer4 = NULL;
  UINT8             Timer;
  CHAR16            *String = NULL;
  PLATFORM_INFO     *PlatformInfo;
  UINTN             StringBuffSize;

  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
  if (EFI_ERROR (Status)) {
    return ;
  }

  if ((PlatformInfo->BoardType == BoardTypeSds) &&
      (PlatformInfo->DockAttached == 1) &&
      (PlatformInfo->EcProtocolVersion != PlatformInfo->BaseEcProtocolVersion)) {
    if (gST->ConOut != NULL && gST->ConIn != NULL) {
      //
      // Popup a menu to notice user
      //
      gST->ConOut->ClearScreen (gST->ConOut);

      String = AllocateZeroPool (2 * sizeof (CHAR16));
      ASSERT (String != NULL);
      if (String == NULL) {
        goto EXIT;
      }

      StringBuffer1 = AllocateZeroPool (200 * sizeof (CHAR16));
      ASSERT (StringBuffer1 != NULL);
      if (StringBuffer1 == NULL) {
        goto EXIT;
      }

      StringBuffer2 = AllocateZeroPool (200 * sizeof (CHAR16));
      ASSERT (StringBuffer2 != NULL);
      if (StringBuffer2 == NULL) {
        goto EXIT;
      }

      StringBuffer3 = AllocateZeroPool (200 * sizeof (CHAR16));
      ASSERT (StringBuffer3 != NULL);
      if (StringBuffer3 == NULL) {
        goto EXIT;
      }

      StringBuffer4 = AllocateZeroPool (200 * sizeof (CHAR16));
      ASSERT (StringBuffer4 != NULL);
      if (StringBuffer4 == NULL) {
        goto EXIT;
      }

      StringBuffSize = (200 * sizeof (CHAR16)) / sizeof (CHAR16);
// AMI_OVERRIDE_START - Fix Linux build error.	 
#if 0 
      StrCpyS (StringBuffer1, StringBuffSize, L"!!! WARNING: Lid and Base EC Protocol version mismatch !!!");
      StrCpyS (StringBuffer2, StringBuffSize, L"EC Protocol version: ");
      UnicodeValueToString (String, PREFIX_ZERO | RADIX_HEX, PlatformInfo->EcProtocolVersion, 2);
      StrCatS (StringBuffer2, StringBuffSize, String);
      StrCpyS (StringBuffer3, StringBuffSize, L"Base EC Protocol version: ");
      UnicodeValueToString (String, PREFIX_ZERO | RADIX_HEX, PlatformInfo->BaseEcProtocolVersion, 2);
      StrCatS (StringBuffer3, StringBuffSize, String);
      StrCpyS (StringBuffer4, StringBuffSize, L"Press <ENTER> to continue.");
#else
      StrnCpy (StringBuffer1, L"!!! WARNING: Lid and Base EC Protocol version mismatch !!!", StringBuffSize - 1);
      StrnCpy (StringBuffer2, L"EC Protocol version: ", StringBuffSize - 1);
      UnicodeValueToString (String, PREFIX_ZERO | RADIX_HEX, PlatformInfo->EcProtocolVersion, 2);
      StrnCat (StringBuffer2, String, StringBuffSize - StrLen (StringBuffer2) - 1);
      StrnCpy (StringBuffer3, L"Base EC Protocol version: ", StringBuffSize - 1);
      UnicodeValueToString (String, PREFIX_ZERO | RADIX_HEX, PlatformInfo->BaseEcProtocolVersion, 2);
      StrnCat (StringBuffer3, String, StringBuffSize - StrLen (StringBuffer3) - 1);
      StrnCpy (StringBuffer4, L"Press <ENTER> to continue.", StringBuffSize - 1);
#endif	  
// AMI_OVERRIDE_END - Fix Linux build error.	  

      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_RED, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);

      //
      // Wait for a keystroke
      //
      Key.UnicodeChar = 0;
      Timer           = 30;

      do {
        Status = gBS->CheckEvent (gST->ConIn->WaitForKey);
        if (!EFI_ERROR (Status)) {
          Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
        }
        gBS->Stall (1000000);
        Timer --;
        if (Timer == 0) {
          gST->ConOut->ClearScreen (gST->ConOut);
          return;
        }
      } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

      gST->ConOut->ClearScreen (gST->ConOut);
    }
  }

EXIT:

  if (StringBuffer1 != NULL) {
    FreePool (StringBuffer1);
  }

  if (StringBuffer2 != NULL) {
    FreePool (StringBuffer2);
  }

  if (StringBuffer3 != NULL) {
    FreePool (StringBuffer3);
  }

  if (StringBuffer4 != NULL) {
    FreePool (StringBuffer4);
  }

  if (String != NULL) {
    FreePool (String);
  }

  return;
}