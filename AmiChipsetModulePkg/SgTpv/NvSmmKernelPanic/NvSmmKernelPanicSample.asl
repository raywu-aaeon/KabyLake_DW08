External(XBAS)
External(NVAF)
External(\_SB.SHPO, MethodObj)
External(\_SB.GPC0, MethodObj)
External(\_SB.GGIV, MethodObj)
External(\_SB.SGOV, MethodObj)
External(\_SB.SPC0, MethodObj)	
External(\_SB.CAGS, MethodObj)
External(\_SB.PCI0.P0RM, IntObj)
External(\_SB.PCI0.P0AP, IntObj)	
External(\_SB.PCI0.P0LS, IntObj)
External(\_SB.PCI0.P0LD, IntObj)
External(ASL_PEG_SCOPE.TGPC, IntObj)
External(ASL_PEG_SCOPE.DGCX, IntObj)
External(ASL_PEG_SCOPE.TDGC, IntObj)
External(\_SB.PCI0.PEG0.PRBN, IntObj)
External(\_SB.PCI0.PEG0.LREN, IntObj)
External(\_SB.PCI0.PEG0.CEDR, IntObj)
External(\_SB.PCI0.PEG0.PEGP.LTRE, IntObj)

#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
    Scope(_GPE)
    {
        Method(ASL_NV_L_EVENT_FBEN_PIN, 0x0, NotSerialized)        // Need to adjust the Token "NV_L_EVENT_FBEN_PIN" for FBEN pin
        {
            If(LEqual(And(\_SB.GPC0(ASL_GPIO_GC6_FB_EN), 0x00800000), 0x00800000)) //if RX Invert bit(RXINV) = 1
            {
                \_SB.PCI0.PEG0.PEGP.GC6O()
            }
        }
    }
#endif
    
    Scope (\_SB.PCI0.PEG0.PEGP)
    {
#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
        Name (GEIF, Zero)               // GEI support flag
        Name (GSWF, Zero)               // GSW support flag
#endif
        
        Method (_INI, 0, NotSerialized)
        {
            Store (Zero, NHDA)          // Always disable HDA device after boot

            // IBV_customize/OEM_customize: need to do below GPIO initialization during POST time +++
            // Initialize setting for SCI of FB_EN - Disable, LOW_Active and clear GPE_STS
            // Host Software Pad Ownership
            \_SB.SHPO(ASL_GPIO_GC6_FB_EN, One)        // HostSW_Own: 1 = GPIO Driver Mode
            \_SB.SHPO(ASL_GPIO_GPU_EVENT, One)        // HostSW_Own: 1 = GPIO Driver Mode

            // Pad Configuration DW0
            // bit[11:10] = GPIO control the Pad, bit[9] = GPIO RX Disable, bit[8] = GPIO TX Disable, bit[0] = GPIO TX State
            \_SB.SPC0(ASL_GPIO_GC6_FB_EN, Or(And(\_SB.GPC0(ASL_GPIO_GC6_FB_EN), 0xFFFFF0FF), 0x100))
            \_SB.SPC0(ASL_GPIO_GPU_EVENT, Or(And(\_SB.GPC0(ASL_GPIO_GPU_EVENT), 0xFFFFF0FE), 0x201))    // GPU_EVENT default high

#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
            // Enable GSW and GEI
            Store (One, GEIF)           // Enable GEI
            Store (One, GSWF)           // Enable GSW

            \_SB.CAGS(ASL_GPIO_GC6_FB_EN)             //Clear GPI General Purpose Events Status
#endif
            // IBV_customize/OEM_customize: need to do above GPIO initialization during POST time +++
        }

        // NVIDIA NVJT related start
        Method (NVJT, 4, SERIALIZED)
        {
            Store("------- NV JT DSM --------", Debug)
            If (LLess(Arg1, 0x100))
            {
                Return(0x80000001)
            }
            Switch (ToInteger (Arg2))
            {
                case (Zero)
                {
                    //
                    // JT support function
                    // Sub-Functions 0,1,3,4,5 are supported
                    //
                    Store("JT fun0 JT_FUNC_SUPPORT", Debug)
                    Return (Buffer(4) {0x7B, 0x00, 0x00, 0x00})
                }
                case (One)
                {
                    Store("JT fun1 JT_FUNC_CAPS", Debug)
                    Name (JTCA, Buffer(4) {0x00})
                    CreateField (JTCA, 0,  1,  JTEN) // JT Enable
                    CreateField (JTCA, 1,  2,  SREN) // NVSR Enable
                    CreateField (JTCA, 3,  2,  PLPR) // Panel Power Rail
                    CreateField (JTCA, 5,  1,  SRPR) // Self-Refresh Controller Power Rail
                    CreateField (JTCA, 6,  2,  FBPR) // FB Power Rail
                    CreateField (JTCA, 8,  2,  GUPR) // GPU Power Rail
                    CreateField (JTCA, 10, 1,  GC6R) // GC6 ROM
                    CreateField (JTCA, 11, 1,  PTRH) // Panic Trap Handler
                    CreateField (JTCA, 13, 1,  MHYB) // MS Hybrid supported
                    CreateField (JTCA, 14, 1,  RPCL) // Root Port control
                    CreateField (JTCA, 15, 2,  GC6V) // GC6 Version
                    CreateField (JTCA, 17, 1,  GEIS) // GC6 Exit ISR Support flag
                    CreateField (JTCA, 18, 1,  GSWS) // GC6 Self Wakeup Support
                    CreateField (JTCA, 20, 12, JTRV) // JT Revision

                    Store (0x1,  JTEN)
                    Store (0x0,  GC6R)      // IBV_customize/OEM_customize: 0: External SPI ROM; 1: Integrated ROM
                    Store (0x1,  RPCL)      // Supports fine grain Root Port Control PLON, PRPC

                    Store (0x0,  FBPR)
                    Store (0x0,  GUPR)
                    Store (0x0,  PTRH)
                    Store (0x1,  MHYB)      // IBV_customize/OEM_customize: 0: discrete NVSR support, 1: MShybrid
                                            // Supports deferred GC6Enter/Exit in _PSx/_PRx, as per DFGC bits
                    
                    // CONDITION 1 : Following setting is for NVSR
                    Store (0x0,  SREN)      // 0 = NVSR enabled.
                    Store (0x2,  PLPR)      // PanelPowerRail = 2 , for NVSR support
                    Store (0x1,  SRPR)      // SelfRefreshControllerPowerRail = 1, for NVSR support
                    
                    // CONDITION 2 : Following settings is for non NVSR
                    //Store (0x1,  SREN)    // 1 = NVSR disabled.
                    //Store (0x2,  PLPR)    // PanelPowerRail = 2 , for NVSR support
                    //Store (0x0,  SRPR)    // SelfRefreshControllerPowerRail = 0

#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
                    // for Selfwakeup support
                    Store (0x01, GC6V) // Support GC6-A
                    Store (GEIF, GEIS) // Enable GEI Support
                    Store (GSWF, GSWS) // Enable GSW Support
#endif
                    Store (0x103, JTRV)
                    
                    Return (JTCA)
                }
                case (0x02)
                {
                    Store("JT fun2 JT_FUNC_POLICYSELECT", Debug)
                    Return (0x80000002)
                }
                case (0x03)
                {
                    Store("JT fun3 JT_FUNC_POWERCONTROL", Debug)
                    CreateField (Arg3, 0x00, 0x03, GUPC)    // GPU Power Control
                    CreateField (Arg3, 0x04, 0x01, PLPC)    // Panel Power Control
                    CreateField (Arg3, 0x07, 0x01, ECOC)    // Notify on complete
                    CreateField (Arg3, 0x0E, 0x02, DFGC)    // Defer GC6 Enter/Exit until D3-Cold
                    CreateField (Arg3, 0x10, 0x03, GPCX)
                    Store(Arg3, TGPC)

                    If (LOr (LNotEqual (ToInteger (GUPC), Zero), LNotEqual (ToInteger (DFGC), Zero)))
                    {
                        Store(ToInteger(DFGC), TDGC)    // Defer GPC & GPCX to be processed when setting Device Power State (D0/D3-Hot/Cold)
                        Store(ToInteger(GPCX), DGCX)
                    }   

                    Name (JTPC, Buffer(4) {0x00})
                    CreateField (JTPC, 0, 3, GUPS)      // dGPU Power Status
                    CreateField (JTPC, 3, 1, GPWO)      // dGPU Power OK Status
                    CreateField (JTPC, 7, 1, PLST)      // Panel State

                    If (LNotEqual (ToInteger(DFGC), 0x0))
                    {
                        // dGPU On
                        Store (One, GPWO)
                        Store (One, GUPS) 
                        Return (JTPC)
                    }

                    If (LEqual (ToInteger(GUPC), One))
                    {
                        //
                        // Enter GC6 and no Panel Self-Refresh
                        // Panel is powered down with GPU
                        //
                        GC6I () // dGPU enter GC6 entry
                        // Panel Status is powered off
                        Store (One, PLST)
                        // dGPU Status is Completely Off
                        Store (Zero, GUPS) 
                    }
                    ElseIf (LEqual (ToInteger(GUPC), 0x02))
                    {
                        //
                        // Enter GC6 and stay in Panel Self-Refresh
                        // Panel and FB remain powered while GPU is shutdown
                        //
                        GC6I () // dGPU through EC enter GC6
                        // Panel remains powered while GPU is in GC6
                        If (LEqual (ToInteger(PLPC), Zero)) 
                        {
                            // Panel Status is powered on
                            Store (Zero, PLST)
                        }
                        // GC6 (dGPU off w/FB_CLAMP assert)
                        Store (Zero, GUPS) 
                    }
                    ElseIf (LEqual (ToInteger(GUPC), 0x03))
                    {
                        //
                        // Exit GC6 and exit Self-Refresh
                        // GPU is powered on, modeset and link train, GPU drives FB to the screen
                        //
                        GC6O () // dGPU through EC exit GC6

                        If (LNotEqual (ToInteger(PLPC), 0x00)) // Panel remains powered while GPU is in GC6
                        {
                            // Panel is powered on
                            Store (Zero, PLST) 
                        }
                        // dGPU On
                        Store (One, GPWO)
                        Store (One, GUPS)
                    }
                    ElseIf (LEqual (ToInteger(GUPC), 0x04))
                    {
                        //
                        // Exit GC6 but remain in Self-Refresh
                        // GPU is powered on, modeset and link train, SRC continues to drive the screen
                        //
                        //
                        // According section 1.5, SBIOS turn-on the dGPU and let dGPU to control SRC.
                        // 
                        GC6O () // dGPU through EC exit GC6

                        If (LNotEqual (ToInteger(PLPC), Zero)) // Panel remains powered while GPU is in GC6
                        {
                            // Panel is powered on
                            Store (0, PLST)
                        }
                        // dGPU On
                        Store (One, GPWO)
                        Store (One, GUPS)
                    }
                    Else
                    {
                        //
                        // Get the current GPU GCx Sleep Status
                        //
                        Store("<<< GETS >>>", Debug)
                        If(LEqual(\_SB.GGIV (ASL_GPIO_GC6_FB_EN), Zero))
                        {
                            Store("<<< GETS() return 0x1 >>>", Debug)
                            Store (One, GPWO)
                            Store (One, GUPS)
                        }
                        Else
                        {
                            Store("<<< GETS() return 0x3 >>>", Debug)
                            Store (Zero, GPWO)
                            Store (0x3, GUPS)
                        }
                    }
                    Return (JTPC)
                }
                case (0x04)
                {
                    Store("JT fun4 JT_FUNC_PLATPOLICY", Debug)
                    CreateField (Arg3, 2, 1, PAUD)      // AUD : Azalia Audio Device
                    CreateField (Arg3, 3, 1, PADM)      // ADM : Audio Disable Mask
                    
                    Store(Zero, Local0)
                    If (LEqual (ToInteger(PADM), One))
                    {
                        If (LEqual (ToInteger(PAUD), Zero))
                        {
                            Store(Zero, NVAF)       //IBV_customize: save a GNVS for S3/S4 resume HDA disable
                        }
                        Else
                        {
                            Store(One, NVAF)        //IBV_customize: save a GNVS for S3/S4 resume HDA enable
                        }
                    }
                    //IBV_customize: 
                    //SBIOS have to set the dGPU HDA control register (func:0,reg:0x488,bit:25) by this CMOS flag before 
                    //OS resume from S3/S4 (_WAK is acceptable). The HDA_CMOS_FLAG is also be used in PG00._ON
                    
                    ShiftLeft(\_SB.PCI0.PEG0.PEGP.NHDA, 0x02, Local0)   // Returns the currnet status of the audio device
                    Return (Local0)
                }
                //
                // the sub-function5 should be the same as the Optimus sub-function5.  
                //
                case(0x05)
                {
                    Store("JT fun5 JT_FUNC_DISPLAYSTATUS", Debug)
                    // CreateField(Arg3,31,1,NCSM) // Check the next combination sequence mask bit 31.
                    Store(0, Local0)
                    // Force LID status return open
                    Or(Local0, 0x00100010, Local0) // bit4=1 : LID info inlcuded, bit20=1 LID opened
                    Return(Local0)
                }
                case(0x06)
                {
                    Return ( Package() {
                                        0x00000110, 0x2c,
                                        0x80007340, 0x2c,
                                        0x80006350, 0x2c,
                                        0x80007350, 0x2c,
                                        0x80006320, 0x2c,
                                        0x80007320, 0x2c,
                                        0x80000100, 0x2c,
                                        0x00000110, 0x80007340, 0x2c,
                                        0x00000110, 0x80007340, 0x2b,
                                        0x00000110, 0x80006350, 0x2c,
                                        0x00000110, 0x80006350, 0x2b,
                                        0x00000110, 0x80006320, 0x2c,
                                        0x00000110, 0x80006320, 0x2b
                                        // roll-over to beginning
                                        })
                }                                
            } // end of switch
            Return(0x80000002)
        } // end of NVJT

        Mutex(LG6O,0) // expand LG6O Mutex to both GC6I and GC6O
        
        Method (GC6I, 0, Serialized)
        {
            Store("<<< GC6I >>>", Debug)
            
            Acquire(LG6O,0xFFFF)
            Store (\_SB.PCI0.PEG0.LREN, \_SB.PCI0.PEG0.PEGP.LTRE)
            Store (One, \_SB.PCI0.P0LD)
#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT != 1)            
            While (LNotEqual (\_SB.PCI0.P0LS, Zero))
            {
                Sleep (One)
            }
#endif            
            // access P0RM/P0AP after Link Disable successfully
            Store (One, \_SB.PCI0.P0RM)
            Store (0x03,  \_SB.PCI0.P0AP)

            While(LNotEqual(\_SB.GGIV (ASL_GPIO_GC6_FB_EN), One))
            {
#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
                Stall (0x32)    // Use Stall() to reduce GC6 latency for NVSR case
#else
                Sleep(One)      // Wait for FB_EN to assert
#endif
            }

#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
            If(LEqual(GSWF, One))
            {
                //bit[26:25] = RX Level/Edge Configuration, bit[23] = RX Invert, bit[19] = GPIO Input Route SCI
                //For Falling Edge triggering
                //bit[26:25] = 01b : Edge mode
                //bit[23] = 8 RX-Invert mode
                // FB_EN is HIGH now. Enable GPE on FB_EN falling edge
                \_SB.SPC0(ASL_GPIO_GC6_FB_EN, Or(And(\_SB.GPC0(ASL_GPIO_GC6_FB_EN), 0xF977FFFF), 0x02880000))       // Drive '0', No inversion, Routing can cause SCI
                \_SB.SHPO(ASL_GPIO_GC6_FB_EN, Zero)
                \_SB.CAGS(ASL_GPIO_GC6_FB_EN)                                                         // Clear GPI General Purpose Events Status
                Stall (0x64)
            }
            Store(ASL_SWSMI_JT_IO_TRAP_EN, SSMP)
#endif
            Release(LG6O)
        }

        Method (GC6O, 0, Serialized)
        {
            Store("<<< GC6O >>>", Debug)
#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
            Acquire(LG6O,0xFFFF)
            Store(ASL_SWSMI_JT_IO_TRAP_DIS, SSMP)
            CreateField(TGPC, 0x13, One, EGEI)
            If(LEqual(GSWF, One))
            {
                If(LEqual(And(\_SB.GPC0(ASL_GPIO_GC6_FB_EN), 0x00080000), 0x00080000))                // check bit[19] = GPIO Input Route SCI
                {
                    \_SB.SHPO(ASL_GPIO_GC6_FB_EN, One)                                                              // switch to GPIO_Driver mode                
                    \_SB.SPC0(ASL_GPIO_GC6_FB_EN, Or(And(\_SB.GPC0(ASL_GPIO_GC6_FB_EN), 0xFFF7FFFF), 0x00000000))   // Disable FB_EN SCI
                    If(LEqual(\_SB.GGIV(ASL_GPIO_GC6_FB_EN), Zero))
                    {
                        // FB_EN is LOW already! Change polarity and clear GPE_STS
                        \_SB.SPC0(ASL_GPIO_GC6_FB_EN, Or(And(\_SB.GPC0(ASL_GPIO_GC6_FB_EN), 0xFF7FFFFF), 0x00000000))  // Change to HIGH ACTIVE
                        \_SB.CAGS(ASL_GPIO_GC6_FB_EN)                                                                  // Clear GPI General Purpose Events Status
                    }
                }
                Else
                {
                    // This is not the first GC6O. return directly
                    Store(" -- GC6O -- Not First GC6O after GC6I. Skip",Debug)
                    // Since it's not first GC6O. Clear SCI as well to prevent continuous SCI
                    \_SB.SPC0(ASL_GPIO_GC6_FB_EN, Or(And(\_SB.GPC0(ASL_GPIO_GC6_FB_EN), 0xFF7FFFFF), 0x00000000))   // No Invert FB_EN polarity
                    \_SB.CAGS(ASL_GPIO_GC6_FB_EN)                                                     // Clear GPI General Purpose Events Status
                    Release(LG6O)
                    Return(One)
                }
            }
#endif

            // access P0RM/P0AP after Link Disable successfully
            Store (Zero, \_SB.PCI0.P0RM)
            Store (Zero, \_SB.PCI0.P0AP)
            
            Store(Zero, \_SB.PCI0.P0LD)         //  PCIE link enabling
            If(LEqual(\_SB.GGIV(ASL_GPIO_GC6_FB_EN), One))
            {
                \_SB.SGOV (ASL_GPIO_GPU_EVENT, Zero) // assert GPU_EVENT#
                While(LNotEqual(\_SB.GGIV (ASL_GPIO_GC6_FB_EN), Zero))
                {
#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
                    Stall (0x32)    // Use Stall() to reduce GC6 latency for NVSR case
#else
                    Sleep(One)      // Wait for FB_EN to de-assert
#endif
                }
                \_SB.SGOV (ASL_GPIO_GPU_EVENT, One) // de-assert GPU_EVENT#
            }

#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
            // FB_EN is HIGH already! Change polarity and clear GPE_STS
            \_SB.SPC0(ASL_GPIO_GC6_FB_EN, Or(And(\_SB.GPC0(ASL_GPIO_GC6_FB_EN), 0xFF7FFFFF), 0x00800000))           // Invert FB_EN polarity
            \_SB.CAGS(ASL_GPIO_GC6_FB_EN)                                                             // Clear GPI General Purpose Events Status
            
            // for self wakeup, write PCI register 0x00 to trigger INTR to RM
            If (LEqual(ToInteger(EGEI),One))
            {
                OperationRegion(PTMP, SystemMemory, Add(XBAS, 0x00100000), 0x04)
                Field(PTMP, DWordAcc, NoLock, Preserve)
                {
                    TEMP,8
                }
                Name( PTM1, Zero)
                Store(TEMP, PTM1)
                Store(PTM1, TEMP)
            }
#endif
            
#if defined(ASL_NVSR_SUPPORT) && (ASL_NVSR_SUPPORT == 1)
            While (LLess (\_SB.PCI0.P0LS, 0x03))
            {
                Stall (0x32)    // Use Stall() to reduce GC6 latency for NVSR case
            }
#else            
            While (LLess (\_SB.PCI0.P0LS, 0x07))
            {
                Sleep(One)
            }
#endif                        
            Store (\_SB.PCI0.PEG0.PEGP.LTRE, \_SB.PCI0.PEG0.LREN)
            Store (One, \_SB.PCI0.PEG0.CEDR)
            Release(LG6O)            
        }
        // NVIDIA JT related end
    }