//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file MEFwUpdatePlatformLib.c

**/

//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <Token.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Library/DxeMeLib.h>
#include <Library/MeChipsetLib.h>
#include <Library/MbpDataLib.h>

#include <Protocol/HeciProtocol.h>
#include <Protocol/MeFwUpdLclProtocol.h>
#include <Protocol/MePlatformGetResetType.h>
#include <Protocol/PlatformMeHook.h>
#include <Protocol/Wdt.h>
#include <Protocol/PciRootBridgeIo.h>

#include <PchResetPlatformSpecific.h>

#include <AmiDxeLib.h>
#include "FWUpdateLib.h"
#include "errorlist.h"

//----------------------------------------------------------------------------
// Function Externs

//----------------------------------------------------------------------------
// Local prototypes
typedef struct _PCH_PLATFORM_POLICY  PRIVATE_PCH_PLATFORM_POLICY_PROTOCOL;

//----------------------------------------------------------------------------
// Local Variables
//PRIVATE_PCH_PLATFORM_POLICY_PROTOCOL    *gPchPlatformPolicy = NULL;
_UUID                       gOemId = ME_FW_OEM_ID;
EFI_BOOT_SERVICES           *BS;
AMI_POST_MANAGER_PROTOCOL   *gAmiPostMgr = NULL;

//----------------------------------------------------------------------------
// Function Definitions

/** The strncmp function compares not more than n characters (characters that
    follow a null character are not compared) from the array pointed to by s1
    to the array pointed to by s2.

    @return   The strncmp function returns an integer greater than, equal to,
              or less than zero, accordingly as the possibly null-terminated
              array pointed to by s1 is greater than, equal to, or less than
              the possibly null-terminated array pointed to by s2.
**/
int  strncmp(const char *s1, const char *s2, size_t n)
{
  return (int)AsciiStrnCmp( s1, s2, n);
}
/**
    This hook updates the BiosLock function of the PchPlatformPolicy to disabled.

    @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
    @param UnlockType Type to be set.

    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
EFIAPI
MeUpdPlatformUnlock(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT32                      UnlockType
)
{
    // ==== PORTING REQUEST ==== 
    // if (gPchPlatformPolicy != NULL) gPchPlatformPolicy->LockDownConfig.BiosLock = 0;
    // Patch disabling IDE_R if ME is Disabled for system assert if DEBUG_MODE is ON.
    // The IDE-R device will be disabled if ME is in Normal state (HeciInit.c), 
    // Here ME is in ME_MODE_SECOVER, the IDE-R is active and could cause assert error
    // in IdeBus.Start proceduce.
    // MeDeviceControl (IDER, Disabled);
    // ==== PORTING REQUEST ==== 
    return EFI_SUCCESS;
}
/**
    This hook is callback fucntion for PchPolicyProtocol.

    @param Event The EFI event
    @param Context Context for the callback

    @retval EFI_STATUS Return the EFI Stauts
**/
EFI_STATUS
EFIAPI
MeFwUpdatePchPolicyEvent(
    IN  EFI_EVENT   Event,
    IN  EFI_HANDLE  ImageHandle
)
{
    // EFI_STATUS          Status;
    // Status = pBS->LocateProtocol(&gPchPlatformPolicyProtocolGuid, \
    //                                             NULL, &gPchPlatformPolicy);
    // if (EFI_ERROR(Status)) return EFI_SUCCESS;
    // pBS->CloseEvent(Event);
    return EFI_SUCCESS;
}
/**
    This hook initial for the ME_FW_UPDATE_LOCAL_PROTOCOL.

    @param VOID

    @retval VOID

**/
VOID
MeFwUpdateLibInit(
    VOID
)
{
    // static EFI_EVENT    gEvtPchPolicy;
    // static VOID         *gRgnPchPolicy;
    // Create PchPlatformPolicy Callback for unlocking BIOS_LOCK. 
    // RegisterProtocolCallback ( &gPchPlatformPolicyProtocolGuid, \
    //                            MeFwUpdatePchPolicyEvent, \
    //                            NULL, \
    //                            &gEvtPchPolicy, \
    //                            &gRgnPchPolicy  );
    // MeFwUpdatePchPolicyEvent(gEvtPchPolicy, NULL);
    return;
}
/**
    This hook resets system via PchResetProtocol.

    @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
    @param ResetType Type to be reset.

    @retval EFI_SUCCESS Command succeeded

**/
EFI_STATUS
EFIAPI
MeUpdPlatformReset(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  EFI_RESET_TYPE              ResetType
)
{
    PCH_RESET_DATA  ResetData;

    CopyGuid( &ResetData.Guid, &gPchGlobalResetGuid );

    StrCpyS(
        ResetData.Description,
        PCH_RESET_DATA_STRING_MAX_LENGTH,
        PCH_PLATFORM_SPECIFIC_RESET_STRING );

    gRT->ResetSystem(
            EfiResetPlatformSpecific,
            EFI_SUCCESS,
            sizeof(PCH_RESET_DATA),
            &ResetData );

    return EFI_SUCCESS;
}
/**
    This hook checks the ME FW state.

    @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
    @param MeFwState ME FW state to be checked.

    @retval EFI_SUCCESS ME FW state match.
    @retval EFI_UNSUPPORTED ME FW state not match.

**/
EFI_STATUS
EFIAPI
MeCheckFwState(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  ME_FW_STATE                 MeFwState
)
{
    EFI_STATUS          Status;
    UINT32              MeMode;
    HECI_PROTOCOL       *Heci = NULL;

    Status = gBS->LocateProtocol( &gHeciProtocolGuid, NULL, (VOID**)&Heci );
    if( EFI_ERROR(Status) )
        return EFI_UNSUPPORTED;

    Status = Heci->GetMeMode( &MeMode );
    if( EFI_ERROR(Status) )
        return EFI_UNSUPPORTED;

    if( ((MeFwState == MeModeNormal) && (MeMode == ME_MODE_NORMAL)) || \
        ((MeFwState == MeModeDebug) && (MeMode == ME_MODE_DEBUG)) || \
        ((MeFwState == MeModeTempDisabled) && (MeMode == ME_MODE_TEMP_DISABLED)) || \
        ((MeFwState == MeModeSecOver) && (MeMode == ME_MODE_SECOVER)) || \
        ((MeFwState == MeModeFailed) && (MeMode == ME_MODE_FAILED)) )
        return EFI_SUCCESS;

    return EFI_UNSUPPORTED;
}
/**
    This hook gets the ME FW version from MeBiosPayloadData Protocol..

    @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.
    @param MeFwVersion ME FW Version returned. 

    @retval EFI_SUCCESS Get ME FW Version succeeded
**/
EFI_STATUS
EFIAPI
MeGetFwVersion(
    IN      ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  OUT ME_FW_UPD_VERSION           *MeFwVersion
)
{
    EFI_STATUS  Status;
    ME_CAP      MeCapability;    

    ZeroMem( &MeCapability, sizeof(ME_CAP) );

    // Get ME Firmware Version from MBP, then save for future used.
    Status = MbpGetMeFwInfo( &MeCapability );
    if( !EFI_ERROR(Status) )
    {
        MeFwVersion->MajorVersion  = MeCapability.MeMajorVer;
        MeFwVersion->MinorVersion  = MeCapability.MeMinorVer;
        MeFwVersion->HotfixVersion = MeCapability.MeHotFixNo;
        MeFwVersion->BuildVersion  = MeCapability.MeBuildNo;
    }    
    
    return Status;
}
/**
    HeciPdtUpdateMsg() is in both FWUpdateLib.lib and DxeMeLib. Clone the HeciHmrfpoEnable()
    in DxeMeLib for avoiding build error.

    @param[in] Nonce            Nonce received in previous HMRFPO_ENABLE Response Message
    @param[in] Result           HMRFPO_ENABLE response

    @retval EFI_UNSUPPORTED     Current ME mode doesn't support this function
    @retval EFI_SUCCESS         Command succeeded
    @retval EFI_DEVICE_ERROR    HECI Device error, command aborts abnormally
    @retval EFI_TIMEOUT         HECI does not return the buffer before timeout

**/
EFI_STATUS
HeciHmrfpoEnable(
    OUT UINT8   *Result
)
{
    EFI_STATUS              Status;
    HECI_PROTOCOL           *Heci;
    HMRFPO_ENABLE_BUFFER    HmrfpoEnable;
    UINT32                  Length;
    UINT32                  RecvLength;
    UINT32                  MeMode;

    Status = gBS->LocateProtocol(
                    &gHeciProtocolGuid,
                    NULL,
                    (VOID**)&Heci );
    if( EFI_ERROR(Status) )
    {
        return Status;
    }

    Status = Heci->GetMeMode( &MeMode );
    if( EFI_ERROR(Status) || (MeMode != ME_MODE_NORMAL) )
    {
        return EFI_UNSUPPORTED;
    }

    ZeroMem( &HmrfpoEnable, sizeof(HMRFPO_ENABLE_BUFFER) );
    HmrfpoEnable.Request.MkhiHeader.Data = 0;
    HmrfpoEnable.Request.MkhiHeader.Fields.GroupId = MKHI_HMRFPO_GROUP_ID;
    HmrfpoEnable.Request.MkhiHeader.Fields.Command = HMRFPO_ENABLE_CMD_ID;
    Length = sizeof (HMRFPO_ENABLE);
    RecvLength = sizeof (HMRFPO_ENABLE_ACK);

    Status = Heci->SendwAck(
                    HECI1_DEVICE,
                    (UINT32*)&HmrfpoEnable,
                    Length,
                    &RecvLength,
                    BIOS_FIXED_HOST_ADDR,
                    HECI_MKHI_MESSAGE_ADDR );

    if( !EFI_ERROR(Status) && (HmrfpoEnable.Response.MkhiHeader.Fields.Result == 0) )
    {
        *Result = HmrfpoEnable.Response.Status;
    }

    DEBUG((DEBUG_INFO, "Send HMRFPO_ENABLE_CMD_ID Result - %r\n", Status));
    return Status;
}
/**
    HeciPdtUpdateMsg() is in both FWUpdateLib.lib and DxeMeLib. Clone the MeIsAfterEndOfPost()
    in DxeMeLib for avoiding build error.

    @retval TRUE    End of post already happened
    @retval FALSE   End of post did not happen yet

**/
BOOLEAN
MeIsAfterEndOfPost(
    VOID
)
{
    EFI_STATUS              Status;
    EFI_PEI_HOB_POINTERS    HobPtr;
    BOOLEAN                 Done;
    VOID                    *Interface;

    Done = FALSE;

    HobPtr.Guid  = GetFirstGuidHob( &gMeEopDoneHobGuid );
    if( HobPtr.Guid != NULL )
    {
        Done = TRUE;
        DEBUG((EFI_D_INFO, "MeIsAfterEndOfPost in DXE, get from HOB\n"));
    }

    if( Done == FALSE )
    {
        Status = gBS->LocateProtocol( &gMeEopDoneProtocolGuid, NULL, (VOID**)&Interface );
        if( !EFI_ERROR(Status) )
        {
            Done = TRUE;
        }
        DEBUG((EFI_D_INFO, "MeIsAfterEndOfPost in DXE, LocateProtocol, Status: %r\n", Status));
    }

    DEBUG((EFI_D_INFO, "MeIsAfterEndOfPost in DXE, Done: %x\n", Done));
    return Done;
}
/**
    HeciPdtUpdateMsg() is in both FWUpdateLib.lib and DxeMeLib. Clone the HeciSendCbmResetRequest()
    in DxeMeLib for avoiding build error.

    @param[in] ResetOrigin      Reset source
    @param[in] ResetType        Global or Host reset

    @retval EFI_UNSUPPORTED     Current ME mode doesn't support this function
    @retval EFI_SUCCESS         Command succeeded
    @retval EFI_DEVICE_ERROR    HECI Device error, command aborts abnormally
    @retval EFI_TIMEOUT         HECI does not return the buffer before timeout

**/
EFI_STATUS
HeciSendCbmResetRequest(
    IN  UINT8   ResetOrigin,
    IN  UINT8   ResetType
)
{
    HECI_PROTOCOL               *Heci;
    EFI_STATUS                  Status;
    UINT32                      HeciLength;
    UINT32                      AckLength;
    CBM_RESET_REQ               CbmResetRequest;
    PLATFORM_ME_HOOK_PROTOCOL   *PlatformMeHook;
    WDT_PROTOCOL                *WdtProtocol;
    UINT32                      MeMode;
    UINT32                      Result;

    DEBUG((EFI_D_INFO, "ME-BIOS: ME: Global Reset Request Entry.\n"));

    if( MeIsAfterEndOfPost() )
    {
        DEBUG((EFI_D_ERROR, "ME-BIOS: ME: Global Reset Request Exit - Error by message is not allowed.\n"));
        return EFI_UNSUPPORTED;
    }

    Status = gBS->LocateProtocol(
                    &gPlatformMeHookProtocolGuid,
                    NULL,
                    (VOID**)&PlatformMeHook );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "Unable to Locate PlatformMeHook Protocol for Global Reset Hook, so skip instead.- %r\n", Status));
    }
    else
    {
        PlatformMeHook->PreGlobalReset();
    }

    Status = gBS->LocateProtocol (
                    &gHeciProtocolGuid,
                    NULL,
                    (VOID**)&Heci );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "ME-BIOS: ME: Global Reset Request Exit - Error by HECI service not found.\n"));
        return Status;
    }

    Status = Heci->GetMeMode( &MeMode );
    if( EFI_ERROR(Status) || (MeMode != ME_MODE_NORMAL) )
    {
        DEBUG((EFI_D_ERROR, "ME-BIOS: ME: Global Reset Request Exit - Error by message is not allowed.\n"));
        return EFI_UNSUPPORTED;
    }

    CbmResetRequest.MkhiHeader.Data             = 0;
    CbmResetRequest.MkhiHeader.Fields.GroupId   = MKHI_CBM_GROUP_ID;
    CbmResetRequest.MkhiHeader.Fields.Command   = CBM_RESET_CMD;
    CbmResetRequest.Data.RequestOrigin          = ResetOrigin;
    CbmResetRequest.Data.ResetType              = ResetType;
    HeciLength                                  = sizeof (CBM_RESET_REQ);
    AckLength                                   = sizeof (CBM_RESET_ACK);

    Status = gBS->LocateProtocol( &gWdtProtocolGuid, NULL, (VOID**)&WdtProtocol );
    ASSERT_EFI_ERROR(Status);
    WdtProtocol->AllowKnownReset ();

    Status = Heci->SendwAck(
                    HECI1_DEVICE,
                    (UINT32*)&CbmResetRequest,
                    HeciLength,
                    &AckLength,
                    BIOS_FIXED_HOST_ADDR,
                    HECI_MKHI_MESSAGE_ADDR );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "Unable to Send Reset Request - %r\n", Status));
        DEBUG((EFI_D_ERROR, "ME-BIOS: ME: Global Reset Request Exit - Error by message sent fail.\n"));
        return Status;
    }

    Result = ((CBM_RESET_ACK*)&CbmResetRequest)->MkhiHeader.Fields.Result;
    if( Result != 0)
    {
        DEBUG((EFI_D_ERROR, "ME-BIOS: ME: Global Reset Request Exit - Error by message ack error. Result: %x\n", Result));
        Status = EFI_DEVICE_ERROR;
    }
    else
    {
        DEBUG((EFI_D_INFO, "ME-BIOS: ME: Global Reset Request Exit - Success.\n"));
    }

    return Status;
}
/**
    This hook sends HMRFPO Enable MEI to disable ME/TXE FW for Firmware Update.

    @param This A pointer to the ME_FW_UPDATE_LOCAL_PROTOCOL instance.

    @retval EFI_SUCCESS Command succeeded
    @retval EFI_DEVICE_ERROR HECI Device error, command aborts abnormally
    @retval EFI_TIMEOUT HECI does not return the buffer before timeout

**/
EFI_STATUS
EFIAPI
MeUpdHmrfpoEnable(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This
)
{
    EFI_STATUS  Status;
    UINT8       HeciHmrfpoEnableResult;

    HeciHmrfpoEnableResult = HMRFPO_ENABLE_UNKNOWN_FAILURE;
    Status = HeciHmrfpoEnable( &HeciHmrfpoEnableResult );
    if( (Status == EFI_SUCCESS) && (HeciHmrfpoEnableResult == HMRFPO_ENABLE_SUCCESS) )
    {
        /// (A6) The BIOS sends the GLOBAL RESET MEI message
        HeciSendCbmResetRequest( CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET );
        MeUpdPlatformReset( This, EfiResetPlatformSpecific );
        CpuDeadLoop();
    }

    return Status;
}
/**
    This dummy hook for FWUpdateLib used only.

    @param VOID

    @retval void

**/
void
DbgPrint(
    VOID
)
{
}
/**
    This dummy hook for FWUpdateLib used only.

    @param VOID

    @retval void

**/
void
_fltused(
    VOID
)
{
}
/**
    This hook displays the progress status when ME Firmware update.

    @param BytesSent
    @param BytestobeSent

    @retval void

**/
void
DisplaySendStatus(
    UINT32   BytesSent,
    UINT32   BytestobeSent
)
{
    if( gAmiPostMgr != NULL )
    {
        UINT32              PercentWritten = (UINT32)(BytesSent * 100/BytestobeSent);
        static VOID         *DisplayHandle;
        static UINT8        BoxState = 0;
        AMI_POST_MGR_KEY    OutKey;

        if( !BoxState )
            BoxState = AMI_PROGRESS_BOX_INIT;
        else if( PercentWritten == 100 )
            BoxState = AMI_PROGRESS_BOX_CLOSE;
        else
            BoxState = AMI_PROGRESS_BOX_UPDATE;

        gAmiPostMgr->DisplayProgress(
                        BoxState,
                        L"ME FW update",
                        L"Loading New ME Firmware",
                        NULL,
                        PercentWritten,
                        &DisplayHandle,
                        &OutKey );

        if( PercentWritten == 100 )
            gAmiPostMgr = NULL;
    }
}
/**
    This hook displays the progress status when ISH Firmware update.

    @param BytesSent
    @param BytestobeSent

    @retval void

**/
void
DisplayIshSendStatus(
    UINT32     BytesSent,
    UINT32     BytestobeSent
)
{
    if( gAmiPostMgr != NULL )
    {
        UINT32              PercentWritten = (UINT32)(BytesSent * 100/BytestobeSent);
        static VOID         *DisplayHandle;
        static UINT8        BoxState = 0;
        AMI_POST_MGR_KEY    OutKey;

        if( !BoxState )
            BoxState = AMI_PROGRESS_BOX_INIT;
        else if( PercentWritten == 100 )
            BoxState = AMI_PROGRESS_BOX_CLOSE;
        else
            BoxState = AMI_PROGRESS_BOX_UPDATE;

        gAmiPostMgr->DisplayProgress(
                        BoxState,
                        L"ISH FW update",
                        L"Loading New ISH Firmware",
                        NULL,
                        PercentWritten,
                        &DisplayHandle,
                        &OutKey );
        
        if( PercentWritten == 100 )
            gAmiPostMgr = NULL;
    }
}
/**
    This hook update the ME/TXE Firmware via FWUpdateLib.

    @param FileBuffer buffer to be updated.
    @param FileLength length to be updated.

    @retval EFI_SUCCESS Command succeeded

**/
EFI_STATUS
EFIAPI
FwUpdLclFunc(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT8                       *FileBuffer,
    IN  UINTN                       FileLength,
    IN  UINT8                       FirmwareType
)
{
    UINTN               ImageLength = 0;
    UINT8               *ImageBuffer;
    UINT32              FwUpdateStatus = 0;
    UINT32              FwStatus;
    UINT16              EnabledState = FALSE;
    UINT32              CurrentPercent = 0;
    UINT32              NeededResetType = MFT_PART_INFO_EXT_UPDATE_ACTION_NONE;
    UINT32              Timer = 0;
    BOOLEAN             InProgress = FALSE;
    UINT32              PreviousPercent = 0;
    VOID                *DisplayHandle;
    AMI_POST_MGR_KEY    OutKey;

    ImageBuffer = FileBuffer;
    ImageLength = FileLength;

    BS = gBS;

    // Set ISH config file (PDT Data).
    if( FirmwareType == PDT_DATA_TYPE )
    {
        FwStatus = FwuSetIshConfig( ImageBuffer, (UINT32)ImageLength );
        if( This->AmiPostMgr != NULL )
        {
            EFI_EVENT Event;

            if( FwStatus == FW_SUCCESS )
            {
                This->AmiPostMgr->DisplayInfoBox(
                                    L"ISH config update",
                                    L"Flash New ISH config Completed.",
                                    1,
                                    &Event );
            }
            else
            {
                This->AmiPostMgr->DisplayInfoBox(
                                    L"ISH config update",
                                    L"Flash New ISH config Failed.",
                                    1,
                                    &Event );
                
                DEBUG((EFI_D_ERROR, "ISH config Failed :%x\n", FwStatus));
            }
        }
        
        return EFI_SUCCESS;
    }

    //
    // Is update supported? 
    //
    FwStatus = FwuEnabledState(&EnabledState);
    if( FW_SUCCESS != FwStatus )
    {
        return EFI_UNSUPPORTED;
    }

    switch( EnabledState )
    {
        case FW_UPDATE_DISABLED:
            DEBUG((EFI_D_ERROR, "FW Update is disabled. MEBX has options to disable / enable FW Update.\n"));
            return EFI_UNSUPPORTED;
        case FW_UPDATE_ENABLED:
            break;
        default:
            break;
    }

    // Initial "gAmiPostMgr" for loading progress.
    if( This->AmiPostMgr != NULL )
        gAmiPostMgr = This->AmiPostMgr;


    if( FirmwareType == ISH_FIRMWARE )
    {

        DEBUG((EFI_D_ERROR, "Executing ISHC Partial FW Update.\n\n"));
        DEBUG((EFI_D_ERROR, "Warning: Do not exit the process or power off the machine before the firmware update process ends.\n"));
        
        FwStatus = FwuPartialUpdateFromBuffer(ImageBuffer, (UINT32)ImageLength, FPT_PARTITION_NAME_ISHC, &DisplayIshSendStatus);
        
        if( FwStatus != FW_SUCCESS )
        {
            DEBUG((EFI_D_ERROR, "FwStatus: %x\n", FwStatus));
            return EFI_SUCCESS;
        }
    }
    else
    {
        FwStatus = FwuFullUpdateFromBuffer(
                         ImageBuffer,
                         (UINT32)ImageLength,
                         &gOemId,                    //Please make sure it same with FwUpdateOemId in the FIT tool.
                         &DisplaySendStatus
                         );
                
        
        if( FwStatus != FW_SUCCESS )
        {
            DEBUG((EFI_D_ERROR, "FwStatus: %x\n", FwStatus));
            return EFI_SUCCESS;
        }
    }

        if( This->AmiPostMgr != NULL )
        {
            if( FirmwareType == ISH_FIRMWARE )
            {
                This->AmiPostMgr->DisplayProgress(
                                    AMI_PROGRESS_BOX_INIT,
                                    L"ISH FW update", 
                                    L"Flash New ISH Firmware",
                                    NULL,
                                    0,
                                    &DisplayHandle,
                                    &OutKey );
            }
            else
            {
                This->AmiPostMgr->DisplayProgress(
                                    AMI_PROGRESS_BOX_INIT,
                                    L"ME FW update", 
                                    L"Flash New ME Firmware",
                                    NULL,
                                    0,
                                    &DisplayHandle,
                                    &OutKey );
            }
        }

        do
        {
            FwStatus = FwuCheckUpdateProgress(&InProgress,
                                              &CurrentPercent,
                                              &FwUpdateStatus,
                                              &NeededResetType);
            
            if (FwStatus != FW_SUCCESS) // update failed
            {
                break;
            }
            
			if( !InProgress ) // Update finished successfully
			{
				break;
			}

                DEBUG((EFI_D_ERROR, "FW Update:  %d\r" ,CurrentPercent));
                if( This->AmiPostMgr != NULL )
                {
                    if( FirmwareType == ISH_FIRMWARE )
                    {
                        This->AmiPostMgr->DisplayProgress(
                                            AMI_PROGRESS_BOX_UPDATE,
                                            L"ISH FW update",
                                            L"Flash New ISH Firmware",
                                            NULL,
                                            CurrentPercent,
                                            &DisplayHandle,
                                            &OutKey );
                    }
                    else
                    {
                        This->AmiPostMgr->DisplayProgress(
                                            AMI_PROGRESS_BOX_UPDATE,
                                            L"ME FW update",
                                            L"Flash New ME Firmware",
                                            NULL,
                                            CurrentPercent,
                                            &DisplayHandle,
                                            &OutKey );
                    }
                }


            gBS->Stall(250000); // Wait 250 milliseconds before polling again
			if( Timer >= 30000 ) 
			{
	            if (CurrentPercent == PreviousPercent) // If percent didn't change in 30 seconds
	            {
	                FwStatus = FWU_UPDATE_TIMEOUT;
	                break;
	            }
	            
	            // Percent changed
	            PreviousPercent = CurrentPercent;
	            Timer = 0;
			}
            else
			{
                Timer += 250;
			}
        }while(TRUE);

        if( This->AmiPostMgr != NULL )
        {
            if( FirmwareType == ISH_FIRMWARE )
            {
                This->AmiPostMgr->DisplayProgress(
                                    AMI_PROGRESS_BOX_CLOSE,
                                    L"ISH FW update",
                                    L"Flash New ISH Firmware",
                                    NULL,
                                    0,
                                    &DisplayHandle,
                                    &OutKey );
            }
            else
            {
                This->AmiPostMgr->DisplayProgress(
                                    AMI_PROGRESS_BOX_CLOSE,
                                    L"ME FW update",
                                    L"Flash New ME Firmware",
                                    NULL,
                                    0,
                                    &DisplayHandle,
                                    &OutKey );
            }
        }
    return EFI_SUCCESS;
}
