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
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciLib.h>
#include <Library/PeiMeLib.h>  // AdvancedFeaturesContent
#include <Library/PchResetLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Ppi/PchReset.h>
#ifdef FSP_FLAG
#include <Library/FspCommonLib.h>
#endif
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

  @param[in] ResetType            Reset Types which includes PowerCycle, GlobalReset.

  @retval EFI_SUCCESS             The callback function has been done successfully
  @retval EFI_NOT_FOUND           Failed to find Pch Reset Callback protocol. Or, none of
                                  callback protocol is installed.
**/
EFI_STATUS
PchResetCallback (
  IN  PCH_RESET_TYPE      ResetType
  )
{
  EFI_STATUS              Status;
  UINTN                   Instance;
  PCH_RESET_CALLBACK_PPI  *PchResetCallbackPpi;

  Instance = 0;
  do {
    Status = PeiServicesLocatePpi (
               &gPchResetCallbackPpiGuid,
               Instance,
               NULL,
               (VOID **) &PchResetCallbackPpi
               );

    switch (Status) {
      case EFI_SUCCESS:
        PchResetCallbackPpi->ResetCallback (ResetType);
        break;
      case EFI_NOT_FOUND:
        break;
      default:
        ASSERT_EFI_ERROR (Status);
        break;
    }
    ++Instance;
  } while (Status == EFI_SUCCESS);

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
  // Loop through callback functions of PchResetCallback PPI
  //
  PchResetCallback ((PCH_RESET_TYPE) EfiResetCold);
#ifdef FSP_FLAG
  FspApiReturnStatusReset (FSP_STATUS_RESET_REQUIRED_COLD);
#endif
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
  // Loop through callback functions of PchResetCallback PPI
  //
  PchResetCallback ((PCH_RESET_TYPE) EfiResetWarm);
#ifdef FSP_FLAG
  FspApiReturnStatusReset (FSP_STATUS_RESET_REQUIRED_WARM);
#endif
  DumpResetMessage ();

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
  UINT16         ABase;
  UINT32         Data32;

  //
  // Loop through callback functions of PchResetCallback PPI
  //
  PchResetCallback ((PCH_RESET_TYPE) EfiResetShutdown);

  PchAcpiBaseGet (&ABase);
  ///
  /// Firstly, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
  ///
  IoWrite32 ((UINTN) (ABase + R_PCH_ACPI_GPE0_EN_127_96), 0);

  ///
  /// Secondly, PwrSts register must be cleared
  ///
  /// Write a "1" to bit[8] of power button status register at
  /// (PM_BASE + PM1_STS_OFFSET) to clear this bit
  ///
  IoWrite16 ((UINTN) (ABase + R_PCH_ACPI_PM1_STS), B_PCH_ACPI_PM1_STS_PWRBTN);

  ///
  /// Finally, transform system into S5 sleep state
  ///
  Data32 = IoRead32 ((UINTN) (ABase + R_PCH_ACPI_PM1_CNT));

  Data32 = (UINT32) ((Data32 &~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);

  IoWrite32 ((UINTN) (ABase + R_PCH_ACPI_PM1_CNT), Data32);

  Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

  DumpResetMessage ();

  IoWrite32 ((UINTN) (ABase + R_PCH_ACPI_PM1_CNT), Data32);

  return;
}

/**
  Internal function to execute the required HECI command for GlobalReset,
  if failed will use PCH Reest.

**/
STATIC
VOID
PeiPchGlobalReset (
  VOID
  )
{
#ifndef MINTREE_FLAG
  EFI_STATUS       Status;
  EFI_STATUS       ResetStatus;
#endif
  UINTN            PchPmcBase;

  //
  // Loop through callback functions of PchResetCallback PPI
  //
  PchResetCallback (PchGlobalReset);

#ifdef FSP_FLAG
  FspApiReturnStatusReset (FSP_STATUS_RESET_REQUIRED_3);
#endif

  PchPmcBase = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_PMC,
                 PCI_FUNCTION_NUMBER_PCH_PMC
                 );
  //
  // PCH BIOS Spec Section 4.6 GPIO Reset Requirement
  //
  MmioOr32 (PchPmcBase + R_PCH_PMC_ETR3, (UINT32) B_PCH_PMC_ETR3_CF9GR);

  DumpResetMessage ();

#ifndef MINTREE_FLAG
  //
  // Locate ME Did Sent Ppi
  //
  Status = PeiServicesLocatePpi (
             &gMeDidSentPpiGuid,
             0,
             NULL,
             NULL
             );
  if (Status == EFI_SUCCESS) {
    //
    // After sending DRAM Init Done to ME FW, please do the global reset through HECI.
    //
    ResetStatus = PeiHeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
  }
  //
  // In case reset through HECI fails and we get here, returning EFI_NOT_FOUND will result in continuing with CF9 reset
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
    DEBUG ((DEBUG_ERROR, "[PeiResetSystemLib] ResetData is not available.\n"));
    return;
  }
  GuidPtr = (EFI_GUID *) ((UINT8 *) ResetData + DataSize - sizeof (EFI_GUID));
  if (CompareGuid (GuidPtr, &gPchGlobalResetGuid)) {
    PeiPchGlobalReset();
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

