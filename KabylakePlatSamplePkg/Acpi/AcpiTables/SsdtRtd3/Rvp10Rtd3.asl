/** @file
  ACPI RTD3 SSDT table for SKL RVP3

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

#define GPIO_NO_CONTROL 0
#define GPIO_INTERNAL 1
#define GPIO_EXTERNAL 2
#define EXPANDER0 0
#define EXPENDER1 1
#define INDEX_CONTROL 0
#define INDEX_EXPANDER_NR 1
#define INDEX_PIN_NR 2
#define INDEX_POLARITY 3

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "AcpiRef",
    "RV10Rtd3",
    0x1000
    )
{
External(RTD3, IntObj)
External(S0ID)

// AMI_OVERRIDE_START - Fix Linux build error.
//Include ("Rtd3Common.asl")
Include ("RTD3Common.asl")
// AMI_OVERRIDE_END - Fix Linux build error.

External(PEPC)

External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)

External(\_SB.PCI0.SAT0.VOL0, DeviceObj)
External(\_SB.PCI0.SAT0.VOL1, DeviceObj)
External(\_SB.PCI0.SAT0.VOL2, DeviceObj)
External(\_SB.PCI0.SAT0.NVM1, DeviceObj)
External(\_SB.PCI0.SAT0.NVM2, DeviceObj)
External(\_SB.PCI0.SAT0.NVM3, DeviceObj)
// AMI_OVERRIDE_START - Fixed build errors with the latest ASL compiler
// The following external declarations have to be removed, cause External declaration cannot be used if method is defined in the same file.
//External(\_SB.PCI0.SAT0.PON, MethodObj)
//External(\_SB.PCI0.SAT0.POFF, MethodObj)
//External(\_SB.PCI0.SAT0.PSTA, MethodObj)
// AMI_OVERRIDE_END - Fixed build errors with the latest ASL compier
  Scope(\_SB.PCI0.SAT0) {
    Method(PSTA,1,Serialized) {  //PowerCheck, Arg0=PWRG package
      If(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)), GPIO_NO_CONTROL)) { Return(1) } // no power control means it's always enabled
      If(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)), GPIO_INTERNAL)) {
        if(LEqual(\_SB.GGOV(DeRefOf(Index(Arg0, INDEX_PIN_NR))),DeRefOf(Index(Arg0, INDEX_POLARITY)))) {
          Return(0x01)
        }
        Return(0x00)
      }
      If(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)),GPIO_EXTERNAL))  { // IOEX mode
        If(LEqual(\_SB.PCI0.GEXP.GEPS(DeRefOf(Index(Arg0, INDEX_EXPANDER_NR)),DeRefOf(Index(Arg0, INDEX_PIN_NR))),DeRefOf(Index(Arg0, INDEX_POLARITY)))) {
          Return(0x01)
        }
        Return(0x00)
      }
      Return (0)
    }
    Method(PON,1,Serialized) {  //PowerOn, Arg0=PWRG package
      if(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)),GPIO_NO_CONTROL)) { return() }
      if(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)),GPIO_INTERNAL)) {
        \_SB.SGOV(DeRefOf(Index(Arg0, INDEX_PIN_NR)),DeRefOf(Index(Arg0, INDEX_POLARITY)))
      }
      if(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)),GPIO_EXTERNAL))  {
        \_SB.PCI0.GEXP.SGEP(DeRefOf(Index(Arg0, INDEX_EXPANDER_NR)),DeRefOf(Index(Arg0, INDEX_PIN_NR)),DeRefOf(Index(Arg0, INDEX_POLARITY)))
      }
    }
    Method(POFF,1,Serialized) {  //PowerOff, Arg0=PWRG package
      If(LEqual(S0ID,0)) {Return()}
      If(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)),0)) { Return() }
      if(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)),1)) {
        \_SB.SGOV(DeRefOf(Index(Arg0, INDEX_PIN_NR)),Xor(DeRefOf(Index(Arg0, INDEX_POLARITY)),1))
      }
      if(LEqual(DeRefOf(Index(Arg0, INDEX_CONTROL)),2))  {
        \_SB.PCI0.GEXP.SGEP(DeRefOf(Index(Arg0, INDEX_EXPANDER_NR)),DeRefOf(Index(Arg0, INDEX_PIN_NR)),Xor(DeRefOf(Index(Arg0, INDEX_POLARITY)),1))
      }
    }
    Method(ROFF,1,Serialized) {  //RegisterOff, Arg0=PBAR
      If(LEqual(S0ID,0)) {Return()}
      OperationRegion(PSTS, SystemMemory, Add(\_SB.PCI0.SAT0.MBR6, Arg0), 0x18)
      Field(PSTS, DWordAcc, NoLock, Preserve) {
        Offset(0x0),
        CMST, 1,  //PxCMD.ST
        CSUD, 1,  //PxCMD.SUD
        , 2,
        CFRE, 1,  //PxCMD.FRE
        Offset(0x10),
        SDET, 4,  //PxSSTS.DET
        Offset(0x14),
        CDET, 4   //PxSCTL.DET
      }
      If(LOr(LEqual(SDET, 1), LEqual(SDET, 3))) { /// Execute offline flow only if Device detected and Phy not offline
        Store(0, CMST)                   ///- Clear ST (PxCMD.ST)
        Store(0, CFRE)                   ///- Clear FRE
        Store(0, CSUD)                   ///- Clear SUD (PxCMD.SUD)
        Store(4, CDET)                   ///- Set DET to 4 (PxSCTL.DET)
        Sleep(16)
        While(LNotEqual(SDET, 4)){ Sleep(16) } ///- Wait until PxSSTS.DET == 4
      }
    }
  }//end scope



  Scope(\_SB.PCI0.SAT0) {
    Method(_PS0,0,Serialized){}
    Method(_PS3,0,Serialized){}

    /// Define SATA PCI Config OperationRegion
    OperationRegion(SMIO,PCI_Config,0x24,4)
    Field(SMIO,AnyAcc, NoLock, Preserve) {
      Offset(0x00), ///- SATA MABR6
      MBR6, 32,     ///- SATA ABAR
    }

    OperationRegion(MAPR,PCI_Config,0x90,2)
    Field(MAPR,AnyAcc, NoLock, Preserve) {
      Offset(0x00), // MAP -Address Map Register (SATA-D31:F2)
      , 6,          // [5:0]
      SMSR, 2       // [7:6]SATA Mode Select (SMS), 10b = RAID mode
    }

    OperationRegion(PCIR, PCI_Config, 0x00, 0x10)
    Field(PCIR,DWordAcc, NoLock, Preserve)
    {
      Offset(0x0),
      SVID, 16,
      SDID, 16,
      Offset(0x0A),
      SUBC,   8  // Link Control register
    }


  //SATA lane 0 - cable from vertical header J4F3LB / option: cable from double port J4H5LB. Both without power control.
  //SATA lane 1 - ZPODD J4J3LB / option: cable from double port J4H5LB. 1st controlled by GPIO G1
  //SATA lanes 2,3 - SATAe cable, no power control
  //SATA lanes 4,5 - hijacked by PCIE

    Scope(PRT0) {
      Name(PORT, 0)
      Name(PBAR, 0x118)       // P0 command port = ABAR+0x118
      Name(PWRG, Package() {GPIO_NO_CONTROL,0,0,1}) // power pin = (IOEX, 2)
      Name(_S0W, 4)// (D3hot)
      Name(_PR0, Package(){SPPR})
      Name(_PR3, Package(){SPPR})
      PowerResource(SPPR, 0, 0) {
        Method(_STA){ Return(PSTA(PWRG)) }
        Method(_ON) { PON(PWRG) } // add timer support
        Method(_OFF){ ROFF(PBAR);POFF(PWRG); } // add timer support
      }
    }

    Scope(PRT1) {
      Name(PORT, 1)
      Name(PBAR, 0x198)       // P1 command port = ABAR+0x198
      Name(PWRG, Package() {GPIO_NO_CONTROL,0,0,1}) // power pin = (IOEX, 2) //GPIO_INTERNAL,0,GPIO_SKL_H_GPP_G1,1})
      Name(_PR0, Package(){SPPR})
      Name(_PR3, Package(){SPPR})
      Name(_S0W, 4)// (D3hot)
      PowerResource(SPPR, 0, 0) {
        Method(_STA){ Return(PSTA(PWRG)) }
        Method(_ON) { PON(PWRG) } // add timer support
        Method(_OFF){ ROFF(PBAR);POFF(PWRG); } // add timer support
      }
    }

    Scope(PRT2) {
      Name(PORT, 2)
      Name(PBAR, 0x218)       // P2 command port = ABAR+0x218
      Name(PWRG, Package() {GPIO_NO_CONTROL,0,0,1})
      Name(_PR0, Package(){SPPR})
      Name(_PR3, Package(){SPPR})
      Name(_S0W, 4)// (D0)
      PowerResource(SPPR, 0, 0) {
        Method(_STA){ Return(PSTA(PWRG)) }
        Method(_ON) { PON(PWRG) } // add timer support
        Method(_OFF){ ROFF(PBAR);POFF(PWRG); } // add timer support
      }
    }

    Scope(PRT3) {
      Name(PORT, 3)
      Name(PBAR, 0x298)       // P3 command port = ABAR+unknown
      Name(PWRG, Package() {GPIO_NO_CONTROL,0,0,1})
      Name(_PR0, Package(){SPPR})
      Name(_PR3, Package(){SPPR})
      Name(_S0W, 4)// (D0)
      PowerResource(SPPR, 0, 0) {
        Method(_STA){ Return(PSTA(PWRG)) }
        Method(_ON) { PON(PWRG) } // add timer support
        Method(_OFF){ ROFF(PBAR);POFF(PWRG); } // add timer support
      }
    }

    Scope(PRT4) {
      Name(PORT, 4)
      Name(PBAR, 0x318)       // P3 command port = ABAR+unknown
      Name(PWRG, Package() {GPIO_NO_CONTROL,0,0,1})
      Name(_PR0, Package(){SPPR})
      Name(_PR3, Package(){SPPR})
      Name(_S0W, 4)// (D0)
      PowerResource(SPPR, 0, 0) {
        Method(_STA){ Return(PSTA(PWRG)) }
        Method(_ON) { PON(PWRG) } // add timer support
        Method(_OFF){ ROFF(PBAR);POFF(PWRG); } // add timer support
      }
    }

    Include ("RTD3RstRaid.asl")

      // CPON  : Method to turn ON ports per PMAP bitmap status
      //  Arg0 : PMAP - Port Bitmap Status
      Method(CPON,1,Serialized,,,{IntObj})
      {
          //Rvp10 platform does not support D3Cold (no power toggling options)
      }

      // CPOF  : Method to turn OFF ports per PMAP bitmap status
      //  Arg0 : PMAP - Port Bitmap Status
      Method(CPOF,1,Serialized,,,{IntObj})
      {
          //Rvp10 platform does not support D3Cold (no power toggling options)
      }


    ///
    /// _DSM Device Specific Method supporting AHCI DEVSLP
    ///
    /// Arg0: UUID Unique function identifier \n
    /// Arg1: Integer Revision Level \n
    /// Arg2: Integer Function Index \n
    /// Arg3: Package Parameters \n
    ///
    /// Variables:
    Name(DRV, 0)  /// Storage for _DSM Arg3 parameter 0
    Name(PWR, 0)  /// Storage for _DSM Arg3 parameter 1


    Method (SDSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {

      If (LEqual(Arg0, ToUUID ("E4DB149B-FCFE-425b-A6D8-92357D78FC7F"))) {
        Switch(ToInteger(Arg2)) {
          Case (0) {
            Return(0x0f)
          }
          Case (1) {
            ///- Query Device IDs (Addresses) of children where drive power and/or DevSleep are supported.
            ///- SATA HBA provides autonomous link (DevSleep) support, return a package of 0 elements
            Return( Package(){})     ///- SATA HBA provides native DevSleep
          }
          Case (2) { /// Case 2: Control power to device.
            Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
            Store(ToInteger(DerefOf(Index(Arg3, One))), PWR)

            Switch(ToInteger(DRV)) {
              Case (Ones){
                ///- Turn on drive and link power to all drives. Note that even though
                If(PWR){ // Applying Power
                  \_SB.PCI0.SAT0.PRT0.SPPR._ON() ///- apply power to port 0
                  \_SB.PCI0.SAT0.PRT1.SPPR._ON() ///- apply power to port 1
                  \_SB.PCI0.SAT0.PRT2.SPPR._ON() ///- apply power to port 2
                  \_SB.PCI0.SAT0.PRT4.SPPR._ON() ///- apply power to port 4
                }
              }
            } //Switch(DRV)
            Return (0)
          } //Case (2)
          Case (3){ /// Case 3: Current status of Device/Link of Port
            Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
            Switch(ToInteger(DRV)) {  /// Check for SATA port
              Case (0x0000FFFF){  ///- SATA Port 0
                ///- Bit0 => Device power state
                If(LEqual(\_SB.PCI0.SAT0.PRT0.SPPR._STA(), 0)){
                  Store(0, Local0)
                }Else{
                  Store(1, Local0)
                }
                Return (Local0)
              }
              Case (0x0001FFFF){  ///- SATA Port 1
                ///- Bit0 => Device power state
                If(LEqual(\_SB.PCI0.SAT0.PRT1.SPPR._STA(), 0)){
                  Store(0, Local0)
                }Else{
                  Store(1, Local0)
                }
                Return (Local0)
              }
              Case (0x0002FFFF){  ///- SATA Port 2
                ///- Bit0 => Device power state
                If(LEqual(\_SB.PCI0.SAT0.PRT2.SPPR._STA(), 0)){
                  Store(0, Local0)
                }Else{
                  Store(1, Local0)
                }
                Return (Local0)
              }
              Case (0x0004FFFF){  ///- SATA Port 4
                ///- Bit0 => Device power state
                If(LEqual(\_SB.PCI0.SAT0.PRT4.SPPR._STA(), 0)){
                  Store(0, Local0)
                }Else{
                  Store(1, Local0)
                }
                Return (Local0)
              }
              Default { ///- Invalid SATA Port - error
                Return (Ones)
              }
            }
          }
          Default {
            Return (0)
          }
        }
      } Else { // UUID does not match
        Return (Buffer() {0})
      }
    } /// @defgroup sata_dsm SATA _DSM Method
  } //Scope(\_SB.PCI0.SAT0)
// SATA - END


} // End SSDT
