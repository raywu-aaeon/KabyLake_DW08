/** @file
  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2015 Intel Corporation.

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

    Name(TRT0, Package()
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           20,     50,   0, 0, 0, 0},
      Package(){\_SB.PCI0.LPCB.H_EC.SEN1,\_SB.PCI0.B0D4,        10,    100,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.SEN1, 30,     30,   0, 0, 0, 0},
      Package(){\_SB.PCI0.LPCB.H_EC.SEN1,\_SB.PCI0.LPCB.H_EC.SEN1,20,   30,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.SEN3, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.SEN4, 20,    200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.LPCB.H_EC.SEN1,\_SB.PCI0.LPCB.H_EC.SEN4, 1,  200,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.STRG,                20,    200,   0, 0, 0, 0}
    })

    Name(TRT1, Package()
    {
      // Source                       Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.B0D4,           21,     51,   0, 0, 0, 0},
      Package(){\_SB.PCI0.LPCB.H_EC.SEN1,\_SB.PCI0.B0D4,        11,    101,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.SEN1, 31,     31,   0, 0, 0, 0},
      Package(){\_SB.PCI0.LPCB.H_EC.SEN1,\_SB.PCI0.LPCB.H_EC.SEN1,21,   31,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.SEN3, 21,    201,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.PCI0.LPCB.H_EC.SEN4, 21,    201,   0, 0, 0, 0},
      Package(){\_SB.PCI0.LPCB.H_EC.SEN1,\_SB.PCI0.LPCB.H_EC.SEN4, 2,  201,   0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,       \_SB.STRG,                21,    201,   0, 0, 0, 0}
    })

    Name(TRT2, Package() // TRT Table for Windows Media Player
    {
      // Source                           Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,           \_SB.PCI0.B0D4,           9,     50,    0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,           \_SB.PCI0.LPCB.H_EC.SEN3, 7,    600,    0, 0, 0, 0},
      Package(){\_SB.PCI0.LPCB.H_EC.CHRG, \_SB.PCI0.LPCB.H_EC.SEN3, 9,    600,    0, 0, 0, 0}
    })

    Name(TRT3, Package() // TRT Table for Skype
    {
      // Source                           Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,           \_SB.PCI0.B0D4,           9,     20,    0, 0, 0, 0},
      Package(){\_SB.PCI0.B0D4,           \_SB.PCI0.LPCB.H_EC.SEN3, 9,    300,    0, 0, 0, 0},
      Package(){\_SB.PCI0.LPCB.H_EC.CHRG, \_SB.PCI0.LPCB.H_EC.SEN3, 7,    300,    0, 0, 0, 0}
    })

    Name(TRT4, Package() // TRT Table for Skype
    {
      // Source                           Target             Influence  Period    Reserved
      Package(){\_SB.PCI0.B0D4,           \_SB.PCI0.LPCB.H_EC.GEN5, 10,     20,    0, 0, 0, 0},
    })


    // TRTR (Thermal Relationship Table Revision)
    //
    // This object evaluates to an integer value that defines the revision of the _TRT object.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    // 0: Traditional TRT as defined by the ACPI Specification.
    // 1: Priority based TRT
    //
    Method(TRTR)
    {
      Return(TRTV)
    }

    // _TRT (Thermal Relationship Table)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Thermal Relationship Packages as described below.
    //
    // Return Value Information
    //   Package {
    //   ThermalRelationship[0] // Package
    //    ...
    //   ThermalRelationship[n] // Package
    //   }
    //
    Method(_TRT,,,,PkgObj)
    {
      If (LEqual(WKLD, 1)) // check workload hint and return TRT for Windows Media Player
      {
        Return(TRT2)
      }
      If (LEqual(WKLD, 2)) // check workload hint and return TRT for Skype
      {
        Return(TRT3)
      }

      ShiftRight(DSTA,8,Local5)
      And(Local5,0x000000FF,Local5)
      If(Local5) // check Device State Hint and return TRT table for vertical orientation
      {
        Return(TRT1)
      }

      If(ALMT)
      {
        Return(TRT4) // TRT for acoustic level for GEN5
      }

      Return(TRT0) // default
    } // End Method(_TRT)

}// End Scope(\_SB.IETM)
