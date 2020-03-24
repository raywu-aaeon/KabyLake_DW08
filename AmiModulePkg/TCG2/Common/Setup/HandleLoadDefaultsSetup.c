//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TcgSetup/HandleLoadDefaultsSetup.c 1     11/22/11 6:41p Fredericko $
//
// $Revision: 1 $
//
// $Date: 11/22/11 6:41p $
//**********************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: TPMPwd.c
//
// Description:
// Contains functions that handle TPM authentication
//
//<AMI_FHDR_END>
//*************************************************************************

#include "token.h"
#include <EFI.h>
#include <Protocol/SimpleTextIn.h>
#include <Setup.h>
#include <Library/HiiLib.h>
#include "AmiTsePkg\Core\EM\AMITSE\Inc\Variable.h"
#include <Protocol\TcgPlatformSetupPolicy.h>

#if EFI_SPECIFICATION_VERSION>0x20000 && !defined(GUID_VARIABLE_DEFINITION)
#include "Include\UefiHii.h"
#include "Protocol/HiiDatabase.h"
#include "Protocol/HiiString.h"
#else
#include "Protocol/HII.h"
#endif

extern EFI_BOOT_SERVICES    *gBS;
extern EFI_SYSTEM_TABLE     *gST;
extern EFI_RUNTIME_SERVICES *gRT;


//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: TcgUpdateDefaultsHook
//
// Description: Updates TCG status on F3
//
// Input:       VOID
//
// Output:      BOOLEAN
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
VOID TcgUpdateDefaultsHook(VOID )
{
    EFI_STATUS  Status;
    SETUP_DATA  SetupData;
    TCG_PLATFORM_SETUP_PROTOCOL     *ProtocolInstance;
    EFI_GUID                        Policyguid = TCG_PLATFORM_SETUP_POLICY_GUID;

    //for OEMS that might want to update some policy on loaddefaults
    //they need to update the policy on load defaults before this function is run
    Status = gBS->LocateProtocol (&Policyguid,  NULL, &ProtocolInstance);
    if (EFI_ERROR (Status) || ProtocolInstance == NULL)
    {
        return;
    }

    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.Tpm20Device    - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.Tpm20Device );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmSupport    - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmSupport );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmEnable     - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmEnable );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmAuthenticate - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmAuthenticate );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmOperation    - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmOperation );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmHrdW         - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmHardware );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmEnaDisable   - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmEnaDisable );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmActDeact     - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmActDeact );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmOwnedUnowned - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmOwnedUnowned  );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TcgSupportEnabled - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TcgSupportEnabled );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TcmSupportEnabled - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TcmSupportEnabled );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.TpmError          - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.TpmError );
    Status = VarSetValue(0, (UINT32)(((UINTN)&SetupData.SuppressTcg  - (UINTN)&SetupData)), (UINTN)sizeof(UINT8), &ProtocolInstance->ConfigFlags.DisallowTpm );

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
