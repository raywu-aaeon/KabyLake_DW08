/** @file
  Bios Vendor boot time changes.
  SMBIOS type 0.

  @par Revision Reference:
  SMBIOS Specification version 2.8.0 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
  Unified Extensible Firmware Interface (UEFI) Specifications: http://www.uefi.org/specs/

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2019 Intel Corporation.

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
#include <Library/BiosIdLib.h>
#include <PlatformBoardId.h>

/**
  Field Filling Function. Transform an UINTN to a byte, with '64k' as the unit.

  @param[in]  Value                   The value based on UINTN.

  @retval     Value                   The value which is transformed by '64k'.

**/
UINTN
GetValueBy64KUnit(
  IN UINTN          Value
  )
{
  UINTN         Uint64K;

  Uint64K       = 0x10000;

  return Value/Uint64K;
}

/**
  This function returns the BiosMajorVersion and BiosMinorVersion by getting and converting
  BIOS ID.

  @param[in]  BiosVersionString     The Bios Version string for the conversion.
  @param[out] MajorVersion          The Bios Major Version out of the conversion
  @param[out] MinorVersion          The Bios Minor Version out of the conversion

  @retval     EFI_SUCCESS           BIOS Version & Release Date and Time have been got successfully.
  @retval     EFI_INVALID_PARAMETER Either one of the parameters is NULL.

**/
EFI_STATUS
GetSystemBiosVersion (
  IN  CHAR16      *BiosVersionString,
  OUT UINT8       *MajorVersion,
  OUT UINT8       *MinorVersion
  )
{
  CHAR8           *Buffer;
  UINTN           BufferSize;
  BIOS_ID_STRING  BiosIdString;

  if ((BiosVersionString == NULL) || (MajorVersion == NULL) || (MinorVersion == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (&BiosIdString, BiosVersionString, sizeof (BIOS_ID_STRING));

  if (StrLen (BiosIdString.VersionMajor) > StrLen (BiosIdString.VersionMinor)) {
    BufferSize = (StrLen (BiosIdString.VersionMajor) + 1) * sizeof (CHAR8);
  } else {
    BufferSize = (StrLen (BiosIdString.VersionMinor) + 1) * sizeof (CHAR8);
  }

  Buffer = AllocateZeroPool (BufferSize);
  ASSERT (Buffer != NULL);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // The left most character of the MAJOR VERSION field within the BIOS ID string {Offset 0} specifies
  // the color of the bar at the top of the screen.
  // Make it to zero for the computation of System BIOS Major Release
  //
  BiosIdString.VersionMajor[0] = (CHAR16)'0';
  UnicodeStrToAsciiStrS (BiosIdString.VersionMajor, Buffer, BufferSize);
  *MajorVersion = (UINT8) AsciiStrDecimalToUintn(Buffer);

  UnicodeStrToAsciiStrS (BiosIdString.VersionMinor, Buffer, BufferSize);
  *MinorVersion = (UINT8) AsciiStrDecimalToUintn (Buffer);

  FreePool (Buffer);

  return EFI_SUCCESS;
}
/**
  This function makes boot time changes to the contents of the
  MiscBiosVendor (Type 0).

  @param[in]  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval     EFI_SUCCESS                All parameters were valid.
  @retval     EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval     EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscBiosVendor)
{
  CHAR8                        *OptionalStrStart;
  UINTN                        StringNumber;
  UINTN                        StringLength;
  UINTN                        BiosPhysicalSizeHexValue;
  UINTN                        BiosPhysicalSizeExponent;
  CHAR16                       ReleaseDate[100];
  CHAR16                       Version[100];
  UINT8                        SystemBiosMajorVersion;
  UINT8                        SystemBiosMinorVersion;
  EFI_STATUS                   Status;
  SMBIOS_TABLE_TYPE0           *SmbiosRecord;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  SMBIOS_TABLE_TYPE00_STRINGS  SmbiosTableType00Strings;
  CHAR16                       *StrBufferStart;
  CHAR16                       *StrBufferPtr;
  PLATFORM_INFO                *PlatformInfo;
  UINTN                        StrBuffSize;
  UINTN                        OptionalStrBufferSize;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "(MiscBiosVendor) Entry.\n"));
  ZeroMem (&SmbiosTableType00Strings, sizeof(SMBIOS_TABLE_TYPE00_STRINGS));
  BiosPhysicalSizeHexValue        = 0x0;
  BiosPhysicalSizeExponent        = 0x0;
  StringNumber                    = 0;
  StringLength                    = 0;
  StrBufferStart                  = AllocateZeroPool (3 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16));
  StrBufferPtr                    = StrBufferStart;
  StrBuffSize                     = (3 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16)) / sizeof (CHAR16);

  //
  // Update BIOS version and release date string.
  //
  Status = GetBiosVersionDateTime (Version, ReleaseDate, NULL);
  SmbiosStrInit (&SmbiosTableType00Strings.Vendor, SMBIOS_MISC_TYPE00_VENDOR, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
  SmbiosStrInit (&SmbiosTableType00Strings.BiosVersion, Version, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
  SmbiosStrInit (&SmbiosTableType00Strings.BiosReleaseDate, ReleaseDate, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);

  //
  // Update the BiosPhysicalSize
  //
  BiosPhysicalSizeHexValue = PcdGet32 (PcdFlashAreaSize);

  //
  // Two zeros following the last string.
  //
  OptionalStrBufferSize = StringLength + StringNumber + 1;
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE0) + OptionalStrBufferSize);
  if (SmbiosRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE0) + OptionalStrBufferSize);
  CopyMem (SmbiosRecord, RecordData, sizeof (SMBIOS_TABLE_TYPE0));

  //
  // Update the BiosSize
  //
  SmbiosRecord->BiosSize = (UINT8)(GetValueBy64KUnit(BiosPhysicalSizeHexValue) - 1);

  //
  // Update the System BIOS version
  //
  Status = GetSystemBiosVersion (Version,&SystemBiosMajorVersion, &SystemBiosMinorVersion);
  if (Status == EFI_SUCCESS) {
    SmbiosRecord->SystemBiosMajorRelease = SystemBiosMajorVersion;
    SmbiosRecord->SystemBiosMinorRelease = SystemBiosMinorVersion;
  }

  //
  // Update the EC Revision
  //
  Status = gBS->LocateProtocol (
                &gPlatformInfoProtocolGuid,
                NULL,
                (VOID **) &PlatformInfo
                );
  if (!EFI_ERROR (Status)) {
    if (PlatformInfo->EcPresent) {
      //
      // EC is retuning Version information in either BCD or HEX format
      // KBL S Type C , KBL R , KBLY RVP3 Type C follow HEX format EC version, rest boards follow BCD format
      //
      if ((PlatformInfo->BoardIDOrgvalue == BoardIdKabyLakeSDdr4UdimmEvErb) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdKabyLakeSDdr4UdimmEvCrb) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdKabyLakeSDdr4UdimmCrb) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdKabylakeRDdr4) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdKabylakeRLpddr3) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdKabyLakeYLpddr3Rvp3) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3Rvp3) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3Cff) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3CffCc) ||
          (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeLpddr3Rvp3)) {
        SmbiosRecord->EmbeddedControllerFirmwareMajorRelease = PlatformInfo->EcMajorRevision;
        SmbiosRecord->EmbeddedControllerFirmwareMinorRelease = PlatformInfo->EcMinorRevision;
      } else {
        SmbiosRecord->EmbeddedControllerFirmwareMajorRelease = BCD_TO_DECIMAL(PlatformInfo->EcMajorRevision);
        SmbiosRecord->EmbeddedControllerFirmwareMinorRelease = BCD_TO_DECIMAL(PlatformInfo->EcMinorRevision);
      }
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to locate PlatformInfo Protocol!\n"));
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Update SMBIOS Tables type00 strings
  //
  OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
  SmbiosStringsUpdate ((CHAR16 **) &SmbiosTableType00Strings, (UINT8 *) OptionalStrStart, StringNumber, OptionalStrBufferSize);

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  FreePool(StrBufferStart);
  return Status;
}
