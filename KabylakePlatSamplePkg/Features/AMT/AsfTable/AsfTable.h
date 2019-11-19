/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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

#ifndef _ASF_TABLE_H_
#define _ASF_TABLE_H_

#include <IndustryStandard/AlertStandardFormatTable.h>
#include <Protocol/AcpiTable.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/AlertStandardFormat.h>

#define ASF_RMCP_BOOT_OPTION_SUCCESS       0x00
#define ASF_RMCP_NO_BOOT_OPTION            0x01

#ifdef EMBEDDED_FLAG
// For Saddle brook platform
#define ASF_ALRT_NUMBER_OF_ALERTS          8
#define ASF_RCTL_NUMBER_OF_CONTROLS        4
#define ASF_ADDR_NUMBER_OF_DEVICES         6
#else
#define ASF_ALRT_NUMBER_OF_ALERTS          3
#define ASF_RCTL_NUMBER_OF_CONTROLS        4
#define ASF_ADDR_NUMBER_OF_DEVICES         11
#endif

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  EFI_ACPI_ASF_INFO           AsfInfo;
  EFI_ACPI_ASF_ALRT           AsfAlert;
  EFI_ACPI_ASF_ALERTDATA      AsfAlertData[ASF_ALRT_NUMBER_OF_ALERTS];
  EFI_ACPI_ASF_RCTL           AsfRctl;
  EFI_ACPI_ASF_CONTROLDATA    AsfControlData[ASF_RCTL_NUMBER_OF_CONTROLS];
  EFI_ACPI_ASF_RMCP           AsfRmcp;
  EFI_ACPI_ASF_ADDR           AsfAddr;
  UINT8                       FixedSmbusAddresses[ASF_ADDR_NUMBER_OF_DEVICES];
} ACPI_2_0_ASF_DESCRIPTION_TABLE_WITH_DATA;

/**
  Building Alert Standard Format (ASF) Table.

  @param[in] Event         - A pointer to the Event that triggered the callback.
  @param[in] Context       - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
BuildAsfTable (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

#endif
