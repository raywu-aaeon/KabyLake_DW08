/** @file
  Interface definition details for North Bridge Setup Routines.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2014 Intel Corporation.

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

#ifndef __SA_SETUP_H__  // To Avoid this header get compiled twice
#define __SA_SETUP_H__

#define MAX_TOLUD_DYNAMIC  0
#define MAX_TOLUD_1G       1
#define MAX_TOLUD_1_25G    2
#define MAX_TOLUD_1_5G     3
#define MAX_TOLUD_1_75G    4
#define MAX_TOLUD_2G       5
#define MAX_TOLUD_2_25G    6
#define MAX_TOLUD_2_5G     7
#define MAX_TOLUD_2_75G    8
#define MAX_TOLUD_3G       9
#define MAX_TOLUD_3_25G    10
#define MAX_TOLUD_3_5G     11
//
// SPD Profile
//
#define DEFAULT_SPD_PROFILE 0
#define CUSTOM_PROFILE      1
#define XMP_PROFILE_1       2
#define XMP_PROFILE_2       3

//
// FamilyModel
//
#define CPU_FM_SKL   0x6E0
#define CPU_FM_KBL   0x6E0

//
// ExtendedFamilyModel
//
#define CPU_EXT_FM_SKL_ULT_ULX  4
#define CPU_EXT_FM_SKL_DT_HALO  5
#define CPU_EXT_FM_KBL_ULT_ULX  8
#define CPU_EXT_FM_KBL_DT_HALO  9

//
// ULX/ULT Family Stepping
//
#define CS_SKL_A0     0
#define CS_SKL_B0     1
#define CS_SKL_J0     2
#define CS_SKL_C0     2
#define CS_SKL_K0     3
#define CS_SKL_D0     3
#define CS_SKL_K1     4

//
// Skylake DT/Halo Family Stepping
//
#define CS_SKL_P0     0
#define CS_SKL_Q0     1
#define CS_SKL_M0     2
#define CS_SKL_R0     3
#define CS_SKL_S0     3
#define CS_SKL_N0     4

/// Kabylake ULX/ULT Steppings
///
#define CS_KBL_G0     8
#define CS_KBL_H0     9
#define CS_KBL_J0     9

///
/// Kabylake DT/Halo Steppings
///
#define CS_KBL_A0     8
#define CS_KBL_B0     9
#define CS_KBL_S0     9
#define CS_KBL_M0     9
#define CS_KBL_N0     0xA

///
/// Coffeelake DT Steppings
///
#define CS_CFL_B0     0xB
#define CS_CFL_P0     0xC

#endif
