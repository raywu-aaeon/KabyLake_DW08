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
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************

/** @file AmiSiPolicyHobLib.c
    

**/

#include <Library/DebugLib.h>
#include <AmiCspLib.h>
#include <Ppi\SiPolicy.h>
#include <Library\PeiSiPolicyLibPreMem\PeiSiPolicyLibraryPreMem.h>
//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------

// GUID Definition(s)

// Protocol/Ppi Definition(s)

// Function Definition(s)

// Function Prototype(s)

// External Declaration(s)

//---------------------------------------------------------------------------

/**
    This function invokes AmiUpdateSiPolicyHob()

    @param *Policy         - Pointer to RC policy
        structure  
    @param PolicySize      - RC policy Size

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID AmiUpdateSiPolicyHob (
  IN OUT  VOID       *Policy,
  IN      UINT16     PolicySize,
  IN OUT  VOID       *PeiRcPolicyHobPtr
)
{
	SI_POLICY_PPI            *mSiPolicyPpi = Policy; 
	
	mSiPolicyPpi->Revision        = SI_POLICY_REVISION;

	  //
	  // PlatformData configuration
	  //
	//  SiPolicy->EcPresent       = 0;
	  //
	  // Temporary Bus range for silicon initialization.
	  //
	mSiPolicyPpi->TempPciBusMin   = 2;
	mSiPolicyPpi->TempPciBusMax   = 10;
	  //
	  // Temporary Memory Base Address for PCI devices to be used to initialize MMIO registers.
	  // Minimum size is 2MB bytes.
	  //
	mSiPolicyPpi->TempMemBaseAddr = TEMP_MEM_BASE_ADDRESS;
	mSiPolicyPpi->TempMemSize     = TEMP_MEM_SIZE;
	  //
	  // Temporary IO Base Address for PCI devices to be used to initialize IO registers.
	  // And size of temporary IO space.
	  //
	mSiPolicyPpi->TempIoBaseAddr  = TEMP_IO_BASE_ADDRESS;
	mSiPolicyPpi->TempIoSize      = TEMP_IO_SIZE;
	  
 return;
}
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
