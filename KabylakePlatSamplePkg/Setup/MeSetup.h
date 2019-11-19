/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2015 Intel Corporation.

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
#ifndef __ME_SETUP_H__
#define __ME_SETUP_H__

#include <MeBiosExtensionSetup.h>

#define ME_MODE_MASK                  0x0F
#define NORMAL_MODE                   0x00
#define TEMP_DISABLE_MODE             0x03
#define ME_IMAGE_CONSUMER_SKU_FW      0x03
#define ME_IMAGE_CORPORATE_SKU_FW     0x04

#define ME_FW_IMAGE_TYPE_CONSUMER_STRING       "Consumer SKU"
#define ME_FW_IMAGE_TYPE_CORPORATE_STRING      "Corporate SKU"
#define ME_FW_IMAGE_TYPE_UNIDENTIFIED_STRING   "Unidentified"

#define STR_NFC_DISABLED    "Disabled"
#define STR_NFC_MODULE_1    "NXP"

#pragma pack(1)

typedef struct {
  UINT8  MngState;
  UINT8  LocalFwUpdEnabled;
  UINT8  MeStateControl;
  UINT8  AfterEoP;
  UINT8  RemoteSessionActive;
  UINT8  PttState;
  UINT8  TpmDeactivate;
  UINT8  BootGuardSupport;
  UINT8  MeasureBoot;
  UINT8  NfcState;
} ME_SETUP_STORAGE;

typedef struct {
  UINT8   MeFirmwareInfo;
  UINT32  MeMajor;
  UINT32  MeMinor;
  UINT32  MeHotFix;
  UINT32  MeBuildNo;
// AMI_OVERRIDE_START - Fixed ME INFO Error in setup.  
  UINT8   MeFwType;  
// AMI_OVERRIDE_END -
  UINT32  MeFileSystem;
} ME_INFO_SETUP_DATA;

// AMI_OVERRIDE_START
//This structure is used for setup.
typedef struct {
  UINT8 GrayOut;
} SETUP_AMT_FEATURES;
// AMI_OVERRIDE_END

#pragma pack()

extern EFI_GUID           gMeInfoSetupGuid;

#endif
