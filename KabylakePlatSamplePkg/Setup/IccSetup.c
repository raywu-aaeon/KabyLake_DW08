/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2017 Intel Corporation.

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

#include <Base.h>
#include "IccSetup.h"
#include <PchAccess.h>
#include <Protocol/WdtApp.h>
#include <Protocol/HiiConfigAccess.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeMeLib.h>
#include <Library/HiiLib.h>
#include <Library/CmosAccessLib.h>
#include <SetupPrivate.h>
#include <IccSetupData.h>
#include "PlatformBoardId.h"
#include "OemSetup.h"
#include <CmosMap.h>
#include <Library/ConfigBlockLib.h>
// AMI_OVERRIDE_START - Fix BCLK value kept old value in ME configuration when BCLK's WDT happened
#include "AmiSetupPrivate.h"
#include <Protocol/Wdt.h>
// AMI_OVERRIDE_END - Fix BCLK value kept old value in ME configuration when BCLK's WDT happened

#define NUM_USAGES      8
#define NOT_INITIALIZED 0xFF
#define HZ_TO_10KHZ     10000

static ICC_GETSET_CLOCK_SETTINGS_REQRSP mSettingsBclk;
static ICC_GETSET_CLOCK_SETTINGS_REQRSP mSettingsPcie;
static ICC_GETSET_CLOCK_SETTINGS_REQRSP mDefaultsBclk;
static ICC_GETSET_CLOCK_SETTINGS_REQRSP mDefaultsPcie;
static ICC_SINGLE_CLK_RANGE_DEF mRangesBclk;
static ICC_SINGLE_CLK_RANGE_DEF mRangesPcie;
static BOOLEAN            mProfileChanged = FALSE;
static BOOLEAN            gClockChanged[MAX_UI_CLOCKS] = {0};
static HECI_PROTOCOL      *mHeci = NULL;

GLOBAL_REMOVE_IF_UNREFERENCED ICC_MBP_DATA               gIccMbpData;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_HII_HANDLE             gHiiHandle;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                    gIccReset = FALSE;
// AMI_OVERRIDE_START - For our Setup variable design.
static SA_SETUP               mSaSetup;
// AMI_OVERRIDE_END - For our Setup variable design.


VOID
UpdateClockData (
  IN  ICC_GETSET_CLOCK_SETTINGS_REQRSP  ClockSettings,
  IN  UINT8                             ClockNumber,
  OUT ICC_SETUP_DATA*                   SetupPtr
  )
{
  SetupPtr->Frequency[ClockNumber]  = (UINT16) ( (ClockSettings.Freq + (HZ_TO_10KHZ/2)) / HZ_TO_10KHZ);
  SetupPtr->Spread[ClockNumber] = ClockSettings.SscPercent;
}

/**
  Extract clock data from ICC setup variable.

  @param[in]  IccSetupPtr              Pointer to the ICC_SETUP_DATA buffer.
  @param[in]  ClockNumber              The number of Clock.
  @param[out] ClockSettings            Pointer to the ICC_GETSET_CLOCK_SETTINGS_REQRSP buffer.
**/
VOID
ExtractClockData (
  IN  ICC_SETUP_DATA*                     IccSetupPtr,
  IN  UINT8                               ClockNumber,
  OUT ICC_GETSET_CLOCK_SETTINGS_REQRSP*   ClockSettings
  )
{
  ClockSettings->UserFreq    = HZ_TO_10KHZ * IccSetupPtr->Frequency[ClockNumber];
  ClockSettings->SscPercent  = IccSetupPtr->Spread[ClockNumber];
  ClockSettings->SscMode     = ICC_SSC_DOWN;
  ClockSettings->ReqClock    = ClockNumber;
}

/**
  Reads string from Hii database. Allocates memory.
  Caller of this function is responsible for freeing it.

**/
VOID
GetStringFromHii (
  IN  EFI_STRING_ID StringId,
  OUT UINT16**      BufferPtr
  )
{
  CHAR16*    StrBuffer = 0;
  UINTN      StrLen = 0;

#if 0  // AMI_OVERRIDE_START - For our HiiLib design.
  HiiLibGetString(gHiiHandle, StringId, StrBuffer, &StrLen);
#else
  HiiLibGetString(gHiiHandle, StringId, &StrLen, StrBuffer);  
#endif // AMI_OVERRIDE_END - For our HiiLib design.
  StrBuffer = AllocatePool(StrLen);
#if 0  // AMI_OVERRIDE_START - For our HiiLib design.
  ASSERT (StrBuffer != NULL);
  if (StrBuffer != NULL) {
    HiiLibGetString(gHiiHandle, StringId, StrBuffer, &StrLen);
  }
#else
  HiiLibGetString(gHiiHandle, StringId, &StrLen, StrBuffer);  
#endif // AMI_OVERRIDE_END - For our HiiLib design.
  *BufferPtr = StrBuffer;
}

/**
  Checks if provided settings are equal to clock's defaults

**/
BOOLEAN
AreSettingsAtDefault (
  IN ICC_GETSET_CLOCK_SETTINGS_REQRSP *Requested,
  IN UINT8                            ClockNumber
  )
{
  ICC_GETSET_CLOCK_SETTINGS_REQRSP Default;
  if (ClockNumber == CLOCK1) {
    CopyMem(&Default, &mDefaultsBclk, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP));
  } else {
    CopyMem(&Default, &mDefaultsPcie, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP));
  }
  if (Requested->Freq        == Default.Freq   &&
      Requested->SscMode     == Default.SscMode     &&
      Requested->SscPercent  == Default.SscPercent ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

EFI_STATUS
IccCallback (
#if 0 // AMI_OVERRIDE_START - For our Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION Action,
  IN EFI_QUESTION_ID KeyValue,
  IN UINT8 Type,
  IN EFI_IFR_TYPE_VALUE *Value,
  OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif // AMI_OVERRIDE_END - For our Callback function design.
  )
{
// AMI_OVERRIDE_START - For our Callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
  EFI_IFR_TYPE_VALUE 	  *Value;
		    
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
  Value = CallbackValues->Value;
// AMI_OVERRIDE_END - For our Callback function design.
  
  //
  // Cannot access ICC menu after EOP.
  //
  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  switch (KeyValue) {

    case KEY_ICC_FREQ2:
      if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        gClockChanged[CLOCK1] = TRUE;
        Value->u16 = (UINT16)(mDefaultsBclk.Freq / HZ_TO_10KHZ);
        return EFI_SUCCESS;
      } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
        gClockChanged[CLOCK1] = TRUE;
        if (Value->u16 < (UINT16)(mRangesBclk.Fields.ClkFreqMin)) {
          Value->u16 = (UINT16)(mRangesBclk.Fields.ClkFreqMin);
        } else if (Value->u16 > (UINT16)(mRangesBclk.Fields.ClkFreqMax)) {
          Value->u16 = (UINT16)(mRangesBclk.Fields.ClkFreqMax);
        }
        return EFI_SUCCESS;
      } else {
        return EFI_UNSUPPORTED;
      }

    case KEY_ICC_FREQ3:
      if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        gClockChanged[CLOCK0] = TRUE;
        Value->u16 = (UINT16)(mDefaultsPcie.Freq / HZ_TO_10KHZ);
        return EFI_SUCCESS;
      } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
        gClockChanged[CLOCK0] = TRUE;
        if (Value->u16 < (UINT16)(mRangesPcie.Fields.ClkFreqMin)) {
          Value->u16 = (UINT16)(mRangesPcie.Fields.ClkFreqMin);
        } else if (Value->u16 > (UINT16)(mRangesPcie.Fields.ClkFreqMax)) {
          Value->u16 = (UINT16)(mRangesPcie.Fields.ClkFreqMax);
        }
        return EFI_SUCCESS;
      } else {
        return EFI_UNSUPPORTED;
      }

    case KEY_ICC_SPREAD2:
      if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        gClockChanged[CLOCK1] = TRUE;
        Value->u8 = mDefaultsBclk.SscPercent;
        return EFI_SUCCESS;
      } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
        gClockChanged[CLOCK1] = TRUE;
        if (Value->u8 > mRangesBclk.Fields.SscSprPercentMax) {
          Value->u8 = (UINT8) mRangesBclk.Fields.SscSprPercentMax;
        }
        return EFI_SUCCESS;
      } else {
        return EFI_UNSUPPORTED;
      }

    case KEY_ICC_SPREAD3:
      if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        gClockChanged[CLOCK0] = TRUE;
        Value->u8 = mDefaultsPcie.SscPercent;
        return EFI_SUCCESS;
      } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
        gClockChanged[CLOCK0] = TRUE;
        if (Value->u8 > mRangesPcie.Fields.SscSprPercentMax) {
          Value->u8 = (UINT8) mRangesPcie.Fields.SscSprPercentMax;
        }
        return EFI_SUCCESS;
      } else {
        return EFI_UNSUPPORTED;
      }

    default:
      return EFI_UNSUPPORTED;
  }
}

VOID
UpdateSubmenuStrings (
  IN UINT8 SubMenuNumber,
  IN ICC_SINGLE_CLK_RANGE_DEF Ranges
  )
{
  CHAR16  StringBuffer[100];
  CHAR16* TempBuffer;
  UINT8   i;
  UINT8   NeedSlash;
  UINT16  StrClockTitle;
  UINT16  StrFreqRange;
  UINT16  StrMaxSpread;
  UINT16  StrSpreadAdjust;
  CHAR16* ClockUsageName[NUM_USAGES];

  //
  // This function is shard by both clocks. We need to map the string tokens
  // before we use them later in the function. This is intended to avoid unneeded
  // if-else statements. Any new clock support will need to add a new case.
  //
  switch (SubMenuNumber)
  {
    case CLOCK1:
      StrClockTitle   =  STR_ICC_CLOCK1_TITLE;
      StrFreqRange    =  STR_ICC_CLOCK1_FREQ_RANGE_TEXT;
      StrMaxSpread    =  STR_ICC_CLOCK1_MAX_SPREAD_TEXT;
      StrSpreadAdjust =  STR_ICC_CLOCK1_SPREAD_ADJUSTMENTS_TEXT;
    break;

    case CLOCK0:
      StrClockTitle   =  STR_ICC_CLOCK0_TITLE;
      StrFreqRange    =  STR_ICC_CLOCK0_FREQ_RANGE_TEXT;
      StrMaxSpread    =  STR_ICC_CLOCK0_MAX_SPREAD_TEXT;
      StrSpreadAdjust =  STR_ICC_CLOCK0_SPREAD_ADJUSTMENTS_TEXT;
    break;

    default:
      DEBUG ((DEBUG_ERROR, "(ICC) Unsupported submenu detected\n"));
      return;
    break;
  }

  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_USE_DMI),         &ClockUsageName[0]);
  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_USE_BCLK),        &ClockUsageName[1]);
  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_USE_PEG),         &ClockUsageName[2]);
  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_USE_SATA),        &ClockUsageName[3]);
  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_USE_RESERVED),    &ClockUsageName[4]);
  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_USE_RESERVED),    &ClockUsageName[5]);
  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_USE_USB3),        &ClockUsageName[6]);
  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_USE_PCIE),        &ClockUsageName[7]);

  //
  // ClockUsage mask is defined in the SPT ICC FAS. Bit 4 and 5 of ClockUsage is reserved,
  // so let's pretend it's zero for purpose of displaying clock names
  //
#if 0  // AMI_OVERRIDE_START - EIP219638 : Fixed the Static code analysis issues.
  Ranges.Fields.ClockUsage &= ~(BIT5 || BIT4);
#else
  Ranges.Fields.ClockUsage &= ~(BIT5 | BIT4);
#endif // AMI_OVERRIDE_END - EIP219638 : Fixed the Static code analysis issues..
  StringBuffer[0] = 0;
  NeedSlash = 0;

  //
  // Clear the title buffer before we process the usages
  //
  InitString(gHiiHandle, STRING_TOKEN(StrClockTitle), L"%s", StringBuffer);

  //
  // Search through the usage mask list and concatenate valid usages to string
  //
  for (i=0; i<NUM_USAGES; i++) {
    if (Ranges.Fields.ClockUsage & (1<<i)) {
      if (NeedSlash == 1) {
        StrCatS (StringBuffer, sizeof (StringBuffer) / sizeof (CHAR16), L"/");
      }
      StrCatS (StringBuffer, sizeof (StringBuffer) / sizeof (CHAR16), ClockUsageName[i]);
      NeedSlash = 1;
    }
  }

  for (i=0; i<NUM_USAGES; i++) {
    FreePool(ClockUsageName[i]);
  }

  //
  // Add "Clock Settings" to the end of the string to complete formatting
  //
  GetStringFromHii(STRING_TOKEN(STR_ICC_CLOCK_SETTINGS), &TempBuffer);
  if (TempBuffer == NULL) return;
  StrCatS (StringBuffer, sizeof (StringBuffer) / sizeof (CHAR16), TempBuffer);
  FreePool(TempBuffer);

  //
  // Copy formatted strings into clock settings menu
  //
  InitString(gHiiHandle, STRING_TOKEN(StrClockTitle), L"%s", StringBuffer);

  //
  // Frequency Range
  //
  InitString(gHiiHandle, STRING_TOKEN(StrFreqRange), L"%d.%02d - %d.%02d MHz",
    Ranges.Fields.ClkFreqMin/100, Ranges.Fields.ClkFreqMin%100, Ranges.Fields.ClkFreqMax/100, Ranges.Fields.ClkFreqMax%100);

  //
  // Maximum Spread %
  //
  InitString(gHiiHandle, STRING_TOKEN(StrMaxSpread), L"%d.%02d%%", Ranges.Fields.SscSprPercentMax/100, Ranges.Fields.SscSprPercentMax%100);

  //
  // Spread Mode Adjustments
  //
  if(Ranges.Fields.SscChangeAllowed) {
    GetStringFromHii(STRING_TOKEN(STR_ICC_SSC_CHANGE_ALLOWED), &TempBuffer);
  } else if(Ranges.Fields.SscHaltAllowed) {
    GetStringFromHii(STRING_TOKEN(STR_ICC_SSC_HALT_ALLOWED), &TempBuffer);
  } else {
    GetStringFromHii(STRING_TOKEN(STR_ICC_SSC_NOTHING_ALLOWED), &TempBuffer);
  }
  InitString(gHiiHandle, STRING_TOKEN(StrSpreadAdjust), TempBuffer);
  FreePool(TempBuffer);
}

/**
  Setup callback executed when user selects a ICC Profile from the BIOS UI.
  Changes visibility of other options

  @param This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action                 Specifies the type of action taken by the browser.
  @param QuestionId             A unique value which is sent to the original exporting driver
                                so that it can identify the type of data to expect.
  @param Type                   The type of value for the question.
  @param Value                  A pointer to the data being sent to the original exporting driver.
  @param ActionRequest          On return, points to the action requested by the callback function.

  @retval EFI_SUCCESS           The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES  Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR      The variable could not be saved.
  @retval EFI_UNSUPPORTED       The specified Action is not supported by the callback.
  @retval EFI_INVALID_PARAMETER The parameter of Value or ActionRequest is invalid.
**/
EFI_STATUS
EFIAPI
IccProfileCallback (
#if 0 // AMI_OVERRIDE_START - For our Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif // AMI_OVERRIDE_END - For our Callback function design.
  )
{
  UINTN          Size = sizeof(ICC_SETUP_DATA);
  ICC_SETUP_DATA IccSetup;
  EFI_STRING     RequestString = NULL;
// AMI_OVERRIDE_START - For our Callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
  EFI_IFR_TYPE_VALUE      *Value;
  	    
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
  Value = CallbackValues->Value;
// AMI_OVERRIDE_END - For our Callback function design.

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action < EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    return EFI_UNSUPPORTED;
  }

  if (MeIsAfterEndOfPost ()) {
    return EFI_UNSUPPORTED;
  }

  if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    if (gIccMbpData.ProfileSelectionAllowed) {
      //
      // Perform change only if profile is selectable by BIOS
      //
      Value->u8 = 0;
    }
  }

  //
  // sanity check: can't choose profile that doesn't exist
  //
  if (Value->u8 > gIccMbpData.NumProfiles - 1) {
    Value->u8 = gIccMbpData.NumProfiles - 1;
  }

  //
  // when profile is changed, most other icc options can't be changed before reboot. hide those options.
  //
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (ICC_SETUP_DATA, AllowAdvancedOptions), sizeof (IccSetup.AllowAdvancedOptions));
  if (mProfileChanged || (Value->u8 != gIccMbpData.Profile)) {
    HiiGetBrowserData (&gIccGuid, ICC_SETUP_DATA_C_NAME, Size, (UINT8*)&IccSetup);
    IccSetup.AllowAdvancedOptions= DONT_DISPLAY;
#if 0  // AMI_OVERRIDE_START - For our HiiLib design.
    HiiSetBrowserData(&gIccGuid, ICC_SETUP_DATA_C_NAME, sizeof(ICC_SETUP_DATA), (UINT8 *)&IccSetup, RequestString);
#else
    HiiSetBrowserData(&gIccGuid, ICC_SETUP_DATA_C_NAME, sizeof(ICC_SETUP_DATA), (UINT8 *)&IccSetup, NULL);
#endif // AMI_OVERRIDE_END - For our HiiLib design.
  } else if (!mProfileChanged && (Value->u8 == gIccMbpData.Profile)) {
    HiiGetBrowserData (&gIccGuid, ICC_SETUP_DATA_C_NAME, Size, (UINT8*)&IccSetup);
    IccSetup.AllowAdvancedOptions= DISPLAY;
#if 0  // AMI_OVERRIDE_START - For our HiiLib design.
    HiiSetBrowserData(&gIccGuid, ICC_SETUP_DATA_C_NAME, sizeof(ICC_SETUP_DATA), (UINT8 *)&IccSetup, RequestString);
#else
    HiiSetBrowserData(&gIccGuid, ICC_SETUP_DATA_C_NAME, sizeof(ICC_SETUP_DATA), (UINT8 *)&IccSetup, NULL);
#endif // AMI_OVERRIDE_END - For our HiiLib design.
  }
  FreePool(RequestString);
  return EFI_SUCCESS;
}

/**
  Executed by setup calback function
  Based on data entered by user, sends clock change requests to ICC OverClocking
  Writing to susram or flash requires that old susram and flash contents be invalidated
  In case of any problem, messagebox is displayed so user can know what corrective action is required

  @param[in] RequestedClockSettings initial clock divider value

**/
VOID
ApplyClockSettings (
  IN ICC_GETSET_CLOCK_SETTINGS_REQRSP *RequestedClockSettings
  )
{
  EFI_STATUS          Status;
  WDT_APP_PROTOCOL*   WdtAppProtocol;
  UINTN               VariableSize;
  UINT8               BclkRampFlag;
  UINT32              VariableAttributes;

  Status = HeciSetIccClockSettings(RequestedClockSettings);
  DEBUG ((DEBUG_INFO, "(ICC) HeciSetIccClockSettings, Status = %r\n", Status));

  VariableSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &mSaSetup
                  );
  ASSERT_EFI_ERROR (Status);

  if (Status == EFI_SUCCESS) {
    //
    // Update the BclkFrequency setup option to sync with MRC and XTU
    //
    if (RequestedClockSettings->ReqClock == CLOCK1) {
      mSaSetup.BclkFrequency = RequestedClockSettings->UserFreq / HZ_TO_10KHZ;
      Status = gRT->SetVariable (
                      L"SaSetup",
                      &gSaSetupVariableGuid,
                      VariableAttributes,
                      VariableSize,
                      &mSaSetup
                      );
      ASSERT_EFI_ERROR (Status);
      DEBUG ((DEBUG_INFO, "(ICC) BclkFrequency = %d\n", mSaSetup.BclkFrequency));
      DEBUG ((DEBUG_INFO, "(ICC) BclkOverride  = %d\n", mSaSetup.BclkOverride));
      DEBUG ((DEBUG_INFO, "(ICC) BclkChangeWarmReset = %d\n", mSaSetup.BclkChangeWarmReset));
      DEBUG ((DEBUG_INFO, "(ICC) BclkChangeContinues  = %d\n", mSaSetup.BclkChangeContinues));
    }

    if (!AreSettingsAtDefault(RequestedClockSettings, RequestedClockSettings->ReqClock)) {
      Status = gBS->LocateProtocol(&gWdtAppProtocolGuid, NULL, (VOID **) &WdtAppProtocol);
      ASSERT_EFI_ERROR(Status);
      //@todo: maybe switch to using Wdt instead of WdtApp?
      WdtAppProtocol->RequestWdtAfterReboot();
    }

    //
    //  Set the BCLK Ramp flag
    //
    BclkRampFlag = 1;
    CmosWrite8 (CMOS_OC_SEND_BCLK_RAMP_MSG, BclkRampFlag);
  }
}

VOID DebugDumpConfig (ICC_SETUP_DATA IccSetup) {
  DEBUG ((DEBUG_INFO, "Frequency %d %d\n", IccSetup.Frequency[0], IccSetup.Frequency[1]));
  DEBUG ((DEBUG_INFO, "Spread %d %d\n", IccSetup.Spread[0], IccSetup.Spread[1]));
  DEBUG ((DEBUG_INFO, "ShowFrequency %d %d\n", IccSetup.ShowFrequency[0], IccSetup.ShowFrequency[1]));
  DEBUG ((DEBUG_INFO, "ShowSpread %d %d\n", IccSetup.ShowSpread[0], IccSetup.ShowSpread[1]));
  DEBUG ((DEBUG_INFO, "ShowClock %d %d\n", IccSetup.ShowClock[0], IccSetup.ShowClock[1]));
  DEBUG ((DEBUG_INFO, "ShowProfile %d\n", IccSetup.ShowProfile));
  DEBUG ((DEBUG_INFO, "Profile %d\n", IccSetup.Profile));
  DEBUG ((DEBUG_INFO, "RegLock %d\n", IccSetup.RegLock));
  DEBUG ((DEBUG_INFO, "AllowAdvancedOptions %d\n", IccSetup.AllowAdvancedOptions));
}

VOID
InitIccStrings (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  )
{
  if(Class != ADVANCED_FORM_SET_CLASS) {return;}
  gHiiHandle = HiiHandle;
  
// AMI_OVERRIDE_START - For our Callback function design.
  InitIccSetupCallback ();
  IccExtractConfig ();
// AMI_OVERRIDE_END - For our Callback function design.
}

VOID
GetClockSettings (
  VOID
  )
{
  EFI_STATUS            Status;
  static BOOLEAN        StringInit = FALSE;
  ICC_CLK_RANGE_DEF_REC Crdr;

  if (MeIsAfterEndOfPost () || StringInit) {
    return;
  }

  Status = HeciGetClockRangeDefinitionRecord(&Crdr);
  DEBUG ((DEBUG_INFO, "(ICC) CRDR[0] Dword[0] = %X Dword[1] = %X\n", Crdr.SingleClkRangeDef[0].Dword[0], Crdr.SingleClkRangeDef[0].Dword[1]));
  DEBUG ((DEBUG_INFO, "(ICC) CRDR[1] Dword[0] = %X Dword[1] = %X\n", Crdr.SingleClkRangeDef[1].Dword[0], Crdr.SingleClkRangeDef[1].Dword[1]));

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "(ICC) Get Clock Range Definition Record message failed. Status = %r\n", Status));
    return;
  }

  //
  //  Get BCLK Settings
  //
  SetMem(&mSettingsBclk, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP), 0);

  mSettingsBclk.ReqClock = CLOCK1;
  Status = HeciGetIccClockSettings(&mSettingsBclk);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "(ICC) Get Clock Settings messge failed, clock 1. Status = %r\n", Status));
    return;
  }
  CopyMem(&mRangesBclk.Dword[0], &Crdr.SingleClkRangeDef[CLOCK1], sizeof (ICC_SINGLE_CLK_RANGE_DEF));

  //
  //  Get PCIe Settings
  //
  SetMem(&mSettingsPcie, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP), 0);

  mSettingsPcie.ReqClock = CLOCK0;
  Status = HeciGetIccClockSettings(&mSettingsPcie);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "(ICC) Get Clock Settings messge failed, clock 0. Status = %r\n", Status));
    return;
  }
  CopyMem(&mRangesPcie.Dword[0], &Crdr.SingleClkRangeDef[CLOCK0], sizeof (ICC_SINGLE_CLK_RANGE_DEF));

  StringInit = TRUE;
}

VOID
UpdateVisibility (
  ICC_SETUP_DATA* SetupPtr
  )
{
  UINT32              MeMode;
  SETUP_VOLATILE_DATA SysCfg;
  UINTN               Size;
  EFI_STATUS          Status;
  BOOLEAN             EopSent;

  Size = sizeof (SETUP_VOLATILE_DATA);
  Status = gRT->GetVariable (L"SetupVolatileData", &gSetupVariableGuid, NULL, &Size, &SysCfg);
  ASSERT_EFI_ERROR (Status);

  EopSent = MeIsAfterEndOfPost ();
  mHeci->GetMeMode (&MeMode);

  if (EopSent || (MeMode != ME_MODE_NORMAL)) {
    SetupPtr->AllowAdvancedOptions = DONT_DISPLAY;
  } else {
    SetupPtr->AllowAdvancedOptions = DISPLAY;
  }
  if (EopSent || gIccMbpData.ProfileSelectionAllowed == DONT_DISPLAY || (MeMode != ME_MODE_NORMAL)) {
    SetupPtr->ShowProfile = DONT_DISPLAY;
  } else {
    SetupPtr->ShowProfile = DISPLAY;
  }
  if (EopSent || mRangesBclk.Fields.ClockUsage == 0) {
    SetupPtr->ShowClock[CLOCK1]= DONT_DISPLAY;
  } else {
    SetupPtr->ShowClock[CLOCK1] = DISPLAY;
  }
  if (EopSent || mRangesPcie.Fields.ClockUsage == 0) {
    SetupPtr->ShowClock[CLOCK0] = DONT_DISPLAY;
  } else {
    SetupPtr->ShowClock[CLOCK0] = DISPLAY;
  }
  if (EopSent || mRangesBclk.Fields.ClockUsage == 0 || mRangesBclk.Fields.ClkFreqMax == mRangesBclk.Fields.ClkFreqMin) {
    SetupPtr->ShowFrequency[CLOCK1]= DONT_DISPLAY;
  } else {
    SetupPtr->ShowFrequency[CLOCK1] = DISPLAY;
  }
  if (EopSent || mRangesPcie.Fields.ClockUsage == 0 || mRangesPcie.Fields.ClkFreqMin == mRangesPcie.Fields.ClkFreqMin) {
    SetupPtr->ShowFrequency[CLOCK0] = DONT_DISPLAY;
  } else {
    SetupPtr->ShowFrequency[CLOCK0] = DISPLAY;
  }

  if (EopSent || mRangesBclk.Fields.ClockUsage == 0 || mRangesBclk.Fields.SscChangeAllowed == 0) {
    SetupPtr->ShowSpread[CLOCK1] = DONT_DISPLAY;
  } else {
    SetupPtr->ShowSpread[CLOCK1] = DISPLAY;
  }
  if (EopSent || mRangesPcie.Fields.ClockUsage == 0 || mRangesPcie.Fields.SscChangeAllowed == 0) {
    SetupPtr->ShowSpread[CLOCK0] = DONT_DISPLAY;
  } else {
    SetupPtr->ShowSpread[CLOCK0] = DISPLAY;
  }
}

VOID
IccExtractConfig (
  VOID
  )
{
  ICC_SETUP_DATA  IccSetup;
  EFI_STATUS      Status;
  UINTN           Size;

  ZeroMem ((VOID *) &IccSetup, sizeof (ICC_SETUP_DATA));
  if (!mHeci) {
    Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, (VOID **) &mHeci);
    if (EFI_ERROR(Status)) {
      IccSetup.AllowAdvancedOptions = DONT_DISPLAY;
      IccSetup.ShowProfile = DONT_DISPLAY;
      gRT->SetVariable (
             ICC_SETUP_DATA_C_NAME,
             &gIccGuid,
// AMI_OVERRIDE_START - Fix ICC BCLK function and fix ICC BCLK setup information.
#if 0
             EFI_VARIABLE_BOOTSERVICE_ACCESS,
#else
             (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
#endif
// AMI_OVERRIDE_END - Fix ICC BCLK function and fix ICC BCLK setup information.
             sizeof(IccSetup),
             &IccSetup
             );
      return;
    }
  }

  //
  //  Get Icc setup data
  //
  Size = sizeof(IccSetup);
  Status = gRT->GetVariable (ICC_SETUP_DATA_C_NAME, &gIccGuid, NULL, &Size, &IccSetup);
  
  if (!EFI_ERROR(Status)) {
    //
    // if the variable exists, it means init was completed. Check for EoP only and hide everything if it's after EoP.
    //
// AMI_OVERRIDE_START - Fix ICC BCLK function and fix ICC BCLK setup information.
#if 0
    if (MeIsAfterEndOfPost ()) {
      IccSetup.AllowAdvancedOptions = DONT_DISPLAY;
      IccSetup.ShowProfile = DONT_DISPLAY;
      gRT->SetVariable (
             ICC_SETUP_DATA_C_NAME,
             &gIccGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS,
             sizeof(IccSetup),
             &IccSetup
             );
    }
    return;
#endif
// AMI_OVERRIDE_END - Fix ICC BCLK function and fix ICC BCLK setup information.
  }

  GetClockSettings();

  UpdateSubmenuStrings(CLOCK1, mRangesBclk);
  UpdateSubmenuStrings(CLOCK0, mRangesPcie);

  //
  // Use data retrieved from MBP
  //
  Size = sizeof(gIccMbpData);
  Status = gRT->GetVariable (ICC_MBP_DATA_NAME, &gIccGuid, NULL, &Size, &gIccMbpData);

  IccSetup.Profile = gIccMbpData.Profile;
  IccSetup.RegLock = gIccMbpData.RegLock;

  UpdateClockData (mSettingsBclk, CLOCK1, &IccSetup);
  UpdateClockData (mSettingsPcie, CLOCK0, &IccSetup);
  UpdateVisibility(&IccSetup);

  gRT->SetVariable (
         ICC_SETUP_DATA_C_NAME,
         &gIccGuid,
// AMI_OVERRIDE_START - Fix ICC BCLK function and fix ICC BCLK setup information.
#if 0
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
#else
         (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
#endif
// AMI_OVERRIDE_END - Fix ICC BCLK function and fix ICC BCLK setup information.
         sizeof(IccSetup),
         &IccSetup
         );

  DebugDumpConfig(IccSetup);

}

VOID
IccRouteConfig (
// AMI_OVERRIDE_START - For our notify event function design.
  EFI_EVENT         Event,
  VOID              *Context
// AMI_OVERRIDE_END - For our notify event function design. 
  )
{
  EFI_STATUS                        Status;
  ICC_SETUP_DATA                    IccSetup;
  UINTN                             Size = sizeof(ICC_SETUP_DATA);
  UINTN                             VariableSize = sizeof(SA_SETUP);
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  ClockSettings;
  UINT32                            VariableAttributes;
  VOID                              *Interface;
  UINT8                             gWdtTimeOut = 0;
  WDT_PROTOCOL                      *gWdtDxe;
    
  Status = gBS->LocateProtocol (&gAmiTseNVRAMUpdateGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  DEBUG ((DEBUG_INFO, "IccRouteConfig\n"));  
  
  Status = gBS->LocateProtocol (
                &gWdtProtocolGuid,
                NULL,
                (VOID **) &gWdtDxe
                );
  if (!EFI_ERROR (Status)) {
      gWdtTimeOut = gWdtDxe->CheckStatus ();
  }
// AMI_OVERRIDE_END - Fix BCLK value kept old value in ME configuration when BCLK's WDT happened

  ZeroMem ((VOID *) &IccSetup, sizeof (ICC_SETUP_DATA));
  if (MeIsAfterEndOfPost () || mHeci == NULL) {
    return;
  }

  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &mSaSetup
                  );
  ASSERT_EFI_ERROR (Status);

  gRT->GetVariable (ICC_SETUP_DATA_C_NAME, &gIccGuid, NULL, &Size, &IccSetup);
  MaybeChangeProfile(IccSetup.Profile);
  MaybeChangeRegLock(IccSetup.RegLock);

  SetMem(&ClockSettings, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP), 0);
  if (!mProfileChanged && IccSetup.AllowAdvancedOptions) {
    //
    //  Update BCLK settings if needed
    //
#if 0 // AMI_OVERRIDE_START - Fix BCLK value kept old value in ME configuration when BCLK's WDT happened
    if (IccSetup.ShowClock[CLOCK1] && gClockChanged[CLOCK1]) {
#else
    if ((IccSetup.ShowClock[CLOCK1] && gClockChanged[CLOCK1]) || gWdtTimeOut) {
#endif // AMI_OVERRIDE_END - Fix BCLK value kept old value in ME configuration when BCLK's WDT happened
      gIccReset = TRUE;
      gClockChanged[CLOCK1] = FALSE;
      ExtractClockData(&IccSetup, CLOCK1, &ClockSettings);

      ///
      /// Set permanent and applied on warm reset flags based on Bclk Change Type
      ///
      if (mSaSetup.BclkChangeWarmReset) {
        ClockSettings.CurrentFlags.Flags.Type = 0;
        ClockSettings.CurrentFlags.Flags.AppliedOnWarmRst = 1;
        gIccReset = TRUE;
      } else {
        switch (mSaSetup.BclkChangeContinues) {
          case ICC_BCLK_PERM_NO_WR:
            ClockSettings.CurrentFlags.Flags.Type = 0;
            ClockSettings.CurrentFlags.Flags.AppliedOnWarmRst = 0;
            gIccReset = FALSE;
            break;
  
          case ICC_BCLK_REAL_TIME:
            ClockSettings.CurrentFlags.Flags.Type = 1;
            ClockSettings.CurrentFlags.Flags.AppliedOnWarmRst = 0;
            gIccReset = FALSE;
            break;
  
           default:
            break;
        }
      }
      ApplyClockSettings(&ClockSettings);
    }
  }
// AMI_OVERRIDE_START - For our notify event function design.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }  
// AMI_OVERRIDE_END - For our notify event function design.

}

VOID
MaybeChangeProfile (
  UINT8 Profile
  )
{
  static UINT8 LastSavedProfile;

  if (!mProfileChanged) {
    LastSavedProfile = gIccMbpData.Profile;
  }
  if (Profile != LastSavedProfile) {
    DEBUG ((DEBUG_INFO, "(ICC) Changing profile: old %d, new %d\n", LastSavedProfile, Profile));
    HeciSetIccProfile (Profile);
    LastSavedProfile = Profile;
    mProfileChanged = TRUE;
    gIccReset = TRUE;
  }
}

VOID
MaybeChangeRegLock (
  UINT8 RegLock
  )
{
  static UINT8        LastSavedRegLock = NOT_INITIALIZED;

  if (LastSavedRegLock == NOT_INITIALIZED) {
    LastSavedRegLock = gIccMbpData.RegLock;
  }
  if (RegLock != LastSavedRegLock) {
    DEBUG ((DEBUG_INFO, "(ICC) Changing RegLock: old %d, new %d\n", LastSavedRegLock, RegLock));
    HeciSetIccRegLock (RegLock);
  }

  LastSavedRegLock = RegLock;
}

// AMI_OVERRIDE_START - For our callback function design.
VOID
InitIccSetupCallback (
  VOID
  )
{
  VOID    *Registration;    
  
  AmiSetupRegisterCallbackHandler (KEY_ICC_PROFILE, IccProfileCallback);
  AmiSetupRegisterCallbackHandler (KEY_ICC_FREQ2, IccCallback);
  AmiSetupRegisterCallbackHandler (KEY_ICC_FREQ3, IccCallback);
  AmiSetupRegisterCallbackHandler (KEY_ICC_SPREAD2, IccCallback);
  AmiSetupRegisterCallbackHandler (KEY_ICC_SPREAD3, IccCallback);
  
  EfiCreateProtocolNotifyEvent (
           &gAmiTseNVRAMUpdateGuid,
           TPL_NOTIFY,
           IccRouteConfig,
           NULL,
           &Registration
           );  
}
// AMI_OVERRIDE_END - For our callback function design.
