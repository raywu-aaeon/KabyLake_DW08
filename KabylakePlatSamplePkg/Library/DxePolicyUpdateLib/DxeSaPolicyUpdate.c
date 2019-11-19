/** @file
  This file is the library for SA DXE Policy initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2004 - 2019 Intel Corporation.

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

#include <DxeSaPolicyUpdate.h>
#include <Library/BoardConfigLib.h>
// AMI_OVERRIDE_START - For AMI Setup.
#include <NbSetupData.h>
#include <Library/SystemAgentHookDxeLib.h>

#define SaSetup     SetupVariables
// AMI_OVERRIDE_END - For AMI Setup.


/**
  Get data for platform policy from setup options.

  @param[in] SaPolicy                  The pointer to get SA Policy protocol instance

  @retval EFI_SUCCESS                  Operation success.

**/
EFI_STATUS
EFIAPI
UpdateDxeSaPolicy (
  IN OUT  SA_POLICY_PROTOCOL    *SaPolicy
  )
{
  UINTN                     VariableSize;
// AMI_OVERRIDE_START - For AMI Setup.
#if 0
  SA_SETUP                  SaSetup;
  SETUP_DATA                SetupVariables;
#else  
  NB_SETUP_DATA             SetupVariables; 
#endif
// AMI_OVERRIDE_END - For AMI Setup.
  EFI_STATUS                Status;
  UINTN                     McD0BaseAddress;
  EFI_PHYSICAL_ADDRESS      RemapBase;
  EFI_PHYSICAL_ADDRESS      RemapLimit;
  EFI_PHYSICAL_ADDRESS      MeSegMask;
  EFI_PHYSICAL_ADDRESS      MeStolenSize;
  BOOLEAN                   MeStolenEnable;
  UINT32                    Tolud;
  UINT8                     pegFn;
  GRAPHICS_DXE_CONFIG       *GraphicsDxeConfig;
  PCIE_DXE_CONFIG           *PcieDxeConfig;
  MISC_DXE_CONFIG           *MiscDxeConfig;
  MEMORY_DXE_CONFIG         *MemoryDxeConfig;
//
//  KblGBegin
//
  UINTN                     DataSize;
  UINT8                     XConnectMode;
//
//  KblGEnd
//
  GraphicsDxeConfig = NULL;
  PcieDxeConfig = NULL;
  MiscDxeConfig = NULL;
  MemoryDxeConfig = NULL;
  //
  // Get requisite IP Config Blocks which needs to be used here
  //
  Status = GetConfigBlock ((VOID *)SaPolicy, &gGraphicsDxeConfigGuid, (VOID *)&GraphicsDxeConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *)SaPolicy, &gMiscDxeConfigGuid, (VOID *)&MiscDxeConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *)SaPolicy, &gPcieDxeConfigGuid, (VOID *)&PcieDxeConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *)SaPolicy, &gMemoryDxeConfigGuid, (VOID *)&MemoryDxeConfig);
  ASSERT_EFI_ERROR (Status);

// AMI_OVERRIDE_START - For AMI Chipset Setup.
#if 0
  VariableSize = sizeof (SETUP_DATA);
  ZeroMem (&SetupVariables, VariableSize);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupVariables
                  );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SaSetup
                  );
  ASSERT_EFI_ERROR (Status);
#else
  DEBUG ((DEBUG_INFO, "UpdateDxeSaPlatformPolicy() - Start\n"));
  
  VariableSize = sizeof (NB_SETUP_DATA);
  ZeroMem (&SetupVariables, VariableSize);
  GetNbSetupData (gRT, &SetupVariables, FALSE);
#endif
// AMI_OVERRIDE_END - For AMI Chipset Setup.


  if (PcdGetBool (PcdSpdAddressOverride)) {
    MemoryDxeConfig->SpdAddressTable[0] = DIMM_SMB_SPD_P0C0D0_STP;
    MemoryDxeConfig->SpdAddressTable[1] = DIMM_SMB_SPD_P0C0D1_STP;
    MemoryDxeConfig->SpdAddressTable[2] = DIMM_SMB_SPD_P0C1D0_STP;
    MemoryDxeConfig->SpdAddressTable[3] = DIMM_SMB_SPD_P0C1D1_STP;
  }

  if (SaSetup.EnableVtd == 0) {
    MiscDxeConfig->RmrrUsbBaseAddress[0] = 0;
    MiscDxeConfig->RmrrUsbBaseAddress[1] = 0;
  }

  if (!EFI_ERROR(Status)) {
    //
    // Initialize the PCIE Configuration
    //
    for ( pegFn=0; pegFn < 3; pegFn++ ) {
      PcieDxeConfig->PegAspm[pegFn]        = SaSetup.PegAspm[pegFn];
      PcieDxeConfig->PegAspmL0s[pegFn]     = SaSetup.PegAspmL0s[pegFn];
    }

    PcieDxeConfig->PegPwrOpt[0].LtrEnable  = SaSetup.Peg0LtrEnable;
    PcieDxeConfig->PegPwrOpt[0].ObffEnable = SaSetup.Peg0ObffEnable;
    PcieDxeConfig->PegPwrOpt[1].LtrEnable  = SaSetup.Peg1LtrEnable;
    PcieDxeConfig->PegPwrOpt[1].ObffEnable = SaSetup.Peg1ObffEnable;
    PcieDxeConfig->PegPwrOpt[2].LtrEnable  = SaSetup.Peg2LtrEnable;
    PcieDxeConfig->PegPwrOpt[2].ObffEnable = SaSetup.Peg2ObffEnable;

    if(SaSetup.Peg0Enable == 1) {
      PcieDxeConfig->PegRootPortHPE[0] = SaSetup.PegRootPortHPE[0];
    }
    if(SaSetup.Peg1Enable == 1) {
      PcieDxeConfig->PegRootPortHPE[1] = SaSetup.PegRootPortHPE[1];
    }
    if(SaSetup.Peg2Enable == 1) {
      PcieDxeConfig->PegRootPortHPE[2] = SaSetup.PegRootPortHPE[2];
    }

    //
    // Global NVS Graphics configuration
    //
    GraphicsDxeConfig->AlsEnable                    = SaSetup.AlsEnable;
    GraphicsDxeConfig->BacklightControlSupport      = SaSetup.IgdLcdBlc;
    GraphicsDxeConfig->IgdBootType                  = SaSetup.IgdBootType;
    GraphicsDxeConfig->IgdPanelType                 = SaSetup.LcdPanelType;
    GraphicsDxeConfig->IgdPanelScaling              = SaSetup.LcdPanelScaling;
    GraphicsDxeConfig->IgdBlcConfig                 = SaSetup.IgdLcdBlc;
    GraphicsDxeConfig->LowPowerMode                 = SaSetup.LowPowerMode;
    GraphicsDxeConfig->IgdDvmtMemSize               = SaSetup.IgdDvmt50TotalAlloc;
    GraphicsDxeConfig->GfxTurboIMON                 = SaSetup.GfxTurboIMON;

    MiscDxeConfig->EnableAbove4GBMmio          = SaSetup.EnableAbove4GBMmio;

    GraphicsDxeConfig->IuerStatusVal = 0;
    GraphicsDxeConfig->IuerStatusVal |= SaSetup.SlateIndicatorRT << 6;
    GraphicsDxeConfig->IuerStatusVal |= SaSetup.DockIndicatorRT << 7;
  }

  //
  // PCI HostBridge resource range
  //
  // Calculate PCI memory space, which ranges from TOLUD through (MCH_PCI_EXPRESS_BASE_ADDRESS-1).
  //
  McD0BaseAddress    = MmPciBase (SA_MC_BUS, 0, 0);
  RemapBase          = (MmioRead32 (McD0BaseAddress + R_SA_REMAPBASE) + LShiftU64 (MmioRead32 (McD0BaseAddress + R_SA_REMAPBASE + 4), 32)) & B_SA_REMAPBASE_REMAPBASE_MASK;
  RemapLimit         = (MmioRead32 (McD0BaseAddress + R_SA_REMAPLIMIT) + LShiftU64 (MmioRead32 (McD0BaseAddress + R_SA_REMAPLIMIT + 4), 32)) & B_SA_REMAPLIMIT_REMAPLMT_MASK;
  Tolud              = MmioRead32 (McD0BaseAddress + R_SA_TOLUD) & B_SA_TOLUD_TOLUD_MASK;
  MeSegMask          = (MmioRead32 (McD0BaseAddress + R_SA_MESEG_MASK) + LShiftU64 (MmioRead32 (McD0BaseAddress + R_SA_MESEG_MASK + 4), 32));
  MeStolenEnable     = (BOOLEAN) ((MeSegMask & B_SA_MESEG_MASK_ME_STLEN_EN_MASK) != 0);

  //
  // First check if memory remap is used
  //
  if ((RemapBase > RemapLimit) && (MeStolenEnable)) {
    MeStolenSize = MeSegMask & B_SA_MESEG_MASK_MEMASK_MASK;
    if (MeStolenSize != 0) {
      MeStolenSize = 0x8000000000L - MeStolenSize;
    }
  }

  //
  // Initialize the Memory Configuration
  //
  MemoryDxeConfig->ChannelASlotMap = PcdGet8 (PcdChannelASlotMap);
  MemoryDxeConfig->ChannelBSlotMap = PcdGet8 (PcdChannelBSlotMap);

  //
  // Initialize the Platform dependent Graphics configuration.
  // Set BIT0 & BIT1 if Platform is Connected Standby capable & it's capability field is valid respectively. 
  // Please refer to IGD's ACPI Opregion spec for other bit definitions.
  //
  if (SetupVariables.LowPowerS0Idle == 0) {
    GraphicsDxeConfig->PlatformConfig |= (BIT1 | BIT0);
  } else {
    GraphicsDxeConfig->PlatformConfig &= (UINT32) (~BIT0);
  }

  //
  // KblG XConnectMode 
  //
  if (IsKblGPackage()) {
    XConnectMode = 0;
    DataSize = sizeof(UINT8);
    Status = gRT->GetVariable(
      L"XConnectMode",
      &gXConnectModeGuid,
      NULL,
      &DataSize,
      &XConnectMode
    );
    if (Status == EFI_SUCCESS) {
      if (XConnectMode == 0x1) {
        PcieDxeConfig->XConnectEnable = 0x1;
      } else {
        PcieDxeConfig->XConnectEnable = 0x0;
      }
    } else {
      // Ensure Policy value is not set if EFI Variable is not found
      PcieDxeConfig->XConnectEnable = 0x0;
    }
  }
// AMI_OVERRIDE_START - For AMI update Policy. 
  AmiUpdateDxeSaPolicy(SaPolicy, &SetupVariables);
// AMI_OVERRIDE_END - For AMI update Policy. 
  return EFI_SUCCESS;
}

