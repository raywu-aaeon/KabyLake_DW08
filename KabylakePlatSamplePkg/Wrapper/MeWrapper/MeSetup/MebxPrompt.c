//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************
/** @file MebxPrompt.c
    AMT TSE Functions.

**/
#include <SetupVariable.h>
#include <variable.h>
#include <AmiDxeLib.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/AmtPolicy.h>
#include <MeBiosExtensionSetup.h>
#include <MeBiosPayloadData.h>
#include <Protocol/HeciProtocol.h>
#include <Library/ConfigBlockLib.h>
#include <Library/DxeAmtLib.h>

#if defined iAMT_SUPPORT && iAMT_SUPPORT == 1
#include <Protocol/AmtWrapperProtocol.h>
#endif
extern EFI_STATUS PostManagerDisplayPostMessage( CHAR16 *message );

extern EFI_BOOT_SERVICES      *gBS;
extern EFI_SYSTEM_TABLE       *gST;
extern EFI_RUNTIME_SERVICES   *gRT;
ME_BIOS_EXTENSION_SETUP       MeBiosExtensionSetupData;

EFI_HANDLE                    MebxKeyHandle[15][2];
EFI_HANDLE                    CIRAKeyHandle[15][4];
AMT_POLICY_PROTOCOL           *mDxePlatformAmtPolicy = NULL;

#define EFI_EVENT_ME_PLATFORM_READY_TO_BOOT \
  { \
    0x3fdf171, 0x1d67, 0x4ace, 0xa9, 0x4, 0x3e, 0x36, 0xd3, 0x38, 0xfa, 0x74 \
  }
EFI_GUID      gEfiSimpleTextInExProtocolGuid = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;

EFI_GUID      gMePlatformReadyToBootGuid = EFI_EVENT_ME_PLATFORM_READY_TO_BOOT;
/**
    Set to GraphicMode

    @param Event 
    @param ParentImageHandle 

    @retval VOID

**/
VOID
EFIAPI
SetConsoleControlModeBack(
  EFI_EVENT   Event,
  VOID        *ParentImageHandle
)
{
  EFI_STATUS                        Status;
  EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;

  Status = gBS->LocateProtocol( &gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);

 //Set back to Graphics
  Status = ConsoleControl->SetMode( ConsoleControl, EfiConsoleControlScreenGraphics );

  gBS->CloseEvent(Event);
  return ;
}

/**

    @param Key 

    @retval EFI_STATUS Status

**/
EFI_STATUS EFIAPI CIRACheckForKey
(
  IN EFI_KEY_DATA *Key
)
{
#if CIRA_SCANCODE || CIRA_UNICODE
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;
  EFI_STATUS                        Status;
  UINTN                             HandleCount, i;
  EFI_HANDLE                        *HandleBuffer;  
  AMT_DXE_CONFIG                    *AmtDxeConfig;  
  
  if(mDxePlatformAmtPolicy == NULL)
  {    
    Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, (VOID**) &mDxePlatformAmtPolicy);
    if (EFI_ERROR(Status)) return Status;
  }
  Status = GetConfigBlock ((VOID *) mDxePlatformAmtPolicy, &gAmtDxeConfigGuid, (VOID *) &AmtDxeConfig);
  ASSERT_EFI_ERROR (Status);
    
  Status = gBS->HandleProtocol(gST->ConsoleInHandle, 
                              &gEfiSimpleTextInExProtocolGuid, 
                              (VOID**)&SimpleTextInEX);

  if (EFI_ERROR(Status)) return Status;
  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiSimpleTextInExProtocolGuid,
            NULL, 
            &HandleCount,
            &HandleBuffer
            );
  for(i = 0 ; i < HandleCount ; i++)
  {
      Status = gBS->HandleProtocol (
                      HandleBuffer[i],
                      &gEfiSimpleTextInExProtocolGuid,
                      (VOID**) &SimpleTextInEX
                      );
      if(EFI_ERROR(Status))
          continue;
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2][0]);  
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2][1]);  
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2][2]);  
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2][3]);  
      if((CIRA_UNICODE>0x40 && CIRA_UNICODE<0x5b) || (CIRA_UNICODE>0x60 && CIRA_UNICODE<0x7b)){
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1][0]);
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1][1]);
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1][2]);
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1][3]);
      }
  }
  
  AmtDxeConfig->CiraRequest = 1;
  PostManagerDisplayPostMessage(L"Requesting CIRA ......");
#endif
  return EFI_SUCCESS;
}
/**

    @param Key 

    @retval EFI_STATUS Status

**/
EFI_STATUS EFIAPI MebxCheckForKey
(
  IN EFI_KEY_DATA *Key
)
{
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;
  EFI_STATUS                        Status;
  EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;
  EFI_EVENT                         SetConsoleControlModeBackEvent;
  EFI_CONSOLE_CONTROL_SCREEN_MODE   ScreenMode;
  UINTN                             HandleCount, i;
  EFI_HANDLE                        *HandleBuffer; 
  AMT_DXE_CONFIG                    *AmtDxeConfig;    

  if(mDxePlatformAmtPolicy == NULL)
  {    
    Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, (VOID **)&mDxePlatformAmtPolicy);
    if (EFI_ERROR(Status)) return Status;
  }
  Status = GetConfigBlock ((VOID *) mDxePlatformAmtPolicy, &gAmtDxeConfigGuid, (VOID *) &AmtDxeConfig);
  ASSERT_EFI_ERROR (Status);

  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiSimpleTextInExProtocolGuid,
            NULL, 
            &HandleCount,
            &HandleBuffer
            );
  for(i = 0 ; i < HandleCount ; i++)
  {
//      if(HandleBuffer[i] == gST->ConsoleInHandle)
//          continue;
      Status = gBS->HandleProtocol (
                      HandleBuffer[i],
                      &gEfiSimpleTextInExProtocolGuid,
                      (VOID **)&SimpleTextInEX
                      );
      if(EFI_ERROR(Status))
          continue;
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[i*2][0]);  
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[i*2][1]);  
      if((MEBX_UNICODE>0x40 && MEBX_UNICODE<0x5b) || (MEBX_UNICODE>0x60 && MEBX_UNICODE<0x7b)){
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*2) + 1][0]);
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*2) + 1][1]);
      }
  }
  AmtDxeConfig->AmtbxHotkeyPressed = 1;
  PostManagerDisplayPostMessage(L"Entering MEBX setup menu ......");
  // Fix Mebx Setup screen not full screen.
  Status = gBS->LocateProtocol( &gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);

  ConsoleControl->GetMode(ConsoleControl, &ScreenMode, NULL, NULL);
  if (ScreenMode == EfiConsoleControlScreenGraphics) 
  {
    ConsoleControl->SetMode( ConsoleControl, EfiConsoleControlScreenText );  
	  
    Status = gBS->CreateEventEx (
                    EFI_EVENT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    SetConsoleControlModeBack,
                    NULL,
                    &gMePlatformReadyToBootGuid,
                    &SetConsoleControlModeBackEvent
                    );
  }
  return EFI_SUCCESS;
}
/**
    This function is a hook called when TSE determines
    that SETUP utility has to be displayed. This function
    is available as ELINK. In the generic implementation
    setup password is prompted in this function.

    @param VOID

    @retval VOID

**/
BOOLEAN MebxPromptConInAvailabilityHook  (VOID)
{

    EFI_STATUS        Status;
    UINTN             VarSize;
    ME_SETUP          MeSetup;    
#if MEBX_UNICODE>0x40 && MEBX_UNICODE<0x5b == 1
                      // ScanCode, UnicodeChar, KeyShiftState, KeyToggleState
    EFI_KEY_DATA      Key[] = {{MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState_R, MEBX_KeyToggleState},
                               {MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState_L, MEBX_KeyToggleState},
                               {MEBX_SCANCODE, MEBX_UNICODE+0x20, MEBX_KeyShiftState_R, MEBX_KeyToggleState},
                               {MEBX_SCANCODE, MEBX_UNICODE+0x20, MEBX_KeyShiftState_L, MEBX_KeyToggleState}};

#else
#if MEBX_UNICODE>0x60 && MEBX_UNICODE<0x7b == 1
    EFI_KEY_DATA      Key[] = {{MEBX_SCANCODE, MEBX_UNICODE-0x20, MEBX_KeyShiftState_R, MEBX_KeyToggleState},
                               {MEBX_SCANCODE, MEBX_UNICODE-0x20, MEBX_KeyShiftState_L, MEBX_KeyToggleState},
                               {MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState_R, MEBX_KeyToggleState}
                               {MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState_L, MEBX_KeyToggleState}};
#else
    EFI_KEY_DATA      Key[] = {{MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState_R, MEBX_KeyToggleState},
                               {MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState_L, MEBX_KeyToggleState}};
#endif
#endif
    HECI_PROTOCOL     *Heci;
    UINT32            MeMode = ME_MODE_FAILED;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;
    UINTN                       HandleCount, i;
    EFI_HANDLE                  *HandleBuffer; 
    AMT_DXE_CONFIG              *AmtDxeConfig;      
#if defined iAMT_SUPPORT && iAMT_SUPPORT == 1
    //EFI_GUID      gEfiAmtWrapperGuid = EFI_AMT_WRAPPER_PROTOCOL_GUID;
    AMT_WRAPPER_PROTOCOL              *AmtWrapperProtocol;

    Status = gBS->LocateProtocol (&gEfiAmtWrapperProtocolGuid, NULL, (VOID**) &AmtWrapperProtocol);
    if (EFI_ERROR(Status)) return FALSE;
    if ((AmtWrapperProtocol->ActiveManagementIsKvmEnabled()) ||
        (AmtWrapperProtocol->ActiveManagementIsSolEnabled()))
        return FALSE;
#endif

    VarSize = sizeof(ME_SETUP);
    Status = gRT->GetVariable (
                    L"MeSetup",
                    &gMeSetupVariableGuid,
                    NULL,
                    &VarSize,
                    &MeSetup
                    );
    if (EFI_ERROR(Status)) return FALSE;

    if(MeSetup.MeImageType != 4)
      return FALSE;
    Status = pBS->LocateProtocol (
                    &gHeciProtocolGuid,
                    NULL,
                    (VOID **)&Heci
                    );

    if (EFI_ERROR(Status)) return EFI_SUCCESS; 

    Status = Heci->GetMeMode (&MeMode);

    // If not normal mode
    if(MeMode != 0)
      return FALSE;

    if(mDxePlatformAmtPolicy == NULL)
    {    
        Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, (VOID **)&mDxePlatformAmtPolicy);

        if (EFI_ERROR(Status)) 
            return FALSE;
    }   
    Status = GetConfigBlock ((VOID *) mDxePlatformAmtPolicy, &gAmtDxeConfigGuid, (VOID *) &AmtDxeConfig);
    ASSERT_EFI_ERROR (Status);
    
    if(AmtDxeConfig->AmtbxHotkeyPressed == 1)
    {
        EFI_EVENT        SetConsoleControlModeBackEvent;
        EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;
        EFI_CONSOLE_CONTROL_SCREEN_MODE   ScreenMode;

        // Fix Mebx Setup screen not full screen.
        Status = gBS->LocateProtocol( 
                 &gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);

        ConsoleControl->GetMode(ConsoleControl, &ScreenMode, NULL, NULL);
        if (ScreenMode == EfiConsoleControlScreenGraphics) {

          Status = gBS->CreateEventEx (
                          EFI_EVENT_NOTIFY_SIGNAL,
                          TPL_CALLBACK,
                          SetConsoleControlModeBack,
                          NULL,
                          &gMePlatformReadyToBootGuid,
                          &SetConsoleControlModeBackEvent
                          );
        }
        return FALSE;
    }

    HandleCount = 0;
    Status = gBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiSimpleTextInExProtocolGuid,
              NULL, 
              &HandleCount,
              &HandleBuffer
              );

    for(i = 0 ; i < HandleCount ; i++)
    {
//        if(HandleBuffer[i] == gST->ConsoleInHandle)
//            continue;
        
        Status = gBS->HandleProtocol (
                        HandleBuffer[i],
                        &gEfiSimpleTextInExProtocolGuid,
                        (VOID **)&SimpleTextInEX
                        );
        if(EFI_ERROR(Status))
            continue;
        Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[0], &MebxCheckForKey, &MebxKeyHandle[i*2][0]);
        Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[1], &MebxCheckForKey, &MebxKeyHandle[i*2][1]);
        if((MEBX_UNICODE>0x40 && MEBX_UNICODE<0x5b) || (MEBX_UNICODE>0x60 && MEBX_UNICODE<0x7b)){
            Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[2], &MebxCheckForKey, &MebxKeyHandle[i*2 +1][0]);
            Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[3], &MebxCheckForKey, &MebxKeyHandle[i*2 +1][1]);
        }
    }
    
    PostManagerDisplayPostMessage(L"Press <CTRL + P> to Enter MEBX setup menu ");
    
    return FALSE;
}
/**
    This function is a hook called when TSE determines
    that SETUP utility has to be displayed. This function
    is available as ELINK. In the generic implementation
    setup password is prompted in this function.

    @param VOID

    @retval VOID

**/
BOOLEAN RemoteAssistConInAvailabilityHook  (VOID)
{
#if CIRA_SUPPORT
    EFI_STATUS    Status;
    UINTN         VariableSize = sizeof (ME_BIOS_EXTENSION_SETUP);
   // EFI_GUID      gEfiAmtWrapperGuid = EFI_AMT_WRAPPER_PROTOCOL_GUID;
    AMT_WRAPPER_PROTOCOL              *AmtWrapperProtocol;
    UINTN                             HandleCount, i;
    EFI_HANDLE                        *HandleBuffer;  
    AMT_DXE_CONFIG                    *AmtDxeConfig; 
    
    Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, (VOID**) &mDxePlatformAmtPolicy);
    if (EFI_ERROR(Status)) return FALSE;

    Status = GetConfigBlock ((VOID *) mDxePlatformAmtPolicy, &gAmtDxeConfigGuid, (VOID *) &AmtDxeConfig);
    ASSERT_EFI_ERROR (Status);    

    Status = gBS->LocateProtocol (&gEfiAmtWrapperProtocolGuid, NULL, (VOID**) &AmtWrapperProtocol);
    if (EFI_ERROR(Status)) return FALSE;

    Status = gRT->GetVariable (
             ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
             &gMeBiosExtensionSetupGuid,
             NULL,
             &VariableSize,
             &MeBiosExtensionSetupData);

    if(EFI_ERROR(Status))
        return FALSE;

    if((MeBiosExtensionSetupData.RemoteAssistanceTriggerAvailablilty) &&
       IsAmtBiosSupportEnabled() &&
       (!AmtWrapperProtocol->ActiveManagementIsKvmEnabled()) &&
       (!AmtWrapperProtocol->ActiveManagementIsSolEnabled()))
    {
#if CIRA_UNICODE>0x40 && CIRA_UNICODE<0x5b == 1
                      // ScanCode, UnicodeChar, KeyShiftState, KeyToggleState
    EFI_KEY_DATA      Key[] = {{CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_1, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_2, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_3, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_4, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE+0x20, CIRA_KeyShiftState_1, CIRA_KeyToggleState},                              
                               {CIRA_SCANCODE, CIRA_UNICODE+0x20, CIRA_KeyShiftState_2, CIRA_KeyToggleState},                               
                               {CIRA_SCANCODE, CIRA_UNICODE+0x20, CIRA_KeyShiftState_3, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE+0x20, CIRA_KeyShiftState_4, CIRA_KeyToggleState}};

#else
#if CIRA_UNICODE>0x60 && CIRA_UNICODE<0x7b == 1
                      // ScanCode, UnicodeChar, KeyShiftState, KeyToggleState
    EFI_KEY_DATA      Key[] = {{CIRA_SCANCODE, CIRA_UNICODE-0x20, CIRA_KeyShiftState_1, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE-0x20, CIRA_KeyShiftState_2, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE-0x20, CIRA_KeyShiftState_3, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE-0x20, CIRA_KeyShiftState_4, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_1, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_2, CIRA_KeyToggleState},                              
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_3, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_4, CIRA_KeyToggleState}};
#else
    EFI_KEY_DATA      Key[] = {{CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_1, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_2, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_3, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState_4, CIRA_KeyToggleState}};
#endif
#endif

        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;

        HandleCount = 0;
        Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleTextInExProtocolGuid,
                  NULL, 
                  &HandleCount,
                  &HandleBuffer
                  );

        for(i = 0 ; i < HandleCount ; i++)
        {
            Status = gBS->HandleProtocol (
                            HandleBuffer[i],
                            &gEfiSimpleTextInExProtocolGuid,
                            (VOID**) &SimpleTextInEX
                            );
            if(EFI_ERROR(Status))
                continue;
            Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[0], &CIRACheckForKey, &CIRAKeyHandle[i*2][0]);
            Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[1], &CIRACheckForKey, &CIRAKeyHandle[i*2][1]);
            Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[2], &CIRACheckForKey, &CIRAKeyHandle[i*2][2]);
            Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[3], &CIRACheckForKey, &CIRAKeyHandle[i*2][3]);
            if((CIRA_UNICODE>0x40 && CIRA_UNICODE<0x5b) || (CIRA_UNICODE>0x60 && CIRA_UNICODE<0x7b)){
                Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[4], &CIRACheckForKey, &CIRAKeyHandle[i*2 +1][0]);
                Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[5], &CIRACheckForKey, &CIRAKeyHandle[i*2 +1][1]);
                Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[6], &CIRACheckForKey, &CIRAKeyHandle[i*2 +1][2]);
                Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[7], &CIRACheckForKey, &CIRAKeyHandle[i*2 +1][3]);
            }
        }
        PostManagerDisplayPostMessage(L"Press <CTRL + ALT + F1> Remote Assistance");
    }
#endif //CIRA_SUPPORT
    return FALSE;
}
/**
    This function is a hook called when TSE determines
    that it has to load the boot options in the boot
    order. This function is available as ELINK.

    @param VOID

    @retval VOID

**/
VOID MEProcessEnterSetup(VOID)
{
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;
  EFI_STATUS                        Status;
  UINTN                             HandleCount, i;
  EFI_HANDLE                        *HandleBuffer;  

  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiSimpleTextInExProtocolGuid,
            NULL, 
            &HandleCount,
            &HandleBuffer
            );

  for(i = 0 ; i < HandleCount ; i++)
  {
//      if(HandleBuffer[i] == gST->ConsoleInHandle)
//          continue;
      
      Status = gBS->HandleProtocol (
                      HandleBuffer[i],
                      &gEfiSimpleTextInExProtocolGuid,
                      (VOID **)&SimpleTextInEX
                      );
      if(EFI_ERROR(Status))
          continue;
#if CIRA_SUPPORT
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2][0]);  
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2][1]);
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2][2]);
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2][3]);
      if((MEBX_UNICODE>0x40 && MEBX_UNICODE<0x5b) || (MEBX_UNICODE>0x60 && MEBX_UNICODE<0x7b)){
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1][0]);
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1][1]);
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1][2]);
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1][3]);
      }
#endif
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[i*2][0]);  
      Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[i*2][1]);  
      if((MEBX_UNICODE>0x40 && MEBX_UNICODE<0x5b) || (MEBX_UNICODE>0x60 && MEBX_UNICODE<0x7b)){
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*2) + 1][0]);
        Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*2) + 1][1]);
        
      }
  }
}
