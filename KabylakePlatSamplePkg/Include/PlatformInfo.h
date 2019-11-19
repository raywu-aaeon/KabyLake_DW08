/** @file
  GUID used for Platform Info Data entries in the HOB list.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2018 Intel Corporation.

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

#ifndef _PLATFORM_INFO_H_
#define _PLATFORM_INFO_H_

#ifndef PLATFORM_INFO_VARIABLE_NAME
#define PLATFORM_INFO_VARIABLE_NAME             L"PlatformInfo"
#endif

extern EFI_GUID gPlatformInfoHobGuid;
extern EFI_GUID gPlatformInfoProtocolGuid;

#include <PlatformBoardType.h>

#pragma pack(1)

//
// This definition contains platform Flavor,ID,Revision etc. information.
//
typedef struct _PLATFORM_INFO {
  UINT8                      RevisonId;           // Structure Revision ID
  UINT8                      PlatformFlavor;      // Platform Flavor
  UINT8                      PlatformType;        // Platform Type TRAD or ULT
  UINT8                      PlatformGeneration;  // Generation (Tock/Tick)
// AMI_OVERRIDE_START >>> Since RC160 add BoardID to this structure and would cause fault tolerance fail. So keep this override.
  UINT16                     BoardID;             // Board ID
// AMI_OVERRIDE_END <<< Since RC160 add BoardID to this structure and would cause fault tolerance fail. So keep this override.
  UINT8                      BoardRev;            // Board Revision
  UINT8                      BoardBomId;          // Board BOM information
  UINT8                      BoardType;           // Board Type for example with EcolaFalls south board ther are 4 flavors of north board.
  BOOLEAN                    EcPresent;
  BOOLEAN                    SpdPresent;          // 0 - BIOS hard-codes SPD; 1 - User/factory programs SPD in the onboard EEPROM.
  UINT8                      DockAttached;        // 1-docked, 0-undocked or no dock support
  UINT8                      WakeUpType;          // Wake Event Type for SMBIOS Table
  BOOLEAN                    MfgMode;
  CHAR8                      BoardName[32];
  BOOLEAN                    UpdateData;          // 0 = No need to update data; 1 = Need to update data
  UINT8                      EcMajorRevision;
  UINT8                      EcMinorRevision;
  UINT8                      EcProtocolVersion;
  UINT8                      BaseEcFwMajorVersion;
  UINT8                      BaseEcFwMinorVersion;
  UINT8                      BaseEcProtocolVersion;
  BOOLEAN                    HsioBoardPresent;     // Indicates whether HSIO card is detected
  UINT8                      HsioBoardType;        // HSIO card type used with certain platforms. Valid only when HsioBoardPresent is TRUE
  BOOLEAN                    GfxCrbDetect;         // GFX_CRB_DETECT
  CHAR16                     BiosVersion[40];
  CHAR16                     ReleaseDate[20];
  CHAR16                     ReleaseTime[20];
  BOOLEAN                    TypeCSupport;         // USB Type C Support
  UINT8                      RunTimeVmControl;     // 0:Disabled, 1:Premium PMIC, 2:VR controlled by GPIO, 3:DVRM Without IO EXP, 4:DVRM With IO EXP. If not 0 PrimCore Voltage on SLP_S0 assertion drops from 1v to 0.7v.
// AMI_OVERRIDE_START >>> Since RC160 add BoardID to this structure and would cause fault tolerance fail. So mark this code.
//UINT16                     BoardID;
// AMI_OVERRIDE_END <<< Since RC160 add BoardID to this structure and would cause fault tolerance fail. So mark this code.
  UINT16                     BoardIDOrgvalue;
  UINT8                      BiosGuardEcStatus;     // BiosGuard status read from EC
} PLATFORM_INFO;

#ifndef MINTREE_FLAG
///
/// PCH_GPIO_PAD is equivalent to GPIO_PAD which is defined in GpioConfig.h
///
typedef UINT32 PCH_GPIO_PAD; //Copied from GpioConfig.h (need to change it based on include)

typedef struct {
UINT8    Expander;
UINT8    Pin;
UINT16   Reserved; // Reserved for future use
} IO_EXPANDER_PAD;

typedef union {
PCH_GPIO_PAD       PchGpio;
IO_EXPANDER_PAD    IoExpGpio;
} GPIO_PAD_CONFIG;

typedef struct {
UINT8                GpioType;    // 0: Disabled (no GPIO support), 1: PCH, 2: I/O Expander
UINT8                Reserved[3]; // Reserved for future use
GPIO_PAD_CONFIG      GpioData;
} PACKED_GPIO_CONFIG;

typedef union {
PACKED_GPIO_CONFIG    PackedGpio;
UINT64                Data64;
} COMMON_GPIO_CONFIG;
#endif

#pragma pack()

#endif
