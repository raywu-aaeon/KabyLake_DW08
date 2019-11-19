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
/** @file SbCspLib.h

**/

#ifndef __SBLIB_H__
#define __SBLIB_H__

#include <Token.h>
#include <Efi.h>
#include <Pei.h>
#include <AmiDxeLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiSio.h>
#include <AmiChipsetPkg/Include/Library/AmiChipsetIoLib.h>
#include "AmiGpioLib.h"
#include "AmiSbMiscLib.h"
#include "AmiCmosLib.h"
#include "AmiSmmLib.h"
#include "AmiPirqRouterLib.h"
#include "AmiCspFlashLib.h"
#include "AmiSmbiosLib.h"
#include "AmiHpetApicLib.h"

#if ACPI_SUPPORT
 #include <Protocol/S3SaveState.h>
#endif

#if CSM_SUPPORT
#include <Protocol/LegacyInterrupt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PCIBUS_VERSION
#define PCIBUS_VERSION ( PCI_BUS_MAJOR_VER * 10000 + \
                         PCI_BUS_MINOR_VER * 100 + \
                         PCI_BUS_REVISION )
#endif 

#define COMPLETE_SAVE_RESTORE_STD_CMOS  0x00
#define ENABLE_NMI_BEFORE_SMI_EXIT      0x01
#define DISABLE_NMI_BEFORE_SMI_EXIT     0x02

#ifndef EFI_SIGNATURE_16
#define EFI_SIGNATURE_16(A, B)  ((A) | (B << 8))
#endif

#ifndef EFI_SIGNATURE_32
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#endif

#ifndef PCI_VENDOR_ID_OFFSET
#define PCI_VENDOR_ID_OFFSET                        0x00
#endif

#ifndef PCI_DEVICE_ID_OFFSET
#define PCI_DEVICE_ID_OFFSET                        0x02
#endif

#ifndef PCI_COMMAND_OFFSET
#define PCI_COMMAND_OFFSET                          0x04
#endif

#ifndef PCI_REVISION_ID_OFFSET
#define PCI_REVISION_ID_OFFSET                      0x08
#endif

// Type Definition(s)

typedef struct {
    UINT64                      Address;
    EFI_BOOT_SCRIPT_WIDTH       Width;
    UINT32                      Mask;
} BOOT_SCRIPT_SB_PCI_REG_SAVE;

typedef enum {
    NO_EFFECT = 0,
    SMI_MODE,
    SCI_MODE,
    NMI_MODE,
    GPIO_ROUT_MODE_MAX
} SB_GPIO_ROUT_MODE;

//---------------------------------------------------------------------------

EFI_STATUS SbLibSetLpcDeviceDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type
);

EFI_STATUS SbLibSetLpcGenericDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable
);

//---------------------------------------------------------------------------
// Chipset PCI Macros, Porting Required.
//---------------------------------------------------------------------------

#define READ_PCI8_SB(Rx)          READ_PCI8(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx)
#define WRITE_PCI8_SB(Rx, Val)    WRITE_PCI8(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Val)
#define SET_PCI8_SB(Rx, Set)      SET_PCI8(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set)
#define RESET_PCI8_SB(Rx, Rst)    RESET_PCI8(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Rst)
#define RW_PCI8_SB(Rx, St, Rt)    RW_PCI8(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, St, Rt)
#define READ_PCI16_SB(Rx)         READ_PCI16(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx)
#define WRITE_PCI16_SB(Rx, Val)   WRITE_PCI16(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Val)
#define SET_PCI16_SB(Rx, Set)     SET_PCI16(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set)
#define RESET_PCI16_SB(Rx, Rst)   RESET_PCI16(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Rst)
#define RW_PCI16_SB(Rx, St, Rt)   RW_PCI16(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, St, Rt)
#define READ_PCI32_SB(Rx)         READ_PCI32(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx)
#define WRITE_PCI32_SB(Rx, Val)   WRITE_PCI32(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Val)
#define SET_PCI32_SB(Rx, Set)     SET_PCI32(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set)
#define RESET_PCI32_SB(Rx, Rst)   RESET_PCI32(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Rst)
#define RW_PCI32_SB(Rx, St, Rt)   RW_PCI32(SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, St, Rt)

//---------------------------------------------------------------------------

#define READ_PCI8_SATA(Rx) \
                         READ_PCI8(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx)
#define WRITE_PCI8_SATA(Rx, Val) \
                         WRITE_PCI8(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Val)
#define SET_PCI8_SATA(Rx, Set) \
                         SET_PCI8(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set)
#define RESET_PCI8_SATA(Rx, Rst) \
                         RESET_PCI8(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Rst)
#define RW_PCI8_SATA(Rx, Set, Rst) \
                         RW_PCI8(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set, Rst)
#define READ_PCI16_SATA(Rx) \
                         READ_PCI16(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx)
#define WRITE_PCI16_SATA(Rx, Val) \
                         WRITE_PCI16(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Val)
#define SET_PCI16_SATA(Rx, Set) \
                         SET_PCI16(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set)
#define RESET_PCI16_SATA(Rx, Rst) \
                         RESET_PCI16(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Rst)
#define RW_PCI16_SATA(Rx, Set, Rst) \
                         RW_PCI16(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set, Rst)
#define READ_PCI32_SATA(Rx) \
                         READ_PCI32(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx)
#define WRITE_PCI32_SATA(Rx, Val) \
                         WRITE_PCI32(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Val)
#define SET_PCI32_SATA(Rx, Set) \
                         SET_PCI32(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set)
#define RESET_PCI32_SATA(Rx, Rst) \
                         RESET_PCI32(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Rst)
#define RW_PCI32_SATA(Rx, Set, Rst) \
                         RW_PCI32(SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define READ_PCI8_PCIEBRS(Rx) \
                 READ_PCI8(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx)
#define WRITE_PCI8_PCIEBRS(Rx, Val) \
                 WRITE_PCI8(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Val)
#define SET_PCI8_PCIEBRS(Rx, Set) \
                 SET_PCI8(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set)
#define RESET_PCI8_PCIEBRS(Rx, Rst) \
                 RESET_PCI8(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Rst)
#define RW_PCI8_PCIEBRS(Rx, Set, Rst) \
                 RW_PCI8(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set, Rst)
#define READ_PCI16_PCIEBRS(Rx) \
                 READ_PCI16(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx)
#define WRITE_PCI16_PCIEBRS(Rx, Val) \
                 WRITE_PCI16(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Val)
#define SET_PCI16_PCIEBRS(Rx, Set) \
                 SET_PCI16(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set)
#define RESET_PCI16_PCIEBRS(Rx, Rst) \
                 RESET_PCI16(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Rst)
#define RW_PCI16_PCIEBRS(Rx, Set, Rst) \
                 RW_PCI16(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set, Rst)
#define READ_PCI32_PCIEBRS(Rx) \
                 READ_PCI32(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx)
#define WRITE_PCI32_PCIEBRS(Rx, Val) \
                 WRITE_PCI32(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Val)
#define SET_PCI32_PCIEBRS(Rx, Set) \
                 SET_PCI32(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set)
#define RESET_PCI32_PCIEBRS(Rx, Rst) \
                 RESET_PCI32(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Rst)
#define RW_PCI32_PCIEBRS(Rx, Set, Rst) \
                 RW_PCI32(PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define READ_PCI8_PCIBR(Rx) \
                      READ_PCI8(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx)
#define WRITE_PCI8_PCIBR(Rx, Val) \
                      WRITE_PCI8(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Val)
#define SET_PCI8_PCIBR(Rx, Set) \
                      SET_PCI8(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set)
#define RESET_PCI8_PCIBR(Rx, Rst) \
                      RESET_PCI8(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Rst)
#define RW_PCI8_PCIBR(Rx, Set, Rst) \
                      RW_PCI8(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set, Rst)
#define READ_PCI16_PCIBR(Rx) \
                      READ_PCI16(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx)
#define WRITE_PCI16_PCIBR(Rx, Val) \
                      WRITE_PCI16(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Val)
#define SET_PCI16_PCIBR(Rx, Set) \
                      SET_PCI16(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set)
#define RESET_PCI16_PCIBR(Rx, Rst) \
                      RESET_PCI16(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Rst)
#define RW_PCI16_PCIBR(Rx, Set, Rst) \
                      RW_PCI16(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set, Rst)
#define READ_PCI32_PCIBR(Rx) \
                      READ_PCI32(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx)
#define WRITE_PCI32_PCIBR(Rx, Val) \
                      WRITE_PCI32(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Val)
#define SET_PCI32_PCIBR(Rx, Set) \
                      SET_PCI32(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set)
#define RESET_PCI32_PCIBR(Rx, Rst) \
                      RESET_PCI32(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Rst)
#define RW_PCI32_PCIBR(Rx, Set, Rst) \
                      RW_PCI32(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define READ_PCI8_XHCI(Rx) \
                         READ_PCI8(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx)
#define WRITE_PCI8_XHCI(Rx, Val) \
                         WRITE_PCI8(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Val)
#define SET_PCI8_XHCI(Rx, Set) \
                         SET_PCI8(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set)
#define RESET_PCI8_XHCI(Rx, Rst) \
                         RESET_PCI8(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Rst)
#define RW_PCI8_XHCI(Rx, Set, Rst) \
                         RW_PCI8(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set, Rst)
#define READ_PCI16_XHCI(Rx) \
                         READ_PCI16(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx)
#define WRITE_PCI16_XHCI(Rx, Val) \
                         WRITE_PCI16(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Val)
#define SET_PCI16_XHCI(Rx, Set) \
                         SET_PCI16(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set)
#define RESET_PCI16_XHCI(Rx, Rst) \
                         RESET_PCI16(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Rst)
#define RW_PCI16_XHCI(Rx, Set, Rst) \
                         RW_PCI16(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set, Rst)
#define READ_PCI32_XHCI(Rx) \
                         READ_PCI32(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx)
#define WRITE_PCI32_XHCI(Rx, Val) \
                         WRITE_PCI32(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Val)
#define SET_PCI32_XHCI(Rx, Set) \
                         SET_PCI32(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set)
#define RESET_PCI32_XHCI(Rx, Rst) \
                         RESET_PCI32(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Rst)
#define RW_PCI32_XHCI(Rx, Set, Rst) \
                         RW_PCI32(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define READ_PCI8_LAN(Rx)       READ_PCI8(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx)
#define WRITE_PCI8_LAN(Rx, Vx)  WRITE_PCI8(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Vx)
#define SET_PCI8_LAN(Rx, Set)   SET_PCI8(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set)
#define RESET_PCI8_LAN(Rx, Rt)  RESET_PCI8(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Rt)
#define RW_PCI8_LAN(Rx,St,Rt)   RW_PCI8(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, St, Rt)
#define READ_PCI16_LAN(Rx)      READ_PCI16(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx)
#define WRITE_PCI16_LAN(Rx, Vx) WRITE_PCI16(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Vx)
#define SET_PCI16_LAN(Rx, Set)  SET_PCI16(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set)
#define RESET_PCI16_LAN(Rx, Rt) RESET_PCI16(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Rt)
#define RW_PCI16_LAN(Rx,St,Rt)  RW_PCI16(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, St,Rt)
#define READ_PCI32_LAN(Rx)      READ_PCI32(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx)
#define WRITE_PCI32_LAN(Rx, Vx) WRITE_PCI32(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Vx)
#define SET_PCI32_LAN(Rx, Set)  SET_PCI32(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set)
#define RESET_PCI32_LAN(Rx, Rt) RESET_PCI32(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Rt)
#define RW_PCI32_LAN(Rx,St,Rt)  RW_PCI32(SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, St,Rt)

//---------------------------------------------------------------------------

#define READ_PCI8_HDA(Rx)       READ_PCI8(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx)
#define WRITE_PCI8_HDA(Rx, Vx)  WRITE_PCI8(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Vx)
#define SET_PCI8_HDA(Rx, Set)   SET_PCI8(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set)
#define RESET_PCI8_HDA(Rx, Rt)  RESET_PCI8(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Rt)
#define RW_PCI8_HDA(Rx,St,Rt)   RW_PCI8(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, St, Rt)
#define READ_PCI16_HDA(Rx)      READ_PCI16(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx)
#define WRITE_PCI16_HDA(Rx, Vx) WRITE_PCI16(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Vx)
#define SET_PCI16_HDA(Rx, Set)  SET_PCI16(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set)
#define RESET_PCI16_HDA(Rx, Rt) RESET_PCI16(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Rt)
#define RW_PCI16_HDA(Rx,St,Rt)  RW_PCI16(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, St,Rt)
#define READ_PCI32_HDA(Rx)      READ_PCI32(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx)
#define WRITE_PCI32_HDA(Rx, Vx) WRITE_PCI32(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Vx)
#define SET_PCI32_HDA(Rx, Set)  SET_PCI32(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set)
#define RESET_PCI32_HDA(Rx, Rt) RESET_PCI32(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Rt)
#define RW_PCI32_HDA(Rx,St,Rt)  RW_PCI32(SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, St,Rt)

//---------------------------------------------------------------------------

#define READ_PCI8_SMBUS(Rx) \
                       READ_PCI8(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx)
#define WRITE_PCI8_SMBUS(Rx, Val) \
                       WRITE_PCI8(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Val)
#define SET_PCI8_SMBUS(Rx, Set) \
                       SET_PCI8(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set)
#define RESET_PCI8_SMBUS(Rx, Rst) \
                       RESET_PCI8(SB_SMBUS_BUS, SMBUS_DEV, SB_SMBUS_FUN, Rx, Rst)
#define RW_PCI8_SMBUS(Rx, Set, Rst) \
                       RW_PCI8(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set, Rst)
#define READ_PCI16_SMBUS(Rx) \
                       READ_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx)
#define WRITE_PCI16_SMBUS(Rx, Val) \
                       WRITE_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Val)
#define SET_PCI16_SMBUS(Rx, Set) \
                       SET_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set)
#define RESET_PCI16_SMBUS(Rx, Rst) \
                       RESET_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Rst)
#define RW_PCI16_SMBUS(Rx, Set, Rst) \
                       RW_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set, Rst)
#define READ_PCI32_SMBUS(Rx) \
                       READ_PCI32(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx)
#define WRITE_PCI32_SMBUS(Rx, Val) \
                       WRITE_PCI32(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Val)
#define SET_PCI32_SMBUS(Rx, Set) \
                       SET_PCI32(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set)
#define RESET_PCI32_SMBUS(Rx, Rst) \
                       RESET_PCI32(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Rst)
#define RW_PCI32_SMBUS(Rx, Set, Rst) \
                       RW_PCI32(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define READ_PCI8_HECI(Rx) \
                         READ_PCI8(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx)
#define WRITE_PCI8_HECI(Rx, Val) \
                         WRITE_PCI8(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Val)
#define SET_PCI8_HECI(Rx, Set) \
                         SET_PCI8(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set)
#define RESET_PCI8_HECI(Rx, Rst) \
                         RESET_PCI8(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Rst)
#define RW_PCI8_HECI(Rx, Set, Rst) \
                         RW_PCI8(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set, Rst)
#define READ_PCI16_HECI(Rx) \
                         READ_PCI16(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx)
#define WRITE_PCI16_HECI(Rx, Val) \
                         WRITE_PCI16(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Val)
#define SET_PCI16_HECI(Rx, Set) \
                         SET_PCI16(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set)
#define RESET_PCI16_HECI(Rx, Rst) \
                         RESET_PCI16(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Rst)
#define RW_PCI16_HECI(Rx, Set, Rst) \
                         RW_PCI16(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set, Rst)
#define READ_PCI32_HECI(Rx) \
                         READ_PCI32(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx)
#define WRITE_PCI32_HECI(Rx, Val) \
                         WRITE_PCI32(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Val)
#define SET_PCI32_HECI(Rx, Set) \
                         SET_PCI32(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set)
#define RESET_PCI32_HECI(Rx, Rst) \
                         RESET_PCI32(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Rst)
#define RW_PCI32_HECI(Rx, Set, Rst) \
                         RW_PCI32(SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define READ_PCI8_HECI2(Rx) \
                         READ_PCI8(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx)
#define WRITE_PCI8_HECI2(Rx, Val) \
                         WRITE_PCI8(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Val)
#define SET_PCI8_HECI2(Rx, Set) \
                         SET_PCI8(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set)
#define RESET_PCI8_HECI2(Rx, Rst) \
                         RESET_PCI8(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Rst)
#define RW_PCI8_HECI2(Rx, Set, Rst) \
                         RW_PCI8(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set, Rst)
#define READ_PCI16_HECI2(Rx) \
                         READ_PCI16(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx)
#define WRITE_PCI16_HECI2(Rx, Val) \
                         WRITE_PCI16(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Val)
#define SET_PCI16_HECI2(Rx, Set) \
                         SET_PCI16(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set)
#define RESET_PCI16_HECI2(Rx, Rst) \
                         RESET_PCI16(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Rst)
#define RW_PCI16_HECI2(Rx, Set, Rst) \
                         RW_PCI16(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set, Rst)
#define READ_PCI32_HECI2(Rx) \
                         READ_PCI32(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx)
#define WRITE_PCI32_HECI2(Rx, Val) \
                         WRITE_PCI32(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Val)
#define SET_PCI32_HECI2(Rx, Set) \
                         SET_PCI32(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set)
#define RESET_PCI32_HECI2(Rx, Rst) \
                         RESET_PCI32(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Rst)
#define RW_PCI32_HECI2(Rx, Set, Rst) \
                         RW_PCI32(SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define WRITE_PCI8_SB_S3(mBoot, Rx, Val) \
                     WRITE_PCI8_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Val)
#define SET_PCI8_SB_S3(mBoot, Rx, Set) \
                     SET_PCI8_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set)
#define RESET_PCI8_SB_S3(mBoot, Rx, Reset) \
                     RESET_PCI8_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Reset)
#define RW_PCI8_SB_S3(mBoot, Rx, Set, Rst) \
                     RW_PCI8_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set, Rst)
#define WRITE_PCI16_SB_S3(mBoot, Rx, Val) \
                     WRITE_PCI16_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Val)
#define SET_PCI16_SB_S3(mBoot, Rx, Set) \
                     SET_PCI16_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set)
#define RESET_PCI16_SB_S3(mBoot, Rx, Reset) \
                     RESET_PCI16_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Reset)
#define RW_PCI16_SB_S3(mBoot, Rx, Set, Rst) \
                     RW_PCI16_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set, Rst)
#define WRITE_PCI32_SB_S3(mBoot, Rx, Val) \
                     WRITE_PCI32_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Val)
#define SET_PCI32_SB_S3(mBoot, Rx, Set) \
                     SET_PCI32_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set)
#define RESET_PCI32_SB_S3(mBoot, Rx, Reset) \
                     RESET_PCI32_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Reset)
#define RW_PCI32_SB_S3(mBoot, Rx, Set, Rst) \
                     RW_PCI32_S3(mBoot, SB_LPC_BUS, SB_LPC_DEV, SB_LPC_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define WRITE_PCI8_SATA_S3(mBoot, Rx, Val) \
                WRITE_PCI8_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Val)
#define SET_PCI8_SATA_S3(mBoot, Rx, Set) \
                SET_PCI8_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set)
#define RESET_PCI8_SATA_S3(mBoot, Rx, Rst) \
                RESET_PCI8_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Rst)
#define RW_PCI8_SATA_S3(mBoot, Rx, Set, Rst) \
                RW_PCI8_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set, Rst)
#define WRITE_PCI16_SATA_S3(mBoot, Rx, Val) \
                WRITE_PCI16_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Val)
#define SET_PCI16_SATA_S3(mBoot, Rx, Set) \
                SET_PCI16_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set)
#define RESET_PCI16_SATA_S3(mBoot, Rx, Rst) \
                RESET_PCI16_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Rst)
#define RW_PCI16_SATA_S3(mBoot, Rx, Set, Rst) \
                RW_PCI16_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set, Rst)
#define WRITE_PCI32_SATA_S3(mBoot, Rx, Val) \
                WRITE_PCI32_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Val)
#define SET_PCI32_SATA_S3(mBoot, Rx, Set) \
                SET_PCI32_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set)
#define RESET_PCI32_SATA_S3(mBoot, Rx, Rst) \
                RESET_PCI32_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Rst)
#define RW_PCI32_SATA_S3(mBoot, Rx, Set, Rst) \
                RW_PCI32_S3(mBoot, SB_SATA_BUS, SB_SATA_DEV, SB_SATA_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define WRITE_PCI8_PCIEBRS_S3(mBoot, Rx, Val) \
         WRITE_PCI8_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Val)
#define SET_PCI8_PCIEBRS_S3(mBoot, Rx, Set) \
         SET_PCI8_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set)
#define RESET_PCI8_PCIEBRS_S3(mBoot, Rx, Rst) \
         RESET_PCI8_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Rst)
#define RW_PCI8_PCIEBRS_S3(mBoot, Rx, St, Rt) \
         RW_PCI8_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, St, Rt)
#define WRITE_PCI16_PCIEBRS_S3(mBoot, Rx, Val) \
         WRITE_PCI16_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Val)
#define SET_PCI16_PCIEBRS_S3(mBoot, Rx, Set) \
         SET_PCI16_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set)
#define RESET_PCI16_PCIEBRS_S3(mBoot, Rx, Rst) \
         RESET_PCI16_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Rst)
#define RW_PCI16_PCIEBRS_S3(mBoot, Rx, St, Rt) \
         RW_PCI16_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, St, Rt)
#define WRITE_PCI32_PCIEBRS_S3(mBoot, Rx, Val) \
         WRITE_PCI32_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Val)
#define SET_PCI32_PCIEBRS_S3(mBoot, Rx, Set) \
         SET_PCI32_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Set)
#define RESET_PCI32_PCIEBRS_S3(mBoot, Rx, Rst) \
         RESET_PCI32_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, Rst)
#define RW_PCI32_PCIEBRS_S3(mBoot, Rx, St, Rt) \
         RW_PCI32_S3(mBoot, PCIEBRS_BUS, PCIEBRS_DEV, PCIEBRS_FUN, Rx, St, Rt)

//---------------------------------------------------------------------------
/* remove
#define WRITE_PCI8_PCIBR_S3(mBoot, Rx, Val) \
               WRITE_PCI8_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Val)
#define SET_PCI8_PCIBR_S3(mBoot, Rx, Set) \
               SET_PCI8_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set)
#define RESET_PCI8_PCIBR_S3(mBoot, Rx, Rst) \
               RESET_PCI8_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Rst)
#define RW_PCI8_PCIBR_S3(mBoot, Rx, St, Rt) \
               RW_PCI8_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, St, Rt)
#define WRITE_PCI16_PCIBR_S3(mBoot, Rx, Val) \
               WRITE_PCI16_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Val)
#define SET_PCI16_PCIBR_S3(mBoot, Rx, Set) \
               SET_PCI16_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set)
#define RESET_PCI16_PCIBR_S3(mBoot, Rx, Rst) \
               RESET_PCI16_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Rst)
#define RW_PCI16_PCIBR_S3(mBoot, Rx, St, Rt) \
               RW_PCI16_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, St, Rt)
#define WRITE_PCI32_PCIBR_S3(mBoot, Rx, Val) \
               WRITE_PCI32_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Val)
#define SET_PCI32_PCIBR_S3(mBoot, Rx, Set) \
               SET_PCI32_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Set)
#define RESET_PCI32_PCIBR_S3(mBoot, Rx, Rst) \
               RESET_PCI32_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, Rst)
#define RW_PCI32_PCIBR_S3(mBoot, Rx, St, Rt) \
               RW_PCI32_S3(mBoot, PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, Rx, St, Rt)
*/
//---------------------------------------------------------------------------

#define WRITE_PCI8_XHCI_S3(mBoot, Rx, Val) \
                WRITE_PCI8_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Val)
#define SET_PCI8_XHCI_S3(mBoot, Rx, Set) \
                SET_PCI8_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set)
#define RESET_PCI8_XHCI_S3(mBoot, Rx, Rst) \
                RESET_PCI8_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Rst)
#define RW_PCI8_XHCI_S3(mBoot, Rx, Set, Rst) \
                RW_PCI8_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set, Rst)
#define WRITE_PCI16_XHCI_S3(mBoot, Rx, Val) \
                WRITE_PCI16_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Val)
#define SET_PCI16_XHCI_S3(mBoot, Rx, Set) \
                SET_PCI16_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set)
#define RESET_PCI16_XHCI_S3(mBoot, Rx, Rst) \
                RESET_PCI16_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Rst)
#define RW_PCI16_XHCI_S3(mBoot, Rx, Set, Rst) \
                RW_PCI16_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set, Rst)
#define WRITE_PCI32_XHCI_S3(mBoot, Rx, Val) \
                WRITE_PCI32_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Val)
#define SET_PCI32_XHCI_S3(mBoot, Rx, Set) \
                SET_PCI32_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set)
#define RESET_PCI32_XHCI_S3(mBoot, Rx, Rst) \
                RESET_PCI32_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Rst)
#define RW_PCI32_XHCI_S3(mBoot, Rx, Set, Rst) \
                RW_PCI32_S3(mBoot, SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define WRITE_PCI8_LAN_S3(mBoot, Rx, Val) \
                   WRITE_PCI8_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Val)
#define SET_PCI8_LAN_S3(mBoot, Rx, Set) \
                   SET_PCI8_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set)
#define RESET_PCI8_LAN_S3(mBoot, Rx, Rst) \
                   RESET_PCI8_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Rst)
#define RW_PCI8_LAN_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI8_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set, Rst)
#define WRITE_PCI16_LAN_S3(mBoot, Rx, Val) \
                   WRITE_PCI16_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Val)
#define SET_PCI16_LAN_S3(mBoot, Rx, Set) \
                   SET_PCI16_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set)
#define RESET_PCI16_LAN_S3(mBoot, Rx, Rst) \
                   RESET_PCI16_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Rst)
#define RW_PCI16_LAN_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI16_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set, Rst)
#define WRITE_PCI32_LAN_S3(mBoot, Rx, Val) \
                   WRITE_PCI32_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Val)
#define SET_PCI32_LAN_S3(mBoot, Rx, Set) \
                   SET_PCI32_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set)
#define RESET_PCI32_LAN_S3(mBoot, Rx, Rst) \
                   RESET_PCI32_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Rst)
#define RW_PCI32_LAN_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI32_S3(mBoot, SB_LAN_BUS, SB_LAN_DEV, SB_LAN_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define WRITE_PCI8_HDA_S3(mBoot, Rx, Val) \
                   WRITE_PCI8_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Val)
#define SET_PCI8_HDA_S3(mBoot, Rx, Set) \
                   SET_PCI8_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set)
#define RESET_PCI8_HDA_S3(mBoot, Rx, Rst) \
                   RESET_PCI8_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Rst)
#define RW_PCI8_HDA_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI8_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set, Rst)
#define WRITE_PCI16_HDA_S3(mBoot, Rx, Val) \
                   WRITE_PCI16_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Val)
#define SET_PCI16_HDA_S3(mBoot, Rx, Set) \
                   SET_PCI16_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set)
#define RESET_PCI16_HDA_S3(mBoot, Rx, Rst) \
                   RESET_PCI16_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Rst)
#define RW_PCI16_HDA_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI16_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set, Rst)
#define WRITE_PCI32_HDA_S3(mBoot, Rx, Val) \
                   WRITE_PCI32_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Val)
#define SET_PCI32_HDA_S3(mBoot, Rx, Set) \
                   SET_PCI32_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set)
#define RESET_PCI32_HDA_S3(mBoot, Rx, Rst) \
                   RESET_PCI32_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Rst)
#define RW_PCI32_HDA_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI32_S3(mBoot, SB_HDA_BUS, SB_HDA_DEV, SB_HDA_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define WRITE_PCI8_SMBUS_S3(mBoot, Rx, Val) \
               WRITE_PCI8_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Val)
#define SET_PCI8_SMBUS_S3(mBoot, Rx, Set) \
               SET_PCI8_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set)
#define RESET_PCI8_SMBUS_S3(mBoot, Rx, Rst) \
               RESET_PCI8_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Rst)
#define RW_PCI8_SMBUS_S3(mBoot, Rx, St, Rt) \
               RW_PCI8_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, St, Rt)
#define WRITE_PCI16_SMBUS_S3(mBoot, Rx, Val) \
               WRITE_PCI16_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Val)
#define SET_PCI16_SMBUS_S3(mBoot, Rx, Set) \
               SET_PCI16_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set)
#define RESET_PCI16_SMBUS_S3(mBoot, Rx, Rst) \
               RESET_PCI16_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Rst)
#define RW_PCI16_SMBUS_S3(mBoot, Rx, St, Rt) \
               RW_PCI16_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, St, Rt)
#define WRITE_PCI32_SMBUS_S3(mBoot, Rx, Val) \
               WRITE_PCI32_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Val)
#define SET_PCI32_SMBUS_S3(mBoot, Rx, Set) \
               SET_PCI32_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Set)
#define RESET_PCI32_SMBUS_S3(mBoot, Rx, Rst) \
               RESET_PCI32_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, Rst)
#define RW_PCI32_SMBUS_S3(mBoot, Rx, St, Rt) \
               RW_PCI32_S3(mBoot, SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, Rx, St, Rt)

//---------------------------------------------------------------------------

#define WRITE_PCI8_HECI_S3(mBoot, Rx, Val) \
                WRITE_PCI8_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Val)
#define SET_PCI8_HECI_S3(mBoot, Rx, Set) \
                SET_PCI8_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set)
#define RESET_PCI8_HECI_S3(mBoot, Rx, Rst) \
                RESET_PCI8_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Rst)
#define RW_PCI8_HECI_S3(mBoot, Rx, Set, Rst) \
                RW_PCI8_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set, Rst)
#define WRITE_PCI16_HECI_S3(mBoot, Rx, Val) \
                WRITE_PCI16_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Val)
#define SET_PCI16_HECI_S3(mBoot, Rx, Set) \
                SET_PCI16_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set)
#define RESET_PCI16_HECI_S3(mBoot, Rx, Rst) \
                RESET_PCI16_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Rst)
#define RW_PCI16_HECI_S3(mBoot, Rx, Set, Rst) \
                RW_PCI16_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set, Rst)
#define WRITE_PCI32_HECI_S3(mBoot, Rx, Val) \
                WRITE_PCI32_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Val)
#define SET_PCI32_HECI_S3(mBoot, Rx, Set) \
                SET_PCI32_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set)
#define RESET_PCI32_HECI_S3(mBoot, Rx, Rst) \
                RESET_PCI32_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Rst)
#define RW_PCI32_HECI_S3(mBoot, Rx, Set, Rst) \
                RW_PCI32_S3(mBoot, SB_HECI_BUS, SB_HECI_DEV, SB_HECI_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------

#define WRITE_PCI8_HECI2_S3(mBoot, Rx, Val) \
                WRITE_PCI8_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Val)
#define SET_PCI8_HECI2_S3(mBoot, Rx, Set) \
                SET_PCI8_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set)
#define RESET_PCI8_HECI2_S3(mBoot, Rx, Rst) \
                RESET_PCI8_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Rst)
#define RW_PCI8_HECI2_S3(mBoot, Rx, Set, Rst) \
                RW_PCI8_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set, Rst)
#define WRITE_PCI16_HECI2_S3(mBoot, Rx, Val) \
                WRITE_PCI16_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Val)
#define SET_PCI16_HECI2_S3(mBoot, Rx, Set) \
                SET_PCI16_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set)
#define RESET_PCI16_HECI2_S3(mBoot, Rx, Rst) \
                RESET_PCI16_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Rst)
#define RW_PCI16_HECI2_S3(mBoot, Rx, Set, Rst) \
                RW_PCI16_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set, Rst)
#define WRITE_PCI32_HECI2_S3(mBoot, Rx, Val) \
                WRITE_PCI32_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Val)
#define SET_PCI32_HECI2_S3(mBoot, Rx, Set) \
                SET_PCI32_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set)
#define RESET_PCI32_HECI2_S3(mBoot, Rx, Rst) \
                RESET_PCI32_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Rst)
#define RW_PCI32_HECI2_S3(mBoot, Rx, Set, Rst) \
                RW_PCI32_S3(mBoot, SB_HECI2_BUS, SB_HECI2_DEV, SB_HECI2_FUN, Rx, Set, Rst)

//---------------------------------------------------------------------------
// SPI MMIO Macros, Porting Required.
//---------------------------------------------------------------------------
/*
#define READ_MEM8_SPI(wReg)        READ_MEM8(SPI_BASE | wReg)
#define WRITE_MEM8_SPI(wReg, bVal) WRITE_MEM8(SPI_BASE | wReg,bVal)
#define SET_MEM8_SPI(wReg, Set)    RW_MEM8(SPI_BASE | wReg, Set, 0)
#define RESET_MEM8_SPI(wReg, Rst)  RW_MEM8(SPI_BASE | wReg,0,Rst)
#define RW_MEM8_SPI(wReg,Set,Rst)  RW_MEM8(SPI_BASE |wReg,Set,Rst)
#define READ_MEM16_SPI(wReg)       READ_MEM16(SPI_BASE | wReg)
#define WRITE_MEM16_SPI(wReg,Val)  WRITE_MEM16(SPI_BASE|wReg,Val)
#define SET_MEM16_SPI(wReg, Set)   RW_MEM16(SPI_BASE|wReg, Set,0)
#define RESET_MEM16_SPI(wReg, Rst) RW_MEM16(SPI_BASE|wReg, 0,Rst)
#define RW_MEM16_SPI(Reg,Set,Rst)  RW_MEM16(SPI_BASE|Reg,Set,Rst)
#define READ_MEM32_SPI(wReg)       READ_MEM32(SPI_BASE | wReg)
#define WRITE_MEM32_SPI(wReg,Val)  WRITE_MEM32(SPI_BASE|wReg,Val)
#define SET_MEM32_SPI(wReg,Set)    RW_MEM32(SPI_BASE|wReg, Set,0)
#define RESET_MEM32_SPI(wReg,Rst)  RW_MEM32(SPI_BASE|wReg,0,Rst)
#define RW_MEM32_SPI(Reg,Set,Rst)  RW_MEM32(SPI_BASE|Reg,Set,Rst)
*/
//---------------------------------------------------------------------------
// Chipset I/O Macros, Porting Required.
//---------------------------------------------------------------------------

#define READ_IO8_PM(bReg)           READ_IO8(PM_BASE_ADDRESS+bReg)
#define WRITE_IO8_PM(bReg, bVal)    WRITE_IO8(PM_BASE_ADDRESS+bReg, bVal)
#define SET_IO8_PM(bReg, Set)       SET_IO8(PM_BASE_ADDRESS+bReg, Set)
#define RESET_IO8_PM(bReg, Reset)   RESET_IO8(PM_BASE_ADDRESS+bReg, Reset)
#define RW_IO8_PM(bReg, Set, Rst)   RW_IO8(PM_BASE_ADDRESS+bReg, Set, Rst)
#define READ_IO16_PM(bReg)          READ_IO16(PM_BASE_ADDRESS+bReg)
#define WRITE_IO16_PM(bReg, wVal)   WRITE_IO16(PM_BASE_ADDRESS+bReg, wVal)
#define SET_IO16_PM(bReg, Set)      SET_IO16(PM_BASE_ADDRESS+bReg, Set)
#define RESET_IO16_PM(bReg, Reset)  RESET_IO16(PM_BASE_ADDRESS+bReg, Reset)
#define RW_IO16_PM(bReg, Set, Rst)  RW_IO16(PM_BASE_ADDRESS+bReg, Set, Rst)
#define READ_IO32_PM(bReg)          READ_IO32(PM_BASE_ADDRESS+bReg)
#define WRITE_IO32_PM(bReg, dVal)   WRITE_IO32(PM_BASE_ADDRESS+bReg, dVal)
#define SET_IO32_PM(bReg, Set)      SET_IO32(PM_BASE_ADDRESS+bReg, Set)
#define RESET_IO32_PM(bReg, Reset)  RESET_IO32(PM_BASE_ADDRESS+bReg, Reset)
#define RW_IO32_PM(bReg, Set, Rst)  RW_IO32(PM_BASE_ADDRESS+bReg, Set, Rst)

//---------------------------------------------------------------------------

#define READ_IO8_TCO(bReg)          READ_IO8(TCO_BASE_ADDRESS+bReg)
#define WRITE_IO8_TCO(bReg, bVal)   WRITE_IO8(TCO_BASE_ADDRESS+bReg, bVal)
#define SET_IO8_TCO(bReg, Set)      SET_IO8(TCO_BASE_ADDRESS+bReg, Set)
#define RESET_IO8_TCO(bReg, Reset)  RESET_IO8(TCO_BASE_ADDRESS+bReg, Reset)
#define RW_IO8_TCO(bReg, Set, Rst)  RW_IO8(TCO_BASE_ADDRESS+bReg, Set, Rst)
#define READ_IO16_TCO(bReg)         READ_IO16(TCO_BASE_ADDRESS+bReg)
#define WRITE_IO16_TCO(bReg, wVal)  WRITE_IO16(TCO_BASE_ADDRESS+bReg, wVal)
#define SET_IO16_TCO(bReg, Set)     SET_IO16(TCO_BASE_ADDRESS+bReg, Set)
#define RESET_IO16_TCO(bReg, Reset) RESET_IO16(TCO_BASE_ADDRESS+bReg, Reset)
#define RW_IO16_TCO(bReg, Set, Rst) RW_IO16(TCO_BASE_ADDRESS+bReg, Set, Rst)
#define READ_IO32_TCO(bReg)         READ_IO32(TCO_BASE_ADDRESS+bReg)
#define WRITE_IO32_TCO(bReg, dVal)  WRITE_IO32(TCO_BASE_ADDRESS+bReg, dVal)
#define SET_IO32_TCO(bReg, Set)     SET_IO32(TCO_BASE_ADDRESS+bReg, Set)
#define RESET_IO32_TCO(bReg, Reset) RESET_IO32(TCO_BASE_ADDRESS+bReg, Reset)
#define RW_IO32_TCO(bReg, Set, Rst) RW_IO32(TCO_BASE_ADDRESS+bReg, Set, Rst)

//---------------------------------------------------------------------------

#define WRITE_IO8_PM_S3(mBoot, bReg, bVal) \
                            WRITE_IO8_S3(mBoot, PM_BASE_ADDRESS+bReg, bVal)
#define RW_IO8_PM_S3(mBoot, bReg, Set, Reset) \
                            RW_IO8_S3(mBoot, PM_BASE_ADDRESS+bReg, Set, Reset)
#define SET_IO8_PM_S3(mBoot, bReg, Set) \
                            RW_IO8_S3(mBoot, PM_BASE_ADDRESS+bReg, Set, 0)
#define RESET_IO8_PM_S3(mBoot, bReg, Reset) \
                            RW_IO8_S3(mBoot, PM_BASE_ADDRESS+bReg, 0, Reset)

#define WRITE_IO16_PM_S3(mBoot, bReg, bVal) \
                            WRITE_IO16_S3(mBoot, PM_BASE_ADDRESS+bReg, bVal)
#define RW_IO16_PM_S3(mBoot, bReg, Set, Rst) \
                            RW_IO16_S3(mBoot, PM_BASE_ADDRESS+bReg, Set, Rst)
#define SET_IO16_PM_S3(mBoot, bReg, Set) \
                            RW_IO16_S3(mBoot, PM_BASE_ADDRESS+bReg, Set, 0)
#define RESET_IO16_PM_S3(mBoot, bReg, Reset) \
                            RW_IO16_S3(mBoot, PM_BASE_ADDRESS+bReg, 0, Reset)

#define WRITE_IO32_PM_S3(mBoot, bReg, bVal) \
                            WRITE_IO32_S3(mBoot, PM_BASE_ADDRESS+bReg, bVal)
#define RW_IO32_PM_S3(mBoot, bReg, Set, Rst) \
                            RW_IO32_S3(mBoot, PM_BASE_ADDRESS+bReg, Set, Rst)
#define SET_IO32_PM_S3(mBoot, bReg, Set) \
                            RW_IO32_S3(mBoot, PM_BASE_ADDRESS+bReg, Set, 0)
#define RESET_IO32_PM_S3(mBoot, bReg, Reset) \
                            RW_IO32_S3(mBoot, PM_BASE_ADDRESS+bReg, 0, Reset)

//---------------------------------------------------------------------------

#define WRITE_IO8_TCO_S3(mBoot, bReg, bVal) \
                            WRITE_IO8_S3(mBoot, TCO_BASE_ADDRESS+bReg, bVal)
#define RW_IO8_TCO_S3(mBoot, bReg, Set, Rst) \
                            RW_IO8_S3(mBoot, TCO_BASE_ADDRESS+bReg, Set, Rst)
#define SET_IO8_TCO_S3(mBoot, bReg, Set) \
                            RW_IO8_S3(mBoot, TCO_BASE_ADDRESS+bReg, Set, 0)
#define RESET_IO8_TCO_S3(mBoot, bReg, Reset) \
                            RW_IO8_S3(mBoot, TCO_BASE_ADDRESS+bReg, 0, Reset)

#define WRITE_IO16_TCO_S3(mBoot, bReg, bVal) \
                            WRITE_IO16_S3(mBoot, TCO_BASE_ADDRESS+bReg, bVal)
#define RW_IO16_TCO_S3(mBoot, bReg, Set, Rst) \
                            RW_IO16_S3(mBoot, TCO_BASE_ADDRESS+bReg, Set, Rst)
#define SET_IO16_TCO_S3(mBoot, bReg, Set) \
                            RW_IO16_S3(mBoot, TCO_BASE_ADDRESS+bReg, Set, 0)
#define RESET_IO16_TCO_S3(mBoot, bReg, Reset) \
                            RW_IO16_S3(mBoot, TCO_BASE_ADDRESS+bReg, 0, Reset)

#define WRITE_IO32_TCO_S3(mBoot, bReg, bVal) \
                            WRITE_IO32_S3(mBoot, TCO_BASE_ADDRESS+bReg, bVal)
#define RW_IO32_TCO_S3(mBoot, bReg, Set, Rst) \
                            RW_IO32_S3(mBoot, TCO_BASE_ADDRESS+bReg, Set, Rst)
#define SET_IO32_TCO_S3(mBoot, bReg, Set) \
                            RW_IO32_S3(mBoot, TCO_BASE_ADDRESS+bReg, Set, 0)
#define RESET_IO32_TCO_S3(mBoot, bReg, Reset) \
                            RW_IO32_S3(mBoot, TCO_BASE_ADDRESS+bReg, 0, Reset)

//---------------------------------------------------------------------------

#define READ_IO8_RTC(bReg)          ReadIo8IdxData(CMOS_ADDR_PORT, bReg)
#define WRITE_IO8_RTC(bReg, bVal)   WriteIo8IdxData(CMOS_ADDR_PORT, bReg, bVal)
#define RW_IO8_RTC(bReg, Set, Rst)  RwIo8IdxData(CMOS_ADDR_PORT, bReg, Set, Rst)
#define SET_IO8_RTC(bReg, Set)      RwIo8IdxData(CMOS_ADDR_PORT, bReg, Set, 0)
#define RESET_IO8_RTC(bReg, Reset)  RwIo8IdxData(CMOS_ADDR_PORT, bReg, 0, Reset)

//---------------------------------------------------------------------------

#define WRITE_IO8_RTC_S3(mBoot, bReg, bVal) \
                            WriteIo8IdxDataS3(mBoot, CMOS_ADDR_PORT, bReg, bVal)
#define RW_IO8_RTC_S3(mBoot, bReg, Set, Rst) \
                            RwIo8IdxDataS3(mBoot, CMOS_ADDR_PORT, bReg, Set,Rst)
#define SET_IO8_RTC_S3(mBoot, bReg, Set) \
                            RwIo8IdxDataS3(mBoot, CMOS_ADDR_PORT, bReg, Set, 0)
#define RESET_IO8_RTC_S3(mBoot, bReg, Rst) \
                            RwIo8IdxDataS3(mBoot, CMOS_ADDR_PORT, bReg, 0, Rst)

#ifdef __cplusplus
}
#endif
#endif
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
