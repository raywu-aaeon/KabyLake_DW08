//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiTbtInfo.c
  Provide an UEFI protocol to store Thunderbolt information.

**/

#include <TbtBoardInfo.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TbtCommonLib.h>
#include <Guid/HobList.h>
#include <Protocol/TbtInfoProtocol.h>

TBT_INFO_HOB                    *mTbtInfo;

/**
  Get Thunderbolt Information.

  @param  This                  Indicates a pointer to the calling context.
  @param  AicIndex              Select Thunderbolt AIC number
  @param  Information           A pointer points to the AMI_TBT_STATUS data structure

  @retval EFI_SUCCESS           All valid data is successfully output
  @retval EFI_NO_MEDIA          The selected AIC is not presented
  @retval EFI_INVALID_PARAMETER Information is NULL
  @retval EFI_NOT_READY         Some required information is missed.
  @retval EFI_OUT_OF_RESOURCES  Bus number is not enough for Thunderbolt host
**/
EFI_STATUS
GetTbtStatus (
  IN  AMI_THUNDERBOLT_INFO_PROTOCOL *This,
  IN  UINTN                     AicIndex,
  OUT AMI_TBT_STATUS            *Information
  )
{
  UINT8                         Bus, Dev, Fun;
  UINT8                         IoBase = 0, IoLimit = 0;
  UINT16                        MemBase = 0, MemLimit = 0;
  UINT64                        PrefetchBase = 0, PrefetchLimit = 0;

  if (This == NULL || Information == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Information, sizeof (AMI_TBT_STATUS));
  Information->DeviceID = 0xffff;

  //
  // Kabylake support only 1 host
  //
  if (mTbtInfo->TbtSelector == 0 || AicIndex > 0) {
    return EFI_NO_MEDIA;
  }

  Bus = mTbtInfo->Bus;
  Dev = mTbtInfo->Dev;
  Fun = mTbtInfo->Fun;

  //
  // Bus range for Thunderbolt host is incorrect
  //
  if (MmioRead8 (MmPCIeAddress (Bus, Dev, Fun, 0x19)) == 0xff ||
      MmioRead8 (MmPCIeAddress (Bus, Dev, Fun, 0x1A)) == 0xff ||
      (MmioRead8 (MmPCIeAddress (Bus, Dev, Fun, 0x1A)) -
       MmioRead8 (MmPCIeAddress (Bus, Dev, Fun, 0x19))) < 4) {
    return EFI_OUT_OF_RESOURCES;
  }

  IoBase  = MmioRead8 (MmPCIeAddress (Bus, Dev, Fun, 0x1C));
  IoLimit = MmioRead8 (MmPCIeAddress (Bus, Dev, Fun, 0x1D));

  MemBase  = MmioRead16 (MmPCIeAddress (Bus, Dev, Fun, 0x20));
  MemLimit = MmioRead16 (MmPCIeAddress (Bus, Dev, Fun, 0x22));

  PrefetchBase   = MmioRead16 (MmPCIeAddress (Bus, Dev, Fun, 0x24));
  PrefetchBase  |= (UINT64)(MmioRead32 (MmPCIeAddress (Bus, Dev, Fun, 0x28))) << 16;
  PrefetchLimit  = MmioRead16 (MmPCIeAddress (Bus, Dev, Fun, 0x26));
  PrefetchLimit |= (UINT64)(MmioRead32 (MmPCIeAddress (Bus, Dev, Fun, 0x2C))) << 16;

  Information->SecurityLevel                   = mTbtInfo->SecurityMode;
  Information->Location.Bus                    = Bus;
  Information->Location.Dev                    = Dev;
  Information->Location.Fun                    = Fun;
  Information->ResourceConsumed.IoKB           = (IoBase > IoLimit) ? 0 : (((IoLimit >> 4) & 0xf) - ((IoBase >> 4) & 0xf) + 1) * 4;
  Information->ResourceConsumed.MemoryMB       = (MemBase > MemLimit) ? 0 : (((MemLimit >> 4) & 0xfff) - ((MemBase >> 4) & 0xfff) + 1);
  Information->ResourceConsumed.PrefetchMB     = (UINT16)(PrefetchBase > PrefetchLimit) ? 0 : (((PrefetchLimit >> 4) & 0xfff) - ((PrefetchBase >> 4) & 0xfff) + 1);
  Information->ResourceConsumed.StartedBusNo   = MmioRead8 (MmPCIeAddress (Bus, Dev, Fun, 0x19));
  Information->ResourceConsumed.TerminalBusNo  = MmioRead8 (MmPCIeAddress (Bus, Dev, Fun, 0x1A));

  return EFI_SUCCESS;
}

/**
  Create Thunderbolt Infomation and publish as an UEFI Protocol

  @param  Event                 The event registered.
  @param  Context               Event context. Not used in this event handler.

  @return                       No return value
**/
VOID
EFIAPI
PublishInterface (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  EFI_STATUS                    Status;
  AMI_THUNDERBOLT_INFO_PROTOCOL AmiTbtInformation;
  EFI_HANDLE                    Handle = NULL;

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &mTbtInfo);
  if (!EFI_ERROR (Status) && mTbtInfo != NULL) {
    mTbtInfo = GetNextGuidHob (&gTbtInfoHobGuid, mTbtInfo);
    if (mTbtInfo == NULL) {
      return;
    }
  } else {
    return;
  }

  AmiTbtInformation.GetAicInfomation = GetTbtStatus;
  AmiTbtInformation.ReportAicCount   = 1;   // Hardcoding due to Kabylake support only 1 host
  gBS->InstallProtocolInterface (
         &Handle,
         &gAmiThunderboltInfoProtocolGuid,
         EFI_NATIVE_INTERFACE,
         &AmiTbtInformation
         );
}

/**
  Create Thunderbolt Infomation and publish as an UEFI Protocol

  @param  ImageHandle           Image handle.
  @param  SystemTable           Pointer to the EFI system table.

  @retval EFI_SUCCESS           Function is executed successfully
  @retval others                Any error is occured
**/
EFI_STATUS
TbtInfoEntryPoint (
  IN  EFI_HANDLE                ImageHandle,
  IN  EFI_SYSTEM_TABLE          *SystemTable
  )
{
  EFI_EVENT                     Event;
  EFI_GUID                      TbtHookDoneGuid = {0x12124545, 0x033c, 0x4783, 0x55, 0x2b, 0x84, 0xab, 0x74, 0xa6, 0x97, 0xb7};
  EFI_STATUS                    Status;
  VOID                          *Registration;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PublishInterface,
                  NULL,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return gBS->RegisterProtocolNotify (
                &TbtHookDoneGuid,
                Event,
                &Registration
                );
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
