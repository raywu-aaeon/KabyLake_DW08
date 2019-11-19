/** @file
  Intel ACPI Sample Code for Alto dGPU

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 - 2018 Intel Corporation.

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

// ATIF Common Functions 0-2
    
        //
        // Function 0: Verify Interface
        //
        // Returns the interface version and
        // functions/notifications supported by System BIOS
        //
        If(LEqual(Arg0,0))
        {
            ADBG("ATIF(0)")

            Name(TMP0,Buffer(256) {0x00})
            CreateWordField (TMP0, 0, F0SS)
            CreateWordField (TMP0, 2, F0IV)
            CreateDwordField (TMP0, 4, F0SN)
            CreateDwordField (TMP0, 8, F0SF)

            // Size of return structure
            Store(0x000C,F0SS)

            // Interface version
            Store(0x0001,F0IV)

            // Supported Notifications Mask
            Store(0x00000041,F0SN)  // Display switch request and PowerXpress graphics switch toggle request supported

            //Supported Functions Bit Vector
            Store(0x00000007,F0SF)

            // For Muxless: No ATIF Fn supported
            If(LEqual(And(SGMD,0x7F),0x2))
            {
              Store(0x00000000,F0SN)
              Store(0x00000000,F0SF)
            }

            If(LEqual(TDGS,1))
            {
              /*
                Bit 08 - Discrete GPU display connect/disconnect event is supported
                Bit 09 - Dock notification event is supported
                Bit 10 - Undock request is supported
                Bit 11 - Surprise undock notification event is supported
              */
              Store(0xA00, F0SN)

              // Functions 1, 2, 17, 18, 19, 20 are supported [Bit n, if set, indicates that Function n+1 is supported]
              Store(0xF0003,F0SF)
            }

            Return(TMP0)
        }

        //
        // Function 1: Get System Parameters
        //
        // Returns various system parameters
        //
        If(LEqual(Arg0,1))
        {
            ADBG("ATIF(1)")

            Name(TMP1,Buffer(256) {0x0})
            CreateWordField  (TMP1, 0, F1SS)
            CreateDwordField (TMP1, 2, F1VF)
            CreateDwordField (TMP1, 6, F1FG)
            CreateByteField  (TMP1, 10, F1NC)

            // Size of return structure
            Store(0x000B, F1SS)

            // Valid Fields Mask
            Store(0x0000000B,F1VF)

            // Flags
            Store(0x00000002,F1FG)  // Notify (VGA, 0xd0) is used as a general purpose notification
            //
            // TGOC: 0 - OC Disabled, set bit[4:3] to 01
            //       1 - OC Enabled, set bit[4:3] to 11
            //       2 - OC Auto, set bit[4:3] to 00
            //
            If(LEqual(TGOC,0))
            {
              And(F1FG,0xFFFFFFEF,F1FG)   // SBIOS disables overclocking (Clear BIT4)
              Or(F1FG,0x00000008,F1FG)   // SBIOS disables overclocking (Set BIT3)
            }
            Else
            {
              If(LEqual(TGOC,1))
              {
                Or(F1FG,0x00000018,F1FG) // SBIOS enables overclocking
             }
              Else
              {
                If(LEqual(TGOC,2))
                {
                  And(F1FG,0xFFFFFFE7,F1FG)   // SBIOS overclocking set to AUTO (Clear BIT3 and BIT4)
                }
              }
            }
	    
            //This field is valid only when the Flags field bits 1-0 is are set to 10.
            //Indicates that Notify (VGA, n) is used as a general purpose notification, where n
            //is one of: 0xd0 to 0xd9.
            //Range: 0xd0 to 0xd9.
            Store (0xd0, F1NC);    //valid Field

            Return(TMP1)
        }

        //
        // Function 2: Get System BIOS Requests
        //
        // Reports pending system BIOS requests
        //
        // Invoked whenever driver receives Notify(VGA,0x81) and
        // the Notify is designated as a general purpose notification
        // in the function "Get System Parameters"
        //
        If(LEqual(Arg0,2))
        {
            ADBG("ATIF(2)")

            Name(TMP2,Buffer(256) {0x0})
            CreateWordField (TMP2, 0 , F2SS)
            CreateDwordField (TMP2, 2 , F2PR)
            CreateByteField (TMP2, 6, F2EM)
            CreateByteField (TMP2, 7, F2TG)
            CreateByteField (TMP2, 8, F2SI)
            CreateByteField (TMP2, 9, F2FG)
            CreateByteField (TMP2, 10, F2FI)
            CreateByteField (TMP2, 11, F2SP)

            // Size of return structure
            Store(0x000A,F2SS)

            // Initialize Pending System BIOS Requests field
            /*
              Bit 08 - Discrete GPU display connect state change request
              Bit 09 - Dock event
              Bit 10 - Undock request
              Bit 11 - Surprise undock event
            */
            Name(STS2,0x00000000)

            Or(STS2, ShiftLeft(DCKE, 9), STS2)
            Or(STS2, ShiftLeft(UDCK, 10), STS2)
            Or(STS2, ShiftLeft(SUDK, 11), STS2)

            // Display switch request
            Or(DSWR,STS2,STS2)
            // PowerXpress graphics switch toggle request
            Or(ShiftLeft(PXGS,6),STS2,STS2)

//<Overriding as per edited spec - only display switch request supported>
//    Expansion Mode
//    Or(ShiftLeft(EMDR,1),STS2,STS2)
//    Store(EXPM,F2EM)
//
//    // Thermal State: Target Graphics Controller
//    Or(ShiftLeft(TSTR,2),STS2,STS2)
//        Store(TSTG,F2TG)
//        // Thermal State: State Id
//        Store(TSSI,F2SI)
//
//        // Forced Power State: Target Graphics Controller
//  Or(ShiftLeft(FPSR,3),STS2,STS2)
//        Store(FPTG,F2FG)
//        // Forced Power State: State Id
//        Store(FPSI,F2FI)
//
//        // System Power Source
//  Or(ShiftLeft(SPSR,4),STS2,STS2)
//        Store(SPWS,F2SP)
//
//  // Display configuration change request
//  Or(ShiftLeft(DCFR,5),STS2,STS2)

            Store(STS2,F2PR)
            Store(0,DSWR)  // Clear Display switch request
            Store(0,PXGS)  // Clear PowerXpress graphics switch toggle request

            Store(0, DCKE)
            Store(0, UDCK)
            Store(0, SUDK)

//<Overriding as per edited spec - only display switch request supported>
//  Store(0,EMDR)  // Clear Expansion mode change request indicator
//  Store(0,TSTR)  // Clear Thermal state change request indicator
//  Store(0,FPSR)  // Clear Forced power state change request indicator
//  Store(0,SPSR)  // Clear System power source change request indicator
//  Store(0,DCFR)  // Clear Display configuration change request indicator
//  Store(0,LDST)  // Clear Lid switch toggle

            Return(TMP2)
        }


