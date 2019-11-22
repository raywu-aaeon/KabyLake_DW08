#ifndef __AAEON_DXE_LIB__H__
#define __AAEON_DXE_LIB__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <token.h>
#include <Library/IoLib.h>
#include <Setup.h>	
#include <AaeonSetup.h>

//****************************************************************************/
//		TYPE DECLARATIONS
//****************************************************************************/
//**********************************************************************
//<AMI_THDR_START>
//
// Name: DBE_DATABASE_STRUCT
//
// Fields:	
// ColNum	Type		Description
//	1		UINTN		Initial elements Count of index array to be created
//	2		UINTN		Current RecordCount stored in Database
//	3		UINTN		Number of Indexes in Database
//	4		DBE_INDEX*	Pointerto the indexes information of this Database;
//	5		VOID		*IndexArray;
//
// Description:	Initial structure to hold Database information
//
// Notes:	
//
//<AMI_THDR_END>
//**********************************************************************


//****************************************************************************/
//		MACROS DECLARATIONS
//****************************************************************************/
#define  Gpio0x  0
#define  Gpio1x  1
#define  Gpio2x  2
#define  Gpio3x  3
#define  Gpio4x  4
#define  Gpio5x  5
#define  Gpio6x  6
#define  Gpio7x  7
#define  Gpio8x  8 
#define  Gpio9x  9 

#define Output 1
#define Input 0	

#define High 1
#define Low 0		

//****************************************************************************/
//		VARIABLE DECLARATIONS
//****************************************************************************/

//****************************************************************************/
//		FUNCTION DECLARATIONS
//****************************************************************************/
#if defined(IT8728F_SUPPORT) && (IT8728F_SUPPORT == 1)	
VOID IT8728FSetGpio ( 
	IN UINT8 	GpioNum,
	IN UINT8 	GpioVal,
	IN UINT8 	GpioType,
	IN SETUP_DATA	*SetupData
);

UINT8 IT8728FGetGpio(IN UINT8 GpioNum);
#endif //IT8728F_SUPPORT

#if defined(F81866_SUPPORT) && (F81866_SUPPORT == 1)
VOID F81866SetGpio ( 
	IN UINT8 	GpioNum,
	IN UINT8 	GpioVal,
	IN UINT8 	GpioType,
	IN SETUP_DATA	*SetupData
);	

UINT8 F81866GetGpio(IN UINT8 GpioNum);
#endif //F81866_SUPPORT

#if defined(F81966_SUPPORT) && (F81966_SUPPORT == 1)
VOID F81966SetGpio ( 
	IN UINT8 	GpioNum,
	IN UINT8 	GpioVal,
	IN UINT8 	GpioType,
	IN SETUP_DATA	*SetupData
);	

UINT8 F81966GetGpio(IN UINT8 GpioNum);
#endif //F81966_SUPPORT

#if defined(F81804_SUPPORT) && (F81804_SUPPORT == 1)
VOID F81804SetGpio ( 
	IN UINT8 	GpioNum,
	IN UINT8 	GpioVal,
	IN UINT8 	GpioType,
	IN SETUP_DATA	*SetupData
);	

UINT8 F81804GetGpio(IN UINT8 GpioNum);
#endif //F81804_SUPPORT

VOID AaeonSetGpio ( 
	IN UINT8 	GpioNum,
	IN UINT8 	GpioVal,
	IN UINT8 	GpioType,
	IN UINT8	ChipType,
	IN SETUP_DATA	*SetupData
);

UINT8 AaeonGetGpio(IN UINT8 ChipType, IN UINT8 GpioNum);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
