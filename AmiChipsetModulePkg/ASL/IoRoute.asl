//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
//
// $Header: /Alaska/BIN/Chipset/Template/SouthBridge/Chipset/IOROUTE.ASL 4     6/27/11 2:05p Artems $
//
// $Revision: 4 $
//
// $Date: 6/27/11 2:05p $
//
//*****************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        IOROUTE.ASL
//
// Description: This file contains Set of ACPI Control Methods for SB LPC device 
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

    OperationRegion(Rx80, PCI_Config, 0x00, 0xFF)
    Field(Rx80, ByteAcc, NoLock, Preserve) {
//  LPC_I/O_DEC, reg#80h
        Offset(0x80),
        LPCD,   16, // BIT[2:0] : COMA Decode Range
                // BIT[3]   : RESERVED
                // BIT[6:4] : COMB Decode Range
                // BIT[7]   : RESERVED
                // BIT[9:8] : LPT Decode Range
                // BIT[11:10]   : RESERVED
                // BIT[12]  : FDD Decode Range
                // BIT[15:13]   : RESERVED
//  LPC_EN, reg#82h
        LPCE,   16, // BIT[0]   : COMA Decoding
                // BIT[1]   : COMB Decoding
                // BIT[2]   : LPT Decoding
                // BIT[3]   : FDD Decoding
                // BIT[7:4] : RESERVED
                // BIT[8]   : GAME Decoding in 0x200-207
                // BIT[9]   : GAME Decoding in 0x208-20F
                // BIT[10]  : KBC Decoding
                // BIT[11]  : MC Decoding
                // BIT[12]  : CFG1 (2Eh & 2Fh) Decoding
                // BIT[13]  : CFG2 (4Eh & 4Fh) Decoding
                // BIT[15:14]   : RESERVED
    }

//  Devices Base Port Table
    Name(DBPT, Package() {
        //  0x00 - COMA
        Package() {
            0x3F8, 0x2F8, 0x220, 0x228, 0x238, 0x2E8, 0x338, 0x3E8
        },
        //  0x01 - COMB
        Package() {
            0x3F8, 0x2F8, 0x220, 0x228, 0x238, 0x2E8, 0x338, 0x3E8
        },
        //  0x02 - LPT
        Package() {
            0x378, 0x278, 0x3BC
        },
        //  0x03 - FDD
        Package() {
            0x3F0, 0x370
        },
    })

//  Devices Decode ranges register Layout Table
    Name(DDLT, Package() {
        //  0x00 - COMA
        Package() { 0,  0xFFF8 },
        //  0x01 - COMB
        Package() { 4,  0xFF8F },
        //  0x02 - LPT
        Package() { 8,  0xFCFF },
        //  0x03 - FDD
        Package() { 12, 0xEFFF },
    })

/*
;<AMI_PHDR_START>
;------------------------------------------------------------------------
;
; Procedure:    RRIO
;
; Description:  Route/Release IO ports to(from) EIO(LPC) Bus 
;
; Input:    Arg0   Device Category
;       0x00 - Serial A (Modem)
;         01 - Serial B (IR)
;         02 - LPT
;         03 - FDD
;         04 - SB16 Audio
;         05 - Midi
;         06 - Mss Audio
;         07 - Adlib sound (FM synth)
;         08 - Game 1 port
;         09 - Game 2 port
;         0A - KBC 60 & 64
;         0B - EC 62 & 66
;         0C - CNF1 2e & 2f (static location)
;         0D - CNF2 4e & 4f (static location)
;         0E - RESERVED
;         0F - RESERVED
;       0x10 - Generic I/O Range 1 // IRDA
;         11 - Generic I/O Range 2
;         12 - Generic I/O Range 3
;         13 - Generic I/O Range 4
;
;       Arg1   1/0 Route/Release Mode
;       Arg2   Port to Route/Release
;       Arg3   Port SIZE to Route (not used for ICH)
;
; Output:   Nothing
;
;-------------------------------------------------------------------------
;<AMI_PHDR_END>
*/
Method(RRIO, 4) {
//  COMA/COMB/LPT/FDD Decoding Handle
    if  ( LAnd(LLessEqual(Arg0, 0x03), LGreaterEqual(Arg0, 0x00)) ) {
        Store(Match(DeRefOf(Index(DBPT, Arg0)), MEQ, Arg2, MTR, 0, 0), Local0)
                        // Local0 - offset within I/O Devices Base Port Table
        if  ( LNotEqual(Local0, Ones) ) {
                        // port is match any one listed in Devices Base Port Table of Device Category
            Store(DeRefOf(Index(DeRefOf(Index(DDLT, Arg0)), 0)), Local1)
                        // Local1 - shift left #
            Store(DeRefOf(Index(DeRefOf(Index(DDLT, Arg0)), 1)), Local2)
                        // Local2 - register mask
            ShiftLeft(Local0, Local1, Local0)
            And(LPCD, Local2, LPCD)
            Or(LPCD, Local0, LPCD)

            Wx82(Arg0, Arg1)
        }
    }

//  Midi Decoding Handle
    if  ( LEqual(Arg0, 0x08) ) {
        if  ( LEqual(Arg2, 0x200) ) {
            Wx82(0x08, Arg0)
        }
        elseif  ( LEqual(Arg2, 0x208) ) {
            Wx82(0x09, Arg0)
        }
    }

//  KBC/MC/CNF1/CNF2 Decoding Handle
    if  ( LAnd(LLessEqual(Arg0, 0x0D), LGreaterEqual(Arg0, 0x0A)) ) {
        Wx82(Arg0, Arg1)
    }
}

Method (Wx82, 2) {
//  Arg0: bit# to set/reset
//  Arg1: decode enable/disable

    ShiftLeft(0x01, Arg0, Local0)
    if  (Arg1) {
        Or(LPCE, Local0, LPCE)
    }
    else    {
        Not(Local0, Local0)
        And(LPCE, Local0, LPCE)
    }
}

/*
;<AMI_PHDR_START>
;------------------------------------------------------------------------
;
; Procedure:    rDMA
;
; Description:  Route DMA Channels to EIO/LPC Bus
;
; Input:    Arg0    Device Category
;       Arg1    1/0 Route/Release Mode
;       Arg2    DMA channel to route (1 based)
;
; Output:   Nothing
;
;-------------------------------------------------------------------------
;<AMI_PHDR_END>
*/
Method(rDMA, 3) {
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
