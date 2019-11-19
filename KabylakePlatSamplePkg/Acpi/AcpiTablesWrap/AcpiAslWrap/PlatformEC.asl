/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Sandy Bridge        *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2010 Intel Corporation. All rights reserved   *;
;*    This software and associated documentation (if any) is furnished    *;
;*    under a license and may only be used or copied in accordance        *;
;*    with the terms of the license. Except as permitted by such          *;
;*    license, no part of this software or documentation may be           *;
;*    reproduced, stored in a retrieval system, or transmitted in any     *;
;*    form or by any means without the express written consent of         *;
;*    Intel Corporation.                                                  *;
;*                                                                        *;
;*                                                                        *;
;**************************************************************************/
/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/

#if !defined(ASL_CRB_EC_SUPPORT) || (ASL_CRB_EC_SUPPORT == 0)  

Scope(\_SB.PCI0.LPCB)
{

Device(H_EC)  // Hitachi Embedded Controller
{
  Name(_HID, EISAID("PNP0C09"))

  Name(_UID,1)

  Method(_STA)
  {
    Store(3, \_SB.PCI0.GFX0.CLID)
    Return(0x00) //Hide device
  }
  Name(ECAV, 0)   // OS Bug Checks if EC OpRegion accessed before Embedded Controller Driver loaded

  Name(B1ST, ASL_B1ST_DEFOF_NUM)
  Name(B2ST, ASL_B2ST_DEFOF_NUM)
  Name(CFAN, ASL_CFAN_DEFOF_NUM)
  Name(CMDR, ASL_CMDR_DEFOF_NUM)
  Name(DOCK, ASL_DOCK_DEFOF_NUM)
  Name(PLMX, ASL_PLMX_DEFOF_NUM)
  Name(PECH, ASL_PECH_DEFOF_NUM)
  Name(PECL, ASL_PECL_DEFOF_NUM)
  Name(PENV, ASL_PENV_DEFOF_NUM)
  Name(PINV, ASL_PINV_DEFOF_NUM)
  Name(PPSH, ASL_PPSH_DEFOF_NUM)
  Name(PPSL, ASL_PPSL_DEFOF_NUM)
  Name(PSTP, ASL_PSTP_DEFOF_NUM)
  Name(RPWR, ASL_RPWR_DEFOF_NUM)
  Name(LSTE, ASL_LSTE_DEFOF_NUM)
  Name(VPWR, ASL_VPWR_DEFOF_NUM)
  Name(WTMS, ASL_WTMS_DEFOF_NUM)
  Name(AWT2, ASL_AWT2_DEFOF_NUM)
  Name(AWT1, ASL_AWT1_DEFOF_NUM)
  Name(AWT0, ASL_AWT0_DEFOF_NUM)
  Name(DLED, ASL_DLED_DEFOF_NUM)
  Name(SPT2, ASL_SPT2_DEFOF_NUM)
  Name(PB10, ASL_PB10_DEFOF_NUM)
  Name(IWCW, ASL_IWCW_DEFOF_NUM)
  Name(IWCR, ASL_IWCR_DEFOF_NUM)
  Name(PVOL, ASL_PVOL_DEFOF_NUM)    
  Name(PWRT, ASL_PWRT_DEFOF_NUM)      
  

  // ECRD (Embedded Read Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (1)
  //    Arg0 - Object to Read
  //  Return Value:
  //    Read Value
  //    
  Method(ECRD,1,Serialized, 0, IntObj, FieldUnitObj)
  {
      Store(DerefOf (Arg0), Local0)
      Switch(ToInteger(Local0))
      {
        Case(ASL_B1ST_DEFOF_NUM)
        {
          ASL_B1ST_ECRD_METHOD
        }        
        Case(ASL_B2ST_DEFOF_NUM)
        {
          ASL_B2ST_ECRD_METHOD
        }
        Case(ASL_CFAN_DEFOF_NUM)
        {
          ASL_CFAN_ECRD_METHOD
        }
        Case(ASL_CMDR_DEFOF_NUM)
        {
          ASL_CMDR_ECRD_METHOD
        }
        Case(ASL_DOCK_DEFOF_NUM)
        {
          ASL_DOCK_ECRD_METHOD
        }
        Case(ASL_PLMX_DEFOF_NUM)
        {
          ASL_PLMX_ECRD_METHOD
        }
        Case(ASL_PECH_DEFOF_NUM)
        {
          ASL_PECH_ECRD_METHOD
        }
        Case(ASL_PECL_DEFOF_NUM)
        {
          ASL_PECL_ECRD_METHOD
        }
        Case(ASL_PENV_DEFOF_NUM)
        {
          ASL_PENV_ECRD_METHOD
        }
        Case(ASL_PINV_DEFOF_NUM)
        {
          ASL_PINV_ECRD_METHOD
        }
        Case(ASL_PPSH_DEFOF_NUM)
        {
          ASL_PPSH_ECRD_METHOD
        }
        Case(ASL_PPSL_DEFOF_NUM)
        {
          ASL_PPSL_ECRD_METHOD
        }
        Case(ASL_PSTP_DEFOF_NUM)
        {
          ASL_PSTP_ECRD_METHOD
        }
        Case(ASL_RPWR_DEFOF_NUM)
        {
          ASL_RPWR_ECRD_METHOD
        }
        Case(ASL_LSTE_DEFOF_NUM)
        {
          ASL_LSTE_ECRD_METHOD
        }
        Case(ASL_VPWR_DEFOF_NUM)
        {
          ASL_VPWR_ECRD_METHOD
        }
        Case(ASL_WTMS_DEFOF_NUM)
        {
          ASL_WTMS_ECRD_METHOD
        }
        Case(ASL_AWT2_DEFOF_NUM)
        {
          ASL_AWT2_ECRD_METHOD
        }
        Case(ASL_AWT1_DEFOF_NUM)
        {
          ASL_AWT1_ECRD_METHOD
        }
        Case(ASL_AWT0_DEFOF_NUM)
        {
          ASL_AWT0_ECRD_METHOD
        }
        Case(ASL_DLED_DEFOF_NUM)
        {
          ASL_DLED_ECRD_METHOD
        }
        Case(ASL_SPT2_DEFOF_NUM)
        {
          ASL_SPT2_ECRD_METHOD
        }
        Case(ASL_PB10_DEFOF_NUM)
        {
          ASL_PB10_ECRD_METHOD
        }
        Case(ASL_IWCW_DEFOF_NUM)
        {
          ASL_IWCW_ECRD_METHOD
        }
        Case(ASL_IWCR_DEFOF_NUM)
        {
          ASL_IWCR_ECRD_METHOD
        }
        Case(ASL_PVOL_DEFOF_NUM)
        {
          ASL_PVOL_ECRD_METHOD
        }
        Case(ASL_PWRT_DEFOF_NUM)
        {
          ASL_PWRT_ECRD_METHOD
        }                
      }
      Return(0)        
  }
    
  // ECWT (Embedded Write Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (2)
  //    Arg0 - Value to Write
  //    Arg1 - Object to Write to
  //    
  Method(ECWT,2,Serialized,,,{IntObj, FieldUnitObj})
  {
      Store(DerefOf (Arg0), Local0)
      Switch(ToInteger(Local0))
      {
        Case(ASL_B1ST_DEFOF_NUM)
        {
          ASL_B1ST_ECWT_METHOD
        }        
        Case(ASL_B2ST_DEFOF_NUM)
        {
          ASL_B2ST_ECWT_METHOD
        }
        Case(ASL_CFAN_DEFOF_NUM)
        {
          ASL_CFAN_ECWT_METHOD
        }
        Case(ASL_CMDR_DEFOF_NUM)
        {
          ASL_CMDR_ECWT_METHOD
        }
        Case(ASL_DOCK_DEFOF_NUM)
        {
          ASL_DOCK_ECWT_METHOD
        }
        Case(ASL_PLMX_DEFOF_NUM)
        {
          ASL_PLMX_ECWT_METHOD
        }
        Case(ASL_PECH_DEFOF_NUM)
        {
          ASL_PECH_ECWT_METHOD
        }
        Case(ASL_PECL_DEFOF_NUM)
        {
          ASL_PECL_ECWT_METHOD
        }
        Case(ASL_PENV_DEFOF_NUM)
        {
          ASL_PENV_ECWT_METHOD
        }
        Case(ASL_PINV_DEFOF_NUM)
        {
          ASL_PINV_ECWT_METHOD
        }
        Case(ASL_PPSH_DEFOF_NUM)
        {
          ASL_PPSH_ECWT_METHOD
        }
        Case(ASL_PPSL_DEFOF_NUM)
        {
          ASL_PPSL_ECWT_METHOD
        }
        Case(ASL_PSTP_DEFOF_NUM)
        {
          ASL_PSTP_ECWT_METHOD
        }
        Case(ASL_RPWR_DEFOF_NUM)
        {
          ASL_RPWR_ECWT_METHOD
        }
        Case(ASL_LSTE_DEFOF_NUM)
        {
          ASL_LSTE_ECWT_METHOD
        }
        Case(ASL_VPWR_DEFOF_NUM)
        {
          ASL_VPWR_ECWT_METHOD
        }
        Case(ASL_WTMS_DEFOF_NUM)
        {
          ASL_WTMS_ECWT_METHOD
        }
        Case(ASL_AWT2_DEFOF_NUM)
        {
          ASL_AWT2_ECWT_METHOD
        }
        Case(ASL_AWT1_DEFOF_NUM)
        {
          ASL_AWT1_ECWT_METHOD
        }
        Case(ASL_AWT0_DEFOF_NUM)
        {
          ASL_AWT0_ECWT_METHOD
        }
        Case(ASL_DLED_DEFOF_NUM)
        {
          ASL_DLED_ECWT_METHOD
        }
        Case(ASL_SPT2_DEFOF_NUM)
        {
          ASL_SPT2_ECWT_METHOD
        }
        Case(ASL_PB10_DEFOF_NUM)
        {
          ASL_PB10_ECWT_METHOD
        }
        Case(ASL_IWCW_DEFOF_NUM)
        {
          ASL_IWCW_ECWT_METHOD
        }
        Case(ASL_IWCR_DEFOF_NUM)
        {
          ASL_IWCR_ECWT_METHOD
        }
        Case(ASL_PVOL_DEFOF_NUM)
        {
          ASL_PVOL_ECWT_METHOD
        }
        Case(ASL_PWRT_DEFOF_NUM)
        {
          ASL_PWRT_ECWT_METHOD
        }                
      }
  }
  
    Method(ECMD,1,Serialized)  // Handle all commands sent to EC by BIOS
  {
  }
  
  // ECNT (Embedded Controller Notify)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (1)
  //    Arg0 - 1 = CS Entry Notify
  //           0 = CS Exit Notify
  //  Return Value:
  //    0x00 = Success
  //    0xFF = Failure
  //
  Method(ECNT,1,Serialized)
  {
  }    
      
  Device(BAT0)
  {
    Name(_HID,EISAID("PNP0C0A"))
    
    Name(_UID,0)
    
    Method(_STA,0)
    {
      Return(0)       // Hide device 
    }
  }

  Device(BAT1)
  {
    Name(_HID,EISAID("PNP0C0A"))

    Name(_UID,1)
    
    Method(_STA,0)
    {
        Return(0)       // Hide device 
    }
    Method(_BST,0)
    {
      ASL_BST1_METHOD
    }        
  }

  Device(BAT2)
  {
    Name(_HID,EISAID("PNP0C0A"))

    Name(_UID,2)
    
    Method(_STA,0)
    {
        Return(0)       // Hide device 
    }
    Method(_BST,0)
    {
      ASL_BST2_METHOD
    }            
  }
  
}// end Device(H_EC)
}// end scope Scope(\_SB.PCI0.LPCB)
  // System Bus

  Scope(\)
  {
    // these fields come from the Global NVS area
    Field(GNVS,AnyAcc,Lock,Preserve)
    {
      Offset(32),
        B1SC, 8,    //   (32) Battery 1 Stored Capacity
      Offset(35),
        B1SS, 8,    //   (35) Battery 1 Stored Status
    }
  } // end  Scope(\)

  Scope(\)
  {
    // these fields come from the Global NVS area
    Field(GNVS,AnyAcc,Lock,Preserve)
    {
          Offset(33),
      B2SC, 8,    //   (33) Battery 2 Stored Capacity
      Offset(36),
      B2SS, 8 //   (36) Battery 2 Stored Status
    }
  } // end Scope(\)

Device (\_SB.PCI0.DOCK)
{
    Name(_HID, "ABCD0000")
    Name(_CID, EISAID("PNP0C15"))
    Name(_UID,2)
    Method(_STA)
    {
      Return(0x00)
    }
}

Scope(\_SB)
{

  Method(PWPR, 0, Serialized) // Power Button Press Helper Method
  {
     //
     // OEM Porting...
     //
  }
  
  Method(PWRR, 0, Serialized) // Power Button Release Helper Method  
  {
     //
     // OEM Porting...
     //
  }

  // Define a Lid Switch.    
  Device(LID0)
  {
    Name(_HID,EISAID("PNP0C0D"))

    Method(_STA)
    {
      Return(0x00)
    }
    Method(_LID,0)
    {
      // 0 = Closed, 1 = Open.
      Return(0x01)
    }
  }
}//end scope _SB

#endif


