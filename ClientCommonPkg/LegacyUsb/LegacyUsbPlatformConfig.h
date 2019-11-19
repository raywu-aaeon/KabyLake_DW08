/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2016 Intel Corporation.

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

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be 
  modified, except as allowed by additional terms of your license agreement.

@par Specification
**/


#ifndef __LEGACY_USB__PLATFORM_CONFIG_H__
#define __LEGACY_USB__PLATFORM_CONFIG_H__

//
// Customized section - begin
//
#define DEBUG_SWITCH        0
#define EMUL6064_SUPPORT	1
#define UHCI_EMUL_SUPPORT	1
#define IRQ_EMUL_SUPPORT	1
#define EMULATION_ON_KBC_PRESENT	1
#define SW_IRQ_GENERATION_REG	0x470

#define USB_DRIVER_MAJOR_VER	0x8
#define USB_DRIVER_MINOR_VER	0x8
#define USB_DRIVER_BUILD_VER	0x2
#define USB_UHCI	0x2
#define USB_OHCI	0x0
#define UHCI_PCI_DEVICES	{0xE8, 0}, {0xE9, 0}
#define USB_HOTPLUG_FDD	0
#define USB_HOTPLUG_CDROM	0
#define KBC_EMULATION_8MSTD_OVERRIDE	0x19
#define USB_EHCI_HANDOFF_SUPPORT	1
#define USB_DEV_KBD	1
#define USB_DEV_MOUSE	1
#define USB_DEV_HUB	1
#define USB_DEV_MASS	1
#define UHCI_RESET_DELAY_MS	0xa
#define USB_SEND_COMMAND_TO_KBC	0x60
#define USB_MOUSE_UPDATE_EBDA_DATA	1
#define UHCI_DISABLE_ROOTHUB_POLLING	0
#define EHCI_64BIT_DATA_STRUCTURE	1
#define EHCI_ASYNC_BELL_SUPPORT	0
#define USB_DEV_HID_COUNT	0xe
#define USB_DEV_HUB_COUNT	0x5
#define USB_DEV_MASS_COUNT	0x8
#define TopDebugLevel	0x3
#define BottomDebugLevel	0x3
#define USB_SWSMI	0x31
#define USB_BEEP_ENABLE	0
#define USB_MASS_EMULATION_NATIVE	1
#define MAX_SIZE_FOR_FLOPPY_EMULATION 530
#define MAX_BULK_DATA_SIZE	0x400
#define USB_HOST_SCAN	0x1
#define USB_HOST_DEVICE_MIN	0x1a
#define USB_HOST_DEVICE_MAX	0x1f
#define USB_EHCI	0x2
#define EHCI_PCI_DEVICES	{0xD0, 0x20}, {0xE8, 0x20}
#define XHCI_PCI_DEVICES    {0xA0, 0x30}
#define INTEL_USB2_EN	1

#define PM_BASE_ADDRESS	0x1800

//
// Add for XHCI integration
//
#define CORE_REVISION 3
#define MEM_PAGE_COUNT	0x8
#define USB_XHCI      2
#define XHCI_EVENT_SERVICE_MODE 0
#define USB_XHCI_EXT_HW_SMI_PIN	0xff
#define XHCI_64BYTE_CONTEXT_SIZE	0
#define PEI_XHCI_MEM_BASE_ADDRESS	0xE0000000
#define USB_DEV_UNSUPPORTED	0x10
#define HIDE_USB_SUPPORT_SETUP_QUESTION	1
#define CSM_SUPPORT 1
#define USB_HOTPLUG_FDD	0
#define USB_HOTPLUG_HDD	0
#define USB_HOTPLUG_CDROM	0
#define REMOVE_USB_STORAGE_FROM_BBS_IF_NO_MEDIA	0
#define USB_EFIMS_DIRECT_ACCESS	0
#define FULLSPEED_MAX_BULK_DATA_SIZE_PER_FRAME	0x380
#define HIGHSPEED_MAX_BULK_DATA_SIZE	0x400
#define KEY_REPEAT_DELAY	0x1
#define KEY_REPEAT_INITIAL_DELAY	0x6
#define EXTRA_CHECK_DEVICE_READY	0
#define CHECK_MOUSE_FLAG	0

//
// Customized section - end
//

#endif
