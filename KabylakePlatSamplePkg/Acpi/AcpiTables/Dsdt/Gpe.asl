/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2018 Intel Corporation.

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

  // General Purpose Events.  This Scope handles the Run-time and
  // Wake-time SCIs.  The specific method called will be determined by
  // the _Lxx value, where xx equals the bit location in the General
  // Purpose Event register(s).


#if 0  // AMI_OVERRIDE_START - we should change "D1F0" to "\_SB.PCI0.D1F0" for AMI SKL ASL architecture.
  External(D1F0)
  External(D1F1)
  External(D1F2)
#endif // AMI_OVERRIDE_END - we should change "D1F0" to "\_SB.PCI0.D1F0" for AMI SKL ASL architecture.
  External(\_SB.PCI0.PEG0.HPME, MethodObj)
  External(\_SB.PCI0.PEG1.HPME, MethodObj)
  External(\_SB.PCI0.PEG2.HPME, MethodObj)
  External(\_GPE.AL6F, MethodObj)
  External(\_GPE.P0L6, MethodObj)
  External(\_GPE.P1L6, MethodObj)
  External(\_GPE.P2L6, MethodObj)
  External(SGGP)
  External(P1GP)
  External(P2GP)
  External(P0WK)
  External(P1WK)
  External(P2WK)
  External(\_PR.HWPI, IntObj)
  External(\_PR.DTSI, IntObj)
  External(\_SB.AWAC, DeviceObj)  // ACPI000E Wake Alarm Timer device
  External(\_SB.AWAC.WAST, IntObj)

  Scope(\_GPE)
  {
  // Note:
  // Originally, the two GPE methods below are automatically generated, but, for ASL code restructuring,
  // disabled the automatic generation and declare the ASL code here.
  //

  //
  // This PME event (PCH's GPE 69h) is received on one or more of the PCI Express* ports or
  // an assert PMEGPE message received via DMI
  //
  Method(_L69, 0, serialized) {
    \_SB.PCI0.RP01.HPME()
    \_SB.PCI0.RP02.HPME()
    \_SB.PCI0.RP03.HPME()
    \_SB.PCI0.RP04.HPME()
    \_SB.PCI0.RP05.HPME()
    \_SB.PCI0.RP06.HPME()
    \_SB.PCI0.RP07.HPME()
    \_SB.PCI0.RP08.HPME()
    \_SB.PCI0.RP09.HPME()
    \_SB.PCI0.RP10.HPME()
    \_SB.PCI0.RP11.HPME()
    \_SB.PCI0.RP12.HPME()
    \_SB.PCI0.RP13.HPME()
    \_SB.PCI0.RP14.HPME()
    \_SB.PCI0.RP15.HPME()
    \_SB.PCI0.RP16.HPME()
    \_SB.PCI0.RP17.HPME()
    \_SB.PCI0.RP18.HPME()
    \_SB.PCI0.RP19.HPME()
    \_SB.PCI0.RP20.HPME()
    \_SB.PCI0.RP21.HPME()
    \_SB.PCI0.RP22.HPME()
    \_SB.PCI0.RP23.HPME()
    \_SB.PCI0.RP24.HPME()

#if 0  // AMI_OVERRIDE_START - we should change "D1F0" to "\_SB.PCI0.D1F0" for AMI SKL ASL architecture.
    If(LEqual(D1F0,1))
#else
    If(LEqual(\_SB.PCI0.D1F0,1))
#endif // AMI_OVERRIDE_END - we should change "D1F0" to "\_SB.PCI0.D1F0" for AMI SKL ASL architecture.
    {
      \_SB.PCI0.PEG0.HPME()
      Notify(\_SB.PCI0.PEG0, 0x02)
      Notify(\_SB.PCI0.PEG0.PEGP, 0x02)
    }

#if 0  // AMI_OVERRIDE_START - we should change "D1F1" to "\_SB.PCI0.D1F1" for AMI SKL ASL architecture.
    If(LEqual(D1F1,1))
#else
    If(LEqual(\_SB.PCI0.D1F1,1))
#endif // AMI_OVERRIDE_END - we should change "D1F1" to "\_SB.PCI0.D1F1" for AMI SKL ASL architecture. 
    {
      \_SB.PCI0.PEG1.HPME()
      Notify(\_SB.PCI0.PEG1, 0x02)
    }

#if 0  // AMI_OVERRIDE_START - we should change "D1F2" to "\_SB.PCI0.D1F2" for AMI SKL ASL architecture.
    If(LEqual(D1F2,1))
#else
    If(LEqual(\_SB.PCI0.D1F2,1))
#endif // AMI_OVERRIDE_END - we should change "D1F2" to "\_SB.PCI0.D1F2" for AMI SKL ASL architecture.
    {
      \_SB.PCI0.PEG2.HPME()
      Notify(\_SB.PCI0.PEG2, 0x02)
    }
  }

  // PCI Express Hot-Plug caused the wake event.

  Method(_L61)
  {
    Add(L01C,1,L01C)  // Increment L01 Entry Count.
// AMI_OVERRIDE_START - For TBT
#if !defined ASL_Thunderbolt_SUPPORT || ASL_Thunderbolt_SUPPORT == 0

    P8XH(0,0x01)      // Output information to Port 80h.
#endif
// AMI_OVERRIDE_END - For TBT
    P8XH(1,L01C)


    // Check Root Port 1 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP01.VDID,0xFFFFFFFF),\_SB.PCI0.RP01.HPSX))
    {
      // Delay for 100ms to meet the timing requirements
      // of the PCI Express Base Specification, Revision
      // 1.0A, Section 6.6 ("...software must wait at
      // least 100ms from the end of reset of one or more
      // device before it is permitted to issue
      // Configuration Requests to those devices").

      Sleep(100)

      If(\_SB.PCI0.RP01.PDCX)
      {
        // Clear all status bits first.

        Store(1,\_SB.PCI0.RP01.PDCX)
        Store(1,\_SB.PCI0.RP01.HPSX)

        //
        // PCH BIOS Spec Update Rev 1.03, Section 8.9 PCI Express* Hot-Plug BIOS Support
        // In addition, BIOS should intercept Presence Detect Changed interrupt, enable L0s on
        // hot plug and disable L0s on hot unplug. BIOS should also make sure the L0s is
        // disabled on empty slots prior booting to OS.
        //
        If(LNot(\_SB.PCI0.RP01.PDSX)) {
          // The PCI Express slot is empty, so disable L0s on hot unplug
          //
          Store(0,\_SB.PCI0.RP01.L0SE)

        }

        // Perform proper notification
        // to the OS.
        Notify(\_SB.PCI0.RP01,0)
      }
      Else
      {
        // False event.  Clear Hot-Plug Status
        // then exit.

        Store(1,\_SB.PCI0.RP01.HPSX)
      }
    }

    // Check Root Port 2 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP02.VDID,0xFFFFFFFF),\_SB.PCI0.RP02.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP02.PDCX)
      {
        Store(1,\_SB.PCI0.RP02.PDCX)
        Store(1,\_SB.PCI0.RP02.HPSX)

        If(LNot(\_SB.PCI0.RP02.PDSX)) {
          Store(0,\_SB.PCI0.RP02.L0SE)
        }

        Notify(\_SB.PCI0.RP02,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP02.HPSX)
      }
    }

    // Check Root Port 3 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP03.VDID,0xFFFFFFFF),\_SB.PCI0.RP03.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP03.PDCX)
      {
        Store(1,\_SB.PCI0.RP03.PDCX)
        Store(1,\_SB.PCI0.RP03.HPSX)

        If(LNot(\_SB.PCI0.RP03.PDSX)) {
          Store(0,\_SB.PCI0.RP03.L0SE)
        }

        Notify(\_SB.PCI0.RP03,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP03.HPSX)
      }
    }

    // Check Root Port 4 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP04.VDID,0xFFFFFFFF),\_SB.PCI0.RP04.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP04.PDCX)
      {
        Store(1,\_SB.PCI0.RP04.PDCX)
        Store(1,\_SB.PCI0.RP04.HPSX)

        If(LNot(\_SB.PCI0.RP04.PDSX)) {
          Store(0,\_SB.PCI0.RP04.L0SE)
        }

        Notify(\_SB.PCI0.RP04,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP04.HPSX)
      }
    }

    // Check Root Port 5 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP05.VDID,0xFFFFFFFF),\_SB.PCI0.RP05.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP05.PDCX)
      {
        Store(1,\_SB.PCI0.RP05.PDCX)
        Store(1,\_SB.PCI0.RP05.HPSX)

        If(LNot(\_SB.PCI0.RP05.PDSX)) {
          Store(0,\_SB.PCI0.RP05.L0SE)
        }

        Notify(\_SB.PCI0.RP05,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP05.HPSX)
      }
    }

    // Check Root Port 6 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP06.VDID,0xFFFFFFFF),\_SB.PCI0.RP06.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP06.PDCX)
      {
        Store(1,\_SB.PCI0.RP06.PDCX)
        Store(1,\_SB.PCI0.RP06.HPSX)

        If(LNot(\_SB.PCI0.RP06.PDSX)) {
          Store(0,\_SB.PCI0.RP06.L0SE)
        }

        Notify(\_SB.PCI0.RP06,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP06.HPSX)
      }
    }

    // Check Root Port 7 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP07.VDID,0xFFFFFFFF),\_SB.PCI0.RP07.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP07.PDCX)
      {
        Store(1,\_SB.PCI0.RP07.PDCX)
        Store(1,\_SB.PCI0.RP07.HPSX)

        If(LNot(\_SB.PCI0.RP07.PDSX)) {
          Store(0,\_SB.PCI0.RP07.L0SE)
        }

        //
        // AdvancedFeaturesBegin
        //
        If(LEqual(PFLV,FlavorDesktop))
        {
          Notify(\_SB.PCI0.RP07,0)
        }
        Else
        {
          If (\ECON)
          {
// @todo: Currently KBL\KBLR doesn't support real dock. Waiting for supporting real dock
//        and DOCK offset data.
//            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), 0))
//            { // If not docked then it's hot plug
//              If(LOr(LNotEqual(TBTS, 0x01),LOr(LNotEqual(TBSE,0x7),LNotEqual(TBS1,0x7)))) {
//                Notify(\_SB.PCI0.RP07,0)
//              }
//            }
          }
        }
        //
        // AdvancedFeaturesEnd
        //
      }
      Else
      {
        Store(1,\_SB.PCI0.RP07.HPSX)
      }
    }

    // Check Root Port 8 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP08.VDID,0xFFFFFFFF),\_SB.PCI0.RP08.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP08.PDCX)
      {
        Store(1,\_SB.PCI0.RP08.PDCX)
        Store(1,\_SB.PCI0.RP08.HPSX)

        If(LNot(\_SB.PCI0.RP08.PDSX)) {
          Store(0,\_SB.PCI0.RP08.L0SE)
        }

        //
        // AdvancedFeaturesBegin
        //
        If(LEqual(PFLV,FlavorDesktop))
        {
          Notify(\_SB.PCI0.RP08,0)
        }
        Else
        {
          If (\ECON)
          {
// @todo: Currently KBL\KBLR doesn't support real dock. Waiting for supporting real dock
//        and DOCK offset data.
//            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), 0))
//            { // If not docked then it's hot plug
//              If(LOr(LNotEqual(TBTS, 0x01),LOr(LNotEqual(TBSE,0x8),LNotEqual(TBS1,0x8)))) {
//                Notify(\_SB.PCI0.RP08,0)
//              }
//            }
          }
        }
        //
        // AdvancedFeaturesEnd
        //
      }
      Else
      {
        Store(1,\_SB.PCI0.RP08.HPSX)
      }
    }

    // Check Root Port 9 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP09.VDID,0xFFFFFFFF),\_SB.PCI0.RP09.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP09.PDCX)
      {
        Store(1,\_SB.PCI0.RP09.PDCX)
        Store(1,\_SB.PCI0.RP09.HPSX)

        If(LNot(\_SB.PCI0.RP09.PDSX)) {
          Store(0,\_SB.PCI0.RP09.L0SE)
        }

        Notify(\_SB.PCI0.RP09,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP09.HPSX)
      }
    }

    // Check Root Port 10 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP10.VDID,0xFFFFFFFF),\_SB.PCI0.RP10.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP10.PDCX)
      {
        Store(1,\_SB.PCI0.RP10.PDCX)
        Store(1,\_SB.PCI0.RP10.HPSX)

        If(LNot(\_SB.PCI0.RP10.PDSX)) {
          Store(0,\_SB.PCI0.RP10.L0SE)
        }

        Notify(\_SB.PCI0.RP10,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP10.HPSX)
      }
    }

    // Check Root Port 11 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP11.VDID,0xFFFFFFFF),\_SB.PCI0.RP11.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP11.PDCX)
      {
        Store(1,\_SB.PCI0.RP11.PDCX)
        Store(1,\_SB.PCI0.RP11.HPSX)

        If(LNot(\_SB.PCI0.RP11.PDSX)) {
          Store(0,\_SB.PCI0.RP11.L0SE)
        }

        Notify(\_SB.PCI0.RP11,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP11.HPSX)
      }
    }

    // Check Root Port 12 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP12.VDID,0xFFFFFFFF),\_SB.PCI0.RP12.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP12.PDCX)
      {
        Store(1,\_SB.PCI0.RP12.PDCX)
        Store(1,\_SB.PCI0.RP12.HPSX)

        If(LNot(\_SB.PCI0.RP12.PDSX)) {
          Store(0,\_SB.PCI0.RP12.L0SE)
        }

        Notify(\_SB.PCI0.RP12,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP12.HPSX)
      }
    }

    // Check Root Port 13 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP13.VDID,0xFFFFFFFF),\_SB.PCI0.RP13.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP13.PDCX)
      {
        Store(1,\_SB.PCI0.RP13.PDCX)
        Store(1,\_SB.PCI0.RP13.HPSX)

        If(LNot(\_SB.PCI0.RP13.PDSX)) {
          Store(0,\_SB.PCI0.RP13.L0SE)
        }

        Notify(\_SB.PCI0.RP13,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP13.HPSX)
      }
    }

    // Check Root Port 14 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP14.VDID,0xFFFFFFFF),\_SB.PCI0.RP14.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP14.PDCX)
      {
        Store(1,\_SB.PCI0.RP14.PDCX)
        Store(1,\_SB.PCI0.RP14.HPSX)

        If(LNot(\_SB.PCI0.RP14.PDSX)) {
          Store(0,\_SB.PCI0.RP14.L0SE)
        }

        Notify(\_SB.PCI0.RP14,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP14.HPSX)
      }
    }

    // Check Root Port 15 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP15.VDID,0xFFFFFFFF),\_SB.PCI0.RP15.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP15.PDCX)
      {
        Store(1,\_SB.PCI0.RP15.PDCX)
        Store(1,\_SB.PCI0.RP15.HPSX)

        If(LNot(\_SB.PCI0.RP15.PDSX)) {
          Store(0,\_SB.PCI0.RP15.L0SE)
        }

        Notify(\_SB.PCI0.RP15,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP15.HPSX)
      }
    }

    // Check Root Port 16 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP16.VDID,0xFFFFFFFF),\_SB.PCI0.RP16.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP16.PDCX)
      {
        Store(1,\_SB.PCI0.RP16.PDCX)
        Store(1,\_SB.PCI0.RP16.HPSX)

        If(LNot(\_SB.PCI0.RP16.PDSX)) {
          Store(0,\_SB.PCI0.RP16.L0SE)
        }

        Notify(\_SB.PCI0.RP16,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP16.HPSX)
      }
    }

    // Check Root Port 17 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP17.VDID,0xFFFFFFFF),\_SB.PCI0.RP17.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP17.PDCX)
      {
        Store(1,\_SB.PCI0.RP17.PDCX)
        Store(1,\_SB.PCI0.RP17.HPSX)

        If(LNot(\_SB.PCI0.RP17.PDSX)) {
          Store(0,\_SB.PCI0.RP17.L0SE)
        }

        Notify(\_SB.PCI0.RP17,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP17.HPSX)
      }
    }

    // Check Root Port 18 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP18.VDID,0xFFFFFFFF),\_SB.PCI0.RP18.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP18.PDCX)
      {
        Store(1,\_SB.PCI0.RP18.PDCX)
        Store(1,\_SB.PCI0.RP18.HPSX)

        If(LNot(\_SB.PCI0.RP18.PDSX)) {
          Store(0,\_SB.PCI0.RP18.L0SE)
        }

        Notify(\_SB.PCI0.RP18,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP18.HPSX)
      }
    }

    // Check Root Port 19 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP19.VDID,0xFFFFFFFF),\_SB.PCI0.RP19.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP19.PDCX)
      {
        Store(1,\_SB.PCI0.RP19.PDCX)
        Store(1,\_SB.PCI0.RP19.HPSX)

        If(LNot(\_SB.PCI0.RP19.PDSX)) {
          Store(0,\_SB.PCI0.RP19.L0SE)
        }

        Notify(\_SB.PCI0.RP19,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP19.HPSX)
      }
    }

    // Check Root Port 20 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP20.VDID,0xFFFFFFFF),\_SB.PCI0.RP20.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP20.PDCX)
      {
        Store(1,\_SB.PCI0.RP20.PDCX)
        Store(1,\_SB.PCI0.RP20.HPSX)

        If(LNot(\_SB.PCI0.RP20.PDSX)) {
          Store(0,\_SB.PCI0.RP20.L0SE)
        }

        Notify(\_SB.PCI0.RP20,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP20.HPSX)
      }
    }

    // Check Root Port 21 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP21.VDID,0xFFFFFFFF),\_SB.PCI0.RP21.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP21.PDCX)
      {
        Store(1,\_SB.PCI0.RP21.PDCX)
        Store(1,\_SB.PCI0.RP21.HPSX)

        If(LNot(\_SB.PCI0.RP21.PDSX)) {
          Store(0,\_SB.PCI0.RP21.L0SE)
        }

        Notify(\_SB.PCI0.RP21,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP21.HPSX)
      }
    }

    // Check Root Port 22 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP22.VDID,0xFFFFFFFF),\_SB.PCI0.RP22.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP22.PDCX)
      {
        Store(1,\_SB.PCI0.RP22.PDCX)
        Store(1,\_SB.PCI0.RP22.HPSX)

        If(LNot(\_SB.PCI0.RP22.PDSX)) {
          Store(0,\_SB.PCI0.RP22.L0SE)
        }

        Notify(\_SB.PCI0.RP22,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP22.HPSX)
      }
    }

    // Check Root Port 23 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP23.VDID,0xFFFFFFFF),\_SB.PCI0.RP23.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP23.PDCX)
      {
        Store(1,\_SB.PCI0.RP23.PDCX)
        Store(1,\_SB.PCI0.RP23.HPSX)

        If(LNot(\_SB.PCI0.RP23.PDSX)) {
          Store(0,\_SB.PCI0.RP23.L0SE)
        }

        Notify(\_SB.PCI0.RP23,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP23.HPSX)
      }
    }

    // Check Root Port 24 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PCI0.RP24.VDID,0xFFFFFFFF),\_SB.PCI0.RP24.HPSX))
    {
      Sleep(100)

      If(\_SB.PCI0.RP24.PDCX)
      {
        Store(1,\_SB.PCI0.RP24.PDCX)
        Store(1,\_SB.PCI0.RP24.HPSX)

        If(LNot(\_SB.PCI0.RP24.PDSX)) {
          Store(0,\_SB.PCI0.RP24.L0SE)
        }

        Notify(\_SB.PCI0.RP24,0)
      }
      Else
      {
        Store(1,\_SB.PCI0.RP24.HPSX)
      }
    }
  }

  //
  // Software GPE caused the event.
  //
  Method(_L62)
  {
    // Clear GPE status bit.
    Store(0,GPEC)

    //
    // Handle DTS Thermal SCI Event.
    //
    If(CondRefOf(\_PR.DTSE)){
      If(LGreaterEqual(\_PR.DTSE, 0x01)){
        If(LEqual(\_PR.DTSI, 1)){
          Notify(\_TZ.TZ00,0x80)
          Notify(\_TZ.TZ01,0x80)
          ///
          /// Clear HWP interrupt status
          ///
          Store(0,\_PR.DTSI)
        }
      }
    }
    ///
    /// Handle HWP SCI event
    ///
    External(\_GPE.HLVT, MethodObj)
    If(LEqual(\_PR.HWPI, 1)){
      If(CondRefOf(\_GPE.HLVT)){
        \_GPE.HLVT()
      }
      ///
      /// Clear HWP interrupt status
      ///
      Store(0,\_PR.HWPI)
    }
  }

  // IGD OpRegion SCI event (see IGD OpRegion/Software SCI BIOS SPEC).

  Method(_L66)
  {
    If(LAnd(\_SB.PCI0.GFX0.GSSE, LNot(GSMI))) // Graphics software SCI event?
    {
      \_SB.PCI0.GFX0.GSCI() // Handle the SWSCI
    }
  }

#if 0 // AMI_OVERRIDE_START - It is for CRB SIO using,OEM doesn't need to it.
  //
  // AdvancedFeaturesBegin
  //
  // RI# caused the wake event (COMA).

  Method(_L68)
  {
    Notify(\_SB.PCI0.LPCB.SM17.UAR1,0x02)
  }
#endif // AMI_OVERRIDE_END - It is for CRB SIO using,OEM doesn't need to it.

  //
  // GPP_E_16 = EC WAKETIME SCI
  //
// AMI_OVERRIDE_START - Fix building error when EC_SUPPORT = 0x0
#if defined(ASL_CRB_EC_SUPPORT) && (ASL_CRB_EC_SUPPORT == 1)
  Method(_L50)
  {
    // Do nothing if Desktop platform
    If (LEqual(\ECON,0))
    {
      Return ()
    }

    // If the wake event is not a Virtual Lid or Battery, then
    // it must be a Wake from either the Keyboard or Mouse. A
    // Power Button Event will be sent for both events such
    // that the video turns back on for these "attended wake"
    // events.

    // Check for Virtual Lid Event.

    If(LNotEqual(LIDS,\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE))))
    {
      Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE)),LIDS)

      If(IGDS)
      {
        // Upon waking a lid event may fail if driver is not ready.
        // If it does, set the high bit of the variable to indicate that
        // another notification is required during system callbacks.
        If(\_SB.PCI0.GFX0.GLID(LIDS))
        {
          Or(0x80000000,\_SB.PCI0.GFX0.CLID, \_SB.PCI0.GFX0.CLID)
        }
      }
      Notify(\_SB.PCI0.LPCB.H_EC.LID0,0x80)
    }
    Else
    {
      // Check for Virtual Power Event.

      If(LEqual(BNUM,0))
      {
        If(LNotEqual(PWRS,\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR))))
        {
          // Update NVS Power State.
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR)),PWRS)
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR)),\_PR.POWS)

          // Perform needed ACPI Notifications.
          PNOT()
        }
      }

      // Make it an attended wake event, no matter what.
      Notify(\_SB.PWRB,0x02)
    }
    Return ()
  }
#endif
// AMI_OVERRIDE_END - Fix building error when EC_SUPPORT = 0x0

  Name(PRES, 1) // keep last PCH PwrBtn status, 0=press, 1=release
  Method(_L6F) // 2-tier GPE event handler
  {
    //
    // handle events from RTD3 support
    //
    If(LEqual(RTD3,1)) // if RTD3 enabled
    {
      If (CondRefOf(\_GPE.AL6F))
      {
        \_GPE.AL6F()        // call handler from RTD3 table
      }
    }

    //
    // handle events from PEG RTD3 support
    //

      If(LEqual(SGGP,1))      // if PEG0 GPIO support is enabled
      {
        If (CondRefOf(\_GPE.P0L6))
        {
          \_GPE.P0L6()        // call handler from RTD3 table
        }
      }

      If(LEqual(P1GP,1)) // if PEG1 GPIO support is enabled
      {
        If (CondRefOf(\_GPE.P1L6))
        {
          \_GPE.P1L6()        // call handler from RTD3 table
        }
      }

      If(LEqual(P2GP,1)) // if PEG2 GPIO support is enabled
      {
        If (CondRefOf(\_GPE.P2L6))
        {
          \_GPE.P2L6()        // call handler from RTD3 table
        }
      }
// AMI_OVERRIDE_START - Below ASL code is for Thunderbolt, AMI thunderbolt module has taken care it already
#if ASL_Thunderbolt_SUPPORT

    If(LEqual(TBTS, 1)) // if TBTsupport enabled
    {
      If (\_SB.ISME(CPGN))
      {
        \_SB.THDR(TBSE,CPGN) // Check for TBT Hotplug Handler event (2-tier GPI GPE event architecture)
      }
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
      If (\_SB.ISME(CPG1))
      {
        \_SB.THDR(TBS1,CPG1) // Check for TBT Hotplug Handler event (2-tier GPI GPE event architecture)
      }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
    }
#endif 
// AMI_OVERRIDE_END - Below ASL code is for Thunderbolt, AMI thunderbolt module has taken care it already
  }

  // GPE 72h handler for Time And Alarm Device
  Method (_L72, 0, Serialized){
    If (CondRefOf(\_SB.AWAC)) {
      Store (1, \_SB.AWAC.WAST) // Indicates Wake Alarm expired.
      Notify (\_SB.AWAC, 0x02) // Notify the OSPM of Alarm Device Wake.
    }
  }

  //
  // AdvancedFeaturesEnd
  //
}
