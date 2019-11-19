/** @file
  This protocol provides the memory information data, such as
  total physical memory size, memory frequency, memory size
  of each dimm and rank.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _MEM_INFO_PROTOCOL_H_
#define _MEM_INFO_PROTOCOL_H_


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gMemInfoProtocolGuid;

//
// Protocol definitions
//
#define NODE_NUM  1
#define CH_NUM    2
#define DIMM_NUM  2
#define RANK_NUM  2
#define SLOT_NUM  (CH_NUM * DIMM_NUM)
#define PROFILE_NUM 4 // number of memory profiles supported
#define XMP_PROFILE_NUM 2 // number of XMP profiles supported

//
// Matches MrcDdrType enum in MRC
//
#ifndef MRC_DDR_TYPE_DDR4
#define MRC_DDR_TYPE_DDR4     0
#endif
#ifndef MRC_DDR_TYPE_DDR3
#define MRC_DDR_TYPE_DDR3     1
#endif
#ifndef MRC_DDR_TYPE_LPDDR3
#define MRC_DDR_TYPE_LPDDR3   2
#endif
#ifndef MRC_DDR_TYPE_UNKNOWN
#define MRC_DDR_TYPE_UNKNOWN  3
#endif

//
// Matches MrcDimmSts enum in MRC
//
#ifndef DIMM_ENABLED
#define DIMM_ENABLED     0  // DIMM/rank Pair is enabled, presence will be detected.
#endif
#ifndef DIMM_DISABLED
#define DIMM_DISABLED    1  // DIMM/rank Pair is disabled, regardless of presence.
#endif
#ifndef DIMM_PRESENT
#define DIMM_PRESENT     2  // There is a DIMM present in the slot/rank pair and it will be used.
#endif
#ifndef DIMM_NOT_PRESENT
#define DIMM_NOT_PRESENT 3  // There is no DIMM present in the slot/rank pair.
#endif

#pragma pack(1)
///
/// Memory timing Structure
///
typedef struct {
  UINT32 tCK;     ///< Offset 0 Memory cycle time, in femtoseconds.
  UINT16 NMode;   ///< Offset 4 Number of tCK cycles for the channel DIMM's command rate mode.
  UINT16 tCL;     ///< Offset 6 Number of tCK cycles for the channel DIMM's CAS latency.
  UINT16 tCWL;    ///< Offset 8 Number of tCK cycles for the channel DIMM's minimum CAS write latency time.
  UINT16 tFAW;    ///< Offset 10 Number of tCK cycles for the channel DIMM's minimum four activate window delay time.
  UINT16 tRAS;    ///< Offset 12 Number of tCK cycles for the channel DIMM's minimum active to precharge delay time.
  UINT16 tRCDtRP; ///< Offset 14 Number of tCK cycles for the channel DIMM's minimum RAS# to CAS# delay time and Row Precharge delay time
  UINT16 tREFI;   ///< Offset 16 Number of tCK cycles for the channel DIMM's minimum Average Periodic Refresh Interval.
  UINT16 tRFC;    ///< Offset 18 Number of tCK cycles for the channel DIMM's minimum refresh recovery delay time.
  UINT16 tRPab;   ///< Offset 20 Number of tCK cycles for the channel DIMM's minimum row precharge delay time for all banks.
  UINT16 tRRD;    ///< Offset 22 Number of tCK cycles for the channel DIMM's minimum row active to row active delay time.
  UINT16 tRTP;    ///< Offset 24 Number of tCK cycles for the channel DIMM's minimum internal read to precharge command delay time.
  UINT16 tWR;     ///< Offset 26 Number of tCK cycles for the channel DIMM's minimum write recovery time.
  UINT16 tWTR;    ///< Offset 28 Number of tCK cycles for the channel DIMM's minimum internal write to read command delay time.
  UINT8  Rsvd[2]; ///< Offset 30
} MEMORY_TIMING;

typedef struct {
  UINT8 SG;       ///< Number of tCK cycles between transactions in the same bank group.
  UINT8 DG;       ///< Number of tCK cycles between transactions when switching bank groups.
  UINT8 DR;       ///< Number of tCK cycles between transactions when switching between Ranks (in the same DIMM).
  UINT8 DD;       ///< Number of tCK cycles between transactions when switching between DIMMs
} TURNAROUND_TIMING;

// @todo use the MemInfoHob data instead of duplicate structure.
///
/// Memory information Data Structure
///
typedef struct {
  MEMORY_TIMING Timing[PROFILE_NUM];                   ///< Offset 0 Timming information for the DIMM
  UINT32  memSize;                                     ///< Offset 128 Total physical memory size
  UINT16  ddrFreq;                                     ///< Offset 132 DDR Current Frequency
  UINT16  ddrFreqMax;                                  ///< Offset 134 DDR Maximum Frequency
  UINT16  dimmSize[NODE_NUM * CH_NUM * DIMM_NUM];      ///< Offset 136 Size of each DIMM
  UINT16  VddVoltage[PROFILE_NUM];                     ///< Offset 144 The voltage setting for the DIMM
  UINT8   DimmStatus[NODE_NUM * CH_NUM * DIMM_NUM];    ///< Offset 152 The enumeration value from MrcDimmSts
  UINT8   RankInDimm[NODE_NUM * CH_NUM * DIMM_NUM];    ///< Offset 156 Number of ranks in a DIMM
  UINT8   *DimmsSpdData[NODE_NUM * CH_NUM * DIMM_NUM]; ///< Offset 160 SPD data of each DIMM
  UINT8   RefClk;                                      ///< Offset 192 Reference Clock
  UINT8   Ratio;                                       ///< Offset 193 Clock Multiplier
  BOOLEAN EccSupport;                                  ///< Offset 194 ECC supported or not
  UINT8   Profile;                                     ///< Offset 195 Currently running memory profile
  UINT8   XmpProfileEnable;                            ///< Offset 196 If XMP capable DIMMs are detected, this will indicate which XMP Profiles are common among all DIMMs.
  UINT8   DdrType;                                     ///< Offset 197 Current DDR type, see DDR_TYPE_xxx defines above
  UINT8   Reserved[2];                                 ///< Offset 198 Reserved bytes for future use
  UINT32  DefaultXmptCK[XMP_PROFILE_NUM];              ///< Offset 200 The Default XMP tCK values read from SPD.
  TURNAROUND_TIMING tRd2Rd;                            ///< Read-to-Read   Turn Around Timings for Channel 0
  TURNAROUND_TIMING tRd2Wr;                            ///< Read-to-Write  Turn Around Timings for Channel 0
  TURNAROUND_TIMING tWr2Rd;                            ///< Write-to-Read  Turn Around Timings for Channel 0
  TURNAROUND_TIMING tWr2Wr;                            ///< Write-to-Write Turn Around Timings for Channel 0
  TURNAROUND_TIMING tRd2Rd_Ch1;                        ///< Read-to-Read   Turn Around Timings for Channel 1
  TURNAROUND_TIMING tRd2Wr_Ch1;                        ///< Read-to-Write  Turn Around Timings for Channel 1
  TURNAROUND_TIMING tWr2Rd_Ch1;                        ///< Write-to-Read  Turn Around Timings for Channel 1
  TURNAROUND_TIMING tWr2Wr_Ch1;                        ///< Write-to-Write Turn Around Timings for Channel 1
  UINT16 tRRD_L[PROFILE_NUM];                          ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time for same bank groups.
  UINT16 tRRD_S[PROFILE_NUM];                          ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time for different bank groups.
  UINT16 tWTR_L[PROFILE_NUM];                          ///< Number of tCK cycles for the channel DIMM's minimum internal write to read command delay time for same bank groups.
  UINT16 tWTR_S[PROFILE_NUM];                          ///< Number of tCK cycles for the channel DIMM's minimum internal write to read command delay time for different bank groups.
} MEMORY_INFO_DATA;
#pragma pack()

///
/// Memory information Protocol definition
///
typedef struct {
  MEMORY_INFO_DATA  MemInfoData; ///< Memory Information Data Structure
} MEM_INFO_PROTOCOL;

#endif
