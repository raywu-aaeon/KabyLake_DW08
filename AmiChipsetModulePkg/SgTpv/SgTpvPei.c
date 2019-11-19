//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SgTpvPEI.c
//
// Description:	This file contains the PEI SG Entry point SgPei_Init.
//              The function updates SG Policy settings 
//
// Notes:		MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
#include <Efi.h>
#include <Pei.h> 
#include <AmiPeiLib.h>

#include <SaAccess.h>
#include <Ppi/SiPolicy.h>
#include <Library/PchInfoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <ConfigBlock/SwitchableGraphicsConfig.h>
#include <SbGpioConfigDonePpi.h>
#include <SaPolicyCommon.h>

EFI_STATUS SgTpvPeiNotify(
  IN EFI_PEI_SERVICES **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID *NullPpi
  );

// PPI that are notified
static EFI_PEI_NOTIFY_DESCRIPTOR gNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
      &gAmiSbGpioConfigDonePpiGuid,
      SgTpvPeiNotify
  }
};

VOID
PcieGpioWrite(
  IN       UINT8                        GpioSupport,
  IN       UINT8                        Expander,
  IN       UINT32                       Gpio,
  IN       BOOLEAN                      Active,
  IN       BOOLEAN                      Level
)
{
  EFI_STATUS  Status;

  if (Active == 0) {
    Level = (~Level) & 0x1;
  }

  if (GpioSupport == PchGpio) {
    Status = GpioSetOutputValue(Gpio, (UINT32)Level);
      if (Status != EFI_SUCCESS) {
        return;
      }
  }
    ///
    ///
}

VOID
SgTpvPowerSequence (
  IN EFI_PEI_SERVICES          **PeiServices,		
  IN BOOLEAN                   SgTpvGpioSupport
)
{
   DEBUG ((DEBUG_INFO,  "SgTpv:: SgTpvPowerSequence Start.\n"));	
  ///
  ///  Drive DGPU HOLD RST Enable to make sure we hold reset
  ///
  PcieGpioWrite (SgTpvGpioSupport,
  	         EXPANDER_dGPU_HOLD_RST,
  	         GPIO_dGPU_HOLD_RST,
  	         ACTIVE_dGPU_HOLD_RST,
                 GP_ENABLE
                 );
  ///
  /// wait SG_DELAY_AFTER_HOLD_RESET ms
  ///
  MicroSecondDelay ((SG_DELAY_AFTER_HOLD_RESET) * STALL_ONE_MILLI_SECOND);
  ///
  /// Drive DGPU PWR EN to Power On MXM
  ///
  PcieGpioWrite (SgTpvGpioSupport,
  	         EXPANDER_dGPU_PWR_EN,
  	         GPIO_dGPU_PWR_EN,
  	         ACTIVE_dGPU_PWR_EN,
                 GP_ENABLE
                 );
#if defined (DGPU_MSTBY_EN_SUPPORT) && (DGPU_MSTBY_EN_SUPPORT == 1)  
  ///
  /// Drive DGFX_MSTBY_EN On
  ///
  PcieGpioWrite (SgTpvGpioSupport,
             EXPANDER_dGPU_MSTBY_EN,
             GPIO_dGPU_MSTBY_EN,
             ACTIVE_dGPU_MSTBY_EN,
                 GP_ENABLE
                 );
#endif  
  
  ///
  /// wait SG_DELAY_AFTER_POWER_ENABLE ms
  ///
  MicroSecondDelay ((SG_DELAY_AFTER_POWER_ENABLE) * STALL_ONE_MILLI_SECOND);
  ///
  /// Drive DGPU HOLD RST Disabled to remove reset
  ///
  PcieGpioWrite (SgTpvGpioSupport,
	         EXPANDER_dGPU_HOLD_RST,
	         GPIO_dGPU_HOLD_RST,
	         ACTIVE_dGPU_HOLD_RST,
                 GP_DISABLE
                 );
  ///
  /// wait SG_DELAY_AFTER_HOLD_RESET ms
  ///
  MicroSecondDelay ((SG_DELAY_AFTER_HOLD_RESET) * STALL_ONE_MILLI_SECOND);

}

EFI_STATUS SgTpvPeiNotify (
  IN EFI_PEI_SERVICES **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID *NullPpi
  )
{
  EFI_STATUS            Status;
  PCH_SERIES            PchSeries;
  SI_PREMEM_POLICY_PPI            *SiPreMemPolicyPpi;  
  SA_MISC_PEI_PREMEM_CONFIG       *MiscPeiPreMemConfig;  
	  
  DEBUG ((DEBUG_INFO, "SgTpv:: SgTpvPeiNotify Start.\n"));
	  
  PchSeries = GetPchSeries();
	 
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmiSbGpioConfigDonePpiGuid,
                             0,
                             NULL,
                             NULL
                             );
  if ( EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SgTpvPei::LocatePpi gAmiSbGpioConfigDonePpiGuid == %r\n",Status));
  } else {
    Status =  (*PeiServices)->LocatePpi  (
                                PeiServices,
                                &gSiPreMemPolicyPpiGuid,
                                0,
                                NULL,
                                (VOID **) &SiPreMemPolicyPpi
                                );
    ASSERT_EFI_ERROR(Status); 
    Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *)&MiscPeiPreMemConfig);
    ASSERT_EFI_ERROR(Status);    

    DEBUG ((DEBUG_INFO, "SgTpvPei::MiscPeiPreMemConfig->SgMode == %x\n",MiscPeiPreMemConfig->SgMode));
    if ((MiscPeiPreMemConfig->SgMode == SgModeMuxless) ||
        (MiscPeiPreMemConfig->SgMode == SgModeDgpu)) {
      DEBUG ((DEBUG_INFO, "SgTpvPei::Configure GPIOs for driving the dGPU.\n"));
      
      if (PchSeries == PchH) {
        if (SG_PCIE0_GPIO_SUPPORT) {
          SgTpvPowerSequence(PeiServices, SG_PCIE0_GPIO_SUPPORT);		      
        }else if (SG_PCIE1_GPIO_SUPPORT) {
          SgTpvPowerSequence(PeiServices, SG_PCIE1_GPIO_SUPPORT);		      
        }else if (SG_PCIE2_GPIO_SUPPORT) {
          SgTpvPowerSequence(PeiServices, SG_PCIE2_GPIO_SUPPORT);		      
        }
      }else if (PchSeries == PchLp){
        if (SG_PCIE0_GPIO_SUPPORT) {
          SgTpvPowerSequence(PeiServices, SG_PCIE0_GPIO_SUPPORT);		      
        }	      
      }
    }   
  }
  return EFI_SUCCESS;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SgTpvPeiInit
//
// Description: This function is the entry point for this PEI. 
//
// Input:		FfsHeader	Pointer to the FFS file header
//	    		PeiServices	Pointer to the PEI services table
//
// Output:      Return Status based on errors that occurred while waiting for
//              time to expire.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
SgTpvPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
  EFI_STATUS                            Status;

  DEBUG ((DEBUG_INFO, "SgTpv:: SgTpvPeiInit start.\n"));

  //  
  // Set up necessary PPI notifications
  //
  Status = (*PeiServices)->NotifyPpi(PeiServices, gNotifyList);
  ASSERT_EFI_ERROR ( Status );
  
  return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
