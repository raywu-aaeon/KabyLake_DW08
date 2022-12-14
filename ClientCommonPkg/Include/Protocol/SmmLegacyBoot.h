/** @file
  EDKII SMM Legacy Boot protocol.

  This SMM protocol is to be published by the SMM Foundation code to associate
  with EFI_EVENT_LEGACY_BOOT_GUID to notify SMM driver that system enter legacy boot.

  Copyright (c) 2015 Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SMM_LEGACY_BOOT_H_
#define _SMM_LEGACY_BOOT_H_

#define EDKII_SMM_LEGACY_BOOT_PROTOCOL_GUID \
  { \
    0x85a8ab57, 0x644, 0x4110, { 0x85, 0xf, 0x98, 0x13, 0x22, 0x4, 0x70, 0x70 } \
  }

extern EFI_GUID gEdkiiSmmLegacyBootProtocolGuid;

#endif
