/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

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
    // ETRM (Intel DPTF Participant List)
    //
    // The ETRM object evaluates to a package of packages each of which describes one participant device.
    //
    Name(ETRM, Package()
    {
      Package() {\_SB.PCI0.LPCB.H_EC.SEN1,  "INT3403",    0x06, "SEN1"}, // Sensor 1 participant
      Package() {\_SB.PCI0.LPCB.H_EC.SEN2,  "INT3403",    0x06, "SEN2"}, // Sensor 2 participant
      Package() {\_SB.PCI0.LPCB.H_EC.SEN3,  "INT3403",    0x06, "SEN3"}, // Sensor 3 participant
      Package() {\_SB.PCI0.LPCB.H_EC.SEN4,  "INT3403",    0x06, "SEN4"}, // Sensor 4 participant
      Package() {\_SB.PCI0.LPCB.H_EC.SEN5,  "INT3403",    0x06, "SEN5"}, // Sensor 5 participant
      Package() {\_SB.PCI0.LPCB.H_EC.GEN1,  "INT3403",    0x06, "GEN1"}, // Thermistor 1 participant
      Package() {\_SB.PCI0.LPCB.H_EC.GEN2,  "INT3403",    0x06, "GEN2"}, // Thermistor 2 participant
      Package() {\_SB.PCI0.LPCB.H_EC.GEN3,  "INT3403",    0x06, "GEN3"}, // Thermistor 3 participant
      Package() {\_SB.PCI0.LPCB.H_EC.GEN4,  "INT3403",    0x06, "GEN4"}, // Thermistor 4 participant
      Package() {\_SB.PCI0.LPCB.H_EC.CHRG,  "INT3403",    0x06, "CHRG"}, //
      Package() {\_SB.WWAN,                 "INT3408",    0x06, "WWAN"}, // Wireless WWAN temperature sensor Device
      Package() {\_SB.WRLS,                 "INT3408",    0x06, "WRLS"}, // Wireless WRLS temperature sensor Device
      Package() {\_SB.TPWR,                 "INT3407",    0x06, "TPWR"}, // Platform Power Device
      Package() {\_SB.STRG,                 "INT340A",    0x06, "STRG"}, // Storage Participant
      Package() {\_SB.PCI0.LPCB.H_EC.TFN1,  "INT3404",    0x06, "TFN1"}, // Fan Control Device
      Package() {\_SB.PCI0.LPCB.H_EC.TFN2,  "INT3404",    0x06, "TFN2"}, // Fan Control Device
      Package() {\_SB.PCI0.DPLY,            "INT3406",    0x06, "DPLY"}, // Display Participant Device
//
// KblGBegin
//
      Package() {\_SB.DGFC,                 "INT340D",    0x06, "DGFC"}, // Discrete Gfx Core Participant
      Package() {\_SB.DGHM,                 "INT340D",    0x06, "DGFM"}, // Discrete Gfx Memory Participant
      Package() {\_SB.MCPP,                 "INT3530",    0x06, "MCPP"}, // Multi Chip Pkg Participant
//
// KblGEnd
//
      Package() {\_SB.PCI0.B0D4,            "8086_1903",  0x00, "0x00040000"}  // SKL SA Thermal Device
    })
}
