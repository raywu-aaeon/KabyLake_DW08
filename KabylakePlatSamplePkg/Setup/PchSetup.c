/**@file
  PCH Setup Routines.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2017 Intel Corporation.

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

#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <PlatformInfo.h>
#include <SetupPrivate.h>
#include <Library/SiFviLib.h>
#include "PchSetup.h"
#include "OemSetup.h"
#include <Library/PchGbeLib.h>
#include <Library/PchInfoLib.h>
#include <Library/MmPciLib.h>
#include <Protocol/PchInfo.h>
#include <Library/GpioLib.h>
#include <Library/GpioNativeLib.h>
#include <Protocol/Spi.h>
#include <Protocol/Smbios.h>
#include <Library/CpuPlatformLib.h>
// AMI_OVERRIDE_START - For our Setup variable design.
#include "AmiSetupPrivate.h"
// AMI_OVERRIDE_END - For our Setup variable design.
// AMI_OVERRIDE_START - Set SetupVolatileData for Rst remap menu. 
#include <Library/HobLib.h>
#include <PchPcieStorageDetectHob.h>
// AMI_OVERRIDE_END - Set SetupVolatileData for Rst remap menu. 

//
// Print primitives
//
#define LEFT_JUSTIFY  0x01
#define PREFIX_SIGN   0x02
#define PREFIX_BLANK  0x04
#define COMMA_TYPE    0x08
#define LONG_TYPE     0x10
#define PREFIX_ZERO   0x20

#define DXE_DEVICE_DISABLED                     0
#define DXE_DEVICE_ENABLED                      1

// AMI_OVERRIDE_START - Workstation PCH can support maximum 8 ports.
#define MAX_SATA_PORTS 8
// AMI_OVERRIDE_END - Workstation PCH can support maximum 8 ports.
//
// Length of temp string buffer to store value string.
//
#define CHARACTER_NUMBER_FOR_VALUE              30
#define _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED   0x0400
#define ATAPI_DEVICE                            0x8000

typedef enum {
  EfiCompatibility,
  EfiEnhancedMode
} EFI_SATA_MODE;

// AMI_OVERRIDE_START - Workstation PCH can support maximum 8 ports.
#if 0
GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID gSATA[6] = {
  STRING_TOKEN(STR_SATA0_NAME),
  STRING_TOKEN(STR_SATA1_NAME),
  STRING_TOKEN(STR_SATA2_NAME),
  STRING_TOKEN(STR_SATA3_NAME),
  STRING_TOKEN(STR_SATA4_NAME),
  STRING_TOKEN(STR_SATA5_NAME)
};
GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID gSOFTPRES[6] = {
  STRING_TOKEN(STR_SATA0_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA1_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA2_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA3_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA4_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA5_SOFT_PRESERVE_STATUS)
};
#else
GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID gSATA[MAX_SATA_PORTS] = {
  STRING_TOKEN(STR_SATA0_NAME),
  STRING_TOKEN(STR_SATA1_NAME),
  STRING_TOKEN(STR_SATA2_NAME),
  STRING_TOKEN(STR_SATA3_NAME),
  STRING_TOKEN(STR_SATA4_NAME),
  STRING_TOKEN(STR_SATA5_NAME),
  STRING_TOKEN(STR_SATA6_NAME),
  STRING_TOKEN(STR_SATA7_NAME)
};
GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID gSOFTPRES[MAX_SATA_PORTS] = {
  STRING_TOKEN(STR_SATA0_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA1_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA2_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA3_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA4_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA5_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA6_SOFT_PRESERVE_STATUS),
  STRING_TOKEN(STR_SATA7_SOFT_PRESERVE_STATUS)
};
#endif
// AMI_OVERRIDE_END - Workstation PCH can support maximum 8 ports.

GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN SataControllerConnected = FALSE;

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

VOID
SataDeviceCallBack (
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Class
  )
{
  EFI_STATUS                      Status;
  PCI_DEVICE_PATH                 *PciDevicePath;
  CHAR8                           *NewString;
  CHAR8                           *SoftPres;
  UINT8                           Index;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePathNode;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  UINT32                          SataPortIndex, IdeChannel;
  EFI_ATA_IDENTIFY_DATA           *IdentifyDriveInfo = NULL;
  UINT32                          BufferSize = 0;
  EFI_STRING_ID                   Token;
  EFI_STRING_ID                   SoftPresStatus;
  CHAR8                           ModelNumber[42];
  UINT64                          NumSectors = 0;
  UINT64                          DriveSizeInBytes = 0;
  UINT64                          RemainderInBytes = 0;
  UINT32                          DriveSizeInGB = 0;
  UINT32                          NumTenthsOfGB = 0;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  UINTN                           Segment;
  UINTN                           Bus;
  UINTN                           Device;
  UINTN                           Function;
//AMI_OVERRIDE_START >>> EIP476336 : BIOS doesn't show correct 4KN HDD capacity.
  UINT32                          SectorSize = 512;
//AMI_OVERRIDE_START <<<

  //
  // If SATA controller has been connected, just return
  //
  if (SataControllerConnected) {
    return;
  }

  DEBUG ((DEBUG_INFO, "Update SATA device info\n"));

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    HandleCount = 0;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID *) &PciIo
                    );
    ASSERT_EFI_ERROR(Status);

    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    if ((Bus == DEFAULT_PCI_BUS_NUMBER_PCH) &&
        (Device == PCI_DEVICE_NUMBER_PCH_SATA) &&
        (Function == PCI_FUNCTION_NUMBER_PCH_SATA)) {
      gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE);
    }
  }

  if (HandleBuffer) {
    FreePool (HandleBuffer);
  }

  //
  // Indicate SATA controller has been connected
  //
  SataControllerConnected = TRUE;

  //
  // Assume no line strings is longer than 256 bytes.
  //
  NewString = AllocatePool (0x100);
  ASSERT (NewString != NULL);
  if (NewString == NULL) {
    return;
  }

  SoftPres  = AllocatePool (0x40);
  ASSERT (SoftPres != NULL);
  if (SoftPres == NULL) {
    FreePool (NewString);
    return;
  }

  PciDevicePath = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskInfoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) HandleCount = 0;

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    ASSERT_EFI_ERROR(Status);

    DevicePathNode = DevicePath;
    while (!IsDevicePathEndType (DevicePathNode)) {
      if ((DevicePathNode->Type == HARDWARE_DEVICE_PATH) &&
          (DevicePathNode->SubType == HW_PCI_DP))
      {
        PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
        break;
      }

      DevicePathNode = NextDevicePathNode (DevicePathNode);
    }

    if (PciDevicePath == NULL) continue;

    if ((PciDevicePath->Device == PCI_DEVICE_NUMBER_PCH_SATA) &&
        (PciDevicePath->Function == PCI_FUNCTION_NUMBER_PCH_SATA)) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDiskInfoProtocolGuid,
                      (VOID **) &DiskInfo
                      );
      ASSERT_EFI_ERROR (Status);

      Status = DiskInfo->WhichIde (
                           DiskInfo,
                           &IdeChannel,
                           &SataPortIndex
                           );
// AMI_OVERRIDE_START - Workstation PCH can support maximum 8 ports.
      if (IdeChannel >= MAX_SATA_PORTS) {
          continue;
      }
// AMI_OVERRIDE_END - Workstation PCH can support maximum 8 ports.
      Token = gSATA[IdeChannel];
      SoftPresStatus = gSOFTPRES[IdeChannel];

      IdentifyDriveInfo = AllocatePool(sizeof (EFI_ATAPI_IDENTIFY_DATA));
      ASSERT (IdentifyDriveInfo != NULL);
      if (IdentifyDriveInfo == NULL) {
        return;
      }
      ZeroMem(IdentifyDriveInfo, sizeof (EFI_ATAPI_IDENTIFY_DATA));

      BufferSize = sizeof (EFI_ATAPI_IDENTIFY_DATA);
      Status = DiskInfo->Identify (
                           DiskInfo,
                           IdentifyDriveInfo,
                           &BufferSize
                           );
      ASSERT_EFI_ERROR (Status);

    } else {
      continue;
    }

    ZeroMem(ModelNumber, 42);
    CopyMem (ModelNumber, IdentifyDriveInfo->ModelName, 40);
    SwapEntries (ModelNumber, 40);
#if 0  // AMI_OVERRIDE_START - Modify for OEM to adjust Sata device name length
    ModelNumber[14] = '\0';           // Truncate it at 14 characters
#else
    ModelNumber[DEVICE_NAME_LENGTH] = '\0';           
#endif // AMI_OVERRIDE_END - Modify for OEM to adjust Sata device name length

    //
    // For HardDisk append the size. Otherwise display atapi
    //
    if (!(IdentifyDriveInfo->config & ATAPI_DEVICE)) {
      if (IdentifyDriveInfo->command_set_supported_83 & _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED) {
        NumSectors = *(UINT64 *) &IdentifyDriveInfo->maximum_lba_for_48bit_addressing;
      } else {
        NumSectors = (UINT64) *(UINT32 *) &IdentifyDriveInfo->user_addressable_sectors_lo;
      }
//AMI_OVERRIDE_START >>> EIP476336 : BIOS doesn't show correct 4KN HDD capacity.
//      DriveSizeInBytes = MultU64x32 (NumSectors, 512);
      if (IdentifyDriveInfo->phy_logic_sector_support & BIT12) {
        SectorSize = (UINT32)(IdentifyDriveInfo->logic_sector_size_lo+ \
                (IdentifyDriveInfo->logic_sector_size_hi << 16)) * 2;    
      }
      DriveSizeInBytes = MultU64x32 (NumSectors, SectorSize);
//AMI_OVERRIDE_START <<<
      //DriveSizeInGB is DriveSizeInBytes / 1 GB (1 Decimal GB = 10^9 bytes)
      DriveSizeInGB = (UINT32) DivU64x64Remainder (DriveSizeInBytes, 1000000000, &RemainderInBytes);
      //Convert the Remainder, which is in bytes, to number of tenths of a Decimal GB.
      NumTenthsOfGB = (UINT32) DivU64x64Remainder (RemainderInBytes, 100000000, NULL);

      AsciiSPrint(NewString, 0x100, "%a (%d.%dGB)", ModelNumber, DriveSizeInGB, NumTenthsOfGB);

      if ((IdentifyDriveInfo->serial_ata_capabilities != 0xFFFF) && (IdentifyDriveInfo->serial_ata_features_supported & 0x0040))
        AsciiSPrint(SoftPres, 0x40, "SUPPORTED");
      else
        AsciiSPrint(SoftPres, 0x40, "NOT SUPPORTED");
    } else {
      AsciiSPrint(NewString, 0x100, "%a ATAPI", ModelNumber);
      AsciiSPrint(SoftPres, 0x40, "  N/A  ");
    }

    InitString(
      HiiHandle,
      Token,
      L"%a",
      NewString
      );

    InitString(
      HiiHandle,
      SoftPresStatus,
      L"%a",
      SoftPres
      );

    if (IdentifyDriveInfo) {
      FreePool (IdentifyDriveInfo);
      IdentifyDriveInfo = NULL;
    }
  }

  if (HandleBuffer)
    FreePool (HandleBuffer);

  FreePool (NewString);
  FreePool (SoftPres);
}

VOID
DisplaySpiInformation (
  EFI_HII_HANDLE HiiHandle
  )
{
  UINT32      Flcomp;
  UINT32      FlashComponents;
  UINT32      Signature;
  UINT32      PchSpiBar0;
  UINTN       LpcBaseAddress;
  UINT32      Data32;
//AMI_OVERRIDE_START - EIP351659 : Protect platform critical Boot Service variables when platform is locked at EndOfDxe
#if 0
  EFI_STATUS  Status;
  UINTN       VariableSize;
  SETUP_DATA  SetupData;
  UINT32      SetupAttr;
#endif  
//AMI_OVERRIDE_END - EIP351659 : Protect platform critical Boot Service variables when platform is locked at EndOfDxe  

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  PchSpiBar0 = MmioRead32 (MmPciBase (
                              DEFAULT_PCI_BUS_NUMBER_PCH,
                              PCI_DEVICE_NUMBER_PCH_SPI,
                              PCI_FUNCTION_NUMBER_PCH_SPI)
                              + R_PCH_SPI_BAR0) &~(B_PCH_SPI_BAR0_MASK);

//AMI_OVERRIDE_START - EIP351659 : Protect platform critical Boot Service variables when platform is locked at EndOfDxe
#if 0
  SetupAttr = 0;
  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  &SetupAttr,
                  &VariableSize,
                  &SetupData
                  );
  if (EFI_ERROR (Status)) {
    SetupAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  }
  ASSERT_EFI_ERROR (Status);
#endif  
//AMI_OVERRIDE_END - EIP351659 : Protect platform critical Boot Service variables when platform is locked at EndOfDxe  

  // Read Descriptor offset 0x10 - To get Descriptor Signature
  Data32 = 0x0;
  Data32 = Data32 | V_PCH_SPI_FDOC_FDSS_FSDM | 0x0;  // Signature section 0x0000 + offset 0x0 which points to Descriptor offset 0x10
  MmioWrite32 (PchSpiBar0 + R_PCH_SPI_FDOC, Data32);
  Signature  = MmioRead32 (PchSpiBar0 + R_PCH_SPI_FDOD);
  DEBUG ((DEBUG_INFO, "\nSignature = 0x%.8x\n", Signature));

  // Read Descriptor offset 0x30 - To get supported features and R/W frequencies
  Data32 = 0x0;
  Data32 = Data32 | V_PCH_SPI_FDOC_FDSS_COMP | 0x0;  // Component section 0x1000 + offset 0x0 which points to Descriptor offset 0x30
  MmioWrite32 (PchSpiBar0 + R_PCH_SPI_FDOC, Data32);
  Flcomp  = MmioRead32 (PchSpiBar0 + R_PCH_SPI_FDOD);
  DEBUG ((DEBUG_INFO, "Flcomp = 0x%.8x\n", Flcomp));

  // Read Descriptor offset 0x14 - To get number of components
  Data32 = 0x0;
  Data32 = Data32 | V_PCH_SPI_FDOC_FDSS_FSDM | 0x4;  // Signature section 0x0000 + offset 0x4 which points to Descriptor offset 0x14
  MmioWrite32 (PchSpiBar0 + R_PCH_SPI_FDOC, Data32);
  FlashComponents  = MmioRead32 (PchSpiBar0 + R_PCH_SPI_FDOD);
  DEBUG ((DEBUG_INFO, "FlashComponents = 0x%.8x\n\n", FlashComponents));

  //
  // Dual Output Fast Read support
  //
  if ((Flcomp) & (BIT30)) {
    InitString (HiiHandle, STRING_TOKEN (STR_SPI_DUAL_OUTPUT_FAST_READ_SUPPORT_VALUE), L"%a", "Supported");
  } else {
    InitString (HiiHandle, STRING_TOKEN (STR_SPI_DUAL_OUTPUT_FAST_READ_SUPPORT_VALUE), L"%a", "Not supported");
  }

  //
  // Read ID and Read Status Clock Frequency
  //
  Data32 = (((Flcomp) & (B_PCH_SPI_FLCOMP_RIDS_FREQ)) >> 27);

  switch (Data32) {
    case V_PCH_SPI_FLCOMP_FREQ_48MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_READ_ID_STATUS_CLOCK_FREQUENCY_VALUE), L"%a", "48 MHz");
      break;
    case V_PCH_SPI_FLCOMP_FREQ_30MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_READ_ID_STATUS_CLOCK_FREQUENCY_VALUE), L"%a", "30 MHz");
      break;
    case V_PCH_SPI_FLCOMP_FREQ_17MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_READ_ID_STATUS_CLOCK_FREQUENCY_VALUE), L"%a", "17 MHz");
      break;
    default:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_READ_ID_STATUS_CLOCK_FREQUENCY_VALUE), L"%a", "Invalid Setting");
      break;
  }

  //
  // Write and Erase Clock Frequency
  //
  Data32 = (((Flcomp) & (B_PCH_SPI_FLCOMP_WE_FREQ)) >> 24);

  switch (Data32) {
    case V_PCH_SPI_FLCOMP_FREQ_48MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_WRITE_ERASE_CLOCK_FREQUENCY_VALUE), L"%a", "48 MHz");
      break;
    case V_PCH_SPI_FLCOMP_FREQ_30MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_WRITE_ERASE_CLOCK_FREQUENCY_VALUE), L"%a", "30 MHz");
      break;
    case V_PCH_SPI_FLCOMP_FREQ_17MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_WRITE_ERASE_CLOCK_FREQUENCY_VALUE), L"%a", "17 MHz");
      break;
    default:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_WRITE_ERASE_CLOCK_FREQUENCY_VALUE), L"%a", "Invalid Setting");
      break;
  }

  //
  // Fast Read Clock Frequency
  //
  Data32 = (((Flcomp) & (B_PCH_SPI_FLCOMP_FRCF_FREQ)) >> 21);

  switch (Data32) {
    case V_PCH_SPI_FLCOMP_FREQ_48MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_FAST_READ_CLOCK_FREQUENCY_VALUE), L"%a", "48 MHz");
      break;
    case V_PCH_SPI_FLCOMP_FREQ_30MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_FAST_READ_CLOCK_FREQUENCY_VALUE), L"%a", "30 MHz");
      break;
    case V_PCH_SPI_FLCOMP_FREQ_17MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_FAST_READ_CLOCK_FREQUENCY_VALUE), L"%a", "17 MHz");
      break;
    default:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_FAST_READ_CLOCK_FREQUENCY_VALUE), L"%a", "Invalid Setting");
      break;
  }

  //
  // Fast Read support
  //
  if ((Flcomp) & (B_PCH_SPI_FLCOMP_FR_SUP)) {
    InitString (HiiHandle, STRING_TOKEN (STR_SPI_FAST_READ_SUPPORT_VALUE), L"%a", "Supported");
  } else {
    InitString (HiiHandle, STRING_TOKEN (STR_SPI_FAST_READ_SUPPORT_VALUE), L"%a", "Not supported");
  }

  //
  // Read Clock Frequency
  //
  Data32 = (((Flcomp) & (B_PCH_SPI_FLCOMP_RC_FREQ)) >> 17);

  switch (Data32) {
    case V_PCH_SPI_FLCOMP_FREQ_48MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_READ_CLOCK_FREQUENCY_VALUE), L"%a", "48 MHz");
      break;
    case V_PCH_SPI_FLCOMP_FREQ_30MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_READ_CLOCK_FREQUENCY_VALUE), L"%a", "30 MHz");
      break;
    case V_PCH_SPI_FLCOMP_FREQ_17MHZ:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_READ_CLOCK_FREQUENCY_VALUE), L"%a", "17 MHz");
      break;
    default:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_READ_CLOCK_FREQUENCY_VALUE), L"%a", "Invalid Setting");
      break;
  }

  //
  // Number of components
  //
  Data32 = (((FlashComponents) & (B_PCH_SPI_FDBAR_NC )) >> N_PCH_SPI_FDBAR_NC);

//AMI_OVERRIDE_START - EIP351659 : Protect platform critical Boot Service variables when platform is locked at EndOfDxe
  switch (Data32) {
    case 0:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_NUMBER_OF_COMPONENTS_VALUE), L"%a", "1 Component");
//      SetupData.TwoComponents = 0;
      break;
    case 1:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_NUMBER_OF_COMPONENTS_VALUE), L"%a", "2 Components");
//      SetupData.TwoComponents = 1;
      break;
    default:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI_NUMBER_OF_COMPONENTS_VALUE), L"%a", "Reserved");
      break;
  }
#if 0  
  Status = gRT->SetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  SetupAttr,
                  sizeof (SETUP_DATA),
                  &SetupData
                  );
  ASSERT_EFI_ERROR (Status);
#endif
//AMI_OVERRIDE_END - EIP351659 : Protect platform critical Boot Service variables when platform is locked at EndOfDxe
  //
  // Display SPI Component 1 Density
  //

  // Execute if there are 2 components
  if (Data32 == 1){
    Data32 = ((Flcomp) & (B_PCH_SPI_FLCOMP_COMP1_MASK));

    switch (Data32) {
      case 0:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "512 KB");
        break;
      case 1:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "1 MB");
        break;
      case 2:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "2 MB");
        break;
      case 3:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "4 MB");
        break;
      case 4:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "8 MB");
        break;
      case 5:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "16 MB");
        break;
      case 6:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "32 MB");
        break;
      case 7:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "64 MB");
        break;
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "Reserved");
        break;
      default:
        InitString (HiiHandle, STRING_TOKEN (STR_SPI1_DENSITY_VALUE), L"%a", "Not present");
        break;
    }
  }
  //
  // Display SPI Component 0 Density
  //
  Data32 = ((Flcomp) & (B_PCH_SPI_FLCOMP_COMP0_MASK));

  switch (Data32) {
    case 0:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI0_DENSITY_VALUE), L"%a", "512 KB");
      break;
    case 1:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI0_DENSITY_VALUE), L"%a", "1 MB");
      break;
    case 2:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI0_DENSITY_VALUE), L"%a", "2 MB");
      break;
    case 3:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI0_DENSITY_VALUE), L"%a", "4 MB");
      break;
    case 4:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI0_DENSITY_VALUE), L"%a", "8 MB");
      break;
    case 5:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI0_DENSITY_VALUE), L"%a", "16 MB");
      break;
    case 6:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI0_DENSITY_VALUE), L"%a", "32 MB");
      break;
    case 7:
      InitString (HiiHandle, STRING_TOKEN (STR_SPI0_DENSITY_VALUE), L"%a", "64 MB");
      break;
    default:
      break;
  }
}

FVI_DATA *
GetPchHsioVersion (
  IN  CHAR8       *ComponentName
  )
{
  EFI_STATUS              Status;
  EFI_SMBIOS_HANDLE       SmbiosHandle;
  EFI_SMBIOS_PROTOCOL     *Smbios;
  EFI_SMBIOS_TABLE_HEADER *Record;
  FVI_HEADER              *FviHeader;
  FVI_DATA                *FviData;
  FVI_STRINGS             *FviString;
  UINT8                   Index;
  UINT8                   Index2;
  UINTN                   StringSize;
  CHAR8                   *StringArray;
  CHAR8                   StringPtr[SMBIOS_STRING_MAX_LENGTH];
  UINT8                   ComponentNum;

  FviData    = NULL;
  FviHeader  = NULL;
  FviString  = NULL;
  StringSize = 0;

  if (ComponentName == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }
    if (Record->Type == PcdGet8 (PcdSmbiosOemTypeFirmwareVersionInfo)) {
      FviHeader = (FVI_HEADER *) Record;
      FviString = (FVI_STRINGS *) ((UINT8 *) FviHeader + FviHeader->SmbiosHeader.Length);
      //
      // Search string of "PCH_FVI_STRING" in Smbios Type "0xDD"
      //
      if (AsciiStrnCmp ((CHAR8 *) &FviString->ComponentName, PCH_FVI_STRING, AsciiStrLen (PCH_FVI_STRING)) == 0) {
        StringArray = (CHAR8 *) FviString;

        for (Index = 0; Index < (FviHeader->Count * FVI_NUMBER_OF_STRINGS); Index++) {

          StringSize = AsciiStrSize ((const CHAR8 *) StringArray);
          AsciiStrCpyS (StringPtr, StringSize, (const CHAR8 *) StringArray);
          StringArray = StringArray + StringSize;
          //
          // Search HSIO string in FviString array to get the Component Number (Index + 1)
          //
          if (AsciiStrnCmp (StringPtr, ComponentName, AsciiStrLen (ComponentName)) == 0) {
            FviData = (FVI_DATA *) ((UINT8 *) FviHeader + sizeof (FVI_HEADER));
            ComponentNum = Index + 1;
            //
            // Search Component Number in FviData array, if find out we got the corresponding FviData
            //
            for (Index2 = 0; Index2 < FviHeader->Count; Index2++) {
              if (FviData[Index2].ComponentName == ComponentNum) {
                return &FviData[Index2];
              }
            }
          }
        }
      }
    }
  } while (Status == EFI_SUCCESS);

  return NULL;
}

#if 0 // AMI_OVERRIDE_START - Implement it in EndofDxeCallback() of PlatformInitDxe.c   
VOID
BiosIshDataPass (
  VOID
  )
{
  static CONST CHAR8      FileName[] = "bios2ish";

  DEBUG ((DEBUG_INFO, "Sending PDT Unlock Message\n"));
  HeciPdtUnlockMsg (FileName);

}
#endif // AMI_OVERRIDE_END - Implement it in EndofDxeCallback() of PlatformInitDxe.c   

//
// Compares xHCI strap values to check if SSIC is disabled in straps.
// If disabled options will be hidden from setup.
//
VOID
UpdateSsicData (
  SETUP_VOLATILE_DATA  *VolatileData
  )
{
  UINT32                        XhciUsb3Mode;
  UINT32                        XhciMmioBase;
  UINTN                         XhciPciMmBase;

  XhciPciMmBase = MmPciBase (
    PCI_BUS_NUMBER_PCH_XHCI,
    PCI_DEVICE_NUMBER_PCH_XHCI,
    PCI_FUNCTION_NUMBER_PCH_XHCI
    );

  XhciMmioBase = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE);

  XhciUsb3Mode = MmioRead32 (XhciMmioBase + R_PCH_XHCI_STRAP2);

  VolatileData->UsbSsicEnabled[0] = XhciUsb3Mode & BIT1;
  VolatileData->UsbSsicEnabled[1] = XhciUsb3Mode & BIT2;

  DEBUG ((DEBUG_INFO, "PCH_XHCI_STRAP2 = %x\n", XhciUsb3Mode));
}

STATIC
BOOLEAN
InitializeSataInterfaceMode (
  IN OUT  PCH_SETUP  *PchSetup
  )
{
  UINTN    PciSataRegBase;
  UINT8    SataMode;
  UINT8    SataInterfaceMode;
  BOOLEAN  UpdateSetupData;

  UpdateSetupData = FALSE;

  PciSataRegBase = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SATA,
                     PCI_FUNCTION_NUMBER_PCH_SATA
                     );
  SataMode     = MmioRead8 (PciSataRegBase + R_PCI_SCC_OFFSET);

  switch (SataMode) {
    case PCI_CLASS_MASS_STORAGE_RAID:
      SataInterfaceMode = SATA_MODE_RAID;
      break;
    case PCI_CLASS_MASS_STORAGE_SATADPA:
    default:
      SataInterfaceMode = SATA_MODE_AHCI;
      break;
  }

  if (PchSetup->SataInterfaceMode != SataInterfaceMode) {
    DEBUG ((DEBUG_INFO, "Mismatch detected between current SATA controller mode and user settings, changing PchSetup to reflect actual settings\n"));
    PchSetup->SataInterfaceMode = SataInterfaceMode;
    UpdateSetupData = TRUE;
  }

  return UpdateSetupData;

}

STATIC
BOOLEAN
IsOptaneSupported (
  VOID
  )
{
  UINT8   PchGeneration;
  UINT8   CpuGeneration;
  UINT16  LpcDeviceId;

  PchGeneration = GetPchGeneration ();
  CpuGeneration = GetCpuGeneration ();
  LpcDeviceId   = GetLpcDid ();

  if (CpuGeneration == EnumKblCpu) {
    if ((PchGeneration == KblPch) || (LpcDeviceId == V_SKL_PCH_H_LPC_DEVICE_ID_SVR_0) || (LpcDeviceId == V_PCH_LP_LPC_DEVICE_ID_MB_9)) {
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    return FALSE;
  }

}

STATIC
VOID
InitializeRstModeStrings (
  IN  EFI_HII_HANDLE  HiiHandle,
  IN  RST_MODE  RstMode
  )
{

  if (RstMode != RstUnsupported) {
    if (IsOptaneSupported ()) {
      if (RstMode == RstPremium) {
       InitString (HiiHandle, STRING_TOKEN (STR_SATA_RAID), L"Intel RST Premium With Intel Optane System Acceleration");
      } else {
       InitString (HiiHandle, STRING_TOKEN (STR_SATA_RAID), L"Intel RST With Intel Optane System Acceleration");
      }
    } else {
      if (RstMode == RstPremium) {
       InitString (HiiHandle, STRING_TOKEN (STR_SATA_RAID), L"Intel RST Premium");
      } else {
       InitString (HiiHandle, STRING_TOKEN (STR_SATA_RAID), L"Intel RST");
      }
    }
  } else {
    InitString(
      HiiHandle,
      STRING_TOKEN(STR_SATA_MODE_SELECTION_HELP),
      L"Determines how SATA controller(s) operate. \nThis PCH SKU dosen't support RST feature"
      );
  }

}

VOID
InitSBStrings (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  )
{
  UINTN                           Index;
  UINT16                          LpcDid;
  EFI_STATUS                      Status;
  UINTN                           VariableSize;
  UINT32                          VariableAttr;
  PCH_SETUP                       PchSetup;
  SETUP_VOLATILE_DATA             VolatileData;
  UINT32                          SetupVolVarAttr;
  BOOLEAN                         GbePresent;
  UINT32                          GbePciePort;
  UINTN                           SataPortsNo;
  BOOLEAN                         UpdateSetupData;
  FVI_DATA                        *FviData;
  CHAR8                           *ComponentName;
  PLATFORM_INFO                   *PlatformInfo;
  CHAR8                           HsioString[4];
  PCH_INFO_PROTOCOL               *PchInfoProtocol;
  CHAR8                           Buffer[32];
  UINT32                          BufferSize;
  RST_MODE                        RstMode;
// AMI_OVERRIDE_START - Set SetupVolatileData for Rst remap menu. 
  VOID                            *Hob;
  PCIE_STORAGE_INFO_HOB           *PcieStorageInfoHob;  
  
  Hob = NULL;
  PcieStorageInfoHob = NULL;
  Hob = GetFirstGuidHob (&gPchPcieStorageDetectHobGuid);
  if (Hob != NULL) {
    PcieStorageInfoHob = (PCIE_STORAGE_INFO_HOB *) GET_GUID_HOB_DATA (Hob);
  }
// AMI_OVERRIDE_END - Set SetupVolatileData for Rst remap menu.

  Status = gBS->LocateProtocol (&gPchInfoProtocolGuid, NULL, (VOID **) &PchInfoProtocol);
  ASSERT_EFI_ERROR (Status);

  LpcDid = GetLpcDid ();
#if 0  // AMI_OVERRIDE_START - For our Setup menu design.
  if (Class == ADVANCED_FORM_SET_CLASS) {
#else
   if (Class == CHIPSET_FORM_SET_CLASS) {
#endif // AMI_OVERRIDE_END - For our Setup menu design.
      
    DEBUG ((DEBUG_INFO, "<InitSBStrings>"));

    UpdateSetupData          = FALSE;

    Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
    ASSERT_EFI_ERROR (Status);

    GbePresent   = PchIsGbePresent ();
    GbePciePort  = PchGetGbePortNumber ();

    VariableSize = sizeof (SETUP_VOLATILE_DATA);
    Status = gRT->GetVariable (
                    L"SetupVolatileData",
                    &gSetupVariableGuid,
                    &SetupVolVarAttr,
                    &VariableSize,
                    &VolatileData
                    );
    ASSERT_EFI_ERROR (Status);

    VariableSize = sizeof (PCH_SETUP);
    Status = gRT->GetVariable (
                    L"PchSetup",
                    &gPchSetupVariableGuid,
                    &VariableAttr,
                    &VariableSize,
                    &PchSetup
                    );
    ASSERT_EFI_ERROR (Status);

    RstMode = GetSupportedRstMode ();
    UpdateSetupData = InitializeSataInterfaceMode (&PchSetup);
    InitializeRstModeStrings (HiiHandle, RstMode);

    //
    // Update the volatile variable for reference by SETUP Sata device form.
    //
    if (RstMode != RstUnsupported) {
      VolatileData.RstAvailable   = TRUE;
    }
    VolatileData.PchSku         = GetPchSkuType ();
    VolatileData.GbeAvailable   = GbePresent;
    VolatileData.GbePciePortNum = (UINT8) GbePciePort;

    //
    // Get PCH PCIE ports configuration details
    //
    VolatileData.PcieControllerCfg1   = PchInfoProtocol->PcieControllerCfg1;
    VolatileData.PcieControllerCfg2   = PchInfoProtocol->PcieControllerCfg2;
    VolatileData.PcieControllerCfg3   = PchInfoProtocol->PcieControllerCfg3;
    VolatileData.PcieControllerCfg4   = PchInfoProtocol->PcieControllerCfg4;
    VolatileData.PcieControllerCfg5   = PchInfoProtocol->PcieControllerCfg5;
    VolatileData.PcieControllerCfg6   = PchInfoProtocol->PcieControllerCfg6;
    
// AMI_OVERRIDE_START - Set SetupVolatileData for Rst remap menu.     
    //
    // Set SetupVolatileData for Rst remap menu
    //
    for (Index = 0; Index < PCH_MAX_PCIE_ROOT_PORTS; Index++) {
      if (PcieStorageInfoHob != NULL) {
          VolatileData.PcieStorageMap[Index] = PcieStorageInfoHob->PcieStorageLinkWidth[Index];
          VolatileData.PcieStorageProgrammingInterface[Index] = PcieStorageInfoHob->PcieStorageProgrammingInterface[Index];
      } else {
          VolatileData.PcieStorageMap[Index] = 0;
          VolatileData.PcieStorageProgrammingInterface[Index] = 0;
      }
    }    

    for (Index = 0; Index < PCH_MAX_PCIE_CONTROLLERS; Index++) {
      if (PcieStorageInfoHob != NULL) {
          VolatileData.CycleRouterMap[Index] = PcieStorageInfoHob->RstCycleRouterMap[Index];
        DEBUG ((DEBUG_INFO, "CycleRouterMap[%d] = %d\n", Index, VolatileData.CycleRouterMap[Index]));
      } else {
          VolatileData.CycleRouterMap[Index] = 0;
      }
    }
// AMI_OVERRIDE_END - Set SetupVolatileData for Rst remap menu.     

#if 0 // AMI_OVERRIDE_START - Implement it in EndofDxeCallback() of PlatformInitDxe.c   
    // Send PDT Unlock Message to ISH
    if (PchSetup.PchIshPdtUnlock == 1) {
      BiosIshDataPass ();
      //Set the value back to 0 so the user needs to manually enable the option to send the next PDT unlock message
      PchSetup.PchIshPdtUnlock = 0;
      UpdateSetupData = TRUE;
    }
#endif // AMI_OVERRIDE_END - Implement it in EndofDxeCallback() of PlatformInitDxe.c   

    //
    // Check Gbe Region to determine if hide LAN controller setup option
    //
    if (!GbePresent && PchSetup.PchLan == 1) {
      PchSetup.PchLan = 0;
      UpdateSetupData = TRUE;
    }

    //
    // Check Ssic straps to check if setup options should be hidden
    //
    UpdateSsicData (&VolatileData);

    SataPortsNo = GetPchMaxSataPortNum ();
    for (Index = 0; Index < SataPortsNo; Index++) {
      ///
      /// Check the GPIO Pin is set as used for native function not a normal GPIO
      ///
      if (GpioIsSataResetPortInGpioMode (Index)) {
        if (PchSetup.SataMechanicalSw[Index] == 1) {

          DEBUG ((DEBUG_INFO,
                  "BIOS must set the SATA%0xGP GPIO pin to native function if Inter Lock Switch is enabled!\n",
                  Index));

          PchSetup.SataMechanicalSw[Index] = 0;
          UpdateSetupData = TRUE;
        }
        VolatileData.SataMpsPresent[Index]  = 0;
      } else {
        VolatileData.SataMpsPresent[Index]  = 1;
      }
    }

    //
    // Update EnableDebugSerialIoUartNumber to now show the related SerialIo UART controller
    //
    if (PcdGet8 (PcdSerialIoUartDebugEnable) != 0) {
      VolatileData.EnableDebugSerialIoUartNumber = 1 + PcdGet8 (PcdSerialIoUartNumber);
    }

    //
    // Update Setup Data if needed
    //
    if (UpdateSetupData == TRUE) {
      Status = gRT->SetVariable (
                      L"PchSetup",
                      &gPchSetupVariableGuid,
                      VariableAttr,
                      sizeof (PCH_SETUP),
                      &PchSetup
                      );
      ASSERT_EFI_ERROR (Status);
    }

    Status = gRT->SetVariable (
                    L"SetupVolatileData",
                    &gSetupVariableGuid,
                    SetupVolVarAttr,
                    sizeof (SETUP_VOLATILE_DATA),
                    &VolatileData
                    );
    ASSERT_EFI_ERROR (Status);

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_PCIE_LAN_VALUE),
      (GbePciePort != 0) ? L"%d" : L"Disabled",
      GbePciePort
      );

    SataDeviceCallBack (HiiHandle, 0);
  } // ADVANCED_FORM_SET_CLASS

  if (Class == MAIN_FORM_SET_CLASS) {
    DEBUG ((DEBUG_INFO, "<InitSBStrings>"));

    BufferSize = sizeof (Buffer);
    PchGetSeriesStr (GetPchSeries (), Buffer, &BufferSize);
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_CHIP_SB_VALUE),
      L"%a",
      Buffer
      );

    BufferSize = sizeof (Buffer);
    PchGetSteppingStr (PchStepping (), Buffer, &BufferSize);
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_CHIP_SB_REV_VALUE),
      L"%a",
      Buffer
      );

    BufferSize = sizeof (Buffer);
    PchGetSkuStr (LpcDid, Buffer, &BufferSize);
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_CHIP_SB_SKU_VALUE),
      L"%a",
      Buffer
      );

    FviData = NULL;
    ComponentName = NULL;

    if (GetPchGeneration () == SklPch) {
      switch (PchStepping ())
      {
        case PchHB0:
        case PchHC0:
          ComponentName = SKL_PCH_HBX_HSIO_STRING;
          break;
        case PchHD0:
        case PchHD1:
          ComponentName = SKL_PCH_HDX_HSIO_STRING;
          break;
        case PchLpB0:
        case PchLpB1:
          ComponentName = SKL_PCH_LPBX_HSIO_STRING;
          break;
        case PchLpC0:
        case PchLpC1:
          ComponentName = SKL_PCH_LPCX_HSIO_STRING;
          break;
        default:
          ComponentName = NULL;
          break;
      }
    } else {
      switch (PchStepping ())
      {
        case KblPchHA0:
          ComponentName = KBL_PCH_HAX_HSIO_STRING;
          break;
        default:
          ComponentName = NULL;
          break;
      }
    }
    if (ComponentName != NULL) {
      FviData = GetPchHsioVersion (ComponentName);
      if (FviData != NULL) {
        AsciiSPrint (HsioString, 0x4, "%d", FviData->Version.MajorVersion);

        InitString (
          HiiHandle,
          STRING_TOKEN (STR_CHIP_HSIO_REV_VALUE),
          L"%a",
          HsioString
          );
      } else {
        DEBUG ((DEBUG_INFO, "Get PCH HSIO Version failure!\n"));
      }
    }

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_CHIP_SB_PACKAGE_VALUE),
      L"%a",
      "Not Implemented Yet"
      );
    //
    // SPI controller information
    //
    DisplaySpiInformation(HiiHandle);
  } // MAIN_FORM_SET_CLASS
}

