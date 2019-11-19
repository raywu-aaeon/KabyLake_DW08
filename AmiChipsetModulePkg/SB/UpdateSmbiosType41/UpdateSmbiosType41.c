//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

//**********************************************************************
/** @file UpdateSmbiosType41.c
    This file contains code for updating smbios type41 structure.

**/
//**********************************************************************
#include <Protocol/AmiSmbios.h>
#include <Protocol/SmbiosUpdateDataProtocol.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>
#include <Library/BaseLib.h>
#include "UpdateSmbiosType41.h"
#include "AmiOemSmbiosType41Lib.h"
#include "Token.h"

AMI_SMBIOS_PROTOCOL             *gAmiSmbiosProtocol;
EFI_SMBIOS_UPDATE_DATA_PROTOCOL *gSmbiosUpdateDataProtocol;
DEVICE_TYPES_FIELD              OnBoardDeviceNum = {0x0};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_BOOT_SERVICES      *pBS=NULL;
GLOBAL_REMOVE_IF_UNREFERENCED OEM_ONBOARD_DEV_TABLE  gOemOnboardDevList[] =
//-------------------------------------------------------------------------------------------------------------------------------------
// enum        | UINT16  | UINT8 | UINT8  | UINT8    | UINT8       | CHAR8  *              | UINT8            |
// IncompType; | Segment |  Bus  | Device | Function | Device Type | Reference Designation | Root port Number |
//-------------------------------------------------------------------------------------------------------------------------------------
{ 
#include <AmiOemOnboardDevicesTable.h>
{0xFFFF, 0xFF, 0xFF, 0xFF, 0xFF, NULL, 0xFF}
};

/**
    This function is to get total size of SMBIOS structure

    @param BufferStart - Pointer to SMBIOS structure 

    @retval UINT16 Total size of SMBIOS structure.

**/
UINT16 SMBIOS_GetStructureTotalSize(
    IN UINT8	*BufferStart
)
{
    UINT8 *BufferEnd;

    BufferEnd = BufferStart;
    BufferEnd += ((SMBIOS_STRUCTURE_HEADER*)BufferStart)->Length;
    while (*(UINT16*)BufferEnd != 0)
    {
        BufferEnd++;
    }

    return (UINT16)(BufferEnd + 2 - BufferStart);   // +2 for double zero terminator
}

/**
    This function is to check onboard device state and then update device state of SMBIOS type 41

    @param DeviceList     - One of devices of OEM device list
    @param DeviceState    - Device State

    @retval VOID

**/
VOID CheckDeviceExist(
    IN SMBIOS_ONBOARD_DEV_EXT_INFO  DeviceList,
    IN OUT UINT8              *DeviceType
)
{
	UINT16    DeviceId, VendorId;
	UINTN     DeviceBase;
	UINT8     BusNum, DevNum, FuncNum;

	*DeviceType = DeviceList.DeviceType;
	BusNum      = DeviceList.BusNumber;
	DevNum      = (DeviceList.DevFuncNumber & 0xF8) >> 3;
	FuncNum     = DeviceList.DevFuncNumber & 0x7;
	
	DeviceBase  = ((UINTN) (PcdGet64 (PcdPciExpressBaseAddress)) + (UINTN) (BusNum << 20) + (UINTN) (DevNum << 15) + (UINTN) (FuncNum << 12));
	DeviceId    = *(volatile UINT16*) (DeviceBase + PCI_DEVICE_ID_OFFSET);
	VendorId    = *(volatile UINT16*) (DeviceBase + PCI_VENDOR_ID_OFFSET);
	
	if ((DeviceId == 0xFFFF) && (VendorId == 0xFFFF))
		*DeviceType &= ~BIT7;
}

#if COLLECT_OTHER_ONBOARD_PCIE_DEVICES == 1
/**
    This function is to check if SMBIOS Type 41 table of onboard deivce is created

    @param PciSeg       - Device's Segment
    @param PciBus       - Device's Bus
    @param PciDev       - Device's Device
    @param PciFun       - Device's Function
    @param ClassCode    - Device's ClassCode
    @param SubClassCode - Device's SubClassCode

    @retval FALSE    - Create this device's Smbios Type 41 structure
	        TRUE     - Skip this device

**/
BOOLEAN CheckSmbiosType41IsCreated(
    IN  UINTN               PciSeg,
    IN  UINTN               PciBus,
    IN  UINTN               PciDev,
    IN  UINTN               PciFun,
    IN OUT  UINT8           *ClassCode,
    IN OUT  UINT8           *SubClassCode
)
{
	UINTN    DeviceBase;
	UINTN    OEMTableCount = 0;
	UINTN    Index;
	
    OEMTableCount = sizeof(gOemOnboardDevList) / sizeof(OEM_ONBOARD_DEV_TABLE);
	
	if (OEMTableCount > 1){
		for (Index = 0; Index < (OEMTableCount - 1); Index++){
			if((PciSeg == gOemOnboardDevList[Index].SegGroupNumber) && (PciBus == gOemOnboardDevList[Index].BusNumber) && (PciDev == gOemOnboardDevList[Index].DevNumber) && (PciFun == gOemOnboardDevList[Index].FuncNumber)){
				DEBUG((DEBUG_INFO, "SMBIOS type 41 table of PCI device is created\n"));
				return TRUE;
			}
		}
	}
	
    // Skip External Device
    if (PciBus > 0) return TRUE;
	
	DeviceBase    = ((UINTN) (PcdGet64 (PcdPciExpressBaseAddress)) + (UINTN) (PciBus << 20) + (UINTN) (PciDev << 15) + (UINTN) (PciFun << 12));
	*ClassCode    = *(volatile UINT8*) (DeviceBase + 0xB);
	*SubClassCode = *(volatile UINT8*) (DeviceBase + 0xA);
	if(*(volatile UINT16*) (DeviceBase) == 0xFFFF) return TRUE;
                           
    // Skip PCI-to-PCI bridge/Semi-transparent PCI-to-PCI bridge
    if (((*ClassCode == 0x6) && (*SubClassCode == 0x4)) || ((*ClassCode == 0x6) && (*SubClassCode == 0x9)))
        return TRUE;
    
    // ClassCode of devices is reserved and then those devices need to skip
    if (*ClassCode >= 0x14)
        return TRUE;
    
    return FALSE;
}

/**
    This function is to add Smbios Type 41 Tables automatically for onboard PCI devices

    @param OnBoardDevice - OnBoard Device information

    @retval VOID

**/
VOID AutoUpdateSmbiosType41(
    IN ONBOARD_DEV_EXT  OnBoardDevice
)
{
    EFI_STATUS                   Status;
    SMBIOS_ONBOARD_DEV_EXT_INFO	 *SmbiosType41Buffer;
	UINT16                       BufferSize;
    UINTN	                     StringLength;
	CHAR8                        *AsciiString = NULL;
	CHAR8                        *OptionalStrStart;
	UINTN                        StringSize = 0;
	
	switch (OnBoardDevice.DeviceType & 0x7F){
        case OnBoardDeviceTypeOther:
            StringSize   = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_OTHER);
			AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_OTHER, AsciiString);
			break;
		case OnBoardDeviceTypeVideo:
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_VIDEO);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_VIDEO, AsciiString);
		    break;
		case OnBoardDeviceTypeScsiController:
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_SCSI);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_SCSI, AsciiString);
		    break;
		case OnBoardDeviceTypeEthernet:
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_ETHERNET);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_ETHERNET, AsciiString);
		    break;
		case OnBoardDeviceTypeTokenRing:
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_TOKEN_RING);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_TOKEN_RING, AsciiString);
		    break;
		case OnBoardDeviceTypeSound:
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_SOUND);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_SOUND, AsciiString);
		    break;
		case OnBoardDeviceTypePATAController:
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_PATA);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_PATA, AsciiString);
		    break;
		case OnBoardDeviceTypeSATAController:
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_SATA);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_SATA, AsciiString);
		    break;
		case OnBoardDeviceTypeSASController:
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_SAS);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_SAS, AsciiString);
		    break;
        default:  // Unknown
            StringSize = StrSize(SMBIOS_ONBOARD_DEVICE_EXT_UNKNOWN);
            AsciiString  = AllocateZeroPool (StringSize);
			ASSERT (AsciiString != NULL);
            UnicodeStrToAsciiStr (SMBIOS_ONBOARD_DEVICE_EXT_UNKNOWN, AsciiString);
	}
	
	StringLength       = AsciiStrLen (AsciiString);
    SmbiosType41Buffer = AllocateZeroPool(sizeof(SMBIOS_ONBOARD_DEV_EXT_INFO) + StringLength + 1 + 1);
	ASSERT (SmbiosType41Buffer != NULL);

	SmbiosType41Buffer->StructureType.Type   = 41;
	SmbiosType41Buffer->StructureType.Length = SMBIOS_TYPE_41_LENGTH;
	SmbiosType41Buffer->StructureType.Handle = gSmbiosUpdateDataProtocol->SMBIOS_GetFreeHandle();
	SmbiosType41Buffer->RefDesignation       = 1;
    SmbiosType41Buffer->DeviceType           = OnBoardDevice.DeviceType;
	SmbiosType41Buffer->DeviceTypeInstance   = OnBoardDevice.DeviceTypeInstance;
	SmbiosType41Buffer->SegGroupNumber       = OnBoardDevice.SegGroupNumber;
	SmbiosType41Buffer->BusNumber            = OnBoardDevice.BusNumber;
	SmbiosType41Buffer->DevFuncNumber        = OnBoardDevice.DevFuncNumber;
    
	// Update Reference Designation
	OptionalStrStart = (CHAR8 *)(SmbiosType41Buffer + 1);
	AsciiStrCpy(OptionalStrStart, AsciiString);

    BufferSize = SMBIOS_GetStructureTotalSize((UINT8 *)SmbiosType41Buffer);
	Status = gAmiSmbiosProtocol->SmbiosAddStructure((UINT8 *)SmbiosType41Buffer, BufferSize);
	if (EFI_ERROR(Status))
	{
	    ASSERT_EFI_ERROR(Status);
	}
    
    pBS->FreePool(SmbiosType41Buffer);
	pBS->FreePool(AsciiString);

}

/**
    This function is to collect information of other onboard PCI devices 
	for SMBIOS Type 41 tables. And it creates SMBIOS type 41 tables for 
	those onboard PCI devices in function AutoUpdateSmbiosType41.

    @param  VOID

    @retval VOID

**/
VOID
CollectOtherOnboardDevices (
    VOID
)
{
	EFI_STATUS	        Status;
	EFI_HANDLE	        *HandleBuffer;
	UINTN	            HandleCount;
	ONBOARD_DEV_EXT     OnBoardDevice = {0xFF};
	EFI_PCI_IO_PROTOCOL	*PciDevice;
	UINT8               SubClassCode = 0;
	UINT8               ClassCode = 0;
    UINTN               PciSeg;
    UINTN               PciBus;
    UINTN               PciDev;
    UINTN               PciFun;
    UINTN               Index;
    BOOLEAN             DeviceSMBIOSExist;
	BOOLEAN             OtherDevice;
	
	HandleCount  = 0;
	HandleBuffer = NULL;
	Status = pBS->LocateHandleBuffer (
				ByProtocol,
				&gEfiPciIoProtocolGuid,
				NULL,
				&HandleCount,
				&HandleBuffer
				);

	if (EFI_ERROR (Status)) {
		return; 
	}
	
	// Check Onboard PCI devices
	for (Index = 0; Index < HandleCount; Index++) {
		// Collect Onboard PCI devices information 
		Status = pBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID **) &PciDevice);
		if (!EFI_ERROR (Status)) {
			// Get PCI Device Bus/Device/Function Numbers
    	    Status = PciDevice->GetLocation( PciDevice, &PciSeg, &PciBus, &PciDev, &PciFun );
    	    if (EFI_ERROR(Status)) continue;
			
			DeviceSMBIOSExist = CheckSmbiosType41IsCreated(PciSeg, PciBus, PciDev, PciFun, &ClassCode, &SubClassCode);
			if (DeviceSMBIOSExist) continue;
			
	        DEBUG((DEBUG_INFO, "PCI Seg, Bus, Device, function = %02d, %02d, %02d, %02d\n", PciSeg, PciBus, PciDev, PciFun));
			DEBUG((DEBUG_INFO, "PCI ClassCode, SubClassCode = %02d, %02d\n", ClassCode, SubClassCode));
            
			OtherDevice = FALSE;
			// Onboard Device Types
			switch (ClassCode){
				case 0x3: // Base Class 03h - For all types of display controllers
					OnBoardDevice.DeviceType = 0x3; // Video
					OnBoardDeviceNum.VideoNum = OnBoardDeviceNum.VideoNum + 1;
					OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.VideoNum;
					break;
				case 0x1: // Base Class 01h - For all types of mass storage controllers
				    if (SubClassCode == 0x7){
						OnBoardDevice.DeviceType = 0xA; // SAS Controller
						OnBoardDeviceNum.SASControllerNum = OnBoardDeviceNum.SASControllerNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.SASControllerNum;
					} else if ((SubClassCode == 0x5) || (SubClassCode == 0x6)){
						OnBoardDevice.DeviceType = 0x9; // SATA Controller
						OnBoardDeviceNum.SATAControllerNum = OnBoardDeviceNum.SATAControllerNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.SATAControllerNum;
					} else if (SubClassCode == 0x0){
						OnBoardDevice.DeviceType = 0x4; // SCSI Controller
						OnBoardDeviceNum.SCSIControllerNum = OnBoardDeviceNum.SCSIControllerNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.SCSIControllerNum;
					} else if (SubClassCode == 0x1){
						OnBoardDevice.DeviceType = 0x8; // PATA Controller
						OnBoardDeviceNum.PATAControllerNum = OnBoardDeviceNum.PATAControllerNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.PATAControllerNum;
					} else {
					    OtherDevice = TRUE;
					}
				    break;
				case 0x2: // Base Class 02h - For all types of network controllers
				    if ((SubClassCode == 0x0) || (SubClassCode == 0x80)){
						OnBoardDevice.DeviceType = 0x5; // Ethernet
						OnBoardDeviceNum.EthernetNum = OnBoardDeviceNum.EthernetNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.EthernetNum;
					} else if (SubClassCode == 0x1){
						OnBoardDevice.DeviceType = 0x6; // Token Ring
						OnBoardDeviceNum.TokenRingNum = OnBoardDeviceNum.TokenRingNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.TokenRingNum;
					} else {
					    OtherDevice = TRUE;
					}

                    break;
				case 0xD: // Base Class 0Dh - For all types of wireless controllers
				    if ((SubClassCode == 0x20) || (SubClassCode == 0x21) || (SubClassCode == 0x41)){
				        OnBoardDevice.DeviceType = 0x5; // Ethernet
						OnBoardDeviceNum.EthernetNum = OnBoardDeviceNum.EthernetNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.EthernetNum;
					} else {
					    OtherDevice = TRUE;
					}
                    break;
				case 0x4: // Base Class 04h - For all types of multimedia devices
				    if (SubClassCode == 0x0){
						OnBoardDevice.DeviceType = 0x3; // Video
						OnBoardDeviceNum.VideoNum = OnBoardDeviceNum.VideoNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.VideoNum;
					} else if ((SubClassCode == 0x1) || (SubClassCode == 0x3) || (SubClassCode == 0x80)){
						OnBoardDevice.DeviceType = 0x7; // Sound
						OnBoardDeviceNum.SoundNum = OnBoardDeviceNum.SoundNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.SoundNum;
					} else {
					    OtherDevice = TRUE;
					}
                    break;
				case 0xF: // Base Class 0Fh - For satellite communication controllers
					if ((SubClassCode == 0x2) || (SubClassCode == 0x3)){
						OnBoardDevice.DeviceType = 0x7; // Sound
						OnBoardDeviceNum.SoundNum = OnBoardDeviceNum.SoundNum + 1;
						OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.SoundNum;
					} else {
					    OtherDevice = TRUE;
					}
                    break;
				default:
                    OtherDevice = TRUE;
			}
			
			if (OtherDevice) {
				OnBoardDevice.DeviceType = 0x1; // Other
                OnBoardDeviceNum.OtherNum = OnBoardDeviceNum.OtherNum + 1;
                OnBoardDevice.DeviceTypeInstance = OnBoardDeviceNum.OtherNum;
			}

			// Collect all Onboard devices
			OnBoardDevice.DeviceType         = OnBoardDevice.DeviceType | BIT7; // Device Enabled
			OnBoardDevice.SegGroupNumber     = (UINT16) PciSeg;
			OnBoardDevice.BusNumber          = (UINT8) PciBus;
			OnBoardDevice.DevFuncNumber      = (((((UINT8)PciDev) & 0x1F) << 3) | (((UINT8)PciFun) & 0x7));
	       
    	    // Add Smbios Type 41
			AutoUpdateSmbiosType41(OnBoardDevice);
		}
	}
}
#endif

/**
    This function is to create SMBIOS Type 41 tables according to gOemOnboardDevList.
	If OEM want to modify information of SMBIOS type 41 table except for string of 
	SMBIOS type 41 table, OEM can override it directly in function OEMUpdateSMBIOSType41.
	OEM need to modify string of OnboardDevice in function OEMUpdateSMBIOSType41 if 
	OEM want to modify string of SMBIOS type 41 table. And then the string will be 
	updated in SMBIOS type 41 table.

    @param OnBoardDevice - OnBoard Device information

    @retval VOID

**/
VOID UpdateOemDevListSmbiosType41(
    IN OEM_ONBOARD_DEV_TABLE  DeviceList
)
{
    EFI_STATUS                   Status;
    SMBIOS_ONBOARD_DEV_EXT_INFO	 *SmbiosType41Buffer;
	OEM_ONBOARD_DEV_TABLE        OnboardDevice;
	UINT16                       BufferSize = 0;
	UINTN                        StringSize = 0;
	CHAR8                        *OptionalStrStart;
	UINT8                        DeviceTypeInstance;
	UINT8                        DeviceType = 0;
	
	OnboardDevice = DeviceList;
	
	switch (OnboardDevice.DeviceType & 0x7F){
        case OnBoardDeviceTypeOther:
            OnBoardDeviceNum.OtherNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.OtherNum;
			break;
		case OnBoardDeviceTypeVideo:
            OnBoardDeviceNum.VideoNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.VideoNum;
		    break;
		case OnBoardDeviceTypeScsiController:
            OnBoardDeviceNum.SCSIControllerNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.SCSIControllerNum;
		    break;
		case OnBoardDeviceTypeEthernet:
            OnBoardDeviceNum.EthernetNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.EthernetNum;
		    break;
		case OnBoardDeviceTypeTokenRing:
            OnBoardDeviceNum.TokenRingNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.TokenRingNum;
		    break;
		case OnBoardDeviceTypeSound:
            OnBoardDeviceNum.SoundNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.SoundNum;
		    break;
		case OnBoardDeviceTypePATAController:
            OnBoardDeviceNum.PATAControllerNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.PATAControllerNum;
		    break;
		case OnBoardDeviceTypeSATAController:
            OnBoardDeviceNum.SATAControllerNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.SATAControllerNum;
		    break;
		case OnBoardDeviceTypeSASController:
            OnBoardDeviceNum.SASControllerNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.SASControllerNum;
		    break;
        default:  // Unknown
            OnBoardDeviceNum.UnknownNum += 1;
			DeviceTypeInstance = OnBoardDeviceNum.UnknownNum;
	}
	
	StringSize = AsciiStrSize(OnboardDevice.String);
    SmbiosType41Buffer = AllocateZeroPool(sizeof(SMBIOS_ONBOARD_DEV_EXT_INFO) + StringSize + 1 + 1);
	ASSERT (SmbiosType41Buffer != NULL);

	SmbiosType41Buffer->StructureType.Type   = 41;
	SmbiosType41Buffer->StructureType.Length = SMBIOS_TYPE_41_LENGTH;
	SmbiosType41Buffer->StructureType.Handle = gSmbiosUpdateDataProtocol->SMBIOS_GetFreeHandle();
	SmbiosType41Buffer->RefDesignation       = 1;
    SmbiosType41Buffer->DeviceTypeInstance   = DeviceTypeInstance;
	SmbiosType41Buffer->DeviceType           = OnboardDevice.DeviceType;
	SmbiosType41Buffer->SegGroupNumber       = OnboardDevice.SegGroupNumber;
	SmbiosType41Buffer->BusNumber            = OnboardDevice.BusNumber;
	SmbiosType41Buffer->DevFuncNumber        = (((((UINT8)OnboardDevice.DevNumber) & 0x1F) << 3) | (((UINT8)OnboardDevice.FuncNumber) & 0x7));

	// OEM update SMBIOS type 41 table
    OEMUpdateSMBIOSType41(&OnboardDevice, SmbiosType41Buffer);
    
    // Update Reference Designation
    OptionalStrStart = (CHAR8 *)(SmbiosType41Buffer + 1);
    AsciiStrCpy(OptionalStrStart, OnboardDevice.String);
	
    // Check device state and modify Device state of SMBIOS type 41 table
	if (SmbiosType41Buffer->DeviceType & BIT7){
		CheckDeviceExist(*SmbiosType41Buffer, &DeviceType);
		SmbiosType41Buffer->DeviceType       = DeviceType;
	}

    BufferSize = SMBIOS_GetStructureTotalSize((UINT8 *)SmbiosType41Buffer);
	Status = gAmiSmbiosProtocol->SmbiosAddStructure((UINT8 *)SmbiosType41Buffer, BufferSize);
	if (EFI_ERROR(Status))
	{
	    ASSERT_EFI_ERROR(Status);
	}
    
    pBS->FreePool(SmbiosType41Buffer);
}

/**
    This function is to add SMBIOS type 41 tables according to gOemOnboardDevList
	and it also collects other onboard PCI devices to add SMBIOS type 41 tables except 
	for gOemOnboardDevList.

    @param  Event   - Callback Event
    @param  Context - Context for the callback

    @retval VOID

**/
VOID
SmbiosOnboardExt (
    IN EFI_EVENT  Event,
    IN VOID       *Context
)
{
	UINTN         OEMTableCount = 0;
	UINTN         Index;
	
	OEMTableCount = sizeof(gOemOnboardDevList) / sizeof(OEM_ONBOARD_DEV_TABLE);

	// Create SMBIOS Type 41 tables according to gOemOnboardDevList
	if (OEMTableCount > 1){
		for (Index = 0; Index < (OEMTableCount - 1); Index++){
			UpdateOemDevListSmbiosType41(gOemOnboardDevList[Index]);
		}
	}
	
	// Collect other onboard PCI devices and add SMBIOS type 41 tables for other onboard PCI devices
#if COLLECT_OTHER_ONBOARD_PCIE_DEVICES == 1
	CollectOtherOnboardDevices();
#endif
	
    if (Event != NULL)
        pBS->CloseEvent(Event);
}

/**
    This function is to entry point of update Smbios Type 41 driver and it registers 
	one callback function to add SMBIOS type 41 tables.

    @param ImageHandle - Pointer to Image Handle
    @param SystemTable - Pointer to System Table structure

    @retval EFI_STATUS EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UpdateSmbiosType41EntryPoint(
    IN EFI_HANDLE               ImageHandle,
    IN EFI_SYSTEM_TABLE         *SystemTable
)
{
    EFI_STATUS  Status;
	EFI_EVENT   OnboardDeviceEvent;

	pBS = SystemTable->BootServices;
	
    Status = pBS->LocateProtocol(&gAmiSmbiosProtocolGuid, NULL, (VOID **)&gAmiSmbiosProtocol);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }

    Status = pBS->LocateProtocol(&gSmbiosUpdateDataProtocolGuid, NULL, (VOID **)&gSmbiosUpdateDataProtocol);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }

	/*Status = pBS->CreateEvent(
		            EVT_NOTIFY_SIGNAL,
					TPL_CALLBACK,
					SmbiosOnboardExt,
					NULL,
					&OnboardDeviceEvent
					);
	if (EFI_ERROR(Status)) return Status;
	
	Status = pBS->RegisterProtocolNotify(
	                &StartSMBIOSType41Guid, 
	                OnboardDeviceEvent,
	                &gOnboardDeviceNotifyReg
	                );*/
	
	Status = pBS->CreateEventEx(
				    EVT_NOTIFY_SIGNAL,
					TPL_CALLBACK,
					SmbiosOnboardExt,
					NULL,
					&gEfiEventReadyToBootGuid,
					&OnboardDeviceEvent
					);
	
    return EFI_SUCCESS;
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
