Scope(\_SB.PCI0.LPCB.H_EC)
{
    Method(BST1,0)
    {
      Name(PKG1,Package() {
        0x00000000, // Battery State.
        0x00000000, // Battery Present Rate. (in mWh)
        0x00000000, // Battery Remaining Capacity. (in mWh)
        0x00000000  // Battery Present Voltage. (in mV)
      })    
      Return(PKG1)
    }
    
   Method(BST2,0)
    {
      Name(PKG2,Package() {
        0x00000000, // Battery State.
        0x00000000, // Battery Present Rate. (in mWh)
        0x00000000, // Battery Remaining Capacity. (in mWh)
        0x00000000  // Battery Present Voltage. (in mV)
      })    
      Return(PKG2)
    }    
}
