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
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: ECPei.c
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <Protocol/AmiSio.h>
#include <Library/AmiSioPeiLib.h>
#include <AmiCspLib.h>

#ifndef SIO_Smsc1007
  #define SIO_Smsc1007    0
#endif

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: PEI_EC_INIT
//
// Description: This eLink function is used to open IoDecode for logic devices initialized in PEI
//
// Input:
//  IN  EFI_FFS_FILE_HEADER    *FfsHeader - Logical Device's information
//  IN  EFI_PEI_SERVICES       **PeiServices  - Read/Write PCI config space
//
// Output:    None
//
// Modified:  Nothing
//
// Referrals: None
//
// Note:
//------------------------------------------------------------------------- 
// <AMI_PHDR_END> 

EFI_STATUS
EFIAPI
PeiECInit (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    Status = SbLibSetLpcDeviceDecoding(NULL, 0x60, 0, dsPS2K);   // Decode SB Port 0x60,0x64
    Status = SbLibSetLpcDeviceDecoding(NULL, 0x62, 0, 0xFF);     // Decode SB Port 0x62,0x66
    Status = SbLibSetLpcDeviceDecoding(NULL, 0x3F8, 0, dsUART);  // Decode SB COMA port 0x3F8    

    //Decode IO space range for SIO and EC
#if SMSC1007_SUPPORT
    WRITE_PCI8_SB(SB_REG_GEN2_DEC + 0x02, 0x7C);
    WRITE_PCI8_SB(SB_REG_GEN2_DEC + 0x01, Smsc1007_PME_BASE_ADDRESS >> 8);
    WRITE_PCI8_SB(SB_REG_GEN2_DEC , ((Smsc1007_PME_BASE_ADDRESS & 0x0F0) + 1));
#else
    Status = SbLibSetLpcGenericDecoding(NULL, EC_PME_BASE_ADDRESS, 0x80, TRUE);  // PME Base decode
#endif

    return Status;
}

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
