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

/** @file AmtCompatiblity.c
    Amt Wrapper Define file

**/
#include "Efi.h"

#include <Protocol/AmtCompatiblity.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

EFI_GUID gMbpWrapperProtocolGuid = MBP_WRAP_PROTOCOL_GUID;

EFI_STATUS
EFIAPI
AmtCompatiblityEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
    ME_BIOS_PAYLOAD_HOB    *MbpHob;
    EFI_HANDLE             Handle = NULL;
    EFI_STATUS          Status;
    
    //
    // Get Mbp Protocol
    //
    MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
    
    if (MbpHob != NULL) {

        Status = gBS->InstallMultipleProtocolInterfaces (
                        &Handle,
                        &gMbpWrapperProtocolGuid,
                        &MbpHob->MeBiosPayload,
                        NULL
                        );
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "MBP data protocol install failed, Status is %r \n", Status));
        }     
    }
return EFI_SUCCESS;
}
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
