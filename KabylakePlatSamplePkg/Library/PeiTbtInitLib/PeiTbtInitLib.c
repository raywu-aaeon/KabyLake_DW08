/**@file
  Thunderbolt(TM) Pei Library

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2018 Intel Corporation.

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

#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/GpioExpanderLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <SetupVariable.h>
#include <PlatformInfo.h>
#include <CmosMap.h>
#include <Include/CommonDefinitions.h>
#include <Library/TbtCommonLib.h>
#include <TbtBoardInfo.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PeiPlatformLib.h>
#include <Library/PeiPlatformHookLib.h>
#include <Library/BoardConfigLib.h>
#include <PlatformBoardConfig.h>
#include <Library/CmosAccessLib.h>
// AMI_OVERRIDE_START - For AMI and OEM Feature.
#include <Library/PchInfoLib.h>
#include <Library/AmiTbtOemLib.h>
// AMI_OVERRIDE_END - For AMI and OEM Feature.


GLOBAL_REMOVE_IF_UNREFERENCED TBT_INFO_HOB          *gTbtInfoHob;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_BOOT_MODE         BootMode;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                 TbtBootOn;


/**
  Create Tbt info HOB

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
CreateTbtInfoHob (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           VarSize;
  SETUP_DATA                      SystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  PLATFORM_INFO                   *PlatformInfo;
  BOOLEAN                         SetupValid;

  BOARD_CONFIG_BLOCK_PEI_POSTMEM  *PlatformBoardConfig;
// AMI_OVERRIDE_START - For AMI and OEM Feature.
  UINTN                           TbtDev, TbtFun;
// AMI_OVERRIDE_END - For AMI and OEM Feature.

  SetupValid  = FALSE;
  Status      = EFI_SUCCESS;
  PlatformInfo = NULL;

  DEBUG ((DEBUG_INFO, "TbtInfoHob EntryPoint \n"));

  //
  // Locate System Configuration Table
  //
  VarSize = sizeof (SETUP_DATA);
  ZeroMem (&SystemConfiguration, VarSize);
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );

  DEBUG ((DEBUG_INFO, "TbtBoardInfo LocatePpi \n"));
  if (!EFI_ERROR(Status)) {
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 PLATFORM_SETUP_VARIABLE_NAME,
                                 &gSetupVariableGuid,
                                 NULL,
                                 &VarSize,
                                 &SystemConfiguration
                                 );

    DEBUG ((DEBUG_INFO, "TbtBoardInfo PeiGetVariable \n"));
  }
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Setup valid is TRUE \n"));
    SetupValid = TRUE;
  } else {
    DEBUG ((DEBUG_INFO, "Setup valid is FALSE \n"));
  }

  //
  // Create HOB for TBT Data
  //
  Status = PeiServicesCreateHob (
             EFI_HOB_TYPE_GUID_EXTENSION,
             sizeof (TBT_INFO_HOB),
             (VOID **) &gTbtInfoHob
             );
  DEBUG ((DEBUG_INFO, "TbtBoardInfo CreateHob \n"));
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize the TBT INFO HOB data Once TBT enabled in setup.
  //
  gTbtInfoHob->EfiHobGuidType.Name = gTbtInfoHobGuid;
  gTbtInfoHob->RevisionId          = TBT_INFO_HOB_REVISION;
  gTbtInfoHob->HRPowerOffSupport   = 0;

  //
  // TBT Selector
  // 00 - AUTO
  // 01 - PCH RP1 [1 based RP]
  // 02 - PCH RP2 [1 based RP]
  // ....
  // 08 - PCH RP8 [1 based RP]
  //
  // Obtain Platform Info from HOB.
  //

  Status = GetPlatformInfo (&PlatformInfo);

  DEBUG ((DEBUG_INFO, "TbtBoardInfo GetPlatformInfo \n"));
  ASSERT_EFI_ERROR (Status);

  if (PlatformInfo == NULL) {
    DEBUG ((DEBUG_INFO, "PlatformInfo HOB retrieval failure. TBT failed \n"));
    return EFI_NOT_FOUND;
  }

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPostMemGuid, (VOID **) &PlatformBoardConfig);

  if (!EFI_ERROR(Status)) {
    gTbtInfoHob->TbtSupportedPortsInfo              = PlatformBoardConfig->TbtSupportedPortsInfo;
    gTbtInfoHob->TbtDefaultSelectorInfo             = PlatformBoardConfig->TbtDefaultSelectorInfo;

    gTbtInfoHob->ForcePwrGpio.GpioAccessType        = PlatformBoardConfig->ForcePwrGpio.GpioAccessType;
    gTbtInfoHob->ForcePwrGpio.Expander              = PlatformBoardConfig->ForcePwrGpio.Expander;
    gTbtInfoHob->ForcePwrGpio.GpioNumber            = PlatformBoardConfig->ForcePwrGpio.GpioNumber;
    gTbtInfoHob->ForcePwrGpio.GpioLevel             = PlatformBoardConfig->ForcePwrGpio.GpioLevel;

    gTbtInfoHob->CioPlugEventGpio.GpioAccessType    = PlatformBoardConfig->CioPlugEventGpio.GpioAccessType;
    gTbtInfoHob->CioPlugEventGpio.GpioNumber        = PlatformBoardConfig->CioPlugEventGpio.GpioNumber;
    gTbtInfoHob->CioPlugEventGpio.GpioMode          = PlatformBoardConfig->CioPlugEventGpio.GpioMode;
    gTbtInfoHob->CioPlugEventGpio.AcpiGpeSignature  = PlatformBoardConfig->CioPlugEventGpio.AcpiGpeSignature;

    gTbtInfoHob->CioPlugEventGpio.AcpiGpeStsAddress = PlatformBoardConfig->CioPlugEventGpio.AcpiGpeStsAddress;
    gTbtInfoHob->CioPlugEventGpio.AcpiGpeStsBit     = PlatformBoardConfig->CioPlugEventGpio.AcpiGpeStsBit;
    gTbtInfoHob->CioPlugEventGpio.AcpiGpeSignaturePorting = PlatformBoardConfig->CioPlugEventGpio.AcpiGpeSignaturePorting;
    gTbtInfoHob->CioPlugEventGpio.AcpiGpioGpeNumber = PlatformBoardConfig->CioPlugEventGpio.AcpiGpioGpeNumber;
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
    if (SystemConfiguration.Multicontroller) {
      gTbtInfoHob->TbtSupportedPortsInfo1              = PlatformBoardConfig->TbtSupportedPortsInfo1;
      gTbtInfoHob->TbtDefaultSelectorInfo1             = PlatformBoardConfig->TbtDefaultSelectorInfo1;

      gTbtInfoHob->ForcePwrGpio1.GpioAccessType        = PlatformBoardConfig->ForcePwrGpio1.GpioAccessType;
      gTbtInfoHob->ForcePwrGpio1.Expander              = PlatformBoardConfig->ForcePwrGpio1.Expander;
      gTbtInfoHob->ForcePwrGpio1.GpioNumber            = PlatformBoardConfig->ForcePwrGpio1.GpioNumber;
      gTbtInfoHob->ForcePwrGpio1.GpioLevel             = PlatformBoardConfig->ForcePwrGpio1.GpioLevel;

      gTbtInfoHob->CioPlugEventGpio1.GpioAccessType    = PlatformBoardConfig->CioPlugEventGpio1.GpioAccessType;
      gTbtInfoHob->CioPlugEventGpio1.GpioNumber        = PlatformBoardConfig->CioPlugEventGpio1.GpioNumber;
      gTbtInfoHob->CioPlugEventGpio1.GpioMode          = PlatformBoardConfig->CioPlugEventGpio1.GpioMode;
      gTbtInfoHob->CioPlugEventGpio1.AcpiGpeSignature  = PlatformBoardConfig->CioPlugEventGpio1.AcpiGpeSignature;

      gTbtInfoHob->CioPlugEventGpio1.AcpiGpeStsAddress = PlatformBoardConfig->CioPlugEventGpio1.AcpiGpeStsAddress;
      gTbtInfoHob->CioPlugEventGpio1.AcpiGpeStsBit     = PlatformBoardConfig->CioPlugEventGpio1.AcpiGpeStsBit;
      gTbtInfoHob->CioPlugEventGpio1.AcpiGpeSignaturePorting = PlatformBoardConfig->CioPlugEventGpio1.AcpiGpeSignaturePorting;
      gTbtInfoHob->CioPlugEventGpio1.AcpiGpioGpeNumber = PlatformBoardConfig->CioPlugEventGpio1.AcpiGpioGpeNumber;
    }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
  }


  if ((!SetupValid) || (SetupValid && SystemConfiguration.TbtSupport == 0) || (gTbtInfoHob->CioPlugEventGpio.AcpiGpeSignaturePorting == 0)) {
    gTbtInfoHob->CioPlugEventGpio.AcpiGpeSignature = SIGNATURE_32 ('X', 'T', 'B', 'T');
  }
  gTbtInfoHob->TbtUsbSplitMode          = PlatformBoardConfig->TbtUsbSplitMode;
  gTbtInfoHob->SplitModePchUsb2TbtPort1 = PlatformBoardConfig->SplitModePchUsb2TbtPort1;
  gTbtInfoHob->SplitModePchUsb2TbtPort2 = PlatformBoardConfig->SplitModePchUsb2TbtPort2;

// AMI_OVERRIDE_START - For AMI and OEM Feature.
  if (GetPchSeries() == PchLp) {
    gTbtInfoHob->TbtSupportedPortsInfo            = 0x1FFF;
  } else {
    gTbtInfoHob->TbtSupportedPortsInfo            = 0x1FFFFF;
  }
  gTbtInfoHob->TbtDefaultSelectorInfo             = DEFAULT_TBT_AIC_LOCATION;

  gTbtInfoHob->TbtSupport            = SystemConfiguration.TbtSupport;
  gTbtInfoHob->TbtSelector           = SystemConfiguration.TbtSelector;

  Status = GetTbtRpDevFun ((gTbtInfoHob->TbtSelector - 1), &TbtDev, &TbtFun);
  DEBUG ((DEBUG_INFO, "Get Dev Fun: %x %x\n", TbtDev, TbtFun));
  gTbtInfoHob->Bus                   = TBT_UP_PORT_BUS;
  gTbtInfoHob->Dev                   = TBT_UP_PORT_DEV;
  if (gTbtInfoHob->Dev != TbtDev) gTbtInfoHob->Dev = (UINT8)TbtDev;
  gTbtInfoHob->Fun                   = TBT_UP_PORT_FUN;
  if (gTbtInfoHob->Fun != TbtFun) gTbtInfoHob->Fun = (UINT8)TbtFun;

  gTbtInfoHob->SecurityMode          = SystemConfiguration.SecurityMode;
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  gTbtInfoHob->SprSupport            = SystemConfiguration.SprSupport;
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
  gTbtInfoHob->AicArSupport          = SystemConfiguration.AicArSupport;
  gTbtInfoHob->TbtBootOn             = SystemConfiguration.TbtBootOn;
  gTbtInfoHob->TbtUsbOn              = SystemConfiguration.TbtUsbOn;
  gTbtInfoHob->TbtWakeupSupport      = SystemConfiguration.TbtWakeupSupport;
  gTbtInfoHob->TbtCacheLineSize      = SystemConfiguration.TbtCacheLineSize;
  gTbtInfoHob->TbtForcePwrGroup      = ConvertGpioGroup(TBT_FORCEPWR_GROUP);
  gTbtInfoHob->TbtForcePwrPad        = TBT_FORCEPWR_PAD;
  gTbtInfoHob->TbtHotPlugEvtGroup    = ConvertGpioGroup(TBT_HOTPLUGEVT_GROUP);
  gTbtInfoHob->TbtHotPlugEvtPad      = TBT_HOTPLUGEVT_PAD;
  gTbtInfoHob->TbtSkipPciOprom       = SystemConfiguration.TbtSkipPciOprom;
  gTbtInfoHob->TbtSkipPciEnumeration = SystemConfiguration.TbtSkipPciEnumeration;
  gTbtInfoHob->TbtAspm               = SystemConfiguration.TbtAspm;
  gTbtInfoHob->TbtLtr                = SystemConfiguration.TbtLtr;
  gTbtInfoHob->TBTHotSMI             = SystemConfiguration.TBTHotSMI;
  gTbtInfoHob->TBTHotNotify          = SystemConfiguration.TBTHotNotify;
  gTbtInfoHob->TbtAcpiRemovalSupport = SystemConfiguration.TbtAcpiRemovalSupport;
  gTbtInfoHob->ReserveMemoryPerSlot  = SystemConfiguration.ReserveMemoryPerSlot;
  gTbtInfoHob->ReservePMemoryPerSlot = SystemConfiguration.ReservePMemoryPerSlot;
  gTbtInfoHob->AssignIo              = SystemConfiguration.AssignIo;
  gTbtInfoHob->ReserveIoPerSlot      = SystemConfiguration.ReserveIoPerSlot;
// AMI_OVERRIDE_END - For AMI and OEM Feature.

  DEBUG ((DEBUG_INFO, "TbtSupportedPortsInfo = 0x%lx \n", gTbtInfoHob->TbtSupportedPortsInfo));
  DEBUG ((DEBUG_INFO, "TbtDefaultSelectorInfo = 0x%x \n", gTbtInfoHob->TbtDefaultSelectorInfo));
  DEBUG ((DEBUG_INFO, "ForcePwrGpio.GpioNumber = 0x%x \n", gTbtInfoHob->ForcePwrGpio.GpioNumber));
  DEBUG ((DEBUG_INFO, "ForcePwrGpio.GpioLevel = 0x%x \n", gTbtInfoHob->ForcePwrGpio.GpioLevel));
  DEBUG ((DEBUG_INFO, "CioPlugEventGpio.GpioNumber = 0x%x \n", gTbtInfoHob->CioPlugEventGpio.GpioNumber));
  DEBUG ((DEBUG_INFO, "CioPlugEventGpio.AcpiGpioGpeNumber = 0x%x \n", gTbtInfoHob->CioPlugEventGpio.AcpiGpioGpeNumber));
  DEBUG ((DEBUG_INFO, "CioPlugEventGpio.AcpiGpeStsAddress = 0x%x \n", gTbtInfoHob->CioPlugEventGpio.AcpiGpeStsAddress));
  DEBUG ((DEBUG_INFO, "CioPlugEventGpio.AcpiGpeStsBit = 0x%x \n", gTbtInfoHob->CioPlugEventGpio.AcpiGpeStsBit));
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  if (SystemConfiguration.Multicontroller) {
    DEBUG ((DEBUG_INFO, "TbtSupportedPortsInfo1 = 0x%lx \n", gTbtInfoHob->TbtSupportedPortsInfo1));
    DEBUG ((DEBUG_INFO, "TbtDefaultSelectorInfo1 = 0x%x \n", gTbtInfoHob->TbtDefaultSelectorInfo1));
    DEBUG ((DEBUG_INFO, "ForcePwrGpio.GpioNumber1 = 0x%x \n", gTbtInfoHob->ForcePwrGpio1.GpioNumber));
    DEBUG ((DEBUG_INFO, "ForcePwrGpio.GpioLevel1 = 0x%x \n", gTbtInfoHob->ForcePwrGpio1.GpioLevel));
    DEBUG ((DEBUG_INFO, "CioPlugEventGpio.GpioNumber1 = 0x%x \n", gTbtInfoHob->CioPlugEventGpio1.GpioNumber));
    DEBUG ((DEBUG_INFO, "CioPlugEventGpio.AcpiGpioGpeNumber1 = 0x%x \n", gTbtInfoHob->CioPlugEventGpio1.AcpiGpioGpeNumber));
    DEBUG ((DEBUG_INFO, "CioPlugEventGpio.AcpiGpeStsAddress1 = 0x%x \n", gTbtInfoHob->CioPlugEventGpio1.AcpiGpeStsAddress));
    DEBUG ((DEBUG_INFO, "CioPlugEventGpio.AcpiGpeStsBit1 = 0x%x \n", gTbtInfoHob->CioPlugEventGpio1.AcpiGpeStsBit));
  }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
// AMI_OVERRIDE_START - For AMI and OEM Feature.
  DEBUG ((DEBUG_INFO, "TbtSupport : %x\n", gTbtInfoHob->TbtSupport));
  DEBUG ((DEBUG_INFO, "TbtSelector: %x\n", gTbtInfoHob->TbtSelector));
  DEBUG ((DEBUG_INFO, "Bus: %x\n", gTbtInfoHob->Bus));
  DEBUG ((DEBUG_INFO, "Dev: %x\n", gTbtInfoHob->Dev));
  DEBUG ((DEBUG_INFO, "Fun: %x\n", gTbtInfoHob->Fun));
  DEBUG ((DEBUG_INFO, "SecurityMode: %x\n", gTbtInfoHob->SecurityMode));
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  DEBUG ((DEBUG_INFO, "SprSupport: %x\n", gTbtInfoHob->SprSupport));
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
  DEBUG ((DEBUG_INFO, "AicArSupport: %x\n", gTbtInfoHob->AicArSupport));
  DEBUG ((DEBUG_INFO, "TbtBootOn : %x\n", gTbtInfoHob->TbtBootOn));
  DEBUG ((DEBUG_INFO, "TbtUsbOn : %x\n", gTbtInfoHob->TbtUsbOn));
  DEBUG ((DEBUG_INFO, "TbtWakeupSupport: %x\n", gTbtInfoHob->TbtWakeupSupport));
  DEBUG ((DEBUG_INFO, "TbtCacheLineSize: %x\n", gTbtInfoHob->TbtCacheLineSize));
  DEBUG ((DEBUG_INFO, "TbtForcePwrGroup: %x\n", gTbtInfoHob->TbtForcePwrGroup));
  DEBUG ((DEBUG_INFO, "TbtForcePwrPad: %x\n", gTbtInfoHob->TbtForcePwrPad));
  DEBUG ((DEBUG_INFO, "TbtHotPlugEvtGroup: %x\n", gTbtInfoHob->TbtHotPlugEvtGroup));
  DEBUG ((DEBUG_INFO, "TbtHotPlugEvtPad: %x\n", gTbtInfoHob->TbtHotPlugEvtPad));
  DEBUG ((DEBUG_INFO, "TbtSkipPciOprom: %x\n", gTbtInfoHob->TbtSkipPciOprom));
  DEBUG ((DEBUG_INFO, "TbtAspm: %x\n", gTbtInfoHob->TbtAspm));
  DEBUG ((DEBUG_INFO, "TbtLtr: %x\n", gTbtInfoHob->TbtLtr));
  DEBUG ((DEBUG_INFO, "TBTHotSMI: %x\n", gTbtInfoHob->TBTHotSMI));
  DEBUG ((DEBUG_INFO, "TBTHotNotify: %x\n", gTbtInfoHob->TBTHotNotify));
  DEBUG ((DEBUG_INFO, "TbtAcpiRemovalSupport: %x\n", gTbtInfoHob->TbtAcpiRemovalSupport));
  DEBUG ((DEBUG_INFO, "ReserveMemoryPerSlot: %x\n", gTbtInfoHob->ReserveMemoryPerSlot));
  DEBUG ((DEBUG_INFO, "ReservePMemoryPerSlot: %x\n", gTbtInfoHob->ReservePMemoryPerSlot));
  DEBUG ((DEBUG_INFO, "AssignIo: %x\n", gTbtInfoHob->AssignIo));
  DEBUG ((DEBUG_INFO, "ReserveIoPerSlot: %x\n", gTbtInfoHob->ReserveIoPerSlot));
// AMI_OVERRIDE_END - For AMI and OEM Feature.

  return EFI_SUCCESS;
}


/**
  GPIO write

  @param[in]  GpioAccessType
  @param[in]  Expander
  @param[in]  GpioNumber
  @param[in]  Value
**/
VOID
GpioWrite (
  IN  UINT8          GpioAccessType,
  IN  UINT8          Expander,
  IN  UINT32         GpioNumber,
  IN  BOOLEAN        Value
  )
{

  if (GpioAccessType == 0x01) {
    // PCH
    GpioSetOutputValue (GpioNumber, (UINT32)Value);
  } else if (GpioAccessType == 0x02) {
    // IoExpander {TCA6424A}
    GpioExpSetOutput (Expander, (UINT8)GpioNumber, (UINT8)Value);
  }
}

/**
  GPIO read

  @param[in]  GpioAccessType
  @param[in]  Expander
  @param[in]  GpioNumber
  @param[in]  PadState {0: GPO [GPIO TX State], 1: GPI [GPIO RX State]}
**/
UINT8
GpioRead (
  IN  UINT8          GpioAccessType,
  IN  UINT8          Expander,
  IN  UINT32         GpioNumber,
  IN  UINT8          PadState
  )
{
  UINT8       Data8;
  UINT32      Data32;

  Data8 = 0;
  Data32 = 0;

  if (GpioAccessType == 0x01) {
    // PCH
    if (PadState == 0x01) {
      // GPIO RX State
      GpioGetInputValue (GpioNumber, &Data32);
      Data8 = (UINT8) Data32;
    }
  } else if (GpioAccessType == 0x02) {
    // IoExpander {TCA6424A}

    Data8 = GpioExpGetInput (Expander, (UINT8)GpioNumber);
  }

  return Data8;
}

/**
  Based on the Security Mode Selection, BIOS drives FORCE_PWR.

  @param[in]  GpioAccessType
  @param[in]  Expander
  @param[in]  GpioNumber
  @param[in]  Value
**/
VOID
ForcePower (
  IN  UINT8          GpioAccessType,
  IN  UINT8          Expander,
  IN  UINT32         GpioNumber,
  IN  BOOLEAN        Value
)
{
  GpioWrite (GpioAccessType, Expander, GpioNumber, Value);
}


/**
  Is host router present before sleep

  @Retval     true      if everything's OK
**/
UINT8
IsHostRouterPresentBeforeSleep (
// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM.
#if 0
  VOID
#else
  IN AMI_TBT_HR_STATUS_DATA     *HrInfo
#endif
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM.
  )
{
// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM
#if 0
  UINT8  Data = 0;
  Data = CmosRead8 (CMOS_TBTHR_PRESENT_ON_RESUME);
  return Data;
#else
  DEBUG ((DEBUG_INFO, "TbtPei: Chcek Tbt Host status...\n"));

  DEBUG ((DEBUG_INFO, "TbtPei: Hr Status: %x\n", HrInfo->TbtHrStatus));
  if (HrInfo->TbtHrStatus != 0) return TRUE;
  else return FALSE;
#endif
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM
}


/**
  Set the output variables as per Setup options.

  @param[in]  Command   TBT Command

  @Retval     true      if everything's OK
**/
BOOLEAN
TbtSetPCIe2TbtCommand (
  IN    UINT8                   Command
  )
{
  UINTN   TbtUs;
  UINTN   Pcie2Tbt;
  UINTN   Tbt2Pcie;
  UINT32  RegVal;
  UINT32  MaxLoopCount;
  UINTN   Delay;
  UINT8   RetCode;
  BOOLEAN Ret;
  BOOLEAN Ret1;
// AMI_OVERRIDE_START - Avoid this address for OEM used.
#if 0
  TbtUs         = MmPciBase (0xF0, 0, 0);
#else
  TbtUs         = MmPciBase (0x3F, 0, 0);
#endif
// AMI_OVERRIDE_END - Avoid this address for OEM used.
  Pcie2Tbt      = TbtUs + PCIE2TBT_R;
  Tbt2Pcie      = TbtUs + TBT2PCIE_R;
  MaxLoopCount  = 10; // Wait 1 sec
  Delay         = 100 * 1000;
  RetCode       = 0x52;
  Ret           = FALSE;
  Ret1          = FALSE;

  //
  // Pre-boot ACL during S4 exit:
  // Bios needs to execute SX_EXIT_TBT_CONNECTED mailbox command with Data parameter equal to 1 and timeout value equal to 3 sec.
  // Execute Fast Link bring up flow with 3 seconds timeout.
  //
  if (Command == PCIE2TBT_SX_EXIT_TBT_CONNECTED && BootMode == BOOT_ON_S4_RESUME && TbtBootOn == 2 ) {
    RegVal = (1 << 8) | Command | PCIE2TBT_VLD_B;
    DEBUG ((DEBUG_INFO, "Data to be written = 0x%lx \n", (1 << 8) | Command | PCIE2TBT_VLD_B));
    MmioWrite32 (Pcie2Tbt, RegVal);
    MaxLoopCount  = 30; // Wait 3 sec
  }
  else {
    RegVal        = Command | PCIE2TBT_VLD_B;
    MmioWrite32 (Pcie2Tbt, RegVal);
  }

  //
  // for Pre-boot ACL timeout value is 3 sec
  //
  if (Command == PCIE2TBT_PREBOOTACL) {
    MaxLoopCount  = 30; // Wait 3 sec 
  }

  DEBUG ((DEBUG_INFO, "MaxLoopCount for TBT mailbox is = %d\n",MaxLoopCount));
  while (MaxLoopCount-- > 0) {
    RegVal = MmioRead32 (Tbt2Pcie);
    if (0xFFFFFFFF == RegVal) {
      //
      // Device is not here return now
      //
      RetCode = 0x5F;
      break;
    }

    if (RegVal & TBT2PCIE_DON_R) {
      RetCode = 0x51;
      Ret     = TRUE;
      break;
    }

    MicroSecondDelay (Delay);
  }

  MmioWrite32 (Pcie2Tbt, 0);
  MaxLoopCount = 10;
  while (MaxLoopCount-- > 0) {
    RegVal = MmioRead32(Tbt2Pcie);
    if (0xFFFFFFFF == RegVal) {
      //
      // Device is not here return now
      //
      RetCode = 0x5F;
      break;
    }

    if (!(RegVal & TBT2PCIE_DON_R)) {
      RetCode = 0x51;
      Ret1 = TRUE;
      break;
    }

    MicroSecondDelay(Delay);
  }
  return (Ret && Ret1);
}


/**
  This function gets current TBT settings as per Setup options.

  @param[out] TbtSelector
  @param[out] TbtSupport
  @param[out] Gpio3ForcePwr
  @param[out] Gpio3ForcePwrDly
  @param[out] AICSupport
  @param[out] TbtUsbOn
  @param[out] Multicontroller
  @param[out] Gpio3ForcePwr1

**/
STATIC
VOID
GetSetupVars (
  OUT     UINT8                     *TbtSelector,
  OUT     UINT8                     *TbtSupport,
  OUT     BOOLEAN                   *Gpio3ForcePwr,
  OUT     UINT16                    *Gpio3ForcePwrDly,
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  OUT     BOOLEAN               	*SprSupport,
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
  OUT     BOOLEAN                   *TbtUsbOn,
  OUT     UINT8                     *Multicontroller,
  OUT     BOOLEAN                   *Gpio3ForcePwr1
  )
{
  EFI_STATUS                      Status;
  UINTN                           VarSize;
  SETUP_DATA                      SystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;

  *TbtSelector        = AUTO;
  *TbtSupport         = 0;
  *Gpio3ForcePwr      = FALSE;
  *Gpio3ForcePwr1     = FALSE;
  *Gpio3ForcePwrDly   = 200;
  *TbtUsbOn           = FALSE;
  TbtBootOn           = 0;

  //
  // Locate System Configuration Table
  //
  VarSize = sizeof (SETUP_DATA);

  SetMem (&SystemConfiguration, VarSize, 0);

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );

  if (Status != EFI_SUCCESS) {
    return;
  }

  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );

  if (Status != EFI_SUCCESS) {
    return;
  }

  *TbtSupport         = SystemConfiguration.TbtSupport;
  *TbtSelector        = SystemConfiguration.TbtSelector;
  *Gpio3ForcePwr      = SystemConfiguration.Gpio3ForcePwr;
  *Gpio3ForcePwrDly   = SystemConfiguration.Gpio3ForcePwrDly;
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  *SprSupport         = SystemConfiguration.SprSupport;
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
  *TbtUsbOn           = SystemConfiguration.TbtUsbOn;
  TbtBootOn           = SystemConfiguration.TbtBootOn;
// AMI_OVERRIDE_START - unsupported TBT Multicontroller.
#if 0
  *Multicontroller    = SystemConfiguration.Multicontroller;
  *Gpio3ForcePwr1     = SystemConfiguration.Gpio3ForcePwr1;
#else
  *Multicontroller    = 0;
  *Gpio3ForcePwr1     = FALSE;
#endif
// AMI_OVERRIDE_END - unsupported TBT Multicontroller.
}

/**
  Read/Write the GPE_STS for CioPlugEventGpio.AcpiGpeSts

  @param[in] AcpiGpeStsAddress
  @param[in] AcpiGpeStsBit
  @param[in] GpioPad
  @param[in] Operation {0: Clear the bit, 1:Read the bit}
**/
UINT8
AcpiGpeStsRW (
  IN      UINT16                    AcpiGpeStsAddress,
  IN      UINT8                     AcpiGpeStsBit,
  IN      GPIO_PAD                  GpioPad,
  IN      UINT8                     Operation
  )
{
  UINT16    Address;
  UINT8     BitPosition;
  UINT8     Mask;
  UINT8     Data8;
  BOOLEAN   TwoTierState;
  UINT32    Data32;

  TwoTierState = GpioCheckFor2Tier (GpioPad);

  Address = AcpiGpeStsAddress;
  BitPosition = AcpiGpeStsBit;

  while (BitPosition > 8) {
    Address+=1;
    BitPosition-=8;
  }

  Mask = 1<<BitPosition;
  Data8 = 0;

  if (Operation == 0x00) /* Clear the bit */ {
    if (TwoTierState) {
      //Clear GPI_GPE_STS
      GpioClearGpiGpeSts(GpioPad);
    }

    // Clear ACPI_GPE_STS
    IoWrite8(Address, Mask);
  } else if (Operation == 0x01) /* Read the bit */ {
    if (TwoTierState) {
      //Read GPI_GPE_STS
      GpioGetGpiGpeSts(GpioPad, &Data32);
      Data8 = (UINT8) Data32;
    } else {
      // Read ACPI_GPE_STS
      Data8 = IoRead8(Address);
      Data8>>=BitPosition;
      Data8&=1;
    }
  }

  return (Data8);
}

/**
  Set the Sleep Mode if the HR is up.
**/
VOID
TbtSetSxMode (
  VOID
  )
{
  UINTN   TbtUs;
  UINTN   TbtUsDevId;
  UINTN   Tbt2Pcie;
  UINT32  RegVal;
  UINT32  MaxLoopCount;
  UINTN   Delay;
  UINT8   RetCode;
  UINT32  Count;
// AMI_OVERRIDE_START - Avoid this address for OEM used.
#if 0
  TbtUs         = MmPciBase (0xF0, 0, 0);
#else
  TbtUs         = MmPciBase (0x3F, 0, 0);
#endif
// AMI_OVERRIDE_END - Avoid this address for OEM used.
  TbtUsDevId    = TbtUs;
  Tbt2Pcie      = TbtUs + TBT2PCIE_R;
  MaxLoopCount  = 150;  // Wait 15 sec
  Delay         = 100 * 1000;
  RetCode       = 0x62;

  //
  // WA: When system with TBT 3.1 device, resume SX system need to wait device ready. In document that maximum time out should be 500ms.
  //
  Count = 0;
  while (MmioRead32(TbtUs + PCI_RID) == 0xffffffff) { //End Device will be with Device Number 0x0, Function Number 0x0.
    MicroSecondDelay(STALL_ONE_MICRO_SECOND * 1000);  // 1000usec
                                                      //
                                                      // Make sure PCH link up is ready for TBT controller.
                                                      //
    MmioWrite32(TbtUs, 0x00);
    Count++;
    if (Count > 1000) { //Allowing Max Delay of 1 sec for CFL-S board.
      break;
    }
  }

  if (!TbtSetPCIe2TbtCommand (PCIE2TBT_SX_EXIT_TBT_CONNECTED)) {
    //
    // Nothing to wait, HR is not responsive
    //
    return;
  }

  DEBUG ((DEBUG_INFO, "Wait for Dev ID != 0xFF\n"));
//AMI_OVERRIDE_START - Add for checking the TBT status.
  IoWrite8(0x80, 0x60);
//AMI_OVERRIDE_END - Add for checking the TBT status.

  while (MaxLoopCount-- > 0) {
    //
    // Check what HR still here
    //
    RegVal = MmioRead32 (Tbt2Pcie);
    if (0xFFFFFFFF == RegVal) {
      RetCode = 0x6F;
      break;
    }
    //
    // Check completion of TBT link
    //
    RegVal = MmioRead32 (TbtUsDevId);
    if (0xFFFFFFFF != RegVal) {
      RetCode = 0x61;
      break;
    }

    MicroSecondDelay (Delay);
  }

  DEBUG ((DEBUG_INFO, "Return code = 0x%x\n", RetCode));
//AMI_OVERRIDE_START - Add for checking the TBT status.
  IoWrite8(0x80, RetCode);
//AMI_OVERRIDE_END - Add for checking the TBT status.
}


/**
  Initialize Thunderbolt(TM)

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
TbtInit (
  VOID
  )
{
  EFI_STATUS            Status;
  UINT8                 TbtSelector;
  UINT32                SubBus;
  BOOLEAN               TbtSupport;
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  BOOLEAN               SprSupport;
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
  UINTN                 TbtRp;
// AMI_OVERRIDE_START - EIP279674 : In DEBUG mode, we will find TBT boot command failed in PEI.
  //UINTN                 TbtBaseAddress;
  //UINT32                MaxWaitIter;
  //UINT32                RegVal;
// AMI_OVERRIDE_END - EIP279674 : In DEBUG mode, we will find TBT boot command failed in PEI.
  UINT32                PchPwrmBase;
  BOOLEAN               Gpio3ForcePwr;
  BOOLEAN               Gpio3ForcePwr1;
  UINT16                Gpio3ForcePwrDly;
// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM >>
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  AMI_TBT_HR_STATUS_DATA        HrStatusData;
  UINTN                         HrStatusSize;
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM >>
  EFI_PEI_HOB_POINTERS  Hob;
  UINTN                 HrBaseAddress;
  UINT16                Data16;
  UINTN                 RpDev;
  UINTN                 RpFunc;
  BOOLEAN               TbtUsbOn;
  UINT8                 Multicontroller;

  BOARD_CONFIG_BLOCK_PEI_POSTMEM  *PlatformBoardConfig;

//AMI_OVERRIDE_START >>> EIP353107 : Boot On command has the fail rate
  SETUP_DATA            SystemConfiguration;
  UINTN                 VarSize;
  UINT16                TbtBeforeBootOnCmdDelay;
  UINT16                TbtBeforeUsbOnCmdDelay;
  UINT16                TbtBeforeSxExitCmdDelay;
//AMI_OVERRIDE_END <<< EIP353107 : Boot On command has the fail rate

  Status          = EFI_SUCCESS;
  TbtSelector     = 0;
  TbtSupport      = 0;
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  SprSupport      = 0;
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
  Gpio3ForcePwr   = FALSE;
  Gpio3ForcePwr1  = FALSE;
  TbtUsbOn        = FALSE;
  TbtBootOn       = 0;
  Multicontroller = 0;
// AMI_OVERRIDE_START - EIP279674 : In DEBUG mode, we will find TBT boot command failed in PEI.
//MaxWaitIter     = 50;   // Wait 5 sec
// AMI_OVERRIDE_END - EIP279674 : In DEBUG mode, we will find TBT boot command failed in PEI.
// AMI_OVERRIDE_START - EIP253592 : PCIe SSD can't be found on slot(D29F0) if the option of "Fully Disabled" on ThunderBolt is no function.
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  GetSetupVars (&TbtSelector, &TbtSupport, &Gpio3ForcePwr, &Gpio3ForcePwrDly, &SprSupport, &TbtUsbOn, &Multicontroller, &Gpio3ForcePwr1);
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
// AMI_OVERRIDE_END - EIP253592 : PCIe SSD can't be found on slot(D29F0) if the option of "Fully Disabled" on ThunderBolt is no function.

// AMI_OVERRIDE_START - EIP261617 : Thunderbolt callback functions will fail on KBL CRB_05.
/*
// AMI_OVERRIDE_START - System doesn't need to initialize TBT if TbtSupport is Fully Disable.
  if (TbtSupport == Tbt_Fully_Disabled) {
    DEBUG ((DEBUG_INFO, "<TbtPei> Thunderbolt Feature is disable fully in Setup !!!\n"));
    DEBUG ((DEBUG_INFO, "<TbtPei> Initalize Thunderbolt (TBT) Exit\n"));
    return EFI_SUCCESS;
  }
// AMI_OVERRIDE_END - System doesn't need to initialize TBT if TbtSupport is Fully Disable.
*/
// AMI_OVERRIDE_END - EIP261617 : Thunderbolt callback functions will fail on KBL CRB_05.

  DEBUG ((DEBUG_INFO, "Initalize Thunderbolt(TM) (TBT) EntryPoint\n"));

  Status = PeiServicesGetBootMode (&BootMode);
// AMI_OVERRIDE_START - Remove the condition to bring EIP280264 back
  //
  // It will cause EIP280264 have no effect.
  //
  //if ((!(BootMode == BOOT_ON_S3_RESUME ))) {
  Status = CreateTbtInfoHob ();
  ASSERT_EFI_ERROR (Status);

  //
  // Obtain TBT Info from HOB.
  //
  Status = PeiServicesGetHobList ((VOID **) &Hob.Raw);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hob.Raw = GetNextGuidHob (&gTbtInfoHobGuid, Hob.Raw);
  if (Hob.Raw == NULL) {
    return EFI_NOT_FOUND;
  }

  gTbtInfoHob = (TBT_INFO_HOB *) Hob.Raw;
  if (gTbtInfoHob == NULL) {
    return EFI_NOT_FOUND;
  }
  //}
// AMI_OVERRIDE_END - Remove the condition to bring EIP280264 back

// AMI_OVERRIDE_START - EIP261617 : Thunderbolt callback functions will fail on KBL CRB_05.
  if (TbtSupport == Tbt_Fully_Disabled) {
    DEBUG ((DEBUG_INFO, "<TbtPei> Thunderbolt Feature is disable fully in Setup !!!\n"));
    DEBUG ((DEBUG_INFO, "<TbtPei> Initalize Thunderbolt (TBT) Exit\n"));
    return EFI_SUCCESS;
  }
// AMI_OVERRIDE_END - EIP261617 : Thunderbolt callback functions will fail on KBL CRB_05.

// AMI_OVERRIDE_START - EIP253592 : PCIe SSD can't be found on slot(D29F0) if the option of "Fully Disabled" on ThunderBolt is no function.
#if 0
  GetSetupVars (&TbtSelector, &TbtSupport, &Gpio3ForcePwr, &Gpio3ForcePwrDly, &AICSupport, &TbtUsbOn,&Multicontroller,&Gpio3ForcePwr1);
#endif
// AMI_OVERRIDE_END - EIP253592 : PCIe SSD can't be found on slot(D29F0) if the option of "Fully Disabled" on ThunderBolt is no function.

  (TbtSupport == TRUE)? PcdSetBoolS (PcdTbtSupport, TRUE) : PcdSetBoolS (PcdTbtSupport, FALSE);

  if (TbtSupport == FALSE) {
    return EFI_SUCCESS;
  }

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPostMemGuid, (VOID **) &PlatformBoardConfig);
  ASSERT_EFI_ERROR (Status);

  if (TbtSelector == 0) {
    TbtSelector = PlatformBoardConfig->TbtDefaultSelectorInfo;
  }

// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM.
  // Get AMI TBT variable
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             &VariableServices
             );
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "TbtPei: Can't get PeiReadOnlyVariable2Ppi...\n"));
    return EFI_SUCCESS;
  }

  HrStatusSize = sizeof(AMI_TBT_HR_STATUS_DATA);
  Status = VariableServices->GetVariable (
                                 VariableServices,
                                 AMI_TBT_HR_STATUS_VARIABLE,
                                 &gAmiTbtHrStatusGuid,
                                 NULL,
                                 &HrStatusSize,
                                 &HrStatusData
                                 );
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "TbtPei: Can't get Ami Tbt Variable...\n"));
    return EFI_SUCCESS;
  }
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM.
  DEBUG ((DEBUG_INFO, "<TbtPei> TbtSelector = %x \n", TbtSelector));

//AMI_OVERRIDE_START >>> EIP353107 : Boot On command has the fail rate
  //
  // Locate System Configuration Table for command delay
  //
  VarSize = sizeof (SETUP_DATA);
  SetMem (&SystemConfiguration, VarSize, 0);

  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );

  //
  // These delay numbers are used for MicroSecondDelay()
  //
  if (Status == EFI_SUCCESS) {
    TbtBeforeBootOnCmdDelay = SystemConfiguration.TbtBeforeBootOnCmdDelay;
    TbtBeforeUsbOnCmdDelay  = SystemConfiguration.TbtBeforeUsbOnCmdDelay;
    TbtBeforeSxExitCmdDelay = SystemConfiguration.TbtBeforeSxExitCmdDelay;
  } else {
    //
    // 200 ms delay is suitable on KBL-R
    //

    TbtBeforeBootOnCmdDelay = 200;
    TbtBeforeUsbOnCmdDelay  = 200;
    TbtBeforeSxExitCmdDelay = 200;
  }
//AMI_OVERRIDE_END <<< EIP353107 : Boot On command has the fail rate

  //
  //When Thunderbolt(TM) boot [TbtBootOn] is enabled in bios setup we need to do the below:
  //   Bios should send "Boot On" message through PCIE2TBT register
  //   The Boot On command as described above would include the command and acknowledge from FW (with the default timeout in BIOS),
  //   once the Boot On command is completed it is guaranteed that the AlpineRidge(AR) device is there and the PCI tunneling was done by FW,
  //   next step from BIOS is enumeration using SMI
  //

  if (TbtBootOn > 0) {
    Status = GetTbtRpDevFun (TbtSelector -1, &RpDev, &RpFunc);
    ASSERT_EFI_ERROR(Status);
    TbtRp = MmPCIeAddress(0x00, RpDev, RpFunc, PCI_PBUS);

    SubBus = MmioRead32 (TbtRp);
    //
    // Set Sec/Sub buses to 0xF0
    //
// AMI_OVERRIDE_START - Avoid this address for OEM used.
#if 0
    MmioWrite32 (TbtRp, 0x00F0F000);
#else
    MmioWrite32 (TbtRp, 0x003F3F00);
#endif
// AMI_OVERRIDE_END - Avoid this address for OEM used.


    //
    // Exectue Mail box command "Boot On "
    //
    //Command may be executed only during boot/reboot and not during Sx exit flow
    if (((BootMode != BOOT_ON_S2_RESUME) && (BootMode != BOOT_ON_S3_RESUME) &&
      (BootMode != BOOT_ON_S4_RESUME))) {
//AMI_OVERRIDE_START >>> EIP353107 : Boot On command has the fail rate
        MicroSecondDelay ((UINTN)(TbtBeforeBootOnCmdDelay * 1000));
//AMI_OVERRIDE_END <<< EIP353107 : Boot On command has the fail rate
        if (TbtBootOn == 1) {
          if (!TbtSetPCIe2TbtCommand (PCIE2TBT_BOOT_ON)) {
            //
            // Nothing to wait, HR is not responsive
            //
            DEBUG ((DEBUG_INFO, "<TbtPei> TbtBootSupport - Boot On message sent failed \n"));
          }
        }
        if (TbtBootOn == 2) {
          if (!TbtSetPCIe2TbtCommand (PCIE2TBT_PREBOOTACL)) {
            //
            // Nothing to wait, HR is not responsive
            //
            DEBUG ((DEBUG_INFO, "<TbtPei> TbtBootSupport - Preboot ACL message sent failed \n"));
          }        
        }
    }


    //
    // Reset Sec/Sub buses to original value
    //
    MmioWrite32 (TbtRp, SubBus);
  }

  //
  //When Thunderbolt(TM) Usb boot [TbtUsbOn] is enabled in bios setup we need to do the below:
  //   Bios should send "Usb On" message through PCIE2TBT register
  //   The Usb On command as described above would include the command and acknowledge from FW (with the default timeout in BIOS),
  //   once the Usb On command is completed it is guaranteed that the AlpineRidge(AR) device is there and the PCI tunneling was done by FW,
  //   next step from BIOS is enumeration using SMI
  //

  if(TbtUsbOn) {
    Status = GetTbtRpDevFun (TbtSelector - 1, &RpDev, &RpFunc);
    ASSERT_EFI_ERROR (Status);
    TbtRp = MmPCIeAddress (0x00, RpDev, RpFunc, PCI_PBUS);

    SubBus = MmioRead32 (TbtRp);
    //
    // Set Sec/Sub buses to 0xF0
    //
// AMI_OVERRIDE_START - Avoid this address for OEM used.
#if 0
    MmioWrite32 (TbtRp, 0x00F0F000);
#else
    MmioWrite32 (TbtRp, 0x003F3F00);
#endif
// AMI_OVERRIDE_END - Avoid this address for OEM used.

// AMI_OVERRIDE_START - EIP279674 : In DEBUG mode, we will find TBT boot command failed in PEI.
    /*
    if(TbtBootOn > 0) {
    TbtBaseAddress = MmPciBase(0xF0, 0, 0);
    //
    // Driver clears the PCIe2TBT Valid bit to support two consicutive mailbox commands
    //
    MmioWrite32(TbtBaseAddress + PCIE2TBT_R, 0);
    DEBUG((DEBUG_INFO, "TbtBaseAddress + PCIE2TBT_R = 0x%lx \n", TbtBaseAddress + PCIE2TBT_R));
    while (MaxWaitIter-- > 0) {
      RegVal = MmioRead32(TbtBaseAddress + TBT2PCIE_R);
      if (0xFFFFFFFF == RegVal) {
        //
        // Device is not here return now
        //
        DEBUG((DEBUG_INFO, "TBT device is not present \n"));
        IoWrite8(0x80, 0x9F);
        break;
      }

      if (!(RegVal & TBT2PCIE_DON_R)) {
        IoWrite8(0x80, 0x91);
        break;
      }
      MicroSecondDelay(100 * 1000);
    }
  } */
// AMI_OVERRIDE_END - EIP279674 : In DEBUG mode, we will find TBT boot command failed in PEI.

    //
    // Exectue Mail box command "Usb On"
    //
    //Command may be executed only during boot/reboot and not during S3 exit flow
    //In case of S4 Exit send USB ON cmd only if Host Router was inactive/not present during S4 entry
    if (BootMode != BOOT_ON_S3_RESUME) {
// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM.
#if 0
      if ((BootMode == BOOT_ON_S4_RESUME) && IsHostRouterPresentBeforeSleep()) {
#else
      if ((BootMode == BOOT_ON_S4_RESUME) && IsHostRouterPresentBeforeSleep(&HrStatusData)) {
#endif
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM.
        // USB_ON cmd not required
      } else {
//AMI_OVERRIDE_START >>> EIP353107 : Boot On command has the fail rate
        MicroSecondDelay ((UINTN)(TbtBeforeUsbOnCmdDelay * 1000));
//AMI_OVERRIDE_END <<< EIP353107 : Boot On command has the fail rate
        if (!TbtSetPCIe2TbtCommand (PCIE2TBT_USB_ON)) {
          //
          // Nothing to wait, HR is not responsive
          //
          DEBUG ((DEBUG_INFO, "<TbtPei> TbtBootSupport - Usb On message sent failed \n"));
        }
      }
    }
    //
    // Reset Sec/Sub buses to original value
    //
    MmioWrite32 (TbtRp, SubBus);
  }


//AMI_OVERRIDE_START >>> EIP340393 : Follow AR BWG 1.35 to remove SPR support and update Sx Exit Delay
  //
  //During boot, reboot and wake  (bits [1:0]) of PCH PM_CFG register should be
  //set to 11b - 10 ms (default value is 0b - 10 us)
  //
  PchPwrmBaseGet (&PchPwrmBase);
  MmioOr32 (
    (UINTN) (PchPwrmBase + R_PCH_PWRM_CFG),
    (BIT0 | BIT1)
    );
//AMI_OVERRIDE_END <<< EIP340393 : Follow AR BWG 1.35 to remove SPR support and update Sx Exit Delay


  if (Gpio3ForcePwr) {
    DEBUG ((DEBUG_INFO, "<TbtPei> ForcePwrGpio.GpioNumber = %x \n", PlatformBoardConfig->ForcePwrGpio.GpioNumber));
    ForcePower (PlatformBoardConfig->ForcePwrGpio.GpioAccessType, PlatformBoardConfig->ForcePwrGpio.Expander, PlatformBoardConfig->ForcePwrGpio.GpioNumber, PlatformBoardConfig->ForcePwrGpio.GpioLevel);
    DEBUG ((DEBUG_INFO, "ForcePower asserted \n"));
    MicroSecondDelay (Gpio3ForcePwrDly * 1000);
    DEBUG ((DEBUG_INFO, "Delay after ForcePower = 0x%x ms \n", Gpio3ForcePwrDly));
  }

  //
  // VGA Enable and VGA 16-bit decode registers of Bridge control register of Root port where
  // Host router resides should be cleaned
  //
  //
  // Get the TBT Root Port Base Address
  //
  Status = GetTbtRpDevFun (TbtSelector - 1, &RpDev, &RpFunc);
  ASSERT_EFI_ERROR (Status);
  HrBaseAddress = MmPciBase (0x00, (UINT32)RpDev, (UINT32)RpFunc);
  Data16        = MmioRead16 (HrBaseAddress + PCI_BRIDGE_CONTROL_REGISTER_OFFSET);
  Data16 &= (~(EFI_PCI_BRIDGE_CONTROL_VGA | EFI_PCI_BRIDGE_CONTROL_VGA_16));
  MmioWrite16 (HrBaseAddress + PCI_BRIDGE_CONTROL_REGISTER_OFFSET, Data16);

  Status = PeiServicesGetBootMode (&BootMode);

// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM.
#if 0
  if ((IsHostRouterPresentBeforeSleep () || TbtBootOn == 2 ) && ((BootMode == BOOT_ON_S3_RESUME || BootMode == BOOT_ON_S4_RESUME))) {
#else
  if (IsHostRouterPresentBeforeSleep (&HrStatusData) && ((BootMode == BOOT_ON_S3_RESUME || BootMode == BOOT_ON_S4_RESUME))) {
#endif
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM.

//AMI_OVERRIDE_START - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.
    if (BootMode == BOOT_ON_S4_RESUME && !TbtUsbOn && !TbtBootOn) {
      //
      // Skip to execute "Sx_Exit_TBT_Connected" mailbox command in PEI phase when S4 resume.
      // After PCI connect, do it in TbtDxeInvokeSmiHandler().
      //
      return EFI_SUCCESS;
    }
//AMI_OVERRIDE_END - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.

//AMI_OVERRIDE_START >>> EIP340393 : Follow AR BWG 1.35 to remove SPR support and update Sx Exit Delay
	//
    // AR BWG 1.35:
    // 2.2.3.2 Sx Exit Delay:
    //   BIOS should add 500 ms delay befor proceeding to Sx Exit Flow.
    //
//AMI_OVERRIDE_START >>> EIP353107 : Boot On command has the fail rate
    MicroSecondDelay ((UINTN)(TbtBeforeSxExitCmdDelay * 1000));
//AMI_OVERRIDE_END <<< EIP353107 : Boot On command has the fail rate
//AMI_OVERRIDE_END <<< EIP340393 : Follow AR BWG 1.35 to remove SPR support and update Sx Exit Delay

    //
    // Upon wake, if BIOS saved pre-Sx Host Router state as active (system went to sleep with
    // attached devices), BIOS should:
    // 1. Execute "Sx_Exit_TBT_Connected" mailbox command.
    // 2. If procedure above returns true, BIOS should perform "wait for fast link bring-up" loop
    // 3. Continue regular wake flow.
    //
    TbtRp = MmPCIeAddress (0x00, RpDev, RpFunc, PCI_PBUS);
    SubBus = MmioRead32 (TbtRp);
    //
    // Set Sec/Sub buses to 0xF0
    //
// AMI_OVERRIDE_START - Avoid this address for OEM used.
#if 0
    MmioWrite32 (TbtRp, 0x00F0F000);
#else
    MmioWrite32 (TbtRp, 0x003F3F00);
#endif
// AMI_OVERRIDE_END - Avoid this address for OEM used.

    //
    // Exectue Mail box command and perform "wait for fast link bring-up" loop
    //
    TbtSetSxMode ();
    //
    // Reset Sec/Sub buses to original value
    //
    MmioWrite32 (TbtRp, SubBus);
  }
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  if (Multicontroller) {
    TbtSelector = PlatformBoardConfig->TbtDefaultSelectorInfo1;
    DEBUG ((DEBUG_INFO, "<TbtPei> TbtSelector = %x \n", TbtSelector));
      if (TbtBootOn > 0) {
        Status = GetTbtRpDevFun (TbtSelector -1, &RpDev, &RpFunc);
        ASSERT_EFI_ERROR(Status);
        TbtRp = MmPCIeAddress(0x00, RpDev, RpFunc, PCI_PBUS);
        SubBus = MmioRead32 (TbtRp);
        //
        // Set Sec/Sub buses to 0xF0
        //
        MmioWrite32 (TbtRp, 0x00F0F000);

        //
        // Exectue Mail box command "Boot On "
        //
        //Command may be executed only during boot/reboot and not during Sx exit flow
        if (((BootMode != BOOT_ON_S2_RESUME) && (BootMode != BOOT_ON_S3_RESUME) &&
        (BootMode != BOOT_ON_S4_RESUME))) {
        if (TbtBootOn == 1) {
          if (!TbtSetPCIe2TbtCommand (PCIE2TBT_BOOT_ON)) {
            //
            // Nothing to wait, HR is not responsive
            //
            DEBUG ((DEBUG_INFO, "<TbtPei> TbtBootSupport - Boot On message sent failed \n"));
          }
        }
        if (TbtBootOn == 2) {
          if (!TbtSetPCIe2TbtCommand (PCIE2TBT_PREBOOTACL)) {
            //
            // Nothing to wait, HR is not responsive
            //
            DEBUG ((DEBUG_INFO, "<TbtPei> TbtBootSupport - Preboot ACL message sent failed \n"));
          }        
        }
      }


      //
      // Reset Sec/Sub buses to original value
      //
      MmioWrite32 (TbtRp, SubBus);
    }

    //
    //When Thunderbolt(TM) Usb boot [TbtUsbOn] is enabled in bios setup we need to do the below:
    //   Bios should send "Usb On" message through PCIE2TBT register
    //   The Usb On command as described above would include the command and acknowledge from FW (with the default timeout in BIOS),
    //   once the Usb On command is completed it is guaranteed that the AlpineRidge(AR) device is there and the PCI tunneling was done by FW,
    //   next step from BIOS is enumeration using SMI
    //

    if (TbtUsbOn) {
      Status = GetTbtRpDevFun (TbtSelector - 1, &RpDev, &RpFunc);
      ASSERT_EFI_ERROR (Status);
      TbtRp = MmPCIeAddress (0x00, RpDev, RpFunc, PCI_PBUS);

      SubBus = MmioRead32 (TbtRp);
      //
      // Set Sec/Sub buses to 0xF0
      //
      MmioWrite32 (TbtRp, 0x00F0F000);

      if(TbtBootOn > 0) {
      TbtBaseAddress = MmPciBase(0xF0, 0, 0);
      //
      // Driver clears the PCIe2TBT Valid bit to support two consicutive mailbox commands
      //
      MmioWrite32(TbtBaseAddress + PCIE2TBT_R, 0);
      DEBUG((DEBUG_INFO, "TbtBaseAddress + PCIE2TBT_R = 0x%lx \n", TbtBaseAddress + PCIE2TBT_R));
      while (MaxWaitIter-- > 0) {
        RegVal = MmioRead32(TbtBaseAddress + TBT2PCIE_R);
        if (0xFFFFFFFF == RegVal) {
          //
          // Device is not here return now
          //
          DEBUG((DEBUG_INFO, "TBT device is not present \n"));
          break;
        }

        if (!(RegVal & TBT2PCIE_DON_R)) {
          break;
        }
        MicroSecondDelay(100 * 1000);
      }
    }
      //
      // Exectue Mail box command "Usb On"
      //
      //Command may be executed only during boot/reboot and not during S3 exit flow
      //In case of S4 Exit send USB ON cmd only if Host Router was inactive/not present during S4 entry
      if (BootMode != BOOT_ON_S3_RESUME) {
        if ((BootMode == BOOT_ON_S4_RESUME) && IsHostRouterPresentBeforeSleep()) {
          // USB_ON cmd not required
        } else {
          if (!TbtSetPCIe2TbtCommand (PCIE2TBT_USB_ON)) {
            //
            // Nothing to wait, HR is not responsive
            //
            DEBUG ((DEBUG_INFO, "<TbtPei> TbtBootSupport - Usb On message sent failed \n"));
          }
        }
      }
      //
      // Reset Sec/Sub buses to original value
      //
      MmioWrite32 (TbtRp, SubBus);
    }

    //
    //During boot, reboot and wake  (bits [1:0]) of PCH PM_CFG register should be
    //set to 11b - 10 ms (default value is 0b - 10 us)
    //
    PchPwrmBaseGet (&PchPwrmBase);
    MmioOr32 (
      (UINTN) (PchPwrmBase + R_PCH_PWRM_CFG),
      (BIT0 | BIT1)
      );

    if (Gpio3ForcePwr1) {
      DEBUG ((DEBUG_INFO, "<TbtPei> ForcePwrGpio1.GpioNumber = %x \n", PlatformBoardConfig->ForcePwrGpio1.GpioNumber));
      ForcePower (PlatformBoardConfig->ForcePwrGpio1.GpioAccessType, PlatformBoardConfig->ForcePwrGpio1.Expander, PlatformBoardConfig->ForcePwrGpio1.GpioNumber, PlatformBoardConfig->ForcePwrGpio1.GpioLevel);
      DEBUG ((DEBUG_INFO, "ForcePower asserted \n"));
      MicroSecondDelay (Gpio3ForcePwrDly * 1000);
      DEBUG ((DEBUG_INFO, "Delay after ForcePower = 0x%x ms \n", Gpio3ForcePwrDly));
    }

    //
    // VGA Enable and VGA 16-bit decode registers of Bridge control register of Root port where
    // Host router resides should be cleaned
    //
    //
    // Get the TBT Root Port Base Address
    //
    Status = GetTbtRpDevFun (TbtSelector - 1, &RpDev, &RpFunc);
    ASSERT_EFI_ERROR (Status);
    HrBaseAddress = MmPciBase (0x00, (UINT32)RpDev, (UINT32)RpFunc);
    Data16        = MmioRead16 (HrBaseAddress + PCI_BRIDGE_CONTROL_REGISTER_OFFSET);
    Data16 &= (~(EFI_PCI_BRIDGE_CONTROL_VGA | EFI_PCI_BRIDGE_CONTROL_VGA_16));
    MmioWrite16 (HrBaseAddress + PCI_BRIDGE_CONTROL_REGISTER_OFFSET, Data16);

    Status = PeiServicesGetBootMode (&BootMode);
    if ((Status != EFI_SUCCESS) || (!(BootMode == BOOT_ON_S3_RESUME || BootMode == BOOT_ON_S4_RESUME))) {
      return EFI_SUCCESS;
    }

    if (IsHostRouterPresentBeforeSleep ()) {
      //
      // Upon wake, if BIOS saved pre-Sx Host Router state as active (system went to sleep with
      // attached devices), BIOS should:
      // 1. Execute "Sx_Exit_TBT_Connected" mailbox command.
      // 2. If procedure above returns true, BIOS should perform "wait for fast link bring-up" loop
      // 3. Continue regular wake flow.
      //
      TbtRp = MmPCIeAddress (0x00, RpDev, RpFunc, PCI_PBUS);
      SubBus = MmioRead32 (TbtRp);
      //
      // Set Sec/Sub buses to 0xF0
      //
      MmioWrite32 (TbtRp, 0x00F0F000);

      //
      // Exectue Mail box command and perform "wait for fast link bring-up" loop
      //
      TbtSetSxMode ();
      //
      // Reset Sec/Sub buses to original value
      //
      MmioWrite32 (TbtRp, SubBus);
    }
  }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
  return EFI_SUCCESS;
}
