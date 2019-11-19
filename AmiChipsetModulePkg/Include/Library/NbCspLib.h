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

//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Chipset/NBCspLib.h $
// 
//*************************************************************************
/** @file NbCspLib.h
    Northbridge chipset library functions definition

**/
//*************************************************************************

#ifndef __NBLIB_H__
#define __NBLIB_H__

#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/PciRootBridgeIo.h>
#include <PciHostBridge.h>
#include <AmiDxeLib.h>
#include <AmiChipsetPkg/Include/Library/AmiChipsetIoLib.h>

#if ACPI_SUPPORT
// #if defined(PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x0001000A)    // [ EIP427935 ]
 #include <Protocol/S3SaveState.h>
// #else
// #include <Protocol/BootScriptSave.h>
// #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CSM_SUPPORT
#define LEGACY_REGION_LOCK          0               // Read only Read to RAM, Write to ROM
#define LEGACY_REGION_BOOT_LOCK     1
#define LEGACY_REGION_UNLOCK        2               // Read/Write to RAM
#define LEGACY_REGION_DECODE_ROM    3               // Read/Write to ROM
#endif

typedef struct {
    UINT8                   Bus;
    UINT8                   Dev;
    UINT8                   Fun;
    UINT8                   Reg;
    EFI_BOOT_SCRIPT_WIDTH   Width;
    UINT32                  Mask;
} BOOT_SCRIPT_NB_PCI_REG_SAVE;

typedef struct {
    UINT8   Register;
    UINT8   Mask;
    UINT32  StartAddress;
    UINT32  Length;
} NB_PAM_STRUCT;

UINT8
NbFrequencyToRatio (
  UINT32    Frequency,
  UINT8     RefClk,
  UINT32    RefBClk
);

UINT32
NbRatioToFrequency (
  UINT8   Ratio,
  UINT8   RefClk,
  UINT32  RefBClk
);

VOID NbRetrainLinkPciDevice (
    IN UINT8                PciBus,
    IN UINT8                PciDev,
    IN UINT8                PciFun,
    IN UINT8                CapPtr
);

EFI_STATUS NBProtectedPciDevice (
    IN VOID         *PciDevice
);

EFI_STATUS NBProgramPciDevice (
    IN VOID         *PciDevice
);

EFI_STATUS NBUpdatePciDeviceAttributes (
    IN VOID         *PciDevice,
    IN OUT UINT64   *Attributes,
    IN UINT64       Capabilities,
    IN BOOLEAN      Set
);

EFI_STATUS NBPAMWriteBootScript(
    IN EFI_S3_SAVE_STATE_PROTOCOL       *BootScriptSave
);

BOOLEAN
CheckPeiFvCopyToRam (
    IN  EFI_PEI_SERVICES  **PeiServices
);

#if (CORE_COMBINED_VERSION >= 0x4027C) // 4.6.3.6
#if AMI_ROOT_BRIDGE_SUPPORT == 1
UINTN HbCspMapRootBrgToHost(
    IN PCI_BUS_XLAT_HDR    *RootBrgXlatHdr, 
    IN UINT64              *AllocationAttr,
    IN UINT64              *RbSuportedAttr
);

EFI_STATUS HbCspAllocateResources(
    IN PCI_HOST_BRG_DATA    *HostBrgData,
    IN PCI_ROOT_BRG_DATA    *RootBrgData,
    IN UINTN                RootBrgIndex
);
#endif
#else
UINTN HbCspMapRootBrgToHost(
    IN PCI_BUS_XLAT_HDR             *RootBrgXlatHdr
);
#endif

//----------------------------------------------------------------------------
// Chipset PCI Macros, Porting Required.
//----------------------------------------------------------------------------
#define READ_PCI8_NB(Rx)          READ_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx)
#define WRITE_PCI8_NB(Rx, Val)    WRITE_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI8_NB(Rx, Set)      SET_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI8_NB(Rx, Rst)    RESET_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx, Rst)
#define RW_PCI8_NB(Rx, St, Rt)    RW_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx, St, Rt)
#define READ_PCI16_NB(Rx)         READ_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx)
#define WRITE_PCI16_NB(Rx, Val)   WRITE_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI16_NB(Rx, Set)     SET_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI16_NB(Rx, Rst)   RESET_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx, Rst)
#define RW_PCI16_NB(Rx, St, Rt)   RW_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx, St, Rt)
#define READ_PCI32_NB(Rx)         READ_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx)
#define WRITE_PCI32_NB(Rx, Val)   WRITE_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI32_NB(Rx, Set)     SET_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI32_NB(Rx, Rst)   RESET_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx, Rst)
#define RW_PCI32_NB(Rx, St, Rt)   RW_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx, St, Rt)

//----------------------------------------------------------------------------

#define READ_PCI8_PCIEBRN(Rx) \
                    READ_PCI8(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx)
#define WRITE_PCI8_PCIEBRN(Rx, Val) \
                    WRITE_PCI8(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Val)
#define SET_PCI8_PCIEBRN(Rx, Set) \
                    SET_PCI8(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Set)
#define RESET_PCI8_PCIEBRN(Rx, Rst) \
                    RESET_PCI8(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Rst)
#define RW_PCI8_PCIEBRN(Rx, St, Rt) \
                    RW_PCI8(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, St, Rt)
#define READ_PCI16_PCIEBRN(Rx) \
                    READ_PCI16(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx)
#define WRITE_PCI16_PCIEBRN(Rx, Vx) \
                    WRITE_PCI16(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Vx)
#define SET_PCI16_PCIEBRN(Rx, Set) \
                    SET_PCI16(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Set)
#define RESET_PCI16_PCIEBRN(Rx, Rt) \
                    RESET_PCI16(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Rt)
#define RW_PCI16_PCIEBRN(Rx, St, Rt) \
                    RW_PCI16(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, St,Rt)
#define READ_PCI32_PCIEBRN(Rx) \
                    READ_PCI32(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx)
#define WRITE_PCI32_PCIEBRN(Rx, Vx) \
                    WRITE_PCI32(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Vx)
#define SET_PCI32_PCIEBRN(Rx, Set) \
                    SET_PCI32(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Set)
#define RESET_PCI32_PCIEBRN(Rx, Rt) \
                    RESET_PCI32(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Rt)
#define RW_PCI32_PCIEBRN(Rx, St, Rt) \
                    RW_PCI32(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, St,Rt)

//----------------------------------------------------------------------------

#define READ_PCI8_IGD(Rx)       READ_PCI8(IGD_BUS, IGD_DEV, IGD_FUN, Rx)
#define WRITE_PCI8_IGD(Rx, Val) WRITE_PCI8(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Val)
#define SET_PCI8_IGD(Rx, Set)   SET_PCI8(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set)
#define RESET_PCI8_IGD(Rx, Rst) RESET_PCI8(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Rst)
#define RW_PCI8_IGD(Rx, St, Rt) RW_PCI8(IGD_BUS, IGD_DEV, IGD_FUN, Rx, St, Rt)
#define READ_PCI16_IGD(Rx)      READ_PCI16(IGD_BUS, IGD_DEV, IGD_FUN, Rx)
#define WRITE_PCI16_IGD(Rx, Vx) WRITE_PCI16(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Vx)
#define SET_PCI16_IGD(Rx, Set)  SET_PCI16(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set)
#define RESET_PCI16_IGD(Rx, Rt) RESET_PCI16(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Rt)
#define RW_PCI16_IGD(Rx, St,Rt) RW_PCI16(IGD_BUS, IGD_DEV, IGD_FUN, Rx, St,Rt)
#define READ_PCI32_IGD(Rx)      READ_PCI32(IGD_BUS, IGD_DEV, IGD_FUN, Rx)
#define WRITE_PCI32_IGD(Rx, Vx) WRITE_PCI32(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Vx)
#define SET_PCI32_IGD(Rx, Set)  SET_PCI32(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set)
#define RESET_PCI32_IGD(Rx, Rt) RESET_PCI32(IGD_BUS, IGD_DEV, IGD_FUN, Rx, Rt)
#define RW_PCI32_IGD(Rx, St,Rt) RW_PCI32(IGD_BUS, IGD_DEV, IGD_FUN, Rx, St,Rt)

//----------------------------------------------------------------------------

#define WRITE_PCI8_NB_S3(mBoot, Rx, Val) \
                     WRITE_PCI8_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI8_NB_S3(mBoot, Rx, Set) \
                     SET_PCI8_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI8_NB_S3(mBoot, Rx, Reset) \
                     RESET_PCI8_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Reset)
#define RW_PCI8_NB_S3(mBoot, Rx, Set, Rst) \
                     RW_PCI8_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Set, Rst)
#define WRITE_PCI16_NB_S3(mBoot, Rx, Val) \
                     WRITE_PCI16_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI16_NB_S3(mBoot, Rx, Set) \
                     SET_PCI16_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI16_NB_S3(mBoot, Rx, Reset) \
                     RESET_PCI16_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Reset)
#define RW_PCI16_NB_S3(mBoot, Rx, Set, Rst) \
                     RW_PCI16_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Set, Rst)
#define WRITE_PCI32_NB_S3(mBoot, Rx, Val) \
                     WRITE_PCI32_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI32_NB_S3(mBoot, Rx, Set) \
                     SET_PCI32_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI32_NB_S3(mBoot, Rx, Reset) \
                     RESET_PCI32_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Reset)
#define RW_PCI32_NB_S3(mBoot, Rx, Set, Rst) \
                     RW_PCI32_S3(mBoot, NB_BUS, NB_DEV, NB_FUN, Rx, Set, Rst)

//----------------------------------------------------------------------------

#define WRITE_PCI8_PCIEBRN_S3(mBoot, Rx, Val) \
         WRITE_PCI8_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Val)
#define SET_PCI8_PCIEBRN_S3(mBoot, Rx, Set) \
         SET_PCI8_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Set)
#define RESET_PCI8_PCIEBRN_S3(mBoot, Rx, Rst) \
         RESET_PCI8_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Rst)
#define RW_PCI8_PCIEBRN_S3(mBoot, Rx, St, Rt) \
         RW_PCI8_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, St, Rt)
#define WRITE_PCI16_PCIEBRN_S3(mBoot, Rx, Val) \
         WRITE_PCI16_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Val)
#define SET_PCI16_PCIEBRN_S3(mBoot, Rx, Set) \
         SET_PCI16_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Set)
#define RESET_PCI16_PCIEBRN_S3(mBoot, Rx, Rst) \
         RESET_PCI16_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Rst)
#define RW_PCI16_PCIEBRN_S3(mBoot, Rx, St, Rt) \
         RW_PCI16_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, St, Rt)
#define WRITE_PCI32_PCIEBRN_S3(mBoot, Rx, Val) \
         WRITE_PCI32_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Val)
#define SET_PCI32_PCIEBRN_S3(mBoot, Rx, Set) \
         SET_PCI32_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Set)
#define RESET_PCI32_PCIEBRN_S3(mBoot, Rx, Reset) \
         RESET_PCI32_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, Rst)
#define RW_PCI32_PCIEBRN_S3(mBoot, Rx, St, Rt) \
         RW_PCI32_S3(mBoot, PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, Rx, St, Rt)

//----------------------------------------------------------------------------

#define WRITE_PCI8_IGD_S3(mBoot, Rx, Val) \
                   WRITE_PCI8_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Val)
#define SET_PCI8_IGD_S3(mBoot, Rx, Set) \
                   SET_PCI8_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set)
#define RESET_PCI8_IGD_S3(mBoot, Rx, Reset) \
                   RESET_PCI8_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Reset)
#define RW_PCI8_IGD_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI8_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set, Rst)
#define WRITE_PCI16_IGD_S3(mBoot, Rx, Val) \
                   WRITE_PCI16_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Val)
#define SET_PCI16_IGD_S3(mBoot, Rx, Set) \
                   SET_PCI16_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set)
#define RESET_PCI16_IGD_S3(mBoot, Rx, Reset) \
                   RESET_PCI16_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Reset)
#define RW_PCI16_IGD_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI16_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set, Rst)
#define WRITE_PCI32_IGD_S3(mBoot, Rx, Val) \
                   WRITE_PCI32_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Val)
#define SET_PCI32_IGD_S3(mBoot, Rx, Set) \
                   SET_PCI32_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set)
#define RESET_PCI32_IGD_S3(mBoot, Rx, Reset) \
                   RESET_PCI32_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Reset)
#define RW_PCI32_IGD_S3(mBoot, Rx, Set, Rst) \
                   RW_PCI32_S3(mBoot, IGD_BUS, IGD_DEV, IGD_FUN, Rx, Set, Rst)

//----------------------------------------------------------------------------
// Chipset MMIO Macros, Porting Required.
//----------------------------------------------------------------------------

#define READ_MEM8_EP(wReg)          READ_MEM8(NB_EP_BASE_ADDRESS | wReg)
#define READ_MMIO8_EP(wReg)         READ_MEM8(NB_EP_BASE_ADDRESS | wReg)
#define WRITE_MEM8_EP(wReg, bVal)   WRITE_MEM8(NB_EP_BASE_ADDRESS | wReg,bVal)
#define WRITE_MMIO8_EP(wReg, bVal)  WRITE_MEM8(NB_EP_BASE_ADDRESS | wReg,bVal)
#define SET_MEM8_EP(wReg, Set)      RW_MEM8(NB_EP_BASE_ADDRESS | wReg, Set, 0)
#define SET_MMIO8_EP(wReg, Set)     RW_MEM8(NB_EP_BASE_ADDRESS | wReg, Set, 0)
#define RESET_MEM8_EP(wReg, Rst)    RW_MEM8(NB_EP_BASE_ADDRESS | wReg, 0, Rst)
#define RESET_MMIO8_EP(wReg, Rst)   RW_MEM8(NB_EP_BASE_ADDRESS | wReg, 0, Rst)
#define RW_MEM8_EP(wReg, Set, Rst)  RW_MEM8(NB_EP_BASE_ADDRESS|wReg, Set, Rst)
#define RW_MMIO8_EP(wReg, Set, Rst) RW_MEM8(NB_EP_BASE_ADDRESS|wReg, Set, Rst)
#define READ_MEM16_EP(wReg)         READ_MEM16(NB_EP_BASE_ADDRESS | wReg)
#define READ_MMIO16_EP(wReg)        READ_MEM16(NB_EP_BASE_ADDRESS | wReg)
#define WRITE_MEM16_EP(wReg, wVal)  WRITE_MEM16(NB_EP_BASE_ADDRESS|wReg, wVal)
#define WRITE_MMIO16_EP(wReg, wVal) WRITE_MEM16(NB_EP_BASE_ADDRESS|wReg, wVal)
#define SET_MEM16_EP(wReg, Set)     RW_MEM16(NB_EP_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO16_EP(wReg, Set)    RW_MEM16(NB_EP_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM16_EP(wReg, Rst)   RW_MEM16(NB_EP_BASE_ADDRESS | wReg, 0,Rst)
#define RESET_MMIO16_EP(wReg, Rst)  RW_MEM16(NB_EP_BASE_ADDRESS | wReg, 0,Rst)
#define RW_MEM16_EP(wReg, Set, Rst) RW_MEM16(NB_EP_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO16_EP(wReg, Set,Rst) RW_MEM16(NB_EP_BASE_ADDRESS|wReg, Set,Rst)
#define READ_MEM32_EP(wReg)         READ_MEM32(NB_EP_BASE_ADDRESS | wReg)
#define READ_MMIO32_EP(wReg)        READ_MEM32(NB_EP_BASE_ADDRESS | wReg)
#define WRITE_MEM32_EP(wReg, dVal)  WRITE_MEM32(NB_EP_BASE_ADDRESS|wReg, dVal)
#define WRITE_MMIO32_EP(wReg, dVal) WRITE_MEM32(NB_EP_BASE_ADDRESS|wReg, dVal)
#define SET_MEM32_EP(wReg, Set)     RW_MEM32(NB_EP_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO32_EP(wReg, Set)    RW_MEM32(NB_EP_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM32_EP(wReg, Rst)   RW_MEM32(NB_EP_BASE_ADDRESS|wReg, 0, Rst)
#define RESET_MMIO32_EP(wReg, Rst)  RW_MEM32(NB_EP_BASE_ADDRESS|wReg, 0, Rst)
#define RW_MEM32_EP(wReg, Set, Rst) RW_MEM32(NB_EP_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO32_EP(wReg, Set,Rst) RW_MEM32(NB_EP_BASE_ADDRESS|wReg, Set,Rst)

//----------------------------------------------------------------------------

#define READ_MEM8_MCH(wReg)         READ_MEM8(NB_MCH_BASE_ADDRESS | wReg)
#define READ_MMIO8_MCH(wReg)        READ_MEM8(NB_MCH_BASE_ADDRESS | wReg)
#define WRITE_MEM8_MCH(wReg, bVal)  WRITE_MEM8(NB_MCH_BASE_ADDRESS| wReg,bVal)
#define WRITE_MMIO8_MCH(wReg, bVal) WRITE_MEM8(NB_MCH_BASE_ADDRESS| wReg,bVal)
#define SET_MEM8_MCH(wReg, Set)     RW_MEM8(NB_MCH_BASE_ADDRESS | wReg, Set,0)
#define SET_MMIO8_MCH(wReg, Set)    RW_MEM8(NB_MCH_BASE_ADDRESS | wReg, Set,0)
#define RESET_MEM8_MCH(wReg, Rst)   RW_MEM8(NB_MCH_BASE_ADDRESS | wReg,0, Rst)
#define RESET_MMIO8_MCH(wReg, Rst)  RW_MEM8(NB_MCH_BASE_ADDRESS | wReg,0, Rst)
#define RW_MEM8_MCH(wReg, Set, Rst) RW_MEM8(NB_MCH_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO8_MCH(wReg, Set,Rst) RW_MEM8(NB_MCH_BASE_ADDRESS|wReg, Set,Rst)
#define READ_MEM16_MCH(wReg)        READ_MEM16(NB_MCH_BASE_ADDRESS | wReg)
#define READ_MMIO16_MCH(wReg)       READ_MEM16(NB_MCH_BASE_ADDRESS | wReg)
#define WRITE_MEM16_MCH(wReg, wVal) WRITE_MEM16(NB_MCH_BASE_ADDRESS|wReg,wVal)
#define WRITE_MMIO16_MCH(wReg,wVal) WRITE_MEM16(NB_MCH_BASE_ADDRESS|wReg,wVal)
#define SET_MEM16_MCH(wReg, Set)    RW_MEM16(NB_MCH_BASE_ADDRESS|wReg,Set, 0)
#define SET_MMIO16_MCH(wReg, Set)   RW_MEM16(NB_MCH_BASE_ADDRESS|wReg,Set, 0)
#define RESET_MEM16_MCH(wReg, Rst)  RW_MEM16(NB_MCH_BASE_ADDRESS | wReg,0,Rst)
#define RESET_MMIO16_MCH(wReg, Rst) RW_MEM16(NB_MCH_BASE_ADDRESS | wReg,0,Rst)
#define RW_MEM16_MCH(wReg, Set,Rst) RW_MEM16(NB_MCH_BASE_ADDRESS|wReg,Set,Rst)
#define RW_MMIO16_MCH(wReg,Set,Rst) RW_MEM16(NB_MCH_BASE_ADDRESS|wReg,Set,Rst)
#define READ_MEM32_MCH(wReg)        READ_MEM32(NB_MCH_BASE_ADDRESS | wReg)
#define READ_MMIO32_MCH(wReg)       READ_MEM32(NB_MCH_BASE_ADDRESS | wReg)
#define WRITE_MEM32_MCH(wReg, dVal) WRITE_MEM32(NB_MCH_BASE_ADDRESS|wReg,dVal)
#define WRITE_MMIO32_MCH(wReg,dVal) WRITE_MEM32(NB_MCH_BASE_ADDRESS|wReg,dVal)
#define SET_MEM32_MCH(wReg, Set)    RW_MEM32(NB_MCH_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO32_MCH(wReg, Set)   RW_MEM32(NB_MCH_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM32_MCH(wReg, Rst)  RW_MEM32(NB_MCH_BASE_ADDRESS|wReg, 0, Rst)
#define RESET_MMIO32_MCH(wReg, Rst) RW_MEM32(NB_MCH_BASE_ADDRESS|wReg, 0, Rst)
#define RW_MEM32_MCH(wReg, Set,Rst) RW_MEM32(NB_MCH_BASE_ADDRESS|wReg,Set,Rst)
#define RW_MMIO32_MCH(wReg,Set,Rst) RW_MEM32(NB_MCH_BASE_ADDRESS|wReg,Set,Rst)

//----------------------------------------------------------------------------

#define READ_MEM8_DMI(wReg)         READ_MEM8(NB_DMI_BASE_ADDRESS | wReg)
#define READ_MMIO8_DMI(wReg)        READ_MEM8(NB_DMI_BASE_ADDRESS | wReg)
#define WRITE_MEM8_DMI(wReg, bVal)  WRITE_MEM8(NB_DMI_BASE_ADDRESS| wReg,bVal)
#define WRITE_MMIO8_DMI(wReg, bVal) WRITE_MEM8(NB_DMI_BASE_ADDRESS| wReg,bVal)
#define SET_MEM8_DMI(wReg, Set)     RW_MEM8(NB_DMI_BASE_ADDRESS | wReg, Set,0)
#define SET_MMIO8_DMI(wReg, Set)    RW_MEM8(NB_DMI_BASE_ADDRESS | wReg, Set,0)
#define RESET_MEM8_DMI(wReg, Rst)   RW_MEM8(NB_DMI_BASE_ADDRESS | wReg,0, Rst)
#define RESET_MMIO8_DMI(wReg, Rst)  RW_MEM8(NB_DMI_BASE_ADDRESS | wReg,0, Rst)
#define RW_MEM8_DMI(wReg, Set, Rst) RW_MEM8(NB_DMI_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO8_DMI(wReg, Set,Rst) RW_MEM8(NB_DMI_BASE_ADDRESS|wReg, Set,Rst)
#define READ_MEM16_DMI(wReg)        READ_MEM16(NB_DMI_BASE_ADDRESS | wReg)
#define READ_MMIO16_DMI(wReg)       READ_MEM16(NB_DMI_BASE_ADDRESS | wReg)
#define WRITE_MEM16_DMI(wReg, wVal) WRITE_MEM16(NB_DMI_BASE_ADDRESS|wReg,wVal)
#define WRITE_MMIO16_DMI(wReg,wVal) WRITE_MEM16(NB_DMI_BASE_ADDRESS|wReg,wVal)
#define SET_MEM16_DMI(wReg, Set)    RW_MEM16(NB_DMI_BASE_ADDRESS|wReg,Set, 0)
#define SET_MMIO16_DMI(wReg, Set)   RW_MEM16(NB_DMI_BASE_ADDRESS|wReg,Set, 0)
#define RESET_MEM16_DMI(wReg, Rst)  RW_MEM16(NB_DMI_BASE_ADDRESS | wReg,0,Rst)
#define RESET_MMIO16_DMI(wReg, Rst) RW_MEM16(NB_DMI_BASE_ADDRESS | wReg,0,Rst)
#define RW_MEM16_DMI(wReg, Set,Rst) RW_MEM16(NB_DMI_BASE_ADDRESS|wReg,Set,Rst)
#define RW_MMIO16_DMI(wReg,Set,Rst) RW_MEM16(NB_DMI_BASE_ADDRESS|wReg,Set,Rst)
#define READ_MEM32_DMI(wReg)        READ_MEM32(NB_DMI_BASE_ADDRESS | wReg)
#define READ_MMIO32_DMI(wReg)       READ_MEM32(NB_DMI_BASE_ADDRESS | wReg)
#define WRITE_MEM32_DMI(wReg, dVal) WRITE_MEM32(NB_DMI_BASE_ADDRESS|wReg,dVal)
#define WRITE_MMIO32_DMI(wReg,dVal) WRITE_MEM32(NB_DMI_BASE_ADDRESS|wReg,dVal)
#define SET_MEM32_DMI(wReg, Set)    RW_MEM32(NB_DMI_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO32_DMI(wReg, Set)   RW_MEM32(NB_DMI_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM32_DMI(wReg, Rst)  RW_MEM32(NB_DMI_BASE_ADDRESS|wReg, 0, Rst)
#define RESET_MMIO32_DMI(wReg,Rst)  RW_MEM32(NB_DMI_BASE_ADDRESS|wReg, 0, Rst)
#define RW_MEM32_DMI(wReg, Set,Rst) RW_MEM32(NB_DMI_BASE_ADDRESS|wReg,Set,Rst)
#define RW_MMIO32_DMI(wReg,Set,Rst) RW_MEM32(NB_DMI_BASE_ADDRESS|wReg,Set,Rst)

//----------------------------------------------------------------------------

#define WRITE_MEM8_EP_S3(mBoot, wReg, bVal) \
                          WRITE_MEM8_S3(mBoot, NB_EP_BASE_ADDRESS | wReg,bVal)
#define WRITE_MMIO8_EP_S3(mBoot, wReg, bVal) \
                          WRITE_MEM8_S3(mBoot, NB_EP_BASE_ADDRESS | wReg,bVal)
#define SET_MEM8_EP_S3(mBoot, wReg, Set) \
                          RW_MEM8_S3(mBoot, NB_EP_BASE_ADDRESS | wReg, Set, 0)
#define SET_MMIO8_EP_S3(mBoot, wReg, Set) \
                          RW_MEM8_S3(mBoot, NB_EP_BASE_ADDRESS | wReg, Set, 0)
#define RESET_MEM8_EP_S3(mBoot, wReg, Rst) \
                          RW_MEM8_S3(mBoot, NB_EP_BASE_ADDRESS | wReg, 0, Rst)
#define RESET_MMIO8_EP_S3(mBoot, wReg, Rst) \
                          RW_MEM8_S3(mBoot, NB_EP_BASE_ADDRESS | wReg, 0, Rst)
#define RW_MEM8_EP_S3(mBoot, wReg, Set, Rst) \
                          RW_MEM8_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set, Rst)
#define RW_MMIO8_EP_S3(mBoot, wReg, Set, Rst) \
                          RW_MEM8_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set, Rst)
#define WRITE_MEM16_EP_S3(mBoot, wReg, wVal) \
                          WRITE_MEM16_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, wVal)
#define WRITE_MMIO16_EP_S3(mBoot, wReg, wVal) \
                          WRITE_MEM16_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, wVal)
#define SET_MEM16_EP_S3(mBoot, wReg, Set) \
                          RW_MEM16_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO16_EP_S3(mBoot, wReg, Set) \
                          RW_MEM16_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM16_EP_S3(mBoot, wReg, Rst) \
                          RW_MEM16_S3(mBoot, NB_EP_BASE_ADDRESS | wReg, 0,Rst)
#define RESET_MMIO16_EP_S3(mBoot, wReg, Rst) \
                          RW_MEM16_S3(mBoot, NB_EP_BASE_ADDRESS | wReg, 0,Rst)
#define RW_MEM16_EP_S3(mBoot, wReg, Set, Rst) \
                          RW_MEM16_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO16_EP_S3(mBoot, wReg, Set, Rst) \
                          RW_MEM16_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set,Rst)
#define WRITE_MEM32_EP_S3(mBoot, wReg, dVal) \
                          WRITE_MEM32_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, dVal)
#define WRITE_MMIO32_EP_S3(mBoot, wReg, dVal) \
                          WRITE_MEM32_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, dVal)
#define SET_MEM32_EP_S3(mBoot, wReg, Set) \
                          RW_MEM32_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO32_EP_S3(mBoot, wReg, Set) \
                          RW_MEM32_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM32_EP_S3(mBoot, wReg, Rst) \
                          RW_MEM32_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, 0, Rst)
#define RESET_MMIO32_EP_S3(mBoot, wReg, Rst) \
                          RW_MEM32_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, 0, Rst)
#define RW_MEM32_EP_S3(mBoot, wReg, Set, Rst) \
                          RW_MEM32_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO32_EP_S3(mBoot, wReg, Set, Rst) \
                          RW_MEM32_S3(mBoot, NB_EP_BASE_ADDRESS|wReg, Set,Rst)

//----------------------------------------------------------------------------

#define WRITE_MEM8_MCH_S3(mBoot, wReg, bVal) \
                         WRITE_MEM8_S3(mBoot, NB_MCH_BASE_ADDRESS | wReg,bVal)
#define WRITE_MMIO8_MCH_S3(mBoot, wReg, bVal) \
                         WRITE_MEM8_S3(mBoot, NB_MCH_BASE_ADDRESS | wReg,bVal)
#define SET_MEM8_MCH_S3(mBoot, wReg, Set) \
                         RW_MEM8_S3(mBoot, NB_MCH_BASE_ADDRESS | wReg, Set, 0)
#define SET_MMIO8_MCH_S3(mBoot, wReg, Set) \
                         RW_MEM8_S3(mBoot, NB_MCH_BASE_ADDRESS | wReg, Set, 0)
#define RESET_MEM8_MCH_S3(mBoot, wReg, Rst) \
                         RW_MEM8_S3(mBoot, NB_MCH_BASE_ADDRESS | wReg, 0, Rst)
#define RESET_MMIO8_MCH_S3(mBoot, wReg, Rst) \
                         RW_MEM8_S3(mBoot, NB_MCH_BASE_ADDRESS | wReg, 0, Rst)
#define RW_MEM8_MCH_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM8_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set, Rst)
#define RW_MMIO8_MCH_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM8_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set, Rst)
#define WRITE_MEM16_MCH_S3(mBoot, wReg, wVal) \
                         WRITE_MEM16_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, wVal)
#define WRITE_MMIO16_MCH_S3(mBoot, wReg, wVal) \
                         WRITE_MEM16_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, wVal)
#define SET_MEM16_MCH_S3(mBoot, wReg, Set) \
                         RW_MEM16_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO16_MCH_S3(mBoot, wReg, Set) \
                         RW_MEM16_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM16_MCH_S3(mBoot, wReg, Rst) \
                         RW_MEM16_S3(mBoot, NB_MCH_BASE_ADDRESS | wReg, 0,Rst)
#define RESET_MMIO16_MCH_S3(mBoot, wReg, Rst) \
                         RW_MEM16_S3(mBoot, NB_MCH_BASE_ADDRESS | wReg, 0,Rst)
#define RW_MEM16_MCH_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM16_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO16_MCH_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM16_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set,Rst)
#define WRITE_MEM32_MCH_S3(mBoot, wReg, dVal) \
                         WRITE_MEM32_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, dVal)
#define WRITE_MMIO32_MCH_S3(mBoot, wReg, dVal) \
                         WRITE_MEM32_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, dVal)
#define SET_MEM32_MCH_S3(mBoot, wReg, Set) \
                         RW_MEM32_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO32_MCH_S3(mBoot, wReg, Set) \
                         RW_MEM32_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM32_MCH_S3(mBoot, wReg, Rst) \
                         RW_MEM32_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, 0, Rst)
#define RESET_MMIO32_MCH_S3(mBoot, wReg, Rst) \
                         RW_MEM32_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, 0, Rst)
#define RW_MEM32_MCH_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM32_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO32_MCH_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM32_S3(mBoot, NB_MCH_BASE_ADDRESS|wReg, Set,Rst)

//----------------------------------------------------------------------------

#define WRITE_MEM8_DMI_S3(mBoot, wReg, bVal) \
                         WRITE_MEM8_S3(mBoot, NB_DMI_BASE_ADDRESS | wReg,bVal)
#define WRITE_MMIO8_DMI_S3(mBoot, wReg, bVal) \
                         WRITE_MEM8_S3(mBoot, NB_DMI_BASE_ADDRESS | wReg,bVal)
#define SET_MEM8_DMI_S3(mBoot, wReg, Set) \
                         RW_MEM8_S3(mBoot, NB_DMI_BASE_ADDRESS | wReg, Set, 0)
#define SET_MMIO8_DMI_S3(mBoot, wReg, Set) \
                         RW_MEM8_S3(mBoot, NB_DMI_BASE_ADDRESS | wReg, Set, 0)
#define RESET_MEM8_DMI_S3(mBoot, wReg, Rst) \
                         RW_MEM8_S3(mBoot, NB_DMI_BASE_ADDRESS | wReg, 0, Rst)
#define RESET_MMIO8_DMI_S3(mBoot, wReg, Rst) \
                         RW_MEM8_S3(mBoot, NB_DMI_BASE_ADDRESS | wReg, 0, Rst)
#define RW_MEM8_DMI_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM8_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set, Rst)
#define RW_MMIO8_DMI_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM8_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set, Rst)
#define WRITE_MEM16_DMI_S3(mBoot, wReg, wVal) \
                         WRITE_MEM16_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, wVal)
#define WRITE_MMIO16_DMI_S3(mBoot, wReg, wVal) \
                         WRITE_MEM16_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, wVal)
#define SET_MEM16_DMI_S3(mBoot, wReg, Set) \
                         RW_MEM16_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO16_DMI_S3(mBoot, wReg, Set) \
                         RW_MEM16_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM16_DMI_S3(mBoot, wReg, Rst) \
                         RW_MEM16_S3(mBoot, NB_DMI_BASE_ADDRESS | wReg, 0,Rst)
#define RESET_MMIO16_DMI_S3(mBoot, wReg, Rst) \
                         RW_MEM16_S3(mBoot, NB_DMI_BASE_ADDRESS | wReg, 0,Rst)
#define RW_MEM16_DMI_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM16_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO16_DMI_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM16_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set,Rst)
#define WRITE_MEM32_DMI_S3(mBoot, wReg, dVal) \
                         WRITE_MEM32_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, dVal)
#define WRITE_MMIO32_DMI_S3(mBoot, wReg, dVal) \
                         WRITE_MEM32_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, dVal)
#define SET_MEM32_DMI_S3(mBoot, wReg, Set) \
                         RW_MEM32_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set, 0)
#define SET_MMIO32_DMI_S3(mBoot, wReg, Set) \
                         RW_MEM32_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set, 0)
#define RESET_MEM32_DMI_S3(mBoot, wReg, Rst) \
                         RW_MEM32_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, 0, Rst)
#define RESET_MMIO32_DMI_S3(mBoot, wReg, Rst) \
                         RW_MEM32_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, 0, Rst)
#define RW_MEM32_DMI_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM32_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set,Rst)
#define RW_MMIO32_DMI_S3(mBoot, wReg, Set, Rst) \
                         RW_MEM32_S3(mBoot, NB_DMI_BASE_ADDRESS|wReg, Set,Rst)

//----------------------------------------------------------------------------
// Chipset I/O Macros, Porting Required if needed.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//To Exclude AMI Native Root Bridge Driver HOOKS from CSP LIB
#if AMI_ROOT_BRIDGE_SUPPORT == 1

//----------------------------------------------------------------------------
//Function Prototypes for PciRootBridgeIo
//----------------------------------------------------------------------------

extern DXE_SERVICES     *gDxeSvcTbl;

EFI_STATUS HbResAllocNotifyPhase (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE        Phase
);

EFI_STATUS HbResAllocGetNextRootBridge (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN OUT EFI_HANDLE                                       *RootBridgeHandle
);

EFI_STATUS HbResAllocGetAllocAttributes (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    OUT UINT64                                              *Attributes
);

EFI_STATUS HbResAllocStartBusEnumeration (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    OUT VOID                                                **Configuration
);

EFI_STATUS HbResAllocSetBusNumbers (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    IN VOID                                                 *Configuration
);

EFI_STATUS HbResAllocSubmitResources (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    IN VOID                                                 *Configuration
);

EFI_STATUS HbResAllocGetProposedResources (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    OUT VOID                                                **Configuration
);

EFI_STATUS HbResAllocPreprocessController (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    IN EFI_PCI_CONFIGURATION_ADDRESS                        PciAddress,
    IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE         Phase
);

EFI_STATUS HbNotifyCspBeforeEnumeration (
    EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    UINTN                                               RbCount
);

EFI_STATUS HbNotifyCspBeginBusAllocation (
    EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    UINTN                                               RbCount
);

EFI_STATUS HbNotifyCspEndBusAllocation (
    EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    UINTN                                               RbCount
);

EFI_STATUS HbNotifyCspBeginResourceAllocation (
    EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    UINTN                                               RbCount
);

EFI_STATUS HbNotifyCspAllocateResources (
    EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    UINTN                                               RbCount
);

EFI_STATUS HbNotifyCspSetResources (
    EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    UINTN                                               RbCount
);

EFI_STATUS HbNotifyCspEndResourceAllocation (
    EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    UINTN                                               RbCount
);

EFI_STATUS HbCspStartBusEnumeration (
    PCI_HOST_BRG_DATA       *HostBrgData,
    PCI_ROOT_BRG_DATA       *RootBrgData,
    UINTN RootBrgIndex
);

EFI_STATUS HbCspSetBusNnumbers (
    PCI_HOST_BRG_DATA       *HostBrgData,
    PCI_ROOT_BRG_DATA       *RootBrgData,
    UINTN RootBrgIndex
);

EFI_STATUS HbCspSubmitResources (
    PCI_HOST_BRG_DATA       *HostBrgData,
    PCI_ROOT_BRG_DATA       *RootBrgData,
    UINTN RootBrgIndex
);

EFI_STATUS HbCspAdjustMemoryMmioOverlap (
    PCI_HOST_BRG_DATA       *HostBrgData,
    PCI_ROOT_BRG_DATA       *RootBrgData,
    UINTN RootBrgIndex
);

EFI_STATUS HbCspBasicChipsetInit (
    PCI_HOST_BRG_DATA       *HostBrg0
);

UINTN HbCspGetPciSegment (
    UINTN                   HostBridgeNumber,
    UINTN                   RootBridgeNumber
);

EFI_STATUS HbCspPreprocessController (
    PCI_HOST_BRG_DATA                               *HostBrgData,
    PCI_ROOT_BRG_DATA                               *RootBrgData, 
    UINTN                                           RootBrgNumber,
    EFI_PCI_CONFIGURATION_ADDRESS                   PciAddress,
    EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE    Phase
);

EFI_STATUS RbCspIoPciMap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION    Operation,
    IN EFI_PHYSICAL_ADDRESS                         HostAddress,
    IN OUT UINTN                                    *NumberOfBytes,
    OUT EFI_PHYSICAL_ADDRESS                        *DeviceAddress,
    OUT VOID                                        **Mapping
);

EFI_STATUS RbCspIoPciUnmap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN PCI_ROOT_BRIDGE_MAPPING                      *Mapping
);

EFI_STATUS RbCspIoPciAttributes( 
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN UINT64                                       Attributes,
    IN OUT UINT64                                   *ResourceBase OPTIONAL,
    IN OUT UINT64                                   *ResourceLength OPTIONAL
);

EFI_STATUS RootBridgeIoPciRW (
    IN PCI_ROOT_BRG_DATA                        *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
    IN UINT64                                   Address,
    IN UINTN                                    Count,
    IN OUT VOID                                 *Buffer,
    IN BOOLEAN                                  Write
);

EFI_STATUS RbCspIoPciUnmap (
    IN  PCI_ROOT_BRG_DATA                           *RbData, 
    OUT PCI_ROOT_BRIDGE_MAPPING                     *Mapping
);

EFI_STATUS RbCspIoPciMap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION    Operation,
    IN EFI_PHYSICAL_ADDRESS                         HostAddress,
    IN OUT UINTN                                    *NumberOfBytes,
    OUT EFI_PHYSICAL_ADDRESS                        *DeviceAddress,
    OUT VOID                                        **Mapping
);

//--------------------------------------
//#if AMI_ROOT_BRIDGE_SUPPORT == 1
#endif 

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
