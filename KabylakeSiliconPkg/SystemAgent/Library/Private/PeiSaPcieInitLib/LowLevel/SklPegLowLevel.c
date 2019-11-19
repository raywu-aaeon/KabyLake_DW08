/** @file
  SA PCIe Initialization Low Level functions for Skylake DT & HALO

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2016 Intel Corporation.

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

@par Specification
**/

#include "SaPegLowLevel.h"
#include <SystemAgent/Library/Private/PeiSaPcieInitLib/PcieTraining.h>

/**
  This function determines if the silicon implements the PCIe bus interface
  that this instance of PCIE_SI_LOW_LEVEL_FUNCTION_CALLS is intended for.

  @retval TRUE - Silicon supports the bus interface
  @retval FALSE - otherwise
**/
BOOLEAN
EFIAPI
SklPcieExists (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This
  )
{
  CPU_FAMILY        CpuFamilyId;

  CpuFamilyId   = GetCpuFamily ();
  ///
  /// Check if correct silicon
  ///
  if (!IsPchLinkDmi (CpuFamilyId)) {
    return FALSE;
  }

  ///
  /// Check to see if PEG exists
  ///
  if (MmioRead16 (MmPciBase (SA_PEG_BUS_NUM, SA_PEG_DEV_NUM, SA_PEG10_FUN_NUM) + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return FALSE;
  }
  return TRUE;
}

/**
  This function determines the topology of the PCIe bus interface that is being
  initialized using silicon defined mechanisms.  The PciePorts pointer must
  point to a pre-allocated array which is capable of containing the maximum
  number of root ports that this function will return.  Generally this is done
  by a component specific entrypoint that can allocate the array on the stack
  using a fixed size appropriate for the HW.  If this needs to be called from
  generic code, the generic code must allocate a buffer that can contain 256
  entries (which should be avoided.)

  @param[in]  This                        - Low level function table
  @param[out] PciePorts                   - Array of Detected PCIe Root Ports
  @param[out] PciePortsLength             - Length of the PciePorts array
**/
VOID
EFIAPI
SklGetPcieRootPorts (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This,
  OUT PCIE_PORT_INFO                    *PciePorts,
  OUT UINT8                             *PciePortsLength
  )
{
  PCIE_PEI_PREMEM_CONFIG  *PciePeiPreMemConfig;
  UINT32                  HwStrap;
  BOOLEAN                 Gen3Capable;
  UINT8                   PegBus;
  UINT8                   PegDev;
  UINT8                   PcieController;
  UINT8                   Index;
  UINT8                   Lane;
  UINT8                   LaneIndex;
  UINT8                   FurcationSetup[SA_PEG_MAX_FUN];
  UINT8                   PcieControllerList[SA_PEG_MAX_FUN];
  UINT8                   NumberToCheck;
  UINT8                   StartLane;
  UINT8                   Width;
  UINT8                   LaneReversal;
  UINT8                   NextPortLaneOffset;

  PegBus              = SA_PEG_BUS_NUM;
  PegDev              = SA_PEG_DEV_NUM;
  PciePeiPreMemConfig = ((SA_PCIE_PRIVATE_FUNCTION_CALLS*) This->PrivateData)->PciePeiPreMemConfig;
  HwStrap             = (MmioRead32 (
                           MmPciBase (
                             SA_PEG_BUS_NUM,
                             SA_PEG_DEV_NUM,
                             SA_PEG10_FUN_NUM
                             ) + R_SA_PEG_FUSESCMN_OFFSET
                           ) >> 16) & 0x3;
  LaneReversal        = (MmioRead32 (
                           MmPciBase (
                             SA_PEG_BUS_NUM,
                             SA_PEG_DEV_NUM,
                             SA_PEG10_FUN_NUM
                             ) + R_SA_PEG_PEGTST_OFFSET
                           ) >> 20) & 0x1;
  Gen3Capable         = TRUE;
  if (MmioRead32 (MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN) + R_SA_MC_CAPID0_B) & BIT20) {
    Gen3Capable = FALSE;
    DEBUG ((DEBUG_INFO, "PEG Gen3 Fused off\n"));
  }

  switch (HwStrap) {
    case SA_PEG_x8_x4_x4:
      FurcationSetup[0] = 8;
      FurcationSetup[1] = 4;
      FurcationSetup[2] = 4;
      NumberToCheck     = 3;
      break;
    case SA_PEG_x8_x8_x0:
      FurcationSetup[0] = 8;
      FurcationSetup[1] = 8;
      NumberToCheck     = 2;
      break;
    default:
    case SA_PEG_x16_x0_x0:
      FurcationSetup[0] = 16;
      NumberToCheck     = 1;
      break;
  }

  ///
  /// Figure out which PcieControllers are enabled
  ///
  (*PciePortsLength) = 0;
  for (PcieController = 0; PcieController < NumberToCheck; PcieController++) {
    ///
    /// Sanity check to make sure width > 0
    ///
    if (FurcationSetup[PcieController] == 0) {
      continue;
    }

    ///
    /// Check to make sure the Root Port Exists
    ///
    if (MmioRead16 (MmPciBase (PegBus, PegDev, PcieController) + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      continue;
    }

    ///
    /// Add the PcieController to the list of enabled controllers
    ///
    PcieControllerList[ (*PciePortsLength) ] = PcieController;
    (*PciePortsLength) ++;
  }

  if (LaneReversal == 0) {
    StartLane = 0;
  } else {
    StartLane = 15;
  }
  for (Index = 0; Index < (*PciePortsLength); Index++) {
    PcieController = PcieControllerList[Index];

    ///
    /// Get information for the current port
    ///
    (PciePorts[Index]).Bus                          = PegBus;
    (PciePorts[Index]).Device                       = PegDev;
    (PciePorts[Index]).Function                     = PcieController;
    (PciePorts[Index]).ConfigSpaceBase              = MmPciBase (PegBus, PegDev, PcieController);
    (PciePorts[Index]).PcieCapOffset                = This->GetPcieCapOffset (This, &(PciePorts[Index]));
    Width                                           = FurcationSetup[PcieController];
    (PciePorts[Index]).MaxPortWidth                 = Width;
    if (SaPolicyGetPegMaxLinkSpeed (PcieController, PciePeiPreMemConfig) == PEG_AUTO) {
      (PciePorts[Index]).MaxPortSpeed               = 3;
    } else {
      (PciePorts[Index]).MaxPortSpeed               = SaPolicyGetPegMaxLinkSpeed (PcieController, PciePeiPreMemConfig);
    }
    if (!Gen3Capable && ((PciePorts[Index]).MaxPortSpeed) >= 3) {
      (PciePorts[Index]).MaxPortSpeed               = 2;
    }
    (PciePorts[Index]).EndpointPresent              = FALSE;
    (PciePorts[Index]).SwEqData.EnableMargin        = TRUE;
    (PciePorts[Index]).SwEqData.SkipMargin          = FALSE;
    (PciePorts[Index]).SwEqData.FoundUsableTxEq     = FALSE;
    (PciePorts[Index]).MaxPortLaneListLength        = Width;
    for (Lane = 0, LaneIndex = 0; Lane < Width; Lane++) {
      if (LaneIndex < SA_PEG_MAX_LANE) {
        if (LaneReversal == 0) {
          (PciePorts[Index]).MaxPortLaneList[LaneIndex] = (Lane + StartLane);
        } else {
          (PciePorts[Index]).MaxPortLaneList[LaneIndex] = (StartLane - Lane);
        }
      }
      LaneIndex++;
    }

    NextPortLaneOffset = FurcationSetup[PcieController];
    if (LaneReversal == 0) {
      StartLane += NextPortLaneOffset;
    } else {
      StartLane -= NextPortLaneOffset;
    }
  } ///< End of for each port

  return;
}

/**
  Gets the PCIe Capability Structure Pointer

  @param[in]  This                        - Low level function table
  @param[in]  PciePort                    - PCIe Root Port

  @retval Offset to the PCIe Capability Structure
**/
UINT8
EFIAPI
SklPegGetPcieCapOffset (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This,
  IN  PCIE_PORT_INFO                    *PciePort
  )
{
  if (PciePort->Bus == SA_PEG_BUS_NUM && PciePort->Device == SA_PEG_DEV_NUM) {
    return R_SA_PEG_CAPL_OFFSET;
  } else {
    return (UINT8) PcieLibFindCapId (PciePort->Bus, PciePort->Device, PciePort->Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
  }
}

/**
  Programs static equalization settings for the given list of PCIe root ports.
  The PCIE_PORT_EQs structure is laid out such that the Root Port preset for
  PHYSICAL lane number PciePortEqs->PciePort->MaxPortLaneList[0] is
  PciePortEqs->RootPortPresets[0].  Note that physical lane numbers may not
  start at or include zero.  Package pin 0 may not be mapped to a given Root Port

  @param[in]  This                        - Low level function table
  @param[in]  PciePortEqs                 - Array of Root Ports + Eqs to program
  @param[in]  PciePortEqsLength           - Number of Root Ports to program
**/
VOID
EFIAPI
SklProgramStaticGen3Eq (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This,
  IN  PCIE_PORT_EQS                     *PciePortEqs,
  IN  UINT8                             PciePortEqsLength
  )
{
  UINTN         BaseAddress;
  UINT32        OrData;
  UINT8         PortIndex;
  UINT8         BundleIndex;
  UINT8         LaneIndex;

  DEBUG ((DEBUG_INFO, "Programming Static Gen3 Eq\n"));

  for (PortIndex = 0; PortIndex < PciePortEqsLength; PortIndex++) {
    ///
    /// Do bounds checking on input
    ///
    for (LaneIndex = 0;
         LaneIndex < (PciePortEqs[PortIndex]).PciePort->MaxPortLaneListLength;
         LaneIndex++) {
      if ((PciePortEqs[PortIndex]).RootPortPresets[LaneIndex] > 9) {
        (PciePortEqs[PortIndex]).RootPortPresets[LaneIndex] = 8;
      }
      if ((PciePortEqs[PortIndex]).EndpointPresets[LaneIndex] > 9) {
        (PciePortEqs[PortIndex]).EndpointPresets[LaneIndex] = 7;
      }
      if ((PciePortEqs[PortIndex]).EndpointHints[LaneIndex] > 6) {
        (PciePortEqs[PortIndex]).EndpointHints[LaneIndex] = 2;
      }
    }
    BaseAddress = (PciePortEqs[PortIndex]).PciePort->ConfigSpaceBase;

    for (BundleIndex = 0;
         BundleIndex < ((PciePortEqs[PortIndex]).PciePort->MaxPortLaneListLength >> 1);
         BundleIndex++) {
      ///
      /// Compute data to program
      ///
      OrData  = ((PciePortEqs[PortIndex]).EndpointPresets[BundleIndex << 1] << 8);
      OrData |= (UINT32) ((PciePortEqs[PortIndex]).EndpointHints[BundleIndex << 1] << 12);
      OrData |= (UINT32) ((PciePortEqs[PortIndex]).EndpointPresets[ (BundleIndex << 1) + 1] << 24);
      OrData |= (UINT32) ((PciePortEqs[PortIndex]).EndpointHints[ (BundleIndex << 1) + 1] << 28);
      OrData |= (UINT32) (PciePortEqs[PortIndex]).RootPortPresets[BundleIndex << 1];
      OrData |= (UINT32) ((PciePortEqs[PortIndex]).RootPortPresets[ (BundleIndex << 1) + 1] << 16);

      ///
      /// Program Eq Settings
      ///
      MmioAndThenOr32 (BaseAddress + R_SA_PEG_EQCTL0_1_OFFSET + (BundleIndex << 2), 0x80F080F0, OrData);
    }
  }
}

/**
  Sets Gen3 Equalization Phase 2 Bypass for all given Root Ports

  @param[in]  This                        - Low level function table
  @param[in]  PciePorts                   - PCIe Root Ports to program Phase2 for
  @param[in]  PciePortsLength             - Length of the PciePorts array
  @param[in]  BypassPhase2                - TRUE to enable Phase2 bypass, FALSE otherwise

  @retval EFI_SUCCESS     - Phase 2 bypass was successful
  @retval EFI_UNSUPPORTED - Hardware does not support the given Phase2 bypass request
**/
EFI_STATUS
EFIAPI
SklSetPhase2Bypass (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This,
  IN  PCIE_PORT_INFO                    *PciePorts,
  IN  UINT8                             PciePortsLength,
  IN  BOOLEAN                           BypassPhase2
  )
{
  UINT8   PortIndex;

  ///
  /// Set Phase2 Bypass on specified ports
  ///
  for (PortIndex = 0; PortIndex < PciePortsLength; PortIndex++) {
    if (BypassPhase2) {
      MmioOr32 (PciePorts[PortIndex].ConfigSpaceBase + R_SA_PEG_EQCFG_OFFSET, BIT15);
    } else {
      MmioAnd32 (PciePorts[PortIndex].ConfigSpaceBase + R_SA_PEG_EQCFG_OFFSET, (UINT32) ~(BIT15));
    }
  }
  if (BypassPhase2) {
    DEBUG ((DEBUG_INFO, "Set PH2 Bypass.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "Clear PH2 Bypass.\n"));
  }

  return EFI_SUCCESS;
}

/**
  Checks if the Data Link Layer is in DL_Active state on the given root port

  @param[in]  This                        - Low level function table
  @param[in]  PciePorts                   - Root Port to check for VC0 negotiation complete

  @retval TRUE  - Data Link Layer is in DL_Active state
  @retval FALSE - Data Link Layer is NOT in DL_Active state
**/
BOOLEAN
EFIAPI
SklDataLinkLayerLinkActive (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This,
  IN  PCIE_PORT_INFO                    *PciePort
  )
{
  if ((BIT1 & MmioRead16 (PciePort->ConfigSpaceBase + R_SA_PEG_VC0RSTS_OFFSET)) != BIT1) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 This function reports a PCIe controller's link status

  @param[in]  This                        - Low level function table
  @param[in]  PciePort                    - PCIe Root Port
**/
VOID
EFIAPI
SklReportPcieLinkStatus (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This,
  IN  PCIE_PORT_INFO                    *PciePort
  )
{
  DEBUG_CODE_BEGIN ();
  UINT32 Deven;
  UINT16 LinkStatus;
  UINT8  LinkWidth;
  UINT8  LinkSpeed;
  UINT16 Vc0Pending;
  UINTN  PegBaseAddress;
  UINT8  PegBus;
  UINT8  PegDev;
  UINT8  PegFunc;

  PegBus  = PciePort->Bus;
  PegDev  = PciePort->Device;
  PegFunc = PciePort->Function;
  Deven   = MmioRead32 (MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN) + R_SA_DEVEN);
  Deven   = (Deven >> 1) & 0x7;
  DEBUG ((DEBUG_INFO, "PEG%x%x (%x:%x:%x) - ", PegDev, PegFunc, PegBus, PegDev, PegFunc));
  if (((Deven >> (SA_PEG_MAX_FUN - 1 - PegFunc)) & 0x1) == 0x1) {
    PegBaseAddress = PciePort->ConfigSpaceBase;
    LinkStatus     = MmioRead16 (PegBaseAddress + R_SA_PEG_LSTS_OFFSET);
    LinkWidth      = (LinkStatus >> 4) & 0x3F;
    LinkSpeed      = LinkStatus & 0xF;
    Vc0Pending     = ((MmioRead16 (PegBaseAddress + R_SA_PEG_VC0RSTS_OFFSET)) >> 1) & 0x1;
    DEBUG ((DEBUG_INFO, "Trained to x%d at Gen%d.", LinkWidth, LinkSpeed));
    DEBUG ((DEBUG_INFO, " VC0 Negotiation Pending = %d.", Vc0Pending));
    DEBUG ((DEBUG_INFO, "\n"));
  } else {
    DEBUG ((DEBUG_INFO, "Disabled.\n"));
  }
  DEBUG_CODE_END ();
}

/**
 Enable or Disable Polling Compliance Mode

  @param[in]  This                        - Low level function table
  @param[in]  PciePorts                   - PCIe Root Ports
  @param[in]  PciePortsLength             - Length of PciePorts array
  @param[in]  Enable                      - TRUE to enable, FALSE to disable
**/
EFI_STATUS
EFIAPI
SklPollingComplianceMode (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This,
  IN  PCIE_PORT_INFO                    *PciePorts,
  IN  UINT8                             PciePortsLength,
  IN  BOOLEAN                           Enable
  )
{
  UINTN Peg0BaseAddress;

  if (!AllRootPortsSpecified (This, PciePorts, PciePortsLength)) {
    DEBUG ((DEBUG_WARN, "Attempt to set Phase2 bypass on specific RPs detected\n"));
    return EFI_UNSUPPORTED;
  }

  Peg0BaseAddress = MmPciBase (SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM);
  if (Enable) {
    MmioAnd32 (Peg0BaseAddress + R_SA_PEG_REUT_PH_CTR_OFFSET, (UINT32) ~BIT13);
  } else {
    MmioOr32 (Peg0BaseAddress + R_SA_PEG_REUT_PH_CTR_OFFSET, BIT13);
  }

  return EFI_SUCCESS;
}

/**
  Program TxEQs on the endpoint attached to the given root port.

  @param[in]  This                        - Low level function table
  @param[in]  PciePorts                   - PCIe Root Ports
  @param[in]  Presets                     - Array of presets to program per lane
                                            must be of sufficient length to program all lanes
**/
VOID
EFIAPI
SklProgramPortPhase3TxEq (
  IN  PCIE_SI_LOW_LEVEL_FUNCTION_CALLS  *This,
  IN  PCIE_PORT_INFO                    *PciePort,
  IN  UINT8                             *Presets///[PCIE_MAX_LANE]
  )
{
  SA_PCIE_PRIVATE_FUNCTION_CALLS  *PciePrivate;
  UINTN                           Peg0BaseAddress;
  UINT8                           LaneReversal;
  UINT8                           LaneIndex;
  UINT8                           Lane;
  UINT8                           PreCursor;
  UINT8                           Cursor;
  UINT8                           PostCursor;
  UINT8                           FullSwing;

  PciePrivate     = (SA_PCIE_PRIVATE_FUNCTION_CALLS*) This->PrivateData;
  Peg0BaseAddress = MmPciBase (SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM);
  LaneReversal    = (MmioRead32 (Peg0BaseAddress + R_SA_PEG_PEGTST_OFFSET) >> 20) & 0x1;
  if (PciePort->SwEqData.MaxCapableSpeed < 3) {
    DEBUG ((DEBUG_INFO, "PCIe RP (%x:%x:%x) - Link Not Gen3 capable, TxEQ programming.\n",
            PciePort->Bus, PciePort->Device, PciePort->Function));
    return;
  }
  ///
  /// Find first lane of the port for coefficient programming
  ///
  switch (PciePort->Function) {
    default:
    case 0:
      Lane = 0;
      break;
    case 1:
      Lane = 8;
      break;
    case 2:
      Lane = 12;
      break;
  }
  ///
  /// Get FullSwing
  ///
  PciePrivate->GetLinkPartnerFullSwing (This, Lane, &FullSwing);

  for (LaneIndex = 0; LaneIndex < PciePort->SwEqData.ActiveLaneListLength; LaneIndex++) {
    Lane = ReverseLane (PciePort->SwEqData.ActiveLaneList[LaneIndex], LaneReversal);
    ///
    /// Get Coefficients
    ///
    GetCoefficientsFromPreset (Presets[ (PciePort->SwEqData.ActiveLaneList[LaneIndex]) ], FullSwing, &PreCursor, &Cursor, &PostCursor);

    ///
    /// Set Lane's Coefficients
    ///
    PciePrivate->SetPartnerTxCoefficients (This, Lane, &PreCursor, &Cursor, &PostCursor);
  }
  ///
  /// Go to Gen1
  ///
  MmioAndThenOr16 (PciePort->ConfigSpaceBase + R_SA_PEG_LCTL2_OFFSET, (UINT16) ~(0x0F), 1);
  This->RetrainLink (This, PciePort);
  This->WaitForL0 (This, PciePort);
  for (LaneIndex = 0; LaneIndex < PciePort->SwEqData.ActiveLaneListLength; LaneIndex++) {
    Lane = ReverseLane (PciePort->SwEqData.ActiveLaneList[LaneIndex], LaneReversal);
    ///
    /// Set Phase 1 Presets
    ///
    PciePrivate->ProgramPhase1Preset (This, PciePort, 1, Presets[ (PciePort->SwEqData.ActiveLaneList[LaneIndex]) ], Lane);
  }

  ///
  /// Set DOEQ bit and retrain link to go back to Gen3
  ///
  MmioAndThenOr16 (PciePort->ConfigSpaceBase + R_SA_PEG_LCTL2_OFFSET, (UINT16) ~(0x0F), 3);
  MmioOr32 (PciePort->ConfigSpaceBase + R_SA_PEG_LCTL3_OFFSET, BIT0);
  This->RetrainLink (This, PciePort);
}

/**
  This function gets the low level functions for the SA PCIe interface

  @param[in]  PCIE_PEI_PREMEM_CONFIG                   PciePeiPreMemConfig
  @param[out] SaPcieLowLevelFunctionCalls - Table of function calls for SA PEG

  @retval EFI_SUCCESS - Table of function calls returned successfully
**/
EFI_STATUS
GetSklPegLowLevelFunctionCalls (
  IN  PCIE_PEI_PREMEM_CONFIG                         *PciePeiPreMemConfig,
  OUT PCIE_SI_LOW_LEVEL_FUNCTION_CALLS    *SaPcieLowLevelFunctionCalls
  )
{
  EFI_STATUS  Status;

  Status = GetGenericPcieLowLevelFunctionCalls (SaPcieLowLevelFunctionCalls);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Generic Function Overrides
  ///
  SaPcieLowLevelFunctionCalls->DataLinkLayerLinkActive  = SklDataLinkLayerLinkActive;

  ///
  /// Silicon Specific Functions
  ///
  SaPcieLowLevelFunctionCalls->PcieExists               = SklPcieExists;
  SaPcieLowLevelFunctionCalls->GetPcieRootPorts         = SklGetPcieRootPorts;
  SaPcieLowLevelFunctionCalls->GetPcieCapOffset         = SklPegGetPcieCapOffset;
  SaPcieLowLevelFunctionCalls->ProgramStaticGen3Eq      = SklProgramStaticGen3Eq;
  SaPcieLowLevelFunctionCalls->SetPhase2Bypass          = SklSetPhase2Bypass;
  SaPcieLowLevelFunctionCalls->ReportPcieLinkStatus     = SklReportPcieLinkStatus;
  SaPcieLowLevelFunctionCalls->WaitForL0                = SklWaitForL0;
  SaPcieLowLevelFunctionCalls->ResetEndpointPerst       = SklResetEndpointPerst;
  SaPcieLowLevelFunctionCalls->RecoverLinkWidth         = RecoverLinkWidth;
  SaPcieLowLevelFunctionCalls->SetPchGpio               = SetPchGpio;
  SaPcieLowLevelFunctionCalls->OpenMonitor              = SklOpenMonitor;
  SaPcieLowLevelFunctionCalls->CloseMonitor             = SklCloseMonitor;
  SaPcieLowLevelFunctionCalls->GetErrorCount            = SklSaPcieGetErrorCount;
  SaPcieLowLevelFunctionCalls->ClearErrorCount          = SklSaPcieClearErrorCount;
  SaPcieLowLevelFunctionCalls->PollingComplianceMode    = SklPollingComplianceMode;
  SaPcieLowLevelFunctionCalls->ProgramPortPhase3TxEq    = SklProgramPortPhase3TxEq;
  SaPcieLowLevelFunctionCalls->RunMarginTest            = RunMarginTest;
  return EFI_SUCCESS;
}
