/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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


//#include "Efi.h"
#include <PiPei.h>
#include <Base.h>
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PeiSwbpeLib.h>
#include <Ppi/MemoryDiscovered.h>
#include <Library/PeiServicesLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR  mDsRamNotifyDesc = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiMemoryDiscoveredPpiGuid,
  DsRamAllocationCallback
};

//
// Function Implementations
//

VOID
DsAllocation (
  VOID
  )
/*++

Routine Description:

  Allocate NEM buffer for Data Store function and set callback for DRAM allocation in PEI phase

Arguments:

  None

Returns:

  None

--*/
{
  EFI_STATUS                 Status;
  EFI_PHYSICAL_ADDRESS       *DsNemAddress;

  //
  // Check for Debugger
  //
  DsBreakpointEvent (DsConfigSwBpeIdCheck, 0, 0, &Status);

  //
  // If Debugger is present, ...
  //
  if (Status == EFI_SUCCESS) {
      //
      // ...allocate DS buffer in NEM
      //
      Status = PeiServicesAllocatePool (
                                DS_CONFIG_NEM_SIZE,
                                (VOID **) &DsNemAddress
                                );
      ASSERT (Status == EFI_SUCCESS);

      //
      // Notify Debugger
      //
      DsBreakpointEvent (
        DsConfigSwBpeIdNemStart,
        (EFI_PHYSICAL_ADDRESS) (UINTN) DsNemAddress,
        DS_CONFIG_NEM_SIZE,
        &Status
        );

    //
    // Register Notify callback for doing RAM buffer allocation
    //
    Status = PeiServicesNotifyPpi (&mDsRamNotifyDesc);
    ASSERT (Status == EFI_SUCCESS);
  }
}

EFI_STATUS
EFIAPI
DsRamAllocationCallback (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN VOID                         *Ppi
  )
/*++

Routine Description:

  Internal function to allocate RAM buffer for DS function in PEI phase

Arguments:

  PeiServices       General purpose services available to every PEIM.
  NotifyDescriptor  The notification structure this PEIM registered on install.
  Ppi               The memory discovered PPI.  Not used.

Returns:

  EFI_SUCCESS        The function completed successfully.

--*/
{
  EFI_STATUS                 Status;
  EFI_PHYSICAL_ADDRESS       DsDramAddress;
  UINTN                      SizeInByte;

  //
  // Disable DS Cache As Ram buffer
  //
  DsBreakpointEvent (DsConfigSwBpeIdNemStop, 0, 0, &Status);

  //
  // Check for Debugger
  //
  DsBreakpointEvent (DsConfigSwBpeIdCheck, 0, 0, &Status);

  //
  // If Debugger is present, ...
  //
  if (Status == EFI_SUCCESS) {
    SizeInByte = DS_CONFIG_DRAM_SIZE;
    //
    // ...allocate DS buffer from DRAM
    //
    Status = PeiServicesAllocatePages (
                              EfiACPIMemoryNVS,
                              SizeInByte / EFI_PAGE_SIZE,
                              &DsDramAddress
                              );
    ASSERT (Status == EFI_SUCCESS);

    //
    // Notify Debugger
    //
    DsBreakpointEvent (
      DsConfigSwBpeIdDramStart,
      DsDramAddress,
      DS_CONFIG_DRAM_SIZE,
      &Status
      );
  }

  return Status;
}
