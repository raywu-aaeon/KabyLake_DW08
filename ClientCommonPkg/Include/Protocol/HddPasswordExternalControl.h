/**@file
  Protocol used to take control over HDD unlocking mechanism. Allows
  to either delay or skip build in HDD password popup (and whole
  unlock procedure).  Intended to be used only in special and
  justified cases (not during normal boot). To work properly, the
  protocol instance has to be installed before end of DXE.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be 
  modified, except as allowed by additional terms of your license agreement.

@par Specification Reference:
**/

#ifndef _HDD_PASSWORD_EXTERNAL_CONTROL_H_
#define _HDD_PASSWORD_EXTERNAL_CONTROL_H_

typedef struct _HDD_PASSWORD_EXTERNAL_CONTROL_PROTOCOL HDD_PASSWORD_EXTERNAL_CONTROL_PROTOCOL;

/**
  Function used to register save HDD unlock event handler required to trigger standard HDD unlock
  mechanism.

  @param[in]       This
  @param[in]       Event               The Hdd Unlock event handler

  @retval          EFI_SUCCESS
**/
typedef
EFI_STATUS
(EFIAPI *REGISTER_HDD_UNLOCK_EVENT_HANDLER) (
  IN HDD_PASSWORD_EXTERNAL_CONTROL_PROTOCOL      *This,
  IN EFI_EVENT                                   *Event
  );

/**
  Function used to trigger previously registered HDD unlock event handler

  @param[in]       This

  @retval          EFI_SUCCESS
  @retval          EFI_NOT_READY       There is no HDD unlock event handler registered
**/
typedef
EFI_STATUS
(EFIAPI *RETURN_CONTROL) (
  IN HDD_PASSWORD_EXTERNAL_CONTROL_PROTOCOL  *This
  );

///
/// Protocol used to bypass standard HDD Password handling implemented in the HddPasswordDxe module.
/// If the protocol is present in the system before HddPasswordDxe initialization, the module
/// will pass to it any required information to allow to delay or omit standard HDD password prompt.
///
struct _HDD_PASSWORD_EXTERNAL_CONTROL_PROTOCOL {
  REGISTER_HDD_UNLOCK_EVENT_HANDLER    RegisterUnlockEvent;
  RETURN_CONTROL                       ReturnControl;
};


extern EFI_GUID gHddPasswordExternalControlProtocolGuid;

#endif
