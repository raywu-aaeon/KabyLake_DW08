
Scope(\_SB_.PCI0.RP01) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)
    Method(_INI)
    {
      Store (LTR1, LTRZ)
      Store (PML1, LMSL)
      Store (PNL1, LNSL)
      Store (OBF1, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #01"

Scope(\_SB_.PCI0.RP02) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTR2, LTRZ)
      Store (PML2, LMSL)
      Store (PNL2, LNSL)
      Store (OBF2, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #02"

Scope(\_SB_.PCI0.RP03) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTR3, LTRZ)
      Store (PML3, LMSL)
      Store (PNL3, LNSL)
      Store (OBF3, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #03"

Scope(\_SB_.PCI0.RP04) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTR4, LTRZ)
      Store (PML4, LMSL)
      Store (PNL4, LNSL)
      Store (OBF4, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #04"

Scope(\_SB_.PCI0.RP05) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTR5, LTRZ)
      Store (PML5, LMSL)
      Store (PNL5, LNSL)
      Store (OBF5, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #05"

Scope(\_SB_.PCI0.RP06) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTR6, LTRZ)
      Store (PML6, LMSL)
      Store (PNL6, LNSL)
      Store (OBF6, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #06"

Scope(\_SB_.PCI0.RP07) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTR7, LTRZ)
      Store (PML7, LMSL)
      Store (PNL7, LNSL)
      Store (OBF7, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #07"

Scope(\_SB_.PCI0.RP08) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTR8, LTRZ)
      Store (PML8, LMSL)
      Store (PNL8, LNSL)
      Store (OBF8, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #08"

Scope(\_SB_.PCI0.RP09) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTR9, LTRZ)
      Store (PML9, LMSL)
      Store (PNL9, LNSL)
      Store (OBF9, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #09"

Scope(\_SB_.PCI0.RP10) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRA, LTRZ)
      Store (PMLA, LMSL)
      Store (PNLA, LNSL)
      Store (OBFA, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #10"

Scope(\_SB_.PCI0.RP11) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRB, LTRZ)
      Store (PMLB, LMSL)
      Store (PNLB, LNSL)
      Store (OBFB, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #11"

Scope(\_SB_.PCI0.RP12) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRC, LTRZ)
      Store (PMLC, LMSL)
      Store (PNLC, LNSL)
      Store (OBFC, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #12"

Scope(\_SB_.PCI0.RP13) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRD, LTRZ)
      Store (PMLD, LMSL)
      Store (PNLD, LNSL)
      Store (OBFD, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #13"

Scope(\_SB_.PCI0.RP14) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRE, LTRZ)
      Store (PMLE, LMSL)
      Store (PNLE, LNSL)
      Store (OBFE, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #14"

Scope(\_SB_.PCI0.RP15) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRF, LTRZ)
      Store (PMLF, LMSL)
      Store (PNLF, LNSL)
      Store (OBFF, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #15"

Scope(\_SB_.PCI0.RP16) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRG, LTRZ)
      Store (PMLG, LMSL)
      Store (PNLG, LNSL)
      Store (OBFG, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #16"

Scope(\_SB_.PCI0.RP17) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRH, LTRZ)
      Store (PMLH, LMSL)
      Store (PNLH, LNSL)
      Store (OBFH, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #17"

Scope(\_SB_.PCI0.RP18) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRI, LTRZ)
      Store (PMLI, LMSL)
      Store (PNLI, LNSL)
      Store (OBFI, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #18"

Scope(\_SB_.PCI0.RP19) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRJ, LTRZ)
      Store (PMLJ, LMSL)
      Store (PNLJ, LNSL)
      Store (OBFJ, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #19"

Scope(\_SB_.PCI0.RP20) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRK, LTRZ)
      Store (PMLK, LMSL)
      Store (PNLK, LNSL)
      Store (OBFK, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #20"

Scope(\_SB_.PCI0.RP21) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRL, LTRZ)
      Store (PMLL, LMSL)
      Store (PNLL, LNSL)
      Store (OBFL, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #21"

Scope(\_SB_.PCI0.RP22) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRM, LTRZ)
      Store (PMLM, LMSL)
      Store (PNLM, LNSL)
      Store (OBFM, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #22"

Scope(\_SB_.PCI0.RP23) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRN, LTRZ)
      Store (PMLN, LMSL)
      Store (PNLN, LNSL)
      Store (OBFN, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #23"

Scope(\_SB_.PCI0.RP24) {
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(LTRZ, 0)
    Name(OBFZ, 0)

    Name(LMSL, 0)
    Name(LNSL, 0)    
    Method(_INI)
    {
      Store (LTRO, LTRZ)
      Store (PMLO, LMSL)
      Store (PNLO, LNSL)
      Store (OBFO, OBFZ)
    }
    Method(_PRW, 0) { Return(GPRW(0x69, 4)) }
} // end "PCIE Root Port #24"


