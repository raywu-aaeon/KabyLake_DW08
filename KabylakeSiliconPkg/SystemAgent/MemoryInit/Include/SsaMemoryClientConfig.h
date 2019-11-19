/** @file
  This file contains the definition of the BIOS-SSA Client Memory Configuration API.

  NOTE: This file is only intended to be included from MrcSsaServices.h.  It
  cannot be included separately.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

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
#ifndef _SsaMemoryClientConfig_h_
#define _SsaMemoryClientConfig_h_

#ifdef SSA_FLAG

#pragma pack (push, 1)

///
/// CPGC capability information definitions.
///
typedef struct {
  UINT8   MaxNumberDqdbCachelines;              ///< The maximum number of DqDB pattern cachelines per channel.
  UINT8   MaxDqdbIncRateExponentVal;            ///< The maximum DqDB increment rate exponent value.  For a CPGC engine that doesn't support exponential DqDB increment rate values, this value should be equal to 0.
  UINT8   MaxDqdbIncRateLinearVal;              ///< The maximum DqDB increment rate linear value.  Some CPGC engines use linear for smaller values and exponential (2^n) for larger values.  For a CPGC engine that doesn't support linear DqDB increment rate values, this value should be 0.
  UINT8   MaxNumberDqdbUniseqs;                 ///< The maximum number of DqDB unisequencers per channel.
  UINT32  MaxDqdbUniseqSeedVal;                 ///< The maximum DqDB unisequencer seed value.
  UINT8   MaxDqdbUniseqLVal;                    ///< The maximum DqDB unisequencer L value.
  UINT8   MaxDqdbUniseqMVal;                    ///< The maximum DqDB unisequencer M value.
  UINT8   MaxDqdbUniseqNVal;                    ///< The maximum DqDB unisequencer N value.
  UINT8   MaxDqdbUniseqSeedReloadRateVal;       ///< The maximum DqDB unisequencer seed reload rate value.
  UINT8   MaxDqdbUniseqSeedSaveRateVal;         ///< The maximum DqDB unisequencer seed save rate value.
  UINT8   MaxDqdbInvertDcShiftRateExponentVal;  ///< The maximum DqDB invert/DC shift rate exponent value.
  UINT8   MaxNumberCadbRows;                    ///< The maximum number of CADB pattern rows.
  UINT8   MaxNumberCadbUniseqs;                 ///< The maximum number of CADB unisequencers per channel.
  UINT32  MaxCadbUniseqSeedVal;                 ///< The maximum CADB unisequencer seed value.
  UINT8   MaxCadbUniseqLVal;                    ///< The maximum CADB unisequencer L value.
  UINT8   MaxCadbUniseqMVal;                    ///< The maximum CADB unisequencer M value.
  UINT8   MaxCadbUniseqNVal;                    ///< The maximum CADB unisequencer N value.
  UINT8   MaxDummyReadLVal;                     ///< The maximum Dummy Read L value.
  UINT8   MaxDummyReadMVal;                     ///< The maximum Dummy Read M value.
  UINT8   MaxDummyReadNVal;                     ///< The maximum Dummy Read N value.
  UINT32  MaxStartDelayVal;                     ///< The maximum start delay value in dclks.
  BOOLEAN IsLoopCountExponential;               ///< Boolean value indicating whether or not loop count value is exponential.
  UINT32  MaxLoopCountVal;                      ///< The maximum loop count value.  If IsLoopCountExponential is TRUE then the MaxLoopCountVal value is the maximum exponent value.
  UINT8   MaxNumberSubseqs;                     ///< The maximum number of subsequences per channel.
  UINT8   MaxBurstLengthExponentVal;            ///< The maximum burst length exponent value.  For a CPGC engine that doesn't support exponential burst length values, this value should be 0.
  UINT32  MaxBurstLengthLinearVal;              ///< The maximum burst length linear value.  Some CPGC engines use linear for smaller values and exponential (2^n) for larger values.  For a CPGC engine that doesn't support linear burst length values, this value should be 0.
  UINT32  MaxInterSubseqWaitVal;                ///< The maximum inter-subsequence wait value.
  UINT8   MaxOffsetAddrUpdateRateVal;           ///< The maximum offset address update rate value.
  UINT8   MaxAddrInvertRateVal;                 ///< The maximum address invert rate value.
  UINT8   MaxRankAddrIncRateExponentVal;        ///< The maximum rank address increment rate exponent value.
  UINT8   MaxRankAddrIncRateLinearVal;          ///< The maximum rank address increment rate linear value.
  INT8    MinRankAddrIncVal;                    ///< The minimum rank address increment value.
  INT8    MaxRankAddrIncVal;                    ///< The maximum rank address increment value.
  UINT8   MaxBankAddrIncRateExponentVal;        ///< The maximum bank address increment rate exponent value.
  UINT8   MaxBankAddrIncRateLinearVal;          ///< The maximum bank address increment rate linear value.
  INT8    MinBankAddrIncVal;                    ///< The minimum bank address increment value.
  INT8    MaxBankAddrIncVal;                    ///< The maximum bank address increment value.
  UINT8   MaxRowAddrIncRateExponentVal;         ///< The maximum row address increment rate exponent value.
  UINT8   MaxRowAddrIncRateLinearVal;           ///< The maximum row address increment rate linear value.
  INT16   MinRowAddrIncVal;                     ///< The minimum row address increment value.
  INT16   MaxRowAddrIncVal;                     ///< The maximum row address increment value.
  UINT8   MaxColAddrIncRateExponentVal;         ///< The maximum column address increment rate exponent value.
  UINT8   MaxColAddrIncRateLinearVal;           ///< The maximum column address increment rate linear value.
  INT16   MinColAddrIncVal;                     ///< The minimum column address increment value.
  INT16   MaxColAddrIncVal;                     ///< The maximum column address increment value.
  UINT8   MaxNumberRankMapEntries;              ///< The maximum number of entries in the logical to physical rank map.
  UINT8   MaxNumberBankMapEntries;              ///< The maximum number of entries in the logical to physical bank map.  If the system does not support a logical to physical bank map then the value should be 0.
  UINT8   MaxNumberRowAddrSwizzleEntries;       ///< The maximum number of row address swizzle entries.
  UINT8   MaxRowAddrSwizzleVal;                 ///< The maximum row address swizzle value.
  UINT8   MaxStopOnNthErrorCountVal;            ///< The maximum stop-on-nth-error value.
  UINT8   MaxNumberErrorCounters;               ///< The maximum number of error counters.
  UINT32  MaxScramblerKeyVal;                   ///< The maximum scrambler key value.
  UINT32  MaxRefreshIdleTimerVal;               ///< The maximum refresh idle timer value.
} MRC_CLIENT_CPGC_INFO;

///
/// DqDB increment rate mode.
///
typedef enum {
  ExponentialDqdbIncRateMode = 0,         ///< Exponential DqDB increment rate.
  LinearDqdbIncRateMode      = 1,         ///< Linear DqDB increment rate.
  DqdbIncRateModeMax,                     ///< DQDB_INC_RATE_MODE enumeration maximum value.
  DqdbIncRateModeDelim = INT32_MAX        ///< This value ensures the enum size is consistent on both sides of the PPI.
} DQDB_INC_RATE_MODE;

///
/// ECC data source.
///
typedef enum {
  ByteGroup0 = 0,                         ///< Source ECC data from byte group 0.
  ByteGroup7 = 1,                         ///< Source ECC data from byte group 7.
  EccDataSourceMax,                       ///< ECC_DATA_SOURCE enumeration maximum value.
  EccDataSourceDelim = INT32_MAX          ///< This value ensures the enum size is consistent on both sides of the PPI.
} ECC_DATA_SOURCE;

///
/// DqDB unisequencer modes.
///
typedef enum {
  LmnDqdbUniseqMode,                      ///< LMN DqDB unisequencer select.
  BufferDqdbUniseqMode,                   ///< Buffer DqDB unisequencer select.
  LfsrDqdbUniseqMode,                     ///< LFSR DqDB unisequencer select.
  DqdbUniseqModeMax,                      ///< DQDB_UNISEQ_SEL enumeration maximum value.
  DqdbUniseqModeDelim = INT32_MAX         ///< This value ensures the enum size is consistent on both sides of the PPI.
} DQDB_UNISEQ_MODE;

///
/// DqDB LFSR modes.
///
typedef enum {
  DqdbLfsrMode3  = 0,                     ///< f = x^3  + x^2  + 1
  DqdbLfsrMode4  = 1,                     ///< f = x^4  + x^3  + 1
  DqdbLfsrMode6  = 2,                     ///< f = x^6  + x^5  + 1
  DqdbLfsrMode8  = 3,                     ///< f = x^8  + x^6  + x^5  + x^4  + 1
  DqdbLfsrMode10 = 4,                     ///< f = x^10 + x^8  + x^7  + x^2  + 1
  DqdbLfsrMode12 = 5,                     ///< f = x^12 + x^11 + x^10 + x^4  + 1
  DqdbLfsrMode16 = 6,                     ///< f = x^16 + x^14 + x^13 + x^11 + 1
  DqdbLfsrMode24 = 7,                     ///< f = x^24 + x^23 + x^21 + x^20 + 1
  DqdbLfsrTypeMax,
  DqdbLfsrTypeDelim = INT32_MAX
} DQDB_LFSR_MODE;

///
/// Invert/DC mode.
///
typedef enum {
  InvertMode,                             ///< invert mode.
  DcMode,                                 ///< DC mode.
  InvertDcModeMax,                        ///< INVERT_DC_MODE enumeration maximum value.
  InvertDcModeDelim = INT32_MAX           ///< This value ensures the enum size is consistent on both sides of the PPI.
} INVERT_DC_MODE;

///
/// CADB unisequencer modes.
///
typedef enum {
  LmnCadbUniseqMode,                      ///< LMN CADB unisequencer mode.
  BufferCadbUniseqMode,                   ///< Buffer CADB unisequencer mode.
  LfsrCadbUniseqMode,                     ///< LFSR CADB unisequencer mode.
  CadbUniseqModeMax,                      ///< CADB_UNISEQ_MODE enumeration maximum value.
  CadbUniseqModeDelim = INT32_MAX         ///< This value ensures the enum size is consistent on both sides of the PPI.
} CADB_UNISEQ_MODE;

///
/// DIMM/rank pairs.
///
typedef struct {
  UINT8 Dimm;                             ///< Zero based DIMM number.
  UINT8 Rank;                             ///< Zero based physical rank number.
} DIMM_RANK_PAIR;

///
/// Burst length mode.
///
typedef enum {
  ExponentialBurstLengthMode = 0,         ///< Burst length value is an exponent.
  LinearBurstLengthMode      = 1,         ///, Burst length value is linear.
  BurstLengthModeMax,                     ///< BURST_LENGTH_MODE enumeration maximum value.
  BurstLengthModeDelim = INT32_MAX        ///< This value ensures the enum size is consistent on both sides of the PPI.
} BURST_LENGTH_MODE;

///
/// Subsequence type.
///
typedef enum {
  BaseReadSubseqType      = 0,            ///< Base read.
  BaseWriteSubseqType     = 1,            ///< Base write.
  BaseReadWriteSubseqType = 2,            ///< Base read one cacheline followed by write one cacheline.
  BaseWriteReadSubseqType = 3,            ///< Base write one cacheline followed by read one cacheline.
  OffsetReadSubseqType    = 4,            ///< Offset read.
  OffsetWriteSubseqType   = 5,            ///< Offset write.
  SubseqTypeMax,                          ///< SUBSEQ_TYPE enumeration maximum value.
  SubseqTypeDelim = INT32_MAX             ///< This value ensures the enum size is consistent on both sides of the PPI.
} SUBSEQ_TYPE;

///
/// Sequence state.
///
typedef struct {
  UINT32 LoopCount;                       ///< The current loop count value.
  UINT8  SubSeqIndex;                     ///< The current subsequence index value.
  UINT32 Cacheline;                       ///< The current cacheline index value.
  UINT8  BaseRank;                        ///< The current base rank address value.
  UINT8  BaseBank;                        ///< The current base bank address value.
  UINT32 BaseRow;                         ///< The current base row address value.
  UINT32 BaseCol;                         ///< The current base column address value.
  UINT32 OffsetRow;                       ///< The current offset row address value.
  UINT32 OffsetCol;                       ///< The current offset column address value.
} SSA_SEQ_STATE;

///
/// CPGC initialization mode.
///
typedef enum {
  IdleOnResetCpgcInitMode = 0,            ///< Idle on MC reset.
  ReutCpgcInitMode        = 1,            ///< REUT.
  MrsCpgcInitMode         = 2,            ///< MRS.
  NormalCpgcInitMode      = 3,            ///< Normal protocol mode.
  CpgcInitModeMax,                        ///< CPGC_INIT_MODE enumeration maximum value.
  CpgcInitModeDelim = INT32_MAX           ///< This value ensures the enum size is consistent on both sides of the PPI.
} CPGC_INIT_MODE;

///
/// Address update mode.
///
typedef enum {
  CachelineAddrUpdateRate = 0,            ///< Address is updated based on number of cacheline transactions.
  LoopCountAddrUpdateRate = 1,            ///< Address is updated based on number of loop counts.
  AddrUpdateRateModeMax,                  ///< ADDR_UPDATE_RATE_MODE enumeration maximum value.
  AddrUpdateRateModeDelim = INT32_MAX     ///< This value ensures the enum size is consistent on both sides of the PPI.
} ADDR_UPDATE_RATE_MODE;

///
/// Address increment mode.
///
typedef enum {
  ExponentialAddrIncMode = 0,             ///< Exponential address increment mode.
  LinearAddrIncMode      = 1,             ///< Linear address increment mode.
  AddrIncModeMax,                         ///< ADDR_INC_MODE enumeration maximum value.
  AddrincModeDelim = INT32_MAX            ///< This value ensures the enum size is consistent on both sides of the PPI.
} ADDR_INC_MODE;

///
/// CPGC test execution stop mode.
///
typedef enum {
  DoNotStopCpgcStopMode      = 0,         ///< Let the test run to completion regardless of errors.
  NthAnyLaneFailCpgcStopMode = 1,         ///< Stop the test when nth error is detected on any lane.
  AllBytesFailCpgcStopMode   = 2,         ///< Stop the test when errors are detected on any lane of all byte groups.
  AllLanesFailCpgcStopMode   = 3,         ///< Stop the test when errors are detected on all lanes.
  CpgcStopModeMax,                        ///< CPGC_STOP_MODE enumeration maximum value.
  CpgcStopModeDelim = INT32_MAX           ///< This value ensures the enum size is consistent on both sides of the PPI.
} CPGC_STOP_MODE;

///
/// Error counter mode.
///
typedef enum {
  ChannelErrorCounterMode,                ///< Errors are counted on the whole channel.  If errors occur simultaneously on multiple lanes, the counter is only incremented once.
  LaneErrorCounterMode,                   ///< Errors are counted on a specific lane.
  NibbleErrorCounterMode,                 ///< Errors are counted on a specific nibble.  If errors occur simultaneously on multiple lanes within the specified nibble, the counter is only incremented once.
  ByteErrorCounterMode,                   ///< Errors are counted on a specific byte group.  If errors occur simultaneously on multiple lanes within the specified byte group, the counter is only incremented once.
  ChunkErrorCounterMode,                  ///< Errors are counted on a specific chunk.
  ErrorCounterModeMax,                    ///< ERROR_COUNTER_MODE enumeration maximum value.
  ErrorCounterModeDelim = INT32_MAX       ///< This value ensures the enum size is consistent on both sides of the PPI.
} ERROR_COUNTER_MODE;

///
/// SSA address.
///
typedef struct {
  UINT8  Rank;                            ///< The rank address.
  UINT8  Bank;                            ///< The bank address.
  UINT32 Row;                             ///< The row address.
  UINT16 Col;                             ///< The column address.
} SSA_ADDRESS;

///
/// Clock gate cipher A/B mode.
///
typedef enum {
  NoneClockGateABMode        = 0,         ///< No clock gating.
  TwoCycleCClockGateABMode   = 1,         ///< 2 cycles.
  ThreeCycleCClockGateABMode = 2,         ///< 3 cycles.
  FourCycleCClockGateABMode  = 3,         ///< 4 cycles.
  ClockGateABModeMax,                     ///< CLOCK_GATE_AB_MODE enumeration maximum value.
  ClockGateABModedelim = INT32_MAX        ///< This value ensures the enum size is consistent on both sides of the PPI.
} CLOCK_GATE_AB_MODE;

///
/// Clock gate cipher C mode.
///
typedef enum {
  NoneClockGateCMode        = 0,          ///< No clock gating.
  TwoCycleCClockGateCMode   = 1,          ///< 2 cycles.
  FourCycleCClockGateCMode  = 2,          ///< 4 cycles.
  EightCycleCClockGateCMode = 3,          ///< 8 cycles.
  ClockGateCModeMax,                      ///< CLOCK_GATE_C_MODE enumeration maximum value.
  ClockGateCModeDelim = INT32_MAX         ///< This value ensures the enum size is consistent on both sides of the PPI.
} CLOCK_GATE_C_MODE;

/**
  Function used to get information about about the CPGC capabilities.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     CpgcInfo - Pointer to buffer to be filled with CPGC capabilities information.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_CPGC_INFO) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT MRC_CLIENT_CPGC_INFO     *CpgcInfo
  );

/**
  Function used to enable/disable address-as-data.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      EnableRead  - Specifies whether address-as-data for reads is enabled.  TRUE enables address-as-data for reads, FALSE disables it.
  @param[in]      EnableWrite - Specifies whether address-as-data for writes is enabled.  TRUE enables address-as-data for writes, FALSE disables it.

  @retval NotAvailable if the system does not support the address-as-data feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_ADDRESS_AS_DATA) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN BOOLEAN                   EnableRead,
  IN BOOLEAN                   EnableWrite
  );

/**
  Function used to write pattern content to the DqDB.

  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Channel             - Zero based channel number.
  @param[in]      Pattern             - Pointer to buffer containing the DqDB pattern.
  @param[in]      CachelineCount      - Size of the buffer pattern in number of cachelines.  Each cacheline has 8 UINT64 elements.  For example, if CachelineCount is 1 then the Pattern buffer size is 64 bytes.
  @param[in]      StartCachelineIndex - Start offset on the DqDB.  The sum of the CachelineCount and StartCachelineIndex values must be less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_DQDB_PATTERN) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN CONST UINT64              *Pattern,
  IN UINT8                     CachelineCount,
  IN UINT8                     StartCachelineIndex
  );

/**
  Function used to read pattern content from the DqDB.

  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Channel             - Zero based channel number.
  @param[out]     Pattern             - Pointer to buffer to be filled with the DqDB pattern.
  @param[in]      CachelineCount      - Size of the buffer pattern in number of cachelines.  Each cacheline has 8 UINT64 elements.  For example, if CachelineCount is 1 then the Pattern buffer size is 64 bytes.
  @param[in]      StartCachelineIndex - Start offset on the DqDB.  The sum of the CachelineCount and StartCachelineIndex values must be less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_DQDB_PATTERN) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  OUT UINT64                   *Pattern,
  IN UINT8                     CachelineCount,
  IN UINT8                     StartCachelineIndex
  );

/**
  Function used to set the WDB start and end indexes as well as the WDB increment rate.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      StartIndex  - Starting index in the DqDB.  The value must be less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param[in]      EndIndex    - Ending index in the DqDB.  The value must be less than or equal to the StartIndex value and less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param[in]      IncRateMode - Increment rate mode.
  @param[in]      IncRate     - Increment rate value.  If the IncRateMode is ExponentialDqdbIncRateMode then the IncRate value must be less than or equal to the MaxDqdbIncRateExponentVal from the GetCpgcInfo() function.  If the IncRateMode is LinearDqdbIncRateMode then the IncRate value must be less than or equal to the MaxDqdbIncRateLinearVal from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_DQDB_INDEXES) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     StartIndex,
  IN UINT8                     EndIndex,
  IN DQDB_INC_RATE_MODE        IncRateMode,
  IN UINT8                     IncRate
  );

/**
  Function used to set the WDB start and end indexes as well as the WDB increment rate.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      EccDataSource - Specifies the ECC signal data source.

  @retval NotAvailable if the system does not support the ECC Data Source feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_DQDB_ECC_DATA_SOURCE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN ECC_DATA_SOURCE           EccDataSource
  );

/**
  Function used to set the DqDB unisequencer mode.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[in]      Mode       - DqDB unisequencer mode.

  @retval NotAvailable if the given Mode value is not available on the given Uniseq.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_DQDB_UNISEQ_MODE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  IN DQDB_UNISEQ_MODE          Mode
  );

/**
  Function used to set the DqDB write unisequencer seed value.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[in]      Seed       - DqDB write unisequencer seed value.  The value must be less than or equal to the MaxDqdbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_DQDB_UNISEQ_WR_SEED) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  IN UINT32                    Seed
  );

/**
  Function used to set the DqDB read unisequencer seed value.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[in]      Seed       - DqDB read unisequencer seed value.  The value must be less than or equal to the MaxDqdbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_DQDB_UNISEQ_RD_SEED) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  IN UINT32                    Seed
  );

/**
  Function used to get the DqDB write unisequencer seed value.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[out]     Seed       - Pointer to where the DqDB write unisequencer seed value will be stored.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_DQDB_UNISEQ_WR_SEED) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  OUT UINT32                   *Seed
  );

/**
  Function used to get the DqDB read unisequencer seed value.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Uniseq     - Zero based DqDB unisequencer number.
  @param[out]     Seed       - Pointer to where the DqDB read unisequencer seed value will be stored.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_DQDB_UNISEQ_RD_SEED) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  OUT UINT32                   *Seed
  );

/**
  Function used to set the DqDB unisequencer L/M/N values.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      Uniseq          - DqDB unisequencer mode.
  @param[in]      L               - DqDB unisequencer L counter value.  The value must be less than or equal to the MaxDqdbUniseqLVal value from the GetCpgcInfo() function.
  @param[in]      M               - DqDB unisequencer M counter value.  The value must be less than or equal to the MaxDqdbUniseqMVal value from the GetCpgcInfo() function.
  @param[in]      N               - DqDB unisequencer N counter value.  The value must be less than or equal to the MaxDqdbUniseqNVal value from the GetCpgcInfo() function.
  @param[in]      LDataSel        - DqDB unisequencer L data select value.  The value must be 0 or 1.
  @param[in]      EnableFreqSweep - Enables the Frequency Sweep feature.

  @retval NotAvailable if the LMN mode is not available on the given Uniseq.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_DQDB_UNISEQ_LMN) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  IN UINT16                    L,
  IN UINT16                    M,
  IN UINT16                    N,
  IN UINT8                     LDataSel,
  IN BOOLEAN                   EnableFreqSweep
  );

/**
  Function used to set the DqDB unisequencer LFSR configuration.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      ReloadRate      - DqDB unisequencer LFSR seed reload rate.  The value must be less than or equal to the MaxDqdbUniseqSeedReloadRateVal value from the GetCpgcInfo() function.
  @param[in]      SaveRate        - DqDB unisequencer LFSR seed save rate.  The value must be less than or equal to the MaxDqdbUniseqSeedSaveRateVal value from the GetCpgcInfo() function.
  @param[in]      LfsrMode        - LFSR mode.
  @param[in]      EnableChaining  - If TRUE then the LFSR seeds will be chained causing wrap of LFSR[n] to increase the seed of LFSR[n+2], LFSR[0] is not incremented due to wrap.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_DQDB_UNISEQ_LFSR_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     ReloadRate,
  IN UINT8                     SaveRate,
  IN DQDB_LFSR_MODE            LfsrMode,
  IN BOOLEAN                   EnableChaining
  );

/**
  Function used to set the DqDB Invert/DC configuration.

  @param[in, out] This              - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket            - Zero based CPU socket number.
  @param[in]      Controller        - Zero based controller number.
  @param[in]      Channel           - Zero based channel number.
  @param[in]      LaneMasks         - Pointer to array of lane bitmasks.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  Bit value 1 = participate in the Invert/DC feature; bit value 0 = do not participate in the Invert/DC feature.  If all lane bitmasks are 0 then the feature is disabled.
  @param[in]      Mode              - Invert/DC mode.
  @param[in]      DcPolarity        - DC polarity (when operating in the DC mode). 0 = DC low. 1 = DC high.
  @param[in]      ShiftRateExponent - Exponent of the bitmask shift rate.  The value must be less than or equal to the MaxDqdbInvertDcShiftRateExponentVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_DQDB_INVERT_DC_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN CONST UINT8               *LaneMasks,
  IN INVERT_DC_MODE            Mode,
  IN UINT8                     DcPolarity,
  IN UINT8                     ShiftRateExponent
  );

/**
  This function sets the given channels CADB pattern generation mode to the given value.  The default value is NormalCadbMode.

  @param[in, out] This                   - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                 - Zero based CPU socket number.
  @param[in]      Controller             - Zero based controller number.
  @param[in]      Channel                - Zero based channel number.
  @param[in]      EnableAlwaysOn         - Specifies whether CADB patterns will be driven out on all command and address pins every Dclk while a test is running.
  @param[in]      EnableOnDeselect       - Specifies whether CADB patterns will be driven out during deselect packets.
  @param[in]      CmdDeselectStartMask   - Provides a per command mechanism to enable driving CADB patterns during deselect cycles.  This parameter only applies if the EnableOnDeselect parameter is TRUE.  Bit 0 = 1: Start driving CADB patterns during deselects on the cycle following a WR CMD.  Bit 1 = 1: Start driving CADB patterns during deselects on the cycle following a RD CMD.  Bit 2 = 1: Start driving CADB patterns during deselects on the cycle following a ACT CMD.  Bit 3 = 1: Start driving CADB patterns during deselects on the cycle following a PRE CMD.
  @param[in]      CmdDeselectStopMask    - Provides a per command mechanism to disable driving CADB patterns during deselect cycles.  This parameter only applies if the EnableOnDeselect parameter is TRUE.  Bit 0 = 1: Stop driving CADB patterns during deselects on the cycle following a WR CMD.  Bit 1 = 1: Stop driving CADB patterns during deselects on the cycle following a RD CMD.  Bit 2 = 1: Stop driving CADB patterns during deselects on the cycle following a ACT CMD.  Bit 3 = 1: Stop driving CADB patterns during deselects on the cycle following a PRE CMD.
  @param[in]      LaneDeselectMask       - Specifies which lanes will have CADB patterns driven on deselects.  This parameter only applies if the EnableOnDeselect parameter is TRUE.  Bit 0 = 1: Drive CADB patterns on deselects on MA.  Bit 1 = 1: Drive CADB patterns on deselects on BA.  Bit 2 = 1: Drive CADB patterns on deselects on CMD.  Bit 3 = 1: Drive CADB patterns on deselects on CS.
  @param[in]      CasSelectMask          - Provides a per signal group mechanism to enable driving CADB patterns during CAS CMD select cycles.  Bit 0 = 1: Drive CADB patterns during an CAS on MA [11] and MA [9:3].  Bit 1 = 1: Drive CADB patterns during an CAS on BA.
  @param[in]      ActSelectMask          - Provides a per signal group mechanism to enable driving CADB patterns during ACT CMD select cycles.  Bit 0 = 1: Drive CADB patterns during an ACT on MA.  Bit 1 = 1: Drive CADB patterns during an ACT on BA.
  @param[in]      PreSelectMask          - Provides a per signal group mechanism to enable driving CADB patterns during PRE CMD select cycles.  Bit 0 = 1: Drive CADB patterns during an PRE on MA except AP bit.  Bit 1 = 1: Drive CADB patterns during an PRE on BA.
  @param[in]      SaveCurrentSeedMask    - Provides a per command mechanism to saving the current CADB seed to the starting seed.  Bit 0 = 1: Save the current CADB seed to the starting seed on a WR CMD.  Bit 1 = 1: Save the current CADB seed to the starting seed on a RD CMD.  Bit 2 = 1: Save the current CADB seed to the starting seed on a ACT CMD.  Bit 3 = 1: Save the current CADB seed to the starting seed on a PRE CMD.
  @param[in]      ReloadStartingSeedMask - Provides a per command mechanism to reloading the starting CADB seed to the current seed.  Bit 0 = 1: Reload the starting CADB seed to the current seed on a WR CMD.  Bit 1 = 1: Reload the starting CADB seed to the current seed on a RD CMD.  Bit 2 = 1: Reload the starting CADB seed to the current seed on a ACT CMD.  Bit 3 = 1: Reload the starting CADB seed to the current seed on a PRE CMD.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_CADB_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   EnableAlwaysOn,
  IN BOOLEAN                   EnableOnDeselect,
  IN UINT8                     CmdDeselectStartMask,
  IN UINT8                     CmdDeselectStopMask,
  IN UINT8                     LaneDeselectMask,
  IN UINT8                     CasSelectMask,
  IN UINT8                     ActSelectMask,
  IN UINT8                     PreSelectMask,
  IN UINT8                     SaveCurrentSeedMask,
  IN UINT8                     ReloadStartingSeedMask
  );

/**
  Function used to write to the CADB.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      Pattern       - Pointer to buffer containing the CADB pattern.
  @param[in]      RowCount      - Size of the buffer pattern in term of the count of rows.  Each row has 2 UINT32 elements.
  @param[in]      StartRowIndex - Start offset on the CADB.  The sum of the RowCount and StartRowIndex values must be less than or equal to the MaxNumberCadbRows value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_CADB_PATTERN) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN CONST UINT64              *Pattern,
  IN UINT8                     RowCount,
  IN UINT8                     StartRowIndex
  );

/**
  Function used to set the CADB unisequencer modes.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      Uniseq       - Zero based CADB unisequencer number.
  @param[in]      DeselectMode - CADB deselect cycle unisequencer mode.  The value must be less than or equal to the MaxCadbUniseqSeedVal value from the GetCpgcInfo() function.
  @param[in]      SelectMode   - CADB select cycle unisequencer mode.  The value must be less than or equal to the MaxCadbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval NotAvailable if the given DeselectMode or SelectMode value is not available on the given Uniseq.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_CADB_UNISEQ_MODE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  IN CADB_UNISEQ_MODE          DeselectMode,
  IN CADB_UNISEQ_MODE          SelectMode
  );

/**
  Function used to set the CADB unisequencer seed values.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      Uniseq       - Zero based CADB unisequencer number.
  @param[in]      DeselectSeed - CADB deselect cycle unisequencer seed value.The value must be less than or equal to the MaxCadbUniseqSeedVal value from the GetCpgcInfo() function.
  @param[in]      SelectSeed   - CADB select cycle unisequencer seed value.The value must be less than or equal to the MaxCadbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_CADB_UNISEQ_SEED) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  IN UINT32                    DeselectSeed,
  IN UINT32                    SelectSeed
  );

/**
  Function used to set the CADB unisequencer L/M/N values.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      Uniseq          - Zero based CADB unisequencer number.
  @param[in]      L               - CADB unisequencer L counter value.  The value must be less than or equal to the MaxCadbUniseqLVal value from the GetCpgcInfo() function.
  @param[in]      M               - CADB unisequencer M counter value.  The value must be less than or equal to the MaxCadbUniseqMVal value from the GetCpgcInfo() function.
  @param[in]      N               - CADB unisequencer N counter value.  The value must be less than or equal to the MaxCadbUniseqNVal value from the GetCpgcInfo() function.
  @param[in]      LDataSel        - CADB unisequencer L data select value.  The value must be 0 or 1.
  @param[in]      EnableFreqSweep - Enables LMN frequency sweep feature.

  @retval NotAvailable if the LMN mode is not available on the given Uniseq.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_CADB_UNISEQ_LMN) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Uniseq,
  IN UINT16                    L,
  IN UINT16                    M,
  IN UINT16                    N,
  IN UINT8                     LDataSel,
  IN BOOLEAN                   EnableFreqSweep
  );

/**
  Function used to set the Dummy Read LMN values.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      L               - Dummy Read unisequencer L counter value.  The value must be less than or equal to the MaxDummyReadLVal value from the GetCpgcInfo() function.
  @param[in]      M               - Dummy Read unisequencer M counter value.  The value must be less than or equal to the MaxDummyReadMVal value from the GetCpgcInfo() function.
  @param[in]      N               - Dummy Read unisequencer N counter value.  The value must be less than or equal to the MaxDummyReadNVal value from the GetCpgcInfo() function.
  @param[in]      LDataSel        - Dummy Read unisequencer L data select value.  The value must be 0 or 1.
  @param[in]      EnableFreqSweep - Enables LMN frequency sweep feature.

  @retval NotAvailable if the system does not support the Dummy Read feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_DUMMY_READ_LMN) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT16                    L,
  IN UINT16                    M,
  IN UINT16                    N,
  IN UINT8                     LDataSel,
  IN BOOLEAN                   EnableFreqSweep
  );

/**
  Function used to set the Dummy Read bank mask.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      BankMask        - Dummy Read bank mask.  The value must be less than (1 << (the BankCount value from the GetDimmInfo function for the DIMMs that will be tested)).

  @retval NotAvailable if the system does not support the Dummy Read feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_DUMMY_READ_BANK_MASK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT16                    BankMask
  );

/**
  Function used to set the Dummy Read ranks.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      PairCount     - Number of entries in the DimmRankPairs array.  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      DimmRankPairs - Array of DIMM and physical rank pair values.  If a DIMM/rank appears in the list, then the corresponding bit will be set to 1 in the rank mask.  If a DIMM/rank doesn't appear in the list then then the corresponding bit will be set to 0 in the rank mask.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.

  @retval NotAvailable if the system does not support the Dummy Read feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_DUMMY_READ_RANKS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     PairCount,
  IN CONST DIMM_RANK_PAIR      *DimmRankPairs
  );

/**
  Function used to set the subsequence configuration.

  @param[in, out] This                              - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                            - Zero based CPU socket number.
  @param[in]      Controller                        - Zero based controller number.
  @param[in]      Channel                           - Zero based channel number.
  @param[in]      Subseq                            - Zero based subsequence number.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param[in]      BurstLengthMode,                  - Burst length mode.
  @param[in]      BurstLength                       - Burst length in cachelines.  If BurstLengthMode is ExponentialBurstLengthMode then the Burstlength value is an exponent and the value must be less than or equal to the MaxBurstLengthExponentVal value from the in GetCpgcInfo() function.  If BurstLengthMode is LinearBurstLengthMode then the Burstlength value is linear and the value must be less than or equal to the MaxBurstLengthLinearVal value from the in GetCpgcInfo() function.
  @param[in]      InterSubseqWait                   - Inter-subsequence wait in dclks.  The value must be less than or equal to the MaxInterSubseqWaitVal value from the GetCpgcinfo() function.
  @param[in]      SubseqType                        - Subsequence type.
  @param[in]      EnableSaveCurrentBaseAddrToStart  - Specifies whether the current base address is saved to the start address with this subsequence.
  @param[in]      EnableResetCurrentBaseAddrToStart - Specifies whether the current base address is reset to the start address with this subsequence.
  @param[in]      EnableAddrInversion               - Specifies whether address inversion enabled for this subsequence.
  @param[in]      EnableDataInversion               - Specifies whether data inversion enabled for this subsequence.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SUBSEQ_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Subseq,
  IN BURST_LENGTH_MODE         BurstLengthMode,
  IN UINT32                    BurstLength,
  IN UINT32                    InterSubseqWait,
  IN SUBSEQ_TYPE               SubseqType,
  IN BOOLEAN                   EnableSaveCurrentBaseAddrToStart,
  IN BOOLEAN                   EnableResetCurrentBaseAddrToStart,
  IN BOOLEAN                   EnableAddrInversion,
  IN BOOLEAN                   EnableDataInversion
  );

/**
  Function used to set the subsequence configuration.

  @param[in, out] This                   - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                 - Zero based CPU socket number.
  @param[in]      Controller             - Zero based controller number.
  @param[in]      Channel                - Zero based channel number.
  @param[in]      Subseq                 - Zero based subsequence number.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param[in]      EnableBaseInvertData   - Specifies whether base inversion of the data is enabled.
  @param[in]      EnableRowIncrement     - Specifies whether row increment is enabled.
  @param[in]      RowIncrementOrder      - Specifies row increment order.  The value must be 0 or 1.
  @param[in]      EnableColIncrement     - Specifies whether column increment is enabled.
  @param[in]      ColIncrementOrder      - Specifies column increment order.  The value must be 0 or 1.
  @param[in]      EnableCadbSeedsSave    - Specifies whether saving the CADB seeds is enabled.
  @param[in]      EnableCadbSeedsRestore - Specifies whether restoring the CADB seeds is enabled.
  @param[in]      EnableCadbSelect       - Specifies whether CADB patterns will be driven during select cycles.
  @param[in]      EnableCadbDeselect     - Specifies whether CADB patterns will be driven during deselect cycles.
  @param[in]      BaseSubseqType         - Every time the BaseSubseqRepeatRate is reached a Read or Write is performed for one command based on the following fields: BaseSubseqType indicates whether a read (BaseReadSubseqType) or write (BaseWriteSubseqType) operation will occur at the current base address.  EnableBaseInvertData indicates whether the read or write (BaseSubseqType) operation at the current base address is inverted (EnableBaseInvertData = TRUE) or not (EnableBaseInvertData = FALSE).
  @param[in]      BaseSubseqRepeatRate   - Specifies how often the BaseSubseqType operation occurs for 1 cacheline.  0 = No BaseSubseqType cacheline operations will occur during the Offset_Read or Offset_Write.  1 = Reserved value.  2 = Repeat the BaseSubseqType cacheline operation after every offset cacheline operation.  ...  31 = Repeat the BaseSubseqType cacheline operation after 30 offset cacheline operations.
  @param[in]      OffsetAddrUpdateRate   - Specifies the rate that the Current Offset Address is updated.  The value must be less than or equal to the MaxOffsetAddrUpdateRateVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SUBSEQ_OFFSET_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Subseq,
  IN BOOLEAN                   EnableBaseInvertData,
  IN BOOLEAN                   EnableRowIncrement,
  IN UINT8                     RowIncrementOrder,
  IN BOOLEAN                   EnableColIncrement,
  IN UINT8                     ColIncrementOrder,
  IN BOOLEAN                   EnableCadbSeedsSave,
  IN BOOLEAN                   EnableCadbSeedsRestore,
  IN BOOLEAN                   EnableCadbSelect,
  IN BOOLEAN                   EnableCadbDeselect,
  IN SUBSEQ_TYPE               BaseSubseqType,
  IN UINT32                    BaseSubseqRepeatRate,
  IN UINT32                    OffsetAddrUpdateRate
  );

/**
  Function used to set the sequence configuration.

  @param[in, out] This                      - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                    - Zero based CPU socket number.
  @param[in]      Controller                - Zero based controller number.
  @param[in]      Channel                   - Zero based channel number.
  @param[in]      StartDelay                - Specifies the delay period (in dclks) between the start of the test and the actual entry to Loopback.Pattern where the first SubSequence is executed immediately.  The value must be less than or equal to the MaxStartDelayVal value from the GetCpgcInfo() function.
  @param[in]      SubseqStartIndex          - Specifies the initial SubSequence pointer where a Sequence first enters Loopback.Pattern (start of test).  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param[in]      SubseqEndIndex            - When the subsequence pointed to by the SubseqEndIndex is completed the loop count is incremented by 1 and the current subsequence index is reset to the SubseqStartIndex.  The SubseqEndIndex must be greater than or equal to the SubseqStartIndex value.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param[in]      LoopCount                 - Specifies how many iterations of the complete sequence loop takes place before a test stops, where a sequence loop is defined by the completion of all subsequences indicated by the SubseqStartIndex to SubseqEndIndex. The value must be less than or equal to the MaxLoopCountVal from the GetCpgcInfo() function.  If the IsLoopCountExponential value from the GetCpgcInfo() function is TRUE then the number of loops is 2^(LoopCount-1).  If the IsLoopCountExponential value from the GetCpgcInfo() function is FALSE then the number of loops is simply LoopCount.  In both the linear of exponential cases, 0 is a special value means that the loop count is infinite.
  @param[in]      InitMode                  - Initialization mode.
  @param[in]      EnableGlobalControl       - Specifies whether global control is enabled for the given channel.
  @param[in]      EnableConstantWriteStrobe - Specifies whether constant write strobe is enabled.
  @param[in]      EnableDummyReads          - Specifies whether dummy reads are enabled.
  @param[in]      AddrUpdateRateMode        - Address update rate mode.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SEQ_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT32                    StartDelay,
  IN UINT8                     SubseqStartIndex,
  IN UINT8                     SubseqEndIndex,
  IN UINT32                    LoopCount,
  IN CPGC_INIT_MODE            InitMode,
  IN BOOLEAN                   EnableGlobalControl,
  IN BOOLEAN                   EnableConstantWriteStrobe,
  IN BOOLEAN                   EnableDummyReads,
  IN ADDR_UPDATE_RATE_MODE     AddrUpdateRateMode
  );

/**
  Function used to set the loop count.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      LoopCount  - Specifies how many iterations of the complete sequence loop takes place before a test stops, where a sequence loop is defined by the completion of all subsequences indicated by the SubseqStartIndex to SubseqEndIndex. The value must be less than or equal to the MaxLoopCountVal from the GetCpgcInfo() function.  If the IsLoopCountExponential value from the GetCpgcInfo() function is TRUE then the number of loops is 2^(LoopCount-1).  If the IsLoopCountExponential value from the GetCpgcInfo() function is FALSE then the number of loops is simply LoopCount.  In both the linear of exponential cases, 0 is a special value means that the loop count is infinite.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_LOOP_COUNT) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT32                    LoopCount
  );

/**
  Function used to set the sequence address configuration.

  @param[in, out] This                 - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket               - Zero based CPU socket number.
  @param[in]      Controller           - Zero based controller number.
  @param[in]      Channel              - Zero based channel number.
  @param[in]      EnableRankWrapCarry  - If a base rank address wrap occurs  and EnableRankWrapCarry is set to TRUE then a carry increment or decrement from the rank address (order N) will propagate to the N+1 order address field(s).
  @param[in]      EnableBankWrapCarry  - If a base bank address wrap occurs  and EnableBankWrapCarry is set to TRUE then a carry increment or decrement from the bank address (order N) will propagate to the N+1 order address field(s).
  @param[in]      EnableRowWrapCarry   - If a base row address wrap occurs and EnableRowWrapCarry is set to TRUE then a carry increment or decrement from the row address (order N) will propagate to the N+1 order address field(s).
  @param[in]      EnableColWrapCarry   - If a base column address wrap occurs  and EnableColWrapCarry is set to TRUE then a carry increment or decrement from the column address (order N) will propagate to the N+1 order address field(s).
  @param[in]      EnableRankAddrInvert - Setting to TRUE will cause the current rank address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param[in]      EnableBankAddrInvert - Setting to TRUE will cause the current bank address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param[in]      EnableRowAddrInvert  - Setting to TRUE will cause the current row address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param[in]      EnableColAddrInvert  - Setting to TRUE will cause the current column address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param[in]      AddrInvertRate       - Base address inversion rate.  The SetSeqConfig() function's AddrUpdateRateMode parameter specifies whether the base address inversion logic is based on number of cachelines written and read or on the number of sequences performed.  If AddrUpdateRateMode from the SetSeqConfig() function is CachelineAddrUpdateRate then the current base address is inverted based on 2^AddrInvertRate read and write cacheline transactions.  If AddrUpdateRateMode from the SetSeqConfig() function is LoopCountAddrUpdateRate then the current base address is inverted based on 2^AddrInvertRate loops through the sequence.  The value must be less than or equal to the MaxAddrInvertRateVal value from the GetCpgcInfo() function.
  @param[in]      RankAddrOrder        - Specifies the relative ordering of the rank address update logic in the nested for loop in relation to the bank, row, and column address update logic.  Any address fields set to the same order that roll over will cause a distinct carry of +1 or -1 to the next higher order address field (multiple simultaneous carries will cause only one increment/decrement).  All fields can be arbitrarily placed in any order as long as all address order fields are set in a continuous order starting from 0 and may not skip over an order number.  Example: setting 0,1,2,1 in the 4 fields (Col,Row,Bank,Rank) is legal; setting 0,2,2,3 is illegal (not continuous).  The value must be less than or equal to 3.
  @param[in]      BankAddrOrder        - Specifies the relative ordering of the bank address update logic in the nested for loop in relation to the rank, row, and column address update logic.  The value must be less than or equal to 3.
  @param[in]      RowAddrOrder         - Specifies the relative ordering of the row address update logic in the nested for loop in relation to the rank, bank, and column address update logic.  The value must be less than or equal to 3.
  @param[in]      ColAddrOrder         - Specifies the relative ordering of the column address update logic in the nested for loop in relation to the rank, bank, and row address update logic.  The value must be less than or equal to 3.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SEQ_ADDR_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   EnableRankWrapCarry,
  IN BOOLEAN                   EnableBankWrapCarry,
  IN BOOLEAN                   EnableRowWrapCarry,
  IN BOOLEAN                   EnableColWrapCarry,
  IN BOOLEAN                   EnableRankAddrInvert,
  IN BOOLEAN                   EnableBankAddrInvert,
  IN BOOLEAN                   EnableRowAddrInvert,
  IN BOOLEAN                   EnableColAddrInvert,
  IN UINT8                     AddrInvertRate,
  IN UINT8                     RankAddrOrder,
  IN UINT8                     BankAddrOrder,
  IN UINT8                     RowAddrOrder,
  IN UINT8                     ColAddrOrder
  );

/**
  Function used to set the sequence start address.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      RankAddr   - Rank start address.  This value is the index into the CPGC rank map (see the SetSeqRankMap() function).  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      BankAddr   - Bank start address.  If the CPGC engine supports a bank map (i.e., the MaxNumberBankMapEntried value from the GetCpgcInfo() function is greater than 0) then this value is is the index into the CPGC bank map (see the SetSeqBankMap() function) and the value must be less than the MaxNumberBankMapEntried value from the GetCpgcInfo() function.  If the CPGC engine does not support a bank map then the value must be less than the BankCount value from the GetDimmInfo function for the DIMMs that will be tested.
  @param[in]      RowAddr    - Row start address.  The value must be less than the RowSize value from the GetDimmInfo function for the DIMMs that will be tested.
  @param[in]      ColAddr    - Column start address.  The value must be less than the ColumnSize value from the GetDimmInfo function for the DIMMs that will be tested.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SEQ_START_ADDR) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     RankAddr,
  IN UINT8                     BankAddr,
  IN UINT32                    RowAddr,
  IN UINT32                    ColAddr
  );

/**
  Function used to set the sequence wrap address.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      RankAddr   - Rank start address.  This value is the index into the CPGC rank map (see the SetSeqRankMap() function).  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      BankAddr   - Bank start address.  If the CPGC engine supports a bank map (i.e., the MaxNumberBankMapEntried value from the GetCpgcInfo() function is greater than 0) then this value is is the index into the CPGC bank map (see the SetSeqBankMap() function) and the value must be less than the MaxNumberBankMapEntried value from the GetCpgcInfo() function.  If the CPGC engine does not support a bank map then the value must be less than the BankCount value from the GetDimmInfo function for the DIMMs that will be tested.
  @param[in]      RowAddr    - Row start address.  The value must be less than the RowSize value from the GetDimmInfo function for the DIMMs that will be tested.
  @param[in]      ColAddr    - Column start address.  The value must be less than the ColumnSize value from the GetDimmInfo function for the DIMMs that will be tested.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SEQ_WRAP_ADDR) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     RankAddr,
  IN UINT8                     BankAddr,
  IN UINT32                    RowAddr,
  IN UINT32                    ColAddr
  );

/**
  Function used to set the sequence address increment.

  @param[in, out] This            - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket          - Zero based CPU socket number.
  @param[in]      Controller      - Zero based controller number.
  @param[in]      Channel         - Zero based channel number.
  @param[in]      RankAddrIncMode - Rank address increment rate mode.
  @param[in]      RankAddrIncRate - Rank address increment rate.  In linear mode (see RankAddrIncMode), RankAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the RankAddrIncVal.  In exponential mode, 2^RankAddrIncRate defines how many times a specific rank address is repeated before adding the RankAddrIncVal.  If RankAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxRankAddrIncRateExponentVal from the GetCpgcInfo() function.  If RankAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxRankAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param[in]      RankAddrIncVal  - Rank start address increment value.  When dictated by RankAddrIncRate the current rank address will be modified by the RankAddrIncVal (which is a 2's compliment field).  A value of 0 means the rank address increment is effectively disabled since no offset will be applied.  Disabling the rank address increment by setting RankAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinRankAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxRankAddrIncVal from the GetCpgcInfo() function.
  @param[in]      BankAddrIncMode - Bank address increment rate mode.
  @param[in]      BankAddrIncRate - Bank address increment rate.  In linear mode (see BankAddrIncMode), BankAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the BankAddrIncVal.  In exponential mode, 2^BankAddrIncRate defines how many times a specific rank address is repeated before adding the BankAddrIncVal.  If BankAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxBankAddrIncRateExponentVal from the GetCpgcInfo() function.  If BankAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxBankAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param[in]      BankAddrIncVal  - Bank start address increment value.  When dictated by BankAddrIncRate the current bank address will be modified by the BankAddrIncVal (which is a 2's compliment field).  A value of 0 means the bank address increment is effectively disabled since no offset will be applied.  Disabling the bank address increment by setting BankAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinBankAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxBankAddrIncVal from the GetCpgcInfo() function.
  @param[in]      RowAddrIncMode  - Row address increment rate mode.
  @param[in]      RowAddrIncRate  - Row address increment rate.  In linear mode (see RowAddrIncMode), RowAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the RowAddrIncVal.  In exponential mode, 2^RowAddrIncRate defines how many times a specific rank address is repeated before adding the RowAddrIncVal.  If RowAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxRowAddrIncRateExponentVal from the GetCpgcInfo() function.  If RowAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxRowAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param[in]      RowAddrIncVal   - Row start address increment value.  When dictated by RowAddrIncRate the current row address will be modified by the RowAddrIncVal (which is a 2's compliment field.  A value of 0 means the row address increment is effectively disabled since no offset will be applied.  Disabling the row address increment by setting RowAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinRowAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxRowAddrIncVal from the GetCpgcInfo() function.
  @param[in]      ColAddrIncMode  - Column address increment rate mode.
  @param[in]      ColAddrIncRate  - Column address increment rate.  If ColAddrIncMode is ExponentialAddrIncMode then 2^ColAddrIncRate defines how many times a specific rank address is repeated before adding the ColAddrIncVal.  If ColAddrIncMode is LinearAddrIncMode then ColAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the ColAddrIncVal.  If ColAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxColAddrIncRateExponentVal from the GetCpgcInfo() function.  If ColAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxColAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param[in]      ColAddrIncVal   - Column start address increment value.  When dictated by ColAddrIncRate the current column address will be modified by the ColAddrIncVal (which is a 2's compliment field).  A value of 0 means the column address increment is effectively disabled since no offset will be applied.  Disabling the column address increment by setting column address increment to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinColAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxColAddrIncVal from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SEQ_ADDR_INC) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN ADDR_INC_MODE             RankAddrIncMode,
  IN UINT32                    RankAddrIncRate,
  IN INT8                      RankAddrIncVal,
  IN ADDR_INC_MODE             BankAddrIncMode,
  IN UINT32                    BankAddrIncRate,
  IN INT8                      BankAddrIncVal,
  IN ADDR_INC_MODE             RowAddrIncMode,
  IN UINT32                    RowAddrIncRate,
  IN INT16                     RowAddrIncVal,
  IN ADDR_INC_MODE             ColAddrIncMode,
  IN UINT32                    ColAddrIncRate,
  IN INT16                     ColAddrIncVal
  );

/**
  Function used to set the sequence rank map.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      PairCount     - Number of entries in the DimmRankPairs array.  The minimum value is 1.  The value must be less than or equal to the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      DimmRankPairs - Pointer to array of DIMM and physical rank pair values.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.  Note that the same rank may appear more than once in the map.  Note also that all ranks do not need to appear in the map.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SEQ_RANK_MAP) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     PairCount,
  IN CONST DIMM_RANK_PAIR      *DimmRankPairs
  );

/**
  Function used to set the sequence bank map.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      BankCount  - Number of entries in the Banks array.  The minimum value is 1.  The value must be less than or equal to the MaxNumberBankMapEntries value from the GetCpgcInfo() function.
  @param[in]      Banks      - Pointer to array of zero-based bank values.  The values must be less than the BankCount value from the GetDimmInfo function for the DIMMs that will be tested.  Note that the same bank may appear more than once in the map.  Note also that all banks do not need to appear in the map.

  @retval NotAvailable if the system does not support the bank mapping feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_SEQ_BANK_MAP) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     BankCount,
  IN CONST UINT8               *Banks
  );

/**
  Function used to set the sequence lower row address swizzles.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      Channel      - Zero based channel number.
  @param[in]      SwizzleCount - Number of entries in the Swizzles array.  The value must be less than or equal to the MaxNumberRowAddrSwizzleEntries value from the GetCpgcInfo() function.  If the SwizzleCount value is 0 (and the Swizzles value is NULL) then the row address swizzling will be configured so that all the lower row addresses are mapped to themselves; effectively disabling the swizzling.
  @param[in]      Swizzles     - Pointer to array of lower row address swizzle values.  The values must be unique and less than or equal to the MaxRowAddrSwizzleVal value from the GetCpgcInfo() function.  The array position indicates the row address that the specified row address will be swapped with, e.g., a value of 10 at the array index 1 means that row addresses 10 and 1 will be swapped.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SEQ_ROW_ADDR_SWIZZLE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     SwizzleCount,
  IN CONST UINT8               *Swizzles
  );

/**
  Function used to get the current sequence state.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     SeqState   - Pointer to buffer where sequence state will be stored.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_SEQ_STATE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  OUT SSA_SEQ_STATE            *SeqState
  );

/**
  Function used to set the CPGC stop mode.

  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Channel             - Zero based channel number.
  @param[in]      StopMode            - CPGC stop mode.
  @param[in]      StopOnNthErrorCount - Error count for stop-on-nth-any-lane error mode.  Minimum value is 1.  The value must be less than or equal to the MaxStopOnNthErrorCountVal value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_STOP_MODE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN CPGC_STOP_MODE            StopMode,
  IN UINT32                    StopOnNthErrorCount
  );

/**
  Function used to set the error counter mode.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      Counter     - Zero based error counter number.  The value must be less than or equal to the MaxNumberErrorCounters value from the GetCpgcInfo() function.
  @param[in]      Mode        - Error counter mode.
  @param[in]      ModeIndex   - Extra index used to provide additional information if needed by the mode selected. This indicates which lane, nibble, byte, or chunk has been selected.  If CounterMode value is LaneErrorCounterMode then the ModeIndex value must be less than the BusWidth value from the GetSystemInfo() function.  If CounterMode is NibbleErrorCounterMode then the ModeIndex value must be less than the BusWidth/4 value from the GetSystemInfo() function.  If CounterMode is ByteErrorCounterMode then the ModeIndex value must be less than the BusWidth/8 value from the GetSystemInfo() function.  If CounterMode is ChunkErrorCounterMode then the ModeIndex value must be less than 8.

  @retval NotAvailable if the given Mode value is not supported.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_ERROR_COUNTER_MODE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Counter,
  IN ERROR_COUNTER_MODE        Mode,
  IN UINT32                    ModeIndex
  );

/**
  Function used to set the lane validation mask.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      LaneMasks  - Pointer to array of lane bitmasks.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Note that this may not be the same logic as the hardware.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_LANE_VALIDATION_MASK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN CONST UINT8               *LaneMasks
  );

/**
  Function used to set the chunk validation mask.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      ChunkMask  - Mask for the chunks. One bit for each chunk of a cacheline.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Bit position 0 = chunk 0; bit position 1 = chunk 1, etc.  Note that this may not be the same logic as the hardware.

  @retval NotAvailable if the system does not provide chunk validation control.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_CHUNK_VALIDATION_MASK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     ChunkMask
  );

/**
  Function used to set the cacheline mask.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Channel       - Zero based channel number.
  @param[in]      CachelineMask - Mask for cachelines to be enabled.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Bit position 0 = cacheline 0; bit position 1 = cacheline 1, etc.  Note that this may not be the same logic as the hardware.

  @retval NotAvailable if the system does not provide cacheline validation control.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_CACHELINE_VALIDATION_MASK) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     CachelineMask
  );

/**
  Function used to get the lane error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      ChunkMask  - Mask for the chunks. One bit for each chunk.  This mask should set to be the same value as the mask being passed to the SetChunkValidationMask() function.  Products don't have separate error status for even and odd UI should ignore this parameter.
  @param[out]     Status     - Pointer to array where the lane error status values will be stored.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_LANE_ERROR_STATUS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     ChunkMask,
  OUT UINT8                    *Status
  );

/**
  Function used to get the chunk error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     Status     - Pointer to where the chunk error status will be stored.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred. Bit position 0 = chunk 0; bit position 1 = chunk 1, etc.

  @retval NotAvailable if the system does not provide chunk error status.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_GET_CHUNK_ERROR_STATUS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  OUT UINT8                     *Status
  );

/**
  Function used to get the rank error status.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     Status     - Pointer to where the phyical rank error status of the given Dimm will be stored.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred. Bit position 0 = rank 0; bit position 1 = rank 1, etc.

  @retval NotAvailable if the system does not provide rank error status.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_GET_RANK_ERROR_STATUS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  OUT UINT8                    *Status
  );

/**
  Function used to get the address associated with the last error detected.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[out]     Address    - Pointer to buffer where error address will be stored.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_ERROR_ADDRESS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  OUT SSA_ADDRESS              *Address
  );

/**
  Function used to get the error count.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket     - Zero based CPU socket number.
  @param[in]      Controller - Zero based controller number.
  @param[in]      Channel    - Zero based channel number.
  @param[in]      Counter    - Zero based counter number.  The value must be less than or equal to the MaxNumberErrorCounters value from the GetCpgcInfo() function.
  @param[out]     Count      - Pointer to where the error counter value will be stored.
  @param[out]     Overflow   - Pointer to where the error counter overflow status will be stored.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_ERROR_COUNT) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN UINT8                     Counter,
  OUT UINT32                   *Count,
  OUT BOOLEAN                  *Overflow
  );

/**
  Function used to set the DqDB error capture configuration.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket         - Zero based CPU socket number.
  @param[in]      Controller     - Zero based controller number.
  @param[in]      Channel        - Zero based channel number.
  @param[in]      Enable         - If set to TRUE then cachelines for which errors are detected are written to the DqDB starting at the StartCacheline and continuing to the EndCacheline before wrapping around to the StartCacheline.  If set to FALSE then the DqDB error capture feature is disabled.
  @param[in]      StartCacheline - The starting cacheline in the DqDB where the error status will be captured.  The value must be less than the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param[in]      EndCacheline   - The ending cacheline in the DqDB where the error status will be captured.  The value must be greater than or equal to the StartCacheline value.  The value must be less than the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_DQDB_ERROR_CAPTURE_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   Enable,
  IN UINT8                     StartCacheline,
  IN UINT8                     EndCacheline
  );

/**
  Function used to get the DqDB error capture status.

  @param[in, out] This             - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket           - Zero based CPU socket number.
  @param[in]      Controller       - Zero based controller number.
  @param[in]      Channel          - Zero based channel number.
  @param[out]     CurrentCacheline - Pointer to where the current DqDB error capture cacheline value will be stored.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_DQDB_ERROR_CAPTURE_STATUS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  OUT UINT8                    *CurrentCacheline
  );

/**
  Function used to set the local test control.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      Channel     - Zero based channel number.
  @param[in]      ClearErrors - Setting this parameter to TRUE causes all the channel's error counters and status to be cleared.
  @param[in]      StartTest   - Setting this parameter to TRUE starts the channel's CPGC engine.
  @param[in]      StopTest    - Setting this parameter to TRUE stops the channel's CPGC engine.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_LOCAL_TEST_CONTROL) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   ClearErrors,
  IN BOOLEAN                   StartTest,
  IN BOOLEAN                   StopTest
  );

/**
  Function used to set the global test control.  Settings apply to all channels whose EnableGlobalControl is set to TRUE (see SetSeqConfig function).

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket      - Zero based CPU socket number.
  @param[in]      Controller  - Zero based controller number.
  @param[in]      ClearErrors - Setting this parameter to TRUE causes all controller's error counters and status to be cleared.
  @param[in]      StartTest   - Setting this parameter to TRUE starts the channel's CPGC engine.
  @param[in]      StopTest    - Setting this parameter to TRUE stops the channel's CPGC engine.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_GLOBAL_TEST_CONTROL) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN BOOLEAN                   ClearErrors,
  IN BOOLEAN                   StartTest,
  IN BOOLEAN                   StopTest
  );

/**
  Function used to get the test done status.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket       - Zero based CPU socket number.
  @param[in]      Controller   - Zero based controller number.
  @param[in]      ChannelMask  - Channel mask.  Each bit represents a channel to be checked.  Bit value 1 = perform check; bit value 0 = do not perform check.  Bit position 0 = channel 0; bit position 1 = channel 1, etc.
  @param[out]     TestDoneMask - Pointer to where mask of per-channel test done status will be placed.  Bit value 1 = test complete; bit value 0 = test not complete or check not performed.  Bit position 0 = channel 0; bit position 1 = channel 1, etc.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_GET_TEST_DONE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     ChannelMask,
  OUT UINT8                    *TestDoneMask
  );

/**
  Function used to set the scrambler configuration.

  @param[in, out] This                - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket              - Zero based CPU socket number.
  @param[in]      Controller          - Zero based controller number.
  @param[in]      Enable              - Specifies whether the scrambler is enabled.
  @param[in]      ClockGateABMode     - Clock gate cipher A/B mode.
  @param[in]      ClockGateCMode      - Clock gate cipher C mode.
  @param[in]      EnableDbiAB         - Specifies whether DBI is enabled for cipher A/B.
  @param[in]      Key                 - Scrambler key.  The value must be less than or equal to the MaxScramblerKeyVal value from the GetCpgcInfo() function.
  @param[out]     PrevEnable          - Pointer to where the previous scrambler enable setting will be stored.  This value may be NULL.
  @param[out]     PrevClockGateABMode - Pointer to where the previous clock gate cipher A/B mode setting will be stored.  This value may be NULL.
  @param[out]     PrevClockGateCMode  - Pointer to where the previous clock gate cipher C mode setting will be stored.  This value may be NULL.
  @param[out]     PrevEnableDbiAB     - Pointer to where the previous DBI enable for cipher A/B setting will be stored.  This value may be NULL.
  @param[out]     PrevKey             - Pointer to where the previous scrambler key setting will be stored.  This value may be NULL.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SCRAMBLER_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN BOOLEAN                   Enable,
  IN CLOCK_GATE_AB_MODE        ClockGateABMode,
  IN CLOCK_GATE_C_MODE         ClockGateCMode,
  IN BOOLEAN                   EnableDbiAB,
  IN UINT16                    Key,
  OUT BOOLEAN                  *PrevEnable,
  OUT CLOCK_GATE_AB_MODE       *PrevClockGateABMode,
  OUT CLOCK_GATE_C_MODE        *PrevClockGateCMode,
  OUT BOOLEAN                  *PrevEnableDbiAB,
  OUT UINT16                   *PrevKey
  );

/**
  Function used to set the self-refresh configuration.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket        - Zero based CPU socket number.
  @param[in]      Controller    - Zero based controller number.
  @param[in]      Enable        - Specifies whether self-refreshes are enabled.
  @param[in]      IdleTimer     - Specifies the number of consecutive cycles without any transaction in order to enter self-refresh.  The value must be less than or equal to the MaxRefreshIdleTimerVal value from the GetCpgcInfo() function.
  @param[out]     PrevEnable    - Pointer to where the previous self-refresh enable setting will be stored.  This value may be NULL.
  @param[out]     PrevIdleTimer - Pointer to where the previous idle timer setting will be stored.  This value may be NULL.

  @retval Nothing.
**/
typedef
VOID
(EFIAPI * CLIENT_SET_SELF_REFRESH_CONFIG) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN BOOLEAN                   Enable,
  IN UINT32                    IdleTimer,
  OUT BOOLEAN                  *PrevEnable,
  OUT UINT32                   *PrevIdleTimer
  );

/**
  Function used to set the refresh override.

  @param[in, out] This                   - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                 - Zero based CPU socket number.
  @param[in]      Controller             - Zero based controller number.
  @param[in]      Channel                - Zero based channel number.
  @param[in]      EnablePanicRefreshOnly - If set to TRUE then opportunistic and high priority auto refreshes will be disabled for all ranks and refreshes will only occur when the panic refresh watermark has been exceeded.  If set to FALSE then refreshes will occur using the existing MC functional refresh logic.
  @param[in]      EnableStartTestSync    - If set to TRUE then all refresh/ZQ powerdown logic (counters, FSMs, etc.) is kept in the default reset state (no refresh/ZQ happening) while in Loopback.Marker during REUT Mode. Once the test starts and the transition to Loopback.Pattern occurs the refresh/ZQ logic will behave identical to normal functional MC mode based on the values programmed in the refresh/ZQ CRs. This allows deterministic and reproducible behavior across REUT tests with respect to refresh/ZQ since the logic is synchronized to the start of a REUT test.  IF set to FALSE then the refresh/ZQ logic will not be synchronized to the start of a REUT test and will always behave identical to normal functional MC mode based on the values programmed in the refresh/ZQ CRs, whether in Loopback.Marker or Loopback.Pattern.  NOTE: This bit should not be used in conjunction with Self Refresh.
  @param[in]      PairCount              - Number of entries in the DimmRankPairs array.  If you wish to have no rank's refresh overridden then set PairCnt=0 and DimmRankPairs=NULL.  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      DimmRankPairs          - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in the list, then the CPGC engine will disable executing any refresh to that rank while in REUT mode. If a DIMM/rank doesn't appear in the list then the CPGC engine will enable executing a refresh (assuming the MC has refreshes enabled) to that rank while in REUT mode based on the EnablePanicRefreshOnly parameter value.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.

  @retval NotAvailable if the system does not support the refresh override feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_REFRESH_OVERRIDE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   EnablePanicRefreshOnly,
  IN BOOLEAN                   EnableStartTestSync,
  IN UINT8                     PairCount,
  IN CONST DIMM_RANK_PAIR      *DimmRankPairs
  );

/**
  Function used to set the refresh override.

  @param[in, out] This                    - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                  - Zero based CPU socket number.
  @param[in]      Controller              - Zero based controller number.
  @param[in]      Channel                 - Zero based channel number.
  @param[in]      EnableZQCalAfterRefresh - If set to TRUE then a ZQ operation will always take place after a refresh occurs as long as the DIMM/rank does not appear in the DimmRankPairs array.  If set to FALSE then a ZQ operation will take place at the normal programmed period after a refresh occurs as long as the DIMM/rank does not appear in the DimmRankPairs array.
  @param[in]      PairCount               - Number of entries in the DimmRankPairs array.  If you wish to have no rank's ZQCal overridden then set PairCnt=0 and DimmRankPairs=NULL.  The value must be less than the product of the MaxNumberDimms and MaxNumberRanks values from the GetSystemInfo() function.
  @param[in]      DimmRankPairs           - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in the list, then the CPGC engine will disable executing any ZQCal to that rank while in REUT mode.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.

  @retval NotAvailable if the system does not support the ZQCal override feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_ZQCAL_OVERRIDE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   EnableZQCalAfterRefresh,
  IN UINT8                     PairCount,
  IN CONST DIMM_RANK_PAIR      *DimmRankPairs
  );

/**
  Function used to set the on-die-termination override.

  @param[in, out] This                  - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                - Zero based CPU socket number.
  @param[in]      Controller            - Zero based controller number.
  @param[in]      Channel               - Zero based channel number.
  @param[in]      EnableMprDdrTrain     - This parameter is set to TRUE during DDR training.
  @param[in]      ValCount              - Number of entries in the DimmRankPairs and OverrideVals arrays.  If you wish to have no rank's ODT overridden then set ValCnt=0, DimmRankPairs=NULL, and OverrideVals=NULL.  The value must be less than twice the MaxNumberDimms value from the GetSystemInfo() function.
  @param[in]      DimmRankPairs         - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in this array then its ODT value will be overridden with the value from the corresponding index in the OverrideVals array.  If a DIMM/rank does not appear in this array then its ODT will not be overridden.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.
  @param[in]      OverrideVals          - Pointer to array of override values.  Value 1 = enable ODT for that rank while in REUT mode; value 0 = disable ODT for that rank while in REUT mode.

  @retval NotAvailable if the system does not support the ODT override feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_ODT_OVERRIDE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   EnableMprDdrTrain,
  IN UINT8                     ValCount,
  IN CONST DIMM_RANK_PAIR      *DimmRankPairs,
  IN CONST UINT8               *OverrideVals
  );

/**
  Function used to set the clock-enable override.

  @param[in, out] This                  - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Socket                - Zero based CPU socket number.
  @param[in]      Controller            - Zero based controller number.
  @param[in]      Channel               - Zero based channel number.
  @param[in]      DisableCkePowerdown   - If set to TRUE then a CKE powerdown will be disabled when in REUT mode.  If set to FALSE then CKE power down will behave normally when in REUT.
  @param[in]      ValCount              - Number of entries in the DimmRankPairs and OverrideVals arrays.  If you wish to have no rank's CKE overridden then set ValCnt=0, DimmRankPairs=NULL, and OverrideVals=NULL.  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param[in]      DimmRankPairs         - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in this array then its CKE value will be overridden with the value from the corresponding index in the OverrideVals array.  If a DIMM/rank does not appear in this array then its CKE will not be overridden.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.
  @param[in]      OverrideVals          - Pointer to array of override values.  Value 1 = enable CKE for that rank while in REUT mode; value 0 = disable CKE for that rank while in REUT mode.

  @retval NotAvailable if the system does not support the CKE override feature.  Else Success.
**/
typedef
SSA_STATUS
(EFIAPI * CLIENT_SET_CKE_OVERRIDE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT8                     Socket,
  IN UINT8                     Controller,
  IN UINT8                     Channel,
  IN BOOLEAN                   DisableCkePowerDown,
  IN UINT8                     ValCount,
  IN CONST DIMM_RANK_PAIR      *DimmRankPairs,
  IN CONST UINT8               *OverrideVals
  );

/**
  BIOS-SSA Client Memory Configuration
**/
typedef struct _SSA_MEMORY_CLIENT_CONFIG {
  UINT32                                Revision;                   ///< Incremented when a backwards compatible binary change is made to the PPI.
  CLIENT_GET_CPGC_INFO                  GetCpgcInfo;                ///< Function used to get information about about the CPGC capabilities.
  CLIENT_SET_ADDRESS_AS_DATA            SetAddressAsData;           ///< Function used to enable/disable address as data.
  CLIENT_SET_DQDB_PATTERN               SetDqdbPattern;             ///< Function used to write pattern content to the DqDB.
  CLIENT_GET_DQDB_PATTERN               GetDqdbPattern;             ///< Function used to read pattern content from the DqDB.
  CLIENT_SET_DQDB_INDEXES               SetDqdbIndexes;             ///< Function used to set the DqDB start and end indexes as well as the DqDB increment rate.
  CLIENT_SET_DQDB_ECC_DATA_SOURCE       SetDqdbEccDataSource;       ///< Function used to set the DqDB source for the ECC signals.
  CLIENT_SET_DQDB_UNISEQ_MODE           SetDqdbUniseqMode;          ///< Function used to set the DqDB unisequencer mode.
  CLIENT_SET_DQDB_UNISEQ_WR_SEED        SetDqdbUniseqWrSeed;        ///< Function used to set the DqDB write unisequencer seed value.
  CLIENT_SET_DQDB_UNISEQ_RD_SEED        SetDqdbUniseqRdSeed;        ///< Function used to set the DqDB read unisequencer seed value.
  CLIENT_GET_DQDB_UNISEQ_WR_SEED        GetDqdbUniseqWrSeed;        ///< Function used to get the DqDB write unisequencer seed value.
  CLIENT_GET_DQDB_UNISEQ_RD_SEED        GetDqdbUniseqRdSeed;        ///< Function used to get the DqDB read unisequencer seed value.
  CLIENT_SET_DQDB_UNISEQ_LMN            SetDqdbUniseqLmn;           ///< Function used to set the DqDB unisequencer L/M/N values.
  CLIENT_SET_DQDB_UNISEQ_LFSR_CONFIG    SetDqdbUniseqLfsrConfig;    ///< Function used to set the DqDB unisequencer LFSR configuration.
  CLIENT_SET_DQDB_INVERT_DC_CONFIG      SetDqdbInvertDcConfig;      ///< Function used to set the DqDB Invert/DC configuration.
  CLIENT_SET_CADB_CONFIG                SetCadbConfig;              ///< Function used to set the CADB pattern generation configuration.
  CLIENT_SET_CADB_PATTERN               SetCadbPattern;             ///< Function used to write pattern content to the CADB.
  CLIENT_SET_CADB_UNISEQ_MODE           SetCadbUniseqMode;          ///< Function used to set the CADB unisequencer modes.
  CLIENT_SET_CADB_UNISEQ_SEED           SetCadbUniseqSeed;          ///< Function used to set the CADB unisequencer seed values.
  CLIENT_SET_CADB_UNISEQ_LMN            SetCadbUniseqLmn;           ///< Function used to set the CADB unisequencer L/M/N values.
  CLIENT_SET_DUMMY_READ_LMN             SetDummyReadLmn;            ///< Function used to set the Dummy Read LMN values.
  CLIENT_SET_DUMMY_READ_BANK_MASK       SetDummyReadBankMask;       ///< Function used to set the Dummy Read bank mask.
  CLIENT_SET_DUMMY_READ_RANKS           SetDummyReadRanks;          ///< Function used to set the Dummy Read ranks.
  CLIENT_SET_SUBSEQ_CONFIG              SetSubseqConfig;            ///< Function used to set the subsequence configuration.
  CLIENT_SET_SUBSEQ_OFFSET_CONFIG       SetSubseqOffsetConfig;      ///< Function used to set the subsequence offset configuration.
  CLIENT_SET_SEQ_CONFIG                 SetSeqConfig;               ///< Function used to set the sequence configuration.
  CLIENT_SET_LOOP_COUNT                 SetLoopCount;               ///< Function used to set the loop count.
  CLIENT_SET_SEQ_ADDR_CONFIG            SetSeqAddrConfig;           ///< Function used to set the sequence address configuration.
  CLIENT_SET_SEQ_START_ADDR             SetSeqStartAddr;            ///< Function used to set the sequence start address.
  CLIENT_SET_SEQ_WRAP_ADDR              SetSeqWrapAddr;             ///< Function used to set the sequence wrap address.
  CLIENT_SET_SEQ_ADDR_INC               SetSeqAddrInc;              ///< Function used to set the sequence address increment.
  CLIENT_SET_SEQ_RANK_MAP               SetSeqRankMap;              ///< Function used to set the sequence rank map.
  CLIENT_SET_SEQ_BANK_MAP               SetSeqBankMap;              ///< Function used to set the sequence bank map.
  CLIENT_SET_SEQ_ROW_ADDR_SWIZZLE       SetSeqRowAddrSwizzle;       ///< Function used to set the sequence lower row address swizzle.
  CLIENT_GET_SEQ_STATE                  GetSeqState;                ///< Function used to get the sequence state.
  CLIENT_SET_STOP_MODE                  SetStopMode;                ///< Function used to set the CPGC stop mode.
  CLIENT_SET_ERROR_COUNTER_MODE         SetErrorCounterMode;        ///< Function used to set the error counter mode.
  CLIENT_SET_LANE_VALIDATION_MASK       SetLaneValidationMask;      ///< Function used to set the lane validation mask.
  CLIENT_SET_CHUNK_VALIDATION_MASK      SetChunkValidationMask;     ///< Function used to set the chunk validation mask.
  CLIENT_SET_CACHELINE_VALIDATION_MASK  SetCachelineValidationMask; ///< Function used to set the cacheline validation mask.
  CLIENT_GET_LANE_ERROR_STATUS          GetLaneErrorStatus;         ///< Function used to get the lane error status.
  CLIENT_GET_CHUNK_ERROR_STATUS         GetChunkErrorStatus;        ///< Function used to get the chunk error status.
  CLIENT_GET_RANK_ERROR_STATUS          GetRankErrorStatus;         ///< Function used to get the rank error status.
  CLIENT_GET_ERROR_ADDRESS              GetErrorAddress;            ///< Function used to get the address associated with the last error detected.
  CLIENT_GET_ERROR_COUNT                GetErrorCount;              ///< Function used to get the error count.
  CLIENT_SET_DQDB_ERROR_CAPTURE_CONFIG  SetDqdbErrorCaptureConfig;  ///< Function used to set the DqDB error capture configuration.
  CLIENT_GET_DQDB_ERROR_CAPTURE_STATUS  GetDqdbErrorCaptureStatus;  ///< Function used to get the DqDB error capture status.
  CLIENT_SET_LOCAL_TEST_CONTROL         SetLocalTestControl;        ///< Function used to set the local test control.
  CLIENT_SET_GLOBAL_TEST_CONTROL        SetGlobalTestControl;       ///< Function used to set the global test control.
  CLIENT_GET_TEST_DONE                  GetTestDone;                ///< Function used to get the test done status.
  CLIENT_SET_SCRAMBLER_CONFIG           SetScramblerConfig;         ///< Function used to set the scrambler configuration.
  CLIENT_SET_SELF_REFRESH_CONFIG        SetSelfRefreshConfig;       ///< Function used to set the self-refresh configuration.
  CLIENT_SET_REFRESH_OVERRIDE           SetRefreshOverride;         ///< Function used to set the refresh override.
  CLIENT_SET_ZQCAL_OVERRIDE             SetZQCalOverride;           ///< Function used to set the ZQCal override.
  CLIENT_SET_ODT_OVERRIDE               SetOdtOverride;             ///< Function used to set the on-die-termination override.
  CLIENT_SET_CKE_OVERRIDE               SetCkeOverride;             ///< Function used to set the clock-enable override.
} SSA_MEMORY_CLIENT_CONFIG;

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // _SsaMemoryClientConfig_h_
