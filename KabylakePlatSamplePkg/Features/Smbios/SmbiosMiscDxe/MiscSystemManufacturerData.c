/** @file
  Type 1: System Information.
  This driver will determine the default value settting for SMBIOS_TABLE_TYPE1.

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

//
// Static (possibly build generated) System Manufacturer data.
//
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE1, MiscSystemManufacturer) = {
  {                                             // Hdr
    EFI_SMBIOS_TYPE_SYSTEM_INFORMATION,         /// Hdr.Type
    sizeof (SMBIOS_TABLE_TYPE1),                /// Hdr.Size
    0                                           /// Hdr.Handle
  },
  SMBIOS_MISC_STRING_1,                         // SystemManufactrurer
  SMBIOS_MISC_STRING_2,                         // SystemProductName
  SMBIOS_MISC_STRING_3,                         // SystemVersion
  SMBIOS_MISC_STRING_4,                         // SystemSerialNumber
  {                                             // SystemUuid
    0x5e24fe9c, 0xc8d0, 0x45bd, { 0xa7, 0x9f, 0x54, 0xea, 0x5f, 0xbd, 0x3d, 0x97 }
  },
  SystemWakeupTypeOther,                        // SystemWakeupType
  SMBIOS_MISC_STRING_5,                         // SystemSKUNumber
  SMBIOS_MISC_STRING_6                          // SystemFamily
};
