/** @file
  Source code file for the Silicon Init DXE module

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

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/Smbios.h>
#include <Protocol/SiPolicyProtocol.h>
#include <Private/Library/SmbiosMemoryLib.h>
#include <Private/Library/SiFviInitLib.h>

//
// Module-wide global variables
//
VOID       *mRegistration;
EFI_EVENT  EndOfDxeEvent;

/**
  This function gets registered as a callback to perform

  @param[in]  Event     - A pointer to the Event that triggered the callback.
  @param[in]  Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
SiInitDxeSmbiosCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS          Status;
  EFI_SMBIOS_PROTOCOL *SmbiosProtocol;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error locating gEfiSmbiosProtocolGuid at SiInitDxe. Status = %r\n", Status));
    return;
  }

  Status = SmbiosMemory (SmbiosProtocol);
//  AMI_OVERRIDE_SIPKG0002_START >>> For capsule debug mode assert. Capsule mode will not insatll SmbiosProtocol. So it's normal not need assert. 
//  ASSERT_EFI_ERROR (Status);
//  AMI_OVERRIDE_SIPKG0002_END <<<

  return;
}


/**
  This function handles PlatformInit task at the end of DXE

  @param[in]  Event
  @param[in]  *Context
**/
VOID
EFIAPI
SiInitEndOfDxe (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                 Status;
  DXE_SI_POLICY_PROTOCOL     *SiPolicyData;

  Status = gBS->LocateProtocol (&gDxeSiPolicyProtocolGuid, NULL, (VOID **) &SiPolicyData);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Failed to locate DxeSiPolicyProtocolGuid Protocol\n"));
    return;
  }

  CpuFviInit (SiPolicyData->SmbiosOemTypeFirmwareVersionInfo);
  MeFviInit (SiPolicyData->SmbiosOemTypeFirmwareVersionInfo);
  PchFviInit (SiPolicyData->SmbiosOemTypeFirmwareVersionInfo);
  SaFviInit (SiPolicyData->SmbiosOemTypeFirmwareVersionInfo);

  gBS->CloseEvent (Event);

  return;
}


/**
  Entry point for the driver.

  @param[in]  ImageHandle  Image Handle.
  @param[in]  SystemTable  EFI System Table.

  @retval     EFI_SUCCESS  Function has completed successfully.
  @retval     Others       All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
SiInitDxe (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_EVENT           CallbackEvent;
  EFI_SMBIOS_PROTOCOL *Smbios;

  //
  // If EfiSmbiosProtocol already installed, invoke the callback directly.
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  if (!EFI_ERROR (Status)) {
    SiInitDxeSmbiosCallback (NULL,NULL);
  } else {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    SiInitDxeSmbiosCallback,
                    NULL,
                    &CallbackEvent
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (
                    &gEfiSmbiosProtocolGuid,
                    CallbackEvent,
                    &mRegistration
                    );
    ASSERT_EFI_ERROR (Status);

  }

  ///
  /// Performing SiInitEndOfDxe after the gEfiEndOfDxeEventGroup is signaled.
  ///
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SiInitEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
