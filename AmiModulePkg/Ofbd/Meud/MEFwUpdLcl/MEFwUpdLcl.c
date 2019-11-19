//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file MEFwUpdLcl.c
**/

//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/MeFwUpdLclProtocol.h>

//----------------------------------------------------------------------------
// Function Externs
VOID
MeFwUpdateLibInit(
    VOID
);

EFI_STATUS
EFIAPI
MeUpdPlatformUnlock(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT32                      UnlockType
);

EFI_STATUS
EFIAPI
MeUpdPlatformReset(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  EFI_RESET_TYPE              ResetType
);

EFI_STATUS
EFIAPI
MeCheckFwState(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  ME_FW_STATE                 MeFwState
);

EFI_STATUS
EFIAPI
MeGetFwVersion(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  ME_FW_UPD_VERSION           *MeFwVersion
);

EFI_STATUS
EFIAPI
MeUpdHmrfpoEnable(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This
);

EFI_STATUS
EFIAPI
FwUpdLclFunc (
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT8                       *FileBuffer,
    IN  UINTN                       FileLength,
    IN  UINT8                       FiremwareType
);

//----------------------------------------------------------------------------
// Local prototypes

//----------------------------------------------------------------------------
// Local Variables

//----------------------------------------------------------------------------
// Function Definitions
/**
 * This hook is Module DXE driver entry.
 * @param ImageHandle The handle associated with this image being loaded into memory
 * @param SystemTable Pointer to the system table
**/
EFI_STATUS
EFIAPI
MeFwUpdLclEntry(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS                   Status;
    EFI_HANDLE                   Handle = NULL;
    ME_FW_UPDATE_LOCAL_PROTOCOL  *MeFwUpdateLclProtocol = NULL;
    EFI_GUID                     MeFwUpdLclProtocolGuid = ME_FW_UPD_LOCAL_PROTOCOL_GUID;

    InitAmiLib( ImageHandle, SystemTable );
    //Prepare ME firmware update local protocol
    Status = pBS->AllocatePool( EfiBootServicesData, \
                  sizeof(ME_FW_UPDATE_LOCAL_PROTOCOL), (VOID**)&MeFwUpdateLclProtocol );
    if( EFI_ERROR(Status) ) return Status;

    MeFwUpdateLibInit();
    MeFwUpdateLclProtocol->FwUpdBufferAddress  = 0;
    MeFwUpdateLclProtocol->FwUpdBufferLength   = 0;
    MeFwUpdateLclProtocol->FwUpdLcl            = FwUpdLclFunc;
    MeFwUpdateLclProtocol->MeFwCheckMode       = MeCheckFwState;
    MeFwUpdateLclProtocol->MeFwGetVersion      = MeGetFwVersion;
    MeFwUpdateLclProtocol->HmrfpoEnable        = MeUpdHmrfpoEnable;
    MeFwUpdateLclProtocol->PlatformReset       = MeUpdPlatformReset;
    MeFwUpdateLclProtocol->PlatformUnlock      = MeUpdPlatformUnlock;
    MeFwUpdateLclProtocol->AmiPostMgr          = NULL;

    Status = pBS->InstallProtocolInterface( 
                    &Handle,
                    &MeFwUpdLclProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    MeFwUpdateLclProtocol );

    return Status;
}
