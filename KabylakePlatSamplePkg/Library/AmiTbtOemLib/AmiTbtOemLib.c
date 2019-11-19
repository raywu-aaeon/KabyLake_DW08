//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiTbtOemLib.c
    Provide lib function for OEM.

**/

#include <Token.h>
#include <AmiCspLib.h>
#include <Library/IoLib.h>
#include <Library/AmiChipsetIoLib.h>
#include <Library/AmiTbtOemLib.h>
#include <Library/PchInfoLib.h>
#include <Library/GpioLib.h>
#include <Library/PchPcieRpLib.h>
#include <Pci.h>

/**
  Pull High GPIO_3 (__FORCE_PWR)

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
PullHighTbtForcePowerPin (
  VOID
  )
{
/*  Sample code OEM porting required !!!
    UINT32 ForcePowerPin = 0;
    UINT32 RegVal;

    ForcePowerPin = (UINT32)((ConvertGpioGroup(TBT_FORCEPWR_GROUP) << 16) | TBT_FORCEPWR_PAD);
    GpioSetOutputValue(ForcePowerPin, 1);

    RegVal = MMIO_READ32(0xFDAE0728);
    DEBUG ((DEBUG_INFO, "ForcePowerPin: %x\n", RegVal));
//*/
}

/**
  Pull Low GPIO_3 (__FORCE_PWR)

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
PullDownTbtForcePowerPin (
  VOID
  )
{
/*  Sample code OEM porting required !!!
    UINT32 ForcePowerPin = 0;
    UINT32 RegVal;

    ForcePowerPin = (UINT32)((ConvertGpioGroup(TBT_FORCEPWR_GROUP) << 16) | TBT_FORCEPWR_PAD);
    GpioSetOutputValue(ForcePowerPin, 0);

    RegVal = MMIO_READ32(0xFDAE0728);
    DEBUG ((DEBUG_INFO, "ForcePowerPin: %x\n", RegVal));
//*/
}

/**
  Convert GPIO group setting by SDL setup value.

  @param  SdlSetting            Setting from SDL setup value

  @return                       GPIO value
**/
UINT16
ConvertGpioGroup (
  IN UINT8                      SdlSetting
  )
{
    PCH_SERIES      PchSeries = GetPchSeries();

    if (SdlSetting == 0xFF) return SdlSetting;

    switch(SdlSetting)
    {
        case 0:
            if(PchSeries == PchLp) return Lp_Gpio_GPP_A;
            else return H_Gpio_GPP_A;
            break;
        case 1:
            if(PchSeries == PchLp) return Lp_Gpio_GPP_B;
            else return H_Gpio_GPP_B;
            break;
        case 2:
            if(PchSeries == PchLp) return Lp_Gpio_GPP_C;
            else return H_Gpio_GPP_C;
            break;
        case 3:
            if(PchSeries == PchLp) return Lp_Gpio_GPP_D;
            else return H_Gpio_GPP_D;
            break;
        case 4:
            if(PchSeries == PchLp) return Lp_Gpio_GPP_E;
            else return H_Gpio_GPP_E;
            break;
        case 5:
            if(PchSeries == PchLp) return Lp_Gpio_GPP_F;
            else return H_Gpio_GPP_F;
            break;
        case 6:
            if(PchSeries == PchLp) return Lp_Gpio_GPP_G;
            else return H_Gpio_GPP_G;
            break;
        case 7:
            if(PchSeries == PchLp) return 0xFFFF;
            else return H_Gpio_GPP_H;
            break;
        case 8:
            if(PchSeries == PchLp) return 0xFFFF;
            else return H_Gpio_GPP_I;
            break;
        default:
            return 0xFFFF;
    }
}

/**
  This function delays for the number of micro seconds passed in

  @param  DelayTime             Number of microseconds(us) to delay
  @param  BaseAddr              The I/O base address of the ACPI registers

  @retval EFI_SUCCESS           Always return EFI_SUCCESS
**/
EFI_STATUS
TbtCountTime (
  IN UINTN                      DelayTime,
  IN UINT16                     BaseAddr  // Only needs to be 16 bit for I/O address
  )
{
// The following code is to generate delay for specified amount of micro
// seconds using ACPI timer.
    UINTN                       TicksNeeded;
    UINT32                      TimerValue;
    UINT32                      NewTimerValue;
    UINTN                       OverFlow;
    UINTN                       TheRest;
    UINTN                       EndValue;

    // Set up timer to point to the ACPI Timer register
    BaseAddr += R_SB_ACPI_PM1_TMR; // *** PORTING NEEDED

    // There are 3.58 ticks per us, so we have to convert the number of us
    // passed in to the number of ticks that need to pass before the timer has
    // expired convert us to Ticks, don't loose significant figures or as few
    // as possible do integer math in ticks/tens of ns and then divide by 100
    // to get ticks per us
    OverFlow = 0;

//#### TheRest = TicksNeeded = (DelayTime * 358) /100;
    TicksNeeded = DelayTime * 3;            //(DelayTime * 3)
    TicksNeeded += (DelayTime) / 2;         //(DelayTime * 5)/10
    TicksNeeded += (DelayTime * 2) / 25;        //(DelayTime * 8)/100
    TheRest = TicksNeeded;

    // 32 bits corresponds to approz 71 mins  no delay should be that long
    // otherwise get the number of times the counter will have to overflow
    // to delay as long as needed
    if (NUM_BITS_IN_ACPI_TIMER < MAX_ACPI_TIMER_BITS) {
        OverFlow = TicksNeeded / (1 << NUM_BITS_IN_ACPI_TIMER);
        TheRest = TicksNeeded % (1 << NUM_BITS_IN_ACPI_TIMER);
    }

    // Read ACPI Timer
    TimerValue = IoRead32( BaseAddr );

    // Need to adjust the values based off of the start time
    EndValue = TheRest + TimerValue;

    // Check for overflow on addition.  possibly a problem
    if (EndValue < TimerValue) {
        OverFlow++;
    } else {
        if (NUM_BITS_IN_ACPI_TIMER < MAX_ACPI_TIMER_BITS) {
            // Here make sure that EndValue is less than the max value
            // of the counter
            OverFlow += EndValue / (1 << NUM_BITS_IN_ACPI_TIMER);
            EndValue = EndValue % (1 << NUM_BITS_IN_ACPI_TIMER);
        }
    }

    // Let the timer wrap around as many times as calculated
    while (OverFlow) {
        // read timer amd look to see if the new value read is less than
        // the current timer value.  if this happens the timer overflowed
        NewTimerValue = IoRead32(BaseAddr);

        if (NewTimerValue < TimerValue) OverFlow--;

        TimerValue = NewTimerValue;
    }

    // Now wait for the correct number of ticks that need to occur after
    // all the needed overflows
    while (EndValue > TimerValue) {
        NewTimerValue = IoRead32(BaseAddr);

        // check to see if the timer overflowed.  if it did then
        // the time has elapsed. Because EndValue should be greater than
        // TimerValue
        if (NewTimerValue < TimerValue) break;

        TimerValue = NewTimerValue;
      }

    return EFI_SUCCESS;
}

/**
  This snipped code contains PCIE2TBT <-> TBT2PCIE handshake
  procedure and all related methods called directly or underectly
  by TbtSetPCIe2TBTCommand.
  This function is Intel Sample code(Rev. 1.5).

  @param  UpPortBus             Thunderbolt host upstream bridge bus number
  @param  Data                  Thunderbolt FW setting data
  @param  Command               Thunderbolt FW setting command
  @param  Timeout               Timeout value

  @retval TRUE                  Set Thunderbolt FW success
  @retval FALSE                 Set Thunderbolt FW fail
**/
BOOLEAN
AmiTbtSetPCIe2TBTCommand(
  IN UINT8                      UpPortBus,
  IN UINT8                      Data,
  IN UINT8                      Command,
  IN UINTN                      Timeout
  )
{
    UINT32                      REG_VAL = 0;
    UINTN                       Counter = Timeout;

    REG_VAL = (Data << 8) | Command | PCIE2TBT_VLD_B;

    MMIO_WRITE32(TBT_CFG_ADDRESS(UpPortBus, 0, 0, PCIE2TBT_R), REG_VAL);

    IoWrite8 (0x80, 0x50);

    while(Counter-- > 0){
        // BIOS support of Thunderbolt devices Specification Update Revision 0.9
        REG_VAL = MMIO_READ32(TBT_CFG_ADDRESS(UpPortBus, 0, 0, TBT2PCIE_R));
        if (0xFFFFFFFF == REG_VAL){
            // Device is not here return now
            return FALSE;
        }
        if (REG_VAL & TBT2PCIE_DON_R){
            // Check TBT2PCIE.Done
            break;
        }
    }
    MMIO_WRITE32(TBT_CFG_ADDRESS(UpPortBus, 0, 0, PCIE2TBT_R), 0);

    return TRUE;
}

/**
  Get Thunderbolt host series number

  @param  VOID                  No parameter

  @return                       Thunderbolt host series number
**/
UINT8
GetTbtHrInfo (
  VOID
  )
{
    UINT8                       TbtHostSeries = DEFAULT_TBT_CHIP;
/* Sample code
    UINTN        TbtBus = TBT_UP_PORT_BUS;
    UINTN        TbtDev = TBT_UP_PORT_DEV;
    UINTN        TbtFun = TBT_UP_PORT_FUN;
    UINT32       HRID;
    BOOLEAN      CMD;

    // Get correct default thunderbolt host location
    GetPchPcieRpDevFun ((DEFAULT_TBT_AIC_LOCATION - 1), &TbtDev, &TbtFun);
    DEBUG ((DEBUG_INFO, "TBT default host location: %x %x\n", TbtDev, TbtFun));

    // Pull High GPIO_3(__FORCE_PWR)
    PullHighTbtForcePowerPin();

    // Delay 100 ms
    TbtCountTime(100000, PM_BASE_ADDRESS);

    // Assign temp bus
    MMIO_WRITE32(TBT_CFG_ADDRESS(TbtBus, TbtDev, TbtFun, PCI_PBUS), 0x00050500);

    // Do a dummy Write
    MMIO_WRITE32(TBT_CFG_ADDRESS(5, 0, 0, PCI_VID), 0x12345678);

    // Write OS_UP commond
    CMD = AmiTbtSetPCIe2TBTCommand(5, 0, PCIE2TBT_OS_UP, 0x8FFFF);
    DEBUG ((DEBUG_INFO, "TBT set command return: %x\n", CMD));

    // Delay 600 ms
    TbtCountTime(600000, PM_BASE_ADDRESS);

    // Get HR Info
    HRID = MMIO_READ16(TBT_CFG_ADDRESS(5, 0, 0, PCI_DID));
    DEBUG ((DEBUG_INFO, "TBT host ID: %x\n", HRID));
    switch(HRID){
        case RR_HR_2C:
        case RR_HR_4C:
            TbtHostSeries = Redwood_Ridge;
            break;
        case FR_HR_2C:
        case FR_HR_4C:
            TbtHostSeries = Falcon_Ridge;
            break;
        case WR_HR_2C:
            TbtHostSeries = BDW_TBT_LP;
            break;
        case AR_HR_2C:
        case AR_HR_4C:
        default:
            TbtHostSeries = Alpine_Ridge;
    }

    // Reset Thunderbolt security level to match BIOS setting.
    AmiTbtSetPCIe2TBTCommand(5, (DEFAULT_TBT_SECURITY_LEVEL - 1), PCIE2TBT_SET_SECURITY_LEVEL, 0x008FFFFF);

    // Pull Low GPIO_3(__FORCE_PWR)
    PullDownTbtForcePowerPin();

    // Delay 100 ms
    TbtCountTime(100000, PM_BASE_ADDRESS);

    // Remove temp bus
    MMIO_WRITE32(TBT_CFG_ADDRESS(TbtBus, TbtDev, TbtFun, PCI_PBUS), 0xFF000000);
//*/
    return TbtHostSeries;
}

/**
  When entering Setup page, double check Security Level setting
  is same or not between Thunderbolt host Fw and BIOS.

  This function only work for Thunderbolt Redwood Ridge based
  host chip

  @param  BiosSecurityLevel     BIOS thunderbolt security level setting
  @param  TbtHostLocation       Thunderbolt host location

  @retval 0                     Security Level synchnized without change
  @retval 1                     Security Level synchnized with programming again
  @retval 2                     Error
**/
UINT8
SynchSecurityLevel(
  IN UINT8                      BiosSecurityLevel,
  IN UINT8                      TbtHostLocation
  )
{
    UINT8            SynchState = 0;
// Sample code
/*
    UINT8           TbtHrbus;
    UINT8           PWRFlag = 0;
    UINTN           TbtBus = TBT_UP_PORT_BUS;
    UINTN           TbtDev = TBT_UP_PORT_DEV;
    UINTN           TbtFun = TBT_UP_PORT_FUN;
    UINT32          RegVal;
    BOOLEAN         CmdDone;

    // Get Thunderbolt Host Router Location
    GetPchPcieRpDevFun ((TbtHostLocation - 1), &TbtDev, &TbtFun);

    TbtHrbus = (UINT8)((MMIO_READ16(TBT_CFG_ADDRESS(TbtBus, TbtDev, TbtFun, PCI_PBUS)) & 0xFF00) >> 8);

    // Check Thunderbolt Host state
    RegVal = MMIO_READ32(TBT_CFG_ADDRESS(TbtHrbus, 0, 0, PCI_RID));
    if (RegVal == 0xFFFFFFFF){
        // Pull high GPIO_3(__FORCE_PWR) pin
        PullHighTbtForcePowerPin();

        // Delay 50ms
        TbtCountTime(50000, PM_BASE_ADDRESS);
        PWRFlag = 1;
    }

    // Do PCIE2TBT handshake to get Thunderbolt FW security level
    CmdDone = AmiTbtSetPCIe2TBTCommand(TbtHrbus, 0, PCIE2TBT_GET_SECURITY_LEVEL, 0x008FFFFF);

    if (CmdDone){
        RegVal = MMIO_READ32(TBT_CFG_ADDRESS(TbtHrbus, 0, 0, TBT2PCIE_R));

        if ((RegVal & MASK_ERROR) == 0){
            RegVal = (RegVal & MASK_DATA) >> 8;
        }
        else SynchState = 2;
    } else SynchState = 2;
    // So far, RegVal variable might be:
    // 1: 0xFFFFFFFF
    // 2: Thunderbolt host Revision ID and Class Code
    // 3: Thunderbolt host Fw security level setting

    // check Security Level setting between Thunderbolt Fw and BIOS
    if ((UINT8)RegVal != (BiosSecurityLevel - 1)){
        if (PWRFlag == 0){
            // Pull high GPIO_3(__FORCE_PWR) pin
            PullHighTbtForcePowerPin();

            PWRFlag = 1;
        }
        IoWrite8(0x80, (BiosSecurityLevel - 1 + 0xC0));
        // After testing, TBT Fw needs Delay 600ms
        TbtCountTime(600000, PM_BASE_ADDRESS);

        // Re-config Security Level with BIOS setting
        CmdDone = AmiTbtSetPCIe2TBTCommand(TbtHrbus, (BiosSecurityLevel - 1), PCIE2TBT_SET_SECURITY_LEVEL, 0x008FFFFF);

        if (CmdDone)  SynchState = 1;
        else SynchState = 2;
    }

    if (PWRFlag == 1){
        // Pull low GPIO_3(__FORCE_PWR) pin
        PullDownTbtForcePowerPin();

        // Delay 100ms
        TbtCountTime(100000, PM_BASE_ADDRESS);
    }
//*/
    return SynchState;
}

/**
  This function is configure the Thunderbolt security level.
  OEM Porting required !!!

  @param  TbtSecurityLevel      BIOS Thunderbolt security level setting
  @param  TBTHostSeries         Thunderbolt host chip series
  @param  TbtHostLocation       Thunderbolt host location
  @param  IsPei                 Flag to point out now is in PEI phase or not

  @return                       No return value
**/
VOID
ProgramTbtSecurityLevel (
  IN UINT8                      *TbtSecurityLevel,
  IN UINT8                      TBTHostSeries,
  IN UINT8                      TbtHostLocation,
  IN BOOLEAN                    IsPei
  )
{
    BOOLEAN                     TbtHostCmd = FALSE;
    UINTN                       TbtBus     = TBT_UP_PORT_BUS;
    UINTN                       TbtDev     = TBT_UP_PORT_DEV;
    UINTN                       TbtFun     = TBT_UP_PORT_FUN;
    UINT8                       SecLevel   = *TbtSecurityLevel;
    UINT8                       TbtHRbus;

    if (!IsPei){
        //Get Thunderbolt Host Router Location
        GetPchPcieRpDevFun ((TbtHostLocation - 1), &TbtDev, &TbtFun);

        TbtHRbus = (UINT8)((MMIO_READ16(TBT_CFG_ADDRESS(TbtBus, TbtDev, TbtFun, PCI_PBUS)) & 0xFF00) >> 8);
        DEBUG ((DEBUG_INFO, "TBT host location: D:%x|F:%x first bridge @B:%x\n", TbtDev, TbtFun, TbtHRbus));
        IoWrite8(0x80, (0xB0 | TbtHRbus));
        TbtCountTime(50000, PM_BASE_ADDRESS);

        // First pull high GPIO_3(__FORCE_PWR) pin
        PullHighTbtForcePowerPin();

        IoWrite8(0x80, (0x80 | (SecLevel - 1)));
        TbtCountTime(500000, PM_BASE_ADDRESS);

        // Do PCIE2TBT handshake
        TbtHostCmd = AmiTbtSetPCIe2TBTCommand(TbtHRbus, (SecLevel - 1), PCIE2TBT_SET_SECURITY_LEVEL, 0x008FFFFF);

        //for debug
        if(TbtHostCmd){
            IoWrite8(0x80, 0x5D);
            TbtCountTime(100000, PM_BASE_ADDRESS);
        }

        // Finally pull low GPIO_3(__FORCE_PWR) pin
        PullDownTbtForcePowerPin();
    } // Not at PEI phase
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
