/** @file
  Define TBT information HOB data structure

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2017 Intel Corporation.

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
#ifndef _TBT_INFO_GUID_H_
#define _TBT_INFO_GUID_H_

#include <Library/GpioLib.h>
#include <Acpi/AcpiTables/Fadt/Fadt.h>

#define MAX_BUS_NUM_TO_SCAN (UINT8) ((PcdGet32 (PcdPciExpressRegionLength) >> 20) - 1)
#define PCI_VENDOR_DEV_ID   0x00
#define PCI_DEVICE          0x0
#define PCI_BRIDGE          0x1
#define INVALID_PCI_DEVICE  0xFFFFFFFF
#define PCI_HEADER_TYPE     0x0E
#define PCI_PRI_BUS_NUM     0x18
#define PCI_SEC_BUS_NUM     0x19
#define PCI_SUB_BUS_NUM     0x1A
#define PCI_CMD_REG         0x04

extern EFI_GUID  gTbtInfoHobGuid;

#pragma pack(1)
//
// TBT GPIO Data Structure
//
typedef struct {
  UINT8          GpioAccessType;     ///< Where the GPIO comes from [a.k.a how to access the GPIO],Where the GPIO comes from. 0: Disabled; 1: PCH, 2: I/O Expander
  UINT8          Expander;           ///< Applicable to GpioAccessType = IoExpander {TCA6424A} type
  UINT32         GpioNumber;         ///< GPIO Number/Pin Number/Pad Number
  BOOLEAN        GpioMode;           ///< 0 = Edge Mode; 1 = Level Mode
  BOOLEAN        GpioLevel;          ///< 0 = Active Low; 1 = Active High
  UINT32         AcpiGpeSignature;
  UINT16         AcpiGpeStsAddress;
  UINT8          AcpiGpeStsBit;
  UINT32         AcpiGpioGpeNumber;  ///< GPIO GPE Number ('xx' in _Exx, _Lxx)
  BOOLEAN        AcpiGpeSignaturePorting; ///< 0 = No porting required(for 2-tier GPI GPE event architecture), 1 = Porting required(for 1-tier GPI GPE event architecture)
} TBT_GPIO_INFO;

//
// TBT Info HOB
//
/**
 Making any TBT_INFO_HOB structure change after code frozen
 will need to maintain backward compatibility, bump up
 structure revision and update below history table\n
  <b>Revision 1</b>:   - Initial version.\n
  <b>Revision 2</b>:   - Add TbtUsbSplitMode/SplitModePchUsb2TbtPort1/SplitModePchUsb2TbtPort2 for USBC - Split USB sub system.\n
**/
#define TBT_INFO_HOB_REVISION       2

typedef struct _TBT_INFO_HOB {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  UINT8             RevisionId;                 ///< Revision ID
  UINT32            TbtSupportedPortsInfo;      ///< Bit 0 = 1 means PCH-RootPort 0 is supported, Bit 1 = 1 means PCH-RootPort 1 is supported .... Bit 7 = 1 means PCH-RootPort 7 is supported
                                                ///< Bit 8 = 1 means PCH-RootPort 8 is supported, Bit 9 = 1 means PCH-RootPort 9 is supported .... Bit 15 = 1 means PCH-RootPort 15 is supported
                                                ///< Bit 16 = 1 means PCH-RootPort 16 is supported, Bit 17 = 1 means PCH-RootPort 17 is supported .... Bit 19 = 1 means PCH-RootPort 19 is supported
                                                ///< Bit 20 = 1 means Peg 0 is supported, Bit 21 = 1 means Peg 1 is supported .... Bit 22 = 1 means Peg 2 is supported
  UINT8             TbtDefaultSelectorInfo;     ///< Default Selector no
  UINT8             HRPowerOffSupport;          ///< Host Router will power off completely during Sx, 1=Supported; 0=Not Supported
  TBT_GPIO_INFO     ForcePwrGpio;
  TBT_GPIO_INFO     CioPlugEventGpio;
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  //
  // For Controller 2
  //
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
// AMI_OVERRIDE_START - For AMI and OEM Feature.
  UINT8             TbtSupport;
  UINT8             TbtSelector;
  UINT8             Bus;
  UINT8             Dev;
  UINT8             Fun;
  UINT8             SecurityMode;
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  UINT8             SprSupport;
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
  UINT8             AicArSupport;
  UINT8             TbtBootOn;
  UINT8             TbtUsbOn;
  UINT8             TbtWakeupSupport;
  UINT8             TbtCacheLineSize;
  UINT16            TbtForcePwrGroup;
  UINT16            TbtForcePwrPad;
  UINT16            TbtHotPlugEvtGroup;
  UINT16            TbtHotPlugEvtPad;
  UINT8             TbtSkipPciOprom;
  UINT8             TbtSkipPciEnumeration;
  UINT8             TbtAspm;
  UINT8             TbtLtr;
  UINT8             TBTHotSMI;
  UINT8             TBTHotNotify;
  UINT8             TbtAcpiRemovalSupport;
  UINT16            ReserveMemoryPerSlot;
  UINT16            ReservePMemoryPerSlot;
  UINT8             AssignIo;
  UINT8             ReserveIoPerSlot;
// AMI_OVERRIDE_END - For AMI and OEM Feature.
} TBT_INFO_HOB;
#pragma pack()

#endif
