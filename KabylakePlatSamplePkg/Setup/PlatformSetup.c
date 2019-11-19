/** @file
  Platform Setup Routines

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2018 Intel Corporation.

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

#include "SetupPrivate.h"
#include <Library/MmPciLib.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/Wdt.h>

#include <Txt.h>
#include <SaAccess.h>
#include <PchAccess.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <PlatformBoardId.h>
#include <PlatformInfo.h>
#include <Protocol/I2cIo.h>
#include <Library/GpioLib.h>
#include <SystemConfigVariable.h>

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/CmosAccessLib.h>
#include <BootState.h>
#include <Protocol/MePlatformGetResetType.h>
#include <Protocol/UsbIo.h>
#include <IndustryStandard/Usb.h>

#include <PlatformBoardConfig.h>
// AMI_OVERRIDE_START - For our Setup variable design.
#include "AmiSetupPrivate.h"
// AMI_OVERRIDE_END - For our Setup variable design.

#include <Board/BoardConfigInit.h>
//
// ME/AMT Protocol
//
#include <Protocol/HeciProtocol.h>
#include "MeSetup.h"
#include <CoreBiosMsg.h>
#include "IccSetup.h"
#include <IccSetupData.h>
#include <Library/DxeMeLib.h>

extern BOOLEAN   mMeReset;
extern BOOLEAN   gIccReset;

static const UINT16 SECONDSCREEN_VID                                   = 0x8087;
static const UINT16 SECONDSCREEN_EINK_PID                              = 0x0A06;
static const UINT8  SECONDSCREEN_EINK_IFACE                            = 0;
static const UINT8  SECONDSCREEN_FEATURE_DISPLAY_ATTRIBUTES_REPORT_ID  = 1;
static BOOLEAN      gSTMDevFwFind = FALSE;
// AMI_OVERRIDE_START - For PlatformReset function design.
ME_PLATFORM_GET_RESET_TYPE_PROTOCOL mMePlatformGetResetType;
static SETUP_DATA           gNewSetupData;
static SETUP_DATA           gOldSetupData;

#define SaCurrentData       gNewSetupData
#define SaSetup             gOldSetupData
#define CpuCurrentData      gNewSetupData
#define CpuSetup            gOldSetupData
#define PchCurrentData      gNewSetupData
#define PchSetup            gOldSetupData
// AMI_OVERRIDE_END - For PlatformReset function design.

//
// BCD to Decimal Conversion
//
#define BCD_TO_DECIMAL(A)           (((((A) >> 4) & 0xF) * 10) + ((A) & 0xF))

#define SENSOR_HUB_FW_VERSION_OFFSET 0x1000
#define SENSOR_HUB_FW_VERSION_LENGTH 0x04

GLOBAL_REMOVE_IF_UNREFERENCED EFI_HII_HANDLE            gPsHiiHandle = INVALID_HII_HANDLE;

//
// LAN PHY Revision definitions
//
#define PHY_CONFIG_REG    0x00000020
#define PHY_SEMAPHORE_REG 0x00000F00
#define PHY_PAGE769_SET_REG 0x43f6020
#define PHY_SLOW_MDIO_MODE_REG 0x4302580
#define PHY_READ_PHY_OFFSET3_REG 0x8430000
#define PHY_FAST_MDIO_MODE_REG 0x4302180
#define LAN_PHY_REV_TC 0xA0
#define LAN_PHY_REV_A0 0xA1
#define LAN_PHY_REV_A1 0xA2
#define LAN_PHY_REV_A2 0xA3
#define LAN_PHY_REV_A3 0xA4
#define LAN_PHY_REV_B1 0xA5
#define LAN_PHY_REV_B2 0xA6

// Max loop value for GBE check
#define GBE_MAX_LOOP_TIME 4000
typedef struct _LAN_PHY_INFO {
  UINT16 LanPhyRev;
  char *LanPhyString;
} LAN_PHY_INFO;

#pragma pack(push, 1)
typedef struct {
  UINT8       ReportId;
  struct
  {
    UINT16  Width;
    UINT16  Height;
  } Dimensions;
  UINT8       BitDepth;
  UINT8       Orientation : 2;
  UINT8       SupportsCS : 1;
  UINT8       : 5; // padding
  UINT8       NumberOfBuffers;
  UINT8       NumberOfButtons;
  UINT8       NumberOfFingers;
  UINT8       NumberOfFrontLights;
  UINT32      FirmwareVersion;
  UINT32      PipSizeInBytes;
} HID_FEATURE_REPORT_DISPLAY_ATTRIBUTES;
#pragma pack(pop)

LAN_PHY_INFO LanPhyInfoTable[] = {
  {LAN_PHY_REV_TC, "(TC Stepping)"},
  {LAN_PHY_REV_A0, "(A0 Stepping)"},
  {LAN_PHY_REV_A1, "(A1 Stepping)"},
  {LAN_PHY_REV_A2, "(A2 Stepping)"},
  {LAN_PHY_REV_A3, "(A3 Stepping)"},
  {LAN_PHY_REV_B1, "(B1 Stepping)"},
  {LAN_PHY_REV_B2, "(B2 Stepping)"}
};

VOID
EFIAPI
CallbackGetLanPhyRevision(
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  );

VOID
EFIAPI
CallbackGetSensorHubFwVersion(
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  );

// AMI_OVERRIDE_START - For AMI Callback function design.
EFI_STATUS
EFIAPI
CnvFormCallBackFunction (
#if 0 
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif 
  );
// AMI_OVERRIDE_END - For AMI Callback function design.  
// AMI_OVERRIDE_START - For PlatformReset function design.
ME_SPECIAL_RESET_TYPES
PlatformReset (
  IN  EFI_RESET_TYPE       PlatformResetType
  );

VOID 
RcProcessEnterSetup (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 VariableSize;
  EFI_HANDLE                            PlatformResetHandle;     
  
  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &gOldSetupData
                  );
  
  if (!EFI_ERROR (Status)) {  
    //
    // Install the protocol on a new handle.
    //
    PlatformResetHandle = NULL;
    mMePlatformGetResetType.Revision       = ME_PLATFORM_GET_RESET_TYPE_PROTOCOL_REVISION;
    mMePlatformGetResetType.MeGetResetType = PlatformReset;
    
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &PlatformResetHandle,
                    &gMePlatformGetResetTypeGuid,  &mMePlatformGetResetType,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
    
    DEBUG ((DEBUG_INFO, "PlatformSetup: PlatformReset was installed\n"));
  }
}

VOID
MeResetHook (
  VOID
  )
{          
  if (gNewSetupData.TpmDeviceSelection != gOldSetupData.TpmDeviceSelection) {
    mMeReset = TRUE;
    
    if (gNewSetupData.TpmDeviceSelection == 1) {
      PttHeciSetState(TRUE);
    }
    else {
      PttHeciSetState(FALSE);
    }
  } 
  return;    
}
// AMI_OVERRIDE_END - For PlatformReset function design.

/**
  Display ICC Profile Data

  @retval   EFI_SUCCESS       - Function successfully finishes.
**/
EFI_STATUS
EFIAPI
DisplayIccProfileData (
  VOID
  )
{
  ICC_SETUP_DATA IccSetup;
  EFI_STRING     RequestString;
  UINTN          DataSize;

  DataSize = sizeof(IccSetup);

  //
  // Icc Setup variable will be created only if we enter into Bios Setup.
  // Expose All the Icc Setup Data to BIOS CONFIG TOOL irrespective of the variable presence
  //
  HiiGetBrowserData (&gIccGuid, ICC_SETUP_DATA_C_NAME, sizeof(ICC_SETUP_DATA), (UINT8 *)&IccSetup);
  IccSetup.AllowAdvancedOptions  = DISPLAY;
  IccSetup.ShowClock[CLOCK1]     = DISPLAY;
  IccSetup.ShowFrequency[CLOCK1] = DISPLAY;
  IccSetup.ShowSpread[CLOCK1]    = DISPLAY;
  IccSetup.ShowClock[CLOCK0]     = DISPLAY;
  IccSetup.ShowFrequency[CLOCK0] = DISPLAY;
  IccSetup.ShowSpread[CLOCK0]    = DISPLAY;

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

  RequestString = NULL;
  HiiSetBrowserData(&gIccGuid, ICC_SETUP_DATA_C_NAME, sizeof(ICC_SETUP_DATA), (UINT8 *)&IccSetup, RequestString);

  return EFI_SUCCESS;
}

/**
  Acquire the string associated with the Index from smbios structure and return it.
  The caller is responsible for free the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS       The function returns EFI_SUCCESS always.
**/
EFI_STATUS
GetSmBiosStringByIndex (
  IN   CHAR8                   *OptionalStrStart,
  IN   UINT8                   Index,
  OUT  CHAR8                   **String
  )
{
  UINTN          StrSize;

  if (Index == 0) {
    *String = AllocateZeroPool (sizeof (CHAR8));
    return EFI_SUCCESS;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart);
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  *String = AllocatePool (StrSize);
  ASSERT (*String != NULL);
  if (*String != NULL) {
    CopyMem (*String, OptionalStrStart, StrSize);
  }
  return EFI_SUCCESS;
}

/**
  Platform Setup Callback

  @param[in] Event         - Event based on EFI_SETUP_ENTER_GUID.
  @param[in] Context       - Pointer to the notification function's context.

**/
VOID
EFIAPI
PlatformSetupCallback (
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
  EFI_STATUS                      Status;
  UINT8                           StrIndex;
  CHAR8                           *NewString;
  UINT8                           RevisionHigh;
  UINT8                           RevisionLow;
  BOOLEAN                         BoardInfo;
  BOOLEAN                         BiosInfo;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  EFI_SMBIOS_PROTOCOL             *Smbios;
  SMBIOS_TABLE_TYPE2              *Type2Record;
  SMBIOS_TABLE_TYPE0              *Type0Record;
  EFI_SMBIOS_TABLE_HEADER         *Record;
  PLATFORM_INFO                   *PlatformInfo = NULL;

  gBS->CloseEvent (Event);
  BoardInfo = FALSE;
  BiosInfo = FALSE;

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION) {
      Type2Record = (SMBIOS_TABLE_TYPE2 *) Record;
      //Update Board ID
      StrIndex = Type2Record->ProductName;
      GetSmBiosStringByIndex ((CHAR8*)((UINT8*)Type2Record + Type2Record->Hdr.Length), StrIndex, &NewString);
      InitString (gPsHiiHandle,STRING_TOKEN (STR_BOARD_ID_VALUE),L"%a%",NewString);
      FreePool (NewString);
      //Update Fab ID
      StrIndex = Type2Record->Version;
      GetSmBiosStringByIndex ((CHAR8*)((UINT8*)Type2Record + Type2Record->Hdr.Length), StrIndex, &NewString);
      InitString (gPsHiiHandle,STRING_TOKEN (STR_FAB_ID_VALUE),L"%a%",NewString);
      FreePool (NewString);
      BoardInfo = TRUE;
    }
    if (Record->Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION) {
      Type0Record = (SMBIOS_TABLE_TYPE0 *) Record;
      //Update EC FirmwareMajorRelease
      RevisionHigh = Type0Record->EmbeddedControllerFirmwareMajorRelease;
      //Update EC FirmwareMinorRelease
      RevisionLow = Type0Record->EmbeddedControllerFirmwareMinorRelease;
      InitString (gPsHiiHandle,STRING_TOKEN (STR_CHIP_EC_REV_VALUE),L"%02d%a%02d",RevisionHigh,".",RevisionLow);
      BiosInfo = TRUE;
    }
  } while (!(BoardInfo && BiosInfo));

  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
  ASSERT_EFI_ERROR (Status);
  if ((!EFI_ERROR (Status)) && (PlatformInfo->BoardType == BoardTypeSds) && (PlatformInfo->DockAttached == 1)) {
    RevisionHigh = BCD_TO_DECIMAL(PlatformInfo->BaseEcFwMajorVersion); // Update base EC FW major version
    RevisionLow  = BCD_TO_DECIMAL(PlatformInfo->BaseEcFwMinorVersion); // Update base EC FW minor version
    InitString (gPsHiiHandle, STRING_TOKEN (STR_CHIP_BASE_EC_FW_REV_VALUE), L"%02d%a%02d", RevisionHigh, ".", RevisionLow);

    RevisionHigh = BCD_TO_DECIMAL(PlatformInfo->EcProtocolVersion); // Update EC protocol version
    InitString (gPsHiiHandle, STRING_TOKEN (STR_CHIP_EC_PROTOCOL_REV_VALUE), L"%02d", RevisionHigh);

    RevisionLow  = BCD_TO_DECIMAL(PlatformInfo->BaseEcProtocolVersion);  // Update base EC protocol version
    InitString (gPsHiiHandle, STRING_TOKEN (STR_CHIP_BASE_EC_PROTOCOL_REV_VALUE), L"%02d", RevisionLow);
  }
  
// AMI_OVERRIDE_START - Create EC version checking.
  InitString (gPsHiiHandle,STRING_TOKEN (STR_CHIP_EC_REV_VALUE),L"%02d%a%02d",PlatformInfo->EcMajorRevision,".",PlatformInfo->EcMinorRevision);
// AMI_OVERRIDE_END - Create EC version checking.
}

// AMI_OVERRIDE_START - No need to show SETUP data revision in AMI setup items.
#if 0
VOID
UpdateSetUpVarRevisionString (
  EFI_HII_HANDLE HiiHandle
  )
{
  EFI_STATUS Status;
  SA_SETUP SaSetup;
  CPU_SETUP CpuSetup;
  PCH_SETUP PchSetup;
  ME_SETUP MeSetup;
  SETUP_DATA SetupData;
  UINTN VarSize;

  VarSize = sizeof (ME_SETUP);
  Status = gRT->GetVariable (
                  L"MeSetup",
                  &gMeSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &MeSetup
                  );

  if (!EFI_ERROR (Status)) {
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_ME_SETUP_REVISION_VALUE),
      L"%d",
      MeSetup.Revision
      );
  }

  VarSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &CpuSetup
                  );

  if (!EFI_ERROR (Status)) {
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_CPU_SETUP_REVISION_VALUE),
      L"%d",
      CpuSetup.Revision
      );
  }

  VarSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SaSetup
                  );

  if (!EFI_ERROR (Status)) {
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_SA_SETUP_REVISION_VALUE),
      L"%d",
      SaSetup.Revision
      );
  }

  VarSize = sizeof (PCH_SETUP);
  Status = gRT->GetVariable (
                  L"PchSetup",
                  &gPchSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &PchSetup
                  );

  if (!EFI_ERROR (Status)) {
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_PCH_SETUP_REVISION_VALUE),
      L"%d",
      PchSetup.Revision
      );
  }


  VarSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );

  if (!EFI_ERROR (Status)) {
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_SETUP_DATA_REVISION_VALUE),
      L"%d",
      SetupData.Revision
      );
  }
}
#endif
// AMI_OVERRIDE_END - No need to show SETUP data revision in AMI setup items.

/**
  Initialize Platform Strings

  @param[in] HiiHandle    HiiHandle Handle to HII database
  @param[in] Class        Indicates the setup class

**/
VOID
InitPlatformStrings (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  )
{
  EFI_STATUS            Status;
  UINTN                 VariableSize;
#if 0  // AMI_OVERRIDE_START - Tse handle it.
  EFI_BOOT_MODE         BootMode;
#endif // AMI_OVERRIDE_END - Tse handle it.
  ME_INFO_SETUP_DATA    MeInfoSetupData;
  UINT8                 *FwMeSku[] = {
     (UINT8 *) ME_FW_IMAGE_TYPE_CONSUMER_STRING,
     (UINT8 *) ME_FW_IMAGE_TYPE_CORPORATE_STRING,
     (UINT8 *) ME_FW_IMAGE_TYPE_UNIDENTIFIED_STRING,
  };
  UINT8                 *MeFwSkuValue;
#if 0  // AMI_OVERRIDE_START - Tse handle it.
  EFI_EVENT             SetupEnterEvent;
  VOID                  *pSetupRegistration;
  EFI_EVENT             PlatformSetupEvent;
  VOID                  *Registration;
  EFI_EVENT             SensorFwVersionSetupEvent;
  VOID                  *SensorFwVersionRegistration;
  BOOLEAN               IsTriggeredAtSetupMenu;
#endif // AMI_OVERRIDE_END - Tse handle it.
  BOOLEAN               HiiDataExport;
  BOOLEAN               XmlCliEnabled;
  BOARD_PRIVATE_DATA    *BoardInfo;

  BoardInfo = (BOARD_PRIVATE_DATA *) PcdGetPtr (PcdBoardPrivateData);

  if (Class == MAIN_FORM_SET_CLASS) {

    DEBUG ((DEBUG_INFO, "<InitPlatformStrings>"));
    HiiDataExport = FALSE;
    XmlCliEnabled = FALSE;

    //
    // Preserve HiiHandle to access Strings in callbacks
    //
    if (gPsHiiHandle == INVALID_HII_HANDLE) {
      gPsHiiHandle = HiiHandle;
    }

    VariableSize = sizeof (ME_INFO_SETUP_DATA);
    Status = gRT->GetVariable (
                    L"MeInfoSetup",
                    &gMeInfoSetupGuid,
                    NULL,
                    &VariableSize,
                    &MeInfoSetupData
                    );
    if (!EFI_ERROR (Status)) {
      InitString (
        HiiHandle,
        STRING_TOKEN (STR_ME_FW_REV_VALUE),
        L"%d.%d.%d.%d",
        MeInfoSetupData.MeMajor,
        MeInfoSetupData.MeMinor,
        MeInfoSetupData.MeHotFix,
        MeInfoSetupData.MeBuildNo
        );
    }


#if 0 // AMI_OVERRIDE_START - Tse handle it.

    //
    // Check if InitPlatformStrings is triggered via mSetupEnterGuid
    //
    IsTriggeredAtSetupMenu = FALSE;
    VariableSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable (
                    L"Setup",
                    &gSetupVariableGuid,
                    NULL,
                    &VariableSize,
                    &mSetupData
                    );
    ASSERT_EFI_ERROR(Status);
    if (!EFI_ERROR (Status) && mSetupData.FastBoot) {
      BootMode = GetBootModeHob();
      if ((mSetupData.SkipHIIUpdate) &&
          ((BootMode == BOOT_WITH_MINIMAL_CONFIGURATION) ||
           (BootMode == BOOT_ON_S4_RESUME) ||
           (BootMode == BOOT_ASSUMING_NO_CONFIGURATION_CHANGES))) {
        IsTriggeredAtSetupMenu = TRUE;
      }
    }

    //
    // Call routine to calculate LAN PHY Revision when entering Setup UI
    //
    if (IsTriggeredAtSetupMenu == TRUE) {
      PlatformSetupCallback (NULL, NULL);
      CallbackGetLanPhyRevision (NULL, NULL);
      CallbackGetSensorHubFwVersion (NULL, NULL);
    } else {
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      PlatformSetupCallback,
                      NULL,
                      &PlatformSetupEvent
                      );
      ASSERT_EFI_ERROR (Status);

      Status = gBS->RegisterProtocolNotify (
                      &gSetupEnterGuid,
                      PlatformSetupEvent,
                      &Registration
                      );
      ASSERT_EFI_ERROR (Status);

      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      CallbackGetLanPhyRevision,
                      NULL,
                      &SetupEnterEvent
                      );
      ASSERT_EFI_ERROR(Status);

      Status = gBS->RegisterProtocolNotify (
                      &gSetupEnterGuid,
                      SetupEnterEvent,
                      &pSetupRegistration
                      );
      ASSERT_EFI_ERROR(Status);

      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      CallbackGetSensorHubFwVersion,
                      NULL,
                      &SensorFwVersionSetupEvent
                      );
      ASSERT_EFI_ERROR(Status);

      Status = gBS->RegisterProtocolNotify (
                      &gSetupEnterGuid,
                      SensorFwVersionSetupEvent,
                      &SensorFwVersionRegistration
                      );
      ASSERT_EFI_ERROR(Status);
    }
#endif // AMI_OVERRIDE_END - Tse handle it.

    if (mMeSetup.MeImageType == ME_IMAGE_CONSUMER_SKU_FW) {
      MeFwSkuValue = FwMeSku[0];
    } else if (mMeSetup.MeImageType == ME_IMAGE_CORPORATE_SKU_FW) {
      MeFwSkuValue = FwMeSku[1];
    } else  {
      MeFwSkuValue = FwMeSku[2];
    }
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_ME_FW_SKU_VALUE),
      L"%a",
      MeFwSkuValue
      );

    //
    // Platform/PCH TXT capability
    //
    if (MmioRead32(TXT_PUBLIC_BASE + 0x10) & BIT0) {
      InitString (
        HiiHandle,
        STRING_TOKEN (STR_PLATFORM_TXT_VALUE),
        L"%a",
        "Supported"
        );
    } else {
      InitString (
        HiiHandle,
        STRING_TOKEN (STR_PLATFORM_TXT_VALUE),
        L"%a",
        "Unsupported"
        );
    }

    //
    // Production Type
    //
    if (MmioRead32(TXT_PUBLIC_BASE + 0x200) & BIT31) {
      InitString (
        HiiHandle,
        STRING_TOKEN (STR_PRODUCTION_TYPE_VALUE),
        L"%a",
        "Production"
        );
    } else {
      InitString (
        HiiHandle,
        STRING_TOKEN (STR_PRODUCTION_TYPE_VALUE),
        L"%a",
        "Pre-Production"
        );
    }

    //
    // if HiiExport variable is present bios will update the BoardID, FabID, KSC version and LanPhyversion without entering into setup
    //
#if 0  // AMI_OVERRIDE_START - Tse handle it.
    if (HiiDataExport || XmlCliEnabled) {
#endif // AMI_OVERRIDE_END - Tse handle it.
      PlatformSetupCallback (NULL, NULL);
      CallbackGetLanPhyRevision (NULL, NULL);
      CallbackGetSensorHubFwVersion (NULL, NULL);
      DisplayIccProfileData ();
#if 0  // AMI_OVERRIDE_START - Tse handle it.
    }
#endif // AMI_OVERRIDE_END - Tse handle it.
  }

// AMI_OVERRIDE_START - No need to show SETUP data revision in AMI setup items.
#if 0
  UpdateSetUpVarRevisionString (HiiHandle);
#endif
// AMI_OVERRIDE_END - No need to show SETUP data revision in AMI setup items.

// AMI_OVERRIDE_START - For PlatformReset function design.
  RcProcessEnterSetup ();
// AMI_OVERRIDE_END - For PlatformReset function design.
// AMI_OVERRIDE_START - For AMI TSE callback design
  AmiSetupRegisterCallbackHandler (KEY_WIFI_POWER_LIMIT, CnvFormCallBackFunction);
// AMI_OVERRIDE_END - For AMI TSE callback design

}

/**
  Setup callback executed when user selects a Connectivity from the BIOS UI.
  Changes visibility and settings of other options.

  @param[in] This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in] Action          Specifies the type of action taken by the browser.
  @param[in] KeyValue        A unique value which is sent to the original exporting driver
                             so that it can identify the type of data to expect.
  @param[in] Type            The type of value for the question.
  @param[in] Value           A pointer to the data being sent to the original exporting driver.
  @param[out] ActionRequest  On return, points to the action requested by the callback function.

  @retval EFI_SUCCESS        The callback successfully handled the action
  @retval EFI_UNSUPPORTED    The specified action is not supported by the callback
**/
EFI_STATUS
EFIAPI
CnvFormCallBackFunction (
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
  SETUP_DATA SetupData;
  UINTN      VarSize;
  EFI_STRING RequestString;
  EFI_STATUS Status;
// AMI_OVERRIDE_START - For our Callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
    
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
// AMI_OVERRIDE_END - For our Callback function design.

  RequestString = NULL;

  if ((Action != EFI_BROWSER_ACTION_CHANGING) && (Action != EFI_BROWSER_ACTION_CHANGED)) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "CnvFormCallBackFunction ()\n"));
  VarSize = sizeof (SETUP_DATA);
  Status = HiiGetBrowserData (&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) &SetupData);
  ASSERT_EFI_ERROR(Status);


  if ((SetupData.WifiPowerLimit1 > 10000) && (SetupData.WifiPowerLimit1 < 0xFFFF)) {
    SetupData.WifiPowerLimit1 = 0xFFFF;
    RequestString = HiiConstructRequestString(RequestString, OFFSET_OF(SETUP_DATA, WifiPowerLimit1), sizeof(SetupData.WifiPowerLimit1));
    if (RequestString != NULL) {
      Status = HiiSetBrowserData (&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) &SetupData, RequestString);
      ASSERT_EFI_ERROR(Status);
      FreePool (RequestString);
    }
  }

  return EFI_SUCCESS;
}

#if 0  // AMI_OVERRIDE_START - We don't use this callback function.
/**
  Call Back function for retrieves STM32 Firmware version

  @param[in] HiiHandle  Handle to HII database
  @param[in] Class      Indicates the setup class
  @param[in] SubClass   Indicates setup Subclass
  @param[in] Key        Key of Setup token

**/
VOID
GetSTM32FwVersionCallback (
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Class,
  IN UINT16         SubClass,
  IN UINT16         Key
  )
{
  EFI_STATUS                      Status;
  PLATFORM_INFO                   *PlatformInfo = NULL;
  EFI_HANDLE                      *HandleArray = NULL;
  UINTN                           HandleArrayCount;
  EFI_USB_IO_PROTOCOL             *UsbIoProtocol = NULL;
  UINTN                           HandleIndex;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_USB_DEVICE_DESCRIPTOR       DeviceDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR    InterfaceDescriptor;
  EFI_USB_DEVICE_REQUEST          Request;
  UINT32                          TransferStatus;
  HID_FEATURE_REPORT_DISPLAY_ATTRIBUTES   FeatureReport;

  if (!gSTMDevFwFind) {
    Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);

    if (PlatformInfo->BoardType == BoardTypeSds) {
      // Get all of the handles that support the USB IO protocol
      Status = gBS->LocateHandleBuffer(
                      ByProtocol,
                      &gEfiUsbIoProtocolGuid,
                      NULL,
                      &HandleArrayCount,
                      &HandleArray
                      );

      if (!EFI_ERROR(Status)) {
        // For each handle, we are going to see if it is the Second Screen device we want.
        for (HandleIndex = 0; HandleIndex < HandleArrayCount; ++HandleIndex) {
          DevicePath = DevicePathFromHandle(HandleArray[HandleIndex]);
          if (!DevicePath) {
            continue;
          }
          // Until we hit a device end not in the device path, look until we find
          // a USB node.  If we find a USB node, go ahead and get an open handle
          // to the protocol and get the USB device descriptor.  If the VID and PID
          // match the Second Screen VID/PID we have our device.
          while (!IsDevicePathEnd(DevicePath)) {
            if ((MESSAGING_DEVICE_PATH == DevicePathType(DevicePath)) &&
                (MSG_USB_DP == DevicePathSubType(DevicePath))) {
              // Connect to the controller
              gBS->ConnectController(HandleArray[HandleIndex], NULL, NULL, FALSE);

              // Open the USB IO protocol so we can talk to our device
              Status = gBS->OpenProtocol(
                              HandleArray[HandleIndex],
                              &gEfiUsbIoProtocolGuid,
                              (VOID **) &UsbIoProtocol,
                              NULL,
                              NULL,
                              EFI_OPEN_PROTOCOL_GET_PROTOCOL
                              );
              if (EFI_ERROR(Status)) {
                break;
              }
              // Get the device descriptor from the device
              Status = UsbIoProtocol->UsbGetDeviceDescriptor(
                                        UsbIoProtocol,
                                        &DeviceDescriptor
                                        );
              if (EFI_ERROR(Status)) {
                break;
              }

              // See if the VID and PID match our device
              if ((SECONDSCREEN_VID == DeviceDescriptor.IdVendor) &&
                  (SECONDSCREEN_EINK_PID == DeviceDescriptor.IdProduct)) {
                // Since the Second Screen device has multiple interfaces, we need to
                // make sure that we are talking to the correct one for the eInk screen.
                Status = UsbIoProtocol->UsbGetInterfaceDescriptor(
                                          UsbIoProtocol,
                                          &InterfaceDescriptor
                                          );

                if ((EFI_SUCCESS == Status) &&
                    (SECONDSCREEN_EINK_IFACE == InterfaceDescriptor.InterfaceNumber)) {
                  // Zero out the local feature report and set the report ID appropriately.
                  ZeroMem(&FeatureReport, sizeof(FeatureReport));
                  FeatureReport.ReportId = SECONDSCREEN_FEATURE_DISPLAY_ATTRIBUTES_REPORT_ID;
                  // Construct the control transfer request:
                  // HID get feature report
                  Request.RequestType   = USB_HID_CLASS_GET_REQ_TYPE;
                  Request.Request       = EFI_USB_GET_REPORT_REQUEST;
                  Request.Value         = (UINT16) ((HID_FEATURE_REPORT << 8) |
                                          SECONDSCREEN_FEATURE_DISPLAY_ATTRIBUTES_REPORT_ID);
                  Request.Index         = 0;
                  Request.Length        = (UINT16) sizeof(FeatureReport);
                  // Issue the request to get the feature report
                  Status = UsbIoProtocol->UsbControlTransfer (
                                            UsbIoProtocol,
                                            &Request,
                                            EfiUsbDataIn,
                                            3000,
                                            &FeatureReport,
                                            sizeof(FeatureReport),
                                            &TransferStatus
                                            );
                  if (EFI_ERROR(Status)) {
                    break;
                  }
                  // Here is where you will grab the firmware version from the feature report.
                  // You can also probably stop looping as we have found our device.
                  InitString (gPsHiiHandle, STRING_TOKEN (STR_CHIP_STM_REV_VALUE), L"%d", FeatureReport.FirmwareVersion);
                  gSTMDevFwFind = TRUE;
                  break;
                }
              }
            }
            DevicePath = NextDevicePathNode(DevicePath);
          }
        }
      }
      if (HandleArray != NULL) {
        FreePool(HandleArray);
      }
    }
  }
}
#endif // AMI_OVERRIDE_END - We don't use this callback function.

/**
  Retrieves LAN PHY Revision

  @param[in] Event         - Event based on EFI_SETUP_ENTER_GUID.
  @param[in] Context       - Pointer to the notification function's context.

  @retval EFI_SUCCESS
  @retval EFI_TIMEOUT

**/
EFI_STATUS
GetPhyVersion(
  UINT32 GbEBar,
  UINT8  *LanPhyRev
  )
{
  UINT32          RegisterValue = 0;
  UINTN           LoopTime;

  //
  // Write PHY_CONFIG_REG with set page 769
  //
  MmioWrite32((GbEBar + PHY_CONFIG_REG), PHY_PAGE769_SET_REG);
  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_CONFIG_REG);

    if (RegisterValue & BIT28) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }
  //
  // Delay 4ms after page change
  //
  gBS->Stall(4000);

  //
  // Write PHY_CONFIG_REG with slow MDIO mode
  //
  MmioWrite32((GbEBar + PHY_CONFIG_REG), PHY_SLOW_MDIO_MODE_REG);
  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_CONFIG_REG);
    if (RegisterValue & BIT28) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  //
  // Read register PHY Version (offset 0x3)
  //
  MmioWrite32((GbEBar + PHY_CONFIG_REG), PHY_READ_PHY_OFFSET3_REG);
  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_CONFIG_REG);
    if (RegisterValue & BIT28) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }
  //
  // Read LAN PHY revision from PHY register 3
  //
  *LanPhyRev = (UINT8)MmioRead16(GbEBar + PHY_CONFIG_REG);

  //
  // Write PHY_CONFIG_REG to write fast mdio mode
  //
  MmioWrite32((GbEBar + PHY_CONFIG_REG), PHY_FAST_MDIO_MODE_REG);
  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_CONFIG_REG);
    if (RegisterValue & BIT28) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  Release GbeSempahore

  @param[in] GbEBar    - GbE Base Address

**/
VOID
ReleaseGbeSemaphore(
  IN  UINT32      GbEBar
  )
{
  MmioAnd32 ((GbEBar + PHY_SEMAPHORE_REG), (UINT32) ~BIT5);
}

/**
  Acquire GbeSempahore

  @param[in] GbEBar        GbE Base Address

  @retval EFI_SUCCESS      Success to excute Gbe Semaphore
  @retval EFI_TIMEOUT      The operation is time out.

**/
EFI_STATUS
AcquireGbeSemaphore(
  IN  UINT32      GbEBar
  )
{
  UINTN           LoopTime;
  UINT32          RegisterValue = 0;

  MmioOr32 ((GbEBar + PHY_SEMAPHORE_REG), BIT5);

  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_SEMAPHORE_REG);

    if (RegisterValue & BIT5) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    DEBUG((DEBUG_WARN, "Unable to acquire Gbe semaphore \n"));
    //
    // Always release Gbe Semaphore
    //
    ReleaseGbeSemaphore (GbEBar);
    return EFI_TIMEOUT;
  }
  return EFI_SUCCESS;
}

/**
  Retrieves LAN PHY Revision

  @param[in] Event         - Event based on EFI_SETUP_ENTER_GUID.
  @param[in] Context       - Pointer to the notification function's context.

**/
VOID
EFIAPI
CallbackGetLanPhyRevision(
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  UINT32          GbEBar = 0;
  EFI_STATUS      Status;
  UINTN           LanPciRegBase;
  UINT8           Data;
  BOOLEAN         Found = FALSE;
  UINT8           LanPhyRev = 0;
  UINTN           Index;
  BOOLEAN         LanPhyRevisionUpdated = FALSE;

  LanPciRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_LAN,
                    PCI_FUNCTION_NUMBER_PCH_LAN
                    );

  GbEBar = MmioRead32(LanPciRegBase + R_PCH_LAN_MBARA) & B_PCH_LAN_MBARA_BA;

  Data = MmioRead8 (LanPciRegBase + PCI_COMMAND_OFFSET);

  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);

  //
  // Command Register's Value is 0 in Fast Boot and Modifying here
  //
  if((Data & EFI_PCI_COMMAND_MEMORY_SPACE) == 0) {
    MmioWrite8 ((LanPciRegBase + PCI_COMMAND_OFFSET), 0x07);
    Found = TRUE;
  }

  //
  // Acquire Gbe Semaphore
  //
  Status = AcquireGbeSemaphore (GbEBar);
  if (EFI_ERROR(Status)) {
    return;
  }

  //
  // Get Phy Version
  //
  Status = GetPhyVersion (GbEBar, &LanPhyRev);

  //
  // If timeout is reached while getting phy version force MAC to SMB and try again
  //
  if (Status == EFI_TIMEOUT) {
    DEBUG((DEBUG_INFO, "GetPhyVersion timed out, forcing MAC to SMB.\n"));
    //
    // Change Extended Device Control Register BIT 11 to 1 which
    // forces the interface between the MAC and the Phy to be on SMBus.
    // Cleared on the assertion of PCI reset.
    //
    MmioOr32 (GbEBar + R_PCH_LAN_CSR_CTRL_EXT, B_PCH_LAN_CSR_CTRL_EXT_FORCE_SMB);
    GetPhyVersion (GbEBar, &LanPhyRev);
  }
  //
  // Free Gbe semaphore
  //
  ReleaseGbeSemaphore (GbEBar);

  //
  // LAN PHY Revision
  //
  if (LanPhyRev) {
    for (Index = 0; Index < (sizeof(LanPhyInfoTable) / sizeof(LAN_PHY_INFO)); Index++) {
      if (LanPhyRev == LanPhyInfoTable[Index].LanPhyRev) {
        InitString (
          gPsHiiHandle,
          STRING_TOKEN (STR_LAN_PHY_REV_VALUE),
          L"%02x %a",
          LanPhyRev,
          LanPhyInfoTable[Index].LanPhyString
          );
        LanPhyRevisionUpdated = TRUE;
        break;
      }
    }

    if(LanPhyRevisionUpdated == FALSE) {
      InitString (
        gPsHiiHandle,
        STRING_TOKEN (STR_LAN_PHY_REV_VALUE),
        L"%02x",
        LanPhyRev
        );
    }
  }

  if(Found == TRUE) {
    MmioWrite8 ((LanPciRegBase + PCI_COMMAND_OFFSET), Data);
  }

  return;
}

/**
  Retrieves Sensor Hub Firmware version

  @param[in] Event         - Event based on EFI_SETUP_ENTER_GUID.
  @param[in] Context       - Pointer to the notification function's context.

**/
VOID
EFIAPI
CallbackGetSensorHubFwVersion (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  EFI_STATUS              Status;
  CHAR16                  *fpGetVar = NULL;
  UINTN                   VariableSize;
#if 0
  EFI_HANDLE              *HandleArray = NULL;
  UINT32                  SensorHubFwVersion = 0;
  EFI_I2C_BUS_PROTOCOL    *I2cBusProtocol = NULL;
  EFI_I2C_REQUEST_PACKET  Request;
  UINTN                   HandleArrayCount = 0;
  UINTN                   Index;
  UINT8                   ReadBuffer [0x100];
  UINT8                   WriteBuffer [0x100];
  CHAR8                   mSensorHubString [] = \
                          {'I', '2', 'C', '0', '0', \
                           '\\','S', 'S', 'H', 'U', 'B', 0};
#endif

  if (PcdGetBool (PcdSensorHubSupport)) {
      VariableSize = 0;
      Status = gRT->GetVariable (
                      L"SensorSerialNumber",
                      &gSetupVariableGuid,
                      NULL,
                      &VariableSize,
                      fpGetVar
                      );

      if (Status == EFI_BUFFER_TOO_SMALL) {
        fpGetVar = AllocateZeroPool (VariableSize);
        Status = gRT->GetVariable (
                        L"SensorSerialNumber",
                        &gSetupVariableGuid,
                        NULL,
                        &VariableSize,
                        fpGetVar
                        );
      }

      if (EFI_ERROR(Status)) {
        InitString (
          gPsHiiHandle,
          STRING_TOKEN (STR_SENSOR_HUB_FW_REV_VALUE),
          L"%a",
          "N/A"
          );
      } else {
        InitString (
          gPsHiiHandle,
          STRING_TOKEN (STR_SENSOR_HUB_FW_REV_VALUE),
          L"%s",
          fpGetVar
          );
      }
      if (fpGetVar != NULL) {
        FreePool (fpGetVar);
      }

#if 0
    case BoardIdSkylakeSds:
    {
      ///@todo Check how SKL SDS will interact with Sensor Hub, HSB and WSB was I2C
      VariableSize = sizeof (SETUP_DATA);
      Status = gRT->GetVariable (
                      L"Setup",
                      &gSetupVariableGuid,
                      NULL,
                      &VariableSize,
                      &mSetupData
                      );
      ASSERT_EFI_ERROR (Status);

      //
      // Return N/A, if I2C0 Sensor Hub is not enabled
      //
      if (mSetupData.PchI2c0SensorDevice[0] == 0) {
        InitString (
          gPsHiiHandle,
          STRING_TOKEN (STR_SENSOR_HUB_FW_REV_VALUE),
          L"%a",
          "N/A"
          );
      } else {
        //
        // Find I2C0 controller and connect I2C stack
        //
        Status = gBS->LocateHandleBuffer (
                        ByProtocol,
                        &gMmioDeviceProtocolGuid,
                        NULL,
                        &HandleArrayCount,
                        &HandleArray
                        );
        for (Index = 0; Index < HandleArrayCount; Index++) {
          //
          // Start I2C stack to start I2c bus protocol
          //
          gBS->ConnectController (HandleArray[Index], NULL, NULL, FALSE);
        }
        if (HandleArray != NULL) {
          FreePool (HandleArray);
          HandleArray = NULL;
        }
        Status = gBS->LocateHandleBuffer (
                        ByProtocol,
                        &gEfiI2cIoProtocolGuid,
                        NULL,
                        &HandleArrayCount,
                        &HandleArray
                        );
        if (!EFI_ERROR (Status)) {
          for (Index = 0; HandleArrayCount > Index; Index++) {
            if (NULL != DlAcpiFindDeviceWithMatchingCid (
                          HandleArray [Index],
                          0,
                          (CONST CHAR8 *) mSensorHubString)) {
              //
              // The device was found
              //
              Status = gBS->OpenProtocol (
                              HandleArray [Index],
                              &gEfiI2cIoProtocolGuid,
                              (VOID **) &I2cBusProtocol,
                              NULL,
                              NULL,
                              EFI_OPEN_PROTOCOL_GET_PROTOCOL
                              );

              //
              // Send request to get sensor hub revision
              //
              ZeroMem (ReadBuffer, 0x100);
              ZeroMem (WriteBuffer, 0x100);
              Request.ReadBytes = SENSOR_HUB_FW_VERSION_LENGTH;
              Request.ReadBuffer = &ReadBuffer[0];
              Request.WriteBytes = 0x02;
              Request.WriteBuffer = &WriteBuffer[0];
              WriteBuffer[0] = SENSOR_HUB_FW_VERSION_OFFSET & 0xFF;
              WriteBuffer[1] = (SENSOR_HUB_FW_VERSION_OFFSET & 0xFF00) >> 8;
              Request.Timeout = 1000;

              Status = I2cBusProtocol->StartRequest (
                                         I2cBusProtocol,
                                         0,  //SlaveAddressIndex; NOT Slave Address
                                         NULL,
                                         &Request,
                                         NULL
                                         );
              if (!EFI_ERROR (Status)) {
                //
                // Retrieved Sensor Hub FW version
                //
                SensorHubFwVersion = (ReadBuffer[0]) | (ReadBuffer[1] << 8) | (ReadBuffer[2] << 16) | (ReadBuffer[3] << 24);
                InitString (
                  gPsHiiHandle,
                  STRING_TOKEN (STR_SENSOR_HUB_FW_REV_VALUE),
                  L"%ld",
                  SensorHubFwVersion
                  );
              }
            }
          }
        }
        if (HandleArray != NULL) {
          FreePool (HandleArray);
          HandleArray = NULL;
        }
      }
    }
    break;
#endif
  }

  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);

  return;
}

/**
  Determines if Full Sku ME is present.

  @retval TRUE     if fullSku ME is present

**/
BOOLEAN
IsFullSkuMePresent (
  VOID
  )
{
  EFI_STATUS        Status;
  HECI_PROTOCOL     *Heci;
  UINT32            MeMode;
  BOOLEAN           RetVal;

  RetVal = FALSE;
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (!EFI_ERROR (Status)) {
    Status = Heci->GetMeMode (&MeMode);
    //
    // ASSERT_EFI_ERROR(Status);
    //
    if (!EFI_ERROR (Status)) {
      if (MeMode == ME_MODE_NORMAL) {
        RetVal = TRUE;
      }
    }
  }

  return RetVal;
}

/**
  Determines if SusPwrDnAck needs to be set.
  For IbexPeak Platform with EC support, SusPwrDnAck should be to set.

  @retval TRUE        if SusPwrDnAck needs to be set

**/
BOOLEAN
CheckIfSetSusPwrDnAck (
  VOID
  )
{
  EFI_STATUS          Status;
  UINTN               VariableSize;
  SETUP_VOLATILE_DATA SetupVolatileData;
  BOOLEAN             SusPwrDnAck;

  SusPwrDnAck  = FALSE;

  VariableSize = sizeof (SETUP_VOLATILE_DATA);
  Status = gRT->GetVariable (
                  L"SetupVolatileData",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupVolatileData
                  );
  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR (Status)) {
    if ((SetupVolatileData.PlatformFlavor == FlavorMobile) ||
        (SetupVolatileData.PlatformFlavor == FlavorEmbedded)) {
      SusPwrDnAck = TRUE;
    }
  }

  return SusPwrDnAck;
}

/**
  Platform Reset function

  @param[in] PlatformResetType          Reset type aligned with EFI_RESET_TYPE

  @retval ME_SPEICAL_RESET_TYPES        ME reset type aligned with PCH_EXTENDED_RESET_TYPES

**/
ME_SPECIAL_RESET_TYPES
PlatformReset (
  IN  EFI_RESET_TYPE       PlatformResetType
  )
{
  EFI_STATUS               Status;
  UINTN                    VariableSize;

  //
  // Reset Flag
  //
  BOOLEAN                  GlobalResetRequired = FALSE;
  BOOLEAN                  PowerCycleResetRequired = FALSE;

#if 0  // AMI_OVERRIDE_START - For PlatformReset function design.
  SETUP_DATA               CurrentData;
  SETUP_DATA               SetupData;
  SA_SETUP                 SaCurrentData;
  SA_SETUP                 SaSetup;
  CPU_SETUP                CpuCurrentData;
  CPU_SETUP                CpuSetup;
  PCH_SETUP                PchCurrentData;
  PCH_SETUP                PchSetup;
#endif  // AMI_OVERRIDE_END - For PlatformReset function design.

  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
#if 0  // AMI_OVERRIDE_START - For PlatformReset function design.
                  &SetupData
#else
                  &gNewSetupData
#endif // AMI_OVERRIDE_END - For PlatformReset function design.
                  );
  //
  // ASSERT_EFI_ERROR(Status);
  //
  if (EFI_ERROR (Status)) {
    return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
  }

#if 0  // AMI_OVERRIDE_START - For PlatformReset function design.
  VariableSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SaSetup
                  );
  if (EFI_ERROR (Status)) {
    return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
  }

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &CpuSetup
                  );
  if (EFI_ERROR (Status)) {
    return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
  }

  VariableSize = sizeof (PCH_SETUP);
  Status = gRT->GetVariable (
                  L"PchSetup",
                  &gPchSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &PchSetup
                  );
  if (EFI_ERROR (Status)) {
    return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
  }


  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"ColdReset",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &CurrentData
                  );
  if (EFI_ERROR (Status)) {
    return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
  }

  VariableSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaColdReset",
                  &gSaSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SaCurrentData
                  );
  if (EFI_ERROR (Status)) {
    return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
  }

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuColdReset",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &CpuCurrentData
                  );
  if (EFI_ERROR (Status)) {
    return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
  }

  VariableSize = sizeof (PCH_SETUP);
  Status = gRT->GetVariable (
                  L"PchColdReset",
                  &gPchSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &PchCurrentData
                  );
  if (EFI_ERROR (Status)) {
    return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
  }
#endif // AMI_OVERRIDE_END - For PlatformReset function design.


  //
  // Change boot mode for critical setup questions.
  //
  if ((PchSetup.SataInterfaceMode != PchCurrentData.SataInterfaceMode)
// AMI_OVERRIDE_START - CSM_SUPPORT turn off will build errors.
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
      || 		  
#if 0  // AMI_OVERRIDE_START - For PlatformReset function design.
      (SetupData.PxeRom != CurrentData.PxeRom) ||
      (SetupData.EfiNetworkSupport != CurrentData.EfiNetworkSupport) ||
      ((SetupData.FastBoot == 0) && (CurrentData.FastBoot == 1))
#else
      (gOldSetupData.PxeOpRom != gNewSetupData.PxeOpRom)
#endif // AMI_OVERRIDE_END - For PlatformReset function design.
#endif
// AMI_OVERRIDE_END - CSM_SUPPORT turn off will build errors.
     ) {
    //
    // Delete BootState variable to force next boot is FullConfiguration boot
    // Boot option enumeration only happens in FullConfiguration boot
    //
    gRT->SetVariable (
           BOOT_STATE_VARIABLE_NAME,
           &gBootStateGuid,
           0,
           0,
           NULL
           );
  }

  if ((SaSetup.EnableVtd != SaCurrentData.EnableVtd) ||
      (PchSetup.PchHdAudio != PchCurrentData.PchHdAudio) ||
      (CpuSetup.Txt != CpuCurrentData.Txt) ||
      (SaSetup.GTTSize != SaCurrentData.GTTSize) ||
      (SaSetup.IgdDvmt50PreAlloc != SaCurrentData.IgdDvmt50PreAlloc) ||
      (SaSetup.MaxTolud != SaCurrentData.MaxTolud) ||
      (SaSetup.InternalGraphics != SaCurrentData.InternalGraphics) ||
      (SaSetup.PrimaryDisplay != SaCurrentData.PrimaryDisplay) ||
      (SaSetup.SpdProfileSelected != SaCurrentData.SpdProfileSelected) ||
      (SaSetup.Peg0MaxLinkSpeed != SaCurrentData.Peg0MaxLinkSpeed) ||
      (SaSetup.Peg1MaxLinkSpeed != SaCurrentData.Peg1MaxLinkSpeed) ||
      (SaSetup.Peg2MaxLinkSpeed != SaCurrentData.Peg2MaxLinkSpeed) ||
      (CpuSetup.BiosGuard != CpuCurrentData.BiosGuard) ||
      (CpuSetup.FlashWearOutProtection != CpuCurrentData.FlashWearOutProtection)
      ) {
    PowerCycleResetRequired = TRUE;
  }

// AMI_OVERRIDE_START - For PlatformReset function design.
  MeResetHook ();
// AMI_OVERRIDE_END - For PlatformReset function design.
  
  if (mMeReset
    ) {
    GlobalResetRequired = TRUE;
  }

  if (gIccReset) {
    PowerCycleResetRequired = TRUE;
  }

  if((CpuSetup.ConfigTdpLevel != CpuCurrentData.ConfigTdpLevel) ||
     (CpuSetup.ConfigTdpLock != CpuCurrentData.ConfigTdpLock) ||
     (CpuSetup.Custom1TurboActivationRatio != CpuCurrentData.Custom1TurboActivationRatio) ||
     (CpuSetup.Custom2TurboActivationRatio != CpuCurrentData.Custom2TurboActivationRatio) ||
     (CpuSetup.Custom3TurboActivationRatio != CpuCurrentData.Custom3TurboActivationRatio)
    ) {
    PowerCycleResetRequired = TRUE;
  }

  if (CpuSetup.EnableSgx != CpuCurrentData.EnableSgx) {
    PowerCycleResetRequired = TRUE;
  }

  if (IsFullSkuMePresent () && GlobalResetRequired) {
    HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
    gBS->Stall (1000000);
    //
    // System goes to PCH Reset after one second if ME reset failed
    //
    DEBUG ((DEBUG_INFO, "OemReset: ME reset failed, goes to PCH Reset\n"));
  }

// AMI_OVERRIDE_START - For PlatformReset function design.
  DEBUG ((DEBUG_INFO, "PlatformReset: GlobalResetRequired = %x, PowerCycleResetRequired = %x.\n", GlobalResetRequired, PowerCycleResetRequired));  
// AMI_OVERRIDE_END - For PlatformReset function design.

  //
  // Doesn't need to do platform special reset
  //
  if (GlobalResetRequired) {
    if (CheckIfSetSusPwrDnAck ()) {
      return GlobalResetWithEcReq;
    }

    return GlobalResetReq;
  } else if (PowerCycleResetRequired) {
    return PowerCycleResetReq;
  }

  return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
}

VOID
InitBoardStrings (
  EFI_HII_HANDLE  HiiHandle,
  UINT16          Class
  )
{
  return;
}
