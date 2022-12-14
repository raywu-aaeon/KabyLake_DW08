//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  S3BootScriptLib instance built on top of the S3SaveState protocol.
**/
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/S3SmmSaveState.h>

extern EFI_S3_SAVE_STATE_PROTOCOL *S3SaveState;

/**
  Library constructor. Called in Driver's entry point.
  @param ImageHandle     Image handle.
  @param SystemTable    Pointer to the EFI system table.

  @retval: EFI_SUCCESS
**/
EFI_STATUS EFIAPI SmmS3BootScriptLibS3SmmSaveStateProtocolConstructor(
    IN  EFI_HANDLE ImageHandle, IN  EFI_SYSTEM_TABLE *SystemTable
){
    EFI_STATUS Status;
    Status = gSmst->SmmLocateProtocol(&gEfiS3SmmSaveStateProtocolGuid, NULL, &S3SaveState);
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
