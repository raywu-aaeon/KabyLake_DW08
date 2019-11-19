/** @file
  TBT Security Lib.

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

#include <Library/DxeTbtSecurityLib.h>
#include <Library/TpmMeasurementLib.h>
#include <PciExpressHelperLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <TbtBoardInfo.h>
#include <Protocol/DisableBmeProtocol.h>
#include <Library/HobLib.h>

DISABLE_TBT_BME_PROTOCOL mDisableBmeProtocol = {
  TbtSecurityExitBootCallBackFunction,
};

/**
  TBTSecurityReadyToBootCallBackFunction

  For Windows 10 RS4, a new security feature is added to protect against Physical DMA attacks over Thunderbolt connects.
  In order to do this, they need a new flag added to the DMAR tables that a DMA is only permitted into RMRR at ExitBootServices().  With this flag available, OS can then Bug Check if any DMA is requested outside of the RMRR before OS supported device drivers are started.
  ReadyToBoot callback routine to update DMAR BIT2
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
  )
{
  EFI_ACPI_TABLE_PROTOCOL         *AcpiTable;
  EFI_STATUS                      Status;
  UINTN                           Handle;
  EFI_ACPI_DMAR_TABLE             *DmarTable;
  EFI_ACPI_DESCRIPTION_HEADER     *VtdAcpiTable;

  DEBUG ((DEBUG_INFO, "[TBT] TbtSecurityReadyToLockCallBackFunction START\n"));

  Handle        = 0;
  DmarTable     = NULL;
  VtdAcpiTable  = NULL;
  AcpiTable     = NULL;

  //
  // Locate ACPI Table Protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to locate Acpi Protocol\n"));
    goto Exit;
  }

  //
  // Initialize ASL manipulation library
  //
  Status = InitializeAslUpdateLib ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Initialize ASL manipulation library\n"));
    return;
  }

  //
  // Locate the DMAR Table
  //
  Status = LocateAcpiTableBySignature (
             EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE,
             &VtdAcpiTable,
             &Handle
             );
  if (EFI_ERROR (Status) || (VtdAcpiTable == NULL)) {
    DEBUG ((DEBUG_ERROR, "Error updating the DMAR ACPI table\n"));
    goto Exit;
  }

  DEBUG((DEBUG_INFO, "DMAR ACPI Table: Address = 0x%x\n", VtdAcpiTable));

  //
  // Update the DMAR table structure
  //
  DmarTable   = (EFI_ACPI_DMAR_TABLE *) VtdAcpiTable;
  DmarTable->Flags |= BIT2;

  //
  // Update the DMAR ACPI table
  //
  if (Handle != 0) {
    Status = AcpiTable->UninstallAcpiTable (
                          AcpiTable,
                          Handle
                          );
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to uninstall DMAR ACPI table\n"));
      goto Exit;
    }
  }

  //
  // Update the Acpi Vtd table
  //
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        VtdAcpiTable,
                        VtdAcpiTable->Length,
                        &Handle
                        );
  if (!EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "DMAR ACPI table was successfully updated\n"));
  } else {
    DEBUG ((DEBUG_ERROR, "Error updating the DMAR ACPI table\n"));
  }

Exit:
  DEBUG ((DEBUG_INFO, "[TBT] TbtSecurityReadyToLockCallBackFunction END\n"));
  gBS->CloseEvent (Event);
}

/**
  Disable PCI device Bus Master bit
  @param[in]  Sbdf       device's segment:bus:device:function coordinates
**/
VOID
DisableBME (
  IN SBDF       Sbdf
  )
{
  UINT16  Command;
  UINT64  DeviceCmdRegAddress;

  DeviceCmdRegAddress = PCI_SEGMENT_LIB_ADDRESS (Sbdf.Seg, Sbdf.Bus, Sbdf.Dev, Sbdf.Func, PCI_COMMAND_OFFSET);

  Command = PciSegmentRead16 (DeviceCmdRegAddress);
  DEBUG ((DEBUG_INFO, "Before disable BME (0x%x:0x%x:0x%x:0x%x): Command: 0x%x \n", Sbdf.Seg, Sbdf.Bus, Sbdf.Dev, Sbdf.Func, Command));

  if ((Command & EFI_PCI_COMMAND_BUS_MASTER) == EFI_PCI_COMMAND_BUS_MASTER) {
    PciSegmentAnd16 (DeviceCmdRegAddress, (UINT16)~EFI_PCI_COMMAND_BUS_MASTER);
  }

  DEBUG ((DEBUG_INFO, "After  BME (0x%x:0x%x:0x%x:0x%x): Command: 0x%x \n", Sbdf.Seg, Sbdf.Bus, Sbdf.Dev, Sbdf.Func, PciSegmentRead16 (DeviceCmdRegAddress)));
}

/**
  Recursive Tbt Hierarchy Configuration

  @param[in]  Sbdf       device's segment:bus:device:function coordinates
**/
VOID
RecursiveTbtHierarchyConfiguration (
  IN SBDF       Sbdf
  )
{
  SBDF          ChildSbdf = {0,0,0,0,0};
  PCI_DEV_TYPE  DevType;
  UINT32        Data32;

  DEBUG ((DEBUG_INFO, "RecursiveTbtHierarchyConfiguration %02x:%02x:%02x:%02x START\n", Sbdf.Seg, Sbdf.Bus, Sbdf.Dev, Sbdf.Func));

  DevType   = DevTypeMax;
  Data32    = PciSegmentRead32 (SbdfToBase (Sbdf)) + 1;

  if ((Data32 == 0) || (Data32 == 0xFF)) {
    DEBUG ((DEBUG_INFO, "Controller doesn't exist or it's not supported, value: %d\n", Data32));
    return;
  }

  DevType = GetDeviceType (Sbdf);
  DEBUG ((DEBUG_INFO, "DevType: %d\n", DevType));

  if (DevType == DevTypePcieEndpoint) {
    DisableBME (Sbdf);
  }

  if (HasChildBus (Sbdf, &ChildSbdf) == TRUE) {
    while (FindNextPcieChild (DevType, &ChildSbdf)) {
      RecursiveTbtHierarchyConfiguration (ChildSbdf);
    }
  }

  if ((DevType != DevTypePcieEndpoint)) {
    DisableBME (Sbdf);
  }

  DEBUG ((DEBUG_INFO, "RecursiveTbtHierarchyConfiguration %02x:%02x:%02x:%02x END_1\n", Sbdf.Seg, Sbdf.Bus, Sbdf.Dev, Sbdf.Func));
}


/**
  The function install DisableBme protocol for TBT Shell validation
**/
VOID
InstallDisableBmeProtocol (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_HANDLE        Handle;

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gDxeDisableTbtBmeProtocolGuid,
                  &mDisableBmeProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "InstallDisableBmeProtocol Status: %d\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "InstallDisableBmeProtocol Installed\n"));
  }
}

/**
  TbtSecurityExitBootCallBackFunction

  For Windows RS4, BIOS need to disable BME and tear down the Thunderbolt DMAR tables at ExitBootServices,
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
  )
{
  EFI_STATUS      Status;
  SBDF            Sbdf = {0,0,0,0,0};
  UINTN           RpDev;
  UINTN           RpFunc;
  TBT_INFO_HOB    *TbtInfoHob;

  DEBUG((DEBUG_INFO, "[TBT] TbtSecurityExitBootCallBackFunction START\n"));

  Status      = EFI_SUCCESS;
  RpDev       = 0;
  RpFunc      = 0;

  TbtInfoHob = (TBT_INFO_HOB *) GetFirstGuidHob (&gTbtInfoHobGuid);
  if (TbtInfoHob == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to get TbtInfoHob\n"));
    goto Exit;
  }

  Status = GetTbtRpDevFun (TbtInfoHob->TbtDefaultSelectorInfo - 1, &RpDev, &RpFunc);
  if (EFI_ERROR (Status)) {
     DEBUG ((DEBUG_ERROR, "Failed to get GetDTbtRpDevFun, Status: %d", Status));
  } else {
    Sbdf.Dev = (UINT32)RpDev;
    Sbdf.Func = (UINT32)RpFunc;
    RecursiveTbtHierarchyConfiguration (Sbdf);
  }

// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  Status = GetTbtRpDevFun (TbtInfoHob->TbtDefaultSelectorInfo1 - 1, &RpDev, &RpFunc);
  if (EFI_ERROR (Status)) {
     DEBUG ((DEBUG_ERROR, "Failed to get GetTbtRpDevFun, Status: %d", Status));
  } else {
    Sbdf.Dev = (UINT32)RpDev;
    Sbdf.Func = (UINT32)RpFunc;
    RecursiveTbtHierarchyConfiguration (Sbdf);
  }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.

Exit:
  DEBUG((DEBUG_INFO, "[TBT] TbtSecurityExitBootCallBackFunction END\n"));
  gBS->CloseEvent (Event);
}

/**
  TBT Security EndOfDxe CallBack Function
  If the firmware/BIOS has an option to enable and disable DMA protections via a VT-d switch in BIOS options, then the shipping configuration must be with VT-d protection enabled.
  On every boot where VT-d/DMA protection is disabled, or will be disabled, or configured to a lower security state, and a platform has a TPM enabled, then the platform SHALL
  extend an EV_EFI_ACTION event into PCR[7] before enabling external DMA
  The event string SHALL be "DMA Protection Disabled". The platform firmware MUST log this measurement in the event log using the string "DMA Protection Disabled" for the Event Data.
  Measure and log launch of TBT Security, and extend the measurement result into a specific PCR.
  Extend an EV_EFI_ACTION event into PCR[7] before enabling external DMA. The event string SHALL be "DMA Protection Disabled". The platform firmware MUST log this measurement
  in the event log using the string "DMA Protection Disabled" for the Event Data.

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
TbtSecurityEndOfDxeCallBackFunction (
  IN EFI_EVENT          Event,
  IN VOID               *Context
  )
{
  UINTN                 Status;
  UINT64                HashDataLen;

  DEBUG ((DEBUG_INFO, "[TBT] TbtSecurityEndOfDxeCallBackFunction START\n"));

  //
  // When VT-d/DMA protection is disabled and a platform has a TPM enabled,
  // the platform SHALL extend an EV_EFI_ACTION event into PCR[7]
  //
  HashDataLen = TBT_SECURITY_EVENT_STRING_LEN;

  Status = TpmMeasureAndLogData (
             7,
             EV_EFI_ACTION,
             TBT_SECURITY_EVENT_STRING,
             (UINT32) HashDataLen,
             TBT_SECURITY_EVENT_STRING,
             HashDataLen
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TpmMeasureAndLogData Status: %d\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "TpmMeasureAndLogData Successfully\n"));
  }

  DEBUG ((DEBUG_INFO, "[TBT] TbtSecurityEndOfDxeCallBackFunction END\n"));
  gBS->CloseEvent (Event);
}

