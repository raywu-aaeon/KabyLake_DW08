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

/** @file I2cController.c
    Create I2C controller template driver for read/write data to device.

**/

//---------------------------------------------------------------------------

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/I2cProtocol.h>
#include <Library/DebugLib.h>

//---------------------------------------------------------------------------

/**
    The function provide the template for read data from i2c device.

    @param[in] BusNo - Bus number of I2c device
    @param[in] SlaveAddress - Address of I2c device
    @param[in] ReadBuffer - Read data to I2c device
    @param[in] RLength - length for read data

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
AmiReadI2C (
    IN  UINT8 BusNo, 
    IN  UINT8 SlaveAddress, 
    IN  OUT  UINT8 *ReadBuffer, 
    IN  UINT16 RLength
)
{
    // Implement the I2c read function in here.

    return EFI_SUCCESS;
}

/**
    The function provide the template for write data to i2c device

    @param[in] BusNo - Bus number of I2c device
    @param[in] SlaveAddress - Address of I2c device
    @param[in] WriteBuffer - Write data to I2c device
    @param[in] WLength - length for write data

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
AmiWriteI2C (
    IN  UINT8 BusNo, 
    IN  UINT8 SlaveAddress, 
    IN  UINT8 *WriteBuffer, 
    IN  UINT16 WLength
)
{
    // Implement the I2c write function in here.

    return EFI_SUCCESS;
}

/**
    The function provide the read/write for i2c device

    @param[in] BusNo - Bus number of I2c device
    @param[in] SlaveAddress - Address of I2c device
    @param[in] WriteBuffer - Write data to I2c device
    @param[in, out] ReadBuffer - Read data to I2c device
    @param[in] WLength - length for write data
    @param[in] RLength - length for read data

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
AmiCombinReadI2C (
    IN  UINT8 BusNo, 
    IN  UINT8 SlaveAddress, 
    IN  UINT8 *WriteBuffer, 
    IN OUT  UINT8 *ReadBuffer, 
    IN  UINT16 WLength,
    IN  UINT16 RLength
)
{
    // Implement the I2c read/write function in here.

    // return I2CCombineRead(BusNo, SlaveAddress, Write_Buffer, Read_Buffer, WLength,RLength);
    return EFI_SUCCESS;
}

EFI_AMI_I2C_PROTOCOL  AmiI2CProtocol = {
    AmiCombinReadI2C,
    AmiReadI2C,
    AmiWriteI2C,
};

/**
    I2c controller entry point.

    @param[in] ImageHandle - Image handle for this driver
    @param[in] SystemTable - Pointer to the EFI system table

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
EFIAPI
I2cControllerEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    Status = gBS->InstallProtocolInterface(
                    &ImageHandle,
                    &gEfiAmiI2CProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &AmiI2CProtocol
    );

    ASSERT_EFI_ERROR(Status);

    return Status;
}

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
