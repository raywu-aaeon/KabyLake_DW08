/** @file
  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2014 Intel Corporation.

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

    Name(ART1, Package()    // ART package when in Quiet Mode / Passive Cooling Mode
    {
      0, // Revision
      // Source                           Target                    Weight, AC0MaxLevel, AC1MaxLevel, AC21MaxLevel, AC31MaxLevel, AC41MaxLevel, AC51MaxLevel, AC61MaxLevel, AC71MaxLevel, AC81MaxLevel, AC91MaxLevel
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.B0D4,           100,             70,          50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN1, 100,             70,  0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN2, 100,             40,          35,           30,           25,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN3, 100,     0xFFFFFFFF,  0xFFFFFFFF,   0xFFFFFFFF,           60,           50,           40,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN4, 100,     0xFFFFFFFF,  0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN5, 100,             60,          50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN2, \_SB.PCI0.LPCB.H_EC.SEN1, 100,             65,  0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN2, \_SB.PCI0.B0D4,           100,             75,          20,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF}
    })

    Name(ART0, Package()    // ART package when in Performance Mode / Active Cooling Mode
    {
      0,    // Revision
      // Source                           Target                    Weight, AC0MaxLevel, AC1MaxLevel, AC21MaxLevel, AC31MaxLevel, AC41MaxLevel, AC51MaxLevel, AC61MaxLevel, AC71MaxLevel, AC81MaxLevel, AC91MaxLevel
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.B0D4,           100,            100,          80,           65,           45,           25,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN1, 100,             90,  0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN2, 100,             60,          50,           40,           30,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN3, 100,     0xFFFFFFFF,  0xFFFFFFFF,   0xFFFFFFFF,           90,           75,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN4, 100,             75,          55,           35,           15,            5,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN1, \_SB.PCI0.LPCB.H_EC.SEN5, 100,             80,          60,           50,           30,           10,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN2, \_SB.PCI0.LPCB.H_EC.SEN1, 100,             95,  0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PCI0.LPCB.H_EC.TFN2, \_SB.PCI0.B0D4,           100,            100,          50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF}
    })

    // _ART (Active Cooling Relationship Table)
    //
    // Arguments:
    //   None
    // Return Value:
    //   A variable-length Package containing a Revision ID and a list of Active Relationship Packages as described below:
    //
    // Return Value Information
    //   Package {
    //   Revision, // Integer - Current revision is: 0
    //   ActiveRelationship[0] // Package
    //   ...
    //   ActiveRelationship[n] // Package
    //   }
    //
    Method(_ART,,,,PkgObj)
    {
      If(\_SB.PCI0.LPCB.H_EC.SEN3.CTYP)
      {
        Return(ART1)
      } Else {
        Return(ART0)
      }
    } // End _ART
}
