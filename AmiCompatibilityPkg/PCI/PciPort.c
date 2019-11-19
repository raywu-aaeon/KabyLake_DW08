//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  	PCI Porting Protocol for compatibility with Aptio 4.x
*/
 
#include <Efi.h>
#include <Dxe.h>
#include <Token.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <PciPort.h>
#include <PciPortElink.h>
#include <Protocol/AmiPciCompatibility.h>

//----------------------------------------------------------------------
//Define Variables == TOKENS to be able to use binary
// See token help for details
//----------------------------------------------------------------------
EFI_HANDLE							gAmiPciPortHandle=NULL;
AMI_PCI_PORT_COMPATIBILITY_PROTOCOL *gAmiPciPort=NULL;

//To avoid compilation error define STRUCT delivered trough eLink as extern function.
#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) Callback
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_PROGRAM_DEVICE_CALLBACK_LIST EndOfList;
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_SKIP_DEVICE_CALLBACK_LIST EndOfList;
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_DEVICE_SET_ASPM_CALLBACK_LIST EndOfList;
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_DEVICE_SET_LNK_SPEED_CALLBACK_LIST EndOfList;
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_DEVICE_ATTRIBUTE_CALLBACK_LIST EndOfList;
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_DEVICE_OUT_OF_RESOURCES_CALLBACK_LIST EndOfList;
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_DEVICE_GET_OPT_ROM_CALLBACK_LIST EndOfList;
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_CHECK_BAD_PCIE_DEVICE_CALLBACK_LIST EndOfList;
extern OEM_PCI_PROGRAM_DEVICE_FUNCTION  OEM_PCI_QUERY_PCI_DEVICE_CALLBACK_LIST EndOfList;
#undef OEM_PCI_DEVICE_CALLBACK


#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemSkipPciDevice[] =
    { OEM_PCI_SKIP_DEVICE_CALLBACK_LIST { 0, 0, NULL } };


/**
    Porting function called for all PCI Devices right before enumeration about to start,
    and give a possibility to remove this device from Enumeration by the PCI Bus Driver.
  
   @param Pointer to the PCI Bus Driver Private data buffer. 
    
   @retval	 EFI_STATUS     
    EFI_SUCCESS     - SKIP this device, do not touch PCI Command register.
    EFI_UNSUPPORTED - DON'T SKIP this device do complete enumeration as usual.
    
   @note
    If device decodes or requires some resources to function properly, 
    allocating and and reporting this resources to GCD is a total responsibility 
    of this hook. If this FUNCTION RETURNS EFI_SUCCESS PciBus Driver assumes that 
    RESOURCES ARE ALLOCATED AND REPORTED TO THE GCD DXE SERVICES.
    Reporting resources to GCD Services is very important to avoid same resource range
    been allocated twice.
    If device you want to skip is PCI2PCI or PCI2CRD bus bridge 1 bus will be allocated 
**/
EFI_STATUS EFIAPI PciPortSkipThisDevice(PCI_DEV_INFO *Device){
    EFI_STATUS      Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK *List = OemSkipPciDevice;
    EFI_STATUS      retStatus=EFI_UNSUPPORTED;
//-------------------
    while(List->Callback)
    {
        OEM_PCI_PROGRAM_DEVICE_FUNCTION *CallBack=(OEM_PCI_PROGRAM_DEVICE_FUNCTION*)List->Callback;
    //--------------------------------
        if((List->VendorId == Device->DevVenId.VenId && List->DeviceId == Device->DevVenId.DevId)||
            (List->VendorId == 0 && List->DeviceId == 0))
        {
            DEBUG((DEBUG_VERBOSE, "\n  PortA4.x: OEM Skip Device Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
	        Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
            Device->DevVenId.VenId, Device->DevVenId.DevId));

            Status = CallBack(Device);
            DEBUG((DEBUG_VERBOSE, " %r.\n", Status));
            
            if( EFI_ERROR(Status) && (Status!=EFI_UNSUPPORTED))return Status;
            else if(Status==EFI_SUCCESS)retStatus=Status;
        }

        List++;
    }
    
    return retStatus;
}


#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemProgramPciDevice[] =
    { OEM_PCI_PROGRAM_DEVICE_CALLBACK_LIST { 0, 0, NULL } };

/**
    Porting function called for all PCI Devices right after PCI_IO_PROTOCOL instance
    was installed for this PCI Device. It gives a possibility to do some additional 
    device programming if needed.
  
   @param Pointer to the PCI Bus Driver Private data buffer. 
    
   @retval   EFI_STATUS     
    EFI_SUCCESS     - this device was programmed successfully.
    EFI_UNSUPPORTED - this device do not have this porting hook implemented.
    ERROR_STATUS    - actual ERROR code of the unsuccessful operation.
    
**/
EFI_STATUS EFIAPI PciPortOemProgDevice(PCI_DEV_INFO *Device){
    EFI_STATUS                      Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK *List = OemProgramPciDevice;
//-------------------
    while(List->Callback)
    {
        OEM_PCI_PROGRAM_DEVICE_FUNCTION *CallBack=(OEM_PCI_PROGRAM_DEVICE_FUNCTION*)List->Callback;
    //--------------------------------
        if((List->VendorId == Device->DevVenId.VenId && List->DeviceId == Device->DevVenId.DevId)||
            (List->VendorId == 0 && List->DeviceId == 0))
        {
            DEBUG((DEBUG_VERBOSE, "\n  PortA4.x: OEM Programm Device Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
	        Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
            Device->DevVenId.VenId, Device->DevVenId.DevId));

            Status = CallBack(Device);
            DEBUG((DEBUG_VERBOSE, " %r.\n", Status));
            if( EFI_ERROR(Status) )
                return Status;
        }
        List++;
    }

    return Status;
}

//To avoid compilation error define Srtuct delivered trough eLink as extern function.
#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemSetAspmPciDevice[] =
    { OEM_PCI_DEVICE_SET_ASPM_CALLBACK_LIST { 0, 0, NULL } };


/**
    Porting function called for all PCI Express capable Devices. 
    It Suppose to be called 2 times for UpStream and DownStream component of a LINK.

    This function allow to override ASPM settings of the link.
  
   @param Pointer to the PCI Bus Driver Private data buffer. 
   @param VOID Pointer to the ASPM Mode settings of the link.
    
   @retval   EFI_STATUS     
    EFI_SUCCESS     - this device was programmed successfully.
    EFI_UNSUPPORTED - this device do not have this porting hook implemented.
    ERROR_STATUS    - actual ERROR code of the unsuccessful operation.
    
**/    
EFI_STATUS EFIAPI PciPortOemSetAspm(PCI_DEV_INFO *Device, VOID *AspmMode){
    EFI_STATUS                          Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK     *List = OemSetAspmPciDevice;
//-------------------
    while(List->Callback)
    {
        OEM_PCI_DEVICE_SET_ASPM_FUNCTION *CallBack =(OEM_PCI_DEVICE_SET_ASPM_FUNCTION*)List->Callback; 
    //--------------------------------

        if((List->VendorId == Device->DevVenId.VenId && List->DeviceId == Device->DevVenId.DevId)||
            (List->VendorId == 0 && List->DeviceId == 0))
        {
            DEBUG((DEBUG_VERBOSE, "\n  PortA4.x: OEM Device Set ASPM Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
	        Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
            Device->DevVenId.VenId, Device->DevVenId.DevId));

            Status = CallBack(Device, AspmMode);
            DEBUG((DEBUG_VERBOSE, " %r.\n", Status));
        	if( EFI_ERROR(Status))
                return Status;
        }
        List++;
    }

    return Status;
}

//To avoid compilation error define Srtuct delivered trough eLink as extern function.
#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemSetLnkSpeedPciDevice[] =
    { OEM_PCI_DEVICE_SET_LNK_SPEED_CALLBACK_LIST { 0, 0, NULL } };

/**
    Porting function called for all PCI Express capable Devices. 
    It Suppose to be called 2 times for UpStream and DownStream component of a LINK.

    This function allow to override Link Speed settings of the link.
  
   @param Pointer to the PCI Bus Driver Private data buffer. 
   @param Pointer to the proposed Link Speed Settings.
   @param Supported Link speed. 
    
   @retval   EFI_STATUS     
    EFI_SUCCESS     - this device was programmed successfully.
    EFI_UNSUPPORTED - this device do not have this porting hook implemented.
    ERROR_STATUS    - actual ERROR code of the unsuccessful operation.
    
**/    
EFI_STATUS EFIAPI PciPortOemSetLnkSpeed(PCI_DEV_INFO *Device, UINT8 *LnkSpeed, UINT8 SuppSpeeds){
    EFI_STATUS                          Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK     *List = OemSetLnkSpeedPciDevice;
//-------------------
    while(List->Callback)
    {
        OEM_PCI_DEVICE_SET_LNK_SPEED_FUNCTION *CallBack =(OEM_PCI_DEVICE_SET_LNK_SPEED_FUNCTION*)List->Callback; 
    //--------------------------------

        if((List->VendorId == Device->DevVenId.VenId && List->DeviceId == Device->DevVenId.DevId)||
            (List->VendorId == 0 && List->DeviceId == 0))
        {
            DEBUG((DEBUG_VERBOSE, "\n  PortA4.x: OEM Device Set LNK SPEED Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
	        Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
            Device->DevVenId.VenId, Device->DevVenId.DevId));

            Status = CallBack(Device, LnkSpeed, SuppSpeeds);
            DEBUG((DEBUG_VERBOSE, " %r.\n", Status));
        	//Handle the results...
        	if( EFI_ERROR(Status))
                return Status;
        }
        List++;
    }

    return Status;
}

//To avoid compilation error define Srtuct delivered trough eLink as extern function.
#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemGetOptRomPciDevice[] =
    { OEM_PCI_DEVICE_GET_OPT_ROM_CALLBACK_LIST { 0, 0, NULL } };



/**
    Porting function called for all PCI Devices. 

    This function allow to override or provide an Option ROM for this device.
  
   @param Pointer to the PCI Bus Driver Private data buffer. 
   @param Double Pointer to the proposed Option ROM Buffer (must be updated with the Option ROM image).
   @param Pointer to the Option ROM size (must be updated with the Option ROM image size). 

   @retval   EFI_STATUS     
    EFI_SUCCESS     - this device was programmed successfully.
    EFI_UNSUPPORTED - this device do not have this porting hook implemented.
    ERROR_STATUS    - actual ERROR code of the unsuccessful operation.
    
**/    
EFI_STATUS EFIAPI PciPortOemGetOptRom(PCI_DEV_INFO *Device, VOID **OptRom, UINT64 *OptRomSize){
    EFI_STATUS                          Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK     *List = OemGetOptRomPciDevice;
//-------------------
    while(List->Callback)
    {
        OEM_PCI_DEVICE_GET_OPT_ROM_FUNCTION *CallBack =(OEM_PCI_DEVICE_GET_OPT_ROM_FUNCTION*)List->Callback; 
    //--------------------------------

         if((List->VendorId == Device->DevVenId.VenId && List->DeviceId == Device->DevVenId.DevId)||
            (List->VendorId == 0 && List->DeviceId == 0))
        {
             DEBUG((DEBUG_VERBOSE, "\n  PortA4.x: OEM Device GetOpROM Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
	        Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
            Device->DevVenId.VenId, Device->DevVenId.DevId));

            Status = CallBack(Device, OptRom, OptRomSize);
            DEBUG((DEBUG_VERBOSE, " %r.\n", Status));
        	if( EFI_ERROR(Status))
        	    return Status;
        }
        List++;
    }

    return Status;
}



//To avoid compilation error define Srtuct delivered trough eLink as extern function.
#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemPciOutOfResourcesRemoveDevice[] =
    { OEM_PCI_DEVICE_OUT_OF_RESOURCES_CALLBACK_LIST { 0, 0, NULL } };

/**
    Porting function called for all PCI Devices. 
    This function will be called only if enumeration process will run out of resources.
    It allow to Remove Device's resource request of "LowResType" from resource pool.
    
   @param Pointer to the PCI Bus Driver Private data buffer. 
   @param Iteration counter value.
   @param Resource type that system is running out. 

   @retval   EFI_STATUS     
    EFI_SUCCESS     - this device was programmed successfully.
    EFI_UNSUPPORTED - this device do not have this porting hook implemented.
    ERROR_STATUS    - actual ERROR code of the unsuccessful operation.
    
**/    
EFI_STATUS EFIAPI PciPortOutOfResourcesRemoveDevice(PCI_DEV_INFO *Device, UINTN Count, UINTN LowResType){
    EFI_STATUS                          Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK     *List = OemPciOutOfResourcesRemoveDevice;
//-----------------------------    

    //must be only one + TERMIONATOR if any.
    while(List->Callback)
    {
        OEM_PCI_DEVICE_OUT_OF_RESOURCES_FUNCTION *CallBack =(OEM_PCI_DEVICE_OUT_OF_RESOURCES_FUNCTION*)List->Callback; 
    //-----------------------------

        DEBUG((DEBUG_VERBOSE, "\n\n  PortA4.x: OEM PCI OUT OF RESOURCES Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
        Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
        Device->DevVenId.VenId, Device->DevVenId.DevId));

        Status = CallBack(Device, Count, LowResType);
        DEBUG((DEBUG_VERBOSE, " %r.\n\n", Status));

        List++;

    }
    return Status;
}


//To avoid compilation error define Srtuct delivered trough eLink as extern function.
#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemAttributesPciDevice[] =
    { OEM_PCI_DEVICE_ATTRIBUTE_CALLBACK_LIST { 0, 0, NULL } };


/**
    Porting function called for all PCI Devices. 
    This function will be called only if enumeration process will run out of resources.
    It allow to Remove Device's resource request of "LowResType" from resource pool.
    
   @param Pointer to the PCI Bus Driver Private data buffer. 
   @param Iteration counter value.
   @param Resource type that system is running out. 

   @retval   EFI_STATUS     
    EFI_SUCCESS     - this device was programmed successfully.
    EFI_UNSUPPORTED - this device do not have this porting hook implemented.
    ERROR_STATUS    - actual ERROR code of the unsuccessful operation.
    
**/    
EFI_STATUS EFIAPI PciPortOemAttributes(PCI_DEV_INFO *Device, UINT64 *Attr, UINT64 Capab, BOOLEAN Set){
    EFI_STATUS                          Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK     *List = OemAttributesPciDevice;
//-------------------
    while(List->Callback)
    {
        OEM_PCI_DEVICE_ATTRIBUTE_FUNCTION *CallBack =(OEM_PCI_DEVICE_ATTRIBUTE_FUNCTION*)List->Callback; 
    //--------------------------------

        if((List->VendorId == Device->DevVenId.VenId && List->DeviceId == Device->DevVenId.DevId)||
            (List->VendorId == 0 && List->DeviceId == 0))
        {
            DEBUG((DEBUG_VERBOSE, "\n  PortA4.x: OEM Device Attributes Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
	        Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
            Device->DevVenId.VenId, Device->DevVenId.DevId));

            Status = CallBack(Device, Attr, Capab, Set);
            DEBUG((DEBUG_VERBOSE, " %r.\n", Status));
        	if( EFI_ERROR(Status))
        	    return Status;
        }
        List++;
    }

    return Status;
}


#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemCheckBadPcieDevice[] =
    { OEM_PCI_CHECK_BAD_PCIE_DEVICE_CALLBACK_LIST { 0, 0, NULL } };

/**
    Porting function called for all PCI Express capable Devices. 
    This function allows to disable PCI Express initialization for devices that not fully compliant 
    with PCIe Specification, and were initialized by RC or Board specific code.
    
   @param Pointer to the PCI Bus Driver Private data buffer. 

   @retval   EFI_STATUS     
    EFI_SUCCESS     - this device was programmed successfully.
    EFI_UNSUPPORTED - this device do not have this porting hook implemented.
    ERROR_STATUS    - actual ERROR code of the unsuccessful operation.
    
**/    
EFI_STATUS EFIAPI PciPortCheckBadPcieDevice(PCI_DEV_INFO *Device){
    EFI_STATUS                      Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK *List = OemCheckBadPcieDevice;
//-------------------
    while(List->Callback)
    {
        OEM_PCI_PROGRAM_DEVICE_FUNCTION *CallBack=(OEM_PCI_PROGRAM_DEVICE_FUNCTION*)List->Callback;
    //--------------------------------
        if((List->VendorId == Device->DevVenId.VenId && List->DeviceId == Device->DevVenId.DevId)||
                (List->VendorId == 0 && List->DeviceId == 0))
        {
            DEBUG((DEBUG_VERBOSE, "\n  PortA4.x: OEM Check Bad PCIe Device Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
	        Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
            Device->DevVenId.VenId, Device->DevVenId.DevId));

            Status = CallBack(Device);
            DEBUG((DEBUG_VERBOSE, " %r.\n", Status));
        	if( EFI_ERROR(Status))
        	return Status;
        }
        List++;
    }

    return Status;
}


#define OEM_PCI_DEVICE_CALLBACK( VendorId, DeviceId, Callback ) { VendorId, DeviceId, &Callback }
OEM_PCI_DEVICE_PROGRAM_CALLBACK OemQueryPciDevice[] =
    { OEM_PCI_QUERY_PCI_DEVICE_CALLBACK_LIST { 0, 0, NULL } };

/**
    Porting function called for all PCI Devices. 
    This function allows additional device programming and Private data properties 
    setting right after QueryPciDevice() function finished collecting all PCi Device 
    Properties. 
    
   @param Pointer to the PCI Bus Driver Private data buffer. 

   @retval   EFI_STATUS     
    EFI_SUCCESS     - this device was programmed successfully.
    EFI_UNSUPPORTED - this device do not have this porting hook implemented.
    ERROR_STATUS    - actual ERROR code of the unsuccessful operation.
    
**/    
EFI_STATUS EFIAPI PciPortQueryPcieDevice(PCI_DEV_INFO *Device){
    EFI_STATUS                      Status=EFI_UNSUPPORTED;
    OEM_PCI_DEVICE_PROGRAM_CALLBACK *List = OemQueryPciDevice;
//-------------------
    while(List->Callback)
    {
        OEM_PCI_PROGRAM_DEVICE_FUNCTION *CallBack=(OEM_PCI_PROGRAM_DEVICE_FUNCTION*)List->Callback;
    //--------------------------------
        if((List->VendorId == Device->DevVenId.VenId && List->DeviceId == Device->DevVenId.DevId)||
                (List->VendorId == 0 && List->DeviceId == 0))
        {
            DEBUG((DEBUG_VERBOSE, "\n  PortA4.x: OEM Query PCI Device Callback @ B%X|D%X|F%X VID=0x%X; DID=0x%X;", 
            Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function, 
            Device->DevVenId.VenId, Device->DevVenId.DevId));

            Status = CallBack(Device);
            DEBUG((DEBUG_VERBOSE, " %r.\n", Status));
        	if( EFI_ERROR(Status))
                return Status;
        }
        List++;
    }

    return Status;
}


/**
    This function is the entry point for PCI Compatibility Driver. 
    This is a DXE driver. It will initialize and install an instance of  
    AMI Proprietary AMI_PCI_PORT_COMPATIBILITY_PROTOCOL. 

        
    @param ImageHandle Image handle.
    @param SystemTable Pointer to the EFI system table.

    @retval EFI_STATUS
        EFI_SUCCESS         When everything is going on fine!
        EFI_NOT_FOUND       When something required is not found!
        EFI_DEVICE_ERROR    When the device is not responding!

    @note  
    Entry Points are used to locate or install protocol interfaces and
    notification events. 

**/

EFI_STATUS EFIAPI PciPortEntryPoint(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
	EFI_STATUS				Status=EFI_SUCCESS;
//-----------------
	InitAmiLib(ImageHandle,SystemTable);

	gAmiPciPort=MallocZ(sizeof(AMI_PCI_PORT_COMPATIBILITY_PROTOCOL));
	if(gAmiPciPort==NULL){
		Status=EFI_OUT_OF_RESOURCES;
		ASSERT_EFI_ERROR(Status);
		return Status;
	}
	
	gAmiPciPort->PciPortSkipThisDevice		= PciPortSkipThisDevice;
	gAmiPciPort->PciPortOemProgDevice		= PciPortOemProgDevice;
	gAmiPciPort->PciPortCheckBadPcieDevice	= PciPortCheckBadPcieDevice;
	gAmiPciPort->PciPortOemAttributes		= PciPortOemAttributes;
	gAmiPciPort->PciPortOemSetAspm			= PciPortOemSetAspm;
	gAmiPciPort->PciPortOemSetLnkSpeed		= PciPortOemSetLnkSpeed;
	gAmiPciPort->PciPortOemGetOptRom		= PciPortOemGetOptRom;
	gAmiPciPort->PciPortOutOfResourcesRemoveDevice	= PciPortOutOfResourcesRemoveDevice;
	gAmiPciPort->PciPortQueryPcieDevice     = PciPortQueryPcieDevice;
	
	
	//Now install all Protocol Instance
    Status=pBS->InstallMultipleProtocolInterfaces(
        &gAmiPciPortHandle,
        &gAmiPciPortCompatibilityProtocolGuid, gAmiPciPort, NULL
    );
    ASSERT_EFI_ERROR(Status);

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
