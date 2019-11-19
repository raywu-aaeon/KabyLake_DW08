/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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


External(DSPR, IntObj)
External(DSAD, IntObj)
External(DSRS, IntObj)


//
// Name: DS01
// Description: This method simulates the USB Port Capabilities for DS endpoints.
// Input: Arg0 -> Endpoint number
// Return: _UPC package
//
Method(DS01, 1, Serialized, , PkgObj) {
  Name(UPCP, Package(4) {0xFF,0xFF,0x00,0x00}) // Proprietary connector
  Return(UPCP)
} // End of Method(DS01, 1, Serialized, , PkgObj)


//
// Name: DS02
// Description: This method simulates the Physical Device Location for DS endpoints.
// Input: Arg0 -> Endpoint number
// Return: _PLD package
//
Method(DS02, 1, Serialized, , PkgObj) {
  Name(PLDP, Package() {Buffer(0x14) {0x82,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x28,0x01,0x80,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} })
  CreateField(DerefOf(Index(PLDP,0)), 112, 8, ROTA) //Rotation field - 115:118
  CreateField(DerefOf(Index(PLDP,0)), 128, 8, VOFL) //Vertical offset field - 128:135
  CreateField(DerefOf(Index(PLDP,0)), 136, 8, VOFH) //Vertical offset field - 136:143
  CreateField(DerefOf(Index(PLDP,0)), 144, 8, HOFL) //Horizontal offset field - 144:151
  CreateField(DerefOf(Index(PLDP,0)), 152, 8, HOFH) //Horizontal offset field - 152:159

  If (LEqual(Arg0,0)) // Endpoint 0 - Depth
  {
    switch (ToInteger(DSRS))
    {
      case (0)
      {
        Store(0xA0, VOFL)
        Store(0x00, VOFH)
        Store(0x46, HOFL)
        Store(0x00, HOFH)
        Store(0x00, ROTA)
      }
      case (2)
      {
        Store(0x8C, VOFL)
        Store(0x00, VOFH)
        Store(0xA0, HOFL)
        Store(0x00, HOFH)
        Store(0x10, ROTA)
      }
      case (4)
      {
        Store(0xA0, VOFL)
        Store(0x00, VOFH)
        Store(0x46, HOFL)
        Store(0x00, HOFH)
        Store(0x20, ROTA)
      }
      case (6)
      {
        Store(0x46, VOFL)
        Store(0x00, VOFH)
        Store(0xA0, HOFL)
        Store(0x00, HOFH)
        Store(0x30, ROTA)
      }
    }
  }
  ElseIf (LEqual(Arg0,1)) // Endpoint 1 - RGB
  {
    switch (ToInteger(DSRS))
    {
      case (0)
      {
        Store(0xA0, VOFL)
        Store(0x00, VOFH)
        Store(0x5A, HOFL)
        Store(0x00, HOFH)
        Store(0x00, ROTA)
      }
      case (2)
      {
        Store(0x5A, VOFL)
        Store(0x00, VOFH)
        Store(0xA0, HOFL)
        Store(0x00, HOFH)
        Store(0x10, ROTA)
      }
      case (4)
      {
        Store(0xA0, VOFL)
        Store(0x00, VOFH)
        Store(0x5A, HOFL)
        Store(0x00, HOFH)
        Store(0x20, ROTA)
      }
      case (6)
      {
        Store(0x5A, VOFL)
        Store(0x00, VOFH)
        Store(0xA0, HOFL)
        Store(0x00, HOFH)
        Store(0x30, ROTA)
      }
    }
  }
  ElseIf (LEqual(Arg0,2)) // Endpoint 2 - IR
  {
    switch (ToInteger(DSRS))
    {
      case (0)
      {
        Store(0xA0, VOFL)
        Store(0x00, VOFH)
        Store(0x8C, HOFL)
        Store(0x00, HOFH)
        Store(0x00, ROTA)
      }
      case (2)
      {
        Store(0x46, VOFL)
        Store(0x00, VOFH)
        Store(0xA0, HOFL)
        Store(0x00, HOFH)
        Store(0x10, ROTA)
      }
      case (4)
      {
        Store(0xA0, VOFL)
        Store(0x00, VOFH)
        Store(0x8C, HOFL)
        Store(0x00, HOFH)
        Store(0x20, ROTA)
      }
      case (6)
      {
        Store(0x8C, VOFL)
        Store(0x00, VOFH)
        Store(0xA0, HOFL)
        Store(0x00, HOFH)
        Store(0x30, ROTA)
      }
    }
  }

  Return(PLDP)
} // End of Method(DS02, 1, Serialized, , PkgObj)


//
// DS4 - Depth (Endpoint 0)
//
Device(CDPT) {
  Method(_STA, 0, Serialized) {
    Return(DSPR)
  }

  Method(_ADR, 0, Serialized)
  {
    Return(DSAD+2)
  }

   Method(_UPC, 0, Serialized) {
    Return(DS01(0))
  }

  Method(_PLD, 0, Serialized) {
    Return(DS02(0))
  }
} // Device(CDPT)


//
// DS4 - RGB (Endpoint 1)
//
Device(CRGB) {
  Method(_STA, 0, Serialized) {
    Return(DSPR)
  }

  Method(_ADR, 0, Serialized)
  {
    Return(DSAD+4)
  }

   Method(_UPC, 0, Serialized) {
    Return(DS01(1))
  }

  Method(_PLD, 0, Serialized) {
    Return(DS02(1))
  }
} // Device(CRGB)

//
// DS4 - IR (Endpoint 2)
//
Device(CLRY) {
  Method(_STA, 0, Serialized) {
    Return(DSPR)
  }

  Method(_ADR, 0, Serialized)
  {
    Return(DSAD)
  }

   Method(_UPC, 0, Serialized) {
    Return(DS01(2))
  }

  Method(_PLD, 0, Serialized) {
    Return(DS02(2))
  }
} // Device(CRGB)
