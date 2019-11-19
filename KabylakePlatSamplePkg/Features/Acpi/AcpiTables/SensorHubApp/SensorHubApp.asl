/** @file
  ACPI SensorHubApp SSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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
    "SensorHubApp.aml",
    "SSDT",
    0x02, // SHAD revision.
        // A Revision field value greater than or equal to 2 signifies that integers
        // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL", // OEM ID (6 byte string)
    "sensrhub", // OEM table ID  (8 byte string)
    0x0 // OEM version of DSDT table (4 byte Integer)
    )

// BEGIN OF ASL SCOPE

{
  External(\SHAP)
  External(\GPBS)
  External(\SDS0)
  External(\USBH)
  External(\_SB.GGOV, MethodObj)
  External(\_SB.SGOV, MethodObj)
  External(\_SB.PCI0.I2C0.DFUD, DeviceObj) // DFU

  include("GpioAcpiDefines.h")

  Scope(\)
  {
    Device(SHAD)                  // Sensor Hub Application Device.
    {
      Name(_HID, EISAID("INT33D0"))
      Name(_CID, EISAID("PNP0C02"))

      Method(_STA, 0,Serialized)
      {
        If(LOr(And(SDS0,1), And(USBH,1)))
        {
          Return(0x000F)      // Sensor Hub Enabled, Show it
        }
        Return(0x00)          // Sensor Hub Disabled, Hide it
      }

      //
      // _DSM : Device Specific Method supporting USB Sideband Deferring function
      //
      // Arg0: UUID Unique function identifier
      // Arg1: Integer Revision Level
      // Arg2: Integer Function Index
      // Arg3: Package Parameters
      //
      Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
      {

        // Define the Local Variables used throughout the method.

        Name(PGCE, 0) // Power Gate Control Enable.
        Name(PGCD, 0) // Power Gate Control Duration.
        Name(PGCG, 46) // Power Gate Control GPIO PWRGATE#
        Name(DFUE, 0) // DFU Enable.
        Name(DFUD, 0) // DFU Duration.
        Name(OLDV, 0) // Old value of both Power Gate and DFU GPIO.
        Name(PGCV, 0) // Power Gate Control Value
        Name(DFUV, 0) // DFU Control Value

        // Compare passed in UUID to supported UUID.

        If (LEqual(Arg0, ToUUID ("03C868D5-563F-42A8-9F57-9A18D949B7CB")))
        {
          If (LEqual(SHAP, 1)) {
            Store (58, PGCG)    // GPIO58 = PWRGATE#
          }

          If (LGreaterEqual(ToInteger(Arg1),1))        // Revision 1.
          {
            Switch (ToInteger(Arg2))            // Switch to Function Index.
            {
              //
              // Function 0, Query - return supported functions BitIndex.
              //

              Case (0)
              {
                Return (Buffer() {0x0F})
              }

              //
              // Function 1, Power Gate Control - Pass in 2 value package via Arg3
              //

              Case (1)
              {

                Store (DeRefOf (Index (Arg3, 0)), PGCE)
                Store (DeRefOf (Index (Arg3, 1)), PGCD)

                Store(\_SB.GGOV(GPIO_SKL_LP_GPP_B22), OLDV)                // Save off GPIO46 = PWRGATE#
                \_SB.SGOV(GPIO_SKL_LP_GPP_B22, PGCE)                       // Set value to GPIO46 = PWRGATE#

                If (LGreater(PGCD, 0))               // test duration value
                {
                  Sleep(PGCD)
                  \_SB.GGOV(GPIO_SKL_LP_GPP_B22, OLDV)                     // Restore GPIO46 = PWRGATE#
                }

                If (LEqual(\_SB.GGOV(GPIO_SKL_LP_GPP_B22), 0x01))     // check for powered on (1 = on)
                {
                  Sleep(150)                         // Delay so sensor hub has time to init.
                  If (LEqual(\_SB.GGOV(GPIO_SKL_LP_GPP_B20), 0x01)) { // check mode, Sensor or DFU (1 = Sensor)
                    // Powering up in sensor hub mode
                  } Else {
                    // Powering up in DFU mode
                    Notify(\_SB.PCI0.I2C0.DFUD, 1)   // Check the DFU device status
                  }
                }

                Return (0)
              } // End Case (1)

              //
              // Function2, DFU Control - Pass in 2 value package via Arg3
              //

              Case (2)
              {

                Store (DeRefOf (Index (Arg3, 0)), DFUE)
                Store (DeRefOf (Index (Arg3, 1)), DFUD)

                Store(\_SB.GGOV(GPIO_SKL_LP_GPP_B20), OLDV)               // Save off GPIO44 = DFU_ENA#
                \_SB.GGOV(GPIO_SKL_LP_GPP_B20, DFUE)                      // Set Value to GPIO44 = DFU_ENA#

                If (LGreater(DFUD, 0))              // Test duration value
                {
                    Sleep(DFUD)                     // Delay for passed in duration.
                    \_SB.GGOV(GPIO_SKL_LP_GPP_B20, OLDV)                  // Restore GPIO44 = DFU_ENA#
                }

                Return (0)
              } // End Case 2...

              //
              // Function 3, Query Status - Return the current status of GPIO signals.
              //

              Case (3)
              {
                Store(\_SB.GGOV(GPIO_SKL_LP_GPP_B20), DFUV)
                Store(\_SB.GGOV(GPIO_SKL_LP_GPP_B22), PGCV)
                Return(Package(){PGCV, DFUV})
              } // End Case 3

            } // End Function Index...
            Return (0)
          }  // End Revision check...
          Return (0)
        }  // End UUID check...
        Return (Buffer() {0})
      }  // End _DSM Method...
    }  // End Device....
  }  // End Scope...
}  // End DefinitionBlock Termlist

