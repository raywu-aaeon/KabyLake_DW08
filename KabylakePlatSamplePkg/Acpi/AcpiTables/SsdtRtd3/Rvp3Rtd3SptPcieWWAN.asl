/** @file
  ACPI RTD3 SSDT table for WWAN PCIE interface

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

#define PID_ICC                                   0xDC
#define R_PCH_PCR_ICC_MSKCKRQ                     0x100C                  ///< Mask Control CLKREQ

External(PCRA,MethodObj)
External(PCRO,MethodObj)
External(\WGUR)
External(WWAN_PCIE_ROOT_PORT._ADR, MethodObj)
External(WWAN_PCIE_ROOT_PORT.PXSX, DeviceObj)
External(WWAN_PCIE_ROOT_PORT.PXSX._ADR, IntObj)
External(\_SB.PCI0.GPCB, MethodObj)
External(\NEXP, IntObj)
External(\_SB.CGWR, MethodObj)

/// @details
/// The following namespace(variable) must be initialized
/// when including this file under PCIe Root Port scope:
/// SLOT: PCIe slot number                       \n
/// RSTG: reset GPIO pad    \n
//    sample:Name(RSTG, Package() {
//                   0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
//                   0, // IOEX #
//                   0, // GPIO pad #/IOEX pin #
//                   0  // reset pin de-assert polarity
//                 })
/// PWRG: power GPIO pad
//    sample:Name(PWRG, Package() {
//                   0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
//                   0, // IOEX #
//                   0, // GPIO pad #/IOEX pin #
//                   0  // power on polarity
//                 })
/// WAKG: WAKE GPIO pad
//    sample:Name(WAKG, Package() {
//                   0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
//                   0, // IOEX #
//                   0, // GPIO pad #/IOEX pin #
//                   0  // wake en polarity
//                 })
/// SCLK: ICC CLK_REQ pad
//    sample:Name(SCLK, Package() {
//                   0, // 0-disable, 1-enable
//                   0, // MSKCRQSRC[x]
//                   0  // assert polarity, ICC should be LOW-activated
//                 })
/// @defgroup pcie_scope PCIe Root Port Scope **/
    Name(WKEN, 0)
    Name(MOL3, 1)
    Name(OFEN, 0)
    Name(FLGN, 0)
    Name(FLGF, 0)
    Name(FLGW, 0)
    Name(_S0W, 3)

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
      OperationRegion (PXCS,SystemMemory, M2PC(WWAN_PCIE_ROOT_PORT._ADR()), 0x20)  // Pci eXpress Configuration Space
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

    Scope (WWAN_PCIE_ROOT_PORT.PXSX)
    {
      Method(_RST, 0, NotSerialized)
      {
        // WWAN PCI config space registers.
        OperationRegion(PXCS,SystemMemory,GMIO(WWAN_PCIE_ROOT_PORT._ADR),0x480)
        Field(PXCS,AnyAcc, NoLock, Preserve) {
          VDID, 16,
          DVID, 16,
          Offset(0x80),  // LCTL Link Control Register
          LCTL, 16,
          Offset(0x158), // L1 PM Substates Control Register
          L1SC, 32,
        }

        // Drive RESET Pin low
        \_SB.SGOV(\WGUR, 0)
        Sleep(200) // Safe 200ms
        Notify(WWAN_PCIE_ROOT_PORT.PXSX, 1)

        // De-Assert PERST
        if(LNotEqual(DeRefOf(Index(RSTG, 0)),0)) { // if reset pin enabled
          \_SB.CGWR(DeRefOf(Index(RSTG, 0)), DeRefOf(Index(RSTG, 1)), DeRefOf(Index(RSTG, 2)), DeRefOf(Index(RSTG, 3)))
        }

        // Drive RESET Pin high
        \_SB.SGOV(\WGUR, 1)

        Store(0, Local0)
        While(LEqual(WWAN_PCIE_ROOT_PORT.LASX, 0)) {
          If(Lgreater(Local0, 20))     // Wait for ~300ms
          {
            Break
          }
          Sleep(16)
          Increment(Local0)
        }
        Sleep(10)

        // Restore saved parameters
        Or (L1SC, 0x0F, L1SC)
        Store (0x142, LCTL)

        Notify(WWAN_PCIE_ROOT_PORT.PXSX, 1)
      }

      // For PXSX device
      Method(_PR0, 0) {
        Return (Package(){WWAN_PCIE_ROOT_PORT.PXP})
      }

      Method(_PR3, 0) {
        Return(Package(){WWAN_PCIE_ROOT_PORT.PXP})
      }

      Method(_S0W, 0) {
        //
        // WRTO - WWAN RTD3 Option
        //   3: D3/L2
        //   1: D0/L1.2
        //   0: Disabled
        //
        If(LEqual(WRTO, 3)) {
          Return(4)
        } Else {
          Return(3)
        }
      }

      //----------------------------------------------------------------------------------------
      //  Intel Proprietary WWAN Enabling DSM Method
      //
      //  Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
      //  Arguments:
      //    Arg0: UUID Unique function identifier - bad01b75-22a8-4f48-8792-bdde9467747d
      //    Arg1: Integer Revision Level - 0
      //    Arg2: Integer Function Index -
      //        Function Index - 0 (Returns the supported _DSM methods)
      //        Function Index - 1 (Reserved)
      //        Function Index - 2 (Reserved)
      //        Function Index - 3 (Conveys the BIOS ACPI Device Power Mode setup option selection to the WWAN driver)
      //    Arg3: Package Parameters - None
      //-----------------------------------------------------------------------------------------
      // DSM UUID for WWAN Enabling. Do Not change.
      Method(_DSM, 0x4, Serialized) {
        // DSM UUID for WWAN. Do Not change.
        If(LEqual(Arg0, ToUUID("bad01b75-22a8-4f48-8792-bdde9467747d")))
        {
          //
          // Function 0: Query of supported functions.
          //
          If(LEqual(Arg2, Zero))
          {
            Return(Buffer(One) { 0x3 })
          }

          //
          // Function 1: Reserved.
          //
          If(LEqual(Arg2, 1))
          {
            //Reserved
          }

          //
          // Function 2: Reserved
          //
          If(LEqual(Arg2, 2))
          {
            //Reserved
          }

          //
          // Function 3: Conveys the BIOS ACPI Device Power Mode setup option selection to the WWAN driver
          //
          If(LEqual(Arg2, 3))
          {
            //
            // _DSM Power
            // WRTO - WWAN RTD3 Option
            //   3: D3/L2
            //   1: D0/L1.2
            //   0: Disabled
            //
            Return(WRTO)
          }
        }
        Return(Buffer(One) { 0x00 }) // Guid mismatch
      }
    } // (WWAN_PCIE_ROOT_PORT.PXSX)

    Method(_DSW, 3) {
      // This method is used to enable/disable wake from PCIe (WKEN)
      If(Arg1)
      { /// If entering Sx, need to disable WAKE# from generating runtime PME
        Store(0, WKEN)
      } Else {  /// If Staying in S0
        If(LAnd(Arg0, Arg2)) ///- Check if Exiting D0 and arming for wake
        { ///- Set PME
          Store(1, WKEN)
        } Else { ///- Disable runtime PME, either because staying in D0 or disabling wake
          Store(0, WKEN)
        }
      }

      If(LEqual(Arg0,0)) {
        Store(1, MOL3) //Modem will go into L3
      } Else {
        Store(0, MOL3) // Modem will go to L2 state
      }
      /** @defgroup pcie_dsw PCIE _DSW **/
    } // End _DSW

    PowerResource(PXP, 0, 0)
    {
      /// Define the PowerResource for PCIe slot
      /// Method: _STA(), _ON(), _OFF()
      /** @defgroup pcie_pxp PCIE Power Resource **/
      Method(_STA, 0)
      {
        If (LEqual(\EMOD, 1)) {
          Return(1)
        } Else {
          Return(PSTA())
        }
      }  /** @defgroup pcie_sta PCIE _STA method **/

      Method(_ON) /// Turn on core power to PCIe Slot
      {
        // Store the current MOL3 to prevent race problem that a new _DSW MOL3 setting is called before _ON finish
        Store(MOL3, FLGN)
        Store(1, OFEN)
        If (LEqual(\EMOD, 1)) {
          // Do nothing
        } Else {
          //M.2 Slot Duplicate Start
          // Always turn ON power to M.2 Slot if ON
          // drive RESET Pin high (GPP_A_12)
          // Delay of 20ms to make it ensure modem is come out from Reset modem
          \_SB.SGOV(\WGUR, 1)
          Sleep(20)
          \_SB.PCI0.GEXP.SGEP(1, 0, 1)   // set power pin to high
          PON()
          If(FLGN) {
            // This time includes PCIe Detect Timer and PCIe Link Establishment Timer for modem. Time delay should
            // kick in L3 Path
            Sleep(500)
          }
        }
      } /** @defgroup pcie_on PCIE _ON method **/

      Method(_OFF) /// Turn off core power to PCIe Slot
      {
        // Store the current MOL3/WKEN to prevent race problem that a new _DSW MOL3/WKEN setting is called before _OFF finish
        Store(MOL3, FLGF)
        Store(WKEN, FLGW)
        If (LOr(LEqual(\EMOD, 1),LEqual(OFEN, 0))) {
          // Do nothing
        } Else {
          POFF()
          //M.2 Slot Duplicate Start
          If(FLGF){
            // drive RESET Pin low (GPP_A_12)
            \_SB.SGOV(\WGUR, 0)
            Sleep(2) // 2ms
            \_SB.PCI0.GEXP.SGEP(1, 0, 0)   // set power pin to low
            // Modem Rigorous Cycle and hence needs introduce sleep of sleep 500 ms
            Sleep(500)
          }
          //M.2 Slot Duplicate End
        }
        Store(0,OFEN)
      } // End of Method_OFF
    } // End PXP

    Method(PSTA, 0)
    {
      /// Returns the status of PCIe slot core power

      If (LEqual (VDID, 0xFFFFFFFF)) {
        Return(0)
      }

      // detect power pin status
      if(LNotEqual(DeRefOf(Index(PWRG, 0)),0)) {
        if(LEqual(DeRefOf(Index(PWRG, 0)),1)) { // GPIO mode
          if(LEqual(\_SB.GGOV(DeRefOf(Index(PWRG, 2))),DeRefOf(Index(PWRG, 3)))){
            Return (1)
          } Else {
            Return (0)
          }
        } // GPIO mode
        if(LEqual(DeRefOf(Index(PWRG, 0)),2))  { // IOEX mode
          if(LEqual(\_SB.PCI0.GEXP.GEPS(DeRefOf(Index(PWRG, 1)),DeRefOf(Index(PWRG, 2))),DeRefOf(Index(PWRG, 3)))){
              Store(1, Local0)
          } Else {
            Return (0)
          }
        } // IOEX mode
      }
      // detect reset pin status
      if(LNotEqual(DeRefOf(Index(RSTG, 0)),0)) {
        if(LEqual(DeRefOf(Index(RSTG, 0)),1)) { // GPIO mode
          if(LEqual(\_SB.GGOV(DeRefOf(Index(RSTG, 2))),DeRefOf(Index(RSTG, 3)))){
            Return (1)
          } Else {
            Return (0)
          }
        } // GPIO mode
        if(LEqual(DeRefOf(Index(RSTG, 0)),2))  { // IOEX mode
          if(LEqual(\_SB.PCI0.GEXP.GEPS(DeRefOf(Index(RSTG, 1)),DeRefOf(Index(RSTG, 2))),DeRefOf(Index(RSTG, 3)))){
            if(LEqual(Local0, 1)) {
              Return (1)
            } Else {
              Return (0)
            }
          } Else {
            Return (0)
          }
        } // IOEX mode
      }
      Return (0)
    }  /** @defgroup pcie_sta PCIE _STA method **/

    Method(PON) /// Turn on core power to PCIe Slot
    {

      If (LEqual (VDID, 0xFFFFFFFF)) {
        Return()
      }

      /// Turn ON Power for PCIe Slot

      //If (MOL3) {
        /// Turn ON Power for PCIe Slot
        If(LNotEqual(DeRefOf(Index(PWRG, 0)),0)) { // if power gating enabled
          \_SB.CGWR(DeRefOf(Index(PWRG, 0)), DeRefOf(Index(PWRG, 1)), DeRefOf(Index(PWRG, 2)), DeRefOf(Index(PWRG, 3)))
          Sleep(PEP0)     /// Sleep for programmable delay
        }
      //}

      // As per PCIe M.2 Spec , clock Should be applied in 20 microsecond
      Stall(20)
      /// De-assert CLK_REQ MSK
      If(LNotEqual(DeRefOf(Index(SCLK, 0)),0)) { // if CLK_REQ gating enabled
        PCRA(PID_ICC, R_PCH_PCR_ICC_MSKCKRQ, Not(DeRefOf(Index(SCLK, 1))))  // And ~SCLK to clear bit
      }

      // PERST De-assertion should be greater than 100 ms
      If(FLGN) {
        Sleep(120)
      }


      /// De-Assert Reset Pin
      If(LNotEqual(DeRefOf(Index(RSTG, 0)),0)) { // if reset pin enabled
        \_SB.CGWR(DeRefOf(Index(RSTG, 0)), DeRefOf(Index(RSTG, 1)), DeRefOf(Index(RSTG, 2)), DeRefOf(Index(RSTG, 3)))
        //Sleep(PEP0)     /// Sleep for programmable delay
      }
      /// Clear DLSULPPGE, then set L23_Rdy to Detect Transition  (L23R2DT)
      Store(0, DPGE)
      Store(1, L23R)
      Sleep(16)
      Store(0, Local0)
      /// Wait up to 12 ms for transition to Detect
      While(L23R) {
        If(Lgreater(Local0, 4))    // Debug - Wait for 5 ms
        {
          Break
        }
        Sleep(16)
        Increment(Local0)
      }
      /// Once in Detect, wait up to 124 ms for Link Active (typically happens in under 70ms)
      /// Worst case per PCIe spec from Detect to Link Active is:
      /// 24ms in Detect (12+12), 72ms in Polling (24+48), 28ms in Config (24+2+2+2+2)
      Store(1, DPGE)
      Store(0, Local0)
      While(LEqual(LASX,0)) {
        If(Lgreater(Local0, 8))
        {
          Break
        }
        Sleep(16)
        Increment(Local0)
      }
      // ADBG(Local0) // uncomment to print the timeout value for debugging
      Store(0, LEDM) /// Set PCIEDBG.DMIL1EDM (324[3]) = 0
    } /** @defgroup pcie_on PCIE _ON method **/

    Method(POFF) /// Turn off core power to PCIe Slot
    {

      If (LEqual (VDID, 0xFFFFFFFF)) {
        Return()
      }

      /// Set L23_Rdy Entry Request (L23ER)
      Store(1, L23E)
      Sleep(16)
      Store(0, Local0)
      While(L23E) {
        If(Lgreater(Local0, 4))    /// Debug - Wait for 5 ms
        {
          Break
        }
        Sleep(16)
        Increment(Local0)
      }
      Store(1, LEDM) /// PCIEDBG.DMIL1EDM (324[3]) = 1

      /// Assert Reset Pin
      if(LNotEqual(DeRefOf(Index(RSTG, 0)),0)) { // if reset pin enabled
        \_SB.CGWR(DeRefOf(Index(RSTG, 0)), DeRefOf(Index(RSTG, 1)), DeRefOf(Index(RSTG, 2)), Xor(DeRefOf(Index(RSTG, 3)), 1))
      }

      /// Assert CLK_REQ MSK
      if(LNotEqual(DeRefOf(Index(SCLK, 0)), 0)) { // if CLK_REQ gating enabled
        PCRO(PID_ICC, R_PCH_PCR_ICC_MSKCKRQ, DeRefOf(Index(SCLK, 1)))    // Or SCLK to set bit
        Sleep(16)
      }
      /// Power OFF for Slot
      If(FLGF){
        If(LNotEqual(DeRefOf(Index(PWRG, 0)), 0)) { // if power gating enabled
          \_SB.CGWR(DeRefOf(Index(PWRG, 0)), DeRefOf(Index(PWRG, 1)), DeRefOf(Index(PWRG, 2)), Xor(DeRefOf(Index(PWRG, 3)), 1))
        }
      }



      Store(1, LDIS) /// Set Link Disable
      Store(0, LDIS) /// Toggle Link Disable

      /// Enable WAKE
      If(FLGW) {
        if(LNotEqual(DeRefOf(Index(WAKG, 0)),0)) { // if wake gating enabled
          if(LEqual(DeRefOf(Index(WAKG, 0)),1)) { // GPIO mode
            \_SB.SHPO(DeRefOf(Index(WAKG, 2)), 0) // set gpio ownership to ACPI(0=ACPI mode, 1=GPIO mode)
          }
          if(LEqual(DeRefOf(Index(WAKG, 0)),2)) { // IOEX mode
            \_SB.PCI0.GEXP.SGEP(DeRefOf(Index(WAKG, 1)),DeRefOf(Index(WAKG, 2)),DeRefOf(Index(WAKG, 3)))
          }
        }
      }
      /** @defgroup pcie_off PCIE _OFF method **/
    } // End of Method_OFF

    Name(_PR0, Package(){PXP})
