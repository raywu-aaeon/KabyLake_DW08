/** @file
  AMT BDS Support include file

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

#ifndef _AMT_WRAPPER_DXE_H_
#define _AMT_WRAPPER_DXE_H_

#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/AmtWrapperProtocol.h>
#include <Protocol/AlertStandardFormat.h>
#include <Protocol/LegacyBios.h>
#include <Library/DxeAmtLib.h>
#include <MeChipset.h>
#include <MeBiosExtensionSetup.h>
#include <Protocol/AmtPolicy.h>
#include <Protocol/ActiveManagement.h>
#include <Protocol/HeciProtocol.h>
#include <Library/HobLib.h>
#include <MeBiosPayloadHob.h>
#include <Library/ConfigBlockLib.h>

#define R_PCI_SVID                      0x2C

#define     DISPLAY_SCREEN              0x01                // for LegacySredr CSM16
#define     SUPPORT8025                 0x02                // for LegacySredr CSM16

// define for SREDIR_DISPLAY_MODE
#define     DISPLAY_BY_SETUP            0x00
#define     DISPLAY_BY_80x24            0x01
#define     DISPLAY_BY_80x25            0x02

#define STOPB   0x4                 //      Bit2: Number of Stop Bits
#define PAREN   0x8                 //      Bit3: Parity Enable
#define EVENPAR 0x10                //      Bit4: Even Parity Select
#define STICPAR 0x20                //      Bit5: Sticky Parity
#define SERIALDB 0x3                //      Bit0-1: Number of Serial

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           SerialOverLAN;
  UART_DEVICE_PATH          Uart;
  VENDOR_DEVICE_PATH        TerminalType;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_PCI_SERIAL_OVER_LAN_DEVICE_PATH;

#define gPciRootBridge \
  { \
    { \
      ACPI_DEVICE_PATH, \
      ACPI_DP, \
      { \
        (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), \
        (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8) \
      } \
    }, \
    EISA_PNP_ID (0x0A03), 0 \
  }

#define gEndEntire \
  { \
    END_DEVICE_PATH_TYPE,\
    END_ENTIRE_DEVICE_PATH_SUBTYPE,\
    { \
      END_DEVICE_PATH_LENGTH,\
      0 \
    } \
  }

EFI_STATUS
EFIAPI
AmtWrapperInit (
  IN EFI_HANDLE ImageHandle,
  IN OUT EFI_SYSTEM_TABLE *SystemTable
  );

VOID
InitAmtWrapperLib (
  VOID
  );

BOOLEAN
EFIAPI
AmtWrapperGet (
  IN UINTN Index,
  IN OUT VOID *pVar
  );

BOOLEAN
EFIAPI
AmtWrapperSet (
  IN UINTN Index,
  IN OUT VOID *pVar
  );

BOOLEAN
EFIAPI
AmtWrapperBootOptionExist (
  VOID
  );

BOOLEAN
EFIAPI
AmtWrapperEnableStorageRedir (
  VOID
  );

BOOLEAN
EFIAPI
AmtWrapperEnterSetup (
  VOID
  );

BOOLEAN
EFIAPI
AmtWrapperPauseBoot (
  VOID
  );

BOOLEAN
EFIAPI
AmtWrapperEnableKvm (
  VOID
  );

BOOLEAN
EFIAPI
AmtWrapperEnableSecureErase (
  VOID
  );

BOOLEAN
EFIAPI
AmtWrapperConsoleLocked (
  VOID
  );

BOOLEAN
EFIAPI
AmtWrapperEnableSol (
  VOID
  );

EFI_STATUS
BdsAsfInitialization (
  VOID
  );

EFI_STATUS
EFIAPI
AmtWrapperBdsBootViaAsf (
  VOID
  );

EFI_STATUS
BdsBootViaAsf (
  VOID
  );

VOID
EFIAPI
AmtWrapperReadyToBoot (
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
  );

UINTN IsSOL (
  IN EFI_HANDLE Controller
  );

EFI_STATUS
EFIAPI
GetSetupPrompt (
  IN OUT  UINTN     *PromptIndex,
  OUT     CHAR16    **String
  );

EFI_STATUS
EFIAPI
DetermineSetupHotKey (
  IN EFI_KEY_DATA       *KeyData
  );

EFI_STATUS
EFIAPI
GetSupportedHotkeys (
  OUT  UINTN             *KeyCnt,
  OUT  EFI_KEY_DATA      **KeyData
  );

//AMI_OVERRIDE_START - Enable/Disable SOL COM when AMT SOL is enable.
VOID
SetSOLCOMEnable (
  IN BOOLEAN         Enabled
  );
//AMI_OVERRIDE_END - Enable/Disable SOL COM when AMT SOL is enable.
#endif
