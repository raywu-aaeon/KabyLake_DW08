/** @file
 Thunderbolt ACPI methods

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
// AMI_OVERRIDE_START - Fixed build errors with the latest ASL compiler
// The following external declarations have to be removed, cause External declaration cannot be used if method is defined in the same file.
//External(\_SB.PCI0.RP01.PXSX, DeviceObj)
//External(\_SB.PCI0.RP02.PXSX, DeviceObj)
//External(\_SB.PCI0.RP03.PXSX, DeviceObj)
//External(\_SB.PCI0.RP04.PXSX, DeviceObj)
//External(\_SB.PCI0.RP05.PXSX, DeviceObj)
//External(\_SB.PCI0.RP06.PXSX, DeviceObj)
//External(\_SB.PCI0.RP07.PXSX, DeviceObj)
//External(\_SB.PCI0.RP08.PXSX, DeviceObj)
//External(\_SB.PCI0.RP09.PXSX, DeviceObj)
//External(\_SB.PCI0.RP10.PXSX, DeviceObj)
//External(\_SB.PCI0.RP11.PXSX, DeviceObj)
//External(\_SB.PCI0.RP12.PXSX, DeviceObj)
//External(\_SB.PCI0.RP13.PXSX, DeviceObj)
//External(\_SB.PCI0.RP14.PXSX, DeviceObj)
//External(\_SB.PCI0.RP15.PXSX, DeviceObj)
//External(\_SB.PCI0.RP16.PXSX, DeviceObj)
//External(\_SB.PCI0.RP17.PXSX, DeviceObj)
//External(\_SB.PCI0.RP18.PXSX, DeviceObj)
//External(\_SB.PCI0.RP19.PXSX, DeviceObj)
//External(\_SB.PCI0.RP20.PXSX, DeviceObj)
// AMI_OVERRIDE_END - Fixed build errors with the latest ASL compier
External(\_SB.PCI0.PEG0.PG00.PEGP, DeviceObj)
External(\_SB.PCI0.PEG1.PG01.PEGP, DeviceObj)
External(\_SB.PCI0.PEG2.PG02.PEGP, DeviceObj)

  //
  // Name: TBTD
  // Description: Function to return the TBT RP# device no
  // Input: Arg0 -> Tbt selector value from setup
  // Return: TBT RP# device no
  //
  Method(TBTD,1)
  {
    ADBG("TBTD")

    Switch(ToInteger(Arg0))
    {
      Case (Package () {1, 2, 3, 4, 5, 6, 7, 8})
      {
        Store(0x1C, Local0) //Device28-Function0...Function7 = 11100.000...111
      }
      Case (Package () {9, 10, 11, 12, 13, 14, 15, 16})
      {
        Store(0x1D, Local0) //Device29-Function0...Function7 = 11101.000...111
      }
// AMI_OVERRIDE_START - EIP279338 The resource for PEG Thunderbolt host is incorrect.
      Case (Package () {17, 18, 19, 20, 21, 22, 23, 24}) 
      {
        Store(0x1B, Local0) //Device27-Function0...Function7 = 11011.000...111
      }
      Case (Package () {25, 26, 27})
// AMI_OVERRIDE_END - EIP279338 The resource for PEG Thunderbolt host is incorrect.
      {
        Store(0x1, Local0) //Device1-Function0...Function2 = 00001.000...010
      }
    }

    ADBG("Device no")
    ADBG(Local0)

    Return(Local0)
  } // End of Method(TBTD,1)

  //
  // Name: TBTF
  // Description: Function to return the TBT RP# function no
  // Input: Arg0 -> Tbt selector value from setup
  // Return: TBT RP# function no
  //
  Method(TBTF,1)
  {
    ADBG("TBTF")

    Switch(ToInteger(Arg0))
    {
      Case (1)
      {
        Store(And(\RPA1,0xF), Local0) //Device28-Function0 = 11100.000
      }
      Case (2)
      {
        Store(And(\RPA2,0xF), Local0) //Device28-Function1 = 11100.001
      }
      Case (3)
      {
        Store(And(\RPA3,0xF), Local0) //Device28-Function2 = 11100.010
      }
      Case (4)
      {
        Store(And(\RPA4,0xF), Local0) //Device28-Function3 = 11100.011
      }
      Case (5)
      {
        Store(And(\RPA5,0xF), Local0) //Device28-Function4 = 11100.100
      }
      Case (6)
      {
        Store(And(\RPA6,0xF), Local0) //Device28-Function5 = 11100.101
      }
      Case (7)
      {
        Store(And(\RPA7,0xF), Local0) //Device28-Function6 = 11100.110
      }
      Case (8)
      {
        Store(And(\RPA8,0xF), Local0) //Device28-Function7 = 11100.111
      }
      Case (9)
      {
        Store(And(\RPA9,0xF), Local0) //Device29-Function0 = 11101.000
      }
      Case (10)
      {
        Store(And(\RPAA,0xF), Local0) //Device29-Function1 = 11101.001
      }
      Case (11)
      {
        Store(And(\RPAB,0xF), Local0) //Device29-Function2 = 11101.010
      }
      Case (12)
      {
        Store(And(\RPAC,0xF), Local0) //Device29-Function3 = 11101.011
      }
      Case (13)
      {
        Store(And(\RPAD,0xF), Local0) //Device29-Function4 = 11101.100
      }
      Case (14)
      {
        Store(And(\RPAE,0xF), Local0) //Device29-Function5 = 11101.101
      }
      Case (15)
      {
        Store(And(\RPAF,0xF), Local0) //Device29-Function6 = 11101.110
      }
      Case (16)
      {
        Store(And(\RPAG,0xF), Local0) //Device29-Function7 = 11101.111
      }
      Case (17)
      {
        Store(And(\RPAH,0xF), Local0) //Device27-Function0 = 11011.000
      }
      Case (18)
      {
        Store(And(\RPAI,0xF), Local0) //Device27-Function1 = 11011.001
      }
      Case (19)
      {
        Store(And(\RPAJ,0xF), Local0) //Device27-Function2 = 11011.010
      }
      Case (20)
      {
        Store(And(\RPAK,0xF), Local0) //Device27-Function3 = 11011.011
      }
// AMI_OVERRIDE_START - EIP279338 The resource for PEG Thunderbolt host is incorrect.
      Case (21)
      {
        Store(And(\RPAL,0xF), Local0) //Device27-Function4 = 11011.100
      }
      Case (22)
      {
        Store(And(\RPAM,0xF), Local0) //Device27-Function5 = 11011.101
      }
      Case (23)
      {
        Store(And(\RPAN,0xF), Local0) //Device27-Function6 = 11011.110
      }
      Case (24)
      {
        Store(And(\RPAO,0xF), Local0) //Device27-Function7 = 11011.111
      }
      Case (25)
      {
        Store(0x0, Local0) //Device1-Function0 = 00001.000
      }
      Case (26)
      {
        Store(0x1, Local0) //Device1-Function1 = 00001.001
      }
      Case (27)
      {
        Store(0x2, Local0) //Device1-Function2 = 00001.010
      }
// AMI_OVERRIDE_END - EIP279338 The resource for PEG Thunderbolt host is incorrect.
    }

    ADBG("Function no")
    ADBG(Local0)

    Return(Local0)
  } // End of Method(TBTF,1)

  //
  // Name: MMRP
  // Description: Function to return the Pci base address of TBT rootport
  //

  Method(MMRP,1, Serialized)
  {
    Store(\_SB.PCI0.GPCB(), Local0) // MMIO Base address
    Add(Local0, ShiftLeft(TBTD(Arg0), 15), Local0) // Device no
    Add(Local0, ShiftLeft(TBTF(Arg0), 12), Local0) // Function no

    Return(Local0)
  } // End of Method(MMRP)

  //
  // Name: MMRP
  // Description: Function to return the Pci base address of TBT Up stream port
  //
  Method(MMTB, 1, Serialized)
  {
    ADBG("MMTB")

    Store(\_SB.PCI0.GPCB(), Local0) // MMIO Base address

    Add(Local0, ShiftLeft(TBTD(Arg0), 15), Local0) // Device no
    Add(Local0, ShiftLeft(TBTF(Arg0), 12), Local0) // Function no

    OperationRegion (MMMM, SystemMemory, Local0, 0x1A)
    Field (MMMM, AnyAcc, NoLock, Preserve)
    {
      Offset(0x19),
      SBUS, 8
    }
    Store(SBUS, Local2)
    Store(\_SB.PCI0.GPCB(), Local0)
    Multiply(Local2, 0x100000, Local2)
    Add(Local2, Local0, Local0) // TBT HR US port

    ADBG("TBT-US-ADR")
    ADBG(Local0)

    Return(Local0)
  } // End of Method(MMTB, 1, Serialized)

  //
  // Name: FFTB
  // Description: Function to  Check for FFFF in TBT PCIe
  // Return: 1 if TBT PCIe space has value FFFF, 0 if not
  //
  Method(FFTB, 1, Serialized)
  {
    ADBG("FFTB")

    Add(MMTB(Arg0), 0x548, Local0)
    OperationRegion(PXVD,SystemMemory,Local0,0x08)
    Field(PXVD,DWordAcc, NoLock, Preserve)
    {
      TB2P, 32,
      P2TB, 32
    }

    Store(TB2P, Local1)

    If(LEqual(Local1, 0xFFFFFFFF))
    {
      ADBG("FFTb 1")
      Return (1)
    }
    Else
    {
      ADBG("FFTb 0")
      Return (0)
    }
  } // End of Method(FFTB)

