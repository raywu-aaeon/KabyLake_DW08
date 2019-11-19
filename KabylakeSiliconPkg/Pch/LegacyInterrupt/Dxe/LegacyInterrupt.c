/** @file
  This code supports a the private implementation
  of the Legacy Interrupt protocol

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#include "LegacyInterrupt.h"

//
// Handle for the Legacy Interrupt Protocol instance produced by this driver
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_HANDLE                    mLegacyInterruptHandle = NULL;

//
// The Legacy Interrupt Protocol instance produced by this driver
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_LEGACY_INTERRUPT_PROTOCOL mLegacyInterrupt = {
  GetNumberPirqs,
  GetLocation,
  ReadPirq,
  WritePirq
};

//
// Module Global:
//  Since this driver will only ever produce one instance of the Private Data
//  protocol you are not required to dynamically allocate the PrivateData.
//

/**
  Return the number of PIRQs supported by this chipset.

  @param[in] This                 Pointer to LegacyInterrupt Protocol
  @param[out] NumberPirqs         The pointer which point to the max IRQ number supported by this PCH.

  @retval EFI_SUCCESS             Legacy BIOS protocol installed
**/
EFI_STATUS
EFIAPI
GetNumberPirqs (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  OUT UINT8                          *NumberPirqs
  )
{
  *NumberPirqs = MAX_PIRQ_NUMBER;

  return EFI_SUCCESS;
}

/**
  Return PCI location of this device. $PIR table requires this info.

  @param[in] This                 Protocol instance pointer.
  @param[out] Bus                 PCI Bus
  @param[out] Device              PCI Device
  @param[out] Function            PCI Function

  @retval EFI_SUCCESS             Bus/Device/Function returned
**/
EFI_STATUS
EFIAPI
GetLocation (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  OUT UINT8                          *Bus,
  OUT UINT8                          *Device,
  OUT UINT8                          *Function
  )
{
  *Bus      = DEFAULT_PCI_BUS_NUMBER_PCH;
  *Device   = PCI_DEVICE_NUMBER_PCH_LPC;
  *Function = PCI_FUNCTION_NUMBER_PCH_LPC;

  return EFI_SUCCESS;
}

/**
  Read the given PIRQ register

  @param[in] This                 Pointer to LegacyInterrupt Protocol
  @param[in] PirqNumber           The Pirq register 0 = A, 1 = B etc
  @param[out] PirqData            Value read

  @retval EFI_SUCCESS             Decoding change affected.
  @retval EFI_INVALID_PARAMETER   Invalid PIRQ number
**/
EFI_STATUS
EFIAPI
ReadPirq (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  IN  UINT8                          PirqNumber,
  OUT UINT8                          *PirqData
  )
{

  if (PirqNumber >= MAX_PIRQ_NUMBER) {
    return EFI_INVALID_PARAMETER;
  }

  PchPcrRead8 (PID_ITSS, R_PCH_PCR_ITSS_PIRQA_ROUT + PirqNumber, PirqData);
  *PirqData = (UINT8) (*PirqData & 0x7f);

  return EFI_SUCCESS;
}

/**
  Read the given PIRQ register

  @param[in] This                 Pointer to LegacyInterrupt Protocol
  @param[in] PirqNumber           The Pirq register 0 = A, 1 = B etc
  @param[in] PirqData             Value read

  @retval EFI_SUCCESS             Decoding change affected.
  @retval EFI_INVALID_PARAMETER   Invalid PIRQ number
**/
EFI_STATUS
EFIAPI
WritePirq (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  IN  UINT8                          PirqNumber,
  IN  UINT8                          PirqData
  )
{
  if (PirqNumber >= MAX_PIRQ_NUMBER) {
    return EFI_INVALID_PARAMETER;
  }
  PchPcrWrite8 (PID_ITSS, R_PCH_PCR_ITSS_PIRQA_ROUT + PirqNumber, PirqData);
  return EFI_SUCCESS;
}

/**
  <b>LegacyInterrupt DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
    The LegacyInterrupt module is a DXE driver which provides a standard way for
    other drivers to get/set the PIRQ/legacy IRQ mappings.

  - @pre
    PCH PCR base address configured

  - @result
    The LegacyInterrupt driver produces EFI_LEGACY_INTERRUPT_PROTOCOL which is
    documented in the Compatibility Support Module Specification.

  @param[in] ImageHandle          Handle for this drivers loaded image protocol.
  @param[in] SystemTable          EFI system table.

  @retval EFI_SUCCESS             Legacy Interrupt protocol installed
  @retval Other                   No protocol installed, unload driver.
**/
EFI_STATUS
EFIAPI
LegacyInterruptInstall (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "LegacyInterruptInstall() Start\n"));

  //
  // Make sure the Legacy Interrupt Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiLegacyInterruptProtocolGuid);

  //
  // Make a new handle and install the protocol
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mLegacyInterruptHandle,
                  &gEfiLegacyInterruptProtocolGuid,
                  &mLegacyInterrupt,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "LegacyInterruptInstall() End\n"));
  return Status;
}
