/** @file
  Define the data structure and GUID of Capsule Processing Result Variable.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2014 Intel Corporation.

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

#ifndef __CAPSULE_PROCESSING_RESULT_H__
#define __CAPSULE_PROCESSING_RESULT_H__

#define EFI_CAPSULE_REPORT_GUID \
  { \
    0x39b68c46, 0xf7fb, 0x441b, { 0xb6, 0xec, 0x16, 0xb0, 0xf6, 0x98, 0x21, 0xf3 } \
  }

///
/// The format of Capsule Processing Result Variable. Defined in UEFI spec 2.4 section 7.5.6.
///
#pragma pack(1)

typedef struct {
  //
  // Size in bytes of the variable
  //
  UINT16 VariableTotalSize;
  //
  // Guid from EFI_CAPSULE_HEADER
  //
  EFI_GUID CapsuleGuid;
  //
  // Timestamp using system time when processing completed
  //
  EFI_TIME CapsuleProcessed;
  //
  // Result of the capsule processing
  //
  EFI_STATUS CapsuleStatus;
} EFI_CAPSULE_PROCESSING_RESULT;

#pragma pack()

extern EFI_GUID gEfiCapsuleReportGuid;

#endif
