/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2014 Intel Corporation.

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

#ifndef _DXE_POLICY_UPDATE_PROTOCOL_H_
#define _DXE_POLICY_UPDATE_PROTOCOL_H_

//
// DXE Policy Update Protocol GUID
// {D5E1268B-F62B-4b12-BB27-CBF07AE7B8B9}
//
#define DXE_POLICY_UPDATE_PROTOCOL_GUID \
  { \
    0xd5e1268b, 0xf62b, 0x4b12, 0xbb, 0x27, 0xcb, 0xf0, 0x7a, 0xe7, 0xb8, 0xb9 \
  }
//
// Extern the GUID for protocol users.
//
extern EFI_GUID gDxePolicyProtocolGuid;



#endif
