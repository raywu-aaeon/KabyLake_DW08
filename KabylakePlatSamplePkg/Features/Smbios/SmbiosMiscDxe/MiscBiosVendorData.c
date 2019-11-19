/** @file
  Type 0: Bios Vendor.
  This driver will determine the default value settting for SMBIOS_TABLE_TYPE0.

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

///
/// Bios Characteristics Extension setting
///

GLOBAL_REMOVE_IF_UNREFERENCED MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE0, MiscBiosVendor) = {
  {                                         // Hdr
    EFI_SMBIOS_TYPE_BIOS_INFORMATION,       /// Hdr.Type
    sizeof (SMBIOS_TABLE_TYPE0),            /// Hdr.Length
    0                                       /// Hdr.Handle
  },
  SMBIOS_MISC_STRING_1,                     // Vendor
  SMBIOS_MISC_STRING_2,                     // BiosVersion
  0x0000,                                   // BiosSegment
  SMBIOS_MISC_STRING_3,                     // BiosReleaseDate
  TO_BE_FILLED,                             // BiosSize = 64K *(n+1) where n=20 for 2MB
  {                                         // BiosCharacteristics
    0,                                      ///< Bit 0-1  Reserved1                         :2
    0,                                      ///< Bit 02 - Unknown                           :1
    0,                                      ///< Bit 03 - BiosCharacteristicsNotSupported   :1
    0,                                      ///< Bit 04 - IsaIsSupported                    :1
    0,                                      ///< Bit 05 - McaIsSupported                    :1
    0,                                      ///< Bit 06 - EisaIsSupported                   :1
    1,                                      ///< Bit 07 - PciIsSupported                    :1
    0,                                      ///< Bit 08 - PcmciaIsSupported                 :1
    0,                                      ///< Bit 09 - PlugAndPlayIsSupported            :1
    0,                                      ///< Bit 10 - ApmIsSupported                    :1
    1,                                      ///< Bit 11 - BiosIsUpgradable                  :1
    1,                                      ///< Bit 12 - BiosShadowingAllowed              :1
    0,                                      ///< Bit 13 - VLVesaIsSupported                 :1
    0,                                      ///< Bit 14 - EscdSupportIsAvailable            :1
    1,                                      ///< Bit 15 - BootFromCdIsSupported             :1
    1,                                      ///< Bit 16 - SelectableBootIsSupported         :1
    0,                                      ///< Bit 17 - RomBiosIsSocketed                 :1
    0,                                      ///< Bit 18 - BootFromPcmciaIsSupported         :1
    1,                                      ///< Bit 19 - EDDSpecificationIsSupported       :1
    0,                                      ///< Bit 20 - JapaneseNecFloppyIsSupported      :1
    0,                                      ///< Bit 21 - JapaneseToshibaFloppyIsSupported  :1
    0,                                      ///< Bit 22 - Floppy525_360IsSupported          :1
    0,                                      ///< Bit 23 - Floppy525_12IsSupported           :1
    0,                                      ///< Bit 24 - Floppy35_720IsSupported           :1
    0,                                      ///< Bit 25 - Floppy35_288IsSupported           :1
    1,                                      ///< Bit 26 - PrintScreenIsSupported            :1
    1,                                      ///< Bit 27 - Keyboard8042IsSupported           :1
    1,                                      ///< Bit 28 - SerialIsSupported                 :1
    1,                                      ///< Bit 29 - PrinterIsSupported                :1
    0,                                      ///< Bit 30 - CgaMonoIsSupported                :1
    0,                                      ///< Bit 31 - NecPc98                           :1
    0                                       ///< Bit 32-63 - Reserved for BIOS/System Vendor :32
  },
  {                                         // BiosCharacteristicsExtensionBytes
/**
  MISC_BIOS_CHARACTERISTICS_EXTENSION.BiosReserved
  1, Bit 0 - AcpiIsSupported                    :1
  1, Bit 1 - UsbLegacyIsSupported               :1
  0, Bit 2 - AgpIsSupported                     :1
  0, Bit 3 - I20BootIsSupported                 :1
  1, Bit 4 - Ls120BootIsSupported               :1
  1, Bit 5 - AtapiZipDriveBootIsSupported       :1
  0, Bit 6 - Boot1394IsSupported                :1
  0, Bit 7 - SmartBatteryIsSupported            :1
**/
   (UINT8)(BIT0 | BIT1 | BIT4 | BIT5),
/**
  MISC_BIOS_CHARACTERISTICS_EXTENSION.SystemReserved
  1, Bit 0 - BiosBootSpecIsSupported            :1
  1, Bit 1 - FunctionKeyNetworkBootIsSupported  :1
  1, Bit 2 - TargetContentDistributionEnabled   :1
  1, Bit 3 - UefiSpecificationSupported         :1
  0, Bit 4 - VirtualMachineSupported            :1
  0, Bit 5-7 - ExtensionByte2Reserved           :3
**/
    (UINT8)(BIT0 | BIT1 | BIT2 | BIT3)
  },
  0x0,                                      // System BIOS Major Release
  0x1,                                      // System BIOS Minor Release
  0xFF,                                     // Embedded controller firmware major Release
  0xFF                                      // Embedded controller firmware minor Release
};
