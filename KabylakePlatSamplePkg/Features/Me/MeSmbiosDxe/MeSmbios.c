/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2012 - 2016 Intel Corporation.

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

#include "MeSmbios.h"
#include <Guid/EventGroup.h>

extern UINT8 SmbiosMiscStrings[];

VOID
EFIAPI
UpdateMeSmbiosTable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
/*++
Description:

  Parses the mMiscSubclassDataTable structure and adds tables 82 and 83 to SMBIOS

Arguments:

  Event                   Pointer to the event that triggered this Callback Function
  Context                 VOID Pointer required for Callback function

Returns:

  EFI_SUCCESS             The data was successfully reported to the Data Hub

--*/
{
  EFI_SMBIOS_TABLE_HEADER       *RecordDataHeader;
  EFI_SMBIOS_TABLE_HEADER       *TempRecordHeader;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINT32                        ThisRecordType;
  UINT32                        LastRecordType;
  EFI_SMBIOS_PROTOCOL           *Smbios;
  EFI_SMBIOS_HANDLE             SmbiosHandle;

  ThisRecordType = 0;
  LastRecordType = 0;
  SmbiosHandle   = SMBIOS_HANDLE_PI_RESERVED;
  Status         = EFI_SUCCESS;

  for (Index = 0; Index < mSmbiosMiscDataTableEntries; Index++) {
    //
    // Initialize SMBIOS Record pointer
    //
    RecordDataHeader         = (EFI_SMBIOS_TABLE_HEADER *) mMiscSubclassDataTable[Index].RecordData;
    RecordDataHeader->Handle = SMBIOS_HANDLE_PI_RESERVED;

    //
    // Remove the table from SMBIOS tables for updating a new one
    //
    Status = gBS->LocateProtocol (
                    &gEfiSmbiosProtocolGuid,
                    NULL,
                    (VOID *) &Smbios
                    );
// AMI_OVERRIDE_START - If SmbiosProtocol not found, end this callback function instead of continue.
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "UpdateMeSmbiosTable: LocateProtocol gEfiSmbiosProtocolGuid failed - %r\n", Status));
    ASSERT_EFI_ERROR (Status);
      goto Exit;
    }
// AMI_OVERRIDE_END - If SmbiosProtocol not found, end this callback function instead of continue.
    do {
      Status = Smbios->GetNext (
                           Smbios,
                           &SmbiosHandle,
                           NULL,
                           &TempRecordHeader,
                           NULL
                           );
      if (EFI_ERROR(Status)) {
        break;
      }

      if (TempRecordHeader->Type == RecordDataHeader->Type) {
        Status = Smbios->Remove (
                             Smbios,
                             SmbiosHandle
                             );
        ASSERT_EFI_ERROR (Status);
        DEBUG ((DEBUG_INFO, "UpdateMeSmbiosTable() SMBIOS Type %d has been removed!!\n", RecordDataHeader->Type));
      }
    } while (Status == EFI_SUCCESS);

    if (mMiscSubclassDataTable[Index].Function != NULL) {
      //
      // The entry has a valid function pointer.
      //
      Status = (*mMiscSubclassDataTable[Index].Function) (
                            mMiscSubclassDataTable[Index].RecordType,
                            &mMiscSubclassDataTable[Index].RecordLen,
                            (SMBIOS_STRUCTURE *)RecordDataHeader
                            );
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "Type %d table function failed - %r\n", RecordDataHeader->Type, Status));
        return;
      }
    }

    Status = Smbios->Add (
                      Smbios,
                      NULL,
                      &RecordDataHeader->Handle,
                      RecordDataHeader
                      );
    if (!EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "UpdateMeSmbiosTable: The type %d table is installed into SMBIOS records\n", RecordDataHeader->Type));
    } else {
      DEBUG ((DEBUG_ERROR, "UpdateMeSmbiosTable: The type %d table is not installed into SMBIOS records - %r\n",
        RecordDataHeader->Type, Status));
    }
  }

// AMI_OVERRIDE_START - If SmbiosProtocol not found, end this callback function instead of continue.
Exit:
// AMI_OVERRIDE_END - If SmbiosProtocol not found, end this callback function instead of continue.
  gBS->CloseEvent(Event);
}

EFI_STATUS
EFIAPI
MeSmbiosDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++
Description:

  Standard EFI driver point. This driver registers  the notify function to reports any generated
  data to the DataHub.

Arguments:

  ImageHandle             Handle for the image of this driver
  SystemTable             Pointer to the EFI System Table

Returns:

  EFI_SUCCESS             The function completed successfully

--*/
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  //
  // Register End of DXE event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UpdateMeSmbiosTable,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
