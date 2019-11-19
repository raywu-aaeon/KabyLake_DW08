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

External(\_SB.PCI0.PEG0.PEGP._OFF, MethodObj)
External(\_SB.PCI0.PEG0.PEGP._ON, MethodObj)
//External(MXD1)
//External(MXD2)
//External(MXD3)
//External(MXD4)
//External(MXD5)
//External(MXD6)
//External(MXD7)
//External(MXD8)
External(DID1)
External(DID2)
External(DID3)
External(DID4)
External(DID5)
External(DID6)
External(DID7)
//External(DSEL)
//External(ESEL)
External(SGMD)
External(SGFL)
External(SGGP)
External(TDGS)
External(DCKE)
External(UDCK)
External(SUDK)
External(EGPC)
External(EGPV)
External(TBDT)

Scope(\_SB.PCI0.GFX0)
{

    OperationRegion(APXM,SystemMemory,AMDA,0x10400)
    Field(APXM, AnyAcc, NoLock, Preserve)
    {
        // OpRegion Header

        APSG,   0x80,               // (000h) Signature-"AMD--PowerXpress".
        APSZ,   0x20,               // (010h) OpRegion Size.
        APVR,   0x20,               // (014h) OpRegion Version.

        // OpRegion Data
        APXA,   0x20,    // PX opregion address
        RVBS,   0x20,    // PX Runtime VBIOS image size
        NTLE,   0x10,    // Total number of toggle list entries
        TLE1,   0x10,    // The display combinations in the list...
        TLE2,   0x10,
        TLE3,   0x10,
        TLE4,   0x10,
        TLE5,   0x10,
        TLE6,   0x10,
        TLE7,   0x10,
        TLE8,   0x10,
        TLE9,   0x10,
        TL10,   0x10,
        TL11,   0x10,
        TL12,   0x10,
        TL13,   0x10,
        TL14,   0x10,
        TL15,   0x10,
        TGXA,   0x10,    // Target GFX adapter as notified by ATPX function 5
        AGXA,   0x10,    // Active GFX adapter as notified by ATPX function 6
        GSTP,   0x08,    // GPU switch transition in progress
        DSWR,   0x08,    // Display switch request
        EMDR,   0x08,    // Expansion mode change request
        PXGS,   0x08,    // PowerXpress graphics switch toggle request
        CACD,   0x10,    // Currently active displays
        CCND,   0x10,    // Currently connected displays
        NACD,   0x10,    // Next active displays
        EXPM,   0x08,    // Expansion Mode
        TLSN,   0x10,    // Toggle list sequence index
        ELCT,   0x10,    // Endpoint Link Contol Register Value
  // for _ATRM (_ROM equivalent) data
        RBF1,   0x40000, // 0x8000 bytes in bits
        RBF2,   0x40000  // 0x8000 bytes in bits
    }

    //
    // ADPM -> ATPX Fn 8 Digital port mux mode
    //
    // Arg0 : Integer   User selected option (via., setup 0 -> Shared, 1 -> iGPU Only, 2 -> dGPU Only)
    // Arg1 : 1 -> iGPU connector record, 2->dgpu connector record
    //
    // Returns : Flag value for ATPX Fn 8.
    //          Bit0-> display can be driven by the GPU
    //          Bit1-> HPD can be detected by the GPU
    //          Bit2-> AUX/DDC can be driven by the GPU
    //
    Method(ADPM, 2, Serialized)
    {
        Store(0, Local1)

        // AUX/DDC Mux settings
        ShiftRight(Arg0, 16, Local0)

        if(LEqual(Arg1, 1))  // If iGPU connector record
        {
            Or (And(Local0, 0x1), Local1, Local1)
        }
        else // If dGPU connector record
        {
            Or (ShiftRight(And(Local0, 0x2),1), Local1, Local1)
        }
        ShiftLeft(Local1, 1, Local1)


        // HPD Mux settings
        ShiftRight(Arg0, 24, Local0)

        if(LEqual(Arg1, 1))  // If iGPU connector record
        {
            Or (And(Local0, 0x1), Local1, Local1)
        }
        else // If dGPU connector record
        {
            Or (ShiftRight(And(Local0, 0x2),1), Local1, Local1)
        }
        ShiftLeft(Local1, 1, Local1)


        // Display Mux settings
        ShiftRight(Arg0, 8, Local0)

        if(LEqual(Arg1, 1))  // If iGPU connector record
        {
            Or (And(Local0, 0x1), Local1, Local1)
        }
        else // If dGPU connector record
        {
            Or (ShiftRight(And(Local0, 0x2),1), Local1, Local1)
        }

        Return (Local1)

    }

    //  PowerXpress (PX) Contrl Method:
    //  PX specific Control Method used by integrated graphics
    //      or discrete graphics driver on PX enabled platforms.
    //
    //  Arguments:
    //    Arg0:  Integer     Function code.
    //    Arg1:   Buffer      Parameter buffer, 256 bytes
    //
    //  Return Value:
    //    Returns Buffer      256 bytes

    Method(ATPX,2,Serialized)
    {

        ADBG("ATPX")
        //
        // Function 0: Verify PowerXpress Interface
        //
        // Returns the PX interface version and
        // functions supported by PX System BIOS
        //
        If(LEqual(Arg0,0))
        {
            ADBG("ATPX(0)")
            Name(TMP1,Buffer(256) {0x00})
            CreateWordField ( TMP1, 0, F0SS)
            CreateWordField ( TMP1, 2, F0IV)
            CreateDwordField( TMP1, 4, F0SF)

            Store(0x0008,F0SS)
            Store(0x0001,F0IV)
            Store(0x000000BF,F0SF)
            // For Muxless: Support only Fun1, Fun2, Fun5 and Fun6
            If(LEqual(And(SGMD,0x7F),0x2))
            {
              Store(0x00000033,F0SF)
            }
            Return(TMP1)
        }

        //
        // Function 1: Get PowerXpress Parameters
        //
        // Returns various PX related platform parameters
        //
        If(LEqual(Arg0,1))
        {
            ADBG("ATPX(1)")

            Name(TMP2,Buffer(256) {0x00})
            CreateWordField ( TMP2, 0, F1SS)
            CreateDwordField ( TMP2, 2, F1VM)
            CreateDwordField( TMP2, 6, F1FG)
            Store(0x000A,F1SS)  //Structure size of return package

            //BIT6 is the valid field. Set BIT6
            Store(0x00000040,F1VM)  // Mask used for valid bit fields
            Store(0x00000040,F1FG)  // Actual PX parameters field

            // For Muxless: Set BIT7 for dynamic PX scheme is supported
            If(LEqual(And(SGMD,0x7F),0x2))
            {
              If(LNotEqual(SGGP,0x0))
              {                                 // Bit 7 = 1 :Dynamic mode supported, Bit 9 = 1 : Fixed Mode Not Supported, Bit 10 = 1 :dGPU power off
                                                // Bit 14 = 1 : Windows Blue "Hybrid Graphics"is supported
                Store(0x00004680,F1VM)          // Mask used for valid bit fields
                Store(0x00004680,F1FG)          // Actual PX parameters field
              }
              Else
              {                                 // Bit 7 = 1 :Dynamic mode supported, Bit 9 = 1 : Fixed Mode Not Supported, Bit 10 = 0 :No dGPU power off
                                                // Bit 14 = 1 : Windows Blue "Hybrid Graphics" is supported
                Store(0x00004280,F1VM)          // Mask used for valid bit fields
                Store(0x00004280,F1FG)          // Actual PX parameters field
              }
            }

            If(LEqual(TDGS,1))
            {
              // Bit 7: Indicates that a "dynamic" PX scheme is supported.
              // Bit 9: Indicates that "fixed" scheme is not supported, if set to one.
              // Bit 10: Indicates that full dGPU power off in "dynamic" scheme is supported, if set to one
              // Bit 12: Indicates that discrete graphics can drive display outputs (local dGPU displays are supported), if set to one.
              // Bit 13: Indicates that long idle detection is disabled, if set to one.
              // Bit 14: Indicates that Win Blue "Hybrid Graphics" is required (supported), if set to one.
              // Bit 16: Indicates that Detachable Graphics is supported, if set to one.
              // Bit 17: Indicated that discrete graphics monitor Hot Plug Detect is required, if set
              Store(0x00037680,F1VM)            // Mask used for valid bit fields
              Store(0x00037680,F1FG)            // Actual PX parameters field
            }

            Return(TMP2)
        }

        //
        // Function 2: Power Control
        //
        // Powers on/off the discrete graphics
        //
        If(LEqual(Arg0,2))
        {
            ADBG("ATPX(2)")

            CreateWordField(Arg1,0,FN2S)
            CreateByteField(Arg1,2,DGPR)

            If(LOr(LEqual(SGGP,0x01),LEqual(SGGP,0x02)))
            {
              If(LEqual(DGPR,0))  // Powers off discrete graphics
              {
                ADBG("Power.Off.dGPU")
                \_SB.PCI0.PEG0.PEGP._OFF()
              }
              If(LEqual(DGPR,1))  // Powers on discrete graphics
              {
                ADBG("Power.On.dGPU")
                \_SB.PCI0.PEG0.PEGP._ON()
              }
            }
            Return(0)
        }

        //
        // Function 3: Display Multiplexer Control
        //
        // Controls display multiplexers
        //
        If(LEqual(Arg0,3))
        {
            CreateWordField(Arg1,0,FN3S)
            CreateWordField(Arg1,2,SDMG)

            // Display Multiplexer Control
            If(LEqual(SDMG,0))  // Switch Display Muxes to iGFX
            {
//                \_SB.PCI0.PEG0.PEGP.SGPO(DSEL, 0)
            }
            If(LEqual(SDMG,1))  // Switch Display Muxes to dGFX
            {
//                \_SB.PCI0.PEG0.PEGP.SGPO(DSEL, 1)
            }

            Return(0)
        }

        //
        // Function 4: Monitor I2C Control
        //
        // Controls monitor I2C lines
        //
        If(LEqual(Arg0,4))
        {
            CreateWordField(Arg1,0,FN4S)
            CreateWordField(Arg1,2,SIMG)

            // I2C Multiplexer Control
            If(LEqual(SIMG,0))  // Switch I2C Muxes to iGFX
            {
//                \_SB.PCI0.PEG0.PEGP.SGPO(ESEL,0)
            }
            If(LEqual(SIMG,1))  // Switch I2C Muxes to dGFX
            {
//                \_SB.PCI0.PEG0.PEGP.SGPO(ESEL,1)
            }

            Return(0)
        }

        //
        // Function 5: Graphics Device Switch Start Notification
        //
        // Notifies SBIOS that GFX device switch process has been started
        //
        If(LEqual(Arg0,5))
        {
            ADBG("ATPX(5)")

            CreateWordField(Arg1,0,FN5S)
            CreateWordField(Arg1,2,TGFX)
            Store(TGFX,TGXA)
            Store(1,GSTP)

            Return(0)
        }

        //
        // Function 6: Graphics Device Switch End Notification
        //
        // Notifies SBIOS that GFX device switch process has been completed
        //
        If(LEqual(Arg0,6))
        {
            ADBG("ATPX(6)")

            CreateWordField(Arg1,0,FN6S)
            CreateWordField(Arg1,2,AGFX)
            Store(AGFX,AGXA)
            Store(0,GSTP)

            Return(0)
        }

        //
        // Function 8: Get Display Connectors mapping
        //
        If(LEqual(Arg0,8))
        {
            Name(TMP3,Buffer(256) {
            0x0e,0x00,        //Number of reported display connectors
            0x46,0x00,        //Total Connector structure size in bytes (num of structures * structure size)
            0x07,0x01,0x00,0x00,0x01,   //Connector structure 1 - CRT on iGPU
            0x07,0x01,0x01,0x00,0x01,   //Connector structure 2 - CRT on dGPU
            0x05,0x00,0x00,0x00,0x04,   //Connector structure 3 - LFP on iGPU
            0x05,0x00,0x01,0x10,0x01,   //Connector structure 4 - LFP on dGPU

            // Digital port mapping on EC4
            //
            // Intel        Alto        EC4 output
            // Port B ->    Port B      DP
            // Port C ->    Port C      HDMI
            // Port D ->    Port D      DP
            //

            0x07,0x03,0x00,0x00,0x03, //Connector structure 5 - DisplayPort_B on iGPU
            0x07,0x03,0x01,0x10,0x02, //Connector structure 6 - DP on dGPU (MXM port B on EC4)
            0x07,0x07,0x00,0x01,0x03, //Connector structure 7 - HDMI/DVI dongle on port B
            0x07,0x07,0x01,0x10,0x02, //Connector structure 8 - HDMI/DVI dongle on dGPU (MXM port B on EC4)
            0x07,0x09,0x00,0x02,0x03, //Connector structure 9 - HDMI_C on iGPU
            0x07,0x09,0x01,0x20,0x02, //Connector structure 10 - HDMI on dGPU (MXM port C on EC4)
            0x07,0x0a,0x00,0x03,0x03, //Connector structure 11 - DisplayPort_D on iGPU
            0x07,0x0a,0x01,0x30,0x02, //Connector structure 12 - DP on dGPU (MXM port B. Not routed to EC4)
            0x07,0x0b,0x00,0x04,0x03, //Connector structure 13 - HDMI/DVI dongle on port D
            0x07,0x0b,0x01,0x30,0x02, //Connector structure 14 - HDMI/DVI dongle on dGPU (MXM port B. Not routed to EC4)
            0x07,0x0C,0x00,0x06,0x03, //Connector structure 15 - Place holder for Intel Wireless Display
            })

            CreateWordField (TMP3, 0, ATNO)
            CreateWordField (TMP3, 2, ATSZ)

            //Modify the display, HPD and Aux/DDC flag in the connector structure based on iGPU Digital port setup option

            //Connector structure 3 - LFP on iGPU
//            Store(ADPM(MXD2, 1), Index(TMP3, 14))

            //Connector structure 4 - LFP on dGPU
//            Store(ADPM(MXD2, 2), Index(TMP3, 19))

            //Connector structure 5 - DisplayPort_B on iGPU
//            Store(ADPM(MXD3, 1), Index(TMP3, 24))

            //Connector structure 6 - DP on dGPU
//            Store(ADPM(MXD3, 2), Index(TMP3, 29))

            //Connector structure 7 - HDMI/DVI dongle on port B
//            Store(ADPM(MXD4, 1), Index(TMP3, 34))

            //Connector structure 8 - HDMI/DVI dongle on dGPU
//            Store(ADPM(MXD4, 2), Index(TMP3, 39))

            //Connector structure 9 - HDMI_C on iGPU
//            Store(ADPM(MXD5, 1), Index(TMP3, 44))

            //Connector structure 10 - HDMI on dGPU
//            Store(ADPM(MXD5, 2), Index(TMP3, 49))

            //Connector structure 11 - DisplayPort_D on iGPU
//            Store(ADPM(MXD6, 1), Index(TMP3, 54))

            //Connector structure 12 - DP on dGPU
//            Store(ADPM(MXD6, 2), Index(TMP3, 59))

            //Connector structure 13 - HDMI/DVI dongle on port D
//            Store(ADPM(MXD7, 1), Index(TMP3, 64))

            //Connector structure 14 - HDMI/DVI dongle on dGPU
//            Store(ADPM(MXD7, 2), Index(TMP3, 69))

            If(And(SGFL, 0x01))
            {
              Store(Add(ATNO, 0x1), ATNO)
              Store(Add(ATSZ, 0x5), ATSZ)
            }

            Return(TMP3)
        }

        Return(0)   //End of ATPX
    }

    // PowerXpress (PX) get ROM Method:
    //  PX specific Control Method used by discrete graphics driver
    //      on PX enabled platforms to get a runtime modified copy of
    //      the discrete graphics device ROM data (Video BIOS).
    //
    //  Arguments:
    //    Arg0:  Integer     Offset of the graphics device ROM data
    //        Arg1:   Integer     Size of the buffer to fill in (up to 4K)
    //
    //  Return Value:
    //    Returns Buffer      Buffer of requested video ROM bytes

    Method(ATRM,2,Serialized)
    {
        ADBG("ATRM")

        Store(Arg0,Local0)
        Store(Arg1,Local1)

        If(LGreater(Local1,0x1000))
        {
            Store(0x1000,Local1)
        }
        If(LGreater(Local0,0x10000))
        {
            Return(Buffer(Local1){0})
        }
        If(LGreater(Local0,RVBS))
        {
            Return(Buffer(Local1){0})
        }
//  If (LGreater (Add (Local0, Local1), RVBS))
//  {
//    Store (0x00, Local0)
//  }

        Multiply(Local1,0x08,Local3)
        Name(ROM1,Buffer(0x8000){0})
        Name(ROM2,Buffer(Local1){0})

        If(LLess(Local0,0x8000))
        {
            Store(RBF1,ROM1)
        }
        Else
        {
            Subtract(Local0,0x8000,Local0)
            Store(RBF2,ROM1)
        }
        Multiply(Local0,0x08,Local2)
        CreateField(ROM1,Local2,Local3,TMPB)
        Store(TMPB,ROM2)
        Return(ROM2)

    }
}
