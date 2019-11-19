//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciIo.h>
#include <PchAccess.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/BlockIo.h>

/**
  AMI BDS flow check SATA device before PCIE device.
  If Intel pcie NVMe device use software remapping, AMI NVMe driver can not get the DiskIo when check PCIE device.
  It is because the device has remapped to SATA.
  Need to Connect SATA controller again to initial DiskIo and Partition protocol.
      
    @param VOID

    @retval VOID
**/
VOID
AmiConnectPcieNvme ()
{
    EFI_STATUS             Status;
    UINTN                  Index;
    UINTN                  HandleCount;
    EFI_HANDLE             *HandleBuffer;
    EFI_PCI_IO_PROTOCOL    *PciIo;
    UINTN                  Segment;
    UINTN                  Bus;
    UINTN                  Device;
    UINTN                  Function;
    PCI_TYPE00             PciData;
    EFI_BLOCK_IO_PROTOCOL  *EfiBlockIoProtocol;
    VOID                   *RstStatusProtocol;
    EFI_HANDLE             *SataHandle = NULL;
    
    DEBUG ((DEBUG_INFO, "AmiConnectPcieNvme() - Start\n"));

    //
    // Find RstStatusProtocol to determine RST is run or not
    //
    Status = gBS->LocateProtocol (
                    &gRstStatusProtocolGuid,
                    NULL,
                    (VOID **) &RstStatusProtocol
                    );
    if (EFI_ERROR (Status)) {
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
      
      if ((Bus == DEFAULT_PCI_BUS_NUMBER_PCH) &&
          (Device == PCI_DEVICE_NUMBER_PCH_SATA) &&
          (Function == PCI_FUNCTION_NUMBER_PCH_SATA)) {
        SataHandle = HandleBuffer[Index];
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
            (PciData.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_SOLID_STATE)) {
          
          DEBUG ((DEBUG_INFO, "Found Nvme devices in [B%x/D%x/F%x]\n", Bus, Device, Function));
          
          Status = gBS->HandleProtocol (
                          HandleBuffer[Index],
                          &gEfiBlockIoProtocolGuid,
                          (VOID *) &EfiBlockIoProtocol
                          );
          if (EFI_ERROR(Status)) {
            //
            // gEfiBlockIoProtocolGuid should be installed in Nvme controller.
            // If not, Intel RST driver may do software remap to map it under SATA controller
            // Connect SATA controller again to initial DiskIo and Partition protocol.
            //
            DEBUG ((DEBUG_INFO, "Connect SATA controller for Pcie Nvme remap.\n")); 
            gBS->ConnectController (SataHandle, NULL, NULL, TRUE);  
            break;
          }
        }
      }
    }
    if (HandleBuffer) {
      FreePool (HandleBuffer);
    }

    DEBUG ((DEBUG_INFO, "AmiConnectPcieNvme() - End\n"));    
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
