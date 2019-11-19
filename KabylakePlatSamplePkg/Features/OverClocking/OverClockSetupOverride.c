//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  OverClockSetupOverride.c
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/MePlatformGetResetType.h>
#include <SetupVariable.h>
#include "AmiTsePkg/Core/em/AMITSE/Inc/variable.h"

EFI_GUID gSetupVariableGuid = { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 }};

static SETUP_DATA            gNewSetupData;
static SETUP_DATA            gOldSetupData;

// SPD Profile
#define DEFAULT_SPD_PROFILE 0

ME_PLATFORM_GET_RESET_TYPE_PROTOCOL mOcPlatformGetResetType;

ME_SPECIAL_RESET_TYPES
OcPlatformReset (
  IN  EFI_RESET_TYPE                  PlatformResetType
  )
{
  if ((gNewSetupData.DdrRefClk != gOldSetupData.DdrRefClk) ||
      (gNewSetupData.SpdProfileSelected != gOldSetupData.SpdProfileSelected) ||
      (gNewSetupData.CoreMaxOcRatio != gOldSetupData.CoreMaxOcRatio)
      ){
    DEBUG ((DEBUG_INFO,"OcSetupReset: ME Power Cycle Reset.\n"));
    return PowerCycleResetReq;
  }
  
  return (ME_SPECIAL_RESET_TYPES)PlatformResetType;
}

/**
    This function is a hook called when TSE determines
    that it has to load the boot options in the boot
    order. This function is available as ELINK.

    @param VOID

    @retval VOID

**/
VOID OcProcessEnterSetup(VOID)
{
  EFI_STATUS            Status;
  UINTN                 VariableSize;

  VariableSize = sizeof (SETUP_DATA);
  Status = pRS->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &gOldSetupData
                  );
} 

/**
    This function is a hook called after some control 
    modified in the setup utility by user. This
    function is available as ELINK.

    @param VOID

    @retval VOID

**/
VOID OcSetupResetHook(VOID)
{
  EFI_STATUS            Status;
  UINTN                 VariableSize;
  EFI_HANDLE            PlatformResetHandle; 
  UINT32                VariableAttr;
  
  VariableSize = sizeof (SETUP_DATA);
  Status = pRS->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  &VariableAttr,
                  &VariableSize,
                  &gNewSetupData
                  );
  
  if (!EFI_ERROR (Status)) {
	 PlatformResetHandle = NULL;
	 mOcPlatformGetResetType.Revision       = ME_PLATFORM_GET_RESET_TYPE_PROTOCOL_REVISION;
	 mOcPlatformGetResetType.MeGetResetType = OcPlatformReset;
    
	 Status = pBS->InstallMultipleProtocolInterfaces (
                    &PlatformResetHandle,
                    &gMePlatformGetResetTypeGuid,  
		            &mOcPlatformGetResetType,
                    NULL
                    );
	 ASSERT_EFI_ERROR (Status);
  }
}

/**
    Callback function to load default config

    @param VOID

    @retval VOID

**/
VOID
LoadOverClockHwDefault (
    VOID
)
{
    EFI_STATUS      Status;
    UINT32			temp;
    
    DEBUG ((DEBUG_INFO, "LoadOverClockHwDefault Enter\n"));
    if (gOldSetupData.OverclockingSupport == 1) {
    	Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.CpuRatio    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.FlexRatioOverrideDefault );
  	    if (gOldSetupData.Ck505Present) {
  	    	Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.ProcessorBusSpeedOverride    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.ProcessorBusSpeedOverrideDefault );
  	    }
 	    temp = gOldSetupData.PowerLimit1Default;
  	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.PowerLimit1    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT32), &temp );
  	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.PowerLimit1Time    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.PowerLimit1TimeDefault );
  	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.XeTdcLimit    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT16), &gOldSetupData.XeTdcLimitDefault );
  	    temp = gOldSetupData.PowerLimit2Default;
   	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.PowerLimit2Power    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT32), &temp );
   	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.RatioLimit1    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.RatioLimit1Default );
   	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.RatioLimit2    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.RatioLimit2Default );
   	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.RatioLimit3    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.RatioLimit3Default );
   	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.RatioLimit4    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.RatioLimit4Default );  
        Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.RatioLimit5    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.RatioLimit5Default );
   	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.RatioLimit6    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.RatioLimit6Default );
   	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.RatioLimit7    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.RatioLimit7Default );
   	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.RatioLimit8    - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.RatioLimit8Default );  
    }
    if (gOldSetupData.XmpProfileEnable > DEFAULT_SPD_PROFILE) { //Check if there is XMP profiles of memory 
	    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetupData.XmpProfileEnable  - (UINTN)&gOldSetupData)), (UINTN)sizeof(UINT8), &gOldSetupData.XmpProfileEnable );
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
