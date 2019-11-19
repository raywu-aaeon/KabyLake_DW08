/** @file
  ACPI EINK App SSDT table

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

//
// Defined as an SSDT to be able to dynamically load based on BIOS
// setup options
//

DefinitionBlock (
    "EInkApp.aml",
    "SSDT",
    0x01,       // EIAD revision.
                // A Revision field value greater than or equal to 2 signifies that integers
                // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL",    // OEM ID (6 byte string)
    "EInkApp",  // OEM table ID  (8 byte string)
    0x0         // OEM version of DSDT table (4 byte Integer)
    )

// BEGIN OF ASL SCOPE
{
  External(\ADBG, MethodObj)
  External(\EIAP)
  External(\EIDF, IntObj)
  External(\_SB.GGOV, MethodObj)
  External(\_SB.SGOV, MethodObj)
  External(\_SB.PCI0.GEXP, DeviceObj) //IO expander Controller
  External(\_SB.PCI0.GEXP.SGEP, MethodObj) // set pin state
  External(\_SB.PCI0.GEXP.GEPS, MethodObj) // get pin state
  include("GpioAcpiDefines.h")

  Scope(\)
  {
    Device(EIAD)                  // EInk Application Device.
    {
      Name(_HID, EISAID("INT3399"))
      Name(_S0W, 0)

      Method(_STA, 0, Serialized)
      {
        ADBG("EIAD STA")
        If(LEqual(EIAP,1)) {
          Return(0x0F)
        } ElseIf(LEqual(EIDF,1)) { // BoardIdSkylakeRvp3, BoardIdSkylakeRvp7
          Return(0x0F)
        }
        Return(0x00)
      }

      //
      // _DSM : Device Specific Method supporting USB Sideband Deferring function
      //
      // Arg0: UUID Unique function identifier
      // Arg1: Integer Revision Level
      // Arg2: Integer Function Index
      // Arg3: Package Parameters
      //
      Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
      {
        // Define the Local Variables used throughout the method.
        Name(PECE, 0) // Power Control Enable.
        Name(PECD, 0) // Power Control Duration.
        Name(DFUE, 0) // DFU Control Enable.
        Name(DFUD, 0) // DFU Control Duration.
        Name(OLDV, 0) // Old value of both Power Enable and DFU Enable.
        Name(PECV, 0) // Power Control Value
        Name(DFUV, 0) // DFU Control Value

        // Compare passed in UUID to supported UUID.
        If (LEqual(Arg0, ToUUID ("ADF03C1F-EE76-4f23-9DEF-CDAE22A36ACF")))
        {
          If (LGreaterEqual(ToInteger(Arg1),1))        // Revision 1.
          {
            Switch (ToInteger(Arg2))            // Switch to Function Index.
            {
              //
              // Function 0, Query - return supported functions BitIndex.
              //
              Case (0)
              {
                ADBG("EIAD F:0")
                Return (Buffer() {0x0F})
              }

              //
              // Function 1, Power Control - Pass in 2 value package via Arg3
              //
              Case (1)
              {
                ADBG("EIAD F:1")
                If(LEqual(EIAP,0)) {
                  Store (DeRefOf (Index (Arg3, 0)), PECE)
                  Store (DeRefOf (Index (Arg3, 1)), PECD)
                  Store(\_SB.PCI0.GEXP.GEPS(0, 12), OLDV)  // Save off PWR_EN GPIO Value - EINK_PWREN_IOEXP (IOEX1, 12)
                  \_SB.PCI0.GEXP.SGEP(0, 12, PECE)         // Set value to PWR_EN GPIO - EINK_PWREN_IOEXP (IOEX1, 12)
                  If(LGreater(PECD, 0))                    // Test duration value
                  {
                    Sleep(PECD)                            // Delay for passed in duration.
                    \_SB.PCI0.GEXP.SGEP(0, 12, OLDV)       // Restore value to PWR_EN GPIO - EINK_PWREN_IOEXP (IOEX1, 12)
                  }
                }
                Return (0)
              } // End Case (1)

              //
              // Function 2, DFU Control - Pass in 2 value package via Arg3
              //
              Case (2)
              {
                ADBG("EIAD F:2")
                Store (DeRefOf (Index (Arg3, 0)), DFUE)
                Store (DeRefOf (Index (Arg3, 1)), DFUD)
                If(LEqual(EIAP,1)) {
                  Store(\_SB.GGOV(GPIO_SKL_LP_GPP_A21), OLDV)  // Save off DFU_EN# GPIO Value
                  \_SB.SGOV(GPIO_SKL_LP_GPP_A21, DFUE)         // Set Value to DFU_EN# GPIO
                } Else {                                       // BoardIdSkylakeRvp3, BoardIdSkylakeRvp7
                  Store(\_SB.GGOV(GPIO_SKL_LP_GPP_E3), OLDV)   // Save off DFU_EN# GPIO Value
                  \_SB.SGOV(GPIO_SKL_LP_GPP_E3, DFUE)          // Set Value to DFU_EN# GPIO
                }
                If (LGreater(DFUD, 0))                         // Test duration value
                {
                  Sleep(DFUD)                                  // Delay for passed in duration.
                  If(LEqual(EIAP,1)) {
                    \_SB.SGOV(GPIO_SKL_LP_GPP_A21, OLDV)       // Restore DFU_EN# GPIO Value
                  } Else {                                     // BoardIdSkylakeRvp3, BoardIdSkylakeRvp7
                    \_SB.SGOV(GPIO_SKL_LP_GPP_E3, OLDV)        // Restore Value to DFU_EN# GPIO
                  }
                }
                Return (0)
              } // End Case 2

              //
              // Function 3, Query Status - Return the current status of GPIO signals.
              //
              Case (3)
              {
                ADBG("EIAD F:3")
                If(LEqual(EIAP,1)) {
                  Store(\_SB.GGOV(GPIO_SKL_LP_GPP_A21), DFUV)
                  Store(0x1, PECV)                             // SDS do not have PWR_EN GPIO, EInk Power is always present
                } Else {                                       // BoardIdSkylakeRvp3, BoardIdSkylakeRvp7
                  Store(\_SB.GGOV(GPIO_SKL_LP_GPP_E3), DFUV)
                  Store(\_SB.PCI0.GEXP.GEPS(0, 12), PECV)
                }
                Return(Package(){PECV, DFUV})
              } // End Case 3
            } // End Function Index
            Return (0)
          }  // End Revision check
          Return (0)
        }  // End UUID check
        Return (Buffer() {0})
      }  // End _DSM Method
    }  // End Device
  }  // End Scope
}  // End DefinitionBlock Termlist