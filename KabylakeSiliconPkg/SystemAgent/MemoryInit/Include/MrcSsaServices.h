/** @file
  This file contains the definition of the SSA services PPI.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2012 - 2017 Intel Corporation.

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
#ifndef __MRC_SSA_SERVICES_H__
#define __MRC_SSA_SERVICES_H__

#ifdef SSA_FLAG
#pragma pack (push, 1)

typedef struct _SSA_BIOS_SERVICES_PPI SSA_BIOS_SERVICES_PPI;

#ifdef SSA_CLIENT_FLAG

// Uncomment this to turn [off|on] additional SSA debug messages.
// #define SSA_DEBUG_PRINT                          (1)

#ifdef SSA_DEBUG_PRINT
#define SSA_DEBUG_MSG(DEBUG, LEVEL, FORMAT, ...) SsaStackCheck (DEBUG); MrcPrintf (DEBUG, LEVEL, FORMAT, ##__VA_ARGS__)
#define SSA_PRINT_MEMORY(DEBUG, START, SIZE)     MrcPrintMemory (DEBUG, START, SIZE)
#define SSA_PARAM_ERROR_CHECK           (1)
#else
#define SSA_DEBUG_MSG(DEBUG, LEVEL, FORMAT, ...)
#define SSA_PRINT_MEMORY(DEBUG, START, SIZE)
#endif
#endif // SSA_CLIENT_FLAG

#include "MrcCommonTypes.h"

///
/// SSA function return error codes.
///
typedef enum {
  Success,                                ///< The function completed successfully.
  Failure,                                ///< Generic, non specified failure.
  NotYetAvailable,                        ///< The function is not yet available.
  NotAvailable,                           ///< The function or selected item is not available.
  UnsupportedValue,                       ///< A function parameter is incorrect.
  SsaStatusMax,                           ///< SSA_STATUS enumeration maximum value.
  SsaStatusDelim = INT32_MAX              ///< This value ensures the enum size is consistent on both sides of the PPI.
} SSA_STATUS;

#pragma pack (pop)

#ifdef SIM_BUILD
#include <malloc.h>
#endif // SIM_BUILD

#ifdef SSA_SERVER_FLAG
#include "SysHost.h"
#endif // SSA_SERVER_FLAG

#if defined(SIM_BUILD) || defined(MINIBIOS_BUILD)
#pragma pack (push, 1)
typedef UINT32 EFI_STATUS;

#ifdef SIM_BUILD
#ifndef CHAR8
typedef char CHAR8;
#endif
#endif // SIM_BUILD

#ifndef EFIAPI
#define EFIAPI __cdecl
#endif
#define IN
#define OUT

///
/// EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS
///
typedef struct {
  ///
  /// 8-bit register offset within the PCI configuration space for a given device's function
  /// space.
  ///
  UINT8   Register;
  ///
  /// Only the 3 least-significant bits are used to encode one of 8 possible functions within a
  /// given device.
  ///
  UINT8   Function;
  ///
  /// Only the 5 least-significant bits are used to encode one of 32 possible devices.
  ///
  UINT8   Device;
  ///
  /// 8-bit value to encode between 0 and 255 buses.
  ///
  UINT8   Bus;
  ///
  /// Register number in PCI configuration space. If this field is zero, then Register is used
  /// for the register number. If this field is non-zero, then Register is ignored and this field
  /// is used for the register number.
  ///
  UINT32  ExtendedRegister;
} EFI_PEI_PCI_CFG_PPI_PCI_ADDRESS;

typedef UINT64 EFI_PHYSICAL_ADDRESS;

typedef struct {
  UINT32  Data1;
  UINT16  Data2;
  UINT16  Data3;
  UINT8   Data4[8];
} EFI_GUID;

#define DEBUG_WARN      0x00000002  // Warnings
#define DEBUG_LOAD      0x00000004  // Load events
#define DEBUG_INFO      0x00000040  // Informational debug messages
#define DEBUG_EVENT     0x00080000  // Event messages
#define DEBUG_ERROR     0x80000000  // Error

#define MAX(a, b)                       \
  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)                       \
  (((a) < (b)) ? (a) : (b))
#pragma pack (pop)

#endif // SIM_BUILD || MINIBIOS_BUILD

#ifdef SSA_CLIENT_FLAG
#ifdef MRC_MINIBIOS_BUILD
#include "MrcMiniBiosEfiDefs.h"
#else
#include <Library/DebugLib.h>
#include "SsaCallbackPeim.h"
#endif // MRC_MINIBIOS_BUILD
#include "MrcInterface.h"
#define SSA_CONST const
#endif // SSA_CLIENT_FLAG

#include "SsaCommonConfig.h"
#include "SsaResultsConfig.h"
#include "SsaMemoryConfig.h"
#include "SsaMemoryClientConfig.h"
#include "SsaMemoryServerConfig.h"
#include "SsaMemoryDdrtConfig.h"

#pragma pack (push, 1)

///
/// SSA BIOS Services PPI
///
struct _SSA_BIOS_SERVICES_PPI {
  UINT32                   Revision;                  ///< Incremented when a backwards compatible binary change is made to the PPI.
  SSA_COMMON_CONFIG        *SsaCommonConfig;          ///< Pointer to the BIOS-SSA common functions.
  SSA_RESULTS_CONFIG       *SsaResultsConfig;         ///< Pointer to the BIOS results related functions.
  SSA_MEMORY_CONFIG        *SsaMemoryConfig;          ///< Pointer to the BIOS-SSA memory related functions.
  SSA_MEMORY_CLIENT_CONFIG *SsaMemoryClientConfig;    ///< Pointer to the BIOS-SSA client memory related functions.
  SSA_MEMORY_SERVER_CONFIG *SsaMemoryServerConfig;    ///< Pointer to the BIOS-SSA server memory related functions.
  SSA_MEMORY_DDRT_CONFIG   *SsaMemoryDdrtConfig;      ///< Pointer to the BIOS-SSA DDR-T memory related functions.
};

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // __MRC_SSA_SERVICES_H__
