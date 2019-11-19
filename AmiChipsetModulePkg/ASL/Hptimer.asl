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
// $Header: /Alaska/BIN/Chipset/Template/SouthBridge/Chipset/Hptimer.asl 5     6/27/11 2:05p Artems $
//
// $Revision: 5 $
//
// $Date: 6/27/11 2:05p $
//**********************************************************************;
//**********************************************************************;

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        Hptimer.ASL
//
// Description: This file contains ACPI methods implementation for High Performance Event Timer (HPET)
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

Device(HPET)
        {
    Name(_HID,EISAID("PNP0103"))
    Name(_CRS,ResourceTemplate()
    {
        IRQNoFlags(){0}
        IRQNoFlags(){8}
        Memory32Fixed(ReadOnly,0xFEC01000, 0x00000400, MMT) // 1kb reserved space
    })
    OperationRegion(\_SB.PCI0.SBRG.LPCR, PCI_Config, 0xA0, 0x1)
    Field(\_SB.PCI0.SBRG.LPCR, ByteAcc, NoLock, Preserve)
    {
        HPTE,1,
    }
    Method(_STA, 0)
    {
//      ShiftRight(And(HPTE ,0x00038000),0x0F,Local0)
        If(HPTE){
        //And(HPTE,0x01,Local0)
//      If(OSFL)
//      {
//          If(Local0){ Return(0x0B)} // hide for win9x
//      } Else {    //XP/W2k/.NET,...
            //If(Local0) { 
//              Store(0xEE, DBG8)
                Return(0x0F) 
            }
//      }
//      Store(0xE0, DBG8)
        Return(0x00)
    }
/*  Method(_CRS, 0)
    {
//      ShiftRight(And(HPTE, 0x00038000), 0x0F,Local0)
//      If(And(Local0,0x04))
//      {

            CreateDwordField(CRS, CRS.MMT, HPT)
            If(LEqual(Local0,0x05))
            {
                Store(0xFEC00100,HPT)
            }
            If(LEqual(Local0,0x06))
            {
                Store(0xFEC00200,HPT)
            }
            If(LEqual(Local0,0x07))
            {
                Store(0xFEC00300,HPT)
            }

//      }
        Return(CRS)
    }
*/

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
