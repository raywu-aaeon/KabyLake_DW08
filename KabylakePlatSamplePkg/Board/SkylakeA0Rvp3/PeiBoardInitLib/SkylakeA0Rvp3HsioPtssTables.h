/** @file
    Skylake A0 Rvp3 HSIO PTSS H File

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

#ifndef SKYLAKE_A0_RVP3_HSIO_PTSS_H_
#define SKYLAKE_A0_RVP3_HSIO_PTSS_H_

#include <Library/PeiPolicyUpdateLib/PchHsioPtssTables.h>

#ifndef HSIO_PTSS_TABLE_SIZE
#define HSIO_PTSS_TABLE_SIZE(A) A##_Size = sizeof (A) / sizeof (HSIO_PTSS_TABLES)
#endif

//BoardId SkylakeA0Rvp3
HSIO_PTSS_TABLES PchLpHsioPtss_Cx_SkylakeA0Rvp3[] = {
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoM2},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoDirectConnect},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00322900, (UINT32) ~0x3F3F00}, PchSataTopoM2},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00382C00, (UINT32) ~0x3F3F00}, PchSataTopoDirectConnect},
  {{15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown}
};

UINT16 HSIO_PTSS_TABLE_SIZE(PchLpHsioPtss_Cx_SkylakeA0Rvp3);

HSIO_PTSS_TABLES PchLpHsioPtss_Bx_SkylakeA0Rvp3[] = {
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoDirectConnect},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00322900, (UINT32) ~0x3F3F00}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00382C00, (UINT32) ~0x3F3F00}, PchSataTopoDirectConnect},
  {{15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
};

UINT16 HSIO_PTSS_TABLE_SIZE(PchLpHsioPtss_Bx_SkylakeA0Rvp3);

#ifndef MINTREE_FLAG
//BoardId SkylakeA0Rvp3Ppv
HSIO_PTSS_TABLES PchLpHsioPtss_Bx_SkylakeA0Rvp3Ppv[] = {
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoDirectConnect},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{12, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00322900, (UINT32) ~0x3F3F00}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00382C00, (UINT32) ~0x3F3F00}, PchSataTopoDirectConnect},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{12, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
};

UINT16 HSIO_PTSS_TABLE_SIZE(PchLpHsioPtss_Bx_SkylakeA0Rvp3Ppv);

//BoardId SkylakeA0Rvp3Ppv
HSIO_PTSS_TABLES PchLpHsioPtss_Cx_SkylakeA0Rvp3Ppv[] = {
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoM2},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoDirectConnect},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{12, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x03000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00322900, (UINT32) ~0x3F3F00}, PchSataTopoM2},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00382C00, (UINT32) ~0x3F3F00}, PchSataTopoDirectConnect},
  {{10, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{15, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00060000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{12, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{14, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown}
};

UINT16 HSIO_PTSS_TABLE_SIZE(PchLpHsioPtss_Cx_SkylakeA0Rvp3Ppv);
#endif //MINTREE_FLAG
#endif // SKYLAKE_A0_RVP3_HSIO_PTSS_H_
