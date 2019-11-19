/** @file
  Type 28: Temperature Probe.
  Type 34: Management Device.
  Type 35: Management Device Component.
  Type 36: Management Device Threshold Data.
  Type 38: IPMI Device Information.
  This driver will determine the default value settting for following types:
  - SMBIOS_TABLE_TYPE28.
  - SMBIOS_TABLE_TYPE34.
  - SMBIOS_TABLE_TYPE35.
  - SMBIOS_TABLE_TYPE36.
  - SMBIOS_TABLE_TYPE38.

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

// Type 28: Temperature Probe
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(EFI_MISC_TEMPERATURE_PROBE_DESCRIPTION, MiscTemperatureProbe) = {
  STRING_TOKEN(STR_MISC_TEMPERATURE_PROBE),     // Description
  // Location and Status
  {
    2,
    2,
    0
  },
  0x8000,   // TemperatureProbeMaximumValue
  0x8000,   // TemperatureProbeMinimumValue
  0x8000,   // TemperatureProbeResolution
  0x8000,   // TemperatureProbeTolerance
  0x8000,   // TemperatureProbeAccuracy
  0x8000    // TemperatureProbeNormalValue
};

// Type 34: Management Device
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(EFI_MISC_MANAGEMENT_DEVICE_DESCRIPTION, MiscManagementDevice) = {
  STRING_TOKEN(STR_MISC_MANAGEMENT_DEVICE_STRING),  // Description
  EfiManagementDeviceTypeUnknown,                   // Type
  0,                                                // Address
  EfiManagementDeviceAddressTypeUnknown             // Address Type
};

// Type 35: Management Device Component
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(EFI_MISC_MANAGEMENT_DEVICE_COMPONENT_DESCRIPTION, MiscManagementDeviceComponent) = {
  STRING_TOKEN(STR_MISC_OEM),       // Description
  {
    EFI_MISC_SUBCLASS_GUID, // ProducerName
    1,                      // Instance
    1,                      // SubInstance
  },
  {
    EFI_MISC_SUBCLASS_GUID, // ProducerName
    1,                      // Instance
    1,                      // SubInstance
  },
  {
    EFI_MISC_SUBCLASS_GUID, // ProducerName
    1,                      // Instance
    1,                      // SubInstance
  },
  27                        // Component Type, according to SMBIOS spec, it can be Type 26, 27, 28, 29
};

// Type 36: Management Device Threshold
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(EFI_MISC_MANAGEMENT_DEVICE_THRESHOLD, MiscManagementDeviceThreshold) = {
  0x8000,   // LowerThresNonCritical
  0x8000,   // UpperThresNonCritical
  0x8000,   // LowerThresCritical
  0x8000,   // UpperThresCritical
  0x8000,   // LowerThresNonRecover
  0x8000    // UpperThresNonRecover
};

// Type 38: IPMI Device Information
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(EFI_MISC_IPMI_INTERFACE_TYPE_DATA, MiscIpmiInterface) = {
  EfiIpmiKcs,   // Interface Type
  // IPMI Specification Revision
  {
    0,
    2,
    0
  },
  0x20,         // I2C Slave Address
  0xFF,         // NV Storage Device Address
  0xCA3,        // Base Address
  0             // DevicePath
};
