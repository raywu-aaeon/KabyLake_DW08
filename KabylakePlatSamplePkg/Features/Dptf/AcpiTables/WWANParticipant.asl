/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2016 Intel Corporation.

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

Scope(\_SB)
{

  Device(WWAN)  // WWAN temperature sensor
  {
    Name(_HID, EISAID("INT3408"))
    Name(_UID, "WWAN")
    Name(_STR, Unicode("Intel DPTF WWAN Participant"))
    Name(PTYP, 0x0F)
    Name(CTYP,0)  // Mode

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
      If (LEqual(WAND,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // Thermal Sensor Hysteresis, 2 degrees
    Name(GTSH, 20)

    Name(LSTM,0)  // Last temperature reported

    // _DTI (Device Temperature Indication)
    //
    // Conveys the temperature of a device's internal temperature sensor to the platform when a temperature trip point
    // is crossed or when a meaningful temperature change occurs.
    //
    // Arguments: (1)
    //   Arg0 - An Integer containing the current value of the temperature sensor (in tenths Kelvin)
    // Return Value:
    //   None
    //
    Method(_DTI, 1)
    {
      Store(Arg0,LSTM)
      Notify(\_SB.WWAN, 0x91) // notify the participant of a trip point change event
    }

    // _NTT (Notification Temperature Threshold)
    //
    // Returns the temperature change threshold for devices containing native temperature sensors to cause
    // evaluation of the _DTI object
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the temperature threshold in tenths of degrees Kelvin.
    //
    Method(_NTT, 0)
    {
      Return(2782)  // 5 degree Celcius, this could be a platform policy with setup item
    }

    // _ACx (Active Cooling)
    //
    // This optional object, if present under a thermal zone, returns the
    //  temperature trip point at which OSPM must start or stop Active cooling,
    //  where x is a value between 0 and 9 that designates multiple active cooling levels of the thermal zone.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC0,0,Serialized)
    {
      If(CTYP)
      {
        If (LEqual(WWPT,0)) {Return(0xFFFFFFFF)} // signal DPTF driver to disable trip point
        Store(\_SB.IETM.CTOK(WWPT),Local1) // Passive Cooling Policy
      } Else {
        If (LEqual(WWAT,0)) {Return(0xFFFFFFFF)} // signal DPTF driver to disable trip point
        Store(\_SB.IETM.CTOK(WWAT),Local1) // Active Cooling Policy
      }
      If(LGreater(LSTM,Local1))
      {
        Return(Subtract(Local1,GTSH)) // subtract 2 degrees which is the Hysteresis
      }
      Else
      {
        Return(Local1)
      }
    }

    // _PSV (Passive)
    //
    // This optional object, if present under a thermal zone, evaluates to the temperature
    //  at which OSPM must activate passive cooling policy.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the passive cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_PSV,0,Serialized)
    {
      If(CTYP)
      {
        If (LEqual(WWAT,0)) {Return(0xFFFFFFFF)} // signal DPTF driver to disable trip point
        Return(\_SB.IETM.CTOK(WWAT)) // Active Cooling Policy
      } Else {
        If (LEqual(WWPT,0)) {Return(0xFFFFFFFF)} // signal DPTF driver to disable trip point
        Return(\_SB.IETM.CTOK(WWPT)) // Passive Cooling Policy
      }
    }

    // _CRT (Critical Temperature)
    //
    // This object, when defined under a thermal zone, returns the critical temperature at which OSPM must shutdown the system.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the critical temperature threshold in tenths of degrees Kelvin
    //
    Method(_CRT,0,Serialized)
    {
      If (LEqual(WWCT,0)) {Return(0xFFFFFFFF)} // signal DPTF driver to disable trip point
      Return(\_SB.IETM.CTOK(WWCT))
    }

    // _CR3 (Critical Temperature for S3/CS)
    //
    // This object, when defined under a thermal zone, returns the critical temperature at which OSPM
    // must transition to Standby or Connected Standy.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the critical temperature threshold in tenths of degrees Kelvin
    //
    Method(_CR3,0,Serialized)
    {
      If (LEqual(WWC3,0)) {Return(0xFFFFFFFF)} // signal DPTF driver to disable trip point
      Return(\_SB.IETM.CTOK(WWC3))
    }

    // _HOT (Hot Temperature)
    //
    // This optional object, when defined under a thermal zone, returns the critical temperature
    //  at which OSPM may choose to transition the system into the S4 sleeping state.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    The return value is an integer that represents the critical sleep threshold tenths of degrees Kelvin.
    //
    Method(_HOT,0,Serialized)
    {
      If (LEqual(WWHT,0)) {Return(0xFFFFFFFF)} // signal DPTF driver to disable trip point
      Return(\_SB.IETM.CTOK(WWHT))
    }

    // _SCP (Set Cooling Policy)
    //
    //  Arguments: (3)
    //    Arg0 - Mode An Integer containing the cooling mode policy code
    //    Arg1 - AcousticLimit An Integer containing the acoustic limit
    //    Arg2 - PowerLimit An Integer containing the power limit
    //  Return Value:
    //    None
    //
    //  Argument Information:
    //    Mode - 0 = Active, 1 = Passive
    //    Acoustic Limit - Specifies the maximum acceptable acoustic level that active cooling devices may generate.
    //    Values are 1 to 5 where 1 means no acoustic tolerance and 5 means maximum acoustic tolerance.
    //    Power Limit - Specifies the maximum acceptable power level that active cooling devices may consume.
    //    Values are from 1 to 5 where 1 means no power may be used to cool and 5 means maximum power may be used to cool.
    //
    Method(_SCP, 3, Serialized)
    {
      If(LOr(LEqual(Arg0,0),LEqual(Arg0,1)))
      {
        Store(Arg0, CTYP)
        P8XH(0, Arg1)
        P8XH(1, Arg2)
        Notify(\_SB.WWAN, 0x91)
      }
    }

    Name(VERS,0)  // Version
    Name(ALMT,0)  // Acoustic Limit
    Name(PLMT,0)  // Power Limit
    Name(WKLD,0)  // Workload Hint
    Name(DSTA,0)  // Device State Hint
    Name(RES1,0)  // Reserved 1

    // DSCP (DPTF Set Cooling Policy)
    //
    //  Arguments: (7)
    //    Arg0 - Version: For DPTF 8.0, this value is always 0.
    //    Arg1 - Mode: Integer containing the cooling mode policy code
    //    Arg2 - Acoustic Limit: Acoustic Limit value as defined in ACPI specification
    //    Arg3 - Power Limit: Power Limit value as defined in ACPI specification
    //    Arg4 - Workload Hint: Arbitrary Platform defined Integer that indicates to the platform the type of workload run in the OS.
    //    Arg5 - Device State Hint: An integer value that indicates the state of the device.
    //    Arg6 - Reserved 1
    //  Return Value:
    //    None
    //
    //  Argument Information:
    //    Mode: 0 = Active(Typically AC Power Source), 1 = Passive(Typically Battery Power Source)
    //    Acoustic Limit: Specifies the maximum acceptable acoustic level that active cooling devices may generate.
    //      Values are 1 to 5 where 1 means no acoustic tolerance and 5 means maximum acoustic tolerance.
    //    Power Limit: Specifies the maximum acceptable power level that active cooling devices may consume.
    //      Values are from 1 to 5 where 1 means no power may be used to cool and 5 means maximum power may be used to cool.
    //    Workload Hint: Refer to Intel DPTF Configuration Guide on how to set this value for various workloads for each Operating System.
    //    Device State Hint: An integer value that indicates the state of the device.
    //      First byte indicates portrait or landscape mode.
    //      Second byte indicates horizontal or vertical orientation.
    //      Third byte indicates proximity sensor status (if available).
    //      Fourth byte is unused.
    //      xxxxxx00h: Portrait
    //      xxxxxx01h: Landscape
    //      xxxx00xxh: Horizontal
    //      xxxx01xxh: Vertical
    //      xx00xxxxh: Proximity sensor Off (Device not in proximity to user)
    //      xx01xxxxh: Proximity sensor On (Device in proximity to user)
    //
    Method(DSCP, 7, Serialized)
    {
      If(LOr(LEqual(Arg1,0),LEqual(Arg1,1)))
      {
        Store(Arg0, VERS)
        Store(Arg1, CTYP)
        Store(Arg2, ALMT)
        Store(Arg3, PLMT)
        Store(Arg4, WKLD)
        Store(Arg5, DSTA)
        Store(Arg6, RES1)
        P8XH(0, Arg2)
        P8XH(1, Arg3)
        Notify(\_SB.WWAN, 0x91)
      }
    }

  } // end of Device(WWAN)
} // end of Scope(\_SB)
