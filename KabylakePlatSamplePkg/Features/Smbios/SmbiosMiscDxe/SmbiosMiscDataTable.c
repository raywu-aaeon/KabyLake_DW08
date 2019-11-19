/** @file
  This driver parses the mSmbiosMiscDataTable structure and reports
  any generated data using SMBIOS protocol.

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
// External definitions referenced by Data Table entries.
//

MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE0, MiscBiosVendor, MiscBiosVendor);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE1, MiscSystemManufacturer, MiscSystemManufacturer);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE2, MiscBaseBoardManufacturer, MiscBaseBoardManufacturer);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE3, MiscChassisManufacturer, MiscChassisManufacturer);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE32, MiscBootInfoStatus, MiscBootInfoStatus);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE13, NumberOfInstallableLanguages, NumberOfInstallableLanguages);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE12, SystemOptionString, SystemOptionString);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortKeyboard, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortMouse, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortCom1, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortVideo, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortHDMI, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortUsb1, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortUsb2, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortUsb3, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortUsb4, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortUsb5, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortUsb201, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortUsb202, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortNetwork, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortIde1, MiscPortIde1);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortSata0, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPorteSata4, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPorteSata3, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortSata2, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortSata1, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortACIN, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortPCHJTAG, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortPORT80, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPort2X8HEADER, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPort8PINHEADER, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortHDAHDMI, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortMKEYBOARD, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortSPI, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortLPCDOCKING, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortSIDEBAND, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortLPCSLOT, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortPCHXDP, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortSATAPOWER, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortFPHEADER, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortATXPOWER, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortAVMC, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortBATTB, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortBATTA, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortCPUFAN, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortXDP, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortMEMORY1, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortMEMORY2, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE8, MiscPortFANPWR, MiscPortInternalConnectorDesignator);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE10, MiscOnBoardDeviceVideo, MiscOnBoardDevice);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE10, MiscOnBoardDeviceEthernet, MiscOnBoardDevice);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE10, MiscOnBoardDeviceSound, MiscOnBoardDevice);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE0x88, MiscOemType0x88, MiscOemType0x88);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE22, MiscPortableBatteryReal1, MiscPortableBattery);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE22, MiscPortableBatteryReal2, MiscPortableBattery);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE22, MiscPortableBatteryVirtual, MiscPortableBattery);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE27, MiscCoolingDevice, MiscCoolingDevice);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE27, MiscCoolingDevice1, MiscCoolingDevice);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE39, MiscSystemPowerSupply, MiscSystemPowerSupply);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE41, MiscOnBoardDeviceExtendedVideo, MiscOnBoardDevicesExtended);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE41, MiscOnBoardDeviceExtendedEthernet, MiscOnBoardDevicesExtended);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE41, MiscOnBoardDeviceExtendedSound, MiscOnBoardDevicesExtended);
MISC_SMBIOS_TABLE_EXTERNS (SMBIOS_TABLE_TYPE41, MiscOnBoardDeviceExtendedSATAController, MiscOnBoardDevicesExtended);
//
// Data Table
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_MISC_SMBIOS_DATA_TABLE mSmbiosMiscDataTable[] = {
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscBiosVendor, MiscBiosVendor),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscSystemManufacturer, MiscSystemManufacturer),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscBaseBoardManufacturer, MiscBaseBoardManufacturer),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscChassisManufacturer, MiscChassisManufacturer),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscBootInfoStatus, MiscBootInfoStatus),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (NumberOfInstallableLanguages, NumberOfInstallableLanguages),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (SystemOptionString, SystemOptionString),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortKeyboard, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortMouse, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortCom1, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortVideo, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortHDMI, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortUsb1, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortUsb2, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortUsb3, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortUsb4, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortUsb5, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortUsb201, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortUsb202, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortNetwork, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortSata0, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPorteSata4, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPorteSata3, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortSata2, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortSata1, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortACIN, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortPCHJTAG, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortPORT80, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPort2X8HEADER, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPort8PINHEADER, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortHDAHDMI, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortMKEYBOARD, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortSPI, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortLPCDOCKING, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortSIDEBAND, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortLPCSLOT, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortPCHXDP, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortSATAPOWER, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortFPHEADER, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortATXPOWER, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortAVMC, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortBATTB, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortBATTA, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortCPUFAN, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortXDP, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortMEMORY1, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortMEMORY2, MiscPortInternalConnectorDesignator),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscPortFANPWR, MiscPortInternalConnectorDesignator),

  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscOnBoardDeviceVideo, MiscOnBoardDevice),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscOnBoardDeviceEthernet, MiscOnBoardDevice),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscOnBoardDeviceSound, MiscOnBoardDevice),

  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscOemType0x88, MiscOemType0x88),

  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscPortableBatteryReal1, MiscPortableBattery),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscPortableBatteryReal2, MiscPortableBattery),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(MiscPortableBatteryVirtual, MiscPortableBattery),

  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscCoolingDevice, MiscCoolingDevice),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscCoolingDevice1, MiscCoolingDevice),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscSystemPowerSupply, MiscSystemPowerSupply),

  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscOnBoardDeviceExtendedVideo, MiscOnBoardDevicesExtended),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscOnBoardDeviceExtendedEthernet, MiscOnBoardDevicesExtended),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscOnBoardDeviceExtendedSound, MiscOnBoardDevicesExtended),
  MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION (MiscOnBoardDeviceExtendedSATAController, MiscOnBoardDevicesExtended)
};

//
// Number of Data Table entries.
//
GLOBAL_REMOVE_IF_UNREFERENCED UINTN mSmbiosMiscDataTableEntries =
  (sizeof mSmbiosMiscDataTable) / sizeof(EFI_MISC_SMBIOS_DATA_TABLE);
