/** @file
Do Platform Stage System Agent initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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

#include "PeiSaPolicyUpdate.h"
#include <Guid/MemoryTypeInformation.h>
#include <Library/HobLib.h>
#include <Platform.h>
#include <Ppi/FirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiPeiCis.h>
#include <Core/Pei/PeiMain.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiSaPolicyLib.h>
// AMI_OVERRIDE_START - for AMI Chipset Setup.
#include <NbSetupData.h>

#define SaSetup         SystemConfiguration
// AMI_OVERRIDE_END - for AMI Chipset Setup.

//
// AdvancedFeaturesBegin
//
EFI_PEI_PPI_DESCRIPTOR  mLoadGopConfigPeiBinPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gLoadGopConfigPeiBinPpiGuid,
  NULL
};
//
// AdvancedFeaturesEnd
//

/**
  UpdatePeiSaPolicy performs SA PEI Policy initialization

  @param[in out] SiPolicyPpi     - SI_POLICY PPI

  @retval EFI_SUCCESS              The policy is installed and initialized.
**/
EFI_STATUS
UpdatePeiSaPolicy (
  IN OUT   SI_POLICY_PPI      *SiPolicyPpi
  )
{
  EFI_STATUS                      Status;
  VOID                            *Buffer;
#ifdef FSP_WRAPPER_FLAG
  VOID                            *MemBuffer;
#endif
  UINT32                          Size;
  GRAPHICS_PEI_CONFIG             *GtConfig;
#ifndef MINTREE_FLAG
  UINT8                           Index;
// AMI_OVERRIDE_START - for AMI Chipset Setup. 
#if 0
  EFI_STATUS                      Status2;
#endif
// AMI_OVERRIDE_END - for AMI Chipset Setup.
// AMI_OVERRIDE_START - for AMI Chipset Setup.
#if 0
  SA_SETUP                        SaSetup;
#endif
// AMI_OVERRIDE_END - for AMI Chipset Setup.
// AMI_OVERRIDE_START - for AMI Chipset Setup.
  CONST EFI_PEI_SERVICES          **PeiServices;
  NB_SETUP_DATA                   SystemConfiguration = {0};
// AMI_OVERRIDE_END - for AMI Chipset Setup.
// AMI_OVERRIDE_START - for AMI Chipset Setup.
#if 0
  UINTN                           VarSize;
#endif
// AMI_OVERRIDE_END - for AMI Chipset Setup.
  WDT_PPI                         *gWdtPei;
  VTD_CONFIG                      *Vtd;
  GMM_CONFIG                      *GmmConfig;
  PCIE_PEI_CONFIG                 *PciePeiConfig;
  SA_MISC_PEI_CONFIG              *MiscPeiConfig;
  CPU_FAMILY                      CpuFamilyId;
  CPU_STEPPING                    CpuStepping;
// AMI_OVERRIDE_START - Get the CsmFlag from SiConfig.
  SI_CONFIG                       *SiConfig;
// AMI_OVERRIDE_END - Get the CsmFlag from SiConfig.
// AMI_OVERRIDE_START - for AMI Chipset Setup.
#if 0
  SETUP_DATA                      SystemConfiguration;
#endif
// AMI_OVERRIDE_END - for AMI Chipset Setup.
// AMI_OVERRIDE_START - Vbt and logo are bult by PeiVbtFdfFileStatements.txt.
  EFI_GUID                        PeiVbtGuid  = {0x7e175642, 0xf3ad, 0x490a, 0x9f, 0x8a, 0x2e, 0x9f, 0xc6, 0x93, 0x3d, 0xdd};
  EFI_GUID                        PeiLogoGuid = {0x52bf7d53, 0x79cf, 0x4555, 0xa5, 0x17, 0x1e, 0x8b, 0xb3, 0x04, 0x23, 0x32};
// AMI_OVERRIDE_END -  Vbt and logo are bult by PeiVbtFdfFileStatements.txt.
  EFI_BOOT_MODE                   BootMode;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
#endif //MINTREE_FLAG

  DEBUG((DEBUG_INFO, "\nUpdating SA Policy in Post Mem\n"));

  Size          = 0;
  GtConfig      = NULL;
#ifndef MINTREE_FLAG
  Vtd           = NULL;
  GmmConfig     = NULL;
  PciePeiConfig = NULL;
  MiscPeiConfig = NULL;

  CpuFamilyId = GetCpuFamily();
  CpuStepping = GetCpuStepping();
#endif //MINTREE_FLAG

  Status = GetConfigBlock((VOID *) SiPolicyPpi, &gGraphicsPeiConfigGuid, (VOID *)&GtConfig);
  ASSERT_EFI_ERROR(Status);
#ifndef MINTREE_FLAG
  Status = GetConfigBlock((VOID *) SiPolicyPpi, &gVtdConfigGuid, (VOID *)&Vtd);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock((VOID *) SiPolicyPpi, &gGmmConfigGuid, (VOID *)&GmmConfig);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSaPciePeiConfigGuid, (VOID *)&PciePeiConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSaMiscPeiConfigGuid, (VOID *)&MiscPeiConfig);
  ASSERT_EFI_ERROR (Status);

// AMI_OVERRIDE_START - Get the CsmFlag from SiConfig.
  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSiConfigGuid, (VOID *) &SiConfig);
  ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_END - Get the CsmFlag from SiConfig.

  //
  // Locate WDT_PPI (ICC WDT PPI)
  //
  gWdtPei = NULL;
  Status = PeiServicesLocatePpi(
             &gWdtPpiGuid,
             0,
             NULL,
             (VOID **) &gWdtPei
             );

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR(Status);

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi(
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices       // PPI
             );
  ASSERT_EFI_ERROR(Status);

// AMI_OVERRIDE_START - for AMI Chipset Setup.
#if 0
  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );

  VarSize = sizeof (SA_SETUP);
  Status2 = VariableServices->GetVariable(
                                VariableServices,
                                L"SaSetup",
                                &gSaSetupVariableGuid,
                                NULL,
                                &VarSize,
                                &SaSetup
                                );

#else
    PeiServices = GetPeiServicesTablePointer ();
    GetNbSetupData ((VOID*)PeiServices, &SystemConfiguration, TRUE); 
#endif
// AMI_OVERRIDE_END - for AMI Chipset Setup.
#endif // MINTREE_FLAG

// AMI_OVERRIDE_START - for AMI Chipset Setup.
#if 0
  if (!EFI_ERROR (Status)
      && !EFI_ERROR (Status2) // AdvancedFeaturesContent
     ) {
#else
  if (!EFI_ERROR (Status)) {
#endif
// AMI_OVERRIDE_END - for AMI Chipset Setup.

#ifndef MINTREE_FLAG
    GtConfig->PavpEnable          = SaSetup.PavpEnable;
    GtConfig->CdClock             = SystemConfiguration.CdClock;
    GtConfig->CdynmaxClampEnable  = SaSetup.CdynmaxClampEnable;
    GtConfig->PeiGraphicsPeimInit = SaSetup.PeiGraphicsPeimInit;
    GtConfig->GtFreqMax           = SaSetup.GtFreqMax;
    GtConfig->GtFreqMax           = SaSetup.GtFreqMax;
#else
    GtConfig->PeiGraphicsPeimInit = 1;
#endif // MINTREE_FLAG


    Buffer = NULL;

// AMI_OVERRIDE_START - Reduce post time.
    if (GtConfig->PeiGraphicsPeimInit == 1) {
//    PeiGetSectionFromFv(gIntelPeiGraphicsVbtGuid, &Buffer, &Size);
      PeiGetSectionFromFv(PeiVbtGuid, &Buffer, &Size);
    }
// AMI_OVERRIDE_END - Reduce post time.

    if (Buffer == NULL) {
      DEBUG((DEBUG_WARN, "Could not locate VBT\n"));
    }
//
// AdvancedFeaturesBegin
//
#ifdef EFI_S3_RESUME

// AMI_OVERRIDE_START - Get the CsmFlag from SiConfig.
#if 0
    if ((BootMode == BOOT_ON_S3_RESUME) || (PcdGetBool (PcdCsmEnable))) {
#else
    if ((BootMode == BOOT_ON_S3_RESUME) || (((UINT8) SiConfig->CsmFlag)) == 1) {
#endif
// AMI_OVERRIDE_END - Get the CsmFlag from SiConfig.
      GtConfig->GraphicsConfigPtr = NULL;
    } else {
#endif //EFI_S3_RESUME
//
// AdvancedFeaturesEnd
//
#ifdef FSP_WRAPPER_FLAG
      MemBuffer = (VOID *)AllocatePages (EFI_SIZE_TO_PAGES ((UINTN)Size));
      if ((MemBuffer != NULL) && (Buffer != NULL)) {
        CopyMem (MemBuffer, (VOID *)Buffer, (UINTN)Size);
        GtConfig->GraphicsConfigPtr = MemBuffer;
      } else {
        DEBUG((DEBUG_WARN, "Error in locating / copying VBT.\n"));
        GtConfig->GraphicsConfigPtr = NULL;
      }
#else
      GtConfig->GraphicsConfigPtr = Buffer;
#endif
//
// AdvancedFeaturesBegin
//
  ///
  /// Install PPI to load Gop Config Pei binary
  ///
  Status = PeiServicesInstallPpi (&mLoadGopConfigPeiBinPpi);
  ASSERT_EFI_ERROR (Status);
#ifdef EFI_S3_RESUME
    }
#endif //EFI_S3_RESUME
//
// AdvancedFeaturesEnd
//
    DEBUG((DEBUG_INFO, "Vbt Pointer from PeiGetSectionFromFv is 0x%x\n", GtConfig->GraphicsConfigPtr));
    DEBUG((DEBUG_INFO, "Vbt Size from PeiGetSectionFromFv is 0x%x\n", Size));

    Buffer = NULL;
// AMI_OVERRIDE_START - Reduce post time.
    if (GtConfig->PeiGraphicsPeimInit == 1) {
//    PeiGetSectionFromFv(gTianoLogoGuid, &Buffer, &Size);
      PeiGetSectionFromFv(PeiLogoGuid, &Buffer, &Size);
    }
// AMI_OVERRIDE_END - Reduce post time.
    if (Buffer == NULL) {
      DEBUG((DEBUG_WARN, "Could not locate Logo\n"));
    }
#ifdef FSP_WRAPPER_FLAG
    MemBuffer = (VOID *)AllocatePages (EFI_SIZE_TO_PAGES ((UINTN)Size));
    if ((MemBuffer != NULL) && (Buffer != NULL)) {
      CopyMem (MemBuffer, (VOID *)Buffer, (UINTN)Size);
      GtConfig->LogoPtr = MemBuffer;
      GtConfig->LogoSize = Size;
    } else {
      DEBUG((DEBUG_WARN, "Error in locating / copying LogoPtr.\n"));
      GtConfig->LogoPtr = NULL;
      GtConfig->LogoSize = 0;
    }
#else
    GtConfig->LogoPtr = Buffer;
    GtConfig->LogoSize = Size;
#endif
    DEBUG((DEBUG_INFO, "LogoPtr from PeiGetSectionFromFv is 0x%x\n", GtConfig->LogoPtr));
    DEBUG((DEBUG_INFO, "LogoSize from PeiGetSectionFromFv is 0x%x\n", GtConfig->LogoSize));

#ifndef MINTREE_FLAG
    ///
    /// Initialize the VTD Configuration
    ///
    if (SaSetup.EnableVtd == 0) {
      Vtd->VtdDisable = 1;
    } else {
      Vtd->VtdDisable = 0;
    }
    Vtd->X2ApicOptOut = SaSetup.X2ApicOptOut;

    if (Vtd->VtdDisable == 1) {
      Vtd->BaseAddress[0] = 0;
      Vtd->BaseAddress[1] = 0;
      Vtd->X2ApicOptOut = 1;
    }

    //
    // Initialize Misc SA Configuration
    //
    GmmConfig->GmmEnable = SaSetup.GmmEnable;

    PciePeiConfig->DmiAspm = SaSetup.DmiAspm;
    for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
      PciePeiConfig->PegDeEmphasis[Index] = SaSetup.PegDeEmphasis[Index];
    }
    for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
      PciePeiConfig->PegSlotPowerLimitScale[Index] = SaSetup.PegSlotPowerLimitScale[Index];
      PciePeiConfig->PegSlotPowerLimitValue[Index] = SaSetup.PegSlotPowerLimitValue[Index];
      PciePeiConfig->PegPhysicalSlotNumber[Index] = SaSetup.PegPhysicalSlotNumber[Index];
    }

    MiscPeiConfig->EdramTestMode = SaSetup.EdramTestMode;

    //
    // DPTF might need SA thermal device to be enabled.
    //
    if ((SystemConfiguration.EnableDptf == 1) && (SystemConfiguration.EnableSaDevice == 1)) {
      SaSetup.SaDevice4 = 1;
    }
    MiscPeiConfig->Device4Enable = SaSetup.SaDevice4;
    MiscPeiConfig->CridEnable = SaSetup.CridEnable;
#endif // MINTREE_FLAG
  }

  return EFI_SUCCESS;
}

/**
  PeiGetSectionFromFv finds the file in FV and gets file Address and Size

  @param[in] NameGuid              - File GUID
  @param[out] Address              - Pointer to the File Address
  @param[out] Size                 - Pointer to File Size

  @retval EFI_SUCCESS                Successfull in reading the section from FV
**/
EFI_STATUS
EFIAPI
PeiGetSectionFromFv (
  IN CONST  EFI_GUID        NameGuid,
  OUT VOID                  **Address,
  OUT UINT32                *Size
  )
{
  EFI_STATUS                           Status;
  EFI_PEI_FIRMWARE_VOLUME_PPI          *FvPpi;
  EFI_FV_FILE_INFO                     FvFileInfo;
  PEI_CORE_INSTANCE                    *PrivateData;
  UINTN                                CurrentFv;
  PEI_CORE_FV_HANDLE                   *CoreFvHandle;
  EFI_PEI_FILE_HANDLE                  VbtFileHandle;
  EFI_GUID                             *VbtGuid;
  EFI_COMMON_SECTION_HEADER            *Section;
  CONST EFI_PEI_SERVICES               **PeiServices;

  PeiServices = GetPeiServicesTablePointer();

  PrivateData = PEI_CORE_INSTANCE_FROM_PS_THIS(PeiServices);

  Status = PeiServicesLocatePpi(
             &gEfiFirmwareFileSystem2Guid,
             0,
             NULL,
             (VOID **)&FvPpi
             );
  ASSERT_EFI_ERROR(Status);

  CurrentFv = PrivateData->CurrentPeimFvCount;
  CoreFvHandle = &(PrivateData->Fv[CurrentFv]);

  Status = FvPpi->FindFileByName(FvPpi, &NameGuid, &CoreFvHandle->FvHandle, &VbtFileHandle);
  if (!EFI_ERROR(Status) && VbtFileHandle != NULL) {

    DEBUG((DEBUG_INFO, "Find SectionByType \n"));

    Status = FvPpi->FindSectionByType(FvPpi, EFI_SECTION_RAW, VbtFileHandle, (VOID **)&VbtGuid);
    if (!EFI_ERROR(Status)) {

      DEBUG((DEBUG_INFO, "GetFileInfo \n"));

      Status = FvPpi->GetFileInfo(FvPpi, VbtFileHandle, &FvFileInfo);
      Section = (EFI_COMMON_SECTION_HEADER *)FvFileInfo.Buffer;

      if (IS_SECTION2(Section)) {
        ASSERT(SECTION2_SIZE(Section) > 0x00FFFFFF);
        *Size = SECTION2_SIZE(Section) - sizeof (EFI_COMMON_SECTION_HEADER2);
        *Address = ((UINT8 *)Section + sizeof (EFI_COMMON_SECTION_HEADER2));
      } else {
        *Size = SECTION_SIZE(Section) - sizeof (EFI_COMMON_SECTION_HEADER);
        *Address = ((UINT8 *)Section + sizeof (EFI_COMMON_SECTION_HEADER));
      }
    }
  }

  return EFI_SUCCESS;
}
