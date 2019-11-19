//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
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

#include <Token.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Library/DxeServicesLib.h>
#include <Library/DebugLib.h>

#include <SecureBoot.h>
#include <AmiRedFishApi.h>
#include <AmiSecureBootLib.h>

//----------------------------------------------------------------------------
// Function forward declaration
//----------------------------------------------------------------------------

// Delete the content of all UEFI Secure Boot key databases (PK, KEK, DB, DBX). This puts the system in Setup Mode
EFI_STATUS EFIAPI DeleteAllKeys (
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This
);
// Reset the content of all UEFI Secure Boot key databases (PK, KEK, DB, DBX) to their default values
EFI_STATUS EFIAPI ResetAllKeysToDefault (
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This
);
// Delete the content of the PK UEFI Secure Boot database. This puts the system in Setup Mode.
EFI_STATUS EFIAPI DeletePK (
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This
);
// Current Secure Boot Mode.
EFI_STATUS EFIAPI SecureBootMode(
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This,
    IN OUT UINT8 *SecureBootModeType
);
//Secure Boot state during the current boot cycle.
EFI_STATUS EFIAPI SecureBootCurrentBoot (
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This,
    IN OUT UINT8 *SecureBootState
);
// Enable or disable UEFI Secure Boot (takes effect on next boot).
// Setting this property to true enables UEFI Secure Boot, and 
// setting it to false disables it. This property can be enabled only in UEFI boot mode.
EFI_STATUS EFIAPI SecureBootEnable (
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This,
    IN BOOLEAN Enable
);    
//----------------------------------------------------------------------------
// Protocol Identifiers
//----------------------------------------------------------------------------
//EFI_GUID gAmiRedFishSecBootApiGuid = AMI_REDFISH_SECBOOT_PROTOCOL_GUID;

AMI_REDFISH_SECBOOT_PROTOCOL mRedfishSecbootAPI = {
  0x10040001,
  DeleteAllKeys,
  ResetAllKeysToDefault,
  DeletePK,
  SecureBootMode,
  SecureBootCurrentBoot,
  SecureBootEnable
};
//----------------------------------------------------------------------------

/**
  Provisioning of factory default Secure Boot policy Variables
  Install EFI Variables: PK, KEK, db, dbx, ...
  
  @param[in]  InstallVars Supported Types: RESET_NV_KEYS & SET_NV_DEFAULT_KEYS

  @retval     Status
**/
EFI_STATUS
InstallSecureVariables (
    UINT16    InstallVars
    )
{
    return AmiInstallSecureBootDefaults (InstallVars);
}

/**
  Enable or disable UEFI Secure Boot (takes effect on next boot).

  @param[in]  Enable 

  @retval     Status
**/
EFI_STATUS EFIAPI
SecureBootEnable(
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This,
    IN BOOLEAN Enable
    )
{
    EFI_STATUS  Status;
    UINTN       Size;
    UINT32      Attributes;
    SECURE_BOOT_SETUP_VAR SecureBootSetup;
    static EFI_GUID guidSecurity = SECURITY_FORM_SET_GUID;

    Size = sizeof(SECURE_BOOT_SETUP_VAR);
    Status = pRS->GetVariable (AMI_SECURE_BOOT_SETUP_VAR, &guidSecurity, &Attributes, &Size, &SecureBootSetup);
    if(EFI_ERROR(Status))
        return EFI_UNSUPPORTED;
    SecureBootSetup.SecureBootSupport = Enable;
    Status = pRS->SetVariable (AMI_SECURE_BOOT_SETUP_VAR, &guidSecurity, Attributes, Size, &SecureBootSetup);
//    DEBUG((SecureBoot_DEBUG_LEVEL,"Update %s Var %r\n", AMI_SECURE_BOOT_SETUP_VAR, Status));

    return Status;
}

/**
  Secure Boot state during the current boot cycle.

  @retval     SecureBootState
  @retval     Status
**/
EFI_STATUS EFIAPI
SecureBootCurrentBoot( 
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This,
    UINT8 *SecureBootState
    )
{
    UINTN       Size;
    if(SecureBootState==NULL)
        return EFI_INVALID_PARAMETER;

    Size = sizeof(UINT8);
    return pRS->GetVariable(EFI_SECURE_BOOT_NAME, &gEfiGlobalVariableGuid, NULL, &Size, SecureBootState);
}

/**
  Return Current Secure Boot Mode.

  SetupMode    0 Secure Boot is currently in Setup Mode
  UserMode     1 Secure Boot is currently in User Mode
  AuditMode    2 Secure Boot is currently in Audit Mode
  DeployedMode 3 Secure Boot is currently in Deployed Mode

  @retval     SecureBootModeType
  @retval     Status
**/
EFI_STATUS EFIAPI
SecureBootMode( 
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This,
    IN OUT UINT8 *SecureBootModeType
    )
{
    EFI_STATUS  Status;
    UINTN       Size;
    UINT8       SetupMode, AuditMode, DeployedMode;

    if(SecureBootModeType==NULL)
        return EFI_INVALID_PARAMETER;

    Size = sizeof(UINT8);
    Status = pRS->GetVariable(EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &SetupMode);
    if(EFI_ERROR(Status))
        return EFI_UNSUPPORTED;
    // User=1/Setup=0
    *SecureBootModeType = SetupMode?0:1;

    // Update Audit/Deployed Vars
    Size = sizeof(UINT8);
    if(EFI_ERROR(pRS->GetVariable(EFI_AUDIT_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &AuditMode)))
        return Status;
    Size = sizeof(UINT8);
    if(EFI_ERROR(pRS->GetVariable(EFI_DEPLOYED_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &DeployedMode)))
        return Status; // User/Setup

    if(AuditMode) {
        *SecureBootModeType = AmiRfAuditMode;
    }
    else
        if(DeployedMode) {
            *SecureBootModeType = AmiRfDeployedMode;
        }

    return Status;
}

/**
  Delete the content of the PK UEFI Secure Boot database. This puts the system in Setup Mode.

  @retval     Status
**/
EFI_STATUS EFIAPI
DeletePK(
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This
    )
{
    EFI_STATUS Status;
    // try to erase. should succeed if system in pre-boot and physical user authenticated mode
    Status = pRS->SetVariable(SecureVariableFileName[PkVar],&gEfiGlobalVariableGuid,0,0,NULL);
//    DEBUG((SecureBoot_DEBUG_LEVEL,"Del Var %s, Status %r\n",SecureVariableFileName[PkVar], Status));
    return Status;
}

/**
  Delete the content of all UEFI Secure Boot key databases (PK, KEK, DB, DBX,..). This puts the system in Setup Mode

  @retval     Status
**/
EFI_STATUS EFIAPI
DeleteAllKeys(
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This
    )
{
    return InstallSecureVariables(RESET_NV_KEYS);    // erase
}

/**
  Reset the content of all UEFI Secure Boot key databases (PK, KEK, DB, DBX,..) to their default values

  @retval     Status
**/
EFI_STATUS EFIAPI
ResetAllKeysToDefault (
    IN CONST AMI_REDFISH_SECBOOT_PROTOCOL *This
    )
{
    return InstallSecureVariables(RESET_NV_KEYS | SET_NV_DEFAULT_KEYS);    // erase+set
}

/**
  Entry point of RedFish Secure Boot DXE driver
  @param[in]  EFI_HANDLE 
  @param[in]  EFI_SYSTEM_TABLE 
  
  @retval     Status
**/
EFI_STATUS EFIAPI 
RedFishApi_Init (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
    )
{
    EFI_HANDLE Handle;

    InitAmiLib(ImageHandle, SystemTable);

    Handle = NULL;
    // 0-15 - Protocol version
    //      0- 7  Min version
    //      8-15  Major
    // 16-31 - compatible Redfish SecBoot resource spec version
    mRedfishSecbootAPI.Version = 0x01040001;
    return pBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gAmiRedFishSecBootApiGuid,
                    &mRedfishSecbootAPI,
                    NULL
                    );
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
