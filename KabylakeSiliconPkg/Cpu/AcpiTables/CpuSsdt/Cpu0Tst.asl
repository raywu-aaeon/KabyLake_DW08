/** @file
  Intel Processor Power Management ACPI Code.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2017 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/

#include "CpuPowerMgmt.h"

DefinitionBlock(
  "CPU0TST.aml",
  "SSDT",
  0x02,
  "PmRef",
  "Cpu0Tst",
  0x3000
  )
{
  External(\_SB.OSCP, IntObj)
  External(\PC00, IntObj)
  External(\TCNT, FieldUnitObj)
  External(\_PR.CTPC, IntObj)
  External(\_PR.CFGD, FieldUnitObj)
  External(\_PR.PR00, DeviceObj)
  External(\_PR.PR00._PSS, PkgObj)
  External(\_PR.PR00.LPSS, PkgObj)
  External(\_PR.PR00.TPSS, PkgObj)

  Scope(\_PR.PR00)
  {
    Method(_TPC,0)
    {
      Return(\_PR.CTPC) // Return max T-state available
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      //
      // IF OSPM is capable of direct access to MSR
      //    Report MSR interface
      // ELSE
      //    Report I/O interface
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //
      If(And(\PC00, 0x0004)) {

        Return(Package() {
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)}
        })
      }

      Return(Package() {
        ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)},
        ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)}
      })
    }

    //
    // _TSS package for fine-grained T-State control.
    // "Power" fields are replaced with real values by the first
    // call of _TSS method.
    //
    Name(TSMF, Package() {
        Package(){100, 1000, 0, 0x00, 0},
        Package(){ 94,  940, 0, 0x1F, 0},
        Package(){ 88,  880, 0, 0x1E, 0},
        Package(){ 82,  820, 0, 0x1D, 0},
        Package(){ 75,  760, 0, 0x1C, 0},
        Package(){ 69,  700, 0, 0x1B, 0},
        Package(){ 63,  640, 0, 0x1A, 0},
        Package(){ 57,  580, 0, 0x19, 0},
        Package(){ 50,  520, 0, 0x18, 0},
        Package(){ 44,  460, 0, 0x17, 0},
        Package(){ 38,  400, 0, 0x16, 0},
        Package(){ 32,  340, 0, 0x15, 0},
        Package(){ 25,  280, 0, 0x14, 0},
        Package(){ 19,  220, 0, 0x13, 0},
        Package(){ 13,  160, 0, 0x12, 0},
        Package(){  7,  100, 0, 0x11, 0}
    })

    //
    // _TSS package for T-State control (Coarse grained)
    // "Power" fields are replaced with real values by the first
    // call of _TSS method.
    //
    Name(TSMC, Package() {
        Package(){100, 1000, 0, 0x00, 0},
        Package(){ 88,  875, 0, 0x1E, 0},
        Package(){ 75,  750, 0, 0x1C, 0},
        Package(){ 63,  625, 0, 0x1A, 0},
        Package(){ 50,  500, 0, 0x18, 0},
        Package(){ 38,  375, 0, 0x16, 0},
        Package(){ 25,  250, 0, 0x14, 0},
        Package(){ 13,  125, 0, 0x12, 0}
    })

    Name(TSSF, 0)  // Flag for TSIF/TSIC/TSMF/TSMC initialization
    Mutex(TSMO, 0) // Mutex object to ensure the _TSS initialization code is only executed once
    Name ( LFMI, 0) //LFM Index from _PSS
    Name ( LFMP, 0) //LFM Power from _PSS

    // _TSS (Throttling Supported States)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Tstate sub-packages as described below
    //
    // Return Value Information
    //   Package {
    //   TState [0] // Package - Throttling state 0
    //   ....
    //   TState [n] // Package - Throttling state n
    //   }
    //
    //   Each Tstate sub-Package contains the elements described below:
    //   Package {
    //     Percent // Integer (DWORD)
    //     Power   // Integer (DWORD)
    //     Latency // Integer (DWORD)
    //     Control // Integer (DWORD)
    //     Status  // Integer (DWORD)
    //   }
    //
    Method(_TSS,,,,PkgObj)
    {
      //
      // Update "Power" fields of TSIC or TSIF or TSMC or TSMF with the LFM
      // power data IF _PSS is available
      //    Power caluclation:
      //               n - Number of T-states available
      //              _TSS(x).power = LFM.Power * (n-x)/n
      //
      If (LAnd(LNot(TSSF),CondRefOf(\_PR.PR00._PSS)))
      {
        //
        // Acquire Mutex to make sure the initialization happens only once.
        //
        Acquire (TSMO, 0xFFFF)
        //
        // Only one thread will be able to acquire the mutex at a time, but the other threads which have acquired the mutex previously, will eventually try to execute the TSS initialization code.
        // So, let's check if TSS has already been initialized once again. If its initalized, skip the initialization.
        //
        If (LAnd(LNot(TSSF),CondRefOf(\_PR.PR00._PSS)))
        {
          //
          // \_SB.OSCP[10] = Platform-Wide OS Capable for no limit 16 P-states
          //
          If(And(\_SB.OSCP, 0x0400)) {
            Store (SizeOf(\_PR.PR00.TPSS), LFMI)
            Decrement(LFMI)    // Index of LFM entry in _PSS
            Store ( DerefOf(Index(DerefOf(Index(\_PR.PR00.TPSS,LFMI)),1)),LFMP)
          } Else {
            Store (SizeOf(\_PR.PR00.LPSS), LFMI)
            Decrement(LFMI)    // Index of LFM entry in _PSS
            Store ( DerefOf(Index(DerefOf(Index(\_PR.PR00.LPSS,LFMI)),1)),LFMP)
          }

          //
          // Copy reference of appropiate TSS package based on Fine grained T-state support
          // We'll update the power in the package directly (via the reference variable Local1)
          //
          // If Fine Grained T-states is enabled
          //      TSMF
          //    ELSE
          //      TSMC
          //
          If(And(CFGD,PPM_TSTATE_FINE_GRAINED))
          {
            Store ( RefOf(TSMF), Local1 )
            Store ( SizeOf(TSMF),Local2 )
          }
          Else
          {
            Store ( RefOf(TSMC), Local1 )
            Store ( SizeOf(TSMC),Local2 )
          }

          Store (0, Local0)
          While(LLess(Local0, Local2))
          {
            Store(Divide(Multiply(LFMP, Subtract(Local2, Local0)), Local2),
                  Local4)    // Power for this entry
            Store(Local4,Index(DerefOf(Index(DerefOf(Local1),Local0)),1))
            Increment(Local0)
          }

          Store(Ones, TSSF)    // Set flag to indicate TSS table initialization is complete
        }

        Release (TSMO)

      }
      //
      //  If Fine Grained T-states is enabled
      //    Report TSMF
      //  ELSE
      //    Report TSMC
      //
      If(And(CFGD, PPM_TSTATE_FINE_GRAINED))
      {
        Return(TSMF)
      }
      Else
      {
        Return(TSMC)
      }
    }

    // _TSD (T-State Dependency)
    //
    // This optional object provides T-state control cross logical processor dependency information to OSPM.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of T-state dependency Packages as described below.
    //
    // Return Value Information
    //   Package {
    //     NumEntries    // Integer
    //     Revision      // Integer (BYTE)
    //     Domain        // Integer (DWORD)
    //     CoordType     // Integer (DWORD)
    //     NumProcessors // Integer (DWORD)
    //   }
    //
    Method(_TSD,,,,PkgObj)
    {
      //
      // IF !(direct access to MSR)
      //    Report SW_ANY as the coordination type
      // ELSE
      //   Report SW_ALL as the coordination type
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //
      If (LNot(And(\PC00,4))) {
        Return(Package(){     // SW_ANY
          Package(){
            5,                // # entries.
            0,                // Revision.
            0,                // Domain #.
            0xFD,             // Coord Type- SW_ANY
            TCNT              // # processors.
          }
        })
      }
      Return(Package(){       // SW_ALL
        Package(){
          5,                  // # entries.
          0,                  // Revision.
          0,                  // Domain #.
          0xFC,               // Coord Type- SW_ALL
          1                   // # processors.
        }
      })
    }
  }
} // End of Definition Block

