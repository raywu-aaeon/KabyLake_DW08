/** @file
  CPU Power Management VR Config Block.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2018 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _CPU_POWER_MGMT_VR_CONFIG_H_
#define _CPU_POWER_MGMT_VR_CONFIG_H_

#include <CpuPowerMgmt.h>

#define CPU_POWER_MGMT_VR_CONFIG_REVISION 5

extern EFI_GUID gCpuPowerMgmtVrConfigGuid;

#pragma pack (push,1)

///
/// Defines the maximum number of VR domains supported.
/// @warning: Changing this define would cause DWORD alignment issues in policy structures.
///
#define MAX_NUM_VRS         4

typedef enum {
  EnumUnknownCpuVrPowerDesign = 0,

  ///
  /// CFL-S Power Delivery Designs
  ///
  EnumMinCflVrPowerDesign      = CFL_S_35_WATT_2_2,
  EnumCflS35Watt22PowerDesign  = CFL_S_35_WATT_2_2,
  EnumCflS54Watt22PowerDesign  = CFL_S_54_WATT_2_2,
  EnumCflS58Watt22PowerDesign  = CFL_S_58_WATT_2_2,
  EnumCflS35Watt42PowerDesign  = CFL_S_35_WATT_4_2,
  EnumCflS62Watt42PowerDesign  = CFL_S_62_WATT_4_2,
  EnumCflS65Watt42PowerDesign  = CFL_S_65_WATT_4_2,
  EnumCflS95Watt42PowerDesign  = CFL_S_95_WATT_4_2,
  EnumCflS35Watt62PowerDesign  = CFL_S_35_WATT_6_2,
  EnumCflS65Watt62PowerDesign  = CFL_S_65_WATT_6_2,
  EnumCflS95Watt62PowerDesign  = CFL_S_95_WATT_6_2,
  EnumCflS35Watt82PowerDesign  = CFL_S_35_WATT_8_2,
  EnumCflS65Watt82PowerDesign  = CFL_S_65_WATT_8_2,
  EnumCflS95Watt82PowerDesign  = CFL_S_95_WATT_8_2,
  EnumMaxCflVrPowerDesign      = CFL_S_95_WATT_8_2  /// same as EnumCflS80Watt82CpuId
} CPU_VR_POWER_DELIVERY_DESIGN;

/**
  CPU Power Management VR Configuration Structure.

  <b>Revision 1</b>:
  - Initial version.
  <b>Revision 2</b>:
  - Deprecate SendVrMbxCmd
  - Add SendVrMbxCmd1 for VR specific mailbox commands:
      001b: MPS IMPV8 VR mailbox command.
      010b: VR specific command sent for PS4 exit issue.
      100b: VR specific command sent for MPS VR decay issue.
  <b>Revision 3</b>:
  - Add IslVrCmd for specific Intersil VR issues.
  <b>Revision 4</b>:
  - Update Thermal Design Current limit default description.
  <b>Revision 5</b>:
  - Add VrPowerDeliveryDesign.
**/
typedef struct {
  CONFIG_BLOCK_HEADER   Header;                   ///< Config Block Header
  UINT32 AcousticNoiseMitigation        : 1;      ///< Enable or Disable Acoustic Noise Mitigation feature. <b>0: Disabled</b>; 1: Enabled
  UINT32 FastPkgCRampDisableIa          : 1;      ///< Disable Fast Slew Rate for Deep Package C States for VR IA domain based on Acoustic Noise Mitigation feature enabled. <b>0: False</b>; 1: True
  UINT32 FastPkgCRampDisableGt          : 1;      ///< Disable Fast Slew Rate for Deep Package C States for VR GT domain based on Acoustic Noise Mitigation feature enabled. <b>0: False</b>; 1: True
  UINT32 FastPkgCRampDisableSa          : 1;      ///< Disable Fast Slew Rate for Deep Package C States for VR SA domain based on Acoustic Noise Mitigation feature enabled. <b>0: False</b>; 1: True
  UINT32 SlowSlewRateForIa              : 2;      ///< Slew Rate configuration for Deep Package C States for VR IA domain based on Acoustic Noise Mitigation feature enabled. <b>0: Fast/2</b>; 1: Fast/4; 2: Fast/8; 3: Fast/16
  UINT32 SlowSlewRateForGt              : 2;      ///< Slew Rate configuration for Deep Package C States for VR GT domain based on Acoustic Noise Mitigation feature enabled. <b>0: Fast/2</b>; 1: Fast/4; 2: Fast/8; 3: Fast/16
  UINT32 SlowSlewRateForSa              : 2;      ///< Slew Rate configuration for Deep Package C States for VR SA domain based on Acoustic Noise Mitigation feature enabled. <b>0: Fast/2</b>; 1: Fast/4; 2: Fast/8; 3: Fast/16
  /**
   Deprecated:
   VR specific mailbox commands.
   <b>00b - no VR specific command sent.</b>
   01b - A VR mailbox command specifically for the MPS IMPV8 VR will be sent.
   10b - VR specific command sent for PS4 exit issue.
   11b - Reserved.
   **/
  UINT32 SendVrMbxCmd                   : 2;
  /**
   VR specific mailbox commands.
   <b>000b - no VR specific command sent.</b>
   001b - A VR mailbox command specifically for the MPS IMPV8 VR will be sent.
   010b - VR specific command sent for PS4 exit issue.
   100b - VR specific command sent for MPS VR decay issue.
   Bitwise encoding. Allows for any combination of commands to be sent.
   example 110b - to send both PS4 exit issue and MPS VR decay issue commands.
   **/
  UINT32 SendVrMbxCmd1                  : 3;
  /**
   ISL9585X/ISL9586X IA/GT/SA VR Command
   <b>0 - No VR command sent.</b>
   1 - A VR mailbox command is sent for IA and GT rails only.
   2 - A VR mailbox command is sent for IA, GT, and SA rails.
   This command is intended for systems with specific issues related to some Intersil VRs.
   **/
  UINT32 IslVrCmd                       : 2;
  UINT32 RsvdBits                       : 15;     ///< Reserved for future use.
  UINT8  PsysSlope;                               ///< PCODE MMIO Mailbox: Platform Psys slope correction. <b>0 - Auto</b> Specified in 1/100 increment values. Range is 0-200. 125 = 1.25.
  UINT8  PsysOffset;                              ///< PCODE MMIO Mailbox: Platform Psys offset correction. <b>0 - Auto</b> Units 1/4, Range 0-255. Value of 100 = 100/4 = 25 offset.
  UINT8  Rsvd[2];                                 ///< Reserved for future use and config block alignment
  /** @name VR Settings
  The VR related settings are sorted in an array where each index maps to the VR domain as defined below:
   - 0 = System Agent VR
   - 1 = IA Core VR
   - 2 = GT unsliced VR
   - 3 = GT sliced VR

  The VR settings for a given domain must be populated in the appropriate index.
  **/
  ///@{
  UINT16 TdcPowerLimit[MAX_NUM_VRS];              ///< PCODE MMIO Mailbox: Thermal Design Current current limit. Specified in 1/8A units. Range is 0-4095. 1000 = 125A. <b>0: Auto/Intel Defaults</b>
  UINT16 AcLoadline[MAX_NUM_VRS];                 ///< PCODE MMIO Mailbox: AcLoadline in 1/100 mOhms (ie. 1250 = 12.50 mOhm); Range is 0-6249. <b>Intel Recommended Defaults vary by domain and SKU.</b>
  UINT16 DcLoadline[MAX_NUM_VRS];                 ///< PCODE MMIO Mailbox: DcLoadline in 1/100 mOhms (ie. 1250 = 12.50 mOhm); Range is 0-6249.<b>Intel Recommended Defaults vary by domain and SKU.</b>
  UINT16 Psi1Threshold[MAX_NUM_VRS];              ///< PCODE MMIO Mailbox: Power State 1 current cuttof in 1/4 Amp increments. Range is 0-128A.  <b>Default Value = 20A.</b>
  UINT16 Psi2Threshold[MAX_NUM_VRS];              ///< PCODE MMIO Mailbox: Power State 2 current cuttof in 1/4 Amp increments. Range is 0-128A.  <b>Default Value = 5A.</b>
  UINT16 Psi3Threshold[MAX_NUM_VRS];              ///< PCODE MMIO Mailbox: Power State 3 current cuttof in 1/4 Amp increments. Range is 0-128A.  <b>Default Value = 1A.</b>
  UINT8  Psi3Enable[MAX_NUM_VRS];                 ///< PCODE MMIO Mailbox: Power State 3 enable/disable; 0: Disable; <b>1: Enable</b>.
  UINT8  Psi4Enable[MAX_NUM_VRS];                 ///< PCODE MMIO Mailbox: Power State 4 enable/disable; 0: Disable; <b>1: Enable</b>.
  UINT8  ImonSlope[MAX_NUM_VRS];                  ///< PCODE MMIO Mailbox: Imon slope correction. Specified in 1/100 increment values. Range is 0-200. 125 = 1.25. <b>0: Auto</b>
  INT16  ImonOffset[MAX_NUM_VRS];                 ///< PCODE MMIO Mailbox: Imon offset correction. Value is a 2's complement signed integer. Units 1/1000, Range 0-63999. For an offset = 12.580, use 12580. <b>0: Auto</b>
  UINT16 IccMax[MAX_NUM_VRS];                     ///< PCODE MMIO Mailbox: VR Icc Max limit. 0-255A in 1/4 A units. 400 = 100A. <b>Default: 0 - Auto, no override</b>
  UINT16 VrVoltageLimit[MAX_NUM_VRS];             ///< PCODE MMIO Mailbox: VR Voltage Limit. Range is 0-7999mV.
  UINT8  VrConfigEnable[MAX_NUM_VRS];             ///< Enable/Disable BIOS configuration of VR; 0: Disable; <b>1: Enable.</b>
  UINT8  TdcEnable[MAX_NUM_VRS];                  ///< PCODE MMIO Mailbox: Thermal Design Current enable/disable; <b>0: Disable; </b>1: Enable
  UINT8  TdcTimeWindow[MAX_NUM_VRS];              ///< PCODE MMIO Mailbox: Thermal Design Current time window. Defined in milli seconds. <b>1ms default</b>
  UINT8  TdcLock[MAX_NUM_VRS];                    ///< PCODE MMIO Mailbox: Thermal Design Current Lock; <b>0: Disable</b>; 1: Enable.
  ///@}
  /**
    VrPowerDeliveryDesign

    This field is required to be non-zero on CFL S-based platforms.

    Used to communicate the power delivery design capability of the board. This value is an
    enum of the available power delivery segments that are defined in the Platform Design Guide.
    This is required due to CPU socket compatibility across different board designs. Without
    this field populated, the system may experience VR OCP/OVP shutdowns, hangs, thermal and
    performance loss.
  **/
  CPU_VR_POWER_DELIVERY_DESIGN   VrPowerDeliveryDesign;
} CPU_POWER_MGMT_VR_CONFIG;

#pragma pack (pop)

#endif // _CPU_POWER_MGMT_VR_CONFIG_H_
