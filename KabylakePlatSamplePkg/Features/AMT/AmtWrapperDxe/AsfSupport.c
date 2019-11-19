/** @file
  Support routines for ASF boot options in the BDS

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2005 - 2016 Intel Corporation.

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

#include <Library/UefiBootManagerLib.h>
#include "AmtWrapperDxe.h"
#include "AsfSupport.h"
#include <Library/DebugLib.h>
//AMI_OVERRIDE_START - AMI IDER boot feature override
#include <AmiDxeLib.h>
#include <Library/GenericBdsLib.h>
#include <Guid/LegacyDevOrder.h>
//AMI_OVERRIDE_END - AMI IDER boot feature override	

//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED ASF_BOOT_OPTIONS                *mAsfBootOptions;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID                        gAsfRestoreBootSettingsGuid = RESTORE_SECURE_BOOT_GUID;

GLOBAL_REMOVE_IF_UNREFERENCED STORAGE_REDIRECTION_DEVICE_PATH mUsbrDevicePath = {
  gPciRootBridge,
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_PCI_DP,
      {
        (UINT8)(sizeof(PCI_DEVICE_PATH)),
        (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)
      }
    },
    PCI_FUNCTION_NUMBER_PCH_XHCI,
    PCI_DEVICE_NUMBER_PCH_XHCI
  },
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_USB_DP,
      {
        (UINT8)(sizeof(USB_DEVICE_PATH)),
        (UINT8)((sizeof(USB_DEVICE_PATH)) >> 8)
      }
    },
    V_ME_PCH_LP_USBR2_PORT_NUMBER,
    0x0
  },
  gEndEntire
};
//AMI_OVERRIDE_START - AMI IDER boot feature override
static EFI_DEVICE_PATH_PROTOCOL EndDevicePath[] = {
  END_DEVICE_PATH_TYPE,
  END_ENTIRE_DEVICE_PATH_SUBTYPE,
  END_DEVICE_PATH_LENGTH,
  0
};

//
// Legacy Device Order
//
typedef struct {
  UINT32  Type;
  UINT16  Length;
  UINT16  Device[1];
} LEGACY_DEVICE_ORDER;
//AMI_OVERRIDE_END - AMI IDER boot feature override

/**
  Retrieve the ASF boot options previously recorded by the ASF driver.

  @param[in] None.

  @retval EFI_SUCCESS        Initialized Boot Options global variable and AMT protocol
**/
EFI_STATUS
BdsAsfInitialization (
  IN  VOID
  )
{
  EFI_STATUS                          Status;
  ALERT_STANDARD_FORMAT_PROTOCOL      *Asf;

  mAsfBootOptions = NULL;

  //
  // Amt Library Init
  //
  Status = AmtLibInit ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Info : Error init AmtLibInit -> %r\n", Status));
    return Status;
  }
  //
  // Get Protocol for ASF
  //
  Status = gBS->LocateProtocol (
                  &gAlertStandardFormatProtocolGuid,
                  NULL,
                  (VOID **) &Asf
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Info : Error getting ASF protocol -> %r\n", Status));
    return Status;
  }

  Status = Asf->GetBootOptions (Asf, &mAsfBootOptions);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Info : Error getting ASF BootOptions -> %r\n", Status));
    return Status;
  }

  Status = ManageSecureBootState();

  return Status;
}

/**
  This routine makes necessary Secure Boot & CSM state changes for Storage Redirection boot

  @param[in] None.

  @retval EFI_SUCCESS      Changes applied succesfully
**/
EFI_STATUS
ManageSecureBootState(
  IN VOID
  )
{
  EFI_STATUS Status;
  BOOLEAN    EnforceSecureBoot;
  UINT8      SecureBootState;
  UINT8      UsbrBoot;
  UINTN      VarSize;
  UINT32     VarAttributes;
  UINT8      RestoreBootSettings;

  VarSize = sizeof(UINT8);

  //
  // Get boot parameters (Storage Redirection boot?, EnforceSecureBoot flag set?, secure boot enabled?)
  //
  EnforceSecureBoot = ActiveManagementIsEnforceSecureBootEnabled();
  UsbrBoot = ActiveManagementIsStorageRedirectionEnabled();

  Status = gRT->GetVariable (
                  L"SecureBootEnable",
                  &gEfiSecureBootEnableDisableGuid,
                  &VarAttributes,
                  &VarSize,
                  &SecureBootState
                  );
  if (EFI_ERROR(Status)) {
    return EFI_SUCCESS;
  }

  //
  // Check whether we need to restore SecureBootEnable value changed in previous Storage Redirection boot
  //
  Status = gRT->GetVariable(
                  L"RestoreBootSettings",
                  &gAsfRestoreBootSettingsGuid,
                  NULL,
                  &VarSize,
                  &RestoreBootSettings
                  );

  if (Status == EFI_SUCCESS && RestoreBootSettings != RESTORE_SECURE_BOOT_NONE) {
    if (RestoreBootSettings == RESTORE_SECURE_BOOT_ENABLED && SecureBootState == SECURE_BOOT_DISABLED &&
      !(UsbrBoot && !EnforceSecureBoot)) {

      SecureBootState = SECURE_BOOT_ENABLED;
      Status = gRT->SetVariable(
                      L"SecureBootEnable",
                      &gEfiSecureBootEnableDisableGuid,
                      VarAttributes,
                      VarSize,
                      &SecureBootState
                      );
      ASSERT_EFI_ERROR (Status);

      //
      // Delete RestoreBootSettings variable
      //
      Status = gRT->SetVariable(
                      L"RestoreBootSettings",
                      &gAsfRestoreBootSettingsGuid,
                      0,
                      0,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);

      DEBUG ((DEBUG_INFO, "Secure Boot settings restored after Storage Redirection boot - Cold Reset!\n"));
      gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
      CpuDeadLoop ();
    }
  }

  Status = EFI_SUCCESS;

  if (UsbrBoot) {
    if (SecureBootState == SECURE_BOOT_ENABLED && !EnforceSecureBoot) {
      //
      // Secure boot needs to be disabled if we're doing Storage Redirection and EnforceSecureBoot not set
      //
      SecureBootState     = SECURE_BOOT_DISABLED;
      RestoreBootSettings = RESTORE_SECURE_BOOT_ENABLED;

      Status = gRT->SetVariable(
                      L"SecureBootEnable",
                      &gEfiSecureBootEnableDisableGuid,
                      VarAttributes,
                      sizeof(UINT8),
                      &SecureBootState
                      );
      ASSERT_EFI_ERROR (Status);

      //
      // Set variable to restore previous secure boot state
      //
      Status = gRT->SetVariable(
                      L"RestoreBootSettings",
                      &gAsfRestoreBootSettingsGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                      sizeof(UINT8),
                      &RestoreBootSettings
                      );
      ASSERT_EFI_ERROR (Status);

      DEBUG ((DEBUG_INFO, "Secure Boot disabled for USBr boot - Cold Reset!\n"));
      gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
      CpuDeadLoop ();
    }
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
CompareDevicePaths(
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
GetFullBootDevicePath(
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

/*++
  Translate ASF request type to BBS or EFI device path type

  @param[in] DeviceType     - ASF request type
  @param[in]  Efi           - Set to TRUE if DeviceType is to be translated
                              to EFI device path type; FALSE if BBS type
  @retval UINTN Translated device type
--*/
UINTN
GetBootDeviceType (
  IN UINTN    DeviceType,
  IN BOOLEAN  Efi
  )
{
  UINTN Type = 0;

  switch (DeviceType) {
    case FORCE_PXE:
      if (Efi) {
        Type = MSG_MAC_ADDR_DP;
      } else {
        Type = BBS_EMBED_NETWORK;
      }
      break;
    case FORCE_HARDDRIVE:
    case FORCE_SAFEMODE:
      if (Efi) {
        Type = MEDIA_HARDDRIVE_DP;
      } else {
        Type = BBS_TYPE_HARDDRIVE;
      }
      break;
    case FORCE_DIAGNOSTICS:
      if (Efi) {
        Type = MEDIA_PIWG_FW_FILE_DP;
      }
      break;
    case FORCE_CDDVD:
      if (Efi) {
        Type = MEDIA_CDROM_DP;
      } else {
        Type = BBS_TYPE_CDROM;
      }
      break;
    default:
      break;
  }

  return Type;
}

/**
  Update the BBS table with our required boot device

  @param[in] DeviceIndex   Boot device whose device index
  @param[in] DevType     Boot device whose device type
  @param[in] BbsCount    Number of BBS_TABLE structures
  @param[in] BbsTable    BBS entry
  @param[in] UsbrBoot    set to TRUE if this is USBR boot

  @retval EFI_SUCCESS   BBS table successfully updated
**/
EFI_STATUS
ProcessBbsTable (
  IN UINT16     BbsIndex,
  OUT BOOLEAN   *IsUsbr
  )
{
#ifdef CSM_FLAG
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  BBS_TABLE                 *BbsTable;
  UINT16                    HddCount;
  UINT16                    BbsCount;
  HDD_INFO                  *LocalHddInfo;
  EFI_STATUS                Status;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_HANDLE                UsbrHandle;

  HddCount      = 0;
  BbsCount      = 0;
  LocalHddInfo  = NULL;

  //
  // Make sure the Legacy Boot Protocol is available
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (LegacyBios == NULL) {
    return Status;
  }

  //
  // Get BBS table instance
  //
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

  //DEBUG_CODE(PrintBbsTable (BbsTable, BbsCount););

  if (BbsIndex > BbsCount) {
    return EFI_ABORTED;
  }

  //
  // Check if called only to determine whether this is Storage Redirection legacy option
  //
  if (IsUsbr != NULL) {
    UsbrHandle     = (EFI_HANDLE)((UINTN)BbsTable[BbsIndex].IBV1 | ((UINTN)BbsTable[BbsIndex].IBV2 << 32));
    TempDevicePath = DevicePathFromHandle (UsbrHandle);
    *IsUsbr = CompareDevicePaths((EFI_DEVICE_PATH_PROTOCOL*)&mUsbrDevicePath, TempDevicePath);

    return EFI_SUCCESS;
  }

  //
  // First set all disks priority to BBS_DO_NOT_BOOT_FROM
  //
  for (Index = 0; Index < BbsCount; Index++) {
    if (BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
      continue;
    }
    BbsTable[Index].BootPriority = BBS_DO_NOT_BOOT_FROM;
  }

  BbsTable[BbsIndex].BootPriority = 0;

  // DEBUG_CODE(PrintBbsTable (BbsTable, BbsCount););

  return Status;
#else
  return EFI_NOT_FOUND;
#endif
}

/**
  Build the Device Path for this boot selection

  @param[in] BootOptions   Boot Option data pointer
  @param[in] OptionCount   Boot Option count
  @param[in] DeviceType    Boot device whose device type
  @param[in] DeviceIndex   Boot device whose device index
  @param[in] UsbrBoot      If UsbrBoot is TRUE then check USBr device
  @param[in] EfiBoot       Set to TRUE if this is EFI boot

  @retval BootDeviceIndex  Index of Boot Device
**/
UINTN
GetBootOptionIndex (
  IN EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,
  IN UINTN                        OptionCount,
  IN UINT16                       DeviceType,
  IN UINT16                       DeviceIndex,
  IN BOOLEAN                      UsbrBoot,
  IN BOOLEAN                      EfiBoot
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *FullDevicePath;
  UINTN                        Index;
  UINTN                        TempIndex;
  EFI_DEVICE_PATH_PROTOCOL     *DevPathNode;
  UINTN                        EfiDeviceType;
  UINTN                        LegacyDeviceType;
  BOOLEAN                      TypeMatched;
  INTN                         BootDeviceIndex;
  UINTN                        EfiNodeType;

  FullDevicePath   = NULL;
  TempIndex        = 1;
  EfiDeviceType    = GetBootDeviceType(DeviceType, TRUE);
  LegacyDeviceType = GetBootDeviceType(DeviceType, FALSE);
  TypeMatched      = FALSE;
  BootDeviceIndex  = -1;
  EfiNodeType      = (EfiDeviceType == MSG_MAC_ADDR_DP) ? MESSAGING_DEVICE_PATH : MEDIA_DEVICE_PATH;

  mUsbrDevicePath.Usbr.InterfaceNumber =
            (mAsfBootOptions->SpecialCommandParam >> USBR_BOOT_DEVICE_SHIFT) & USBR_BOOT_DEVICE_MASK;
  if (GetPchSeries () == PchH) {
    mUsbrDevicePath.Usbr.ParentPortNumber = V_ME_PCH_H_USBR2_PORT_NUMBER;
  }

  for (Index = 0; Index < OptionCount; Index++) {
    if (DevicePathType(BootOptions[Index].FilePath) == BBS_DEVICE_PATH && DevicePathSubType(BootOptions[Index].FilePath) == BBS_BBS_DP) {
      FullDevicePath = DuplicateDevicePath(BootOptions[Index].FilePath);
    } else {
      //
      // If this is EFI boot option, we need to get full device path from EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
      // to determine type of device
      //
//AMI_OVERRIDE_START - AMI IDER boot feature override
      if (!UsbrBoot) {
	    FullDevicePath = DuplicateDevicePath(BootOptions[Index].FilePath);
      }
      else {
      FullDevicePath = GetFullBootDevicePath(BootOptions[Index].FilePath);
      }
//AMI_OVERRIDE_END - AMI IDER boot feature override
      if (FullDevicePath == NULL) {
        continue;
      }
    }

    DevPathNode = FullDevicePath;
    if (DevPathNode == NULL) {
      continue;
    }

    //
    // Check if this is our requested boot device
    //
    while (!IsDevicePathEnd(DevPathNode)) {
      if (UsbrBoot) {
        if (EfiBoot) {
          if (CompareDevicePaths((EFI_DEVICE_PATH_PROTOCOL*)&mUsbrDevicePath, FullDevicePath)) {
            TypeMatched = TRUE;
          }
        } else {
          //
          // USBR legacy boot, lookup BBS table to check if this is Storage Redirection boot option
          //
          ProcessBbsTable((UINT16)*BootOptions[Index].OptionalData, &TypeMatched);
        }
      } else {
#if 0  //AMI_OVERRIDE_START - AMI IDER boot feature override
        if (DevicePathType(DevPathNode) == BBS_DEVICE_PATH && DevicePathSubType(DevPathNode) == BBS_BBS_DP) {
          //
          // Legacy boot option
          //
          if (((BBS_BBS_DEVICE_PATH *)DevPathNode)->DeviceType == LegacyDeviceType) {
            TypeMatched = TRUE;
          }
        } else {
#endif  //AMI_OVERRIDE_END - AMI IDER boot feature override
          //
          // EFI boot option
          //
//AMI_OVERRIDE_START - AMI IDER boot feature override
	      //Skip USB
          if((DevicePathType(DevPathNode) == MESSAGING_DEVICE_PATH) &&
   		     (DevicePathSubType(DevPathNode) == MSG_USB_DP))
          {
//AMI_OVERRIDE_START - EIP283703 : AMT_015,report: Failed. BAE PET for Boot From Removable Device was not sent from BIOS.
              EFI_DEVICE_PATH_PROTOCOL    *NextNode;
              NextNode = NextDevicePathNode( DevPathNode );
              if( (DevicePathType( NextNode ) == MEDIA_DEVICE_PATH) &&
                  (DevicePathSubType( NextNode ) == MEDIA_HARDDRIVE_DP) )
              {
                  break;
              }
//AMI_OVERRIDE_END - EIP283703 : AMT_015,report: Failed. BAE PET for Boot From Removable Device was not sent from BIOS.
          }  
//AMI_OVERRIDE_END - AMI IDER boot feature override			  	      
          if (DevicePathType(DevPathNode) == EfiNodeType && DevicePathSubType(DevPathNode) == EfiDeviceType) {
            if (DeviceType == FORCE_DIAGNOSTICS) {
              //
              // If boot to EFI shell, find shell file by GUID
              //
              if (CompareMem(&((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)DevPathNode)->FvFileName, PcdGetPtr(PcdShellFile), sizeof(EFI_GUID))) {
                TypeMatched = TRUE;
              }
            } else {
              TypeMatched = TRUE;
            }
          }
//AMI_OVERRIDE_START - AMI IDER boot feature override		  
        //}
//AMI_OVERRIDE_END - AMI IDER boot feature override		
      }

      if (TypeMatched) {
        //
        // Type matched, check for device index
        //
        if (!UsbrBoot && TempIndex < DeviceIndex) {
          TempIndex++;
          TypeMatched = FALSE;
          break;
        }

        BootDeviceIndex = Index;
        //
        // Refresh BBS table if legacy option
        //
        if (DevicePathType(FullDevicePath) == BBS_DEVICE_PATH && DevicePathSubType(FullDevicePath) == BBS_BBS_DP) {
          ProcessBbsTable((UINT16)*BootOptions[Index].OptionalData, NULL);
        }
        break;
      }

      DevPathNode = NextDevicePathNode(DevPathNode);
    }

    if (FullDevicePath != NULL) {
      FreePool(FullDevicePath);
      FullDevicePath = NULL;
    }

    if (BootDeviceIndex != -1) {
      break;
    }
  }

  return BootDeviceIndex;
}

/**
  Found out ASF boot options.

  @param[in] EfiBoot      Set to TRUE if this is EFI boot

  @retval EFI_DEVICE_PATH_PROTOCOL     Device path for booting.
**/
INTN
GetAsfBootOptionIndex (
  IN EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,
  IN UINT8                        DeviceType,
  IN UINTN                        OptionCount,
  IN BOOLEAN                      EfiBoot
  )
{
  INTN BootOptionIndex = -1;

  //
  // First we check ASF boot options Special Command
  //
  switch (DeviceType) {
  //
  // The Special Command Parameter can be used to specify a PXE
  // parameter. When the parameter value is 0, the system default PXE device is booted. All
  // other values for the PXE parameter are reserved for future definition by this specification.
  //
  case FORCE_PXE:
  case FORCE_DIAGNOSTICS:
    if (mAsfBootOptions->SpecialCommandParam != 0) {
      //
      // ASF spec says 0 currently is the only option
      //
      break;
    }

    //
    // We want everything connected up for PXE or EFI shell
    //
    EfiBootManagerConnectAll ();
    BootOptionIndex = GetBootOptionIndex(BootOptions, OptionCount, DeviceType, mAsfBootOptions->SpecialCommandParam, FALSE, EfiBoot);
    break;

  //
  // The Special Command Parameter identifies the boot-media index for
  // the managed client. When the parameter value is 0, the default hard-drive/optical drive is booted, when the
  // parameter value is 1, the primary hard-drive/optical drive is booted; when the value is 2, the secondary
  // hard-drive/optical drive is booted and so on.
  //
  case FORCE_HARDDRIVE:
  case FORCE_SAFEMODE:
  case FORCE_CDDVD:
    BootOptionIndex = GetBootOptionIndex(BootOptions, OptionCount, DeviceType, mAsfBootOptions->SpecialCommandParam, FALSE, EfiBoot);
    break;

  //
  // No additional special command is included; the Special Command Parameter has no
  // meaning.
  //
  case NOP:
  default:
    break;
  }

  return BootOptionIndex;
}

/**
  Check Storage Redirection boot device and Asf boot device

  @param[in] EfiBoot      Set to TRUE if this is EFI boot

  @retval EFI_DEVICE_PATH_PROTOCOL     Device path for booting.
**/
INTN
GetForcedBootOptionIndex (
  IN EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,
  IN UINTN                        OptionCount,
  IN BOOLEAN                      EfiBoot
  )
{
  INTN BootOptionIndex = -1;

  //
  // OEM command values; the interpretation of the Special Command and associated Special
  // Command Parameters is defined by the entity associated with the Enterprise ID.
  //
  if (ActiveManagementIsStorageRedirectionEnabled ()) {
    BootOptionIndex = GetBootOptionIndex (
                    BootOptions,
                    OptionCount,
                    FORCE_CDDVD,
                    0,
                    TRUE,
                    EfiBoot
                    );
  } else if (mAsfBootOptions->IanaId != ASF_INDUSTRY_CONVERTED_IANA) {
    BootOptionIndex = GetAsfBootOptionIndex (
                          BootOptions,
                          mAsfBootOptions->SpecialCommand,
                          OptionCount,
                          EfiBoot
                          );
  }

  return BootOptionIndex;
}

//AMI_OVERRIDE_START - AMI IDER boot feature override
UINTN
GetFirstIndexByType (
  UINT16 DevType
  )
/*++

Routine Description:

  Boot HDD by BIOS Default Priority

Arguments:

  DevType - Boot device whose device type

Returns:

  None

--*/
{
  EFI_STATUS          Status;
  UINTN               LegacyDevOrderSize;
  LEGACY_DEVICE_ORDER *LegacyDevOrder;
  UINTN               Index;
  UINT8               *p;
  UINTN               o;

  Index               = 0;
  o                   = 0;

  LegacyDevOrderSize  = 0;
  LegacyDevOrder      = NULL;

  Status = gRT->GetVariable (
                  L"LegacyDevOrder",
                  &gEfiLegacyDevOrderVariableGuid,
                  NULL,
                  &LegacyDevOrderSize,
                  NULL
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    LegacyDevOrder = AllocateZeroPool (LegacyDevOrderSize);
    if (LegacyDevOrder != NULL) {
      Status = gRT->GetVariable (
                      L"LegacyDevOrder",
                      &gEfiLegacyDevOrderVariableGuid,
                      NULL,
                      &LegacyDevOrderSize,
                      LegacyDevOrder
                      );
      if (!EFI_ERROR (Status)) {
        p = (UINT8 *) LegacyDevOrder;
        o = 0;
        for (o = 0; o < LegacyDevOrderSize; o += sizeof (LegacyDevOrder->Type) + LegacyDevOrder->Length) {
          LegacyDevOrder = (LEGACY_DEVICE_ORDER *) (p + o);
          if (LegacyDevOrder->Type == DevType) {
            Index = LegacyDevOrder->Device[0];
          }
        }
      }
    }
  }

  return Index;
}

static
EFI_STATUS
RefreshBbsTableForBoot (
  IN     UINT16        DeviceIndex,
  IN     UINT16        DevType,
  IN     UINT16        BbsCount,
  IN OUT BBS_TABLE     *BbsTable
  )
/*++

Routine Description:
  
  Update the table with our required boot device

Arguments:

  DeviceIndex - Boot device whose device index
  DevType     - Boot device whose device type
  BbsCount    - Number of BBS_TABLE structures
  BbsTable    - BBS entry
  
Returns:

  EFI_STATUS
  
--*/
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINT16      TempIndex;

  Status          = EFI_NOT_FOUND;
  TempIndex       = 1;


  //
  // Find the first present boot device whose device type
  // matches the DevType, we use it to boot first. This is different
  // from the other Bbs table refresh since we are looking for the device type
  // index instead of the first device to match the device type.
  //
  // And set other present boot devices' priority to BBS_UNPRIORITIZED_ENTRY
  // their priority will be set by LegacyBiosPlatform protocol by default
  //
  if (DeviceIndex > 0) {
    for (Index = 0; Index < BbsCount; Index++) {
      if (BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
        continue;
      }

      BbsTable[Index].BootPriority = BBS_DO_NOT_BOOT_FROM;

      if (BbsTable[Index].DeviceType == DevType) {
        if (TempIndex++ == DeviceIndex) {
          BbsTable[Index].BootPriority  = 0;
          Status                        = EFI_SUCCESS;
          continue;
        }
      }
    }
  } else {
    //
    // Boot HDD by BIOS Default Priority
    //
    Index                         = GetFirstIndexByType (DevType);
    BbsTable[Index].BootPriority  = 0;
    Status                        = EFI_SUCCESS;
  }

  return Status;
}

static
EFI_STATUS
RefreshBbsTableForUSBrBoot (
  IN EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,
  IN UINTN                        OptionCount,
  IN     UINT16        DeviceIndex,
  IN     UINT16        DevType,
  IN     UINT16        BbsCount,
  IN OUT BBS_TABLE     *BbsTable
  )
/*++

Routine Description:
  
  Update the table with IdeR boot device

Arguments:

  DeviceIndex - Boot device whose device index
  DevType     - Boot device whose device type
  BbsCount    - Number of BBS_TABLE structures
  BbsTable    - BBS entry
  
Returns:

  EFI_STATUS
  
--*/
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINTN	      OptionIndex;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_HANDLE                UsbrHandle;

  Status          = EFI_NOT_FOUND;

  
  mUsbrDevicePath.Usbr.InterfaceNumber = 
            (mAsfBootOptions->SpecialCommandParam >> USBR_BOOT_DEVICE_SHIFT) & USBR_BOOT_DEVICE_MASK;
  if (GetPchSeries () == PchH) {
    mUsbrDevicePath.Usbr.ParentPortNumber = V_ME_PCH_H_USBR2_PORT_NUMBER;
  }
		   
  //
  // Find the first present boot device whose device type
  // matches the DevType, we use it to boot first.
  //
  // And set other present boot devices' priority to BBS_DO_NOT_BOOT_FROM
  // their priority will be set by LegacyBiosPlatform protocol by default
  //
  
  for (Index = 0; Index < BbsCount; Index++) {
    if (BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
      continue;
    }
    BbsTable[Index].BootPriority = BBS_DO_NOT_BOOT_FROM;
  }
  
  for (OptionIndex = 0; OptionIndex < OptionCount; OptionIndex++) {  
        if (DevicePathType(BootOptions[OptionIndex].FilePath) == BBS_DEVICE_PATH && DevicePathSubType(BootOptions[OptionIndex].FilePath) == BBS_BBS_DP) {
    	    for (Index = 0; Index < BbsCount; Index++) {
  		    UsbrHandle     = (EFI_HANDLE)((UINTN)BbsTable[Index].IBV1 | ((UINTN)BbsTable[Index].IBV2 << 32));
  		    TempDevicePath = DevicePathFromHandle (UsbrHandle);
  		    if(CompareDevicePaths((EFI_DEVICE_PATH_PROTOCOL*)&mUsbrDevicePath, TempDevicePath))
  		    {
  			  BbsTable[Index].BootPriority  = 0;
  			  Status                        = EFI_SUCCESS;
  	            }
  	    }
      }
  }
  
  return Status;
}

static
EFI_DEVICE_PATH_PROTOCOL *
BdsCreateLegacyBootDevicePath (
  IN EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,	
  IN UINTN                        OptionCount,
  IN UINT16     DeviceType,
  IN UINT16     DeviceIndex,
  IN BOOLEAN    USBrBoot
  )
/*++

Routine Description:
  
  Build the BBS Device Path for this boot selection

Arguments:

  DeviceType  - Boot device whose device type
  DeviceIndex - Boot device whose device index
  IdeRBoot    - If USBrBoot is TRUE then check USBr device
  
Returns:

  EFI_STATUS
  
--*/
{
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  BBS_BBS_DEVICE_PATH       BbsDevicePathNode;
  BBS_TABLE                 *BbsTable;
  UINT16                    HddCount;
  UINT16                    BbsCount;
  HDD_INFO                  *LocalHddInfo;
  UINT16                    Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_STATUS                Status;


  HddCount      = 0;
  BbsCount      = 0;
  LocalHddInfo  = NULL;

  Index         = DeviceIndex;

  //
  // Make sure the Legacy Boot Protocol is available
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID**) &LegacyBios);
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  //
  // Get BBS table instance
  //
  Status = LegacyBios->GetBbsInfo (
                        LegacyBios,
                        &HddCount,
                        &LocalHddInfo,
                        &BbsCount,
                        &BbsTable
                        );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  //
  // For debug
  //
  //PrintBbsTable (BbsTable, BbsCount);
   

  //
  // Update the table with our required boot device
  //
  if (USBrBoot) {
    Status = RefreshBbsTableForUSBrBoot (
	         BootOptions,
	         OptionCount,
              Index,
              DeviceType,
              BbsCount,
              BbsTable
              );
  } else {
    Status = RefreshBbsTableForBoot (
              Index,
              DeviceType,
              BbsCount,
              BbsTable
              );
  }
  //
  // For debug
  //
  //PrintBbsTable (BbsTable, BbsCount);
  
 

  if (EFI_ERROR (Status)) {
    //
    // Device not found - do normal boot
    //
    gST->ConOut->OutputString (
                  gST->ConOut,
                  L"Can't Find Boot Device by Boot Option !!\r\n"
                  );
    while(1);
    return NULL;
  }
  //
  // Build the BBS Device Path for this boot selection
  //
  DevicePath = AllocateZeroPool (sizeof (EFI_DEVICE_PATH_PROTOCOL));
  if (DevicePath == NULL) {
    return NULL;
  }

  BbsDevicePathNode.Header.Type     = BBS_DEVICE_PATH;
  BbsDevicePathNode.Header.SubType  = BBS_BBS_DP;
  SetDevicePathNodeLength (&BbsDevicePathNode.Header, sizeof (BBS_BBS_DEVICE_PATH));
  BbsDevicePathNode.DeviceType  = DeviceType;
  BbsDevicePathNode.StatusFlag  = 0;
  BbsDevicePathNode.String[0]   = 0;

  DevicePath = AppendDevicePathNode (
                EndDevicePath,
                (EFI_DEVICE_PATH_PROTOCOL *) &BbsDevicePathNode
                );
  if (NULL == DevicePath) {
    return NULL;
  }

  return DevicePath;
}

EFI_DEVICE_PATH_PROTOCOL *
BdsCreateShellDevicePath (
  VOID
  )
/*++

Routine Description:

  This function will create a SHELL BootOption to boot.

Arguments:

  None.

Returns:

  Shell Device path for booting.

--*/
{
  UINTN                             FvHandleCount;
  EFI_HANDLE                        *FvHandleBuffer;
  UINTN                             Index;
  EFI_STATUS                        Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL     *Fv;
  UINTN                             Size;
  UINT32                            AuthenticationStatus;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  VOID                              *Buffer;

  DevicePath  = NULL;
  Status      = EFI_SUCCESS;

  DEBUG ((DEBUG_ERROR, "[FVMAIN2] BdsCreateShellDevicePath\n"));
  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiFirmwareVolume2ProtocolGuid,
        NULL,
        &FvHandleCount,
        &FvHandleBuffer
        );

  for (Index = 0; Index < FvHandleCount; Index++) {
    gBS->HandleProtocol (
          FvHandleBuffer[Index],
          &gEfiFirmwareVolume2ProtocolGuid,
          (VOID **) &Fv
          );

    Buffer  = NULL;
    Size    = 0;
    Status  = Fv->ReadSection (
                    Fv,
                    PcdGetPtr(PcdShellFile),
                    EFI_SECTION_PE32,
                    0,
                    &Buffer,
                    &Size,
                    &AuthenticationStatus
                    );
    if (EFI_ERROR (Status)) {
      //
      // Skip if no shell file in the FV
      //
      continue;
    } else {
      //
      // Found the shell
      //
      break;
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // No shell present
    //
    if (FvHandleCount) {
      FreePool (FvHandleBuffer);
    }
    return NULL;
  }
  //
  // Build the shell boot option
  //
  DevicePath = DevicePathFromHandle (FvHandleBuffer[Index]);

  if (FvHandleCount) {
    FreePool (FvHandleBuffer);
  }

  return DevicePath;
}

EFI_STATUS
BdsAsfBoot (
  OUT  EFI_DEVICE_PATH_PROTOCOL        **DevicePath
  )
/*++

Routine Description:

  Found out ASF boot options.

Arguments:

  DevicePath - The device path which describe where to load 
               the boot image or the legcy BBS device path 
               to boot the legacy OS

Returns:

  EFI_STATUS

--*/
{

  //
  // First we check ASF boot options Special Command
  //
  switch (mAsfBootOptions->SpecialCommand) {
  //
  // No additional special command is included; the Special Command Parameter has no
  // meaning.
  //
  case NOP:
    break;

  //
  // The Special Command Parameter can be used to specify a PXE
  // parameter. When the parameter value is 0, the system default PXE device is booted. All
  // other values for the PXE parameter are reserved for future definition by this specification.
  //
  case FORCE_PXE:
    if (mAsfBootOptions->SpecialCommandParam != 0) {
      //
      // ASF spec says 0 currently only option
      //
      return EFI_UNSUPPORTED;
    }
    BmConnectAllDriversToAllControllers ();
    *DevicePath = BdsCreateLegacyBootDevicePath (NULL, 0, BBS_EMBED_NETWORK, mAsfBootOptions->SpecialCommandParam, FALSE);

    break;

    //
    // The Special Command Parameter identifies the boot-media index for
    // the managed client. When the parameter value is 0, the default hard-drive/optical drive is booted, when the
    // parameter value is 1, the primary hard-drive/optical drive is booted; when the value is 2, the secondary
    // hard-drive/optical drive is booted and so on.
    //
  case FORCE_HARDDRIVE:
  case FORCE_SAFEMODE:
    *DevicePath = BdsCreateLegacyBootDevicePath (NULL, 0, BBS_TYPE_HARDDRIVE, mAsfBootOptions->SpecialCommandParam, FALSE);
    break;

  //
  // The Special Command Parameter can be used to specify a
  // diagnostic parameter. When the parameter value is 0, the default diagnostic media is booted.
  // All other values for the diagnostic parameter are reserved for future definition by this
  // specification.
  //
  case FORCE_DIAGNOSTICS:
    if (mAsfBootOptions->SpecialCommandParam != 0) {
      //
      // ASF spec says 0 currently only option
      //
      return EFI_UNSUPPORTED;
    }

    *DevicePath = BdsCreateShellDevicePath ();

    //
    // We want everything connected up for shell
    //
    BmConnectAllDriversToAllControllers();  
   
    break;

  //
  // The Special Command Parameter identifies the boot-media index for
  // the managed client. When the parameter value is 0, the default CD/DVD is booted, when the
  // parameter value is 1, the primary CD/DVD is booted; when the value is 2, the secondary
  // CD/DVD is booted ¡V and so on.
  //
  case FORCE_CDDVD:
    *DevicePath = BdsCreateLegacyBootDevicePath (NULL, 0, BBS_TYPE_CDROM, mAsfBootOptions->SpecialCommandParam, FALSE);
    break;

  default:
    return EFI_UNSUPPORTED;
    break;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
BdsAmtBoot (
  IN EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,	
  IN UINTN                        OptionCount,
  OUT  EFI_DEVICE_PATH_PROTOCOL        **DevicePath
  )
/*++

Routine Description:

  Check IdeR boot device and Asf boot device

Arguments:

  DevicePath - The device path which describe where to load 
               the boot image or the legcy BBS device path 
               to boot the legacy OS

Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS  Status;

  Status = EFI_NOT_FOUND;
  //
  // OEM command values; the interpretation of the Special Command and associated Special
  // Command Parameters is defined by the entity associated with the Enterprise ID.
  //
  if (ActiveManagementEnableStorageRedir ()) {
      *DevicePath = BdsCreateLegacyBootDevicePath (
		      	  	  BootOptions,
		      	  	  OptionCount,
                      BBS_TYPE_CDROM,                      
                      (mAsfBootOptions->SpecialCommandParam >> USBR_BOOT_DEVICE_SHIFT) & USBR_BOOT_DEVICE_MASK,
                      TRUE
                      );


    Status = EFI_SUCCESS;
  } else {
    Status = BdsAsfBoot (DevicePath);
  }

  return Status;
}

BDS_COMMON_OPTION *
BdsCreateBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description
  )
/*++

Routine Description:
  
  This function will create a BootOption from the give device path and 
  description string.

Arguments:

  DevicePath       - The device path which the option represent
  Description      - The description of the boot option
  
Returns:
  
  BDS_COMMON_OPTION - A BDS_COMMON_OPTION pointer
  
--*/
{
  BDS_COMMON_OPTION *Option;

  Option = AllocateZeroPool (sizeof (BDS_COMMON_OPTION));
  if (Option == NULL) {
    return NULL;
  }

  Option->Signature   = BDS_LOAD_OPTION_SIGNATURE;
  Option->DevicePath  = AllocateZeroPool (GetDevicePathSize (DevicePath));
  CopyMem (Option->DevicePath, DevicePath, GetDevicePathSize (DevicePath));

  Option->Attribute   = LOAD_OPTION_ACTIVE;
  Option->Description = AllocateZeroPool (StrSize (Description));
  CopyMem (Option->Description, Description, StrSize (Description));

  return Option;
}

EFI_STATUS
AsfDoLegacyBoot (
  IN  BDS_COMMON_OPTION           *Option
  )
/*++

Routine Description:
 
  Boot the legacy system with the boot option

Arguments:

  Option           - The legacy boot option which have BBS device path

Returns:

  EFI_UNSUPPORTED  - There is no legacybios protocol, do not support
                     legacy boot.
                         
  EFI_STATUS       - Return the status of LegacyBios->LegacyBoot ().

--*/
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID**) &LegacyBios);
  if (EFI_ERROR (Status)) {
    //
    // If no LegacyBios protocol we do not support legacy boot
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Write boot to OS performance data to a file
  //
//  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Legacy Boot: %S\n", Option->Description));
  return LegacyBios->LegacyBoot (
                      LegacyBios,
                      (BBS_BBS_DEVICE_PATH *) Option->DevicePath,
                      Option->LoadOptionsSize,
                      Option->LoadOptions
                      );
}
//AMI_OVERRIDE_END - AMI IDER boot feature override
/**
  Process ASF boot options and if available, attempt the boot

  @param[in] None.

  @retval EFI_SUCCESS    The command completed successfully
**/
EFI_STATUS
BdsBootViaAsf (
  IN  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions;
  UINTN                        OptionCount;
  INTN                         BootOptionIndex;
  BOOLEAN                      EfiBoot;
#ifdef CSM_FLAG
  EFI_LEGACY_BIOS_PROTOCOL     *LegacyBios;
#endif
  EFI_INPUT_KEY                Key = {0};
  UINTN                        EventIndex;
  EFI_TPL                      OldTpl;
//AMI_OVERRIDE_START - AMI IDER boot feature override
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BDS_COMMON_OPTION         *LegacyBootOption;
  EFI_EVENT                 ReadyToBootEvent;
//AMI_OVERRIDE_END - AMI IDER boot feature override

  Status          = EFI_SUCCESS;
  BootOptions     = NULL;
  EfiBoot         = TRUE;

#ifdef CSM_FLAG
  //
  // Check if this is legacy or UEFI boot
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (LegacyBios != NULL) {
    EfiBoot = FALSE;
  }
#endif
// AMI_OVERRIDE_START - AMI IDER boot feature override, Always set TRUE, Try to boot EFI first
  EfiBoot = TRUE;
// AMI_OVERRIDE_END - AMI IDER boot feature override, Always set TRUE, Try to boot EFI first

  //
  // Check if ASF Boot Options is present.
  //
  if (mAsfBootOptions->SubCommand != ASF_BOOT_OPTIONS_PRESENT) {
    return EFI_NOT_FOUND;
  }

  BootOptions = EfiBootManagerGetLoadOptions(&OptionCount, LoadOptionTypeBoot);
  if (BootOptions == NULL) {
    return EFI_UNSUPPORTED;
  }

  BootOptionIndex = GetForcedBootOptionIndex (BootOptions, OptionCount, EfiBoot);
  //
  // If device path was set, the we have a boot option to use
  //
#if 0 // AMI_OVERRIDE_START - AMI IDER boot feature override
  if (BootOptionIndex == -1) {
    return EFI_UNSUPPORTED;
  }
#endif // AMI_OVERRIDE_END - AMI IDER boot feature override

  //
  // If this is RCO/Storage Redirection EFI Boot, keep trying unless user cancels
  //
  while (!(Key.ScanCode == SCAN_ESC && Key.UnicodeChar == 0)) {
    EfiBootManagerBoot (&BootOptions[BootOptionIndex]);
//AMI_OVERRIDE_START - For AMT security Boot, If BIOS can't find signed SROU boot image, the BIOS shall not attempt to boot form next bootable device	
    if(ActiveManagementIsStorageRedirectionEnabled() && ((BootOptions[BootOptionIndex].Status == EFI_SECURITY_VIOLATION) || (BootOptions[BootOptionIndex].Status == EFI_ACCESS_DENIED)))
    {
      gST->ConOut->OutputString (
           gST->ConOut,
           L"USBR Boot Fail, system halt !! \r\n");

      while(1);
    }  
//AMI_OVERRIDE_END - For AMT security Boot, If BIOS can't find signed SROU boot image, the BIOS shall not attempt to boot form next bootable device    
    //
    // Returning from EfiBootManagerBoot means the boot failed
    // Display message to user before attempting another RCO/Storage Redirection boot
    //
//AMI_OVERRIDE_START - AMI IDER boot feature override, If UEFI boot Failed, try to Legacy boot
	  Status = BdsAmtBoot (BootOptions, OptionCount, &DevicePath);
	  if (DevicePath == NULL || EFI_ERROR(Status)) {
	    return EFI_UNSUPPORTED;
	  }
	  
	  LegacyBootOption = BdsCreateBootOption (DevicePath, L"ASF Boot");
	  if (LegacyBootOption == NULL) {
	    return EFI_OUT_OF_RESOURCES;
	  }
	  
	  Status = EfiCreateEventReadyToBootEx (
	            TPL_CALLBACK,
	            NULL,
	            NULL,
	            &ReadyToBootEvent
	            );

	  if (!EFI_ERROR (Status)) {
	    gBS->SignalEvent (ReadyToBootEvent);
	    gBS->CloseEvent (ReadyToBootEvent);
	  }	  
	   Status = AsfDoLegacyBoot (LegacyBootOption);
//AMI_OVERRIDE_END - AMI IDER boot feature override
    gST->ConOut->ClearScreen (gST->ConOut);
    gST->ConOut->OutputString (
                  gST->ConOut,
                  L"RCO/USBR boot failed. Press ENTER to try again or ESC to return to regular boot\r\n"
                  );
    Key.ScanCode    = 0;
    Key.UnicodeChar = 0;
    OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
    gBS->RestoreTPL (TPL_APPLICATION);
    while (!(Key.ScanCode == 0 && Key.UnicodeChar == L'\r')) {
      gBS->WaitForEvent (1, &(gST->ConIn->WaitForKey), &EventIndex);
      gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

      if (Key.ScanCode == SCAN_ESC && Key.UnicodeChar == 0) {
        break;
      }
    }

    if (OldTpl > TPL_APPLICATION) {
      gBS->RaiseTPL (OldTpl);
    }
  }

//AMI_OVERRIDE_START - AMI IDER boot feature override
  gBS->FreePool (DevicePath);
  gBS->FreePool (LegacyBootOption);
//AMI_OVERRIDE_END - AMI IDER boot feature override  
  EfiBootManagerFreeLoadOptions (BootOptions, OptionCount);

  return Status;
}

