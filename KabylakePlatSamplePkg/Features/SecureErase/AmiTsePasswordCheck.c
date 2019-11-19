//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
#include "Token.h"
#include <Efi.h>
#include "PwdLib.h"

VOID *SavePostScreen( UINTN *SizeOfX, UINTN *SizeOfY );
void SaveActiveBuffer(void **TempScreenBuffer);
void RestoreActiveBuffer(void **TempScreenBuffer);
VOID RestorePostScreen( VOID *UgaBlt, UINTN SizeOfX, UINTN SizeOfY );

extern EFI_BOOT_SERVICES *gBS;

#if OVERRIDE_TSEIDEPasswordCheck
/**
    Hook function for the IDE Password fuctionality based on
    the security token SETUP_IDE_SECURITY_SUPPORT 

    @param VOID

    @retval VOID

**/
VOID TSEIDEPasswordCheck()
{
	VOID *UgaBlt = NULL;
	UINTN SizeOfX=0, SizeOfY=0;
	VOID *TempScreenBuffer = (VOID*)NULL;
    EFI_STATUS Status;
    HDD_PASSWORD_EXTERNAL_CONTROL_PROTOCOL *ExternalControl;    

	//
	// Look for EFI_HDD_PASSWORD_EXTERNAL_CONTROL_PROTOCOL, if it is present in the system omit normal
	// password handling mechanism.
	//
	Status = gBS->LocateProtocol (&gHddPasswordExternalControlProtocolGuid, NULL, (VOID **) &ExternalControl);
	if (!EFI_ERROR (Status)) {
	  return ;
	}
	  
#if defined SECURITY_SUPPORT && SECURITY_SUPPORT == 1
	UgaBlt = SavePostScreen(&SizeOfX, &SizeOfY);
	//Save the contents of Active buffer so password code can use it
	SaveActiveBuffer(&TempScreenBuffer);
	IDEPasswordCheck();
	//Restore the original active buffer contents
	RestoreActiveBuffer(&TempScreenBuffer);
	RestorePostScreen( UgaBlt, SizeOfX, SizeOfY);
#endif
}
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
