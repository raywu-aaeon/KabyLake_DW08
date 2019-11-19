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

#include "PeiSkylakeHaloLpddr3Rvp16InitLib.h"
#include <../../Others/PeiBoardInitLib/PchHdaVerbTables.h>
#include <../../SkylakeHaloDdr4Rvp11/PeiBoardInitLib/SkylakeHaloDdr4Rvp11HdaVerbTables.h>

#include <Ppi/SiPolicy.h>


EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeHaloDdr4Rvp11Init (
  IN VOID *Content
  );

//
// RVP16 Init Table
//
BOARD_INIT_TABLE mRvp16BoardInit [] = {
  {BoardIdSkylakeHaloLpddr3Rvp16, PeiSkylakeHaloLpddr3Rvp16Init},
  { 0, 0 }
};

//
// RVP16 Post Memory config depends on RVP11.
//

EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeHaloLpddr3Rvp16Init (
  IN VOID *Content
  )
{
  EFI_STATUS Status;

  Status = PeiSkylakeHaloDdr4Rvp11Init (Content);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PcdSet32S (PcdHdaVerbTable, (UINTN) &HdaVerbTableAlc298);
  PcdSet32S (PcdExtHdaVerbTable, (UINTN) &HdaVerbTableAlc298Rvp11);

  return EFI_SUCCESS;
}

/**
  The constructor function for SkylakeA0Rvp3 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiSkylakeHaloLpddr3Rvp16BoardInitLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp16BoardInit, TABLE_LIST_NUM);
}
