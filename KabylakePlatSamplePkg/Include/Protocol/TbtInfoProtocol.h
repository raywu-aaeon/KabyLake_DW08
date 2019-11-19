//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#ifndef _TBT_INFO_H_
#define _TBT_INFO_H_


#define AMI_THUNDERBOLT_INFO_PROTOCOL_GUID \
{0xba1d8920, 0x803e, 0x4b26, 0x40, 0xff, 0x58, 0x57, 0x3, 0x10, 0x7b, 0x36}

typedef struct _AMI_THUNDERBOLT_INFO_PROTOCOL AMI_THUNDERBOLT_INFO_PROTOCOL;

//
// What PCIe slot does the AIC use
//
#pragma pack (1)
typedef struct {
  UINT8    Bus;
  UINT8    Dev;
  UINT8    Fun;
} TBT_ROOT_PORT_LOCATION;

//
// How much resource does the AIC consumed
//
typedef struct {
  UINT16   MemoryMB;
  UINT8    IoKB;
  UINT16   PrefetchMB;
  UINT8    StartedBusNo;
  UINT8    TerminalBusNo;
} TBT_ROOT_PORT_RESOURCE;

//
// Thunderbolt Security Level Flag
//
typedef enum {
  SL0_NoSecurity = 0,
  SL1_UserAuthorization,
  SL2_SecureConnect,
  SL3_DpOnly,
  SLx_NotSet = 0xff
} TBT_AIC_SLX;

//
// TBT information descriptor
//
typedef struct {
  TBT_ROOT_PORT_LOCATION    Location;           // slot location
  TBT_ROOT_PORT_RESOURCE    ResourceConsumed;   // resource usage
  TBT_AIC_SLX               SecurityLevel;      // security configuration, SL0=0 / SL1=1 / SL2=2 / SL3=3 / Unknown=FFh
  UINT16                    DeviceID;           // identify host series
} AMI_TBT_STATUS;
#pragma pack ()

/**
  Get Thunderbolt Information.

  @param  This                   Indicates a pointer to the calling context.
  @param  AicIndex               Select Thunderbolt AIC number
  @param  Information            A pointer points to the AMI_TBT_STATUS data structure

  @retval EFI_SUCCESS            All valid data is successfully output
  @retval EFI_NO_MEDIA           The selected AIC is not presented
  @retval EFI_INVALID_PARAMETER  Information is NULL
  @retval EFI_NOT_READY          Some required information is missed.
  @retval EFI_OUT_OF_RESOURCES   Bus number is not enough for Thunderbolt host

**/
typedef
EFI_STATUS
(EFIAPI *AMI_THUNDERBOLT_DESCRIPTOR)(
  IN  AMI_THUNDERBOLT_INFO_PROTOCOL    *This,
  IN  UINTN                            AicIndex,
  OUT AMI_TBT_STATUS                   *Information
  );

//
// Thunderbolt Information Protocol
//
struct _AMI_THUNDERBOLT_INFO_PROTOCOL {
  UINTN                         ReportAicCount;
  AMI_THUNDERBOLT_DESCRIPTOR    GetAicInfomation;
};

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gAmiThunderboltInfoProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************