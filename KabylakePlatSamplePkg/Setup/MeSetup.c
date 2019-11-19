/**@file
ME Setup Implementation

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2008 - 2017 Intel Corporation.

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

#include <SetupPrivate.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/AmtWrapperProtocol.h>
#include <Library/Tpm2CommandLib.h>
#include <MeSetup.h>
#include <OemSetup.h>
#include <PchAccess.h>
#include <MeChipset.h>
#include <Library/DxeMeLib.h>
#include <Library/PttHeciLib.h>
#include <TcgSetup.h>
#include <CpuRegs.h>
#include <Library/BootGuardLib.h>
// AMI_OVERRIDE_START - For our Setup variable design.
#include "AmiSetupPrivate.h"
// AMI_OVERRIDE_END - For our Setup variable design.

///
/// HECI PCI Access Macro
///
#define HeciPciRead32(Register) PciRead32 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, Register))

GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN            mMeReset = FALSE;
// AMI_OVERRIDE_START - For our Setup variable design.
GLOBAL_REMOVE_IF_UNREFERENCED ME_SETUP           mMeSetup;
// AMI_OVERRIDE_END - For our Setup variable design.

/**
  Initialize ME strings.

  @param[in] EFI_EVENT    Event
  @param[in] VOID         *Context
**/
VOID
InitMeInfo (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  )
{
  EFI_STATUS          Status;
  UINTN               VariableSize;
  UINT8               *MeFwSkuValue;
  UINT8               *NfcDeviceValue;
  CHAR16              String[0x100];
  UINTN               StringLength;
  static  EFI_STRING_ID MeMode[] = {
    STRING_TOKEN(STR_ME_FW_MODE_NORMAL_MODE),
    STRING_TOKEN(STR_ME_FW_MODE_FAILED),
    STRING_TOKEN(STR_ME_FW_MODE_ALT_DISABLED),
    STRING_TOKEN(STR_ME_FW_MODE_TEMP_DISABLED),
    STRING_TOKEN(STR_ME_FW_MODE_SECOVER),
    STRING_TOKEN(STR_ME_FW_MODE_FAILED),
    STRING_TOKEN(STR_ME_FW_MODE_FAILED),
  };
  static  UINT8 *FwMeSku[] = {
     (UINT8 *) ME_FW_IMAGE_TYPE_CONSUMER_STRING,
     (UINT8 *) ME_FW_IMAGE_TYPE_CORPORATE_STRING,
     (UINT8 *) ME_FW_IMAGE_TYPE_UNIDENTIFIED_STRING,
  };
  static  UINT8 *NfcDevices[] = {
     (UINT8 *) STR_NFC_DISABLED,
     (UINT8 *) STR_NFC_MODULE_1
  };
  BOOLEAN              PttCapability;
  BOOLEAN              PttCurrentState;
  ME_INFO_SETUP_DATA   MeInfoSetupData;

  if ((Class != MAIN_FORM_SET_CLASS) && (Class != ADVANCED_FORM_SET_CLASS)) {
    return;
  }

  DEBUG ((DEBUG_INFO, "<InitMeInfo>\n"));

  //
  // Set default string size assumption at no more than 256 bytes
  //
  StringLength = 0x100;

  VariableSize = sizeof(ME_INFO_SETUP_DATA);
  Status = gRT->GetVariable (
                  L"MeInfoSetup",
                  &gMeInfoSetupGuid,
                  NULL,
                  &VariableSize,
                  &MeInfoSetupData
                  );
  if (EFI_ERROR(Status)) {
    return;
  }

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_ME_FW_VERSION_VALUE),
    L"%d.%d.%d.%d",
    MeInfoSetupData.MeMajor,
    MeInfoSetupData.MeMinor,
    MeInfoSetupData.MeHotFix,
    MeInfoSetupData.MeBuildNo
  );

  InitString (HiiHandle, STRING_TOKEN (STR_ME_FILE_SYSTEM_CORRUPTION_VALUE), L"%x", MeInfoSetupData.MeFileSystem);
#if 0  // AMI_OVERRIDE_START - For our HiiLib design.
  Status = HiiLibGetString (HiiHandle, MeMode[(MeInfoSetupData.MeFirmwareInfo & ME_MODE_MASK)], String, &StringLength);
#else
  Status = HiiLibGetString(HiiHandle, MeMode[(MeInfoSetupData.MeFirmwareInfo & ME_MODE_MASK)], &StringLength, String);   
#endif // AMI_OVERRIDE_END - For our HiiLib design.
  InitString (HiiHandle, STRING_TOKEN (STR_ME_FW_MODE_VALUE), L"%s", String);

  VariableSize = sizeof (ME_SETUP);
  Status = gRT->GetVariable (
                  L"MeSetup",
                  &gMeSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &mMeSetup
                  );

  if (mMeSetup.MeImageType == ME_IMAGE_CONSUMER_SKU_FW) {
    MeFwSkuValue = FwMeSku[0];
  } else if (mMeSetup.MeImageType == ME_IMAGE_CORPORATE_SKU_FW) {
    MeFwSkuValue = FwMeSku[1];
  } else {
    MeFwSkuValue = FwMeSku[2];
  }
  InitString (
    HiiHandle,
    STRING_TOKEN (STR_ME_FW_SKU_VALUE),
    L"%a",
    MeFwSkuValue
    );

  if (mMeSetup.NfcDevice != 0) {
    NfcDeviceValue = NfcDevices[1];
  } else {
    NfcDeviceValue = NfcDevices[0];
  }

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_NFC_STATE_VALUE),
    L"%a",
    NfcDeviceValue
    );

// AMI_OVERRIDE_START - ME State Error on KBL Platform   
  MeExtractConfig ();
// AMI_OVERRIDE_END - ME State Error on KBL Platform  
  //
  // Check if Heci device is enabled
  //
  if (HeciPciRead32 (PCI_VENDOR_ID_OFFSET) == 0xFFFFFFFF) {
    return;
  }

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_ME_FW_STATUS_1_VALUE),
    L"0x%08x",
    HeciPciRead32 (R_ME_HFS)
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_ME_FW_STATUS_2_VALUE),
    L"0x%08x",
    HeciPciRead32 (R_ME_GS_SHDW)
    );

  PttHeciGetCapability (&PttCapability);
  PttHeciGetState (&PttCurrentState);

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_PTT_CAP_STATE_VALUE),
    L"%d / %d",
    PttCapability,
    PttCurrentState
    );
  
// AMI_OVERRIDE_START - Sync ME setup data TpmDeviceSelection with Me PttCurrentState
#if 0
  { 
      UINTN         SetupVariableSize;
      UINT32        Attributes = 0;
      SETUP_DATA    gSetupData;
      EFI_STATUS    Status;
      EFI_GUID      SetupGuid = SETUP_GUID;
      
      if ( PttCurrentState != mMeSetup.TpmDeviceSelection) { // EIP269631
          // Sync ME setup data TpmDeviceSelection with Me PttCurrentState
          SetupVariableSize = sizeof (SETUP_DATA);
          Status = gRT->GetVariable (
                       L"Setup",
                       &SetupGuid,
                       &Attributes,
                       &SetupVariableSize,
                       &gSetupData
                   );
          
          gSetupData.TpmDeviceSelection = PttCurrentState;
          
          Status = gRT->SetVariable (
                       L"Setup",
                       &SetupGuid,
                       Attributes,
                       SetupVariableSize,
                       &gSetupData
                   );
      }
  }
#endif  
  InitMeSetupCallback ();
// AMI_OVERRIDE_END - Sync ME setup data TpmDeviceSelection with Me PttCurrentState
}

VOID
MeRouteConfig(
// AMI_OVERRIDE_START - For our notify event function design.
  EFI_EVENT         Event,
  VOID              *Context
// AMI_OVERRIDE_END - For our notify event function design.
  )
{
  ME_SETUP_STORAGE        MeSetupStorage = {0};
  ME_SETUP_STORAGE        MeBackupStorage = {0};
  TCG_SETUP               TcgSetup = {0};
  UINTN                   Size;
  EFI_STATUS              Status;
  UINT32                  EnableBitmap;
  UINT32                  DisableBitmap;
  HECI_PROTOCOL           *Heci;
// AMI_OVERRIDE_START - Keep the gAmiTseNVRAMUpdateGuid is ready.
  VOID                    *Interface;
  
  Status = gBS->LocateProtocol (&gAmiTseNVRAMUpdateGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  DEBUG ((DEBUG_INFO, "MeRouteConfig\n"));  
// AMI_OVERRIDE_END - Keep the gAmiTseNVRAMUpdateGuid is ready.

  Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, (VOID **) &Heci);
  if (MeIsAfterEndOfPost () || EFI_ERROR (Status)) {
    return;
  }

  Size = sizeof (ME_SETUP_STORAGE);
  gRT->GetVariable (L"MeSetupStorage", &gMeSetupVariableGuid, NULL, &Size, &MeSetupStorage);
  gRT->GetVariable (L"MeBackupStorage", &gMeSetupVariableGuid, NULL, &Size, &MeBackupStorage);
  Size = sizeof (ME_SETUP);
  gRT->GetVariable (L"MeSetup", &gMeSetupVariableGuid, NULL, &Size, &mMeSetup);
#if 0 // AMI_OVERRIDE_START - We don't use this because we use AMI TCG module.
  Size = sizeof (TCG_SETUP);
  gRT->GetVariable (L"TcgSetup", &gSetupVariableGuid, NULL, &Size, &TcgSetup);
#endif // AMI_OVERRIDE_END - We don't use this because we use AMI TCG module.

  if (MeSetupStorage.MeStateControl != MeBackupStorage.MeStateControl) {
    mMeReset = TRUE;

    if (MeSetupStorage.MeStateControl == 0) {
      Status = HeciSetMeDisableMsg ();
    } else {
      Status = HeciSetMeEnableMsg ();
    }
  }

  if (mMeSetup.MeImageType == IntelMeConsumerFw && MeSetupStorage.LocalFwUpdEnabled != MeBackupStorage.LocalFwUpdEnabled) {
    Status = HeciSetLocalFwUpdate (MeSetupStorage.LocalFwUpdEnabled);
  }

  if (MeSetupStorage.MngState != MeBackupStorage.MngState) {
    mMeReset = TRUE;
    if (MeSetupStorage.MngState == 1) {
      //
      // User wants to enable Manageability Features
      //
      EnableBitmap  = MANAGEABILITY_BITMASK;
      DisableBitmap = CLEAR_FEATURES_BITMASK;
    } else {
      //
      // User wants to disable Manageability Features
      //
      EnableBitmap  = CLEAR_FEATURES_BITMASK;
      DisableBitmap = MANAGEABILITY_BITMASK;
    }
    Status = HeciFwFeatureStateOverride (EnableBitmap, DisableBitmap);
  }

  if (MeSetupStorage.PttState != MeBackupStorage.PttState) {
    mMeReset = TRUE;
  }

  //
  // Pass BIOS TPM Deactivate setting to BtG ACM
  //
#if 0 // AMI_OVERRIDE_START - We don't use this because we use AMI TCG module.
  if (MeSetupStorage.TpmDeactivate != MeBackupStorage.TpmDeactivate) {
    if (TcgSetup.dTpm12Present == 1) {
      DEBUG ((DEBUG_INFO, "TPM is present.\nCalling HeciSendTpmData\n"));
      Status = HeciSendTpmData(MeSetupStorage.TpmDeactivate);
    } else {
      DEBUG ((DEBUG_INFO, "TPM is not present, do not send MKHI ACM TPM Data.\n"));
    }
  }
#endif // AMI_OVERRIDE_END - We don't use this because we use AMI TCG module.

  if (MeSetupStorage.NfcState != MeBackupStorage.NfcState) {
    mMeReset = TRUE;
    if (MeSetupStorage.NfcState == 1) {
      //
      // User wants to enable NFC
      //
      EnableBitmap  = NFC_BITMASK;
      DisableBitmap = CLEAR_FEATURES_BITMASK;
    } else {
      //
      // User wants to disable NFC
      //
      EnableBitmap  = CLEAR_FEATURES_BITMASK;
      DisableBitmap = NFC_BITMASK;
    }
    Status = HeciFwFeatureStateOverride (EnableBitmap, DisableBitmap);
    ASSERT_EFI_ERROR (Status);
  }
  //
  // Remember current FW's state to avoid sending unnecessary messages next time
  //
  gRT->SetVariable (
         L"MeBackupStorage",
         &gMeSetupVariableGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof (ME_SETUP_STORAGE),
         &MeSetupStorage
         );

// AMI_OVERRIDE_START - For our notify event function design.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }  
// AMI_OVERRIDE_END - For our notify event function design.
}

/**
  This function validates the Flex Ratio setup value

  @param[in] EFI_FORM_CALLBACK_PROTOCOL   *This
  @param[in] UINT16                       KeyValue
  @param[in] EFI_IFR_DATA_ARRAY           *Data,
  @param[in] EFI_HII_CALLBACK_PACKET      **Packet

  @retval EFI_SUCCESS        The callback successfully handled the action
  @retval EFI_UNSUPPORTED    The specified action is not supported by the callback
**/
EFI_STATUS
EFIAPI
MeFormCallBackFunction(
#if 0  // AMI_OVERRIDE_START - For our callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif // AMI_OVERRIDE_END - For our callback function design.
  )
{
// AMI_OVERRIDE_START - For our callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
	    
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
// AMI_OVERRIDE_END - For our callback function design.
	  
  if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {

    switch (KeyValue) {

     case MNG_STATE_KEY:
     case NFC_STATE_KEY:
       return EFI_SUCCESS;

     default:
       return EFI_UNSUPPORTED;
    }

  } else {
    return EFI_UNSUPPORTED;
  }
}

VOID
MeExtractConfig (
  VOID
  )
{
  EFI_STATUS              Status;
  UINT8                   FwUpdateState;
  UINT32                  MeMode;
  MEFWCAPS_SKU            CurrentFeatures;
  ME_SETUP_STORAGE        MeSetupStorage;
  UINTN                   Size;
  UINT32                  VariableAttr;
  AMT_WRAPPER_PROTOCOL    *AmtWrapper;
  UINT32                  MeFwSts4;
  UINT32                  MsrValue;
  HECI_PROTOCOL           *Heci;

  AmtWrapper = NULL;
  VariableAttr = 0;

  DEBUG ((DEBUG_INFO, "MeExtractConfig\n"));

  Size = sizeof (ME_SETUP_STORAGE);
  Status = gRT->GetVariable (L"MeSetupStorage", &gMeSetupVariableGuid, &VariableAttr, &Size, &MeSetupStorage);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "MeSetupStorage does not exist. Assuming default values.\n"));
    VariableAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
    MeSetupStorage.MngState = 1;
    MeSetupStorage.LocalFwUpdEnabled = 1;
    MeSetupStorage.MeStateControl = 1;
    MeSetupStorage.AfterEoP = 0;
    MeSetupStorage.RemoteSessionActive = 0;
    MeSetupStorage.PttState = 0;
    MeSetupStorage.BootGuardSupport = 0;
    MeSetupStorage.MeasureBoot = 0;
    MeSetupStorage.TpmDeactivate = 0;
    MeSetupStorage.NfcState = 0;
  }

  if (IsBootGuardSupported()) {
    MeSetupStorage.BootGuardSupport = 1;
  }

  MsrValue = (UINT32) AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);
  if (MsrValue & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) {
    MeSetupStorage.MeasureBoot = 1;
  }

  if (MeIsAfterEndOfPost ()) {
    MeSetupStorage.AfterEoP = 1;
  } else {
    MeSetupStorage.AfterEoP = 0;
  }

  Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, (VOID **) &Heci);
  if (EFI_ERROR (Status)) {
    MeSetupStorage.AfterEoP = 1;
    goto Done;
  }

  Heci->GetMeMode (&MeMode);

  if (MeMode == ME_MODE_NORMAL) {
    MeSetupStorage.MeStateControl = 1;
  }
// AMI_OVERRIDE_START >>> Fix MeState is wrong when disable ME and clear CMOS.
  else {
    MeSetupStorage.MeStateControl = 0;
  }
// AMI_OVERRIDE_END <<< Fix MeState is wrong when disable ME and clear CMOS.

  if (!MeSetupStorage.MeStateControl || MeSetupStorage.AfterEoP) {
    goto Done;
  }

  Status = gBS->LocateProtocol(&gEfiAmtWrapperProtocolGuid, NULL, (VOID **) &AmtWrapper);
  if (!EFI_ERROR(Status)) {
    MeSetupStorage.RemoteSessionActive = AmtWrapper->ActiveManagementIsStorageRedirectionEnabled() ||
                                         AmtWrapper->ActiveManagementIsSolEnabled() ||
                                         AmtWrapper->ActiveManagementIsKvmEnabled();
  }

  Status = HeciGetFwFeatureStateMsg (&CurrentFeatures);
  if (!EFI_ERROR (Status)) {
    MeSetupStorage.MngState   = !!CurrentFeatures.Fields.Manageability;
    MeSetupStorage.NfcState   = !!CurrentFeatures.Fields.NFC;
    MeSetupStorage.PttState   = !!CurrentFeatures.Fields.PTT;
  }

  MeFwSts4 = HeciPciRead32 (R_ME_HFS_4);
  if ((MeFwSts4 & B_TPM1_2_DEACTIVATED) != 0) {
    MeSetupStorage.TpmDeactivate = 1;
  }

  //
  // Skip Local Firmware update if it is not Consumer sku or not in normal mode
  //
  Size = sizeof (ME_SETUP);
  Status = gRT->GetVariable (L"MeSetup", &gMeSetupVariableGuid, NULL, &Size, &mMeSetup);
  ASSERT_EFI_ERROR (Status);

  if (mMeSetup.MeImageType != IntelMeConsumerFw) {
    goto Done;
  }

  Status = HeciGetLocalFwUpdate (&FwUpdateState);
  if (!EFI_ERROR (Status)) {
    MeSetupStorage.LocalFwUpdEnabled = FwUpdateState;
  }

Done:
  Status = gRT->SetVariable (
                  L"MeSetupStorage",
                  &gMeSetupVariableGuid,
                  VariableAttr,
                  sizeof (ME_SETUP_STORAGE),
                  &MeSetupStorage
                  );
  Status = gRT->SetVariable (
                  L"MeBackupStorage",
                  &gMeSetupVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof (ME_SETUP_STORAGE),
                  &MeSetupStorage
                  );

  DEBUG ((DEBUG_INFO, "MeExtractConfig %d %d %d %d %d %d %d %d %d %d\n",
      MeSetupStorage.MngState,
      MeSetupStorage.LocalFwUpdEnabled,
      MeSetupStorage.MeStateControl,
      MeSetupStorage.AfterEoP,
      MeSetupStorage.RemoteSessionActive,
      MeSetupStorage.PttState,
      MeSetupStorage.BootGuardSupport,
      MeSetupStorage.MeasureBoot,
      MeSetupStorage.TpmDeactivate,
      MeSetupStorage.NfcState ));
}

// AMI_OVERRIDE_START - For our callback function design.
VOID
InitMeSetupCallback (
  VOID
  )
{
  VOID    *Registration;
  
  AmiSetupRegisterCallbackHandler (MNG_STATE_KEY, MeFormCallBackFunction);   
  AmiSetupRegisterCallbackHandler (NFC_STATE_KEY, MeFormCallBackFunction); 
  
  EfiCreateProtocolNotifyEvent (
           &gAmiTseNVRAMUpdateGuid,
           TPL_NOTIFY,
           MeRouteConfig,
           NULL,
           &Registration
           );  
}
// AMI_OVERRIDE_END - For our callback function design.
