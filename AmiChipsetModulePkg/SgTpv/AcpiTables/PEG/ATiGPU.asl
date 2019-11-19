//****************************************************************************
//****************************************************************************
//**                                                                        **
//**         (C)Copyright 1985-2013, American Megatrends, Inc.              **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//****************************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPEG/ATiGPU.asl 2     2/21/13 5:46a Joshchou $
//
// $Revision: 2 $
//
// $Date: 2/21/13 5:46a $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPEG/ATiGPU.asl $
// 
// 
// 
//**********************************************************************
External(\ECON, IntObj) // Embedded Controller Availability Flag.
External(ASL_PCI_SCOPE.LPCB.H_EC.LSTE)
External(MXD1)
External(MXD2)
External(MXD3)
External(MXD4)
External(MXD5)
External(MXD6)
External(MXD7)
External(MXD8)

External(NXD1)
External(NXD2)
External(NXD3)
External(NXD4)
External(NXD5)
External(NXD6)
External(NXD7)
External(NXD8)

External(AMDA)
External(SGMD)
External(SGFL)
External(OSYS)

Scope (ASL_IGPU_SCOPE)
{
/*
    Method(_INI,0)
    {
// Init all scratch pad fields if not already done so in OpRegion Init 
    }
*/

//<AMI_SHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    APXM
//
// Description:  AMD PowerExpress OperationRegion. 
//               OpRegion address (AMDA)is defined in IDG Nvs OpRegion
//
//-------------------------------------------------------------------------
//<AMI_SHDR_END>    
    OperationRegion(APXM,SystemMemory,AMDA,0x10400)
    Field(APXM, AnyAcc, NoLock, Preserve)
    {
        // OpRegion Header

        APSG,   0x80,    // (000h) Signature-"AMD--PowerXpress".
        APSZ,   0x20,    // (010h) OpRegion Size.
        APVR,   0x20,    // (014h) OpRegion Version.

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
        ELCL,   0x10,    // Endpoint Link Contol Register Value
	// for ATRM (_ROM equivalent) data
        RBF1,   0x40000, // 0x8000 bytes in bits
        RBF2,   0x40000  // 0x8000 bytes in bits
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    ADPM
//
// Description:  ADPM -> ATPX Fn 8 Digital port mux mode
//
// Input:
//      Arg0 : Integer   User selected option (via., setup 0 -> Shared, 1 -> iGPU Only, 2 -> dGPU Only)
//      Arg1 : 1 -> iGPU connector record, 2->dgpu connector record
//
// Output: 
// 		Flag value for ATPX Fn 8. 
//          Bit0-> display can be driven by the GPU  
//          Bit1-> HPD can be detected by the GPU
//          Bit2-> AUX/DDC can be driven by the GPU    
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
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

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    ATPX
//
// Description:  ATI PowerXpress (PX) Contrl Method: Revision 0.19
//	            PX specific Control Method used by integrated graphics
//              or discrete graphics driver on PX enabled platforms.
//
// Input:
//		Arg0:	Integer     Function code.
//		Arg1:   Buffer      Parameter buffer, 256 bytes
//
//  Output:
//		Returns Buffer      256 bytes
//-------------------------------------------------------------------------
//<AMI_PHDR_END> 
    Method(ATPX,2,Serialized)
    {
        P8DB(0xA1, Arg0, 2000)
        //
        // Function 0: Verify PowerXpress Interface
        //
        // Returns the PX interface version and
        // functions supported by PX System BIOS
        //
        If(LEqual(Arg0,0))
        {
            Name(TMP1,Buffer(256) {0x00})
            CreateWordField ( TMP1, 0, F0SS)
            CreateWordField ( TMP1, 2, F0IV)
            CreateDwordField( TMP1, 4, F0SF)
         
            Store(0x0008,F0SS)
            Store(0x0001,F0IV)
            Store(0x000000BF,F0SF)

            // For Muxless: Support only Fun1, Fun2, Fun5 and Fun6
            If(LEqual(SGMD,0x2))
            {

              Store(0x00000033,F0SF)
/*
	        	If (LEqual(PXDY, 0x01)) {	// PX Dynamic Mode Switch Enabled
		        And(F0SF, 0xFFFFFFFD, F0SF)			// Don't support PX02
		        }
  		        If (LAnd(LEqual(PXDY, 0x01), // Support both Dynamic and Fixed PX switch
                         LEqual(PXFX, 0x01))) {
		        Or(F0SF, 0x2, F0SF)				 // Support PX02
		        }
*/
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
            Name(TMP2,Buffer(256) {0x00})
            CreateWordField ( TMP2, 0, F1SS)
            CreateDwordField ( TMP2, 2, F1VM)
            CreateDwordField( TMP2, 6, F1FG)
            Store(0x000A,F1SS)	//Structure size of return package
//            Store(0x000007FF,F1VM)	// Bit[10:0]Mask used for valid bit fields
            Store(0x00007FFF,F1VM)  // Bit[12:0]Mask used for valid bit fields            
        
            // Bit0=1: LVDS I2C is accessible to both graphics controllers.
			// Bit1=1: CRT1 I2C is accessible to both graphics controllers.
			// Bit2=0: DVI1 I2C is accessible to both graphics controllers.
			// Bit3=1: CRT1 RGB signals are multiplexed.
			// Bit4=0: TV1 signals are multiplexed.
			// Bit5=0: DFP1 signals are multiplexed.
			// Bit6=1: Indicates that a separate multiplexer control for I2C/Aux/HPD exists.
			// Bit7=1: Indicates that a "dynamic" PX scheme is supported.
			// Bit8=x: Reserved.
			// Bit9=1: Indicates that gfixedh scheme is not supported, if set to one.
			// Bit10=1: Indicates that full dGPU power off in gdynamich scheme is supported, if set to one.
			// Bits[31:11]: Reserved (must be zero).    

            // For Muxless: Set BIT7 for dynamic" PX scheme is supported
            If(LEqual(SGMD,0x2))
            {
              Store(0x00004680,F1FG) 
              If(LLess(OSYS,2013)) {
                Store(0x00001E80,F1FG)      // dGPU Power off under the Dynamic Scheme 
              }
            }
           Else
           {
             // For Muxed: Set BIT6  to Indicates that a separate multiplexer control for I2C/Aux/HPD exists 
             //   and is controlled by function 4 (Monitor I2C Control).
             Store(0x00000040,F1FG)	// Actual PX parameters field
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
            CreateWordField(Arg1,0,FN2S)
            CreateByteField(Arg1,2,DGPR)
    
            If(LEqual(DGPR,0))	// Powers off discrete graphics
            {
                ASL_DGPU_SCOPE._OFF()
            }
            If(LEqual(DGPR,1))	// Powers on discrete graphics
            {
                ASL_DGPU_SCOPE._ON()
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
            If(LEqual(SDMG,0))	// Switch Display Muxes to iGFX
            {
//                ASL_DGPU_SCOPE.SGPO(DSEL, 0)
            }
            If(LEqual(SDMG,1))	// Switch Display Muxes to dGFX
            {
//                ASL_DGPU_SCOPE.SGPO(SSEL, 1)
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
            If(LEqual(SIMG,0))	// Switch I2C Muxes to iGFX
            {
//                ASL_DGPU_SCOPE.SGPO(ESEL, 0)
            }
            If(LEqual(SIMG,1))	// Switch I2C Muxes to dGFX
            {
//                ASL_DGPU_SCOPE.SGPO(ESEL, 1)
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
            // Intel        ATI         EC4 output
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
            Store(ADPM(MXD2, 1), Index(TMP3, 14))

            //Connector structure 4 - LFP on dGPU
            Store(ADPM(MXD2, 1), Index(TMP3, 19))

            //Connector structure 5 - DisplayPort_B on iGPU
            Store(ADPM(MXD3, 1), Index(TMP3, 24))
    
            //Connector structure 6 - DP on dGPU
            Store(ADPM(MXD3, 2), Index(TMP3, 29))

            //Connector structure 7 - HDMI/DVI dongle on port B
            Store(ADPM(MXD4, 1), Index(TMP3, 34))

            //Connector structure 8 - HDMI/DVI dongle on dGPU
            Store(ADPM(MXD4, 2), Index(TMP3, 39))

            //Connector structure 9 - HDMI_C on iGPU
            Store(ADPM(MXD5, 1), Index(TMP3, 44))
    
            //Connector structure 10 - HDMI on dGPU
            Store(ADPM(MXD5, 2), Index(TMP3, 49))
            
            //Connector structure 11 - DisplayPort_D on iGPU
            Store(ADPM(MXD6, 1), Index(TMP3, 54))

            //Connector structure 12 - DP on dGPU
            Store(ADPM(MXD6, 2), Index(TMP3, 59))

            //Connector structure 13 - HDMI/DVI dongle on port D
            Store(ADPM(MXD7, 1), Index(TMP3, 64))

            //Connector structure 14 - HDMI/DVI dongle on dGPU
            Store(ADPM(MXD7, 2), Index(TMP3, 69))
            
            If(And(SGFL, 0x01))
            {
              Store(Add(ATNO, 0x1), ATNO)
              Store(Add(ATSZ, 0x5), ATSZ)
            }

            Return(TMP3)
        }

        Return(0)   //End of ATPX
    }
    
//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    ATRM
//
// Description:  ATI PowerXpress (PX) get ROM Method: Revision 0.19
//	            PX specific Control Method used by discrete graphics driver
//              on PX enabled platforms to get a runtime modified copy of
//              the discrete graphics device ROM data (Video BIOS).
//
// Input:
//          Arg0:   Integer     Offset of the graphics device ROM data
//          Arg1:   Integer     Size of the buffer to fill in (up to 4K)
//
//  Output:
//          Buffer      Buffer of requested video ROM bytes
//-------------------------------------------------------------------------
//<AMI_PHDR_END> 
   
    Method(ATRM,2,Serialized)
    {
        Store(Arg0,Local0)
        Store(Arg1,Local1)

        P8DB(0x44, ShiftRight(Local0, 8), 1000)

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
//	If (LGreater (Add (Local0, Local1), RVBS))
//	{
//		Store (0x00, Local0)
//	}

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

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    ATIF
//
// Description:  ATI GFX Interface.Provides ATI specific GFX functionality on mobile platforms.
//
// Input:
//      Arg0:    Integer    Function code.
//      Arg1:    Package    Parameter buffer, 256 bytes
//
//
// Output:
//      Returns Buffer, 256 bytes
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
    Method(ATIF,2,Serialized)
    {
        P8DB(0xAF, Arg0, 2000)

        //
        // Function 0: Verify Interface
        //
        // Returns the interface version and
        // functions/notifications supported by System BIOS
        //
        If(LEqual(Arg0,0))
        {
            Name(TMP0,Buffer(256) {0x00})
            CreateWordField (TMP0, 0, F0SS)
            CreateWordField (TMP0, 2, F0IV)
            CreateDwordField (TMP0, 4, F0SN)
            CreateDwordField (TMP0, 8, F0SF)

            // Size of return structure=12
            Store(0x000C,F0SS)

            // Interface version
            Store(0x0001,F0IV)

            // Supported Notifications Mask
            Store(0x00000041,F0SN)	// Display switch request and PowerXpress graphics switch toggle request supported
//            Store(0x00000001,F0SN)	//<Overriding as per edited ATIF spec 0.22- only display switch request supported>>

            //Supported Functions Bit Vector
            Store(0x00000007,F0SF)

/*
            // For Muxless: No ATIF Fn supported
            If(LEqual(SGMD,0x2))
            {
              Store(0x00000000,F0SN)
              Store(0x00000000,F0SF)
            }
*/
            Return(TMP0)
        }

        //
        // Function 1: Get System Parameters
        //
        // Returns various system parameters
        //
        If(LEqual(Arg0,1))
        {
            Name(TMP1,Buffer(256) {0x0})
            CreateWordField  (TMP1, 0, F1SS)
            CreateDwordField (TMP1, 2, F1VF) 
            CreateDwordField (TMP1, 6, F1FG)

            // Size of return structure=10
            Store(0x000A, F1SS)

            // Valid Fields Mask
            Store(0x00000003,F1VF)

            // Flags
            Store(0x00000001,F1FG)	// Notify (VGA, 0x81) is used as a general purpose notification
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
            Name(TMP2,Buffer(256) {0x0})
            CreateWordField (TMP2, 0 , F2SS)
            CreateDwordField (TMP2, 2 , F2PR)
            CreateByteField (TMP2, 6, F2EM)
            CreateByteField (TMP2, 7, F2TG)
            CreateByteField (TMP2, 8, F2SI)
            CreateByteField (TMP2, 9, F2FG)
            CreateByteField (TMP2, 10, F2FI)
            CreateByteField (TMP2, 11, F2SP)

            // Size of return structure=11
//            Store(0x000A,F2SS)
            Store(0x000B,F2SS)

            // Initialize Pending System BIOS Requests field
            Name(STS2,0x00000000)

            // Display switch request
            Or(DSWR,STS2,STS2)
            // PowerXpress graphics switch toggle request
            Or(ShiftLeft(PXGS,6),STS2,STS2)

//<Overriding as per edited ATIF spec 0.22- only display switch request supported>>
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
//	Or(ShiftLeft(FPSR,3),STS2,STS2)
//        Store(FPTG,F2FG)
//        // Forced Power State: State Id
//        Store(FPSI,F2FI)
//
//        // System Power Source
//	Or(ShiftLeft(SPSR,4),STS2,STS2)
//        Store(SPWS,F2SP)
//
//	// Display configuration change request
//	Or(ShiftLeft(DCFR,5),STS2,STS2)

            Store(STS2,F2PR)
            Store(0,DSWR)	// Clear Display switch request
            Store(0,PXGS)	// Clear PowerXpress graphics switch toggle request

//<Overriding as per edited ATIF spec 0.22- only display switch request supported>>
//	Store(0,EMDR)	// Clear Expansion mode change request indicator
//	Store(0,TSTR)	// Clear Thermal state change request indicator
//	Store(0,FPSR)	// Clear Forced power state change request indicator
//	Store(0,SPSR)	// Clear System power source change request indicator
//	Store(0,DCFR)	// Clear Display configuration change request indicator
//	Store(0,LDST)	// Clear Lid switch toggle

            Return(TMP2)
        }

        //
        // Function 3: Select Active Displays
        //
        // Returns displays to be selected in reposnse to
        // a display switch request notification
        //
        If(LEqual(Arg0,3))
        {
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
               If(LEqual(ASL_PCI_SCOPE.LPCB.H_EC.LSTE,1))
               {
                 Or(CCND,0x0001,CCND)    // ATI does not send LFP as connected when not LFP is not active. This is as per design
               }
            }

            // Size of return structure=4
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
                    If(LEqual(And(Local1,CCND),Local1))	// If entries in the next combination are actually connected..
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

//<Overriding as per edited ATIF spec 0.22- only Functions 0,1,2,3 supported>
//    //
//    // Function 5: Get TV Standard from CMOS
//    //
//    // Retrieves current TV standard
//    //
//    If(LEqual(Arg0,5))
//    {
//	Name(TMP5,Buffer(256) {0x0})
//	CreateWordField (TMP5, 0, F5SS)
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
//    	CreateWordField(Arg1,0,AI6S)
//    	CreateByteField(Arg1,2,TSEF)
//    	CreateByteField(Arg1,3,TVST)
//	
//	// Records current TV standard in CMOS
//	Store(TVST,TVSD)
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
//	Name(TMP7,Buffer(256) {0x0})
//	CreateWordField (TMP7, 0 , F7SS)
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
//    	CreateWordField(Arg1,0,AI8S)
//    	CreateByteField(Arg1,2,EMCM)
    //
//	// Record Expansion Mode in CMOS
//	Store(EMCM,EXPM)
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
//	Name(TMP9,Buffer(256) {0x0})
//	CreateWordField (TMP5, 0, F9SS)
//	CreateWordField (TMP5, 2, F9SD)
//	CreateWordField (TMP5, 4, F9DV)
    //
//        // Size of return structure
//        Store(0x0006,F9SS)
    //
//        // Supported Displays Mask
//        Store(BSPD,F9SD)
    //
//        // Selected Displays Vector
//        Store(And(BPSD,3),F9DV)	// Only LFP, CRT supported by IGD in PX
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
//    	CreateWordField(Arg1,0,AIAS)
//    	CreateWordField(Arg1,2,SDCM)
    //
//	// Records Selected Displays in CMOS
//	Store(And(SDCM,3),BPSD)	// Only LFP, CRT supported by IGD in PX
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
//        Store(0x00000003,Index(TMPC,1))
//
//        // High Temperature Threshold
///        Store(0x00,Index(TMPC,2))
//
//        // Low Temperature Threshold
//        Store(0x00,Index(TMPC,3))
//
//        // Thermal State At High Temperature Threshold
//        Store(0x00,Index(TMPC,4))
//
//        // Thermal State At Low Temperature Threshold
//        Store(0x00,Index(TMPC,5))
//
//        Return(TMPC)
//    }
    //
//    //
//    // Function 13: Temperature Change Notification
//    //
//    // GPU temperature threshold related function
//    //
//	If(LEqual(Arg0,0xD))
//	{
//	        Name(TMPD,Package() {0xFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF,0xFF})
//	    	CreateWordField(Arg1,0,AIDS)
//	    	CreateWordField(Arg1,2,TGCI)
//	    	CreateByteField(Arg1,4,CGPT)
    //
//	        Return(TMPD)
//	}
//    //
//    // Function 15: Get Graphics Device Types
//    //
//    // This function reports all graphics devices and XGP ports supported by a given platform
//    //
//	If(LEqual(Arg0,0xF))
//	{
//            Name(TMPF,Buffer(256) {0x0})
//            CreateWordField (TMPF, 0, FFND)
//            CreateWordField (TMPF, 2, FFDS)
//            CreateDwordField (TMPF, 4, FFFG) 
//            CreateWordField (TMPF, 8, FFBS)
//            CreateWordField (TMPF, 10, FFDV)
//	    Return(TMPF)
//	}



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
} // end PCI0.GFX0 scope


//**********************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************