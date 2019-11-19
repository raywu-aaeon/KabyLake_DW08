//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2012, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
	
//****************************************************************************
// $Header:$
//
// $Revision:$
//
// $Date:$
//
//****************************************************************************
// Revision History
// ----------------
// $Log:$
// 
//****************************************************************************
//<AMI_FHDR_START>
//
// Name: I2cController.c
//
// Description: create I2C controller template driver for read/write data to device
//
//<AMI_FHDR_END>
//****************************************************************************

//============================================================================
// Includes
//============================================================================
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/I2cProtocol.h>
#include <AmiDxeLib.h>
#include <Library/I2cAccessLib.h>
#include <Library/PchSerialIoLib.h>

//============================================================================
// Define
//============================================================================


//============================================================================
// External Golbal Variable Declaration
//============================================================================

//============================================================================
// Golbal Variable Declaration
//============================================================================

//============================================================================
// External Function Definitions
//============================================================================

//============================================================================
// Function Definitions
//============================================================================

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   I2CCombineRead  
//
// Description: The function provide the read/write for i2c device
//   
//
// Input:
//	IN  UINT8 BusNo 	   - Bus number of I2c device 
//	IN  UINT8 SlaveAddress     - Address of I2c device
//	IN  UINT8 Write_Buffer     - write command to I2c deivce
//	IN OUT  UINT8 Read_Buffer  - write data to I2c device
//	IN  UINT16 UINT16 WLength  - length for writer data
//	IN  UINT16 UINT16 RLength  - length for read data
//
// Output:
//	IN OUT  UINT8 Read_Buffer  - read data from I2c device
//	EFI_STATUS
// Notes:
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS I2CCombineRead(UINT8 BusNo, UINT8 SlaveAddress, UINT8 *Write_Buffer, UINT8 *Read_Buffer, UINTN WLength,UINTN RLength)
{
	EFI_STATUS      Status;
	UINTN 		MmioBase; 

	//We set I2CHID_DEV_0_BUS_NUMBER=5, I2CHID_DEV_1_BUS_NUMBER=6 ... and so on
	MmioBase = FindSerialIoBar(BusNo-5, 0); 

// [ EIP359738 ]	
//	Status = I2cWriteRead (MmioBase, SlaveAddress, (UINT8)WLength, Write_Buffer, 
//				(UINT8)RLength, Read_Buffer, WAIT_1_SECOND);
    Status = I2cWriteRead (MmioBase, SlaveAddress, (UINT16)WLength, Write_Buffer, 
                (UINT16)RLength, Read_Buffer, WAIT_1_SECOND);    
	
	DEBUG((DEBUG_INFO, "I2CCombineRead: %r\n",Status));
	
	return Status;
}


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   AmiReadI2C  
//
// Description: The function provide the template for read data from i2c device
//   
// Input:
//	IN  UINT8 BusNo 	   - Bus number of I2c device 
//	IN  UINT8 SlaveAddress     - Address of I2c device
//	IN OUT  UINT8 Read_Buffer  - write data to I2c device
//	IN  UINT16 UINT16 RLength  - length for read data
//
// Output:
//	EFI_STATUS
// Notes:
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AmiReadI2C(
	      IN  UINT8 BusNo, 
	      IN  UINT8 SlaveAddress, 
	      IN  OUT  UINT8 *Read_Buffer, 
	      IN  UINT16 RLength
)
{
	return I2CCombineRead(BusNo, SlaveAddress, NULL, Read_Buffer, 0, RLength);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   AmiWriteI2C  
//
// Description: The function provide the template for write data to i2c device
//   
// Input:
//	IN  UINT8 BusNo 	   - Bus number of I2c device 
//	IN  UINT8 SlaveAddress     - Address of I2c device
//	IN  UINT8 Write_Buffer     - write command to I2c deivce
//	IN  UINT16 UINT16 WLength  - length for writer data
//
// Output:
//	EFI_STATUS
// Notes:
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AmiWriteI2C(
	      IN  UINT8 BusNo, 
	      IN  UINT8 SlaveAddress, 
	      IN  UINT8 *Write_Buffer, 
	      IN  UINT16 WLength
)
{
	return I2CCombineRead(BusNo, SlaveAddress, Write_Buffer, NULL, WLength, 0);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   AmiCombinReadI2C  
//
// Description: The function provide the read/write for i2c device
//   
//
// Input:
//	IN  UINT8 BusNo 	   - Bus number of I2c device 
//	IN  UINT8 SlaveAddress     - Address of I2c device
//	IN  UINT8 Write_Buffer     - write command to I2c deivce
//	IN OUT  UINT8 Read_Buffer  - write data to I2c device
//	IN  UINT16 UINT16 WLength  - length for writer data
//	IN  UINT16 UINT16 RLength  - length for read data
//
// Output:
//	IN OUT  UINT8 Read_Buffer  - read data from I2c device
//	EFI_STATUS
// Notes:
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AmiCombinReadI2C(
	      IN  UINT8 BusNo, 
	      IN  UINT8 SlaveAddress, 
	      IN  UINT8 *Write_Buffer, 
	      IN OUT  UINT8 *Read_Buffer, 
	      IN  UINT16 WLength,
	      IN  UINT16 RLength
)
{
	return I2CCombineRead(BusNo, SlaveAddress, Write_Buffer, Read_Buffer, WLength,RLength);
}



EFI_AMI_I2C_PROTOCOL  AmiI2CProtocol = {
    AmiCombinReadI2C,
    AmiReadI2C,
    AmiWriteI2C,    
};
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   I2cControllerEntryPoint  
//
// Description: 
//  I2c controller entry point.
//
// Input:       
//  IN EFI_HANDLE           ImageHandle - Image handle for this driver
//  IN EFI_SYSTEM_TABLE     *SystemTable - Pointer to the EFI system table.
//
// Output: 
//        EFI_SUCCESS - The function completed successfully.
//
// Notes:
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS I2cControllerEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status = EFI_SUCCESS;

    InitAmiLib (ImageHandle, SystemTable);
  

    Status = pBS->InstallProtocolInterface(
                 	&ImageHandle,
                 	&gEfiAmiI2CProtocolGuid,
			EFI_NATIVE_INTERFACE,
			&AmiI2CProtocol
             );


    ASSERT_EFI_ERROR(Status);

    return Status;
}
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2012, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
