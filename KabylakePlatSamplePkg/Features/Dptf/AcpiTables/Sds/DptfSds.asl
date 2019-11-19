/** @file
  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2019 Intel Corporation.

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

// AMI_OVERRIDE_START - Add to avoid building failed because doesn't have the definition of BIT number. >>
//#include <Base.h>
// AMI_OVERRIDE_END - Add to avoid building failed because doesn't have the definition of BIT number. <<
#include "CpuPowerMgmt.h"

DefinitionBlock (
  "DptfSds.aml",
  "SSDT",
  2,
  "DptfSd",
  "DptfSds_",
  0x1000
  )
{

External(\P8XH, MethodObj)
External(\TSOD, IntObj)
External(\PNHM, IntObj)
External(\TCNT, IntObj)
External(\MPL0, IntObj)
External(\MPL1, IntObj)
External(\MPL2, IntObj)
External(\PWRS, IntObj)
External(\LPMV, IntObj)
External(\CRTT, IntObj)
External(\ATMC, IntObj)
External(\PTMC, IntObj)
External(\ACTT, IntObj)
External(\PSVT, IntObj)
External(\ATPC, IntObj)
External(\PTPC, IntObj)

External(\DPTF, IntObj) // EnableDptf
External(\DCFE, IntObj) // EnableDCFG

External(\SADE, IntObj) // EnableSaDevice
External(\SACR, IntObj) // CriticalThermalTripPointSa
External(\SAC3, IntObj) // CriticalThermalTripPointSaS3
External(\SAHT, IntObj) // HotThermalTripPointSa
External(\CPUS, IntObj) // ThermalSamplingPeriodSA

External(\FND1, IntObj) // EnableFan1Device
External(\FND2, IntObj) // EnableFan2Device

//
// Wireless Participant
//
External(\WIFD, IntObj) // EnableWifiDevice
External(\WFAT, IntObj) // ActiveThermalTripPointWifi
External(\WFPT, IntObj) // PassiveThermalTripPointWifi
External(\WFCT, IntObj) // CriticalThermalTripPointWifi
External(\WFC3, IntObj) // CriticalThermalTripPointWifiS3
External(\WFHT, IntObj) // ActiveHotThermalTripPointWifi
External(\WTSP, IntObj) // ThermalSamplingPeriodWifi

External(\WAND, IntObj) // EnableWWANDevice
External(\WWAT, IntObj) // ActiveThermalTripPointWWAN
External(\WWPT, IntObj) // PassiveThermalTripPointWWAN
External(\WWCT, IntObj) // CriticalThermalTripPointWWAN
External(\WWC3, IntObj) // CriticalThermalTripPointWwanS3
External(\WWHT, IntObj) // ActiveHotThermalTripPointWWAN



External(\S1DE, IntObj) // EnableSen1Participant
External(\S1AT, IntObj) // ActiveThermalTripPointSen1
External(\S1PT, IntObj) // PassiveThermalTripPointSen1
External(\S1CT, IntObj) // CriticalThermalTripPointSen1
External(\S1S3, IntObj) // CriticalThermalTripPointSen1S3
External(\S1HT, IntObj) // HotThermalTripPointSen1
External(\SSP1, IntObj) // SensorSamplingPeriodSen1

External(\S2DE, IntObj) // EnableSen2Participant
External(\S2AT, IntObj) // ActiveThermalTripPointSen2
External(\S2PT, IntObj) // PassiveThermalTripPointSen2
External(\S2CT, IntObj) // CriticalThermalTripPointSen2
External(\S2S3, IntObj) // CriticalThermalTripPointSen2S3
External(\S2HT, IntObj) // HotThermalTripPointSen2
External(\SSP2, IntObj) // SensorSamplingPeriodSen2

External(\S3DE, IntObj) // EnableSen3Participant
External(\S3AT, IntObj) // ActiveThermalTripPointSen3
External(\S3PT, IntObj) // PassiveThermalTripPointSen3
External(\S3CT, IntObj) // CriticalThermalTripPointSen3
External(\S3S3, IntObj) // CriticalThermalTripPointSen3S3
External(\S3HT, IntObj) // HotThermalTripPointSen3
External(\SSP3, IntObj) // SensorSamplingPeriodSen3

External(\S4DE, IntObj) // EnableSen4Participant
External(\S4AT, IntObj) // ActiveThermalTripPointSen4
External(\S4PT, IntObj) // PassiveThermalTripPointSen4
External(\S4CT, IntObj) // CriticalThermalTripPointSen4
External(\S4S3, IntObj) // CriticalThermalTripPointSen4S3
External(\S4HT, IntObj) // HotThermalTripPointSen4
External(\SSP4, IntObj) // SensorSamplingPeriodSen4

External(\S5DE, IntObj) // EnableSen5Participant
External(\S5AT, IntObj) // ActiveThermalTripPointSen5
External(\S5PT, IntObj) // PassiveThermalTripPointSen5
External(\S5CT, IntObj) // CriticalThermalTripPointSen5
External(\S5S3, IntObj) // CriticalThermalTripPointSen5S3
External(\S5HT, IntObj) // HotThermalTripPointSen5
External(\SSP5, IntObj) // SensorSamplingPeriodSen5

External(\S6DE, IntObj) // EnableSen6Participant
External(\S6AT, IntObj) // ActiveThermalTripPointSen6
External(\S6PT, IntObj) // PassiveThermalTripPointSen6
External(\S6CT, IntObj) // CriticalThermalTripPointSen6
External(\S6S3, IntObj) // CriticalThermalTripPointSen6S3
External(\S6HT, IntObj) // HotThermalTripPointSen6
External(\SSP6, IntObj) // SensorSamplingPeriodSen6

External(\S7DE, IntObj) // EnableSen7Participant
External(\S7AT, IntObj) // ActiveThermalTripPointSen7
External(\S7PT, IntObj) // PassiveThermalTripPointSen7
External(\S7CT, IntObj) // CriticalThermalTripPointSen7
External(\S7S3, IntObj) // CriticalThermalTripPointSen7S3
External(\S7HT, IntObj) // HotThermalTripPointSen7
External(\SSP7, IntObj) // SensorSamplingPeriodSen7

External(\S8DE, IntObj) // EnableSen8Participant
External(\S8AT, IntObj) // ActiveThermalTripPointSen8
External(\S8PT, IntObj) // PassiveThermalTripPointSen8
External(\S8CT, IntObj) // CriticalThermalTripPointSen8
External(\S8S3, IntObj) // CriticalThermalTripPointSen8S3
External(\S8HT, IntObj) // HotThermalTripPointSen8
External(\SSP8, IntObj) // SensorSamplingPeriodSen8

External(\GN1E, IntObj) // EnableGen1Participant
External(\G1AT, IntObj) // ActiveThermalTripPointGen1
External(\G1PT, IntObj) // PassiveThermalTripPointGen1
External(\G1CT, IntObj) // CriticalThermalTripPointGen1
External(\G1C3, IntObj) // CriticalThermalTripPointGen1S3
External(\G1HT, IntObj) // HotThermalTripPointGen1
External(\TSP1, IntObj) // ThermistorSamplingPeriodGen1

External(\GN2E, IntObj) // EnableGen2Participant
External(\G2AT, IntObj) // ActiveThermalTripPointGen2
External(\G2PT, IntObj) // PassiveThermalTripPointGen2
External(\G2CT, IntObj) // CriticalThermalTripPointGen2
External(\G2C3, IntObj) // CriticalThermalTripPointGen2S3
External(\G2HT, IntObj) // HotThermalTripPointGen2
External(\TSP2, IntObj) // ThermistorSamplingPeriodGen2

External(\GN3E, IntObj) // EnableGen3Participant
External(\G3AT, IntObj) // ActiveThermalTripPointGen3
External(\G3PT, IntObj) // PassiveThermalTripPointGen3
External(\G3CT, IntObj) // CriticalThermalTripPointGen3
External(\G3C3, IntObj) // CriticalThermalTripPointGen3S3
External(\G3HT, IntObj) // HotThermalTripPointGen3
External(\TSP3, IntObj) // ThermistorSamplingPeriodGen3

External(\GN4E, IntObj) // EnableGen4Participant
External(\G4AT, IntObj) // ActiveThermalTripPointGen4
External(\G4PT, IntObj) // PassiveThermalTripPointGen4
External(\G4CT, IntObj) // CriticalThermalTripPointGen4
External(\G4C3, IntObj) // CriticalThermalTripPointGen4S3
External(\G4HT, IntObj) // HotThermalTripPointGen4
External(\TSP4, IntObj) // ThermistorSamplingPeriodGen4

External(\GN5E, IntObj) // EnableGen5Participant
External(\G5AT, IntObj) // ActiveThermalTripPointGen5
External(\G5PT, IntObj) // PassiveThermalTripPointGen5
External(\G5CT, IntObj) // CriticalThermalTripPointGen5
External(\G5C3, IntObj) // CriticalThermalTripPointGen5S3
External(\G5HT, IntObj) // HotThermalTripPointGen5
External(\TSP5, IntObj) // ThermistorSamplingPeriodGen5

External(\GN6E, IntObj) // EnableGen6Participant
External(\G6AT, IntObj) // ActiveThermalTripPointGen6
External(\G6PT, IntObj) // PassiveThermalTripPointGen6
External(\G6CT, IntObj) // CriticalThermalTripPointGen6
External(\G6C3, IntObj) // CriticalThermalTripPointGen6S3
External(\G6HT, IntObj) // HotThermalTripPointGen6
External(\TSP6, IntObj) // ThermistorSamplingPeriodGen6

External(\GN7E, IntObj) // EnableGen7Participant
External(\G7AT, IntObj) // ActiveThermalTripPointGen7
External(\G7PT, IntObj) // PassiveThermalTripPointGen7
External(\G7CT, IntObj) // CriticalThermalTripPointGen7
External(\G7C3, IntObj) // CriticalThermalTripPointGen7S3
External(\G7HT, IntObj) // HotThermalTripPointGen7
External(\TSP7, IntObj) // ThermistorSamplingPeriodGen7

External(\GN8E, IntObj) // EnableGen8Participant
External(\G8AT, IntObj) // ActiveThermalTripPointGen8
External(\G8PT, IntObj) // PassiveThermalTripPointGen8
External(\G8CT, IntObj) // CriticalThermalTripPointGen8
External(\G8C3, IntObj) // CriticalThermalTripPointGen8S3
External(\G8HT, IntObj) // HotThermalTripPointGen8
External(\TSP8, IntObj) // ThermistorSamplingPeriodGen8

External(\CTDP, IntObj) // EnableCtdpPolicy
External(\LPMP, IntObj) // EnableLpmPolicy
External(\DPAP, IntObj) // EnableActivePolicy
External(\DPPP, IntObj) // EnablePassivePolicy
External(\DPCP, IntObj) // EnableCriticalPolicy
External(\DCMP, IntObj) // EnableCoolingModePolicy
External(\APPE, IntObj) // EnableAPPolicy
External(\PIDE, IntObj) // EnablePIDPolicy
External(\VSPE, IntObj) // EnableVSPolicy
External(\ECEU, IntObj) // EnableCurrentExecutionUnit
External(\TGFG, IntObj) // TargetGfxFreq
External(\TRTV, IntObj) // TrtRevision
External(\CHGE, IntObj) // EnableChargerParticipant
External(\PWRE, IntObj) // EnablePowerParticipant
External(\PPPR, IntObj) // PowerParticipantPollingRate
External(\PBPE, IntObj) // EnablePowerBossPolicy

External(\PERE, IntObj) // EnablePerCParticipant
External(\PEAT, IntObj) // ActiveThermalTripPointPerC
External(\PEPV, IntObj) // PassiveThermalTripPointPerC
External(\PECR, IntObj) // CriticalThermalTripPointPerC
External(\PEC3, IntObj) // CriticalThermalTripPointPerCS3
External(\PEHT, IntObj) // HotThermalTripPointPerC

External(\VSP1, IntObj) // EnableVS1Participant
External(\V1AT, IntObj) // ActiveThermalTripPointVS1
External(\V1PV, IntObj) // PassiveThermalTripPointVS1
External(\V1CR, IntObj) // CriticalThermalTripPointVS1
External(\V1C3, IntObj) // CriticalThermalTripPointVS1S3
External(\V1HT, IntObj) // HotThermalTripPointVS1

External(\VSP2, IntObj) // EnableVS2Participant
External(\V2AT, IntObj) // ActiveThermalTripPointVS2
External(\V2PV, IntObj) // PassiveThermalTripPointVS2
External(\V2CR, IntObj) // CriticalThermalTripPointVS2
External(\V2C3, IntObj) // CriticalThermalTripPointVS2S3
External(\V2HT, IntObj) // HotThermalTripPointVS2

External(\STGE, IntObj) // EnableStorageParticipant
External(\STAT, IntObj) // ActiveThermalTripPointStrg
External(\STPT, IntObj) // PassiveThermalTripPointStrg
External(\STCT, IntObj) // CriticalThermalTripPointStrg
External(\STC3, IntObj) // CriticalThermalTripPointStrgS3
External(\STHT, IntObj) // HotThermalTripPointStrg

External(\ICAE, IntObj) // EnableIvCamParticipant
External(\ICAT, IntObj) // ActiveThermalTripPointIv
External(\ICPV, IntObj) // PassiveThermalTripPointIv
External(\ICCR, IntObj) // CriticalThermalTripPointIv
External(\ICC3, IntObj) // CriticalThermalTripPointIvS3
External(\ICHT, IntObj) // HotThermalTripPointIv

External(\LPOE, IntObj) // LPOEnable
External(\LPOP, IntObj) // LPOStartPState
External(\LPOS, IntObj) // LPOStepSize
External(\LPOW, IntObj) // LPOPowerControlSetting
External(\LPER, IntObj) // LPOPerformanceControlSetting

External(\PPSZ, IntObj) // PPCC Step Size
External(\PC00, IntObj) // PR00 _PDC Flags

External(\WWSD, IntObj) // EnableWwanTempSensorDevice
External(\CVSD, IntObj) // EnableCpuVrTempSensorDevice
External(\SSDD, IntObj) // EnableSsdTempSensorDevice

External(\ODV0, IntObj) // OemDesignVariable0
External(\ODV1, IntObj) // OemDesignVariable1
External(\ODV2, IntObj) // OemDesignVariable2
External(\ODV3, IntObj) // OemDesignVariable3
External(\ODV4, IntObj) // OemDesignVariable4
External(\ODV5, IntObj) // OemDesignVariable5

External(\_TZ.ETMD, IntObj)
External(\_TZ.TZ00, ThermalZoneObj)
External(\_TZ.FAN0, DeviceObj)
External(\_TZ.FAN0._FST, PkgObj)
External(\_TZ.WBAC, PkgObj)
External(\VFN0, IntObj)
External(\VFN1, IntObj)
External(\VFN2, IntObj)
External(\VFN3, IntObj)

External(\_SB.PCI0, DeviceObj)
External(\_SB.PCI0.B0D4, DeviceObj)
External(\_SB.PCI0.MHBR, FieldUnitObj)

External(\_SB.PCI0.LPCB.H_EC, DeviceObj)
External(\_SB.PCI0.LPCB.H_EC.ECAV, IntObj)
External(\_SB.PCI0.LPCB.H_EC.ECMD, MethodObj)    // EC Command Method
External(\_SB.PCI0.LPCB.H_EC.ECRD, MethodObj)    // EC Read Method
External(\_SB.PCI0.LPCB.H_EC.ECWT, MethodObj)    // EC Write Method
External(\_SB.PCI0.LPCB.H_EC.ECF2, OpRegionObj)
External(\_SB.PCI0.LPCB.H_EC.TSR1, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSR2, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSR3, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSR4, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSR5, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PPSL, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PPSH, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PINV, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PENV, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PSTP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CMDR, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSR6, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSR7, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CFSP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CPUP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CPAP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PMAX, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PLMX, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PECH, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CFAN, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSI, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.HYST, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSHT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSLT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSSR, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TESR, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.B1RC, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.B1FC, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.BAT1._BST, MethodObj)
External(\_SB.PCI0.LPCB.H_EC.BAT1._BIX, MethodObj)
External(\_SB.PCI0.LPCB.H_EC.BMAX, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PWRT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PWRN, FieldUnitObj)  // KblGContent
External(\_SB.PCI0.LPCB.H_EC.PBSS, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.ARTG, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.PPWR, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CHGR, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TER1, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TER2, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TER3, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TER4, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TER5, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TER6, FieldUnitObj)

External(\_PR.CPPC, FieldUnitObj)
External(\_PR.PR00, ProcessorObj)
External(\_PR.PR00._PSS, MethodObj)
External(\_PR.PR00.TPSS, PkgObj)
External(\_PR.PR00.LPSS, PkgObj)
External(\_PR.PR00._PPC, MethodObj)
External(\_PR.PR00._TSS, MethodObj)
External(\_PR.CFGD, FieldUnitObj)
External(\_PR.PR00.TSMF, PkgObj)
External(\_PR.PR00.TSMC, PkgObj)
External(\_PR.PR00._PTC, MethodObj)
External(\_PR.PR00._TSD, MethodObj)
External(\_PR.PR00._TPC, IntObj)
External(\_PR.PR01, ProcessorObj)
External(\_PR.PR02, ProcessorObj)
External(\_PR.PR03, ProcessorObj)
External(\_PR.PR04, ProcessorObj)
External(\_PR.PR05, ProcessorObj)
External(\_PR.PR06, ProcessorObj)
External(\_PR.PR07, ProcessorObj)
External(\_PR.PR08, ProcessorObj)
External(\_PR.PR09, ProcessorObj)
External(\_PR.PR10, ProcessorObj)
External(\_PR.PR11, ProcessorObj)
External(\_PR.PR12, ProcessorObj)
External(\_PR.PR13, ProcessorObj)
External(\_PR.PR14, ProcessorObj)
External(\_PR.PR15, ProcessorObj)

External(\_PR.CLVL, FieldUnitObj)
External(\_PR.CBMI, FieldUnitObj)
External(\_PR.PL10, FieldUnitObj)
External(\_PR.PL20, FieldUnitObj)
External(\_PR.PLW0, FieldUnitObj)
External(\_PR.CTC0, FieldUnitObj)
External(\_PR.TAR0, FieldUnitObj)
External(\_PR.PL11, FieldUnitObj)
External(\_PR.PL21, FieldUnitObj)
External(\_PR.PLW1, FieldUnitObj)
External(\_PR.CTC1, FieldUnitObj)
External(\_PR.TAR1, FieldUnitObj)
External(\_PR.PL12, FieldUnitObj)
External(\_PR.PL22, FieldUnitObj)
External(\_PR.PLW2, FieldUnitObj)
External(\_PR.CTC2, FieldUnitObj)
External(\_PR.TAR2, FieldUnitObj)
External(\_PR.HDCE, FieldUnitObj)
External(\_PR.APSV, FieldUnitObj)
External(\_PR.AAC0, FieldUnitObj)
External(\_PR.ACRT, FieldUnitObj)
External(\_SB.PAGD, DeviceObj)
External(\_SB.PAGD._PUR, PkgObj)
External(\_SB.PAGD._STA, MethodObj)

// Display participant externals
External(\DISE, IntObj)  // EnableDisplayParticipant
External(\DPLL, IntObj)  // DisplayDepthLowerLimit
External(\DPHL, IntObj)  // DisplayDepthUpperLimit
External(\_SB.PCI0.GFX0.DD1F._BCL, MethodObj)
External(\_SB.PCI0.GFX0.DD1F._BCM, MethodObj)
External(\_SB.PCI0.GFX0.DD1F._BQC, MethodObj)
External(\_SB.PCI0.GFX0.DD1F._DCS, MethodObj)

// Platform-Wide OS Capable externals
External(\_SB.OSCP, IntObj)

// Intel Proprietary Wake up Event support externals.
External(\_SB.HIDD.HPEM, MethodObj)
External(\_SB.SLPB, DeviceObj)
External(HIDW, MethodObj)
External(HIWC, MethodObj)

Scope(\_SB)
{
  //
  // DPTF Thermal Zone Device
  //
  //
  Device(IETM)
  {
    //
    // Intel DPTF Thermal Framework Device
    //
    Name(_HID, EISAID("INT3400"))

    //-------------------------------------------
    //  Intel Proprietary Wake up Event solution
    //-------------------------------------------
    Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
    {
      If(CondRefOf(HIWC)) {
        If(HIWC(Arg0)) {
          If(CondRefOf(HIDW)) {
            Return (HIDW(Arg0, Arg1, Arg2, Arg3))
          }
        }
      }
      Return(Buffer(One) { 0x00 }) // Guid mismatch
    }


    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If (LEqual(DPTF,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    //
    // Note:
    // The number of GUID package elements in TMPP must always equal or exceed the number of
    // store statements in IDSP to prevent an overrun of the TMPP package.
    //
    Name(TMPP,Package()
    {
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000")
    })

    //
    // Save original trip points so _OSC method can enable/disable Legacy thermal policies by manipulating trip points.
    //
    Name (PTRP,0)  // Passive trip point
    Name (PSEM,0)  // Passive semaphore
    Name (ATRP,0)  // Active trip point
    Name (ASEM,0)  // Active semaphore
    Name (YTRP,0)  // Critical trip point
    Name (YSEM,0)  // Critical semaphore

    // IDSP (Intel DPTF Supported Policies)
    //
    // This object evaluates to a package of packages, with each package containing the UUID
    // values to represent a policy implemented and supported by the Intel DPTF software stack.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Package of Guid packages
    //
    Method(IDSP,0,Serialized,,PkgObj)
    {
        Name(TMPI,0)

        // Passive Policy 2.0 GUID
        If(LAnd(LEqual(\DPPP,2),CondRefOf(DP2P))){
          Store(DeRefOf(Index(DP2P,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Passive Policy 1.0 GUID
        If(LAnd(LEqual(\DPPP,1),CondRefOf(DPSP))){
          Store(DeRefOf(Index(DPSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Active Policy 1.0 GUID
        If(LAnd(LEqual(\DPAP,1),CondRefOf(DASP))){
          Store(DeRefOf(Index(DASP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Active Policy 2.0 GUID
        If(LAnd(LEqual(\DPAP,2),CondRefOf(DA2P))){
          Store(DeRefOf(Index(DA2P,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Critical Policy GUID
        If(LAnd(LEqual(\DPCP,1),CondRefOf(DCSP))){
          Store(DeRefOf(Index(DCSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Cooling Mode Policy GUID
        If(LAnd(LEqual(\DCMP,1),CondRefOf(DMSP))){
          Store(DeRefOf(Index(DMSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }
        // LPM policy GUID
        If(CondRefOf(LPSP)){ // is guid present?
          If(LAnd(LEqual(\SADE,1),LEqual(\LPMP,1))){ // is processor participant and policy enabled?
            Store(DeRefOf(Index(LPSP,0)), Index(TMPP,TMPI))
            Increment(TMPI)
          }
        }

        // CTDP Policy GUID
        If(CondRefOf(CTSP)){ // is guid present?
          If(LAnd(LEqual(\SADE,1),LEqual(\CTDP,1))){ // is processor participant and policy enabled?
            Store(DeRefOf(Index(CTSP,0)), Index(TMPP,TMPI))
            Increment(TMPI)
          }
        }

        // Power Boss Policy GUID
        If(LAnd(LEqual(\PBPE,1),CondRefOf(POBP))) { // is power boss guid present and enabled?
          Store(DeRefOf(Index(POBP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Hardware Duty Cycling Policy GUID
        If(LAnd(LEqual(\_PR.HDCE,1),CondRefOf(HDCP))){
          Store(DeRefOf(Index(HDCP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Adaptive Performance Policy GUID
        If(LAnd(LEqual(\APPE,1),CondRefOf(DAPP))){
          Store(DeRefOf(Index(DAPP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        // Virtual Sensor Policy GUID
        If(LAnd(LEqual(\VSPE,1),CondRefOf(DVSP))){
          Store(DeRefOf(Index(DVSP,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }
        // PID Policy GUID
        If(LAnd(LEqual(\PIDE,1),CondRefOf(DPID))){
          Store(DeRefOf(Index(DPID,0)), Index(TMPP,TMPI))
          Increment(TMPI)
        }

        Return(TMPP)
    }

    // _OSC (Operating System Capabilities)
    //
    // This object is evaluated by each DPTF policy implementation to communicate to the platform of the existence and/or control transfer.
    //
    // Arguments: (4)
    //   Arg0 - A Buffer containing a UUID
    //   Arg1 - An Integer containing a Revision ID of the buffer format
    //   Arg2 - An Integer containing a count of entries in Arg3
    //   Arg3 - A Buffer containing a list of DWORD capabilities
    // Return Value:
    //   A Buffer containing a list of capabilities
    //
    Method(_OSC, 4,Serialized,,BuffObj,{BuffObj,IntObj,IntObj,BuffObj})
    {
      Name(NUMP,0)
      Name (UID2,ToUUID("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"))

      // Point to Status DWORD in the Arg3 buffer (STATUS)
      CreateDWordField(Arg3, 0, STS1)

      // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
      CreateDWordField(Arg3, 4, CAP1)

      //
      // _OSC needs to validate the UUID and Revision.
      //
      // IF Unrecognized UUID
      //  Return Unrecognized UUID _OSC Failure
      // IF Unsupported Revision
      //  Return Unsupported Revision _OSC Failure
      //
      //    STS0[0] = Reserved
      //    STS0[1] = _OSC Failure
      //    STS0[2] = Unrecognized UUID
      //    STS0[3] = Unsupported Revision
      //    STS0[4] = Capabilities masked
      //
      // Get the platform UUID's that are available, this will be a package of packages.
      //
      IDSP()                     // initialize TMPP with GUID's
      Store(SizeOf(TMPP),NUMP)   // how many GUID's in the package?

      // Note:  The comparison method used is necessary due to
      // limitations of certain OSes which cannot perform direct
      // buffer comparisons.
      //
      // Create a set of "Input" UUID fields.
      //
      CreateDWordField(Arg0, 0x0, IID0)
      CreateDWordField(Arg0, 0x4, IID1)
      CreateDWordField(Arg0, 0x8, IID2)
      CreateDWordField(Arg0, 0xC, IID3)
      //
      // Create a set of "Expected" UUID fields.
      //
      CreateDWordField(UID2, 0x0, EID0)
      CreateDWordField(UID2, 0x4, EID1)
      CreateDWordField(UID2, 0x8, EID2)
      CreateDWordField(UID2, 0xC, EID3)
      //
      // Compare the input UUID to the list of UUID's in the system.
      //
      While(NUMP){
        //
        // copy one uuid from TMPP to UID2
        //
        Store(DeRefOf (Index (TMPP, Subtract(NUMP,1))),UID2)
        //
        // Verify the input UUID matches the expected UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          Break  // break out of while loop when matching UUID is found
        }
        Decrement(NUMP)
      }

      If(LEqual(NUMP,0)){
        //
        // Return Unrecognized UUID _OSC Failure
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0x6,STS1)
        Return(Arg3)
      }

      If(LNot(LEqual(Arg1, 1)))
      {
        //
        // Return Unsupported Revision _OSC Failure
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0xA,STS1)
        Return(Arg3)
      }

      If(LNot(LEqual(Arg2, 2)))
      {
        //
        // Return Argument 3 Buffer Count not sufficient
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0x2,STS1)
        Return(Arg3)
      }

      //
      // Passive Policy 2.0 GUID
      //
      If(LAnd(LEqual(\DPPP,2),CondRefOf(\_PR.APSV))){
        If(LEqual(PSEM,0)){
          Store(1,PSEM)
          Store(\_PR.APSV,PTRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DP2P)){
          Store(DeRefOf (Index (DP2P, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do passive notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\_PR.APSV)
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(PTRP,\_PR.APSV)
            }
            // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
            If(CondRefOf(\_TZ.TZ00)){
              Notify(\_TZ.TZ00, 0x81)
            }
          }
          Return(Arg3)
        }
      }

      //
      // Passive Policy 1.0 GUID
      //
      If(LAnd(LEqual(\DPPP,1),CondRefOf(\_PR.APSV))){
        If(LEqual(PSEM,0)){
          Store(1,PSEM)
          Store(\_PR.APSV,PTRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DPSP)){
          Store(DeRefOf (Index (DPSP, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do passive notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\_PR.APSV)
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(PTRP,\_PR.APSV)
            }
            // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
            If(CondRefOf(\_TZ.TZ00)){
              Notify(\_TZ.TZ00, 0x81)
            }
          }
          Return(Arg3)
        }
      }

      //
      // PID Policy GUID
      //
      If(LAnd(LEqual(\PIDE,1),CondRefOf(\_PR.APSV))){
        If(LEqual(PSEM,0)){
          Store(1,PSEM)
          Store(\_PR.APSV,PTRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DPID)){
          Store(DeRefOf (Index (DPID, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do passive notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\_PR.APSV)
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(PTRP,\_PR.APSV)
            }
            // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
            If(CondRefOf(\_TZ.TZ00)){
              Notify(\_TZ.TZ00, 0x81)
            }
          }
          Return(Arg3)
        }
      }

      //
      // Active Policy 1.0 GUID
      //
      If(LAnd(LEqual(\DPAP,1),CondRefOf(\_PR.AAC0))){
        If(LEqual(ASEM,0)){
          Store(1,ASEM)
          Store(\_PR.AAC0,ATRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DASP)){
          Store(DeRefOf (Index (DASP, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do active notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\_PR.AAC0)
              Store(0, \_TZ.ETMD)  // Legacy Active TM Management relies on this variable.
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(ATRP,\_PR.AAC0)
              Store(1, \_TZ.ETMD)
            }
            // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
            If(CondRefOf(\_TZ.TZ00)){
              Notify(\_TZ.TZ00, 0x81)
            }
          }
          Return(Arg3)
        }
      }

      //
      // Active Policy 2.0 GUID
      //
      If(LAnd(LEqual(\DPAP,2),CondRefOf(\_PR.AAC0))){
        If(LEqual(ASEM,0)){
          Store(1,ASEM)
          Store(\_PR.AAC0,ATRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DA2P)) {
          Store(DeRefOf (Index (DA2P, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do active notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\_PR.AAC0)
              Store(0, \_TZ.ETMD)  // Legacy Active TM Management relies on this variable.
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(ATRP,\_PR.AAC0)
              Store(1, \_TZ.ETMD)
            }
            // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
            If(CondRefOf(\_TZ.TZ00)){
              Notify(\_TZ.TZ00, 0x81)
            }
          }
          Return(Arg3)
        }
      }

      //
      // Critical Policy GUID
      //
      If(LAnd(LEqual(\DPCP,1),CondRefOf(\_PR.ACRT))){
        If(LEqual(YSEM,0)){
          Store(1,YSEM)
          Store(\_PR.ACRT,YTRP)  // use semaphore so variable is only initialized once
        }
        //
        // copy the GUID to UID2
        //
        If(CondRefOf(DCSP)){
          Store(DeRefOf (Index (DCSP, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If(LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))){
          // do critical notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If(And(CAP1, 0x01))
            { // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(210,\_PR.ACRT)
            } Else {  // Passive unloading, re-enable legacy thermal zone
              Store(YTRP,\_PR.ACRT)
            }
            // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
            If(CondRefOf(\_TZ.TZ00)){
              Notify(\_TZ.TZ00, 0x81)
            }
          }
          Return(Arg3)
        }
      }

      Return(Arg3)
    }
    // KTOC (Kelvin to Celsius)
    //
    // This control method converts from 10ths of degree Kelvin to Celsius.
    //
    // Arguments: (1)
    //   Arg0 - Temperature in 10ths of degree Kelvin
    // Return Value:
    //   Temperature in Celsius
    //
    Method(KTOC,1,Serialized)
    {
      If(LGreater(Arg0,2732)) // make sure we have a temperature above zero Celcius
      {
        Return(Divide(Subtract(Arg0, 2732), 10))
      } Else {
        Return(0) // negative temperatures Celcius are changed to 0 degrees Celcius
      }
    }

    // CTOK (Celsius to Kelvin)
    //
    // This control method converts from Celsius to 10ths of degree Kelvin.
    //
    // Arguments: (1)
    //   Arg0 - Temperature in Celsius
    // Return Value:
    //   Temperature in 10ths of degree Kelvin
    //
    Method(CTOK,1,Serialized)
    {
      Return(Add(Multiply(Arg0,10),2732))
    }

    Name(VERS,0)  // Version
    Name(CTYP,0)  // Mode
    Name(ALMT,0)  // Acoustic Limit
    Name(PLMT,0)  // Power Limit
    Name(WKLD,0)  // Workload Hint
    Name(DSTA,0)  // Device State Hint
    Name(RES1,0)  // Reserved 1

    // DSCP (DPTF Set Cooling Policy)
    //
    //  Arguments: (7)
    //    Arg0 - Version: For DPTF 8.0, this value is always 0.
    //    Arg1 - Mode: Integer containing the cooling mode policy code
    //    Arg2 - Acoustic Limit: Acoustic Limit value as defined in ACPI specification
    //    Arg3 - Power Limit: Power Limit value as defined in ACPI specification
    //    Arg4 - Workload Hint: Arbitrary Platform defined Integer that indicates to the platform the type of workload run in the OS.
    //    Arg5 - Device State Hint: An integer value that indicates the state of the device.
    //    Arg6 - Reserved 1
    //  Return Value:
    //    None
    //
    //  Argument Information:
    //    Mode: 0 = Active(Typically AC Power Source), 1 = Passive(Typically Battery Power Source)
    //    Acoustic Limit: Specifies the maximum acceptable acoustic level that active cooling devices may generate.
    //      Values are 1 to 5 where 1 means no acoustic tolerance and 5 means maximum acoustic tolerance.
    //    Power Limit: Specifies the maximum acceptable power level that active cooling devices may consume.
    //      Values are from 1 to 5 where 1 means no power may be used to cool and 5 means maximum power may be used to cool.
    //    Workload Hint: Refer to Intel DPTF Configuration Guide on how to set this value for various workloads for each Operating System.
    //    Device State Hint: An integer value that indicates the state of the device.
    //      First byte indicates portrait or landscape mode.
    //      Second byte indicates horizontal or vertical orientation.
    //      Third byte indicates proximity sensor status (if available).
    //      Fourth byte is unused.
    //      xxxxxx00h: Portrait
    //      xxxxxx01h: Landscape
    //      xxxx00xxh: Horizontal
    //      xxxx01xxh: Vertical
    //      xx00xxxxh: Proximity sensor Off (Device not in proximity to user)
    //      xx01xxxxh: Proximity sensor On (Device in proximity to user)
    //
    Method(DSCP, 7, Serialized)
    {
      Name(CHNG,0)  // Semaphore to record policy change

      If(LNotEqual(Arg0, 0)){
        Return() // unsupported version
      }

      If(LOr(LEqual(Arg1,0),LEqual(Arg1,1))) // bounds check
      {
        If(LNotEqual(Arg1, CTYP)){
          Store(1,CHNG)
          Store(Arg1, CTYP)
        }
      }

      If(LOr(LGreaterEqual(Arg1,0),LLessEqual(Arg1,5))) // bounds check
      {
        If(LNotEqual(Arg2, ALMT)){
          Store(1, CHNG)
          Store(Arg2, ALMT)
          Store(ALMT,\ODV0)
          Notify(\_SB.IETM, 0x88)
        }
      }

      If(LOr(LGreaterEqual(Arg1,0),LLessEqual(Arg1,5))) // bounds check
      {
        If(LNotEqual(Arg3, PLMT)){
          Store(1, CHNG)
          Store(Arg3, PLMT)
        }
      }

      If(LNotEqual(Arg4, WKLD)){
        Store(1, CHNG)
        Store(Arg4, WKLD)
      }

      If(LNotEqual(Arg5, DSTA)){
        Store(1,CHNG)
        Store(Arg5, DSTA)
      }

      If(LNotEqual(Arg6, RES1)){
        Store(1, CHNG)
        Store(Arg6, RES1)
      }

      If(CHNG){ // only notify when change occurrs
        If(LEqual(\DPPP,1)){
          Notify(\_SB.IETM, 0x83) // notify of _TRT change
        }
        If(LEqual(\DPPP,2)){
          Notify(\_SB.IETM, 0x87) // notify of PSVT change
        }
        If(LEqual(\DPAP,1)){
          Notify(\_SB.IETM, 0x84) // notify of _ART change
        }
      }
    }

    // DCFG (DPTF Configuration)
    //
    // Returns a DWORD data representing the desired behavior of DPTF besides supported DSP and participants.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the DPTF Configuration bitmap:
    //    Bit 0 = Generic UI Access Control (0 - enable as default, 1 - disable access)
    //    Bit 1 = Restricted UI Access Control ( 0 - enable as default, 1 - disable access )
    //    Bit 2 = Shell Access Control ( 0 - enable as default, 1 - disable access)
    //    Bit 3 = Environment Monitoring Report Control ( 0 - report is allowed as default, 1 - No environmental monitoring report to Microsoft )
    //    Bit 4 = Thermal Mitigation Report Control ( 0 - No mitigation report to Microsoft as default, 1 - report is allowed)
    //    Bit 5 = Thermal Policy Report Control ( 0 - No policy report to Microsoft as default, 1 - report is allowed)
    //    Bits[31:6] - Reserved (must be cleared).
    //
    Method(DCFG)
    {
      Return(\DCFE)
    }

    // ODVP (Oem Design Variables Package)
    //
    // Variables for OEM's to customize DPTF behavior based on platform changes.
    //
    Name(ODVX,Package(){0,0,0,0,0,0})

    // ODVP (Oem Design Variables Package)
    //
    // Variables for OEM's to customize DPTF behavior based on platform changes.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Package of integers
    //
    Method(ODVP,0,Serialized,,PkgObj)
    {
      Store(\ODV0,Index(ODVX,0))
      Store(\ODV1,Index(ODVX,1))
      Store(\ODV2,Index(ODVX,2))
      Store(\ODV3,Index(ODVX,3))
      Store(\ODV4,Index(ODVX,4))
      Store(\ODV5,Index(ODVX,5))
      Return(ODVX)
    }

  } // End IETM Device
} // End \_SB Scope

//
// EC support code
//
Scope(\_SB.PCI0.LPCB.H_EC) // Open scope to Embedded Controller
{
  //
  // Create a Mutex for PATx methods to prevent Sx resume race condition problems asscociated with EC commands.
  //
  Mutex(PATM, 0)

  // _QF1 (Query - Embedded Controller Query F1)
  //
  // Handler for EC generated SCI number F1.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(_QF1)
  { // Thermal sensor threshold crossing event handler
    Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSSR)), Local0)
    While(Local0) // Ensure that events occuring during execution
    {             // of this handler are not dropped
      \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.TSSR)) // clear all status bits
      If(And(Local0, 0x8))
      { // BIT3: Sensor 4 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.SEN4, 0x90)
      }
      If(And(Local0, 0x4))
      { // BIT2: Sensor 3 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.SEN3, 0x90)
      }
      If(And(Local0, 0x2))
      { // BIT1: Sensor 2 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.SEN2, 0x90)
      }
      If(And(Local0, 0x1))
      { // BIT0: Sensor 1 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.SEN1, 0x90)
      }
      Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TSSR)), Local0)
    }

    Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TESR)), Local0)
    While(Local0) // Ensure that events occuring during execution
    {             // of this handler are not dropped
      \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.TESR)) // clear all status bits
      If(And(Local0, 0x10))
      { // BIT4: Thermistor 5 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.GEN5, 0x90)
      }
      If(And(Local0, 0x8))
      { // BIT3: Thermistor 4 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.GEN4, 0x90)
      }
      If(And(Local0, 0x4))
      { // BIT2: Thermistor 3 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.GEN3, 0x90)
      }
      If(And(Local0, 0x2))
      { // BIT1: Thermistor 2 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.GEN2, 0x90)
      }
      If(And(Local0, 0x1))
      { // BIT0: Thermistor 1 Threshold Crossed
        Notify(\_SB.PCI0.LPCB.H_EC.GEN1, 0x90)
      }
      Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.TESR)), Local0)
    }
  }
} // End \_SB.PCI0.LPCB.H_EC Scope

#if 0 // AMI_OVERRIDE_START - EIP272559
//
// Processor participant.
//
  Include("B0d4Participant.asl")
  Include("B0d4CtdpPolicy.asl")
  Include("B0d4LpmPolicy.asl")

//
// Fan participant.
//
  Include("TFN1Participant.asl")

//
// Participants using device sensors.
//
  Include("DplyParticipant.asl")
  Include("ChrgParticipant.asl")
  Include("TPwrParticipant.asl")
  Include("WWANParticipant.asl")
  Include("WrlsParticipant.asl")
  Include("PerCParticipant.asl")
  Include("StrgParticipant.asl")

//
// Participants using virtual sensors.
//
  Include("Vir1Participant.asl")
  Include("Vir2Participant.asl")

//
// Participants using motherboard sensors.
//
  Include("Sen1Participant.asl")
  Include("Sen2Participant.asl")
  Include("Sen3Participant.asl")
  Include("Sen4Participant.asl")

//
// Participants using motherboard thermistors.
//
  Include("Gen1Participant.asl")
  Include("Gen2Participant.asl")
  Include("Gen3Participant.asl")
  Include("Gen4Participant.asl")
  Include("Gen5Participant.asl")

//
// IVCAM Participant
//
  Include("IVCamParticipant.asl")

//
// Policy support files
//
  Include("Etrm.asl")
  Include("Trt.asl")
  Include("Psvt.asl")
  Include("Art.asl")
  Include("Dppm.asl")
#else
//
// For OEM Includ ASL use ELINK.
//
  Include("DptfSdsIncludeFile.asl")
#endif //AMI_OVERRIDE_END - EIP272559

Scope(\_SB.IETM)
{
  // GDDV (Get Dptf Data Vault)
  //
  // The data vault can contain APCT, APAT, and PSVT tables.
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    A package containing the data vault
  //
  Method(GDDV,0,Serialized,0,PkgObj)
  {
    Return(Package()
    {
      Buffer()
      {
#if 0  // AMI_OVERRIDE_START - EIP272559
        Include("BiosDataVault.asl") // empty data vault for documentation purposes
#else
        Include("BiosDataVaultSdsFile.asl") // empty data vault for documentation purposes
#endif // AMI_OVERRIDE_END - EIP272559
      }
    })
  }
} // End Scope(\_SB.IETM)


} // End SSDT
