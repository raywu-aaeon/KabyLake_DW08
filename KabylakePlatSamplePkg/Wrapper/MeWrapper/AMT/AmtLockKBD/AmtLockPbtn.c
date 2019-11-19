//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************
/** @file AmtLockPbtn.c
    AMT Lock KeyBoard Functions.

**/
#include <Token.h>
#include <Efi.h>
#include <Dxe.h>
#include <AmiDxeLib.h>
#include <Protocol/AlertStandardFormat.h>
//============================================================================
// Local defines for transaction types
//============================================================================
#define BDS_CONNECT_DRIVERS_PROTOCOL_GUID \
    { 0x3aa83745, 0x9454, 0x4f7a, { 0xa7, 0xc0, 0x90, 0xdb, 0xd0, 0x2f, 0xab, 0x8e } }
static EFI_GUID gBdsConnectDriversProtocolGuid = BDS_CONNECT_DRIVERS_PROTOCOL_GUID;

/**
    This routine disables Power Button SMI if Lock Power Button
    bit of Boot Option BitMask is set.

    @param VOID

    @retval VOID


**/
VOID
ClearPowerButtonSmi (VOID)
{
    UINT32                      dPwrMgValue;

    // Read PM_1 Status/Enable Register
    dPwrMgValue = IoRead32(PM_BASE_ADDRESS);
    dPwrMgValue |= BIT08;           // clear Power Button Status
    dPwrMgValue &= ~BIT24;          // clear Power Button Enable
    IoWrite32 ( PM_BASE_ADDRESS, dPwrMgValue );
}

/**

    @param Event 
    @param Context 

    @retval EFI_STATUS Status


**/
VOID
EFIAPI
DisablePowerButtonSmiEvent  (
    IN EFI_EVENT        Event,
    IN VOID             *Context
    )
{
  ClearPowerButtonSmi();
  pBS->CloseEvent(Event);
  return;
}

/**
    Lock Power Button Entrypoint.

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Status


**/
EFI_STATUS EFIAPI AmtLockPbtnEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS  Status;
    ALERT_STANDARD_FORMAT_PROTOCOL      *Asf;
    ASF_BOOT_OPTIONS                    *mAsfBootOptions;
    EFI_EVENT                   EvtASFBdsConnectDrivers;
    VOID                        *RgnASFBdsConnectDrivers;

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->LocateProtocol ( \
        &gAlertStandardFormatProtocolGuid, \
        NULL, \
       (VOID**) &Asf );
    if (EFI_ERROR (Status)) return Status;

    Status = Asf->GetBootOptions (Asf, &mAsfBootOptions);
    if (EFI_ERROR (Status)) return Status;
    
    if (mAsfBootOptions->BootOptions & LOCK_POWER_BUTTON) {
      Status = RegisterProtocolCallback ( \
          &gBdsConnectDriversProtocolGuid, \
          (EFI_EVENT_NOTIFY) DisablePowerButtonSmiEvent, \
          NULL, \
          &EvtASFBdsConnectDrivers, \
          &RgnASFBdsConnectDrivers  );
          
      
      ClearPowerButtonSmi();
    }
    return EFI_SUCCESS;
}
