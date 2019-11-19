//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file EcPs2Kbd.h
    defines needed for EcPs2Kbd.

**/

//---------------------------------------------------------------------------

#include <Protocol/DevicePath.h>
#include <AmiDxeLib.h>
#include <Protocol/SuperIo.h>
#include <AcpiRes.h>

//---------------------------------------------------------------------------

typedef struct {
	ACPI_HID_DEVICE_PATH		AcpiDevicePath;
	EFI_DEVICE_PATH_PROTOCOL	EndDevicePath;
} PS2_KBD_DEVICE_PATH;


typedef struct _PS2KBD_DEV {
	EFI_DEVICE_PATH_PROTOCOL	*DevicePath;
	EFI_HANDLE					Handle;
} PS2KBD_DEV;

//
// EFI SIO protocol API for Mouse
//

EFI_STATUS
EFIAPI 
MouseEfiSioRegisterAccess (
    IN  CONST  EFI_SIO_PROTOCOL  *This,
    IN  BOOLEAN                  Write,
    IN  BOOLEAN                  ExitCfgMode,
    IN  UINT8                    Register,
    IN  OUT UINT8                *Value
);

EFI_STATUS
EFIAPI 
MouseEfiSioGetResources ( 
    IN  CONST EFI_SIO_PROTOCOL    *This,
    OUT ACPI_RESOURCE_HEADER_PTR  *ResourceList
);

EFI_STATUS 
EFIAPI
MouseEfiSioSetResources (
    IN  CONST  EFI_SIO_PROTOCOL  *This,
    IN  ACPI_RESOURCE_HEADER_PTR ResourceList
);

EFI_STATUS 
EFIAPI
MouseEfiSioPossibleResources (
    IN  CONST EFI_SIO_PROTOCOL   *This,
    OUT ACPI_RESOURCE_HEADER_PTR *ResourceCollection
);

EFI_STATUS 
EFIAPI
MouseEfiSioModify (
    IN  CONST EFI_SIO_PROTOCOL         *This,
    IN  CONST EFI_SIO_REGISTER_MODIFY  *Command,
    IN  UINTN                          NumberOfCommands
);


//
// EFI SIO protocol API for KBD
//

EFI_STATUS
EFIAPI 
KbdEfiSioRegisterAccess (
    IN  CONST  EFI_SIO_PROTOCOL  *This,
    IN  BOOLEAN                  Write,
    IN  BOOLEAN                  ExitCfgMode,
    IN  UINT8                    Register,
    IN  OUT UINT8                *Value
);

EFI_STATUS
EFIAPI 
KbdEfiSioGetResources ( 
    IN  CONST EFI_SIO_PROTOCOL    *This,
    OUT ACPI_RESOURCE_HEADER_PTR  *ResourceList
);

EFI_STATUS 
EFIAPI
KbdEfiSioSetResources (
    IN  CONST  EFI_SIO_PROTOCOL  *This,
    IN  ACPI_RESOURCE_HEADER_PTR ResourceList
);

EFI_STATUS 
EFIAPI
KbdEfiSioPossibleResources (
    IN  CONST EFI_SIO_PROTOCOL    *This,
    OUT ACPI_RESOURCE_HEADER_PTR  *ResourceCollection
);

EFI_STATUS 
EFIAPI
KbdEfiSioModify (
    IN  CONST EFI_SIO_PROTOCOL         *This,
    IN  CONST EFI_SIO_REGISTER_MODIFY  *Command,
    IN  UINTN                          NumberOfCommands
);


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
