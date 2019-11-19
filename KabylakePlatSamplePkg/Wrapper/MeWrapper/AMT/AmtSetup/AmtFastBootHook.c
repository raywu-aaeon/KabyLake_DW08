//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//**********************************************************************
/** @file AmtFastBootHook.c
    Implementation of IsiAmtBoot functionality.

**/
#include <Setup.h>
#include "bootflow.h"
#include "variable.h"
#include <Protocol/AmtWrapperProtocol.h>
#include "Protocol/AlertStandardFormat.h"
#include <Amt.h>

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;

//
// Please refer to FastBoot.c
//
EFI_STATUS 
ChangeSetupBootFlow (
  IN UINT32 BootFlow 
);

#if iME_SUPPORT
BOOLEAN 
EFIAPI
IsiAmtBoot(IN SETUP_DATA *FbSetupData)
{
    EFI_GUID EfiAmtWrapperProtocolGuidTse = EFI_AMT_WRAPPER_PROTOCOL_GUID;
    AMT_WRAPPER_PROTOCOL *pAmtWrapper = NULL;
    EFI_STATUS            Status;
    ALERT_STANDARD_FORMAT_PROTOCOL  *AsfCheck;
    ASF_BOOT_OPTIONS  *mInternalAsfBootOptions;

    //AMI_OVERRIDE_START >>> Fix cpp check error
	//if (pAmtWrapper == NULL) {
    Status = gBS->LocateProtocol(&EfiAmtWrapperProtocolGuidTse, NULL, (VOID**)&pAmtWrapper);
    //}
	//AMI_OVERRIDE_END <<< Fix cpp check error

    ///case IDER
    if (pAmtWrapper != NULL) {
        if (pAmtWrapper->ActiveManagementIsStorageRedirectionEnabled() || pAmtWrapper->ActiveManagementIsSolEnabled()){
            ChangeSetupBootFlow(BOOT_FLOW_CONDITION_NORMAL);
            return FALSE;   ///Is AMT boot, return FALSE for fast boot disabled.
        }
    }

    ///case ASF
    ///Get the ASF options
    ///if set then we have to do and Asfboot
    Status = gBS->LocateProtocol (
                    &gAlertStandardFormatProtocolGuid,
                    NULL,
                    (VOID**)&AsfCheck
                    );
    
    if (EFI_ERROR (Status)) {
        ///Is not AMT boot, return TRUE for fast boot enabled.
        return TRUE;
    }

    Status = AsfCheck->GetBootOptions (AsfCheck, &mInternalAsfBootOptions);
    if (EFI_ERROR (Status)) {
        return FALSE;
    }
    if (mInternalAsfBootOptions->SubCommand != ASF_BOOT_OPTIONS_PRESENT) {
        return TRUE;   ///Is not AMT boot, return TRUE for fast boot enabled.
    } else {
        ChangeSetupBootFlow(BOOT_FLOW_CONDITION_NORMAL);
        return FALSE;    ///Is AMT boot, return FALSE for fast boot disabled.
    }

}
#else

#define AMI_AMT_BOOT_OPTION_GUID \
{0x9ba25957, 0x21bf, 0x41d0, {0x81, 0xe7, 0xe7, 0xb6, 0xd8, 0x88, 0x2a, 0x49}}

/**
 * Do NOT perform FastBoot if AMT boot is request.
 *    
 * @param[in]   FbSetupData        Point of SetupData. 
 *
 * @retval TRUE                            Fast boot is enabled.
 * @retval FALSE                        Fast boot is not enabled.
 */

BOOLEAN IsiAmtBoot(IN SETUP_DATA *FbSetupData)
{
    EFI_GUID gAmtBootOptionGuid = AMI_AMT_BOOT_OPTION_GUID;
    EFI_STATUS Status;
    UINT16  AmtBootOption;
    UINTN   VariableSize;

    /// Get Device Type Variable of AMT Boot Option.
    VariableSize = sizeof(UINT16);

    Status = pRS->GetVariable ( L"AmtBootOption",
                                  &gAmtBootOptionGuid,
                                  NULL,
                                  &VariableSize,
                                  &AmtBootOption  );

    if (EFI_ERROR(Status))  {
///Is not AMT boot, return TRUE for fastboot enabled.
        return TRUE;
    } else {
///Is AMT boot, return FALSE for fastboot disabled.
        ChangeSetupBootFlow(BOOT_FLOW_CONDITION_NORMAL);
        return FALSE;
    }   
}                                
#endif    
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************