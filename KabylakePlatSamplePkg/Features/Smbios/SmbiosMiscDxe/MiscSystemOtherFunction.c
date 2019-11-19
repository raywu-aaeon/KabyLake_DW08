/** @file
  Temperature Probe boot time changes - SMBIOS Type28.
  Management Device boot time chages - SMBIOS Type34.
  Management Device Component boot time chages - SMBIOS Type35.
  Management Device Threshold Data boot time changes - SMBIOS Type36.
  IPMI Device Information boot time changes - SMBIOS Type38.

  @par Revision Reference:
  SMBIOS Specification version 2.8.0 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
   - Human Interface Infrastructure Specification
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

MISC_SMBIOS_TABLE_FUNCTION(MiscTemperatureProbe) {
  CHAR8 *OptionalStrStart;
  EFI_STATUS Status;
  EFI_SMBIOS_HANDLE                     SmbiosHandle;
  SMBIOS_TABLE_TYPE28                   *SmbiosRecord;
  EFI_MISC_TEMPERATURE_PROBE_LOCATION   *ForType28InputData;
  EFI_STRING                            Description;
  STRING_REF                            TokenToGet;
  UINTN                                 DescriptionStrLen;

  ForType28InputData = (EFI_MISC_TEMPERATURE_PROBE_LOCATION *)RecordData;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_TEMPERATURE_PROBE);
  Description = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
  DescriptionStrLen = StrLen(Description);
  if (DescriptionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE28) + DescriptionStrLen + 1 + 1);
  ASSERT (SmbiosRecord != NULL);
  if (SmbiosRecord != NULL) {
    ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE3) + DescriptionStrLen + 1 + 1);

    SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_TEMPERATURE_PROBE;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE28);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;
    //
    // Description will be the 1st optional string following the formatted structure.
    //
    SmbiosRecord->Description = 1;


    SmbiosRecord->LocationAndStatus.TemperatureProbeSite = (UINT8)ForType28InputData->TemperatureProbeSite;
    SmbiosRecord->LocationAndStatus.TemperatureProbeStatus = (UINT8)ForType28InputData->TemperatureProbeStatus;
    SmbiosRecord->MaximumValue  = 0x8000;
    SmbiosRecord->MinimumValue  = 0x8000;
    SmbiosRecord->Resolution = 0x8000;
    SmbiosRecord->Tolerance = 0x8000;
    SmbiosRecord->Accuracy = 0x8000;
    SmbiosRecord->OEMDefined = 0;
    SmbiosRecord->NominalValue = 0x8000;

    OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);

    UnicodeStrToAsciiStrS (Description, OptionalStrStart, DescriptionStrLen);

    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

    FreePool(SmbiosRecord);
  }
  return Status;
}

MISC_SMBIOS_TABLE_FUNCTION(MiscManagementDevice) {
  EFI_STATUS Status;
  CHAR8                           *OptionalStrStart;
  UINTN                           DescriptionStrLen;
  EFI_STRING                      Description;
  STRING_REF                      TokenToGet;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  SMBIOS_TABLE_TYPE34             *SmbiosRecord;
  EFI_MISC_MANAGEMENT_DEVICE_DESCRIPTION   *ForType34InputData;

  ForType34InputData = (EFI_MISC_MANAGEMENT_DEVICE_DESCRIPTION *)RecordData;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_MANAGEMENT_DEVICE_STRING);
  Description = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
  DescriptionStrLen = StrLen(Description);
  if (DescriptionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE34) + DescriptionStrLen + 1 + 1);
  ASSERT (SmbiosRecord != NULL);
  if (SmbiosRecord != NULL) {
    ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE34) + DescriptionStrLen + 1 + 1);

    SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_MANAGEMENT_DEVICE;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE34);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;
    //
    // Description will be the 1st optional string following the formatted structure.
    //
    SmbiosRecord->Description = 1;

    SmbiosRecord->Type = ForType34InputData->ManagementDeviceType;
    SmbiosRecord->Address  = (UINT32)ForType34InputData->ManagementDeviceAddress;
    SmbiosRecord->AddressType = ForType34InputData->ManagementDeviceAddressType;

    OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);

    UnicodeStrToAsciiStrS (Description, OptionalStrStart, DescriptionStrLen);

    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

    FreePool(SmbiosRecord);
  }
  return Status;
}

MISC_SMBIOS_TABLE_FUNCTION(MiscManagementDeviceComponent) {
  EFI_STATUS Status;
  CHAR8                           *OptionalStrStart;
  UINTN                           DescriptionStrLen;
  EFI_STRING                      Description;
  STRING_REF                      TokenToGet;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  SMBIOS_TABLE_TYPE35              *SmbiosRecord;
  EFI_MISC_BASE_BOARD_MANUFACTURER   *ForType35InputData;

  ForType35InputData = (EFI_MISC_BASE_BOARD_MANUFACTURER *)RecordData;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_OEM);
  Description = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
  DescriptionStrLen = StrLen(Description);
  if (DescriptionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE35) + DescriptionStrLen + 1 + 1);
  ASSERT (SmbiosRecord != NULL);
  if (SmbiosRecord != NULL) {
    ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE35) + DescriptionStrLen + 1 + 1);

    SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE35);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;
    //
    // Description will be the 1st optional string following the formatted structure.
    //
    SmbiosRecord->Description = 1;

    SmbiosRecord->ManagementDeviceHandle =0x0d;
    SmbiosRecord->ComponentHandle = 0x0a;
    SmbiosRecord->ThresholdHandle = 0x0f;

    OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);

    UnicodeStrToAsciiStrS (Description, OptionalStrStart, DescriptionStrLen);

    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

    FreePool(SmbiosRecord);
  }
  return Status;
}

MISC_SMBIOS_TABLE_FUNCTION(MiscManagementDeviceThreshold) {
  EFI_STATUS Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  SMBIOS_TABLE_TYPE36              *SmbiosRecord;
  EFI_MISC_MANAGEMENT_DEVICE_THRESHOLD   *ForType36InputData;

  ForType36InputData = (EFI_MISC_MANAGEMENT_DEVICE_THRESHOLD *)RecordData;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE36) + 1 +1);
  ASSERT (SmbiosRecord != NULL);
  if (SmbiosRecord != NULL) {
    ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE36)  + 1 +1);

    SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE36);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;

    SmbiosRecord->LowerThresholdNonCritical = ForType36InputData->LowerThresNonCritical;
    SmbiosRecord->UpperThresholdNonCritical  = ForType36InputData->UpperThresNonCritical;
    SmbiosRecord->LowerThresholdCritical      = ForType36InputData->LowerThresCritical;
    SmbiosRecord->UpperThresholdCritical = ForType36InputData->UpperThresCritical;
    SmbiosRecord->LowerThresholdNonRecoverable = ForType36InputData->LowerThresNonRecover;
    SmbiosRecord->UpperThresholdNonRecoverable = ForType36InputData->UpperThresNonRecover;

    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

    FreePool(SmbiosRecord);
  }
  return Status;
}



MISC_SMBIOS_TABLE_FUNCTION(MiscIpmiInterface){
  EFI_STATUS Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  SMBIOS_TABLE_TYPE38              *SmbiosRecord;
  EFI_MISC_IPMI_INTERFACE_TYPE_DATA   *ForType38InputData;

  ForType38InputData = (EFI_MISC_IPMI_INTERFACE_TYPE_DATA *)RecordData;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE38) + 1 +1);
  ASSERT (SmbiosRecord != NULL);
  if (SmbiosRecord != NULL) {
    ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE38) + 1 +1);

    SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE38);
    //
    // Make handle chosen by smbios protocol.add automatically.
    //
    SmbiosRecord->Hdr.Handle = 0;

    SmbiosRecord->InterfaceType = ForType38InputData->IpmiInterfaceType;
    SmbiosRecord->IPMISpecificationRevision  = *((UINT8 *)&(ForType38InputData->IpmiSpecificationRevision));
    SmbiosRecord->I2CSlaveAddress      = (UINT8)ForType38InputData->IpmiI2CSlaveAddress;
    SmbiosRecord->NVStorageDeviceAddress = (UINT8)ForType38InputData->IpmiNvDeviceAddress;
    SmbiosRecord->BaseAddress = ForType38InputData->IpmiBaseAddress;

    //
    // Now we have got the full smbios record, call smbios protocol to add this record.
    //
    Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

    FreePool(SmbiosRecord);
  }
  return Status;
}
