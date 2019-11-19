/**@file
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

#include "TbtSmiHandler.h"
#include <CommonDefinitions.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <Protocol/SmmVariable.h>
#include <Library/BaseMemoryLib.h>
#include <CmosMap.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/CmosAccessLib.h>
#include <Library/DebugLib.h>
#include <Protocol/GlobalNvsArea.h>
//AMI_OVERRIDE_START - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler
#include <Library/AmiBufferValidationLib.h>
//AMI_OVERRIDE_END - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler

#define MEM_PER_SLOT  (SysConfig.ReserveMemoryPerSlot << 4)
#define PMEM_PER_SLOT (SysConfig.ReservePMemoryPerSlot << 4)
#define IO_PER_SLOT   (SysConfig.ReserveIoPerSlot << 2)

GLOBAL_REMOVE_IF_UNREFERENCED UINTN                 DeviceBaseAddress;
// AMI_OVERRIDE_START - EIP313994 - Unable to change MEM & PMEM size for each TBT slot
extern SETUP_DATA                                   SysConfig;
// AMI_OVERRIDE_END - EIP313994 - Unable to change MEM & PMEM size for each TBT slot

extern SETUP_DATA                 *mSysConfig;
// AMI_OVERRIDE_START - Fix first tbt hot-plug fail.
extern UINT8                      IsFirstEnterFlag;
// AMI_OVERRIDE_END - Fix first tbt hot-plug fail.

STATIC UINT32 PDebugAddressConf;
STATIC UINT32 *PDebugAddress    = &PDebugAddressConf;
STATIC UINT32 PDebugAddressInit = 0;
STATIC UINT8  *PrintCpStart     = 0;
STATIC UINT8  *PrintCpEnd       = 0;
//AMI_OVERRIDE_START >>> EIP426038 - Fix build failed with new GCC flags
//GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA           *mGlobalNvsAreaPtr;
extern EFI_GLOBAL_NVS_AREA           *mGlobalNvsAreaPtr;
//AMI_OVERRIDE_END <<< EIP426038 - Fix build failed with new GCC flags
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
GLOBAL_REMOVE_IF_UNREFERENCED UINT8 TbtXConnectSupport;
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN LastTimeDgpuFound = FALSE;
//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN mIsPortA = TRUE;
GLOBAL_REMOVE_IF_UNREFERENCED TBT_DGFX_INFO mThunderboltDGfx = { 0xff, 0xff, 0xff, 0xff, FALSE };
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
// AMI_OVERRIDE_START - Fix Linux build error.
//GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS  gTbtDgpuPciBaseAddressMem = 0;
//GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS gTbtDebugBaseAddressMem = 0;
extern EFI_PHYSICAL_ADDRESS  gTbtDgpuPciBaseAddressMem;
extern  EFI_PHYSICAL_ADDRESS gTbtDebugBaseAddressMem;
// AMI_OVERRIDE_END - Fix Linux build error.
#define TBT_DEBUG_AREA_SIZE_IN_KB     64

// AMI_OVERRIDE_START - Fix Linux build error.
//GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS gTbtDeviceTreeBuffer = 0; // Tbt Device Tree buffer
extern EFI_PHYSICAL_ADDRESS gTbtDeviceTreeBuffer; // Tbt Device Tree buffer
// AMI_OVERRIDE_END - Fix Linux build error.

STATIC UINT32 *DebugAddr        = 0;
BOOLEAN isLegacyDevice          = FALSE;

//AMI_OVERRIDE_START - EIP336316 - Improve WSMT validation on Thunderbolt
VOID
PcieLinkEnable (
  IN UINT8       RpBus,
  IN UINT8       RpDev,
  IN UINT8       RpFun,
  IN BOOLEAN     LinkEnable
  );
//AMI_OVERRIDE_END - EIP336316 - Improve WSMT validation on Thunderbolt

//AMI_OVERRIDE_START - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler
/**
  Validate MMIO buffer through AmiValidateMmioBuffer() and update 80 port when error occurred.

  This function verifies the buffer to make sure its address range is legal for a MMIO buffer.
  SMI handlers receive buffer address and/or size from outside of SMM at runtime must validate
  the buffer by this function prior to using the buffer or passing to other SMM interfaces.

  @param  ErrorCode             Update 80 port when error occurred
  @param  PhysicalAddress       MMIO buffer address
  @param  BufferSize            MMIO buffer size

  @return                       Status is returned from AmiValidateMmioBuffer()
**/
EFI_STATUS
CheckMmio (
  IN  UINT8                     ErrorCode,
  IN  UINTN                     PhysicalAddress,
  IN  CONST UINTN               BufferSize
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;

  if (VALID_MMIO_AT_TBT_SMM) {
    Status = AmiValidateMmioBuffer ((VOID *) PhysicalAddress, BufferSize);
    if (EFI_ERROR (Status)) {
      IoWrite8 (0x80, ErrorCode);
      Stall (1000000);
    }
  }

  return Status;
}
//AMI_OVERRIDE_END - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler

/**
  Read from default setup value

  @param  VOID                  No parameter

  @return                       No return value
**/
STATIC
VOID
SetReservedResources (
  VOID
  )
{

  SysConfig.TbtCacheLineSize      = DEF_CACHE_LINE_SIZE;
  SysConfig.ReserveMemoryPerSlot  = DEF_RES_MEM_PER_DEV;
  SysConfig.ReservePMemoryPerSlot = DEF_RES_PMEM_PER_DEV;
  SysConfig.ReserveIoPerSlot      = DEF_RES_IO_PER_DEV;

// AMI_OVERRIDE_START - EIP278493 Daisy Chain failed when DGFX is enabled
  //
  // It's already set in TbtSmm.c
  //
  //ReserveBusForDGfx = 8;
  //ReserveMemoryForDGfx = 32;
  //ReservePMemoryForDGfx = 256;

  //
  // It will be done at ConfigurePort (),
  // DO NOT add it, Otherwise, the memory will be overdriven.
  //
  //ReserveMemoryForDGfx <<= 4;
  //ReservePMemoryForDGfx <<= 4;
// AMI_OVERRIDE_END - EIP278493 Daisy Chain failed when DGFX is enabled
}

/**
  Get BUS limit number

  @param  Pi                    Pointer to PortInfo strusture

  @return                       No return value
**/
STATIC
VOID
PortInfoInit (
  IN  OUT PortInfo              *Pi
  )
{
  Pi->BusNumLimit = 4;
}

/**
  Print debug message content to gTbtDebugBaseAddressMem buffer by UINT8 based.

  @param  cp                    Debug message content by UINT8 based

  @return                       No return value
**/
STATIC
VOID
PrintCp (
  IN    UINT8                   cp
  )
{
  if (!PDebugAddressInit) {
    PDebugAddressConf = (UINT32) gTbtDebugBaseAddressMem;

    PDebugAddressInit = 1;
    if (!PDebugAddress) {
      return ;

    }

    PrintCpStart  = (UINT8 *) (UINTN) (*PDebugAddress);
    DebugAddr     = (UINT32 *) PrintCpStart;
    //PrintCpStart += BUF_LEN; //Comment this to make sure the buffer start from offset 0
    PrintCpEnd = PrintCpStart + BUF_LEN;
  }

  if (!PrintCpStart) {
    return ;

  }

  if (PrintCpStart < PrintCpEnd) {
    *PrintCpStart = cp;
    PrintCpStart++;
  }
} // PrintCp

/**
  Print debug message content to gTbtDebugBaseAddressMem buffer by UINT16 based.
  The content will be transferd to CHAR8 based.

  @param  cp                    Debug message content by UINT16 based

  @return                       No return value
**/
STATIC
VOID
PrintCp16 (
  IN    UINT16                  cp
  )
{
  PrintCp (cp >> 8);
  PrintCp (cp & 0xff);
} // PrintCp16

/**
  Print debug message content to gTbtDebugBaseAddressMem buffer by UINT32 based.
  The content will be transferd to CHAR8 based.

  @param  cp                    Debug message content by UINT32 based

  @return                       No return value
**/
STATIC
VOID
PrintCp32 (
  IN    UINT32                  cp
  )
{
  PrintCp16 (cp >> 16);
  PrintCp16 (cp & 0xffff);
} // PrintCp32

/**
  Print debug message to gTbtDebugBaseAddressMem buffer by CHAR8 based.

  @param  cp                    Debug message string pointer

  @return                       No return value
**/
STATIC
VOID
PrintCpStr (
  IN    CHAR8                   *cp
  )
{
  for (; cp && *cp; ++cp) {
    PrintCp (*cp);
  }

  PrintCp ('|');
} // PrintCpStr


/**
  This routine is used to read a byte from Thunderbolt Device Tree Info Buffer

  @param  Offset                Offset of Thunderbolt Device Tree Info Buffer
  @param  Data                  Pointer to Data

  @return                       No return value
**/
VOID
ReadTbtDeviceTreeBuffer (
  IN UINT16                     Offset,
  IN UINT8*                     Data
  )
{
  UINT8*  Data8Ptr = 0;
// AMI_OVERRIDE_START - Fix Linux build error.
//  Data8Ptr = (UINT8*) (UINTN) (gTbtDeviceTreeBuffer);
  Data8Ptr = (UINT8*) (UINTN) (gTbtDeviceTreeBuffer);
// AMI_OVERRIDE_END - Fix Linux build error.
  Data8Ptr+=Offset;

  *Data = *Data8Ptr;
 }

/**
  This routine is used to fill Thunderbolt Device Tree Info Buffer

  @param  Offset                Offset of Thunderbolt Device Tree Info Buffer
  @param  Data                  Data which is stored to Thunderbolt Device Tree Info Buffer

  @return                       No return value
**/
VOID
FillTbtDeviceTreeBuffer (
  IN UINT16                     Offset,
  IN UINT8                      Data
  )
{
  UINT8*  Data8Ptr = 0;
// AMI_OVERRIDE_START - Fix Linux build error.
//  Data8Ptr = (UINT8*) (UINTN) (gTbtDeviceTreeBuffer);
  Data8Ptr = (UINT8*) (UINTN) (gTbtDeviceTreeBuffer);
// AMI_OVERRIDE_END - Fix Linux build error.
  Data8Ptr+=Offset;

  *Data8Ptr = Data;
 }

/**
  This routine is used to fill a Device Info using 'FillTbtDeviceTreeBuffer'
  {
    Bus number
    Device number
    Function number
    Vendor ID
    Device ID
  }

  @param  Offset                Offset of Thunderbolt Device Tree Info Buffer
  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number

  @return                       No return value
**/
VOID
FillATbtDeviceTree (
  IN UINT16                     Offset,
  IN UINT8                      Bus,
  IN UINT8                      Dev,
  IN UINT8                      Fun
  )
{
  UINT8                         TempData8;
  UINTN                         TempDeviceBaseAddress;
  UINT16                        TempData16;

  TempDeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
  FillTbtDeviceTreeBuffer(Offset, Bus);
  FillTbtDeviceTreeBuffer(Offset+2, Dev);
  FillTbtDeviceTreeBuffer(Offset+4, Fun);
  TempData8 = (UINT8) (MmioRead16 (TempDeviceBaseAddress + PCI_VID) & 0xFF);
  FillTbtDeviceTreeBuffer(Offset+6, TempData8);
  TempData8 = (UINT8) ((MmioRead16 (TempDeviceBaseAddress + PCI_VID) >> 8) & 0xFF);
  FillTbtDeviceTreeBuffer(Offset+7, TempData8);
  TempData8 = (UINT8) (MmioRead16 (TempDeviceBaseAddress + PCI_DID) & 0xFF);
  FillTbtDeviceTreeBuffer(Offset+8, TempData8);
  TempData8 = (UINT8) ((MmioRead16 (TempDeviceBaseAddress + PCI_DID) >> 8) & 0xFF);
  FillTbtDeviceTreeBuffer(Offset+9, TempData8);

  // Update Structure size
  TempData16 = 0;
  ReadTbtDeviceTreeBuffer(0, &TempData8);
  TempData16 = TempData8;
  ReadTbtDeviceTreeBuffer(1, &TempData8);
  TempData16 |= (TempData8<<8);

  TempData16 += 10; //Updated structure size

  TempData8 = (UINT8) (TempData16 & 0xFF);
  FillTbtDeviceTreeBuffer(0, TempData8);
  TempData8 = (UINT8) ((TempData16>>8) & 0xFF);
  FillTbtDeviceTreeBuffer(1, TempData8);
 }

/**
  This routine is used to read the Dgpu Pci Config Space into a buffer

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number

  @return                       No return value
**/
VOID
FillDgpuPciConfigData (
  IN UINT8                      Bus,
  IN UINT8                      Dev,
  IN UINT8                      Fun
  )
{
  UINT16                        Reg;
  UINT8*                        Data8Ptr = 0;

  Data8Ptr = (UINT8*) (UINTN) (gTbtDgpuPciBaseAddressMem);
  DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);

  for(Reg=0;  ; Reg++) {
    *Data8Ptr = MmioRead8 (DeviceBaseAddress + Reg);
    Data8Ptr++;

    if(Reg == 0xFFF) {
      break;
    }
  }

}

//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
///**
//  This routine is used to assign bus number to the given PCI bus system
//
//  @param StartBusNumber     Number of beginning.
//  @param SubBusNumber       The number of sub bus.
//
//  @retval EFI_SUCCESS       Successfully assigned bus number.
//  @retval EFI_DEVICE_ERROR  Failed to assign bus number.
//
//**/
//EFI_STATUS
//PciAssignBusNumber (
//  IN UINT8                              StartBusNumber,
//  OUT UINT8                             *SubBusNumber
//  )
//{
//  EFI_STATUS                      Status;
//  UINT8                           Device;
//  UINT8                           Func;
//  UINTN                           SecondBus;
//  UINT16                          Register;
//  UINT8                           Register8;
//  UINT8                           RegVal;
//
//  PrintCpStr("PciAssignBusNumber");
//  PrintCpStr("####");
//  PrintCpStr("StartBusNumber");
//  PrintCp(StartBusNumber);
//  PrintCpStr("*SubBusNumber");
//  PrintCp(*SubBusNumber);
//  PrintCpStr("####");
//
//  SecondBus = 0;
//  Register = 0;
//
//  *SubBusNumber = StartBusNumber;
//
//  //
//  // First check to see whether the parent is ppb
//  //
//  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
//    for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
//      Status = EFI_SUCCESS;
//
//      //
//      // Check to see whether a pci device is present
//      //
//      DeviceBaseAddress = MmPciBase (StartBusNumber, Device, Func);
//      RegVal = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE);
//      if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID))
//      {
//        Status = EFI_NOT_FOUND;
//      }
//
//      if (!EFI_ERROR (Status)   &&
//          (((RegVal & (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6)) == 0x01) || ((RegVal & (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6)) == 0x02))) {
//
//        //
//        // Reserved one bus for cardbus bridge
//        //
//        *SubBusNumber = *SubBusNumber + 1;
//        SecondBus = *SubBusNumber;
//
//        Register  = (UINT16) ((SecondBus << 8) | (UINT16) StartBusNumber);
//
//        DeviceBaseAddress = MmPciBase (StartBusNumber, Device, Func);
//
//        MmioWrite16 (DeviceBaseAddress + 0x18, Register);
//        PrintCpStr("StartBusNumber");
//        PrintCp(StartBusNumber);
//        PrintCpStr("Device");
//        PrintCp(Device);
//        PrintCpStr("Func");
//        PrintCp(Func);
//        PrintCpStr("Pri/Sec Bus");
//        PrintCp16(Register);
//
//
//        //
//        // Initialize SubBusNumber to SecondBus
//        //
//        DeviceBaseAddress = MmPciBase (StartBusNumber, Device, Func);
//        MmioWrite8 (DeviceBaseAddress + 0x1A, *SubBusNumber);
//        PrintCpStr("Sub Bus");
//        PrintCp(*SubBusNumber);
//
//        //
//        // If it is PPB, recursively search down this bridge
//        //
//        if ((RegVal & (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6)) == 0x01) {
//
//          Register8 = 0xFF;
//          MmioWrite8 (DeviceBaseAddress + 0x1A, Register8);
//          PrintCpStr("Sub Bus");
//          PrintCp(Register8);
//
//          Status = PciAssignBusNumber (
//                    (UINT8) (SecondBus),
//                    SubBusNumber
//                    );
//
//          if (EFI_ERROR (Status)) {
//            return EFI_DEVICE_ERROR;
//          }
//        }
//
//        //
//        // Set the current maximum bus number under the PPB
//        //
//        DeviceBaseAddress = MmPciBase (StartBusNumber, Device, Func);
//
//        MmioWrite8 (DeviceBaseAddress + 0x1A, *SubBusNumber);
//        PrintCpStr("Set the current maximum bus number under the PPB");
//        PrintCpStr("StartBusNumber");
//        PrintCp(StartBusNumber);
//        PrintCpStr("Device");
//        PrintCp(Device);
//        PrintCpStr("Func");
//        PrintCp(Func);
//        PrintCpStr("Sub Bus");
//        PrintCp(*SubBusNumber);
//
//      }
//
//      DeviceBaseAddress = MmPciBase (StartBusNumber, Device, Func);
//      RegVal = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE);
//      if (INVALID_PCI_DEVICE != MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
//        if (Func == 0 && ((RegVal & BIT7) == 0) /*Single function*/) {
//
//          //
//          // Skip sub functions, this is not a multi function device
//          //
//          Func = PCI_MAX_FUNC;
//        }
//      }
//    }
//  }
//
//  return EFI_SUCCESS;
//}
//
///**
//  This routine is used to clear the bus number to the given PCI bus system
//
//  @param StartBusNumber     Number of beginning.
//  @param EndBusNumber       Number of ending.
//
//  @retval EFI_SUCCESS       Successfully cleared bus number.
//
//**/
//EFI_STATUS
//PciClearBusNumber (
//  IN UINT8                              StartBusNumber,
//  IN UINT8                              EndBusNumber
//  )
//{
//  UINT8   Bus;
//  UINT8   Dev;
//  UINT8   Fun;
//  UINT8   RegVal;
//  // AMI_OVERRIDE_START - EIP278493 Daisy Chain failed when DGFX is enabled
//  UINT8   CioHotplugSlot = 0xff;
//  BOOLEAN CioSlotPresent = FALSE;
//  BOOLEAN ArPcie;
//  UINT8   MaxSlotNum;
//  // AMI_OVERRIDE_END - EIP278493 Daisy Chain failed when DGFX is enabled
//
//  for (Bus = EndBusNumber; Bus >= StartBusNumber; --Bus) {
//    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
//      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
//        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
//        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
//          if (Fun == 0) {
//            break;
//          }
//
//          continue;
//        }
//
//// AMI_OVERRIDE_START - EIP278493 Daisy Chain failed when DGFX is enabled
//        CioSlotPresent = GetCioSlotByDevId (
//                          Bus,
//                          &CioHotplugSlot,
//                          &MaxSlotNum,
//                          &ArPcie
//                          );
//        //
//        // Upstream port
//        //
//        if (Dev == 0 && Fun == 0 && CioSlotPresent) {
//          //
//          // Restore SUB Bus after finishing GFX detection
//          //
//          MmioWrite8 (DeviceBaseAddress + PCI_SUB_BUS_NUM, mBusLimitInCurrentPort);
//          continue;
//        }
//
//        //
//        // Downstream port: CIO slot
//        //
//        if (Dev == CioHotplugSlot && Fun == 0 && CioSlotPresent) {
//          //
//          // Restore SUB Bus after finishing GFX detection
//          //
//          MmioWrite8 (DeviceBaseAddress + PCI_SUB_BUS_NUM, mBusLimitInCurrentPort);
//          continue;
//        }
//// AMI_OVERRIDE_END - EIP278493 Daisy Chain failed when DGFX is enabled
//
//        RegVal = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE);
//        if (((RegVal & (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6)) == 0x01) || ((RegVal & (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6)) == 0x02)) {
//          MmioWrite8 (DeviceBaseAddress + 0x18, 0);
//          MmioWrite8 (DeviceBaseAddress + 0x19, 0);
//          MmioWrite8 (DeviceBaseAddress + 0x1A, 0);
//        }
//      } // for ( Fun .. )
//    } // for ( Dev ... )
//  } // for ( Bus ... )
//
//  return EFI_SUCCESS;
//}
//
///**
//  This routine is used to clear the Dgpu Pci Config Space IOSpaceEnable bit
//**/
//VOID
//DgpuPciConfigIoSpaceDisable (
//  IN UINT8                              Bus,
//  IN UINT8                              Dev,
//  IN UINT8                              Fun
//  )
//{
//  DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
//  MmioAnd8 (DeviceBaseAddress + PCI_CMD_REG, (UINT8)~(BIT0));
//}
//
///**
//  This routine is used to findout the dGPU present in the given PCI bus system
//**/
//BOOLEAN
//CheckForDgpuPresence (
//  IN UINT8                              StartBusNumber,
//  IN UINT8                              EndBusNumber,
//  IN UINT16                             DgpuVendorId    OPTIONAL,
//  IN UINT16                             DgpuDeviceId    OPTIONAL,
//  IN UINT8*                             DGfxBus
//  )
//{
//  UINT8   Bus;
//  UINT8   Dev;
//  UINT8   Fun;
//  UINT8   RegVal;
//  UINT8   Register8;
//  BOOLEAN DgpuFound;
//
//  DgpuFound = FALSE;
//
//  for (Bus = StartBusNumber; (Bus <= EndBusNumber) && (!DgpuFound); ++Bus) {
//    for (Dev = 0; (Dev <= PCI_MAX_DEVICE) && (!DgpuFound); ++Dev) {
//      for (Fun = 0; (Fun <= PCI_MAX_FUNC) && (!DgpuFound); ++Fun) {
//        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
//        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
//          if (Fun == 0) {
//            break;
//          }
//
//          continue;
//        }
//
//        RegVal = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE);
//        if ((RegVal & (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6)) == 0x00) {
//          Register8 = MmioRead8 (DeviceBaseAddress + 0x0B);
//          if (Register8 == 0x03) /*Display controllers*/ {
//            DgpuFound = TRUE;
//
//           /* Clear IOSpaceEnable bit in PCI Config Space */
//           DgpuPciConfigIoSpaceDisable(Bus, Dev, Fun);
//
//           FillDgpuPciConfigData(Bus, Dev, Fun);
//
//            //
//            // Fill Endpoint information structure
//            //
//            FillATbtDeviceTree(52, Bus, Dev, Fun);
//            *DGfxBus = Bus;
//          }
//        }
//      } // for ( Fun .. )
//    } // for ( Dev ... )
//  } // for ( Bus ... )
//
//  return DgpuFound;
//}
//
///**
//  This routine is used to findout the dGPU present in the first depth of the tree or not
//**/
//BOOLEAN
//CheckForFirstDepth (
//  IN UINT8                              UpstreamBus,
//  IN UINT8                              DgpuBus
//  )
//{
//  UINT8   Bus;
//  UINT8   Dev;
//  UINT8   Fun;
//  UINT8   RegVal;
//  BOOLEAN FirstDepth;
//  UINT8   SecBusNo;
//  UINT8   DownBus;
//  UINT8   DownDev;
//  UINT8   DownFun;
//
//  Bus = UpstreamBus+1; //Downstream bus number
//  FirstDepth = FALSE;
//  DownBus = 0;
//  DownDev = 0;
//  DownFun = 0;
//
//  for (Dev = 0; (Dev <= PCI_MAX_DEVICE) && (!FirstDepth); ++Dev) {
//    for (Fun = 0; (Fun <= PCI_MAX_FUNC) && (!FirstDepth); ++Fun) {
//      DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
//      if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
//        if (Fun == 0) {
//          break;
//        }
//
//        continue;
//      }
//
//      RegVal = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE);
//      if ((RegVal & (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6)) != 0x01) {
//        //
//        // Not a PCI-to-PCI bridges device
//        //
//        continue;
//      }
//
//      SecBusNo = MmioRead8 (DeviceBaseAddress + PCI_SEC_BUS_NUM);
//
//      if (SecBusNo == DgpuBus) {
//        //
//        // This is the Rootbridge for the given 'DgpuBus' device
//        //
//        FirstDepth = TRUE;
//        DownBus = Bus;
//        DownDev = Dev;
//        DownFun = Fun;
//      }
//    } // for ( Fun .. )
//  } // for ( Dev ... )
//
//
//  if (FirstDepth) {
//    //
//    // Fill Endpoint Downstream port information structure
//    //
//    FillATbtDeviceTree(42, DownBus, DownDev, DownFun);
//  }
//
//  return FirstDepth;
//}
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"

//AMI_OVERRIDE_START - EIP317127 - Implement "Thunderbolt Dock with NIC Hard Hang BIOS Workaround"
/**
  Set soft L1

  This feature will set vendor specific enhanced capability register:
  VESC_REG 5:  (Offset 1Ch): General configuration register
  VESC_REG 42: (Offset B0h): General configuration register

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number

  @return                       No return value
**/
STATIC
VOID
SetSoftL1 (
  IN  UINT8                     Bus,
  IN  UINT8                     Dev,
  IN  UINT8                     Fun
  )
{
  UINT32 Data32;
  DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);

  //
  // Check for abcence of DS bridge
  //
  if (0xFFFF == MmioRead16 (DeviceBaseAddress + PCI_DID)) {
    return;
  }

  //
  // set vesc_reg5[8] bit
  //
  // L1 Soft exit timer fix disable
  // 0: Dn Port does not wait for 1.5 ms during L1 soft exit
  // 1: Dn Port waits for 1.5 ms during L1 soft exit (bug)
  // (This bit is valid only for Dn and reserved for Up/EP ports)
  //
  Data32 = MmioRead32 (DeviceBaseAddress + PCI_TBT_VESC_REG5);
  Data32 |= 0x00000100;
  MmioWrite32 (DeviceBaseAddress + PCI_TBT_VESC_REG5, Data32);

  //
  // set vesc_reg42[12] bit
  //
  //  cb_l1soft_fsm_rst_in_recov_dis
  //  0: Entry to Recovery Reset L1 Soft FSM
  //  1: Disable the L1 Soft FSM reset
  //
  Data32 = MmioRead32 (DeviceBaseAddress + PCI_TBT_VESC_REG42);
  Data32 |= 0x00001000;
  MmioWrite32 (DeviceBaseAddress + PCI_TBT_VESC_REG42, Data32);
}
//AMI_OVERRIDE_END - EIP317127 - Implement "Thunderbolt Dock with NIC Hard Hang BIOS Workaround"

/**
  Unset vesc_reg2[23] bit function

  This function will unset vendor specific enhanced capability register:
  VESC_REG2 (Offset 10h): General configuration register

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  Fun                   PCI function number

  @return                       No return value
**/
STATIC
VOID
UnsetVesc (
  IN       UINT8     Bus,
  IN       UINT8     Dev,
  IN       UINT8     Fun
  )
{
  UINT32 Data32;
  DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);

  //
  // Check for abcence of DS bridge
  //
  if(0xFFFF == MmioRead16(DeviceBaseAddress + PCI_DID)) {
    return;
  }
  //
  // Set vesc_reg5[8]
  //
  Data32 = MmioRead32 (DeviceBaseAddress + PCI_TBT_VESC_REG5);
  Data32 |= 1 << 8;
  MmioWrite32(DeviceBaseAddress + PCI_TBT_VESC_REG5, Data32);
  //
  // Set vesc_reg42[12] bits set
  //
  Data32 = MmioRead32 (DeviceBaseAddress + PCI_TBT_VESC_REG42);
  Data32 |= 1 << 12;
  MmioWrite32 (DeviceBaseAddress + PCI_TBT_VESC_REG42, Data32);
}// Unset_VESC_REG2

/**
  Round up requested memory

  @param  CurrentUsage          Consumed resource

  @return                       Assign memory size
**/
STATIC
UINT16
MemPerSlot (
  IN    UINT16                  CurrentUsage
  )
{
  if (CurrentUsage == 0) {
    return 0;
  }

  if (CurrentUsage <= 16) {
    return 16;
  }

  if (CurrentUsage <= 64) {
    return 64;
  }

  if (CurrentUsage <= 128) {
    return 128;
  }

  if (CurrentUsage <= 256) {
    return 256;
  }

  if (CurrentUsage <= 512) {
    return 512;
  }

  if (CurrentUsage <= 1024) {
    return 1024;
  }

  return CurrentUsage;
} // MemPerSlot

/**
  Round up requested p-memory

  @param  CurrentUsage          Consumed resource

  @return                       Assign memory size
**/
STATIC
UINT64
PMemPerSlot (
  IN    UINT64                  CurrentUsage
  )
{
  if (CurrentUsage == 0) {
    return 0;
  }

  if (CurrentUsage <= 1024ULL) {
    return 1024ULL;
  }

  if (CurrentUsage <= 4096ULL) {
    return 4096ULL;
  }

  return CurrentUsage;
} // PMemPerSlot

/**
  Set physical port resource

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  SubBus                Subordinate bus number
  @param  Depth                 The depth of daisy chain
  @param  CurrentPi             Pointer to current PortInfo
  @param  Pi                    Pointer to PortInfo

  @return                       No return value
**/
STATIC
VOID
SetPhyPortResources (
  IN       UINT8                Bus,
  IN       UINT8                Dev,
  IN       UINT8                SubBus,
  IN       INT8                 Depth,
  IN       PortInfo             *CurrentPi,
  IN  OUT  PortInfo             *Pi
  )
{
  UINT8                         Cmd;
  UINT16                        DeltaMem;
  UINT64                        DeltaPMem;
  UINT8                         DeltaIo;

  Cmd               = CMD_BUS_MASTER;
  DeviceBaseAddress = MmPciBase (Bus, Dev, 0x00);

  MmioWrite8 (DeviceBaseAddress + PCI_SUBUS, SubBus);
  MmioWrite8 (DeviceBaseAddress + PCI_CMD, Cmd);

  DeltaIo = Pi->IoBase - CurrentPi->IoBase;
  if (Depth >= 0 && SysConfig.ReserveIoPerSlot && DeltaIo < IO_PER_SLOT) {
    Pi->IoBase += IO_PER_SLOT - DeltaIo;

  }

  if (Pi->IoBase > CurrentPi->IoBase && (Pi->IoBase - 0x10) <= Pi->IoLimit) {
    MmioWrite8 (DeviceBaseAddress + PCI_IOBASE, CurrentPi->IoBase);
    MmioWrite8 (DeviceBaseAddress + PCI_IOLIMIT, Pi->IoBase - 0x10);
    MmioWrite32 (DeviceBaseAddress + PCI_IOBASE_U, 0x00000000);
    Cmd |= CMD_BM_IO;
  } else {
    MmioWrite16 (DeviceBaseAddress + PCI_IOBASE, DISBL_IO_REG1C);
    Pi->IoBase = CurrentPi->IoBase;
  }

  DeltaMem = Pi->MemBase - CurrentPi->MemBase;
  if (isLegacyDevice) {
    if (Depth >= 0 && SysConfig.ReserveMemoryPerSlot && DeltaMem < MEM_PER_SLOT) {
      Pi->MemBase += MEM_PER_SLOT - DeltaMem;
    }
  } else {
    if (DeltaMem < MemPerSlot (DeltaMem)) {
      Pi->MemBase += MemPerSlot (DeltaMem) - DeltaMem;
    }
  }

  if (Pi->MemBase > CurrentPi->MemBase && (Pi->MemBase - 0x10) <= Pi->MemLimit) {
    MmioWrite16 (DeviceBaseAddress + PCI_MEMBASE, CurrentPi->MemBase);
    MmioWrite16 (DeviceBaseAddress + PCI_MEMLIMIT, Pi->MemBase - 0x10);
    Cmd |= CMD_BM_MEM;
  } else {
    MmioWrite32 (DeviceBaseAddress + PCI_MEMBASE, DISBL_MEM32_REG20);
    Pi->MemBase = CurrentPi->MemBase;
  }

  DeltaPMem = Pi->PMemBase64 - CurrentPi->PMemBase64;
  if (isLegacyDevice) {
    if ((Depth >= 0) && (SysConfig.ReservePMemoryPerSlot) && ((UINTN)DeltaPMem < (UINTN)PMEM_PER_SLOT)) {
      Pi->PMemBase64 += PMEM_PER_SLOT - DeltaPMem;
    }
  } else {
    if (DeltaPMem < PMemPerSlot (DeltaPMem)) {
      Pi->PMemBase64 += PMemPerSlot (DeltaPMem) - DeltaPMem;
    }
  }

  if (Pi->PMemBase64 > CurrentPi->PMemBase64 && (Pi->PMemBase64 - 0x10) <= Pi->PMemLimit64) {
    MmioWrite16 (DeviceBaseAddress + PCI_PRE_MEMBASE, (UINT16) (CurrentPi->PMemBase64 & 0xFFFF));
    MmioWrite16 (DeviceBaseAddress + PCI_PRE_MEMLIMIT, (UINT16) ((Pi->PMemBase64 - 0x10) & 0xFFFF));
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U, (UINT32) (CurrentPi->PMemBase64 >> 16));
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMLIMIT_U, (UINT32) ((Pi->PMemBase64 - 0x10) >> 16));
    Cmd |= CMD_BM_MEM;
  } else {
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE, DISBL_PMEM_REG24);
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U, 0);
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMLIMIT_U, 0);
    Pi->PMemBase64 = CurrentPi->PMemBase64;
  }

  MmioWrite8 (DeviceBaseAddress + PCI_CMD, Cmd);
  MmioWrite8 (DeviceBaseAddress + PCI_CLS, SysConfig.TbtCacheLineSize);
} // SetPhyPortResources

/**
  Get resource request from BAR

  @param  Bar                   Pointer to PCI BAR

  @return                       Return BAR request
**/
STATIC
UINT32
SaveSetGetRestoreBar (
  IN  volatile  UINT32          *Bar
  )
{
  UINT32                        BarReq;
  UINT32                        OrigBar;
  OrigBar = *Bar;// Save BAR
  *Bar    = 0xFFFFFFFF;    // Set BAR
  BarReq  = *Bar;          // Get BAR
  *Bar    = OrigBar;       // Restore BAR

  return BarReq;
} // SaveSetGetRestoreBar

/**
  Set IO resource to BAR if resoure is enough.

  @param  Bar                   PCI BAR address
  @param  BarReq                BAR request value
  @param  Cmd                   PCI command register address
  @param  IoReg                 Pointer to IO_REGS

  @return                       No return value
**/
STATIC
VOID
SetIoBar (
  IN  volatile  UINT32          *BAR,
  IN            UINT32          BarReq,
  IN  OUT       UINT8           *Cmd,
  IN  OUT       IO_REGS         *IoReg
  )
{
  UINT16                        Alignment;
  UINT16                        Size;
  UINT16                        NewBase;

  Alignment = ~(BarReq & 0xFFFC);
  Size      = Alignment + 1;

  if (IoReg->Base > IoReg->Limit || !Size) {
    return ;

  }

  NewBase = BAR_ALIGN (IoReg->Base, Alignment);
  if (NewBase > IoReg->Limit || NewBase + Size - 1 > IoReg->Limit) {
    return ;

  }
  *BAR       = NewBase; // Set BAR
  IoReg->Base = NewBase + Size; // Advance to new position
  *Cmd      |= CMD_BM_IO; // Set Io Space Enable
} // SetIoBar

/**
  Set memory resource to BAR if resoure is enough.

  @param  Bar                   PCI BAR address
  @param  BarReq                BAR request value
  @param  Cmd                   PCI command register address
  @param  MemReg                Pointer to MEM_REGS

  @return                       No return value
**/
STATIC
VOID
SetMemBar (
  IN  volatile  UINT32          *BAR,
  IN            UINT32          BarReq,
  IN  OUT       UINT8           *Cmd,
  IN  OUT       MEM_REGS        *MemReg
  )
{
  UINT32                        Alignment;
  UINT32                        Size;
  UINT32                        NewBase;

  Alignment = ~(BarReq & 0xFFFFFFF0);
  Size      = Alignment + 1;

  if (MemReg->Base > MemReg->Limit || !Size) {
    return ;

  }

  NewBase = BAR_ALIGN (MemReg->Base, Alignment);
  if (NewBase > MemReg->Limit || NewBase + Size - 1 > MemReg->Limit) {
    return ;

  }
  *BAR        = NewBase; // Set BAR
  MemReg->Base = NewBase + Size; // Advance to new position
  *Cmd       |= CMD_BM_MEM; // Set Memory Space Enable
} // SetMemBar

/**
  Set p-memory resource to BAR if resoure is enough.

  @param  Bar                   PCI BAR address
  @param  IsMaxBar              PCI_BAR5 for devices, PCI_BAR1 for bridge
  @param  BarReq                BAR request value
  @param  Cmd                   PCI command register address
  @param  MemReg                Pointer to PMEM_REGS

  @return                       No return value
**/
STATIC
VOID
SetPMem64Bar (
  IN    volatile  UINT32        *BAR,
  IN              BOOLEAN       IsMaxBar,
  IN              UINT32        BarReq,
  IN    OUT       UINT8         *Cmd,
  IN    OUT       PMEM_REGS     *MemReg
  )
{
  UINT32                        Alignment;
  UINT32                        Size;
  UINT64                        NewBase;

  Alignment = ~(BarReq & 0xFFFFFFF0);
  Size      = Alignment + 1;

  if (MemReg->Base64 > MemReg->Limit64 || !Size) {
    return ;
  }

  NewBase = BAR_ALIGN (MemReg->Base64, Alignment);
  if (NewBase > MemReg->Limit64 || NewBase + Size - 1 > MemReg->Limit64) {
    return ;
  }
  *BAR = (UINT32)(NewBase & 0xFFFFFFFF); // Set BAR
  if (!IsMaxBar) {
    BAR++;
    *BAR = (UINT32)(NewBase >> 32); // Set BAR U
  }
  MemReg->Base64 = NewBase + Size; // Advance to new position
  *Cmd         |= CMD_BM_MEM; // Set Memory Space Enable
} // SetPMem64Bar

//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
/**
  Set device resource

  @param  Bus                   PCI bus number
  @param  Dev                   PCI device number
  @param  MaxFun                PCI_MAX_FUNC for devices, 1 for bridge
  @param  MaxBar                PCI_BAR5 for devices, PCI_BAR1 for bridge
  @param  Pi                    Pointer to PortInfo
  @param  DetectGfxInserted     TRUE: GFX is inserted; FALSE: GFX is notinserted

  @return                       No return value
**/
STATIC
VOID
SetDevResources (
  IN       UINT8                Bus,
  IN       UINT8                Dev,
  IN       UINT8                MaxFun,  // PCI_MAX_FUNC for devices, 1 for bridge
  IN       UINT8                MaxBar,     // PCI_BAR5 for devices, PCI_BAR1 for bridge
  IN  OUT  PortInfo             *Pi,
      OUT  BOOLEAN              *DetectGfxInserted
  )
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
{
  UINT8                         Fun;
  UINT8                         Reg;
  UINT32                        BarReq;
  IO_REGS                       Io;
  MEM_REGS                      Mem;
  PMEM_REGS                     PMem;
  UINT8                         Cmd;

  Io.Base       = Pi->IoBase << 8;
  Io.Limit      = (Pi->IoLimit << 8) | 0xFF;
  Mem.Base      = Pi->MemBase << 16;
  Mem.Limit     = (Pi->MemLimit << 16) | 0xFFFF;
  PMem.Base64   = Pi->PMemBase64 << 16;
  PMem.Limit64  = (Pi->PMemLimit64 << 16) | 0xFFFF;

  for (Fun = 0; Fun < MaxFun; ++Fun) {
    DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
    MmioWrite8 (DeviceBaseAddress + PCI_CMD, CMD_BUS_MASTER);
    Cmd = MmioRead8 (DeviceBaseAddress + PCI_CMD);
    if (0xFFFF == MmioRead16 (DeviceBaseAddress + PCI_DID)) {
      continue;

    }

//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
    //
    // "MaxFun == PCI_MAX_FUNC" means it's a device instead of a bridge
    // "(*DetectGfxInserted) == FALSE" and "!mThunderboltDGfx.IsLocationDgpu" are different:
    //
    // 1. Current GFX at L1  >> Insert any TBT device to L2:
    //                         *DetectGfxInserted is FALSE / mThunderboltDGfx.IsLocationDgpu is TRUE
    // 2. Current has no GPU >> Insert 2 TBT devices including a GFX
    //                         *DetectGfxInserted is TRUE / mThunderboltDGfx.IsLocationDgpu is TRUE
    //
    // >>>> So, we separate the 2 flags
    //
    if ((*DetectGfxInserted) == FALSE && !mThunderboltDGfx.IsLocationDgpu &&
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
        MaxFun == PCI_MAX_FUNC && mGlobalNvsAreaPtr->TbtXConnectSupport) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
      //
      // Video card found
      //
      if (MmioRead8 (MmPciBase (Bus, Dev, Fun) + PCI_BCC) == 3) {
        FillATbtDeviceTree (52, Bus, Dev, Fun);
        DEBUG ((DEBUG_INFO, "[DGPU]GFX at [%03d:%02d:%d] -> (BufferTree: 52)\n", Bus, Dev, Fun));
        FillDgpuPciConfigData (Bus, Dev, Fun);
        (*DetectGfxInserted) = TRUE;
        mThunderboltDGfx.DgpuBus = Bus;
        mThunderboltDGfx.DgpuDev = Dev;
        mThunderboltDGfx.DgpuFun = Fun;
        mThunderboltDGfx.IsLocationDgpu = TRUE;
        //
        // Port A or Port B
        //
        mThunderboltDGfx.HrDownstreamDevice = mIsPortA ? 1 : 4;
      }
    }
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"

    for (Reg = PCI_BAR0; Reg <= MaxBar; Reg += 4) {
      BarReq = SaveSetGetRestoreBar((volatile UINT32*)  (DeviceBaseAddress + Reg)); // Perform BAR sizing

      if (BarReq & BIT0) {
        //
        // I/O BAR
        //
        SetIoBar (
          (volatile UINT32 *) (DeviceBaseAddress + Reg),
          BarReq,
          &Cmd,
          &Io
          );
        continue;
      }

      if (BarReq & BIT3) {
        //
        // P-Memory BAR
        //
        SetPMem64Bar ((volatile UINT32 *) (DeviceBaseAddress + Reg), MaxBar == Reg, BarReq, &Cmd, &PMem);
      } else {
        SetMemBar ((volatile UINT32 *) (DeviceBaseAddress + Reg), BarReq, &Cmd, &Mem);
      }

      if (BIT2 == (BarReq & (BIT2 | BIT1))) {
        //
        // Base address is 64 bits wide
        //
        Reg += 4;
        if (!(BarReq & BIT3)) {
          //
          // 64-bit memory bar
          //
          MmioWrite32 (DeviceBaseAddress + Reg, 0);
        }
      }
    }

// AMI_OVERRIDE_START - To avoid system detect wrong device.
    // Assign ROM BAR
    if (!(IsTbtHostRouter(MmioRead16 (DeviceBaseAddress + PCI_DID)) && MmioRead16 (DeviceBaseAddress + PCI_VID) == 0x8086)){
      //DEBUG ((DEBUG_INFO, "Start Assign ROM BAR for device....\n"));
      if (MmioRead8 (DeviceBaseAddress + PCI_BCC) == PCI_CLASS_BRIDGE)
        Reg = PCI_BRIDGE_ROMBAR;
      else
        Reg = PCI_EXPANSION_ROM_BASE;

        BarReq = SaveSetGetRestoreBar((volatile UINT32 *) (DeviceBaseAddress + Reg));// Perform BAR sizing
        SetMemBar((volatile UINT32 *) (DeviceBaseAddress + Reg), BarReq, &Cmd, &Mem);
        //DEBUG ((DEBUG_INFO, "Assign rom bar end....\n"));
    }
// AMI_OVERRIDE_END - To avoid system detect wrong device.

    if (Cmd & BIT1) {
      //
      // If device uses I/O and MEM mapping use only MEM mepping
      //
      Cmd &= ~BIT0;
    }

    MmioWrite8 (DeviceBaseAddress + PCI_CMD, Cmd);
    MmioWrite8 (DeviceBaseAddress + PCI_CLS, SysConfig.TbtCacheLineSize);
  }
  //
  // Update Pi if any changes
  //
  if (Io.Base > ((UINT32) Pi->IoBase << 8)) {
    Pi->IoBase = (UINT8) (BAR_ALIGN (Io.Base, 0xFFF) >> 8);
  }

  if (Mem.Base > ((UINT32) Pi->MemBase << 16)) {
    Pi->MemBase = (UINT16) (BAR_ALIGN (Mem.Base, 0xFFFFF) >> 16);
  }

  if (PMem.Base64 > (Pi->PMemBase64 << 16)) {
    Pi->PMemBase64 = (BAR_ALIGN (PMem.Base64, 0xFFFFF) >> 16);
  }
} // SetDevResources

/**
  Init. AR HR config.

  @param  Hr_Config             Pointer to HR_CONFIG
  @param  BusNumLimit           Bus number limit
  @param  HrResConf             Pointer to BRDG_RES_CONFIG

  @return                       No return value
**/
STATIC
VOID
InitARHRConfigs(
  IN       HR_CONFIG            *Hr_Config,
  IN       UINT8                BusNumLimit,
  IN  OUT  BRDG_RES_CONFIG      *HrResConf
  )
{
  UINT8 i,j;

  //
  // DS port for USB device
  //
  HrConfigs[AR_DS_PORT2].DevId.Bus = HrConfigs[HR_US_PORT].DevId.Bus + 1;
  HrConfigs[AR_DS_PORT2].DevId.Dev = 2;
  HrConfigs[AR_DS_PORT2].DevId.Fun = 0;
  HrConfigs[AR_DS_PORT2].PBus = HrConfigs[AR_DS_PORT2].DevId.Bus;
  HrConfigs[AR_DS_PORT2].SBus = HrConfigs[AR_DS_PORT2].PBus + 1;
  HrConfigs[AR_DS_PORT2].SubBus = HrConfigs[AR_DS_PORT2].PBus + 1;
  //
  // CIO port
  //
  HrConfigs[AR_DS_PORT1].DevId.Bus = HrConfigs[HR_US_PORT].DevId.Bus + 1;
  HrConfigs[AR_DS_PORT1].DevId.Dev = 1;
  HrConfigs[AR_DS_PORT1].DevId.Fun = 0;
  HrConfigs[AR_DS_PORT1].PBus = HrConfigs[AR_DS_PORT1].DevId.Bus;
  HrConfigs[AR_DS_PORT1].SBus = HrConfigs[HR_DS_PORT0].SubBus + 1;
  HrConfigs[AR_DS_PORT1].SubBus = BusNumLimit;

  switch(Hr_Config->DeviceId)
  {
    //
    // HR with 1 DS and 1 USB
    //
    case AR_HR_2C:
    case AR_HR_LP:
    case AR_HR_C0_2C:
    case TR_HR_2C:
      Hr_Config->MinDSNumber = HrConfigs[AR_DS_PORT1].DevId.Dev;
      Hr_Config->MaxDSNumber = HrConfigs[AR_DS_PORT2].DevId.Dev;
      Hr_Config->BridgeLoops = 4;
      break;
    //
    // HR with 2 DS and 1 USB
    //
    case AR_HR_4C:
    case TR_HR_4C:
    case AR_HR_C0_4C:
      Hr_Config->MinDSNumber = 1;
      Hr_Config->MaxDSNumber = 4;
      Hr_Config->BridgeLoops = 6;
      for(j = 2, i = Hr_Config->MinDSNumber; j < count(HrConfigs) && i <= Hr_Config->MaxDSNumber; ++j, ++i)
      {
        HrConfigs[j].DevId.Bus = HrConfigs[HR_US_PORT].DevId.Bus + 1;
        HrConfigs[j].DevId.Dev = i;
        HrConfigs[j].DevId.Fun = 0;
        HrConfigs[j].PBus = HrConfigs[j].DevId.Bus;
        HrConfigs[j].Res.Cls = SysConfig.TbtCacheLineSize;
      }
    break;
  }
}//InitARHRConfigs

/**
  Init. common HR config.

  @param  Hr_Config             Pointer to HR_CONFIG
  @param  BusNumLimit           Bus number limit
  @param  HrResConf             Pointer to BRDG_RES_CONFIG

  @return                       No return value
**/
STATIC
VOID
InitCommonHRConfigs (
  IN       HR_CONFIG            *Hr_Config,
  IN       UINT8                BusNumLimit,
  IN  OUT  BRDG_RES_CONFIG      *HrResConf
  )
{
  UINT8 i;

  UINT8 j;
  for(i = 0; i < count(HrConfigs); ++i) {
    HrConfigs[i].IsDSBridge = TRUE;
  }
  //
  // US(HRBus:0:0)
  //
  HrConfigs[HR_US_PORT].DevId.Bus   = Hr_Config->HRBus;
  HrConfigs[HR_US_PORT].DevId.Dev   = 0;
  HrConfigs[HR_US_PORT].DevId.Fun   = 0;
  HrConfigs[HR_US_PORT].Res         = *HrResConf;
  // AMI_OVERRIDE_START - Support Legacy TBT device boot.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  if (SysConfig.TbtPcieIoRsvd == 0) {
    HrConfigs[HR_US_PORT].Res.IoBase  = DISBL_IO_REG1C & 0xFF;
    HrConfigs[HR_US_PORT].Res.IoLimit = DISBL_IO_REG1C >> 8;
  }
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  // AMI_OVERRIDE_END - Support Legacy TBT device boot.
  HrConfigs[HR_US_PORT].PBus        = HrConfigs[HR_US_PORT].DevId.Bus;
  HrConfigs[HR_US_PORT].SBus        = HrConfigs[HR_US_PORT].PBus + 1;
  HrConfigs[HR_US_PORT].SubBus      = BusNumLimit;
  HrConfigs[HR_US_PORT].IsDSBridge  = FALSE;

  //
  // HIA resides here
  //
  HrConfigs[HR_DS_PORT0].DevId.Bus    = HrConfigs[HR_US_PORT].DevId.Bus + 1;
  HrConfigs[HR_DS_PORT0].DevId.Dev    = 0;
  HrConfigs[HR_DS_PORT0].DevId.Fun    = 0;
  HrConfigs[HR_DS_PORT0].Res          = NOT_IN_USE_BRIDGE;
  HrConfigs[HR_DS_PORT0].Res.MemBase  = HrResConf->MemLimit;
  HrConfigs[HR_DS_PORT0].Res.MemLimit = HrResConf->MemLimit;
  HrResConf->MemLimit                -= 0x10; //This 1 MB chunk will be used by HIA
  HrConfigs[HR_DS_PORT0].Res.Cmd      = CMD_BM_MEM;
  HrConfigs[HR_DS_PORT0].Res.Cls      = SysConfig.TbtCacheLineSize;
  HrConfigs[HR_DS_PORT0].PBus         = HrConfigs[HR_DS_PORT0].DevId.Bus;
  HrConfigs[HR_DS_PORT0].SBus         = HrConfigs[HR_DS_PORT0].PBus + 1;
  HrConfigs[HR_DS_PORT0].SubBus       = HrConfigs[HR_DS_PORT0].PBus + 1;

  switch (Hr_Config->DeviceId) {
  case WR_HR_2C:
    //
    // HR with 1 DS only
    //
    HrConfigs[HR_DS_PORT1].DevId.Bus  = HrConfigs[HR_US_PORT].DevId.Bus + 1;
    HrConfigs[HR_DS_PORT1].DevId.Dev  = 1;
    HrConfigs[HR_DS_PORT1].DevId.Fun  = 0;
    HrConfigs[HR_DS_PORT1].Res        = *HrResConf;
    HrConfigs[HR_DS_PORT1].PBus       = HrConfigs[HR_DS_PORT1].DevId.Bus;
    HrConfigs[HR_DS_PORT1].SBus       = HrConfigs[HR_DS_PORT0].SubBus + 1;
    HrConfigs[HR_DS_PORT1].SubBus     = BusNumLimit;
    Hr_Config->MinDSNumber            = HrConfigs[HR_DS_PORT1].DevId.Dev;
    Hr_Config->MaxDSNumber            = HrConfigs[HR_DS_PORT1].DevId.Dev;
    Hr_Config->BridgeLoops            = 3;
    break;
   //
   // Alpine Ridge
   //
  case AR_HR_2C:
  case AR_HR_C0_2C:
  case AR_HR_LP:
  case AR_HR_4C:
  case AR_HR_C0_4C:
  //
  // Titan Ridge
  //
  case TR_HR_2C:
  case TR_HR_4C:
    InitARHRConfigs(Hr_Config, BusNumLimit, HrResConf);
    break;

  default:
    //
    // DS(HRBus+2:3-6:0)
    //
    Hr_Config->MinDSNumber  = 3;
    Hr_Config->MaxDSNumber  = 6;
    Hr_Config->BridgeLoops  = count (HrConfigs);

    for (j = 2, i = Hr_Config->MinDSNumber; j < count (HrConfigs) && i <= Hr_Config->MaxDSNumber; ++j, ++i) {
      HrConfigs[j].DevId.Bus  = HrConfigs[HR_US_PORT].DevId.Bus + 1;
      HrConfigs[j].DevId.Dev  = i;
      HrConfigs[j].DevId.Fun  = 0;
      HrConfigs[j].PBus       = HrConfigs[j].DevId.Bus;
      HrConfigs[j].Res.Cls    = SysConfig.TbtCacheLineSize;
    }
  }
} // InitCommonHRConfigs

/**
  Init. for disabled HR downstream port

  @param  id                    Index of BrdgConf
  @param  BrdgConf              BRDG_CONFIG

  @return                       No return value
**/
STATIC
VOID
InitHRDSPort_Disable (
  IN       UINT8                id,
  IN  OUT  BRDG_CONFIG          *BrdgConf
  )
{
  HrConfigs[id].Res     = NOT_IN_USE_BRIDGE;
  HrConfigs[id].SBus    = BrdgConf->SBus;
  HrConfigs[id].SubBus  = BrdgConf->SBus;

  BrdgConf->SBus++;
} // InitHRDSPort_Disable

/**
  Init. HR downstream port if only 1 port supported.

  @param  BrdgConf              BRDG_CONFIG

  @return                       No return value
**/
STATIC
VOID
InitHRDSPort_1Port (
  IN  OUT  BRDG_CONFIG          *BrdgConf
  )
{
  UINT16                        MemBase;
  UINT64                        PMemBase64;
  UINT8                         BusRange;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
  UINT8                         IoBase = BrdgConf->Res.IoBase & 0xF0;
// AMI_OVERRIDE_END - Support Legacy TBT device boot.

  MemBase                                = BrdgConf->Res.MemBase & 0xFFF0;
  PMemBase64                             = BrdgConf->Res.PMemBase64 &~0xFULL;
  BusRange                               = BrdgConf->SubBus - BrdgConf->PBus - (MAX_CFG_PORTS - 2); // MAX_CFG_PORTS-1(US)-1(HIA) is num of bridges in HR, on each bridge bus# is incremented
  BusRange                               -= DOCK_BUSSES; // Bus range for Dock port

  HrConfigs[HR_DS_PORT3].Res              = NOT_IN_USE_BRIDGE;
  HrConfigs[HR_DS_PORT3].Res.Cls          = SysConfig.TbtCacheLineSize;
  HrConfigs[HR_DS_PORT3].Res.MemBase      = MemBase;
  HrConfigs[HR_DS_PORT3].Res.MemLimit     = MemBase + 0xE00 - 1;
  HrConfigs[HR_DS_PORT3].Res.PMemBase64   = PMemBase64;
  HrConfigs[HR_DS_PORT3].Res.PMemLimit64  = PMemBase64 + 0x1A00 - 1;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  if (SysConfig.TbtPcieIoRsvd != 0){
    HrConfigs[HR_DS_PORT3].Res.IoBase     = IoBase;
    if ((BrdgConf->Res.IoLimit & 0xF0) < (IoBase + 0x50))
      HrConfigs[HR_DS_PORT3].Res.IoLimit  = BrdgConf->Res.IoLimit & 0xF0;
    else
      HrConfigs[HR_DS_PORT3].Res.IoLimit  = IoBase + 0x50;

    HrConfigs[HR_DS_PORT3].Res.Cmd        = CMD_BM_MEM_IO;
  } else {
    HrConfigs[HR_DS_PORT3].Res.Cmd        = CMD_BM_MEM;
  }
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
  HrConfigs[HR_DS_PORT3].SBus             = BrdgConf->SBus;
  HrConfigs[HR_DS_PORT3].SubBus           = BrdgConf->SBus + BusRange;

  BrdgConf->SBus                          = HrConfigs[HR_DS_PORT3].SubBus + 1;

  HrConfigs[HR_DS_PORT4].Res              = NOT_IN_USE_BRIDGE;
//AMI_OVERRIDE_START - EIP329956 - Do not assign I/O to AR XHCI port
  HrConfigs[HR_DS_PORT4].Res.Cmd          = CMD_BM_MEM;
//AMI_OVERRIDE_END - EIP329956 - Do not assign I/O to AR XHCI port
  HrConfigs[HR_DS_PORT4].Res.Cls          = SysConfig.TbtCacheLineSize;
  HrConfigs[HR_DS_PORT4].Res.MemBase      = MemBase + 0xE00;
  HrConfigs[HR_DS_PORT4].Res.MemLimit     = MemBase + 0x1600 - 1;
  HrConfigs[HR_DS_PORT4].Res.PMemBase64   = PMemBase64 + 0x1A00;
  HrConfigs[HR_DS_PORT4].Res.PMemLimit64  = PMemBase64 + 0x2200 - 1;
  HrConfigs[HR_DS_PORT4].SBus             = BrdgConf->SBus;
  HrConfigs[HR_DS_PORT4].SubBus           = BrdgConf->SBus + DOCK_BUSSES;

  BrdgConf->SBus                          = HrConfigs[HR_DS_PORT4].SubBus + 1;
} // InitHRDSPort_1Port


/**
  Init. AR downstream port if only 1 port supported.

  @param  BrdgConf              BRDG_CONFIG

  @return                       No return value
**/
STATIC
VOID
InitARDSPort_1Port(
  IN  OUT  BRDG_CONFIG          *BrdgConf
  )
{
  UINT16                        MemBase    = BrdgConf->Res.MemBase & 0xFFF0;
  UINT64                        PMemBase64 = BrdgConf->Res.PMemBase64 & ~0xFULL;
  UINT8                         BusRange   = BrdgConf->SubBus - BrdgConf->PBus - 2;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
  UINT8                         IoBase     = BrdgConf->Res.IoBase & 0xF0;
// AMI_OVERRIDE_END - Support Legacy TBT device boot.

  HrConfigs[AR_DS_PORT1].Res             = NOT_IN_USE_BRIDGE;
  HrConfigs[AR_DS_PORT1].Res.Cls         = SysConfig.TbtCacheLineSize;
  HrConfigs[AR_DS_PORT1].Res.MemBase     = MemBase;
  HrConfigs[AR_DS_PORT1].Res.MemLimit    = BrdgConf->Res.MemLimit - 1;
  HrConfigs[AR_DS_PORT1].Res.PMemBase64  = PMemBase64;
  HrConfigs[AR_DS_PORT1].Res.PMemLimit64 = BrdgConf->Res.PMemLimit64;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  if (SysConfig.TbtPcieIoRsvd != 0){
    HrConfigs[AR_DS_PORT1].Res.IoBase    = IoBase;
    if ((BrdgConf->Res.IoLimit & 0xF0) < (IoBase + 0x50))
      HrConfigs[AR_DS_PORT1].Res.IoLimit = BrdgConf->Res.IoLimit & 0xF0;
    else
      HrConfigs[AR_DS_PORT1].Res.IoLimit = IoBase + 0x50;

    HrConfigs[AR_DS_PORT1].Res.Cmd       = CMD_BM_MEM_IO;
  } else {
    HrConfigs[AR_DS_PORT1].Res.Cmd       = CMD_BM_MEM;
  }
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
  HrConfigs[AR_DS_PORT1].SBus            = BrdgConf->SBus;
  HrConfigs[AR_DS_PORT1].SubBus          = BrdgConf->SBus + BusRange;

  BrdgConf->SBus                         = HrConfigs[AR_DS_PORT1].SubBus + 1;

  HrConfigs[AR_DS_PORT2].Res             = NOT_IN_USE_BRIDGE;
  HrConfigs[AR_DS_PORT2].Res.Cls         = SysConfig.TbtCacheLineSize;
//AMI_OVERRIDE_START - EIP329956 - Do not assign I/O to AR XHCI port
  HrConfigs[AR_DS_PORT2].Res.Cmd         = CMD_BM_MEM;
//AMI_OVERRIDE_END - EIP329956 - Do not assign I/O to AR XHCI port
  HrConfigs[AR_DS_PORT2].Res.MemBase     = BrdgConf->Res.MemLimit;
  HrConfigs[AR_DS_PORT2].Res.MemLimit    = BrdgConf->Res.MemLimit;
  HrConfigs[AR_DS_PORT2].SBus            = BrdgConf->SBus;
  HrConfigs[AR_DS_PORT2].SubBus          = BrdgConf->SBus;

  BrdgConf->SBus                         = HrConfigs[AR_DS_PORT2].SubBus + 1;
}//InitARDSPort_1Port

/**
  Init. AR downstream port if 2 port supported.

  @param  BrdgConf              BRDG_CONFIG

  @return                       No return value
**/
STATIC
VOID
InitARDSPort_2Port(
  IN  OUT  BRDG_CONFIG          *BrdgConf
  )
{
  UINT16                        MemBase    = BrdgConf->Res.MemBase & 0xFFF0;
  UINT64                        PMemBase64 = BrdgConf->Res.PMemBase64 & ~0xFULL;
  UINT8                         BusRange   = BrdgConf->SubBus - BrdgConf->PBus - 3;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
  UINT8                         IoBase     = BrdgConf->Res.IoBase & 0xF0;
// AMI_OVERRIDE_END - Support Legacy TBT device boot.

  // Busses are split between ports 1 and 4
  BusRange /= 2;

  HrConfigs[AR_DS_PORT1].Res             = NOT_IN_USE_BRIDGE;
  HrConfigs[AR_DS_PORT1].Res.Cls         = SysConfig.TbtCacheLineSize;
  HrConfigs[AR_DS_PORT1].Res.MemBase     = MemBase;
  HrConfigs[AR_DS_PORT1].Res.MemLimit    = MemBase + 0x17F0 - 1;
  HrConfigs[AR_DS_PORT1].Res.PMemBase64  = PMemBase64;
  HrConfigs[AR_DS_PORT1].Res.PMemLimit64 = PMemBase64 + 0x2000 - 1;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  if (SysConfig.TbtPcieIoRsvd != 0){
    HrConfigs[AR_DS_PORT1].Res.IoBase    = IoBase;
    if ((BrdgConf->Res.IoLimit & 0xF0) < (IoBase + 0x50))
      HrConfigs[AR_DS_PORT1].Res.IoLimit = BrdgConf->Res.IoLimit & 0xF0;
    else
      HrConfigs[AR_DS_PORT1].Res.IoLimit = IoBase + 0x50;

    HrConfigs[AR_DS_PORT1].Res.Cmd       = CMD_BM_MEM_IO;
  } else {
    HrConfigs[AR_DS_PORT1].Res.Cmd       = CMD_BM_MEM;
  }
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
  HrConfigs[AR_DS_PORT1].SBus            = BrdgConf->SBus;
  HrConfigs[AR_DS_PORT1].SubBus          = BrdgConf->SBus + BusRange;

  BrdgConf->SBus                         = HrConfigs[AR_DS_PORT1].SubBus + 1;

  HrConfigs[AR_DS_PORT2].Res             = NOT_IN_USE_BRIDGE;
  HrConfigs[AR_DS_PORT2].Res.Cls         = SysConfig.TbtCacheLineSize;
  HrConfigs[AR_DS_PORT2].Res.Cmd         = CMD_BM_MEM;
  HrConfigs[AR_DS_PORT2].Res.MemBase     = MemBase + 0x17F0;
  HrConfigs[AR_DS_PORT2].Res.MemLimit    = MemBase + 0x1800 - 1;
  HrConfigs[AR_DS_PORT2].SBus            = BrdgConf->SBus;
  HrConfigs[AR_DS_PORT2].SubBus          = BrdgConf->SBus;

  BrdgConf->SBus                         = HrConfigs[AR_DS_PORT2].SubBus + 1;


  HrConfigs[AR_DS_PORT4].Res             = NOT_IN_USE_BRIDGE;
  HrConfigs[AR_DS_PORT4].Res.Cls         = SysConfig.TbtCacheLineSize;
  HrConfigs[AR_DS_PORT4].Res.MemBase     = MemBase + 0x1800;
  HrConfigs[AR_DS_PORT4].Res.MemLimit    = BrdgConf->Res.MemLimit;
  HrConfigs[AR_DS_PORT4].Res.PMemBase64  = PMemBase64 + 0x2000;
  HrConfigs[AR_DS_PORT4].Res.PMemLimit64 = BrdgConf->Res.PMemLimit64;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  if (SysConfig.TbtPcieIoRsvd != 0 &&
      HrConfigs[AR_DS_PORT1].Res.IoLimit != (BrdgConf->Res.IoLimit & 0xF0)){

    HrConfigs[AR_DS_PORT4].Res.IoBase     = IoBase + 0x60;
    HrConfigs[AR_DS_PORT4].Res.IoLimit    = BrdgConf->Res.IoLimit & 0xF0;

    HrConfigs[AR_DS_PORT4].Res.Cmd        = CMD_BM_MEM_IO;
  } else {
    HrConfigs[AR_DS_PORT4].Res.Cmd        = CMD_BM_MEM;
  }
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
  HrConfigs[AR_DS_PORT4].SBus             = BrdgConf->SBus;
  HrConfigs[AR_DS_PORT4].SubBus           = BrdgConf->SubBus;

  BrdgConf->SBus                          = HrConfigs[AR_DS_PORT4].SubBus + 1;
}//InitARDSPort_2Port

/**
  Init. HR downstream port if 2 port supported.

  @param  BrdgConf              BRDG_CONFIG

  @return                       No return value
**/
STATIC
VOID
InitHRDSPort_2Port (
  IN  OUT  BRDG_CONFIG          *BrdgConf
  )
{
  UINT16                        MemBase;
  UINT64                        PMemBase64;
  UINT8                         BusRange;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
  UINT8                         IoBase = BrdgConf->Res.IoBase & 0xF0;
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
  MemBase                                 = BrdgConf->Res.MemBase & 0xFFF0;
  PMemBase64                              = BrdgConf->Res.PMemBase64 &~0xFULL;
  BusRange                                = BrdgConf->SubBus - BrdgConf->PBus - (MAX_CFG_PORTS - 2); // MAX_CFG_PORTS-1(US)-1(HIA) is num of bridges in HR, on each bridge bus# is incremented

  BusRange                               -= 2 * DOCK_BUSSES; // Bus range for Dock ports
  // Rest of busses split between ports 3 and 5
  BusRange                               /= 2; // Bus range for port 3/5

  HrConfigs[HR_DS_PORT3].Res              = NOT_IN_USE_BRIDGE;
  HrConfigs[HR_DS_PORT3].Res.Cls          = SysConfig.TbtCacheLineSize;
  HrConfigs[HR_DS_PORT3].Res.MemBase      = MemBase;
  HrConfigs[HR_DS_PORT3].Res.MemLimit     = MemBase + 0x1000 - 1;
  HrConfigs[HR_DS_PORT3].Res.PMemBase64   = PMemBase64;
  HrConfigs[HR_DS_PORT3].Res.PMemLimit64  = PMemBase64 + 0x2000 - 1;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  if (SysConfig.TbtPcieIoRsvd != 0){
    HrConfigs[HR_DS_PORT3].Res.IoBase     = IoBase;
    if ((BrdgConf->Res.IoLimit & 0xF0) < (IoBase + 0x50))
      HrConfigs[HR_DS_PORT3].Res.IoLimit  = BrdgConf->Res.IoLimit & 0xF0;
    else
      HrConfigs[HR_DS_PORT3].Res.IoLimit  = IoBase + 0x50;

    HrConfigs[HR_DS_PORT3].Res.Cmd        = CMD_BM_MEM_IO;
  } else {
    HrConfigs[HR_DS_PORT3].Res.Cmd        = CMD_BM_MEM;
  }
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
  HrConfigs[HR_DS_PORT3].SBus             = BrdgConf->SBus;
  HrConfigs[HR_DS_PORT3].SubBus           = BrdgConf->SBus + BusRange;

  BrdgConf->SBus                          = HrConfigs[HR_DS_PORT3].SubBus + 1;

  HrConfigs[HR_DS_PORT4].Res              = NOT_IN_USE_BRIDGE;
  HrConfigs[HR_DS_PORT4].Res.Cmd          = CMD_BM_MEM;
  HrConfigs[HR_DS_PORT4].Res.Cls          = SysConfig.TbtCacheLineSize;
  HrConfigs[HR_DS_PORT4].Res.MemBase      = MemBase + 0x1000;
  HrConfigs[HR_DS_PORT4].Res.MemLimit     = MemBase + 0x1800 - 1;
  HrConfigs[HR_DS_PORT4].Res.PMemBase64   = PMemBase64 + 0x2000;
  HrConfigs[HR_DS_PORT4].Res.PMemLimit64  = PMemBase64 + 0x2800 - 1;
  HrConfigs[HR_DS_PORT4].SBus             = BrdgConf->SBus;
  HrConfigs[HR_DS_PORT4].SubBus           = BrdgConf->SBus + DOCK_BUSSES;

  BrdgConf->SBus                          = HrConfigs[HR_DS_PORT4].SubBus + 1;

  HrConfigs[HR_DS_PORT5].Res              = NOT_IN_USE_BRIDGE;
  HrConfigs[HR_DS_PORT5].Res.Cls          = SysConfig.TbtCacheLineSize;
  HrConfigs[HR_DS_PORT5].Res.MemBase      = MemBase + 0x1800;
  HrConfigs[HR_DS_PORT5].Res.MemLimit     = MemBase + 0x2600 - 1;
  HrConfigs[HR_DS_PORT5].Res.PMemBase64   = PMemBase64 + 0x3000;
  HrConfigs[HR_DS_PORT5].Res.PMemLimit64  = PMemBase64 + 0x4A00 - 1;
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  if (SysConfig.TbtPcieIoRsvd != 0 &&
      HrConfigs[HR_DS_PORT3].Res.IoLimit != (BrdgConf->Res.IoLimit & 0xF0)){
    HrConfigs[HR_DS_PORT5].Res.IoBase     = IoBase + 0x60;
    HrConfigs[HR_DS_PORT5].Res.IoLimit    = BrdgConf->Res.IoLimit & 0xF0;

    HrConfigs[HR_DS_PORT5].Res.Cmd        = CMD_BM_MEM_IO;
  } else {
    HrConfigs[HR_DS_PORT5].Res.Cmd        = CMD_BM_MEM;
  }
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
  HrConfigs[HR_DS_PORT5].SBus             = BrdgConf->SBus;
  HrConfigs[HR_DS_PORT5].SubBus           = BrdgConf->SBus + BusRange;

  BrdgConf->SBus                          = HrConfigs[HR_DS_PORT5].SubBus + 1;

  HrConfigs[HR_DS_PORT6].Res              = NOT_IN_USE_BRIDGE;
  HrConfigs[HR_DS_PORT6].Res.Cmd          = CMD_BM_MEM;
  HrConfigs[HR_DS_PORT6].Res.Cls          = SysConfig.TbtCacheLineSize;
  HrConfigs[HR_DS_PORT6].Res.MemBase      = MemBase + 0x2600;
  HrConfigs[HR_DS_PORT6].Res.MemLimit     = MemBase + 0x2E00 - 1;
  HrConfigs[HR_DS_PORT6].Res.PMemBase64   = PMemBase64 + 0x2800;
  HrConfigs[HR_DS_PORT6].Res.PMemLimit64  = PMemBase64 + 0x3000 - 1;
  HrConfigs[HR_DS_PORT6].SBus             = BrdgConf->SBus;
  HrConfigs[HR_DS_PORT6].SubBus           = BrdgConf->SBus + DOCK_BUSSES;

  BrdgConf->SBus                          = HrConfigs[HR_DS_PORT6].SubBus + 1;
} // InitHRDSPort_2Port

/**
  Check HR resource to meet Thunderbolt requirement

  @param  Is2PortDev            TRUE: 2 ports; FALSE: 1 port
  @param  HrResConf             Pointer to BRDG_RES_CONFIG
  @param  BusRange              BUS resource

  @retval TRUE                  Resource is enough
  @retval FALSE                 Resource is not enough
**/
STATIC
BOOLEAN
CheckLimits (
  IN    BOOLEAN                 Is2PortDev,
  IN    BRDG_RES_CONFIG         *HrResConf,
  IN    UINT8                   BusRange
  )
{
  UINT16                        MemBase;
  UINT16                        MemLimit;
  UINT64                        PMemBase64;
  UINT64                        PMemLimit64;

  MemBase     = HrResConf->MemBase & 0xFFF0;
  MemLimit    = HrResConf->MemLimit & 0xFFF0;
  PMemBase64  = HrResConf->PMemBase64 & 0xFFF0;
  PMemLimit64 = HrResConf->PMemLimit64 & 0xFFF0;
  //
  // Check memoty alignment
  //
  if (MemBase & 0x3FF) {
    PrintCpStr ("M alig");
    return FALSE;
  }

  if (PMemBase64 & 0xFFF) {
    PrintCpStr ("PM alig");
    return FALSE;
  }

  if (Is2PortDev) {
    //
    // Check mem size
    //
    if (MemLimit + 0x10 - MemBase < 0x2E00) {
      PrintCpStr ("M size");
      return FALSE;
    }
    //
    // Check P-mem size
    //
    if (PMemLimit64 + 0x10 - PMemBase64 < 0x4A00) {
      PrintCpStr ("PM size");
      return FALSE;
    }
    //
    // Check bus range
    //
    if (BusRange < 106) {
      PrintCpStr ("Bus range");
      return FALSE;
    }
  } else {
    //
    // Check mem size
    //
    if (MemLimit + 0x10 - MemBase < 0x1600) {
      PrintCpStr ("M size");
      return FALSE;
    }
    //
    // Check P-mem size
    //
    if (PMemLimit64 + 0x10 - PMemBase64 < 0x2200) {
      PrintCpStr ("PM size");
      return FALSE;
    }
    //
    // Check bus range
    //
    if (BusRange < 56) {
      PrintCpStr ("Bus range");
      return FALSE;
    }
  }

  return TRUE;
} // CheckLimits

/**
  Init. HR resource configuration

  @param  Hr_Config             Pointer to HR_CONFIG
  @param  BusNumLimit           BUS number limit
  @param  HrResConf             Pointer to BRDG_RES_CONFIG

  @retval TRUE                  Init. successfully
  @retval FALSE                 Resource is not enough
**/
STATIC
BOOLEAN
InitHRResConfigs (
  IN  OUT  HR_CONFIG            *Hr_Config,
  IN       UINT8                BusNumLimit,
  IN  OUT  BRDG_RES_CONFIG      *HrResConf
  )
{
  BRDG_CONFIG                   BrdgConf = { { 0 } };

  InitCommonHRConfigs (Hr_Config, BusNumLimit, HrResConf);
  BrdgConf.PBus   = Hr_Config->HRBus + 2;// Take into account busses
  BrdgConf.SBus   = Hr_Config->HRBus + 3;// for US and DS of HIA
  BrdgConf.SubBus = BusNumLimit;
  BrdgConf.Res    = *HrResConf;
  while (TRUE) {
    switch (Hr_Config->DeviceId) {
    case CR_HR_4C:
    case RR_HR_4C:
    case FR_HR_4C:
    case AR_HR_4C:
    case TR_HR_4C:
    case AR_HR_C0_4C:
      //
      // 2 Port host
      //
      if (CheckLimits (TRUE, HrResConf, BusNumLimit - Hr_Config->HRBus)) {
        switch(Hr_Config->DeviceId)
        {
          case AR_HR_4C:
          case TR_HR_4C:
          case AR_HR_C0_4C:
            InitARDSPort_2Port(&BrdgConf);
            PrintCpStr("AR2");
            break;
          default:
            InitHRDSPort_2Port (&BrdgConf);
            PrintCpStr("2P");
            break;
          }
        return TRUE;
      } else {
        Hr_Config->DeviceId = 0;
        //
        // Jump to default on next loop
        //
        continue;
      }
    // AR only
  case AR_HR_2C: // 1 port host
  case AR_HR_C0_2C:
  case AR_HR_LP:
  case TR_HR_2C:
    PrintCpStr("AR1");
    InitARDSPort_1Port(&BrdgConf);
    return TRUE;

    case CR_HR_2C:
    case RR_HR_2C:
    case FR_HR_2C:
      //
      // 1 Port host
      //
      if (CheckLimits (FALSE, HrResConf, BusNumLimit - Hr_Config->HRBus)) {
        InitHRDSPort_1Port (&BrdgConf);
        InitHRDSPort_Disable (HR_DS_PORT5, &BrdgConf);
        InitHRDSPort_Disable (HR_DS_PORT6, &BrdgConf);
        return TRUE;
      }

    case WR_HR_2C:
      //
      // 1 Port host
      //
      return TRUE;

    default:
      InitHRDSPort_Disable (HR_DS_PORT3, &BrdgConf);
      InitHRDSPort_Disable (HR_DS_PORT4, &BrdgConf);
      InitHRDSPort_Disable (HR_DS_PORT5, &BrdgConf);
      InitHRDSPort_Disable (HR_DS_PORT6, &BrdgConf);
      return FALSE;
    }
  }
} // InitHRResConfigs

/**
  Init. HR (Host Router)

  @param  Hr_Config             Pointer to HR_CONFIG
  @param  RpNumber              Root port number

  @retval TRUE                  Init. successfully
  @retval FALSE                 Not TBT HR or resource not enough
**/
STATIC
BOOLEAN
InitializeHostRouter (
      OUT  HR_CONFIG            *Hr_Config,
  IN       UINT32               RpNumber
  )
{
  UINT8                         BusNumLimit;
  BRDG_RES_CONFIG               HrResConf = { 0 };
  UINT8                         i;
  BOOLEAN                       Ret;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  EFI_STATUS                    Status;
//AMI_OVERRIDE_START - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler
  UINT16                        MemBase;
  UINT16                        MemLimit;
  UINT64                        PMemBase64;
  UINT64                        PMemLimit64;
  UINTN                         Address;
//AMI_OVERRIDE_END - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler

  Ret = TRUE;
  Status = GetTbtRpDevFun(RpNumber - 1, &RpDev, &RpFunc);
  DeviceBaseAddress   = MmPciBase (0x00,(UINT32) RpDev, (UINT32) RpFunc);
  PrintCpStr("Dev");
  PrintCp32 ((UINT32) RpDev);
  PrintCpStr("RpFunc");
  PrintCp32 ((UINT32) RpFunc);
  Hr_Config->HRBus    = MmioRead8 (DeviceBaseAddress + PCI_SBUS);
  DeviceBaseAddress   = MmPciBase (Hr_Config->HRBus, 0x00, 0x00);
  Hr_Config->DeviceId = MmioRead16 (DeviceBaseAddress + PCI_DID);
  PrintCpStr("DeviceId");
  PrintCp16 ((UINT16) Hr_Config->DeviceId);
  if (!(IsTbtHostRouter (Hr_Config->DeviceId))) {
      return FALSE;
    }

//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
  if(mGlobalNvsAreaPtr->TbtXConnectSupport) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
    //
    // Fill Structure size
    //
    PrintCpStr("FillATbtDeviceTree.Begin_InHR");
    FillTbtDeviceTreeBuffer(0, 2);
    //
    // Fill Root port information structure
    //
    FillATbtDeviceTree(2, 0, (UINT8) RpDev,(UINT8)  RpFunc);
//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
    DEBUG ((DEBUG_INFO, "[DGPU]Thunderbolt RP at [%03d:%02d:%d] -> (BufferTree: 2)\n", 0x00, RpDev, RpFunc));
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
    //
    // Fill Host Router Upstream port information structure
    //
    FillATbtDeviceTree(12, Hr_Config->HRBus, 0x00, 0x00);
//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
    DEBUG ((DEBUG_INFO, "[DGPU]Thunderbolt HR at [%03d:%02d:%d] -> (BufferTree: 12)\n", Hr_Config->HRBus, 0, 0));
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
    PrintCpStr("FillATbtDeviceTree.End_InHR");
  }
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
  if (SysConfig.TbtPcieIoRsvd == 0){
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
    HrResConf.Cmd          = CMD_BM_MEM;
  } else {
    HrResConf.Cmd          = CMD_BM_MEM_IO;
  }
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
  HrResConf.Cls          = SysConfig.TbtCacheLineSize;
  DeviceBaseAddress      = MmPciBase (0,(UINT8)  RpDev,(UINT8)  RpFunc);
  HrResConf.IoBase       = MmioRead8 (DeviceBaseAddress + PCI_IOBASE);
  HrResConf.IoLimit      = MmioRead8 (DeviceBaseAddress + PCI_IOLIMIT);
  HrResConf.MemBase      = MmioRead16 (DeviceBaseAddress + PCI_MEMBASE);
  HrResConf.MemLimit     = MmioRead16 (DeviceBaseAddress + PCI_MEMLIMIT);
  HrResConf.PMemBase64   = MmioRead16 (DeviceBaseAddress + PCI_PRE_MEMBASE);
  HrResConf.PMemLimit64  = MmioRead16 (DeviceBaseAddress + PCI_PRE_MEMLIMIT);
  HrResConf.PMemBase64  |= (UINT64)(MmioRead32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U)) << 16;
  HrResConf.PMemLimit64 |= (UINT64)(MmioRead32 (DeviceBaseAddress + PCI_PRE_MEMLIMIT_U)) << 16;
  BusNumLimit = MmioRead8 (DeviceBaseAddress + PCI_SUBUS);


//AMI_OVERRIDE_START - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler
  MemBase                = HrResConf.MemBase & 0xFFF0;
  MemLimit               = HrResConf.MemLimit & 0xFFF0;
  PMemBase64             = HrResConf.PMemBase64 & 0xFFFFFFFFFFF0;
  PMemLimit64            = HrResConf.PMemLimit64 & 0xFFFFFFFFFFF0;

  //
  // WSMT validation - Valid root port MMIO
  //
  Address = (MemBase << 16) & 0xffffffff;  //UINT32 only
  if (EFI_ERROR (CheckMmio (0xB1, Address, ((MemLimit + 0x10 - MemBase) << 16)))) {
    DEBUG ((DEBUG_INFO, "TbtSmm: WSMT validation is invalid on MEM\n"));
    return FALSE;
  }
  //
  // WSMT validation - Valid root port Prefetch
  //
  Address = (PMemBase64 << 16);
  if (EFI_ERROR (CheckMmio (0xB2, Address, ((PMemLimit64 + 0x10 - PMemBase64) << 16)))) {
    DEBUG ((DEBUG_INFO, "TbtSmm: WSMT validation is invalid on P-MEM\n"));
    return FALSE;
  }
//AMI_OVERRIDE_END - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler

  Ret         = InitHRResConfigs (Hr_Config, BusNumLimit, &HrResConf);

  for (i = 0; i < Hr_Config->BridgeLoops; ++i) {
    UINT8 Bus;
    UINT8 Dev;
    UINT8 Fun;
    Bus               = HrConfigs[i].DevId.Bus;
    Dev               = HrConfigs[i].DevId.Dev;
    Fun               = HrConfigs[i].DevId.Fun;
    DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);

    MmioWrite8 (DeviceBaseAddress + PCI_CLS, HrConfigs[i].Res.Cls);
    MmioWrite8 (DeviceBaseAddress + PCI_PBUS, HrConfigs[i].PBus);
    MmioWrite8 (DeviceBaseAddress + PCI_SBUS, HrConfigs[i].SBus);
    MmioWrite8 (DeviceBaseAddress + PCI_SUBUS, HrConfigs[i].SubBus);
    MmioWrite16 (DeviceBaseAddress + PCI_MEMBASE, HrConfigs[i].Res.MemBase);
    MmioWrite16 (DeviceBaseAddress + PCI_MEMLIMIT, HrConfigs[i].Res.MemLimit);
    MmioWrite16 (DeviceBaseAddress + PCI_PRE_MEMBASE, (UINT16) (HrConfigs[i].Res.PMemBase64 & 0xFFFF));
    MmioWrite16 (DeviceBaseAddress + PCI_PRE_MEMLIMIT, (UINT16) (HrConfigs[i].Res.PMemLimit64 & 0xFFFF));
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U, (UINT32) (HrConfigs[i].Res.PMemBase64 >> 16));
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMLIMIT_U, (UINT32) (HrConfigs[i].Res.PMemLimit64 >> 16));
    MmioWrite8 (DeviceBaseAddress + PCI_IOBASE, HrConfigs[i].Res.IoBase);
    MmioWrite8 (DeviceBaseAddress + PCI_IOLIMIT, HrConfigs[i].Res.IoLimit);
    MmioWrite32 (DeviceBaseAddress + PCI_IOBASE_U, 0x00000000);
    MmioWrite8 (DeviceBaseAddress + PCI_CMD, HrConfigs[i].Res.Cmd);

  }

  DeviceBaseAddress = MmPciBase ((Hr_Config->HRBus + 2), 0x00, 0x00);
  MmioWrite32 (DeviceBaseAddress + PCI_BAR0, HrConfigs[HR_DS_PORT0].Res.MemLimit << 16);
  MmioWrite32 (DeviceBaseAddress + PCI_BAR1, (HrConfigs[HR_DS_PORT0].Res.MemLimit + 0x4) << 16);
  MmioWrite8 (DeviceBaseAddress + PCI_CLS, SysConfig.TbtCacheLineSize);
  MmioWrite8 (DeviceBaseAddress + PCI_CMD, CMD_BM_MEM);
  return Ret;
} // InitializeHostRouter

//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
/**
  Configure Slot

  @param  Bus                   Current bus number
  @param  MAX_DEVICE            Max device number
  @param  Depth                 The depth of daisy chain
  @param  ArPcie                TRUE: AR
  @param  Pi                    Pointer to PortInfo
  @param  DetectGfxInserted     TRUE: GFX inserted

  @return                       Remain bus number
**/
STATIC
UINT8
ConfigureSlot (
  IN       UINT8                Bus,
  IN       UINT8                MAX_DEVICE,
  IN       INT8                 Depth,
  IN       BOOLEAN              ArPcie,
  IN  OUT  PortInfo             *Pi,
      OUT  BOOLEAN              *DetectGfxInserted
  )
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
{
  UINT8                         Device;
  UINT8                         SBus;
  UINT8                         UsedBusNumbers;
  UINT8                         RetBusNum;
  PortInfo                      CurrentSlot;

  RetBusNum = 0;
//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
  (*DetectGfxInserted) = FALSE;
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"

  for (Device = 0; Device < MAX_DEVICE; Device++) {
    //
    // Continue if device is absent
    //
    DeviceBaseAddress = MmPciBase (Bus, Device, 0x00);
    if (0xFFFF == MmioRead16 (DeviceBaseAddress + PCI_DID)) {
      continue;

    }

    if (P2P_BRIDGE != MmioRead16 (DeviceBaseAddress + PCI_SCC)) {
//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
      SetDevResources (
        Bus,
        Device,
        PCI_MAX_FUNC,
        PCI_BAR5,
        Pi,
        DetectGfxInserted
        );
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
      continue;
    }
    //
    // Else Bridge
    //
    CopyMem (&CurrentSlot, Pi, sizeof (PortInfo));

    ++RetBusNum; // UP Bridge
    SBus = Bus + RetBusNum; // DS Bridge

    if (SBus + 1 >= Pi->BusNumLimit) {
      continue;

    }

//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
//    SetDevResources (Bus, Device, 1, PCI_BAR1, Pi);
    SetDevResources (Bus, Device, 1, PCI_BAR1, Pi, DetectGfxInserted);
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"

    //
    // Init UP Bridge to reach DS Bridge
    //
    MmioWrite8 (DeviceBaseAddress + PCI_PBUS, Bus);
    MmioWrite8 (DeviceBaseAddress + PCI_SBUS, SBus);
    MmioWrite8 (DeviceBaseAddress + PCI_SUBUS, Pi->BusNumLimit);
    MmioWrite8 (DeviceBaseAddress + PCI_CMD, CMD_BM_MEM);

    if(ArPcie) {
//AMI_OVERRIDE_START - EIP317127 - Implement "Thunderbolt Dock with NIC Hard Hang BIOS Workaround"
      SetSoftL1 (Bus, Device, 0x00);
//AMI_OVERRIDE_END - EIP317127 - Implement "Thunderbolt Dock with NIC Hard Hang BIOS Workaround"
    }

//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
    UsedBusNumbers = ConfigureSlot(SBus, PCI_MAX_DEVICE + 1, -1, FALSE, Pi, DetectGfxInserted);
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
    if ((*DetectGfxInserted) == TRUE && mGlobalNvsAreaPtr->TbtXConnectSupport) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
      FillATbtDeviceTree (42, Bus, Device, 0);
      DEBUG ((DEBUG_INFO, "[DGPU]GFX at endpoint DS [%03d:%02d:%d] -> (BufferTree: 42)\n", Bus, Device, 0));
    }
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"

    RetBusNum += UsedBusNumbers;

    SetPhyPortResources (
      Bus,
      Device,
      SBus + UsedBusNumbers,
      Depth,
      &CurrentSlot,
      Pi
      );
  }
  //
  // for (Device = 0; Device <= PCI_MAX_DEVICE; Device++)
  //
  return RetBusNum;
} // ConfigureSlot

/**
  Set CIO port resources

  @param  Bus                   Current bus number
  @param  Dev                   Current device number
  @param  SBus                  Assigned secondary bus number
  @param  SubBus                Assigned subordinate bus number
  @param  portInfoBeforeChange  PortInfo before change
  @param  Pi                    Pointer to PortInfo

  @return                       No return value
**/
STATIC
VOID
SetCioPortResources (
  IN       UINT8                Bus,
  IN       UINT8                Dev,
  IN       UINT8                SBus,
  IN       UINT8                SubBus,
  IN       PortInfo             *portInfoBeforeChange,
  IN  OUT  PortInfo             *Pi
  )
{
  UINT8 Cmd;
  Cmd               = CMD_BUS_MASTER;

  DeviceBaseAddress = MmPciBase (Bus, Dev, 0x00);
  MmioWrite8 (DeviceBaseAddress + PCI_PBUS, Bus);
  MmioWrite8 (DeviceBaseAddress + PCI_SBUS, SBus);
  MmioWrite8 (DeviceBaseAddress + PCI_SUBUS, SubBus);
  MmioWrite8 (DeviceBaseAddress + PCI_CMD, Cmd);

  if (Pi->IoBase <= Pi->IoLimit) {
    MmioWrite8 (DeviceBaseAddress + PCI_IOBASE, Pi->IoBase);
    MmioWrite8 (DeviceBaseAddress + PCI_IOLIMIT, Pi->IoLimit);
    MmioWrite32 (DeviceBaseAddress + PCI_IOBASE_U, 0x00000000);
    Cmd |= CMD_BM_IO;
  } else {
    MmioWrite16 (DeviceBaseAddress + PCI_IOBASE, DISBL_IO_REG1C);
  }

  if (Pi->MemBase <= Pi->MemLimit) {
    MmioWrite16 (DeviceBaseAddress + PCI_MEMBASE, Pi->MemBase);
    MmioWrite16 (DeviceBaseAddress + PCI_MEMLIMIT, Pi->MemLimit);
    Cmd |= CMD_BM_MEM;
  } else {
    MmioWrite32 (DeviceBaseAddress + PCI_MEMBASE, DISBL_MEM32_REG20);
  }

  if (Pi->PMemBase64 <= Pi->PMemLimit64) {
    MmioWrite16 (DeviceBaseAddress + PCI_PRE_MEMBASE, (UINT16) (Pi->PMemBase64 & 0xFFFF));
    MmioWrite16 (DeviceBaseAddress + PCI_PRE_MEMLIMIT, (UINT16) (Pi->PMemLimit64 & 0xFFFF));
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U, (UINT32) (Pi->PMemBase64 >> 16));
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMLIMIT_U, (UINT32) (Pi->PMemLimit64 >> 16));
    Cmd |= CMD_BM_MEM;
  } else {
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE, DISBL_PMEM_REG24);
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U, 0);
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMLIMIT_U, 0);
  }

  MmioWrite8 (DeviceBaseAddress + PCI_CMD, Cmd);
  MmioWrite8 (DeviceBaseAddress + PCI_CLS, SysConfig.TbtCacheLineSize);
} // SetCioPortResources

/**
  Set slot as unused when slot number is bigger than CioSlot.

  @param  Bus                   Current bus number
  @param  MaxSlotNum            Max slot number
  @param  CioSlot               After this CIO slot number, the slot will set as unused.
  @param  Pi                    Pointer to PortInfo

  @return                       No return value
**/
STATIC
VOID
SetSlotsAsUnused (
  IN       UINT8                Bus,
  IN       UINT8                MaxSlotNum,
  IN       UINT8                CioSlot,
  IN  OUT  PortInfo             *Pi
  )
{
  UINT8 Slot;
  for (Slot = MaxSlotNum; Slot > CioSlot; --Slot) {
    DeviceBaseAddress = MmPciBase (Bus, Slot, 0x00);
    if (0xFFFF == MmioRead16 (DeviceBaseAddress + PCI_DID)) {
      continue;
    }

    MmioWrite8 (DeviceBaseAddress + PCI_CLS, SysConfig.TbtCacheLineSize);
    MmioWrite8 (DeviceBaseAddress + PCI_PBUS, Bus);
    MmioWrite8 (DeviceBaseAddress + PCI_SBUS, Pi->BusNumLimit);
    MmioWrite8 (DeviceBaseAddress + PCI_SUBUS, Pi->BusNumLimit);
    MmioWrite16 (DeviceBaseAddress + PCI_IOBASE, DISBL_IO_REG1C);
    MmioWrite32 (DeviceBaseAddress + PCI_MEMBASE, DISBL_MEM32_REG20);
    MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE, DISBL_PMEM_REG24);
    MmioWrite8 (DeviceBaseAddress + PCI_CMD, CMD_BUS_MASTER);
    Pi->BusNumLimit--;
  }
} // SetSlotsAsUnused

/**
  Find vendor specific header which extend capability ID is PCIE_CAP_ID_VSEC (0x000B).

  @param  Bus                   Current bus number

  @return                       Extended register offset
**/
STATIC
UINT16
FindVendorSpecificHeader (
  IN  UINT8                     Bus
  )
{
  PCIE_EXT_CAP_HDR              ExtCap;
  UINT16                        ExtendedRegister;
  ExtendedRegister  = 0x100;

  DeviceBaseAddress = MmPciBase (Bus, 0x00, 0x00);
  while (ExtendedRegister) {
    ExtCap.EXT_CAP_HDR = MmioRead32 (DeviceBaseAddress + ExtendedRegister);
    if (ExtCap.W.ExtCapId == 0xFFFF) {
      return 0x0000; // No Vendor-Specific Extended Capability header
    }

    if (PCIE_CAP_ID_VSEC == ExtCap.W.ExtCapId) {
      return ExtendedRegister;

    }

    ExtendedRegister = ExtCap.W.NextItemPtr;
  }
  return 0x0000; // No Vendor-Specific Extended Capability header
}

/**
  Find specific capability ID which is PCIE_CAP_ID_SSID_SSVID (0x0D).

  @param  Bus                   Current bus number

  @return                       Capability register offset
**/
STATIC
UINT8
FindSsid_SsvidHeader (
  IN    UINT8                   Bus
  )
{
  UINT8                         CapHeaderId;
  UINT8                         CapHeaderOffset;

  DeviceBaseAddress = MmPciBase (Bus, 0x00, 0x00);
  CapHeaderOffset   = MmioRead8 (DeviceBaseAddress + PCI_CAPP);

  while (CapHeaderOffset != 0) {
    CapHeaderId = MmioRead8 (DeviceBaseAddress + CapHeaderOffset);

    if (CapHeaderId == PCIE_CAP_ID_SSID_SSVID) {
      return CapHeaderOffset;
    }

    CapHeaderOffset = MmioRead8 (DeviceBaseAddress + CapHeaderOffset + 1);
  }

  PrintCpStr ("SID0");
  return 0;
} // FindSsid_SsvidHeader

/**
  Get CIO slot number, max slot number, and AR PCIe flag through device ID.

  @param  Bus                   Current bus number
  @param  CioSlot               CIO slot number
  @param  MaxSlotNum            Max slot number
  @param  ArPcie                AR PCIe flag

  @retval TRUE                  Cio SLOT is present in EP
  @retval FALSE                 Cio SLOT is not present in EP
**/
STATIC
BOOLEAN
GetCioSlotByDevId (
  IN   UINT8                    Bus,
  OUT  UINT8                    *CioSlot,
  OUT  UINT8                    *MaxSlotNum,
  OUT  BOOLEAN                  *ArPcie
  )
{
  UINT16                        VSECRegister;
  BRDG_CIO_MAP_REG              BridgMap;
  UINT32                        BitScanRes;
  UINT16                        DevId;
  DeviceBaseAddress = MmPciBase (Bus, 0x00, 0x00);
  DevId             = MmioRead16 (DeviceBaseAddress + PCI_DID);

  //
  // Init out params in case device is not recognised
  //
  *CioSlot    = 4;
  *MaxSlotNum = 7;
  *ArPcie     = FALSE;

  switch (DevId) {
    //
    // For known device IDs
    //
    case 0x1578:
      *ArPcie = TRUE;
  }

  switch (DevId) {
  //
  // For known device IDs
  //
  case 0x1513:
  case 0x151A:
  case 0x151B:
  case 0x1547:
  case 0x1548:
    return TRUE; // Just return
  case 0x1549:
    return FALSE; // Just return
  }

  VSECRegister = FindVendorSpecificHeader(Bus);
  if (!VSECRegister) {
    return TRUE; // Just return
  }
  //
  // Go to Bridge/CIO map register
  //
  VSECRegister += 0x18;
  BridgMap.AB_REG = MmioRead32(DeviceBaseAddress + VSECRegister);
  //
  // Check for range
  //
  if (BridgMap.Bits.NumOfDSPorts < 1 || BridgMap.Bits.NumOfDSPorts > 27) {
    return TRUE;
  //
  // Not a valid register
  //
  }
  //
  // Set OUT params
  //
  *MaxSlotNum = (UINT8) BridgMap.Bits.NumOfDSPorts;

#ifdef _MSC_VER
  if(!_BitScanForward(&BitScanRes, BridgMap.Bits.CioPortMap)) { // No DS bridge which is CIO port
    return FALSE;
  }
#else
#ifdef __GNUC__
  if (BridgMap.Bits.CioPortMap == 0) {
    return FALSE;
  }
  BitScanRes = __builtin_ctz (BridgMap.Bits.CioPortMap);
#else
#error Unsupported Compiler
#endif
#endif

  *CioSlot = (UINT8)BitScanRes;
  return TRUE;
} // GetCioSlotByDevId

#define TBT_LEGACY_SUB_SYS_ID 0x11112222

/**
  Check the device is legacy TBT device or not.

  @param  Bus                   Current bus number

  @retval TRUE                  Legacy TBT device
  @retval FALSE                 Not legacy TBT device
**/
STATIC
BOOLEAN
IsLegacyDevice (
  IN    UINT8                   Bus
  )
{
  UINT32                        Sid;
  UINT8                         SidRegister;
  UINT16                        DevId;

  DeviceBaseAddress = MmPciBase (Bus, 0x00, 0x00);
  DevId             = MmioRead16 (DeviceBaseAddress + PCI_DID);
  switch (DevId) {
  //
  // For known device IDs
  //
  case 0x1513:
  case 0x151A:
  case 0x151B:
    PrintCpStr ("Le");
    PrintCp16 (
      DevId
      );
    return TRUE;
    //
    // Legacy device by Device Id
    //
  }

  SidRegister = FindSsid_SsvidHeader(Bus);

  if (!SidRegister) {
    return TRUE; // May be absent for legacy devices
  }
  //
  // Go to register
  //
  SidRegister += 0x4;
  Sid = MmioRead32(DeviceBaseAddress + SidRegister);
  PrintCpStr("SID");
  PrintCp32 (
    Sid
    );

return TBT_LEGACY_SUB_SYS_ID == Sid || 0 == Sid;
} // IsLegacyDevice

/**
  Unset vendor specific enhanced capability register by use SetSoftL1.

  @param  Bus                   Current bus number
  @param  MaxSlotNum            Max slot number

  @return                       No return value
**/
STATIC
VOID
UnsetVescEp(
  IN  UINT8                     Bus,
  IN  UINT8                     MaxSlotNum
  )
{
  UINT8                         i;

  for (i = 0; i <= MaxSlotNum; ++i)
  {
//AMI_OVERRIDE_START - EIP317127 - Implement "Thunderbolt Dock with NIC Hard Hang BIOS Workaround"
    SetSoftL1 (Bus, i, 0);
//AMI_OVERRIDE_END - EIP317127 - Implement "Thunderbolt Dock with NIC Hard Hang BIOS Workaround"
  }
}// Unset_VESC_REG2_EP

/**
  Configure EP

  @param  Depth                 The depth of daisy chain
  @param  Bus                   Current bus number
  @param  Pi                    Pointer to PortInfo

  @retval TRUE                  Continue resource assignment to next chain
  @retval FALSE                 Stop resource assignment on this chain
**/
STATIC
BOOLEAN
ConfigureEP (
  IN       INT8                 Depth,
  IN  OUT  UINT8                *Bus,
  IN  OUT  PortInfo             *Pi
  )
{
  UINT8                         SBus;
  UINT8                         CioSlot;
  UINT8                         MaxSlotNum;
  BOOLEAN                       ArPcie;
  UINT8                         MaxPHYSlots;
  UINT8                         UsedBusNumbers;
  UINT8                         cmd;
  BOOLEAN                       CioSlotPresent;
  BOOLEAN                       Continue;
  PortInfo                      portInfo;
  UINT8                         CioBus;
//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
  BOOLEAN                       DetectGfxInserted = FALSE;
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
//AMI_OVERRIDE_START - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler
  UINTN                         CheckedAddress;
  UINTN                         CheckedSize;
//AMI_OVERRIDE_END - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler

  PrintCpStr("ConfigureEP");
  PrintCpStr("Depth");
  PrintCp(Depth);
  PrintCpStr("InputBus");
  PrintCp(*Bus);

  CioSlot     = 4;
  MaxSlotNum  = 7;
  CopyMem (&portInfo, Pi, sizeof (PortInfo));

  DeviceBaseAddress = MmPciBase (*Bus, 0x00, 0x00);
  cmd               = MmioRead8 (DeviceBaseAddress + PCI_CMD);
  // AR ONLY
  // Endpoint on CIO slot, but not a bridge device
  if (P2P_BRIDGE != MmioRead16 (DeviceBaseAddress + PCI_SCC)) {
    PrintCpStr("UEP");
    // Check whether EP already configured by examining CMD register
    //cmd = MmPci8  (0x00, *Bus, 0x00, 0x00, PCI_CMD);
    cmd = MmioRead8 (DeviceBaseAddress + PCI_CMD);
// AMI_OVERRIDE_START - Fix first tbt hot-plug fail.
    if(IsFirstEnterFlag) cmd &= 0;
// AMI_OVERRIDE_END - Fix first tbt hot-plug fail.
    if(cmd & CMD_BUS_MASTER) // Yes, no need to touch this EP
    {
      PrintCpStr("BMF");
      return FALSE;
    }
    // Configure it as regular PCIe device
//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
    ConfigureSlot(*Bus, PCI_MAX_DEVICE + 1, -1, FALSE, Pi, &DetectGfxInserted);
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"

    return FALSE;
  }

  //
  // Based on Device ID assign Cio slot and max number of PHY slots to scan
  //
  CioSlotPresent  =  GetCioSlotByDevId(*Bus, &CioSlot, &MaxSlotNum, &ArPcie);
  MaxPHYSlots     = MaxSlotNum;
  //
  // Check whether EP already configured by examining CMD register
  //

// AMI_OVERRIDE_START - Fix first tbt hot-plug fail.
  if(IsFirstEnterFlag) cmd &= 0;
// AMI_OVERRIDE_END - Fix first tbt hot-plug fail.
  if (cmd & CMD_BUS_MASTER) {
    //
    // Yes no need to touch this EP, just move to next one in chain
    //
    CioBus = *Bus + 1;
    if(ArPcie){
      UnsetVescEp(CioBus, MaxSlotNum);
    }
    if (!CioSlotPresent) {
      //
      // Cio slot is not present in EP, just return FALSE
      //
      PrintCpStr ("BMF");
      return FALSE;
    }
    //
    // Take all resources from Cio slot and return
    //
    DeviceBaseAddress = MmPciBase (CioBus, CioSlot, 0x00);


    Pi->BusNumLimit   = MmioRead8 (DeviceBaseAddress + PCI_SUBUS);
    Pi->IoBase        = MmioRead8 (DeviceBaseAddress + PCI_IOBASE);
    Pi->IoLimit       = MmioRead8 (DeviceBaseAddress + PCI_IOLIMIT);
    Pi->MemBase       = MmioRead16 (DeviceBaseAddress + PCI_MEMBASE);
    Pi->MemLimit      = MmioRead16 (DeviceBaseAddress + PCI_MEMLIMIT);
    Pi->PMemBase64    = MmioRead16 (DeviceBaseAddress + PCI_PRE_MEMBASE) & 0xFFF0;
    Pi->PMemLimit64   = MmioRead16 (DeviceBaseAddress + PCI_PRE_MEMLIMIT) & 0xFFF0;
    Pi->PMemBase64   |= (UINT64)(MmioRead32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U)) << 16;
    Pi->PMemLimit64  |= (UINT64)(MmioRead32 (DeviceBaseAddress + PCI_PRE_MEMLIMIT_U)) << 16;
    Pi->PMemLimit64  |= 0xF;

//AMI_OVERRIDE_START - EIP336316 - Improve WSMT validation on Thunderbolt
//AMI_OVERRIDE_START - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler
    //
    // Check whether MMIO Registers were modified by malicious attack or not
    // Pi->MemBase > Pi->MemLimit: Range is incorrect, OS will not catch this MMIO
    // Depth + 1 < MAX_TBT_DEPTH: The last node within the Daisy Chain,
    //                            CIO can't accept anything
    //
    if (Pi->MemBase <= Pi->MemLimit && Depth + 1 < MAX_TBT_DEPTH) {
      CheckedAddress    = ((Pi->MemBase & 0xFFF0) << 16) & 0xFFFFFFFF;  //UINT32 only
      CheckedSize       = ((Pi->MemLimit & 0xFFF0) - (Pi->MemBase & 0xFFF0) + 0x10) << 16;
      if (EFI_ERROR (CheckMmio (0xB0 | Depth, CheckedAddress, CheckedSize))) {
        DEBUG ((DEBUG_INFO, "TbtSmm: WSMT validation is invalid on MEM\n"));
        return FALSE;
      }
    }
    //
    // Check whether Prefetch Registers were modified by malicious attack or not
    // Pi->PMemBase64 > Pi->PMemLimit64: Range is incorrect, OS will not catch this MMIO
    // Depth + 1 < MAX_TBT_DEPTH: The last node within the Daisy Chain,
    //                            CIO can't accept anything
    //
    if (Pi->PMemBase64 <= Pi->PMemLimit64 && Depth + 1 < MAX_TBT_DEPTH) {
      CheckedAddress    = (Pi->PMemBase64 & 0xFFFFFFFFFFF0) << 16;
      CheckedSize       = ((Pi->PMemLimit64 & 0xFFFFFFFFFFF0) - (Pi->PMemBase64 & 0xFFFFFFFFFFF0) + 0x10) << 16;
      if (EFI_ERROR (CheckMmio (0xC0 | Depth, CheckedAddress, CheckedSize))) {
        DEBUG ((DEBUG_INFO, "TbtSmm: WSMT validation is invalid on P-MEM\n"));
        return FALSE;
      }
    }
//AMI_OVERRIDE_END - EIP329548 - [Win10 RS2] Need to verify PCIe MMIO buffer at TBT SMI handler
//AMI_OVERRIDE_END - EIP336316 - Improve WSMT validation on Thunderbolt

    //
    // Jump to next EP
    //
    *Bus = MmioRead8 (DeviceBaseAddress + PCI_SBUS);
    //
    // Should we continue?
    //
    DeviceBaseAddress = MmPciBase (*Bus, 0x00, 0x00);
    Continue          = 0xFFFF != MmioRead16 (DeviceBaseAddress + PCI_DID);
    return Continue;
  }
  //
  // Set is legacy dvice
  //
  isLegacyDevice = IsLegacyDevice (*Bus);

  SetCioPortResources (
    *Bus,
    0, // Assign all available resources to US port of EP
    *Bus + 1,
    Pi->BusNumLimit,
    0,
    Pi
    );

  SBus = *Bus + 1;// Jump to DS port

  if (CioSlotPresent) {
    MaxPHYSlots = CioSlot;
  }

//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
  UsedBusNumbers = ConfigureSlot(SBus, MaxPHYSlots, Depth, ArPcie, Pi, &DetectGfxInserted);
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
  if (DetectGfxInserted && mGlobalNvsAreaPtr->TbtXConnectSupport) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
    FillATbtDeviceTree (32, *Bus, 0, 0);
    DEBUG ((DEBUG_INFO, "[DGPU]GFX found at endpoint US [%03d:%02d:%d] -> (BufferTree: 32)\n", *Bus, 0, 0));
  }
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"

  if (!CioSlotPresent) {
    return FALSE;
    //
    // Stop resource assignment on this chain
    //
  }
  //
  // Set rest of slots us unused
  //
  SetSlotsAsUnused (SBus, MaxSlotNum, CioSlot, Pi);

  SetCioPortResources (
    SBus,
    CioSlot,
    SBus + UsedBusNumbers + 1,
    Pi->BusNumLimit,
    &portInfo,
    Pi
    );
  *Bus = SBus + UsedBusNumbers + 1;// Go to next EP
  if(ArPcie) {
//AMI_OVERRIDE_START - EIP317127 - Implement "Thunderbolt Dock with NIC Hard Hang BIOS Workaround"
    SetSoftL1 (SBus, CioSlot, 0x00);
//AMI_OVERRIDE_END - EIP317127 - Implement "Thunderbolt Dock with NIC Hard Hang BIOS Workaround"
  }
//AMI_OVERRIDE_START - EIP336316 - Improve WSMT validation on Thunderbolt
  if (*Bus > Pi->BusNumLimit - 2 || Depth + 1 == MAX_TBT_DEPTH) {
    //
    // In case of bus numbers are exhausted stop enumeration
    // Set CIO with LINK_DIS to avoid device control of Windows driver
    //
    PcieLinkEnable (SBus, CioSlot, 0, FALSE);
    return FALSE;
  }
//AMI_OVERRIDE_END - EIP336316 - Improve WSMT validation on Thunderbolt
  //
  // Check whether we should continue on this chain
  //
  DeviceBaseAddress = MmPciBase (*Bus, 0x00, 0x00);
  Continue          = 0xFFFF != MmioRead16 (DeviceBaseAddress + PCI_DID);
  return Continue;
} // ConfigureEP

/**
  Get port resources

  @param  Bus                   Current bus number
  @param  Dev                   Current device number
  @param  Pi                    Pointer to PortInfo

  @return                       No return value
**/
STATIC
VOID
GetPortResources (
  IN       UINT8                Bus,
  IN       UINT8                Dev,
  IN  OUT  PortInfo             *Pi
  )
{
  DeviceBaseAddress = MmPciBase (Bus, Dev, 0x00);
  Pi->BusNumLimit   = MmioRead8 (DeviceBaseAddress + PCI_SUBUS);
  Pi->IoBase        = MmioRead8 (DeviceBaseAddress + PCI_IOBASE) & 0xF0;
  Pi->IoLimit       = MmioRead8 (DeviceBaseAddress + PCI_IOLIMIT) & 0xF0;
  Pi->MemBase       = MmioRead16 (DeviceBaseAddress + PCI_MEMBASE) & 0xFFF0;
  Pi->MemLimit      = MmioRead16 (DeviceBaseAddress + PCI_MEMLIMIT) & 0xFFF0;
  Pi->PMemBase64    = MmioRead16 (DeviceBaseAddress + PCI_PRE_MEMBASE) & 0xFFF0;
  Pi->PMemLimit64   = MmioRead16 (DeviceBaseAddress + PCI_PRE_MEMLIMIT) & 0xFFF0;
  Pi->PMemBase64   |= (UINT64)(MmioRead32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U)) << 16;
  Pi->PMemLimit64  |= (UINT64)(MmioRead32 (DeviceBaseAddress + PCI_PRE_MEMLIMIT_U)) << 16;
  Pi->IoLimit |= 0xF;
  Pi->MemLimit |= 0xF;
  Pi->PMemLimit64 |= 0xF;
} // GetPortResources

/**
  Configure Port

  @param  Bus                   Current bus number
  @param  Dev                   Current device number
  @param  Pi                    Pointer to PortInfo

  @return                       No return value
**/
STATIC
VOID
ConfigurePort (
  IN       UINT8                Bus,
  IN       UINT8                Dev,
  IN  OUT  PortInfo             *Pi
  )
{
//AMI_OVERRIDE_START - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
//  UINT8      SubBusNumber;
//  UINT8      StartBusNumber;
//  BOOLEAN   DgpuDetected;
//  BOOLEAN   FirstDepthDevice;
//  UINT16     DgpuVid = 0xFFFF;
//  UINT16     DgpuDid = 0xFFFF;
//  UINT8      GfxBus = 0;
  INT8                          i;
  UINT8                         USBusNum;
//  UINT16 DevId = 0x0;
//  UINT8 HR_CIO_DownStreamPort = 0x0;
  DeviceBaseAddress = MmPciBase (Bus, Dev, 0x00);
  USBusNum          = MmioRead8 (DeviceBaseAddress + PCI_SBUS);
  DeviceBaseAddress = MmPciBase (USBusNum, 0x00, 0x00);

  PrintCpStr("ConfigurePort");
  PrintCpStr("Bus");
  PrintCp(Bus);
  PrintCpStr("Dev");
  PrintCp(Dev);
  PrintCpStr(".");
  PrintCpStr("OriginalUSBusNum");
  PrintCp(USBusNum);
  mGlobalNvsAreaPtr->DGfxDockEventDetected = 0;
  mGlobalNvsAreaPtr->DGfxSurpriseUndockEventDetected = 0;

  if (0xFFFF == MmioRead16 (DeviceBaseAddress + PCI_DID)) {
    //
    // No TBT device, we can't jump out of this condition
    // mThunderboltDGfx.HrDownstreamDevice == Dev: Last time we connected DGPU at this port
    //
    if (LastTimeDgpuFound == TRUE && mThunderboltDGfx.HrDownstreamDevice == Dev) {
      DEBUG ((DEBUG_INFO, "[DGPU] GFX is absent\n"));
      DEBUG ((DEBUG_INFO, "[DGPU] Remove GFX event\n"));
      mGlobalNvsAreaPtr->DGfxSurpriseUndockEventDetected = 1;
      LastTimeDgpuFound = FALSE;
      mThunderboltDGfx.IsLocationDgpu = FALSE;
    }
    //
    // Nothing to do if TBT device is not connected
    //
    return ;
  }
//
//  DevId = MmioRead16 (DeviceBaseAddress + PCI_DID);
//  if (DevId == FR_HR_2C) {
//    HR_CIO_DownStreamPort = 3; //FalconRidge
//  } else if (DevId == AR_HR_2C || DevId == AR_HR_4C) {
//    HR_CIO_DownStreamPort = 1; //AlpineRidge
//  }
//
//
//  if (mGlobalNvsAreaPtr->TbtDetachableGfxSupport && (Dev == HR_CIO_DownStreamPort /* CIO-DOWNSTREAM-PORT */)){
//    //
//    // Fill Host Router Downstream port information structure
//    //
//    PrintCpStr("FillATbtDeviceTree.Begin|");
//    FillATbtDeviceTree(22, Bus, Dev, 0x00);
//
//    //
//    // Fill Endpoint Upstream port information structure
//    //
//    FillATbtDeviceTree(32, USBusNum, 0x00, 0x00);
//	PrintCpStr("FillATbtDeviceTree.End|");
//    }
//
//    DgpuDetected = FALSE;
//    FirstDepthDevice = FALSE;
//
//
//   if ((mGlobalNvsAreaPtr->TbtDetachableGfxSupport) && (Dev == HR_CIO_DownStreamPort/* CIO-DOWNSTREAM-PORT */)) {
//      SubBusNumber    = 0;
//      StartBusNumber  = USBusNum;
//
//      PrintCpStr("StartBusNumber");
//      PrintCp(StartBusNumber);
//      PrintCpStr("SubBusNumber");
//      PrintCp(SubBusNumber);
//      PrintCpStr(".");
//
//      PrintCpStr("PciAssignBusNumber.Begin");
//      PciAssignBusNumber(StartBusNumber, &SubBusNumber);
//      PrintCpStr("PciAssignBusNumber.End");
//
//      PrintCpStr("StartBusNumber");
//      PrintCp(StartBusNumber);
//      PrintCpStr("SubBusNumber");
//      PrintCp(SubBusNumber);
//      PrintCpStr(".");
//
//      PrintCpStr("CheckForDgpuPresence.Begin");
//      DgpuDetected = CheckForDgpuPresence(StartBusNumber, SubBusNumber, DgpuVid, DgpuDid, &GfxBus);
//      if (DgpuDetected) {
//        PrintCpStr("CheckForFirstDepth.Begin");
//        FirstDepthDevice = CheckForFirstDepth(StartBusNumber, GfxBus);
//        PrintCpStr("CheckForFirstDepth.End");
//        if(!FirstDepthDevice) {
//          PrintCpStr("Dgpu is not in 1st depth");
//          DgpuDetected = FALSE;
//        }
//      }
//      PrintCpStr("CheckForDgpuPresence.End");
//
//      PrintCpStr("PciClearBusNumber.Begin");
//      PciClearBusNumber(StartBusNumber, SubBusNumber);
//      PrintCpStr("PciClearBusNumber.End");
//
//      if (DgpuDetected == TRUE) {
//        PrintCpStr("DgpuDetected");
//        mGlobalNvsAreaPtr->DGfxPciBaseAddressMemValid = 1;
//      } else {
//        PrintCpStr("DgpuNotDetected");
//        mGlobalNvsAreaPtr->DGfxPciBaseAddressMemValid = 0;
//      }
//
//      mGlobalNvsAreaPtr->DGfxDockEventDetected = 0;
//      mGlobalNvsAreaPtr->DGfxSurpriseUndockEventDetected = 0;
//
//      if (DgpuDetected == TRUE) {
//        if (LastTimeDgpuFound == FALSE) {
//          PrintCpStr("DGfxDockEventDetected");
//          mGlobalNvsAreaPtr->DGfxDockEventDetected = 1;
//          LastTimeDgpuFound = TRUE;
//        }
//      } else {
//        if (LastTimeDgpuFound == TRUE) {
//          PrintCpStr("DGfxSurpriseUndockEventDetected");
//          mGlobalNvsAreaPtr->DGfxSurpriseUndockEventDetected = 1;
//          LastTimeDgpuFound = FALSE;
//        }
//      }
//    }

//AMI_OVERRIDE_START - EIP336316 - Improve WSMT validation on Thunderbolt
  //
  // DeviceID = 1: AR or TR Port A
  // DeviceID = 3: FR Port A
  //
  if (Dev != 1 && Dev != 3) {
    mIsPortA = FALSE;
  } else {
    mIsPortA = TRUE;
  }
//AMI_OVERRIDE_END - EIP336316 - Improve WSMT validation on Thunderbolt

  if (mThunderboltDGfx.IsLocationDgpu) {
    if (MmioRead8 (MmPciBase (
                    mThunderboltDGfx.DgpuBus,
                    mThunderboltDGfx.DgpuDev,
                    mThunderboltDGfx.DgpuFun
                    ) + PCI_BCC) != 3) {
      //
      // Previously inserted GFX is removed, we release the chance to scan other new inserted GFX
      //
      mThunderboltDGfx.IsLocationDgpu = FALSE;
    }
  }


  GetPortResources(Bus, Dev, Pi);// Take reserved resources from DS port

//    if (mGlobalNvsAreaPtr->TbtDetachableGfxSupport && (!DgpuDetected) && (Dev == HR_CIO_DownStreamPort /* CIO-DOWNSTREAM-PORT */)) {
//      // Adjust resources of pi
//      // 1) Move Prefetchable memory "ReservePMemoryForDGfx" size (eg:256 MB) forward
//      // 2) Move memory "ReserveMemoryForDGfx" size (eg:32 MB) forward
//      PrintCpStr("Adjust PMem, Mem resources for DGfx");
//      Pi->PMemBase64 += (ReservePMemoryForDGfx << 4);
//      Pi->MemBase += (ReserveMemoryForDGfx << 4);
//    }

    // Assign resources to EPs
    for (i = 0; i < MAX_TBT_DEPTH; ++i)
    {
//      if (i == 0) // First depth
//      {
//// AMI_OVERRIDE_START - EIP278493 Daisy Chain failed when DGFX is enabled
//        //Assumption : GPU EP will be connected at first depth only
//        if (mGlobalNvsAreaPtr->TbtDetachableGfxSupport && ReserveBusForDGfx > 0 &&
//            (!DgpuDetected) && (Dev == HR_CIO_DownStreamPort /* CIO-DOWNSTREAM-PORT */)) {
//          // Move CIO DS port buses to "ReserveBusForDGfx" forward
//          PrintCpStr("Adjust Bus resources for DGfx");
//          PrintCpStr("OldUSBusNum");
//          PrintCp(USBusNum);
//          PrintCpStr("ReserveBusForDGfx");
//          PrintCp(ReserveBusForDGfx);
//          USBusNum += ReserveBusForDGfx; // Start configure EP from new value
//          PrintCpStr("NewUSBusNum");
//          PrintCp(USBusNum);
//          MmioWrite8  (MmPciBase (Bus, Dev, 0x00) + PCI_SBUS, USBusNum);
//
//          //
//          // Overwrite Endpoint Upstream port information structure - Bus information
//          //
//          FillTbtDeviceTreeBuffer(32, USBusNum);
//          //
//          // If Upstream port Bus is changed, the old resource settings will be reset from
//          // PciAssignBusNumber (). We should make "PCI Bridge" do a further initialization.
//          // Otherwise, the hot plug will fail.
//          //
//          IsFirstEnterFlag = 1;
//        }
//// AMI_OVERRIDE_END - EIP278493 Daisy Chain failed when DGFX is enabled
//      }
//
//    PrintCpStr("FinalUSBusNum");
//    PrintCp(USBusNum);


    Pi->ConfedEP++;
    if (!ConfigureEP (i, &USBusNum, Pi)) {
      break;
    }
  }

  if (mThunderboltDGfx.IsLocationDgpu) {
    //
    // HR Port A or B
    //
    FillATbtDeviceTree (22, Bus, mThunderboltDGfx.HrDownstreamDevice, 0);
    DEBUG ((DEBUG_INFO, "[DGPU]Thunderbolt DS Port [%03d:%02d:%d] -> (BufferTree: 22)\n",
            Bus, mThunderboltDGfx.HrDownstreamDevice, 0));
    //
    // Rescan Windows Device Manager HW change: Inserted
    //
    DEBUG ((DEBUG_INFO, "[DGPU] GFX is presented\n"));
    if (LastTimeDgpuFound == FALSE) {
      DEBUG ((DEBUG_INFO, "[DGPU] Insert GFX event\n"));
      mGlobalNvsAreaPtr->DGfxDockEventDetected = 1;
      LastTimeDgpuFound = TRUE;
    }
  } else {
    //
    // Rescan Windows Device Manager HW change: Removed
    //
    DEBUG ((DEBUG_INFO, "[DGPU] GFX is absent\n"));
    if (LastTimeDgpuFound == TRUE) {
      DEBUG ((DEBUG_INFO, "[DGPU] Remove GFX event\n"));
      mGlobalNvsAreaPtr->DGfxSurpriseUndockEventDetected = 1;
      LastTimeDgpuFound = FALSE;
    }
  }
//AMI_OVERRIDE_END - EIP311058 - Fix Intel TBT FRC side effects which are come from "dGPU Support is Enabled"
} // ConfigurePort

/**
  Disable MSI in current EP

  @param  UBus                  Upstream bus number

  @retval TRUE                  Continue to disable MSI to next chain
  @retval FALSE                 Stop to disable MSI to next chain
**/
STATIC
BOOLEAN
DisableMSIEP(
  IN  OUT  UINT8                *UBus
  )
{
  UINT8                         CioSlot = 4;
  UINT8                         MaxSlotNum = 7;
  BOOLEAN                       ArPcie;
  BOOLEAN                       CioSlotPresent;
  BOOLEAN                       Continue;
  UINT8                         MSIHeader;
  UINT8                         Bus = *UBus + 1;

  ArPcie = FALSE;
  // Based on Device ID assign Cio slot and max number of PHY slots to scan
  CioSlotPresent = GetCioSlotByDevId(*UBus, &CioSlot, &MaxSlotNum, &ArPcie);
  if(!CioSlotPresent)
    return FALSE;
  PrintCpStr("BD=");
  PrintCp(Bus);
  PrintCp(CioSlot);

  // Find MSI cap header and clean MSI Enable bit
  MSIHeader = PcieFindCapId(Bus, CioSlot, 0, PCI_CAP_ID_MSI);
  if(0 != MSIHeader)
  {
    MSIHeader += 0x2; // Go to Message Control register
    MmioAnd16 (MmPciBase (Bus, CioSlot, 0) + MSIHeader, (UINT16) (0xFFFF - 1));
  }

  *UBus = MmioRead8  (MmPciBase (Bus, CioSlot, 0) + PCI_SBUS);

  // Check whether we should continue on this chain
  Continue = 0xFFFF != MmioRead16 (MmPciBase (*UBus, 0x00, 0x00) + PCI_DID);
  return Continue;
}//DisableMSIEP

/**
  Disable MSI for whole chain

  @param  Bus                   PCI bus number
  @param  Device                PCI device number
  @param  CleanedEP             Pointer to store cleaned EP number

  @return                       No return value
**/
STATIC
VOID
DisableMSIPort(
  IN       UINT8                Bus,
  IN       UINT8                Dev,
  IN  OUT  UINT8                *CleanedEP
  )
{
  INT8                          i;
  UINT8                         USBusNum = MmioRead8  (MmPciBase (Bus, Dev, 0x00) + PCI_SBUS);

  if (0xFFFF == MmioRead16 (MmPciBase (USBusNum, 0x00, 0x00) + PCI_DID))// Nothing to do if TBT device is not connected
    return;

  // Disable MSI for whole chain
  for (i = 0; i < MAX_TBT_DEPTH; ++i)
  {
    CleanedEP++;
    if(!DisableMSIEP(&USBusNum))
      return;
  }
}//DisableMSIPort

/**
  Init. host router MSI

  @param  Hr_Config             Pointer to HR_CONFIG

  @retval TRUE                  Init. finished
  @retval FLASE                 Device not found
**/
STATIC
BOOLEAN
InitializeHostRouterMSI(
  OUT  HR_CONFIG                *Hr_Config
  )
{
  UINT8                         i,j;
  UINT8                         Bus;
  UINT8                         Dev;
  UINT8                         Fun;

  for(i = 0; i < count(HR_Slots); ++i)
  {
    Hr_Config->HRBus = MmioRead8  (MmPciBase (HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun) + PCI_SBUS);
    Hr_Config->DeviceId = MmioRead16 (MmPciBase (Hr_Config->HRBus, 0x00, 0x00) + PCI_DID);
    if (IsTbtHostRouter(Hr_Config->DeviceId)) {
      break;
    }
  }

  if(i >= count(HR_Slots))
  {
    PrintCpStr("!Fnd");
    return FALSE;
  }

  switch(Hr_Config->DeviceId)
  {
    case 0x157E: case 0x157A:// HR with 1 DS only
      Bus = Hr_Config->HRBus + 1;
      Dev = 1;
      Fun = 0;

      Hr_Config->MinDSNumber = Dev;
      Hr_Config->MaxDSNumber = Dev;

      // Clean MSI Enable bit
      MmioAnd16 (MmPciBase (Bus, Dev, Fun) + TBT_HR_MSG_CTRL_REG, (UINT16) (0xFFFF - 1));
      break;
    default:
      // DS(HRBus+2:3-6:0)
      Hr_Config->MinDSNumber = 3;
      Hr_Config->MaxDSNumber = 6;

      for(j = 2, i = Hr_Config->MinDSNumber;j < count(HrConfigs) && i <= Hr_Config->MaxDSNumber;++j, ++i) {
        HrConfigs[j].DevId.Bus = Hr_Config->HRBus + 1;
        HrConfigs[j].DevId.Dev = i;
        HrConfigs[j].DevId.Fun = 0;
      }

      // Clean MSI Enable bit for all downstream ports
      for(j = HR_DS_PORT3; j <= HR_DS_PORT6; ++j)
      {
        Bus = HrConfigs[j].DevId.Bus;
        Dev = HrConfigs[j].DevId.Dev;
        Fun = HrConfigs[j].DevId.Fun;
        MmioAnd16 (MmPciBase (Bus, Dev, Fun) + TBT_HR_MSG_CTRL_REG, (UINT16) (0xFFFF - 1));
      }
  }

  return TRUE;
}//InitializeHostRouterMSI

/**
  Disable MSI (Message Signaled Interrupts)

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
DisableMSI (
  VOID
  )
{
  UINT8                         i;
  HR_CONFIG                     HrConfig = {0};
  UINT8                         CleanedEP = 0;

  PrintCpStr("MSIEnr");
  PROGRESS_CODE (SMM_CLEAN_MSI_CALL);
  //MmioWrite32 (MmPciBase (0x01, 0x00, 0x00) + PCI_VID, 0x21212121);

  if (!InitializeHostRouterMSI(&HrConfig)) {
    PROGRESS_CODE (0xCB); //CaBle is unplugged
    //MmioWrite32 (MmPciBase (0x01, 0x00, 0x00) + PCI_VID, 0x32323232);
    PrintCpStr("MSIExtCB");
    return;
  }

  // Clean MSI Enable bit for all downstream ports
  for(i = HrConfig.MinDSNumber; i <= HrConfig.MaxDSNumber; ++i) {
    DisableMSIPort(HrConfig.HRBus + 1, i, &CleanedEP);
  }

  PrintCpStr("MSIExt");
  PrintCp(CleanedEP);
  PROGRESS_CODE (SMM_CLEAN_MSI_CALL + 2 + CleanedEP); //PostCode = 0xDC + CleanedEP
} // DisableMSI

//AMI_OVERRIDE_START - EIP296758 Implement Intel document 563778, version 2
/**
  WA563778 version 2 programming

  Intel Technical Advisory #563778:
  Using Non-LTR Capable PCIe Devices with Alpine Ridge.

  @param  BridgeBus             The PCI bus number which is used by Alpine Ridge XHCI controller.
  @param  BridgeDev             The PCI device number which is used by Alpine Ridge XHCI controller.
  @param  PrewakeEnabled        Alpine Ridge XHCI Pre-wake Enabled or Disabled.
  @param  ActiveLtrMicrosecond  Alpine Ridge XHCI Active LTR allow 1~1023 ms.
  @param  HighLtrMicrosecond    Alpine Ridge XHCI High LTR allow 3~124 ms.
  @param  MediumLtrMicrosecond  Alpine Ridge XHCI Medium LTR allow 2~123 ms.
  @param  LowLtrMicrosecond     Alpine Ridge XHCI Low LTR allow 1~122 ms.

  @retval EFI_SUCCESS           Programming finished
  @retval EFI_INVALID_PARAMETER Parameter is invalid
**/
EFI_STATUS
WA563778Version2Programming (
  IN UINT8                      BridgeBus,
  IN UINT8                      BridgeDev,
  IN BOOLEAN                    PrewakeEnabled,
  IN UINT32                     ActiveLtrMicrosecond,
  IN UINT32                     HighLtrMicrosecond,
  IN UINT32                     MediumLtrMicrosecond,
  IN UINT32                     LowLtrMicrosecond
  )
{
  UINTN                         XhciPcieAddress;
  UINT8                         XhciBus;
  UINTN                         XhciBAR;

  if (BridgeBus == 0 || BridgeBus > 0xff || BridgeDev > 0x1f) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Alpine Ridge DP 1.06 Page 565 mentions "MIWL must be larger than LIWL"
  //
  if (MediumLtrMicrosecond <= LowLtrMicrosecond) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Alpine Ridge DP 1.06 Page 564 mentions "HIWL must be larger than MIWL & LIWL"
  //
  if (HighLtrMicrosecond <= MediumLtrMicrosecond) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find XHCI MMIO:
  // PCI-to-PCI Bridge
  // >> Intel(R) USB 3.1 eXtensible Host Controller
  //
  XhciBus = MmioRead8 (MmPciBase (BridgeBus, BridgeDev, 0) + PCI_SBUS);
  XhciPcieAddress = MmPciBase (XhciBus, 0, 0);

  //
  // Get XHCI BAR
  //
  XhciBAR = MmioRead32 (XhciPcieAddress + PCI_BAR0);
  if (PrewakeEnabled) {
    MmioOr32 (XhciBAR + 0x8174, BIT28);
  }

  //
  // 8174h, [21:12] describes the Ltr in nanosecond,
  // we should mutiple 1024 to transfer the type into microsecond
  // 8174h, [23:22]>>> BIT22: x1024; BIT23: x1024x32; BIT22|BIT23: x1024x32x32
  //
  MmioOr32 (XhciBAR + 0x8174, BIT22);
  //
  // Clear BIT[21:12], and fill Active Ltr
  //
  MmioAndThenOr32 (XhciBAR + 0x8174, 0xFFC00FFF, ActiveLtrMicrosecond << 12);

  //
  // 817Ch, 8180h, and 8184h's [12:7] & [22:16] are the multiple of 125us
  // 817Ch, 8180h, and 8184h's [6:0] & [23:28] are the Ltr in microsecond
  //
  // should program LIWL -> MIWL -> HIWL sequentially
  //
  MmioAndThenOr32 (XhciBAR + 0x8184, 0xE000E000, LowLtrMicrosecond    | (LowLtrMicrosecond << 16));
  MmioAndThenOr32 (XhciBAR + 0x8180, 0xE000E000, MediumLtrMicrosecond | (MediumLtrMicrosecond << 16));
  MmioAndThenOr32 (XhciBAR + 0x817C, 0xE000E000, HighLtrMicrosecond   | (HighLtrMicrosecond << 16));

  return EFI_SUCCESS;
}
//AMI_OVERRIDE_END - EIP296758 Implement Intel document 563778, version 2

/**
  Thunderbolt callback function

  @param  RpIndex               Root port index

  @return                       No return value
**/
VOID
ThunderboltCallback (
  IN  UINT32 RpIndex
  )
{
  PortInfo                      portInfo  = { 0 };
  HR_CONFIG                     HrConfig  = { 0 };
  UINT8                         Index;
//AMI_OVERRIDE_START - EIP329111 - Thunderbolt FRC uses 2 CMOS register without enumerating in SSP file
//  UINT8                         Data8;
//AMI_OVERRIDE_END - EIP329111 - Thunderbolt FRC uses 2 CMOS register without enumerating in SSP file
  EFI_STATUS                    Status;
  UINTN                         DataSize;
  EFI_SMM_VARIABLE_PROTOCOL     *SmmVariable;
// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM
  UINT16                        TbtHostDeviceId;
//AMI_OVERRIDE_START - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
  UINT32                        AmiTbtHrStatusAttribute;
//AMI_OVERRIDE_END - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
  UINTN                         HrStatusSize = sizeof(AMI_TBT_HR_STATUS_DATA);
  AMI_TBT_HR_STATUS_DATA        HrStatusData;
//AMI_OVERRIDE_START - EIP296758 Implement Intel document 563778, version 2
  UINTN                         NumberOfDeviceOnPortA = 0;
  BOOLEAN                       HrXhciPresent = FALSE;
//AMI_OVERRIDE_END - EIP296758 Implement Intel document 563778, version 2
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM

  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol;

  DataSize = sizeof (SETUP_DATA);

  PrintCpStr("ThunderboltCallback.Entry");
  Status = gSmst->SmmLocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;

  DataSize = sizeof (SETUP_DATA);

  PrintCpStr("ThunderboltCallback.Entry");
  Status = gSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&SmmVariable);

// AMI_OVERRIDE_START - EIP313994 - Unable to change MEM & PMEM size for each TBT slot
//  Status = SmmVariable->SmmGetVariable (
//                          PLATFORM_SETUP_VARIABLE_NAME,
//                          &gSetupVariableGuid,
//                          NULL,
//                          &DataSize,
//                          &SysConfig
//                          );
//  if (EFI_ERROR (Status)) {
//    SetReservedResources ();
//  }
// AMI_OVERRIDE_END - EIP313994 - Unable to change MEM & PMEM size for each TBT slot

  PROGRESS_CODE (SMM_THUNDERBOLT_CALL);
/*
  if (!(SysConfig.CallSmiBeforeBoot & BIT4)) {
    PROGRESS_CODE (SMM_THUNDERBOLT_CALL + 1);
    return ;
  }
*/

//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
  if (GlobalNvsAreaProtocol->Area->TbtXConnectSupport) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
//AMI_OVERRIDE_START - EIP329111 - Thunderbolt FRC uses 2 CMOS register without enumerating in SSP file
    //Data8 = CmosRead8 (CMOS_TBT_NOTIFY_OS);
    //mGlobalNvsAreaPtr->DGfxOsHotplugNotification = Data8;

    //PrintCpStr("DGfxOsHotplugNotification");
    //PrintCp(Data8);
    //PrintCpStr(".");

    //Data8 = 1;
    //CmosWrite8 (CMOS_TBT_NOTIFY_GPU, Data8);
    mGlobalNvsAreaPtr->DGfxGpuHotplugNotification = 1;

    //PrintCpStr ("DGfxGpuHotplugNotification");
    //PrintCp (Data8);
    //PrintCpStr (".");
//AMI_OVERRIDE_END - EIP329111 - Thunderbolt FRC uses 2 CMOS register without enumerating in SSP file
  }

  PrintCpStr ("PortInfoInit");
  PortInfoInit (&portInfo);

  PrintCpStr ("InitializeHostRouter");
  PrintCp32 (RpIndex);
// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM
  Status = SmmVariable->SmmGetVariable( AMI_TBT_HR_STATUS_VARIABLE, \
                             &gAmiTbtHrStatusGuid, \
                             &AmiTbtHrStatusAttribute, \
                             &HrStatusSize, \
                             &HrStatusData );
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM

  if (!InitializeHostRouter (&HrConfig,(UINT32) RpIndex)) {
// AMI_OVERRIDE_START - CaBle Progress code.
    PROGRESS_CODE (0xCB); //CaBle is unplugged
// AMI_OVERRIDE_END - - CaBle Progress code.

// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM
    HrStatusData.TbtHrStatus = 0;
    Status = SmmVariable->SmmSetVariable( AMI_TBT_HR_STATUS_VARIABLE, \
                               &gAmiTbtHrStatusGuid, \
                               AmiTbtHrStatusAttribute, \
                               sizeof(AMI_TBT_HR_STATUS_DATA), \
                               &HrStatusData );
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM
    return;
  }
  //
  // Configure DS ports
  //
  for (Index = HrConfig.MinDSNumber; Index <= HrConfig.MaxDSNumber; ++Index) {
    PrintCpStr ("ConfigurePort");
    ConfigurePort (HrConfig.HRBus + 1, Index, &portInfo);
//AMI_OVERRIDE_START - EIP296758 Implement Intel document 563778, version 2
    //
    // Root Port Bus #                 (PCIe slot)
    // >> PCI-to-PCI Bridge            (Thunderbolt Upstream Port)
    //   >> i = 0: PCI-to-PCI Bridge   (Thunderbolt Downstream Port: Physical Port)
    //   >> i = 1: PCI-to-PCI Bridge   (Thunderbolt Downstream Port: Port A)
    //   >> i = 2: PCI-to-PCI Bridge   (Thunderbolt Downstream Port: XHCI Controller)
    //   >> i = 4: PCI-to-PCI Bridge   (Thunderbolt Downstream Port: Port B)  2 Port only
    //
    if (Index == 1) {
      NumberOfDeviceOnPortA = portInfo.ConfedEP;
    }

    //
    // If "device count" is increased at XHCI,
    // we say USB is attached on the Thunderbolt host router
    //
    if (Index == 2) {
      HrXhciPresent = (portInfo.ConfedEP - NumberOfDeviceOnPortA != 0) ? TRUE : FALSE;
    }
//AMI_OVERRIDE_END - EIP296758 Implement Intel document 563778, version 2
  }
    PROGRESS_CODE (SMM_THUNDERBOLT_CALL + 2 + portInfo.ConfedEP); //PostCode = 0xAC + # of connected EP

// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM
  TbtHostDeviceId  = MmioRead16 (MmPciBase(HrConfig.HRBus, 0x00, 0x00) + PCI_DID);

  if ((TbtHostDeviceId == RR_HR_2C) || (TbtHostDeviceId == RR_HR_4C))
      HrStatusData.TbtHrSeries = Redwood_Ridge;
  if ((TbtHostDeviceId == FR_HR_2C) || (TbtHostDeviceId == FR_HR_4C))
      HrStatusData.TbtHrSeries = Falcon_Ridge;
  if (TbtHostDeviceId == WR_HR_2C)
      HrStatusData.TbtHrSeries = BDW_TBT_LP;
  if ((TbtHostDeviceId == AR_HR_2C) || (TbtHostDeviceId == AR_HR_4C) || (TbtHostDeviceId == AR_HR_LP) || (TbtHostDeviceId == AR_HR_C0_2C) || (TbtHostDeviceId == AR_HR_C0_4C))
      HrStatusData.TbtHrSeries = Alpine_Ridge;

  HrStatusData.TbtHrStatus = 1;
  Status = SmmVariable->SmmSetVariable( AMI_TBT_HR_STATUS_VARIABLE, \
                             &gAmiTbtHrStatusGuid, \
                             AmiTbtHrStatusAttribute, \
                             sizeof(AMI_TBT_HR_STATUS_DATA), \
                             &HrStatusData );
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM

    PrintCpStr("EndOfThunderboltCallback");
//AMI_OVERRIDE_START - EIP296758 Implement Intel document 563778, version 2
  //
  // Intel Technical Advisory WW35 #563778, version 2:
  // Using Non-LTR Capable PCIe Devices with Alpine Ridge
  //
  if (HrXhciPresent && SysConfig.TbtWaSwitch == 2) {
    Status = WA563778Version2Programming (
               HrConfig.HRBus + 1,
               2,
               (BOOLEAN) SysConfig.TbtWaPrewakeEnable,
               SysConfig.TbtWaXhciActiveLtr, //20
               SysConfig.TbtWaXhciHighLtr,   //80
               SysConfig.TbtWaXhciMediumLtr, //40
               SysConfig.TbtWaXhciLowLtr     //30
               );
    if (EFI_ERROR (Status)) {
      PrintCpStr("Thunderbolt-WA_II.Failed");
    } else {
      //
      // It should be enabled to apply the settings
      //
      SysConfig.TbtLtr = 1;
    }
  }
//AMI_OVERRIDE_END - EIP296758 Implement Intel document 563778, version 2
  EndOfThunderboltCallback ((UINT32) mGlobalNvsAreaPtr->TbtSelector);

  //
  // InitializeHostRouter for another controller
  //

  PrintCpStr("ThunderboltCallback.Exit");


} // ThunderboltCallback

/**
  Thunderbolt callback function

  @param  RpNumber              Root port number

  @return                       No return value
**/
VOID
DisablePCIDevicesAndBridges (
  IN  UINT32                    RpNumber
  )
{
  UINT8                         Bus;
  UINT8                         Dev;
  UINT8                         Fun;
  UINT8                         RegVal;
  UINT8                         MinBus;
  UINT8                         MaxBus;
  UINT16                        DeviceId;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  EFI_STATUS                    Status;

  MinBus = 1;
  MaxBus = 1;

  //
  // for(Dev = 0; Dev < 8; ++Dev)
  // {
  // PciOr8(PCI_LIB_ADDRESS(2, Dev, 0, PCI_BRIDGE_CONTROL_REGISTER_OFFSET), 0x40);
  // gBS->Stall(2000);      // 2msec
  // PciAnd8(PCI_LIB_ADDRESS(2, Dev, 0, PCI_BRIDGE_CONTROL_REGISTER_OFFSET), 0xBF);
  // }
  // gBS->Stall(200 * 1000);        // 200 msec
  //
  Status = GetTbtRpDevFun (RpNumber - 1, &RpDev, &RpFunc);
  DeviceBaseAddress = MmPciBase (0x00,(UINT32) RpDev, (UINT32) RpFunc);
  MinBus            = MmioRead8 (DeviceBaseAddress + PCI_SBUS);
  MaxBus            = MmioRead8 (DeviceBaseAddress + PCI_SUBUS);
  DeviceBaseAddress = MmPciBase (MinBus, 0x00, 0x00);
  DeviceId          = MmioRead16 (DeviceBaseAddress + PCI_DID);
  PrintCpStr ("DeviceId");
  PrintCp16 ((UINT16)DeviceId);

  if (!IsTbtHostRouter (DeviceId)) {
    return;
  }

  MinBus++;

  //
  //  Disable PCI device First, and then Disable PCI Bridge
  //
  for (Bus = MaxBus; Bus > MinBus; --Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
          if (Fun == 0) {
            break;

          }

          continue;
        }

        RegVal = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE);
        if (PCI_DEVICE == (RegVal & 1)) {
          //
          // ********     Disable PCI Device   ********
          // BIT0  I/O Space Enabled    BIT1  Memory Space Enabled
          // BIT2  Bus Master Enabled   BIT4  Memory Write and Invalidation Enable
          //
          MmioAnd8 (DeviceBaseAddress + PCI_CMD_REG, (UINT8)~(BIT0 | BIT1 | BIT2 | BIT4));
          MmioWrite32 (DeviceBaseAddress + PCI_BAR0, 0);
          MmioWrite32 (DeviceBaseAddress + PCI_BAR1, 0);
          MmioWrite32 (DeviceBaseAddress + PCI_BAR2, 0);
          MmioWrite32 (DeviceBaseAddress + PCI_BAR3, 0);
          MmioWrite32 (DeviceBaseAddress + PCI_BAR4, 0);
          MmioWrite32 (DeviceBaseAddress + PCI_BAR5, 0);
        }
      }
    }
  }
  //
  // now no more PCI dev on another side of PCI Bridge can safty disable PCI Bridge
  //
  for (Bus = MAX_BUS_NUM_TO_SCAN; Bus > MinBus; --Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        DeviceBaseAddress = MmPciBase (Bus, Dev, Fun);
        if (INVALID_PCI_DEVICE == MmioRead32 (DeviceBaseAddress + PCI_VENDOR_DEV_ID)) {
          if (Fun == 0) {
            break;
          }

          continue;
        }

        RegVal = MmioRead8 (DeviceBaseAddress + PCI_HEADER_TYPE);
        if (PCI_BRIDGE == (RegVal & BIT0)) {
          MmioAnd8 (DeviceBaseAddress + PCI_CMD_REG, (UINT8)~(BIT0 | BIT1 | BIT2 | BIT4));
          MmioWrite8 (DeviceBaseAddress + PCI_PRI_BUS_NUM, 0);
          MmioWrite8 (DeviceBaseAddress + PCI_SUB_BUS_NUM, 0);
          MmioWrite8 (DeviceBaseAddress + PCI_SEC_BUS_NUM, 0);
          MmioWrite32 (DeviceBaseAddress + PCI_PRE_MEMBASE_U, 0);
        }
      } // for ( Fun .. )
    } // for ( Dev ... )
  } // for ( Bus ... )
} // DisablePCIDevicesAndBridges

//AMI_OVERRIDE_START - EIP336316 - Improve WSMT validation on Thunderbolt
/**
  Clear PCIe LINK_DIS, and set PCIe to D0 states

  @param  RpBus                 TBT root port Bus
  @param  RpDev                 TBT root port Device
  @param  RpFun                 TBT root port Function
  @param  LinkEnable            TRUE: enable PCIE link / FALSE: disable PCIE link

  @return                       No return value
**/
VOID
PcieLinkEnable (
  IN  UINT8                     RpBus,
  IN  UINT8                     RpDev,
  IN  UINT8                     RpFun,
  IN  BOOLEAN                   LinkEnable
  )
{
  UINT8                         PowerManagementInterfacePtr = 0;
  UINT8                         PowerManagementControl = 0;
  UINT8                         PcieCapabilityPtr = 0;
  UINT8                         LinkControl = 0;
  UINTN                         LinkControlAddr = 0;
  UINTN                         PowerManagementControlAddr = 0;
  BOOLEAN                       UpdateLinkStatus = FALSE;

  PowerManagementInterfacePtr = PcieFindCapId (RpBus, RpDev, RpFun, 1);
  PcieCapabilityPtr = PcieFindCapId (RpBus, RpDev, RpFun, 0x10);
  PowerManagementControlAddr = MmPCIeAddress (RpBus, RpDev, RpFun, PowerManagementInterfacePtr + 4);
  LinkControlAddr = MmPCIeAddress (RpBus, RpDev, RpFun, PcieCapabilityPtr + 0x10);
  //
  // Set to D0 if it's not.
  //
  PowerManagementControl = MmioRead8 (PowerManagementControlAddr);
  if ((PowerManagementControl & (BIT0 | BIT1)) != 0) {
    PowerManagementControl = (PowerManagementControl & (~(BIT0 | BIT1)));
    MmioWrite8 (PowerManagementControlAddr, PowerManagementControl);
  }

  LinkControl = MmioRead8 (LinkControlAddr);
  if ((LinkControl & BIT4) != 0 && LinkEnable) {
    LinkControl = (LinkControl & (~BIT4));
    UpdateLinkStatus = TRUE;
  }

  if ((LinkControl & BIT4) == 0 && !LinkEnable) {
    LinkControl = (LinkControl | BIT4);
    UpdateLinkStatus = TRUE;
  }

  if (UpdateLinkStatus) {
    MmioWrite8 (LinkControlAddr, LinkControl);
    Stall (20000);
  }
}
//AMI_OVERRIDE_END - EIP336316 - Improve WSMT validation on Thunderbolt

