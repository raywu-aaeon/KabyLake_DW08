/** @file
  Base Board Information boot time changes.
  SMBIOS type 2.

  @par Revision Reference:
  SMBIOS Specification version 2.8.0 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
  Unified Extensible Firmware Interface (UEFI) Specifications: http://www.uefi.org/specs/

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

@par Specification Reference:
**/

#include "CommonHeader.h"
#include "SmbiosMisc.h"
#include <PlatformBoardConfig.h>
#include <Library/BoardConfigLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_INFO *mPlatformInfo;

/**
  This function makes boot time changes to the contents of the
  MiscBaseBoardManufacturer (Type 2).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscBaseBoardManufacturer)
{
  CHAR8                           *OptionalStrStart;
  UINTN                           LenOfStrings;
  UINTN                           NumOfStrings;
  CHAR16                          *NewString;
  UINT16                          *FabBoardName;
  EFI_STATUS                      Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  SMBIOS_TABLE_TYPE2              *SmbiosRecord;
  SMBIOS_TABLE_TYPE02_STRINGS     SmbiosTableType02Strings;
  CHAR16                          *StrBufferStart;
  CHAR16                          *StrBufferPtr;
  UINTN                           StrBuffSize;
  UINTN                           OptionalStrBufferSize;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((DEBUG_INFO, "(MiscBaseBoardManufacturer) Entry.\n"));
  ZeroMem (&SmbiosTableType02Strings, sizeof(SMBIOS_TABLE_TYPE02_STRINGS));
  LenOfStrings    = 0;
  NumOfStrings    = 0;

  NewString       = AllocateZeroPool (SMBIOS_STRING_MAX_LENGTH);
  if (NewString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StrBufferStart  = AllocateZeroPool (6 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16));
  if (StrBufferStart == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StrBufferPtr    = StrBufferStart;

  Status = gBS->LocateProtocol (
                  &gPlatformInfoProtocolGuid,
                  NULL,
                  (VOID **) &mPlatformInfo
                  );
  ASSERT_EFI_ERROR(Status);

  //
  // Initialize SMBIOS Tables Type02 strings
  //
  StrBuffSize = (6 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16)) / sizeof (CHAR16);
  FabBoardName = (CHAR16 *) (UINTN) PcdGet64 (PcdSmbiosFabBoardName);
  if (FabBoardName) {
    StrCpyS (NewString, SMBIOS_STRING_MAX_LENGTH / sizeof (CHAR16), FabBoardName);
    SmbiosStrInit (&SmbiosTableType02Strings.ProductName, NewString, &StrBufferPtr, &NumOfStrings, &LenOfStrings, &StrBuffSize);
  } else {
    SmbiosStrInit (&SmbiosTableType02Strings.ProductName, SMBIOS_MISC_TYPE02_BOARD_PRODUCT_NAME, &StrBufferPtr, &NumOfStrings, &LenOfStrings, &StrBuffSize);
    DEBUG ((DEBUG_WARN, "(MiscBaseBoardManufacturer) BoardId problem!! Set default string as product name.\n"));
  }

  UnicodeValueToString (NewString, 0, (INT64) mPlatformInfo->BoardRev + 1, 0);
  SmbiosStrInit (&SmbiosTableType02Strings.Version, NewString, &StrBufferPtr, &NumOfStrings, &LenOfStrings, &StrBuffSize);

  SmbiosStrInit (&SmbiosTableType02Strings.Manufacturer, SMBIOS_MISC_TYPE02_BOARD_MANUFACTURER, &StrBufferPtr, &NumOfStrings, &LenOfStrings, &StrBuffSize);
  SmbiosStrInit (&SmbiosTableType02Strings.SerialNumber, SMBIOS_MISC_TYPE02_BOARD_SERIAL_NUMBER, &StrBufferPtr, &NumOfStrings, &LenOfStrings, &StrBuffSize);
  SmbiosStrInit (&SmbiosTableType02Strings.AssetTag, SMBIOS_MISC_TYPE02_BOARD_ASSET_TAG, &StrBufferPtr, &NumOfStrings, &LenOfStrings, &StrBuffSize);
  SmbiosStrInit (&SmbiosTableType02Strings.LocationInChassis, SMBIOS_MISC_TYPE02_BOARD_CHASSIS_LOCATION, &StrBufferPtr, &NumOfStrings, &LenOfStrings, &StrBuffSize);

  //
  // Two zeros following the last string.
  //
  OptionalStrBufferSize = LenOfStrings + NumOfStrings + 1;
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE2) + OptionalStrBufferSize);
  if (SmbiosRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE2) + OptionalStrBufferSize);
  CopyMem (SmbiosRecord, RecordData, sizeof (SMBIOS_TABLE_TYPE2));

  //
  // Update SMBIOS Tables Type02 strings
  // Since we fill NumberOfContainedObjectHandles = 0 for simple, just after this filed to fill string
  //
  OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
  SmbiosStringsUpdate ((CHAR16 **) &SmbiosTableType02Strings, (UINT8 *) OptionalStrStart, NumOfStrings, OptionalStrBufferSize);

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  FreePool(StrBufferStart);
  FreePool(NewString);

  return Status;
}
