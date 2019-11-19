/** @file
  PeiTbtInit library implementition with empty functions.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2018 Intel Corporation.

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

@par Specification
**/

#include <Library/DebugLib.h>
#include <Uefi/UefiBaseType.h>
#include <Library/PchPcieRpLib.h>
#include <Library/TbtCommonLib.h>

// AMI_OVERRIDE_START - EIP279338 : The resource for PEG Thunderbolt host is incorrect.
#define PCIE_NUM  (27)
#define NUMBER_OF_PCH_PCIE (PCIE_NUM - 3)
// AMI_OVERRIDE_END - EIP279338 : The resource for PEG Thunderbolt host is incorrect.

/**
  Get Pch/Peg Pcie Root Port Device and Function Number for TBT by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
**/
EFI_STATUS
EFIAPI
GetTbtRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFunc
  )
{
    EFI_STATUS            Status;
    UINTN                 TbtRpDev;
    UINTN                 TbtRpFunc;

// AMI_OVERRIDE_START - EIP279338 : The resource for PEG Thunderbolt host is incorrect.
  //
  // Total is 23 PCIE, out of the range
  //
  if (RpNumber >= PCIE_NUM) {
    return EFI_INVALID_PARAMETER;
  }

  if (RpNumber >= NUMBER_OF_PCH_PCIE)
  {
    //
    //  PEG Rootport      RpNumber = 20, 21, 22
    //
    *RpDev  =   0x01;
    *RpFunc =   RpNumber - NUMBER_OF_PCH_PCIE;
  } else {
    //
    //  PCH Rootport      RpNumber = 0 ~ 19
    //
    Status  = GetPchPcieRpDevFun (RpNumber, &TbtRpDev, &TbtRpFunc);
    *RpDev  = TbtRpDev;
    *RpFunc = TbtRpFunc;
  }
// AMI_OVERRIDE_END - EIP279338 : The resource for PEG Thunderbolt host is incorrect.

  return EFI_SUCCESS;
}

/**
  Check connected TBT controller is supported or not by DeviceID

  @param[in]  DeviceID              DeviceID of of TBT controller


  @retval     TRUE                  Valid DeviceID
  @retval     FALSE                 Invalid DeviceID
**/
BOOLEAN
IsTbtHostRouter(
    IN    UINT16  DeviceID
)
{

  switch (DeviceID) {
    case LR_HR:
    case ER_SFF_HR:
    case ER_HR:
    case CR_HR_4C:
    case CR_HR_2C:
    case RR_HR_2C:
    case RR_HR_4C:
    case FR_HR_2C:
    case FR_HR_4C:
    case WR_HR_2C:
    case AR_HR_2C:
    case AR_HR_4C:
    case AR_HR_LP:
    case AR_HR_C0_2C:
    case AR_HR_C0_4C:
    case TR_HR_2C:
    case TR_HR_4C:
        return TRUE;
  }

  return FALSE;
} // IsTbtHostRouter