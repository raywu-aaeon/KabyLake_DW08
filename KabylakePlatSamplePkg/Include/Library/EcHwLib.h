/** @file
  EC hardware library functions and definitions.

  This library provides basic Embedded Controller Hardware interface.
  It is keeped simple enough and used by other BaseEc libraries to abstacrt the usage

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 Intel Corporation.

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

@par Specification Reference:
**/


#ifndef _BASE_EC_HW_LIB_H_
#define _BASE_EC_HW_LIB_H_

#include <Base.h>
#include <Uefi.h>
#include <Library/EcCommands.h>

typedef enum {
  LpcInterface = 0,
  SpiInterface,
  EspiInterface,
  I2cInterface,
  EcInterfaceMax
} EC_INTERFACE_TYPE;

typedef enum {
  EcId0 = 0,
  EcIdMax
} EC_ID;

#endif
