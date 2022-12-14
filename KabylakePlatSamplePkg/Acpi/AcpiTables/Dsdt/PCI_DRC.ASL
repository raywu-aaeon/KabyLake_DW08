/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2015 Intel Corporation.

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


Scope (\_SB.PCI0){

  Device(PDRC)
  {
    //
    // PCI Device Resource Consumption
    //

    Name(_HID,EISAID("PNP0C02"))        

    Name(_UID,1)

    Name(BUF0,ResourceTemplate()
    {
      //
      // MCH BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.48h
      //
      Memory32Fixed(ReadWrite,0,0x08000,MCHB) 
      //
      // DMI BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.68h
      //
      Memory32Fixed(ReadWrite,0,0x01000,DMIB) 
      //
      // EP BAR _BAS will be updated in _CRS below according to B0:D0:F0:Reg.40h
      //
      Memory32Fixed(ReadWrite,0,0x01000,EGPB)
      //
      // PCI Express BAR _BAS and _LEN will be updated in _CRS below according to B0:D0:F0:Reg.60h
      //
      Memory32Fixed(ReadWrite,0,0,PCIX)  

      //
      // MISC ICH TTT base address reserved for the TxT module use. Check if the hard code meets the real configuration.
      // If not, dynamically update it like the _CRS method below.
      //
      Memory32Fixed(ReadWrite,0xFED20000,0x20000)   
      
      //
      // VTD engine memory range. Check if the hard code meets the real configuration.
      // If not, dynamically update it like the _CRS method below.
      //
      Memory32Fixed(ReadOnly, 0xFED90000, 0x00004000)
      
      //
      // MISC ICH. Check if the hard code meets the real configuration.
      // If not, dynamically update it like the _CRS method below.
      //
      Memory32Fixed(ReadWrite,0xFED45000,0x4B000,TPMM)
      
      //
      // FLASH range
      //
      Memory32Fixed (ReadOnly, 0xFF000000, 0x1000000, FIOH) //16MB as per IOH spec
      
      //
      // Local APIC range(0xFEE0_0000 to 0xFEEF_FFFF)
      //
      Memory32Fixed (ReadOnly, 0xFEE00000, 0x100000, LIOH)
      
      //
      // Sx handler reserved MMIO
      //
      Memory32Fixed (ReadWrite, 0, 0, SXRE)
      
      //
      // Reserve HPET address decode range
      //
      Memory32Fixed (ReadWrite, 0, 0, HPET)

    })
 

    Method(_CRS,0,Serialized)
    {

      CreateDwordField(BUF0,^MCHB._BAS,MBR0)
      Store(\_SB.PCI0.GMHB(), MBR0)

      CreateDwordField(BUF0,^DMIB._BAS,DBR0)
      Store(\_SB.PCI0.GDMB(), DBR0)

      CreateDwordField(BUF0,^EGPB._BAS,EBR0)
      Store(\_SB.PCI0.GEPB(), EBR0)

      CreateDwordField(BUF0,^PCIX._BAS,XBR0)
      Store(\_SB.PCI0.GPCB(), XBR0)

      CreateDwordField(BUF0,^PCIX._LEN,XSZ0)
      Store(\_SB.PCI0.GPCL(), XSZ0)

      CreateDwordField(BUF0,^SXRE._BAS,SXRA)
      Store(SXRB, SXRA)
      CreateDwordField(BUF0,^SXRE._LEN,SXRL)
      Store(SXRS, SXRL)

      // HPET device claims the resource in LPC_DEV.ASL.
      If(LNOT(HPTE)){
        CreateDwordField(BUF0,^HPET._BAS,HBAS)
        CreateDwordField(BUF0,^HPET._LEN,HLEN)
        Store(HPTB, HBAS)
        Store(0x400, HLEN)
      }

      Return(BUF0)
    }
  } //end of PDRC
} // end of SB
