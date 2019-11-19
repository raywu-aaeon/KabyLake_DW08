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
// $Header: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPCH/ATdGPUPCH.asl 1     1/15/13 6:02a Joshchou $
//
// $Revision: 1 $
//
// $Date: 1/15/13 6:02a $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPCH/ATdGPUPCH.asl $
// 
//**********************************************************************
Scope(ASL_DGPUPCH_SCOPE)
{   
//    OperationRegion (PEGR, PCI_Config, 0, 0x100)
//    Field(PEGR, DWordAcc, Lock, Preserve)
//    {
//        Offset(0x4C),
//        SSID, 32,
//    }        

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

} // end Scope(ASL_DGPUPCH_SCOPE)
