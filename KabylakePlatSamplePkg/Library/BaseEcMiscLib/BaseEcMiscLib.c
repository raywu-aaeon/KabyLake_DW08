/** @file
  BaseEcMisc library implementation.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2018 Intel Corporation.

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

#ifndef MINTREE_FLAG
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
  UINT8  DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_USBC_SX_ENTRY_WAIT, &DataSize, DataBuffer));
}
#endif

/**
  Send USBC Sx Exit command to EC

  @param[out]     DataBuffer

  @retval         EFI_SUCCESS         Command success
  @retval         EFI_DEVICE_ERROR    Command error
**/
EFI_STATUS
UsbcSxExit (
  OUT UINT8       *DataBuffer
  )
{
  UINT8  DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_USBC_SX_EXIT_WAIT, &DataSize, DataBuffer));
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
  if (Enable == TRUE) {
    return (EcInterface (EcId0, LpcInterface, EC_C_ACPI_ENABLE, NULL, NULL));
  } else {
    return (EcInterface (EcId0, LpcInterface, EC_C_ACPI_DISABLE, NULL, NULL));
  }
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
  return (EcInterface (EcId0, LpcInterface, EC_C_SMI_DISABLE, NULL, NULL));
}

#ifndef MINTREE_FLAG
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
  UINT8  DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_SET_CTEMP, &DataSize, &DataBuffer));
}
#endif

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
  EFI_STATUS  Status;
  UINT8       PortDataOut;
  UINT8       DataSize;

  ///
  /// Query EC for system status
  ///
  PortDataOut = 0;
  DataSize = 1;
  Status = EcInterface (EcId0, LpcInterface, EC_C_QUERY_SYS_STATUS, &DataSize, &PortDataOut);
  ///
  /// At this point, we expect this to always succeed
  ///
  ASSERT_EFI_ERROR (Status);

  ///
  /// Return power state
  /// If we had an issue, we default to battery mode to save a small amount of power.
  ///
  if (!EFI_ERROR (Status) && PortDataOut & EC_B_SYS_STATUS_AC) {
    return TRUE;
  } else {
    return FALSE;
  }
}

#ifndef MINTREE_FLAG
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
  return (EcInterface (EcId0, LpcInterface, EC_C_RESET, NULL, NULL));
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
  return(LpcEcResetEcInNormalMode());
}
//
// ROYAL_PARK_OVERRIDE: RoyalParkOverrideEnd
//
#endif

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
  if (Enable == TRUE) {
    return (EcInterface (EcId0, LpcInterface, EC_C_LAN_ON, NULL, NULL));
  } else {
    return (EcInterface (EcId0, LpcInterface, EC_C_LAN_OFF, NULL, NULL));
  }
}

#ifndef MINTREE_FLAG
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
  UINT8       DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_SMI_QUERY, &DataSize, DataBuffer));
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
  UINT8       DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_SMI_HOTKEY, &DataSize, DataBuffer));
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
  UINT8       DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_QUERY_SYS_STATUS, &DataSize, DataBuffer));
}
#endif


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
  UINT8       DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_DOCK_STATUS, &DataSize, DataBuffer));
}


/**
  Clear ten second power button mode

**/
VOID
ClearTenSecPwrButtonMode (
  VOID
  )
{
  EFI_STATUS            Status;
  UINT8                 DataBuffer[2];

  DataBuffer[0] = EC_D_TEN_SEC_MODE;
  Status = ReadEcRam (DataBuffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }
  DataBuffer[1] = DataBuffer[0] & ~(BIT1); // Clear 10sec Power button Override in EC
  DataBuffer[0] = EC_D_TEN_SEC_MODE;
  Status = WriteEcRam (DataBuffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }
  Status = UpdateDevicePower ();
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }
}

#ifndef MINTREE_FLAG
/**
  Enable SATA port2 power

**/
VOID
EnableSataPort2Power (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT8                           DataBuffer[2];

  DataBuffer[0] = EC_D_SATA2_POWER;
  Status = ReadEcRam (DataBuffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }
  DataBuffer[1] = DataBuffer[0] | BIT0;
  DataBuffer[0] = EC_D_SATA2_POWER;
  Status = WriteEcRam (DataBuffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }
  Status = UpdateDevicePower ();
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }
}
#endif

/**
  Detect SKL Premium Pmic Vendor ID

  @retval     Pmic Vendor ID present on the system
**/
EFI_STATUS
DetectPmicVendorID (
  OUT UINT8       *DataBuffer
  )
{
  UINT8   DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_GET_PMIC_VENDOR_ID, &DataSize, DataBuffer));
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
  UINT8          DataSize;
  UINT8          DataBuffer[1];

  DataBuffer[0] = EcDataV085ACNT;
  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_SLP_S0_VOLTAGE, &DataSize, DataBuffer));
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
  UINT8          DataSize;
  UINT8          DataBuffer[2];

  DataBuffer[0] = EcDataVcc;
  DataBuffer[1] = EcDataVddq;
  DataSize = 2;
  return (EcInterface (EcId0, LpcInterface, EC_C_PMIC_VOLTAGE, &DataSize, DataBuffer));
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
  UINT8          DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_READ_MEM, &DataSize, DataBuffer));
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
  UINT8          DataSize;

  DataSize = 2;
  return (EcInterface (EcId0, LpcInterface, EC_C_WRITE_MEM, &DataSize, DataBuffer));
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
  return (EcInterface (EcId0, LpcInterface, EC_C_UPDATE_EC, NULL, NULL));
}

#ifndef MINTREE_FLAG
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
  UINT8          DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_QUERY_SYS_STATUS, &DataSize, DataBuffer));
}
#endif

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
  UINT8   DataSize;

  //
  // For 'EC_C_FAB_ID' command NumberOfSendData = 0, NumberOfReceiveData =2.
  //
  DataSize = 2;
  return (EcInterface (EcId0, LpcInterface, EC_C_FAB_ID, &DataSize, DataBuffer));
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
  return (EcInterface (EcId0, LpcInterface, EC_C_LOW_POWER_EXIT, NULL, NULL));
}

#ifndef MINTREE_FLAG
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
  return (EcInterface (EcId0, LpcInterface, EC_C_SYSTEM_POWER_OFF, NULL, NULL));
}
#endif

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
  UINT8      DataSize;

  DataSize = sizeof (DataBuffer);
  return (EcInterface (EcId0, LpcInterface, EC_C_SMC_GET_MODE, &DataSize, DataBuffer));
}

#ifndef MINTREE_FLAG
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
  UINT8       DataSize;

  DataSize = 2;
  return (EcInterface (EcId0, LpcInterface, EC_C_BASE_EC_FW_VERSION, &DataSize, DataBuffer));
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
  UINT8       DataSize;

  DataSize = 2;
  return (EcInterface (EcId0, LpcInterface, EC_C_EC_2_EC_PROTOCOL_VERSION, &DataSize, DataBuffer));
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
  UINT8          DataSize;

  DataSize = 1;
  return (EcInterface (EcId0, LpcInterface, EC_C_DIS_PCH_DTS_READ, &DataSize, &Disable));
}
#endif

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
  return ExitLowPowerMode ();
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
  return (EcInterface (EcId0, LpcInterface, EC_C_UPDATE_PWM, NULL, NULL));
}

#ifndef MINTREE_FLAG
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
  EFI_STATUS Status;
  UINT8      DataBuffer[2];

  Status = EFI_SUCCESS;
  //
  //Enable ACPI mode
  //
  Status = EnableEcAcpiMode(TRUE);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Write Pwm value at offset 0x44 to set the Fan speed
  //
  DataBuffer[1] = Pwmvalue;
  DataBuffer[0] = 0x44;
  Status = WriteEcRam(DataBuffer);
  if (EFI_ERROR(Status)) {
    goto Exit;
  }
  //
  // UPDATE_PWM command progamms the Fan Speed to Pwm value Value.
  //
  Status = UpdatePwm();
  if (EFI_ERROR(Status)) {
    goto Exit;
  }
  //
  // Read data from offset 0x74(Higher Byte) & 0x73(Lower Byte) for Fan Speed.
  //
  DataBuffer[0] = 0x74;
  Status = ReadEcRam(DataBuffer);
  if (EFI_ERROR(Status)) {
    goto Exit;
  }

  *Speed = (UINT16) (DataBuffer[0] << 8);

  DataBuffer[0] = 0x73;
  Status = ReadEcRam(DataBuffer);
  if (EFI_ERROR(Status)) {
    goto Exit;
  }

  *Speed = (UINT16) ((*Speed) | (DataBuffer[0]));
  DEBUG ((DEBUG_INFO, "Fan Speed : %04X \n",*Speed));

Exit:
  //
  //Disable ACPI mode
  //
  Status = EnableEcAcpiMode(FALSE);

  return Status;
}

//
// KblGBegin
//
/**
  Tell EC to Enable Power_En for dGPU RVP17

  @param  EFI_SUCCESS         Command success
  @param  EFI_DEVICE_ERROR    Command error
**/
EFI_STATUS
DgpuPowerEn (
  VOID
  )
{
  EFI_STATUS Status;
  UINT8 Data[2];
  UINT8 DataSize;

  DEBUG ((DEBUG_INFO, "Sending EC command 0x49\n"));

  //
  // Set EC ACPI Offset 0xF2 with value 0x1 followed by Command 0x49 in order to turn GPU_PWR_EN on
  //
  DataSize = 2;
  Data[0] = 0xF2;
  Data[1] = 1;
  Status = EcInterface (EcId0, LpcInterface, EC_C_WRITE_MEM, &DataSize, Data); //Write ACPI offset Command
  ASSERT_EFI_ERROR (Status);

  Status = SendEcCommand (EC_C_GPU_PWR_EN);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
//
// KblGEnd
//

#endif
