/** @file
  This library provides SMM functions for IO and PCI IO access.
  These can be used to save size and simplify code.
  All contents must be runtime and SMM safe.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2014 Intel Corporation.

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

#ifndef _SMM_IO_LIB_H_
#define _SMM_IO_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiMultiPhase.h>
#include <Uefi/UefiSpec.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Pi/PiDxeCis.h>
#include <Library/SmmServicesTableLib.h>
#include <IndustryStandard/Pci22.h>



///
/// Global variables that must be defined and initialized to use this library
///

///
/// Definitions
///
#define ICH_ACPI_TIMER_MAX_VALUE  0x1000000 ///< The timer is 24 bit overflow
///
/// Pci I/O related data structure deifinition
///
typedef enum {
  SmmPciWidthUint8  = 0,
  SmmPciWidthUint16 = 1,
  SmmPciWidthUint32 = 2,
  SmmPciWidthUint64 = 3,
  SmmPciWidthMaximum
} SMM_PCI_IO_WIDTH;

#define SMM_PCI_ADDRESS(bus, dev, func, reg) \
        ((UINT64) ((((UINT32) bus) << 24) + (((UINT32) dev) << 16) + (((UINT32) func) << 8) + ((UINT32) reg)))

typedef struct {
  UINT8  Register;
  UINT8  Function;
  UINT8  Device;
  UINT8  Bus;
  UINT32 ExtendedRegister;
} SMM_PCI_IO_ADDRESS;

///
/// CPU I/O Access Functions
///
/**
  Do a one byte IO read

  @param[in] Address - IO address to read

  @retval Data read
**/
UINT8
SmmIoRead8 (
  IN UINT16 Address
  );

/**
  Do a one byte IO write

  @param[in] Address - IO address to write
  @param[in] Data    - Data to write
**/
VOID
SmmIoWrite8 (
  IN UINT16 Address,
  IN UINT8  Data
  );

/**
  Do a two byte IO read

  @param[in] Address - IO address to read

  @retval Data read
**/
UINT16
SmmIoRead16 (
  IN UINT16 Address
  );

/**
  Do a two byte IO write

  @param[in] Address - IO address to write
  @param[in] Data    - Data to write
**/
VOID
SmmIoWrite16 (
  IN UINT16 Address,
  IN UINT16 Data
  );

/**
  Do a four byte IO read

  @param[in] Address - IO address to read

  @retval Data read
**/
UINT32
SmmIoRead32 (
  IN UINT16 Address
  );

/**
  Do a four byte IO write

  @param[in] Address - IO address to write
  @param[in] Data    - Data to write
**/
VOID
SmmIoWrite32 (
  IN UINT16 Address,
  IN UINT32 Data
  );

/**
  Do a one byte Memory write

  @param[in] Dest - Memory address to write
  @param[in] Data - Data to write
**/
VOID
SmmMemWrite8 (
  IN UINT64 Dest,
  IN UINT8  Data
  );

/**
  Do a one byte Memory read

  @param[in] Dest - Memory address to read

  @retval Data read
**/
UINT8
SmmMemRead8 (
  IN UINT64 Dest
  );

/**
  Do a two bytes Memory write

  @param[in] Dest - Memory address to write
  @param[in] Data - Data to write
**/
VOID
SmmMemWrite16 (
  IN UINT64 Dest,
  IN UINT16 Data
  );

/**
  Do a two bytes Memory read

  @param[in]  Dest - Memory address to read

  @retval Data read
**/
UINT16
SmmMemRead16 (
  IN UINT64 Dest
  );

/**
  Do a four bytes Memory write

  @param[in] Dest - Memory address to write
  @param[in] Data - Data to write
**/
VOID
SmmMemWrite32 (
  IN UINT64 Dest,
  IN UINT32 Data
  );

/**
  Do a four bytes Memory read

  @param[in] Dest - Memory address to read

  @retval Data read
**/
UINT32
SmmMemRead32 (
  IN UINT64 Dest
  );

/**
  Do a four bytes Memory read, then AND with Data, then write back to the same address

  @param[in] Dest - Memory address to write
  @param[in] Data - Data to do AND
**/
VOID
SmmMemAnd32 (
  IN UINT64 Dest,
  IN UINT32 Data
  );
///
/// Pci Configuration Space access functions definition
///
/**
  Read value from the specified PCI config space register

  @param[in] Width   - The width (8, 16 or 32 bits) of accessed pci config space register
  @param[in] Address - The address of the accessed pci register (bus, dev, func, offset)
  @param[in] Buffer  - The returned value

  @retval EFI_SUCCESS           - All operations successfully
  @retval EFI_INVALID_PARAMETER - Width is not valid or dosn't match register address
  @retval Other error code      - If any error occured when calling libiary functions
**/
EFI_STATUS
SmmPciCfgRead (
  IN SMM_PCI_IO_WIDTH   Width,
  IN SMM_PCI_IO_ADDRESS *Address,
  IN OUT VOID           *Buffer
  );
/**
  Write value into the specified PCI config space register

  @param[in] Width   - The width (8, 16 or 32 bits) of accessed pci config space register
  @param[in] Address - The address of the accessed pci register (bus, dev, func, offset)
  @param[in] Buffer  - The returned value

  @retval EFI_SUCCESS           - All operations successfully
  @retval EFI_INVALID_PARAMETER - Width is not valid or dosn't match register address
  @retval Other error code      - If any error occured when calling libiary functions
**/
EFI_STATUS
SmmPciCfgWrite (
  IN SMM_PCI_IO_WIDTH   Width,
  IN SMM_PCI_IO_ADDRESS *Address,
  IN OUT VOID           *Buffer
  );
#endif
