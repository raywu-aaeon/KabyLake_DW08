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
// $Header: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPEG/AtiSSDT.asl 1     1/15/13 5:58a Joshchou $
//
// $Revision: 1 $
//
// $Date: 1/15/13 5:58a $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/SharkBayRefCodes/SwitchableGraphics/Sg TPV/Sg Acpi Tables/SgTpvPEG/AtiSSDT.asl $
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
External(ASL_PEG_SCOPE, DeviceObj)
External(ASL_DGPU_SCOPE, DeviceObj)
External(ASL_IGPU_SCOPE, DeviceObj)
External(ASL_DGPU_SCOPE._ADR, DeviceObj)
External(ASL_PCI_SCOPE.PGON, MethodObj)
External(ASL_PCI_SCOPE.PGOF, MethodObj)
External(ASL_PCI_SCOPE.SGPI, MethodObj)
External(ASL_PCI_SCOPE.SGPO, MethodObj)
External(\DSEL)
External(\ESEL)
External(\SSEL)
External(\PSEL)

#include <ATdGPU.asl>     // Include dGPU device namespace
#include <ATiGPU.asl>     // Include IGD _DSM and AMD ATIF/ATPM/ATRM methods
#include <ATiGDmisc.asl>  // Include misc event callback methods

} // end SSDT       

//**********************************************************************
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
