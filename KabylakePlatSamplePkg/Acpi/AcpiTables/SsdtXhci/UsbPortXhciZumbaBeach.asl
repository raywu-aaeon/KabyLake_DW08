/** @file
  ACPI SSDT table

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

DefinitionBlock (
    "UsbPortXhciZumbaBeach.aml",
    "SSDT",
    0x02, // SHAD revision.
        // A Revision field value greater than or equal to 2 signifies that integers
        // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL", // OEM ID (6 byte string)
    "xh_Zumba", // OEM table ID  (8 byte string)
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
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,1)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS02) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,2)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS03) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,3)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS04) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,4)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS05) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,5)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS06) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,6)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS07) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,7)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS08) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,8)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS09) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,9)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS10) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,10)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS11) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,11)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS12) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,12)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS13) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,13)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.HS14) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,14)) }
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
    Method(_PLD) { Return (GPLD(1,2)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS02) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,4)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS03) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,9)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS04) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,11)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS05) {
    Method(_UPC) { Return (GUPC(1)) }
    Method(_PLD) { Return (GPLD(1,3)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS06) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,14)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS07) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,1)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS08) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,10)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS09) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,13)) }
  }

  Scope (\_SB.PCI0.XHC.RHUB.SS10) {
    Method(_UPC) { Return (GUPC(0)) }
    Method(_PLD) { Return (GPLD(0,0)) }
  }
}
