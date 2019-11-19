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
    2631,   0x2F0,  // -10,     753(0x2F1)
    2681,   0x2DC,  // -05,     734(0x2DE)
    2731,   0x2C4,  // 000,     710(0x2C6)
    2781,   0x2A8,  // 005,     682(0x2AA)
    2831,   0x288,  // 010,     650(0x28A)
    2881,   0x264,  // 015,     615(0x267)
    2931,   0x240,  // 020,     576(0x240)
    2981,   0x214,  // 025,     535(0x217)
    3031,   0x1EC,  // 030,     492(0x1EC)
    3081,   0x1C0,  // 035,     449(0x1C1)
    3131,   0x194,  // 040,     406(0x196)
    3181,   0x16C,  // 045,     365(0x16D)
    3231,   0x144,  // 050,     326(0x146)
    3281,   0x11C,  // 055,     286(0x11E)
    3331,   0x0FC,  // 060,     252(0x0FC)
    3381,   0x0DC,  // 065,     222(0x0DE)
    3431,   0x0C0,  // 070,     194(0x0C2)
    3481,   0x0A8,  // 075,     170(0x0AA)
    3531,   0x094,  // 080,     149(0x095)
    3581,   0x080,  // 085,     130(0x082)
    3631,   0x070,  // 090,     112(0x070)
    3681,   0x060,  // 095,     099(0x063)
    3731,   0x054,  // 100,     086(0x056)
    3781,   0x048   // 105,     072(0x048)
    })

