//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//*****************************************************************************

/** @file SmBusPei.c
    SMBUS driver PEI functions implementation

**/

#include <SmBusPei.h>

/**
    SMBUS driver PEI entry point

    @param FileHandle  - File handle from which the image was loaded
    @param PeiServices - Pointer to the PEI services table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS SmBusPeiEntryPoint (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS          Status;
    SMBUS_PEI_PRIVATE   *Private;

    Status = (*PeiServices)->AllocatePool(
                              PeiServices,
                              sizeof(SMBUS_PEI_PRIVATE),
                              (VOID **)&Private
                              );
    if(EFI_ERROR(Status))
        return Status;

    Status = (*PeiServices)->AllocatePool(
                              PeiServices,
                              sizeof(EFI_SMBUS_DEVICE_MAP) * MAX_PEI_ARP_DEVICES,
                              (VOID **)&(Private->SmBusContext.ArpDeviceList)
                              );
    if(EFI_ERROR(Status))
        return Status;

    Private->SmBusPpi.Execute   = SmBusPeiExecute;
    Private->SmBusPpi.ArpDevice = SmBusPeiArpDevice;
    Private->SmBusPpi.GetArpMap = SmBusPeiGetArpMap;
    Private->SmBusPpi.Notify    = SmBusPeiNotify;

    Private->SmBusPpiDesc.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
//#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x00010000) // [ EIP427935 ]
    Private->SmBusPpiDesc.Guid  = &gEfiPeiSmbus2PpiGuid;
//#else
//    Private->SmBusPpiDesc.Guid  = &gPeiSmbusPpiGuid;
//#endif
    Private->SmBusPpiDesc.Ppi   = &Private->SmBusPpi;

    Private->NotifyDesc.Flags   = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    Private->NotifyDesc.Guid    = &gEfiEndOfPeiSignalPpiGuid;
    Private->NotifyDesc.Notify  = SmBusEndOfPeiCallback;

    Private->SmBusContext.ArpDeviceCount  = 0;
    Private->SmBusContext.MaxDevices      = MAX_PEI_ARP_DEVICES;
    Private->SmBusContext.SmBusWait       = MicroSecondDelay;

//#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x00010000) // [ EIP427935 ]
    Private->SmBusPpi.Identifier = SmBusIdentifierGuid;
//#else
//    Private->Identifier = SmBusIdentifierGuid;
//#endif

    SmBusPeiInitialize (PeiServices, &Private->SmBusContext);

    Status = (*PeiServices)->NotifyPpi(PeiServices, &Private->NotifyDesc);
    if(EFI_ERROR(Status))
        return Status;

    return (*PeiServices)->InstallPpi(PeiServices, &Private->SmBusPpiDesc);
}

/**
    This function is the SMBUS PPI Execute function. This
    function performs the actual SMBUS read/write to the SMBus
    Controller.

    @param *This         - Pointer to the SMBUS PPI structure
    @param SlaveAddress  - Address of the SMBUS device to be used to
        send this command
    @param Command       - Command to be sent to the device
    @param Operation     - SMBus operation to be performed
    @param PecCheck      - Flag indicating the usage of PEC
    @param *Length       - Length of the data in the Buffer (IN/OUT)
    @param *Buffer       - Pointer to the buffer with the data (IN/OUT)

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS SmBusPeiExecute (
    IN CONST EFI_PEI_SMBUS2_PPI   *This,
    IN EFI_SMBUS_DEVICE_ADDRESS   SlaveAddress,
    IN EFI_SMBUS_DEVICE_COMMAND   Command,
    IN EFI_SMBUS_OPERATION        Operation,
    IN BOOLEAN                    PecCheck,
    IN OUT UINTN                  *Length,
    IN OUT VOID                   *Buffer
)
{
    SMBUS_PEI_PRIVATE *Private = (SMBUS_PEI_PRIVATE *)This;

    return Execute(
            &(Private->SmBusContext),
            SlaveAddress,
            Command,
            Operation,
            PecCheck,
            Length,
            Buffer
            );
}

/**
    This functions performs the address resolution for the
    existing SMBus devices using ARP(Address Resolution Protocol)

    @param *This          - Pointer to the SMBus PPI structure
    @param ArpAll         - Flag indicating ARP type - ALL or specific
    @param *SmbusUdid     - SMBus UDID for the device whose Address has
        to be resolved
    @param *SlaveAddress  - Slave address to be assigned to the device

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS SmBusPeiArpDevice (
    IN CONST EFI_PEI_SMBUS2_PPI     *This,
    IN BOOLEAN                      ArpAll,
    IN EFI_SMBUS_UDID               *SmbusUdid, OPTIONAL
    IN OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress OPTIONAL
)
{
    SMBUS_PEI_PRIVATE *Private = (SMBUS_PEI_PRIVATE *)This;

    return ArpDevice(
            &(Private->SmBusContext),
            ArpAll,
            SmbusUdid,
            SlaveAddress
            );
}

/**
    This function returns the ARP map (list of SMBus devices with
    address assigned to them) for the SMBus controller

    @param *This           - Pointer to the SMBus PPI structure
    @param *Length         - Length of the Device map structure(IN & OUT)
    @param *SmBusDeviceMap - Pointer to the buffer where the SMBus
        device map will be filled in

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS SmBusPeiGetArpMap (
    IN CONST EFI_PEI_SMBUS2_PPI   *This,
    IN OUT UINTN                  *Length,
    IN OUT EFI_SMBUS_DEVICE_MAP   **SmbusDeviceMap
)
{
    SMBUS_PEI_PRIVATE *Private = (SMBUS_PEI_PRIVATE *)This;

    return GetArpMap(
            &(Private->SmBusContext),
            Length,
            SmbusDeviceMap
            );
}

/**
    This function allows a PEIM to register for a callback when
    the bus driver detects a state that it needs to probagate to
    other drivers that are registered for a callback

    @param **PeiServices  - Pointer to the PEI services table
    @param *This          - Pointer to the SMBus PPI structure
    @param SlaveAddress   - Address of the SMBus device to be monitored
    @param Data           - Data to be associated with the Notification
    @param NotifyFunction - Function to be invoked when message arrives

    @retval EFI_UNSUPPORTED

**/
EFI_STATUS SmBusPeiNotify (
    IN CONST EFI_PEI_SMBUS2_PPI       *This,
    IN EFI_SMBUS_DEVICE_ADDRESS       SlaveAddress,
    IN UINTN                          Data,
    IN EFI_PEI_SMBUS_NOTIFY2_FUNCTION NotifyFunction
)
{
    return EFI_UNSUPPORTED;
}

/**
    This function will be invoked when end of PEI phase.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - The descriptor for the notification event
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval Return Status based on errors that occurred on CreateHob 
        function.

**/
EFI_STATUS SmBusEndOfPeiCallback (
    IN CONST EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
    IN VOID                       *Ppi
)
{
    SMBUS_PEI_PRIVATE *Private = OUTTER(NotifyDescriptor, NotifyDesc, SMBUS_PEI_PRIVATE);
    UINTN             HobSize;
    AMI_SMBUS_HOB     *Hob;
    EFI_STATUS        Status;

    HobSize = sizeof(AMI_SMBUS_HOB) + Private->SmBusContext.ArpDeviceCount * sizeof(EFI_SMBUS_DEVICE_MAP);
    Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, HobSize, &Hob);
    if(!EFI_ERROR(Status))
    {
        Hob->Header.Name = Private->SmBusPpi.Identifier;
        Hob->BoardReservedAddressCount = Private->SmBusContext.BoardReservedAddressCount;
        Hob->BoardReservedAddressList = (UINT32) (Private->SmBusContext.BoardReservedAddressList);
        Hob->ArpDeviceCount = Private->SmBusContext.ArpDeviceCount;
        CopyMem (
          Hob->ArpDeviceList,
          Private->SmBusContext.ArpDeviceList,
          Private->SmBusContext.ArpDeviceCount * sizeof(EFI_SMBUS_DEVICE_MAP)
          );
    }
    return Status;
}

//**********************************************************************
//                  Porting functions
//**********************************************************************

/**
    This function initializes SMBUS PCI device and fills device context

    @param **PeiServices - Pointer to the PEI Services table
    @param *Context      - Pointer to the SMBus private structure.

    @retval VOID

    @note  Porting required

**/
VOID SmBusPeiInitialize(
    IN CONST EFI_PEI_SERVICES     **PeiServices,
    IN OUT SMBUS_PRIVATE          *Context
)
{
//Porting required - initialize PCI device and fill SmBusBase

    // Locate PCI CFG and CPU I/O PPIs

//and Board reserved addresses
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
