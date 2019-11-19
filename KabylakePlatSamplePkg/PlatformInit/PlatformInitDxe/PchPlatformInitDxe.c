/** @file
  Source code file for the PCH Platform Init DXE module

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2016 Intel Corporation.

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

#include "PlatformInitDxe.h"
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci22.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PCI_IO_PROTOCOL               *m1394PciIo = NULL;

//#define EFI_MAX_ADDRESS                   0xFFFFFFFFFFFFFFFFULL
#define GLOBAL_UNIQUE_IDHI                0x354fc000
#define GLOBAL_UNIQUE_IDLO                0x35e71a01
#define GLOBAL_UNIQUE_IDHI_OFFSET         0x24
#define GLOBAL_UNIQUE_IDLO_OFFSET         0x28
#define SUBSYSTEM_ACCESS_REGISTER_OFFSET  0xF8
#define TI_XIO2200A_VENDOR_ID             0x104C
#define TI_XIO2200A_DEVICE_ID             0x8231
#define TI_1394_OHCI_VENDOR_ID            0x104C
#define TI_1394_OHCI_DEVICE_ID            0x8235
#define MAX_STRING_LEN                    200
#define PCIE_CFG_ADDR(bus,dev,func,reg) \
  ((UINTN) PcdGet64 (PcdPciExpressBaseAddress) + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg)

/**
  Enables SW SMI and Global SMI.

  @todo merge with SiPkg code PchSmmEnableGlobalSmiBit

**/
VOID
EnableSwAndGlobalSmi(
  VOID
  )
{
  UINT16                            ABase;
  UINT32                            SmiEn;

  PchAcpiBaseGet (&ABase);
  SmiEn = IoRead32 (ABase + R_PCH_SMI_EN);
  //
  // Enable SW SMI and Global SMI for S3 resume.
  // This enables SMI and SW SMI as early as possible for S3 resume to prevent from missing SW SMI.
  //
  SmiEn |= (B_PCH_SMI_EN_APMC | B_PCH_SMI_EN_GBL_SMI);

  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint32,
    (UINTN) (ABase + R_PCH_SMI_EN),
    1,
    &SmiEn
    );
}

/**
  Configures PCIE port8 and TI XIO2200A Bridge to access OHCI controller on Desktop.


**/
VOID
TiXio2200aBridgeConfigS3 (
  VOID
  )
{
  UINT8 PriBusNum;
  UINT8 BusNum;
  UINT8 Data8;
  UINT32 Data32;

  //
  // On Desktop, The bridge relationship for 1394 is PCIE Root Port #8 -> TI XIO2200A Bridge -> 1394 Host Controller.
  // First,preset PCH Root Port #8's Primary Bus Number, Secondary Bus Number and Subordinate Bus Number
  //
  BusNum = MmioRead8(PCIE_CFG_ADDR (
                          0,
                          PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                          PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8,
                          PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET
                          ));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    PCIE_CFG_ADDR(0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),
    (UINTN) 1,
    &BusNum
    );

  BusNum = MmioRead8 (PCIE_CFG_ADDR (
                          0,
                          PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                          PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8,
                          PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                          ));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    PCIE_CFG_ADDR(0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),
    (UINTN) 1,
    &BusNum
    );

  BusNum = MmioRead8 ( PCIE_CFG_ADDR (
                          0,
                          PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                          PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8,
                          PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET
                          ));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    PCIE_CFG_ADDR(0, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),
    (UINTN) 1,
    &BusNum
    );

  //
  // Get the Bus number of TI XIO2200A
  //
  PriBusNum = MmioRead8 ( PCIE_CFG_ADDR (
                          0,
                          PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                          PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8,
                          PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                          ));

  //
  // TI XIO2200A: Program Primary Bus Number
  //
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    PCIE_CFG_ADDR(PriBusNum, 0x0, 0x0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),
    (UINTN) 1,
    &PriBusNum
    );

  //
  // TI XIO2200A: Program Secondary Bus Number
  //
  Data8 = PriBusNum + 1;
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    PCIE_CFG_ADDR(PriBusNum, 0x0, 0x0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),
    (UINTN) 1,
    &Data8
    );

  //
  // TI XIO2200A: Program Subordinary Bus Number
  //
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    PCIE_CFG_ADDR(PriBusNum, 0x0, 0x0, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),
    (UINTN) 1,
    &Data8
    );

  //
  // TI XIO2200A: Program Subsystem Vendor ID - 0x104C - Texas Instruments and Subsystem Device ID - 0x8231
  //
  MmioWrite32 (PCIE_CFG_ADDR (PriBusNum, 0, 0, PCI_SUBSYSTEM_VENDOR_ID_OFFSET), (UINT32) ((TI_XIO2200A_DEVICE_ID << 16) | TI_XIO2200A_VENDOR_ID));
  Data32 = MmioRead32 (PCIE_CFG_ADDR(PriBusNum, 0, 0, PCI_SUBSYSTEM_VENDOR_ID_OFFSET));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    PCIE_CFG_ADDR(PriBusNum, 0, 0, PCI_SUBSYSTEM_VENDOR_ID_OFFSET),
    (UINTN) 1,
    &Data32
    );

}

/**
  Programs 1394 OHCI host controller GUID at offset 0x24 and 0x28 in MMIO


**/
VOID
Callback1394BootScript (
  VOID
  )
{
  UINT32                        Mmio1394Bar;
  EFI_STATUS                    Status;
  UINTN                         Seg;
  UINTN                         Bus;
  UINTN                         Dev;
  UINTN                         Func;
  UINTN                         Index;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  UINT16                        PciCommandRegOrig;
  UINT16                        PciCommandReg;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  PCI_TYPE00                    Pci;
  UINT32                        GlobalUniqueIDHi;
  UINT32                        GlobalUniqueIDLo;
  UINT32                        PciSubsystemId;

  GlobalUniqueIDHi  = GLOBAL_UNIQUE_IDHI;
  GlobalUniqueIDLo  = GLOBAL_UNIQUE_IDLO;

  //
  // Start to check all the PCI IO to find 1394 OHCI host controller
  //
  HandleCount = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID **) &PciIo);
    if (!EFI_ERROR (Status)) {
      //
      // Check for 1394 OHCI host controller
      //
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint32,
                            0,
                            sizeof (Pci) / sizeof (UINT32),
                            &Pci
                            );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (!((Pci.Hdr.VendorId == TI_1394_OHCI_VENDOR_ID) && (Pci.Hdr.DeviceId == TI_1394_OHCI_DEVICE_ID))) {
        continue;
      }

      //
      // Save PciIo Handle
      //
      m1394PciIo = PciIo;
      break;
    }
  }

  if (HandleBuffer != NULL) {
    (gBS->FreePool) (HandleBuffer);
  }

  if (Index == HandleCount) {
    return;
  }

  //
  // On Desktop, The bridge relationship for 1394 is PCIE port8 -> TI XIO2200A Bridge -> 1394 Host Controller
  // Configure PCIE port8 and TI XIO2200A Bridge to access OHCI controller.
  //
  TiXio2200aBridgeConfigS3 ();
  //
  // Now 1394 Host controller can be accessed behind TI XIO2200A Bridge
  //
  Status = m1394PciIo->GetLocation (m1394PciIo, &Seg, &Bus, &Dev, &Func);
  if (EFI_ERROR (Status)) {
    return ;
  }
  //
  // Enable MMIO
  //
  Status = m1394PciIo->Pci.Read (
                             m1394PciIo,
                             EfiPciIoWidthUint16,
                             PCI_COMMAND_OFFSET,
                             1,
                             &PciCommandReg
                             );
  ASSERT_EFI_ERROR (Status);
  PciCommandRegOrig = PciCommandReg;
  PciCommandReg |= EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER;

  Status = m1394PciIo->Pci.Write (
                             m1394PciIo,
                             EfiPciIoWidthUint16,
                             PCI_COMMAND_OFFSET,
                             1,
                             &PciCommandReg
                             );
  ASSERT_EFI_ERROR (Status);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    PCIE_CFG_ADDR (Bus, Dev, Func, PCI_COMMAND_OFFSET),
    (UINTN) 1,
    &PciCommandReg
    );

  //
  // We need to program the SubSystem Access register (0xF8) to update the
  // Subsystem Vendor Id - 0x104C - Texas Instruments and Subsystem Device Id - 0x8235
  //
  PciSubsystemId = (UINT32) ((TI_1394_OHCI_DEVICE_ID << 16) | TI_1394_OHCI_VENDOR_ID);

  Status = m1394PciIo->Pci.Write (
                             m1394PciIo,
                             EfiPciIoWidthUint32,
                             SUBSYSTEM_ACCESS_REGISTER_OFFSET,
                             1,
                             &PciSubsystemId
                             );
  ASSERT_EFI_ERROR (Status);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    PCIE_CFG_ADDR (Bus, Dev, Func, SUBSYSTEM_ACCESS_REGISTER_OFFSET),
    (UINTN) 1,
    &PciSubsystemId
    );
  //
  // Write GUID at offset 0x24 and 0x28 in MMIO space
  //
  Status = m1394PciIo->Mem.Write (
                             m1394PciIo,
                             EfiPciIoWidthUint32,
                             0,
                             GLOBAL_UNIQUE_IDHI_OFFSET,
                             1,
                             (VOID *) (&GlobalUniqueIDHi)
                             );
  ASSERT_EFI_ERROR (Status);

  Status = m1394PciIo->Mem.Write (
                             m1394PciIo,
                             EfiPciIoWidthUint32,
                             0,
                             GLOBAL_UNIQUE_IDLO_OFFSET,
                             1,
                             (VOID *) (&GlobalUniqueIDLo)
                             );
  ASSERT_EFI_ERROR (Status);

  //
  // Get MMIO base address for S3 path
  //
  Status = m1394PciIo->Pci.Read (
                             m1394PciIo,
                             EfiPciIoWidthUint32,
                             PCI_BASE_ADDRESSREG_OFFSET,
                             1,
                             &Mmio1394Bar
                             );
  ASSERT_EFI_ERROR (Status);

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    PCIE_CFG_ADDR (Bus, Dev, Func, PCI_BASE_ADDRESSREG_OFFSET),
    (UINTN) 1,
    &Mmio1394Bar
    );

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    Mmio1394Bar + GLOBAL_UNIQUE_IDHI_OFFSET,
    (UINTN) 1,
    &GlobalUniqueIDHi
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    Mmio1394Bar + GLOBAL_UNIQUE_IDLO_OFFSET,
    (UINTN) 1,
    &GlobalUniqueIDLo
    );

  //
  // Restore command register
  //
  Status = m1394PciIo->Pci.Write (
                             m1394PciIo,
                             EfiPciIoWidthUint16,
                             PCI_COMMAND_OFFSET,
                             1,
                             &PciCommandRegOrig
                             );
  ASSERT_EFI_ERROR (Status);

  return ;
}

/**
  Initilaizes PCH platform parts

**/
VOID
PchPlatformInitDxe (
  VOID
  )
{
  EnableSwAndGlobalSmi();
}
