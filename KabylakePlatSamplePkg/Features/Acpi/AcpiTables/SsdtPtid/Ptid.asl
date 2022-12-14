/** @file
  ACPI PTID SSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

//
// Defined as an SSDT to be able to dynamically load based on BIOS
// setup options
//

DefinitionBlock (
  "PTID.aml",
  "SSDT",
  0x02,
  "TrmRef",
  "PtidDevc",
  0x1000
  )

{
  Scope(\_SB)
  {
    // External References to the actual data locations that stores
    // various temperature and power values (either from EC or by
    // other means)
    //

    // Embedded Controller Availability Flag
    External(\_SB.PCI0.LPCB.H_EC.ECAV, IntObj)
    // externs for TSDD
    External(\_PR.DTS1)  // DTS Core 0 Temp
    External(\_PR.DTS2)  // DTS Core 1 Temp
    External(\_PR.DTS3)  // DTS Core 2 Temp
    External(\_PR.DTS4)  // DTS Core 3 Temp
    External(\_PR.PDTS)  // Package DTS Core Temp
    External(\_SB.PCI0.LPCB.H_EC.GTVR)    // CPU GT VR (IMVP) Temperature
    External(\_SB.PCI0.LPCB.H_EC.TSR1)    // Sensor 1
    External(\_SB.PCI0.LPCB.H_EC.TSR2)    // Sensor 2
    External(\_SB.PCI0.LPCB.H_EC.TSR3)    // Sensor 3
    External(\_SB.PCI0.LPCB.H_EC.TSR4)    // Sensor 4
    External(\_SB.PCI0.LPCB.H_EC.TSR5)    // Sensor 5
    External(\_SB.PCI0.LPCB.H_EC.TSR6)    // Sensor 6
    External(\_SB.PCI0.LPCB.H_EC.TSR7)    // Sensor 7
    External(\_SB.PCI0.LPCB.H_EC.PMAX)    // CPU, MCH & PCH Max Temperature
    External(\_SB.PCI0.LPCB.H_EC.PPDT)    // PCH DTS Temperature from PCH
    External(\_SB.PCI0.LPCB.H_EC.PECH)    // CPU PECI reading
    External(\_TZ.TZ00._TMP)              // Thermal Zone 00 Temperature
    External(\_TZ.TZ01._TMP)              // Thermal Zone 01 Temperature

    // externs for PSDD
    External(\_SB.PCI0.LPCB.H_EC.CPUP)    // Cpu Power mW
    External(\_SB.PCI0.LPCB.H_EC.PPWR)    // Platform Power cW(100ths)
    External(\_SB.PCI0.LPCB.H_EC.CPAP)    // Cpu Average Power mW
    External(\_SB.PCI0.LPCB.H_EC.PLAP)    // Platform average power in cW(0.01)
    External(\_SB.PCI0.LPCB.H_EC.B1DC)    // Battery 1 Design Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B1RC)    // Battery 1 Remaining Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B1FC)    // Battery 1 Full Charge Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B1FV)    // Battery 1 Full Resolution Voltage (mV)
    External(\_SB.PCI0.LPCB.H_EC.B1DI)    // Battery 1 Full Resolution Discharge Current (mA)
    External(\_SB.PCI0.LPCB.H_EC.B1CI)    // Battery 1 Full Resolution Charge Current (mA)
    External(\_SB.PCI0.LPCB.H_EC.B2RC)    // Battery 2 Remaining Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B2FC)    // Battery 2 Full Charge Capacity (mWh)
    External(\_SB.PCI0.LPCB.H_EC.B2FV)    // Battery 2 Full Resolution Voltage (mV)
    External(\_SB.PCI0.LPCB.H_EC.B2DI)    // Battery 2 Full Resolution Discharge Current (mA)
    External(\_SB.PCI0.LPCB.H_EC.B2CI)    // Battery 2 Full Resolution Charge Current (mA)
    External(\_SB.PCI0.LPCB.H_EC.BMAX)    // Battery Pack A maximum
    External(\_SB.PCI0.LPCB.H_EC.B2ML)    // Battery Pack B maximum low byte
    External(\_SB.PCI0.LPCB.H_EC.B2MH)    // Battery Pack B maximum high byte

    // externs for OSDD
    External(\_SB.PCI0.LPCB.H_EC.PENV)    // CPU Fan duty cycle
    External(\_SB.PCI0.LPCB.H_EC.CFSP)    // CPU Fan #1 speed
    External(\_SB.PCI0.LPCB.H_EC.CFS2)    // CPU Fan #2 speed
    External(\_SB.PCI0.LPCB.H_EC.MFSP)    // GMCH Fan speed
    // HTS - Host Turbo Status(TBARB offset 50h) from MMIO
    External(\_SB.PCI0.LPCB.H_EC.MMTL)    // MCP Temperature Limit(MTL)
    External(\_SB.PCI0.LPCB.H_EC.MGTD)    // GMCH Turbo Disabled(GTD)
    External(\_SB.PCI0.LPCB.H_EC.MCTD)    // CPU Turbo Disabled(CTD)
    External(\_SB.PCI0.LPCB.H_EC.MPOL)    // Policy Preference(PP)
    External(\_SB.PCI0.LPCB.H_EC.MGPL)    // GMCH Power Limit(GPL)
    External(\_SB.PCI0.LPCB.H_EC.MCPL)    // CPU Power Limit(CPL)
    External(\_SB.PCI0.LPCB.H_EC.MMPL)    // MPC Power Limit(MPL)
    // STS - SMBus Turbo Status(TBARB offset 98h) from MMIO
    External(\_SB.PCI0.LPCB.H_EC.NMTL)    // MCP Temperature Limit(MTL)
    External(\_SB.PCI0.LPCB.H_EC.NGTD)    // GMCH Turbo Disabled(GTD)
    External(\_SB.PCI0.LPCB.H_EC.NCTD)    // CPU Turbo Disabled(CTD)
    External(\_SB.PCI0.LPCB.H_EC.NPOL)    // Policy Preference(PP)
    External(\_SB.PCI0.LPCB.H_EC.NGPL)    // GMCH Power Limit(GPL)
    External(\_SB.PCI0.LPCB.H_EC.NCPL)    // CPU Power Limit(CPL)
    // TC1 - Thermal Compares 1(TBARB offset A8h) from MMIO
    External(\_SB.PCI0.LPCB.H_EC.TMPL)    // MPC Power Limit(MPL)
    // EC Data Buffer for ME SMBus Write Commands
    External(\_SB.PCI0.LPCB.H_EC.MDT0)
    // EC Command register
    External(\_SB.PCI0.LPCB.H_EC.CMDR)
    // EC Buffer to for ACPIPECIPacket
    External(\_SB.PCI0.LPCB.H_EC.PCAD)
    External(\_SB.PCI0.LPCB.H_EC.PEWL)
    External(\_SB.PCI0.LPCB.H_EC.PWRL)
    External(\_SB.PCI0.LPCB.H_EC.PECD)
    External(\_SB.PCI0.LPCB.H_EC.PEHI)
    External(\_SB.PCI0.LPCB.H_EC.PECI)
    External(\_SB.PCI0.LPCB.H_EC.PEPL)
    External(\_SB.PCI0.LPCB.H_EC.PEPM)
    External(\_SB.PCI0.LPCB.H_EC.PWFC)
    External(\_SB.PCI0.LPCB.H_EC.PECC)
    External(\_SB.PCI0.LPCB.H_EC.PDT0)
    External(\_SB.PCI0.LPCB.H_EC.PDT1)
    External(\_SB.PCI0.LPCB.H_EC.PDT2)
    External(\_SB.PCI0.LPCB.H_EC.PDT3)
    External(\_SB.PCI0.LPCB.H_EC.PRFC)
    External(\_SB.PCI0.LPCB.H_EC.PRS0)
    External(\_SB.PCI0.LPCB.H_EC.PRS1)
    External(\_SB.PCI0.LPCB.H_EC.PRS2)
    External(\_SB.PCI0.LPCB.H_EC.PRS3)
    External(\_SB.PCI0.LPCB.H_EC.PRS4)
    External(\_SB.PCI0.LPCB.H_EC.PRCS)
    External(\_SB.PCI0.LPCB.H_EC.PEC0)
    External(\_SB.PCI0.LPCB.H_EC.PEC1)
    External(\_SB.PCI0.LPCB.H_EC.PEC2)
    External(\_SB.PCI0.LPCB.H_EC.PEC3)
    External(\_SB.PCI0.LPCB.H_EC.TER1)
    External(\_SB.PCI0.LPCB.H_EC.TER2)
    External(\_SB.PCI0.LPCB.H_EC.TER3)
    External(\_SB.PCI0.LPCB.H_EC.TER4)
    External(\_SB.PCI0.LPCB.H_EC.TER5)
    External(\_SB.PCI0.LPCB.H_EC.TER6)

    External(\_SB.PCI0.LPCB.H_EC.ECMD, MethodObj)
    External(\_SB.PCI0.LPCB.H_EC.ECRD, MethodObj)
    External(\_SB.PCI0.LPCB.H_EC.ECWT, MethodObj)

    External(\PAMT)
    External(\TSDB)

    Device(PTID)
    {
      Name(_HID, EISAID("INT340E"))
      Name(_CID, EISAID("PNP0C02"))
      Name(IVER, 0x00030000)      // Version 3
      Name(_STA, 0x0F)

      Method(TSDL)
      {
        If(LEqual(TSDB,1))
        {
          Return(TSD2)
        }
        Return(TSD1) // default all other boards
      }

      // SKL RVP, all other boards
      Name(TSD1, Package()
      { //Device Class  //Name of Temperature Value
        0x00000000, "CPU Core 0 DTS", //not from ec
        0x00000000, "CPU Core 1 DTS", //not from ec
        0x00000000, "CPU Core 2 DTS", //not from ec
        0x00000000, "CPU Core 3 DTS", //not from ec
        0x00000000, "CPU Core Package DTS", //not from ec
        0x00000003, "DIMM0_hotspot_U4C1",
        0x00000003, "DIMM1_hotspot_Q4D1",
        0x00000003, "IMVP_conn_Q7C1",
        0x00000003, "board_hotspot1_U3G3",
        0x00000003, "board_hotspot2_Q3G1",
        0x00000000, "CPU Package Temperature",
        0x00000005, "PCH DTS Temperature from PCH",
        0x00000000, "CPU PECI reading",
        0x00000005, "SA DTS Temperature from PCH",
        0x00000002, "TZ00 _TMP",
        0x00000002, "TZ01 _TMP",
        0x00000003, "SDRAM_hotspot_RT5B1",
        0x00000003, "NGFF_slot_RT6G1",
        0x00000003, "IMVP_conn_RT7D1",
        0x00000003, "board_hotspot3_RT8F1"
      })

      // SKL SDS
      Name(TSD2, Package()
      { //Device Class  //Name of Temperature Value
        0x00000000, "CPU Core 0 DTS", //not from ec
        0x00000000, "CPU Core 1 DTS", //not from ec
        0x00000000, "CPU Core 2 DTS", //not from ec
        0x00000000, "CPU Core 3 DTS", //not from ec
        0x00000000, "CPU Core Package DTS", //not from ec
        0x00000003, "A-Skin Hotspot U21",
        0x00000003, "A-Skin Hotspot U21 IR",
        0x00000003, "B-Skin Hotspot U1",
        0x00000003, "B-Skin Hotspot U1 IR",
        0x00000000, "CPU Package Temperature",
        0x00000005, "PCH DTS Temperature from PCH",
        0x00000000, "CPU PECI reading",
        0x00000005, "SA DTS Temperature from PCH",
        0x00000002, "TZ00 _TMP",
        0x00000003, "SoC VR Thermsitor RT11",
        0x00000003, "Memory Thermistor RT9",
        0x00000003, "GNSS Board Ambient Thermistor R4",
        0x00000003, "System VR Thermistor RT10",
        0x00000003, "A-Skin Skycam Thermistor RT8"
      })

      Name(PSDL, Package()
      { // Device Class   //Name of Power Value
        0x0000000B,       "Cpu Power (mW)",
        0x0000000B,       "Platform Power (mW)",
        0x0000000B,       "Cpu Average Power (mW)",
        0x0000000B,       "Platform Average Power (mW)",
        0x0000000C,       "Battery 1 Design Capacity (mWh)",
        0x0000000C,       "Battery 1 Remaining Capacity (mWh)",
        0x0000000C,       "Battery 1 Full Charge Capacity (mWh)",
        0x0000000C,       "Battery 1 Full Resolution Voltage (mV)",
        0x0000000C,       "Battery 1 Full Resolution Discharge Current (mA)",
        0x0000000C,       "Battery 1 Full Resolution Charge Current (mA)",
        0x0000000C,       "Battery 2 Remaining Capacity (mWh)",
        0x0000000C,       "Battery 2 Full Charge Capacity (mWh)",
        0x0000000C,       "Battery 2 Full Resolution Voltage (mV)",
        0x0000000C,       "Battery 2 Full Resolution Discharge Current (mA)",
        0x0000000C,       "Battery 2 Full Resolution Charge Current (mA)",
        0x0000000C,       "Battery Pack 1 maximum power (mW)",
        0x0000000C,       "Battery Pack 2 maximum power (mW)"
      })

      Method(OSDL)
      {
        If(LEqual(TSDB,1))
        {
          Return(OSD2)
        }
          Return(OSD1) // default all other boards
      }

      // SKL RVP, all other boards
      Name(OSD1, Package()
      { // Device Class   // Descriptive Name   //Unit
        0x00000004, "CPU Fan Duty Cycle", "RAW",
        0x00000004, "CPU Fan #1 Speed", "RPM",
        0x00000003, "Skin Temp 0", "RAW",
        0x00000003, "Thermistor 1 ", "RAW",
        0x00000003, "Thermistor 2 ", "RAW",
        0x00000003, "Thermistor 3 ", "RAW",
        0x00000003, "Thermistor 4 ", "RAW"
      })

      // SKL SDS
      Name(OSD2, Package()
      { // Device Class   // Descriptive Name   //Unit
        0x00000004, "CPU Fan Duty Cycle", "RAW",
        0x00000004, "CPU Fan #1 Speed", "RPM",
        0x00000003, "Skin Temp 0", "RAW",
        0x00000003, "Thermistor 1 ", "RAW",
        0x00000003, "Thermistor 2 ", "RAW",
        0x00000003, "Thermistor 3 ", "RAW",
        0x00000003, "Thermistor 4 ", "RAW",
        0x00000003, "Thermistor 5 ", "RAW"
      })

      // XLAT (Translate) LPAT table for RVP Thermistors
      //
      // Translate the thermistor raw value into tenths Kelvin using the LPAT table
      //
      // Arguments: (1)
      //   Raw value from thermistor
      // Return Value:
      //   Temperature in tenths of degree Kelvin
      //
      Method(XLAT,1,Serialized)
      {
        If(LGreaterEqual(Arg0,0x3AC)) {Return(2631)}
        If(LGreaterEqual(Arg0,0x394)) {Return(2681)}
        If(LGreaterEqual(Arg0,0x374)) {Return(2731)}
        If(LGreaterEqual(Arg0,0x354)) {Return(2781)}
        If(LGreaterEqual(Arg0,0x32C)) {Return(2831)}
        If(LGreaterEqual(Arg0,0x300)) {Return(2881)}
        If(LGreaterEqual(Arg0,0x2D0)) {Return(2931)}
        If(LGreaterEqual(Arg0,0x29C)) {Return(2981)}
        If(LGreaterEqual(Arg0,0x264)) {Return(3031)}
        If(LGreaterEqual(Arg0,0x230)) {Return(3081)}
        If(LGreaterEqual(Arg0,0x1FC)) {Return(3131)}
        If(LGreaterEqual(Arg0,0x1C8)) {Return(3181)}
        If(LGreaterEqual(Arg0,0x194)) {Return(3231)}
        If(LGreaterEqual(Arg0,0x164)) {Return(3281)}
        If(LGreaterEqual(Arg0,0x138)) {Return(3331)}
        If(LGreaterEqual(Arg0,0x114)) {Return(3381)}
        If(LGreaterEqual(Arg0,0x0F0)) {Return(3431)}
        If(LGreaterEqual(Arg0,0x0D4)) {Return(3481)}
        If(LGreaterEqual(Arg0,0x0B8)) {Return(3531)}
        If(LGreaterEqual(Arg0,0x0A0)) {Return(3581)}
        If(LGreaterEqual(Arg0,0x08C)) {Return(3631)}
        If(LGreaterEqual(Arg0,0x07C)) {Return(3681)}
        If(LGreaterEqual(Arg0,0x068)) {Return(3731)}
        If(LGreaterEqual(Arg0,0x058)) {Return(3781)}
        Return(3781)
      }

      // XSDS (Translate) LPAT table for SDS Thermistor 5
      //
      // Translate the thermistor raw value into tenths Kelvin using the LPAT table
      //
      // Arguments: (1)
      //   Raw value from thermistor
      // Return Value:
      //   Temperature in tenths of degree Kelvin
      //
      Method(XSDS,1,Serialized)
      {
        If(LGreaterEqual(Arg0,0x2F0)) {Return(2631)}
        If(LGreaterEqual(Arg0,0x2DC)) {Return(2681)}
        If(LGreaterEqual(Arg0,0x2C4)) {Return(2731)}
        If(LGreaterEqual(Arg0,0x2A8)) {Return(2781)}
        If(LGreaterEqual(Arg0,0x288)) {Return(2831)}
        If(LGreaterEqual(Arg0,0x264)) {Return(2881)}
        If(LGreaterEqual(Arg0,0x240)) {Return(2931)}
        If(LGreaterEqual(Arg0,0x214)) {Return(2981)}
        If(LGreaterEqual(Arg0,0x1EC)) {Return(3031)}
        If(LGreaterEqual(Arg0,0x1C0)) {Return(3081)}
        If(LGreaterEqual(Arg0,0x194)) {Return(3131)}
        If(LGreaterEqual(Arg0,0x16C)) {Return(3181)}
        If(LGreaterEqual(Arg0,0x144)) {Return(3231)}
        If(LGreaterEqual(Arg0,0x11C)) {Return(3281)}
        If(LGreaterEqual(Arg0,0x0FC)) {Return(3331)}
        If(LGreaterEqual(Arg0,0x0DC)) {Return(3381)}
        If(LGreaterEqual(Arg0,0x0C0)) {Return(3431)}
        If(LGreaterEqual(Arg0,0x0A8)) {Return(3481)}
        If(LGreaterEqual(Arg0,0x094)) {Return(3531)}
        If(LGreaterEqual(Arg0,0x080)) {Return(3581)}
        If(LGreaterEqual(Arg0,0x070)) {Return(3631)}
        If(LGreaterEqual(Arg0,0x060)) {Return(3681)}
        If(LGreaterEqual(Arg0,0x054)) {Return(3731)}
        If(LGreaterEqual(Arg0,0x048)) {Return(3781)}
        Return(3781)
      }

      Method(TSDD, 0, Serialized)
      {
          If(LEqual(TSDB,1)){
            Name (TMPX, Package() {
              0x80000000,0x80000000,0x80000000,0x80000000,
              0x80000000,0x80000000,0x80000000,0x80000000,
              0x80000000,0x80000000,0x80000000,0x80000000,
              0x80000000,0x80000000,0x80000000,0x80000000,
              0x80000000,0x80000000,0x80000000})

            Store(Add(Multiply(\_PR.DTS1, 10), 2732), Index(TMPX, 0))
            Store(Add(Multiply(\_PR.DTS2, 10), 2732), Index(TMPX, 1))
            Store(Add(Multiply(\_PR.DTS3, 10), 2732), Index(TMPX, 2))
            Store(Add(Multiply(\_PR.DTS4, 10), 2732), Index(TMPX, 3))
            Store(Add(Multiply(\_PR.PDTS, 10), 2732), Index(TMPX, 4))
            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
            {
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR1)), 10), 2732), Index(TMPX, 5))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR2)), 10), 2732), Index(TMPX, 6))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR3)), 10), 2732), Index(TMPX, 7))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR4)), 10), 2732), Index(TMPX, 8))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PMAX)), 10), 2732), Index(TMPX, 9))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PPDT)), 10), 2732), Index(TMPX, 10))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECH)), 10), 2732), Index(TMPX, 11))
            }
            Store(\_TZ.TZ00._TMP, Index(TMPX, 13))
            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
            {
              // Thermistor values are 10 bit but EC only exposes the high 8 bits.  Shift left 2 gives the real number.
              Store(XLAT(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER1)),2)), Index(TMPX, 14))
              Store(XLAT(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER2)),2)), Index(TMPX, 15))
              Store(XLAT(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER3)),2)), Index(TMPX, 16))
              Store(XLAT(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER4)),2)), Index(TMPX, 17))
              Store(XSDS(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER5)),2)), Index(TMPX, 18))
            }
            Return(TMPX)
          }
            Name (TMPV, Package() {
              0x80000000,0x80000000,0x80000000,0x80000000,
              0x80000000,0x80000000,0x80000000,0x80000000,
              0x80000000,0x80000000,0x80000000,0x80000000,
              0x80000000,0x80000000,0x80000000,0x80000000,
              0x80000000,0x80000000,0x80000000,0x80000000})

            Store(Add(Multiply(\_PR.DTS1, 10), 2732), Index(TMPV, 0))
            Store(Add(Multiply(\_PR.DTS2, 10), 2732), Index(TMPV, 1))
            Store(Add(Multiply(\_PR.DTS3, 10), 2732), Index(TMPV, 2))
            Store(Add(Multiply(\_PR.DTS4, 10), 2732), Index(TMPV, 3))
            Store(Add(Multiply(\_PR.PDTS, 10), 2732), Index(TMPV, 4))
            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
            {
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR1)), 10), 2732), Index(TMPV, 5))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR2)), 10), 2732), Index(TMPV, 6))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR3)), 10), 2732), Index(TMPV, 7))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR4)), 10), 2732), Index(TMPV, 8))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR5)), 10), 2732), Index(TMPV, 9))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PMAX)), 10), 2732), Index(TMPV, 10))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PPDT)), 10), 2732), Index(TMPV, 11))
              Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECH)), 10), 2732), Index(TMPV, 12))
            }
            Store(\_TZ.TZ00._TMP, Index(TMPV, 14))
            Store(\_TZ.TZ01._TMP, Index(TMPV, 15))
            If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
            {
              // Thermistor values are 10 bit but EC only exposes the high 8 bits.  Shift left 2 gives the real number.
              Store(XLAT(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER1)),2)), Index(TMPV, 16))
              Store(XLAT(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER2)),2)), Index(TMPV, 17))
              Store(XLAT(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER3)),2)), Index(TMPV, 18))
              Store(XLAT(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER4)),2)), Index(TMPV, 19))
            }
            Return(TMPV)
      } // end of Method(TSDD)

      Method(PSDD, 0, Serialized)
      {
        Name (PWRV, Package() {
          0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,
          0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,0x80000000,
          0x80000000,0x80000000,0x80000000,0x80000000,0x80000000
        })
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CPUP)), Index(PWRV, 0))
        Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PPWR)), 10, Index(PWRV, 1))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CPAP)), Index(PWRV, 2))
        Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PLAP)), 10, Index(PWRV, 3))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1DC)), Index(PWRV, 4))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)), Index(PWRV, 5))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)), Index(PWRV, 6))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FV)), Index(PWRV, 7))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1DI)), Index(PWRV, 8))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1CI)), Index(PWRV, 9))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2RC)), Index(PWRV, 10))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2FC)), Index(PWRV, 11))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2FV)), Index(PWRV, 12))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2DI)), Index(PWRV, 13))
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2CI)), Index(PWRV, 14))
        // convert PMAX from signed to unsigned integer
        // PMAX Value = (NOT(OR(FFFF0000, ECValue))+1) * 10.
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.BMAX)), Local0)
        If(Local0){ // don't convert a zero
          Not(Or(0xFFFF0000,Local0,Local0),Local0) // make it a 32 bit value before inverting the bits
          Multiply(Add(1,Local0,Local0),10,Local0) // add 1 and multiply by 10
        }
        Store(Local0,Index(PWRV, 15))
        // PMAX Value = (NOT(OR(FFFF0000, ECValue))+1) * 10.
        Add(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2MH)), 8),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2ML)),Local0) // Get high and low byte from EC.
        If(Local0){ // don't convert a zero
          Not(Or(0xFFFF0000,Local0,Local0),Local0) // make it a 32 bit value before inverting the bits
          Multiply(Add(1,Local0,Local0),10,Local0) // add 1 and multiply by 10
        }
         Store(Local0,Index(PWRV, 16))
        }
        Return (PWRV)
      }

      Method(OSDD, 0, Serialized)
      {
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
            If(LEqual(TSDB,1))
            {
              Name (OSD2, Package () {0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
                0x80000000, 0x80000000})
              Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PENV)), Index(OSD2, 0))
              Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CFSP)), Index(OSD2, 1))
              Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR3)), Index(OSD2, 2))
              // Thermistor values are 10 bit but EC only exposes the high 8 bits.  Shift left 2 gives the real number.
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER1)),2), Index(OSD2, 3))
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER2)),2), Index(OSD2, 4))
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER3)),2), Index(OSD2, 5))
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER4)),2), Index(OSD2, 6))
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER5)),2), Index(OSD2, 7))
              Return(OSD2)
            }
              Name (OSD1, Package () {0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000,
                0x80000000, 0x80000000})
              Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PENV)), Index(OSD1, 0))
              Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.CFSP)), Index(OSD1, 1))
              Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSR3)), Index(OSD1, 2))
              // Thermistor values are 10 bit but EC only exposes the high 8 bits.  Shift left 2 gives the real number.
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER1)),2), Index(OSD1, 3))
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER2)),2), Index(OSD1, 4))
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER3)),2), Index(OSD1, 5))
              Store(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TER4)),2), Index(OSD1, 6))
              Return(OSD1) // default all other boards
        }
        Return(Package(){})
      }

      Method (SDSP)
      {
        Return(10)      // Sampling period .
      }

      // PECI ACPI Access method
      Name(PADA, Package()
      {
        0x01,              // Access Mode -ACPI
        0xFFFFFFFF,        // Command Status port
        0xFFFFFFFF,        // Data port
        26,                // MailBox length
        0xFFFFFFFF,        // WritePECIMailBoxDataCommand
        0xFFFFFFFF,        // ReadPECIMailBoxDataCommand
        0xFFFFFFFF,        // IssuePECIcommand
        0xFFFFFFFF,        // EndPECICommand
        0xFFFFFFFF,        // ReadStatusCommand
        0xFFFFFFFF         // ReadErrorCountCommand
      })

      // PECI Direct I/O Access method
      Name(PADD, Package()
      {
        0x02,        // Access Mode
        0x6A4,       // Command Status port
        0x6A0,       // Data port
        26,          // MailBox length
        0x62,        // WritePECIMailBoxDataCommand
        0x63,        // ReadPECIMailBoxDataCommand
        0x60,        // IssuePECIcommand
        0x61,        // EndPECICommand
        0x65,        // ReadStatusCommand
        0x64         // ReadErrorCountCommand
      })

      Method (PADT)
      {
        If(PAMT) {
          Return (PADA)
        }
        Return (PADD)
      }

      Method (RPMD, 0, Serialized)
      {
        Name (MTMP, Buffer(26) {})
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PCAD)), Index(MTMP,6))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEWL)), Index(MTMP,7))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PWRL)), Index(MTMP,8))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECD)), Index(MTMP,9))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEHI)), Index(MTMP,10))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECI)), Index(MTMP,11))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEPL)), Index(MTMP,12))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEPM)), Index(MTMP,13))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PWFC)), Index(MTMP,14))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PECC)), Index(MTMP,15))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PDT0)), Index(MTMP,16))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PDT1)), Index(MTMP,17))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PDT2)), Index(MTMP,18))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PDT3)), Index(MTMP,19))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRFC)), Index(MTMP,20))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS0)), Index(MTMP,21))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS1)), Index(MTMP,22))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS2)), Index(MTMP,23))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS3)), Index(MTMP,24))
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRS4)), Index(MTMP,25))
        }
        Return (MTMP)
      }

      Method (WPMD,1)
      {
        If (LNotEqual(Sizeof(Arg0), 26)) {
          Return (0xFFFFFFFF)
        }

        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,6)), RefOf(\_SB.PCI0.LPCB.H_EC.PCAD))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,7)), RefOf(\_SB.PCI0.LPCB.H_EC.PEWL))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,8)), RefOf(\_SB.PCI0.LPCB.H_EC.PWRL))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,9)), RefOf(\_SB.PCI0.LPCB.H_EC.PECD))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,10)), RefOf(\_SB.PCI0.LPCB.H_EC.PEHI))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,11)), RefOf(\_SB.PCI0.LPCB.H_EC.PECI))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,12)), RefOf(\_SB.PCI0.LPCB.H_EC.PEPL))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,13)), RefOf(\_SB.PCI0.LPCB.H_EC.PEPM))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,14)), RefOf(\_SB.PCI0.LPCB.H_EC.PWFC))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,15)), RefOf(\_SB.PCI0.LPCB.H_EC.PECC))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,16)), RefOf(\_SB.PCI0.LPCB.H_EC.PDT0))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,17)), RefOf(\_SB.PCI0.LPCB.H_EC.PDT1))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,18)), RefOf(\_SB.PCI0.LPCB.H_EC.PDT2))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,19)), RefOf(\_SB.PCI0.LPCB.H_EC.PDT3))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,20)), RefOf(\_SB.PCI0.LPCB.H_EC.PRFC))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,21)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS0))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,22)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS1))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,23)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS2))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,24)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS3))
          \_SB.PCI0.LPCB.H_EC.ECWT(DeRefOf(Index(Arg0,25)), RefOf(\_SB.PCI0.LPCB.H_EC.PRS4))
          \_SB.PCI0.LPCB.H_EC.ECMD (0x67)
        }
        Return (0)
      }

      Method (ISPC)
      {
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
          \_SB.PCI0.LPCB.H_EC.ECMD (0x65)
        }
        Return (0)
      }

      Method (ENPC)
      {
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
          \_SB.PCI0.LPCB.H_EC.ECMD (0x66)
        }
        Return (0)
      }

      Method (RPCS)
      {
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PRCS)), Local0)
        }
        Return (Local0)
      }

      Method (RPEC)
      {
        Store(0,Local0)
        If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,1)) // check EC opregion available
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEC0)), Local1)
          Or(Local0,Local1,Local0)
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEC1)), Local1)
          Or(Local0,ShiftLeft(Local1,8),Local0)
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEC2)), Local1)
          Or(Local0,ShiftLeft(Local1,16),Local0)
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PEC3)), Local1)
          Or(Local0,ShiftLeft(Local1,24),Local0)
        }
        return (Local0)
      }

    } // end Device(PTID)
  } // end Scope(\_SB)
} // end DefinitionBlock
