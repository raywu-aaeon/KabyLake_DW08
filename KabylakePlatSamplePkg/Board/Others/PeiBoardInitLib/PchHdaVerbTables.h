/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2018 Intel Corporation.

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

#ifndef _PCH_HDA_VERB_TABLES_H_
#define _PCH_HDA_VERB_TABLES_H_

#include <Ppi/SiPolicy.h>

STATIC HDAUDIO_VERB_TABLE HdaVerbTableDisplayAudio = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable:
  //  Revision ID = 0xFF
  //  Codec Vendor KBL: 0x8086280B
  //
  0x8086, 0x280B,
  0xFF, 0xFF,
  //
  // Display Audio Verb Table
  //
  // Enable the third converter and Pin first (NID 08h)
  0x00878101,
  // Pin Widget 5 - PORT B - Configuration Default: 0x18560010
  0x00571C10,
  0x00571D00,
  0x00571E56,
  0x00571F18,
  // Pin Widget 6 - PORT C - Configuration Default: 0x18560020
  0x00671C20,
  0x00671D00,
  0x00671E56,
  0x00671F18,
  // Pin Widget 7 - PORT D - Configuration Default: 0x18560030
  0x00771C30,
  0x00771D00,
  0x00771E56,
  0x00771F18,
  // Disable the third converter and third Pin (NID 08h)
  0x00878100
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableDisplayAudioSkl = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable:
  //  Revision ID = 0xFF
  //  Codec Vendor SKL: 0x80862809
  //
  0x8086, 0x2809,
  0xFF, 0xFF,
  //
  // Display Audio Verb Table
  //
  // Enable the third converter and Pin first (NID 08h)
  0x00878101,
  // Pin Widget 5 - PORT B - Configuration Default: 0x18560010
  0x00571C10,
  0x00571D00,
  0x00571E56,
  0x00571F18,
  // Pin Widget 6 - PORT C - Configuration Default: 0x18560020
  0x00671C20,
  0x00671D00,
  0x00671E56,
  0x00671F18,
  // Pin Widget 7 - PORT D - Configuration Default: 0x18560030
  0x00771C30,
  0x00771D00,
  0x00771E56,
  0x00771F18,
  // Disable the third converter and third Pin (NID 08h)
  0x00878100
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc298Rvp5 = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC298) for RVP5
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
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_10EC1090
  //The number of verb command block : 17

  //    NID 0x12 : 0x411111F0
  //    NID 0x13 : 0x40000000
  //    NID 0x14 : 0x9017011F
  //    NID 0x17 : 0x90170110
  //    NID 0x18 : 0x03A11040
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x411111F0
  //    NID 0x1D : 0x4065A145
  //    NID 0x1E : 0x411111F0
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x03211020


  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC1090
  0x00172090,
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
  0x01D71C45,
  0x01D71DA1,
  0x01D71E65,
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
  0x02045009,
  0x02050036,
  0x020462C0,
  //Widget node 0x20 - 1 :
  0x0205002B,
  0x02040D10,
  0x0205002D,
  0x02044020,
  //Widget node 0x20 - 2 :
  0x0205000E,
  0x02046F80,
  0x01771F90,
  0x01771F90,
  //Widget node 0x20 - 3 :
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

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc274Rvp9 = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC274)
  //  Revision ID = 0xff
  //  Codec Verb Table for KBL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0274
  //
  0x10EC, 0x0274,
  0xFF, 0xFF,
  //===================================================================================================
  //
  //                               Realtek Semiconductor Corp.
  //
  //===================================================================================================

  //Realtek High Definition Audio Configuration - Version : 5.0.3.0
  //Realtek HD Audio Codec : ALC274
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0274&SUBSYS_10EC10B4
  //The number of verb command block : 16

  //    NID 0x12 : 0x90A601A0
  //    NID 0x13 : 0x40000000
  //    NID 0x14 : 0x411111F0
  //    NID 0x15 : 0x411111F0
  //    NID 0x16 : 0x03011010
  //    NID 0x17 : 0x9017011F
  //    NID 0x18 : 0x411111F0
  //    NID 0x19 : 0x03A11080
  //    NID 0x1A : 0x411111F0
  //    NID 0x1B : 0x03A11070
  //    NID 0x1D : 0x4045A145
  //    NID 0x1E : 0x03451130
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x03211020
  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC10B4
  0x001720B4,
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
  0x01271CA0,
  0x01271D01,
  0x01271D01,
  0x01271EA6,
  0x01271F90,
  //Pin widget 0x13 - DMIC
  0x01371C00,
  0x01371D00,
  0x01371E00,
  0x01371F40,
  //Pin widget 0x14 - SPEAKER-OUT (Port-D)
  0x01471CF0,
  0x01471D11,
  0x01471E11,
  0x01471F41,
  //Pin widget 0x15 - I2S-OUT2
  0x01571CF0,
  0x01571D11,
  0x01571E11,
  0x01571F41,
  //Pin widget 0x16 - LINE3 (Port-B)
  0x01671C10,
  0x01671D10,
  0x01671E01,
  0x01671F03,
  //Pin widget 0x17 - I2S-OUT
  0x01771C1F,
  0x01771D01,
  0x01771E17,
  0x01771F90,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871CF0,
  0x01871D11,
  0x01871E11,
  0x01871F41,
  //Pin widget 0x19 - MIC2
  0x01971C80,
  0x01971D10,
  0x01971EA1,
  0x01971F03,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71CF0,
  0x01A71D11,
  0x01A71E11,
  0x01A71F41,
  //Pin widget 0x1B - LINE2 (Port-E)
  0x01B71C70,
  0x01B71D10,
  0x01B71EA1,
  0x01B71F03,
  //Pin widget 0x1D - PC-BEEP
  0x01D71C45,
  0x01D71DA1,
  0x01D71E45,
  0x01D71F40,
  //Pin widget 0x1E - S/PDIF-OUT
  0x01E71C30,
  0x01E71D11,
  0x01E71E45,
  0x01E71F03,
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
  //Widget node 0x20 : Do Headphone Fake JD
  0x02050008,
  0x0204A80F,
  0x02050008,
  0x0204A80F,
  //Widget node 0x20 :
  0x02050045,
  0x02045089,
  0x0205004A,
  0x0204201B,
  //Widget node 0x20 - 1 :
  0x02050035,
  0x02048968,
  0x05B50001,
  0x05B48540,
  //Widget node 0x20 - 2 :
  0x05850000,
  0x05843888,
  0x0205006B,
  0x02044278,
  //Widget node 0x20 - 3 : set no AUX mode
  0x0205006F,
  0x02042C0B,
  0x0205001A,
  0x02048F0B,
  //Widget node 0x20 - 4 : FSM reset
  0x0205006F,
  0x02042C2B,
  0x0205006F,
  0x02042C0B,
  //Widget node 0x20 -5 : Headphone Fake JD release
  0x02050008,
  0x0204A807,
  0x02050008,
  0x0204A807,
  //Widget node 0X20 for ALC1305
  //1
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040000,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //2
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204005D,
  0x02050029,
  0x0204B024,
  //3
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040442,
  0x02050029,
  0x0204B024,
  //4
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040004,
  0x02050028,
  0x02040500,
  0x02050029,
  0x0204B024,
  //5
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040004,
  0x02050028,
  0x02040500,
  0x02050029,
  0x0204B024,
  //6
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040006,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //7
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040008,
  0x02050028,
  0x0204B000,
  0x02050029,
  0x0204B024,
  //8
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204002E,
  0x02050028,
  0x02040800,
  0x02050029,
  0x0204B024,
  //9
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x020400C3,
  0x02050029,
  0x0204B024,
  //10
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x0204D4A0,
  0x02050029,
  0x0204B024,
  //11
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x020400CC,
  0x02050029,
  0x0204B024,
  //12
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x0204400A,
  0x02050029,
  0x0204B024,
  //13
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x020400C1,
  0x02050029,
  0x0204B024,
  //14
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040320,
  0x02050029,
  0x0204B024,
  //15
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040039,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //16
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003B,
  0x02050028,
  0x0204FFFF,
  0x02050029,
  0x0204B024,
  //17
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003C,
  0x02050028,
  0x0204FC20,
  0x02050029,
  0x0204B024,
  //18
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040006,
  0x02050029,
  0x0204B024,
  //19
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x020400C0,
  0x02050029,
  0x0204B024,
  //20
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003C,
  0x02050028,
  0x0204FCA0,
  0x02050029,
  0x0204B024,
  //21
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003C,
  0x02050028,
  0x0204FCE0,
  0x02050029,
  0x0204B024,
  //22
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003C,
  0x02050028,
  0x0204FCF0,
  0x02050029,
  0x0204B024,
  //23
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040080,
  0x02050028,
  0x02040080,
  0x02050029,
  0x0204B024,
  //24
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040080,
  0x02050028,
  0x02040880,
  0x02050029,
  0x0204B024,
  //25
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003C,
  0x02050028,
  0x0204FCE0,
  0x02050029,
  0x0204B024,
  //26
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003C,
  0x02050028,
  0x0204FCA0,
  0x02050029,
  0x0204B024,
  //27
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003C,
  0x02050028,
  0x0204FC20,
  0x02050029,
  0x0204B024,
  //28
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //29
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040080,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //30
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040062,
  0x02050028,
  0x0204C000,
  0x02050029,
  0x0204B024,
  //31
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040063,
  0x02050028,
  0x02045F5F,
  0x02050029,
  0x0204B024,
  //32
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040064,
  0x02050028,
  0x02041100,
  0x02050029,
  0x0204B024,
  //33
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040065,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //34
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040066,
  0x02050028,
  0x02044004,
  0x02050029,
  0x0204B024,
  //35
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040067,
  0x02050028,
  0x02040702,
  0x02050029,
  0x0204B024,
  //36
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040068,
  0x02050028,
  0x0204890F,
  0x02050029,
  0x0204B024,
  //37
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040069,
  0x02050028,
  0x0204E021,
  0x02050029,
  0x0204B024,
  //38
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040078,
  0x02050029,
  0x0204B024,
  //39
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x0204F726,
  0x02050029,
  0x0204B024,
  //40
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040079,
  0x02050029,
  0x0204B024,
  //41
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x0204F726,
  0x02050029,
  0x0204B024,
  //42
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204007A,
  0x02050029,
  0x0204B024,
  //43
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x0204C249,
  0x02050029,
  0x0204B024,
  //44
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204007B,
  0x02050029,
  0x0204B024,
  //45
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x0204C249,
  0x02050029,
  0x0204B024,
  //46
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204007C,
  0x02050029,
  0x0204B024,
  //47
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02041E06,
  0x02050029,
  0x0204B024,
  //48
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204007D,
  0x02050029,
  0x0204B024,
  //49
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02041E06,
  0x02050029,
  0x0204B024,
  //50
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040068,
  0x02050029,
  0x0204B024,
  //51
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //52
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040069,
  0x02050029,
  0x0204B024,
  //53
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //54
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204006A,
  0x02050029,
  0x0204B024,
  //55
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02041EA9,
  0x02050029,
  0x0204B024,
  //56
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204006B,
  0x02050029,
  0x0204B024,
  //57
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040079,
  0x02050029,
  0x0204B024,
  //58
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204004B,
  0x02050028,
  0x02049800,
  0x02050029,
  0x0204B024,
  //59
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040040,
  0x02050028,
  0x0204000F,
  0x02050029,
  0x0204B024,
  //60
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040070,
  0x02050028,
  0x02048012,
  0x02050029,
  0x0204B024,
  //61
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040071,
  0x02050028,
  0x02043570,
  0x02050029,
  0x0204B024,
  //62
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040072,
  0x02050028,
  0x02040123,
  0x02050029,
  0x0204B024,
  //63
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040073,
  0x02050028,
  0x02045665,
  0x02050029,
  0x0204B024,
  //64
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040074,
  0x02050028,
  0x02044320,
  0x02050029,
  0x0204B024,
  //65
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040075,
  0x02050028,
  0x02046543,
  0x02050029,
  0x0204B024,
  //66
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040076,
  0x02050028,
  0x02042F00,
  0x02050029,
  0x0204B024,
  //67
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040050,
  0x02050029,
  0x0204B024,
  //68
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040064,
  0x02050029,
  0x0204B024,
  //69
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040051,
  0x02050029,
  0x0204B024,
  //70
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040770,
  0x02050029,
  0x0204B024,
  //71
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040052,
  0x02050029,
  0x0204B024,
  //72
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x0204C30C,
  0x02050029,
  0x0204B024,
  //73
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204005E,
  0x02050029,
  0x0204B024,
  //74
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040040,
  0x02050029,
  0x0204B024,
  //75
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204004F,
  0x02050029,
  0x0204B024,
  //76
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040000,
  0x02050029,
  0x0204B024,
  //77
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x02040273,
  0x02050029,
  0x0204B024,
  //78
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204003A,
  0x02050028,
  0x02041DFE,
  0x02050029,
  0x0204B024,
  //79
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204005E,
  0x02050029,
  0x0204B024,
  //80
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02040020,
  0x02050029,
  0x0204B024,
  //81
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040050,
  0x02050028,
  0x020492CC,
  0x02050029,
  0x0204B024,
  //82
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040051,
  0x02050028,
  0x02044707,
  0x02050029,
  0x0204B024,
  //83
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040052,
  0x02050028,
  0x020440F0,
  0x02050029,
  0x0204B024,
  //84
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040018,
  0x02050028,
  0x02040219,
  0x02050029,
  0x0204B024,
  //85
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040050,
  0x02050028,
  0x020412CC,
  0x02050029,
  0x0204B024,
  //86
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006A,
  0x02050028,
  0x0204005D,
  0x02050029,
  0x0204B024,
  //87
  0x02050024,
  0x02040010,
  0x02050026,
  0x0204006C,
  0x02050028,
  0x02049142,
  0x02050029,
  0x0204B024,
  //88
  0x02050024,
  0x02040010,
  0x02050026,
  0x02040060,
  0x02050028,
  0x0204C273,
  0x02050029,
  0x0204B024
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc898Rvp8 = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC898) for RVP8
  //  Revision ID = 0xff
  //  Codec Verb Table for SKL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0899
  //
  0x10EC, 0x0899,
  0xFF, 0xFF,
  //===================================================================================================
  //
  //                               Realtek Semiconductor Corp.
  //
  //===================================================================================================

  //Realtek High Definition Audio Configuration - Version : 5.0.3.0
  //Realtek HD Audio Codec : ALC898
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0899&SUBSYS_80861092
  //The number of verb command block : 18

  //    NID 0x11 : 0x4017C000
  //    NID 0x12 : 0x411111F0
  //    NID 0x14 : 0x01014010
  //    NID 0x15 : 0x01011012
  //    NID 0x16 : 0x01016011
  //    NID 0x17 : 0x411111F0
  //    NID 0x18 : 0x01A19040
  //    NID 0x19 : 0x02A19050
  //    NID 0x1A : 0x0181304F
  //    NID 0x1B : 0x02214020
  //    NID 0x1C : 0x411111F0
  //    NID 0x1D : 0x4027F641
  //    NID 0x1E : 0x01441130
  //    NID 0x1F : 0x411111F0


  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x80861092
  0x00172092,
  0x00172110,
  0x00172286,
  0x00172380,

  //===== Pin Widget Verb-table =====
  //Widget node 0x01 :
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  //Pin widget 0x11 - S/PDIF-OUT2
  0x01171C00,
  0x01171DC0,
  0x01171E17,
  0x01171F40,
  //Pin widget 0x12 - DMIC
  0x01271CF0,
  0x01271D11,
  0x01271E11,
  0x01271F41,
  //Pin widget 0x14 - FRONT (Port-D)
  0x01471C10,
  0x01471D40,
  0x01471E01,
  0x01471F01,
  //Pin widget 0x15 - SURR (Port-A)
// AMI_OVERRIDE_START - EIP329748  RVP8's VerbTable (Realtek HD Audio Codec : ALC898) is not correct and it would cause HLK test - KS Topology Test Fail.
//  0x01571C12,
  0x01571C14,
// AMI_OVERRIDE_END - EIP329748  RVP8's VerbTable (Realtek HD Audio Codec : ALC898) is not correct and it would cause HLK test - KS Topology Test Fail.  
  0x01571D10,
  0x01571E01,
  0x01571F01,
  //Pin widget 0x16 - CEN/LFE (Port-G)
  0x01671C11,
  0x01671D60,
  0x01671E01,
  0x01671F01,
  //Pin widget 0x17 - SIDESURR (Port-H)
  0x01771CF0,
  0x01771D11,
  0x01771E11,
  0x01771F41,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C40,
  0x01871D90,
  0x01871EA1,
  0x01871F01,
  //Pin widget 0x19 - MIC2 (Port-F)
  0x01971C50,
  0x01971D90,
  0x01971EA1,
  0x01971F02,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71C4F,
  0x01A71D30,
  0x01A71E81,
  0x01A71F01,
  //Pin widget 0x1B - LINE2 (Port-E)
  0x01B71C20,
  0x01B71D40,
  0x01B71E21,
  0x01B71F02,
  //Pin widget 0x1C - CD-IN
  0x01C71CF0,
  0x01C71D11,
  0x01C71E11,
  0x01C71F41,
  //Pin widget 0x1D - BEEP-IN
  0x01D71C41,
  0x01D71DF6,
  0x01D71E27,
  0x01D71F40,
  //Pin widget 0x1E - S/PDIF-OUT1
  0x01E71C30,
  0x01E71D11,
  0x01E71E44,
  0x01E71F01,
  //Pin widget 0x1F - S/PDIF-IN
  0x01F71CF0,
  0x01F71D11,
  0x01F71E11,
  0x01F71F41,
  //Widget node 0x20 :
  0x02050007,
  0x02040180,
  0x02050001,
  0x0204C7AA,
  //Widget node 0x20 - 1 :
  0x02050015,
  0x0204026A,
  0x02050008,
  0x02040071
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc298Rvp10 = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC298) for RVP10
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
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_80861096
  //The number of verb command block : 17

  //    NID 0x12 : 0x411111F0
  //    NID 0x13 : 0x40000000
  //    NID 0x14 : 0x9017011F
  //    NID 0x17 : 0x90170110
  //    NID 0x18 : 0x03A11060
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x03811040
  //    NID 0x1D : 0x40638105
  //    NID 0x1E : 0x03451130
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x03211020


  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x80861096
  0x00172096,
  0x00172110,
  0x00172286,
  0x00172380,

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
  0x01871C60,
  0x01871D10,
  0x01871EA1,
  0x01871F03,
  //Pin widget 0x19 - I2S-IN
  0x01971CF0,
  0x01971D11,
  0x01971E11,
  0x01971F41,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71C40,
  0x01A71D10,
  0x01A71E81,
  0x01A71F03,
  //Pin widget 0x1D - PC-BEEP
  0x01D71C05,
  0x01D71D81,
  0x01D71E63,
  0x01D71F40,
  //Pin widget 0x1E - S/PDIF-OUT
  0x01E71C30,
  0x01E71D11,
  0x01E71E45,
  0x01E71F03,
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
  0x02045009,
  0x02050036,
  0x020462C0,
  //Widget node 0x20 - 1 :
  0x0205002B,
  0x02040D10,
  0x0205002D,
  0x02044020,
  //Widget node 0x20 - 2 :
  0x0205000E,
  0x02046F80,
  0x01771F90,
  0x01771F90,
  //Widget node 0x20 - 3 :
  0x02050079,
  0x02046800,
  0x02050080,
  0x02040431,
  //TI AMP settings 0x4C:
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
  0x0204B010,

  //TI AMP settings 0x4D :
  0x02050022,
  0x0204004D,
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
  0x0204004D,
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
  0x0204004D,
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
  0x0204004D,
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
  0x0204004D,
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
  0x0204004D,
  0x02050023,
  0x02040003,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc298Sds = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC298) for SDS
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
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_10EC1076
  //The number of verb command block : 17

  //    NID 0x12 : 0x411111F0
  //    NID 0x13 : 0x400C0000
  //    NID 0x14 : 0x9017011F
  //    NID 0x17 : 0x90170110
  //    NID 0x18 : 0x03A11030
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x0381103F
  //    NID 0x1D : 0x40638205
  //    NID 0x1E : 0x411111F0
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x03011020


  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC1076
  0x00172076,
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
  0x01371E0C,
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
  0x01871C30,
  0x01871D10,
  0x01871EA1,
  0x01871F03,
  //Pin widget 0x19 - I2S-IN
  0x01971CF0,
  0x01971D11,
  0x01971E11,
  0x01971F41,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71C3F,
  0x01A71D10,
  0x01A71E81,
  0x01A71F03,
  //Pin widget 0x1D - PC-BEEP
  0x01D71C05,
  0x01D71D82,
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
  0x02171E01,
  0x02171F03,
  //Widget node 0x20 :
  0x0205004F,
  0x02045009,
  0x02050036,
  0x020462C0,
  //Widget node 0x20 - 1 :
  0x0205002B,
  0x02040D10,
  0x0205002D,
  0x02044020,
  //Widget node 0x20 - 2 :
  0x0205000E,
  0x02046F80,
  0x01771F90,
  0x01771F90,
  //Widget node 0x20 - 3 :
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

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc298PantherMtn = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC298) for PantherMtn
  //  Revision ID = 0xff
  //  Codec Verb Table for Panther Mountain and StarBrook
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

  //Realtek High Definition Audio Configuration - Version : 5.0.2.5
  //Realtek HD Audio Codec : ALC298
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_10EC1078
  //The number of verb command block : 13

  //    NID 0x12 : 0x90A60130
  //    NID 0x13 : 0x40000000
  //    NID 0x14 : 0x90170110
  //    NID 0x17 : 0x411111F0
  //    NID 0x18 : 0x03A19040
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x411111F0
  //    NID 0x1D : 0x4066862D
  //    NID 0x1E : 0x411111F0
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x03214020


  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC1078
  0x00172078,
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
  0x01271C30,
  0x01271D01,
  0x01271EA6,
  0x01271F90,
  //Pin widget 0x13 - DMIC
  0x01371C00,
  0x01371D00,
  0x01371E00,
  0x01371F40,
  //Pin widget 0x14 - SPEAKER-OUT (Port-D)
  0x01471C10,
  0x01471D01,
  0x01471E17,
  0x01471F90,
  //Pin widget 0x17 - I2S-OUT
  0x01771CF0,
  0x01771D11,
  0x01771E11,
  0x01771F41,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C40,
  0x01871D90,
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
  0x01D71D86,
  0x01D71E66,
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
  0x02171D40,
  0x02171E21,
  0x02171F03,
  //Widget node 0x20 :
  0x02050019,
  0x02040217,
  0x02050001,
  0x0204ADA8,
  //Widget node 0x20 - 1 :
  0x02050002,
  0x02048EB5,
  0x02050034,
  0x02045610,
  //Widget node 0x20 - 2 :
  0x02050035,
  0x02041AA4,
  0x0205008F,
  0x02041000,
  //Widget node 0x20 - 3 :
  0x0205004F,
  0x02045009,
  0x02050008,
  0x0204200F,
  //Widget node 0x20 - 4 :
  0x0205006D,
  0x0204C908,
  0x0205006D,
  0x0204C908
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc298GrizzlyMtn = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC298) for GrizzlyMtn
  //  Revision ID = 0xff
  //  Codec Verb Table for Grizzly Mountain
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

  //Realtek High Definition Audio Configuration - Version : 5.0.3.0
  //Realtek HD Audio Codec : ALC298
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_10EC10AC
  //The number of verb command block : 14

  //    NID 0x12 : 0x40000000
  //    NID 0x12 : 0x40000000
  //    NID 0x13 : 0x411111F0
  //    NID 0x14 : 0x411111F0
  //    NID 0x17 : 0x90170110
  //    NID 0x18 : 0x03A11030
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x411111F0
  //    NID 0x1D : 0x4045A605
  //    NID 0x1E : 0x411111F0
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x03211020

  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC10AC
  0x001720AC,
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
  0x01271C00,
  0x01271D00,
  0x01271E00,
  0x01271F40,
  //Pin widget 0x13 - DMIC
  0x01371CF0,
  0x01371D11,
  0x01371E11,
  0x01371F41,
  //Pin widget 0x14 - SPEAKER-OUT (Port-D)
  0x01471CF0,
  0x01471D11,
  0x01471E11,
  0x01471F41,
  //Pin widget 0x17 - I2S-OUT
  0x01771C10,
  0x01771D01,
  0x01771E17,
  0x01771F90,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C30,
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
  0x01D71C05,
  0x01D71DA6,
  0x01D71E45,
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
  0x02045009,
  0x0205004F,
  0x02045009,
  //Widget node 0x20 : mono spk
  0x0205000C,
  0x0204011F,
  0x0205000D,
  0x02042810
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc286S = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC286S)
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

  //Realtek High Definition Audio Configuration - Version : 5.0.2.1
  //Realtek HD Audio Codec : ALC286
  //PCI PnP ID : PCI\VEN_0000&DEV_0000&SUBSYS_00000000
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0286&SUBSYS_00000000
  //The number of verb command block : 15

  //    NID 0x12 : 0x90A60130
  //    NID 0x13 : 0x40000000
  //    NID 0x14 : 0x90170110
  //    NID 0x17 : 0x411111F0
  //    NID 0x18 : 0x03A19020
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x0381302F
  //    NID 0x1D : 0x4066832D
  //    NID 0x1E : 0x411111F0
  //    NID 0x21 : 0x0321101F

  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x00000000
  0x00172000,
  0x00172100,
  0x00172200,
  0x00172300,

  //===== Pin Widget Verb-table =====
  //Widget node 0x01 :
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  //Pin widget 0x12 - DMIC
  0x01271C40,
  0x01271D01,
  0x01271EA6,
  0x01271F90,
  //Pin widget 0x13 - DMIC
  0x01371C00,
  0x01371D00,
  0x01371E00,
  0x01371F40,
  //Pin widget 0x14 - SPEAKER-OUT (Port-D)
  0x01471C10,
  0x01471D01,
  0x01471E17,
  0x01471F90,
  //Pin widget 0x17 - I2S-OUT
  0x01771CF0,
  0x01771D11,
  0x01771E11,
  0x01771F41,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C30,
  0x01871D90,
  0x01871EA1,
  0x01871F03,
  //Pin widget 0x19 - I2S-IN
  0x01971CF0,
  0x01971D11,
  0x01971E11,
  0x01971F41,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71C3F,
  0x01A71D30,
  0x01A71E81,
  0x01A71F03,
  //Pin widget 0x1D - PC-BEEP
  0x01D71C2D,
  0x01D71D23,
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
  0x02050051,
  0x02045428,
  //Widget node 0x20 - 2 :
  0x0205002B,
  0x02040C50,
  0x0205002D,
  0x02041020,
  // New verbs from Realtek
  0x02050063,
  0x02042906
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc298 = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC298)
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

  //Realtek High Definition Audio Configuration - Version : 5.0.3.1
  //Realtek HD Audio Codec : ALC298
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID :INTELAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_10EC11DC
  //The number of verb command block : 15

  //    NID 0x12 : 0x90A60130
  //    NID 0x13 : 0x40000000
  //    NID 0x14 : 0x90170110
  //    NID 0x17 : 0x411111F0
  //    NID 0x18 : 0x04A11040
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x411111F0
  //    NID 0x1D : 0x40600001
  //    NID 0x1E : 0x411111F0
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x04211020

  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC11DC
  0x001720DC,
  0x00172111,
  0x001722EC,
  0x00172310,
  //===== Pin Widget Verb-table =====
  //Widget node 0x01 :
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  //Pin widget 0x12 - DMIC
  0x01271C30,
  0x01271D01,
  0x01271EA6,
  0x01271F90,
  //Pin widget 0x13 - DMIC
  0x01371C00,
  0x01371D00,
  0x01371E00,
  0x01371F40,
  //Pin widget 0x14 - SPEAKER-OUT (Port-D)
  0x01471C10,
  0x01471D01,
  0x01471E17,
  0x01471F90,
  //Pin widget 0x17 - I2S-OUT
  0x01771CF0,
  0x01771D11,
  0x01771E11,
  0x01771F41,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C40,
  0x01871D10,
  0x01871EA1,
  0x01871F04,
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
  0x01D71C01,
  0x01D71D00,
  0x01D71E60,
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
  0x02171F04,
  //Widget node 0x20 : global headset // internal speaker 1.9W/4ohm
  0x0205004F,
  0x02045009,
  0x0205006D,
  0x02048908,
  //Widget node 0x20-1 : 1 pin detect 2 port and if Pin8 floating its need internal pull high . This case had external pull high for Pin8
  0x02050062,
  0x0204AFE7,
  0x02050064,
  0x02047F01,
  //Widget node 0x20-2 : Set Pin55 to MCLK.
  0x0205002F,
  0x0204004C,
  0x02050019,
  0x02040217,
  //Widget node 0x20 - 3 :  I2S in SRC + Nonauto detect
  0x0205002B,
  0x02040D10,
  0x0205002D,
  0x02044020,
  //Widget node 0x20 - 4 :  LDO3 = 1.3V  I2S out config
  0x0205000E,
  0x02046F80,
  0x01771F00,
  0x01771F00,
  //Widget node 0x20 - 5 :HD-A parameter dynamic control  ¡AI2SOUT need to set this command  Set Pin52 as cmos EAPD
  0x02050080,
  0x02045C52,
  0x02050033,
  0x02040204,
  //Widget node 0x20 - 6 :  To increase I2S Interface, EAPD and DMIC driving current
  0x02050036,
  0x02047BC0,
  0x02050079,
  0x02047BC0
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc888S = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC888S)
  //  Revision ID = 0xFF
  //  Codec Verb Table for SKL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0888
  //
  0x10EC, 0x0888,
  0xFF, 0xFF,
  //===================================================================================================
  //
  //                               Realtek Semiconductor Corp.
  //
  //===================================================================================================

  //Realtek High Definition Audio Configuration - Version : 5.0.2.1
  //Realtek HD Audio Codec : ALC888S-VD
  //PCI PnP ID : PCI\VEN_0000&DEV_0000&SUBSYS_00000000
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0888&SUBSYS_00000000
  //The number of verb command block : 17

  //    NID 0x11 : 0x40000000
  //    NID 0x12 : 0x411111F0
  //    NID 0x14 : 0x01014010
  //    NID 0x15 : 0x01011012
  //    NID 0x16 : 0x01016014
  //    NID 0x17 : 0x01012011
  //    NID 0x18 : 0x01A19030
  //    NID 0x19 : 0x02A19031
  //    NID 0x1A : 0x0181303F
  //    NID 0x1B : 0x0221401F
  //    NID 0x1C : 0x411111F0
  //    NID 0x1D : 0x4026DE01
  //    NID 0x1E : 0x01441120
  //    NID 0x1F : 0x411111F0

  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x00000000
  0x00172000,
  0x00172100,
  0x00172200,
  0x00172300,

  //===== Pin Widget Verb-table =====
  //Widget node 0x01 :
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  0x0017FF00,
  //Pin widget 0x11 - S/PDIF-OUT2
  0x01171C00,
  0x01171D00,
  0x01171E00,
  0x01171F40,
  //Pin widget 0x12 - DMIC
  0x01271CF0,
  0x01271D11,
  0x01271E11,
  0x01271F41,
  //Pin widget 0x14 - FRONT (Port-D)
  0x01471C10,
  0x01471D40,
  0x01471E01,
  0x01471F01,
  //Pin widget 0x15 - SURR (Port-A)
  0x01571C12,
  0x01571D10,
  0x01571E01,
  0x01571F01,
  //Pin widget 0x16 - CEN/LFE (Port-G)
  0x01671C14,
  0x01671D60,
  0x01671E01,
  0x01671F01,
  //Pin widget 0x17 - SIDESURR (Port-H)
  0x01771C11,
  0x01771D20,
  0x01771E01,
  0x01771F01,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C30,
  0x01871D90,
  0x01871EA1,
  0x01871F01,
  //Pin widget 0x19 - MIC2 (Port-F)
  0x01971C31,
  0x01971D90,
  0x01971EA1,
  0x01971F02,
  //Pin widget 0x1A - LINE1 (Port-C)
  0x01A71C3F,
  0x01A71D30,
  0x01A71E81,
  0x01A71F01,
  //Pin widget 0x1B - LINE2 (Port-E)
  0x01B71C1F,
  0x01B71D40,
  0x01B71E21,
  0x01B71F02,
  //Pin widget 0x1C - CD-IN
  0x01C71CF0,
  0x01C71D11,
  0x01C71E11,
  0x01C71F41,
  //Pin widget 0x1D - BEEP-IN
  0x01D71C01,
  0x01D71DDE,
  0x01D71E26,
  0x01D71F40,
  //Pin widget 0x1E - S/PDIF-OUT1
  0x01E71C20,
  0x01E71D11,
  0x01E71E44,
  0x01E71F01,
  //Pin widget 0x1F - S/PDIF-IN
  0x01F71CF0,
  0x01F71D11,
  0x01F71E11,
  0x01F71F41,
  //Widget node 0x20 :
  0x02050007,
  0x020409C8,
  0x02050007,
  0x020409C8
);

STATIC HDAUDIO_VERB_TABLE HdaVerbTableAlc298KblR = HDAUDIO_VERB_TABLE_INIT (
  //
  //  VerbTable: (Realtek ALC298)
  //  Revision ID = 0xff
  //  Codec Verb Table for SKL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0286
  //
  0x10EC, 0x0298,
  0xFF, 0xFF,
  //===================================================================================================
  //
  //                               Realtek Semiconductor Corp.
  //
  //===================================================================================================

  //Realtek High Definition Audio Configuration - Version : 5.0.3.1
  //Realtek HD Audio Codec : ALC298
  //PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
  //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_10EC1120
  //The number of verb command block : 14

  //    NID 0x12 : 0x411111F0
  //    NID 0x13 : 0x40000000
  //    NID 0x14 : 0x90170120
  //    NID 0x17 : 0x411111F0
  //    NID 0x18 : 0x04A11030
  //    NID 0x19 : 0x411111F0
  //    NID 0x1A : 0x411111F0
  //    NID 0x1D : 0x4067A62D
  //    NID 0x1E : 0x411111F0
  //    NID 0x1F : 0x411111F0
  //    NID 0x21 : 0x04211010

  //===== HDA Codec Subsystem ID Verb-table =====
  //HDA Codec Subsystem ID  : 0x10EC1120
  0x00172020,
  0x00172111,
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
  0x01471C20,
  0x01471D01,
  0x01471E17,
  0x01471F90,
  //Pin widget 0x17 - I2S-OUT
  0x01771CF0,
  0x01771D11,
  0x01771E11,
  0x01771F41,
  //Pin widget 0x18 - MIC1 (Port-B)
  0x01871C30,
  0x01871D10,
  0x01871EA1,
  0x01871F04,
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
  0x01D71DA6,
  0x01D71E67,
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
  0x02171C10,
  0x02171D10,
  0x02171E21,
  0x02171F04,
  //Widget node 0x20 : global headset and set ALC298 Class to 2W/4ohm
  0x0205004F,
  0x02045009,
  0x0205006D,
  0x02047908,
  //Widget node 0x20-1 : 1 pin detect 2 port and if Pin8 floating its need internal pull high . This case had external pull high for JD1
  0x02050062,
  0x0204AFE7,
  0x02050064,
  0x02047F01,
  //Widget node 0x20-2 : Set Pin55 to MCLK.
  0x0205002F,
  0x0204004C,
  0x02050019,
  0x02040217,
  //Widget node 0x20 - 3 :  I2S in SRC + Nonauto detect
  0x0205002B,
  0x02040D10,
  0x0205002D,
  0x02044020,
  //Widget node 0x20 - 4 :  LDO3 = 1.3V  I2S out config
  0x0205000E,
  0x02046F80,
  0x01771F00,
  0x01771F00,
  //Widget node 0x20 - 5 :HD-A parameter dynamic control  AI2SOUT need to set this command  Set Pin52 as dedicate input(PD pin)
  0x02050080,
  0x02045C52,
  0x02050033,
  0x02040208,
  //Widget node 0x20 - 6 :  To increase I2S Interface, EAPD and DMIC driving current
  0x02050036,
  0x02047BC0,
  0x02050079,
  0x02047BC0,
  //Widget node 0x20 - 7 :  To set MIC2_VREFO_L for headset sleeve and ring2
  0x02050050,
  0x02042000,
  0x02050050,
  0x02042000,
  //Widget node 0x20 - 7 : TI AMP initial
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040000,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040002,
  0x02050025,
  0x02040011,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040001,
  0x02050025,
  0x02040011,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040025,
  0x02050025,
  0x0204007E,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x0204000D,
  0x02050025,
  0x02040010,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040014,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040015,
  0x02050025,
  0x02040010,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040016,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040017,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040018,
  0x02050025,
  0x02040001,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x0204001B,
  0x02050025,
  0x02040001,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x0204001C,
  0x02050025,
  0x0204000F,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x0204001D,
  0x02050025,
  0x02040003,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x0204001E,
  0x02050025,
  0x02040007,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040022,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040023,
  0x02050025,
  0x02040004,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040024,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040002,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010,
  0x02050022,
  0x0204009A,
  0x02050023,
  0x02040003,
  0x02050025,
  0x02040000,
  0x02050026,
  0x0204B010
);

#endif // _PCH_HDA_VERB_TABLES_H_
