/** @file
  Header file for USB device mode APIs.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 Intel Corporation.

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


#ifndef _USB_DEVICE_MODE_DXE_H_
#define _USB_DEVICE_MODE_DXE_H_

#include <Uefi.h>
#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UsbDeviceLib.h>
#include <Protocol/UsbDeviceModeProtocol.h>
#include "XdciCommon.h"
#include "XdciDevice.h"


///
/// Function declaration
///
EFI_STATUS
UsbdSetupHdlr (
  IN EFI_USB_DEVICE_REQUEST    *CtrlRequest
  );

EFI_STATUS
UsbdResetHdlr (
  VOID
  );

EFI_STATUS
UsbdConnDoneHdlr (
  VOID
  );

VOID
UsbdXferDoneHdlr (
  IN EFI_USB_DEVICE_XFER_INFO    *XferInfo
  );

extern EFI_USB_DEVICE_MODE_PROTOCOL  mUsbDeviceModeProtocol;

#endif
