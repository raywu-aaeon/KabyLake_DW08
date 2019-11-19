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

/** @file AmiBiosGuardMiscLib.h
  
*/

#ifndef __AMI_BIOSGUARD_VERIFY_OEM_BIN_LIB_H__
#define __AMI_BIOSGUARD_VERIFY_OEM_BIN_LIB_H__

/**    
    This function is use to verify OEM Bin.
    OEM porting required.
    
    @param Data - pointer to OEM bin.
    @param DataLength - size of the OEM bin.

    @retval  EFI_SUCCESS - Image is verify pass.
**/
EFI_STATUS
VerifiyOemBinImage(
  IN UINT8  *Data,
  IN UINT32 DataLength
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
