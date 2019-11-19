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
// $Header: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPEG/ATdGPU.asl 1     1/15/13 5:58a Joshchou $
//
// $Revision: 1 $
//
// $Date: 1/15/13 5:58a $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPEG/ATdGPU.asl $
// 
//**********************************************************************
Scope(ASL_DGPU_SCOPE)
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
        ASL_PCI_SCOPE.PGON(ASL_SG_H_PORT_FUNC)  // OEM Mxm Power On

        //Set the SSID for the ATI MXM
//        Store(MXM_SSVID_DID, SSID)

        //Ask OS to do a PnP rescan
        Notify(ASL_PEG_SCOPE,0)                      
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
        ASL_PCI_SCOPE.PGOF(ASL_SG_H_PORT_FUNC)  // OEM Mxm Power On

        //Ask OS to do a PnP rescan
        Notify(ASL_PEG_SCOPE,0)
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    _INI
//
// Description:  dGPU Init control method. Used to force dGPU _ADR to return proper PCI address
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>    
//    Method (_INI)
//    {       
// should already be set by now...
////        Store(MXM_SSVID_DID, SSID) //Set the SSID for the ATI MXM
//        Store(0x0, ASL_DGPU_SCOPE._ADR) //make sure PEGP address returns 0x00000000
//    }

} // end Scope(ASL_DGPU_SCOPE)

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