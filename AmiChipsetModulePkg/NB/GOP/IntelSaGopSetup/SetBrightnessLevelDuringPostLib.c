//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: 
//
// $Revision: 
//
// $Date: 
//*************************************************************************
/** @file SetBrightnessLevelDuringPostLib.c
    GOP Setup Rountines

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------
#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <PciBus.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/IntelSaGopProtocol.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

#pragma pack(1)

  typedef struct _NB_GOP_PLATFORM_DATA
  {
    UINT8   BrightnessAvailable;
    UINT8   GopBistAvailable;
  } NB_GOP_PLATFORM_DATA;

#pragma pack()

static EFI_GUID gSetupGuid                          = SETUP_GUID;
static EFI_GUID gGopDisplayBrightnessProtocolGuid   = GOP_DISPLAY_BRIGHTNESS_PROTOCOL_GUID;
extern EFI_BOOT_SERVICES       *gBS;

/**
    SetBrightnessLevelDuringPost Callback func
    
    @param None.

    @retval None.

    @note  None.

**/
VOID 
SetBrightnessLevelCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  GOP_DISPLAY_BRIGHTNESS_PROTOCOL    *GopDisplayBrightnessProtocol = NULL;
  NB_GOP_PLATFORM_DATA        NBGopPlatformData = {0};
  SETUP_DATA                  *SetupData = NULL;
  UINTN                       SetupSize = 0;
  UINT32                      MaxBrightness = 255;
  UINT32                      VarAttr;
  UINT32                      CurrentBrightness = 255;

  Status = pRS->GetVariable (
                  L"Setup",
                  &gSetupGuid,
                  &VarAttr,
                  &SetupSize,
                  NULL
                  );  
  
  if (Status == EFI_BUFFER_TOO_SMALL) {
    SetupData = AllocateZeroPool (SetupSize);
    ASSERT (SetupData != NULL);
    if (SetupData == NULL) {
      return;
    }

    Status = pRS->GetVariable (
                    L"Setup",
                    &gSetupGuid,
                    &VarAttr,
                    &SetupSize,
                    SetupData
                    );

    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "Error!!! GetVariable status: %r\n", Status));   
      return;
    }
  } else {
    DEBUG((DEBUG_ERROR, "Fatal Error!!! GetVariable status: %r\n", Status));
    return;
  }
    
  Status = pBS->LocateProtocol (&gGopDisplayBrightnessProtocolGuid, NULL, (VOID **)&GopDisplayBrightnessProtocol);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Fatal Error!!! LocateProtocol gGopDisplayBrightnessProtocolGuid status: %r\n", Status));      
    return;
  }
        
  if (!EFI_ERROR(Status)) {
    if (GopDisplayBrightnessProtocol->Revision >= GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01) {
      NBGopPlatformData.BrightnessAvailable = 1;  
      Status = pRS->SetVariable (
                      L"NBGopPlatformData",
                      &gSetupGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      sizeof (NB_GOP_PLATFORM_DATA),
                      &NBGopPlatformData
                      );  
                
      if (EFI_ERROR(Status)) { 
        DEBUG((DEBUG_ERROR, "Error!!! Set NBGopPlatformData Status: %r\n", Status));                                          
        return;
      }         
                
      Status = GopDisplayBrightnessProtocol->GetMaxBrightnessLevel(GopDisplayBrightnessProtocol, &MaxBrightness);
      if (!EFI_ERROR(Status)) {
        DEBUG((DEBUG_INFO, "Get GetMaxBrightnessLevel Success!\n"));                  
        if (SetupData->GopBrightness > MaxBrightness) {
          SetupData->GopBrightness = MaxBrightness;
          DEBUG((DEBUG_INFO, "SetupData->GopBrightness: %d\n", SetupData->GopBrightness));
//#if defined(EFI_SPECIFICATION_VERSION) && EFI_SPECIFICATION_VERSION > 0x20000 // [ EIP427935 ]
          SetupSize = sizeof (SETUP_DATA);
          pRS->SetVariable(L"Setup", &gSetupGuid, VarAttr, SetupSize, SetupData);
//#endif
        } 
                  
        Status = GopDisplayBrightnessProtocol->GetCurrentBrightnessLevel(GopDisplayBrightnessProtocol, &CurrentBrightness);
        if (!EFI_ERROR(Status)) {
          DEBUG((DEBUG_INFO, "Get GetCurrentBrightnessLevel Success!\n"));                  
          if(CurrentBrightness != SetupData->GopBrightness){
            DEBUG((DEBUG_INFO, "SetupData->GopBrightness: %d\n", SetupData->GopBrightness));
            Status = GopDisplayBrightnessProtocol->SetBrightnessLevel(GopDisplayBrightnessProtocol, SetupData->GopBrightness);
            DEBUG((DEBUG_INFO, "Get SetBrightnessLevel Status: %r\n", Status));                                                
          }
        }
      }
    } // GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01
  }  
}

/**
    SetBrightnessLevelDuringPost func
    
    @param None.

    @retval None.

    @note  None.

**/
VOID
SetBrightnessLevelDuringPost (
  )
{
  EFI_EVENT                     Event;
  EFI_STATUS                    Status;
  VOID                          *Registration;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  SetBrightnessLevelCallback,
                  NULL,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Error!!! SetBrightnessLevelCallback Status: %r\n", Status));                                          
    return;
  }

  Status = gBS->RegisterProtocolNotify (
                  &gGopDisplayBrightnessProtocolGuid,
                  Event,
                  &Registration
                  );
  
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Error!!! RegisterProtocolNotify Status: %r\n", Status));                                          
    return;
  }  
  
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
