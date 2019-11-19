//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************


/** @file EcPs2Kbd.c
    This file contains code necessary to install the EC PS2 devices
    for EFI, ACPI and DOS.

**/

//---------------------------------------------------------------------------

#include "Token.h"
#include "EcPs2Kbd.h"

//---------------------------------------------------------------------------

EFI_SIO_PROTOCOL KbdEfiSio;
EFI_SIO_PROTOCOL MouseEfiSio;

#define BDS_CONNECT_DRIVERS_PROTOCOL_GUID \
    { 0x3aa83745, 0x9454, 0x4f7a, 0xa7, 0xc0, 0x90, 0xdb, 0xd0, 0x2f, 0xab, 0x8e }

EFI_GUID  gBdsConnectDriversProtocolGuid = BDS_CONNECT_DRIVERS_PROTOCOL_GUID;

#define EC_KBC_DATA_PORT	0x60
#define EC_KBC_CMD_PORT		0x64

static PS2_KBD_DEVICE_PATH  mEcPs2KbdDevicePath[1][1] = {
    {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
        (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
        EISA_PNP_ID(0x303),
        0,
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        END_DEVICE_PATH_LENGTH,
        0
    }
};
  
static PS2_KBD_DEVICE_PATH  mEcPs2MsDevicePath[1][1] = {
    {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
        (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
        EISA_PNP_ID(0xF03),
        0,
        END_DEVICE_PATH_TYPE,
        END_ENTIRE_DEVICE_PATH_SUBTYPE,
        END_DEVICE_PATH_LENGTH,
        0
    }
};

EFI_EVENT   gEcPs2DeviceEvent;
VOID        *gEcPs2DeviceReg;

/**
    Install DevicePath protocol for the EC Ps2 devices

    @param Event   - The event that triggered this notification function
    @param Context - Pointer to the notification functions context

    @retval VOID
    
**/

VOID 
EFIAPI 
InstallEcPs2Devices (
    EFI_EVENT   Event,
    VOID        *Context
)
{
    EFI_DEVICE_PATH_PROTOCOL    *KbTmpDp = NULL;
    EFI_DEVICE_PATH_PROTOCOL    *MsTmpDp = NULL;
    ACPI_HID_DEVICE_PATH        RbDp;
    PCI_DEVICE_PATH             PciDp;
    PS2KBD_DEV                  *PrivateData;
    EFI_STATUS                  Status;
    

    // Get root bridge device path

    RbDp.Header.Type = ACPI_DEVICE_PATH;
    RbDp.Header.SubType = ACPI_DP;
    SET_NODE_LENGTH(&RbDp.Header, ACPI_DEVICE_PATH_LENGTH);
    RbDp.HID = EISA_PNP_ID(0x0A03);
    RbDp.UID = 0;
    KbTmpDp = DPAddNode(KbTmpDp, &RbDp.Header);
    MsTmpDp = DPAddNode(MsTmpDp, &RbDp.Header);

    // Get PCI device path

    PciDp.Header.SubType = HW_PCI_DP;
    PciDp.Header.Type = HARDWARE_DEVICE_PATH;
    SET_NODE_LENGTH(&PciDp.Header, HW_PCI_DEVICE_PATH_LENGTH);
    PciDp.Function = SB_FUN_NUMBER;
    PciDp.Device = SB_DEV_NUMBER;
    KbTmpDp = DPAddNode(KbTmpDp, &PciDp.Header);
    MsTmpDp = DPAddNode(MsTmpDp, &PciDp.Header);

    // Install PS2 Keyboard Device Path

    Status = pBS->AllocatePool (EfiBootServicesData, sizeof (PS2KBD_DEV), \
                                (VOID**)&PrivateData);
    
    ASSERT (!EFI_ERROR (Status));

    KbTmpDp = DPAddNode(KbTmpDp, (EFI_DEVICE_PATH_PROTOCOL *) \
                                                &mEcPs2KbdDevicePath [0][0]);
    PrivateData->DevicePath = KbTmpDp;
    PrivateData->Handle = NULL;

    Status = pBS->InstallProtocolInterface (&PrivateData->Handle, \
                        &gEfiDevicePathProtocolGuid, EFI_NATIVE_INTERFACE, \
                        PrivateData->DevicePath);

    if (EFI_ERROR (Status)) {
        pBS->FreePool (PrivateData);
        return ;
    }
    
    KbdEfiSio.RegisterAccess     = KbdEfiSioRegisterAccess;
    KbdEfiSio.GetResources       = KbdEfiSioGetResources;
    KbdEfiSio.SetResources       = KbdEfiSioSetResources;
    KbdEfiSio.PossibleResources  = KbdEfiSioPossibleResources;
    KbdEfiSio.Modify             = KbdEfiSioModify;
    
    //
    //Install EFI_SIO_PROTOCOL for PS2 Keyboard device
    //
    Status = pBS->InstallProtocolInterface (&PrivateData->Handle, \
                        &gEfiSioProtocolGuid, EFI_NATIVE_INTERFACE, \
                        &KbdEfiSio);
    if(EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return;
    }

    DEBUG ((EFI_D_INFO,"\r \nInstallProtocol of EfiSioProtocol Status:%r for PS2 Kbd",Status));

    // Install PS2 Mouse Device Path

    Status = pBS->AllocatePool (EfiBootServicesData, sizeof (PS2KBD_DEV), \
                        (VOID**)&PrivateData);

    ASSERT (!EFI_ERROR (Status));

    MsTmpDp = DPAddNode(MsTmpDp, (EFI_DEVICE_PATH_PROTOCOL *) \
                                                 &mEcPs2MsDevicePath [0][0]);
    PrivateData->DevicePath = MsTmpDp;
    PrivateData->Handle = NULL;

    Status = pBS->InstallProtocolInterface (&PrivateData->Handle, \
                    &gEfiDevicePathProtocolGuid, EFI_NATIVE_INTERFACE, \
                    PrivateData->DevicePath);

    if (EFI_ERROR (Status)) {
        pBS->FreePool (PrivateData);
        return ;
    }

    MouseEfiSio.RegisterAccess     = MouseEfiSioRegisterAccess;
    MouseEfiSio.GetResources       = MouseEfiSioGetResources;
    MouseEfiSio.SetResources       = MouseEfiSioSetResources;
    MouseEfiSio.PossibleResources  = MouseEfiSioPossibleResources;
    MouseEfiSio.Modify             = MouseEfiSioModify;
    
    //
    //Install EFI_SIO_PROTOCOL for PS2 Mouse device
    //
    Status = pBS->InstallProtocolInterface (&PrivateData->Handle, \
                    &gEfiSioProtocolGuid, EFI_NATIVE_INTERFACE, \
                    &MouseEfiSio
                    );
    if (EFI_ERROR (Status)) {
        ASSERT_EFI_ERROR(Status);
        return;
    }

    DEBUG ((EFI_D_INFO,"\r \n InstallProtocol of EfiSioProtocol Status:%r for PS2 Mouse",Status));

    //
    //Kill the Event
    //
    pBS->CloseEvent(Event);

    return;
}

/**
    This function installs the Device Path for the EC Ps2.

    @param  Image handle
    @param  Pointer to the system table

    @retval EFI_STATUS - Returned from PCI read call

**/

EFI_STATUS
EFIAPI
EcPs2KbdEntry (
    IN  EFI_HANDLE       ImageHandle,
    IN  EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                  Status;
    EFI_PHYSICAL_ADDRESS        IoAddress;
    DXE_SERVICES                *DxeSvcTbl;
    UINT8                       IoAllign;
    UINT8                       IoLength;
    UINT16                      AvailableIsaIrq=0;
    
    InitAmiLib(ImageHandle,SystemTable);
    DxeSvcTbl=(DXE_SERVICES*)GetEfiConfigurationTable( SystemTable, \
                                                &gEfiDxeServicesTableGuid);

    IoAddress = EC_KBC_DATA_PORT;
    IoLength = 1;
    IoAllign=0;

    // Add KBC Data Port to GCD

    Status = DxeSvcTbl->AddIoSpace (
                     EfiGcdIoTypeIo,
                     IoAddress,
                     IoLength
                     );
    if(EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    // Allocate the KBC Data port from GCD

    Status = DxeSvcTbl->AllocateIoSpace(EfiGcdAllocateAddress,
                                EfiGcdIoTypeIo,
                                IoAllign,
                                IoLength,
                                &IoAddress,
                                ImageHandle,
                                NULL);
    if(EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    
    // Add KBC Command Port to GCD

    IoAddress = EC_KBC_CMD_PORT;
    IoLength = 1;
    IoAllign=0;

    Status = DxeSvcTbl->AddIoSpace (
                     EfiGcdIoTypeIo,
                     IoAddress,
                     IoLength
                     );
    
    if(EFI_ERROR(Status)) {
        DxeSvcTbl->FreeIoSpace(EC_KBC_DATA_PORT,IoLength);
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    // Allocate the KBC Data port from GCD

    Status = DxeSvcTbl->AllocateIoSpace(
                                EfiGcdAllocateAddress,
                                EfiGcdIoTypeIo,
                                IoAllign,
                                IoLength,
                                &IoAddress,
                                ImageHandle,
                                NULL
                                );
    if(EFI_ERROR(Status)) {
        DxeSvcTbl->FreeIoSpace(EC_KBC_DATA_PORT,IoLength);
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    Status = RegisterProtocolCallback ( 
                                        &gBdsConnectDriversProtocolGuid, \
                                        InstallEcPs2Devices, \
                                        NULL, \
                                        &gEcPs2DeviceEvent, \
                                        &gEcPs2DeviceReg
                                        );

    if(!EFI_ERROR(Status)) {
        // Consume IRQ1, so that it won't be allocated to other devices
        Status = AmiIsaIrqMask(&AvailableIsaIrq, TRUE);
        if(Status==EFI_NOT_FOUND){
                AvailableIsaIrq=ISA_IRQ_MASK;
        }
        AvailableIsaIrq |= 0x2;
        Status = AmiIsaIrqMask(&AvailableIsaIrq, FALSE);
        ASSERT_EFI_ERROR( Status );
        return Status;
    }
    DxeSvcTbl->FreeIoSpace(EC_KBC_CMD_PORT,IoLength);
    DxeSvcTbl->FreeIoSpace(EC_KBC_DATA_PORT,IoLength); 
    ASSERT_EFI_ERROR(Status);
    return Status;
}

//
// EFI SIO protocol API definition
//

/**
    Get the Keyboard Device Register details

    @param  This 
    @param  Write 
    @param  ExitCfgMode 
    @param  Register 
    @param  Value 

    @retval EFI_STATUS - This functionality is not supported for  KBD device

**/
EFI_STATUS
EFIAPI
KbdEfiSioRegisterAccess (
    IN  CONST  EFI_SIO_PROTOCOL  *This,
    IN  BOOLEAN           Write,
    IN  BOOLEAN           ExitCfgMode,
    IN  UINT8             Register,
    IN  OUT UINT8         *Value
)
{
    return EFI_UNSUPPORTED;
}

/**
    Get the Keyboard Device current resource

    @param  This 
    @param  EfiResourceList 

    @retval EFI_STATUS - Returns the Keyboard device list of the current resources

**/
EFI_STATUS
EFIAPI
KbdEfiSioGetResources (
    IN  CONST EFI_SIO_PROTOCOL            *This,
    OUT ACPI_RESOURCE_HEADER_PTR          *EfiResourceList
)

{
    VOID                 *EfiSioRes = NULL;
    UINT8                *TempRes = NULL;
    UINT32               Size = 0;
    ASLR_FixedIO         KbdCommandReg;
    ASLR_FixedIO         KbdDataReg;
    ASLR_IRQNoFlags      KbdIrq;

    if (!This || !EfiResourceList)
        return EFI_INVALID_PARAMETER;

    //  KBD Device Base address Resource for Data Register
    KbdDataReg.Hdr.Length = sizeof(ASLR_FixedIO) - sizeof(ASLRF_S_HDR);
    KbdDataReg.Hdr.Name   = ASLV_RT_FixedIO;
    KbdDataReg.Hdr.Type   = ASLV_SMALL_RES;
    KbdDataReg._BAS       = 60;   
    KbdDataReg._LEN       = 0x8; 
    Size += sizeof(ASLR_FixedIO);
    
    
    //  KBD Device Base address Resource for Command Register
    KbdCommandReg.Hdr.Length = sizeof(ASLR_FixedIO) - sizeof(ASLRF_S_HDR);
    KbdCommandReg.Hdr.Name   = ASLV_RT_FixedIO;
    KbdCommandReg.Hdr.Type   = ASLV_SMALL_RES;
    KbdCommandReg._BAS       = 64;   
    KbdCommandReg._LEN       = 0x8; 
    Size += sizeof(ASLR_FixedIO);

    //  Device IRQ details
    KbdIrq.Hdr.Length = sizeof(ASLR_IRQNoFlags) - sizeof(ASLRF_S_HDR);
    KbdIrq.Hdr.Name   = ASLV_RT_IRQ;
    KbdIrq.Hdr.Type   = ASLV_SMALL_RES;
    KbdIrq._INT       = 1; 
    Size += sizeof(ASLR_IRQNoFlags);

    EfiSioRes =  TempRes = Malloc(Size + sizeof(ASLR_EndTag));

    if(!EfiSioRes)
        return EFI_OUT_OF_RESOURCES;

    MemCpy(TempRes, &KbdDataReg, sizeof(ASLR_FixedIO));
    TempRes += sizeof(ASLR_FixedIO);

    MemCpy(TempRes, &KbdCommandReg, sizeof(ASLR_FixedIO));
    TempRes += sizeof(ASLR_FixedIO);

    MemCpy(TempRes, &KbdIrq, sizeof(ASLR_IRQNoFlags));
    TempRes += sizeof(ASLR_IRQNoFlags);

    ((ASLR_EndTag*)TempRes)->Hdr.HDR = ASLV_END_TAG_HDR;
    ((ASLR_EndTag*)TempRes)->Chsum = 0;

    EfiResourceList->SmallHeader = EfiSioRes;

    return EFI_SUCCESS;
}

/**
    Sets the  Keyboard Device current resource

    @param  This 
    @param  ResourceList 

    @retval EFI_STATUS - This functionality is not supported for  KBD device

**/

EFI_STATUS
EFIAPI
KbdEfiSioSetResources (
    IN  CONST  EFI_SIO_PROTOCOL     *This,
    IN  ACPI_RESOURCE_HEADER_PTR    ResourceList
)
{
    return EFI_UNSUPPORTED;
}

/**
    Provides combination of resources that can be used by the device

    @param  This 
    @param  ResourceCollection 

    @retval EFI_STATUS - This functionality is not supported for  KBD device

**/

EFI_STATUS
EFIAPI
KbdEfiSioPossibleResources (
    IN  CONST EFI_SIO_PROTOCOL         *This,
    OUT ACPI_RESOURCE_HEADER_PTR       *ResourceCollection
)
{
    return EFI_UNSUPPORTED;
}

/**
    Interface for multiple registers programming with single call

    @param  This 
    @param  Command 
    @param  NumberOfCommands 

    @retval EFI_STATUS - This functionality is not supported for KBD device

**/

EFI_STATUS
EFIAPI
KbdEfiSioModify (
    IN  CONST EFI_SIO_PROTOCOL         *This,
    IN  CONST EFI_SIO_REGISTER_MODIFY  *Command,
    IN  UINTN                          NumberOfCommands
)
{
    return EFI_UNSUPPORTED;
}

/**
    Get the  Mouse Device Register details

    @param  This 
    @param  Write 
    @param  ExitCfgMode 
    @param  Register 
    @param  Value 

    @retval EFI_STATUS - This functionality is not supported for  Mouse device

**/

EFI_STATUS
EFIAPI
MouseEfiSioRegisterAccess (
    IN  CONST  EFI_SIO_PROTOCOL  *This,
    IN  BOOLEAN                  Write,
    IN  BOOLEAN                  ExitCfgMode,
    IN  UINT8                    Register,
    IN  OUT UINT8                *Value
)
{
    return EFI_UNSUPPORTED;
}

/**
    Get the  Mouse Device current resource

    @param  This 
    @param  EfiResourceList 

    @retval EFI_STATUS - Returns the  Mouse device list of the current resources

**/

EFI_STATUS
EFIAPI
MouseEfiSioGetResources (
    IN  CONST EFI_SIO_PROTOCOL      *This,
    OUT ACPI_RESOURCE_HEADER_PTR    *EfiResourceList
)

{
    VOID                 *EfiSioRes = NULL;
    UINT8                *TempRes = NULL;
    UINT32               Size = 0;
    ASLR_FixedIO         MouseCommandReg;
    ASLR_FixedIO         MouseDataReg;
    ASLR_IRQNoFlags      MouseIrq;


    if (!This || !EfiResourceList)
        return EFI_INVALID_PARAMETER;


    //  PS2 Mouse Device Data Register Base address Resource
    MouseDataReg.Hdr.Length = sizeof(ASLR_FixedIO) - sizeof(ASLRF_S_HDR);
    MouseDataReg.Hdr.Name   = ASLV_RT_FixedIO;
    MouseDataReg.Hdr.Type   = ASLV_SMALL_RES;
    MouseDataReg._BAS       = 60;   
    MouseDataReg._LEN       = 0x8; 
    Size += sizeof(ASLR_FixedIO);

    //  PS2 Mouse Device Command Register Base address Resource
    MouseCommandReg.Hdr.Length = sizeof(ASLR_FixedIO) - sizeof(ASLRF_S_HDR);
    MouseCommandReg.Hdr.Name   = ASLV_RT_FixedIO;
    MouseCommandReg.Hdr.Type   = ASLV_SMALL_RES;
    MouseCommandReg._BAS       = 64;   
    MouseCommandReg._LEN       = 0x8; 
    Size += sizeof(ASLR_FixedIO);

    //  Device IRQ details
    MouseIrq.Hdr.Length = sizeof(ASLR_IRQNoFlags) - sizeof(ASLRF_S_HDR);
    MouseIrq.Hdr.Name   = ASLV_RT_IRQ;
    MouseIrq.Hdr.Type   = ASLV_SMALL_RES;
    MouseIrq._INT       = 12; 
    Size += sizeof(ASLR_IRQNoFlags);

    EfiSioRes =  TempRes = Malloc(Size + sizeof(ASLR_EndTag));

    if(!EfiSioRes)
        return EFI_OUT_OF_RESOURCES;

    MemCpy(TempRes, &MouseDataReg, sizeof(ASLR_FixedIO));
    TempRes += sizeof(ASLR_FixedIO);
    
    MemCpy(TempRes, &MouseCommandReg, sizeof(ASLR_FixedIO));
    TempRes += sizeof(ASLR_FixedIO);

    MemCpy(TempRes, &MouseIrq, sizeof(ASLR_IRQNoFlags));
    TempRes += sizeof(ASLR_IRQNoFlags);

    ((ASLR_EndTag*)TempRes)->Hdr.HDR = ASLV_END_TAG_HDR;
    ((ASLR_EndTag*)TempRes)->Chsum = 0;

    EfiResourceList->SmallHeader = EfiSioRes;

    return EFI_SUCCESS;
}

/**
    Sets the  Mouse Device current resource

    @param  This 
    @param  ResourceList 

    @retval EFI_STATUS - This functionality is not supported for  Mouse device

**/

EFI_STATUS
EFIAPI
MouseEfiSioSetResources (
    IN  CONST  EFI_SIO_PROTOCOL  *This,
    IN  ACPI_RESOURCE_HEADER_PTR ResourceList
)
{
    return EFI_UNSUPPORTED;
}

/**
    Provides combination of resources that can be used by the device

    @param  This 
    @param  ResourceCollection 

    @retval EFI_STATUS - This functionality is not supported for  Mouse device

**/

EFI_STATUS
EFIAPI
MouseEfiSioPossibleResources (
    IN  CONST EFI_SIO_PROTOCOL    *This,
    OUT ACPI_RESOURCE_HEADER_PTR  *ResourceCollection
)
{
    return EFI_UNSUPPORTED;
}

/**
    Interface for multiple registers programming with single call

    @param  This 
    @param  Command 
    @param  NumberOfCommands 

    @retval EFI_STATUS - This functionality is not supported for  Mouse device

**/
EFI_STATUS
EFIAPI
MouseEfiSioModify (
    IN  CONST EFI_SIO_PROTOCOL         *This,
    IN  CONST EFI_SIO_REGISTER_MODIFY  *Command,
    IN  UINTN                          NumberOfCommands
)
{
    return EFI_UNSUPPORTED;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
