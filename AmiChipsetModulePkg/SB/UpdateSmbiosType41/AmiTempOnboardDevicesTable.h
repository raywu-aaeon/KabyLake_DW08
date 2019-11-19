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

/** @file AmiOemOnboardDevicesTable.h
    AmiOemOnboardDevicesTable Header file.

**/
#ifndef __AMI_OEM_ONBOARD_DEVICES_TABLE_H__
#define __AMI_OEM_ONBOARD_DEVICES_TABLE_H__

/**
 Description:
  This file is used for OEM increasing Smbios Type 41 List.
**/
//-------------------------------------------------------------------------------------------------------------------------------------
// enum        | UINT16  | UINT8 | UINT8  | UINT8    | UINT8       | CHAR8  *              | UINT8            |
// IncompType; | Segment |  Bus  | Device | Function | Device Type | Reference Designation | Root port Number |
//-------------------------------------------------------------------------------------------------------------------------------------
/// The rule:
/// 1. If PCIe device is one onboard device, you need to write one new function that gets actual PCIe device's bus number to override the function OemGetPCIeDeviceBusNumber when root port Number isn't 0xFF.
///    Or you need to define actual PCIe device's bus number when root port Number is 0xFF.  
/// 2. If device state of device type (BIT7) of SMBIOS type 41 table is enabled, it will help check device state again and modify its device state.
///
/// Example:
/// NV Graphics card B1 D0 F0
/// {0x0, 0x1, 0x0, 0x0, 0x83, "Onboard - NV GD", 0xFF},
#endif // __AMI_OEM_ONBOARD_DEVICES_TABLE_H__

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
