

/** @file
  ACPI DSDT table

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2019 Intel Corporation.

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

  // Define a Global region of ACPI NVS Region that may be used for any
  // type of implementation.  The starting offset and size will be fixed
  // up by the System BIOS during POST.  Note that the Size must be a word
  // in size to be fixed up correctly.

  


  OperationRegion(GNVS,SystemMemory,0xFFFF0000,0xAA55)
  Field(GNVS,AnyAcc,Lock,Preserve)
  {
  //
  // Miscellaneous Dynamic Registers:
  //
  Offset(0),      OSYS, 16, // Offset(0),     Operating System
  Offset(2),      SMIF, 8,  // Offset(2),     SMI Function Call (ASL to SMI via I/O Trap)
  Offset(3),      PRM0, 8,  // Offset(3),     SMIF - Parameter 0
  Offset(4),      PRM1, 8,  // Offset(4),     SMIF - Parameter 1
  Offset(5),      SCIF, 8,  // Offset(5),     SCI Function Call (SMI to ASL via _L00)
  Offset(6),      PRM2, 8,  // Offset(6),     SCIF - Parameter 0
  Offset(7),      PRM3, 8,  // Offset(7),     SCIF - Parameter 1
  Offset(8),      LCKF, 8,  // Offset(8),     Global Lock Function Call (EC Communication)
  Offset(9),      PRM4, 8,  // Offset(9),     LCKF - Parameter 0
  Offset(10),     PRM5, 8,  // Offset(10),    LCKF - Parameter 1
  Offset(11),     P80D, 32, // Offset(11),    Port 80 Debug Port Value @deprecated since revision 24
  Offset(15),     PWRS, 8,  // Offset(15),    Power State (AC Mode = 1)
  Offset(16),     DBGS, 8,  // Offset(16),    Debug State
  //
  // Thermal Policy Registers:
  //
  Offset(17),     THOF, 8,  // Offset(17),    Enable Thermal Offset for KSC
  Offset(18),     ACT1, 8,  // Offset(18),    Active Trip Point 1
  Offset(19),     ACTT, 8,  // Offset(19),    Active Trip Point
  Offset(20),     PSVT, 8,  // Offset(20),    Passive Trip Point
  Offset(21),     TC1V, 8,  // Offset(21),    Passive Trip Point TC1 Value
  Offset(22),     TC2V, 8,  // Offset(22),    Passive Trip Point TC2 Value
  Offset(23),     TSPV, 8,  // Offset(23),    Passive Trip Point TSP Value
  Offset(24),     CRTT, 8,  // Offset(24),    Critical Trip Point
  Offset(25),     DTSE, 8,  // Offset(25),    Digital Thermal Sensor Enable
  Offset(26),     DTS1, 8,  // Offset(26),    Digital Thermal Sensor 1 Reading
  Offset(27),     DTS2, 8,  // Offset(27),    Digital Thermal Sensor 2 Reading
  Offset(28),     DTSF, 8,  // Offset(28),    DTS SMI Function Call
  //
  // Battery Support Registers:
  //
  Offset(30),               // Offset(29) : Offset(29), Reserved bytes
  Offset(30),     BNUM, 8,  // Offset(30),    Battery Number Present
  Offset(31),         , 8,  // Offset(31),    Battery 0 Stored Capacity
  Offset(32),         , 8,  // Offset(32),    Battery 1 Stored Capacity
  Offset(33),         , 8,  // Offset(33),    Battery 2 Stored Capacity
  Offset(34),         , 8,  // Offset(34),    Battery 0 Stored Status
  Offset(35),         , 8,  // Offset(35),    Battery 1 Stored Status
  Offset(36),         , 8,  // Offset(36),    Battery 2 Stored Status
  //
  // Revision Field:
  //
  Offset(37),     REVN, 8,  // Offset(37),    Revison of GlobalNvsArea
  //                 
  // CPU Identification Registers:
  //
  Offset(38),     APIC, 8,  // Offset(38),    APIC Enabled by SBIOS (APIC Enabled = 1)
  Offset(39),     TCNT, 8,  // Offset(39),    Number of Enabled Threads
  Offset(40),     PCP0, 8,  // Offset(40),    PDC Settings, Processor 0
  Offset(41),     PCP1, 8,  // Offset(41),    PDC Settings, Processor 1
  Offset(42),     PPCM, 8,  // Offset(42),    Maximum PPC state
  Offset(43),     PPMF, 32, // Offset(43),    PPM Flags (Same as CFGD)
  Offset(47),     C67L, 8,  // Offset(47),    C6/C7 Entry/Exit latency
  //
  // SIO CMOS Configuration Registers:
  //
  Offset(48),     NATP, 8,  // Offset(48),    National SIO Present
  Offset(49),     CMAP, 8,  // Offset(49),    COM A Port
  Offset(50),     CMBP, 8,  // Offset(50),    COM B Port
  Offset(51),     LPTP, 8,  // Offset(51),    LPT Port
  Offset(52),     FDCP, 8,  // Offset(52),    FDC Port
  Offset(53),     CMCP, 8,  // Offset(53),    SMSC Com Port
  Offset(54),     CIRP, 8,  // Offset(54),    SMSC Com CIR Port
  Offset(55),     SMSC, 8,  // Offset(55),    SMSC1007 SIO Present
  Offset(56),     W381, 8,  // Offset(56),    WPCN381U SIO Present
  Offset(57),     SMC1, 8,  // Offset(57),    SMSC1000 SIO Present
  //
  // Extended Mobile Access Values
  //
  Offset(58),     EMAE, 8,  // Offset(58),    EMA Enable
  Offset(59),     EMAP, 16, // Offset(59),    EMA Pointer
  Offset(61),     EMAL, 16, // Offset(61),    EMA Length
  //
  // MEF Registers:
  //
  Offset(63),     MEFE, 8,  // Offset(63),    MEF Enable
  //
  // PCIe Dock Status:
  //
  Offset(64),     DSTS, 8,  // Offset(64),    PCIe Dock Status
  //
  // TPM Registers
  //
  Offset(65),     MORD, 8,  // Offset(65),    Memory Overwrite Request Data
  Offset(66),     TCGP, 8,  // Offset(66),    Used for save the Mor and/or physical presence paramter
  Offset(67),     PPRP, 32, // Offset(67),    Physical Presence request operation response
  Offset(71),     PPRQ, 8,  // Offset(71),    Physical Presence request operation
  Offset(72),     LPPR, 8,  // Offset(72),    Last Physical Presence request operation
  //
  // SATA Registers:
  //
  Offset(73),     IDEM, 8,  // Offset(73),    IDE Mode (Compatible\Enhanced)
  //
  // Board Id
  //
  Offset(74),     PLID, 8,  // Offset(74),    Platform id
  Offset(75),     BTYP, 8,  // Offset(75),    Board Type
  //
  // PCIe Hot Plug
  //
  Offset(76),     OSCC, 8,  // Offset(76),    PCIE OSC Control
  Offset(77),     NEXP, 8,  // Offset(77),    Native PCIE Setup Value
  //
  // USB Sideband Deferring Support
  //
  Offset(78),     SBV1, 8,  // Offset(78),    USB Sideband Deferring GPE Vector (HOST_ALERT#1)
  Offset(79),     SBV2, 8,  // Offset(79),    USB Sideband Deferring GPE Vector (HOST_ALERT#2)
  //
  // Embedded Controller Availability Flag.
  //
  Offset(80),     ECON, 8,  // Offset(80),    Embedded Controller Availability Flag.
  //
  // Global Variables
  //
  Offset(81),     DSEN, 8,  // Offset(81),    _DOS Display Support Flag.
  Offset(82),     GPIC, 8,  // Offset(82),    Global IOAPIC/8259 Interrupt Mode Flag.
  Offset(83),     CTYP, 8,  // Offset(83),    Global Cooling Type Flag.
  Offset(84),     L01C, 8,  // Offset(84),    Global L01 Counter.
  Offset(85),     VFN0, 8,  // Offset(85),    Virtual Fan0 Status.
  Offset(86),     VFN1, 8,  // Offset(86),    Virtual Fan1 Status.
  Offset(87),     VFN2, 8,  // Offset(87),    Virtual Fan2 Status.
  Offset(88),     VFN3, 8,  // Offset(88),    Virtual Fan3 Status.
  Offset(89),     VFN4, 8,  // Offset(89),    Virtual Fan4 Status.
  Offset(90),     VFN5, 8,  // Offset(90),    Virtual Fan5 Status.
  Offset(91),     VFN6, 8,  // Offset(91),    Virtual Fan6 Status.
  Offset(92),     VFN7, 8,  // Offset(92),    Virtual Fan7 Status.
  Offset(93),     VFN8, 8,  // Offset(93),    Virtual Fan8 Status.
  Offset(94),     VFN9, 8,  // Offset(94),    Virtual Fan9 Status.
  //
  // Thermal
  //
  Offset(95),     ATMC, 8,  // Offset(95),    Active Trip Point for MCH
  Offset(96),     PTMC, 8,  // Offset(96),    Passive Trip Point for MCH
  Offset(97),     PNHM, 32, // Offset(97),    CPUID Feature Information [EAX]
  Offset(101),    TBAL, 32, // Offset(101),   Reserved for Thermal Base Low Address for BIOS
  Offset(105),    TBAH, 32, // Offset(105),   Reserved for Thermal Base High Address for BIOS
  Offset(109),    RTIP, 8,  // Offset(109),   Run Time Interface for Intelligent Power Savings
  Offset(110),    TSOD, 8,  // Offset(110),   TS-on-DIMM is chosen in SETUP and present on the DIMM
  //
  // Board info
  //
  Offset(111),    PFLV, 8,  // Offset(111),   Platform Flavor
  Offset(112),    BREV, 8,  // Offset(112),   Board Rev
  //
  // Package temperature
  //
  Offset(113),    PDTS, 8,  // Offset(113),   Package Temperature
  Offset(114),    PKGA, 8,  // Offset(114),   Package Temperature MSR available
  Offset(115),    PAMT, 8,  // Offset(115),   Peci Access Method
  Offset(116),    AC0F, 8,  // Offset(116),   _AC0 Fan Speed
  Offset(117),    AC1F, 8,  // Offset(117),   _AC1 Fan Speed
  Offset(118),    DTS3, 8,  // Offset(118),   Digital Thermal Sensor 3 Reading
  Offset(119),    DTS4, 8,  // Offset(119),   Digital Thermal Sensor 4 Reading
  Offset(120),    LTR1, 8,  // Offset(120),   Latency Tolerance Reporting Enable
  Offset(121),    LTR2, 8,  // Offset(121),   Latency Tolerance Reporting Enable
  Offset(122),    LTR3, 8,  // Offset(122),   Latency Tolerance Reporting Enable
  Offset(123),    LTR4, 8,  // Offset(123),   Latency Tolerance Reporting Enable
  Offset(124),    LTR5, 8,  // Offset(124),   Latency Tolerance Reporting Enable
  Offset(125),    LTR6, 8,  // Offset(125),   Latency Tolerance Reporting Enable
  Offset(126),    LTR7, 8,  // Offset(126),   Latency Tolerance Reporting Enable
  Offset(127),    LTR8, 8,  // Offset(127),   Latency Tolerance Reporting Enable
  Offset(128),    LTR9, 8,  // Offset(128),   Latency Tolerance Reporting Enable
  Offset(129),    LTRA, 8,  // Offset(129),   Latency Tolerance Reporting Enable
  Offset(130),    LTRB, 8,  // Offset(130),   Latency Tolerance Reporting Enable
  Offset(131),    LTRC, 8,  // Offset(131),   Latency Tolerance Reporting Enable
  Offset(132),    LTRD, 8,  // Offset(132),   Latency Tolerance Reporting Enable
  Offset(133),    LTRE, 8,  // Offset(133),   Latency Tolerance Reporting Enable
  Offset(134),    LTRF, 8,  // Offset(134),   Latency Tolerance Reporting Enable
  Offset(135),    LTRG, 8,  // Offset(135),   Latency Tolerance Reporting Enable
  Offset(136),    LTRH, 8,  // Offset(136),   Latency Tolerance Reporting Enable
  Offset(137),    LTRI, 8,  // Offset(137),   Latency Tolerance Reporting Enable
  Offset(138),    LTRJ, 8,  // Offset(138),   Latency Tolerance Reporting Enable
  Offset(139),    LTRK, 8,  // Offset(139),   Latency Tolerance Reporting Enable
  Offset(140),    LTRL, 8,  // Offset(140),   Latency Tolerance Reporting Enable
  Offset(141),    LTRM, 8,  // Offset(141),   Latency Tolerance Reporting Enable
  Offset(142),    LTRN, 8,  // Offset(142),   Latency Tolerance Reporting Enable
  Offset(143),    LTRO, 8,  // Offset(143),   Latency Tolerance Reporting Enable
  Offset(144),    OBF1, 8,  // Offset(144),   Optimized Buffer Flush and Fill
  Offset(145),    OBF2, 8,  // Offset(145),   Optimized Buffer Flush and Fill
  Offset(146),    OBF3, 8,  // Offset(146),   Optimized Buffer Flush and Fill
  Offset(147),    OBF4, 8,  // Offset(147),   Optimized Buffer Flush and Fill
  Offset(148),    OBF5, 8,  // Offset(148),   Optimized Buffer Flush and Fill
  Offset(149),    OBF6, 8,  // Offset(149),   Optimized Buffer Flush and Fill
  Offset(150),    OBF7, 8,  // Offset(150),   Optimized Buffer Flush and Fill
  Offset(151),    OBF8, 8,  // Offset(151),   Optimized Buffer Flush and Fill
  Offset(152),    OBF9, 8,  // Offset(152),   Optimized Buffer Flush and Fill
  Offset(153),    OBFA, 8,  // Offset(153),   Optimized Buffer Flush and Fill
  Offset(154),    OBFB, 8,  // Offset(154),   Optimized Buffer Flush and Fill
  Offset(155),    OBFC, 8,  // Offset(155),   Optimized Buffer Flush and Fill
  Offset(156),    OBFD, 8,  // Offset(156),   Optimized Buffer Flush and Fill
  Offset(157),    OBFE, 8,  // Offset(157),   Optimized Buffer Flush and Fill
  Offset(158),    OBFF, 8,  // Offset(158),   Optimized Buffer Flush and Fill
  Offset(159),    OBFG, 8,  // Offset(159),   Optimized Buffer Flush and Fill
  Offset(160),    OBFH, 8,  // Offset(160),   Optimized Buffer Flush and Fill
  Offset(161),    OBFI, 8,  // Offset(161),   Optimized Buffer Flush and Fill
  Offset(162),    OBFJ, 8,  // Offset(162),   Optimized Buffer Flush and Fill
  Offset(163),    OBFK, 8,  // Offset(163),   Optimized Buffer Flush and Fill
  Offset(164),    OBFL, 8,  // Offset(164),   Optimized Buffer Flush and Fill
  Offset(165),    OBFM, 8,  // Offset(165),   Optimized Buffer Flush and Fill
  Offset(166),    OBFN, 8,  // Offset(166),   Optimized Buffer Flush and Fill
  Offset(167),    OBFO, 8,  // Offset(167),   Optimized Buffer Flush and Fill
  //                    
  // XTU 3.0 Specification
  //
  Offset(168),    XTUB, 32, // Offset(168),   XTU Continous structure Base Address
  Offset(172),    XTUS, 32, // Offset(172),   XMP Size
  Offset(176),    XMPB, 32, // Offset(176),   XMP Base Address
  Offset(180),    DDRF, 8,  // Offset(180),   DDR Reference Frequency
  Offset(181),    RTD3, 8,  // Offset(181),   Runtime D3 support.
  Offset(182),    PEP0, 8,  // Offset(182),   User selctable Delay for Device D0 transition.
  Offset(183),    PEP3, 8,  // Offset(183),   User selctable Delay for Device D3 transition.
  //
  // DPTF Devices and trip points
  //
  Offset(184),    DPTF, 8,  // Offset(184),   EnableDptf
  Offset(185),    DCFE, 16, // Offset(185),   EnableDCFG
  Offset(187),    SADE, 8,  // Offset(187),   EnableSaDevice
  Offset(188),    SACR, 8,  // Offset(188),   CriticalThermalTripPointSa
  Offset(189),    SAHT, 8,  // Offset(189),   HotThermalTripPointSa
  Offset(190),    CPUS, 8,  // Offset(190),   ThermalSamplingPeriodSA
  //
  // DPTF Policies
  //
  Offset(191),    CTDP, 8,  // Offset(191),   EnableCtdpPolicy
  Offset(192),    LPMP, 8,  // Offset(192),   EnableLpmPolicy @deprecated since revision 16
  Offset(193),    LPMV, 8,  // Offset(193),   CurrentLowPowerMode for LPM @deprecated since revision 16
  Offset(194),    ECEU, 8,  // Offset(194),   EnableCurrentExecutionUnit @deprecated since revision 16
  Offset(195),    TGFG, 16, // Offset(195),   TargetGfxFreq @deprecated since revision 16
  //
  // DPPM Devices and trip points
  //
  Offset(197),    MEMD, 8,  // Offset(197),   EnableMemoryDevice @deprecated. Memory Participant is not POR for DPTF
  Offset(198),    ATRA, 8,  // Offset(198),   Active Trip Point for TMEM @deprecated. Memory Participant is not POR for DPTF
  Offset(199),    PTRA, 8,  // Offset(199),   Passive Trip Point for TMEM @deprecated. Memory Participant is not POR for DPTF
  Offset(200),    MEMC, 8,  // Offset(200),   CriticalThermalTripPointTMEM @deprecated. Memory Participant is not POR for DPTF
  Offset(201),    MEMH, 8,  // Offset(201),   HotThermalTripPointTMEM @deprecated. Memory Participant is not POR for DPTF
  Offset(202),    FND1, 8,  // Offset(202),   EnableFan1Device
  Offset(203),    FND2, 8,  // Offset(203),   EnableFan2Device
  Offset(204),    AMBD, 8,  // Offset(204),   EnableAmbientDevice
  Offset(205),    AMAT, 8,  // Offset(205),   ActiveThermalTripPointAmbient
  Offset(206),    AMPT, 8,  // Offset(206),   PassiveThermalTripPointAmbient
  Offset(207),    AMCT, 8,  // Offset(207),   CriticalThermalTripPointAmbient
  Offset(208),    AMHT, 8,  // Offset(208),   HotThermalTripPointAmbient
  Offset(209),    SKDE, 8,  // Offset(209),   EnableSkinDevice
  Offset(210),    SKAT, 8,  // Offset(210),   ActiveThermalTripPointSkin
  Offset(211),    SKPT, 8,  // Offset(211),   PassiveThermalTripPointSkin
  Offset(212),    SKCT, 8,  // Offset(212),   CriticalThermalTripPointSkin
  Offset(213),    SKHT, 8,  // Offset(213),   HotThermalTripPointSkin
  Offset(214),    EFDE, 8,  // Offset(214),   EnableExhaustFanDevice
  Offset(215),    EFAT, 8,  // Offset(215),   ActiveThermalTripPointExhaustFan
  Offset(216),    EFPT, 8,  // Offset(216),   PassiveThermalTripPointExhaustFan
  Offset(217),    EFCT, 8,  // Offset(217),   CriticalThermalTripPointExhaustFan
  Offset(218),    EFHT, 8,  // Offset(218),   HotThermalTripPointExhaustFan
  Offset(219),    VRDE, 8,  // Offset(219),   EnableVRDevice
  Offset(220),    VRAT, 8,  // Offset(220),   ActiveThermalTripPointVR
  Offset(221),    VRPT, 8,  // Offset(221),   PassiveThermalTripPointVR
  Offset(222),    VRCT, 8,  // Offset(222),   CriticalThermalTripPointVR
  Offset(223),    VRHT, 8,  // Offset(223),   HotThermalTripPointVR
  //
  // DPPM Policies
  //
  Offset(224),    DPAP, 8,  // Offset(224),   EnableActivePolicy
  Offset(225),    DPPP, 8,  // Offset(225),   EnablePassivePolicy
  Offset(226),    DPCP, 8,  // Offset(226),   EnableCriticalPolicy
  Offset(227),    DCMP, 8,  // Offset(227),   EnableCoolingModePolicy @deprecated since revision 16
  Offset(228),    TRTV, 8,  // Offset(228),   TrtRevision
  //
  // CLPO (Current Logical Processor Off lining Setting)
  //
  Offset(229),    LPOE, 8,  // Offset(229),   LPOEnable
  Offset(230),    LPOP, 8,  // Offset(230),   LPOStartPState
  Offset(231),    LPOS, 8,  // Offset(231),   LPOStepSize
  Offset(232),    LPOW, 8,  // Offset(232),   LPOPowerControlSetting
  Offset(233),    LPER, 8,  // Offset(233),   LPOPerformanceControlSetting
  //
  // Miscellaneous DPTF
  //
  Offset(234),    PPSZ, 32, // Offset(234),   PPCC Step Size
  Offset(238),    DISE, 8,  // Offset(238),   EnableDisplayParticipant
  //
  // BIOS Guard
  //
  Offset(239),    BGMA, 64, // Offset(239),   BIOS Guard Memory Address for Tool Interface
  Offset(247),    BGMS, 8,  // Offset(247),   BIOS Guard Memory Size for Tool Interface
  Offset(248),    BGIA, 16, // Offset(248),   BIOS Guard IoTrap Address for Tool Interface
  //
  // Never Sleep Technology
  //
  Offset(250),    IRMC, 8,  // Offset(250),   Irmt Configuration
  //
  // NFC support
  //
  Offset(251),    NFCE, 8,  // Offset(251),   NFC module selection
  //
  // Comms Hub
  //
  Offset(252),    CHEN, 8,  // Offset(252),   Comms Hub Enable/Disable
  Offset(253),    S0ID, 8,  // Offset(253),   Low Power S0 Idle Enable
  //
  // BIOS only version of Config TDP
  //
  Offset(254),    CTDB, 8,  // Offset(254),   enable/disable BIOS only version of Config TDP
  Offset(255),    DKSM, 8,  // Offset(255),   Dock SMI number
  //
  // LPC SIO configuration
  //
  Offset(256),    SIO1, 16, // Offset(256),   SIO config port 1
  Offset(258),    SIO2, 16, // Offset(258),   SIO config port 2
  Offset(260),    SPBA, 16, // Offset(260),   SIO PME Base Address
  //
  // Software Guard
  // SEC0 - SEC8 members have been deprecated.
  //
  Offset(262),    SEC0, 32, // Offset(262),   @deprecated SGX Leaf ID 0: EAX This member has been deprecated.
  Offset(266),    SEC1, 32, // Offset(266),   @deprecated SGX Leaf ID 1: EAX This member has been deprecated.
  Offset(270),    SEC2, 32, // Offset(270),   @deprecated SGX Leaf ID 1: EBX This member has been deprecated.
  Offset(274),    SEC3, 32, // Offset(274),   @deprecated SGX Leaf ID 1: ECX This member has been deprecated.
  Offset(278),    SEC4, 32, // Offset(278),   @deprecated SGX Leaf ID 1: EDX This member has been deprecated.
  Offset(282),    SEC5, 32, // Offset(282),   @deprecated SGX Leaf ID 2: EAX This member has been deprecated.
  Offset(286),    SEC6, 32, // Offset(286),   @deprecated SGX Leaf ID 2: EBX This member has been deprecated.
  Offset(290),    SEC7, 32, // Offset(290),   @deprecated SGX Leaf ID 2: ECX This member has been deprecated.
  Offset(294),    SEC8, 32, // Offset(294),   @deprecated SGX Leaf ID 2: EDX This member has been deprecated.
  Offset(500),              // Offset(298) : Offset(499), Reserved bytes
  Offset(500),    WIFD, 8,  // Offset(500),   EnableWifiParticipant
  Offset(501),    WFAT, 8,  // Offset(501),   ActiveThermalTripPointWifi
  Offset(502),    WFPT, 8,  // Offset(502),   PassiveThermalTripPointWifi
  Offset(503),    WFCT, 8,  // Offset(503),   CriticalThermalTripPointWifi
  Offset(504),    WFHT, 8,  // Offset(504),   HotThermalTripPointWifi
  Offset(505),    PWRE, 8,  // Offset(505),   EnablePowerParticipant
  Offset(506),        , 16, // Offset(506),   DPTFRsvd0
  Offset(508),    PPPR, 16, // Offset(508),   PowerParticipantPollingRate
  Offset(510),    PBPE, 8,  // Offset(510),   EnablePowerBossPolicy
  Offset(511),    VSPE, 8,  // Offset(511),   EnableVSPolicy
  Offset(515),              // Offset(512) : Offset(514), Reserved bytes
  Offset(515),    XHPR, 8,  // Offset(515),   RTD3 USB Power Resource config
  Offset(516),    RIC0, 8,  // Offset(516),   RTD3 support for I2C0 SH
  Offset(517),    GBSX, 8,  // Offset(517),   Virtual GPIO button Notify Sleep State Change
  Offset(518),    IUBE, 8,  // Offset(518),   IUER Button Enable
  Offset(519),    IUCE, 8,  // Offset(519),   IUER Convertible Enable
  Offset(520),    IUDE, 8,  // Offset(520),   IUER Dock Enable
  Offset(521),    ECNO, 8,  // Offset(521),   EC Notification of Low Power S0 Idle State
  Offset(522),    AUDD, 16, // Offset(522),   RTD3 Audio Codec device delay
  Offset(524),    IC0D, 16, // Offset(524),   RTD3 SensorHub delay time after applying power to device
  Offset(526),    IC1D, 16, // Offset(526),   RTD3 TouchPanel delay time after applying power to device
  Offset(528),    IC1S, 16, // Offset(528),   RTD3 TouchPad delay time after applying power to device
  Offset(530),    VRRD, 16, // Offset(530),   VR Ramp up delay
  Offset(532),    PSCP, 8,  // Offset(532),   P-state Capping
  Offset(533),    I20D, 16, // Offset(533),   Delay in _PS0 after powering up I2C0 Controller
  Offset(535),    I21D, 16, // Offset(535),   Delay in _PS0 after powering up I2C1 Controller
  Offset(537),    RCG0, 16, // Offset(537),   RTD3 Config Setting0(BIT0:ZPODD, BIT1:USB3 Camera Port4(IVCAM), BIT2:PCIe NVMe, Bit4:SKL SDS SIP I2C Touch, BIT6:Card Reader, BIT7:WWAN)
  Offset(539),    RCG1, 16, // Offset(539),   RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3, BIT4:Sata Port4, BIT5:Reserved, BIT6:CR1, BIT7:CR2, BIT8:CR3, BIT9:RaidVolumes)
  Offset(541),    ECDB, 8,  // Offset(541),   EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
  Offset(542),    P2ME, 8,  // Offset(542),   Ps2 Mouse Enable
  Offset(543),    P2MK, 8,  // Offset(543),   Ps2 Keyboard and Mouse Enable
  Offset(544),    SSH0, 16, // Offset(544),   SSCN-HIGH for I2C0
  Offset(546),    SSL0, 16, // Offset(546),   SSCN-LOW  for I2C0
  Offset(548),    SSD0, 16, // Offset(548),   SSCN-HOLD for I2C0
  Offset(550),    FMH0, 16, // Offset(550),   FMCN-HIGH for I2C0
  Offset(552),    FML0, 16, // Offset(552),   FMCN-LOW  for I2C0
  Offset(554),    FMD0, 16, // Offset(554),   FMCN-HOLD for I2C0
  Offset(556),    FPH0, 16, // Offset(556),   FPCN-HIGH for I2C0
  Offset(558),    FPL0, 16, // Offset(558),   FPCN-LOW  for I2C0
  Offset(560),    FPD0, 16, // Offset(560),   FPCN-HOLD for I2C0
  Offset(562),    SSH1, 16, // Offset(562),   SSCN-HIGH for I2C1
  Offset(564),    SSL1, 16, // Offset(564),   SSCN-LOW  for I2C1
  Offset(566),    SSD1, 16, // Offset(566),   SSCN-HOLD for I2C1
  Offset(568),    FMH1, 16, // Offset(568),   FMCN-HIGH for I2C1
  Offset(570),    FML1, 16, // Offset(570),   FMCN-LOW  for I2C1
  Offset(572),    FMD1, 16, // Offset(572),   FMCN-HOLD for I2C1
  Offset(574),    FPH1, 16, // Offset(574),   FPCN-HIGH for I2C1
  Offset(576),    FPL1, 16, // Offset(576),   FPCN-LOW  for I2C1
  Offset(578),    FPD1, 16, // Offset(578),   FPCN-HOLD for I2C1
  Offset(580),    M0C0, 16, // Offset(580),   M0D3 for I2C0
  Offset(582),    M1C0, 16, // Offset(582),   M1D3 for I2C0
  Offset(584),    M2C0, 16, // Offset(584),   M0D0 for I2C0
  Offset(586),    M0C1, 16, // Offset(586),   M0D3 for I2C1
  Offset(588),    M1C1, 16, // Offset(588),   M1D3 for I2C1
  Offset(590),    M2C1, 16, // Offset(590),   M0D0 for I2C1
  Offset(592),    M0C2, 16, // Offset(592),   M0D3 for SPI0
  Offset(594),    M1C2, 16, // Offset(594),   M1D3 for SPI0
  Offset(596),    M0C3, 16, // Offset(596),   M0D3 for SPI1
  Offset(598),    M1C3, 16, // Offset(598),   M1D3 for SPI1
  Offset(600),    M0C4, 16, // Offset(600),   M0D3 for UA00
  Offset(602),    M1C4, 16, // Offset(602),   M1D3 for UA00
  Offset(604),    M0C5, 16, // Offset(604),   M0D3 for UA01
  Offset(606),    M1C5, 16, // Offset(606),   M1D3 for UA01
  Offset(608),    TBSF, 8,  // Offset(608),   Thunderbolt(TM) SMI Function Number
  //
  // Driver Mode
  //
  Offset(609),    GIRQ, 32, // Offset(609),   GPIO IRQ
  Offset(613),    DMTP, 8,  // Offset(613),   PIRQS 34,50(GPIO)
  Offset(614),    DMTD, 8,  // Offset(614),   PIRQX 39,55(GPIO)
  Offset(615),    DMSH, 8,  // Offset(615),   PIRQM 28,14(GPIO)
  Offset(616),    SHSB, 8,  // Offset(616),   Sensor Standby mode
  Offset(617),    PLCS, 8,  // Offset(617),   set PL1 limit when entering CS
  Offset(618),    PLVL, 16, // Offset(618),   PL1 limit value
  Offset(620),    GN1E, 8,  // Offset(620),   EnableGen1Participant
  Offset(621),    G1AT, 8,  // Offset(621),   ActiveThermalTripPointGen1
  Offset(622),    G1PT, 8,  // Offset(622),   PassiveThermalTripPointGen1
  Offset(623),    G1CT, 8,  // Offset(623),   CriticalThermalTripPointGen1
  Offset(624),    G1HT, 8,  // Offset(624),   HotThermalTripPointGen1
  Offset(625),    GN2E, 8,  // Offset(625),   EnableGen2Participant
  Offset(626),    G2AT, 8,  // Offset(626),   ActiveThermalTripPointGen2
  Offset(627),    G2PT, 8,  // Offset(627),   PassiveThermalTripPointGen2
  Offset(628),    G2CT, 8,  // Offset(628),   CriticalThermalTripPointGen2
  Offset(629),    G2HT, 8,  // Offset(629),   HotThermalTripPointGen2
  Offset(630),    WWSD, 8,  // Offset(630),   EnableWwanTempSensorDevice
  Offset(631),    CVSD, 8,  // Offset(631),   EnableCpuVrTempSensorDevice
  Offset(632),    SSDD, 8,  // Offset(632),   EnableSsdTempSensorDevice
  Offset(633),    INLD, 8,  // Offset(633),   EnableInletFanTempSensorDevice
  Offset(634),    IFAT, 8,  // Offset(634),   ActiveThermalTripPointInletFan
  Offset(635),    IFPT, 8,  // Offset(635),   PassiveThermalTripPointInletFan
  Offset(636),    IFCT, 8,  // Offset(636),   CriticalThermalTripPointInletFan
  Offset(637),    IFHT, 8,  // Offset(637),   HotThermalTripPointInletFan
  Offset(638),    SDWE, 8,  // Offset(638),   SCS SDCard sideband D3 Wake Event support (0)Disabled, (1)GPP_B17 pin as GpioInt, (2)GPP_D10 pin as GpioInt
  Offset(639),    USBH, 8,  // Offset(639),   Sensor Hub Type - (0)None, (1)USB, (2)I2C Intel, (3)I2C STM
  Offset(640),    BCV4, 8,  // Offset(640),   Broadcom's Bluetooth adapter's revision
  Offset(641),    WTV0, 8,  // Offset(641),   I2C0/WITT devices version
  Offset(642),    WTV1, 8,  // Offset(642),   I2C1/WITT devices version
  Offset(643),    APFU, 8,  // Offset(643),   Atmel panel FW update Enable/Disable
  Offset(644),    SOHP, 8,  // Offset(644),   SMI on Hot Plug for TBT devices
  Offset(645),    GP5F, 8,  // Offset(645),   Gpio filter to detect USB Hotplug event
  Offset(646),    NOHP, 8,  // Offset(646),   Notify on Hot Plug for TBT devices
  Offset(647),    TBSE, 8,  // Offset(647),   Thunderbolt(TM) Root port selector
  Offset(648),    WKFN, 8,  // Offset(648),   WAK Finished
  Offset(649),    PEPC, 32, // Offset(649),   PEP Constraints
  // Bit[0]  - En/Dis GBE
  // Bit[1]  - En/Dis ISP0
  // Bit[2]  - En/Dis UART0
  // Bit[3]  - En/Dis UART1
  // Bit[4]  - En/Dis TBT RP
  // Bit[5]  - En/Dis I2C0
  // Bit[6]  - En/Dis I2C1
  // Bit[7]  - En/Dis XHCI
  // Bit[8]  - En/Dis HD Audio (includes ADSP)
  // Bit[9]  - En/Dis Gfx
  // Bit[10] - En/Dis CPU
  // Bit[11] - En/Dis EMMC
  // Bit[12] - En/Dis SDXC
  // Bit[13] - En/Dis I2C2
  // Bit[14] - En/Dis I2C3
  // Bit[15] - En/Dis I2C4
  // Bit[16] - En/Dis I2C5
  // Bit[17] - En/Dis UART2
  // Bit[18] - En/Dis SPI0
  // Bit[19] - En/Dis SPI1
  // Bit[20] - En/Dis SATA
  // Bit[21] - En/Dis RAID VOL0
  // Bit[22] - En/Dis SATA PORT0
  // Bit[23] - En/Dis SATA PORT1
  // Bit[24] - En/Dis SATA PORT2
  // Bit[25] - En/Dis SATA PORT3
  // Bit[26] - En/Dis SATA PORT4
  // Bit[27] - En/Dis SATA PORT5
  // Bit[28] - En/Dis SATA NVM1
  // Bit[29] - En/Dis SATA NVM2
  // Bit[30] - En/Dis SATA NVM3
  // Bit[31] - En/Dis CSME
  Offset(653),    VRSD, 16, // Offset(653),   VR Staggering delay
  Offset(655),    PB1E, 8,  // Offset(655),   10sec Power button support
  // Bit0: 10 sec P-button Enable/Disable
  // Bit1: Internal Flag
  // Bit2: Rotation Lock flag, 0:unlock, 1:lock
  // Bit3: Slate/Laptop Mode Flag, 0: Slate, 1: Laptop
  // Bit4: Undock / Dock Flag, 0: Undock, 1: Dock
  // Bit5: VBDL Flag. 0: VBDL is not called, 1: VBDL is called, Virtual Button Driver is loaded.
  // Bit6: Reserved for future use.
  // Bit7: EC 10sec PB Override state for S3/S4 wake up.
  //
  // Generation Id(Tock/Tick)
  //
  Offset(656),    GNID, 8,  // Offset(656),   Generation Id(0=Shark bay, 1=Crescent Bay)
  //
  // DPTF
  //
  Offset(657),    WAND, 8,  // Offset(657),   EnableWWANParticipant
  Offset(658),    WWAT, 8,  // Offset(658),   ActiveThermalTripPointWWAN
  Offset(659),    WWPT, 8,  // Offset(659),   PassiveThermalTripPointWWAN
  Offset(660),    WWCT, 8,  // Offset(660),   CriticalThermalTripPointWWAN
  Offset(661),    WWHT, 8,  // Offset(661),   HotThermalTripPointWWAN
  Offset(675),              // Offset(662) : Offset(674), Reserved bytes
  Offset(675),    MPL0, 16, // Offset(675),   Minimum Power Limit 0 for DPTF use via PPCC Object
  Offset(677),    CHGE, 8,  // Offset(677),   EnableChargerParticipant
  Offset(678),    SAC3, 8,  // Offset(678),   CriticalThermalTripPointSaS3
  Offset(679),    MEM3, 8,  // Offset(679),   CriticalThermalTripPointTMEMS3 @deprecated. Memory Participant is not POR for DPTF
  Offset(680),    AMC3, 8,  // Offset(680),   CriticalThermalTripPointAmbientS3
  Offset(681),    SKC3, 8,  // Offset(681),   CriticalThermalTripPointSkinS3
  Offset(682),    EFC3, 8,  // Offset(682),   CriticalThermalTripPointExhaustFanS3
  Offset(683),    VRC3, 8,  // Offset(683),   CriticalThermalTripPointVRS3
  Offset(684),    WFC3, 8,  // Offset(684),   CriticalThermalTripPointWifiS3
  Offset(685),    G1C3, 8,  // Offset(685),   CriticalThermalTripPointGen1S3
  Offset(686),    G2C3, 8,  // Offset(686),   CriticalThermalTripPointGen2S3
  Offset(687),    IFC3, 8,  // Offset(687),   CriticalThermalTripPointInletFanS3
  Offset(688),    WWC3, 8,  // Offset(688),   CriticalThermalTripPointWWANS3
  Offset(689),    WGC3, 8,  // Offset(689),   CriticalThermalTripPointWGigS3 @deprecated since revision 4.
  Offset(690),    SPST, 8,  // Offset(690),   SATA port state, Bit0 - Port0, Bit1 - Port1, Bit2 - Port2, Bit3 - Port3
  //
  // DPTF
  //
  Offset(691),    PERE, 8,  // Offset(691),   EnablePerCParticipant
  Offset(692),    PEAT, 8,  // Offset(692),   ActiveThermalTripPointPerC
  Offset(693),    PEPV, 8,  // Offset(693),   PassiveThermalTripPointPerC
  Offset(694),    PECR, 8,  // Offset(694),   CriticalThermalTripPointPerC
  Offset(695),    PEC3, 8,  // Offset(695),   CriticalThermalTripPointPerCS3
  Offset(696),    PEHT, 8,  // Offset(696),   HotThermalTripPointPerC
  Offset(697),    GN3E, 8,  // Offset(697),   EnableGen3Participant
  Offset(698),    G3AT, 8,  // Offset(698),   ActiveThermalTripPointGen3
  Offset(699),    G3PT, 8,  // Offset(699),   PassiveThermalTripPointGen3
  Offset(700),    G3CT, 8,  // Offset(700),   CriticalThermalTripPointGen3
  Offset(701),    G3HT, 8,  // Offset(701),   HotThermalTripPointGen3
  Offset(702),    GN4E, 8,  // Offset(702),   EnableGen4Participant
  Offset(703),    G4AT, 8,  // Offset(703),   ActiveThermalTripPointGen4
  Offset(704),    G4PT, 8,  // Offset(704),   PassiveThermalTripPointGen4
  Offset(705),    G4CT, 8,  // Offset(705),   CriticalThermalTripPointGen4
  Offset(706),    G4HT, 8,  // Offset(706),   HotThermalTripPointGen4
  Offset(707),    GN5E, 8,  // Offset(707),   EnableGen5Participant
  Offset(708),    G5AT, 8,  // Offset(708),   ActiveThermalTripPointGen5
  Offset(709),    G5PT, 8,  // Offset(709),   PassiveThermalTripPointGen5
  Offset(710),    G5CT, 8,  // Offset(710),   CriticalThermalTripPointGen5
  Offset(711),    G5HT, 8,  // Offset(711),   HotThermalTripPointGen5
  Offset(712),    GN6E, 8,  // Offset(712),   EnableGen6Participant
  Offset(713),    G6AT, 8,  // Offset(713),   ActiveThermalTripPointGen6
  Offset(714),    G6PT, 8,  // Offset(714),   PassiveThermalTripPointGen6
  Offset(715),    G6CT, 8,  // Offset(715),   CriticalThermalTripPointGen6
  Offset(716),    G6HT, 8,  // Offset(716),   HotThermalTripPointGen6
  Offset(717),    ECLP, 8,  // Offset(717),   EC Low Power Mode: 1 - Enabled, 0 - Disabled
  Offset(718),    G3C3, 8,  // Offset(718),   CriticalThermalTripPointGen3S3
  Offset(719),    G4C3, 8,  // Offset(719),   CriticalThermalTripPointGen4S3
  Offset(720),    G5C3, 8,  // Offset(720),   CriticalThermalTripPointGen5S3
  Offset(721),    G6C3, 8,  // Offset(721),   CriticalThermalTripPointGen6S3
  Offset(722),    TSP1, 8,  // Offset(722),   ThermistorSamplingPeriodGen1
  Offset(723),    TSP2, 8,  // Offset(723),   ThermistorSamplingPeriodGen2
  Offset(724),    TSP3, 8,  // Offset(724),   ThermistorSamplingPeriodGen3
  Offset(725),    TSP4, 8,  // Offset(725),   ThermistorSamplingPeriodGen4
  Offset(726),    TSP5, 8,  // Offset(726),   ThermistorSamplingPeriodGen5
  Offset(727),    TSP6, 8,  // Offset(727),   ThermistorSamplingPeriodGen6
  Offset(728),    TSP7, 8,  // Offset(728),   ThermistorSamplingPeriodGen7
  Offset(729),    TSP8, 8,  // Offset(729),   ThermistorSamplingPeriodGen8
  Offset(730),    SSP1, 8,  // Offset(730),   SensorSamplingPeriodSen1
  Offset(731),    SSP2, 8,  // Offset(731),   SensorSamplingPeriodSen2
  Offset(732),    SSP3, 8,  // Offset(732),   SensorSamplingPeriodSen3
  Offset(733),    SSP4, 8,  // Offset(733),   SensorSamplingPeriodSen4
  Offset(734),    SSP5, 8,  // Offset(734),   SensorSamplingPeriodSen5
  Offset(735),    SSP6, 8,  // Offset(735),   SensorSamplingPeriodSen6
  Offset(736),    SSP7, 8,  // Offset(736),   SensorSamplingPeriodSen7
  Offset(737),    SSP8, 8,  // Offset(737),   SensorSamplingPeriodSen8
  Offset(738),    MEMS, 8,  // Offset(738),   ThermalSamplingPeriodTMEM @deprecated. Memory Participant is not POR for DPTF
  Offset(739),    STGE, 8,  // Offset(739),   EnableStorageParticipant
  Offset(740),    STAT, 8,  // Offset(740),   ActiveThermalTripPointStrg
  Offset(741),    STPT, 8,  // Offset(741),   PassiveThermalTripPointStrg
  Offset(742),    STCT, 8,  // Offset(742),   CriticalThermalTripPointStrg
  Offset(743),    STC3, 8,  // Offset(743),   CriticalThermalTripPointStrgS3
  Offset(744),    STHT, 8,  // Offset(744),   HotThermalTripPointStrg
  Offset(745),    VSP1, 8,  // Offset(745),   EnableVS1Participant
  Offset(746),    V1AT, 8,  // Offset(746),   ActiveThermalTripPointVS1
  Offset(747),    V1PV, 8,  // Offset(747),   PassiveThermalTripPointVS1
  Offset(748),    V1CR, 8,  // Offset(748),   CriticalThermalTripPointVS1
  Offset(749),    V1C3, 8,  // Offset(749),   CriticalThermalTripPointVS1S3
  Offset(750),    V1HT, 8,  // Offset(750),   HotThermalTripPointVS1
  Offset(751),    VSP2, 8,  // Offset(751),   EnableVS2Participant
  Offset(752),    V2AT, 8,  // Offset(752),   ActiveThermalTripPointVS2
  Offset(753),    V2PV, 8,  // Offset(753),   PassiveThermalTripPointVS2
  Offset(754),    V2CR, 8,  // Offset(754),   CriticalThermalTripPointVS2
  Offset(755),    V2C3, 8,  // Offset(755),   CriticalThermalTripPointVS2S3
  Offset(756),    V2HT, 8,  // Offset(756),   HotThermalTripPointVS2
  Offset(757),    S1DE, 8,  // Offset(757),   EnableSen1Participant
  Offset(758),    S1AT, 8,  // Offset(758),   ActiveThermalTripPointSen1
  Offset(759),    S1PT, 8,  // Offset(759),   PassiveThermalTripPointSen1
  Offset(760),    S1CT, 8,  // Offset(760),   CriticalThermalTripPointSen1
  Offset(761),    S1HT, 8,  // Offset(761),   HotThermalTripPointSen1
  Offset(762),    S2DE, 8,  // Offset(762),   EnableSen2Participant
  Offset(763),    S2AT, 8,  // Offset(763),   ActiveThermalTripPointSen2
  Offset(764),    S2PT, 8,  // Offset(764),   PassiveThermalTripPointSen2
  Offset(765),    S2CT, 8,  // Offset(765),   CriticalThermalTripPointSen2
  Offset(766),    S2HT, 8,  // Offset(766),   HotThermalTripPointSen2
  Offset(767),    S3DE, 8,  // Offset(767),   EnableSen3Participant
  Offset(768),    S3AT, 8,  // Offset(768),   ActiveThermalTripPointSen3
  Offset(769),    S3PT, 8,  // Offset(769),   PassiveThermalTripPointSen3
  Offset(770),    S3CT, 8,  // Offset(770),   CriticalThermalTripPointSen3
  Offset(771),    S3HT, 8,  // Offset(771),   HotThermalTripPointSen3
  Offset(772),    S4DE, 8,  // Offset(772),   EnableSen4Participant
  Offset(773),    S4AT, 8,  // Offset(773),   ActiveThermalTripPointSen4
  Offset(774),    S4PT, 8,  // Offset(774),   PassiveThermalTripPointSen4
  Offset(775),    S4CT, 8,  // Offset(775),   CriticalThermalTripPointSen4
  Offset(776),    S4HT, 8,  // Offset(776),   HotThermalTripPointSen4
  Offset(777),    S5DE, 8,  // Offset(777),   EnableSen5Participant
  Offset(778),    S5AT, 8,  // Offset(778),   ActiveThermalTripPointSen5
  Offset(779),    S5PT, 8,  // Offset(779),   PassiveThermalTripPointSen5
  Offset(780),    S5CT, 8,  // Offset(780),   CriticalThermalTripPointSen5
  Offset(781),    S5HT, 8,  // Offset(781),   HotThermalTripPointSen5
  Offset(782),    S6DE, 8,  // Offset(782),   EnableSen6Participant
  Offset(783),    S6AT, 8,  // Offset(783),   ActiveThermalTripPointSen6
  Offset(784),    S6PT, 8,  // Offset(784),   PassiveThermalTripPointSen6
  Offset(785),    S6CT, 8,  // Offset(785),   CriticalThermalTripPointSen6
  Offset(786),    S6HT, 8,  // Offset(786),   HotThermalTripPointSen6
  Offset(787),    S7DE, 8,  // Offset(787),   EnableSen7Participant
  Offset(788),    S7AT, 8,  // Offset(788),   ActiveThermalTripPointSen7
  Offset(789),    S7PT, 8,  // Offset(789),   PassiveThermalTripPointSen7
  Offset(790),    S7CT, 8,  // Offset(790),   CriticalThermalTripPointSen7
  Offset(791),    S7HT, 8,  // Offset(791),   HotThermalTripPointSen7
  Offset(792),    S1S3, 8,  // Offset(792),   CriticalThermalTripPointSen1S3
  Offset(793),    S2S3, 8,  // Offset(793),   CriticalThermalTripPointSen2S3
  Offset(794),    S3S3, 8,  // Offset(794),   CriticalThermalTripPointSen3S3
  Offset(795),    S4S3, 8,  // Offset(795),   CriticalThermalTripPointSen4S3
  Offset(796),    S5S3, 8,  // Offset(796),   CriticalThermalTripPointSen5S3
  Offset(797),    S6S3, 8,  // Offset(797),   CriticalThermalTripPointSen6S3
  Offset(798),    S7S3, 8,  // Offset(798),   CriticalThermalTripPointSen7S3
  Offset(799),    ICAE, 8,  // Offset(799),   EnableIvCamParticipant
  Offset(800),    PSME, 8,  // Offset(800),   PowerSharingManagerEnable
  Offset(801),    PDT1, 8,  // Offset(801),   PsmSplcDomainType1
  Offset(802),    PLM1, 32, // Offset(802),   PsmSplcPowerLimit1
  Offset(806),    PTW1, 32, // Offset(806),   PsmSplcTimeWindow1
  Offset(810),    PDT2, 8,  // Offset(810),   PsmSplcDomainType2
  Offset(811),    PLM2, 32, // Offset(811),   PsmSplcPowerLimit2
  Offset(815),    PTW2, 32, // Offset(815),   PsmSplcTimeWindow2
  Offset(819),    DDT1, 8,  // Offset(819),   PsmDplcDomainType1
  Offset(820),    DDP1, 8,  // Offset(820),   PsmDplcDomainPreference1
  Offset(821),    DLI1, 16, // Offset(821),   PsmDplcPowerLimitIndex1
  Offset(823),    DPL1, 16, // Offset(823),   PsmDplcDefaultPowerLimit1
  Offset(825),    DTW1, 32, // Offset(825),   PsmDplcDefaultTimeWindow1
  Offset(829),    DMI1, 16, // Offset(829),   PsmDplcMinimumPowerLimit1
  Offset(831),    DMA1, 16, // Offset(831),   PsmDplcMaximumPowerLimit1
  Offset(833),    DMT1, 16, // Offset(833),   PsmDplcMaximumTimeWindow1
  Offset(835),    DDT2, 8,  // Offset(835),   PsmDplcDomainType2
  Offset(836),    DDP2, 8,  // Offset(836),   PsmDplcDomainPreference2
  Offset(837),    DLI2, 16, // Offset(837),   PsmDplcPowerLimitIndex2
  Offset(839),    DPL2, 16, // Offset(839),   PsmDplcDefaultPowerLimit2
  Offset(841),    DTW2, 32, // Offset(841),   PsmDplcDefaultTimeWindow2
  Offset(845),    DMI2, 16, // Offset(845),   PsmDplcMinimumPowerLimit2
  Offset(847),    DMA2, 16, // Offset(847),   PsmDplcMaximumPowerLimit2
  Offset(849),    DMT2, 16, // Offset(849),   PsmDplcMaximumTimeWindow2
  Offset(851),    WIFE, 8,  // Offset(851),   WifiEnable
  Offset(852),    DOM1, 8,  // Offset(852),   WifiDomainType1
  Offset(853),    LIM1, 16, // Offset(853),   WifiPowerLimit1
  Offset(855),    TIM1, 32, // Offset(855),   WifiTimeWindow1
  Offset(859),    DOM2, 8,  // Offset(859),   WifiDomainType2
  Offset(860),    LIM2, 16, // Offset(860),   WifiPowerLimit2
  Offset(862),    TIM2, 32, // Offset(862),   WifiTimeWindow2
  Offset(866),    DOM3, 8,  // Offset(866),   WifiDomainType3
  Offset(867),    LIM3, 16, // Offset(867),   WifiPowerLimit3
  Offset(869),    TIM3, 32, // Offset(869),   WifiTimeWindow3
  Offset(873),    TRD0, 8,  // Offset(873),   TRxDelay0
  Offset(874),    TRL0, 8,  // Offset(874),   TRxCableLength0
  Offset(875),    TRD1, 8,  // Offset(875),   TRxDelay1
  Offset(876),    TRL1, 8,  // Offset(876),   TRxCableLength1
  Offset(877),    WDM1, 8,  // Offset(877),   WrddDomainType1
  Offset(878),    CID1, 16, // Offset(878),   WrddCountryIndentifier1
  Offset(880),    WDM2, 8,  // Offset(880),   WrddDomainType2
  Offset(881),    CID2, 16, // Offset(881),   WrddCountryIndentifier2
  Offset(888),              // Offset(883) : Offset(887), Reserved bytes
  Offset(888),    APPE, 8,  // Offset(888),   Adaptive Performance Policy
  Offset(889),    MPL1, 16, // Offset(889),   Minimum Power Limit 1 for DPTF use via PPCC Object
  Offset(891),    MPL2, 16, // Offset(891),   Minimum Power Limit 2 for DPTF use via PPCC Object
  //
  // Intel Serial(R) IO Sensor Device Selection
  //
  Offset(893),    SDS0, 8,  // Offset(893),   SerialIo Devices for controller0
  Offset(894),    SDS1, 8,  // Offset(894),   SerialIo Devices for controller1
  Offset(895),    SDS2, 8,  // Offset(895),   SerialIo Devices for controller2
  Offset(896),    SDS3, 8,  // Offset(896),   SerialIo Devices for controller3
  Offset(897),    SDS4, 8,  // Offset(897),   SerialIo Devices for controller4
  Offset(898),    SDS5, 8,  // Offset(898),   SerialIo Devices for controller5
  Offset(899),    SDS6, 8,  // Offset(899),   SerialIo Devices for controller6
  Offset(900),    SDS7, 8,  // Offset(900),   SerialIo Devices for controller7
  Offset(901),    SDS8, 8,  // Offset(901),   SerialIo Devices for controller8
  Offset(902),    SDS9, 8,  // Offset(902),   SerialIo Devices for controller9
  Offset(903),    SDSA, 8,  // Offset(903),   SerialIo Devices for controller10
  Offset(904),    TPLB, 8,  // Offset(904),   custom touchpanel's bus address
  Offset(905),    TPLH, 16, // Offset(905),   custom touchpanel's HID address
  Offset(907),    WTVX, 8,  // Offset(907),   WITT test devices' version
  Offset(908),    WITX, 8,  // Offset(908),   WITT test devices' connection point
  Offset(909),    GPTD, 8,  // Offset(909),   GPIO test devices
  Offset(910),    GDBT, 16, // Offset(910),   GPIO test devices' debounce value,
  Offset(912),    UTKX, 8,  // Offset(912),   UTK test devices' connection point
  Offset(913),    SPTD, 8,  // Offset(913),   SerialIo additional test devices
  Offset(914),    GEXN, 8,  // Offset(914),   Gpio expander's bus number
  Offset(915),    TBTS, 8,  // Offset(915),   Thunderbolt(TM) support
  Offset(916),    TBWS, 8,  // Offset(916),   Wake From Thunderbolt(TM) Devices
  Offset(917),    AICS, 8,  // Offset(917),   Thunderbolt(TM) AR AIC support
  Offset(918),    TARS, 8,  // Offset(918),   TbtAcpiRemovalSupport
  Offset(919),    FPAT, 8,  // Offset(919),   TbtFrcPwrGpioAccessType
  Offset(920),    FPEN, 8,  // Offset(920),   TbtFrcPwrExpander
  Offset(921),    FPGN, 32, // Offset(921),   TbtFrcPwrGpioNo
  Offset(925),    FPLV, 8,  // Offset(925),   TbtFrcPwrGpioLevel
  Offset(926),    CPAD, 16, // Offset(926),   TbtCioPlugEvtAcpiGpeStsAddress
  Offset(928),    CPAB, 8,  // Offset(928),   TbtCioPlugEvtAcpiGpeStsBit
  Offset(929),    TNAT, 8,  // Offset(929),   TbtNativeOsHotPlug @deprecated since revision 25.
  Offset(930),    CPGN, 32, // Offset(930),   TbtCioPlugEventGpioNo
  Offset(934),    CF2T, 8,  // Offset(934),   Check for 2tier support
  Offset(935),    TDGS, 8,  // Offset(935),   ThunderBolt Detachable Graphics Support
  Offset(936),    DCSC, 8,  // Offset(936),   Discrete GPU display connect state change request
  Offset(937),    DCKE, 8,  // Offset(937),   Dock event
  Offset(938),    UDCK, 8,  // Offset(938),   Undock request
  Offset(939),    SUDK, 8,  // Offset(939),   Surprise undock event
  Offset(940),    OHPN, 8,  // Offset(940),   OS Hot Plug/Unplug Notifications
  Offset(941),    GHPN, 8,  // Offset(941),   GPU Hot Plug/Unplug Notifications
  Offset(942),    EGPC, 32, // Offset(942),   External GPU PCI Configuration Space Buffer Address
  Offset(946),    EGPV, 8,  // Offset(946),   External GPU PCI Configuration Space Valid
  Offset(947),    TBDT, 32, // Offset(947),   Thunderbolt Device Tree Info Buffer Address
  Offset(951),    ATLB, 32, // Offset(951),   Buffer for runtime ACPI Table loading
  Offset(955),    SDM0, 8,  // Offset(955),   interrupt mode for controller0 devices
  Offset(956),    SDM1, 8,  // Offset(956),   interrupt mode for controller1 devices
  Offset(957),    SDM2, 8,  // Offset(957),   interrupt mode for controller2 devices
  Offset(958),    SDM3, 8,  // Offset(958),   interrupt mode for controller3 devices
  Offset(959),    SDM4, 8,  // Offset(959),   interrupt mode for controller4 devices
  Offset(960),    SDM5, 8,  // Offset(960),   interrupt mode for controller5 devices
  Offset(961),    SDM6, 8,  // Offset(961),   interrupt mode for controller6 devices
  Offset(962),    SDM7, 8,  // Offset(962),   interrupt mode for controller7 devices
  Offset(963),    SDM8, 8,  // Offset(963),   interrupt mode for controller8 devices
  Offset(964),    SDM9, 8,  // Offset(964),   interrupt mode for controller9 devices
  Offset(965),    SDMA, 8,  // Offset(965),   interrupt mode for controller10 devices
  Offset(966),    USTP, 8,  // Offset(966),   use SerialIo timing parameters
  Offset(967),    SSHI, 16, // Offset(967),   SSCN-HIGH for I2C
  Offset(969),    SSLI, 16, // Offset(969),   SSCN-LOW  for I2C
  Offset(971),    SSDI, 16, // Offset(971),   SSCN-HOLD for I2C
  Offset(973),    FMHI, 16, // Offset(973),   FMCN-HIGH for I2C
  Offset(975),    FMLI, 16, // Offset(975),   FMCN-LOW  for I2C
  Offset(977),    FMDI, 16, // Offset(977),   FMCN-HOLD for I2C
  Offset(979),    FPHI, 16, // Offset(979),   FPCN-HIGH for I2C
  Offset(981),    FPLI, 16, // Offset(981),   FPCN-LOW  for I2C
  Offset(983),    FPDI, 16, // Offset(983),   FPCN-HOLD for I2C
  Offset(985),    M0CI, 16, // Offset(985),   M0D3 for I2C
  Offset(987),    M1CI, 16, // Offset(987),   M1D3 for I2C
  Offset(989),    M0CS, 16, // Offset(989),   M0D3 for SPI
  Offset(991),    M1CS, 16, // Offset(991),   M1D3 for SPI
  Offset(993),    M0CU, 16, // Offset(993),   M0D3 for UART
  Offset(995),    M1CU, 16, // Offset(995),   M1D3 for UART
  //
  // Camera specific policies - Begin
  //
  Offset(997),    CAMT, 8,  // Offset(997),   Type of Camera attached to the System
  Offset(998),    IVDF, 8,  // Offset(998),   3D Camera DFU support
  Offset(999),    IFWG, 64, // Offset(999),   3D Camera FW Gpio
  Offset(1007),   IVWS, 8,  // Offset(1007),  3D Camera wake support
  Offset(1008),   IVPR, 8,  // Offset(1008),  Ivcam present in platform
  Offset(1009),   DIVO, 16, // Offset(1009),  Delay needed for Ivcam power on
  Offset(1011),   DIVF, 16, // Offset(1011),  Delay needed for Ivcam power off
  Offset(1013),   IVAD, 8,  // Offset(1013),  Ivcam endpoint address
  Offset(1014),   IVRS, 8,  // Offset(1014),  Ivcam rotation support
  Offset(1015),   IVDG, 64, // Offset(1015),  3D Camera VDD Gpio
  Offset(1023),   DSPR, 8,  // Offset(1023),  DS present in platform
  Offset(1024),   DDSO, 16, // Offset(1024),  Delay needed for DS camera power on
  Offset(1026),   DDSF, 16, // Offset(1026),  Delay needed for DS camera power off
  Offset(1028),   DSAD, 8,  // Offset(1028),  DS endpoint address
  Offset(1029),   DSRS, 8,  // Offset(1029),  DS camera rotation support
  Offset(1030),   DVDG, 64, // Offset(1030),  DS Camera VDD Gpio
  //
  // Camera specific policies - End
  //
  Offset(1038),   EIDF, 8,  // Offset(1038),  EInk DFU enable
  Offset(1039),   GFPS, 32, // Offset(1039),  Gpio for fingerprint sleep
  Offset(1043),   GFPI, 32, // Offset(1043),  Gpio for fingerprint irq
  Offset(1047),   GNSM, 8,  // Offset(1047),  GNSS device model
  Offset(1048),   GNSC, 8,  // Offset(1048),  GNSS connection, 0=disabled, 1=SerialIO, 2=ISH
  Offset(1049),   GGNR, 32, // Offset(1049),  Gpio for GNSS reset
  Offset(1053),   GBTW, 32, // Offset(1053),  Gpio for BlueTooth Wake
  Offset(1057),   GBTK, 32, // Offset(1057),  Gpio for BlueTooth rfKill
  Offset(1061),   GBTI, 32, // Offset(1061),  Gpio for BlueTooth Interrupt
  Offset(1065),   GPDI, 32, // Offset(1065),  Gpio for touchPaD Interrupt
  Offset(1069),   GPLI, 32, // Offset(1069),  Gpio for touchPaneL Interrupt
  //
  // SkyCam specific
  //
  Offset(1073),   CL00, 8,  // Offset(1073),
  Offset(1074),   CL01, 8,  // Offset(1074),
  Offset(1075),   CL02, 8,  // Offset(1075),
  Offset(1076),   CL03, 8,  // Offset(1076),
  Offset(1077),   L0EN, 8,  // Offset(1077),
  Offset(1078),   L1EN, 8,  // Offset(1078),
  Offset(1079),   L2EN, 8,  // Offset(1079),
  Offset(1080),   L3EN, 8,  // Offset(1080),
  Offset(1081),   CDIV, 8,  // Offset(1081),
  // Control Logic 0 options
  Offset(1082),   C0TP, 8,  // Offset(1082),  Type
  Offset(1083),   C0CV, 8,  // Offset(1083),  Version of CRD
  Offset(1084),   C0GP, 8,  // Offset(1084),  Number of GPIO Pins enabled
  Offset(1085),   C0IB, 8,  // Offset(1085),  I2C Serial Bus Number
  Offset(1086),   C0IA, 16, // Offset(1086),  I2C Address
  Offset(1088),   C0P0, 8,  // Offset(1088),  GPIO Group Pad Number
  Offset(1089),   C0P1, 8,  // Offset(1089),
  Offset(1090),   C0P2, 8,  // Offset(1090),
  Offset(1091),   C0P3, 8,  // Offset(1091),
  Offset(1092),   C0G0, 8,  // Offset(1092),  GPIO Group Number
  Offset(1093),   C0G1, 8,  // Offset(1093),
  Offset(1094),   C0G2, 8,  // Offset(1094),
  Offset(1095),   C0G3, 8,  // Offset(1095),
  Offset(1096),   C0F0, 8,  // Offset(1096),  GPIO Function
  Offset(1097),   C0F1, 8,  // Offset(1097),
  Offset(1098),   C0F2, 8,  // Offset(1098),
  Offset(1099),   C0F3, 8,  // Offset(1099),
  Offset(1100),   C0A0, 8,  // Offset(1100),  GPIO Active Value
  Offset(1101),   C0A1, 8,  // Offset(1101),
  Offset(1102),   C0A2, 8,  // Offset(1102),
  Offset(1103),   C0A3, 8,  // Offset(1103),
  Offset(1104),   C0I0, 8,  // Offset(1104),  GPIO Initial Value
  Offset(1105),   C0I1, 8,  // Offset(1105),
  Offset(1106),   C0I2, 8,  // Offset(1106),
  Offset(1107),   C0I3, 8,  // Offset(1107),
  Offset(1108),   C0PL, 8,  // Offset(1108),  Camera Position
  // Control Logic 1 options
  Offset(1109),   C1TP, 8,  // Offset(1109),  Type
  Offset(1110),   C1CV, 8,  // Offset(1110),  Version of CRD
  Offset(1111),   C1GP, 8,  // Offset(1111),  Number of GPIO Pins enabled
  Offset(1112),   C1IB, 8,  // Offset(1112),  I2C Serial Bus Number
  Offset(1113),   C1IA, 16, // Offset(1113),  I2C Address
  Offset(1115),   C1P0, 8,  // Offset(1115),  GPIO Group Pad Number
  Offset(1116),   C1P1, 8,  // Offset(1116),
  Offset(1117),   C1P2, 8,  // Offset(1117),
  Offset(1118),   C1P3, 8,  // Offset(1118),
  Offset(1119),   C1G0, 8,  // Offset(1119),  GPIO Group Number
  Offset(1120),   C1G1, 8,  // Offset(1120),
  Offset(1121),   C1G2, 8,  // Offset(1121),
  Offset(1122),   C1G3, 8,  // Offset(1122),
  Offset(1123),   C1F0, 8,  // Offset(1123),  GPIO Function
  Offset(1124),   C1F1, 8,  // Offset(1124),
  Offset(1125),   C1F2, 8,  // Offset(1125),
  Offset(1126),   C1F3, 8,  // Offset(1126),
  Offset(1127),   C1A0, 8,  // Offset(1127),  GPIO Active Value
  Offset(1128),   C1A1, 8,  // Offset(1128),
  Offset(1129),   C1A2, 8,  // Offset(1129),
  Offset(1130),   C1A3, 8,  // Offset(1130),
  Offset(1131),   C1I0, 8,  // Offset(1131),  GPIO Initial Value
  Offset(1132),   C1I1, 8,  // Offset(1132),
  Offset(1133),   C1I2, 8,  // Offset(1133),
  Offset(1134),   C1I3, 8,  // Offset(1134),
  Offset(1135),   C1PL, 8,  // Offset(1135),  Camera Position
  // Control Logic 2 options
  Offset(1136),   C2TP, 8,  // Offset(1136),  Type
  Offset(1137),   C2CV, 8,  // Offset(1137),  Version of CRD
  Offset(1138),   C2GP, 8,  // Offset(1138),  Number of GPIO Pins enabled
  Offset(1139),   C2IB, 8,  // Offset(1139),  I2C Serial Bus Number
  Offset(1140),   C2IA, 16, // Offset(1140),  I2C Address
  Offset(1142),   C2P0, 8,  // Offset(1142),  GPIO Group Pad Number
  Offset(1143),   C2P1, 8,  // Offset(1143),
  Offset(1144),   C2P2, 8,  // Offset(1144),
  Offset(1145),   C2P3, 8,  // Offset(1145),
  Offset(1146),   C2G0, 8,  // Offset(1146),  GPIO Group Number
  Offset(1147),   C2G1, 8,  // Offset(1147),
  Offset(1148),   C2G2, 8,  // Offset(1148),
  Offset(1149),   C2G3, 8,  // Offset(1149),
  Offset(1150),   C2F0, 8,  // Offset(1150),  GPIO Function
  Offset(1151),   C2F1, 8,  // Offset(1151),
  Offset(1152),   C2F2, 8,  // Offset(1152),
  Offset(1153),   C2F3, 8,  // Offset(1153),
  Offset(1154),   C2A0, 8,  // Offset(1154),  GPIO Active Value
  Offset(1155),   C2A1, 8,  // Offset(1155),
  Offset(1156),   C2A2, 8,  // Offset(1156),
  Offset(1157),   C2A3, 8,  // Offset(1157),
  Offset(1158),   C2I0, 8,  // Offset(1158),  GPIO Initial Value
  Offset(1159),   C2I1, 8,  // Offset(1159),
  Offset(1160),   C2I2, 8,  // Offset(1160),
  Offset(1161),   C2I3, 8,  // Offset(1161),
  Offset(1162),   C2PL, 8,  // Offset(1162),  Camera Position
  // Control Logic 3 options
  Offset(1163),   C3TP, 8,  // Offset(1163),  Type
  Offset(1164),   C3CV, 8,  // Offset(1164),  Version of CRD
  Offset(1165),   C3GP, 8,  // Offset(1165),  Number of GPIO Pins enabled
  Offset(1166),   C3IB, 8,  // Offset(1166),  I2C Serial Bus Number
  Offset(1167),   C3IA, 16, // Offset(1167),  I2C Address
  Offset(1169),   C3P0, 8,  // Offset(1169),  GPIO Group Pad Number
  Offset(1170),   C3P1, 8,  // Offset(1170),
  Offset(1171),   C3P2, 8,  // Offset(1171),
  Offset(1172),   C3P3, 8,  // Offset(1172),
  Offset(1173),   C3G0, 8,  // Offset(1173),  GPIO Group Number
  Offset(1174),   C3G1, 8,  // Offset(1174),
  Offset(1175),   C3G2, 8,  // Offset(1175),
  Offset(1176),   C3G3, 8,  // Offset(1176),
  Offset(1177),   C3F0, 8,  // Offset(1177),  GPIO Function
  Offset(1178),   C3F1, 8,  // Offset(1178),
  Offset(1179),   C3F2, 8,  // Offset(1179),
  Offset(1180),   C3F3, 8,  // Offset(1180),
  Offset(1181),   C3A0, 8,  // Offset(1181),  GPIO Active Value
  Offset(1182),   C3A1, 8,  // Offset(1182),
  Offset(1183),   C3A2, 8,  // Offset(1183),
  Offset(1184),   C3A3, 8,  // Offset(1184),
  Offset(1185),   C3I0, 8,  // Offset(1185),  GPIO Initial Value
  Offset(1186),   C3I1, 8,  // Offset(1186),
  Offset(1187),   C3I2, 8,  // Offset(1187),
  Offset(1188),   C3I3, 8,  // Offset(1188),
  Offset(1189),   C3PL, 8,  // Offset(1189),  Camera Position
  // Sky Cam Link0 options
  Offset(1190),   L0SM, 8,  // Offset(1190),  Sensor Model
  Offset(1191),   L0H0, 8,  // Offset(1191),  User defined HID ASCII character 0
  Offset(1192),   L0H1, 8,  // Offset(1192),
  Offset(1193),   L0H2, 8,  // Offset(1193),
  Offset(1194),   L0H3, 8,  // Offset(1194),
  Offset(1195),   L0H4, 8,  // Offset(1195),
  Offset(1196),   L0H5, 8,  // Offset(1196),
  Offset(1197),   L0H6, 8,  // Offset(1197),
  Offset(1198),   L0H7, 8,  // Offset(1198),
  Offset(1199),   L0H8, 8,  // Offset(1199),  User defined HID ASCII character 8
  Offset(1200),   L0PL, 8,  // Offset(1200),  Camera Position
  Offset(1201),   L0M0, 8,  // Offset(1201),  Camera Module Name ASCII character 0
  Offset(1202),   L0M1, 8,  // Offset(1202),
  Offset(1203),   L0M2, 8,  // Offset(1203),
  Offset(1204),   L0M3, 8,  // Offset(1204),
  Offset(1205),   L0M4, 8,  // Offset(1205),
  Offset(1206),   L0M5, 8,  // Offset(1206),
  Offset(1207),   L0M6, 8,  // Offset(1207),
  Offset(1208),   L0M7, 8,  // Offset(1208),
  Offset(1209),   L0M8, 8,  // Offset(1209),
  Offset(1210),   L0M9, 8,  // Offset(1210),
  Offset(1211),   L0MA, 8,  // Offset(1211),
  Offset(1212),   L0MB, 8,  // Offset(1212),
  Offset(1213),   L0MC, 8,  // Offset(1213),
  Offset(1214),   L0MD, 8,  // Offset(1214),
  Offset(1215),   L0ME, 8,  // Offset(1215),
  Offset(1216),   L0MF, 8,  // Offset(1216),  Camera Module Name ASCII character 15
  Offset(1217),   L0DI, 8,  // Offset(1217),  Number of I2C devices
  Offset(1218),   L0BS, 8,  // Offset(1218),  I2C Serial Bus number
  Offset(1219),   L0A0, 16, // Offset(1219),  Address of I2C Device0 on Link0
  Offset(1221),   L0A1, 16, // Offset(1221),  Address of I2C Device1 on Link0
  Offset(1223),   L0A2, 16, // Offset(1223),  Address of I2C Device2 on Link0
  Offset(1225),   L0A3, 16, // Offset(1225),  Address of I2C Device3 on Link0
  Offset(1227),   L0A4, 16, // Offset(1227),  Address of I2C Device4 on Link0
  Offset(1229),   L0A5, 16, // Offset(1229),  Address of I2C Device5 on Link0
  Offset(1231),   L0A6, 16, // Offset(1231),  Address of I2C Device6 on Link0
  Offset(1233),   L0A7, 16, // Offset(1233),  Address of I2C Device7 on Link0
  Offset(1235),   L0A8, 16, // Offset(1235),  Address of I2C Device8 on Link0
  Offset(1237),   L0A9, 16, // Offset(1237),  Address of I2C Device9 on Link0
  Offset(1239),   L0AA, 16, // Offset(1239),  Address of I2C Device10 on Link0
  Offset(1241),   L0AB, 16, // Offset(1241),  Address of I2C Device11 on Link0
  Offset(1243),   L0D0, 8,  // Offset(1243),  Type of I2C Device0 on Link0
  Offset(1244),   L0D1, 8,  // Offset(1244),  Type of I2C Device1 on Link0
  Offset(1245),   L0D2, 8,  // Offset(1245),  Type of I2C Device2 on Link0
  Offset(1246),   L0D3, 8,  // Offset(1246),  Type of I2C Device3 on Link0
  Offset(1247),   L0D4, 8,  // Offset(1247),  Type of I2C Device4 on Link0
  Offset(1248),   L0D5, 8,  // Offset(1248),  Type of I2C Device5 on Link0
  Offset(1249),   L0D6, 8,  // Offset(1249),  Type of I2C Device6 on Link0
  Offset(1250),   L0D7, 8,  // Offset(1250),  Type of I2C Device7 on Link0
  Offset(1251),   L0D8, 8,  // Offset(1251),  Type of I2C Device8 on Link0
  Offset(1252),   L0D9, 8,  // Offset(1252),  Type of I2C Device9 on Link0
  Offset(1253),   L0DA, 8,  // Offset(1253),  Type of I2C Device10 on Link0
  Offset(1254),   L0DB, 8,  // Offset(1254),  Type of I2C Device11 on Link0
  Offset(1255),   L0DV, 8,  // Offset(1255),  Version of SSDB structure
  Offset(1256),   L0CV, 8,  // Offset(1256),  Version of CRD
  Offset(1257),   L0LU, 8,  // Offset(1257),  CSI2 Link used
  Offset(1258),   L0NL, 8,  // Offset(1258),  MIPI-CSI2 Data Lane
  Offset(1259),   L0EE, 8,  // Offset(1259),  EEPROM Type
  Offset(1260),   L0VC, 8,  // Offset(1260),  VCM Type
  Offset(1261),   L0FS, 8,  // Offset(1261),  Flash Support
  Offset(1262),   L0DG, 8,  // Offset(1262),  Degree
  Offset(1263),   L0C0, 8,  // Offset(1263),  Platform @deprecated since revision 9.
  Offset(1264),   L0C1, 8,  // Offset(1264),  PlatformSub @deprecated since revision 9.
  Offset(1265),   L0C2, 8,  // Offset(1265),  pLED @deprecated since revision 9.
  Offset(1266),   L0C3, 8,  // Offset(1266),  mipiDefine @deprecated since revision 9.
  Offset(1267),   L0CK, 32, // Offset(1267),  MCLK
  Offset(1271),   L0CL, 8,  // Offset(1271),  Control Logic
  // Sky Cam Link1 options
  Offset(1272),   L1SM, 8,  // Offset(1272),  Sensor Model
  Offset(1273),   L1H0, 8,  // Offset(1273),  User defined HID ASCII character 0
  Offset(1274),   L1H1, 8,  // Offset(1274),
  Offset(1275),   L1H2, 8,  // Offset(1275),
  Offset(1276),   L1H3, 8,  // Offset(1276),
  Offset(1277),   L1H4, 8,  // Offset(1277),
  Offset(1278),   L1H5, 8,  // Offset(1278),
  Offset(1279),   L1H6, 8,  // Offset(1279),
  Offset(1280),   L1H7, 8,  // Offset(1280),
  Offset(1281),   L1H8, 8,  // Offset(1281),  User defined HID ASCII character 8
  Offset(1282),   L1PL, 8,  // Offset(1282),  Camera Position
  Offset(1283),   L1M0, 8,  // Offset(1283),  Camera Module Name ASCII character 0
  Offset(1284),   L1M1, 8,  // Offset(1284),
  Offset(1285),   L1M2, 8,  // Offset(1285),
  Offset(1286),   L1M3, 8,  // Offset(1286),
  Offset(1287),   L1M4, 8,  // Offset(1287),
  Offset(1288),   L1M5, 8,  // Offset(1288),
  Offset(1289),   L1M6, 8,  // Offset(1289),
  Offset(1290),   L1M7, 8,  // Offset(1290),
  Offset(1291),   L1M8, 8,  // Offset(1291),
  Offset(1292),   L1M9, 8,  // Offset(1292),
  Offset(1293),   L1MA, 8,  // Offset(1293),
  Offset(1294),   L1MB, 8,  // Offset(1294),
  Offset(1295),   L1MC, 8,  // Offset(1295),
  Offset(1296),   L1MD, 8,  // Offset(1296),
  Offset(1297),   L1ME, 8,  // Offset(1297),
  Offset(1298),   L1MF, 8,  // Offset(1298),  Camera Module Name ASCII character 15
  Offset(1299),   L1DI, 8,  // Offset(1299),  Number of I2C devices
  Offset(1300),   L1BS, 8,  // Offset(1300),  I2C Serial Bus number
  Offset(1301),   L1A0, 16, // Offset(1301),  Address of I2C Device0 on Link1
  Offset(1303),   L1A1, 16, // Offset(1303),  Address of I2C Device1 on Link1
  Offset(1305),   L1A2, 16, // Offset(1305),  Address of I2C Device2 on Link1
  Offset(1307),   L1A3, 16, // Offset(1307),  Address of I2C Device3 on Link1
  Offset(1309),   L1A4, 16, // Offset(1309),  Address of I2C Device4 on Link1
  Offset(1311),   L1A5, 16, // Offset(1311),  Address of I2C Device5 on Link1
  Offset(1313),   L1A6, 16, // Offset(1313),  Address of I2C Device6 on Link1
  Offset(1315),   L1A7, 16, // Offset(1315),  Address of I2C Device7 on Link1
  Offset(1317),   L1A8, 16, // Offset(1317),  Address of I2C Device8 on Link1
  Offset(1319),   L1A9, 16, // Offset(1319),  Address of I2C Device9 on Link1
  Offset(1321),   L1AA, 16, // Offset(1321),  Address of I2C Device10 on Link1
  Offset(1323),   L1AB, 16, // Offset(1323),  Address of I2C Device11 on Link1
  Offset(1325),   L1D0, 8,  // Offset(1325),  Type of I2C Device0 on Link1
  Offset(1326),   L1D1, 8,  // Offset(1326),  Type of I2C Device1 on Link1
  Offset(1327),   L1D2, 8,  // Offset(1327),  Type of I2C Device2 on Link1
  Offset(1328),   L1D3, 8,  // Offset(1328),  Type of I2C Device3 on Link1
  Offset(1329),   L1D4, 8,  // Offset(1329),  Type of I2C Device4 on Link1
  Offset(1330),   L1D5, 8,  // Offset(1330),  Type of I2C Device5 on Link1
  Offset(1331),   L1D6, 8,  // Offset(1331),  Type of I2C Device6 on Link1
  Offset(1332),   L1D7, 8,  // Offset(1332),  Type of I2C Device7 on Link1
  Offset(1333),   L1D8, 8,  // Offset(1333),  Type of I2C Device8 on Link1
  Offset(1334),   L1D9, 8,  // Offset(1334),  Type of I2C Device9 on Link1
  Offset(1335),   L1DA, 8,  // Offset(1335),  Type of I2C Device10 on Link1
  Offset(1336),   L1DB, 8,  // Offset(1336),  Type of I2C Device11 on Link1
  Offset(1337),   L1DV, 8,  // Offset(1337),  Version of SSDB structure
  Offset(1338),   L1CV, 8,  // Offset(1338),  Version of CRD
  Offset(1339),   L1LU, 8,  // Offset(1339),  CSI2 Link used
  Offset(1340),   L1NL, 8,  // Offset(1340),  MIPI-CSI2 Data Lane
  Offset(1341),   L1EE, 8,  // Offset(1341),  EEPROM Type
  Offset(1342),   L1VC, 8,  // Offset(1342),  VCM Type
  Offset(1343),   L1FS, 8,  // Offset(1343),  Flash Support
  Offset(1344),   L1DG, 8,  // Offset(1344),  Degree
  Offset(1345),   L1C0, 8,  // Offset(1345),  Platform @deprecated since revision 9.
  Offset(1346),   L1C1, 8,  // Offset(1346),  PlatformSub @deprecated since revision 9.
  Offset(1347),   L1C2, 8,  // Offset(1347),  pLED @deprecated since revision 9.
  Offset(1348),   L1C3, 8,  // Offset(1348),  mipiDefine @deprecated since revision 9.
  Offset(1349),   L1CK, 32, // Offset(1349),  MCLK
  Offset(1353),   L1CL, 8,  // Offset(1353),  Control Logic
  // Sky Cam Link2 options
  Offset(1354),   L2SM, 8,  // Offset(1354),  Sensor Model
  Offset(1355),   L2H0, 8,  // Offset(1355),  User defined HID ASCII character 0
  Offset(1356),   L2H1, 8,  // Offset(1356),
  Offset(1357),   L2H2, 8,  // Offset(1357),
  Offset(1358),   L2H3, 8,  // Offset(1358),
  Offset(1359),   L2H4, 8,  // Offset(1359),
  Offset(1360),   L2H5, 8,  // Offset(1360),
  Offset(1361),   L2H6, 8,  // Offset(1361),
  Offset(1362),   L2H7, 8,  // Offset(1362),
  Offset(1363),   L2H8, 8,  // Offset(1363),  User defined HID ASCII character 8
  Offset(1364),   L2PL, 8,  // Offset(1364),  Camera Position
  Offset(1365),   L2M0, 8,  // Offset(1365),  Camera Module Name ASCII character 0
  Offset(1366),   L2M1, 8,  // Offset(1366),
  Offset(1367),   L2M2, 8,  // Offset(1367),
  Offset(1368),   L2M3, 8,  // Offset(1368),
  Offset(1369),   L2M4, 8,  // Offset(1369),
  Offset(1370),   L2M5, 8,  // Offset(1370),
  Offset(1371),   L2M6, 8,  // Offset(1371),
  Offset(1372),   L2M7, 8,  // Offset(1372),
  Offset(1373),   L2M8, 8,  // Offset(1373),
  Offset(1374),   L2M9, 8,  // Offset(1374),
  Offset(1375),   L2MA, 8,  // Offset(1375),
  Offset(1376),   L2MB, 8,  // Offset(1376),
  Offset(1377),   L2MC, 8,  // Offset(1377),
  Offset(1378),   L2MD, 8,  // Offset(1378),
  Offset(1379),   L2ME, 8,  // Offset(1379),
  Offset(1380),   L2MF, 8,  // Offset(1380),  Camera Module Name ASCII character 15
  Offset(1381),   L2DI, 8,  // Offset(1381),  Number of I2C devices
  Offset(1382),   L2BS, 8,  // Offset(1382),  I2C Serial Bus number
  Offset(1383),   L2A0, 16, // Offset(1383),  Address of I2C Device0 on Link2
  Offset(1385),   L2A1, 16, // Offset(1385),  Address of I2C Device1 on Link2
  Offset(1387),   L2A2, 16, // Offset(1387),  Address of I2C Device2 on Link2
  Offset(1389),   L2A3, 16, // Offset(1389),  Address of I2C Device3 on Link2
  Offset(1391),   L2A4, 16, // Offset(1391),  Address of I2C Device4 on Link2
  Offset(1393),   L2A5, 16, // Offset(1393),  Address of I2C Device5 on Link2
  Offset(1395),   L2A6, 16, // Offset(1395),  Address of I2C Device6 on Link2
  Offset(1397),   L2A7, 16, // Offset(1397),  Address of I2C Device7 on Link2
  Offset(1399),   L2A8, 16, // Offset(1399),  Address of I2C Device8 on Link2
  Offset(1401),   L2A9, 16, // Offset(1401),  Address of I2C Device9 on Link2
  Offset(1403),   L2AA, 16, // Offset(1403),  Address of I2C Device10 on Link2
  Offset(1405),   L2AB, 16, // Offset(1405),  Address of I2C Device11 on Link2
  Offset(1407),   L2D0, 8,  // Offset(1407),  Type of I2C Device0 on Link2
  Offset(1408),   L2D1, 8,  // Offset(1408),  Type of I2C Device1 on Link2
  Offset(1409),   L2D2, 8,  // Offset(1409),  Type of I2C Device2 on Link2
  Offset(1410),   L2D3, 8,  // Offset(1410),  Type of I2C Device3 on Link2
  Offset(1411),   L2D4, 8,  // Offset(1411),  Type of I2C Device4 on Link2
  Offset(1412),   L2D5, 8,  // Offset(1412),  Type of I2C Device5 on Link2
  Offset(1413),   L2D6, 8,  // Offset(1413),  Type of I2C Device6 on Link2
  Offset(1414),   L2D7, 8,  // Offset(1414),  Type of I2C Device7 on Link2
  Offset(1415),   L2D8, 8,  // Offset(1415),  Type of I2C Device8 on Link2
  Offset(1416),   L2D9, 8,  // Offset(1416),  Type of I2C Device9 on Link2
  Offset(1417),   L2DA, 8,  // Offset(1417),  Type of I2C Device10 on Link2
  Offset(1418),   L2DB, 8,  // Offset(1418),  Type of I2C Device11 on Link2
  Offset(1419),   L2DV, 8,  // Offset(1419),  Version of SSDB structure
  Offset(1420),   L2CV, 8,  // Offset(1420),  Version of CRD
  Offset(1421),   L2LU, 8,  // Offset(1421),  CSI2 Link used
  Offset(1422),   L2NL, 8,  // Offset(1422),  MIPI-CSI2 Data Lane
  Offset(1423),   L2EE, 8,  // Offset(1423),  EEPROM Type
  Offset(1424),   L2VC, 8,  // Offset(1424),  VCM Type
  Offset(1425),   L2FS, 8,  // Offset(1425),  Flash Support
  Offset(1426),   L2DG, 8,  // Offset(1426),  Degree
  Offset(1427),   L2C0, 8,  // Offset(1427),  Platform @deprecated since revision 9.
  Offset(1428),   L2C1, 8,  // Offset(1428),  PlatformSub @deprecated since revision 9.
  Offset(1429),   L2C2, 8,  // Offset(1429),  pLED @deprecated since revision 9.
  Offset(1430),   L2C3, 8,  // Offset(1430),  mipiDefine @deprecated since revision 9.
  Offset(1431),   L2CK, 32, // Offset(1431),  MCLK
  Offset(1435),   L2CL, 8,  // Offset(1435),  Control Logic
  // Sky Cam Link3 options
  Offset(1436),   L3SM, 8,  // Offset(1436),  Sensor Model
  Offset(1437),   L3H0, 8,  // Offset(1437),  User defined HID ASCII character 0
  Offset(1438),   L3H1, 8,  // Offset(1438),
  Offset(1439),   L3H2, 8,  // Offset(1439),
  Offset(1440),   L3H3, 8,  // Offset(1440),
  Offset(1441),   L3H4, 8,  // Offset(1441),
  Offset(1442),   L3H5, 8,  // Offset(1442),
  Offset(1443),   L3H6, 8,  // Offset(1443),
  Offset(1444),   L3H7, 8,  // Offset(1444),
  Offset(1445),   L3H8, 8,  // Offset(1445),  User defined HID ASCII character 8
  Offset(1446),   L3PL, 8,  // Offset(1446),  Camera Position
  Offset(1447),   L3M0, 8,  // Offset(1447),  Camera Module Name ASCII character 0
  Offset(1448),   L3M1, 8,  // Offset(1448),
  Offset(1449),   L3M2, 8,  // Offset(1449),
  Offset(1450),   L3M3, 8,  // Offset(1450),
  Offset(1451),   L3M4, 8,  // Offset(1451),
  Offset(1452),   L3M5, 8,  // Offset(1452),
  Offset(1453),   L3M6, 8,  // Offset(1453),
  Offset(1454),   L3M7, 8,  // Offset(1454),
  Offset(1455),   L3M8, 8,  // Offset(1455),
  Offset(1456),   L3M9, 8,  // Offset(1456),
  Offset(1457),   L3MA, 8,  // Offset(1457),
  Offset(1458),   L3MB, 8,  // Offset(1458),
  Offset(1459),   L3MC, 8,  // Offset(1459),
  Offset(1460),   L3MD, 8,  // Offset(1460),
  Offset(1461),   L3ME, 8,  // Offset(1461),
  Offset(1462),   L3MF, 8,  // Offset(1462),  Camera Module Name ASCII character 15
  Offset(1463),   L3DI, 8,  // Offset(1463),  Number of I2C devices
  Offset(1464),   L3BS, 8,  // Offset(1464),  I2C Serial Bus number
  Offset(1465),   L3A0, 16, // Offset(1465),  Address of I2C Device0 on Link3
  Offset(1467),   L3A1, 16, // Offset(1467),  Address of I2C Device1 on Link3
  Offset(1469),   L3A2, 16, // Offset(1469),  Address of I2C Device2 on Link3
  Offset(1471),   L3A3, 16, // Offset(1471),  Address of I2C Device3 on Link3
  Offset(1473),   L3A4, 16, // Offset(1473),  Address of I2C Device4 on Link3
  Offset(1475),   L3A5, 16, // Offset(1475),  Address of I2C Device5 on Link3
  Offset(1477),   L3A6, 16, // Offset(1477),  Address of I2C Device6 on Link3
  Offset(1479),   L3A7, 16, // Offset(1479),  Address of I2C Device7 on Link3
  Offset(1481),   L3A8, 16, // Offset(1481),  Address of I2C Device8 on Link3
  Offset(1483),   L3A9, 16, // Offset(1483),  Address of I2C Device9 on Link3
  Offset(1485),   L3AA, 16, // Offset(1485),  Address of I2C Device10 on Link3
  Offset(1487),   L3AB, 16, // Offset(1487),  Address of I2C Device11 on Link3
  Offset(1489),   L3D0, 8,  // Offset(1489),  Type of I2C Device0 on Link3
  Offset(1490),   L3D1, 8,  // Offset(1490),  Type of I2C Device1 on Link3
  Offset(1491),   L3D2, 8,  // Offset(1491),  Type of I2C Device2 on Link3
  Offset(1492),   L3D3, 8,  // Offset(1492),  Type of I2C Device3 on Link3
  Offset(1493),   L3D4, 8,  // Offset(1493),  Type of I2C Device4 on Link3
  Offset(1494),   L3D5, 8,  // Offset(1494),  Type of I2C Device5 on Link3
  Offset(1495),   L3D6, 8,  // Offset(1495),  Type of I2C Device6 on Link3
  Offset(1496),   L3D7, 8,  // Offset(1496),  Type of I2C Device7 on Link3
  Offset(1497),   L3D8, 8,  // Offset(1497),  Type of I2C Device8 on Link3
  Offset(1498),   L3D9, 8,  // Offset(1498),  Type of I2C Device9 on Link3
  Offset(1499),   L3DA, 8,  // Offset(1499),  Type of I2C Device10 on Link3
  Offset(1500),   L3DB, 8,  // Offset(1500),  Type of I2C Device11 on Link3
  Offset(1501),   L3DV, 8,  // Offset(1501),  Version of SSDB structure
  Offset(1502),   L3CV, 8,  // Offset(1502),  Version of CRD
  Offset(1503),   L3LU, 8,  // Offset(1503),  CSI2 Link used
  Offset(1504),   L3NL, 8,  // Offset(1504),  MIPI-CSI2 Data Lane
  Offset(1505),   L3EE, 8,  // Offset(1505),  EEPROM Type
  Offset(1506),   L3VC, 8,  // Offset(1506),  VCM Type
  Offset(1507),   L3FS, 8,  // Offset(1507),  Flash Support
  Offset(1508),   L3DG, 8,  // Offset(1508),  Degree
  Offset(1509),   L3C0, 8,  // Offset(1509),  Platform @deprecated since revision 9.
  Offset(1510),   L3C1, 8,  // Offset(1510),  PlatformSub @deprecated since revision 9.
  Offset(1511),   L3C2, 8,  // Offset(1511),  pLED @deprecated since revision 9.
  Offset(1512),   L3C3, 8,  // Offset(1512),  mipiDefine @deprecated since revision 9.
  Offset(1513),   L3CK, 32, // Offset(1513),  MCLK
  Offset(1517),   L3CL, 8,  // Offset(1517),  Control Logic
  Offset(1518),   ECR1, 8,  // Offset(1518),
  Offset(1550),             // Offset(1519) : Offset(1549), Reserved bytes
  Offset(1550),   I2SC, 8,  // Offset(1550),  HD Audio I2S Codec Selection
  Offset(1551),   ODV0, 8,  // Offset(1551),  DPTF Oem Design Variables
  Offset(1552),   ODV1, 8,  // Offset(1552),  DPTF Oem Design Variables
  Offset(1553),   ODV2, 8,  // Offset(1553),  DPTF Oem Design Variables
  Offset(1554),   ODV3, 8,  // Offset(1554),  DPTF Oem Design Variables
  Offset(1555),   ODV4, 8,  // Offset(1555),  DPTF Oem Design Variables
  Offset(1556),   ODV5, 8,  // Offset(1556),  DPTF Oem Design Variables
  Offset(1557),   UBCB, 32, // Offset(1557),  USB Type C Opregion base address
  Offset(1561),   EMOD, 8,  // Offset(1561),  Enable / Disable Modern Standby Mode
  Offset(1562),   WIFC, 8,  // Offset(1562),  WirelessCharging
  Offset(1570),             // Offset(1563) : Offset(1569), Reserved bytes
  Offset(1570),   TPLS, 8,  // Offset(1570),  custom touchpanel's bus speed
  Offset(1571),   TPDB, 8,  // Offset(1571),  custom touchpad's bus address
  Offset(1572),   TPDH, 16, // Offset(1572),  custom touchpad's HID address
  Offset(1574),   TPDS, 8,  // Offset(1574),  custom touchpad's bus speed
  Offset(1575),   ADPM, 32, // Offset(1575),  HD-Audio DSP Post-Processing Module Mask
  Offset(1579),   AG1L, 64, // Offset(1579),  HDA PP module custom GUID 1 - first 64bit  [0-63]
  Offset(1587),   AG1H, 64, // Offset(1587),  HDA PP module custom GUID 1 - second 64bit [64-127]
  Offset(1595),   AG2L, 64, // Offset(1595),  HDA PP module custom GUID 2 - first 64bit  [0-63]
  Offset(1603),   AG2H, 64, // Offset(1603),  HDA PP module custom GUID 2 - second 64bit [64-127]
  Offset(1611),   AG3L, 64, // Offset(1611),  HDA PP module custom GUID 3 - first 64bit  [0-63]
  Offset(1619),   AG3H, 64, // Offset(1619),  HDA PP module custom GUID 3 - second 64bit [64-127]
  Offset(1627),   HEFE, 8,  // Offset(1627),  HID Event Filter Driver enable
  Offset(1628),   XDCE, 8,  // Offset(1628),  XDCI Enable/Disable status
  Offset(1629),   STXE, 8,  // Offset(1629),  WrdsWiFiSarEnable
  Offset(1630),   STX0, 8,  // Offset(1630),  WrdsWiFiSarTxPowerLimit1
  Offset(1631),   STX1, 8,  // Offset(1631),  WrdsWiFiSarTxPowerLimit2
  Offset(1632),   STX2, 8,  // Offset(1632),  WrdsWiFiSarTxPowerLimit3
  Offset(1633),   STX3, 8,  // Offset(1633),  WrdsWiFiSarTxPowerLimit4
  Offset(1634),   STX4, 8,  // Offset(1634),  WrdsWiFiSarTxPowerLimit5
  Offset(1635),   STX5, 8,  // Offset(1635),  WrdsWiFiSarTxPowerLimit6
  Offset(1636),   STX6, 8,  // Offset(1636),  WrdsWiFiSarTxPowerLimit7
  Offset(1637),   STX7, 8,  // Offset(1637),  WrdsWiFiSarTxPowerLimit8
  Offset(1638),   STX8, 8,  // Offset(1638),  WrdsWiFiSarTxPowerLimit9
  Offset(1639),   STX9, 8,  // Offset(1639),  WrdsWiFiSarTxPowerLimit10
  Offset(1640),   RTVM, 8,  // Offset(1640),  RunTime VM Control. 0:Disabled, 1:Premium PMIC, 2:VR controlled by GPIO, 3:DVRM Without IO EXP, 4:DVRM With IO EXP
  //
  //Feature Specific Data Bits
  //
  Offset(1641),   USTC, 8,  // Offset(1641),  USB Type C Supported
  Offset(1642),   BATP, 8,  // Offset(1642),  Battery Present - Bit0: Real Battery is supported on this platform. Bit1: Virtual Battery is supported on this platform.
  Offset(1643),   TSDB, 8,  // Offset(1643),  TS-on-DIMM temperature
  Offset(1644),   DEPC, 8,  // Offset(1644),  Ds End Point Camera
  Offset(1645),   PDFC, 8,  // Offset(1645),  Percep Ds 4 Camera
  Offset(1646),   IVCM, 8,  // Offset(1646),  Disable IV Camera before going into Sx state
  Offset(1647),   HEB1, 32, // Offset(1647),  HebcValue
  Offset(1651),   RBY1, 8,  // Offset(1651),  Real Battery 1 Control
  Offset(1652),   RBY2, 8,  // Offset(1652),  Real Battery 2 Control
  Offset(1653),   SCSS, 8,  // Offset(1653),  Sky Camera Sensor
  Offset(1654),   HAID, 8,  // Offset(1654),  HD Audio I2S
  Offset(1655),   NCTC, 8,  // Offset(1655),  NCT6776F COM
  Offset(1656),   NCTI, 8,  // Offset(1656),  NCT6776F SIO
  Offset(1657),   NCTH, 8,  // Offset(1657),  NCT6776F HWMON
  Offset(1658),   HSIO, 8,  // Offset(1658),  H8S2113 SIO
  Offset(1659),   TPPT, 8,  // Offset(1659),  Power Participant
  Offset(1660),   SHAP, 8,  // Offset(1660),  Sensor Hub Application
  Offset(1661),   EIAP, 8,  // Offset(1661),  EInk Application
  Offset(1662),   ZPOD, 8,  // Offset(1662),  ZPODD
  Offset(1663),   SRSP, 32, // Offset(1663),  SMC Runtime Sci Pin
  Offset(1667),   CEDS, 8,  // Offset(1667),  Convertable Dock Support
  Offset(1668),   EHK3, 8,  // Offset(1668),  Ec Hotkey F3 Support
  Offset(1669),   EHK4, 8,  // Offset(1669),  Ec Hotkey F4 Support
  Offset(1670),   EHK5, 8,  // Offset(1670),  Ec Hotkey F5 Support
  Offset(1671),   EHK6, 8,  // Offset(1671),  Ec Hotkey F6 Support
  Offset(1672),   EHK7, 8,  // Offset(1672),  Ec Hotkey F7 Support
  Offset(1673),   EHK8, 8,  // Offset(1673),  Ec Hotkey F8 Support
  Offset(1674),   VBVP, 8,  // Offset(1674),  Virtual Button Volume Up Support
  Offset(1675),   VBVD, 8,  // Offset(1675),  Virtual Button Volume Down Support
  Offset(1676),   VBHB, 8,  // Offset(1676),  Virtual Button Home Button Support
  Offset(1677),   VBRL, 8,  // Offset(1677),  Virtual Button Rotation Lock Support
  Offset(1678),   SMSS, 8,  // Offset(1678),  Slate Mode Switch Support
  Offset(1679),   VBST, 8,  // Offset(1679),  Virtual Button Support /< @deprecated
  Offset(1680),   ADAS, 8,  // Offset(1680),  Ac Dc Auto Switch Support
  Offset(1681),   PPBG, 32, // Offset(1681),  Pm Power Button Gpio Pin
  Offset(1685),   AEAB, 8,  // Offset(1685),  Acpi Enable All Button Support
  Offset(1686),   AHDB, 8,  // Offset(1686),  Acpi Hid Driver Button Support
  Offset(1687),   PBSD, 8,  // Offset(1687),  Power Button SCI disable support
  Offset(1688),   DPLL, 8,  // Offset(1688),  DPTF Display Depth Lower Limit in percent
  Offset(1689),   DPHL, 8,  // Offset(1689),  DPTF Display Depth Upper Limit in percent
  Offset(1690),   PWIG, 8,  // Offset(1690),  PEP F1 constraints for WiGig device
  Offset(1691),   MESE, 8,  // Offset(1691),  Enable eSPI support
  //
  // DPTF IVCAM Participant
  //
  Offset(1692),   ICAT, 8,  // Offset(1692),  ActiveThermalTripPointIvCam
  Offset(1693),   ICPV, 8,  // Offset(1693),  PassiveThermalTripPointIvCam
  Offset(1694),   ICCR, 8,  // Offset(1694),  CriticalThermalTripPointIvCam
  Offset(1695),   ICC3, 8,  // Offset(1695),  CriticalThermalTripPointIvCamS3
  Offset(1696),   ICHT, 8,  // Offset(1696),  HotThermalTripPointIvCam
  //
  // XTU SMI base address
  //
  Offset(1697),   XSMI, 32, // Offset(1697),  XTU SMI structure Base Address
  Offset(1701),   PAPE, 32, // Offset(1701),  Pch Audio Power Enable
  Offset(1705),   PSTW, 32, // Offset(1705),  Pcie Slot 2 Wake
  Offset(1709),   MWLR, 32, // Offset(1709),  M.2 Wireless Lan Reset
  Offset(1713),   UP8P, 32, // Offset(1713),  Usb Port 8 Power
  Offset(1717),   MS2R, 32, // Offset(1717),  M.2 Pcie Ssd2 Reset
  Offset(1721),   MS2P, 32, // Offset(1721),  M.2 Pcie Ssd2 Power
  //
  // UCMC setup option, GPIO Pad
  //
  Offset(1725),   UCSI, 8,  // Offset(1725),  Option to Enable/Disable UCSI/UCMC device
  Offset(1726),   UCG1, 32, // Offset(1726),  Gpio for UCMC Port 1 Interrupt
  Offset(1730),   UCG2, 32, // Offset(1730),  Gpio for UCMC Port 2 Interrupt
  Offset(1734),   WGUR, 32, // Offset(1734),  Wwan Gnss Uart Reset GPIO
  Offset(1738),   WRFE, 8,  // Offset(1738),  WigigRfe
  Offset(1739),   WRC1, 8,  // Offset(1739),  WiGigRfeCh1
  Offset(1740),   WRC2, 8,  // Offset(1740),  WiGigRfeCh2
  Offset(1741),   WRC3, 8,  // Offset(1741),  WiGigRfeCh3
  Offset(1742),   WRC4, 8,  // Offset(1742),  WiGigRfeCh4
  Offset(1743),   AWVI, 32, // Offset(1743),  AwvClassIndex
  Offset(1760),             // Offset(1747) : Offset(1759), Reserved bytes
  Offset(1760),   WTSP, 8,  // Offset(1760),  ThermalSamplingPeriodWifi
  Offset(1761),   WGWS, 8,  // Offset(1761),  Wake on S3-S4 WiGig Docking Support
  Offset(1762),   PIDE, 8,  // Offset(1762),  EnablePIDPolicy
  Offset(1763),   C0VE, 8,  // Offset(1763),  Version of CLDB structure
  Offset(1764),   C0W0, 8,  // Offset(1764),  WLED1 Flash Max Current
  Offset(1765),   C0W1, 8,  // Offset(1765),  WLED1 Torch Max Current
  Offset(1766),   C0W2, 8,  // Offset(1766),  WLED2 Flash Max Current
  Offset(1767),   C0W3, 8,  // Offset(1767),  WLED2 Torch Max Current
  Offset(1768),   C0W4, 8,  // Offset(1768),  WLED1 Type
  Offset(1769),   C0W5, 8,  // Offset(1769),  WLED2 Type
  Offset(1770),   C1VE, 8,  // Offset(1770),  Version of CLDB structure
  Offset(1771),   C1W0, 8,  // Offset(1771),  WLED1 Flash Max Current
  Offset(1772),   C1W1, 8,  // Offset(1772),  WLED1 Torch Max Current
  Offset(1773),   C1W2, 8,  // Offset(1773),  WLED2 Flash Max Current
  Offset(1774),   C1W3, 8,  // Offset(1774),  WLED2 Torch Max Current
  Offset(1775),   C1W4, 8,  // Offset(1775),  WLED1 Type
  Offset(1776),   C1W5, 8,  // Offset(1776),  WLED2 Type
  Offset(1777),   C2VE, 8,  // Offset(1777),  Version of CLDB structure
  Offset(1778),   C2W0, 8,  // Offset(1778),  WLED1 Flash Max Current
  Offset(1779),   C2W1, 8,  // Offset(1779),  WLED1 Torch Max Current
  Offset(1780),   C2W2, 8,  // Offset(1780),  WLED2 Flash Max Current
  Offset(1781),   C2W3, 8,  // Offset(1781),  WLED2 Torch Max Current
  Offset(1782),   C2W4, 8,  // Offset(1782),  WLED1 Type
  Offset(1783),   C2W5, 8,  // Offset(1783),  WLED2 Type
  Offset(1784),   C3VE, 8,  // Offset(1784),  Version of CLDB structure
  Offset(1785),   C3W0, 8,  // Offset(1785),  WLED1 Flash Max Current
  Offset(1786),   C3W1, 8,  // Offset(1786),  WLED1 Torch Max Current
  Offset(1787),   C3W2, 8,  // Offset(1787),  WLED2 Flash Max Current
  Offset(1788),   C3W3, 8,  // Offset(1788),  WLED2 Torch Max Current
  Offset(1789),   C3W4, 8,  // Offset(1789),  WLED1 Type
  Offset(1790),   C3W5, 8,  // Offset(1790),  WLED2 Type
  Offset(1791),   L0LE, 8,  // Offset(1791),  Privacy LED
  Offset(1792),   L0PP, 8,  // Offset(1792),  PMIC Position
  Offset(1793),   L0VR, 8,  // Offset(1793),  Voltage Rail
  Offset(1794),   L1LE, 8,  // Offset(1794),  Privacy LED
  Offset(1795),   L1PP, 8,  // Offset(1795),  PMIC Position
  Offset(1796),   L1VR, 8,  // Offset(1796),  Voltage Rail
  Offset(1797),   L2LE, 8,  // Offset(1797),  Privacy LED
  Offset(1798),   L2PP, 8,  // Offset(1798),  PMIC Position
  Offset(1799),   L2VR, 8,  // Offset(1799),  Voltage Rail
  Offset(1800),   L3LE, 8,  // Offset(1800),  Privacy LED
  Offset(1801),   L3PP, 8,  // Offset(1801),  PMIC Position
  Offset(1802),   L3VR, 8,  // Offset(1802),  Voltage Rail
  Offset(1803),   WLRP, 8,  // Offset(1803),  WLAN PCIe root port value  @deprecated since revision 18
  Offset(1804),   SSRP, 8,  // Offset(1804),  SSD PCIe root port value    @deprecated since revision 18
  Offset(1805),   WIPR, 8,  // Offset(1805),  WIGIG present or not     @deprecated since revision 18
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  Offset(1806),   TBS1, 8,  // Offset(1806),  Thunderbolt(TM) Root port selector
  Offset(1807),   TBMP, 8,  // Offset(1807),  Thunderbolt(TM) Multiport support
  Offset(1808),   FPA1, 8,  // Offset(1808),  TbtFrcPwrGpioAccessType
  Offset(1809),   FPE1, 8,  // Offset(1809),  TbtFrcPwrExpander
  Offset(1810),   FPG1, 32, // Offset(1810),  TbtFrcPwrGpioNo
  Offset(1814),   FP1L, 8,  // Offset(1814),  TbtFrcPwrGpioLevel
  Offset(1815),   CPD1, 16, // Offset(1815),  TbtCioPlugEvtAcpiGpeStsAddress
  Offset(1817),   CPB1, 8,  // Offset(1817),  TbtCioPlugEvtAcpiGpeStsBit
  Offset(1818),   CPG1, 32, // Offset(1818),  TbtCioPlugEventGpioNo
#else
  Offset(1822),             // Offset(1822) : Offset(1821), Reserved bytes
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
  Offset(1822),   UTCM, 8,  // Offset(1822),  USB type C Multi Port Support
  Offset(1823),   USME, 8,  // Offset(1823),  Tbt USB Type C Split mode
  Offset(1824),   UPT1, 8,  // Offset(1824),  Tbt USB Type C Split mode
  Offset(1825),   UPT2, 8,  // Offset(1825),  Tbt USB Type C Split mode
  Offset(1826),   TWIN, 8,  // Offset(1826),  TbtWin10Support
  Offset(1827),   TRWA, 8,  // Offset(1827),  Titan Ridge Osup command
  Offset(1828),   PEWE, 8,  // Offset(1828),  Pcie Wwan Enable @deprecated since revision 23
  Offset(1870),             // Offset(1829) : Offset(1869), Reserved bytes
  Offset(1870),   ELPM, 32, // Offset(1870),  EcLowPowerModeGpioPin
  Offset(1874),   ELPS, 32, // Offset(1874),  EcSmiGpioPin
  Offset(1880),             // Offset(1878) : Offset(1879), Reserved bytes
  Offset(1880),   UCRT, 8,  // Offset(1880),  UcsiRetryEC
  Offset(1881),   TBOD, 16, // Offset(1881),  Rtd3TbtOffDelay TBT RTD3 Off Delay
  Offset(1883),   TSXW, 8,  // Offset(1883),  TbtSxWakeSwitchLogicEnable Set True if TBT_WAKE_N will be routed to PCH WakeB at Sx entry point. HW logic is required.
  Offset(1884),   VRGP, 32, // Offset(1884),  RunTime VM GPIO pin for VR
  Offset(1888),   PVSC, 8,  // Offset(1888),  DPTF GENx/SENx selection in VSCT table for Virx participants
  Offset(1889),   RTBT, 8,  // Offset(1889),  Enable Rtd3 support for TBT. Corresponding to Rtd3Tbt in Setup.
  Offset(1890),   RTBC, 8,  // Offset(1890),  Enable TBT RTD3 CLKREQ mask.
  Offset(1891),   TBCD, 16, // Offset(1891),  TBT RTD3 CLKREQ mask delay.
  Offset(1893),   TBTE, 8,  // Offset(1893),  TBT Enable/Disable
  Offset(1894),   RWAN, 8,  // Offset(1894),  WWAN D3/L3 Enable/Disable
  Offset(1895),   WDCT, 16, // Offset(1895),  WWAN Device Control register save area
  Offset(1897),   WLCT, 16, // Offset(1897),  WWAN Link Control register save area
  Offset(1899),   WDC2, 16, // Offset(1899),  WWAN Device Control register save area
  Offset(1901),   WMXS, 16, // Offset(1901),  WWAN LTR Max Snoop Latency save area
  Offset(1903),   WMNS, 16, // Offset(1903),  WWAN LTR Max No Snoop Latency save area
  Offset(1905),   SSPC, 8,  // Offset(1905),  SensorSamplingPeriodDGFC
  Offset(1906),   SSPM, 8,  // Offset(1906),  SensorSamplingPeriodDGHM
  Offset(1907),   DGCE, 8,  // Offset(1907),  EnableDGFCParticipant
  Offset(1908),   DCAT, 8,  // Offset(1908),  ActiveThermalTripPointDGFC
  Offset(1909),   DCPT, 8,  // Offset(1909),  PassiveThermalTripPointDGFC
  Offset(1910),   DCCT, 8,  // Offset(1910),  CriticalThermalTripPointDGFC
  Offset(1911),   DCHT, 8,  // Offset(1911),  HotThermalTripPointDGFC
  Offset(1912),   DCS3, 8,  // Offset(1912),  CriticalThermalTripPointDGFCS3
  Offset(1913),   DCSZ, 32, // Offset(1913),  PPCC Step Size DGFC
  Offset(1917),   DCL0, 16, // Offset(1917),  Minimum Power Limit 0 for DGFC DPTF use via PPCC Object
  Offset(1919),   DGME, 8,  // Offset(1919),  EnableDGHMParticipant
  Offset(1920),   DMAT, 8,  // Offset(1920),  ActiveThermalTripPointDGHM
  Offset(1921),   DMPT, 8,  // Offset(1921),  PassiveThermalTripPointDGHM
  Offset(1922),   DMCT, 8,  // Offset(1922),  CriticalThermalTripPointDGHM
  Offset(1923),   DMHT, 8,  // Offset(1923),  HotThermalTripPointDGHM
  Offset(1924),   DMS3, 8,  // Offset(1924),  CriticalThermalTripPointDGHMS3
  Offset(1925),   MCPE, 8,  // Offset(1925),  EnableMCPParticipant
  Offset(1926),   MCSZ, 32, // Offset(1926),  PPCC Step Size MCP
  Offset(1930),   MCL0, 32, // Offset(1930),  Minimum Power Limit 0 for MCP DPTF use via PPCC Object
  Offset(1934),   PSPE, 8,  // Offset(1934),  EnablePowerSharePolicy
  Offset(1935),   PBEN, 8,  // Offset(1935),  PcdEnablePbok
  Offset(1936),   WRTO, 8,  // Offset(1936),  WWAN RTD3 options
  Offset(1937),   SD11, 8,  // Offset(1937),  WgdsWiFiSarDeltaGroup1PowerMax1
  Offset(1938),   SD12, 8,  // Offset(1938),  WgdsWiFiSarDeltaGroup1PowerChainA1
  Offset(1939),   SD13, 8,  // Offset(1939),  WgdsWiFiSarDeltaGroup1PowerChainB1
  Offset(1940),   SD14, 8,  // Offset(1940),  WgdsWiFiSarDeltaGroup1PowerMax2
  Offset(1941),   SD15, 8,  // Offset(1941),  WgdsWiFiSarDeltaGroup1PowerChainA2
  Offset(1942),   SD16, 8,  // Offset(1942),  WgdsWiFiSarDeltaGroup1PowerChainB2
  Offset(1943),   SD21, 8,  // Offset(1943),  WgdsWiFiSarDeltaGroup2PowerMax1
  Offset(1944),   SD22, 8,  // Offset(1944),  WgdsWiFiSarDeltaGroup2PowerChainA1
  Offset(1945),   SD23, 8,  // Offset(1945),  WgdsWiFiSarDeltaGroup2PowerChainB1
  Offset(1946),   SD24, 8,  // Offset(1946),  WgdsWiFiSarDeltaGroup2PowerMax2
  Offset(1947),   SD25, 8,  // Offset(1947),  WgdsWiFiSarDeltaGroup2PowerChainA2
  Offset(1948),   SD26, 8,  // Offset(1948),  WgdsWiFiSarDeltaGroup2PowerChainB2
  Offset(1949),   SD31, 8,  // Offset(1949),  WgdsWiFiSarDeltaGroup3PowerMax1
  Offset(1950),   SD32, 8,  // Offset(1950),  WgdsWiFiSarDeltaGroup3PowerChainA1
  Offset(1951),   SD33, 8,  // Offset(1951),  WgdsWiFiSarDeltaGroup3PowerChainB1
  Offset(1952),   SD34, 8,  // Offset(1952),  WgdsWiFiSarDeltaGroup3PowerMax2
  Offset(1953),   SD35, 8,  // Offset(1953),  WgdsWiFiSarDeltaGroup3PowerChainA2
  Offset(1954),   SD36, 8,  // Offset(1954),  WgdsWiFiSarDeltaGroup3PowerChainB2
  Offset(1955),   BTSE, 8,  // Offset(1955),  BluetoothSar
  Offset(1956),   BTBR, 8,  // Offset(1956),  BluetoothSarBr
  Offset(1957),   BED2, 8,  // Offset(1957),  BluetoothSarEdr2
  Offset(1958),   BED3, 8,  // Offset(1958),  BluetoothSarEdr3
  Offset(1959),   BTLE, 8,  // Offset(1959),  BluetoothSarLe
  Offset(1960),   BTL2, 8,  // Offset(1960),  BluetoothSarLe2Mhz
  Offset(1961),   BTLL, 8,  // Offset(1961),  BluetoothSarLeLr
  // Power Meter Support
  Offset(1962),   POME, 8,  // Offset(1962),  PowerMeterEnable
  Offset(1963),   P193, 8,  // Offset(1963),  PAC193xPowerMeterSupport
  Offset(1964),   PMTC, 8,  // Offset(1964),  PowerMeterI2cControllerNumber
  // Time Alaram Device Support
  Offset(1965),   STAS, 8,  // Offset(1965),  Select source for System time and alarm
  Offset(1966),   LSGN, 8,  // Offset(1966),  S3 wake capability LID Switch GPE0 Number
  Offset(1967),   PEP1, 32, // Offset(1967),  PEP Constraints 01
  // Bit[0]  - En/Dis SkyCamSensors
  Offset(1971),   SRLD, 8,  // Offset(1971),  Serial Port ACPI debug
// AMI_OVERRIDE_START - EIP251482
  Offset(1972),   AES3, 8,  // Offset(1972),  Entry S3.
// AMI_OVERRIDE_END - EIP251482    
  }
