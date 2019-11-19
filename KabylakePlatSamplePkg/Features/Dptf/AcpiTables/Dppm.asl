/** @file
  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2018 Intel Corporation.

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

Scope(\_SB.IETM)
{

    //
    // DPPM Passive Policy 2.0
    //
    Name (DP2P, Package ()
    {
      ToUUID("9E04115A-AE87-4D1C-9500-0F3E340BFE75")
    })

    //
    // DPPM Passive Policy 1.0
    //
    Name (DPSP, Package ()
    {
      ToUUID("42A441D6-AE6A-462B-A84B-4A8CE79027D3")
    })

    //
    // DPPM Active Policy 1.0
    //
    Name (DASP, Package ()
    {
      ToUUID("3A95C389-E4B8-4629-A526-C52C88626BAE")
    })

    //
    // DPPM Active Policy 2.0
    //
    Name (DA2P, Package ()
    {
      ToUUID("0E56FAB6-BDFC-4E8C-8246-40ECFD4D74EA")
    })

    //
    // DPPM Critical Policy
    //
    Name (DCSP, Package ()
    {
      ToUUID("97C68AE7-15FA-499c-B8C9-5DA81D606E0A")
    })

    //
    // DPPM Cooling Mode Policy
    //
    Name (DMSP, Package ()
    {
      ToUUID("16CAF1B7-DD38-40ed-B1C1-1B8A1913D531")
    })
// AMI_OVERRIDE_START - EIP253792: For DPTF observation tool cannot startup correctly.   
    Name(DACP, Package()
    {
      ToUUID("C4CE1849-243A-49F3-B8D5-F97002F38E6A")
    })
// AMI_OVERRIDE_END - EIP253792: For DPTF observation tool cannot startup correctly. 
    //
    // Power Boss Policy
    //
    Name (POBP, Package ()
    {
      ToUUID("F5A35014-C209-46A4-993A-EB56DE7530A1")
    })

    //
    // Hardware Duty Cycling Policy
    //
    Name (HDCP, Package ()
    {
      ToUUID("BE84BABF-C4D4-403d-B495-3128FD44DAC1")
    })

    //
    // Adaptive Performance Policy
    //
    Name (DAPP, Package ()
    {
      ToUUID("63BE270F-1C11-48fd-A6F7-3AF253FF3E2D")
    })

    //
    // Virtual Sensor Policy
    //
    Name (DVSP, Package ()
    {
      ToUUID("6ED722A7-9240-48a5-B479-31EEF723D7CF")
    })

    //
    // PID Policy
    //
    Name (DPID, Package ()
    {
      ToUUID("42496E14-BC1B-46E8-A798-CA915464426F")
    })

//
// KblGBegin
//
    //
    // Discrete Graphics Power Share Policy
    //
    Name (DGPS, Package ()
    {
      ToUUID("A01DBC39-A15A-4915-A215-9324B4C03366")
    })
//
// KblGEnd
//
} // End Scope(\_SB.IETM)
