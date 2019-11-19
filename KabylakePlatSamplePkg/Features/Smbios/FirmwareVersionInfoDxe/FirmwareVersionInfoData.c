/** @file
  Platform Firmware Version Info implementation.

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

#include <FirmwareVersionInfoDxe.h>

GLOBAL_REMOVE_IF_UNREFERENCED FVI_DATA mPlatformFviData[] = {
  FVI_VERSION (LAN_PHY_VERSION),
  FVI_VERSION (SENSOR_FW_VERSION),
  FVI_VERSION (DEBUG_MODE_VERSION),
  FVI_VERSION (PERFORMANCE_MODE_VERSION),
  FVI_VERSION (DEBUG_USE_USB_VERSION),
  FVI_VERSION (ICC_OVERLOCKING_VERSION),
  FVI_VERSION (UNDI_VERSION),
  FVI_VERSION (EC_VERSION),
  FVI_VERSION (GOP_VERSION),
  FVI_VERSION (BASE_EC_FW_VERSION),
  FVI_VERSION (EC_2_EC_PROTOCOL_VERSION),
  FVI_VERSION (ROYALPARK_VERSION),
  FVI_VERSION (PLATFORM_VERSION),
};

GLOBAL_REMOVE_IF_UNREFERENCED FVI_HEADER mPlatformFviHeader = DEFAULT_FVI_HEADER_DATA(mPlatformFviData);

GLOBAL_REMOVE_IF_UNREFERENCED FVI_STRINGS mPlatformFviStrings[] = {
  { LAN_PHY_FVI_STRING, NULL, },
  { SENSOR_FW_FVI_STRING, NULL, },
  {
    DEBUG_MODE_STATUS,
    PLATFORM_FVI_STATUS_DISABLED,
  },
  {
    PERFORMANCE_MODE_STATUS,
    PLATFORM_FVI_STATUS_DISABLED,
  },
  {
    DEBUG_USE_USB_STATUS,
    PLATFORM_FVI_STATUS_DISABLED,
  },
  { ICC_OVERLOCKING_FVI_STRING, NULL, },
  { UNDI_FVI_STRING, NULL, },
  { EC_FVI_STRING, NULL, },
  { GOP_FVI_STRING, NULL, },
  { BASE_EC_FW_FVI_STRING, NULL, },
  { EC_2_EC_PROTOCOL_FVI_STRING, NULL, },
  { ROYALPARK_FVI_STRING, NULL, },
  { PLATFORM_FVI_STRING, NULL, },
};
