/** @file
  ACPI DSDT table for SCS Platform Configuration

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 Intel Corporation.

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

If(LEqual(PCHV(), SPTL)) { // SCS available on PCH-LP only

If(LNotEqual(SDWE,0)) { // Check if SCS SDCard Sideband Events Supported
// SD Card Controller
Scope(\_SB.PCI0.PSDC) {
  Method (_CRS, 0, NotSerialized) {

    If(LLess(OSYS,2012)) { return (ResourceTemplate(){}) } // Do not report for Win 7 (it doesn't support ACPI5.0 and wouldn't understand GpioInt)

    //Wake from SD card events while host controller in D3
    Name (RBUF, ResourceTemplate () {
      GpioInt (Edge, ActiveBoth, SharedAndWake, PullNone, 10000, "\\_SB.PCI0.GPI0", 0x00, ResourceConsumer, INTG) {0} // SD CARD DETECT GpioInt
      GpioIo (Shared, PullDefault, 0, 0, IoRestrictionInputOnly, "\\_SB.PCI0.GPI0",,, GPIO) {0} // SD CARD DETECT GpioIo
    })

    CreateWordField (RBUF, INTG._PIN, SDIP)
    CreateWordField (RBUF, GPIO._PIN, SDGP)

    If(LEqual(SDWE,1)) {
      // Update GpioInt (GPIO Interrupt Connection Resource) descriptor with GPP_B17 - PIN 41
      Store (GNUM(GPIO_SKL_LP_GPP_B17), SDIP)
      // Update GpioIo (GPIO Connection IO Resource) descriptor with GPP_B17 - PIN 41
      Store (GNUM(GPIO_SKL_LP_GPP_B17), SDGP)
    }

    If(LEqual(SDWE,2)) {
      // Update GpioInt (GPIO Interrupt Connection Resource) descriptor with GPP_D10 - PIN 82
      Store (GNUM(GPIO_SKL_LP_GPP_D10), SDIP)
      // Update GpioIo (GPIO Connection IO Resource) descriptor  with GPP_D10 - PIN 82
      Store (GNUM(GPIO_SKL_LP_GPP_D10), SDGP)
    }

    Return (RBUF)
  }
}
}// End If(LNotEqual(SDWE,0)) - SDCard Wake enabled
}// End If(LEqual(PCHV(), SPTL)) - PCH-LP only
