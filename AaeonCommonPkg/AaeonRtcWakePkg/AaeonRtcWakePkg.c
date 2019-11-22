//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <Uefi.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Token.h>
#include <Setup.h>
//#include <Protocol/SmmBase2.h>
//#include <Protocol/SmmPowerButtonDispatch2.h>
//#include <Protocol/SmmSxDispatch2.h>
//#include <Library/IoLib.h>
//#include <Library/UefiBootServicesTableLib.h>

//VOID RayDebug80(UINT8 Time, UINT8 Code){
//	UINTN i;
//	
//	i = 0x0FFFF | (Time << 16);
//	while(i != 0){
//		IoWrite8(0x80, Code);
//		i--;
//	}
//}

typedef struct {
  UINT8    AaeonRTCWakeupTimeHour ;
  UINT8    AaeonRTCWakeupTimeMinute ;
  UINT8    AaeonRTCWakeupTimeSecond ;
  UINT8    AaeonRTCWakeupDateDay;
} SMMData ;

SMMData gSmmData ;

//----------------------------------------------------------------------------
// Procedure:   DecToBCD
//
// Description: Converts a Decimal value to a BCD value.
//
// Input:       
//      IN UINT8 Dec - Decimal value
//
// Output:      
//      UINT8 - BCD value
//
// Notes:       
//      Only for 2 digit decimal.
//
//----------------------------------------------------------------------------
UINT8 DecToBCD(UINT8 Dec)
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}

//----------------------------------------------------------------------------
// Procedure:   BCDToDec
//
// Description: Converts a BCD value to a Decimal value.
//
// Input:       
//      IN UINT8 BCD -- BCD value
//
// Output:      
//      UINT8 - decimal value
//
// Notes:       
//      Only for 2 digit BCD.
//
//----------------------------------------------------------------------------
UINT8 BCDToDec(UINT8 BCD)
{
    UINT8 FirstDigit = BCD & 0xf;
    UINT8 SecondDigit = BCD >> 4;;
    
    return SecondDigit * 10  + FirstDigit;
}

//----------------------------------------------------------------------------
// Procedure:   ReadRtcIndex
//
// Description: Read the RTC value at the given Index.
//
// Input:       Index       RTC Index
//
// Output:      RTC Value read from the provided Index
//
// Notes:       Here is the control flow of this function:
//              1. Read port 0x70 (RTC Index Register) to get bit 7.
//                  Bit 7 is the NMI bit-it should not be changed.
//              2. Output 0x70 with the Index and NMI bit setting.
//              3. Read 0x71 for Data. Getting Dec when appropriate.
//              4. Return the Data.
//----------------------------------------------------------------------------
UINT8 ReadRtcIndex(IN UINT8 Index)
{
    UINT8 volatile Value;

    // Check if Data Time is valid
    if(Index <= 9) do {
    IoWrite8(0x70, 0x0A | 0x80);
    Value = IoRead8(0x71);        
    } while (Value & 0x80); 

    IoWrite8(0x70, Index | 0x80);
    // Read register
    Value = IoRead8(0x71);               
    if (Index <= 9) Value = BCDToDec(Value);    
    return (UINT8)Value;
}

//----------------------------------------------------------------------------
// Procedure:   WriteRtcIndex
//
// Description: Write the RTC value at the given Index.
//
// Input:       Index   RTC Index
//              Data    RTC Index
//
// Output:      None
//
// Notes:       Here is the control flow of this function:
//              1. Read port 0x70 (RTC Index Register) to get bit 7.
//                 Bit 7 is the NMI bit-it should not be changed.
//              2. Output 0x70 with the Index. Switch to BCD when needed.
//              3. Write the data to 0x71.
//----------------------------------------------------------------------------
void WriteRtcIndex(IN UINT8 Index, IN UINT8 Value)
{
    IoWrite8(0x70,Index | 0x80);
      if (Index <= 9 ) Value = DecToBCD(Value);
    // Write Register
    IoWrite8(0x71,Value);               
}


//----------------------------------------------------------------------------
// Procedure:   SetWakeupTime
//
// Description: Set the alarm time to CMOS location and enable alarm interrupt
//
// Input:       
//      IN EFI_TIME *Time
//
// Output:      
//      VOID
//
//----------------------------------------------------------------------------
void SetWakeupTime (
    IN EFI_TIME     *Time
)
{
    UINT8 Value;

    WriteRtcIndex(5,Time->Hour);
    WriteRtcIndex(3,Time->Minute);
    WriteRtcIndex(1,Time->Second);
    Value = ReadRtcIndex(0x0D) & 0xC0;
    WriteRtcIndex(0x0D,(Value|DecToBCD(Time->Day)));

    //Set Enable
    Value = ReadRtcIndex(0xB);
    Value |= 1 << 5;
    WriteRtcIndex(0xB,Value);
}

//WakeFuntion- set RTC wake parameter
VOID RTCWakeFunc(){
	UINT32 i = 0;
	EFI_TIME Time;
    
	Time.Hour = gSmmData.AaeonRTCWakeupTimeHour;
	Time.Minute = gSmmData.AaeonRTCWakeupTimeMinute;
	Time.Second = gSmmData.AaeonRTCWakeupTimeSecond;
	Time.Day = gSmmData.AaeonRTCWakeupDateDay;

	SetWakeupTime(&Time);            
	//Clear RTC PM1 status
	IoWrite16(PM_BASE_ADDRESS , ( IoRead16(PM_BASE_ADDRESS) | (1 << 10) ));
	//set RTC_EN bit to wake up from the alarm
	IoWrite32(PM_BASE_ADDRESS, ( IoRead32(PM_BASE_ADDRESS) | (1 << 26) ));
}

EFI_STATUS
EFIAPI
AaeonRtcWake_Init (SETUP_DATA *SetupData)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8	Value;

	//Disable RTC alarm and clear RTC PM1 status
	Value = ReadRtcIndex(0xB);
	Value &= ~((UINT8)1 << 5);
	WriteRtcIndex(0xB,Value);
	//Clear Alarm Flag (AF) by reading RTC Reg C
	Value = ReadRtcIndex(0xC);
	IoWrite16(PM_BASE_ADDRESS , ( IoRead16(PM_BASE_ADDRESS) | (1 << 10) ));

	if(!EFI_ERROR(Status)){
		//if(gSetupData.FixedWakeOnRTCS5 == 1){
		if(SetupData->AaeonWakeOnRtc == 1){
			gSmmData.AaeonRTCWakeupTimeHour = SetupData->AaeonRTCWakeupTimeHour ;
			gSmmData.AaeonRTCWakeupTimeMinute = SetupData->AaeonRTCWakeupTimeMinute ;
			gSmmData.AaeonRTCWakeupTimeSecond = SetupData->AaeonRTCWakeupTimeSecond ;
			gSmmData.AaeonRTCWakeupDateDay = SetupData->AaeonRTCWakeupTimeDay;
		}

		//if(gSetupData.DynamicWakeOnRTCS5 == 1){
		if(SetupData->AaeonWakeOnRtc == 2){
			gSmmData.AaeonRTCWakeupTimeHour = ReadRtcIndex(4);
			gSmmData.AaeonRTCWakeupTimeMinute = ReadRtcIndex(2);
			gSmmData.AaeonRTCWakeupTimeSecond = ReadRtcIndex(0);
			gSmmData.AaeonRTCWakeupTimeMinute += SetupData->AaeonRTCWakeupTimeMinuteIncrease;
			if  (gSmmData.AaeonRTCWakeupTimeMinute >= 60)
			{
				gSmmData.AaeonRTCWakeupTimeMinute = 0;
				++gSmmData.AaeonRTCWakeupTimeHour;
				if (gSmmData.AaeonRTCWakeupTimeHour == 24)
					gSmmData.AaeonRTCWakeupTimeHour = 0;
			}
		}
	}

	if(SetupData->AaeonWakeOnRtc == 1 || SetupData->AaeonWakeOnRtc == 2)
		RTCWakeFunc();

	return Status;
}
