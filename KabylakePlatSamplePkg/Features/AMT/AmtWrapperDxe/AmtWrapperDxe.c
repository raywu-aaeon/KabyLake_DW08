/**@file
AMT Wrapper Implementation.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2016 Intel Corporation.

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
#include "AmtWrapperDxe.h"
//AMI_OVERRIDE_START - For AMT security Boot, Set a empty File Authentication to skip Secure check.
#include "Token.h"
#include <SetupVariable.h>
#include <Include/Protocol/Security2.h>
#include "Amt.h"
//AMI_OVERRIDE_END - For AMT security Boot, Set a empty File Authentication to skip Secure check.

//AMI_OVERRIDE_START - Install dummy Diskinfo for USB device to pass AMT_013 test. 
#include <Protocol/DiskInfo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/AmtReadyToBoot.h>
//AMI_OVERRIDE_END - Install dummy Diskinfo for USB device to pass AMT_013 test.


//AMI_OVERRIDE_START - Enable/Disable SOL COM when AMT SOL is enable.
#if defined(Terminal_SUPPORT) && (Terminal_SUPPORT == 1)
#include "Include/TerminalSetupVar.h"
#endif
//AMI_OVERRIDE_END - Enable/Disable SOL COM when AMT SOL is enable.


//AMI_OVERRIDE_START - Install dummy Diskinfo for USB device to pass AMT_013 test.
EFI_STATUS EFIAPI InstallUSBDummyDiskInfo (VOID);
AMT_READY_TO_BOOT_PROTOCOL      mUSBDummyDiskInfo = {
  AMT_READY_TO_BOOT_PROTOCOL_REVISION_1,
  InstallUSBDummyDiskInfo
};
//AMI_OVERRIDE_END - Install dummy Diskinfo for USB device to pass AMT_013 test.


//AMI_OVERRIDE_START - Define Setup Guid
static EFI_GUID   gSetupGuid = SETUP_GUID;
//AMI_OVERRIDE_END - Define Setup Guid

typedef struct {
  EFI_INPUT_KEY        Key;
  UINT32               KeyShiftState;
} ME_HOT_KEY_TABLE;

#define NUMBER_OF_HOTKEY_CTRL_P      6
#define NUMBER_OF_HOTKEY_ALL         10

GLOBAL_REMOVE_IF_UNREFERENCED AMT_WRAPPER_PROTOCOL mAmtWrapperInstance = {
  AmtWrapperInit,
  AmtWrapperGet,
  AmtWrapperSet,
  AmtWrapperBootOptionExist,
  AmtWrapperEnableStorageRedir,
  AmtWrapperEnterSetup,
  AmtWrapperPauseBoot,
  AmtWrapperEnableKvm,
  AmtWrapperEnableSecureErase,
  AmtWrapperConsoleLocked,
  AmtWrapperEnableSol,
  AmtWrapperBdsBootViaAsf,
  GetSetupPrompt,
  DetermineSetupHotKey,
  GetSupportedHotkeys
};

extern ACTIVE_MANAGEMENT_PROTOCOL       *mActiveManagement;
extern ASF_BOOT_OPTIONS                 *mAsfBootOptions;
extern EFI_GUID                         gSetupEnterGuid;
extern AMT_DXE_CONFIG                   *mAmtDxeConfig;

GLOBAL_REMOVE_IF_UNREFERENCED UINTN                                    mEnterRemoteAssistance = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                                    mEnterIntelMeSetup = 0;
GLOBAL_REMOVE_IF_UNREFERENCED ME_BIOS_EXTENSION_SETUP                  mMeBiosExtensionSetup;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                                  mMeBiosExtensionSetupReady = FALSE;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                    mFwImageType;

GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_PCI_SERIAL_OVER_LAN_DEVICE_PATH mSerialOverLanDevicePath = {
  gPciRootBridge,
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_PCI_DP,
      {
        (UINT8)(sizeof(PCI_DEVICE_PATH)),
        (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)
      }
    },
    SOL_FUNCTION_NUMBER,
    ME_DEVICE_NUMBER
  },
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_UART_DP,
      {
        (UINT8)(sizeof(UART_DEVICE_PATH)),
        (UINT8)((sizeof(UART_DEVICE_PATH)) >> 8)
      }
    },
    0,
    115200,
    8,
    1,
    1
  },
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_VENDOR_DP,
      {
        (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    DEVICE_PATH_MESSAGING_PC_ANSI
  },
  gEndEntire
};

//
// Driver entry point
//

//
// Function implementations
//

#define EFI_CONTROL_PRESSED         (EFI_RIGHT_CONTROL_PRESSED + EFI_LEFT_CONTROL_PRESSED)
#define EFI_ALT_PRESSED             (EFI_RIGHT_ALT_PRESSED + EFI_LEFT_ALT_PRESSED)

BOOLEAN
IsCtrlKey (
  IN UINT32 KeyShiftState
  )
{
  if ((KeyShiftState & EFI_SHIFT_STATE_VALID) && (KeyShiftState & EFI_CONTROL_PRESSED)) {
    return TRUE;
  } else {
    return FALSE;
  }
}


BOOLEAN
IsAltKey (
  IN UINT32 KeyShiftState
  )
{
  if ((KeyShiftState & EFI_SHIFT_STATE_VALID) && (KeyShiftState & EFI_ALT_PRESSED)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

BOOLEAN
IsPKey (
  IN CHAR16 UnicodeChar
  )
{
  if (UnicodeChar == L'p' || UnicodeChar == L'P') {
    return TRUE;
  } else {
    return FALSE;
  }
}

VOID
LocateSetupAndPolicyData (
  VOID
  )
{
  EFI_STATUS                Status;
  UINTN                     VariableSize;

  Status = AmtDxeConfigInit ();
  if (EFI_ERROR (Status)) {
    return;
  }

  if (mMeBiosExtensionSetupReady == FALSE) {
    VariableSize = sizeof (mMeBiosExtensionSetup);
    Status = gRT->GetVariable (
                    ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
                    &gMeBiosExtensionSetupGuid,
                    NULL,
                    &VariableSize,
                    &mMeBiosExtensionSetup
                    );
    if (!EFI_ERROR(Status))
      mMeBiosExtensionSetupReady = TRUE;
  }
}

VOID
EnterIntelMeSetup (
  VOID
  )
{
  mAmtDxeConfig->AmtbxHotkeyPressed = 1;
  mAmtDxeConfig->CiraRequest = 0;
  DEBUG ((DEBUG_INFO, "Entering Intel ME Setup\n"));
}

VOID
EnterRemoteAssistance (
  VOID
  )
{
  mAmtDxeConfig->AmtbxHotkeyPressed = 1;
  mAmtDxeConfig->CiraRequest = 1;
  DEBUG ((DEBUG_INFO, "Entering Remote Assistance\n"));
}

BOOLEAN
DisplayRemoteAssistancePrompt (
  VOID
  )
{
  BOOLEAN                         st;

  st = FALSE;

  DEBUG((DEBUG_INFO, "<DisplayRemoteAssistancePrompt>" ));

  if ((mAmtDxeConfig != NULL) && (mMeBiosExtensionSetupReady == TRUE)) {
    DEBUG((DEBUG_INFO, "<AmtbxSelectionScreen=%d>", mAmtDxeConfig->AmtbxSelectionScreen));
    DEBUG((DEBUG_INFO, "<RemoteAssistanceTriggerAvailablilty=%d>", mMeBiosExtensionSetup.RemoteAssistanceTriggerAvailablilty));
    if ((mAmtDxeConfig->AmtbxSelectionScreen == 0) &&
        (mMeBiosExtensionSetup.RemoteAssistanceTriggerAvailablilty == 1)) {
      st = TRUE;
    } else {
      st = FALSE;
    }
  }

  DEBUG((DEBUG_INFO, "<st=%d>", st));

  return st;
}

BOOLEAN
IsBiosHotkeyEnabled (
  VOID
  )
{
  BOOLEAN        st = FALSE;
  EFI_STATUS     Status;
  HECI_PROTOCOL  *Heci;
  UINT32         MeMode;

  Heci = NULL;
  MeMode = ME_MODE_TEMP_DISABLED;

  Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, (VOID **) &Heci);
  if (!EFI_ERROR(Status)) {
    Heci->GetMeMode(&MeMode);
  } else {
    return FALSE;
  }

  if ((mFwImageType != IntelMeConsumerFw) && (MeMode == ME_MODE_NORMAL)) {
    st = TRUE;
  }

  return st;
}


EFI_STATUS
EFIAPI
GetSetupPrompt (
  IN OUT  UINTN     *PromptIndex,
  OUT     CHAR16    **String
  )
{
  EFI_STATUS     Status;
  EFI_STRING     SetupPrompt[] = {
    L"Press [CTRL+P] to enter the MEBx Setup Menu.",
    L"Press [CTRL+ALT+F1] to enter MEBx Remote Assistance."
  };
  EFI_STRING     SetupPromptSelectionMenu[] = {
    L"Press [CTRL+P] to enter the MEBx Setup Menu."
  };


  if (PromptIndex == NULL || String == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;
  *String = NULL;
  InitAmtWrapperLib();
  LocateSetupAndPolicyData ();

  if(!ActiveManagementIsSolEnabled() && !ActiveManagementIsKvmEnabled ())
    if (IsBiosHotkeyEnabled ()) {
      if (DisplayRemoteAssistancePrompt ()) {
        if (*PromptIndex < sizeof (SetupPrompt) / sizeof (EFI_STRING)) {
          *String = SetupPrompt[*PromptIndex];
          Status = EFI_SUCCESS;
          (*PromptIndex)++;
        }
      } else {
        if (*PromptIndex < sizeof (SetupPromptSelectionMenu) / sizeof (EFI_STRING)) {
          *String = SetupPromptSelectionMenu[*PromptIndex];
          Status = EFI_SUCCESS;
          (*PromptIndex)++;
        }
      }
    }
  return Status;
}

// Ctrl+ALT+F1  : Scan    = 0x0B, Shift = 0x3C
// Ctrl+P       : Unicode = 0x10/L'p', Shift = 0x0C

EFI_STATUS
EFIAPI
DetermineSetupHotKey(
  IN EFI_KEY_DATA                       *KeyData
  )
{
  EFI_STATUS Status = EFI_NOT_FOUND;

  //
  // React to hotkey once per boot only
  //
  if (mEnterIntelMeSetup || mEnterRemoteAssistance) {
    return Status;
  }

  InitAmtWrapperLib();
  LocateSetupAndPolicyData ();
  if(!ActiveManagementIsSolEnabled () && !ActiveManagementIsKvmEnabled ())
    if (IsBiosHotkeyEnabled()) {
      if ((KeyData->Key.UnicodeChar == 0x10 || IsPKey (KeyData->Key.UnicodeChar)) && IsCtrlKey (KeyData->KeyState.KeyShiftState)) {
        EnterIntelMeSetup();
        mEnterIntelMeSetup = 1;
        Status = EFI_SUCCESS;
      } else if ((DisplayRemoteAssistancePrompt() == TRUE) &&
                 (KeyData->Key.ScanCode == SCAN_F1) && IsCtrlKey(KeyData->KeyState.KeyShiftState) && IsAltKey(KeyData->KeyState.KeyShiftState)) {
        EnterRemoteAssistance();
        mEnterRemoteAssistance = 1;
        Status = EFI_SUCCESS;
      }

      DEBUG ((DEBUG_INFO, "\n"));
      DEBUG ((DEBUG_INFO, "<Scan=0x%X>", KeyData->Key.ScanCode));
      DEBUG ((DEBUG_INFO, "<Unicode=0x%X>", KeyData->Key.UnicodeChar));
      DEBUG ((DEBUG_INFO, "<ShiftState=0x%X>", KeyData->KeyState.KeyShiftState));
      DEBUG ((DEBUG_INFO, "<ToggleState=0x%X>", KeyData->KeyState.KeyToggleState));
      DEBUG ((DEBUG_INFO, "<mEnterIntelMeSetup=0x%X>", mEnterIntelMeSetup));
      DEBUG ((DEBUG_INFO, "<mEnterRemoteAssistance=0x%X>", mEnterRemoteAssistance));
    }
  return Status;
}


EFI_STATUS
EFIAPI
GetSupportedHotkeys (
  OUT  UINTN             *KeyCnt,
  OUT  EFI_KEY_DATA      **KeyData
  )
{
  EFI_STATUS          Status;
  UINTN               Index;
  ME_HOT_KEY_TABLE    MeHotKeyTables[] = {
  // Ctrl + P
  { {CHAR_NULL, 0x10}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED},
  { {CHAR_NULL, 0x10}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED},
  { {CHAR_NULL, L'p'}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED},
  { {CHAR_NULL, L'p'}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED},
  { {CHAR_NULL, L'P'}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED},
  { {CHAR_NULL, L'P'}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED},

  // Ctrl + Alt + F1
  { {SCAN_F1, CHAR_NULL}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED | EFI_RIGHT_ALT_PRESSED},
  { {SCAN_F1, CHAR_NULL}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED | EFI_LEFT_ALT_PRESSED},
  { {SCAN_F1, CHAR_NULL}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED | EFI_RIGHT_ALT_PRESSED},
  { {SCAN_F1, CHAR_NULL}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED | EFI_LEFT_ALT_PRESSED}
};

  Status = EFI_NOT_FOUND;
  LocateSetupAndPolicyData ();
  *KeyCnt = 0;
  *KeyData = NULL;

  if (IsBiosHotkeyEnabled ()) {
    if (DisplayRemoteAssistancePrompt ()) {
      //
      // CTRL + P and CTRL + ALT + F1 hotkey
      //
      *KeyCnt = NUMBER_OF_HOTKEY_ALL;
    } else {
      //
      // CTRL + P hotkey
      //
      *KeyCnt = NUMBER_OF_HOTKEY_CTRL_P;
    }
    *KeyData = AllocateZeroPool (sizeof (EFI_KEY_DATA) * (*KeyCnt));
    if (*KeyData == NULL){
      return EFI_OUT_OF_RESOURCES;
    }
    for (Index = 0; Index < *KeyCnt; Index++) {
      (*KeyData)[Index].Key.ScanCode = MeHotKeyTables[Index].Key.ScanCode;
      (*KeyData)[Index].Key.UnicodeChar = MeHotKeyTables[Index].Key.UnicodeChar;
      (*KeyData)[Index].KeyState.KeyShiftState = MeHotKeyTables[Index].KeyShiftState;
    }

    Status = EFI_SUCCESS;
  }

  return Status;
}

VOID
EFIAPI
AsfWatchDogStopOnSetupEntry (
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
  AmtWrapperSet (SET_WDT_STOP, 0);
}

//AMI_OVERRIDE_START - Enable/Disable SOL COM when AMT SOL is enable.
#if defined(Terminal_SUPPORT) && (Terminal_SUPPORT == 1)
/**
    Enable/Disable SOL COM.

    @param Enabled 

    @retval VOID


**/
VOID
SetSOLCOMEnable(
    IN BOOLEAN         Enabled
)
{
    UINT32      ComPort;
    UINTN       SetupDataSize;
    SETUP_DATA  gSetupData;
    EFI_STATUS  Status;
    UINT32      Attributes = 0;
#if (TOTAL_PCI_SERIAL_PORTS > 0)
    UINT32      i = 0;
    EFI_GUID    gTerminalVarGuid   = TERMINAL_VAR_GUID;
    UINTN  		gTerminalSerialSupportVarSize = sizeof(TERMINAL_SERIAL_SUPPORT);
    TERMINAL_SERIAL_SUPPORT gTerminalSerialSupport;
    UINT32      TerminalSerialSupportVarAttributes=0;
    UINT32 gTotalSioSerialPorts = TOTAL_SIO_SERIAL_PORTS;

    Status = gRT->GetVariable (L"TerminalSerialVar",
                              &gTerminalVarGuid, 
                              &TerminalSerialSupportVarAttributes,
                              &gTerminalSerialSupportVarSize,
                              &gTerminalSerialSupport);

    if(EFI_ERROR(Status)) {
        ComPort = gTotalSioSerialPorts; //Since AMT module execute earlier. We can't get the var in the first remote boot. 
    }
    else{
        ComPort = gTotalSioSerialPorts; 

        for (i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
            if ((gTerminalSerialSupport.PciSerialPortsLocationVar.Device[i] == SOL_DEVICE_NUMBER) && 
               (gTerminalSerialSupport.PciSerialPortsLocationVar.Function[i] == SOL_FUNCTION_NUMBER)) 
            {
                    ComPort = gTotalSioSerialPorts+i; 
                    break;            
            }
        }
    }
    
#else 
    return;
#endif
    SetupDataSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable ( L"Setup", \
                                &gSetupGuid, \
                                &Attributes, \
                                &SetupDataSize, \
                                &gSetupData );

    if(Enabled)
     {
         gSetupData.ConsoleRedirectionEnable[ComPort] = 1;
         gSetupData.ComPortforLegacy = ComPort;
     }
     else
     {
         gSetupData.ConsoleRedirectionEnable[ComPort] = 0;
     }
	
    Status = gRT->SetVariable ( L"Setup", \
                                &gSetupGuid, \
                                Attributes, \
                                SetupDataSize, \
                                &gSetupData );
    
}
#endif
//AMI_OVERRIDE_END - Enable/Disable SOL COM when AMT SOL is enable.

//AMI_OVERRIDE_START - For AMT security Boot, Set a empty File Authentication to skip Secure check.
EFI_STATUS EFIAPI EmptyFileAuthentication (
    IN CONST EFI_SECURITY2_ARCH_PROTOCOL *This,
    IN CONST EFI_DEVICE_PATH_PROTOCOL *File,
    IN VOID *FileBuffer,
    IN UINTN FileSize,
    IN BOOLEAN BootPolicy
)
{
    if (File==NULL) return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}    
//AMI_OVERRIDE_END - For AMT security Boot, Set a empty File Authentication to skip Secure check.

//AMI_OVERRIDE_START - Install dummy Diskinfo for USB device to pass AMT_013 test.
EFI_STATUS
EFIAPI
DiskInfoInquiry (
        IN EFI_DISK_INFO_PROTOCOL   *This,
        IN OUT VOID                 *InquiryData,
        IN OUT UINT32               *InquiryDataSize
    	)
{
	    return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
DiskInfoIdentify (
        EFI_DISK_INFO_PROTOCOL      *This,
        IN OUT VOID                 *IdentifyData,
        IN OUT UINT32               *IdentifyDataSize
)
{
	    return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
DiskInfoSenseData (
        EFI_DISK_INFO_PROTOCOL      *This,
        VOID                        *SenseData,
        UINT32                      *SenseDataSize,
        UINT8                       *SenseDataNumber
)
{
	    return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
DiskInfoWhichIDE (
        IN EFI_DISK_INFO_PROTOCOL   *This,
        OUT UINT32                  *IdeChannel,
        OUT UINT32                  *IdeDevice
)
{
	    return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
InstallUSBDummyDiskInfo (
  VOID
  )
{
	  EFI_STATUS             Status;
	  UINTN                     Index;
	  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
	  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
	  UINTN                     HandleCount;
	  EFI_HANDLE                *HandleBuffer;
	  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
	  EFI_DEVICE_PATH_PROTOCOL  *TmpDevicePathNode; 
	  UINT8			    ParentPortNumber;
	  UINT8			    InterfaceNumber;  


	      Status = gBS->LocateHandleBuffer (
	                       ByProtocol,
	                       &gEfiBlockIoProtocolGuid,
	                       NULL,
	                       &HandleCount,
	                       &HandleBuffer
	                       );

	       if (EFI_ERROR (Status) || HandleCount == 0) {
	         return Status;
	       }

	       for (Index = 0; Index < HandleCount; Index++) {
	         Status = gBS->HandleProtocol (
	                         HandleBuffer[Index],
	                         &gEfiBlockIoProtocolGuid,
	                         (VOID **) &BlkIo
	                         );
	         ASSERT_EFI_ERROR (Status);

	         Status = gBS->HandleProtocol (
	                         HandleBuffer[Index],
	                         &gEfiDevicePathProtocolGuid,
	                         (VOID *) &DevicePath
	                         );
	         ASSERT_EFI_ERROR (Status);      
	         
	          TmpDevicePathNode = DevicePath;
	          DiskInfo = NULL;
	          while (!IsDevicePathEnd (TmpDevicePathNode)) {   
	            if(TmpDevicePathNode->Type == MESSAGING_DEVICE_PATH) {
	                 if(TmpDevicePathNode->SubType == MSG_USB_DP) { 
	                         	
	             ///USBR ParentPortNumber is 0xB(PCH-LP) and 0xF(PCH-H)
	             ///     InterfaceNumber is 0 and 1
	             ///If we find this device, skip it 
	                                 
	                      ParentPortNumber = ((USB_DEVICE_PATH*)TmpDevicePathNode)->ParentPortNumber;
	                      InterfaceNumber = ((USB_DEVICE_PATH*)TmpDevicePathNode)->InterfaceNumber;

	                       if(((ParentPortNumber == 0xB) || (ParentPortNumber == 0xF)) && 
	                         ((InterfaceNumber == 0) || (InterfaceNumber == 1)))  
	                         {
	                	       Status = gBS->HandleProtocol (
	                	                       HandleBuffer[Index],
	                	                       &gEfiDiskInfoProtocolGuid,
	                	                       (VOID **) &DiskInfo
	                	                       );
	                	       if (EFI_ERROR (Status)) {
	                		       
	                		       Status = gBS->AllocatePool (EfiBootServicesData,
	                		                   sizeof(EFI_DISK_INFO_PROTOCOL),
	                		                   (VOID**)&DiskInfo);
	                		       if (EFI_ERROR(Status)){
	                		           return Status;
	                		       }
	                		       
	                		       DiskInfo->Identify = DiskInfoIdentify;
	                		       DiskInfo->Inquiry = DiskInfoInquiry;
	                		       DiskInfo->SenseData = DiskInfoSenseData;
	                		       DiskInfo->WhichIde = DiskInfoWhichIDE;
	                		       DiskInfo->Interface = gEfiDiskInfoUsbInterfaceGuid;
	                		       
	                		       Status = gBS->InstallProtocolInterface(
	                		   		    &HandleBuffer[Index],
	                		   		    &gEfiDiskInfoProtocolGuid,
	                		   		    EFI_NATIVE_INTERFACE,
	                		   		    DiskInfo); 
		                	       
	                	       }
	                         }
	                    }
	             }   
	                     TmpDevicePathNode = NextDevicePathNode (TmpDevicePathNode);
	          }    
	                 
	       }         
	      	
	         return EFI_SUCCESS;	
}
//AMI_OVERRIDE_END - Install dummy Diskinfo for USB device to pass AMT_013 test.

/**
  Entry point for the Active Management Driver.

  @param[in] ImageHandle        Image handle of this driver.
  @param[in] SystemTable        Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
AmtWrapperDxeEntryPoint (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS             Status;
  EFI_EVENT              ReadyToBootEvent;
  ME_BIOS_PAYLOAD_HOB    *MbpHob;
  EFI_EVENT              PlatformSetupEvent;
  VOID                   *Registration;

  mAsfBootOptions   = NULL;
  mActiveManagement = NULL;
  MbpHob            = NULL;

  InitAmtWrapperLib();

  mFwImageType = IntelMeConsumerFw;

  //
  // Get Mbp Protocol
  //
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob != NULL) {
    mFwImageType = (UINT8)MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;
  }

  //
  // Install the EFI_ACTIVE_MANAGEMENT_PROTOCOL interface
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiAmtWrapperProtocolGuid,
                  &mAmtWrapperInstance,
                  NULL
                  );
//AMI_OVERRIDE_START - Enable/Disable SOL COM when AMT SOL is enable.
#if defined (Terminal_SUPPORT) && (Terminal_SUPPORT == 1)  
    // Enable/Disable Sol redirection
      SetSOLCOMEnable(AmtWrapperEnableSol());
#endif
//AMI_OVERRIDE_END - Enable/Disable SOL COM when AMT SOL is enable.

  //
  // Register Ready to Boot Event for AMT Wrapper
  //
  Status = EfiCreateEventReadyToBootEx (
              TPL_CALLBACK,
              AmtWrapperReadyToBoot,
              (VOID *)&ImageHandle,
              &ReadyToBootEvent
              );

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  AsfWatchDogStopOnSetupEntry,
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
//AMI_OVERRIDE_START - Install dummy Diskinfo for USB device to pass AMT_013 test.
  {
          EFI_HANDLE                      *Handles, Handle;
          UINTN                           Count, i, *Buffer;

      if(ActiveManagementEnableStorageRedir())
      {  	  
          Status = gBS->LocateHandleBuffer (ByProtocol, \
                          &gAmtReadyToBootProtocolGuid,  NULL, &Count, &Handles);
          if (EFI_ERROR(Status)) Count = 0;
          
          // Allocate enough buffer to save current AmtReadyToBootProtocol.    
      	Status = gBS->AllocatePool(EfiBootServicesData, \
                                      (Count + 1) * sizeof(UINTN), (VOID*)&Buffer);
          
          // In order to install the mUSBDummyDiskInfo prior to 
          // AmtReadyToBootProtocol of Mebx driver, below to save then uninstall
          // all the AmtReadyToBootProtocols
          if ((!EFI_ERROR(Status)) && (Count > 0)) { 
              for (i = 0; i < Count; i++) {
                  Status = gBS->HandleProtocol (Handles[i], 
                              &gAmtReadyToBootProtocolGuid, (VOID**)&Buffer[i + 1]);
                  if (!EFI_ERROR(Status)) {
                      Status = gBS->UninstallProtocolInterface (Handles[i],
                              &gAmtReadyToBootProtocolGuid, (VOID*)Buffer[i + 1]);
                  }
              }
          }

          // Force the mUSBDummyDiskInfo at 1st AmtReadyToBoot Protocol then 
          // reinstall all the saved AmtReadyToBootProtocols.
          Buffer[0] = (UINTN)&mUSBDummyDiskInfo;
          for (i = 0; i < (Count + 1); i++) {
              Handle = NULL;
              Status = gBS->InstallProtocolInterface (\
                          &Handle,  &gAmtReadyToBootProtocolGuid, 
                          EFI_NATIVE_INTERFACE, (VOID*)Buffer[i]);
          }
      }
  }
//AMI_OVERRIDE_END - Install dummy Diskinfo for USB device to pass AMT_013 test.

  return Status;
}

BOOLEAN
EFIAPI
AmtWrapperBootOptionExist (
  VOID
  )
{
  return (BOOLEAN) (mAsfBootOptions->SubCommand == ASF_BOOT_OPTIONS_PRESENT);
}

BOOLEAN
EFIAPI
AmtWrapperGet (
  IN UINTN     Index,
  IN OUT VOID  *pVar
  )
{
  InitAmtWrapperLib();

  if (Index == GET_SOL_DP) {
    *((PLATFORM_PCI_SERIAL_OVER_LAN_DEVICE_PATH **)pVar) = &mSerialOverLanDevicePath;
    return TRUE;
  } else if (Index == GET_BOOT_OPTION) {
    *((UINT16 *)pVar) = mAsfBootOptions->BootOptions;
    return TRUE;
  } else if (Index == GET_IS_SOL) {
    *((UINTN *)pVar) = IsSOL(*((EFI_HANDLE *)pVar));
    return TRUE;
  } else {
    return FALSE;
  }
}

UINTN IsSOL (
  IN EFI_HANDLE Controller
  )
{
  EFI_STATUS                  Status;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  UINTN                       Segment=0;
  UINTN                       Bus=0;
  UINTN                       Device=0;
  UINTN                       Function=0;
  UINTN                       flag = FALSE;

  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (!EFI_ERROR(Status)) {
      Status = PciIo->GetLocation(
                        PciIo,
                        &Segment,
                        &Bus,
                        &Device,
                        &Function
                        );
      if (!EFI_ERROR(Status)) {
          if (Device == SOL_DEVICE_NUMBER && Function == SOL_FUNCTION_NUMBER) {
              flag = TRUE;
          }
      }
  }
  return flag;
}

BOOLEAN
EFIAPI
AmtWrapperSet (
  IN UINTN Index,
  IN OUT VOID *pVar
  )
{
  InitAmtWrapperLib();

  if (Index == SET_BIOS_WDT_START) {
    AsfStartWatchDog(ASF_START_BIOS_WDT);
    return TRUE;
  }
  else if (Index == SET_OS_WDT_START) {
    AsfStartWatchDog(ASF_START_OS_WDT);
    return TRUE;
  }
  else if (Index == SET_WDT_STOP) {
    AsfStopWatchDog();
    return TRUE;
  }
  else if (Index == SET_OS_SOL) {
    DEBUG((DEBUG_INFO, "<SET_OS_SOL>"));
    return TRUE;
  } else {
    return FALSE;
  }
}

EFI_STATUS
EFIAPI
AmtWrapperInit (
  IN EFI_HANDLE              ImageHandle,
  IN OUT EFI_SYSTEM_TABLE    *SystemTable
  )
{
  InitAmtWrapperLib();
  return EFI_SUCCESS;
}


VOID
InitAmtWrapperLib (
  VOID
  )
{
  if (mActiveManagement == NULL) {
    AmtLibInit();
  }

  if (mAsfBootOptions == NULL) {
    BdsAsfInitialization();
  }
}

BOOLEAN
EFIAPI
AmtWrapperConsoleLocked (
  VOID
  )
{
  InitAmtWrapperLib();
  return ActiveManagementIsConsoleLocked ();
}

BOOLEAN
EFIAPI
AmtWrapperEnableSecureErase (
  VOID
  )
{
  InitAmtWrapperLib();
  return ActiveManagementIsSecureEraseEnabled ();
}

BOOLEAN
EFIAPI
AmtWrapperEnableSol (
  VOID
  )
{
  InitAmtWrapperLib ();
  return ActiveManagementIsSolEnabled ();
}

BOOLEAN
EFIAPI
AmtWrapperEnableStorageRedir (
  VOID
  )
{
// AMI_OVERRIDE_START - For AMT security Boot, Set a empty File Authentication to skip Secure check.
  BOOLEAN						st;
  EFI_STATUS 					Status; 
  EFI_SECURITY2_ARCH_PROTOCOL 	*Security2; 
// AMI_OVERRIDE_END - For AMT security Boot, Set a empty File Authentication to skip Secure check.

  InitAmtWrapperLib ();
// AMI_OVERRIDE_START - For AMT security Boot, Set a empty File Authentication to skip Secure check.    
//  return ActiveManagementIsStorageRedirectionEnabled ();    
  st = ActiveManagementIsStorageRedirectionEnabled();
    
  if(!st) return st;
    
  // USBR enabled and Enforce Secure Boot disabled
  if((mAsfBootOptions->SpecialCommandParam & ENFORCE_SECURE_BOOT) != ENFORCE_SECURE_BOOT) {        
    Status = gBS->LocateProtocol(&gEfiSecurity2ArchProtocolGuid, NULL, (VOID**) &Security2);
    if(EFI_ERROR(Status))
      return st;

    // Set a empty File Authentication to skip Secure check.
    // This will not been restore, because USBRR boot fail system HALT
    Security2->FileAuthentication = EmptyFileAuthentication;
  }    
  return st;
// AMI_OVERRIDE_END - For AMT security Boot, Set a empty File Authentication to skip Secure check.    
}

BOOLEAN
EFIAPI
AmtWrapperPauseBoot (
  VOID
  )
{
  InitAmtWrapperLib ();
  return ActiveManagementIsPauseBootEnabled ();
}

BOOLEAN
EFIAPI
AmtWrapperEnterSetup (
  VOID
  )
{
  InitAmtWrapperLib ();
  return ActiveManagementIsEnterSetupEnabled ();
}

EFI_STATUS
EFIAPI
AmtWrapperBdsBootViaAsf (
  VOID
  )
{
  EFI_STATUS Status = EFI_NOT_FOUND;

  InitAmtWrapperLib ();
  if (mActiveManagement != NULL && mAsfBootOptions != NULL) {
    Status = BdsBootViaAsf ();
  }
  return Status;
}

VOID
EFIAPI
AmtWrapperReadyToBoot (
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
  )
{
  gBS->CloseEvent (Event);
}

BOOLEAN
EFIAPI
AmtWrapperEnableKvm (
  VOID
  )
{
  InitAmtWrapperLib ();
  return ActiveManagementIsKvmEnabled ();
}
