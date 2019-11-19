/** @file
  Driver for Amt INT16 support.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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

#include "AmtInt16.h"

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS      mInt16TSR;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                     mInt16TSR_Pages;

#define INT16TSR_INIT       0
#define INT16TSR_HOOK       1
#define INT16TSR_RELEASE    2

BOOLEAN
AmtIsConsoleLocked (
  VOID
  )
{
  BOOLEAN                 KeyboardLocked = FALSE;
  EFI_STATUS              Status;
  AMT_WRAPPER_PROTOCOL    *AmtWrapper;

  Status = gBS->LocateProtocol (&gEfiAmtWrapperProtocolGuid, NULL, (VOID **) &AmtWrapper);
  if (!EFI_ERROR (Status)){
    if (AmtWrapper->ActiveManagementIsConsoleLocked ()) {
      KeyboardLocked = TRUE;
    }
  }
  return KeyboardLocked;
}

EFI_STATUS
LoadRealModeFile (
  EFI_GUID    *Guid,
  VOID        **Buffer,
  UINTN       *BufferSize
  )
{
  EFI_STATUS                      Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL   *Fv;
  UINT32                          Authentication;
  UINTN                           NumHandles;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           i;

  *Buffer=0;
  *BufferSize=0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status))
    return Status;

  for (i = 0; i < NumHandles; ++i) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (EFI_ERROR (Status)) continue;

    Status = Fv->ReadSection (
                   Fv,
                   Guid,
                   EFI_SECTION_RAW,
                   0,
                   Buffer,
                   BufferSize,
                   &Authentication
                   );

    if (Status == EFI_SUCCESS)
      break;
  }

  gBS->FreePool (HandleBuffer);

  return Status;
}

VOID
CallINT16TSR (
  UINT16  Function,
  UINT16  P1,
  UINT16  P2,
  UINT16  P3
  )
{
  EFI_IA32_REGISTER_SET     RegSet;
  UINT16                    ip, cs;
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  ASSERT_EFI_ERROR(Status);

  cs = (UINT16)(RShiftU64 (mInt16TSR, 4));
  ip = (UINT16)(mInt16TSR & 0xF);
  ip = ip + *(UINT16 *)(UINTN)mInt16TSR;

  gBS->SetMem (&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
  RegSet.X.AX = Function;
  RegSet.X.BX = P1;
  RegSet.X.CX = P2;
  RegSet.X.DX = P3;

  LegacyBios->FarCall86 (
                LegacyBios,
                cs,
                ip,
                &RegSet,
                NULL,
                0
                );
}

VOID
EFIAPI
AmtINT16LegacyBoot(
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
  )
{
  DEBUG ((DEBUG_INFO, "<INT16TSR_RELEASE>"));
  CallINT16TSR (INT16TSR_RELEASE << 8,0,0,0);
  DEBUG ((DEBUG_INFO, "<INT16TSR_FREE>"));
  gBS->FreePages (mInt16TSR, mInt16TSR_Pages);
  gBS->CloseEvent (Event);
}

EFI_STATUS
EFIAPI
AmtInt16Entry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS    Status;
  UINT8         *Buffer;
  UINTN         BufferSize;
  EFI_EVENT     LegacyBootEvent;

  DEBUG((DEBUG_INFO, "<AmtInt16Entry>"));

  if (AmtIsConsoleLocked ()) {
    Status = LoadRealModeFile (&gAmtInt16CsmGuid, (VOID **) &Buffer, &BufferSize);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "<FAILED>"));
    } else {
      DEBUG ((DEBUG_INFO, "<Buffer=0x%X,0x%X>", (UINTN)Buffer, BufferSize));

      mInt16TSR = 0x98000;
      mInt16TSR_Pages = BufferSize / 0x1000 + 1;
      Status = gBS->AllocatePages (
                      AllocateMaxAddress,
                      EfiBootServicesData,
                      mInt16TSR_Pages,
                      &mInt16TSR
                      );
      if (EFI_ERROR (Status))
        return Status;

      DEBUG ((DEBUG_INFO, "<Address=0x%X>", mInt16TSR));

      gBS->CopyMem ((VOID *)(UINTN)mInt16TSR, Buffer, BufferSize);

      DEBUG ((DEBUG_INFO, "<INT16TSR_INIT>"));
      CallINT16TSR (INT16TSR_INIT << 8,0,0,0);

      DEBUG ((DEBUG_INFO, "<INT16TSR_HOOK>"));
      CallINT16TSR(INT16TSR_HOOK << 8,0,0,0);

      Status = EfiCreateEventLegacyBootEx (
                 TPL_CALLBACK,
                 AmtINT16LegacyBoot,
                 (VOID *)&ImageHandle,
                 &LegacyBootEvent
                 );
      ASSERT_EFI_ERROR (Status);

      gBS->FreePool(Buffer);
    }
  }

  return EFI_SUCCESS;
}
