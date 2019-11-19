//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
External(\_SB.PCI0.P0LD)
External(\_SB.PCI0.PEG0.LREN)
External(\_SB.PCI0.PEG0.PEGP.LTRE)
External(\_SB.PCI0.P0RM)
External(\_SB.PCI0.P0AP)
External(\_SB.PCI0.PEG0.CEDR)
External(\_SB.PCI0.P0LS)
External(\_SB.GGIV, MethodObj)
External(ASL_PEG_SCOPE.TGPC, IntObj)
External(ASL_PEG_SCOPE.DGCX, IntObj)
External(ASL_PEG_SCOPE.TDGC, IntObj)

#define JT_REVISION_ID        0x00000103               // Revision number
#define JT_FUNC_SUPPORT       0x00000000               // Function is supported?
#define JT_FUNC_CAPS          0x00000001               // Capabilities
#define JT_FUNC_POWERCONTROL  0x00000003               // dGPU Power Control
#define JT_FUNC_PLATPOLICY    0x00000004               // Platform Policy
#define JT_FUNC_DISPLAYSTATUS 0x00000005               // Query the Display Hot-Key
#define JT_FUNC_MDTK          0x00000006               // Display Hot-Key Toggle List

Scope (ASL_DGPU_SCOPE)
{
#if ASL_GC6_ELINK_METHOD_SUPPORT == 1
  #include <NVGC6Elink.asl>
#endif

  Method (GC6I, 0, Serialized)
  {
    Store("<<< GC6I >>>", Debug)
#if ASL_GC6_ELINK_METHOD_SUPPORT == 1
    G6EL(0)
#endif    
    Store (\_SB.PCI0.PEG0.LREN, \_SB.PCI0.PEG0.PEGP.LTRE)
    // Disable PCIE_Link    
    Store (One, \_SB.PCI0.P0LD)
    // Access P0RM/P0AP after Link Disable successfully
    Store (One, \_SB.PCI0.P0RM)
    Store (0x03, \_SB.PCI0.P0AP)
    // Sensing GC6_FB_EN = 1    
    While(LNotEqual(\_SB.GGIV (ASL_GPIO_GC6_FB_EN), One))
    {
      Sleep(One)
    }
#if ASL_GC6_ELINK_METHOD_SUPPORT == 1
    G6EL(1)
#endif    
  }
    
  Method (GC6O, 0, Serialized)
  {
    Store("<<< GC6O >>>", Debug)
#if ASL_GC6_ELINK_METHOD_SUPPORT == 1
    G6EL(2)
#endif     
    // Access P0RM/P0AP before Link enable
    Store (Zero, \_SB.PCI0.P0RM)
    Store (Zero, \_SB.PCI0.P0AP)
    // Enable PCIE_Link            
    Store(Zero, \_SB.PCI0.P0LD)         //  PCIE link enabling
    If(LEqual(\_SB.GGIV(ASL_GPIO_GC6_FB_EN), One))
    {
      // Asserts GPU_EVENT#
      ASL_PCI_SCOPE.SGPO(SGGP, ASL_EXPANDER_GPU_EVENT, ASL_GPIO_GPU_EVENT, ASL_ACTIVE_GPU_EVENT, One)
      // Sensing GC6_FB_EN = 0
      While(LNotEqual(\_SB.GGIV (ASL_GPIO_GC6_FB_EN), Zero))
      {
        Sleep(One)      // Wait for FB_EN to de-assert
      }
      // De-asserts GPU_EVENT#      
      ASL_PCI_SCOPE.SGPO(SGGP, ASL_EXPANDER_GPU_EVENT, ASL_GPIO_GPU_EVENT, ASL_ACTIVE_GPU_EVENT, Zero)
    }

    While (LLess (\_SB.PCI0.P0LS, 0x07))
    {
      Sleep(One)
    }
    Store (\_SB.PCI0.PEG0.PEGP.LTRE, \_SB.PCI0.PEG0.LREN)
    Store (One, \_SB.PCI0.PEG0.CEDR)
#if ASL_GC6_ELINK_METHOD_SUPPORT == 1
    G6EL(3)
#endif    
  } 

   Method(GETS, 0, Serialized)
   {
          If(LEqual(\_SB.GGIV(ASL_GPIO_GC6_FB_EN), Zero))
          {
                Store("<<< GETS() return 0x1 >>>", Debug)
                Return(One)
          }Else
          {
                Store("<<< GETS() return 0x3 >>>", Debug)
                Return(0x03)
          }
   }


//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    NGC6
//
// Description:  Called from _DSM -Device Specific Method for dGPU device.
//               Implement Ventura specific callback functions
//
// Input:
// Arg0:   UUID      Unique function identifier. 
//               Ventura DSM_GUID CBECA351-067B4924-9CBDB46B00B86F34
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
    Method (NGC6, 4, NotSerialized)
    {

        Store("------- GC6 DSM --------", Debug)
        // Only Interface Revision 0x0100 is supported
        If (LLess(Arg1, 0x100))
        {
                Return(0x80000001)
        }

        // (Arg2) Sub-Function
        Switch (ToInteger(Arg2))
        {
            //
            // Function 0:  
            //
            case (JT_FUNC_SUPPORT)
            {
				Return(Buffer(0x04)
				{
					0x1B, 0x00, 0x00, 0x00
				})
            }
            //
            // Function 1:  
            //
            case (JT_FUNC_CAPS)
            {

                Name(JTB1, Buffer(0x4)
                {
    				0x00
                })
				CreateField(JTB1,Zero,One,JTEN)
				CreateField(JTB1,One,0x02,SREN)
				CreateField(JTB1,0x03,0x03,PLPR)
				CreateField(JTB1,0x06,0x02,FBPR)
				CreateField(JTB1,0x08,0x02,GUPR)
				CreateField(JTB1,0x0A,One,GC6R)
				CreateField(JTB1,0x0B,One,PTRH)
				CreateField(JTB1,0x0D,One,MHYB)
				CreateField(JTB1,0x0E,One,RPCL)
                                CreateField(JTB1,0x0F,0x02,GC6M)
				CreateField(JTB1,0x14,0x0C,JTRV)
				Store(One, JTEN)     // JT enable
				Store(One, GC6R)     // GC6 integrated ROM
				Store(One, PTRH)     // No SMI Handler
                                Store(One, GC6M)
				Store(One, SREN)     // Disable NVSR
				If(\_OSI("Windows 2013"))
				{
					Store(One, MHYB)
				}
				Store(JT_REVISION_ID, JTRV)  // JT rev

                Return(JTB1)
            }
            //
            // Function 2:  
            //
	        case(0x00000002)
    		{
                Store("GPS fun 19", Debug)
                return(arg3)
    		}
            //
            // Function 3:  
            //
	        case(0x00000003)
    		{
				CreateField(Arg3,Zero,0x03,GUPC)
				CreateField(Arg3,0x04,One,PLPC)
				CreateField(Arg3,0x0E,0x02,DFGC)
				CreateField(Arg3,0x10,0x03,GPCX)
                                                                Store (Arg3, TGPC)
								//
								// This's the error handle for the following MSHybrid GC6 cases
								// 1. TDGC be cleared if driver call _GSS during GC6I --> GC6O. 
								//    The Driver _DSM input : GUPC = 0, DFGC = 0
								// 2. OS send the power-on IRP before the deferred GC6 entry be called. We added the WAR which is clearing the deferred immediately when driver request DFGC = 2 via the GC6 _DSM (original, we clear it when GC6O be executed).  
								//    The Driver _DSM input : GUPC = 0, DFGC = 2
								//
                                                                If (LOr (LNotEqual (ToInteger (GUPC), Zero), LNotEqual (ToInteger (DFGC), Zero)))
                                                                {
                                                                               Store (ToInteger (DFGC), ASL_PEG_SCOPE.TDGC)
                                                                               Store (ToInteger (GPCX), ASL_PEG_SCOPE.DGCX)
									
				}
				Name(JTB3, Buffer(0x04)
				{
					0x00
				})
				CreateField(JTB3,Zero,0x03,GUPS)
				CreateField(JTB3,0x03,One,GPGS)	    // dGPU Power status 
				CreateField(JTB3,0x07,One,PLST)
				If(LNotEqual(ToInteger(DFGC), Zero))
				{
					Store(One, GPGS)
					Store(One, GUPS)
					Return(JTB3)
				}
				If(LEqual(ToInteger(GUPC), One))           // EGNS 
				{
					GC6I()
					Store(One, PLST)
				}
				Else
				{
					If(LEqual(ToInteger(GUPC), 0x02))  // EGIS 
					{
                                                GC6I()
						If(LEqual(ToInteger(PLPC), Zero))
						{
							Store(Zero, PLST)
						}
					}
					Else
					{
						If(LEqual(ToInteger(GUPC), 0x03)) // XGXS 
						{
							GC6O()
							If(LNotEqual(ToInteger(PLPC), Zero))
							{
								Store(Zero, PLST)
							}
						}						
						Else
						{
							If(LEqual(ToInteger(GUPC), 0x04)) // XGIS
							{
								GC6O()
								If(LNotEqual(ToInteger(PLPC), Zero))
								{
									Store(Zero, PLST)
								}
							}
							Else
							{
								If(LEqual(ToInteger(GUPC), Zero)) 
								{
									Store(GETS(), GUPS)
									If(LEqual(ToInteger(GUPS), 0x01)) 
									{
                                  					   Store(One, GPGS)       // dGPU power status is Power OK
									}
									Else
									{
									    Store(Zero, GPGS)       // dGPU power status is Power off
									} 
								  }
								  Else 
								  {
								      If(LEqual(ToInteger(GUPC), 0x6))  
								      { 
								   
								  } 
								}
							}
						}
					}
				}
				Return(JTB3)
    		}
            //
            // Function 4:  
            //
	        case(JT_FUNC_PLATPOLICY)
    		{
                Return(0x80000002)
    		}

        } // end of switch

        Return(0x80000002)
    } // end NGC6
} // end ASL_DGPU_SCOPE scope
//**********************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
