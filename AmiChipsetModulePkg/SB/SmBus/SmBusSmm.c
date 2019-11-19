//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************

/** @file SmBusSmm.c
    SMBUS SMM functions implementation

**/

#include "SmBusDxeSmm.h"
#include <Protocol/SbSmiProtocol.h>

SMBUS_DXE_PRIVATE *gPrivate;


/**
    This Protocol Function can be used to execute SMBus command
    on a particular SMBus controller in SMM.

    @param *This        - Pointer to the SMBus Protocol structure
    @param SlaveAddress - Address of the SMBus device
    @param Command      - Command to be sent to the device
    @param Operation    - SMBus operation to be performed
    @param PecCheck     - Flag indicating the usage of PEC
    @param *Length      - Length of the data in the Buffer (IN or OUT)
    @param *Buffer      - Pointer to the buffer with the data (IN or OUT)

    @retval EFI_STATUS

    @note  PORTING REQUIRED
**/

EFI_STATUS SmBusSmmExecute (
    IN EFI_SMBUS_HC_PROTOCOL            *This,
    IN EFI_SMBUS_DEVICE_ADDRESS         SlaveAddress,
    IN EFI_SMBUS_DEVICE_COMMAND         Command,
    IN EFI_SMBUS_OPERATION              Operation,
    IN BOOLEAN                          PecCheck,
    IN OUT UINTN                        *Length,
    IN OUT VOID                         *Buffer )
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;
/*
    // Porting Required
    UINT16      SmBusBase = READ_PCI16_SMBUS(SMBUS_REG_BASE_ADDR) & 0xfff0;
    UINT16      SmBusCmd = READ_PCI16_SMBUS(SMBUS_REG_PCICMD);

    if (SmBusBase == 0) { // Assign a new I/O if the original address is 0
        WRITE_PCI16_SMBUS(SMBUS_REG_BASE_ADDR, SMBUS_BASE_ADDRESS);
        SmBusBase = SMBUS_BASE_ADDRESS;
    }

    gPrivate->SmBusContext.SmBusBase = SmBusBase;

    if ((SmBusCmd & 1) == 0) { // Enable I/O command if needed.
        WRITE_PCI16_SMBUS(SMBUS_REG_PCICMD, 1);
    }
    // Porting End

    Status = Execute( &(((SMBUS_DXE_PRIVATE *)This)->SmBusContext), \
                      SlaveAddress, \
                      Command, \
                      Operation, \
                      PecCheck, \
                      Length, \
                      Buffer );

    // Porting Required
    // Restore the SMBus PCI Command Register
    WRITE_PCI16_SMBUS(SMBUS_REG_PCICMD, SmBusCmd);
    // Porting End
*/
    return Status;
}

/**
    This function installs SMBus SMM protocol for the SMBus
    controller present in the SB.

    @param ImageHandle  - Image handle for the SB component
    @param *SystemTable - Pointer to the system table

    @retval EFI_STATUS

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmBusInSmmInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_HANDLE                  Handle = NULL;
    static SMBUS_DXE_PRIVATE    SmbusSmmPrivate;

    gPrivate = &SmbusSmmPrivate;
    gPrivate->SmBusProtocol.Execute   = SmBusSmmExecute;
    gPrivate->SmBusProtocol.ArpDevice = SmBusDxeArpDevice;
    gPrivate->SmBusProtocol.GetArpMap = SmBusDxeGetArpMap;
    gPrivate->SmBusProtocol.Notify    = SmBusDxeNotify;

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;

    return pSmst->SmmInstallProtocolInterface( &Handle, \
                                               &gAmiSmbusSmmProtocolGuid, \
                                               EFI_NATIVE_INTERFACE, \
                                               &gPrivate->SmBusProtocol );
}

/**
    This function installs SMBus DXE/SMM protocols for the SMBus
    controller present in the SB.

    @param ImageHandle  - Image handle for the SB component
    @param *SystemTable - Pointer to the system table

    @retval EFI_STATUS

    @note  GENERALLY NO PORTING REQUIRED
**/

EFI_STATUS SmBusSmmEntryPoint (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib(ImageHandle, SystemTable);

    return InitSmmHandlerEx( ImageHandle, \
                             SystemTable, \
                             SmBusInSmmInit, \
                             NULL );
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
