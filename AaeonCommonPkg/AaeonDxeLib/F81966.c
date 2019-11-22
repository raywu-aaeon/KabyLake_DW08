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
#if defined(F81966_SUPPORT) && (F81966_SUPPORT == 1)
VOID
F81966SetGpio ( 
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
		case Gpio0x:
			SetupData->F81966Gpio0x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio0x_Oe[GpioGroupNum] = GpioType;
			break;

		case Gpio1x:
			SetupData->F81966Gpio1x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio1x_Oe[GpioGroupNum] = GpioType;
			break;	

		case Gpio2x:
			SetupData->F81966Gpio2x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio2x_Oe[GpioGroupNum] = GpioType;
			break;

		case Gpio3x:
			SetupData->F81966Gpio3x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio3x_Oe[GpioGroupNum] = GpioType;			
			break;

		case Gpio4x:
			SetupData->F81966Gpio4x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio4x_Oe[GpioGroupNum] = GpioType;			
			break;

		case Gpio5x:
			SetupData->F81966Gpio5x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio5x_Oe[GpioGroupNum] = GpioType;			
			break;

		case Gpio6x:
			SetupData->F81966Gpio6x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio6x_Oe[GpioGroupNum] = GpioType;			
			break;

		case Gpio7x:
			SetupData->F81966Gpio7x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio7x_Oe[GpioGroupNum] = GpioType;
			break;	

		case Gpio8x:
			SetupData->F81966Gpio8x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio8x_Oe[GpioGroupNum] = GpioType;
			break;		

		case Gpio9x:
			SetupData->F81966Gpio9x_Val[GpioGroupNum] = GpioVal;
			SetupData->F81966Gpio9x_Oe[GpioGroupNum] = GpioType;
			break;																	

		default:
			break;

	}	
}

VOID F81966LDNSelect(UINT8 Ldn)
{
	IoWrite8(F81966_CONFIG_INDEX, 0x07);
	IoWrite8(F81966_CONFIG_DATA, Ldn);
}

VOID F81966ConfigRegisterWrite(UINT8 Index, UINT8 Data)
{
	IoWrite8(F81966_CONFIG_INDEX, Index);
	IoWrite8(F81966_CONFIG_DATA, Data);
}

UINT8 F81966ConfigRegisterRead(UINT8 Index)
{
	UINT8 RegTmp8;
	IoWrite8(F81966_CONFIG_INDEX, Index);
	RegTmp8 = IoRead8(F81966_CONFIG_DATA);
	return RegTmp8;
}

VOID F81966EnterConfigMode()
{
	UINT8 F81966ConfigModeEnterValue[2] = {0x87,0x87};
	UINT8 i;

	// Enter config mode
	for(i=0;i<=0x01;i++){
		IoWrite8(F81966_CONFIG_INDEX, F81966ConfigModeEnterValue[i]);
		IoWrite8(0xEB, 0xFF); // IO delay
	}
}

VOID F81966ExitConfigMode()
{
	UINT8 F81966ConfigModeExitRegister = 0xAA;
	
	// Exit config mode
	IoWrite8(F81966_CONFIG_INDEX, F81966ConfigModeExitRegister);
}

UINT8 F81966GetGpio(IN UINT8 GpioNum)
{
	UINT8	RegTmp8, Offset;
	
	F81966EnterConfigMode();	
	F81966LDNSelect(F81966_LDN_GPIO);	//GPIO Device Configuration Registers
	
	if( (GpioNum/10) <= 7 )
	{
		Offset = ( ~(GpioNum/10) );
		Offset = Offset << 4;
		Offset &= 0xF0;
	}
	else if ( (GpioNum/10) >= 8 )
	{
		Offset = GpioNum/10;
		Offset = Offset << 4;
		Offset += 0x08; 	
	}
	RegTmp8 = F81966ConfigRegisterRead(Offset + 2);	//Pin Status Register
	RegTmp8 = RegTmp8 >> ( GpioNum%10 );
	RegTmp8 &= BIT0;
	
	F81966ExitConfigMode();
	return RegTmp8;
}	
#endif //F81966_SUPPORT