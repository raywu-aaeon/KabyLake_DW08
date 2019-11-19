//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
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
//<AMI_FHDR_START>
//
// Name:        PlatformExternal.asl
//
//<AMI_FHDR_END>
//*************************************************************************

External(LHIH)
External(LLOW)
External(IGDS)
External(LIDS)
External(BRTL)
External(ALSE)
External(GSMI)
External(\_SB.PCI0.GFX0.ALSI)
External(\_SB.PCI0.GFX0.CDCK)
External(\_SB.PCI0.GFX0.CBLV)
External(\_SB.PCI0.GFX0.GSSE)
External(\_SB.PCI0.GFX0.DD01, DeviceObj)
External(\_SB.PCI0.GFX0.DD02, DeviceObj)
External(\_SB.PCI0.GFX0.DD03, DeviceObj)
External(\_SB.PCI0.GFX0.DD04, DeviceObj)
External(\_SB.PCI0.GFX0.DD05, DeviceObj)
External(\_SB.PCI0.GFX0.DD06, DeviceObj)
External(\_SB.PCI0.GFX0.DD07, DeviceObj)
External(\_SB.PCI0.GFX0.DD08, DeviceObj)
External(\_SB.PCI0.GFX0.DD1F, DeviceObj)
External(\_SB.PCI0.GFX0.GDCK, MethodObj)
External(\_SB.PCI0.GFX0.GHDS, MethodObj)
External(\_SB.PCI0.GFX0.AINT, MethodObj)
External(\_SB.PCI0.GFX0.GLID, MethodObj)
External(\_SB.PCI0.GFX0.GSCI, MethodObj)
External(\_PR.PR00, DeviceObj)
External(\_PR.PR00._PSS, MethodObj)
External(\_PR.PR00.LPSS, PkgObj)
External(\_PR.PR00.TPSS, PkgObj)
External(\_PR.PR00._PPC, MethodObj)
External(\_PR.CPPC, IntObj)
External(\_TZ.TZ00, DeviceObj)
External(\_TZ.TZ01, DeviceObj)
External(\_TZ.ETMD, IntObj)
External(\_TZ.FN00._OFF, MethodObj)

#if !defined(ASL_EC_SUPPORT) || (ASL_EC_SUPPORT == 0)
External(\_SB.PCI0.GFX0.CLID)
External(\_SB.PCI0.GFX0.IUEH, MethodObj)
External(\_SB.IETM, DeviceObj)
#endif // ASL_EC_SUPPORT

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
