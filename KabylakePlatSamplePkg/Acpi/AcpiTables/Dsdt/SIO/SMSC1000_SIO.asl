/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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

Device(SM10)
{
  Name(_HID,EISAID("PNP0A05"))

  Name(_UID,4)

  Method(_STA,0,Serialized)
  {
    // Only report resources to the OS if the SIO Device is
    // present on the Docking Board.

    If(SMC1)
    {
      Return(0x000F)
    }

    Return(0x0000)
  }

  // Define the SMSC1000 Reduced SIO Index/Data Registers as an ACPI
  // Operating Region.  These registers will be used communicate
  // to the SIO.

  OperationRegion(SM10,SystemIO,SIO2,0x02)
  Field(SM10, ByteAcc, Lock, Preserve)
  {
    INDX, 8,
    DATA, 8
  }

  // Use ACPI Defined IndexField so consecutive Index/Data I/Os are
  // assured to be uninterrupted.

  IndexField(INDX,DATA,ByteAcc,Lock,Preserve)
  {
        CR00,8,
        CR01,8,
    Offset(0x02),
    CR02, 8,  // UART Power Management
          Offset(0x0C),
    CR0C, 8,  // UARTMODE
    Offset(0x23),
    CR23,8,
    Offset(0x24),
    CR24, 8,  // UART 1 Base Address
    CR25, 8,  // UART 2 Base Address
    Offset(0x27),
    CR27, 8,
    Offset(0x28),
    CR28, 8,  // UART IRQ Select
    Offset(0x2B),
        CR2B, 8,  //SCE (FIR) Base Address
        CR2C, 8,  //SCE (FIR) DMA Select
    Offset(0x3A),
    CR3A, 8,  // LPC Docking Activate
    CR3B, 8,  // LPC Docking Base (High)
    CR3C, 8,  // LPC Docking Base (Low)
    Offset(0x55),
    CR55, 8,  // Configuration Enable Key
    Offset(0xAA),
    CRAA, 8 // Configuration Disable Key
  }
  Include("SMSC1000_COM.asl")
  Include("SMSC1000_LPT.asl")
}