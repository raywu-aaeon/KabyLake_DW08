/** @file
  This file contains the system BIOS switchable graphics code for
  ULX.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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

DefinitionBlock (
    "SgUlx.aml",
    "SSDT",
    2,
    "SgRef",
    "SgUlx",
    0x1000
    )
{
// AMI_OVERRIDE_SA0004_START >>>
#ifdef ASL_SG_ULT_RP_NUM
External(ASL_SG_ULT_RP_NUM, DeviceObj)
External(ASL_SG_ULT_RP_NUM._ADR, MethodObj)
#else
External(\_SB.PCI0.RP01, DeviceObj)
External(\_SB.PCI0.RP01.PEGP, DeviceObj)
External(\_SB.PCI0.RP01._ADR, MethodObj)
#endif
// AMI_OVERRIDE_SA0004_END <<<
External(\_SB.PCI0.HGON, MethodObj)
External(\_SB.PCI0.HGOF, MethodObj)
// AMI_OVERRIDE_SA0030_START >>>
#if defined(ASL_NV_N17P_WORKAROUND_SUPPORT) && (ASL_NV_N17P_WORKAROUND_SUPPORT == 1)  
External(\_SB.PCI0.SGPO, MethodObj)
External(\HRE0)
External(\HRG0)
External(\HRA0)
#endif
// AMI_OVERRIDE_SA0030_END <<<
// AMI_OVERRIDE_SA0001_START >>>
#if ASL_NV_GC6_SUPPORT == 2
External(ASL_DGPUPCH_SCOPE.GC6O, MethodObj)
External(ASL_DGPUPCH_SCOPE.GC6I, MethodObj)
#endif
External(\EBAS)
External(\HYSS)
External(\XBAS)
// AMI_OVERRIDE_SA0001_END <<<
// AMI_OVERRIDE_SA0004_START >>>
#ifdef ASL_SG_ULT_RP_ADR
External(ASL_SG_ULT_RP_ADR)
#else
External(\RPA1)
#endif
// AMI_OVERRIDE_SA0004_END <<<
External(GPRW, MethodObj)
External(OSYS)

// AMI_OVERRIDE_SA0004_START >>>
#ifdef ASL_SG_ULT_RP_NUM
Scope(ASL_SG_ULT_RP_NUM)
#else
  Scope(\_SB.PCI0.RP01)
#endif
// AMI_OVERRIDE_SA0004_END <<<
  {
// AMI_OVERRIDE_SA0001_START >>>
    OperationRegion (MSID, SystemMemory, EBAS, 0x500)
    Field(MSID, DWordAcc, Lock, Preserve)
    {
        VEID, 16,
        Offset(0x40),
        NVID, 32,
        offset(0x4c),
        ATID, 32,
        Offset(0x48B),
        , 1,
        NHDA, 1,
    }
#ifdef ASL_SG_ULT_RP_NUM
    OperationRegion(RPCX,SystemMemory,Add(Add(\XBAS,ShiftLeft(ShiftRight(And(ASL_SG_ULT_RP_NUM._ADR(),0xFF0000),0x10),0xF)), ShiftLeft(And(ASL_SG_ULT_RP_ADR,0xF),12)),0x1000)
#else
    OperationRegion(RPCX,SystemMemory,Add(Add(\XBAS,ShiftLeft(ShiftRight(And(\_SB.PCI0.RP01._ADR(),0xFF0000),0x10),0xF)), ShiftLeft(And(\RPA1,0xF),12)),0x1000)
#endif
    Field(RPCX,DWordAcc,NoLock,Preserve)
    {
        Offset(0x4),
        CMDR,   8,
        Offset(0xA4),
        D0ST,   2,
    }
#if ASL_NV_GC6_SUPPORT == 2    
    Name(TDGC, Zero)
    Name(DGCX, Zero)
    Name(TGPC, Buffer(0x04)
    {
      0x00
    })
#endif
// AMI_OVERRIDE_SA0001_END <<<
// AMI_OVERRIDE_SA0030_START >>>
#if defined(ASL_NV_N17P_WORKAROUND_SUPPORT) && (ASL_NV_N17P_WORKAROUND_SUPPORT == 1)  
  Name(DCNT, Zero)
  Name(OPTD, Zero)            
  Method(DCHK, 0, Serialized) { // Detect GPU Vender ID.
    Store(Zero, DCNT)                          
    While(LLess(DCNT, 50))                 
    {
      If(LEqual(VEID, 0xFFFF))
      {
        Return(One)
      } 
      Else 
      {
        Add(DCNT,1, DCNT) 
      } 
    } 
    Return(Zero)  
  } 
#endif  
// AMI_OVERRIDE_SA0030_END <<<  
    //-----------------------------------------
    // Runtime Device Power Management - Begin
    //-----------------------------------------
    //
    // Name: PC01
    // Description: Declare a PowerResource object for RP01 slot device
    //
    PowerResource(PC01, 0, 0) {
      Name(_STA, One)
      Method(_ON, 0, Serialized) {
// AMI_OVERRIDE_SA0001_START >>>
#if ASL_NV_GC6_SUPPORT == 2
        If(LEqual(TDGC, One))
        {
          If(LEqual(DGCX, 0x03))
          {
            Store(One, _STA)
            ASL_DGPUPCH_SCOPE.GC6O()
          }
          Else
          {
            If(LEqual(DGCX, 0x04))
            {
              Store(One, _STA)
              ASL_DGPUPCH_SCOPE.GC6O()
            }
          } 
          Store(Zero, TDGC)
          Store(Zero, DGCX)
        }
        Else
        {
#endif
// AMI_OVERRIDE_SA0001_END <<<
        If(LNotEqual(OSYS,2009)) {
// AMI_OVERRIDE_SA0004_START >>>
#if ASL_SG_ULT_PORT_DEV == 28
          \_SB.PCI0.HGON(1)
#else
          \_SB.PCI0.HGON(9)
#endif
// AMI_OVERRIDE_SA0004_END <<<
// AMI_OVERRIDE_SA0001_START >>>
         Or(CMDR, 0x07, CMDR) // Fixed Hi bytes be write to zero.
         Store(0, D0ST)
// AMI_OVERRIDE_SA0030_START >>>
#if defined(ASL_NV_N17P_WORKAROUND_SUPPORT) && (ASL_NV_N17P_WORKAROUND_SUPPORT == 1)  
        Sleep(50) //50ms delay        
        Store(Zero, OPTD) //10 loops                     
        While(LAnd(DCHK(), LLess(OPTD,10))) // To do GPU reset when detect GPU Vender ID is invalid.
        {
          \_SB.PCI0.SGPO(HRE0, HRG0, HRA0, One) // Assert GPU reset
          Stall(100) 
          \_SB.PCI0.SGPO(HRE0, HRG0, HRA0, Zero) // De-assert GPU reset
          Sleep(50)
          Add(OPTD,1,OPTD)
        }
#endif	
// AMI_OVERRIDE_SA0030_END >>>
         If(LEqual(VEID,0x10DE))
         {
           Store(HYSS, NVID)
           Store(0, NHDA)
         }
         If(LEqual(VEID,0x1002))
         {
           Store(HYSS, ATID)
         }
// AMI_OVERRIDE_SA0001_END <<<
          Store(One, _STA)
        }
// AMI_OVERRIDE_SA0001_START >>>
#if ASL_NV_GC6_SUPPORT == 2
        }//Else
#endif
// AMI_OVERRIDE_SA0001_END <<<  
      }
      Method(_OFF, 0, Serialized) {
// AMI_OVERRIDE_SA0001_START >>>
#if ASL_NV_GC6_SUPPORT == 2
        If(LEqual(TDGC, One))
        {
          CreateField(TGPC,Zero,0x03,GUPC)
          If(LEqual(ToInteger(GUPC), One))
          {
            ASL_DGPUPCH_SCOPE.GC6I()
            Store(Zero, _STA)
          }
          Else
          {
            If(LEqual(ToInteger(GUPC), 0x02))
            {
              ASL_DGPUPCH_SCOPE.GC6I()
              Store(Zero, _STA)
            }
          }
        }
        Else
        {
#endif
// AMI_OVERRIDE_SA0001_END <<<
        If(LNotEqual(OSYS,2009)) {
// AMI_OVERRIDE_SA0004_START >>>
#if ASL_SG_ULT_PORT_DEV == 28
          \_SB.PCI0.HGOF(1)
#else
          \_SB.PCI0.HGOF(9)
#endif
// AMI_OVERRIDE_SA0004_END <<<
          Store(Zero, _STA)
        }
// AMI_OVERRIDE_SA0001_START >>>
#if ASL_NV_GC6_SUPPORT == 2
        }
#endif
// AMI_OVERRIDE_SA0001_END <<<   
      }
    } //End of PowerResource(PC01, 0, 0)

    Name(_PR0,Package(){PC01})
    Name(_PR2,Package(){PC01})
    Name(_PR3,Package(){PC01})

    Method(_S0W, 0) { Return(4) } //D3cold is supported

    //-----------------------------------------
    // Runtime Device Power Management - End
    //-----------------------------------------

    Device(PEGP) { // (PCI Express* -> B00:D28:F00) Slot Device D0F0
      Name(_ADR, 0x00000000)
      Method(_PRW, 0) { Return(GPRW(0x69, 4)) } // can wakeup from S4 state
    } // (PCI Express* -> B00:D28:F00) Slot Device D0F0

    Device(PEGA) { // (PCI Express* -> B00:D28:F01) Slot Device D0F1
      Name(_ADR, 0x00000001)
      Method(_PRW, 0) { Return(GPRW(0x69, 4)) } // can wakeup from S4 state
    } // (PCI Express* -> B00:D28:F01) Slot Device D0F1
  }

// AMI_OVERRIDE_SA0004_START >>>
#ifdef ASL_SG_ULT_RP_NUM
  Scope(ASL_SG_ULT_RP_NUM.PEGP)
#else
  Scope(\_SB.PCI0.RP01.PEGP)
#endif
// AMI_OVERRIDE_SA0004_END <<<
  {

    Method (_INI)
    {
// AMI_OVERRIDE_SA0004_START >>>
#ifdef ASL_SG_ULT_RP_NUM
        Store (0x0, ASL_SG_ULT_RP_NUM.PEGP._ADR)
#else
        Store (0x0, \_SB.PCI0.RP01.PEGP._ADR)
#endif
// AMI_OVERRIDE_SA0004_END <<<
    }

// AMI_OVERRIDE_SA0001_START >>>
#if 0
    Method(_ON,0,Serialized)
    {
      \_SB.PCI0.HGON(1)

      //Ask OS to do a PnP rescan
      Notify(\_SB.PCI0.RP01,0)

      Return ()
    }

    Method(_OFF,0,Serialized)
    {
      \_SB.PCI0.HGOF(1)

      //Ask OS to do a PnP rescan
      Notify(\_SB.PCI0.RP01,0)

      Return ()
    }
#endif
// AMI_OVERRIDE_SA0001_END <<<
  }
}