/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2018 Intel Corporation.

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

#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

//
// Includes
//
#define GLOBAL_NVS_DEVICE_ENABLE 1
#define GLOBAL_NVS_DEVICE_DISABLE 0

//
// Forward reference for pure ANSI compatibility
//

typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL EFI_GLOBAL_NVS_AREA_PROTOCOL;

//
// Global NVS Area Protocol GUID
//
#define EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID \
{ 0x74e1e48, 0x8132, 0x47a1, 0x8c, 0x2c, 0x3f, 0x14, 0xad, 0x9a, 0x66, 0xdc }

/**
 Making any EFI_GLOBAL_NVS_AREA structure change after code frozen
 will need to maintain backward compatibility, bump up
 structure revision and update below history table\n
  <b>Revision 1</b>:   - Initial version.\n
  <b>Revision 2</b>:   - Add UCSI/UcmcPort1Gpio/UcmcPort2Gpio for UCSI/UCMC device.\n
  <b>Revision 3</b>:   - Add PcdWwanGnssUartResetGpio for Rtd3 Wwan Gnss Uart Reset GPIO.\n
  <b>Revision 4</b>:   - Deprecate CriticalThermalTripPointWGigS.\n
  <b>Revision 5</b>:   - WiGig Regulatory Update: add WigigRfe/WiGigRfeCh1/WiGigRfeCh2/WiGigRfeCh3/WiGigRfeCh4/AwvClassIndex.\n
  <b>Revision 6</b>:   - Wireless Participant, Add ThermalSamplingPeriodWifi for TSP setup option.\n
  <b>Revision 7</b>:   - Add WakeOnWiGigSupport for Wake on S3-S4 WiGig Docking Support.\n
  <b>Revision 8</b>:   - Add EnablePIDPolicy for DPTF.\n
  <b>Revision 9</b>:   - Add SkyCam related items to support Privacy LED and Shared voltage rails enablement for IPU.\n
  <b>Revision 10</b>:  - Add PcdWLanRootPort/PcdSsdRootPort/PcdWigigSupported for KBL-KC platform.\n
  <b>Revision 11</b>:  - Add UsbTypeCMultiPortSupport (UTCM) for [Type-C KBL-R] USB UCSI connector manager driver is not enumerating in device manager..\n
  <b>Revision 12</b>:  - Add TbtUsbSplitMode/SplitModePchUsb2TbtPort1/SplitModePchUsb2TbtPort2 for Type-C Split mode support on KBL-R \n
  <b>Revision 13</b>:  - Add RS2 Native OS Support for Thunderbolt \n
  <b>Revision 14</b>:  - Add Workaround for Titan Ridge OSUP flow \n
  <b>Revision 15</b>:  - Add PowerToModemResetDelay and PowerToPeResetDelay for Modem (7360) Enumeration. \n
  <b>Revision 16</b>:  - Deprecate DPTF policy: EnableLpmPolicy, CurrentLowPowerMode, EnableCurrentExecutionUnit, TargetGfxFreq and EnableCoolingModePolicy.\n
  <b>Revision 17</b>:  - Add PcdPcieWwanEnable for Modem (7360) WA.\n
  <b>Revision 18</b>:  - Deprecate PcdWLanRootPort, PcdSsdRootPort, PcdWigigSupported\n
  <b>Revision 19</b>:  - Deprecate PcdPcieWwanEnable since the Modem (7360) WA won't be used anymore.\n
  <b>Revision 20</b>:  - Add EcLowPowerModeGpioPin, EcSmiGpioPin to handle EC Low Power Mode. \n
  <b>Revision 21</b>:  - Add UcsiRetryEC, BIOS will retry to write parameters before EC goes out of burst mode. \n
  <b>Revision 22</b>:  - Use PcdPcieWwanEnable.\n
  <b>Revision 23</b>:  - Deprecate PcdPcieWwanEnable.\n
  <b>Revision 24</b>:  - En/Dis CSME, Bit31 PEP Constraints.\n
  <b>Revision 25</b>:  - Deprecate TbtNativeOsHotPlug, Add Rtd3TbtOffDelay,TbtSxWakeSwitchLogicEnable.\n
  <b>Revision 26</b>:  - Add PcdVsctTableForSens,RunTimeVmVrGpio.\n
  <b>Revision 27</b>:  - Deprecate Port80DebugValue.\n
  <b>Revision 28</b>:  - Add TbtEnable for TBT Enable/Disable.\n
  <b>Revision 29</b>:  - Add Rtd3WwanDsm for WWAN D3/L3 Enable/Disable
  <b>Revision 30</b>:  - Add Power Meter Support for AmberLake.\n
  <b>Revision 31</b>:  - Add Time Alaram Device support.\n
  <b>Revision 32</b>:  - Add S3 wake capability from LID Switch Through GPE0 Number.\n
  <b>Revision 33</b>:  - Add PepCio2 Support. Adding PEP1 New PEP Constraint Device List.\n
**/
#define GLOBAL_NVS_AREA_REVISION       33
//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

//
// Global NVS Area definition
//
#include "GloblNvsArea_Def.h"

//
// Global NVS Area Protocol
//
typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL {
  EFI_GLOBAL_NVS_AREA     *Area;
} EFI_GLOBAL_NVS_AREA_PROTOCOL;

#endif
