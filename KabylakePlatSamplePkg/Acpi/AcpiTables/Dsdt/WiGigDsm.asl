/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2019 Intel Corporation.

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

//--------------------
//  WGIG Wake up Event solution
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
//   If Function Index = 0, a Buffer containing a function index bitfield.
//   Otherwise, the return value and type depends on the UUID and revision ID.
//
Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
{
  If(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }
  If(LEqual(Arg0, ToUUID("1730E71D-E5DD-4A34-BE57-4D76B6A2FE37")))
  {
    // Function 0 : Query Function
    If(LEqual(Arg2, Zero))
    {
      // Revision 0
      If(LEqual(Arg1, Zero)){ // The current revision is 0
        Return(Buffer(){0x03}) // There are 1 function defined other than Query.
      } Else {
        Return(Buffer(){0}) // No supported functions.
      }
    }
    // Function 1 :
    If(LEqual(Arg2, One))
    {
      Switch(ToInteger(DeRefOf(Index(Arg3, 0))))
      {
        Case (0) // Send HID Event/Wake Notification
        {
// AMI_OVERRIDE_START - Fix building error when CRB_EC_SUPPORT is 0x0
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1)        
          \_SB.HIDD.HPEM(27)
#endif
// AMI_OVERRIDE_END - Fix building error when CRB_EC_SUPPORT is 0x0
        }
        Case (1) // Send Sleep button Notification. Put the system to Sleep
        {
          If(CondRefOf(\_SB.SLPB)) {
            Notify(\_SB.SLPB, 0x80)
          }
        }
        Case (2) // Send Dock Event
        {
// AMI_OVERRIDE_START - Fix building error when CRB_EC_SUPPORT is 0x0
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1) 
          If(CondRefOf(\_SB.PCI0.LPCB.H_EC.UPBT))
          {
            \_SB.PCI0.LPCB.H_EC.UPBT(0x80, One) // Arg0:Bit7 - Dock Indicator change request
          }
#endif          
// AMI_OVERRIDE_END - Fix building error when CRB_EC_SUPPORT is 0x0
        }
        Case (3) // Send Undock Event
        {
// AMI_OVERRIDE_START - Fix building error when CRB_EC_SUPPORT is 0x0
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1)         
          If(CondRefOf(\_SB.PCI0.LPCB.H_EC.UPBT))
          {
            \_SB.PCI0.LPCB.H_EC.UPBT(0x80, Zero) // Arg0:Bit7 - Dock Indicator change request
          }
#endif           
// AMI_OVERRIDE_END - Fix building error when CRB_EC_SUPPORT is 0x0
        }
        Case (4) // Send Sleep Button Wake Notification. Wake system via sleep button
        {
          If(CondRefOf(\_SB.SLPB)) {
            Notify(\_SB.SLPB, 0x02)
          }
        }
      }
    }
    // no other supported functions, but normal return.
    Return(0)
  }
  Elseif(LEqual(Arg0, ToUUID("7574EB17-D1A2-4CC2-9929-4A08FCC29107")))
  {
    Switch(ToInteger(Arg2))
    {
      // Function 0 : Query Function
      Case(0)
      {
        // Revision 0
        If(LEqual(Arg1, Zero)) // The current revision is 0
        {
          Return(Buffer(){0x07}) // function 0, 1 & 2 are supported.
        }
        Else
        {
          Return(Buffer(){0}) // No supported functions.
        }
      }
      // Function 1 : For WiGig device this function will return the WiGig
      // Dock Locking White List.  This consists of a Package of Packages
      // containing the White List data.
      Case(1)
      {
        Return(\_SB.PCI0.WHIT()) // return White list
      }
      // Function 2: For WiGig device this function will return the Self ID
      // of the PC.  This is defined as an OEM ID and SKU ID combination
      // for the notebook/convertible itself.
      Case (2)
      {
        Return(\_SB.PCI0.SELF()) // return SelfID structure
      }
      Default
      {
        Return(Buffer(){0}) // No other supported functions.
      }
    } // end Switch(Arg2)
  }
  Else
  {
    Return(Buffer(){0}) // Guid mismatch
  }
}

