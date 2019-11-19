/** @file
  Source code file for the Platform Init DXE module

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2017 Intel Corporation.

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
#include "PlatformInitDxe.h"
#include <Protocol/BlockIo.h>
#include <Protocol/GopPolicy.h>
#include <Protocol/SaPolicy.h>
#include <Library/DebugLib.h>
#ifndef MINTREE_FLAG
#include <PlatformBoardId.h>
#include <CmosMap.h>
#include <Library/CmosAccessLib.h>
#include <LegacyUsb/Protocol/UsbController.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS                  mAddress;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                                 mSize;

#define EFI_MAX_ADDRESS       0xFFFFFFFF


EFI_STATUS
EFIAPI
RegisterDisplayDimmPopulationErrMsg (
  VOID
  );

VOID
EFIAPI
DisplayDimmPopulationErrMsg (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );


/**
  Update RMRR Base and Limit Address for USB.

**/
VOID
UpdateRmrrUsbAddress (
  VOID
  )
{
  EFI_STATUS              Status;
  SA_POLICY_PROTOCOL      *SaPolicy;
  MISC_DXE_CONFIG         *MiscDxeConfig;
  UINTN                   VariableSize;
  SA_SETUP                SaSetup;

  VariableSize = sizeof(SA_SETUP);
  Status = gRT->GetVariable(
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SaSetup
                  );
  ASSERT_EFI_ERROR(Status);

  Status = gBS->LocateProtocol (&gSaPolicyProtocolGuid, NULL, (VOID **) &SaPolicy);
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = GetConfigBlock ((VOID *)SaPolicy, &gMiscDxeConfigGuid, (VOID *)&MiscDxeConfig);
  ASSERT_EFI_ERROR (Status);

  if (SaSetup.EnableVtd == 1) {
    mSize = EFI_SIZE_TO_PAGES(SA_VTD_RMRR_USB_LENGTH);
    mAddress = EFI_MAX_ADDRESS;

    Status = (gBS->AllocatePages) (
                     AllocateMaxAddress,
                     EfiReservedMemoryType,
                     mSize,
                     &mAddress
                    );
    ASSERT_EFI_ERROR (Status);

    MiscDxeConfig->RmrrUsbBaseAddress[0] = mAddress;
    MiscDxeConfig->RmrrUsbBaseAddress[1] = mAddress + SA_VTD_RMRR_USB_LENGTH - 1;
  }
}
#endif // MINTREE_FLAG

/**
  Update SaPolicyGopVbtPointer and Size

**/
EFI_STATUS
UpdateSaPolicyGopVbtPointer (
  VOID
  )
{
  EFI_STATUS                       Status;
  SA_POLICY_PROTOCOL               *SaPolicy;
  GOP_POLICY_PROTOCOL              *GopPolicy;
  EFI_PHYSICAL_ADDRESS             VbtAddress;
  UINT32                           Size;
  GRAPHICS_DXE_CONFIG              *GraphicsDxeConfig;

  Status = gBS->LocateProtocol (&gSaPolicyProtocolGuid, NULL, (VOID **) &SaPolicy);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetConfigBlock ((VOID *)SaPolicy, &gGraphicsDxeConfigGuid, (VOID *)&GraphicsDxeConfig);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "Locate GopPolicy and GetVbtData\n"));
  ///
  /// Locate the GOP Policy Protocol.
  ///
  GopPolicy = NULL;
  Status = gBS->LocateProtocol (
                  &gGopPolicyProtocolGuid,
                  NULL,
                  (VOID **) &GopPolicy
                  );
  ASSERT (GopPolicy != NULL);
  if (EFI_ERROR (Status) || (GopPolicy == NULL)) {
    return Status;
  }

  ///
  /// Get VBT data
  ///
  VbtAddress      = 0;
  Size            = 0;
  DEBUG ((DEBUG_INFO, "GetVbtData\n"));
  Status          = GopPolicy->GetVbtData (&VbtAddress, &Size);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  GraphicsDxeConfig->VbtAddress = VbtAddress;
  GraphicsDxeConfig->Size = Size;

  return Status;
}


#ifndef MINTREE_FLAG
/**
  Updates DIMM slots status for Desktop,server and workstation boards

**/
VOID
UpdateDimmPopulation(
  PLATFORM_INFO                *PlatformInfo
  )
{

  if (PcdGetBool (PcdDisplayDimmPopulationError) && PcdGetBool (PcdDimmPopulationError)) {
    RegisterDisplayDimmPopulationErrMsg ();
  }
}

/**
  Register DisplayDIMMPopulationErrMsg Notification function on SimpleIn protocol Installation.

  @param[in] SystemTable    Pointer to the System Table

  @retval  EFI_SUCCESS      return EFI_STATUS as success
**/
EFI_STATUS
EFIAPI
RegisterDisplayDimmPopulationErrMsg (
  VOID
  )
{
  EFI_EVENT DimmPopultionErrMsgEvent;
  VOID      *Registration;

  DimmPopultionErrMsgEvent  = NULL;

  //
  // Create event for SimpleInProtocol Callback notification
  //
  gBS->CreateEvent (
        EVT_NOTIFY_SIGNAL,
        TPL_CALLBACK,
        DisplayDimmPopulationErrMsg,
        NULL,
        &DimmPopultionErrMsgEvent
        );
  gBS->RegisterProtocolNotify (
        &gBdsAllDriversConnectedProtocolGuid,
        DimmPopultionErrMsgEvent,
        &Registration
        );

  return EFI_SUCCESS;
}

/**
  Display DIMM population error message, while Wrong DIMM Population is found.

  @param[in] Event    A pointer to the Event that triggered the callback
  @param[in] Context  A pointer to private data registered with the callback function.

  @retval  EFI_SUCCESS     If Successfully exectuted else returns the error status
**/
VOID
EFIAPI
DisplayDimmPopulationErrMsg (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                Status;
  EFI_INPUT_KEY             Key;

  DEBUG ((DEBUG_WARN, "\n DIMMPopulation Error msg\n"));

  if (gST->ConOut != NULL && gST->ConIn != NULL) {
    if (Event != NULL) {
      gBS->CloseEvent (Event);
    }
    //
    // Display the Error Message
    //
    gST->ConOut->OutputString (gST->ConOut, L"");

    gST->ConOut->OutputString (gST->ConOut, L"Error...! Invalid DIMM Population.");

    gST->ConOut->OutputString (gST->ConOut, L"Slot 0 can't be populated when Slot 1 is not populated on the same channel");

    gST->ConOut->OutputString (gST->ConOut, L"Press <Y> to Continue. <N> to Shutdown");

    //
    // Wait for User Input
    //
    do {
      Status = gBS->CheckEvent (gST->ConIn->WaitForKey);
      if (!EFI_ERROR (Status)) {
        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
        if (Key.UnicodeChar == L'Y' || Key.UnicodeChar == L'y') {
          DEBUG ((DEBUG_WARN, " Boot Continue\n"));
          break;
        } else if (Key.UnicodeChar == L'N' || Key.UnicodeChar == L'n') {
          DEBUG ((DEBUG_WARN, " System shutdown\n"));
          gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        }
      }
    } while (1);
  }
}
#endif // MINTREE_FLAG

#ifdef CSM_FLAG
/**
  Updates CMOS registers related to display

**/
VOID
UpdateCmosDisplayRegisters(
  PLATFORM_INFO                *PlatformInfo
  )
{
  EFI_STATUS            Status;
  UINT8                 LcdPanelType;
  UINT8                 LcdPanelScaling;
  UINT8                 VideoBootType;
  UINT8                 DisplayPipeB;
  UINT8                 BacklightType;
  UINT8                 ActiveLFP;
  UINT8                 LfpColorDepth;
  UINT8                 PrimaryDisplay;
  UINTN                 SaSetupSize;
  UINT8                 VbiosBrightness;
  UINT8                 DDISelection;
  SA_SETUP              SaSetup;

  SaSetupSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  NULL,
                  &SaSetupSize,
                  &SaSetup
                  );

  if (!EFI_ERROR(Status)) {
    LcdPanelType    = SaSetup.LcdPanelType;
    LcdPanelScaling = SaSetup.LcdPanelScaling;
    VideoBootType   = SaSetup.IgdBootType;
    DisplayPipeB    = SaSetup.DisplayPipeB;
    BacklightType   = SaSetup.IgdLcdBlc;
    ActiveLFP       = SaSetup.ActiveLFP;
    LfpColorDepth   = SaSetup.LfpColorDepth;
    PrimaryDisplay  = SaSetup.PrimaryDisplay;
    VbiosBrightness = SaSetup.VbiosBrightness;
  } else {
    LcdPanelType    = 0;
    LcdPanelScaling = 0;
    VideoBootType   = 0;
    DisplayPipeB    = 0;
    BacklightType   = 0;
    ActiveLFP       = 0;
    LfpColorDepth   = 0;
    PrimaryDisplay  = 0;
    VbiosBrightness = 0;
  }

  DDISelection = PcdGet8 (PcdDDISelection);
  CmosWrite8 (CMOS_LCDPANELTYPE_REG, LcdPanelType);
  CmosWrite8 (CMOS_LCDPANELSCALING_REG, LcdPanelScaling);
  CmosWrite8 (CMOS_IGDBOOTTYPE_REG, VideoBootType);
  CmosWrite8 (CMOS_IGD_DISPLAY_PIPE_B_REG, DisplayPipeB);
  CmosWrite8 (CMOS_BACKLIGHT_REG, BacklightType);
  CmosWrite8 (CMOS_EDP_ACTIVE_LFP_CONFIG_REG, ActiveLFP);
  CmosWrite8 (CMOS_LFP_PANEL_COLOR_DEPTH_REG, LfpColorDepth);
  CmosWrite8 (CMOS_PRIMARY_DISPLAY_REG, PrimaryDisplay);
  CmosWrite8 (CMOS_DISP_DDI_REG, DDISelection);
  CmosWrite8 (CMOS_BRIGHTNESS_REG, VbiosBrightness);
}
#endif // CSM_FLAG

/**
  System Agent platform Initialization

**/
VOID
SaPlatformInitDxe (
  VOID
  )
{
#ifndef MINTREE_FLAG
  //
  // Update RMRR Base and Limit Address for USB
  //
  UpdateRmrrUsbAddress();
#endif // MINTREE_FLAG

  //
  // Update GOP VBT Address and Size from FV
  //
  UpdateSaPolicyGopVbtPointer();

}
