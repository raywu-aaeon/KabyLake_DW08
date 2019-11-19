//Endpoint PCIe Base Address.
External(EBAS)
External(NVHA)

#ifdef OPTIMUS_DSM_GUID
Scope(ASL_PCI_SCOPE){

    Name(OTM, "OTMACPI 2010-Mar-09 12:08:26")       // OTMACPIP build time stamp.
} // end of Scope
#endif

Scope(ASL_DGPUPCH_SCOPE)
{       
    
//<AMI_SHDR_START>
//------------------------------------------------------------------------
//
// Procedure: PCI2   
//
// Description: For save/store PCIE NV PCI register from 0 to 256 byte by Dword access while do _ON & OFF
//
//-------------------------------------------------------------------------
//<AMI_SHDR_END>
//    OperationRegion (PCI2, PCI_Config, 0x00, 0x0100)
// PEG Endpoint PCIe Base Address.
    OperationRegion (PCI2, SystemMemory, EBAS, 0x01000)
    Field(PCI2, DWordAcc, Lock, Preserve)
    {
        Offset(0x4), 
        CMDR, 8,
        VGAR, 2000,
        Offset(0x48B),
        , 1,
        NHDA, 1,
    } 
    Name(VGAB, Buffer(0xFA)
    {
        0x00
    })
    Name(GPRF, Zero)
    
//<AMI_SHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    NVHM
//
// Description:  Nvidia NVHG (dGPU) OperationRegion
//               OpRegion address (NVHA)is defined in IDG Nvs OpRegion
//
//-------------------------------------------------------------------------
//<AMI_SHDR_END>    
    OperationRegion(NVHM,SystemMemory, NVHA, 0x30400)
    Field(NVHM, DWordAcc, NoLock, Preserve)
    {
        // OpRegion Header

        NVSG,   0x80,  // (000h) Signature-"NVSG".
        NVSZ,   0x20,  // (010h) OpRegion Size in KB.
        NVVR,   0x20,  // (014h) OpRegion Version.

        // NVHG data

        NVHO,   0x20,   // (018h)NVHM opregion address
        RVBS,   0x20,   // (01Ch)NVIDIA VBIOS image size
                        // (020h)for _ROM 
        RBF1,   0x40000, // 0x8000 bytes in bits
        RBF2,   0x40000, // 0x8000 bytes in bits
        RBF3,   0x40000, // 0x8000 bytes in bits
        RBF4,   0x40000, // 0x8000 bytes in bits
        RBF5,   0x40000, // 0x8000 bytes in bits
        RBF6,   0x40000, // 0x8000 bytes in bits
        MXML,   0x20,    // Mxm3 buffer length
#if MXM30_SUPPORT
        MXM3,   MXM_ROM_MAX_SIZE_bits // MXM 3.0 Data buffer
#else
        MXM3,   0x640 // MXM 3.0 Data buffer
#endif

    }

    Name(OPCE, 2)   // Optimus Power-Control ENABLE
                    // 2: The platform should not power down the GPU subsystem 
                    //    in the _PS3 method (Default)
                    // 3: The platform should power down the GPU subsystem
                    //    at the end of the _PS3 ACPI method

    Name(DGPS, Zero)// Power State. dummy control field. Can be a GPIO in EC or PCH

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    SGST
//
// Description:  
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>

    Method(SGST,0,Serialized)
    {
      //
      // Check SGMode and dGPU Present Detect GPIO for SG system
      //
      If(And(SGMD,0x0F))
      {
        If(LNotEqual(SGGP,0x01))
        {
           Return(0x0F)
        }

        // To detect dGPU: Check Device is present and which belongs to display controllers type also.
#if ASL_SG_ULT_PORT_DEV == 28
        If(LNotEqual(DVI1,0xFFFF))
#else
        If(LNotEqual(DVI9,0xFFFF))
#endif	
        {
#if ASL_SG_ULT_PORT_DEV == 28
            If(LEqual(CBC1,0x3)) // Base Class Code 03h which is referring all types of display controllers
#else
            If(LEqual(CBC9,0x3)) // Base Class Code 03h which is referring all types of display controllers
#endif	
            {
               Return(0x0F)
            }
        }
      
        Return(0x00)
      }

      //
      // For non-SG system check for valid Vendor Id
      //
#if ASL_SG_ULT_PORT_DEV == 28
      If(LNotEqual(DVI1,0xFFFF))
#else
      If(LNotEqual(DVI9,0xFFFF))
#endif	      
      {
          Return(0x0F)
      }
      Return(0x00)
    }

#ifdef OPTIMUS_DSM_GUID

//If dGPU power control is available....

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _PSC
//
// Description:  Curent dGPU power state, 0-D0, 3-D3, etc.
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
   Name(_PSC, Zero)

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _PS0
//
// Description:  dGPU power ON control method
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>

    Method(_PS0, 0, NotSerialized)
    {
    P8DB(0xB0, OPCE, 2000)
        Store(Zero, _PSC)
        If(LNotEqual(DGPS, Zero))
        {
            _ON()   // with Optimus w/a
            Store(Zero, DGPS)
        }
    }

    Method(_PS1, 0x0, NotSerialized)
    {
        Store(One, _PSC)
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _PS3
//
// Description:  dGPU power OFF control method
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>

    Method(_PS3, 0, NotSerialized)
    {
    P8DB(0xB3, OPCE, 2000)
        If(LEqual(OPCE, 0x3)) 
        {
            If(LEqual(DGPS, Zero))
            {
                _OFF()   // w Optimus w/a
                Store(One, DGPS)
            }
            Store(0x2, OPCE) // Reset NV GPU power down flag
        }
        Store(0x3, _PSC)
    }


//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _STA
//
// Description:  dGPU power status.
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
    Method(_STA,0x0)
    {
        Return(0x0F)                             // Always return DGPU is powered-ON
    }

#endif // OPTIMUS_DSM_GUID

#if HYBRID_DSM_GUID || MXM_DSM_GUID
// NON-OPTIMUS mode - MUXed

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _ON
//
// Description:  dGPU power ON control method
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
    Method(_ON,0,Serialized)
    {
#if ASL_SG_ULT_PORT_DEV == 28
        \_SB.PCI0.HGON(1)
#else
        \_SB.PCI0.HGON(9)
#endif
        //Ask OS to do a PnP rescan
        Notify(ASL_SG_ULT_RP_NUM,0)
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _OFF
//
// Description:  dGPU power OFF control method
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
    Method(_OFF,0,Serialized)
    {
#if ASL_SG_ULT_PORT_DEV == 28
        \_SB.PCI0.HGOF(1)
#else
        \_SB.PCI0.HGOF(9)
#endif

        //Ask OS to do a PnP rescan
        Notify(ASL_SG_ULT_RP_NUM,0)
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _STA
//
// Description:  Returns curent dGPU power/presence state
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
    Method(_STA,0,Serialized)
    {
        Return(SGST())  // OEM Mxm Power status 
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    MXMX
//
// Description:  Display DDC Mux Control
//
// Input:
// Arg0: Get/Set DDC/Aux Mux State
//  0- Acquire DDC/Aux Mux on this GPU
//  1- Release Mux from this GPU
//  2- Get Mux state
//
// Output:
//    0, Not Acquired.
//       if Arg0 = 0 or 1, Non-Zero return indicates success acquiring MUX 
//          (and MUX has switched to this output)
//       if Arg0 = 2, Non-Zero return indicates MUX is currently set to this output
//-------------------------------------------------------------------------
//<AMI_PHDR_END> 
    Method (MXMX, 1, Serialized) 
    {
        If (LEqual (Arg0, 0)) 
        {
            P8XH (One, 0x99)
            P8XH (Zero, Zero)
            Return(1)
        }
            
        If (LEqual (Arg0, 1)) 
        {
            P8XH (One, 0x99)
            P8XH (Zero, One)
            Return(0x1)
        }

        If (LEqual (Arg0, 2))
        {
            P8XH (One, 0x99)
            P8XH (Zero, 0x02)
        }

        Return(0x0) // mutex not acquired
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    MXDS
//
// Description:  Display output MUX control
//
// Input:
// Arg0:
// 0 - Get Mux state
// 1 - Set Display to active on this GPU
// 2 - Set Backlight control to active on this GPU
// 3 - Set Display & Backlight to active on this GPU 
//
// Output:
//  If Arg0 = 0, Error = Display is not MUXed
//  Else return value of the state of the MUX
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
    Method(MXDS, 1, Serialized)
    {
        If (LEqual (Arg0, Zero)) {}
        If (LEqual (Arg0, One)) {}
        Return (Zero)
    }
#endif // MXM && HYBRID

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _ROM
//
// Description:  Video ROM data buffer
//
// Input:
//          Arg0:   Integer     Offset of the graphics device ROM data
//          Arg1:   Integer     Size of the buffer to fill in (up to 4K)
//
//  Output:
//          Buffer      Buffer of requested video ROM bytes
//-------------------------------------------------------------------------
//<AMI_PHDR_END> 
    Method(_ROM,2)
    {

        Store (Arg0, Local0)
        Store (Arg1, Local1)

    P8DB(0x44, Local1, 100)

  
        If (LGreater (Local1, 0x1000))
        {
                Store (0x1000, Local1)
        }
        If (LGreater (Local0, 0x30000))
        {
                Return(Buffer(Local1){0})
        }

        
        Multiply (Local1, 0x08, Local3)
        Name (ROM1, Buffer (0x8000) {0})
        Name (ROM2, Buffer (Local1) {0})

        If(LLess(Local0, 0x8000)){
            Store(RBF1, ROM1)
        }
        ElseIf(LLess(Local0, 0x10000)){
            Subtract(Local0,0x8000,Local0)
            Store(RBF2, ROM1)
        }
        ElseIf(LLess(Local0, 0x18000)){
            Subtract(Local0,0x10000,Local0)
            Store(RBF3, ROM1)
        }
        ElseIf(LLess(Local0, 0x20000)){
            Subtract(Local0,0x18000,Local0)
            Store(RBF4, ROM1)
        }
        ElseIf(LLess(Local0, 0x28000)){
            Subtract(Local0,0x20000,Local0)
            Store(RBF5, ROM1)
        }
        ElseIf(LLess(Local0, 0x30000)){
            Subtract(Local0,0x28000,Local0)
            Store(RBF6, ROM1)
        }        
   
        Multiply (Local0, 0x08, Local2)
        CreateField (ROM1, Local2, Local3, TMPB)
        Store (TMPB, ROM2)
        Return (ROM2)
    }

//
// Check for Nvidia _DSM  UUIDs
//
//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    HDSM
//
// Description:  Device Specific Methods for Hybrid dGPU (may be invoked from iGD as well)
//               Implements Mxm and SG specific callback functions
//
// Input:
// Arg0:   UUID      Unique function identifier. 
//                   nVidia
//                         MXM dGPU GUID :9D95A0A0-0060-4D48-B34D-7E5FEA129FD4
//
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
    Method(HDSM, 4, SERIALIZED)
    {      

#ifdef MXM_DSM_GUID

        If(LEqual(Arg0, ToUUID("4004A400-917D-4cf2-B89C-79B62FD55665")))
        {

        P8DB(0xEE, Arg2, 1000)

            Switch(ToInteger(Arg2))
            {
                //
                // Function 0: MXM_FUNC_MXSS
                //
                case (0)
                {
                    //Sub-Functions 0,16,24 are supported
                    Return(ToBuffer(0x01010001))
                }
        
                //
                // Function 24: MXM_FUNC_MXMI
                //
                case (24)
                {
                    Return(ToBuffer(0x300)) // MXM 1.101 defines revision as 0x300
        //                    Return(ToBuffer(0x30)) // MXM 1.101 defines revision as 0x300
                }
        
                //
                // Function 16: MXM_FUNC_MXMS
                //
                case (16)
                {
                    If(LEqual (Arg1, 0x300)) // MXM 1.101 defines revision as 0x300
                    {   
#if MXM30_SUPPORT
                        // calculate true length of MXM block
                        CreateWordField(MXM3, 6, MXLN)
                        Add(MXLN, 8, Local0)            // Add length of MXM header
                        CreateField(MXM3, 0, Local0, MXM)
                        Return(ToBuffer(MXM))
#else
                        // ElkCreek 4 Mxm data structure
                        Name(MXM3, Buffer()
                        {
                            0x4d, 0x58, 0x4d, 0x5f, 0x03, 0x00, 0x5d, 0x00,
                            0x30, 0x11, 0xb8, 0xff, 0xf9, 0x3e, 0x00, 0x00,
                            0x00, 0x00, 0x0a, 0xf0, 0xf9, 0x3e, 0x00, 0x00,
                            0x60, 0xe9, 0xd0, 0xfe, 0xf9, 0x3e, 0x00, 0x00,
                            0x60, 0x6a, 0xda, 0xfe, 0xf9, 0x3e, 0x00, 0x00, 
                            0x20, 0x2b, 0xe2, 0xfe, 0xf9, 0x3e, 0x00, 0x00,
                            0x60, 0x6c, 0xea, 0xfe, 0xf9, 0x3e, 0x00, 0x00,
                            0x20, 0x6a, 0xda, 0xfe, 0xf9, 0x3e, 0x00, 0x00,
                            0x20, 0x6c, 0xea, 0xfe, 0xf9, 0x3e, 0x00, 0x00,
                            0x01, 0x90, 0x01, 0x00, 0x03, 0x00, 0x90, 0x01,
                            0x13, 0x00, 0x90, 0x01, 0xe5, 0x0d, 0x01, 0x01,
                            0x01, 0x00, 0x00, 0x00, 0xe5, 0x0d, 0x01, 0x03,
                            0x00, 0x90, 0xd8, 0x09, 0x11, 0x0a
                        })
                        Return(MXM3)
#endif
                    }
                }       
            } // switch 
            Return(0x80000002)          //MXM_ERROR_UNSUPPORTED - FunctionCode or SubfunctionCode not supported
        } // "4004A400-917D-4cf2-B89C-79B62FD55665"

#endif //  MXM_DSM_GUID

        Return (0x80000001)             //MXM_ERROR_UNSPECIFIED
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _DSM Device Specific Method for dGPU device 
//
// Description:  Implement Mxm and SG specific callback functions
//
// Input:
// Arg0:   UUID      Unique function identifier. 
//                         MXM dGPU GUID :9D95A0A0-0060-4D48-B34D-7E5FEA129FD4
//                          NBCI    GUID :D4A50B75-65C7-46F7-BfB7-41514CEA0244
//                          Ventura GUID :95DB88FD-940A-4253-A446-70CE0504AEDF
//                      Optimus DSM_GUID :A486D8F8-0BDA-471B-A72B-6042A6B5BEE0
//
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
    Method(_DSM,4,SERIALIZED)
    {
        CreateByteField (Arg0, 3, GUID)
        P8DB(0xDD, GUID, 1000)
        //
        // Check for Nvidia _DSM  UUIDs
        //
        // common _DSM for dGPU and iGPU: NBCI, SG DSM, Ventura
        return(ASL_IGPU_SCOPE.HDSM(Arg0, Arg1, Arg2, Arg3))     
    }


#ifdef OPTIMUS_DSM_GUID


// TEST !!! TEST !!! TEST !!!!
// NvOptimus should not be be using _ON and _OFF methods for power cycling 
// Used here for testing with Intel ElkCreek Mxm interposer
// 
    Name(CTXT, Zero)// Save Context flag

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _ON
//
// Description:  Optimus w/a for before dGPU _ON
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END> 

    Method(_ON, 0, Serialized)
    {
    P8DB(0x01, 0x11, 2000)

        // OEM Mxm Power status 
#if ASL_SG_ULT_PORT_DEV == 28
        \_SB.PCI0.HGON(1)
#else
        \_SB.PCI0.HGON(9)
#endif

// Nvidia Optimus driver w/a. Restore saved PCI context of PEG Video card
// Skip restore resource if GPRF = 1 
    If(LNotEqual(GPRF, One))
    {
            Store(CMDR,local0)
            Store(Zero, CMDR)
            Store(VGAB, VGAR)
            Store(0x06, CMDR)
//            Store(0x0,NHDA)  //NV HDMI audio did not need enable
// Clear HDA enable bit if flag not set
            if(LEqual(And(SGFL, 2), 0)) 
            {
               Store(0, NHDA)
            }
            Store(local0,CMDR)

      }

//        Store(1, RETR)   // retrain PCI-E bus
//+<
// doesn't look like we need delay here...
//        Sleep(0x64)

    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _OFF
//
// Description:  Optimus w/a before dGPU _OFF
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END> 

    Method(_OFF, 0, Serialized)
    {

    P8DB(0x0F, 0xFF, 2000)

// store PCI context only once
    If(LEqual(CTXT, Zero))
    {
// Nvidia Optimus driver w/a. Save PCI context of PEG Video card
    //+ for GC6 , need to de-assert EC FB_CLAMP
               
        //- 
        // Skip restore resource if GPRF = 1
    If(LNotEqual(GPRF, One))
    {
            Store(VGAR, VGAB)

           }
//+<
        Store(1, CTXT)
    }
#if ASL_SG_ULT_PORT_DEV == 28
        \_SB.PCI0.HGOF(1)
#else
        \_SB.PCI0.HGOF(9)
#endif
    }
#endif
} // end Scope(ASL_DGPUPCH_SCOPE)
