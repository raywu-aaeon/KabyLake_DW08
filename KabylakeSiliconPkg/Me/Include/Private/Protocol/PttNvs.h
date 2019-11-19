
/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/

  //
  // Define TCG NVS Area operation region.
  //
  
#ifndef _PTT_NVS_H_
#define _PTT_NVS_H_

#pragma pack (push,1)
typedef struct {
  UINT8    PhysicalPresenceSoftwareSmi;             ///< Offset 0       Software SMI for Physical Presence Interface
  UINT32   PhysicalPresenceParameter;               ///< Offset 1       Used for save physical presence parameter
  UINT32   PhysicalPresenceResponse;                ///< Offset 5       Physical Presence request operation response
  UINT32   PhysicalPresenceRequest;                 ///< Offset 9       Physical Presence request operation
  UINT32   PhysicalPresenceRequestParameter;        ///< Offset 13      Physical Presence request operation parameter
  UINT32   PhysicalPresenceLastRequest;             ///< Offset 17      Last Physical Presence request operation
  UINT32   PhysicalPresenceReturnCode;              ///< Offset 21      Physical Presence function return code
  UINT8    MemoryClearSoftwareSmi;                  ///< Offset 25      Software SMI for Memory Clear Interface
  UINT32   MemoryClearParameter;                    ///< Offset 26      Used for save the Mor parameter
  UINT32   MemoryClearRequest;                      ///< Offset 30      Memory Overwrite Request Data
  UINT32   MemoryClearReturnCode;                   ///< Offset 34      Memory Overwrite function return code
} TCG_NVS;

#pragma pack(pop)
#endif
