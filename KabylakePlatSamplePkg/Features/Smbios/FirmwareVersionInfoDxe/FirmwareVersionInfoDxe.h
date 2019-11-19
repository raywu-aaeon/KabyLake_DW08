/** @file
  Private data structures

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2017 Intel Corporation.

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

#ifndef _PLATFORM_FVI_DXE_H
#define _PLATFORM_FVI_DXE_H

#include <Library/SiFviLib.h>
#include <PlatformInfo.h>

#define PLATFORM_FVI_STATUS_DISABLED "Disabled"
#define PLATFORM_FVI_STATUS_ENABLED  "Enabled "

#define PLATFORM_FVI_SETUP_NAME      L"PlatformFviSetupDataVar"

//
// Ensure proper structure formats
//
#pragma pack(1)

typedef struct _PLATFORM_FVI_SETUP{
  UINT8 LanPhyVersion;
  UINT16 IccMajorVersion;
  UINT16 IccMinorVersion;
  UINT16 IccHotfixVersion;
  UINT16 IccBuildVersion;
} PLATFORM_FVI_SETUP;

#pragma pack()


#define SENSOR_FW_FVI_STRING  "Sensor Firmware Version"
#define SENSOR_FW_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define LAN_PHY_FVI_STRING  "Lan Phy Version"
#define LAN_PHY_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define DEBUG_MODE_STATUS  "Debug Mode Status"
#define DEBUG_MODE_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define PERFORMANCE_MODE_STATUS  "Performance Mode Status"
#define PERFORMANCE_MODE_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define DEBUG_USE_USB_STATUS  "Debug Use USB(Disabled:Serial)"
#define DEBUG_USE_USB_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define ICC_OVERLOCKING_FVI_STRING  "ICC Overclocking Version"
#define ICC_OVERLOCKING_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define UNDI_FVI_STRING  "UNDI Version"
#define UNDI_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define EC_FVI_STRING  "EC FW Version"
#define EC_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define BASE_EC_FW_FVI_STRING  "Base EC FW Version"
#define BASE_EC_FW_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define EC_2_EC_PROTOCOL_FVI_STRING  "EC-EC Protocol Version"
#define EC_2_EC_PROTOCOL_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define GOP_FVI_STRING  "GOP Version"
#define GOP_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define ROYALPARK_FVI_STRING  "Royal Park Version"
#define ROYALPARK_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

#define PLATFORM_FVI_STRING  "Platform Version"
#define PLATFORM_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }
//AMI_OVERRIDE_START >>> EIP426038 - Fix build failed with new GCC flags
typedef enum {
  LAN_PHY_VER = 0,
  SENSOR_FW_VER,
  DEBUG_MODE_VER,
  PERFORMANCE_MODE_VER,
  DEBUB_USE_USB_VER,
  ICC_OVERLOCKING_VER,
  UNDI_VER,
  EC_VER,
  GOP_VER,
  BASE_EC_FW_VER,
  EC_2_EC_PROTOCOL_VER,
  ROYALPARK_VER,
  PLATFORM_VER,
} PLATFORM_FVI_INDEX;
//AMI_OVERRIDE_END <<< EIP426038 - Fix build failed with new GCC flags
extern FVI_HEADER  mPlatformFviHeader;
extern FVI_DATA    mPlatformFviData[];
extern FVI_STRINGS mPlatformFviStrings[];

#define SENSOR_HUB_FW_VERSION_OFFSET 0x1000
#define SENSOR_HUB_FW_VERSION_LENGTH 0x04

//
// LAN PHY Revision definitions
//
#define PHY_CONFIG_REG    0x00000020
#define PHY_SEMAPHORE_REG 0x00000F00
#define PHY_PAGE769_SET_REG 0x43f6020
#define PHY_SLOW_MDIO_MODE_REG 0x4302580
#define PHY_READ_PHY_OFFSET3_REG 0x8430000
#define PHY_FAST_MDIO_MODE_REG 0x4302180
#define LAN_PHY_REV_TC 0xA0
#define LAN_PHY_REV_A0 0xA1
#define LAN_PHY_REV_A1 0xA2
#define LAN_PHY_REV_A2 0xA3
#define LAN_PHY_REV_A3 0xA4
#define LAN_PHY_REV_B1 0xA5

// Max loop value for GBE check
#define GBE_MAX_LOOP_TIME 4000

#endif
