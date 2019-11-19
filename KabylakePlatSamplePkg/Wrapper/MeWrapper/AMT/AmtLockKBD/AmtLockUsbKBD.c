//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmtLockUsbKBD.c
    AMT Lock USB KeyBoard Functions under the legacy boot.

**/
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/AmiUsbController.h>
#include <Protocol/AlertStandardFormat.h>
//============================================================================
// Type definitions
//======================================================================
//Function Prototypes
//======================================================================
// Global and extern variables
EFI_SMM_SYSTEM_TABLE2   *gSmst2 = NULL;
AMI_USB_SMM_PROTOCOL    *gAmiUsbSmmProtocol = NULL;

/**
    Use UsbRtKbcAccessControl() to lock keyboard.

          
    @param DispatchHandle 
    @param Context 
    @param CommBuffer 
    @param CommBufferSize 

    @retval EFI_STATUS Status


**/
EFI_STATUS
EFIAPI
AmtLockUsbKbdSmi (
  IN     EFI_HANDLE     DispatchHandle,
  IN     CONST VOID     *Context,
  IN OUT VOID           *CommBuffer,
  IN OUT UINTN          *CommBufferSize
  )
{
    URP_STRUC   UrpStruc;
    API_FUNC    UsbRtKbcAccessControl;
    
    if (gAmiUsbSmmProtocol == NULL) {
        return EFI_SUCCESS;
    }
    
    // USBAPI_KbcAccessControl is the 17th API_FUNC in UsbApiTable.    
    UrpStruc.ApiData.KbcControlCode = 1;    
    UsbRtKbcAccessControl = (API_FUNC)gAmiUsbSmmProtocol->UsbApiTable[17];
    if (UsbRtKbcAccessControl != NULL) {
        UsbRtKbcAccessControl(&UrpStruc);
    }
    return EFI_SUCCESS;
}

/**
    If the BootOption is set with LockKeyboard, register the
    event with gEfiEventLegacyBootGuid.
    The PiSmmIpl will trigger it to lock keyboard.

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Status


**/
EFI_STATUS
InSmmFunction(
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
#if AMIUSB_SUPPORT == 1
#if USB_DRIVER_MAJOR_VER >= 10    
    EFI_STATUS Status;
    EFI_SMM_BASE2_PROTOCOL          *pEfiSmmBase2Protocol = NULL;
    ALERT_STANDARD_FORMAT_PROTOCOL  *pAlertStandardFormatProtocol = NULL;
    ASF_BOOT_OPTIONS                *pAsfBootOptions = NULL;
    EFI_HANDLE                      SmiHandle = NULL;
    
    Status = pBS->LocateProtocol( &gEfiSmmBase2ProtocolGuid, NULL,
                                  (VOID **) &pEfiSmmBase2Protocol );
    if( !EFI_ERROR(Status) )
    {
        Status = pEfiSmmBase2Protocol->GetSmstLocation(
                    pEfiSmmBase2Protocol, &gSmst2 );
        if( EFI_ERROR(Status) ) return Status;
    }

    Status = pBS->LocateProtocol( &gAlertStandardFormatProtocolGuid, NULL,
                                  (VOID **) &pAlertStandardFormatProtocol );
    if( !EFI_ERROR(Status) )
    {
        Status = pAlertStandardFormatProtocol->GetBootOptions (
                        pAlertStandardFormatProtocol, &pAsfBootOptions) ;
        if( EFI_ERROR(Status) ) return Status;
    }
    
    if( pAsfBootOptions->BootOptions & LOCK_KEYBOARD )
    {
#if USB_RUNTIME_DRIVER_IN_SMM
        Status = gSmst2->SmmLocateProtocol(&gAmiUsbSmmProtocolGuid , NULL,
                                           (VOID **) &gAmiUsbSmmProtocol);
        if(!EFI_ERROR(Status))
        {
            Status = gSmst2->SmiHandlerRegister( &AmtLockUsbKbdSmi,
                                                 &gEfiEventLegacyBootGuid,
                                                 &SmiHandle );
        }
#endif
    }
    return Status;
#endif
#else
    return EFI_NOT_FOUND;
#endif    
}
/**
    Lock USB Keyboard Entrypoint.

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Status

**/
EFI_STATUS
EFIAPI
AmtLockUsbKbdEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandlerEx (ImageHandle, SystemTable, InSmmFunction, NULL);
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
