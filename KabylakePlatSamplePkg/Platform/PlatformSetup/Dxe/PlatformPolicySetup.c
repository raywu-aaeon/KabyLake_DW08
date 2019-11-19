/** @file
  Platform Policy Initialization Driver.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2018 Intel Corporation.

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

#include "PlatformSetup.h"
#include <OemSetup.h>
#include <MemInfoHob.h>
#include <PlatformInfo.h>

/**
  Populate MEM_INFO_PROTOCOL from the Memory Info Data HOB

  @param[in, out] MemoryInfoProtocol          Pointer to MEM_INFO_PROTOCOL
**/
VOID
SetupPlatformPolicies (
  IN OUT  MEM_INFO_PROTOCOL *MemoryInfoProtocol
  )
{
  UINT8                     Node;
  UINT8                     Channel;
  UINT8                     Dimm;
  MEMORY_INFO_DATA_HOB      *MemInfo;
  MEMORY_INFO_DATA          *MemInfoData;
  CONTROLLER_INFO           *ControllerInfo;
  CHANNEL_INFO              *ChannelInfo;
  DIMM_INFO                 *DimmInfo;
  MEMORY_TIMING             *MemoryTiming;
  MRC_CH_TIMING             *Timing;
  UINT8                     Profile;
  UINT32                    Index;
  EFI_HOB_GUID_TYPE         *GuidHob;

  MemInfo = NULL;
  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  GuidHob = NULL;
  GuidHob = GetFirstGuidHob (&gSiMemoryInfoDataGuid);
//AMI_OVERRIDE_START - Remove for ASSERT in Capsule mode
#if 0
  ASSERT (GuidHob != NULL);
#endif  
//AMI_OVERRIDE_END - Remove for ASSERT in Capsule mode  
  if (GuidHob == NULL) {
    return;
  }

  MemInfo = (MEMORY_INFO_DATA_HOB *) GET_GUID_HOB_DATA (GuidHob);

  MemInfoData                   = &MemoryInfoProtocol->MemInfoData;
  MemInfoData->Profile          = MemInfo->MemoryProfile;
  MemInfoData->XmpProfileEnable = MemInfo->XmpProfileEnable;
  MemInfoData->memSize          = (UINT32) (MemInfo->TotalPhysicalMemorySize);
  MemInfoData->ddrFreq          = (UINT16) MemInfo->ConfiguredMemoryClockSpeed;
  MemInfoData->ddrFreqMax       = (UINT16) MemInfo->MaximumMemoryClockSpeed;
  MemInfoData->EccSupport       = MemInfo->EccSupport;
  MemInfoData->Ratio            = MemInfo->Ratio;
  MemInfoData->RefClk           = (UINT8) (MemInfo->RefClk + 1);
  MemInfoData->DdrType          = MemInfo->MemoryType;

  for (Profile = 0; Profile < PROFILE_NUM; Profile++) {
    MemInfoData->VddVoltage[Profile]  = (UINT16) MemInfo->VddVoltage[Profile];
    if (Profile < XMP_PROFILE_NUM) {
      // Store the tCK value read from SPD XMP profiles.
      // Use Profile as the logical array indecies for XMP1 and XMP2.
      MemInfoData->DefaultXmptCK[Profile] = MemInfo->DefaultXmptCK[Profile];
    }
  }

  for (Node = 0; Node < NODE_NUM; Node++) {
    ControllerInfo = &MemInfo->Controller[Node];

    for (Channel = 0; Channel < CH_NUM; Channel++) {
      ChannelInfo = &ControllerInfo->ChannelInfo[Channel];
      if (ChannelInfo->Status) {
        //
        // Updating memory timings to MemInfoProtocol from MemInfoHob for each memory profile.
        //
        for (Profile = 0; Profile < PROFILE_NUM; Profile++) {
          MemoryTiming          = &MemInfoData->Timing[Profile];
          Timing                = &ChannelInfo->Timing[Profile];
          MemoryTiming->tCK     = Timing->tCK;
          MemoryTiming->tCL     = Timing->tCL;
          MemoryTiming->tCWL    = Timing->tCWL;
          MemoryTiming->tFAW    = Timing->tFAW;
          MemoryTiming->tRAS    = Timing->tRAS;
          MemoryTiming->tRCDtRP = Timing->tRCDtRP;
          MemoryTiming->tREFI   = Timing->tREFI;
          MemoryTiming->tRFC    = Timing->tRFC;
          MemoryTiming->tRPab   = Timing->tRPab;
          MemoryTiming->tRRD    = Timing->tRRD;
          MemoryTiming->tRTP    = Timing->tRTP;
          MemoryTiming->tWR     = Timing->tWR;
          MemoryTiming->tWTR    = Timing->tWTR;
          MemoryTiming->NMode   = (UINT8) Timing->NMode;
          MemInfoData->tRRD_S[Profile] = Timing->tRRD_S;
          MemInfoData->tRRD_L[Profile] = Timing->tRRD_L;
          MemInfoData->tWTR_S[Profile] = Timing->tWTR_S;
          MemInfoData->tWTR_L[Profile] = Timing->tWTR_L;
        }
        if (Channel == 0) {
          MemInfoData->tRd2Rd.SG = ChannelInfo->tRd2Rd.SG;
          MemInfoData->tRd2Rd.DG = ChannelInfo->tRd2Rd.DG;
          MemInfoData->tRd2Rd.DR = ChannelInfo->tRd2Rd.DR;
          MemInfoData->tRd2Rd.DD = ChannelInfo->tRd2Rd.DD;
          MemInfoData->tRd2Wr.SG = ChannelInfo->tRd2Wr.SG;
          MemInfoData->tRd2Wr.DG = ChannelInfo->tRd2Wr.DG;
          MemInfoData->tRd2Wr.DR = ChannelInfo->tRd2Wr.DR;
          MemInfoData->tRd2Wr.DD = ChannelInfo->tRd2Wr.DD;
          MemInfoData->tWr2Rd.SG = ChannelInfo->tWr2Rd.SG;
          MemInfoData->tWr2Rd.DG = ChannelInfo->tWr2Rd.DG;
          MemInfoData->tWr2Rd.DR = ChannelInfo->tWr2Rd.DR;
          MemInfoData->tWr2Rd.DD = ChannelInfo->tWr2Rd.DD;
          MemInfoData->tWr2Wr.SG = ChannelInfo->tWr2Wr.SG;
          MemInfoData->tWr2Wr.DG = ChannelInfo->tWr2Wr.DG;
          MemInfoData->tWr2Wr.DR = ChannelInfo->tWr2Wr.DR;
          MemInfoData->tWr2Wr.DD = ChannelInfo->tWr2Wr.DD;
        } else {
          MemInfoData->tRd2Rd_Ch1.SG = ChannelInfo->tRd2Rd.SG;
          MemInfoData->tRd2Rd_Ch1.DG = ChannelInfo->tRd2Rd.DG;
          MemInfoData->tRd2Rd_Ch1.DR = ChannelInfo->tRd2Rd.DR;
          MemInfoData->tRd2Rd_Ch1.DD = ChannelInfo->tRd2Rd.DD;
          MemInfoData->tRd2Wr_Ch1.SG = ChannelInfo->tRd2Wr.SG;
          MemInfoData->tRd2Wr_Ch1.DG = ChannelInfo->tRd2Wr.DG;
          MemInfoData->tRd2Wr_Ch1.DR = ChannelInfo->tRd2Wr.DR;
          MemInfoData->tRd2Wr_Ch1.DD = ChannelInfo->tRd2Wr.DD;
          MemInfoData->tWr2Rd_Ch1.SG = ChannelInfo->tWr2Rd.SG;
          MemInfoData->tWr2Rd_Ch1.DG = ChannelInfo->tWr2Rd.DG;
          MemInfoData->tWr2Rd_Ch1.DR = ChannelInfo->tWr2Rd.DR;
          MemInfoData->tWr2Rd_Ch1.DD = ChannelInfo->tWr2Rd.DD;
          MemInfoData->tWr2Wr_Ch1.SG = ChannelInfo->tWr2Wr.SG;
          MemInfoData->tWr2Wr_Ch1.DG = ChannelInfo->tWr2Wr.DG;
          MemInfoData->tWr2Wr_Ch1.DR = ChannelInfo->tWr2Wr.DR;
          MemInfoData->tWr2Wr_Ch1.DD = ChannelInfo->tWr2Wr.DD;
        }
      }
      for (Dimm = 0; Dimm < DIMM_NUM; Dimm++) {
        Index = (Node << 2) + (Channel << 1) + Dimm;
        DimmInfo = &ChannelInfo->DimmInfo[Dimm];
        MemInfoData->dimmSize[Index] = (UINT16) DimmInfo->DimmCapacity;
        MemInfoData->DimmStatus[Index] = DimmInfo->Status;
        MemInfoData->RankInDimm[Index] = DimmInfo->RankInDimm;
        /**
          Get pointer of SPD data read from MRC for SMBIOS driver to report w/o reading SMBus again.
          MRC only provides SMBIOS data and Manufacturing data after first boot is saved to NVRAM.
        **/
        MemInfoData->DimmsSpdData[Index] = (DimmInfo->Status < DIMM_NOT_PRESENT) ? (UINT8 *) &(DimmInfo->SpdSave) : NULL;
      }
    }
  }

  return;
}
