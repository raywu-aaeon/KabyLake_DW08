/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2016 Intel Corporation.

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

//
// Comms Hub module definition.
//
scope (\)
{
  Device(CHUB)
  {
    Name(_HID, EISAID("INT339B"))

    Method(_STA)
    {
      If (LEqual(CHEN, 1))
      {
        Return(0x0F) // Comms Hub enabled, show it
      }
      Return(0x0)
    }

    //--------------------
    //  Comms Hub Event solution
    //--------------------
    //
    // _DSM (Device Specific Method)
    //
    // This optional object is a control method that enables devices to provide device specific control
    // functions that are consumed by the device driver.
    //
    // Arguments: (4)
    //   Arg0 - A Buffer containing a UUID
    //   Arg1 - An Integer containing the Revision ID
    //   Arg2 - An Integer containing the Function Index
    //   Arg3 - A Package that contains function-specific arguments
    // Return Value:
    //   If Function Index = 0, a Buffer containing a function index bit-field.
    //   Otherwise, the return value and type depends on the UUID and revision ID.
    //
    Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
    {
      //
      // verify UUID -- "1730E71D-E5DD-4A34-BE57-4D76B6A2FE37"
      //
      If (LEqual(Arg0, ToUUID ("1730E71D-E5DD-4A34-BE57-4D76B6A2FE37")))
      {
        If (LGreaterEqual(ToInteger(Arg1),0))        // Revision 0.
        {
          Switch (ToInteger(Arg2))            // Switch to Function Index.
          {
            //
            // Function 0, Query Function - return supported functions BitIndex.
            //
            Case (0)
            {
              Return (Buffer() {0x03}) // There is 1 function defined other than Query.
            }
            //
            // Function 1,  -
            //
            Case (1)
            {
              Switch (ToInteger (DeRefOf (Index (Arg3, 0))))
              {
                Case (0) // Send HID Event/Wake Notification
                {
// AMI_OVERRIDE_START - Fix building error when EC_SUPPORT = 0x0
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1)
                  \_SB.HIDD.HPEM(0x1C)
#endif
// AMI_OVERRIDE_END - Fix building error when EC_SUPPORT = 0x0
                }
              }
            }
          } // End Function Index
          Return (0)
        }  // End Revision check
        Return (0)
      }  // End UUID check
      Return (Buffer() {0})
    }  // End _DSM Method
  }
}