/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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

#ifndef _SKYLAKE_U_RVP7_HDA_VERB_TABLES_H_
#define _SKYLAKE_U_RVP7_HDA_VERB_TABLES_H_

#include <Ppi/SiPolicy.h>

HDAUDIO_VERB_TABLE HdaVerbTableAlc286Rvp7 = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC286) for RVP7
  //  Revision ID = 0xff
  //  Codec Verb Table for SKL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0286
  //
  0x10EC, 0x0286,
  0xFF, 0xFF,
  //===================================================================================================
  //
  //                               Realtek Semiconductor Corp.
  //
  //===================================================================================================

  //Realtek High Definition Audio Configuration - Version : 5.0.2.9
  //Realtek HD Audio Codec : ALC286
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0286&SUBSYS_10EC1092
  //The number of verb command block : 16

  //    NID 0x12 : 0x411111F0
  //    NID 0x13 : 0x40000000
  //    NID 0x14 : 0x9017011F
  //    NID 0x17 : 0x90170110
  //    NID 0x18 : 0x03A11040
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x411111F0
  //    NID 0x1D : 0x4066A22D
  //    NID 0x1E : 0x411111F0
  //    NID 0x21 : 0x03211020


  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC1092
  0x00172092,
  0x00172110,
  0x001722EC,
  0x00172310,

  //===== Pin Widget Verb-table =====
  //Widget node 0x01 :
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  //Pin widget 0x12 - DMIC
  0x01271CF0,
  0x01271D11,
  0x01271E11,
  0x01271F41,
  //Pin widget 0x13 - DMIC
  0x01371C00,
  0x01371D00,
  0x01371E00,
  0x01371F40,
  //Pin widget 0x14 - SPEAKER-OUT (Port-D)
  0x01771C1F,
  0x01771D01,
  0x01771E17,
  0x01771F90,
  //Pin widget 0x17 - I2S-OUT
  0x01771C10,
  0x01771D01,
  0x01771E17,
  0x01771F90,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C40,
  0x01871D10,
  0x01871EA1,
  0x01871F03,
  //Pin widget 0x19 - I2S-IN
  0x01971CF0,
  0x01971D11,
  0x01971E11,
  0x01971F41,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71CF0,
  0x01A71D11,
  0x01A71E11,
  0x01A71F41,
  //Pin widget 0x1D - PC-BEEP
  0x01D71C2D,
  0x01D71DA2,
  0x01D71E66,
  0x01D71F40,
  //Pin widget 0x1E - S/PDIF-OUT
  0x01E71CF0,
  0x01E71D11,
  0x01E71E11,
  0x01E71F41,
  //Pin widget 0x21 - HP-OUT (Port-A)
  0x02171C20,
  0x02171D10,
  0x02171E21,
  0x02171F03,
  //Widget node 0x20 :
  0x02050071,
  0x02040014,
  0x02050010,
  0x02040C22,
  //Widget node 0x20 - 1 :
  0x0205004F,
  0x02045029,
  0x0205004F,
  0x02045029,
  //Widget node 0x20 - 2 :
  0x0205002B,
  0x02040DD0,
  0x0205002D,
  0x02047020,
  //Widget node 0x20 - 3 :
  0x0205000E,
  0x02046C80,
  0x01771F90,
  0x01771F90,
  //TI AMP settings :
  0x02050022,
  0x0204004C,
  0x02050023,
  0x02040000,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,

  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,

  0x02050022,
  0x0204004C,
  0x02050023,
  0x02040002,
  0x02050025,
  0x02040011,
  0x02050026,
  0x0204B010,

  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,

  0x02050022,
  0x0204004C,
  0x02050023,
  0x0204000D,
  0x02050025,
  0x02040010,
  0x02050026,
  0x0204B010,

  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,

  0x02050022,
  0x0204004C,
  0x02050023,
  0x02040025,
  0x02050025,
  0x02040008,
  0x02050026,
  0x0204B010,

  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,

  0x02050022,
  0x0204004C,
  0x02050023,
  0x02040002,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,

  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,
  0x000F0000,

  0x02050022,
  0x0204004C,
  0x02050023,
  0x02040003,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010
);

#endif // _SKYLAKE_U_RVP7_HDA_VERB_TABLES_H_
