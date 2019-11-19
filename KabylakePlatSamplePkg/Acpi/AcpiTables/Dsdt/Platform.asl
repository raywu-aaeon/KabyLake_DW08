/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2019 Intel Corporation.

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

#define TRAP_TYPE_DTS     0x02
#define TRAP_TYPE_IGD     0x03
#define TRAP_TYPE_BGD     0x04  // BIOS Guard

// Define the following External variables to prevent a WARNING when
// using ASL.EXE and an ERROR when using IASL.EXE.

External(\PC00, IntObj) // PR00 _PDC Flags
External(\PC01)
External(\PC02)
External(\PC03)
External(\PC04)
External(\PC05)
External(\PC06)
External(\PC07)
External(\PC08)
External(\PC09)
External(\PC10)
External(\PC11)
External(\PC12)
External(\PC13)
External(\PC14)
External(\PC15)
External(\_PR.CFGD)
External(\SGMD)
//
// Battery Support
//
External(\BNUM)

//
// DTS externals
//
External(\_PR.DTSF)
External(\_PR.DTSE)
External(\_PR.DTS1)
External(\_PR.DTS2)
External(\_PR.PDTS)
External(\_PR.PKGA)
External(\_PR.DTS3)
External(\_PR.DTS4)
External(\_PR.TRPD)
External(\_PR.TRPF)
External(\_PR.DSAE)
//
// BIOS Guard externals
//
External(\_PR.BGMA)
External(\_PR.BGMS)
External(\_PR.BGIA)
//
// AdvancedFeaturesBegin
//
//
// DPTF Externals
//
External(\_SB.PCI0.LPCB.H_EC.CHRG,DeviceObj)
//
//
// AdvancedFeaturesEnd
//
//
//  SGX
//
External(\_PR.EPCS)
External(\_PR.EMNA)
External(\_PR.ELNG)
External(\_TZ.FCRN)

External(\_SB.PCI0.GFX0.TCHE)   // Technology enabled indicator
External(\_SB.PCI0.GFX0.STAT)   // State Indicator

External(\_SB.TPM.PTS, MethodObj)
External(\_SB.PCI0.PAUD.PUAM, MethodObj) //PUAM - PowerResource User Absent Mode
External(\_SB.PCI0.XHC.DUAM, MethodObj)  //DUAM - Device User Absent Mode for XHCI controller
External(\_SB.PCI0.I2C4.GEXP.INVC, MethodObj)

External(\_SB.PCI0.RP01.PON, MethodObj)
External(\_SB.PCI0.RP02.PON, MethodObj)
External(\_SB.PCI0.RP03.PON, MethodObj)
External(\_SB.PCI0.RP04.PON, MethodObj)
External(\_SB.PCI0.RP05.PON, MethodObj)
External(\_SB.PCI0.RP06.PON, MethodObj)
External(\_SB.PCI0.RP07.PON, MethodObj)
External(\_SB.PCI0.RP08.PON, MethodObj)
External(\_SB.PCI0.RP09.PON, MethodObj)
External(\_SB.PCI0.RP10.PON, MethodObj)
External(\_SB.PCI0.RP11.PON, MethodObj)
External(\_SB.PCI0.RP12.PON, MethodObj)
External(\_SB.PCI0.RP13.PON, MethodObj)
External(\_SB.PCI0.RP14.PON, MethodObj)
External(\_SB.PCI0.RP15.PON, MethodObj)
External(\_SB.PCI0.RP16.PON, MethodObj)
External(\_SB.PCI0.RP17.PON, MethodObj)
External(\_SB.PCI0.RP18.PON, MethodObj)
External(\_SB.PCI0.RP19.PON, MethodObj)
External(\_SB.PCI0.RP20.PON, MethodObj)
External(\_SB.PCI0.PEG0.PG00._ON, MethodObj)
External(\_SB.PCI0.PEG1.PG01._ON, MethodObj)
External(\_SB.PCI0.PEG2.PG02._ON, MethodObj)

External(\_SB.PCI0.GFX0.IUEH, MethodObj)
// AMI_OVERRIDE_START - For the structure of AMI asl code scope.
External(\MDBG, MethodObj)
// AMI_OVERRIDE_END - For the structure of AMI asl code scope.
// AMI_OVERRIDE_START - For the structure of AMI asl code scope.
#if !defined(ASL_CRB_EC_SUPPORT) || (ASL_CRB_EC_SUPPORT == 0)  
External(\_PR.POWS)
#endif 
// AMI_OVERRIDE_END - For the structure of AMI asl code scope.

#define CONVERTIBLE_BUTTON   6
#define DOCK_INDICATOR       7

Name(ECUP, 1)  // EC State indicator: 1- Normal Mode 0- Low Power Mode
Mutex(EHLD, 0) // EC Hold indicator: 0- No one accessing the EC Power State 1- Someone else is accessing the EC Power State



// AMI_OVERRIDE_START - It is for Thunderbolt, AMI thunderbolt module has taken care it already.
#if ASL_Thunderbolt_SUPPORT
// AMI_OVERRIDE_START - Fixed build errors with the latest ASL compiler
// The following external declarations have to be removed, cause External declaration cannot be used if method is defined in the same file.
//External(TBTD, MethodObj)
//External(TBTF, MethodObj)
//External(MMRP, MethodObj)
//External(MMTB, MethodObj)
//External(TBFF, MethodObj)
//External(FFTB, MethodObj)
//External(SXTB, MethodObj)
// AMI_OVERRIDE_END - Fixed build errors with the latest ASL compier

//
// AdvancedFeaturesBegin
//
include("Tbt.asl")
#endif 
// AMI_OVERRIDE_END - It is for Thunderbolt, AMI thunderbolt module has taken care it already.
// Comms Hub module support
include("CommsHub.asl")
//
// AdvancedFeaturesEnd
//

// Interrupt specific registers
include("Itss.asl")

// Create a Global MUTEX.

Mutex(MUTX,0)

// AMI_OVERRIDE_START - It is for Thunderbolt, AMI thunderbolt module has taken care it already.
#if ASL_Thunderbolt_SUPPORT
// OS Up mutex
Mutex(OSUM, 0)
// _WAK Finished Event
Event(WFEV)
#endif 
// AMI_OVERRIDE_END - It is for Thunderbolt, AMI thunderbolt module has taken care it already.

// Define Port 80 as an ACPI Operating Region to use for debugging.  Please
// note that the Intel CRBs have the ability to ouput a Word to
// Port 80h for debugging purposes, so the model implemented here may not be
// able to be used on OEM Designs.

OperationRegion(PRT0,SystemIO,0x80,2)
Field(PRT0,WordAcc,Lock,Preserve)
{
  P80B, 16
}

// Port 80h Update:
//    Update 2 bytes of Port 80h.
//
//  Arguments:
//    Arg0: 0 = Write Port 80h
//          1 = Write Port 81h
//    Arg1: 8-bit Value to write
//
//  Return Value:
//    None

Name(P80T, 0) // temp buffer for P80

Method(D8XH,2,Serialized)
{
  If(LEqual(Arg0,0))    // Write Port 80h
  {
    Store(Or(And(P80T,0xFF00),And(Arg1,0xFF)),P80T)
  }
  If(LEqual(Arg0,1))    // Write Port 81h
  {
    Store(Or(And(P80T,0x00FF),ShiftLeft(Arg1,8)),P80T)
  }
  Store(P80T,P80B)
}

// Debug Port 80h Update:
//    If Acpidebug is enabled only then call D8XH to update 8 bits of the 32-bit Port 80h.
//
//  Arguments:
//    Arg0: 0 = Write Port 80h, Bits 7:0 Only.
//            1 = Write Port 80h, Bits 15:8 Only.
//            2 = Write Port 80h, Bits 23:16 Only.
//            3 = Write Port 80h, Bits 31:24 Only.
//    Arg1: 8-bit Value to write
//
//  Return Value:
//    None
Method(P8XH,2,Serialized)
{
  // If ACPI debug is enabled, then display post codes on Port 80h
  If(CondRefOf(MDBG)) {// Check if ACPI Debug SSDT is loaded
     D8XH(Arg0,Arg1)
   }
}


Method(ADBG,1,Serialized)
{
// AMI_OVERRIDE_START - For the structure of AMI asl code scope.
  If(CondRefOf(MDBG))
  {
     Return(MDBG(Arg0))
  }
// AMI_OVERRIDE_END - For the structure of AMI asl code scope.
  Return(0)
}

//
// Define SW SMI port as an ACPI Operating Region to use for generate SW SMI.
//
OperationRegion(SPRT,SystemIO, 0xB2,2)
Field (SPRT, ByteAcc, Lock, Preserve) {
  SSMP, 8
}
// AMI_OVERRIDE_START - For the structure of AMI asl code scope.
// Return package for _WAK method 2 DWORDS
    Name(WAKP, Package(){Zero, Zero})
// AMI_OVERRIDE_END - For the structure of AMI asl code scope.

// The _PIC Control Method is optional for ACPI design.  It allows the
// OS to inform the ASL code which interrupt controller is being used,
// the 8259 or APIC.  The reference code in this document will address
// PCI IRQ Routing and resource allocation for both cases.
//
// The values passed into _PIC are:
//   0 = 8259
//   1 = IOAPIC

Method(\_PIC,1)
{
  Store(Arg0,GPIC)
  Store(Arg0,PICM)
}

// Prepare to Sleep.  The hook is called when the OS is about to
// enter a sleep state.  The argument passed is the numeric value of
// the Sx state.

// AMI_OVERRIDE_START - For the structure of AMI asl code scope.
Method(RPTS,1)
// AMI_OVERRIDE_END - For the structure of AMI asl code scope.
{
  Store(0,P80T)   // Zero out the entire Port 80h Word.
  D8XH(0,Arg0)    // Output Sleep State to Port 80h, Byte 0.

  ADBG(Concatenate("_PTS=",ToHexString(Arg0)))

// AMI_OVERRIDE_START - It is for Thunderbolt, AMI thunderbolt module has taken care it already.
#if ASL_Thunderbolt_SUPPORT
  //
  // AdvancedFeaturesBegin
  //
  If (LEqual(\TBTS, 0x01)) {
    Store(1, TRD3) // Invalidating TBT Hotplug handler while Going to Sx
    Reset(WFEV)
    If (LEqual(\RTBT, 1)) {
      Store(0, TOFF)
    }
    If (LEqual(\TSXW, 1)) {
      ADBG(Concatenate("TSXW=",\TSXW))
      ADBG("TBT Wake switch")
      ADBG(Concatenate("Before=",\_SB.CGRD(2, 0, 16, 0)))
      \_SB.CGWR(2, 0, 16, 0) // Switch TBT_WAKE_N routing to PCH WAKEB. Write IO expander P20.
      ADBG("TBT switch done")
      ADBG(Concatenate("After=",\_SB.CGRD(2, 0, 16, 0)))
    } else {
      ADBG(Concatenate("TSXW=",\TSXW))
      ADBG("No Wake switch")
    }
  }
  //
  // AdvancedFeaturesEnd
  //

#endif
// AMI_OVERRIDE_END - It is for Thunderbolt, AMI thunderbolt module has taken care it already.
  
  // If code is executed, Wake from RI# via Serial Modem will be
  // enabled.  If code is not executed, COM Port Debugging throughout
  // all Sx states will be enabled.

  If(LEqual(Arg0,3))
  {
    //
    // Disable update DTS temperature and threshold value in every SMI
    //
    If(CondRefOf(\_PR.DTSE)){
      If(LAnd(\_PR.DTSE, LGreater(TCNT, 1)))
      {
        TRAP(TRAP_TYPE_DTS,30)
      }
    }
  }

  //
  // AdvancedFeaturesBegin
  //
  //
  //  Save EC 10Sec mode to NVS data PB1E Bit7 at S3/S4 entry
  //
  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))
  {
    If(LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.PB10), \ECON)){
      //
      // Check EC 10sec PB mode is enabled.(H_EC.PB10)
      // PB10 is cleared at every boot so we need to keep the state before entering S3/S4.
      //
      If(\_SB.PCI0.LPCB.H_EC.ECAV)
      {
        If(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PB10)))
        {
          //
          // EC 10sec PB mode is enabled. Save the state in PB1E bit7
          //
          Or(PB1E, 0x80, PB1E)
        }
        Else
        {
          //
          // EC 10sec PB mode is not enabled. Clear PB1E bit7.
          //
          And(PB1E, 0x7F, PB1E)
        }
      } // If(\_SB.PCI0.LPCB.H_EC.ECAV)
    }
  }

// AMI_OVERRIDE_START - For the structure of AMI asl code scope.
#if 0
  If(LEqual(DBGS,0))
  {
    Store(0,RT10)     // Set MAXIM Transceiver = FORCEOFF.
    Store(0x20,PME1)  // Set SMSC GPI15 = Wake Event.
    Store(1,PME0)     // Set SMSC PME Enable.
    Store(0x20,PMS1)  // Clear GPI15 Status.
    Store(1,PMS0)     // Clear PME Status.
  }

#endif
// AMI_OVERRIDE_END - For the structure of AMI asl code scope.

  // Disable IVCAM before going into Sx state.
  If(LEqual(IVCM,1))
  {
    \_SB.SGOV(GPIO_SKL_LP_GPP_E0, 0)                  /// DFU down IVCAM
    \_SB.SGOV(GPIO_SKL_LP_GPP_B2, 0)                  /// Power down IVCAM.
  }
  //
  // AdvancedFeaturesEnd
  //

  // Generate a SW SMI trap to save some NVRAM data back to CMOS.

  //  Don't enable IGD OpRegion support yet.
  //  TRAP(1, 81)
  //
  // Call TPM.PTS
  //
  If(CondRefOf(\_SB.TPM.PTS))
  {
    //
    // Call TPM PTS method
    //
    \_SB.TPM.PTS (Arg0)
  }

  //
  // AdvancedFeaturesBegin
  //
  // StandBy LED Drive. Program High for sleep state s3, s4 and s5 only fro desktop.
  If(LOr(LOr(LEqual(Arg0,3), LEqual(Arg0,4)), LEqual(Arg0,5))){
    If(LEqual(PFLV,FlavorDesktop)){
      \_SB.SGOV(GPIO_SKL_LP_GPP_B3,1)
    }
  }
  //
  // AdvancedFeaturesEnd
  //
}

// Platform Hook for _BCL method for Igfx.
// Platform can also add logic here  to create run time creation of package
// example: return package without 0-10 values when in tablet mode,else return all values 0-100.
Method(PBCL ,0)
{
  //
  // List of supported brightness levels in the following sequence.
  // Level when machine has full power.
  // Level when machine is on batteries.
  // Other supported levels.
  //
  Return(Package(){80, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100})
}

// Wake.  This hook is called when the OS is about to wake from a
// sleep state.  The argument passed is the numeric value of the
// sleep state the system is waking from.

// AMI_OVERRIDE_START - For the structure of AMI asl code scope.
Method(RWAK,1,Serialized)
// AMI_OVERRIDE_END - For the structure of AMI asl code scope.
{
  D8XH(1,0xAB)    // Beginning of _WAK.

  ADBG("_WAK")
  Store(0, \_TZ.FCRN)
  //
  // AdvancedFeaturesBegin
  //
// AMI_OVERRIDE_START >>> It is for Thunderbolt, AMI thunderbolt module has taken care it already.
#if ASL_Thunderbolt_SUPPORT
  Store(1, TBPE)
  If(LEqual(TBTS, 0x01)) {
    Store(0, TRD3) // Validating TBT Hotplug handler while Waking from Sx
  }
#endif
// AMI_OVERRIDE_END <<< It is for Thunderbolt, AMI thunderbolt module has taken care it already.
  //
  // Invalidate I2c GPIO expander cache
  //
  \_SB.PCI0.GEXP.INVC()
  //
  // AdvancedFeaturesEnd
  //

  //
  // Only set 8254 CG if Low Power S0 Idle Capability is enabled
  //
  If (LEqual(S0ID, One)) {
    //
    //  Set ITSSPRC.8254CGE: Offset 3300h ITSSPRC[2]
    //
// AMI_OVERRIDE_START - For ASL_SPS_ACPI_SUPPORT
#ifndef ASL_SPS_ACPI_SUPPORT
    Store(0x01, \_SB.SCGE)
#endif
// AMI_OVERRIDE_END - For ASL_SPS_ACPI_SUPPORT
  }

  If(NEXP)
  {
    // Reinitialize the Native PCI Express after resume
    If(And(OSCC,0x01))
    {
      \_SB.PCI0.NHPG()
    }
    If(And(OSCC,0x04))  // PME control granted?
    {
      \_SB.PCI0.NPME()
    }
  }

  //
  // AdvancedFeaturesBegin
  //
  If(LEqual(Arg0,3))
  {
    // Turn off the CPU Fan for all OSes if Active Cooling
    // is disabled.

    If(LEqual(0,ACTT))
    {
      If(LEqual(\ECON,1))
      {
        \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.CFAN))
      }
    }
  }
  //
  // AdvancedFeaturesEnd
  //

  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
  {

    //
    // AdvancedFeaturesBegin
    //
    //
    // Restore EC 10sec PB override mode
    //
    If(LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.PB10), \ECON)){
      If(And(PB1E, 0x80))
      {
        \_SB.PCI0.LPCB.H_EC.ECWT(1, RefOf(\_SB.PCI0.LPCB.H_EC.PB10))
      }
    }
    //
    // AdvancedFeaturesEnd
    //
    // Check to send Convertible/Dock state changes upon resume from Sx.
    If(And(GBSX,0x40))
    {
      \_SB.PCI0.GFX0.IUEH(6)

      //
      //  Do the same thing for Virtul Button device.
      //  Toggle Bit3 of PB1E(Slate/Notebook status)
      //
// AMI_OVERRIDE_START - For EC support
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1)
      Xor(PB1E, 0x08, PB1E)

      //
      // AdvancedFeaturesBegin
      //
      //
      // Update button status.
      //
      If(And(PB1E, 0x08)){
        If(CondRefOf(\_SB.PCI0.LPCB.H_EC.UPBT))
        {
          \_SB.PCI0.LPCB.H_EC.UPBT(CONVERTIBLE_BUTTON, One)
        }
      }
      Else
      {
        If(CondRefOf(\_SB.PCI0.LPCB.H_EC.UPBT))
        {
          \_SB.PCI0.LPCB.H_EC.UPBT(CONVERTIBLE_BUTTON, Zero)
        }
      }
      //
      // AdvancedFeaturesEnd
      //
#endif //#ifdef ASL_CRB_EC_SUPPORT
// AMI_OVERRIDE_END - For EC support
    }

    If(And(GBSX,0x80))
    {
      \_SB.PCI0.GFX0.IUEH(7)

      //
      //  Do the same thing for Virtul Button device.
      //  Toggle Bit4 of PB1E (Dock/Undock status)
      //
// AMI_OVERRIDE_START - For EC support
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1)
      Xor(PB1E, 0x10, PB1E)

      //
      // AdvancedFeaturesBegin
      //
      //
      // Update button status.
      //
      If(And(PB1E, 0x10))
      {
        If(CondRefOf(\_SB.PCI0.LPCB.H_EC.UPBT))
        {
          \_SB.PCI0.LPCB.H_EC.UPBT(DOCK_INDICATOR, One)
        }
      }
      Else
      {
        If(CondRefOf(\_SB.PCI0.LPCB.H_EC.UPBT))
        {
          \_SB.PCI0.LPCB.H_EC.UPBT(DOCK_INDICATOR, Zero)
        }
      }
      //
      // AdvancedFeaturesEnd
      //
#endif //#ifdef ASL_CRB_EC_SUPPORT
// AMI_OVERRIDE_END - For EC support
    }

// AMI_OVERRIDE_START - For EC support
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1)
    //
    // AdvancedFeaturesBegin
    //
    //
    // Prevent reinitializing Virtual GPIO Button status
    //
    If(CondRefOf(\_SB.PCI0.LPCB.H_EC.ONTM))
    {
      Store(1, \_SB.PCI0.LPCB.H_EC.ONTM)
    }
    //
    // AdvancedFeaturesEnd
    //
#endif //#ifdef ASL_CRB_EC_SUPPORT
// AMI_OVERRIDE_END - For EC support

    If(CondRefOf(\_PR.DTSE)){
      If(LAnd(\_PR.DTSE, LGreater(TCNT, 1)))
      {
        TRAP(TRAP_TYPE_DTS, 20)
      }
    }

// AMI_OVERRIDE_START - For EC support
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1)
    //
    // AdvancedFeaturesBegin
    //
    If(LEqual(\ECON,1))
    {
      // Update Lid state after S3 or S4 resume
      If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
      {
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE)), LIDS)

        If(IGDS)
        {
            If (LEqual(LIDS, 0))
            {
              Store(0x80000000,\_SB.PCI0.GFX0.CLID)
            }
            If (LEqual(LIDS, 1))
            {
              Store(0x80000003,\_SB.PCI0.GFX0.CLID)
            }
        }
        Notify(\_SB.PCI0.LPCB.H_EC.LID0,0x80)
      }

// @todo: Currently KBL\KBLR doesn't support real dock. Waiting for supporting real dock
//        and DOCK offset data.
//      // Detect the change of Dock state
//      If(LNotEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), \DSTS))
//      {
//        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), \DSTS)
//        If(LEqual(\DSTS, 1))  // Docked.
//        {
//          If(LEqual(Arg0,3))
//          {
//            Sleep(1000) // Delay 1 second for hot docking stability
//            Store(DKSM, \SSMP)
//            Sleep(1000) // Delay 1 second for hot docking stability
//          }
//          Notify(\_SB.PCI0.DOCK, 0)
//        }
//        Else      // Undocked.
//        {
//          Notify(\_SB.PCI0.DOCK, 1)
//        }
//      }

      // Detect the change of Power State.
      If(LEqual(BNUM,0))
      {
        If(LNotEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR)),PWRS))
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR)),PWRS)
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR)),\_PR.POWS)
          // Perform needed ACPI Notifications.
          PNOT()
        }
      }
      Else
      {
        If(LNotEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.RPWR)),PWRS))
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.RPWR)),PWRS)
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.RPWR)),\_PR.POWS)
          // Perform needed ACPI Notifications.
          PNOT()
        }
      }
    }
#endif
// AMI_OVERRIDE_END - For EC support

// AMI_OVERRIDE_START - It is for Thunderbolt, AMI thunderbolt module has taken care it already
#if ASL_Thunderbolt_SUPPORT
    If(LEqual(TBTS, 0x01)) {
      Acquire(OSUM, 0xFFFF)
//AMI_OVERRIDE_START - EIP315519 - Unable to detect TBT devices after system reset (Boot with TBT attached)
      \_GPE.TINI(TBSE, Arg0)
//AMI_OVERRIDE_END - EIP315519 - Unable to detect TBT devices after system reset (Boot with TBT attached)
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
      If(LEqual(TBMP, 0x01)) {
      \_GPE.TINI(TBS1)
      }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
      Release(OSUM)
    }
#endif
// AMI_OVERRIDE_END - It is for Thunderbolt, AMI thunderbolt module has taken care it already
    //
    // AdvancedFeaturesEnd
    //

    // For PCI Express Express Cards, it is possible a device was
    // either inserted or removed during an Sx State.  The problem
    // is that no wake event will occur for a given warm insertion
    // or removal, so the OS will not become aware of any change.
    // To get around this, re-enumerate all Express Card slots.
    //
    // If the Root Port is enabled, it may be assumed to be hot-pluggable.

    If(LNotEqual(\_SB.PCI0.RP01.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP01,0)
    }

    If(LNotEqual(\_SB.PCI0.RP02.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP02,0)
    }

    If(LNotEqual(\_SB.PCI0.RP03.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP03,0)
    }

    If(LNotEqual(\_SB.PCI0.RP04.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP04,0)
    }

    If(LNotEqual(\_SB.PCI0.RP05.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP05,0)
    }

    If(LNotEqual(\_SB.PCI0.RP06.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP06,0)
    }

    If(LNotEqual(\_SB.PCI0.RP07.VDID,0xFFFFFFFF))
    {
      If(LEqual(\DSTS,0)) // UnDocked.
      {
        Notify (\_SB.PCI0.RP07,0)
      }
    }

    If(LNotEqual(\_SB.PCI0.RP08.VDID,0xFFFFFFFF))
    {
      If(LEqual(\DSTS,0)) // UnDocked.
      {
        Notify (\_SB.PCI0.RP08,0)
      }
    }

    If(LNotEqual(\_SB.PCI0.RP09.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP09,0)
    }

    If(LNotEqual(\_SB.PCI0.RP10.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP10,0)
    }

    If(LNotEqual(\_SB.PCI0.RP11.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP11,0)
    }

    If(LNotEqual(\_SB.PCI0.RP12.VDID,0xFFFFFFFF))
    {
        Notify (\_SB.PCI0.RP12,0)
    }

    If(LNotEqual(\_SB.PCI0.RP13.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP13,0)
    }

    If(LNotEqual(\_SB.PCI0.RP14.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP14,0)
    }

    If(LNotEqual(\_SB.PCI0.RP15.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP15,0)
    }

    If(LNotEqual(\_SB.PCI0.RP16.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP16,0)
    }

    If(LNotEqual(\_SB.PCI0.RP17.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP17,0)
    }

    If(LNotEqual(\_SB.PCI0.RP18.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP18,0)
    }

    If(LNotEqual(\_SB.PCI0.RP19.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP19,0)
    }

    If(LNotEqual(\_SB.PCI0.RP20.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP20,0)
    }
// AMI_OVERRIDE_START - EIP279338 The resource for PEG Thunderbolt host is incorrect.
    If(LNotEqual(\_SB.PCI0.RP21.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP21,0)
    }

    If(LNotEqual(\_SB.PCI0.RP22.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP22,0)
    }    

    If(LNotEqual(\_SB.PCI0.RP23.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP23,0)
    }

    If(LNotEqual(\_SB.PCI0.RP24.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PCI0.RP24,0)
    }
// AMI_OVERRIDE_END - EIP279338 The resource for PEG Thunderbolt host is incorrect.
    //
    // AdvancedFeaturesBegin
    //
    If(CondRefOf(\_SB.VMEN))
    {
      Store (0xFF, \_SB.VMEN) // Reset Voltage Margining enabled flag to 0xFF - undefined value
    }
    //
    // AdvancedFeaturesEnd
    //
  }

  //
  // AdvancedFeaturesBegin
  //
  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
  {
    //
    // If Using Control Method Power Button, notify PWRD device with 0x2
    //
    If(LEqual(\_SB.PWRB.PBST, 0x1)) {
      If(PBSS) { //Power Button woke the system
        Notify(\_SB.PWRB, 0x02) // Send release notification to Power Button device 0x02
        Store(1, PBSS)
      }
    }

  }

// AMI_OVERRIDE_START - It is for Thunderbolt, AMI thunderbolt module has taken care it already.
#if ASL_Thunderbolt_SUPPORT
  If(LEqual(TBTS, 0x01)) {
    Signal(WFEV)
  }
#endif
// AMI_OVERRIDE_END - It is for Thunderbolt, AMI thunderbolt module has taken care it already.
  //
  // AdvancedFeaturesEnd
  //

  Return(Package(){0,0})
}

// Get Buffer:
//    This method will take a buffer passed into the method and
//    create then return a smaller buffer based on the pointer
//    and size parameters passed in.
//
//  Arguments:
//    Arg0: Pointer to start of new Buffer in passed in Buffer.
//    Arg1: Size of Buffer to create.
//    Arg2: Original Buffer
//
//  Return Value:
//    Newly created buffer.

Method(GETB,3,Serialized)
{
  Multiply(Arg0,8,Local0)     // Convert Index.
  Multiply(Arg1,8,Local1)     // Convert Size.
  CreateField(Arg2,Local0,Local1,TBF3)  // Create Buffer.

  Return(TBF3)        // Return Buffer.
}

// Power Notification:
//    Perform all needed OS notifications during a
//    Power Switch.
//
//  Arguments:
//    None
//
//  Return Value:
//    None

Method(PNOT,0,Serialized)
{
  //
  // If MP enabled and driver support is present, notify all
  // processors.
  //
  If(LGreater(TCNT, 1))
  {
    If(And(\PC00,0x0008)){
      Notify(\_PR.PR00,0x80)    // Eval PR00 _PPC.
    }
    If(And(\PC01,0x0008)){
      Notify(\_PR.PR01,0x80)    // Eval PR01 _PPC.
    }
    If(And(\PC02,0x0008)){
      Notify(\_PR.PR02,0x80)    // Eval PR02 _PPC.
    }
    If(And(\PC03,0x0008)){
      Notify(\_PR.PR03,0x80)    // Eval PR03 _PPC.
    }
    If(And(\PC04,0x0008)){
      Notify(\_PR.PR04,0x80)    // Eval PR04 _PPC.
    }
    If(And(\PC05,0x0008)){
      Notify(\_PR.PR05,0x80)    // Eval PR05 _PPC.
    }
    If(And(\PC06,0x0008)){
      Notify(\_PR.PR06,0x80)    // Eval PR06 _PPC.
    }
    If(And(\PC07,0x0008)){
      Notify(\_PR.PR07,0x80)    // Eval PR07 _PPC.
    }
    If(And(\PC08,0x0008)){
      Notify(\_PR.PR08,0x80)    // Eval PR08 _PPC.
    }
    If(And(\PC09,0x0008)){
      Notify(\_PR.PR09,0x80)    // Eval PR09 _PPC.
    }
    If(And(\PC10,0x0008)){
      Notify(\_PR.PR10,0x80)    // Eval PR10 _PPC.
    }
    If(And(\PC11,0x0008)){
      Notify(\_PR.PR11,0x80)    // Eval PR11 _PPC.
    }
    If(And(\PC12,0x0008)){
      Notify(\_PR.PR12,0x80)    // Eval PR12 _PPC.
    }
    If(And(\PC13,0x0008)){
      Notify(\_PR.PR13,0x80)    // Eval PR13 _PPC.
    }
    If(And(\PC14,0x0008)){
      Notify(\_PR.PR14,0x80)    // Eval PR14 _PPC.
    }
    If(And(\PC15,0x0008)){
      Notify(\_PR.PR15,0x80)    // Eval PR15 _PPC.
    }
  }Else{
    Notify(\_PR.PR00,0x80)      // Eval _PPC.
  }

  If(LGreater(TCNT, 1)){
    If(LAnd(And(\PC00,0x0008),And(\PC00,0x0010))){
      Notify(\_PR.PR00,0x81)  // Eval PR00 _CST.
    }
    If(LAnd(And(\PC01,0x0008),And(\PC01,0x0010))){
      Notify(\_PR.PR01,0x81)  // Eval PR01 _CST.
    }
    If(LAnd(And(\PC02,0x0008),And(\PC02,0x0010))){
      Notify(\_PR.PR02,0x81)  // Eval PR02 _CST.
    }
    If(LAnd(And(\PC03,0x0008),And(\PC03,0x0010))){
      Notify(\_PR.PR03,0x81)  // Eval PR03 _CST.
    }
    If(LAnd(And(\PC04,0x0008),And(\PC04,0x0010))){
      Notify(\_PR.PR04,0x81)  // Eval PR04 _CST.
    }
    If(LAnd(And(\PC05,0x0008),And(\PC05,0x0010))){
      Notify(\_PR.PR05,0x81)  // Eval PR05 _CST.
    }
    If(LAnd(And(\PC06,0x0008),And(\PC06,0x0010))){
      Notify(\_PR.PR06,0x81)  // Eval PR06 _CST.
    }
    If(LAnd(And(\PC07,0x0008),And(\PC07,0x0010))){
      Notify(\_PR.PR07,0x81)  // Eval PR07 _CST.
    }
    If(LAnd(And(\PC08,0x0008),And(\PC08,0x0010))){
      Notify(\_PR.PR08,0x81)  // Eval PR08 _CST.
    }
    If(LAnd(And(\PC09,0x0008),And(\PC09,0x0010))){
      Notify(\_PR.PR09,0x81)  // Eval PR09 _CST.
    }
    If(LAnd(And(\PC10,0x0008),And(\PC10,0x0010))){
      Notify(\_PR.PR10,0x81)  // Eval PR10 _CST.
    }
    If(LAnd(And(\PC11,0x0008),And(\PC11,0x0010))){
      Notify(\_PR.PR11,0x81)  // Eval PR11 _CST.
    }
    If(LAnd(And(\PC12,0x0008),And(\PC12,0x0010))){
      Notify(\_PR.PR12,0x81)  // Eval PR12 _CST.
    }
    If(LAnd(And(\PC13,0x0008),And(\PC13,0x0010))){
      Notify(\_PR.PR13,0x81)  // Eval PR13 _CST.
    }
    If(LAnd(And(\PC14,0x0008),And(\PC14,0x0010))){
      Notify(\_PR.PR14,0x81)  // Eval PR14 _CST.
    }
    If(LAnd(And(\PC15,0x0008),And(\PC15,0x0010))){
      Notify(\_PR.PR15,0x81)  // Eval PR15 _CST.
    }
  } Else {
    Notify(\_PR.PR00,0x81)      // Eval _CST.
  }

  //
  // AdvancedFeaturesBegin
  //
  // Update the Battery 1 and 2 Stored Capacity and
  // Stored Status.  Battery 0 information is always accurrate.

// AMI_OVERRIDE_START - The ASL file which contains the code which needs to be called in PNOT().
//  If(LEqual(\ECON,1))
//  { 

    // Perform update to all Batteries in the System.
//    Notify(\_SB.PCI0.LPCB.H_EC.BAT0,0x81)       // Eval BAT0 _BST.
//    Notify(\_SB.PCI0.LPCB.H_EC.BAT1,0x81)       // Eval BAT1 _BST.
//    Notify(\_SB.PCI0.LPCB.H_EC.BAT2,0x81)       // Eval BAT2 _BST.
//  }
  Include("NotifyInPNOT.asl")
// AMI_OVERRIDE_END - The ASL file which contains the code which needs to be called in PNOT().

  If (LEqual(DPTF,1)){
    Notify(\_SB.IETM, 0x86) // Notification sent to DPTF driver (Policy) for PDRT reevaluation after AC/DC transtion has occurred.
    If (LEqual(CHGE,1)){
      Notify(\_SB.PCI0.LPCB.H_EC.CHRG, 0x80) // PPPC/PPDL reevaluation after AC/DC transtion has occurred.
    }
  }
  //
  // AdvancedFeaturesEnd
  //

} // end of Method(PNOT)

//
// Memory window to the CTDP registers starting at MCHBAR+5000h.
//
  OperationRegion (MBAR, SystemMemory, Add(\_SB.PCI0.GMHB(),0x5000), 0x1000)
  Field (MBAR, ByteAcc, NoLock, Preserve)
  {
    Offset (0x938), // PACKAGE_POWER_SKU_UNIT (MCHBAR+0x5938)
    PWRU,  4,       // Power Units [3:0] unit value is calculated by 1 W / Power(2,PWR_UNIT). The default value of 0011b corresponds to 1/8 W.
    Offset (0x9A0), // TURBO_POWER_LIMIT1 (MCHBAR+0x59A0)
    PPL1, 15,       // PKG_PWR_LIM_1 [14:0]
    PL1E,1,         // PKG_PWR_LIM1_EN [15]
    CLP1,1,         // Package Clamping Limitation 1
  }
Name(CLMP, 0) // save the clamp bit
Name(PLEN,0) // save the power limit enable bit
Name(PLSV,0x8000) // save value of PL1 upon entering CS
Name(CSEM, 0) //semaphore to avoid multiple calls to SPL1.  SPL1/RPL1 must always be called in pairs, like push/pop off a stack
//
// SPL1 (Set PL1 to 4.5 watts with clamp bit set)
//   Per Legacy Thermal management CS requirements, we would like to set the PL1 limit when entering CS to 4.5W with clamp bit set via MMIO.
//   This can be done in the ACPI object which gets called by graphics driver during CS Entry.
//   Likewise, during CS exit, the BIOS must reset the PL1 value to the previous value prior to CS entry and reset the clamp bit.
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(SPL1,0,Serialized)
{
    Name(PPUU,0) // units
    If (LEqual(CSEM, 1))
    {
      Return() // we have already been called, must have CS exit before calling again
    }
    Store(1, CSEM) // record first call

    Store (PPL1, PLSV) // save PL1 value upon entering CS
    Store (PL1E, PLEN) // save PL1 Enable bit upon entering CS
    Store (CLP1, CLMP) // save PL1 Clamp bit upon entering CS

    If (LEqual(PWRU,0)) {  // use PACKAGE_POWER_SKU_UNIT - Power Units[3:0]
      Store(1,PPUU)
    } Else {
      ShiftLeft(Decrement(PWRU),2,PPUU) // get units
    }

    Multiply(PLVL,PPUU,Local0)  // convert SETUP value to power units in milli-watts
    Divide(Local0,1000,,Local1) // convert SETUP value to power units in watts
    Store(Local1, PPL1)   // copy value to PL1
    Store(1, PL1E)     // set Enable bit
    Store(1, CLP1)     // set Clamp bit
}
//
// RPL1 (Restore the PL1 register to the values prior to CS entry)
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(RPL1,0,Serialized)
{
    Store (PLSV, PPL1)  // restore value of PL1 upon exiting CS
    Store(PLEN, PL1E)   // restore the PL1 enable bit
    Store(CLMP, CLP1)   // restore the PL1 Clamp bit
    Store(0, CSEM)      // restore semaphore
}

Name(UAMS, 0) // User Absent Mode state, Zero - User Present; non-Zero - User not present
Name(GLCK, 0) // a spin lock to avoid multi execution of GUAM
// GUAM - Global User Absent Mode
//    Run when a change to User Absent mode is made,  e.g. screen/display on/off events.
//    Any device that needs notifications of these events includes its own UAMN Control Method.
//
//    Arguments:
//      Power State:
//        00h = On
//        01h = Standby
//        other value = do nothing & return
//
//    Return Value:
//      None
//
Method(GUAM,1,Serialized)
{
  Switch(ToInteger(Arg0))
  {
    Case(0) // exit CS
    {
      If(LEqual(GLCK, 1)){
        store(0, GLCK)

        P8XH(0, 0xE1)
        P8XH(1, 0xAB)
        ADBG("Exit Resiliency")

        // @Todo: Exit EC Low Power Mode here

        //
        // AdvancedFeaturesBegin
        //
        \_SB.DION() // Voltage margining settings on CS Exit

        If(LLess(OSYS, 2015)) {
          \_SB.PCI0.LPCB.H_EC.ECNT(0) // 0x00 - Power State On (EC CS Exit)
        }
        //
        // AdvancedFeaturesEnd
        //

        If(PSCP){
          // if P-state Capping s enabled
          If (CondRefOf(\_PR.PR00._PPC))
          {
            Store(Zero, \_PR.CPPC)
            PNOT()
          }
        }
        If(PLCS){
          RPL1() // restore PL1 to pre-CS value upon exiting CS
        }
      } // end GLCK=1
    } // end case(0)

    Case(1) // enter CS
    {
      If(LEqual(GLCK, 0)){
        store(1, GLCK)

        P8XH(0, 0xE0)
        P8XH(1, 00)
        ADBG("Enter Resiliency")

        //@Todo: Enter EC Low Power Mode here

        //
        // AdvancedFeaturesBegin
        //
        \_SB.DIOF() // Voltage margining settings on CS Entry

        If(LLess(OSYS, 2015)) {
          \_SB.PCI0.LPCB.H_EC.ECNT(1) // 0x01 - Power State Standby (EC CS Entry)
        }
        //
        // AdvancedFeaturesEnd
        //

        If(PSCP){
          // if P-state Capping is enabled
          If (LAnd(CondRefOf(\_PR.PR00._PSS), CondRefOf(\_PR.PR00._PPC)))
          {
            If(And(\PC00,0x0400))
            {
              Subtract(SizeOf(\_PR.PR00.TPSS), One, \_PR.CPPC)
            } Else {
              Subtract(SizeOf(\_PR.PR00.LPSS), One, \_PR.CPPC)
            }
            PNOT()
          }
        }
        If(PLCS){
          SPL1() // set PL1 to low value upon CS entry
        }
      } // end GLCK=0
    } // end case(1)
    Default{
      Return()  // do nothing
    }
  } // end switch(arg0)

  Store(LAnd(Arg0, LNot(PWRS)), UAMS)  // UAMS: User Absent Mode state, Zero - User Present; non-Zero - User not present
  P_CS()                               // Powergating during CS

} // end method GUAM()

// Power CS Powergated Devices:
//    Method to enable/disable power during CS
Method(P_CS,0,Serialized)
{
    // NOTE: Do not turn ON Touch devices from here. Touch does not have PUAM
    If(CondRefOf(\_SB.PCI0.PAUD.PUAM)){           // Notify Codec(HD-A/ADSP)
        \_SB.PCI0.PAUD.PUAM()
    }
    // Adding back USB powergating (ONLY for Win8) until RTD3 walkup port setup implementation is complete */
    If(LEqual(OSYS,2012)){    // ONLY for Win8 OS
      If(CondRefOf(\_SB.PCI0.XHC.DUAM)){ // Notify USB port- RVP
        \_SB.PCI0.XHC.DUAM()
      }
    }
    // TODO: Add calls to UAMN methods for
    //    * USB controller(s)
    //    * Embedded Controller
    //    * Sensor devices
    //    * Audio DSP?
    //    * Any other devices dependent on User Absent mode for power controls
}

// SMI I/O Trap:
//    Generate a Mutex protected SMI I/O Trap.
//
//  Arguments:
//    Arg0: I/O Trap type.
//               2 - For DTS
//               3 - For IGD
//               4 - For BIOS Guard Tools
//    Arg1: SMI I/O Trap Function to call.
//
//  Return Value:
//    SMI I/O Trap Return value.
//      0 = Success.  Non-zero = Failure.

Scope(\)
{
  //
  // The IO address in this ACPI Operating Region will be updated during POST.
  // This address range is used as a HotKey I/O Trap SMI so that ASL and SMI can
  // communicate when needed.
  //
  OperationRegion(IO_H,SystemIO,0x1000,0x4)
  Field(IO_H,ByteAcc,NoLock,Preserve) {
    TRPH,  8
  }
}

Method(TRAP,2,Serialized)
{
  Store(Arg1,SMIF)        // Store SMI Function.

  If(LEqual(Arg0,TRAP_TYPE_DTS))  // Is DTS IO Trap?
  {
    Store(Arg1,\_PR.DTSF)      // Store the function number global NVS
    Store(0,\_PR.TRPD)         // Generate IO Trap.
    Return(\_PR.DTSF)          // Return status from SMI handler
  }

  If(LEqual(Arg0,TRAP_TYPE_IGD))  // Is IGD IO Trap?
  {
    Store(0,TRPH)         // Generate IO Trap.
  }

  If(LEqual(Arg0,TRAP_TYPE_BGD))  // Is BIOS Guard TOOLS IO Trap?
  {
    Store(0,\_PR.TRPF)         // Generate IO Trap
  }

  Return(SMIF)            // Return SMIF.  0 = Success.
}

// Note:  Only add the indicator device needed by the platform.

//
// System Bus
//
Scope(\_SB.PCI0)
{

  Method(PTMA)
  {
    Return(\_PR.BGMA)
  }

  Method(PTMS)
  {
    Return(\_PR.BGMS)
  }

  Method(PTIA)
  {
    Return(\_PR.BGIA)
  }

  Method(_INI,0, Serialized)
  {
// AMI_OVERRIDE_START >>> It is for Thunderbolt, AMI thunderbolt module has taken care it already.
#if ASL_Thunderbolt_SUPPORT
    Store(1, TBPE)  // AdvancedFeaturesContent
#endif
// AMI_OVERRIDE_END <<< It is for Thunderbolt, AMI thunderbolt module has taken care it already.
    // Determine the OS and store the value, where:
    //
    //   OSYS = 1000 = Linux.
    //   OSYS = 2000 = WIN2000.
    //   OSYS = 2001 = WINXP, RTM or SP1.
    //   OSYS = 2002 = WINXP SP2.
    //   OSYS = 2006 = Vista.
    //   OSYS = 2009 = Windows 7 and Windows Server 2008 R2.
    //   OSYS = 2012 = Windows 8 and Windows Server 2012.
    //   OSYS = 2013 = Windows Blue.
    //
    // Assume Windows 2000 at a minimum.

    Store(2000,OSYS)

    // Check for a specific OS which supports _OSI.

    If(CondRefOf(\_OSI))
    {
// AMI_OVERRIDE_START - Remove incorrect usage    
//      If(\_OSI("Linux"))
//      {
//        Store(1000,OSYS)
//      }
// AMI_OVERRIDE_END - Remove incorrect usage    

      If(\_OSI("Windows 2001")) // Windows XP
      {
        Store(2001,OSYS)
      }

      If(\_OSI("Windows 2001 SP1")) //Windows XP SP1
      {
        Store(2001,OSYS)
      }

      If(\_OSI("Windows 2001 SP2")) //Windows XP SP2
      {
        Store(2002,OSYS)
      }

      If (\_OSI( "Windows 2001.1"))  //Windows Server 2003
      {
        Store(2003,OSYS)
      }

      If(\_OSI("Windows 2006")) //Windows Vista
      {
        Store(2006,OSYS)
      }

      If(\_OSI("Windows 2009")) //Windows 7 and Windows Server 2008 R2
      {
        Store(2009,OSYS)
      }

      If(\_OSI("Windows 2012")) //Windows 8 and Windows Server 2012
      {
        Store(2012,OSYS)
      }

      If(\_OSI("Windows 2013")) //Windows 8.1 and Windows Server 2012 R2
      {
        Store(2013,OSYS)
      }

      If(\_OSI("Windows 2015")) //Windows 10
      {
        Store(2015,OSYS)
      }
    }

    //
    // Set DTS NVS data means in OS ACPI mode enabled insteads of GlobalNvs OperatingSystem (OSYS)
    //
    If(CondRefOf(\_PR.DTSE)){
      If(LGreaterEqual(\_PR.DTSE, 0x01)){
        Store(0x01, \_PR.DSAE)
      }
    }

// AMI_OVERRIDE_START - It is for Thunderbolt, AMI thunderbolt module has taken care it already
#if ASL_Thunderbolt_SUPPORT
    //
    // AdvancedFeaturesBegin
    //
    If(LEqual(TBTS, 0x01)) {
      Acquire(OSUM, 0xFFFF)
// AMI_OVERRIDE_TBT0018 - Add for debug >>
      P8XH(0, 0x51)     // for debug
// AMI_OVERRIDE_TBT0018 - Add for debug <<
//AMI_OVERRIDE_START - EIP315519 - Unable to detect TBT devices after system reset (Boot with TBT attached)
      \_GPE.TINI(TBSE, 0)
//AMI_OVERRIDE_END - EIP315519 - Unable to detect TBT devices after system reset (Boot with TBT attached)
      Release(OSUM)
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
      If(LEqual(TBMP, 0x01)) {
      Acquire(OSUM, 0xFFFF)
      \_GPE.TINI(TBS1)
      Release(OSUM)
      }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
      Signal(WFEV)
    }
    //
    // AdvancedFeaturesEnd
    //
#endif
// AMI_OVERRIDE_END - It is for Thunderbolt, AMI thunderbolt module has taken care it already
  }

  Method(NHPG,0,Serialized)
  {
    Store(0,^RP01.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP02.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP03.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP04.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP05.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP06.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP07.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP08.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP09.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP10.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP11.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP12.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP13.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP14.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP15.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP16.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP17.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP18.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP19.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP20.HPEX) // clear the hot plug SCI enable bit

    Store(1,^RP01.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP02.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP03.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP04.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP05.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP06.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP07.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP08.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP09.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP10.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP11.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP12.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP13.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP14.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP15.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP16.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP17.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP18.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP19.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP20.HPSX) // clear the hot plug SCI status bit
// AMI_OVERRIDE_START - EIP279338 The resource for PEG Thunderbolt host is incorrect.
    Store(1,^RP21.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP22.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP23.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP24.HPSX) // clear the hot plug SCI status bit
// AMI_OVERRIDE_END - EIP279338 The resource for PEG Thunderbolt host is incorrect.
  }

  Method(NPME,0,Serialized)
  {
    Store(0,^RP01.PMEX) // clear the PME SCI enable bit
    Store(0,^RP02.PMEX) // clear the PME SCI enable bit
    Store(0,^RP03.PMEX) // clear the PME SCI enable bit
    Store(0,^RP04.PMEX) // clear the PME SCI enable bit
    Store(0,^RP05.PMEX) // clear the PME SCI enable bit
    Store(0,^RP06.PMEX) // clear the PME SCI enable bit
    Store(0,^RP07.PMEX) // clear the PME SCI enable bit
    Store(0,^RP08.PMEX) // clear the PME SCI enable bit
    Store(0,^RP09.PMEX) // clear the PME SCI enable bit
    Store(0,^RP10.PMEX) // clear the PME SCI enable bit
    Store(0,^RP11.PMEX) // clear the PME SCI enable bit
    Store(0,^RP12.PMEX) // clear the PME SCI enable bit
    Store(0,^RP13.PMEX) // clear the PME SCI enable bit
    Store(0,^RP14.PMEX) // clear the PME SCI enable bit
    Store(0,^RP15.PMEX) // clear the PME SCI enable bit
    Store(0,^RP16.PMEX) // clear the PME SCI enable bit
    Store(0,^RP17.PMEX) // clear the PME SCI enable bit
    Store(0,^RP18.PMEX) // clear the PME SCI enable bit
    Store(0,^RP19.PMEX) // clear the PME SCI enable bit
    Store(0,^RP20.PMEX) // clear the PME SCI enable bit

    Store(1,^RP01.PMSX) // clear the PME SCI status bit
    Store(1,^RP02.PMSX) // clear the PME SCI status bit
    Store(1,^RP03.PMSX) // clear the PME SCI status bit
    Store(1,^RP04.PMSX) // clear the PME SCI status bit
    Store(1,^RP05.PMSX) // clear the PME SCI status bit
    Store(1,^RP06.PMSX) // clear the PME SCI enable bit
    Store(1,^RP07.PMSX) // clear the PME SCI status bit
    Store(1,^RP08.PMSX) // clear the PME SCI status bit
    Store(1,^RP09.PMSX) // clear the PME SCI status bit
    Store(1,^RP10.PMSX) // clear the PME SCI status bit
    Store(1,^RP11.PMSX) // clear the PME SCI status bit
    Store(1,^RP12.PMSX) // clear the PME SCI status bit
    Store(1,^RP13.PMSX) // clear the PME SCI status bit
    Store(1,^RP14.PMSX) // clear the PME SCI status bit
    Store(1,^RP15.PMSX) // clear the PME SCI status bit
    Store(1,^RP16.PMSX) // clear the PME SCI status bit
    Store(1,^RP17.PMSX) // clear the PME SCI status bit
    Store(1,^RP18.PMSX) // clear the PME SCI status bit
    Store(1,^RP19.PMSX) // clear the PME SCI status bit
    Store(1,^RP20.PMSX) // clear the PME SCI status bit
// AMI_OVERRIDE_START - EIP279338 The resource for PEG Thunderbolt host is incorrect.
    Store(1,^RP21.PMSX) // clear the PME SCI status bit
    Store(1,^RP22.PMSX) // clear the PME SCI status bit
    Store(1,^RP23.PMSX) // clear the PME SCI status bit
    Store(1,^RP24.PMSX) // clear the PME SCI status bit
// AMI_OVERRIDE_END - EIP279338 The resource for PEG Thunderbolt host is incorrect.
  }
}

Scope (\)
{
  //
  // Global Name, returns current Interrupt controller mode;
  // updated from _PIC control method
  //
  Name(PICM, 0)

  //
  // Procedure: GPRW
  //
  // Description: Generic Wake up Control Method ("Big brother")
  //              to detect the Max Sleep State available in ASL Name scope
  //              and Return the Package compatible with _PRW format.
  // Input: Arg0 =  bit offset within GPE register space device event will be triggered to.
  //        Arg1 =  Max Sleep state, device can resume the System from.
  //                If Arg1 = 0, Update Arg1 with Max _Sx state enabled in the System.
  // Output:  _PRW package
  //
  Name(PRWP, Package(){Zero, Zero})   // _PRW Package

  Method(GPRW, 2)
  {
    Store(Arg0, Index(PRWP, 0))             // copy GPE#
    //
    // SS1-SS4 - enabled in BIOS Setup Sleep states
    //
    Store(ShiftLeft(SS1,1),Local0)          // S1 ?
    Or(Local0,ShiftLeft(SS2,2),Local0)      // S2 ?
    Or(Local0,ShiftLeft(SS3,3),Local0)      // S3 ?
    Or(Local0,ShiftLeft(SS4,4),Local0)      // S4 ?
    //
    // Local0 has a bit mask of enabled Sx(1 based)
    // bit mask of enabled in BIOS Setup Sleep states(1 based)
    //
    If(And(ShiftLeft(1, Arg1), Local0))
    {
      //
      // Requested wake up value (Arg1) is present in Sx list of available Sleep states
      //
      Store(Arg1, Index(PRWP, 1))           // copy Sx#
    }
    Else
    {
      //
      // Not available -> match Wake up value to the higher Sx state
      //
      ShiftRight(Local0, 1, Local0)
      // If(LOr(LEqual(OSFL, 1), LEqual(OSFL, 2))) {  // ??? Win9x
      // FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Max Sx
      // } Else {           // ??? Win2k / XP
     FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Min Sx
      // }
    }

    Return(PRWP)
  }
}


Scope (\_SB)
{
  Name(OSCI, 0)  // \_SB._OSC DWORD2 input
  Name(OSCO, 0)  // \_SB._OSC DWORD2 output
  Name(OSCP, 0)  // \_SB._OSC CAPABILITIES
  // _OSC (Operating System Capabilities)
  //    _OSC under \_SB scope is used to convey platform wide OSPM capabilities.
  //    For a complete description of _OSC ACPI Control Method, refer to ACPI 5.0
  //    specification, section 6.2.10.
  // Arguments: (4)
  //    Arg0 - A Buffer containing the UUID "0811B06E-4A27-44F9-8D60-3CBBC22E7B48"
  //    Arg1 - An Integer containing the Revision ID of the buffer format
  //    Arg2 - An Integer containing a count of entries in Arg3
  //    Arg3 - A Buffer containing a list of DWORD capabilities
  // Return Value:
  //    A Buffer containing the list of capabilities
  //
// AMI Greenlow Override >>
#ifndef ASL_WS_ACPI_SUPPORT
  Method(_OSC,4,Serialized)
  {
    //
    // Point to Status DWORD in the Arg3 buffer (STATUS)
    //
    CreateDWordField(Arg3, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg3, 4, CAP0)


    //
    // Only set 8254 CG if Low Power S0 Idle Capability is enabled
    //
    If (LEqual(S0ID, One)) {
      //
      //  Set ITSSPRC.8254CGE: Offset 3300h ITSSPRC[2]
      //
// AMI_OVERRIDE_START - For ASL_SPS_ACPI_SUPPORT
#ifndef ASL_SPS_ACPI_SUPPORT
      Store(0x01, \_SB.SCGE)
#endif
// AMI_OVERRIDE_END - For ASL_SPS_ACPI_SUPPORT
    }

    //
    // Check UUID
    //
    If(LEqual(Arg0,ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48")))
    {
      //
      // Check Revision
      //
      If(LEqual(Arg1,One))
      {
        Store(CAP0, OSCP)
        If(And(CAP0,0x04)) // Check _PR3 Support(BIT2)
        {
          Store(0x04, OSCO)
          If(LNotEqual(And(SGMD,0x0F),2)) // Check Switchable/Hybrid graphics is not enabled in bios setup [SgModeMuxless]?
          {
            If(LEqual(RTD3,0)) // Is RTD3 support disabled in Bios Setup?
            {
              // RTD3 is disabled via BIOS Setup.
              And(CAP0, 0x3B, CAP0) // Clear _PR3 capability
              Or(STS0, 0x10, STS0) // Indicate capability bit is cleared
            }
          }
        }
      } Else{
        And(STS0,0xFFFFFF00,STS0)
        Or(STS0,0xA, STS0) // Unrecognised Revision and report OSC failure
      }
    } Else {
      And(STS0,0xFFFFFF00,STS0)
      Or (STS0,0x6, STS0) // Unrecognised UUID and report OSC failure
    }

    Return(Arg3)
  } // End _OSC
#else
  Method(_OSC,4,Serialized)
  {
    //
    // Point to Status DWORD in the Arg3 buffer (STATUS)
    //
    CreateDWordField(Arg3, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg3, 4, CAP0)
    //
    // Only set 8254 CG if Low Power S0 Idle Capability is enabled
    //
    If (LEqual(S0ID, One)) {
      //
      //  Set ITSSPRC.8254CGE: Offset 3300h ITSSPRC[2]
      //
      #ifndef ASL_SPS_ACPI_SUPPORT
      Store(0x01, \_SB.SCGE)
#endif
    }
// AMI Greenlow Server Override for Legacy Windows >>
    // Store(0x01, \_SB.SCGE)
// AMI Greenlow Server Override for Legacy Windows >>
    //
    // Check UUID
    //
    If(LEqual(Arg0,ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48")))
    {
      //
      // Check Revision
      //
      If(LEqual(Arg1,One))
      {
        Store(CAP0, OSCP)
        If(And(CAP0,0x04)) // Check _PR3 Support(BIT2)
        {
          Store(0x04, OSCO)
          If(LNotEqual(And(SGMD,0x0F),2)) // Check Switchable/Hybrid graphics is not enabled in bios setup [SgModeMuxless]?
          {
            If(LEqual(RTD3,0)) // Is RTD3 support disabled in Bios Setup?
            {
              // RTD3 is disabled via BIOS Setup. 
              And(CAP0, 0x3B, CAP0) // Clear _PR3 capability
              Or(STS0, 0x10, STS0) // Indicate capability bit is cleared
            }
          }
        }
        
        //WHEA
        If(And(CAP0,0x10)) { //Os is requesting whea support 
                
            // If OS not querying, enable WHEA in platform
            If(Not(And(STS0,1))) {   //Query Flag Clear?
            //SWWE(0x9E) //enable whea signals 
            }                                   
        }
        Return(Arg3)
      } 
      // Unrecognised Revision and report OSC failure
      And(STS0,0xFFFFFF00,STS0)
      Or(STS0,0xA, STS0) 
      Return(Arg3)
    }
    
    // OSC for WHEA
    // Check for WHEA GUID
    If(LEqual(Arg0, ToUUID("ed855e0c-6c90-47bf-a62a-26de0fc5ad5c")))
    {

        If(And(CAP0, 0x01)) { //Whea requested

            If(Not(And(STS0,1))) {   //Query Flag Clear?
            //SWWE(0x9E) //enable whea signals 
            }                      
        }
    Return(Arg3)
    }
    
#ifdef ASL_SPS_ACPI_SUPPORT

#endif
    
    // Unrecognised UUID and report OSC failure
    And(STS0,0xFFFFFF00,STS0)
    Or (STS0,0x6, STS0) 
    Return(Arg3)
  } // End _OSC
#endif //ASL_WS_ACPI_SUPPORT
// AMI Greenlow Override <<

  //
  // AdvancedFeaturesBegin
  //
   Device (EPC)
   {
     Name (_HID, EISAID ("INT0E0C"))
     Name (_STR, Unicode ("Enclave Page Cache 1.0"))
     Name (_MLS, Package () {
       Package (2) { "en", Unicode ("Enclave Page Cache 1.0") }
     })

     Name (RBUF, ResourceTemplate ()
     {
       QWordMemory (
         ResourceConsumer,    // ResourceUsage
         PosDecode,           // Decode               _DEC
         MinNotFixed,         // IsMinFixed           _MIF
         MaxNotFixed,         // IsMaxFixed           _MAF
         NonCacheable,        // Cacheable            _MEM
         ReadWrite,           // ReadAndWrite         _RW
         0,                   // AddressGranularity   _GRA
         0,                   // AddressMinimum       _MIN  <= To be patched in Runtime
         0,                   // AddressMaximum       _MAX  <= To be patched in Runtime
         0,                   // AddressTranslation   _TRA
         1,                   // RangeLength          _LEN  <= To be patched in Runtime
         ,                    // ResourceSourceIndex
         ,                    // ResourceSource
         BAR0                 // DescriptorName
       )
     })

     Method (_CRS, 0x0, NotSerialized)
     {
       CreateQwordField (RBUF, ^BAR0._MIN, EMIN)
       CreateQwordField (RBUF, ^BAR0._MAX, EMAX)
       CreateQwordField (RBUF, ^BAR0._LEN, ELEN)
       Store (\_PR.EMNA, EMIN)
       Store (\_PR.ELNG, ELEN)
       Subtract (Add (\_PR.EMNA, \_PR.ELNG), 1, EMAX)
       Return (RBUF)
     }

     Method (_STA, 0x0, NotSerialized)
     {
       If (LNotEqual (\_PR.EPCS, 0))
       {
         Return (0xF)
       }
       Return (0x0)
     }
   } // end EPC Device
} // End of Scope(\_SB)

Scope (\_SB)
{
  //
  //  define a BT RF-Kill device.
  //
  Device (BTKL)
  {
    Name(_HID, "INT3420")

    Method (_STA, 0x0, NotSerialized)
    {
      Return(0x00)  // not support BT RF-Kill, device hide.
    }

    // D0 Method for BTKL
    Method(_PS0,0,Serialized)
    {
    }

    // D3 Method for BTKL
    Method(_PS3,0,Serialized)
    {
    }

    // detect GPIO pin status
    Method(PSTS)
    {
    }
  }
  //
  // AdvancedFeaturesEnd
  //
} // End of Scope(\_SB)

//
// CS Wake up event support
//
Scope (\_SB)
{
  // Define Sleep button to put the system in sleep
  Device (SLPB)
  {
    Name (_HID, EISAID ("PNP0C0E"))
    Name (_STA, 0x0B)
    // Bit0 - the device is present: Yes.
    // Bit1 - the device is enabled and decoding its resources: Yes.
    // Bit2 - the device should be shown in the UI: No.
    // Bit3 - the device is functioning properly: Yes.
    // Bit4 - the battery is present: N/A
  }
} // End of Scope(\_SB)
//
// AdvancedFeaturesBegin
//
//
// RTVM - Enable Run Time VM control to voltage margin PrimCore Volatge on SLP_S0 assertion
//
If(LNotEqual(RTVM,0))
{
  Scope(\_SB){
    Name (VMEN, 0xFF) // VM enabled flag (0xFF - undefined value)

    // Enable Voltage Margining
    Method(VMON, 0, Serialized)
    {
      If(LEqual(VMEN, 1)) {
        Return // VM already enabled
      }

      If(LEqual(RTVM,1)) { // Premium PMIC
        // Program PMIC V085ACNT[7:6]= 01
        \_SB.PCI0.LPCB.H_EC.ECWT(0x7A, RefOf(\_SB.PCI0.LPCB.H_EC.PVOL))
        \_SB.PCI0.LPCB.H_EC.ECMD(0x1C)
      } ElseIf(LEqual(RTVM,2)) {
        // Assert pin (active low signal) controlling discrete VR
        SGOV(VRGP, 0)
      } ElseIf (LEqual(RTVM, 4)) { // For DVRM Card with IO EXP VM Capability
        // Program DVRM IO EXP
        \_SB.PCI0.LPCB.H_EC.ECMD(0xDF)
      }
      Store(1, VMEN)
    }

    // Disable Voltage Margining
    Method(VMOF, 0, Serialized)
    {
      If(LEqual(VMEN, 0)) {
        Return // VM already disabled
      }

      If(LEqual(RTVM,1)) { // Premium PMIC
        // Program PMIC V085ACNT[7:6]= 00
        \_SB.PCI0.LPCB.H_EC.ECWT(0x3A, RefOf(\_SB.PCI0.LPCB.H_EC.PVOL))
        \_SB.PCI0.LPCB.H_EC.ECMD(0x1C)
      } ElseIf(LEqual(RTVM,2)) {
        // Deassert pin (active low signal) controlling discrete VR
        SGOV(VRGP, 1)
      } ElseIf (LEqual(RTVM,4)) { // For DVRM Card with IO EXP VM Capability
        // Program DVRM IO EXP
        \_SB.PCI0.LPCB.H_EC.ECMD(0xDE)
      }
      Store(0, VMEN)
    }
  }
} // End RTVM

// -------------------------------------------------------
//        TBT ACPI implementation - Begin
// -------------------------------------------------------
// General Purpose Events.  This Scope handles the Run-time and
// Wake-time SCIs.  The specific method called will be determined by
// the _Lxx value, where xx equals the bit location in the General
// Purpose Event register(s).

// AMI_OVERRIDE_START - It is for Thunderbolt, AMI thunderbolt module has taken care it already
#if ASL_Thunderbolt_SUPPORT
Name(TDMA, 0x80000000) // Address of Thunderbolt(TM) debug memory buffer, fixed up during POST
Name(TDPG, 0x80000000) // Address of Thunderbolt External GPU PCI Configuration Space Buffer, fixed up during POST
Name(TDTI, 0x80000000) // Address of Thunderbolt Device Tree Info Buffer, fixed up during POST
Name(TRDO, 0) // 1 during TBT RTD3 _ON
Name(TRD3, 0) // 1 during TBT RTD3 _OFF
Name(TBPE, 0) // Reflects RTD3_PWR_EN value
Name(TOFF, 0) // param to TBT _OFF method
Scope(\_GPE)
{
  //
  //
  //OS up handshake procedure to host router upstream port each time
  //exiting from Sx State .Avoids intermediate
  //PCIe Scan by OS during resorce allocation
  //
  Method(OSUP, 1, Serialized)
  {
    ADBG(Concatenate("OSUP=",ToHexString(Arg0)))
    Add(Arg0, 0x548, Local0)
    OperationRegion(PXVD,SystemMemory,Local0,0x08)
    Field(PXVD,DWordAcc, NoLock, Preserve)
    {
      TB2P, 32,
      P2TB, 32
    }

    Store(100, Local1)
    Store(0x0D, P2TB) // Write OS_Up to PCIe2TBT
    While(LGreater(Local1, 0))
    {
      Store(Subtract(Local1, 1), Local1)
      Store(TB2P, Local2)
      If(LEqual(Local2, 0xFFFFFFFF))// Device gone
      {
        ADBG("Dev gone")
        Return(2)
      }
      If(And(Local2, 1)) // Done
      {
        ADBG("Cmd acknowledged")
        break
      }
      Sleep(50)
    }
    If(LEqual(TRWA,1))
    {
      Store(0xC, P2TB) // Write OSUP to PCIe2TBT
    }
    Else
    {
      Store(0x0, P2TB) // Write 0 to PCIe2TBT
    }

    ADBG("End-of-OSUP")

    Return(1)
  } // End of Method(OSUP, 1, Serialized)

  //
  // Set PEG controllers to D0 state (only if it is not in D0 state)
  // and set their links to be enabled if it disabled means
  //
  Method(PGWA, 1, Serialized)
  {
    ADBG("PGWA")
// AMI_OVERRIDE_START - EIP279338 The resource for PEG Thunderbolt host is incorrect.
    If(LGreaterEqual(Arg0, 25))
// AMI_OVERRIDE_END - EIP279338 The resource for PEG Thunderbolt host is incorrect.
    {
      Store(\_SB.PCI0.GPCB(), Local0) // MMIO Base address
      Add(Local0, ShiftLeft(TBTD(Arg0), 15), Local0) // Device no
      Add(Local0, ShiftLeft(TBTF(Arg0), 12), Local0) // Function no

      ADBG(Local0) // PEGx controller MMIO address

      OperationRegion (ABCD, SystemMemory, Local0, 0x1000)
      Field (ABCD, AnyAcc, NoLock, Preserve)
      {
        Offset(0x84),  // Power Management Control/Status
        PWRS, 2,       // 1:0 PowerState
        Offset(0xB0),  // Link Control
            , 4,
        LNKD, 1,       // 4:4 Link Disable
        Offset(0x11A), // VC0RSTS
        ,     1,
        VCNP, 1,       // 1:1 VC0 Negotiation Pending
        Offset(0x508), // TRNEN
        TREN, 1,       // 0:0 Train Enable
      }

      If(LNotEqual(PWRS, 0))
      {
        ADBG("Force D0")

        Store (0, PWRS) // Set to D0 state
        Store (0, \_PR.POWS) // Set to D0 state
        //Provide minimum recovery time requirement of 10 ms
        Sleep(16) //In some OS one tick is equal to 1/64 second (15.625ms)
      }

      If(LNotEqual(LNKD, 0))
      {
        ADBG("Link Enable")

        // Enable x16 Link
        Store (0, LNKD) // Set to Link Enable

        // Enable controller initial training
        Store (1, TREN)

        // Wait until the VC negotiation is complete
        Store(0, Local6)
        Store(100, Local7)
        While(LLess(Local6, Local7))
        {
          If(LEqual(VCNP,0))
          {
            Break
          }

          Sleep(16) //In some OS one tick is equal to 1/64 second (15.625ms)
          Add(Local6, 16, Local6)
        }
      }
    }
  }


  Method(TBFF, 1, Serialized) { // Check for FFFF in TBT
    ADBG("TBFF")

    Store(MMTB(Arg0), Local0)
    OperationRegion (PXVD, SystemMemory, Local0, 0x8)
    Field (PXVD, DWordAcc, NoLock, Preserve) {
      VEDI, 32, // Vendor/Device ID
      CMDR, 32 // CMD register
    }

    Store(VEDI, Local1)

    If (LEqual(Local1, 0xFFFFFFFF)) {
      If (LNotEqual(\TWIN, 0)) { // TBT Enumeration is Native mode?
        If (LEqual(CMDR, 0xFFFFFFFF)) { // Device Gone
          Return (2)// Notify only
        }
        Return (1)// Exit w/o notify
      } Else {
        Return (OSUP(Local0))
      }
    } Else {
      ADBG("Dev Present")
      Return (0)
    }
  } // End of Method(TBFF, 1, Serialized)

  // Secondary bus of TBT RP
  Method(TSUB, 1, Serialized)
  {
    ADBG("TSUB")

    Store(\_SB.PCI0.GPCB(), Local0) // MMIO Base address

    Add(Local0, ShiftLeft(TBTD(Arg0), 15), Local0) // Device no
    Add(Local0, ShiftLeft(TBTF(Arg0), 12), Local0) // Function no

    ADBG("ADR")
    ADBG(Local0)

    OperationRegion (MMMM, SystemMemory, Local0, 0x1A)
    Field (MMMM, AnyAcc, NoLock, Preserve)
    {
      Offset(0x19),
      SBUS, 8
    }

    ADBG("Sec Bus")
    ADBG(SBUS)

    Return(SBUS)
  } // End of Method(TSUB, 0, Serialized)

  // Wait for secondary bus in TBT RP
  Method(WSUB, 1, Serialized)
  {
    ADBG("WSUB")

    Store(0, Local0)
    Store(0, Local1)
    While(1)
    {
      Store(TSUB(Arg0), Local1)
      If(Local1)
      {
        ADBG("WSUB-Finished")
        Break
      }
      Else
      {
        Add(Local0, 1, Local0)
        If(LGreater(Local0, 1000))
        {
          Sleep(1000)
          ADBG("WSUB-Deadlock")
        }
        Else
        {
          Sleep(16)
        }
      }
    }
  } // End of Method(WSUB)

  // Wait for _WAK finished
  Method(WWAK)
  {
    ADBG("WWAK")

    Wait(WFEV, 0xFFFF)
    Signal(WFEV) // Set it, to enter on next HP
  } // End of Method(WWAK)

  Method(NTFY, 1, Serialized)
  {
    ADBG("NTFY")

    If(LEqual(NOHP,1))
    {
      Switch(ToInteger(Arg0)) // TBT Selector
      {
        Case (1)
        {
          ADBG("Notify RP01")
          Notify(\_SB.PCI0.RP01,0)
        }
        Case (2)
        {
          ADBG("Notify RP02")
          Notify(\_SB.PCI0.RP02,0)
        }
        Case (3)
        {
          ADBG("Notify RP03")
          Notify(\_SB.PCI0.RP03,0)
        }
        Case (4)
        {
          ADBG("Notify RP04")
          Notify(\_SB.PCI0.RP04,0)
        }
        Case (5)
        {
          ADBG("Notify RP05")
          Notify(\_SB.PCI0.RP05,0)
        }
        Case (6)
        {
          ADBG("Notify RP06")
          Notify(\_SB.PCI0.RP06,0)
        }
        Case (7)
        {
          ADBG("Notify RP07")
          Notify(\_SB.PCI0.RP07,0)
        }
        Case (8)
        {
          ADBG("Notify RP08")
          Notify(\_SB.PCI0.RP08,0)
        }
        Case (9)
        {
          ADBG("Notify RP09")
          Notify(\_SB.PCI0.RP09,0)
        }
        Case (10)
        {
          ADBG("Notify RP10")
          Notify(\_SB.PCI0.RP10,0)
        }
        Case (11)
        {
          ADBG("Notify RP11")
          Notify(\_SB.PCI0.RP11,0)
        }
        Case (12)
        {
          ADBG("Notify RP12")
          Notify(\_SB.PCI0.RP12,0)
        }
        Case (13)
        {
          ADBG("Notify RP13")
          Notify(\_SB.PCI0.RP13,0)
        }
        Case (14)
        {
          ADBG("Notify RP14")
          Notify(\_SB.PCI0.RP14,0)
        }
        Case (15)
        {
          ADBG("Notify RP15")
          Notify(\_SB.PCI0.RP15,0)
        }
        Case (16)
        {
          ADBG("Notify RP16")
          Notify(\_SB.PCI0.RP16,0)
        }
        Case (17)
        {
          ADBG("Notify RP17")
          Notify(\_SB.PCI0.RP17,0)
        }
        Case (18)
        {
          ADBG("Notify RP18")
          Notify(\_SB.PCI0.RP18,0)
        }
        Case (19)
        {
          ADBG("Notify RP19")
          Notify(\_SB.PCI0.RP19,0)
        }
        Case (20)
        {
          ADBG("Notify RP20")
          Notify(\_SB.PCI0.RP20,0)
        }
// AMI_OVERRIDE_START - EIP279338 The resource for PEG Thunderbolt host is incorrect.
        Case (21)
        {
          ADBG("Notify RP21")
          Notify(\_SB.PCI0.RP21,0)
        }
        Case (22)
        {
          ADBG("Notify RP22")
          Notify(\_SB.PCI0.RP22,0)
        }        
        Case (23)
        {
          ADBG("Notify RP23")
          Notify(\_SB.PCI0.RP23,0)
        }
        Case (24)
        {
          ADBG("Notify RP24")
          Notify(\_SB.PCI0.RP24,0)
        }
        Case (25)
        {
          ADBG("Notify PEG0")
          Notify(\_SB.PCI0.PEG0,0)
        }
        Case (26)
        {
          ADBG("Notify PEG1")
          Notify(\_SB.PCI0.PEG1,0)
        }
        Case (27)
        {
          ADBG("Notify PEG2")
          Notify(\_SB.PCI0.PEG2,0)
        }
// AMI_OVERRIDE_END - EIP279338 The resource for PEG Thunderbolt host is incorrect.
      }//Switch(ToInteger(TBSS)) // TBT Selector
    }//If(NOHP())
// AMI_OVERRIDE_START - Add for debug
//    P8XH(0,0xC2)
//    P8XH(1,0xC2)
// AMI_OVERRIDE_END - Add for debug
  }// End of Method(NTFY)

  Method(NFYG)
  {
    ADBG("NFYG")

    If(LEqual(TDGS,1))
    {
      If(LEqual(DCKE, 1))
      {
        ADBG("NFYG.DCKE")
        Notify(\_SB.PCI0.GFX0,0x81)
      }
      ElseIf(LEqual(SUDK, 1))
      {
        ADBG("NFYG.SUDK")
        Notify(\_SB.PCI0.GFX0,0x81)
      }
    }
  }// End of Method(NFYG)

  Method(TFPS) // Force power status
  {
    ADBG("TFPS")

    Store(\_SB.CGRD(FPAT, FPEN, FPGN, 0), Local0)

    If(Local0)
    {
      ADBG("ExtFrcPwr1")
    }
    Else
    {
      ADBG("ExtFrcPwr0")
    }

    Return(Local0)
  } // End of Method(TFPS)

  Method(CNCT)// Is Connect event
  {
    ADBG("CNCT")
    ADBG("Read")
    ADBG("ACPI_GPE_STS")
    Store(CPAD, Local7) //Address
    Store(CPAB, Local6) //Bit position

    While(LGreater(Local6, 8))
    {
      Add(Local7, 1, Local7)
      Subtract(Local6, 8, Local6)
    }

    OperationRegion(GPE0, SystemIO, Local7, 1)
    Field (GPE0, ByteAcc, Lock, Preserve)
    {
      TEMP, 8,
    }
    Store(TEMP, Local0)
    ShiftRight(Local0, Local6, Local0)
    And(Local0, 1, Local0)

    Return(Local0)
  } // End of Method(CNCT)

  Method(CLNE)// Clean GPIO event
  {
    ADBG("CLNE")
    ADBG("Clear")
    ADBG("ACPI_GPE_STS")
    Store(CPAD, Local7) //Address
    Store(CPAB, Local6) //Bit position

    While(LGreater(Local6, 8))
    {
      Add(Local7, 1, Local7)
      Subtract(Local6, 8, Local6)
    }

    OperationRegion(GPE0, SystemIO, Local7, 1)
    Field (GPE0, ByteAcc, Lock, Preserve)
    {
      TEMP, 8,
    }
    ShiftLeft(1, Local6, Local6)
    Or(TEMP, Local6, TEMP) // Clean GPIO status
  } // End of Method(CLNE)

//
//  TBT BIOS, GPIO 5 filtering,
//  Hot plug of 12V USB devices, into TBT host router, cause electrical noise on PCH GPIOs,
//  This noise cause false hot-plug events, and negatively influence BIOS assisted hot-plug.
//  SPT GPIO does not implement Glitch Filter logic (refer to GPIO HAS) on any GPIO pad. Native functions have to implement their own digital glitch-filter logic
//  if needed. As HW filter was not implemented on SKL PCH, because of that SW workaround should be implemented in BIOS.
//  Register 0x544(Bios mailbox) bit 0 definition:
//  if BIOS reads bit as 1, BIOS will clear the bit and continue normal flow, if bit is 0 BIOS will exit from method
//

  Method(GNIS,1, Serialized)
  {

    ADBG("GNIS")
    If(LEqual(GP5F, 0))
    {
      ADBG("GNIS_Dis=0")
      Return(0)
    }
    //
    // BIOS mailbox command for GPIO filter
    //
    Add(MMTB(Arg0), 0x544, Local0)
    OperationRegion(PXVD,SystemMemory,Local0,0x08)

    Field(PXVD,DWordAcc, NoLock, Preserve)
    {
      HPFI, 1,
      Offset(0x4),
      TB2P, 32
    }
    Store(TB2P, Local1)
    ADBG(Concatenate("TB2P=", ToHexString(Local1)))
    If(LEqual(Local1, 0xFFFFFFFF)) // Disconnect?
    {
      ADBG("GNIS=0")
      Return(0)
    }
    Store(HPFI, Local2)
    ADBG(Concatenate("HPFI=", ToHexString(Local2)))
    If(LEqual(Local2, 0x01))
    {
      Store(0x00, HPFI)
      ADBG("GNIS=0")
      Return(0)
    }
    // Any other values treated as a GPIO noise
    ADBG("GNIS=1")
    Return(1)
  }

  //
  // Method to Handle enumerate PCIe structure through
  // SMI for Thunderbolt(TM) devices
  //
  Method(XTBT,2, Serialized)
  {
    ADBG("XTBT")
    //
    // Bios needs to clear status bit if its a 2-tier edge triggered GPIO.
    //
    If(LEqual(CF2T, 1))
    {
      ADBG("Clear")
      ADBG("GPI_GPE_STS")
      \_SB.CAGS(Arg1)
    }

    If (TRDO) {
      ADBG("Durng TBT_ON")
      Return ()
    }

    If (TRD3) {
      ADBG("During TBT_OFF")
      Return ()
    }

    WWAK()
    WSUB(Arg0)

    If(GNIS(Arg0))
    {
      Return()
    }

    OperationRegion(SPRT,SystemIO, 0xB2,2)
    Field (SPRT, ByteAcc, Lock, Preserve)
    {
      SSMP, 8
    }

    ADBG("TBT-HP-Handler")

    ADBG("PEG WorkAround")
    PGWA(Arg0)

    Acquire(OSUM, 0xFFFF)
    Store(TBFF(Arg0), Local1)
    If(LEqual(Local1, 1))// Only HR
    {
      Sleep(16)
      Release(OSUM)
      ADBG("OS_Up_Received")
      Return ()
    }
    If(LEqual(Local1, 2)) // Disconnect
    {
      ADBG("Disconnect")
      If(LEqual(OHPN, 1))
      {
        NTFY(Arg0)
      }

      If(LEqual(GHPN, 1))
      {
        NFYG()
      }

      Sleep(16)
      Release(OSUM)
// AMI_OVERRIDE_START - Add for debug
      P8XH(0,0x7d)
// AMI_OVERRIDE_END - Add for debug
      ADBG("Disconnect")
      Return ()
    }

    // HR and EP

    // Trigger SMI to enumerate PCIe Structure
    If(LEqual(SOHP, 1))
    {
// AMI_OVERRIDE_START - Update the SWSMI value by Token.
#if 0
      If(LEqual(Arg1,CPG1))
      {
        ADBG("TBT SW SMI 2")
        Store(24, TBSF)
        Store(0xF7, SSMP)
        Store(27, TBSF)
        Store(0xF7, SSMP)
      }
      Else
      {
        ADBG("TBT SW SMI")
        Store(21, TBSF)
        Store(0xF7, SSMP)
        Store(26, TBSF)
        Store(0xF7, SSMP)
      }
#else
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
      If(LEqual(Arg1,CPG1))
      {
        ADBG("TBT SW SMI 2")
        Store(TEF1, TBSF)
        Store(TBSW, SSMP)
        Store(TEC1, TBSF)
        Store(TBSW, SSMP)
      }
      Else
      {
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
        ADBG("TBT SW SMI")
        Store(TBEF, TBSF)
        Store(TBSW, SSMP)
        Store(TEC0, TBSF)
        Store(TBSW, SSMP)
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
      }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
#endif
// AMI_OVERRIDE_END - Update the SWSMI value by Token.
    }
    If(LEqual(OHPN, 1))
    {
      NTFY(Arg0)
    }

    If(LEqual(GHPN, 1))
    {
      NFYG()
    }

    Sleep(16)
    Release(OSUM)

    ADBG("End-of-XTBT")
  } // End of Method(XTBT)

  //
  // Calling Method to Handle enumerate PCIe structure through
  // SMI for Thunderbolt(TM) devices for Tier 1 GPIOs
  //
  Method(YTBT)
  {
    ADBG("YTBT")
    //
    // Calling Method to Handle enumerate PCIe structure through
    //
    XTBT(TBSE,CPGN)
    ADBG("End-of-YTBT")
  } // End of Method(YTBT)

//AMI_OVERRIDE_START - EIP315519 - Unable to detect TBT devices after system reset (Boot with TBT attached)
//
//  Arg0: TBSE, Thunderbolt(TM) Root port selector
//  Arg1: Sx state, the sleeping state
//
//  Method(TINI, 1, Serialized)
  Method(TINI, 2, Serialized)
  {
    ADBG("TINI")
    //
    // S3 wake:
    // Assign a temp bus for Thunderbolt HR due to
    // Secondary Bus of TBT root port is not ready yet
    //
    If (LEqual(Arg1, 3))
    {

    Store(MMRP(Arg0), Local0)
    OperationRegion(RP_X,SystemMemory,Local0,0x20)
    Field(RP_X,DWordAcc, NoLock, Preserve)
    {
      REG0, 32,
      REG1, 32,
      REG2, 32,
      REG3, 32,
      REG4, 32,
      REG5, 32,
      REG6, 32,
      REG7, 32
    }
    Store(REG6, Local1)
// AMI_OVERRIDE_START - Avoid this address for OEM used.
#if 0
    Store(0x00F0F000, REG6)
#else
    Store(0x003F3F00, REG6)
#endif
// AMI_OVERRIDE_END - Avoid this address for OEM used.
    }
//AMI_OVERRIDE_END - EIP315519 - Unable to detect TBT devices after system reset (Boot with TBT attached)
    Store(MMTB(Arg0), Local2)
    OSUP(Local2)
//AMI_OVERRIDE_START - EIP315519 - Unable to detect TBT devices after system reset (Boot with TBT attached)
    //
    // Restore the original bus
    //
    If (LEqual(Arg1, 3))
    {
      Store(Local1, REG6)
    }
//AMI_OVERRIDE_END - EIP315519 - Unable to detect TBT devices after system reset (Boot with TBT attached)

    ADBG("End-of-TINI")
  }
} // End of Scope (\_GPE)

Scope (\_SB)
{
  //
  // The code needs to be executed for TBT Hotplug Handler event (2-tier GPI GPE event architecture) is presented here
  //
  Method(THDR, 2, Serialized)
  {
    ADBG("THDR")
    \_GPE.XTBT(Arg0,Arg1)
  } // End of Method(THDR, 0, Serialized)
} // End of Scope(\_SB)
#endif
// AMI_OVERRIDE_END - It is for Thunderbolt, AMI thunderbolt module has taken care it already.

Scope (\_SB)
{
  //
  // Name: CGWR [Combined GPIO Write]
  // Description: Function to write into GPIO
  // Input: Arg0 -> GpioAccessType
  //        Arg1 -> Expander
  //        Arg2 -> GpioPad / Expander pin
  //        Arg3 -> Value
  // Return: Nothing
  //
  Method(CGWR, 4, Serialized)
  {
    If (LEqual(Arg0, 1))
    {
      // PCH
      If (CondRefOf(\_SB.SGOV))
      {
        \_SB.SGOV(Arg2, Arg3)
      }
    }
    ElseIf (LEqual(Arg0, 2))
    {
      // IoExpander {TCA6424A}
      If (CondRefOf(\_SB.PCI0.GEXP.SGEP))
      {
        \_SB.PCI0.GEXP.SGEP(Arg1, Arg2, Arg3)
      }
    }
  } // End of Method(CGWR, 4, Serialized)

  //
  // Name: CGRD [Combined GPIO Read]
  // Description: Function to read from GPIO
  // Input: Arg0 -> GpioAccessType
  //        Arg1 -> Expander
  //        Arg2 -> GpioPad / Expander pin
  //        Arg3 -> 0: GPO [GPIO TX State]
  //                1: GPI [GPIO RX State]
  // Return: Value
  //
  Method(CGRD, 4, Serialized)
  {
    Store(1, Local0)

    If (LEqual(Arg0, 1))
    {
      // PCH
      If (LEqual(Arg3, 0))
      {
        // GPIO TX State
        If (CondRefOf(\_SB.GGOV))
        {
          Store(\_SB.GGOV(Arg2), Local0)
        }
      }
      ElseIf (LEqual(Arg3, 1))
      {
        // GPIO RX State
        If (CondRefOf(\_SB.GGIV))
        {
          Store(\_SB.GGIV(Arg2), Local0)
        }
      }
    }
    ElseIf (LEqual(Arg0, 2))
    {
      // IoExpander {TCA6424A}
      If (CondRefOf(\_SB.PCI0.GEXP.GEPS))
      {
        Store(\_SB.PCI0.GEXP.GEPS(Arg1, Arg2), Local0)
      }
    }

    Return(Local0)
  } // End of Method(CGRD, 4, Serialized)

  //
  // Name: WRGP [GPIO Write]
  // Description: Function to write into GPIO
  // Input: Arg0 -> COMMON_GPIO_CONFIG GpioInfo
  //        Arg1 -> Value
  // Return: Nothing
  //
  Method(WRGP, 2, Serialized)
  {
    Store(Arg0, Local0)
    Store(Arg0, Local1)
    And(Local0, 0xFFFFFFFF, Local0) // Low  32 bits (31:00)
    ShiftRight(Local1, 32, Local1)  // High 32 bits (63:32)
    If (LEqual(And(Local0, 0xFF), 1))
    {
      // PCH
      \_SB.CGWR(And(Local0, 0xFF), ShiftRight(Local1, 24), Local1, Arg1)
    }
    ElseIf (LEqual(And(Local0, 0xFF), 2))
    {
      // I/O Expander
      \_SB.CGWR(And(Local0, 0xFF), ShiftRight(Local1, 24), ShiftRight(ShiftLeft(Local1,8), 24), Arg1)
    }
  } // End of Method(WRGP, 2, Serialized)

  //
  // Name: RDGP [GPIO Read]
  // Description: Function to write into GPIO
  // Input: Arg0 -> COMMON_GPIO_CONFIG GpioInfo
  //        Arg1 -> In case of PCH Gpio Read {GPIO TX(0)/RX(1) State indicator}
  // Return: Value
  //
  Method(RDGP, 2, Serialized)
  {
    Store(1, Local7)
    Store(Arg0, Local0)
    Store(Arg0, Local1)
    And(Local0, 0xFFFFFFFF, Local0) // Low  32 bits (31:00)
    ShiftRight(Local1, 32, Local1)  // High 32 bits (63:32)
    If (LEqual(And(Local0, 0xFF), 1))
    {
      // PCH
      Store(\_SB.CGRD(And(Local0, 0xFF), ShiftRight(Local1, 24), Local1, Arg1), Local7)
    }
    ElseIf (LEqual(And(Local0, 0xFF), 2))
    {
      // I/O Expander
      Store(\_SB.CGRD(And(Local0, 0xFF), ShiftRight(Local1, 24), ShiftRight(ShiftLeft(Local1,8), 24), Arg1), Local7)
    }

    Return(Local7)
  } // End of Method(RDGP, 2, Serialized)

} // End of Scope(\_SB)

// AMI_OVERRIDE_START - It is for Thunderbolt, AMI thunderbolt module has taken care it already
#if ASL_Thunderbolt_SUPPORT
Scope(\_SB)
{
  // Asserts/De-asserts TBT force power
  Method(TBFP, 1)
  {
    If(Arg0)
    {
      // Implementation dependent way to assert TBT force power
      CGWR(FPAT, FPEN, FPGN, FPLV)
    }
    Else
    {
      // Implementation dependent way to de-assert TBT force power
      CGWR(FPAT, FPEN, FPGN, LNot(FPLV))
    }
  }

  // WMI ACPI device to control TBT force power
  Device(WTBT)
  {
    // pnp0c14 is pnp id assigned to WMI mapper
    Name(_HID, "PNP0C14")
    Name(_UID, "TBFP")

    Name(_WDG, Buffer() {
      // {86CCFD48-205E-4A77-9C48-2021CBEDE341}
      0x48, 0xFD, 0xCC, 0x86,
      0x5E, 0x20,
      0x77, 0x4A,
      0x9C, 0x48,
      0x20, 0x21, 0xCB, 0xED, 0xE3, 0x41,
      84, 70,    // Object Id (TF)
      1,         // Instance Count
      0x02       // Flags (WMIACPI_REGFLAG_METHOD)
    })

    // Set TBT force power
    // Arg2 is force power value
    Method(WMTF, 3)
    {
      CreateByteField(Arg2,0,FP)

      If(FP)
      {
        TBFP(1)
      }
      Else
      {
        TBFP(0)
      }
    }
  }
} // End of Scope(\_SB)


// AMI_OVERRIDE_START - For the structure of AMI asl code scope.
#if 0
If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 1),LEqual(TBS1, 1))))
{
  Scope(\_SB.PCI0.RP01)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP01)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 2),LEqual(TBS1, 2))))
{
  Scope(\_SB.PCI0.RP02)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP02)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 3),LEqual(TBS1, 3))))
{
  Scope(\_SB.PCI0.RP03)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP03)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 4),LEqual(TBS1, 4))))
{
  Scope(\_SB.PCI0.RP04)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP04)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 5),LEqual(TBS1, 5))))
{
  Scope(\_SB.PCI0.RP05)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP05)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 6),LEqual(TBS1, 6))))
{
  Scope(\_SB.PCI0.RP06)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP06)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 7),LEqual(TBS1, 7))))
{
  Scope(\_SB.PCI0.RP07)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP07)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 8),LEqual(TBS1, 8))))
{
  Scope(\_SB.PCI0.RP08)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP08)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 9),LEqual(TBS1, 9))))
{
  Scope(\_SB.PCI0.RP09)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP09)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 10),LEqual(TBS1, 10))))
{
  Scope(\_SB.PCI0.RP10)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP10)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 11),LEqual(TBS1, 11))))
{
  Scope(\_SB.PCI0.RP11)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP11)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 12),LEqual(TBS1, 12))))
{
  Scope(\_SB.PCI0.RP12)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP12)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 13),LEqual(TBS1, 13))))
{
  Scope(\_SB.PCI0.RP13)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP13)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 14),LEqual(TBS1, 14))))
{
  Scope(\_SB.PCI0.RP14)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP14)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 15),LEqual(TBS1, 15))))
{
  Scope(\_SB.PCI0.RP15)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP15)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 16),LEqual(TBS1, 16))))
{
  Scope(\_SB.PCI0.RP16)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP16)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 17),LEqual(TBS1, 17))))
{
  Scope(\_SB.PCI0.RP17)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP17)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 18),LEqual(TBS1, 18))))
{
  Scope(\_SB.PCI0.RP18)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP18)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 19),LEqual(TBS1, 19))))
{
  Scope(\_SB.PCI0.RP19)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP19)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 20),LEqual(TBS1, 20))))
{
  Scope(\_SB.PCI0.RP20)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.RP20)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 21),LEqual(TBS1, 21))))
{
  Scope(\_SB.PCI0.PEG0)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.PEG0)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 22),LEqual(TBS1, 22))))
{
  Scope(\_SB.PCI0.PEG1)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.PEG1)
}

If(LAnd(LEqual(TBTS, 1),LOr(LEqual(TBSE, 23),LEqual(TBS1, 23))))
{
  Scope(\_SB.PCI0.PEG2)
  {
    Device(HRUS)// Host router Upstream port
    {
      Name(_ADR, 0x00000000)

      Method(_RMV)
      {
        Return(TARS)
      } // end _RMV
    }
  }//End of Scope(\_SB.PCI0.PEG2)
}
#endif
// AMI_OVERRIDE_END - For the structure of AMI asl code scope.

Scope(\_SB)
{
    //
    // Name: R008
    // Description: Function to read a Byte from given Memory address
    // Input: Arg0 -> Memory address
    // Return: Byte data read from Memory address
    //
    Method(R008,1,Serialized)
    {
      ADBG("R008")

      Store(Arg0, Local7)
      OperationRegion(MEM0, SystemMemory, Local7, 1)
      Field(MEM0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 8
      }

      Return(TEMP)
    } // End of Method(R008,1,Serialized)

    //
    // Name: W008
    // Description: Function to write a Byte into given Memory address
    // Input: Arg0 -> Memory address
    //        Arg1 -> Data
    // Return: Nothing
    //
    Method(W008,2,Serialized)
    {
      ADBG("W008")

      Store(Arg0, Local7)
      OperationRegion(MEM0, SystemMemory, Local7, 1)
      Field(MEM0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 8
      }

      Store(Arg1,TEMP)
    } // End of Method(W008,2,Serialized)

    //
    // Name: R016
    // Description: Function to read a Word from given Memory address
    // Input: Arg0 -> Memory address
    // Return: Word data read from Memory address
    //
    Method(R016,1,Serialized)
    {
      ADBG("R016")

      Store(Arg0, Local7)
      OperationRegion(MEM0, SystemMemory, Local7, 2)
      Field(MEM0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 16
      }

      Return(TEMP)
    } // End of Method(R016,1,Serialized)

    //
    // Name: W016
    // Description: Function to write a Word into given Memory address
    // Input: Arg0 -> Memory address
    //        Arg1 -> Data
    // Return: Nothing
    //
    Method(W016,2,Serialized)
    {
      ADBG("W016")

      Store(Arg0, Local7)
      OperationRegion(MEM0, SystemMemory, Local7, 2)
      Field(MEM0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 16
      }

      Store(Arg1,TEMP)
    } // End of Method(W016,2,Serialized)

    //
    // Name: R032
    // Description: Function to read a Dword from given Memory address
    // Input: Arg0 -> Memory address
    // Return: Dword data read from Memory address
    //
    Method(R032,1,Serialized)
    {
      ADBG("R032")

      Store(Arg0, Local7)
      OperationRegion(MEM0, SystemMemory, Local7, 4)
      Field(MEM0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 32
      }

      Return(TEMP)
    } // End of Method(R032,1,Serialized)

    //
    // Name: W032
    // Description: Function to write a Dword into given Memory address
    // Input: Arg0 -> Memory address
    //        Arg1 -> Data
    // Return: Nothing
    //
    Method(W032,2,Serialized)
    {
      ADBG("W032")

      Store(Arg0, Local7)
      OperationRegion(MEM0, SystemMemory, Local7, 4)
      Field(MEM0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 32
      }

      Store(Arg1,TEMP)
    } // End of Method(W032,2,Serialized)

    //
    // Name: PERB
    // Description: Function to read a Byte from PCIE-MMIO
    // Input: Arg0 -> PCIE base address
    //        Arg1 -> Bus
    //        Arg2 -> Device
    //        Arg3 -> Function
    //        Arg4 -> Register offset
    // Return: Byte data read from PCIE-MMIO
    //
    Method(PERB,5,Serialized)
    {
      ADBG("PERB")

      Store(Arg0, Local7)
      Or(Local7, ShiftLeft(Arg1, 20), Local7)
      Or(Local7, ShiftLeft(Arg2, 15), Local7)
      Or(Local7, ShiftLeft(Arg3, 12), Local7)
      Or(Local7, Arg4, Local7)

      OperationRegion(PCI0, SystemMemory, Local7, 1)
      Field(PCI0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 8
      }

      Return(TEMP)
    } // End of Method(PERB,5,Serialized)

    //
    // Name: PEWB
    // Description: Function to write a Byte into PCIE-MMIO
    // Input: Arg0 -> PCIE base address
    //        Arg1 -> Bus
    //        Arg2 -> Device
    //        Arg3 -> Function
    //        Arg4 -> Register offset
    //        Arg5 -> Data
    // Return: Nothing
    //
    Method(PEWB,6,Serialized)
    {
      ADBG("PEWB")

      Store(Arg0, Local7)
      Or(Local7, ShiftLeft(Arg1, 20), Local7)
      Or(Local7, ShiftLeft(Arg2, 15), Local7)
      Or(Local7, ShiftLeft(Arg3, 12), Local7)
      Or(Local7, Arg4, Local7)

      OperationRegion(PCI0, SystemMemory, Local7, 1)
      Field(PCI0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 8
      }

      Store(Arg5,TEMP)
    } // End of Method(PEWB,6,Serialized)

    //
    // Name: PERW
    // Description: Function to read a Word from PCIE-MMIO
    // Input: Arg0 -> PCIE base address
    //        Arg1 -> Bus
    //        Arg2 -> Device
    //        Arg3 -> Function
    //        Arg4 -> Register offset
    // Return: Word data read from PCIE-MMIO
    //
    Method(PERW,5,Serialized)
    {
      ADBG("PERW")

      Store(Arg0, Local7)
      Or(Local7, ShiftLeft(Arg1, 20), Local7)
      Or(Local7, ShiftLeft(Arg2, 15), Local7)
      Or(Local7, ShiftLeft(Arg3, 12), Local7)
      Or(Local7, Arg4, Local7)

      OperationRegion(PCI0, SystemMemory, Local7, 2)
      Field(PCI0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 16
      }

      Return(TEMP)
    } // End of Method(PERW,5,Serialized)

    //
    // Name: PEWW
    // Description: Function to write a Word into PCIE-MMIO
    // Input: Arg0 -> PCIE base address
    //        Arg1 -> Bus
    //        Arg2 -> Device
    //        Arg3 -> Function
    //        Arg4 -> Register offset
    //        Arg5 -> Data
    // Return: Nothing
    //
    Method(PEWW,6,Serialized)
    {
      ADBG("PEWW")

      Store(Arg0, Local7)
      Or(Local7, ShiftLeft(Arg1, 20), Local7)
      Or(Local7, ShiftLeft(Arg2, 15), Local7)
      Or(Local7, ShiftLeft(Arg3, 12), Local7)
      Or(Local7, Arg4, Local7)

      OperationRegion(PCI0, SystemMemory, Local7, 2)
      Field(PCI0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 16
      }

      Store(Arg5,TEMP)
    } // End of Method(PEWW,6,Serialized)

    //
    // Name: PERD
    // Description: Function to read a Dword from PCIE-MMIO
    // Input: Arg0 -> PCIE base address
    //        Arg1 -> Bus
    //        Arg2 -> Device
    //        Arg3 -> Function
    //        Arg4 -> Register offset
    // Return: Dword data read from PCIE-MMIO
    //
    Method(PERD,5,Serialized)
    {
      ADBG("PERD")

      Store(Arg0, Local7)
      Or(Local7, ShiftLeft(Arg1, 20), Local7)
      Or(Local7, ShiftLeft(Arg2, 15), Local7)
      Or(Local7, ShiftLeft(Arg3, 12), Local7)
      Or(Local7, Arg4, Local7)

      OperationRegion(PCI0, SystemMemory, Local7, 4)
      Field(PCI0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 32
      }

      Return(TEMP)
    } // End of Method(PERD,5,Serialized)

    //
    // Name: PEWD
    // Description: Function to write a Dword into PCIE-MMIO
    // Input: Arg0 -> PCIE base address
    //        Arg1 -> Bus
    //        Arg2 -> Device
    //        Arg3 -> Function
    //        Arg4 -> Register offset
    //        Arg5 -> Data
    // Return: Nothing
    //
    Method(PEWD,6,Serialized)
    {
      ADBG("PEWD")

      Store(Arg0, Local7)
      Or(Local7, ShiftLeft(Arg1, 20), Local7)
      Or(Local7, ShiftLeft(Arg2, 15), Local7)
      Or(Local7, ShiftLeft(Arg3, 12), Local7)
      Or(Local7, Arg4, Local7)

      OperationRegion(PCI0, SystemMemory, Local7, 4)
      Field(PCI0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 32
      }

      Store(Arg5,TEMP)
    } // End of Method(PEWD,6,Serialized)

    //
    // Name: STDC
    // Description: Function to get Standard Capability Register Offset
    // Input: Arg0 -> PCIE base address
    //        Arg1 -> Bus
    //        Arg2 -> Device
    //        Arg3 -> Function
    //        Arg4 -> Capability ID
    // Return: Capability Register Offset data
    //
    Method(STDC,5,Serialized)
    {
      ADBG("STDC")

      //Check for Referenced device is present or not
      Store(PERW(Arg0, Arg1, Arg2, Arg3, 0x00), Local7) //Vendor ID register
      If(LEqual(Local7, 0xFFFF))
      {
        ADBG("Referenced device is not present")
        Return(0)
      }

      Store(PERW(Arg0, Arg1, Arg2, Arg3, 0x06), Local0) //Device Status register
      If (LEqual(And(Local0, 16), 0)) //Bit4 - Capabilities List
      {
        //No Capabilities linked list is available
        ADBG("No Capabilities linked list is available")
        Return(0)
      }

      //Local1 is for storing CapabilityID
      //Local2 is for storing CapabilityPtr
      Store(PERB(Arg0, Arg1, Arg2, Arg3, 0x34), Local2) //CapabilityPtr

      While(1)
      {
        And(Local2, 0xFC, Local2) //Each capability must be DWORD aligned

        If(LEqual(Local2, 0)) //A pointer value of 00h is used to indicate the last capability in the list
        {
          ADBG("Capability ID is not found")
          Return(0)
        }

        Store(PERB(Arg0, Arg1, Arg2, Arg3, Local2), Local1) //CapabilityID

        If(LEqual(Arg4, Local1)) //CapabilityID match
        {
          ADBG("Capability ID is found")
          ADBG("Capability Offset : ")
          ADBG(Local2)
          Return(Local2)
        }
        Store(PERB(Arg0, Arg1, Arg2, Arg3, Add(Local2, 1)), Local2) //CapabilityPtr
        Return(0)
      }
    } // End of Method(STDC,5,Serialized)

} // End Scope(\_SB)
  //
  // Name: RDCM
  // Description: Function to read a Byte from CMOS
  // Input: Arg0 -> CMOS offset to read
  // Return: Byte data read from CMOS
  //
  Method(RDCM, 1, Serialized)
  {
    ADBG("RDCM")
    ADBG("CMOS Offset")
    ADBG(Arg0)

    OperationRegion(CMOS,SystemIO,0x70,4)
    Field(CMOS,AnyAcc,NoLock,Preserve)
    {
      LIND, 8, //Cmos Low Index
      LDAT, 8, //Cmos Low Data
      HIND, 8, //Cmos High Index
      HDAT, 8, //Cmos High Data
    }

    If(LLessEqual(Arg0, 127))
    {
      Store(Arg0, LIND)
      Store(LDAT, Local0)
    }
    ElseIf(LLessEqual(Arg0, 255))
    {
      Store(Arg0, HIND)
      Store(HDAT, Local0)
    }

    ADBG("CMOS Data")
    ADBG(Local0)

    Return(Local0)
  }

  //
  // Name: WRCM
  // Description: Function to write a Byte into CMOS
  // [Note: The checksum update will not be done by this method]
  // Input: Arg0 -> CMOS offset to write
  //        Arg1 -> Data
  // Return: Nothing
  //
  Method(WRCM, 2, Serialized)
  {
    ADBG("WRCM")
    ADBG("CMOS Offset")
    ADBG(Arg0)
    ADBG("CMOS Data")
    ADBG(Arg1)

    OperationRegion(CMOS,SystemIO,0x70,4)
    Field(CMOS,AnyAcc,NoLock,Preserve)
    {
      LIND, 8, //Cmos Low Index
      LDAT, 8, //Cmos Low Data
      HIND, 8, //Cmos High Index
      HDAT, 8, //Cmos High Data
    }

    If(LLessEqual(Arg0, 127))
    {
      Store(Arg0, LIND)
      Store(Arg1, LDAT)
    }
    ElseIf(LLessEqual(Arg0, 255))
    {
      Store(Arg0, HIND)
      Store(Arg1, HDAT)
    }
  }

  Method (TBON, 0, Serialized) {
    // TBT On process before entering Sx state.
    Store(1, TRDO)
    Switch (ToInteger(\TBSE)) { // TBT Root port Selector
      Case (1) {
        If (CondRefOf(\_SB.PCI0.RP01.PON)) {
          \_SB.PCI0.RP01.PON()
        }
      }
      Case (2) {
        If (CondRefOf(\_SB.PCI0.RP02.PON)) {
          \_SB.PCI0.RP02.PON()
        }
      }
      Case (3) {
        If (CondRefOf(\_SB.PCI0.RP03.PON)) {
          \_SB.PCI0.RP03.PON()
        }
      }
      Case (4) {
        If (CondRefOf(\_SB.PCI0.RP04.PON)) {
          \_SB.PCI0.RP04.PON()
        }
      }
      Case (5) {
        If (CondRefOf(\_SB.PCI0.RP05.PON)) {
          \_SB.PCI0.RP05.PON()
        }
      }
      Case (6) {
        If (CondRefOf(\_SB.PCI0.RP06.PON)) {
          \_SB.PCI0.RP06.PON()
        }
      }
      Case (7) {
        If (CondRefOf(\_SB.PCI0.RP07.PON)) {
          \_SB.PCI0.RP07.PON()
        }
      }
      Case (8) {
        If (CondRefOf(\_SB.PCI0.RP08.PON)) {
          \_SB.PCI0.RP08.PON()
        }
      }
      Case (9) {
        If (CondRefOf(\_SB.PCI0.RP09.PON)) {
          \_SB.PCI0.RP09.PON()
        }
      }
      Case (10) {
        If (CondRefOf(\_SB.PCI0.RP10.PON)) {
          \_SB.PCI0.RP10.PON()
        }
      }
      Case (11) {
        If (CondRefOf(\_SB.PCI0.RP11.PON)) {
          \_SB.PCI0.RP11.PON()
        }
      }
      Case (12) {
        If (CondRefOf(\_SB.PCI0.RP12.PON)) {
          \_SB.PCI0.RP12.PON()
        }
      }
      Case (13) {
        If (CondRefOf(\_SB.PCI0.RP13.PON)) {
          \_SB.PCI0.RP13.PON()
        }
      }
      Case (14) {
        If (CondRefOf(\_SB.PCI0.RP14.PON)) {
          \_SB.PCI0.RP14.PON()
        }
      }
      Case (15) {
        If (CondRefOf(\_SB.PCI0.RP15.PON)) {
          \_SB.PCI0.RP15.PON()
        }
      }
      Case (16) {
        If (CondRefOf(\_SB.PCI0.RP16.PON)) {
          \_SB.PCI0.RP16.PON()
        }
      }
      Case (17) {
        If (CondRefOf(\_SB.PCI0.RP17.PON)) {
          \_SB.PCI0.RP17.PON()
        }
      }
      Case (18) {
        If (CondRefOf(\_SB.PCI0.RP18.PON)) {
          \_SB.PCI0.RP18.PON()
        }
      }
      Case (19) {
        If (CondRefOf(\_SB.PCI0.RP19.PON)) {
          \_SB.PCI0.RP19.PON()
        }
      }
      Case (20) {
        If (CondRefOf(\_SB.PCI0.RP20.PON)) {
          \_SB.PCI0.RP20.PON()
        }
      }
      Case (21) {
// Placeholder
// Currently on KBL H/S, there's no TBT RTD3 support for PEG slot.
//        If (CondRefOf(\_SB.PCI0.PEG0.PG00._ON)) {
//          \_SB.PCI0.PEG0.PG00._ON()
//        }
      }
      Case (22) {
// Placeholder
// Currently on KBL H/S, there's no TBT RTD3 support for PEG slot.
//        If (CondRefOf(\_SB.PCI0.PEG1.PG01._ON)) {
//          \_SB.PCI0.PEG1.PG01._ON()
//        }
      }
      Case (23)
      {
// Placeholder
// Currently on KBL H/S, there's no TBT RTD3 support for PEG slot.
//        If (CondRefOf(\_SB.PCI0.PEG2.PG02._ON)) {
//          \_SB.PCI0.PEG2.PG02._ON()
//        }
      }
    }//Switch(ToInteger(TBSE)) // TBT Selector
    Store(0, TRDO)
  } // End of TBON
#endif
// AMI_OVERRIDE_END - It is for Thunderbolt, AMI thunderbolt module has taken care it already
// -------------------------------------------------------
//        TBT ACPI implementation - End
// -------------------------------------------------------
//
// AdvancedFeaturesEnd
//