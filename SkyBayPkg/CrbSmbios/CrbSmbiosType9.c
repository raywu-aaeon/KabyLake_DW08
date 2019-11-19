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

#include <AmiLib.h>
#include <Token.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <CrbSmbiosType9.h>
#include <Library/PchPcieRpLib.h>
#include <PlatformBoardId.h>
#include <Library/PcdLib.h>
#include <Library/AmiChipsetIoLib.h>
#include <IndustryStandard/Pci.h>

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard0[] = { //RVP3
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 1",  1},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 2",  5},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 3, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 3",  6},
    {0}
};

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard1[] = { //RVP5
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 1",  9},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 2",  4},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 3, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 3",  3},
    {0}
};

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard2[] = { //RVP7
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 1",  9},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 2",  4},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 3, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 3",  3},
    {0}
};

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard3[] = { //RVP8
    {{{9, 0x11, 0}, 1, SlotTypePciExpressX16,  SlotDataBusWidth16X, SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS, DEV_FUN_NUMBER(1, 0)}, "PCI-Express", 0xFF},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 6",  6},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 3, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 7",  7},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 4, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 8",  8},
    {0}
};

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard5[] = { //RVP10
    {{{9, 0x11, 0}, 1, SlotTypePciExpressX16,  SlotDataBusWidth16X, SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS, DEV_FUN_NUMBER(1, 0)}, "PCI-Express", 0xFF},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 6",  6},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 3, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 7",  7},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 4, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 8",  8},
    {0}
};

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard6[] = { //ZumbaBeach
    {{{9, 0x11, 0}, 1, SlotTypePciExpressX8,  SlotDataBusWidth8X, SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS, DEV_FUN_NUMBER(1, 0)}, "PCI-Express", 0xFF},
    {{{9, 0x11, 0}, 1, SlotTypePciExpressX4,  SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS, DEV_FUN_NUMBER(1, 1)}, "PCI-Express", 0xFF},
    {{{9, 0x11, 0}, 1, SlotTypePciExpressX4,  SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 3, 0x04, 0x01, 0, PCIE_BUS, DEV_FUN_NUMBER(1, 2)}, "PCI-Express", 0xFF},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,    SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 4, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 1",  1},
    {0}
};

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard7[] = { //RVP11
    {{{9, 0x11, 0}, 1, SlotTypePciExpressX16,  SlotDataBusWidth16X, SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS, DEV_FUN_NUMBER(1, 0)}, "PCI-Express", 0xFF},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express x1",  7},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 3, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express x4",  8},
    {0}
};

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard9[] = { //udimm
    {{{9, 0x11, 0}, 1, SlotTypePciExpressX16,  SlotDataBusWidth16X, SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS, DEV_FUN_NUMBER(1, 0)}, "PCI-Express", 0xFF},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 6",  6},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 3, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 7",  7},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 4, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 8",  8},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 5, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 14",  14},
    {0}
};

SMBIOS_TYPE_9 SystemSlotsInfo_CrbBoard11[] = { //KBLR
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthLong, 1, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 1",  1},
    {{{9, 0x11, 0}, 1, SlotTypePciExpress,     SlotDataBusWidth1X,  SlotUsageAvailable, SlotLengthLong, 2, 0x04, 0x01, 0, PCIE_BUS,                    0}, "PCI-Express 2",  7},
    {0}
};

/**
  Function to update SMBIOS type 9 for system slot information

  @param None
  @return EFI_STATUS
  
**/
VOID
EFIAPI
UpdateSmbiosType9 (VOID)
{
    EFI_STATUS                          Status;
    EFI_SMBIOS_PROTOCOL                 *EfiSmbios;
    SMBIOS_TYPE_9                       *Type9;
    SMBIOS_TYPE_9                       *SlotEntries;
    UINTN                               DevNum;
    UINTN                               FuncNum;
    UINT64                              RpBase;
    UINT64                              EpBase;
    UINT8                               CapPtr;
    UINT32                              SlotCapabilities;
    UINT16                              VendorId;
    
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &EfiSmbios);
    if (EFI_ERROR (Status))
    {
        DEBUG ((DEBUG_ERROR , "gBS->LocateProtocol(gEfiSmbiosProtocolGuid)... ERROR\n"));
        return;
    }
    
    if (PcdGet64 (PcdSmbiosMainSlotEntry) == 0) return;
    
    SlotEntries = (SMBIOS_TYPE_9 *) (UINTN) PcdGet64 (PcdSmbiosMainSlotEntry);
    
    while(SlotEntries->SlotInfo.StructureType.Type == 0x9) {
        Type9 = AllocateZeroPool (sizeof(SMBIOS_SYSTEM_SLOTS_INFO) + AsciiStrLen(SlotEntries->sSlotDes) + 2);
        MemCpy(Type9, SlotEntries, sizeof(SMBIOS_SYSTEM_SLOTS_INFO));
        Type9->SlotInfo.StructureType.Handle = 0xFFFE;
        MemCpy(((UINT8*)Type9 + sizeof(SMBIOS_SYSTEM_SLOTS_INFO)), SlotEntries->sSlotDes, AsciiStrLen(SlotEntries->sSlotDes));
        
        DevNum = SlotEntries->SlotInfo.DevFuncNumber >> 3;
        FuncNum = SlotEntries->SlotInfo.DevFuncNumber & 0x7;
        
        if (SlotEntries->RootPortIndex != 0xFF) {
            GetPchPcieRpDevFun (SlotEntries->RootPortIndex, &DevNum, &FuncNum);
            Type9->SlotInfo.DevFuncNumber = (UINT8) (DevNum << 3 | FuncNum);
        }
        
        RpBase = PCIE_CFG_ADDRESS (0, DevNum, FuncNum, 0);
        VendorId = MmioRead16(RpBase);
        if (VendorId != 0xFFFF) {

            if (VendorId == 0x8086) {
                // Port is used by Plug-in card or enabled for hot plug
                Type9->SlotInfo.CurrentUsage = SlotUsageInUse;
            } else {
                EpBase = PCIE_CFG_ADDRESS (MmioRead8(RpBase + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET), 0, 0, 0);
                if (MmioRead16(EpBase) != 0xFFFF) {
                    // Device present
                    Type9->SlotInfo.CurrentUsage = SlotUsageInUse;
                } else {
                    CapPtr = FindCapPtr(PCIEX_BASE_ADDRESS + RpBase, EFI_PCI_CAPABILITY_ID_PCIEXP);
                    SlotCapabilities = MmioRead32(RpBase + CapPtr + 0x14);
                    // Check Hot Plug Capable bit
                    if ((CapPtr != 0) && (SlotCapabilities & BIT6)) {
                        Type9->SlotInfo.CurrentUsage = SlotUsageInUse;
                    }
                }
            }
        }
        
        Status = EfiSmbios->Add(EfiSmbios, NULL, &Type9->SlotInfo.StructureType.Handle, (EFI_SMBIOS_TABLE_HEADER *) Type9);
        DEBUG ((DEBUG_INFO , "Add SMBIOS type 9, Status %r\n", Status));
        
        SlotEntries++;
    }
    
    return;
}

/**
  Callback function executed when the EndOfDxe event group is signaled.

  @param[in] Event      Event whose notification function is being invoked.
  @param[in] Context    The pointer to the notification function's context, which
                        is implementation-dependent.
**/
VOID
EFIAPI
UpdateSmbiosOnEndOfDxe (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
    EFI_STATUS          Status;
    
    UpdateSmbiosType9();
    
    Status = gBS->CloseEvent (Event);
    ASSERT_EFI_ERROR (Status);
}
/**
   The entry function of the CrbSmbios driver.

   This function updates SMBIOS for different board design.

   @param[in] ImageHandle  The firmware allocated handle for the EFI image.
   @param[in] SystemTable  A pointer to the EFI System Table.

   @retval EFI_SUCCESS     The entry point is executed successfully.
   @retval EFI_UNSUPPORTED Do not support ACPI S3.
   @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
CrbSmbiosType9Init (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS    Status;
    EFI_EVENT     EndOfDxeEvent;
    
    DEBUG((DEBUG_INFO, "CrbSmbiosInit enter\n"));
    
#if CRB_BOARD == 0
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard0);
#elif CRB_BOARD == 1
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard1);
#elif CRB_BOARD == 2
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard2);
#elif CRB_BOARD == 3
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard3);
#elif CRB_BOARD == 5
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard5);
#elif CRB_BOARD == 6
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard6);
#elif CRB_BOARD == 7
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard7);
#elif CRB_BOARD == 9
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard9);
#elif CRB_BOARD == 11
    PcdSet64 (PcdSmbiosMainSlotEntry, (UINTN)&SystemSlotsInfo_CrbBoard11);
#endif

    
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    UpdateSmbiosOnEndOfDxe,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    ASSERT_EFI_ERROR (Status);
    return Status;
}

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
