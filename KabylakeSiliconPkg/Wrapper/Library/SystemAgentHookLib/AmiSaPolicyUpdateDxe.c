//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Setup.h>
#include <NbSetupData.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SaPolicy.h>
#include <SaRegs.h>
#include <Library/IoLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

/**
    This Function is update IGD & USB Rmrr BaseAddress

    @param Event             A pointer to the Event that triggered the callback.
        Context           A pointer to private data registered with the callback function.

    @retval VOID
**/

EFI_STATUS
RmrrUpdateCallback (
  IN EFI_EVENT          Event,
  IN VOID               *Context
)
{
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmiUpdateDxeSaPolicy (
  IN OUT  VOID          *DxeSaPolicyPtr,
  IN VOID               *NbSetupDataPtr
  )
/*++
 
Routine Description:
 
  This function installs Dxe System Agent Platform Policy Protocol
  
Arguments:
 
  DxePlatformSaPolicyPtr          The SA Platform Policy protocol instance
 
Returns:
 
  EFI_SUCCESS                     Initialization complete.
  EFI_UNSUPPORTED                 The chipset is unsupported by this driver.
  EFI_OUT_OF_RESOURCES            Do not have enough resources to initialize the driver.
  EFI_DEVICE_ERROR                Device error, driver exits abnormally.
 
--*/
{
    EFI_STATUS              Status;	
	SA_POLICY_PROTOCOL		*DxeSaPolicy = NULL; 
#ifndef AMI_CRB_EC_SUPPORT_FLAG
    NB_SETUP_DATA           *NbSetupData = NULL;    
#endif
    MEMORY_DXE_CONFIG       *MemoryDxeConfig= NULL;    

    DEBUG ((EFI_D_INFO, "AmiUpdateDxeSaPolicy Start.\n"));
    
    DxeSaPolicy = (SA_POLICY_PROTOCOL *) DxeSaPolicyPtr;
#ifndef AMI_CRB_EC_SUPPORT_FLAG
    NbSetupData = (NB_SETUP_DATA*)NbSetupDataPtr;
#endif
    
    Status = GetConfigBlock ((VOID *)DxeSaPolicy, &gMemoryDxeConfigGuid, (VOID *)&MemoryDxeConfig);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);    
    }
    
    MemoryDxeConfig->ChannelASlotMap = 0x03;
    MemoryDxeConfig->ChannelBSlotMap = 0x03;
    
#if A1_MEMORY_SOCKETS == 2
    MemoryDxeConfig->ChannelASlotMap = 0x01;
    MemoryDxeConfig->ChannelBSlotMap = 0x01;    
#endif

#ifndef AMI_CRB_EC_SUPPORT_FLAG
    if(NbSetupData->SaSpdAddressInfo.IsOverrideSpdAddress) {
    	MemoryDxeConfig->SpdAddressTable[0] = NbSetupData->SaSpdAddressInfo.SpdAddressTable[0];
    	MemoryDxeConfig->SpdAddressTable[1] = NbSetupData->SaSpdAddressInfo.SpdAddressTable[1];
    	MemoryDxeConfig->SpdAddressTable[2] = NbSetupData->SaSpdAddressInfo.SpdAddressTable[2];
    	MemoryDxeConfig->SpdAddressTable[3] = NbSetupData->SaSpdAddressInfo.SpdAddressTable[3];
    }
#endif
    
    DEBUG ((EFI_D_INFO, "AmiUpdateDxeSaPolicy End.\n"));
    return EFI_SUCCESS;
}
