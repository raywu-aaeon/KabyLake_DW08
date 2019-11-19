//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file NvmeRstPassword
	Nvme password implemnetation for Intel Rst driver
**/

//---------------------------------------------------------------------------
#include<Token.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/AmiHddSecurityInit.h>
#include <Protocol/NvmExpressPassthru.h>
#include <Include/NvmeRstPwdIncludes.h>
#include <Library/UefiBootServicesTableLib.h>
#include <IndustryStandard/Nvme.h>

EFI_GUID    gRstStatusProtocolGuid = RST_STATUS_PROTOCOL_GUID;
EFI_GUID    gRstPasswordDeviceProtocolGuid = NVME_RST_PASSWORD_PROTOCOL_GUID;

//---------------------------------------------------------------------------
AMI_HDD_SECURITY_INIT_PROTOCOL      *gNvmeSecurityInitProtocol = NULL;
AMI_HDD_SECURITY_INIT_PROTOCOL      *gNvmeTcgStorageSecurityInitProtocol = NULL;

VOID                                *gNvmeRstRegistration;
EFI_EVENT                           gNvmeRstEvent;
#ifndef     NVME_COMMAND_TIMEOUT
#define     NVME_COMMAND_TIMEOUT            0x01
#endif

EFI_STATUS 
EFIAPI
NvmeRstPwdSupported (
    IN  EFI_DRIVER_BINDING_PROTOCOL *This,
    IN  EFI_HANDLE                  Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
);

EFI_STATUS 
EFIAPI
NvmeRstPwdStart (
    IN  EFI_DRIVER_BINDING_PROTOCOL *This,
    IN  EFI_HANDLE                  Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
);

EFI_STATUS 
EFIAPI
NvmeRstPwdStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL *This,
    IN  EFI_HANDLE                  Controller,
    IN  UINTN                       NumberOfChildren,
    IN  EFI_HANDLE                  *ChildHandleBuffer
);

EFI_DRIVER_BINDING_PROTOCOL gNvmeRstPwdDrverBinding = {
    NvmeRstPwdSupported, 
    NvmeRstPwdStart,     
    NvmeRstPwdStop,      
    1,  // version
    NULL,                    // ImageHandle
    NULL                     // DriverBindingHandle
};

/** @internal
    Function coverts Nvme Model number into Unicodestring.

    @param NvmeController

    @retval EFI_STATUS
    @endinternal
**/
EFI_STATUS
GetNvmeUnicodeStringName (
    NVME_RST_INTERFACE              *NvmeController,
    NVME_ADMIN_CONTROLLER_DATA      *IdentifyControllerData
)
{
    EFI_STATUS                  Status=EFI_SUCCESS;
    UINT16                      DeviceName[41];
    CHAR8                       Language[] = "en-US";
    EFI_UNICODE_STRING_TABLE    *tempUnicodeTable;
    UINT32                      Index;
    
    gBS->SetMem(DeviceName, sizeof(DeviceName), 0);

    for (Index = 0; Index < 40; Index ++) {
        DeviceName[Index] = ((UINT8 *)IdentifyControllerData->Mn)[Index];
    }

    DeviceName[40] = 0; // Word
    
    // Remove the spaces from the end of the device name
    for (Index = 39; Index > 0; Index-- ) {
        
        if (DeviceName[Index] != 0x20) { 
             break;
        }
        DeviceName[Index] = 0;
    }

    Status = gBS->AllocatePool ( EfiBootServicesData,
                                 sizeof (EFI_UNICODE_STRING_TABLE) * 2,
                                 (VOID**)&tempUnicodeTable
                                 );

    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    gBS->SetMem(tempUnicodeTable, sizeof(EFI_UNICODE_STRING_TABLE) * 2, 0);

    Status = gBS->AllocatePool ( EfiBootServicesData,
                                 sizeof (Language),
                                (VOID**)&tempUnicodeTable[0].Language
                                );

    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    Status = gBS->AllocatePool ( EfiBootServicesData,
                                 sizeof (DeviceName),
                                 (VOID**)&tempUnicodeTable[0].UnicodeString
                                 );

    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    gBS->CopyMem(tempUnicodeTable[0].Language, &Language, sizeof(Language)); 
    gBS->CopyMem(tempUnicodeTable[0].UnicodeString, DeviceName, sizeof (DeviceName)); 
    tempUnicodeTable[1].Language = NULL;
    tempUnicodeTable[1].UnicodeString = NULL;
    NvmeController->UDeviceName = tempUnicodeTable;

    return Status;
}

/** @internal
    Function Get NvmeIdentifyData

    @param NvmeDevice
    @param NamespaceId
    @param Buffer

    @retval EFI_STATUS
    @endinternal
**/
EFI_STATUS
GetNvmeIdentifyData (
  IN  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *NvmeDevice,
  IN  UINT32                                NamespaceId,
  OUT VOID                                  *Buffer
  )
{
  EFI_STATUS                                Status;
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  gNvmeCommandPacket;
  EFI_NVM_EXPRESS_COMMAND                   gNvmeCmd;
  EFI_NVM_EXPRESS_COMPLETION                gNvmeCompletion;

  // Clear the Global gNvmeCommandPacket structure to Zero
  gBS->SetMem( &gNvmeCommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET), 0 );
  
  // Clear Global Nvm Express Admin Command to Zero
  gBS->SetMem(&gNvmeCmd, sizeof(EFI_NVM_EXPRESS_COMMAND), 0);

  // Clear Global Nvm Express completion structure to Zero
  gBS->SetMem(&gNvmeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION), 0);
  gNvmeCmd.Cdw0.Opcode = 0x06;

  gNvmeCmd.Nsid                 	= NamespaceId;
  gNvmeCommandPacket.NvmeCmd        = &gNvmeCmd;
  gNvmeCommandPacket.NvmeCompletion = &gNvmeCompletion;
  gNvmeCommandPacket.TransferBuffer = Buffer;
  gNvmeCommandPacket.TransferLength = sizeof (NVME_ADMIN_CONTROLLER_DATA);
  gNvmeCommandPacket.CommandTimeout = NVME_COMMAND_TIMEOUT * 10000000;   // Timeout in 100ns Unit
  gNvmeCommandPacket.QueueType      = 0;              // 0 - Admin Command type, 1- I/O command type 
  //
  // Set bit 0 (Cns bit) to 1 to identify a controller
  //
  gNvmeCmd.Cdw10                	= 1;
  gNvmeCmd.Flags                	= CDW10_VALID;

  Status = NvmeDevice->PassThru (NvmeDevice, NamespaceId, &gNvmeCommandPacket, NULL);
    
  return Status;
}

/**
    Install BlockIo, DiskInfo and DevicePath protocol for Active namespace

    @param This 
    @param NvmeController 

    @retval EFI_STATUS

**/
EFI_STATUS
InstallSecurityProtocol (
    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  EFI_HANDLE               ControllerHandle,
    IN  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *NvmePassthruProtocol
) 
{
    EFI_STATUS                              Status;   
    UINT32                                  NamespaceId;
    VOID                                    *AmiHddSecurityEndProtocol=NULL;    
    EFI_PCI_IO_PROTOCOL                     *PciIo = NULL;
    NVME_RST_INTERFACE                      *NvmeDeviceInterface = NULL;
    NVME_ADMIN_CONTROLLER_DATA               *ControllerData;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY     *OpenInfoBuffer;
    UINTN                                    EntryCount;
    UINTN                                   OpenInfoIndex;
    EFI_HANDLE                              DeviceHandle;
    
    // Getting the detected NamespaceId for the each of drive
    // Due to the fact that RAID Driver expects other value of NamespaceId parameter than AHCI driver,
    // need to send the Identify Controller command twice - once with the NamespaceId retrieved
    // using GetNextNamespace. If it fails we need to set NamespaceId = 0.      
    
    Status = gBS->HandleProtocol (
                            ControllerHandle,
                            &gEfiPciIoProtocolGuid,
                            (VOID **) &PciIo
                            );
    if(EFI_ERROR(Status)) {
        return Status;
    }
    
    NamespaceId = 0xFFFFFFFF;
    while (TRUE) {
        
        Status = NvmePassthruProtocol->GetNextNamespace (NvmePassthruProtocol, &NamespaceId);          

        if(!EFI_ERROR(Status)) {
            
            Status = gBS->AllocatePool(EfiBootServicesData,
                                 sizeof(NVME_RST_INTERFACE),
                                 (VOID**)&NvmeDeviceInterface);
            ASSERT_EFI_ERROR(Status);
        
            gBS->SetMem(NvmeDeviceInterface, sizeof(NVME_RST_INTERFACE), 0);              
        
            // Get the IdentifyData
            Status = gBS->AllocatePool (EfiBootServicesData,
                                   sizeof (NVME_ADMIN_CONTROLLER_DATA),
                                   (VOID**)&ControllerData
                                   );
               
            gBS->SetMem(ControllerData, sizeof(NVME_ADMIN_CONTROLLER_DATA), 0);
                   
            Status = GetNvmeIdentifyData (NvmePassthruProtocol, NamespaceId, (VOID *) ControllerData);                       
        
            if (EFI_ERROR (Status)) {
                Status = GetNvmeIdentifyData (NvmePassthruProtocol, 0, (VOID *) ControllerData);
            }
            ASSERT_EFI_ERROR(Status);
            DeviceHandle = NULL;
            Status = gBS->InstallMultipleProtocolInterfaces (&DeviceHandle,
                                                       &gRstPasswordDeviceProtocolGuid,
                                                       NvmeDeviceInterface,
                                                       NULL
                                                       );
        
            // Open PCI IO protocol by CHILD
            Status = gBS->OpenProtocol (
                        ControllerHandle,
                        &gEfiPciIoProtocolGuid,
                        (VOID **)&PciIo,
                        This->DriverBindingHandle,     
                        DeviceHandle,   
                        EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);
                        
            DEBUG ((DEBUG_INFO, "GetNvmeIdentifyData NamespaceId = %x\n",NamespaceId));
            DEBUG ((DEBUG_INFO, "ControllerData->OACS = %x\n",ControllerData->Oacs));        
            DEBUG ((DEBUG_INFO, "ControllerData->VID = %x\n",ControllerData->Vid));                 
            DEBUG ((DEBUG_INFO, "ControllerData->SSVID = %x\n",ControllerData->Ssvid));                                          
            NvmeDeviceInterface->Signature = RST_PWD_SIGNATURE;
            NvmeDeviceInterface->RstControlled = TRUE;                    //use this flag in smm driver
            NvmeDeviceInterface->RstControllerHandle = ControllerHandle;  
            NvmeDeviceInterface->ControllerHandle = DeviceHandle;     //For raid driver , it only install one passthtu protocol on top of controller ,not each of device
            NvmeDeviceInterface->PciIO = PciIo;
            NvmeDeviceInterface->EfiNvmExpressPassThru = NvmePassthruProtocol;
            NvmeDeviceInterface->RstNamespaceId = NamespaceId;              
            NvmeDeviceInterface->OACS = ControllerData->Oacs;
            GetNvmeUnicodeStringName(NvmeDeviceInterface, ControllerData);
                    
            // For Raid driver password , need to set SELECT_SAT3_OPAL_SECURITY_PROTOCOL = 0.
            // Install HDDSecurity protocol for NVMe device that supports ATA security Protocol
            if(gNvmeSecurityInitProtocol == NULL) {
                  
                Status = gBS->LocateProtocol(&gAmiHddSecurityInitProtocolGuid,
                                            NULL,
                                            (VOID **) &gNvmeSecurityInitProtocol);                 
            }
        
            if( gNvmeSecurityInitProtocol != NULL) {
                Status = gNvmeSecurityInitProtocol->InstallSecurityInterface(NvmeDeviceInterface, AmiStorageInterfaceNvme );

            }
            // If install SAT3 security interface fail/unsupported then install Tcg security
            if (EFI_ERROR (Status)) {
                if(gNvmeTcgStorageSecurityInitProtocol == NULL) {
                    Status = gBS->LocateProtocol(&gAmiTcgStorageSecurityInitProtocolGuid,
                                                   NULL,
                                                  (VOID **) &gNvmeTcgStorageSecurityInitProtocol);
          
                }
        
                if( gNvmeTcgStorageSecurityInitProtocol != NULL ) {

                    Status = gBS->OpenProtocolInformation (
                                              ControllerHandle,
                                              &gEfiPciIoProtocolGuid,
                                              &OpenInfoBuffer,
                                              &EntryCount);       
                    
                    for (OpenInfoIndex = 0; OpenInfoIndex < EntryCount; OpenInfoIndex++) {
                        if ((OpenInfoBuffer[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) != 0) {
                            Status = gBS->HandleProtocol (
                                                  OpenInfoBuffer[OpenInfoIndex].ControllerHandle,
                                                  &gEfiStorageSecurityCommandProtocolGuid,
                                                  (VOID **) &(NvmeDeviceInterface->EfiStorageSecInterface));
                            if(!EFI_ERROR(Status))
                                break;
                        }
                    }
                    
                    if ( NvmeDeviceInterface->EfiStorageSecInterface != NULL) {
                        Status = gNvmeTcgStorageSecurityInitProtocol->InstallSecurityInterface(NvmeDeviceInterface, AmiStorageInterfaceNvme );
                    } else {
                        Status = EFI_NOT_FOUND;
                    }
                }
            }
        
            // If one of security interface installed successfully , invoke TSE to verify password
            if (!EFI_ERROR (Status)) {   
                Status = gBS->HandleProtocol( DeviceHandle, 
                                        &gAmiHddSecurityEndProtocolGuid, 
                                        &AmiHddSecurityEndProtocol
                                        );
          
                if(!EFI_ERROR(Status)) { 
                    // Protocol already installed on the Controller handle.
                    // UnInstall and Install back the protocol interface to Notify the Password verification 
                    Status = gBS->UninstallProtocolInterface(
                                          DeviceHandle, 
                                          &gAmiHddSecurityEndProtocolGuid, 
                                          NULL
                                          );
                    ASSERT_EFI_ERROR(Status);
                }
          
                // This will notify AMITSE to invoke the HDD password Screen
                Status = gBS->InstallProtocolInterface(&DeviceHandle, 
                                                 &gAmiHddSecurityEndProtocolGuid, 
                                                 EFI_NATIVE_INTERFACE,
                                                 NULL
                                                 );
                
           
            }
        } else if(Status == EFI_NOT_FOUND) {
            break;
        }
    }
    return EFI_SUCCESS;
}
/**

    Checks whether it is a Nvme RST controller or not. 
    If 'yes', return SUCCESS else ERROR

    @param This 
    @param Controller 
    @param RemainingDevicePath 

    @retval EFI_STATUS

                  

**/

EFI_STATUS
EFIAPI
NvmeRstPwdSupported (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
) {
    EFI_STATUS                  Status;
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL      *NvmePassthruProtocol;
    VOID                                    *RstStatusProtocol;    

    // Open gEfiNvmExpressPassThruProtocolGuid BY_DRIVER to get notification 
    // for EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL Reinstallation/Uninstallation.
    //
    Status = gBS->OpenProtocol( Controller,
                            &gEfiNvmExpressPassThruProtocolGuid,
                            (VOID **)&NvmePassthruProtocol,
                            This->DriverBindingHandle,
                            Controller,
                            EFI_OPEN_PROTOCOL_BY_DRIVER);
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    gBS->CloseProtocol (Controller,
                        &gEfiNvmExpressPassThruProtocolGuid,
                        This->DriverBindingHandle,
                        Controller );
    
    // Check if gAmiNvmeControllerProtocolGuid is installed on the controller
    Status = gBS->OpenProtocol( Controller,
                                &gAmiNvmeControllerProtocolGuid,
                                NULL,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

    if(!EFI_ERROR(Status)){
        return EFI_UNSUPPORTED;
    }
    
    // Find RstStatusProtocol to determine RST is run or not
    Status = gBS->LocateProtocol (
                    &gRstStatusProtocolGuid,
                    NULL,
                    (VOID **) &RstStatusProtocol
                    );
    
    if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
    }
    return EFI_SUCCESS;
}
/**
    Installs HddSecurity or TcgStorageSecurity protocol for 
    NVMe devices handled by RST driver.
        
    @param This 
    @param Controller 
    @param RemainingDevicePath 

    @retval 
        EFI_STATUS

**/
EFI_STATUS 
EFIAPI
NvmeRstPwdStart (
    IN  EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN  EFI_HANDLE                       Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath
){
    EFI_STATUS     Status;
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL      *NvmePassthruProtocol;
    VOID                                    *RstStatusProtocol;    
    
    // Open gEfiNvmExpressPassThruProtocolGuid BY_DRIVER to get notification 
    // for EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL Reinstallation/Uninstallation.
    //
    Status = gBS->OpenProtocol( Controller,
                            &gEfiNvmExpressPassThruProtocolGuid,
                            (VOID **)&NvmePassthruProtocol,
                            This->DriverBindingHandle,
                            Controller,
                            EFI_OPEN_PROTOCOL_BY_DRIVER);
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    // Find RstStatusProtocol to determine RST is run or not
    Status = gBS->LocateProtocol (
                    &gRstStatusProtocolGuid,
                    NULL,
                    (VOID **) &RstStatusProtocol
                    );

    if (!EFI_ERROR (Status)) {
        Status = InstallSecurityProtocol(This, Controller, NvmePassthruProtocol);
    }
    
    if(EFI_ERROR(Status)) {
        gBS->CloseProtocol (Controller,
                            &gEfiNvmExpressPassThruProtocolGuid,
                            This->DriverBindingHandle,
                            Controller );
    }
    
    return Status;
}

/**
    Uninstall all devices installed in start procedure.

    @param This 
    @param Controller 
    @param NumberOfChildren 
    @param ChildHandleBuffer 

    @retval EFI_STATUS

**/
EFI_STATUS 
EFIAPI
NvmeRstPwdStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL *This,
    IN  EFI_HANDLE                  Controller,
    IN  UINTN                       NumberOfChildren,
    IN  EFI_HANDLE                  *ChildHandleBuffer
 ) {
    EFI_STATUS                         Status;
    NVME_RST_INTERFACE                 *NvmeRstInterface = NULL;
    UINTN                              Index = 0;
    
    // Check if gEfiNvmExpressPassThruProtocolGuid is installed on the device
    Status = gBS->OpenProtocol( Controller,
                        &gEfiNvmExpressPassThruProtocolGuid,
                        NULL,
                        This->DriverBindingHandle,
                        Controller,
                        EFI_OPEN_PROTOCOL_TEST_PROTOCOL);
    
    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }
    
    while (NumberOfChildren) {
        
        // Does the child handle have the RSTPasswordDeviceProtocol instance
        Status = gBS->OpenProtocol(
                        ChildHandleBuffer[Index],
                        &gRstPasswordDeviceProtocolGuid,
                        (VOID **)&NvmeRstInterface,
                        This->DriverBindingHandle,     
                        Controller,
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        
        
        if (EFI_ERROR(Status)) {
            return EFI_DEVICE_ERROR;
        }
        
        // Before un-installing HDD security check whether it is installed or not
        Status = gBS->OpenProtocol( ChildHandleBuffer[Index],
                                   &gAmiHddSecurityProtocolGuid,
                                   NULL,
                                   This->DriverBindingHandle,
                                   Controller, 
                                   EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                                  );

        if ( Status == EFI_SUCCESS ) {
            if( gNvmeSecurityInitProtocol != NULL) {         
                gNvmeSecurityInitProtocol->StopSecurityModeSupport( 
                                                NvmeRstInterface,  
                                                AmiStorageInterfaceNvme
                                               );
            }
        }

        // Before un-installing TCGStoragesecurity check whether it is installed or not
        Status = gBS->OpenProtocol( ChildHandleBuffer[Index],
                                   &gAmiTcgStorageSecurityProtocolGuid,
                                   NULL,
                                   This->DriverBindingHandle,
                                   Controller,
                                   EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

        if ( Status == EFI_SUCCESS ) {
            if( gNvmeTcgStorageSecurityInitProtocol != NULL) {
                gNvmeTcgStorageSecurityInitProtocol->StopSecurityModeSupport( 
                                                      NvmeRstInterface, 
                                                      AmiStorageInterfaceNvme
                                                     );
            }
        }

        Status = gBS->UninstallMultipleProtocolInterfaces (
               ChildHandleBuffer[Index],
               &gRstPasswordDeviceProtocolGuid,
               NvmeRstInterface,
               NULL);
        
        Index++;
        NumberOfChildren --;
        gBS->FreePool(NvmeRstInterface);
    }
    
    Status = gBS->CloseProtocol ( 
                            Controller,
                            &gEfiNvmExpressPassThruProtocolGuid,
                            This->DriverBindingHandle,
                            Controller);
    return Status;
}
/** @internal
    
    Function NvmeRstPasswordEntryPoint

    @param ImageHandle
    @param *SystemTable
    
    @retval EFI_STATUS
    @endinternal
**/

EFI_STATUS
EFIAPI
NvmeRstPasswordEntryPoint (
    IN  EFI_HANDLE        ImageHandle,
    IN  EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_HANDLE Handle = NULL;
    EFI_STATUS Status;
   
    gNvmeRstPwdDrverBinding.DriverBindingHandle = ImageHandle;
    gNvmeRstPwdDrverBinding.ImageHandle = ImageHandle;
    
    
    Status = gBS->InstallMultipleProtocolInterfaces(
                                    &gNvmeRstPwdDrverBinding.DriverBindingHandle,
                                    &gEfiDriverBindingProtocolGuid, &gNvmeRstPwdDrverBinding,
                                    NULL);
    return Status;
    
}

