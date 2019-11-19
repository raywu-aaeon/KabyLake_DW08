/** @file
  Policy definition for PCIe Config Block

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCIE_PEI_CONFIG_H_
#define _PCIE_PEI_CONFIG_H_

#include <Library/GpioLib.h>
#include <SaAccess.h>

#pragma pack(push, 1)

#define SA_PCIE_PEI_CONFIG_REVISION 1

/**
 PCI Express and DMI controller configuration - PostMem\n
 @note <b>Optional.</b> These policies will be ignored if there is no PEG port present on board.
  <b>Revision 1</b>:
  - Initial version.
**/
typedef struct {
  CONFIG_BLOCK_HEADER    Header;                  ///< Offset 0-27 Config Block Header
  /**
    Offset 28:0
   <b>(Test)</b>DMI Extended Sync Control
  - <b>Disabled</b> (0x0)  : Disable DMI Extended Sync (Default)
  - Enabled         (0x1)  : Enable DMI Extended Sync
  **/
  UINT32                 DmiExtSync                      :  1;
  /**
    Offset 28:1
   <b>(Test)</b>DMI IOT Control
  - <b>Disabled</b> (0x0)  : Disable DMI IOT (Default)
  - Enabled         (0x1)  : Enable DMI IOT
  **/
  UINT32                 DmiIot                          :  1;
  UINT32                 RsvdBits1                       :  30;       ///< Offset 28:2 :Reserved for future use.
  UINT8                  DmiAspm;                                     ///< Offset 32 This field is used to describe the ASPM control for DMI: <b>2=PcieAspmL1</b>, 0=PcieAspmDisabled.
  UINT8                  PegDeEmphasis[SA_PEG_MAX_FUN];               ///< Offset 33 This field is used to describe the DeEmphasis control for PEG (-6 dB and -3.5 dB are the options)
  UINT8                  PegMaxPayload[SA_PEG_MAX_FUN];               ///< <b>(Test)</b> Offset 36 This field is used to describe the PEG Max Pay Load Size (0xFF: Auto, 0:128B, 1:256B)
  /**
   PCIe Slot Power Capabilities. SlotPowerLimitValue in combination with SlotPowerLimitScale specifies the upper limit on power supplied by slot.
  **/
  UINT8                  PegSlotPowerLimitValue[SA_PEG_MAX_FUN];      ///< Offset 39 8 bit value
  UINT8                  PegSlotPowerLimitScale[SA_PEG_MAX_FUN];      ///< Offset 42 2 bit value: <b>00 = 1.0x</b>, 01 = 0.1x, 10 = 0.01x and 11 = 0.001x
  UINT8                  Rsvd1[1];                                    ///< Offset 45
  /**
   Offset 46
   PCIe Physical Slot Number (13 bit value). Indicates the physical slot number attached to the port.
  **/
  UINT16                 PegPhysicalSlotNumber[SA_PEG_MAX_FUN];
  //
  //  Next offset is 52
  //
} PCIE_PEI_CONFIG;
#pragma pack(pop)

#endif // _PCIE_PEI_CONFIG_H_
