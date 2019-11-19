/** @file
  Portable Battery boot time changes.
  SMBIOS type 22.

  @par Revision Reference:
  SMBIOS Specification version 2.8.0 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
  Unified Extensible Firmware Interface (UEFI) Specifications: http://www.uefi.org/specs/

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#include "CommonHeader.h"
#include "SmbiosMisc.h"
#include <PlatformBoardId.h>

#define BATT_TYPE_UNKNOWN     0x00
#define BATT_TYPE_VIRTUAL     0x60
#define BATT_TYPE_REAL1       0x61
#define BATT_TYPE_REAL2       0x62

GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_INFO                 *mPlatformInfo;

/**
  Return the desired value from ACPI memory space. Assumes the EC has ACPI memory space read
  enabled.

  @param[in]    Register      Register to read from
  @param[out]   RetValue      Return value from EC

  @retval       EFI_SUCCESS   successful
  @retval       Other Vals    something wrong

**/
static EFI_STATUS
GetBatteryValue (
  IN  UINT8    Register,
  OUT UINT8   *RetValue
  )
{
  EFI_STATUS    Status;
  UINT8         DataBuffer[1];

  DataBuffer[0] = Register;
  Status = ReadEcRam (DataBuffer);
  if (Status == EFI_SUCCESS) {
    *RetValue = DataBuffer[0];
  }

  return Status;
}

/**
  Get the specific battery information through EC part.

  @param[out]   *SmbiosRecord           Return the battery information for the
                                        installation of SMBIOS Table type22.
  @param[in]    BatteryType             The type of the battery, virtual, real1 or
                                        real2.
  @param[out]   *InstallBattery         Does the battery information need to be
                                        added into SMBIOS tables.

  @retval       EFI_SUCCESS             All parameters are valid.
  @retval       EFI_INVALID_PARAMETER   Invalid parameter is found.
  @retval       EFI_OUT_OF_RESOURCES    Locate PlatformInfo failed.
  @retval       EFI_OUT_OF_RESOURCES    The platform is not needed to install
                                        the battery information.
  @retval       EFI_OUT_OF_RESOURCES    Initialize EC library failed.

**/
EFI_STATUS
UpdateBatteryInfo (
  OUT SMBIOS_TABLE_TYPE22    *SmbiosRecord,
  IN  UINT8                  BatteryType,
  OUT BOOLEAN                *InstallBattery
)
{
  static BOOLEAN            NeedVirtualBattery = TRUE;
  EFI_STATUS                Status;
  UINT8                     RegData;
  UINT16                    Value;
  PLATFORM_INFO             *PlatformInfo;

  RegData = 0;
  Value   = 0;

  if (SmbiosRecord == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gPlatformInfoProtocolGuid,
                  NULL,
                  (VOID **) &mPlatformInfo
                  );

  if ((mPlatformInfo->PlatformFlavor != FlavorMobile) &&
      (mPlatformInfo->PlatformFlavor != FlavorEmbedded)) {
    *InstallBattery = FALSE;
    DEBUG ((DEBUG_ERROR, "Battery is not supported in this platform \n"));
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                &gPlatformInfoProtocolGuid,
                NULL,
                (VOID **) &PlatformInfo
                );
  if (!EFI_ERROR (Status)) {
    if (!PlatformInfo->EcPresent) {
      *InstallBattery = FALSE;
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to locate PlatformInfo Protocol!\n"));
    ASSERT_EFI_ERROR (Status);
  }

  switch (BatteryType) {
    case BATT_TYPE_REAL1:
      Status = GetBatteryValue (EC_REAL_BAT1_STATUS, &RegData);
      DEBUG ((DEBUG_INFO, "Battery 1 Status = %x\n", RegData));
      if (Status == EFI_SUCCESS) {
        if (RegData & EC_REAL_BAT_PRESENT_MASK) {
          DEBUG ((DEBUG_INFO, "Adding Battery 1\n"));
          NeedVirtualBattery = FALSE;
          Status = GetBatteryValue (EC_REAL_BAT1_REMAINING_CAPACITY + 1, &RegData); // Get uppder Byte
          Value = 0;
          Value |= (UINT16)RegData;
          Value = Value << 8;
          Status = GetBatteryValue (EC_REAL_BAT1_REMAINING_CAPACITY, &RegData); // Get lower Byte
          Value |= (UINT16)RegData;
          SmbiosRecord->DeviceCapacity = Value;
          Status = GetBatteryValue (EC_REAL_BAT1_RESOLUTION_VOLTAGE + 1, &RegData); // Get uppder Byte
          Value = 0;
          Value |= (UINT16)RegData;
          Value = Value << 8;
          Status = GetBatteryValue (EC_REAL_BAT1_RESOLUTION_VOLTAGE, &RegData); // Get lower Byte
          Value |= (UINT16)RegData;
          SmbiosRecord->DesignVoltage = Value;
          SmbiosRecord->DeviceChemistry = EfiPortableBatteryDeviceChemistryLithiumIon;
          *InstallBattery = TRUE;
        }
      } else {
          *InstallBattery = FALSE;
      }
      break;
    case BATT_TYPE_REAL2:
      Status = GetBatteryValue (EC_REAL_BAT2_STATUS, &RegData);
      DEBUG ((DEBUG_INFO, "Battery 2 Status = %x\n", RegData));
      if (Status == EFI_SUCCESS) {
        if (RegData & EC_REAL_BAT_PRESENT_MASK) {
          DEBUG ((DEBUG_INFO, "Adding Battery 2\n"));
          NeedVirtualBattery = FALSE;
          Status = GetBatteryValue(EC_REAL_BAT2_REMAINING_CAPACITY + 1, &RegData); // Get uppder Byte
          Value = 0;
          Value |= (UINT16)RegData;
          Value = Value << 8;
          Status = GetBatteryValue(EC_REAL_BAT2_REMAINING_CAPACITY, &RegData); // Get lower Byte
          Value |= (UINT16)RegData;
          SmbiosRecord->DeviceCapacity = Value;
          Status = GetBatteryValue(EC_REAL_BAT2_RESOLUTION_VOLTAGE + 1, &RegData); // Get uppder Byte
          Value = 0;
          Value |= (UINT16)RegData;
          Value = Value << 8;
          Status = GetBatteryValue(EC_REAL_BAT2_RESOLUTION_VOLTAGE, &RegData); // Get lower Byte
          Value |= (UINT16)RegData;
          SmbiosRecord->DesignVoltage = Value;
          SmbiosRecord->DeviceChemistry = EfiPortableBatteryDeviceChemistryLithiumIon;
          *InstallBattery = TRUE;
        }
      } else {
          *InstallBattery = FALSE;
      }
      break;
    case BATT_TYPE_VIRTUAL:
      if (NeedVirtualBattery) {
        Status = GetBatteryValue (EC_VIRTUAL_BAT_STATUS, &RegData);
        DEBUG ((DEBUG_INFO, "Virtual Battery Status = %x\n", RegData));
        if (Status == EFI_SUCCESS) {
          if (RegData & EC_REAL_BAT_PRESENT_MASK) {
            *InstallBattery = TRUE;
          }
        }
      } else {
        *InstallBattery = FALSE;
      }
      break;
  }

  return EFI_SUCCESS;
}

/**
  This function makes boot time changes to the contents of the
  MiscPortableBatteryData.

  @param[in]  RecordData            Pointer to copy of RecordData from the
                                    data table.

  @retval     EFI_SUCCESS           All parameters were valid.
  @retval     EFI_INVALID_PARAMETER Invalid parameter was found.
  @retval     EFI_OUT_OF_RESOURCES  Can't allocate memory for the installation
                                    of SMBIOS tables.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscPortableBattery)
{
  EFI_STATUS                  Status;
  CHAR8                       *OptionalStrStart;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  UINTN                       StringNumber;
  UINTN                       StringLength;
  UINT8                       BatteryType;
  SMBIOS_TABLE_TYPE22         *SmbiosRecord;
  SMBIOS_TABLE_TYPE22         *ForType22InputData;
  SMBIOS_TABLE_TYPE22_STRINGS SmbiosTableType22Strings;
  BOOLEAN                     NeedToInstall = FALSE;
  CHAR16                      *StrBufferStart;
  CHAR16                      *StrBufferPtr;
  UINTN                       StrBuffSize;
  UINTN                       OptionalStrBufferSize;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "(MiscPortableBattery) Entry.\n"));
  ZeroMem (&SmbiosTableType22Strings, sizeof(SMBIOS_TABLE_TYPE22_STRINGS));
  StringNumber          = 0;
  StringLength          = 0;
  BatteryType           = BATT_TYPE_UNKNOWN;
  ForType22InputData    = (SMBIOS_TABLE_TYPE22 *)RecordData;
  StrBufferStart        = AllocateZeroPool (5 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16));
  StrBufferPtr          = StrBufferStart;
  StrBuffSize           = (5 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16)) / sizeof (CHAR16);

  //
  // Initialize SMBIOS Tables Type22 strings
  //
  switch (ForType22InputData->Location) {
    case SMBIOS_MISC_PROTABLE_BATTERY_TOKEN_REAL1:
      SmbiosStrInit (&SmbiosTableType22Strings.Location, SMBIOS_MISC_PORTABLE_BATTERY_REAL1_LOCATION, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.Manufacturer, SMBIOS_MISC_PORTABLE_BATTERY_REAL1_MANUFACTURER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.ManufacturerDate, SMBIOS_MISC_PORTABLE_BATTERY_REAL1_MANUFACTURER_DATE, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.SerialNumber, SMBIOS_MISC_PORTABLE_BATTERY_REAL1_SERIAL_NUMBER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.DeviceName, SMBIOS_MISC_PORTABLE_BATTERY_REAL1_DEVICE_NAME, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      BatteryType = BATT_TYPE_REAL1;
      break;
    case SMBIOS_MISC_PROTABLE_BATTERY_TOKEN_REAL2:
      SmbiosStrInit (&SmbiosTableType22Strings.Location, SMBIOS_MISC_PORTABLE_BATTERY_REAL2_LOCATION, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.Manufacturer, SMBIOS_MISC_PORTABLE_BATTERY_REAL2_MANUFACTURER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.ManufacturerDate, SMBIOS_MISC_PORTABLE_BATTERY_REAL2_MANUFACTURER_DATE, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.SerialNumber, SMBIOS_MISC_PORTABLE_BATTERY_REAL2_SERIAL_NUMBER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.DeviceName, SMBIOS_MISC_PORTABLE_BATTERY_REAL2_DEVICE_NAME, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      BatteryType = BATT_TYPE_REAL2;
      break;
    case SMBIOS_MISC_PROTABLE_BATTERY_TOKEN_VIRTUAL:
      SmbiosStrInit (&SmbiosTableType22Strings.Location, SMBIOS_MISC_PORTABLE_BATTERY_VIRTUAL_LOCATION, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.Manufacturer, SMBIOS_MISC_PORTABLE_BATTERY_VIRTUAL_MANUFACTURER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.ManufacturerDate, SMBIOS_MISC_PORTABLE_BATTERY_VIRTUAL_MANUFACTURER_DATE, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.SerialNumber, SMBIOS_MISC_PORTABLE_BATTERY_VIRTUAL_SERIAL_NUMBER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType22Strings.DeviceName, SMBIOS_MISC_PORTABLE_BATTERY_VIRTUAL_DEVICE_NAME, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      BatteryType = BATT_TYPE_VIRTUAL;
      break;
    default:
      break;
  }
  ForType22InputData->Location = SMBIOS_MISC_STRING_1;

  //
  // Two zeros following the last string.
  //
  OptionalStrBufferSize = StringLength + StringNumber + 1;
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE22) + OptionalStrBufferSize);
  if (SmbiosRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE22) + OptionalStrBufferSize);

  CopyMem (SmbiosRecord, RecordData, sizeof (SMBIOS_TABLE_TYPE22));

  //
  // Update SMBIOS Tables Type22 Strings
  //
  OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
  SmbiosStringsUpdate ((CHAR16 **) &SmbiosTableType22Strings, (UINT8 *) OptionalStrStart, StringNumber, OptionalStrBufferSize);

  //
  // Update Battery Information
  //
  Status = UpdateBatteryInfo (SmbiosRecord, BatteryType, &NeedToInstall);

  if (!EFI_ERROR(Status)) {
    if (NeedToInstall) {
      Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);
    }
  }

  FreePool(SmbiosRecord);
  FreePool(StrBufferStart);
  return Status;
}
