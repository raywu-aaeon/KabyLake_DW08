/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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

#include "PeiSkylakeHaloLpddr3Rvp16InitLib.h"

#include "SkylakeHaloLpddr3Rvp16HsioPtssTables.h"

#include <PlatformBoardConfig.h>                    // for USB 20 AFE & Root Port Clk Info

//
// Share with RVP3 related definition.
//
extern UINT8 mSkylakeRvp16Spd[];
extern UINT8 mSkylakeRvp3Spd110[];
extern UINT16 mSkylakeRvp16SpdSize;
extern UINT16 mSkylakeRvp3Spd110Size;
extern UINT8 DqByteMapSklRvp5[];
extern UINT16 DqByteMapSklRvp5Size;
extern UINT8 mSpdLpddr32133[];
extern UINT16 mSpdLpddr32133Size;

//
// RVP16 depends on RVP11 config
//
EFI_STATUS
PeiSkylakeHaloDdr4Rvp11CommonInitPreMem (
  IN VOID *Content
  );

//
// Reference RCOMP resistors on motherboard - for SKL DT Halo with LPDDR3
//
const UINT16 RcompResistorSklDtHaloLpddr3[SA_MRC_MAX_RCOMP] = { 200, 121, 162 };
//
// RCOMP target values for RdOdt, WrDS, WrDSCmd, WrDSCtl, WrDSClk - for SKL DT Halo with LPDDR3
//
const UINT16 RcompTargetSklDtHaloLpddr3[SA_MRC_MAX_RCOMP_TARGETS] = { 100, 40, 40, 23, 40 };

//
// DQS byte swizzling between CPU and DRAM - for SKL RVP16
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklRvp16[2][8] = {
  { 0, 1, 2, 3, 4, 5, 6, 7 }, // Channel 0
  { 2, 1, 0, 3, 6, 5, 4, 7 }  // Channel 1
};

//
// RVP13 Init Table
//
BOARD_INIT_TABLE mRvp16BoardInitPreMem[] = {
  {BoardIdSkylakeHaloLpddr3Rvp16, PeiSkylakeHaloLpddr3Rvp16InitPreMem},
  { 0, 0 }
};


/**
  SkylaeHaloLpDdr3Rvp16 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeHaloLpddr3Rvp16InitPreMem (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;

  Status = PeiSkylakeHaloDdr4Rvp11CommonInitPreMem (Content);  // depends on RVP11.

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformInfo = (PLATFORM_INFO *) Content;

  PcdSet8S (PcdSaMiscUserBd, 5); // MrcBoardType btUser4 for ULT platform

  PcdSetBoolS (PcdMobileDramPresent, TRUE);

  PcdSet8S (PcdMrcSpdAddressTable0, 0xA2);
  PcdSet8S (PcdMrcSpdAddressTable1, 0xA0);
  PcdSet8S (PcdMrcSpdAddressTable2, 0xA2);
  PcdSet8S (PcdMrcSpdAddressTable3, 0xA0);

  PcdSet32S (PcdMrcDqByteMap, (UINTN) DqByteMapSklRvp5);
  PcdSet16S (PcdMrcDqByteMapSize, DqByteMapSklRvp5Size);
  PcdSet32S (PcdMrcDqsMapCpu2Dram, (UINTN) DqsMapCpu2DramSklRvp16);
  PcdSet16S (PcdMrcDqsMapCpu2DramSize, sizeof (DqsMapCpu2DramSklRvp16));

  PcdSet32S (PcdMrcRcompResistor, (UINTN) RcompResistorSklDtHaloLpddr3);
  PcdSet32S (PcdMrcRcompTarget, (UINTN) RcompTargetSklDtHaloLpddr3);

#ifdef MEM_DOWN_FLAG
  //
  // No SPD_PRSNT jumper on RVP16.
  // BOM ID = 0 means solder down (SPD in BIOS), BOM ID = 1 - socketed.
  // Solder down - 1867 QDP; Socketed - 2133 QDP
  //
  if (PlatformInfo->BoardBomId == 0x0) {
    PcdSet8S (PcdMrcSpdAddressTable0, 0);
    PcdSet8S (PcdMrcSpdAddressTable1, 0);
    PcdSet8S (PcdMrcSpdAddressTable2, 0);
    PcdSet8S (PcdMrcSpdAddressTable3, 0);
    PcdSet32S (PcdMrcSpdData, (UINTN) mSkylakeRvp16Spd);
    PcdSet16S (PcdMrcSpdDataSize, mSkylakeRvp16SpdSize);
  }
  //
  // BOM ID = 2 means solder down (SPD in BIOS), BOM ID = 3 - socketed.
  // Solder down - 1866 DDP; Socketed - 2133 DDP
  //
  if (PlatformInfo->BoardBomId == 0x2) {
    PcdSet8S(PcdMrcSpdAddressTable0, 0);
    PcdSet8S(PcdMrcSpdAddressTable1, 0);
    PcdSet8S(PcdMrcSpdAddressTable2, 0);
    PcdSet8S(PcdMrcSpdAddressTable3, 0);
    PcdSet32S(PcdMrcSpdData, (UINTN)mSkylakeRvp3Spd110);
    PcdSet16S(PcdMrcSpdDataSize, mSkylakeRvp3Spd110Size);
  }

  if (PlatformInfo->BoardBomId == 0x4) {
    PcdSet8S(PcdMrcSpdAddressTable0, 0);
    PcdSet8S(PcdMrcSpdAddressTable1, 0);
    PcdSet8S(PcdMrcSpdAddressTable2, 0);
    PcdSet8S(PcdMrcSpdAddressTable3, 0);
    PcdSet32S(PcdMrcSpdData, (UINTN)mSpdLpddr32133);
    PcdSet16S(PcdMrcSpdDataSize,mSpdLpddr32133Size);
  }
#endif

  PcdSet32S (PcdSpecificHHsioPtssTable1, (UINTN) PchHHsioPtss_Bx_SkylakeHaloLpddr3Rvp16);
  PcdSet16S (PcdSpecificHHsioPtssTable1Size, PchHHsioPtss_Bx_SkylakeHaloLpddr3Rvp16_Size);
  PcdSet32S (PcdSpecificHHsioPtssTable2, (UINTN) PchHHsioPtss_Dx_SkylakeHaloLpddr3Rvp16);
  PcdSet16S (PcdSpecificHHsioPtssTable2Size, PchHHsioPtss_Dx_SkylakeHaloLpddr3Rvp16_Size);

  return EFI_SUCCESS;
}

/**
  The constructor function for SkylakeA0Rvp3 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiSkylakeHaloLpddr3Rvp16BoardInitPreMemLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp16BoardInitPreMem, TABLE_LIST_NUM);
}

