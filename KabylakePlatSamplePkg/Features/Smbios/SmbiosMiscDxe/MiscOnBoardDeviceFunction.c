/** @file
  On Board Device Information boot time changes.
  SMBIOS type 10.

  @par Revision Reference:
  SMBIOS Specification version 2.8.0 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
  Unified Extensible Firmware Interface (UEFI) Specifications: http://www.uefi.org/specs/

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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
#include <IndustryStandard/Pci30.h>
#include <PchAccess.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo;

/**
  Get the enabling/disabling setting through PciBridgeIo and return that.

  @param[out]   OnBoardDeviceType

**/
VOID
GetOnBoardDeviceType (
  UINT8         *OnBoardDeviceType
)
{
  EFI_STATUS    Status;
  UINT8         Value8;
  UINT32        Value32;
  UINT64        PciAddress;

  Value8        = 0;
  Value32       = 0;
  PciAddress    = 0;

  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID**) &gPciRootBridgeIo
                  );
  if (EFI_ERROR(Status)) {
    //
    // Can't access PciRootBridgeIoProtocol, don't change the default setting.
    //
    DEBUG ((DEBUG_ERROR, "(SMBIOS TABLE Type10) Don't change the default setting for the OnBoardDeviceType.\n"));
    return;
  }

  //
  // Read PCI registers to set enabling/disabling onboard device
  //
  switch (*OnBoardDeviceType) {
    case OnBoardDeviceTypeVideo:
      //
      // Read command reg of device 0, function 0, reg 0x52 for onboard device enabled
      //
      PciAddress = EFI_PCI_ADDRESS (0, 0, 0, 0x52);
      gPciRootBridgeIo->Pci.Read (
                          gPciRootBridgeIo,
                          EfiPciWidthUint8,
                          PciAddress,
                          1,
                          &Value8
                          );
      if (Value8 & 0x02) {
        *OnBoardDeviceType &= 0x7F;
        DEBUG ((DEBUG_INFO, "(SMBIOS TABLE Type10) OnBoardDevice Video is enabled.\n"));
      } else {
        *OnBoardDeviceType |= 0x80;
      }
      break;
    case OnBoardDeviceTypeEthernet:
      //
      // Read command reg of LAN, reg 0x04 for onboard device enabled
      //
      PciAddress = EFI_PCI_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LAN,
                     PCI_FUNCTION_NUMBER_PCH_LAN,
                     PCI_COMMAND_OFFSET
                     );
      gPciRootBridgeIo->Pci.Read (
                          gPciRootBridgeIo,
                          EfiPciWidthUint8,
                          PciAddress,
                          1,
                          &Value8
                          );
      if (Value8 != 0xff) {
        if ((Value8 & 0x02) || (Value8 & 0x01)) {
          *OnBoardDeviceType |= 0x80;
          DEBUG ((DEBUG_INFO, "(SMBIOS TABLE Type10) OnBoardDevice Ethernet is enabled.\n"));
        } else {
          *OnBoardDeviceType &= 0x7F;
        }
      }
      break;
    case OnBoardDeviceTypeSound:
      //
      // Read command reg of device 31, function 3, reg 0x04 onboard device enabled
      //
      PciAddress = EFI_PCI_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_HDA,
                     PCI_FUNCTION_NUMBER_PCH_HDA,
                     PCI_COMMAND_OFFSET
                     );
      gPciRootBridgeIo->Pci.Read (
                          gPciRootBridgeIo,
                          EfiPciWidthUint32,
                          PciAddress,
                          1,
                          &Value32
                          );
      if (Value32 != 0xffffffff) {
        if ((Value32 & 0x02) || (Value32 & 0x01)) {
          *OnBoardDeviceType |= 0x80;
          DEBUG ((DEBUG_INFO, "(SMBIOS TABLE Type10) OnBoardDevice Sound is enabled.\n"));
        } else {
          *OnBoardDeviceType &= 0x7F;
        }
      }
      break;
  }

}

/**
  This function makes boot time changes to the contents of the
  MiscOnboardDevice (Type 10).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscOnBoardDevice)
{
  CHAR8                         *OptionalStrStart;
  UINTN                         StringNumber;
  UINTN                         StringLength;
  EFI_STATUS                    Status;
  EFI_SMBIOS_HANDLE             SmbiosHandle;
  SMBIOS_TABLE_TYPE10           *SmbiosRecord;
  SMBIOS_TABLE_TYPE10           *ForType10InputData;
  SMBIOS_TABLE_TYPE10_STRINGS   SmbiosTableType10Strings;
  CHAR16                        *StrBufferStart;
  CHAR16                        *StrBufferPtr;
  UINTN                         StrBuffSize;
  UINTN                         OptionalStrBufferSize;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "(MiscOnBoardDevice) Entry.\n"));
  ZeroMem (&SmbiosTableType10Strings, sizeof(SMBIOS_TABLE_TYPE10_STRINGS));
  StringNumber        = 0;
  StringLength        = 0;
  ForType10InputData  = (SMBIOS_TABLE_TYPE10 *) RecordData;
  StrBufferStart      = AllocateZeroPool (1 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16));
  StrBufferPtr        = StrBufferStart;
  StrBuffSize         = (1 * SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR16)) / sizeof (CHAR16);

  //
  // Initialize SMBIOS Tables Type10 strings
  //
  switch (ForType10InputData->Device[0].DeviceType & 0x7F) {
    case OnBoardDeviceTypeVideo:
      SmbiosStrInit (&SmbiosTableType10Strings.DescriptionString, SMBIOS_MISC_ONBOARD_DEVICE_VIDEO, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case OnBoardDeviceTypeEthernet:
      SmbiosStrInit (&SmbiosTableType10Strings.DescriptionString, SMBIOS_MISC_ONBOARD_DEVICE_ETHERNET, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    case OnBoardDeviceTypeSound:
      SmbiosStrInit (&SmbiosTableType10Strings.DescriptionString, SMBIOS_MISC_ONBOARD_DEVICE_SOUND, &StrBufferPtr, &StringNumber, &StringLength, &StrBuffSize);
      break;
    default:
      break;
  }

  //
  // Two zeros following the last string.
  //
  OptionalStrBufferSize = StringLength + StringNumber + 1;
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE10) + OptionalStrBufferSize);
  if (SmbiosRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE10) + OptionalStrBufferSize);
  CopyMem (SmbiosRecord, RecordData, sizeof (SMBIOS_TABLE_TYPE10));
  DEBUG((DEBUG_INFO, "(Type10) SmbiosRecord->Hdr.Type is %d \n", SmbiosRecord->Hdr.Type));
  DEBUG((DEBUG_INFO, "(Type10) SmbiosRecord->Hdr.Length is %d \n", SmbiosRecord->Hdr.Length));

  //
  // Get device status
  //
  GetOnBoardDeviceType (&SmbiosRecord->Device[0].DeviceType);

  //
  // Update SMBIOS Tables Type10 strings
  //
  OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
  SmbiosStringsUpdate ((CHAR16 **)&SmbiosTableType10Strings, (UINT8*)OptionalStrStart, StringNumber, OptionalStrBufferSize);

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  SmbiosHandle = 0;
  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  FreePool(StrBufferStart);
  return Status;
}
