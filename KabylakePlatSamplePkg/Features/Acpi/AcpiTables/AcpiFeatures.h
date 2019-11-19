/** @file
  This is an implementation of the ACPI features driver.

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

#ifndef _ACPI_FEATURES_H_
#define _ACPI_FEATURES_H_

//
// Statements that include other header files
//
#include <Base.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PchSerialIoLib.h>
#include <Library/CpuPlatformLib.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/GlobalNvsArea.h>

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/WatchdogActionTable.h>
#include <IndustryStandard/DebugPortTable.h>
#include <IndustryStandard/DebugPort2Table.h>
#include <Lpit/Lpit.h>
#include <Library/PchCycleDecodingLib.h>
#include <Register/PchRegsPmc.h>
#include <Security/Wsmt/Wsmt.h>
//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_OEM_ID                       "INTEL "
#define ACPI_OEM_TABLE_ID_SKL                 0x4C4B53           // SKL
#define ACPI_OEM_TABLE_ID_SKL_ULT             0x544C552D4C4B53   // SKL-ULT
#define ACPI_OEM_TABLE_ID_SKL_SDS             0x5344532D4C4B53   // SKL-SDS
#define ACPI_OEM_TABLE_ID_KBL                 0x4C424B           // KBL
#define ACPI_OEM_TABLE_ID_KBL_ULT             0x544C552D4C424B   // KBL-ULT
#define ACPI_OEM_TABLE_ID_KBL_SDS             0x5344532D4C424B   // KBL-SDS
#define ACPI_OEM_TABLE_ID_UNKNOWN             0x304E574F4E4B4E55 // "UNKNOWN0"
#define EFI_ACPI_OEM_REVISION                 0x00000001
#define EFI_ACPI_CREATOR_ID                   0x5446534D         // "MSFT"
#define EFI_ACPI_CREATOR_REVISION             0x00000005F

//
// ACPI DBG2 - Debug Device Name Space String
//
#define ACPI_DBG2_DEFAULT_NAME_SPACE            "."
#define ACPI_DBG2_SERIALIO_UART0_NAME_SPACE     "\\_SB.PCI0.UA00"
#define ACPI_DBG2_SERIALIO_UART1_NAME_SPACE     "\\_SB.PCI0.UA01"
#define ACPI_DBG2_SERIALIO_UART2_NAME_SPACE     "\\_SB.PCI0.UA02"

//
// Function definitions
//
EFI_STATUS
LocateSupportProtocol (
  IN     EFI_GUID                      *Protocol,
  IN     EFI_GUID                      gEfiAcpiMultiTableStorageGuid,
  OUT    VOID                          **Instance,
  IN     BOOLEAN                       Type
  );

EFI_STATUS
IrmtDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

/**
  Entry point of the ACPI Features driver.

  @param[in] ImageHandle  A handle for the image that is initializing this driver
  @param[in] SystemTable  A pointer to the EFI system table

  @retval    EFI_SUCCESS  The driver installed without error.
  @retval    EFI_ABORTED  The driver encountered an error and could not complete
                          installation of the ACPI tables.
**/
EFI_STATUS
EFIAPI
InstallAcpiPlatformFeatures (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );
#endif
