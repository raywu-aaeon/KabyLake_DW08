/** @file
  Support for IO expander TCA6424.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2017 Intel Corporation.

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

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <PchAccess.h>
#include <Library/PchSerialIoLib.h>

#define WAIT_1_SECOND            1600000000 //1.6 * 10^9
#ifndef MINTREE_FLAG
#define MAX_DATA_COUNT           6
#define BLOCK_COMMAND_WAIT       0
#define BLOCK_COMMAND_I2C_READ   1
#define BLOCK_COMMAND_I2C_WRITE  2
#endif

EFI_STATUS
I2cWriteRead (
  IN UINTN  MmioBase,
  IN UINT8  SlaveAddress,
// AMI_OVERRIDE_START >>> Change WriteLength and ReadLength to UINT16.
#if 0
  IN UINT8  WriteLength,
#else
  IN UINT16 WriteLength,
#endif
// AMI_OVERRIDE_END <<< Change WriteLength and ReadLength to UINT16.
  IN UINT8  *WriteBuffer,
// AMI_OVERRIDE_START >>> Change WriteLength and ReadLength to UINT16.
#if 0
  IN UINT8  ReadLength,
#else
  IN UINT16 ReadLength,
#endif
// AMI_OVERRIDE_END <<< Change WriteLength and ReadLength to UINT16.
  IN UINT8  *ReadBuffer,
  IN UINT64  TimeBudget
  );

#ifndef MINTREE_FLAG
EFI_STATUS
I2cBlockWriteRead(
  IN PCH_SERIAL_IO_CONTROLLER,
  IN UINT8  SlaveAddress,
  IN UINT8 *I2cCommandBlock,
  IN UINT32 BlockSize
  );
#endif
