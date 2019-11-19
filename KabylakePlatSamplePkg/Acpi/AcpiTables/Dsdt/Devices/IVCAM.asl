/** @file
  ACPI IVCAM 3D Camera

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2015 Intel Corporation.

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

@par Specification
**/

// AMI_OVERRIDE_START - Check for CameraType = IVCAM
If (LEqual(CAMT, 1)) { 
// AMI_OVERRIDE_END - Check for CameraType = IVCAM
Scope(\_SB) {
  Device(ICAM) {
    Name (_HID, "INT33A3")
    Name (IDFU, 0) // Variable for FW_GPIO (DFU) state set by _DSM
    Name (IPWR, 0) // Variable for VDD_GPIO (Power) state set by _DSM
    Method(_STA, 0, Serialized) {
      If(LEqual(IVDF, 1)){
        Return (0x0F)
      }
      Return(0x00)
    }
    //
    // _DSM  Device Specific Method supporting IVCAM
    //
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index
    // Arg3: Package Parameters
    //
    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (LEqual(Arg0, ToUUID ("F5CF0FF7-5D60-4842-82C0-FA1A61D873F2"))) {
        Switch (ToInteger(Arg2)) {
          //
          // Function 0 - Query Supported Functions
          //
          Case (0) {
            If(LEqual(ToInteger(Arg1),0)) {  // Revision 0
              Return (Buffer () {0x7})
            }
            Return (Buffer () {0})
          }
          //
          // Function 1 - FW GPIO Toggle Method
          //
          Case (1) {
            If(LEqual(DeRefOf(Index(Arg3, 0)), 0)) {
              \_SB.WRGP(IFWG, 0)
              Store(0,IDFU)
            } Else {
              \_SB.WRGP(IFWG, 1)
              Store(1,IDFU)
            }

            Return (0)
          }
          //
          // Function 2 - VDD GPIO Toggle Method
          //
          Case (2) {
            If(LEqual(DeRefOf(Index(Arg3, 0)), 0)) {
              \_SB.WRGP(IVDG, 0)
              Store(0,IPWR)
            } Else {
              \_SB.WRGP(IVDG, 1)
              Store(1,IPWR)
            }

            Return (0)
          }
          Default {
            Return (0)
          }
        } // End of Function Index switch
      } Else {
        Return (Buffer() {0})
      }
    } // Method (_DSM)
  } // Device(ICAM)
} //Scope(\_SB)
// AMI_OVERRIDE_START - Check for CameraType = IVCAM
}
// AMI_OVERRIDE_END - Check for CameraType = IVCAM