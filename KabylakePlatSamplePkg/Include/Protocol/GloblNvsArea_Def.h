

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

  
#ifndef _GLOBL_NVS_AREA__DEF_H_
#define _GLOBL_NVS_AREA__DEF_H_

#pragma pack (push,1)
typedef struct {
  //
  // Miscellaneous Dynamic Registers:
  //
  UINT16   OperatingSystem;                         ///< Offset 0       Operating System
  UINT8    SmiFunction;                             ///< Offset 2       SMI Function Call (ASL to SMI via I/O Trap)
  UINT8    SmiParameter0;                           ///< Offset 3       SMIF - Parameter 0
  UINT8    SmiParameter1;                           ///< Offset 4       SMIF - Parameter 1
  UINT8    SciFunction;                             ///< Offset 5       SCI Function Call (SMI to ASL via _L00)
  UINT8    SciParameter0;                           ///< Offset 6       SCIF - Parameter 0
  UINT8    SciParameter1;                           ///< Offset 7       SCIF - Parameter 1
  UINT8    GlobalLock;                              ///< Offset 8       Global Lock Function Call (EC Communication)
  UINT8    LockParameter0;                          ///< Offset 9       LCKF - Parameter 0
  UINT8    LockParameter1;                          ///< Offset 10      LCKF - Parameter 1
  UINT32   Port80DebugValue;                        ///< Offset 11      Port 80 Debug Port Value @deprecated since revision 24
  UINT8    PowerState;                              ///< Offset 15      Power State (AC Mode = 1)
  UINT8    DebugState;                              ///< Offset 16      Debug State
  //
  // Thermal Policy Registers:
  //
  UINT8    EnableThermalKSC;                        ///< Offset 17      Enable Thermal Offset for KSC
  UINT8    Ac1TripPoint;                            ///< Offset 18      Active Trip Point 1
  UINT8    Ac0TripPoint;                            ///< Offset 19      Active Trip Point
  UINT8    PassiveThermalTripPoint;                 ///< Offset 20      Passive Trip Point
  UINT8    PassiveTc1Value;                         ///< Offset 21      Passive Trip Point TC1 Value
  UINT8    PassiveTc2Value;                         ///< Offset 22      Passive Trip Point TC2 Value
  UINT8    PassiveTspValue;                         ///< Offset 23      Passive Trip Point TSP Value
  UINT8    CriticalThermalTripPoint;                ///< Offset 24      Critical Trip Point
  UINT8    EnableDigitalThermalSensor;              ///< Offset 25      Digital Thermal Sensor Enable
  UINT8    BspDigitalThermalSensorTemperature;      ///< Offset 26      Digital Thermal Sensor 1 Reading
  UINT8    ApDigitalThermalSensorTemperature;       ///< Offset 27      Digital Thermal Sensor 2 Reading
  UINT8    DigitalThermalSensorSmiFunction;         ///< Offset 28      DTS SMI Function Call
  //
  // Battery Support Registers:
  //
  UINT8    Reserved0[1];                            ///< Offset 29:29
  UINT8    NumberOfBatteries;                       ///< Offset 30      Battery Number Present
  UINT8    BatteryCapacity0;                        ///< Offset 31      Battery 0 Stored Capacity
  UINT8    BatteryCapacity1;                        ///< Offset 32      Battery 1 Stored Capacity
  UINT8    BatteryCapacity2;                        ///< Offset 33      Battery 2 Stored Capacity
  UINT8    BatteryStatus0;                          ///< Offset 34      Battery 0 Stored Status
  UINT8    BatteryStatus1;                          ///< Offset 35      Battery 1 Stored Status
  UINT8    BatteryStatus2;                          ///< Offset 36      Battery 2 Stored Status
  //
  // Revision Field:
  //
  UINT8    Revision;                                ///< Offset 37      Revison of GlobalNvsArea
  //                 
  // CPU Identification Registers:
  //
  UINT8    ApicEnable;                              ///< Offset 38      APIC Enabled by SBIOS (APIC Enabled = 1)
  UINT8    ThreadCount;                             ///< Offset 39      Number of Enabled Threads
  UINT8    CurentPdcState0;                         ///< Offset 40      PDC Settings, Processor 0
  UINT8    CurentPdcState1;                         ///< Offset 41      PDC Settings, Processor 1
  UINT8    MaximumPpcState;                         ///< Offset 42      Maximum PPC state
  UINT32   PpmFlags;                                ///< Offset 43      PPM Flags (Same as CFGD)
  UINT8    C6C7Latency;                             ///< Offset 47      C6/C7 Entry/Exit latency
  //
  // SIO CMOS Configuration Registers:
  //
  UINT8    DockedSioPresent;                        ///< Offset 48      National SIO Present
  UINT8    DockComA;                                ///< Offset 49      COM A Port
  UINT8    DockComB;                                ///< Offset 50      COM B Port
  UINT8    DockLpt;                                 ///< Offset 51      LPT Port
  UINT8    DockFdc;                                 ///< Offset 52      FDC Port
  UINT8    OnboardCom;                              ///< Offset 53      SMSC Com Port
  UINT8    OnboardComCir;                           ///< Offset 54      SMSC Com CIR Port
  UINT8    SMSC1007;                                ///< Offset 55      SMSC1007 SIO Present
  UINT8    WPCN381U;                                ///< Offset 56      WPCN381U SIO Present
  UINT8    SMSC1000;                                ///< Offset 57      SMSC1000 SIO Present
  //
  // Extended Mobile Access Values
  //
  UINT8    EmaEnable;                               ///< Offset 58      EMA Enable
  UINT16   EmaPointer;                              ///< Offset 59      EMA Pointer
  UINT16   EmaLength;                               ///< Offset 61      EMA Length
  //
  // MEF Registers:
  //
  UINT8    MefEnable;                               ///< Offset 63      MEF Enable
  //
  // PCIe Dock Status:
  //
  UINT8    PcieDockStatus;                          ///< Offset 64      PCIe Dock Status
  //
  // TPM Registers
  //
  UINT8    MorData;                                 ///< Offset 65      Memory Overwrite Request Data
  UINT8    TcgParamter;                             ///< Offset 66      Used for save the Mor and/or physical presence paramter
  UINT32   PPResponse;                              ///< Offset 67      Physical Presence request operation response
  UINT8    PPRequest;                               ///< Offset 71      Physical Presence request operation
  UINT8    LastPPRequest;                           ///< Offset 72      Last Physical Presence request operation
  //
  // SATA Registers:
  //
  UINT8    IdeMode;                                 ///< Offset 73      IDE Mode (Compatible\Enhanced)
  //
  // Board Id
  //
  UINT8    PlatformId;                              ///< Offset 74      Platform id
  UINT8    BoardType;                               ///< Offset 75      Board Type
  //
  // PCIe Hot Plug
  //
  UINT8    PcieOSCControl;                          ///< Offset 76      PCIE OSC Control
  UINT8    NativePCIESupport;                       ///< Offset 77      Native PCIE Setup Value
  //
  // USB Sideband Deferring Support
  //
  UINT8    HostAlertVector1;                        ///< Offset 78      USB Sideband Deferring GPE Vector (HOST_ALERT#1)
  UINT8    HostAlertVector2;                        ///< Offset 79      USB Sideband Deferring GPE Vector (HOST_ALERT#2)
  //
  // Embedded Controller Availability Flag.
  //
  UINT8    EcAvailable;                             ///< Offset 80      Embedded Controller Availability Flag.
  //
  // Global Variables
  //
  UINT8    DisplaySupportFlag;                      ///< Offset 81      _DOS Display Support Flag.
  UINT8    InterruptModeFlag;                       ///< Offset 82      Global IOAPIC/8259 Interrupt Mode Flag.
  UINT8    CoolingTypeFlag;                         ///< Offset 83      Global Cooling Type Flag.
  UINT8    L01Counter;                              ///< Offset 84      Global L01 Counter.
  UINT8    VirtualFan0Status;                       ///< Offset 85      Virtual Fan0 Status.
  UINT8    VirtualFan1Status;                       ///< Offset 86      Virtual Fan1 Status.
  UINT8    VirtualFan2Status;                       ///< Offset 87      Virtual Fan2 Status.
  UINT8    VirtualFan3Status;                       ///< Offset 88      Virtual Fan3 Status.
  UINT8    VirtualFan4Status;                       ///< Offset 89      Virtual Fan4 Status.
  UINT8    VirtualFan5Status;                       ///< Offset 90      Virtual Fan5 Status.
  UINT8    VirtualFan6Status;                       ///< Offset 91      Virtual Fan6 Status.
  UINT8    VirtualFan7Status;                       ///< Offset 92      Virtual Fan7 Status.
  UINT8    VirtualFan8Status;                       ///< Offset 93      Virtual Fan8 Status.
  UINT8    VirtualFan9Status;                       ///< Offset 94      Virtual Fan9 Status.
  //
  // Thermal
  //
  UINT8    ActiveThermalTripPointSA;                ///< Offset 95      Active Trip Point for MCH
  UINT8    PassiveThermalTripPointSA;               ///< Offset 96      Passive Trip Point for MCH
  UINT32   PlatformCpuId;                           ///< Offset 97      CPUID Feature Information [EAX]
  UINT32   TBARB;                                   ///< Offset 101     Reserved for Thermal Base Low Address for BIOS
  UINT32   TBARBH;                                  ///< Offset 105     Reserved for Thermal Base High Address for BIOS
  UINT8    RunTimeInterface;                        ///< Offset 109     Run Time Interface for Intelligent Power Savings
  UINT8    TsOnDimmEnabled;                         ///< Offset 110     TS-on-DIMM is chosen in SETUP and present on the DIMM
  //
  // Board info
  //
  UINT8    PlatformFlavor;                          ///< Offset 111     Platform Flavor
  UINT8    BoardRev;                                ///< Offset 112     Board Rev
  //
  // Package temperature
  //
  UINT8    PackageDTSTemperature;                   ///< Offset 113     Package Temperature
  UINT8    IsPackageTempMSRAvailable;               ///< Offset 114     Package Temperature MSR available
  UINT8    PeciAccessMethod;                        ///< Offset 115     Peci Access Method
  UINT8    Ac0FanSpeed;                             ///< Offset 116     _AC0 Fan Speed
  UINT8    Ac1FanSpeed;                             ///< Offset 117     _AC1 Fan Speed
  UINT8    Ap2DigitalThermalSensorTemperature;      ///< Offset 118     Digital Thermal Sensor 3 Reading
  UINT8    Ap3DigitalThermalSensorTemperature;      ///< Offset 119     Digital Thermal Sensor 4 Reading
  UINT8    LtrEnable[24];                           ///< Offset 120     Latency Tolerance Reporting Enable
                                                    ///< Offset 121     Latency Tolerance Reporting Enable
                                                    ///< Offset 122     Latency Tolerance Reporting Enable
                                                    ///< Offset 123     Latency Tolerance Reporting Enable
                                                    ///< Offset 124     Latency Tolerance Reporting Enable
                                                    ///< Offset 125     Latency Tolerance Reporting Enable
                                                    ///< Offset 126     Latency Tolerance Reporting Enable
                                                    ///< Offset 127     Latency Tolerance Reporting Enable
                                                    ///< Offset 128     Latency Tolerance Reporting Enable
                                                    ///< Offset 129     Latency Tolerance Reporting Enable
                                                    ///< Offset 130     Latency Tolerance Reporting Enable
                                                    ///< Offset 131     Latency Tolerance Reporting Enable
                                                    ///< Offset 132     Latency Tolerance Reporting Enable
                                                    ///< Offset 133     Latency Tolerance Reporting Enable
                                                    ///< Offset 134     Latency Tolerance Reporting Enable
                                                    ///< Offset 135     Latency Tolerance Reporting Enable
                                                    ///< Offset 136     Latency Tolerance Reporting Enable
                                                    ///< Offset 137     Latency Tolerance Reporting Enable
                                                    ///< Offset 138     Latency Tolerance Reporting Enable
                                                    ///< Offset 139     Latency Tolerance Reporting Enable
                                                    ///< Offset 140     Latency Tolerance Reporting Enable
                                                    ///< Offset 141     Latency Tolerance Reporting Enable
                                                    ///< Offset 142     Latency Tolerance Reporting Enable
                                                    ///< Offset 143     Latency Tolerance Reporting Enable
  UINT8    ObffEnable[24];                          ///< Offset 144     Optimized Buffer Flush and Fill
                                                    ///< Offset 145     Optimized Buffer Flush and Fill
                                                    ///< Offset 146     Optimized Buffer Flush and Fill
                                                    ///< Offset 147     Optimized Buffer Flush and Fill
                                                    ///< Offset 148     Optimized Buffer Flush and Fill
                                                    ///< Offset 149     Optimized Buffer Flush and Fill
                                                    ///< Offset 150     Optimized Buffer Flush and Fill
                                                    ///< Offset 151     Optimized Buffer Flush and Fill
                                                    ///< Offset 152     Optimized Buffer Flush and Fill
                                                    ///< Offset 153     Optimized Buffer Flush and Fill
                                                    ///< Offset 154     Optimized Buffer Flush and Fill
                                                    ///< Offset 155     Optimized Buffer Flush and Fill
                                                    ///< Offset 156     Optimized Buffer Flush and Fill
                                                    ///< Offset 157     Optimized Buffer Flush and Fill
                                                    ///< Offset 158     Optimized Buffer Flush and Fill
                                                    ///< Offset 159     Optimized Buffer Flush and Fill
                                                    ///< Offset 160     Optimized Buffer Flush and Fill
                                                    ///< Offset 161     Optimized Buffer Flush and Fill
                                                    ///< Offset 162     Optimized Buffer Flush and Fill
                                                    ///< Offset 163     Optimized Buffer Flush and Fill
                                                    ///< Offset 164     Optimized Buffer Flush and Fill
                                                    ///< Offset 165     Optimized Buffer Flush and Fill
                                                    ///< Offset 166     Optimized Buffer Flush and Fill
                                                    ///< Offset 167     Optimized Buffer Flush and Fill
  //                    
  // XTU 3.0 Specification
  //
  UINT32   XTUBaseAddress;                          ///< Offset 168     XTU Continous structure Base Address
  UINT32   XTUSize;                                 ///< Offset 172     XMP Size
  UINT32   XMPBaseAddress;                          ///< Offset 176     XMP Base Address
  UINT8    DDRReferenceFreq;                        ///< Offset 180     DDR Reference Frequency
  UINT8    Rtd3Support;                             ///< Offset 181     Runtime D3 support.
  UINT8    Rtd3P0dl;                                ///< Offset 182     User selctable Delay for Device D0 transition.
  UINT8    Rtd3P3dl;                                ///< Offset 183     User selctable Delay for Device D3 transition.
  //
  // DPTF Devices and trip points
  //
  UINT8    EnableDptf;                              ///< Offset 184     EnableDptf
  UINT16   EnableDCFG;                              ///< Offset 185     EnableDCFG
  UINT8    EnableSaDevice;                          ///< Offset 187     EnableSaDevice
  UINT8    CriticalThermalTripPointSA;              ///< Offset 188     CriticalThermalTripPointSa
  UINT8    HotThermalTripPointSA;                   ///< Offset 189     HotThermalTripPointSa
  UINT8    ThermalSamplingPeriodSA;                 ///< Offset 190     ThermalSamplingPeriodSA
  //
  // DPTF Policies
  //
  UINT8    EnableCtdpPolicy;                        ///< Offset 191     EnableCtdpPolicy
  UINT8    EnableLpmPolicy;                         ///< Offset 192     EnableLpmPolicy @deprecated since revision 16
  UINT8    CurrentLowPowerMode;                     ///< Offset 193     CurrentLowPowerMode for LPM @deprecated since revision 16
  UINT8    EnableCurrentExecutionUnit;              ///< Offset 194     EnableCurrentExecutionUnit @deprecated since revision 16
  UINT16   TargetGfxFreq;                           ///< Offset 195     TargetGfxFreq @deprecated since revision 16
  //
  // DPPM Devices and trip points
  //
  UINT8    EnableMemoryDevice;                      ///< Offset 197     EnableMemoryDevice @deprecated. Memory Participant is not POR for DPTF
  UINT8    ActiveThermalTripPointTMEM;              ///< Offset 198     Active Trip Point for TMEM @deprecated. Memory Participant is not POR for DPTF
  UINT8    PassiveThermalTripPointTMEM;             ///< Offset 199     Passive Trip Point for TMEM @deprecated. Memory Participant is not POR for DPTF
  UINT8    CriticalThermalTripPointTMEM;            ///< Offset 200     CriticalThermalTripPointTMEM @deprecated. Memory Participant is not POR for DPTF
  UINT8    HotThermalTripPointTMEM;                 ///< Offset 201     HotThermalTripPointTMEM @deprecated. Memory Participant is not POR for DPTF
  UINT8    EnableFan1Device;                        ///< Offset 202     EnableFan1Device
  UINT8    EnableFan2Device;                        ///< Offset 203     EnableFan2Device
  UINT8    EnableAmbientDevice;                     ///< Offset 204     EnableAmbientDevice
  UINT8    ActiveThermalTripPointAmbient;           ///< Offset 205     ActiveThermalTripPointAmbient
  UINT8    PassiveThermalTripPointAmbient;          ///< Offset 206     PassiveThermalTripPointAmbient
  UINT8    CriticalThermalTripPointAmbient;         ///< Offset 207     CriticalThermalTripPointAmbient
  UINT8    HotThermalTripPointAmbient;              ///< Offset 208     HotThermalTripPointAmbient
  UINT8    EnableSkinDevice;                        ///< Offset 209     EnableSkinDevice
  UINT8    ActiveThermalTripPointSkin;              ///< Offset 210     ActiveThermalTripPointSkin
  UINT8    PassiveThermalTripPointSkin;             ///< Offset 211     PassiveThermalTripPointSkin
  UINT8    CriticalThermalTripPointSkin;            ///< Offset 212     CriticalThermalTripPointSkin
  UINT8    HotThermalTripPointSkin;                 ///< Offset 213     HotThermalTripPointSkin
  UINT8    EnableExhaustFanDevice;                  ///< Offset 214     EnableExhaustFanDevice
  UINT8    ActiveThermalTripPointExhaustFan;        ///< Offset 215     ActiveThermalTripPointExhaustFan
  UINT8    PassiveThermalTripPointExhaustFan;       ///< Offset 216     PassiveThermalTripPointExhaustFan
  UINT8    CriticalThermalTripPointExhaustFan;      ///< Offset 217     CriticalThermalTripPointExhaustFan
  UINT8    HotThermalTripPointExhaustFan;           ///< Offset 218     HotThermalTripPointExhaustFan
  UINT8    EnableVRDevice;                          ///< Offset 219     EnableVRDevice
  UINT8    ActiveThermalTripPointVR;                ///< Offset 220     ActiveThermalTripPointVR
  UINT8    PassiveThermalTripPointVR;               ///< Offset 221     PassiveThermalTripPointVR
  UINT8    CriticalThermalTripPointVR;              ///< Offset 222     CriticalThermalTripPointVR
  UINT8    HotThermalTripPointVR;                   ///< Offset 223     HotThermalTripPointVR
  //
  // DPPM Policies
  //
  UINT8    EnableActivePolicy;                      ///< Offset 224     EnableActivePolicy
  UINT8    EnablePassivePolicy;                     ///< Offset 225     EnablePassivePolicy
  UINT8    EnableCriticalPolicy;                    ///< Offset 226     EnableCriticalPolicy
  UINT8    EnableCoolingModePolicy;                 ///< Offset 227     EnableCoolingModePolicy @deprecated since revision 16
  UINT8    TrtRevision;                             ///< Offset 228     TrtRevision
  //
  // CLPO (Current Logical Processor Off lining Setting)
  //
  UINT8    LPOEnable;                               ///< Offset 229     LPOEnable
  UINT8    LPOStartPState;                          ///< Offset 230     LPOStartPState
  UINT8    LPOStepSize;                             ///< Offset 231     LPOStepSize
  UINT8    LPOPowerControlSetting;                  ///< Offset 232     LPOPowerControlSetting
  UINT8    LPOPerformanceControlSetting;            ///< Offset 233     LPOPerformanceControlSetting
  //
  // Miscellaneous DPTF
  //
  UINT32   PpccStepSize;                            ///< Offset 234     PPCC Step Size
  UINT8    EnableDisplayParticipant;                ///< Offset 238     EnableDisplayParticipant
  //
  // BIOS Guard
  //
  UINT64   BiosGuardMemAddress;                     ///< Offset 239     BIOS Guard Memory Address for Tool Interface
  UINT8    BiosGuardMemSize;                        ///< Offset 247     BIOS Guard Memory Size for Tool Interface
  UINT16   BiosGuardIoTrapAddress;                  ///< Offset 248     BIOS Guard IoTrap Address for Tool Interface
  //
  // Never Sleep Technology
  //
  UINT8    IrmtCfg;                                 ///< Offset 250     Irmt Configuration
  //
  // NFC support
  //
  UINT8    NfcDevice;                               ///< Offset 251     NFC module selection
  //
  // Comms Hub
  //
  UINT8    CommsHubEnable;                          ///< Offset 252     Comms Hub Enable/Disable
  UINT8    LowPowerS0Idle;                          ///< Offset 253     Low Power S0 Idle Enable
  //
  // BIOS only version of Config TDP
  //
  UINT8    ConfigTdpBios;                           ///< Offset 254     enable/disable BIOS only version of Config TDP
  UINT8    DockSmi;                                 ///< Offset 255     Dock SMI number
  //
  // LPC SIO configuration
  //
  UINT16   LpcSioPort1;                             ///< Offset 256     SIO config port 1
  UINT16   LpcSioPort2;                             ///< Offset 258     SIO config port 2
  UINT16   LpcSioPmeBar;                            ///< Offset 260     SIO PME Base Address
  //
  // Software Guard
  // SEC0 - SEC8 members have been deprecated.
  //
  UINT32   SgxLeafId0;                              ///< Offset 262     @deprecated SGX Leaf ID 0: EAX This member has been deprecated.
  UINT32   SgxLeafId1;                              ///< Offset 266     @deprecated SGX Leaf ID 1: EAX This member has been deprecated.
  UINT32   SgxLeafId2;                              ///< Offset 270     @deprecated SGX Leaf ID 1: EBX This member has been deprecated.
  UINT32   SgxLeafId3;                              ///< Offset 274     @deprecated SGX Leaf ID 1: ECX This member has been deprecated.
  UINT32   SgxLeafId4;                              ///< Offset 278     @deprecated SGX Leaf ID 1: EDX This member has been deprecated.
  UINT32   SgxLeafId5;                              ///< Offset 282     @deprecated SGX Leaf ID 2: EAX This member has been deprecated.
  UINT32   SgxLeafId6;                              ///< Offset 286     @deprecated SGX Leaf ID 2: EBX This member has been deprecated.
  UINT32   SgxLeafId7;                              ///< Offset 290     @deprecated SGX Leaf ID 2: ECX This member has been deprecated.
  UINT32   SgxLeafId8;                              ///< Offset 294     @deprecated SGX Leaf ID 2: EDX This member has been deprecated.
  UINT8    Reserved1[202];                          ///< Offset 298:499
  UINT8    EnableWifiParticipant;                   ///< Offset 500     EnableWifiParticipant
  UINT8    ActiveThermalTripPointWifi;              ///< Offset 501     ActiveThermalTripPointWifi
  UINT8    PassiveThermalTripPointWifi;             ///< Offset 502     PassiveThermalTripPointWifi
  UINT8    CriticalThermalTripPointWifi;            ///< Offset 503     CriticalThermalTripPointWifi
  UINT8    HotThermalTripPointWifi;                 ///< Offset 504     HotThermalTripPointWifi
  UINT8    EnablePowerParticipant;                  ///< Offset 505     EnablePowerParticipant
  UINT16   DPTFRsvd0;                               ///< Offset 506     DPTFRsvd0
  UINT16   PowerParticipantPollingRate;             ///< Offset 508     PowerParticipantPollingRate
  UINT8    EnablePowerBossPolicy;                   ///< Offset 510     EnablePowerBossPolicy
  UINT8    EnableVSPolicy;                          ///< Offset 511     EnableVSPolicy
  UINT8    Reserved3[3];                            ///< Offset 512:514
  UINT8    UsbPowerResourceTest;                    ///< Offset 515     RTD3 USB Power Resource config
  UINT8    Rtd3I2C0SensorHub;                       ///< Offset 516     RTD3 support for I2C0 SH
  UINT8    VirtualGpioButtonSxBitmask;              ///< Offset 517     Virtual GPIO button Notify Sleep State Change
  UINT8    IuerButtonEnable;                        ///< Offset 518     IUER Button Enable
  UINT8    IuerConvertibleEnable;                   ///< Offset 519     IUER Convertible Enable
  UINT8    IuerDockEnable;                          ///< Offset 520     IUER Dock Enable
  UINT8    CSNotifyEC;                              ///< Offset 521     EC Notification of Low Power S0 Idle State
  UINT16   Rtd3AudioDelay;                          ///< Offset 522     RTD3 Audio Codec device delay
  UINT16   Rtd3SensorHub;                           ///< Offset 524     RTD3 SensorHub delay time after applying power to device
  UINT16   Rtd3TouchPanelDelay;                     ///< Offset 526     RTD3 TouchPanel delay time after applying power to device
  UINT16   Rtd3TouchPadDelay;                       ///< Offset 528     RTD3 TouchPad delay time after applying power to device
  UINT16   VRRampUpDelay;                           ///< Offset 530     VR Ramp up delay
  UINT8    PstateCapping;                           ///< Offset 532     P-state Capping
  UINT16   Rtd3I2C0ControllerPS0Delay;              ///< Offset 533     Delay in _PS0 after powering up I2C0 Controller
  UINT16   Rtd3I2C1ControllerPS0Delay;              ///< Offset 535     Delay in _PS0 after powering up I2C1 Controller
  UINT16   Rtd3Config0;                             ///< Offset 537     RTD3 Config Setting0(BIT0:ZPODD, BIT1:USB3 Camera Port4(IVCAM), BIT2:PCIe NVMe, Bit4:SKL SDS SIP I2C Touch, BIT6:Card Reader, BIT7:WWAN)
  UINT16   Rtd3Config1;                             ///< Offset 539     RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3, BIT4:Sata Port4, BIT5:Reserved, BIT6:CR1, BIT7:CR2, BIT8:CR3, BIT9:RaidVolumes)
  UINT8    CSDebugLightEC;                          ///< Offset 541     EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
  UINT8    Ps2MouseEnable;                          ///< Offset 542     Ps2 Mouse Enable
  UINT8    Ps2KbMsEnable;                           ///< Offset 543     Ps2 Keyboard and Mouse Enable
  UINT16   SSH0;                                    ///< Offset 544     SSCN-HIGH for I2C0
  UINT16   SSL0;                                    ///< Offset 546     SSCN-LOW  for I2C0
  UINT16   SSD0;                                    ///< Offset 548     SSCN-HOLD for I2C0
  UINT16   FMH0;                                    ///< Offset 550     FMCN-HIGH for I2C0
  UINT16   FML0;                                    ///< Offset 552     FMCN-LOW  for I2C0
  UINT16   FMD0;                                    ///< Offset 554     FMCN-HOLD for I2C0
  UINT16   FPH0;                                    ///< Offset 556     FPCN-HIGH for I2C0
  UINT16   FPL0;                                    ///< Offset 558     FPCN-LOW  for I2C0
  UINT16   FPD0;                                    ///< Offset 560     FPCN-HOLD for I2C0
  UINT16   SSH1;                                    ///< Offset 562     SSCN-HIGH for I2C1
  UINT16   SSL1;                                    ///< Offset 564     SSCN-LOW  for I2C1
  UINT16   SSD1;                                    ///< Offset 566     SSCN-HOLD for I2C1
  UINT16   FMH1;                                    ///< Offset 568     FMCN-HIGH for I2C1
  UINT16   FML1;                                    ///< Offset 570     FMCN-LOW  for I2C1
  UINT16   FMD1;                                    ///< Offset 572     FMCN-HOLD for I2C1
  UINT16   FPH1;                                    ///< Offset 574     FPCN-HIGH for I2C1
  UINT16   FPL1;                                    ///< Offset 576     FPCN-LOW  for I2C1
  UINT16   FPD1;                                    ///< Offset 578     FPCN-HOLD for I2C1
  UINT16   M0C0;                                    ///< Offset 580     M0D3 for I2C0
  UINT16   M1C0;                                    ///< Offset 582     M1D3 for I2C0
  UINT16   M2C0;                                    ///< Offset 584     M0D0 for I2C0
  UINT16   M0C1;                                    ///< Offset 586     M0D3 for I2C1
  UINT16   M1C1;                                    ///< Offset 588     M1D3 for I2C1
  UINT16   M2C1;                                    ///< Offset 590     M0D0 for I2C1
  UINT16   M0C2;                                    ///< Offset 592     M0D3 for SPI0
  UINT16   M1C2;                                    ///< Offset 594     M1D3 for SPI0
  UINT16   M0C3;                                    ///< Offset 596     M0D3 for SPI1
  UINT16   M1C3;                                    ///< Offset 598     M1D3 for SPI1
  UINT16   M0C4;                                    ///< Offset 600     M0D3 for UA00
  UINT16   M1C4;                                    ///< Offset 602     M1D3 for UA00
  UINT16   M0C5;                                    ///< Offset 604     M0D3 for UA01
  UINT16   M1C5;                                    ///< Offset 606     M1D3 for UA01
  UINT8    ThunderboltSmiFunction;                  ///< Offset 608     Thunderbolt(TM) SMI Function Number
  //
  // Driver Mode
  //
  UINT32   GpioIrqRoute;                            ///< Offset 609     GPIO IRQ
  UINT8    DriverModeTouchPanel;                    ///< Offset 613     PIRQS 34,50(GPIO)
  UINT8    DriverModeTouchPad;                      ///< Offset 614     PIRQX 39,55(GPIO)
  UINT8    DriverModeSensorHub;                     ///< Offset 615     PIRQM 28,14(GPIO)
  UINT8    SensorStandby;                           ///< Offset 616     Sensor Standby mode
  UINT8    PL1LimitCS;                              ///< Offset 617     set PL1 limit when entering CS
  UINT16   PL1LimitCSValue;                         ///< Offset 618     PL1 limit value
  UINT8    EnableGen1Participant;                   ///< Offset 620     EnableGen1Participant
  UINT8    ActiveThermalTripPointGen1;              ///< Offset 621     ActiveThermalTripPointGen1
  UINT8    PassiveThermalTripPointGen1;             ///< Offset 622     PassiveThermalTripPointGen1
  UINT8    CriticalThermalTripPointGen1;            ///< Offset 623     CriticalThermalTripPointGen1
  UINT8    HotThermalTripPointGen1;                 ///< Offset 624     HotThermalTripPointGen1
  UINT8    EnableGen2Participant;                   ///< Offset 625     EnableGen2Participant
  UINT8    ActiveThermalTripPointGen2;              ///< Offset 626     ActiveThermalTripPointGen2
  UINT8    PassiveThermalTripPointGen2;             ///< Offset 627     PassiveThermalTripPointGen2
  UINT8    CriticalThermalTripPointGen2;            ///< Offset 628     CriticalThermalTripPointGen2
  UINT8    HotThermalTripPointGen2;                 ///< Offset 629     HotThermalTripPointGen2
  UINT8    EnableWwanTempSensorDevice;              ///< Offset 630     EnableWwanTempSensorDevice
  UINT8    EnableCpuVrTempSensorDevice;             ///< Offset 631     EnableCpuVrTempSensorDevice
  UINT8    EnableSsdTempSensorDevice;               ///< Offset 632     EnableSsdTempSensorDevice
  UINT8    EnableInletFanTempSensorDevice;          ///< Offset 633     EnableInletFanTempSensorDevice
  UINT8    ActiveThermalTripPointInletFan;          ///< Offset 634     ActiveThermalTripPointInletFan
  UINT8    PassiveThermalTripPointInletFan;         ///< Offset 635     PassiveThermalTripPointInletFan
  UINT8    CriticalThermalTripPointInletFan;        ///< Offset 636     CriticalThermalTripPointInletFan
  UINT8    HotThermalTripPointInletFan;             ///< Offset 637     HotThermalTripPointInletFan
  UINT8    SDWE;                                    ///< Offset 638     SCS SDCard sideband D3 Wake Event support (0)Disabled, (1)GPP_B17 pin as GpioInt, (2)GPP_D10 pin as GpioInt
  UINT8    UsbSensorHub;                            ///< Offset 639     Sensor Hub Type - (0)None, (1)USB, (2)I2C Intel, (3)I2C STM
  UINT8    BCV4;                                    ///< Offset 640     Broadcom's Bluetooth adapter's revision
  UINT8    WTV0;                                    ///< Offset 641     I2C0/WITT devices version
  UINT8    WTV1;                                    ///< Offset 642     I2C1/WITT devices version
  UINT8    AtmelPanelFwUpdate;                      ///< Offset 643     Atmel panel FW update Enable/Disable
  UINT8    ThunderboltHotSmi;                       ///< Offset 644     SMI on Hot Plug for TBT devices
  UINT8    TbtGpioFilter;                           ///< Offset 645     Gpio filter to detect USB Hotplug event
  UINT8    ThunderboltHotNotify;                    ///< Offset 646     Notify on Hot Plug for TBT devices
  UINT8    TbtSelector;                             ///< Offset 647     Thunderbolt(TM) Root port selector
  UINT8    WAKFinished;                             ///< Offset 648     WAK Finished
  UINT32   LowPowerS0IdleConstraint;                ///< Offset 649     PEP Constraints
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
  UINT16   VRStaggeringDelay;                       ///< Offset 653     VR Staggering delay
  UINT8    TenSecondPowerButtonEnable;              ///< Offset 655     10sec Power button support
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
  UINT8    GenerationId;                            ///< Offset 656     Generation Id(0=Shark bay, 1=Crescent Bay)
  //
  // DPTF
  //
  UINT8    EnableWWANParticipant;                   ///< Offset 657     EnableWWANParticipant
  UINT8    ActiveThermalTripPointWWAN;              ///< Offset 658     ActiveThermalTripPointWWAN
  UINT8    PassiveThermalTripPointWWAN;             ///< Offset 659     PassiveThermalTripPointWWAN
  UINT8    CriticalThermalTripPointWWAN;            ///< Offset 660     CriticalThermalTripPointWWAN
  UINT8    HotThermalTripPointWWAN;                 ///< Offset 661     HotThermalTripPointWWAN
  UINT8    Reserved5[13];                           ///< Offset 662:674
  UINT16   MinPowerLimit0;                          ///< Offset 675     Minimum Power Limit 0 for DPTF use via PPCC Object
  UINT8    EnableChargerParticipant;                ///< Offset 677     EnableChargerParticipant
  UINT8    CriticalThermalTripPointSaS3;            ///< Offset 678     CriticalThermalTripPointSaS3
  UINT8    CriticalThermalTripPointTmemS3;          ///< Offset 679     CriticalThermalTripPointTMEMS3 @deprecated. Memory Participant is not POR for DPTF
  UINT8    CriticalThermalTripPointAmbientS3;       ///< Offset 680     CriticalThermalTripPointAmbientS3
  UINT8    CriticalThermalTripPointSkinS3;          ///< Offset 681     CriticalThermalTripPointSkinS3
  UINT8    CriticalThermalTripPointExhaustFanS3;    ///< Offset 682     CriticalThermalTripPointExhaustFanS3
  UINT8    CriticalThermalTripPointVrS3;            ///< Offset 683     CriticalThermalTripPointVRS3
  UINT8    CriticalThermalTripPointWifiS3;          ///< Offset 684     CriticalThermalTripPointWifiS3
  UINT8    CriticalThermalTripPointGen1S3;          ///< Offset 685     CriticalThermalTripPointGen1S3
  UINT8    CriticalThermalTripPointGen2S3;          ///< Offset 686     CriticalThermalTripPointGen2S3
  UINT8    CriticalThermalTripPointInletFanS3;      ///< Offset 687     CriticalThermalTripPointInletFanS3
  UINT8    CriticalThermalTripPointWwanS3;          ///< Offset 688     CriticalThermalTripPointWWANS3
  UINT8    CriticalThermalTripPointWGigS3;          ///< Offset 689     CriticalThermalTripPointWGigS3 @deprecated since revision 4.
  UINT8    SataPortState;                           ///< Offset 690     SATA port state, Bit0 - Port0, Bit1 - Port1, Bit2 - Port2, Bit3 - Port3
  //
  // DPTF
  //
  UINT8    EnablePerCParticipant;                   ///< Offset 691     EnablePerCParticipant
  UINT8    ActiveThermalTripPointPerC;              ///< Offset 692     ActiveThermalTripPointPerC
  UINT8    PassiveThermalTripPointPerC;             ///< Offset 693     PassiveThermalTripPointPerC
  UINT8    CriticalThermalTripPointPerC;            ///< Offset 694     CriticalThermalTripPointPerC
  UINT8    CriticalThermalTripPointPerCS3;          ///< Offset 695     CriticalThermalTripPointPerCS3
  UINT8    HotThermalTripPointPerC;                 ///< Offset 696     HotThermalTripPointPerC
  UINT8    EnableGen3Participant;                   ///< Offset 697     EnableGen3Participant
  UINT8    ActiveThermalTripPointGen3;              ///< Offset 698     ActiveThermalTripPointGen3
  UINT8    PassiveThermalTripPointGen3;             ///< Offset 699     PassiveThermalTripPointGen3
  UINT8    CriticalThermalTripPointGen3;            ///< Offset 700     CriticalThermalTripPointGen3
  UINT8    HotThermalTripPointGen3;                 ///< Offset 701     HotThermalTripPointGen3
  UINT8    EnableGen4Participant;                   ///< Offset 702     EnableGen4Participant
  UINT8    ActiveThermalTripPointGen4;              ///< Offset 703     ActiveThermalTripPointGen4
  UINT8    PassiveThermalTripPointGen4;             ///< Offset 704     PassiveThermalTripPointGen4
  UINT8    CriticalThermalTripPointGen4;            ///< Offset 705     CriticalThermalTripPointGen4
  UINT8    HotThermalTripPointGen4;                 ///< Offset 706     HotThermalTripPointGen4
  UINT8    EnableGen5Participant;                   ///< Offset 707     EnableGen5Participant
  UINT8    ActiveThermalTripPointGen5;              ///< Offset 708     ActiveThermalTripPointGen5
  UINT8    PassiveThermalTripPointGen5;             ///< Offset 709     PassiveThermalTripPointGen5
  UINT8    CriticalThermalTripPointGen5;            ///< Offset 710     CriticalThermalTripPointGen5
  UINT8    HotThermalTripPointGen5;                 ///< Offset 711     HotThermalTripPointGen5
  UINT8    EnableGen6Participant;                   ///< Offset 712     EnableGen6Participant
  UINT8    ActiveThermalTripPointGen6;              ///< Offset 713     ActiveThermalTripPointGen6
  UINT8    PassiveThermalTripPointGen6;             ///< Offset 714     PassiveThermalTripPointGen6
  UINT8    CriticalThermalTripPointGen6;            ///< Offset 715     CriticalThermalTripPointGen6
  UINT8    HotThermalTripPointGen6;                 ///< Offset 716     HotThermalTripPointGen6
  UINT8    EcLowPowerMode;                          ///< Offset 717     EC Low Power Mode: 1 - Enabled, 0 - Disabled
  UINT8    CriticalThermalTripPointGen3S3;          ///< Offset 718     CriticalThermalTripPointGen3S3
  UINT8    CriticalThermalTripPointGen4S3;          ///< Offset 719     CriticalThermalTripPointGen4S3
  UINT8    CriticalThermalTripPointGen5S3;          ///< Offset 720     CriticalThermalTripPointGen5S3
  UINT8    CriticalThermalTripPointGen6S3;          ///< Offset 721     CriticalThermalTripPointGen6S3
  UINT8    ThermistorSamplingPeriodGen1;            ///< Offset 722     ThermistorSamplingPeriodGen1
  UINT8    ThermistorSamplingPeriodGen2;            ///< Offset 723     ThermistorSamplingPeriodGen2
  UINT8    ThermistorSamplingPeriodGen3;            ///< Offset 724     ThermistorSamplingPeriodGen3
  UINT8    ThermistorSamplingPeriodGen4;            ///< Offset 725     ThermistorSamplingPeriodGen4
  UINT8    ThermistorSamplingPeriodGen5;            ///< Offset 726     ThermistorSamplingPeriodGen5
  UINT8    ThermistorSamplingPeriodGen6;            ///< Offset 727     ThermistorSamplingPeriodGen6
  UINT8    ThermistorSamplingPeriodGen7;            ///< Offset 728     ThermistorSamplingPeriodGen7
  UINT8    ThermistorSamplingPeriodGen8;            ///< Offset 729     ThermistorSamplingPeriodGen8
  UINT8    SensorSamplingPeriodSen1;                ///< Offset 730     SensorSamplingPeriodSen1
  UINT8    SensorSamplingPeriodSen2;                ///< Offset 731     SensorSamplingPeriodSen2
  UINT8    SensorSamplingPeriodSen3;                ///< Offset 732     SensorSamplingPeriodSen3
  UINT8    SensorSamplingPeriodSen4;                ///< Offset 733     SensorSamplingPeriodSen4
  UINT8    SensorSamplingPeriodSen5;                ///< Offset 734     SensorSamplingPeriodSen5
  UINT8    SensorSamplingPeriodSen6;                ///< Offset 735     SensorSamplingPeriodSen6
  UINT8    SensorSamplingPeriodSen7;                ///< Offset 736     SensorSamplingPeriodSen7
  UINT8    SensorSamplingPeriodSen8;                ///< Offset 737     SensorSamplingPeriodSen8
  UINT8    ThermalSamplingPeriodTMEM;               ///< Offset 738     ThermalSamplingPeriodTMEM @deprecated. Memory Participant is not POR for DPTF
  UINT8    EnableStorageParticipant;                ///< Offset 739     EnableStorageParticipant
  UINT8    ActiveThermalTripPointStrg;              ///< Offset 740     ActiveThermalTripPointStrg
  UINT8    PassiveThermalTripPointStrg;             ///< Offset 741     PassiveThermalTripPointStrg
  UINT8    CriticalThermalTripPointStrg;            ///< Offset 742     CriticalThermalTripPointStrg
  UINT8    CriticalThermalTripPointStrgS3;          ///< Offset 743     CriticalThermalTripPointStrgS3
  UINT8    HotThermalTripPointStrg;                 ///< Offset 744     HotThermalTripPointStrg
  UINT8    EnableVS1Participant;                    ///< Offset 745     EnableVS1Participant
  UINT8    ActiveThermalTripPointVS1;               ///< Offset 746     ActiveThermalTripPointVS1
  UINT8    PassiveThermalTripPointVS1;              ///< Offset 747     PassiveThermalTripPointVS1
  UINT8    CriticalThermalTripPointVS1;             ///< Offset 748     CriticalThermalTripPointVS1
  UINT8    CriticalThermalTripPointVS1S3;           ///< Offset 749     CriticalThermalTripPointVS1S3
  UINT8    HotThermalTripPointVS1;                  ///< Offset 750     HotThermalTripPointVS1
  UINT8    EnableVS2Participant;                    ///< Offset 751     EnableVS2Participant
  UINT8    ActiveThermalTripPointVS2;               ///< Offset 752     ActiveThermalTripPointVS2
  UINT8    PassiveThermalTripPointVS2;              ///< Offset 753     PassiveThermalTripPointVS2
  UINT8    CriticalThermalTripPointVS2;             ///< Offset 754     CriticalThermalTripPointVS2
  UINT8    CriticalThermalTripPointVS2S3;           ///< Offset 755     CriticalThermalTripPointVS2S3
  UINT8    HotThermalTripPointVS2;                  ///< Offset 756     HotThermalTripPointVS2
  UINT8    EnableSen1Participant;                   ///< Offset 757     EnableSen1Participant
  UINT8    ActiveThermalTripPointSen1;              ///< Offset 758     ActiveThermalTripPointSen1
  UINT8    PassiveThermalTripPointSen1;             ///< Offset 759     PassiveThermalTripPointSen1
  UINT8    CriticalThermalTripPointSen1;            ///< Offset 760     CriticalThermalTripPointSen1
  UINT8    HotThermalTripPointSen1;                 ///< Offset 761     HotThermalTripPointSen1
  UINT8    EnableSen2Participant;                   ///< Offset 762     EnableSen2Participant
  UINT8    ActiveThermalTripPointSen2;              ///< Offset 763     ActiveThermalTripPointSen2
  UINT8    PassiveThermalTripPointSen2;             ///< Offset 764     PassiveThermalTripPointSen2
  UINT8    CriticalThermalTripPointSen2;            ///< Offset 765     CriticalThermalTripPointSen2
  UINT8    HotThermalTripPointSen2;                 ///< Offset 766     HotThermalTripPointSen2
  UINT8    EnableSen3Participant;                   ///< Offset 767     EnableSen3Participant
  UINT8    ActiveThermalTripPointSen3;              ///< Offset 768     ActiveThermalTripPointSen3
  UINT8    PassiveThermalTripPointSen3;             ///< Offset 769     PassiveThermalTripPointSen3
  UINT8    CriticalThermalTripPointSen3;            ///< Offset 770     CriticalThermalTripPointSen3
  UINT8    HotThermalTripPointSen3;                 ///< Offset 771     HotThermalTripPointSen3
  UINT8    EnableSen4Participant;                   ///< Offset 772     EnableSen4Participant
  UINT8    ActiveThermalTripPointSen4;              ///< Offset 773     ActiveThermalTripPointSen4
  UINT8    PassiveThermalTripPointSen4;             ///< Offset 774     PassiveThermalTripPointSen4
  UINT8    CriticalThermalTripPointSen4;            ///< Offset 775     CriticalThermalTripPointSen4
  UINT8    HotThermalTripPointSen4;                 ///< Offset 776     HotThermalTripPointSen4
  UINT8    EnableSen5Participant;                   ///< Offset 777     EnableSen5Participant
  UINT8    ActiveThermalTripPointSen5;              ///< Offset 778     ActiveThermalTripPointSen5
  UINT8    PassiveThermalTripPointSen5;             ///< Offset 779     PassiveThermalTripPointSen5
  UINT8    CriticalThermalTripPointSen5;            ///< Offset 780     CriticalThermalTripPointSen5
  UINT8    HotThermalTripPointSen5;                 ///< Offset 781     HotThermalTripPointSen5
  UINT8    EnableSen6Participant;                   ///< Offset 782     EnableSen6Participant
  UINT8    ActiveThermalTripPointSen6;              ///< Offset 783     ActiveThermalTripPointSen6
  UINT8    PassiveThermalTripPointSen6;             ///< Offset 784     PassiveThermalTripPointSen6
  UINT8    CriticalThermalTripPointSen6;            ///< Offset 785     CriticalThermalTripPointSen6
  UINT8    HotThermalTripPointSen6;                 ///< Offset 786     HotThermalTripPointSen6
  UINT8    EnableSen7Participant;                   ///< Offset 787     EnableSen7Participant
  UINT8    ActiveThermalTripPointSen7;              ///< Offset 788     ActiveThermalTripPointSen7
  UINT8    PassiveThermalTripPointSen7;             ///< Offset 789     PassiveThermalTripPointSen7
  UINT8    CriticalThermalTripPointSen7;            ///< Offset 790     CriticalThermalTripPointSen7
  UINT8    HotThermalTripPointSen7;                 ///< Offset 791     HotThermalTripPointSen7
  UINT8    CriticalThermalTripPointSen1S3;          ///< Offset 792     CriticalThermalTripPointSen1S3
  UINT8    CriticalThermalTripPointSen2S3;          ///< Offset 793     CriticalThermalTripPointSen2S3
  UINT8    CriticalThermalTripPointSen3S3;          ///< Offset 794     CriticalThermalTripPointSen3S3
  UINT8    CriticalThermalTripPointSen4S3;          ///< Offset 795     CriticalThermalTripPointSen4S3
  UINT8    CriticalThermalTripPointSen5S3;          ///< Offset 796     CriticalThermalTripPointSen5S3
  UINT8    CriticalThermalTripPointSen6S3;          ///< Offset 797     CriticalThermalTripPointSen6S3
  UINT8    CriticalThermalTripPointSen7S3;          ///< Offset 798     CriticalThermalTripPointSen7S3
  UINT8    EnableIvCamParticipant;                  ///< Offset 799     EnableIvCamParticipant
  UINT8    PowerSharingManagerEnable;               ///< Offset 800     PowerSharingManagerEnable
  UINT8    PsmSplcDomainType1;                      ///< Offset 801     PsmSplcDomainType1
  UINT32   PsmSplcPowerLimit1;                      ///< Offset 802     PsmSplcPowerLimit1
  UINT32   PsmSplcTimeWindow1;                      ///< Offset 806     PsmSplcTimeWindow1
  UINT8    PsmSplcDomainType2;                      ///< Offset 810     PsmSplcDomainType2
  UINT32   PsmSplcPowerLimit2;                      ///< Offset 811     PsmSplcPowerLimit2
  UINT32   PsmSplcTimeWindow2;                      ///< Offset 815     PsmSplcTimeWindow2
  UINT8    PsmDplcDomainType1;                      ///< Offset 819     PsmDplcDomainType1
  UINT8    PsmDplcDomainPreference1;                ///< Offset 820     PsmDplcDomainPreference1
  UINT16   PsmDplcPowerLimitIndex1;                 ///< Offset 821     PsmDplcPowerLimitIndex1
  UINT16   PsmDplcDefaultPowerLimit1;               ///< Offset 823     PsmDplcDefaultPowerLimit1
  UINT32   PsmDplcDefaultTimeWindow1;               ///< Offset 825     PsmDplcDefaultTimeWindow1
  UINT16   PsmDplcMinimumPowerLimit1;               ///< Offset 829     PsmDplcMinimumPowerLimit1
  UINT16   PsmDplcMaximumPowerLimit1;               ///< Offset 831     PsmDplcMaximumPowerLimit1
  UINT16   PsmDplcMaximumTimeWindow1;               ///< Offset 833     PsmDplcMaximumTimeWindow1
  UINT8    PsmDplcDomainType2;                      ///< Offset 835     PsmDplcDomainType2
  UINT8    PsmDplcDomainPreference2;                ///< Offset 836     PsmDplcDomainPreference2
  UINT16   PsmDplcPowerLimitIndex2;                 ///< Offset 837     PsmDplcPowerLimitIndex2
  UINT16   PsmDplcDefaultPowerLimit2;               ///< Offset 839     PsmDplcDefaultPowerLimit2
  UINT32   PsmDplcDefaultTimeWindow2;               ///< Offset 841     PsmDplcDefaultTimeWindow2
  UINT16   PsmDplcMinimumPowerLimit2;               ///< Offset 845     PsmDplcMinimumPowerLimit2
  UINT16   PsmDplcMaximumPowerLimit2;               ///< Offset 847     PsmDplcMaximumPowerLimit2
  UINT16   PsmDplcMaximumTimeWindow2;               ///< Offset 849     PsmDplcMaximumTimeWindow2
  UINT8    WifiEnable;                              ///< Offset 851     WifiEnable
  UINT8    WifiDomainType1;                         ///< Offset 852     WifiDomainType1
  UINT16   WifiPowerLimit1;                         ///< Offset 853     WifiPowerLimit1
  UINT32   WifiTimeWindow1;                         ///< Offset 855     WifiTimeWindow1
  UINT8    WifiDomainType2;                         ///< Offset 859     WifiDomainType2
  UINT16   WifiPowerLimit2;                         ///< Offset 860     WifiPowerLimit2
  UINT32   WifiTimeWindow2;                         ///< Offset 862     WifiTimeWindow2
  UINT8    WifiDomainType3;                         ///< Offset 866     WifiDomainType3
  UINT16   WifiPowerLimit3;                         ///< Offset 867     WifiPowerLimit3
  UINT32   WifiTimeWindow3;                         ///< Offset 869     WifiTimeWindow3
  UINT8    TRxDelay0;                               ///< Offset 873     TRxDelay0
  UINT8    TRxCableLength0;                         ///< Offset 874     TRxCableLength0
  UINT8    TRxDelay1;                               ///< Offset 875     TRxDelay1
  UINT8    TRxCableLength1;                         ///< Offset 876     TRxCableLength1
  UINT8    WrddDomainType1;                         ///< Offset 877     WrddDomainType1
  UINT16   WrddCountryIndentifier1;                 ///< Offset 878     WrddCountryIndentifier1
  UINT8    WrddDomainType2;                         ///< Offset 880     WrddDomainType2
  UINT16   WrddCountryIndentifier2;                 ///< Offset 881     WrddCountryIndentifier2
  UINT8    Reserved6[5];                            ///< Offset 883:887
  UINT8    EnableAPPolicy;                          ///< Offset 888     Adaptive Performance Policy
  UINT16   MinPowerLimit1;                          ///< Offset 889     Minimum Power Limit 1 for DPTF use via PPCC Object
  UINT16   MinPowerLimit2;                          ///< Offset 891     Minimum Power Limit 2 for DPTF use via PPCC Object
  //
  // Intel Serial(R) IO Sensor Device Selection
  //
  UINT8    SDS0;                                    ///< Offset 893     SerialIo Devices for controller0
  UINT8    SDS1;                                    ///< Offset 894     SerialIo Devices for controller1
  UINT8    SDS2;                                    ///< Offset 895     SerialIo Devices for controller2
  UINT8    SDS3;                                    ///< Offset 896     SerialIo Devices for controller3
  UINT8    SDS4;                                    ///< Offset 897     SerialIo Devices for controller4
  UINT8    SDS5;                                    ///< Offset 898     SerialIo Devices for controller5
  UINT8    SDS6;                                    ///< Offset 899     SerialIo Devices for controller6
  UINT8    SDS7;                                    ///< Offset 900     SerialIo Devices for controller7
  UINT8    SDS8;                                    ///< Offset 901     SerialIo Devices for controller8
  UINT8    SDS9;                                    ///< Offset 902     SerialIo Devices for controller9
  UINT8    SDSA;                                    ///< Offset 903     SerialIo Devices for controller10
  UINT8    TPLB;                                    ///< Offset 904     custom touchpanel's bus address
  UINT16   TPLH;                                    ///< Offset 905     custom touchpanel's HID address
  UINT8    WTVX;                                    ///< Offset 907     WITT test devices' version
  UINT8    WITX;                                    ///< Offset 908     WITT test devices' connection point
  UINT8    GPTD;                                    ///< Offset 909     GPIO test devices
  UINT16   GDBT;                                    ///< Offset 910     GPIO test devices' debounce value,
  UINT8    UTKX;                                    ///< Offset 912     UTK test devices' connection point
  UINT8    SPTD;                                    ///< Offset 913     SerialIo additional test devices
  UINT8    GEXN;                                    ///< Offset 914     Gpio expander's bus number
  UINT8    TbtSupport;                              ///< Offset 915     Thunderbolt(TM) support
  UINT8    TbtWakeupSupport;                        ///< Offset 916     Wake From Thunderbolt(TM) Devices
  UINT8    AicArSupport;                            ///< Offset 917     Thunderbolt(TM) AR AIC support
  UINT8    TbtAcpiRemovalSupport;                   ///< Offset 918     TbtAcpiRemovalSupport
  UINT8    TbtFrcPwrGpioAccessType;                 ///< Offset 919     TbtFrcPwrGpioAccessType
  UINT8    TbtFrcPwrExpander;                       ///< Offset 920     TbtFrcPwrExpander
  UINT32   TbtFrcPwrGpioNo;                         ///< Offset 921     TbtFrcPwrGpioNo
  UINT8    TbtFrcPwrGpioLevel;                      ///< Offset 925     TbtFrcPwrGpioLevel
  UINT16   TbtCioPlugEvtAcpiGpeStsAddress;          ///< Offset 926     TbtCioPlugEvtAcpiGpeStsAddress
  UINT8    TbtCioPlugEvtAcpiGpeStsBit;              ///< Offset 928     TbtCioPlugEvtAcpiGpeStsBit
  UINT8    TbtNativeOsHotPlug;                      ///< Offset 929     TbtNativeOsHotPlug @deprecated since revision 25.
  UINT32   TbtCioPlugEventGpioNo;                   ///< Offset 930     TbtCioPlugEventGpioNo
  UINT8    Checkfor2tier;                           ///< Offset 934     Check for 2tier support
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
  UINT8    TbtXConnectSupport;                      ///< Offset 935     ThunderBolt Detachable Graphics Support
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
  UINT8    DGfxDisplayConnectStateChange;           ///< Offset 936     Discrete GPU display connect state change request
  UINT8    DGfxDockEventDetected;                   ///< Offset 937     Dock event
  UINT8    DGfxUndockEventDetected;                 ///< Offset 938     Undock request
  UINT8    DGfxSurpriseUndockEventDetected;         ///< Offset 939     Surprise undock event
  UINT8    DGfxOsHotplugNotification;               ///< Offset 940     OS Hot Plug/Unplug Notifications
  UINT8    DGfxGpuHotplugNotification;              ///< Offset 941     GPU Hot Plug/Unplug Notifications
  UINT32   DGfxPciBaseAddressMem;                   ///< Offset 942     External GPU PCI Configuration Space Buffer Address
  UINT8    DGfxPciBaseAddressMemValid;              ///< Offset 946     External GPU PCI Configuration Space Valid
  UINT32   DGfxTbtDeviceTreeBuffer;                 ///< Offset 947     Thunderbolt Device Tree Info Buffer Address
  UINT32   TableLoadBuffer;                         ///< Offset 951     Buffer for runtime ACPI Table loading
  UINT8    SDM0;                                    ///< Offset 955     interrupt mode for controller0 devices
  UINT8    SDM1;                                    ///< Offset 956     interrupt mode for controller1 devices
  UINT8    SDM2;                                    ///< Offset 957     interrupt mode for controller2 devices
  UINT8    SDM3;                                    ///< Offset 958     interrupt mode for controller3 devices
  UINT8    SDM4;                                    ///< Offset 959     interrupt mode for controller4 devices
  UINT8    SDM5;                                    ///< Offset 960     interrupt mode for controller5 devices
  UINT8    SDM6;                                    ///< Offset 961     interrupt mode for controller6 devices
  UINT8    SDM7;                                    ///< Offset 962     interrupt mode for controller7 devices
  UINT8    SDM8;                                    ///< Offset 963     interrupt mode for controller8 devices
  UINT8    SDM9;                                    ///< Offset 964     interrupt mode for controller9 devices
  UINT8    SDMA;                                    ///< Offset 965     interrupt mode for controller10 devices
  UINT8    USTP;                                    ///< Offset 966     use SerialIo timing parameters
  UINT16   SSHI;                                    ///< Offset 967     SSCN-HIGH for I2C
  UINT16   SSLI;                                    ///< Offset 969     SSCN-LOW  for I2C
  UINT16   SSDI;                                    ///< Offset 971     SSCN-HOLD for I2C
  UINT16   FMHI;                                    ///< Offset 973     FMCN-HIGH for I2C
  UINT16   FMLI;                                    ///< Offset 975     FMCN-LOW  for I2C
  UINT16   FMDI;                                    ///< Offset 977     FMCN-HOLD for I2C
  UINT16   FPHI;                                    ///< Offset 979     FPCN-HIGH for I2C
  UINT16   FPLI;                                    ///< Offset 981     FPCN-LOW  for I2C
  UINT16   FPDI;                                    ///< Offset 983     FPCN-HOLD for I2C
  UINT16   M0CI;                                    ///< Offset 985     M0D3 for I2C
  UINT16   M1CI;                                    ///< Offset 987     M1D3 for I2C
  UINT16   M0CS;                                    ///< Offset 989     M0D3 for SPI
  UINT16   M1CS;                                    ///< Offset 991     M1D3 for SPI
  UINT16   M0CU;                                    ///< Offset 993     M0D3 for UART
  UINT16   M1CU;                                    ///< Offset 995     M1D3 for UART
  //
  // Camera specific policies - Begin
  //
  UINT8    CameraType;                              ///< Offset 997     Type of Camera attached to the System
  UINT8    IvcamDfuSupport;                         ///< Offset 998     3D Camera DFU support
  UINT64   IvcamDfuGpio;                            ///< Offset 999     3D Camera FW Gpio
  UINT8    IvcamWakeSupport;                        ///< Offset 1007    3D Camera wake support
  UINT8    IvcamPresent;                            ///< Offset 1008    Ivcam present in platform
  UINT16   IvcamPowerOnDelay;                       ///< Offset 1009    Delay needed for Ivcam power on
  UINT16   IvcamPowerOffDelay;                      ///< Offset 1011    Delay needed for Ivcam power off
  UINT8    IvcamEpAddress;                          ///< Offset 1013    Ivcam endpoint address
  UINT8    IvcamRotation;                           ///< Offset 1014    Ivcam rotation support
  UINT64   IvcamPwrGpio;                            ///< Offset 1015    3D Camera VDD Gpio
  UINT8    DsPresent;                               ///< Offset 1023    DS present in platform
  UINT16   DsPowerOnDelay;                          ///< Offset 1024    Delay needed for DS camera power on
  UINT16   DsPowerOffDelay;                         ///< Offset 1026    Delay needed for DS camera power off
  UINT8    DsEpAddress;                             ///< Offset 1028    DS endpoint address
  UINT8    DsRotation;                              ///< Offset 1029    DS camera rotation support
  UINT64   DsPwrGpio;                               ///< Offset 1030    DS Camera VDD Gpio
  //
  // Camera specific policies - End
  //
  UINT8    EInkDfuEnable;                           ///< Offset 1038    EInk DFU enable
  UINT32   FingerPrintSleepGpio;                    ///< Offset 1039    Gpio for fingerprint sleep
  UINT32   FingerPrintIrqGpio;                      ///< Offset 1043    Gpio for fingerprint irq
  UINT8    GnssModel;                               ///< Offset 1047    GNSS device model
  UINT8    GnssConnection;                          ///< Offset 1048    GNSS connection, 0=disabled, 1=SerialIO, 2=ISH
  UINT32   GnssResetGpio;                           ///< Offset 1049    Gpio for GNSS reset
  UINT32   BluetoothWakeGpio;                       ///< Offset 1053    Gpio for BlueTooth Wake
  UINT32   BluetoothRfKillGpio;                     ///< Offset 1057    Gpio for BlueTooth rfKill
  UINT32   BluetoothIrqGpio;                        ///< Offset 1061    Gpio for BlueTooth Interrupt
  UINT32   TouchpadIrqGpio;                         ///< Offset 1065    Gpio for touchPaD Interrupt
  UINT32   TouchpanelIrqGpio;                       ///< Offset 1069    Gpio for touchPaneL Interrupt
  //
  // SkyCam specific
  //
  UINT8    SkyCamControlLogic0;                     ///< Offset 1073    
  UINT8    SkyCamControlLogic1;                     ///< Offset 1074    
  UINT8    SkyCamControlLogic2;                     ///< Offset 1075    
  UINT8    SkyCamControlLogic3;                     ///< Offset 1076    
  UINT8    SkyCamLink0Enabled;                      ///< Offset 1077    
  UINT8    SkyCamLink1Enabled;                      ///< Offset 1078    
  UINT8    SkyCamLink2Enabled;                      ///< Offset 1079    
  UINT8    SkyCamLink3Enabled;                      ///< Offset 1080    
  UINT8    SkyCamLanesClkDiv;                       ///< Offset 1081    
  // Control Logic 0 options
  UINT8    SkyCamCtrlLogic0_Type;                   ///< Offset 1082    Type
  UINT8    SkyCamCtrlLogic0_CrdVersion;             ///< Offset 1083    Version of CRD
  UINT8    SkyCamCtrlLogic0_GpioPinsEnabled;        ///< Offset 1084    Number of GPIO Pins enabled
  UINT8    SkyCamCtrlLogic0_I2cBus;                 ///< Offset 1085    I2C Serial Bus Number
  UINT16   SkyCamCtrlLogic0_I2cAddress;             ///< Offset 1086    I2C Address
  UINT8    SkyCamCtrlLogic0_GpioGroupPadNumber[4];  ///< Offset 1088    GPIO Group Pad Number
                                                    ///< Offset 1089    
                                                    ///< Offset 1090    
                                                    ///< Offset 1091    
  UINT8    SkyCamCtrlLogic0_GpioGroupNumber[4];     ///< Offset 1092    GPIO Group Number
                                                    ///< Offset 1093    
                                                    ///< Offset 1094    
                                                    ///< Offset 1095    
  UINT8    SkyCamCtrlLogic0_GpioFunction[4];        ///< Offset 1096    GPIO Function
                                                    ///< Offset 1097    
                                                    ///< Offset 1098    
                                                    ///< Offset 1099    
  UINT8    SkyCamCtrlLogic0_GpioActiveValue[4];     ///< Offset 1100    GPIO Active Value
                                                    ///< Offset 1101    
                                                    ///< Offset 1102    
                                                    ///< Offset 1103    
  UINT8    SkyCamCtrlLogic0_GpioInitialValue[4];    ///< Offset 1104    GPIO Initial Value
                                                    ///< Offset 1105    
                                                    ///< Offset 1106    
                                                    ///< Offset 1107    
  UINT8    SkyCamCtrlLogic0_Pld;                    ///< Offset 1108    Camera Position
  // Control Logic 1 options
  UINT8    SkyCamCtrlLogic1_Type;                   ///< Offset 1109    Type
  UINT8    SkyCamCtrlLogic1_CrdVersion;             ///< Offset 1110    Version of CRD
  UINT8    SkyCamCtrlLogic1_GpioPinsEnabled;        ///< Offset 1111    Number of GPIO Pins enabled
  UINT8    SkyCamCtrlLogic1_I2cBus;                 ///< Offset 1112    I2C Serial Bus Number
  UINT16   SkyCamCtrlLogic1_I2cAddress;             ///< Offset 1113    I2C Address
  UINT8    SkyCamCtrlLogic1_GpioGroupPadNumber[4];  ///< Offset 1115    GPIO Group Pad Number
                                                    ///< Offset 1116    
                                                    ///< Offset 1117    
                                                    ///< Offset 1118    
  UINT8    SkyCamCtrlLogic1_GpioGroupNumber[4];     ///< Offset 1119    GPIO Group Number
                                                    ///< Offset 1120    
                                                    ///< Offset 1121    
                                                    ///< Offset 1122    
  UINT8    SkyCamCtrlLogic1_GpioFunction[4];        ///< Offset 1123    GPIO Function
                                                    ///< Offset 1124    
                                                    ///< Offset 1125    
                                                    ///< Offset 1126    
  UINT8    SkyCamCtrlLogic1_GpioActiveValue[4];     ///< Offset 1127    GPIO Active Value
                                                    ///< Offset 1128    
                                                    ///< Offset 1129    
                                                    ///< Offset 1130    
  UINT8    SkyCamCtrlLogic1_GpioInitialValue[4];    ///< Offset 1131    GPIO Initial Value
                                                    ///< Offset 1132    
                                                    ///< Offset 1133    
                                                    ///< Offset 1134    
  UINT8    SkyCamCtrlLogic1_Pld;                    ///< Offset 1135    Camera Position
  // Control Logic 2 options
  UINT8    SkyCamCtrlLogic2_Type;                   ///< Offset 1136    Type
  UINT8    SkyCamCtrlLogic2_CrdVersion;             ///< Offset 1137    Version of CRD
  UINT8    SkyCamCtrlLogic2_GpioPinsEnabled;        ///< Offset 1138    Number of GPIO Pins enabled
  UINT8    SkyCamCtrlLogic2_I2cBus;                 ///< Offset 1139    I2C Serial Bus Number
  UINT16   SkyCamCtrlLogic2_I2cAddress;             ///< Offset 1140    I2C Address
  UINT8    SkyCamCtrlLogic2_GpioGroupPadNumber[4];  ///< Offset 1142    GPIO Group Pad Number
                                                    ///< Offset 1143    
                                                    ///< Offset 1144    
                                                    ///< Offset 1145    
  UINT8    SkyCamCtrlLogic2_GpioGroupNumber[4];     ///< Offset 1146    GPIO Group Number
                                                    ///< Offset 1147    
                                                    ///< Offset 1148    
                                                    ///< Offset 1149    
  UINT8    SkyCamCtrlLogic2_GpioFunction[4];        ///< Offset 1150    GPIO Function
                                                    ///< Offset 1151    
                                                    ///< Offset 1152    
                                                    ///< Offset 1153    
  UINT8    SkyCamCtrlLogic2_GpioActiveValue[4];     ///< Offset 1154    GPIO Active Value
                                                    ///< Offset 1155    
                                                    ///< Offset 1156    
                                                    ///< Offset 1157    
  UINT8    SkyCamCtrlLogic2_GpioInitialValue[4];    ///< Offset 1158    GPIO Initial Value
                                                    ///< Offset 1159    
                                                    ///< Offset 1160    
                                                    ///< Offset 1161    
  UINT8    SkyCamCtrlLogic2_Pld;                    ///< Offset 1162    Camera Position
  // Control Logic 3 options
  UINT8    SkyCamCtrlLogic3_Type;                   ///< Offset 1163    Type
  UINT8    SkyCamCtrlLogic3_CrdVersion;             ///< Offset 1164    Version of CRD
  UINT8    SkyCamCtrlLogic3_GpioPinsEnabled;        ///< Offset 1165    Number of GPIO Pins enabled
  UINT8    SkyCamCtrlLogic3_I2cBus;                 ///< Offset 1166    I2C Serial Bus Number
  UINT16   SkyCamCtrlLogic3_I2cAddress;             ///< Offset 1167    I2C Address
  UINT8    SkyCamCtrlLogic3_GpioGroupPadNumber[4];  ///< Offset 1169    GPIO Group Pad Number
                                                    ///< Offset 1170    
                                                    ///< Offset 1171    
                                                    ///< Offset 1172    
  UINT8    SkyCamCtrlLogic3_GpioGroupNumber[4];     ///< Offset 1173    GPIO Group Number
                                                    ///< Offset 1174    
                                                    ///< Offset 1175    
                                                    ///< Offset 1176    
  UINT8    SkyCamCtrlLogic3_GpioFunction[4];        ///< Offset 1177    GPIO Function
                                                    ///< Offset 1178    
                                                    ///< Offset 1179    
                                                    ///< Offset 1180    
  UINT8    SkyCamCtrlLogic3_GpioActiveValue[4];     ///< Offset 1181    GPIO Active Value
                                                    ///< Offset 1182    
                                                    ///< Offset 1183    
                                                    ///< Offset 1184    
  UINT8    SkyCamCtrlLogic3_GpioInitialValue[4];    ///< Offset 1185    GPIO Initial Value
                                                    ///< Offset 1186    
                                                    ///< Offset 1187    
                                                    ///< Offset 1188    
  UINT8    SkyCamCtrlLogic3_Pld;                    ///< Offset 1189    Camera Position
  // Sky Cam Link0 options
  UINT8    SkyCamLink0SensorModel;                  ///< Offset 1190    Sensor Model
  UINT8    SkyCamLink0UserHid[9];                   ///< Offset 1191    User defined HID ASCII character 0
                                                    ///< Offset 1192    
                                                    ///< Offset 1193    
                                                    ///< Offset 1194    
                                                    ///< Offset 1195    
                                                    ///< Offset 1196    
                                                    ///< Offset 1197    
                                                    ///< Offset 1198    
                                                    ///< Offset 1199    User defined HID ASCII character 8
  UINT8    SkyCamLink0Pld;                          ///< Offset 1200    Camera Position
  UINT8    SkyCamLink0ModuleName[16];               ///< Offset 1201    Camera Module Name ASCII character 0
                                                    ///< Offset 1202    
                                                    ///< Offset 1203    
                                                    ///< Offset 1204    
                                                    ///< Offset 1205    
                                                    ///< Offset 1206    
                                                    ///< Offset 1207    
                                                    ///< Offset 1208    
                                                    ///< Offset 1209    
                                                    ///< Offset 1210    
                                                    ///< Offset 1211    
                                                    ///< Offset 1212    
                                                    ///< Offset 1213    
                                                    ///< Offset 1214    
                                                    ///< Offset 1215    
                                                    ///< Offset 1216    Camera Module Name ASCII character 15
  UINT8    SkyCamLink0I2cDevicesEnabled;            ///< Offset 1217    Number of I2C devices
  UINT8    SkyCamLink0I2cBus;                       ///< Offset 1218    I2C Serial Bus number
  UINT16   SkyCamLink0I2cAddrDev[12];               ///< Offset 1219    Address of I2C Device0 on Link0
                                                    ///< Offset 1221    Address of I2C Device1 on Link0
                                                    ///< Offset 1223    Address of I2C Device2 on Link0
                                                    ///< Offset 1225    Address of I2C Device3 on Link0
                                                    ///< Offset 1227    Address of I2C Device4 on Link0
                                                    ///< Offset 1229    Address of I2C Device5 on Link0
                                                    ///< Offset 1231    Address of I2C Device6 on Link0
                                                    ///< Offset 1233    Address of I2C Device7 on Link0
                                                    ///< Offset 1235    Address of I2C Device8 on Link0
                                                    ///< Offset 1237    Address of I2C Device9 on Link0
                                                    ///< Offset 1239    Address of I2C Device10 on Link0
                                                    ///< Offset 1241    Address of I2C Device11 on Link0
  UINT8    SkyCamLink0I2cDeviceType[12];            ///< Offset 1243    Type of I2C Device0 on Link0
                                                    ///< Offset 1244    Type of I2C Device1 on Link0
                                                    ///< Offset 1245    Type of I2C Device2 on Link0
                                                    ///< Offset 1246    Type of I2C Device3 on Link0
                                                    ///< Offset 1247    Type of I2C Device4 on Link0
                                                    ///< Offset 1248    Type of I2C Device5 on Link0
                                                    ///< Offset 1249    Type of I2C Device6 on Link0
                                                    ///< Offset 1250    Type of I2C Device7 on Link0
                                                    ///< Offset 1251    Type of I2C Device8 on Link0
                                                    ///< Offset 1252    Type of I2C Device9 on Link0
                                                    ///< Offset 1253    Type of I2C Device10 on Link0
                                                    ///< Offset 1254    Type of I2C Device11 on Link0
  UINT8    SkyCamLink0DD_Version;                   ///< Offset 1255    Version of SSDB structure
  UINT8    SkyCamLink0DD_CrdVersion;                ///< Offset 1256    Version of CRD
  UINT8    SkyCamLink0DD_LinkUsed;                  ///< Offset 1257    CSI2 Link used
  UINT8    SkyCamLink0DD_LaneUsed;                  ///< Offset 1258    MIPI-CSI2 Data Lane
  UINT8    SkyCamLink0DD_EepromType;                ///< Offset 1259    EEPROM Type
  UINT8    SkyCamLink0DD_VcmType;                   ///< Offset 1260    VCM Type
  UINT8    SkyCamLink0DD_FlashSupport;              ///< Offset 1261    Flash Support
  UINT8    SkyCamLink0DD_Degree;                    ///< Offset 1262    Degree
  UINT8    SkyCamLink0DD_CustomData[4];             ///< Offset 1263    Platform @deprecated since revision 9.
                                                    ///< Offset 1264    PlatformSub @deprecated since revision 9.
                                                    ///< Offset 1265    pLED @deprecated since revision 9.
                                                    ///< Offset 1266    mipiDefine @deprecated since revision 9.
  UINT32   SkyCamLink0DD_Mclk;                      ///< Offset 1267    MCLK
  UINT8    SkyCamLink0DD_ControlLogic;              ///< Offset 1271    Control Logic
  // Sky Cam Link1 options
  UINT8    SkyCamLink1SensorModel;                  ///< Offset 1272    Sensor Model
  UINT8    SkyCamLink1UserHid[9];                   ///< Offset 1273    User defined HID ASCII character 0
                                                    ///< Offset 1274    
                                                    ///< Offset 1275    
                                                    ///< Offset 1276    
                                                    ///< Offset 1277    
                                                    ///< Offset 1278    
                                                    ///< Offset 1279    
                                                    ///< Offset 1280    
                                                    ///< Offset 1281    User defined HID ASCII character 8
  UINT8    SkyCamLink1Pld;                          ///< Offset 1282    Camera Position
  UINT8    SkyCamLink1ModuleName[16];               ///< Offset 1283    Camera Module Name ASCII character 0
                                                    ///< Offset 1284    
                                                    ///< Offset 1285    
                                                    ///< Offset 1286    
                                                    ///< Offset 1287    
                                                    ///< Offset 1288    
                                                    ///< Offset 1289    
                                                    ///< Offset 1290    
                                                    ///< Offset 1291    
                                                    ///< Offset 1292    
                                                    ///< Offset 1293    
                                                    ///< Offset 1294    
                                                    ///< Offset 1295    
                                                    ///< Offset 1296    
                                                    ///< Offset 1297    
                                                    ///< Offset 1298    Camera Module Name ASCII character 15
  UINT8    SkyCamLink1I2cDevicesEnabled;            ///< Offset 1299    Number of I2C devices
  UINT8    SkyCamLink1I2cBus;                       ///< Offset 1300    I2C Serial Bus number
  UINT16   SkyCamLink1I2cAddrDev[12];               ///< Offset 1301    Address of I2C Device0 on Link1
                                                    ///< Offset 1303    Address of I2C Device1 on Link1
                                                    ///< Offset 1305    Address of I2C Device2 on Link1
                                                    ///< Offset 1307    Address of I2C Device3 on Link1
                                                    ///< Offset 1309    Address of I2C Device4 on Link1
                                                    ///< Offset 1311    Address of I2C Device5 on Link1
                                                    ///< Offset 1313    Address of I2C Device6 on Link1
                                                    ///< Offset 1315    Address of I2C Device7 on Link1
                                                    ///< Offset 1317    Address of I2C Device8 on Link1
                                                    ///< Offset 1319    Address of I2C Device9 on Link1
                                                    ///< Offset 1321    Address of I2C Device10 on Link1
                                                    ///< Offset 1323    Address of I2C Device11 on Link1
  UINT8    SkyCamLink1I2cDeviceType[12];            ///< Offset 1325    Type of I2C Device0 on Link1
                                                    ///< Offset 1326    Type of I2C Device1 on Link1
                                                    ///< Offset 1327    Type of I2C Device2 on Link1
                                                    ///< Offset 1328    Type of I2C Device3 on Link1
                                                    ///< Offset 1329    Type of I2C Device4 on Link1
                                                    ///< Offset 1330    Type of I2C Device5 on Link1
                                                    ///< Offset 1331    Type of I2C Device6 on Link1
                                                    ///< Offset 1332    Type of I2C Device7 on Link1
                                                    ///< Offset 1333    Type of I2C Device8 on Link1
                                                    ///< Offset 1334    Type of I2C Device9 on Link1
                                                    ///< Offset 1335    Type of I2C Device10 on Link1
                                                    ///< Offset 1336    Type of I2C Device11 on Link1
  UINT8    SkyCamLink1DD_Version;                   ///< Offset 1337    Version of SSDB structure
  UINT8    SkyCamLink1DD_CrdVersion;                ///< Offset 1338    Version of CRD
  UINT8    SkyCamLink1DD_LinkUsed;                  ///< Offset 1339    CSI2 Link used
  UINT8    SkyCamLink1DD_LaneUsed;                  ///< Offset 1340    MIPI-CSI2 Data Lane
  UINT8    SkyCamLink1DD_EepromType;                ///< Offset 1341    EEPROM Type
  UINT8    SkyCamLink1DD_VcmType;                   ///< Offset 1342    VCM Type
  UINT8    SkyCamLink1DD_FlashSupport;              ///< Offset 1343    Flash Support
  UINT8    SkyCamLink1DD_Degree;                    ///< Offset 1344    Degree
  UINT8    SkyCamLink1DD_CustomData[4];             ///< Offset 1345    Platform @deprecated since revision 9.
                                                    ///< Offset 1346    PlatformSub @deprecated since revision 9.
                                                    ///< Offset 1347    pLED @deprecated since revision 9.
                                                    ///< Offset 1348    mipiDefine @deprecated since revision 9.
  UINT32   SkyCamLink1DD_Mclk;                      ///< Offset 1349    MCLK
  UINT8    SkyCamLink1DD_ControlLogic;              ///< Offset 1353    Control Logic
  // Sky Cam Link2 options
  UINT8    SkyCamLink2SensorModel;                  ///< Offset 1354    Sensor Model
  UINT8    SkyCamLink2UserHid[9];                   ///< Offset 1355    User defined HID ASCII character 0
                                                    ///< Offset 1356    
                                                    ///< Offset 1357    
                                                    ///< Offset 1358    
                                                    ///< Offset 1359    
                                                    ///< Offset 1360    
                                                    ///< Offset 1361    
                                                    ///< Offset 1362    
                                                    ///< Offset 1363    User defined HID ASCII character 8
  UINT8    SkyCamLink2Pld;                          ///< Offset 1364    Camera Position
  UINT8    SkyCamLink2ModuleName[16];               ///< Offset 1365    Camera Module Name ASCII character 0
                                                    ///< Offset 1366    
                                                    ///< Offset 1367    
                                                    ///< Offset 1368    
                                                    ///< Offset 1369    
                                                    ///< Offset 1370    
                                                    ///< Offset 1371    
                                                    ///< Offset 1372    
                                                    ///< Offset 1373    
                                                    ///< Offset 1374    
                                                    ///< Offset 1375    
                                                    ///< Offset 1376    
                                                    ///< Offset 1377    
                                                    ///< Offset 1378    
                                                    ///< Offset 1379    
                                                    ///< Offset 1380    Camera Module Name ASCII character 15
  UINT8    SkyCamLink2I2cDevicesEnabled;            ///< Offset 1381    Number of I2C devices
  UINT8    SkyCamLink2I2cBus;                       ///< Offset 1382    I2C Serial Bus number
  UINT16   SkyCamLink2I2cAddrDev[12];               ///< Offset 1383    Address of I2C Device0 on Link2
                                                    ///< Offset 1385    Address of I2C Device1 on Link2
                                                    ///< Offset 1387    Address of I2C Device2 on Link2
                                                    ///< Offset 1389    Address of I2C Device3 on Link2
                                                    ///< Offset 1391    Address of I2C Device4 on Link2
                                                    ///< Offset 1393    Address of I2C Device5 on Link2
                                                    ///< Offset 1395    Address of I2C Device6 on Link2
                                                    ///< Offset 1397    Address of I2C Device7 on Link2
                                                    ///< Offset 1399    Address of I2C Device8 on Link2
                                                    ///< Offset 1401    Address of I2C Device9 on Link2
                                                    ///< Offset 1403    Address of I2C Device10 on Link2
                                                    ///< Offset 1405    Address of I2C Device11 on Link2
  UINT8    SkyCamLink2I2cDeviceType[12];            ///< Offset 1407    Type of I2C Device0 on Link2
                                                    ///< Offset 1408    Type of I2C Device1 on Link2
                                                    ///< Offset 1409    Type of I2C Device2 on Link2
                                                    ///< Offset 1410    Type of I2C Device3 on Link2
                                                    ///< Offset 1411    Type of I2C Device4 on Link2
                                                    ///< Offset 1412    Type of I2C Device5 on Link2
                                                    ///< Offset 1413    Type of I2C Device6 on Link2
                                                    ///< Offset 1414    Type of I2C Device7 on Link2
                                                    ///< Offset 1415    Type of I2C Device8 on Link2
                                                    ///< Offset 1416    Type of I2C Device9 on Link2
                                                    ///< Offset 1417    Type of I2C Device10 on Link2
                                                    ///< Offset 1418    Type of I2C Device11 on Link2
  UINT8    SkyCamLink2DD_Version;                   ///< Offset 1419    Version of SSDB structure
  UINT8    SkyCamLink2DD_CrdVersion;                ///< Offset 1420    Version of CRD
  UINT8    SkyCamLink2DD_LinkUsed;                  ///< Offset 1421    CSI2 Link used
  UINT8    SkyCamLink2DD_LaneUsed;                  ///< Offset 1422    MIPI-CSI2 Data Lane
  UINT8    SkyCamLink2DD_EepromType;                ///< Offset 1423    EEPROM Type
  UINT8    SkyCamLink2DD_VcmType;                   ///< Offset 1424    VCM Type
  UINT8    SkyCamLink2DD_FlashSupport;              ///< Offset 1425    Flash Support
  UINT8    SkyCamLink2DD_Degree;                    ///< Offset 1426    Degree
  UINT8    SkyCamLink2DD_CustomData[4];             ///< Offset 1427    Platform @deprecated since revision 9.
                                                    ///< Offset 1428    PlatformSub @deprecated since revision 9.
                                                    ///< Offset 1429    pLED @deprecated since revision 9.
                                                    ///< Offset 1430    mipiDefine @deprecated since revision 9.
  UINT32   SkyCamLink2DD_Mclk;                      ///< Offset 1431    MCLK
  UINT8    SkyCamLink2DD_ControlLogic;              ///< Offset 1435    Control Logic
  // Sky Cam Link3 options
  UINT8    SkyCamLink3SensorModel;                  ///< Offset 1436    Sensor Model
  UINT8    SkyCamLink3UserHid[9];                   ///< Offset 1437    User defined HID ASCII character 0
                                                    ///< Offset 1438    
                                                    ///< Offset 1439    
                                                    ///< Offset 1440    
                                                    ///< Offset 1441    
                                                    ///< Offset 1442    
                                                    ///< Offset 1443    
                                                    ///< Offset 1444    
                                                    ///< Offset 1445    User defined HID ASCII character 8
  UINT8    SkyCamLink3Pld;                          ///< Offset 1446    Camera Position
  UINT8    SkyCamLink3ModuleName[16];               ///< Offset 1447    Camera Module Name ASCII character 0
                                                    ///< Offset 1448    
                                                    ///< Offset 1449    
                                                    ///< Offset 1450    
                                                    ///< Offset 1451    
                                                    ///< Offset 1452    
                                                    ///< Offset 1453    
                                                    ///< Offset 1454    
                                                    ///< Offset 1455    
                                                    ///< Offset 1456    
                                                    ///< Offset 1457    
                                                    ///< Offset 1458    
                                                    ///< Offset 1459    
                                                    ///< Offset 1460    
                                                    ///< Offset 1461    
                                                    ///< Offset 1462    Camera Module Name ASCII character 15
  UINT8    SkyCamLink3I2cDevicesEnabled;            ///< Offset 1463    Number of I2C devices
  UINT8    SkyCamLink3I2cBus;                       ///< Offset 1464    I2C Serial Bus number
  UINT16   SkyCamLink3I2cAddrDev[12];               ///< Offset 1465    Address of I2C Device0 on Link3
                                                    ///< Offset 1467    Address of I2C Device1 on Link3
                                                    ///< Offset 1469    Address of I2C Device2 on Link3
                                                    ///< Offset 1471    Address of I2C Device3 on Link3
                                                    ///< Offset 1473    Address of I2C Device4 on Link3
                                                    ///< Offset 1475    Address of I2C Device5 on Link3
                                                    ///< Offset 1477    Address of I2C Device6 on Link3
                                                    ///< Offset 1479    Address of I2C Device7 on Link3
                                                    ///< Offset 1481    Address of I2C Device8 on Link3
                                                    ///< Offset 1483    Address of I2C Device9 on Link3
                                                    ///< Offset 1485    Address of I2C Device10 on Link3
                                                    ///< Offset 1487    Address of I2C Device11 on Link3
  UINT8    SkyCamLink3I2cDeviceType[12];            ///< Offset 1489    Type of I2C Device0 on Link3
                                                    ///< Offset 1490    Type of I2C Device1 on Link3
                                                    ///< Offset 1491    Type of I2C Device2 on Link3
                                                    ///< Offset 1492    Type of I2C Device3 on Link3
                                                    ///< Offset 1493    Type of I2C Device4 on Link3
                                                    ///< Offset 1494    Type of I2C Device5 on Link3
                                                    ///< Offset 1495    Type of I2C Device6 on Link3
                                                    ///< Offset 1496    Type of I2C Device7 on Link3
                                                    ///< Offset 1497    Type of I2C Device8 on Link3
                                                    ///< Offset 1498    Type of I2C Device9 on Link3
                                                    ///< Offset 1499    Type of I2C Device10 on Link3
                                                    ///< Offset 1500    Type of I2C Device11 on Link3
  UINT8    SkyCamLink3DD_Version;                   ///< Offset 1501    Version of SSDB structure
  UINT8    SkyCamLink3DD_CrdVersion;                ///< Offset 1502    Version of CRD
  UINT8    SkyCamLink3DD_LinkUsed;                  ///< Offset 1503    CSI2 Link used
  UINT8    SkyCamLink3DD_LaneUsed;                  ///< Offset 1504    MIPI-CSI2 Data Lane
  UINT8    SkyCamLink3DD_EepromType;                ///< Offset 1505    EEPROM Type
  UINT8    SkyCamLink3DD_VcmType;                   ///< Offset 1506    VCM Type
  UINT8    SkyCamLink3DD_FlashSupport;              ///< Offset 1507    Flash Support
  UINT8    SkyCamLink3DD_Degree;                    ///< Offset 1508    Degree
  UINT8    SkyCamLink3DD_CustomData[4];             ///< Offset 1509    Platform @deprecated since revision 9.
                                                    ///< Offset 1510    PlatformSub @deprecated since revision 9.
                                                    ///< Offset 1511    pLED @deprecated since revision 9.
                                                    ///< Offset 1512    mipiDefine @deprecated since revision 9.
  UINT32   SkyCamLink3DD_Mclk;                      ///< Offset 1513    MCLK
  UINT8    SkyCamLink3DD_ControlLogic;              ///< Offset 1517    Control Logic
  UINT8    PciDelayOptimizationEcr;                 ///< Offset 1518    
  UINT8    Reserved8[31];                           ///< Offset 1519:1549
  UINT8    I2SC;                                    ///< Offset 1550    HD Audio I2S Codec Selection
  UINT8    OemDesignVariable0;                      ///< Offset 1551    DPTF Oem Design Variables
  UINT8    OemDesignVariable1;                      ///< Offset 1552    DPTF Oem Design Variables
  UINT8    OemDesignVariable2;                      ///< Offset 1553    DPTF Oem Design Variables
  UINT8    OemDesignVariable3;                      ///< Offset 1554    DPTF Oem Design Variables
  UINT8    OemDesignVariable4;                      ///< Offset 1555    DPTF Oem Design Variables
  UINT8    OemDesignVariable5;                      ///< Offset 1556    DPTF Oem Design Variables
  UINT32   UsbTypeCOpBaseAddr;                      ///< Offset 1557    USB Type C Opregion base address
  UINT8    EnableModernStandby;                     ///< Offset 1561    Enable / Disable Modern Standby Mode
  UINT8    WirelessCharging;                        ///< Offset 1562    WirelessCharging
  UINT8    Reserved9[7];                            ///< Offset 1563:1569
  UINT8    TPLS;                                    ///< Offset 1570    custom touchpanel's bus speed
  UINT8    TPDB;                                    ///< Offset 1571    custom touchpad's bus address
  UINT16   TPDH;                                    ///< Offset 1572    custom touchpad's HID address
  UINT8    TPDS;                                    ///< Offset 1574    custom touchpad's bus speed
  UINT32   HdaDspPpModuleMask;                      ///< Offset 1575    HD-Audio DSP Post-Processing Module Mask
  UINT64   HdaDspPpModCustomGuid1Low;               ///< Offset 1579    HDA PP module custom GUID 1 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid1High;              ///< Offset 1587    HDA PP module custom GUID 1 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid2Low;               ///< Offset 1595    HDA PP module custom GUID 2 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid2High;              ///< Offset 1603    HDA PP module custom GUID 2 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid3Low;               ///< Offset 1611    HDA PP module custom GUID 3 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid3High;              ///< Offset 1619    HDA PP module custom GUID 3 - second 64bit [64-127]
  UINT8    HidEventFilterEnable;                    ///< Offset 1627    HID Event Filter Driver enable
  UINT8    XdciFnEnable;                            ///< Offset 1628    XDCI Enable/Disable status
  UINT8    WrdsWiFiSarEnable;                       ///< Offset 1629    WrdsWiFiSarEnable
  UINT8    WrdsWiFiSarTxPowerLimit1;                ///< Offset 1630    WrdsWiFiSarTxPowerLimit1
  UINT8    WrdsWiFiSarTxPowerLimit2;                ///< Offset 1631    WrdsWiFiSarTxPowerLimit2
  UINT8    WrdsWiFiSarTxPowerLimit3;                ///< Offset 1632    WrdsWiFiSarTxPowerLimit3
  UINT8    WrdsWiFiSarTxPowerLimit4;                ///< Offset 1633    WrdsWiFiSarTxPowerLimit4
  UINT8    WrdsWiFiSarTxPowerLimit5;                ///< Offset 1634    WrdsWiFiSarTxPowerLimit5
  UINT8    WrdsWiFiSarTxPowerLimit6;                ///< Offset 1635    WrdsWiFiSarTxPowerLimit6
  UINT8    WrdsWiFiSarTxPowerLimit7;                ///< Offset 1636    WrdsWiFiSarTxPowerLimit7
  UINT8    WrdsWiFiSarTxPowerLimit8;                ///< Offset 1637    WrdsWiFiSarTxPowerLimit8
  UINT8    WrdsWiFiSarTxPowerLimit9;                ///< Offset 1638    WrdsWiFiSarTxPowerLimit9
  UINT8    WrdsWiFiSarTxPowerLimit10;               ///< Offset 1639    WrdsWiFiSarTxPowerLimit10
  UINT8    RunTimeVmControl;                        ///< Offset 1640    RunTime VM Control. 0:Disabled, 1:Premium PMIC, 2:VR controlled by GPIO, 3:DVRM Without IO EXP, 4:DVRM With IO EXP
  //
  //Feature Specific Data Bits
  //
  UINT8    UsbTypeCSupport;                         ///< Offset 1641    USB Type C Supported
  UINT8    BatteryPresent;                          ///< Offset 1642    Battery Present - Bit0: Real Battery is supported on this platform. Bit1: Virtual Battery is supported on this platform.
  UINT8    TsOnDimmTemperature;                     ///< Offset 1643    TS-on-DIMM temperature
  UINT8    PcdDsEndpointCamera;                     ///< Offset 1644    Ds End Point Camera
  UINT8    PcdPercepDs4Camera;                      ///< Offset 1645    Percep Ds 4 Camera
  UINT8    PcdIvCamera;                             ///< Offset 1646    Disable IV Camera before going into Sx state
  UINT32   HebcValue;                               ///< Offset 1647    HebcValue
  UINT8    PcdRealBattery1Control;                  ///< Offset 1651    Real Battery 1 Control
  UINT8    PcdRealBattery2Control;                  ///< Offset 1652    Real Battery 2 Control
  UINT8    PcdSkyCamSensor;                         ///< Offset 1653    Sky Camera Sensor
  UINT8    PcdHDAudioI2S;                           ///< Offset 1654    HD Audio I2S
  UINT8    PcdNCT6776FCOM;                          ///< Offset 1655    NCT6776F COM
  UINT8    PcdNCT6776FSIO;                          ///< Offset 1656    NCT6776F SIO
  UINT8    PcdNCT6776FHWMON;                        ///< Offset 1657    NCT6776F HWMON
  UINT8    PcdH8S2113SIO;                           ///< Offset 1658    H8S2113 SIO
  UINT8    PcdPowerParticipant;                     ///< Offset 1659    Power Participant
  UINT8    PcdSensorHubApp;                         ///< Offset 1660    Sensor Hub Application
  UINT8    PcdEInkApp;                              ///< Offset 1661    EInk Application
  UINT8    PcdZPODD;                                ///< Offset 1662    ZPODD
  UINT32   PcdSmcRuntimeSciPin;                     ///< Offset 1663    SMC Runtime Sci Pin
  UINT8    PcdConvertableDockSupport;               ///< Offset 1667    Convertable Dock Support
  UINT8    PcdEcHotKeyF3Support;                    ///< Offset 1668    Ec Hotkey F3 Support
  UINT8    PcdEcHotKeyF4Support;                    ///< Offset 1669    Ec Hotkey F4 Support
  UINT8    PcdEcHotKeyF5Support;                    ///< Offset 1670    Ec Hotkey F5 Support
  UINT8    PcdEcHotKeyF6Support;                    ///< Offset 1671    Ec Hotkey F6 Support
  UINT8    PcdEcHotKeyF7Support;                    ///< Offset 1672    Ec Hotkey F7 Support
  UINT8    PcdEcHotKeyF8Support;                    ///< Offset 1673    Ec Hotkey F8 Support
  UINT8    PcdVirtualButtonVolumeUpSupport;         ///< Offset 1674    Virtual Button Volume Up Support
  UINT8    PcdVirtualButtonVolumeDownSupport;       ///< Offset 1675    Virtual Button Volume Down Support
  UINT8    PcdVirtualButtonHomeButtonSupport;       ///< Offset 1676    Virtual Button Home Button Support
  UINT8    PcdVirtualButtonRotationLockSupport;     ///< Offset 1677    Virtual Button Rotation Lock Support
  UINT8    PcdSlateModeSwitchSupport;               ///< Offset 1678    Slate Mode Switch Support
  UINT8    PcdVirtualGpioButtonSupport;             ///< Offset 1679    Virtual Button Support  @deprecated
  UINT8    PcdAcDcAutoSwitchSupport;                ///< Offset 1680    Ac Dc Auto Switch Support
  UINT32   PcdPmPowerButtonGpioPin;                 ///< Offset 1681    Pm Power Button Gpio Pin
  UINT8    PcdAcpiEnableAllButtonSupport;           ///< Offset 1685    Acpi Enable All Button Support
  UINT8    PcdAcpiHidDriverButtonSupport;           ///< Offset 1686    Acpi Hid Driver Button Support
  UINT8    PcdPowerButtonSciDisableSupport;         ///< Offset 1687    Power Button SCI disable support
  UINT8    DisplayDepthLowerLimit;                  ///< Offset 1688    DPTF Display Depth Lower Limit in percent
  UINT8    DisplayDepthUpperLimit;                  ///< Offset 1689    DPTF Display Depth Upper Limit in percent
  UINT8    PepWiGigF1;                              ///< Offset 1690    PEP F1 constraints for WiGig device
  UINT8    PcdMchpEspiSupportEnable;                ///< Offset 1691    Enable eSPI support
  //
  // DPTF IVCAM Participant
  //
  UINT8    ActiveThermalTripPointIvCam;             ///< Offset 1692    ActiveThermalTripPointIvCam
  UINT8    PassiveThermalTripPointIvCam;            ///< Offset 1693    PassiveThermalTripPointIvCam
  UINT8    CriticalThermalTripPointIvCam;           ///< Offset 1694    CriticalThermalTripPointIvCam
  UINT8    CriticalThermalTripPointIvCamS3;         ///< Offset 1695    CriticalThermalTripPointIvCamS3
  UINT8    HotThermalTripPointIvCam;                ///< Offset 1696    HotThermalTripPointIvCam
  //
  // XTU SMI base address
  //
  UINT32   XtuSmiBaseAddress;                       ///< Offset 1697    XTU SMI structure Base Address
  UINT32   PcdPchAudioPowerEnable;                  ///< Offset 1701    Pch Audio Power Enable
  UINT32   PcdPcieSlot2Wake;                        ///< Offset 1705    Pcie Slot 2 Wake
  UINT32   PcdM2WlanReset;                          ///< Offset 1709    M.2 Wireless Lan Reset
  UINT32   PcdUsbP8Power;                           ///< Offset 1713    Usb Port 8 Power
  UINT32   PcdM2PcieSsd2Reset;                      ///< Offset 1717    M.2 Pcie Ssd2 Reset
  UINT32   PcdM2PcieSsd2Power;                      ///< Offset 1721    M.2 Pcie Ssd2 Power
  //
  // UCMC setup option, GPIO Pad
  //
  UINT8    UCSI;                                    ///< Offset 1725    Option to Enable/Disable UCSI/UCMC device
  UINT32   UcmcPort1Gpio;                           ///< Offset 1726    Gpio for UCMC Port 1 Interrupt
  UINT32   UcmcPort2Gpio;                           ///< Offset 1730    Gpio for UCMC Port 2 Interrupt
  UINT32   PcdWwanGnssUartResetGpio;                ///< Offset 1734    Wwan Gnss Uart Reset GPIO
  UINT8    WigigRfe;                                ///< Offset 1738    WigigRfe
  UINT8    WiGigRfeCh1;                             ///< Offset 1739    WiGigRfeCh1
  UINT8    WiGigRfeCh2;                             ///< Offset 1740    WiGigRfeCh2
  UINT8    WiGigRfeCh3;                             ///< Offset 1741    WiGigRfeCh3
  UINT8    WiGigRfeCh4;                             ///< Offset 1742    WiGigRfeCh4
  UINT32   AwvClassIndex;                           ///< Offset 1743    AwvClassIndex
  UINT8    Reserved11[13];                          ///< Offset 1747:1759
  UINT8    ThermalSamplingPeriodWifi;               ///< Offset 1760    ThermalSamplingPeriodWifi
  UINT8    WakeOnWiGigSupport;                      ///< Offset 1761    Wake on S3-S4 WiGig Docking Support
  UINT8    EnablePIDPolicy;                         ///< Offset 1762    EnablePIDPolicy
  UINT8    SkyCamCtrlLogic0Version;                 ///< Offset 1763    Version of CLDB structure
  UINT8    SkyCamCtrlLogic0Wled1FlashMaxCurrent;    ///< Offset 1764    WLED1 Flash Max Current
  UINT8    SkyCamCtrlLogic0Wled1TorchMaxCurrent;    ///< Offset 1765    WLED1 Torch Max Current
  UINT8    SkyCamCtrlLogic0Wled2FlashMaxCurrent;    ///< Offset 1766    WLED2 Flash Max Current
  UINT8    SkyCamCtrlLogic0Wled2TorchMaxCurrent;    ///< Offset 1767    WLED2 Torch Max Current
  UINT8    SkyCamCtrlLogic0Wled1Type;               ///< Offset 1768    WLED1 Type
  UINT8    SkyCamCtrlLogic0Wled2Type;               ///< Offset 1769    WLED2 Type
  UINT8    SkyCamCtrlLogic1Version;                 ///< Offset 1770    Version of CLDB structure
  UINT8    SkyCamCtrlLogic1Wled1FlashMaxCurrent;    ///< Offset 1771    WLED1 Flash Max Current
  UINT8    SkyCamCtrlLogic1Wled1TorchMaxCurrent;    ///< Offset 1772    WLED1 Torch Max Current
  UINT8    SkyCamCtrlLogic1Wled2FlashMaxCurrent;    ///< Offset 1773    WLED2 Flash Max Current
  UINT8    SkyCamCtrlLogic1Wled2TorchMaxCurrent;    ///< Offset 1774    WLED2 Torch Max Current
  UINT8    SkyCamCtrlLogic1Wled1Type;               ///< Offset 1775    WLED1 Type
  UINT8    SkyCamCtrlLogic1Wled2Type;               ///< Offset 1776    WLED2 Type
  UINT8    SkyCamCtrlLogic2Version;                 ///< Offset 1777    Version of CLDB structure
  UINT8    SkyCamCtrlLogic2Wled1FlashMaxCurrent;    ///< Offset 1778    WLED1 Flash Max Current
  UINT8    SkyCamCtrlLogic2Wled1TorchMaxCurrent;    ///< Offset 1779    WLED1 Torch Max Current
  UINT8    SkyCamCtrlLogic2Wled2FlashMaxCurrent;    ///< Offset 1780    WLED2 Flash Max Current
  UINT8    SkyCamCtrlLogic2Wled2TorchMaxCurrent;    ///< Offset 1781    WLED2 Torch Max Current
  UINT8    SkyCamCtrlLogic2Wled1Type;               ///< Offset 1782    WLED1 Type
  UINT8    SkyCamCtrlLogic2Wled2Type;               ///< Offset 1783    WLED2 Type
  UINT8    SkyCamCtrlLogic3Version;                 ///< Offset 1784    Version of CLDB structure
  UINT8    SkyCamCtrlLogic3Wled1FlashMaxCurrent;    ///< Offset 1785    WLED1 Flash Max Current
  UINT8    SkyCamCtrlLogic3Wled1TorchMaxCurrent;    ///< Offset 1786    WLED1 Torch Max Current
  UINT8    SkyCamCtrlLogic3Wled2FlashMaxCurrent;    ///< Offset 1787    WLED2 Flash Max Current
  UINT8    SkyCamCtrlLogic3Wled2TorchMaxCurrent;    ///< Offset 1788    WLED2 Torch Max Current
  UINT8    SkyCamCtrlLogic3Wled1Type;               ///< Offset 1789    WLED1 Type
  UINT8    SkyCamCtrlLogic3Wled2Type;               ///< Offset 1790    WLED2 Type
  UINT8    SkyCamLink0DDPrivacyLed;                 ///< Offset 1791    Privacy LED
  UINT8    SkyCamLink0DDPmicPosition;               ///< Offset 1792    PMIC Position
  UINT8    SkyCamLink0DDVoltageRail;                ///< Offset 1793    Voltage Rail
  UINT8    SkyCamLink1DDPrivacyLed;                 ///< Offset 1794    Privacy LED
  UINT8    SkyCamLink1DDPmicPosition;               ///< Offset 1795    PMIC Position
  UINT8    SkyCamLink1DDVoltageRail;                ///< Offset 1796    Voltage Rail
  UINT8    SkyCamLink2DDPrivacyLed;                 ///< Offset 1797    Privacy LED
  UINT8    SkyCamLink2DDPmicPosition;               ///< Offset 1798    PMIC Position
  UINT8    SkyCamLink2DDVoltageRail;                ///< Offset 1799    Voltage Rail
  UINT8    SkyCamLink3DDPrivacyLed;                 ///< Offset 1800    Privacy LED
  UINT8    SkyCamLink3DDPmicPosition;               ///< Offset 1801    PMIC Position
  UINT8    SkyCamLink3DDVoltageRail;                ///< Offset 1802    Voltage Rail
  UINT8    PcdWLanRootPort;                         ///< Offset 1803    WLAN PCIe root port value  @deprecated since revision 18
  UINT8    PcdSsdRootPort;                          ///< Offset 1804    SSD PCIe root port value    @deprecated since revision 18
  UINT8    PcdWigigSupported;                       ///< Offset 1805    WIGIG present or not     @deprecated since revision 18
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  UINT8    TbtSelector1;                            ///< Offset 1806    Thunderbolt(TM) Root port selector
  UINT8    Multicontroller;                         ///< Offset 1807    Thunderbolt(TM) Multiport support
  UINT8    TbtFrcPwrGpioAccessType1;                ///< Offset 1808    TbtFrcPwrGpioAccessType
  UINT8    TbtFrcPwrExpander1;                      ///< Offset 1809    TbtFrcPwrExpander
  UINT32   TbtFrcPwrGpioNo1;                        ///< Offset 1810    TbtFrcPwrGpioNo
  UINT8    TbtFrcPwrGpioLevel1;                     ///< Offset 1814    TbtFrcPwrGpioLevel
  UINT16   TbtCioPlugEvtAcpiGpeStsAddress1;         ///< Offset 1815    TbtCioPlugEvtAcpiGpeStsAddress
  UINT8    TbtCioPlugEvtAcpiGpeStsBit1;             ///< Offset 1817    TbtCioPlugEvtAcpiGpeStsBit
  UINT32   TbtCioPlugEventGpioNo1;                  ///< Offset 1818    TbtCioPlugEventGpioNo
#else
  UINT8    ReservedTBT[16];                         ///< Offset 1806:1821
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
  UINT8    UsbTypeCMultiPortSupport;                ///< Offset 1822    USB type C Multi Port Support
  UINT8    TbtUsbSplitMode;                         ///< Offset 1823    Tbt USB Type C Split mode
  UINT8    SplitModePchUsb2TbtPort1;                ///< Offset 1824    Tbt USB Type C Split mode
  UINT8    SplitModePchUsb2TbtPort2;                ///< Offset 1825    Tbt USB Type C Split mode
  UINT8    TbtWin10Support;                         ///< Offset 1826    TbtWin10Support
  UINT8    TrOsup;                                  ///< Offset 1827    Titan Ridge Osup command
  UINT8    PcdPcieWwanEnable;                       ///< Offset 1828    Pcie Wwan Enable @deprecated since revision 23
  UINT8    Reserved12[41];                          ///< Offset 1829:1869
  UINT32   EcLowPowerModeGpioPin;                   ///< Offset 1870    EcLowPowerModeGpioPin
  UINT32   EcSmiGpioPin;                            ///< Offset 1874    EcSmiGpioPin
  UINT8    Reserved13[2];                           ///< Offset 1878:1879
  UINT8    UcsiRetryEC;                             ///< Offset 1880    UcsiRetryEC
  UINT16   Rtd3TbtOffDelay;                         ///< Offset 1881    Rtd3TbtOffDelay TBT RTD3 Off Delay
  UINT8    TbtSxWakeSwitchLogicEnable;              ///< Offset 1883    TbtSxWakeSwitchLogicEnable Set True if TBT_WAKE_N will be routed to PCH WakeB at Sx entry point. HW logic is required.
  UINT32   RunTimeVmVrGpio;                         ///< Offset 1884    RunTime VM GPIO pin for VR
  UINT8    PcdVsctTableForSens;                     ///< Offset 1888    DPTF GENx/SENx selection in VSCT table for Virx participants
  UINT8    Rtd3TbtSupport;                          ///< Offset 1889    Enable Rtd3 support for TBT. Corresponding to Rtd3Tbt in Setup.
  UINT8    Rtd3TbtClkReq;                           ///< Offset 1890    Enable TBT RTD3 CLKREQ mask.
  UINT16   Rtd3TbtClkReqDelay;                      ///< Offset 1891    TBT RTD3 CLKREQ mask delay.
  UINT8    TbtEnable;                               ///< Offset 1893    TBT Enable/Disable
  UINT8    Rtd3WwanDsm;                             ///< Offset 1894    WWAN D3/L3 Enable/Disable
  UINT16   WwanDeviceControlReg;                    ///< Offset 1895    WWAN Device Control register save area
  UINT16   WwanLinkControlReg;                      ///< Offset 1897    WWAN Link Control register save area
  UINT16   WwanDeviceControlReg2;                   ///< Offset 1899    WWAN Device Control register save area
  UINT16   WwanMaxSnoopLatency;                     ///< Offset 1901    WWAN LTR Max Snoop Latency save area
  UINT16   WwanMaxNoSnoopLatency;                   ///< Offset 1903    WWAN LTR Max No Snoop Latency save area
  UINT8    SensorSamplingPeriodDGFC;                ///< Offset 1905    SensorSamplingPeriodDGFC
  UINT8    SensorSamplingPeriodDGHM;                ///< Offset 1906    SensorSamplingPeriodDGHM
  UINT8    EnableDGFCParticipant;                   ///< Offset 1907    EnableDGFCParticipant
  UINT8    ActiveThermalTripPointDGFC;              ///< Offset 1908    ActiveThermalTripPointDGFC
  UINT8    PassiveThermalTripPointDGFC;             ///< Offset 1909    PassiveThermalTripPointDGFC
  UINT8    CriticalThermalTripPointDGFC;            ///< Offset 1910    CriticalThermalTripPointDGFC
  UINT8    HotThermalTripPointDGFC;                 ///< Offset 1911    HotThermalTripPointDGFC
  UINT8    CriticalThermalTripPointDGFCS3;          ///< Offset 1912    CriticalThermalTripPointDGFCS3
  UINT32   PpccStepSizeDGFC;                        ///< Offset 1913    PPCC Step Size DGFC
  UINT16   MinPowerLimit0DGFC;                      ///< Offset 1917    Minimum Power Limit 0 for DGFC DPTF use via PPCC Object
  UINT8    EnableDGHMParticipant;                   ///< Offset 1919    EnableDGHMParticipant
  UINT8    ActiveThermalTripPointDGHM;              ///< Offset 1920    ActiveThermalTripPointDGHM
  UINT8    PassiveThermalTripPointDGHM;             ///< Offset 1921    PassiveThermalTripPointDGHM
  UINT8    CriticalThermalTripPointDGHM;            ///< Offset 1922    CriticalThermalTripPointDGHM
  UINT8    HotThermalTripPointDGHM;                 ///< Offset 1923    HotThermalTripPointDGHM
  UINT8    CriticalThermalTripPointDGHMS3;          ///< Offset 1924    CriticalThermalTripPointDGHMS3
  UINT8    EnableMCPParticipant;                    ///< Offset 1925    EnableMCPParticipant
  UINT32   PpccStepSizeMCP;                         ///< Offset 1926    PPCC Step Size MCP
  UINT32   MinPowerLimit0MCP;                       ///< Offset 1930    Minimum Power Limit 0 for MCP DPTF use via PPCC Object
  UINT8    EnablePowerSharePolicy;                  ///< Offset 1934    EnablePowerSharePolicy
  UINT8    PcdEnablePbok;                           ///< Offset 1935    PcdEnablePbok
  UINT8    WwanRtd3Option;                          ///< Offset 1936    WWAN RTD3 options
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax1;         ///< Offset 1937    WgdsWiFiSarDeltaGroup1PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA1;      ///< Offset 1938    WgdsWiFiSarDeltaGroup1PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB1;      ///< Offset 1939    WgdsWiFiSarDeltaGroup1PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax2;         ///< Offset 1940    WgdsWiFiSarDeltaGroup1PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA2;      ///< Offset 1941    WgdsWiFiSarDeltaGroup1PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB2;      ///< Offset 1942    WgdsWiFiSarDeltaGroup1PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax1;         ///< Offset 1943    WgdsWiFiSarDeltaGroup2PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA1;      ///< Offset 1944    WgdsWiFiSarDeltaGroup2PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB1;      ///< Offset 1945    WgdsWiFiSarDeltaGroup2PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax2;         ///< Offset 1946    WgdsWiFiSarDeltaGroup2PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA2;      ///< Offset 1947    WgdsWiFiSarDeltaGroup2PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB2;      ///< Offset 1948    WgdsWiFiSarDeltaGroup2PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax1;         ///< Offset 1949    WgdsWiFiSarDeltaGroup3PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA1;      ///< Offset 1950    WgdsWiFiSarDeltaGroup3PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB1;      ///< Offset 1951    WgdsWiFiSarDeltaGroup3PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax2;         ///< Offset 1952    WgdsWiFiSarDeltaGroup3PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA2;      ///< Offset 1953    WgdsWiFiSarDeltaGroup3PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB2;      ///< Offset 1954    WgdsWiFiSarDeltaGroup3PowerChainB2
  UINT8    BluetoothSar;                            ///< Offset 1955    BluetoothSar
  UINT8    BluetoothSarBr;                          ///< Offset 1956    BluetoothSarBr
  UINT8    BluetoothSarEdr2;                        ///< Offset 1957    BluetoothSarEdr2
  UINT8    BluetoothSarEdr3;                        ///< Offset 1958    BluetoothSarEdr3
  UINT8    BluetoothSarLe;                          ///< Offset 1959    BluetoothSarLe
  UINT8    BluetoothSarLe2Mhz;                      ///< Offset 1960    BluetoothSarLe2Mhz
  UINT8    BluetoothSarLeLr;                        ///< Offset 1961    BluetoothSarLeLr
  // Power Meter Support
  UINT8    PowerMeterEnable;                        ///< Offset 1962    PowerMeterEnable
  UINT8    PAC193xPowerMeterSupport;                ///< Offset 1963    PAC193xPowerMeterSupport
  UINT8    PowerMeterI2cControllerNumber;           ///< Offset 1964    PowerMeterI2cControllerNumber
  // Time Alaram Device Support
  UINT8    SystemTimeAndAlarmSource;                ///< Offset 1965    Select source for System time and alarm
  UINT8    LidSwitchGpe0Number;                     ///< Offset 1966    S3 wake capability LID Switch GPE0 Number
  UINT32   LowPowerS0IdleConstraint1;               ///< Offset 1967    PEP Constraints 01
  // Bit[0]  - En/Dis SkyCamSensors
  UINT8    SerialPortAcpiDebug;                     ///< Offset 1971    Serial Port ACPI debug
// AMI_OVERRIDE_START - Windows 8.1 Secure Flash Test failed.
  UINT8    AmiSleepEntryS3;                         ///< Offset 1972    Entry S3
// AMI_OVERRIDE_END - Windows 8.1 Secure Flash Test failed.
} EFI_GLOBAL_NVS_AREA;

#pragma pack(pop)
#endif
