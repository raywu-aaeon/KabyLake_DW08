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

External(\TPPT)
//
// KblGBegin
//
External(\PBEN)
External (\_SB.PCI0.LPCB.H_EC.PBOK, FieldUnitObj)
//
// KblGEnd
//
Scope(\_SB)
{

  Device(TPWR)  // Power participant
  {
    Name(_HID, EISAID("INT3407")) //Intel DPTF platform power device
    Name(_UID, "TPWR")
    Name(_STR, Unicode ("Platform Power"))
    Name(PTYP, 0x11)

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If(LEqual(\PWRE,1)) {
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // _BST (Battery Status)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A Package containing the battery status in the format below:
    //   Package {
    //      Battery State
    //      Battery Present Rate
    //      Battery Remaining Capacity
    //      Battery Present Voltage
    //   }
    //
    Method(_BST,,,,PkgObj)
    {
      If(CondRefOf(\_SB.PCI0.LPCB.H_EC.BAT1._BST))
      {
        Return(\_SB.PCI0.LPCB.H_EC.BAT1._BST())
      } Else {
        Return(Package(){0,0,0,0})
      }
    }

    // _BIX (Battery Information Extended)
    //
    // The _BIX object returns the static portion of the Control Method Battery information.
    // This information remains constant until the battery is changed.
    // The _BIX object returns all information available via the _BIF object plus additional battery information.
    // The _BIF object is deprecated in lieu of _BIX in ACPI 4.0.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A Package containing the battery information as described below
    //   Package {
    //      Revision
    //      Power Unit
    //      Design Capacity
    //      Last Full Charge Capacity
    //      Battery Technology
    //      Design Voltage
    //      Design Capacity of Warning
    //      Design Capacity of Low
    //      Cycle Count
    //      Measurement Accuracy
    //      Max Sampling Time
    //      Min Sampling Time
    //      Max Averaging Interval
    //      Min Averaging Interval
    //      Battery Capacity Granularity 1
    //      Battery Capacity Granularity 2
    //      Model Number
    //      Serial Number
    //      Battery Type
    //      OEM Information
    //   }
    //
    Method(_BIX,,,,PkgObj)
    {
      If(CondRefOf(\_SB.PCI0.LPCB.H_EC.BAT1._BIX))
      {
        Return(\_SB.PCI0.LPCB.H_EC.BAT1._BIX())
      } Else {
        Return(Package(){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"0","0","0","0"})
      }
    }

    // PSOC (Platform State of Charge)
    //
    // This object evaluates to the remaining battery state of charge in %.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   remaining battery charge in %
    //
    Method(PSOC)
    {
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,0)) // check EC opregion available
      {
        Return (0) // EC not available
      }
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)),0)) // prevent divide by zero
      {
        Return (0) // Full charge cannot be 0, indicate malfunction in battery
      }
      If(LGreater(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)))) // remaining charge > full charge
      {
        Return(0) // Remaining Charge cannot be greater than Full Charge, this illegal value indicates the battery is malfunctioning
      }
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)))) // remaining charge == full charge
      {
        Return(100) // indicate battery is fully charged
      }
      If(LLess(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)))) // (RC*100)/FC=percent
      {
        Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)),100,Local0) // RC*100
        Divide(Local0,\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)),Local2,Local1) // divide by FC
        Divide(Local2,100,,Local2) // account for the remainder in percent
        Divide(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)),200,,Local3) // what number is 0.5%?
        If(LGreaterEqual(Local2,Local3)){ // round up if the remainder is equal or above .5
          Add(Local1,1,Local1)
        }
        Return(Local1) // current charge in percent
      } Else {
        Return (0) // avoid compiler warning
      }
    }

    // PMAX (Platform MAXimum)
    //
    // The PMAX object provides maximum power that can be supported by the battery in mW.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   maximum power in mW
    //
    Method(PMAX,0,Serialized)
    {
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // Check If EC opregion is available
      {
        // convert PMAX from signed to unsigned integer
        // PMAX Value = (NOT(OR(FFFF0000, ECValue))+1) * 10.
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.BMAX)),Local0)
        If(Local0){ // don't convert a zero
          Not(Or(0xFFFF0000,Local0,Local0),Local0) // make it a 32 bit value before inverting the bits
          Multiply(Add(1,Local0,Local0),10,Local0) // add 1 and multiply by 10
        }
        Return(Local0)
      } Else {
        Return(0) // EC not available
      }
    }

    // NPWR (N PoWeR)
    //
    // True Platform Power: Could be AC supplied+Battery Pack supplied power or AC supplied.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Power required to charge battery in mW.
    //
    Method(NPWR)
    {
      Return(20000) // 20 watts
    }

    // PSRC (Power SouRCe)
    //
    // The PSRC object provides power source type.
    //
    // Arguments: (0)
    //   None
    // Return Value:(enumeration which DPTF driver expects)
    //   Bit[3:0] = Power Resource Type
    //   0x00 = DC
    //   0x01 = AC
    //   0x02 = USB
    //   0x03 = WC
    //   Bit[7:4] = Power Delivery State Change Sequence Number
    //
    // Notes: (Bitmap from EC for Non SDS)
    //   Bit0=1 if AC
    //   Bit1=1 if USB-PD
    //   Bit2=1 if Wireless Charger
    //
    // Notes: (Bitmap from EC for SDS)
    //   Bit0=1 PwrSrcType valid, 0=no longer valid(surprise Undock)
    //   Bit1=1 Power source is Battery
    //   Bit2=1 USB-PD
    //   Bit3=1 Wireless Charging
    //
    Method(PSRC,0, Serialized)
    {
     If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,0))
      {
        Return(0) // EC not available, default to battery
      } Else {
//
// KblGBegin
//
      If (LEqual (PBEN, 1)) {
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PWRN)),Local0)
      } Else {
//
// KblGEnd
//
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PWRT)),Local0)
      } // KblGContent
      And (Local0, 0xF0, Local1) // Keep Bit[7:4], clear Bit[3:0] in Local1
      }

      If(LEqual(TPPT, 0))
      {
        // For NON SDS systems
        Switch(ToInteger(And(ToInteger(Local0),0x07))) // Switch Power Source Type Bits [3:0]
        {
          Case(0x0) { // Power source is DC
            Or (Local1, 0x00, Local1)
          }
          Case(0x1) { // Power source is AC
            Or (Local1, 0x01, Local1)
          }
          Case(0x2) { // Power source is USB-PD
            Or (Local1, 0x02, Local1)
          }
          Case(0x4) { // Power source is Wireless Charging
            Or (Local1, 0x03, Local1)
          }
          Default {
            Or (Local1, 0x00, Local1) //default to battery
          }
        } // end of switch case check
      } Else {
        // For SDS systems
        Switch(ToInteger(And(ToInteger(Local0),0x0F))) // Switch Power Source Type Bits [3:0]
        {
          Case(0x03) { // Power source is DC
            Or (Local1, 0x00, Local1)
          }
          Case(0x01) { // Power source is AC
            Or (Local1, 0x01, Local1)
          }
          Case(0x05) { // Power source is USB-PD
            Or (Local1, 0x02, Local1)
          }
          Case(0x09) { // Power source is Wireless Charging
            Or (Local1, 0x03, Local1)
          }
          Default {
            Or (Local1, 0x00, Local1) // default to battery
          }
        } // end of switch case check
      }
      Return(Local1)
   }

    // ARTG (Adapter RaTinG)
    //
    // The ARTG object provides AC adapter rating in mW.
    // ARTG should return 0 if PSRC is DC (0).
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   AC adapter rating in mW
    //
    Method(ARTG)
    {
      If(LEqual(PSRC(),0)) {
        Return(0)
      } 
//
// KblGBegin
//
      If (LAnd((LEqual (PBEN, 1)),(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)))) { // Check if PBOK is enabled and check If EC opregion is available
        Return(Multiply(\_SB.PCI0.LPCB.H_EC.ARTG, 10))
      } 
//
// KblGEnd
//
	  Else {
        Return(90000) // 90 watts
      }
    }

    // LSOC (Long term SoC)
    //
    // Long term battery charge is based on c/20 SoC. Every vendor has a different terminology.
    // This is the value predicted by the FG based on light/no load, in %.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Long term battery charge in %.
    //
    Method(LSOC)
    {
      Return(50) // 50%
    }

    // CTYP (Charger TYPe)
    //
    // The CTYP object provides charger type.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //  0x01 = Traditional
    //  0x02 = Hybrid
    //  0x03 = NVDC
    //
    Method(CTYP)
    {
      Return(3)
    }

    // PROP (Platform Rest Of worst case Power)
    //
    // This object provides maximum worst case platform rest of power.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   power in milliwatts
    //
    Method(PROP)
    {
      Return(25000) // 25 watts
    }

    // APKP (Ac PeaK Power)
    //
    // This object provides maximum adapter power output.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   power in milliwatts
    //
    Method(APKP)
    {
      Return(90000) // 90 watts
    }

    // APKT (Ac PeaK Time)
    //
    // This object provides the maximum time the adapter can maintain peak power.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   time in milliseconds
    //
    Method(APKT)
    {
      Return(10) // 10 milliseconds
    }

    // PBSS (Power Battery Steady State)
    //
    // Returns max sustained power for battery.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   power in milliwatts
    //
    Method(PBSS)
    {
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) {
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PBSS)),Local0)
        Return(Local0)
       }
       Return(100)
    }

    // DPSP (DPTF Power Sampling Period)
    //
    // Sets the polling interval in 10ths of seconds. A value of 0 tells the driver to use event notification for PMAX and PBSS..
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the polling rate in tenths of seconds.
    //   A value of 0 will specify using interrupts through the ACPI notifications.
    //
    //   The granularity of the sampling period is 0.1 seconds. For example, if the sampling period is 30.0
    //   seconds, then report 300; if the sampling period is 0.5 seconds, then will report 5.
    //
    Method(DPSP,0,Serialized)
    {
      Return(\PPPR)
    }
//
// KblGBegin
//
    // PBOK
    //
    // PBOK is a command which BIOS sends to EC. 
    // It will have a parameter which is the sequence number.
    //
    // Arguments: (1)
    //   Arg0 should be sent by DPTF driver which will be corresponding to Power delivery state change sequence number.
    //   Bit[3:0] = Power Delivery State Change Sequence number
    // Return Value:
    //   None
    //
    Method(PBOK,1,Serialized)
    {
      If (LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // Check If EC opregion is available
      {
        Store (And(Arg0, 0x000F), Local0)
        \_SB.PCI0.LPCB.H_EC.ECWT (Local0, RefOf(\_SB.PCI0.LPCB.H_EC.PBOK)) // Store sequence number
        \_SB.PCI0.LPCB.H_EC.ECMD (0x15)
      }
    }
//
// KblGEnd
//
  } // End TPWR Device
}// end Scope(\_SB)
