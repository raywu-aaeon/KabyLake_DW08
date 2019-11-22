Scope(\_SB.BFPI) {

#if ASL_WDT_TYPE_OEM_SUPPORT
//---------------------------------------------------------------------------
// Name: OWRC
//
// Description:  OEM WDT report capability - Max timeout
//
// Output:
//   Interger: Max timeout value in ms
//
//---------------------------------------------------------------------------
    Method (OWRC, 0) {
        Return (ASL_BFPI_NOT_SUPPORTED)  // TODO: Porting required.
    }

//---------------------------------------------------------------------------
// Name: OWGT
//
// Description:  OEM WDT Get timeout
//
// Output:
//   Interger: Remained timeout value in ms
//   ASL_BFPI_INVALID_PARAMETER
//
//---------------------------------------------------------------------------
    Method (OWGT, 0) {
        Return (ASL_BFPI_NOT_SUPPORTED)  // TODO: Porting required.
    }

//---------------------------------------------------------------------------
// Name: OWST
//
// Description:  OEM WDT set watchdog timeout value and start/stop watchdog
//   Arg0 : Timeout value in ms
//            0: stop watchdog
//            other: start watchdog with value as timeout
//
// Output:
//   BFPI_STATUS
//
// Note: 
//   1. BIOS should have already configure WDT output during POST.
//      So we just set counter here.
//   2. Follow EAPI specification:
//      If the hardware implementation of
//      the watchdog timer does not allow a setting at the exact time selected, the
//      EAPI selects the next possible longer timing.
//
//---------------------------------------------------------------------------
    Method (OWST, 1){
        // Validate input
        If (LGreater(Arg0, ASL_WDT_MAX_TIMEOUT)) { 
            Return (ASL_BFPI_INVALID_PARAMETER)
        }

        Return (ASL_BFPI_NOT_SUPPORTED)  // TODO: Porting required.
    }
#endif  // ASL_WDT_TYPE_OEM_SUPPORT

#if ASL_HWM_TYPE_OEM_SUPPORT
//---------------------------------------------------------------------------
// Name: OHSN
//
// Description:  OEM HWM report sensor number
//   Arg0 : Sensor Type
//     0 : Temperature
//     1 : Fan
//     2 : Voltage
//
// Output:
//   Integer - Total senser number for specified sensor type
//   
//---------------------------------------------------------------------------
    Method (OHSN, 1){
        Store (0, Local0)
        //If (LEqual(Arg0, 0)) {
        //    // Temperature
        //    Store (ASL_OEM_TEMP_SENSOR_NUMBER, Local0)
        //} ElseIf (LEqual(Arg0, 1)) {
        //    // Fan
        //    Store (ASL_OEM_FAN_SENSOR_NUMBER, Local0)
        //} ElseIf (LEqual(Arg0, 2)) {
        //    // Voltage
        //    Store (ASL_OEM_VOLT_SENSOR_NUMBER, Local0)
        //}
        Return (Local0)
    }

//---------------------------------------------------------------------------
// Name: OHSL
//
// Description:  OEM HWM report sensor name list
//   Arg0 : Sensor Type
//     0 : Temperature
//     1 : Fan
//     2 : Voltage
//
// Output:
//   String - List of supported sensor name for specified sensor type
//   
//---------------------------------------------------------------------------
    Method (OHSL, 1){
        Store ("", Local0)
        Store (ONE, Local1)

        If (LEqual(Arg0, 0)) {
            // Temperature
            // Example 1: Add code here in ASL
            // Store ("\"Name 1\", \"Name 2\", \"Name 3\"", Local0)
            // Example 2: use token
            // Concatenate (Local0, Concatenate ("\"", Concatenate (ASL_OEM_TEMP_SENSOR_STRING1, "\"", ), ), Local0)
            // Concatenate (Local0, ",", Local0)
            // Concatenate (Local0, Concatenate ("\"", Concatenate (ASL_OEM_TEMP_SENSOR_STRING2, "\"", ), ), Local0)
            // Concatenate (Local0, ",", Local0)
            // Concatenate (Local0, Concatenate ("\"", Concatenate (ASL_OEM_TEMP_SENSOR_STRING3, "\"", ), ), Local0)
        } ElseIf (LEqual(Arg0, 1)) {
            // Fan
            // ...
        } ElseIf (LEqual(Arg0, 2)) {
            // Voltage
            // ...
        Return (Local0)
    }

//---------------------------------------------------------------------------
// Name: OHRS
//
// Description:  OEM HWM read sensor
//   Arg0 : Sensor Type
//     0 : Temperature
//     1 : Fan
//     2 : Voltage
//   Arg1 : Sensor Name
//
// Output:
//   Sensor reading accoring to Arg0
//     Temperature in mini-degree-C
//     Fan in RPM
//     Voltage in mini-volt
//
//---------------------------------------------------------------------------
    Method (OHRS, 2){
        Store (ASL_BFPI_NOT_SUPPORTED, local0)
        If (LEqual(Arg0, 0)) {
            // Temperature
            //If (LEqual(Arg1, "Name for sensor 1")) {  // Name match for OEM Temperature sensor 1
            //    ...
            //}
            //ElseIf (LEqual(Arg1, "Name for sensor 2")) {  // Name match for OEM Temperature sensor 2
            //    ...
        } ElseIf (LEqual(Arg0, 1)) {
            // Fan
            //If (LEqual(Arg1, "Name for sensor 1")) {  // Name match for OEM FAN sensor 1
            //    ...
            //}
            //ElseIf ...
        } ElseIf (LEqual(Arg0, 2)) {
            // Voltage
            //If (LEqual(Arg1, "Name for sensor 1")) {  // Name match for OEM Voltage sensor 1
            //    ...
            //}
            //ElseIf ...
        }
        Return (local0)
    }
#endif  // ASL_HWM_TYPE_OEM_SUPPORT

#if ASL_SMF_TYPE_OEM_SUPPORT
//---------------------------------------------------------------------------
// Name: OFRN
//
// Description:  OEM SMF report FAN number
//
// Output:
//   Integer - Total smart FAN number supported by this OEM controller
//   
//---------------------------------------------------------------------------
    Method (F1RN, 0){
        Return (0)
    }

//---------------------------------------------------------------------------
// Name: OFRL
//
// Description:  SIO SMF Report name list
//
// Input: N/A
//
// Output:
//   String : A list of FAN names that supported Smart FAN from this OEM controller, seperated by comma.
//
//---------------------------------------------------------------------------
    Method (F1RL, 0){
        Store ("", local0)
        // Example 1: Add code here in ASL
        // Store ("\"Name 1\", \"Name 2\", \"Name 3\"", Local0)
        // Example 2: use token
        // Concatenate (Local0, Concatenate ("\"", Concatenate (ASL_OEM_FAN_SENSOR1_STRING, "\"", ), ), Local0)
        // Concatenate (Local0, ",", Local0)
        // Concatenate (Local0, Concatenate ("\"", Concatenate (ASL_OEM_FAN_SENSOR2_STRING, "\"", ), ), Local0)
        // Concatenate (Local0, ",", Local0)
        // Concatenate (Local0, Concatenate ("\"", Concatenate (ASL_OEM_FAN_SENSOR3_STRING, "\"", ), ), Local0)
        Return (local0)
    }

//------------------------------------------------------------------------
// Table:    OTFT
// Description:  Look up table for target temperature of OEM FAN.
//   Order should be same as register definition.
//   Temperature name should follow STD_XXX_TEMP tokens in AaeonIoPkg.sdl
//-------------------------------------------------------------------------
    Name(OTFT, Package() {
        // Just example
        "CPU Temperature",
        "System Temperature",
        "System Temperature 2",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
    })

//---------------------------------------------------------------------------
// Name: OFRC
//
// Description:  OEM SMF Report capability
//
// Input:
//   Arg0 : String -
//     "General" : Return a list of FAN names that supported Smart FAN from this OEM controller, seperated by comma.
//     FAN name string : Return correspond Smart FAN functionality in JSON format. Return "NULL" is no FAN matched the name.
//
// Output:
//   String : Content depends on Input. Refer to Input.
//
//---------------------------------------------------------------------------
    Method (F1RC, 1){
//        Store ("", local0)
//        Store (ONE, local1)
//
//        If (LEqual(Arg0, "General")) {
//            // Return a list of FAN names that supported Smart FAN from this controller, seperated by comma.
//
//            //Example 1: Add code here in ASL
//            Store ("\"Name 1\", \"Name 2\", \"Name 3\"", Local0)
//            //Example 2: use token
//            Concatenate (Local0, Concatenate ("\"", Concatenate (ASL_OEM_FAN_SENSOR_STRING1, "\"", ), ), Local0)
//            Concatenate (Local0, ",", Local0)
//            Concatenate (Local0, Concatenate ("\"", Concatenate (ASL_OEM_FAN_SENSOR_STRING2, "\"", ), ), Local0)
//            // Add if more FAN supported.
//            // ...
//            Return (local0)
//        }
//
//        Store ("", local2)
//
//        // Otherwise, Return Smart FAN functionality in JSON format.
//        If (LEqual(Arg0, "Name for FAN 1")) {
//            // Example for a FAN with fixed temperature target.
//            Return ("{\"Name\":\"Name for FAN 1",\"Temperature Point\":4,\"Temperature Target\":\"CPU Temperature\",\"Supported Mode\":[\"Manual\",\"Linear\"]}")
//        }
//        If (LEqual(Arg0, "Name for FAN 2")) {
//            // Example for reading temperature target from register.
//            Store ("{\"Name\":\"Name for FAN 2",\"Temperature Point\":4,\"Temperature Target\":\"", Local0)
//            Store (DerefOf(Index(OTFT, XXXX)), Local2)  // OTFT: OEM look-up table for FAN target; XXXX: Retrived Register Value. They should be define separately.
//            Concatenate (Local0, Local2, Local0)
//            Concatenate (Local0, "\",\"Supported Mode\":[\"Manual\",\"Linear\"]}", Local0)
//            Return (Local0)
//        }
//        // Add if more FAN supported.
//        // ...
        Return ("NULL")
    }

//---------------------------------------------------------------------------
// Name: OFGM
//
// Description:  OEM SMF get mode
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
//     BFPI_NOT_SUPPORTED : No FAN matches name provided
//
//---------------------------------------------------------------------------
    Method (OFGM, 1){
        Store (ASL_BFPI_NOT_SUPPORTED, local0)
        // Retrive data to local0
        //If (LEqual(Arg0, "Name for FAN 1")) {  // Name match for OEM FAN 1, should be same as sensor name.
        //    ...
        //}
        //ElseIf (LEqual(Arg0, "Name for FAN 2")) {  // Name match for OEM FAN 2, should be same as sensor name.
        //    ...
        //}
        // Add if more FAN supported.
        // ...
        Return (local0)
    }

//---------------------------------------------------------------------------
// Name: OFSM
//
// Description:  OEM SMF set mode
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
//     BFPI_NOT_SUPPORTED : No FAN matches name provided
//     BFPI_SUCCESS
//
//---------------------------------------------------------------------------
    Method (AFSM, 3){
        Store (ASL_BFPI_NOT_SUPPORTED, local0)
        //If (LEqual(Arg0, "Name for FAN 1")) {  // Name match for OEM FAN 1, should be same as sensor name.
        //    If (LEqual(Arg1, 0)) {
        //        // Manual mode
        //        ...
        //    }
        //    Else {
        //        // Linear mode
        //        ...
        //    }
        //    Store (ASL_BFPI_SUCCESS, local0)
        //}
        //ElseIf (LEqual(Arg0, "Name for FAN 2")) {  // Name match for OEM FAN 2, should be same as sensor name.
        //    If (LEqual(Arg1, 0)) {
        //        // Manual mode
        //        ...
        //    }
        //    Else {
        //        // Linear mode
        //        ...
        //    }
        //    Store (ASL_BFPI_SUCCESS, local0)
        //}
        // Add if more FAN supported.
        // ...
        Return (local0)
    }

//---------------------------------------------------------------------------
// Name: OFGP
//
// Description:  OEM SMF control get control point configuration
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
    Method (OFGP, 2){
        Store (ASL_BFPI_NOT_SUPPORTED, local0)
        // Retrive data to local0
        //If (LEqual(Arg0, "Name for FAN 1")) {  // Name match for OEM FAN 1, should be same as sensor name.
        //    If (LEqual(Arg1, 0)) {   // lowest?
        //        ...
        //    }
        //    ElseIf (LEqual(Arg1, 1)) {   // Highest?
        //        ...
        //    }
        //    ElseIf (LEqual(Arg1, 2)) {   // Secondary low?
        //        ...
        //    }
        //    ...
        //}
        //ElseIf (LEqual(Arg0, "Name for FAN 2")) {  // Name match for OEM FAN 1, should be same as sensor name.
        //    If (LEqual(Arg1, 0)) {   // lowest?
        //        ...
        //    }
        //    ElseIf (LEqual(Arg1, 1)) {   // Highest?
        //        ...
        //    }
        //    ElseIf (LEqual(Arg1, 2)) {   // Secondary low?
        //        ...
        //    }
        //    ...
        //}
        // Add if more FAN supported.
        // ...
        Return (local0)
    }

//---------------------------------------------------------------------------
// Name: OFSP
//
// Description:  OEM SMF control set control point configuration
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
//     BFPI_NOT_SUPPORTED : No FAN matches name provided
//     BFPI_INVALID_PARAMETER : A FAN matches name provided, but is given a wrong control point.
//     BFPI_SUCCESS
//
// Note:
//   If a control point is given with a value bigger than higher control points,
//   the value should also be set to higher point. Vice versa for value smaller.
//---------------------------------------------------------------------------
    Method (OFSP, 4){
        //If (LEqual(Arg0, "Name for FAN 1")) {  // Name match for OEM FAN 1, should be same as sensor name.
        //    If (LEqual(Arg1, 0)) {   // lowest?
        //        ...
        //        // If Lowest > secondary low, update Secondary low point
        //        // If Lowest > Third low, update Third low point
        //        // ...
        //        // If Lowest > Highest, Highest point
        //    }
        //    ElseIf (LEqual(Arg1, 1)) {   // Highest?
        //        ...
        //        // If Highest < secondary low, update Secondary low point
        //        // If Highest < Third low, update Third low point
        //        // ...
        //        // If Highest < Lowest, Highest point
        //    }
        //    ElseIf (LEqual(Arg1, 2)) {   // Secondary low?
        //        ...
        //        // If Secondary low < Lowest, update Lowest point
        //        // ...
        //        // If Secondary low > Third low, update Third low point
        //        // ...
        //        // If Secondary low > Highest, Highest point
        //    }
        //    ...
        //    Else {
        //        Return (ASL_BFPI_INVALID_PARAMETER)
        //    }
        //}
        //ElseIf (LEqual(Arg0, "Name for FAN 2")) {  // Name match for OEM FAN 1, should be same as sensor name.
        //    If (LEqual(Arg1, 0)) {   // lowest?
        //        ...
        //        // If Lowest > secondary low, update Secondary low point
        //        // If Lowest > Third low, update Third low point
        //        // ...
        //        // If Lowest > Highest, Highest point
        //    }
        //    ElseIf (LEqual(Arg1, 1)) {   // Highest?
        //        ...
        //        // If Highest < secondary low, update Secondary low point
        //        // If Highest < Third low, update Third low point
        //        // ...
        //        // If Highest < Lowest, Highest point
        //    }
        //    ElseIf (LEqual(Arg1, 2)) {   // Secondary low?
        //        ...
        //        // If Secondary low < Lowest, update Lowest point
        //        // ...
        //        // If Secondary low > Third low, update Third low point
        //        // ...
        //        // If Secondary low > Highest, Highest point
        //    }
        //    ...
        //    Else {
        //        Return (ASL_BFPI_INVALID_PARAMETER)
        //    }
        //}
        // Add if more FAN supported.
        // ...
        //Else {
            Return (ASL_BFPI_NOT_SUPPORTED)
        //}
        //Return (ASL_BFPI_SUCCESS)
    }
#endif // ASL_SMF_TYPE_OEM_SUPPORT
}