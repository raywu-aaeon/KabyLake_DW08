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
// $Header: /Alaska/Projects/Intel/SandyBridge/CougarPoint_SugarBay_Crb_1AOQI/CRB/Setup/SBSetup.c 3     6/04/10 6:16a Tonywu $
//
// $Revision: 3 $
//
// $Date: 6/04/10 6:16a $
//**********************************************************************
/** @file SbSetup.c
    South Bridge Setup Routines

**/
//**********************************************************************
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <SetupVariable.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
//
// Print primitives
//
#define LEFT_JUSTIFY  0x01
#define PREFIX_SIGN   0x02
#define PREFIX_BLANK  0x04
#define COMMA_TYPE    0x08
#define LONG_TYPE     0x10
#define PREFIX_ZERO   0x20

/**

    @param HiiHandle IN UINT16 Class

    @retval VOID

**/
VOID InitSbStrings(IN EFI_HII_HANDLE HiiHandle, IN UINT16 Class)
{

}


EFI_STATUS
EFIAPI
SbSetupCallback (
    IN EFI_HII_HANDLE HiiHandle, 
    IN UINT16         Class, 
    IN UINT16         SubClass, 
    IN UINT16         Key
)
{
    EFI_STATUS              Status;  
    EFI_GUID                SetupGuid = SETUP_GUID;
    SETUP_DATA              *SetupData = NULL;
    UINTN                   BufferSize= sizeof(SETUP_DATA);
    CALLBACK_PARAMETERS     *Callback;
  
    Callback = GetCallbackParameters();
    if (!Callback || Callback->Action != EFI_BROWSER_ACTION_CHANGING)
        return EFI_UNSUPPORTED;
    
    SetupData = AllocatePool(BufferSize);

    Status = HiiGetBrowserData(
               &SetupGuid, L"Setup",
               BufferSize, (UINT8*) SetupData
               );
    if (EFI_ERROR(Status)) return Status;
    
    switch (Key) {
        case SB_CRID_KEY:
           // Sync up value of PchEnableCrid to EnableNbCrid.
           SetupData->CridEnable = Callback->Value->u8;
           break;     
    }

    Status = HiiSetBrowserData(
               &SetupGuid, L"Setup", 
               BufferSize, (UINT8 *) SetupData, NULL
               );
    ASSERT_EFI_ERROR(Status);   

    FreePool(SetupData);

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
