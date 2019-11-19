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
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************************

/** @file SmBusPei.h
    This file contains PEI SMBUS Driver functions and data structures definition

**/

#ifndef __SMBUS_PEI__H__
#define __SMBUS_PEI__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Ppi/Smbus2.h>
#include <Library/SmBusCommon.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <AmiLib.h>
#include <AmiHobs.h>

#define MAX_PEI_ARP_DEVICES 8

#pragma pack(1)

/**
    AMI SMBUS driver PEI private data structure

 Fields: Name         Type                        Description
 SmBusPpi             EFI_PEI_SMBUS/2_PPI         SMBUS (2) PPI structure
 Identifier           EFI_GUID                    SMBUS controller identifier
 SmBusContext         SMBUS_PRIVATE               SMBUS private data structure
 NotifyDesc           EFI_PEI_NOTIFY_DESCRIPTOR   Notify descriptor structure
 SmBusPpiDesc         EFI_PEI_PPI_DESCRIPTOR      PPI descriptor structure
 
**/
typedef struct _SMBUS_PEI_PRIVATE
{
    EFI_PEI_SMBUS2_PPI        SmBusPpi;
    SMBUS_PRIVATE             SmBusContext;
    EFI_PEI_NOTIFY_DESCRIPTOR NotifyDesc;
    EFI_PEI_PPI_DESCRIPTOR    SmBusPpiDesc;
} SMBUS_PEI_PRIVATE;

#pragma pack()

EFI_STATUS SmBusPeiExecute (
    IN CONST EFI_PEI_SMBUS2_PPI *This,
    IN EFI_SMBUS_DEVICE_ADDRESS SlaveAddress,
    IN EFI_SMBUS_DEVICE_COMMAND Command,
    IN EFI_SMBUS_OPERATION      Operation,
    IN BOOLEAN                  PecCheck,
    IN OUT UINTN                *Length,
    IN OUT VOID                 *Buffer
);

EFI_STATUS SmBusPeiArpDevice (
    IN CONST EFI_PEI_SMBUS2_PPI     *This,
    IN BOOLEAN                      ArpAll,
    IN EFI_SMBUS_UDID               *SmbusUdid, OPTIONAL
    IN OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress OPTIONAL
);

EFI_STATUS SmBusPeiGetArpMap (
    IN CONST EFI_PEI_SMBUS2_PPI     *This,
    IN OUT UINTN                    *Length,
    IN OUT EFI_SMBUS_DEVICE_MAP     **SmbusDeviceMap
);

EFI_STATUS SmBusPeiNotify (
    IN CONST EFI_PEI_SMBUS2_PPI       *This,
    IN EFI_SMBUS_DEVICE_ADDRESS       SlaveAddress,
    IN UINTN                          Data,
    IN EFI_PEI_SMBUS_NOTIFY2_FUNCTION NotifyFunction
);

EFI_STATUS SmBusEndOfPeiCallback (
    IN CONST EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
    IN VOID                       *Ppi
);

VOID SmBusPeiInitialize(
    IN CONST EFI_PEI_SERVICES     **PeiServices,
    IN OUT SMBUS_PRIVATE          *Context
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

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
