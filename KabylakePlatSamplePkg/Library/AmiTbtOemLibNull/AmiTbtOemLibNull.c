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

/** @file AmiTbtOemLibNull.c
    Provide dummy lib function for OEM.

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
    return;
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
    return;
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
    return 0xffff;
}

/**
  This function delays for the number of micro seconds passed in

  @param  DelayTime             Number of microseconds(us) to delay
  @param  BaseAddr              The I/O base address of the ACPI registers

  @retval EFI_SUCCESS           Delay successfully
**/
EFI_STATUS
TbtCountTime (
  IN  UINTN                     DelayTime,
  IN  UINT16                    BaseAddr  // Only needs to be 16 bit for I/O address
  )
{
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
AmiTbtSetPCIe2TBTCommand (
  IN  UINT8                     UpPortBus,
  IN  UINT8                     Data,
  IN  UINT8                     Command,
  IN  UINTN                     Timeout
  )
{
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
    return 3;
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
SynchSecurityLevel (
  IN  UINT8                     BiosSecurityLevel,
  IN  UINT8                     TbtHostLocation
  )
{
    return 0;
}

/**
  This function is configure the Thunderbolt security level.

  @param  TbtSecurityLevel      BIOS Thunderbolt security level setting
  @param  TBTHostSeries         Thunderbolt host chip series
  @param  TbtHostLocation       Thunderbolt host location
  @param  IsPei                 Flag to point out now is in PEI phase or not

  @return                       No return value
**/
VOID
ProgramTbtSecurityLevel (
  IN  UINT8                     *TbtSecurityLevel,
  IN  UINT8                     TBTHostSeries,
  IN  UINT8                     TbtHostLocation,
  IN  BOOLEAN                   IsPei
  )
{
    return;
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
