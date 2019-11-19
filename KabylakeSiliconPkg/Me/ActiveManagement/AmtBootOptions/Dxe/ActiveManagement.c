/** @file
  Defines and prototypes for the ActiveManagement driver.
  This driver implements the ActiveManagement protocol for iAMT.
  It provides some functions to get Boot Options from ASF.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2004 - 2015 Intel Corporation.

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

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include "ActiveManagement.h"
//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED ASF_BOOT_OPTIONS            *mAsfBootOptions;

GLOBAL_REMOVE_IF_UNREFERENCED ACTIVE_MANAGEMENT_INSTANCE  ActiveManagementInstance = {
  ACTIVE_MANAGEMENT_PRIVATE_DATA_SIGNATURE,
  NULL,
  {
    DXE_ACTIVE_MANAGEMENT_PROTOCOL_REVISION_1,
    (ACTIVE_MANAGEMENT_ASF_BOOT_OPTIONS_GET) GetAsfBootOptions,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetStorageRedirState,
    (ACTIVE_MANAGEMENT_STORAGE_REDIR_BOOT_DEVICE_SELECTED) GetStorageRedirBootDeviceSelected,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetEnforceSecureBootState,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetRemoteFlashState,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetBiosSetupState,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetBiosPauseState,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetKvmState,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetSecureEraseState,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetConsoleLockState,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetProgressMsgRequest,
    (ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetSolState
  },
  NULL
};

//
// Function implementations
//

/**
  Return current ASF Boot Options

  @param[in] This                 Pointer to the EFI_ACTIVE_MANAGEMENT_PROTOCOL instance.
  @param[in, out] AsfBootOptions  ASF Boot Options

  @retval EFI_SUCCESS             Boot options updated
**/
EFI_STATUS
EFIAPI
GetAsfBootOptions (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT ASF_BOOT_OPTIONS                 **AsfBootOptions
  )
{
  *AsfBootOptions = mAsfBootOptions;
  return EFI_SUCCESS;
}

/**
  Check if ASF boot options is present.

  @retval True                    ASF boot option is present.
  @retval False                   ASF boot option is not present
**/
BOOLEAN
IsBootOptionsPresent (
  VOID
  )
{
  return mAsfBootOptions->SubCommand == ASF_BOOT_OPTIONS_PRESENT;
}

/**
  Check if LANA ID of ASF boot options is Industry ID.

  @retval True                    IANA ID of ASF boot options is Industry ID.
  @retval False                   IANA ID of ASF boot options is not Industry ID.
**/
BOOLEAN
IsIndustryIanaId (
  VOID
  )
{
  volatile BOOLEAN  RetVal;

  RetVal = FALSE;
  if (IsBootOptionsPresent ()) {
    if (mAsfBootOptions->IanaId == ASF_INDUSTRY_CONVERTED_IANA) {
      RetVal = TRUE;
    }
  }

  return RetVal;
}

/**
  Check if LANA ID of ASF boot options is Intel ID.

  @retval True                    IANA ID of ASF boot options is Intel ID.
  @retval False                   IANA ID of ASF boot options is not Intel ID.
**/
BOOLEAN
IsIntelIanaId (
  VOID
  )
{
  volatile BOOLEAN  RetVal;

  RetVal = FALSE;

  if (IsBootOptionsPresent ()) {
    if (mAsfBootOptions->IanaId == ASF_INTEL_CONVERTED_IANA) {
      RetVal = TRUE;
    }
  }

  return RetVal;
}

/**
  Check if it is Intel ASF boot options.

  @retval True                    It is Intel ASF boot options.
  @retval False                   It is not Intel ASF boot options.
**/
BOOLEAN
IsIntelAmtBootOptions (
  VOID
  )
{
  BOOLEAN RetVal;

  RetVal = FALSE;

  if (IsIntelIanaId ()) {
    if (mAsfBootOptions->SpecialCommand == ASF_INTEL_OEM_CMD) {
      RetVal = TRUE;
    }
  }

  return RetVal;
}

///
/// Special Command Parameters
///

/**
  Check the Special Command Parameter of Intel ASF boot options

  @param[in] Options              Special Command Parameter bit we want to check
                                  Bit 0: Set if Storage Redirection is to be used on the next boot. Parameter 2 is set
                                  to the driver number to be used.
                                  Bit 1: Set if Secure Boot is enforced over Storage Redirection
                                  Bit 2: Set if the BIOS is to be re-flashed on the next boot
                                  Bit 3: Set if the BIOS is to boot into the BIOS set-up screen.
                                  Bit 4: Boot into BIOS Pause on the next boot is supported
                                  Bit 5: Set if the BIOS is to participate in KVM session
                                  Bit 6: Set if the BIOS is to start a Remote Secure Erase (RSE) session
  @param[in, out] CurrentState    Return the state of result
                                  True  - Special Command Parameter bit in Options is enabled.
                                  False - Special Command Parameter bit in Options is disabled.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
GetSpecialParamState (
  IN     UINT16   Options,
  IN OUT BOOLEAN  *CurrentState
  )
{
  *CurrentState = FALSE;
  if (IsIntelAmtBootOptions ()) {
    if ((mAsfBootOptions->SpecialCommandParam & Options) == Options) {
      *CurrentState = TRUE;
    }
  }

  return EFI_SUCCESS;
}

/**
  Check the OEM Parameter of Intel ASF boot options

  @param[in] Options              OEM Parameter bit we want to check
                                  Bit 0: Set if SOL is to be used on the next boot.
  @param[in, out] CurrentState    Return the state of result
                                  True : OEM Parameter bit in Options is enabled.
                                  False : OEM Parameter bit in Options is disabled.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
GetBootOptionsMaskState (
  IN     UINT16   Options,
  IN OUT BOOLEAN  *CurrentState
  )
{
  *CurrentState = FALSE;
  if (IsBootOptionsPresent ()) {
    if ((mAsfBootOptions->BootOptions & Options) == Options) {
      *CurrentState = TRUE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This will return Storage Redirection Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of Storage Redireciton Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetStorageRedirState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (USE_STORAGE_REDIR, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return Storage Redirection boot device to boot

  @param[in] This                 The address of protocol
  @param[in, out] IdeBootDevice   Return the boot device number to boot
                                  Bit 1  Bit0
                                   0      0    USBr is connected to CD/DVD device
                                   0      1    USBr is connected to floppy device
                                  Bits 2-7: Reserved set to 0

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetStorageRedirBootDeviceSelected (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT UINT8                            *BootDevice
  )
{
  *BootDevice = (UINT8) ((mAsfBootOptions->SpecialCommandParam & STORAGE_REDIRECTION_BOOT_DEVICE_MASK)
                         >> STORAGE_REDIRECTION_BOOT_DEVICE_SHIFT);
  return EFI_SUCCESS;
}

/**
  This will return Enforce Secure Boot over Storage Redirection Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of Enforce Secure Boot over Storage Redirection Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetEnforceSecureBootState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (ENFORCE_SECURE_BOOT, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return Remote Flash Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of Remote Flash Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetRemoteFlashState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (REFLASH_BIOS, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return BIOS Setup Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of BIOS Setup Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetBiosSetupState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (BIOS_SETUP, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return BIOS Pause Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of BIOS Pause Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetBiosPauseState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (BIOS_PAUSE, CurrentState);
  return EFI_SUCCESS;
}


/**
  This will return KVM Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of KVM Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetKvmState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (USE_KVM, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return Secure Erase state.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of Secure Erase Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetSecureEraseState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  *CurrentState = FALSE;
  if (IsIntelAmtBootOptions ()) {
    GetSpecialParamState (ENABLE_SECURE_ERASE, CurrentState);
  }
  return EFI_SUCCESS;
}

//
// Boot Options
//


/**
  This will return Console Lock Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of Console Lock Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetConsoleLockState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetBootOptionsMaskState (LOCK_KEYBOARD, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return Progress Event Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of progress event Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetProgressMsgRequest (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetBootOptionsMaskState (FORCE_PROGRESS_EVENTS, CurrentState);
  return EFI_SUCCESS;
}

//
// OEM Parameters
//


/**
  Check the OEM Parameter of Intel ASF boot options

  @param[in] Options              OEM Parameter bit we want to check
                                  Bit 0: Set if SOL is to be used on the next boot.
  @param[in, out] CurrentState    Return the state of result
                                  True  : OEM Parameter bit in Options is enabled.
                                  False : OEM Parameter bit in Options is disabled.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
GetOemParamatersState (
  IN     UINT16   Options,
  IN OUT BOOLEAN  *CurrentState
  )
{
  *CurrentState = FALSE;
  if (IsIntelAmtBootOptions ()) {
    if ((mAsfBootOptions->OemParameters & Options) == Options) {
      *CurrentState = TRUE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This will return Serial-over-Lan Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in, out] CurrentState    Return the state of Serial-over-Lan Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetSolState (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetOemParamatersState (USE_SOL, CurrentState);
  return EFI_SUCCESS;
}

/**
  Entry point for the Active Management Driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
ActiveManagementEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS  Status;

  ///
  /// Check policy if AMT is supported
  ///
  if (!IsAmtBiosSupportEnabled () || !IsAsfBiosSupportEnabled ()) {
    SolDisable ();
    return EFI_UNSUPPORTED;
  }
  ///
  /// Get Protocol for ASF
  ///
  Status = gBS->LocateProtocol (
                  &gAlertStandardFormatProtocolGuid,
                  NULL,
                  (VOID **) &(ActiveManagementInstance.Asf)
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Get ASF Boot Options
  ///
  Status = ActiveManagementInstance.Asf->GetBootOptions (ActiveManagementInstance.Asf, &mAsfBootOptions);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Install the EFI_ACTIVE_MANAGEMENT_PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(ActiveManagementInstance.Handle),
                  &gActiveManagementProtocolGuid,
                  &(ActiveManagementInstance.ActiveManagementProtocol),
                  NULL
                  );

  return Status;
}
