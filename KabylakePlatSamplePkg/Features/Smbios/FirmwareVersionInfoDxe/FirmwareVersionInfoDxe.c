/** @file
  This file implements firmware version information.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2017 Intel Corporation.

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

#include <PiDxe.h>
#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>

#include <OemSetup.h>
#include <SetupVariable.h>
#include <Library/I2cDriverLib.h>
#include <CommonDefinitions.h>
#include <PchAccess.h>
#include <Protocol/Smbios.h>
#include <BiosGuard.h>
// AMI_OVERRIDE_START - Fixed EFI_EVENT_RUNTIME_CONTEXT redefine.
#define _DXECIS_H_
// AMI_OVERRIDE_END - Fixed EFI_EVENT_RUNTIME_CONTEXT redefine.
#include <FirmwareVersionInfoDxe.h>
#include <Library/MmPciLib.h>

#include <Protocol/IccOverClocking.h>
#include <Protocol/NetworkInterfaceIdentifier.h>
#include <Protocol/PxeBaseCode.h>
#include <Protocol/GopComponentName2.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/FirmwareVolume2.h>

#include <PlatformBoardConfig.h>
#include <Library/BoardConfigLib.h>
#include <Library/CpuPlatformLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED CONST CHAR8 StrEnabled[] = PLATFORM_FVI_STATUS_ENABLED;

GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_INFO                             *mPlatformInfo;

//
// BCD to Decimal Conversion
//
#define BCD_TO_DECIMAL(A)           (((((A) >> 4) & 0xF) * 10) + ((A) & 0xF))

extern EFI_GUID gPlatformFviSetupGuid;
extern EFI_GUID gBdsAllDriversConnectedProtocolGuid;
extern EFI_GUID gUndiDriverImageGuid;
extern EFI_GUID gEfiI2cIoProtocolGuid;
extern EFI_GUID gEfiSmbiosProtocolGuid;

/**
  Get EC Version
**/
VOID
GetEcVersion(
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_SMBIOS_HANDLE       SmbiosHandle;
  EFI_SMBIOS_PROTOCOL     *mSmbios;
  SMBIOS_TABLE_TYPE0      *Type0Record;
  EFI_SMBIOS_TABLE_HEADER *Record;

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **)&mSmbios
                  );
  if (!EFI_ERROR (Status)) {
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    do {
      Status = mSmbios->GetNext (mSmbios, &SmbiosHandle, NULL, &Record, NULL);
      if (EFI_ERROR(Status)) {
        break;
      }
      if (Record->Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION) {
        Type0Record = (SMBIOS_TABLE_TYPE0 *) Record;
        mPlatformFviData[EC_VER].Version.MajorVersion = Type0Record->EmbeddedControllerFirmwareMajorRelease;
        mPlatformFviData[EC_VER].Version.MinorVersion = Type0Record->EmbeddedControllerFirmwareMinorRelease;
        mPlatformFviData[EC_VER].Version.Revision     = 0;
        mPlatformFviData[EC_VER].Version.BuildNum     = 0;
        break;
      }
    } while (Record->Type != EFI_SMBIOS_TYPE_BIOS_INFORMATION);
  }
}


/**
  Get base EC firmware version
**/
VOID
GetBaseEcFwVersion (
  VOID
  )
{
  mPlatformFviData[BASE_EC_FW_VER].Version.MajorVersion = BCD_TO_DECIMAL(mPlatformInfo->BaseEcFwMajorVersion);
  mPlatformFviData[BASE_EC_FW_VER].Version.MinorVersion = BCD_TO_DECIMAL(mPlatformInfo->BaseEcFwMinorVersion);
  mPlatformFviData[BASE_EC_FW_VER].Version.Revision     = 0;
  mPlatformFviData[BASE_EC_FW_VER].Version.BuildNum     = 0;
}


/**
  Get EC to EC protocol version
**/
VOID
Ec2EcProtocolVersion (
  VOID
  )
{
  mPlatformFviData[EC_2_EC_PROTOCOL_VER].Version.MajorVersion = BCD_TO_DECIMAL(mPlatformInfo->EcProtocolVersion);     // EC protocol version
  mPlatformFviData[EC_2_EC_PROTOCOL_VER].Version.MinorVersion = BCD_TO_DECIMAL(mPlatformInfo->BaseEcProtocolVersion); // Base EC protocol version
  mPlatformFviData[EC_2_EC_PROTOCOL_VER].Version.Revision     = 0;
  mPlatformFviData[EC_2_EC_PROTOCOL_VER].Version.BuildNum     = 0;
}

/**
  Change a Unicode string to an ASCII string.

  @param[in]  UStr    The Unicode string.
  @param[in]  Length  The maximum size of AStr.
  @param[out] AStr    ASCII string to pass out.

  @retval     Index   The actuall length.
**/
UINTN
EFIAPI
UnicodeToAscii (
  IN CONST CHAR16   *UStr,
  IN CONST UINTN    Length,
  OUT CHAR8         *AStr
  )
{
  UINTN Index;

  //
  // just buffer copy, not character copy
  //
  for (Index = 0; Index < Length; Index++) {
    *AStr++ = (CHAR8) *UStr++;
  }

  return Index;
}


/**
  Get Sensor FW Version - The sensor FW version is set in the variable
  "SensorSerialNumber" if not sensor is available SENSOR_FW_VER isnt updated
**/
VOID
GetSensorFwVersion (
  VOID
  )
{
  EFI_STATUS              Status;
  CHAR16                  *fpGetVar = NULL;
  CHAR8                   *Str;
  UINTN                   VariableSize;
  UINTN                   Length;
#if 0
  UINT32                  SensorHubFwVersion = 0;
  SETUP_DATA              gSetupData;
  EFI_I2C_BUS_PROTOCOL    *I2cBusProtocol = NULL;
  EFI_I2C_REQUEST_PACKET  Request;
  EFI_HANDLE              *HandleArray = NULL;
  UINTN                   HandleArrayCount = 0;
  UINTN                   Index;
  UINT8                   ReadBuffer [0x100];
  UINT8                   WriteBuffer [0x100];
  CHAR8                   mSensorHubString [] = \
                          {'I', '2', 'C', '0', '0', \
                           '\\','S', 'S', 'H', 'U', 'B', 0};
#endif

  Str = NULL;

  if (PcdGetBool (PcdSensorHubSupport)) {
    VariableSize = 0;
    Status = gRT->GetVariable (
                    L"SensorSerialNumber",
                    &gSetupVariableGuid,
                    NULL,
                    &VariableSize,
                    fpGetVar
                    );

    if (Status == EFI_BUFFER_TOO_SMALL) {
      fpGetVar = AllocateZeroPool (VariableSize);
      if (fpGetVar == NULL) return;

      Status = gRT->GetVariable (
                      L"SensorSerialNumber",
                      &gSetupVariableGuid,
                      NULL,
                      &VariableSize,
                      fpGetVar
                      );
    }

    if (!EFI_ERROR(Status) && fpGetVar != NULL) {
      Length = StrLen (fpGetVar);
      Str = AllocateZeroPool (Length + 1);
      if (Str == NULL) return;
      UnicodeToAscii (fpGetVar, Length, Str);
      mPlatformFviStrings[SENSOR_FW_VER].VersionString = Str;
    }

    if (fpGetVar != NULL) {
      FreePool (fpGetVar);
    }
  }
#if 0
    case BoardIdSkylakeSds:
    {
      ///@todo Check how SKL SDS will interact with Sensor Hub, HSB and WSB was I2C
      VariableSize = sizeof (SETUP_DATA);
      Status = gRT->GetVariable (
                      L"Setup",
                      &gSetupVariableGuid,
                      NULL,
                      &VariableSize,
                      &gSetupData
                      );
      ASSERT_EFI_ERROR (Status);

      //
      // Return N/A, if I2C0 Sensor Hub is not enabled
      //
      if (gSetupData.PchI2c0SensorDevice != 0) {
        //
        // Find I2C0 controller and connect I2C stack
        //
        Status = gBS->LocateHandleBuffer (
                        ByProtocol,
                        &gMmioDeviceProtocolGuid,
                        NULL,
                        &HandleArrayCount,
                        &HandleArray
                        );
        for (Index = 0; Index < HandleArrayCount; Index++) {
          //
          // Start I2C stack to start I2c bus protocol
          //
          gBS->ConnectController (HandleArray[Index], NULL, NULL, FALSE);
        }
        if (HandleArray != NULL) {
          FreePool (HandleArray);
          HandleArray = NULL;
        }
        Status = gBS->LocateHandleBuffer (
                        ByProtocol,
                        &gEfiI2cIoProtocolGuid,
                        NULL,
                        &HandleArrayCount,
                        &HandleArray
                        );
        if (!EFI_ERROR (Status)) {
          for (Index = 0; HandleArrayCount > Index; Index++) {
            if (NULL != DlAcpiFindDeviceWithMatchingCid (
                          HandleArray [Index],
                          0,
                          (CONST CHAR8 *) mSensorHubString)) {
              //
              //  The device was found
              //
              Status = gBS->OpenProtocol (
                              HandleArray [Index],
                              &gEfiI2cIoProtocolGuid,
                              (VOID **) &I2cBusProtocol,
                              NULL,
                              NULL,
                              EFI_OPEN_PROTOCOL_GET_PROTOCOL
                              );

              //
              // Send request to get sensor hub revision
              //
              ZeroMem (ReadBuffer, 0x100);
              ZeroMem (WriteBuffer, 0x100);
              Request.ReadBytes = SENSOR_HUB_FW_VERSION_LENGTH;
              Request.ReadBuffer = &ReadBuffer[0];
              Request.WriteBytes = 0x02;
              Request.WriteBuffer = &WriteBuffer[0];
              WriteBuffer[0] = SENSOR_HUB_FW_VERSION_OFFSET & 0xFF;
              WriteBuffer[1] = (SENSOR_HUB_FW_VERSION_OFFSET & 0xFF00) >> 8;
              Request.Timeout = 1000;

              Status = I2cBusProtocol->StartRequest (
                                         I2cBusProtocol,
                                         0,  //SlaveAddressIndex; NOT Slave Address
                                         NULL,
                                         &Request,
                                         NULL
                                         );
              if (!EFI_ERROR (Status)) {
                //
                //  Retrieved Sensor Hub FW version
                //
                SensorHubFwVersion = (ReadBuffer[0]) | (ReadBuffer[1] << 8) | (ReadBuffer[2] << 16) | (ReadBuffer[3] << 24);
                mPlatformFviData[SENSOR_FW_VER].Version.MajorVersion = 1;
                mPlatformFviData[SENSOR_FW_VER].Version.MinorVersion = 0;
                mPlatformFviData[SENSOR_FW_VER].Version.Revision     = (UINT8) ((SensorHubFwVersion / 10000) & 0xFF);
                mPlatformFviData[SENSOR_FW_VER].Version.BuildNum     = (UINT16) (SensorHubFwVersion % 10000);
              }
            }
          }
        }
        if (HandleArray != NULL) {
          FreePool (HandleArray);
          HandleArray = NULL;
        }
      }
    }
    break;
#endif
}


/**
  GetPhyVersion

  @param[in]  GbEBar      GbE Base Address
  @param[out] LanPhyRev   Lan Phy Revision

  @retval     EFI_TIMEOUT
  @retval     EFI_SUCCESS
**/
EFI_STATUS
GetPhyVersion(
  IN  UINT32 GbEBar,
  OUT UINT8  *LanPhyRev
  )
{
  UINT32          RegisterValue = 0;
  UINTN           LoopTime;

  //
  // Write PHY_CONFIG_REG with set page 769
  //
  MmioWrite32((GbEBar + PHY_CONFIG_REG), PHY_PAGE769_SET_REG);
  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_CONFIG_REG);
    if (RegisterValue & BIT28) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  //
  // Delay 4ms after page change
  //
  gBS->Stall(4000);

  //
  // Write PHY_CONFIG_REG with slow MDIO mode
  //
  MmioWrite32((GbEBar + PHY_CONFIG_REG), PHY_SLOW_MDIO_MODE_REG);
  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_CONFIG_REG);
    if (RegisterValue & BIT28) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  //
  // Read register PHY Version (offset 0x3)
  //
  MmioWrite32((GbEBar + PHY_CONFIG_REG), PHY_READ_PHY_OFFSET3_REG);
  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_CONFIG_REG);
    if (RegisterValue & BIT28) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  //
  // Read LAN PHY revision from PHY register 3
  //
  *LanPhyRev = (UINT8)MmioRead16(GbEBar + PHY_CONFIG_REG);

  //
  // Write PHY_CONFIG_REG to write fast mdio mode
  //
  MmioWrite32((GbEBar + PHY_CONFIG_REG), PHY_FAST_MDIO_MODE_REG);
  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_CONFIG_REG);
    if (RegisterValue & BIT28) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}


/**
  Release GbeSempahore

  @param[in] GbEBar  GbE Base Address
**/
VOID
ReleaseGbeSemaphore(
  IN  UINT32      GbEBar
  )
{
  MmioAnd32 ((GbEBar + PHY_SEMAPHORE_REG), (UINT32) ~BIT5);
}


/**
  Acquire GbeSempahore

  @param[in] GbEBar      GbE Base Address

  @retval    EFI_TIMEOUT
  @retval    EFI_SUCCESS
**/
EFI_STATUS
AcquireGbeSemaphore(
  IN  UINT32      GbEBar
  )
{
  UINTN           LoopTime;
  UINT32          RegisterValue = 0;

  MmioOr32 ((GbEBar + PHY_SEMAPHORE_REG), BIT5);

  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    RegisterValue = MmioRead32(GbEBar + PHY_SEMAPHORE_REG);

    if (RegisterValue & BIT5) {
      break;
    }
    gBS->Stall(10);
  }
  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }
  return EFI_SUCCESS;
}


/**
  Retrieves LAN PHY Revision by reading register at the first boot time
  after flashing FW image or updating capsule and set into NV variable.
  Then get the revision from NV varialbe during POST at the other time.
**/
VOID
GetLanPhyVersion(
  VOID
  )
{
  UINT32             GbEBar = 0;
  EFI_STATUS         Status;
  UINTN              LanPciRegBase;
  UINT8              Data;
  BOOLEAN            Found = FALSE;
  UINT8              LanPhyRev = 0;
  PLATFORM_FVI_SETUP PlatformFviSetup = {0};
  UINTN              VariableSize;

  VariableSize = sizeof(PLATFORM_FVI_SETUP);
  Status = gRT->GetVariable (
                  PLATFORM_FVI_SETUP_NAME,
                  &gPlatformFviSetupGuid,
                  NULL,
                  &VariableSize,
                  &PlatformFviSetup
                  );

  if ((!EFI_ERROR(Status)) && (PlatformFviSetup.LanPhyVersion != 0)) {
    //
    // The other time, to get Lan Phy revision from NV.
    //
    LanPhyRev = PlatformFviSetup.LanPhyVersion;
  } else if (Status == EFI_NOT_FOUND){
    //
    // First time, get Lan Phy revision by reading register
    //
    LanPciRegBase = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LAN,
                      PCI_FUNCTION_NUMBER_PCH_LAN
                      );
    GbEBar = MmioRead32(LanPciRegBase + R_PCH_LAN_MBARA) & B_PCH_LAN_MBARA_BA;
    Data = MmioRead8(LanPciRegBase + PCI_COMMAND_OFFSET);

    //
    // Command Register's Value is 0 in Fast Boot and Modifying here
    //
    if((Data & EFI_PCI_COMMAND_MEMORY_SPACE) == 0) {
      MmioWrite8 ((LanPciRegBase + PCI_COMMAND_OFFSET), 0x07);
      Found = TRUE;
    }

    //
    // Acquire Gbe Semaphore
    //
    Status = AcquireGbeSemaphore(GbEBar);
    if (!EFI_ERROR(Status)) {
      //
      // Get Phy Version
      //
      GetPhyVersion(GbEBar, &LanPhyRev);
    }

    //
    // Free Gbe semaphore
    //
    ReleaseGbeSemaphore(GbEBar);

    if(Found == TRUE) {
      MmioWrite8 ((LanPciRegBase + PCI_COMMAND_OFFSET), Data);
    }

    PlatformFviSetup.LanPhyVersion = LanPhyRev;
    gRT->SetVariable (
           PLATFORM_FVI_SETUP_NAME,
           &gPlatformFviSetupGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
           sizeof(PLATFORM_FVI_SETUP),
           &PlatformFviSetup
           );
  }

  //
  // LAN PHY Revision
  //
  mPlatformFviData[LAN_PHY_VER].Version.MajorVersion = 0;
  mPlatformFviData[LAN_PHY_VER].Version.MinorVersion = 0;
  mPlatformFviData[LAN_PHY_VER].Version.BuildNum = LanPhyRev;
  mPlatformFviData[LAN_PHY_VER].Version.Revision = 0;

  return;
}


/**
  Update the Debug Mode Status, Performance Mode Status and Debug Use USB Status.

**/
VOID
UpdateDebugInformation(
  VOID
  )
{
  //
  // Default debug mode is disabled and update when MDEPKG_NDEBUG is False
  //
  DEBUG_CODE(mPlatformFviStrings[DEBUG_MODE_VER].VersionString = (CHAR8 *) &StrEnabled;);

  //
  // Default Performance mode is disabled and update it when PERFORMANCE_FLAG is defined.
  //
#ifdef PERFORMANCE_FLAG
  mPlatformFviStrings[PERFORMANCE_MODE_VER].VersionString = (CHAR8 *) &StrEnabled;
#endif

  //
  // Default Debug usage is Serial port and update it when DEBUGUSEUSB_FLAG is TRUE.
  //
#ifdef DEBUGUSEUSB_FLAG
  mPlatformFviStrings[DEBUB_USE_USB_VER].VersionString = (CHAR8 *) &StrEnabled;
#endif
}

/**
  Get Icc Overclock version from the ICC_OVERCLOCKING_PROTOCOL
**/
VOID
GetIccOverclockingVersion(
  VOID
  )
{
  EFI_STATUS                Status;
  ICC_LIB_VERSION           Version = {0};
  PLATFORM_FVI_SETUP        PlatformFviSetup = {0};
  UINTN                     VariableSize;

  VariableSize = sizeof(PLATFORM_FVI_SETUP);
  Status = gRT->GetVariable (
                  PLATFORM_FVI_SETUP_NAME,
                  &gPlatformFviSetupGuid,
                  NULL,
                  &VariableSize,
                  &PlatformFviSetup
                  );
  if ((!EFI_ERROR(Status)) && (PlatformFviSetup.IccMajorVersion != 0)){
    Version.Major = PlatformFviSetup.IccMajorVersion;
    Version.Minor = PlatformFviSetup.IccMinorVersion;
    Version.Hotfix= PlatformFviSetup.IccHotfixVersion;
    Version.Build = PlatformFviSetup.IccBuildVersion;
  } else {
    DEBUG ((DEBUG_WARN, "FirmwareVersionInfo: ICC librar version uninitialized.\n"));
  }

  mPlatformFviData[ICC_OVERLOCKING_VER].Version.MajorVersion = (UINT8) Version.Major;
  mPlatformFviData[ICC_OVERLOCKING_VER].Version.MinorVersion = (UINT8) Version.Minor;
  mPlatformFviData[ICC_OVERLOCKING_VER].Version.Revision     = (UINT8) Version.Hotfix;
  mPlatformFviData[ICC_OVERLOCKING_VER].Version.BuildNum     = Version.Build;
}


/**
  Get UNDI version.

  @param[in] Event    The Event this notify function registered to.
  @param[in] Context  Pointer to the context data registered to the Event.
**/
VOID
EFIAPI
GetUndiVersion (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       DriverImageHandleCount;
  EFI_HANDLE                  *DriverImageHandleBuffer;
  EFI_LOADED_IMAGE_PROTOCOL   *LoadedImage;
  EFI_GUID                    *NameGuid;
  EFI_DEVICE_PATH_PROTOCOL    *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *LastDeviceNode;
  EFI_DRIVER_BINDING_PROTOCOL *DriverBinding;
  UINT32                      RetVal;
  VOID                        *ProtocolPointer;

  RetVal = (UINT32)-1;
  DriverImageHandleCount  = 0;

  Status = gBS->LocateProtocol (&gEfiPxeBaseCodeProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
    return;
  }

  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadedImageProtocolGuid,
                  NULL,
                  &DriverImageHandleCount,
                  &DriverImageHandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  for (Index = 0; Index < DriverImageHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    DriverImageHandleBuffer[Index],
                    &gEfiLoadedImageProtocolGuid,
                    (VOID *) &LoadedImage
                    );
    if (LoadedImage->FilePath == NULL) {
      continue;
    }

    TempDevicePath = LoadedImage->FilePath;
    LastDeviceNode = TempDevicePath;
    while (!IsDevicePathEnd (TempDevicePath)) {
      LastDeviceNode = TempDevicePath;
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }
    NameGuid = EfiGetNameGuidFromFwVolDevicePathNode (
                 (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode
                 );
    if ((NameGuid != NULL) && (CompareGuid (NameGuid, &gUndiDriverImageGuid))) {
      Status = gBS->HandleProtocol(
                      DriverImageHandleBuffer[Index],
                      &gEfiDriverBindingProtocolGuid,
                      (VOID*)&DriverBinding
                      );
      if (!EFI_ERROR(Status)) {
        RetVal = DriverBinding->Version;
        break;
      }
    }
  }

  mPlatformFviData[UNDI_VER].Version.MajorVersion = (RetVal >> 24) & 0x000000FF;
  mPlatformFviData[UNDI_VER].Version.MinorVersion = (RetVal >> 16) & 0x000000FF;
  mPlatformFviData[UNDI_VER].Version.Revision     = (RetVal >>  8) & 0x000000FF;
  mPlatformFviData[UNDI_VER].Version.BuildNum     = RetVal & 0x000000FF;
}


/**
  Get GOP Version - To get the GOP version depends on the
  GOP_COMPONENT_NAME2_PROTOCOL_GUID and transfer the Revision the CHAR16 to UINT.

  @param[in] Event    The Event this notify function registered to.
  @param[in] Context  Pointer to the context data registered to the Event.
**/
VOID
EFIAPI
GetGopVersion(
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                    Status;
  UINT32                        Version[3];
  UINT32                        Index = 0;
  UINT32                        VersionIndex =0;
  CHAR16                        TempStr;
  CHAR16                        *DriverVersion;
  UINT32                        StringLength = 0;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios = NULL;
  GOP_COMPONENT_NAME2_PROTOCOL  *GopComponentName2Protocol = NULL;
  VOID                          *ProtocolPointer;

  Status = gBS->LocateProtocol (&gGopComponentName2ProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
    return;
  }

  gBS->CloseEvent (Event);

  //
  // Set the version default with zero
  //
  for (Index = 0; Index < 3; Index++){
    Version[Index] = 0;
  }

// AMI_OVERRIDE_START -Use token to replace CSM_FLAG because we don't definet it.>>
//#ifdef CSM_FLAG
#if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
// AMI_OVERRIDE_END  -Use token to replace CSM_FLAG because we don't definet it.<<
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **) &LegacyBios
                  );
#endif
  if (LegacyBios == NULL) {
    Status = gBS->LocateProtocol (&gGopComponentName2ProtocolGuid, NULL, (VOID **)&GopComponentName2Protocol);
    if (!EFI_ERROR (Status)) {
      Status = GopComponentName2Protocol->GetDriverVersion (
                                            GopComponentName2Protocol,
                                            "en-US",
                                            &DriverVersion
                                            );
      if (!EFI_ERROR (Status)) {
        for (Index = 0; (DriverVersion[Index] != '\0'); Index++) {
        }
        StringLength = (Index+1)*2;
        for (Index = 0 ; Index <= StringLength ; Index++) {
          TempStr = DriverVersion[Index];
          if (TempStr == '.'){
           VersionIndex++;
          } else if ((TempStr <= '9') && (TempStr >= '0')){
            Version[VersionIndex] *= 0x10;
            Version[VersionIndex] += ((UINT32)(TempStr) - '0');
          }
        }

        mPlatformFviData[GOP_VER].Version.MajorVersion = 0;
        mPlatformFviData[GOP_VER].Version.MinorVersion = (UINT8) Version[0];
        mPlatformFviData[GOP_VER].Version.Revision     = (UINT8) Version[1];
        mPlatformFviData[GOP_VER].Version.BuildNum     = (UINT16)Version[2];
      }
    }
  }
}

/**
  Get RoyalPark Version
**/
VOID
GetRoyalParkVersion (
  VOID
  )
{
  mPlatformFviData[ROYALPARK_VER].Version.MajorVersion = PcdGet8 (PcdClientCommonVersionMajor);
  mPlatformFviData[ROYALPARK_VER].Version.MinorVersion = PcdGet8 (PcdClientCommonVersionMinor);
  mPlatformFviData[ROYALPARK_VER].Version.Revision     = PcdGet8 (PcdClientCommonVersionRevision);
  mPlatformFviData[ROYALPARK_VER].Version.BuildNum     = (PcdGet8 (PcdClientCommonVersionBuild) << 8) | (PcdGet8 (PcdClientCommonVersionRoyalParkBuild));

  mPlatformFviStrings[ROYALPARK_VER].VersionString     = (CHAR8 *)PcdGetPtr (PcdClientCommonVersionString);
}

/**
  Get Platform Version
**/
VOID
GetPlatformVersion (
  VOID
  )
{
  mPlatformFviData[PLATFORM_VER].Version.MajorVersion = (UINT8)  PcdGet8 (PcdPlatformVersionMajor);
  mPlatformFviData[PLATFORM_VER].Version.MinorVersion = (UINT8)  PcdGet8 (PcdPlatformVersionMinor);
  mPlatformFviData[PLATFORM_VER].Version.Revision     = (UINT8)  PcdGet8 (PcdPlatformVersionRevision);
  mPlatformFviData[PLATFORM_VER].Version.BuildNum     = (UINT16) PcdGet8 (PcdPlatformVersionBuild);
}

/**
  Create SMBIOS Table type

  @param[in] Event    The Event this notify function registered to.
  @param[in] Context  Pointer to the context data registered to the Event.
**/
VOID
EFIAPI
InitializeFviDataCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS Status;
  VOID       *ProtocolPointer;

  Status = gBS->LocateProtocol (&gBdsAllDriversConnectedProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
    return;
  }

  gBS->CloseEvent (Event);

  Status = gBS->LocateProtocol (
                &gPlatformInfoProtocolGuid,
                NULL,
                (VOID **) &mPlatformInfo
                );
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize and report Platform FVI information to Smbios
  //
  mPlatformFviHeader.SmbiosHeader.Type = PcdGet8(PcdFviSmbiosType);

  GetEcVersion();
  GetSensorFwVersion ();
  GetLanPhyVersion ();
  UpdateDebugInformation ();
  GetIccOverclockingVersion ();

  if ((!EFI_ERROR (Status)) && (mPlatformInfo->DockAttached == 1)) {
    GetBaseEcFwVersion ();
    Ec2EcProtocolVersion ();
  }
  GetRoyalParkVersion ();
  GetPlatformVersion ();

  Status = AddFviEntry (mPlatformFviHeader, mPlatformFviData, mPlatformFviStrings);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AddFviEntry failed. Status = %r\n", Status));
  }
  return;
}


/**
  This is driver entry point to register the notification event.

  @param[in] ImageHandle  A handle for the image that is initializing this driver
  @param[in] SystemTable  A pointer to the EFI system table

  @retval    EFI_SUCCESS  The initialization finished successfully.
**/
EFI_STATUS
EFIAPI
FirmwareVersionInfoDxeEntry (
  IN  EFI_HANDLE         ImageHandle,
  IN  EFI_SYSTEM_TABLE   *SystemTable
  )
{
  VOID       *Registration1;
  VOID       *Registration2;
  VOID       *Registration3;

  //
  // Register InitializeFviDataCallback
  //
  EfiCreateProtocolNotifyEvent (
          &gBdsAllDriversConnectedProtocolGuid,
          TPL_CALLBACK,
          InitializeFviDataCallback,
          NULL,
          &Registration1
          );

  //
  // Get the UNDI driver version.
  //
  EfiCreateProtocolNotifyEvent (
          &gEfiPxeBaseCodeProtocolGuid,
          TPL_CALLBACK,
          GetUndiVersion,
          NULL,
          &Registration2
          );

  //
  // Get the GOP driver version.
  //
  EfiCreateProtocolNotifyEvent (
          &gGopComponentName2ProtocolGuid,
          TPL_CALLBACK,
          GetGopVersion,
          NULL,
          &Registration3
          );

  return EFI_SUCCESS;
}
