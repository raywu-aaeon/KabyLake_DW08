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
/** @file AmiOemSmbiosType41LibNull.c
    OEM smbios type41 structure Library implementation.

**/
//**********************************************************************
#include <PiDxe.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)
typedef struct _OEM_ONBOARD_DEV_TABLE {
    UINT16       SegGroupNumber;
    UINT8        BusNumber;
    UINT8        DevNumber;
    UINT8        FuncNumber;
    UINT8        DeviceType;
    CHAR8        *String;
    UINT8        RpNumber;
} OEM_ONBOARD_DEV_TABLE;

#pragma pack(1)
typedef struct {
    UINT8   Type;
    UINT8   Length;
    UINT16  Handle;
} SMBIOS_STRUCTURE_HEADER;

/**
    Onboard Devices Extended Information (Type 41)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     RefDesignation;
    UINT8                     DeviceType;
    UINT8                     DeviceTypeInstance;
    UINT16                    SegGroupNumber;
    UINT8                     BusNumber;
    UINT8                     DevFuncNumber;
} SMBIOS_ONBOARD_DEV_EXT_INFO;
#pragma pack()

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// GUID Definition(s)

// Protocol/Ppi Definition(s)

// External Declaration(s)

// Variable Declaration(s)

/**
    The function is to use current PCIe root port to get current PCIe device's bus number

    @param RpNumber - PCIe device's root port number
	@param BusNum   - PCIe device's bus number

    @retval VOID

**/
VOID
OemGetPCIeDeviceBusNumber(
    IN     UINT8    RpNumber,
	IN OUT UINT8    *BusNum
)
{
	/* Oem implements it
	UINTN    RpDev;
	UINTN    RpFun;
	UINTN    DeviceBase;
	
    GetPchPcieRpDevFun(RpNumber, &RpDev, &RpFun);
	
	DeviceBase = ((UINTN) (PcdGet64 (PcdPciExpressBaseAddress)) + (UINTN) (0x0 << 20) + (UINTN) (RpDev << 15) + (UINTN) (RpFun << 12));
	*BusNum    = *(volatile UINT8*) (DeviceBase + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
	*BusNum    = *(volatile UINT8*) (DeviceBase + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
    */
    
    return;
}

/**
    The function is to modify information of SMBIOS type 41 table by customer.
	If OEM want to modify information of SMBIOS type 41 table except for string 
	of SMBIOS type 41 table, OEM can override it directly in buffer.
	OEM need to modify string of OnboardDevice if OEM want to modify string of 
	SMBIOS type 41 table. And then the string will be updated in SMBIOS type 41
	table of onboard device.

    @param OnboardDevice - Pointer to OEM onboard device information
    @param Buffer - Pointer to onboard device's SMBIOS type 41 table

    @retval VOID

**/
VOID
OEMUpdateSMBIOSType41(
    IN OUT OEM_ONBOARD_DEV_TABLE        *OnboardDevice,
    IN OUT SMBIOS_ONBOARD_DEV_EXT_INFO  *Buffer
)
{
    /* Oem implements it
    UINT8    BusNum = 0;

    // Update the actual bus number of PCIe device during POST
    if (OnboardDevice->RpNumber != 0xFF){
        OemGetPCIeDeviceBusNumber(OnboardDevice->RpNumber, &BusNum);
        OnboardDevice->BusNumber = BusNum;
        Buffer->BusNumber        = BusNum;
    }
    */
        
    return;
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
