//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
	
/** @file I2cProtocol.h
    Header of the I2c protocol.

**/

#ifndef _EFI_I2cProtocol_H_
#define _EFI_I2cProtocol_H_

extern EFI_GUID gEfiAmiI2CProtocolGuid;

typedef struct _EFI_AMI_I2C_PROTOCOL EFI_AMI_I2C_PROTOCOL;

typedef EFI_STATUS (EFIAPI *I2CCOMBINREAD) (
    IN  UINT8 BusNo,
    IN  UINT8 SlaveAddress,
    IN  UINT8 *WriteBuffer,
    IN OUT  UINT8 *ReadBuffer,
    IN  UINT16 WLength,
    IN  UINT16 RLength
);

typedef EFI_STATUS (EFIAPI *I2CREAD) (
    IN  UINT8 BusNo,
    IN  UINT8 SlaveAddress,
    IN OUT  UINT8 *ReadBuffer,
    IN  UINT16 RLength
);

typedef EFI_STATUS (EFIAPI *I2CWRITE) (
    IN  UINT8 BusNo,
    IN  UINT8 SlaveAddress,
    IN  UINT8 *WriteBuffer,
    IN  UINT16 WLength
);

struct _EFI_AMI_I2C_PROTOCOL {
    I2CCOMBINREAD   CombinReadI2C;  // S,Addr+W,Data,Data+n,RS,Addr+R,Data,Data+n ;WLength=0 => S,Addr+R,Data,Data+n ;RLength=0 => S,Addr+W,Data,Data+n
    I2CREAD         I2CReadData;    // S,Addr+W,Data,Data+n,RS,Addr+R,Data,Data+n ;WLength=0 => S,Addr+R,Data,Data+n ;
    I2CWRITE        I2CWriteData;   // S,Addr+W,Data,Data+n,RS,Addr+R,Data,Data+n ;RLength=0 => S,Addr+W,Data,Data+n
};


#define AMI_I2C_POLICY_PROTOCOL_GUID  \
	{0xE10C45AC, 0xA6FD, 0x4732, { 0x84, 0x9, 0x3F, 0xBD, 0x54, 0xE5, 0x77, 0xA7}}

EFI_GUID gEfiAmiI2CProtocolGuid = AMI_I2C_POLICY_PROTOCOL_GUID;
#pragma pack()

#define I2C_ACPI_ID_LEN 16

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
