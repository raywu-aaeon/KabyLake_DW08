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
// $Header: /Alaska/BIN/Chipset/Template/SouthBridge/Chipset/PciEPort.ASL 4     6/27/11 2:05p Artems $Revision:
//
// $Date: 6/27/11 2:05p $Log:
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        PciEPort.ASL
//
// Description: This file contains ACPI methods implementation for Pci Express root port
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

//-----------------------------------------------------------------------
// This file will be included in PCIExpress Bridge scope
// this file will have PCI Express features of the ICH7 Root Complex
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

