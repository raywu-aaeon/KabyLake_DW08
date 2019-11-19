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

/** @file AmiTxtPei.c
    Install Tpm initialize ppi.

**/

#include <Pei.h>
#include <AmiPeiLib.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Setup.h>
#include <Token.h>
//#include <Library\DebugLib.h>

#define PEI_TPM_INITIALIZED_PPI_GUID \
  { \
    0xe9db0d58, 0xd48d, 0x47f6, 0x9c, 0x6e, 0x6f, 0x40, 0xe8, 0x6c, 0x7b, 0x41 \
  }

#define PEI_TPM_DETECTIONAND_INIT_DONE_PPI_GUID \
  { \
    0xa030d115, 0x54dd, 0x447b, 0x90, 0x64, 0xf2, 0x06, 0x88, 0x3d, 0x7c, 0xcc  \
  }

static EFI_GUID gPeiTpmInitializedPpiGuid = PEI_TPM_INITIALIZED_PPI_GUID;
EFI_GUID gEfiPeiReadOnlyVariablePpiGuid = EFI_PEI_READ_ONLY_VARIABLE_PPI_GUID;
static EFI_GUID gEfiSetupGuid                  = SETUP_GUID;
static EFI_GUID gPeiTpmDetectionandInitDonePpiGuid = PEI_TPM_DETECTIONAND_INIT_DONE_PPI_GUID;

static EFI_PEI_PPI_DESCRIPTOR Tpm_Initialized[] =
{ 
	{
		EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
		&gPeiTpmInitializedPpiGuid, NULL 
	}	
};

static EFI_PEI_PPI_DESCRIPTOR Tpm_Initialized2[] =
{ 
	{
		EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
		&gPeiTpmDetectionandInitDonePpiGuid, NULL 
	}
};

/**
    PEI Entry Point for Ami TXT Driver.

    @param FfsHeader pointer to image FFS file
    @param PeiServices pointer to PEI services

    @retval EFI_STATUS Return EFI status.
**/

EFI_STATUS
AmiTxtPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{  
    UINT8       *TpmBaseAddr = (UINT8*)0xfed40000;
    EFI_PEI_READ_ONLY_VARIABLE_PPI  *ReadOnlyVariable;
    EFI_STATUS  Status;
    UINTN       VariableSize;
    
    SETUP_DATA SetupData;

    if ( *TpmBaseAddr != 0xff ){  //If TPM device is present, check TPM setup data
    	Status = (*PeiServices)->LocatePpi(
            PeiServices,
            &gEfiPeiReadOnlyVariablePpiGuid,
            0, 
            NULL,
            &ReadOnlyVariable       
        );                                        
    
        if (EFI_ERROR(Status)) return EFI_SUCCESS;
                                        
        VariableSize = sizeof(SETUP_DATA);
    
        Status = ReadOnlyVariable->PeiGetVariable(
            PeiServices,
            L"Setup", 
            &gEfiSetupGuid,
            NULL,
            &VariableSize,  
            &SetupData
        );
		
		if ( EFI_ERROR(Status) ) return EFI_SUCCESS;
		
#if TCG_SUPPORT || TCG2Support     
        if ( SetupData.TpmSupport ) {
            (*PeiServices)->InstallPpi(PeiServices, Tpm_Initialized);    //Install PPI if TPM device detected and enable in setup
        }
#endif            
    }
    
    // Always intall TpmInitializationDonePpi no matter success or fail.
    (*PeiServices)->InstallPpi(PeiServices, Tpm_Initialized2);
    
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
