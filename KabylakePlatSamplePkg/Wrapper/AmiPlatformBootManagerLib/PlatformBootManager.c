//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//                                           
/** @file

Copyright (c) 2012 - 2013, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  BdsPlatform.c

Abstract:

  This file include all platform action which can be customized
  by IBV/OEM.

**/

//AMI_OVERRIDE_START >>>
#include <Uefi.h>

#include <Pi/PiStatusCode.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PrintLib.h>

#include <Protocol/LegacyBios.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/AtaPassThru.h>

extern EFI_GUID                        gEfiBlockIoProtocolGuid;

/**
  This function converts an input device structure to a Unicode string.

  @param DevPath                  A pointer to the device path structure.

  @return A new allocated Unicode string that represents the device path.
**/
CHAR16 *
DevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  CHAR16                           *ToText;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

  if (DevPath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathToText
                  );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }               
  ToText = DevPathToText->ConvertDevicePathToText (
                            DevPath,
                            FALSE,
                            TRUE
                            );
  ASSERT (ToText != NULL);
  return ToText;
}

/**
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

  @param  BlockIoDevicePath      Multi device path instances which need to check
  @param  HardDriveDevicePath    A device path which starts with a hard drive media
                                 device path.

  @retval TRUE                   There is a matched device path instance.
  @retval FALSE                  There is no matched device path instance.

**/
BOOLEAN
DevPathContainsPartion (
  IN  HARDDRIVE_DEVICE_PATH           *ParitionDevPath,
  IN  EFI_DEVICE_PATH_PROTOCOL        *FullDevPath
  )
{
  HARDDRIVE_DEVICE_PATH               *PartitionToCompare;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  BOOLEAN                              Match;

  DEBUG ((DEBUG_INFO , "Device Path Contains Partition\n"));

  if ((FullDevPath == NULL) || (ParitionDevPath == NULL)) {
    return FALSE;
  }

  //
  // find the partition device path node
  //
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL*)FullDevPath;
  PartitionToCompare = NULL;
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP)) {

      PartitionToCompare = (HARDDRIVE_DEVICE_PATH *) DevicePath;
      break;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (PartitionToCompare == NULL) {
    return FALSE;
  }

  DEBUG_CODE(
      CHAR16                          *PartitionToCompareStr;
      CHAR16                          *PartitionStr;
      PartitionToCompareStr = DevicePathToStr ((EFI_DEVICE_PATH_PROTOCOL*)PartitionToCompare);
      PartitionStr = DevicePathToStr ((EFI_DEVICE_PATH_PROTOCOL*)ParitionDevPath);
      DEBUG ((DEBUG_INFO, "\t\tpartition         : \"%s\"\n\t\tpartitionToCompare: \"%s\"\n", PartitionStr, PartitionToCompareStr));
      FreePool (PartitionStr);
      FreePool (PartitionToCompareStr);
  );

  //
  // Check for the match
  //
  Match = FALSE;

  if ((PartitionToCompare->MBRType == ParitionDevPath->MBRType) &&
      (PartitionToCompare->SignatureType == ParitionDevPath->SignatureType)) {

    switch (PartitionToCompare->SignatureType) {
    case SIGNATURE_TYPE_GUID:
      Match = CompareGuid (
                (EFI_GUID *)PartitionToCompare->Signature,
                (EFI_GUID *)ParitionDevPath->Signature
              );

      DEBUG ((
          DEBUG_INFO, "GUID Signature\n\t\tpartition          %016X%016X\n"
              "\t\tpartitionToCompare: %016X%016X\n",
          (UINT64*) ((VOID*)&ParitionDevPath->Signature[0]),
          (UINT64*) ((VOID*)&ParitionDevPath->Signature[1]),
          (UINT64*) ((VOID*)&PartitionToCompare->Signature[0]),
          (UINT64*) ((VOID*)&PartitionToCompare->Signature[1])
      ));
      break;

    case SIGNATURE_TYPE_MBR:
      Match = (
                (BOOLEAN)(*((UINT32 *)(&(PartitionToCompare->Signature[0]))) ==
                ReadUnaligned32((UINT32 *)(&(ParitionDevPath->Signature[0]))))
              );

      DEBUG ((
          DEBUG_INFO,
          "MBR signature type:\n\tpartition          %08X\n\tpartitionToCompare %08X\n",
          ReadUnaligned32((UINT32 *)(&(ParitionDevPath->Signature[0]))),
          *((UINT32 *)(&(PartitionToCompare->Signature[0])))
      ));
      break;

    default:
      Match = FALSE;
      break;
    }
  }

  if (Match) {
    DEBUG ((DEBUG_INFO, "Partition is contained inside dev path.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "Partition is not contained inside dev path.\n"));
  }

  return Match;
}

/**
  Tries to expand the given partitions device path to start with physical HDD
  device path
  @param[in]       LoadOption          pointer to an Efi Boot Manager boot option
  @param[out]      BootDevicePath      indirect pointer to retrieved device path

  @retval          EFI_SUCCESS         Function found
**/
EFI_STATUS
ExpandPartitionsDevPath (
  IN EFI_BOOT_MANAGER_LOAD_OPTION     *LoadOption,
  OUT EFI_DEVICE_PATH_PROTOCOL       **BootDevicePath
  )
{
  EFI_STATUS                           Status;
  UINTN                                BlockIoHandleCount;
  EFI_HANDLE                          *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL            *PartitionDevPath;
  EFI_DEVICE_PATH                     *BlockIoDevicePath;
  UINT32                               i;
  DEBUG ((DEBUG_INFO, "Expand partitions device path\n"));

  *BootDevicePath = NULL;
  PartitionDevPath = LoadOption->FilePath;

  DEBUG_CODE (
    CHAR16                            *DevPathStr;
    DevPathStr = DevicePathToStr(PartitionDevPath);
    DEBUG ((DEBUG_INFO, "Partition dev path : %s\n", DevPathStr));
    FreePool (DevPathStr);
  );

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0 || BlockIoBuffer == NULL) {
    return EFI_NOT_FOUND;
  }

  for (i=0; i < BlockIoHandleCount ; i++) {

    Status = gBS->HandleProtocol (BlockIoBuffer[i], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }

    DEBUG_CODE (
      CHAR16                          *DevPathStr;
      DevPathStr = DevicePathToStr(BlockIoDevicePath);
      DEBUG ((DEBUG_INFO, "Checking dev path : %s\n", DevPathStr));
      FreePool (DevPathStr);
    );

    if (DevPathContainsPartion((HARDDRIVE_DEVICE_PATH*) PartitionDevPath, BlockIoDevicePath)) {
      *BootDevicePath = DuplicateDevicePath(BlockIoDevicePath);
      break;
    }
  }

  if (*BootDevicePath == NULL) {
    Status = EFI_NOT_FOUND;
  }
  if (BlockIoBuffer != NULL) {
    FreePool (BlockIoBuffer);
  }
  return Status;
}

/**
  Checks whenever a given load option starts with a partition device path

  @param[in]       LoadOption          Pointer to load option to test

  @retval          TRUE
  @retval          FALSE
**/
BOOLEAN
IsItPartitionsDevicePath(
    IN EFI_BOOT_MANAGER_LOAD_OPTION     *LoadOption
  )
{
  return  DevicePathType (LoadOption->FilePath) == MEDIA_DEVICE_PATH &&
          (DevicePathSubType (LoadOption->FilePath) == MEDIA_HARDDRIVE_DP);
}
//AMI_OVERRIDE_END <<<

/**
  Do the platform specific action before the console is ready
  Possible things that can be done in PlatformBootManagerBeforeConsole:
  > Update console variable
  > Register new Driver#### or Boot####
  > Signal ReadyToLock event
  > Authentication action: 1. connect Auth devices; 2. Identify auto logon user.

**/
VOID
EFIAPI
PlatformBootManagerBeforeConsole (
  VOID
  )
{
  return;
}

/**
  Do the platform specific action after the console is ready
  Possible things that can be done in PlatformBootManagerAfterConsole:
  > Dynamically switch output mode
  > Signal console ready platform customized event
  > Run diagnostics: 1. driver health check; 2. memory test
  > Connect certain devices: 1. connect all; 2. connect nothing; 3. connect platform specified devices
  > Dispatch aditional option roms
**/
VOID
EFIAPI
PlatformBootManagerAfterConsole (
  VOID
  )
{
  return;
}

/**
  This function is called each second during the boot manager waits the timeout.

  @param TimeoutRemain  The remaining timeout.
**/
VOID
EFIAPI
PlatformBootManagerWaitCallback (
  UINT16          TimeoutRemain
  )
{
  return;
}
