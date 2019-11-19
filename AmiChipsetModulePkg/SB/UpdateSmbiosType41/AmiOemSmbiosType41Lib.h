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

/** @file AmiOemSmbiosType41Lib.h
    Ami Oem Smbios Type41 Library Header file.

**/
#ifndef __AMI_OEM_SMBIOS_TYPE41_LIB__H__
#define __AMI_OEM_SMBIOS_TYPE41_LIB__H__

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
);
#endif

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
