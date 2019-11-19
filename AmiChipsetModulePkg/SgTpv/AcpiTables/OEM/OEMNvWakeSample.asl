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

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    NBCI function related definition
//
// Description:  Sample code for OEM to modify for their requirement to meet their H/W design, 
//               and replace the path in the TOKEN "OEM_NV_NBCI_NAME_DEFINE_ELINK_PATH"      
//               
//<AMI_PHDR_END>
//
External(NVAF)
External(\_SB.PCI0.PEG0.PEGP.NHDA, IntObj)

Method(PWAK,1,Serialized)
{
  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
  {
    If(LEqual(NVAF, 1))
    {
      Store(1, \_SB.PCI0.PEG0.PEGP.NHDA)
    }
    Else
    {
      Store(0, \_SB.PCI0.PEG0.PEGP.NHDA)
    }
  }
}

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
//**********************************************************************