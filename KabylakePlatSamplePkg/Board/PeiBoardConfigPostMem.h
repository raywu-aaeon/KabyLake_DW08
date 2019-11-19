/** @file
  PEI Boards Configurations for PostMem phase.

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

#ifndef PEI_BOARD_CONFIG_POST_MEM_H_
#define PEI_BOARD_CONFIG_POST_MEM_H_

#include <PlatformBoardConfig.h>

GLOBAL_REMOVE_IF_UNREFERENCED BOARD_CONFIG_BLOCK mSampleBoardConfigPeiPostMem = {
  {{{0, ALIGN16(sizeof(BOARD_CONFIG_BLOCK_PEI_POSTMEM)), 0},
    {0x16eed7eb, 0x4b99, 0x4337, {0xe6, 0xa0, 0x21, 0x53, 0x56, 0x17, 0xcc, 0xe2}}},
    BOARD_CONFIG_BLOCK_PEI_POSTMEM_VERSION, 0, {0, 0}}
  };

#endif // PEI_BOARD_CONFIG_POST_MEM_H_
