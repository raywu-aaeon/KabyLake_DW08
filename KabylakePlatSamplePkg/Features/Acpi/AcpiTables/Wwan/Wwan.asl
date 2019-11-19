/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 - 2019 Intel Corporation.

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
  "Wwan.aml",
  "SSDT",
  2,
  "Intel",
  "Wwan",
  0x01
  )
{
  // GPIO methods
  External(\_SB.SGOV, MethodObj)
  External(\_SB.CGWR, MethodObj)

  External(\_SB.PCI0.RP12, DeviceObj)
  External(\_SB.PCI0.RP12.PXSX, DeviceObj)
  External(\_SB.PCI0.RP12._ADR, MethodObj)
  External(\_SB.PCI0.RP12.PXSX._ADR, IntObj)
  External(\_SB.PCI0.GPCB, MethodObj)
  External(\NEXP, IntObj)
  External(\WGUR, IntObj) // Wwan reset pin. M.2 spec
  External(\WDCT, IntObj) // Register save data
  External(\WLCT, IntObj) // Register save data
  External(\WDC2, IntObj) // Register save data
  External(\WMXS, IntObj) // Register save data
  External(\WMNS, IntObj) // Register save data
  include("GpioAcpiDefines.h")

  Name(RSTP, Package() { // Reset pin definition for WWAN module. PCIe PERST
        2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
        0, // IOEX #
       22, // GPIO pad #/IOEX pin #
        0  // reset pin de-assert polarity
      })


  Scope (\_SB.PCI0.RP12) { // Current KBL-R WWAN location.
    //
    // PCIe root port Pci Cfg space to MMIO address
    // Converts PCI device address from _ADR method to physical memory address
    // Arg0 - _ADR object of the device that will be accessed
    //
    Method (M2PC,1,Serialized) {  // Memory to PCI_CFG. Convert _ADR to PCI MMIO address.
      Store (\_SB.PCI0.GPCB(), Local0) // MMIO Base address
      Add (Local0, ShiftRight(And(Arg0,0x001F0000), 1), Local0) // Device no
      Add (Local0, ShiftLeft(And(Arg0, 0x00000007), 12), Local0) // Function no
      Return (Local0)
    }

    //
    // PCIe End point Pci Cfg space to MMIO address
    //
    Method (GMIO, 1, Serialized) { // Get Memory Mapped I/O address
      // Arg0 - _ADR of the target Endpoint.
      // Bus number will be grabbed from the Root port. (under the scope where this method exists)
      // Root port PCI config space definition
      OperationRegion (PXCS,SystemMemory, M2PC(\_SB.PCI0.RP12._ADR()), 0x20)  // Pci eXpress Configuration Space
      Field (PXCS,AnyAcc, NoLock, Preserve) {
        Offset(0x18), // Bus Number register
        PBUS, 8,      // Primary Bus Number
        SBUS, 8,      // Subordinate Bus Number
      }
      Store (\_SB.PCI0.GPCB(), Local0) // MMIO Base address
      Add (Local0, ShiftRight(And(Arg0,0x001F0000), 1), Local0)  // Device no
      Add (Local0, ShiftLeft(And(Arg0, 0x00000007), 12), Local0) // Function no
      Add (Local0, ShiftLeft(SBUS, 20), Local0)                  // Bus no

      Return (Local0)
    }

    Scope (PXSX){
      Method(_RST, 0, Serialized) {
        // WWAN PCI config space registers.
        OperationRegion(PXCS,SystemMemory,GMIO(\_SB.PCI0.RP12.PXSX._ADR),0x480)
        Field(PXCS,AnyAcc, NoLock, Preserve) {
          VDID, 16,
          DVID, 16,
          Offset(0x78),  // DCTL - Device Control Register
          DCTL, 16,
          Offset(0x7A),  // DSTS Device Status Register
          DSTS, 16,
          Offset(0x80),  // LCTL Link Control Register
          LCTL, 16,
          LSTS, 16,      // LSTS Link Status Register
          Offset(0x98),  // DCT2 Device Control 2 Register
          DCT2, 16,
          Offset(0x148), // LTR Capability
            , 32,        // Header
          MXSL, 16,      // Max Snoop Latency
          MNSL, 16,      // Max No Snoop Latency
          Offset(0x158), // L1 PM Substates Control Register
          L1SC, 32,
        }


        // Drive RESET Pin low (GPP_A_12)
        \_SB.SGOV (\WGUR, 0)
        Sleep (200) // Safe 200ms
        Notify (\_SB.PCI0.RP12.PXSX, 1)

        // De-Assert PERST
        If (LNotEqual(DeRefOf(Index(RSTP, 0)),0)) { // if reset pin enabled
          \_SB.CGWR (DeRefOf(Index(RSTP, 0)), DeRefOf(Index(RSTP, 1)), DeRefOf(Index(RSTP, 2)), DeRefOf(Index(RSTP, 3)))
        }

        // Drive RESET Pin high (GPP_A_12)
        \_SB.SGOV (\WGUR, 1)
        Sleep (200)

        // Restore saved parameters
        Or (L1SC, 0x0F, L1SC)
        If (LEqual(NEXP, 0)) { // If Native PCIe support is disabled
          Store (\WDCT, DCTL)
          Store (\WLCT, LCTL)
          Store (\WDC2, DCT2)
          Store (\WMXS, MXSL)
          Store (\WMNS, MNSL)
        } Else {
          Store (0x142, LCTL)
        }

        Notify (\_SB.PCI0.RP12.PXSX, 1)
      } // Method _RST
    } // Scope (PXSX)
  } // Scope (\_SB.PCI0.RP12)
} // End of Wwan SSDT

