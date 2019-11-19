/** @file
  This file contains PEI PreMem board config block initial functions.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiPlatformHookLib.h>
#include <PlatformInfo.h>
#include <PlatformBoardId.h>
#include <Board/BoardHook.h>
#include <PlatformBoardConfig.h>

#ifndef MINTREE_FLAG
EFI_STATUS
InitPlatformBoardConfigFunction (
  IN PLATFORM_INFO *PlatformInfo
);
#endif

GLOBAL_REMOVE_IF_UNREFERENCED BOARD_CONFIG_BLOCK mSampleBoardConfigPeiPreMem = {
  {{{0, ALIGN16(sizeof(BOARD_CONFIG_BLOCK_PEI_PREMEM)), 0},
    {0x44afdd70, 0xa7b1, 0x406f, {0x87, 0x8d, 0x8b, 0x4e, 0xb8, 0x1b, 0x57, 0xdf}}},
    BOARD_CONFIG_BLOCK_PEI_PREMEM_VERSION, 0, {0, 0}}
  };

/**
  Init SA Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.
  @param[in]  ConfigBlock       a pointer to PEI PreMem structure.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
InitSaPlatformConfigBlockPreMem (
  IN PLATFORM_INFO *PlatformInfo,
  IN CONFIG_BLOCK *ConfigBlock
)
{
  BOARD_CONFIG_BLOCK_PEI_PREMEM *PeiPreMem;
  VOID *Buffer;
  UINTN Size;

  PeiPreMem = (BOARD_CONFIG_BLOCK_PEI_PREMEM *) ConfigBlock;

  if (PcdGet8 (PcdSaMiscUserBd) != 0) {
    PeiPreMem->UserBd = PcdGet8 (PcdSaMiscUserBd);
    PeiPreMem->MmioSizeAdjustment = PcdGet16 (PcdSaMiscMmioSizeAdjustment);
  }

  PeiPreMem->MobilePlatform = PcdGetBool (PcdMobileDramPresent);

  if (PcdGet8 (PcdMrcSpdAddressTable0) != 0) {
    PeiPreMem->SpdAddressTable[0] = PcdGet8 (PcdMrcSpdAddressTable0);
    PeiPreMem->SpdAddressTable[1] = PcdGet8 (PcdMrcSpdAddressTable1);
    PeiPreMem->SpdAddressTable[2] = PcdGet8 (PcdMrcSpdAddressTable2);
    PeiPreMem->SpdAddressTable[3] = PcdGet8 (PcdMrcSpdAddressTable3);
  }

  //
  // CA Vref routing: board-dependent
  // 0 - VREF_CA goes to both CH_A and CH_B (LPDDR3/DDR3L)
  // 1 - VREF_CA to CH_A, VREF_DQ_A to CH_B (should not be used)
  // 2 - VREF_CA to CH_A, VREF_DQ_B to CH_B (DDR4)
  //
  PeiPreMem->CaVrefConfig = PcdGet8 (PcdMrcCaVrefConfig);

  //
  // Spd Data
  //
  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcRcompResistor);
  Size = sizeof (PeiPreMem->RcompResistor);
  if (Buffer) {
    PeiPreMem->Enable.RcompResistor = TRUE;
    CopyMem (PeiPreMem->RcompResistor, Buffer, Size);
  }


  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcRcompTarget);
  Size = sizeof (PeiPreMem->RcompTarget);
  if (Buffer) {
    PeiPreMem->Enable.RcompTarget = TRUE;
    CopyMem (PeiPreMem->RcompTarget, Buffer, Size);
  }

  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcDqByteMap);
  Size = PcdGet16 (PcdMrcDqByteMapSize);
  if (Buffer) {
    PeiPreMem->Enable.DqByteMap = TRUE;
    CopyMem (PeiPreMem->DqByteMap, Buffer, Size);
  }


  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcDqsMapCpu2Dram);
  Size = PcdGet16 (PcdMrcDqsMapCpu2DramSize);
  if (Buffer) {
    PeiPreMem->Enable.DqsMapCpu2Dram = TRUE;
    CopyMem (PeiPreMem->DqsMapCpu2Dram, Buffer, Size);
  }


#ifdef MEM_DOWN_FLAG
  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcSpdData);
  Size = PcdGet16 (PcdMrcSpdDataSize);
  if (Buffer) {
    PeiPreMem->Enable.SpdData = TRUE;
    CopyMem(PeiPreMem->SpdData, Buffer, Size);
  }


#endif

  PeiPreMem->Enable.DqPinsInterleaved = PcdGetBool (PcdMrcDqPinsInterleavedControl);
  PeiPreMem->DqPinsInterleaved = PcdGetBool (PcdMrcDqPinsInterleaved);


  //
  // Update board's GPIO for PEG slot reset
  //
  PeiPreMem->Enable.GpioSupport = PcdGetBool (PcdPegGpioResetControl);
  PeiPreMem->GpioSupport = PcdGetBool (PcdPegGpioResetSupoort);
  PeiPreMem->GpioPad = PcdGet32 (PcdPegResetGpioPad);
  PeiPreMem->Active = PcdGetBool (PcdPegResetGpioActive);
#ifndef MINTREE_FLAG
  //
  // Initialize the GPIO Configuration
  //
  PeiPreMem->RootPortDev = PcdGet8 (PcdRootPortDev);
  PeiPreMem->RootPortFun = PcdGet8 (PcdRootPortFunc);

  PeiPreMem->SaRtd3Pcie0Gpio.GpioSupport =         PcdGet8  (PcdPcie0GpioSupport);
  PeiPreMem->SaRtd3Pcie0Gpio.WakeGpioNo =          PcdGet32 (PcdPcie0WakeGpioNo);
  PeiPreMem->SaRtd3Pcie0Gpio.HoldRstExpanderNo =   PcdGet8  (PcdPcie0HoldRstExpanderNo);
  PeiPreMem->SaRtd3Pcie0Gpio.HoldRstGpioNo =       PcdGet32 (PcdPcie0HoldRstGpioNo);
  PeiPreMem->SaRtd3Pcie0Gpio.HoldRstActive =       PcdGetBool (PcdPcie0HoldRstActive);
  PeiPreMem->SaRtd3Pcie0Gpio.PwrEnableExpanderNo = PcdGet8  (PcdPcie0PwrEnableExpanderNo);
  PeiPreMem->SaRtd3Pcie0Gpio.PwrEnableGpioNo =     PcdGet32 (PcdPcie0PwrEnableGpioNo);
  PeiPreMem->SaRtd3Pcie0Gpio.PwrEnableActive =     PcdGetBool (PcdPcie0PwrEnableActive);
  PeiPreMem->SaRtd3Pcie0ClkRecNumber =             PcdGet8  (PcdPcie0ClkReqNumber);

  PeiPreMem->SaRtd3Pcie1Gpio.GpioSupport =         PcdGet8  (PcdPcie1GpioSupport);
  PeiPreMem->SaRtd3Pcie1Gpio.WakeGpioNo =          PcdGet32 (PcdPcie1WakeGpioNo);
  PeiPreMem->SaRtd3Pcie1Gpio.HoldRstExpanderNo =   PcdGet8  (PcdPcie1HoldRstExpanderNo);
  PeiPreMem->SaRtd3Pcie1Gpio.HoldRstGpioNo =       PcdGet32 (PcdPcie1HoldRstGpioNo);
  PeiPreMem->SaRtd3Pcie1Gpio.HoldRstActive =       PcdGetBool (PcdPcie1HoldRstActive);
  PeiPreMem->SaRtd3Pcie1Gpio.PwrEnableExpanderNo = PcdGet8  (PcdPcie1PwrEnableExpanderNo);
  PeiPreMem->SaRtd3Pcie1Gpio.PwrEnableGpioNo =     PcdGet32 (PcdPcie1PwrEnableGpioNo);
  PeiPreMem->SaRtd3Pcie1Gpio.PwrEnableActive =     PcdGetBool (PcdPcie1PwrEnableActive);
  PeiPreMem->SaRtd3Pcie1ClkRecNumber =             PcdGet8  (PcdPcie1ClkReqNumber);

  PeiPreMem->SaRtd3Pcie2Gpio.GpioSupport =         PcdGet8  (PcdPcie2GpioSupport);
  PeiPreMem->SaRtd3Pcie2Gpio.WakeGpioNo =          PcdGet32 (PcdPcie2WakeGpioNo);
  PeiPreMem->SaRtd3Pcie2Gpio.HoldRstExpanderNo =   PcdGet8  (PcdPcie2HoldRstExpanderNo);
  PeiPreMem->SaRtd3Pcie2Gpio.HoldRstGpioNo =       PcdGet32 (PcdPcie2HoldRstGpioNo);
  PeiPreMem->SaRtd3Pcie2Gpio.HoldRstActive =       PcdGetBool (PcdPcie2HoldRstActive);
  PeiPreMem->SaRtd3Pcie2Gpio.PwrEnableExpanderNo = PcdGet8  (PcdPcie2PwrEnableExpanderNo);
  PeiPreMem->SaRtd3Pcie2Gpio.PwrEnableGpioNo =     PcdGet32 (PcdPcie2PwrEnableGpioNo);
  PeiPreMem->SaRtd3Pcie2Gpio.PwrEnableActive =     PcdGetBool (PcdPcie2PwrEnableActive);
  PeiPreMem->SaRtd3Pcie2ClkRecNumber =             PcdGet8  (PcdPcie2ClkReqNumber);
#endif

  return EFI_SUCCESS;
}

/**
  Init PCH Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.
  @param[in]  ConfigBlock       a pointer to PEI PreMem structure.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
InitPchPlatformConfigBlockPreMem (
  IN PLATFORM_INFO *PlatformInfo,
  IN CONFIG_BLOCK *ConfigBlock
)
{
  BOARD_CONFIG_BLOCK_PEI_PREMEM *PeiPreMem;
  PCD64_BLOB Pcd64;
  PCD32_BLOB Pcd32;

  PeiPreMem = (BOARD_CONFIG_BLOCK_PEI_PREMEM *) ConfigBlock;

  PeiPreMem->GpioTablePreMem = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTablePreMem);
  PeiPreMem->GpioTablePreMemSize = PcdGet16 (PcdBoardGpioTablePreMemSize);


  //
  // Assign ClkReq signal to root port. (Base 0)
  // For LP, Set 0 - 5
  // For H,  Set 0 - 15
  // Note that if GbE is enabled, ClkReq assigned to GbE will not be available for Root Port.
  //

  Pcd64.Blob = PcdGet64 (PcdRootPort0ClkInfo);  PeiPreMem->RootPort[ 0] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort1ClkInfo);  PeiPreMem->RootPort[ 1] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort2ClkInfo);  PeiPreMem->RootPort[ 2] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort3ClkInfo);  PeiPreMem->RootPort[ 3] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort4ClkInfo);  PeiPreMem->RootPort[ 4] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort5ClkInfo);  PeiPreMem->RootPort[ 5] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort6ClkInfo);  PeiPreMem->RootPort[ 6] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort7ClkInfo);  PeiPreMem->RootPort[ 7] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort8ClkInfo);  PeiPreMem->RootPort[ 8] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort9ClkInfo);  PeiPreMem->RootPort[ 9] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort10ClkInfo); PeiPreMem->RootPort[10] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort11ClkInfo); PeiPreMem->RootPort[11] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort12ClkInfo); PeiPreMem->RootPort[12] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort13ClkInfo); PeiPreMem->RootPort[13] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort14ClkInfo); PeiPreMem->RootPort[14] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort15ClkInfo); PeiPreMem->RootPort[15] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort16ClkInfo); PeiPreMem->RootPort[16] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort17ClkInfo); PeiPreMem->RootPort[17] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort18ClkInfo); PeiPreMem->RootPort[18] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort19ClkInfo); PeiPreMem->RootPort[19] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort20ClkInfo); PeiPreMem->RootPort[20] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort21ClkInfo); PeiPreMem->RootPort[21] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort22ClkInfo); PeiPreMem->RootPort[22] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort23ClkInfo); PeiPreMem->RootPort[23] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort24ClkInfo); PeiPreMem->RootPort[24] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort25ClkInfo); PeiPreMem->RootPort[25] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort26ClkInfo); PeiPreMem->RootPort[26] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort27ClkInfo); PeiPreMem->RootPort[27] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort28ClkInfo); PeiPreMem->RootPort[28] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort29ClkInfo); PeiPreMem->RootPort[29] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort30ClkInfo); PeiPreMem->RootPort[30] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPort31ClkInfo); PeiPreMem->RootPort[31] = Pcd64.Info;
  Pcd64.Blob = PcdGet64 (PcdRootPortLanClkInfo); PeiPreMem->Lan = Pcd64.Info;

  //
  // USB2 AFE settings.
  //
  Pcd32.Blob = PcdGet32 (PcdUsb20Port0Afe);  PeiPreMem->UsbPort20Afe[ 0] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port1Afe);  PeiPreMem->UsbPort20Afe[ 1] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port2Afe);  PeiPreMem->UsbPort20Afe[ 2] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port3Afe);  PeiPreMem->UsbPort20Afe[ 3] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port4Afe);  PeiPreMem->UsbPort20Afe[ 4] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port5Afe);  PeiPreMem->UsbPort20Afe[ 5] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port6Afe);  PeiPreMem->UsbPort20Afe[ 6] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port7Afe);  PeiPreMem->UsbPort20Afe[ 7] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port8Afe);  PeiPreMem->UsbPort20Afe[ 8] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port9Afe);  PeiPreMem->UsbPort20Afe[ 9] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port10Afe); PeiPreMem->UsbPort20Afe[10] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port11Afe); PeiPreMem->UsbPort20Afe[11] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port12Afe); PeiPreMem->UsbPort20Afe[12] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port13Afe); PeiPreMem->UsbPort20Afe[13] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port14Afe); PeiPreMem->UsbPort20Afe[14] = Pcd32.Info;
  Pcd32.Blob = PcdGet32 (PcdUsb20Port15Afe); PeiPreMem->UsbPort20Afe[15] = Pcd32.Info;

  //
  // Platform Board programming per the layout of each port.
  //
  PeiPreMem->UsbPort20OverCurrentPin[ 0] = PcdGet8 (PcdUsb20OverCurrentPinPort0);
  PeiPreMem->UsbPort20OverCurrentPin[ 1] = PcdGet8 (PcdUsb20OverCurrentPinPort1);
  PeiPreMem->UsbPort20OverCurrentPin[ 2] = PcdGet8 (PcdUsb20OverCurrentPinPort2);
  PeiPreMem->UsbPort20OverCurrentPin[ 3] = PcdGet8 (PcdUsb20OverCurrentPinPort3);
  PeiPreMem->UsbPort20OverCurrentPin[ 4] = PcdGet8 (PcdUsb20OverCurrentPinPort4);
  PeiPreMem->UsbPort20OverCurrentPin[ 5] = PcdGet8 (PcdUsb20OverCurrentPinPort5);
  PeiPreMem->UsbPort20OverCurrentPin[ 6] = PcdGet8 (PcdUsb20OverCurrentPinPort6);
  PeiPreMem->UsbPort20OverCurrentPin[ 7] = PcdGet8 (PcdUsb20OverCurrentPinPort7);
  PeiPreMem->UsbPort20OverCurrentPin[ 8] = PcdGet8 (PcdUsb20OverCurrentPinPort8);
  PeiPreMem->UsbPort20OverCurrentPin[ 9] = PcdGet8 (PcdUsb20OverCurrentPinPort9);
  PeiPreMem->UsbPort20OverCurrentPin[10] = PcdGet8 (PcdUsb20OverCurrentPinPort10);
  PeiPreMem->UsbPort20OverCurrentPin[11] = PcdGet8 (PcdUsb20OverCurrentPinPort11);
  PeiPreMem->UsbPort20OverCurrentPin[12] = PcdGet8 (PcdUsb20OverCurrentPinPort12);
  PeiPreMem->UsbPort20OverCurrentPin[13] = PcdGet8 (PcdUsb20OverCurrentPinPort13);
  PeiPreMem->UsbPort20OverCurrentPin[14] = PcdGet8 (PcdUsb20OverCurrentPinPort14);
  PeiPreMem->UsbPort20OverCurrentPin[15] = PcdGet8 (PcdUsb20OverCurrentPinPort15);

  PeiPreMem->UsbPort30OverCurrentPin[0]  = PcdGet8 (PcdUsb30OverCurrentPinPort0);
  PeiPreMem->UsbPort30OverCurrentPin[1]  = PcdGet8 (PcdUsb30OverCurrentPinPort1);
  PeiPreMem->UsbPort30OverCurrentPin[2]  = PcdGet8 (PcdUsb30OverCurrentPinPort2);
  PeiPreMem->UsbPort30OverCurrentPin[3]  = PcdGet8 (PcdUsb30OverCurrentPinPort3);
  PeiPreMem->UsbPort30OverCurrentPin[4]  = PcdGet8 (PcdUsb30OverCurrentPinPort4);
  PeiPreMem->UsbPort30OverCurrentPin[5]  = PcdGet8 (PcdUsb30OverCurrentPinPort5);
  PeiPreMem->UsbPort30OverCurrentPin[6]  = PcdGet8 (PcdUsb30OverCurrentPinPort6);
  PeiPreMem->UsbPort30OverCurrentPin[7]  = PcdGet8 (PcdUsb30OverCurrentPinPort7);
  PeiPreMem->UsbPort30OverCurrentPin[8]  = PcdGet8 (PcdUsb30OverCurrentPinPort8);
  PeiPreMem->UsbPort30OverCurrentPin[9]  = PcdGet8 (PcdUsb30OverCurrentPinPort9);

  //
  // PCH-H HSIO PTSS Table Init
  //
  PeiPreMem->UnknowHHsioPtssTable [0] = (VOID *) (UINTN) PcdGet32 (PcdUnknowHHsioPtssTable1);
  PeiPreMem->UnknowHHsioPtssTable [1] = (VOID *) (UINTN) PcdGet32 (PcdUnknowHHsioPtssTable2);
  PeiPreMem->UnknowHHsioPtssTableSize [0] = PcdGet16 (PcdUnknowHHsioPtssTable1Size);
  PeiPreMem->UnknowHHsioPtssTableSize [1] = PcdGet16 (PcdUnknowHHsioPtssTable2Size);

  PeiPreMem->SpecificHHsioPtssTable [0] = (VOID *) (UINTN) PcdGet32 (PcdSpecificHHsioPtssTable1);
  PeiPreMem->SpecificHHsioPtssTable [1] = (VOID *) (UINTN) PcdGet32 (PcdSpecificHHsioPtssTable2);
  PeiPreMem->SpecificHHsioPtssTableSize [0] = PcdGet16 (PcdSpecificHHsioPtssTable1Size);
  PeiPreMem->SpecificHHsioPtssTableSize [1] = PcdGet16 (PcdSpecificHHsioPtssTable2Size);

  //
  // PCH-LP HSIO PTSS Table Init
  //
  PeiPreMem->UnknowLpHsioPtssTable [0] = (VOID *) (UINTN) PcdGet32 (PcdUnknowLpHsioPtssTable1);
  PeiPreMem->UnknowLpHsioPtssTable [1] = (VOID *) (UINTN) PcdGet32 (PcdUnknowLpHsioPtssTable2);
  PeiPreMem->UnknowLpHsioPtssTableSize [0] = PcdGet16 (PcdUnknowLpHsioPtssTable1Size);
  PeiPreMem->UnknowLpHsioPtssTableSize [1] = PcdGet16 (PcdUnknowLpHsioPtssTable2Size);

  PeiPreMem->SpecificLpHsioPtssTable [0] = (VOID *) (UINTN) PcdGet32 (PcdSpecificLpHsioPtssTable1);
  PeiPreMem->SpecificLpHsioPtssTable [1] = (VOID *) (UINTN) PcdGet32 (PcdSpecificLpHsioPtssTable2);
  PeiPreMem->SpecificLpHsioPtssTableSize [0] = PcdGet16 (PcdSpecificLpHsioPtssTable1Size);
  PeiPreMem->SpecificLpHsioPtssTableSize [1] = PcdGet16 (PcdSpecificLpHsioPtssTable2Size);

  return EFI_SUCCESS;
}

/**
  Init Common Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.
  @param[in]  ConfigBlock       a pointer to PEI PreMem structure.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
InitCommonPlatformConfigBlockPreMem (
  IN PLATFORM_INFO *PlatformInfo,
  IN CONFIG_BLOCK *ConfigBlock
)
{
  BOARD_CONFIG_BLOCK_PEI_PREMEM *PeiPreMem;
  PCD64_BLOB Pcd64;

  PeiPreMem = (BOARD_CONFIG_BLOCK_PEI_PREMEM *) ConfigBlock;
#ifndef MINTREE_FLAG
  //
  // IoExpander Present?
  //
  PeiPreMem->IoExpander = PcdGetBool (PcdIoExpanderPresent);

  //
  // GPIO Group Tier
  //
  if (PcdGet32 (PcdGpioGroupToGpeDw0)) {
    PeiPreMem->Enable.GpioGroupTierInit = 1;
    PeiPreMem->GroupToGpeDw0 = PcdGet32 (PcdGpioGroupToGpeDw0);
    PeiPreMem->GroupToGpeDw1 = PcdGet32 (PcdGpioGroupToGpeDw1);
    PeiPreMem->GroupToGpeDw2 = PcdGet32 (PcdGpioGroupToGpeDw2);
  } else {
    PeiPreMem->Enable.GpioGroupTierInit = 0;
  }

  //
  // Pc8374SioKbc
  //
  PeiPreMem->Enable.Pc8374SioKbc = PcdGetBool (PcdPc8374SioKbcPresent);

  //
  // OddPower Init
  //
  PeiPreMem->Enable.OddPowerInit = PcdGetBool (PcdOddPowerInitEnable);

  //
  // Vboost Specific Init
  //
  PeiPreMem->Enable.VboostSpecificInit = PcdGetBool (PcdCpuVboostEnable);

  //
  // IVCAM Specific Init
  //
  PeiPreMem->Enable.IvCamSpecificInit = PcdGetBool (PcdIvCamInitPresent);
#endif
  //
  // RecoveryMode GPIO
  //
  Pcd64.Blob = PcdGet64 (PcdRecoveryModeGpio);
  if (Pcd64.BoardGpioConfig.Type != BoardGpioTypeNotSupported) {
    PeiPreMem->Enable.RecoveryModeGpio = 1;
    PeiPreMem->RecoveryModeGpio.Type = Pcd64.BoardGpioConfig.Type;
    PeiPreMem->RecoveryModeGpio.u.Expander.Section = Pcd64.BoardGpioConfig.u.Expander.Section;
    PeiPreMem->RecoveryModeGpio.u.Expander.Pin = Pcd64.BoardGpioConfig.u.Expander.Pin;
  } else {
    PeiPreMem->Enable.RecoveryModeGpio = 0;
  }

  return EFI_SUCCESS;
}

/**
  Init Platform Board Config Block for PreMem phase.

  @param[in]  ConfigBlock       a pointer to PEI PreMem structure.

  @retval     EFI_SUCCESS       The function completed successfully.
**/


EFI_STATUS
InitPlatformBoardConfigBlockPreMem (
  IN CONFIG_BLOCK *ConfigBlock
)
{
  EFI_STATUS Status;
  BOARD_CONFIG_BLOCK_PEI_PREMEM *PeiPreMem;
  PLATFORM_INFO *PlatformInfo;
  PEI_BOARD_CONFIG_PCD_INIT BoardConfigPcdInitPreMem;

  PlatformInfo = NULL;
  Status = GetPlatformInfo(&PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Init Board Config Pcd.
  //
  BoardConfigPcdInitPreMem = (PEI_BOARD_CONFIG_PCD_INIT) (UINTN) PcdGet32 (PcdFuncBoardConfigInitPreMem);
  ASSERT (BoardConfigPcdInitPreMem != NULL);

  if (BoardConfigPcdInitPreMem == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  Status = BoardConfigPcdInitPreMem (PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PeiPreMem = (BOARD_CONFIG_BLOCK_PEI_PREMEM *) ConfigBlock;
  CopyGuid (&PeiPreMem->Header.GuidHob.Name, &gPlatformBoardConfigPreMemGuid);

  Status = InitSaPlatformConfigBlockPreMem (PlatformInfo, ConfigBlock);
  ASSERT_EFI_ERROR(Status);

  Status = InitPchPlatformConfigBlockPreMem (PlatformInfo, ConfigBlock);
  ASSERT_EFI_ERROR(Status);

  Status = InitCommonPlatformConfigBlockPreMem (PlatformInfo, ConfigBlock);
  ASSERT_EFI_ERROR(Status);

  return Status;
}