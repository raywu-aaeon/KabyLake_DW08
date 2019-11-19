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

/** @file AmiSbMiscLib.c
    South Bridge Miscellaneous functions Library implementation

*/

#include <Library/AmiSbMiscLib.h>
#include <Library/AmiGpioLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Sb.h>
#include <Library/PcdLib.h>
#include <Library/GpioLib.h>
#include <AmiChipsetIoLib.h>
#include <AmiCspLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchInfoLib.h>
#include <PchAccess.h>
#include <SbElinks.h>
#include <PciBus.h>

#if Capsule_SUPPORT
#include <Capsule.h>    //CAPSULE20
#include <Guid/CapsuleVendor.h>
#endif

#if defined(POST_TIME_TUNING_SUPPORT) && POST_TIME_TUNING_SUPPORT
#include <FastBoot2.h>
#endif

#if defined Capsule_SUPPORT && Capsule_SUPPORT == 1
typedef VOID (SB_OEM_S3_WARMRESET_CALLBACK) (VOID);
extern SB_OEM_S3_WARMRESET_CALLBACK \
                                 SB_OEM_S3_WARMRESET_CALLBACK_LIST EndOfList;
SB_OEM_S3_WARMRESET_CALLBACK* SbS3InsteadOfWarmResetCallBackList[] = \
                                    {SB_OEM_S3_WARMRESET_CALLBACK_LIST NULL};
#endif

typedef enum {
    SystemWarmBoot = 1,                 ///< Warm boot
    SystemS3Resume,                     ///< S3 resume
    SystemBootModeMax = 0xFF            ///< 
} SbWarmBootType;

/**
    This function Shuts the system down (S5)

    @param VOID

    @retval VOID

**/
VOID SbLibShutdown (
    VOID
)
{
    //Check if Sleep SMI is enabled we will disable it.
    RESET_IO16_PM(R_PCH_SMI_EN, BIT04);  // 0x30

    //Clear All PM Statuses
    WRITE_IO16_PM(R_PCH_ACPI_PM1_STS, \
           READ_IO16_PM(R_PCH_ACPI_PM1_STS)); // 0x00

    //Go to S5
    SET_IO16_PM(R_PCH_ACPI_PM1_CNT, 0x0f << 10); // 0x04

    EFI_DEADLOOP()
}

/**
    This function is the extended reset call interface function
    provided by SB.

    @param ResetType - The extended type of reset to be generated

    @retval SYSTEM RESET
**/

VOID SbLibExtResetSystem (
    IN SB_EXT_RESET_TYPE  ResetType
)
{
    //  UINT16              GpioBase;
    UINT8               OutputData;
    UINTN               PmcBaseAddress;
    UINT32              PchPwrmBase;


    //  GpioBase = READ_PCI16_SB (R_PCH_LPC_GPIO_BASE) & B_PCH_LPC_GPIO_BASE_BAR;
    PmcBaseAddress = SB_PCIE_CFG_ADDRESS (
                         DEFAULT_PCI_BUS_NUMBER_PCH,
                         PCI_DEVICE_NUMBER_PCH_PMC,
                         PCI_FUNCTION_NUMBER_PCH_PMC, 0
                         );
    PchPwrmBaseGet (&PchPwrmBase);

      // Issue some delay before system reset.
#if SB_STALL_PPI_SUPPORT
    CountTime(4000, PM_BASE_ADDRESS); // 1ms
#endif  // #if SB_STALL_PPI_SUPPORT

    // Disable All SMI Enables, include the Global SMI.
    WRITE_IO32_PM(R_PCH_SMI_EN, 0); // 0x30

    switch (ResetType) {
        case SbResetFull:
        case SbResetGlobal:
        case SbResetGlobalWithEc:
          /*
          WRITE_IO32 ((GpioBase + R_PCH_GP_RST_SEL), 0);
          WRITE_IO32 ((GpioBase + R_PCH_GP_RST_SEL2), 0);
          WRITE_IO32 ((GpioBase + R_PCH_GP_RST_SEL3), 0);
          */

          if ((ResetType == SbResetGlobal) || (ResetType == SbResetGlobalWithEc))
              SET_MEM32 (
                PmcBaseAddress + R_PCH_PMC_ETR3,
                (UINT32) (B_PCH_PMC_ETR3_CF9GR)
                );

          if ((ResetType == SbResetGlobalWithEc) &&
                ((READ_MEM32 (PchPwrmBase + R_PCH_PWRM_S4_PWRGATE_POL) &
                 (B_PCH_PWRM_S4AC_GATE_SUS | B_PCH_PWRM_S4DC_GATE_SUS)) == 0) &&
                ((READ_MEM32 (PchPwrmBase + R_PCH_PWRM_S5_PWRGATE_POL) &
                 (B_PCH_PWRM_S5AC_GATE_SUS | B_PCH_PWRM_S5DC_GATE_SUS)) == 0)) {
            if (AmiGetSbSeries() == SbH) {
              /// 1. Set GPIOBASE + offset 00h[30] = 1b (for non-Deep Sx enabled platforms)
              /// 2. Set GPIOBASE + offset 04h[30] = 0b (for non-Deep Sx enabled platforms)
              /// 3. Set GPIOBASE + offset 0Ch[30] = 0b (for non-Deep Sx enabled platforms)
              /// 4. Set GPIOBASE + offset 60h[30] = 1b (for non-Deep Sx enabled platforms)
              /// NOTE: For Deep Sx enabled platforms steps 1,2 and 3 should be skipped and pin should be left in native mode
              /// 5. Set CF9GR bit, D31:F0:ACh[20], issue a Global Reset through a 0xCF9 write of either 06h or 0Eh commands.
              /// Global Reset MEI Message
              /// 1. BIOS makes sure GPIO30 is left in native mode (default mode) before sending a Global Reset MEI message.
              ///
              /*
              SET_IO32(GpioBase + R_PCH_GPIO_USE_SEL, BIT30);
              RESET_IO32(GpioBase + R_PCH_GPIO_IO_SEL, BIT30);
              RESET_IO32(GpioBase + R_PCH_GPIO_LVL, BIT30);
              */
            }

            if (AmiGetSbSeries() == SbH) {
              /*
              SET_IO32(GpioBase + R_PCH_GP_30_CONFIG0, B_PCH_GPIO_OWN0_GPIO_USE_SEL);
              RESET_IO32(GpioBase + R_PCH_GP_30_CONFIG0, B_PCH_GPIO_OWN0_GPIO_IO_SEL);
              RESET_IO32(GpioBase + R_PCH_GP_30_CONFIG0, B_PCH_GPIO_OWN0_GPO_LVL);
              */
            }
            //SET_IO32(GpioBase + R_PCH_GP_RST_SEL, BIT30);
          }
              
          OutputData = V_PCH_RST_CNT_FULLRESET;
          break;

        default:
          return;
    }

    WRITE_IO8 (R_PCH_RST_CNT, OutputData);

      // We should never get this far
    EFI_DEADLOOP();
}

#if SB_RESET_PPI_SUPPORT
#if Capsule_SUPPORT == 1
/**
    This function calls registered callbacks for S3 RTC/Timer
    (WarmBoot) eLink.

    @param VOID

    @retval VOID
**/
VOID SBLib_S3InsteadOfWarmResetCallBack (VOID)
{
    UINTN   i;

    for (i = 0; SbS3InsteadOfWarmResetCallBackList[i] != NULL; i++) 
        SbS3InsteadOfWarmResetCallBackList[i]();
}

/**
    This function puts system into ACPI S3 State.
    if token ENABLE_RTC_ONE_SECOND_WAKEUP = 1, then it setups RTC
    1 second alarm as well.

    @param VOID

    @retval None, system will enter ACPI S3 State.
**/

VOID SBLib_S3InsteadOfWarmReset (VOID)
{
#if ENABLE_RTC_ONE_SECOND_WAKEUP
    UINT8       Hour;
    UINT8       Minute;
    UINT8       Second;
    BOOLEAN     InBCD = TRUE;
#endif
    
    // Do any specific porting if needed.
    SBLib_S3InsteadOfWarmResetCallBack();
    
#if ENABLE_RTC_ONE_SECOND_WAKEUP    
    // Determine if RTC is in BCD mode
    if ( READ_IO8_RTC(0x0b) & 0x4 ) InBCD = FALSE; // Bit 2
        
    // Wait for time update to complete before reading the values,
    // while bit 7 is set the time update is in progress.
    while( READ_IO8_RTC(0x0a) & 0x80 );

    // Read current hour, minute, second
    Hour = READ_IO8_RTC(0x04); // Hours
    Minute = READ_IO8_RTC(0x02); // Minutes
    Second = READ_IO8_RTC(0x00); // Seconds

    // Convert second to decimal from BCD and increment by 1
    if (InBCD) Second = (Second >> 4) * 10 + (Second & 0x0F);
    Second += 3;
    
    if (Second > 59) {
        Second -= 60;
        if (InBCD) Minute = (Minute >> 4) * 10 + (Minute & 0x0F);
        Minute++;
        if (Minute > 59){
            Minute = 0;
            if (InBCD) Hour = (Hour >> 4) * 10 + (Hour & 0x0F);
            Hour++;
            // Check 24 hour mode/12 hour mode, Bit1 1=24hour else 12 hour
            if ( READ_IO8_RTC(0x0b) & 0x2 ) {
                if(Hour > 23) Hour = 0;
            } else {
                if(Hour > 11) Hour = 0;
            }

            if (InBCD) Hour = Hour % 10 + ( (Hour / 10) << 4 ) ;
        }

        if (InBCD) Minute = Minute % 10 + ( (Minute / 10) << 4 ) ;
    }

    // Convert from decimal to BCD
    if (InBCD) Second = Second % 10 + ( (Second / 10) << 4 ) ;
    
    // Set the alarm
    WRITE_IO8_RTC(0x05, Hour); // Hours Alarm
    WRITE_IO8_RTC(0x03, Minute); // Minutes Alarm
    WRITE_IO8_RTC(0x01, Second); // Seconds Alarm

    // Clear date alarm.
    WRITE_IO8_RTC(0x0D, READ_IO8_RTC(0x0D) & BIT7);

    // Enable the alarm
    SET_IO8_RTC(0x0b, 0x20); // Bit 5

    if (READ_IO16_PM(R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_RTC) {
      READ_IO8_RTC(0x8C);
      WRITE_IO16_PM(R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_RTC);  // 0x00
    }

    // Set RTC_EN bit in PM1_EN to wake up from the alarm
    SET_IO16_PM(R_PCH_ACPI_PM1_EN, B_PCH_ACPI_PM1_EN_RTC ); // 0x02
#endif

    // Enable Sleep SMI for all S3 sleep SMI callback functions.
    SET_IO32_PM(R_SB_SMI_EN, BIT04); // 0x30
    // Triger S3 sleep callback functions.
    RW_IO32_PM(R_PCH_ACPI_PM1_CNT, V_PCH_ACPI_PM1_CNT_S3, V_PCH_ACPI_PM1_CNT_S5 ); // 0x04
    SET_IO32_PM(R_PCH_ACPI_PM1_CNT, B_PCH_ACPI_PM1_CNT_SLP_EN ); // 0x04
}
#endif
#endif

#if SB_RESET_PPI_SUPPORT

/**
    This function is the reset call interface function published
    by the reset PPI

    @param ResetType Type of reset to be generated

    @retval VOID

**/
VOID SBLib_ResetSystem (
    IN  EFI_RESET_TYPE  ResetType
)
{
    UINT8               OutputData;
    UINT32              Data32;
    UINT16              Data16;
    UINTN               PmcBaseAddress;
    UINT16              PmBase;
    UINT32              OcWdtCtlReg;
  #if defined Capsule_SUPPORT && Capsule_SUPPORT == 1
    EFI_GUID        SbWarmResetGuid = SB_WARM_RESET_GUID;
    CHAR16          SbWarmResetVar[] = SB_WARM_RESET_VARIABLE;
    UINT32          SbWarmResetFlag = SB_WARM_RESET_TAG;
    EFI_GUID        gEfiCapsuleVendorGuid = EFI_CAPSULE_VENDOR_GUID;
    EFI_PHYSICAL_ADDRESS  IoData;
    UINTN           Size = sizeof(EFI_PHYSICAL_ADDRESS);
  #endif

    PmcBaseAddress = SB_PCIE_CFG_ADDRESS (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_PMC,
                       PCI_FUNCTION_NUMBER_PCH_PMC, 0
                       );
    
    PmBase = READ_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, 
                        PCI_DEVICE_NUMBER_PCH_PMC, 
                        PCI_FUNCTION_NUMBER_PCH_PMC, 
                        R_PCH_PMC_ACPI_BASE) & B_PCH_PMC_ACPI_BASE_BAR;
    
    // Set WDT to allow known reset
    OcWdtCtlReg = IoRead32(PmBase + R_PCH_OC_WDT_CTL);
    OcWdtCtlReg &= ~(B_PCH_OC_WDT_CTL_UNXP_RESET_STS | B_PCH_OC_WDT_CTL_FORCE_ALL);
    IoWrite32(PmBase + R_PCH_OC_WDT_CTL, OcWdtCtlReg);

    switch (ResetType) {
      case EfiResetWarm:
#if defined Capsule_SUPPORT && Capsule_SUPPORT == 1
      if (pRS->GetVariable(EFI_CAPSULE_VARIABLE_NAME, &gEfiCapsuleVendorGuid, NULL, &Size, &IoData) == EFI_SUCCESS) {
          pRS->SetVariable( SbWarmResetVar, \
                            &SbWarmResetGuid, \
                            EFI_VARIABLE_NON_VOLATILE | \
                            EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                            EFI_VARIABLE_RUNTIME_ACCESS, \
                            sizeof(SbWarmResetFlag), \
                            &SbWarmResetFlag );
// EIP[448389] >>>>
#if defined WARM_RESET_S3_SUPPORT && WARM_RESET_S3_SUPPORT == 1
// EIP[448389] <<<<
        SBLib_S3InsteadOfWarmReset();
        return;
// EIP[448389] >>>>
#else
        //
        // Flush cache into memory before we go to sleep. It is necessary for WarmReset
        // because the changes in gpFwCapsuleMailbox are only in cache now
        //
        AsmWbinvd (); 
#endif
// EIP[448389] <<<<
      }
#endif
        WRITE_IO8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTSTARTSTATE);
        // Follow Intel PchResetCommon.c PchReset() to change WarmReset from 04 to 06 
        OutputData  = V_PCH_RST_CNT_HARDRESET;
        break;

      case EfiResetCold:
        WRITE_IO8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDSTARTSTATE);
                                          // [EIP81593]>
  #if defined COLD_RESET_WITH_POWER_CYCLE && \
              COLD_RESET_WITH_POWER_CYCLE == 1
        OutputData  = V_PCH_RST_CNT_FULLRESET;
  #else
        OutputData  = V_PCH_RST_CNT_HARDRESET;
  #endif
                                          // <[EIP81593]
        break;

      case EfiResetShutdown:
        //
        // Firstly, ACPI decode must be enabled
        //
        SET_MEM8 (
          PmcBaseAddress + R_PCH_PMC_ACPI_CNT,
          (UINT8) (B_PCH_PMC_ACPI_CNT_ACPI_EN)
          );

        //
        // Then, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
        //
        WRITE_IO32 ((UINTN) (PmBase + R_PCH_ACPI_GPE0_EN_127_96), 0);

        //
        // Secondly, PwrSts register must be cleared
        //
        // Write a "1" to bit[8] of power button status register at
        // (PM_BASE + PM1_STS_OFFSET) to clear this bit
        //
        Data16 = B_PCH_SMI_STS_PM1_STS_REG;
        WRITE_IO16 ((UINTN) (PmBase + R_PCH_SMI_STS), Data16);

        //
        // Finally, transform system into S5 sleep state
        //
        Data32  = READ_IO32 ((UINTN) (PmBase + R_PCH_ACPI_PM1_CNT));
        Data32  = (UINT32) ((Data32 &~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);
        WRITE_IO32 ((UINTN) (PmBase + R_PCH_ACPI_PM1_CNT), Data32);
        Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;
        WRITE_IO32 ((UINTN) (PmBase + R_PCH_ACPI_PM1_CNT), Data32);

        return;

      default:
        SbLibExtResetSystem(ResetType);
        return;
    }

    WRITE_IO8 (R_PCH_RST_CNT, OutputData);
    //
    // Waiting for system reset
    //
    EFI_DEADLOOP ()
}

#endif

/**
    This function determines if the system should boot with the
    default configuration. 

    @param PeiServices Pointer to the PEI services table
    @param ReadVariablePpi Pointer to the Read Variable#2 PPI
                           (The pointer can be used to read and 
                           enumerate existing NVRAM variables)

    @retval TRUE Firmware will boot with default configuration.

    @note  1. If boot with default configuration is detected, default
                 values for NVRAM variables are used.
              2. Normally we have to check RTC power status or CMOS clear
                 jumper status to determine whether the system should boot
                 with the default configuration.
**/
BOOLEAN SbIsDefaultConfigMode (
    IN EFI_PEI_SERVICES                 **PeiServices,
    IN EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariablePpi )
{
    UINT8           Buffer8;

    Buffer8 = READ_PCI8(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B); // 0xA4     

    return (Buffer8 & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) ? TRUE : FALSE;
}

#if SB_STALL_PPI_SUPPORT

/**
    This function delays for the number of micro seconds passed in

    @param DelayTime Number of microseconds(us) to delay
    @param BaseAddr The I/O base address of the ACPI registers

    @retval EFI_STATUS based on errors that occurred while waiting
                       for time to expire.

**/
EFI_STATUS CountTime (
    IN  UINTN   DelayTime,
    IN  UINT16  BaseAddr // only needs to be 16 bit for I/O address
)
{
    // The following code is to generate delay for specified amount of micro
    // seconds using ACPI timer.
    UINTN           TicksNeeded;
    UINT32          TimerValue;
    UINT32          NewTimerValue;
    UINTN           OverFlow;
    UINTN           TheRest;
    UINTN           EndValue;

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

#endif

// End Generic Flash part porting hooks

/**
    This function returns SMI state
              
    @param VOID

    @retval TRUE SMI enabled, FALSE - otherwise

    @note  This function must work at runtime. Do not use boot time services/protocols

**/
BOOLEAN SbLib_GetSmiState(
    VOID
)
{
#if SMM_SUPPORT
    static BOOLEAN    OriginalSmiState = FALSE;
    static BOOLEAN    HadBeenCame = FALSE;
    volatile BOOLEAN  SmiState = (READ_IO32_PM(R_PCH_SMI_EN) & 1) ? \
                                                                 TRUE : FALSE;

    if (HadBeenCame && SmiState) HadBeenCame = FALSE;

    if (HadBeenCame) {
        SmiState = OriginalSmiState;
    } else {
        OriginalSmiState = SmiState;
        HadBeenCame = TRUE;
    }

    return SmiState;
#else
    return FALSE;
#endif
}

/**
    This function disables SMI
              
    @param VOID

    @retval VOID

    @note  This function should be used ONLY in critical parts of code
           This function must work at runtime. Do not use boot time services/protocols

**/
VOID SbLib_SmiDisable(
    VOID
)
{
#if SMM_SUPPORT
    UINT32  SmiCtl = IoRead32 (PM_BASE_ADDRESS + 0x30) ;
//----------------------------------
    SmiCtl &= (~BIT1); //Mask OFF EOS bit
    IoWrite32 ( PM_BASE_ADDRESS + 0x30, SmiCtl & (~BIT0));
#endif
}

/**
    This function enables SMI
              
    @param VOID

    @retval VOID

    @note  This function should be used ONLY in critical parts of code
           This function must work at runtime. Do not use boot time services/protocols

**/
VOID SbLib_SmiEnable(
    VOID
)
{
#if SMM_SUPPORT
    SET_IO32_PM(R_PCH_SMI_EN, 2); // 0x30
    SET_IO32_PM(R_PCH_SMI_EN, 1); // 0x30
#endif
}

/**
    This function is PM Specific function to check and Report to
    the System Status Code - CMOS Battery and Power Supply Power 
    loss/failure.

    @param VOID

    @retval VOID

**/
VOID CspLibCheckPowerLoss(
    VOID
)
{
#if  AMI_CSP_DXE_SUPPORT
    SB_POWER_LOSS_DATA  SbPwrLossData;

    SbPwrLossData.PowerLossData = PcdGet16(PcdSbPwrLossData) & PcdGet16(PcdSbPwrLossDataMask);
    
    if(SbPwrLossData.PwrOkFail) ERROR_CODE(0x00,  EFI_ERROR_MAJOR);

    //-------------------------------------------------------
    //this bit means it was a power loss on CPU VRM well 
    // during S0 or S1, cleared by writing 0 to it
    if(SbPwrLossData.CpuPwrFail) ERROR_CODE(0x01,  EFI_ERROR_MAJOR);

    //-------------------------------------------------------
    //this bit means that S4 signal assertion was not long 
    //enough, cleared by writing 1 to it
    if(SbPwrLossData.SlpS4Fail) ERROR_CODE(0x02,  EFI_ERROR_MINOR);

    //-------------------------------------------------------
    //this bit means that a power was shut down because of 
    //CPU Power Trip, cleared by writing 1 to it
    if(SbPwrLossData.ThermTripSts) ERROR_CODE(GENERIC_CPU_THERMAL_ERROR, EFI_ERROR_MAJOR);
    
    //-------------------------------------------------------
    //this bit means that RESET# was active 
    //cleared by writing 1 to it
    if(SbPwrLossData.ResetSts) ERROR_CODE(0x04,  EFI_ERROR_MINOR);

    //-------------------------------------------------------
    //this bit means that Battery Bad or was removed
    //cleared by writing 1 to it
    if(SbPwrLossData.PwrFail) ERROR_CODE(0x05,  EFI_ERROR_MAJOR);

    if(SbPwrLossData.RtcLostPower){
        #if defined(SB_BAD_BATTERY_ERR_CODE_SUPPORT) && (SB_BAD_BATTERY_ERR_CODE_SUPPORT != 0)
        ERROR_CODE(DXE_SB_BAD_BATTERY,  EFI_ERROR_MAJOR);
        #endif
    }
#endif
}

/**
    Enable or disable SB common functions.

    @param SbDevType - The type of SB device, refer to SB_DEV_TYPE
    @param Enable - TRUE: Enable / FALSE: Disable

    @retval VOID

**/
VOID SbEnableDisableFunctions (
    IN SB_DEV_TYPE  SbDevType,
    IN BOOLEAN      Enable
)
{
    UINTN   DevBase;
    
    if (SbDevType >= SB_DEV_MAX) return;
    
    switch(SbDevType)
    {
        case SB_DEV_LAN:
            // Before modifying LAN Disable bit, make sure it's not locked.
            if (MmioRead32 (PWRM_BASE_ADDRESS + R_SB_PWRM_ST_PG_FDIS_PMC_1) & B_SB_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK) {
                return;
            }
            if (Enable){
                // Enable the Internal GbE Controller
                // Set PWRMBASE + 620h[0] = 0b to enable GbE controller
                MmioAnd32 (PWRM_BASE_ADDRESS + R_SB_PWRM_ST_PG_FDIS_PMC_1, (UINT32) ~B_SB_PWRM_ST_PG_FDIS_PMC_1_GBE_FDIS_PMC);
                // Read back for posted write to take effect
            } else {
                // Disable the Internal GbE Controller
                
                DevBase = SB_PCIE_CFG_ADDRESS(SB_BUS, SB_LAN_DEV, SB_LAN_FUN, 0);
                
                // Set LANDISCTRL, GbE PCI config + 0xA0 bit[0] to 1
                MmioOr32 (DevBase + R_SB_LAN_LANDISCTRL, B_SB_LAN_LANDISCTRL_DISABLE);
                
                // Set LOCKLANDIS, GbE PCI config + 0xA4 bit[0] to 1
                MmioOr32 (DevBase + R_SB_LAN_LOCKLANDIS, B_SB_LAN_LOCKLANDIS_LOCK);
                
                // Set PWRMBASE + 620h[0] = 1b to statically disable GbE Controller
                MmioOr32 (PWRM_BASE_ADDRESS + R_PCH_PWRM_ST_PG_FDIS_PMC_1, B_PCH_PWRM_ST_PG_FDIS_PMC_1_GBE_FDIS_PMC);
            }
            break;
        case SB_DEV_SATA1:
            if (Enable){
                // Enable SATA Controller
                MmioAnd32 (PWRM_BASE_ADDRESS + R_SB_PWRM_NST_PG_FDIS_1, (UINT32) ~B_SB_PWRM_NST_PG_FDIS_1_SATA_FDIS_PMC);
                
            } else {
                // Disable SATA Controller
                MmioOr32 (PWRM_BASE_ADDRESS + R_SB_PWRM_NST_PG_FDIS_1, B_SB_PWRM_NST_PG_FDIS_1_SATA_FDIS_PMC);
            }
            break;
        case SB_DEV_AZALIA:
            if (Enable){
                // Enable High Definition Audio
                
                // Set PWRMBASE + 628h [23] = 0
                MmioAnd32 (PWRM_BASE_ADDRESS + R_SB_PWRM_NST_PG_FDIS_1, (UINT32)~B_SB_PWRM_NST_PG_FDIS_1_ADSP_FDIS_PMC);
            } else {
                // Disable High Definition Audio
                
                // Set PCR[HDA] + 530h [4,2,0] = 1b,1b,1b (BIOS Configuration Lock Down, ADSP Disabled, HDA Disabled)
                MmioAndThenOr32(SB_PCR_ADDRESS(PID_DSP, R_PCH_PCR_HDA_FNCFG), (UINT32)~0, \
                                (UINT32)(B_PCH_PCR_HDA_FNCFG_BCLD | B_PCH_PCR_HDA_FNCFG_ADSPD | B_PCH_PCR_HDA_FNCFG_HDASD));
                
                // Set PWRMBASE + 628h [23] = 1
                MmioOr32 (PWRM_BASE_ADDRESS + R_SB_PWRM_NST_PG_FDIS_1, B_SB_PWRM_NST_PG_FDIS_1_ADSP_FDIS_PMC);
                
                // Set PCR[PSF3] + PSF_3_AGNT_T0_SHDW_PCIEN_AUD_RS0_D31_F3[8] to 1
                MmioAndThenOr32(SB_PCR_ADDRESS(PID_PSF3, R_PCH_PCR_PSF3_T0_SHDW_AUD_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN), \
                                (UINT32)~0, \
                                (UINT32)B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);
            }
            break;
        case SB_DEV_FUNCTION_0:
        default:
            break;
    } // end of switch
}

/**
    Save or Restore All Hardware SMIs

    @param Save TRUE  : Save all HW SMIs
                FALSE : Restore saved HW SMIs

    @retval VOID

**/
VOID SbSaveRestoreAllHwSmi (
    IN BOOLEAN      Save
)
{
    static UINT16 Pm1EnSave;
    static UINT32 SmiEnSave;
    static UINT32 Gpe0EnSave;
    static UINT32 GpiSmiEnSave[V_PCH_H_GPIO_GROUP_MAX];
    static UINT8  IoTrapSaveMap; // IoTrapSaveMap[3:0] = IOTR[3:0]
    UINT32 Data32;
    UINT8  Index;
    UINT8  MaxGpioGroup;
    AMI_GPIO_GROUP_INFO  *GpioGroupInfo;
    
    if (AmiGetSbSeries() == SbLp){
            GpioGroupInfo = SbLpGpioGroupInfo;
            MaxGpioGroup = V_PCH_LP_GPIO_GROUP_MAX;
    } else {
            GpioGroupInfo = SbHGpioGroupInfo;
            MaxGpioGroup = V_PCH_H_GPIO_GROUP_MAX;
    }
    
    if (Save) {
        Pm1EnSave = READ_IO16_PM (R_PCH_ACPI_PM1_EN);
        SmiEnSave = READ_IO32_PM (R_PCH_SMI_EN) & ~(B_PCH_SMI_EN_GBL_SMI | B_PCH_SMI_EN_EOS | B_PCH_SMI_EN_APMC);
        
        IoTrapSaveMap = 0;
        for ( Index = 0; Index < 4; Index++ ) {
            IoTrapSaveMap |= (MmioRead32(SB_PCR_ADDRESS(PID_PSTH, R_PCH_PCR_PSTH_TRPREG0 + Index * 8)) & B_PCH_PCR_PSTH_TRPREG_TSE) << Index;
        }
        
        Gpe0EnSave = READ_IO32_PM (R_PCH_ACPI_GPE0_EN_127_96);
        
        for ( Index = 0; Index < MaxGpioGroup; Index++ ) {
            GpiSmiEnSave[Index] = 0;
            GpiSmiEnSave[Index] = MmioRead32(SB_PCR_ADDRESS(GpioGroupInfo[Index].Community, GpioGroupInfo[Index].SmiEnOffset));
        }
    } else {
        WRITE_IO16_PM (R_PCH_ACPI_PM1_EN, Pm1EnSave);
        Data32 = READ_IO32_PM (R_PCH_SMI_EN);
        Data32 &= (B_PCH_SMI_EN_GBL_SMI | B_PCH_SMI_EN_EOS | B_PCH_SMI_EN_APMC);
        WRITE_IO32_PM (R_PCH_SMI_EN, (Data32 | SmiEnSave));
        
        for ( Index = 0; Index < 4; Index++ ) {
            Data32 = MmioRead32(SB_PCR_ADDRESS(PID_PSTH, R_PCH_PCR_PSTH_TRPREG0 + Index * 8));
            if(IoTrapSaveMap & (1 << Index)){
                Data32 |= B_PCH_PCR_PSTH_TRPREG_TSE;
            } else {
                Data32 &= ~B_PCH_PCR_PSTH_TRPREG_TSE;
            }
            MmioWrite32(SB_PCR_ADDRESS(PID_PSTH, R_PCH_PCR_PSTH_TRPREG0 + Index * 8), Data32);
            MmioWrite32 (SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_IOT1 +( Index * 8)), Data32);
        }
        
        WRITE_IO32_PM (R_PCH_ACPI_GPE0_EN_127_96, Gpe0EnSave);
        
        for ( Index = 0; Index < MaxGpioGroup; Index++ ) {
            MmioWrite32(SB_PCR_ADDRESS(GpioGroupInfo[Index].Community, GpioGroupInfo[Index].SmiEnOffset), GpiSmiEnSave[Index]);
        }
    }
}

/**
    Disable all HW SMIs

    @param VOID

    @retval VOID

**/
VOID SbDisableAllHwSmi (
    VOID
)
{
    UINT16 PmBase;

    PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);
    
    // Clear PM1_EN
    IoAnd16(PmBase + R_PCH_ACPI_PM1_EN, 0);  
    // Clear SMI_EN except B_PCH_SMI_EN_GBL_SMI, B_PCH_SMI_EN_EOS and B_PCH_SMI_EN_APMC.
    IoAnd32(PmBase + R_PCH_SMI_EN, (B_PCH_SMI_EN_GBL_SMI | B_PCH_SMI_EN_EOS | B_PCH_SMI_EN_APMC));  
    // Clear GPE0_EN.
    IoAnd32(PmBase + R_SB_ACPI_GPE0_EN_127_96, 0);
    // Clear all GPI SMI.
    GpioDisableAllGpiSmi();
}

/**
    Trigger SW SMI and adding extra I/O instructions around
    the SMI port write reduces the failure rate.

    @param SwSmi - The number of Software SMI

    @retval VOID

**/
VOID SbSwSmiTrigger (
    IN UINT8        SwSmi
)
{
    IoWrite8(0x81, 0xEF);
    IoWrite8(0x81, 0xEF);
    IoWrite8(R_PCH_APM_CNT, SwSmi);
    IoWrite8(0x81, 0xEF);
    IoWrite8(0x81, 0xEF);
}

/**
    Trigger SW SMI and return the S/W SMI data from the S/W SMI data port.

    @param SwSmi The number of Software SMI
    @param Data Pointer to the data to be written to the S/W SMI data port.

    @return Data Pointer to the data read from the S/W SMI data port.

**/
VOID SbSwSmiIo (
    IN     UINT8    SwSmi,
    IN OUT UINT8    *Data
)
{
    IoWrite8(R_PCH_APM_STS, *Data);
    SbSwSmiTrigger(SwSmi);
    *Data = IoRead8(R_PCH_APM_STS);
}

/**
    Get data from S/W SMI I/O port and return the address of S/W SMI 
    I/O port.

    @param DataValue Pointer to the data buffer for receiving the S/W SMI
                     number read from S/W SMI I/O port.

    @return DataValue Pointer to the data buffer contained the S/W SMI
                      number read from S/W SMI I/O port.

**/
UINT16 SbGetSwSmi (
    IN OUT UINT32   *DataValue
)
{
    *DataValue = (UINT32)(IoRead8(R_PCH_APM_CNT));
    
    return R_PCH_APM_CNT;
}

/**
    Get Intel TCO2 Status address.(Intel Chipset Only)

    @param AcpiBaseAddr ACPI Base address, it is defined by PM_BASE_ADDRESS
                        normally.

    @return The address of TCO2 status register.
            0xFFFF - not supported.

**/
UINT16 SbGetTco2StsAddress (
    IN UINT16       AcpiBaseAddr
)
{
    return TCO_BASE_ADDRESS + R_PCH_TCO2_STS;
}

/**
    Get Intel TCO2 Control address. (Intel Chipset Only)

    @param AcpiBaseAddr ACPI Base address, it is defined by PM_BASE_ADDRESS
                        normally.

    @retval The address of TCO2 control register.
            0xFFFF - not supported.

**/
UINT16 SbGetTco2CntAddress (
    IN UINT16       AcpiBaseAddr
)
{
    return TCO_BASE_ADDRESS + R_PCH_TCO2_CNT;
}

/**
    Set After G3 bit.

    @param Set
           TRUE  = Enable G3 Bit
           FALSE = Disable G3 Bit

    @retval VOID

**/
VOID SetAfterG3Bit (
    IN BOOLEAN      Set
)
{
    UINT8  Data8;

    Data8 = MmioRead8 (SB_PMC_MMIO_REG(R_SB_PMC_GEN_PMCON_B));
    if (Set)
        Data8 |= B_SB_PMC_GEN_PMCON_B_AFTERG3_EN;
    else
        Data8 &= (UINT8)~B_SB_PMC_GEN_PMCON_B_AFTERG3_EN;
    
    MmioWrite8 (SB_PMC_MMIO_REG(R_SB_PMC_GEN_PMCON_B), Data8);
}

/**
    Check After G3 bit.

    @param VOID

    @retval TRUE:  G3 bit is set
            FALSE: G3 bit is not set

**/
BOOLEAN SbCheckAfterG3 (
    VOID
)
{
    if (MmioRead8 (SB_PMC_MMIO_REG(R_SB_PMC_GEN_PMCON_B)) & B_SB_PMC_GEN_PMCON_B_AFTERG3_EN)
        return TRUE;
    else
        return FALSE;
}

/**
    Disable LPC Super I/O Devices

    @param SioType - The type of SIO device, refer to SB_LPC_SIO_TYPE.

    @retval VOID

**/
VOID SbDisableLpcDevices (
    IN SB_LPC_SIO_TYPE  SioType
)
{
    UINT16 Data16;
    Data16 = READ_PCI16_SB (R_PCH_LPC_IOE);
    switch (SioType) {
      case SbComA:
        Data16 &= ~B_PCH_LPC_IOE_CAE;
        break;
      case SbComB:
        Data16 &= ~B_PCH_LPC_IOE_CBE;
        break;
      case SbLpt:
        Data16 &= ~B_PCH_LPC_IOE_PPE;
        break;
      case SbFdd:
        Data16 &= ~B_PCH_LPC_IOE_FDE;
        break;
      case SbGameL:
        Data16 &= ~B_PCH_LPC_IOE_LGE;
        break;
      case SbGameH:
        Data16 &= ~B_PCH_LPC_IOE_HGE;
        break;
      case SbKbc:
        Data16 &= ~B_PCH_LPC_IOE_KE;
        break;
      case SbMc:
        Data16 &= ~B_PCH_LPC_IOE_ME1;
        break;
      case SbCnf1:
        Data16 &= ~B_PCH_LPC_IOE_SE;
        break;
      case SbCnf2:
        Data16 &= ~B_PCH_LPC_IOE_ME2;
        break;
      default:
        break;
    }
    WRITE_PCI16_SB (R_PCH_LPC_IOE, Data16);
}

/**
    Enable EHCI USB SMI

    @param VOID

    @retval VOID

**/
VOID SbEnableEhciSmi (
    VOID
)
{
  /** PORTING REQUIRED **
  UINT16 PmBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);
  IoOr32 (PmBase + R_PCH_SMI_EN, B_PCH_SMI_EN_LEGACY_USB2);
  **/
}

/**
    Disable EHCI USB SMI

    @param VOID

    @retval VOID

**/
VOID SbDisableEhciSmi (
    VOID
)
{
  /** PORTING REQUIRED **
  UINT16 PmBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);
  IoAnd32 (PmBase + R_PCH_SMI_EN, ~B_PCH_SMI_EN_LEGACY_USB2);
  IoWrite32 (PmBase + R_PCH_SMI_STS, B_PCH_SMI_STS_LEGACY_USB2);
  **/
}

/**
    Check power button pressed.

    @param VOID

    @retval TRUE:  Pressed
            FALSE: No Pressed

**/
BOOLEAN IsPowerButtonPressed (
    VOID
)
{  

  UINT16 PmBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);

  if (IoRead16 (PmBase + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_PWRBTN) {
    return TRUE;
  } else {
    return FALSE;
  }

  return FALSE;
}

/**
    Enable PCI PME bit.

    @param PciAddress The address got from PCI_LIB_ADDRESS macro in
                      PciLib.h
        #define PCI_LIB_ADDRESS(Bus,Device,Function,Register) \
        (((Register) & 0xfff) | (((Function) & 0x07) << 12) | \
        (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

    @retval VOID

**/
VOID SbEnablePciPme (
    IN UINTN        PciAddress
)
{

  UINT8  CapPtr;
  UINTN  PcieAddress;
  
  PcieAddress = CSP_PCIE_CFG_ADDRESS(
                  (PciAddress >> 20) & 0xFF,
                  (PciAddress >> 15) & 0x1F,
                  (PciAddress >> 12) & 0x07,
                  0);  
  
  CapPtr = FindCapPtr (PcieAddress, 0x01);
  // CapPtr is found
  if (CapPtr != 0) {
    // Set PMEE.
    MmioOr16 (PcieAddress + CapPtr + 4, BIT8);
  }
}

/**
    Disable PCI PME bit.

    @param PciAddress The address got from PCI_LIB_ADDRESS macro in
                      PciLib.h
        #define PCI_LIB_ADDRESS(Bus,Device,Function,Register) \
        (((Register) & 0xfff) | (((Function) & 0x07) << 12) | \
        (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

    @retval VOID

**/
VOID SbDisablePciPme (
    IN UINTN        PciAddress
)
{
  UINT8  CapPtr;
  UINTN  PcieAddress;
  
  PcieAddress = CSP_PCIE_CFG_ADDRESS(
                  (PciAddress >> 20) & 0xFF,
                  (PciAddress >> 15) & 0x1F,
                  (PciAddress >> 12) & 0x07,
                  0);  
  
  CapPtr = FindCapPtr (PcieAddress, 0x01);
  // CapPtr is found
  if (CapPtr != 0) {
    // Disable PMEE.
    MmioAnd16 (PcieAddress + CapPtr + 4, ~BIT8);
    // Clear PMES
    MmioWrite16 (PcieAddress + CapPtr + 4, BIT15);
  }
}

/**
    Program the related registers for WOL enabled.

    @param VOID

    @retval VOID

**/
VOID SbEnableWolPmConfg (
    VOID
)
{
    UINT32 RegData32;

    ///
    /// PCH BIOS Spec Rev 0.5.0 Section 10.4 Wake-On-LAN (WOL) Implementation
    /// Step 1
    /// Clear D31:F0:A2h[14] = 0b to ensure the LAN PHY will be powered for WOL
    /// when the power source is either the AC or the DC battery.
    ///
    RegData32 = READ_PCI32(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_A);
    RegData32 &= (UINT32) ~B_PCH_PMC_GEN_PMCON_A_DC_PP_DIS;

    ///
    /// Step 2
    /// Clear D31:F0:A2h[13] = 0b to ensure the LAN PHY will be powered for WOL in Deep Sx.
    ///
    RegData32 &= (UINT32) ~B_PCH_PMC_GEN_PMCON_A_DSX_PP_DIS;

    ///
    /// Step 3
    /// Set D31:F0:A2h[12] = 1b to ensure the LAN PHY will be powered for WOL after a G3 transition.
    ///
    RegData32 |= (UINT32) B_PCH_PMC_GEN_PMCON_A_AG3_PP_EN;

    ///
    /// Step 4
    /// Set D31:F0:A2h[11] = 1b to ensure the LAN PHY will be powered for WOL from Sx.
    ///
    RegData32 |= (UINT32) B_PCH_PMC_GEN_PMCON_A_SX_PP_EN;
    ///
    /// Step 5
    /// Set D31:F0:A2h[0] = 0b and D31:F0:A2h[2] = 0b to avoid clearing write 1 clear bits.
    ///  
    RegData32 &= (UINT32) ~B_PCH_PMC_GEN_PMCON_A_GBL_RST_STS;
    RegData32 &= (UINT32) ~B_PCH_PMC_GEN_PMCON_A_MS4V;  
    
    WRITE_PCI32(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_A, RegData32);
}

/**
    Program the related registers for WOL disabled.

    @param VOID

    @retval VOID

**/
VOID SbDisableWolPmConfg (
    VOID
)
{
    UINT32 RegData32;

    ///
    /// D31:F0:A2h[14:11] and D31:F0:A4h[13] are all in RTC or DSW well, so BIOS also
    /// needs to program them while WOL setup option is disabled.
    ///
    RegData32 = READ_PCI32(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_A);
    RegData32 |= (UINT32) (B_PCH_PMC_GEN_PMCON_A_DC_PP_DIS | B_PCH_PMC_GEN_PMCON_A_DSX_PP_DIS);
    RegData32 &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_A_AG3_PP_EN | B_PCH_PMC_GEN_PMCON_A_SX_PP_EN);
    ///
    /// Set D31:F0:A2h[0] = 0b and D31:F0:A2h[2] = 0b to avoid clearing write 1 clear bits.
    ///  
    RegData32 &= (UINT32) ~B_PCH_PMC_GEN_PMCON_A_GBL_RST_STS;
    RegData32 &= (UINT32) ~B_PCH_PMC_GEN_PMCON_A_MS4V;    

    WRITE_PCI32(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_A, RegData32);
}

/**
    Enable wake on LAN PM Confg3

    @param VOID

    @retval VOID

**/
VOID SbEnableWolPmConfg3 ( VOID )
{
  UINT16 RegData16;

  ///
  /// Step 6
  /// Set "WOL Enable Override", D31:F0:A4h:[13], bit to 1b to guarantee the
  /// LAN-Wakes are enabled at the Power Management Controller, even in surprise
  /// S5 cases such as power loss/return and Power Button Override
  ///
  RegData16 = READ_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B);
  RegData16 |= (UINT16) B_PCH_PMC_GEN_PMCON_B_WOL_EN_OVRD;
  ///
  /// Step 6
  /// Set D31:F0:A4h[1] = 0b,D31:F0:A4h[9] = 0b ,and D31:F0:A4h[14] = 0b to avoid clearing write 1 clear bits.
  ///  
  RegData16 &= (UINT16) ~B_PCH_PMC_GEN_PMCON_B_PWR_FLR;
  RegData16 &= (UINT16) ~B_PCH_PMC_GEN_PMCON_B_HOST_RST_STS;  
  RegData16 &= (UINT16) ~B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR;    

  WRITE_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B, RegData16);
}

/**
    Disable wake on LAN PM Confg3

    @param VOID

    @retval VOID

**/
VOID SbDisableWolPmConfg3 ( VOID )
{
  UINT16 RegData16;

  ///
  /// D31:F0:A2h[14:11] and D31:F0:A4h[13] are all in RTC or DSW well, so BIOS also
  /// needs to program them while WOL setup option is disabled.
  ///
  RegData16 = READ_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B);
  RegData16 &= (UINT16) ~(B_PCH_PMC_GEN_PMCON_B_WOL_EN_OVRD);
  ///
  /// Set D31:F0:A4h[1] = 0b,D31:F0:A4h[9] = 0b, and D31:F0:A4h[14] = 0b to avoid clearing write 1 clear bits.
  ///  
  RegData16 &= (UINT16) ~B_PCH_PMC_GEN_PMCON_B_PWR_FLR;
  RegData16 &= (UINT16) ~B_PCH_PMC_GEN_PMCON_B_HOST_RST_STS;  
  RegData16 &= (UINT16) ~B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR;

  WRITE_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B, RegData16);
}

/**
    Get I/O Trap Info

    @param TrappedIoData Pointer to a 32bit data buffer for receiving the
                         data of I/O trap information.

    @return TrappedIoData Pointer to a 32bit data buffer contained the
                          data of I/O trap information.
            I/O Trap address

**/
UINT16 SbGetIoTrapInfo (
    IN OUT UINT32   *TrappedIoData 
)
{
    UINT32 Data32;
    
    Data32 = MmioRead32(SB_PCR_ADDRESS(PID_PSTH, R_PCH_PCR_PSTH_TRPC));
    if (Data32 & B_PCH_PCR_PSTH_TRPC_RW){
        //  Trapped cycle was a read cycle
        *TrappedIoData = 0;
    } else {
        //  Trapped cycle was a write cycle
        *TrappedIoData = MmioRead32(SB_PCR_ADDRESS(PID_PSTH, R_PCH_PCR_PSTH_TRPD));
    }
    
    // Return Trapped I/O Address
    return (MmioRead32(SB_PCR_ADDRESS(PID_PSTH, R_PCH_PCR_PSTH_TRPC)) & B_PCH_PCR_PSTH_TRPC_IOA);  
}

/**
    Get ACPI Base Address

    @param VOID

    @return The address of ACPI Base.

**/
UINT16 SbGetAcpiBaseAddress (
    VOID
)
{
    UINT16 PmBase;
    
    PchAcpiBaseGet(&PmBase);
    return PmBase;
}

/**
    Get PM1 Control Register Offset

    @param VOID

    @return The offset of PM1 Control Register

**/
UINT16 SbGetPm1CntOffset (
    VOID
)
{
    return SbGetAcpiBaseAddress() + R_PCH_ACPI_PM1_CNT;
}

/**
    Get Intel RCBB Address (Intel Chipset Only)

    @param VOID

    @return The address of RCRB

**/
UINT32 SbGetRcrbAddress (
    VOID
)
{
    return 0xFFFFFFFF;
}

/**
    Get South Bridge Miscellaneous BIT Status

    @param SbMiscType   Please check SB_MISC_TYPE for the details
    @param BitStatus    The value of the Miscellaneous BIT

    @retval EFI_UNSUPPORTED   This Miscellaneous BIT is not supported
    @retval EFI_SUCCESS       Success to get the value of the Miscellaneous BIT

**/
EFI_STATUS SbGetMiscBitStatus(
    IN SB_MISC_TYPE         SbMiscType,
    IN OUT UINT8            *BitStatus
)
{
/**** PORTING REQUIRED ****
    // Please check the datasheet of the chipset for the register to know
    // how to get the value of the Miscellaneous BIT indicated by SbMiscType.
    // If the type is not supported by the chip, please return EFI_UNSUPPORTED.
    switch (SbMiscType) {
      case CPU_THERMAL_TRIP_STATUS:
      case AFTERG3_EN:
      case PWR_FLR:
        return EFI_SUCCESS;
        break;
      default: 
        break;
    }
**** PORTING REQUIRED ****/    
    return EFI_UNSUPPORTED;
}

/**
    Program South Bridge Miscellaneous BIT

    @param SbMiscType   Please check SB_MISC_TYPE for the details
    @param Set          Set/Clear the value of the Miscellaneous BIT

    @retval EFI_UNSUPPORTED   This Miscellaneous BIT is not supported
    @retval EFI_SUCCESS       Success to Set/Clear the value of the Miscellaneous BIT

**/
EFI_STATUS SbProgramMiscBit(
    IN SB_MISC_TYPE         SbMiscType,
    IN BOOLEAN              Set
)
{
/**** PORTING REQUIRED ****
    // Please check the datasheet of the chipset for the register to know
    // how to program the value of the Miscellaneous BIT indicated by SbMiscType.
    // If the type is not supported by the chip, please return EFI_UNSUPPORTED.
    switch (SbMiscType) {
      case CPU_THERMAL_TRIP_STATUS:
      case AFTERG3_EN:
      case PWR_FLR:
        return EFI_SUCCESS;
        break;
      default: 
        break;
    }
**** PORTING REQUIRED ****/
    return EFI_UNSUPPORTED;
}

/**
    Check if any USB port is routed to XHCI controller

    @param VOID

    @return TRUE  There is
    @return FALSE There is not

**/
BOOLEAN SbIsXhciRouting ( 
    VOID 
)
{
    /**** PORTING REQUIRED ****
        Please check the datasheet of the chipset for the register to know
        if any USB port is routed to XHCI controller.
        
        From SkyLake/KabyLake platform, there is only xhci controller
        so all usb ports are under xhci controller
    **** PORTING REQUIRED ****/
    return  TRUE;
}

/**
    Get the Mac Address of the LAN inside the chipset

    @param  MacAddress        Pointer to the buffer used to store the Mac Address

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to get the Mac Address

**/
EFI_STATUS SbGetChipLanMacAddress ( 
    IN OUT UINT8            *MacAddress
)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to know
    how to get the Mac Address of the LAN inside the chipset.
    For Intel chip, you also can get MAC address from Gbe Region of 
    the flash.
    If it is not supported by the chip or there is no LAN inside the 
    chip, please return EFI_UNSUPPORTED.
**** PORTING REQUIRED ****/
    return EFI_UNSUPPORTED;
}

/**
    Get Bus/Dev/Func of SbDevType

    @param SbDevType - The type of SB device, refer to SB_DEV_TYPE

    @retval 0xFFFFFFFF        This function is not supported
    @retval Not 0xFFFFFFFF    Bus[31:24]/Dev[23:16]/Func[15:8] of SbDevType

**/
UINTN SbGetPciBusDevFunc ( 
    IN SB_DEV_TYPE  SbDevType
)
{
    switch (SbDevType) {
        case SB_DEV_XHCI:
            return SB_XHCI_REG(0);
        case SB_DEV_THERMAL:
            return SB_THERMAL_REG(0);
        case SB_DEV_SATA1:
            return SB_SATA_REG(0);
        case SB_DEV_PCIEX_PORT1:
            return SB_PCIERP0_REG(0);
        case SB_DEV_PCIEX_PORT2:
            return SB_PCIERP1_REG(0);
        case SB_DEV_PCIEX_PORT3:
            return SB_PCIERP2_REG(0);
        case SB_DEV_PCIEX_PORT4:
            return SB_PCIERP3_REG(0);
        case SB_DEV_PCIEX_PORT5:
            return SB_PCIERP4_REG(0);
        case SB_DEV_PCIEX_PORT6:
            return SB_PCIERP5_REG(0);
        case SB_DEV_PCIEX_PORT7:
            return SB_PCIERP6_REG(0);
        case SB_DEV_PCIEX_PORT8:
            return SB_PCIERP7_REG(0);
        case SB_DEV_PCIEX_PORT9:
            return SB_PCIERP8_REG(0);
        case SB_DEV_PCIEX_PORT10:
            return SB_PCIERP9_REG(0);
        case SB_DEV_PCIEX_PORT11:
            return SB_PCIERP10_REG(0);
        case SB_DEV_PCIEX_PORT12:
            return SB_PCIERP11_REG(0);
        case SB_DEV_PCIEX_PORT13:
            return SB_PCIERP12_REG(0);
        case SB_DEV_PCIEX_PORT14:
            return SB_PCIERP13_REG(0);
        case SB_DEV_PCIEX_PORT15:
            return SB_PCIERP14_REG(0);
        case SB_DEV_PCIEX_PORT16:
            return SB_PCIERP15_REG(0);
        case SB_DEV_PCIEX_PORT17:
            return SB_PCIERP16_REG(0);
        case SB_DEV_PCIEX_PORT18:
            return SB_PCIERP17_REG(0);
        case SB_DEV_PCIEX_PORT19:
            return SB_PCIERP18_REG(0);
        case SB_DEV_PCIEX_PORT20:
            return SB_PCIERP19_REG(0);
        case SB_DEV_PCIEX_PORT21:
            return SB_PCIERP20_REG(0);
        case SB_DEV_PCIEX_PORT22:
            return SB_PCIERP21_REG(0);
        case SB_DEV_PCIEX_PORT23:
            return SB_PCIERP22_REG(0);
        case SB_DEV_PCIEX_PORT24:
            return SB_PCIERP23_REG(0);
        case SB_DEV_SMBUS:
            return SB_SMBUS_REG(0);
        case SB_DEV_AZALIA:
            return SB_HDA_REG(0);
        case SB_DEV_LAN:
            return SB_LAN_REG(0);
        default:
            return 0xFFFFFFFF;
    }
}

#if defined(AMI_COMPATIBILITY_PKG_VERSION)&&(AMI_COMPATIBILITY_PKG_VERSION > 18)
#if KBC_SUPPORT
/**
    This function detects if the Ctrl + Home key press has been made.
    Is function is only included if KBC_SUPPORT is enabled.  It enables
    the keyboard checks for the Ctrl + Home key press and then disables
    the keyboard

        
    @param PeiServices pointer to the PeiServices Table

    @retval TRUE - Ctrl + Home was pressed
            FALSE - Ctrl + Home not detected

    @note  Please make sure the following items have been done before
           calling IsCtrlHomePressed.
           - The decode of Port 60/64 has been enabled.
           - KBC controller has been enabled in SIO/EC device.
  
**/
BOOLEAN IsCtrlHomePressed (
    EFI_PEI_SERVICES **PeiServices
)
{
  UINT8               Index;
  UINT8               KeyValue = 0;
  UINT8               KeyScan = 0;
  UINT8               LoopCount = CTRL_HOME_CHECK_LOOP;
  UINT8               KbcSts = 0;
  UINT8               KbcSts1 = 0;
  UINT8               KbcSts2 = 0;
  BOOLEAN             KbcIBF = TRUE;
  BOOLEAN             KbcIBF1 = TRUE;
  UINT8               KbcRes = 0;
  
#if defined(POST_TIME_TUNING_SUPPORT) && POST_TIME_TUNING_SUPPORT
{
    EFI_GUID                              EfiPeiReadOnlyVariable2PpiGuid = EFI_PEI_READ_ONLY_VARIABLE2_PPI_GUID;        
    EFI_PEI_READ_ONLY_VARIABLE2_PPI       *ReadVariablePpi;
    UINTN                                 Size;
    UINT8                                 PowerFailure;    
    FAST_BOOT2                            FastBootVar;
    EFI_GUID                              FastBootVariableGuid = FAST_BOOT_VARIABLE_GUID;
    EFI_STATUS                            Status;
    EFI_PEI_PCI_CFG2_PPI                  *PciCfg;
    
    PciCfg = (*PeiServices)->PciCfg;
    
    Status = (*PeiServices)->LocatePpi (
                               (CONST EFI_PEI_SERVICES**)PeiServices, 
                               &EfiPeiReadOnlyVariable2PpiGuid,
                               0,
                               NULL,
                               (VOID **)&ReadVariablePpi
                               );
    if (EFI_ERROR(Status)) return FALSE;
    
    Status = PciCfg->Read (
                       (CONST EFI_PEI_SERVICES**)PeiServices,
                       PciCfg,
                       EfiPeiPciCfgWidthUint8,
                       SB_PMC_REG(R_SB_PMC_GEN_PMCON_B), // 0xA4 
                       &PowerFailure
                       );
    if (EFI_ERROR(Status)) return FALSE; 
       
    PowerFailure &= BIT1;
    
    Size = sizeof(FastBootVar);
    Status = ReadVariablePpi->GetVariable(
                                ReadVariablePpi, 
                                L"LastBoot",
                                &FastBootVariableGuid, 
                                NULL,
                                &Size, 
                                &FastBootVar
                                );
    if ((!EFI_ERROR(Status)) && (PowerFailure == 0)) {
      // Fast Boot is enabled and no Power Failure, so skip detecting CtrlHome.
      return FALSE;
    }
}
#endif 

  //
  // Enable KBD
  //
  KbcSts = IoRead8 (0x64);
  //
  // Check KeyBoard Presence
  //
  if (KbcSts != 0xFF) {
    //
    // Save the Sys Bit for later use
    //
    KbcRes = KbcSts & 0x04;
    IoWrite8 (0x64, 0x60);  
    //
    // Wait for Input Buffer Empty
    //
    while(KbcIBF) {
      KbcIBF = FALSE;
      KbcSts1 = IoRead8 (0x64);
      if ((KbcSts1 & 0x02) == 0x02) {
        KbcIBF = TRUE;
      }
    }
    //
    // Keyboard enable, Mouse disable
    //
    IoWrite8 (0x60, KbcRes | 0x61);          
    //  
    // Wait for Input Buffer Empty
    //
    while(KbcIBF1) {
      KbcIBF1 = FALSE;
      KbcSts2 = IoRead8 (0x64);
      if ((KbcSts2 & 0x02) == 0x02) {
        KbcIBF1 = TRUE;
      }
    } 
  }  
  //
  // Loop for Key scan
  //
  for (Index = 0 ; Index < LoopCount ; Index++) {
    //
    // Wait at least for 6ms
    //
    CountTime(6000, PM_BASE_ADDRESS);
    //
    // Read Key stroke
    //
    if (IoRead8 (0x64) & 0x01) {
      KeyValue = IoRead8 (0x60);
    }
    //
    // Check for valid key
    //
    if (KeyValue == 0x1d) {
      //
      // Set BIT0 if it is CTRL key
      //
      KeyScan |= (1 << 0);
    }
    
    if (KeyValue == 0x47) {
      //
      // Set BIT1 if it is HOME key
      //
      KeyScan |= (1 << 1);
    }
    //
    // Check for CTRL_HOME Combination
    //
    if ((KeyScan & 0x03) == 0x03) {
      //
      // Disable KeyBoard Controller
      //
      IoWrite8 (0x64, 0x60);
      IoWrite8 (0x60, 0x30);
      return TRUE;
    }
  } // for (Index = 0 ; Index < LoopCount ; Index++)
  //
  // Disable KeyBoard Controller
  //
  IoWrite8 (0x64, 0x60);
  IoWrite8 (0x60, 0x30);
  //
  // No valid key is pressed
  //
  return FALSE;
}
#endif        // KBC_SUPPORT
#endif        // AMI_COMPATIBILITY_PKG_VERSION

/**
    Get South Bridge Information Data

    @param SB_INFO_DATA     Pointer to the SB Information Data.

    @retval VOID

**/
VOID GetSbInfoData ( 
    IN OUT SB_INFO_DATA    *SbInfoData
)
{
    SbInfoData->ChipVendor = CHIPSET_VENDOR;
    SbInfoData->Stepping = PchStepping();
    SbInfoData->PciePortNum = GetPchMaxPciePortNum();
    SbInfoData->SataPortNum = GetPchMaxSataPortNum();
    SbInfoData->UsbPhysicalPortNum = GetPchUsbMaxPhysicalPortNum();
    SbInfoData->Usb2PortNum = GetPchXhciMaxUsb2PortNum();
    SbInfoData->Usb3PortNum = GetPchXhciMaxUsb3PortNum();
}

/**
    Return Sb Series.

    @param VOID

    @retval Pch Series

**/
AMI_SB_SERIES
EFIAPI
AmiGetSbSeries ( VOID )
{
    UINT16  LpcDeviceId;
    UINT32  PchSeries;
    
    LpcDeviceId = READ_PCI16_SB(PCI_DEVICE_ID_OFFSET);
    
    if (IS_PCH_H_LPC_DEVICE_ID (LpcDeviceId)) {
        PchSeries = SbH;
    } else if (IS_PCH_LP_LPC_DEVICE_ID (LpcDeviceId)) {
        PchSeries = SbLp;
    } else {
        PchSeries = SbUnknownSeries;
    }
    
    return PchSeries;
}

UINT8 
GetSbUsbMaxPhysicalPortNum(
    VOID
)
{
    AMI_SB_SERIES  SbSeries;

    SbSeries = AmiGetSbSeries();
    switch (SbSeries) {
      case SbLp:
        return PCH_LP_XHCI_MAX_USB2_PHYSICAL_PORTS;

      case SbH:
        return PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS;

      default:
        return 0;
    }
}

UINT8 
GetSbXhciMaxUsb2PortNum(
    VOID 
)
{
    AMI_SB_SERIES  SbSeries;

    SbSeries = AmiGetSbSeries();
    switch (SbSeries) {
      case SbLp:
        return PCH_LP_XHCI_MAX_USB2_PORTS;

      case SbH:
        return PCH_H_XHCI_MAX_USB2_PORTS;

      default:
        return 0;
    }
}

UINT8 GetSbXhciMaxUsb3PortNum ( VOID )
{
    AMI_SB_SERIES  SbSeries;

    SbSeries = AmiGetSbSeries();
    switch (SbSeries) {
      case SbLp:
        return PCH_LP_XHCI_MAX_USB3_PORTS;

      case SbH:
        return PCH_H_XHCI_MAX_USB3_PORTS;

      default:
        return 0;
    }
}

UINT8 GetSbMaxSataPortNum ( VOID )
{
    AMI_SB_SERIES  SbSeries;

    SbSeries = AmiGetSbSeries();
    switch (SbSeries) {
      case SbLp:
        return PCH_LP_AHCI_MAX_PORTS;

      case SbH:
        return PCH_H_AHCI_MAX_PORTS;

      default:
        return 0;
    }
}

/**
    This function searches the PCI address space for the PCI
    device specified for a particular capability ID and returns
    the offset in the PCI address space if one found

    @param UINT64  PciAddress,
    @param UINT8   CapId

    @retval Capability ID Address if one found
        Otherwise returns 0
**/
UINT32 SbFindCapPtr(
    IN UINT64  PciAddress,
    IN UINT8   CapId
)
{
    UINT8   Value;
    UINT32  Address = (UINT32)PciAddress;

    Address = (Address & 0xffffff00) | 6; //PCI Status Register.
    Value = READ_MEM8(Address + 0);

    if (Value == 0xff) return 0;          // No device.
    if (!(Value & (1 << 4))) return 0;    // Check if capabilities list.
   
    *(UINT8*)&Address = 0x34;             // Register to First capabilities pointer
                                        // if 0, then capabilities
    for(;;) {
        Value = READ_MEM8(Address + 0);
        if (Value == 0) return 0;

        *(UINT8*)&Address = Value;          // PciAddress = ptr to CapID
        Value = READ_MEM8(Address + 0);     // New cap ptr.

        //If capablity ID, return register that points to it.
        if (Value == CapId) return Address; 

        ++Address;                          // Equals to next capability pointer.
    }
}

VOID
SbLibBeforeShutdown(
    VOID
)
{
    UINT8           DevNo;
    UINT8           FunNo;

    for (DevNo = 0; DevNo < 0x20 ; DevNo++) {
        if (READ_PCI32(0, DevNo, 0, PCI_VID) != 0xffffffff) {
            FunNo = (READ_PCI8(0, DevNo, 0, PCI_HDR) & 0x80) ? 8 : 1;
            do {
                FunNo--;
                if (READ_PCI32(0, DevNo, FunNo, PCI_VID) != 0xffffffff) {
                    RESET_PCI16(0, DevNo, FunNo, PCI_CMD, 4);
                }
            } while (FunNo);
        }
    }
}


/**
    Shutdown the system (Enter soft-off/S5)

    @param VOID

    @retval VOID
**/

VOID
SBLib_Shutdown(
    VOID
)
{
    UINT16          Buffer16;

    // Disable Sleep SMI
    RESET_IO32_PM(R_PCH_SMI_EN, B_PCH_SMI_EN_ON_SLP_EN); // 0x30

    SbLibBeforeShutdown();

    // Clear power button status
    Buffer16 = READ_IO16(PM_BASE_ADDRESS) | (UINT16)(1 << 8);
    do {
        WRITE_IO16(PM_BASE_ADDRESS, Buffer16);
        for (Buffer16 = 0; Buffer16 < 0x100; Buffer16++)
            WRITE_IO8(IO_DELAY_PORT, (UINT8)Buffer16);
        Buffer16 = READ_IO16(PM_BASE_ADDRESS);
    } while (Buffer16 & 0x100);

    // Enter soft-off/S5
    RW_IO16(PM_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, \
                                              (SLP_S5 | 8 ) << 10, 15 << 10);

    // We should never get this far
    EFI_DEADLOOP();
}

/**
    Clear the ME wake up status.

    @param VOID

    @retval VOID

**/

VOID
ClearMeWakeSts(
    VOID
)
{
    UINT32              PchPwrmBase;

    PchPwrmBaseGet(&PchPwrmBase);
    WRITE_MEM8 (PchPwrmBase + R_PCH_PWRM_PRSTS, B_PCH_PWRM_PRSTS_ME_WAKE_STS);
}

BOOLEAN 
SbIsWarmBoot (
    VOID
)
{
    /**** PORTING REQUIRED ****
    // Please check the datasheet of the chipset for the register to know
    // how to check warm boot status.
    // If it is not supported by the chip, please return FALSE.
    **** PORTING REQUIRED ****/
// AmiChipsetModulepkg Template default - START
//    return FALSE;
// AmiChipsetModulepkg Template default - END

    UINT32        PmConA;
    UINT8         SbWarmBootType = SystemBootModeMax;

    PmConA = READ_PCI32 (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_A);

    if(GetBootModeHob () == BOOT_ON_S3_RESUME) {
      SbWarmBootType = SystemS3Resume;
    } else if ((PmConA & B_PCH_PMC_GEN_PMCON_A_MEM_SR)
            && (PmConA & B_PCH_PMC_GEN_PMCON_A_DISB) ) {
      //
      // If B_PCH_PMC_GEN_PMCON_A_MEM_SR and B_PCH_PMC_GEN_PMCON_A_DISB BITs can be Set.
      // But the Boot Mode type is not S3 resume, it mean warm boot. (narrow sense)
      //
      SbWarmBootType = SystemWarmBoot;
    }

    DEBUG ((DEBUG_INFO, "SbIsWarmBoot: %s\n", ((SbWarmBootType == SystemWarmBoot) ? L"Warm Boot" : ((SbWarmBootType == SystemS3Resume) ? L"S3 Resume" : L"Cold Boot" ) )));

    return (SbWarmBootType == SystemWarmBoot) ? TRUE : FALSE;
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
