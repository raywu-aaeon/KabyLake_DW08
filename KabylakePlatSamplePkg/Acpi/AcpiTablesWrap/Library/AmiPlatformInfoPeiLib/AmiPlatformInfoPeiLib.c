//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
/** @file SbPeiLib.c
    This file contains code for Southbridge initialization library function
    in the PEI stage

**/

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Pei.h>
#include <Hob.h>
#include <Token.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/CpuIo.h>
#include <AmiCspLib.h>
#include <AmiPeiLib.h>
#include <Guid/AmiPlatformInfoHob.h>
//----------------------------------------------------------------------------
// GUID Definition(s)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

BOOLEAN IsRecovery (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI         *PciCfg,
    IN EFI_PEI_CPU_IO_PPI           *CpuIo
);

/**
    This function determines if the system is Override BoardID
    sleep state.

    @param PeiServices - Pointer to the Pei Services function and data
        structure.
    @param AmiOemPlatformInfo - Pointer to the Ami oem platform info data
        structure.
        
    @retval TRUE It is Override to PLATFORM INFO
    @retval FALSE It is not Override.
**/

BOOLEAN AmiPlatformInfoPeiOverride (
  IN EFI_PEI_SERVICES     **PeiServices,
  IN AMI_OEM_PLATFORM_INFO   *AmiOemPlatformInfo )
{

    
    return FALSE;
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
