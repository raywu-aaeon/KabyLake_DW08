//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************
/** @file MeEsrtUpdate.c
 Functions for Windows firmware Update for Me stand along
**/
#include "MeEsrtUpdate.h"

EFI_CAPSULE_HEADER           *gWindowsScreenCapsule;
EFI_CAPSULE_HEADER           *gMeEsrtCapsule;
UINT32                       gMeVersion = 0;
EFI_GUID                     gMeFwUpdLocalGuid = ME_FW_UPD_LOCAL_PROTOCOL_GUID;
EFI_GUID                     gMeEsrtGuid       = ME_ESRT_DEVICE_GUID;
ME_FW_UPDATE_LOCAL_PROTOCOL  *gMeFwUpdLocal    = NULL;     

/**
  Get the Me version via MeFwUpdLocal
  
  @param VOID
  @retval VOID 

**/
VOID
EFIAPI
UpdateMeEsrtVersion(
    IN  VOID
)
{
    EFI_STATUS          Status;
    ME_FW_UPD_VERSION   MeFwUpdVersion;

    ZeroMem( &MeFwUpdVersion, sizeof(ME_FW_UPD_VERSION) );

    //Get ME current verison with FwUpdLocal protocol
    Status = gMeFwUpdLocal->MeFwGetVersion( gMeFwUpdLocal, &MeFwUpdVersion );
    if( !EFI_ERROR(Status) )
    {
        gMeVersion = ((MeFwUpdVersion.MajorVersion * 100000000) +
                      (MeFwUpdVersion.MinorVersion * 1000000) +
                      (MeFwUpdVersion.HotfixVersion * 10000) +
                      MeFwUpdVersion.BuildVersion
                     );
    }

    DEBUG((DEBUG_INFO, "MeEsrt: Me version: %x\n", gMeVersion));

    return;
}

/**
  Initial and install Me Esrt table 
  
  @param VOID
  @retval EFI_SUCCESS   Esrt table installed successfully
  @retval other         There is some error occurred during execution

**/
EFI_STATUS
EFIAPI
InstallMeEsrtTable(
    IN  VOID
)
{
    EFI_STATUS                  Status;
    ESRT_MANAGEMENT_PROTOCOL    *EsrtManagement;
    EFI_SYSTEM_RESOURCE_ENTRY   *Esrt;
    EFI_SYSTEM_RESOURCE_ENTRY   *TempEsrt;

    Status = gBS->LocateProtocol(
                    &gEsrtManagementProtocolGuid,
                    NULL,
                    (VOID **)&EsrtManagement );
    DEBUG((DEBUG_INFO, "MeEsrt: Locate EsrtManagement: %r\n", Status));
    if( !EFI_ERROR(Status) )
    {
        Esrt = NULL;
        TempEsrt = NULL;
        Esrt = AllocateZeroPool( sizeof(EFI_SYSTEM_RESOURCE_ENTRY) );
        if( Esrt == NULL )
            return EFI_BUFFER_TOO_SMALL;

       TempEsrt = AllocateZeroPool( sizeof(EFI_SYSTEM_RESOURCE_ENTRY) );
        if( TempEsrt == NULL )
            return EFI_BUFFER_TOO_SMALL;

        CopyGuid( &Esrt->FwClass, &gMeEsrtGuid );
        Esrt->FwType = ESRT_FW_TYPE_DEVICEFIRMWARE;
        Esrt->FwVersion = gMeVersion;
        Esrt->LowestSupportedFwVersion = gMeVersion;
        Esrt->CapsuleFlags = 0;
        Esrt->LastAttemptVersion = gMeVersion;
        Esrt->LastAttemptStatus = LAST_ATTEMPT_STATUS_SUCCESS;

        Status = EsrtManagement->GetEsrtEntry( &Esrt->FwClass, TempEsrt );
        if( !EFI_ERROR(Status) )
        {
            //If the Esrt already has the entry, compare it and update if there has difference
            if( CompareMem( TempEsrt, Esrt, sizeof(EFI_SYSTEM_RESOURCE_ENTRY) ) )
            {
                Status = EsrtManagement->UpdateEsrtEntry( Esrt );
                DEBUG((DEBUG_INFO, "MeEsrt: UpdateEsrtEntry: %r\n", Status));
            }
        }
        else
        {
            //Entry can not found, Register it
            Status = EsrtManagement->RegisterEsrtEntry( Esrt );
            DEBUG((DEBUG_INFO, "MeEsrt: RegisterEsrtEntry: %r\n", Status));
        }

        if( TempEsrt != NULL )
            gBS->FreePool( TempEsrt );
    }

    return Status;
}

/**
  Get Me buffer and image capsule from Hoblist
  
  @param VOID
  @retval EFI_SUCCESS   Find Me buffer and image capsule
  @retval EFI_NOT_FOUND Can't find

**/
EFI_STATUS
EFIAPI
FindMeEsrtCapsule(
    IN  VOID
)
{
    VOID                    *HobList;
    EFI_HOB_UEFI_CAPSULE    *CapsuleHob;
    EFI_CAPSULE_HEADER      *CapsuleHeader;
    EFI_GUID                WindowsScreenGuid = W8_SCREEN_IMAGE_CAPSULE_GUID;

    gWindowsScreenCapsule = NULL;
    gMeEsrtCapsule = NULL;

    HobList = NULL;
    HobList = GetFirstHob( EFI_HOB_TYPE_UEFI_CAPSULE );
    if( HobList == NULL )
        return EFI_NOT_FOUND;

    do
    {
        CapsuleHob = (EFI_HOB_UEFI_CAPSULE*)HobList;
        CapsuleHeader = (EFI_CAPSULE_HEADER*)(CapsuleHob->BaseAddress);

        if( CompareGuid( &CapsuleHeader->CapsuleGuid, &gMeEsrtGuid ) == TRUE )
        {
            gMeEsrtCapsule = CapsuleHeader;
        }

        if( CompareGuid( &CapsuleHeader->CapsuleGuid, &WindowsScreenGuid ) == TRUE )
        {
            gWindowsScreenCapsule = CapsuleHeader;
        }

        HobList = GET_NEXT_HOB(HobList);
        HobList = GetNextHob( EFI_HOB_TYPE_UEFI_CAPSULE, HobList );
    }while( HobList != NULL );

    if( gMeEsrtCapsule == NULL)
        return EFI_NOT_FOUND;

    return EFI_SUCCESS;
}

/**
  This function performs Windows firmware update for Me stand along
  
  @param This           Pointer to the EFI_REFLASH_PROTOCOL instance

  @retval EFI_SUCCESS   Firmware updated successfully
  @retval other         There is some error occurred during execution

**/
EFI_STATUS
EFIAPI
MeEsrtUpdate(
    IN  EFI_REFLASH_PROTOCOL    *This
)
{
    EFI_STATUS            Status;
    UINT8          *MeImageBuffer;
    UINTN          MeImageLength;

    Status = gBS->LocateProtocol(
                    &gMeFwUpdLocalGuid,
                    NULL,
                    (VOID **)&gMeFwUpdLocal );
    if( gMeFwUpdLocal == NULL )
        return EFI_NOT_FOUND;

    //Skip capsule header
    MeImageBuffer = (UINT8*)(((UINT8*)gMeEsrtCapsule) + gMeEsrtCapsule->HeaderSize);
    MeImageLength = gMeEsrtCapsule->CapsuleImageSize - gMeEsrtCapsule->HeaderSize;

    DEBUG((DEBUG_INFO, "MeEsrt: Update with FwUpdLocal protocol\n"));
    Status = gMeFwUpdLocal->FwUpdLcl(
                                gMeFwUpdLocal,
                                MeImageBuffer,
                                MeImageLength,
                                ME_FIRMWARE );

    DEBUG((DEBUG_INFO, "MeEsrt: Reset platform\n"));
    gMeFwUpdLocal->PlatformReset( gMeFwUpdLocal, EfiResetPlatformSpecific );

    return EFI_SUCCESS;
}

/**
  This function returns Windows firmware update image attributes
  
  @param This           Pointer to the EFI_REFLASH_PROTOCOL instance
  @param CoordinateX    Pointer where to store image left corner horizontal coordinate
  @param CoordinateY    Pointer where to store image left corner vertical coordinate
  @param ImageAddress   Pointer where to store pointer to image data

  @retval EFI_SUCCESS   Image returned successfully
  @retval other         There is some error occurred during execution

**/
EFI_STATUS
EFIAPI
MeEsrtGetImage(
    IN  EFI_REFLASH_PROTOCOL    *This,
    OUT UINTN                   *CoordinateX,
    OUT UINTN                   *CoordinateY,
    OUT VOID                    **ImageAddress
)
{
    W8_IMAGE_CAPSULE    *WindowsImageCapsule;

    if( gWindowsScreenCapsule == NULL )
        return EFI_NOT_FOUND;

    WindowsImageCapsule = (W8_IMAGE_CAPSULE*)gWindowsScreenCapsule;

    if( (CoordinateX == NULL) || (CoordinateY == NULL) || (ImageAddress == NULL) )
        return EFI_INVALID_PARAMETER;

    *CoordinateX = WindowsImageCapsule->ImageOffsetX;
    *CoordinateY = WindowsImageCapsule->ImageOffsetY;
    *ImageAddress = WindowsImageCapsule->Image;

    return EFI_SUCCESS;
}

/**
  This function install MeReflashProtocol for OS firmware update
  
  @param VOID

  @retval EFI_SUCCESS   Installed successfully
  @retval other         There is some error occurred during execution

**/
EFI_STATUS
EFIAPI
InstallMeReflashProtocol(
    IN  VOID
)
{
    EFI_REFLASH_PROTOCOL    *MeReflashProtocol;
    EFI_STATUS              Status;
    EFI_HANDLE              Handle;

    MeReflashProtocol = NULL;
    MeReflashProtocol = (EFI_REFLASH_PROTOCOL*)AllocatePool( sizeof(EFI_REFLASH_PROTOCOL) );
    if( MeReflashProtocol == NULL )
        return EFI_BUFFER_TOO_SMALL;

    MeReflashProtocol->CapUpdProgress = MeEsrtUpdate;
    MeReflashProtocol->GetDisplayImage = MeEsrtGetImage;

    Handle = NULL;
    Status = gBS->InstallProtocolInterface(
                    &Handle,
                    &gAmiReflashProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    MeReflashProtocol );

    DEBUG((DEBUG_INFO, "MeEsrt: InstallProtocolInterface:%r\n", Status));

    return Status;
}

/**
    This function is called when MeFwUpdLocal Protocol is installed

    @param Event signaled event
    @param Context calling context

    @return VOID

**/
VOID
EFIAPI
MeFwUpdLocalCallback(
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)
{
    EFI_STATUS                  Status;

    Status = gBS->LocateProtocol(
                    &gMeFwUpdLocalGuid,
                    NULL,
                    (VOID **)&gMeFwUpdLocal );
    if( !EFI_ERROR(Status) )
    {
        gBS->CloseEvent( Event );
        //Install Me Esrt entry
        UpdateMeEsrtVersion();
        Status = InstallMeEsrtTable();
        if( EFI_ERROR(Status) )
        {
            ASSERT_EFI_ERROR(Status);
            return;
        }
    }

    ASSERT_EFI_ERROR(Status);
    return;
}

/**
    The entry function of MeEsrtUpdate module

    @param ImageHandle The firmware allocated handle for the EFI image.
    @param SystemTable A pointer to the EFI System Table.

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
MeEsrtUpdateEntry(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS          Status;
    VOID                *MeFwUpdLocalReg = NULL;
    EFI_GUID            BootFlowGuid     = BOOT_FLOW_VARIABLE_GUID;
    UINT32              BootFlow         = BOOT_FLOW_CONDITION_OS_UPD_CAP;

    DEBUG((DEBUG_INFO, "MeEsrtUpdateEntry\n"));

    //If we can get the Me recovery Hob and Windows Image hob, then we are on OS FW Update path
    Status = FindMeEsrtCapsule();
    if( !EFI_ERROR(Status) )
    {
        //Install Reflash protocol for TSE and set boot flow
        Status = InstallMeReflashProtocol();
        if( !EFI_ERROR(Status) )
        {
            gRT->SetVariable(
                    L"BootFlow",
                    &BootFlowGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(UINT32),
                    &BootFlow );
        }
    }
    else
    {        
        //Locate FwUpdLocal protocol
        Status = gBS->LocateProtocol(
                        &gMeFwUpdLocalGuid,
                        NULL,
                        (VOID **)&gMeFwUpdLocal );
        if( !EFI_ERROR(Status) )
        {
            //Install Me Esrt entry
            UpdateMeEsrtVersion();
            Status = InstallMeEsrtTable();
            ASSERT_EFI_ERROR(Status);
        }
        else
        {
            //Register event
            Status = EfiNamedEventListen(
                        &gMeFwUpdLocalGuid,
                        TPL_CALLBACK,
                        MeFwUpdLocalCallback,
                        NULL,
                        &MeFwUpdLocalReg );
            ASSERT_EFI_ERROR(Status);
            DEBUG((DEBUG_INFO, "MeEsrt: Register event: %r\n", Status));
        }
    }

    return Status;
}
