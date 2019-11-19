/** @file
  Header file for NV data structure definition.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2012 - 2015 Intel Corporation.

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

#ifndef __TREE_CONFIG_NV_DATA_H__
#define __TREE_CONFIG_NV_DATA_H__

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/TrEEPhysicalPresenceData.h>
#include <Guid/TrEEConfigHii.h>
#include <Guid/PttInstance.h>
#include <Guid/PttPTPInstanceGuid.h>


#define TREE_CONFIGURATION_VARSTORE_ID  0x0002
#define TREE_CONFIGURATION_FORM_ID      0x0001

#define KEY_TPM_DEVICE                  0x2000
#define KEY_TPM2_REVOKE_TRUST           0x2001
#define KEY_TREE_ACTION                 0x2002
#define KEY_TPM2_CLEAR_TPM              0x2003
#define KEY_TPM2_RESET_LOCKOUT_AUTH     0x2004
#define KEY_TPM2_DISABLE_PLATFORM_HIERARCHY     0x2005
#define KEY_TPM2_DISABLE_STORAGE_HIERARCHY      0x2006
#define KEY_TPM2_DISABLE_ENDORSEMENT_HIERARCHY  0x2007
#define KEY_TPM2_REVOKE_PLATFORM                0x2008
#define KEY_TPM2_REVOKE_ENDORSEMENT             0x2009
#define KEY_TPM2_REVOKE_STORAGE                 0x200A
#define KEY_TPM2_OPERATION                      0x200B

#define TPM_DEVICE_NULL           0
#define TPM_DEVICE_DTPM_1_2       1
#define TPM_DEVICE_DTPM_2_0       2
#define TPM_DEVICE_PTT            3
#define TPM_DEVICE_MIN            TPM_DEVICE_DTPM_1_2
#define TPM_DEVICE_MAX            TPM_DEVICE_PTT
#define TPM_DEVICE_DEFAULT        TPM_DEVICE_DTPM_2_0

#define TREE_EVENT_LOG_FORMAT_TCG_1_2        0x00000001

//
// Nv Data structure referenced by IFR, TPM device user desired
//
typedef struct {
  UINT8   TpmDevice;
  UINT8   Tpm2Operation;
  BOOLEAN RevokeTrust;
  BOOLEAN ClearTPM;
  BOOLEAN SupportedAlgSHA1;
  BOOLEAN SupportedAlgSHA256;
  BOOLEAN ResetLockoutAuth;
  BOOLEAN DisablePlatformHierarchy;
  BOOLEAN DisableStorageHierarchy;
  BOOLEAN DisableEndorsementHierarchy;
  BOOLEAN RevokePlatform;
  BOOLEAN RevokeEndorsement;
  BOOLEAN RevokeStorage;
  UINT8 Rsvd;
} TREE_CONFIGURATION;

#define TREE_STORAGE_NAME  L"TREE_CONFIGURATION"
#define TREE_DEVICE_DETECTION_NAME  L"TREE_DEVICE_DETECTION"

#define TPM_INSTANCE_ID_LIST  { \
  {TPM_DEVICE_INTERFACE_NONE,           TPM_DEVICE_NULL},      \
  {TPM_DEVICE_INTERFACE_TPM12,          TPM_DEVICE_DTPM_1_2},  \
  {TPM_DEVICE_INTERFACE_TPM20_DTPM,     TPM_DEVICE_DTPM_2_0},  \
  {TPM_DEVICE_INTERFACE_TPM20_PTT_PTP,  TPM_DEVICE_PTT},       \
}

//
// BUGBUG: In order to pass VfrCompiler, we have to redefine GUID here.
//
#ifndef __BASE_H__
typedef struct {
  UINT32  Data1;
  UINT16  Data2;
  UINT16  Data3;
  UINT8   Data4[8];
} GUID;
#endif

typedef struct {
  GUID       TpmInstanceGuid;
  UINT8      TpmDevice;
} TPM_INSTANCE_ID;

#endif
