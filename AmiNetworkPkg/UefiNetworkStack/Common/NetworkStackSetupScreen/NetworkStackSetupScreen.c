//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
/** @file NetworkStackSetupScreen.c
    NetworkStackSetupScreen related functions

**/
//**********************************************************************
#include <AmiDxeLib.h>
#include <NetworkStackSetup.h>
#include <Token.h>
#include <Library/DebugLib.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>


///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH               VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         End;
} HII_VENDOR_DEVICE_PATH;

NETWORK_STACK  	mNetworkStackData;
extern UINT8    NetworkStackSetupBin[];

HII_VENDOR_DEVICE_PATH  mNetworkStackHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    }, 
    { 0xD1405D16, 0x7AFC, 0x4695, {0xBB, 0x12, 0x41, 0x45, 0x9D, 0x36, 0x95, 0xA2} }
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { 
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};
/**
    Entry point of the  network stack setup driver. This entry point is 
    necessary to initialize the NetworkStack setup driver.

    @param 
        ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
        SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table  
 
    @retval 
  EFI_SUCCESS:              Driver initialized successfully
**/

EFI_STATUS EFIAPI InitNetworkStackVar (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{

  UINTN                Size;
  EFI_STATUS           Status;
  EFI_HII_HANDLE     RegisteredHandle;

  //InitAmiLib(ImageHandle,SystemTable);

  Size = sizeof(NETWORK_STACK);
  Status = SystemTable->RuntimeServices->GetVariable(L"NetworkStackVar",&gEfiNetworkStackSetupGuid, NULL, &Size, &mNetworkStackData);

  if (Status == EFI_NOT_FOUND) {

    SystemTable->BootServices->SetMem(&mNetworkStackData, 
                    sizeof(NETWORK_STACK), 
                    0);
    //
    // Sync Default values for Network configuration across the component
    //
    mNetworkStackData.Enable   = NWSTACK_DEFAULT_VALUE;
    mNetworkStackData.Ipv4Http = NETWORKSTACK_IPV4_HTTP_SUPPORT;
    mNetworkStackData.Ipv4Pxe  = NETWORKSTACK_IPV4_PXE_SUPPORT;
    mNetworkStackData.Ipv6Http = NETWORKSTACK_IPV6_HTTP_SUPPORT;
    mNetworkStackData.Ipv6Pxe  = NETWORKSTACK_IPV6_PXE_SUPPORT;
    mNetworkStackData.MediaDetectCount = DEFAULT_MEDIA_DETECT_COUNT;
    
#if (IpSecDxe_SUPPORT == 1)
    mNetworkStackData.IpsecCertificate = 1;
#else   
    mNetworkStackData.IpsecCertificate = 0;
#endif    	

    Status = SystemTable->RuntimeServices->SetVariable(
         L"NetworkStackVar",&gEfiNetworkStackSetupGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof(NETWORK_STACK), &mNetworkStackData
     );

  } 

  //
  // Install Device Path Protocol on driver handle
  //
  Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mNetworkStackHiiVendorDevicePath,
                  NULL
                  );
  
  //
  // Publish our HII data.
  //
  RegisteredHandle = HiiAddPackages (
                                     &gEfiNetworkStackSetupGuid,
                                     ImageHandle,
                                     NetworkStackSetupScreenStrings,
                                     NetworkStackSetupBin,
                                     NULL
                                     );
  return EFI_SUCCESS;

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

