/** @file
  Intel ACPI Sample Code for Alto dGPU

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2018 Intel Corporation.

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

Method(HGAS)        // SG Adaptor Select
{
    // Toggle GFX Adapter.
    Store(1,\_SB.PCI0.GFX0.PXGS)
    Notify(\_SB.PCI0.GFX0,0x81)
}

Method(HBRT, 1 , Serialized)
{
    //
    // Send backlight notifications to the DGPU LFP device.
    // This is required for Win7 and is backward compatible with Vista.
    //

    If(And(4,DSEN))     //Note: DSEN variable is expected to be set by IGD miniport only.
    {
        If(LEqual(Arg0,4))
        {
            Notify(\_SB.PCI0.GFX0.DD1F,0x87)  //Note: DD1F is hardcoded as the LFP device in intelgfx.asl
        }
        If(LEqual(Arg0,3))
        {
            Notify(\_SB.PCI0.GFX0.DD1F,0x86)
        }
    }

}

Method(HPFS, 0, Serialized)     //Panel Fitting Hot Key
{
    If(LNotEqual(\_SB.PCI0.GFX0.GSTP,1))
    {

        //
        // SG Handling of Panel Fitting Switch
        //
        If(LEqual(\_SB.PCI0.GFX0.AGXA,0))
        {
            \_SB.PCI0.GFX0.AINT(2, 0)
        }
        Else
        {
            Store(1,\_SB.PCI0.GFX0.EMDR)

            //
            // Expansion Mode toggling
            //
            If(LEqual(\_SB.PCI0.GFX0.EXPM,2))
            {
                Store(0,\_SB.PCI0.GFX0.EXPM)
            }
            Else
            {
                Increment(\_SB.PCI0.GFX0.EXPM)
            }
            Notify(\_SB.PCI0.GFX0,0x81)
        }
    }
}

Method (HNOT, 1, Serialized)
{

    Switch(ToInteger(Arg0))
    {
        case(1) //Display Switch event
        {
            If(LNotEqual(\_SB.PCI0.GFX0.GSTP,1))
            {

                //
                // SG Handling of Display Switch Event
                //
                If(LEqual(And(SGMD,0x7F),0x2))
                {
                     //
                     // Notify the Active Gfx Adapter
                     //
                    If(LEqual(\_SB.PCI0.GFX0.AGXA,0))
                    {
                      Notify(\_SB.PCI0.GFX0,0x80)
                    }

                    If(LEqual(\_SB.PCI0.GFX0.AGXA,1))
                    {
                      Notify(\_SB.PCI0.PEG0.PEGP,0x80)
                    }
                }
                Else
                {
                    Store(1,\_SB.PCI0.GFX0.DSWR)
                    Notify(\_SB.PCI0.GFX0,0x81)
                }
            }
        }

        case (2) //Lid switch event
        {
                If(LEqual(And(SGMD,0x7F),0x2))
                {
                     //
                     // Notify the Active Gfx Adapter
                     //
                    If(LEqual(\_SB.PCI0.GFX0.AGXA,0))
                    {
                      Notify(\_SB.PCI0.GFX0,0x80)
                    }

                    If(LEqual(\_SB.PCI0.GFX0.AGXA,1))
                    {
                      Notify(\_SB.PCI0.PEG0.PEGP,0x80)
                    }
                 }
                 Else
                 {

                     Notify(\_SB.PCI0.GFX0,0x80)     //Placeholder for LID event
                 }
        }

        case (3) //Dock event
        {
                If(LEqual(And(SGMD,0x7F),0x2))
                {
                     //
                     // Notify the Active Gfx Adapter
                     //
                    If(LEqual(\_SB.PCI0.GFX0.AGXA,0))
                    {
                      Notify(\_SB.PCI0.GFX0,0x80)
                    }

                    If(LEqual(\_SB.PCI0.GFX0.AGXA,1))
                    {
                      Notify(\_SB.PCI0.PEG0.PEGP,0x80)
                    }
                }
                Else
                {
                    Notify(\_SB.PCI0.GFX0,0x80)     //Placeholder for handling dock event
                }
        }

        Default
        {
            Notify(\_SB.PCI0.GFX0,0x80)
        }

    }

}


