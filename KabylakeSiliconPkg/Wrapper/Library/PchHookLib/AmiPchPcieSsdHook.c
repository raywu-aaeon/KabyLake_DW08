//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
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
// Revision History
// ----------------
// $Log: $
// 
//
//*****************************************************************************

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <PlatformInfo.h>
#include <PlatformBoardId.h>
#include <Protocol/ConsoleControl.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciIo.h>
#include <PchAccess.h>
#include <Library/PchPcieRpLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
EFI_GUID gConInStartedProtocolGuid = CONSOLE_OUT_DEVICES_STARTED_PROTOCOL_GUID;


/**
  Locate and Initialize a PCIe SSD
      
    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/
VOID
LocateAndInitializePcieSsd ()
{
  EFI_STATUS           Status;
  UINTN                Index;
  UINTN                HandleCount;
  EFI_HANDLE           *HandleBuffer;
  PCI_TYPE00           PciData;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINTN                Segment;
  UINTN                Bus;
  UINTN                Device;
  UINTN                Function;
  BOOLEAN              MD2Exist = FALSE;
  BOOLEAN              MD2ExistPrevious;
  UINTN                DataSize;
  UINTN                RpIndex;
  UINT16               LSTS;
  EFI_GUID             gHaloMd2VariableGuid = {0x4b534b3b, 0x9bdc, 0x4460, {0x93, 0x73, 0x57, 0x2e, 0x48, 0x11, 0xee, 0x83}};
  PLATFORM_INFO        *PlatformInfo = NULL;
  EFI_BOOT_MODE        BootMode = GetBootModeHob();
  
  DEBUG((EFI_D_INFO, "LocateAndInitializePcieSsdCallback() - Start\n"));
  
  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
  if (EFI_ERROR (Status)) {
      return;
  }
  
  if (BootMode == BOOT_ON_FLASH_UPDATE || BootMode == BOOT_IN_RECOVERY_MODE) {
      return;
  }
  
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    HandleCount = 0;
  }
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID *) &PciIo
                    );
    ASSERT_EFI_ERROR(Status);
    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);

    //
    // RVP10/16 M.2 power GPIO fix, detecting whether Root Port 0x0C/0x10 having device
    //
    if ((PlatformInfo->BoardID == BoardIdSkylakeHaloLpddr3Rvp16) || (PlatformInfo->BoardID == BoardIdSkylakeHaloDdr4Rvp11) || (PlatformInfo->BoardID == BoardIdSkylakeHaloDdr4Rvp11Ppv)) {
      if ((Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && ((PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3 <= Device) && (Device <= PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2))) {
        Status = GetPchPcieRpNumber (Device, Function, &RpIndex); 
        if (!EFI_ERROR(Status) && ((RpIndex == 0x0C)||(RpIndex == 0x10))) {
          Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint16,
                    R_PCH_PCIE_LSTS,
                    sizeof (UINT16),
                    &LSTS
                    );
          if (LSTS & B_PCIE_LSTS_LA) {
            MD2Exist = TRUE;
          }
        }
      }
    }

    if (Bus != 0) {
      Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint8,
                    0,
                    sizeof (PciData),
                    &PciData
                    );
      if ((PciData.Hdr.ClassCode[2] == PCI_CLASS_MASS_STORAGE) && 
          ((PciData.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_SATADPA) || (PciData.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_SOLID_STATE))) {
        gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE);
      }
    }
  }
  if (HandleBuffer) {
      gBS->FreePool (HandleBuffer);
  }

  if ((PlatformInfo->BoardID == BoardIdSkylakeHaloLpddr3Rvp16) || (PlatformInfo->BoardID == BoardIdSkylakeHaloDdr4Rvp11) || (PlatformInfo->BoardID == BoardIdSkylakeHaloDdr4Rvp11Ppv)) {
    Status = gRT->GetVariable (
                    L"HaloMd2Variable",
                    &gHaloMd2VariableGuid,
                    NULL,
                    &DataSize,
                    &MD2ExistPrevious
                    );
    if ((MD2Exist != MD2ExistPrevious) || (Status == EFI_NOT_FOUND)) {
      Status = gRT->SetVariable (
                      L"HaloMd2Variable",
                      &gHaloMd2VariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                      sizeof (BOOLEAN),
                      &MD2Exist
                      );
    }
  }

  DEBUG((EFI_D_INFO, "LocateAndInitializePcieSsdCallback() - End\n"));
}

