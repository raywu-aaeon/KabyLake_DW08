
#include <token.h>
#include <AmiDxeLib.h> // Optional. should use Mde Library instead.
#include <AmiCspLib.h> // Optional. should use Mde Library instead.
#include <Setup.h> // Optional. should use Mde Library instead.
#include <AaeonDxeLib.h>
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
EFI_STATUS
EFIAPI
HwDesignSetMode ( 
	IN UINT8 	UartMode,
	IN UINT8 	UartModeSource,
	IN UINT8	*UartModeGpioTable,
	IN SETUP_DATA	*SetupData
)	
{
	UINT8 		HwDesignRs232TruthTable[MAX_HW_DESIGN_GPIO_NUM] = HW_DESIGN_RS232_TRUTH_TABLE;
	UINT8 		HwDesignRs422TruthTable[MAX_HW_DESIGN_GPIO_NUM] = HW_DESIGN_RS422_TRUTH_TABLE;
	UINT8 		HwDesignRs485TruthTable[MAX_HW_DESIGN_GPIO_NUM] = HW_DESIGN_RS485_TRUTH_TABLE;
	UINT8		i;

	for (i = 0; i < MAX_HW_DESIGN_GPIO_NUM; i++)
	{
		switch(UartMode)
		{
			case RS232:
				AaeonSetGpio(UartModeGpioTable[i], HwDesignRs232TruthTable[i], Output, UartModeSource, SetupData);
				break;
	
			case RS422:
				AaeonSetGpio(UartModeGpioTable[i], HwDesignRs232TruthTable[i], Output, UartModeSource, SetupData);
				break;
	
			case RS485:
				AaeonSetGpio(UartModeGpioTable[i], HwDesignRs232TruthTable[i], Output, UartModeSource, SetupData);
				break;
	
			default:
				return EFI_NOT_FOUND;	
		}		
	}

	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
F81438SetMode (
	IN UINT8 	UartMode,
	IN UINT8 	UartModeSource,
	IN UINT8	*UartModeGpioTable,
	IN SETUP_DATA	*SetupData
)	
{
	UINT8 		F81438Rs232TruthTable[MAX_F81438_GPIO_NUM] = F81438_RS232_TRUTH_TABLE;
	UINT8 		F81438Rs422TruthTable[MAX_F81438_GPIO_NUM] = F81438_RS422_TRUTH_TABLE;
	UINT8 		F81438Rs485TruthTable[MAX_F81438_GPIO_NUM] = F81438_RS485_TRUTH_TABLE;
	UINT8		i;

	for (i = 0; i < MAX_F81438_GPIO_NUM; i++)
	{
		switch(UartMode)
		{
			case 0: //RS232
				AaeonSetGpio(UartModeGpioTable[i], F81438Rs232TruthTable[i], Output, UartModeSource, SetupData);
				break;
	
			case 1: //RS422
				AaeonSetGpio(UartModeGpioTable[i], F81438Rs422TruthTable[i], Output, UartModeSource, SetupData);
				break;
	
			case 2: //RS485
				AaeonSetGpio(UartModeGpioTable[i], F81438Rs485TruthTable[i], Output, UartModeSource, SetupData);
				break;
	
			default:
				return EFI_NOT_FOUND;	
		}
	}	
		
	return EFI_SUCCESS;
}

#if SIO_FLOW_CTRL_SUPPORT
VOID
SioSetFlowControl ( 
	IN UINT8 	UartIndex,
	IN UINT8 	UartMode,
	IN UINT8	UartModeSource,
	IN SETUP_DATA	*SetupData
)	
{
	UINT8		UartFlowControl = 0;

	if(UartMode == RS485) UartFlowControl = 1;

	switch(UartModeSource)
	{
		case F81866_CHIP_TYPE:
			#if defined(F81866_SUPPORT) && (F81866_SUPPORT == 1)
			SetupData->F81866UartAutoFlowControlEnable[UartIndex] = UartFlowControl;
			#endif
			break;

		case F81966_CHIP_TYPE:
			#if defined(F81966_SUPPORT) && (F81966_SUPPORT == 1)
			SetupData->F81966UartAutoFlowControlEnable[UartIndex] = UartFlowControl;
			#endif
			break;

		case F81801_CHIP_TYPE:

			break;	

		case F81216SEC_CHIP_TYPE:

			break;

		case IT8728F_CHIP_TYPE:
			
			break;

		default:
			break;

	}	
}
#endif	

EFI_STATUS
EFIAPI
UartModeInit ( 
	IN UINT8 	UartNum,
	IN UINT8 	UartModeIndex,
	IN EFI_STRING	SioNvDataName,
	IN SETUP_DATA	*SetupData
)
{
	EFI_STATUS 	Status = EFI_NOT_FOUND;
	EFI_GUID	SioSetupGuid = SIO_VARSTORE_GUID;
	UINTN		SioVariableSize = sizeof(SIO_DEV_NV_DATA);
	SIO_DEV_NV_DATA	SioSetupData;
	UINT8		UartIndex = UartNum - 1, UartMode;
	UINT8		UartModeGpio0 = SetupData->AaeonUartMode2Gpio0[UartModeIndex];
	UINT8		UartModeGpio1 = SetupData->AaeonUartMode2Gpio1[UartModeIndex];
	UINT8		UartModeGpio2 = SetupData->AaeonUartMode2Gpio2[UartModeIndex];
	UINT8 		UartModeGpioTable[3] = {UartModeGpio0, UartModeGpio1, UartModeGpio2};
	UINT8		UartModeSource = SetupData->AaeonUartMode2Source[UartModeIndex];

	Status = pRS->GetVariable( SioNvDataName, &SioSetupGuid, NULL, &SioVariableSize, &SioSetupData );
	if(EFI_ERROR(Status)) return Status;

	UartMode = SioSetupData.DevMode;

	switch(SetupData->AaeonUartMode2Transceiver)
	{
		case F81438:
			Status = F81438SetMode(UartMode, UartModeSource, UartModeGpioTable, SetupData);
			break;

		case HW_DESIGN:
			Status = HwDesignSetMode(UartMode, UartModeSource, UartModeGpioTable, SetupData);
			break;	

		default:
			break;
	}		

	#if SIO_FLOW_CTRL_SUPPORT
		SioSetFlowControl(UartIndex, UartMode, UartModeSource, SetupData);
	#endif	

	return Status;
}
