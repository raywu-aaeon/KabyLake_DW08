/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2017 Intel Corporation.

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

// AMI_OVERRIDE_START - For Hybric Graphics feature.
If (CondRefOf(\_SB.PCI0.RP01.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP01.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - For Hybric Graphics feature.
}
// AMI_OVERRIDE_END - For Hybric Graphics feature.

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP02.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP02.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP03.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP03.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP04.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP04.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - For Hybric Graphics feature.
If (CondRefOf(\_SB.PCI0.RP05.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP05.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - For Hybric Graphics feature.
}
// AMI_OVERRIDE_END - For Hybric Graphics feature.

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP06.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP06.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP07.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP07.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP08.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP08.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - For Hybric Graphics feature.
If (CondRefOf(\_SB.PCI0.RP09.PXSX)) {
// AMI_OVERRIDE_END - For Hybric Graphics feature.
Scope (\_SB.PCI0.RP09.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - For Hybric Graphics feature.
}
// AMI_OVERRIDE_END - For Hybric Graphics feature.

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP10.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP10.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error


// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP11.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP11.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP12.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP12.PXSX)
{
 Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP13.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP13.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP14.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP14.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP15.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP15.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP16.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP16.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP17.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP17.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP18.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP18.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP19.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP19.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
If (CondRefOf(\_SB.PCI0.RP20.PXSX)) {
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error
Scope (\_SB.PCI0.RP20.PXSX)
{
  Include("DiscreteConnectivity.asl")
}
// AMI_OVERRIDE_START - To avoid Ubuntu dmesg error
}
// AMI_OVERRIDE_END - To avoid Ubuntu dmesg error

//
// WiGig Dock Locking White List and Self ID lists.
//
Scope (\_SB.PCI0)
{

  // WHIT (return White List)
  //
  // WiGig Dock Locking White List.
  // OEM can update structure or add more structures
  // and return them based on BoardID or other factors.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   package of packages containing White list
  //
  Method(WHIT)
  {
    Return(Package() {
            Package() {
            "DockOemId",      // String - Dock OEM ID
            "DockSkuId",      // String - Dock SKU ID
            0,                  // DWORD  - Full Connect (0:not supported,  1:supported)
            2,                  // DWORD  - # of Displays (0-2)
            2 }                 // DWORD  - USB Rate enumeration, (0:none, 1:USB2.0, 2:USB3.0)
          })
  } // end Method(WHIT)

  // SELF (return SelfID List)
  //
  // WiGig Dock Locking Self ID List.
  // OEM can update structure or add more structures
  // and return them based on BoardID or other factors.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   package containing SelfID list
  //
  Method(SELF)
  {
    Return(Package() {
            "PcOemId",        // String - PC OEM ID
            "PcSkuId" }       // String - PC SKU ID
          )
  } // end Method(SELF)

} //end Scope(\_SB.PCI0)
