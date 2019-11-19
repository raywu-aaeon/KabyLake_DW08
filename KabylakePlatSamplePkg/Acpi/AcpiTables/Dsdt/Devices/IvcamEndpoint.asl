/** @file
  ACPI DSDT table

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

@par Specification
**/


External(\IVPR, IntObj) //IvcamPresent
External(\IVAD, IntObj) //IvcamEpAddress
External(\IVRS, IntObj) //IvcamRotation
External(\IVWS, IntObj) //IvcamWakeSupport

External(\GPRW, MethodObj)

//
// Name: IV01
// Description: This method simulates the _UPC (USB Port Capabilities) for Ivcam endpoints
// Input: Arg0 -> Endpoint number
// Return: _UPC package
//
Method(IV01, 1, Serialized, , PkgObj)
{
  Name(UPCP, Package(4) {0xFF,0xFF,0x00,0x00}) // Port is connectable + Proprietary connector
  Return(UPCP)
} // End of Method(IV01, 1, Serialized, , PkgObj)

//
// Name: IV02
// Description: This method simulates the _PLD (Physical Location of Device) for Ivcam endpoints
// Input: Arg0 -> Endpoint number
// Return: _PLD package
//
Method(IV02, 1, Serialized, , PkgObj)
{

  Name(PLDP, Package(0x01)
  {
    ToPLD (
      PLD_Revision                = 0x02,
      PLD_IgnoreColor             = 0x01,
      PLD_Lid                     = 0x01,
      PLD_Panel                   = "FRONT",
      PLD_VerticalPosition        = "UPPER",
      PLD_HorizontalPosition      = "CENTER",
      PLD_GroupToken              = 0x01,
      PLD_GroupPosition           = 0x07,
      PLD_Rotation                = 0,
      PLD_VerticalOffset          = 0xFFFF,
      PLD_HorizontalOffset        = 0xFFFF
    )
  })

  CreateField(DerefOf(Index(PLDP,0)), 115,  4, ROTA) // Rotation field
  CreateField(DerefOf(Index(PLDP,0)), 128, 16, VOFF) // Vertical offset field
  CreateField(DerefOf(Index(PLDP,0)), 144, 16, HOFF) // Horizontal field

  Store(\IVRS, ROTA)

  If (LEqual(Arg0,0)) // Endpoint 0
  {
    switch (ToInteger(\IVRS))
    {
      case (0)
      {
        Store(0x00C2, VOFF)
        Store(0x009E, HOFF)
      }
      case (2)
      {
        Store(0x009E, VOFF)
        Store(0x00C2, HOFF)
      }
      case (4)
      {
        Store(0x00C2, VOFF)
        Store(0x009E, HOFF)
      }
      case (6)
      {
        Store(0x009E, VOFF)
        Store(0x00C2, HOFF)
      }
    }
  } ElseIf (LEqual(Arg0,1)) { // Endpoint 1
    switch (ToInteger(\IVRS))
    {
      case (0)
      {
        Store(0x00C2, VOFF)
        Store(0x0086, HOFF)
      }
      case (2)
      {
        Store(0x0086, VOFF)
        Store(0x00C2, HOFF)
      }
      case (4)
      {
        Store(0x00C2, VOFF)
        Store(0x0086, HOFF)
      }
      case (6)
      {
        Store(0x0086, VOFF)
        Store(0x00C2, HOFF)
      }
    }
  }

  Return(PLDP)

} // End of Method(IV02, 1, Serialized, , PkgObj)

//
// IVCAM - RGB (Endpoint 0)
//
Device(CRGB)
{
  Method(_STA, 0, Serialized)
  {
    Return(\IVPR)
  }

  Method(_ADR, 0, Serialized)
  {
    Return(\IVAD)
  }

  Method(_UPC, 0, Serialized)
  {
    Return(IV01(0))
  }

  Method(_PLD, 0, Serialized)
  {
    Return(IV02(0))
  }
} // Device(CRGB)

//
// IVCAM - DEPTH (Endpoint 1)
//
Device(CDPT)
{
  Method(_STA, 0, Serialized)
  {
    Return(\IVPR)
  }

  Method(_ADR, 0, Serialized)
  {
    Return(\IVAD+2)
  }

  Method(_UPC, 0, Serialized)
  {
    Return(IV01(1))
  }

  Method(_PLD, 0, Serialized)
  {
    Return(IV02(1))
  }
} // Device(CDPT)

//
// IVCAM Wake support
//
Method (_PRW, 0, Serialized)
{
  If (LEqual(\IVWS, 1))
  {
    Return (GPRW(0x6D, 4)) // Can wakeup from S4 state
  }
  Else
  {
    Return (Package(){Zero, Zero})
  }
}

Method (_S3D, 0, Serialized)
{
  If (LEqual(\IVWS, 1))
  {
    Return (0x2)
  }
  Else
  {
    Return (0x3)
  }
}

Method (_S3W, 0, Serialized)
{
  If (LEqual(\IVWS, 1))
  {
    Return (0x2)
  }
  Else
  {
    Return (0x3)
  }
}

Method (_S4D, 0, Serialized)
{
  If (LEqual(\IVWS, 1))
  {
    Return (0x2)
  }
  Else
  {
    Return (0x3)
  }
}

Method (_S4W, 0, Serialized)
{
  If (LEqual(\IVWS, 1))
  {
    Return (0x2)
  }
  Else
  {
    Return (0x3)
  }
}
