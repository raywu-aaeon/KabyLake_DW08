/**@file
Secure Erase Implementation.

@copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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

#include "SecureErase.h"
#include "Ui.h"
#include "AtaSecureErase.h"
#include "NvmeSecureErase.h"
#include "SimulatedSecureErase.h"
#include <Library/HobLib.h>
#include <MeBiosPayloadHob.h>
#include <SetupVariable.h>
#include <Protocol/AmtWrapperProtocol.h>
#include <Library/PcdLib.h>

SETUP_DATA                                mSetupData;
UINT32                                    mSetupDataAttr = 0;

AMT_READY_TO_BOOT_PROTOCOL mSecureEraseReadyToBoot = {
  AMT_READY_TO_BOOT_PROTOCOL_REVISION_1,
  SecureEraseOnReadyToBootEntryPoint
};

/**
  Get Setup Variable from NVM

  @retval EFI_SUCCESS      Data retrieved
  @retval Others           Data was not retrieved
**/
EFI_STATUS
GetSetupData (
  VOID
  )
{
  EFI_STATUS                           Status;
  UINTN                                VarSize = sizeof (SETUP_DATA);

  DEBUG ((DEBUG_INFO, "SecureErase::GetSetupData : "));
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  &mSetupDataAttr,
                  &VarSize,
                  &mSetupData
                  );
  DEBUG ((DEBUG_INFO, "Status=%r\n", Status));
  return Status;
}

/**
  Save Setup Variable to NVM

  @retval EFI_SUCCESS      Data saved successfully
  @retval Others           Data was not retrieved
**/
EFI_STATUS
SaveSetupData (
  VOID
  )
{
  EFI_STATUS                           Status;
  UINTN                                VarSize = sizeof (SETUP_DATA);

  Status = gRT->SetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  mSetupDataAttr,
                  VarSize,
                  &mSetupData
                  );

  DEBUG ((DEBUG_INFO, "SecureErase::SaveSetupData : Status=%r\n", Status));
  return Status;
}

/**
  Check if Corporate firmware is installed

  @retval EFI_SUCCESS                  The firmware is Corporate
  @retval EFI_NOT_READY                Can't locate required data
  @retvak EFI_UNSUPPORTED              The firmware is NOT Corporate
**/
EFI_STATUS
IsCorporateFwInstalled (
  VOID
  )
{
  ME_BIOS_PAYLOAD_HOB                 *MbpHob = NULL;

  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);

  if (MbpHob == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to obtain hob : gMeBiosPayloadHobGuid\n"));
    return EFI_NOT_READY;
  }

  if (!MbpHob->MeBiosPayload.FwPlatType.Available) {
    DEBUG ((DEBUG_INFO, "mMbpHob->MeBiosPayload.FwPlatType.Available is FALSE\n"));
    return EFI_NOT_READY;
  }

  if (MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType != IntelMeCorporateFw) {
    DEBUG ((DEBUG_INFO, "IntelMeFwImageType != INTEL_ME_CORPORATE_FW\n"));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Function checks current setup to see if required functionality is avalaible

  @retval TRUE                Firmware supports required functionality
  @retval FALSE               Firmware Image type is not Corporate
                              or there is no ASF/AMT support
**/
BOOLEAN
IsFeatureSupportedByFw (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "SecureErase::IsFeatureSupportedByFw\n"));

  if (!IsAmtBiosSupportEnabled () || !IsAsfBiosSupportEnabled ()) {
    DEBUG ((DEBUG_INFO, "Either AMT or ASF is not supported\n"));
    return FALSE;
  }

  DEBUG ((DEBUG_INFO, "Feature is supported\n"));

  return TRUE;
}

/**
  Checks if Secure Erase has been requested using boot options

  @retval           TRUE
  @retval           FALSE    Either secure erase hasn't been requested or there
                             is no AMT_WRAPPER_PROTOCOL present on the system
**/
BOOLEAN
IsSecureEraseRequested (
  VOID
  )
{
  EFI_STATUS                           Status;
  AMT_WRAPPER_PROTOCOL                 *AmtWrapper;

  DEBUG ((DEBUG_INFO, "SecureErase :: IsSecureEraseRequested\n"));

  Status = gBS->LocateProtocol (&gEfiAmtWrapperProtocolGuid, NULL, (VOID **) &AmtWrapper);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "SecureErase :: IsSecureEraseRequested :: No AMT Wrapper\n"));
  } else {
      if (AmtWrapper->ActiveManagementIsSecureEraseEnabled ()) {
      DEBUG ((DEBUG_INFO, "SecureErase :: IsSecureEraseRequested : Requested by AMT\n"));
      return TRUE;
    }
  }

  Status = GetSetupData ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "SecureErase :: IsSecureEraseRequested :: Can't get Setup Data\n"));
  } else if (mSetupData.ForceSecureErase) {
    DEBUG ((DEBUG_INFO, "SecureErase :: IsSecureEraseRequested : Requested by SetupData\n"));
    return TRUE;
  }

  DEBUG ((DEBUG_INFO, "SecureErase :: IsSecureEraseRequested : Secure Erase not requested\n"));
  return FALSE;
}

/**
  Attempts to erase ATA and NVMe drives.

  @retval EFI_SUCCESS         Erase device succeed
  @retval EFI_UNSUPPORTED     The device is not supported
  @retval EFI_ACCESS_DENIED   User has entered wrong password too many times
  @retval EFI_ABORTED         The device is supported, but the system
                              has failed to erase it
**/
EFI_STATUS
EraseDevices (
  VOID
  )
{
  EFI_STATUS                           Status;
  EFI_TPL                              OldTpl;

  DEBUG ((DEBUG_INFO, "SecureErase :: EraseDevice\n"));
  OldTpl = EfiGetCurrentTpl ();

  gBS->RestoreTPL (TPL_APPLICATION);
  InitUiLib ();
  SetScreenTitle (L"Secure erase");
  ClearScreen ();

  Status = GetSetupData ();
  if ((!EFI_ERROR (Status)) && mSetupData.SecureEraseModeRealMode == TRUE) {
    DEBUG ((DEBUG_INFO, "SecureErase in progress\n"));
    Status = EraseAtaDevice ();
    DEBUG ((DEBUG_INFO, "AtaStatus = %r\n", Status));
    if (!EFI_ERROR (Status)) {
      Status = EraseNvmeDevice ();
      DEBUG ((DEBUG_INFO, "NvmeStatus = %r\n", Status));
    }
  } else {
    Status = SimulateSecureErase ();
  }

  if (Status == EFI_SUCCESS) {
    if (mSetupData.ForceSecureErase == TRUE) {
      mSetupData.ForceSecureErase = FALSE;
      SaveSetupData ();
    }
    ClearBootOptions ();
  }

  DEBUG ((DEBUG_INFO, "SecureErase :: Report Secure Erase Operation Status: %r\nSecureErase :: EraseDevices end\n", Status));
  SendRsePetAlert (Status);
  ReportBiosStatus (Status);

  if (mSetupData.SecureEraseModeRealMode == FALSE) {
    //
    // If demo mode is enabled, don't restart. Use dead loop to make sure screen keeps
    // displaying "missing operating system" message from SimulatedSecureErase.c forever.
    //
    CpuDeadLoop ();
  }

  PrintSummary (Status);

  gBS->RaiseTPL (OldTpl);
  return Status;
}


/**
  This function is executed on AMT Ready to boot and performs Secure Erase.

  @retval EFI_SUCCESS        Function was executed.
**/
EFI_STATUS
EFIAPI
SecureEraseOnReadyToBootEntryPoint (
  VOID
  )
{
  EFI_STATUS                           Status;

  DEBUG ((DEBUG_INFO, "SecureErase :: SecureEraseOnReadyToBootEntryPoint\n"));

  Status = EraseDevices ();

  gRT->ResetSystem (EfiResetShutdown, Status, 0, NULL);
  ASSERT (TRUE);

  return Status;
}

// AMI_OVERRIDE_START >>> EIP307386
#define BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID \
 { 0xDBC9FD21, 0xFAD8, 0x45B0, 0x9E, 0x78, 0x27, 0x15, 0x88, 0x67, 0xCC, 0x93 }

VOID 
EFIAPI
SecureEraseEvent(
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)
{
    SecureEraseOnReadyToBootEntryPoint();
}

EFI_STATUS
RegisterDriverConnectedEvent(
    IN  VOID
)
{
    EFI_STATUS  Status;
    EFI_GUID    AllDriverConnectedGuid = BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID;
    
    Status = EfiNamedEventListen(
                &AllDriverConnectedGuid,
                TPL_CALLBACK,
                SecureEraseEvent,
                NULL,
                NULL );

    return Status;
}
// AMI_OVERRIDE_END <<< EIP307386

/**
  Drivers entry point. Checks if secure erase has been requested via boot options,
  if so, starts the procedure.

  @param[in] ImageHandle      The firmware allocated handle for the EFI image.
  @param[in] SystemTable      A pointer to the EFI System Table.

  @retval EFI_SUCCESS         Secure erase procedure has been started
  @retval EFI_ABORTED         Failed to register ready to boot event
  @retval EFI_UNSUPPORTED     Feature is not supported by the FW
**/
EFI_STATUS
EFIAPI
SecureEraseEntryPoint (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                 *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;

  DEBUG ((DEBUG_INFO, "SecureErase :: SecureEraseEntryPoint\n"));

  Status = IsCorporateFwInstalled ();
  DEBUG ((DEBUG_INFO, "IsCorporateFwInstalled -> status=%r\n",Status));
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (!IsFeatureSupportedByFw ()) {
    return EFI_UNSUPPORTED;
  }

  if (!IsSecureEraseRequested ()) {
    return EFI_ABORTED;
  }
  //
  // Secure Erase is requested, so this module will take control over HDD's
  // unlocking process
  //
  PcdSetBoolS (PcdSkipHddPasswordPrompt, TRUE);
  
// AMI_OVERRIDE_START >>> EIP307386
  //Move secure erase from AMT ready to boot to all driver connected
  Status = RegisterDriverConnectedEvent();
  ASSERT_EFI_ERROR (Status);
  return EFI_SUCCESS;
// AMI_OVERRIDE_END <<< EIP307386

  //
  // Install an Amt ready to boot protocol.
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gAmtReadyToBootProtocolGuid,
                  &mSecureEraseReadyToBoot,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
