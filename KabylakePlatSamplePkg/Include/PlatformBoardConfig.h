/** @file
  Header file for Platform Boards Configurations.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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

#ifndef _PLATFORM_BOARD_CONFIG_H
#define _PLATFORM_BOARD_CONFIG_H

#include <ConfigBlock.h>
#include <PchPolicyCommon.h>
#include <ConfigBlock/MemoryConfig.h>
#include <Library/GpioLib.h>
#ifndef MINTREE_FLAG
#include <TbtBoardInfo.h>
#endif
#define IS_ALIGNED(addr, size) (((addr) & (size - 1)) ? 0 : 1)
#define ALIGN16(size)          (IS_ALIGNED(size, 16) ? size : ((size + 16) & 0xFFF0))

#define BOARD_CONFIG_BLOCK_PEI_PREMEM_VERSION  0x00000001
#define BOARD_CONFIG_BLOCK_PEI_POSTMEM_VERSION 0x00000001
#define BOARD_CONFIG_BLOCK_DXE_VERSION 0x00000001
#define BOARD_NO_BATTERY_SUPPORT 0
#define BOARD_REAL_BATTERY_SUPPORTED BIT0
#define BOARD_VIRTUAL_BATTERY_SUPPORTED BIT1

#pragma pack(1)

typedef struct {
  CONFIG_BLOCK_HEADER  Header;               ///< Offset 0-27 Config Block Header
} BOARD_CONFIG_BLOCK;

typedef struct {
  UINT8 GpioSupport;
  UINT32 WakeGpioNo;
  UINT8 HoldRstExpanderNo;
  UINT32 HoldRstGpioNo;
  BOOLEAN HoldRstActive;
  UINT8 PwrEnableExpanderNo;
  UINT32 PwrEnableGpioNo;
  BOOLEAN PwrEnableActive;
} SWITCH_GRAPHIC_GPIO;

typedef struct {
  UINT8 ClkReqNumber : 4;
  UINT8 ClkReqSupported : 1;
  UINT8 DeviceResetPadActiveHigh : 1;
  UINT32 DeviceResetPad;
} ROOT_PORT_CLK_INFO;

typedef struct {
  UINT8 Section;
  UINT8 Pin;
} EXPANDER_GPIO_CONFIG;

typedef enum {
  BoardGpioTypePch,
  BoardGpioTypeExpander,
  BoardGpioTypeNotSupported = 0xFF
} BOARD_GPIO_TYPE;

typedef struct {
  UINT8 Type;
  UINT8 Reserved[3];  // alignment for COMMON_GPIO_CONFIG
  union {
    UINT32 Pin;
    EXPANDER_GPIO_CONFIG Expander;
  } u;
} BOARD_GPIO_CONFIG;

typedef union {
  UINT64 Blob;
  BOARD_GPIO_CONFIG  BoardGpioConfig;
  ROOT_PORT_CLK_INFO Info;
} PCD64_BLOB;

typedef union {
  UINT32        Blob;
  PCH_USB20_AFE Info;
} PCD32_BLOB;

#ifndef IO_EXPANDER_DISABLED
#define IO_EXPANDER_DISABLED      0xFF
#endif

typedef struct {
  CONFIG_BLOCK_HEADER  Header;               ///< Offset 0-27 Config Block Header

  struct {

    // SA

    UINT32 RcompResistor : 1;
    UINT32 RcompTarget : 1;
    UINT32 SpdData : 1;
    UINT32 DqByteMap : 1;
    UINT32 DqsMapCpu2Dram : 1;
    UINT32 DqPinsInterleaved : 1;
    UINT32 GpioSupport : 1;
    UINT32 OverclockingSupport : 1;

    // PCH

    // COMMON
    UINT32 GpioGroupTierInit : 1;
    UINT32 Pc8374SioKbc : 1;
    UINT32 OddPowerInit : 1;
    UINT32 VboostSpecificInit : 1;
    UINT32 IvCamSpecificInit : 1;
    UINT32 RecoveryModeGpio:1;
    UINT32 Reserved : 17;
  } Enable;

  // SA

  UINT8 UserBd;
  UINT16 MmioSizeAdjustment;
  BOOLEAN MobilePlatform;
  UINT8 SpdAddressTable[4];
  UINT8 CaVrefConfig;

  UINT16 RcompResistor[SA_MRC_MAX_RCOMP];
  UINT16 RcompTarget[SA_MRC_MAX_RCOMP_TARGETS];
  UINT8 SpdData[512];
  UINT8 DqByteMap[2][6][2];
  UINT8 DqsMapCpu2Dram[2][8];
  UINT8 DqPinsInterleaved;

  BOOLEAN GpioSupport;
  UINT32 GpioPad;
  BOOLEAN Active;

  UINT8 RootPortDev;
  UINT8 RootPortFun;
#ifndef MINTREE_FLAG
  SWITCH_GRAPHIC_GPIO SaRtd3Pcie0Gpio;
  SWITCH_GRAPHIC_GPIO SaRtd3Pcie1Gpio;
  SWITCH_GRAPHIC_GPIO SaRtd3Pcie2Gpio;
  UINT8               SaRtd3Pcie0ClkRecNumber;
  UINT8               SaRtd3Pcie1ClkRecNumber;
  UINT8               SaRtd3Pcie2ClkRecNumber;
#endif

  // PCH

  ROOT_PORT_CLK_INFO RootPort[32];
  ROOT_PORT_CLK_INFO Lan;

  PCH_USB20_AFE UsbPort20Afe[PCH_MAX_USB2_PORTS];
  UINT8 UsbPort20OverCurrentPin[PCH_MAX_USB2_PORTS];
  UINT8 UsbPort30OverCurrentPin[PCH_MAX_USB3_PORTS];

  // Common
#ifndef MINTREE_FLAG
  BOOLEAN IoExpander;

  GPIO_GROUP GroupToGpeDw0;
  GPIO_GROUP GroupToGpeDw1;
  GPIO_GROUP GroupToGpeDw2;
#endif
  VOID *UnknowHHsioPtssTable [2];
  UINT32 UnknowHHsioPtssTableSize [2];
  VOID *UnknowLpHsioPtssTable [2];
  UINT32 UnknowLpHsioPtssTableSize [2];

  VOID *SpecificHHsioPtssTable [2];
  UINT32 SpecificHHsioPtssTableSize [2];
  VOID *SpecificLpHsioPtssTable [2];
  UINT32 SpecificLpHsioPtssTableSize [2];

  BOARD_GPIO_CONFIG RecoveryModeGpio;
  UINT32 RecoveryModeBitMask;

  GPIO_INIT_CONFIG *GpioTablePreMem;
  UINT16 GpioTablePreMemSize;

} BOARD_CONFIG_BLOCK_PEI_PREMEM;

typedef struct {
  CONFIG_BLOCK_HEADER  Header;               ///< Offset 0-27 Config Block Header
#ifndef MINTREE_FLAG
  struct {

    // GPIO

    UINT32 DebugUsbUart : 1;
    UINT32 SkyCamGpio : 1;
    UINT32 PhabletPocGpioSpecificInit : 1;
    UINT32 PantherMtnGpioSpecificInit : 1;
    UINT32 M2GpioSpecificInit : 1;
    UINT32 GrizzlyMtnGpioSpecificInit : 1;
    UINT32 CoyoteMtnGpioSpecificInit : 1;

    UINT32 Reserved : 25;
  } Enable;
#endif
  // GPIO

  GPIO_INIT_CONFIG *GpioTable1;
  UINT16 Table1Size;

  GPIO_INIT_CONFIG *GpioTable2;
  UINT16 Table2Size;

  VOID *GpioExpanderTable1;
  UINT16 GpioExpanderTable1Size;

  VOID *GpioExpanderTable2;
  UINT16 GpioExpanderTable2Size;

  GPIO_INIT_CONFIG *TouchpanelPad;

#ifndef MINTREE_FLAG
  GPIO_INIT_CONFIG *SdhcSidebandCardDetect;

  // TBT

  UINT32            TbtSupportedPortsInfo;      ///< Bit 0 = 1 means PCH-RootPort 0 is supported, Bit 1 = 1 means PCH-RootPort 1 is supported .... Bit 7 = 1 means PCH-RootPort 7 is supported
                                                ///< Bit 8 = 1 means PCH-RootPort 8 is supported, Bit 9 = 1 means PCH-RootPort 9 is supported .... Bit 15 = 1 means PCH-RootPort 15 is supported
                                                ///< Bit 16 = 1 means PCH-RootPort 16 is supported, Bit 17 = 1 means PCH-RootPort 17 is supported .... Bit 19 = 1 means PCH-RootPort 19 is supported
                                                ///< Bit 20 = 1 means Peg 0 is supported, Bit 21 = 1 means Peg 1 is supported .... Bit 22 = 1 means Peg 2 is supported
  UINT8             TbtDefaultSelectorInfo;     ///< Default Selector no
  TBT_GPIO_INFO     ForcePwrGpio;
  TBT_GPIO_INFO     CioPlugEventGpio;
#endif
  VOID *DisplayAudioVerbTable;
  VOID *CommonVerbTable[3];
  VOID *VerbTable1[3];
  VOID *VerbTable2[3];
#ifndef MINTREE_FLAG
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  UINT32            TbtSupportedPortsInfo1;     ///< Bit 0 = 1 means PCH-RootPort 0 is supported, Bit 1 = 1 means PCH-RootPort 1 is supported .... Bit 7 = 1 means PCH-RootPort 7 is supported
                                                ///< Bit 8 = 1 means PCH-RootPort 8 is supported, Bit 9 = 1 means PCH-RootPort 9 is supported .... Bit 15 = 1 means PCH-RootPort 15 is supported
                                                ///< Bit 16 = 1 means PCH-RootPort 16 is supported, Bit 17 = 1 means PCH-RootPort 17 is supported .... Bit 19 = 1 means PCH-RootPort 19 is supported
                                                ///< Bit 20 = 1 means Peg 0 is supported, Bit 21 = 1 means Peg 1 is supported .... Bit 22 = 1 means Peg 2 is supported
  UINT8             TbtDefaultSelectorInfo1;    ///< Default Selector no
  TBT_GPIO_INFO     ForcePwrGpio1;
  TBT_GPIO_INFO     CioPlugEventGpio1;
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
  BOOLEAN           TbtUsbSplitMode;
  UINT8             SplitModePchUsb2TbtPort1;
  UINT8             SplitModePchUsb2TbtPort2;

  // WWAN
  GPIO_INIT_CONFIG *GpioTablePcieWwan;
  VOID             *GpioExpanderTablePcieWwan;
  UINT16            GpioExpanderTablePcieWwanSize;

  // PCIe Slot1
  GPIO_INIT_CONFIG *GpioTablePcieSlot1;
  VOID             *GpioExpanderTablePcieSlot1;
  UINT16            GpioExpanderTablePcieSlot1Size;

  UINT16            GpioTablePcieWwanSize;
  UINT16            GpioTablePcieSlot1Size;

  GPIO_INIT_CONFIG *ModemReset;
#endif

} BOARD_CONFIG_BLOCK_PEI_POSTMEM;

typedef struct {
  CONFIG_BLOCK_HEADER  Header;               ///< Offset 0-27 Config Block Header

  struct {

    UINT32 Reserved : 32;
  } Enable;

} BOARD_CONFIG_BLOCK_DXE;

#pragma pack()

extern EFI_GUID gPlatformBoardConfigPreMemPpiGuid;
extern EFI_GUID gPlatformBoardConfigPreMemGuid;

extern EFI_GUID gPlatformBoardConfigPostMemPpiGuid;
extern EFI_GUID gPlatformBoardConfigPostMemGuid;

extern EFI_GUID gPlatformBoardConfigProtocolGuid; // AdvancedFeaturesContent
extern EFI_GUID gPlatformBoardConfigGuid; // AdvancedFeaturesContent

#endif // _PLATFORM_BOARD_CONFIG_H
