/** @file
  This file is SampleCode of the library for Intel PCH PEI Policy initialization.

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

#include "PeiPchPolicyUpdate.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchHsioLib.h>
#include <Library/PchPcieRpLib.h>
#include "PchHsioPtssTables.h"
#include <Library/DebugLib.h>
#include <PlatformBoardConfig.h>
#include <Library/BoardConfigLib.h>

VOID
InstallPlatformHsioPtssTable (
  IN          BOARD_CONFIG_BLOCK_PEI_PREMEM *PlatformBoardConfig,
  IN          PCH_SETUP                     *PchSetup,  // AdvancedFeaturesContent
  IN OUT      SI_PREMEM_POLICY_PPI          *SiPreMemPolicy
  )
{
  HSIO_PTSS_TABLES            *UnknowPtssTables;
  HSIO_PTSS_TABLES            *SpecificPtssTables;
  HSIO_PTSS_TABLES            *PtssTables;
  UINT8                       PtssTableIndex;
  UINT32                      UnknowTableSize;
  UINT32                      SpecificTableSize;
  UINT32                      TableSize;
  UINT32                      Entry;
  UINT8                       LaneNum;
  UINT8                       Index;
  UINT8                       MaxSataPorts;
  UINT8                       MaxPciePorts;
  UINT8                       PcieTopologyReal[PCH_MAX_PCIE_ROOT_PORTS];
  UINT8                       PciePort;
  UINTN                       RpBase;
  UINTN                       RpDevice;
  UINTN                       RpFunction;
  UINT32                      StrapFuseCfg;
  UINT8                       PcieControllerCfg;
  EFI_STATUS                  Status;
  PCH_HSIO_PCIE_PREMEM_CONFIG *HsioPciePreMemConfig;
  PCH_HSIO_SATA_PREMEM_CONFIG *HsioSataPreMemConfig;

  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gHsioPciePreMemConfigGuid, (VOID *) &HsioPciePreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gHsioSataPreMemConfigGuid, (VOID *) &HsioSataPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  UnknowPtssTables = NULL;
  UnknowTableSize = 0;
  SpecificPtssTables = NULL;
  SpecificTableSize = 0;

  if (GetPchGeneration () == SklPch) {
    switch (PchStepping ()) {
      case PchLpB0:
      case PchLpB1:
        UnknowPtssTables = PlatformBoardConfig->UnknowLpHsioPtssTable [0];
        UnknowTableSize = PlatformBoardConfig->UnknowLpHsioPtssTableSize [0];
        SpecificPtssTables = PlatformBoardConfig->SpecificLpHsioPtssTable [0];
        SpecificTableSize = PlatformBoardConfig->SpecificLpHsioPtssTableSize [0];
        break;
      case PchLpC0:
      case PchLpC1:
        UnknowPtssTables = PlatformBoardConfig->UnknowLpHsioPtssTable [1];
        UnknowTableSize = PlatformBoardConfig->UnknowLpHsioPtssTableSize [1];
        SpecificPtssTables = PlatformBoardConfig->SpecificLpHsioPtssTable [1];
        SpecificTableSize = PlatformBoardConfig->SpecificLpHsioPtssTableSize [1];
        break;
      case PchHB0:
      case PchHC0:
        UnknowPtssTables = PlatformBoardConfig->UnknowHHsioPtssTable [0];
        UnknowTableSize = PlatformBoardConfig->UnknowHHsioPtssTableSize [0];
        SpecificPtssTables = PlatformBoardConfig->SpecificHHsioPtssTable [0];
        SpecificTableSize = PlatformBoardConfig->SpecificHHsioPtssTableSize [0];
        break;
      case PchHD0:
      case PchHD1:
        UnknowPtssTables = PlatformBoardConfig->UnknowHHsioPtssTable [1];
        UnknowTableSize = PlatformBoardConfig->UnknowHHsioPtssTableSize [1];
        SpecificPtssTables = PlatformBoardConfig->SpecificHHsioPtssTable [1];
        SpecificTableSize = PlatformBoardConfig->SpecificHHsioPtssTableSize [1];
        break;
      default:
        UnknowPtssTables = NULL;
        UnknowTableSize = 0;
        SpecificPtssTables = NULL;
        SpecificTableSize = 0;
        DEBUG ((DEBUG_ERROR, "Unsupported PCH Stepping\n"));
    }
  } else {
    switch (PchStepping ()) {
      case KblPchHA0:
        UnknowPtssTables = PlatformBoardConfig->UnknowHHsioPtssTable [1];
        UnknowTableSize = PlatformBoardConfig->UnknowHHsioPtssTableSize [1];
        SpecificPtssTables = PlatformBoardConfig->SpecificHHsioPtssTable [1];
        SpecificTableSize = PlatformBoardConfig->SpecificHHsioPtssTableSize [1];
        break;
      default:
        UnknowPtssTables = NULL;
        UnknowTableSize = 0;
        SpecificPtssTables = NULL;
        SpecificTableSize = 0;
        DEBUG ((DEBUG_ERROR, "Unsupported PCH Stepping\n"));
    }
  }

  PtssTableIndex = 0;
  MaxSataPorts = GetPchMaxSataPortNum ();
  MaxPciePorts = GetPchMaxPciePortNum ();
  ZeroMem (PcieTopologyReal, sizeof (PcieTopologyReal));

  //Populate PCIe topology based on lane configuration
  //
  // AdvancedFeaturesBegin
  //
  for (Index = 0; Index < MaxPciePorts; Index++) {
    PcieTopologyReal[Index] = PchSetup->PcieTopology[Index];
  }
  //
  // AdvancedFeaturesEnd
  //
  for (PciePort = 0; PciePort < MaxPciePorts; PciePort += 4) {
    Status = GetPchPcieRpDevFun (PciePort, &RpDevice, &RpFunction);
    ASSERT_EFI_ERROR (Status);

    RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDevice, (UINT32) RpFunction);
    StrapFuseCfg = MmioRead32 (RpBase + R_PCH_PCIE_STRPFUSECFG);
    PcieControllerCfg = (UINT8) ((StrapFuseCfg & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC);
    DEBUG ((DEBUG_INFO, "PCIE Port %d StrapFuseCfg Value = %d\n", PciePort, PcieControllerCfg));
    //
    // AdvancedFeaturesBegin
    //
    PcieTopologyReal[PciePort] = PchSetup->PcieTopology[PciePort];
    if (PcieControllerCfg != V_PCH_PCIE_STRPFUSECFG_RPC_1_1_1_1) {
      PcieTopologyReal[PciePort + 1] = PchSetup->PcieTopology[PciePort];
    }
    if (PcieControllerCfg == V_PCH_PCIE_STRPFUSECFG_RPC_4) {
      PcieTopologyReal[PciePort + 2] = PchSetup->PcieTopology[PciePort];
      PcieTopologyReal[PciePort + 3] = PchSetup->PcieTopology[PciePort];
    }
    if (PcieControllerCfg == V_PCH_PCIE_STRPFUSECFG_RPC_2_2) {
      PcieTopologyReal[PciePort + 2] = PchSetup->PcieTopology[PciePort + 2];
      PcieTopologyReal[PciePort + 3] = PchSetup->PcieTopology[PciePort + 2];
    }
    if (PcieControllerCfg == V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1) {
      PcieTopologyReal[PciePort + 2] = PchSetup->PcieTopology[PciePort + 2];
      PcieTopologyReal[PciePort + 3] = PchSetup->PcieTopology[PciePort + 3];
    }
    //
    // AdvancedFeaturesEnd
    //
  }
  for (Index = 0; Index < MaxPciePorts; Index++) {
    DEBUG ((DEBUG_INFO, "PCIE PTSS Setup RP %d Topology = %d\n", Index, PchSetup->PcieTopology[Index]));  // AdvancedFeaturesContent
    DEBUG ((DEBUG_INFO, "PCIE PTSS Assigned RP %d Topology = %d\n", Index, PcieTopologyReal[Index]));
  }

  //Case 1: BoardId is known, Topology is known/unknown
  //Case 1a: SATA
  PtssTables = SpecificPtssTables;
  TableSize = SpecificTableSize;
  for (Index = 0; Index < MaxSataPorts; Index++) {
    if (PchGetSataLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
      for (Entry = 0; Entry < TableSize; Entry++) {
        if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
            (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA)
            && (PchSetup->SataTopology[Index] == PtssTables[Entry].Topology) // AdvancedFeaturesContent
          )
        {
          PtssTableIndex++;
          if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD20) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0) == (UINT32) B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0)) {
            HsioSataPreMemConfig->PortLane[Index].HsioRxGen3EqBoostMagEnable = TRUE;
            HsioSataPreMemConfig->PortLane[Index].HsioRxGen3EqBoostMag = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8)) {
            if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) {
              HsioSataPreMemConfig->PortLane[Index].HsioTxGen1DownscaleAmpEnable = TRUE;
              HsioSataPreMemConfig->PortLane[Index].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
            }
            if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) {
              HsioSataPreMemConfig->PortLane[Index].HsioTxGen2DownscaleAmpEnable = TRUE;
              HsioSataPreMemConfig->PortLane[Index].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
            }
          } else {
            ASSERT (FALSE);
          }
        }
      }
    }
  }
  //Case 1b: PCIe
  for (Index = 0; Index < MaxPciePorts; Index++) {
    if (PchGetPcieLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
      for (Entry = 0; Entry < TableSize; Entry++) {
        if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
            (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) &&
            (PcieTopologyReal[Index] == PtssTables[Entry].Topology)) {
          PtssTableIndex++;
          if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD25) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0) == (UINT32) B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0)) {
            HsioPciePreMemConfig->Lane[Index].HsioRxSetCtleEnable = TRUE;
            HsioPciePreMemConfig->Lane[Index].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
          } else {
            ASSERT (FALSE);
          }
        }
      }
    }
  }
  //Case 2: BoardId is unknown, Topology is known/unknown
  if (PtssTableIndex == 0) {
    DEBUG ((DEBUG_INFO, "PTSS Settings for unknown board will be applied\n"));

    PtssTables = UnknowPtssTables;
    TableSize = UnknowTableSize;

    for (Index = 0; Index < MaxSataPorts; Index++) {
      if (PchGetSataLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
        for (Entry = 0; Entry < TableSize; Entry++) {
          if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
              (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA)
              && (PchSetup->SataTopology[Index] == PtssTables[Entry].Topology) // AdvancedFeaturesContent
             )
          {
            if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD20) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0) == (UINT32) B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0)) {
              HsioSataPreMemConfig->PortLane[Index].HsioRxGen3EqBoostMagEnable = TRUE;
              HsioSataPreMemConfig->PortLane[Index].HsioRxGen3EqBoostMag = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
            } else if (PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8) {
              if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) {
                HsioSataPreMemConfig->PortLane[Index].HsioTxGen1DownscaleAmpEnable = TRUE;
                HsioSataPreMemConfig->PortLane[Index].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
              }
              if (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) {
                HsioSataPreMemConfig->PortLane[Index].HsioTxGen2DownscaleAmpEnable = TRUE;
                HsioSataPreMemConfig->PortLane[Index].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
              }
            } else {
              ASSERT (FALSE);
            }
          }
        }
      }
    }
    for (Index = 0; Index < MaxPciePorts; Index++) {
      if (PchGetPcieLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
        for (Entry = 0; Entry < TableSize; Entry++) {
          if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
              (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) &&
              (PcieTopologyReal[Index] == PtssTables[Entry].Topology)) {
            if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD25) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0) == (UINT32) B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0)) {
              HsioPciePreMemConfig->Lane[Index].HsioRxSetCtleEnable = TRUE;
              HsioPciePreMemConfig->Lane[Index].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
            } else {
              ASSERT (FALSE);
            }
          }
        }
      }
    }
  }
}


//
// Sawtooth Peak
// Single SPD EEPROM at 0xA2 serves both C0D0 and C1D0 (LPDDR is 1DPC only)
//
#define DIMM_SMB_SPD_P0C0D0_STP 0xA2
#define DIMM_SMB_SPD_P0C0D1_STP 0xA0
#define DIMM_SMB_SPD_P0C1D0_STP 0xA2
#define DIMM_SMB_SPD_P0C1D1_STP 0xA0

// AMI_OVERRIDE_START - Fix Linux build error.
#if 0
GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mSmbusSTPRsvdAddresses[] = {
  DIMM_SMB_SPD_P0C0D0_STP,
  DIMM_SMB_SPD_P0C0D1_STP,
  DIMM_SMB_SPD_P0C1D0_STP,
  DIMM_SMB_SPD_P0C1D1_STP
};
#endif
// AMI_OVERRIDE_END - Fix Linux build error.


/**
  This function performs PCH PEI Policy initialization.

  @param[in, out] SiPreMemPolicy  The SI PREMEM Policy PPI instance

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
UpdatePeiPchPolicyPreMem (
  IN OUT   SI_PREMEM_POLICY_PPI  *SiPreMemPolicy
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
#ifndef MINTREE_FLAG
  PCH_SETUP                       PchSetup;
  UINT32                          RpIndex;
#endif // MINTREE_FLAG
  SETUP_DATA                      SetupVariables;
  PLATFORM_INFO                   *PlatformInfo;

  PCH_GENERAL_PREMEM_CONFIG       *PchGeneralPreMemConfig;
  PCH_TRACE_HUB_PREMEM_CONFIG     *TraceHubPreMemConfig;
  PCH_HPET_PREMEM_CONFIG          *HpetPreMemConfig;
  PCH_LPC_PREMEM_CONFIG           *LpcPreMemConfig;
  BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig;
  PCH_WDT_PREMEM_CONFIG           *WatchDogPreMemConfig;
  PCH_DCI_PREMEM_CONFIG           *DciPreMemConfig;
  PCH_PCIE_RP_PREMEM_CONFIG       *PcieRpPreMemConfig;
  PCH_HSIO_PCIE_PREMEM_CONFIG     *HsioPciePreMemConfig;

  Status = EFI_SUCCESS;

  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gPchGeneralPreMemConfigGuid, (VOID *) &PchGeneralPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gTraceHubPreMemConfigGuid, (VOID *) &TraceHubPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gHpetPreMemConfigGuid, (VOID *) &HpetPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gLpcPreMemConfigGuid, (VOID *) &LpcPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gWatchDogPreMemConfigGuid, (VOID *) &WatchDogPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gDciPreMemConfigGuid, (VOID *) &DciPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gPcieRpPreMemConfigGuid, (VOID *) &PcieRpPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPreMemPolicy, &gHsioPciePreMemConfigGuid, (VOID *) &HsioPciePreMemConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPreMemGuid, (VOID **) &PlatformBoardConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Retrieve Setup variable
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices       // PPI
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SetupVariables
                               );
  ASSERT_EFI_ERROR (Status);

#ifndef MINTREE_FLAG
  VariableSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &PchSetup
                               );
  ASSERT_EFI_ERROR (Status);


  PchGeneralPreMemConfig->Port80Route = PchSetup.IchPort80Route;

  //
  // DCI
  //
  DciPreMemConfig->DciEn = PchSetup.PchDciEn;

  //
  // Update TraceHub config based on setup options
  //
  TraceHubPreMemConfig->EnableMode = PchSetup.TraceHubEnableMode;

  switch (PchSetup.MemRegion0BufferSize) {
    case 0:
       TraceHubPreMemConfig->MemReg0Size = 0; // No memory
       break;
    case 1:
       TraceHubPreMemConfig->MemReg0Size = 0x100000;  // 1MB
       break;
    case 2:
       TraceHubPreMemConfig->MemReg0Size = 0x800000;  // 8MB
       break;
    case 3:
       TraceHubPreMemConfig->MemReg0Size = 0x4000000; // 64MB
       break;
    case 4:
       TraceHubPreMemConfig->MemReg0Size = 0x8000000; // 128MB
       break;
    case 5:
       TraceHubPreMemConfig->MemReg0Size = 0x10000000; // 256MB
       break;
    case 6:
       TraceHubPreMemConfig->MemReg0Size = 0x20000000; // 512MB
       break;
    case 7:
       TraceHubPreMemConfig->MemReg0Size = 0x40000000; // 1GB
       break;
  }

  switch (PchSetup.MemRegion1BufferSize) {
    case 0:
       TraceHubPreMemConfig->MemReg1Size = 0; // No memory
       break;
    case 1:
       TraceHubPreMemConfig->MemReg1Size = 0x100000;  // 1MB
       break;
    case 2:
       TraceHubPreMemConfig->MemReg1Size = 0x800000;  // 8MB
       break;
    case 3:
       TraceHubPreMemConfig->MemReg1Size = 0x4000000; // 64MB
       break;
    case 4:
       TraceHubPreMemConfig->MemReg1Size = 0x8000000; // 128MB
       break;
    case 5:
       TraceHubPreMemConfig->MemReg1Size = 0x10000000; // 256MB
       break;
    case 6:
       TraceHubPreMemConfig->MemReg1Size = 0x20000000; // 512MB
       break;
    case 7:
       TraceHubPreMemConfig->MemReg1Size = 0x40000000; // 1GB
       break;
  }
  //
  // HPET
  //
  HpetPreMemConfig->Enable = PchSetup.Hpet;

  //
  // LPC
  //
  LpcPreMemConfig->EnhancePort8xhDecoding = PchSetup.EnhancePort8xhDecoding;
#endif // MINTREE_FLAG
  //
  // HSIO SATA and PCIE.
  //
#ifndef MINTREE_FLAG
  InstallPlatformHsioPtssTable (PlatformBoardConfig, &PchSetup, SiPreMemPolicy);
#else
  InstallPlatformHsioPtssTable (PlatformBoardConfig, SiPreMemPolicy);
#endif // MINTREE_FLAG

#ifndef MINTREE_FLAG
  HsioPciePreMemConfig->PciePllSsc = PchSetup.PciePllSsc;

  //
  // WatchDog Config
  //
  WatchDogPreMemConfig->DisableAndLock = !PchSetup.WdtEnable;

  //
  // PCIE RP config
  //
  for (RpIndex = 0; RpIndex < GetPchMaxPciePortNum (); RpIndex ++) {
    if (PchSetup.PcieRootPortEn[RpIndex]) {
      PcieRpPreMemConfig->RpEnabledMask |= (UINT32) (1 << RpIndex);
    } else {
      PcieRpPreMemConfig->RpEnabledMask &= ~(UINT32) (1 << RpIndex);
    }
  }
#endif // MINTREE_FLAG

  return Status;
}

