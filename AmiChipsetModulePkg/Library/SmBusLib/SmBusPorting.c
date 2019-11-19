//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SmBusPorting.c
    SMBUS driver porting functions

**/

#include "Library/SmBusCommon.h"
#include <Sb.h>

//Porting Required - Put unique GUID for given SMBUS controller
#define SM_BUS_CONTROLLER_IDENTIFIER_GUID \
  {0x882f2546, 0xef1f, 0x4090, 0x9f, 0x9c, 0x93, 0x84, 0x5a, 0xd7, 0x84, 0x1c}

EFI_GUID SmBusIdentifierGuid = SM_BUS_CONTROLLER_IDENTIFIER_GUID;

UINT8 SmBusPlatformReservedAddress[] = {
  NB_DIMM0_SMBUS_ADDRESS, 
  NB_DIMM1_SMBUS_ADDRESS, 
  NB_DIMM2_SMBUS_ADDRESS, 
  NB_DIMM3_SMBUS_ADDRESS
};

UINT8 SmBusPlatformReservedAddressSize = \
                        sizeof(SmBusPlatformReservedAddress) / sizeof(UINT8); 

// GUID Definition(s)

// Protocol/PPI Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

/**
    Set an enviornment for SMBus transfering.

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
    @param Protocol8     - SMBus operation to be performed

        
    @retval EFI_TIMEOUT The caller can't obtain ownership of SMBus.
**/

EFI_STATUS BeforeSMBusTransfer (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            Protocol8 )
{
    UINT16               Timeout = 0xffff;

    // Waiting for other software's usage.
    while (IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_STS) & HST_STS_INUSE_STS) {
        IoRead8(0xed); // I/O Delay
        Timeout--;
        if (Timeout == 0) return EFI_TIMEOUT; 
    }

    // BIOS obtains ownership of SMBus & Waiting for transmission completed.
    while (IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_STS) & HST_STS_HOST_BUSY) {
        Timeout--;
        if (Timeout == 0) break; 
    }

    // Clears all statues
    IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_STS, HST_STS_ALL);

    if (Protocol8 == SMB_CMD_BLOCK) {
//#### SET_IO8(SmBusIoAddr16 + SMB_IOREG_AUX_CTL, AUX_CTL_E32B); // 0x0D
//#### // Reset Buffer Pointer
//#### IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_CNT); // 0x02
    }

    return EFI_SUCCESS;
}

/**
    This support function writes command field to SMBus Controller

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
    @param Command8      - SMBus command to be performed

    @retval VOID
**/

VOID WriteCommand (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            Command8 )
{
    IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_CMD, Command8);
}

/**
    This support function writes slave address to SMBus Controller

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
    @param SlAddr8       - Address of the SMBus device

    @retval VOID
**/

VOID WriteSlAddr (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            SlAddr8 )
{
    IoWrite8(SmBusIoAddr16 + SMB_IOREG_XMIT_SLVA, SlAddr8);
}

/**
    This support function writes data(s) to SMBus Controller for
    SMBus write operation.

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
    @param Protocol8     - SMBus operation to be performed
    @param Length8       - Size of data buffer in bytes
    @param *Data8        - Buffer to be written

    @retval VOID
**/

VOID WriteSMBusData (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            Protocol8,
    IN UINT8            Length8,
    IN UINT8            *Data8 )
{
    UINT8               i;

    for (i = 0; i < Length8; i++) {
        if (Protocol8 == SMB_CMD_BLOCK) {
            IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_D0, Length8);
            IoWrite8(SmBusIoAddr16 + SMB_IOREG_HOST_BLOCK_DB, *Data8++);
            break;
        } else {
            IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_D0 + i, *Data8++);
        }
    }
}

/**
    This support function waits the Byte Done bit to be set for
    SMBus Block operation.

    @param SmBusStsReg - 16 Bit I/O address for SMBus status register

        
    @retval EFI_DEVICE_ERROR An error on the SMBus device.
    @retval EFI_SUCCESS The Byte Done bit had been set.
**/

EFI_STATUS WaitForHostByteDoneStatus (
    IN UINT16           SmBusStsReg )
{
    UINT8           HostSts;

    while (1) {
        HostSts = IoRead8( SmBusStsReg );
        IoWrite8( IO_DELAY_PORT, HostSts );
        if ( HostSts & HST_STS_ERROR ) return EFI_DEVICE_ERROR;
        if ( HostSts & HST_STS_BDS ) return EFI_SUCCESS;
    }
}

/**
    This support function writes to SMBus Controller

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
    @param Length8       - Size of data buffer in bytes
    @param *Data8        - Buffer to be written

        
    @retval EFI_DEVICE_ERROR An error on the SMBus device.
    @retval EFI_SUCCESS Write SMBus Block data successfully.
**/

EFI_STATUS WriteSmBusBlockData (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            Length8,
    IN UINT8            *Data8 )
{
    EFI_STATUS          Status;
    UINT8               i;
    UINT16              StatusReg = SmBusIoAddr16 + SMB_IOREG_HST_STS;

    for (i = 1; i < Length8; i++) {
        if (WaitForHostByteDoneStatus(StatusReg)) return EFI_DEVICE_ERROR;
        IoWrite8(SmBusIoAddr16 + SMB_IOREG_HOST_BLOCK_DB, *Data8++);
        IoWrite8(StatusReg, HST_STS_BDS);
    }
    Status = WaitForHostByteDoneStatus(StatusReg);
    IoWrite8(StatusReg, HST_STS_BDS);
    return Status;
}

/**
    This support function reads from SMBus Controller

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
    @param Protocol8 - SMBus operation to be performed
    @param Length8 - Size of data buffer in bytes
    @param *Data8 - Buffer for the read data

        
    @retval EFI_DEVICE_ERROR An error on the SMBus device.
    @retval EFI_SUCCESS Read SMBus data successfully.

**/

EFI_STATUS ReadSmBusData (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            Protocol8,
    IN UINT8            Length8,
    OUT UINT8           *Data8 )
{
    UINT8               i;

    if (Protocol8 == SMB_CMD_BLOCK)
        Length8 = IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_D0);

    for (i = 0; i < Length8; i++) {
        if (Protocol8 == SMB_CMD_BLOCK) {
            if (WaitForHostByteDoneStatus(SmBusIoAddr16 + SMB_IOREG_HST_STS))
                return EFI_DEVICE_ERROR;
            *Data8++ = IoRead8(SmBusIoAddr16 + SMB_IOREG_HOST_BLOCK_DB);
            if (i == (Length8 - 1)) {
                IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_CNT, \
                                           HST_CNT_LAST_BYTE | SMB_CMD_BLOCK);
            }
            IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_STS, HST_STS_BDS);
        } else {
            *Data8++ = IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_D0 + i);
        }
    }
    return EFI_SUCCESS;
}

/**
    This support function starts SMBus operation.

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
    @param Protocol8 - SMBus operation to be performed
    @param PecCheck - TRUE will set PecCheck bit.

 	@retval VOID
**/

VOID StartSmBusTransition (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            Protocol8,
    IN BOOLEAN          PecCheck )
{
    UINT8               Buffer8;

    Buffer8 = Protocol8;
    if (PecCheck) {
        Buffer8 |= HST_CNT_PEC_EN;
        SET_IO8(SmBusIoAddr16 + SMB_IOREG_AUX_CTL, AUX_CTL_AAC); // 0x0D
    }
    IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_CNT, Buffer8);

    Buffer8 |= HST_CNT_START;

    if (Protocol8 == SMB_CMD_BLOCK)
        // Clear SECOND_TO_STS status before SMBus block read/write.
        WRITE_IO8_TCO(TCO_IOREG_STS2, 2); // 0x06

    IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_CNT, Buffer8);
}

/**
    This support function waits for the operation complete

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller

        
    @retval EFI_DEVICE_ERROR An error on the SMBus device.
    @retval EFI_SUCCESS SMBus transaction is successfully
        completed.
**/

EFI_STATUS WaitForSmBusComplete (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            Protocol8 )
{
    volatile UINT8      Buffer8;
    UINT8               StsChkBit;
    UINT16              TimeoutCounter;

    for (TimeoutCounter = 0; TimeoutCounter < 0x6000; TimeoutCounter++) {
        IoRead8(IO_DELAY_PORT); // I/O Delay
        Buffer8 = IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_STS);
        if (Buffer8 & (HST_STS_BDS | HST_STS_ERROR | HST_STS_INTR)) break;
    }

    StsChkBit = (Protocol8 == SMB_CMD_BLOCK) ? HST_STS_BDS : HST_STS_INTR;
    if (Buffer8 & StsChkBit)
        return (Buffer8 & HST_STS_ERROR) ? EFI_DEVICE_ERROR : EFI_SUCCESS;
    return EFI_DEVICE_ERROR;
}

/**
    This support function clears all statues of the SMBus
    controller

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller

 	@retval VOID
**/

VOID ClearSmBusStatusAndDelay (
    IN UINT16           SmBusIoAddr16 )
{
    UINT16              Timeout = 0x4000;
    volatile UINT8      HstSts;

    // Waiting for transmission completed.
    do {
        Timeout--;
        if (Timeout == 0) break; 
        HstSts = IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_STS);
        if (HstSts & HST_STS_HOST_BUSY) continue;
    } while ((HstSts & (HST_STS_ERROR | HST_STS_INTR | HST_STS_BDS)) == 0);

    // Clears all statues.
    IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_STS, HST_STS_ALL);
}

/**
    Restore the enviornment.

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
        Protocol8     - SMBus operation to be performed

    @retval VOID
**/

VOID AfterSMBusTransfer (
    IN UINT16           SmBusIoAddr16,
    IN UINT8            Protocol8 )
{
//#### if (Protocol8 == SMB_CMD_BLOCK)
//####    RESET_IO8(SmBusIoAddr16 + SMB_IOREG_AUX_CTL, AUX_CTL_E32B); // 0x0D
    RESET_IO8(SmBusIoAddr16 + SMB_IOREG_AUX_CTL, AUX_CTL_AAC); // 0x0D

    // BIOS releases the ownership of SMBus.    
    IoWrite8(SmBusIoAddr16 + SMB_IOREG_HST_STS, HST_STS_INUSE_STS);

}

/**
    This support function reads/writes from/to SMBus Controller.

    @param SmBusIoAddr16 - I/O base address of the SMBus Controller
    @param SlAddr8       - Address of the SMBus device
    @param Command8      - SMBus command to be performed
    @param Protocol8     - SMBus operation to be performed
    @param PecCheck - TRUE will set PecCheck bit.
    @param Length8       - Size of data buffer in bytes
    @param Data8         - Buffer for the read/write data(s)

    @retval Return Status based on errors that occurred while SMBus
        transaction.
**/

EFI_STATUS RwSmBusData (
    IN UINT16               SmBusIoAddr16,
    IN UINT8                SlAddr8,
    IN UINT8                Command8,
    IN UINT8                Protocol8,
    IN BOOLEAN              PecCheck,
    IN UINT8                Length8,
    IN OUT UINT8            *Data8 )
{
    EFI_STATUS          Status;
    BOOLEAN             IsWriteOperation = (!(SlAddr8 & XMIT_SLVA_RW));

    Status = BeforeSMBusTransfer(SmBusIoAddr16, Protocol8);
    if (Status != EFI_SUCCESS) return Status;
    WriteCommand(SmBusIoAddr16, Command8);
    WriteSlAddr(SmBusIoAddr16, SlAddr8);
    if (IsWriteOperation)
        WriteSMBusData(SmBusIoAddr16, Protocol8, Length8, Data8);
    StartSmBusTransition(SmBusIoAddr16, Protocol8, PecCheck);
    Status = WaitForSmBusComplete(SmBusIoAddr16, Protocol8);
    if (Status == EFI_SUCCESS) {
        if (IsWriteOperation) {
            if (Protocol8 == SMB_CMD_BLOCK)
                WriteSmBusBlockData(SmBusIoAddr16, Length8, &Data8[1]);
        } else {
            Status = ReadSmBusData(SmBusIoAddr16, Protocol8, Length8, Data8);
        }
    }

    ClearSmBusStatusAndDelay(SmBusIoAddr16);
    AfterSMBusTransfer(SmBusIoAddr16, Protocol8);

    return Status;
}

/**
    This function sends commands via SMBUS interface

    @param Context SMBUS device private data
    @param SlaveAddress slave address value
    @param Command command
    @param Operation operation
    @param PecCheck parity check flag
    @param Length pointer to size of data buffer
    @param Buffer pointer to data buffer

    @retval EFI_STATUS

    @note  Porting required

**/
EFI_STATUS Execute (
    IN     SMBUS_PRIVATE            *Context,
    IN     EFI_SMBUS_DEVICE_ADDRESS SlaveAddress,
    IN     EFI_SMBUS_DEVICE_COMMAND Command,
    IN     EFI_SMBUS_OPERATION      Operation,
    IN     BOOLEAN                  PecCheck,
    IN OUT UINTN                    *Length,
    IN OUT VOID                     *Buffer
)
{
    // Porting required - implement internal Smbus protocols here
    EFI_STATUS      Status = EFI_UNSUPPORTED;
    UINT8           *bData = (UINT8 *)Buffer; 
    UINT8           bSlAddr = (UINT8)(SlaveAddress.SmbusDeviceAddress << 1);
    UINT8           bCommand = (UINT8)Command; 
    UINT8           bLength = (UINT8)(*Length);
    UINT16          SmBusIoAddr16;
   
    if (Context == NULL) {
        SmBusIoAddr16 = MmioRead16((UINTN) SMBUS_PCIE_CFG_ADDRESS(R_SB_SMBUS_BASE_ADDR)) & 0xFFF0;
    } else {
        SmBusIoAddr16 = Context->SmBusBase;
    }

    // Wait for SMBus transaction to finish if needed.
//#### while (IoRead8(SmBusIoAddr16 + SMB_IOREG_CNTL) & 3);

    if (bLength > 32) bLength = 32;
    if (bLength == 0) return EFI_INVALID_PARAMETER;

    switch (Operation) {
            case EfiSmbusQuickRead:
            case EfiSmbusQuickWrite:
                break;
            
            case EfiSmbusReceiveByte:
                break;

            case EfiSmbusSendByte:
                Status = RwSmBusData(SmBusIoAddr16, \
                                      bSlAddr, \
                                      bCommand, \
                                      SMB_CMD_BYTE, \
                                      PecCheck, \
                                      1, \
                                      bData );
                break;
        
            case EfiSmbusReadByte:
                Status = RwSmBusData(SmBusIoAddr16, \
                                      (bSlAddr | XMIT_SLVA_RW), \
                                      bCommand, \
                                      SMB_CMD_BYTE_DATA, \
                                      PecCheck, \
                                      1, \
                                      bData );
                break;
            case EfiSmbusReadWord:
                Status = RwSmBusData(SmBusIoAddr16, \
                                      (bSlAddr | XMIT_SLVA_RW), \
                                      bCommand, \
                                      SMB_CMD_WORD_DATA, \
                                      PecCheck, \
                                      2, \
                                      bData );
                break;
    
            case EfiSmbusWriteByte:
                Status = RwSmBusData(SmBusIoAddr16, \
                                      bSlAddr, \
                                      bCommand, \
                                      SMB_CMD_BYTE_DATA, \
                                      PecCheck, \
                                      1, \
                                      bData );
                break;
            case EfiSmbusWriteWord:
                Status = RwSmBusData(SmBusIoAddr16, \
                                      bSlAddr, \
                                      bCommand, \
                                      SMB_CMD_WORD_DATA, \
                                      PecCheck, \
                                      2, \
                                      bData );
                break;  
    
            case EfiSmbusReadBlock:
                Status = RwSmBusData(SmBusIoAddr16, \
                                      (bSlAddr | XMIT_SLVA_RW), \
                                      bCommand, \
                                      SMB_CMD_BLOCK, \
                                      PecCheck, \
                                      bLength, \
                                      bData );
                *Length = IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_D0 );
                break;
    
            case EfiSmbusWriteBlock:
                Status = RwSmBusData(SmBusIoAddr16, \
                                      bSlAddr, \
                                      bCommand, \
                                      SMB_CMD_BLOCK, \
                                      PecCheck, \
                                      bLength, \
                                      bData );
                *Length = IoRead8(SmBusIoAddr16 + SMB_IOREG_HST_D0 );
                break;
            
            case EfiSmbusProcessCall:
                break;
            
            case EfiSmbusBWBRProcessCall:
                break;
            
            default:
                break;
    }

    return Status;
}

/**
    This function checks if SMBUS host received any notifications

    @param Context SMBUS device private data
    @param SlaveAddress pointer to return address of notificaion device
    @param Data pointer to notification data

    @retval EFI_STATUS

    @note  Porting required

**/
EFI_STATUS CheckNotify (
    IN  SMBUS_PRIVATE            *Context,
    OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress,
    OUT UINTN                    *Data
)
{
//Porting required
    return EFI_UNSUPPORTED;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
