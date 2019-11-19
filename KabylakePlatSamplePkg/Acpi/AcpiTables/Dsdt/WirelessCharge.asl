/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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
  Device(WCT0)
  {
    Name(_HID, EISAID ("IWC0001"))
    Name(_UID, 0x0)
    Method(_STA, 0, NotSerialized) {
      if(LEqual(WIFC,1))
      {
        Return (0xF)
      }
      else
      {
        Return (0)
      }
    }

    // PCMD method, send a PRU command
    Method(PCMD, 1, NotSerialized) {
      // Arg0: Buffer
      //   BYTE 0: BIT 7 = 1 a command (not an ITP packet) to EC
      //           BIT 0 - 6 for the packet length, excluding the length itself
      //   BYTE 1: command code for EC to control PRU
      //     0x01: get WC device information.
      //           EC will read PRU I2C register WC_INFO and copy the bytes into BUFF, and notify iWC driver
      //     0x02: set a firmware update mode.
      //           EC will set PRU or PTU update mode in I2C register CTL bits, and no any data return to caller
      //     0x03: get WC status.
      //           EC will read I2C register FLAG and fill it in BUFF, and notify iWC driver
      //   BYTE 2 - 19: parameters for the command
      //     for command get WC device information: no parameter
      //     for command set firmware update mode: 0x01 set PRU update mode; 0x02 set PTU update mode
      //     for command get WC status: no parameter
      //
      // Arg0 should be forwarded to lid EC, which should forward it to base EC for execution
      //
      // For any command there is a result returning to iWC driver,
      // EC will copy the data into BUFF, and notify iWC driver, which will call ITPR to retrieve the data bytes

      // Below is a loopback implementation:
      if (\_SB.PCI0.LPCB.H_EC.ECAV) {
        Store(Arg0, \_SB.PCI0.LPCB.H_EC.IWCW) // put data in EC buffer
        \_SB.PCI0.LPCB.H_EC.ECMD(0xE1)        // issue EC command, ask EC to read it
      }
    }

    // ITPW method (Image Transport Protocol Write) asks EC to send an ITP packet to PRU over I2C register ITP_IN
    // Note: Implementation should be similar to PCMD.
    Method(ITPW, 1, NotSerialized) {
      // Arg0: ITP packet buffer.
      //   BYTE 0: BIT 7 = 0 an ITP packet (not a command) to EC
      //           BIT 0 - 6:  contains the length, excluding the length itself
      //   BYTE 1 - 19: ITP packet starting from sequence number (length will be in BYTE 0)
      //
      // Arg0 should be forwarded to the lid EC,
      // which should forward it to base EC to transfer it to I2C register ITP_IN
      // and then EC will get ALERT = 1, and if FLAG.ITP_RDY =1, ([GL] question: can EC send a notiifcation to iWC driver when ITPW failed due to PRU is off? and then iWC driver will call ITPR to get the error code)
      // the base EC will read an ITP response from I2C register ITP_OUT and notify lid EC,
      // which fills BUFF and then notify iWC driver with notification 0x80.
      // iWC driver will call ITPR to read back the ITP response packet.

      // Below is a loopback implementation:
      if (\_SB.PCI0.LPCB.H_EC.ECAV) {
        Store(Arg0, \_SB.PCI0.LPCB.H_EC.IWCW) // put data in EC buffer
        \_SB.PCI0.LPCB.H_EC.ECMD(0xE1)        // issue EC command, ask EC to read it
      }
    }

    // ITPR method (Image Transport Protocol Read)
    // Read an ITP response packet generated due to calling ITPW, or a command response due to calling PCMD
    Method(ITPR, 0, NotSerialized) {
      // Arg0:
      //   BYTE 0: BIT 7 = 0: the data is an ITP response packet in response to the last ITPW call
      //           BIT 7 = 1: the data is a response to the last PCMD call
      //           BIT 0 - 6: the length of the return data, excluding the length itself
      // For a PCMD response:
      //   BYTE 1: the command code sent in PCMD
      //   BYTE 2: completion code (0x00 Ok, 0xE1 communication error)
      //   BYTE 3 - 19: command response data
      // For an ITPW response packet:
      //   BYTE 1 - 19: an ITP response packet
      //
      // Base EC send the data to lid EC which will copy it into BIOS BUFF,
      // and then BIOS notifies iWC driver with notification 0x80, which calls ITPR to retrieve the buffer.
      if (\_SB.PCI0.LPCB.H_EC.ECAV) {
        Return (\_SB.PCI0.LPCB.H_EC.IWCR) // Return buffer
      }
      Return(Package(){})
    }

    Method (PSRC, 0, NotSerialized) {
      // Arg0 : None
      // Return - EC Offset 121(0x79)
      // For SKL SDS:-
      // PwrSrcType Bit0=1 PwrSrcType valid, 0=no longer valid(surprise Undock), Bit1=1 Power source is Battery,
      //            Bit2=1 USB-PD, Bit3=1 Wireless Charging
      // For Others:-
      // PwrSrcType Bit0=1 if AC, Bit1=1 if USB-PD, Bit2=1 if Wireless Charging

      //@todo: Check if other platforms need wireless charging support and modify return statment according to driver expectation.
      Return (\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PWRT)))
    }
  } // end Device(WCT0)
} // End Scope(\_SB)