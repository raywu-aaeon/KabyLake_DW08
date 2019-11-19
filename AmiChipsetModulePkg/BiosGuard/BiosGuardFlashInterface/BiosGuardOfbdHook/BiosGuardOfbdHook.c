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
/** @file BiosGuardOfbdHook.c

**/
#include <Token.h>
#include <AmiDxeLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SecSmiFlash.h>
#include <Protocol/SmiFlash.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SmmSwDispatch.h>
#include <Ofbd.h>
#include <Protocol/AmiBiosGuardProtocol.h>

#define OFBD_FS_BGU             BIT22
#define OFBD_EXT_TC_BGU         0x72
#define OFBD_TC_SFU_CHECK_ME_STATE_DISABLE  BIT7

// GUIDs of consumed protocols
static EFI_GUID     gAmiBiosGuardSecSmiFlashProtocolGuid = AMI_BIOSGUARD_SEC_SMI_FLASH_PROTOCOL_GUID;

// Consumed protocols
AMI_BIOSGUARD_SEC_SMI_FLASH_PROTOCOL    *gAmiBiosGuardSecSmiFlash = NULL;

/**    
    

    @param VOID

    @retval VOID
**/
EFI_STATUS BiosGuardLoadFirmwareImage (
    VOID        *Data
)
{ 
    EFI_STATUS      Status;

    if (gAmiBiosGuardSecSmiFlash == NULL) return EFI_UNSUPPORTED;

    if (Data == NULL) return EFI_INVALID_PARAMETER;

    Status = gAmiBiosGuardSecSmiFlash->LoadBiosGuardFwImage((FUNC_BLOCK *)Data);
#if BIOS_GUARD_DEBUG_MODE    
    DEBUG((DEBUG_INFO, "LoadBiosGuardFwImage Status = %r\n", Status));
#endif    
    ((FUNC_BLOCK *)Data)->ErrorCode = (EFI_ERROR(Status) ? 1 : 0);

    return Status;
}

/**    
    

    @param VOID

    @retval VOID
**/
EFI_STATUS BiosGuardGetFlashUpdatePolicy (
    VOID        *Data
)
{
/*    
    EFI_STATUS                  Status;
    FLASH_POLICY_INFO_BLOCK     *pFlashPolicy = (FLASH_POLICY_INFO_BLOCK *)Data;

    if (NULL == SecSmiFlash) return EFI_UNSUPPORTED;
    
    if (Data == NULL) return EFI_INVALID_PARAMETER;

    Status = SecSmiFlash->GetFlUpdPolicy(pFlashPolicy);

    pFlashPolicy->ErrorCode = (EFI_ERROR(Status) ? 1 : 0);

    return Status;
*/    
    return EFI_SUCCESS;
}

/**    
    

    @param VOID

    @retval VOID
**/
EFI_STATUS BiosGuardSetFlashUpdateMethod (
    VOID        *Data
)
{    
    EFI_STATUS                  Status;
    FUNC_FLASH_SESSION_BLOCK    *pFlashSessionBlock = (FUNC_FLASH_SESSION_BLOCK *)Data;
    
    if (gAmiBiosGuardSecSmiFlash == NULL) return EFI_UNSUPPORTED;

    if (Data == NULL) return EFI_INVALID_PARAMETER;
    
    Status = gAmiBiosGuardSecSmiFlash->SetBiosGuardFlUpdMethod(pFlashSessionBlock);

    pFlashSessionBlock->ErrorCode = (EFI_ERROR(Status) ? 1 : 0);

    return Status;
}

/**    
    

    @param VOID

    @retval VOID
**/
EFI_STATUS BiosGuardCheckMeStateDiable (
    VOID        *Data
)
{
    EFI_STATUS                  Status;
    FUNC_FLASH_SESSION_BLOCK    *pFlashSessionBlock = (FUNC_FLASH_SESSION_BLOCK *)Data;
    
    if (gAmiBiosGuardSecSmiFlash == NULL) return EFI_UNSUPPORTED;

    if (Data == NULL) return EFI_INVALID_PARAMETER;

    Status = gAmiBiosGuardSecSmiFlash->CheckMeStateDisable();
    
    pFlashSessionBlock->ErrorCode = (EFI_ERROR(Status) ? 1 : 0);
    
    return Status;
}

/**    
    

    @param VOID

    @retval VOID
**/
EFI_STATUS 
EFIAPI
AmiBiosGuardSecureFlashProtocolCallback (
    CONST EFI_GUID      *Protocol,
    VOID                *Interface,
    EFI_HANDLE          Handle
)
{
    EFI_STATUS  Status;   
    
    Status = pSmst->SmmLocateProtocol(&gAmiBiosGuardSecSmiFlashProtocolGuid, NULL, &gAmiBiosGuardSecSmiFlash);
    DEBUG((DEBUG_INFO, "Locate gAmiBiosGuardSecSmiFlashProtocolGuid(%r)\n", Status));

    return Status;
}

/**    
    This function installs the BiosGuard Utility Protocol.

    @param VOID

    @retval VOID
**/
VOID BGUInSmm (
    VOID
)
{
    EFI_STATUS      Status;
#if BIOS_GUARD_DEBUG_MODE    
    DEBUG((DEBUG_INFO, "BGUInSmm enter\n"));
#endif    
    
    Status = pSmst->SmmLocateProtocol(&gAmiBiosGuardSecSmiFlashProtocolGuid, NULL, &gAmiBiosGuardSecSmiFlash);
#if BIOS_GUARD_DEBUG_MODE    
    DEBUG((DEBUG_INFO, "Locate gAmiBiosGuardSecSmiFlashProtocolGuid(%r)\n", Status));
#endif    
    if (EFI_ERROR(Status)) {
        VOID        *Registration;
        Status = pSmst->SmmRegisterProtocolNotify(
                            &gAmiBiosGuardSecSmiFlashProtocolGuid,
                            AmiBiosGuardSecureFlashProtocolCallback,
                            &Registration);
        ASSERT_EFI_ERROR(Status);
    }
}

/**    
    This function installs the BiosGuard Utility Protocol.

    @param VOID

    @retval VOID
**/
VOID BGUEntry (
    VOID        *Buffer,
    UINT8       *pOFBDDataHandled
)
{
    OFBD_HDR                    *pOFBDHdr;
    OFBD_EXT_HDR                *pOFBDExtHdr;
    VOID                        *pOFBDTblEnd;
    OFBD_TC_70_SFU_STRUCT       *ASFUStructPtr;
    EFI_STATUS                  Status = EFI_SUCCESS;
    
#if BIOS_GUARD_DEBUG_MODE
    DEBUG((DEBUG_INFO, "BGUEntry enter\n"));
#endif    

    if (Buffer == NULL) return;

    if (*pOFBDDataHandled == 0) {
        pOFBDHdr = (OFBD_HDR *)Buffer;
        pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
        ASFUStructPtr = (OFBD_TC_70_SFU_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR));
        pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));

        if (pOFBDHdr->OFBD_FS & OFBD_FS_BGU) {
            //Check Type Code ID
            if (pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_BGU) {
#if BIOS_GUARD_DEBUG_MODE                
                DEBUG((DEBUG_INFO, "ASFUStructPtr->Command = %x\n", ASFUStructPtr->Command));
#endif                                
                switch (ASFUStructPtr->Command) {
                    case OFBD_TC_SFU_LOAD_FIRMWARE_IMAGE :
                        Status = BiosGuardLoadFirmwareImage(pOFBDTblEnd);
                        break;
                    //case OFBD_TC_SFU_GET_FLASH_UPDATE_POLICY :
                    //    Status = BiosGuardGetFlashUpdatePolicy(pOFBDTblEnd);
                    //    break;
                    case OFBD_TC_SFU_SET_FLASH_UPDATE_METHOD :
                        Status = BiosGuardSetFlashUpdateMethod(pOFBDTblEnd);
                        break;
                    case OFBD_TC_SFU_CHECK_ME_STATE_DISABLE :
                        Status = BiosGuardCheckMeStateDiable(pOFBDTblEnd);
                        break;
                    default :
                        Status = EFI_UNSUPPORTED;
                        break;
                }
                if (EFI_ERROR(Status)) {
                    *pOFBDDataHandled = 0xFE;
                    ASFUStructPtr->Status = OFBD_TC_SFU_NOT_SUPPORTED;
                } else {
                    *pOFBDDataHandled = 0xFF;
                    ASFUStructPtr->Status = OFBD_TC_SFU_OK;
                }
            }
        }
    }
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
