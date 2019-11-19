//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
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
/** @file IntelSaGopSetup.c
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
#include "IntelSaGopSetup.h"

static EFI_GUID gSetupGuid                          = SETUP_GUID;
static EFI_GUID gGopDisplayBrightnessProtocolGuid   = GOP_DISPLAY_BRIGHTNESS_PROTOCOL_GUID;
static EFI_GUID gGopDisplayBistProtocolGuid         = GOP_DISPLAY_BIST_PROTOCOL_GUID;

EFI_STATUS AmiGopDeviceCheck (
  IN  EFI_HANDLE           ControllerHandle,
  IN  EFI_HANDLE           DriverBindingHandle,
  IN  EFI_PCI_IO_PROTOCOL  *PciIo
);

EFI_STATUS GetIntelSaGopSetupDriverBindingHandle (
  IN  EFI_HANDLE  ControllerHandle,
  OUT EFI_HANDLE  *DriverBindingHandle
);

EFI_STATUS GetChildDeviceHandlesControledByDriver (
  IN  EFI_HANDLE  DriverBindingHandle,
  IN  EFI_HANDLE  ControllerHandle,
  OUT UINTN       *ChildControllerHandleCount,
  OUT EFI_HANDLE  **ChildControllerHandleBuffer
);

/**
    None.

    @param None.

    @retval None.

    @note  None.

**/
EFI_STATUS
GetIntelSaGopSetupDriverBindingHandle (
  IN  EFI_HANDLE  ControllerHandle,
  OUT EFI_HANDLE  *DriverBindingHandle )
{
    EFI_STATUS                          Status;
    UINTN                               HandleCount;
    EFI_HANDLE                          *HandleBuffer;
    UINTN                               HandleIndex;
    EFI_GUID                            **ProtocolGuidArray;
    UINTN                               ArrayCount;
    UINTN                               ProtocolIndex;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *OpenInfo;
    UINTN                               OpenInfoCount;
    UINTN                               OpenInfoIndex;
    UINTN                               Index;

    Status = pBS->LocateHandleBuffer(
                    AllHandles,   
                    NULL,
                    NULL,
                    &HandleCount, 
                    &HandleBuffer);
    if (EFI_ERROR(Status)) return EFI_NOT_FOUND;

    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
        ProtocolGuidArray = NULL;
        Status = pBS->ProtocolsPerHandle(
                        HandleBuffer[HandleIndex], 
                        &ProtocolGuidArray, 
                        &ArrayCount);
        if (EFI_ERROR(Status)) continue;

        for (ProtocolIndex = 0; ProtocolIndex < ArrayCount; ProtocolIndex++) {
            Status = pBS->OpenProtocolInformation(
                            HandleBuffer[HandleIndex], 
                            ProtocolGuidArray[ProtocolIndex],
                            &OpenInfo,
                            &OpenInfoCount);
            if (EFI_ERROR(Status)) continue;

            for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++) {
                if (OpenInfo[OpenInfoIndex].ControllerHandle == ControllerHandle) {
                    if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_DRIVER) == EFI_OPEN_PROTOCOL_BY_DRIVER) {
                        for(Index = 0; Index < HandleCount; Index++) {
                            if(HandleBuffer[Index] == OpenInfo[OpenInfoIndex].AgentHandle) {
                                *DriverBindingHandle=HandleBuffer[Index];
                                pBS->FreePool(OpenInfo);
                                pBS->FreePool(ProtocolGuidArray);
                                pBS->FreePool(HandleBuffer);
                                return EFI_SUCCESS;
                            }
                        }
                    }
                }
            }
            if (OpenInfo != NULL) pBS->FreePool(OpenInfo);
        }
         if (ProtocolGuidArray != NULL) pBS->FreePool(ProtocolGuidArray);
    }
    if (HandleBuffer != NULL) pBS->FreePool(HandleBuffer);
    return EFI_NOT_FOUND;
}

/**
    Get all child device handles which are being opened by a specific driver.
    The rountine will allocate pool buffer for the found child device handles,
    and it is the caller's responsibility to safe free the buffer.

        
    @param DriverBindingHandle the handle of a driver which
        contains the binding protocol
    @param ControllerHandle the device controller handle be opened
        by its child device 
    @param ChildControllerHandleCount the number of available
        device handles returned in
        ControllerHandleBuffer
    @param ChildControllerHandleBuffer a pointer to the buffer to
        return the array of child
        device handles

    @retval 
        EFI_STATUS
        If returned status is not succeful or find no available device,
        the *ChildControllerHandleBuffer will be NULL

**/

EFI_STATUS
GetChildDeviceHandlesControledByDriver (
  IN  EFI_HANDLE  DriverBindingHandle,
  IN  EFI_HANDLE  ControllerHandle,
  OUT UINTN       *ChildControllerHandleCount,
  OUT EFI_HANDLE  **ChildControllerHandleBuffer )
{
    UINTN                               HandleCount;
    EFI_HANDLE                          *HandleBuffer;
    BOOLEAN                             *HandleBufferMap;
    EFI_STATUS                          Status;
    UINTN                               HandleIndex;
    UINTN                               AvailableIndex;
    EFI_GUID                            **ProtocolGuidArray;
    UINTN                               ArrayCount;
    UINTN                               ProtocolIndex;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *OpenInfo;
    UINTN                               OpenInfoCount;
    UINTN                               OpenInfoIndex;

    *ChildControllerHandleCount  = 0;
    *ChildControllerHandleBuffer = NULL;
    HandleCount = 0;
    HandleBuffer = NULL;

    if ((DriverBindingHandle == NULL) || (ControllerHandle == NULL)) {
        Status = EFI_INVALID_PARAMETER;
        goto Error;
    }

    //
    // Retrieve the list of all handles from the handle database
    //
    Status = pBS->LocateHandleBuffer (
                    AllHandles,
                    NULL,
                    NULL,
                    &HandleCount,
                    &HandleBuffer);
    if (EFI_ERROR(Status)) goto Error;

    //
    // Create a map for HandleBuffer. If a handle in HandleBuffer is the wanted device handle, its map item is true.
    //
    HandleBufferMap = MallocZ (sizeof (BOOLEAN) * HandleCount);
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
        HandleBufferMap[HandleIndex] = FALSE;
    }

    //
    // Retrieve the list of all the protocols on each handle
    //
    Status = pBS->ProtocolsPerHandle (
                    ControllerHandle,
                    &ProtocolGuidArray,
                    &ArrayCount);
    if (!EFI_ERROR (Status)) {
        for (ProtocolIndex = 0; ProtocolIndex < ArrayCount; ProtocolIndex++) {
            //
            // Retrieve the list of agents that have opened each protocol
            //
            Status = pBS->OpenProtocolInformation (
                            ControllerHandle,
                            ProtocolGuidArray[ProtocolIndex],
                            &OpenInfo,
                            &OpenInfoCount);
            if (!EFI_ERROR (Status)) {
                for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++) {
                    if (OpenInfo[OpenInfoIndex].AgentHandle == DriverBindingHandle) {
                        if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) == EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) {
                            //
                            // OpenInfo[OpenInfoIndex].ControllerHandle is the wanted child device handle, find it in the handlebuffer
                            // A bus driver maybe open a Controller with BY_CHILD_CONTROLLER attribute for different protocol many times,
                            //
                            for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
                                if (OpenInfo[OpenInfoIndex].ControllerHandle == HandleBuffer[HandleIndex]) {
                                    HandleBufferMap[HandleIndex] = TRUE;
                                }
                            }
                        }
                    }
                }
            pBS->FreePool (OpenInfo);
            }
        }
        pBS->FreePool (ProtocolGuidArray);
    }

    //
    // count how many device handles are found
    //
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
        if (HandleBufferMap[HandleIndex]) {
            (*ChildControllerHandleCount)++;
        }
    }

    if (*ChildControllerHandleCount > 0) {
        //
        // Copy the found device handle to returned buffer
        //
        *ChildControllerHandleBuffer = MallocZ (sizeof (EFI_HANDLE) * (*ChildControllerHandleCount));
        for (HandleIndex = 0, AvailableIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
            if (HandleBufferMap[HandleIndex]) {
                (*ChildControllerHandleBuffer)[AvailableIndex] = HandleBuffer[HandleIndex];
                AvailableIndex++;
            }
        }
    }

    if (HandleBuffer != NULL) pBS->FreePool (HandleBuffer);
    return EFI_SUCCESS;

Error:
    if (HandleBuffer != NULL) pBS->FreePool (HandleBuffer);
    return Status;
}

/**
    This function will display Brightness Option in setup menu, 
    if the system GOP supports it.

    @param HiiHandle 

    @retval VOID
**/
VOID
IntelSaGopSetupInfo(
    EFI_HII_HANDLE                   HiiHandle,
    UINT16                           Class
)
{
  EFI_STATUS                         Status;
  UINTN                              SetupSize;
  NB_GOP_PLATFORM_DATA               NBGopPlatformData = {0};
  UINTN                              ControllerHandleIndex;
  EFI_HANDLE                         ControllerHandle;
  UINTN                              ControllerHandleCount = 0;
  EFI_HANDLE                         *ControllerHandleBuffer = NULL;
  EFI_HANDLE                         DriverBindingHandle;
  EFI_PCI_IO_PROTOCOL                *PciIo;
  UINTN                              ChildHandleCount = 0;
  EFI_HANDLE                         *ChildHandleBuffer = NULL;
  EFI_HANDLE                         ChildHandle;
  UINTN                              ChildHandleIndex = 0;
  UINT8                              GopBistEnable;
  UINT32                             CurrentBrightness = 255;
  GOP_DISPLAY_BRIGHTNESS_PROTOCOL    *GopDisplayBrightnessProtocol = NULL;
  GOP_DISPLAY_BIST_PROTOCOL          *GopDisplayBistProtocol = NULL;
  SETUP_DATA                         SetupData;

  SetupSize = sizeof (SETUP_DATA);
  Status = pRS->GetVariable (
                  L"Setup",
                  &gSetupGuid,
                  NULL,
                  &SetupSize,
                  &SetupData
                  );
  
  GopBistEnable = (Status == EFI_SUCCESS) ? SetupData.EnableBIST : FALSE;

  //
  // Get all drivers handles which has PCI IO Protocol
  //
  Status = pBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &ControllerHandleCount,
                  &ControllerHandleBuffer);
  if (EFI_ERROR(Status)) return;

  for (ControllerHandleIndex = 0; ControllerHandleIndex < ControllerHandleCount; ControllerHandleIndex++) {

        ControllerHandle = ControllerHandleBuffer[ControllerHandleIndex];
        Status = pBS->HandleProtocol (ControllerHandle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
        if (EFI_ERROR(Status)) continue;

        //
        // Get Driver Binding Protocol for this VGA
        //
        Status = GetIntelSaGopSetupDriverBindingHandle (ControllerHandle, &DriverBindingHandle);
        if (EFI_ERROR(Status)) continue;

        Status = AmiGopDeviceCheck (ControllerHandle, DriverBindingHandle, PciIo);
        if (EFI_ERROR(Status)) continue;

        Status = GetChildDeviceHandlesControledByDriver (DriverBindingHandle, ControllerHandle, &ChildHandleCount, &ChildHandleBuffer);

        if (!EFI_ERROR(Status)) {
            for (ChildHandleIndex = 0; ChildHandleIndex < ChildHandleCount; ChildHandleIndex++) {
                ChildHandle = ChildHandleBuffer[ChildHandleIndex];

                  //
                  // Check if this device have Graphic Output Portocol.
                  // If it does, the system would support Brightness option in setup Menu.
                  //
                  Status = pBS->OpenProtocol (
                                  ChildHandle,
                                  &gEfiGraphicsOutputProtocolGuid,
                                  NULL,
                                  NULL,
                                  NULL,
                                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

                  if (!EFI_ERROR(Status)) {

                     Status = pBS->OpenProtocol (
                                     ChildHandle,
                                     &gGopDisplayBrightnessProtocolGuid,
                                     (VOID**)&GopDisplayBrightnessProtocol,
                                     NULL,
                                     NULL,
                                     EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

                     if (!EFI_ERROR(Status)) {

                        if(GopDisplayBrightnessProtocol->Revision >= GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01) {

                           NBGopPlatformData.BrightnessAvailable = 1;
                           Status = GopDisplayBrightnessProtocol->GetCurrentBrightnessLevel(GopDisplayBrightnessProtocol, &CurrentBrightness);
                           
                           if (EFI_ERROR(Status)) CurrentBrightness = 255;
                        }//if(GopDisplayBrightnessProtocol->Revision >= GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01)
                     }

                     Status = pBS->OpenProtocol (
                                     ChildHandle,
                                     &gGopDisplayBistProtocolGuid,
                                     (VOID**)&GopDisplayBistProtocol,
                                     NULL,
                                     NULL,
                                     EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

                     if (!EFI_ERROR(Status)) {

                        if(GopDisplayBistProtocol->Revision >= GOP_DISPLAY_BIST_PROTOCOL_REVISION_01) {

                           NBGopPlatformData.GopBistAvailable = 1;

                           if (GopBistEnable == 1)
                              Status = GopDisplayBistProtocol->EnableBist(GopDisplayBistProtocol);
                           else
                              Status = GopDisplayBistProtocol->DisableBist(GopDisplayBistProtocol);
                        }//if(GopDisplayBistProtocol->Revision >= GOP_DISPLAY_BIST_PROTOCOL_REVISION_01)
                     }

                  }
                  
                  Status = pRS->SetVariable (
                                  L"NBGopPlatformData",
                                  &gSetupGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                  sizeof (NB_GOP_PLATFORM_DATA),
                                  &NBGopPlatformData
                                  );

            }//for (ChildHandleIndex = 0; ChildHandleIndex < ChildHandleCount; ChildHandleIndex++)
        }//if (!EFI_ERROR(Status)) 
  }//for (ControllerHandleIndex = 0; ControllerHandleIndex < ControllerHandleCount; ControllerHandleIndex++) 
  return;
}

/**
    For overriding ELink, AmiDefaultGopSwitchFunction.
    If it's Intel Device, the system should do this function, IntelGopSwitchFunction.

    @param None.

    @retval None.

    @note  None.

**/
EFI_STATUS
IntelGopSwitchFunction (
    IN  EFI_HANDLE                  ControllerHandle, 
    IN  EFI_HANDLE                  DriverBindingHandle,
    IN  SETUP_DATA                  *SetupData,
    IN  EFI_DEVICE_PATH_PROTOCOL    *DevicePath )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    EFI_HANDLE                      *ChildHandleBuffer = NULL;
    EFI_HANDLE                      OutPutDevHandle;

    EFI_DEVICE_PATH_PROTOCOL        *GopDevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL        *GopAcpiAdrDPNode = NULL;

//#if defined(EFI_SPECIFICATION_VERSION) && EFI_SPECIFICATION_VERSION > 0x20000 // [ EIP427935 ]
    UINTN                           SetupSize;
//#endif
    NB_GOP_PLATFORM_DATA            NBGopPlatformData = {0};
    GOP_DISPLAY_BRIGHTNESS_PROTOCOL *GopDisplayBrightnessProtocol = NULL;
    GOP_DISPLAY_BIST_PROTOCOL       *GopDisplayBistProtocol = NULL;
    UINT32                          MaxBrightness = 255;
    UINT32                          CurrentBrightness = 255;

    EFI_PCI_IO_PROTOCOL             *PciIo;
    PCI_DEV_INFO                    *Dev;

    Status = pBS->HandleProtocol (ControllerHandle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    if (EFI_ERROR(Status)) return Status;

    Dev = (PCI_DEV_INFO*)PciIo;

    //
    // If it's not Intel VGA, don't do this part. 
    //
    if (Dev->DevVenId.VenId != 0x8086) {
      return EFI_UNSUPPORTED;
    } 

//#if defined(EFI_SPECIFICATION_VERSION) && EFI_SPECIFICATION_VERSION > 0x20000 // [ EIP427935 ]
    SetupSize = sizeof (NB_GOP_PLATFORM_DATA);
//#endif
    GopAcpiAdrDPNode = DPGetLastNode(DevicePath);
    Status = pBS->ConnectController( ControllerHandle, NULL, GopAcpiAdrDPNode, TRUE );

//    Status = GetChildDeviceHandlesControledByDriver (DriverBindingHandle, ControllerHandle, &ChildHandleCount, &ChildHandleBuffer);

    if (!EFI_ERROR(Status)) {
      //
      // Find out Current Installed GOP Device
      //
//      for (ChildHandleIndex = 0; ChildHandleIndex < ChildHandleCount; ChildHandleIndex++) {
//        if (SetupData->GopOutputSelect == ChildHandleIndex) {
//
//          Status = pBS->OpenProtocol (
//                            ChildHandleBuffer[ChildHandleIndex],
//                            &gEfiDevicePathProtocolGuid,
//                            (VOID**)&GopDevicePath,
//                            NULL,
//                            NULL,
//                            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
//          if (EFI_ERROR(Status)) break;
//
//          //
//          // If Current installed GOP is Intels', conncect it.
//          //
//          if (!EFI_ERROR(Status)) {
//            AcpiDPNode = (ACPI_ADR_DEVICE_PATH*)GopAcpiAdrDPNode = DPGetLastNode(GopDevicePath);
//            Status = pBS->ConnectController( ControllerHandle, NULL, GopAcpiAdrDPNode, TRUE );
//          }
//
//          //
//          // Check if this device have Graphic Output Portocol.
//          // If it does, the system would support Brightness option in setup Menu.
//          //
//          Status = pBS->OpenProtocol (
//                            ChildHandleBuffer[ChildHandleIndex],
//                            &gEfiGraphicsOutputProtocolGuid,
//                            NULL,
//                            NULL,
//                            NULL,
//                            EFI_OPEN_PROTOCOL_TEST_PROTOCOL);
          Status = pBS->LocateDevicePath(&gGopDisplayBrightnessProtocolGuid, &DevicePath, &OutPutDevHandle);
          
          if (!EFI_ERROR(Status)) {

              Status = pBS->OpenProtocol (
                                OutPutDevHandle,
                                &gGopDisplayBrightnessProtocolGuid,
                                (VOID**)&GopDisplayBrightnessProtocol,
                                NULL,
                                NULL,
                                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
              if (!EFI_ERROR(Status)) {
                  if(GopDisplayBrightnessProtocol->Revision >= GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01) {
                      NBGopPlatformData.BrightnessAvailable = 1;  
                      Status = GopDisplayBrightnessProtocol->GetMaxBrightnessLevel(GopDisplayBrightnessProtocol, &MaxBrightness);
                      if (!EFI_ERROR(Status)) {
                          if (SetupData->GopBrightness > MaxBrightness) {
                                  SetupData->GopBrightness = MaxBrightness;
//#if defined(EFI_SPECIFICATION_VERSION) && EFI_SPECIFICATION_VERSION > 0x20000 // [ EIP427935 ]
                                 Status = HiiLibSetBrowserData (SetupSize, SetupData, &gSetupGuid, L"Setup");
//#endif
                           } 
                           Status = GopDisplayBrightnessProtocol->GetCurrentBrightnessLevel(GopDisplayBrightnessProtocol, &CurrentBrightness);
                           if (!EFI_ERROR(Status)) {
                               if(CurrentBrightness != SetupData->GopBrightness)
                                   Status = GopDisplayBrightnessProtocol->SetBrightnessLevel(GopDisplayBrightnessProtocol, SetupData->GopBrightness);
                           }
                       }
                   } // GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01
               } //OpenProtocol gGopDisplayBrightnessProtocol

               Status = pBS->OpenProtocol (
                              OutPutDevHandle,
                              &gGopDisplayBistProtocolGuid,
                              (VOID**)&GopDisplayBistProtocol,
                              NULL,
                              NULL,
                              EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

               if (!EFI_ERROR(Status)) {

                  if(GopDisplayBistProtocol->Revision >= GOP_DISPLAY_BIST_PROTOCOL_REVISION_01) {

                     NBGopPlatformData.GopBistAvailable = 1;

                     if (SetupData->EnableBIST == 1)
                        Status = GopDisplayBistProtocol->EnableBist(GopDisplayBistProtocol);
                     else
                        Status = GopDisplayBistProtocol->DisableBist(GopDisplayBistProtocol);
                  }//if(GopDisplayBistProtocol->Revision >= GOP_DISPLAY_BIST_PROTOCOL_REVISION_01)
               } //OpenProtocol gGopDisplayBistProtocolGuid

           }     
//        }// if (SetupData->GopOutputSelect == ChildHandleIndex)
//      }// for (ChildHandleIndex = 0; ChildHandleIndex < ChildHandleCount; ChildHandleIndex++)
    }

    Status = HiiLibSetBrowserData (
                      sizeof (NB_GOP_PLATFORM_DATA), 
                      &NBGopPlatformData, 
                      &gSetupGuid, 
                      L"NBGopPlatformData"
                      );

    return Status;
}

/**
    If the device support Multifunction, it supports its key 
    callbak funtion.

    @param None.

    @retval None.

    @note  None.

**/
EFI_STATUS IntelGopMultifunctionCallback (
    IN EFI_HII_HANDLE           HiiHandle, 
    IN UINT16                   Class, 
    IN UINT16                   SubClass, 
    IN UINT16                   Key )
{

    EFI_STATUS                         Status = EFI_SUCCESS;
    SETUP_DATA                         *SetupData = NULL;
    CALLBACK_PARAMETERS                *CallbackParameters;
    UINTN                              SetupSize;
    EFI_STATUS                         NBGopPlatformDataStatus = EFI_UNSUPPORTED;
    NB_GOP_PLATFORM_DATA               NBGopPlatformData = {0};
    UINTN                              ControllerHandleIndex;
    EFI_HANDLE                         ControllerHandle;
    UINTN                              ControllerHandleCount = 0;
    EFI_HANDLE                         *ControllerHandleBuffer = NULL;

    EFI_HANDLE                         DriverBindingHandle;

    EFI_PCI_IO_PROTOCOL                *PciIo;

    UINTN                              ChildHandleCount = 0;
    EFI_HANDLE                         *ChildHandleBuffer = NULL;
    EFI_HANDLE                         ChildHandle;
    UINTN                              ChildHandleIndex = 0;

    UINT32                             CurrentBrightness = 255;
    GOP_DISPLAY_BRIGHTNESS_PROTOCOL    *GopDisplayBrightnessProtocol = NULL;
    GOP_DISPLAY_BIST_PROTOCOL          *GopDisplayBistProtocol = NULL;

    UINT32                             MaxBrightness = 255;

    SetupSize = sizeof (SETUP_DATA);

    CallbackParameters = GetCallbackParameters();

//#if ((TSE_BUILD >= 0x1224) && (EFI_SPECIFICATION_VERSION >= 0x2000A)) // [ EIP427935 ]
#if (TSE_BUILD >= 0x1224) 
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) return Status;
//#elif ((TSE_BUILD > 0x1208) && (EFI_SPECIFICATION_VERSION >= 0x2000A))
#elif (TSE_BUILD > 0x1208)    
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) return Status;
#endif

    Status = pBS->AllocatePool (EfiBootServicesData, SetupSize, (VOID **)&SetupData);
    if(EFI_ERROR(Status)) return Status;

//#if defined(EFI_SPECIFICATION_VERSION) && EFI_SPECIFICATION_VERSION > 0x20000 // [ EIP427935 ]
    Status = HiiLibGetBrowserData (&SetupSize, SetupData, &gSetupGuid, L"Setup");
    if(EFI_ERROR(Status)) return Status;
//#else
//    SetupData = (SETUP_DATA*)CallbackParameters->Data->NvRamMap;
//#endif

//#if defined(EFI_SPECIFICATION_VERSION) && EFI_SPECIFICATION_VERSION > 0x20000 // [ EIP427935 ]
    SetupSize = sizeof (NB_GOP_PLATFORM_DATA);
    NBGopPlatformDataStatus = HiiLibGetBrowserData (&SetupSize, &NBGopPlatformData, &gSetupGuid, L"NBGopPlatformData");
//#endif
  //
  // Get all drivers handles which has PCI IO Protocol
  //
  Status = pBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &ControllerHandleCount,
                  &ControllerHandleBuffer);
  if (EFI_ERROR(Status)) return Status;

  for (ControllerHandleIndex = 0; ControllerHandleIndex < ControllerHandleCount; ControllerHandleIndex++) {

        ControllerHandle = ControllerHandleBuffer[ControllerHandleIndex];
        Status = pBS->HandleProtocol (ControllerHandle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
        if (EFI_ERROR(Status)) continue;

        Status = GetIntelSaGopSetupDriverBindingHandle (ControllerHandle, &DriverBindingHandle);
        if (EFI_ERROR(Status)) continue;

        Status = AmiGopDeviceCheck (ControllerHandle, DriverBindingHandle, PciIo);
        if (EFI_ERROR(Status)) continue;

        Status = GetChildDeviceHandlesControledByDriver (DriverBindingHandle, ControllerHandle, &ChildHandleCount, &ChildHandleBuffer);

        if (!EFI_ERROR(Status)) {
            for (ChildHandleIndex = 0; ChildHandleIndex < ChildHandleCount; ChildHandleIndex++) {
                ChildHandle = ChildHandleBuffer[ChildHandleIndex];

                  //
                  // Check if this device have Graphic Output Portocol.
                  // If it does, the system would support Brightness option in setup Menu.
                  //
                  Status = pBS->OpenProtocol (
                                ChildHandle,
                                &gEfiGraphicsOutputProtocolGuid,
                                NULL,
                                NULL,
                                NULL,
                                EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

                  if (!EFI_ERROR(Status)) {

                     if (Key == AGP_BRIGHTNESS_KEY) {
                         Status = pBS->OpenProtocol (
                                       ChildHandle,
                                       &gGopDisplayBrightnessProtocolGuid,
                                       (VOID**)&GopDisplayBrightnessProtocol,
                                       NULL,
                                       NULL,
                                       EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
                         if (!EFI_ERROR(Status)) {
                            if(GopDisplayBrightnessProtocol->Revision >= GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01) {
                               NBGopPlatformData.BrightnessAvailable = 1;  
                               Status = GopDisplayBrightnessProtocol->GetMaxBrightnessLevel(GopDisplayBrightnessProtocol, &MaxBrightness);
                               if (!EFI_ERROR(Status)) {
                                    if (SetupData->GopBrightness > MaxBrightness) {
                                        SetupData->GopBrightness = MaxBrightness;
//#if defined(EFI_SPECIFICATION_VERSION) && EFI_SPECIFICATION_VERSION > 0x20000 // [ EIP427935 ]
                                        SetupSize = sizeof (SETUP_DATA);
                                        Status = HiiLibSetBrowserData (SetupSize, SetupData, &gSetupGuid, L"Setup");
//#endif
                                    } 
                                    Status = GopDisplayBrightnessProtocol->GetCurrentBrightnessLevel(GopDisplayBrightnessProtocol, &CurrentBrightness);
                                    if (!EFI_ERROR(Status)) {
                                       if(CurrentBrightness != SetupData->GopBrightness)
                                           Status = GopDisplayBrightnessProtocol->SetBrightnessLevel(GopDisplayBrightnessProtocol, SetupData->GopBrightness);
                                    }
                               }
                            } // GOP_DISPLAY_BRIGHTNESS_PROTOCOL_REVISION_01
                         } //OpenProtocol GopDisplayBrightnessProtocol
                     } // AGP_BRIGHTNESS_KEY

                     if (Key == AGP_BIST_KEY) {
                         Status = pBS->OpenProtocol (
                                        ChildHandle,
                                        &gGopDisplayBistProtocolGuid,
                                        (VOID**)&GopDisplayBistProtocol,
                                        NULL,
                                        NULL,
                                        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

                         if (!EFI_ERROR(Status)) {

                            if(GopDisplayBistProtocol->Revision >= GOP_DISPLAY_BIST_PROTOCOL_REVISION_01) {

                               NBGopPlatformData.GopBistAvailable = 1;

                               if (SetupData->EnableBIST == 1)
                                  Status = GopDisplayBistProtocol->EnableBist(GopDisplayBistProtocol);
                               else
                                  Status = GopDisplayBistProtocol->DisableBist(GopDisplayBistProtocol);
                            }//if(GopDisplayBistProtocol->Revision >= GOP_DISPLAY_BIST_PROTOCOL_REVISION_01)
                         } //OpenProtocol gGopDisplayBistProtocolGuid
                     } // AGP_BIST_KEY
                  }//OpenProtocol gEfiGraphicsOutputProtocolGuid

            }//for (ChildHandleIndex = 0; ChildHandleIndex < ChildHandleCount; ChildHandleIndex++)
        }//if (!EFI_ERROR(Status)) 
  }//for (ControllerHandleIndex = 0; ControllerHandleIndex < ControllerHandleCount; ControllerHandleIndex++) 

//#if defined(EFI_SPECIFICATION_VERSION) && EFI_SPECIFICATION_VERSION > 0x20000 // [ EIP427935 ]
  if(!EFI_ERROR(NBGopPlatformDataStatus)) {
     HiiLibSetBrowserData (
              sizeof (NB_GOP_PLATFORM_DATA), 
              &NBGopPlatformData, 
              &gSetupGuid, 
              L"NBGopPlatformData"
              );
  }
//#endif
  return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
