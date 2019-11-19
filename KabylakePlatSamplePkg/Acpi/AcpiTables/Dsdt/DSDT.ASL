/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2018 Intel Corporation.

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

#include "PlatformBoardId.h"

DefinitionBlock (
  "DSDT.aml",
  "DSDT",
  0x02, // DSDT revision.
        // A Revision field value greater than or equal to 2 signifies that integers 
        // declared within the Definition Block are to be evaluated as 64-bit values
  "INTEL",   // OEM ID (6 byte string)
  "SKL     ",// OEM table ID  (8 byte string)
  0x0 // OEM version of DSDT table (4 byte Integer)
)

// BEGIN OF ASL SCOPE
{
  External(LHIH)
  External(LLOW)
  External(IGDS)
  External(LIDS)
  External(BRTL)
  External(ALSE)
  External(GSMI)
  External(\_SB.PCI0.GFX0, DeviceObj)
  External(\_SB.PCI0.GFX0.ALSI)
  External(\_SB.PCI0.GFX0.CDCK)
  External(\_SB.PCI0.GFX0.CBLV)
  External(\_SB.PCI0.GFX0.GSSE)
  External(\_SB.PCI0.ISP0, DeviceObj)
  External(\_SB.PCI0.PEG0, DeviceObj)
  External(\_SB.PCI0.PEG0.PEGP, DeviceObj)
  External(\_SB.PCI0.PEG1, DeviceObj)
  External(\_SB.PCI0.PEG2, DeviceObj)
  External(\_SB.PCI0.GFX0.DD1F, DeviceObj)
  External(\_SB.PCI0.GFX0.GDCK, MethodObj)
  External(\_SB.PCI0.GFX0.GHDS, MethodObj)
  External(\_SB.PCI0.GFX0.AINT, MethodObj)
  External(\_SB.PCI0.GFX0.GLID, MethodObj)
  External(\_SB.PCI0.GFX0.GSCI, MethodObj)
  External(\_PR.PR00, DeviceObj)
  External(\_PR.PR00._PSS, MethodObj)
  External(\_PR.PR00.LPSS, PkgObj)
  External(\_PR.PR00.TPSS, PkgObj)
  External(\_PR.PR00._PPC, MethodObj)
  External(\_PR.CPPC, IntObj)
  External(\_TZ.TZ00, DeviceObj)
  External(\_TZ.TZ01, DeviceObj)
  External(\_TZ.ETMD, IntObj)
  External(\_TZ.FN00._OFF, MethodObj)
  // Miscellaneous services enabled in Project
  Include ("AMLUPD.asl")
  Include ("GloblNvs.asl")
  Include ("PciTree.asl")

  if(LEqual(ECR1,1)){
    Scope(\_SB.PCI0) {
      //
      // PCI-specific method's GUID
      //
      Name(PCIG, ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))
      //
      // PCI's _DSM - an attempt at modular _DSM implementation
      // When writing your own _DSM function that needs to include PCI-specific methods, do this:
      //
      // Method(_YOUR_DSM,4){ 
      //   if(Lequal(Arg0,PCIG)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }
      //   ...continue your _DSM by checking different GUIDs...
      //   else { return(0) }
      // } 
      //
      Method(PCID, 4, Serialized) {
        If(LEqual(Arg0, PCIG)) {         // PCIE capabilities UUID
          If(LGreaterEqual(Arg1,3)) {                                              // revision at least 3
            If(LEqual(Arg2,0)) { Return (Buffer(2){0x01,0x03}) }                   // function 0: list of supported functions
            If(LEqual(Arg2,8)) { Return (1) }                                      // function 8: Avoiding Power-On Reset Delay Duplication on Sx Resume
            If(LEqual(Arg2,9)) { Return (Package(5){50000,Ones,Ones,50000,Ones}) } // function 9: Specifying Device Readiness Durations
          }
        }
        return (Buffer(1){0})
      }
    }//scope
  }//if

  Scope(\_SB.PCI0) {
    //PciCheck, Arg0=UUID, returns true if support for 'PCI delays optimization ECR' is enabled and the UUID is correct
    Method(PCIC,1,Serialized) {
      If(LEqual(ECR1,1)) {
        If(LEqual(Arg0, PCIG)) {
          return (1)
        }
      }
      return (0)
    }
  }

  Include ("Pch.asl")  // Not in this package. Refer to the PCH Reference Code accordingly
  Include ("LpcB.asl")
  Include ("Platform.asl")
  Include ("CPU.asl")
  Include ("PCI_DRC.ASL")
  Include ("Video.asl")
  Include ("PlatformGnvs.asl")
  Include ("Gpe.asl")
  //
  // AdvancedFeaturesBegin
  //
  Include ("PCIEDOCK.ASL")
  Include ("PchRpPxsxWrapper.asl")
  Include ("Pep.asl")
  Include ("Psm.asl")
  Include ("Connectivity.asl")
If (LEqual(CAMT, 1)) { // Check for CameraType = IVCAM
  Include ("IVCAM.asl")
} // End of If(LEqual(CAMT, 1))
  Include ("SkyCamSensors.asl")
  Include ("ScsPlatform.asl")
  Include ("WirelessCharge.asl")
  Include ("HIDWakeDSM.asl")
  Include ("TimeAndAlarmDevice.asl")
  //
  // AdvancedFeaturesEnd
  //

  Name(\_S0, Package(4){0x0,0x0,0,0}) // mandatory System state
  if(SS1) { Name(\_S1, Package(4){0x1,0x0,0,0})}
  if(SS3) { Name(\_S3, Package(4){0x5,0x0,0,0})}
  if(SS4) { Name(\_S4, Package(4){0x6,0x0,0,0})}
  Name(\_S5, Package(4){0x7,0x0,0,0}) // mandatory System state

  Method(PTS, 1) {        // METHOD CALLED FROM _PTS PRIOR TO ENTER ANY SLEEP STATE
    If(Arg0)              // entering any sleep state
      {
      }
    }
    Method(WAK, 1) {      // METHOD CALLED FROM _WAK RIGHT AFTER WAKE UP
  }
}// End of ASL File

