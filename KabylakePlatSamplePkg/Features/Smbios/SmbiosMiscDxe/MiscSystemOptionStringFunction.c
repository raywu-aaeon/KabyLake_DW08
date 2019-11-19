/** @file
  System Configuration Options boot time changes.
  SMBIOS type 12.

  @par Revision Reference:
  SMBIOS Specification version 2.8.0 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
  Unified Extensible Firmware Interface (UEFI) Specifications: http://www.uefi.org/specs/

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

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


/**
  This function makes boot time changes to the contents of the
  MiscSystemOptionString (Type 12).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(SystemOptionString)
{
  CHAR8                       *OptionalStrStart;
  UINTN                       StringNumber;
  UINTN                       StringLength;
  EFI_STATUS                  Status;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  SMBIOS_TABLE_TYPE12         *SmbiosRecord;
  SMBIOS_TABLE_TYPE12_STRINGS SmbiosTableType12Strings;
  CHAR16                      *StrBufferStart;
  CHAR16                      *StrBufferPtr;
  UINTN                       StrBuffSize;
  UINTN                       OptionalStrBufferSize;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "(SystemOptionString) Entry.\n"));
  ZeroMem (&SmbiosTableType12Strings, sizeof(SMBIOS_TABLE_TYPE12_STRINGS));
  StringNumber    = 0;
  StringLength    = 0;
  StrBufferStart  = AllocateZeroPool (1 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16));
  StrBufferPtr    = StrBufferStart;
  StrBuffSize     = (1 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16)) / sizeof (CHAR16);

  //
  // Initialize SMBIOS Tables Type12 strings
  //
  SmbiosStrInit (&SmbiosTableType12Strings.StringCount, SMBIOS_MISC_SYSTEM_OPTION_STRING, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);

  //
  // Two zeros following the last string.
  //
  OptionalStrBufferSize = StringLength + StringNumber + 1;
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE12) + OptionalStrBufferSize);
  if (SmbiosRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE12) + OptionalStrBufferSize);
  CopyMem (SmbiosRecord, RecordData, sizeof (SMBIOS_TABLE_TYPE12));

  //
  // Update SMBIOS Tables Type12 strings
  //
  OptionalStrStart = (CHAR8*) (SmbiosRecord + 1);
  SmbiosStringsUpdate ((CHAR16 **)&SmbiosTableType12Strings, (UINT8 *)OptionalStrStart, StringNumber, OptionalStrBufferSize);

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  FreePool(StrBufferStart);
  return Status;
}
