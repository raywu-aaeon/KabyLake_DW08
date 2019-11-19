/** @file
  This protocol is used to report and control what BIOS is mapped to the
  BIOS address space anchored at 4GB boundary.

  E.G. For current generation, the 4GB-16MB to 4GB range can be mapped
  to LPC/eSPI or SPI.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2005 - 2016 Intel Corporation.

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
#ifndef _PCH_ACTIVE_BIOS_PROTOCOL_H_
#define _PCH_ACTIVE_BIOS_PROTOCOL_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                           gPchActiveBiosProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_ACTIVE_BIOS_PROTOCOL  PCH_ACTIVE_BIOS_PROTOCOL;

///
/// Protocol definitions
///
typedef enum {
  ActiveBiosStateSpi,
  ActiveBiosStateLpc,
  ActiveBiosStateMax
} PCH_ACTIVE_BIOS_STATE;

/**
  Change the current active BIOS settings to the requested state.
  The caller is responsible for requesting a supported state from
  the PCH_ACTIVE_BIOS_STATE selections.
  This will fail if someone has locked the interface and the correct key is
  not provided.

  @param[in] This                 Pointer to the PCH_ACTIVE_BIOS_PROTOCOL instance.
  @param[in] DesiredState         The requested state to configure the system for.
  @param[in] Key                  If the interface is locked, Key must be the Key
                                  returned from the LockState function call.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_ACCESS_DENIED       The interface is currently locked.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_ACTIVE_BIOS_SET_ACTIVE_BIOS_STATE) (
  IN PCH_ACTIVE_BIOS_PROTOCOL     * This,
  IN PCH_ACTIVE_BIOS_STATE        DesiredState,
  IN UINTN                        Key
  );

/**
  Lock the current active BIOS state from further changes. This allows a
  caller to implement a critical section. This is optionally supported
  functionality. Size conscious implementations may choose to require
  callers cooperate without support from this protocol.

  @param[in] This                 Pointer to the PCH_ACTIVE_BIOS_PROTOCOL instance.
  @param[in] Lock                 TRUE to lock the current state, FALSE to unlock.
  @param[in, out] Key             If Lock is TRUE, then a key will be returned.  If
                                  Lock is FALSE, the key returned from the prior call
                                  to lock the protocol must be provided to unlock the
                                  protocol.  The value of Key is undefined except that
                                  it cannot be 0.

  @retval EFI_SUCCESS             Command succeed.
  @exception EFI_UNSUPPORTED      The function is not supported.
  @retval EFI_ACCESS_DENIED       The interface is currently locked.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_ACTIVE_BIOS_LOCK_ACTIVE_BIOS_STATE) (
  IN     PCH_ACTIVE_BIOS_PROTOCOL   * This,
  IN     BOOLEAN                    Lock,
  IN OUT UINTN                      *Key
  );

/**
  This protocol allows the PCH to be configured to map the top 16 MB of memory
  below 4 GB to different buses, LPC/eSPI or SPI. The State reflects the current
  setting. SetState() allows consumers to request a new state, and LockState()
  allows consumers to prevent other consumers from changing the state. It is the
  caller's responsibility to configure and lock the desired state to prevent issues
  resulting from other consumers changing the state.
  Note that some functions are optional.  This means that they may be NULL.
  Caller is required to verify that an optional function is defined by checking
  that the value is not NULL.
**/
struct _PCH_ACTIVE_BIOS_PROTOCOL {
  PCH_ACTIVE_BIOS_STATE                   State;        ///< The current state mapping that is selected.
  PCH_ACTIVE_BIOS_SET_ACTIVE_BIOS_STATE   SetState;     ///< Change the current state to the requested state mapping.
  PCH_ACTIVE_BIOS_LOCK_ACTIVE_BIOS_STATE  LockState;    ///< Lock the current state mapping to prevent changes to the current state.
};

#endif
