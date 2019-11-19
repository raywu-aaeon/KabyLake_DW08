/** @file
    Kabylake DDR4 Rvp17 HSIO PTSS H File

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 - 2018 Intel Corporation.

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

#ifndef KABYLAKE_DDR4_RVP17_HSIO_PTSS_H_
#define KABYLAKE_DDR4_RVP17_HSIO_PTSS_H_

#include <Library/PeiPolicyUpdateLib/PchHsioPtssTables.h>

//BoardId KabylakeDdr4Rvp17
HSIO_PTSS_TABLES PchHHsioPtss_Bx_KabylakeDdr4Rvp17[] = {
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{13, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{18, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{18, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{19, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{19, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{20, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{20, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{21, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{21, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x05000000, (UINT32) ~0x3F000000}, PchSataTopoIsata},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x02000000, (UINT32) ~0x3F000000}, PchSataTopoM2},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoIsata},
  {{23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00040000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoIsata},
  {{24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoFlex},
  {{25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{26, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{26, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{26, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x00000000, (UINT32) ~0x3F000000}, PchSataTopoM2},
  {{26, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoIsata},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoM2},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoIsata},
  {{23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{27, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{27, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{27, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoIsata},
  {{24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoFlex},
  {{25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{28, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{28, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{28, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{26, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoM2},
  {{26, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{27, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{29, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{29, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{29, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{28, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{29, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
};

UINT16 HSIO_PTSS_TABLE_SIZE(PchHHsioPtss_Bx_KabylakeDdr4Rvp17);

//BoardId KabylakeDdr4Rvp17
HSIO_PTSS_TABLES PchHHsioPtss_Dx_KabylakeDdr4Rvp17[] = {
  {{4, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{5, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{6, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{7, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{8, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{9, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{10, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{11, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{12, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{13, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox1},
  {{13, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{18, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{18, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{19, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{19, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{20, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{20, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{21, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopox4},
  {{21, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{22, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x05000000, (UINT32) ~0x3F000000}, PchSataTopoIsata},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x02000000, (UINT32) ~0x3F000000}, PchSataTopoM2},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{23, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoIsata},
  {{23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00040000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{24, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoIsata},
  {{24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{25, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoFlex},
  {{25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{26, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{26, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{26, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x00000000, (UINT32) ~0x3F000000}, PchSataTopoM2},
  {{26, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoIsata},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoM2},
  {{22, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoIsata},
  {{23, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{27, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{27, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{27, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoIsata},
  {{24, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoFlex},
  {{25, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{28, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{28, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{28, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{26, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoM2},
  {{26, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{27, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{29, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00080000, (UINT32) ~0x1F0000}, PchPcieTopoUnknown},
  {{29, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, 0x164, 0x00030000, (UINT32) ~0x1F0000}, PchPcieTopoM2},
  {{29, V_PCH_PCR_FIA_LANE_OWN_SATA, 0x150, 0x04000000, (UINT32) ~0x3F000000}, PchSataTopoUnknown},
  {{28, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
  {{29, V_PCH_PCR_FIA_LANE_OWN_SATA, 0xa0, 0x00000000, (UINT32) ~0x3F3F00}, PchSataTopoUnknown},
};

UINT16 HSIO_PTSS_TABLE_SIZE(PchHHsioPtss_Dx_KabylakeDdr4Rvp17);

#endif // KABYLAKE_DDR4_RVP17_HSIO_PTSS_H_
