/** @file
  This file describes the contents of the ACPI Firmware ACPI Control
  Structure (FACS).  All changes to the FACS contents should be done
  in this file.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2014 Intel Corporation.

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

#ifndef _FACS_H_
#define _FACS_H_

//
// Statements that include other files
//

#include <IndustryStandard/Acpi50.h>

//
// FACS Definitions
//

#define EFI_ACPI_FIRMWARE_WAKING_VECTOR 0x00000000
#define EFI_ACPI_GLOBAL_LOCK 0x00000000

//
// Firmware Control Structure Feature Flags are defined in AcpiX.0.h
//
#define EFI_ACPI_FIRMWARE_CONTROL_STRUCTURE_FLAGS 0x00000000

#define EFI_ACPI_X_FIRMWARE_WAKING_VECTOR 0x0000000000000000

#endif
