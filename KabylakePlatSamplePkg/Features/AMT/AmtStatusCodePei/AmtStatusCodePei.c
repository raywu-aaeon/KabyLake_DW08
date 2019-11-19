/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2016 Intel Corporation.

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

#include <Ppi/ReportStatusCodeHandler.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/AmtStatusCode.h>
#include <AmtForcePushPetHob.h>
#include <AmtPetQueueHob.h>
//AMI_OVERRIDE_START - Send AsfMessageOSWakeVector directly when S3 resume
#include <Token.h> 
#if AMT_OS_Wake_Vector_Message
#include "AlertStandardFormat.h"
#include <Pi/PiBootMode.h>
#include "AmiStatusCodes.h"

extern EFI_STATUS
SendPostPacket (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE  MessageType
  );
#endif
//AMI_OVERRIDE_END - Send AsfMessageOSWakeVector directly when S3 resume


EFI_STATUS
AmtQueuePetMessage (
    IN  EFI_STATUS_CODE_TYPE    Type,
    IN  EFI_STATUS_CODE_VALUE   Value
    )
/*++
Routine Description:

  This routine puts PET message to MessageQueue, which will be sent later.

Arguments:

  Type        - StatusCode message type.
  Value       - StatusCode message value.

Returns:

  EFI_SUCCESS      - The function completed successfully

--*/
{
    AMT_PET_QUEUE_HOB     *PETQueueHob;
    EFI_STATUS            Status;

    // Create PET queue hob
    Status = PeiServicesCreateHob ( EFI_HOB_TYPE_GUID_EXTENSION,
                                    sizeof(AMT_PET_QUEUE_HOB),
                                    (VOID **) &PETQueueHob );
    if (EFI_ERROR (Status)) {
        return Status;
    }
    PETQueueHob->EfiHobGuidType.Name = gAmtPetQueueHobGuid;
    PETQueueHob->Type = Type;
    PETQueueHob->Value = Value;

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmtStatusCode(
  IN       EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_STATUS_CODE_TYPE        Type,
  IN       EFI_STATUS_CODE_VALUE       Value,
  IN       UINT32                      Instance,
  IN       EFI_GUID                    *CallerId,
  IN       EFI_STATUS_CODE_DATA        *Data
)
/*++
Routine Description:

  Provides an interface that a software module can call to report an ASF PEI status code.

Arguments:

  PeiServices - PeiServices pointer.

  Type     - Indicates the type of status code being reported.

  Value    - Describes the current status of a hardware or software entity.
             This included information about the class and subclass that is
             used to classify the entity as well as an operation.

  Instance - The enumeration of a hardware or software entity within
             the system. Valid instance numbers start with 1.

  CallerId - This optional parameter may be used to identify the caller.
             This parameter allows the status code driver to apply different
             rules to different callers.

  Data     - This optional parameter may be used to pass additional data.

Returns:

  None

--*/
{
    AMT_STATUS_CODE_PPI             *AmtStatusCode;
    EFI_STATUS                      Status;

//AMI_OVERRIDE_START - Send AsfMessageOSWakeVector directly when S3 resume
#if AMT_OS_Wake_Vector_Message   
    EFI_BOOT_MODE                   BootMode;
    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
	    if(Value == (EFI_SOFTWARE_PEI_MODULE | AMI_PEIM_PC_OS_WAKE) || 
	       Value == (EFI_SOFTWARE_PEI_MODULE | EFI_SW_DXE_RT_PC_S3) ||
	       Value == (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_OS_WAKE))
	    {
		    Status = SendPostPacket(AsfMessageOSWakeVector);
	    }
    }
#endif    
//AMI_OVERRIDE_END - Send AsfMessageOSWakeVector directly when S3 resume
    
    Status = PeiServicesLocatePpi ( &gAmtStatusCodePpiGuid,
                                    0,
                                    NULL,
                                    (VOID **) &AmtStatusCode );
    if ( EFI_ERROR (Status) ) {
        if (((Type & EFI_STATUS_CODE_TYPE_MASK) != EFI_PROGRESS_CODE) &&
            ((Type & EFI_STATUS_CODE_TYPE_MASK) != EFI_ERROR_CODE)) {
            return EFI_SUCCESS;
        }

        // Register to Hob

        // Create PET queue hob
        AmtQueuePetMessage (Type, Value);

        return EFI_SUCCESS;
    }

    AmtStatusCode->ReportStatusCode (
                     AmtStatusCode,
                     Type,
                     Value,
                     Instance,
                     CallerId,
                     Data
                     );

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
AmtStatusInit (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
/*++
Routine Description:

  Init routine for PEI ASF StatusCode.

Arguments:

  FfsHeader   - FfsHeader pointer.
  PeiServices - PeiServices pointer.

Returns:

  EFI_SUCCESS      - The function completed successfully

--*/
{
  EFI_STATUS                  Status;
  EFI_PEI_RSC_HANDLER_PPI     *RscHandlerPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **) &RscHandlerPpi
             );
  ASSERT_EFI_ERROR (Status);

  Status = RscHandlerPpi->Register ((EFI_PEI_RSC_HANDLER_CALLBACK) AmtStatusCode);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
