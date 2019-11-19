/** @file
  Others Board's PCD function hook.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 Intel Corporation.

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

#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/GpioNativeLib.h>
#include <Library/GpioLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <Library/GpioExpanderLib.h>
#include <SetupVariable.h>

/**
  SDS Board Specific Init function hook for PEI post memory phase.

  @param  PlatformInfo  pointer to the PLATFORM_INFO data structure.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
SdsPeiBoardSpecificInitPostMem (
  VOID
  )
{
  EFI_STATUS                      Status;
  SETUP_DATA                      SystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VarSize;

  //
  // Locate Setup variables
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SystemConfiguration.TouchPanelMuxSelector == 0) {
    // If we want to use Integrated (SPI) Touch (0) instead of Default I2C Touch (1)
    //  * Configure GPP_E_11 Output to Low
    GpioSetOutputValue (GPIO_SKL_LP_GPP_E11, V_PCH_GPIO_TX_STATE_LOW);
  }
  if (SystemConfiguration.BtUartMuxSelector == 0) {
    // If we want to select UART Signals (0) to go to the M.2 Connector instead of Default BT Signals (1)
    //  * Configure GPP_G_6 Output to Low
    GpioSetOutputValue (GPIO_SKL_LP_GPP_G6, V_PCH_GPIO_TX_STATE_LOW);
  }
  if (SystemConfiguration.WWANEnable == 0) {
    // Disabling WWAN
    //  * Configure GPP_A_12(Reset) Output to Low
    //  * Configure GPP_A_16(Power FET) Output to Low
    GpioSetOutputValue (GPIO_SKL_LP_GPP_A12, V_PCH_GPIO_TX_STATE_LOW);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_A16, V_PCH_GPIO_TX_STATE_LOW);
  }
  // Toggle EInk DFU GPIO from HIGH to LOW to kick-off time-delay circuit that will initiate Power Cycle
  // and ensure EInk Device boot into a well-known state. EInk DFU GPIO needs to be kept LOW for at least
  // 250ms, it is being pulled back to HIGH in AcpiPlatform.c to comply with time request
  GpioSetOutputValue (GPIO_SKL_LP_GPP_A21, V_PCH_GPIO_TX_STATE_LOW);

  return EFI_SUCCESS;
}

EFI_STATUS
PeiBoardSpecificInitPostMemNull (
  VOID
  )
{
  return EFI_SUCCESS;
}

