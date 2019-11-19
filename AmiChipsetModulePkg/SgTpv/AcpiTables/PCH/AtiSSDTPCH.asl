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
// $Header: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPCH/AtiSSDTPCH.asl 1     1/15/13 6:02a Joshchou $
//
// $Revision: 1 $
//
// $Date: 1/15/13 6:02a $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPCH/AtiSSDTPCH.asl $
// 
// 
// 
//**********************************************************************
DefinitionBlock (
        "Amd.aml",
        "SSDT",
        1,
        "AmdRef",
        "AmdTabl",
        0x1000
        ){       

External(P8XH, MethodObj) 
#if defined(ASL_SGTPV_ASL_DEBUG) && (ASL_SGTPV_ASL_DEBUG ==1)
#define P8DB(arg0, arg1, arg2) P8XH (0, arg1) P8XH (1, arg0) sleep(arg2)
#else
#define P8DB(arg0, arg1, arg2) 
#endif

External(ASL_PCI_SCOPE, DeviceObj)
External(ASL_SG_ULT_RP_NUM, DeviceObj)
External(ASL_DGPUPCH_SCOPE, DeviceObj)
External(ASL_IGPU_SCOPE, DeviceObj)
External(ASL_DGPUPCH_SCOPE._ADR, DeviceObj)
External(\_SB.PCI0.HGON, MethodObj)
External(\_SB.PCI0.HGOF, MethodObj)
External(ASL_PCI_SCOPE.SGPI, MethodObj)
External(ASL_PCI_SCOPE.SGPO, MethodObj)
External(\DSEL)
External(\ESEL)
External(\SSEL)
External(\PSEL)


#include <ATdGPUPCH.asl>     // Include dGPU device namespace
#include <ATiGPUPCH.asl>     // Include IGD _DSM and AMD ATIF/ATPM/ATRM methods
#include <ATiGDmiscPCH.asl>  // Include misc event callback methods

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