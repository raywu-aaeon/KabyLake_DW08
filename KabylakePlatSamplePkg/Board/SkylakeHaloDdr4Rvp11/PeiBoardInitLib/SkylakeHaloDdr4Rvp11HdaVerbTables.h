/** @file
  HDA Verb table for Skylake Halo DDR4 RVP11

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

#ifndef _SKYLAKE_HALO_DDR4_RVP11_HDA_VERB_TABLES_H_
#define _SKYLAKE_HALO_DDR4_RVP11_HDA_VERB_TABLES_H_

#include <Ppi/SiPolicy.h>

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc298Rvp11 = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC298) for RVP11
  //  Revision ID = 0xff
  //  Codec Verb Table for SKL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0298
  //
  0x10EC, 0x0298,
  0xFF, 0xFF,
  //===================================================================================================
  //
  //                               Realtek Semiconductor Corp.
  //
  //===================================================================================================

  //Realtek High Definition Audio Configuration - Version : 5.0.2.9
  //Realtek HD Audio Codec : ALC298
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_109C8086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_10EC109C
  //The number of verb command block : 19

  //    NID 0x12 : 0x411111F0
  //    NID 0x13 : 0x40040000
  //    NID 0x14 : 0x9017011F
  //    NID 0x17 : 0x90170110
  //    NID 0x18 : 0x03A11050
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x03811030
  //    NID 0x1D : 0x40638105
  //    NID 0x1E : 0x411111F0
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x03211020


  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC109C
  0x0017209C,
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
  0x01371E04,
  0x01371F40,
  //Pin widget 0x14 - SPEAKER-OUT (Port-D)
  0x01471C1F,
  0x01471D01,
  0x01471E17,
  0x01471F90,
  //Pin widget 0x17 - I2S-OUT
  0x01771C10,
  0x01771D01,
  0x01771E17,
  0x01771F90,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C50,
  0x01871D10,
  0x01871EA1,
  0x01871F03,
  //Pin widget 0x19 - I2S-IN
  0x01971CF0,
  0x01971D11,
  0x01971E11,
  0x01971F41,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71C30,
  0x01A71D10,
  0x01A71E81,
  0x01A71F03,
  //Pin widget 0x1D - PC-BEEP
  0x01D71C05,
  0x01D71D81,
  0x01D71E63,
  0x01D71F40,
  //Pin widget 0x1E - S/PDIF-OUT
  0x01E71CF0,
  0x01E71D11,
  0x01E71E11,
  0x01E71F41,
  //Pin widget 0x1F - S/PDIF-IN
  0x01F71CF0,
  0x01F71D11,
  0x01F71E11,
  0x01F71F41,
  //Pin widget 0x21 - HP-OUT (Port-A)
  0x02171C20,
  0x02171D10,
  0x02171E21,
  0x02171F03,
  //Widget node 0x20 :
  0x0205004F,
  0x0204B009,
  0x02050050,
  0x02041000,
  //Widget node 0x20 - 1 :
  0x02050019,
  0x02040017,
  0x02050020,
  0x02040002,
  //Widget node 0x20 - 2 :
  0x0205008F,
  0x02041008,
  0x02050036,
  0x020462C0,
  //Widget node 0x20 - 3 :
  0x0205002B,
  0x02040D10,
  0x0205002D,
  0x02044020,
  //Widget node 0x20 - 4 :
  0x0205000E,
  0x02046F80,
  0x01771F90,
  0x01771F90,
  //Widget node 0x20 - 5 :
  0x02050079,
  0x02046800,
  0x02050079,
  0x02046800,
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
  0x0204000F,
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

#endif // _SKYLAKE_HALO_DDR4_RVP11_HDA_VERB_TABLES_H_
