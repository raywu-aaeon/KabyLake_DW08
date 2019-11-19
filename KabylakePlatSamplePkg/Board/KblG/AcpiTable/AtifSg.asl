/** @file
  Intel ACPI Sample Code for Alto dGPU

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

External(\ECON, IntObj) // Embedded Controller Availability Flag.
External(NXD1)
External(NXD2)
External(NXD3)
External(NXD4)
External(NXD5)
External(NXD6)
External(NXD7)
External(NXD8)
External(TDTI)

External(\_SB.PCI0.LPCB.H_EC.ECRD, MethodObj)    // EC Read Method
External(\_SB.PCI0.LPCB.H_EC.ECWT, MethodObj)    // EC Write Method
External(\_SB.R008, MethodObj)

Scope(\_SB.PCI0.GFX0)
{
    //
    // INDL : Initialize Global Next active device list.
    //
    // Argments : None.
    //
    // returns : None.
    //
    Method(INDL, 0, Serialized)
    {
        Store(0, NXD1)
        Store(0, NXD2)
        Store(0, NXD3)
        Store(0, NXD4)
        Store(0, NXD5)
        Store(0, NXD6)
        Store(0, NXD7)
        Store(0, NXD8)
    }
    //
    // SNXD -> Set Next active device.
    //
    // Arg0 : Display vector of devices that will be activated
    //
    // Returns : None.
    //
    Method(SNXD, 1, Serialized)
    {
        INDL()

        Store(Arg0, Local0)
        If(And(Local0, ShiftLeft(1, 1)))  // 1-> BIT1 CRT1
        {
            Store(1, NXD1)  // NXD1 -> CRT
        }
        If(And(Local0, ShiftLeft(1, 0)))  // 0 -> BIT2 LCD1
        {
            Store(1, NXD2)  // NXD2 -> LCD
        }
        If(And(Local0, ShiftLeft(1, 3)))  // 3 -> BIT3 DFP1 (DP_B)
        {
            Store(1, NXD3)  // NXD3 -> Display port B
        }
        If(And(Local0, ShiftLeft(1, 7)))  // 7 -> BIT7 DFP2 (HDMI_B)
        {
            Store(1, NXD4)  // NXD4 -> HDMI B
        }
        If(And(Local0, ShiftLeft(1, 9)))  // 9 -> BIT9 DFP3 (HDMI_C)
        {
            Store(1, NXD5)  // NXD5 -> HDMI C
        }
        If(And(Local0, ShiftLeft(1, 10)))  // 10 -> BIT10 DFP4 (DP_D)
        {
            Store(1, NXD6)  // NXD6 -> Display port D
        }
        If(And(Local0, ShiftLeft(1, 11)))  // 11 -> BIT11 DFP5 (HDMI_D)
        {
            Store(1, NXD7)  // NXD7 -> HDMI D
        }

        //NXD8 is not used since there are only 7 entries in _DOD
    }

// GFX Interface:
// Provides specific GFX functionality on mobile platforms.
//
// Arguments:
//      Arg0:    Integer    Function code.
//      Arg1:    Package    Parameter buffer, 256 bytes
//
// Return Value:
//      Returns Buffer, 256 bytes

    Method(ATIF,2,Serialized)
    {
        ADBG("ATIF")
        Include("AtifCommonFunc.asl")  //ATIF Func 0-2 are common in PEG and SG 

        //
        // Function 3: Select Active Displays
        //
        // Returns displays to be selected in reposnse to
        // a display switch request notification
        //
        If(LEqual(Arg0,3))
        {
            ADBG("ATIF(3)")	
            Name(TMP3,Buffer(256) {0x0})
            CreateWordField (TMP3, 0, F3SS)
            CreateWordField (TMP3, 2, F3SD)
            CreateWordField(Arg1,0,AI3S)
            CreateWordField(Arg1,2,SLDS)
            CreateWordField(Arg1,4,CODS)
            Store(SLDS,CACD)
            Store(CODS,CCND)

            If (\ECON)
            {
               If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE)),1))
               {
                 Or(CCND,0x0001,CCND)    // Does not send LFP as connected when not LFP is not active. This is as per design
               }
            }

            // Size of return structure
            Store(0x0004,F3SS)

            // Next Displays to be Selected
            // Populate next displays based on Currently Connected and Active displays and the Toggle List Index
            // CCND, CACD, TLSN,
            Store(CTOI(CACD),TLSN)      // Get current toggle index based on currently Active display vector
            Store(CACD, Local1)         // Initialize Local1 to a safe value
            Store(NTLE, Local0)         // Total number of toggle list entries
            While(Local0)
            {
                Store(NATL(TLSN),Local1)    // Get the next combination from toggle list into Local1

                If(LNotEqual(Local1, 0))             //If next toggle list entry is not empty, then
                {
                    If(LEqual(And(Local1,CCND),Local1))  // If entries in the next combination are actually connected..
                    {
                        Store(1,Local0)                 // Exit since we got the next active list
                    }
                }
                Decrement(Local0)       // Decrement toggle list sequence counter

                Increment(TLSN)                 // Increment toggle list number to point to next active list
                If(LGreater(TLSN, NTLE))        // If sequence index currently points to last entry....
                {
                    Store(1,TLSN)           // Roll-up to the start of the toggle list
                }
            }
            SNXD(Local1)                // Set the selected displays as the next active for _DGS
            Store(Local1,NACD)          // The next active toggle list - put it on Opregion
            Store(NACD,F3SD)            // Store it in the return buffer
            Return(TMP3)
        }

        //
        // Function 17: Ready to Undock Notification
        //
        // This function will be invoked when GPU device is ready to be undocked and/or the docking station lock can be released.
        // Return none
        //
        If(LEqual(Arg0,17))
        {
          ADBG("ATIF(17)")
        }

        //
        // Function 18: Disable OS GPU Hot Plug/Unplug Notifications
        //
        // Return none
        //
        If(LEqual(Arg0,18))
        {
          ADBG("ATIF(18)")

          CreateWordField (Arg1, 0 , F18S)
          CreateByteField (Arg1, 2 , F18O)

          Store(F18O, Local0)
          And(Local0, 1, Local0)
          If(LEqual(Local0, 1))
          {
            ADBG("TBT.Notify.OS=1")
            WRCM(0xFB,1) //CMOS_TBT_NOTIFY_OS = 1
          }
          Else
          {
            ADBG("TBT.Notify.OS=0")
            WRCM(0xFB,0) //CMOS_TBT_NOTIFY_OS = 0
          }
        }

        //
        // Function 19: Retrieve External GPU PCI Configuration Space
        //
        // Returns External GPU PCI Configuration Space if available
        //
        If(LEqual(Arg0,19))
        {
          ADBG("ATIF(19)")

          Name(TM19,Buffer(256) {0x0})

          CreateWordField (Arg1, 0 , F19S)
          CreateWordField (Arg1, 2 , F19O)

          CreateWordField (TM19, 0 , T19S)
          CreateWordField (TM19, 2 , T19R)
          Store(256, T19S) // Structure size in bytes
          Store(0, T19R)
          If(LEqual(EGPV, 1))
          {
            Or(T19R, 1, T19R)
          }

          Store(EGPC, Local7)
          Add(Local7, F19O, Local7)
          Store (4, Local6) // Start offset to fill data
          While (1)
          {
            Store(\_SB.R008(Local7), Index(TM19, Local6))
            Increment (Local6)
            Increment (Local7)

            If(LEqual(Local6,256)) // End offset to fill data
            {
              Break
            }
          }

          Return(TM19)
        }

        //
        // Function 20: Get Thunderbolt devices information
        //
        // Returns Bus-Device-Function, VendorID-DeviceID data of Thunderbolt devices if available
        //

        If(LEqual(Arg0,20))
        {
          ADBG("ATIF(20)")

          Name(TM20,Buffer(256) {0x0})

          // Every device will have the following attributes
          // Bus number
          // Device number
          // Function number
          // Vendor ID
          // Device ID

          //Rootport information
          //HostRouter - Upstream port information
          //HostRouter - Downstream port(CIO) information
          //Endpoint - Upstream port information
          //Endpoint - Downstream port information
          //Endpoint information

          Store(TBDT, Local7)

          Store (0, Local6) // Start offset to fill data
          While (1)
          {
            Store(\_SB.R008(Local7), Index(TM20, Local6))

            Increment (Local6)
            Increment (Local7)
            If(LEqual(Local6,256)) // End offset to fill data
            {

              Break
            }
          }
          Return(TM20)
        }

//<Overriding as per edited spec - only Functions 0,1,2,3 supported>
//    //
//    // Function 5: Get TV Standard from CMOS
//    //
//    // Retrieves current TV standard
//    //
//    If(LEqual(Arg0,5))
//    {
//  Name(TMP5,Buffer(256) {0x0})
//  CreateWordField (TMP5, 0, F5SS)
//
//        // Size of return structure
//        Store(0x0004,F5SS)
//
//        // TV Standard Encoding Format
//        Store(0x00,Index(TMP5,2))
//
//        // TV Standard
//        Store(TVSD,Index(TMP5,3))
//
//        Return(TMP5)
//
//    }
    //
//    //
//    // Function 6: Set TV Standard in CMOS
//    //
//    // Records current TV standard in CMOS
//    //
//    If(LEqual(Arg0,6))
//    {
//        Name(TMP6,Buffer(256) {0x0})
//
//      CreateWordField(Arg1,0,AI6S)
//      CreateByteField(Arg1,2,TSEF)
//      CreateByteField(Arg1,3,TVST)
//
//  // Records current TV standard in CMOS
//  Store(TVST,TVSD)
//
//        Return(TMP6)
//
//    }
//
//    //
//    // Function 7: Get Panel Expansion Mode from CMOS
//    //
//    // Retrieves built-in panel expansion mode
//    //
//    If(LEqual(Arg0,7))
//    {
//  Name(TMP7,Buffer(256) {0x0})
//  CreateWordField (TMP7, 0 , F7SS)
//
//        // Size of return structure
//        Store(0x0003,F7SS)
    //
//        // Expansion Mode
//        Store(EXPM,Index(TMP7,2))
    //
//        Return(TMP7)
    //
//    }
    //
//    //
//    // Function 8: Set Panel Expansion Mode in CMOS
//    //
//    // Records built-in panel expansion mode in CMOS
//    //
//    If(LEqual(Arg0,8))
//    {
//        Name(TMP8,Buffer(256) {0x0})
    //
//      CreateWordField(Arg1,0,AI8S)
//      CreateByteField(Arg1,2,EMCM)
    //
//  // Record Expansion Mode in CMOS
//  Store(EMCM,EXPM)
    //
//        Return(TMP8)
//    }
    //
//    //
//    // Function 9: Get Selected Displays from CMOS
//    //
//    // Retrieves Selected Displays
//    //
//    If(LEqual(Arg0,9))
//    {
//  Name(TMP9,Buffer(256) {0x0})
//  CreateWordField (TMP5, 0, F9SS)
//  CreateWordField (TMP5, 2, F9SD)
//  CreateWordField (TMP5, 4, F9DV)
    //
//        // Size of return structure
//        Store(0x0006,F9SS)
    //
//        // Supported Displays Mask
//        Store(BSPD,F9SD)
    //
//        // Selected Displays Vector
//        Store(And(BPSD,3),F9DV)  // Only LFP, CRT supported by IGD in PX
    //
//        Return(TMP9)
//    }
    //
//    //
//    // Function 10: Set Selected Displays in CMOS
//    //
//    // Records Selected Displays in CMOS
//    //
//    If(LEqual(Arg0,0xA))
//    {
//        Name(TMPA,Buffer(256) {0x0})
    //
//      CreateWordField(Arg1,0,AIAS)
//      CreateWordField(Arg1,2,SDCM)
    //
//  // Records Selected Displays in CMOS
//  Store(And(SDCM,3),BPSD)  // Only LFP, CRT supported by IGD in PX
    //
//        Return(TMPA)
//    }
    //
//    //
//    // Function 12: Thermal Change Notification
//    //
//    // GPU temperature threshold related function
//    //
//    If(LEqual(Arg0,0xC))
//    {
//        Name(TMPC,Package() {0xFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF,0xFF})
//
//        // Size of return structure
//        Store(0x000A,Index(TMPC,0))
//
//        // Flags
////<TO DO>check        Store(0x00000003,Index(TMPC,1))
//
//        // High Temperature Threshold
////<TO DO>check        Store(0x00,Index(TMPC,2))
//
//        // Low Temperature Threshold
////<TO DO>check        Store(0x00,Index(TMPC,3))
//
//        // Thermal State At High Temperature Threshold
////<TO DO>check        Store(0x00,Index(TMPC,4))
//
//        // Thermal State At Low Temperature Threshold
////<TO DO>check        Store(0x00,Index(TMPC,5))
//
//        Return(TMPC)
//    }
    //
//    //
//    // Function 13: Temperature Change Notification
//    //
//    // GPU temperature threshold related function
//    //
//  If(LEqual(Arg0,0xD))
//  {
//          Name(TMPD,Package() {0xFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF,0xFF})
//        CreateWordField(Arg1,0,AIDS)
//        CreateWordField(Arg1,2,TGCI)
//        CreateByteField(Arg1,4,CGPT)
    //
//          Return(TMPD)
//  }
            Return (0)
    }

    Method(CTOI,1)
    {
        If(LNotEqual(NTLE, Zero))
        {
            If(LEqual(TLE1, Arg0))
            {
                Return(1)
            }
            If(LEqual(TLE2, Arg0))
            {
                Return(2)
            }
            If(LEqual(TLE3, Arg0))
            {
                Return(3)
            }
            If(LEqual(TLE4, Arg0))
            {
                Return(4)
            }
            If(LEqual(TLE5, Arg0))
            {
                Return(5)
            }
            If(LEqual(TLE6, Arg0))
            {
                Return(6)
            }
            If(LEqual(TLE7, Arg0))
            {
                Return(7)
            }
            If(LEqual(TLE8, Arg0))
            {
                Return(8)
            }
            If(LEqual(TLE9, Arg0))
            {
                Return(9)
            }
            If(LEqual(TL10, Arg0))
            {
                Return(10)
            }
            If(LEqual(TL11, Arg0))
            {
                Return(11)
            }
            If(LEqual(TL12, Arg0))
            {
                Return(12)
            }
            If(LEqual(TL13, Arg0))
            {
                Return(13)
            }
            If(LEqual(TL14, Arg0))
            {
                Return(14)
            }
            If(LEqual(TL15, Arg0))
            {
                Return(15)
            }
        }
        Return(1)   //If no match, then set TLSN to 1
    }

    Method(NATL,1)
    {
        If(LNotEqual(NTLE, Zero))
        {
            If(LEqual(Arg0,1))
            {
                Return(TLE2)
            }
            If(LEqual(Arg0,2))
            {
                Return(TLE3)
            }
            If(LEqual(Arg0,3))
            {
                Return(TLE4)
            }
            If(LEqual(Arg0,4))
            {
                Return(TLE5)
            }
            If(LEqual(Arg0,5))
            {
                Return(TLE6)
            }
            If(LEqual(Arg0,6))
            {
                Return(TLE7)
            }
            If(LEqual(Arg0,7))
            {
                Return(TLE8)
            }
            If(LEqual(Arg0,8))
            {
                Return(TLE9)
            }
            If(LEqual(Arg0,9))
            {
                Return(TL10)
            }
            If(LEqual(Arg0,10))
            {
                Return(TL11)
            }
            If(LEqual(Arg0,11))
            {
                Return(TL12)
            }
            If(LEqual(Arg0,12))
            {
                Return(TL13)
            }
            If(LEqual(Arg0,13))
            {
                Return(TL14)
            }
            If(LEqual(Arg0,14))
            {
                Return(TL15)
            }
            If(LEqual(Arg0,15))
            {
                Return(TLE1)
            }
        }
        Return(0)
    }
}
