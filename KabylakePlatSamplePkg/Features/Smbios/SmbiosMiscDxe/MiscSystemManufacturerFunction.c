/** @file
  System Information boot time changes.
  SMBIOS type 1.

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
#include <IndustryStandard/Pci22.h>
#include <Library/MmPciLib.h>
#include <Library/HobLib.h>
#include <PchAccess.h>
#include <Library/CpuPlatformLib.h>

/**
  Get UUID from Gbe region MAC

  @param[out]   UuidBuffer    Buffer for the MAC

  @retval       EFI_SUCCESS   successful
  @retval       Other Vals    something wrong

**/
EFI_STATUS
GetGbeUuid (
  UINT8 *UuidBuffer
  )
{
  EFI_STATUS        Status;
  PCH_SPI_PROTOCOL  *SpiProtocol;

  if (UuidBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Locate the SPI protocol.
  //
  Status = gBS->LocateProtocol (
                  &gPchSpiProtocolGuid,
                  NULL,
                  (VOID **) &SpiProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Read MAC address from GbE region
  //
  Status = SpiProtocol->FlashRead (
                          SpiProtocol,
                          FlashRegionGbE,
                          (UINT32)0,
                          (UINT32)6,
                          UuidBuffer
                          );
  return Status;
}

/**
  This function get the next bus range of given address space descriptors.
  It also moves the pointer backward a node, to get prepared to be called
  again.

  @param[in] Descriptors          points to current position of a serial of address space
                                  descriptors
  @param[in] MinBus               The lower range of bus number
  @param[in] MaxBus               The upper range of bus number
  @param[in] IsEnd                Meet end of the serial of descriptors

  @retval EFI_SUCCESS             The command completed successfully

**/
EFI_STATUS
PciGetNextBusRange (
  IN OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR  **Descriptors,
  OUT    UINT16                             *MinBus,
  OUT    UINT16                             *MaxBus,
  OUT    BOOLEAN                            *IsEnd
  )
{
  *IsEnd = FALSE;

  //
  // When *Descriptors is NULL, Configuration() is not implemented, so assume
  // range is 0~PCI_MAX_BUS
  //
  if ((*Descriptors) == NULL) {
    *MinBus = 0;
    *MaxBus = PCI_MAX_BUS;
    return EFI_SUCCESS;
  }
  //
  // *Descriptors points to one or more address space descriptors, which
  // ends with a end tagged descriptor. Examine each of the descriptors,
  // if a bus typed one is found and its bus range covers bus, this handle
  // is the handle we are looking for.
  //
  if ((*Descriptors)->Desc == ACPI_END_TAG_DESCRIPTOR) {
    *IsEnd = TRUE;
  }

  while ((*Descriptors)->Desc != ACPI_END_TAG_DESCRIPTOR) {
    if ((*Descriptors)->ResType == ACPI_ADDRESS_SPACE_TYPE_BUS) {
      *MinBus = (UINT16) (*Descriptors)->AddrRangeMin;
      *MaxBus = (UINT16) (*Descriptors)->AddrRangeMax;
    }

    (*Descriptors)++;
  }

  return EFI_SUCCESS;
}

/**
  This function gets the protocol interface from the given handle, and
  obtains its address space descriptors.

  @param[in]  Handle              The PCI_ROOT_BRIDIGE_IO_PROTOCOL handle
  @param[in]  IoDev               Handle used to access configuration space of PCI device
  @param[in]  Descriptors         Points to the address space descriptors

  @retval     EFI_SUCCESS         The command completed successfully
**/
EFI_STATUS
PciGetProtocolAndResource (
  IN  EFI_HANDLE                            Handle,
  OUT EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       **IoDev,
  OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     **Descriptors
  )
{
  EFI_STATUS  Status;

  //
  // Get inferface from protocol
  //
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  (VOID **) IoDev
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Call Configuration() to get address space descriptors
  //
  Status = (*IoDev)->Configuration (*IoDev, (VOID **) Descriptors);
  if (Status == EFI_UNSUPPORTED) {
    *Descriptors = NULL;
    return EFI_SUCCESS;

  } else {
    return Status;
  }
}

/**
  Check if WLAN with given device Id is on the list of supported adapters

  @param[in]    DeviceId    PCI Device ID to be checked

  @retval       TRUE        device supported
  @retval       FALSE       not supported

**/
BOOLEAN
IsSupportedIntelWlanDevice (
  IN  UINT16     DeviceId
  )
{
  UINT16 SupportedIntelWlanDevices[] = SUPPORTED_INTEL_WLAN_DEVICES;
  UINTN  Index;

  for (Index = 0; Index < sizeof(SupportedIntelWlanDevices)/sizeof(UINT16); Index++) {
    if (DeviceId == SupportedIntelWlanDevices[Index]) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Find WLAN device on PCI

  @param[out]     Bus           Updated with PCI bus number of discovered WLAN device
  @param[out]     Device        Updated with PCI device number of discovered WLAN device

  @retval         EFI_SUCCESS   successful
  @retval         Other Vals    something wrong

**/
EFI_STATUS
FindCompatibleWlanDevice (
  OUT UINT16     *Bus,
  OUT UINT16     *Device
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             NumberOfHandles;
  UINTN                             Index;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *IoDev;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;
  UINT16                            MinBus;
  UINT16                            MaxBus;
  BOOLEAN                           IsEnd;
  UINT16                            TempBus;
  UINT16                            TempDev;
  PCI_DEVICE_INDEPENDENT_REGION     PciHeader;

  //
  // Get all instances of PciRootBridgeIo
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // For each handle, which decides a segment and a bus number range,
  // enumerate all devices on it.
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = PciGetProtocolAndResource (
              HandleBuffer[Index],
              &IoDev,
              &Descriptors
              );
    if (EFI_ERROR (Status)) {
      FreePool (HandleBuffer);
      return Status;
    }

    //
    // No document say it's impossible for a RootBridgeIo protocol handle
    // to have more than one address space descriptors, so find out every
    // bus range and for each of them do device enumeration.
    //
    while (TRUE) {
      Status = PciGetNextBusRange (&Descriptors, &MinBus, &MaxBus, &IsEnd);

      if (EFI_ERROR (Status)) {
        FreePool (HandleBuffer);
        return Status;
      }
      //
      // Wifi device will not be on Bus 0.
      //
      if (MinBus == 0 && MaxBus == 0) {
        DEBUG((DEBUG_INFO, "SMBIOS type 1: Find Wifi- PCIe MaxBus is 0, no wifi device exists on Bus 0, no wifi device found\n"));
        return EFI_NOT_FOUND;
      }
      if (IsEnd) {
        break;
      }
      //
      // Wifi device will not be on Bus 0. Increase MinBus to Bus 1 to optimize search.
      //
      if (MinBus == 0) {
        MinBus = 1;
      }
      for (TempBus = MinBus; TempBus <= MaxBus; TempBus++) {
        //
        // Enumerate all devices on a bus
        //
        for (TempDev = 0; TempDev <= PCI_MAX_DEVICE; TempDev++) {
          SetMem (&PciHeader, sizeof(PCI_DEVICE_INDEPENDENT_REGION), 0);

          //
          // Read vendor and device id
          //
          IoDev->Pci.Read (
                      IoDev,
                      EfiPciWidthUint32,
                      EFI_PCI_ADDRESS (TempBus, TempDev, 0, PCI_VENDOR_ID_OFFSET),
                      3,
                      &PciHeader.VendorId
                      );

          //
          // Find a supported device on a hard-coded list
          // Or, since this is our internal implementation, check PCI class ID only to allow using more devices for validation purposes
          //
          if (PciHeader.VendorId == V_PCH_INTEL_VENDOR_ID &&
             (IsSupportedIntelWlanDevice(PciHeader.DeviceId) ||
              (PciHeader.ClassCode[2] == PCI_CLASS_NETWORK && PciHeader.ClassCode[1] == PCI_CLASS_NETWORK_OTHER))) {
            *Bus = TempBus;
            *Device = TempDev;
            return EFI_SUCCESS;
          }
        }
      }
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Get UUID from WLAN MAC.

  @param[in]  UuidBuffer    Buffer for the MAC

  @retval     EFI_SUCCESS   successful
  @retval     Other Vals    something wrong

**/
EFI_STATUS
GetWlanUuid (
  UINT8 *UuidBuffer,
  UINT16 Bus,
  UINT16 Dev
  )
{
  UINT32            Data32;
  UINTN             WlanBaseAddress;

  if (UuidBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Read MAC from WLAN pci config space offset 0x144 and 0x148
  //
  WlanBaseAddress = MmPciBase (Bus, Dev, 0);
  Data32 = MmioRead32 (WlanBaseAddress + 0x148);
  *UuidBuffer = (UINT8)(Data32 >> 24);
  *(UuidBuffer+1) = (UINT8)(Data32 >> 16);
  *(UuidBuffer+2) = (UINT8)(Data32 >> 8);

  Data32 = MmioRead32 (WlanBaseAddress + 0x144);
  *(UuidBuffer+3) = (UINT8)(Data32 >> 16);
  *(UuidBuffer+4) = (UINT8)(Data32 >> 8);
  *(UuidBuffer+5) = (UINT8)Data32;

  return EFI_SUCCESS;
}

/**
  This function makes boot time changes to the contents of the
  MiscSystemManufacturer (Type 1).

  @param[in]    RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval       EFI_SUCCESS                All parameters were valid.
  @retval       EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval       EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscSystemManufacturer)
{
  CHAR8                       *OptionalStrStart;
  CHAR16                      UuidVarName[sizeof(SMBIOS_T1_SYSTEM_UUID_VARIABLE)+1];
  UINTN                       UuidVarSize;
  UINT8                       UuidBuffer[6];
  UINT8                       *UuidPtr;
  UINT8                       *UuidPtrTemp;
  UINT8                       i;
  UINTN                       StringLength;
  UINTN                       StringNumber;
  EFI_STATUS                  Status;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  SMBIOS_TABLE_TYPE1          *SmbiosRecord;
  SMBIOS_TABLE_TYPE01_STRINGS SmbiosTableType01Strings;
  CHAR16                      *StrBufferStart;
  CHAR16                      *StrBufferPtr;
  UINT16                      Bus;
  UINT16                      Dev;
  PLATFORM_INFO               *PlatformInfo;
  UINT8                       UuidRetry;
  UINTN                       StrBuffSize;
  UINTN                       OptionalStrBufferSize;
//AMI_OVERRIDE_START >>> Move local variable definition to the top of the function to avoid build errors.
  CPU_OVERRIDE_IDENTIFIER     CpuIdentifier;
//AMI_OVERRIDE_END <<< Move local variable definition to the top of the function to avoid build errors.
  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "(MiscSystemManufacturer) Entry.\n"));
  ZeroMem (&SmbiosTableType01Strings, sizeof(SMBIOS_TABLE_TYPE01_STRINGS));
  StrCpyS (UuidVarName, sizeof (UuidVarName) / sizeof (CHAR16), SMBIOS_T1_SYSTEM_UUID_VARIABLE);
  StringLength    = 0;
  StringNumber    = 0;
  UuidPtr         = NULL;
  UuidPtrTemp     = NULL;
  StrBufferStart  = AllocateZeroPool (6 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16));
  StrBufferPtr    = StrBufferStart;
  Bus             = 0;
  Dev             = 0;
  UuidRetry       = 0;
  StrBuffSize     = (6 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16)) / sizeof (CHAR16);
//AMI_OVERRIDE_START >>> Move local variable definition to the top of the function to avoid build errors.
  //CPU_OVERRIDE_IDENTIFIER CpuIdentifier;
//AMI_OVERRIDE_END <<< Move local variable definition to the top of the function to avoid build errors.
  //
  // Get CpuIdentifier to identify Amberlake system
  //
  CpuIdentifier = GetCpuIdentifier ();

  //
  // Initialize SMBIOS Tables Type01 strings
  //

  SmbiosStrInit (&SmbiosTableType01Strings.Manufacturer, SMBIOS_MISC_TYPE01_MANUFACTURER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);

  if ((CpuIdentifier == EnumAmlY5Watt22CpuId) || (CpuIdentifier == EnumAmlY7Watt22CpuId) || (CpuIdentifier == EnumAmlY7Watt42CpuId) ) {
    SmbiosStrInit (&SmbiosTableType01Strings.ProductName, SMBIOS_MISC_TYPE01_PRODUCT_NAME2, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
    SmbiosStrInit (&SmbiosTableType01Strings.Family, SMBIOS_MISC_TYPE01_FAMILY2, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
  } else {
    SmbiosStrInit (&SmbiosTableType01Strings.ProductName, SMBIOS_MISC_TYPE01_PRODUCT_NAME, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
    SmbiosStrInit (&SmbiosTableType01Strings.Family, SMBIOS_MISC_TYPE01_FAMILY, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
  }
  SmbiosStrInit (&SmbiosTableType01Strings.Version , SMBIOS_MISC_TYPE01_VERSION, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
  SmbiosStrInit (&SmbiosTableType01Strings.SerialNumber, SMBIOS_MISC_TYPE01_SERIAL_NUMBER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
  SmbiosStrInit (&SmbiosTableType01Strings.SkuNumber, SMBIOS_MISC_TYPE01_SKU_NUMBER, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);

  //
  // Two zeros following the last string.
  //
  OptionalStrBufferSize = StringLength + StringNumber + 1;
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE1) + OptionalStrBufferSize);
  if (SmbiosRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE1) + OptionalStrBufferSize);
  CopyMem (SmbiosRecord, RecordData, sizeof (SMBIOS_TABLE_TYPE1));

  //
  // Fill in UUID by first copying default GUID.  Then update last six bytes
  // with MAC address to make it unique.
  //
  UuidPtr = (UINT8 *) &SmbiosRecord->Uuid;
  UuidPtrTemp = UuidPtr;
  UuidVarSize = sizeof(EFI_GUID);
  Status = gRT->GetVariable (
                  UuidVarName,
                  &gEfiSmbiosVariableGuid,
                  NULL,
                  &UuidVarSize,
                  UuidPtr
                  );
  //
  // If variable is not present or we are booting with defaults, re-create the UUID.
  // Use the following methods in order of priority to create the UUID:
  // 1) UUID based on WLAN MAC Address
  // 2) UUID based on LAN MAC Address
  // 3) Default UUID
  //

  //
  // If the current UUID Matches the default UUID, then re-evaluate to find WLAN MAC
  //
  if (!EFI_ERROR(Status) && (UuidPtrTemp != NULL)) {
    if (CompareMem(UuidPtr,UuidPtrTemp,sizeof (EFI_GUID)) == 0x0) {
      DEBUG ((DEBUG_INFO, "Default UUID detected. Re-search for WLAN or LAN to generate UUID\n"));
      UuidRetry = 0x1;
    }
  }

  if (EFI_ERROR(Status) || (GetBootModeHob() == BOOT_WITH_DEFAULT_SETTINGS) || (UuidRetry == 0x1)) {
    //
    // Set system GUID. If there is no MAC address, use default UUID set when structure was defined
    //
    Status = FindCompatibleWlanDevice(&Bus, &Dev);
    if (Status == EFI_SUCCESS) { // Check for Wifi device
      Status = GetWlanUuid(UuidBuffer, Bus, Dev);
      DEBUG ((DEBUG_INFO, "SMBIOS type 1: UUID will be derived from WLAN MAC\n"));
    } else if (PchIsGbeRegionValid ()) { // Check for LAN device
      Status = GetGbeUuid (UuidBuffer);
      DEBUG ((DEBUG_INFO, "SMBIOS type 1: UUID will be derived from GbE MAC\n"));
    } else { //No device with MAC found- use default UUID
      Status = EFI_NOT_FOUND;
      DEBUG ((DEBUG_INFO, "SMBIOS type 1: No WLAN or LAN detected. UUID set as default.\n"));
    }

    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "UUID = "));
      for (i = 0; i < sizeof (EFI_GUID); i++) {
        UuidPtr[i] = UuidBuffer[i % 6];
        DEBUG ((DEBUG_INFO, "%02x ", UuidPtr[i]));
      }
      DEBUG ((DEBUG_INFO, "\n"));
    }
    //
    // Store UUID in NVRAM
    //
    Status = gRT->SetVariable (
                    UuidVarName,
                    &gEfiSmbiosVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    UuidVarSize,
                    UuidPtr
                    );
    ASSERT_EFI_ERROR(Status);
  }
  //
  // Update wakeUpType field
  //
  Status = gBS->LocateProtocol (
                  &gPlatformInfoProtocolGuid,
                  NULL,
                  (VOID **) &PlatformInfo
                  );
  ASSERT_EFI_ERROR(Status);

  SmbiosRecord->WakeUpType = PlatformInfo->WakeUpType;
  DEBUG ((DEBUG_INFO, "SMBIOS type 1: WakeUpType  %x \n",SmbiosRecord->WakeUpType));
  //
  // Update SMBIOS Tables Type01 strings
  //
  OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
  SmbiosStringsUpdate ((CHAR16 **) &SmbiosTableType01Strings, (UINT8 *) OptionalStrStart, StringNumber, OptionalStrBufferSize);

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  FreePool(StrBufferStart);
  return Status;
}
