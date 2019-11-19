/** @file
  ACPI RTD3 SSDT table for SKL SDS

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

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
    "Rtd3.aml",
    "SSDT",
    2,
    "AcpiRef",
    "SDS_Rtd3",
    0x1000
    )
{
External(RTD3, IntObj)
External(S0ID)
External(OSYS)

// AMI_OVERRIDE_START - Fix Linux build error.
//Include ("Rtd3Common.asl")
Include ("RTD3Common.asl")
// AMI_OVERRIDE_END - Fix Linux build error.

External(\_SB.PCI0.HDAS, DeviceObj)
External(\_SB.PCI0.HDAS.VDID)

External(\_SB.PCI0.XHC.RHUB.HS03, DeviceObj) //EHCI HSP port 3
External(\_SB.PCI0.XHC.RHUB.HS04, DeviceObj) //EHCI HSP port 4
External(\_SB.PCI0.XHC.RHUB.SS03, DeviceObj) //xHCI SSP port 3
External(\_SB.PCI0.XHC.RHUB.SS04, DeviceObj) //xHCI SSP port 4
External(\_SB.PCI0.XHC.XFLT, IntObj)         //xHCI Filter Driver Flag

External(PEPC)

External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)
External(\_SB.PCI0.I2C1.TPL1._STA, MethodObj)
External(\_SB.PCI0.RP05.PXSX, DeviceObj)
External(\_SB.PCI0.RP05.PXSX.PAHC, MethodObj)
External(\_SB.PCI0.RP05.PXSX.PNVM, MethodObj)

// PCIe root ports - START

    ///
    /// PCIe RTD3 - PCIe SSD
    ///
  If (And(RCG0, 0x04)){
    Scope(\_SB.PCI0.RP05)
    {
      // reset pin = GPIO_SKL_LP_GPP_G2; PLT_RST signal seems to be used after it is changed to BUF_PLT_RST
      // power pin = GPIO_SKL_LP_GPP_G3
      // wake pin  = none
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC5 (MSKCRQSRC5)
      Name(SLOT, 5)   ///- Slot #5
      Name(RSTG, Package() {
              1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              GPIO_SKL_LP_GPP_G2, // GPIO pad #/IOEX pin #
              1  // reset pin de-assert polarity
            })
      Name(PWRG, Package() {
              1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              GPIO_SKL_LP_GPP_G3, // GPIO pad #/IOEX pin #
              1  // power on polarity
            })
      Name(WAKG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // wake en polarity
            })
    Name(SCLK, Package() {
            1,    // 0-disable, 1-enable
            0x08, // MSKCRQSRC3
            0     // assert polarity, ICC should be LOW-activated
          })
// AMI_OVERRIDE_START - Fix Linux build error.
//      Include("Rtd3SptPcie.asl")
      Include("RTD3SptPcie.asl")
// AMI_OVERRIDE_END - Fix Linux build error.
    }

    Scope(\_SB.PCI0.RP05.PXSX)
    {
      Name(_S0W,0x04) // PCIe SSD NVMe

      Method(_PR0, Zero, NotSerialized, 0, PkgObj){
        If(LOr(PAHC(), PNVM())){ // Check if PCIe AHCI controller or NVMe
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          }Else{
            If (CondRefOf(\_SB.PCI0.RP05.PXP)) {
              Return(Package(){\_SB.PCI0.RP05.PXP})
            }
          }
        }
        Return(Package(){})
      }

      Method(_PR3, Zero, NotSerialized, 0, PkgObj){
        If(LOr(PAHC(), PNVM())){ // Check if PCIe AHCI controller or NVMe
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          }Else{
            If (CondRefOf(\_SB.PCI0.RP05.PXP)) {
              Return(Package(){\_SB.PCI0.RP05.PXP})
            }
          }
        }
        Return(Package(){})
      }

      Method(_PS0,0,Serialized){
      }

      Method(_PS3,0,Serialized){
      }

      Device(MINI){
        Method(_STA,0,Serialized){
          If(PAHC()){
            Return(0x0F) // AHCI controller mode
          }Else{
            Return(0x00) // NVMe mode
          }
        } // End of _STA

        Name(_ADR,0x0000FFFF) // Port 0
        Name(_S0W,0x04) // PCIe SSD AHCI MiniPort D3 cold capable

        Method(_PS0,0,Serialized){
        }

        Method(_PS3,0,Serialized){
        }

        Method(_PR0, Zero, NotSerialized, 0, PkgObj){
          If(PAHC()){ // Check if PCIe AHCI controller
            If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
              If (CondRefOf(\_SB.MODS)) {
                Return(Package(){\_SB.MODS})
              }
            }Else{
              If (CondRefOf(\_SB.PCI0.RP05.PXP)) {
                Return(Package(){\_SB.PCI0.RP05.PXP}) // AHCI controller mode
              }
            }
          }
          Return(Package(){}) // NVMe mode
        }

        Method(_PR3, Zero, NotSerialized, 0, PkgObj){
          If(PAHC()){ // Check if PCIe AHCI controller
            If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
              If (CondRefOf(\_SB.MODS)) {
                Return(Package(){\_SB.MODS})
              }
            }Else{
              If (CondRefOf(\_SB.PCI0.RP05.PXP)) {
                Return(Package(){\_SB.PCI0.RP05.PXP}) // AHCI controller mode
              }
            }
          }
          Return(Package(){}) // NVMe mode
        }
      }// Endof MINI
    } // End of Scope(\_SB.PCI0.RP05)
  } // End of If (And(RCG0, 0x04))
// PCIe root ports - END

// SATA - START
    If(LAnd(And(SPST, 0x02),And(RCG1, 0x02))){  // SDS supports only Port1. RTD3 for SATA Port1(RCG1 Bit1) is enabled.
      Scope(\_SB.PCI0.SAT0) {
        /// _PS0 Method for SATA HBA
        Method(_PS0,0,Serialized)
        {
        }

        /// _PS3 Method for SATA HBA
        Method(_PS3,0,Serialized)
        {
        }

        /// Define SATA PCI Config OperationRegion
        OperationRegion(SMIO,PCI_Config,0x24,4)
        Field(SMIO,AnyAcc, NoLock, Preserve) {
          Offset(0x00), ///- SATA MABR6
          MBR6, 32,     ///- SATA ABAR
        }

        OperationRegion(PCIR, PCI_Config, 0x00, 0x10)
        Field(PCIR,DWordAcc, NoLock, Preserve)
        {
          Offset(0x0),
          SVID, 16,
          SDID, 16,
          Offset(0x0A),
          SUBC,   8  // Link Control register
        }


        Scope(PRT1) {
          /// Define _PR0, _PR3 PowerResource Package
          // P1 command port = ABAR+0x198
          // power pin = none
          Name(PORT, 1)
          Name(PBAR, 0x198)       // port0 command port address
          Name(PWRG, Package() {
                     1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                     0, // IOEX # / Not used for GPIO
                     GPIO_SKL_LP_GPP_G3, // GPIO pad #/IOEX pin #
                     1  // power on polarity
          })

          /// _S0W method returns the lowest D-state can be supported in state S0.
          Method(_S0W, 0)
          {
            Return(3) // return 3 (D3hot)
          } // End _S0W

      Method(_PR0) {
        If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
          If (CondRefOf(\_SB.MODS)) {
            Return(Package(){\_SB.MODS})
          }
        }Else{
          If (CondRefOf(SPPR)) {
            Return(Package(){SPPR})
          }
        }
        Return(Package(){})
      }
      Method(_PR3) {
        If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
          If (CondRefOf(\_SB.MODS)) {
            Return(Package(){\_SB.MODS})
          }
        }Else{
          If (CondRefOf(SPPR)) {
            Return(Package(){SPPR})
          }
        }
        Return(Package(){})
      }

      PowerResource(SPPR, 0, 0)
      {
        Method(_STA)
        {
          If (LEqual(\EMOD, 1)) {
            Return(0x01)
          } Else {
            Return(SPSA())
          }
        }


         Method(_ON)
        {
          If (LEqual(\EMOD, 1)) {
            // Do nothing
          } Else {
            SPON()                                              /// _ON Method
            Sleep(16) // Delay for power ramp.
          }
        }
        Method(_OFF)
        {
          If (LEqual(\EMOD, 1)) {
            // Do nothing
          } Else {
            SPOF()
          }
        }

      } // end SPPR
      Name(OFTM, Zero) /// OFTM: Time returned by Timer() when the Power resource was turned OFF

      Method(SPSA)
      {
        // check power control enable
        If(LNotEqual(DeRefOf(Index(PWRG, 0)), 0)) {
          // read power pin status
          If(LEqual(DeRefOf(Index(PWRG, 0)),1)) { // GPIO mode
            if(LEqual(\_SB.GGOV(DeRefOf(Index(PWRG, 2))),DeRefOf(Index(PWRG, 3)))) {
              Return(0x01)
            } Else {
              Return(0x00)
            }
          }
          If(LEqual(DeRefOf(Index(PWRG, 0)),2))  { // IOEX mode
            If(LEqual(\_SB.PCI0.GEXP.GEPS(DeRefOf(Index(PWRG, 1)),DeRefOf(Index(PWRG, 2))),DeRefOf(Index(PWRG, 3)))) {
              Return(0x01)
            } Else {
              Return(0x00)
            }
          }
        }
        Return(0x00) // disabled
      } /// @defgroup sata_prt1_sta     SATA Port 1 PowerResource _STA Method

      Method(SPON, 0)
      {
        If(LNotEqual(^OFTM, Zero)) { /// if OFTM != 0 => Disk was turned OFF by asl
          Divide(Subtract(Timer(), ^OFTM), 10000, , Local0) ///- Store Elapsed time in ms
          Store(Zero, ^OFTM) ///- Reset OFTM to zero to indicate minimum 50ms requirement does not apply when _ON called next time
          If(LLess(Local0, 50)) ///- Do not sleep if already past the delay requirement
          {
            Sleep(Subtract(50, Local0)) ///- Sleep 50ms - time elapsed
          }
        }

        // drive power pin "ON"
        if(LNotEqual(DeRefOf(Index(PWRG, 0)),0)) {     // if power gating enabled
          if(LEqual(DeRefOf(Index(PWRG, 0)),1)) {      // GPIO mode
            \_SB.SGOV(DeRefOf(Index(PWRG, 2)),DeRefOf(Index(PWRG, 3)))
          }
          if(LEqual(DeRefOf(Index(PWRG, 0)),2))  {     // IOEX mode
            \_SB.PCI0.GEXP.SGEP(DeRefOf(Index(PWRG, 1)),DeRefOf(Index(PWRG, 2)),DeRefOf(Index(PWRG, 3)))
          }
        }
      }

      Method(SPOF, 0)
      {
        Add(\_SB.PCI0.SAT0.MBR6, PBAR, Local0)
            /// if S0Ix enabled
        If(LEqual(S0ID, 1))
        {
            OperationRegion(PSTS, SystemMemory, Local0, 0x18)
            Field(PSTS, DWordAcc, NoLock, Preserve)
            {
              Offset(0x0),
              CMST, 1,  //PxCMD.ST
              CSUD, 1,  //PxCMD.SUD
              , 2,
              CFRE, 1,  //PxCMD.FRE
              Offset(0x10),
              SDET, 4,  //PxSSTS.DET
              Offset(0x14),
              CDET, 4   //PxSCTL.DET
            }

              // WA for RST driver creating volume failed.
              // In order to fix the error caused by reigster change during creating volume of RST driver, skip to disable phy in RAID mode.
              If(LAnd(LEqual(\_SB.PCI0.SAT0.SUBC, 0x06),LOr(LEqual(SDET, 1), LEqual(SDET, 3))))  /// Execute offline flow only if Device detected and Phy not offline
              {
                ///- Clear ST (PxCMD.ST)
                Store(0, CMST)                   // PBAR[0]
                ///- Clear FRE
                Store(0, CFRE)                   // PBAR[4]
                ///- Clear SUD (PxCMD.SUD)
                Store(0, CSUD)                   // PBAR[1]
                ///- Set DET to 4 (PxSCTL.DET)
                Store(4, CDET)                   // PBAR+0x14[3:0]
                Sleep(16)
                ///- Wait until PxSSTS.DET == 4
                While(LNotEqual(SDET, 4)){
                  Sleep(16)
                }
              }
         } // if S0Ix enabled
          // drive power pin "OFF"
          If(LNotEqual(DeRefOf(Index(PWRG, 0)),0)) { // if power gating enabled
            if(LEqual(DeRefOf(Index(PWRG, 0)),1)) { // GPIO mode
              \_SB.SGOV(DeRefOf(Index(PWRG, 2)),Xor(DeRefOf(Index(PWRG, 3)),1))
            }
            if(LEqual(DeRefOf(Index(PWRG, 0)),2))  { // IOEX mode
              \_SB.PCI0.GEXP.SGEP(DeRefOf(Index(PWRG, 1)),DeRefOf(Index(PWRG, 2)),Xor(DeRefOf(Index(PWRG, 3)),1))
            }
          }

        Store(Timer(), ^OFTM) /// Store time when Disk turned OFF(non-zero OFTM indicate minimum 50ms requirement does apply when _ON called next time)
      } // end _OFF

        } // end device(PRT1)

        ///
        /// _DSM Device Specific Method supporting AHCI DEVSLP
        ///
        /// Arg0: UUID Unique function identifier \n
        /// Arg1: Integer Revision Level \n
        /// Arg2: Integer Function Index \n
        /// Arg3: Package Parameters \n
        ///
        /// Variables:
        Name(DRV, 0)  /// Storage for _DSM Arg3 parameter 0
        Name(PWR, 0)  /// Storage for _DSM Arg3 parameter 1

        Method (SDSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {

        /// Check UUID of _DSM defined by Microsoft
          If (LEqual(Arg0, ToUUID ("E4DB149B-FCFE-425b-A6D8-92357D78FC7F"))) {
            ///
            /// Switch by function index
            ///
            Switch(ToInteger(Arg2)) {
              Case (0) {  /// case 0:
                ///- Standard query - A bitmask of functions supported
                ///- Supports function 0-3
                Return(0x0f)
              }
              Case (1) {  /// case 1:
                ///- Query Device IDs (Addresses) of children where drive power and/or DevSleep are supported.
                ///- SATA HBA provides autonomous link (DevSleep) support, return a package of 0 elements
                Return( Package(){})     ///- SATA HBA provides native DevSleep
              }
              Case (2) { /// Case 2: Control power to device.
                Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
                Store(ToInteger(DerefOf(Index(Arg3, One))), PWR)

                Switch(ToInteger(DRV)) {
                  Case (Ones){
                    ///- Turn on drive and link power to all drives. Note that even though
                    If(PWR){ // Applying Power
                      \_SB.PCI0.SAT0.PRT1.SPPR._ON() ///- apply power to port 1
                    }
                  }
                } //Switch(DRV)
                Return (0)
              } //Case (2)
              Case (3){ /// Case 3: Current status of Device/Link of Port
                Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
                Switch(ToInteger(DRV)) {  /// Check for SATA port
                  Case (0x0001FFFF){  ///- SATA Port 1
                    ///- Bit0 => Device power state
                    If(LEqual(\_SB.PCI0.SAT0.PRT1.SPPR._STA(), 0)){
                      Store(0, Local0)
                    }Else{
                      Store(1, Local0)
                    }
                    Return (Local0)
                  }
                  Default { ///- Invalid SATA Port - error
                    Return (Ones)
                  }
                }
              }
              Default {
                Return (0)
              }
            }
          } Else { // UUID does not match
            Return (Buffer() {0})
          }
        } /// @defgroup sata_dsm SATA _DSM Method
      } //Scope(\_SB.PCI0.SAT0)
    }
// SATA - END

// USB - START
    Scope(\_SB.PCI0.XHC.RHUB){ //USB XHCI RHUB

      /// PS0X method is called by PS0 method in PchXhci.asl
      Method(PS0X,0,Serialized)
      {
      }

      /// PS2X method is called by PS2 method in PchXhci.asl
      /// It updates USPP variable base on USB port current connect status
      Method(PS2X,0,Serialized)
      {
      } // End of PS2 method
      ///@defgroup xhc_rhub_ps2x XHCI RHUB PS2X Method

      /// PS3X method is called by _PS3 method in PchXhci.asl
      Method(PS3X,0,Serialized)
      {
      } // End of PS3 method
      ///@defgroup xhc_rhub_ps3x XHCI RHUB PS3X Method

    } // Scope(\_SB.PCI0.XHC.RHUB)

    ///
    /// If USB Camera RTD3 support is enabled, associate _PR0, PR2, PR3 for USB
    /// SuperSpeed Port 4 (IVCAM Camera) with \ref xhc_pr_pcam
    /// @addtogroup xhc_scope
    ///
    Include ("Rtd3Camera.asl")

    If(LEqual(And(RCG0,2),2)) {
      Scope(\_SB.PCI0.XHC.RHUB.SS04) {       // USB Superspeed Port 4 - IVCAM RGB camera
        Method(_PR0) {
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          }Else{
            If (CondRefOf(\_SB.PCI0.XHC.RHUB.PCAM)) {
              Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
            }
          }
          Return(Package(){})
        }
        Method(_PR2) {
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          } Else {
            If (CondRefOf(\_SB.PCI0.XHC.RHUB.PCAM)) {
              Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
            }
          }
          Return(Package(){})
        }
        Method(_PR3) {
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          }Else{
            If (CondRefOf(\_SB.PCI0.XHC.RHUB.PCAM)) {
              Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
            }
          }
          Return(Package(){})
        }
      } // Scope(\_SB.PCI0.XHC.RHUB.SS04)
    } // If(LEqual(And(RCG0,2),2)

    ///
    /// If WWAN RTD3 support is enabled, associate _PR0, PR2, PR3 for USB Highspeed Port 3 with \ref xhc_pr_px03
    ///@addtogroup xhc_scope
    If(LEqual(And(RCG0,0x80),0x80)) {
      Scope(\_SB.PCI0.XHC.RHUB){
        // USB power resource 3
        // power pin = GPIO_SKL_LP_GPP_A16
        // wake pin = GPIO_SKL_LP_GPP_G7
        // sim card detect pin = GPIO_SKL_LP_GPP_F23
        PowerResource(PX03, 0, 0){                              /// Power rail for USB2.0 port 3 - power pin = GPP_A16
          Name(WOFF, 0)  // Last OFF Timestamp (WOFF): The time stamp of the last power resource _OFF method evaluation
          Method(_STA){                                         /// _STA method
            If(LEqual(\_SB.GGOV(GPIO_SKL_LP_GPP_A16),1)) {
              Return(0x01)
            } Else {
              Return(0x00)
            }
          }
          Method(_ON, 0) {                                      /// _ON Method
            If(LNotEqual(^WOFF, Zero)) {
              Divide(Subtract(Timer(), ^WOFF), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
              If(LLess(Local0,600)) {                           // If Elapsed time is less than 100ms
                Sleep(Subtract(600,Local0))                     // Sleep for the remaining time
              }
              Store(0, ^WOFF)
            }

            Store(Timer(), Local0)                               // Start OFF timer here.
            // drive RESET Pin high (GPP_A_12)
            \_SB.SGOV(GPIO_SKL_LP_GPP_A12, 1)

            Divide(Subtract(Timer(), Local0), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
            If(LLess(Local0,2)) {                           // If Elapsed time is less than 2ms
              Sleep(Subtract(2,Local0))                     // Sleep for the remaining time
            }
            \_SB.SGOV(GPIO_SKL_LP_GPP_A16, 1)                   // VR stagger GPP_A16 to high to power ON device
          }

          Method(_OFF, 0) {                                     /// _OFF Method
            Store(Timer(), Local0)                               // Start OFF timer here.

            // drive RESET Pin low (GPP_A_12)
            \_SB.SGOV(GPIO_SKL_LP_GPP_A12, 0)

            Divide(Subtract(Timer(), Local0), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
            If(LLess(Local0,2)) {                           // If Elapsed time is less than 2ms
              Sleep(Subtract(2,Local0))                     // Sleep for the remaining time
            }

            \_SB.SGOV(GPIO_SKL_LP_GPP_A16,0)                    // VR stagger GPP_A16 to low to power OFF device
            Store(Timer(), ^WOFF)                               // Start OFF timer here.
          }
        } // End PX03
      } // End Scope(\_SB.PCI0.XHC.RHUB)

      Scope(\_SB.PCI0.XHC.RHUB.HS03){
        Name(_S0W,2)                                  // Indicate WWAN can wake from D2 while in S0 using selective suspend
        Name(_PR0,Package(){\_SB.PCI0.XHC.RHUB.PX03}) // Power Resource required to support D0
        Name(_PR2,Package(){\_SB.PCI0.XHC.RHUB.PX03}) // Power Resource required to support D2
        Name(_PR3,Package(){\_SB.PCI0.XHC.RHUB.PX03}) // Power Resource required to support D3

        //
        // WWAN Modem device with the same power resource as its composite parent device
        //
        Device (MODM) {
          Name(_ADR, 0x3)
          Name(_PR0,Package(){\_SB.PCI0.XHC.RHUB.PX03}) // Power Resource required to support D0
          Name(_PR2,Package(){\_SB.PCI0.XHC.RHUB.PX03}) // Power Resource required to support D2
          Name(_PR3,Package(){\_SB.PCI0.XHC.RHUB.PX03}) // Power Resource required to support D3
        }
      } // Scope(\_SB.PCI0.XHC.RHUB.HS03)
    } // If(LEqual(And(RCG0,0x80),0x80))

// USB - END

// Serial IO - START
  // PCH I2C1 - TouchPanel Power control
  If (And(RCG0, 0x10)){ // I2C Touch panel RTD3 is enabled?
    Scope(\_SB.PCI0.I2C1){
      Method(_INI, 0){
        If (LAnd(LEqual(\EMOD, 1), LGreaterEqual(\OSYS, 2015))) {
          // If Modern Standby is enabled and OS is beyond Win10, use MODS for Touch panel power resource.
          If (CondRefOf(\_SB.MODS)){
            CopyObject(\_SB.PCI0.I2C1.TPL1.TD_Z, \_SB.PCI0.I2C1.TPL1.TD_P)
          }
        } Else {
          If (CondRefOf(\_SB.PCI0.I2C1.PXTC)) {
            // If normal Touch Panel PowerResource exists, use that.
            CopyObject(\_SB.PCI0.I2C1.TPL1.TD_X, \_SB.PCI0.I2C1.TPL1.TD_P)
          }
        }
      }

      /// PS0X Method, called by PS0 method in PchSerialIo.asl
      Method(PS0X,0,Serialized)
      {
      }

      /// PS3X Method, called by PS3 method in PchSerialIo.asl
      Method(PS3X,0,Serialized)
      {
      }
      /// \ref i2c1_pr_pxtc
      ///@defgroup i2c1_scope  I2C1 Scope

      PowerResource(PXTC, 0, 0){ /// Power rail for Touch Panel (GPP, B, 14)      Method(_STA){
        Method(_STA){
          If (LEqual(\EMOD, 1)) {
            Return(1)
          } Else {
            Return(PSTA())
          }
        }

        Method(_ON){
          If (LEqual(\EMOD, 1)) {
            // Do nothing
          } Else {
            PON()
          }
        }

        Method(_OFF){
          If (LEqual(\EMOD, 1)) {
            // Do nothing
          } Else {
            POFF()
          }
        }
      } // End PXTC

      /// Variable:
      Name(ONTM, 0) ///ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON

      Method(PSTA){
        If(LEqual(\_SB.GGOV(GPIO_SKL_LP_GPP_B14),1)) {
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(PON, 0) /// _ON Method \n Turn on
      {
        // De-Assert GPIO RST (GPP_E_3)
        \_SB.SGOV(GPIO_SKL_LP_GPP_E3, 1)
        // drive pwr high (GPP_B_14)
        \_SB.SGOV(GPIO_SKL_LP_GPP_B14, 1)
        // update ONTM
        Store(Timer(), ONTM)
        // enable int line (GPP_E_7)
        \_SB.SGRA(GPIO_SKL_LP_GPP_E7, 1)
      }

      Method(POFF, 0)  /// _OFF method \n Turn off
      {
        // disable int line (GPP_E_7)
        \_SB.SGRA(GPIO_SKL_LP_GPP_E7, 0)
        // drive pwr low (GPP_B_14)
        \_SB.SGOV(GPIO_SKL_LP_GPP_B14, 0)
        // Assert GPIO RST (GPP_E_3)
        \_SB.SGOV(GPIO_SKL_LP_GPP_E3, 0)
        // update ONTM
        Store(Zero , ^ONTM)  ///- Clear ONTM
      }

      Scope(TPL1){
        Name (TD_P, Package(){})                                  // TD_P - Touch Device Power Package
        Name (TD_X, Package(){\_SB.PCI0.I2C1.PXTC})               // TD_P - Touch Device Power Package
        Name (TD_Z, Package(){\_SB.MODS})                         // TD_Z - Touch Device Power Package

        Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
        Alias(\_SB.PCI0.I2C1.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

        Include("Rtd3I2cTouchDev.asl")
        Method(_PS0) { PS0X() }
        Method(_PS3) { PS3X() }
      }// End Of Scope(TPL1)
    }//  Scope(\_SB.PCI0.I2C1)
  } //  If (And(RCG0, 0x10))

// Serial IO End
    //Power Resource for Audio Codec
    //
    // Audio RTD3 is NOT supported on SDS.
    //


// Check HDAS (HD-Audio) controller present
    //
    // Audio RTD3 is disabled by default for SKL SDS to support Low-power Always Listening mode
    //

//
// HDA - end

// Modern Standby N:1 Power Resource definition. Place Holder.
  If (LEqual(\EMOD, 1)) {
    Scope(\_SB){
      Name(GBPS, 0) // Power state flag for Modern Standby. Initial value = 1 (On).
      PowerResource(MODS, 0, 0){                  /// Modern Standby Power Resource

        Method(_STA) {  /// _STA method
          If(LGreaterEqual(OSYS, 2015)){
            Return(GBPS)
          }
          Return(0)
        }

        Method(_ON, 0) {                            /// _ON Method
          If(LGreaterEqual(OSYS, 2015)){

            Store(1, GBPS) // Indicate devices are ON
          }
        }
        Method(_OFF, 0) {                           /// _OFF Method
          If(LGreaterEqual(OSYS, 2015)){

            Store(0, GBPS)
          }
        }
      } // End MODS
    } // \_SB
  } // End If ((LEqual(EMOD, 1))

} // End SSDT
