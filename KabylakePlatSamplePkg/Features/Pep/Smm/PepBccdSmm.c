/** @file
  SW SMI Handler for enabling PEP Bugcheck Critical Device.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

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

#include "PepBccdSmm.h"
#include <PchAccess.h>
#include <Library/MmPciLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/GpioLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <Library/BoardConfigLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                  mSdsSpecificInit;
GLOBAL_REMOVE_IF_UNREFERENCED SETUP_DATA               mSystemConfiguration;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_EC_ACCESS_PROTOCOL   *mEcAccess;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_SYSTEM_TABLE2    *mSmst;

#define EFI_PEP_BCCD_SW_SMI                 0xCD

//
// Board specific power gate GPIO for storage device
//
// #define <BOARD>_GPIO_<SIGNAL_NAME>       <GPIO Pin>
//
#define SDS_M2_SSD_DETECT                 GPIO_SKL_LP_GPP_E1
#define SDS_M2_SSD_RST                    GPIO_SKL_LP_GPP_G2
#define SDS_M2_SSD_PWR                    GPIO_SKL_LP_GPP_G3


VOID
PepBccdCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     CommBufferSize
  )
{
  UINT32      InputVal;
  UINTN       RootDeviceBase;
  UINT8       Data8;

  if (mSdsSpecificInit) {
      GpioGetInputValue (SDS_M2_SSD_DETECT, &InputVal);
      if (InputVal == V_PCH_GPIO_TX_STATE_HIGH) {    // SATA SSD
        GpioSetOutputValue (SDS_M2_SSD_PWR, V_PCH_GPIO_TX_STATE_HIGH);
      } else {                                       // PCIe SSD
        GpioSetOutputValue (SDS_M2_SSD_PWR, V_PCH_GPIO_TX_STATE_HIGH);
        MicroSecondDelay (mSystemConfiguration.Rtd3P0dl * 1000); // Delay in ms
        GpioSetOutputValue (SDS_M2_SSD_RST, V_PCH_GPIO_TX_STATE_HIGH);
        // Set L23_Ready to Detect Transition.
        RootDeviceBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5);
        Data8 = MmioRead8(RootDeviceBase + R_PCH_PCIE_RPPGEN); // RPGEN
        Data8 |= BIT3;
        MmioWrite8((RootDeviceBase + R_PCH_PCIE_RPPGEN), Data8); //Store 1 at L23R
        MicroSecondDelay (16 * 1000); // Delay in ms
        // Wait up to 12 ms for transition to detect
        Data8 = 0;
        while (((MmioRead16(RootDeviceBase + R_PCH_PCIE_RPPGEN)) & BIT3)) {
          if (Data8 > 4) {
            break;
          }
          MicroSecondDelay(3 * 1000); // Delay in ms
          Data8++;
        }
        // Once in Detect, wait up to 124 ms for Link Active (typically happens in under 70ms)
        Data8 = 0;
        while (!((MmioRead16(RootDeviceBase + 0x52)) & BIT13)) {
          if (Data8 > 123) {
            break;
          }
          MicroSecondDelay(1 * 1000); // Delay in ms
          Data8++;
        }
        // Set PCIEDBG.DMIL1EDM (324[3]) = 0
        Data8 = MmioRead8(RootDeviceBase + 0x324);
        Data8 &= ~BIT3;
        MmioWrite8((RootDeviceBase +  0x324), Data8);
      }
  }
      //
      // Do nothing for other board that does not support RTD3 storage device power gating
      //
}


/**
  Register SW SMI Handler for PEP BugCheck Critical Device.

  @retval  EFI_SUCCESS      SW SMI Handler is registered successfully.
  @retval  EFI_UNSUPPORTED  SW SMI Handler is not registered because RTD3 is not supported.
**/
EFI_STATUS
RegisterPepBccdSwSmi (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     SwHandle;
  EFI_SMM_SW_REGISTER_CONTEXT    SwContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;

  //
  // Return EFI_UNSUPPORTED if RTD3 is disabled
  //
  if (mSystemConfiguration.Rtd3Support == 0) {
    return EFI_UNSUPPORTED;
  }

  //
  //  Locate the ICH SMM SW dispatch protocol
  //
  Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch);
  ASSERT_EFI_ERROR (Status);

  //
  // Register PEP Bugcheck Critical Device SW SMI handler
  //
  SwContext.SwSmiInputValue = EFI_PEP_BCCD_SW_SMI;

  Status = SwDispatch->Register (SwDispatch,
                                 (EFI_SMM_HANDLER_ENTRY_POINT2)PepBccdCallback,
                                 &SwContext,
                                 &SwHandle);
  ASSERT_EFI_ERROR (Status);

  return Status;
}



/**
  Initializes the SMM PEP BCCD Driver

  @param[in] ImageHandle    Pointer to the loaded image protocol for this driver
  @param[in] SystemTable    Pointer to the EFI System Table

  @retval    EFI_SUCCESS    Assert, otherwise.
**/
EFI_STATUS
EFIAPI
InitializePepBccdSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  UINTN                                     DataSize;

  //
  // Get Platform Board Config Block.
  //
  mSdsSpecificInit = PcdGetBool (PcdSdsPepBccdSmmCallbackSupport);

  //
  // Initialize global variables
  //
  mSmst = gSmst;

  //
  // Locate EC Access Protocol
  //
  Status = mSmst->SmmLocateProtocol(&gEfiEcAccessProtocolGuid, NULL, (VOID **) &mEcAccess);

  //
  // Locate setup variable
  //
  DataSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gSetupVariableGuid,
                  NULL,
                  &DataSize,
                  &mSystemConfiguration
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register PEP Bugcheck Critical Device SW SMI handler
  //
  Status = RegisterPepBccdSwSmi();
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "PEP BCCD SW SMI Handler is not installed\n"));
  }

  return EFI_SUCCESS;
}
