/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2017 Intel Corporation.

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

Scope(\_SB.PCI0.LPCB.H_EC)
{

  Device(TFN2)
  {
    Name(_HID, EISAID("INT3404"))  // Intel DPPM Fan Device 2 DUMMY DEVICE
    Name(_UID, "TFN2")
    Name(_STR, Unicode ("Fan 2 (virtual fan)"))

    Name(FCTL, 100)
    Name(FSPD, 200)
    Name(PTYP, 0x04)

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
      If (LEqual(FND2,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // _FIF (Fan Information)
    //
    // The optional _FIF object provides OSPM with fan device capability information.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //  A Package containing the fan device parameters.
    //
    Name(_FIF, Package()
    {
        0,
        1,
        50,
        0
    })

    // _FPS (Fan Performance States)
    //
    // Evaluates to a variable-length package containing a list of packages that describe the fan device's performance states.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //  A variable-length Package containing a Revision ID and a list of Packages that describe the fan device's performance states.
    //
    Method(_FPS,,,,PkgObj) // Get Fan P-States
    {
      // Get Fan Device Data default as CRB
      Return (Package()
      {
          0,  // Revision
          //
          //        Control   Trip Point   Speed   NoiseLevel  Power
          Package(){100,      0xFFFFFFFF,   200,   700,        7000},
          Package(){ 50,      0xFFFFFFFF,   100,   350,        3500},
          Package(){  0,      0xFFFFFFFF,     0,     0,           0}   // OFF
        })
    }

    // _FSL (Fan Set Level)
    //
    // The optional _FSL object is a control method that OSPM evaluates to set a fan device's speed (performance state) to a specific level.
    //
    // Arguments: (1)
    //  Arg0 - Level (Integer): conveys to the platform the fan speed level to be set.
    // Return Value:
    //  None
    //
    // Argument Information
    //  Arg0: Level. If the fan supports fine-grained control, Level is a percentage of maximum level (0-100)
    //  that the platform is to engage the fan. If the fan does not support fine-grained control,
    //  Level is a Control field value from a package in the _FPS object's package list.
    //  A Level value of zero causes the platform to turn off the fan.
    //
    Method(_FSL, 1, Serialized)
    {
      If(\_SB.PCI0.LPCB.H_EC.ECAV)
      {
        If(LNot(LEqual(Arg0, FCTL)))
        {
          Store(Arg0, FCTL)
          If (LGreaterEqual(Arg0, 51))
          {
            Store(200, FSPD)
          }
          If (LGreaterEqual(Arg0, 1))
          {
            Store(100, FSPD)
          }
          Else
          {
            Store(0, FSPD)
          }
        }
      }
    }

    Name(TFST, Package()
    {
        0,  // Revision
        0xFFFFFFFF, // Control
        0xFFFFFFFF  // Speed
    })

    //_FST (Fan Status)
    //
    // The optional _FST object provides status information for the fan device.
    //
    // Arguments: (0)
    //  None
    // Return Value:
    //  A Package containing fan device status information
    //
    Method(_FST,0,Serialized,,PkgObj)
    {
      Store(FCTL, Index(TFST, 1))
      Store(FSPD, Index(TFST, 2))
      Return(TFST)
    }
  } // End TFN2 Device
}// end Scope(\_SB.PCI0.LPCB.H_EC)
