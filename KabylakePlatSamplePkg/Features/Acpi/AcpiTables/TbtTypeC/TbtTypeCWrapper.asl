/** @file
  TBT TypeC ports information

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
      //
      // TBT DS port USB
      //
      Device(TBDU)
      {
        Name(_ADR, 0x00020000)

        Device(XHC)
        {
          Name(_ADR, 0x00)

          Device(RHUB)
          {
            Name(_ADR, 0x00)
            //
            // Method for creating generic _PLD buffers
            // _PLD contains lots of data, but for purpose of internal validation we care only about
            // ports visibility and pairing (this requires group position)
            // so these are the only 2 configurable parameters
            //

            Method(TPLD, 2, Serialized) { // For the port related with USB Type C.
            // Arg0:  Visible
            // Arg1:  Group position
              Name(PCKG, Package() { Buffer(0x10) {} } )
              CreateField(DerefOf(Index(PCKG,0)), 0, 7, REV)
              Store(1,REV)
              CreateField(DerefOf(Index(PCKG,0)), 64, 1, VISI)
              Store(Arg0, VISI)
              CreateField(DerefOf(Index(PCKG,0)), 87, 8, GPOS)
              Store(Arg1, GPOS)

            // For USB type C, Standerd values
              CreateField(DerefOf(Index(PCKG,0)), 74, 4, SHAP)  // Shape set to Oval
              Store(1, SHAP)
              CreateField(DerefOf(Index(PCKG,0)), 32, 16, WID)  // Width of the connector, 8.34mm
              Store(8, WID)
              CreateField(DerefOf(Index(PCKG,0)), 48, 16, HGT)  // Height of the connector, 2.56mm
              Store(3, HGT)
              return (PCKG)
            }

            //
            // Method for creating generic _UPC buffers
            // Similar to _PLD, for internal testing we only care about 1 parameter
            //

            Method(TUPC, 2, Serialized) { // For the port related with USB Type C.
            // Arg0: Connectable
            // Arg1: Type
            // Type:
            //  0x08:     Type C connector - USB2-only
            //  0x09:     Type C connector - USB2 and SS with Switch
            //  0x0A:     Type C connector - USB2 and SS without Switch
              Name(PCKG, Package(4) { 1, 0x00, 0, 0 } )
              Store(Arg0,Index(PCKG,0))
              Store(Arg1,Index(PCKG,1))
              return (PCKG)
            }
            Device(HS01)
            {
              Name(_ADR, 0x01)
              // Type C connector - USB2-only
              Method(_UPC) {
                If(LEqual(USME, 0)) {
                  Return (TUPC(1,8))
                } Else {
                  Return (TUPC(0,0)) // In Split mode, USB2 is routed through PCH XHCI. So make it Not Connectable
                }
              }
              Method(_PLD) {
                If(LEqual(USME, 0)) {
                  Return (TPLD(1,1))
                } Else {
                  Return (TPLD(0,0)) // In Split mode, USB2 is routed through PCH XHCI. So make it Not Visible
                }
              }
            } // End of Device(HS01)

            Device(HS02)
            {
              Name(_ADR, 0x02)
              // Type C connector - USB2-only
              Method(_UPC) {
                If(LEqual(USME, 0)) {
                  Return (TUPC(1,8))
                } Else {
                  Return (TUPC(0,0)) // In Split mode, USB2 is routed through PCH XHCI. So make it Not Connectable
                }
              }
              Method(_PLD) {
                If(LEqual(USME, 0)) {
                  Return (TPLD(1,2))
                } Else {
                  Return (TPLD(0,0)) // In Split mode, USB2 is routed through PCH XHCI. So make it Not Visible
                }
              }
            } // End of Device(HS02)

            Device(SS01)
            {
              Name(_ADR, 0x03)
              // Type C connector - USB2 and SS with Switch
              Method(_UPC) { Return (TUPC(1,9)) }
              Method(_PLD) {
                If(LEqual(USME, 0)) {
                  Return (TPLD(1,1))
                } Else {
                  Return (TPLD(1,UPT1))  // Use Companion Port number on PCH XHCI
                }
              }
            } // End of Device(SS01)

            Device(SS02)
            {
              Name(_ADR, 0x04)
              // Type C connector - USB2 and SS with Switch
              Method(_UPC) { Return (TUPC(1,9)) }
              Method(_PLD) {
                If(LEqual(USME, 0)) {
                  Return (TPLD(1,2))
                } Else {
                  Return (TPLD(1,UPT2)) // Use Companion Port number on PCH XHCI
                }
              }
            } // End of Device(SS02)
          } // End of Device(RHUB)
        } // End of Device(XHC)
      } // End of Device(TBDU)
