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

/** @file UpdateSmbiosType41.h
    UpdateSmbiosType41 Header file.

**/
#ifndef __UPDATESMBIOSTYPE41__H__
#define __UPDATESMBIOSTYPE41__H__

#define SMBIOS_TYPE_41_LENGTH   0xB

#define SMBIOS_ONBOARD_DEVICE_EXT_OTHER        L"Onboard - Other"
#define SMBIOS_ONBOARD_DEVICE_EXT_UNKNOWN      L"Unknown"
#define SMBIOS_ONBOARD_DEVICE_EXT_VIDEO        L"Onboard - Video"
#define SMBIOS_ONBOARD_DEVICE_EXT_SCSI         L"Onboard - SCSI"
#define SMBIOS_ONBOARD_DEVICE_EXT_ETHERNET     L"Onboard - Ethernet"
#define SMBIOS_ONBOARD_DEVICE_EXT_TOKEN_RING   L"Onboard - Token Ring"
#define SMBIOS_ONBOARD_DEVICE_EXT_SOUND        L"Onboard - Sound"
#define SMBIOS_ONBOARD_DEVICE_EXT_PATA         L"Onboard - PATA"
#define SMBIOS_ONBOARD_DEVICE_EXT_SATA         L"Onboard - SATA"
#define SMBIOS_ONBOARD_DEVICE_EXT_SAS          L"Onboard - SAS"

#if ONBOARD_DEVICE_EXTENDED_INFO == 0
#pragma pack(1)
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
#endif

typedef struct {
    UINT8        DeviceType;
    UINT8        DeviceTypeInstance;
    UINT16       SegGroupNumber;
    UINT8        BusNumber;
    UINT8        DevFuncNumber;
} ONBOARD_DEV_EXT;

typedef struct {
    UINT8        OtherNum;
    UINT8        UnknownNum;
    UINT8        VideoNum;
    UINT8        SCSIControllerNum;
    UINT8        EthernetNum;
    UINT8        TokenRingNum;
    UINT8        SoundNum;
    UINT8        PATAControllerNum;
    UINT8        SATAControllerNum;
    UINT8        SASControllerNum;
} DEVICE_TYPES_FIELD;

typedef struct _OEM_ONBOARD_DEV_TABLE {
    UINT16       SegGroupNumber;
    UINT8        BusNumber;
    UINT8        DevNumber;
    UINT8        FuncNumber;
    UINT8        DeviceType;
    CHAR8        *String;
    UINT8        RpNumber;
} OEM_ONBOARD_DEV_TABLE;

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
