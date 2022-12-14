/** @file
  PCH Port 61h bit-4 toggling in SMM IO Trap

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

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

#include "PchInitSmm.h"


GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_CPU_PROTOCOL                        *mSmmCpu;               ///< To read and write save state
GLOBAL_REMOVE_IF_UNREFERENCED EFI_HANDLE                                  mPchIoTrapHandle;       ///< Pause and resume via EFI_HANDLE
GLOBAL_REMOVE_IF_UNREFERENCED PCH_SMM_IO_TRAP_CONTROL_PROTOCOL            *mPchSmmIoTrapControl;  ///< To pause and resume IO trap
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                                       mInternalState = 0;     ///< To store the toggle value for each CPU threads

/**
  This SMI handle will pause the IO trap in the beginning

  @param[in] DispatchHandle  - The handle of this callback, obtained when registering
  @param[in] DispatchContext - Pointer to the EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT

  @retval None
**/
VOID
PchIoTrapPort61hSmiCallback (
  IN  EFI_HANDLE                            DispatchHandle,
  IN CONST VOID                             *CallbackContext,
  IN OUT VOID                               *CommBuffer,
  IN OUT UINTN                              *CommBufferSize
  )
{
  EFI_STATUS                  Status;
  UINTN                       CpuIndex;
  UINTN                       Mask;
  UINT8                       ReadValue, WriteValue;
  EFI_SMM_SAVE_STATE_IO_INFO  IoInfo;
  //
  // Disabling the IO Trap, prevent SMI re-enter because of reading port 61h in handler itself
  //
  Status = mPchSmmIoTrapControl->Pause (
                                   mPchSmmIoTrapControl,
                                   mPchIoTrapHandle
                                   );
  ASSERT_EFI_ERROR (Status);

  //
  // Detects the CPU which causes the IO Trap
  //
  for (CpuIndex = 0; CpuIndex < gSmst->NumberOfCpus; CpuIndex++) {
    Status = mSmmCpu->ReadSaveState (
                        mSmmCpu,
                        sizeof (EFI_SMM_SAVE_STATE_IO_INFO),
                        EFI_SMM_SAVE_STATE_REGISTER_IO,
                        CpuIndex,
                        &IoInfo
                        );
    if (Status == EFI_SUCCESS) {
      //
      // Make sure the IO trap is byte read, port input only, and it is port 0x61
      //
      if ((IoInfo.IoWidth == EFI_SMM_SAVE_STATE_IO_WIDTH_UINT8) &&
          (IoInfo.IoType == EFI_SMM_SAVE_STATE_IO_TYPE_INPUT) &&
          (IoInfo.IoPort == 0x61)) {
        //
        // Read from Port 61h, toggle bit4 based on the internal state
        //
        ReadValue = IoRead8 (0x61);
        Mask = (UINTN) (1 << CpuIndex);
        mInternalState ^= Mask;
        WriteValue = (mInternalState & Mask) ? (ReadValue | 0x10) : (ReadValue & ~0x10);
        Status = mSmmCpu->WriteSaveState (
                            mSmmCpu,
                            sizeof (UINT8),
                            EFI_SMM_SAVE_STATE_REGISTER_RAX,
                            CpuIndex,
                            &WriteValue
                            );
        ASSERT_EFI_ERROR (Status);
      } //if the iotrap is the one we want
    } //if iotrap happens for this thread
  } //for (iterate for all threads)

  //
  // Resume the IO Trap
  // Port 61h bit-4 emulation will be disabled after exit boot service
  // The disabling mechanism is actually to not resume the trap.
  // This means, even after exit boot service, this handler will be called once
  // before it is disabled
  //
  if (mPchNvsArea->ExitBootServicesFlag == 0) {
    Status = mPchSmmIoTrapControl->Resume (
                                     mPchSmmIoTrapControl,
                                     mPchIoTrapHandle
                                     );
    ASSERT_EFI_ERROR (Status);
  } else {
    DEBUG ((DEBUG_INFO, "Port61H trap has been disabled. \n"));
  }
}


/**
  Locate required protocol and register the IO trap in this entry point

  @param[in] ImageHandle - Handle for the image of this driver
  @param[in] SystemTable - Pointer to the EFI System Table

  @retval EFI_SUCCESS    - PCH SMM handler was installed
**/
EFI_STATUS
EFIAPI
InstallIoTrapPort61h (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_IO_TRAP_REGISTER_CONTEXT          PchIoTrapContext;

  DEBUG ((DEBUG_INFO, "InstallIoTrapPort61h()\n"));

  if (mPchConfigHob->Port61hSmm.Enable == 0) {
    return EFI_SUCCESS;
  }

  //
  // Locate the protocol to read and write save state
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmCpuProtocolGuid, NULL, (VOID **) &mSmmCpu);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate the protocol for pause and resume
  //
  Status = gSmst->SmmLocateProtocol (&gPchSmmIoTrapControlGuid, NULL, (VOID **) &mPchSmmIoTrapControl);
  ASSERT_EFI_ERROR (Status);

  //
  // Configure trap type, in ECP, MergeDisable should be TRUE for pause and resume to work
  //
  PchIoTrapContext.Type     = ReadTrap;
  PchIoTrapContext.Length   = 1;
  PchIoTrapContext.Address  = 0x61;
  Status = mPchIoTrap->Register (
                         mPchIoTrap,
                         (EFI_SMM_HANDLER_ENTRY_POINT2) PchIoTrapPort61hSmiCallback,
                         &PchIoTrapContext,
                         &mPchIoTrapHandle
                         );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}