/** @file
  Platform recovery library implementation.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2017 Intel Corporation.

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

#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Ppi/RecoveryModule.h>
#include <Ppi/DeviceRecoveryModule.h>

EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI *This
  );

static EFI_PEI_RECOVERY_MODULE_PPI mRecoveryPpi = {
  PlatformRecoveryModule
};

static EFI_PEI_PPI_DESCRIPTOR mRecoveryPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRecoveryModulePpiGuid,
  &mRecoveryPpi
};


/**
  Provide the functionality of the Ea Recovery Module.

  @param[in]  PeiServices  General purpose services available to every PEIM.

  @retval     Status       EFI_SUCCESS if the interface could be successfully
                           installed
**/
EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES               **PeiServices,
  EFI_PEI_RECOVERY_MODULE_PPI       *This
  )
{
  EFI_STATUS                             Status;
  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI     *DeviceRecoveryModule;
  UINTN                                  NumberOfImageProviders;
  BOOLEAN                                ProviderAvailable;
  UINTN                                  NumberRecoveryCapsules;
  UINTN                                  RecoveryCapsuleSize;
  EFI_GUID                               DeviceId;
  BOOLEAN                                ImageFound;
  EFI_PHYSICAL_ADDRESS                   Address;
  VOID                                   *Buffer;
  EFI_PEI_HOB_POINTERS                   Hob;
  EFI_PEI_HOB_POINTERS                   HobOld;
  BOOLEAN                                HobUpdate;
  EFI_BOOT_MODE                          BootMode;

  PeiServicesGetBootMode (&BootMode);
  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }
  Status = EFI_SUCCESS;
  HobUpdate = FALSE;

  ProviderAvailable = TRUE;
  ImageFound        = FALSE;
  NumberOfImageProviders = 0;

  DeviceRecoveryModule = NULL;

  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Recovery Entry\n"));

  //
  // Search the platform for some recovery capsule if the DXE IPL
  // discovered a recovery condition and has requested a load.
  //
  while (ProviderAvailable) {
    Status = PeiServicesLocatePpi (
               &gEfiPeiDeviceRecoveryModulePpiGuid,
               NumberOfImageProviders,
               NULL,
               (VOID **) &DeviceRecoveryModule
               );
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Device Recovery PPI located\n"));
      NumberOfImageProviders++;
      Status = DeviceRecoveryModule->GetNumberRecoveryCapsules (
                                                      PeiServices,
                                                      DeviceRecoveryModule,
                                                      &NumberRecoveryCapsules
                                                      );

      DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Number Of Recovery Capsules: %d\n", NumberRecoveryCapsules));

      if (NumberRecoveryCapsules != 0) {
        break;
      }
    } else {
      ProviderAvailable = FALSE;
    }
  }

  //
  // If there is an image provider, get the capsule ID
  //
  if (ProviderAvailable) {
    RecoveryCapsuleSize = 0;

    Status = DeviceRecoveryModule->GetRecoveryCapsuleInfo (
                                     PeiServices,
                                     DeviceRecoveryModule,
                                     0,
                                     &RecoveryCapsuleSize,
                                     &DeviceId
                                     );

    if (EFI_ERROR(Status)) {
      return Status;
    }

    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Recovery Capsule Size: %d\n", RecoveryCapsuleSize));

    //
    // Only support the 2 capsule types known
    // Future enhancement is to rank-order the selection
    //
    //
    // Bugbug here
    //
    /*
    if ((!CompareGuid (&DeviceId, &gPeiCapsuleOnFatIdeDiskGuid)) &&
        (!CompareGuid (&DeviceId, &gPeiCapsuleOnFatFloppyDiskGuid)) &&
        (!CompareGuid (&DeviceId, &gPeiCapsuleOnDataCDGuid)) &&
        (!CompareGuid (&DeviceId, &gPeiCapsuleOnFatUsbDiskGuid))
        ) {
      return EFI_UNSUPPORTED;
    }
    */
    Buffer = NULL;
    Status = PeiServicesAllocatePages (
               EfiBootServicesCode,
               (RecoveryCapsuleSize - 1) / 0x1000 + 1,
               &Address
               );

    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "AllocatePage Returns: %r\n", Status));

    if (EFI_ERROR(Status)) {
      return Status;
    }

    Buffer = (UINT8*)(UINTN)Address;

    Status = DeviceRecoveryModule->LoadRecoveryCapsule (
                                     PeiServices,
                                     DeviceRecoveryModule,
                                     0,
                                     Buffer
                                     );

    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "LoadRecoveryCapsule Returns: %r\n", Status));

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Update FV Hob if found
    //
    Status = PeiServicesGetHobList ((VOID **) &Hob.Raw);
    HobOld.Raw = Hob.Raw;
    while (!END_OF_HOB_LIST (Hob)) {
      if (Hob.Header->HobType == EFI_HOB_TYPE_FV) {
        DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Hob FV Length: %x\n", Hob.FirmwareVolume->Length));
        if (Hob.FirmwareVolume->Length > 0x50000) {
          HobUpdate = TRUE;
          //
          // This looks like the Hob we are interested in
          //
          DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Hob Updated\n"));
          Hob.FirmwareVolume->BaseAddress = (UINTN)Buffer;
          Hob.FirmwareVolume->Length = RecoveryCapsuleSize;
        }
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }

    //
    // build FV Hob if it is not built before
    //
    if (!HobUpdate) {
      DEBUG ((DEBUG_INFO | DEBUG_LOAD, "FV Hob is not found, Build FV Hob then..\n" ));

      BuildFvHob (
        (UINTN)Buffer,
        RecoveryCapsuleSize
      );
    }
  }
  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Recovery Module Returning: %r\n", Status));
  return Status;
}


/**
  Provide the functionality of the Recovery Module.

  @retval  Status  EFI_SUCCESS if the interface could be successfully
                   installed
**/
EFI_STATUS
EFIAPI
InitializeRecovery (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = PeiServicesInstallPpi (&mRecoveryPpiList);

  return Status;
}