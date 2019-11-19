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

/** @file TscTimerLib.c
    This file contains TimeLib library instance

**/

#include <Base.h>
#include <Token.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>

#define PlatformInfoMsr   0xce

/**
    Stalls the CPU for at least the given number of ticks.

    @param Delay A period of time to delay in ticks.

    @retval VOID
**/

VOID
InternalX86Delay (
  IN UINT64         Delay
  )
{
  UINT64            Ticks;

  //
  // The target timer count is calculated here
  //
  Ticks = AsmReadTsc() + Delay;

  //
  // Wait until time out
  // Timer wrap-arounds are NOT handled correctly by this function.
  // Thus, this function must be called within 10 years of reset since
  // Intel guarantees a minimum of 10 years before the TSC wraps.
  //
  while (AsmReadTsc() <= Ticks) CpuPause();
}

/**
    Get Tsc frequency

    @param VOID

    @retval UINT64 The number of TSC counts per second.
**/

UINT64
InternalGetTscFrequency (
  VOID
  )
{
  UINT32    Bclk; 
  UINT64    TscFrequency;  
      
  Bclk = 100;
  TscFrequency = MultU64x64 (
                   MultU64x32 (
                     (RShiftU64(AsmReadMsr64(PlatformInfoMsr), 8) & 0xff),
                     Bclk
                     ),
                   1000000u
                 );
  
  return TscFrequency;
}

/**
    This function delays for the number of micro seconds passed in

    @param MicroSeconds Number of microseconds(us) to delay

    @retval UINTN Value passed in for microseconds delay
**/

UINTN
EFIAPI
MicroSecondDelay (
  IN UINTN          MicroSeconds
  )
{
  InternalX86Delay (
    DivU64x32 (
      MultU64x64 (
        InternalGetTscFrequency (),
        MicroSeconds
        ),
      1000000u
    )
  );
  return MicroSeconds;
}

/**
    This function delays for the number of nano seconds passed in

    @param MicroSeconds Number of nanoseconds(ns) to delay

    @retval UINTN Value passed in for nanoseconds delay
**/

UINTN
EFIAPI
NanoSecondDelay (
  IN UINTN          NanoSeconds
  )
{ 
  InternalX86Delay (
    DivU64x32 (
      MultU64x32 (
        InternalGetTscFrequency (),
        (UINT32)NanoSeconds
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

    @retval UINT64 The current value of the free running performance counter.
**/

UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{
  return AsmReadTsc ();
}

/**
    Retrieves the 64-bit frequency in Hz and the range of
    performance counter values.

    @param StartValue The value the performance counter starts with when
        it rolls over.
    @param EndValue The value that the performance counter ends with
        before it rolls over.

    @retval UINT64 The frequency in Hz.
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
      *EndValue = 0xFFFFFFFFFFFFFFFFull;
    }

    return InternalGetTscFrequency ();
}

/**
    This function converts the elapsed ticks of running performance
    counter to time value in unit of nanoseconds.

    @param Ticks The number of elapsed ticks of running performance
        counter.

    @retval UINT64 The elapsed time in nanoseconds.
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
