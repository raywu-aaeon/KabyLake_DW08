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

//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************

/** @file SmBusDxe.c
    SMBUS DXE functions implementation

**/

#include <AmiHobs.h>
#include "SmBusDxeSmm.h"
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/DriverBinding.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>


                                        // (EIP40778)>
//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

CHAR16 *gSmBusDriverName = L"AMI SB SMBus Controller Driver";
CHAR16 *gSmBusControllerName = L"SB SMBus Controller";
EFI_EVENT gEvent;
VOID      *gSmbusConfigReg = NULL;
EFI_COMPONENT_NAME2_PROTOCOL gSmBusDriverNameProtocol = {
    GetSmBusDriverName,
    GetSmBusControllerName,
    "eng"
};

EFI_DRIVER_BINDING_PROTOCOL SmBusDriverBindingProtocol = {
    DriverBindingSupported,
    DriverBindingStart,
    DriverBindingStop,
    1,
    NULL,
    NULL
    };

SMBUS_DXE_PRIVATE *gPrivate;
EFI_HANDLE gControllerHandle = NULL;
// External Declaration(s)

extern EFI_GUID SmBusIdentifierGuid;
/**
    This function configures and installs SMBUS protocol before
    SMBus EFI 1.1 drvier is installed.

    @param VOID

        
    @retval EFI_SUCCESS SMBUS protocol has been installed.
**/

EFI_STATUS SmBusEarlyDxeDriver ( VOID )
{
    EFI_STATUS                      Status;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo = NULL;
	UINT32                          IoBase32;
	UINT16                          Cmd16;


    Status = pBS->AllocatePool( EfiBootServicesData, \
                                sizeof(SMBUS_DXE_PRIVATE), \
                                (VOID **)&gPrivate );
    if (EFI_ERROR(Status)) return Status;

    Status = pBS->AllocatePool( \
                        EfiBootServicesData, \
                        sizeof(EFI_SMBUS_DEVICE_MAP) * MAX_DXE_ARP_DEVICES, \
                        (VOID **)&(gPrivate->SmBusContext.ArpDeviceList) );
    if (EFI_ERROR(Status)) {
        pBS->FreePool( gPrivate );
        return Status;
    }

    gPrivate->SmBusProtocol.Execute   = SmBusDxeExecute;
    gPrivate->SmBusProtocol.ArpDevice = SmBusDxeArpDevice;
    gPrivate->SmBusProtocol.GetArpMap = SmBusDxeGetArpMap;
    gPrivate->SmBusProtocol.Notify    = SmBusDxeNotify;

    gPrivate->NotifyEvent = NULL;
    gPrivate->Identifier = SmBusIdentifierGuid;

    gPrivate->SmBusContext.SmBusWait = MicroSecondDelay;
    gPrivate->SmBusContext.MaxDevices = MAX_DXE_ARP_DEVICES;
    RetrieveHobData( gPrivate );

/* Porting Required*/
    Status = pBS->LocateProtocol( &gEfiPciRootBridgeIoProtocolGuid, \
                                  NULL, \
                                  (VOID **)&PciRootBridgeIo );
    // Update SMBus I/O Base Address
    PciRootBridgeIo->Pci.Read( PciRootBridgeIo, \
                               EfiPciWidthUint32, \
                               EFI_SB_PCI_CFG_ADDRESS(SMBUS_BUS, SMBUS_DEV, SMBUS_FUNC, ICH_SMB_BASE), \
                               1, \
                               &IoBase32 );
    IoBase32 &= 0xfffffff0;
    gPrivate->SmBusContext.SmBusBase = (UINT16)IoBase32;

    // Enable SMBus controller I/O decode.
    PciRootBridgeIo->Pci.Read( PciRootBridgeIo, \
                               EfiPciWidthUint16, \
                               EFI_SB_PCI_CFG_ADDRESS(SMBUS_BUS, SMBUS_DEV, SMBUS_FUNC, ICH_SMB_CMD), \
                               1, \
                               &Cmd16 );
    Cmd16 |= BIT00;
    PciRootBridgeIo->Pci.Write( PciRootBridgeIo, \
                                EfiPciWidthUint16, \
                                EFI_SB_PCI_CFG_ADDRESS(SMBUS_BUS, SMBUS_DEV, SMBUS_FUNC, ICH_SMB_CMD), \
                                1, \
                                &Cmd16 );
/*Porting End */

    Status = pBS->InstallProtocolInterface( &gControllerHandle, \
                                            &gEfiSmbusHcProtocolGuid, \
                                            EFI_NATIVE_INTERFACE,
                                            &gPrivate->SmBusProtocol );

    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "SBSmbusDxe: Install Protocol Interface Failed.\n"));
    }

    return Status;
}

/**
    This callback function is called when a PCI I/O Protocol is
    installed.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval EFI_SUCCESS
**/
VOID SmbusConfigAfterDeviceInstall (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    EFI_STATUS          Status;
    EFI_HANDLE          Handle;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINTN               BufferSize = 20 * sizeof(EFI_HANDLE);
    UINTN               PciSeg;
    UINTN               PciBus;
    UINTN               PciDev;
    UINTN               PciFun;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo = NULL;
    UINT32              Base32;
    UINT16              Cmd16;

    Status = pBS->LocateHandle( ByRegisterNotify, NULL, gSmbusConfigReg, 
                                &BufferSize, &Handle );

    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
        return;
    }

    // Locate PciIo protocol installed on Handle    
    Status = pBS->HandleProtocol( Handle, &gEfiPciIoProtocolGuid, &PciIo );
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
        return;
    }

    // Get PCI Device Bus/Device/Function Numbers
    Status = PciIo->GetLocation(PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
        return;
    }
    if (PciBus == SMBUS_BUS && PciDev == SMBUS_DEV && PciFun == SMBUS_FUNC)
    {
        // Porting Required
        Status = pBS->LocateProtocol( &gEfiPciRootBridgeIoProtocolGuid,
                                      NULL,
                                      (VOID **)&PciRootBridgeIo );
        // Update SMBus I/O Base Address
        PciRootBridgeIo->Pci.Read( PciRootBridgeIo,
                                   EfiPciWidthUint32,
                                   EFI_SB_PCI_CFG_ADDRESS(SMBUS_BUS, SMBUS_DEV, SMBUS_FUNC, ICH_SMB_BASE),
                                   1,
                                   &Base32 );
        Base32 &= 0xfffffff0;
        gPrivate->SmBusContext.SmBusIoBase = (UINT16)Base32;

        // Enable SMBus controller I/O decode.
        PciRootBridgeIo->Pci.Read( PciRootBridgeIo,
                                   EfiPciWidthUint16,
                                   EFI_SB_PCI_CFG_ADDRESS(SMBUS_BUS, SMBUS_DEV, SMBUS_FUNC, ICH_SMB_CMD),
                                   1,
                                   &Cmd16 );
        Cmd16 |= BIT00;
        PciRootBridgeIo->Pci.Write( PciRootBridgeIo,
                                    EfiPciWidthUint16,
                                    EFI_SB_PCI_CFG_ADDRESS(SMBUS_BUS, SMBUS_DEV, SMBUS_FUNC, ICH_SMB_CMD),
                                    1,
                                    &Cmd16 );
        // Porting End
    }

}

                                        // (EIP54149)>

/**
        // (EIP54149)>

    This function installs SMBus DXE protocols for the SMBus
    controller present in the SB.

    @param ImageHandle  - Image handle for the SB component
    @param *SystemTable - Pointer to the system table

    @retval EFI_STATUS

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmBusDxeEntryPoint (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS  Status;

    InitAmiLib(ImageHandle, SystemTable);

    Status = SmBusEarlyDxeDriver();

#if SMBUS_BLINDING_PROTOCOL_SUPPORT
    SmBusDriverBindingProtocol.ImageHandle = ImageHandle;
    SmBusDriverBindingProtocol.DriverBindingHandle = ImageHandle;
    Status = pBS->InstallMultipleProtocolInterfaces( \
                                             &ImageHandle, \
                                             &gEfiDriverBindingProtocolGuid, \
                                             &SmBusDriverBindingProtocol, \
                                                     &gEfiComponentName2ProtocolGuid,
                                                     &gSmBusDriverNameProtocol,
                                             NULL );
#endif
    Status = RegisterProtocolCallback( &gEfiPciIoProtocolGuid, 
                                       SmbusConfigAfterDeviceInstall, 
                                       NULL,
                                       &gEvent,
                                       &gSmbusConfigReg );

    return Status;
}
                                        // <(EIP54149)
                                        // <(EIP40778)
										
/**
    Retrieves a Unicode string that is the user readable name of
    the EFI Driver.

    @param This       - A pointer to the EFI_COMPONENT_NAME_PROTOCOL
        instance.
    @param Language   - A pointer to a three character ISO 639-2 language
        identifier. This is the language of the driver
        name that that the caller is requesting, and it
        must match one of the languages specified in
        SupportedLanguages. The number of languages
        supported by a driver is up to the driver writer.
    @param DriverName - A pointer to the Unicode string to return.
        This Unicode string is the name of the driver
        specified by This in the language specified by
        Language.

    @retval EFI_SUCCES The Unicode string for the Driver specified by
        This and the language specified by Language was
        returned in DriverName.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER DriverName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This does not
        support the language specified by 
        Language.

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS GetSmBusDriverName (
    IN EFI_COMPONENT_NAME2_PROTOCOL *This,
    IN CHAR8                        *Language,
    OUT CHAR16                      **DriverName )
{
    if(!Language || !DriverName) return EFI_INVALID_PARAMETER;
    if (MemCmp(Language, "eng", 3)) return EFI_UNSUPPORTED;
    *DriverName = gSmBusDriverName;
    return EFI_SUCCESS;
}


/**
    Retrieves a Unicode string that is the user readable name of
    the controller that is being managed by an EFI Driver.

    @param This           - A pointer to the EFI_COMPONENT_NAME_PROTOCOL
        instance. 
    @param Controller     - The handle of a controller that the driver
        specified by This is managing. This handle
        specifies the controller whose name is to
        be returned.
    @param ChildHandle    - The handle of the child controller to
        retrieve the name of. This is an optional
        parameter that  may be NULL. It will be NULL
        for device drivers. It will also be NULL for
        a bus drivers that wish to retrieve the name
        of the bus controller.
        It will not be NULL for a bus driver that
        wishes to retrieve the name of a child
        controller. 
    @param Language       - A pointer to a three character ISO 639-2
        language identifier. This is the language of
        the controller name that that the caller is
        requesting, and it must match one of the
        languages specified in SupportedLanguages.
        The number of languages supported by a
        driver is up to the driver writer. 
    @param ControllerName - A pointer to the Unicode string to return.
        This Unicode string is the name of the
        controller specified by ControllerHandle and
        ChildHandle in the language specified by
        Language from the point of view of the
        driver specified by This.

    @retval EFI_SUCCESS The Unicode string for the user readable name
        in the language specified by Language for the
        driver specified by This was returned in
        DriverName.
    @retval EFI_INVALID_PARAMETER ControllerHandle is not a valid
        EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is
        not a valid EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER ControllerName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This is not
        currently managing the controller
        specified by ControllerHandle and
        ChildHandle.
    @retval EFI_UNSUPPORTED The driver specified by This does not
        support the language specified by 
        Language.

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS GetSmBusControllerName (
    IN EFI_COMPONENT_NAME2_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_HANDLE                   ChildHandle OPTIONAL,
    IN CHAR8                        *Language,
    OUT CHAR16                      **ControllerName )
{
    if(!Language || !ControllerName) return EFI_INVALID_PARAMETER;
    if (MemCmp(Language, "eng", 3)) return EFI_UNSUPPORTED;
    *ControllerName = gSmBusControllerName;
    return EFI_SUCCESS;
}

                                        // (EIP40778)>
/**
    SMBUS DXE Driver binding protocol Supported function

    @param This pointer to EFI_DRIVER_BINDING_PROTOCOL structure
    @param ControllerHandle handle of controller to serve
    @param RemainingDevicePath pointer to EFI_DEVICE_PATH_PROTOCOL structure

    @retval EFI_SUCCESS driver supports given controller
    @retval EFI_UNSUPPORTED given controller not supported  

**/
EFI_STATUS  DriverBindingSupported ( 
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    EFI_STATUS          Status;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINT8               PciData[4];

    Status = pBS->OpenProtocol (
                            ControllerHandle,
                            &gEfiPciIoProtocolGuid,
                            (VOID **) &PciIo,
                            This->DriverBindingHandle,
                            ControllerHandle,
                            EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (EFI_ERROR (Status))
        return EFI_UNSUPPORTED;

  //
  // See if this is a PCI Smbus Controller by looking at the Class Code Register
  //
    Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint32,
                            PCI_RID,
                            1,
                            (VOID *)PciData );
    if (EFI_ERROR (Status))
        return EFI_UNSUPPORTED;

    pBS->CloseProtocol (
         ControllerHandle,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle );

    // See if this is the specific PCI SMBus Controller
    return (PciData[3] == PCI_CL_SER_BUS && PciData[2] == PCI_CL_SER_BUS_SCL_SMB) ? EFI_SUCCESS : EFI_UNSUPPORTED;
}

/**
    SMBUS DXE Driver binding protocol Start function

    @param This pointer to EFI_DRIVER_BINDING_PROTOCOL structure
    @param ControllerHandle handle of controller to serve
    @param RemainingDevicePath pointer to EFI_DEVICE_PATH_PROTOCOL structure

    @retval EFI_SUCCESS driver supports given controller
    @retval EFI_UNSUPPORTED given controller not supported  

**/
EFI_STATUS  DriverBindingStart ( 
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    EFI_STATUS Status;
    SMBUS_DXE_PRIVATE *Private;
    static BOOLEAN          EarlyDxeProtocol = TRUE;

    if (EarlyDxeProtocol) {
        Status = pBS->UninstallProtocolInterface( gControllerHandle, \
                                                  &gEfiSmbusHcProtocolGuid, \
                                                  &gPrivate->SmBusProtocol );
        if (Status == EFI_SUCCESS) {
            pBS->FreePool( gPrivate->SmBusContext.ArpDeviceList );
            pBS->FreePool( gPrivate );
        }
        EarlyDxeProtocol = FALSE;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, 
                               sizeof(SMBUS_DXE_PRIVATE),
                               (VOID **)&Private);
    if(EFI_ERROR(Status))
        return Status;

    Status = pBS->AllocatePool(EfiBootServicesData, 
                               sizeof(EFI_SMBUS_DEVICE_MAP) * MAX_DXE_ARP_DEVICES,
                               (VOID **)&(Private->SmBusContext.ArpDeviceList));
    if(EFI_ERROR(Status))
        return Status;

    Status = pBS->OpenProtocol (
                            ControllerHandle,
                            &gEfiPciIoProtocolGuid,
                            (VOID **) &(Private->PciIo),
                            This->DriverBindingHandle,
                            ControllerHandle,
                            EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (EFI_ERROR (Status))
    {
        pBS->FreePool(Private->SmBusContext.ArpDeviceList);
        pBS->FreePool(Private);
        return Status;
    }

    Private->SmBusProtocol.Execute   = SmBusDxeExecute;
    Private->SmBusProtocol.ArpDevice = SmBusDxeArpDevice;
    Private->SmBusProtocol.GetArpMap = SmBusDxeGetArpMap;
    Private->SmBusProtocol.Notify    = SmBusDxeNotify;

    Private->NotifyEvent = NULL;
    Private->Identifier = SmBusIdentifierGuid;

    Private->SmBusContext.SmBusWait = MicroSecondDelay;
    Private->SmBusContext.MaxDevices = MAX_DXE_ARP_DEVICES;
    RetrieveHobData(Private);

//TODO Fill Private->SmBusContext.SmBusBase with value read from PCI device
    // Porting Required
    (Private->PciIo)->Pci.Read( Private->PciIo, 
                                EfiPciIoWidthUint32, 
                                ICH_SMB_BASE, 
                                1, 
                                &Base32 );
    Base32 &= 0xfffffff0;
    Private->SmBusContext.SmBusIoBase = Base32;

    (Private->PciIo)->Pci.Read( Private->PciIo, 
                                EfiPciIoWidthUint32, 
                                ICH_SMB_MBAR0, 
                                1, 
                                &Base32 );
    Base32 &= 0xfffffff0;
    Private->SmBusContext.SmBusMmioBase = Base32;

    Status = (Private->PciIo)->Attributes( Private->PciIo, 
                                           EfiPciIoAttributeOperationSupported,
                                           EFI_PCI_DEVICE_ENABLE, 
                                           &SupportedAttributes );
    Status = (Private->PciIo)->Attributes( Private->PciIo, 
                                           EfiPciIoAttributeOperationEnable, 
                                           SupportedAttributes, 
                                           NULL );

    // Enable SMBus controller I/O decode.
    Status = (Private->PciIo)->Attributes( Private->PciIo, 
                                           EfiPciIoAttributeOperationEnable,
                                           EFI_PCI_IO_ATTRIBUTE_IO, 
                                           NULL );
    // Porting End

    DListInit(&(Private->NotifyList));

    Status = pBS->InstallMultipleProtocolInterfaces(
                    &ControllerHandle,
                    &gEfiSmbusHcProtocolGuid,
					&Private->SmBusProtocol,
                    NULL);
    if (EFI_ERROR (Status))
    {
        pBS->CloseProtocol(
                        ControllerHandle,
                        &gEfiPciIoProtocolGuid,
                        This->DriverBindingHandle,
                        ControllerHandle );
        pBS->FreePool(Private->SmBusContext.ArpDeviceList);
        pBS->FreePool(Private);
    }
    return Status;
}

/**
    SMBUS DXE Driver binding protocol Stop function

    @param This pointer to EFI_DRIVER_BINDING_PROTOCOL structure
    @param ControllerHandle handle of controller to serve
    @param NumberOfChildren number of child devices of controller
    @param ChildHandleBuffer pointer to child devices handles array

    @retval EFI_SUCCESS driver was successfully uninstalled from controller

**/
EFI_STATUS  DriverBindingStop ( 
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   ControllerHandle,
    IN  UINTN                       NumberOfChildren,
    IN  EFI_HANDLE                  *ChildHandleBuffer
)
{
    EFI_STATUS              Status;
    SMBUS_DXE_PRIVATE       *Private;
    EFI_SMBUS_HC_PROTOCOL   *SmBusProtocol;

    Status = pBS->OpenProtocol (
                        ControllerHandle, 
                        &gEfiSmbusHcProtocolGuid,
                        &SmBusProtocol, 
                        This->DriverBindingHandle, 
                        ControllerHandle, 
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL );
    if (EFI_ERROR (Status))
        return EFI_NOT_STARTED;

    pBS->CloseProtocol (
                    ControllerHandle, 
                    &gEfiSmbusHcProtocolGuid,
                    This->DriverBindingHandle, 
                    ControllerHandle );

    Private = (SMBUS_DXE_PRIVATE *) SmBusProtocol;  

    // uninstall the protocol
    Status = pBS->UninstallMultipleProtocolInterfaces ( 
                    ControllerHandle,            
                    &gEfiSmbusHcProtocolGuid,
					&Private->SmBusProtocol,
                    NULL );
    if (EFI_ERROR (Status))
        return Status;

    if(Private->NotifyEvent != 0)
    {
        SMBUS_NOTIFY_LINK *NotifyLink = (SMBUS_NOTIFY_LINK *)(Private->NotifyList.pHead);
        SMBUS_NOTIFY_LINK *DeleteLink;

        pBS->CloseEvent(Private->NotifyEvent);
        while(NotifyLink != 0)
        {
            DeleteLink = NotifyLink;
            NotifyLink = (SMBUS_NOTIFY_LINK *)NotifyLink->Link.pNext;
            pBS->FreePool(DeleteLink);
        }
    }

    pBS->CloseProtocol (
                    ControllerHandle, 
                    &gEfiPciIoProtocolGuid, 
                    This->DriverBindingHandle, 
                    ControllerHandle );
    pBS->FreePool(Private->SmBusContext.ArpDeviceList);
    pBS->FreePool(Private);

    return EFI_SUCCESS;
}

/**
    EFI_SMBUS_HC_PROTOCOL Execute function

    @param This pointer to EFI_SMBUS_HC_PROTOCOL structure
    @param SlaveAddress slave address
    @param Command command
    @param Operation operation
    @param PecCheck parity check flag
    @param Length pointer to size of data buffer
    @param Buffer pointer to data buffer

    @retval EFI_STATUS

**/
EFI_STATUS SmBusDxeExecute (
    IN      EFI_SMBUS_HC_PROTOCOL         *This,
    IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
    IN      EFI_SMBUS_DEVICE_COMMAND      Command,
    IN      EFI_SMBUS_OPERATION           Operation,
    IN      BOOLEAN                       PecCheck,
    IN OUT  UINTN                         *Length,
    IN OUT  VOID                          *Buffer
)
{
    SMBUS_DXE_PRIVATE *Private = (SMBUS_DXE_PRIVATE *)This;

    return Execute(
                &(Private->SmBusContext),
                SlaveAddress,
                Command,
                Operation,
                PecCheck,
                Length,
                Buffer);
}

/**
    This is the main function that handles the SMBus Address
    Resolution Protocol (ARP) requests.

    @param This         - Pointer to the SMBus HC Protocol
    @param ArpAll       - Flag that indicates the type of ARP
        - to be performed
    @param SmbusUdid    - SMBus UDID obtained from the device
    @param SlaveAddress - Address assigned to the SMBus device

    @retval Return Status based on errors that occurred while waiting
        for time to expire.

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmBusDxeArpDevice (
    IN CONST EFI_SMBUS_HC_PROTOCOL            *This,
    IN CONST BOOLEAN                          ArpAll,
    IN CONST EFI_SMBUS_UDID                   *SmbusUdid, OPTIONAL
    IN OUT EFI_SMBUS_DEVICE_ADDRESS     *SlaveAddress OPTIONAL )
{
    return ArpDevice( &(((SMBUS_DXE_PRIVATE *)This)->SmBusContext), \
                      ArpAll, \
                      SmbusUdid, \
                      SlaveAddress );
}

/**
    This function returns the SMBus device address map configured
    for the Smbus devices in the system

    @param This           - Pointer to the SMBus HC Protocol
    @param Length         - Size of the Smbus device map buffer
    @param SmBusDeviceMap - Buffer that contains the SMBus device map

    @retval Return Status based on errors that occurred while waiting
        for time to expire.

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmBusDxeGetArpMap (
    IN CONST EFI_SMBUS_HC_PROTOCOL            *This,
    IN OUT UINTN                        *Length,
    IN OUT EFI_SMBUS_DEVICE_MAP         **SmBusDeviceMap )
{
    return GetArpMap( &(((SMBUS_DXE_PRIVATE *)This)->SmBusContext), \
                      Length, \
                      SmBusDeviceMap );
}

/**
    This function handles the Notify function ability for the
    Smbus devices.

    @param This           - Pointer to the SMBus HC Protocol
    @param SlaveAddress   - Address of the SMBus device
    @param Data           - Data that the host controller detects as 
        sending a message and calls all the 
        registered functions.
    @param NotifyFunction - Function to be invoked

        
    @retval EFI_INVALID_PARAMETER NotifyFunction is NULL
    @retval EFI_SUCCESS NotifyFunction is invoked
        successfully. 

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmBusDxeNotify (
    IN CONST EFI_SMBUS_HC_PROTOCOL            *This,
    IN CONST EFI_SMBUS_DEVICE_ADDRESS         SlaveAddress,
    IN CONST UINTN                            Data,
    IN CONST EFI_SMBUS_NOTIFY_FUNCTION        NotifyFunction )
{
    EFI_STATUS          Status;
    SMBUS_DXE_PRIVATE   *Private = (SMBUS_DXE_PRIVATE *)This;
    SMBUS_NOTIFY_LINK   *NewLink;

    if(NotifyFunction == NULL) return EFI_INVALID_PARAMETER;

    Status = pBS->AllocatePool( EfiBootServicesData, \
                                sizeof(SMBUS_NOTIFY_LINK), \
                                (VOID **)&NewLink );
    if (EFI_ERROR(Status)) return Status;

    NewLink->SlaveAddress = SlaveAddress;
    NewLink->Data = Data;
    NewLink->NotifyFunction = NotifyFunction;

    DListAdd( &(Private->NotifyList), (DLINK *)NewLink );
    if (Private->NotifyList.Size == 1)
        Status = InitializeNotifyPolling( Private );

    return Status;
}

/**
    This function creates a callback event for SMBus notofy
    function.

    @param *Context - Pointer to the SMBus DXE Private structure

        
    @retval EFI_SUCCESS Notify Function is successfully created.

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS InitializeNotifyPolling (
    IN SMBUS_DXE_PRIVATE        *Context )
{
    EFI_STATUS Status;

    Status = pBS->CreateEvent( (EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL), \
                               TPL_CALLBACK, \
                               PollSmBusNotify, \
                               Context, \
                               &Context->NotifyEvent );
    if (EFI_ERROR(Status)) return Status;

    Status = pBS->SetTimer( Context->NotifyEvent, \
                            TimerPeriodic, \
                            ONESECOND );
    return Status;
}

/**
    This function polls all SMBus notify function.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID

    @note  GENERALLY NO PORTING REQUIRED
**/

VOID PollSmBusNotify (
    IN EFI_EVENT                Event,
    IN VOID                     *Context )
{
    EFI_STATUS                  Status;
    EFI_SMBUS_DEVICE_ADDRESS    Address;
    UINTN                       Data;
    SMBUS_DXE_PRIVATE           *Private = (SMBUS_DXE_PRIVATE *)Context;
    SMBUS_NOTIFY_LINK           *NotifyLink;

    NotifyLink = (SMBUS_NOTIFY_LINK *)(Private->NotifyList.pHead);

    Status = CheckNotify( &(Private->SmBusContext), &Address, &Data );
    if (EFI_ERROR(Status)) return;

    while (NotifyLink != NULL) {
        if ((Address.SmbusDeviceAddress == \
                              NotifyLink->SlaveAddress.SmbusDeviceAddress) \
                                              && (Data == NotifyLink->Data) )
            NotifyLink->NotifyFunction(Address, Data);

        NotifyLink = (SMBUS_NOTIFY_LINK *)NotifyLink->Link.pNext;
    }
}

//-------------------------------------------------------------------
// Struct EFI_SMBUS_DEVICE_MAP has one member, that declared as UINTN
// Due to this declaration this struct may have different size if
// compiled in x64 mode - 4 bytes in PEI and 8 bytes in DXE
// So we need mediator structure, to convert from PEI to DXE map, that
// was saved in Hob in PEI phase
//-------------------------------------------------------------------

#pragma pack(1)
typedef struct {
    UINT32  Address;
    EFI_SMBUS_UDID Udid;
} PEI_EFI_SMBUS_DEVICE_MAP;
#pragma pack()

/**
    Function reads device map created in PEI phase

    @param Context pointer to device private data

    @retval VOID

**/
VOID RetrieveHobData (
    IN OUT SMBUS_DXE_PRIVATE *Private
)
{
    AMI_SMBUS_HOB *Hob;
    EFI_STATUS    Status;
    SMBUS_PRIVATE *Context = &(Private->SmBusContext);

    PEI_EFI_SMBUS_DEVICE_MAP *PeiMap;
    UINTN                    i;

    Context->BoardReservedAddressCount = 0;
    Context->BoardReservedAddressList = 0;
    Context->ArpDeviceCount = 0;

    Hob = (AMI_SMBUS_HOB *)GetEfiConfigurationTable(pST, &gEfiHobListGuid);

    if(Hob == NULL)
        return;

    Status = FindNextHobByGuid(&(Private->Identifier), &Hob);
    if(EFI_ERROR(Status))
        return;

    Context->BoardReservedAddressCount = Hob->BoardReservedAddressCount;
    Context->BoardReservedAddressList = (UINT8 *)(UINTN)Hob->BoardReservedAddressList;
    Context->ArpDeviceCount = Hob->ArpDeviceCount;

    PeiMap = (PEI_EFI_SMBUS_DEVICE_MAP *)Hob->ArpDeviceList;
    for(i = 0; i < Hob->ArpDeviceCount; i++)
    {
        Context->ArpDeviceList[i].SmbusDeviceAddress.SmbusDeviceAddress = PeiMap[i].Address;
        Context->ArpDeviceList[i].SmbusDeviceUdid = PeiMap[i].Udid;
    }
/*
    MemCpy(Context->ArpDeviceList, 
           Hob->ArpDeviceList,
           Context->ArpDeviceCount * sizeof(EFI_SMBUS_DEVICE_MAP));
*/
}

//**********************************************************************
//                  Porting functions
//**********************************************************************

/*
VOID SmBusDxeInitialize(
    IN SMBUS_PRIVATE *Context
    )
{
//Porting required - initialize PCI device and fill SmBusBase

}
*/

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
