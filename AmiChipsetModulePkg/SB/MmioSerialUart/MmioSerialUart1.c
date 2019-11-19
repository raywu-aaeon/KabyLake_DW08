//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
/** @file MmioSerialUart1.c
    This file contains code for Mmio Serial Uart initialization.

**/

// Module specific Includes

#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>    // [ EIP397240 ]
#include <AmiDxeLib.h>
#include <Protocol/TerminalAmiSerial.h>
#include <Protocol/DevicePath.h>
#include <Library/PcdLib.h>
#include <Token.h>
#include <IndustryStandard/Pci22.h>
#include <Setup.h>

// Module specific Includes
#define SERIAL_UART0_MMIO 0
#define SERIAL_UART1_MMIO 1
#define SERIAL_UART2_MMIO 2

#define MmPciAddress( Bus, Device, Function, Register ) \
  ( (UINTN)(PcdGet64 (PcdPciExpressBaseAddress)) + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )
  
// Function Prototype(s)

VOID EFIAPI GetBaseAddress (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT64              *BaseAddress
); 

VOID EFIAPI CheckPciMmio(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT BOOLEAN             *Pci,
    OUT BOOLEAN             *Mmio,
    OUT UINT8               *MmioWidth
);

VOID EFIAPI GetPciLocation(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINTN               *Bus,
    OUT UINTN               *Dev,
    OUT UINTN               *Func,
    OUT UINT8               *Port
);

VOID EFIAPI GetSerialIRQ(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT8               *SerialIRQ
);

VOID EFIAPI GetUID(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT8               *Uid
);

VOID EFIAPI GetUartClock(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINTN              *Clock
);

// Variable Declaration(s)

EFI_HANDLE	 gHandle=NULL;

typedef struct {
  VENDOR_DEVICE_PATH        Guid;
  ACPI_HID_DEVICE_PATH	    AcpiDp;
  UART_DEVICE_PATH          Uart;
  EFI_DEVICE_PATH_PROTOCOL  End;
} SIMPLE_TEXT_OUT_DEVICE_PATH;

SIMPLE_TEXT_OUT_DEVICE_PATH mDevicePathSerialUart = {
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, sizeof (VENDOR_DEVICE_PATH), 0},
    EFI_CALLER_ID_GUID // Use the drivers GUID
  },
  { { ACPI_DEVICE_PATH, ACPI_DP, sizeof(ACPI_HID_DEVICE_PATH)}, EISA_PNP_ID(0x501), 0 },
  {
    { MESSAGING_DEVICE_PATH, MSG_UART_DP, sizeof (UART_DEVICE_PATH), 0},
    0,        // Reserved
    FixedPcdGet64 (PcdUartDefaultBaudRate),   // BaudRate
    FixedPcdGet8 (PcdUartDefaultDataBits),    // DataBits
    FixedPcdGet8 (PcdUartDefaultParity),      // Parity (N)
    FixedPcdGet8 (PcdUartDefaultStopBits)     // StopBits
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof (EFI_DEVICE_PATH_PROTOCOL), 0}
};

AMI_SERIAL_PROTOCOL gAmiSerialUart1Protocol = {
  GetBaseAddress,
  CheckPciMmio,
  GetPciLocation,
  GetSerialIRQ,
  GetUID,
  GetUartClock
};

/**
    Gets the base address for the Serial Port.

    @param AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
    @param UINT64 *BaseAddress - Pointer to return Base Address 

    @retval VOID
**/

VOID EFIAPI GetBaseAddress (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT64              *BaseAddress
) 
{
// [ EIP397240 ]
//    UINTN  PcieBase = 0;
//    UINT8  CommandReg  = 0;
    
//    if (SERIAL_UART_DEV_IS_MMIO) {
//      PcieBase = MmPciAddress (SERIAL_UART1_MMIO_AT_BUS, SERIAL_UART1_MMIO_AT_DEV, SERIAL_UART1_MMIO_AT_FUN, 0);
//      *BaseAddress = (MmioRead32 (PcieBase + PCI_BASE_ADDRESSREG_OFFSET + 8 * SERIAL_UART_DEV_BAR_NUMBER) & 0xFFFFF000);
    UINTN       PcieBase = 0;
    UINT8       CommandReg  = 0;
	BOOLEAN     SupportAbove4G = FALSE;
    UINT32      BaseAddressL32 = 0;
    UINT32      BaseAddressH32 = 0;
    

    if (SERIAL_UART_DEV_IS_MMIO) {	
		PcieBase = MmPciAddress (   SERIAL_UART1_MMIO_AT_BUS, 
                                    SERIAL_UART1_MMIO_AT_DEV, 
                                    SERIAL_UART1_MMIO_AT_FUN, 
                                    0);

		BaseAddressL32 = MmioRead32 ( 	PcieBase + 
										PCI_BASE_ADDRESSREG_OFFSET + 
										(8 * SERIAL_UART_DEV_BAR_NUMBER));

      	BaseAddressL32 &= (BIT01 | BIT02);

      	if(BaseAddressL32 == 4) { 
            SupportAbove4G = TRUE;
		} else {
			SupportAbove4G = FALSE;
		}

		if(SupportAbove4G) {
            BaseAddressL32 = MmioRead32 (
                                    PcieBase + 
                                    PCI_BASE_ADDRESSREG_OFFSET + 
                                    (8 * SERIAL_UART_DEV_BAR_NUMBER));
            BaseAddressL32 &= 0xFFFFF000;
            BaseAddressH32 = MmioRead32 (
                                    PcieBase + 
                                    PCI_BASE_ADDRESSREG_OFFSET + 
                                    (8 * SERIAL_UART_DEV_BAR_NUMBER) + 4 );
            *BaseAddress = ((UINT64)BaseAddressH32 << 32) | (BaseAddressL32);
		} else {
			*BaseAddress = MmioRead32 (
                                    PcieBase + 
                                    PCI_BASE_ADDRESSREG_OFFSET + 
                                    (8 * SERIAL_UART_DEV_BAR_NUMBER));
            *BaseAddress &= 0xFFFFF000;
		}

      CommandReg = MmioRead8 (PcieBase + PCI_COMMAND_OFFSET);
      if(!(CommandReg & EFI_PCI_COMMAND_MEMORY_SPACE)){
	      MmioWrite8 (PcieBase + PCI_COMMAND_OFFSET, (CommandReg | EFI_PCI_COMMAND_MEMORY_SPACE));
      } 
    } else {
      *BaseAddress = SERIAL_UART_IO_BASE;
    }


    return;
}

/**
    Gives information whether it is a pci serial device, if yes,
    tells whether it is a MMIO device.

    @param AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
    @param BOOLEAN             *Pci  -  BOOLEAN to fill whether it is PCI device
    @param BOOLEAN             *Mmio - BOOLEAN to fill whether it is MMIO access
	
    @retval VOID
**/

VOID EFIAPI CheckPciMmio(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT BOOLEAN             *Pci,
    OUT BOOLEAN             *Mmio,
    OUT UINT8               *MmioWidth
)
{
    *Pci = (SERIAL_UART_DEV_IS_MMIO) ? TRUE : FALSE;
    *Mmio = (SERIAL_UART_DEV_IS_MMIO) ? TRUE : FALSE;
    *MmioWidth = PCI_SERIAL_MMIO_WIDTH; 
    return;
}

/**
    This Function will give the Bus/Dev/func if it is a PCI Serial Device.

    @param AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
    @param UINTN               *Bus - Pointer to return Bus No.
    @param UINTN               *Dev - Pointer to return Device No.
    @param UINTN               *Func- Pointer to return Function No.
    @param UINT8               *Port- Pointer to return Port No.
	
    @retval VOID
**/

VOID EFIAPI GetPciLocation(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINTN               *Bus,
    OUT UINTN               *Dev,
    OUT UINTN               *Func,
    OUT UINT8               *Port
)
{
    *Bus  = SERIAL_UART_DEV_IS_MMIO ? SERIAL_UART1_MMIO_AT_BUS : 0xff;
    *Dev  = SERIAL_UART_DEV_IS_MMIO ? SERIAL_UART1_MMIO_AT_DEV : 0xff;
    *Func = SERIAL_UART_DEV_IS_MMIO ? SERIAL_UART1_MMIO_AT_FUN : 0xff;
    *Port = SERIAL_UART_DEV_IS_MMIO ? 1 : 0;

    return;
}

/**
    Gets the IRQ number for the Serial Port.

    @param AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
    @param UINT8               *SerialIRQ - Pointer to return IRQ number.
	
    @retval VOID
**/

VOID EFIAPI GetSerialIRQ(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT8               *SerialIRQ
)
{
    EFI_STATUS    Status;
    UINTN         PcieBase = 0;
    SETUP_DATA    SetupData;
    UINTN         VariableSize;
    EFI_GUID      SetupGuid = SETUP_GUID;
    
    VariableSize = sizeof(SETUP_DATA);
// [ EIP397240 ]    
//    Status = pRS->GetVariable(L"Setup", &SetupGuid, NULL,
//                              &VariableSize, &SetupData );
    Status = gRT->GetVariable(L"Setup", &SetupGuid, NULL,
                              &VariableSize, &SetupData );
    if(EFI_ERROR(Status)) return;
    
    if (SERIAL_UART_DEV_IS_MMIO) {
      PcieBase = MmPciAddress (SERIAL_UART1_MMIO_AT_BUS, SERIAL_UART1_MMIO_AT_DEV, SERIAL_UART1_MMIO_AT_FUN, 0);
      *SerialIRQ = MmioRead8 (PcieBase + PCI_INT_LINE_OFFSET);
    } else {
      *SerialIRQ = SERIAL_UART_IRQ;
    }
    return;
}

/**
    Gets the UID for the Serial Port.

    @param AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
    @param UINT8               *Uid - Pointer to return Uid.
	
    @retval VOID
**/

VOID EFIAPI GetUID(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT8               *Uid
)
{
    static UINT8            AmiSerialUid = 10;

    if (SERIAL_UART_DEV_IS_MMIO) {
      *Uid = 1;
    } else {
      *Uid = AmiSerialUid;
      AmiSerialUid ++;
    }

    return;
}

/**
    Gets the UID for the Serial Port.

    @param AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
    @param UINT8               *Uid - Pointer to return Uid.
    
    @retval VOID
**/

VOID EFIAPI GetUartClock(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINTN              *Clock
)
{
    *Clock = 0;
    
#ifdef PCI_UART_INPUT_CLOCK
    *Clock = PCI_UART_INPUT_CLOCK;
#endif
    
    if ( *Clock == 0x0)
        *Clock = 1843200;

    return ;
}

/**
    Entry point for AMI Serial Driver.It installs AMI Serial DriverBinding
    Protocol.

    @param EFI_HANDLE ImageHandle - The image handle.
    @param EFI_SYSTEM_TABLE *SystemTable - A pointer to the EFI system table.
	
    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI
SerialUart1EntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{

  EFI_STATUS    Status;

  //
  // Install AmiSerial protocol interfaces for the serial device.
  //
  Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
              &gHandle, 
              &gAmiSerialProtocolGuid,   &gAmiSerialUart1Protocol,
              &gEfiDevicePathProtocolGuid, &mDevicePathSerialUart, 
              NULL
              );
  ASSERT_EFI_ERROR (Status);
 
  return Status;
}
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
