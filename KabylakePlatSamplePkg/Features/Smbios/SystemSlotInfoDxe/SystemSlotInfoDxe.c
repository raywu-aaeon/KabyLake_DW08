/** @file
  SystemSlotInfoDxe.c
  SMBIOS Type 9 Table Implementation

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/
#include "SystemSlotInfoDxe.h"

SYSTEM_SLOT_ENTRY *gSlotEntries = NULL;
UINT8 gNumberofSlots = 0;

/**
  Creates SMBIOS Type 9 Table

  @param[in] SYSTEM_SLOT_INFO     Pointer to the Slot entry to which Table needs to create.

  @retval    EFI_STATUS           returns Status if any failure in the function
**/
EFI_STATUS
CreateSmbiosType9 (
  IN  SYSTEM_SLOT_INFO  *SlotEntry
  )
{
  EFI_STATUS            Status;
  UINTN                 SlotIdStrSize;
  UINTN                 TotalSize;
  SMBIOS_TABLE_TYPE9    *Type9 = NULL;
  UINT8                 *StrPart;
  EFI_SMBIOS_HANDLE     SmbiosHandle;
  EFI_SMBIOS_PROTOCOL   *Smbios;

  DEBUG((DEBUG_INFO, "SystemSlotInfo : CreateSmbiosType9\n"));
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
  if (EFI_ERROR (Status)){
    DEBUG ((DEBUG_INFO, "SystemSlotInfo : Locate Protocol failed %r \n",Status));
    return Status;
  }

  SlotIdStrSize = StrLen (SlotEntry->SlotDesignation);

  TotalSize = SlotIdStrSize + sizeof (SMBIOS_TABLE_TYPE9) + 2; /*2 null bytes at the end of struct*/

  Status = gBS->AllocatePool (
    EfiBootServicesData,
    TotalSize,
    (VOID**)&Type9
    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "SystemSlotInfo : Faild to Allocate Memory.\n"));
    return Status;
  }

  gBS->SetMem (Type9, TotalSize, 0);

  //
  // Fill SMBIOS Type 9 - System Slot info
  //
  Type9->Hdr.Type             = 9;
  Type9->Hdr.Length           = sizeof(SMBIOS_TABLE_TYPE9);
  Type9->SlotDesignation      = 1; //string
  Type9->SlotType             = SlotEntry->SlotType;
  Type9->SlotDataBusWidth     = SlotEntry->SlotDataBusWidth;
  Type9->CurrentUsage         = SlotEntry->CurrentUsage;
  Type9->SlotLength           = SlotEntry->SlotLength;
  Type9->SlotID               = SlotEntry->SlotID;
  Type9->SlotCharacteristics1 = SlotEntry->SlotCharacteristics1;
  Type9->SlotCharacteristics2 = SlotEntry->SlotCharacteristics2;
  Type9->SegmentGroupNum      = 0;
  Type9->BusNum               = SlotEntry->BusNum;
  Type9->DevFuncNum           = ((SlotEntry->DevNum << 3) | SlotEntry->FuncNum);

  //
  // Append Slot string
  //
  StrPart = (UINT8*)(Type9 + 1);
  UnicodeStrToAsciiStrS (SlotEntry->SlotDesignation, (CHAR8*)StrPart, SlotIdStrSize + 1);

  //
  // Publish SmBios Type 9
  //
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (Smbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)Type9);
  DEBUG ((DEBUG_INFO, "SystemSlotInfo : SMBIOS Type 9 status %r\n",Status));
  Status = gBS->FreePool (Type9);

  return Status;
}

/**
  Protocol Notify call Back for Smbios Protocol
  Read all the slot entries and call CreateSMBIOSTable function to create Type 9 Table

  @param[in] EFI_EVENT     which will be returned when event is created.
  @param[in] Context       Context if any.
  @retval    VOID         returns None
**/
VOID
EFIAPI
CreateSmbiosType9Table (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  UINT8 Index;
  SYSTEM_SLOT_INFO *SlotInfo;

  for (Index = 0; Index < gNumberofSlots; Index++) {
    SlotInfo = &gSlotEntries->Slotinfo[Index];
    if (SlotInfo->Isvalid) CreateSmbiosType9 (SlotInfo);
  }
}
/**
  Calls the call back if Protocol is Already available
  else registers a protocol notify call back

  @param[in] EFI_GUID             Protocol Guid.
  @param[in] EFI_EVENT_NOTIFY     Protocol CallBack function

  @retval    VOID                 None
**/
VOID
EFIAPI
RegisterProtocolNotifyCallback (
  IN EFI_GUID         *ProtocolGuid,
  IN EFI_EVENT_NOTIFY ProtocolCallback
  )
{
  EFI_STATUS        Status;
  EFI_EVENT         CallbackEvent;
  VOID              *Protocol;
  VOID              *Registration;

  Status = gBS->LocateProtocol (ProtocolGuid, NULL, (VOID **)&Protocol);
  DEBUG ((DEBUG_INFO, "Locate Protocol returned %r \n",Status));
  if (!EFI_ERROR (Status)) {
    ProtocolCallback (NULL, NULL);
  }
  else {
    Status = gBS->CreateEvent (
      EVT_NOTIFY_SIGNAL,
      TPL_CALLBACK,
      ProtocolCallback,
      NULL,
      &CallbackEvent
      );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (
      ProtocolGuid,
      CallbackEvent,
      &Registration
      );
    ASSERT_EFI_ERROR (Status);
  }
}

/**
  Update with Dynamic data of each slot entry

  @param[None]  Updates global varibles

  @retval     VOID           None
**/
VOID
EFIAPI
UpdateDynamicData (
  VOID
  )
{
  UINT8            Index;
  UINTN            PcieRpBaseAddr = 0;
  SYSTEM_SLOT_INFO *SlotInfo;
  //
  //Update the dynamic data
  //
  DEBUG ((DEBUG_INFO, "SystemSlotInfo : UpdateDynamicData\n"));
  for (Index = 0; Index < gNumberofSlots; Index++) {
    SlotInfo = &gSlotEntries->Slotinfo[Index];
    //
    // Check the Root Port is used by Plug-in card or enabled for hot plug
    //
    if (!(SlotInfo->BusNum == PCIE_DEFAULT)) {
      PcieRpBaseAddr = MmPciBase (SlotInfo->BusNum, (UINT32)SlotInfo->DevNum, (UINT32)SlotInfo->FuncNum);

      if ((MmioRead16 (PcieRpBaseAddr) != 0xFFFF)) {
        gSlotEntries->Slotinfo[Index].CurrentUsage = SlotUsageInUse;
      }
    }
  }
}

/**
  Get Board Specific Slot Entries

  @param[out] SlotEntryDetected   Return Slot Entry detected or not.

  @retval     VOID           None
**/
VOID
EFIAPI
GetBoardSpecificSlotEntries (
  OUT BOOLEAN *SlotEntryDetected
  )
{
  *SlotEntryDetected = FALSE;

  //
  // Get Slot entries Specific to the Board
  //
  gSlotEntries = (SYSTEM_SLOT_ENTRY *) (UINTN) PcdGet64 (PcdSmbiosMainSlotEntry);
  gNumberofSlots = gSlotEntries->Numberofentries;
  *SlotEntryDetected = gSlotEntries->Detect;
}

/**
  This is driver entry point to update System Slot Info.

  @param[in] ImageHandle  A handle for the image that is initializing this driver
  @param[in] SystemTable  A pointer to the EFI system table

  @retval    EFI_SUCCESS  The initialization finished successfully.
**/
EFI_STATUS
EFIAPI
SystemSlotInfoDxeEntry (
  IN  EFI_HANDLE         ImageHandle,
  IN  EFI_SYSTEM_TABLE   *SystemTable
  )
{
  BOOLEAN SlotInfoFound = FALSE;

  DEBUG ((DEBUG_INFO, "SystemSlotInfoDxeEntry..\n"));
  //
  // Get Board Specific Entry
  //
  GetBoardSpecificSlotEntries (&SlotInfoFound);
  if (SlotInfoFound) {
    DEBUG ((DEBUG_INFO, "SystemSlotInfoDxe: Call Update Data\n"));
    //
    // Update any dynamic data required
    //
    UpdateDynamicData ();
    //
    // Register a protocol call back to create SMBIOS Tabes for Corresponding Entries
    //
    RegisterProtocolNotifyCallback (&gEfiSmbiosProtocolGuid, CreateSmbiosType9Table);
  }
  return EFI_SUCCESS;
}
