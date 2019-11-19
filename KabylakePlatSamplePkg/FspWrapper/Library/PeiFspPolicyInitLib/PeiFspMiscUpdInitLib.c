/** @file
  Implementation of Fsp Misc UPD Initialization.

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

#include <PeiFspPolicyInitLib.h>

#include <Library/MemoryAllocationLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#ifndef MINTREE_FLAG
#include <Library/SerialPortParameterLib.h>
#endif //MINTREE_FLAG

extern EFI_GUID gMemoryConfigVariableGuid;

/**
  Performs FSP Misc UPD initialization.

  @param[in][out]  FspmUpd             Pointer to FSPM_UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
**/
EFI_STATUS
EFIAPI
PeiFspMiscUpdInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  UINTN                             VariableSize;
  VOID                              *MemorySavedData;
#ifndef MINTREE_FLAG
  UINT32                            SerialPortBaudRate;
  UINT32                            SerialDebugLevel;
#endif //MINTREE_FLAG
  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  FspmUpd->FspmArchUpd.StackBase = (VOID *)(UINTN)(PcdGet32(PcdTemporaryRamBase) + PcdGet32(PcdTemporaryRamSize) - (PcdGet32(PcdFspTemporaryRamSize) + PcdGet32(PcdFspReservedBufferSize)));
  FspmUpd->FspmArchUpd.StackSize = PcdGet32(PcdFspTemporaryRamSize);

  Status = PeiServicesGetBootMode (&(FspmUpd->FspmArchUpd.BootMode));
  if (EFI_ERROR (Status)) {
    FspmUpd->FspmArchUpd.BootMode = BOOT_WITH_FULL_CONFIGURATION;
  }

  FspmUpd->FspmArchUpd.BootLoaderTolumSize = 0x0;

  //
  // Initialize S3 Data variable (S3DataPtr). It may be used for warm and fast boot paths.
  //
  VariableSize = 0;
  MemorySavedData = NULL;
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"MemoryConfig",
                               &gMemoryConfigVariableGuid,
                               NULL,
                               &VariableSize,
                               MemorySavedData
                               );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    MemorySavedData = AllocatePool (VariableSize);
    ASSERT (MemorySavedData != NULL);

    DEBUG ((DEBUG_INFO, "VariableSize is 0x%x\n", VariableSize));
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"MemoryConfig",
                                 &gMemoryConfigVariableGuid,
                                 NULL,
                                 &VariableSize,
                                 MemorySavedData
                                 );
    if (Status == EFI_SUCCESS) {
      FspmUpd->FspmArchUpd.NvsBufferPtr = MemorySavedData;
    } else {
      DEBUG ((DEBUG_ERROR, "Fail to retrieve Variable:\"MemoryConfig\" gMemoryConfigVariableGuid, Status = %r\n", Status));
      ASSERT_EFI_ERROR (Status);
    }
  }
  FspmUpd->FspmArchUpd.NvsBufferPtr = MemorySavedData;

#ifndef MINTREE_FLAG
  FspmUpd->FspmConfig.PcdDebugInterfaceFlags = GetDebugInterface();

  SerialPortBaudRate = GetSerialPortBaudRate ();
  switch (SerialPortBaudRate) {
  case 9600:
    FspmUpd->FspmConfig.PcdSerialDebugBaudRate = 3;
    break;
  case 19200:
    FspmUpd->FspmConfig.PcdSerialDebugBaudRate = 4;
    break;
  case 57600:
    FspmUpd->FspmConfig.PcdSerialDebugBaudRate = 6;
    break;
  case 115200:
    FspmUpd->FspmConfig.PcdSerialDebugBaudRate = 7;
    break;
  default:
    FspmUpd->FspmConfig.PcdSerialDebugBaudRate = 7;
  }

  SerialDebugLevel = GetDebugPrintErrorLevel ();
  switch (SerialDebugLevel) {
  case 0:
    FspmUpd->FspmConfig.PcdSerialDebugLevel = 0;
    break;
  case DEBUG_ERROR:
    FspmUpd->FspmConfig.PcdSerialDebugLevel = 1;
    break;
  case DEBUG_ERROR|DEBUG_WARN:
    FspmUpd->FspmConfig.PcdSerialDebugLevel = 2;
    break;
  case DEBUG_ERROR|DEBUG_WARN|DEBUG_INFO|DEBUG_EVENT|DEBUG_LOAD:
    FspmUpd->FspmConfig.PcdSerialDebugLevel = 4;
    break;
  default:  // DEBUG_ERROR|DEBUG_WARN|DEBUG_INFO|DEBUG_LOAD
    FspmUpd->FspmConfig.PcdSerialDebugLevel = 3;
  }
#endif //MINTREE_FLAG
  return EFI_SUCCESS;
}
