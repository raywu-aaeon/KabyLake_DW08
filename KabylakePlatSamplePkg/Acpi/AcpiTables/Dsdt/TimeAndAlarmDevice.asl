/**@file
  ACPI Time and Wake Alarm Device

@copyright
 INTEL CONFIDENTIAL
  Copyright 2018 Intel Corporation.

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

External(PCRR, MethodObj)
External(PCRW, MethodObj)

Scope (\_SB) {
  Device (AWAC) { // Time and Wake Alarm Device
    Name (_HID, "ACPI000E")

    Name (WAST, 0) // Wake alarm status variable. 1 = Wake alarm set. 0 = Wake alarm not set
    Name (WTTR, 0) // Wake timer track variable. Bit0 - AC timer is used, Bit1 - DC timer is used.

    Method (_PRW, 0) { Return(GPRW(0x72, 4)) }  // GPE 114, can wakeup from S4 state

    Method(_STA) {
      If (LEqual(\STAS,0)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    Method (_GCP, 0) {
      // Return:
      // Bit[0] : AC wake implemented.
      // Bit[1] : DC wake implemented.
      // Bit[2] : Get/Set real time features implemented.
      // Bit[3] : Real time accuracy. 0 = Seconds, 1 = Milliseconds.
      // Bit[4] : _GWS returns correct values for wakes from S4/S5 caused by timer. 0 = not supported.
      // Bit[5] : 1 = Wake supported from S4 on AC, 0 = Wake not supported from S4 on AC
      // Bit[6] : 1 = Wake supported from S5 on AC, 0 = Wake not supported from S5 on AC
      // Bit[7] : 1 = Wake supported from S4 on DC, 0 = Wake not supported from S4 on DC
      // Bit[8] : 1 = Wake supported from S5 on DC, 0 = Wake not supported from S5 on DC
      // Bit[9] to Bit[31] are reserved and must be 0.
      Return(0xB7) // AC/DC wake supported. Get/Set real time features implemented. _GWS returns the correct value from S4/S5 wake. Wake from S4 supported for AC/DC
    }
    OperationRegion(RTCM, SystemCMOS, 0, 0x3F)
    Field(RTCM, ByteAcc, Lock, Preserve)
    {
      Offset(0),    SEC,  8,
      Offset(2),    MIN,  8,
      Offset(4),    HOR,  8,
      Offset(7),    DAY,  8,
      Offset(8),    MON,  8,
      Offset(9),    YEAR, 8,
      Offset(0x32), CNTY, 8
    }

    Method (_GRT, 0) { // Get Real Time
//    Buffer(){
//    WORD Year;        // Index 09h    // 1900 - 9999
//    BYTE Month;       // Index 08h    // 1 - 12
//    BYTE Day;         // Index 07h    // 1 - 31
//    BYTE Hour;        // Index 04h    // 0 - 23
//    BYTE Minute;      // Index 02h    // 0 - 59
//    BYTE Second;      // Index 00h    // 0 - 59
//    BYTE Valid;       //              // 0 - Time is not valid (request failed); 1 - time is valid
//    WORD milliseconds;// Index no     // 1 - 1000
//    WORD TimeZone;    //              // -1440 to 1440 or 2047 (unspecified)
//    BYTE Daylight;
//    BYTE Pad2[3];     // Reserved, must be zero
//    }
      // Read RTC Date and Time and construct Return Buffer here- see Chp 9.18 in ACPI spec
      Store(Buffer(0x10){}, Local0)  // Create buffer to hold return structure
      CreateWordField(Local0, 0x0, Y)     // Year
      CreateByteField(Local0, 0x2, M)     // Month
      CreateByteField(Local0, 0x3, D)     // Day
      CreateByteField(Local0, 0x4, H)     // Hour
      CreateByteField(Local0, 0x5, MIN1)   // Minute
      CreateByteField(Local0, 0x6, S)     // Second
      CreateByteField(Local0, 0x7, V)     // Valid
      //CreateWordField(Local0, 0x8, MS)    // Milliseconds
      CreateWordField(Local0, 0xA, TZ)    // TimeZone
      CreateByteField(Local0, 0xC, DL)    // Daylight
      //CreateField(Local0, 0x68, 0x18, P2) // Pad2

      FromBCD(YEAR,Local5)
      FromBCD(CNTY,Local6)

      Add(Multiply(Local6,100),Local5,Y)
      FromBCD(MON,M)
      FromBCD(DAY,D)
      FromBCD(HOR,H)
      FromBCD(MIN,MIN1)
      FromBCD(SEC,S)

      Store(2047,TZ)
      Store(0,DL)
      Store(1,V)
      Return (Local0)
    }

    Method (_SRT, 1) {
      // Read RTC Date and Time and construct Return Buffer here- see page 564 in ACPI spec
      //Store(Buffer(0x10){}, Local0)        // Create buffer to hold return structure
      CreateWordField(Arg0, 0x0, Y)       // Year
      CreateByteField(Arg0, 0x2, M)       // Month
      CreateByteField(Arg0, 0x3, D)       // Day
      CreateByteField(Arg0, 0x4, H)       // Hour
      CreateByteField(Arg0, 0x5, MIN1)    // Minute
      CreateByteField(Arg0, 0x6, S)       // Second
      //CreateByteField(Arg0, 0x7, V)       // Valid
      //CreateWordField(Arg0, 0x8, MS)      // Milliseconds
      //CreateWordField(Arg0, 0xA, TZ)      // TimeZone
      //CreateByteField(Arg0, 0xC, DL)      // Daylight
      //CreateField(Local0, 0x68, 0x18, P2) // Pad2


      // Convert 4 digit ACPI Year to 2 digit RTC century and year
      Divide(Y,100,Local5,Local4)

      // Update RTC values
      ToBCD(Local4, CNTY)
      ToBCD(Local5, YEAR)
      ToBCD(M, MON)
      ToBCD(D, DAY)
      ToBCD(H, HOR)
      ToBCD(MIN1, MIN)
      ToBCD(S, SEC)

      Return(0x0) // Success
    }

    Method (_GWS, 1, Serialized) { // Get Wake Alarm Status
      // Arg0 - Timer Identifier. 0 - AC Timer, 1 - DC Timer
      // Return: Integer(DWORD)
      // Bit0 - 1 = Timer expired, 0 = Timer did not expired.
      // Bit1 - 1 = Timer caused a platform wake, 0 = Timer did not cause a platform wake.
      // Bit[31:2] - Reserved.
      Store (0, Local0)

      If (LEqual(Arg0, 0)){ // AC timer
        If (LAnd(LEqual(\ACWA, 0xFFFFFFFF), And(BIT0, WTTR))){ // AC Timer expired.
          Or (BIT0, Local0, Local0)
          Xor (BIT0, WTTR, WTTR) // Clear AC timer tracker
        }
      }
      ElseIf (LAnd(LEqual(\DCWA, 0xFFFFFFFF),And(BIT1, WTTR))){ // DC Timer expired.
        Or (BIT0, Local0, Local0)
        Xor (BIT1, WTTR, WTTR) // Clear DC timer tracker
      }
      If (WAST) { // Wake Alarm Device Status is set.
        Or (BIT1, Local0, Local0)
        Store (0, WAST)
      }
      Return (Local0)
    }

    Method (_CWS, 1) { // Clear Wake Alarm Status
      // Both AC/DC wake up status will be reflected to WADT_STS (Bit18 of GPE0_STS[127:96])
      // This bit is located inside GPE so _PRW and _L72 will take of this. We will just return with success.
      // Arg0: Timer Identifier. Since we don't have independent wake state per Timer, we can ignore the Arg0.
      // Return 0 - Success, 1 - Failure
      Return (0) // Always success
    }

    Method (_STP, 2) { // Set Expired Timer Wake Policy
      // Arg0: Timer Identifier (Integer DWORD) 0 - AC Timer, 1 - DC Timer
      // Arg1: Expired Timer Wake Policy (Integer DWORD). This argument contains the value to be set to Wake Alarm Device Expired Timer.
      //       0            - System will wake up instantly after the power source change.
      //       1-0xFFFFFFFE - Time(seconds) till the system wake after the power source change.
      //       0xFFFFFFFF   - The timer will not wake the system after the power source change.
      // Return 0 - Success, 1 - Failure
      If (LEqual(Arg0, 0)) { // AC Timer
        Store (Arg1, \ACET)
      } Else { // DC Timer
        Store (Arg1, \DCET)
      }
      Return (0) // Always success
    }

    Method (_STV, 2, Serialized) { // Set Timer Value
      // Arg0: Timer Identifier (Integer DWORD) 0 - AC Timer, 1 - DC Timer
      // Arg1: Timer Value (Integer). This argument contains the value (seconds) to be set to Wake Alarm Device Timer.
      // Return 0 - Success, 1 - Failure
      If (LEqual(Arg0, 0)) { // AC Timer
        Store (Arg1, \ACWA)
        Or (BIT0, WTTR, WTTR)
      } Else { // DC Timer
        Store (Arg1, \DCWA)
        Or (BIT1, WTTR, WTTR)
      }
      Return (0) // Success
    }

    Method (_TIP, 1) { // Expired Timer Wake Policy
      // Arg0: Timer Identifier (Integer DWORD) 0 - AC Timer, 1 - DC Timer
      // Return: Integer DWORD, Expired Timer Policy value from Wake Alarm Device Expired Timer.

      If (LEqual(Arg0, 0)) { // AC Timer Wake Policy
        Return (\ACET)
      } Else { // DC Timer Wake Policy
        Return (\DCET)
      }
    }

    Method (_TIV, 1) { // Timer Values
      // Arg0: Timer Identifier (Integer DWORD) 0 - AC Timer, 1 - DC Timer
      // Return: Integer DWORD, Timer value from Wake Alarm Device Timer.

      If (LEqual(Arg0, 0)) { // AC Timer
        Return (\ACWA)
      } Else { // DC Timer
        Return (\DCWA)
      }
    }
  } // Device (AWAC)
} // Scope (\_SB)