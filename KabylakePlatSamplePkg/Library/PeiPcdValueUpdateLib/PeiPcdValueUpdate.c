/** @file
  This file is SampleCode for Intel PEI PCD Value Update.

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PchInfoLib.h>

#include <BootTime.h>
#include <SetupVariable.h>
#include <Ppi/ReadOnlyVariable2.h>




/**
  Update PCD value

  @retval EFI_SUCCESS           The function completed successfully.
**/
EFI_STATUS
EFIAPI
PcdValueUpdate (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINTN                            VarSize;
  PCH_SERIES                       PchSeries;
  CPU_SETUP                        CpuSetup;
  PCH_SETUP                        PchSetup;
  SETUP_DATA                       SetupData;
  BOOTTIME_SETUP                   BootTimeSetup;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  UINT32                           PcieRootPortHpeData;
  UINT8                            Index;


  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,  // GUID
             0,                                 // INSTANCE
             NULL,                              // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices        // PPI
             );
  ASSERT_EFI_ERROR (Status);

  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SetupData
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VarSize = sizeof (CPU_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"CpuSetup",
                               &gCpuSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &CpuSetup
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VarSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &PchSetup
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

// AMI_OVERRIDE_START - We use our own Fast Boot module.
#if 0
  if (SetupData.FastBoot) {
    //
    // Disable MdeModulePkg core PcdAtaSmartEnable to skip HDD SMART enabling
    //
    PcdSetBoolS (PcdAtaSmartEnable, FALSE);
    //
    // Enable gClientCommonModuleTokenSpaceGuid PcdFastPS2Detection to enable quick PS2 device detection
    //
    PcdSetBoolS (PcdFastPS2Detection, TRUE);
  }
#endif
// AMI_OVERRIDE_END - We use our own Fast Boot module.
  //
  // Change PcdAcpiDebugEnable PCD to TRUE if ACPI DEBUG is enabled in Setup menu
  //

  if (SetupData.AcpiDebug) {
    PcdSetBoolS (PcdAcpiDebugEnable, TRUE);
  } else {
    PcdSetBoolS (PcdAcpiDebugEnable, FALSE);
  }

  //
  // Change PcdOverclockEnable PCD to TRUE if Over-clocking Support is enabled in Setup menu
  //
  if (CpuSetup.OverclockingSupport) {
    PcdSetBoolS (PcdOverclockEnable, TRUE);
  } else {
    PcdSetBoolS (PcdOverclockEnable, FALSE);
  }

  //
  // Update AHCI max ports
  //
  PchSeries = GetPchSeries ();
  switch (PchSeries) {
    case PchLp:
      PcdSet8S (PcdAhciMaxPorts, PCH_LP_AHCI_MAX_PORTS);
      break;
    case PchH:
      PcdSet8S (PcdAhciMaxPorts, PCH_H_AHCI_MAX_PORTS);
      break;
    default:
      PcdSet8S (PcdAhciMaxPorts, PCH_LP_AHCI_MAX_PORTS);
      break;
  }
  PcieRootPortHpeData = 0;
  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    PcieRootPortHpeData = PcieRootPortHpeData | ((UINT32)(PchSetup.PcieRootPortHPE[Index]) << Index);
  }

  PcdSet32S (PcdPchPcieRootPortHpe, PcieRootPortHpeData);
  //
  // Change PcdBootTime PCD to TRUE if BootTimeLog is enabled in Setup menu
  //
  VarSize = sizeof (BOOTTIME_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"BootTime",
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &BootTimeSetup
                               );
  if (!EFI_ERROR (Status)) {
    (BootTimeSetup.BootTime == 1) ? PcdSetBoolS(PcdBootTime, TRUE) : PcdSetBoolS(PcdBootTime, FALSE);
  }

  return EFI_SUCCESS;
}
