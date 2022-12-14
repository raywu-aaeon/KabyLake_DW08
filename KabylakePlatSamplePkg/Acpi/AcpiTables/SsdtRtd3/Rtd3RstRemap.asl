/** @file
ACPI RTD3 SSDT table.

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
// Power resource for NVMx (remapped) device.

      Name(_PR0, Package(){NVPR})
      Name(_PR3, Package(){NVPR})

      PowerResource(NVPR, 0, 0) { // Power Resource for remapped devices
          Name(_STA, 0x01)
          Method(_ON, 0) {
              If(CondRefOf(IR3A)) {
                If(LEqual(IR3A(),1)) {
                  If(LEqual(RAID(),0)) {
                    RPPC(1)
                  }
                }
              }
              Store(0x1, _STA);
          }
          Method(_OFF, 0) {
              If(CondRefOf(IR3A)) {
                If(LEqual(IR3A(),1)) {
                  If(LEqual(RAID(),0)) {
                    RPPC(0)
                  }
                }
              }
              Store(0x0, _STA);
          }
      }
