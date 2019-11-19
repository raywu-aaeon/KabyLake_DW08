/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2008 - 2016 Intel Corporation.

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

#include "DxeMePolicyUpdate.h"

//
// Record version
//
#define RECORD_REVISION_1              0x01
#define MAX_FW_UPDATE_BIOS_SELECTIONS  2

//
// Function implementations executed during policy initialization phase
//
/**
  This is to display localized message in the console. This is
  used to display message strings in local language. To display
  the message, the routine will check the message ID and ConOut
  the message strings.

  @param[in] MsgId                Me error message ID for displaying on screen message

**/
VOID
EFIAPI
ShowMeReportError (
  IN ME_ERROR_MSG_ID              MsgId
  )
{
  UINTN            MsgDelay;

  MsgDelay = HECI_MSG_DELAY;
  gST->ConOut->ClearScreen (gST->ConOut);

  switch (MsgId) {
  case MSG_EOP_ERROR:
    gST->ConOut->OutputString (gST->ConOut, L"Error sending End Of Post message to ME: HECI disabled, proceeding with boot!\n");
    break;

  case MSG_ME_FW_UPDATE_FAILED:
    gST->ConOut->OutputString (gST->ConOut, L"ME FW Update Failed, please try again!\n");
    break;

  case MSG_ASF_BOOT_DISK_MISSING:
    gST->ConOut->OutputString (gST->ConOut, L"Boot disk missing, please insert boot disk and press ENTER\r\n");
    break;

  case MSG_KVM_TIMES_UP:
    gST->ConOut->OutputString (gST->ConOut, L"Error!! Times up and the KVM session was cancelled!!");
    break;

  case MSG_KVM_REJECTED:
    gST->ConOut->OutputString (gST->ConOut, L"Error!! The request has rejected and the KVM session was cancelled!!");
    break;

  case MSG_HMRFPO_LOCK_FAILURE:
    gST->ConOut->OutputString (gST->ConOut, L"(A7) Me FW Downgrade - Request MeSpiLock Failed\n");
    break;

  case MSG_HMRFPO_UNLOCK_FAILURE:
    gST->ConOut->OutputString (gST->ConOut, L"(A7) Me FW Downgrade - Request MeSpiEnable Failed\n");
    break;

  case MSG_ME_FW_UPDATE_WAIT:
    gST->ConOut->OutputString (gST->ConOut, L"Intel(R) Firmware Update is in progress. It may take up to 90 seconds. Please wait.\n");
    break;

  case MSG_ILLEGAL_CPU_PLUGGED_IN:
    gST->ConOut->OutputString (gST->ConOut, L"\n\n\rAn unsupported CPU/PCH configuration has been identified.\n");
    gST->ConOut->OutputString (gST->ConOut, L"\rPlease refer to the Platform Validation Matrix\n\rfor supported CPU/PCH combinations.");
    break;

  case MSG_KVM_WAIT:
    gST->ConOut->OutputString (gST->ConOut, L"Waiting Up to 8 Minutes For KVM FW.....");
    break;

  case MSG_PLAT_DISABLE_WAIT:
    gST->ConOut->OutputString (gST->ConOut, L"WARNING! Firmware encountered errors and will reboot the platform in 30 minutes.");
    MsgDelay = 5 * HECI_MSG_DELAY;
    break;

  default:
    DEBUG ((DEBUG_ERROR, "This Message Id hasn't been defined yet, MsgId = %x\n", MsgId));
    break;
  }

  gBS->Stall (MsgDelay);
}

/**
  Update the ME Policy Library

  @param[in, out] DxeMePolicy           The pointer to get ME Policy protocol instance

  @retval EFI_SUCCESS                   Initialization complete.
  @retval EFI_UNSUPPORTED               The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES          Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR              Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
UpdateDxeMePolicy (
  IN OUT  ME_POLICY_PROTOCOL            *DxeMePolicy
  )
{
  EFI_STATUS              Status;
  EFI_EVENT               EndOfDxeEvent;
  ME_DXE_CONFIG           *MeDxeConfig;

  DEBUG ((DEBUG_INFO, "UpdateDxeMePolicy\n"));
  UpdateMePolicyFromSetup (DxeMePolicy);
  UpdateMePolicyFromMeSetup (DxeMePolicy);

  Status = GetConfigBlock ((VOID *) DxeMePolicy, &gMeDxeConfigGuid, (VOID *) &MeDxeConfig);
  if (!EFI_ERROR (Status)) {
    MeDxeConfig->MeReportError = ShowMeReportError;
  }

  //
  // Register End of DXE event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UpdateMeSetupCallback,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Update ME Policy while MePlatformProtocol is installed.

  @param[in] MePolicyInstance     Instance of ME Policy Protocol

**/
VOID
UpdateMePolicyFromMeSetup (
  IN ME_POLICY_PROTOCOL           *MePolicyInstance
  )
{
  EFI_STATUS              Status;
  ME_SETUP                MeSetup;
  ME_SETUP                *MeSetupPtr;
  ME_SETUP_STORAGE        MeSetupStorage;
  ME_SETUP_STORAGE        *MeSetupStoragePtr;
  UINTN                   Size;
  ME_DXE_CONFIG           *MeDxeConfig;

  DEBUG ((DEBUG_INFO, "UpdateMePolicyFromMeSetup\n"));

  Size = sizeof (ME_SETUP_STORAGE);
  Status = gRT->GetVariable (L"MeSetupStorage", &gMeSetupVariableGuid, NULL, &Size, &MeSetupStorage);
  if (EFI_ERROR (Status)) {
    MeSetupStoragePtr = NULL;
  } else {
    MeSetupStoragePtr = &MeSetupStorage;
  }

  //
  // Get Me configuration from Setup Data
  //
  Size = sizeof (ME_SETUP);
  Status = gRT->GetVariable (L"MeSetup", &gMeSetupVariableGuid, NULL, &Size, &MeSetup);
  if (EFI_ERROR (Status)) {
    MeSetupPtr = NULL;
  } else {
    MeSetupPtr = &MeSetup;
  }

  Status = GetConfigBlock ((VOID *) MePolicyInstance, &gMeDxeConfigGuid, (VOID *) &MeDxeConfig);
  ASSERT_EFI_ERROR (Status);

  if (MeSetupStoragePtr != NULL) {
    MeDxeConfig->MeLocalFwUpdEnabled  = MeSetupStoragePtr->LocalFwUpdEnabled;
  }

  if (MeSetupPtr != NULL) {
    MeDxeConfig->MeFwDownGrade        = MeSetupPtr->MeFwDowngrade;
    MeDxeConfig->OsPtpAware           = MeSetupPtr->OsPtpAware;
    MeDxeConfig->MeJhiSupport         = MeSetupPtr->MeJhiSupport;
    MeDxeConfig->CoreBiosDoneEnabled  = MeSetupPtr->CoreBiosDoneEnabled;
  }

  DEBUG ((DEBUG_INFO, "UpdateMePolicyFromMeSetup done, Status: %r\n", Status));
}

/**
  Update ME Policy if Setup variable exists.

  @param[in, out] MePolicyInstance     Instance of ME Policy Protocol

**/
VOID
UpdateMePolicyFromSetup (
  IN OUT ME_POLICY_PROTOCOL     *MePolicyInstance
  )
{
  EFI_STATUS                    Status;

  DEBUG ((DEBUG_INFO, "UpdateMePolicyFromSetup\n"));
  Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "UpdateMePolicyFromSetup done, Status: %r\n", Status));
}

/**
  Functions performs HECI exchange with FW to update MePolicy settings.

  @param[in] Event         A pointer to the Event that triggered the callback.
  @param[in] Context       A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
UpdateMeSetupCallback (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *Context
  )
{
  EFI_STATUS                      Status;
  ME_SETUP                        MeSetup;
  ME_INFO_SETUP_DATA              MeInfoSetupData;
  ME_BIOS_PAYLOAD_HOB             *MbpHob;
  HECI_PROTOCOL                   *Heci;
#ifdef PERFORMANCE_FLAG
  FPDT_PERFORMANCE_PROTOCOL       *FpdtProtocol;
  PLATFORM_BOOT_PERFORMANCE_TABLE *Buffer;
#endif
  UINTN                           Size;
  UINT32                          SetupAttr;
  UINT32                          MeMode;
  ME_PEI_CONFIG                   *MePeiConfig;
  UINT32                          UnconfigOnRtcClear;
  EFI_HOB_GUID_TYPE               *GuidHob;

  DEBUG ((DEBUG_INFO, "UpdateMeSetup event start\n"));

  //
  // Default ME Mode
  //
  MeMode             = ME_MODE_FAILED;
  MbpHob             = NULL;
  GuidHob            = NULL;
  UnconfigOnRtcClear = 0;

  ZeroMem (&MeInfoSetupData, sizeof(MeInfoSetupData));
  //
  // Get Mbp Hob
  //
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob == NULL) {
    MeInfoSetupData.MeFirmwareInfo = ME_MODE_FAILED;
  } else {
    MeInfoSetupData.MeMajor      = MbpHob->MeBiosPayload.FwVersionName.MajorVersion;
    MeInfoSetupData.MeMinor      = MbpHob->MeBiosPayload.FwVersionName.MinorVersion;
    MeInfoSetupData.MeHotFix     = MbpHob->MeBiosPayload.FwVersionName.HotfixVersion;
    MeInfoSetupData.MeBuildNo    = MbpHob->MeBiosPayload.FwVersionName.BuildVersion;
// AMI_OVERRIDE_START - Fixed ME INFO Error in setup.
    MeInfoSetupData.MeFwType     = MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;
// AMI_OVERRIDE_END - Fixed ME INFO Error in setup.
    MeInfoSetupData.MeFileSystem = MbpHob->MeBiosPayload.MFSIntegrity;
  }

  Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, (VOID **) &Heci);
  if (!EFI_ERROR (Status)) {
    Status = Heci->GetMeMode (&MeMode);
    ASSERT_EFI_ERROR(Status);

    MeInfoSetupData.MeFirmwareInfo = (UINT8) MeMode;
  }

  Status = gRT->SetVariable (
                  L"MeInfoSetup",
                  &gMeInfoSetupGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof(MeInfoSetupData),
                  &MeInfoSetupData
                  );
  ASSERT_EFI_ERROR(Status);
#ifdef PERFORMANCE_FLAG
  //
  // Locate Fpdt Protocol to allow inserting new record
  //
  Status = gBS->LocateProtocol (&gFpdtPerformanceProtocolGuid, NULL, (VOID **) &FpdtProtocol);

  if (!EFI_ERROR(Status) && (MbpHob != NULL) && MbpHob->MeBiosPayload.PlatBootPerfData.Available) {
    //
    // Protocol is available and Platform boot information is also available in MBP
    //

    //
    // Create Platform Boot Performance Table and populate fields from MBP
    // All timestamp values are required to be in nanosec unit for FPDT table
    //
    Status = (gBS->AllocatePool) (EfiReservedMemoryType, sizeof (PLATFORM_BOOT_PERFORMANCE_TABLE), (VOID **) &Buffer);
    ASSERT_EFI_ERROR (Status);
    (gBS->SetMem) (Buffer, sizeof (PLATFORM_BOOT_PERFORMANCE_TABLE), 0);

    Buffer->Header.Signature = PLATFORM_BOOT_TABLE_SIGNATURE;
    Buffer->Header.Length = sizeof(PLATFORM_BOOT_PERFORMANCE_TABLE);
    Buffer->PlatformBoot.Type = PLATFORM_BOOT_RECORD_TYPE;
    Buffer->PlatformBoot.Length = sizeof(PLATFORM_BOOT_TABLE_RECORD);
    Buffer->PlatformBoot.Revision = RECORD_REVISION_1;
    Buffer->PlatformBoot.TimestampDelta1 = (UINT64)((MbpHob->MeBiosPayload.PlatBootPerfData.MbpPerfData.PwrbtnMrst) * CONVERSION_MULTIPLIER);
    Buffer->PlatformBoot.TimestampDelta2 = (UINT64)((MbpHob->MeBiosPayload.PlatBootPerfData.MbpPerfData.MrstPltrst) * CONVERSION_MULTIPLIER);
    Buffer->PlatformBoot.TimestampDelta3 = (UINT64)((MbpHob->MeBiosPayload.PlatBootPerfData.MbpPerfData.PltrstCpurst) * CONVERSION_MULTIPLIER);

    Status = FpdtProtocol->UpdateRecord (FpdtProtocol, NULL, PLATFORM_BOOT_TABLE_PTR_TYPE, (UINT64)Buffer, 0);
    ASSERT_EFI_ERROR (Status);
  }
#endif
  SetupAttr = 0;
  Size = sizeof (ME_SETUP);
  Status = gRT->GetVariable (
                  L"MeSetup",
                  &gMeSetupVariableGuid,
                  &SetupAttr,
                  &Size,
                  &MeSetup
                  );
  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR(Status)) {
    //
    // FW Downgrade BIOS Setup Option is only valid for one boot
    //
    MeSetup.MeFwDowngrade = 0;

    //
    // Reset Firmware Info
    //
    MeSetup.MeFirmwareInfo = MeInfoSetupData.MeFirmwareInfo;

    MeSetup.UnconfigOnRtcAvailable = 0;
    MeSetup.NfcDevice              = 0;
    MeSetup.NfcAvailable           = 0;
    MeSetup.MeImageType            = 0;
    MeSetup.Heci3Enabled           = 0;
    if (MbpHob != NULL) {
      //
      // Update NFC settings
      //
      if (MbpHob->MeBiosPayload.NfcSupport.Available) {
        MeSetup.NfcDevice = (UINT8)MbpHob->MeBiosPayload.NfcSupport.NfcData.DeviceType;

        if (MbpHob->MeBiosPayload.FwCapsSku.Available && MbpHob->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.NFC == 1) {
          MeSetup.NfcAvailable = 1;
        }
      }

      //
      // Set Image Type for MEBx disposition
      //
      MeSetup.MeImageType = (UINT8) MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;

      //
      // Update ME Unconfig on RTC Clear availability
      //
      if (MbpHob->MeBiosPayload.UnconfigOnRtcClearState.Available) {
        MeSetup.UnconfigOnRtcAvailable = 1;
        GuidHob = GetFirstGuidHob (&gMePolicyHobGuid);
        if (GuidHob != NULL) {
          MePeiConfig = GET_GUID_HOB_DATA (GuidHob);
          if (MePeiConfig->MeUnconfigIsValid == 0) {
            //
            // ME Unconfig item in setup menu could be destroyed because of CMOS clean.
            // If item is not valid, restore it from DisUnconfigOnRtcClearState.
            //
            UnconfigOnRtcClear = MbpHob->MeBiosPayload.UnconfigOnRtcClearState.UnconfigOnRtcClearData.DisUnconfigOnRtcClearState;
            MeSetup.MeUnconfigOnRtcClear = (UnconfigOnRtcClear == EnableState)? 1 : 0;
          }
        }
      } else {
        Status = HeciGetUnconfigOnRtcClearDisableMsg (&UnconfigOnRtcClear);
        if (!EFI_ERROR (Status)) {
          MeSetup.MeUnconfigOnRtcClear = (UnconfigOnRtcClear == EnableState)? 1 : 0;
        }
      }

      //
      // Retrieve IntegratedTouch capability to report whether HECI3 should be enabled for S3 boot mode.
      //
      if ((MbpHob->MeBiosPayload.FwCapsSku.Available == 1) &&
          (MbpHob->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.IntegratedTouch == 1)) {
        MeSetup.Heci3Enabled = 1;
      }
    }

    //
    // Update AMT settings
    //
// AMI_OVERRIDE_START - For iAMT Support.
#if defined iAMT_SUPPORT && iAMT_SUPPORT == 1    
    MeSetup.UnConfigureMe  = 0;
    MeSetup.AmtCiraRequest = 0;
#endif
// AMI_OVERRIDE_END - For iAMT Support.

    Status = gRT->SetVariable (
                    L"MeSetup",
                    &gMeSetupVariableGuid,
                    SetupAttr,
                    sizeof (ME_SETUP),
                    &MeSetup
                    );
    ASSERT_EFI_ERROR (Status);
  }
  gBS->CloseEvent (Event);

  return;
}

