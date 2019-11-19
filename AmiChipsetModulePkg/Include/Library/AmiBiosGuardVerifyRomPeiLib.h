//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiBiosGuardVerifyRomPeiLib.h
  
*/

#ifndef __AMI_BIOSGUARD_VERIFY_ROM_PEI_LIB_H__
#define __AMI_BIOSGUARD_VERIFY_ROM_PEI_LIB_H__

/**    
    This function is use to verify whole Image in PEI phase
    
    @param OriBiosGuardImageAddr - pointer to the verify image
    @param ImageSize - size of the verify image

    @retval  EFI_SUCCESS - Image is verify pass.
**/
EFI_STATUS
AmiBiosGuardVerifiyBiosImage(
  EFI_PEI_SERVICES         **PeiServices,
  EFI_PHYSICAL_ADDRESS     OriBiosGuardImageAddr,
  UINT32                   ImageSize
);

#endif


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
