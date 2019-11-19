//****************************************************************************
//****************************************************************************
//**                                                                        **
//**         (C)Copyright 1985-2016, American Megatrends, Inc.              **
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
// $Header: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPEG/OpSSDT.asl 2     2/21/13 5:37a Joshchou $
//
// $Revision: 2 $
//
// $Date: 2/21/13 5:37a $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPEG/OpSSDT.asl $
// 
// 
//**********************************************************************
DefinitionBlock (
        "NvOpt.aml",
        "SSDT",
        1,
        "OptRef",
        "OptTabl",
        0x1000
        ) {       

#define OPTIMUS_DSM_GUID 1
//#define NBCI_DSM_GUID 1

External(P8XH, MethodObj) 
#if defined(ASL_SGTPV_ASL_DEBUG) && (ASL_SGTPV_ASL_DEBUG ==1)
#define P8DB(arg0, arg1, arg2) P8XH (0, arg1) P8XH (1, arg0) sleep(arg2)
#else
#define P8DB(arg0, arg1, arg2)
#endif


External(ASL_PCI_SCOPE, DeviceObj)
External(ASL_PEG_SCOPE, DeviceObj)
External(ASL_DGPU_SCOPE, DeviceObj)
External(ASL_IGPU_SCOPE, DeviceObj)
External(ASL_DGPU_SCOPE._ADR, DeviceObj)
External(ASL_IGPU_SCOPE._DSM, MethodObj)
External(ASL_DGPU_SCOPE.PVID)
External(ASL_PCI_SCOPE.PGON, MethodObj)
External(ASL_PCI_SCOPE.PGOF, MethodObj)
External(ASL_PCI_SCOPE.SGPI, MethodObj)
External(ASL_PCI_SCOPE.SGPO, MethodObj)
External(\DSEL)
External(\ESEL)
External(\SSEL)
External(\PSEL)
External(\HRE0)
External(\HRG0)
External(\HRA0)
External(\PWE0)
External(\PWG0)
External(\PWA0)
External(\SGGP)
External(\SGMD)
External(\SGFL)
External(\SSMP)

#include <NVdGPU.asl>     // Include DGPU device namespace
#include <NViGPU.asl>     // Include NVHG DSM calls
//#include <NViGDmisc.ASL>  // Include misc event callback methods

#if ASL_NV_VENTURA_SUPPORT == 1
#include <NvVentura.asl>  // Include Ventura support
#endif

#if ASL_NV_GC6_SUPPORT == 1
#include <NvGC6.asl>  // Include GC6 support
#endif

#if ASL_NV_GC6_SUPPORT == 2
#include <NvGC6v2.asl>  // Include GC6 support
#endif

#if ASL_NV_GPS_SUPPORT == 1
#include <NvGPS.asl>  // Include GPS support
#endif

 Scope(ASL_PCI_SCOPE) 
 {
        Device (WMI2)
        {
            Name  (_HID,  "PNP0C14")
            Name (_UID, "OPTM")
            Name (_WDG,  Buffer (0x28)
            {
                //{ CA4982BF-C230-458E-B12F-6F16475F-351B} WMI GUID
                // 0x4F, 0x50      OBJECT ID (OP)
                // 0x01   Instance count
                // 0x02   Flags (WMIACPI_REFLAG_METHOD)
                /* 0000 */    0xBF, 0x82, 0x49, 0xCA, 0x30, 0xC2, 0x8E, 0x45,
                /* 0008 */    0XB1, 0x2F, 0x6F, 0x16, 0x47, 0x5F, 0x35, 0x1B,
                /* 0010 */    0x4F, 0x50, 0x01, 0x02,
                //{ A486D8F8-0BDA-471B-A72B-6042A6B5-BEE0} _DSM GUID
                // 0x53, 0x4D      OBJECT ID (SM)
                // 0x01   Instance count
                // 0x00   Flags
                /* 0000 */    0xF8, 0xD8, 0x86, 0xA4, 0xDA, 0x0B, 0x1B, 0x47,
                /* 0008 */    0XA7, 0x2B, 0x60, 0x42, 0xA6, 0xB5, 0xBE, 0xE0,
                /* 0010 */    0x53, 0x4D, 0x01, 0x00,
            })
            // WMOP ZPB specific method
            //  Arg0:  Instance being queried.
            //  Arg1:  Method Id
            //  Arg2:  Data
            Method (WMOP, 3, NotSerialized)
            {
                If (LEqual(Arg1, One))
                {
                    // Power on DGPU
                    \_SB.PCI0.PEG0.PEGP._PS0()                      // IBV_customize: root port path
                    // Send notification to OS thru. GPU parent bridge.
                    Notify (PEG0, 0)
                    Return(Zero)
                }
                If (LEqual (Arg1, 2) )
                {
                    // Returns GPU Power state from OPTIMUSCAPS state[4:3]
                    If (LEqual (\_SB.PCI0.PEG0.PEGP.DGPS, Zero))    // IBV_customize: root port path
                    {
                        Return(0x10)
                    } 
                    Else
                    {
                        Return(0x20)
                    }
                }
            }
            Method (WQSM,1) 
            {
                Return(ATSM)
            }
            Name (ATSM, Buffer ()
            {
              /* 0000 */    0x52, 0xAA, 0x89, 0xC5, 0x44, 0xCE, 0xC3, 0x3A, 
              /* 0008 */    0x4B, 0x56, 0xE2, 0x00, 0x00, 0x00, 0x01, 0x00, 
              /* 0010 */    0x32, 0x37, 0x34, 0x35, 0x39, 0x31, 0x32, 0x35, 
              /* 0018 */    0x33, 0x36, 0x38, 0x37, 0x47, 0x65, 0x6E, 0x75, 
              /* 0020 */    0x69, 0x6E, 0x65, 0x20, 0x4E, 0x56, 0x49, 0x44, 
              /* 0028 */    0x49, 0x41, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 
              /* 0030 */    0x66, 0x69, 0x65, 0x64, 0x20, 0x4F, 0x70, 0x74, 
              /* 0038 */    0x69, 0x6D, 0x75, 0x73, 0x20, 0x52, 0x65, 0x61, 
              /* 0040 */    0x64, 0x79, 0x20, 0x4D, 0x6F, 0x74, 0x68, 0x65, 
              /* 0048 */    0x72, 0x62, 0x6F, 0x61, 0x72, 0x64, 0x20, 0x66, 
              /* 0050 */    0x6F, 0x72, 0x20, 0x63, 0x6F, 0x6F, 0x6B, 0x69, 
              /* 0058 */    0x65, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x75, 0x6C, 
              /* 0060 */    0x35, 0x30, 0x20, 0x75, 0x73, 0x69, 0x6E, 0x20, 
              /* 0068 */    0x2D, 0x20, 0x5E, 0x57, 0x3C, 0x4A, 0x3D, 0x41, 
              /* 0070 */    0x24, 0x4C, 0x3A, 0x4B, 0x38, 0x32, 0x26, 0x51, 
              /* 0078 */    0x48, 0x35, 0x4C, 0x3E, 0x2B, 0x33, 0x52, 0x2B, 
              /* 0080 */    0x54, 0x35, 0x2A, 0x52, 0x29, 0x3A, 0x5B, 0x4C, 
              /* 0088 */    0x4A, 0x3E, 0x36, 0x48, 0x22, 0x48, 0x41, 0x50, 
              /* 0090 */    0x47, 0x39, 0x5A, 0x39, 0x5E, 0x3E, 0x44, 0x53, 
              /* 0098 */    0x54, 0x3C, 0x20, 0x2D, 0x20, 0x43, 0x6F, 0x70, 
              /* 00A0 */    0x79, 0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x32, 
              /* 00A8 */    0x30, 0x30, 0x39, 0x20, 0x4E, 0x56, 0x49, 0x44, 
              /* 00B0 */    0x49, 0x41, 0x20, 0x43, 0x6F, 0x72, 0x70, 0x6F, 
              /* 00B8 */    0x72, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x41, 
              /* 00C0 */    0x6C, 0x6C, 0x20, 0x52, 0x69, 0x67, 0x68, 0x74, 
              /* 00C8 */    0x73, 0x20, 0x52, 0x65, 0x73, 0x65, 0x72, 0x76, 
              /* 00D0 */    0x65, 0x64, 0x2D, 0x32, 0x37, 0x34, 0x35, 0x39, 
              /* 00D8 */    0x31, 0x32, 0x35, 0x33, 0x36, 0x38, 0x37, 0x28, 
              /* 00E0 */    0x52, 0x29       
            })
        } 
//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    WMI1
//
// Description:  WMI MXM Mapper. ASL Device is used to acccess Nv Optimus native method via WMI API
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END> 
    Device(WMI1) // placed within PCI Bus scope parallel to iGPU 
    { 
        Name(_HID, "PNP0C14")
        Name(_UID, "OPT1")    
   
        Name(_WDG, Buffer() 
        {
            // Methods GUID {F6CB5C3C-9CAE-4ebd-B577-931EA32A2CC0}
            0x3C, 0x5C, 0xCB, 0xF6, 0xAE, 0x9C, 0xbd, 0x4e, 0xB5, 0x77, 0x93, 0x1E,
            0xA3, 0x2A, 0x2C, 0xC0,
            0x4D, 0x58, // Object ID "MX" = method "WMMX"
            1,          // Instance Count
            0x02,       // Flags (WMIACPI_REGFLAG_METHOD)        
        }) // End of _WDG

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    WMMX
//
// Description:  WMI Method execution tunnel. MXM Native methods are called via WMMX index.
//
// Input:
//          Arg1:   Integer     GPU index. 0x10-iGPU, 0x100+PCIe Bus number for the GPU
//
//  Output:
//          Buffer      specific to the funcion being called
//-------------------------------------------------------------------------
//<AMI_PHDR_END> 
        Method(WMMX, 3)
        {

            //Arg1 = 0x10 indicates iGPU, 0x100+PCIe Bus number for the GPU
            // 
            CreateDwordField(Arg2, 0, FUNC)                 // Get the function name

            If (LEqual(FUNC, 0x534F525F))                   // "_ROM"
            {
                If (LGreaterEqual(SizeOf(Arg2), 8))
                {
                    CreateDwordField(Arg2, 4, ARGS)
                    CreateDwordField(Arg2, 8, XARG)
                    Return(ASL_DGPU_SCOPE._ROM(ARGS, XARG))
                }
            }            

            If (LEqual(FUNC, 0x4D53445F))                   // "_DSM"
            {
                If (LGreaterEqual(SizeOf(Arg2), 28))
                {
                    CreateField(Arg2, 0, 128, MUID)
                    CreateDwordField(Arg2, 16, REVI)
                    CreateDwordField(Arg2, 20, SFNC)
                    CreateField(Arg2, 0xe0, 0x20, XRG0)
                    
//                    If(LNotEqual(Arg1,0x10))          
//                    {
                    If (CondRefOf(ASL_IGPU_SCOPE._DSM)) // common with dGPU DSM functions
                    {
                        Return(ASL_IGPU_SCOPE._DSM(MUID, REVI, SFNC, XRG0))
                    }
//                    }
                }
            }            
            Return(0)
        } // End of WMMX
   } // End of WMI1 Device
 } // end scope PCI0
} // end SSDT   
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**         (C)Copyright 1985-2016, American Megatrends, Inc.              **
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
