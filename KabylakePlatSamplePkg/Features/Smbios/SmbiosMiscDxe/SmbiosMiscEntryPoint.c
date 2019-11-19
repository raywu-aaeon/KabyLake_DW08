/** @file
  This driver parses the mSmbiosMiscDataTable structure and reports
  any generated data using SMBIOS protocol.

  @par Revision Reference:
  SMBIOS Specification version 2.8.0 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
  Unified Extensible Firmware Interface (UEFI) Specifications: http://www.uefi.org/specs/

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2018 Intel Corporation.

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

#include <Guid/SmBios.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_HANDLE   mImageHandle;

GLOBAL_REMOVE_IF_UNREFERENCED EFI_HII_HANDLE  mHiiHandle;

extern EFI_GUID gSystemAccessGuid;

/**
  Publish the Reference code version info as per Firmware Version Info (FVI) Interface Spec v0.8
  using smbios type 14, group association.

  @param[in] Event   - Event whose notification function is being invoked (Ready To Boot).
  @param[in] Context - Pointer to the notification functions context, which is implementation dependent.

**/
VOID
EFIAPI
AddSmbiosT14Callback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_SMBIOS_PROTOCOL               *SmbiosProtocol;
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER           *Record;
  UINT32                            FviTypeCount;       // number of Fvi records locates.
  UINT32                            FviTypeCountOld;
  UINT32                            FviT14Size;         // Size of the SMBIOS record to be installed.
  GROUP_STRUCT                      *FviTableOld;       // Pointers to discovered Fvi entries.
  GROUP_STRUCT                      *FviTableNew;
  SMBIOS_TABLE_TYPE14               *SmbiosTableType14;
  UINT8                             *GroupName;
  UINT8                             *EndPtr;
  static UINT8                      T14FviString[sizeof (T14_FVI_STRING)] = T14_FVI_STRING;
  UINT8                             FviSmbiosType;

  SmbiosProtocol    = NULL;
  FviTableNew       = NULL;
  SmbiosTableType14 = NULL;

  DEBUG ((DEBUG_INFO, "AddSmbiosT14Callback(): Executing SMBIOS T14 callback.\n"));

  //
  // Parse the SMBIOS records for
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID *) &SmbiosProtocol
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "AddSmbiosT14Callback(): Locate SMBIOS protocol failure!!\n"));
    return;
  }

  FviTypeCount  = 0;
  SmbiosHandle  = SMBIOS_HANDLE_PI_RESERVED;
  //
  // Get FVI SMBIOS Type
  //
  FviSmbiosType = PcdGet8(PcdFviSmbiosType);
  do {
    Status = SmbiosProtocol->GetNext (
                               SmbiosProtocol,
                               &SmbiosHandle,
                               NULL,
                               &Record,
                               NULL
                               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "AddSmbiosT14Callback(): The SMBIOS record is the last available record.\n"));
      break;
    }
    //
    // Only One EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS with the label T14FviString.
    // is allowed in the system.
    //
    if (Record->Type == EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS) {
      GroupName = ((UINT8 *)Record) + Record->Length;
      if(AsciiStrCmp ((CHAR8 *) GroupName, (CHAR8 *) T14FviString) == 0) {
        DEBUG ((DEBUG_INFO, "AddSmbiosT14Callback(): The T14 table is already in SMBIOS records.\n"));
        FviTypeCount = 0;       //  mark the set as empty
        break;
      }
    }
    //
    // Locate the FviSmbiosType records.
    //
    if (Record->Type == FviSmbiosType) {
      FviTypeCountOld = FviTypeCount;
      FviTypeCount++;
      FviTableOld = FviTableNew;
      FviTableNew = AllocateZeroPool (sizeof(GROUP_STRUCT)*FviTypeCount);
      if (FviTableNew == NULL) {
        DEBUG ((DEBUG_ERROR, "AddSmbiosT14Callback(): Allocate memory for FviTable failed!!\n"));
        goto AddSmbiosT14CallbackExit;
      }

      if (FviTypeCountOld > 0) {
        CopyMem (FviTableNew, FviTableOld, (sizeof (GROUP_STRUCT) * FviTypeCountOld));
      }

      if (FviTableOld != NULL) {
        FreePool (FviTableOld);
      }

      FviTableNew[FviTypeCount - 1].ItemType    = Record->Type;
      FviTableNew[FviTypeCount - 1].ItemHandle  = Record->Handle;
    }
  } while (Status == EFI_SUCCESS);      // End of retrieving Smbios FviSmbiosType records.

  DEBUG ((DEBUG_INFO, "AddSmbiosT14Callback(): Located %d FviSmbiosType (%d) records\n", FviTypeCount, FviSmbiosType));

  if (FviTypeCount != 0) {

    //
    // Add the Record to the SMBIOS table.
    //
// AMI_OVERRIDE_START - Override for AMI SMBios driver.
#if 0
    FviT14Size = sizeof (SMBIOS_TABLE_TYPE14) + (UINT32) sizeof (T14_FVI_STRING) + (sizeof (GROUP_STRUCT) * (FviTypeCount - 1)) + 1;
#else
    FviT14Size = sizeof (SMBIOS_TABLE_TYPE14) + (UINT32) AsciiStrLen (T14_FVI_STRING) + (sizeof (GROUP_STRUCT) * (FviTypeCount - 1)) + 2;
#endif
// AMI_OVERRIDE_END - Override for AMI SMBios driver.

    SmbiosTableType14 = AllocateZeroPool (FviT14Size);
    if (SmbiosTableType14 == NULL) {
      DEBUG ((DEBUG_ERROR, "AddSmbiosT14Callback(): Allocate memory for FviTable failed!!\n"));
      goto AddSmbiosT14CallbackExit;
    }

    SmbiosTableType14->Hdr.Type = EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS;
    SmbiosTableType14->Hdr.Length = (UINT8) (sizeof (SMBIOS_TABLE_TYPE14) + (sizeof (GROUP_STRUCT) * (FviTypeCount - 1)));
    SmbiosTableType14->Hdr.Handle = SMBIOS_HANDLE_PI_RESERVED;  // Assign an unused handle.
    SmbiosTableType14->GroupName  = 1;
    CopyMem (SmbiosTableType14->Group, FviTableNew, sizeof (GROUP_STRUCT) * FviTypeCount);
    CopyMem ((&SmbiosTableType14->Group[FviTypeCount].ItemType), T14FviString, sizeof (T14FviString));
    EndPtr = (UINT8 *) (SmbiosTableType14);       // Extra zero marks the end of the record.
    EndPtr[FviT14Size - 1] = 0;

    Status = SmbiosProtocol->Add (
                               SmbiosProtocol,
                               NULL,
                               &SmbiosTableType14->Hdr.Handle,
                               (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType14
                               );
    if (!EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "AddSmbiosT14Callback(): The T14 table is installed into SMBIOS records, unload this event.\n"));
      gBS->CloseEvent (Event);
    }
  }

AddSmbiosT14CallbackExit:
  if (SmbiosTableType14 != NULL) {
    FreePool (SmbiosTableType14);
  }
  if (FviTableNew != NULL) {
    FreePool (FviTableNew);
  }
}

/*++
  Install a SMBIOS type 24 record - Hardware Security.
  The SMBIOS record is constructed with the 'Power-on Password Status value' and 'Front Panel Reset
  value' set to 'Not Implemented.  The status of the 'Keyboard Password Status value' and 'Administrator
  Password Status value' set according to their variable settings.

  @param[in] Event   - Event whose notification function is being invoked (Ready To Boot).
  @param[in] Context - Pointer to the notification functions context, which is implementation dependent.

--*/
#if 0  // AMI_OVERRIDE_START - Override for AMI SMBios driver. 
VOID
EFIAPI
AddSmbiosT24Callback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_SMBIOS_PROTOCOL *SmbiosProtocol;
  EFI_STATUS          Status;
  UINT32              T24Size;         // Size of the SMBIOS record to be installed.
  SMBIOS_TABLE_TYPE24 *SmbiosTableType24;
  UINT8               *EndPtr;
  EFI_PASSWORD_STATUS PwdData;
  UINTN               VarSize;

  DEBUG ((DEBUG_INFO, "AddSmbiosT24Callback(): Callback to Create Smbios T24 entry.\n"));

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID *) &SmbiosProtocol
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "AddSmbiosT24Callback(): Locate SMBIOS protocol failure!!\n"));
    return;
  }

  T24Size = sizeof (SMBIOS_TABLE_TYPE24) + 2;
  SmbiosTableType24 = AllocateZeroPool (T24Size);
  if (SmbiosTableType24 == NULL) {
    DEBUG ((DEBUG_ERROR, "AddSmbiosT24Callback(): Allocate memory for T24 table failed!!\n"));
    return;
  }

  SmbiosTableType24->Hdr.Type = EFI_SMBIOS_TYPE_HARDWARE_SECURITY;
  SmbiosTableType24->Hdr.Length = (UINT8) (sizeof (SMBIOS_TABLE_TYPE24));
  SmbiosTableType24->Hdr.Handle = SMBIOS_HANDLE_PI_RESERVED;  // Assign an unused handle.

  VarSize = sizeof (PwdData);

  Status  = gRT->GetVariable (
                   L"PasswordStatus",
                   &gSystemAccessGuid,
                   NULL,
                   &VarSize,
                   &PwdData
                   );

  if(EFI_ERROR(Status)) {

    //
    // If no Password Status variable exists, Set feature to not implemented.
    //
    SmbiosTableType24->HardwareSecuritySettings = (UINT8) ((EfiHardwareSecurityStatusNotImplemented << 6) |
                                                           (EfiHardwareSecurityStatusNotImplemented << 4) |
                                                           (EfiHardwareSecurityStatusNotImplemented << 2) |
                                                           (EfiHardwareSecurityStatusNotImplemented)
                                                           );
  } else {
    SmbiosTableType24->HardwareSecuritySettings = (UINT8) ((EfiHardwareSecurityStatusNotImplemented << 6) |
                                                           ((PwdData.UserName & 0x3) << 4) |
                                                           ((PwdData.AdminName & 0x3) << 2) |
                                                           EfiHardwareSecurityStatusNotImplemented
                                                           );
  }

  EndPtr = (UINT8 *) (SmbiosTableType24);       // Extra zero marks the end of the record.
  EndPtr[T24Size - 2] = 0;
  EndPtr[T24Size - 1] = 0;

  Status = SmbiosProtocol->Add (
                            SmbiosProtocol,
                            NULL,
                            &SmbiosTableType24->Hdr.Handle,
                            (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType24
                            );
  if (!EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "AddSmbiosT24Callback(): The T24 table is installed into SMBIOS records, unload this event.\n"));
    gBS->CloseEvent (Event);
  }
  FreePool(SmbiosTableType24);
}
#endif  // AMI_OVERRIDE_END - Override for AMI SMBios driver.

/**
  Standard EFI driver point.  This driver parses the mSmbiosMiscDataTable
  structure and reports any generated data using SMBIOS protocol.

  @param[in] ImageHandle     Handle for the image of this driver
  @param[in] SystemTable     Pointer to the EFI System Table

  @retval EFI_SUCCESS        The data was successfully stored.

**/
EFI_STATUS
EFIAPI
SmbiosMiscEntryPoint(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
#if 0  // AMI_OVERRIDE_START - Override for AMI SMBios driver. 
  UINTN                Index;
#endif // AMI_OVERRIDE_END - Override for AMI SMBios driver.
  EFI_STATUS           EfiStatus;
  EFI_SMBIOS_PROTOCOL  *Smbios;
  EFI_EVENT            Event;

  DEBUG((DEBUG_INFO, "SmbiosMiscEntryPoint(): Entry~!\n"));

  mImageHandle = ImageHandle;
  EfiStatus = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);

  if (EFI_ERROR(EfiStatus)) {
    DEBUG((DEBUG_ERROR, "Could not locate SMBIOS protocol.  %r\n", EfiStatus));
    return EfiStatus;
  }

#if 0  // AMI_OVERRIDE_START - Override for AMI SMBios driver.
  for (Index = 0; Index < mSmbiosMiscDataTableEntries; ++Index) {
    //
    // If the entry have a function pointer, just log the data.
    //
    if (mSmbiosMiscDataTable[Index].Function != NULL) {
      EfiStatus = (*mSmbiosMiscDataTable[Index].Function)(
        mSmbiosMiscDataTable[Index].RecordData,
        Smbios
        );

      if (EFI_ERROR(EfiStatus)) {
        DEBUG((DEBUG_ERROR, "Misc smbios store error.  Index=%d, ReturnStatus=%r\n", Index, EfiStatus));
        return EfiStatus;
      }
    }
  }
#endif  // AMI_OVERRIDE_END - Override for AMI SMBios driver.

  //
  // This callback will parse the SMBIOS table for FVI and create a Type 14 record.
  //
  DEBUG ((DEBUG_INFO, "SmbiosMiscEntryPoint(): Create Smbios T14 callback.\n"));
  EfiStatus = EfiCreateEventReadyToBootEx (
                TPL_CALLBACK,
                AddSmbiosT14Callback,
                NULL,
                &Event
                );
  ASSERT_EFI_ERROR (EfiStatus);

#if 0  // AMI_OVERRIDE_START - Override for AMI SMBios driver.
  //
  // This callback will install a Type 24 record  - Hardware Security.
  //
  DEBUG ((DEBUG_INFO, "SmbiosMiscEntryPoint(): Create Smbios T24 callback.\n"));
  EfiStatus = EfiCreateEventReadyToBootEx (
                TPL_CALLBACK,
                AddSmbiosT24Callback,
                NULL,
                &Event
                );
  ASSERT_EFI_ERROR (EfiStatus);
#endif  // AMI_OVERRIDE_END - Override for AMI SMBios driver.

  return EfiStatus;
}

/**
  Add an SMBIOS record.

  @param[in]  Smbios            The EFI_SMBIOS_PROTOCOL instance.
  @param[out] SmbiosHandle      A unique handle will be assigned to the SMBIOS record.
  @param[in]  Record            The data for the fixed portion of the SMBIOS record. The format of the record is
                                determined by EFI_SMBIOS_TABLE_HEADER.Type. The size of the formatted area is defined
                                by EFI_SMBIOS_TABLE_HEADER.Length and either followed by a double-null (0x0000) or
                                a set of null terminated strings and a null.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added due to lack of system resources.

**/
EFI_STATUS
AddSmbiosRecord (
  IN EFI_SMBIOS_PROTOCOL        *Smbios,
  OUT EFI_SMBIOS_HANDLE         *SmbiosHandle,
  IN EFI_SMBIOS_TABLE_HEADER    *Record
  )
{
  *SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  return Smbios->Add (
                   Smbios,
                   NULL,
                   SmbiosHandle,
                   Record
                   );
}

/**
  Initialize SMBIOS table strings.

  @param[out] **Destination            The pointer for the destination.
  @param[in]  *Source                  The pointer for the source date.
  @param[out] **StrBuffer              The pointer for string buffer.
  @param[out] *Count                   The accumulated number of strings.
  @param[out] *SizeOfStrings           The accumulated number of strings length.
  @param[in, out]  *StrBuffSize         Size of StrBuffer.

  @retval     EFI_SUCCESS              Successful.
  @retval     EFI_INVALID_PARAMETER    Distination pointer is not NULL.
  @retval     EFI_BUFFER_TOO_SMALL     StrBuffSize is zero.

**/
EFI_STATUS
SmbiosStrInit (
  OUT CHAR16        **Destination,
  IN  CHAR16  CONST *Source,
  OUT CHAR16        **StrBuffer,
  OUT UINTN         *Count,
  OUT UINTN         *SizeOfStrings,
  IN OUT  UINTN     *StrBuffSize
  )
{
  UINTN    StrSize;

  if ((*Destination != NULL) || (*StrBuffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (StrBuffSize == 0) {
    return EFI_BUFFER_TOO_SMALL;
  }
  DEBUG ((DEBUG_INFO, "(SmbiosStrInit) Entry.\n"));
  if (StrLen(Source) > SMBIOS_STRING_MAX_LENGTH) {
    StrSize = SMBIOS_STRING_MAX_LENGTH;
  } else {
    StrSize = StrLen(Source);
  }

  DEBUG ((DEBUG_INFO, "(SmbiosStrInit) Source string: %S\n", Source));
  StrCpyS (*StrBuffer, *StrBuffSize, Source);
  *Destination = *StrBuffer;
  *StrBuffer += (StrSize + 1);
  *StrBuffSize -= (StrSize + 1);

  *SizeOfStrings += StrSize;
  *Count += 1;

  return EFI_SUCCESS;
}

/**
  Update SMBIOS strings.

  @param[in]  **StringArray        The strings to be updated.
  @param[in]  *StarAddr            The pointer of start location.
  @param[in]  NumOfStrings         The number of strings.
  @param[in]  BufferSize           The usage size from start location, *StarAddr.
**/
VOID
SmbiosStringsUpdate (
  CHAR16            **StringArray,
  UINT8             *StartAddr,
  UINTN             NumOfStrings,
  UINTN             BufferSize
  )
{
  UINTN      LenStr;
  UINTN      IdxStr;

  LenStr = 0;
  IdxStr = 0;

  for (IdxStr = 0; IdxStr < NumOfStrings; IdxStr++) {
    UnicodeStrToAsciiStrS (StringArray[IdxStr], (CHAR8*) (StartAddr + LenStr + IdxStr), BufferSize);
    LenStr += StrLen(StringArray[IdxStr]);
    BufferSize -= StrLen(StringArray[IdxStr]);
  }
}
