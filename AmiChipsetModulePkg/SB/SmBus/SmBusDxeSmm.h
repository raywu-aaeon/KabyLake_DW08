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

/** @file SmBusDxeSmm.h
    This file contains DXE/SMM SMBUS Driver functions and data
    structures definition.

**/
#ifndef __SMBUS_DXE_SMM__H__
#define __SMBUS_DXE_SMM__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <AmiDxeLib.h>
#include <Protocol/Smbus.h>
#include <Protocol/PciIo.h>
#include <Library/SmBusCommon.h>
#include <Protocol/DriverBinding.h>


#define MAX_DXE_ARP_DEVICES 0x30

#pragma pack(1)

/**
    AMI SMBUS driver PEI private data structure

 Fields: Name         Type                        Description
 SmBusProtocol        EFI_SMBUS_HC_PROTOCOL       SMBUS host controller protocol structure
 Identifier           EFI_GUID                    SMBUS host controller identifier
 SmBusContext         SMBUS_PRIVATE               SMBUS private data structure
 PciIo                EFI_PCI_IO_PROTOCOL*        Pointer to PCI IO protocol of SMBUS device
 NotifyList           DLIST                       Linked list of notify callbacks
 NotifyEvent          EFI_EVENT                   EFI_EVENT structure
 
**/
typedef struct _SMBUS_DXE_PRIVATE
{
    EFI_SMBUS_HC_PROTOCOL    SmBusProtocol;
    EFI_GUID                 Identifier;    
    SMBUS_PRIVATE            SmBusContext;
    EFI_PCI_IO_PROTOCOL      *PciIo;
    DLIST                    NotifyList;
    EFI_EVENT                NotifyEvent;
} SMBUS_DXE_PRIVATE;

#pragma pack()

typedef struct {
    DLINK                         Link;
    EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress;
    UINTN                         Data;
    EFI_SMBUS_NOTIFY_FUNCTION     NotifyFunction;
} SMBUS_NOTIFY_LINK;    

EFI_STATUS SmBusDxeExecute (
    IN CONST  EFI_SMBUS_HC_PROTOCOL       *This,
    IN CONST  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress,
    IN CONST  EFI_SMBUS_DEVICE_COMMAND    Command,
    IN CONST  EFI_SMBUS_OPERATION         Operation,
    IN CONST  BOOLEAN                     PecCheck,
    IN OUT    UINTN                       *Length,
    IN OUT    VOID                        *Buffer
);

EFI_STATUS SmBusDxeArpDevice (
    IN CONST EFI_SMBUS_HC_PROTOCOL        *This,
    IN CONST BOOLEAN                      ArpAll,
    IN CONST EFI_SMBUS_UDID               *SmbusUdid, OPTIONAL
    IN OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress OPTIONAL
);

EFI_STATUS SmBusDxeGetArpMap (
    IN CONST  EFI_SMBUS_HC_PROTOCOL       *This,
    IN OUT    UINTN                       *Length,
    IN OUT    EFI_SMBUS_DEVICE_MAP        **SmbusDeviceMap
);

EFI_STATUS SmBusDxeNotify (
    IN CONST EFI_SMBUS_HC_PROTOCOL        *This,
    IN CONST EFI_SMBUS_DEVICE_ADDRESS     SlaveAddress,
    IN CONST UINTN                        Data,
    IN CONST EFI_SMBUS_NOTIFY_FUNCTION    NotifyFunction
);

EFI_STATUS  DriverBindingSupported ( 
    IN EFI_DRIVER_BINDING_PROTOCOL        *This,
    IN EFI_HANDLE                         ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
);

EFI_STATUS  DriverBindingStart ( 
    IN EFI_DRIVER_BINDING_PROTOCOL        *This,
    IN EFI_HANDLE                         ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
);

EFI_STATUS  DriverBindingStop ( 
    IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
    IN  EFI_HANDLE                        ControllerHandle,
    IN  UINTN                             NumberOfChildren,
    IN  EFI_HANDLE                        *ChildHandleBuffer
);

EFI_STATUS InitializeNotifyPolling (
    IN SMBUS_DXE_PRIVATE *Context
);

VOID PollSmbusNotify (
    IN  EFI_EVENT Event,
    IN  VOID      *Context
);

VOID RetrieveHobData (
    IN OUT SMBUS_DXE_PRIVATE *Context
);

/*
VOID SmBusDxeInitialize(
    IN SMBUS_PRIVATE *Context
    );
*/

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
