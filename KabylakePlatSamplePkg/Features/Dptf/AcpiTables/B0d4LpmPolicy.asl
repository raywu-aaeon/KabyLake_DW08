/** @file
  Intel ACPI Reference Code for Intel Low Power Mode

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

Scope(\_SB.IETM)
{
    Name (LPSP, Package ()
    {
    //
    // LPM Policy
    //
      ToUUID("B9455B06-7949-40c6-ABF2-363A70C8706C")
    })

    // CLPM (Current Low Power Mode Setting)
    //
    // This object evaluates to an integer that indicates the current platform desired LPM mode setting.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    0 - Disable LPM
    //    1 - Enable LPM
    //    2 - Application Specific LPM
    //    3 - Use the LPM setting as requested by the OS level interfaces either through the OS Power Plan settings or
    //        through the LPM API exposed to OS software.
    //
    Method (CLPM)
    {
        If(LEqual(\_SB.PCI0.B0D4.LPMS,0)){ // check LPM support in CPU
          Return (0)
        }
        Return (LPMV) // Update LPMV based on current platform desire.
    } // End of CLPM object

    // This object evaluates to a package of packages that provides the current platform
    // desired LPM mode setting.
    // LPMT is evaluated by Intel LPM Policy when CLPM Object returns a value of 1 (Enable LPM).
    //
    Name (LPMT, Package()
    {
      0x00000001,       // Revision
      Package() {
        \_SB.PCI0.B0D4, // Target: Named Reference (String)
        0x00000000,     // Domain: Domain Type under the source that is controlled (0=TypeProcessor)
        0x00020000,     // ControlKnob: Identifies the type of control knob that need to be exercised
        50,             // ControlValue: Target State to which policy will jump to upon enabling Intel LPM.
        0x80000000,     // Reserved1
        0x80000000      // Reserved2
      },
      Package() {
        \_SB.PCI0.B0D4, // Target: Named Reference (String)
        0x00000000,     // Domain: Domain Type under the source that is controlled (0=TypeProcessor)
        0x00040000,     // ControlKnob: Identifies the type of control knob that need to be exercised
        2,              // ControlValue: Target State to which policy will jump to upon enabling Intel LPM.
        0x80000000,     // Reserved1
        0x80000000      // Reserved2
      },
      Package() {
        \_SB.PCI0.B0D4, // Target: Named Reference (String)
        0x00000001,     // Domain: Domain Type under the source that is controlled (01h=TypeGraphics)
        0x00020000,     // ControlKnob: Identifies the type of control knob that need to be exercised
        50,             // ControlValue: Target State to which policy will jump to upon enabling Intel LPM.
        0x80000000,     // Reserved1
        0x80000000      // Reserved2
      },
      Package() {
        \_SB.PCI0.B0D4, // Target: Named Reference (String)
        0x00000009,     // Domain: Domain Type under the source that is controlled (09h=TypeMultiFunction)
        0x00010000,     // ControlKnob: Identifies the type of control knob that need to be exercised
        15000,          // ControlValue: Target State to which policy will jump to upon enabling Intel LPM.
        0x80000000,     // Reserved1
        0x80000000      // Reserved2
      }
    }) // End of LPMT object

}//end Scope(\_SB.IETM)

