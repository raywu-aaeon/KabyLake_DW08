/** @file

  The header file includes the definition of EFI_SPI_PROTOCOL.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2015 Intel Corporation.

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

#ifndef _EFI_AMT_WRAPPER_PROTOCOL_H_
#define _EFI_AMT_WRAPPER_PROTOCOL_H_


#define EFI_AMT_WRAPPER_PROTOCOL_GUID \
  {0xd54f49f6, 0x0dd4, 0x4276, {0xb0, 0x9c, 0xdf, 0xe1, 0xfd, 0x80, 0x85, 0xf0}}

//
// Forward reference for ANSI C compatibility
//
typedef struct _AMT_WRAPPER_PROTOCOL  AMT_WRAPPER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMT_WRAPPER_INIT) (
  EFI_HANDLE       ImageHandle,
  EFI_SYSTEM_TABLE *SystemTable
);

typedef
BOOLEAN
(EFIAPI *AMT_WRAPPER_GET) (
  UINTN Index,
  VOID  *Variable
);

typedef
BOOLEAN
(EFIAPI *AMT_WRAPPER_SET) (
  UINTN Index,
  VOID  *Variable
);

typedef
BOOLEAN
(EFIAPI *ACTIVE_MANAGEMENT_ENABLE_KVM) (
  VOID
);

typedef
BOOLEAN
(EFIAPI *ACTIVE_MANAGEMENT_ENABLE_SOL) (
  VOID
);

typedef
BOOLEAN
(EFIAPI *ACTIVE_MANAGEMENT_ENABLE_STORAGE_REDIR) (
  VOID
);

typedef
BOOLEAN
(EFIAPI *ACTIVE_MANAGEMENT_BOOT_OPTION_EXIST) (
    IN VOID
);

typedef
EFI_STATUS
(EFIAPI *BDS_BOOT_VIA_ASF) (
  VOID
);

typedef
BOOLEAN
(EFIAPI *ACTIVE_MANAGEMENT_PAUSE_BOOT) (
  VOID
);

typedef
BOOLEAN
(EFIAPI *ACTIVE_MANAGEMENT_ENTER_SETUP) (
  VOID
);

typedef
BOOLEAN
(EFIAPI *ACTIVE_MANAGEMENT_CONSOLE_LOCKED) (
  VOID
);

typedef
BOOLEAN
(EFIAPI *ACTIVE_MANAGEMENT_ENABLE_SECURE_ERASE) (
  VOID
);


typedef
EFI_STATUS
(EFIAPI *GET_SETUP_PROMPT) (
  IN OUT  UINTN     *PromptIndex,
  OUT     CHAR16    **String
);

typedef
EFI_STATUS
(EFIAPI *DETERMINE_SETUP_HOTKEY) (
  IN  EFI_KEY_DATA              *KeyData
);

typedef
EFI_STATUS
(EFIAPI *GET_SUPPORTED_HOTKEYS) (
  OUT  UINTN             *KeyCnt,
  OUT  EFI_KEY_DATA      **KeyData
  );

typedef struct _AMT_WRAPPER_PROTOCOL {
  AMT_WRAPPER_INIT                        AmtWrapperInit;
  AMT_WRAPPER_GET                         AmtWrapperGet;
  AMT_WRAPPER_SET                         AmtWrapperSet;
  ACTIVE_MANAGEMENT_BOOT_OPTION_EXIST     ActiveManagementBootOptionExist;
  ACTIVE_MANAGEMENT_ENABLE_STORAGE_REDIR  ActiveManagementIsStorageRedirectionEnabled;
  ACTIVE_MANAGEMENT_ENTER_SETUP           ActiveManagementIsEnterSetupEnabled;
  ACTIVE_MANAGEMENT_PAUSE_BOOT            ActiveManagementIsPauseBootEnabled;
  ACTIVE_MANAGEMENT_ENABLE_KVM            ActiveManagementIsKvmEnabled;
  ACTIVE_MANAGEMENT_ENABLE_SECURE_ERASE   ActiveManagementIsSecureEraseEnabled;
  ACTIVE_MANAGEMENT_CONSOLE_LOCKED        ActiveManagementIsConsoleLocked;
  ACTIVE_MANAGEMENT_ENABLE_SOL            ActiveManagementIsSolEnabled;
  BDS_BOOT_VIA_ASF                        BdsBootViaAsf;
  GET_SETUP_PROMPT                        GetSetupPrompt;
  DETERMINE_SETUP_HOTKEY                  DetermineSetupHotKey;
  GET_SUPPORTED_HOTKEYS                   GetSupportedHotkeys;
} AMT_WRAPPER_PROTOCOL;

#define GET_SOL_DP      1
#define GET_BOOT_OPTION 2
#define GET_IS_SOL      3

#define SET_BIOS_WDT_START 1
#define SET_WDT_STOP       2
#define SET_OS_SOL         3
#define SET_OS_WDT_START   4

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                   gEfiAmtWrapperProtocolGuid;

#endif
