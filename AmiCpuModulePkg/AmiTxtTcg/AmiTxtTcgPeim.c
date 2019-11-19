//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiTxtTcgPeim.c
    Issue the TPM2 Startup command if TPM2.0 device did not 
    execute startup before.

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <AmiPeiLib.h>
#include <Setup.h>
#include <AmiTxtTcgPeim.h>
#include "TPM20CommLib/Tpm2CommandLib.h"
#include "Tpm12Comm.h"
#include <Hob.h>

#define AMI_SKIP_TPM_STARTUP_GUID \
    { 0x6ee1b483, 0xa9b8, 0x4eaf, 0x9a, 0xe1, 0x3b, 0x28, 0xc5, 0xcf, 0xf3,\
      0x6b}

#define AMI_SGX_SINIT_DATA_GUID \
    { 0x51f30caa, 0x6f0b, 0x4460, 0x8a, 0x67, 0x91, 0x93, 0xf0, 0xf7, 0xd6,\
      0xf3}

EFI_GUID            gSkipTpmStartupGuid = AMI_SKIP_TPM_STARTUP_GUID;
EFI_GUID 			gPeiReadOnlyVariable2PpiGuid = EFI_PEI_READ_ONLY_VARIABLE2_PPI_GUID;
static EFI_GUID 	gEfiSetupGuid                  = SETUP_GUID;
EFI_GUID            gSgxSinitDataGuid = AMI_SGX_SINIT_DATA_GUID;

static EFI_PEI_PPI_DESCRIPTOR SkipTPMStartDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gSkipTpmStartupGuid, \
      NULL }
};

typedef struct {
  EFI_HOB_GUID_TYPE   EfiHobGuidType;
  UINT8  SgxSinitDataFromTpm;           ///< SGX SINIT Data from TPM
} SGX_SINIT_DATA_FROM_TPM;

#define CopyMem AuxCopyMem
#define TPM20_SGX_NVINDEX  0x01800004
#define TPM12_SGX_NVINDEX  0x50000004

#define TPM_TIME_OUT  		750
#define TXT_PUBLIC_BASE		0xFED30000
#define TXT_PTT_FTIF_OFF	0x800
#define TXT_PTT_PRESENT		0x70000
#define TXT_ERROR_STATUS_REG_OFF  0x8

/**
  Determines whether or not the platform has encountered an error during
  a sleep or power-off state.

  @retval TRUE          - If the TXT_WAKE_ERROR bit is asserted.
  @retval FALSE         - If the TXT_WAKE_ERROR bit is unasserted.
**/
BOOLEAN
IsTxtWakeError (
  VOID
  )
{
  UINT8 Ests;

  ///
  /// Read TXT.ESTS register
  ///
  Ests = MmioRead8 (TXT_PUBLIC_BASE + TXT_ERROR_STATUS_REG_OFF);

  DEBUG ((EFI_D_ERROR, "TXT.ESTS=%x\n", Ests));

  return (Ests & (0x1 << 6)) ? TRUE : FALSE;
}

/**
  Is Spt in PTT mode

  @retval TRUE          - If the PTT mode is asserted.
  @retval FALSE         - If the PTT mode is unasserted.
**/

BOOLEAN
IsSptPtt (
  VOID
  )
{
  UINT32 RegRead;

  RegRead = MmioRead32 (TXT_PUBLIC_BASE + TXT_PTT_FTIF_OFF);
  RegRead = RegRead & TXT_PTT_PRESENT;
  if (RegRead == TXT_PTT_PRESENT) {
	DEBUG ((EFI_D_ERROR, "IsSptPtt: PTT cycle\n"));
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Determines whether or not the platform has executed a TXT launch by
  examining the TPM Establishment bit.

  @param[in] TxtContextDataPtr      - A pointer to an initialized TXT PEI Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.
**/
BOOLEAN
TxtIsEstablishmentBitAsserted (
  VOID
  )
{
  UINT8  Access;
  UINT16 TimeOutCount;

  if (IsSptPtt ()) {
    MmioRead8 (TPM_BASE_ADDRESS);
    return FALSE;
  }
  ///
  /// Set TPM.ACCESS polling timeout about 750ms
  ///
  TimeOutCount = TPM_TIME_OUT;
  do {
    ///
    /// Read TPM status register
    ///

    Access = MmioRead8 (TPM_BASE_ADDRESS);

    ///
    /// if TPM.Access == 0xFF, TPM is not present
    ///
    if (Access == 0xFF) {
      return FALSE;
    }
    ///
    /// Check tpmRegValidSts bit before checking establishment bit
    ///
    if ((Access & 0x80) != 0x80) {
      ///
      /// Delay 1ms
      ///
      MicroSecondDelay (1000);
    } else {
      ///
      /// tpmRegValidSts set, we can check establishment bit now.
      ///
      break;
    }

    TimeOutCount--;
  } while (TimeOutCount != 0);
  ///
  /// if tpmRegValidSts is not set, TPM is not usable
  ///
  if ((Access & 0x80) != 0x80) {
    DEBUG ((EFI_D_ERROR, "TPM Valid Status is not set!! TPM.ACCESS=%x\n", Access));
    ASSERT (TRUE);
    CpuDeadLoop ();
  }
  ///
  /// The bit we're interested in uses negative logic:
  /// If bit 0 == 1 then return False
  /// Else return True
  ///
  return (Access & 0x1) ? FALSE : TRUE;
}

BOOLEAN ChkIsTPM20NonInit()
{
    EFI_STATUS                      Status;
    TPM_DIGEST                      Pcr0Digest;    
    
    // Execute the TPM2.0 PCR Read, Since if the Startup is not execute
    // Than the PCR_Read must be error.
    Status = Tpm2PCRRead( 0, &Pcr0Digest );
    
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "[%d]: The TPM20 device did not execute Startup.\n", __LINE__));
        return TRUE;
    }
    
    return FALSE;
    
}

EFI_STATUS ChkExeTxtTpm20Init(
    CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS      Status;
    EFI_BOOT_MODE        BootMode;

    Status = (*PeiServices)->GetBootMode( PeiServices, &BootMode );
    if( EFI_ERROR(Status) )
        return EFI_PROTOCOL_ERROR;
    
    if( FALSE == ChkIsTPM20NonInit() )
        return EFI_SUCCESS;    

    // Issue the TPM2 Startup command
    if( BootMode == BOOT_ON_S3_RESUME)
    {
        Status = Tpm2Startup( TPM_SU_STATE );
    }
    else
    {
        Status = Tpm2Startup( TPM_SU_CLEAR );
    }
    
    if( EFI_ERROR(Status) )
    {
        DEBUG(( EFI_D_ERROR, "[%d]: Tpm might: (1): TPM 1.2, (2): TPM2 have already startup\n", __LINE__));
        return EFI_DEVICE_ERROR;
    }
    
    // Install the Skip TpmStartup    
    Status = (*PeiServices)->InstallPpi(PeiServices, &SkipTPMStartDesc[0]);
    ASSERT_EFI_ERROR (Status);
    if( EFI_ERROR(Status) )
    {
        DEBUG ((EFI_D_ERROR, "[AmiTxtTcgPeim.c][%d] Failed to install Skip TPM Startup\n", __LINE__));
        return EFI_PROTOCOL_ERROR;
    }
    return EFI_SUCCESS;
}

BOOLEAN ChkIsTPM12NonInit()
{
    EFI_STATUS                      Status;
    TPM_DIGEST                      Pcr0Digest;    
    
    // Execute the TPM2.0 PCR Read, Since if the Startup is not execute
    // Than the PCR_Read must be error.
    Status = Tpm12PCRRead( 0, &Pcr0Digest );
    
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "[%d]: The TPM12 devic did not execute Startup.\n", __LINE__));
        return TRUE;
    }
    
    return FALSE;
}

EFI_STATUS ChkExeTxtTpm12Init(
    CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS      Status;
    
    EFI_BOOT_MODE        BootMode;

    Status = (*PeiServices)->GetBootMode( PeiServices, &BootMode );
    if( EFI_ERROR(Status) )
        return Status;
    
    if( FALSE == ChkIsTPM12NonInit() )
        return EFI_SUCCESS;    

    // Issue the TPM12 Startup command
    Status = TpmCommStartup( BootMode );
    if( EFI_ERROR(Status) )
    {
        DEBUG(( EFI_D_ERROR, "[%d]: Tpm12 might have already startup or TPM is 2.0 device \n", __LINE__));
        return EFI_DEVICE_ERROR;
    }
    
    // Install the Skip TpmStartup    
    Status = (*PeiServices)->InstallPpi(PeiServices, &SkipTPMStartDesc[0]);
    ASSERT_EFI_ERROR (Status);
    if( EFI_ERROR(Status) )
    {
        DEBUG ((EFI_D_ERROR, "[AmiTxtTcgPeim.c][%d] Failed to install Skip TPM Startup\n", __LINE__));
        return EFI_PROTOCOL_ERROR;
    }
    return EFI_SUCCESS;
}

EFI_STATUS TxtTpmInitExeChk (
        IN EFI_PEI_SERVICES **PeiServices,
        IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
        IN VOID *InvokePpi)
{
    EFI_STATUS  Status;
    UINT8	SvnNumber = 0xFF;   // Default for SvnNumber is 0xFF;
    
    Status = ChkExeTxtTpm20Init( PeiServices );
    if( !EFI_ERROR(Status) )
    {
        // The Device is TPM20
    	TPM2B_MAX_BUFFER	Data1, *DataPtr1;

    	//We can't read TPM20_SGX_NVINDEX from TPM, because the index attribute is not TPMA_NV_PPREAD. Therefore BIOS can't read the index.
    	DataPtr1 = &Data1;
//    	Status = Tpm2NvRead (TPM_RH_PLATFORM, TPM20_SGX_NVINDEX, NULL, 8, 0, DataPtr1);
    	Status = Tpm2NvRead (TPM20_SGX_NVINDEX, TPM20_SGX_NVINDEX, NULL, 8, 0, DataPtr1);
		if ( EFI_ERROR (Status) ) {
		  DEBUG (( EFI_D_ERROR, "TXTSVN::Tpm 2.0 Reading SINIT SVN Number failed, Status = %r\n", Status));
		} else {
		  SGX_SINIT_DATA_FROM_TPM *SgxSinitDataFromTpm;
		  
		  SvnNumber = Data1.buffer[0];
		  
		  Status = (*PeiServices)->CreateHob(
								PeiServices,
								EFI_HOB_TYPE_GUID_EXTENSION,
								sizeof(SGX_SINIT_DATA_FROM_TPM),
								&SgxSinitDataFromTpm );
		  if( !EFI_ERROR(Status) ) {
			CopyMem(&SgxSinitDataFromTpm->EfiHobGuidType.Name, &gSgxSinitDataGuid, sizeof(EFI_GUID));                        
			SgxSinitDataFromTpm->SgxSinitDataFromTpm = SvnNumber;
			DEBUG (( EFI_D_INFO, "TXTSVN::Tpm 2.0 Reading SINIT SVN Number = %x\n", SvnNumber));
		  }
		}
        return EFI_SUCCESS;
    }
    
    Status = ChkExeTxtTpm12Init( PeiServices );
    
    if( !EFI_ERROR(Status) )
    {
    	// The Device is TPM12
    	UINT8	Data[20], *DataPtr;
    	UINTN	DataSize = 8;

    	DataPtr = &Data[0];
		Status = ReadTPMNVIdxData (TPM12_SGX_NVINDEX, &DataSize, DataPtr);
		if ( EFI_ERROR (Status) ) {
		  DEBUG (( EFI_D_ERROR, "TXTSVN::TPM 1.2 Reading SINIT SVN Number failed, Status = %r\n", Status));
		} else {
		  SGX_SINIT_DATA_FROM_TPM *SgxSinitDataFromTpm;
		  
		  SvnNumber = Data[0];
		  
		  Status = (*PeiServices)->CreateHob(
								PeiServices,
								EFI_HOB_TYPE_GUID_EXTENSION,
								sizeof(SGX_SINIT_DATA_FROM_TPM),
								&SgxSinitDataFromTpm );
		  if( !EFI_ERROR(Status) ) {
			CopyMem(&SgxSinitDataFromTpm->EfiHobGuidType.Name, &gSgxSinitDataGuid, sizeof(EFI_GUID));                        
			SgxSinitDataFromTpm->SgxSinitDataFromTpm = SvnNumber;
			DEBUG (( EFI_D_INFO, "TXTSVN::Tpm 1.2 Reading SINIT SVN Number = %x\n", SvnNumber));
		  }
		}
    }   
    return Status;
}

static EFI_PEI_NOTIFY_DESCRIPTOR  mBootModePpi[] =
{
        { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
          &gEfiPeiMasterBootModePpiGuid,//&gMasterBootModeGuid,
          TxtTpmInitExeChk
        },
};

EFI_STATUS
AmiTxtTcgPeimEntry(
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
    )
{
    EFI_STATUS      Status;    
    EFI_PEI_READ_ONLY_VARIABLE2_PPI	*ReadOnlyVariable2;
    UINTN       	VariableSize;
    SETUP_DATA 		SetupData;
    
    Status = (*PeiServices)->LocatePpi(
            PeiServices, &gPeiReadOnlyVariable2PpiGuid, 0, NULL, &ReadOnlyVariable2       
    );
    if (EFI_ERROR(Status)) return EFI_SUCCESS;
    
    VariableSize = sizeof(SETUP_DATA);
    
    Status = ReadOnlyVariable2->GetVariable(
				 ReadOnlyVariable2,
				 L"Setup",
				 &gEfiSetupGuid,
				 NULL,
				 &VariableSize,  
				 &SetupData
				 );
    if (EFI_ERROR(Status)) return EFI_SUCCESS;
    
    //If Txt is not enabled, don't need to run startup command.
    if (( SetupData.Txt == 0 ) || ( SetupData.TpmDeviceSelection == 1 )) {
    	if (!(TxtIsEstablishmentBitAsserted () && IsTxtWakeError ())) {
    		return EFI_SUCCESS;
    	}
    }

#if TCG_SUPPORT || TCG2Support
    if ( SetupData.TpmSupport == 0 ) {
       	return EFI_SUCCESS;
    }
#else
    return EFI_SUCCESS;
#endif

    if( FALSE == AuxChkIsTpmPresent() )
    {
        DEBUG(( EFI_D_ERROR, "[%d] No TPM Device Detected\n ", __LINE__));
        return EFI_SUCCESS;
    }

    Status = (*PeiServices)->NotifyPpi( PeiServices, mBootModePpi);
    
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
