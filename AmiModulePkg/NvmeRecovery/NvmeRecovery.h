//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeRecovery.h
    Header file contains structure and function declaration used 
    for Nvme PCI enumeration

**/

#ifndef _AMI_NVME_RECOVERY_H_
#define _AMI_NVME_RECOVERY_H_

#include <Token.h>
#include <AmiPeiLib.h>
#include <AmiLib.h>
#include <Ppi/DeviceRecoveryBlockIo.h>
#include <Ppi/Stall.h>
#include <Protocol/BlockIo.h>
#include "NvmeCtrl.h"
#include "Pci.h"
#include "Library/PcdLib.h"
#include  <Library/HobLib.h>
#include "NvmeRootBridgeElink.h"
#if defined(PeiPciEnumeration_SUPPORT) && (PeiPciEnumeration_SUPPORT == 1)
#include <Ppi/AmiPeiPciEnumeration.h>
#endif

#define NVME_CLASS_CODE       0x01
#define NVME_SUBCLASS_CODE    0x08
#define NVME_INTERFACE_CODE   0x02

#define NVME_PCI_MAX_SUBORDINATE_NUMBER 0x10
#define FIRST_SECONDARY_BUS_NUMBER      1

#define INVALID_VENDOR_ID        0x0FFFF
#define MASK_MEM_DECODE_RANGE    0xFFFFFFFFFFFFFFF0
#define NVME_BAR_MASK            0x0FFFFFFFFFFFC000
#define CMD_ENABLE_MEM           0x06
#define NVME_LDWORD_BAR_MASK     0xFFFFFFFF
#define NVME_HDWORD_BAR_MASK     0xFFFFFFFF00000000
#define NVME_MASK_MEM_BUS_MASTER 0xF9

#pragma pack(1)

typedef struct {
    EFI_PEI_RECOVERY_BLOCK_IO_PPI  RecoveryBlkIo;
    BOOLEAN                        HaveEnumeratedDevices;
    UINTN                          DeviceCount;
    NVME_RECOVERY_DEVICE_INFO      *DeviceInfo[MAX_NVME_RECOVERY_DEVICE];
} NVME_RECOVERY_BLK_IO_DEV;

typedef struct {
    UINT8   BusNumber;
    UINT8   Device;
    UINT8   Function;
    BOOLEAN IsMmioDevice;
    UINT64  BaseAddress;
} NVME_PCI_DEVICE_INFO;

typedef struct {
    UINT8   PrimaryBusNumber;
    UINT8   Device;
    UINT8   Function;
    UINT8   IsMMIO;
    UINT16  MemIOBaseLimit;
} NVME_PCI_PROGRAMMED_BRIDGE_INFO;

typedef struct {
    UINT8   PrimaryBusNumber;
    UINT8   SecBusNumber;
    UINT8   SubBusNumber;
    UINT8   Device;
    UINT8   Function;
    BOOLEAN BridgeAlreadyProgrammed;
} NVME_PCI_BRIDGE_INFO;

typedef struct {
    UINT8   Bus;
    UINT8   Dev;
    UINT8   Func;
} NVME_PCI_ROOT_BRIDGE_LIST;

#pragma pack()

EFI_STATUS
EFIAPI
Nvme_GetBlockDeviceMediaInfo (
    IN  EFI_PEI_SERVICES              **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN  UINTN                         DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA        *MediaInfo
);

EFI_STATUS
EFIAPI
Nvme_ReadBlocks (
    IN  EFI_PEI_SERVICES              **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN  UINTN                         DeviceIndex,
    IN  EFI_PEI_LBA                   StartLba,
    IN  UINTN                         BufferSize,
    OUT VOID                          *Buffer
);

EFI_STATUS
EFIAPI
Nvme_GetNumberOfBlockDevices(
    IN  EFI_PEI_SERVICES              **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    OUT UINTN                         *NumberBlockDevices
);

EFI_STATUS
EFIAPI
NotifyOnNvmeRecoveryCapsuleLoaded (
    IN  EFI_PEI_SERVICES          **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN  VOID                      *InvokePpi
);

EFI_STATUS
EnumerateNvmeDevices(
    IN OUT NVME_RECOVERY_BLK_IO_DEV   *NvmeBlkIoDev
);

EFI_STATUS
InitializeNvmeController (
    NVME_RECOVERY_CONTROLLER_INFO *NvmeControllerInfo,
    NVME_PCI_DEVICE_INFO               *PciDeviceInfo
);

VOID
PrintExecuteNvmeCmdData (
     NVME_COMMAND_WRAPPER    *NvmeCmdWrapper
);

EFI_STATUS
EnumerateBus (
    IN  UINT8 Bus,
    IN  UINT64  BridgeMemBaseAddress
);

BOOLEAN
CheckIsBridgeProgrammed(
    IN  UINT8   Bus,
    IN  UINT8   Device,
    IN  UINT8   Function
);

VOID
ProgramSubordinateBusForBridgeAbove (
    IN  UINT8   PrimaryBusNo,
    IN  UINT8   SubordinateBusNo
);

EFI_STATUS
CheckforProgrammedBridgeorDevice (
    IN  UINT8  Bus,
    IN  UINT8  Device,
    IN  UINT8  Function,
    IN  UINT8  SecondaryBusNo,
    IN  UINT8  BridgeAlreadyProgrammed,
    IN  UINT64 BridgeDevBaseAddress
);

VOID
PcdNvmeRootBridgeResources(
    BOOLEAN Flag
);

VOID
FindPciMmioAddress(
    UINT64    *PciMmioStartAddress
);
#endif //_AMI_NVME_RECOVERY_H_

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
