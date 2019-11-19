  //
  // AdvancedFeaturesBegin
  //
  // Update the Battery 1 and 2 Stored Capacity and
  // Stored Status.  Battery 0 information is always accurrate.

  If(LEqual(\ECON,1))
  { 

    // Perform update to all Batteries in the System.
    Notify(\_SB.PCI0.LPCB.H_EC.BAT0,0x81)       // Eval BAT0 _BST.
    Notify(\_SB.PCI0.LPCB.H_EC.BAT1,0x81)       // Eval BAT1 _BST.
    Notify(\_SB.PCI0.LPCB.H_EC.BAT2,0x81)       // Eval BAT2 _BST.
  }

