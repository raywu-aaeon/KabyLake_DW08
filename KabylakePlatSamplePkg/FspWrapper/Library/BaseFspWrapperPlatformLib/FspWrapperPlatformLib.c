/** @file
  Provide FSP wrapper platform related function.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2017 Intel Corporation.

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

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/FspWrapperApiLib.h>
#include <Library/FspPolicyInitLib.h>
#include <FspEas.h>
#include <FspmUpd.h>
#include <FspsUpd.h>

#include <Ppi/ReadOnlyVariable2.h>
#include <S3MemoryVariable.h> // AdvancedFeaturesContent
#include <Library/OcWdtLib.h>
#include <Library/PchResetLib.h>
// AMI_OVERRIDE_START - EIP216501: Use EDKII boot script implementation on Intel platforms.
#include <Guid/AcpiVariableCompatibility.h>
// AMI_OVERRIDE_END - EIP216501: Use EDKII boot script implementation on Intel platforms.


/**
  This function overrides the default configurations in the FSP-M UPD data region.

  @note At this point, memory is NOT ready, PeiServices are available to use.

  @param[in,out] FspUpdRgnPtr   A pointer to the UPD data region data strcture.

**/
VOID
EFIAPI
UpdateFspmUpdData (
  IN OUT VOID        *FspUpdRgnPtr
  )
{
  EFI_STATUS         Status;

  FspPolicyInitPreMem ((FSPM_UPD *)FspUpdRgnPtr);

  //
  // Install PCH RESET PPI and EFI RESET2 PeiService
  //
  Status = PchInitializeReset ();
  ASSERT_EFI_ERROR (Status);

  ///
  /// If there was unexpected reset but no WDT expiration and no resume from S3/S4,
  /// clear unexpected reset status and enforce expiration. This is to inform Firmware
  /// which has no access to unexpected reset status bit, that something went wrong.
  ///
  OcWdtResetCheck();

  Status = OcWdtInit ();
  ASSERT_EFI_ERROR (Status);
}

/**
  This function overrides the default configurations in the FSP-S UPD data region.

  @param[in,out] FspUpdRgnPtr   A pointer to the UPD data region data strcture.

**/
VOID
EFIAPI
UpdateFspsUpdData (
  IN OUT VOID        *FspUpdRgnPtr
  )
{
  FspPolicyInit ((FSPS_UPD *)FspUpdRgnPtr);
}

/**
  Update TempRamExit parameter.

  @note At this point, memory is ready, PeiServices are available to use.

  @return TempRamExit parameter.
**/
VOID *
EFIAPI
UpdateTempRamExitParam (
  VOID
  )
{
  return NULL;
}
//
// AdvancedFeaturesBegin
//
/**
  Get S3 PEI memory information.

  @note At this point, memory is ready, and PeiServices are available to use.
  Platform can get some data from SMRAM directly.

  @param[out] S3PeiMemSize  PEI memory size to be installed in S3 phase.
  @param[out] S3PeiMemBase  PEI memory base to be installed in S3 phase.

  @return If S3 PEI memory information is got successfully.
**/
EFI_STATUS
EFIAPI
GetS3MemoryInfo (
  OUT UINT64               *S3PeiMemSize,
  OUT EFI_PHYSICAL_ADDRESS *S3PeiMemBase
  )
{
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  EFI_STATUS                      Status;
  UINT64                          SystemMemoryLength = 0;
// AMI_OVERRIDE_START - EIP216501: Use EDKII boot script implementation on Intel platforms.
#if 0
  S3_MEMORY_VARIABLE              S3MemVariable;
#else  
  UINT64                          AcpiVariableSet64;
  ACPI_VARIABLE_SET_COMPATIBILITY *pAcpiVariableSet = NULL;  
#endif
// AMI_OVERRIDE_END - EIP216501: Use EDKII boot script implementation on Intel platforms.

  *S3PeiMemBase = 0;
  *S3PeiMemSize = 0;

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi(
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             &VariableServices                 // PPI
             );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    goto GetS3MemoryInfoExit;
  }

// AMI_OVERRIDE_START - EIP216501: Use EDKII boot script implementation on Intel platforms.
#if 0 

  //
  // Get S3 Memory Variable
  //
  VariableSize = sizeof (S3_MEMORY_VARIABLE);
  Status = VariableServices->GetVariable(
                               VariableServices,
                               S3_MEMORY_VARIABLE_NAME,
                               &gS3MemoryVariableGuid,
                               NULL,
                               &VariableSize,
                               &S3MemVariable
                               );
  ASSERT(Status == EFI_SUCCESS);

  if (Status != EFI_SUCCESS) {
    goto GetS3MemoryInfoExit;
  }

  *S3PeiMemBase = S3MemVariable.AcpiReservedMemoryBase;
  *S3PeiMemSize = S3MemVariable.AcpiReservedMemorySize;
  SystemMemoryLength = S3MemVariable.SystemMemoryLength;
#else
// Use EDKII boot script implementation on Intel platforms. 
  VariableSize = sizeof (AcpiVariableSet64);
  Status = VariableServices->GetVariable( 
                               VariableServices,
                               ACPI_GLOBAL_VARIABLE,
                               &gEfiAcpiVariableCompatiblityGuid,
                               NULL,
                               &VariableSize,
                               &AcpiVariableSet64);
  ASSERT_EFI_ERROR (Status);
  
  if (Status != EFI_SUCCESS) {
    goto GetS3MemoryInfoExit;
  }  
  
  pAcpiVariableSet = (ACPI_VARIABLE_SET_COMPATIBILITY *) (UINTN) AcpiVariableSet64;    

  *S3PeiMemBase = pAcpiVariableSet->AcpiReservedMemoryBase;
  *S3PeiMemSize = pAcpiVariableSet->AcpiReservedMemorySize;
  SystemMemoryLength = pAcpiVariableSet->SystemMemoryLength;  
#endif
// AMI_OVERRIDE_END - EIP216501: Use EDKII boot script implementation on Intel platforms. 
GetS3MemoryInfoExit:
  DEBUG ((DEBUG_INFO, "AcpiReservedMemoryBase = 0x%x\n", *S3PeiMemBase));
  DEBUG ((DEBUG_INFO, "AcpiReservedMemorySize = 0x%x\n", *S3PeiMemSize));
  DEBUG ((DEBUG_INFO, "System memory length from previous boot = 0x%x\n", SystemMemoryLength));

  return Status;
}
//
// AdvancedFeaturesEnd
//