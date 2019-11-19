/** @file
  System reset library services.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2017 Intel Corporation.

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
#include <Base.h>
#include <PchAccess.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MmPciLib.h>
#include <Library/DxeMeLib.h>  // AdvancedFeaturesContent
#include <Library/ResetSystemLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Protocol/PchReset.h>

GLOBAL_REMOVE_IF_UNREFERENCED UINTN            mDxeResetSystemPmcBase;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16           mDxeResetSystemABase;

/**
  Dump reset message for debug build readability
**/
VOID
DumpResetMessage (
  VOID
  )
{
  DEBUG_CODE_BEGIN ();
  UINTN       Index;
  //
  // ******************************
  // **    SYSTEM REBOOT !!!     **
  // ******************************
  //
  for (Index = 0; Index < 30; Index++) {
    DEBUG ((DEBUG_INFO, "*"));
  }
  DEBUG ((DEBUG_INFO, "\n**    SYSTEM REBOOT !!!     **\n"));
  for (Index = 0; Index < 30; Index++) {
    DEBUG ((DEBUG_INFO, "*"));
  }
  DEBUG ((DEBUG_INFO, "\n"));
  DEBUG_CODE_END ();
}

/**
  Execute call back function for Pch Reset.

  @param[in] ResetType            Reset Types which includes GlobalReset.

  @retval EFI_SUCCESS             The callback function has been done successfully
  @retval EFI_NOT_FOUND           Failed to find Pch Reset Callback protocol. Or, none of
                                  callback protocol is installed.
**/
EFI_STATUS
EFIAPI
PchResetCallback (
  IN  PCH_RESET_TYPE          ResetType
  )
{
  EFI_STATUS                  Status;
  UINTN                       NumHandles;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       Index;
  PCH_RESET_CALLBACK_PROTOCOL *PchResetCallback;

  ///
  /// Retrieve all instances of Pch Reset Callback protocol
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gPchResetCallbackProtocolGuid,
                  NULL,
                  &NumHandles,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    ///
    /// Those drivers that need to install Pch Reset Callback protocol have the responsibility
    /// to make sure themselves execute before Reset driver.
    ///
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((DEBUG_ERROR | DEBUG_INFO, "None of Pch Reset Callback protocol is installed.\n"));
    }
    return Status;
  }

  for (Index = 0; Index < NumHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gPchResetCallbackProtocolGuid,
                    (VOID **) &PchResetCallback
                    );
    ASSERT_EFI_ERROR (Status);

    if (!EFI_ERROR (Status)) {
      PchResetCallback->ResetCallback (ResetType);
    } else {
      DEBUG ((DEBUG_ERROR | DEBUG_INFO, "Failed to locate Pch Reset Callback protocol.\n"));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Calling this function causes a system-wide reset. This sets
  all circuitry within the system to its initial state. This type of reset
  is asynchronous to system operation and operates without regard to
  cycle boundaries.

  System reset should not return, if it returns, it means the system does
  not support cold reset.
**/
VOID
EFIAPI
ResetCold (
  VOID
  )
{
  //
  // Loop through callback functions of PchResetCallback Protocol
  //
  PchResetCallback ((PCH_RESET_TYPE) EfiResetCold);

  DumpResetMessage ();

  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_FULLRESET);
}

/**
  Calling this function causes a system-wide initialization. The processors
  are set to their initial state, and pending cycles are not corrupted.

  System reset should not return, if it returns, it means the system does
  not support warm reset.
**/
VOID
EFIAPI
ResetWarm (
  VOID
  )
{
  //
  // Loop through callback functions of PchResetCallback Protocol
  //
  PchResetCallback ((PCH_RESET_TYPE) EfiResetWarm);

  DumpResetMessage ();
  //
  // In case there are pending capsules to process, need to flush the cache.
  //
  AsmWbinvd ();

  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET);
}

/**
  Calling this function causes the system to enter a power state equivalent
  to the ACPI G2/S5 or G3 states.

  System shutdown should not return, if it returns, it means the system does
  not support shut down reset.
**/
VOID
EFIAPI
ResetShutdown (
  VOID
  )
{
  UINT32         Data32;

  //
  // Loop through callback functions of PchResetCallback Protocol
  //
  PchResetCallback ((PCH_RESET_TYPE) EfiResetShutdown);

  ///
  /// Firstly, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
  ///
  IoWrite32 ((UINTN) (mDxeResetSystemABase + R_PCH_ACPI_GPE0_EN_127_96), 0);

  ///
  /// Secondly, PwrSts register must be cleared
  ///
  /// Write a "1" to bit[8] of power button status register at
  /// (PM_BASE + PM1_STS_OFFSET) to clear this bit
  ///
  IoWrite16 ((UINTN) (mDxeResetSystemABase + R_PCH_ACPI_PM1_STS), B_PCH_ACPI_PM1_STS_PWRBTN);

  ///
  /// Finally, transform system into S5 sleep state
  ///
  Data32 = IoRead32 ((UINTN) (mDxeResetSystemABase + R_PCH_ACPI_PM1_CNT));

  Data32 = (UINT32) ((Data32 &~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);

  IoWrite32 ((UINTN) (mDxeResetSystemABase + R_PCH_ACPI_PM1_CNT), Data32);

  Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

  DumpResetMessage ();

  IoWrite32 ((UINTN) (mDxeResetSystemABase + R_PCH_ACPI_PM1_CNT), Data32);

  return;
}

/**
  Internal function to execute the required HECI command for GlobalReset,
  if failed will use PCH Reest.

**/
STATIC
VOID
DxePchGlobalReset (
  VOID
  )
{
#ifndef MINTREE_FLAG
  EFI_STATUS       ResetStatus;
#endif
  //
  // Loop through callback functions of PchResetCallback Protocol
  //
  PchResetCallback (PchGlobalReset);

  //
  // PCH BIOS Spec Section 4.6 GPIO Reset Requirement
  //
  MmioOr32 (mDxeResetSystemPmcBase + R_PCH_PMC_ETR3, (UINT32) B_PCH_PMC_ETR3_CF9GR);

  DumpResetMessage ();

#ifndef MINTREE_FLAG
  //
  // Let ME do global reset if Me Fw is available
  //
  ResetStatus = HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
  if (!EFI_ERROR (ResetStatus)) {
    MicroSecondDelay (1000000);
  }
  //
  // ME Global Reset should fail after EOP is sent. Go to use PCH Reset.
  //
#endif
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_FULLRESET);
}

/**
  Calling this function causes the system to enter a power state for platform specific.

  @param[in] ResetStatus          The status code for the reset.
  @param[in] DataSize             The size of ResetData in bytes.
  @param[in] ResetData            Optional element used to introduce a platform specific reset.
                                  The exact type of the reset is defined by the EFI_GUID that follows
                                  the Null-terminated Unicode string.

**/
VOID
EFIAPI
ResetPlatformSpecific (
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN VOID             *ResetData OPTIONAL
  )
{
  EFI_GUID            *GuidPtr;

  if (ResetData == NULL) {
    DEBUG ((DEBUG_ERROR, "[DxeResetSystemLib] ResetData is not available.\n"));
    return;
  }
  GuidPtr = (EFI_GUID *) ((UINT8 *) ResetData + DataSize - sizeof (EFI_GUID));
  if (CompareGuid (GuidPtr, &gPchGlobalResetGuid)) {
    DxePchGlobalReset();
  } else {
    return;
  }
}

/**
  Calling this function causes the system to enter a power state for capsule update.

  Reset update should not return, if it returns, it means the system does
  not support capsule update.

**/
VOID
EFIAPI
EnterS3WithImmediateWake (
  VOID
  )
{
  ASSERT (FALSE);
}

/**
  The library constructuor.

  The function does the necessary initialization work for this library DXE instance.

  @param[in]  ImageHandle       The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable       A pointer to the EFI system table.

  @retval     EFI_SUCCESS       The function always return EFI_SUCCESS for now.
                                It will ASSERT on error for debug version.
**/
EFI_STATUS
EFIAPI
DxeResetSystemLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  //
  // Get ACPI Base Address and PMC Base Address
  //
  PchAcpiBaseGet (&mDxeResetSystemABase);

  mDxeResetSystemPmcBase = MmPciBase (
                             DEFAULT_PCI_BUS_NUMBER_PCH,
                             PCI_DEVICE_NUMBER_PCH_PMC,
                             PCI_FUNCTION_NUMBER_PCH_PMC
                             );
  return EFI_SUCCESS;
}

