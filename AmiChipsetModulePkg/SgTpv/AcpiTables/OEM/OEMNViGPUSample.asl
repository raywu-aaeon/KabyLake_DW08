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
//**********************************************************************
// $Header: 	$
//
// $Revision: 	$
//
// $Date: 		$
//**********************************************************************
// Revision History
// ----------------
// $Log:		$
// 
// 
//**********************************************************************
External(NVGA)

External(NXD1)
External(NXD2)
External(NXD3)
External(NXD4)
External(NXD5)
External(NXD6)
External(NXD7)
External(NXD8)
External(DID1)
External(DID2)
External(DID3)
External(DID4)
External(DID5)
External(DID6)
External(DID7)
External(DID8)
External(ASL_DGPU_SCOPE.NVJT, MethodObj)

Scope (ASL_IGPU_SCOPE)
{

    Method(_INI,0)
    {
        //DIDx values have been changed in MxmAcpiTables.c 
        //Port - D to be used for eDP only and not as DFP. Hence generating a new toggle list
        Store(DID1, Index(TLPK,0))      // CRT
        Store(DID2, Index(TLPK,2))      // LFP 
        Store(DID3, Index(TLPK,4))      // DP_B
        Store(DID4, Index(TLPK,6))      // HDMI_B 
        Store(DID5, Index(TLPK,8))      // HDMI_C
        Store(DID6, Index(TLPK,10))     // DP_D
        Store(DID7, Index(TLPK,12))     // HDMI_D             
        Store(DID2, Index(TLPK,14))     // LFP+CRT                 
        Store(DID1, Index(TLPK,15))
        Store(DID2, Index(TLPK,17))     // LFP+DP_B                
        Store(DID3, Index(TLPK,18))
        Store(DID2, Index(TLPK,20))     // LFP+HDMI_B                
        Store(DID4, Index(TLPK,21))
        Store(DID2, Index(TLPK,23))     // LFP+HDMI_C              
        Store(DID5, Index(TLPK,24))
        Store(DID2, Index(TLPK,26))     // LFP+DP_D                
        Store(DID6, Index(TLPK,27))
        Store(DID2, Index(TLPK,29))     // LFP+HDMI_D
        Store(DID7, Index(TLPK,30))       
    }

//
// MXMX method is dupplicated under GFX0 scope in INTELGFX.ASL
// need to replace it with method in this file.
//
#ifndef OPTIMUS_DSM_GUID
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
        P8DB(0x77, Arg0, 2000)

        If (LEqual (Arg0, 0)) 
        {
            P8XH (One, 0x77)
            P8XH (Zero, Zero)
            Return (One)
        }
            
        If (LEqual (Arg0, 1)) 
        {
            P8XH (One, 0x77)
            P8XH (Zero, One)
            Return (One)
        }

        If (LEqual (Arg0, 2))
        {
            P8XH (One, 0x77)
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

#endif // OPTIMUS_DSM_GUID

//<AMI_SHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    NVIG
//
// Description:  Nvidia NVIG (iGPU) OperationRegion
//
//-------------------------------------------------------------------------
//<AMI_SHDR_END>    
    OperationRegion(NVIG,SystemMemory,NVGA,0x400)
    Field(NVIG, DWordAcc, NoLock, Preserve)
    {
        // OpRegion Header

        NISG,   0x80,          // (000h) Signature-"NVSG-IGD-DSM-VAR".
        NISZ,   0x20,          // (010h) OpRegion Size in KB.
        NIVR,   0x20,          // (014h) OpRegion Version.

        // OpRegion Data
        GPSS,   0x20,           // Policy Selection Switch Status (Policy selection)
        GACD,   0x10,           // Active Displays
        GATD,   0x10,           // Attached Displays
        LDES,   0x08,           // Lid Event State
        DKST,   0x08,           // Dock State
        DACE,   0x08,           // Display ACPI event
        DHPE,   0x08,           // Display Hot-Plug Event
        DHPS,   0x08,           // Display Hot-Plug Status
        SGNC,   0x08,           // Notify Code (Cause of Notify(..,0xD0))
        GPPO,   0x08,           // Policy Override (Temporary ASL variables)
        USPM,   0x08,           // Update Scaling Preference Mask (Temporary ASL variable)
        GPSP,   0x08,           // Panel Scaling Preference
        TLSN,   0x08,           // Toggle List Sequence Number
        DOSF,   0x08,           // Flag for _DOS
        ELCL,   0x10,           // Endpoint Link Contol Register Value
    }
    
    // Toggle List Package
    Name(TLPK,Package()
    {
        //fix this toggle list. DIDx values have been changed in MxmAcpiTables.c 
        0xFFFFFFFF, 0x2C,                           // CRT
        0xFFFFFFFF, 0x2C,                           // LFP
        0xFFFFFFFF, 0x2C,                           // DP_B
        0xFFFFFFFF, 0x2C,                           // HDMI_B
        0xFFFFFFFF, 0x2C,                           // HDMI_C
        0xFFFFFFFF, 0x2C,                           // DP_D
        0xFFFFFFFF, 0x2C,                           // HDMI_D
        0xFFFFFFFF, 0xFFFFFFFF, 0x2C,               // LFP+CRT
        0xFFFFFFFF, 0xFFFFFFFF, 0x2C,               // LFP+DP_B
        0xFFFFFFFF, 0xFFFFFFFF, 0x2C,               // LFP+HDMI_B
        0xFFFFFFFF, 0xFFFFFFFF, 0x2C,               // LFP+HDMI_C
        0xFFFFFFFF, 0xFFFFFFFF, 0x2C,               // LFP+DP_D
        0xFFFFFFFF, 0xFFFFFFFF, 0x2C,               // LFP+HDMI_D

    })
  
//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    INDL
//
// Description:  Initialize Global Next active device list.
//
// Input: None
//
// Output: None
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
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

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    SND1
//
// Description:  Set Next active device for a single device
//
// Input: 
//  Arg0 : Device ID of the device that's to be set as next active device.
//
// Output: None
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
    Method(SND1, 1, Serialized)
    {
        If(LEqual(Arg0, DID1))
        {
                Store(1, NXD1)
        }
        If(LEqual(Arg0, DID2))
        {
                Store(1, NXD2)
        }
        If(LEqual(Arg0, DID3))
        {
                Store(1, NXD3)
        }
        If(LEqual(Arg0, DID4))
        {
                Store(1, NXD4)
        }
        If(LEqual(Arg0, DID5))
        {
                Store(1, NXD5)
        }
        If(LEqual(Arg0, DID6))
        {
                Store(1, NXD6)
        }
        If(LEqual(Arg0, DID7))
        {
                Store(1, NXD7)
        }
        If(LEqual(Arg0, DID8))
        {
                Store(1, NXD8)
        }
                
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    SNXD
//
// Description:  Set Next active device
//
// Input: 
//      Arg0 TLSN
//
// Output: None
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
    Method(SNXD, 1, Serialized)
    {
        INDL()
        
        //
        // Locate the toggle table entry corresponding to TLSN value
        // Toggle list entries are separated by 0x2C.
        //
        
        Store(1, Local0)  // Local0 to track entries. Point to the first entry (TLSN starts from 1)
        Store(0, Local1)  // Local1 to track elements inside the TLPK package (ACPI IDs and '0x2C')

        while(LLess(Local0, Arg0))  // TLSN start from 1!!
        {
            if(LEqual(DeRefOf(Index(TLPK,Local1)), 0x2C))
            {
                Increment(Local0)
            }
            Increment(Local1) 
            
        }

        SND1(DeRefOf(Index(TLPK, Local1)))      // 1 st ACPI ID in the entry corresponding to TLSN
        Increment(Local1)
        if(LNotEqual(DeRefOf(Index(TLPK,Local1)), 0x2C))  // Check for separator
        {
            SND1(DeRefOf(Index(TLPK, Local1)))  // 2 nd ACPI ID in the entry corresponding to TLSN
        }
        
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    CTOI
//
// Description:  Convert _DOD indices-> MDTL index
//
// Input: 
//          Arg 0 is the currently active display list
//
// Output: None
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
    Method(CTOI,1, Serialized)
    {
        Switch(ToInteger(Arg0))        //Arg 0 is the currently active display list
        {
            //_DOD indices-> MDTL index
            case(0x1)       {Return(1)}     //CRT
            case(0x2)       {Return(2)}     //LFP
            case(0x4)       {Return(3)}     //DP_B
            case(0x8)       {Return(4)}     //HDMI_B
            case(0x10)      {Return(5)}     //HDMI_C           
            case(0x20)      {Return(6)}     //DP_D
            case(0x40)      {Return(7)}     //HDMI_D
            case(0x3)       {Return(8)}     //LFP+CRT
            case(0x6)       {Return(9)}     //LFP+DP_B
            case(0xA)       {Return(10)}    //LFP+HDMI_B
            case(0x12)      {Return(11)}    //LFP+HDMI_C
            case(0x22)      {Return(12)}    //LFP+DP_D
            case(0x42)      {Return(13)}    //LFP+HDMI_D
            Default         {Return(1)}
        }
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
