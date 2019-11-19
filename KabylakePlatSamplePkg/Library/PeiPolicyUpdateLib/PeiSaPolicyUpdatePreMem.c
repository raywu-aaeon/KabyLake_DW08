/** @file
Do Platform Stage System Agent initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2019 Intel Corporation.

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
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Guid/AcpiVariable.h>    // AdvancedFeaturesContent
#include <Guid/MemoryTypeInformation.h>
#include <Guid/MemoryOverwriteControl.h>
#include <Library/HobLib.h>
#include <Platform.h>
#include <PlatformBoardConfig.h>
#include <Library/BoardConfigLib.h>
#include <PlatformBoardId.h>
#include <Library/CpuMailboxLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiSaPolicyLib.h>
#include <Library/GpioLib.h>
#include <GpioPinsSklH.h>
#ifdef EFI_S3_RESUME
#include <S3MemoryVariable.h>
#endif //EFI_S3_RESUME

#ifndef MINTREE_FLAG
///
/// Memory Reserved should be between 125% to 150% of the Current required memory
/// otherwise BdsMisc.c would do a reset to make it 125% to avoid s4 resume issues.
///
GLOBAL_REMOVE_IF_UNREFERENCED EFI_MEMORY_TYPE_INFORMATION mDefaultMemoryTypeInformation[] = {
  { EfiACPIReclaimMemory,   FixedPcdGet32 (PcdPlatformEfiAcpiReclaimMemorySize) },  // ASL
  { EfiACPIMemoryNVS,       FixedPcdGet32 (PcdPlatformEfiAcpiNvsMemorySize) },      // ACPI NVS (including S3 related)
  { EfiReservedMemoryType,  FixedPcdGet32 (PcdPlatformEfiReservedMemorySize) },     // BIOS Reserved (including S3 related)
  { EfiRuntimeServicesData, FixedPcdGet32 (PcdPlatformEfiRtDataMemorySize) },       // Runtime Service Data
  { EfiRuntimeServicesCode, FixedPcdGet32 (PcdPlatformEfiRtCodeMemorySize) },       // Runtime Service Code
  { EfiMaxMemoryType, 0 }
};
#endif //MINTREE_FLAG
// AMI_OVERRIDE_START - for AMI Chipset Setup.
#include <Library/PeiServicesTablePointerLib.h>
#include <NbSetupData.h>

#define SaSetup         SystemConfiguration
#define CpuSetup        SystemConfiguration
// AMI_OVERRIDE_END - for AMI Chipset Setup.

/**
  UpdatePeiSaPolicyPreMem performs SA PEI Policy initialization

  @param[in out] SiPreMemPolicyPpi - SI_PREMEM_POLICY PPI

  @retval EFI_SUCCESS              The policy is installed and initialized.
**/
EFI_STATUS
UpdatePeiSaPolicyPreMem (
  IN OUT   SI_PREMEM_POLICY_PPI      *SiPreMemPolicyPpi
  )
{
  EFI_STATUS                      Status;
#ifndef MINTREE_FLAG
// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#if 0
  EFI_STATUS                      Status1;
  EFI_STATUS                      Status2;
  EFI_STATUS                      Status3;
#endif  
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
  UINTN                           Lane;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
#if 0
  SETUP_DATA                      SystemConfiguration;
  SA_SETUP                        SaSetup;
  CPU_SETUP                       CpuSetup;
#endif 
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.
  UINTN                           VariableSize;
  SA_MEMORY_RCOMP                 *RcompData;
  WDT_PPI                         *gWdtPei;
  UINT8                           WdtTimeout;
  UINTN                           Bundle;
  UINT32                          RoundedBclkFreq;
  UINT8                           Index;
  UINT8                           *DqByteMap;
  UINT8                           *DqsMapCpu2Dram;
  const UINT16                    *RcompResistor;       // Reference RCOMP resistors on motherboard
  const UINT16                    *RcompTarget;         // RCOMP target values for DqOdt, DqDrv, CmdDrv, CtlDrv, ClkDrv
#ifndef FSP_WRAPPER_FLAG
  VOID                            *MemorySavedData;
#endif //FSP_WRAPPER_FLAG
  VOID                            *SaPegSavedData;
  UINTN                           DataSize;
  EFI_MEMORY_TYPE_INFORMATION     MemoryData[EfiMaxMemoryType + 1];
  EFI_BOOT_MODE                   BootMode;
#ifdef EFI_S3_RESUME
// AMI_OVERRIDE_START - It will be initialization in NBPei.c.
#if 0
  S3_MEMORY_VARIABLE              S3MemVariable;
#endif
// AMI_OVERRIDE_END - It will be initialization in NBPei.c.  
  UINT8                           MorControl;
#endif //EFI_S3_RESUME
  UINT32                          PcodeBclkFrequency;
  UINT32                          MailboxStatus;

  GRAPHICS_PEI_PREMEM_CONFIG      *GtPreMemConfig = NULL;
  MEMORY_CONFIGURATION            *MemConfig = NULL;
  PCIE_PEI_PREMEM_CONFIG          *PciePeiPreMemConfig = NULL;
  SWITCHABLE_GRAPHICS_CONFIG      *SgGpioData = NULL;
  OVERCLOCKING_PREMEM_CONFIG      *OcPreMemConfig = NULL;
  SA_MISC_PEI_PREMEM_CONFIG       *MiscPeiPreMemConfig = NULL;

#endif //MINTREE_FLAG
  MEMORY_CONFIG_NO_CRC            *MemConfigNoCrc = NULL;
  BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig;
// AMI_OVERRIDE_START - for AMI Chipset Setup.
  CONST EFI_PEI_SERVICES           **PeiServices;
  NB_SETUP_DATA                    SystemConfiguration = {0};

  DEBUG ((DEBUG_INFO, "UpdatePeiSaPlatformPolicy() - Start\n"));

  PeiServices = GetPeiServicesTablePointer ();
  GetNbSetupData ((VOID*)PeiServices, &SystemConfiguration, TRUE);
// AMI_OVERRIDE_END - for AMI Chipset Setup.
  DEBUG((DEBUG_INFO, "Entering Get Config Block function call from UpdatePeiSaPolicyPreMem\n"));

#ifndef MINTREE_FLAG
  Status = GetConfigBlock((VOID *) SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *) &MiscPeiPreMemConfig);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock((VOID *) SiPreMemPolicyPpi, &gGraphicsPeiPreMemConfigGuid, (VOID *) &GtPreMemConfig);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock((VOID *) SiPreMemPolicyPpi, &gMemoryConfigGuid, (VOID *) &MemConfig);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock((VOID *) SiPreMemPolicyPpi, &gSaPciePeiPreMemConfigGuid, (VOID *) &PciePeiPreMemConfig);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock((VOID *) SiPreMemPolicyPpi, &gSwitchableGraphicsConfigGuid, (VOID *) &SgGpioData);
  ASSERT_EFI_ERROR(Status);
#endif //MINTREE_FLAG

  Status = GetConfigBlock((VOID *) SiPreMemPolicyPpi, &gMemoryConfigNoCrcGuid, (VOID *) &MemConfigNoCrc);
  ASSERT_EFI_ERROR(Status);

#ifndef MINTREE_FLAG
  Status = GetConfigBlock((VOID *) SiPreMemPolicyPpi, &gSaOverclockingPreMemConfigGuid, (VOID *) &OcPreMemConfig);
  ASSERT_EFI_ERROR(Status);

  RcompData = MemConfigNoCrc->RcompData;

  DqByteMap      = NULL;
  DqsMapCpu2Dram = NULL;
  RcompResistor  = NULL;
  RcompTarget    = NULL;
  WdtTimeout   = FALSE;
#endif //MINTREE_FLAG

#ifndef MINTREE_FLAG
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
  // @todo: WA. Remove once gWdtPpiGuid is supported in FSP wrapper
  //
#ifndef FSP_WRAPPER_FLAG
  ASSERT_EFI_ERROR(Status);
#endif // FSP_WRAPPER_FLAG

#ifndef FSP_WRAPPER_FLAG
  //
  // Initialize S3 Data variable (S3DataPtr)
  //
  VariableSize = 0;
  MemorySavedData = NULL;
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"MemoryConfig",
                               &gMemoryConfigVariableGuid,
                               NULL,
                               &VariableSize,
                               MemorySavedData
                               );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    MemorySavedData = AllocatePool(VariableSize);
    ASSERT(MemorySavedData != NULL);

    DEBUG((DEBUG_INFO, "VariableSize is 0x%x\n", VariableSize));
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"MemoryConfig",
                                 &gMemoryConfigVariableGuid,
                                 NULL,
                                 &VariableSize,
                                 MemorySavedData
                                 );
    ASSERT_EFI_ERROR(Status);
  }

  if (Status == EFI_SUCCESS) {
    MiscPeiPreMemConfig->S3DataPtr = MemorySavedData;
  }
#else
  MiscPeiPreMemConfig->S3DataPtr = NULL;
#endif // FSP_WRAPPER_FLAG

  //
  // Get SA PEG Data variable (PegDataPtr)
  //
  VariableSize = 0;
  SaPegSavedData = NULL;
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"SaPegData",
                               &gPegConfigVariableGuid,
                               NULL,
                               &VariableSize,
                               SaPegSavedData
                               );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    SaPegSavedData = AllocatePool(VariableSize);
    ASSERT(SaPegSavedData != NULL);
    DEBUG((DEBUG_INFO, "VariableSize is 0x%x\n", VariableSize));
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"SaPegData",
                                 &gPegConfigVariableGuid,
                                 NULL,
                                 &VariableSize,
                                 SaPegSavedData
                                 );
    ASSERT_EFI_ERROR(Status);
  }
  if (Status == EFI_SUCCESS) {
    DEBUG((DEBUG_INFO, "Load SaPegData variable: EFI_SUCCESS - VariableSize is 0x%x\n", VariableSize));
    PciePeiPreMemConfig->PegDataPtr = SaPegSavedData;
  }
#ifdef EFI_S3_RESUME
// AMI_OVERRIDE_START - It will be initialization in NBPei.c.
#if 0
  //
  // Get S3 Memory Variable
  //
  VariableSize = sizeof (S3_MEMORY_VARIABLE);
  Status = VariableServices->GetVariable(
                               VariableServices,
                               S3_MEMORY_VARIABLE_NAME,
                               &gS3MemoryVariableGuid,
                               NULL,
                               &VariableSize,
                               &S3MemVariable
                               );
  if (Status == EFI_SUCCESS) {
    MiscPeiPreMemConfig->AcpiReservedMemoryBase = S3MemVariable.AcpiReservedMemoryBase;
    MiscPeiPreMemConfig->AcpiReservedMemorySize = S3MemVariable.AcpiReservedMemorySize;
    MiscPeiPreMemConfig->SystemMemoryLength     = S3MemVariable.SystemMemoryLength;
  }
#endif 
// AMI_OVERRIDE_END - It will be initialization in NBPei.c.

  VariableSize = sizeof (MorControl);
  Status = VariableServices->GetVariable(
                               VariableServices,
                               MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                               &gEfiMemoryOverwriteControlDataGuid,
                               NULL,
                               &VariableSize,
                               &MorControl
                               );
  if (EFI_ERROR(Status)) {
    MorControl = 0;
  }
#endif //EFI_S3_RESUME
// AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.  
#if 0
  //
  // Get System configuration variables
  //
  VariableSize = sizeof (SETUP_DATA);
  Status1 = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );

  VariableSize = sizeof (SA_SETUP);
  Status2 = VariableServices->GetVariable (
                                VariableServices,
                                L"SaSetup",
                                &gSaSetupVariableGuid,
                                NULL,
                                &VariableSize,
                                &SaSetup
                                );

  VariableSize = sizeof (CPU_SETUP);
  Status3 = VariableServices->GetVariable (
                                VariableServices,
                                L"CpuSetup",
                                &gCpuSetupVariableGuid,
                                NULL,
                                &VariableSize,
                                &CpuSetup
                                );
#endif 
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token.  

  //
  // IED must be enabled when Probeless Trace enabled
  //
  if ((SaSetup.IedMemEnable == 1) || (SaSetup.ProbelessTrace != 0)) {
    MiscPeiPreMemConfig->IedSize = 0x400000;
  }
#endif //MINTREE_FLAG

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPreMemGuid, (VOID **) &PlatformBoardConfig);
  if (!EFI_ERROR(Status)) {
#ifndef MINTREE_FLAG
    MemConfig->MobilePlatform = PlatformBoardConfig->MobilePlatform;
    MemConfig->CaVrefConfig   = PlatformBoardConfig->CaVrefConfig;

    //
    // Spd Address Table configuration
    //
    MiscPeiPreMemConfig->SpdAddressTable[0] = PlatformBoardConfig->SpdAddressTable[0];
    MiscPeiPreMemConfig->SpdAddressTable[1] = PlatformBoardConfig->SpdAddressTable[1];
    MiscPeiPreMemConfig->SpdAddressTable[2] = PlatformBoardConfig->SpdAddressTable[2];
    MiscPeiPreMemConfig->SpdAddressTable[3] = PlatformBoardConfig->SpdAddressTable[3];

    if (PlatformBoardConfig->Enable.RcompResistor) {
      CopyMem((VOID *)RcompData->RcompResistor, PlatformBoardConfig->RcompResistor, sizeof(RcompData->RcompResistor));
    }
    if (PlatformBoardConfig->Enable.RcompTarget) {
      CopyMem((VOID *)RcompData->RcompTarget, PlatformBoardConfig->RcompTarget, sizeof(RcompData->RcompTarget));
    }
    if (PlatformBoardConfig->Enable.DqByteMap) {
      CopyMem((VOID *)MemConfigNoCrc->DqByteMap, PlatformBoardConfig->DqByteMap, sizeof(UINT8)* SA_MC_MAX_CHANNELS * SA_MRC_ITERATION_MAX * 2);
    }
    if (PlatformBoardConfig->Enable.DqsMapCpu2Dram) {
      CopyMem((VOID *)MemConfigNoCrc->DqsMap, PlatformBoardConfig->DqsMapCpu2Dram, sizeof(UINT8)* SA_MC_MAX_CHANNELS * SA_MC_MAX_BYTES_NO_ECC);
    }
    if (PlatformBoardConfig->Enable.DqPinsInterleaved) {
      MemConfig->DqPinsInterleaved = PlatformBoardConfig->DqPinsInterleaved;
    }
#endif //MINTREE_FLAG
    //
    // Spd data configuration
    //
    if (PlatformBoardConfig->Enable.SpdData) {
      CopyMem((VOID *)MemConfigNoCrc->SpdData->SpdData[0][0], PlatformBoardConfig->SpdData, sizeof(PlatformBoardConfig->SpdData));
      CopyMem((VOID *)MemConfigNoCrc->SpdData->SpdData[1][0], PlatformBoardConfig->SpdData, sizeof(PlatformBoardConfig->SpdData));
    }

#ifndef MINTREE_FLAG
    if (PlatformBoardConfig->Enable.GpioSupport) {
      PciePeiPreMemConfig->PegGpioData.GpioSupport = PlatformBoardConfig->GpioSupport;
    } else {
      PciePeiPreMemConfig->PegGpioData.GpioSupport = (BOOLEAN)SaSetup.PegGen3AllowGpioReset;
    }
#ifdef SG_SUPPORT
    //
    // SaRtd3Pcie GPIO configuration
    //
    if (SaSetup.PcieCardSelect != 2) {
      SgGpioData->RootPortDev = PlatformBoardConfig->RootPortDev;
      SgGpioData->RootPortFun = PlatformBoardConfig->RootPortFun;

      SgGpioData->SaRtd3Pcie0Gpio.GpioSupport = PlatformBoardConfig->SaRtd3Pcie0Gpio.GpioSupport;
      SgGpioData->SaRtd3Pcie0Gpio.WakeGpioNo = PlatformBoardConfig->SaRtd3Pcie0Gpio.WakeGpioNo;
      SgGpioData->SaRtd3Pcie0Gpio.HoldRst.ExpanderNo = PlatformBoardConfig->SaRtd3Pcie0Gpio.HoldRstExpanderNo;
      SgGpioData->SaRtd3Pcie0Gpio.HoldRst.GpioNo = PlatformBoardConfig->SaRtd3Pcie0Gpio.HoldRstGpioNo;
      SgGpioData->SaRtd3Pcie0Gpio.HoldRst.Active = PlatformBoardConfig->SaRtd3Pcie0Gpio.HoldRstActive;
      SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.ExpanderNo = PlatformBoardConfig->SaRtd3Pcie0Gpio.PwrEnableExpanderNo;
      SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.GpioNo = PlatformBoardConfig->SaRtd3Pcie0Gpio.PwrEnableGpioNo;
      SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.Active = PlatformBoardConfig->SaRtd3Pcie0Gpio.PwrEnableActive;
      SgGpioData->SaRtd3Pcie0ClkRecNumber = PlatformBoardConfig->SaRtd3Pcie0ClkRecNumber;

      SgGpioData->SaRtd3Pcie1Gpio.GpioSupport = PlatformBoardConfig->SaRtd3Pcie1Gpio.GpioSupport;
      SgGpioData->SaRtd3Pcie1Gpio.WakeGpioNo = PlatformBoardConfig->SaRtd3Pcie1Gpio.WakeGpioNo;
      SgGpioData->SaRtd3Pcie1Gpio.HoldRst.ExpanderNo = PlatformBoardConfig->SaRtd3Pcie1Gpio.HoldRstExpanderNo;
      SgGpioData->SaRtd3Pcie1Gpio.HoldRst.GpioNo = PlatformBoardConfig->SaRtd3Pcie1Gpio.HoldRstGpioNo;
      SgGpioData->SaRtd3Pcie1Gpio.HoldRst.Active = PlatformBoardConfig->SaRtd3Pcie1Gpio.HoldRstActive;
      SgGpioData->SaRtd3Pcie1Gpio.PwrEnable.ExpanderNo = PlatformBoardConfig->SaRtd3Pcie1Gpio.PwrEnableExpanderNo;
      SgGpioData->SaRtd3Pcie1Gpio.PwrEnable.GpioNo = PlatformBoardConfig->SaRtd3Pcie1Gpio.PwrEnableGpioNo;
      SgGpioData->SaRtd3Pcie1Gpio.PwrEnable.Active = PlatformBoardConfig->SaRtd3Pcie1Gpio.PwrEnableActive;
      SgGpioData->SaRtd3Pcie1ClkRecNumber = PlatformBoardConfig->SaRtd3Pcie1ClkRecNumber;

      SgGpioData->SaRtd3Pcie2Gpio.GpioSupport = PlatformBoardConfig->SaRtd3Pcie2Gpio.GpioSupport;
      SgGpioData->SaRtd3Pcie2Gpio.WakeGpioNo = PlatformBoardConfig->SaRtd3Pcie2Gpio.WakeGpioNo;
      SgGpioData->SaRtd3Pcie2Gpio.HoldRst.ExpanderNo = PlatformBoardConfig->SaRtd3Pcie2Gpio.HoldRstExpanderNo;
      SgGpioData->SaRtd3Pcie2Gpio.HoldRst.GpioNo = PlatformBoardConfig->SaRtd3Pcie2Gpio.HoldRstGpioNo;
      SgGpioData->SaRtd3Pcie2Gpio.HoldRst.Active = PlatformBoardConfig->SaRtd3Pcie2Gpio.HoldRstActive;
      SgGpioData->SaRtd3Pcie2Gpio.PwrEnable.ExpanderNo = PlatformBoardConfig->SaRtd3Pcie2Gpio.PwrEnableExpanderNo;
      SgGpioData->SaRtd3Pcie2Gpio.PwrEnable.GpioNo = PlatformBoardConfig->SaRtd3Pcie2Gpio.PwrEnableGpioNo;
      SgGpioData->SaRtd3Pcie2Gpio.PwrEnable.Active = PlatformBoardConfig->SaRtd3Pcie2Gpio.PwrEnableActive;
      SgGpioData->SaRtd3Pcie2ClkRecNumber = PlatformBoardConfig->SaRtd3Pcie2ClkRecNumber;
    }
#endif // SG_SUPPORT
#endif //MINTREE_FLAG
  }

#ifndef MINTREE_FLAG
// AMI_OVERRIDE_START >>
#if 0
  if (!EFI_ERROR(Status2)) {
#else
  if (1) {
#endif   
    //
    // Get the Platform Configuration from SetupData
    //
    MemConfig->CkeRankMapping = 0xAA;
    //
    // Initialize the Graphics configuration
    //
    GtPreMemConfig->IgdDvmt50PreAlloc = SaSetup.IgdDvmt50PreAlloc;
    GtPreMemConfig->InternalGraphics  = SaSetup.InternalGraphics;
    GtPreMemConfig->PrimaryDisplay    = SaSetup.PrimaryDisplay;
    GtPreMemConfig->ApertureSize      = SaSetup.ApertureSize;
    GtPreMemConfig->GttSize           = SaSetup.GTTSize;

    PciePeiPreMemConfig->DmiDeEmphasis            = SaSetup.DmiDeEmphasis;
    PciePeiPreMemConfig->Peg0PowerDownUnusedLanes = SaSetup.Peg0PowerDownUnusedLanes;
    PciePeiPreMemConfig->Peg1PowerDownUnusedLanes = SaSetup.Peg1PowerDownUnusedLanes;
    PciePeiPreMemConfig->Peg2PowerDownUnusedLanes = SaSetup.Peg2PowerDownUnusedLanes;
    PciePeiPreMemConfig->InitPcieAspmAfterOprom   = SaSetup.InitAspmAfterOprom;

    PciePeiPreMemConfig->DmiGen3ProgramStaticEq   = SaSetup.DmiGen3ProgramStaticEq;
    PciePeiPreMemConfig->Peg0Enable               = SaSetup.Peg0Enable;
    PciePeiPreMemConfig->Peg1Enable               = SaSetup.Peg1Enable;
    PciePeiPreMemConfig->Peg2Enable               = SaSetup.Peg2Enable;
    PciePeiPreMemConfig->Peg0MaxLinkSpeed         = SaSetup.Peg0MaxLinkSpeed;
    PciePeiPreMemConfig->Peg1MaxLinkSpeed         = SaSetup.Peg1MaxLinkSpeed;
    PciePeiPreMemConfig->Peg2MaxLinkSpeed         = SaSetup.Peg2MaxLinkSpeed;

    PciePeiPreMemConfig->Peg0MaxLinkWidth         = SaSetup.Peg0MaxLinkWidth;
    PciePeiPreMemConfig->Peg1MaxLinkWidth         = SaSetup.Peg1MaxLinkWidth;
    PciePeiPreMemConfig->Peg2MaxLinkWidth         = SaSetup.Peg2MaxLinkWidth;

    PciePeiPreMemConfig->PegGen3ProgramStaticEq   = SaSetup.PegGen3ProgramStaticEq;

    //
    //
    for (Lane = 0; Lane < SA_DMI_MAX_LANE; Lane++) {
      PciePeiPreMemConfig->DmiGen3RootPortPreset[Lane] = SaSetup.DmiGen3RootPortPreset[Lane];
      PciePeiPreMemConfig->DmiGen3EndPointPreset[Lane] = SaSetup.DmiGen3EndPointPreset[Lane];
      PciePeiPreMemConfig->DmiGen3EndPointHint[Lane]   = SaSetup.DmiGen3EndPointHint[Lane];
    }
// AMI_OVERRIDE_START - EIP244373 : Static code analysis issues found in AptioV SkylakePlatPkg module.
#if 0
    for (Bundle = 0; Bundle < SA_DMI_MAX_BUNDLE; Bundle++) {
      PciePeiPreMemConfig->DmiGen3RxCtlePeaking[Bundle] = SaSetup.DmiGen3RxCtlePeaking[Bundle];
    }
    PciePeiPreMemConfig->PegGen3RxCtleOverride           = SaSetup.PegGen3RxCtleOverride;
    for (Lane = 0; Lane < SA_PEG_MAX_LANE; Lane++) {
      PciePeiPreMemConfig->PegGen3RootPortPreset[Lane]   = SaSetup.PegGen3RootPortPreset[Lane];
      PciePeiPreMemConfig->PegGen3EndPointPreset[Lane]   = SaSetup.PegGen3EndPointPreset[Lane];
      PciePeiPreMemConfig->PegGen3EndPointHint[Lane]     = SaSetup.PegGen3EndPointHint[Lane];
    }
    for (Bundle = 0; Bundle < SA_PEG_MAX_BUNDLE; Bundle++) {
      PciePeiPreMemConfig->PegGen3RxCtlePeaking[Bundle] = SaSetup.PegGen3RxCtlePeaking[Bundle];
    }
#else
      for (Bundle = 0; Bundle < SA_PEG_MAX_BUNDLE; Bundle++) {      
        PciePeiPreMemConfig->DmiGen3RxCtlePeaking[Bundle] = SaSetup.DmiGen3RxCtlePeaking[Bundle];
        PciePeiPreMemConfig->PegGen3RxCtlePeaking[Bundle] = SaSetup.PegGen3RxCtlePeaking[Bundle];
      }
      PciePeiPreMemConfig->PegGen3RxCtleOverride           = SaSetup.PegGen3RxCtleOverride;
      for (Lane = 0; Lane < SA_PEG_MAX_LANE; Lane++) {
        PciePeiPreMemConfig->PegGen3RootPortPreset[Lane]   = SaSetup.PegGen3RootPortPreset[Lane];
        PciePeiPreMemConfig->PegGen3EndPointPreset[Lane]   = SaSetup.PegGen3EndPointPreset[Lane];
        PciePeiPreMemConfig->PegGen3EndPointHint[Lane]     = SaSetup.PegGen3EndPointHint[Lane];
      }
#endif
// AMI_OVERRIDE_END - EIP244373 : Static code analysis issues found in AptioV SkylakePlatPkg module.

    //
    // External Graphics card scan option.
    //
// AMI_OVERRIDE_START
#if 0
    if (!EFI_ERROR(Status1)) {
#else
    if (1) {
#endif
// AMI_OVERRIDE_END	
// AMI_OVERRIDE_START - CSM_SUPPORT turn off will build errors.
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)        
// AMI_OVERRIDE_START - Use AMI CSM module setup data value.
#if 0
      if (SystemConfiguration.CsmControl == EFI_CSM_CONTROL_DETECT_LEGACY_VGA) {
#else
      if (SaSetup.VideoOpRom == CSMSETUP_LEGACY_ONLY_OPROMS) {
#endif
// AMI_OVERRIDE_END - Use AMI CSM module setup data value.
        MiscPeiPreMemConfig->ScanExtGfxForLegacyOpRom = 1;
        MiscPeiPreMemConfig->SkipExtGfxScan = 0;
      } else {
#endif	      
// AMI_OVERRIDE_END - CSM_SUPPORT turn off will build errors.
        MiscPeiPreMemConfig->ScanExtGfxForLegacyOpRom = 0;
        MiscPeiPreMemConfig->SkipExtGfxScan = SaSetup.SkipExtGfxScan;
// AMI_OVERRIDE_START - CSM_SUPPORT turn off will build errors.		
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1) 
      }
#endif	
// AMI_OVERRIDE_END - CSM_SUPPORT turn off will build errors.
    }
#ifdef SG_SUPPORT
    //
    // Initialize the Switchable Graphics Configuration
    //
    if (SaSetup.PcieCardSelect == 0) {
      ///
      /// For Elk-Creek card, invert the Power enable signal
      ///
      SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.Active = !(SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.Active);
// AMI_OVERRIDE_START - Update all of Pcie Gpio configuration.
        SgGpioData->SaRtd3Pcie1Gpio.PwrEnable.Active = !(SgGpioData->SaRtd3Pcie1Gpio.PwrEnable.Active);
        SgGpioData->SaRtd3Pcie2Gpio.PwrEnable.Active = !(SgGpioData->SaRtd3Pcie2Gpio.PwrEnable.Active);  
// AMI_OVERRIDE_END - Update all of Pcie Gpio configuration.
    }

#ifndef FSP_WRAPPER_FLAG
    ///
    /// Switchable Graphics mode set as MUXLESS (By default)
    ///

    ///
    /// In Switchable Gfx mode PEG needs to be always enabled
    /// and IGFX must be set as Primary Display.
    ///
    if (SaSetup.PrimaryDisplay == 4) {
      MiscPeiPreMemConfig->SgMode = 2; //SgModeMuxless
      MiscPeiPreMemConfig->SgSubSystemId = 0x2112;
      GtPreMemConfig->PrimaryDisplay = 0;
    } else if ((SaSetup.PrimaryDisplay == 1) || (SaSetup.PrimaryDisplay == 2) || (SaSetup.PrimaryDisplay == 3)) {
      ///
      /// In PEG or PCI or Auto mode set Switchable Gfx mode as dGPU
      ///
      MiscPeiPreMemConfig->SgMode = 3; //SgModeDgpu
      MiscPeiPreMemConfig->SgSubSystemId = 0x2212;
    } else if (SaSetup.PrimaryDisplay == 0) {
      ///
      /// In IGFX only mode mode set Switchable Gfx mode as Disabled
      ///
      MiscPeiPreMemConfig->SgMode = 0; //SgModeDisabled
      MiscPeiPreMemConfig->SgSubSystemId = 0x2212;
    }
#endif // FSP_WRAPPER_FLAG
    MiscPeiPreMemConfig->SgDelayAfterPwrEn = SaSetup.DelayAfterPwrEn;
    MiscPeiPreMemConfig->SgDelayAfterHoldReset = SaSetup.DelayAfterHoldReset;
//
// KblGBegin
//
    MiscPeiPreMemConfig->SgDelayAfterOffMethod = SaSetup.DelayAfterOffMethod;
    MiscPeiPreMemConfig->SgDelayAfterLinkEnable = SaSetup.DelayAfterLinkEnable;
    MiscPeiPreMemConfig->SgGenSpeedChangeEnable = SaSetup.GenSpeedChangeEnable;
//
// KblGEnd
//
// AMI_OVERRIDE_START - Support ATI detachable graphics.
#if defined(SGOEMSSDT_ATI_SUPPORT) && SGOEMSSDT_ATI_SUPPORT == 1  
  #if defined(TBT_DETACHABLE_GFX_SUPPORT) && TBT_DETACHABLE_GFX_SUPPORT == 1
  #else    
      GtPreMemConfig->PrimaryDisplay = 0;
      PciePeiPreMemConfig->Peg0Enable = 1;  
  #endif
#endif
// AMI_OVERRIDE_END - Support ATI detachable graphics.
#endif // SG_SUPPORT

    MemConfig->HobBufferSize = SaSetup.HobBufferSize;
    MemConfig->EccSupport    = SaSetup.EccSupport;
    MemConfig->SaGv          = SaSetup.SaGv;
    MemConfig->FreqSaGvLow   = SaSetup.FreqSaGvLow;
    MemConfig->EpgEnable     = SaSetup.EpgEnable;
    MemConfig->Idd3n         = SaSetup.Idd3n;
    MemConfig->Idd3p         = SaSetup.Idd3p;

    MemConfig->SpdProfileSelected = (CpuSetup.OverclockingSupport == 0) ? 0 : SaSetup.SpdProfileSelected;

    //
    // If user custom profile is selected, we will send the setup values to the MRC.
    // The setup values will be the current memory settings plus user override values.
    // If any other profile is selected or a WDT timeout has occured, we zero out
    // the settings just to be safe.
    if (gWdtPei != NULL) {
      WdtTimeout = gWdtPei->CheckStatus();
    } else {
      WdtTimeout = FALSE;
    }

    if ((SaSetup.SpdProfileSelected == UserDefined) && (WdtTimeout == FALSE)) {
      //
      // If USER custom profile is selected, we will start the WDT.
      //
      if (gWdtPei != NULL) {
        Status = gWdtPei->ReloadAndStart(WDT_TIMEOUT);
      }

      MemConfig->VddVoltage = SaSetup.MemoryVoltage;    // Vddq in [mV], 0 = platform default.

      //
      // Read DDR RefClk setting selected in Bios setup. Zero means AUTO.
      // If not AUTO, set to 0 for 133MHz and 1 for 100MHz.
      //
      if (SaSetup.DdrRefClk > 0) {
        MemConfig->RefClk = SaSetup.DdrRefClk - 1;
        MemConfig->Ratio = SaSetup.DdrRatio;
      } else {
        MemConfig->Ratio = 0;
      }
      MemConfig->OddRatioMode = SaSetup.DdrOddRatioMode;

      MemConfig->tCL = SaSetup.tCL;
      MemConfig->tCWL = SaSetup.tCWL;
      MemConfig->tFAW = SaSetup.tFAW;
      MemConfig->tRAS = SaSetup.tRAS;
      MemConfig->tRCDtRP = SaSetup.tRCDtRP;
      MemConfig->tREFI = SaSetup.tREFI;
      MemConfig->tRFC = SaSetup.tRFC;
      MemConfig->tRRD = SaSetup.tRRD;
      MemConfig->tRTP = SaSetup.tRTP;
      MemConfig->tWR = SaSetup.tWR;
      MemConfig->tWTR = SaSetup.tWTR;
    } else {
      MemConfig->VddVoltage = 0;    // Use platform default as the safe value.
      MemConfig->Ratio = 0;
      MemConfig->OddRatioMode = 0;
      MemConfig->tCL = 0;
      MemConfig->tCWL = 0;
      MemConfig->tFAW = 0;
      MemConfig->tRAS = 0;
      MemConfig->tRCDtRP = 0;
      MemConfig->tREFI = 0;
      MemConfig->tRFC = 0;
      MemConfig->tRRD = 0;
      MemConfig->tRTP = 0;
      MemConfig->tWR = 0;
      MemConfig->tWTR = 0;
    }
    MemConfig->NModeSupport = SaSetup.NModeSupport;
    MemConfig->RemapEnable = SaSetup.RemapEnable;
    MemConfig->MrcTimeMeasure = SaSetup.MrcTimeMeasure;
    MemConfig->MrcFastBoot = SaSetup.MrcFastBoot;
    MemConfig->LpddrMemWriteLatencySet = SaSetup.LpddrMemWriteLatencySet;
    MemConfig->EvLoader = SaSetup.EvLoader;
    MemConfig->EvLoaderDelay = SaSetup.EvLoaderDelay;

    //
    // Get the Memory Thermal Management configuration from the SetupData
    //
    MemConfig->ThermalManagement = SaSetup.MemoryThermalManagement;
    MemConfig->PeciInjectedTemp = SaSetup.PeciInjectedTemp;
    MemConfig->ExttsViaTsOnBoard = SaSetup.ExttsViaTsOnBoard;
    MemConfig->ExttsViaTsOnDimm = SaSetup.ExttsViaTsOnDimm;
    MemConfig->VirtualTempSensor = SaSetup.VirtualTempSensor;

    MemConfig->ScramblerSupport = SaSetup.ScramblerSupport;
    MemConfig->EnBER = SaSetup.EnBER;

#if 0
    MemConfigNonCrc->SerialDebugLevel = SaSetup.SerialDebug;
#else
    {
      UINT32 CurrentDebugPrintErrorLevel;
      UINT8  ControlLevel;

      CurrentDebugPrintErrorLevel = GetDebugPrintErrorLevel();

      switch (CurrentDebugPrintErrorLevel) {
      case 0:
        ControlLevel = 0;
        break;
      case DEBUG_ERROR:
        ControlLevel = 1;
        break;
      case DEBUG_ERROR | DEBUG_WARN:
        ControlLevel = 2;
        break;
      default:  // DEBUG_ERROR|DEBUG_WARN|DEBUG_INFO|DEBUG_LOAD
        ControlLevel = 3;
      }
      MemConfigNoCrc->SerialDebugLevel = ControlLevel;
    }
#endif
    MemConfig->McLock = SaSetup.McLock;
    MemConfig->ProbelessTrace = SaSetup.ProbelessTrace;
    MemConfig->GdxcIotSize = SaSetup.GdxcIotSize;
    MemConfig->GdxcMotSize = SaSetup.GdxcMotSize;
    MemConfig->MemoryTrace = SaSetup.MemoryTrace;
    MemConfig->ECT = SaSetup.ECT;
    MemConfig->SOT = SaSetup.SOT;
    MemConfig->ERDMPRTC2D = SaSetup.ERDMPRTC2D;
    MemConfig->RDMPRT = SaSetup.RDMPRT;
    MemConfig->RCVET = SaSetup.RCVET;
    MemConfig->JWRL = SaSetup.JWRL;
    MemConfig->EWRTC2D = SaSetup.EWRTC2D;
    MemConfig->ERDTC2D = SaSetup.ERDTC2D;
    MemConfig->WRTC1D = SaSetup.WRTC1D;
    MemConfig->WRVC1D = SaSetup.WRVC1D;
    MemConfig->RDTC1D = SaSetup.RDTC1D;
    MemConfig->DIMMODTT = SaSetup.DIMMODTT;
    MemConfig->DIMMRONT = SaSetup.DIMMRONT;
    MemConfig->WRSRT = SaSetup.WRSRT;
    MemConfig->RDODTT = SaSetup.RDODTT;
    MemConfig->RDEQT = SaSetup.RDEQT;
    MemConfig->RDAPT = SaSetup.RDAPT;
    MemConfig->WRTC2D = SaSetup.WRTC2D;
    MemConfig->RDTC2D = SaSetup.RDTC2D;
    MemConfig->WRVC2D = SaSetup.WRVC2D;
    MemConfig->RDVC2D = SaSetup.RDVC2D;
    MemConfig->CMDVC = SaSetup.CMDVC;
    MemConfig->LCT = SaSetup.LCT;
    MemConfig->RTL = SaSetup.RTL;
    MemConfig->TAT = SaSetup.TAT;
    MemConfig->RCVENC1D = SaSetup.RCVENC1D;
    MemConfig->RMT = SaSetup.RMT;
    MemConfig->MEMTST = SaSetup.MEMTST;
    MemConfig->ALIASCHK = SaSetup.ALIASCHK;
    MemConfig->RMC = SaSetup.RMC;
    MemConfig->WRDSUDT = SaSetup.WRDSUDT;
    MemConfig->CMDSR   = SaSetup.CMDSR;
    MemConfig->CMDDSEQ = SaSetup.CMDDSEQ;
    MemConfig->CMDNORM = SaSetup.CMDNORM;
    MemConfig->EWRDSEQ = SaSetup.EWRDSEQ;

    //
    // TurnAround Timing
    // Read-to-Read
    //
    MemConfig->tRd2RdSG = SaSetup.tRd2RdSG;
    MemConfig->tRd2RdDG = SaSetup.tRd2RdDG;
    MemConfig->tRd2RdDR = SaSetup.tRd2RdDR;
    MemConfig->tRd2RdDD = SaSetup.tRd2RdDD;
    //
    // Write-to-Read
    //
    MemConfig->tWr2RdSG = SaSetup.tWr2RdSG;
    MemConfig->tWr2RdDG = SaSetup.tWr2RdDG;
    MemConfig->tWr2RdDR = SaSetup.tWr2RdDR;
    MemConfig->tWr2RdDD = SaSetup.tWr2RdDD;
    //
    // Write-to-Write
    //
    MemConfig->tWr2WrSG = SaSetup.tWr2WrSG;
    MemConfig->tWr2WrDG = SaSetup.tWr2WrDG;
    MemConfig->tWr2WrDR = SaSetup.tWr2WrDR;
    MemConfig->tWr2WrDD = SaSetup.tWr2WrDD;
    //
    // Read-to-Write
    //
    MemConfig->tRd2WrSG = SaSetup.tRd2WrSG;
    MemConfig->tRd2WrDG = SaSetup.tRd2WrDG;
    MemConfig->tRd2WrDR = SaSetup.tRd2WrDR;
    MemConfig->tRd2WrDD = SaSetup.tRd2WrDD;


    MemConfig->MrcSafeConfig = SaSetup.MrcSafeConfig;
    MemConfig->ExitOnFailure = SaSetup.ExitOnFailure;
    MemConfig->RhPrevention = SaSetup.RhPrevention;
    MemConfig->RhSolution = SaSetup.RhSolution;
    MemConfig->RhActProbability = SaSetup.RhActProbability;
    MemConfig->ChHashEnable = SaSetup.ChHashEnable;
    if (SaSetup.ChHashMask != 0) {
      // 0 = Platform Default (AUTO)
      MemConfig->ChHashMask = SaSetup.ChHashMask;
    }
    MemConfig->ChHashInterleaveBit = SaSetup.ChHashInterleaveBit;
    MemConfig->Vc1ReadMeter = SaSetup.Vc1ReadMeter;
    MemConfig->Vc1ReadMeterTimeWindow = SaSetup.Vc1ReadMeterTimeWindow;
    MemConfig->Vc1ReadMeterThreshold = SaSetup.Vc1ReadMeterThreshold;
    MemConfig->StrongWkLeaker = SaSetup.StrongWkLeaker;
    MemConfig->ForceSingleRank = SaSetup.ForceSingleRank;

#ifdef UP_SERVER_FLAG
    MemConfig->UserThresholdEnable = SaSetup.UserThresholdEnable;
    MemConfig->UserBudgetEnable = SaSetup.UserBudgetEnable;
    MemConfig->TsodTcritMax = SaSetup.TsodTcritMax;
    MemConfig->TsodEventMode = SaSetup.TsodEventMode;
    MemConfig->TsodEventPolarity = SaSetup.TsodEventPolarity;
    MemConfig->TsodCriticalEventOnly = SaSetup.TsodCriticalEventOnly;
    MemConfig->TsodEventOutputControl = SaSetup.TsodEventOutputControl;
    MemConfig->TsodAlarmwindowLockBit = SaSetup.TsodAlarmwindowLockBit;
    MemConfig->TsodCriticaltripLockBit = SaSetup.TsodCriticaltripLockBit;
    MemConfig->TsodShutdownMode = SaSetup.TsodShutdownMode;
    MemConfig->TsodThigMax = SaSetup.TsodThigMax;
    MemConfig->TsodManualEnable = SaSetup.TsodManualEnable;
#endif

    //
    // Thermal Options
    //
    MemConfig->EnableExtts = SaSetup.EnableExtts;
    MemConfig->EnableCltm = SaSetup.EnableCltm;
    MemConfig->EnableOltm = SaSetup.EnableOltm;
    MemConfig->Ddr4SkipRefreshEn = SaSetup.Ddr4SkipRefreshEn;
    MemConfig->EnablePwrDn = SaSetup.EnablePwrDn;
    MemConfig->EnablePwrDnLpddr = SaSetup.EnablePwrDnLpddr;
    MemConfig->Refresh2X = SaSetup.Refresh2X;
    MemConfig->DdrThermalSensor = SaSetup.DdrThermalSensor;
    MemConfig->UserPowerWeightsEn = SaSetup.UserPowerWeightsEn;

    MemConfig->EnergyScaleFact = SaSetup.EnergyScaleFact;
    MemConfig->RaplPwrFlCh1 = SaSetup.RaplPwrFlCh1;
    MemConfig->RaplPwrFlCh0 = SaSetup.RaplPwrFlCh0;

    MemConfig->RaplLim2Lock = SaSetup.RaplLim2Lock;
    MemConfig->RaplLim2WindX = SaSetup.RaplLim2WindX;
    MemConfig->RaplLim2WindY = SaSetup.RaplLim2WindY;
    MemConfig->RaplLim2Ena = SaSetup.RaplLim2Ena;
    MemConfig->RaplLim2Pwr = SaSetup.RaplLim2Pwr;
    MemConfig->RaplLim1WindX = SaSetup.RaplLim1WindX;
    MemConfig->RaplLim1WindY = SaSetup.RaplLim1WindY;
    MemConfig->RaplLim1Ena = SaSetup.RaplLim1Ena;
    MemConfig->RaplLim1Pwr = SaSetup.RaplLim1Pwr;

    MemConfig->WarmThresholdCh0Dimm0 = SaSetup.WarmThresholdCh0Dimm0;
    MemConfig->WarmThresholdCh0Dimm1 = SaSetup.WarmThresholdCh0Dimm1;
    MemConfig->WarmThresholdCh1Dimm0 = SaSetup.WarmThresholdCh1Dimm0;
    MemConfig->WarmThresholdCh1Dimm1 = SaSetup.WarmThresholdCh1Dimm1;
    MemConfig->HotThresholdCh0Dimm0 = SaSetup.HotThresholdCh0Dimm0;
    MemConfig->HotThresholdCh0Dimm1 = SaSetup.HotThresholdCh0Dimm1;
    MemConfig->HotThresholdCh1Dimm0 = SaSetup.HotThresholdCh1Dimm0;
    MemConfig->HotThresholdCh1Dimm1 = SaSetup.HotThresholdCh1Dimm1;
    MemConfig->WarmBudgetCh0Dimm0 = SaSetup.WarmBudgetCh0Dimm0;
    MemConfig->WarmBudgetCh0Dimm1 = SaSetup.WarmBudgetCh0Dimm1;
    MemConfig->WarmBudgetCh1Dimm0 = SaSetup.WarmBudgetCh1Dimm0;
    MemConfig->WarmBudgetCh1Dimm1 = SaSetup.WarmBudgetCh1Dimm1;
    MemConfig->HotBudgetCh0Dimm0 = SaSetup.HotBudgetCh0Dimm0;
    MemConfig->HotBudgetCh0Dimm1 = SaSetup.HotBudgetCh0Dimm1;
    MemConfig->HotBudgetCh1Dimm0 = SaSetup.HotBudgetCh1Dimm0;
    MemConfig->HotBudgetCh1Dimm1 = SaSetup.HotBudgetCh1Dimm1;

    MemConfig->IdleEnergyCh0Dimm1 = SaSetup.IdleEnergyCh0Dimm1;
    MemConfig->IdleEnergyCh0Dimm0 = SaSetup.IdleEnergyCh0Dimm0;
    MemConfig->PdEnergyCh0Dimm1 = SaSetup.PdEnergyCh0Dimm1;
    MemConfig->PdEnergyCh0Dimm0 = SaSetup.PdEnergyCh0Dimm0;
    MemConfig->ActEnergyCh0Dimm1 = SaSetup.ActEnergyCh0Dimm1;
    MemConfig->ActEnergyCh0Dimm0 = SaSetup.ActEnergyCh0Dimm0;
    MemConfig->RdEnergyCh0Dimm1 = SaSetup.RdEnergyCh0Dimm1;
    MemConfig->RdEnergyCh0Dimm0 = SaSetup.RdEnergyCh0Dimm0;
    MemConfig->WrEnergyCh0Dimm1 = SaSetup.WrEnergyCh0Dimm1;
    MemConfig->WrEnergyCh0Dimm0 = SaSetup.WrEnergyCh0Dimm0;

    MemConfig->IdleEnergyCh1Dimm1 = SaSetup.IdleEnergyCh1Dimm1;
    MemConfig->IdleEnergyCh1Dimm0 = SaSetup.IdleEnergyCh1Dimm0;
    MemConfig->PdEnergyCh1Dimm1 = SaSetup.PdEnergyCh1Dimm1;
    MemConfig->PdEnergyCh1Dimm0 = SaSetup.PdEnergyCh1Dimm0;
    MemConfig->ActEnergyCh1Dimm1 = SaSetup.ActEnergyCh1Dimm1;
    MemConfig->ActEnergyCh1Dimm0 = SaSetup.ActEnergyCh1Dimm0;
    MemConfig->RdEnergyCh1Dimm1 = SaSetup.RdEnergyCh1Dimm1;
    MemConfig->RdEnergyCh1Dimm0 = SaSetup.RdEnergyCh1Dimm0;
    MemConfig->WrEnergyCh1Dimm1 = SaSetup.WrEnergyCh1Dimm1;
    MemConfig->WrEnergyCh1Dimm0 = SaSetup.WrEnergyCh1Dimm0;

    MemConfig->SrefCfgEna = SaSetup.SrefCfgEna;
    MemConfig->SrefCfgIdleTmr = SaSetup.SrefCfgIdleTmr;
    MemConfig->ThrtCkeMinDefeat = SaSetup.ThrtCkeMinDefeat;
    MemConfig->ThrtCkeMinTmr = SaSetup.ThrtCkeMinTmr;
    MemConfig->ThrtCkeMinDefeatLpddr = SaSetup.ThrtCkeMinDefeatLpddr;
    MemConfig->ThrtCkeMinTmrLpddr = SaSetup.ThrtCkeMinTmrLpddr;

    MemConfig->DisableDimmChannel[0] = SaSetup.DisableDimmChannel0;
    MemConfig->DisableDimmChannel[1] = SaSetup.DisableDimmChannel1;

    MemConfig->DllBwEn0 = SaSetup.DllBwEn0;
    MemConfig->DllBwEn1 = SaSetup.DllBwEn1;
    MemConfig->DllBwEn2 = SaSetup.DllBwEn2;
    MemConfig->DllBwEn3 = SaSetup.DllBwEn3;

    MemConfig->CmdTriStateDis    = SaSetup.CmdTriStateDis;
    MemConfig->RetrainOnFastFail = SaSetup.RetrainOnFastFail;

    //
    // Pcode BCLK value is in kHz units. Convert to Hertz for MRC to use. We need to round
    // the BCLK value to the nearest coarse BCLK freq.
    //
    Status = MailboxRead(MAILBOX_TYPE_OC, MAILBOX_BCLK_FREQUENCY_CMD, &PcodeBclkFrequency, &MailboxStatus);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "(ICC) Pcode BCLK frequency message failed, Status = %X\n", Status));
      DEBUG ((DEBUG_INFO, "(ICC) Setting to default BCLK frequency.\n"));
      PcodeBclkFrequency = BCLK_100_KHZ;
    }

    RoundedBclkFreq = ((PcodeBclkFrequency+500)/1000) * 1000000;

    if (RoundedBclkFreq > BCLK_MAX) {
      RoundedBclkFreq = BCLK_MAX;
    } else if (RoundedBclkFreq < BCLK_100) {
      RoundedBclkFreq = BCLK_100;
    }

    DEBUG ((DEBUG_INFO, "(ICC) Pcode reported Bclk Frequency = %d\n", PcodeBclkFrequency));
    DEBUG ((DEBUG_INFO, "(ICC) Rounded Bclk Frequency        = %d\n", RoundedBclkFreq));
    MemConfig->BClkFrequency = RoundedBclkFreq;

    //
    // DDR Frequency Limit (0 = Auto)
    // The values must match the definitions in KabylakeSiliconPkg\SystemAgent\MemoryInit\Include\MrcInterface.h
    //
    MemConfig->DdrFreqLimit = SaSetup.DdrFreqLimit;

    // Update MemTestOnWarmBoot variable. Default is run BaseMemoryTest on warm boot.
    MemConfigNoCrc->MemTestOnWarmBoot = SaSetup.MemTestOnWarmBoot;

    //
    // Update CleanMemory variable from Memory overwrite request value. Ignore if we are performing capsule update.
    //
#ifdef EFI_S3_RESUME
    if((BootMode != BOOT_ON_FLASH_UPDATE) && (BootMode != BOOT_ON_S3_RESUME)) {
      MemConfigNoCrc->CleanMemory = (BOOLEAN)(MorControl & MOR_CLEAR_MEMORY_BIT_MASK);
    }
#endif //EFI_S3_RESUME

    //
    // Based on BIOS setup to determine maximum top of memory size below 4G, and reserved for MMIO
    //
    switch (SaSetup.MaxTolud) {
    case MAX_TOLUD_DYNAMIC:
      MiscPeiPreMemConfig->MmioSize = 0x0;
      break;

    case MAX_TOLUD_1G:
      MiscPeiPreMemConfig->MmioSize = 0xC00;
      break;

    case MAX_TOLUD_1_25G:
      MiscPeiPreMemConfig->MmioSize = 0xB00;
      break;

    case MAX_TOLUD_1_5G:
      MiscPeiPreMemConfig->MmioSize = 0xA00;
      break;

    case MAX_TOLUD_1_75G:
      MiscPeiPreMemConfig->MmioSize = 0x900;
      break;

    case MAX_TOLUD_2G:
      MiscPeiPreMemConfig->MmioSize = 0x800;
      break;

    case MAX_TOLUD_2_25G:
      MiscPeiPreMemConfig->MmioSize = 0x700;
      break;

    case MAX_TOLUD_2_5G:
      MiscPeiPreMemConfig->MmioSize = 0x600;
      break;

    case MAX_TOLUD_2_75G:
      MiscPeiPreMemConfig->MmioSize = 0x500;
      break;

    default:
    case MAX_TOLUD_3G:
      MiscPeiPreMemConfig->MmioSize = 0x400;
      break;

    case MAX_TOLUD_3_25G:
      MiscPeiPreMemConfig->MmioSize = 0x300;
      break;

    case MAX_TOLUD_3_5G:
      MiscPeiPreMemConfig->MmioSize = 0x200;
      break;
    }
  }
#endif //MINTREE_FLAG

#ifndef MINTREE_FLAG
  DataSize = sizeof (MemoryData);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                               &gEfiMemoryTypeInformationGuid,
                               NULL,
                               &DataSize,
                               &MemoryData
                               );
  ///
  /// Accumulate maximum amount of memory needed
  ///
  if (EFI_ERROR(Status)) {
// AMI_OVERRIDE_START - Since AMI recovery goes to DXE, PlatformMemorySize should include all of memory type.
#if 0	  
    if (BootMode == BOOT_IN_RECOVERY_MODE) {
      MemConfigNoCrc->PlatformMemorySize = PcdGet32 (PcdPeiMinMemorySize);
    } else {
#endif				
// AMI_OVERRIDE_END - Since AMI recovery goes to DXE, PlatformMemorySize should include all of memory type. 
      ///
      /// Use default value to avoid memory fragment. OS boot/installation fails
      /// if there is not enough continuous memory available
      ///
      MemConfigNoCrc->PlatformMemorySize = PcdGet32 (PcdPeiMinMemorySize);
      DataSize = sizeof (mDefaultMemoryTypeInformation);
      CopyMem(MemoryData, mDefaultMemoryTypeInformation, DataSize);
// AMI_OVERRIDE_START - Since AMI recovery goes to DXE, PlatformMemorySize should include all of memory type. 
#if 0			
    }
#endif			
// AMI_OVERRIDE_END - Since AMI recovery goes to DXE, PlatformMemorySize should include all of memory type. 
  } else {
    ///
    /// Start with at least PcdPeiMinMemorySize of memory for PEI post-mem phase
    ///
    MemConfigNoCrc->PlatformMemorySize = PcdGet32 (PcdPeiMinMemorySize);
  }
// AMI_OVERRIDE_START - Since AMI recovery goes to DXE, PlatformMemorySize should include all of memory type. 
#if 0  
  if (BootMode != BOOT_IN_RECOVERY_MODE) {
#else
  if (1) {
#endif	  
// AMI_OVERRIDE_END - Since AMI recovery goes to DXE, PlatformMemorySize should include all of memory type. 
    for (Index = 0; Index < DataSize / sizeof (EFI_MEMORY_TYPE_INFORMATION); Index++) {
      MemConfigNoCrc->PlatformMemorySize += MemoryData[Index].NumberOfPages * EFI_PAGE_SIZE;
    }

    //
    // Initialize the Overclocking Configuration
    //
    OcPreMemConfig->OcSupport = CpuSetup.OverclockingSupport;

    if (OcPreMemConfig->OcSupport && (WdtTimeout == FALSE)) {
      //
      //  SA Domain
      //
      if (SaSetup.UncoreVoltageOffsetPrefix == 1) {
        //
        // Offset is negative, need to convert
        //
        OcPreMemConfig->SaVoltageOffset = (INT16)(~SaSetup.UncoreVoltageOffset + 1);
      } else {
        OcPreMemConfig->SaVoltageOffset = SaSetup.UncoreVoltageOffset;
      }

      //
      //  GT Domain
      //
      //
      //  GT Slice Domain
      //
      OcPreMemConfig->GtsMaxOcRatio = SaSetup.GtsMaxOcRatio;
      OcPreMemConfig->GtsVoltageMode = SaSetup.GtsVoltageMode;

      if (OcPreMemConfig->GtsVoltageMode == OC_LIB_OFFSET_ADAPTIVE) {
        OcPreMemConfig->GtsVoltageOverride = 0;
        OcPreMemConfig->GtsExtraTurboVoltage = SaSetup.GtsExtraTurboVoltage;
      } else if (OcPreMemConfig->GtsVoltageMode == OC_LIB_OFFSET_OVERRIDE) {
        OcPreMemConfig->GtsVoltageOverride = SaSetup.GtsVoltageOverride;
        OcPreMemConfig->GtsExtraTurboVoltage = 0;
      } else {
        OcPreMemConfig->GtsVoltageOverride = 0;
        OcPreMemConfig->GtsExtraTurboVoltage = 0;
      }

      if (SaSetup.GtsVoltageOffsetPrefix == 1) {
        //
        // Offset is negative, need to convert
        //
        OcPreMemConfig->GtsVoltageOffset = (INT16)(~SaSetup.GtsVoltageOffset + 1);
      } else {
        OcPreMemConfig->GtsVoltageOffset = SaSetup.GtsVoltageOffset;
      }

      //
      //  GT Unslice Domain
      //
      OcPreMemConfig->GtusMaxOcRatio = SaSetup.GtusMaxOcRatio;
      OcPreMemConfig->GtusVoltageMode = SaSetup.GtusVoltageMode;

      if (OcPreMemConfig->GtusVoltageMode == OC_LIB_OFFSET_ADAPTIVE) {
        OcPreMemConfig->GtusVoltageOverride = 0;
        OcPreMemConfig->GtusExtraTurboVoltage = SaSetup.GtusExtraTurboVoltage;
      } else if (OcPreMemConfig->GtusVoltageMode == OC_LIB_OFFSET_OVERRIDE) {
        OcPreMemConfig->GtusVoltageOverride = SaSetup.GtusVoltageOverride;
        OcPreMemConfig->GtusExtraTurboVoltage = 0;
      } else {
        OcPreMemConfig->GtusVoltageOverride = 0;
        OcPreMemConfig->GtusExtraTurboVoltage = 0;
      }

      if (SaSetup.GtusVoltageOffsetPrefix == 1) {
        //
        // Offset is negative, need to convert
        //
        OcPreMemConfig->GtusVoltageOffset = (INT16)(~SaSetup.GtusVoltageOffset + 1);
      } else {
        OcPreMemConfig->GtusVoltageOffset = SaSetup.GtusVoltageOffset;
      }
    } else {
      OcPreMemConfig->GtsMaxOcRatio = 0;
      OcPreMemConfig->GtsVoltageMode = 0;
      OcPreMemConfig->GtsVoltageOverride = 0;
      OcPreMemConfig->GtsExtraTurboVoltage = 0;
      OcPreMemConfig->GtsVoltageOffset = 0;
      OcPreMemConfig->GtusVoltageMode = 0;
      OcPreMemConfig->GtusVoltageOverride = 0;
      OcPreMemConfig->GtusExtraTurboVoltage = 0;
      OcPreMemConfig->GtusVoltageOffset = 0;
      OcPreMemConfig->SaVoltageOffset = 0;
    }
    ///
    /// Realtime Memory Timng support alone cannot cause WDT timeout
    /// No need to disable realtime memory support when wdt expires
    ///
    OcPreMemConfig->RealtimeMemoryTiming = SaSetup.RealtimeMemoryTiming;

    ///
    /// Build the GUID'd HOB for DXE
    ///
    BuildGuidDataHob (
      &gEfiMemoryTypeInformationGuid,
      MemoryData,
      DataSize
      );
  }
#endif //MINTREE_FLAG

  return EFI_SUCCESS;
}

