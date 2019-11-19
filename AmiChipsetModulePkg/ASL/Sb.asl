//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/LynxPoint/Intel Pch SB Chipset/SB.ASL 2     8/13/12 10:24a Victortu $
//
// $Revision: 2 $
//
// $Date: 8/13/12 10:24a $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:        SB.asl
//
// Description: The ASL file is for South Bridge specific function.
//
//<AMI_FHDR_END>
//*************************************************************************
/*
;<AMI_PHDR_START>
;------------------------------------------------------------------------
;
; Procedure:    SPTS
; Description:  METHOD IS CALLED BY OS PRIOR TO ENTER ANY SLEEP STATE
; Input: Arg0 = Arg0 = Sleep state System about to enter
; Output: Nothing
;
;-------------------------------------------------------------------------
;<AMI_PHDR_END>
*/
    Method (SPTS, 1) {
        Store(One, SLPX)        // Clear SLP_SMI Status
        Store(One, SLPE)        // Enable SLP_SMI
        If(LEqual(Arg0, 3)) {Store(One, AES3)}        // Set normal SLP 3 flag for EIP251482
    }

/*
;<AMI_PHDR_START>
;------------------------------------------------------------------------
;
; Procedure:    SWAK
; Description:  METHOD CALLED ON WAKE UP FROM ANY SLEEP STATE
; Input: Arg0 = Sleep state System is resuming from
; Output: Nothing
;
;------------------------------------------------------------------------
;<AMI_PHDR_END>
*/
    Method (SWAK, 1) {
        Store(Zero, SLPE)       // Disable SLP_SMI
        Store(Zero, AES3)        // Clear normal SLP flag for EIP251482.
        // Start of fix for S1->S4 & S3->S4 Transition and Video Repost without pressing Second Key.
#if defined ASL_SX_NOTIFY_PWRB && ASL_SX_NOTIFY_PWRB == 1
        If (NTPB) {
            Notify(\_SB.PWRB, 0x02)
        }
#endif
        // End of fix for S1->S4 & S3->S4 Transition and Video Repost without pressing Second Key.
    }

    OperationRegion (SMIE, SystemIO, PMBA, 4)
    Field (SMIE, ByteAcc,NoLock,Preserve) {
        ,10,
        RTCS, 1,
        , 3,
        PEXS, 1,
        WAKS, 1,
        , 8,
        PWBT, 1,
        , 7,
    }

    OperationRegion (SLPR, SystemIO, SMCR, 8)
    Field (SLPR, ByteAcc,NoLock,Preserve) {
        , 4,
        SLPE,1,
        ,31,
        SLPX,1,
        ,27,
    }

//Scope(\_SB) {                 // System Bus scope
//
//  Device(PWRB) {
//    Name(_HID,EISAID("PNP0C0C"))   
//    Method(_STA, 0) {
//        Return(0x0F)
//    }
//  }//end device PWRB
//
//} // end _SB scope    
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
