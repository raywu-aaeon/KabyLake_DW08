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

/** @file AmiSbMiscLib.h
  This file contains South Bridge(LPC) chipset porting library Miscellaneous 
  functions and data structure definitions for both PEI & DXE stages.
  
*/

#ifndef __AMI_SB_MISC_LIB_H__
#define __AMI_SB_MISC_LIB_H__

#include <PiPei.h>
#include <Uefi.h>
#include "Token.h"
#include <Ppi/ReadOnlyVariable2.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SbResetFull = 3,  // This is the same with power cycle reset of Intel Pch chipsets
	SbResetGlobal,
	SbResetGlobalWithEc,
	SbResetTypeMax,
} SB_EXT_RESET_TYPE;

typedef union {
    UINT16      PowerLossData;
    struct {
      UINT16    PwrOkFail     :1;
      UINT16    CpuPwrFail    :1;
      UINT16    SlpS4Fail     :1;
      UINT16    ThermTripSts  :1;
      UINT16    ResetSts      :1;
      UINT16    PwrFail       :1;
      UINT16    RtcLostPower  :1;
      UINT16    Reserved      :9;
    };
} SB_POWER_LOSS_DATA;

typedef struct {
    UINT8       ChipVendor;
    UINT8       Stepping;
    UINT8       PciePortNum;
    UINT8       SataPortNum;
    UINT8       UsbPhysicalPortNum;
    UINT8       Usb2PortNum;
    UINT8       Usb3PortNum;
} SB_INFO_DATA;

typedef enum {
    SB_DEV_FUNCTION_0 = 0,
    SB_DEV_XHCI,
    SB_DEV_THERMAL,
    SB_DEV_SATA1,
    SB_DEV_PCIEX_PORT1,
    SB_DEV_PCIEX_PORT2,
    SB_DEV_PCIEX_PORT3,
    SB_DEV_PCIEX_PORT4,
    SB_DEV_PCIEX_PORT5,
    SB_DEV_PCIEX_PORT6,
    SB_DEV_PCIEX_PORT7,
    SB_DEV_PCIEX_PORT8,
    SB_DEV_PCIEX_PORT9,
    SB_DEV_PCIEX_PORT10,
    SB_DEV_PCIEX_PORT11,
    SB_DEV_PCIEX_PORT12,
    SB_DEV_PCIEX_PORT13,
    SB_DEV_PCIEX_PORT14,
    SB_DEV_PCIEX_PORT15,
    SB_DEV_PCIEX_PORT16,
    SB_DEV_PCIEX_PORT17,
    SB_DEV_PCIEX_PORT18,
    SB_DEV_PCIEX_PORT19,
    SB_DEV_PCIEX_PORT20,
    SB_DEV_PCIEX_PORT21,
    SB_DEV_PCIEX_PORT22,
    SB_DEV_PCIEX_PORT23,
    SB_DEV_PCIEX_PORT24,
    SB_DEV_SMBUS,
    SB_DEV_AZALIA,
    SB_DEV_LAN,
    SB_DEV_MAX
} SB_DEV_TYPE;

typedef enum {
	SbComA,
	SbComB,
	SbLpt,
	SbFdd,
	SbGameL,
	SbGameH,
	SbKbc,
	SbMc,
	SbCnf1,	// 0x2E & 0x2F
	SbCnf2	// 0x4E & 0x4F
} SB_LPC_SIO_TYPE;

typedef enum {
	CPU_THERMAL_TRIP_STATUS,
	AFTERG3_EN,
	PWR_FLR
} SB_MISC_TYPE;

typedef enum {
    SbH          = 1,
    SbLp,
    SbUnknownSeries
} AMI_SB_SERIES;

VOID SbLibShutdown (
    VOID
);

VOID SbLibExtResetSystem (
    IN SB_EXT_RESET_TYPE  ResetType
);

#if SB_RESET_PPI_SUPPORT
VOID SBLib_ResetSystem (
    IN  EFI_RESET_TYPE  ResetType
);
#endif

BOOLEAN SbIsDefaultConfigMode (
    IN EFI_PEI_SERVICES                 **PeiServices,
    IN EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariablePpi );

EFI_STATUS CountTime (
    IN  UINTN   DelayTime,
    IN  UINT16  BaseAddr // only needs to be 16 bit for I/O address
);

BOOLEAN SbLib_GetSmiState(
    VOID
);

VOID SbLib_SmiDisable(
    VOID
);

VOID SbLib_SmiEnable(
    VOID
);

VOID CspLibCheckPowerLoss(
    VOID
);

VOID SbEnableDisableFunctions (
    IN SB_DEV_TYPE  SbDevType,
    IN BOOLEAN      Enable
);

VOID SbSaveRestoreAllHwSmi (
    IN BOOLEAN      Save
);

VOID SbDisableAllHwSmi (
    VOID
);

VOID SbSwSmiTrigger (
    IN UINT8        SwSmi
);

VOID SbSwSmiIo (
    IN     UINT8    SwSmi,
    IN OUT UINT8    *Data
);

UINT16 SbGetSwSmi (
    IN OUT UINT32   *DataValue
);

UINT16 SbGetTco2StsAddress (
    IN UINT16       AcpiBaseAddr
);

UINT16 SbGetTco2CntAddress (
    IN UINT16       AcpiBaseAddr
);

VOID SetAfterG3Bit (
    IN BOOLEAN      Set
);

BOOLEAN SbCheckAfterG3 (
    VOID
);

VOID SbDisableLpcDevices (
    IN SB_LPC_SIO_TYPE  SioType
);

VOID SbEnableEhciSmi (
    VOID
);

VOID SbDisableEhciSmi (
    VOID
);

BOOLEAN IsPowerButtonPressed (
    VOID
);

VOID SbEnablePciPme (
    IN UINTN        PciAddress
);

VOID SbDisablePciPme (
    IN UINTN        PciAddress
);

VOID SbEnableWolPmConfg (
    VOID
);

VOID SbDisableWolPmConfg (
    VOID
);

VOID SbEnableWolPmConfg3 (
    VOID
);

VOID SbDisableWolPmConfg3 (
    VOID
);


UINT16 SbGetIoTrapInfo (
    IN OUT UINT32   *TrappedIoData 
);

UINT16 SbGetAcpiBaseAddress (
    VOID
);

UINT16 SbGetPm1CntOffset (
    VOID
);

UINT32 SbGetRcrbAddress (
    VOID
);

EFI_STATUS SbGetMiscBitStatus(
    IN SB_MISC_TYPE         SbMiscType,
    IN OUT UINT8            *BitStatus
);

EFI_STATUS SbProgramMiscBit(
    IN SB_MISC_TYPE         SbMiscType,
    IN BOOLEAN              Set
);

BOOLEAN SbIsXhciRouting ( 
    VOID 
);

EFI_STATUS SbGetChipLanMacAddress ( 
    IN OUT UINT8            *MacAddress
);

UINTN SbGetPciBusDevFunc ( 
    IN SB_DEV_TYPE  SbDevType
);

#if defined(AMI_COMPATIBILITY_PKG_VERSION)&&(AMI_COMPATIBILITY_PKG_VERSION > 18)
#if KBC_SUPPORT
BOOLEAN IsCtrlHomePressed (
    EFI_PEI_SERVICES **PeiServices
);
#endif        // KBC_SUPPORT
#endif        // AMI_COMPATIBILITY_PKG_VERSION

VOID GetSbInfoData ( 
    IN OUT SB_INFO_DATA    *SbInfoData
);

AMI_SB_SERIES
EFIAPI
AmiGetSbSeries (
    VOID
);

UINT8
GetSbUsbMaxPhysicalPortNum( 
    VOID 
);

UINT8
GetSbXhciMaxUsb2PortNum( 
    VOID 
);

UINT8
GetSbXhciMaxUsb3PortNum( 
    VOID 
);

UINT8
GetSbMaxSataPortNum( 
    VOID 
);

UINT32
SbFindCapPtr(
    IN UINT64  PciAddress,
    IN UINT8   CapId
);

VOID
SBLib_Shutdown(
    VOID
);

VOID
ClearMeWakeSts(
    VOID
);

BOOLEAN 
SbIsWarmBoot (
    VOID
);

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
