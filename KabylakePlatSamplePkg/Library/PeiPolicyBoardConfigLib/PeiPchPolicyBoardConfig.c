/** @file
 Intel PEI PCH Policy update by board configuration

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
#include "PeiPolicyBoardConfig.h"

/**
  This function performs PEI PCH Policy update by board configuration.

  @param[in, out] SiPolicy        The SI Policy PPI instance

  @retval EFI_SUCCESS             The SI Policy is successfully updated.
  @retval Others                  The SI Policy is not successfully updated.
**/
EFI_STATUS
EFIAPI
UpdatePeiPchPolicyBoardConfig (
  IN OUT  SI_POLICY_PPI              *SiPolicy
  )
{
  EFI_STATUS                      Status;
  BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfigPreMem;
  PCH_PCIE_CONFIG                 *PcieRpConfig;
  PCH_LAN_CONFIG                  *LanConfig;
  PCH_USB_CONFIG                  *UsbConfig;
  UINTN                           Index;
  UINTN                           MaxPciePorts;
  UINTN                           MaxUsb2Ports;
  UINTN                           MaxUsb3Ports;

  DEBUG((DEBUG_INFO, "Updating PCH Policy by board config in Post Mem\n"));

  Status = GetConfigBlock ((VOID *) SiPolicy, &gPcieRpConfigGuid, (VOID *) &PcieRpConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gLanConfigGuid, (VOID *) &LanConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) SiPolicy, &gUsbConfigGuid, (VOID *) &UsbConfig);
  ASSERT_EFI_ERROR (Status);


  Status = GetBoardConfigBlock (&gPlatformBoardConfigPreMemGuid, (VOID **) &PlatformBoardConfigPreMem);
  ASSERT_EFI_ERROR (Status);

  //
  // PCI express config
  //
  MaxPciePorts = GetPchMaxPciePortNum ();

  for (Index = 0; Index < MaxPciePorts; Index ++) {
    if (PlatformBoardConfigPreMem->RootPort[Index].ClkReqSupported) {
      PcieRpConfig->RootPort[Index].ClkReqSupported = PlatformBoardConfigPreMem->RootPort[Index].ClkReqSupported;
      PcieRpConfig->RootPort[Index].ClkReqNumber = PlatformBoardConfigPreMem->RootPort[Index].ClkReqNumber;
    }
    if (PlatformBoardConfigPreMem->RootPort[Index].DeviceResetPadActiveHigh == 1) {
      PcieRpConfig->RootPort[Index].DeviceResetPad           = PlatformBoardConfigPreMem->RootPort[Index].DeviceResetPad;
      PcieRpConfig->RootPort[Index].DeviceResetPadActiveHigh = PlatformBoardConfigPreMem->RootPort[Index].DeviceResetPadActiveHigh;
    }
  }

  //
  // LAN config
  //
  if (PlatformBoardConfigPreMem->Lan.ClkReqSupported) {
    LanConfig->ClkReqSupported = PlatformBoardConfigPreMem->Lan.ClkReqSupported;
    LanConfig->ClkReqNumber = PlatformBoardConfigPreMem->Lan.ClkReqNumber;
  }

  //
  // USB Config
  //
  MaxUsb2Ports = GetPchXhciMaxUsb2PortNum ();
  MaxUsb3Ports = GetPchXhciMaxUsb3PortNum ();

  for (Index = 0; Index < MaxUsb2Ports; Index ++) {
    if (PlatformBoardConfigPreMem->UsbPort20Afe[Index].Petxiset != 0) {
      UsbConfig->PortUsb20[Index].Afe.Petxiset     = PlatformBoardConfigPreMem->UsbPort20Afe[Index].Petxiset;
      UsbConfig->PortUsb20[Index].Afe.Txiset       = PlatformBoardConfigPreMem->UsbPort20Afe[Index].Txiset;
      UsbConfig->PortUsb20[Index].Afe.Predeemp     = PlatformBoardConfigPreMem->UsbPort20Afe[Index].Predeemp;
      UsbConfig->PortUsb20[Index].Afe.Pehalfbit    = PlatformBoardConfigPreMem->UsbPort20Afe[Index].Pehalfbit;
    }
    if (PlatformBoardConfigPreMem->UsbPort20OverCurrentPin[Index] != PchUsbOverCurrentPinMax) {
      UsbConfig->PortUsb20[Index].OverCurrentPin = PlatformBoardConfigPreMem->UsbPort20OverCurrentPin[Index];
    }
  }
  for (Index = 0; Index < MaxUsb3Ports; Index ++) {
    if (PlatformBoardConfigPreMem->UsbPort30OverCurrentPin[Index] != PchUsbOverCurrentPinMax) {
      UsbConfig->PortUsb30[Index].OverCurrentPin = PlatformBoardConfigPreMem->UsbPort30OverCurrentPin[Index];
    }
  }

  return Status;
}
