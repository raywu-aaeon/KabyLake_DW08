/** @file
  TBT SMM phase - Assign PCIe resource to Thunderbolt devices

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

//
// Module specific Includes
//
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/GpioLib.h>
#include <TbtBoardInfo.h>
#include <Protocol/GlobalNvsArea.h>
#include <CmosMap.h>
#include <Protocol/SmmVariable.h>
#include <PchAccess.h>
#include <Library/MmPciLib.h>
#include <Library/GpioExpanderLib.h>
#include <Library/PchInfoLib.h>
#include <Library/IoLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Uefi/UefiSpec.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Guid/HobList.h>
#include <Library/CmosAccessLib.h>
#include "TbtSmiHandler.h"
#include <PchAccess.h>
#include <Protocol/SaPolicy.h>

#define P2P_BRIDGE                    (((PCI_CLASS_BRIDGE) << 8) | (PCI_CLASS_BRIDGE_P2P))

#define CMD_BM_MEM_IO                 (CMD_BUS_MASTER | BIT1 | BIT0)

#define DISBL_IO_REG1C                0x01F1
#define DISBL_MEM32_REG20             0x0000FFF0
#define DISBL_PMEM_REG24              0x0001FFF1

#define DOCK_BUSSES                   8

#define DEF_CACHE_LINE_SIZE           0x20
#define DEF_RES_IO_PER_DEV            4
#define DEF_RES_MEM_PER_DEV           32
#define DEF_RES_PMEM_PER_DEV          32

#define PCI_CAPABILITY_ID_PCIEXP      0x10
#define PCI_CAPBILITY_POINTER_OFFSET  0x34
#define GPIO_SKL_LP_GPP_G4      0x02060004
#define GPIO_SKL_LP_GPP_G5      0x02060005

// AMI_OVERRIDE_START - Following the EIP279338 The resource for PEG Thunderbolt host is incorrect
#define PCH_PCIE_NUM  (24)
// AMI_OVERRIDE_END - Following the EIP279338 The resource for PEG Thunderbolt host is incorrect

// AMI_OVERRIDE_START - EIP313994 - Unable to change MEM & PMEM size for each TBT slot
//STATIC SETUP_DATA                                         SysConfig = { 0 };
SETUP_DATA                                                SysConfig = { 0 };
// AMI_OVERRIDE_END - EIP313994 - Unable to change MEM & PMEM size for each TBT slot
STATIC TBT_INFO_HOB                                       *gTbtInfoHob;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA         *mGlobalNvsAreaPtr;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                       gDefTbtSelector;
GLOBAL_REMOVE_IF_UNREFERENCED SETUP_DATA                  *mSysConfig;
// AMI_OVERRIDE_START - Fix first tbt hot-plug fail.
UINT8                                                     IsFirstEnterFlag = 1;
// AMI_OVERRIDE_END - Fix first tbt hot-plug fail.
// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_VARIABLE_PROTOCOL   *mSmmVariable;
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                      TbtLtrMaxSnoopLatency;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                      TbtLtrMaxNoSnoopLatency;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS        gTbtDebugBaseAddressMem = 0;
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
//AMI_OVERRIDE_START >>> EIP426038 - Fix build failed with new GCC flags
//GLOBAL_REMOVE_IF_UNREFERENCED UINT8 TbtXConnectSupport;
extern UINT8 TbtXConnectSupport;
//AMI_OVERRIDE_END <<< EIP426038 - Fix build failed with new GCC flags
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS  gTbtDgpuPciBaseAddressMem = 0;

#define TBT_DEBUG_AREA_SIZE_IN_KB     64

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS gTbtDeviceTreeBuffer = 0; // Tbt Device Tree buffer


STATIC UINT32               PDebugAddressConf = 0x32AB7000;
STATIC UINT32               *PDebugAddress    = &PDebugAddressConf;

/**
  GPIO write function

  @param  GpioAccessType        Type01: call GpioSetOutputValue;                    Type02: call GpioExpSetOutput
  @param  Expander              Type01: don't care;                                 Type02: Expander Value with in the Contoller
  @param  GpioNumber            Type01: GPIO pad;                                   Type02: Pin with in the Expnader Value
  @param  Value                 Type01: Output value: 0: OutputLow, 1: OutputHigh;  Type02: none

  @retval VOID
**/
VOID
GpioWrite (
  IN  UINT8                     GpioAccessType,
  IN  UINT8                     Expander,
  IN  UINT32                    GpioNumber,
  IN  BOOLEAN                   Value
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
  Search and return the offset of desired Pci Express Capability ID
  CAPID list:
    0x0001 = Advanced Error Reporting Capability
    0x0002 = Virtual Channel Capability
    0x0003 = Device Serial Number Capability
    0x0004 = Power Budgeting Capability

  @param  Bus                   Pci Bus Number
  @param  Device                Pci Device Number
  @param  Function              Pci Function Number
  @param  CapId                 Extended CAPID to search for

  @retval 0                     CAPID not found
  @retval Other                 CAPID found, Offset of desired CAPID
**/
UINT16
PcieFindExtendedCapId (
  IN UINT8                      Bus,
  IN UINT8                      Device,
  IN UINT8                      Function,
  IN UINT16                     CapId
  )
{
  UINT16                        CapHeaderOffset;
  UINT16                        CapHeaderId;
  UINTN                         DeviceBase;

  DeviceBase = MmPciBase (Bus, Device, Function);

  ///
  /// Start to search at Offset 0x100
  /// Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
  ///
  CapHeaderId     = 0;
  CapHeaderOffset = 0x100;
  while (CapHeaderOffset != 0 && CapHeaderId != 0xFFFF) {
    CapHeaderId = MmioRead16 (DeviceBase + CapHeaderOffset);
    if (CapHeaderId == CapId) {
      return CapHeaderOffset;
    }
    ///
    /// Each capability must be DWORD aligned.
    /// The bottom two bits of all pointers are reserved and must be implemented as 00b
    /// although software must mask them to allow for future uses of these bits.
    ///
    CapHeaderOffset = (MmioRead16 (DeviceBase + CapHeaderOffset + 2) >> 4) & ((UINT16) ~(BIT0 | BIT1));
  }

  return 0;
}

/**
  Find the Offset to a given Capabilities ID
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability

  @param  Bus                   Pci Bus Number
  @param  Device                Pci Device Number
  @param  Function              Pci Function Number
  @param  CapId                 CAPID to search for

  @retval 0                     CAPID not found
  @retval Other                 CAPID found, Offset of desired CAPID
**/
UINT8
PcieFindCapId (
  IN UINT8                      Bus,
  IN UINT8                      Device,
  IN UINT8                      Function,
  IN UINT8                      CapId
  )
{
  UINT8                         CapHeaderOffset;
  UINT8                         CapHeaderId;
  UINTN                         DeviceBase;

  DeviceBase = MmPciBase (Bus, Device, Function);

  if ((MmioRead8 (DeviceBase + PCI_PRIMARY_STATUS_OFFSET) & EFI_PCI_STATUS_CAPABILITY) == 0x00) {
    ///
    /// Function has no capability pointer
    ///
    return 0;
  }

  ///
  /// Check the header layout to determine the Offset of Capabilities Pointer Register
  ///
  if ((MmioRead8 (DeviceBase + PCI_HEADER_TYPE_OFFSET) & HEADER_LAYOUT_CODE) == (HEADER_TYPE_CARDBUS_BRIDGE)) {
    ///
    /// If CardBus bridge, start at Offset 0x14
    ///
    CapHeaderOffset = 0x14;
  } else {
    ///
    /// Otherwise, start at Offset 0x34
    ///
    CapHeaderOffset = 0x34;
  }
  ///
  /// Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
  ///
  CapHeaderId     = 0;
  CapHeaderOffset = MmioRead8 (DeviceBase + CapHeaderOffset) & ((UINT8) ~(BIT0 | BIT1));
  while (CapHeaderOffset != 0 && CapHeaderId != 0xFF) {
    CapHeaderId = MmioRead8 (DeviceBase + CapHeaderOffset);
    if (CapHeaderId == CapId) {
      return CapHeaderOffset;
    }
    ///
    /// Each capability must be DWORD aligned.
    /// The bottom two bits of all pointers (including the initial pointer at 34h) are reserved
    /// and must be implemented as 00b although software must mask them to allow for future uses of these bits.
    ///
    CapHeaderOffset = MmioRead8 (DeviceBase + CapHeaderOffset + 1) & ((UINT8) ~(BIT0 | BIT1));
  }

  return 0;
}

/**
  Set ASPM to multi-function by lower ASPM level

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number

  @return                       No return value
**/
VOID
MultiFunctionDeviceAspm (
  IN UINT8                      Bus,
  IN UINT8                      Dev
  )
{
  UINT16                        LowerAspm;
  UINT16                        AspmVal;
  UINT8                         Fun;
  UINTN                         DeviceBaseAddress;
  UINT8                         CapHeaderOffset;

  LowerAspm = 3; // L0s and L1 Supported
  for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
    //
    // Check for Device availability
    //
    DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
    if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
      // Device not present
      continue;
    }

    CapHeaderOffset = PcieFindCapId (Bus, Dev, Fun, 0x10);

    AspmVal = (MmioRead16 (DeviceBaseAddress + CapHeaderOffset + 0x00C) >> 10) & 3;
    if (LowerAspm > AspmVal) {
      LowerAspm = AspmVal;
    }
  } //Fun

  for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
    //
    // Check for Device availability
    //
    DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
    if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
      //
      // Device not present
      //
      continue;
    }

    CapHeaderOffset = PcieFindCapId (Bus, Dev, Fun, 0x10);

    MmioAndThenOr16 (DeviceBaseAddress + CapHeaderOffset + 0x10, 0xFFFC, LowerAspm);
  } //Fun
}

/**
  Compare SelectedAspm and MaxAspmLevel, and return the lower ASPM level

  @param  SelectedAspm          Selected ASPM level
  @param  MaxAspmLevel          MAX ASPM level

  @return                       ASPM level
**/
UINT16
LimitAspmLevel (
  IN UINT16                     SelectedAspm,
  IN UINT16                     MaxAspmLevel
  )
{
  SelectedAspm = SelectedAspm & MaxAspmLevel;

  return SelectedAspm;
}

/**
  Compare ComponentA ASPM and ComponentB ASPM, and return the lower ASPM level

  @param  ComponentAaspm        ComponentA ASPM
  @param  ComponentBaspm        ComponentB ASPM

  @return                       ASPM level
**/
UINT16
FindOptimalAspm (
  IN UINT16                     ComponentAaspm,
  IN UINT16                     ComponentBaspm
  )
{
  UINT16                        SelectedAspm;

  SelectedAspm = ComponentAaspm & ComponentBaspm;

  return SelectedAspm;
}

/**
  Return the rootbridge ASPM level from the specific BUS

  @param  Bus                   PCI bus number
  @param  MaxBus                MAX support BUS

  @return                       ASPM level
**/
UINT16
FindComponentBaspm (
  IN UINT8                      Bus,
  IN UINT8                      MaxBus
  )
{
  UINT8                         BusNo;
  UINT8                         DevNo;
  UINT8                         FunNo;
  UINTN                         DevBaseAddress;
  UINT8                         RegVal;
  UINT8                         SecBusNo;
  UINT16                        SelectedAspm; // No ASPM Support
  UINT8                         CapHeaderOffset_B;
  BOOLEAN                       AspmFound;

  SelectedAspm  = 0;
  AspmFound     = FALSE;

  for (BusNo = MaxBus; (BusNo != 0xFF) && (!AspmFound); --BusNo) {
    for (DevNo = 0; (DevNo <= PCI_MAX_DEVICE) && (!AspmFound); ++DevNo) {
      for (FunNo = 0; (FunNo <= PCI_MAX_FUNC) && (!AspmFound); ++FunNo) {
        //
        // Check for Device availability
        //
        DevBaseAddress = MmPciBase (BusNo, DevNo, FunNo);
        if (INVALID_PCI_DEVICE == MmioRead32 (DevBaseAddress + PCI_VENDOR_DEV_ID)) {
          //
          // Device not present
          //
          continue;
        }

        RegVal = MmioRead8 (DevBaseAddress + PCI_HEADER_TYPE);
        if ((RegVal & (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6)) != 0x01) {
          //
          // Not a PCI-to-PCI bridges device
          //
          continue;
        }

        SecBusNo = MmioRead8 (DevBaseAddress + PCI_SEC_BUS_NUM);

        if (SecBusNo == Bus) {
          //
          // This is the Rootbridge for the given 'Bus' device
          //
          CapHeaderOffset_B = PcieFindCapId (BusNo, DevNo, FunNo, 0x10);
          SelectedAspm      = (MmioRead16 (DevBaseAddress + CapHeaderOffset_B + 0x00C) >> 10) & 3;
          AspmFound         = TRUE;
        }
      } //FunNo
    } //DevNo
  } //BusNo

  return (SelectedAspm);
}

/**
  Disable ASPM support to specific Bx:Dx:Fx

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number
  @param  CapHeaderOffset       PCI Express capability offset

  @return                       No return value
**/
VOID
NoAspmSupport (
  IN UINT8                      Bus,
  IN UINT8                      Dev,
  IN UINT8                      Fun,
  IN UINT8                      CapHeaderOffset
  )
{
  UINTN                         DeviceBaseAddress;

  DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
  MmioAndThenOr16 (DeviceBaseAddress + CapHeaderOffset + 0x10, 0xFFFC, 0x00);
}

/**
  Enable ASPM support to specific Bx:Dx:Fx endpoint

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number
  @param  CapHeaderOffset       PCI Express capability offset
  @param  MaxBus                MAX support BUS
  @param  MaxAspmLevel          Max ASPM level, value from setup page

  @return                       No return value
**/
VOID
EndpointAspmSupport (
  IN UINT8                      Bus,
  IN UINT8                      Dev,
  IN UINT8                      Fun,
  IN UINT8                      CapHeaderOffset,
  IN UINT8                      MaxBus,
  IN UINT16                     MaxAspmLevel
  )
{
  UINTN                         DeviceBaseAddress;
  UINT16                        ComponentAaspm;
  UINT16                        ComponentBaspm;
  UINT16                        SelectedAspm;

  DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
  ComponentAaspm    = (MmioRead16 (DeviceBaseAddress + CapHeaderOffset + 0x00C) >> 10) & 3;
  ComponentBaspm    = FindComponentBaspm (Bus, MaxBus);
  SelectedAspm      = FindOptimalAspm (ComponentAaspm, ComponentBaspm);
  SelectedAspm      = LimitAspmLevel (SelectedAspm, MaxAspmLevel);
  MmioAndThenOr16 (DeviceBaseAddress + CapHeaderOffset + 0x10, 0xFFFC, SelectedAspm);
}

/**
  Enable ASPM support to specific Bx:Dx:Fx Upstream Port of PCI Express Switch

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number
  @param  CapHeaderOffset       PCI Express capability offset
  @param  MaxBus                MAX support BUS
  @param  MaxAspmLevel          Max ASPM level, value from setup page

  @return                       No return value
**/
VOID
UpstreamAspmSupport (
  IN UINT8                      Bus,
  IN UINT8                      Dev,
  IN UINT8                      Fun,
  IN UINT8                      CapHeaderOffset,
  IN UINT8                      MaxBus,
  IN UINT16                     MaxAspmLevel
  )
{
  UINTN                         DeviceBaseAddress;
  UINT16                        ComponentAaspm;
  UINT16                        ComponentBaspm;
  UINT16                        SelectedAspm;

  DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
  ComponentAaspm    = (MmioRead16 (DeviceBaseAddress + CapHeaderOffset + 0x00C) >> 10) & 3;
  ComponentBaspm    = FindComponentBaspm (Bus, MaxBus);
  SelectedAspm      = FindOptimalAspm (ComponentAaspm, ComponentBaspm);
  SelectedAspm      = LimitAspmLevel (SelectedAspm, MaxAspmLevel);
  MmioAndThenOr16 (DeviceBaseAddress + CapHeaderOffset + 0x10, 0xFFFC, SelectedAspm);
}

/**
  Enable ASPM support to specific Bx:Dx:Fx Downstream Port of PCI Express Switch

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number
  @param  CapHeaderOffset       PCI Express capability offset
  @param  MaxAspmLevel          Max ASPM level, value from setup page

  @return                       No return value
**/
VOID
DownstreamAspmSupport (
  IN UINT8                      Bus,
  IN UINT8                      Dev,
  IN UINT8                      Fun,
  IN UINT8                      CapHeaderOffset,
  IN UINT16                     MaxAspmLevel
  )
{
  UINTN                         ComponentABaseAddress;
  UINTN                         ComponentBBaseAddress;
  UINT16                        ComponentAaspm;
  UINT16                        ComponentBaspm;
  UINT16                        SelectedAspm;
  UINT8                         SecBus;
  UINT8                         CapHeaderOffset_B;

  ComponentABaseAddress = MmPciBase (Bus, Dev, Fun);
  ComponentAaspm        = (MmioRead16 (ComponentABaseAddress + CapHeaderOffset + 0x00C) >> 10) & 3;

  SecBus                = MmioRead8 (ComponentABaseAddress + PCI_SEC_BUS_NUM);
  ComponentBBaseAddress = MmPciBase (SecBus, 0, 0);
  ComponentBaspm        = 0; // No ASPM Support
  if (INVALID_PCI_DEVICE != MmioRead32 (ComponentBBaseAddress + PCI_VENDOR_DEV_ID)) {
    CapHeaderOffset_B = PcieFindCapId (SecBus, 0, 0, 0x10);
    ComponentBaspm    = (MmioRead16 (ComponentBBaseAddress + CapHeaderOffset_B + 0x00C) >> 10) & 3;
  }

  SelectedAspm = FindOptimalAspm (ComponentAaspm, ComponentBaspm);
  SelectedAspm = LimitAspmLevel (SelectedAspm, MaxAspmLevel);
  MmioAndThenOr16 (ComponentABaseAddress + CapHeaderOffset + 0x10, 0xFFFC, SelectedAspm);
}

/**
  Enable ASPM support to specific Bx:Dx:Fx Root Port of PCI Express Root Complex

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number
  @param  CapHeaderOffset       PCI Express capability offset
  @param  MaxAspmLevel          Max ASPM level, value from setup page

  @return                       No return value
**/
VOID
RootportAspmSupport (
  IN UINT8                      Bus,
  IN UINT8                      Dev,
  IN UINT8                      Fun,
  IN UINT8                      CapHeaderOffset,
  IN UINT16                     MaxAspmLevel
  )
{
  UINTN                         ComponentABaseAddress;
  UINTN                         ComponentBBaseAddress;
  UINT16                        ComponentAaspm;
  UINT16                        ComponentBaspm;
  UINT16                        SelectedAspm;
  UINT8                         SecBus;
  UINT8                         CapHeaderOffset_B;

  ComponentABaseAddress = MmPciBase (Bus, Dev, Fun);
  ComponentAaspm        = (MmioRead16 (ComponentABaseAddress + CapHeaderOffset + 0x00C) >> 10) & 3;

  SecBus                = MmioRead8 (ComponentABaseAddress + PCI_SEC_BUS_NUM);
  ComponentBBaseAddress = MmPciBase (SecBus, 0, 0);
  ComponentBaspm        = 0; // No ASPM Support
  if (INVALID_PCI_DEVICE != MmioRead32 (ComponentBBaseAddress + PCI_VENDOR_DEV_ID)) {
    CapHeaderOffset_B = PcieFindCapId (SecBus, 0, 0, 0x10);
    ComponentBaspm    = (MmioRead16 (ComponentBBaseAddress + CapHeaderOffset_B + 0x00C) >> 10) & 3;
  }

  SelectedAspm = FindOptimalAspm (ComponentAaspm, ComponentBaspm);
  SelectedAspm = LimitAspmLevel (SelectedAspm, MaxAspmLevel);
  MmioAndThenOr16 (ComponentABaseAddress + CapHeaderOffset + 0x10, 0xFFFC, SelectedAspm);
}

/**
  Enable ASPM support for all the downstreams ports and endpoints devices

  @param  MaxAspmLevel          Max ASPM level, value from setup page
  @param  RpNumber              Root port number

  @return                       No return value
**/
VOID
ThunderboltEnableAspmWithoutLtr (
 IN  UINT16                     MaxAspmLevel,
 IN  UINT32                     RpNumber
  )
{
  UINT8                         Bus;
  UINT8                         Dev;
  UINT8                         Fun;
  UINT8                         RootBus;
  UINT8                         RootDev;
  UINT8                         RootFun;
  UINT8                         MinBus;
  UINT8                         MaxBus;
  UINT16                        DeviceId;
  UINTN                         DeviceBaseAddress;
  UINT8                         RegVal;
  UINT8                         CapHeaderOffset;
  UINT16                        DevicePortType;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  EFI_STATUS                    Status;

  MinBus = 0;
  MaxBus = 0;

  Status = GetTbtRpDevFun (RpNumber - 1, &RpDev, &RpFunc);
  MinBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev ,(UINT32)  RpFunc) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  MaxBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev, (UINT32) RpFunc) + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
  DeviceId  = MmioRead16 (MmPciBase (MinBus, 0x00, 0x00) + PCI_DEVICE_ID_OFFSET);

  RootBus = (UINT8) 0x00;
  RootDev = (UINT8) RpDev;
  RootFun = (UINT8) RpFunc;

  //
  //  Enumerate all the bridges and devices which are available on TBT host controller
  //
  for (Bus = MinBus; Bus <= MaxBus; ++Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      //
      // Check for Device availability
      //
      DeviceBaseAddress = MmPciBase (Bus, Dev, 0);
      if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
        //
        // Device not present
        //
        continue;
      }

      RegVal = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE);
      if ((RegVal & BIT7) == 0) {
        //
        // Not a multi-function device
        //
        continue;
      }

      MultiFunctionDeviceAspm (Bus, Dev);
    } //Dev
  } //Bus

  for (Bus = MinBus; Bus <= MaxBus; ++Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        //
        // Check for Device availability
        //
        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
          //
          // Device not present
          //
          continue;
        }

        CapHeaderOffset = PcieFindCapId (Bus, Dev, Fun, 0x10);
        DevicePortType  = (MmioRead16 (DeviceBaseAddress + CapHeaderOffset + 0x002) >> 4) & 0xF;
        if (MmioRead16 (DeviceBaseAddress + PCI_CLASSCODE_OFFSET) == PCI_CLASS_SERIAL) {
          MaxAspmLevel = (UINT16) 0x1;
        }

        switch (DevicePortType) {
        case 0:
          //
          // PCI Express Endpoint
          //
          EndpointAspmSupport (Bus, Dev, Fun, CapHeaderOffset, MaxBus, MaxAspmLevel);
          break;

        case 1:
          //
          // Legacy PCI Express Endpoint
          //
          EndpointAspmSupport (Bus, Dev, Fun, CapHeaderOffset, MaxBus, MaxAspmLevel);
          break;

        case 4:
          //
          // Root Port of PCI Express Root Complex
          //
          RootportAspmSupport (Bus, Dev, Fun, CapHeaderOffset, MaxAspmLevel);
          break;

        case 5:
          //
          // Upstream Port of PCI Express Switch
          //
          UpstreamAspmSupport (Bus, Dev, Fun, CapHeaderOffset, MaxBus, MaxAspmLevel);
          break;

        case 6:
          //
          // Downstream Port of PCI Express Switch
          //
          DownstreamAspmSupport (Bus, Dev, Fun, CapHeaderOffset, MaxAspmLevel);
          break;

        case 7:
          //
          // PCI Express to PCI/PCI-X Bridge
          //
          NoAspmSupport (Bus, Dev, Fun, CapHeaderOffset);
          break;

        case 8:
          //
          // PCI/PCI-X to PCI Express Bridge
          //
          NoAspmSupport (Bus, Dev, Fun, CapHeaderOffset);
          break;

        case 9:
          //
          // Root Complex Integrated Endpoint
          //
          EndpointAspmSupport (Bus, Dev, Fun, CapHeaderOffset, MaxBus, MaxAspmLevel);
          break;

        case 10:
          //
          // Root Complex Event Collector
          //
          EndpointAspmSupport (Bus, Dev, Fun, CapHeaderOffset, MaxBus, MaxAspmLevel);
          break;

        default:
          break;
        }
        //
        // switch(DevicePortType)
        //
      }
      //
      // Fun
      //
    }
    //
    // Dev
    //
  }
  //
  // Bus
  //
  CapHeaderOffset = PcieFindCapId (RootBus, RootDev, RootFun, 0x10);
  RootportAspmSupport (RootBus, RootDev, RootFun, CapHeaderOffset, MaxAspmLevel);
}

/**
  Disable ASPM support for all the downstreams ports and endpoints devices

  @param  RpNumber              Root port number

  @return                       No return value
**/
VOID
ThunderboltDisableAspmWithoutLtr (
  IN  UINT32                    RpNumber
  )
{
  UINT8                         Bus;
  UINT8                         Dev;
  UINT8                         Fun;
  UINT8                         RootBus;
  UINT8                         RootDev;
  UINT8                         RootFun;
  UINT8                         MinBus;
  UINT8                         MaxBus;
  UINT16                        DeviceId;
  UINTN                         DeviceBaseAddress;
  UINT8                         CapHeaderOffset;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  EFI_STATUS                    Status;

  MinBus = 0;
  MaxBus = 0;

  Status = GetTbtRpDevFun (RpNumber - 1, &RpDev, &RpFunc);
  MinBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev ,(UINT32)  RpFunc) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  MaxBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev, (UINT32) RpFunc) + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
  DeviceId  = MmioRead16 (MmPciBase (MinBus, 0x00, 0x00) + PCI_DEVICE_ID_OFFSET);

  RootBus = (UINT8) 0x00;
  RootDev = (UINT8) RpDev;
  RootFun = (UINT8) RpFunc;

  //
  //  Enumerate all the bridges and devices which are available on TBT host controller
  //
  for (Bus = MinBus; Bus <= MaxBus; ++Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        //
        // Check for Device availability
        //
        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
          //
          // Device not present
          //
          continue;
        }

        CapHeaderOffset = PcieFindCapId (Bus, Dev, Fun, 0x10);
        MmioAndThenOr16 (DeviceBaseAddress + CapHeaderOffset + 0x10, 0xFFFC, 0x00);
      } //Fun
    } //Dev
  } //Bus

  CapHeaderOffset = PcieFindCapId (RootBus, RootDev, RootFun, 0x10);
  NoAspmSupport (RootBus, RootDev, RootFun, CapHeaderOffset);
}

/**
  Set CLKREQ to specific Bx:Dx:Fx by CLKREQ setup (Enable/Disable)

  @param  Bus                   PCI bus number
  @param  Device                PCI device number
  @param  Function              PCI function number
  @param  ClkReqSetup           CLKREQ setup

  @return                       No return value
**/
VOID
TbtProgramClkReq (
  IN  UINT8                     Bus,
  IN  UINT8                     Device,
  IN  UINT8                     Function,
  IN  UINT8                     ClkReqSetup
  )
{
  UINTN                         DeviceBaseAddress;
  UINT8                         CapHeaderOffset;
  UINT16                        Data16;

  DeviceBaseAddress = MmPciBase (Bus, Device, Function);
  CapHeaderOffset   = PcieFindCapId (Bus, Device, Function, 0x10);

  //
  // Check if CLKREQ# is supported
  //
  if ((MmioRead32 (DeviceBaseAddress + CapHeaderOffset + 0x0C) & BIT18) != 0) {
    Data16 = MmioRead16 (DeviceBaseAddress + CapHeaderOffset + 0x010);

    if (ClkReqSetup) {
      Data16 = Data16 | BIT8; // Enable Clock Power Management
    } else {
      Data16 =  Data16 & (UINT16)(~BIT8); // Disable Clock Power Management
    }

    MmioWrite16 (DeviceBaseAddress + CapHeaderOffset + 0x010, Data16);
  }
}

/**
  Enable or disable CLKREQ for all the downstreams ports and endpoints devices

  @param  RpNumber              Root port number

  @return                       No return value
**/
VOID
ConfigureClkRequest (
  IN  UINT32                    RpNumber
  )
{
  UINT8                         Bus;
  UINT8                         Dev;
  UINT8                         Fun;
  UINT8                         MinBus;
  UINT8                         MaxBus;
  UINT16                        DeviceId;
  UINTN                         DeviceBaseAddress;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  EFI_STATUS                    Status;

  MinBus = 0;
  MaxBus = 0;

  Status = GetTbtRpDevFun (RpNumber - 1, &RpDev, &RpFunc);
  MinBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev ,(UINT32)  RpFunc) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  MaxBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev, (UINT32) RpFunc) + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
  DeviceId  = MmioRead16 (MmPciBase (MinBus, 0x00, 0x00) + PCI_DEVICE_ID_OFFSET);

  //
  //  Enumerate all the bridges and devices which are available on TBT host controller
  //
  for (Bus = MaxBus; Bus >= MinBus; --Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        //
        // Check for Device availability
        //
        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
          if (Fun == 0) {
            //
            // IF Fun is zero, stop enumerating other functions of the particular bridge
            //
            break;
          }
          //
          // otherwise, just skip checking for CLKREQ support
          //
          continue;
        }

        TbtProgramClkReq (Bus, Dev, Fun, SysConfig.TBTSetClkReq);

      } //Fun
    } // Dev
  } // Bus
}

/**
  1) Check LTR support in device capabilities 2 register (bit 11).
  2) If supported enable LTR in device control 2 register (bit 10).

  @param  Bus                   PCI bus number
  @param  Device                PCI device number
  @param  Function              PCI function number
  @param  LtrSetup              LTR setup value (Enabled/Disabled)

  @return                       No return value
**/
VOID
TbtProgramLtr (
  IN  UINT8                     Bus,
  IN  UINT8                     Device,
  IN  UINT8                     Function,
  IN  UINT8                     LtrSetup
  )
{
  UINTN                         DeviceBaseAddress;
  UINT8                         CapHeaderOffset;
  UINT16                        Data16;

  DeviceBaseAddress = MmPciBase (Bus, Device, Function);
  CapHeaderOffset   = PcieFindCapId (Bus, Device, Function, 0x10);

  //
  // Check if LTR# is supported
  //
  if ((MmioRead32 (DeviceBaseAddress + CapHeaderOffset + 0x24) & BIT11) != 0) {
    Data16 = MmioRead16 (DeviceBaseAddress + CapHeaderOffset + 0x028);

    if (LtrSetup) {
      Data16 = Data16 | BIT10; // LTR Mechanism Enable
    } else {
      Data16 =  Data16 & (UINT16)(~BIT10); // LTR Mechanism Disable
    }

    MmioWrite16 (DeviceBaseAddress + CapHeaderOffset + 0x028, Data16);
  }
}

/**
  Enable or disable LTR (Latency Tolerance Reporting) supported by setup page setting.

  @param  RpNumber              Root port number

  @return                       No return value
**/
VOID
ConfigureLtr (
  IN  UINT32 RpNumber
  )
{
  UINT8                 Bus;
  UINT8                 Dev;
  UINT8                 Fun;
  UINT8                 MinBus;
  UINT8                 MaxBus;
  UINT16                DeviceId;
  UINTN                 DeviceBaseAddress;
  UINTN                 RpDev;
  UINTN                 RpFunc;
  EFI_STATUS            Status;
  UINT8                 RpBus;

  MinBus = 0;
  MaxBus = 0;

  Status = GetTbtRpDevFun (RpNumber - 1, &RpDev, &RpFunc);
  MinBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev ,(UINT32)  RpFunc) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  MaxBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev, (UINT32) RpFunc) + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
  DeviceId  = MmioRead16 (MmPciBase (MinBus, 0x00, 0x00) + PCI_DEVICE_ID_OFFSET);
  RpBus = (UINT8) 0x00;

  //
  //  Enumerate all the bridges and devices which are available on TBT host controller
  //
  for (Bus = MinBus; Bus <= MaxBus; ++Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        //
        // Check for Device availability
        //
        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
          if (Fun == 0) {
            //
            // IF Fun is zero, stop enumerating other functions of the particular bridge
            //
            break;
          }
          //
          // otherwise, just skip checking for LTR support
          //
          continue;
        }

        TbtProgramLtr (Bus, Dev, Fun, SysConfig.TbtLtr);

      } //Fun
    } // Dev
  } // Bus
  TbtProgramLtr (RpBus, (UINT8) RpDev, (UINT8) RpFunc, SysConfig.TbtLtr);
}

/**
  US ports and endpoints which declare support must also have the LTR capability structure (cap ID 18h).
  In this structure you need to enter the max snoop latency and max non-snoop latency in accordance with the format specified in the PCIe spec.
  The latency value itself is platform specific so you'll need to get it from the platform architect or whatever.

  @param  RpIndex               Root Port Index

  @return                       No return value
**/
VOID
ThunderboltGetLatencyLtr (
  IN  UINT32                    RpIndex
  )
{
  PCH_SERIES                    PchSeries;

// AMI_OVERRIDE_START - Following the EIP279338 The resource for PEG Thunderbolt host is incorrect
#if 0
//AMI_OVERRIDE_START - EIP328410 - Enhance TBT related function description
//  //
//  // Add comment to this disabled code to make sure the behavior of generating CHM file is normal.
//  //
//  if(RpIndex>=21 && RpIndex<=23) {
//AMI_OVERRIDE_END - EIP328410 - Enhance TBT related function description
#else
  if(RpIndex>=PCH_PCIE_NUM + 1 && RpIndex<=PCH_PCIE_NUM + 3) {
#endif
    // PEG selector
    TbtLtrMaxSnoopLatency = LTR_MAX_SNOOP_LATENCY_VALUE;
    TbtLtrMaxNoSnoopLatency = LTR_MAX_NON_SNOOP_LATENCY_VALUE;
#if 0
//AMI_OVERRIDE_START - EIP328410 - Enhance TBT related function description
//  //
//  // Add comment to this disabled code to make sure the behavior of generating CHM file is normal.
//  //
//  } else if (RpIndex>=1 && RpIndex<=20) {
//AMI_OVERRIDE_END - EIP328410 - Enhance TBT related function description
#else
  } else if (RpIndex>=1 && RpIndex<=PCH_PCIE_NUM) {
#endif
// AMI_OVERRIDE_END - Following the EIP279338 The resource for PEG Thunderbolt host is incorrect
  // PCH selector
    PchSeries = GetPchSeries ();

    if (PchSeries == PchLp) {
      TbtLtrMaxSnoopLatency = 0x1003;
      TbtLtrMaxNoSnoopLatency = 0x1003;
    }
    if (PchSeries == PchH) {
      TbtLtrMaxSnoopLatency = 0x0846;
      TbtLtrMaxNoSnoopLatency = 0x0846;
    }
  }
}

/**
  Set LTR to specific Bx:Dx:Fx

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number
  @param  CapHeaderOffsetExtd   LTR capability offset
  @param  LtrMaxSnoopLatency    Max snoop latency value
  @param  LtrMaxNoSnoopLatency  Max no-snoop latency value

  @return                       No return value
**/
VOID
SetLatencyLtr (
  IN UINT8                      Bus,
  IN UINT8                      Dev,
  IN UINT8                      Fun,
  IN UINT16                     CapHeaderOffsetExtd,
  IN UINT16                     LtrMaxSnoopLatency,
  IN UINT16                     LtrMaxNoSnoopLatency
  )
{
  UINTN                         DeviceBaseAddress;

  if(CapHeaderOffsetExtd == 0) {
  return;
  }
  DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
  MmioWrite16 (DeviceBaseAddress + CapHeaderOffsetExtd + 0x004, LtrMaxSnoopLatency);
  MmioWrite16 (DeviceBaseAddress + CapHeaderOffsetExtd + 0x006, LtrMaxNoSnoopLatency);
}

/**
  Enable LTR support for all the Thunderbolt devices

  @param  RpNumber              Root port number

  @return                       No return value
**/
VOID
ThunderboltSetLatencyLtr (
  IN  UINT32                    RpNumber
  )
{
  UINT8                         Bus;
  UINT8                         Dev;
  UINT8                         Fun;
  UINT8                         MinBus;
  UINT8                         MaxBus;
  UINT16                        DeviceId;
  UINTN                         DeviceBaseAddress;
  UINT8                         CapHeaderOffsetStd;
  UINT16                        CapHeaderOffsetExtd;
  UINT16                        DevicePortType;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  EFI_STATUS                    Status;

  MinBus = 0;
  MaxBus = 0;

  Status = GetTbtRpDevFun (RpNumber - 1, &RpDev, &RpFunc);
//  for (Index = 0; Index < count (HR_Slots); ++Index) {
//  for (Index = 0; Index < count (HR_Slots); ++Index) {
  MinBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev ,(UINT32)  RpFunc) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  MaxBus    = MmioRead8 (MmPciBase ((UINT32) 0x00, (UINT32) RpDev, (UINT32) RpFunc) + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
  DeviceId  = MmioRead16 (MmPciBase (MinBus, 0x00, 0x00) + PCI_DEVICE_ID_OFFSET);


  for (Bus = MinBus; Bus <= MaxBus; ++Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        //
        // Check for Device availability
        //
        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
          //
          // Device not present
          //
          continue;
        }

        CapHeaderOffsetStd = PcieFindCapId (Bus, Dev, Fun, 0x10);
        DevicePortType  = (MmioRead16 (DeviceBaseAddress + CapHeaderOffsetStd + 0x002) >> 4) & 0xF;

        CapHeaderOffsetExtd = PcieFindExtendedCapId (Bus, Dev, Fun, 0x0018);

        switch (DevicePortType) {
        case 0:
          //
          // PCI Express Endpoint
          //
          SetLatencyLtr (Bus, Dev, Fun, CapHeaderOffsetExtd, TbtLtrMaxSnoopLatency, TbtLtrMaxNoSnoopLatency);
          break;

        case 1:
          //
          // Legacy PCI Express Endpoint
          //
          SetLatencyLtr (Bus, Dev, Fun, CapHeaderOffsetExtd, TbtLtrMaxSnoopLatency, TbtLtrMaxNoSnoopLatency);
          break;

        case 4:
          //
          // Root Port of PCI Express Root Complex
          //
          // Do-nothing
          break;

        case 5:
          //
          // Upstream Port of PCI Express Switch
          //
          SetLatencyLtr (Bus, Dev, Fun, CapHeaderOffsetExtd, TbtLtrMaxSnoopLatency, TbtLtrMaxNoSnoopLatency);
          break;

        case 6:
          //
          // Downstream Port of PCI Express Switch
          //
          // Do-nothing
          break;

        case 7:
          //
          // PCI Express to PCI/PCI-X Bridge
          //
          // Do-nothing
          break;

        case 8:
          //
          // PCI/PCI-X to PCI Express Bridge
          //
          // Do-nothing
          break;

        case 9:
          //
          // Root Complex Integrated Endpoint
          //
          // Do-nothing
          break;

        case 10:
          //
          // Root Complex Event Collector
          //
          // Do-nothing
          break;

        default:
          break;
        }
        //
        // switch(DevicePortType)
        //
      }
      //
      // Fun
      //
    }
    //
    // Dev
    //
  }
  //
  // Bus
  //
}

/**
  Stall function

  @param  Usec                  Unit of micro second.

  @return                       No return value
**/
VOID
Stall (
  IN  UINTN                     Usec
  )
{
  UINTN                         Index;
  UINT32                        Data32;
  UINT32                        PrevData;
  UINTN                         Counter;

  Counter = (UINTN) ((Usec * 10) / 3);
  //
  // Call WaitForTick for Counter + 1 ticks to try to guarantee Counter tick
  // periods, thus attempting to ensure Microseconds of stall time.
  //
  if (Counter != 0) {

    PrevData = IoRead32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_TMR);
    for (Index = 0; Index < Counter;) {
      Data32 = IoRead32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_TMR);
      if (Data32 < PrevData) {
        //
        // Reset if there is a overlap
        //
        PrevData = Data32;
        continue;
      }

      Index += (Data32 - PrevData);
      PrevData = Data32;
    }
  }

  return ;
}

/**
  This function contains PCIE2TBT <-> TBT2PCIE handshake
  procedure and all related methods called directly or underectly
  by TbtSetPCIe2TBTCommand.

  @param  command               Thunderbolt FW setting command
  @param  TBT_US_BUS            Thunderbolt host location

  @return                       No return value
**/
STATIC
VOID
TbtSetPCIe2TBTCommand (
  IN    UINT8                   command,
  IN    UINT8                   TBT_US_BUS
  )
{
  volatile UINT32               RegVal;
  volatile UINT32               max_wait_Iter;
  volatile UINT8                RetCode;
  UINTN                         DeviceBaseAddress;
  //
  // Wait 5 sec
  //
  max_wait_Iter     = 50;
  RetCode           = 0x52;

  DeviceBaseAddress = MmPciBase (TBT_US_BUS, 0x00, 0x00);
  MmioWrite32 (DeviceBaseAddress + PCIE2TBT_R, command | PCIE2TBT_VLD_B);

  IoWrite8 (0x80, 0x50);

  while (max_wait_Iter-- > 0) {
    RegVal = MmioRead32 (DeviceBaseAddress + TBT2PCIE_R);
    if (0xFFFFFFFF == RegVal) {
      //
      // Device is not here return now
      //
      RetCode = 0x5F;
      break;
    }

    if (RegVal & TBT2PCIE_DON_R) {
      RetCode = 0x51;
      break;
    }

    Stall (100 * 1000);
  }

  MmioWrite32 (DeviceBaseAddress + PCIE2TBT_R, 0);
  IoWrite8 (0x80, RetCode);
}

/**
  Called during Sx entry, initates TbtSetPCIe2TBTCommand HandShake to set GO2SX_NO_WAKE
  for Tbt devices if WakeupSupport is not present.

  @param  DispatchHandle        The unique handle assigned to this handler by SmiHandlerRegister().
  @param  DispatchContext       Points to an optional handler context which was specified when the
                                handler was registered.
  @param  CommBuffer            A pointer to a collection of data in memory that will
                                be conveyed from a non-SMM environment into an SMM environment.
  @param  CommBufferSize        The size of the CommBuffer.

  @retval EFI_SUCCESS           The interrupt was handled successfully.
**/
EFI_STATUS
EFIAPI
SxTbtEntryCallback (
  IN     EFI_HANDLE             DispatchHandle,
  IN     CONST VOID             *DispatchContext,
  IN OUT VOID                   *CommBuffer OPTIONAL,
  IN     UINTN                  *CommBufferSize OPTIONAL
  )
{
  UINT16                        DeviceId;
  UINT8                         CableConnected;
  UINT8                         HoustRouteBus;
  volatile UINT32               *PowerState;
  UINT32                        PowerStatePrev;
  BOOLEAN                       SecSubBusAssigned;
  UINTN                         DeviceBaseAddress;
  UINT8                         CapHeaderOffset;
  UINTN                         TbtRp;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  EFI_STATUS                    Status;
// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM
  AMI_TBT_HR_STATUS_DATA        HrStatusData;
  UINTN                         HrStatusSize;
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM
//AMI_OVERRIDE_START - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
  UINT32                        AmiTbtHrStatusAttribute;
//AMI_OVERRIDE_END - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP

  CableConnected    = 0;
  TbtRp             = SysConfig.TbtSelector;
  HoustRouteBus     = 3;
  SecSubBusAssigned = FALSE;

// AMI_OVERRIDE_START - Save time and avoid to program if TBT isn't enabled.
  if(SysConfig.TbtSupport != 1) return EFI_SUCCESS;
// AMI_OVERRIDE_END - Save time and avoid to program if TBT isn't enabled.

  if (TbtRp == 0) {
    TbtRp = gDefTbtSelector;
  }

  //
  // Get the Power State and Save
  //

  Status = GetTbtRpDevFun (TbtRp - 1, &RpDev, &RpFunc);
  ASSERT_EFI_ERROR (Status);
  CapHeaderOffset = PcieFindCapId (0x00, (UINT8)RpDev, (UINT8)RpFunc, 0x01);
  DeviceBaseAddress = MmPciBase (0x00, (UINT32)RpDev, (UINT32)RpFunc);
  PowerState        = ((volatile UINT32 *) (UINTN) (DeviceBaseAddress + CapHeaderOffset + 4)); //PMCSR
  PowerStatePrev    = *PowerState;
  *PowerState &= 0xFFFFFFFC;

  HoustRouteBus = MmioRead8 (DeviceBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  //
  // Check the Subordinate bus .If it is Zero ,assign temporary bus to
  // find the device presence .
  //
  if (HoustRouteBus == 0) {
//AMI_OVERRIDE_START - EIP299901 Avoid using temp bus which is larger than 64 (40h)
    //
	// PCI Configuration size type: 64MB, 128MB, or 256MB
	// Most of OEM will only use 64MB MMIO as PCI CFG
	// 256MB >> 10000000h size  (0xff Buses)
	// 128MB >> 8000000h  size  (0x80 Buses)
	// 64MB  >> 4000000h  size  (0x40 Buses)
	//
    MmioWrite8 (DeviceBaseAddress + PCI_SEC_BUS_NUM, 0x3F);
    MmioWrite8 (DeviceBaseAddress + PCI_SUB_BUS_NUM, 0x3F);
    HoustRouteBus     = 0x3F;
//AMI_OVERRIDE_END - EIP299901 Avoid using temp bus which is larger than 64 (40h)
    SecSubBusAssigned = TRUE;
  }
  //
  // Read the TBT Host router DeviceID
  //
  DeviceId = MmioRead16 (MmPciBase (HoustRouteBus, 0, 0) + PCI_DEVICE_ID_OFFSET);

  //
  // Check For HostRouter Presence: Cable connected or Force Power = 1
  //
  if (IsTbtHostRouter (DeviceId)) {
    CableConnected = 1;
  }

//AMI_OVERRIDE_START - EIP338529 -  Follow AR BWG 1.35 to update Sx Entry Flow
//
// Follow AR BWG ver. 1.35 updated:
//   Remove SPR supported checking before executing Go2Sx/Go2Sx_No_Wake in Sx entry flow.
//

  //
  // Assert 3.3V force power
  //
  if (CableConnected == 0) {
    GpioWrite (mGlobalNvsAreaPtr->TbtFrcPwrGpioAccessType,mGlobalNvsAreaPtr->TbtFrcPwrExpander,mGlobalNvsAreaPtr->TbtFrcPwrGpioNo,mGlobalNvsAreaPtr->TbtFrcPwrGpioLevel);
    Stall ((UINTN)(SysConfig.Gpio3ForcePwrDly * 1000));
  }

  TbtSetPCIe2TBTCommand (
    SysConfig.TbtWakeupSupport ? PCIE2TBT_GO2SX : PCIE2TBT_GO2SX_NO_WAKE,
    HoustRouteBus
    );

  //
  // De-assert force power
  //
  if (CableConnected == 0) {
    GpioWrite (mGlobalNvsAreaPtr->TbtFrcPwrGpioAccessType,mGlobalNvsAreaPtr->TbtFrcPwrExpander,mGlobalNvsAreaPtr->TbtFrcPwrGpioNo,!(mGlobalNvsAreaPtr->TbtFrcPwrGpioLevel));
    Stall ((UINTN)(SysConfig.Gpio3ForcePwrDly * 1000));
  }
//AMI_OVERRIDE_END - EIP338529 -  Follow AR BWG 1.35 to update Sx Entry Flow

// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM
#if 0
  //
  // Save Host Router Status in CMOS which will be used
  // in PEI Phase
  //
  CmosWrite8 (CMOS_TBTHR_PRESENT_ON_RESUME, CableConnected);
#else
  //
  // Save Host Router Status in NVRAM which will be used
  // in PEI Phase
  //
  switch(DeviceId) {
    case RR_HR_2C:
    case RR_HR_4C:
      HrStatusData.TbtHrSeries = Redwood_Ridge;
      break;
    case FR_HR_2C:
    case FR_HR_4C:
      HrStatusData.TbtHrSeries = Falcon_Ridge;
      break;
    case WR_HR_2C:
      HrStatusData.TbtHrSeries = BDW_TBT_LP;
      break;
    case AR_HR_2C:
    case AR_HR_4C:
    case AR_HR_LP:
    case AR_HR_C0_2C:
    case AR_HR_C0_4C:
    default:
      HrStatusData.TbtHrSeries = Alpine_Ridge;
      break;
  } // end of switch
  HrStatusData.TbtHrStatus = CableConnected;
  HrStatusSize = sizeof(AMI_TBT_HR_STATUS_DATA);

//AMI_OVERRIDE_START - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
  AmiTbtHrStatusAttribute = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
//AMI_OVERRIDE_END - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
  Status = mSmmVariable->SmmSetVariable( AMI_TBT_HR_STATUS_VARIABLE, \
                             &gAmiTbtHrStatusGuid, \
                             AmiTbtHrStatusAttribute, \
                             sizeof(AMI_TBT_HR_STATUS_DATA), \
                             &HrStatusData );
  DEBUG ((DEBUG_INFO, "Set TBT variable %r: HrSeries = %x HrStatus = %x\n", Status, HrStatusData.TbtHrSeries, HrStatusData.TbtHrStatus));
#endif
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM

  *PowerState = PowerStatePrev;
  //
  // Restore the bus number in case we assigned temporarily
  //
  if (SecSubBusAssigned) {
    MmioWrite8 (DeviceBaseAddress + PCI_SEC_BUS_NUM, 0x00);
    MmioWrite8 (DeviceBaseAddress + PCI_SUB_BUS_NUM, 0x00);
  }
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  if (SysConfig.Multicontroller) {
    Stall(100 * 1000); // 200 msec
    GpioSetOutputValue(GPIO_SKL_LP_GPP_G5, 0);
  }
  if (SysConfig.Multicontroller) {
    TbtRp             = SysConfig.TbtSelector1;

    //
    // Get the Power State and Save
    //
    Status = GetTbtRpDevFun (TbtRp - 1, &RpDev, &RpFunc);
    ASSERT_EFI_ERROR (Status);
    CapHeaderOffset = PcieFindCapId (0x00, (UINT8)RpDev, (UINT8)RpFunc, 0x01);
    DeviceBaseAddress = MmPciBase (0x00, (UINT32)RpDev, (UINT32)RpFunc);
    PowerState        = ((volatile UINT32 *) (UINTN) (DeviceBaseAddress + CapHeaderOffset + 4)); //PMCSR
    PowerStatePrev    = *PowerState;
    *PowerState &= 0xFFFFFFFC;

    HoustRouteBus = MmioRead8 (DeviceBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    //
    // Check the Subordinate bus .If it is Zero ,assign temporary bus to
    // find the device presence .
    //
    if (HoustRouteBus == 0) {
      MmioWrite8 (DeviceBaseAddress + PCI_SEC_BUS_NUM, 0xF0);
      MmioWrite8 (DeviceBaseAddress + PCI_SUB_BUS_NUM, 0xF0);
      HoustRouteBus     = 0xF0;
      SecSubBusAssigned = TRUE;
    }
    //
    // Read the TBT Host router DeviceID
    //
    DeviceId = MmioRead16 (MmPciBase (HoustRouteBus, 0, 0) + PCI_DEVICE_ID_OFFSET);

    //
    // Check For HostRouter Presence
    //
    if (IsTbtHostRouter (DeviceId)) {
      CableConnected = 1;
    }

    if (SysConfig.AicArSupport) {
      // Assert Force power pin
      GpioWrite (mGlobalNvsAreaPtr->TbtFrcPwrGpioAccessType1,mGlobalNvsAreaPtr->TbtFrcPwrExpander1,mGlobalNvsAreaPtr->TbtFrcPwrGpioNo1,mGlobalNvsAreaPtr->TbtFrcPwrGpioLevel1);
      Timeout = 600;
      DeviceBaseAddress = MmPciBase(HoustRouteBus, 0x00, 0x00);
      //
      // Check For HostRouter Presence
      //
      while (Timeout -- > 0) {
        RegVal = MmioRead32 (DeviceBaseAddress + TBT2PCIE_R);
        if (0xFFFFFFFF != RegVal) {
          break;
        }
        Stall(1* (UINTN)1000);
      }

      //
      // Before entering Sx state BIOS should execute GO2SX/NO_WAKE mailbox command for AIC.
      //

      if(!SysConfig.TbtWakeupSupport) {
      //Wake Disabled, GO2SX_NO_WAKE Command
        TbtSetPCIe2TBTCommand (PCIE2TBT_GO2SX_NO_WAKE, HoustRouteBus);
      }
      else {
        //Wake Enabled, GO2SX Command
        TbtSetPCIe2TBTCommand (PCIE2TBT_GO2SX, HoustRouteBus);
      }

      if (SysConfig.Gpio3ForcePwr == 0) {
        GpioWrite (mGlobalNvsAreaPtr->TbtFrcPwrGpioAccessType1,mGlobalNvsAreaPtr->TbtFrcPwrExpander1,mGlobalNvsAreaPtr->TbtFrcPwrGpioNo1,!(mGlobalNvsAreaPtr->TbtFrcPwrGpioLevel1));
      }
    }
    //
    // Save Host Router Status in CMOS which will be used
    // in PEI Phase
    //
    CmosWrite8 (CMOS_TBTHR_PRESENT_ON_RESUME, CableConnected);

    if (SysConfig.TbtWakeupSupport && !SysConfig.AicSupport) {
      //In this case,
      //Before instructing the platform to enter Sx state,
      //BIOS should remember whether Host Router is active, (it is active when cable isconnected).
      //No need to send GO2SX command
      *PowerState = PowerStatePrev;
      return EFI_SUCCESS;
    }
    else if (!SysConfig.TbtWakeupSupport && !SysConfig.AicSupport) {
      //Wake Disabled, GO2SX_NO_WAKE Command
      TbtSetPCIe2TBTCommand (PCIE2TBT_GO2SX_NO_WAKE, HoustRouteBus);
    }

    if (SysConfig.AicSupport && !SysConfig.AicArSupport) {
      //
      // Before entering Sx state BIOS should execute GO2SX/NO_WAKE mailbox command for AIC.
      //

      if(!SysConfig.TbtWakeupSupport) {
        //Wake Disabled, GO2SX_NO_WAKE Command
        TbtSetPCIe2TBTCommand (PCIE2TBT_GO2SX_NO_WAKE, HoustRouteBus);
       }
      else {
        //Wake Enabled GO2SX Command
        TbtSetPCIe2TBTCommand (PCIE2TBT_GO2SX, HoustRouteBus);
      }
    }

    *PowerState = PowerStatePrev;
    //
    // Restore the bus number in case we assigned temporarily
    //
    if (SecSubBusAssigned) {
      MmioWrite8 (DeviceBaseAddress + PCI_SEC_BUS_NUM, 0x00);
      MmioWrite8 (DeviceBaseAddress + PCI_SUB_BUS_NUM, 0x00);
    }
  }
    if (SysConfig.Multicontroller) {
      Stall(100 * 1000); // 200 msec
      GpioSetOutputValue(GPIO_SKL_LP_GPP_G4, 0);
    }
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.

  return EFI_SUCCESS;
}
// AMI_OVERRIDE_START - EIP272695 SUT will hang at blackscreen after enable Intel SGX in BIOS.

/**
  Get the memory pointer to debug memory address.

  @param  VOID                  No parameter

  @retval EFI_SUCCESS           Find the memory address.
  @retval others                Can't find the memory address.
**/
EFI_STATUS
GetTbtDebugBaseAddressMem (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         DataSize;
  SETUP_DATA                    SetupData;
  UINT32                        VarAttributes;

  VarAttributes = 0;
  DataSize = sizeof (SETUP_DATA);
  Status = mSmmVariable->SmmGetVariable (
		  PLATFORM_SETUP_VARIABLE_NAME,
		  &gSetupVariableGuid,
		  &VarAttributes,
		  &DataSize,
		  &SetupData
		  );
  if(!EFI_ERROR(Status))
  {
    gTbtDebugBaseAddressMem = SetupData.TbtDebugBaseAddressMem;
  }
  else
  {
    gTbtDebugBaseAddressMem = 0;
  }
  return Status;
}
// AMI_OVERRIDE_END - EIP272695 SUT will hang at blackscreen after enable Intel SGX in BIOS.

/**
  Thunderbolt(TM) SW SMI callback function.

  @param  DispatchHandle        Dispatch handle.
  @param  *DispatchContext      Pointer to dispatch context.
  @param  *CommBuffer           Pointer to communication buffer.
  @param  *CommBufferSize       Pointer to communication buffer size.

  @retval EFI_SUCCESS           Callback function successfully
  @retval others                Callback function failed
**/
STATIC
EFI_STATUS
EFIAPI
ThunderboltSwSmiCallback (
  IN  EFI_HANDLE                DispatchHandle,
  IN  CONST VOID                *DispatchContext,
  IN  OUT VOID                  *CommBuffer OPTIONAL,
  IN  UINTN                     *CommBufferSize OPTIONAL
  )
{
  UINT8                         ThunderboltSmiFunction;

  ThunderboltSmiFunction = mGlobalNvsAreaPtr->ThunderboltSmiFunction;
// AMI_OVERRIDE_START - EIP272695 SUT will hang at blackscreen after enable Intel SGX in BIOS.
  //After SGX enable , the TbtDebugBaseAddressMem maybe changed,
  //So we need to re-get it again
  GetTbtDebugBaseAddressMem();
// AMI_OVERRIDE_END - EIP272695 SUT will hang at blackscreen after enable Intel SGX in BIOS.

// AMI_OVERRIDE_START - Update ThunderboltSmiFunction value by Token.
  DEBUG ((DEBUG_INFO, "Thunderbolt SMI Function register address:%x\n", &(mGlobalNvsAreaPtr->ThunderboltSmiFunction)));
  switch (ThunderboltSmiFunction) {
#if 0
  case 21:
#else
  case TBT_SWSMI_ENUMERATE_FUNCTION:
#endif
    ThunderboltCallback ((UINT32) mGlobalNvsAreaPtr->TbtSelector);
// AMI_OVERRIDE_START - Fix first tbt hot-plug fail.
    if(IsFirstEnterFlag) {
      DEBUG ((DEBUG_INFO, "Reset IsFirstEnterFlag...\n"));
      IsFirstEnterFlag = 0;
    }
// AMI_OVERRIDE_END - Fix first tbt hot-plug fail.
    break;

#if 0
  case 22:
#else
  case TBT_SWSMI_DISABLE_DEVICE_BRIDGAE_FUNCTION:
#endif
    DisablePCIDevicesAndBridges ((UINT32) mGlobalNvsAreaPtr->TbtSelector);
    break;

#if 0
  case 23:
#else
  case TBT_SWSMI_DISABLE_MSI_FUNCTION:
#endif
    DisableMSI();
  break;

// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
#if 0
  case 24:
#else
  case TBT_SWSMI_ENUMERATE_FUNCTION1:
#endif
    ThunderboltCallback ((UINT32) mGlobalNvsAreaPtr->TbtSelector1);
    break;

#if 0
  case 25:
#else
  case TBT_SWSMI_DISABLE_DEVICE_BRIDGAE_FUNCTION1:
#endif
    DisablePCIDevicesAndBridges ((UINT32) mGlobalNvsAreaPtr->TbtSelector1);
    break;
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.

#if 0
  case 26:
#else
  case TBT_SWSMI_ENDOFTBT:
#endif
    EndOfThunderboltCallback ((UINT32) mGlobalNvsAreaPtr->TbtSelector);
    break;

// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
#if 0
  case 27:
#else
  case TBT_SWSMI_ENDOFTBT1:
#endif
    EndOfThunderboltCallback ((UINT32) mGlobalNvsAreaPtr->TbtSelector1);
    break;
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.

  default:
    break;
  }
// AMI_OVERRIDE_END - Update ThunderboltSmiFunction value by Token.

  return EFI_SUCCESS;
}

/**
  Initialize Thunderbolt(TM) SMM driver

  @param  ImageHandle           Image handle.
  @param  SystemTable           Pointer to the EFI system table.

  @retval EFI_SUCCESS           Initialized successfully
  @retval others                Initialized failed
**/
EFI_STATUS
InSmmFunction (
  IN  EFI_HANDLE                ImageHandle,
  IN  EFI_SYSTEM_TABLE          *SystemTable
  )
{
  EFI_SMM_SX_DISPATCH2_PROTOCOL *SxDispatchProtocol;
  EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch;
  EFI_SMM_SX_REGISTER_CONTEXT   EntryDispatchContext;
  EFI_SMM_SW_REGISTER_CONTEXT   SwContext;
  EFI_HANDLE                    SwDispatchHandle;
  EFI_HANDLE                    S1DispatchHandle;
  EFI_HANDLE                    S3DispatchHandle;
  EFI_HANDLE                    S4DispatchHandle;
  EFI_HANDLE                    S5DispatchHandle;
  EFI_STATUS                    Status;
  UINTN                         DataSize;
//AMI_OVERRIDE_START - EIP329111 - Thunderbolt FRC uses 2 CMOS register without enumerating in SSP file
  //UINT8                         Data8;
//AMI_OVERRIDE_END - EIP329111 - Thunderbolt FRC uses 2 CMOS register without enumerating in SSP file

  SwDispatchHandle        = NULL;
  S1DispatchHandle        = NULL;
  S3DispatchHandle        = NULL;
  S4DispatchHandle        = NULL;
  S5DispatchHandle        = NULL;

  DataSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gSetupVariableGuid,
                  NULL,
                  &DataSize,
                  &SysConfig
                  );
  if (EFI_ERROR (Status)) {
    SysConfig.TbtCacheLineSize      = DEF_CACHE_LINE_SIZE;
    SysConfig.ReserveMemoryPerSlot  = DEF_RES_MEM_PER_DEV;
    SysConfig.ReservePMemoryPerSlot = DEF_RES_PMEM_PER_DEV;
    SysConfig.ReserveIoPerSlot      = DEF_RES_IO_PER_DEV;
    mGlobalNvsAreaPtr->TbtSelector  = gDefTbtSelector;
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
    TbtXConnectSupport = 0;
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
  } else {
    if (SysConfig.TbtSelector == AUTO) {
      mGlobalNvsAreaPtr->TbtSelector = gDefTbtSelector;
    } else {
      mGlobalNvsAreaPtr->TbtSelector = SysConfig.TbtSelector;
    }
  }
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
  TbtXConnectSupport = SysConfig.TbtXConnectSupport;
  mGlobalNvsAreaPtr->TbtXConnectSupport = SysConfig.TbtXConnectSupport;
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
  mGlobalNvsAreaPtr->WAKFinished             = 0;
  mGlobalNvsAreaPtr->TbtSupport              = SysConfig.TbtSupport;
  mGlobalNvsAreaPtr->TbtWakeupSupport        = SysConfig.TbtWakeupSupport;
  mGlobalNvsAreaPtr->AicArSupport            = SysConfig.AicArSupport;
  mGlobalNvsAreaPtr->TbtWin10Support         = SysConfig.Win10Support;
  mGlobalNvsAreaPtr->TbtAcpiRemovalSupport   = SysConfig.TbtAcpiRemovalSupport;
  mGlobalNvsAreaPtr->TbtGpioFilter           = SysConfig.Gpio5Filter;
  mGlobalNvsAreaPtr->TrOsup                  = SysConfig.TrOsup;

  mGlobalNvsAreaPtr->TbtFrcPwrGpioAccessType = gTbtInfoHob->ForcePwrGpio.GpioAccessType;
  mGlobalNvsAreaPtr->TbtFrcPwrExpander       = gTbtInfoHob->ForcePwrGpio.Expander;
  mGlobalNvsAreaPtr->TbtFrcPwrGpioNo         = gTbtInfoHob->ForcePwrGpio.GpioNumber;
  mGlobalNvsAreaPtr->TbtFrcPwrGpioLevel      = gTbtInfoHob->ForcePwrGpio.GpioLevel;
  mGlobalNvsAreaPtr->TbtCioPlugEvtAcpiGpeStsAddress  = gTbtInfoHob->CioPlugEventGpio.AcpiGpeStsAddress;
  mGlobalNvsAreaPtr->TbtCioPlugEvtAcpiGpeStsBit = gTbtInfoHob->CioPlugEventGpio.AcpiGpeStsBit;
  mGlobalNvsAreaPtr->TbtCioPlugEventGpioNo   = gTbtInfoHob->CioPlugEventGpio.GpioNumber;
#if 0  // AMI_OVERRID_START - Removed due to Intel doesn't verify it yet.
  if (SysConfig.Multicontroller)
  {
    mGlobalNvsAreaPtr->Multicontroller          = SysConfig.Multicontroller;
    mGlobalNvsAreaPtr->TbtSelector1             = SysConfig.TbtSelector1;
    mGlobalNvsAreaPtr->TbtFrcPwrGpioAccessType1 = gTbtInfoHob->ForcePwrGpio1.GpioAccessType;
    mGlobalNvsAreaPtr->TbtFrcPwrExpander1       = gTbtInfoHob->ForcePwrGpio1.Expander;
    mGlobalNvsAreaPtr->TbtFrcPwrGpioNo1         = gTbtInfoHob->ForcePwrGpio1.GpioNumber;
    mGlobalNvsAreaPtr->TbtFrcPwrGpioLevel1      = gTbtInfoHob->ForcePwrGpio1.GpioLevel;
    mGlobalNvsAreaPtr->TbtCioPlugEvtAcpiGpeStsAddress1  = gTbtInfoHob->CioPlugEventGpio1.AcpiGpeStsAddress;
    mGlobalNvsAreaPtr->TbtCioPlugEvtAcpiGpeStsBit1 = gTbtInfoHob->CioPlugEventGpio1.AcpiGpeStsBit;
    mGlobalNvsAreaPtr->TbtCioPlugEventGpioNo1   = gTbtInfoHob->CioPlugEventGpio1.GpioNumber;
  }
#endif // AMI_OVERRID_END - Removed due to Intel doesn't verify it yet.
  mGlobalNvsAreaPtr->TbtUsbSplitMode = gTbtInfoHob->TbtUsbSplitMode;
  mGlobalNvsAreaPtr->SplitModePchUsb2TbtPort1 = gTbtInfoHob->SplitModePchUsb2TbtPort1;
  mGlobalNvsAreaPtr->SplitModePchUsb2TbtPort2 = gTbtInfoHob->SplitModePchUsb2TbtPort2;
  gTbtDebugBaseAddressMem = SysConfig.TbtDebugBaseAddressMem;
  DEBUG ((DEBUG_INFO, "<TbtSmm> TbtDebugBaseAddressMem = %lx \n", gTbtDebugBaseAddressMem));

//AMI_OVERRIDE_START - EIP329111 - Thunderbolt FRC uses 2 CMOS register without enumerating in SSP file
  //Data8 = 1;
  //CmosWrite8 (CMOS_TBT_NOTIFY_OS, Data8); // By default set TBT notification to OS = True
  mGlobalNvsAreaPtr->DGfxOsHotplugNotification = 1;

  //Data8 = 0;
  //CmosWrite8 (CMOS_TBT_NOTIFY_GPU, Data8); // By default set TBT notification to GPU = False
  mGlobalNvsAreaPtr->DGfxGpuHotplugNotification = 0;
//AMI_OVERRIDE_END - EIP329111 - Thunderbolt FRC uses 2 CMOS register without enumerating in SSP file

  gTbtDgpuPciBaseAddressMem = SysConfig.TbtDgpuPciBaseAddressMem;
  gTbtDeviceTreeBuffer = SysConfig.TbtDeviceTreeBuffer;

  if (SysConfig.TbtSupport == 0) {
    if (SysConfig.TbtPcieSupport == 1) {
      DEBUG ((DEBUG_INFO, "Thunderbolt(TM) Delay = 200 ms\n"));
      Stall(200 * 1000); // 200 msec
    }
    return EFI_SUCCESS;
  }

// AMI_OVERRID_START - EIP259656 S3/S4/S5 with TBT device/ USB device fail
  Status = gSmst->SmmLocateProtocol(
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID**)&mSmmVariable
                    );
  ASSERT_EFI_ERROR (Status);
// AMI_OVERRID_END - EIP259656 S3/S4/S5 with TBT device/ USB device fail
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSxDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &SxDispatchProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  //
  // Register S3 entry phase call back function
  //
  EntryDispatchContext.Type   = SxS3;
  EntryDispatchContext.Phase  = SxEntry;
  Status = SxDispatchProtocol->Register (
                                SxDispatchProtocol,
                                SxTbtEntryCallback,
                                &EntryDispatchContext,
                                &S3DispatchHandle
                                );
  ASSERT_EFI_ERROR (Status);
  //
  // Register S4 entry phase call back function
  //
  EntryDispatchContext.Type   = SxS4;
  EntryDispatchContext.Phase  = SxEntry;
  Status = SxDispatchProtocol->Register (
                                SxDispatchProtocol,
                                SxTbtEntryCallback,
                                &EntryDispatchContext,
                                &S4DispatchHandle
                                );
  ASSERT_EFI_ERROR (Status);
  //
  // Locate the SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &SwDispatch
                    );

  ASSERT_EFI_ERROR (Status);
  //
  // Register SWSMI handler
  //
  SwContext.SwSmiInputValue = SW_SMI_TBT_ENUMERATE;
  Status = SwDispatch->Register (
                        SwDispatch,
                        ThunderboltSwSmiCallback,
                        &SwContext,
                        &SwDispatchHandle
                        );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Thunderbolt(TM) SMM driver entry point.

  @param  ImageHandle           Image handle.
  @param  SystemTable           Pointer to the EFI system table.

  @retval EFI_SUCCESS           Initialized successfully
  @retval others                Initialized failed
**/
EFI_STATUS
EFIAPI
TbtSmmEntryPoint (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol;
  EFI_STATUS                    Status;

  EFI_HANDLE                    Handle;

  Handle = NULL;
  DEBUG ((DEBUG_INFO, "TbtSmmEntryPoint\n"));
  //
  // Get TBT INFO HOB
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &gTbtInfoHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gTbtInfoHob = GetNextGuidHob (&gTbtInfoHobGuid, gTbtInfoHob);
  if (gTbtInfoHob == NULL) {
    return EFI_NOT_FOUND;
  }

  gDefTbtSelector = gTbtInfoHob->TbtDefaultSelectorInfo;

  DEBUG ((DEBUG_INFO, "TbtDefaultSelectorInfo = %x \n", gDefTbtSelector));
  DEBUG ((DEBUG_INFO, "<TbtSmm> TbtSupportedPortsInfo = %lx \n", gTbtInfoHob->TbtSupportedPortsInfo));

  //
  // Locate our shared data area
  //
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;

  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEfiGlobalNvsAreaProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mGlobalNvsAreaPtr
                    );
  ASSERT_EFI_ERROR (Status);

  return InSmmFunction (ImageHandle, SystemTable);
}

/**
  Set CLKREQ (for clock power management), ASPM (Active State Power Management Control),
  and LTR (Latency Tolerance Reporting) after Thunderbolt event.

  @param  RpIndex               Root port index

  @return                       No return value
**/
VOID
EndOfThunderboltCallback (
  IN  UINT32                    RpIndex
  )
{

  ConfigureClkRequest (RpIndex);
  if (!SysConfig.TbtAspm) { //Aspm disable case
    ThunderboltDisableAspmWithoutLtr (RpIndex);
  } else { //Aspm enable case
    ThunderboltEnableAspmWithoutLtr ((UINT16)SysConfig.TbtAspm, RpIndex);
  }

  if (SysConfig.TbtLtr) {
    ThunderboltGetLatencyLtr (RpIndex);
    ThunderboltSetLatencyLtr (RpIndex);
  }
  ConfigureLtr (RpIndex);
} // EndOfThunderboltCallback
