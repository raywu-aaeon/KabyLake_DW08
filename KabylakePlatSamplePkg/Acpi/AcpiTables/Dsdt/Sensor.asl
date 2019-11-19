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

        //--------------------
        //  Intel Sensor Solution
        //--------------------
          Name (_ADR, Zero)
          Name (_UID, One)

          Method (_STA, 0, NotSerialized)
          {
            // check for device enabled in BIOS setup, 1 = enabled
            If(LNotEqual(And(SDS0,0x01), 0x01)) {
              Return (0x00)  // device is disabled in BIOS setup
            }

            // check the GPIO mode select, 1 = Sensor Hub
            // verify the Sensor Hub _HID
            // platform todo: actual pin
//            If(LAnd(LEqual(\_SB.GGOV(0), 0x01),LEqual(_HID, "INT33D1"))) {
//              Return(0xF)  // device is Sensor Hub and Sensor Hub mode is selected
//            }

            // check the GPIO mode select, 0 = DFU
            // verify the DFU _HID
            // platform todo: actual pin
//            If(LAnd(LEqual(\_SB.GGOV(0), 0x00),LEqual(_HID, "INT33D7"))) {
//              Return(0xF)  // device is DFU and DFU mode is selected
//            }

            Return(0x00)
          }

          Method(_DSM, 0x4, NotSerialized)
          {
            // DSM UUID for HIDI2C. Do Not change.
            If(LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE")))
            {
              // Function 0 : Query Function
              If(LEqual(Arg2, Zero))
              {
                // Revision 1
                If(LEqual(Arg1, One))
                {
                  Return(Buffer(One) { 0x03 })
                }
              }
              // Function 1 : HID Function
              If(LEqual(Arg2, One))
              {
                // HID Descriptor Address (IHV Specific)
                Return(0x0001)
              }
            }
            Else
            {
              Return(Buffer(One) { 0x00 })
            }
           }

           Method (_CRS, 0, NotSerialized)
           {
              Name (SBFI, ResourceTemplate ()
              {
                I2cSerialBus (
                0x40,                  //SlaveAddress: bus address
                ControllerInitiated,   //SlaveMode: Default to ControllerInitiated
                400000,                //ConnectionSpeed: in Hz
                AddressingMode7Bit,    //Addressing Mode: default to 7 bit
                "\\_SB.PCI0.I2C0",     //ResourceSource: I2C bus controller name
                ,                      //ResourceSourceIndex: defaults to 0
                ,                      //ResourceUsage: Defaults to ResourceConsumer
                ,                      //Descriptor Name: creates name for offset of resource descriptor
                )  //VendorData

                Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , ,) {28} //Route to PIRQM
              })

                Return (SBFI)
           }

