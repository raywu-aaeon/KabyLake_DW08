/** @file
  Acpi Smm driver.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2017 Intel Corporation.

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

#include "AcpiSmm.h"

GLOBAL_REMOVE_IF_UNREFERENCED UINT16                                    mAcpiBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA                       *mGlobalNvsAreaPtr  = 0;
GLOBAL_REMOVE_IF_UNREFERENCED CPU_GLOBAL_NVS_AREA                       *mCpuGlobalNvsAreaPtr = 0;
GLOBAL_REMOVE_IF_UNREFERENCED SYSTEM_AGENT_GLOBAL_NVS_AREA              *mSaGlobalNvsAreaPtr  = 0;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_EC_ACCESS_PROTOCOL                    *mEcAccess = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                     mPlatformFlavor;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                     mEcPresent;

//AMI_OVERRIDE_START - Move to AcpiModeEnable in AmiChipsetModulePkg SB
#if 0
/**
  Clear Port 80h

  SMI handler to enable ACPI mode

  Dispatched on reads from APM port with value EFI_ACPI_ENABLE_SW_SMI

  Disables the SW SMI Timer.
  ACPI events are disabled and ACPI event status is cleared.
  SCI mode is then enabled.

  Clear SLP SMI status
  Enable SLP SMI

  Disable SW SMI Timer

  Clear all ACPI event status and disable all ACPI events

  Disable PM sources except power button
  Clear status bits

  Disable GPE0 sources
  Clear status bits

  Disable GPE1 sources
  Clear status bits

  Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)

  Enable SCI

  @param[in] DispatchHandle       - The handle of this callback, obtained when registering
  @param[in] DispatchContext      - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
  @param[in] CommBuffer           - A pointer to a collection of data in memory that will
                                    be conveyed from a non-SMM environment into an SMM environment.
  @param[in] CommBufferSize       - The size of the CommBuffer.
**/
EFI_STATUS
EFIAPI
EnableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  OUT UINTN                     *CommBufferSize  OPTIONAL
  )
{
  UINT32                              OutputValue;
  UINT32                              SmiEn;
  UINT32                              SmiSts;
  UINT32                              ULKMC;
  UINT32                              Pm1Cnt;
  EFI_STATUS                          Status;
  UINTN                               LpcBaseAddress;

  Status = EFI_SUCCESS;

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  //
  // BIOS must also ensure that CF9GR is cleared and locked before handing control to the
  // OS in order to prevent the host from issuing global resets and resetting ME
  //
  // EDK2: To match PCCG current BIOS behavior, do not lock CF9 Global Reset
  // MmioWrite32 (
  //     PmcBaseAddress + R_PCH_PMC_ETR3),
  //     PmInit);

  //
  // Clear Port 80h
  //
  OutputValue = 0;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT16, 0x80, 1, &OutputValue);

  //
  // Disable SW SMI Timer and clean the status
  //
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_PCH_SMI_EN, 1, &SmiEn);
  SmiEn &= ~(B_PCH_SMI_EN_LEGACY_USB2 | B_PCH_SMI_EN_SWSMI_TMR | B_PCH_SMI_EN_LEGACY_USB);
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_PCH_SMI_EN, 1, &SmiEn);

  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_PCH_SMI_STS, 1, &SmiSts);
  SmiSts |= B_PCH_SMI_EN_LEGACY_USB2 | B_PCH_SMI_EN_SWSMI_TMR | B_PCH_SMI_EN_LEGACY_USB;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_PCH_SMI_STS, 1, &SmiSts);

  //
  // Disable port 60/64 SMI trap if they are enabled
  //
  ULKMC = MmioRead32 (LpcBaseAddress + R_PCH_LPC_ULKMC) & ~(B_PCH_LPC_ULKMC_60REN | B_PCH_LPC_ULKMC_60WEN | B_PCH_LPC_ULKMC_64REN | B_PCH_LPC_ULKMC_64WEN | B_PCH_LPC_ULKMC_A20PASSEN);
  MmioWrite32 (LpcBaseAddress + R_PCH_LPC_ULKMC, ULKMC);

  //
  // Disable PM sources except power button
  //
  OutputValue = B_PCH_ACPI_PM1_EN_PWRBTN;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT16, mAcpiBaseAddr + R_PCH_ACPI_PM1_EN, 1, &OutputValue);

  //
  // Clear PM status except Power Button status for RapidStart Resume
  //
  OutputValue = 0xFEFF;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT16, mAcpiBaseAddr + R_PCH_ACPI_PM1_STS, 1, &OutputValue);

  //
  // Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
  //
  OutputValue = R_PCH_RTC_REGD;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, R_PCH_RTC_INDEX_ALT, 1, &OutputValue);
  OutputValue = 0x0;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, R_PCH_RTC_TARGET_ALT, 1, &OutputValue);

  //
  // Enable SCI
  //
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, 1, &Pm1Cnt);
  Pm1Cnt |= B_PCH_ACPI_PM1_CNT_SCI_EN;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, 1, &Pm1Cnt);

  if (mEcPresent == 1) {
    //
    // Enable mobile specific stuff
    //
    mSaGlobalNvsAreaPtr->LidState = 1;

    //
    // Check the power state from EC and update it to global NVS area
    //
    if (PowerStateIsAc ()) {
      mGlobalNvsAreaPtr->PowerState = 1;
      mCpuGlobalNvsAreaPtr->PowerState = 1;
    } else {
      mGlobalNvsAreaPtr->PowerState = 0;
      mCpuGlobalNvsAreaPtr->PowerState = 0;
    }

    //
    // Disable SMI mode
    //
    Status = DisableEcSmiMode ();
    ASSERT_EFI_ERROR (Status);

    //
    // Commands EC to begin reading Thermal Diode and comparing to Critical Temperature.
    // Shutdown will occur when current temp equals or exceeds Critical temperature.
    //
    Status = EnableEcAcpiMode (TRUE);
    ASSERT_EFI_ERROR (Status);

    //
    // send command to EC to enable/disable ALS
    //
    if (mEcAccess != NULL) {
      mEcAccess->SMINotifyDisable ();
      mEcAccess->AcpiEnable ();
    }
  }

#ifdef EMBEDDED_FLAG
  //
  // Enable embedded specific stuff
  //
  if (mPlatformFlavor == FlavorEmbedded) {
    mSaGlobalNvsAreaPtr->LidState = 1;
    mGlobalNvsAreaPtr->PowerState = 0;
    mCpuGlobalNvsAreaPtr->PowerState = 0;
  }
#endif

  PlatformSpecificAcpiEnableHook (); // AdvancedFeaturesContent

  //
  // Write ALT_GPI_SMI_EN to disable GPI1 (SMC_EXTSMI#)
  //
  OutputValue = 0;
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + 0x38, 1, &OutputValue);
  OutputValue = OutputValue & ~(1 << (UINTN) PcdGet8 (PcdSmcExtSmiBitPosition));
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + 0x38, 1, &OutputValue);

  return Status;
}

/**
  Disable SCI

  @param[in] DispatchHandle       - The handle of this callback, obtained when registering
  @param[in] DispatchContext      - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
  @param[in] CommBuffer           - A pointer to a collection of data in memory that will
                                    be conveyed from a non-SMM environment into an SMM environment.
  @param[in] CommBufferSize       - The size of the CommBuffer.
**/
EFI_STATUS
EFIAPI
DisableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINT32      Pm1Cnt;

  //
  // Disable SCI
  //
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, 1, &Pm1Cnt);
  Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SCI_EN;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, 1, &Pm1Cnt);

  PlatformSpecificAcpiDisableHook (); // AdvancedFeaturesContent

  //
  // Take EC out of ACPI Mode.
  //
  Status = EnableEcAcpiMode (FALSE);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
#endif
//AMI_OVERRIDE_END - Move to AcpiModeEnable in AmiChipsetModulePkg SB

EFI_STATUS
EFIAPI
ECAccessCallback (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                        Status;

  Status = gSmst->SmmLocateProtocol (&gEfiEcAccessProtocolGuid, NULL, (VOID **) &mEcAccess);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
/**
  Initializes the Acpi Smm Driver

  @param[in] ImageHandle   - Pointer to the loaded image protocol for this driver
  @param[in] SystemTable   - Pointer to the EFI System Table

  @retval Status           - EFI_SUCCESS
  @retval Assert, otherwise.

**/
EFI_STATUS
EFIAPI
InitializeAcpiSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
//AMI_OVERRIDE_START - Move to AcpiModeEnable in AmiChipsetModulePkg SB
#if 0
  EFI_HANDLE                                SwHandle;
#endif
//AMI_OVERRIDE_END - Move to AcpiModeEnable in AmiChipsetModulePkg SB
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch;
//AMI_OVERRIDE_START - Move to AcpiModeEnable in AmiChipsetModulePkg SB
#if 0
  EFI_SMM_SW_REGISTER_CONTEXT               SwContext;
#endif
//AMI_OVERRIDE_END - Move to AcpiModeEnable in AmiChipsetModulePkg SB
  PLATFORM_INFO                             *PlatformInfo;
  EFI_PEI_HOB_POINTERS                      HobList;
  EFI_GLOBAL_NVS_AREA_PROTOCOL              *GlobalNvsAreaProtocol;
#ifndef MINTREE_FLAG
  CPU_GLOBAL_NVS_AREA_PROTOCOL              *CpuGlobalNvsAreaProtocol;
#endif
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL     *SaGlobalNvsAreaProtocol;
  VOID                                      *Registeration;

  PlatformInfo = NULL;

  //
  // Get the HOB list. If it is not present, then ASSERT.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Fail to get the HOB list.\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  HobList.Raw = GetNextGuidHob (&gPlatformInfoHobGuid, HobList.Raw);
  if (HobList.Raw == NULL) {
    return EFI_NOT_FOUND;
  } else {
    PlatformInfo = (PLATFORM_INFO *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
    mPlatformFlavor = PlatformInfo->PlatformFlavor;
    mEcPresent = PlatformInfo->EcPresent;
  }

  //
  // Get the ACPI Base Address
  //
  PchAcpiBaseGet (&mAcpiBaseAddr);

  //
  // Locate the ICH SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate our shared data area
  //
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;

#ifndef MINTREE_FLAG
  //
  // CPU Global NVS
  //
  Status = gBS->LocateProtocol (&gCpuGlobalNvsAreaProtocolGuid, NULL, (VOID **) &CpuGlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mCpuGlobalNvsAreaPtr = CpuGlobalNvsAreaProtocol->Area;
#endif
  //
  // SA Global NVS area
  //
  Status = gBS->LocateProtocol (&gSaGlobalNvsAreaProtocolGuid, NULL, (VOID **) &SaGlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mSaGlobalNvsAreaPtr = SaGlobalNvsAreaProtocol->Area;

  //
  // Locate gEfiEcAccessProtocolGuid protocol
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiEcAccessProtocolGuid,
                    ECAccessCallback,
                    &Registeration
                    );
  ASSERT_EFI_ERROR (Status);

//AMI_OVERRIDE_START - Move to AcpiModeEnable in AmiChipsetModulePkg SB  
#if 0  
  //
  // Register ACPI enable SMI handler
  //
  SwContext.SwSmiInputValue = (UINTN) PcdGet8 (PcdAcpiEnableSwSmi);
  Status = SwDispatch->Register (
                         SwDispatch,
                         EnableAcpiCallback,
                         &SwContext,
                         &SwHandle
                         );
  ASSERT_EFI_ERROR (Status);

  //
  // Register ACPI disable SMI handler
  //
  SwContext.SwSmiInputValue = (UINTN) PcdGet8 (PcdAcpiDisableSwSmi);
  Status = SwDispatch->Register (
                         SwDispatch,
                         DisableAcpiCallback,
                         &SwContext,
                         &SwHandle
                         );
  ASSERT_EFI_ERROR (Status);
#endif 
//AMI_OVERRIDE_END - Move to AcpiModeEnable in AmiChipsetModulePkg SB

  return EFI_SUCCESS;
}
