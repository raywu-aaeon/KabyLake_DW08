/** @file
  This file is SampleCode for Intel SA PEI Policy initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2016 Intel Corporation.

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

#include "PeiSaPolicyInit.h"


/**
  PcieCardResetWorkAround performs PCIe Card reset on root port

  @param[in out] SiPreMemPolicyPpi SI_PREMEM_POLICY_PPI

  @retval EFI_SUCCESS              The policy is installed and initialized.
**/
EFI_STATUS
  PcieCardResetWorkAround (
  IN OUT   SI_PREMEM_POLICY_PPI         *SiPreMemPolicyPpi
  )
{
  EFI_STATUS                      Status;
  SA_MISC_PEI_PREMEM_CONFIG       *MiscPeiPreMemConfig;
  SWITCHABLE_GRAPHICS_CONFIG      *SgGpioData;

  Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *)&MiscPeiPreMemConfig);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gSwitchableGraphicsConfigGuid, (VOID *)&SgGpioData);
  ASSERT_EFI_ERROR(Status);

  if (SgGpioData->SaRtd3Pcie0Gpio.GpioSupport != NotSupported) {
    ///
    /// dGPU is present.
    ///      If PEG Mode or SG Muxless
    ///              Power on MXM
    ///              Configure GPIOs to drive MXM in PEG mode or SG Muxless
    ///      else
    ///              Do Nothing
    ///
    if ((MiscPeiPreMemConfig->SgMode == SgModeMuxless) ||
        (MiscPeiPreMemConfig->SgMode == SgModeDgpu)) {
      DEBUG((DEBUG_INFO, "Configure GPIOs for driving the dGPU.\n"));
      ///
      ///  Drive DGPU HOLD RST Enable to make sure we hold reset
      ///
      PcieGpioWrite (
        SgGpioData->SaRtd3Pcie0Gpio.GpioSupport,
        SgGpioData->SaRtd3Pcie0Gpio.HoldRst.ExpanderNo,
        SgGpioData->SaRtd3Pcie0Gpio.HoldRst.GpioNo,
        SgGpioData->SaRtd3Pcie0Gpio.HoldRst.Active,
        GP_ENABLE
        );
      ///
      /// wait 100ms
      ///
      MicroSecondDelay((MiscPeiPreMemConfig->SgDelayAfterHoldReset) * STALL_ONE_MILLI_SECOND);

      ///
      /// Drive DGPU PWR EN to Power On MXM
      ///
      PcieGpioWrite (
        SgGpioData->SaRtd3Pcie0Gpio.GpioSupport,
        SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.ExpanderNo,
        SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.GpioNo,
        SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.Active,
        GP_ENABLE
        );
      ///
      /// wait 300ms
      ///
      MicroSecondDelay((MiscPeiPreMemConfig->SgDelayAfterPwrEn) * STALL_ONE_MILLI_SECOND);

      ///
      /// Drive DGPU HOLD RST Disabled to remove reset
      ///
      PcieGpioWrite (
        SgGpioData->SaRtd3Pcie0Gpio.GpioSupport,
        SgGpioData->SaRtd3Pcie0Gpio.HoldRst.ExpanderNo,
        SgGpioData->SaRtd3Pcie0Gpio.HoldRst.GpioNo,
        SgGpioData->SaRtd3Pcie0Gpio.HoldRst.Active,
        GP_DISABLE
        );
      ///
      /// wait 100ms
      ///
      MicroSecondDelay((MiscPeiPreMemConfig->SgDelayAfterHoldReset) * STALL_ONE_MILLI_SECOND);
    }
  }
  return EFI_SUCCESS;
}

/**
  PCIe GPIO Write

  @param[in] GpioSupport - GPIO Support; 0=Disable, 1=PCH Based, 2=I2C Based
  @param[in] Expander    - For I2C Based=Expander No
  @param[in] Gpio        - GPIO Number
  @param[in] Active      - GPIO Active Information; High/Low
  @param[in] Level       - Write GPIO value (0/1)

**/
VOID
PcieGpioWrite (
  IN  UINT8                 GpioSupport,
  IN  UINT8                 Expander,
  IN  UINT32                Gpio,
  IN  BOOLEAN               Active,
  IN  BOOLEAN               Level
  )
{
  EFI_STATUS  Status;

  if (Active == 0) {
    Level = (~Level) & 0x1;
  }

  if (GpioSupport == PchGpio) {
    Status = GpioSetOutputValue(Gpio, (UINT32)Level);
    if (Status != EFI_SUCCESS) {
      return;
    }
  }
///
///
}
