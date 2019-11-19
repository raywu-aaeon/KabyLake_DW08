/** @file
  BaseEcMisc library implementation.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be
  modified, except as allowed by additional terms of your license agreement.

@par Specification Reference:
**/
#include <Library/EcLib.h>
#include <Library/EcMiscLib.h>
#include <Library/DebugLib.h>

///
/// @todo: it should move to BaseEcAcpiLib once BaseEcAcpiLib has been created.
///
/**
  Send USBC Sx Entry command to EC

  @param[in][out]  DataBuffer

  @retval          EFI_SUCCESS       Command success
  @retval          EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
UsbcSxEntry (
  IN OUT UINT8     *DataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Send USBC Sx EXit command to EC

  @param[out]     DataBuffer

  @retval         EFI_SUCCESS         Command success
  @retval         EFI_DEVICE_ERROR    Command error
**/
EFI_STATUS
UsbcSxExit (
  OUT UINT8       *DataBuffer
  )
{
  return EFI_SUCCESS;
}


/**
  Send enable ACPI mode command to EC

  @param[in]  Enable            TURE - Enable EC ACPI mode
                                FALSE - Disable EC ACPI mode

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
EnableEcAcpiMode (
  IN BOOLEAN      Enable
  )
{
  return EFI_SUCCESS;
}


/**
  Send disable APM mode command to EC

  @param  EFI_SUCCESS         Command success
  @param  EFI_DEVICE_ERROR    Command error
**/
EFI_STATUS
DisableEcSmiMode (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Set EC critical shutdown temperature

  @param[in]  DataBuffer          Shutdown temperature
  @retval     EFI_SUCCESS         Command success
  @retval     EFI_DEVICE_ERROR    Command error
**/
EFI_STATUS
SetEcCriticalShutdownTemperature (
  IN UINT8   DataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Get power state from EC.  If power state cannot be determined,
  battery powered is assumed.

  @param  TRUE         AC powered
  @param  FALSE        Battery powered
**/
BOOLEAN
PowerStateIsAc (
  VOID
  )
{
  return TRUE;
}

/**
  Do EC reset

  @retval  EFI_SUCCESS         Command success
  @retval  EFI_DEVICE_ERROR    Command error
**/
EFI_STATUS
EcReset (
  VOID
  )
{
  return EFI_SUCCESS;
}

//
// ROYAL_PARK_OVERRIDE: RoyalParkOverrideBegin RPPO-SKL-0007
//
/**
  Initiate EC reset sequence. EC is in Normal Mode.

  @retval  EFI_SUCCESS         Command success
  @retval  EFI_DEVICE_ERROR    Command error
**/
EFI_STATUS
EcResetEcInNormalMode (
  VOID
  )
{
  return EFI_SUCCESS;
}
//
// ROYAL_PARK_OVERRIDE: RoyalParkOverrideEnd
//

///
/// @todo: it should move to BaseEcPowerLib once BaseEcPowerLib has been created.
///
/**
  Enable LAN power

  @param[in]  Enable            TURE - Enable LAN power
                                FALSE - Disable LAN power

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
EnableLanPower (
  IN BOOLEAN      Enable
  )
{
  return EFI_SUCCESS;
}

/**
  Query SMI event

  @param[out] DataBuffer        SMI event

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
QuerySmiEvent (
  OUT UINT8    *DataBuffer
  )
{
  return EFI_SUCCESS;
}


/**
  Get the scan code of hotkey pressed (CTRL + ALT + SHIFT + key)

  @param[out] DataBuffer        Scan code of hotkey

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
GetHotkeyScanCode (
  OUT UINT8    *DataBuffer
  )
{
  return EFI_SUCCESS;
}


/**
  Get the number of battery

  @param[out] DataBuffer        The number of battery

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
GetBatteryNumber (
  OUT UINT8    *DataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Get PCIe dock status

  @param[out] DataBuffer        PCIe dock status

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
GetPcieDockStatus (
  OUT UINT8    *DataBuffer
  )
{
  return EFI_SUCCESS;
}


/**
  Clear ten second power button mode

**/
VOID
ClearTenSecPwrButtonMode (
  VOID
  )
{
  return;
}

/**
  Enable SATA port2 power

**/
VOID
EnableSataPort2Power (
  VOID
  )
{
  return;
}

/**
  Detect SKL Premium Pmic Vendor ID

  @retval     Pmic Vendor ID present on the system
**/
EFI_STATUS
DetectPmicVendorID (
  OUT UINT8       *DataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Set PMIC SLP S0, V085ACNT[7:6] Register

  @param[in]  EcDataV085ACNT

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
SetSlpS0Voltage (
  IN UINT8       EcDataV085ACNT
  )
{
  return EFI_SUCCESS;
}

/**
  Set Pmic Voltage

  @param[in]  EcDataVcc
  @param[in]  EcDataVddq

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
SetPmicVoltage (
  IN UINT8       EcDataVcc,
  IN UINT8       EcDataVddq
  )
{
  return EFI_SUCCESS;
}


/**
  Read data from EC RAM

  @param[in][out]  DataBuffer

  @retval          EFI_SUCCESS       Command success
  @retval          EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
ReadEcRam (
  IN OUT UINT8       *DataBuffer
  )
{
  return EFI_SUCCESS;
}


/**
  Write data to EC RAM

  @param[in][out]  DataBuffer

  @retval          EFI_SUCCESS       Command success
  @retval          EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
WriteEcRam (
  IN OUT UINT8       *DataBuffer
  )
{
  return EFI_SUCCESS;
}


/**
  Update Device Power

  @retval          EFI_SUCCESS       Command success
  @retval          EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
UpdateDevicePower (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Query system status

  @param[out] DataBuffer

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
QuerySysStatus (
  OUT UINT8       *DataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Get board fab ID

  @param[out] DataBuffer

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
GetBoardFabId (
  OUT UINT8       *DataBuffer
  )
{
  return EFI_SUCCESS;
}


/**
  Send Exit Low Power Mode command to EC

  @retval          EFI_SUCCESS       Command success
  @retval          EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
ExitLowPowerMode (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  System power off

  @retval          EFI_SUCCESS       Command success
  @retval          EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
SystemPowerOff (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Detect EC revision

  @param[out] DataBuffer

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
DetectEcRevision (
  OUT UINT8       *DataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Get Base EC FW version

  @param[out] DataBuffer        Byte 1: Base EC FW major version
                                Byte 2: Base EC FW minor version

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
GetBaseEcFwVersion (
  OUT UINT8    *DataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Get EC-EC Protocol version

  @param[out] DataBuffer        Byte 1: Lid EC protocol version
                                Byte 2: Base EC protocol version

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
GetEc2EcProtocolVersion (
  OUT UINT8    *DataBuffer
  )
{
  return EFI_SUCCESS;
}

/**
  Disable PCH DTS reading on SMLink 1

  @param[in]  Disable           0: Enable  PCH DTS reading
                                1: Disable PCH DTS reading

  @retval     EFI_SUCCESS       Command success
  @retval     EFI_DEVICE_ERROR  Command error
**/
EFI_STATUS
DisablePchDtsReading (
  IN UINT8       Disable
  )
{
  return EFI_SUCCESS;
}

/**
  Wake up EC

  @retval     TRUE            EC wake up successfully.
  @retval     FALSE           Fail to wake up EC.
**/
EFI_STATUS
WakeUpEc (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Send Update PWM command to EC

  @param  EFI_SUCCESS         Command success
  @param  EFI_DEVICE_ERROR    Command error
**/
EFI_STATUS
UpdatePwm (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Updates the PWM to the given value  and returns the Fan speed.
  @param  Pwmvalue            Pwm ending value
  @param  Speed               Updates the Speed

  @retval EFI_SUCCESS         when successfully able to get the nominal data.
  @retval ERROR               Error if anything went wrong.

**/
EFI_STATUS
GetFanSpeed(
 IN  UINT8   Pwmvalue,
 OUT UINT16  *Speed
)
{
  return EFI_SUCCESS;
}
