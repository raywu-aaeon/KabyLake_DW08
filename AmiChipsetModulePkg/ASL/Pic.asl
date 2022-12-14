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
// $Header: /Alaska/BIN/Chipset/Template/SouthBridge/Chipset/PIC.asl 5     6/27/11 2:05p Artems $
//
// $Revision: 5 $
//
// $Date: 6/27/11 2:05p $
//**********************************************************************;
//;**********************************************************************;

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        PIC.ASL
//
// Description: This file contains ACPI methods implementation for SB PIC controller
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

Device(PIC) {
    Name(_HID,EISAID("PNP0000"))    // Hardware Device ID
    Name(_CRS, ResourceTemplate()
    {
        IO(Decode16, 0x20, 0x20, 0, 0x2)
        IO(Decode16, 0xA0, 0xA0, 0, 0x2)
//      IRQ(Edge, ActiveHigh, Exclusive ){2}
        IRQNoFlags(){2}
    })
}   // End of PIC

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
