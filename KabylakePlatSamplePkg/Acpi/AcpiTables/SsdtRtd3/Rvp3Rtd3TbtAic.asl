/** @file
  ACPI RTD3 SSDT table for SKL RVP3

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2019 Intel Corporation.

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
    "RV3Rd3AR",
    0x1000
    )
{
External(RTD3, IntObj)
External(S0ID)
External(OSYS)
External(SGMD)
External(\ECON, IntObj)
External(\WGUR)

// AMI_OVERRIDE_START - Fix Linux build error.
//Include ("Rtd3Common.asl")
Include ("RTD3Common.asl")
// AMI_OVERRIDE_END - Fix Linux build error.

External(\_SB.PCI0.HDAS, DeviceObj)
External(\_SB.PCI0.HDAS.VDID)

External(\_SB.PCI0.XHC.RHUB.HS10, DeviceObj) //xHCI HSP port 1

External(\_SB.PCI0.XHC.RHUB.HS11, DeviceObj) //xHCI HSP port 2
External(\_SB.PCI0.XHC.RHUB.HS03, DeviceObj) //xHCI HSP port 3, WWAN
External(\_SB.PCI0.XHC.RHUB.HS04, DeviceObj) //xHCI HSP port 4
External(\_SB.PCI0.XHC.RHUB.SS03, DeviceObj) //xHCI SSP port 3

External(PEPC)

External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)
External(\_SB.PCI0.I2C1.TPL1._STA, MethodObj)
External(\_SB.PCI0.RP08.PXSX, DeviceObj)
External(\_SB.PCI0.RP08.PXSX.PAHC, MethodObj)
External(\_SB.PCI0.RP08.PXSX.PNVM, MethodObj)
External(\_SB.PCI0.LPCB.H_EC.PVOL)

External(\_SB.PCI0.SAT0.VOL0, DeviceObj)
External(\_SB.PCI0.SAT0.VOL1, DeviceObj)
External(\_SB.PCI0.SAT0.VOL2, DeviceObj)
External(\_SB.PCI0.SAT0.NVM1, DeviceObj)
External(\_SB.PCI0.SAT0.NVM2, DeviceObj)
External(\_SB.PCI0.SAT0.NVM3, DeviceObj)
External(\_SB.PCI0.SAT0.NVM2.RPPC, MethodObj)
External(\_SB.PCI0.SAT0.NVM2.NCRN, MethodObj)
External(\_SB.PCI0.SAT0.PON, MethodObj)
External(\_SB.PCI0.SAT0.POFF, MethodObj)
External(\_SB.PCI0.SAT0.PSTA, MethodObj)

External(\_SB.PCI0.RP09.PXSX.WIST, MethodObj)
External(\_SB.GBTR, MethodObj)
External(\_SB.BTRK, MethodObj)

// PCIe root ports - START
  If(LEqual(\RTBT, 1)) {
    ///
    /// PCIe TBT AR AIC RTD3 - SLOT#1, (x4 port)
    ///
    Scope(\_SB.PCI0.RP01)
    {
      // reset pin = IOEX_P10(0,8)
      // power pin = IOEX_P20(0,16)
      // wake pin  = GPIO_SKL_LP_GPP_B18
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC2 (MSKCRQSRC2):
      Name(SLOT, 1)   ///- port #1
      Name(RSTG, Package() {
              2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              8, // GPIO pad #/IOEX pin #
              0  // reset pin de-assert polarity
            })
      Name(PWRG, Package() {
              2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
             16, // GPIO pad #/IOEX pin #
              0  // power on polarity
            })
      Name(WAKG, Package() {
              1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              GPIO_SKL_LP_GPP_B18, // GPIO pad #/IOEX pin #
              1  // wake en polarity
            })
      Name(SCLK, Package() {
              1,   // 0-disable, 1-enable
              0x4, // MSKCRQSRC2
              0    // assert polarity, ICC should be LOW-activated
            })
      Name(TUID, 0) // TBT RP Number 0 for RP21
      Include("Rtd3SptPcieTbt.asl")
    }
  }

    ///
    /// PCIe RTD3 - SLOT#3
    ///
    Scope(\_SB.PCI0.RP06)
    {
      // reset pin = none
      // power pin = none
      // wake pin  = none
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC1 (MSKCRQSRC1):
      Name(SLOT, 6)   ///- port #6
      Name(RSTG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // reset pin de-assert polarity
            })
      Name(PWRG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // power on polarity
            })
      Name(WAKG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // wake en polarity
            })
      Name(SCLK, Package() {
              1,   // 0-disable, 1-enable
              0x2, // MSKCRQSRC1
              0    // assert polarity, ICC should be LOW-activated
            })
// AMI_OVERRIDE_START - Fix Linux build error.          
//      Include("Rtd3SptPcie.asl")
        Include("RTD3SptPcie.asl")
// AMI_OVERRIDE_END - Fix Linux build error.  
    }

    ///
    /// PCIe RTD3 - m.2 PCIe SSD (default disabled on RVP3)
    ///
    Scope(\_SB.PCI0.RP08)
    {
      // reset pin = none
      // power pin = none
      // wake pin  = none
      // CLK_REQ   = none
      Name(SLOT, 8)   ///- Slot #8
      Name(RSTG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // reset pin de-assert polarity
            })
      Name(PWRG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // power on polarity
            })
      Name(WAKG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // wake en polarity
            })
      Name(SCLK, Package() {
              0, // 0-disable, 1-enable
              0, // MSKCRQSRCx
              0  // assert polarity, ICC should be LOW-activated
            })
// AMI_OVERRIDE_START - Fix Linux build error.          
//      Include("Rtd3SptPcie.asl")
        Include("RTD3SptPcie.asl")
// AMI_OVERRIDE_END - Fix Linux build error.  
    }

    Scope(\_SB.PCI0.RP08.PXSX)
    {
      Name(_S0W,0x04) // PCIe SSD NVMe

      Method(_PR0, Zero, NotSerialized, 0, PkgObj){
        If(LOr(PAHC(), PNVM())){ // Check if PCIe AHCI controller or NVMe
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          }Else{
            If (CondRefOf(\_SB.PCI0.RP08.PXP)) {
              Return(Package(){\_SB.PCI0.RP08.PXP})
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
            If (CondRefOf(\_SB.PCI0.RP08.PXP)) {
              Return(Package(){\_SB.PCI0.RP08.PXP})
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
              If (CondRefOf(\_SB.PCI0.RP08.PXP)) {
                Return(Package(){\_SB.PCI0.RP08.PXP})
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
              If (CondRefOf(\_SB.PCI0.RP08.PXP)) {
                Return(Package(){\_SB.PCI0.RP08.PXP})
              }
            }
           }
           Return(Package(){}) // NVMe mode
        }
      }// Endof MINI
    }

    ///
    /// PCIe RTD3 - m.2 WLAN
    ///
    Scope(\_SB.PCI0.RP09)
    {
      // reset pin = IOEX(0,14)
      // power pin = none
      // wake pin  = IOEX(0,4), native PCIe wake
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC5 (MSKCRQSRC5)
      Name(SLOT, 9)   ///- Slot #9
      Name(RSTG, Package() {
              2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
             14, // GPIO pad #/IOEX pin #
              0  // reset pin de-assert polarity
            })
      Name(PWRG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // power on polarity
            })
      Name(WAKG, Package() {
              2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              4, // GPIO pad #/IOEX pin #
              1  // wake en polarity
            })
      Name(SCLK, Package() {
              1,    // 0-disable, 1-enable
              0x20, // MSKCRQSRC5
              0     // assert polarity, ICC should be LOW-activated
            })
// AMI_OVERRIDE_START - Fix Linux build error.          
//      Include("Rtd3SptPcie.asl")
        Include("RTD3SptPcie.asl")
// AMI_OVERRIDE_END - Fix Linux build error.  
    }

    ///
    /// PCIe RTD3 - m.2 WIGIG
    ///
    Scope(\_SB.PCI0.RP10)
    {
      // reset pin = IOEX(0,3)
      // power pin = none
      // wake pin  = IOEX(0,20), native PCIe wake
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC4 (MSKCRQSRC4)
      Name(SLOT, 10)   ///- port #10
      Name(RSTG, Package() {
              2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              3, // GPIO pad #/IOEX pin #
              0  // reset pin de-assert polarity
            })
      Name(PWRG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              0  // power on polarity
            })
      Name(WAKG, Package() {
              2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
             20, // GPIO pad #/IOEX pin #
              1  // wake en polarity
            })
      Name(SCLK, Package() {
              1,    // 0-disable, 1-enable
              0x10, // MSKCRQSRC4
              0     // assert polarity, ICC should be LOW-activated
            })
// AMI_OVERRIDE_START - Fix Linux build error.          
//      Include("Rtd3SptPcie.asl")
        Include("RTD3SptPcie.asl")
// AMI_OVERRIDE_END - Fix Linux build error.  
    }
// PCIe root ports - END

// SATA - START
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

      OperationRegion(MAPR,PCI_Config,0x90,2)
      Field(MAPR,AnyAcc, NoLock, Preserve) {
        Offset(0x00), // MAP -Address Map Register (SATA-D31:F2)
        , 6,          // [5:0]
        SMSR, 2       // [7:6]SATA Mode Select (SMS), 10b = RAID mode
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
    }
    If(And(RCG1_RTD3_PRT0_ENABLED,RCG1)) {
      Scope(\_SB.PCI0.SAT0.PRT0) {
        /// Define _PR0, _PR3 PowerResource Package
        // P0 command port = ABAR+0x118
        // power pin = (IOEX, 2)
        Name(PORT, 0)
        Name(PBAR, 0x118)       // port0 command port address
        Name(PWRG, Package() {
                    2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                    0, // GPIO group #/IOEX #
                    2, // GPIO pad #/IOEX pin #
                    1  // power on polarity
        })
// AMI_OVERRIDE_START - Fix Linux build error.      
//      Include("Rtd3SptSata.asl")
      Include("RTD3SptSata.asl")          
// AMI_OVERRIDE_END - Fix Linux build error. 
      } // end device(PRT0)
    }
    If(And(RCG1_RTD3_PRT1_ENABLED,RCG1)) {
      Scope(\_SB.PCI0.SAT0.PRT1) {
        /// Define _PR0, _PR3 PowerResource Package
        // P1 command port = ABAR+0x198
        // power pin = (IOEX, 0)
        Name(PORT, 1)
        Name(PBAR, 0x198)       // port0 command port address
        Name(PWRG, Package() {
                    2, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                    0, // GPIO group #/IOEX #
                    0, // GPIO pad #/IOEX pin #
                    1  // power on polarity
        })
// AMI_OVERRIDE_START - Fix Linux build error.      
//      Include("Rtd3SptSata.asl")
      Include("RTD3SptSata.asl")          
// AMI_OVERRIDE_END - Fix Linux build error. 
      } // end device(PRT1)
    }
    If(And(RCG1_RTD3_PRT2_ENABLED,RCG1)) {
      Scope(\_SB.PCI0.SAT0.PRT2) {
        /// Define _PR0, _PR3 PowerResource Package
        // P2 command port = ABAR+0x218
        // power pin = none
        Name(PORT, 2)
        Name(PBAR, 0x218)       // port0 command port address
        Name(PWRG, Package() {
                    0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                    0, // GPIO group #/IOEX #
                    0, // GPIO pad #/IOEX pin #
                    1  // power on polarity
        })
// AMI_OVERRIDE_START - Fix Linux build error.      
//      Include("Rtd3SptSata.asl")
      Include("RTD3SptSata.asl")          
// AMI_OVERRIDE_END - Fix Linux build error. 
      } // end device(PRT2)
    }

    If(And(RCG1_RTD3_NVM2_ENABLED,RCG1)) {
      Scope(\_SB.PCI0.SAT0.NVM2) {
        Include("Rtd3RstRemap.asl")
      }
    }

    If(And(RCG1_RTD3_VOLx_ENABLED,RCG1)) {
      Scope(\_SB.PCI0.SAT0) {

        Include ("RTD3RstRaid.asl")

        // CPON  : Method to turn ON ports per PMAP bitmap status
        //  Arg0 : PMAP - Port Bitmap Status
        Method(CPON,1,Serialized,,,{IntObj}) {
          If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT0.SPON),And(Arg0, RAID_VOL_PRT0_MASK))) { \_SB.PCI0.SAT0.PRT0.SPON() }// Turn On Port 0 Power
          If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT1.SPON),And(Arg0, RAID_VOL_PRT1_MASK))) { \_SB.PCI0.SAT0.PRT1.SPON() }// Turn On Port 1 Power
          If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT2.SPON),And(Arg0, RAID_VOL_PRT2_MASK))) { \_SB.PCI0.SAT0.PRT2.SPON() }// Turn On Port 2 Power
          // Remapped PCIe
          If(LAnd(CondRefOf(\_SB.PCI0.SAT0.NVM2.RPPC),And(Arg0, RAID_VOL_NVM2_MASK))) { \_SB.PCI0.SAT0.NVM2.RPPC(1) }// Turn On CR2 Power
        }

        // CPOF  : Method to turn OFF ports per PMAP bitmap status
        //  Arg0 : PMAP - Port Bitmap Status
        Method(CPOF,1,Serialized,,,{IntObj}) {
          If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT0.SPOF),And(Arg0, RAID_VOL_PRT0_MASK))) { \_SB.PCI0.SAT0.PRT0.SPOF() }// Turn Off Port 0 Power
          If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT1.SPOF),And(Arg0, RAID_VOL_PRT1_MASK))) { \_SB.PCI0.SAT0.PRT1.SPOF() }// Turn Off Port 1 Power
          If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT2.SPOF),And(Arg0, RAID_VOL_PRT2_MASK))) { \_SB.PCI0.SAT0.PRT2.SPOF() }// Turn Off Port 2 Power
          // Remapped PCIe
          If(LAnd(CondRefOf(\_SB.PCI0.SAT0.NVM2.RPPC),And(Arg0, RAID_VOL_NVM2_MASK))) { \_SB.PCI0.SAT0.NVM2.RPPC(0) }// Turn Off CR2 Power
        }
      }
    }


 Scope(\_SB.PCI0.SAT0) {
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
                    \_SB.PCI0.SAT0.PRT0.SPPR._ON() ///- apply power to port 0
                    \_SB.PCI0.SAT0.PRT1.SPPR._ON() ///- apply power to port 1
                    \_SB.PCI0.SAT0.PRT2.SPPR._ON() ///- apply power to port 2
                  }
                }
              } //Switch(DRV)
              Return (0)
            } //Case (2)
            Case (3){ /// Case 3: Current status of Device/Link of Port
              Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
              Switch(ToInteger(DRV)) {  /// Check for SATA port
                Case (0x0000FFFF){  ///- SATA Port 0
                  ///- Bit0 => Device power state
                  If(LEqual(\_SB.PCI0.SAT0.PRT0.SPPR._STA(), 0)){
                    Store(0, Local0)
                  }Else{
                    Store(1, Local0)
                  }
                  Return (Local0)
                }
                Case (0x0001FFFF){  ///- SATA Port 1
                  ///- Bit0 => Device power state
                  If(LEqual(\_SB.PCI0.SAT0.PRT1.SPPR._STA(), 0)){
                    Store(0, Local0)
                  }Else{
                    Store(1, Local0)
                  }
                  Return (Local0)
                }
                Case (0x0002FFFF){  ///- SATA Port 2
                  ///- Bit0 => Device power state
                  If(LEqual(\_SB.PCI0.SAT0.PRT2.SPPR._STA(), 0)){
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
// SATA - END

// USB - START
    Scope(\_SB.PCI0.XHC){

      /// USPP: Both HS/SS Ports are disconnected=1, else=0, updated inside PS2X/PS0X method call \n
      /// Bit 0 : USB SD Card Reader \n
      /// Bit 1 : USB(HS1 : 0x480 / SS0 : 0x510) \n
      /// BIT 2 : USB(HS2 : 0x490 / SS1 : 0x520) \n
      /// UPWR: Control power status 1=OFF, 0=ON \n
      Name(UPWR,0)
      Name(USPP,0)

      /// @defgroup xhc_duam        XHCI Device User Absent Mode (DUAM) method
      Method(DUAM,0,Serialized)
      {
        OperationRegion (XHCM, SystemMemory, And(MEMB,0xFFFF0000), 0x600)
        Field (XHCM, DWordAcc, NoLock, Preserve)
        {
          Offset(0x2),
          XHCV,16,
          Offset(0x480),
          HP01, 1, // HS port 1
          Offset(0x490),
          HP02, 1, // HS port 2
          Offset(0x530),
          SP00, 1, // SS port 0
          Offset(0x540),
          SP01, 1, // SS port 1
        }

      if(LNotEqual(\UAMS,0x00)) /// if User not present
      {
        Store(0x00,\_SB.PCI0.XHC.UPWR)
        /// Enterng CS, Remove power if SD card not present

        If(LEqual(XHCV,0xFFFF)) ///- If Controller in D3Hot(MEM_BASE not decoded), remove power base on USPP flags
        {
          ///- >>check USPP BIT0:2 to determine power off criteria
          ///- >>if criteria met, turn off power and update UPWR

          If(And(\_SB.PCI0.XHC.USPP,0x02)) // USB(HS1 : 0x480 /SS0 : 0x530)
          {
            // drive pwr pin low
            \_SB.PCI0.GEXP.SGEP(1, 3, 0)
            Or(\_SB.PCI0.XHC.UPWR,0x02, \_SB.PCI0.XHC.UPWR)
          }
          If(And(\_SB.PCI0.XHC.USPP,0x04)) // USB(HS2 : 0x490 / SS1 : 0x520)
          {
            // drive pwr low
            \_SB.PCI0.GEXP.SGEP(1, 4, 0)
            Or(\_SB.PCI0.XHC.UPWR,0x04, \_SB.PCI0.XHC.UPWR)
          }
        ///- If Controller is not in D3hot (XHCI BAR is decoded)
        }Else{ //    If(Lequal(XHCV,0xFFFF))

          ///- >>check currect port connect status to determine power off criteria
          ///- >>if criteria met, turn off power and update UPWR

          If(LAnd(LEqual(HP01,0x00),LEqual(SP00,0x00))) // USB(HS1 : 0x480 /SS0 : 0x510)
          {
            // drive pwr pin low
            \_SB.PCI0.GEXP.SGEP(1, 3, 0)
            Or(\_SB.PCI0.XHC.UPWR,0x02, \_SB.PCI0.XHC.UPWR)
          }
          If(LAnd(LEqual(HP02,0x00),LEqual(SP01,0x00))) // USB(HS2 : 0x490 / SS1 : 0x520)
          {
            // drive pwr low
            \_SB.PCI0.GEXP.SGEP(1, 4, 0)
            Or(\_SB.PCI0.XHC.UPWR,0x04, \_SB.PCI0.XHC.UPWR)
          }
        }//    If(Lequal(XHCV,0xFFFF))
      }Else{ //if(LNotEqual(\UAMS,0x00))

        /// Exiting CS , Apply power if power removed
        if(And(\_SB.PCI0.XHC.UPWR,0x02)) // USB(HS1 : 0x480 /SS0 : 0x510)
        {
          // drive pwr pin high
          \_SB.PCI0.GEXP.SGEP(1, 3, 1)
        }
        if(And(\_SB.PCI0.XHC.UPWR,0x04)) // USB(HS2 : 0x490 / SS1 : 0x520)
        {
          // drive pwr high
          \_SB.PCI0.GEXP.SGEP(1, 4, 1)
        }
      } //if(LNotEqual(\UAMS,0x00)) // User not present

        /// @addtogroup xhc_duam
      } // End of DUAM
    }

    Scope(\_SB.PCI0.XHC.RHUB){ //USB XHCI RHUB
      /// PS0X method is called by PS0 method in PchXhci.asl
      Method(PS0X,0,Serialized)
      {
        Store(0x00,\_SB.PCI0.XHC.USPP)  ///- clear all bits in USPP
      }

      /// PS2X method is called by PS2 method in PchXhci.asl
      /// It updates USPP variable base on USB port current connect status
      Method(PS2X,0,Serialized)
      {
        OperationRegion (XHCM, SystemMemory, And(MEMB,0xFFFFFFFFFFFF0000), 0x600)
        Field (XHCM, DWordAcc, NoLock, Preserve)
        {
          Offset(0x2),
          XHCV,16,
          Offset(0x480),
          HP01, 1, // HS port 1
          Offset(0x490),
          HP02, 1, // HS port 2
          Offset(0x530),
          SP00, 1, // SS port 0
          Offset(0x540),
          SP01, 1, // SS port 1
        }

        If(LEqual(XHCV,0xFFFF)) /// Check if Controller in D3Hot(MEM_BASE not decoded)
        {
          Return() ///- if yes, no MMIO access, do nothing and returns
        }
        If(LAnd(LEqual(HP01,0x00),LEqual(SP00,0x00))) /// Check USB ports (HS1 : 0x480 /SS0 : 0x510) (0: not connected)
        {
          Or(\_SB.PCI0.XHC.USPP,0x02, \_SB.PCI0.XHC.USPP)  ///- Set USPP BIT1
        }
        If(LAnd(LEqual(HP02,0x00),LEqual(SP01,0x00))) /// Check USB ports (HS2 : 0x490 / SS1 : 0x520) (0: not connected)
        {
          Or(\_SB.PCI0.XHC.USPP,0x04, \_SB.PCI0.XHC.USPP) ///- Set USPP BIT2
        }
      } // End of PS2 method
      ///@defgroup xhc_rhub_ps2x XHCI RHUB PS2X Method

      /// PS3X method is called by _PS3 method in PchXhci.asl
      Method(PS3X,0,Serialized)
      {
      } // End of PS3 method
      ///@defgroup xhc_rhub_ps3x XHCI RHUB PS3X Method
    }

    If(LNotEqual(And(XHPR,0xF),0x0)){
      Scope(\_SB.PCI0.XHC.RHUB) {
        PowerResource(PX01, 0, 0){ /// power rail for USB3.0 ports 1 - (IOEX2, 3)
          Method(_STA){ /// _STA Method
            If(LEqual(\_SB.PCI0.GEXP.GEPS(1, 3),1)) { ///- Get power status base on GPIO13
              Return(0x01)
            } Else {
              Return(0x00)
            }
          }

          Method(_ON, 0)    /// _ON Method
          { /// Turn on
            // drive pwr pin high
            \_SB.PCI0.GEXP.SGEP(1, 3, 1)
          }

          Method(_OFF, 0)   /// _OFF Method
          { /// Turn off
            // drive pwr pin low
            \_SB.PCI0.GEXP.SGEP(1, 3, 0)
          }
          ///@defgroup xhc_pr_px01 XHCI Power Resource PX01
        } // End PX01
      } // End \_SB.PCI0.XHC.RHUB
      }

    If(LNotEqual(And(XHPR,0xF0),0x0)){
      Scope(\_SB.PCI0.XHC.RHUB) {
        PowerResource(PX02, 0, 0){ /// Power rail for USB3.0 ports 2&3&5 - (IOEX2, 4)
          Method(_STA){     /// _STA method
            If(LEqual(\_SB.PCI0.GEXP.GEPS(1, 4),1)) { ///- Get power status based on GPIO43
              Return(0x01)
            } Else {
              Return(0x00)
            }
          }
          Method(_ON, 0)    /// _ON Method
          {
            // drive pwr high
            \_SB.PCI0.GEXP.SGEP(1, 4, 1)
          }

          Method(_OFF, 0)   /// _OFF Method
          {
            // drive pwr low
            \_SB.PCI0.GEXP.SGEP(1, 4, 0)
          }
        } // End PX02
      }
    }

    Include ("Rtd3Camera.asl")

    If(LEqual(And(RCG0,2),2)) {
      Scope(\_SB.PCI0.XHC.RHUB.SS03)        // USB Superspeed Port 3 - IVCAM
      {
        Name(_S0W, 4)
        Method(_PR0) {
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          }Else{
             Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
           }
           Return(Package(){})
        }

        Method(_PR2) {
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          }Else{
            Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
          }
          Return(Package(){})
        }

        Method(_PR3) {
          If (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
            If (CondRefOf(\_SB.MODS)) {
              Return(Package(){\_SB.MODS})
            }
          }Else{
            Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
          }
          Return(Package(){})
        }
      } // Scope(\_SB.PCI0.XHC.RHUB.SS03)
    } // If(LEqual(And(RCG0,2),2))

    ///
    /// If WWAN RTD3 support is enabled, associate _PR0, _PR2, _PR3 for USB Highspeed Port 3 with \ref xhc_pr_px03
    ///@addtogroup xhc_scope
    If(LEqual(And(RCG0,0x80),0x80)) {
      Scope(\_SB.PCI0.XHC.RHUB){
        // USB power resource 5
        // power pin = IOEX2,P00
        // wake pin = GPIO_SKL_LP_GPP_B3 !! NOT use this pin, it's for touch pad now
        // sim card detect pin = GPIO_SKL_LP_GPP_F23, acpi handle or driver handle??
        PowerResource(PX05, 0, 0){
          Name(WOFF, 0)  // Last OFF Timestamp (WOFF): The time stamp of the last power resource _OFF method evaluation
          Name(TIM1, 0)
          Method(_STA){                                         /// _STA method
            If(LEqual(\_SB.PCI0.GEXP.GEPS(1, 0),1)) {
              Return(0x01)
            } Else {
              Return(0x00)
            }
          }
          Method(_ON, 0)                                        /// _ON Method
          {
            If(LNotEqual(^WOFF, Zero)) {
              Divide(Subtract(Timer(), ^WOFF), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
              If(LLess(Local0,600)) {                           // If Elapsed time is less than 600ms
                Sleep(Subtract(600,Local0))                     // Sleep for the remaining time
              }
              Store(0, ^WOFF)
            }

            Store(Timer(), ^TIM1)                               // Start OFF timer here.
            // drive RESET Pin high (GPP_A_12)
            \_SB.SGOV(\WGUR, 1)

            Divide(Subtract(Timer(), ^TIM1), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
            If(LLess(Local0,2)) {                           // If Elapsed time is less than 2ms
              Sleep(Subtract(2,Local0))                     // Sleep for the remaining time
            }

            \_SB.PCI0.GEXP.SGEP(1, 0, 1)                        // set power pin to high
          }
          Method(_OFF, 0)                                       /// _OFF Method
          {
            Store(Timer(), ^TIM1)                               // Start OFF timer here.

            // drive RESET Pin low (GPP_A_12)
            \_SB.SGOV(\WGUR, 0)

            Divide(Subtract(Timer(), ^TIM1), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
            If(LLess(Local0,2)) {                           // If Elapsed time is less than 2ms
              Sleep(Subtract(2,Local0))                     // Sleep for the remaining time
            }

            \_SB.PCI0.GEXP.SGEP(1, 0, 0)                        // set power pin to low
            Store(Timer(), ^WOFF)                               // Start OFF timer here.
          }
        } // End PX05
      } // End \_SB.PCI0.XHC.RHUB for WWAN

      Scope(\_SB.PCI0.XHC.RHUB.HS03){
        If (\_SB.PCI0.RP09.PXSX.WIST ()) {
          PowerResource (DBTP, 0, 0)
          {
            Method(_STA, 0)
            {
              If(LEqual (\_SB.GBTR (), 1)) {
                Return (1)
              } Else {
                Return (0)
              }
            }
            Method(_ON, 0, Serialized) { // Turn on BT Core
              \_SB.BTRK (1)
            }
            Method(_OFF, 0, Serialized) { // Turn off BT Core
              \_SB.BTRK (0)
            }
          }
        }

        Name(_S0W,2)                                //  Indicate WWAN can wake from D2 while in S0 using selective suspend

        Method (_PR0) { // Power Resource required to support D0
          If(\_SB.PCI0.RP09.PXSX.WIST ()){
            Return(Package(){DBTP})
          }
          Return(Package(){\_SB.PCI0.XHC.RHUB.PX05})
        }
        Method (_PR2) { // Power Resource required to support D2
          If(\_SB.PCI0.RP09.PXSX.WIST ()){
            Return(Package(){DBTP})
          }
          Return(Package(){\_SB.PCI0.XHC.RHUB.PX05})
        }
        Method (_PR3) { // Power Resource required to support D3
          If(\_SB.PCI0.RP09.PXSX.WIST ()){
            Return(Package(){DBTP})
          }
          Return(Package(){\_SB.PCI0.XHC.RHUB.PX05})
        }

        If (LEqual(\_SB.PCI0.RP09.PXSX.WIST (), 0)) {
          //
          // WWAN Modem device with the same power resource as its composite parent device
          //
          Device (MODM) {
            Name(_ADR, 0x5)
            Name(_PR0,Package(){\_SB.PCI0.XHC.RHUB.PX05})               // Power Resource required to support D0
            Name(_PR2,Package(){\_SB.PCI0.XHC.RHUB.PX05})               // Power Resource required to support D2
            Name(_PR3,Package(){\_SB.PCI0.XHC.RHUB.PX05})               // Power Resource required to support D3
          }
        }
      } // Scope(\_SB.PCI0.XHC.RHUB.HS03)

      Scope(\_GPE) {
      //    Method(_L36) {
      //      //
      //      // WWAN SIM Detect Wake
      //      //
      //      //@todo platform:Store(1, \GO54)                           //  GPIO_OWN to GPIO Driver to stop SCI flooding
      //      Notify(\_SB.PCI0.XHC.RHUB.HS03, 0x02)     //  DeviceWake: Notify PCIe RootPort
      //    }
      }
    } // If(LEqual(And(RCG0,0x80),0x80))
    /// Associate _PR0, _PR2, _PR3 for USB Highspeed Port 1/2 (HS01/HS02) and SuperSpeed Port 1 (SS01/SS02)
    /// with \ref xhc_pr_px01 and \ref xhc_pr_px02
    ///@addtogroup xhc_scope

    If(LEqual(And(XHPR,0xF),0x01))
    {
      Scope(\_SB.PCI0.XHC.RHUB.HS01){
        Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX01})
        Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX01})
        Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX01})
      }
    }
    If(LEqual(And(XHPR,0xF0),0x10)){
      Scope(\_SB.PCI0.XHC.RHUB.HS02){
        Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX02})
        Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX02})
        Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX02})
      }
    }
    If(LEqual(And(XHPR,0xF),0x02)){
      Scope(\_SB.PCI0.XHC.RHUB.SS01){
        Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX01})
        Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX01})
        Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX01})
      } //  End of Scope(\_SB.PCI0.XHC.RHUB.SS01)
    }

    If (LNotEqual(And(XHPR,0x60),0x0)) {
      Scope (\_SB.PCI0.XHC.RHUB.SS02) {

        Method (_PR0, Zero, NotSerialized, 0, PkgObj) {
          If (LEqual(And(XHPR, 0x60), 0x20)) { // External SS Port2
            Return (Package(){\_SB.PCI0.XHC.RHUB.PX02})
          } Else { // Internal SS WWAN through SSIC
            If (LEqual(And(RCG0, 0x80), 0x80)) { // WWAN RTD3 is supported.
              Return (Package(){\_SB.PCI0.XHC.RHUB.PX05})
            }
          }
          Return(Package(){})
        }
        Method (_PR2, Zero, NotSerialized, 0, PkgObj) {
          If (LEqual(And(XHPR, 0x60), 0x20)) { // External SS Port2
            Return (Package(){\_SB.PCI0.XHC.RHUB.PX02})
          } Else { // Internal SS WWAN through SSIC
            If (LEqual(And(RCG0, 0x80), 0x80)) { // WWAN RTD3 is supported.
              Return (Package(){\_SB.PCI0.XHC.RHUB.PX05})
            }
          }
          Return(Package(){})
        }
        Method (_PR3, Zero, NotSerialized, 0, PkgObj) {
          If (LEqual(And(XHPR, 0x60), 0x20)) { // External SS Port2
            Return (Package(){\_SB.PCI0.XHC.RHUB.PX02})
          } Else { // Internal SS WWAN through SSIC
            If (LEqual(And(RCG0, 0x80), 0x80)) { // WWAN RTD3 is supported.
              Return (Package(){\_SB.PCI0.XHC.RHUB.PX05})
            }
          }
          Return(Package(){})
        }

        Method (_S0W) { // Only applicable for WWAN
          If (LAnd(LEqual(And(RCG0, 0x80), 0x80), LEqual(And(XHPR, 0xF0), 0x40))){
            Return(2) // Can wake from D2 while in S0.
          } Else {
            Return(0) // No WWAN
          }
        }

      } // Scope(\_SB.PCI0.XHC.RHUB.SS02)
    } // End If (LNotEqual(And(XHPR,0x60),0x0))

    If (LAnd(LEqual(And(XHPR,0x40),0x40),LEqual(And(RCG0,0x80), 0x80))) {
      Scope(\_SB.PCI0.XHC.RHUB.SS02){
        //
        // WWAN Modem device with the same power resource as its composite parent device
        //
        Device (MODM) {
          Name(_ADR, 0x5)
          Name(_PR0,Package(){\_SB.PCI0.XHC.RHUB.PX05})               // Power Resource required to support D0
          Name(_PR2,Package(){\_SB.PCI0.XHC.RHUB.PX05})               // Power Resource required to support D2
          Name(_PR3,Package(){\_SB.PCI0.XHC.RHUB.PX05})               // Power Resource required to support D3
        }
      } // End Scope(\_SB.PCI0.XHC.RHUB.SS02) for WWAN child device.
    } // If (LAnd(LEqual(And(XHPR,0x40),0x40),LEqual(And(RCG0,0x80), 0x80)))

    // USB - END

    // XDCI - start
    Scope(\_SB)
    {
      //
      // Dummy power resource for USB D3 cold support
      //
      PowerResource(USBC, 0, 0)
      {
        Method(_STA) { Return (0xF) }
        Method(_ON) {}
        Method(_OFF) {}
      }
    }

    Scope(\_SB.PCI0.XDCI)
    {
      OperationRegion (GENR, SystemMemory, Add(And(XDCB, 0xFF000000), 0x10F81C), 0x4)  //AON MMIO - 10F81C: APBFC_U3PMU_CFG5
      Field (GENR, WordAcc, NoLock, Preserve)
      {
            ,   2,
        CPME,   1,    //bit2 core_pme_en
        U3EN,   1,    //bit3 u3_pme_en
        U2EN,   1     //bit4 u2_pme_en
      }

      Method (_PS3, 0, NotSerialized)
      {
        Store (One, CPME)
        Store (One, U2EN)
        Store (One, U3EN)
      }
      Method (_PS0, 0, NotSerialized)
      {
        Store (Zero, CPME)
        Store (Zero, U2EN)
        Store (Zero, U3EN)
      }

      Method (_RMV, 0, NotSerialized)  // _RMV: Removal Status
      {
        Return (Zero)
      }

      Method (_PR3, 0, NotSerialized)  // _PR3: Power Resources for D3hot
      {
        Return (Package (0x01)
        {
          USBC // return dummy package
        })
      }
    } // Scope(\_SB.PCI0.XDCI)
// XDCI - end

Scope(\_SB.PCI0.GLAN)
{
  Method (_PS3, 0, NotSerialized)
  {
  }
  Method (_PS0, 0, NotSerialized)
  {
  }
} // Scope(\_SB.PCI0.GLAN)


// Serial IO - START
    //Power Resource for Audio Codec
    Scope(\_SB.PCI0)
    {
      PowerResource(PAUD, 0, 0) {

        /// Namespace variable used:
        Name(PSTA, One) /// PSTA: Physical Power Status of Codec 0 - OFF; 1-ON
        Name(ONTM, Zero) /// ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON

        Name(_STA, One) /// _STA: PowerResource Logical Status 0 - OFF; 1-ON

        ///@defgroup pr_paud Power Resource for onboard Audio CODEC

        Method(_ON, 0){     /// _ON method \n
          Store(One, _STA)        ///- Set Logocal power state
          PUAM() ///- Call PUAM() to tansition Physical state to match current logical state
                    ///@addtogroup pr_paud
        } // End _ON

        Method(_OFF, 0){    /// _OFF method \n
          Store(Zero, _STA)    ///- Set the current power state
          PUAM() ///- Call PUAM() to tansition Physical state to match current logical state
        ///@addtogroup pr_paud
        } // End _OFF

        ///  PUAM - Power Resource User Absent Mode for onboard Audio CODEC
        ///  Arguments:
        ///
        ///  Uses:
        ///      _STA - Variable updated by Power Resource _ON/_OFF methods \n
        ///      \\UAMS - Variable updated by GUAM method to show User absent present \n
        ///      ONTM - Local variable to store ON time during Speculative ON \n
        /// ______________________________
        // |  Inputs      |   Outputs    |
        // ______________________________
        // | _STA | \UAMS | GPIO | ONTM |
        // ______________________________
        // |   1  |   0   | ON   |   !0 |
        // |   1  |   !0  | ON   |   !0 |
        // |   0  |   0   | ON   |   !0 |
        // |   0  |   !0  | OFF  |   0  |
        // ______________________________
                    /**
                    <table>
                    <tr> <th colspan="2"> Inputs <th colspan="2"> Output
                    <tr> <th>_STA <th> \\UAMS <th> GPIO <th>ONTM
                    <tr> <td>1 <td>0 <td>ON <td>!0
                    <tr> <td>1 <td>!0<td>ON <td>!0
                    <tr> <td>0 <td>0 <td>ON <td>!0
                    <tr> <td>0 <td>!0<td>OFF<td> 0
                    </table>
                    **/
        ///@addtogroup pr_paud_puam
        Method(PUAM, 0, Serialized)
        {
          // power rail = (IOEX2, 5)
          If (LAnd(LEqual(^_STA, Zero), LNotEqual(\UAMS, Zero))) { ///New state = OFF Check if (_STA ==0 && \UAMS != 0) \n
            // skip below if Power Resource is already in OFF \n
            If(LEqual(\_SB.PCI0.GEXP.GEPS(1, 5), 1)) {
              // drive pwr low
              \_SB.PCI0.GEXP.SGEP(1, 5, 0)
              Store(Zero, ^PSTA)  ///- >> Clear PSTA
              Store(Zero, ^ONTM)  ///- >> Clear ONTM
            }
          } Else { /// New state = ON (_STA=1) or (_STA=0 and \UAMS=0)
            /// Turn power on \n
            If(LNotEqual(^PSTA, One)) { ///- Skip below if Power Resource is already in ON
              // drive pwr high
              \_SB.PCI0.GEXP.SGEP(1, 5, 1)
              Store(One, ^PSTA)  ///- >> Set PSTA to 1
              Store(Timer(), ^ONTM) ///- >> Start the timer for this PR
            }
          }
        ///@defgroup pr_paud_puam Power Resource User Absent Mode for onboard Audio CODEC
        } //PUAM
      } //PAUD
    } //Scope(\_SB.PCI0)
// Check HDAS (HD-Audio) controller present
    If(LNotEqual(\_SB.PCI0.HDAS.VDID, 0xFFFFFFFF)) {
      Scope(\_SB.PCI0.HDAS) {
        Method(PS0X,0,Serialized)     /// Platform D0 Method for HD-A Controller
        {
          If(LEqual(\_SB.PCI0.PAUD.ONTM, Zero)){    ///- Check if ONTM=0
            Return()
          }

          ///
          ///- Make sure "D0 delay" (AUDD) delay is elapsed before returning _PS0
          ///- Local0: Elapse time since the _ON method
          ///- VRRD: VR Rampup Delay
          ///- AUDD: Time required for device to be ready after power on
          ///- Local1 = AUDD + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
          ///- So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
          ///
          Divide(Subtract(Timer(), \_SB.PCI0.PAUD.ONTM), 10000, , Local0) ///- Store Elapsed time in ms, ignore remainder
          Add(AUDD, VRRD, Local1) ///- Incorporate VR Rampup Delay
          If(LLess(Local0, Local1)) { ///- Do not sleep if already past the delay requirement audio
            ///- Delay for device init
            Sleep(Subtract(Local1, Local0)) ///- Sleep (AUDD + VRRD - time elapsed)
          }
        }

        ///Associate _PR0 with \ref pr_paud
        Name(_PR0, Package(){\_SB.PCI0.PAUD})
      ///@defgroup hdef_scope       Intel High Definition Audio Scope
      }
    }// If(LNotEqual(\_SB.PCI0.HDAS.VDID, 0xFFFFFFFF))
  // PCH I2C1 - TouchPanel Power control
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

      PowerResource(PXTC, 0, 0){ /// Power rail for Touch Panel (GPP, B, 14)
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
        If(LEqual(\_SB.GGOV(GPIO_SKL_LP_GPP_B14),1)) { ///- Check GPIO89 NOTE: 1=> ON; 0=> OFF
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(PON, 0) /// _ON Method \n Turn on
      {
        // De-Assert GPIO RST (GPP_E_23)
        \_SB.SGOV(GPIO_SKL_LP_GPP_E23, 1)
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
        // Assert GPIO RST (GPP_E_23)
        \_SB.SGOV(GPIO_SKL_LP_GPP_E23, 0)
        // update ONTM
        Store(Zero , ONTM)  ///- Clear ONTM
      }

      Scope(TPL1){
        Name (TD_P, Package(){})                                  // TD_P - Touch Device Power Package
        Name (TD_X, Package(){\_SB.PCI0.I2C1.PXTC})               // TD_X - Touch Device Power Package
        Name (TD_Z, Package(){\_SB.MODS})                         // TD_Z - Touch Device Power Package

        Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
        Alias(\_SB.PCI0.I2C1.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

        Include("Rtd3I2cTouchDev.asl")
        Method(_PS0) { PS0X() }
        Method(_PS3) { PS3X() }
      }// End Of Scope(TPL1)


    }//  Scope(\_SB.PCI0.I2C1)
//Serial IO End

//GPE Event handling - Start
  Scope(\_GPE) {
    //
    // Alternate _L6F(), to handle 2-tier RTD3 GPE events here
    //
    Method(AL6F) {
      If(LNotEqual(SGMD,2)) {
        // SLOT1 TBT AR wake event
        If (\_SB.ISME(GPIO_SKL_LP_GPP_B18))
        {
          \_SB.SHPO(GPIO_SKL_LP_GPP_B18, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
          Notify(\_SB.PCI0.RP01, 0x02)   // device wake
          \_SB.CAGS(GPIO_SKL_LP_GPP_B18)
        }
      }
      // WLAN wake event
      If (\_SB.ISME(GPIO_SKL_LP_GPP_B16))
      {
        \_SB.SHPO(GPIO_SKL_LP_GPP_B16, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PCI0.RP09, 0x02)   // device wake
        \_SB.CAGS(GPIO_SKL_LP_GPP_B16)
      }
    }  // Method AL6F

    // TBNF is used when PEP _DSM Screen ON is called. This is needed to wake up TBT attached Monitors.
    Method (TBNF) {
      Notify(\_SB.PCI0.RP01, 0x02)   // device wake
    } // Method TBNF

  } //Scope(\_GPE)
//GPE Event handling - End

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
            // USB Camera

            If (And(RCG0, 0x02)) { // USB Camera RTD3 is enabled.
              If (LEqual(\_SB.PCI0.XHC.RHUB.CMSA(), 0)) {
                \_SB.PCI0.XHC.RHUB.CMON()
              }
            }

            \_SB.PCI0.SAT0.PRT0.SPON()
            \_SB.PCI0.SAT0.PRT1.SPON()
            \_SB.PCI0.SAT0.PRT2.SPON()

            \_SB.PCI0.I2C1.PON()

            Store(1, GBPS) // Indicate devices are ON
          }
        }

        Method(_OFF, 0) {                           /// _OFF Method
          If(LGreaterEqual(OSYS, 2015)){

            // USB Camera
            If (And(RCG0, 0x02)) { // USB Camera RTD3 is enabled.
              If (LEqual(\_SB.PCI0.XHC.RHUB.CMSA(), 1)){
                \_SB.PCI0.XHC.RHUB.COFF()
              }
            }

            \_SB.PCI0.SAT0.PRT0.SPOF()
            \_SB.PCI0.SAT0.PRT1.SPOF()
            \_SB.PCI0.SAT0.PRT2.SPOF()

            \_SB.PCI0.I2C1.POFF()

            Store(0, GBPS)
          }
        }
      } // End MODS
    } // \_SB
  } // End If ((LEqual(EMOD, 1))
} // End SSDT
