/** @file
  This file is SampleCode of the library for Intel SA PEI Debug Policy initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2018 Intel Corporation.

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

#include "PeiPolicyDebug.h"
#include <Library/PeiSaPolicyLib.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Guid/AcpiVariable.h>
#include <Guid/MemoryTypeInformation.h>
#include <Library/HobLib.h>
#include <Platform.h>

/*++
  This function performs SA PEI Debug PreMem Policy initialization.

  @param[in] SiPreMemPolicyPpi    The Si PreMem Policy PPI instance

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
--*/
EFI_STATUS
EFIAPI
UpdatePeiSaPolicyDebugPreMem (
  IN OUT  SI_PREMEM_POLICY_PPI  *SiPreMemPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  SA_SETUP                        SaSetup;
  UINTN                           VarSize;
  GRAPHICS_PEI_PREMEM_CONFIG      *GtPreMemConfig;
  PCIE_PEI_PREMEM_CONFIG          *PciePeiPreMemConfig;
  SA_MISC_PEI_PREMEM_CONFIG       *MiscPeiPreMemConfig;

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gGraphicsPeiPreMemConfigGuid, (VOID *) &GtPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaPciePeiPreMemConfigGuid, (VOID *) &PciePeiPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *) &MiscPeiPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices       // PPI
             );
  ASSERT_EFI_ERROR ( Status);

  //
  // Get Setup SA variables
  //
  VarSize = sizeof (SA_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"SaSetup",
                               &gSaSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SaSetup
                               );

  if (!EFI_ERROR (Status)) {
    ///
    /// Initialize the PciExpress Configuration
    ///
    PciePeiPreMemConfig->DmiVc1                          = SaSetup.DmiVc1;
    PciePeiPreMemConfig->DmiVcm                          = SaSetup.DmiVcm;
    PciePeiPreMemConfig->DmiMaxLinkSpeed                 = SaSetup.DmiMaxLinkSpeed;
    PciePeiPreMemConfig->DmiGen3EqPh2Enable              = SaSetup.DmiGen3EqPh2Enable;
    PciePeiPreMemConfig->DmiGen3EqPh3Method              = SaSetup.DmiGen3EqPh3Method;
    PciePeiPreMemConfig->Peg0Gen3EqPh3Method             = SaSetup.Peg0Gen3EqPh3Method;
    PciePeiPreMemConfig->Peg1Gen3EqPh3Method             = SaSetup.Peg1Gen3EqPh3Method;
    PciePeiPreMemConfig->Peg2Gen3EqPh3Method             = SaSetup.Peg2Gen3EqPh3Method;
    PciePeiPreMemConfig->Peg0Gen3EqPh2Enable             = SaSetup.Peg0Gen3EqPh2Enable;
    PciePeiPreMemConfig->Peg1Gen3EqPh2Enable             = SaSetup.Peg1Gen3EqPh2Enable;
    PciePeiPreMemConfig->Peg2Gen3EqPh2Enable             = SaSetup.Peg2Gen3EqPh2Enable;
    PciePeiPreMemConfig->Gen3SwEqAlwaysAttempt           = SaSetup.Gen3SwEqAlwaysAttempt;
    PciePeiPreMemConfig->Gen3SwEqNumberOfPresets         = SaSetup.Gen3SwEqNumberOfPresets;
    PciePeiPreMemConfig->Gen3SwEqEnableVocTest           = SaSetup.Gen3SwEqEnableVocTest;
    PciePeiPreMemConfig->PegRxCemTestingMode             = SaSetup.PegRxCemTestingMode;
    PciePeiPreMemConfig->PegRxCemLoopbackLane            = SaSetup.PegRxCemLoopbackLane;
    PciePeiPreMemConfig->PegGenerateBdatMarginTable      = SaSetup.PegGenerateBdatMarginTable;
    PciePeiPreMemConfig->PegRxCemNonProtocolAwareness    = SaSetup.PegRxCemNonProtocolAwareness;
    PciePeiPreMemConfig->PegDisableSpreadSpectrumClocking = SaSetup.PegDisableSpreadSpectrumClocking;

    PciePeiPreMemConfig->Gen3SwEqJitterDwellTime         = SaSetup.Gen3SwEqJitterDwellTime;
    PciePeiPreMemConfig->Gen3SwEqJitterErrorTarget       = SaSetup.Gen3SwEqJitterErrorTarget;
    PciePeiPreMemConfig->Gen3SwEqVocDwellTime            = SaSetup.Gen3SwEqVocDwellTime;
    PciePeiPreMemConfig->Gen3SwEqVocErrorTarget          = SaSetup.Gen3SwEqVocErrorTarget;

    //
    // Initialize the Graphics configuration
    //
    GtPreMemConfig->PanelPowerEnable  = SaSetup.PanelPowerEnable;

    //
    // Initialize Misc SA Configuration
    //
    MiscPeiPreMemConfig->LockPTMregs        = SaSetup.LockPTMregs;
    MiscPeiPreMemConfig->BdatEnable         = SaSetup.BdatEnable;
    MiscPeiPreMemConfig->BdatTestType       = SaSetup.BdatTestType;
    MiscPeiPreMemConfig->BdatSchema         = SaSetup.BdatSchema;
  }

  return Status;
}

/*++
  This function performs SA PEI Debug Policy initialization.

  @param[in] SiPolicyPpi          The Si Policy PPI instance

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
--*/
EFI_STATUS
EFIAPI
UpdatePeiSaPolicyDebug (
  IN OUT  SI_POLICY_PPI  *SiPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  SA_SETUP                        SaSetup;
  UINTN                           VarSize;
  GRAPHICS_PEI_CONFIG             *GtConfig;
  GMM_CONFIG                      *GmmConfig;
  SKYCAM_CONFIG                   *SkyCamPolicy;
  PCIE_PEI_CONFIG                 *PciePeiConfig;
  UINT8                           Index;
  SA_MISC_PEI_CONFIG              *MiscPeiConfig;


  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gGraphicsPeiConfigGuid, (VOID *) &GtConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gGmmConfigGuid, (VOID *) &GmmConfig );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSkyCamConfigGuid, (VOID *) &SkyCamPolicy);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSaPciePeiConfigGuid, (VOID *) &PciePeiConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSaMiscPeiConfigGuid, (VOID *) &MiscPeiConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices       // PPI
             );
  ASSERT_EFI_ERROR ( Status);

  //
  // Get Setup SA variables
  //
  VarSize = sizeof (SA_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"SaSetup",
                               &gSaSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SaSetup
                               );

  if (!EFI_ERROR (Status)) {
    GtConfig->RenderStandby     = SaSetup.EnableRenderStandby;
    GtConfig->PmSupport         = SaSetup.PmSupport;

    //
    // Initialize SkyCam Configuration
    //
    SkyCamPolicy->SaImguEnable = SaSetup.SaImguEnable;
    PciePeiConfig->DmiExtSync  = SaSetup.DmiExtSync;
    PciePeiConfig->DmiIot      = SaSetup.DmiIot;

    for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
      PciePeiConfig->PegMaxPayload[Index] = SaSetup.PegMaxPayload[Index];
    }
    //
    // Initialize Misc SA Configuration
    //
    MiscPeiConfig->ChapDeviceEnable   = SaSetup.SaDevice7;
  }
  return Status;
}
