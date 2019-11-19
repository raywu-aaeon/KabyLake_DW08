/** @file
  ACPI uPEP Support

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2019 Intel Corporation.

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

External(\_GPE.TBNF, MethodObj)

Scope (\_SB.PCI0.GFX0)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package(){})
    }
  }
}

Scope (\_SB.PCI0.ISP0)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope (\_SB.PCI0.SAT0)
{
  Method(_DEP){
    If(LAnd(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015)), LNotEqual(And(PEPC, 0x03), 0))){ // PEPC Bit[1:0] - SATA (0:None, 1:SATA Ports[all], 2:SATA Controller)
      // SATA PEP not set to No Constraint
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope (\_SB.PCI0.SAT0.VOL0)
{
  Method(_DEP){
    If(LEqual(OSYS, 2013)) {
      Return(Package() {})
    }
    Return(Package() {\_SB.PEPD})
  }
}

If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
  Scope(\_SB.PCI0.I2C0) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C1) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C2) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C3) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C4) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C5) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.SPI0) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.SPI1) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.UA00) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.UA01) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.UA02) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.HECI) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.GLAN) { Name(_DEP, Package(){\_SB.PEPD}) }
}

Scope (\_SB.PCI0.XHC)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope (\_SB.PCI0.HDAS)
{
  Method(_DEP){
    If(LEqual(S0ID, 1)){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

// AMI_OVERRIDE_START - For Hybric Graphics feature.
If (CondRefOf(\_SB.PCI0.RP01.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP01.PXSX)
{
  include("PepPcieSsd.asl")
}

// AMI_OVERRIDE_START - For Hybric Graphics feature.
}
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP02.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP03.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP04.PXSX)
{
  include("PepPcieSsd.asl")
}

// AMI_OVERRIDE_START - For Hybric Graphics feature.
If (CondRefOf(\_SB.PCI0.RP05.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP05.PXSX)
{
  include("PepPcieSsd.asl")
}

// AMI_OVERRIDE_START - For Hybric Graphics feature.
}
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP06.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP07.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP08.PXSX)
{
  include("PepPcieSsd.asl")
}

// AMI_OVERRIDE_START - For Hybric Graphics feature.
If (CondRefOf(\_SB.PCI0.RP09.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP09.PXSX)
{
  include("PepPcieSsd.asl")
}

// AMI_OVERRIDE_START - For Hybric Graphics feature.
}
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP10.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP11.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP12.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP13.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP14.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP15.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP16.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP17.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP18.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP19.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PCI0.RP20.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope(\_PR.PR00)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR01)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR02)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR03)
{
  Method(_DEP){
    ADBG("PR03 DEP Call")
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR04)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR05)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR06)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR07)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR08)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR09)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR10)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR11)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR12)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR13)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR14)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_PR.PR15)
{
  Method(_DEP){
    If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
      Return(Package() {\_SB.PEPD})
    }Else{
      Return(Package() {})
    }
  }
}

Scope(\_SB)
{
  Device (PEPD)
  {
    Name (_HID, "INT33A1")
    Name (_CID, EISAID ("PNP0D80"))
    Name (_UID, 0x1)

    Name(WWD3, Package() {"\\_SB.PCI0.RP10",               0x0, Package() {0, Package() {0xFF, 3}}})
    Name(DEVY, Package() // uPEP Device List
    {
      //
      // 1: ACPI Device Descriptor: Fully Qualified name-string
      // 2: Enabled/Disabled Field
      //      0 = This device is disabled and applies no constraints
      //     1+ = This device is enabled and applies constraints
      // 3: Constraint Package: entry per LPI state in LPIT
      //     a. Associated LPI State UID
      //         ID == 0xFF: same constraints apply to all states in LPIT
      //     b: minimum Dx state as pre-condition
      //     c: (optional) OEM specific OEM may provide an additional encoding
      //         which further defines the D-state Constraint
      //            0x0-0x7F - Reserved
      //            0x80-0xFF - OEM defined
      //
      Package() {"\\_PR.PR00",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  0 - PR.PR00
      Package() {"\\_PR.PR01",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  1 - PR.PR01
      Package() {"\\_PR.PR02",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  2 - PR.PR02
      Package() {"\\_PR.PR03",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  3 - PR.PR03
      Package() {"\\_PR.PR04",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  4 - PR.PR04
      Package() {"\\_PR.PR05",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  5 - PR.PR05
      Package() {"\\_PR.PR06",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  6 - PR.PR06
      Package() {"\\_PR.PR07",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  7 - PR.PR07
      Package() {"\\_SB.PCI0.GFX0",               0x1, Package() {0, Package() {0xFF, 3}}},                   //  8 - Gfx
      Package() {"\\_SB.PCI0.SAT0",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             //  9 - SATA
      Package() {"\\_SB.PCI0.UA00",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 10 - UART0
      Package() {"\\_SB.PCI0.UA01",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 11 - UART1
      Package() {"\\_SB.PCI0.I2C0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 12 - I2C0
      Package() {"\\_SB.PCI0.I2C1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 13 - I2C1
      Package() {"\\_SB.PCI0.XHC",                0x1, Package() {0, Package() {0xFF, 3}}},                   // 14 - XHCI
      Package() {"\\_SB.PCI0.HDAS",               0x1, Package() {0, Package() {0xFF, 0, 0x81}}},             // 15 - cAVS (HDAudio)
      Package() {"\\_SB.PCI0.PEMC",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 16 - EMMC
      Package() {"\\_SB.PCI0.PSDC",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 17 - SCS SD Card (PCI Mode)
      Package() {"\\_SB.PCI0.I2C2",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 18 - I2C2
      Package() {"\\_SB.PCI0.I2C3",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 19 - I2C3
      Package() {"\\_SB.PCI0.I2C4",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 20 - I2C4
      Package() {"\\_SB.PCI0.I2C5",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 21 - I2C5
      Package() {"\\_SB.PCI0.UA02",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 22 - UART2
      Package() {"\\_SB.PCI0.SPI0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 23 - SPI0
      Package() {"\\_SB.PCI0.SPI1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 24 - SPI1
      Package() {"\\_SB.PCI0.RP01.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 25 - RP01
      Package() {"\\_SB.PCI0.RP02.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 26 - RP02
      Package() {"\\_SB.PCI0.RP03.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 27 - RP03
      Package() {"\\_SB.PCI0.RP04.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 28 - RP04
      Package() {"\\_SB.PCI0.RP05.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 29 - RP05
      Package() {"\\_SB.PCI0.RP06.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 30 - RP06
      Package() {"\\_SB.PCI0.RP07.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 31 - RP07
      Package() {"\\_SB.PCI0.RP08.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 32 - RP08
      Package() {"\\_SB.PCI0.RP09.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 33 - RP09
      Package() {"\\_SB.PCI0.RP10.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 34 - RP10
      Package() {"\\_SB.PCI0.RP11.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 35 - RP11
      Package() {"\\_SB.PCI0.RP12.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 36 - RP12
      Package() {"\\_SB.PCI0.RP13.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 37 - RP13
      Package() {"\\_SB.PCI0.RP14.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 38 - RP14
      Package() {"\\_SB.PCI0.RP15.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 39 - RP15
      Package() {"\\_SB.PCI0.RP16.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 40 - RP16
      Package() {"\\_SB.PCI0.RP17.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 41 - RP17
      Package() {"\\_SB.PCI0.RP18.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 42 - RP18
      Package() {"\\_SB.PCI0.RP19.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 43 - RP19
      Package() {"\\_SB.PCI0.RP20.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 44 - RP20
      Package() {"\\_SB.PCI0.ISP0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 45 - ISP0
      Package() {"\\_SB.PCI0.SAT0.PRT0",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 46 - SATA PORT0
      Package() {"\\_SB.PCI0.SAT0.PRT1",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 47 - SATA PORT1
      Package() {"\\_SB.PCI0.SAT0.PRT2",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 48 - SATA PORT2
      Package() {"\\_SB.PCI0.SAT0.PRT3",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 49 - SATA PORT3
      Package() {"\\_SB.PCI0.SAT0.PRT4",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 50 - SATA PORT4
      Package() {"\\_SB.PCI0.SAT0.PRT5",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 51 - SATA PORT5
      Package() {"\\_SB.PCI0.SAT0.NVM1",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 52 - SATA NVM1
      Package() {"\\_SB.PCI0.SAT0.NVM2",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 53 - SATA NVM2
      Package() {"\\_SB.PCI0.SAT0.NVM3",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 54 - SATA NVM3
      Package() {"\\_SB.PCI0.SAT0.VOL0",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 55 - RAID VOL0
      Package() {"\\_PR.PR08",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 56 - PR.PR08
      Package() {"\\_PR.PR09",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 57 - PR.PR09
      Package() {"\\_PR.PR10",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 58 - PR.PR10
      Package() {"\\_PR.PR11",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 59 - PR.PR11
      Package() {"\\_PR.PR12",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 60 - PR.PR12
      Package() {"\\_PR.PR13",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 61 - PR.PR13
      Package() {"\\_PR.PR14",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 62 - PR.PR14
      Package() {"\\_PR.PR15",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 63 - PR.PR15
      Package() {"\\_SB.PCI0.HECI",               0x1, Package() {0, Package() {0xFF, 0, 0x81}}},             // 64 - CSME
      Package() {"\\_SB.PCI0.GLAN",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 65 - LAN(GBE)
      Package() {"\\_SB.PCI0.RP12",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 66 - RP12
      Package() {"USB\\VID_8087&PID_0AC9&MI*",    0x0, Package() {0, Package() {0xFF, 2}}},                   // 67 - CDC MBIM
      Package() {"\\_SB.PCI0.RP10",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 68 - RP10
      Package() {"Reserved For TBT RP",           0x0, Package() {0, Package() {0xFF, 3}}},                   // 69 - TBT RP
      Package() {"Reserved For TBT xHci",         0x0, Package() {0, Package() {0xFF, 3}}},                   // 70 - TBT xHci
      Package() {"Reserved For Multi TBT RP",     0x0, Package() {0, Package() {0xFF, 3}}},                   // 71 - Multi TBT RP
      Package() {"Reserved For Multi TBT xHci",   0x0, Package() {0, Package() {0xFF, 3}}},                   // 72 - Multi TBT xHci
      Package() {"\\_SB.PCI0.CIO2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 73 - SkyCamSensors
    })

    Name(BCCD, Package() // Bugcheck Critical Device(s)
    {
      //
      // 1: ACPI Device Descriptor: Fully Qualified name string
      // 2: Package of packages: 1 or more specific commands to power up critical device
      //  2a: Package: GAS-structure describing location of PEP accessible power control
      //    Refer to ACPI 5.0 spec section 5.2.3.1 for details
      //    a: Address Space ID (0 = System Memory)
      //       NOTE: A GAS Address Space of 0x7F (FFH) indicates remaining package
      //             elements are Intel defined
      //    b: Register bit width (32 = DWORD)
      //    c: Register bit offset
      //    d: Access size (3 = DWORD Access)
      //    e: Address (for System Memory = 64-bit physical address)
      //  2b: Package containing:
      //    a: AND mask !V not applicable for all Trigger Types
      //    b: Value (bits required to power up the critical device)
      //    c: Trigger Type:
      //         0 = Read
      //         1 = Write
      //         2 = Write followed by Read
      //         3 = Read Modify Write
      //         4 = Read Modify Write followed by Read
      //  2c: Power up delay: Time delay before next operation in uSec
      //
      Package() {"\\_SB.PCI0.SAT0", Package() {
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PCI0.SAT0.PRT0", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PCI0.SAT0.PRT1", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PCI0.SAT0.PRT2", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PCI0.SAT0.PRT3", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PCI0.SAT0.VOL0", Package() {
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PCI0.RP01.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP02.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP03.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP04.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP05.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP06.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP07.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP08.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP09.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP10.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP11.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP12.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP13.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP14.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP15.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP16.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP17.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP18.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP19.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PCI0.RP20.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
    })

    Method(_STA, 0x0, NotSerialized)
    {
      If(LOr(LGreaterEqual(OSYS,2015), LAnd(LGreaterEqual(OSYS,2012),LEqual(S0ID, 1))))
      {
        Return(0xf)
      }
      Return(0)
    }

    Method(_DSM, 0x4, Serialized)
    {

      If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
      {
        // Number of Functions (including this one)
        If(LEqual(Arg2, Zero))
        {
          Return(Buffer(One){0x7F})
        }
        // Device Constraints Enumeration
        If(LEqual(Arg2, One))
        {
          If (LEqual(S0ID, 0)) {
            Return(Package() {})
          }

          // Update uPEP device list based on PEPC (Low Power S0 Constraint)
          // Bit[0]   - En/Dis LAN(GBE)
          // Bit[1]   - En/Dis ISP0
          // Bit[2]   - En/Dis UART0
          // Bit[3]   - En/Dis UART1
          // Bit[4]   - En/Dis TBT RP
          // Bit[5]   - En/Dis I2C0
          // Bit[6]   - En/Dis I2C1
          // Bit[7]   - En/Dis XHCI
          // Bit[8]   - En/Dis HD Audio (includes ADSP)
          // Bit[9]   - En/Dis Gfx
          // Bit[10]  - En/Dis CPU
          // Bit[11]  - En/Dis EMMC
          // Bit[12]  - En/Dis SDXC
          // Bit[13]  - En/Dis I2C2
          // Bit[14]  - En/Dis I2C3
          // Bit[15]  - En/Dis I2C4
          // Bit[16]  - En/Dis I2C5
          // Bit[17]  - En/Dis UART2
          // Bit[18]  - En/Dis SPI0
          // Bit[19]  - En/Dis SPI1
          // Bit[20]  - En/Dis SATA
          // Bit[21]  - En/Dis RAID VOL0
          // Bit[22]  - En/Dis SATA PORT0
          // Bit[23]  - En/Dis SATA PORT1
          // Bit[24]  - En/Dis SATA PORT2
          // Bit[25]  - En/Dis SATA PORT3
          // Bit[26]  - En/Dis SATA PORT4
          // Bit[27]  - En/Dis SATA PORT5
          // Bit[28]  - En/Dis SATA NVM1
          // Bit[29]  - En/Dis SATA NVM2
          // Bit[30]  - En/Dis SATA NVM3
          // Bit[31]  - En/Dis CSME

          // Update uPEP device list based on PEP1 (Low Power S0 Constraint)
          // Bit[0]   - En/Dis SkyCamSensors

          If(LNotEqual(And(PEPC, BIT20), 0))
          {
            If(LNotEqual(SPST, 0)) { // Check if any device is connected to SATA controller.
              // Constraint for SATA Controller (SAT0)
              Store (0x01, Index (DeRefOf(Index (DEVY, 9)), 1)) // 9 - SATA
            }

            // Only enable PCIe Root Port for PEP if AHCI Controller or NVMe Class Code is connected.
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP01.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(LOr(\_SB.PCI0.RP01.PXSX.PAHC(), \_SB.PCI0.RP01.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 25)), 1)) // 25 - RP01
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.	    
            If(LOr(\_SB.PCI0.RP02.PXSX.PAHC(), \_SB.PCI0.RP02.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 26)), 1)) // 26 - RP02
            }
            If(LOr(\_SB.PCI0.RP03.PXSX.PAHC(), \_SB.PCI0.RP03.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 27)), 1)) // 27 - RP03
            }
            If(LOr(\_SB.PCI0.RP04.PXSX.PAHC(), \_SB.PCI0.RP04.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 28)), 1)) // 28 - RP04
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP05.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(LOr(\_SB.PCI0.RP05.PXSX.PAHC(), \_SB.PCI0.RP05.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 29)), 1)) // 29 - RP05
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.	    
            If(LOr(\_SB.PCI0.RP06.PXSX.PAHC(), \_SB.PCI0.RP06.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 30)), 1)) // 30 - RP06
            }
            If(LOr(\_SB.PCI0.RP07.PXSX.PAHC(), \_SB.PCI0.RP07.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 31)), 1)) // 31 - RP07
            }
            If(LOr(\_SB.PCI0.RP08.PXSX.PAHC(), \_SB.PCI0.RP08.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 32)), 1)) // 32 - RP08
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP09.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.	    
            If(LOr(\_SB.PCI0.RP09.PXSX.PAHC(), \_SB.PCI0.RP09.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 33)), 1)) // 33 - RP09
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.	    
            If(LOr(\_SB.PCI0.RP10.PXSX.PAHC(), \_SB.PCI0.RP10.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 34)), 1)) // 34 - RP10
            }
            If(LOr(\_SB.PCI0.RP11.PXSX.PAHC(), \_SB.PCI0.RP11.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 35)), 1)) // 35 - RP11
            }
            If(LOr(\_SB.PCI0.RP12.PXSX.PAHC(), \_SB.PCI0.RP12.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 36)), 1)) // 36 - RP12
            }
            If(LOr(\_SB.PCI0.RP13.PXSX.PAHC(), \_SB.PCI0.RP13.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 37)), 1)) // 37 - RP13
            }
            If(LOr(\_SB.PCI0.RP14.PXSX.PAHC(), \_SB.PCI0.RP14.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 38)), 1)) // 38 - RP14
            }
            If(LOr(\_SB.PCI0.RP15.PXSX.PAHC(), \_SB.PCI0.RP15.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 39)), 1)) // 39 - RP15
            }
            If(LOr(\_SB.PCI0.RP16.PXSX.PAHC(), \_SB.PCI0.RP16.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 40)), 1)) // 40 - RP16
            }
            If(LOr(\_SB.PCI0.RP17.PXSX.PAHC(), \_SB.PCI0.RP17.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 41)), 1)) // 41 - RP17
            }
            If(LOr(\_SB.PCI0.RP18.PXSX.PAHC(), \_SB.PCI0.RP18.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 42)), 1)) // 42 - RP18
            }
            If(LOr(\_SB.PCI0.RP19.PXSX.PAHC(), \_SB.PCI0.RP19.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 43)), 1)) // 43 - RP19
            }
            If(LOr(\_SB.PCI0.RP20.PXSX.PAHC(), \_SB.PCI0.RP20.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 44)), 1)) // 40 - RP20
            }
          }

          If(LAnd(LGreaterEqual(OSYS, 2015), LNotEqual(WRTO, 0))){
            //
            // WRTO - WWAN RTD3 Option
            //   3: D3/L2
            //   1: D0/L1.2
            //   0: Disabled
            //
            If(\_SB.PCI0.RP01.PXSX.WWST())
            {
              If(LEqual(WRTO, 3)) {
                // Put D3 constraint on RP01
                Store ("\\_SB.PCI0.RP01", Index (WWD3, 0))
                Store (WWD3, Index (DEVY, 68))                     // 68 - RP01 D3
                Store (0x01, Index (DeRefOf(Index (DEVY, 68)), 1)) // 68 - RP01 WWAN
              }
              If(LEqual(WRTO, 1)) {
                // Put D2 constraint on CDC MBIM
                Store (0x01, Index (DeRefOf(Index (DEVY, 67)), 1)) // 67 - CDC MBIM
              }
            }
            If(\_SB.PCI0.RP10.PXSX.WWST())
            {
              If(LEqual(WRTO, 3)) {
                // Put D3 constraint on RP10
                Store ("\\_SB.PCI0.RP10", Index (WWD3, 0))
                Store (WWD3, Index (DEVY, 68))                     // 68 - RP10 D3
                Store (0x01, Index (DeRefOf(Index (DEVY, 68)), 1)) // 68 - RP10 WWAN
              }
              If(LEqual(WRTO, 1)) {
                // Put D2 constraint on CDC MBIM
                Store (0x01, Index (DeRefOf(Index (DEVY, 67)), 1)) // 67 - CDC MBIM
              }
            }
          }

          If(LNotEqual(And(PEPC, BIT21), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 55)), 1)) // 55 - RAID
          }

          If(LNotEqual(And(PEPC, BIT22), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 46)), 1)) // 46 - SATA PORT0
          }

          If(LNotEqual(And(PEPC, BIT23), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 47)), 1)) // 47 - SATA PORT1
          }

          If(LNotEqual(And(PEPC, BIT24), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 48)), 1)) // 48 - SATA PORT2
          }

          If(LNotEqual(And(PEPC, BIT25), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 49)), 1)) // 49 - SATA PORT3
          }

          If(LNotEqual(And(PEPC, BIT26), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 50)), 1)) // 50 - SATA PORT4
          }

          If(LNotEqual(And(PEPC, BIT27), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 51)), 1)) // 51 - SATA PORT5
          }

          If(LNotEqual(And(PEPC, BIT28), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 52)), 1)) // 52 - SATA NVM1
          }

          If(LNotEqual(And(PEPC, BIT29), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 53)), 1)) // 53 - SATA NVM2
          }

          If(LNotEqual(And(PEPC, BIT30), 0))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 54)), 1)) // 54 - SATA NVM3
          }

          If(LEqual(And(PEPC, BIT31), 0)) // PEPC Bit[31] - En/Dis CSME
          {
            Store (0x00, Index (DeRefOf(Index (DEVY, 64)), 1)) // 64 - CSME
          }

          If(LEqual(And(PEPC, BIT0), 0)) // PEPC Bit[0] - En/Dis LAN(GBE)
          {
            Store (0x00, Index (DeRefOf(Index (DEVY, 65)), 1)) // 65 - LAN(GBE)
          }

          If(LEqual(And(PEPC, BIT2), 0)) // PEPC Bit[2] - En/Dis UART0
          {
            // Disabled UA00
            Store (0x00, Index (DeRefOf(Index (DEVY, 10)), 1)) // 10 - UART0
          }

          If(LEqual(And(PEPC, BIT3), 0)) // PEPC Bit[3] - En/Dis UART1
          {
            // Disabled UA01
            Store (0x00, Index (DeRefOf(Index (DEVY, 11)), 1)) // 11 - UART1
          }

          If(LNotEqual(And(PEPC, BIT4), 0)) // PEPC Bit[4] - En/Dis TBT RP
          {
            Store (0, Local0) // Intialise Valid PCH RP to 0
            If (LGreater (TBSE, 0)) // TBT RP Must be Greater Than 0
            {
              If (LLess (TBSE, 10))
              {
                Store (Concatenate("RP0", ToDecimalString(TBSE)), Local1)
                Store (1, Local0) // Initialise Valid PCH RP to 1
              } Else {
                If (LLess (TBSE, 25))
                {
                  Store (Concatenate("RP", ToDecimalString(TBSE)), Local1)
                  Store (1, Local0) // Initialise Valid PCH RP to 1
                }
              }
            }
            If (LEqual (Local0, 1)) // If Valid PCH RP is passed
            {
              Store (Concatenate("\\_SB.PCI0.", Local1), Index (DeRefOf(Index (DEVY, 69)), 0)) // 69 - TBT RPxx
              Store (0x01, Index (DeRefOf(Index (DEVY, 69)), 1)) // 69 - TBT RPxx
              Store (Concatenate(DeRefOf (Index (DeRefOf(Index (DEVY, 69)), 0)), ".PXSX.TBDU.XHC"), Index (DeRefOf(Index (DEVY, 70)), 0)) // 70 - TBT RPxx xHci
              Store (0x01, Index (DeRefOf(Index (DEVY, 70)), 1)) // 70 - TBT RPxx XHC
            }
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
            // For TBT Multicontroler Support
            If (LEqual (TBMP, 1)) // If TBT MultiController is Supported
            {
              Store (0, Local0) // Intialise Valid PCH RP to 0
              If (LGreater (TBS1, 0)) // TBT RP Must be Greater Than 0
              {
                If (LLess (TBS1, 10))
                {
                  Store (Concatenate("RP0", ToDecimalString(TBS1)), Local1)
                  Store (1, Local0) // Initialise Valid PCH RP to 1
                } Else {
                  If (LLess (TBS1, 25))
                  {
                    Store (Concatenate("RP", ToDecimalString(TBS1)), Local1)
                    Store (1, Local0) // Initialise Valid PCH RP to 1
                  }
                }
              }
              If (LEqual (Local0, 1)) // If Valid PCH RP is passed
              {
                Store (Concatenate("\\_SB.PCI0.", Local1), Index (DeRefOf(Index (DEVY, 71)), 0)) // 71 - Multi TBT RPxx
                Store (0x01, Index (DeRefOf(Index (DEVY, 71)), 1)) // 71 - Multi TBT RPxx
                Store (Concatenate(DeRefOf (Index (DeRefOf(Index (DEVY, 71)), 0)), ".PXSX.TBDU.XHC"), Index (DeRefOf(Index (DEVY, 72)), 0)) // 72 - Multi TBT RPxx xHci
                Store (0x01, Index (DeRefOf(Index (DEVY, 72)), 1)) // 72 - Multi TBT RPxx XHC
              }
            }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
          }

          If(LEqual(And(PEPC, BIT5), 0)) // PEPC Bit[5] - En/Dis I2C0
          {
            // Disabled I2C0
            Store (0x00, Index (DeRefOf(Index (DEVY, 12)), 1)) // 12 - I2C0
          }

          If(LEqual(And(PEPC, BIT6), 0))  // PEPC Bit[6] - En/Dis I2C1
          {
            // Disabled I2C1
            Store (0x00, Index (DeRefOf(Index (DEVY, 13)), 1)) // 13 - I2C1
          }

          If(LEqual(And(PEPC, BIT7), 0))  // PEPC Bit[7] - En/Dis XHCI
          {
            // Disabled XHCI
            Store (0x00, Index (DeRefOf(Index (DEVY, 14)), 1)) // 14 - XHCI
          }

          If(LEqual(And(PEPC, BIT8), 0)) // PEPC Bit[8] - En/Dis Azalia
          {
            // Disabled Azalia
            Store (0x00, Index (DeRefOf(Index (DEVY, 15)), 1)) // 15 - HD Audio (includes ADSP)
          }

          If(LEqual(And(PEPC, BIT9), 0)) // PEPC Bit[9] - En/Dis Gfx
          {
            // Disabled Gfx
            Store (0x00, Index (DeRefOf(Index (DEVY, 8)), 1)) // 8 - Gfx
          }

          // Apply PEP constraints for CPU,
          // Based on number of threads
          If (LGreaterEqual(TCNT, 1))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 0)), 1))  // 0 - PR00
          }
          If (LGreaterEqual(TCNT, 2))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 1)), 1))  // 1 - PR01
          }
          If (LGreaterEqual(TCNT, 3))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 2)), 1))  // 2 - PR02
          }
          If (LGreaterEqual(TCNT, 4))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 3)), 1))  // 3 - PR03
          }
          If (LGreaterEqual(TCNT, 5))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 4)), 1))  // 4 - PR04
          }
          If (LGreaterEqual(TCNT, 6))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 5)), 1))  // 5 - PR05
          }
          If (LGreaterEqual(TCNT, 7))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 6)), 1))  // 6 - PR06
          }
          If (LGreaterEqual(TCNT, 8))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 7)), 1))  // 7 - PR07
          }
          If (LGreaterEqual(TCNT, 9))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 56)), 1))  // 56 - PR08
          }
          If (LGreaterEqual(TCNT, 10))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 57)), 1))  // 57 - PR09
          }
          If (LGreaterEqual(TCNT, 11))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 58)), 1))  // 58 - PR10
          }
          If (LGreaterEqual(TCNT, 12))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 59)), 1))  // 59 - PR11
          }
          If (LGreaterEqual(TCNT, 13))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 60)), 1))  // 60 - PR12
          }
          If (LGreaterEqual(TCNT, 14))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 61)), 1))  // 61 - PR13
          }
          If (LGreaterEqual(TCNT, 15))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 62)), 1))  // 62 - PR14
          }
          If (LGreaterEqual(TCNT, 16))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 63)), 1))  // 63 - PR15
          }

          If(LEqual(And(PEPC, BIT10), 0)) // PEPC Bit[10] - En/Dis CPU
          {
            // Disabled PEP PR0
            Store (0x00, Index (DeRefOf(Index (DEVY, 0)), 1))   // 0 -  PR00
            Store (0x00, Index (DeRefOf(Index (DEVY, 1)), 1))   // 1 -  PR01
            Store (0x00, Index (DeRefOf(Index (DEVY, 2)), 1))   // 2 -  PR02
            Store (0x00, Index (DeRefOf(Index (DEVY, 3)), 1))   // 3 -  PR03
            Store (0x00, Index (DeRefOf(Index (DEVY, 4)), 1))   // 4 -  PR04
            Store (0x00, Index (DeRefOf(Index (DEVY, 5)), 1))   // 5 -  PR05
            Store (0x00, Index (DeRefOf(Index (DEVY, 6)), 1))   // 6 -  PR06
            Store (0x00, Index (DeRefOf(Index (DEVY, 7)), 1))   // 7 -  PR07
            Store (0x00, Index (DeRefOf(Index (DEVY, 56)), 1))  // 56 - PR08
            Store (0x00, Index (DeRefOf(Index (DEVY, 57)), 1))  // 57 - PR09
            Store (0x00, Index (DeRefOf(Index (DEVY, 58)), 1))  // 58 - PR10
            Store (0x00, Index (DeRefOf(Index (DEVY, 59)), 1))  // 59 - PR11
            Store (0x00, Index (DeRefOf(Index (DEVY, 60)), 1))  // 60 - PR12
            Store (0x00, Index (DeRefOf(Index (DEVY, 61)), 1))  // 61 - PR13
            Store (0x00, Index (DeRefOf(Index (DEVY, 62)), 1))  // 62 - PR14
            Store (0x00, Index (DeRefOf(Index (DEVY, 63)), 1))  // 63 - PR15
          }

          If(LEqual(And(PEPC, BIT11), 0)) // PEPC Bit[11] - En/Dis eMMC
          {
            // Disabled eMMC
            Store (0x00, Index (DeRefOf(Index (DEVY, 16)), 1)) // 16 - EMMC
          }

          If(LEqual(And(PEPC, BIT12), 0)) // PEPC Bit[12] - En/Dis SDXC
          {
            // Disabled SDXC
            Store (0x00, Index (DeRefOf(Index (DEVY, 17)), 1)) // 17 - SCS SD Card (PCI Mode)
          }

          If(LEqual(And(PEPC, BIT13), 0)) { // PEPC Bit[13] - En/Dis I2C2
            Store (0x00, Index (DeRefOf(Index (DEVY, 18)), 1)) // 18 - I2C2
          }

          If(LEqual(And(PEPC, BIT14),0)) { // PEPC Bit[14] - En/Dis I2C3
            Store (0x00, Index (DeRefOf(Index (DEVY, 19)), 1)) // 19 - I2C3
          }

          If(LEqual(And(PEPC, BIT15), 0)) { // PEPC Bit[15] - En/Dis I2C4
            Store (0x00, Index (DeRefOf(Index (DEVY, 20)), 1)) // 20 - I2C4
          }

          If(LEqual(And(PEPC, BIT16), 0)) { // PEPC Bit[16] - En/Dis I2C5
            Store (0x00, Index (DeRefOf(Index (DEVY, 21)), 1)) // 21 - I2C5
          }

          If(LEqual(And(PEPC, BIT17),0)) { // PEPC Bit[17] - En/Dis UART2
            Store (0x00, Index (DeRefOf(Index (DEVY, 22)), 1)) // 22 - UART2
          }

          If(LEqual(And(PEPC, BIT18),0)) { // PEPC Bit[18] - En/Dis SPI0
            Store (0x00, Index (DeRefOf(Index (DEVY, 23)), 1)) // 23 - SPI0
          }

          If(LEqual(And(PEPC, BIT19),0)) { // PEPC Bit[19] - En/Dis SPI1
            Store (0x00, Index (DeRefOf(Index (DEVY, 24)), 1)) // 24 - SPI1
          }

          If(LEqual(And(PEPC, BIT1),0)) { // PEPC Bit[1] - En/Dis ISP0
            Store (0x00, Index (DeRefOf(Index (DEVY, 45)), 1)) // 45 - ISP0
          }

          If(LGreaterEqual(OSYS,2015)){ // Add D3 uPEP constraint for PCIe-based WIFI on Win. Threshold
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP01.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(\_SB.PCI0.RP01.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 25)), 2)), 1)), 1)) // 25 - RP01
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 25)), 2)), 1)), 2)) // 25 - RP01
              Store (0x01, Index (DeRefOf(Index (DEVY, 25)), 1)) // 25 - RP01
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.	    
            If(\_SB.PCI0.RP02.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 26)), 2)), 1)), 1)) // 26 - RP02
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 26)), 2)), 1)), 2)) // 26 - RP02
              Store (0x01, Index (DeRefOf(Index (DEVY, 26)), 1)) // 26 - RP02
            }
            If(\_SB.PCI0.RP03.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 27)), 2)), 1)), 1)) // 27 - RP03
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 27)), 2)), 1)), 2)) // 27 - RP03
              Store (0x01, Index (DeRefOf(Index (DEVY, 27)), 1)) // 27 - RP03
            }
            If(\_SB.PCI0.RP04.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 28)), 2)), 1)), 1)) // 28 - RP04
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 28)), 2)), 1)), 2)) // 28 - RP04
              Store (0x01, Index (DeRefOf(Index (DEVY, 28)), 1)) // 28 - RP04
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP05.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.	    
            If(\_SB.PCI0.RP05.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 29)), 2)), 1)), 1)) // 29 - RP05
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 29)), 2)), 1)), 2)) // 29 - RP05
              Store (0x01, Index (DeRefOf(Index (DEVY, 29)), 1)) // 29 - RP05
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(\_SB.PCI0.RP06.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 30)), 2)), 1)), 1)) // 30 - RP06
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 30)), 2)), 1)), 2)) // 30 - RP06
              Store (0x01, Index (DeRefOf(Index (DEVY, 30)), 1)) // 30 - RP06
            }
            If(\_SB.PCI0.RP07.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 31)), 2)), 1)), 1)) // 31 - RP07
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 31)), 2)), 1)), 2)) // 31 - RP07
              Store (0x01, Index (DeRefOf(Index (DEVY, 31)), 1)) // 31 - RP07
            }
            If(\_SB.PCI0.RP08.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 32)), 2)), 1)), 1)) // 32 - RP08
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 32)), 2)), 1)), 2)) // 32 - RP08
              Store (0x01, Index (DeRefOf(Index (DEVY, 32)), 1)) // 32 - RP08
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP09.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.	    
            If(\_SB.PCI0.RP09.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 33)), 2)), 1)), 1)) // 33 - RP09
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 33)), 2)), 1)), 2)) // 33 - RP09
              Store (0x01, Index (DeRefOf(Index (DEVY, 33)), 1)) // 33 - RP09
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.	    
            If(\_SB.PCI0.RP10.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 34)), 2)), 1)), 1)) // 34 - RP10
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 34)), 2)), 1)), 2)) // 34 - RP10
              Store (0x01, Index (DeRefOf(Index (DEVY, 34)), 1)) // 34 - RP10
            }
            If(\_SB.PCI0.RP11.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 35)), 2)), 1)), 1)) // 35 - RP11
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 35)), 2)), 1)), 2)) // 35 - RP11
              Store (0x01, Index (DeRefOf(Index (DEVY, 35)), 1)) // 35 - RP11
            }
            If(\_SB.PCI0.RP12.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 36)), 2)), 1)), 1)) // 36 - RP12
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 36)), 2)), 1)), 2)) // 36 - RP12
              Store (0x01, Index (DeRefOf(Index (DEVY, 36)), 1)) // 36 - RP12
            }
            If(\_SB.PCI0.RP13.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 37)), 2)), 1)), 1)) // 37 - RP13
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 37)), 2)), 1)), 2)) // 37 - RP13
              Store (0x01, Index (DeRefOf(Index (DEVY, 37)), 1)) // 37 - RP13
            }
            If(\_SB.PCI0.RP14.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 38)), 2)), 1)), 1)) // 38 - RP14
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 38)), 2)), 1)), 2)) // 38 - RP14
              Store (0x01, Index (DeRefOf(Index (DEVY, 38)), 1)) // 38 - RP14
            }
            If(\_SB.PCI0.RP15.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 39)), 2)), 1)), 1)) // 39 - RP15
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 39)), 2)), 1)), 2)) // 39 - RP15
              Store (0x01, Index (DeRefOf(Index (DEVY, 39)), 1)) // 39 - RP15
            }
            If(\_SB.PCI0.RP16.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 40)), 2)), 1)), 1)) // 40 - RP16
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 40)), 2)), 1)), 2)) // 40 - RP16
              Store (0x01, Index (DeRefOf(Index (DEVY, 40)), 1)) // 40 - RP16
            }
            If(\_SB.PCI0.RP17.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 41)), 2)), 1)), 1)) // 41 - RP17
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 41)), 2)), 1)), 2)) // 41 - RP17
              Store (0x01, Index (DeRefOf(Index (DEVY, 41)), 1)) // 41 - RP17
            }
            If(\_SB.PCI0.RP18.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 42)), 2)), 1)), 1)) // 42 - RP18
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 42)), 2)), 1)), 2)) // 42 - RP18
              Store (0x01, Index (DeRefOf(Index (DEVY, 42)), 1)) // 42 - RP18
            }
            If(\_SB.PCI0.RP19.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 43)), 2)), 1)), 1)) // 43 - RP19
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 43)), 2)), 1)), 2)) // 43 - RP19
              Store (0x01, Index (DeRefOf(Index (DEVY, 43)), 1)) // 43 - RP19
            }
            If(\_SB.PCI0.RP20.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 44)), 2)), 1)), 1)) // 44 - RP20
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 44)), 2)), 1)), 2)) // 44 - RP20
              Store (0x01, Index (DeRefOf(Index (DEVY, 44)), 1)) // 44 - RP20
            }
          }

          // Add D3 uPEP constraint for PCIe-based WIGIG on Win 10 or above only if enabled via BIOS settings

          If(LAnd(LGreaterEqual(OSYS,2015),LEqual(PWIG, 1))){
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP01.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(\_SB.PCI0.RP01.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 25)), 1)) // 25 - RP01
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(\_SB.PCI0.RP02.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 26)), 1)) // 26 - RP02
            }
            If(\_SB.PCI0.RP03.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 27)), 1)) // 27 - RP03
            }
            If(\_SB.PCI0.RP04.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 28)), 1)) // 28 - RP04
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP01.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(\_SB.PCI0.RP05.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 29)), 1)) // 29 - RP05
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(\_SB.PCI0.RP06.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 30)), 1)) // 30 - RP06
            }
            If(\_SB.PCI0.RP07.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 31)), 1)) // 31 - RP07
            }
            If(\_SB.PCI0.RP08.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 32)), 1)) // 32 - RP08
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            If (CondRefOf(\_SB.PCI0.RP01.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(\_SB.PCI0.RP09.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 33)), 1)) // 33 - RP09
            }
// AMI_OVERRIDE_START - For Hybric Graphics feature.
            }
// AMI_OVERRIDE_END - For Hybric Graphics feature.
            If(\_SB.PCI0.RP10.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 34)), 1)) // 34 - RP10
            }
            If(\_SB.PCI0.RP11.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 35)), 1)) // 35 - RP11
            }
            If(\_SB.PCI0.RP12.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 36)), 1)) // 36 - RP12
            }
            If(\_SB.PCI0.RP13.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 37)), 1)) // 33 - RP13
            }
            If(\_SB.PCI0.RP14.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 38)), 1)) // 34 - RP14
            }
            If(\_SB.PCI0.RP15.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 39)), 1)) // 39 - RP15
            }
            If(\_SB.PCI0.RP16.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 40)), 1)) // 40 - RP16
            }
            If(\_SB.PCI0.RP17.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 41)), 1)) // 41 - RP17
            }
            If(\_SB.PCI0.RP18.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 42)), 1)) // 42 - RP18
            }
            If(\_SB.PCI0.RP19.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 43)), 1)) // 43 - RP19
            }
            If(\_SB.PCI0.RP20.PXSX.WGST()){
              Store (0x01, Index (DeRefOf(Index (DEVY, 44)), 1)) // 44 - RP20
            }
          }

          If(LNotEqual(And(PEP1, BIT0), 0)) // PEP1 Bit[0] - En/Dis SkyCamSensors
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 73)), 1)) // 73 - SkyCamSensors
          }

          Return(DEVY)
        }

        // BCCD
        If(LEqual(Arg2, 2))
        {
          Return(BCCD)
        }

        // Screen off notification
        If(LEqual(Arg2, 0x3))
        {
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is turning the screen off
            // No such method defined as of now. TBD for future platform generations.
          }
        }

        // Screen on notification
        If(LEqual(Arg2, 0x4))
        {
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is turning the screen on
            // No such method defined as of now. TBD for future platform generations.
            If (LAnd(LEqual(\RTBT, 1), CondRefOf(\_GPE.TBNF))) { // TBT RTD3 support is enabled and TBNF exists
              \_GPE.TBNF() // TBT Nofity to wake up TBT attached display.
            }
          }
        }

        // resiliency phase entry (deep standby entry)
        If(LEqual(Arg2, 0x5))
        {
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \GUAM(1) // 0x01 - Power State Standby (CS Resiliency Entry)
          }
          \_SB.PCI0.LPCB.H_EC.ECNT(1) // 0x01 - Call method to notify EC of Idle Resiliency entry
        }

        // resiliency phase exit (deep standby exit)
        If(LEqual(Arg2, 0x6))
        {
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \GUAM(0) // 0x00 - Power State On (CS Resiliency Exit)
          }
          \_SB.PCI0.LPCB.H_EC.ECNT(0) // 0x00 - Call method to notify EC of Idle Resiliency exit
        }
      }// If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
      Return (Buffer() {0})
    } // Method(_DSM)
  } //device (PEPD)
} // End Scope(\_SB)
