/** @file
Private data structures

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 Intel Corporation.

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

#ifndef _SYSTEM_SLOTINFO_DXE_H
#define _SYSTEM_SLOTINFO_DXE_H

#include <PiDxe.h>
#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/SmBios.h>
#include <Library/PchPcieRpLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <Protocol/Smbios.h>
#include <PlatformInfo.h>
#include <Register/PchRegsPcie.h>

#define MAX_SLOT_ENTRIES 20

typedef struct {
  CHAR16                      *SlotDesignation;
  UINT8                       SlotType;                 ///< The enumeration value from MISC_SLOT_TYPE.
  UINT8                       SlotDataBusWidth;         ///< The enumeration value from MISC_SLOT_DATA_BUS_WIDTH.
  UINT8                       CurrentUsage;             ///< The enumeration value from MISC_SLOT_USAGE.
  UINT8                       SlotLength;               ///< The enumeration value from MISC_SLOT_LENGTH.
  UINT16                      SlotID;
  MISC_SLOT_CHARACTERISTICS1  SlotCharacteristics1;
  MISC_SLOT_CHARACTERISTICS2  SlotCharacteristics2;
  UINT16                      SegmentGroupNum; // Valid Only for PCIe Root Ports ,Others 0xFF is expected
  UINT8                       BusNum;
  UINT8                       DevNum;
  UINT8                       FuncNum;
  BOOLEAN                     Isvalid;
} SYSTEM_SLOT_INFO;

typedef struct {
  BOOLEAN          Detect;
  UINT8            Numberofentries;
  SYSTEM_SLOT_INFO Slotinfo[MAX_SLOT_ENTRIES];
}SYSTEM_SLOT_ENTRY;


#define PCIE_DEFAULT 0xFF

#define PCIE_BUS 0

#define PCIE_DEV_1 PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1
#define PCIE_DEV_2 PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2
#define PCIE_DEV_3 PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3

#define PCIE_FUN_1 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1
#define PCIE_FUN_2 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2
#define PCIE_FUN_3 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3
#define PCIE_FUN_4 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4
#define PCIE_FUN_5 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5
#define PCIE_FUN_6 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6
#define PCIE_FUN_7 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7
#define PCIE_FUN_8 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8

#endif
