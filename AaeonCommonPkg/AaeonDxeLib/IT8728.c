//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include "AaeonDxeLib.h"

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
//		function definition _begin
//---------------------------------------------------------------------------
#if defined(IT8728F_SUPPORT) && (IT8728F_SUPPORT == 1)
VOID
IT8728FSetGpio ( 
	IN UINT8 	GpioNum,
	IN UINT8 	GpioVal,
	IN UINT8 	GpioType,
	IN SETUP_DATA	*SetupData
)	
{
	UINT8	GpioGroup, GpioGroupNum;

	GpioGroup = GpioNum / 10;
	GpioGroupNum = GpioNum % 10;

	switch(GpioGroup)
	{
		case Gpio1x:
			SetupData->IT8728FGpio1x_Val[GpioGroupNum] = GpioVal;
			SetupData->IT8728FGpio1x_Oe[GpioGroupNum] = GpioType;
			break;	

		case Gpio2x:
			SetupData->IT8728FGpio2x_Val[GpioGroupNum] = GpioVal;
			SetupData->IT8728FGpio2x_Oe[GpioGroupNum] = GpioType;
			break;

		case Gpio3x:
			SetupData->IT8728FGpio3x_Val[GpioGroupNum] = GpioVal;
			SetupData->IT8728FGpio3x_Oe[GpioGroupNum] = GpioType;			
			break;

		case Gpio4x:
			SetupData->IT8728FGpio4x_Val[GpioGroupNum] = GpioVal;
			SetupData->IT8728FGpio4x_Oe[GpioGroupNum] = GpioType;			
			break;

		case Gpio5x:
			SetupData->IT8728FGpio5x_Val[GpioGroupNum] = GpioVal;
			SetupData->IT8728FGpio5x_Oe[GpioGroupNum] = GpioType;			
			break;

		case Gpio6x:
			SetupData->IT8728FGpio6x_Val[GpioGroupNum] = GpioVal;
			SetupData->IT8728FGpio6x_Oe[GpioGroupNum] = GpioType;			
			break;

		case Gpio7x:
			SetupData->IT8728FGpio7x_Val[GpioGroupNum] = GpioVal;
			SetupData->IT8728FGpio7x_Oe[GpioGroupNum] = GpioType;
			break;	

		case Gpio8x:
			SetupData->IT8728FGpio8x_Val[GpioGroupNum] = GpioVal;
			SetupData->IT8728FGpio8x_Oe[GpioGroupNum] = GpioType;
			break;														

		default:
			break;

	}	
}
VOID IT8728FLDNSelect(UINT8 Ldn)
{
	IoWrite8(IT8728F_CONFIG_INDEX, 0x07);
	IoWrite8(IT8728F_CONFIG_DATA, Ldn);
}
VOID IT8728FConfigRegisterWrite(UINT8 Index, UINT8 Data)
{
	IoWrite8(IT8728F_CONFIG_INDEX, Index);
	IoWrite8(IT8728F_CONFIG_DATA, Data);
}
UINT8 IT8728FConfigRegisterRead(UINT8 Index)
{
	UINT8 Data8;
	IoWrite8(IT8728F_CONFIG_INDEX, Index);
	Data8 = IoRead8(IT8728F_CONFIG_DATA);
	return Data8;
}
VOID IT8728FEnterConfigMode()
{
	UINT8 IT8728FConfigModeEnterValue[4] = {0x87,0x01,0x55,0x55};
	UINT8 i;

	// Enter config mode
	for(i=0;i<=0x03;i++){
		IoWrite8(IT8728F_CONFIG_INDEX, IT8728FConfigModeEnterValue[i]);
		IoWrite8(0xEB, 0xFF); // IO delay
	}
}
VOID IT8728FExitConfigMode()
{
	UINT8 IT8728FConfigModeExitRegister = 0x02;

	// Exit config mode
	IoWrite8(IT8728F_CONFIG_INDEX, IT8728FConfigModeExitRegister);
	IoWrite8(0xEB, 0xFF); // IO delay
	IoWrite8(IT8728F_CONFIG_DATA, BIT1);
}
UINT8 IT8728FGetGpio(IN UINT8 GpioNum)
{
	UINT8 RegTmp8;
	UINT16 Address;
	
	Address = IT8728F_GPIO_BASE_ADDRESS + ((GpioNum/10)-1);
	RegTmp8 = IoRead8(Address);

	return RegTmp8;
}
#endif //IT8728F_SUPPORT