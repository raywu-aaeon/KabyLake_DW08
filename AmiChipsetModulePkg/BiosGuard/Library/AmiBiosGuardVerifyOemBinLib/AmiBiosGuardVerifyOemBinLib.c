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

/** @file AmiBiosGuardVerifyRomPeiLib.c


*/

#include <Token.h>
#include <Uefi.h>
#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>


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
  IN UINT32  DataLength
)
{   
    return EFI_SUCCESS;
}

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
