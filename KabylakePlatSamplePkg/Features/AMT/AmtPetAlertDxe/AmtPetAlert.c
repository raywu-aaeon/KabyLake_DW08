/**@file
BAE Pet Alert Implementation

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2017 Intel Corporation.

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

#include "AmtPetAlert.h"
#include <Library/DxePlatformBootManagerLib.h>

//AMI_OVERRIDE_START - EIP283703 : AMT_015,report: Failed. BAE PET for Boot From Removable Device was not sent from BIOS. >>>
EFI_STATUS
FindDiskInfo(
    IN  EFI_HANDLE                  BootHandle,
    IN  EFI_DEVICE_PATH_PROTOCOL    *BootDp
    
)
{
    EFI_STATUS                  Status;
    EFI_DEVICE_PATH_PROTOCOL    *DpNode;
    SATA_DEVICE_PATH            *SataDpNode;
    UINTN                       HandleNum;
    EFI_HANDLE                  *HandleBuf;
    UINT32                      Index;
    EFI_DISK_INFO_PROTOCOL      *DiskInfo;
    UINT32                      Channel;
    UINT32                      Device;
    BOOLEAN                     Installed;

    //Get sata device path node
    for( DpNode = BootDp; !IsDevicePathEnd( DpNode ); DpNode = NextDevicePathNode( DpNode )  )
    {
        if( (DevicePathType( DpNode ) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType( DpNode ) == MSG_SATA_DP) )
            break;
    }

    if( IsDevicePathEnd( DpNode ) )
        return EFI_NOT_FOUND;

    SataDpNode = (SATA_DEVICE_PATH*)DpNode;

    Status = gBS->LocateHandleBuffer(
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleNum,
                    &HandleBuf );
    if( EFI_ERROR(Status) )
        return Status;

    //Search all disk info protocol
    for( Index = 0, Installed = FALSE; Index < HandleNum; Index++ )
    {
        Status = gBS->HandleProtocol(
                        HandleBuf[Index],
                        &gEfiDiskInfoProtocolGuid,
                        (VOID**)&DiskInfo );
        if( EFI_ERROR(Status) )
            continue;

        Status = DiskInfo->WhichIde(
                            DiskInfo,
                            &Channel,
                            &Device );
        if( EFI_ERROR(Status) )
            continue;

        //HBAPortNumber is port bit map, Channel from WhichIde() is port number
        if( ((UINT16)(1 << Channel) & SataDpNode->HBAPortNumber) != 0 )
        {
            gBS->InstallProtocolInterface(
                    &BootHandle,
                    &gEfiDiskInfoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    (VOID*)DiskInfo );
            Installed = TRUE;
            break;
        }
    }

    if( Installed == TRUE )
        Status = EFI_SUCCESS;
    else
        Status = EFI_NOT_FOUND;

    FreePool( HandleBuf );

    return Status;
}
//AMI_OVERRIDE_END - EIP283703 : AMT_015,report: Failed. BAE PET for Boot From Removable Device was not sent from BIOS. <<<

EFI_STATUS
EFIAPI
InitializeAmtPetAlert (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToBootEvent;

  //
  // Register Ready to Boot Event for AMT Wrapper
  //
  Status = EfiCreateEventReadyToBootEx (
            TPL_CALLBACK,
            AmtPetAlertReadyToBoot,
            (VOID *) &ImageHandle,
            &ReadyToBootEvent
            );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

VOID SwapEntries (
  IN CHAR8    *Data,
  IN UINT16   Size
  )
{
  UINT16  Index;
  CHAR8   Temp8;

  for (Index = 0; (Index+1) < Size; Index+=2) {
    Temp8           = Data[Index];
    Data[Index]     = Data[Index + 1];
    Data[Index + 1] = Temp8;
  }
}

// Example 1: 16 10 0F 6F 02 68 08 FF FF 22 00 40 13 XX XX XX
// Example 2: 15 10 0F 6F 02 68 10 FF FF 22 00 AA 13 03 03 02
BOOT_AUDIT_ENTRY mPetAlertWithoutChange = {
    0x16, 0x10, 0x0F, 0x6F, 0x02, 0x68, 0x08, 0xFF, 0xFF, 0x22, 0x00, 0x40, 0x13, 0x00, 0x00, 0x00
};

BOOT_AUDIT_ENTRY mPetAlertWithChange = {
    0x15, 0x10, 0x0F, 0x6F, 0x02, 0x68, 0x10, 0xFF, 0xFF, 0x22, 0x00, 0xAA, 0x13, 0x03, 0x03, 0x02
};

EFI_STATUS
SendBaeMessage (
  BOOT_AUDIT_ENTRY                *Buffer
  )
{
  HECI_PROTOCOL                   *Heci;
  UINT32                          HeciLength;
  BOOT_AUDIT_ENTRY_PACK           Pack;
  UINT32                          MeStatus;
  EFI_STATUS                      TempStatus;
  EFI_STATUS                      Status;

  CopyMem(&Pack.Data, Buffer, sizeof(BOOT_AUDIT_ENTRY));

  Pack.Command                = ASF_MESSAGE_COMMAND_MESSAGE;  // 0x04
  Pack.ByteCount              = sizeof(BOOT_AUDIT_ENTRY);
  HeciLength                  = sizeof(BOOT_AUDIT_ENTRY_PACK);    // length include Command & ByteCount

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (!EFI_ERROR(Status)) {
    TempStatus = Heci->GetMeStatus(&MeStatus);
    ASSERT_EFI_ERROR(TempStatus);

    //
    // Only send ASF Push Progress code when ME is ready.  Ignore FW Init Status.
    //
    if (ME_STATUS_ME_STATE_ONLY(MeStatus) == ME_READY) {
      Status = Heci->SendMsg(
                      HECI1_DEVICE,
                      (UINT32*)&Pack,
                      HeciLength,
                      BIOS_ASF_HOST_ADDR,
                      HECI_ASF_MESSAGE_ADDR
                      );
    }
  }

  return Status;
}

EFI_STATUS
PetAlert (
  UINT8                   DeviceType,
  CHAR8                   *HarddriveTag
  )
{
  PET_ALERT_CFG           PetAlertCfgVar;
  UINTN                   VarSize;
  UINTN                   UpdateFlag;
  EFI_STATUS              Status;
  BOOT_AUDIT_ENTRY        *PetAlertMsg;

  UpdateFlag  = 0;
  PetAlertMsg = &mPetAlertWithoutChange;

  if (HarddriveTag == NULL) {
    HarddriveTag = "";
  }

  VarSize = sizeof(PetAlertCfgVar);
  Status = gRT->GetVariable (
                  L"PetAlertCfg",
                  &gPlatformGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  &PetAlertCfgVar
                 );
  if (EFI_ERROR (Status)) {
    UpdateFlag = 1;
    PetAlertCfgVar.BootQueue[0] = DeviceType;
    PetAlertCfgVar.BootQueue[1] = DeviceType;
    PetAlertCfgVar.BootQueue[2] = DeviceType;
    AsciiStrCpyS ((CHAR8 *) PetAlertCfgVar.BootHarddriveTag, sizeof (PetAlertCfgVar.BootHarddriveTag) / sizeof (CHAR8), HarddriveTag);
  }

  DEBUG((DEBUG_INFO, "PetAlert HDD ModelSerialNumber = %a\n", PetAlertCfgVar.BootHarddriveTag));

  if ((DeviceType != BAE_HDD_DEVICE) ||
    ((DeviceType == BAE_HDD_DEVICE) && AsciiStrCmp (HarddriveTag, (CHAR8 *) PetAlertCfgVar.BootHarddriveTag))) {
    //
    // If booting from a network/removable device or from a different HDD send expanded BAE message
    //
    UpdateFlag                = 1;
    PetAlertMsg               = &mPetAlertWithChange;
    PetAlertMsg->EventData5   = PetAlertCfgVar.BootQueue[2] = PetAlertCfgVar.BootQueue[1];
    PetAlertMsg->EventData4   = PetAlertCfgVar.BootQueue[1] = PetAlertCfgVar.BootQueue[0];
    PetAlertMsg->EventData3   = PetAlertCfgVar.BootQueue[0] = DeviceType;
  }

  SendBaeMessage(PetAlertMsg);

  if (UpdateFlag) {
    AsciiStrCpyS ((CHAR8 *) PetAlertCfgVar.BootHarddriveTag, sizeof (PetAlertCfgVar.BootHarddriveTag) / sizeof (CHAR8), HarddriveTag);
    VarSize = sizeof(PetAlertCfgVar);
    Status = gRT->SetVariable (
                    L"PetAlertCfg",
                    &gPlatformGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    VarSize,
                    &PetAlertCfgVar
                    );
    ASSERT_EFI_ERROR(Status);
  }

  return Status;
}

/**
  Compare two device paths up to a size of Boot Opion's Device Path

  @param[in] BootOptionDp     Device path acquired from BootXXXX EFI variable
  @param[in] FileSysDp    Device path acquired through EFI_SIMPLE_FILE_SYSTEM_PROTOCOL Handles buffer

  @retval TRUE   Both device paths point to the same device
  @retval FALSE   Device paths point to different devices
**/
BOOLEAN
CompareDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL *BootOptionDp,
  IN  EFI_DEVICE_PATH_PROTOCOL *FileSysDp
  )
{
  UINTN BootOptionDpSize;
  UINTN FileSysDpSize;

  if (BootOptionDp == NULL || FileSysDp == NULL) {
    return FALSE;
  }

  BootOptionDpSize = GetDevicePathSize (BootOptionDp) - END_DEVICE_PATH_LENGTH;
  FileSysDpSize    = GetDevicePathSize (FileSysDp) - END_DEVICE_PATH_LENGTH;

  if ((BootOptionDpSize <= FileSysDpSize) && (CompareMem (FileSysDp, BootOptionDp, BootOptionDpSize) == 0)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Get EFI device path through EFI_SIMPLE_FILE_SYSTEM_PROTOCOL Handles buffer. Acquired path must
  point to the same device as argument DevicePath passed to the function.

  @param[in] DevicePath   Device path acquired from BootXXXX EFI variable

  @retval EFI_DEVICE_PATH_PROTOCOL   Device path for booting
**/
EFI_DEVICE_PATH_PROTOCOL *
GetFullBootDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_STATUS               Status;
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *ReturnDevicePath;
  UINTN                    HandleNum;
  EFI_HANDLE               *HandleBuf;
  UINTN                    Index;

  ReturnDevicePath = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &HandleNum,
                  &HandleBuf
                  );
  if ((EFI_ERROR (Status)) || (HandleBuf == NULL)) {
    return NULL;
  }

  for (Index = 0; Index < HandleNum; Index++) {
    TempDevicePath     = DevicePathFromHandle (HandleBuf[Index]);

    if (CompareDevicePaths(DevicePath, TempDevicePath)) {
      ReturnDevicePath = DuplicateDevicePath(TempDevicePath);
      break;
    }
  }

  return ReturnDevicePath;
}

UINT8
GetDeviceType (
  IN EFI_DEVICE_PATH_PROTOCOL *DevPath
  )
{
  UINT8                    DeviceType;
  BBS_BBS_DEVICE_PATH      *BbsDevicePath;

  DeviceType    = BAE_EMPTY_QUEUE;
  BbsDevicePath = (BBS_BBS_DEVICE_PATH *)DevPath;

  if (BbsDevicePath->Header.Type == BBS_DEVICE_PATH && BbsDevicePath->Header.SubType == BBS_BBS_DP) {
    //
    // Get device type from legacy-BBS style device path
    //
    switch (BbsDevicePath->DeviceType) {
      case BBS_TYPE_EMBEDDED_NETWORK:
        DeviceType = BAE_NETWORK_DEVICE;
        break;
      case BBS_TYPE_HARDDRIVE:
        DeviceType = BAE_HDD_DEVICE;
        break;
      case BBS_TYPE_CDROM:
      case BBS_TYPE_USB:
      case BBS_TYPE_FLOPPY:
        DeviceType = BAE_REMOVABLE_DEVICE;
        break;
      default:
        DeviceType = BAE_EMPTY_QUEUE;
        break;
    }
  } else {
    while (!IsDevicePathEnd(DevPath)) {
      if ((DevicePathType(DevPath) == MESSAGING_DEVICE_PATH) &&
                  (DevicePathSubType(DevPath) == MSG_USB_DP)){
        //
        // If device connected through USB, we automatically treat it as removable
        //
        DeviceType = BAE_REMOVABLE_DEVICE;
        break;
      } else if (DevicePathType(DevPath) == MEDIA_DEVICE_PATH) {
        if (DevicePathSubType(DevPath) == MEDIA_HARDDRIVE_DP) {
          DeviceType = BAE_HDD_DEVICE;
          break;
        } else if (DevicePathSubType(DevPath) == MEDIA_CDROM_DP) {
          DeviceType = BAE_REMOVABLE_DEVICE;
          break;
        } else if (DevicePathSubType(DevPath) == MEDIA_FILEPATH_DP) {
          DeviceType = BAE_NETWORK_DEVICE;
          break;
        }
      }

      DevPath = NextDevicePathNode(DevPath);
    }
  }

  return DeviceType;
}

VOID
Trim (
  CHAR8 *sDesc,
  CHAR8 *sSrc,
  UINT32 MaxLength
  )
{
  UINTN p1 ,p2;
  CHAR8 ch;
  UINTN i,l;

  p1 = 0xFF;
  p2 = 0xFF;
  for(i = 0; i < MaxLength; i++) {
    ch = *(sSrc+i);
    if (ch == 0) {
      break;
    } else if (ch != 0x20) {
      p2 = i;
      if (p1 == 0xFF) {
          p1 = i;
      }
    }
  }
  if ((p1 == 0xFF) || (p2 == 0xFF) ) {
    DEBUG ((DEBUG_WARN, "Buffer Overflow - Array Index Out of Bounds\n"));
    return;
  }

  l = (p2 - p1) + 1;
  if (l >= MaxLength) {
    DEBUG ((DEBUG_WARN, "Buffer Overflow - Array Index Out of Bounds\n"));
    return;
  }
  CopyMem(sDesc, sSrc+p1, l);
  *(sDesc+l) = 0;
}

/**
  Get Nvme device identify data.

  @param[in]   NvmeDevice        The pointer to the NVME_PASS_THRU_DEVICE data structure.
  @param[in]   NamespaceId       NamespaceId for an NVM Express namespace present on the NVM Express controller
  @param[in]   IdentifyStructure Specifies the information to be returned to host.
  @param[out]  Buffer            The buffer used to store the identify controller data.

  @return EFI_SUCCESS            Successfully get the identify controller data.
  @return EFI_DEVICE_ERROR       Fail to get the identify controller data.

**/

EFI_STATUS
NvmeIdentifyCommand (
  IN  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *NvmeDevice,
  IN  UINT32                                NamespaceId,
  IN  UINT32                                IdentifyStructure,
  OUT VOID                                  *Buffer
  )
{
  EFI_STATUS                                Status;
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                   Command;
  EFI_NVM_EXPRESS_COMPLETION                Completion;

  ZeroMem (&CommandPacket, sizeof (EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof (EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof (EFI_NVM_EXPRESS_COMPLETION));

  DEBUG ((DEBUG_INFO, "Sending Identify Command with Cns = %d\n", IdentifyStructure));

  Command.Cdw0.Opcode          = NVME_ADMIN_IDENTIFY_CMD;
  Command.Nsid                 = NamespaceId;
  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;
  CommandPacket.TransferBuffer = Buffer;
  CommandPacket.TransferLength = sizeof (NVME_ADMIN_CONTROLLER_DATA);
  CommandPacket.CommandTimeout = NVME_GENERIC_TIMEOUT;
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;
  //
  // Set bit 0 (Cns bit) to 0 to identify a namespace / 1 to identify a controller
  //
  Command.Cdw10                = IdentifyStructure;
  Command.Flags                = CDW10_VALID;

  Status = NvmeDevice->PassThru (NvmeDevice, NamespaceId, &CommandPacket, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "Identify Command Status=%r\n", Status));
  }

  return Status;
}

EFI_STATUS
GetModelSerialNumberFromNVMePassThru (
  IN EFI_HANDLE Handle,
  OUT CHAR8 *ModelSerialNumber,
  IN  UINTN ModelSerialNumberSize
  )
{
  EFI_STATUS                           Status;
  CHAR8                                ModelNumber[42];
  CHAR8                                SerialNumber[22];
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL   *NvmePassthruProtocol;
  UINT32                               NamespaceId;
  NVME_ADMIN_CONTROLLER_DATA           ControllerData;

  Status               = EFI_NOT_FOUND;
  DevicePath           = NULL;
  NvmePassthruProtocol = NULL;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateDevicePath (
                  &gEfiNvmExpressPassThruProtocolGuid,
                  &DevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiNvmExpressPassThruProtocolGuid,
                  (VOID**)&NvmePassthruProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NamespaceId = NVME_ALL_NAMESPACES;
  Status = NvmePassthruProtocol->GetNextNamespace (NvmePassthruProtocol, &NamespaceId);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Due to the fact that RAID Driver expects other value of NamespaceId parameter than AHCI driver,
  // need to send the Identify Controller command twice - once with the NamespaceId retrieved
  // using GetNextNamespace. If it fails we need to set NamespaceId = 0.
  ZeroMem (&ControllerData, sizeof (NVME_ADMIN_CONTROLLER_DATA));
  Status = NvmeIdentifyCommand (NvmePassthruProtocol, NamespaceId, NVME_IDENTIFY_CONTROLLER_STRUCT, (VOID *) &ControllerData);
  if (EFI_ERROR (Status)) {
    Status = NvmeIdentifyCommand (NvmePassthruProtocol, NVME_CONTROLLER_ID, NVME_IDENTIFY_CONTROLLER_STRUCT, (VOID *) &ControllerData);
     if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  CopyMem (SerialNumber, ControllerData.Sn, 20);
  SerialNumber[20] = '\0';
  Trim (SerialNumber, SerialNumber, sizeof (SerialNumber));

  CopyMem (ModelNumber, ControllerData.Mn, 40);
  ModelNumber[40] = '\0';
  Trim (ModelNumber, ModelNumber, sizeof (ModelNumber));

  AsciiStrCpyS (ModelSerialNumber, ModelSerialNumberSize, ModelNumber);
  AsciiStrCatS (ModelSerialNumber, ModelSerialNumberSize, "_");
  AsciiStrCatS (ModelSerialNumber, ModelSerialNumberSize, SerialNumber);

  return Status;
}

EFI_STATUS
GetModelSerialNumber (
  IN EFI_HANDLE Handle,
  OUT CHAR8 *ModelSerialNumber,
  IN  UINTN ModelSerialNumberSize
  )
{
  EFI_STATUS                      Status = EFI_NOT_FOUND;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  EFI_ATAPI_IDENTIFY_DATA         *IdentifyDriveInfo = NULL;
  UINT32                          BufferSize;
  CHAR8                           ModelNumber[42];
  CHAR8                           SerialNumber[22];
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath = NULL;

  AsciiStrCpyS (ModelSerialNumber, ModelSerialNumberSize, "");

  Status = GetModelSerialNumberFromNVMePassThru (Handle, ModelSerialNumber, ModelSerialNumberSize);
  
  if (!EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol(
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->LocateDevicePath (
                  &gEfiDiskInfoProtocolGuid,
                  &DevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
// AMI_OVERRIDE_START - EIP283703 : AMT_015,report: Failed. BAE PET for Boot From Removable Device was not sent from BIOS.
//Disk info won't be installed on bootxxx handle in iRST mode
//Find and install availiable disk info for following usage
    if (!EFI_ERROR(Status)) return Status;
    Status = FindDiskInfo( Handle, DevicePath );
    if( EFI_ERROR(Status) )
// AMI_OVERRIDE_END - EIP283703 : AMT_015,report: Failed. BAE PET for Boot From Removable Device was not sent from BIOS.
    return Status;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID **) &DiskInfo
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  IdentifyDriveInfo = AllocatePool(sizeof(EFI_ATAPI_IDENTIFY_DATA));
  if (IdentifyDriveInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SetMem(IdentifyDriveInfo, sizeof (EFI_ATAPI_IDENTIFY_DATA), 0);
  BufferSize = sizeof (EFI_ATAPI_IDENTIFY_DATA);
  Status = DiskInfo->Identify (
                      DiskInfo,
                      IdentifyDriveInfo,
                      &BufferSize
                      );
  if (EFI_ERROR(Status)) {
    FreePool(IdentifyDriveInfo);
    return Status;
  }

  CopyMem(SerialNumber, IdentifyDriveInfo->SerialNo, 20);
  SwapEntries (SerialNumber, 20);
  SerialNumber[20] = '\0';
  Trim (SerialNumber, SerialNumber, sizeof (SerialNumber));

  CopyMem(ModelNumber, IdentifyDriveInfo->ModelName, 40);
  SwapEntries (ModelNumber, 40);
  ModelNumber[40] = '\0';
  Trim (ModelNumber, ModelNumber, sizeof (ModelNumber));

  AsciiStrCpyS (ModelSerialNumber, ModelSerialNumberSize, ModelNumber);
  AsciiStrCatS (ModelSerialNumber, ModelSerialNumberSize, "_");
  AsciiStrCatS (ModelSerialNumber, ModelSerialNumberSize, SerialNumber);

  FreePool(IdentifyDriveInfo);

  return EFI_SUCCESS;
}

EFI_STATUS
GetModelSerialNumberFromBbs (
  IN  UINT8   DeviceType,
  OUT CHAR8   *ModelSerialNumber,
  IN  UINTN   ModelSerialNumberSize
  )
{
// AMI_OVERRIDE_START - Use Token to replace CSM_FLAG because we don't define it. >>
//#ifndef CSM_FLAG
//  return EFI_NOT_FOUND;
//#else
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
// AMI_OVERRIDE_END - Use Token to replace CSM_FLAG because we don't define it. <<
  EFI_LEGACY_BIOS_PROTOCOL        *LegacyBios;
  UINT16                          HddCount = 0;
  HDD_INFO                        *LocalHddInfo = NULL;
  UINT16                          BbsCount = 0;
  BBS_TABLE                       *BbsTable;
  EFI_STATUS                      Status;
  UINTN                           i;
  EFI_HANDLE                      Handle;
  UINTN                           DeviceIndex;
  UINTN                           DevicePriority;

  if (ModelSerialNumber == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = LegacyBios->GetBbsInfo (
                        LegacyBios,
                        &HddCount,
                        &LocalHddInfo,
                        &BbsCount,
                        &BbsTable
                        );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find matching device by boot priority
  //
  DeviceIndex = (UINTN)-1;
  DevicePriority = (UINTN)-1;
  for (i = 0; i < BbsCount; i++) {
    if (BbsTable[i].BootPriority != BBS_IGNORE_ENTRY) {
      if (BbsTable[i].DeviceType == DeviceType) {
        if (DevicePriority > BbsTable[i].BootPriority) {
          DevicePriority = BbsTable[i].BootPriority;
          DeviceIndex = i;
        }
      }
    }
  }

  //
  // Get model and serial number
  //
  if (DeviceIndex != (UINTN)-1) {
    Handle = *(VOID**)(&BbsTable[DeviceIndex].IBV1);
    GetModelSerialNumber(Handle, ModelSerialNumber, ModelSerialNumberSize);
  }

  return EFI_SUCCESS;
// AMI_OVERRIDE_START - Use Token to replace CSM_FLAG because we don't define it. >>
#else
  return EFI_NOT_FOUND;
// AMI_OVERRIDE_END - Use Token to replace CSM_FLAG because we don't define it. <<
#endif
}

EFI_STATUS
NotifyPetAlert (
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
  )
{
  EFI_STATUS                      Status;
  UINT16                          BootCurrent;
  UINTN                           VarSize;
  CHAR16                          BootXXXX[16];
  EFI_DEVICE_PATH_PROTOCOL        *BootDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;
  UINT8                           DeviceType;
  CHAR8                           ModelSerialNumber[64];
  EFI_HANDLE                      BootDeviceHandle;
  BBS_BBS_DEVICE_PATH *           LegacyBootDP;
  EFI_BOOT_MANAGER_LOAD_OPTION    BootOption;
  UINTN                           ModelSerialNumberSize;

  ModelSerialNumberSize = sizeof (ModelSerialNumber) / sizeof (CHAR8);
  AsciiStrCpyS (ModelSerialNumber, ModelSerialNumberSize, "");
  BootDeviceHandle            = 0;
  DeviceType                  = 0;
  BootDevicePath              = NULL;

  //
  // Get current boot option from NVRAM variables
  //
  VarSize = sizeof (UINT16);
  Status = gRT->GetVariable (
              L"BootCurrent",
              &gEfiGlobalVariableGuid,
              NULL,
              &VarSize,
              &BootCurrent
              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UnicodeSPrint(BootXXXX, sizeof(BootXXXX), L"Boot%04X", BootCurrent);

  Status = EfiBootManagerVariableToLoadOption (BootXXXX, &BootOption);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (((BBS_BBS_DEVICE_PATH *)BootOption.FilePath)->Header.Type == BBS_DEVICE_PATH) {
    BootDevicePath = DuplicateDevicePath(BootOption.FilePath);
  } else if (IsItPartitionsDevicePath (&BootOption)) {
    Status = ExpandPartitionsDevPath (&BootOption, &BootDevicePath);
   } else {
    //
    // For EFI boot get full device path from EFI_SIMPLE_FILE_SYSTEM
    //
    BootDevicePath = GetFullBootDevicePath(BootOption.FilePath);
  }

  EfiBootManagerFreeLoadOption (&BootOption);

  if (BootDevicePath == NULL) {
    return EFI_ABORTED;
  }

  DeviceType = GetDeviceType(BootDevicePath);

  //
  // Get model serial number if device is HDD
  //
  if (DeviceType == BAE_HDD_DEVICE) {
    LegacyBootDP = (BBS_BBS_DEVICE_PATH *)BootDevicePath;
    if (LegacyBootDP->Header.Type == BBS_DEVICE_PATH &&
      LegacyBootDP->Header.SubType == BBS_BBS_DP) {
      GetModelSerialNumberFromBbs(BBS_TYPE_HARDDRIVE, ModelSerialNumber, ModelSerialNumberSize);
    } else {
      TempDevicePath = BootDevicePath;
      Status = gBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &TempDevicePath,
                    &BootDeviceHandle
                    );
      if (EFI_ERROR(Status)) {
        FreePool(BootDevicePath);
        return Status;
      }

      GetModelSerialNumber(BootDeviceHandle, ModelSerialNumber, ModelSerialNumberSize);
    }
  }

  FreePool(BootDevicePath);

  Status = PetAlert(DeviceType, ModelSerialNumber);

  return Status;
}

VOID
EFIAPI
AmtPetAlertReadyToBoot (
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
  )
{
  if (GetManageabilityModeSetting() != MNT_OFF) {
    DEBUG((DEBUG_INFO, "<BAE_ReadyToBoot=TRUE>\n"));
    NotifyPetAlert(Event, ParentImageHandle);
  } else {
    DEBUG((DEBUG_INFO, "<BAE_ReadyToBoot=FALSE>\n"));
  }

  gBS->CloseEvent(Event);
}

