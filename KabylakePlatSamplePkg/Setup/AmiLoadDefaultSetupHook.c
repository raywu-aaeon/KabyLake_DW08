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
// Name:  AmiLoadDefaultSetupHook.c
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
#include <SetupVariable.h>
#include "AmiTsePkg/Core/em/AMITSE/Inc/variable.h"
#include <MeSetup.h>
#include <AmiDxeLib.h>

extern BOOLEAN gBrowserCallbackEnabled;
EFI_GUID SetupVariableGuid = { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 }};

static SETUP_DATA            gOldSetup;
static ME_SETUP_STORAGE      MeStorageData;
static ME_SETUP_STORAGE      MeBackupStorage;

/**
    This function is a hook called when TSE determines
    that it has to load the boot options in the boot
    order. This function is available as ELINK.

    @param VOID

    @retval VOID

**/
VOID AmiProcessEnterSetup(VOID)
{
  EFI_STATUS            Status;
  UINTN                 VariableSize;

  DEBUG ((DEBUG_INFO, "AmiProcessEnterSetup Enter\n"));
  
  VariableSize = sizeof (SETUP_DATA);
  Status = pRS->GetVariable (
                  L"Setup",
                  &SetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &gOldSetup
                  );
} 

/**
    Callback function to load default config

    @param VOID

    @retval VOID

**/
VOID
AmiLoadDefaultSetupHook (
    VOID
)
{
    EFI_STATUS      Status;
    UINTN           VariableSize;
    UINT32          Index = 0;
    BOOLEAN         OrgBrowserCallbackEnabled;
    UINTN           DataSize;

    DEBUG ((DEBUG_INFO, "AmiLoadDefaultSetupHook Enter\n"));
    
    //AfterEoP is updated in MeExtractConfig, so we can't get the value at MEProcessEnterSetupHook
    VariableSize = sizeof (ME_SETUP_STORAGE);
    Status = pRS->GetVariable (
                    L"MeSetupStorage", 
                    &gMeSetupVariableGuid, 
                    NULL, 
                    &VariableSize, 
                    &MeBackupStorage);
    
    if (!EFI_ERROR (Status))
    {
        OrgBrowserCallbackEnabled = gBrowserCallbackEnabled;
        gBrowserCallbackEnabled = TRUE;
        
        DataSize = sizeof (ME_SETUP_STORAGE);
        
        Status = HiiLibGetBrowserData(&DataSize, \
                                      (VOID*) &MeStorageData, \
                                      &gMeSetupVariableGuid, \
                                      L"MeSetupStorage");
        if (EFI_ERROR(Status)) {      
            gBrowserCallbackEnabled = OrgBrowserCallbackEnabled;
        }
        else
        {
            MeStorageData.MngState = 1; //the default value is 1 in the MeSetup.hfr
            MeStorageData.RemoteSessionActive = MeBackupStorage.RemoteSessionActive;
//            MeStorageData.AmtGlobalState = MeBackupStorage.AmtGlobalState;
            MeStorageData.AfterEoP = MeBackupStorage.AfterEoP;
        
            HiiLibSetBrowserData(sizeof (ME_SETUP_STORAGE), \
                                 (VOID*) &MeStorageData, \
                                 &gMeSetupVariableGuid, \
                                 L"MeSetupStorage");
            gBrowserCallbackEnabled = OrgBrowserCallbackEnabled;
        }
    }
    
    // Update default value for AMT configuration setup
    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetup.MeImageType - (UINTN)&gOldSetup)), (UINTN)sizeof(UINT8), &gOldSetup.MeImageType );
    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetup.MeFirmwareInfo - (UINTN)&gOldSetup)), (UINTN)sizeof(UINT8), &gOldSetup.MeFirmwareInfo );    
    Status = VarSetValue(0, (UINT32)(((UINTN)&gOldSetup.UnconfigOnRtcAvailable - (UINTN)&gOldSetup)), (UINTN)sizeof(UINT8), &gOldSetup.UnconfigOnRtcAvailable );    

    DEBUG ((DEBUG_INFO, "AmiLoadDefaultSetupHook Exit\n"));
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
