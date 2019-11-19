
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
  


  OperationRegion (TNVS, SystemMemory, 0xFFFF0000, 0xF0)
  Field (TNVS, AnyAcc, NoLock, Preserve)
  {
  Offset(0),      PPIN, 8,  // Offset(0),     Software SMI for Physical Presence Interface
  Offset(1),      PPIP, 32, // Offset(1),     Used for save physical presence parameter
  Offset(5),      PPRP, 32, // Offset(5),     Physical Presence request operation response
  Offset(9),      PPRQ, 32, // Offset(9),     Physical Presence request operation
  Offset(13),     PPRM, 32, // Offset(13),    Physical Presence request operation parameter
  Offset(17),     LPPR, 32, // Offset(17),    Last Physical Presence request operation
  Offset(21),     FRET, 32, // Offset(21),    Physical Presence function return code
  Offset(25),     MCIN, 8,  // Offset(25),    Software SMI for Memory Clear Interface
  Offset(26),     MCIP, 32, // Offset(26),    Used for save the Mor parameter
  Offset(30),     MORD, 32, // Offset(30),    Memory Overwrite Request Data
  Offset(34),     MRET, 32, // Offset(34),    Memory Overwrite function return code
  }
