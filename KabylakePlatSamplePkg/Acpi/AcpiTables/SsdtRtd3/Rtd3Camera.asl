/** @file
  ACPI RTD3 - Camera information

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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

External(EMOD, IntObj)
External(CAMT, IntObj)
External(DIVO, IntObj)
External(DIVF, IntObj)
External(DDSO, IntObj)
External(DDSF, IntObj)
External(\_SB.RDGP, MethodObj)
External(\_SB.WRGP, MethodObj)
External(\_SB.ICAM.IDFU, IntObj)
External(\_SB.ICAM.IPWR, IntObj)
External(IVDG)
External(DVDG)

Scope(\_SB.PCI0.XHC.RHUB)
{
    Name (CPWR,0)
    Method(CMSA) {                                     /// _STA method
      If(LEqual(CAMT, 1)){
        Store(IVDG, Local0)
      }
      ElseIf(LEqual(CAMT, 2)){
        Store(DVDG, Local0)
      }

      If(LEqual(\_SB.RDGP(Local0, 1), 1)) {            // Get power status base on GPIO
        Return(0x01)
      }
      Return(0x00)
    }

    Method(CMON, 0) {                                  /// _ON method
      If (LNotEqual(CPWR, 1)) {
        If(LEqual(CAMT, 1)){                           // Check for CameraType = IVCAM
          If (CondRefOf(\_SB.ICAM.IPWR)) {
            If(LEqual(\_SB.ICAM.IPWR, 0)) {            // Check if VDD_GPIO disabled by DSM
              \_SB.WRGP(IVDG, 1)                       // Power up Camera.
              Sleep(DIVO)
              Store(1,CPWR)
            }
          }
        }
        ElseIf(LEqual(CAMT, 2))
        {
          \_SB.WRGP(DVDG, 1)                           // Power up Camera.
          Sleep(DDSO)
          Store(1,CPWR)
        }
      }
    }

    Method(COFF, 0) {                                  /// _OFF method
      If (LNotEqual(CPWR,0)) {
        If(LEqual(CAMT, 1)){                           // Check for CameraType = IVCAM
          If (CondRefOf(\_SB.ICAM.IDFU)) {
            If (LEqual(\_SB.ICAM.IDFU, 0)) {           // Check if FW_GPIO disabled by DSM
              \_SB.WRGP(IVDG, 0)                       // Power down Camera.
              Sleep(DIVF)
              Store(0,CPWR)
            }
          }
        }
        ElseIf(LEqual(CAMT, 2))
        {
          \_SB.WRGP(DVDG, 0)                           // Power down Camera.
          Sleep(DDSF)
          Store(0,CPWR)
        }
      }
    }
}

If(LEqual(And(RCG0,2),2))
{
  Scope(\_SB.PCI0.XHC.RHUB)
    {
    PowerResource(PCAM, 0, 0) {                        /// power rail for USB2.0 Port 4 / USB3.0 Port 3
        Method(_STA) {                                   /// _STA method
          If (LEqual(\EMOD, 1)) {
            Return(1)
          }
          Else
          {
            Return(CMSA())
          }
        }

        Method(_ON, 0, Serialized) {                     /// _ON method
          If (LEqual(\EMOD, 1)) {
            // Do nothing
          }
          Else
          {
            CMON()
          }
        }

        Method(_OFF, 0, Serialized) {                    /// _OFF method
          If (LEqual(\EMOD, 1)) {
            // Do nothing
          }
          Else
          {
            COFF()
          }
        }
      } // End PCAM
    } // End of Scope(\_SB.PCI0.XHC.RHUB)
} // End if(LEqual(And(RCG0,2),2))
