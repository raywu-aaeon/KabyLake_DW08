//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************


/** @file SbAcpiTimerLib.c
    This file contains south bridge related TimeLib library
    instance

**/


// Module specific Includes
#include <Base.h>
#include <PchAccess.h>
#include <Token.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>


/**
    Internal function to read the current tick counter of ACPI

    @param VOID

    @retval The tick counter read

**/

STATIC
UINT32
InternalAcpiGetTimerTick (
  VOID
  )
{
  return IoRead32 (PcdGet16 (PcdAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_TMR);
}

/**
    Stalls the CPU for at least the given number of ticks.
    It's invoked by MicroSecondDelay() and NanoSecondDelay().

    @param Delay     A period of time to delay in ticks

    @retval VOID

**/

STATIC
VOID
InternalAcpiDelay (
  IN      UINT32                    Delay
  )
{
  UINT32                            Ticks;
  UINT32                            Times;

  Times    = Delay >> 22;
  Delay   &= BIT22 - 1;
  do {
    //
    // The target timer count is calculated here
    //
    Ticks    = InternalAcpiGetTimerTick () + Delay;
    Delay    = BIT22;
    //
    // Wait until time out
    // Delay >= 2^23 could not be handled by this function
    // Timer wrap-arounds are handled correctly by this function
    //
    while (((Ticks - InternalAcpiGetTimerTick ()) & BIT23) == 0) {
      CpuPause ();
    }
  } while (Times-- > 0);
}

/**
    This function delays for the number of micro seconds passed in

    @param MicroSeconds Number of microseconds(us) to delay

    @retval Value passed in for microseconds delay

**/

UINTN
EFIAPI
MicroSecondDelay (
  IN  UINTN   MicroSeconds
  )
{
  UINTN   Ticks;
  UINTN   Counts;
  UINTN   CurrentTick;
  UINTN   OriginalTick;
  UINTN   RemainingTick;

  if (MicroSeconds == 0) {
    return MicroSeconds;
  }

  OriginalTick  = IoRead32 ((UINTN) (PM_BASE_ADDRESS + R_PCH_ACPI_PM1_TMR)) & B_PCH_ACPI_PM1_TMR_VAL;
  CurrentTick   = OriginalTick;

  ///
  /// The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  ///
  Ticks = MicroSeconds * 358 / 100 + OriginalTick + 1;

  ///
  /// The loops needed by timer overflow
  ///
  Counts = Ticks / V_PCH_ACPI_PM1_TMR_MAX_VAL;

  ///
  /// Remaining clocks within one loop
  ///
  RemainingTick = Ticks % V_PCH_ACPI_PM1_TMR_MAX_VAL;

  ///
  /// not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  /// one I/O operation, and maybe generate SMI
  ///
  while ((Counts != 0) || (RemainingTick > CurrentTick)) {
    CurrentTick = IoRead32 ((UINTN) (PM_BASE_ADDRESS + R_PCH_ACPI_PM1_TMR)) & B_PCH_ACPI_PM1_TMR_VAL;
    ///
    /// Check if timer overflow
    ///
    if ((CurrentTick < OriginalTick)) {
      if (Counts != 0) {
        Counts--;
      } else {
        ///
        /// If timer overflow and Counts equ to 0, that means we already stalled more than
        /// RemainingTick, break the loop here
        ///
        break;
      }
    }

    OriginalTick = CurrentTick;
  }
      
  return MicroSeconds;
}

/**
    This function delays for the number of nano seconds passed in

    @param MicroSeconds Number of nanoseconds(ns) to delay

    @retval Value passed in for nanoseconds delay

**/

UINTN
EFIAPI
NanoSecondDelay (
  IN      UINTN                     NanoSeconds
  )
{
  InternalAcpiDelay (
    (UINT32)DivU64x32 (
              MultU64x32 (
                NanoSeconds,
                V_PCH_ACPI_TMR_FREQUENCY
                ),
              1000000000u
              )
    );
  return NanoSeconds;
}

/**
    Retrieves the current value of a 64-bit free running
    performance counter.

    @param VOID

    @retval The current value of the free running performance counter.

**/

UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{
  return (UINT64)InternalAcpiGetTimerTick ();  
}

/**
    Retrieves the 64-bit frequency in Hz and the range of
    performance counter values.

    @param StartValue  The value the performance counter starts with when
        it rolls over.
    @param EndValue    The value that the performance counter ends with
        before it rolls over.

    @retval The frequency in Hz.

**/

UINT64
EFIAPI
GetPerformanceCounterProperties (
  OUT      UINT64                    *StartValue,  OPTIONAL
  OUT      UINT64                    *EndValue     OPTIONAL
  )
{
  if (StartValue != NULL) {
    *StartValue = 0;
  }

  if (EndValue != NULL) {
    *EndValue = V_PCH_ACPI_PM1_TMR_MAX_VAL - 1;
  }

  return V_PCH_ACPI_TMR_FREQUENCY;
}

/**
    This function converts the elapsed ticks of running performance
    counter to time value in unit of nanoseconds.

    @param Ticks   The number of elapsed ticks of running performance
        counter.

    @retval The elapsed time in nanoseconds.

**/

UINT64
EFIAPI
GetTimeInNanoSecond (
  IN      UINT64                     Ticks
  )
{
  UINT64  Frequency;
  UINT64  NanoSeconds;
  UINT64  Remainder;
  INTN    Shift;

  Frequency = GetPerformanceCounterProperties (NULL, NULL);

  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  NanoSeconds = MultU64x32 (DivU64x64Remainder (Ticks, Frequency, &Remainder), 1000000000u);

  //
  // Ensure (Remainder * 1,000,000,000) will not overflow 64-bit.
  // Since 2^29 < 1,000,000,000 = 0x3B9ACA00 < 2^30, Remainder should < 2^(64-30) = 2^34,
  // i.e. highest bit set in Remainder should <= 33.
  //
  Shift = MAX (0, HighBitSet64 (Remainder) - 33);
  Remainder = RShiftU64 (Remainder, (UINTN) Shift);
  Frequency = RShiftU64 (Frequency, (UINTN) Shift);
  NanoSeconds += DivU64x64Remainder (MultU64x32 (Remainder, 1000000000u), Frequency, NULL);

  return NanoSeconds;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
