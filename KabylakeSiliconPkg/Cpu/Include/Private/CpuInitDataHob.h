/** @file
  Struct and GUID definitions for CpuInitDataHob.

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
#ifndef _CPU_INIT_DATA_HOB_H_
#define _CPU_INIT_DATA_HOB_H_

#include <Ppi/SiPolicy.h>

extern EFI_GUID gCpuInitDataHobGuid;

#define MAX_PROCESSOR_THREADS  0x40

///
/// CPU Configuration Structure passed from PEI to DXE phase
///
typedef struct {
  UINT32 ApHandoffManner           : 2;
  UINT32 ApIdleManner              : 2;
  UINT32 EnableDts                 : 2;
  UINT32 HdcControl                : 2;
  UINT32 Hwp                       : 2;
  UINT32 ConfigTdpBios             : 1;
  UINT32 RsvdBits                  :21;
  UINT8  SmmbaseSwSmiNumber;
  UINT8  Rsvd[3];
} CPU_CONFIG_DATA;

///
/// This HOB is used to pass only the required information from PEI for DXE consumption.
///
typedef struct {
  UINT32                 Revision;
  EFI_PHYSICAL_ADDRESS   CpuConfigData;     ///< CPU RC Config for DXE consumption
  EFI_PHYSICAL_ADDRESS   CpuGnvsPointer;    ///< CPU_GLOBAL_NVS_AREA Pointer.
  EFI_PHYSICAL_ADDRESS   MpData;            ///< Deprecated. Points to ACPI_CPU_DATA structure with multiprocessor data.
  EFI_PHYSICAL_ADDRESS   FvidTable;         ///< FVID Table.
  UINT32                 SiliconInfo;       ///< SILICON_INFO data
} CPU_INIT_DATA_HOB;

#endif
