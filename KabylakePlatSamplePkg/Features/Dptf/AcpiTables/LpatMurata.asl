/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

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

  // LPAT (Linear Piecewise Approximation Table)
  //
  // This object evaluates to package of integers that represent the intermediate
  // temperature values(10ths Kelvin) for specific raw ADC values.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   Name(LPAT, Package() {
  //       Temp1, RawValue1,
  //       Temp2, RawValue2,
  //       . . .
  //       Tempn, RawValuen})
  //
  Name(LPAT, Package(){

  // This table is the ADC Code mapping table for NCP15WB473F03RC (Murata)
  // with a 24.9k pull-up resistor to NTCREF.
  //
  // Convert Celcius temperature values to 10ths of degree Kelvin.
  // Clear bit's [1:0] of table entries because our EC only reports the top 8 bits of the 10 bit ADC value.
  //
  // Kelvin,  RAW,  // Celcius, RAW(decimal(hex))
    2631,   0x3AC,  // -10,     941(0x3AD)
    2681,   0x394,  // -05,     917(0x395)
    2731,   0x374,  // 000,     887(0x377)
    2781,   0x354,  // 005,     853(0x355)
    2831,   0x32C,  // 010,     813(0x32D)
    2881,   0x300,  // 015,     769(0x301)
    2931,   0x2D0,  // 020,     720(0x2D0)
    2981,   0x29C,  // 025,     669(0x29D)
    3031,   0x264,  // 030,     615(0x267)
    3081,   0x230,  // 035,     561(0x231)
    3131,   0x1FC,  // 040,     508(0x1FC)
    3181,   0x1C8,  // 045,     456(0x1C8)
    3231,   0x194,  // 050,     407(0x197)
    3281,   0x164,  // 055,     357(0x165)
    3331,   0x138,  // 060,     315(0x13B)
    3381,   0x114,  // 065,     277(0x115)
    3431,   0x0F0,  // 070,     243(0x0F3)
    3481,   0x0D4,  // 075,     212(0x0D4)
    3531,   0x0B8,  // 080,     186(0x0BA)
    3581,   0x0A0,  // 085,     162(0x0A2)
    3631,   0x08C,  // 090,     140(0x08C)
    3681,   0x07C,  // 095,     124(0x07C)
    3731,   0x068,  // 100,     107(0x06B)
    3781,   0x058   // 105,     090(0x05A)
    })

