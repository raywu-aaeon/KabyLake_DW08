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

/** @file SmBusDxeSmm.c
    SMBUS DXE/SMM functions implementation

**/

#include "SmBusDxeSmm.h"

#define MICROSECOND     10
#define MILLISECOND     (1000 * MICROSECOND)
#define ONESECOND       (1000 * MILLISECOND)


/**
    EFI_SMBUS_HC_PROTOCOL ArpDevice function

    @param This pointer to EFI_SMBUS_HC_PROTOCOL structure
    @param This pointer to PPI
    @param ArpAll Enumerate all devices flag
    @param SmbusUdid pointer to device ID to assign new address
    @param SlaveAddress pointer to return assigned address

    @retval EFI_STATUS

**/
EFI_STATUS SmBusDxeArpDevice (
    IN      EFI_SMBUS_HC_PROTOCOL         *This,
    IN      BOOLEAN                       ArpAll,
    IN      EFI_SMBUS_UDID                *SmbusUdid, OPTIONAL
    IN OUT  EFI_SMBUS_DEVICE_ADDRESS      *SlaveAddress OPTIONAL
)
{
    SMBUS_DXE_PRIVATE *Private = (SMBUS_DXE_PRIVATE *)This;

    return ArpDevice(
                &(Private->SmBusContext),
                ArpAll,
                SmbusUdid,
                SlaveAddress);
}

/**
    EFI_SMBUS_HC_PROTOCOL GetArpMap function

    @param This pointer to EFI_SMBUS_HC_PROTOCOL structure
    @param Length pointer to store size of address map
    @param SmbusDeviceMap pointer to store pointer to address map

    @retval EFI_STATUS

**/
EFI_STATUS SmBusDxeGetArpMap (
    IN      EFI_SMBUS_HC_PROTOCOL         *This,
    IN OUT  UINTN                         *Length,
    IN OUT  EFI_SMBUS_DEVICE_MAP          **SmbusDeviceMap
)
{
    SMBUS_DXE_PRIVATE *Private = (SMBUS_DXE_PRIVATE *)This;

    return GetArpMap(
                &(Private->SmBusContext),
                Length,
                SmbusDeviceMap);
}

/**
    EFI_SMBUS_HC_PROTOCOL Notify function

    @param This pointer to EFI_SMBUS_HC_PROTOCOL structure
    @param SlaveAddress address of notification device
    @param Data notification data
    @param NotifyFunction pointer to callback function

    @retval EFI_STATUS

**/
EFI_STATUS SmBusDxeNotify (
    IN      EFI_SMBUS_HC_PROTOCOL         *This,
    IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
    IN      UINTN                         Data,
    IN      EFI_SMBUS_NOTIFY_FUNCTION     NotifyFunction
)
{
    SMBUS_DXE_PRIVATE *Private = (SMBUS_DXE_PRIVATE *)This;
    SMBUS_NOTIFY_LINK *NewLink;
    EFI_STATUS        Status;

    if(NotifyFunction == NULL)
        return EFI_INVALID_PARAMETER;

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(SMBUS_NOTIFY_LINK), (VOID **)&NewLink);
    if(EFI_ERROR(Status))
        return Status;

    NewLink->SlaveAddress   = SlaveAddress;
    NewLink->Data           = Data;
    NewLink->NotifyFunction = NotifyFunction;

    DListAdd(&(Private->NotifyList), (DLINK *)NewLink);
    if(Private->NotifyList.Size == 1)
        Status = InitializeNotifyPolling(Private);

    return Status;
}

/**
    Function initializes host notify polling periodic event

    @param Context pointer to SMBUS device private data

    @retval EFI_STATUS

**/
EFI_STATUS InitializeNotifyPolling (
    IN SMBUS_DXE_PRIVATE *Context
)
{
    EFI_STATUS Status;

    Status = pBS->CreateEvent (
                  (EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL),
                  TPL_CALLBACK,
                  PollSmbusNotify, 
                  Context,
                  &Context->NotifyEvent );
    if (EFI_ERROR(Status))
        return Status;

    Status = pBS->SetTimer (
                  Context->NotifyEvent,
                  TimerPeriodic,
                  ONESECOND );
    if (EFI_ERROR(Status))
        return Status;

    return EFI_SUCCESS;
}

/**
    Function performs periodic check of host notifications

    @param Event periodic check event
    @param Context event calling context

    @retval VOID

**/
VOID PollSmbusNotify (
    IN  EFI_EVENT Event,
    IN  VOID      *Context
)
{
    EFI_STATUS Status;
    EFI_SMBUS_DEVICE_ADDRESS Address;
    UINTN Data;
    SMBUS_DXE_PRIVATE *Private = (SMBUS_DXE_PRIVATE *)Context;
    SMBUS_NOTIFY_LINK *NotifyLink = (SMBUS_NOTIFY_LINK *)(Private->NotifyList.pHead);

    Status = CheckNotify(&(Private->SmBusContext), &Address, &Data);
    if (EFI_ERROR(Status))
        return;

    while(NotifyLink != NULL)
    {
        if(Address.SmbusDeviceAddress == NotifyLink->SlaveAddress.SmbusDeviceAddress && 
           Data == NotifyLink->Data)
            NotifyLink->NotifyFunction(Address, Data);

        NotifyLink = (SMBUS_NOTIFY_LINK *)NotifyLink->Link.pNext;
    }
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
