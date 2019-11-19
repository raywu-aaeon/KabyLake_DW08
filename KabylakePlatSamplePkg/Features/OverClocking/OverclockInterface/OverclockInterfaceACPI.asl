/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2012 - 2018 Intel Corporation.

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
// Defined as an SSDT to be able to dynamically load based on BIOS
// setup options
//
DefinitionBlock (
  "OverclockInterfaceAcpi.aml",
  "SSDT",
  0x02,
  "Intel",
  "PerfTune",
  0x1000
  )

{
  Scope(\_SB)
  {
    // External References to the actual data locations that stores
    // various temperature and Fan Speed values from EC.
    //

    // externs for TSDD
    External(\_SB.PCI0.LPCB.H_EC.DIM0)    // DIMM0 Temperature
    External(\_SB.PCI0.LPCB.H_EC.DIM1)    // DIMM1 Temperature
    External(\_TZ.TZ01._TMP, MethodObj)

    // externs for FSDD
    External(\_SB.PCI0.LPCB.H_EC.CFSP)    // CPU Fan speed

    External(\_SB.PCI0.LPCB.H_EC.ECRD, MethodObj)    // EC Read Method
    External(\_SB.PCI0.LPCB.H_EC.ECWT, MethodObj)    // EC Write Method
    // Embedded Controller Availability Flag.
    External(\TSOD, IntObj)
    External(\ECON, IntObj)
    External(XTUB)
    External(XTUS)
    External(XMPB)
    External(DDRF)
    External(XSMI)
    External(\ADBG, MethodObj)
//AMI_OVERRIDE_OVERCLOCK0000_START    
    External(PFTU)
//AMI_OVERRIDE_OVERCLOCK0000_END    

    Device(PTMD)
    {
      Name(_HID, EISAID("INT3394"))
      Name(_CID, EISAID("PNP0C02"))
      Name(IVER, 0x00010000)
      Name(SIZE,0x604)
//AMI_OVERRIDE_START - Modfiy ControlID buffer
      Name(CIDB,0x674)
//AMI_OVERRIDE_END - Modfiy ControlID buffer

      //
      //  Get Available Contol Id's
      //
      Method(GACI, 0x0, NotSerialized, 0, PkgObj)
      {
       Name(RPKG, Package(0x2){}) // Return Package

       Store(0x00, Index(RPKG, 0)) // Error Code

        If (LNotEqual(XTUB, Zero))
        {
         ADBG("XTUB")
         ADBG(XTUB)
         ADBG("XTUS")
         ADBG(XTUS)
#if 0 //AMI_OVERRIDE_START - Modfiy ControlID buffer
         OperationRegion (XNVS, SystemMemory, XTUB, SIZE)
#else
         OperationRegion (XNVS, SystemMemory, XTUB, CIDB)
#endif //AMI_OVERRIDE_END - Modfiy ControlID buffer
           Field (XNVS, ByteAcc, NoLock, Preserve) {
#if 0 //AMI_OVERRIDE_START - Modfiy ControlID buffer
            XBUF, 0x32C0              // Total Controls * 28 * 8, BitLength.
#else
            XBUF, 0x33A0              // Total Controls * 28 * 8, BitLength.
#endif //AMI_OVERRIDE_END - Modfiy ControlID buffer
           }
         Name(TEMP, Buffer(XTUS){})
         Store(XBUF,TEMP)
         Store( TEMP, Index(RPKG, 1)) // Data buffer
       }
       Else
       {
        ADBG("XTUB ZERO")
        Store( 0, Index(RPKG, 1)) // Data buffer
       }
       Return(RPKG)
      }

      //
      //  Get Discrete Supported Values
      //
      Method(GDSV, 0x1, Serialized, 0, PkgObj, IntObj)
      {
        If(LEqual(Arg0, 5))
        {
          Return(Package(0x2)
          {
            0x0, // Error code
            Buffer()
            {
// AMI_OVERRIDE_START - Fix memory voltage value not match with setup
//              0, 0, 0, 0,    // Data value
//              0, 0, 0, 0,    // Display value
//              0x4c, 4, 0, 0,    // Data value
//              0x4c, 4, 0, 0, // Display value (1110)
//              0x7e, 4, 0, 0,    // Data value
//              0x7e, 4, 0, 0, // Display value (1150)
//              0xb0, 4, 0, 0,    // Data value
//              0xb0, 4, 0, 0, // Display value (1200)
//              0xe2, 4, 0, 0,    // Data value
//              0xe2, 4, 0, 0, // Display value (1250)
//              0x14, 5, 0, 0,    // Data value
//              0x14, 5, 0, 0, // Display value (1300)
//              0x46, 5, 0, 0,    // Data value
//              0x46, 5, 0, 0, // Display value (1350)
//              0x78, 5, 0, 0,    // Data value
//              0x78, 5, 0, 0, // Display value (1400)
//              0xaa, 5, 0, 0,    // Data value
//              0xaa, 5, 0, 0, // Display value (1450)
//              0xdc, 5, 0, 0,    // Data value
//              0xdc, 5, 0, 0, // Display value (1500)
//              0x0e, 6, 0, 0,   // Data value
//              0x0e, 6, 0, 0, // Display valuee (1550)
//              0x40, 6, 0, 0,   // Data value
//              0x40, 6, 0, 0, // Display value (1600)
//              0x72, 6, 0, 0,   // Data value
//              0x72, 6, 0, 0, // Display value (1650)
             76, 4, 0, 0,  // Display value 1100
             76, 4, 0, 0,  // Data value
             126, 4, 0, 0,  // Display value 1150
             126, 4, 0, 0,  // Data value
             176, 4, 0, 0,  // Data value 1200
             176, 4, 0, 0,  // Display value
             226, 4, 0, 0,  // Data value 1250
             226, 4, 0, 0,  // Display value
             20, 5, 0, 0,  // Data value 1300
             20, 5, 0, 0,  // Display value            
             70, 5, 0, 0,  // Data value 1350
             70, 5, 0, 0,  // Display value
             120, 5, 0, 0,  // Data value 1400
             120, 5, 0, 0,  // Display value
             170, 5, 0, 0,  // Data value 1450
             170, 5, 0, 0,  // Display value
             220, 5, 0, 0,  // Data value 1500
             220, 5, 0, 0,  // Display value
             14, 6, 0, 0,  // Data value 1550
             14, 6, 0, 0,  // Display value
             64, 6, 0, 0,  // Data value 1600
             64, 6, 0, 0,  // Display value
             114, 6, 0, 0,  // Data value 1650
             114, 6, 0, 0,  // Display value 
// AMI_OVERRIDE_END - Fix memory voltage value not match with setup           
            }
          })
        }
//AMI_OVERRIDE_START - Fix EIP229470
        If(LEqual(Arg0, 19))
        {
          Return (Package(0x2)
          {
            0x0, //Error code
            Buffer()
            {
                0, 0, 0, 0,      // Data value
                0, 0, 0, 0,      // Display value (AUTO)
                0x2b, 4, 0, 0,   // Data value
                0x2b, 4, 0, 0,   // Display value (1067)
                0xb0, 4, 0, 0,   // Data value
                0xb0, 4, 0, 0,   // Display value (1200)
                0x35, 5, 0, 0,   // Data value
                0x35, 5, 0, 0,   // Display value (1333)
                0x78, 5, 0, 0,   // Data value
                0x78, 5, 0, 0,   // Display value (1400)
                0x40, 6, 0, 0,   // Data value
                0x40, 6, 0, 0,   // Display value (1600)
                0x08, 7, 0, 0,   // Data value
                0x08, 7, 0, 0,   // Display value (1800)
                0x4b, 7, 0, 0,   // Data value
                0x4b, 7, 0, 0,   // Display value (1867)
                0xd0, 7, 0, 0,   // Data value
                0xd0, 7, 0, 0,   // Display value (2000)
                0x55, 8, 0, 0,   // Data value
                0x55, 8, 0, 0,   // Display value (2133)
                0x98, 8, 0, 0,   // Data value
                0x98, 8, 0, 0,   // Display value (2200)
                0x60, 9, 0, 0,   // Data value
                0x60, 9, 0, 0,   // Display value (2400)
                0x28, 0xA, 0, 0,   // Data value
                0x28, 0xA, 0, 0,   // Display value (2600)
                0x6b, 0xA, 0, 0,   // Data value
                0x6b, 0xA, 0, 0,   // Display value (2667)
                0xF0, 0xA, 0, 0,   // Data value
                0xF0, 0xA, 0, 0,   // Display value (2800)
                0x75, 0xB, 0, 0,   // Data value
                0x75, 0xB, 0, 0,   // Display value (2933)
                0xB8, 0xB, 0, 0,   // Data value
                0xB8, 0xB, 0, 0,   // Display value (3000)
                0x80, 0xC, 0, 0,   // Data value
                0x80, 0xC, 0, 0,   // Display value (3200)
                0x48, 0xD, 0, 0,   // Data value
                0x48, 0xD, 0, 0,   // Display value (3400)
                0x8B, 0xD, 0, 0,   // Data value
                0x8B, 0xD, 0, 0,   // Display value (3467)
                0x10, 0xE, 0, 0,   // Data value
                0x10, 0xE, 0, 0,   // Display value (3600)
                0x95, 0xE, 0, 0,   // Data value
                0x95, 0xE, 0, 0,   // Display value (3733)
                0xD8, 0xE, 0, 0,   // Data value
                0xD8, 0xE, 0, 0,   // Display value (3800)
                0xA0, 0xF, 0, 0,   // Data value
                0xA0, 0xF, 0, 0,   // Display value (4000)
                0x25, 0x10, 0, 0,   // Data value
                0x25, 0x10, 0, 0,   // Display value (4133)
                0x68, 0x10, 0, 0,   // Data value
                0x68, 0x10, 0, 0,   // Display value (4200)
                0xAB, 0x10, 0, 0,   // Data value
                0xAB, 0x10, 0, 0,   // Display value (4267)
                0x30, 0x11, 0, 0,   // Data value
                0x30, 0x11, 0, 0,   // Display value (4400)
                0xB5, 0x11, 0, 0,   // Data value
                0xB5, 0x11, 0, 0,   // Display value (4533)
                0xF8, 0x11, 0, 0,   // Data value
                0xF8, 0x11, 0, 0,   // Display value (4600)
                0xC0, 0x12, 0, 0,   // Data value
                0xC0, 0x12, 0, 0,   // Display value (4800)
                0x88, 0x13, 0, 0,   // Data value
                0x88, 0x13, 0, 0,   // Display value (5000)
                0xCB, 0x13, 0, 0,   // Data value
                0xCB, 0x13, 0, 0,   // Display value (5067)
                0x50, 0x14, 0, 0,   // Data value
                0x50, 0x14, 0, 0,   // Display value (5200)
                0xD5, 0x14, 0, 0,   // Data value
                0xD5, 0x14, 0, 0,   // Display value (5333)
                0x18, 0x15, 0, 0,   // Data value
                0x18, 0x15, 0, 0,   // Display value (5400)
                0xE0, 0x15, 0, 0,   // Data value
                0xE0, 0x15, 0, 0,   // Display value (5600)
                0xA8, 0x16, 0, 0,   // Data value
                0xA8, 0x16, 0, 0,   // Display value (5800)
                0xEB, 0x16, 0, 0,   // Data value
                0xEB, 0x16, 0, 0,   // Display value (5867)
                0x70, 0x17, 0, 0,   // Data value
                0x70, 0x17, 0, 0,   // Display value (6000)
                0xF5, 0x17, 0, 0,   // Data value
                0xF5, 0x17, 0, 0,   // Display value (6133)
                0x38, 0x18, 0, 0,   // Data value
                0x38, 0x18, 0, 0,   // Display value  (6200)
            }
          })
        }
//AMI_OVERRIDE_END - Fix EIP229470

        If(LEqual(Arg0, 19))
        {
          ADBG("DDR MULT")
          If(LEqual(DDRF,0))
          {
            ADBG("DDRREFCLK 0")
            Return (Package(0x2)
            {
              0x0, //Error code
              Buffer()
              {
                0, 0, 0, 0,      // Data value
                0, 0, 0, 0,      // Display value (AUTO)
                0x2b, 4, 0, 0,   // Data value
                0x2b, 4, 0, 0,   // Display value (1067)
                0xb0, 4, 0, 0,   // Data value
                0xb0, 4, 0, 0,   // Display value (1200)
                0x35, 5, 0, 0,   // Data value
                0x35, 5, 0, 0,   // Display value (1333)
                0x78, 5, 0, 0,   // Data value
                0x78, 5, 0, 0,   // Display value (1400)
                0x40, 6, 0, 0,   // Data value
                0x40, 6, 0, 0,   // Display value (1600)
                0x08, 7, 0, 0,   // Data value
                0x08, 7, 0, 0,   // Display value (1800)
                0x4b, 7, 0, 0,   // Data value
                0x4b, 7, 0, 0,   // Display value (1867)
                0xd0, 7, 0, 0,   // Data value
                0xd0, 7, 0, 0,   // Display value (2000)
                0x55, 8, 0, 0,   // Data value
                0x55, 8, 0, 0,   // Display value (2133)
                0x98, 8, 0, 0,   // Data value
                0x98, 8, 0, 0,   // Display value (2200)
                0x60, 9, 0, 0,   // Data value
                0x60, 9, 0, 0,   // Display value (2400)
                0x28, 0xA, 0, 0,   // Data value
                0x28, 0xA, 0, 0,   // Display value (2600)
                0x6b, 0xA, 0, 0,   // Data value
                0x6b, 0xA, 0, 0,   // Display value (2667)
                0xF0, 0xA, 0, 0,   // Data value
                0xF0, 0xA, 0, 0,   // Display value (2800)
                0x75, 0xB, 0, 0,   // Data value
                0x75, 0xB, 0, 0,   // Display value (2933)
                0xB8, 0xB, 0, 0,   // Data value
                0xB8, 0xB, 0, 0,   // Display value (3000)
                0x80, 0xC, 0, 0,   // Data value
                0x80, 0xC, 0, 0,   // Display value (3200)
                0x48, 0xD, 0, 0,   // Data value
                0x48, 0xD, 0, 0,   // Display value (3400)
                0x8B, 0xD, 0, 0,   // Data value
                0x8B, 0xD, 0, 0,   // Display value (3467)
                0x10, 0xE, 0, 0,   // Data value
                0x10, 0xE, 0, 0,   // Display value (3600)
                0x95, 0xE, 0, 0,   // Data value
                0x95, 0xE, 0, 0,   // Display value (3733)
                0xD8, 0xE, 0, 0,   // Data value
                0xD8, 0xE, 0, 0,   // Display value (3800)
                0xA0, 0xF, 0, 0,   // Data value
                0xA0, 0xF, 0, 0,   // Display value (4000)
                0x25, 0x10, 0, 0,   // Data value
                0x25, 0x10, 0, 0,   // Display value (4133)
                0x68, 0x10, 0, 0,   // Data value
                0x68, 0x10, 0, 0,   // Display value (4200)
                0xAB, 0x10, 0, 0,   // Data value
                0xAB, 0x10, 0, 0,   // Display value (4267)
                0x30, 0x11, 0, 0,   // Data value
                0x30, 0x11, 0, 0,   // Display value (4400)
                0xB5, 0x11, 0, 0,   // Data value
                0xB5, 0x11, 0, 0,   // Display value (4533)
                0xF8, 0x11, 0, 0,   // Data value
                0xF8, 0x11, 0, 0,   // Display value (4600)
                0xC0, 0x12, 0, 0,   // Data value
                0xC0, 0x12, 0, 0,   // Display value (4800)
                0x88, 0x13, 0, 0,   // Data value
                0x88, 0x13, 0, 0,   // Display value (5000)
                0xCB, 0x13, 0, 0,   // Data value
                0xCB, 0x13, 0, 0,   // Display value (5067)
                0x50, 0x14, 0, 0,   // Data value
                0x50, 0x14, 0, 0,   // Display value (5200)
                0xD5, 0x14, 0, 0,   // Data value
                0xD5, 0x14, 0, 0,   // Display value (5333)
                0x18, 0x15, 0, 0,   // Data value
                0x18, 0x15, 0, 0,   // Display value (5400)
                0xE0, 0x15, 0, 0,   // Data value
                0xE0, 0x15, 0, 0,   // Display value (5600)
                0xA8, 0x16, 0, 0,   // Data value
                0xA8, 0x16, 0, 0,   // Display value (5800)
                0xEB, 0x16, 0, 0,   // Data value
                0xEB, 0x16, 0, 0,   // Display value (5867)
                0x70, 0x17, 0, 0,   // Data value
                0x70, 0x17, 0, 0,   // Display value (6000)
                0xF5, 0x17, 0, 0,   // Data value
                0xF5, 0x17, 0, 0,   // Display value (6133)
                0x38, 0x18, 0, 0,   // Data value
                0x38, 0x18, 0, 0,   // Display value  (6200)
              }
            })
          }
          ElseIf(LEqual(DDRF,1))
          {
            ADBG("DDRREFCLK 1")
            Return(Package(0x2)
            {
              0x0, // Error code
              Buffer()
              {
                0, 0, 0, 0,      // Data value
                0, 0, 0, 0,      // Display value (AUTO)
                0x2B, 4, 0, 0,   // Data value
                0x2B, 4, 0, 0,   // Display value (1067)
                0x35, 5, 0, 0,   // Data value
                0x35, 5, 0, 0,   // Display value (1333)
                0x40, 6, 0, 0,   // Data value
                0x40, 6, 0, 0,   // Display value (1600)
                0x4B, 7, 0, 0,   // Data value
                0x4B, 7, 0, 0,   // Display value (1867)
                0x55, 8, 0, 0,   // Data value
                0x55, 8, 0, 0,   // Display vlaue (2133)
                0x60, 9, 0, 0,   // Data value
                0x60, 9, 0, 0,   // Display value (2400)
                0x6B, 0xA, 0, 0, // Data value
                0x6B, 0xA, 0, 0, // Display value (2667)
                0x75, 0xB, 0, 0, // Data value
                0x75, 0xB, 0, 0, // Display value (2933)
                0x80, 0xC, 0, 0, // Data value
                0x80, 0xC, 0, 0, // Display value (3200)
                0x8B, 0xD, 0, 0, // Data value
                0x8B, 0xD, 0, 0, // Display value (3467)
                0x95, 0xE, 0, 0, // Data value
                0x95, 0xE, 0, 0, // Display value (3733)
                0xA0, 0xF, 0, 0, // Data value
                0xA0, 0xF, 0, 0, // Display value (4000)
                0xAB, 0x10, 0, 0, // Data value
                0xAB, 0x10, 0, 0, // Display value (4267)
                0xB5, 0x11, 0, 0, // Data value
                0xB5, 0x11, 0, 0, // Display value (4533)
                0xC0, 0x12, 0, 0, // Data value
                0xC0, 0x12, 0, 0, // Display value (4800)
                0xCB, 0x13, 0, 0, // Data value
                0xCB, 0x13, 0, 0, // Display value (5067)
                0xD5, 0x14, 0, 0, // Data value
                0xD5, 0x14, 0, 0, // Display value (5333)
                0xE0, 0x15, 0, 0, // Data value
                0xE0, 0x15, 0, 0, // Display value (5600)
                0xEB, 0x16, 0, 0, // Data value
                0xEB, 0x16, 0, 0, // Display value (5867)
                0xF5, 0x17, 0, 0, // Data value
                0xF5, 0x17, 0, 0, // Display value (6133)
              }
            })
          }
          ElseIf(LEqual(DDRF,2))
          {
            ADBG("DDRREFCLK 2")
            Return(Package(0x2)
            {
              0x0, // Error code
              Buffer()
              {
                0, 0, 0, 0,      // Data value
                0, 0, 0, 0,      // Display value (AUTO)
                0xB0, 4, 0, 0,   // Data value
                0xB0, 4, 0, 0,   // Display value (1200)
                0x78, 5, 0, 0,   // Data value
                0x78, 5, 0, 0,   // Display value (1400)
                0x40, 6, 0, 0,   // Data value
                0x40, 6, 0, 0,   // Display value (1600)
                0x08, 7, 0, 0,   // Data value
                0x08, 7, 0, 0,   // Display value (1800)
                0xD0, 7, 0, 0,   // Data value
                0xD0, 7, 0, 0,   // Display vlaue (2000)
                0x98, 8, 0, 0,   // Data value
                0x98, 8, 0, 0,   // Display value (2200)
                0x60, 9, 0, 0,   // Data value
                0x60, 9, 0, 0,   // Display value (2400)
                0x28, 0xA, 0, 0, // Data value
                0x28, 0xA, 0, 0, // Display value (2600)
                0xF0, 0xA, 0, 0, // Data value
                0xF0, 0xA, 0, 0, // Display value (2800)
                0xB8, 0xB, 0, 0, // Data value
                0xB8, 0xB, 0, 0, // Display value (3000)
                0x1C, 0xC, 0, 0, // Data value
                0x1C, 0xC, 0, 0, // Display value (3100)
                0x80, 0xC, 0, 0, // Data value
                0x80, 0xC, 0, 0, // Display value (3200)
                0x48, 0xD, 0, 0, // Data value
                0x48, 0xD, 0, 0, // Display value (3400)
                0x10, 0xE, 0, 0, // Data value
                0x10, 0xE, 0, 0, // Display value (3600)
                0xD8, 0xE, 0, 0, // Data value
                0xD8, 0xE, 0, 0, // Display value (3800)
                0xA0, 0xF, 0, 0, // Data value
                0xA0, 0xF, 0, 0, // Display value (4000)
                0x68, 0x10, 0, 0, // Data value
                0x68, 0x10, 0, 0, // Display value (4200)
                0x30, 0x11, 0, 0, // Data value
                0x30, 0x11, 0, 0, // Display value (4400)
                0xF8, 0x11, 0, 0, // Data value
                0xF8, 0x11, 0, 0, // Display value (4600)
                0xC0, 0x12, 0, 0, // Data value
                0xC0, 0x12, 0, 0, // Display value (4800)
                0x88, 0x13, 0, 0, // Data value
                0x88, 0x13, 0, 0, // Display value (5000)
                0x50, 0x14, 0, 0, // Data value
                0x50, 0x14, 0, 0, // Display value (5200)
                0x18, 0x15, 0, 0, // Data value
                0x18, 0x15, 0, 0, // Display value (5400)
                0xE0, 0x15, 0, 0, // Data value
                0xE0, 0x15, 0, 0, // Display value (5600)
                0xA8, 0x16, 0, 0, // Data value
                0xA8, 0x16, 0, 0, // Display value (5800)
                0x70, 0x17, 0, 0, // Data value
                0x70, 0x17, 0, 0, // Display value (6000)
                0x38, 0x18, 0, 0, // Data value
                0x38, 0x18, 0, 0, // Display value (6200)
              }
            })
          }
          ADBG("DDR EXIT")
        }

        If(LEqual(Arg0,11))
        {
          Return(Package(0x2)
          {
            0x0, // Error code
            Buffer()
            {
              0, 0, 0, 0,  // Data value
              0, 0, 0, 0,  // Display value
              5, 0, 0, 0,  // Data value
              5, 0, 0, 0,  // Display value
              6, 0, 0, 0,  // Data value
              6, 0, 0, 0,  // Display value
              7, 0, 0, 0,  // Data value
              7, 0, 0, 0,  // Display value
              8, 0, 0, 0,  // Data value
              8, 0, 0, 0,  // Display value
              10, 0, 0, 0, // Data value
              0xA,0, 0, 0, // Display value
              12, 0, 0, 0, // Data value
              0xC,0, 0, 0, // Display value
              14, 0, 0, 0, // Data Value
              0xE,0, 0,0,  // Display Value
              16, 0, 0, 0, // Data Value
              0x10,0,0,0,  // Display Value
              18, 0, 0, 0, // Data Value
              0x12,0,0,0,  // Display Value
              20, 0, 0, 0, // Data Value
              0x14,0,0,0,  // Display Value
              24, 0, 0, 0, // Data Value
              0x18,0,0,0,  // Display Value
            }
          })
        }
//AMI_OVERRIDE_START - Add Extended Time Window table
        If(LEqual(Arg0,0x42)) 
        {
          Return(Package(0x2)
          {
            0x0, // Error code
            Buffer()
            {
              0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
              0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
              0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
              0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
              0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 
              0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
              0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 
              0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 
              0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 
              0x0C, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 
              0x0E, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 
              0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 
              0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 
              0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 
              0x1C, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 
              0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 
              0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 
              0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 
              0x38, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 
              0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 
              0x50, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 
              0x60, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 
              0x70, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 
              0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00
            }
          })
        }
//AMI_OVERRIDE_END - Add Extended Time Window table

        If(LEqual(Arg0,73))
        {
          Return(Package(0x2)
          {
            0x0, // Error code
            Buffer()
            {
              0, 0, 0, 0,    // Data value for auto
              0, 0, 0, 0,    // Display value
              1, 0, 0, 0,    // Data value
              0x85, 0, 0, 0, // Display value
              2, 0, 0, 0,    // Data value
              0x64, 0, 0, 0, // Display value
            }
          })
        }

        Return(Package(0x1)
        {
        0x01 // Error code for continuous settings
        })
      }

      //
      //  Get XMP Display Values
      //
      Method(GXDV, 0x1, Serialized, 0, PkgObj, IntObj)
      {
        If (LNotEqual(XMPB, Zero))
        {
          OperationRegion (XMPN, SystemMemory, XMPB, SIZE)
          Field (XMPN, ByteAcc, NoLock, Preserve) {
              XMP1, 0x240,              // Total Controls * 8 * 8, BitLength.
              XMP2, 0x240,              // Total Controls * 8 * 8, BitLength.
          }

          If(LEqual(Arg0, 1))
          {
            Name(XP_1, Package(0x2){}) // Return Package
            Store(0x00, Index(XP_1, 0)) // Error Code
            Store(XMP1, Index(XP_1, 1)) // Data buffer
            Return(XP_1)
          }
          If(LEqual(Arg0, 2))
          {
            Name(XP_2, Package(0x2){}) // Return Package
            Store(0x00, Index(XP_2, 0)) // Error Code
            Store(XMP2, Index(XP_2, 1)) // Data buffer
            Return(XP_2)
          }
        }
        Return(Package(0x1)
        {
          1 // Invalid profile type
        })
      }

      //
      //  Get SMI Command Value
      //
      Method(GSCV, 0x0, NotSerialized, 0, PkgObj)
      {
//AMI_OVERRIDE_START - Override SW SMI number     
//      Return(Package(0x1)
//      {
//      0x72
//      })
      	Return(PFTU)
//AMI_OVERRIDE_END - Override SW SMI number	
      }

      //
      // Get SMI Common Buffer
      //
      Method(GSCB, 0x0, NotSerialized, 0, PkgObj)
      {
      Return(XSMI)
      }

      //
      //  Control Device Read
      //
      Method(CDRD, 0x1, Serialized, 0, PkgObj, IntObj)
      {
      Return(Package(0x1)
      {
      0x01 // Non real time device read requested
      })
      }

      //
      //  Control Device Write
      //
      Method(CDWR, 0x2, Serialized, 0, PkgObj, {IntObj, IntObj})
      {
      Return(0x01) // Write to non real time device
      }

      Name(RPMV, Package()
      {
       //UsageId     //DeviceId       //Value     //Reserved
          0x01,         0x0007,          0,          0        //Cpu Fan Speed
      })

      Name(TMP1, Package()
      {
      //UsageId     //DeviceId      //Value     //Reserved
         0x01,         0x0002,         0,          0,       //Core Temprature
         0x05,         0x0004,         0,          0,       //CPU VR Temperature
         0x06,         0x0005,         0,          0,       // TS-on-DIMM Temperature
      })
      Name(TMP2, Package()
      {
       //UsageId     //DeviceId      //Value     //Reserved
          0x01,         0x0002,         0,          0,       //Core Temprature
          0x05,         0x0004,         0,          0,       //CPU VR Temperature
      })
      Name(TMP3, Package()
      {
       //UsageId     //DeviceId      //Value     //Reserved
          0x01,         0x0002,         0,          0,       //Core Temprature
      })

      //
      //  Temperature Sensor Data Dump
      //
      Method(TSDD)
      {
        If(LEqual(XTUS, 0))
        {
          Return (0)
        }
        If (\ECON)
        {
          If(\TSOD)
          {
            Store(\_TZ.TZ01._TMP, Index(TMP1, 2))
            Return(TMP1)
          }
          Else
          {
            Store(\_TZ.TZ01._TMP, Index(TMP2, 2))
            Return(TMP2)
          }//End of TSDD If
        }
        Else
        {
          Store(\_TZ.TZ01._TMP, Index(TMP3, 2))
          Return(TMP3)
        }//End of ECON If
      }//End of TSDD Method

      //
      //  Fan Sensor Data Dump
      //
      Method(FSDD)
      {
        If(LEqual(XTUS, 0))
        {
          Return (0)
        }
        If (\ECON)
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CFSP)), Index(RPMV, 2))
        }
        Return(RPMV)
      }

      //
      //  Sensor Data Sampling Period
      //
      Method (SDSP)
      {
        Return(10)      // Sampling period .
      }


    }// end of PTMD Device

  } // end \_SB scope
} // end SSDT
