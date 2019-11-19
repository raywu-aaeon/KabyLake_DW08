/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 - 2018 Intel Corporation.

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

Scope(\_SB)
{

  Device(MCPP)
  {
    Name(_HID, EISAID("INT3530"))  // Multi Chip Pkg Participant
    Name(_UID, "MCPP")
    Name(_STR, Unicode ("Intel DPTF Multi Chip Participant")) // Intel DPTF Multi Chip Participant
    Name(PTYP, 0x26)

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If (LEqual(\MCPE,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // PPCC (Participant Power Control Capabilities)
    //
    // The PPCC object evaluates to a package of packages that indicates to DPTF processor
    // participant the power control capabilities.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   PPCC package of packages
    //
    Method(PPCC, 0, Serialized, , PkgObj)
    {
      Store (\MCSZ,Index(DerefOf(Index(\_SB.MCPP.NPCC,1)),5))                   // StepSize
      Store (\MCL0,Index(DerefOf(Index(\_SB.MCPP.NPCC,1)),1))                   // PowerLimitMinimum
      Return(NPCC)
    }

    Name (NPCC, Package()
    {                          // Field Name : Field Type
      2,                       // Revision : DWordConst

      Package ()               // Power Limit 1
      {
        0,                     // PowerLimitIndex   : DWordConst = 0
        5000,                  // PowerLimitMinimum : DWordConst
        85000,                 // PowerLimitMaximum : DWordConst
        0,                     // TimeWindowMinimum : DWordConst
        0,                     // TimeWindowMaximum : DWordConst
        1000                   // StepSize          : DWordConst
      }
    }) // End of PPCC object
  } // End MCPP Device
}// end Scope(\_SB.MCPP)
