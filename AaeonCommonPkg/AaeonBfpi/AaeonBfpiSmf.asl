#if ASL_WMI_SMF_SUPPORT
  Scope(\_SB.BFPI) {
    //---------------------------------------------------------------------------
    // Name: SMFO
    //
    // Description:  SMF Operation
    //
    // Input:
    //   Arg0 : Sub-function number
    //   Arg1 : IIA0, Input: IntegerArgument
    //   Arg2 : IIA1, Input: IntegerArgument
    //
    // Output:
    //   According to sub-function.
    //
    //---------------------------------------------------------------------------
    Method (SMFO, 3){
      If (LEqual(Arg0, 0x0000)) {  // Report capability
        Return(FCRC(Arg1, Arg2))
      }
      If (LEqual(Arg0, 0x0001)) {  // Get FAN mode
        Return(FCGM(Arg1, Arg2))
      }
      If (LEqual(Arg0, 0x0002)) {  // Set FAN mode
        Return(FCSM(Arg1, Arg2))
      }
      If (LEqual(Arg0, 0x0003)) {  // Get control point configuration
        Return(FCGP(Arg1, Arg2))
      }
      If (LEqual(Arg0, 0x0004)) {  // Set control point configuration
        Return(FCSP(Arg1, Arg2))
      }
      Return(ASL_BFPI_INVALID_PARAMETER)
    }

    //---------------------------------------------------------------------------
    // Name: FCRC
    //
    // Description:  SMF FAN Controller report capability
    //   Arg0 : IIA0, Select capability data to return
    //   Arg1 : IIA1, Basically not used.
    //
    // Output:
    //   According to sub-function.
    //
    //---------------------------------------------------------------------------
    Method (FCRC, 2){     
      And (Arg0, 0xFF, Local0) // Input: Interger - Select capability data to return

      If (LEqual(Local0, 0x00)) {  // Report capability in JSON format
          Store (FCRG, \_SB.BFPI.WDBF)
          Return(ASL_BFPI_SUCCESS)
      }
      Else {
        Decrement (Local0)
        Store ("NULL", Local1)
        If (LLess(Local0, SizeOf(TFSD))) {
          Store (DerefOf(Index(TFSD, Local0)), Local1)
        }
        If (LEqual(Local1, "NULL")) {
          // Not in table, use string in WDBF instead.
          Store (ToString (\_SB.BFPI.WDBF), Local1)
        }
        Store (FCRF(Local1), Local1)
        IF(LEqual (Local1, "NULL")) { Return (ASL_BFPI_INVALID_PARAMETER) }
        Store (Local1, \_SB.BFPI.WDBF)
        Return(ASL_BFPI_SUCCESS)
      }

      Return(ASL_BFPI_INVALID_PARAMETER)
    }

    //---------------------------------------------------------------------------
    // Name: FCRG
    //
    // Description:  SMF FAN Controller report general capability
    //
    // Output:
    //   According to sub-function.
    //
    //---------------------------------------------------------------------------
    Method (FCRG, 0){
      //Get base prefix
      Store ("{ \"Label\":\"SMF\", \"Instance Number\": ", Local0)
      Store (ONE, Local1)
      Concatenate (Local0, FCRN, Local0)
      Concatenate (Local0, ", \"Instance Data\":[", Local0)

      // Concatenate all supported FAN data
#if ASL_SMF_TYPE_OEM_SUPPORT
      // OEM SMF controlling method can be implemented externally in project level.
      If (LEqual(Local1, ZERO)) { Concatenate (Local0, ",", Local0) } Else { Decrement(Local1) }  // Start to append comma from secondary.
      Concatenate (Local0, ASL_OEM_SMF_RL_METHOD, Local0)
#endif
#if ASL_SMF_TYPE_SOC_SUPPORT
      // SoC SMF controlling method should be implemented externally in platform level.
      If (LEqual(Local1, ZERO)) { Concatenate (Local0, ",", Local0) } Else { Decrement(Local1) }  // Start to append comma from secondary.
      Concatenate (Local0, \_SB.SFRL, Local0)
#endif
#if ASL_SMF_TYPE_SIO1_SUPPORT
      If (LEqual(Local1, ZERO)) { Concatenate (Local0, ",", Local0) } Else { Decrement(Local1) }  // Start to append comma from secondary.
      Concatenate (Local0, \_SB.PCI0.LPCB.SIO1.F1RL, Local0)
#endif
#if ASL_SMF_TYPE_SIO2_SUPPORT
      If (LEqual(Local1, ZERO)) { Concatenate (Local0, ",", Local0) } Else { Decrement(Local1) }  // Start to append comma from secondary.
      Concatenate (Local0, \_SB.PCI0.LPCB.SIO2.F2RL, Local0)
#endif
#if ASL_SMF_TYPE_SIO3_SUPPORT
      If (LEqual(Local1, ZERO)) { Concatenate (Local0, ",", Local0) } Else { Decrement(Local1) }  // Start to append comma from secondary.
      Concatenate (Local0, \_SB.PCI0.LPCB.SIO3.F3RL, Local0)
#endif
#if ASL_SMF_TYPE_EC1_SUPPORT
      If (LEqual(Local1, ZERO)) { Concatenate (Local0, ",", Local0) } Else { Decrement(Local1) }  // Start to append comma from secondary.
      Concatenate (Local0, \_SB.PCI0.LPCB.EC01.E1RL, Local0)
#endif
#if ASL_SMF_TYPE_EC2_SUPPORT
      If (LEqual(Local1, ZERO)) { Concatenate (Local0, ",", Local0) } Else { Decrement(Local1) }  // Start to append comma from secondary.
      Concatenate (Local0, \_SB.PCI0.LPCB.EC02.E2RL, Local0)
#endif

      // Append suffix
      Concatenate (Local0, "]}", Local0)

      Return(Local0)
    }

    //---------------------------------------------------------------------------
    // Name: FCRN
    //
    // Description:  FAN Controller return number
    //
    // Output:
    //   Integer - Total senser number for specified sensor type
    //
    //---------------------------------------------------------------------------
    Method (FCRN, 0){
      Store (0, Local0)

      // Concatenate all supported sensor name for requested type
#if ASL_SMF_TYPE_OEM_SUPPORT
      // OEM SMF controlling method can be implemented externally in project level.
      Add (Local0, ASL_OEM_SMF_RN_METHOD, Local0)
#endif
#if ASL_SMF_TYPE_SOC_SUPPORT
      // SoC SMF controlling method should be implemented externally in platform level.
      Add (Local0, \_SB.SFRN, Local0)
#endif
#if ASL_SMF_TYPE_SIO1_SUPPORT
      Add (Local0, \_SB.PCI0.LPCB.SIO1.F1RN, Local0)
#endif
#if ASL_SMF_TYPE_SIO2_SUPPORT
      Add (Local0, \_SB.PCI0.LPCB.SIO2.F2RN, Local0)
#endif
#if ASL_SMF_TYPE_SIO3_SUPPORT
      Add (Local0, \_SB.PCI0.LPCB.SIO3.F3SN, Local0)
#endif
#if ASL_SMF_TYPE_EC1_SUPPORT
      Add (Local0, \_SB.PCI0.LPCB.EC01.E1RN, Local0)
#endif
#if ASL_SMF_TYPE_EC2_SUPPORT
      Add (Local0, \_SB.PCI0.LPCB.EC02.E2RN, Local0)
#endif

      Return(Local0)
    }

    //---------------------------------------------------------------------------
    // Name: FCRF
    //
    // Description:  SMF FAN Controller report functionality
    //   Arg0 : String - FAN name to get smart FAN functionality
    //
    // Output:
    //   According to sub-function.
    //
    //---------------------------------------------------------------------------
    Method (FCRF, 1){
      Store ("NULL", Local0)
#if ASL_SMF_TYPE_OEM_SUPPORT
      // OEM SMF controlling method can be implemented externally in project level.
      Store (ASL_OEM_SMF_RC_METHOD, Local0)
      If (LNotEqual(Local0, "NULL")) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SOC_SUPPORT
      // SoC SMF controlling method should be implemented externally in platform level.
      Store (\_SB.SFRC(Arg0), Local0)
      If (LNotEqual(Local0, "NULL")) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO1_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO1.F1RC(Arg0), Local0)
      If (LNotEqual(Local0, "NULL")) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO2_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO2.F2RC(Arg0), Local0)
      If (LNotEqual(Local0, "NULL")) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO3_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO3.F3RC(Arg0), Local0)
      If (LNotEqual(Local0, "NULL")) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC1_SUPPORT
      Store (\_SB.PCI0.LPCB.EC01.E1RC(Arg0), Local0)
      If (LNotEqual(Local0, "NULL")) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC2_SUPPORT
      Store (\_SB.PCI0.LPCB.EC02.E2RC(Arg0), Local0)
      If (LNotEqual(Local0, "NULL")) { Return (Local0) }
#endif
      Return (Local0)
    }

    //------------------------------------------------------------------------
    // Table:    TFSD
    // Description:  Look up table for standard name of Smart FAN
    //-------------------------------------------------------------------------
    Name(TFSD, Package() {
        ASL_STD_FAN_STRING_LIST
    })

    //---------------------------------------------------------------------------
    // Name: FCGM
    //
    // Description:  SMF FAN control get mode
    //   Arg0 : IIA0, SMF instance to get.
    //   Arg1 : IIA1, Basically not used.
    //
    // Output:
    //   Interger: Remained timeout value
    //   ASL_BFPI_INVALID_PARAMETER
    //
    //---------------------------------------------------------------------------
    Method (FCGM, 2){
      And (Arg0, 0xF, Local1) // [3:0] - SMF Instance to operate

      Store ("NULL", Local0)
      If (LLess(Local1, SizeOf(TFSD))) {
        Store (DerefOf(Index(TFSD, Local1)), Local0)
      }
      If (LEqual(Local0, "NULL")) {
        // Not in table, use string in WDBF instead.
        Store (ToString (WDBF), Local0)
      }
  
      Return (AFGM(Local0))
    }

    //---------------------------------------------------------------------------
    // Name: AFGM
    //
    // Description:  AAEON FAN control get mode
    //   Arg0 : SMF name to get
    //
    // Output:
    //   Integer :
    //     Bit[3:0]: Fan Mode
    //       0 - Manual mode
    //       1 - Linear mode
    //     Bit[7:4]: Reserved Bits
    //     Bit[15:8]: (valid only for manual mode) Duty cycle for manual mode
    //       0 - 0% duty
    //       X - (X/255)% duty
    //       255 - 100% duty
    //     BFPI_NOT_SUPPORTED : No matched fan found.
    //
    //---------------------------------------------------------------------------
    Method (AFGM, 1) {
      Store (ASL_BFPI_NOT_SUPPORTED, Local0)
#if ASL_SMF_TYPE_OEM_SUPPORT
      // OEM SMF controlling method can be implemented externally in project level.
      Store (ASL_OEM_SMF_GM_METHOD, Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SOC_SUPPORT
      // SoC SMF controlling method should be implemented externally in platform level.
      Store (\_SB.SFGM(Arg0), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO1_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO1.F1GM(Arg0), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO2_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO2.F2GM(Arg0), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO3_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO3.F3GM(Arg0), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC1_SUPPORT
      Store (\_SB.PCI0.LPCB.EC01.E1GM(Arg0), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC2_SUPPORT
      Store (\_SB.PCI0.LPCB.EC02.E2GM(Arg0), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
      Return (Local0)
    }
    
    //---------------------------------------------------------------------------
    // Name: FCSM
    //
    // Description:  SMF FAN control set mode
    //   Arg0 : IIA0, SMF instance to set.
    //   Arg1 : IIA1, Basically not used.
    //
    // Output:
    //   BFPI_STATUS
    //
    //---------------------------------------------------------------------------
    Method (FCSM, 2){
      And (Arg0, 0xF, Local1) // [3:0] - SMF Instance to operate
      And (ShiftRight(Arg0,4), 0xF, Local2) // [7:4] - SMF mode to set
      And (ShiftRight(Arg0,16), 0xFF, Local3) // [23:16] - SMF Manual PWM to set

      Store ("NULL", Local0)
      If (LLess(Local1, SizeOf(TFSD))) {
        Store (DerefOf(Index(TFSD, Local1)), Local0)
      }
      If (LEqual(Local0, "NULL")) {
        // Not in table, use string in WDBF instead.
        Store (ToString (WDBF), Local0)
      }
  
      Return (AFSM(Local0, Local2, Local3))
    }

    //---------------------------------------------------------------------------
    // Name: AFSM
    //
    // Description:  AAEON FAN control set mode
    //   Arg0 : SMF name to set
    //   Arg1 : SMF mode to set
    //       0 - Manual mode
    //       1 - Linear mode
    //   Arg2 : Duty cycle for manual mode
    //       0 - 0% duty
    //       X - (X/255)% duty
    //       255 - 100% duty
    //
    // Output:
    //   BFPI_STATUS
    //
    //---------------------------------------------------------------------------
    Method (AFSM, 3){
      Store (ASL_BFPI_NOT_SUPPORTED, Local0)
#if ASL_SMF_TYPE_OEM_SUPPORT
      // OEM SMF controlling method can be implemented externally in project level.
      Store (ASL_OEM_SMF_SM_METHOD, Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SOC_SUPPORT
      // SoC SMF controlling method should be implemented externally in platform level.
      Store (\_SB.SFSM(Arg0, Arg1, Arg2), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO1_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO1.F1SM(Arg0, Arg1, Arg2), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO2_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO2.F2SM(Arg0, Arg1, Arg2), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO3_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO3.F3SM(Arg0, Arg1, Arg2), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC1_SUPPORT
      Store (\_SB.PCI0.LPCB.EC01.E1SM(Arg0, Arg1, Arg2), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC2_SUPPORT
      Store (\_SB.PCI0.LPCB.EC02.E2SM(Arg0, Arg1, Arg2), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
      Return (ASL_BFPI_NOT_SUPPORTED)
    }
  
    //---------------------------------------------------------------------------
    // Name: FCGP
    //
    // Description:  SMF FAN control get control point configuration
    //   Arg0 : IIA0, SMF instance to get.
    //   Arg1 : IIA1, Basically not used.
    //
    // Output:
    //   Interger: Remained timeout value
    //   ASL_BFPI_INVALID_PARAMETER
    //
    //---------------------------------------------------------------------------
    Method (FCGP, 2){
      And (Arg0, 0xF, Local1) // [3:0] - SMF Instance to operate
      And (ShiftRight(Arg0,4), 0xF, Local2) // [7:4] - SMF Control point number

      Store ("NULL", Local0)
      If (LLess(Local1, SizeOf(TFSD))) {
        Store (DerefOf(Index(TFSD, Local1)), Local0)
      }
      If (LEqual(Local0, "NULL")) {
        // Not in table, use string in WDBF instead.
        Store (ToString (WDBF), Local0)
      }
  
      Return (AFGP(Local0, Local2))
    }

    //---------------------------------------------------------------------------
    // Name: AFGP
    //
    // Description:  AAEON FAN control get control point configuration
    //   Arg0 : SMF name to get
    //   Arg1 : control point to get
    //     0 - lowest point
    //     1 - highest point
    //     2 - secondary low point
    //     3 - third low point
    //
    // Output:
    //   Integer :
    //     Bit[7:0]: DutyCycle coordinate for this pont
    //       0 - 0% duty
    //       X - (X/255)% duty
    //       255 - 100% duty
    //     Bit[16:8] Temperature coordinate for this pont
    //       0~255
    //
    //---------------------------------------------------------------------------
    Method (AFGP, 2) {
      Store (ASL_BFPI_NOT_SUPPORTED, Local0)
#if ASL_SMF_TYPE_OEM_SUPPORT
      // OEM SMF controlling method can be implemented externally in project level.
      Store (ASL_OEM_SMF_GP_METHOD, Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SOC_SUPPORT
      // SoC SMF controlling method should be implemented externally in platform level.
      Store (\_SB.SFGP(Arg0, Arg1), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO1_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO1.F1GP(Arg0, Arg1), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO2_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO2.F2GP(Arg0, Arg1), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO3_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO3.F3GP(Arg0, Arg1), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC1_SUPPORT
      Store (\_SB.PCI0.LPCB.EC01.E1GP(Arg0, Arg1), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC2_SUPPORT
      Store (\_SB.PCI0.LPCB.EC02.E2GP(Arg0, Arg1), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
      Return (ASL_BFPI_NOT_SUPPORTED)
    }
    
    //---------------------------------------------------------------------------
    // Name: FCSP
    //
    // Description:  SMF FAN control set control point configuration
    //   Arg0 : IIA0, SMF instance to set.
    //   Arg1 : IIA1, Basically not used.
    //
    // Output:
    //   BFPI_STATUS
    //
    //---------------------------------------------------------------------------
    Method (FCSP, 2){
      And (Arg0, 0xF, Local1) // [3:0] - SMF Instance to operate
      And (ShiftRight(Arg0,4), 0xF, Local2) // [7:4] - SMF Control point number
      And (ShiftRight(Arg0,8), 0xFF, Local3) // [15:8] - SMF PWM coordinate
      And (ShiftRight(Arg0,16), 0xFF, Local4) // [23:16] - SMF Temperature coordinate

      Store ("NULL", Local0)
      If (LLess(Local1, SizeOf(TFSD))) {
        Store (DerefOf(Index(TFSD, Local1)), Local0)
      }
      If (LEqual(Local0, "NULL")) {
        // Not in table, use string in WDBF instead.
        Store (ToString (WDBF), Local0)
      }
  
      Return (AFSP(Local0, Local2, Local3, Local4))
    }

    //---------------------------------------------------------------------------
    // Name: AFSP
    //
    // Description:  AAEON FAN control set control point configuration
    //   Arg0 : SMF name to set
    //   Arg1 : Point number to configre.
    //     0 - lowest point
    //     1 - highest point
    //     2 - secondary low point
    //     3 - third low point
    //   Arg2 : DutyCycle coordinate for this pont
    //     0 - 0% duty
    //     X - (X/255)% duty
    //     255 - 100% duty
    //   Arg3 : Temperature coordinate for this pont
    //     0~255
    //
    // Output:
    //   BFPI_STATUS
    //
    //---------------------------------------------------------------------------
    Method (AFSP, 4){
      Store (ASL_BFPI_NOT_SUPPORTED, Local0)
#if ASL_SMF_TYPE_OEM_SUPPORT
      // OEM SMF controlling method can be implemented externally in project level.
      Store (ASL_OEM_SMF_SP_METHOD, Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SOC_SUPPORT
      // SoC SMF controlling method should be implemented externally in platform level.
      Store (\_SB.SFSP(Arg0, Arg1, Arg2, Arg3), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO1_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO1.F1SP(Arg0, Arg1, Arg2, Arg3), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO2_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO2.F2SP(Arg0, Arg1, Arg2, Arg3), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_SIO3_SUPPORT
      Store (\_SB.PCI0.LPCB.SIO3.F3SP(Arg0, Arg1, Arg2, Arg3), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC1_SUPPORT
      Store (\_SB.PCI0.LPCB.EC01.E1SP(Arg0, Arg1, Arg2, Arg3), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
#if ASL_SMF_TYPE_EC2_SUPPORT
      Store (\_SB.PCI0.LPCB.EC02.E2SP(Arg0, Arg1, Arg2, Arg3), Local0)
      If (LNotEqual(Local0, ASL_BFPI_NOT_SUPPORTED)) { Return (Local0) }
#endif
      Return (ASL_BFPI_NOT_SUPPORTED)
    }
  
  }  // Scope(\_SB.BFPI)
#endif