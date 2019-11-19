/** @file
  Acpi Gnvs Init Library.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2018 Intel Corporation.

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

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <PchAccess.h>
#include <Protocol/GlobalNvsArea.h>

/**
@brief
  Global NVS initialize.

  @param[in] GlobalNvs         - Pointer of Global NVS area

  @retval EFI_SUCCESS          - Allocate Global NVS completed.
  @retval EFI_OUT_OF_RESOURCES - Failed to allocate required page for GNVS.
**/
EFI_STATUS
EFIAPI
AcpiGnvsInit (
  IN OUT VOID                   **GlobalNvs
  )
{
  UINTN                         Pages;
  EFI_PHYSICAL_ADDRESS          Address;
  EFI_STATUS                    Status;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GNVS;

  Pages = EFI_SIZE_TO_PAGES (sizeof (EFI_GLOBAL_NVS_AREA));
  Address = 0xffffffff; // allocate address below 4G.

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   Pages,
                   &Address
                   );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  *GlobalNvs = (VOID *) (UINTN) Address;
  SetMem (*GlobalNvs, sizeof (EFI_GLOBAL_NVS_AREA), 0);

  //
  // GNVS default value init here...
  //
  GNVS = (EFI_GLOBAL_NVS_AREA_PROTOCOL *) &Address;

  //
  // Thermal trip points
  //
  GNVS->Area->Ac0TripPoint                = 71;
  GNVS->Area->Ac1TripPoint                = 55;
  GNVS->Area->Ac0FanSpeed                 = 100;
  GNVS->Area->Ac1FanSpeed                 = 75;
  GNVS->Area->PassiveThermalTripPoint     = 95;
  GNVS->Area->PassiveTc1Value             = 1;
  GNVS->Area->PassiveTc2Value             = 5;
  GNVS->Area->PassiveTspValue             = 10;
  GNVS->Area->CriticalThermalTripPoint    = 119;

  //
  // DPTF Devices and trip points
  //
  GNVS->Area->EnableDptf                   = 1;
  GNVS->Area->EnableDCFG                   = 0;
  GNVS->Area->EnableSaDevice               = 1;
  GNVS->Area->ActiveThermalTripPointSA     = 90;
  GNVS->Area->PassiveThermalTripPointSA    = 95;
  GNVS->Area->CriticalThermalTripPointSA   = 103;
  GNVS->Area->CriticalThermalTripPointSaS3 = 100;
  GNVS->Area->HotThermalTripPointSA        = 98;
  GNVS->Area->ThermalSamplingPeriodSA      = 0;
  GNVS->Area->PpccStepSize                 = 500;

  GNVS->Area->EnableChargerParticipant     = 1;

  GNVS->Area->EnablePerCParticipant          = 1;
  GNVS->Area->ActiveThermalTripPointPerC     = 63;
  GNVS->Area->PassiveThermalTripPointPerC    = 71;
  GNVS->Area->CriticalThermalTripPointPerC   = 103;
  GNVS->Area->CriticalThermalTripPointPerCS3 = 97;
  GNVS->Area->HotThermalTripPointPerC        = 100;

  GNVS->Area->EnableIvCamParticipant          = 0;
  GNVS->Area->ActiveThermalTripPointIvCam     = 63;
  GNVS->Area->PassiveThermalTripPointIvCam    = 71;
  GNVS->Area->CriticalThermalTripPointIvCam   = 103;
  GNVS->Area->CriticalThermalTripPointIvCamS3 = 97;
  GNVS->Area->HotThermalTripPointIvCam        = 100;

  GNVS->Area->EnableSen1Participant        = 1;
  GNVS->Area->ActiveThermalTripPointSen1   = 1;
  GNVS->Area->PassiveThermalTripPointSen1  = 1;
  GNVS->Area->CriticalThermalTripPointSen1 = 1;
  GNVS->Area->CriticalThermalTripPointSen1S3 = 1;
  GNVS->Area->HotThermalTripPointSen1      = 1;
  GNVS->Area->SensorSamplingPeriodSen1     = 0;

  GNVS->Area->EnableSen2Participant        = 1;
  GNVS->Area->ActiveThermalTripPointSen2   = 1;
  GNVS->Area->PassiveThermalTripPointSen2  = 1;
  GNVS->Area->CriticalThermalTripPointSen2 = 1;
  GNVS->Area->CriticalThermalTripPointSen2S3 = 1;
  GNVS->Area->HotThermalTripPointSen2      = 1;
  GNVS->Area->SensorSamplingPeriodSen2     = 0;

  GNVS->Area->EnableSen3Participant        = 1;
  GNVS->Area->ActiveThermalTripPointSen3   = 1;
  GNVS->Area->PassiveThermalTripPointSen3  = 1;
  GNVS->Area->CriticalThermalTripPointSen3 = 1;
  GNVS->Area->CriticalThermalTripPointSen3S3 = 1;
  GNVS->Area->HotThermalTripPointSen3      = 1;
  GNVS->Area->SensorSamplingPeriodSen3     = 0;

  GNVS->Area->EnableSen4Participant        = 1;
  GNVS->Area->ActiveThermalTripPointSen4   = 1;
  GNVS->Area->PassiveThermalTripPointSen4  = 1;
  GNVS->Area->CriticalThermalTripPointSen4 = 1;
  GNVS->Area->CriticalThermalTripPointSen4S3 = 1;
  GNVS->Area->HotThermalTripPointSen4      = 1;
  GNVS->Area->SensorSamplingPeriodSen4     = 0;

  GNVS->Area->EnableSen5Participant        = 1;
  GNVS->Area->ActiveThermalTripPointSen5   = 1;
  GNVS->Area->PassiveThermalTripPointSen5  = 1;
  GNVS->Area->CriticalThermalTripPointSen5 = 1;
  GNVS->Area->CriticalThermalTripPointSen5S3 = 1;
  GNVS->Area->HotThermalTripPointSen5      = 1;
  GNVS->Area->SensorSamplingPeriodSen5     = 0;

  GNVS->Area->EnableGen1Participant        = 1;
  GNVS->Area->ActiveThermalTripPointGen1   = 1;
  GNVS->Area->PassiveThermalTripPointGen1  = 1;
  GNVS->Area->CriticalThermalTripPointGen1 = 1;
  GNVS->Area->CriticalThermalTripPointGen1S3 = 1;
  GNVS->Area->HotThermalTripPointGen1      = 1;
  GNVS->Area->ThermistorSamplingPeriodGen1 = 10;

  GNVS->Area->EnableGen2Participant        = 1;
  GNVS->Area->ActiveThermalTripPointGen2   = 1;
  GNVS->Area->PassiveThermalTripPointGen2  = 1;
  GNVS->Area->CriticalThermalTripPointGen2 = 1;
  GNVS->Area->CriticalThermalTripPointGen2S3 = 1;
  GNVS->Area->HotThermalTripPointGen2      = 1;
  GNVS->Area->ThermistorSamplingPeriodGen2 = 10;

  GNVS->Area->EnableGen3Participant        = 1;
  GNVS->Area->ActiveThermalTripPointGen3   = 1;
  GNVS->Area->PassiveThermalTripPointGen3  = 1;
  GNVS->Area->CriticalThermalTripPointGen3 = 1;
  GNVS->Area->CriticalThermalTripPointGen3S3 = 1;
  GNVS->Area->HotThermalTripPointGen3      = 1;
  GNVS->Area->ThermistorSamplingPeriodGen3 = 10;

  GNVS->Area->EnableGen4Participant        = 1;
  GNVS->Area->ActiveThermalTripPointGen4   = 1;
  GNVS->Area->PassiveThermalTripPointGen4  = 1;
  GNVS->Area->CriticalThermalTripPointGen4 = 1;
  GNVS->Area->CriticalThermalTripPointGen4S3 = 1;
  GNVS->Area->HotThermalTripPointGen4      = 1;
  GNVS->Area->ThermistorSamplingPeriodGen4 = 10;

  GNVS->Area->EnableGen5Participant        = 1;
  GNVS->Area->ActiveThermalTripPointGen5   = 1;
  GNVS->Area->PassiveThermalTripPointGen5  = 1;
  GNVS->Area->CriticalThermalTripPointGen5 = 1;
  GNVS->Area->CriticalThermalTripPointGen5S3 = 1;
  GNVS->Area->HotThermalTripPointGen5      = 1;
  GNVS->Area->ThermistorSamplingPeriodGen5 = 10;

  GNVS->Area->EnablePowerParticipant       = 1;

  GNVS->Area->EnableVSPolicy               = 1;

  GNVS->Area->EnableVS1Participant          = 1;
  GNVS->Area->ActiveThermalTripPointVS1     = 80;
  GNVS->Area->PassiveThermalTripPointVS1    = 55;
  GNVS->Area->CriticalThermalTripPointVS1   = 100;
  GNVS->Area->HotThermalTripPointVS1        = 97;
  GNVS->Area->CriticalThermalTripPointVS1S3 = 95;

  GNVS->Area->EnableVS2Participant          = 1;
  GNVS->Area->ActiveThermalTripPointVS2     = 80;
  GNVS->Area->PassiveThermalTripPointVS2    = 55;
  GNVS->Area->CriticalThermalTripPointVS2   = 100;
  GNVS->Area->HotThermalTripPointVS2        = 97;
  GNVS->Area->CriticalThermalTripPointVS2S3 = 95;

  GNVS->Area->EnableStorageParticipant     = 1;
  GNVS->Area->ActiveThermalTripPointStrg   = 63;
  GNVS->Area->PassiveThermalTripPointStrg  = 71;
  GNVS->Area->CriticalThermalTripPointStrg = 103;
  GNVS->Area->CriticalThermalTripPointStrgS3 = 100;
  GNVS->Area->HotThermalTripPointStrg      = 95;


  GNVS->Area->EnableWifiParticipant        = 1;
  GNVS->Area->ActiveThermalTripPointWifi   = 63;
  GNVS->Area->PassiveThermalTripPointWifi  = 71;
  GNVS->Area->CriticalThermalTripPointWifi = 103;
  GNVS->Area->CriticalThermalTripPointWifiS3 = 100;
  GNVS->Area->HotThermalTripPointWifi      = 95;

  GNVS->Area->EnableWWANParticipant        = 1;
  GNVS->Area->ActiveThermalTripPointWWAN   = 63;
  GNVS->Area->PassiveThermalTripPointWWAN  = 71;
  GNVS->Area->CriticalThermalTripPointWWAN = 103;
  GNVS->Area->CriticalThermalTripPointWwanS3 = 100;
  GNVS->Area->HotThermalTripPointWWAN      = 95;

  GNVS->Area->EnableDisplayParticipant     = 1;
  //
  // DPTF Policies
  //
  GNVS->Area->EnableActivePolicy           = 1;
  GNVS->Area->EnablePassivePolicy          = 2;
  GNVS->Area->EnableCriticalPolicy         = 1;
  GNVS->Area->EnableCoolingModePolicy      = 1;
  GNVS->Area->TrtRevision                  = 1;
  GNVS->Area->EnableCtdpPolicy             = 1;
  GNVS->Area->EnableLpmPolicy              = 1;
  GNVS->Area->CurrentLowPowerMode          = 1;
  GNVS->Area->EnableCurrentExecutionUnit   = 1;
  GNVS->Area->TargetGfxFreq                = 400;
  GNVS->Area->EnablePowerBossPolicy        = 1;
  GNVS->Area->EnableAPPolicy               = 1;

  GNVS->Area->OemDesignVariable0           = 0;
  GNVS->Area->OemDesignVariable1           = 0;
  GNVS->Area->OemDesignVariable2           = 0;
  GNVS->Area->OemDesignVariable3           = 0;
  GNVS->Area->OemDesignVariable4           = 0;
  GNVS->Area->OemDesignVariable5           = 0;
  //
  // CLPO (Current Logical Processor Off lining Setting)
  //
  GNVS->Area->LPOEnable                    = 1;
  GNVS->Area->LPOStartPState               = 16;
  GNVS->Area->LPOStepSize                  = 1;
  GNVS->Area->LPOPowerControlSetting       = 1;
  GNVS->Area->LPOPerformanceControlSetting = 2;
  //
  // Wireless
  //
  GNVS->Area->PowerSharingManagerEnable    = 1;
  GNVS->Area->PsmSplcDomainType1           = 0x09;
  GNVS->Area->PsmSplcPowerLimit1           = 4000;
  GNVS->Area->PsmSplcTimeWindow1           = 30000;
  GNVS->Area->PsmSplcDomainType2           = 0x14;
  GNVS->Area->PsmSplcPowerLimit2           = 4000;
  GNVS->Area->PsmSplcTimeWindow2           = 30000;

  GNVS->Area->PsmDplcDomainType1           = 9;
  GNVS->Area->PsmDplcDomainPreference1     = 9;
  GNVS->Area->PsmDplcPowerLimitIndex1      = 0;
  GNVS->Area->PsmDplcDefaultPowerLimit1    = 1200;
  GNVS->Area->PsmDplcDefaultTimeWindow1    = 30000;
  GNVS->Area->PsmDplcMinimumPowerLimit1    = 1200;
  GNVS->Area->PsmDplcMaximumPowerLimit1    = 1200;
  GNVS->Area->PsmDplcMaximumTimeWindow1    = 1000;

  GNVS->Area->PsmDplcDomainType2           = 0x14;
  GNVS->Area->PsmDplcDomainPreference2     = 0x14;
  GNVS->Area->PsmDplcPowerLimitIndex2      = 0;
  GNVS->Area->PsmDplcDefaultPowerLimit2    = 1200;
  GNVS->Area->PsmDplcDefaultTimeWindow2    = 30000;
  GNVS->Area->PsmDplcMinimumPowerLimit2    = 1200;
  GNVS->Area->PsmDplcMaximumPowerLimit2    = 1200;
  GNVS->Area->PsmDplcMaximumTimeWindow2    = 1000;

  GNVS->Area->WifiEnable                   = 1;
  GNVS->Area->WifiDomainType1              = 0x7;
  GNVS->Area->WifiPowerLimit1              = 0xFFFF;
  GNVS->Area->WifiTimeWindow1              = 30000;
  GNVS->Area->WifiDomainType2              = 0x10;
  GNVS->Area->WifiPowerLimit2              = 1600;
  GNVS->Area->WifiTimeWindow2              = 30000;
  GNVS->Area->WifiDomainType3              = 0x14;
  GNVS->Area->WifiPowerLimit3              = 1050;
  GNVS->Area->WifiTimeWindow3              = 30000;
  GNVS->Area->TRxDelay0                    = 50;
  GNVS->Area->TRxCableLength0              = 50;
  GNVS->Area->TRxDelay1                    = 50;
  GNVS->Area->TRxCableLength1              = 50;
  GNVS->Area->WrddDomainType1              = 0x7;
  GNVS->Area->WrddCountryIndentifier1      = 0x4150; // "AP"
  GNVS->Area->WrddDomainType2              = 0x10;
  GNVS->Area->WrddCountryIndentifier2      = 0x4150; // "AP"
  GNVS->Area->WrdsWiFiSarEnable            = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit1     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit2     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit3     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit4     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit5     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit6     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit7     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit8     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit9     = 0x0;
  GNVS->Area->WrdsWiFiSarTxPowerLimit10    = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup1PowerMax1      = 0xFF;
  GNVS->Area->WgdsWiFiSarDeltaGroup1PowerChainA1   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup1PowerChainB1   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup1PowerMax2      = 0xFF;
  GNVS->Area->WgdsWiFiSarDeltaGroup1PowerChainA2   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup1PowerChainB2   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup2PowerMax1      = 0xFF;
  GNVS->Area->WgdsWiFiSarDeltaGroup2PowerChainA1   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup2PowerChainB1   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup2PowerMax2      = 0xFF;
  GNVS->Area->WgdsWiFiSarDeltaGroup2PowerChainA2   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup2PowerChainB2   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup3PowerMax1      = 0xFF;
  GNVS->Area->WgdsWiFiSarDeltaGroup3PowerChainA1   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup3PowerChainB1   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup3PowerMax2      = 0xFF;
  GNVS->Area->WgdsWiFiSarDeltaGroup3PowerChainA2   = 0x0;
  GNVS->Area->WgdsWiFiSarDeltaGroup3PowerChainB2   = 0x0;
  GNVS->Area->WigigRfe                     = 0x0;
  GNVS->Area->WiGigRfeCh1                  = 0x0;
  GNVS->Area->WiGigRfeCh2                  = 0x0;
  GNVS->Area->WiGigRfeCh3                  = 0x0;
  GNVS->Area->WiGigRfeCh4                  = 0x0;
  GNVS->Area->AwvClassIndex                = 0x0;
  GNVS->Area->WakeOnWiGigSupport           = 0x1;
  GNVS->Area->BluetoothSar                 = 0x0;
  GNVS->Area->BluetoothSarBr               = 0x0;
  GNVS->Area->BluetoothSarEdr2             = 0x0;
  GNVS->Area->BluetoothSarEdr3             = 0x0;
  GNVS->Area->BluetoothSarLe               = 0x0;
  GNVS->Area->BluetoothSarLe2Mhz           = 0x0;
  GNVS->Area->BluetoothSarLeLr             = 0x0;
  //
  // Miscellaneous
  //
  GNVS->Area->ConfigTdpBios = 0;
  GNVS->Area->PL1LimitCS = 0;
  GNVS->Area->PL1LimitCSValue = 4500;

  //
  // USB Type C Multi Port Support Default Setting
  //

  GNVS->Area->UsbTypeCMultiPortSupport     = 0;

//
// KblGBegin
//
  //
  // Discrete Graphics Power Share Particpants
  //
  GNVS->Area->EnableDGFCParticipant          = 1;
  GNVS->Area->ActiveThermalTripPointDGFC     = 63;
  GNVS->Area->PassiveThermalTripPointDGFC    = 89;
  GNVS->Area->CriticalThermalTripPointDGFC   = 103;
  GNVS->Area->CriticalThermalTripPointDGFCS3 = 100;
  GNVS->Area->HotThermalTripPointDGFC        = 95;
  GNVS->Area->SensorSamplingPeriodDGFC       = 50;
  GNVS->Area->PpccStepSizeDGFC               = 1000;

  GNVS->Area->EnableDGHMParticipant             = 1;
  GNVS->Area->ActiveThermalTripPointDGHM      = 63;
  GNVS->Area->PassiveThermalTripPointDGHM     = 89;
  GNVS->Area->CriticalThermalTripPointDGHM    = 103;
  GNVS->Area->CriticalThermalTripPointDGHMS3  = 100;
  GNVS->Area->HotThermalTripPointDGHM         = 95;
  GNVS->Area->SensorSamplingPeriodDGHM        = 50;

  GNVS->Area->EnableMCPParticipant             = 1;
  GNVS->Area->PpccStepSizeMCP                  = 1000;

  GNVS->Area->EnablePowerSharePolicy           = 1;
//
// KblGEnd
//
  return EFI_SUCCESS;
}

