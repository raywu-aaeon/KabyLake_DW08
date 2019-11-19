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

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    ATIF function 21
//
// Description:  Sample code for OEM to modify for their requirement to meet their H/W design, 
//               and replace the path in the TOKEN "GC6_ELINK_METHOD_PATH"      
//               
//<AMI_PHDR_END>
//
    // Function 21: Get External GPU Information
    //
    // This function reports external graphics port information
    //
    // It is a required function only if a platform supports Detachable Graphics and does not
    // use Thunderbolt.Function 20 must be used if THunderbolt is supported.
    //
    If(LEqual(Arg0, 21))
    {
        Name(TM21, Buffer(256) {0x0})
        CreateWordField(TM21, 0, F21N)
        CreateWordField(TM21, 2, F21S)
        CreateWordField(TM21, 4, EGP1)
        CreateWordField(TM21, 6, EGB1)

        // Number of reported external graphics devices.
        // Default assume it only has one external graphics.
        Store(0x0001, F21N)
        // Size of return structure=8
        Store(0x0008, F21S)
        
        //
        // Device Structure repeated "Num Devices" times.
        //        
        // Bit 0: External Graphics port.
        // Bits 31-1:Reserved (must be zero)
        Store(0x0001, EGP1)
        // Bus number
        Store(ASL_OEM_ATI_EXT_GPU_BUS, EGB1) //

        Return(TM21)
    }   
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
//**********************************************************************