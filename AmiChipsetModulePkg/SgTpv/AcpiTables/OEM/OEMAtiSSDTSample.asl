DefinitionBlock (
        "OemAti.aml",
        "SSDT",
        1,
        "OemRefAti",
        "OemAti",
        0x1000
        ) { 
		
External(ASL_OEM_ATI_SCOPE, DeviceObj)
External(TBDT)

Scope (ASL_OEM_ATI_SCOPE)
{
  Method(ATPX, 2, Serialized)
  {
    //
    // Function 0: Verify PowerXpress Interface
    //
    // Returns the PX interface version and
    // functions supported by PX System BIOS
    //
    If(LEqual(Arg0, 0))
    {
        Name(TMP1, Buffer(256) {0x00})
        CreateWordField ( TMP1, 0, F0SS)
        CreateWordField ( TMP1, 2, F0IV)
        CreateDwordField( TMP1, 4, F0SF)

        Store(0x0008, F0SS)
        Store(0x0001, F0IV)
        // Support only Fun1 and Fun2
        Store(0x00000003, F0SF)

        Return(TMP1)
    }

    //
    // Function 1: Get PowerXpress Parameters
    //
    // Returns various PX related platform parameters
    //
    If(LEqual(Arg0, 1))
    {
        Name(TMP2, Buffer(256) {0x00})
        CreateWordField(TMP2, 0, F1SS)
        CreateDwordField(TMP2, 2, F1VM)
        CreateDwordField(TMP2, 6, F1FG)

        Store(0x000A, F1SS)     //Structure size of return package
        // Bit 7: Indicates that a "dynamic" PX scheme is supported.
        // Bit 9: Indicates that "fixed" scheme is not supported, if set to one.
        // Bit 10: Indicates that full dGPU power off in "dynamic" scheme is supported, if set to one
        // Bit 12: Indicates that discrete graphics can drive display outputs (local dGPU displays are supported), if set to one.
        // Bit 13: Indicates that long idle detection is disabled, if set to one.
        // Bit 14: Indicates that Win Blue "Hybrid Graphics" is required (supported), if set to one.
        // Bit 16: Indicates that Detachable Graphics is supported, if set to one.
        // Bit 17: Indicates that the driver should keep the dGPU active and not go to D3 cold.
        Store(0x00037680, F1VM) // Mask used for valid bit fields
        Store(0x00037680, F1FG) // Actual PX parameters field

        Return(TMP2)
    }
  }

  Method(ATIF, 2, Serialized)
  {
    //
    // Function 0: Verify Interface
    //
    // Returns the interface version and
    // functions/notifications supported by System BIOS
    //
    If(LEqual(Arg0, 0))
    {
        Name(TMP0, Buffer(256) {0x00})
        CreateWordField(TMP0, 0, F0SS)
        CreateWordField(TMP0, 2, F0IV)
        CreateDwordField(TMP0, 4, F0SN)
        CreateDwordField(TMP0, 8, F0SF)

        // Size of return structure=12
        Store(0x000C, F0SS)

        // Interface version
        Store(0x0001, F0IV)

        // Bit 08 - Discrete GPU display connect/disconnect event is supported
        // Bit 09 - Dock notification event is supported
        // Bit 10 - Undock request is supported
        // Bit 11 - Surprise undock notification event is supported
        Store(0xA00, F0SN)
        // Functions supported [Bit n, if set, indicates that Function n+1 is supported]
        Store(ASL_OEM_ATIF_SUPPORT_FUN, F0SF)

        Return(TMP0)
    }
    //
    // Function 1: Get System Parameters
    //
    // Returns various system parameters
    //
    If(LEqual(Arg0, 1))
    {
        Name(TMP1, Buffer(256) {0x0})
        CreateWordField(TMP1, 0, F1SS)
        CreateDwordField(TMP1, 2, F1VF) 
        CreateDwordField(TMP1, 6, F1FG)

        // Size of return structure=10
        Store(0x000A, F1SS)

        // Valid Fields Mask
        Store(0x00000003, F1VF)

        // Flags
        Store(0x00000001, F1FG)  // Notify (VGA, 0x81) is used as a general purpose notification
        Return(TMP1)
    }

    //
    // Function 2: Get System BIOS Requests
    //
    // Reports pending system BIOS requests
    //
    // Invoked whenever driver receives Notify(VGA,0x81) and
    // the Notify is designated as a general purpose notification
    // in the function "Get System Parameters"
    //
    If(LEqual(Arg0, 2))
    {
        Name(TMP2, Buffer(256) {0x0})
        CreateWordField(TMP2, 0, F2SS)
        CreateDwordField(TMP2, 2, F2PR)
        CreateByteField(TMP2, 6, F2EM)
        CreateByteField(TMP2, 7, F2TG)
        CreateByteField(TMP2, 8, F2SI)
        CreateByteField(TMP2, 9, F2FG)
        CreateByteField(TMP2, 10, F2FI)
        CreateByteField(TMP2, 11, F2SP)

        // Size of return structure=11
        Store(0x000B, F2SS)

        Return(TMP2)
    }
    //
    // Function 20: Get Thunderbolt devices information
    //
    // Returns Bus-Device-Function, VendorID-DeviceID data of Thunderbolt devices if available
    //
    If(LEqual(Arg0, 20))
    {
        Name(TM20, Buffer(256) {0x0})
        // Every device will have the following attributes
        // Bus number
        // Device number
        // Function number
        // Vendor ID
        // Device ID
        // Rootport information
        // HostRouter - Upstream port information
        // HostRouter - Downstream port(CIO) information
        // Endpoint - Upstream port information
        // Endpoint - Downstream port information
        // Endpoint information
        OperationRegion(DGFX, SystemMemory, TBDT, 62)
        Field(DGFX, ByteAcc, NoLock, Preserve)
        {
            TBDS, 16,
            RPBN, 16,
            RPDN, 16,
            RPFN, 16,
            RPVI, 16,
            RPDI, 16,
            HUBN, 16,
            HUDN, 16,
            HUFN, 16,
            HUVI, 16,
            HUDI, 16,
            HDBN, 16,
            HDDN, 16,
            HDFN, 16,
            HDVI, 16,
            HDDI, 16,
            EUBN, 16,
            EUDN, 16,
            EUFN, 16,
            EUVI, 16,
            EUDI, 16,
            EDBN, 16,
            EDDN, 16,
            EDFN, 16,
            EDVI, 16,
            EDDI, 16,
            EPBN, 16,
            EPDN, 16,
            EPFN, 16,
            EPVI, 16,
            EPDI, 16
        }
        CreateWordField(TM20, 0, F20S)
        CreateWordField(TM20, 2, P0BN)
        CreateWordField(TM20, 4, P0DN)
        CreateWordField(TM20, 6, P0FN)
        CreateWordField(TM20, 8, P0VI)
        CreateWordField(TM20, 10, P0DI)
        CreateWordField(TM20, 12, P1BN)
        CreateWordField(TM20, 14, P1DN)
        CreateWordField(TM20, 16, P1FN)
        CreateWordField(TM20, 18, P1VI)
        CreateWordField(TM20, 20, P1DI)
        CreateWordField(TM20, 22, P2BN)
        CreateWordField(TM20, 24, P2DN)
        CreateWordField(TM20, 26, P2FN)
        CreateWordField(TM20, 28, P2VI)
        CreateWordField(TM20, 30, P2DI)
        CreateWordField(TM20, 32, P3BN)
        CreateWordField(TM20, 34, P3DN)
        CreateWordField(TM20, 36, P3FN)
        CreateWordField(TM20, 38, P3VI)
        CreateWordField(TM20, 40, P3DI)
        CreateWordField(TM20, 42, P4BN)
        CreateWordField(TM20, 44, P4DN)
        CreateWordField(TM20, 46, P4FN)
        CreateWordField(TM20, 48, P4VI)
        CreateWordField(TM20, 50, P4DI)
        CreateWordField(TM20, 52, P5BN)
        CreateWordField(TM20, 54, P5DN)
        CreateWordField(TM20, 56, P5FN)
        CreateWordField(TM20, 58, P5VI)
        CreateWordField(TM20, 60, P5DI)
        Store(TBDS, F20S)
        Store(RPBN, P0BN)
        Store(RPDN, P0DN)
        Store(RPFN, P0FN)
        Store(RPVI, P0VI)
        Store(RPDI, P0DI)
        Store(HUBN, P1BN)
        Store(HUDN, P1DN)
        Store(HUFN, P1FN)
        Store(HUVI, P1VI)
        Store(HUDI, P1DI)
        Store(HDBN, P2BN)
        Store(HDDN, P2DN)
        Store(HDFN, P2FN)
        Store(HDVI, P2VI)
        Store(HDDI, P2DI)
        Store(EUBN, P3BN)
        Store(EUDN, P3DN)
        Store(EUFN, P3FN)
        Store(EUVI, P3VI)
        Store(EUDI, P3DI)
        Store(EDBN, P4BN)
        Store(EDDN, P4DN)
        Store(EDFN, P4FN)
        Store(EDVI, P4VI)
        Store(EDDI, P4DI)
        Store(EPBN, P5BN)
        Store(EPDN, P5DN)
        Store(EPFN, P5FN)
        Store(EPVI, P5VI)
        Store(EPDI, P5DI)
        Return(TM20)
    }
    #include <OEMAtiAtifF21.asl>
  }
}
}