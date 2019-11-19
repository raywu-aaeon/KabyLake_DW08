//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
External(\_PR.PR00._PSS, MethodObj)
External(\_PR.CPPC, IntObj)
External(ASL_NV_GPS_TEMP)    // CPU GT VR (IMVP) Temperature
External(PNOT, MethodObj)
External(\_PR.PR00._TSS, MethodObj)
External(\_PR.HWPV)

#define GPS_REVISION_ID        0x00000100               // Revision number
#define GPS_ERROR_SUCCESS      0x00000000               // Generic Success
#define GPS_ERROR_UNSPECIFIED  0x00000001               // Generic unspecified error code
#define GPS_ERROR_UNSUPPORTED  0x00000002               // Sub-Function not supported

#define GPS_FUNC_SUPPORT       0x00000000               // Bit list of supported functions
#define GPS_FUNC_GETCALLBACKS  0x00000013               // Get system requested callback
#define GPS_FUNC_PSHARESTATUS  0x00000020               // Get system requested Power Steering Setting
#define GPS_FUNC_GETPSS        0x00000021               // Get _PSS object
#define GPS_FUNC_SETPPC        0x00000022               // Set _PCC object
#define GPS_FUNC_GETPPC        0x00000023               // Get _PCC object
#define GPS_FUNC_PSHAREPARAMS  0x0000002A               // Get sensor information and capabilities

Scope (ASL_DGPU_SCOPE)
{

Name(PSAP, Zero)
      Name(GPSP, Buffer(36) {})
        CreateDWordField(GPSP, 0, RETN)
        CreateDWordField(GPSP, 4, VRV1)
        CreateDWordField(GPSP, 8, TGPU)
        CreateDWordField(GPSP, 12, PDTS)
        CreateDWordField(GPSP, 16, SFAN)
        CreateDWordField(GPSP, 20, SKNT)
        CreateDWordField(GPSP, 24, CPUE)
        CreateDWordField(GPSP, 28, TMP1)
        CreateDWordField(GPSP, 32, TMP2)

Name(NLIM, 0) // Set one flag for GPS_EVENT_STATUS_CHANGE
              // 1: will update parameter
              // 0: just call function 0x1c _PCONTROL

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    GPS
//
// Description:  Called from _DSM -Device Specific Method for dGPU device.
//               Implement Ventura specific callback functions
//
// Input:
// Arg0:   UUID      Unique function identifier. 
//               Ventura DSM_GUID A3132D01-8CDA-49BA-A52E-BC9D46DF6B81
// Arg1:   Integer   Revision Level
// Arg2:   Integer   Function Index (0 = Return Supported Functions)
// Arg3:   Package   Parameters
//
// Output:
//  Sub-function 0 and unsupported function calls always returns a buffer. 
//  Other subfunctions may return a buffer or a package as defined in the function.
//  When a single DWord is returned the following values have special meaning,
//  controlled by reserved Bit31 as follows:
//      MXM_ERROR_SUCCESS 0x00000000 Success
//      MXM_ERROR_UNSPECIFIED 0x80000001 Generic unspecified error code
//      MXM_ERROR_UNSUPPORTED 0x80000002 FunctionCode or SubFunctionCode not
//          supported by this system
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
    Method (GPS, 4, NotSerialized)
    {

        Store("<<< GPS >>>", Debug)
        // Only Interface Revision 0x0100 is supported
        If (LNotEqual(Arg1, 0x100))
        {
                Return(0x80000002)
        }

        P8DB(0xDD, Arg2, 1000)
        // (Arg2) Sub-Function
        Switch (ToInteger(Arg2))
        {

            case (GPS_FUNC_SUPPORT)
            {
              Store("GPS fun 0", Debug)
                return (Buffer (0x08)
                        {
                            0x01, 0x00, 0x08, 0x00, 0x0F, 0x04, 0x00, 0x00
                        })
            }
            //
            // Function 19: GPS_FUNC_GETCALLBACKS, 
            //
	        case(GPS_FUNC_GETCALLBACKS)
    		{
                Store("GPS fun 19", Debug)
                return(arg3)
    		}
            //
            // Function 32: GPS_FUNC_PSHARESTATUS, 
            //
	        case(GPS_FUNC_PSHARESTATUS)
    		{
                Store("GPS fun 32", Debug)

                Name(RET1, Zero)
                CreateBitField(Arg3,24,NRIT) //new request new IGP turbo state(bit 24 is valid)
                CreateBitField(Arg3,25,NRIS) //request new IGP turbo state
                   if (NRIS){
                      if(NRIT){
                       Or(RET1, 0x01000000, RET1)       
                      }else
                        {
                      //help disable IGP turbo boost
                      And(RET1, 0xFeFFFFFF, RET1)
                        }
                      }
                Or(RET1, 0x40000000, RET1)  // if this machine support GPS
       
                if(NLIM){
                   Or(RET1, 0x00000001, RET1)  // if NLIM falg is set, set bit0 =1
                }
                Store("== GPS: HWPV ==", Debug)
                Store(\_PR.HWPV, Debug)
                If(And(\_PR.HWPV, 0x2)) 
                {
	                Or(RET1, 0x800000, RET1)      // If yes, Return Bit 23 to 1
                }                       

                Return(RET1)
    		}
            //
            // Function 33: GPS_FUNC_GETPSS, Get CPU _PSS structure
            //
	        case(GPS_FUNC_GETPSS)
    		{
                Store("GPS fun 21", Debug)
                Return(\_PR.PR00._PSS)
    		}
            //
            // Function 34: GPS_FUNC_SETPPC, Set current CPU _PPC limit
            //
	        case(GPS_FUNC_SETPPC)
    		{
                CreateBYTEField(Arg3, 0, PCAP)
                Store(PCAP, \_PR.CPPC)
                PNOT()
                store(PCAP, PSAP)
                Return(PCAP)
    		}
            //
            // Function 35: GPS_FUNC_GETPPC, Get current CPU _PPC limit
            //    
	        case(GPS_FUNC_GETPPC)
    		{
                Store("GPS fun 23", Debug)
                Return(PSAP)
    		}

            //
            // Function 42: GPS_FUNC_PSHAREPARAMS, Get Power Steering platform parameters
            //
	        case(GPS_FUNC_PSHAREPARAMS)
    		{
                Store("GPS fun 42", Debug)

                CreateBYTEField(Arg3,0,PSH0)
                CreateBYTEField(Arg3,1,PSH1)
                CreateBitField(Arg3,8,GPUT)
                CreateBitField(Arg3,9,CPUT)
                CreateBitField(Arg3,10,FANS)
                CreateBitField(Arg3,11,SKIN)
                CreateBitField(Arg3,12,ENGR)
                CreateBitField(Arg3,13,SEN1)
                CreateBitField(Arg3,14,SEN2)
                Store(0x00010000, VRV1)
                   switch (PSH0){
                      case(0){
                        if(CPUT){
                        store(0x00000200, RETN)
                        Or(RETN, PSH0, RETN)
                     // Please return CPU or EC tempture to PDTS
                        store(ASL_NV_GPS_TEMP,PDTS)            
                        }
                	    return(GPSP)
                       } //case(0)

                      case(1){
                        store(0x00000300, RETN) //need to return CPU and GPU status bits for Querytype1
                        Or(RETN, PSH0, RETN)
                        store(1000,PDTS)
                        return(GPSP)
                      } //case(1)

                      case(2){
                        store(0x0102, RETN)    //RETN[0:3] need to be the same as input argument,  bit8 is GPU temp status bit
                        store(0x0000005C, TGPU)
                        store(0x00000000, PDTS)
                        store(0x00000000, SFAN)
                        store(0x00000000, CPUE)
                        store(0x00000000, SKNT)
                        store(0x00000000, TMP1)
                        store(0x00000000, TMP2)            
                        return(GPSP)          
                      } //case(2)
                   } // PSH0 of switch
                   Return (0x80000002)
    		}
        } // end of switch

        Return(0x80000002)
    } // end GPS


} // end DGPU_SCOPE scope
//**********************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
