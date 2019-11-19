/** @file

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

#include "PeiSkylakeHaloDdr4Rvp11InitLib.h"

#include "GpioTableHaloRvp11.h"
#include "SkylakeHaloDdr4Rvp11HdaVerbTables.h"

#include <Ppi/SiPolicy.h>

//extern GPIO_INIT_CONFIG mGpioTableHaloRvp11[];
//extern UINT16 mGpioTableHaloRvp11Size;
//extern GPIO_INIT_CONFIG mGpioTableHaloRvp11Touchpanel;

//
// RVP11 Init Table
//
BOARD_INIT_TABLE mRvp11BoardInit[] = {
  {BoardIdSkylakeHaloDdr4Rvp11, PeiSkylakeHaloDdr4Rvp11Init},
  {BoardIdSkylakeHaloDdr4Rvp11Ppv, PeiSkylakeHaloDdr4Rvp11PpvInit},
  { 0, 0 }
};

/**
  Board Specific Init function hook for PEI post memory phase.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
PeiSkylakeHaloDdr4Rvp11SpecificInitPostMem (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  SkylaeHaloDdr4Rvp11 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeHaloDdr4Rvp11Init (
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

  if (BoardIdOrgValue == BoardIdHaloDdr4uSFF) {
    PcdSet32S(PcdBoardGpioTable, (UINTN)mGpioTableHaloRvp11uSff);
    PcdSet16S(PcdBoardGpioTableSize, mGpioTableHaloRvp11uSffSize);
  } else {
    PcdSet32S(PcdBoardGpioTable, (UINTN)mGpioTableHaloRvp11);
    PcdSet16S(PcdBoardGpioTableSize, mGpioTableHaloRvp11Size);
  }
  PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableHaloRvp11Touchpanel);

  PcdSet32S (PcdHdaVerbTable, (UINTN) &HdaVerbTableAlc298Rvp11);

  //
  // TBT
  PcdSet32S   (PcdTbtSupportedPortsInfo                    , 0x100100);
  PcdSet8S    (PcdTbtDefaultSelectorInfo                   , 9);

  PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType             , 1);
  PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel                  , 1);

  PcdSet8S    (PcdTbtCioPlugEventGpioGpioAccessType         , 1);
  if (BoardIdOrgValue == BoardIdHaloDdr4uSFF) {
    PcdSet32S(PcdTbtCioPlugEventGpioGpioNumber, GPIO_SKL_H_GPP_D9);
    PcdSet32S(PcdTbtForcePwrGpioGpioNumber, GPIO_SKL_H_GPP_D11);
  } else {
    PcdSet32S(PcdTbtCioPlugEventGpioGpioNumber, GPIO_SKL_H_GPP_G12);
    PcdSet32S(PcdTbtForcePwrGpioGpioNumber, GPIO_SKL_H_GPP_G16);
  }
  PcdSetBoolS (PcdTbtCioPlugEventGpioGpioMode               , 0);

  //PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature        , SIGNATURE_32 ('_', 'E', '0', 'C'));
  PcdSetBoolS (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting , 1);
  PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber       , 0x0c);
  PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature        , SIGNATURE_32 ('_', 'E', '0', 'C'));
  PcdSet16S   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress       , (UINT16) (EFI_ACPI_GPE0_BLK_ADDRESS + (0x0c / 0x08)));
  PcdSet8S    (PcdTbtCioPlugEventGpioAcpiGpeStsBit           , (0x0c % 0x08));

  PcdSet32S (PcdFuncPeiBoardSpecificInitPostMem, (UINTN) PeiSkylakeHaloDdr4Rvp11SpecificInitPostMem);

  return EFI_SUCCESS;
}

/**
  SkylaeHaloDdr4Rvp11 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeHaloDdr4Rvp11PpvInit (
  IN VOID *Content
  )
{
  EFI_STATUS Status;

  Status = PeiSkylakeHaloDdr4Rvp11Init (Content);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  The constructor function for SkylakeHaloDdr4Rvp11 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiSkylakeHaloDdr4Rvp11BoardInitLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp11BoardInit, TABLE_LIST_NUM);
}

