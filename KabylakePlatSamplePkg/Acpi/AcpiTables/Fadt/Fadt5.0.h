/** @file
  This file describes the contents of the ACPI Fixed ACPI Description Table
  (FADT).  All changes to the FADT contents should be done in this file.

@copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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

#ifndef _FADT_H_
#define _FADT_H_

//
// Statements that include other files
//

#include <IndustryStandard/Acpi50.h>

#ifdef PCH_CNL
#include "Register/PchRegsPmcCnl.h"
#else
#include "Register/PchRegsPmc.h"
#endif

//
// FADT Definitions
//
#ifndef EFI_ACPI_OEM_FADT_REVISION
#define EFI_ACPI_OEM_FADT_REVISION  0x00000000
#endif

//
// For ACPI 1.0 support
//
#define EFI_ACPI_INT_MODEL            0x00
/*
1 Desktop
2 Mobile
3 Workstation
4 Enterprise Server
5 SOHO Server
6 Appliance PC
7 Performance Server
*/
#define EFI_ACPI_PREFERRED_PM_PROFILE 0x02
#define EFI_ACPI_SCI_INT              0x0009
#define EFI_ACPI_SMI_CMD              0x000000B2
#define EFI_ACPI_S4_BIOS_REQ          0xF2
#define EFI_ACPI_PSTATE_CNT           0x00
#define EFI_ACPI_GPE1_BASE            0x10
#define EFI_ACPI_CST_CNT              0x0
#define EFI_ACPI_P_LVL2_LAT           101
#define EFI_ACPI_P_LVL3_LAT           1001
#define EFI_ACPI_FLUSH_SIZE           0x0000
#define EFI_ACPI_FLUSH_STRIDE         0x0000
#define EFI_ACPI_DUTY_OFFSET          0x01
#define EFI_ACPI_DUTY_WIDTH           0x03
#define EFI_ACPI_DAY_ALRM             0x0D
#define EFI_ACPI_MON_ALRM             0x00
#define EFI_ACPI_CENTURY              0x32

//
// IA-PC Boot Architecture Flags, see ACPI 5.0 table specification and Acpi50.h
// We should not use EFI_ACPI_5_0_8042 for legacy free platform, or XP reset fail. No impact to mobile.
//
#define EFI_ACPI_IAPC_BOOT_ARCH (EFI_ACPI_5_0_LEGACY_DEVICES)

//
// Fixed Feature Flags, see ACPI 2.0 table specification and Acpi2_0.h
//
#define EFI_ACPI_FIXED_FEATURE_FLAGS (\
  EFI_ACPI_5_0_USE_PLATFORM_CLOCK | \
  EFI_ACPI_5_0_WBINVD | \
  EFI_ACPI_5_0_DCK_CAP | \
  EFI_ACPI_5_0_RTC_S4 | \
  EFI_ACPI_5_0_SLP_BUTTON | \
  EFI_ACPI_5_0_PROC_C1 | \
  EFI_ACPI_5_0_RESET_REG_SUP  \
  )

//
// PM1A Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH         0x20
#define EFI_ACPI_PM1A_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_EVT_BLK_ADDRESS           FixedPcdGet16 (PcdAcpiBaseAddress)

//
// PM1B Event Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_EVT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_EVT_BLK_ADDRESS           0x0000000000000000

//
// PM1A Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH         0x10
#define EFI_ACPI_PM1A_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1A_CNT_BLK_ADDRESS           (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x04)

//
// PM1B Control Register Block Generic Address Information
//
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS_SPACE_ID  EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH         0x00
#define EFI_ACPI_PM1B_CNT_BLK_BIT_OFFSET        0x00
#define EFI_ACPI_PM1B_CNT_BLK_ADDRESS           0x0000000000000000

//
// PM2 Control Register Block Generic Address Information
//
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS_SPACE_ID   EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH          0x08
#define EFI_ACPI_PM2_CNT_BLK_BIT_OFFSET         0x00
#define EFI_ACPI_PM2_CNT_BLK_ADDRESS            (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x50)

//
// Power Management Timer Control Register Block Generic Address
// Information
//
#define EFI_ACPI_PM_TMR_BLK_ADDRESS_SPACE_ID    EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_PM_TMR_BLK_BIT_WIDTH           0x20
#define EFI_ACPI_PM_TMR_BLK_BIT_OFFSET          0x00
#define EFI_ACPI_PM_TMR_BLK_ADDRESS             (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x08)

//
// General Purpose Event 0 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE0_BLK_ADDRESS_SPACE_ID      EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_GPE0_BLK_BIT_WIDTH             0x100
#define EFI_ACPI_GPE0_BLK_BIT_OFFSET            0x00
#ifdef PCH_CNL
#define EFI_ACPI_GPE0_BLK_ADDRESS               (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + R_PCH_ACPI_GPE0_STS_31_0)
#else
#define EFI_ACPI_GPE0_BLK_ADDRESS               (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x80)
#endif

//
// General Purpose Event 1 Register Block Generic Address
// Information
//
#define EFI_ACPI_GPE1_BLK_ADDRESS_SPACE_ID      EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_GPE1_BLK_BIT_WIDTH             0x00
#define EFI_ACPI_GPE1_BLK_BIT_OFFSET            0x00
#define EFI_ACPI_GPE1_BLK_ADDRESS               0x0000000000000000

//
// Reset Register Generic Address Information
//
#define EFI_ACPI_RESET_REG_ADDRESS_SPACE_ID     EFI_ACPI_5_0_SYSTEM_IO
#define EFI_ACPI_RESET_REG_BIT_WIDTH            0x08
#define EFI_ACPI_RESET_REG_BIT_OFFSET           0x00
#define EFI_ACPI_RESET_REG_ADDRESS              0x0000000000000CF9
#define EFI_ACPI_RESET_VALUE                    0x06

//
// Number of bytes decoded by PM1 event blocks (a and b)
//
#define EFI_ACPI_PM1_EVT_LEN ((EFI_ACPI_PM1A_EVT_BLK_BIT_WIDTH + EFI_ACPI_PM1B_EVT_BLK_BIT_WIDTH) / 8)

//
// Number of bytes decoded by PM1 control blocks (a and b)
//
#define EFI_ACPI_PM1_CNT_LEN ((EFI_ACPI_PM1A_CNT_BLK_BIT_WIDTH + EFI_ACPI_PM1B_CNT_BLK_BIT_WIDTH) / 8)

//
// Number of bytes decoded by PM2 control block
//
#define EFI_ACPI_PM2_CNT_LEN (EFI_ACPI_PM2_CNT_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by PM timer block
//
#define EFI_ACPI_PM_TMR_LEN (EFI_ACPI_PM_TMR_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by GPE0 block
//
#define EFI_ACPI_GPE0_BLK_LEN (EFI_ACPI_GPE0_BLK_BIT_WIDTH / 8)

//
// Number of bytes decoded by GPE1 block
//
#define EFI_ACPI_GPE1_BLK_LEN (EFI_ACPI_GPE1_BLK_BIT_WIDTH / 8)

#endif
