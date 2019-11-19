/** @file
  TBT TypeC ports information

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2017 Intel Corporation.

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

DefinitionBlock (
  "TbtTypeC.aml",
  "SSDT",
  2,
  "Intel",
  "TbtTypeC",
  0x0
  )
{
  External(\_SB.PCI0.RP09.PXSX, DeviceObj)
  External(\_SB.PCI0.RP01.PXSX, DeviceObj)
  External(\TBTS, IntObj)
  External(\TBSE, IntObj)
  External(\USME, IntObj)
  External(\UPT1, IntObj)
  External(\UPT2, IntObj)
  If(LAnd(LEqual(TBTS, 1),LEqual(TBSE, 1)))
  {
    Scope(\_SB.PCI0.RP01.PXSX)
    {
       Name(TUSB, Package() {
          //    2, // Number of TBT Ports on the PCie Root port
              1, // PCH XHCI USB2 Port for TBT Port 1
              4  // PCH XHCI USB2 Port for TBT Port 2
       })
// AMI_OVERRIDE_START - EIP345352 : _UPC & _PLD setting of original TbtTypeCWrapper.asl is for Intel CRB desing, customer's board design is different to CRB. They can include its ASL file for its _PLD/__UPC.
#if defined(ASL_TbtTypeC_Wrapper_ASL_SUPPORT) && (ASL_TbtTypeC_Wrapper_ASL_SUPPORT == 1)
       Include ("OemTbtTypeCWrapper.asl")
#endif       
// AMI_OVERRIDE_END - EIP345352 : _UPC & _PLD setting of original TbtTypeCWrapper.asl is for Intel CRB desing, customer's board design is different to CRB. They can include its ASL file for its _PLD/__UPC.
    }// End of Scope(\_SB.PCI0.RP09.PXSX)
  } // If(LAnd(LEqual(TBTS, 1),LEqual(TBSE, 1)))

  If(LAnd(LEqual(TBTS, 1),LEqual(TBSE, 9)))
  {
    Scope(\_SB.PCI0.RP09.PXSX)
    {
       Name(TUSB, Package() {
          //    2, // Number of TBT Ports on the PCie Root port
              1, // PCH XHCI USB2 Port for TBT Port 1
              2  // PCH XHCI USB2 Port for TBT Port 2
       })
// AMI_OVERRIDE_START - EIP345352 : _UPC & _PLD setting of original TbtTypeCWrapper.asl is for Intel CRB desing, customer's board design is different to CRB. They can include its ASL file for its _PLD/__UPC.
#if defined(ASL_TbtTypeC_Wrapper_ASL_SUPPORT) && (ASL_TbtTypeC_Wrapper_ASL_SUPPORT == 1)
       Include ("OemTbtTypeCWrapper.asl")
#endif       
// AMI_OVERRIDE_END - EIP345352 : _UPC & _PLD setting of original TbtTypeCWrapper.asl is for Intel CRB desing, customer's board design is different to CRB. They can include its ASL file for its _PLD/__UPC.
    }// End of Scope(\_SB.PCI0.RP09.PXSX)
  } // If(LAnd(LEqual(TBTS, 1),LEqual(TBSE, 9)))
} // DefinitionBlock
