/** @file
  ACPI SSDT table

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

DefinitionBlock (
    "UsbPortXhciRvp11.aml",
    "SSDT",
    0x02, // SHAD revision.
        // A Revision field value greater than or equal to 2 signifies that integers
        // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL", // OEM ID (6 byte string)
    "xh_rvp11", // OEM table ID  (8 byte string)
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
  External(\_SB.PCI0.XHC.RHUB.HS11, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS12, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS13, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.HS14, DeviceObj)

  External(\_SB.PCI0.XHC.RHUB.SS01, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS02, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS03, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS04, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS05, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS06, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS07, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS08, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS09, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.SS10, DeviceObj)

  External(\_SB.PCI0.XHC.RHUB.USR1, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.USR2, DeviceObj)

  External(SPTH)
  External(PCHV, MethodObj)

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

    //
    // Method for creating generic _UPC buffers
    // Similar to _PLD, for internal testing we only care about 1 parameter
    //
    Method(GUPC, 1, Serialized) {
      Name(PCKG, Package(4) { 0, 0xFF, 0, 0 } )
      Store(Arg0,Index(PCKG,0))


      return (PCKG)
    }



  } //end scope RHUB

  Scope (\_SB.PCI0.XHC.RHUB.HS01) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,1)) } //walkup usb2 port
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS02) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(0,2)) } //flex
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS03) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(0,3)) } //m.2 BT
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS04) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(0,4)) } //touchpanel
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS05) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(0,5)) } //wwan
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS06) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,6)) } //walkup port
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS07) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,7)) } //walkup port
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS08) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,8)) } //walkup port
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS09) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,9)) } //walkup port
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS10) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,10)) } //walkup port
  }

  If(LEqual(PCHV(),SPTH)) {
    Scope (\_SB.PCI0.XHC.RHUB.HS11) {
      Method(_UPC) { Return (GUPC(1)) }
      Method(_PLD) { Return (GPLD(1,11)) } //walkup port
    }

    Scope (\_SB.PCI0.XHC.RHUB.HS12) {
      Method(_UPC) { Return (GUPC(1)) }
      Method(_PLD) { Return (GPLD(0,12)) } //FPS
    }

    Scope (\_SB.PCI0.XHC.RHUB.HS13) {
      Method(_UPC) { Return (GUPC(1)) }
      Method(_PLD) { Return (GPLD(1,13)) } //walkup port
    }

    Scope (\_SB.PCI0.XHC.RHUB.HS14) {
      Method(_UPC) { Return (GUPC(1)) }
      Method(_PLD) { Return (GPLD(1,14)) }
    }
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
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,1)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS02) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(0,15)) } //m.2 wwan
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS03) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(0,16)) } //flex connector
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS04) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(0,17)) } //ivcam
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS05) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,11)) } //walkup port
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS06) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,6)) } //FP port
  }

  If(LEqual(PCHV(),SPTH)) {
    Scope (\_SB.PCI0.XHC.RHUB.SS07) {
      Method(_UPC) { Return (GUPC(1)) }
      Method(_PLD) { Return (GPLD(1,7)) } //walkup port
    }

    Scope (\_SB.PCI0.XHC.RHUB.SS08) {
      Method(_UPC) { Return (GUPC(1)) }
      Method(_PLD) { Return (GPLD(1,8)) } //walkup port
    }

    Scope (\_SB.PCI0.XHC.RHUB.SS09) {
      Method(_UPC) { Return (GUPC(1)) }
      Method(_PLD) { Return (GPLD(1,9)) } //walkup port
    }

    Scope (\_SB.PCI0.XHC.RHUB.SS10) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,10)) } //walkup port
    }
  }
}
