//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
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

/** @file PttWrapper.c
    Setup Hooks for Tdt.

**/
#include <Efi.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmVariable.h>


#define EFI_SMM_RUNTIME_SERVICES_TABLE_GUID \
    { 0x395c33fe, 0x287f, 0x413e, { 0xa0, 0x55, 0x80, 0x88, 0xc0, 0xe1, 0xd4, 0x3e } }



EFI_SMM_BASE2_PROTOCOL       *pSmmBase;
EFI_SMM_SYSTEM_TABLE2       *mSmst;
EFI_RUNTIME_SERVICES        *ptrRuntimeServices = NULL;
EFI_SMM_VARIABLE_PROTOCOL   SmmVariableProtocol;
VOID* GetSmstConfigurationTableFramework(IN EFI_GUID *TableGuid);

/**
    InSmmFunction

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Status 

**/
EFI_STATUS
InSmmFunction 
(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable 
)
{
    EFI_STATUS   Status;
    EFI_HANDLE   mSmmHandle = NULL;
    EFI_GUID     gEfiSmmVariableProtocolGuid = EFI_SMM_VARIABLE_PROTOCOL_GUID;
    EFI_GUID     SmmRtServTableGuid  = EFI_SMM_RUNTIME_SERVICES_TABLE_GUID; 

    ptrRuntimeServices = (EFI_RUNTIME_SERVICES *)GetSmstConfigurationTable(&SmmRtServTableGuid);
    if(ptrRuntimeServices == NULL) return EFI_NOT_FOUND;

    Status = pBS->LocateProtocol( &gEfiSmmBase2ProtocolGuid, NULL, &pSmmBase );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  
    Status = pSmmBase->GetSmstLocation( pSmmBase, &mSmst );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    
    SmmVariableProtocol.SmmGetVariable = ptrRuntimeServices->GetVariable;
    SmmVariableProtocol.SmmGetNextVariableName = ptrRuntimeServices->GetNextVariableName;
    SmmVariableProtocol.SmmSetVariable = ptrRuntimeServices->SetVariable;
    SmmVariableProtocol.SmmQueryVariableInfo = ptrRuntimeServices->QueryVariableInfo;

    Status = mSmst->SmmInstallProtocolInterface(
                       &mSmmHandle,
                      &gEfiSmmVariableProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &SmmVariableProtocol);
    if (EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }
    
    return EFI_SUCCESS;
}

/**
    Driver EntryPoint

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Status

**/
EFI_STATUS
EFIAPI
PttWrapperEntryPoint (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    InitAmiLib(ImageHandle, SystemTable);
    InitSmmHandlerEx(ImageHandle, SystemTable, InSmmFunction, NULL);

    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
