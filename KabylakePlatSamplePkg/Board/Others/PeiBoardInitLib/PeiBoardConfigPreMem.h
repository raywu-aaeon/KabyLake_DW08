/** @file
  PEI Boards Configurations for PreMem phase.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2018 Intel Corporation.

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

#ifndef PEI_BOARD_CONFIG_PRE_MEM_H_
#define PEI_BOARD_CONFIG_PRE_MEM_H_

#include <PlatformBoardConfig.h>
#include <GpioPinsSklH.h>
#include <ConfigBlock/SwitchableGraphicsConfig.h>
#include <SaAccess.h>

GLOBAL_REMOVE_IF_UNREFERENCED BOARD_CONFIG_BLOCK mSampleBoardConfigPeiPreMem = {
  {{{0, ALIGN16(sizeof(BOARD_CONFIG_BLOCK_PEI_PREMEM)), 0},
    { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }}},
    BOARD_CONFIG_BLOCK_PEI_PREMEM_VERSION, 0, {0, 0}}
  };

//
// The following section contains board-specific CMD/CTL/CLK and DQ/DQS mapping, needed for LPDDR3
//

//
// DQByteMap[0] - ClkDQByteMap:
//   If clock is per rank, program to [0xFF, 0xFF]
//   If clock is shared by 2 ranks, program to [0xFF, 0] or [0, 0xFF]
//   If clock is shared by 2 ranks but does not go to all bytes,
//           Entry[i] defines which DQ bytes Group i services
// DQByteMap[1] - CmdNDQByteMap: Entry[0] is CmdN/CAA and Entry[1] is CmdN/CAB
// DQByteMap[2] - CmdSDQByteMap: Entry[0] is CmdS/CAA and Entry[1] is CmdS/CAB
// DQByteMap[3] - CkeDQByteMap : Entry[0] is CKE /CAA and Entry[1] is CKE /CAB
//                For DDR, DQByteMap[3:1] = [0xFF, 0]
// DQByteMap[4] - CtlDQByteMap : Always program to [0xFF, 0] since we have 1 CTL / rank
//                               Variable only exists to make the code easier to use
// DQByteMap[5] - CmdVDQByteMap: Always program to [0xFF, 0] since we have 1 CA Vref
//                               Variable only exists to make the code easier to use
//

//
// DQ byte mapping to CMD/CTL/CLK, from the CPU side - for SKL RVP5 and RVP16
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqByteMapSklRvp5[2][6][2] = {
  // Channel 0:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  },
  // Channel 1:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  }
};

//
// DQ byte mapping to CMD/CTL/CLK, from the CPU side - for SKL Starbrook
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqByteMapSklStarbrook[2][6][2] = {
  // Channel 0:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0x0F }, // CmdN does not have CAA, CAB goes to Bytes[3:0]
    { 0xF0, 0x0F }, // CmdS CAA goes to Bytes [7:4], CmdS CAB goes to Bytes[3:0]
    { 0xF0, 0x00 }, // CKE CAA goes to Bytes[7:4], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  },
  // Channel 1:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  }
};

//
// DQ byte mapping to CMD/CTL/CLK, from the CPU side - for SKL Panther Mountain
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqByteMapSklPantherMtn[2][6][2] = {
  // Channel 0:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  },
  // Channel 1:
  {
    { 0xCC, 0x33 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0x33 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0xCC, 0x33 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0xCC, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  }
};

//
// DQ byte mapping to CMD/CTL/CLK, from the CPU side - for SKL DOE RVP
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqByteMapSklLpdimmDOE[2][6][2] = {
  // Channel 0:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  },
  // Channel 1:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  }
};

//
// DQS byte swizzling between CPU and DRAM - for SKL DOE RVP
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklLpdimmDOE[2][8] = {
  { 2, 1, 3, 0, 5, 7, 6, 4}, // Channel 0
  { 0, 2, 1, 3, 4, 5, 6, 7}  // Channel 1
};

//
// DQS byte swizzling between CPU and DRAM - for SKL RVP5
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklRvp5[2][8] = {
  { 1, 0, 3, 2, 5, 4, 6, 7 }, // Channel 0
  { 2, 3, 1, 0, 6, 4, 5, 7 }  // Channel 1
};

//
// DQS byte swizzling between CPU and DRAM - for SKL RVP16
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklRvp16[2][8] = {
  { 0, 1, 2, 3, 4, 5, 6, 7 }, // Channel 0
  { 2, 1, 0, 3, 6, 5, 4, 7 }  // Channel 1
};

//
// DQS byte swizzling between CPU and DRAM - for SKL SDS
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklSds[2][8] = {
  { 2, 0, 3, 1, 5, 7, 6, 4 }, // Channel 0
  { 3, 1, 6, 4, 2, 0, 5, 7 }  // Channel 1
};

//
// DQS byte swizzling between CPU and DRAM - for SKL Starbrook
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklStarbrook[2][8] = {
  { 3, 1, 0, 2, 4, 7, 6, 5 }, // Channel 0
  { 0, 3, 2, 1, 6, 5, 4, 7 }  // Channel 1
};
//
// DQS byte swizzling between CPU and DRAM - for SKL Panther Mountain
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklPantherMtn[2][8] = {
  { 0, 3, 1, 2, 4, 7, 6, 5 }, // Channel 0
  { 5, 6, 0, 3, 4, 7, 2, 1 }  // Channel 1
};

//
// DQS byte swizzling between CPU and DRAM - for SKL Phablet
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklPhabletPoc[2][8] = {
  { 1, 2, 0, 3, 5, 6, 4, 7 }, // Channel 0
  { 2, 1, 5, 6, 0, 3, 7, 4 }  // Channel 1
};

//
// DQS byte swizzling between CPU and DRAM - for Karkom
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 DqsMapCpu2DramSklSv[2][8] = {
  { 2, 3, 1, 0, 5, 7, 4, 6 }, // Channel 0
  { 2, 0, 5, 6, 1, 3, 4, 7 }  // Channel 1
};

//
// Reference RCOMP resistors on motherboard - for SKL RVP1
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT16 RcompResistorSklRvp1[SA_MRC_MAX_RCOMP] = { 200, 81, 162 };
//
// RCOMP target values for RdOdt, WrDS, WrDSCmd, WrDSCtl, WrDSClk - for SKL RVP1
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT16 RcompTargetSklRvp1[SA_MRC_MAX_RCOMP_TARGETS] = { 100, 40, 40, 23, 40 };

//
// Reference RCOMP resistors on motherboard - for SKL RVP2
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT16 RcompResistorSklRvp2[SA_MRC_MAX_RCOMP] = { 121, 81, 100 };
//
// RCOMP target values for RdOdt, WrDS, WrDSCmd, WrDSCtl, WrDSClk - for SKL RVP2
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT16 RcompTargetSklRvp2[SA_MRC_MAX_RCOMP_TARGETS] = { 100, 40, 20, 20, 26 };

//
// Reference RCOMP resistors on motherboard - for SKL DT/Halo
//
const UINT16 RcompResistorSklDtHalo[SA_MRC_MAX_RCOMP] = { 121, 75, 100 };
//
// RCOMP target values for RdOdt, WrDS, WrDSCmd, WrDSCtl, WrDSClk - for SKL DT/Halo
//
const UINT16 RcompTargetSklDtHalo[SA_MRC_MAX_RCOMP_TARGETS] = { 60, 26, 20, 20, 26 };

//
// Reference RCOMP resistors on motherboard - for SKL DT Halo with LPDDR3
//
const UINT16 RcompResistorSklDtHaloLpddr3[SA_MRC_MAX_RCOMP] = { 200, 121, 162 };
//
// RCOMP target values for RdOdt, WrDS, WrDSCmd, WrDSCtl, WrDSClk - for SKL DT Halo with LPDDR3
//
const UINT16 RcompTargetSklDtHaloLpddr3[SA_MRC_MAX_RCOMP_TARGETS] = { 100, 40, 40, 23, 40 };

//
// Reference RCOMP resistors on motherboard - for Karkom
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT16 RcompResistorKarkom[SA_MRC_MAX_RCOMP] = { 100, 68, 124 };
//
// RCOMP target values for RdOdt, WrDS, WrDSCmd, WrDSCtl, WrDSClk - for Karkom
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT16 RcompTargetKarkom[SA_MRC_MAX_RCOMP_TARGETS] = { 100, 34, 20, 20, 34 };

#ifdef MEM_DOWN_FLAG
///
/// Memory Down SPD data for Skylake SDS, DRAM type:
///   Hynix H9CCNNNBLTALAR-NTD_QDP LPDDR3, 4Gb die (256Mx16), x16
///   1600, 12-15-15-34
///   2 rank per channel, 2 SDRAMs per rank, 8x4Gb = 4GB total per channel
///
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 SkylakeSdsSpd[] = {
  0x24,                                 ///< 0   Number of Serial PD Bytes Written / SPD Device Size
  0x20,                                 ///< 1   SPD Revision
  0x0F,                                 ///< 2   DRAM Device Type
  0x0E,                                 ///< 3   Module Type
  0x14,                                 ///< 4   SDRAM Density and Banks: 8 Banks, 4 Gb SDRAM density
  0x12,                                 ///< 5   SDRAM Addressing: 14 Rows, 11 Columns
  0xB5,                                 ///< 6   SDRAM Package Type: QDP, 1 Channel per die, Signal Loading Matrix 1
  0x00,                                 ///< 7   SDRAM Optional Features
  0x00,                                 ///< 8   SDRAM Thermal and Refresh Options
  0x00,                                 ///< 9   Other SDRAM Optional Features
  0x00,                                 ///< 10  Reserved - must be coded as 0x00
  0x03,                                 ///< 11  Module Nominal Voltage, VDD
  0x0A,                                 ///< 12  Module Organization, SDRAM width: 16 bits, 2 Ranks
  0x23,                                 ///< 13  Module Memory Bus Width: 2 channels, 64 bit channel bus width
  0x00,                                 ///< 14  Module Thermal Sensor
  0x00,                                 ///< 15  Extended Module Type
  0x00,                                 ///< 16  Reserved - must be coded as 0x00
  0x00,                                 ///< 17  Timebases
  0x0A,                                 ///< 18  SDRAM Minimum Cycle Time (tCKmin)
  0xFF,                                 ///< 19  SDRAM Minimum Cycle Time (tCKmax)
  0x54,                                 ///< 20  CAS Latencies Supported, First Byte (tCk): 12 10 8
  0x00,                                 ///< 21  CAS Latencies Supported, Second Byte
  0x00,                                 ///< 22  CAS Latencies Supported, Third Byte
  0x00,                                 ///< 23  CAS Latencies Supported, Fourth Byte
  0x78,                                 ///< 24  Minimum CAS Latency Time (tAAmin)
  0x00,                                 ///< 25  Read and Write Latency Set Options
  0x90,                                 ///< 26  Minimum RAS# to CAS# Delay Time (tRCDmin)
  0xA8,                                 ///< 27  Minimum Row Precharge Delay Time for all banks (tRPab)
  0x90,                                 ///< 28  Minimum Row Precharge Delay Time per bank (tRPpb)
  0x10,                                 ///< 29  Minimum Refresh Recovery Delay Time for all banks (tRFCab), Least Significant Byte
  0x04,                                 ///< 30  Minimum Refresh Recovery Delay Time for all banks (tRFCab), Most Significant Byte
  0xE0,                                 ///< 31  Minimum Refresh Recovery Delay Time for per bank (tRFCpb), Least Significant Byte
  0x01,                                 ///< 32  Minimum Refresh Recovery Delay Time for per bank (tRFCpb), Most Significant Byte
  0, 0, 0, 0, 0, 0, 0,                  ///< 33 - 39
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 40 - 49
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 50 - 59
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 60 - 69 Connector to SDRAM Bit Mapping
  0, 0, 0, 0, 0, 0, 0, 0,               ///< 70 - 77 Connector to SDRAM Bit Mapping
  0, 0,                                 ///< 78 - 79
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 80 - 89
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 90 - 99
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 100 - 109
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 110 - 119
  0x00,                                 ///< 120 Fine Offset for Minimum Row Precharge Delay Time per bank (tRPpb)
  0x00,                                 ///< 121 Fine Offset for Minimum Row Precharge Delay Time for all banks (tRPab)
  0x00,                                 ///< 122 Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
  0x00,                                 ///< 123 Fine Offset for Minimum CAS Latency Time (tAAmin)
  0x7F,                                 ///< 124 Fine Offset for SDRAM Minimum Cycle Time (tCKmax)
  0x00,                                 ///< 125 Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
  0x00,                                 ///< 126 CRC A
  0x00,                                 ///< 127 CRC B
  0, 0,                                 ///< 128 - 129
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 130 - 139
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 140 - 149
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 150 - 159
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 160 - 169
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 170 - 179
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 180 - 189
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 190 - 199
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 200 - 209
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 210 - 219
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 220 - 229
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 230 - 239
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 240 - 249
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 250 - 259
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 260 - 269
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 270 - 279
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 280 - 289
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 290 - 299
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 300 - 309
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 310 - 319
  0x00,                                 ///< 320 Module Manufacturer ID Code, Least Significant Byte
  0x00,                                 ///< 321 Module Manufacturer ID Code, Most Significant Byte
  0x00,                                 ///< 322 Module Manufacturing Location
  0x00,                                 ///< 323 Module Manufacturing Date Year
  0x00,                                 ///< 324 Module Manufacturing Date Week
  0x55,                                 ///< 325 Module Serial Number A
  0x00,                                 ///< 326 Module Serial Number B
  0x00,                                 ///< 327 Module Serial Number C
  0x00,                                 ///< 328 Module Serial Number D
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 329 - 333 Module Part Number: Unused bytes coded as ASCII Blanks (0x20)
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 334 - 338 Module Part Number
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 339 - 343 Module Part Number
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 344 - 348 Module Part Number
  0x00,                                 ///< 349 Module Revision Code
  0x00,                                 ///< 350 DRAM Manufacturer ID Code, Least Significant Byte
  0x00,                                 ///< 351 DRAM Manufacturer ID Code, Most Significant Byte
  0x00,                                 ///< 352 DRAM Stepping
  0, 0, 0, 0, 0, 0, 0,                  ///< 353 - 359
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 360 - 369
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 370 - 379
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 380 - 389
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 390 - 399
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 400 - 409
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 410 - 419
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 420 - 429
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 430 - 439
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 440 - 449
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 450 - 459
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 460 - 469
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 470 - 479
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 480 - 489
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 490 - 499
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 500 - 509
  0, 0                                  ///< 510 - 511
};

// Samsung K4E6E304ED-EGCF 178b QDP LPDDR3, 4Gb die (256Mx16), x16
// or Hynix H9CCNNNBLTALAR-NUD
// or similar
// 1867, 14-17-17-40
// 2 ranks per channel, 2 SDRAMs per rank, 8x4Gb = 4GB total per channel
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 mSkylakeRvp16Spd[] = {
  0x24,                                 ///< 0   Number of Serial PD Bytes Written / SPD Device Size
  0x20,                                 ///< 1   SPD Revision
  0x0F,                                 ///< 2   DRAM Device Type
  0x0E,                                 ///< 3   Module Type
  0x14,                                 ///< 4   SDRAM Density and Banks: 8 Banks, 4 Gb SDRAM density
  0x12,                                 ///< 5   SDRAM Addressing: 14 Rows, 11 Columns
  0xB5,                                 ///< 6   SDRAM Package Type: QDP, 1 Channel per die, Signal Loading Matrix 1
  0x00,                                 ///< 7   SDRAM Optional Features
  0x00,                                 ///< 8   SDRAM Thermal and Refresh Options
  0x00,                                 ///< 9   Other SDRAM Optional Features
  0x00,                                 ///< 10  Reserved - must be coded as 0x00
  0x03,                                 ///< 11  Module Nominal Voltage, VDD
  0x0A,                                 ///< 12  Module Organization, SDRAM width: 16 bits, 2 Ranks
  0x23,                                 ///< 13  Module Memory Bus Width: 2 channels, 64 bit channel bus width
  0x00,                                 ///< 14  Module Thermal Sensor
  0x00,                                 ///< 15  Extended Module Type
  0x00,                                 ///< 16  Reserved - must be coded as 0x00
  0x00,                                 ///< 17  Timebases
  0x09,                                 ///< 18  SDRAM Minimum Cycle Time (tCKmin): tCKmin = 1.071ns (LPDDR3-1867)
  0xFF,                                 ///< 19  SDRAM Minimum Cycle Time (tCKmax)
  0xD4,                                 ///< 20  CAS Latencies Supported, First Byte (tCK): 14, 12, 10, 8
  0x00,                                 ///< 21  CAS Latencies Supported, Second Byte
  0x00,                                 ///< 22  CAS Latencies Supported, Third Byte
  0x00,                                 ///< 23  CAS Latencies Supported, Fourth Byte
  0x78,                                 ///< 24  Minimum CAS Latency Time (tAAmin) = 14.994 ns
  0x00,                                 ///< 25  Read and Write Latency Set Options
  0x90,                                 ///< 26  Minimum RAS# to CAS# Delay Time (tRCDmin)
  0xA8,                                 ///< 27  Minimum Row Precharge Delay Time for all banks (tRPab)
  0x90,                                 ///< 28  Minimum Row Precharge Delay Time per bank (tRPpb)
  0x10,                                 ///< 29  Minimum Refresh Recovery Delay Time for all banks (tRFCab), Least Significant Byte
  0x04,                                 ///< 30  Minimum Refresh Recovery Delay Time for all banks (tRFCab), Most Significant Byte
  0xE0,                                 ///< 31  Minimum Refresh Recovery Delay Time for per bank (tRFCpb), Least Significant Byte
  0x01,                                 ///< 32  Minimum Refresh Recovery Delay Time for per bank (tRFCpb), Most Significant Byte
  0, 0, 0, 0, 0, 0, 0,                  ///< 33 - 39
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 40 - 49
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 50 - 59
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 60 - 69 Connector to SDRAM Bit Mapping
  0, 0, 0, 0, 0, 0, 0, 0,               ///< 70 - 77 Connector to SDRAM Bit Mapping
  0, 0,                                 ///< 78 - 79
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 80 - 89
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 90 - 99
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 100 - 109
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 110 - 119
  0x00,                                 ///< 120 Fine Offset for Minimum Row Precharge Delay Time per bank (tRPpb)
  0x00,                                 ///< 121 Fine Offset for Minimum Row Precharge Delay Time for all banks (tRPab)
  0x00,                                 ///< 122 Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
  0xFA,                                 ///< 123 Fine Offset for Minimum CAS Latency Time (tAAmin): 14.994 ns (LPDDR3-1867)
  0x7F,                                 ///< 124 Fine Offset for SDRAM Minimum Cycle Time (tCKmax): 32.002 ns
  0xCA,                                 ///< 125 Fine Offset for SDRAM Minimum Cycle Time (tCKmin): 1.071 ns (LPDDR-1867)
  0x00,                                 ///< 126 CRC A
  0x00,                                 ///< 127 CRC B
  0, 0,                                 ///< 128 - 129
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 130 - 139
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 140 - 149
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 150 - 159
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 160 - 169
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 170 - 179
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 180 - 189
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 190 - 199
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 200 - 209
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 210 - 219
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 220 - 229
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 230 - 239
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 240 - 249
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 250 - 259
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 260 - 269
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 270 - 279
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 280 - 289
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 290 - 299
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 300 - 309
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 310 - 319
  0x00,                                 ///< 320 Module Manufacturer ID Code, Least Significant Byte
  0x00,                                 ///< 321 Module Manufacturer ID Code, Most Significant Byte
  0x00,                                 ///< 322 Module Manufacturing Location
  0x00,                                 ///< 323 Module Manufacturing Date Year
  0x00,                                 ///< 324 Module Manufacturing Date Week
  0x55,                                 ///< 325 Module Serial Number A
  0x00,                                 ///< 326 Module Serial Number B
  0x00,                                 ///< 327 Module Serial Number C
  0x00,                                 ///< 328 Module Serial Number D
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 329 - 333 Module Part Number: Unused bytes coded as ASCII Blanks (0x20)
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 334 - 338 Module Part Number
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 339 - 343 Module Part Number
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 344 - 348 Module Part Number
  0x00,                                 ///< 349 Module Revision Code
  0x00,                                 ///< 350 DRAM Manufacturer ID Code, Least Significant Byte
  0x00,                                 ///< 351 DRAM Manufacturer ID Code, Most Significant Byte
  0x00,                                 ///< 352 DRAM Stepping
  0, 0, 0, 0, 0, 0, 0,                  ///< 353 - 359
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 360 - 369
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 370 - 379
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 380 - 389
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 390 - 399
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 400 - 409
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 410 - 419
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 420 - 429
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 430 - 439
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 440 - 449
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 450 - 459
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 460 - 469
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 470 - 479
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 480 - 489
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 490 - 499
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 500 - 509
  0, 0                                  ///< 510 - 511
};

//Hynix H9CCNNNBJTMLAR-NUD, DDP, LPDDR3, 8Gb die
//1867
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 mSkylakeRvp3Spd110[] = {
    0x91,                                 ///< 0   Number of Serial PD Bytes Written / SPD Device Size / CRC Coverage 1, 2
    0x20,                                 ///< 1   SPD Revision
    0xF1,                                 ///< 2   DRAM Device Type
    0x03,                                 ///< 3   Module Type
    0x05,                                 ///< 4   SDRAM Density and Banks, 8Gb
    0x19,                                 ///< 5   SDRAM Addressing: 15 Rows, 10 Columns
    0x05,                                 ///< 6   Module Nominal Voltage
    0x0B,                                 ///< 7   Module Organization: 32 bits, 2 Ranks
    0x03,                                 ///< 8   Module Memory Bus Width
    0x11,                                 ///< 9   Fine Timebase (FTB) Dividend / Divisor
    0x01,                                 ///< 10  Medium Timebase (MTB) Dividend
    0x08,                                 ///< 11  Medium Timebase (MTB) Divisor
    0x09,                                 ///< 12  SDRAM Minimum Cycle Time (tCKmin): tCKmin = 1.071 ns (LPDDR3-1867)
    0x00,                                 ///< 13  Reserved0
    0x50,                                 ///< 14  CAS Latencies supported (tCK): 14, 12, 10, 8 (LSB)
    0x05,                                 ///< 15  CAS Latencies supported (tCK): 14, 12, 10, 8 (LSB)
    0x78,                                 ///< 16  Minimum CAS Latency (tAAmin) = 14.994 ns
    0x78,                                 ///< 17  Minimum Write Recovery Time (tWRmin)
    0x90,                                 ///< 18  Minimum RAS# to CAS# Delay Time (tRCDmin)
    0x50,                                 ///< 19  Minimum Row Active to Row Active Delay Time (tRRDmin)
    0x90,                                 ///< 20  Minimum Row Precharge Delay Time (tRPmin)
    0x11,                                 ///< 21  Upper Nibbles for tRAS and tRC
    0x50,                                 ///< 22  Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte
    0xE0,                                 ///< 23  Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte
    0x90,                                 ///< 24  Minimum Refresh Recovery Delay Time (tRFCmin), Least Significant Byte
    0x06,                                 ///< 25  Minimum Refresh Recovery Delay Time (tRFCmin), Most Significant Byte
    0x3C,                                 ///< 26  Minimum Internal Write to Read Command Delay Time (tWTRmin)
    0x3C,                                 ///< 27  Minimum Internal Read to Precharge Command Delay Time (tRTPmin)
    0x01,                                 ///< 28  Upper Nibble for tFAW
    0x90,                                 ///< 29  Minimum Four Activate Window Delay Time (tFAWmin)
    0x00,                                 ///< 30  SDRAM Optional Features
    0x00,                                 ///< 31  SDRAMThermalAndRefreshOptions
    0x00,                                 ///< 32  ModuleThermalSensor
    0x00,                                 ///< 33  SDRAM Device Type
    0xCA,                                 ///< 34  Fine Offset for SDRAM Minimum Cycle Time (tCKmin): 1.071 ns (LPDDR3-1867)
    0xFA,                                 ///< 35  Fine Offset for Minimum CAS Latency Time (tAAmin): 14.994 ns (LPDDR3-1867)
    0x00,                                 ///< 36  Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
    0x00,                                 ///< 37  Fine Offset for Minimum Row Precharge Delay Time (tRPmin)
    0x00,                                 ///< 38  Fine Offset for Minimum Active to Active/Refresh Delay Time (tRCmin)
    0xA8,                                 ///< 39  Row precharge time for all banks (tRPab)
    0x00,                                 ///< 40  FTB for Row precharge time for all banks (tRPab)
    0, 0, 0, 0, 0, 0, 0, 0, 0,            ///< 41 - 49
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 50 - 59
    0, 0,                                 ///< 60 - 61
    0x00,                                 ///< 62  Reference Raw Card Used
    0x00,                                 ///< 63  Address Mapping from Edge Connector to DRAM
    0x00,                                 ///< 64  ThermalHeatSpreaderSolution
    0, 0, 0, 0, 0,                        ///< 65 - 69
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 70 - 79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 80 - 89
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 90 - 99
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 100 - 109
    0, 0, 0, 0, 0, 0, 0,                  ///< 110 - 116
    0x00,                                 ///< 117 Module Manufacturer ID Code, Least Significant Byte
    0x00,                                 ///< 118 Module Manufacturer ID Code, Most Significant Byte
    0x00,                                 ///< 119 Module Manufacturing Location
    0x00,                                 ///< 120 Module Manufacturing Date Year
    0x00,                                 ///< 121 Module Manufacturing Date creation work week
    0x55,                                 ///< 122 Module Serial Number A
    0x00,                                 ///< 123 Module Serial Number B
    0x00,                                 ///< 124 Module Serial Number C
    0x00,                                 ///< 125 Module Serial Number D
    0x00,                                 ///< 126 CRC A
    0x00                                  ///< 127 CRC B
};
// Elpida EDFB164A1MA-JDF 253b QDP LPDDR3, 8Gb die (256Mx32), x32
// or similar
// 1867, 14-17-17-40
// 2 ranks per channel, 2 SDRAMs per rank, 4x8Gb = 4GB total per channel
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 PantherMountainSpd[] = {
  0x24,                                 ///< 0   Number of Serial PD Bytes Written / SPD Device Size
  0x20,                                 ///< 1   SPD Revision
  0x0F,                                 ///< 2   DRAM Device Type
  0x0E,                                 ///< 3   Module Type
  0x15,                                 ///< 4   SDRAM Density and Banks: 8 Banks, 8 Gb SDRAM density
  0x19,                                 ///< 5   SDRAM Addressing: 15 Rows, 10 Columns
  0xB5,                                 ///< 6   SDRAM Package Type: QDP, 1 Channel per die, Signal Loading Matrix 1
  0x00,                                 ///< 7   SDRAM Optional Features
  0x00,                                 ///< 8   SDRAM Thermal and Refresh Options
  0x00,                                 ///< 9   Other SDRAM Optional Features
  0x00,                                 ///< 10  Reserved - must be coded as 0x00
  0x03,                                 ///< 11  Module Nominal Voltage, VDD
  0x0B,                                 ///< 12  Module Organization, SDRAM width: 32 bits, 2 Ranks
  0x23,                                 ///< 13  Module Memory Bus Width: 2 channels, 64 bit channel bus width
  0x00,                                 ///< 14  Module Thermal Sensor
  0x00,                                 ///< 15  Extended Module Type
  0x00,                                 ///< 16  Reserved - must be coded as 0x00
  0x00,                                 ///< 17  Timebases
  0x09,                                 ///< 18  SDRAM Minimum Cycle Time (tCKmin)
  0xFF,                                 ///< 19  SDRAM Minimum Cycle Time (tCKmax)
  0xD4,                                 ///< 20  CAS Latencies Supported, First Byte
  0x00,                                 ///< 21  CAS Latencies Supported, Second Byte
  0x00,                                 ///< 22  CAS Latencies Supported, Third Byte
  0x00,                                 ///< 23  CAS Latencies Supported, Fourth Byte
  0x78,                                 ///< 24  Minimum CAS Latency Time (tAAmin)
  0x00,                                 ///< 25  Read and Write Latency Set Options
  0x90,                                 ///< 26  Minimum RAS# to CAS# Delay Time (tRCDmin)
  0xA8,                                 ///< 27  Minimum Row Precharge Delay Time for all banks (tRPab)
  0x90,                                 ///< 28  Minimum Row Precharge Delay Time per bank (tRPpb)
  0x10,                                 ///< 29  Minimum Refresh Recovery Delay Time for all banks (tRFCab), Least Significant Byte
  0x04,                                 ///< 30  Minimum Refresh Recovery Delay Time for all banks (tRFCab), Most Significant Byte
  0xE0,                                 ///< 31  Minimum Refresh Recovery Delay Time for per bank (tRFCpb), Least Significant Byte
  0x01,                                 ///< 32  Minimum Refresh Recovery Delay Time for per bank (tRFCpb), Most Significant Byte
  0, 0, 0, 0, 0, 0, 0,                  ///< 33 - 39
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 40 - 49
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 50 - 59
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 60 - 69 Connector to SDRAM Bit Mapping
  0, 0, 0, 0, 0, 0, 0, 0,               ///< 70 - 77 Connector to SDRAM Bit Mapping
  0, 0,                                 ///< 78 - 79
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 80 - 89
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 90 - 99
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 100 - 109
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 110 - 119
  0x00,                                 ///< 120 Fine Offset for Minimum Row Precharge Delay Time per bank (tRPpb)
  0x00,                                 ///< 121 Fine Offset for Minimum Row Precharge Delay Time for all banks (tRPab)
  0x00,                                 ///< 122 Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
  0xFA,                                 ///< 123 Fine Offset for Minimum CAS Latency Time (tAAmin)
  0x7F,                                 ///< 124 Fine Offset for SDRAM Minimum Cycle Time (tCKmax)
  0xCA,                                 ///< 125 Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
  0x00,                                 ///< 126 CRC A
  0x00,                                 ///< 127 CRC B
  0, 0,                                 ///< 128 - 129
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 130 - 139
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 140 - 149
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 150 - 159
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 160 - 169
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 170 - 179
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 180 - 189
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 190 - 199
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 200 - 209
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 210 - 219
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 220 - 229
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 230 - 239
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 240 - 249
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 250 - 259
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 260 - 269
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 270 - 279
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 280 - 289
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 290 - 299
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 300 - 309
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 310 - 319
  0x00,                                 ///< 320 Module Manufacturer ID Code, Least Significant Byte
  0x00,                                 ///< 321 Module Manufacturer ID Code, Most Significant Byte
  0x00,                                 ///< 322 Module Manufacturing Location
  0x00,                                 ///< 323 Module Manufacturing Date Year
  0x00,                                 ///< 324 Module Manufacturing Date Week
  0x55,                                 ///< 325 Module Serial Number A
  0x00,                                 ///< 326 Module Serial Number B
  0x00,                                 ///< 327 Module Serial Number C
  0x00,                                 ///< 328 Module Serial Number D
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 329 - 333 Module Part Number: Unused bytes coded as ASCII Blanks (0x20)
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 334 - 338 Module Part Number
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 339 - 343 Module Part Number
  0x20, 0x20, 0x20, 0x20, 0x20,         ///< 344 - 348 Module Part Number
  0x00,                                 ///< 349 Module Revision Code
  0x00,                                 ///< 350 DRAM Manufacturer ID Code, Least Significant Byte
  0x00,                                 ///< 351 DRAM Manufacturer ID Code, Most Significant Byte
  0x00,                                 ///< 352 DRAM Stepping
  0, 0, 0, 0, 0, 0, 0,                  ///< 353 - 359
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 360 - 369
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 370 - 379
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 380 - 389
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 390 - 399
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 400 - 409
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 410 - 419
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 420 - 429
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 430 - 439
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 440 - 449
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 450 - 459
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 460 - 469
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 470 - 479
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 480 - 489
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 490 - 499
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,         ///< 500 - 509
  0, 0                                  ///< 510 - 511
};
// Hynix H9CCNNNBJTALAR-NVD DDP
// or similar
// 178b DDP LPDDR3, 8Gb die (256Mx32), x32
// 2133, 16-20-20-45
// 2 ranks per channel, 2 SDRAMs per rank, 4x8Gb = 4GB total per channel
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 mAmberLakeLpddr32133Spd[] = {
  0x24, // 512 SPD bytes used, 512 total
  0x01, // SPD Revision 0.1
  0x0F, // DRAM Type: LPDDR3 SDRAM
  0x0E, // Module Type: Non-DIMM Solution
  0x15, // 8 Banks, 8 Gb SDRAM density
  0x19, // 15 Rows, 10 Columns
  0x90, // SDRAM Package Type: DDP, 1 Channel per package
  0x00, // SDRAM Optional Features: none, tMAW = 8192 * tREFI
  0x00, // SDRAM Thermal / Refresh options: none
  0x00, // Other SDRAM Optional Features: none
  0x00, // Reserved
  0x0B, // Module Nominal Voltage, VDD = 1.2v
  0x0B, // SDRAM width: 32 bits, 2 Ranks
  0x03, // SDRAM bus width: 1 Channel, 64 bits channel width
  0x00, // Module Thermal Sensor: none
  0x00, // Extended Module Type: Reserved
  0x00, // Signal Loading: Unspecified
  0x00, // MTB = 0.125ns, FTB = 1 ps
  0x08, // tCKmin = 0.938 ns (LPDDR3-2133)
  0xFF, // tCKmax = 32.002 ns
  0xD4, // CAS Latencies supported (tCK): 16, 14, 12, 10, 8 (First Byte)
  0x01, // CAS Latencies supported (tCK): 16, 14, 12, 10, 8 (Second Byte)
  0x00, // CAS Latencies supported (tCK): 16, 14, 12, 10, 8 (Third Byte)
  0x00, // CAS Latencies supported (tCK): 16, 14, 12, 10, 8 (Fourth Byte)
  0x78, // Minimum CAS Latency (tAAmin) = 15.008 ns
  0x00, // Read and Write Latency Set options: none
  0x90, // Minimum RAS-to-CAS delay (tRCDmin) = 18 ns
  0xA8, // Row precharge time for all banks (tRPab) = 21 ns
  0x90, // Minimum row precharge time (tRPmin) = 18 ns
  0x90, // tRFCab = 210 ns (8 Gb)
  0x06, // tRFCab MSB
  0xD0, // tRFCpb = 90 ns (8 Gb)
  0x02, // tRFCpb MSB
  0, 0, 0, 0, 0, 0, 0,          // Bytes 33-39
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 40-49
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 50-59
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 60-69
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 70-79
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 80-89
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 90-99
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 100-109
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 110-119
  0x00, // FTB for Row precharge time per bank (tRPpb) = 18 ns
  0x00, // FTB for Row precharge time for all banks (tRPab) = 21 ns
  0x00, // FTB for Minimum RAS-to-CAS delay (tRCDmin) = 18 ns
  0x08, // FTB for tAAmin = 15.008 ns (LPDDR3-2133)
  0x7F, // FTB for tCKmax = 32.002 ns
  0xC2, // FTB for tCKmin = 0.938 ns (LPDDR3-2133)
  0, 0, 0, 0,                   // Bytes 126-129
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 130-139
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 140-149
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 150-159
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 160-169
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 170-179
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 180-189
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 190-199
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 200-209
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 210-219
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 220-229
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 230-239
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 240-249
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 250-259
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 260-269
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 270-279
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 280-289
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 290-299
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 300-309
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bytes 310-319
  0, 0, 0, 0, 0,                // Bytes 320-324
  0x55, 0, 0, 0,                // Bytes 325-328: Module ID: Module Serial Number
  0x20, 0x20, 0x20, 0x20, 0x20, // Byte  329-333: Module Part Number: Unused bytes coded as ASCII Blanks (0x20)
  0x20, 0x20, 0x20, 0x20, 0x20, // Bytes 334-338: Module Part Number
  0x20, 0x20, 0x20, 0x20, 0x20, // Bytes 339-343: Module Part Number
  0x20, 0x20, 0x20, 0x20, 0x20, // Bytes 344-348: Module Part Number
  0x00,                         // 349 Module Revision Code
  0x00,                         // 350 DRAM Manufacturer ID Code, Least Significant Byte
  0x00,                         // 351 DRAM Manufacturer ID Code, Most Significant Byte
  0x00,                         // 352 DRAM Stepping
  0, 0, 0, 0, 0, 0, 0,          // 353 - 359
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 360 - 369
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 370 - 379
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 380 - 389
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 390 - 399
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 400 - 409
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 410 - 419
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 420 - 429
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 430 - 439
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 440 - 449
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 450 - 459
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 460 - 469
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 470 - 479
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 480 - 489
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 490 - 499
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 500 - 509
  0, 0                          // 510 - 511
};

#endif

#endif // PEI_BOARD_CONFIG_PRE_MEM_H_
