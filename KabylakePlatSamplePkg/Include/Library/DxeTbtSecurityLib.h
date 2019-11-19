/**@file

@copyright
  INTEL CONFIDENTIAL
  Copyright 2018 Intel Corporation.

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
#ifndef _TBT_SECURITY_LIB_H_
#define _TBT_SECURITY_LIB_H_

#include <Protocol/Tcg2Protocol.h>
#include <Protocol/AcpiTable.h>
#include <IndustryStandard/Pci.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/AslUpdateLib.h>
#include <Library/UefiLib.h>
#include <Uefi.h>
#include <SetupVariable.h>
#include <OemSetup.h>
#include <DmaRemappingTable.h>
#include <PcieRegs.h>
//#include <Tcg2ConfigNvData.h> // AMI_OVERRIDE - AMI not using this
#include <Library/TbtCommonLib.h>

#define TBT_SECURITY_EVENT_STRING      "DMA Protection Disabled"
#define TBT_SECURITY_EVENT_STRING_LEN  (sizeof(TBT_SECURITY_EVENT_STRING) - 1)

/**
  TBT Security EndOfDxe CallBack Function
  If the firmware/BIOS has an option to enable and disable DMA protections via a VT-d switch in BIOS options, then the shipping configuration must be with VT-d protection enabled.
  On every boot where VT-d/DMA protection is disabled, or will be disabled, or configured to a lower security state, and a platform has a TPM enabled, then the platform SHALL extend an EV_EFI_ACTION event into PCR[7] before enabling external DMA.
  The event string SHALL be "DMA Protection Disabled". The platform firmware MUST log this measurement in the event log using the string "DMA Protection Disabled" for the Event Data.
  Measure and log launch of TBT Security, and extend the measurement result into a specific PCR.
  Extend an EV_EFI_ACTION event into PCR[7] before enabling external DMA. The event string SHALL be "DMA Protection Disabled". The platform firmware MUST log this measurement in the event log using the string "DMA Protection Disabled" for the Event Data.

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
TbtSecurityEndOfDxeCallBackFunction (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  TBT Security ExitBootCallBack Function

  For Windows RS4, BIOS will disable BME and tear down the Thunderbolt DMAR tables at ExitBootServices
  in order to hand off security of TBT hierarchies to the OS.
  The BIOS is expected to either: Disable BME from power on till the OS starts configuring the devices and enabling BME Enable BME only for devices that can be protected by VT-d in preboot environment,
  but disable BME and tear down any Thunderbolt DMAR tables at ExitBootServices()

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
TbtSecurityExitBootCallBackFunction (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  TBT Security ReadyToLockCallBack Function

  For Windows 10 RS4, a new security feature will be supported to protect against Physical DMA attacks over Thunderbolt connects.
  In order to do this, they need a new flag added to the DMAR tables that a DMA is only permitted into RMRR at ExitBootServices().  With this flag available, OS can then Bug Check if any DMA is requested outside of the RMRR before OS supported device drivers are started.
  ReadyToLock callback routine to update DMAR BIT2
  Bit definition: DMA_CONTROL_GUARANTEE
  If Set, the platform supports blocking all DMA outside of the regions defined in the RMRR structures from ExitBootServices() until OS supported device drivers are started.

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
TbtSecurityReadyToLockCallBackFunction (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  The function install DisableBme protocol for TBT Shell validation
**/
VOID
InstallDisableBmeProtocol (
  VOID
  );

#endif
