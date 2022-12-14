/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2018 Intel Corporation.

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


#ifndef _LPIT_H_
#define _LPIT_H_

//
// Statements that include other files
//

#include <IndustryStandard/Acpi20.h>

#include "LowPowerIdleTable.h"

// signature "LPIT"
#define ACPI_LOW_POWER_IDLE_TABLE_SIGNATURE  0x5449504c
#define ACPI_OEM_LPIT_REVISION               0x00000000

#define ACPI_LOW_POWER_IDLE_MWAIT_TYPE          0x0
#define ACPI_LOW_POWER_IDLE_DEFAULT_FLAG        {{ 0x0, 0x0, 0x0 }}
#define ACPI_LOW_POWER_IDLE_NO_RES_COUNTER_FLAG 0x2
#define ACPI_LOW_POWER_IDLE_RES_FREQ_TSC        0x0

//
// LPI state count
//
#define ACPI_LPI_STATE_COUNT            2
#define ACPI_LPI_STATE_COUNT_RS2_PLUS   1

//
// LPI TRIGGER (HW C10)
//
#define ACPI_LPI_TRIGGER {0x7F,0x1,0x2,0x0,0x60}

//
// LPI residency counter (HW C10)
//
#define  C10_RESIDENCY_COUNTER_MSR  0x632
#define  ACPI_LPI_RES_C10_COUNTER    {0x7F,64,0x0,0x0,0x632}

//
// LPI residency counter (SLP S0)
//
#define  ACPI_LPI_RES_SLP_S0_COUNTER {0x0,32,0x0,0x3,0xFF}

//
// LPI DUMMY COUNTER
//
#define  ACPI_DUMMY_RES_COUNTER   {0x0,0,0x0,0x0,0x0}


//
// LPI break-even residency in us (HW C10)
//
#define  ACPI_LPI_MIN_RES   30000

//
// LPI latency in us (HW C10)
//
#define  ACPI_LPI_LATENCY   3000

//
// LPI ID (HW C10 audio)
//
#define  ACPI_LPI_AUDIO_ID   0

//
// LPI ID (HW C10 CS)
//
#define  ACPI_LPI_CS_ID            1
#define  ACPI_LPI_CS_ID_RS2_PLUS   0

//
//  LPI ACPI table header
//
#pragma pack(1)

typedef struct _ACPI_LOW_POWER_IDLE_TABLE {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_MWAIT_LPI_STATE_DESCRIPTOR     LpiStates[ACPI_LPI_STATE_COUNT];
} ACPI_LOW_POWER_IDLE_TABLE;


//
// LPIT table with only 1 LPI states entries are required for Windows RS2 PLUS (RS3 onwards).
//
typedef struct _ACPI_LOW_POWER_IDLE_TABLE_RS2_PLUS {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_MWAIT_LPI_STATE_DESCRIPTOR     LpiStates[ACPI_LPI_STATE_COUNT_RS2_PLUS];
} ACPI_LOW_POWER_IDLE_TABLE_RS2_PLUS;
#pragma pack()


#endif //_LPIT_H_
