//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/BIN/Chipset/Template/SouthBridge/Chipset/PciEPortHP.ASL 4     6/27/11 2:05p Artems $
//
// $Revision: 4 $
//
// $Date: 6/27/11 2:05p $
//

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        PciEPortHP.ASL
//
// Description: This file contains ACPI methods implementation for Pci Express hot plug port
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

//-----------------------------------------------------------------------
// This file will be included in PCIExpress Bridge scope
// this file will have PCI Express features of the ICH10 Root Complex
//-----------------------------------------------------------------------


// PCI Express Root Complex Port
OperationRegion (PXRC, PCI_Config, 0x00, 0x100)
Field (PXRC, AnyAcc, NoLock, Preserve)
{
  // Root Status Register
  offset (0x60),
  , 16,
  PMS, 1,         // PME Status
  PMP, 1,         // PME Pending
  //Miscellaneous Port Config Register
  offset (0xD8),
  , 30,
  HPE, 1,         // HotPlug SCI Enable
  PCE, 1,         // PME  SCI Enable
  , 30,
  HPS, 1,         // HotPlug SCI Status
  PCS, 1,         // PM SCI Status
}

// Clear Sleep Statuses
Method(CSS, 0) {
  Store(1, PMS)
  Store(1, PCS)
  Store(1, PMS)
}

// PCI Express Port Prepare To Sleep Method
Method(SPRT, 1) {
  // Clear Statuses
  CSS()
  // Enable Wake Events
  // If(LGreater(Arg0,3)){Store(0,PCE)} //we are able to wake from S1 and S3 only
  // Else {
  Store(1,PCE)
  CSS()
  // }
}

Method(WPRT, 1) {
  // Disable Wake events
  Store(0, PCE)
  // Clear SleepStatuses
  CSS()
}

Name(TPXB, 0x0)

Method (_INI, 0, NotSerialized) {
    if(Lequal(^H000._SUN,10)){
        Store(TPXB, \XB0A)
    } else {
        if(Lequal(^H000._SUN,20)){
            Store(^TPXB, \XB14)
        } 
    } 

    EGPE ()          //enable GPE message generation for ACPI hotplug support
}



//Method to enable GPE event generation in RC bridge.
Method (EGPE, 0, Serialized) 
{
        Store(0x61, DBG8)       // DBG8 name translates to IO port 80h
//Porting:
//Program RC to do it HERE
/*
    Store(Rwpe(0x480aa), Local0)
        Or (Local0, 0x1f, Local0)   // Clear status bits for hot plug events
      Wbpe (0x480aa, Local0)    

    Store(Rwpe(0x480a8), Local0)
        Or (Local0, 0x1f, Local0)   // Enable hot plug events
      Wbpe (0x480a8, Local0)    

        Store (Rwpe(0x48188), Local0)
        Or (Local0, 0x08, Local0)   // Enable ACPI mode for hot plug events
      Wbpe (0x48188, Local0)    
*/
}

    
Method(HPEH, 1){
    if(Lequal(Arg0, ^H000._SUN)){
        //Calling "H"andler of "H"ot-"P"lug "E"vent
        Return(\_SB.PCI0.HHPE(TPXB))
    } else {
        Return (0xFF)
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
