
#include <token.h>
#include <AmiDxeLib.h> // Optional. should use Mde Library instead.
#include <AmiCspLib.h> // Optional. should use Mde Library instead.
#include <Setup.h> // Optional. should use Mde Library instead.

#include "AaeonUartMode2.h"

// Produced Protocols

// Consumed Protocols

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------
EFI_STATUS AaeonUartMode2CallbackEntry(EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key)
{
	CALLBACK_PARAMETERS	*Callback;
	//EFI_BROWSER_ACTION_REQUEST	*CallbackActionRequest;
	
	Callback = GetCallbackParameters();
        //CallbackActionRequest = Callback->ActionRequest;

	if(!Callback)
		return EFI_SUCCESS;
	//TRACE((-1,"Callback->Action=%x\n",  Callback->Action));

	if (Callback->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD)
		return EFI_UNSUPPORTED; 
    
	if (Callback->Action == EFI_BROWSER_ACTION_FORM_OPEN || Callback->Action == EFI_BROWSER_ACTION_CHANGING)
		return EFI_SUCCESS;

	if (Callback->Action == EFI_BROWSER_ACTION_FORM_CLOSE)
	{
		EFI_STATUS 			Status = EFI_NOT_FOUND;
    	EFI_GUID			SetupGuid = SETUP_GUID;
    	SETUP_DATA			SetupData;
    	UINTN				VariableSize = sizeof(SetupData);
	
		EFI_HANDLE			*Handle = NULL;
		UINTN				Count, i;
    	EFI_GUID			gEfiAmiSioProtocolGuid = EFI_AMI_SIO_PROTOCOL_GUID;
		AMI_SIO_PROTOCOL    *AmiSio;
		SIO_DEV2        	*Dev = NULL;
		EFI_STRING			SioNvDataName=NULL;
		UINT8				UartNum, UartModeIndex;
		UINT32				Attribute = 0;       		

		Status = pRS->GetVariable( L"Setup", &SetupGuid, &Attribute, &VariableSize, &SetupData);
		if(EFI_ERROR(Status)) return Status;

		Status = pBS->LocateHandleBuffer(ByProtocol, &gEfiAmiSioProtocolGuid, NULL, &Count, &Handle);
		if(EFI_ERROR(Status)) return Status;		
	
		for (i = 0; i < Count; i++)
		{
			Status = pBS->HandleProtocol(Handle[i],&gEfiAmiSioProtocolGuid, &AmiSio);
			if (EFI_ERROR(Status)) continue;

			Dev = (SIO_DEV2*)(UINTN)AmiSio;
			if (Dev->DeviceInfo->Type != dsUART) continue;  // not COM port
			if (Dev->VlData.DevImplemented == 0) continue;

			UartNum = Dev->DeviceInfo->Uid;
			
			for (UartModeIndex = 0; UartModeIndex < MAX_UARTMODE_NUM; UartModeIndex++)
			{
				if (SetupData.AaeonUartMode2[UartModeIndex] == Enabled && SetupData.AaeonUartMode2Uid[UartModeIndex] == UartNum)
				{
					SioNvDataName = GetSioLdVarName(Dev,FALSE);
					Status = UartModeInit(UartNum, UartModeIndex, SioNvDataName, &SetupData);
				}
			}
		}
	
		pBS->FreePool(Handle);

		Status = pRS->SetVariable( L"Setup", &SetupGuid, Attribute, VariableSize, &SetupData);
	}

	return EFI_SUCCESS;
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
