//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <UART3.asl>
//
// Description: Define ACPI device for UART3.
//
//<AMI_FHDR_END>
//**********************************************************************
// Category # :0x11
//======================================================================
//Following commented line will be included in PciTree.asl 
//Device(UAR3) {
//    Name(_UID, 3)                                 //Generic ID for COMC
//    Name(_HID, EISAID("PNP0501"))                 //PnP Device ID 16550 Type
//======================================================================
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//    Name(_DDN, "COM3") //RayWu, ADD 2014/09/03
//
//    Method(_STA, 0) {Return(^^SIO2.DSTA(0x11))}     //Get UART status
//    Method(_DIS, 0) {^^SIO2.DCNT(0x11, 0)}          //Disable UART
//    Method(_CRS, 0) {Return(^^SIO2.DCR3(0x11, 0))}  //Get UART current resources
//    Method(_SRS, 1) {^^SIO2.DSR3(Arg0, 0x11)}       //Set UART resources
    Name(_DDN, "COM7") //RayWu, ADD 2014/09/03

    Method(_STA, 0) {Return(^^SIO2.DSTA(0x08))}     //Get UART status
    Method(_DIS, 0) {^^SIO2.DCNT(0x08, 0)}          //Disable UART
    Method(_CRS, 0) {Return(^^SIO2.DCR3(0x08, 0))}  //Get UART current resources
    Method(_SRS, 1) {^^SIO2.DSR3(Arg0, 0x08)}       //Set UART resources
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
//----------------------------------------------------------------------
//UART3 Possible Resources
//----------------------------------------------------------------------
//NOTE: _PRS MUST be the NAME not a METHOD object 
//to have GENERICSIO.C working right! 
//----------------------------------------------------------------------
//RayWu, ADD 2014/09/03 >>
//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//    Name(_PRS, ResourceTemplate() {
//        StartDependentFn(0, 0) {
//            IO(Decode16, 0x3E8, 0x3E8, 1, 8)
//            IRQ(Level,ActiveLow,Shared) {11}
//        }
//        StartDependentFnNoPri() {
//            IO(Decode16, 0x2E8, 0x2E8, 1, 8)
//            IRQ(Level,ActiveLow,Shared) {11}
//        }
//        EndDependentFn()
//    })
    Name(_PRS, ResourceTemplate() {
        StartDependentFn(0, 0) {
            IO(Decode16, 0x2D8, 0x2D8, 1, 8)
//ray_override / [XI-Fixed] Issue Fixed : Unknown Loading under Windows 7 / Modified >>
//            IRQ(Level,ActiveLow,Shared) {10}
            IRQ(Edge,ActiveLow,Shared) {10}
//ray_override / [XI-Fixed] Issue Fixed : Unknown Loading under Windows 7 / Modified <<
        }
        StartDependentFnNoPri() {
            IO(Decode16, 0x2C8, 0x2C8, 1, 8)
//ray_override / [XI-Fixed] Issue Fixed : Unknown Loading under Windows 7 / Modified >>
//            IRQ(Level,ActiveLow,Shared) {10}
            IRQ(Edge,ActiveLow,Shared) {10}
//ray_override / [XI-Fixed] Issue Fixed : Unknown Loading under Windows 7 / Modified <<
        }
        EndDependentFn()
    })
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<
//RayWu, ADD 2014/09/03 <<
//RayWu, REMOVE 2014/09/03 >>
//    Name(_PRS, ResourceTemplate() {
//        StartDependentFn(0, 0) {
//            IO(Decode16, 0x3E8, 0x3E8, 1, 8)
//            IRQ(Level,ActiveLow,Shared) {7}
//            DMA(Compatibility, NotBusMaster, Transfer8) {}
//        }
//        StartDependentFnNoPri() {
//            IO(Decode16, 0x3E8, 0x3E8, 1, 8)
//            IRQ(Level,ActiveLow,Shared) {3,4,5,6,7,10,11,12}
//            DMA(Compatibility, NotBusMaster, Transfer8) {}
//        }
//        StartDependentFnNoPri() {
//            IO(Decode16, 0x2E8, 0x2E8, 1, 8)
//            IRQ(Level,ActiveLow,Shared) {3,4,5,6,7,10,11,12}
//            DMA(Compatibility, NotBusMaster, Transfer8) {}
//        }
//        StartDependentFnNoPri() {
//            IO(Decode16, 0x2F0, 0x2F0, 1, 8)
//            IRQ(Level,ActiveLow,Shared) {3,4,5,6,7,10,11,12}
//            DMA(Compatibility, NotBusMaster, Transfer8) {}
//        }
//        StartDependentFnNoPri() {
//            IO(Decode16, 0x2E0, 0x2E0, 1, 8)
//            IRQ(Level,ActiveLow,Shared) {3,4,5,6,7,10,11,12}
//            DMA(Compatibility, NotBusMaster, Transfer8) {}
//        }
//        EndDependentFn()
//    })
//RayWu, REMOVE 2014/09/03 <<
//======================================================================
//Following commented line will be included in PciTree.asl 
//} //End Of UAR3
//======================================================================
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
