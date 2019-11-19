/** @file
  Variable definition for overclocking support

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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
#ifndef _OC_SETUP_VARIABLE_H_
#define _OC_SETUP_VARIABLE_H_

#pragma pack(1)
typedef struct{
  UINT8  OcInterfaceRtUpdated;    // New added, used to indicate OC variable has been updated during RT.
  //
  // Sa setup
  //
  UINT8  tCL;
  UINT8  tRCDtRP;
  UINT16 tRAS;
  UINT8  tWR;
  UINT16 tRFC;
  UINT8  tRRD;
  UINT8  tWTR;
  UINT8  tRTP;
  UINT16 tFAW;
  UINT8  tWRDefault;
  UINT8  NModeSupport;
  UINT8  DdrRatioDefault;
  UINT8  DdrRatio;
// AMI_OVERRIDE_START - Adjust Memory multiplier by XTU.
  UINT8  DdrOddRatioMode;
// AMI_OVERRIDE_END - Adjust Memory multiplier by XTU.
  UINT16 MemoryVoltage;
  UINT8  BclkOverride;
  UINT32 BclkFrequency;
  UINT8  GtsMaxOcRatio;
  UINT8  SpdProfileSelected;
  UINT8  DdrRefClk;
  UINT8  GtsVoltageMode;
  UINT16 GtsVoltageOffset;
  UINT8  GtsVoltageOffsetPrefix;
  UINT16 GtsVoltageOverride;
  UINT16 GtsExtraTurboVoltage;
  UINT16 UncoreVoltageOffset;
  UINT8  UncoreVoltageOffsetPrefix;

  //
  // CPU Setup
  //
  UINT8  OverclockingSupport;
  UINT8  EnableGv;
  UINT8  CpuRatio;
  UINT8  CpuRatioOverride;
  UINT8  CoreMaxOcRatio;
  UINT8  CoreVoltageMode;
  UINT16 CoreVoltageOverride;
  UINT16 CoreExtraTurboVoltage;
  UINT16 CoreVoltageOffset;
  UINT8  CoreVoltageOffsetPrefix;
  UINT8  PowerLimit1Time;
  UINT8  TurboMode;
  UINT8  RatioLimit1;
  UINT8  RatioLimit2;
  UINT8  RatioLimit3;
  UINT8  RatioLimit4;
  UINT8  TurboPowerLimitLock;
  UINT32 PowerLimit1;
  UINT32 PowerLimit1Default;
  UINT8  PowerLimit2;
  UINT32 PowerLimit2Power;
  UINT32 PowerLimit2Default;
  UINT8  RingMaxOcRatio;
  UINT8  OverclockingLock;
  UINT8  LongDurationPwrLimitOverride;
  UINT8  WDTSupportforNextOSBoot;
  UINT16 TimeforNextOSBoot;
// AMI_OVERRIDE_START - Add setup items "ICCMAX" and "VrConfig" to OC policy.
  UINT8   VrConfigEnable[4];
// AMI_OVERRIDE_END - Add setup items "ICCMAX" and "VrConfig" to OC policy.

  //
  // Setup
  //
  UINT8  PlatformUnstable;

  //
  // IccMax and AVX Retio Control
  //
  UINT16  IccMax[4];
  UINT8   Avx2RatioOffset;

  //
  // GT Unsliced
  //
  UINT8   GtusMaxOcRatio;
  UINT8   GtusVoltageMode;
  UINT16  GtusVoltageOffset;
  UINT8   GtusVoltageOffsetPrefix;
  UINT16  GtusVoltageOverride;
  UINT16  GtusExtraTurboVoltage;

  //
  // OC Ratio Limit 5-8
  //
  UINT8  RatioLimit5;
  UINT8  RatioLimit6;
  UINT8  RatioLimit7;
  UINT8  RatioLimit8;

  //
  // RealTime Memory Timing Enable/Disable
  //
  UINT8 RealtimeMemoryTiming;

// AMI_OVERRIDE_START >>>
  UINT16  DdrFreqLimit;
// AMI_OVERRIDE_END <<<
} OC_SETUP_DATA;

extern EFI_GUID gOcSetupVariableGuid;

#pragma pack()
#endif
