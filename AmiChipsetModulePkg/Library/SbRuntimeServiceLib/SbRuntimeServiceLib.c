//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file SbRuntimeServiceLib.c
    This file contains code for South Bridge runtime protocol

**/

// Module specific Includes
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Guid/Rtc.h>
#include <Library/AmiChipsetRuntimeServiceLib.h>
#include <Library/HobLib.h>
#include <PchAccess.h>
#include <SbElinks.h>

// Produced Protocols
#include <Protocol/Reset.h>
#include <Protocol/RealTimeClock.h>
#include <Protocol/Timer.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/DebugLib.h>
#include <Library/AmiSbMiscLib.h>

//----------------------------------------------------------------------------
//          Variable Declaration
//----------------------------------------------------------------------------
EFI_EVENT ResetEvent;
static BOOLEAN Runtime = FALSE;
static BOOLEAN  gTimeOut = FALSE;
static UINT16 gTimeZone;
static UINT8  gDaylight;

//This the number of days in a month - 1. (0 Based)
UINT8       DaysInMonth[] = {30, 27, 30, 29, 30,\
                                29, 30, 30, 29, 30, 29, 30};


// Function Prototypes
typedef VOID (SB_RESET_CALLBACK) (
    IN EFI_RESET_TYPE           ResetType,
    IN EFI_STATUS               ResetStatus,
    IN UINTN                    DataSize,
    IN VOID                     *ResetData OPTIONAL
);

// Function Definitions
extern SB_RESET_CALLBACK SB_RESET_CALLBACK_LIST EndOfList;
extern SB_INIT_RTC_CALLBACK SB_INIT_RTC_CALLBACK_LIST EndOfList1;
SB_RESET_CALLBACK* SbResetCallbackList[] = { SB_RESET_CALLBACK_LIST NULL };
SB_INIT_RTC_CALLBACK* SbInitRtcCallbackList[] = { SB_INIT_RTC_CALLBACK_LIST NULL };

//----------------------------------------------------------------------------
//   USUALLY NO PORTING REQUIRED FOR THE FOLLOWING ROUTINES
//----------------------------------------------------------------------------


/**
    This function is the interface for the reset function.  In
    the future, this may allow for a shared library for DXE and PEI.

    @param ResetType       Type of reset to perform
    @param ResetStatus     System status that caused the reset.  if part
        of normal operation then this should be
        EFI_SUCCESS, Otherwise it should reflect the
        state of the system that caused it
    @param DataSize        Size in bytes of the data to be logged
    @param *ResetData      Pointer to the data buffer that is to be logged

    @retval None, Even though it should never get that far

**/

EFI_STATUS EFIAPI
AmiChipsetResetSystem (
    IN EFI_RESET_TYPE   ResetType,
    IN EFI_STATUS       ResetStatus,
    IN UINTN            DataSize,
    IN CHAR16           *ResetData OPTIONAL
    )
{
    UINT32  Index;
	
    for (Index = 0; SbResetCallbackList[Index] != NULL; Index++) {
        SbResetCallbackList[Index](ResetType, ResetStatus, DataSize, ResetData);
    }
    
    // do a cold reset of the system
    SBLib_ResetSystem (ResetType);

    // This should not get here
    return EFI_SUCCESS;
}

/**
    This function converts data from DEC to BCD format

    @param Dec value to be converted

    @retval UINT8 result of conversion

**/
UINT8 DecToBCD(
    IN UINT8 Dec
)
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}

/**
    This function converts data from BCD to DEC format

    @param IN UINT8 BCD - value to be converted

    @retval UINT8 result of conversion

**/
UINT8 BCDToDec(IN UINT8 BCD)
{
    UINT8 FirstDigit = BCD & 0xf;
    UINT8 SecondDigit = BCD >> 4;;

    return SecondDigit * 10  + FirstDigit;
}

/**
    Read the RTC value at the given Index.

    @param Index RTC Index

    @retval RTC Value read from the provided Index

    @note  Here is the control flow of this function:
              1. Read port 0x70 (RTC Index Register) to get bit 7.
                 Bit 7 is the NMI bit-it should not be changed.
              2. Set Index with the NMI bit setting.
              3. Output 0x70 with the Index.
              4. Read 0x71 for Data.
              5. Return the Data.
**/
UINT8 ReadRtcIndex(
    IN UINT8 Index
)
{
    volatile UINT8  Value;
    BOOLEAN         IntState = CPULib_GetInterruptState();

    CPULib_DisableInterrupt();

    IoWrite8(CMOS_IO_INDEX_BACKDOOR, Index);
    Value = IoRead8(CMOS_IO_DATA_BACKDOOR); // Read register.

    if (IntState) CPULib_EnableInterrupt();

    if ((Index <= RTC_YEAR_REG) || (Index == ACPI_CENTURY_CMOS))
        Value = BCDToDec(Value);  

    return (UINT8)Value;
}

/**
    Write the RTC value at the given Index.

    @param Index RTC Index
    @param Value Value to be written

    @retval VOID

    @note  Here is the control flow of this function:
              1. Read port 0x70 (RTC Index Register) to get bit 7.
                 Bit 7 is the NMI bit-it should not be changed.
              2. Set Index with the NMI bit setting.
              3. Output 0x70 with the Index.
              4. Write the data to 0x71.
**/
VOID WriteRtcIndex(
    IN UINT8 Index,
    IN UINT8 Value
)
{
    BOOLEAN IntState = CPULib_GetInterruptState();

    if ((Index <= RTC_YEAR_REG) || (Index == ACPI_CENTURY_CMOS))
        Value = DecToBCD(Value);

    CPULib_DisableInterrupt();

    IoWrite8(CMOS_IO_INDEX_BACKDOOR, Index);
    IoWrite8(CMOS_IO_DATA_BACKDOOR, Value); // Write Register.

    if (IntState) CPULib_EnableInterrupt();

}

/**
    This function initializes RTC

    @param VOID

    @retval VOID

**/
VOID EFIAPI InitRtc(VOID)
{
    UINT32                  Index;
    
#if !EXTERNAL_RTC_SUPPORT
    WriteRtcIndex(RTC_REG_B_INDEX, 0x82);
    WriteRtcIndex(RTC_REG_A_INDEX, 0x26);
    ReadRtcIndex(RTC_REG_C_INDEX);
    WriteRtcIndex(RTC_REG_B_INDEX, 0x02);
#endif

    for (Index = 0; SbInitRtcCallbackList[Index] != NULL; Index++) {
        SbInitRtcCallbackList[Index]();
    }
}

/**
    Enables Disables RTC Date and Time update cicles.

    @param Enable TRUE or FALSE to Enable\Disabe RTC Update.

    @retval VOID

**/
VOID SetUpdate(
    IN BOOLEAN Enable
)
{
    RTC_REG_B       RegB;
    UINT8           Set = (Enable) ? 0 : 1;

    RegB.REG_B = ReadRtcIndex(RTC_REG_B_INDEX);
    if (RegB.Set != Set) {
        RegB.Set = Set;
        WriteRtcIndex(RTC_REG_B_INDEX, RegB.REG_B);
    }
}

/**
    Check if RTC Date and Time update in progress and waits till
    it's over.

    @param VOID

    @retval VOID

**/
VOID CheckUpdateCmplete (VOID)
{
    volatile RTC_REG_A  RegA;
    UINTN               TimeOut = 0;

    RegA.REG_A = ReadRtcIndex(RTC_REG_A_INDEX);
    while (RegA.UpdInProgr) {
        RegA.REG_A = ReadRtcIndex(RTC_REG_A_INDEX);
        TimeOut++;
        if (TimeOut >= 0x0fffff) {
            gTimeOut = TRUE;
            return;
        }
    }

    gTimeOut = FALSE;
}

/**
    Returns an index that represents the day of the week of the date passed in

    
    @param EFI_TIME *Time - pointer to EFI_TIME structure

   
    @retval returns the index to the day of the week.  0 = Sunday, 1 = Monday ... 6 = Saturday

**/
UINT8 GetDayOfTheWeek (
    IN EFI_TIME         *Time
)
{
    UINT16      a;
    UINT16      m;
    UINT16      d;
    UINT16      y;

    a = (14 - Time->Month) / 12;
    y = Time->Year - a;
    m = Time->Month + 12 * a - 2;
    d = (Time->Day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) % 7;

    return (UINT8)d;
}

BOOLEAN IsLeapYear (
    IN EFI_TIME     *Time )
{
    if (Time->Year % 4 == 0) {
        if (Time->Year % 100 == 0) {
            if (Time->Year % 400 == 0) {
                return TRUE;
            } else {
                return FALSE;
            }
        } else {
            return TRUE;
        }
    } else {
        return FALSE;
    }
}

/**
    This routine verifies if time and data if needed, before
    setting the RTC

    @param Time Time to verify with

    @retval TRUE if valid time and date

    @note  Here is the control flow of this function:
              1. Decrease month and date to change to 0-base
              2. Validate Year, Month and Day. If invalid, return FALSE.
              3. Validate Hour, Minute, and Second. If invalid, return FALSE.
              4. Return True.

**/
BOOLEAN VerifyTime(
    IN EFI_TIME *Time
)
{
    //Always check these to satisfy EFI compliancy test even for setting wake-up time.
    UINT8 Month = Time->Month - 1;
    UINT8 Day   = Time->Day - 1;

    if ((Time->Year < EARLIEST_YEAR) || (Time->Year > MAXIMUM_YEAR)) return FALSE;
    if (Month > 11) return FALSE;             // 0 based month
    if ((Month != 1) || (!IsLeapYear(Time))) { // Not leap year or not February.
        if (Day > DaysInMonth[Month]) return FALSE; //All values already adjusted for 0 based.
    } else {
        if (Day > 28) return FALSE; //February
    }

    if (Time->Hour > 23) return FALSE;
    if (Time->Minute > 59) return FALSE;
    if (Time->Second > 59) return FALSE;

    //Check these to satisfy EFI compliancy test.
    if (Time->Nanosecond > 999999999) return FALSE; //999,999,999
    if (Time->TimeZone < -1440) return FALSE;
    if (Time->TimeZone > 1440 && Time->TimeZone != \
                        EFI_UNSPECIFIED_TIMEZONE) return FALSE;

    return TRUE;
}

/**
    Check if RTC Mode and Format have appropriate values and sets
    them if necessary

    @param Set if true, force Rtc to 24 hour mode and binary format

    @retval EFI_SUCCESS Rtc mode and format have appropriate values
    @retval EFI_DEVICE_ERROR Rtc mode and/or format are invalid

**/
EFI_STATUS CheckRtc(
    IN BOOLEAN Set
)
{
    RTC_REG_B   RegB;

    //Check RTC Conditions and stuff
    RegB.REG_B = ReadRtcIndex(RTC_REG_B_INDEX);

    if(RegB.Mode == 0 || RegB.Format == 1) {
        if(Set) {
            RegB.Mode = 1;                  // 0 - 12 hour mode              1 - 24 hour mode
            RegB.Format = 0;                // 0 - BCD Format                1 - Binary Format
            WriteRtcIndex(RTC_REG_B_INDEX,RegB.REG_B);
        } else {
            return EFI_DEVICE_ERROR;
        }
    }
    return EFI_SUCCESS;
}

/**
    Return the current date and time

    @param Time Current time filled in EFI_TIME structure
    @param Capabilities Time capabilities (OPTIONAL)

    @retval EFI_SUCCESS

    @note  Here is the control flow of this function:
              1. Read the original time format 12/24 hours and BCD/binary.
              2. Set the format to 24 hrs and binary.
              3. Read the 2 digit year.
              4. Add either 1900 or 2000, so the year is between 1998 - 2097.
              5. Read the month, day, hour, minute, second.
              6. Set the nanosecond to 0.
              7. Set the time to zone to unspecified.
              8. Set daylight savings value to 0.
              9. Restore the original time format.
              10. Set Capabilities with 1 sec Resolution, 0 Accuracy (Unknown), and False SetsToZero.
              11. Return EFI_SUCCESS.

**/

EFI_STATUS EFIAPI AmiChipsetGetTime (
    OUT EFI_TIME                *Time,
    OUT EFI_TIME_CAPABILITIES   *Capabilities OPTIONAL
)
{
    EFI_STATUS          Status;
    UINT8               Year;
    BOOLEAN             IntState;
    BOOLEAN             SmiState;
    UINTN               TimeVarSize = sizeof(TIME_VARIABLE);
    TIME_VARIABLE       TimeVar;
    UINT8               Buffer8;

    if (Time == NULL) return EFI_INVALID_PARAMETER;

    // Check RTC Conditions (24h Mode and BCD is ON)
    Status = CheckRtc(FALSE);  
    if (EFI_ERROR(Status)) return Status;

    // Get SMI State and disable it
    SmiState = SbLib_GetSmiState();
    SbLib_SmiDisable();    
    // Get INTERRUPT State and disable it
    IntState = CPULib_GetInterruptState();
    CPULib_DisableInterrupt();

    // Wait till RTC is safe to read,
    CheckUpdateCmplete();
    if (gTimeOut) InitRtc();
  
    // After control comes back, we will have 488 u's to read data.
    Time->Second = ReadRtcIndex(RTC_SECONDS_REG);
    Year = ReadRtcIndex(RTC_YEAR_REG);
    Time->Month = ReadRtcIndex(RTC_MONTH_REG);
    Time->Day = ReadRtcIndex(RTC_DAY_OF_MONTH_REG);
    Time->Hour = ReadRtcIndex(RTC_HOURS_REG);
    Time->Minute = ReadRtcIndex(RTC_MINUTES_REG);
    if (Time->Second > ReadRtcIndex(RTC_SECONDS_REG)) {
        Year = ReadRtcIndex(RTC_YEAR_REG);
        Time->Month = ReadRtcIndex(RTC_MONTH_REG);
        Time->Day = ReadRtcIndex(RTC_DAY_OF_MONTH_REG);
        Time->Hour = ReadRtcIndex(RTC_HOURS_REG);
        Time->Minute = ReadRtcIndex(RTC_MINUTES_REG);
        Time->Second = ReadRtcIndex(RTC_SECONDS_REG);
    }
    // Restore SMIs and INTERRUPT State
    if(IntState) CPULib_EnableInterrupt();
    if(SmiState) SbLib_SmiEnable();

    // This Register is not affected by UIP bit so read it very last.
    // If RTC Year only 1 digit, EFI spec says years rang is 1998 - 2097
    Time->Year = ReadRtcIndex(ACPI_CENTURY_CMOS) * 100 + Year;

    Time->Nanosecond= 0;

    // Save BIOSWE bit (B0:D31:F5 Reg#DCh[0])
    Buffer8 = PciRead8 (PCI_LIB_ADDRESS(0, 0x1F, 0x05, R_PCH_SPI_BC));
    //Buffer8 = READ_PCI8_SB(SB_REG_BIOS_CNTL);

    if ((Buffer8 & BIT00) == 0) {
      Status = pRS->GetVariable( L"EfiTime", \
                                 &gEfiTimeVariableGuid, \
                                 NULL, \
                                 &TimeVarSize, \
                                 &TimeVar );
    } else {
      TimeVar.TimeZone = gTimeZone;
      TimeVar.Daylight = gDaylight;
    }

    // Restore BIOSWE bit (B0:D31:F5 Reg#DCh[0])
    //WRITE_PCI8_SB(SB_REG_BIOS_CNTL, Buffer8);
    PciWrite8 (PCI_LIB_ADDRESS(0, 0x1F, 0x05, R_PCH_SPI_BC), Buffer8);

    if (EFI_ERROR(Status)) {
        Time->TimeZone = EFI_UNSPECIFIED_TIMEZONE;
        Time->Daylight = 0;
    } else {
        Time->TimeZone = TimeVar.TimeZone;
        Time->Daylight = TimeVar.Daylight;
    }
    
      gTimeZone = Time->TimeZone;
      gDaylight = Time->Daylight;

    if (Capabilities != NULL) {
        Capabilities->Resolution = 1;
        Capabilities->Accuracy = 0;
        Capabilities->SetsToZero = 0;
    }

    return EFI_SUCCESS;
}

/**
    Sets the RTC time

    @param Time Time to set

        
    @retval EFI_SUCCESS Time is Set
    @retval EFI_INVALID_PARAMETER Time to Set is not valid.

    @note  Here is the control flow of this function:
              1. Read the original time format 12/24 hours and BCD/binary.
              2. Set the format to 24 hrs and binary.
              3. Verify the time to set. If it is an invalid time,
                 restore the time format and return EFI_INVALID_PARAMETER.
              4. Change the 4 digit year to a 2 digit year.
              5. Stop the RTC time.
              6. Store time and data on the RTC.
              7. Read the month, day, hour, minute, second.
              8. Start the RTC time.
              9. Restore the original time format.
              10. Return EFI_SUCCESS.

**/
EFI_STATUS EFIAPI AmiChipsetSetTime (
    IN EFI_TIME     *Time
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    UINTN         TimeVarSize = sizeof(TIME_VARIABLE);
    TIME_VARIABLE TimeVar;

    //Check RTC Conditions and stuff
    CheckRtc(TRUE);

    if (Time == NULL) return EFI_INVALID_PARAMETER;
    if (!VerifyTime(Time)) return EFI_INVALID_PARAMETER;

    SetUpdate(FALSE);
    WriteRtcIndex(ACPI_CENTURY_CMOS, Time->Year / 100);
    WriteRtcIndex(RTC_YEAR_REG, Time->Year % 100);
    WriteRtcIndex(RTC_MONTH_REG,Time->Month);
    WriteRtcIndex(RTC_DAY_OF_MONTH_REG,Time->Day);
    // calculate the day of the week. Add 1, because Algorithm is 0 based and RTC is 1 based
    WriteRtcIndex(RTC_DAY_OF_WEEK_REG, GetDayOfTheWeek(Time) + 1);
    WriteRtcIndex(RTC_HOURS_REG,Time->Hour);
    WriteRtcIndex(RTC_MINUTES_REG,Time->Minute);
    WriteRtcIndex(RTC_SECONDS_REG,Time->Second);
    SetUpdate(TRUE);

    Status = pRS->GetVariable(
                        L"EfiTime",
                        &gEfiTimeVariableGuid,
                        NULL,
                        &TimeVarSize,
                        &TimeVar
                        );

    if (EFI_ERROR(Status) || (TimeVar.TimeZone != Time->TimeZone) || (TimeVar.Daylight != Time->Daylight))
    {
        TimeVar.TimeZone  = Time->TimeZone;
        TimeVar.Daylight  = Time->Daylight;

        Status = pRS->SetVariable(
                        L"EfiTime",
                        &gEfiTimeVariableGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof(TIME_VARIABLE),
                        &TimeVar
                        );
       if(!EFI_ERROR(Status)){
             gTimeZone = Time->TimeZone;
             gDaylight = Time->Daylight;
        }
    }
    return  Status;
}

/**
    Read the wake time. Read the status if it is enabled or
    if the system has woken up.

    @param Enabled Flag indicating the validity of wakeup time
    @param Pending Check if wake up time has expired.
    @param Time Current wake up time setting

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Read the original time format 12/24 hours and BCD/binary.
              2. Set the format to 24 hrs and binary.
              3. Read the status if the wake up time is enabled or if it has expired.
              4. Set the wakeup time.
              5. Restore the original time format.
              6. Return EFI_SUCCESS.

**/

EFI_STATUS EFIAPI AmiChipsetGetWakeupTime (
    OUT BOOLEAN     *Enabled,
    OUT BOOLEAN     *Pending,
    OUT EFI_TIME    *Time
)
{
    EFI_STATUS              Status;
    BOOLEAN                 IntState;
    BOOLEAN                 SmiState;
    RTC_REG_B               RegB;
    RTC_REG_C               RegC;
#if ACPI_ALARM_DAY_CMOS
    RTC_DATE_ALARM_REG      RegDateAlarm;
    UINT8                   Day;
#endif
#if ACPI_ALARM_MONTH_CMOS
    RTC_MONTH_ALARM_REG     RegMonthAlarm;
    UINT8                   Month;
#endif
// AMI_OVERRIDE_SCT000_START >>>
    EFI_TIME                RtcTime;    
    UINTN                   DataSize;
// AMI_OVERRIDE_SCT000_END <<<
    UINT8       Year;
	
    if (!Enabled || !Pending || !Time) return EFI_INVALID_PARAMETER;

    // Check RTC Conditions (24h Mode and BCD is ON)
    Status = CheckRtc(FALSE);  
    if (EFI_ERROR(Status)) return Status;

    // Get SMI State and disable it
    SmiState = SbLib_GetSmiState();
    SbLib_SmiDisable();    
    // Get INTERRUPT State and disable it
    IntState = CPULib_GetInterruptState();
    CPULib_DisableInterrupt();

    // Wait till RTC is safe to read,
    CheckUpdateCmplete();
    if (gTimeOut) InitRtc();

    Time->Hour = ReadRtcIndex(RTC_HOURS_ALARM_REG);
    Time->Minute = ReadRtcIndex(RTC_MINUTES_ALARM_REG);
    Time->Second = ReadRtcIndex(RTC_SECONDS_ALARM_REG);

    // Restore SMIs and INTERRUPT State
    if (IntState) CPULib_EnableInterrupt();
    if (SmiState) SbLib_SmiEnable();
    
    Time->Month = ReadRtcIndex(RTC_MONTH_REG);
    Time->Day = ReadRtcIndex(RTC_DAY_OF_MONTH_REG);
#if ACPI_ALARM_DAY_CMOS
    RegDateAlarm.REG_DATE_ALARM = ReadRtcIndex(ACPI_ALARM_DAY_CMOS);
    Day = BCDToDec(RegDateAlarm.DateAlarm);
    if ((Day < 32) && (Day > 0)) Time->Day = Day;
#endif

#if ACPI_ALARM_MONTH_CMOS
    RegMonthAlarm.REG_MONTH_ALARM = ReadRtcIndex(ACPI_ALARM_MONTH_CMOS);
    Month = BCDToDec(RegMonthAlarm.MonthAlarm);
    if ((Month < 13) && (Month > 0)) Time->Month = Month;
#endif

    RegB.REG_B = ReadRtcIndex(RTC_REG_B_INDEX);
    RegC.REG_C = ReadRtcIndex(RTC_REG_C_INDEX);


    *Enabled = (RegB.AlarmInt == 1) ? TRUE : FALSE;
    *Pending = (RegC.AlarmFlag == 1) ? TRUE : FALSE;
    
    Year = ReadRtcIndex(RTC_YEAR_REG);
    Time->Year = ReadRtcIndex(ACPI_CENTURY_CMOS) * 100 + Year;
    Time->TimeZone = gTimeZone;
    Time->Daylight = gDaylight;
    Time->Nanosecond = 0;
   
// AMI_OVERRIDE_SCT000_START >>>
    //
    // Get the alarm info from variable
    //    
    DataSize = sizeof (EFI_TIME);
    Status = pRS->GetVariable (
                   L"RTCALARM",
                   &gEfiTimeVariableGuid,
                   NULL,
                   &DataSize,
                   &RtcTime
                   );
    if (!EFI_ERROR (Status)) {   
        Time->Year     = RtcTime.Year;
        Time->TimeZone = RtcTime.TimeZone;
        Time->Daylight = RtcTime.Daylight;
    }
    
    return EFI_SUCCESS;
// AMI_OVERRIDE_SCT000_END <<<
}


/**
    Enable/disable and set wakeup time

    @param Enable Flag indicating whether to enable/disble the time
    @param Time Time to set

    
    @retval EFI_SUCCESS             Time is Set and/or Enabled/Disabled.
    @retval EFI_INVALID_PARAMETER   Invalid time or enabling with a NULL Time.

    @note  Here is the control flow of this function:
              1. Read the original time format 12/24 hours and BCD/binary.
              2. If Time is not NULL,
                  a. Verify the wakeup time to set. If it is an invalid time,
                     restore the time format and return EFI_INVALID_PARAMETER.
                  b. Set the wakeup time.
              3. If Time is NULL and Enable is true, restore original time format
                 and return EFI_INVALID_PARAMETER.
              4. Enable/Disable wakeup.
              5. Restore the original time format.
              6. Return EFI_SUCCESS.

**/

EFI_STATUS EFIAPI AmiChipsetSetWakeupTime (
    IN BOOLEAN      Enable,
    IN EFI_TIME     *Time OPTIONAL
)
{

    EFI_STATUS              Status;    
    RTC_REG_B               RegB;
#if ACPI_ALARM_DAY_CMOS
    RTC_DATE_ALARM_REG      RegDateAlarm;
#endif
#if ACPI_ALARM_MONTH_CMOS
    RTC_MONTH_ALARM_REG     RegMonthAlarm;
#endif
#if ACPI_ALARM_EVERYDAY_SUPPORT
    UINT8                   Day;
#endif  
// AMI_OVERRIDE_SCT000_START >>>    
    EFI_TIME                RtcTime = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// AMI_OVERRIDE_SCT000_END <<<
    
    // Check RTC Conditions (24h Mode and BCD is ON)
    Status = CheckRtc(FALSE);  
    if(EFI_ERROR(Status)) return Status;

    if (Time != NULL) {
#if ACPI_ALARM_EVERYDAY_SUPPORT
        Day = Time->Day;
        if (Day == 0) Time->Day = 1;
#endif        
        if (!VerifyTime(Time)) return EFI_INVALID_PARAMETER;
// AMI_OVERRIDE_SCT001_START >>>
        if (Time->Year >= 2100) return EFI_INVALID_PARAMETER;
// AMI_OVERRIDE_SCT001_END <<<
#if ACPI_ALARM_EVERYDAY_SUPPORT
        if (Day != Time->Day) Time->Day = 0;
#endif        
    } else {
        if (Enable) return EFI_INVALID_PARAMETER;
    }

    RegB.REG_B = ReadRtcIndex(RTC_REG_B_INDEX);
    
    SetUpdate(FALSE);
    if (Time != NULL) {
        WriteRtcIndex(RTC_HOURS_ALARM_REG, Time->Hour);
        WriteRtcIndex(RTC_MINUTES_ALARM_REG, Time->Minute);
        WriteRtcIndex(RTC_SECONDS_ALARM_REG, Time->Second);
#if ACPI_ALARM_DAY_CMOS
        // Day == 0 means don't care
        RegDateAlarm.DateAlarm = DecToBCD(Time->Day);
        WriteRtcIndex(ACPI_ALARM_DAY_CMOS, RegDateAlarm.REG_DATE_ALARM);
#endif
#if ACPI_ALARM_MONTH_CMOS
        // Month == 0 means don't care
        RegMonthAlarm.MonthAlarm = DecToBCD(Time->Month);
        WriteRtcIndex(ACPI_ALARM_MONTH_CMOS, RegMonthAlarm.REG_MONTH_ALARM);
#endif
    }

    // Clear Alarm Flag
    ReadRtcIndex(RTC_REG_C_INDEX);

    // Set Enable/Disable
    RegB.AlarmInt = (Enable) ? 1 : 0;
    WriteRtcIndex(RTC_REG_B_INDEX, RegB.REG_B);
    
    SetUpdate(TRUE);
    
// AMI_OVERRIDE_SCT000_START >>>
    //
    // Set the Y/M/D info to variable as it has no corresponding HW registers.
    //
    if (Enable) {
        RtcTime.Year     = Time->Year;
        RtcTime.TimeZone = Time->TimeZone;
        RtcTime.Daylight = Time->Daylight; 
        Status =  pRS->SetVariable (
                         L"RTCALARM",
                         &gEfiTimeVariableGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS | 
                         EFI_VARIABLE_RUNTIME_ACCESS | 
                         EFI_VARIABLE_NON_VOLATILE,
                         sizeof (RtcTime),
                         &RtcTime
                         );
    }

    return EFI_SUCCESS;
// AMI_OVERRIDE_SCT000_END <<<
}

/**
    This function is the entry point for this DXE Driver. This function
    make the runtime services in SMM.

        
    @param ImageHandle Image handle
    @param SystemTable pointer to the UEFI System Table

    
    @retval Return Status based on errors that occurred while waiting for time to expire.

**/
EFI_STATUS EFIAPI
SbRuntimeServiceLibConstructor (
  IN EFI_HANDLE			ImageHandle,
  IN EFI_SYSTEM_TABLE	*SystemTable
  )
{
    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_TIME        Time;
    UINT8           PmConB = MmioRead8 (SB_PMC_MMIO_REG(R_SB_PMC_GEN_PMCON_B));
    UINT8           PM_A2 = MmioRead8 (SB_PMC_MMIO_REG(R_SB_PMC_GEN_PMCON_A + 2));
    RTC_REG_D       RegD;
    BOOLEAN         RtcLostPower = FALSE;
#if defined CREATE_CMOS_BAD_HOB && CREATE_CMOS_BAD_HOB == 1      
    CMOS_BAD_HOB            *CmosBadHob;
#endif
    
    InitAmiLib(ImageHandle, SystemTable);

    PROGRESS_CODE(DXE_SBRUN_INIT);

    // CspLib PM Specific function to check and Report 
    // CMOS Battary and Power Supply Power loss/failure
    CspLibCheckPowerLoss();

//    if(PmConB & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) {
      // Clear RTC_PWR_STS.
//      PmConB &= ~B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS;
//      RtcLostPower = TRUE;
//    }
#if defined CREATE_CMOS_BAD_HOB && CREATE_CMOS_BAD_HOB == 1    
    CmosBadHob = GetEfiConfigurationTable(    //get hob list
                    pST,
                    &gEfiHobListGuid);  
 
    Status = FindNextHobByGuid(    //the hob exit when cmos is bad and creat hob success. 
                (EFI_GUID *)&gAmiCmosBadHobGuid,            
                (VOID **)&CmosBadHob);
                     
    if(!EFI_ERROR(Status)) {
    	RtcLostPower = !CmosBadHob->Health;
    }
#endif    
    //Write back Cleared Statuses
    MmioWrite8 (SB_PMC_MMIO_REG(R_SB_PMC_GEN_PMCON_A + 2), PM_A2);
    MmioWrite8 (SB_PMC_MMIO_REG(R_SB_PMC_GEN_PMCON_B), PmConB);

    RegD.REG_D = ReadRtcIndex(RTC_REG_D_INDEX);
    if (RegD.DataValid == 0)
      RtcLostPower = TRUE;

    if (RtcLostPower) {
      ERROR_CODE(DXE_SB_BAD_BATTERY, EFI_ERROR_MAJOR);
      InitRtc();
    }


    //MakeSure Mode, Format and REG_A is OK
    CheckRtc(TRUE);

    Status = AmiChipsetGetTime(&Time,NULL);

    if( EFI_ERROR(Status) || (!VerifyTime(&Time)))
    {
        EFI_TIME nt;

        ERROR_CODE(GENERIC_BAD_DATE_TIME_ERROR,  EFI_ERROR_MINOR);

        DEBUG((DEBUG_ERROR, "\n\nTime: %d/%d/%d  %d:%d:%d\n",
            Time.Month,
            Time.Day,
            Time.Year,
            Time.Hour,
            Time.Minute,
            Time.Second
        ));

        DEBUG((DEBUG_ERROR, "Nanosecond: %d TimeZone: %d\n\n\n",
            Time.Nanosecond,
            Time.TimeZone
        ));

        //if Time is invalid the battery probably has been removed
        //Let's setup RTC_REG_A just in case...
        WriteRtcIndex(RTC_REG_A_INDEX, 0x26);

        //Check to see what part of EFI_TIME was wrong.
        //reset unrelated to RTC fields.
        Time.TimeZone = EFI_UNSPECIFIED_TIMEZONE;
        Time.Daylight = 0;
        Time.Nanosecond = 0;

        nt = Time;

        nt.Hour   = 0;
        nt.Minute = 0;
        nt.Second = 0;

        if(VerifyTime(&nt)) {
            //if we here that means Time was wrong
            Time.Hour   = 0;
            Time.Minute = 0;
            Time.Second = 0;
        } else {
            //if we here that means Date was wrong
            Time.Month = DEFAULT_MONTH;
            Time.Day   = DEFAULT_DAY;
            Time.Year  = DEFAULT_YEAR;
        }
        //Here is the situation when both Time and Date is Incorrect.
        if(!VerifyTime(&Time)) {
            Time.Hour   = 0;
            Time.Minute = 0;
            Time.Second = 0;
            Time.Month = DEFAULT_MONTH;
            Time.Day   = DEFAULT_DAY;
            Time.Year  = DEFAULT_YEAR;
        }

        DEBUG((DEBUG_INFO, "Reseting Invalid Date and Time to: %d/%d/%d  %d:%d:%d\n\n",
            Time.Month,
            Time.Day,
            Time.Year,
            Time.Hour,
            Time.Minute,
            Time.Second
         ));
        AmiChipsetSetTime(&Time);
    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
