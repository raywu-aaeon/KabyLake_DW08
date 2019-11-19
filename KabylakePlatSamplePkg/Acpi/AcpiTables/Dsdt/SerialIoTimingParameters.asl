/** @file
  ACPI DSDT table

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


Method(PKG1, 1, Serialized) {
  Name (PKG, Package(1){0})
  Store(Arg0, Index(PKG,0))
  Return (PKG)
}

Method(PKG3, 3, Serialized) {
  Name (PKG, Package(3){0,0,0})
  Store(Arg0, Index(PKG,0))
  Store(Arg1, Index(PKG,1))
  Store(Arg2, Index(PKG,2))
  Return (PKG)
}

//
// SerialIo controllers use configurable timing parameters. It is possible but only optional to provide them through ACPI.
// If BIOS doesn't provide relevant methods, controllers' OS drivers will use default values, or in case of Windows use system registry.
// This sample code provides timing parameters optionally (based on USTP variable) and only for 3 SerialIo controllers out of 11.
//

if(USTP) {
  Scope(\_SB.PCI0.I2C1) {
    Method(SSCN){Return(PKG3(SSHI,SSLI,SSDI))} // the default values are 432, 507, 9
    Method(FMCN){Return(PKG3(FMHI,FMLI,FMDI))} // the default values are 72, 160, 9
    Method(FPCN){Return(PKG3(FPHI,FPLI,FPDI))} // the default values are 26, 50, 5
    Method(M0D3){Return(PKG1(M0CI))}           // the default value is 200
    Method(M1D3){Return(PKG1(M1CI))}           // the default value is 2000
  }

  Scope(\_SB.PCI0.SPI1) {
    Method(M0D3){Return(PKG1(M0CS))}           // the default value is 200
    Method(M1D3){Return(PKG1(M1CS))}           // the default value is 2000
  }

  Scope(\_SB.PCI0.UA01) {
    Method(M0D3){Return(PKG1(M0CU))}           // the default value is 200
    Method(M1D3){Return(PKG1(M1CU))}           // the default value is 200
  }
}


