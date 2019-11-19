/** @file
  Port Connector Information boot time changes.
  SMBIOS type 8.

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
  MiscPortConnectorInformation (Type 8).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscPortInternalConnectorDesignator)
{
  CHAR8                       *OptionalStrStart;
  UINTN                       StringNumber;
  UINTN                       StringLength;
  EFI_STATUS                  Status;
  SMBIOS_TABLE_TYPE8          *SmbiosRecord;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  SMBIOS_TABLE_TYPE8          *ForType8InputData;
  SMBIOS_TABLE_TYPE8_STRINGS  SmbiosTableType8Strings;
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

  DEBUG ((DEBUG_INFO, "(MiscPortInternalConnectorDesignator) Entry.\n"));
  ZeroMem (&SmbiosTableType8Strings, sizeof(SMBIOS_TABLE_TYPE8_STRINGS));
  StringNumber          = 0;
  StringLength          = 0;
  ForType8InputData     = (SMBIOS_TABLE_TYPE8 *)RecordData;
  StrBufferStart        = AllocateZeroPool (2 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16));
  StrBufferPtr          = StrBufferStart;
  StrBuffSize           = (2 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16)) / sizeof (CHAR16);

  //
  // Initialize SMBIOS Tables Type8 strings
  //
  switch (ForType8InputData->InternalReferenceDesignator) {
    case SMBIOS_MISC_PORT_TOKEN_KEYBOARD:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_KEYBOARD, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_KEYBOARD, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_MOUSE:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_MOUSE, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_MOUSE, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_COM1:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_COM1, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_COM1, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_VIDEO:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_VIDEO, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_VIDEO, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_HDMI:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_HDMI, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_HDMI, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_USB1:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_USB1, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_USB1, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_USB2:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_USB2, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_USB2, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_USB3:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_USB3, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_USB3, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_USB4:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_USB4, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_USB4, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_USB5:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_USB5, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_USB5, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_USB201:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_USB201, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_USB201, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_USB202:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_USB202, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_USB202, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_NETWORK:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_NETWORK, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_NETWORK, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_SATA0:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_SATA0, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_SATA0, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_ESATA4:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_ESATA4, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_ESATA4, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_ESATA3:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_ESATA3, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_ESATA3, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_SATA2:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_SATA2, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_SATA2, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_SATA1:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_SATA1, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_SATA1, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_ACIN:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_ACIN, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_ACIN, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_PCHJTAG:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_PCHJTAG, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_PCHJTAG, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_PORT80:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_PORT80, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_PORT80, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_2X8HEADER:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_2X8HEADER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_2X8HEADER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_8PINHEADER:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_8PINHEADER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_8PINHEADER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_HDAHDMI:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_HDAHDMI, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_HDAHDMI, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_MKEYBOARD:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_MKEYBOARD, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_MKEYBOARD, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_SPI:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_SPI, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_SPI, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_LPCDOCKING:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_LPCDOCKING, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_LPCDOCKING, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_SIDEBAND:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_SIDEBAND, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_SIDEBAND, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_LPCSLOT:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_LPCSLOT, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_LPCSLOT, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_PCHXDP:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_PCHXDP, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_PCHXDP, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_SATAPOWER:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_SATAPOWER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_SATAPOWER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_FPHEADER:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_FPHEADER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_FPHEADER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_ATXPOWER:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_ATXPOWER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_ATXPOWER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_AVMC:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_AVMC, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_AVMC, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_BATTB:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_BATTB, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_BATTB, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_BATTA:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_BATTA, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_BATTA, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_CPUFAN:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_CPUFAN, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_CPUFAN, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_XDP:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_XDP, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_XDP, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_MEMORY1:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_MEMORY1, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_MEMORY1, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_MEMORY2:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_MEMORY2, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_MEMORY2, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case SMBIOS_MISC_PORT_TOKEN_FANPWR:
      SmbiosStrInit (&SmbiosTableType8Strings.InternalReferenceDesignator, SMBIOS_MISC_PORT_INTERNAL_FANPWR, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      SmbiosStrInit (&SmbiosTableType8Strings.ExternalReferenceDesignator, SMBIOS_MISC_PORT_EXTERNAL_FANPWR, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    default:
      break;
  }
  ForType8InputData->InternalReferenceDesignator = SMBIOS_MISC_STRING_1;

  //
  // Two zeros following the last string.
  //
  OptionalStrBufferSize = StringLength + StringNumber + 1;
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE8) + OptionalStrBufferSize);
  if (SmbiosRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE8) + OptionalStrBufferSize);
  CopyMem (SmbiosRecord, RecordData, sizeof (SMBIOS_TABLE_TYPE8));

  //
  // Update SMBIOS Tables Type8 strings
  //
  OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
  SmbiosStringsUpdate ((CHAR16 **) &SmbiosTableType8Strings, (UINT8 *) OptionalStrStart, StringNumber, OptionalStrBufferSize);

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  SmbiosHandle = 0;
  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  FreePool(StrBufferStart);
  return Status;
}
