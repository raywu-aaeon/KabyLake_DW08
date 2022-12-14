/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/** @file
  Boot service DXE BIOS ID library implementation.

  These functions in this file can be called during DXE and cannot be called during runtime
  or in SMM which should use a RT or SMM library.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BiosIdLib.h>

#include <Guid/BiosId.h>

/**
  This function returns BIOS ID by searching HOB or FV.

  @param[out] BiosIdImage   The BIOS ID got from HOB or FV.

  @retval EFI_SUCCESS               BIOS ID has been got successfully.
  @retval EFI_NOT_FOUND             BIOS ID image is not found, and no parameter will be modified.
  @retval EFI_INVALID_PARAMETER     The parameter is NULL.

**/
EFI_STATUS
EFIAPI
GetBiosId (
  OUT BIOS_ID_IMAGE     *BiosIdImage
  )
{
  EFI_STATUS    Status;
  VOID          *Address;
  UINTN         Size;

  Address = NULL;
  Size = 0;

  if (BiosIdImage == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Address = GetFirstGuidHob (&gBiosIdGuid);
  if (Address != NULL) {
    Size = sizeof (BIOS_ID_IMAGE);
    CopyMem ((VOID *) BiosIdImage, GET_GUID_HOB_DATA (Address), Size);

    DEBUG ((EFI_D_INFO, "DXE get BIOS ID from HOB successfully\n"));
    DEBUG ((EFI_D_INFO, "BIOS ID: %s\n", (CHAR16 *) (&(BiosIdImage->BiosIdString))));
    return EFI_SUCCESS;
  }

  Status = GetSectionFromAnyFv (
             &gBiosIdGuid,
             EFI_SECTION_RAW,
             0,
             &Address,
             &Size
             );
  if ((Status == EFI_SUCCESS) && (Address != NULL)) {
    //
    // BIOS ID image is present in FV.
    //
    Size = sizeof (BIOS_ID_IMAGE);
    CopyMem ((VOID *) BiosIdImage, Address, Size);
    //
    // GetSectionFromAnyFv () allocated buffer for Address, now free it.
    //
    FreePool (Address);

    DEBUG ((EFI_D_INFO, "DXE get BIOS ID from FV successfully\n"));
    DEBUG ((EFI_D_INFO, "BIOS ID: %s\n", (CHAR16 *) (&(BiosIdImage->BiosIdString))));
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
  This function returns the BIOS Version & Release Date and Time by getting and converting BIOS ID.

  @param[out] BiosVersion       The Bios Version out of the conversion.
  @param[out] BiosReleaseDate   The Bios Release Date out of the conversion.
  @param[out] BiosReleaseTime   The Bios Release Time out of the conversion.

  @retval EFI_SUCCESS               BIOS Version & Release Date and Time have been got successfully.
  @retval EFI_NOT_FOUND             BIOS ID image is not found, and no parameter will be modified.
  @retval EFI_INVALID_PARAMETER     All the parameters are NULL.

**/
EFI_STATUS
EFIAPI
GetBiosVersionDateTime (
  OUT CHAR16    *BiosVersion, OPTIONAL
  OUT CHAR16    *BiosReleaseDate, OPTIONAL
  OUT CHAR16    *BiosReleaseTime OPTIONAL
  )
{
  EFI_STATUS        Status;
  BIOS_ID_IMAGE     BiosIdImage;

  if ((BiosVersion == NULL) && (BiosReleaseDate == NULL) && (BiosReleaseTime == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBiosId (&BiosIdImage);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (BiosVersion != NULL) {
    //
    // Fill the BiosVersion data from the BIOS ID.
    //
    CopyMem (BiosVersion, &(BiosIdImage.BiosIdString), sizeof (BIOS_ID_STRING));
  }

  if (BiosReleaseDate != NULL) {
    //
    // Fill the build timestamp date from the BIOS ID in the "MM/DD/YY" format.
    //
    BiosReleaseDate[0] = BiosIdImage.BiosIdString.TimeStamp[2];
    BiosReleaseDate[1] = BiosIdImage.BiosIdString.TimeStamp[3];
    BiosReleaseDate[2] = (CHAR16) ((UINT8) ('/'));

    BiosReleaseDate[3] = BiosIdImage.BiosIdString.TimeStamp[4];
    BiosReleaseDate[4] = BiosIdImage.BiosIdString.TimeStamp[5];
    BiosReleaseDate[5] = (CHAR16) ((UINT8) ('/'));

    //
    // Add 20 for SMBIOS table
    // Current Linux kernel will misjudge 09 as year 0, so using 2009 for SMBIOS table
    //
    BiosReleaseDate[6] = '2';
    BiosReleaseDate[7] = '0';
    BiosReleaseDate[8] = BiosIdImage.BiosIdString.TimeStamp[0];
    BiosReleaseDate[9] = BiosIdImage.BiosIdString.TimeStamp[1];

    BiosReleaseDate[10] = (CHAR16) ((UINT8) ('\0'));
  }

  if (BiosReleaseTime != NULL) {

    //
    // Fill the build timestamp time from the BIOS ID in the "HH:MM" format.
    //
    BiosReleaseTime[0] = BiosIdImage.BiosIdString.TimeStamp[6];
    BiosReleaseTime[1] = BiosIdImage.BiosIdString.TimeStamp[7];
    BiosReleaseTime[2] = (CHAR16) ((UINT8) (':'));

    BiosReleaseTime[3] = BiosIdImage.BiosIdString.TimeStamp[8];
    BiosReleaseTime[4] = BiosIdImage.BiosIdString.TimeStamp[9];

    BiosReleaseTime[5] = (CHAR16) ((UINT8) ('\0'));
  }

  return  EFI_SUCCESS;
}

