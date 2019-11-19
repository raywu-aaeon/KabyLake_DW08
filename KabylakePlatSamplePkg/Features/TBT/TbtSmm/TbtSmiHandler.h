/**@file
  TBT SMM phase - Assign PCIe resource to Thunderbolt devices

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
#ifndef _TBT_SMI_HANDLER_H_
#define _TBT_SMI_HANDLER_H_

#include <Library/TbtCommonLib.h>
#include <Library/IoLib.h>
#include <SetupVariable.h>
#include <IndustryStandard/Pci.h>

#ifdef PROGRESS_CODE
#undef PROGRESS_CODE
#endif
#define PROGRESS_CODE(Data)   IoWrite8 (0x80, Data)

#define TBT_HR_MSG_CTRL_REG   0x8A
// AMI_OVERRIDE_START - Change the TBT call info by Token, TBT_SWSMI_VALUE.
#if 0
#define SMM_THUNDERBOLT_CALL  0xAA
#else
#define SMM_THUNDERBOLT_CALL  TBT_SWSMI_VALUE
#endif
// AMI_OVERRIDE_END - Change the TBT call info by Token, TBT_SWSMI_VALUE.
#define SMM_CLEAN_MSI_CALL    0xDA
#define MAX_TBT_DEPTH         6

#define P2P_BRIDGE            (((PCI_CLASS_BRIDGE) << 8) | (PCI_CLASS_BRIDGE_P2P))

#define BAR_ALIGN(v, a)       ((((v) - 1) | (a)) + 1)

#define CMD_BUS_MASTER        BIT2
#define CMD_BM_IO             (CMD_BUS_MASTER | BIT0)
#define CMD_BM_MEM            (CMD_BUS_MASTER | BIT1)
#define CMD_BM_MEM_IO         (CMD_BUS_MASTER | BIT1 | BIT0)

#define DEF_CACHE_LINE_SIZE   0x20
#define DEF_RES_IO_PER_DEV    4
#define DEF_RES_MEM_PER_DEV   32
#define DEF_RES_PMEM_PER_DEV  32

#define DOCK_BUSSES           8

#define DISBL_IO_REG1C        0x01F1
#define DISBL_MEM32_REG20     0x0000FFF0
#define DISBL_PMEM_REG24      0x0001FFF1

#define BUF_LEN               (32 * 1024)

#define count(x)              (sizeof (x) / sizeof ((x)[0]))

#define PCIE_CAP_ID_VSEC       0x000B
#define PCI_CAP_ID_MSI         0x05
#define PCIE_CAP_ID_SSID_SSVID 0x0D

#define MAX_BUS_NUM_TO_SCAN   (UINT8) ((PcdGet32 (PcdPciExpressRegionLength) >> 20) - 1)
#define PCI_VENDOR_DEV_ID     0x00
#define PCI_DEVICE            0x0
#define PCI_BRIDGE            0x1
#define INVALID_PCI_DEVICE    0xFFFFFFFF
#define PCI_HEADER_TYPE       0x0E
#define PCI_PRI_BUS_NUM       0x18
#define PCI_SEC_BUS_NUM       0x19
#define PCI_SUB_BUS_NUM       0x1A
#define PCI_CMD_REG           0x04

#define PCI_TBT_VESC_REG5     0x51C
#define PCI_TBT_VESC_REG42    0x5B0

typedef union _PCIE_EXT_CAP_HDR {
  UINT32  EXT_CAP_HDR;
  struct {
    UINT16  ExtCapId;
    UINT16  CapVersion : 4;
    UINT16  NextItemPtr : 12;
  } W;
} PCIE_EXT_CAP_HDR;

//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
typedef struct _TBT_DGFX_INFO {
  UINT8    DgpuBus;
  UINT8    DgpuDev;
  UINT8    DgpuFun;
  UINT8    HrDownstreamDevice;
  BOOLEAN  IsLocationDgpu;
} TBT_DGFX_INFO;
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"

typedef struct _PortInfo {
  UINT8   IoBase;
  UINT8   IoLimit;
  UINT16  MemBase;
  UINT16  MemLimit;
  UINT64  PMemBase64;
  UINT64  PMemLimit64;
  UINT8   BusNumLimit;
  UINT8   ConfedEP;
} PortInfo;

typedef struct _MEM_REGS {
  UINT32  Base;
  UINT32  Limit;
} MEM_REGS;

typedef struct _PMEM_REGS {
  UINT64  Base64;
  UINT64  Limit64;
} PMEM_REGS;

typedef struct _IO_REGS {
  UINT16  Base;
  UINT16  Limit;
} IO_REGS;

typedef struct _BRDG_RES_CONFIG {
  UINT8   Cmd;
  UINT8   Cls;
  UINT8   IoBase;
  UINT8   IoLimit;
  UINT16  MemBase;
  UINT16  MemLimit;
  UINT64  PMemBase64;
  UINT64  PMemLimit64;
} BRDG_RES_CONFIG;

typedef struct _BRDG_CONFIG {
  DEV_ID          DevId;
  UINT8           PBus;
  UINT8           SBus;
  UINT8           SubBus;
  BOOLEAN         IsDSBridge;
  BRDG_RES_CONFIG Res;
} BRDG_CONFIG;

enum {
  HR_US_PORT,
  HR_DS_PORT0,
  HR_DS_PORT3,
  HR_DS_PORT4,
  HR_DS_PORT5,
  HR_DS_PORT6,
  MAX_CFG_PORTS
};

enum {
  HR_DS_PORT1   = HR_DS_PORT3
};

//
// Alpine Ridge
//
enum {
  AR_DS_PORT1 = HR_DS_PORT3,
  AR_DS_PORT2,
  AR_DS_PORT3,
  AR_DS_PORT4
};

//
//US(X:0:0), DS(X+1:3:0),DS(X+1:4:0),DS(X+1:5:0),DS(X+1:6:0)
//
STATIC BRDG_CONFIG HrConfigs[MAX_CFG_PORTS];

typedef struct _HR_CONFIG {
  UINT16  DeviceId;
  UINT8   HRBus;
  UINT8   MinDSNumber;
  UINT8   MaxDSNumber;
  UINT8   BridgeLoops;
} HR_CONFIG;

STATIC const BRDG_RES_CONFIG  NOT_IN_USE_BRIDGE = {
  CMD_BUS_MASTER,
  0,
  DISBL_IO_REG1C & 0xFF,
  DISBL_IO_REG1C >> 8,
  DISBL_MEM32_REG20 & 0xFFFF,
  DISBL_MEM32_REG20 >> 16,
  DISBL_PMEM_REG24 & 0xFFFF,
  DISBL_PMEM_REG24 >> 16
};

typedef union _BRDG_CIO_MAP_REG {
  UINT32  AB_REG;
  struct {
    UINT32  NumOfDSPorts : 5;
    UINT32  CioPortMap : 27;
  } Bits;
} BRDG_CIO_MAP_REG;

//
// Functions
//
VOID
ThunderboltCallback (
  IN  UINT32 RpIndex
  );

VOID
DisablePCIDevicesAndBridges (
  IN  UINT32 RpIndex
  );

VOID
EndOfThunderboltCallback (
  IN  UINT32 RpIndex
  );

VOID
DisableMSI (
  VOID
  );

UINT8
PcieFindCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT8   CapId
  );

//AMI_OVERRIDE_START - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler
VOID
Stall (
  UINTN     Usec
  );
//AMI_OVERRIDE_END - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler

#endif
