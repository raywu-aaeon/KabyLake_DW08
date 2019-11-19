/** @file
  ACPI RTD3 SSDT table for SKL RVP11

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2017 Intel Corporation.

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
    "RV11Rtd3",
    0x1000
    )
{
External(RTD3, IntObj)
External(S0ID)
External(OSYS)
External(SPTH)
External(\PAPE)
External(\PSTW)
External(\UP8P)
External(\TBTE)
External(PCHV, MethodObj)

// AMI_OVERRIDE_START - Fix Linux build error.
//Include ("Rtd3Common.asl")
Include ("RTD3Common.asl")
// AMI_OVERRIDE_END - Fix Linux build error.

External(\_SB.PCI0.HDAS, DeviceObj)
External(\_SB.PCI0.HDAS.VDID)

External(\_SB.PCI0.XHC.RHUB.HS08, DeviceObj) //xHCI HSP port 8
External(\_SB.PCI0.XHC.RHUB.HS09, DeviceObj) //xHCI HSP port 9
External(\_SB.PCI0.XHC.RHUB.HS10, DeviceObj) //xHCI HSP port 10
External(\_SB.PCI0.XHC.RHUB.HS11, DeviceObj) //xHCI HSP port 11
External(\_SB.PCI0.XHC.RHUB.HS12, DeviceObj) //xHCI HSP port 12
External(\_SB.PCI0.XHC.RHUB.HS13, DeviceObj) //xHCI HSP port 13
External(\_SB.PCI0.XHC.RHUB.HS14, DeviceObj) //xHCI HSP port 14
External(\_SB.PCI0.XHC.RHUB.SS04, DeviceObj) //xHCI SSP port 4
External(\_SB.PCI0.XHC.RHUB.SS05, DeviceObj) //xHCI SSP port 5

External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)
External(\_SB.PCI0.I2C0.TPD0._STA, MethodObj)
External(\_SB.PCI0.I2C1.TPL1._STA, MethodObj)

External(\_SB.PCI0.RP13.PXSX, DeviceObj)
External(\_SB.PCI0.RP13.PXSX.PAHC, MethodObj)
External(\_SB.PCI0.RP13.PXSX.PNVM, MethodObj)
External(\_SB.PCI0.RP17.PXSX, DeviceObj)
External(\_SB.PCI0.RP17.PXSX.PAHC, MethodObj)
External(\_SB.PCI0.RP17.PXSX.PNVM, MethodObj)

External(\_SB.PCI0.SAT0.VOL0, DeviceObj)
External(\_SB.PCI0.SAT0.VOL1, DeviceObj)
External(\_SB.PCI0.SAT0.VOL2, DeviceObj)
External(\_SB.PCI0.SAT0.NVM1, DeviceObj)
External(\_SB.PCI0.SAT0.NVM2, DeviceObj)
External(\_SB.PCI0.SAT0.NVM3, DeviceObj)
External(\_SB.PCI0.SAT0.NVM1.NCRN, MethodObj)
External(\_SB.PCI0.SAT0.NVM2.NCRN, MethodObj)
External(\_SB.PCI0.SAT0.NVM3.NCRN, MethodObj)

External(\_SB.PCI0.SAT0.PON, MethodObj)
External(\_SB.PCI0.SAT0.POFF, MethodObj)
External(\_SB.PCI0.SAT0.PSTA, MethodObj)
External(\_SB.PCI0.SAT0.NVM1.RPPC, MethodObj)
External(\_SB.PCI0.SAT0.NVM2.RPPC, MethodObj)
External(\_SB.PCI0.SAT0.NVM3.RPPC, MethodObj)

// PCIe root ports - START

//
// For Root Port 9 that has TBT connected and enabled, RTD3 is not supported, so suppress RP09 RTD3.
//
// AMI_OVERRIDE_START >>> AMI TBT has Disable and Fully disable options.
  If(LNotEqual(\TBTE, 1)) {
// AMI_OVERRIDE_END <<< AMI TBT has Disable and Fully disable options.
    ///
    /// PCIe RTD3 - SLOT#2, (x4 port)
    ///
    Scope(\_SB.PCI0.RP09)
    {
      // reset pin = GPIO_SKL_H_GPP_F4, high active
      // power pin = none
      // wake pin  = GPIO_SKL_H_GPP_G18 for SPT-H and GPIO_SKL_H_GPP_B15 for uSFF, high active
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC4 (MSKCRQSRC4)
       Name(SLOT, 9)   ///- RP #9
       Name(RSTG, Package() {
              1,                         // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                         // IOEX #
              GPIO_SKL_H_GPP_F4,         // GPIO pad #/IOEX pin # //RVP11 specific GPIO, Need to port proper Pin for uSFF
              0      // reset pin de-assert polarity
       })
       Name(PWRG, Package() {
              1,  // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,  // IOEX #
              GPIO_SKL_H_GPP_B18,  // GPIO pad #/IOEX pin #
              0   // power on polarity
       })
      Name(WAKG, Package() {
              1,                         // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                         // IOEX #
              GPIO_SKL_H_GPP_G18,        // GPIO pad #/IOEX pin #
              1         // wake en polarity
      })
      Name(SCLK, Package() {
            1,     // 0-disable, 1-enable
            0x10,  // MSKCRQSRC4
            0      // assert polarity, ICC should be LOW-activated
      })
// AMI_OVERRIDE_START - Fix Linux build error.          
//      Include("Rtd3SptPcie.asl")
      Include("RTD3SptPcie.asl")
// AMI_OVERRIDE_END - Fix Linux build error.
 }

    }

    ///
    /// PCIe RTD3 - m.2 PCIe SSD1
    ///
    Scope(\_SB.PCI0.RP13)
    {
      // reset pin = GPIO_SKL_H_GPP_F17, high active
      // power pin = GPIO_SKL_H_GPP_G8, high active
      // wake pin  = none
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC5 (MSKCRQSRC5)
      Name(SLOT, 13)   ///- RP #13
      Name(RSTG, Package() {
              1,                  // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                  // IOEX #
              GPIO_SKL_H_GPP_F17, // GPIO pad #/IOEX pin #
              0            })     // reset pin de-assert polarity
      Name(PWRG, Package() {
              1,                  // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                  // IOEX #
              GPIO_SKL_H_GPP_G8,  // GPIO pad #/IOEX pin #
              1            })     // power on polarity
      Name(WAKG, Package() {
              0,                  // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                  // IOEX #
              0,                  // GPIO pad #/IOEX pin #
              0            })     // wake en polarity
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

    Scope(\_SB.PCI0.RP13.PXSX)
    {
      Name(_S0W,0x04) // PCIe SSD NVMe

      Method(_PR0, Zero, NotSerialized, 0, PkgObj){
        If(LOr(PAHC(), PNVM())){ // Check if PCIe AHCI controller or NVMe
          Return(Package(){\_SB.PCI0.RP13.PXP})
        }Else{
          Return(Package(){})
        }
      }

      Method(_PR3, Zero, NotSerialized, 0, PkgObj){
        If(LOr(PAHC(), PNVM())){ // Check if PCIe AHCI controller or NVMe
          Return(Package(){\_SB.PCI0.RP13.PXP})
        }Else{
          Return(Package(){})
        }
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
            Return(Package(){\_SB.PCI0.RP13.PXP}) // AHCI controller mode
          }Else{
            Return(Package(){}) // NVMe mode
          }
        }

        Method(_PR3, Zero, NotSerialized, 0, PkgObj){
          If(PAHC()){ // Check if PCIe AHCI controller
            Return(Package(){\_SB.PCI0.RP13.PXP}) // AHCI controller mode
          }Else{
            Return(Package(){}) // NVMe mode
          }
        }
      }// Endof MINI
    }

    ///
    /// PCIe RTD3 - m.2 PCIe SSD2
    ///
    Scope(\_SB.PCI0.RP17)
    {
      // reset pin = GPIO_SKL_H_GPP_F18 for SPT-H and GPIO_SKL_H_GPP_D10 for uSFF, high active
      // power pin = GPIO_SKL_H_GPP_G9 for SPT-H and GPIO_SKL_H_GPP_D12 for uSFF, high active
      // wake pin  = none
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC7 (MSKCRQSRC7)
      Name(SLOT, 17)   ///- RP #17
      Name(RSTG, Package() {
              1,                  // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                  // IOEX #
              GPIO_SKL_H_GPP_F18, // GPIO pad #/IOEX pin # //Do clean up while validating RTD3 for uSFF
              0            })     // reset pin de-assert polarity
      Name(PWRG, Package() {
              1,                  // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                  // IOEX #
              GPIO_SKL_H_GPP_G9,  // GPIO pad #/IOEX pin # //Do clean up while validating RTD3 for uSFF
              1            })     // power on polarity
      Name(WAKG, Package() {
              0,                  // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                  // IOEX #
              0,                  // GPIO pad #/IOEX pin #
              0            })     // wake en polarity
      Name(SCLK, Package() {
            1,     // 0-disable, 1-enable
            0x80,  // MSKCRQSRC7
            0      // assert polarity, ICC should be LOW-activated
          })
// AMI_OVERRIDE_START - Fix Linux build error.          
//      Include("Rtd3SptPcie.asl")
      Include("RTD3SptPcie.asl")
// AMI_OVERRIDE_END - Fix Linux build error.
    }

    Scope(\_SB.PCI0.RP17.PXSX)
    {
      Name(_S0W,0x04) // PCIe SSD NVMe

      Method(_PR0, Zero, NotSerialized, 0, PkgObj){
        If(LOr(PAHC(), PNVM())){ // Check if PCIe AHCI controller or NVMe
          Return(Package(){\_SB.PCI0.RP17.PXP})
        }Else{
          Return(Package(){})
        }
      }

      Method(_PR3, Zero, NotSerialized, 0, PkgObj){
        If(LOr(PAHC(), PNVM())){ // Check if PCIe AHCI controller or NVMe
          Return(Package(){\_SB.PCI0.RP17.PXP})
        }Else{
          Return(Package(){})
        }
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
            Return(Package(){\_SB.PCI0.RP17.PXP}) // AHCI controller mode
          }Else{
            Return(Package(){}) // NVMe mode
          }
        }

        Method(_PR3, Zero, NotSerialized, 0, PkgObj){
          If(PAHC()){ // Check if PCIe AHCI controller
            Return(Package(){\_SB.PCI0.RP17.PXP}) // AHCI controller mode
          }Else{
            Return(Package(){}) // NVMe mode
          }
        }
      }// Endof MINI
    }

    ///
    /// PCIe RTD3 - m.2 WLAN
    ///
    Scope(\_SB.PCI0.RP07)
    {
      // reset pin = GPIO_SKL_H_GPP_F2 for SPT-H and GPIO_SKL_H_GPP_B21 for uSFF , high active
      // power pin = none
      // wake pin  = none
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC2 (MSKCRQSRC2)
      Name(SLOT, 7)   ///- RP #7
      Name(RSTG, Package() {
              1,                     // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                     // IOEX #
              GPIO_SKL_H_GPP_F2,     // GPIO pad #/IOEX pin # //Do clean up while validating RTD3 for uSFF
              0            })        // reset pin de-assert polarity
      Name(PWRG, Package() {
              0,                     // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                     // IOEX #
              0,                     // GPIO pad #/IOEX pin #
              0            })        // power on polarity
      Name(WAKG, Package() {
              0,                     // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                     // IOEX #
              0,                     // GPIO pad #/IOEX pin #
              0            })        // wake en polarity
      Name(SCLK, Package() {
            1,    // 0-disable, 1-enable
            0x4,  // MSKCRQSRC2
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
    Scope(\_SB.PCI0.RP06)
    {
      // reset pin = GPIO_SKL_H_GPP_H4, high active
      // power pin = GPIO_SKL_H_GPP_G17, high active
      // wake pin  = none
      // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC1 (MSKCRQSRC1)
      Name(SLOT, 6)   ///- RP #6
      Name(RSTG, Package() {
              1,                   // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                   // IOEX #
              GPIO_SKL_H_GPP_H4,   // GPIO pad #/IOEX pin #
              0            })      // reset pin de-assert polarity
      Name(PWRG, Package() {
              1,                   // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                   // IOEX #
              GPIO_SKL_H_GPP_G17,  // GPIO pad #/IOEX pin #
              1            })      // power on polarity
      Name(WAKG, Package() {
              0,                   // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0,                   // IOEX #
              0,                   // GPIO pad #/IOEX pin #
              0            })      // wake en polarity
      Name(SCLK, Package() {
            1,    // 0-disable, 1-enable
            0x2,  // MSKCRQSRC1
            0     // assert polarity, ICC should be LOW-activated
          })
// AMI_OVERRIDE_START - Fix Linux build error.          
//      Include("Rtd3SptPcie.asl")
      Include("RTD3SptPcie.asl")
// AMI_OVERRIDE_END - Fix Linux build error.
    }
// PCIe root ports - END


// USB - START
    Scope(\_SB.PCI0.XHC){

      /// USPP: Both HS/SS Ports are disconnected=1, else=0, updated inside PS2X/PS0X method call \n
      /// Bit 0 : USB SD Card Reader \n
      /// Bit 1 : USB(HS1 : 0x480 /SS0 : 0x530) \n
      /// BIT 2 : USB(HS2 : 0x490 / SS1 : 0x540) \n
      /// UPWR: Control power status 1=OFF, 0=ON \n
      Name(UPWR,0)
      Name(USPP,0)

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
        If(LAnd(LEqual(HP01,0x00),LEqual(SP00,0x00))) /// Check USB ports (HS1 : 0x480 /SS0 : 0x530) (0: not connected)
        {
          Or(\_SB.PCI0.XHC.USPP,0x02, \_SB.PCI0.XHC.USPP)  ///- Set USPP BIT1
        }
        If(LAnd(LEqual(HP02,0x00),LEqual(SP01,0x00))) /// Check USB ports (HS2 : 0x490 / SS1 : 0x540) (0: not connected)
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

      //usb power resources
      // power rail for USB2 port 8 - (GPIO_SKL_H_GPP_H7 for SPT-H and GPIO_SKL_H_GPP_C12 for uSFF)
      PowerResource(PX01, 0, 0){
        Method(_STA){                                     /// _STA Method
          If(LEqual(\_SB.GGOV(\UP8P),1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Method(_ON, 0)                                    /// _ON Method
        {
          If(LEqual(S0ID, 1))
          {
            \_SB.SGOV(\UP8P, 1)                             // drive pin high
          }
        }
        Method(_OFF, 0)                                   /// _OFF Method
        {
          If(LEqual(S0ID, 1))
          {
            \_SB.SGOV(\UP8P, 0)                             // drive pin low
          }
        }
      } // End PX01
      // power rail for USB2 port 9 - (GPIO_SKL_H_GPP_H8)
      PowerResource(PX02, 0, 0){
        Method(_STA){                                     /// _STA Method
          If(LEqual(\_SB.GGOV(GPIO_SKL_H_GPP_H8),1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Method(_ON, 0)                                    /// _ON Method
        {
          If(LEqual(S0ID, 1))
          {
            \_SB.SGOV(GPIO_SKL_H_GPP_H8, 1)         // drive pin high
          }
        }
        Method(_OFF, 0)                                   /// _OFF Method
        {
           If(LEqual(S0ID, 1))
           {
             \_SB.SGOV(GPIO_SKL_H_GPP_H8, 0)         // drive pin low
           }
        }
      } // End PX02
      // power rail for USB2 port 10 - (GPIO_SKL_H_GPP_H18)
      PowerResource(PX03, 0, 0){
        Method(_STA){                                     /// _STA Method
          If(LEqual(\_SB.GGOV(GPIO_SKL_H_GPP_H18),1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Method(_ON, 0)                                    /// _ON Method
        {
          If(LEqual(S0ID, 1))
          {
            \_SB.SGOV(GPIO_SKL_H_GPP_H18, 1)        // drive pin high
          }
        }
        Method(_OFF, 0)                                   /// _OFF Method
        {
          If(LEqual(S0ID, 1))
          {
             \_SB.SGOV(GPIO_SKL_H_GPP_H18, 0)        // drive pin low
          }
        }
      } // End PX03
      // power rail for USB2 port 11/USB3 port 5 - (GPIO_SKL_H_GPP_H17)
      PowerResource(PX04, 0, 0){
        Method(_STA){                                     /// _STA Method
          If(LEqual(\_SB.GGOV(GPIO_SKL_H_GPP_H17),1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Method(_ON, 0)                                    /// _ON Method
        {
          If(LEqual(S0ID, 1))
          {
             \_SB.SGOV(GPIO_SKL_H_GPP_H17, 1)        // drive pin high
          }
        }
        Method(_OFF, 0)                                   /// _OFF Method
        {
          If(LEqual(S0ID, 1))
          {
             \_SB.SGOV(GPIO_SKL_H_GPP_H17, 0)        // drive pin low
          }
        }
      } // End PX04
      // power rail for USB2 port 13 - (GPIO_SKL_H_GPP_H16)
      PowerResource(PX05, 0, 0){
        Method(_STA){                                     /// _STA Method
          If(LEqual(\_SB.GGOV(GPIO_SKL_H_GPP_H16),1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Method(_ON, 0)                                    /// _ON Method
        {
          If(LEqual(S0ID, 1))
          {
             \_SB.SGOV(GPIO_SKL_H_GPP_H16, 1)        // drive pin high
          }
        }
        Method(_OFF, 0)                                   /// _OFF Method
        {
          If(LEqual(S0ID, 1))
          {
            \_SB.SGOV(GPIO_SKL_H_GPP_H16, 0)        // drive pin low
           }
        }
      } // End PX05
      // power rail for USB2 port 14 - (GPIO_SKL_H_GPP_H11)
      PowerResource(PX06, 0, 0){
        Method(_STA){                                     /// _STA Method
          If(LEqual(\_SB.GGOV(GPIO_SKL_H_GPP_H11),1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Method(_ON, 0)                                    /// _ON Method
        {
          If(LEqual(S0ID, 1))
          {
             \_SB.SGOV(GPIO_SKL_H_GPP_H11, 1)        // drive pin high
          }
        }
        Method(_OFF, 0)                                   /// _OFF Method
        {
          If(LEqual(S0ID, 1))
          {
             \_SB.SGOV(GPIO_SKL_H_GPP_H11, 0)        // drive pin low
          }
        }
      } // End PX06
    }

    // report USB power resources
    Scope(\_SB.PCI0.XHC.RHUB.HS08){
      Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX01})
      Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX01})
      Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX01})

      Method(_S0W, 0)
      {
        Return(3)
      }

      Method (_S3D, 0, Serialized)
      {
         Return (0x2)
      }

      Method (_S3W, 0, Serialized)
      {
         Return (0x3)
      }

      Method (_S4D, 0, Serialized)
      {
         Return (0x2)
      }

      Method (_S4W, 0, Serialized)
      {
         Return (0x3)
      }
   }

    Scope(\_SB.PCI0.XHC.RHUB.HS09){
      Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX02})
      Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX02})
      Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX02})

      Method(_S0W, 0)
      {
        Return(3)
      }

      Method (_S3D, 0, Serialized)
      {
         Return (0x2)
      }

      Method (_S3W, 0, Serialized)
      {
         Return (0x3)
      }

      Method (_S4D, 0, Serialized)
      {
         Return (0x2)
      }

      Method (_S4W, 0, Serialized)
      {
         Return (0x3)
      }
   }

    Scope(\_SB.PCI0.XHC.RHUB.HS10){
      Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX03})
      Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX03})
      Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX03})

      Method(_S0W, 0)
      {
        Return(3)
      }

      Method (_S3D, 0, Serialized)
      {
         Return (0x2)
      }

      Method (_S3W, 0, Serialized)
      {
         Return (0x3)
      }

      Method (_S4D, 0, Serialized)
      {
         Return (0x2)
      }

      Method (_S4W, 0, Serialized)
      {
         Return (0x3)
      }
   }

    //
    // SPT-H
    //
    If(LEqual(PCHV(),SPTH)) {
      Scope(\_SB.PCI0.XHC.RHUB.HS11){
        Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX04})
        Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX04})
        Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX04})

        Method(_S0W, 0)
        {
          Return(3)
        }

        Method (_S3D, 0, Serialized)
        {
           Return (0x2)
        }

        Method (_S3W, 0, Serialized)
        {
           Return (0x3)
        }

        Method (_S4D, 0, Serialized)
        {
           Return (0x2)
        }

        Method (_S4W, 0, Serialized)
        {
           Return (0x3)
        }
      }

      Scope(\_SB.PCI0.XHC.RHUB.HS13){
        Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX05})
        Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX05})
        Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX05})

        Method(_S0W, 0)
        {
          Return(3)
        }

        Method (_S3D, 0, Serialized)
        {
           Return (0x2)
        }

        Method (_S3W, 0, Serialized)
        {
           Return (0x3)
        }

        Method (_S4D, 0, Serialized)
        {
           Return (0x2)
        }

        Method (_S4W, 0, Serialized)
        {
           Return (0x3)
        }
      }

      Scope(\_SB.PCI0.XHC.RHUB.HS14){
        Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX06})
        Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX06})
        Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX06})

        Method(_S0W, 0)
        {
          Return(3)
        }

        Method (_S3D, 0, Serialized)
        {
           Return (0x2)
        }

        Method (_S3W, 0, Serialized)
        {
           Return (0x3)
        }

        Method (_S4D, 0, Serialized)
        {
           Return (0x2)
        }

        Method (_S4W, 0, Serialized)
        {
           Return (0x3)
        }
      }
    } // if

    Scope(\_SB.PCI0.XHC.RHUB.SS05){
      Name(_PR0, Package(){\_SB.PCI0.XHC.RHUB.PX04})
      Name(_PR2, Package(){\_SB.PCI0.XHC.RHUB.PX04})
      Name(_PR3, Package(){\_SB.PCI0.XHC.RHUB.PX04})

      Method(_S0W, 0)
      {
        Return(3)
      }

      Method (_S3D, 0, Serialized)
      {
         Return (0x2)
      }

      Method (_S3W, 0, Serialized)
      {
         Return (0x3)
      }

      Method (_S4D, 0, Serialized)
      {
         Return (0x2)
      }

      Method (_S4W, 0, Serialized)
      {
         Return (0x3)
      }
   }

    Include ("Rtd3Camera.asl")

    If(LEqual(And(RCG0,2),2)) {
      Scope(\_SB.PCI0.XHC.RHUB)
      {
        //
        // Initialize USB Camera Port as no device connected
        //
        Name(SS4C,0)
        //
        // Called by \_SB.PCI0.XHC.RHUB._INI in UsbPortXhci.asl
        //
        Method(INIR, 0, Serialized) {
          OperationRegion (XHCM, SystemMemory, And(MEMB,0xFFFFFFFFFFFF0000), 0x600)
          Field (XHCM, DWordAcc, NoLock, Preserve)
          {
            Offset(0x5A0),
            SP04, 1, // SS port 4
          }
          //
          // Check if there is a device connected to IVCAM USB Camera Port
          //
          If(LEqual(SP04, 1)) {
            Store(1, SS4C)                    // Mark SS Port 4 as connected
          }
        } // Method(_INI)
      } // Scope(\_SB.PCI0.XHC.RHUB)
      //
      // For each USB Camera Port, only return _PRx with power resource if there is a device connected.
      //
      Scope(\_SB.PCI0.XHC.RHUB.SS04)        // USB Superspeed Port 3 - IVCAM
      {
        Method(_PR0) {
          If(LEqual(SS4C, 1)) {
            Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
          } Else {
            Return(Package(){})
          }
        }
        Method(_PR2) {
          If(LEqual(SS4C, 1)) {
            Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
          } Else {
            Return(Package(){})
          }
        }
        Method(_PR3) {
          If(LEqual(SS4C, 1)) {
            Return(Package(){\_SB.PCI0.XHC.RHUB.PCAM})
          } Else {
            Return(Package(){})
          }
        }
      } // Scope(\_SB.PCI0.XHC.RHUB.SS04)
    } // If(LEqual(And(RCG0,2),2))

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
          // power rail = GPIO_SKL_H_GPP_H10 for SPT-H and GPIO_SKL_H_GPP_B8 for uSFF
          If (LAnd(LEqual(^_STA, Zero), LNotEqual(\UAMS, Zero))) { ///New state = OFF Check if (_STA ==0 && \UAMS != 0) \n
            If(LEqual(\_SB.GGOV(\PAPE), 1)) {
              // drive pwr low
              \_SB.SGOV(\PAPE, 0)
              Store(Zero, ^PSTA)  ///- >> Clear PSTA
              Store(Zero, ^ONTM)  ///- >> Clear ONTM
            }
          } Else { /// New state = ON (_STA=1) or (_STA=0 and \UAMS=0)
            /// Turn power on \n
            If(LNotEqual(^PSTA, One)) { ///- Skip below if Power Resource is already in ON
              // drive pwr high
              \_SB.SGOV(\PAPE, 1)
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

  // PCH I2C0 - TouchPad Power control
    Scope(\_SB.PCI0.I2C0){

      Method(PS0X,0,Serialized)
      {
      }

      /// PS3X Method, called by PS3 method in PchSerialIo.asl
      Method(PS3X,0,Serialized)
      {
      }

      PowerResource(PXTC, 0, 0){
        /// for Touch Pad
        /// Power rail (x)
        /// reset (x)
        /// INT_R (GPIO_SKL_H_GPP_F9)
        /// Variable:
        Name(ONTM, 0) ///ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON

        Method(_STA){
          Return(0x01)
        }
        Method(_ON, 0) /// _ON Method \n Turn on
        {
          // enable int line
          \_SB.SGRA(GPIO_SKL_H_GPP_F9, 1)
        }

        Method(_OFF, 0)  /// _OFF method \n Turn off
        {
          // disable int line
          \_SB.SGRA(GPIO_SKL_H_GPP_F9, 0)
        }
      } // End PXTC

      Scope(TPD0){
        Name (TD_P, Package(){\_SB.PCI0.I2C0.PXTC})               // TD_P - Touch Device Power Package

        Alias(IC1S, TD_D)                                         // TD_D - Touch Device power on delay
        Alias(\_SB.PCI0.I2C0.PXTC.ONTM, TD_C)                     // TD_C - Touch Device I2C controller power on timestamp

        Include("Rtd3I2cTouchDev.asl")
        Method(_PS0) { PS0X() }
        Method(_PS3) { PS3X() }
      }// End Of Scope(TPD0)
   }//  Scope(\_SB.PCI0.I2C0)

    // PCH I2C1 - TouchPanel Power control
    Scope(\_SB.PCI0.I2C1){

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

      PowerResource(PXTC, 0, 0){
        /// for Touch Panel
        /// Power rail (GPIO_SKL_H_GPP_B1)
        /// reset (GPIO_SKL_H_GPP_F3)
        /// INT_R (GPIO_SKL_H_GPP_E7)
        /// Variable:
        Name(ONTM, 0) ///ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON

        Method(_STA){
          If(LEqual(\_SB.GGOV(GPIO_SKL_H_GPP_B1),1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Method(_ON, 0) /// _ON Method \n Turn on
        {
          // De-Assert GPIO RST
          \_SB.SGOV(GPIO_SKL_H_GPP_F3, 1)
          // drive pwr high
          \_SB.SGOV(GPIO_SKL_H_GPP_B1, 1)
          // update ONTM
          Store(Timer(), ONTM)
          // enable int line (GPP_E_7)
          \_SB.SGRA(GPIO_SKL_H_GPP_E7, 1)
        }

        Method(_OFF, 0)  /// _OFF method \n Turn off
        {
          // disable int line
          \_SB.SGRA(GPIO_SKL_H_GPP_E7, 0)
          // drive pwr low
          \_SB.SGOV(GPIO_SKL_H_GPP_B1, 0)
          // Assert GPIO RST
          \_SB.SGOV(GPIO_SKL_H_GPP_F3, 0)
          // update ONTM
          Store(Zero , ^ONTM)  ///- Clear ONTM
        }
      } // End PXTC

      Scope(TPL1){
        Name (TD_P, Package(){\_SB.PCI0.I2C1.PXTC})               // TD_P - Touch Device Power Package

        Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
        Alias(\_SB.PCI0.I2C1.PXTC.ONTM, TD_C)                     // TD_C - Touch Device I2C controller power on timestamp

        Include("Rtd3I2cTouchDev.asl")
        Method(_PS0) { PS0X() }
        Method(_PS3) { PS3X() }
      }// End Of Scope(TPL1)


   }//  Scope(\_SB.PCI0.I2C1)

//Serial IO End
  //GPE Event handling - Start
  Scope(\_GPE) {
    ///
    /// PCIE slot 2 wake event - wake pin (GPIO_SKL_H_GPP_G18 for SPT-H and GPIO_SKL_H_GPP_B15 for uSFF)
    ///
    /// PCIe WAKE#
    Method(_L12) {
      \_SB.SHPO(\PSTW, 1)              // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
      Notify(\_SB.PCI0.RP09, 0x02)     // device wake
    }
  } //Scope(\_GPE)


  Scope(\_SB.PCI0.SAT0) {

    Method(_PS0,0,Serialized){}
    Method(_PS3,0,Serialized){}

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
  }//end Scope(\_SB.PCI0.SAT0)


  //SATA lane 0 -
  //SATA lane 1 -
  //SATA lanes 2,3 -
  //SATA lanes 4,5 -
//##############################################################
  If(And(RCG1_RTD3_PRT0_ENABLED,RCG1)) {
    Scope(\_SB.PCI0.SAT0.PRT0) {
      Name(PORT, 0)
      Name(PBAR, 0x118)       // P0 command port = ABAR+0x118
      Name(PWRG, Package() {
              1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              GPIO_SKL_H_GPP_G8, // GPIO pad #/IOEX pin #
              1  // power on polarity
      })
// AMI_OVERRIDE_START - Fix Linux build error.        
//      Include("Rtd3SptSata.asl")
      Include("RTD3SptSata.asl")
// AMI_OVERRIDE_END - Fix Linux build error.  
    }
  }

  If(And(RCG1_RTD3_PRT1_ENABLED,RCG1)) {
    Scope(\_SB.PCI0.SAT0.PRT1) {
      Name(PORT, 1)
      Name(PBAR, 0x198)       // P1 command port = ABAR+0x198
      Name(PWRG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              0, // GPIO pad #/IOEX pin #
              1  // power on polarity
      })
// AMI_OVERRIDE_START - Fix Linux build error.        
//      Include("Rtd3SptSata.asl")
      Include("RTD3SptSata.asl")
// AMI_OVERRIDE_END - Fix Linux build error. 
    }
   }

   If(And(RCG1_RTD3_PRT2_ENABLED,RCG1)) {
     Scope(\_SB.PCI0.SAT0.PRT2) {
      Name(PORT, 2)
      Name(PBAR, 0x218)       // P2 command port = ABAR+0x218
      Name(PWRG, Package() {
              1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              GPIO_SKL_H_GPP_G11, // GPIO pad #/IOEX pin #
              1  // power on polarity
      })
// AMI_OVERRIDE_START - Fix Linux build error.       
//      Include("Rtd3SptSata.asl")
      Include("RTD3SptSata.asl")
// AMI_OVERRIDE_END - Fix Linux build error. 
     }
   }

   If(And(RCG1_RTD3_PRT3_ENABLED,RCG1)) {
    Scope(\_SB.PCI0.SAT0.PRT3) {
      Name(PORT, 3)
      Name(PBAR, 0x298)       // P3 command port = ABAR+0x298
      Name(PWRG, Package() {
              1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              GPIO_SKL_H_GPP_G1, // GPIO pad #/IOEX pin #
              1  // power on polarity
      })
// AMI_OVERRIDE_START - Fix Linux build error. 
//      Include("Rtd3SptSata.asl")
      Include("RTD3SptSata.asl")
// AMI_OVERRIDE_END - Fix Linux build error. 
    }
   }

   If(And(RCG1_RTD3_PRT4_ENABLED,RCG1)) {
    Scope(\_SB.PCI0.SAT0.PRT4) {
      Name(PORT, 4)
      Name(PBAR, 0x318)       // P4 command port = ABAR+0x318
      Name(PWRG, Package() {
              1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
              GPIO_SKL_H_GPP_G9, // GPIO pad #/IOEX pin #
              1  // power on polarity
      })
// AMI_OVERRIDE_START - Fix Linux build error.       
//      Include("Rtd3SptSata.asl")
      Include("RTD3SptSata.asl")
// AMI_OVERRIDE_END - Fix Linux build error. 
    }
   }

   If(And(RCG1_RTD3_NVM1_ENABLED,RCG1)) {
      Scope(\_SB.PCI0.SAT0.NVM1) {
        Include("Rtd3RstRemap.asl")
      }
    }

   If(And(RCG1_RTD3_NVM2_ENABLED,RCG1)) {
      Scope(\_SB.PCI0.SAT0.NVM2) {
        Include("Rtd3RstRemap.asl")
      }
    }

   If(And(RCG1_RTD3_NVM3_ENABLED,RCG1)) {
      Scope(\_SB.PCI0.SAT0.NVM3) {
        Include("Rtd3RstRemap.asl")
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

      If (LEqual(Arg0, ToUUID ("E4DB149B-FCFE-425b-A6D8-92357D78FC7F"))) {
        Switch(ToInteger(Arg2)) {
          Case (0) {
            Return(0x0f)
          }
          Case (1) {
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


 If(And(RCG1_RTD3_VOLx_ENABLED,RCG1)) {
   Scope(\_SB.PCI0.SAT0) {

      Include ("RTD3RstRaid.asl")

      // CPON  : Method to turn ON ports per PMAP bitmap status
      //  Arg0 : PMAP - Port Bitmap Status
      Method(CPON,1,Serialized,,,{IntObj})
      {
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT0.SPON),And(Arg0, RAID_VOL_PRT0_MASK))) { \_SB.PCI0.SAT0.PRT0.SPON() }// Turn On Port 0 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT1.SPON),And(Arg0, RAID_VOL_PRT1_MASK))) { \_SB.PCI0.SAT0.PRT1.SPON() }// Turn On Port 1 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT2.SPON),And(Arg0, RAID_VOL_PRT2_MASK))) { \_SB.PCI0.SAT0.PRT2.SPON() }// Turn On Port 2 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT3.SPON),And(Arg0, RAID_VOL_PRT3_MASK))) { \_SB.PCI0.SAT0.PRT3.SPON() }// Turn On Port 3 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT4.SPON),And(Arg0, RAID_VOL_PRT4_MASK))) { \_SB.PCI0.SAT0.PRT4.SPON() }// Turn On Port 4 Power
        // Remapped PCIe
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.NVM1.RPPC),And(Arg0, RAID_VOL_NVM1_MASK))) { \_SB.PCI0.SAT0.NVM1.RPPC(1) }// Turn On CR1 Powe
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.NVM2.RPPC),And(Arg0, RAID_VOL_NVM2_MASK))) { \_SB.PCI0.SAT0.NVM2.RPPC(1) }// Turn On CR2 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.NVM3.RPPC),And(Arg0, RAID_VOL_NVM3_MASK))) { \_SB.PCI0.SAT0.NVM3.RPPC(1) }// Turn On CR3 Power
      }

      // CPOF  : Method to turn OFF ports per PMAP bitmap status
      //  Arg0 : PMAP - Port Bitmap Status
      Method(CPOF,1,Serialized,,,{IntObj})
      {
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT0.SPOF),And(Arg0, RAID_VOL_PRT0_MASK))) { \_SB.PCI0.SAT0.PRT0.SPOF() }// Turn Off Port 0 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT1.SPOF),And(Arg0, RAID_VOL_PRT1_MASK))) { \_SB.PCI0.SAT0.PRT1.SPOF() }// Turn Off Port 1 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT2.SPOF),And(Arg0, RAID_VOL_PRT2_MASK))) { \_SB.PCI0.SAT0.PRT2.SPOF() }// Turn Off Port 2 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT3.SPOF),And(Arg0, RAID_VOL_PRT3_MASK))) { \_SB.PCI0.SAT0.PRT3.SPOF() }// Turn Off Port 3 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.PRT4.SPOF),And(Arg0, RAID_VOL_PRT4_MASK))) { \_SB.PCI0.SAT0.PRT4.SPOF() }// Turn Off Port 4 Power
        // Remapped PCIe
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.NVM1.RPPC),And(Arg0, RAID_VOL_NVM1_MASK))) { \_SB.PCI0.SAT0.NVM1.RPPC(0) }// Turn Off CR1 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.NVM2.RPPC),And(Arg0, RAID_VOL_NVM2_MASK))) { \_SB.PCI0.SAT0.NVM2.RPPC(0) }// Turn Off CR2 Power
        If(LAnd(CondRefOf(\_SB.PCI0.SAT0.NVM3.RPPC),And(Arg0, RAID_VOL_NVM3_MASK))) { \_SB.PCI0.SAT0.NVM3.RPPC(0) }// Turn Off CR3 Power
      }
   }
 }

// SATA - END


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
