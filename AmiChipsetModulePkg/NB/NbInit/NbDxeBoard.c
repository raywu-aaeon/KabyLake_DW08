//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file NbDxeBoard.c
    This file contains DXE stage board component code for
    Template NB

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <NbSetupData.h>
#include <Protocol/NBMemInfo.h>
#include <Protocol/MpService.h>
#include <Protocol/SaPolicy.h>
#include <Protocol/SaGlobalNvsArea.h>

#if SMBIOS_SUPPORT
#include <Protocol/SmbiosDynamicData.h>
#endif

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

// Protocols that are installed

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)
SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL    *gSaGlobalNvsArea;

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)
VOID NbSaGlobalNvsAreaNotify (
    IN EFI_EVENT        Event,
    IN VOID             *Context 
);
//----------------------------------------------------------------------------

/**
    This function initializes the board specific component in
    in the chipset north bridge

    @param ImageHandle Image handle
        SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.
**/

EFI_STATUS 
NbDxeBoardInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_EVENT       NbSaGlobalNvsAreaEvent;
    VOID            *NbSaGlobalNvsAreaReg;

    Status = pBS->CreateEvent (
                    EFI_EVENT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    NbSaGlobalNvsAreaNotify,
                    NULL,
                    &NbSaGlobalNvsAreaEvent
                    );

    if (!EFI_ERROR (Status)) {
      Status = pBS->RegisterProtocolNotify (
                      &gSaGlobalNvsAreaProtocolGuid,
                      NbSaGlobalNvsAreaEvent,
                      &NbSaGlobalNvsAreaReg
                      );
    }
    return Status;
}

/**
    This callback function is called when a SaGlobalNvsArea Protocol is
    installed.

    @param Event Event of callback
    @param Context Context of callback.

    @retval EFI_SUCCESS
**/

VOID NbSaGlobalNvsAreaNotify (
    IN EFI_EVENT        Event,
    IN VOID             *Context)
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  NB_SETUP_DATA                         *NbSetupData = NULL; 
  UINTN                                 VariableSize = sizeof (NB_SETUP_DATA);

  Status = pBS->AllocatePool (EfiBootServicesData, VariableSize, (VOID **)&NbSetupData);
  ASSERT_EFI_ERROR(Status);

  GetNbSetupData (pRS, NbSetupData, FALSE);
  
  ///  Locate the SA Global NVS Protocol.
  Status = pBS->LocateProtocol (&gSaGlobalNvsAreaProtocolGuid, NULL, (VOID **)&gSaGlobalNvsArea);  
  if (!EFI_ERROR (Status)) 
  {
    // IGD GlobalNvs is updated in GraphicsInit.c

    // Update DeviceIds
    gSaGlobalNvsArea->Area->DeviceId1                   = DeviceID1;
    gSaGlobalNvsArea->Area->DeviceId2                   = DeviceID2;
    gSaGlobalNvsArea->Area->DeviceId3                   = DeviceID3;
    gSaGlobalNvsArea->Area->DeviceId4                   = DeviceID4;
    gSaGlobalNvsArea->Area->DeviceId5                   = DeviceID5;
    gSaGlobalNvsArea->Area->DeviceId6                   = DeviceID6;
    gSaGlobalNvsArea->Area->DeviceId7                   = DeviceID7;
    gSaGlobalNvsArea->Area->DeviceId8                   = DeviceID8;
    gSaGlobalNvsArea->Area->NumberOfValidDeviceId       = ValidDeviceIDs;
    gSaGlobalNvsArea->Area->CurrentDeviceList           = 0x0f;
//    gSaGlobalNvsArea->Area->PreviousDeviceList          = 0x0f;

    gSaGlobalNvsArea->Area->LidState       = 1;
    gSaGlobalNvsArea->Area->ActiveLFP       = NbSetupData->ActiveLFP;    
    // PEG LTR/OBFF is updated in PciExpressInit.c
  }
  
  if (NbSetupData != NULL) {
    pBS->FreePool (NbSetupData);
  }

  // Kill event
  pBS->CloseEvent(Event);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
