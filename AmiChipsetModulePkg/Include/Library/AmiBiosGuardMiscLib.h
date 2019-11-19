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

#ifndef __AMI_BIOSGUARD_MISC_LIB_H__
#define __AMI_BIOSGUARD_MISC_LIB_H__

/**    
    This function is used to re-organize original Bios image.

    @param OriBiosGuardImageAddr - Original BiosGuard image address.
    @param ReOrgImageAddr        - Address to put image after organization.
    @param ImageSize             - Size of BiosGuard image.
    @param CurrentBGUPAddress    - Pointer to the end of image(Equal to the size of ReOrgImage).

    @retval VOID
**/
VOID
AmiBiosGuardReOrgnizeRomImage(
  EFI_PHYSICAL_ADDRESS     OriBiosGuardImageAddr,
  EFI_PHYSICAL_ADDRESS     ReOrgImageAddr,
  UINT32                    ImageSize,
  UINT32                    *CurrentBGUPAddress
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
