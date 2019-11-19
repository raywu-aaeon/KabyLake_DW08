/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 - 2018 Intel Corporation.

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

#include "PeiKabylakeDdr4Rvp17InitLib.h"

#include "GpioTableKblRvp17.h"
#include "KabylakeDdr4Rvp17HdaVerbTables.h"

#include <Ppi/SiPolicy.h>
#include <SetupVariable.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h> // AdvancedFeaturesContent
//
// Kabylake RVP17 Init Table
//
BOARD_INIT_TABLE mKblRvp17BoardInit[] = {
  {BoardIdKabylakeDdr4Rvp17, PeiKabylakeDdr4Rvp17Init},
  { 0, 0 }
};

/**
  Board Specific Init function hook for PEI post memory phase.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
PeiKabylakeDdr4Rvp17SpecificInitPostMem (
  VOID
  )
{
  GPIO_CONFIG         GpioConfig;
  SA_SETUP            SaSetup;
  UINTN               VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  EFI_STATUS          Status;

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi(
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices       // PPI
             );
  ASSERT_EFI_ERROR(Status);

  VariableSize = sizeof (SA_SETUP);
  Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"SaSetup",
                                 &gSaSetupVariableGuid,
                                 NULL,
                                 &VariableSize,
                                 &SaSetup
                                 );
  ASSERT_EFI_ERROR (Status);
  //
  // Compliance Mode Testing On/Off
  //
  if (SaSetup.PegComplianceModeTest) {
    GpioGetPadConfig (GPIO_SKL_H_GPP_H2, &GpioConfig);
    GpioConfig.PadMode = GpioPadModeGpio;
    GpioSetPadConfig (GPIO_SKL_H_GPP_H2, &GpioConfig);
  }

  return EFI_SUCCESS;
}

/**
  KabylakeDdr4Rvp17 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiKabylakeDdr4Rvp17Init (
  IN VOID *Content
  )
{
  PLATFORM_INFO *PlatformInfo;
  EFI_STATUS Status;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;

  Status = GetBoardId(&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR(Status);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformInfo = (PLATFORM_INFO *) Content;

  PcdSetBoolS (PcdM2GpioEnable, TRUE);

  PcdSet32S(PcdBoardGpioTable, (UINTN)mGpioTableKblRvp17);
  PcdSet16S(PcdBoardGpioTableSize, mGpioTableKblRvp17Size);
  PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableKblRvp17Touchpanel);

  PcdSet32S (PcdHdaVerbTable, (UINTN) &HdaVerbTableAlc298Rvp17);

  //
  // TBT
  PcdSet32S   (PcdTbtSupportedPortsInfo                    , 0x100100);
  PcdSet8S    (PcdTbtDefaultSelectorInfo                   , 9);

  PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType             , 1);
  PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel                  , 1);

  PcdSet8S    (PcdTbtCioPlugEventGpioGpioAccessType         , 1);
  PcdSet32S(PcdTbtCioPlugEventGpioGpioNumber, GPIO_SKL_H_GPP_G12);
  PcdSet32S(PcdTbtForcePwrGpioGpioNumber, GPIO_SKL_H_GPP_G16);
  PcdSetBoolS (PcdTbtCioPlugEventGpioGpioMode               , 0);

  //PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature        , SIGNATURE_32 ('_', 'E', '0', 'C'));
  PcdSetBoolS (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting , 1);
  PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber       , 0x0c);
  PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature        , SIGNATURE_32 ('_', 'E', '0', 'C'));
  PcdSet16S   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress       , (UINT16) (EFI_ACPI_GPE0_BLK_ADDRESS + (0x0c / 0x08)));
  PcdSet8S    (PcdTbtCioPlugEventGpioAcpiGpeStsBit           , (0x0c % 0x08));

  PcdSet32S (PcdFuncPeiBoardSpecificInitPostMem, (UINTN) PeiKabylakeDdr4Rvp17SpecificInitPostMem);

  return EFI_SUCCESS;
}

/**
  The constructor function for KabylakeDdr4Rvp17 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiKabylakeDdr4Rvp17BoardInitLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mKblRvp17BoardInit, TABLE_LIST_NUM);
}

