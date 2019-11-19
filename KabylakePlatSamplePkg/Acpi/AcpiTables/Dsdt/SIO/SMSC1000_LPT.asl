/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2014 Intel Corporation.

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

Device(LPT1)  // Printer, Output Mode
{
  Name(_HID,EISAID("PNP0400"))
  Name(_UID,5)

  // Status Method for LPT-Output Mode.

  Method(_STA,0,Serialized)
  {
    If(SMC1)
    {
      // Check if SIO Device is Present & Display LPT1 and return status.

      Store(0,CR55)
      Store(CR01,Local0)
      Store(0,CRAA)

      If(And(Local0,0x0C))
      {
        Return(0x000F)
      }

      Return(0x000D)
    }

    Return(0x0000)
  }


  // Disable Method for LPT-Output Mode.

  Method(_DIS,0,Serialized)
  {
    // Set SIO to LPT.
        Store(0,CR55)
    Store(And(CR01,0x0FB),CR01)
    Store(0,CRAA)
  }

  // Current Resource Setting Method for LPT-Output Mode.

  Method(_CRS,0,Serialized)
  {
    // Create a buffer to return to OS.

    Name(BUF0,ResourceTemplate()
    {
      IO(Decode16,0x0378,0x0378,0x01,0x04)
      IRQNoFlags(){5}
    })

    If(SMC1)
    {
      // Create pointers to the specific byte.

      CreateByteField(BUF0,0x02,IOL0)
      CreateByteField(BUF0,0x03,IOH0)
      CreateByteField(BUF0,0x04,IOL1)
      CreateByteField(BUF0,0x05,IOH1)
      CreateByteField(BUF0,0x07,LEN0)
      CreateWordField(BUF0,0x09,IRQW)

            Store(0,CR55)

      ShiftLeft(CR23,2,IOL0)    // Low Byte.
      ShiftLeft(CR23,2,IOL1)

      ShiftRight(CR23,6,IOH0)   // High Byte.
      ShiftRight(CR23,6,IOH1)

      // Write the length into the Buffer.

      Store(4,LEN0)

          // Write the IRQ value into the Buffer.

            Store(0x0,Local1)
            And(CR27,0x0F,Local1)
            ShiftLeft(1, Local1, IRQW)
      Store(0,CRAA)
    }

    Return(BUF0)
  }

  // Possible Resource Setting Method for LPT-Output Mode.
  // Build a Buffer with all valid LPT Port Resources.

  Name(_PRS,ResourceTemplate()
  {
    StartDependentFn(0,2)
    {
      IO(Decode16,0x378,0x378,0x01,0x04)
      IRQNoFlags(){5}
    }

    StartDependentFn(0,2)
    {
      IO(Decode16,0x278,0x278,0x01,0x04)
      IRQNoFlags(){5}
    }

    StartDependentFn(0,2)
    {
      IO(Decode16,0x3BC,0x3BC,0x01,0x04)
      IRQNoFlags(){5}
    }

    StartDependentFn(0,2)
    {
      IO(Decode16,0x378,0x378,0x01,0x04)
      IRQNoFlags(){7}
    }

    StartDependentFn(0,2)
    {
      IO(Decode16,0x278,0x278,0x01,0x04)
      IRQNoFlags(){7}
    }

    StartDependentFn(0,2)
    {
      IO(Decode16,0x3BC,0x3BC,0x01,0x04)
      IRQNoFlags(){7}
    }

    EndDependentFn()
  })


  // Set Resource Setting Method for LPT-Output Mode.
  Method(_SRS,1,Serialized)
  {
    // Point to the specific information in the passed
    // in Buffer.

    CreateByteField(Arg0,0x02,IOL0)
    CreateByteField(Arg0,0x03,IOH0)
    CreateWordField(Arg0,0x09,IRQW)

      // Disable the device.
    Store(0,CR55)
    Store(And(CR01,0x0FB),CR01)

    // Set the Base IO Address.
    Store(ShiftRight(IOL0,2),CR23)
    Or(CR23,ShiftLeft(IOH0,6),CR23)

    // Set the IRQ.
    FindSetRightBit(IRQW,Local0)
    If (LNotEqual(IRQW,Zero))
    {
      Decrement(Local0)
    }
    Store(Local0,CR27)

    // Set the Decode Range so POUT works when docked.
    And(IOD1,0xFC,IOD1)     // Decode all bits.

    If(LEqual(IOH0,0x03))     // Address = 0x3xx
    {
      If(LEqual(IOL0,0x78))   // Address = 0x378
      {
        Or(IOD1,0x00,IOD1)
      }
      Else
      {
        Or(IOD1,0x02,IOD1)  // Address = 0x3BC
      }
    }
    Else          // Address = 0x278
    {
      Or(IOD1,0x01,IOD1)
    }

    // Enable the LPT.

    Or(CR01,0x04,CR01)
    Store(0,CRAA)
  }

  // D0 Method for LPT-Output Mode.

  Method(_PS0,0,Serialized)
  {
    Store(0,CR55)
    Store(Or(CR01,0x04),CR01)
    Store(0,CRAA)
  }

  // D3 Method for LPT-Output Mode.

  Method(_PS3,0,Serialized)
  {
    Store(0,CR55)
    Store(And(CR01,0x0FB),CR01)
    Store(0,CRAA)
   }
}
