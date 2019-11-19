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

/** @file AmiBiosGuardMiscLib.c


*/

#include <Token.h>
#include <Uefi.h>
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiMultiPhase.h>

#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <BiosGuard.h>

#define BIOSGUARD_PACKAGE_CERT   524

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
)
{
    UINT32              AmiBiosGuardHeaderLength = 0;
    BGUP_HEADER         *BgupHdr;
    
    AmiBiosGuardHeaderLength = *(UINT32*)OriBiosGuardImageAddr;
    BgupHdr = (BGUP_HEADER*)(OriBiosGuardImageAddr + AmiBiosGuardHeaderLength);
    
    do {
        if ( BgupHdr->ScriptSectionSize == 0xFFFFFFFF ) break;

        //
        // Skip ScriptSection and BGUP Header
        //
        AmiBiosGuardHeaderLength += (BgupHdr->ScriptSectionSize + sizeof(BGUP_HEADER));

        //
        // Collect DataSection only to buffer.
        //
        CopyMem ((UINT8*)(ReOrgImageAddr + *(CurrentBGUPAddress)), (UINT8*)OriBiosGuardImageAddr + AmiBiosGuardHeaderLength, BgupHdr->DataSectionSize);

#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((DEBUG_INFO, "ROM Address : %X \n", *CurrentBGUPAddress));
        DEBUG ((DEBUG_INFO, "ROM Length  : %X \n\n", BgupHdr->DataSectionSize));
#endif
        
        //
        // Pointer to next BGUP block.
        //
        AmiBiosGuardHeaderLength += (BgupHdr->DataSectionSize + BIOSGUARD_PACKAGE_CERT);
        *CurrentBGUPAddress += BgupHdr->DataSectionSize;

        BgupHdr = (BGUP_HEADER*)(OriBiosGuardImageAddr + AmiBiosGuardHeaderLength);
    } while ( AmiBiosGuardHeaderLength < ImageSize );
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
