/**@file

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



// AMI_OVERRIDE_PCH0007_START >>>
#include <PchAccess.h>
// AMI_OVERRIDE_PCH0007_END   <<<
  OperationRegion(PXCS,PCI_Config,0x00,0x480)
  Field(PXCS,AnyAcc, NoLock, Preserve)
  {
    Offset(0),
    VDID, 32,
// AMI_OVERRIDE_PCH0007_START >>>
    Offset(0x19), // BNUM - Bus Number Register
    SBNR, 8,      // Secondary Bus Number
// AMI_OVERRIDE_PCH0007_END   <<<
    Offset(0x50), // LCTL - Link Control Register
    L0SE, 1,      // 0, L0s Entry Enabled
    , 3,
    LDIS, 1,
    , 3,
    Offset(0x52), // LSTS - Link Status Register
    , 13,
    LASX, 1,      // 0, Link Active Status
// AMI_OVERRIDE_PCH0007_START >>>
    Offset(0x54), // SLCAP - Slot Capabilities Register
    , 6,
    HPCE, 1,      // 6, Hot Plug Capable
// AMI_OVERRIDE_PCH0007_END   <<<
    Offset(0x5A), // SLSTS[7:0] - Slot Status Register
    ABPX, 1,      // 0, Attention Button Pressed
    , 2,
    PDCX, 1,      // 3, Presence Detect Changed
    , 2,
    PDSX, 1,      // 6, Presence Detect State
    , 1,
    Offset(0x60), // RSTS - Root Status Register
    , 16,
    PSPX, 1,      // 16,  PME Status
// AMI_OVERRIDE_PCH0008_START >>>
    PMEP, 1,      // 17,  PME Pending
// AMI_OVERRIDE_PCH0008_END   <<<
    Offset(0xA4),
    D3HT, 2,      // Power State
    Offset(R_PCH_PCIE_MPC), // 0xD8, MPC - Miscellaneous Port Configuration Register
    , 30,
    HPEX, 1,      // 30,  Hot Plug SCI Enable
    PMEX, 1,      // 31,  Power Management SCI Enable
    #ifndef MINTREE_FLAG
    Offset(R_PCH_PCIE_RPPGEN), // 0xE2, RPPGEN - Root Port Power Gating Enable
    , 2,
    L23E, 1,      // 2,   L23_Rdy Entry Request (L23ER)
    L23R, 1,       // 3,   L23_Rdy to Detect Transition (L23R2DT)
    #endif //MINTREE_FLAG
    Offset(R_PCH_PCIE_PCIEDBG), // 0x324
    , 3,
    LEDM, 1,       // PCIEDBG.DMIL1EDM
    #ifndef MINTREE_FLAG
    Offset(R_PCH_PCIE_PCIEPMECTL), // 0x420, PCIEPMECTL (PCIe PM Extension Control)
    , 30,
    DPGE, 1,       // PCIEPMECTL[30]: Disabled, Detect and L23_Rdy State PHY Lane Power Gating Enable (DLSULPPGE):
    #endif //MINTREE_FLAG
  }
  Field(PXCS,AnyAcc, NoLock, WriteAsZeros)
  {
    Offset(R_PCH_PCIE_SMSCS), // 0xDC, SMSCS - SMI/SCI Status Register
    , 30,
    HPSX, 1,      // 30,  Hot Plug SCI Status
    PMSX, 1       // 31,  Power Management SCI Status
  }


  Name(LTRV, Package(){0,0,0,0})

  //
  // _DSM Device Specific Method
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index (0 = Return Supported Functions)
  // Arg3: Package Parameters
  Method(_DSM, 4, Serialized) {
    //
    // Switch based on which unique function identifier was passed in
    //
    If (LEqual(Arg0, ToUUID ("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))) {
      //
      // _DSM Definitions for Latency Tolerance Reporting
      //
      // Arguments:
      // Arg0: UUID: E5C937D0-3553-4d7a-9117-EA4D19C3434D
      // Arg1: Revision ID: 2
      // Arg2: Function Index: 1, 4 or 6
      // Arg3: Empty Package
      //
      // Return:
      // A Package of four integers corresponding with the LTR encoding defined
      // in the PCI Express Base Specification, as follows:
      // Integer 0: Maximum Snoop Latency Scale
      // Integer 1: Maximum Snoop Latency Value
      // Integer 2: Maximum No-Snoop Latency Scale
      // Integer 3: Maximum No-Snoop Latency Value
      // These values correspond directly to the LTR Extended Capability Structure
      // fields described in the PCI Express Base Specification.
      //
      //
      // Switch by function index
      //
      Switch(ToInteger(Arg2)) {
        //
        // Function Index:0
        // Standard query - A bitmask of functions supported
        //
        Case (0) {
          Name(OPTS,Buffer(2){0,0})
          CreateBitField(OPTS,0,FUN0)
          CreateBitField(OPTS,4,FUN4)
          CreateBitField(OPTS,6,FUN6)
          CreateBitField(OPTS,8,FUN8)
          CreateBitField(OPTS,9,FUN9)

          if (LGreaterEqual(Arg1, 2)){ // test Arg1 for Revision ID: 2
            Store(1,FUN0)
            if (LTRZ){
              Store(1,Fun6)
            }
            if (OBFZ){
              Store(1,Fun4)
            }
            if(LEqual(ECR1,1)){
              if (LGreaterEqual(Arg1, 3)){ // test Arg1 for Revision ID: 3
                Store(1,Fun8)
                Store(1,Fun9)
              }
            }
          }
          Return (OPTS)
        }
        //
        // Function Index: 4
        //
        Case(4) {
          if (LGreaterEqual(Arg1, 2)){ // test Arg1 for Revision ID: 2
            if (OBFZ){
              Return (Buffer () {0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0}) // OBFF capable, offset 4[08h]
            } else {
              Return (Buffer () {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0})
            }
          }
        }
        //
        // Function Index: 5
        // PCI FIRMWARE SPECIFICATION, REV. 3.2
        // 4.6.5. _DSM for Ignoring PCI Boot Configurations
        //
        Case(5) {
          If (LEqual(Arg1, 1)){ // test Arg1 for Revision ID: 1
            return(1)
          }
        }
        //
        // Function Index: 6
        // LTR Extended Capability Structure
        //
        Case(6) {
          if (LGreaterEqual(Arg1, 2)){ // test Arg1 for Revision ID: 2
            if (LTRZ){
              if (LOr(LEqual(LMSL, 0),LEqual(LNSL, 0)))
              {
                if (LEqual (PCHS, SPTH)) {
                  Store (0x0846, LMSL)
                  Store (0x0846, LNSL)
                } elseif (LEqual (PCHS, SPTL)) {
                  Store (0x1003, LMSL)
                  Store (0x1003, LNSL)
                }
              }
              Store(And(ShiftRight(LMSL,10),7), Index(LTRV, 0))
              Store(And(LMSL,0x3FF), Index(LTRV, 1))
              Store(And(ShiftRight(LNSL,10),7), Index(LTRV, 2))
              Store(And(LNSL,0x3FF), Index(LTRV, 3))

              Return (LTRV)
            } else {
              Return (0)
            }
          }
        }
        Case(8) { //ECR ACPI additions for FW latency optimizations, DSM for Avoiding Power-On Reset Delay Duplication on Sx Resume
          if(LEqual(ECR1,1)){
            if (LGreaterEqual(Arg1, 3)) { // test Arg1 for Revision ID: 3
              return (1)
            }
          }
        }
        Case(9) { //ECR ACPI additions for FW latency optimizations, DSM for Specifying Device Readiness Durations
          if(LEqual(ECR1,1)){
            if (LGreaterEqual(Arg1, 3)) { // test Arg1 for Revision ID: 3
              return(Package(5){50000,Ones,Ones,50000,Ones})
            }
          }
        }
      } // End of switch(Arg2)
    } // End of if
    return (Buffer() {0x00})
  } // End of _DSM

  Device(PXSX)
  {
    Name(_ADR, 0x00000000)

    // NOTE:  Any PCIE Hot-Plug dependency for this port is
    // specific to the CRB.  Please modify the code based on
    // your platform requirements.

    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }  // can wakeup from S4 state
// AMI_OVERRIDE_PCH0007_START >>>
    Method(_RMV, 0, NotSerialized){
#if defined ASL_Thunderbolt_SUPPORT && ASL_Thunderbolt_SUPPORT == 1
      If(LAnd(LEqual(TBTS, 1),LEqual(SBNR, TBUS)))
      {
        Return(TARS)
      }Else{
        Return(HPCE)    //0:device cannot be removed    1:device can be removed
      }
#else
      Return(HPCE)    //0:device cannot be removed    1:device can be removed
#endif
    }
// AMI_OVERRIDE_PCH0007_END   <<<
// AMI_OVERRIDE_TBT0019_START >>>
#if defined ASL_Thunderbolt_SUPPORT && ASL_Thunderbolt_SUPPORT == 1
#if defined ASL_TBT_DEEPSX_SUPPORT && ASL_TBT_DEEPSX_SUPPORT == 1
    // AR BIOS Implementation guide rev.0.8 - 2.1.2.5.1 ACPI requirements
    Method(_PS0){
    }
    Method(_PS3){
      Add(MMTB(), 0x548, Local0)
      OperationRegion(PXVD,SystemMemory,Local0,0x08) 
      Field(PXVD,DWordAcc, NoLock, Preserve)
      {
        TB2P, 32,
        P2TB, 32
      }
      If(LAnd(LEqual(TBTS, 1),LEqual(SBNR, TBUS)))
      {
        ADBG("sending SX_START")
        Store(0x1D,P2TB)
      }
    }
#endif
#endif
// AMI_OVERRIDE_TBT0019_END   <<<
  }

  //
  // PCI_EXP_STS Handler for PCIE Root Port
  //
  Method(HPME,0,Serialized) {
    If(LAnd(LNotEqual(VDID,0xFFFFFFFF), LEqual(PMSX,1))) { //if port exists and has PME SCI Status set...
      Notify (PXSX, 0x2) //notify child device; this will cause its driver to clear PME_Status from device
      Store(1,PMSX) // clear rootport's PME SCI status
      Store(1,PSPX) // consume one pending PME notification to prevent it from blocking the queue
    }
  }

