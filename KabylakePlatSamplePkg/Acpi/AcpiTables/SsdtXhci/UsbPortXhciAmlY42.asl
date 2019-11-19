/** @file
  ACPI SSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2019 Intel Corporation.

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

DefinitionBlock (
    "UsbPortXhciRvp3.aml",
    "SSDT",
    0x02, // SHAD revision.
        // A Revision field value greater than or equal to 2 signifies that integers
        // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL", // OEM ID (6 byte string)
    "xh_mly42", // OEM table ID  (8 byte string)
    0x0 // OEM version of DSDT table (4 byte Integer)
    )
{
  External(\_SB.PCI0.XHC.RHUB, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS01, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS02, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS03, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS04, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS05, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS06, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS07, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS08, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS09, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS10, DeviceObj)

  External(\_SB.PCI0.XHC.RHUB.SS01, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS02, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS03, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS04, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS05, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS06, DeviceObj)

  External(\_SB.PCI0.XHC.RHUB.USR1, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.USR2, DeviceObj)

  External(\_SB.PCI0.LPCB.CRID, IntObj)


  External(\_SB.PCI0.XDCI, DeviceObj)
  External(\USME, IntObj)
  External(\TBTS, IntObj)

  External(BTSE)
  External(BTBR)
  External(BED2)
  External(BED3)
  External(BTLE)
  External(BTL2)
  External(BTLL)

  Scope (\_SB.PCI0.XHC.RHUB) {
    //
    // Method for creating generic _PLD buffers
    // _PLD contains lots of data, but for purpose of internal validation we care only about
    // ports' visibility and pairing (this requires group position)
    // so these are the only 2 configurable parameters
    //
    Method(GPLD, 2, Serialized) {
      Name(PCKG, Package() { Buffer(0x10) {} } )
      CreateField(DerefOf(Index(PCKG,0)), 0, 7, REV)
      Store(1,REV)
      CreateField(DerefOf(Index(PCKG,0)), 64, 1, VISI)
      Store(Arg0, VISI)
      CreateField(DerefOf(Index(PCKG,0)), 87, 8, GPOS)
      Store(Arg1, GPOS)


      return (PCKG)
    }

    Method(TPLD, 2, Serialized)
    {
      Name(PCKG, Package(1) { Buffer(0x10){} } )
      CreateField(DerefOf(Index(PCKG, Zero)),Zero,0x07,REV)
      Store(One, REV)
      CreateField(DerefOf(Index(PCKG, Zero)),0x40,One,VISI)
      Store(Arg0, VISI)
      CreateField(DerefOf(Index(PCKG, Zero)),0x57,0x08,GPOS)
      Store(Arg1, GPOS)
      CreateField(DerefOf(Index(PCKG, Zero)),0x4A,0x04,SHAP)
      Store(One, SHAP)
      CreateField(DerefOf(Index(PCKG, Zero)),0x20,0x10,WID)
      Store(0x08, WID)
      CreateField(DerefOf(Index(PCKG, Zero)),0x30,0x10,HGT)
      Store(0x03, HGT)
      Return(PCKG)
    }

    //
    // Method for creating generic _UPC buffers
    // Similar to _PLD, for internal testing we only care about 1 parameter
    //
    Method(GUPC, 1, Serialized) {
      Name(PCKG, Package(4) { 0, 0xFF, 0, 0 } )
      Store(Arg0,Index(PCKG,0))


      return (PCKG)
    }

    Method(TUPC, 1, Serialized) { // For the port related with USB Type C. copied and modified from GUPC
    // Arg0: Type
    // Type:
    //  0x08:     Type-C connector - USB2-only
    //  0x09:     Type-C connector - USB2 and SS with Switch
    //  0x0A:     Type-C connector - USB2 and SS without Switch
      Name(PCKG, Package(4) { 1, 0x00, 0, 0 } )
      Store(Arg0,Index(PCKG,1))
      return (PCKG)
    }
  }
//end scope RHUB

  Scope (\_SB.PCI0.XHC.RHUB.HS01) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,1)) } //walkup port 1
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS02) {
    Method(_UPC) {
      If(LEqual(USME, 0)) {
          Return (GUPC(1)) //walkup Port
      } Else {
          Return (TUPC(9)) // TBT Type-C Port
      }
    }
    Method(_PLD) {
      If(LEqual(USME, 0)) {
          Return (GPLD(1,2)) //walkup port 2
      } Else {
          Return (TPLD(1,2)) //TBT USB-C Port
      }
    }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS03) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(0,3)) } //m.2 connector
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS04) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS05) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,5)) } //webcam // WP3 after rework
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS06) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //not connected
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS07) {
    Method(_UPC) {
      If(LEqual(USME, 0)) {
          Return (GUPC(1)) //walkup Port 4
      } Else {
          Return (TUPC(9)) // TBT Type-C Port
      }
    }
    Method(_PLD) {
      If(LEqual(USME, 0)) {
          Return (GPLD(1,7)) //walkup port 4
      } Else {
          Return (TPLD(1,7)) //TBT USB-C Port
      }
    }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS08) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //not connected
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS09) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,9)) } //walkup port 5
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS10) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //not connected
  }

  Scope (\_SB.PCI0.XHC.RHUB.USR1) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.USR2) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS01) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,1)) } // Walk-up port 1
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS02) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) {
      If(LEqual(USME, 0)) { // if TBTsupport disabled
        Return (GPLD(1,2)) // WP2
      } Else {
        Return (GPLD(0,2))
      }
    }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS03) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,5)) } // WP3 // IVCAM Camera optional or after rework
    include("DsEndpoint.asl")
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS04) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) {
      If(LEqual(USME, 0)) { // if TBTsupport disabled
        Return (GPLD(1,7)) // WP4
      } Else {
        Return (GPLD(0,7))
      }
   }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS05) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //not connected
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS06) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) } //not connected
  }

  Scope(\_SB.PCI0.XDCI) {
    Method (_PLD)  { Return ( \_SB.PCI0.XHC.RHUB.TPLD(1,1) ) }
    Method (_UPC)  { Return ( \_SB.PCI0.XHC.RHUB.TUPC(9) ) }
  } // Type C port directly under xDCI device and making it SS device only
}
