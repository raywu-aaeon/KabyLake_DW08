/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2019 Intel Corporation.

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
#include <PchAccess.h>
#include <Library/PchPcrLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Protocol/SmmGpiDispatch2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SmmIoTrapDispatch2.h>
#include <Protocol/PchSmmIoTrapControl.h>
#include <PlatformInfo.h>
#include <Protocol/EcAccess.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <Protocol/SmmThunk.h>
#include <Library/AslUpdateLib.h>
#include <Protocol/SmmCpu.h>

// AMI_OVERRIDE_START >>>
#include <SetupVariable.h>
#include <Protocol/AcpiModeEnable.h>
#include <Library/PciLib.h> // AMI_OVERRIDE
// AMI_OVERRIDE_END   <<<
// AMI_OVERRIDE_START - Fix Linux build error.
//#include "EC.H"
#include "EC.h"
// AMI_OVERRIDE_END - Fix Linux build error.
#include "AcpiRes.h"
#include "Setup.h"
#include "PlatformBoardId.h"
#include "CpuAccess.h"
#include <Library/GpioLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>

#include <Library/BoardConfigLib.h>
#include <PlatformBoardConfig.h>
// AMI_OVERRIDE_START  >>>
#ifndef PCH_ACPI_TIMER_ADDR
#define PCH_ACPI_TIMER_ADDR                               PM_TMR_BLK_ADDRESS
#endif
// AMI_OVERRIDE_END  <<< 


//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_GPI_DISPATCH2_PROTOCOL         *mSmmGpiDispatch;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL     *mSmmIoTrapDispatch;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_IO_TRAP_REGISTER_CONTEXT        mIchIoTrapContext64;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_THUNK_PROTOCOL                 *mSmmThunk;
GLOBAL_REMOVE_IF_UNREFERENCED PCH_SMM_IO_TRAP_CONTROL_PROTOCOL       *mPchSmmIoTrapControl;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_HANDLE                              mIchIoTrapHandle64;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_CPU_PROTOCOL                   *mSmmCpu;
VOID                                    *mAcpiReg; // AMI_OVERRIDE
static EFI_EC_ACCESS_PROTOCOL           mEcAccess;

GLOBAL_REMOVE_IF_UNREFERENCED UINT8                             *IGDFPtr;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                           ASLFlagsFound;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                             IGDFlag;
GLOBAL_REMOVE_IF_UNREFERENCED UINT64                            BinaryLocation  = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT64                            TableLocation   = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT64                            *VideoStack;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                           BinaryLoaded  = FALSE;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                           PopupLoaded   = FALSE;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                           PopUpOn       = FALSE;

GLOBAL_REMOVE_IF_UNREFERENCED UINT8                             HVComp        = 1;    // Centering Horizontal & Vertical Compensation.
GLOBAL_REMOVE_IF_UNREFERENCED INT16                             BackLight     = 255;  // Brightness
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                            gNextDevice   = 0;    // next device
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                             ACPIFlag;             // ACPI flag
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                            HotKeyIoTrapAddr;

GLOBAL_REMOVE_IF_UNREFERENCED UINT32                            mGpioSmiPad = 0;      // SMC_EXT_SMI GPIO pad
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                            mEcSlpS0Pad = 0;      // EC_SLP_S0_CS GPIO pad

//
// Global NVS area (communication buffer between SMM and ASL code)
// This area is special because it is in ACPI NVS memory and should
// not be relocated by the OS.  It is accessed in BS, SMM, and by ASL.
//
GLOBAL_REMOVE_IF_UNREFERENCED SYSTEM_AGENT_GLOBAL_NVS_AREA      *mSaGlobalNvsAreaPtr;

GLOBAL_REMOVE_IF_UNREFERENCED UINT8   PopupColorTable[] = {
        0x0ff,0x033,0x000,0x000,0x033,0x0ff,0x0ff,0x033,0x000,0x000,0x033,0x0ff };

GLOBAL_REMOVE_IF_UNREFERENCED UINT8   PopupBinary[] = {
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x0F,0x00,0x3C,0x03,0xC3,0xFF,0xCF,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x0F,0x00,0x3C,0x03,0xC3,0xFF,0xCF,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x3F,0xC0,0x3F,0x0F,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x3F,0xC0,0x3F,0x0F,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0x3F,0xFF,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0x3F,0xFF,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0xF3,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0xF3,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xFF,0xFC,0x3C,0x03,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xFF,0xFC,0x3C,0x03,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0x03,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0x03,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0x03,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0x03,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0x03,0xC0,0x3C,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0x03,0xC3,0xFF,0xCF,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xC0,0x3C,0x3C,0x03,0xC3,0xFF,0xCF,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x0F,0xF0,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x0F,0xF0,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0xFF,0xFF,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0xFF,0xFF,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x0F,0xFF,0xFF,0xF0,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x0F,0xFF,0xFF,0xF0,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x0F,0xFF,0xFF,0xFF,0xFF,0xF0,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x0F,0xFF,0xFF,0xFF,0xFF,0xF0,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };

#define PCI_CFG_ADDR(bus, dev, func, reg) \
    ((VOID *) (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))
//
// Magic number definition for values to be updated
//
#define UINT16_BIT_MAGIC_NUMBER     0xFFFF

//
// BugBug: add it since no A, B Segement available in SMM
//
#define  TEMP_ADDRESS_BELOW_1M      (0x00100000 - 16)

/**
  EC get mother board ID

  @param[in] FabID            Pointer to a UINT8 buffer.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECGetMotherBoardID (
  UINT8       *FabID
  )
{
  EFI_STATUS  Status;
  EcWriteCmd (SMC_FAB_ID);
  Status = EcReadData (FabID);
  return Status;
}

//
// Loads Popup Binary to memory
//
VOID LoadPopupToMemory (
  VOID
  )
{
  EFI_STATUS    Status;
//  UINT64      Temp;

//
//  bugbug: reserved any space in SMM instead of the address below 1M
//  since no A, B segment available in SMM, this space will stored the data
//  copyied from space below 1M

//    BinaryLocation = 0x00100000;
  BinaryLocation = (UINT64) (-1);
  Status = gSmst->SmmAllocatePages (  // Allocate space for video binary
                  AllocateMaxAddress,
                  EfiRuntimeServicesData,
                  EFI_SIZE_TO_PAGES(574),
                  &BinaryLocation
                  ) ;

//
//  bugbug: reserved any space in SMM instead of the address below 1M
//  since no A, B segment available in SMM, this space will stored the data
//  copyied from space below 1M

  // TableLocation = 0x00100000;
  TableLocation = (UINT64) (-1);
  Status = gSmst->SmmAllocatePages (  // Allocate space for color table
                  AllocateMaxAddress,
                  EfiRuntimeServicesData,
                  EFI_SIZE_TO_PAGES(sizeof (PopupColorTable)),
                  &TableLocation
                  ) ;
/*
  Temp = 0x00100000;
  Status = mSmst->SmmAllocatePages (  // Allocate space for color table
                  AllocateMaxAddress,
                  EfiRuntimeServicesData,
                  EFI_SIZE_TO_PAGES(256),
                  &Temp
                  ) ;
  *VideoStack = Temp;
*/
  CopyMem((VOID *)(UINTN)BinaryLocation, PopupBinary, 574);
  CopyMem((VOID *)(UINTN)TableLocation, PopupColorTable, sizeof(PopupColorTable));

//  BinaryLocation = (UINT64)PopupBinary;
//  TableLocation = (UINT64)PopupColorTable;

  BinaryLoaded = TRUE;
  DEBUG ((DEBUG_INFO,"Binary Location = 0x%X\n", BinaryLocation));
  DEBUG ((DEBUG_INFO,"Table Location = 0x%X\n", TableLocation));
}

//
// Get DSDT Table
//
VOID *GetDSDTTable (
  VOID
  )
{
  EFI_ACPI_DESCRIPTION_HEADER *Table;
  UINTN                       Handle;
  EFI_STATUS                  Status;

  ///
  /// Locate table with matching ID
  ///
  InitializeAslUpdateLib();

  Handle = 0;
  Status = LocateAcpiTableBySignature (
              EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
              (EFI_ACPI_DESCRIPTION_HEADER **)  &Table,
              &Handle
              );
  if (EFI_ERROR(Status) || (Table == NULL)) return 0;
  return (VOID*)(UINTN)((EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE *)Table)->Dsdt;
}

/**
  Locates flags in Asl.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS LocateFlagsInAsl (
  VOID
  )
{
  EFI_STATUS    Status;
  EFI_ACPI_DESCRIPTION_HEADER  *DsdtTable;
  UINT32        Length;
  UINT8         *ptr;
  UINT8         *pPRScope;
  UINT32        PRScopeLength;

  ASL_OBJ_INFO  ObjInfo;

  DsdtTable = (EFI_ACPI_DESCRIPTION_HEADER *) GetDSDTTable ();
  if (!DsdtTable) {
    DEBUG ((DEBUG_ERROR, "DSDT Table not found.\n"));
    return EFI_NOT_FOUND;
  }

  Length  = DsdtTable->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER);
  ptr     = (UINT8 *) DsdtTable + sizeof (EFI_ACPI_DESCRIPTION_HEADER);

  //
  // Locate _SB scope
  //
  Status = GetAslObj (ptr, Length, (UINT8 *) "_SB", otScope, &ObjInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Scope _SB not found.\n"));
    return Status;
  }

  pPRScope = (UINT8 *) ObjInfo.DataStart;
  PRScopeLength = (UINT32)ObjInfo.Length;


  // Locate IGD flag
  Status = GetAslObj(pPRScope, Length, (UINT8 *) "IGDF", otName, &ObjInfo);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR,"Name IGDF not found.\n"));
    return Status;
  }
  IGDFPtr = (UINT8*)ObjInfo.DataStart + 1;  //Skip byte prefix

  ASLFlagsFound = TRUE;
  return EFI_SUCCESS;

}

/**
  Update EC IoTrap ASL name.

  @param[in] Value            Value written to ASL name

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS Update_Ec_IoTrap_AslName(UINT64 Value)
{
  EFI_STATUS    Status;
  EFI_ACPI_DESCRIPTION_HEADER   *dsdt;
  UINT8         *ptr;
  ASL_OBJ_INFO  ObjInfo;
  UINT32        Length;
  UINT16        OldValue;

  dsdt = (EFI_ACPI_DESCRIPTION_HEADER *) GetDSDTTable();
  if (!dsdt) {
    DEBUG ((DEBUG_ERROR,"DSDT Table not found.\n"));
    return EFI_NOT_FOUND;
  }

  Length = dsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER);
  ptr = (UINT8*)dsdt + sizeof(EFI_ACPI_DESCRIPTION_HEADER);
  Status = GetAslObj (ptr, Length, (UINT8 *) "IO_H", otOpReg, &ObjInfo);
  if (!(EFI_ERROR(Status))) {
    ptr = (UINT8*)ObjInfo.DataStart;
    OldValue = *(UINT16 *)(ptr + 2);
    *(UINT16*)(ptr + 2) = (UINT16)Value;
    OldValue = (UINT16)Value - OldValue;
    dsdt->Checksum -= (UINT8)(OldValue >> 8) + (UINT8)OldValue ;
  } else ASSERT_EFI_ERROR (Status);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

/**
  Updates the Resource field(BaseAddress and Length) in _CRS Method

  @param[in] BaseAddress      Memory base address
  @param[in] AddLength        Address length

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS UpdateAslObj(
  IN UINT16             BaseAddress,
  IN UINT8              AddLength
  )
{
  EFI_STATUS                   Status;
  EFI_ACPI_DESCRIPTION_HEADER  *DsdtTable;
  UINT32                       Length;
  UINT8                        *ptr;
  UINT8                        *pPRScope;
  UINT32                       PRScopeLength;
  UINT8                        Index;
  UINT32                       *Signature;

  ASL_OBJ_INFO ObjInfo;

  DsdtTable = (EFI_ACPI_DESCRIPTION_HEADER *) GetDSDTTable();
  if (!DsdtTable) {
    DEBUG ((DEBUG_ERROR,"DSDT Table not found.\n"));
    return EFI_NOT_FOUND;
  }

  Length = DsdtTable->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER);
  ptr = (UINT8*)DsdtTable + sizeof(EFI_ACPI_DESCRIPTION_HEADER);

  // Locate _SB scope
  Status =  GetAslObj(ptr, Length, (UINT8 *) "_SB", otScope, &ObjInfo);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR,"Scope _SB not found.\n"));
    return Status;
  }
  pPRScope = (UINT8 *) ObjInfo.DataStart;
  PRScopeLength = (UINT32)ObjInfo.Length;

  // Locate PMSC Device
  Status = GetAslObj(pPRScope, Length, (UINT8 *) "LDRC", otDevice, &ObjInfo);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR,"Device LDRC not found.\n"));
    return Status;
  }

  //
  // Conditional match.  Search _CSR in Device (PMSC).
  //
  for (ptr = (UINT8 *)ObjInfo.DataStart; ptr <= (UINT8 *)ObjInfo.DataStart + ObjInfo.Length; ptr++) {
    //
    // Get a pointer to compare for signature
    //
    Signature = (UINT32*) ptr;

    //
    // Check if the signature is "_CRS"
    //
    if ((*Signature) == 0x5352435F) {

      //
      // Now look for an empty resource entry, fix the base address and length fields
      //
      for (Index = 0; *(UINT16*) (ptr + 9 + 8 * Index) != 0x0079; Index++) {
//
// Abstract: Modify _CRS package algorithm
//
        if (*(UINT16*) (ptr + 11 + 8 * Index) == UINT16_BIT_MAGIC_NUMBER) {
          //
          // Fixup the Base Address and Length.
          //
          *(UINT16*) (ptr  + 11 + 8 * Index) = BaseAddress;
          *(UINT16*) (ptr  + 13 + 8 * Index) = BaseAddress;
          *(ptr  + 16 + 8 * Index) = AddLength;
//
// Abstract End
//
          break;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Switch EC to ACPI Mode.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECEnableACPIMode (
  VOID
  )
{
//  ECDisableSMINotify();
  EcWriteCmd (SMC_SMI_DISABLE);
  EcWriteCmd (SMC_ENABLE_ACPI_MODE);
  if ( mSaGlobalNvsAreaPtr->AlsEnable ) {
    EcWriteCmd (SMC_ALS_ENABLE);
    EcWriteData(0x01); // enable ALS
  }
  else {
    EcWriteCmd (SMC_ALS_ENABLE);
    EcWriteData(0x00); // disable ALS
  }
  return EFI_SUCCESS;

}


/**
  Switch EC to Non-ACPI Mode.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECDisableACPIMode (
  VOID
  )
{
  EcWriteCmd (SMC_DISABLE_ACPI_MODE);
  return EFI_SUCCESS;
}

/**
  EC disable SMI notify.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECDisableSMINotify (
  VOID
  )
{
  EcWriteCmd (SMC_DISABLE_SMI_NOTIFY);
  return EFI_SUCCESS;
}


/**
  EC enable SMI notify.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECEnableSMINotify (
  VOID
  )
{
  EcWriteCmd (SMC_ENABLE_SMI_NOTIFY);
  return EFI_SUCCESS;
}


/**
  EC shut down system.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECShutDownSystem (
  VOID
  )
{
  EFI_STATUS  Status;
  Status = EcWriteCmd (SMC_SYSTEM_POWER_OFF);
  return Status;
}

/**
  Switch EC to ACPI Mode.

  @param[in] Revision

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECGetECVersion (
  UINT8       *Revision
)
{
  EFI_STATUS  Status;
  EcWriteCmd (SMC_READ_REVISION);
  Status = EcReadData (Revision);
  return Status;
}

/**
  EC enable Lan.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECEnableLan (
  VOID
)
{
  EFI_STATUS  Status;
  Status = EcWriteCmd (SMC_LAN_ON);
  return Status;
}


/**
  EC disable Lan.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECDisableLan (
  VOID
)
{
  EFI_STATUS  Status;
  Status = EcWriteCmd (SMC_LAN_OFF);
  return Status;
}

/**
  EC deep Sx config.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
ECDeepSxConfig(
  UINT8     EcData
)
{
  EFI_STATUS  Status;
  Status = EcWriteCmd (SMC_DEEPSX_CMD);
  Status = EcWriteData (EcData);
  return Status;
}

/**
  EC turbo ctrl mode.

    @param[in] Enable             Enable / Disable turbo ctrl test mode
    @param[in] ACBrickCap         Data of AC Brick Capacity
    @param[in] ECPollPeriod       Data of EC Poll Period
    @param[in] ECGuardBandValue   Value of EC Guard Band
    @param[in] ECAlgorithmSel     Value of EC Algorithm Selection

  @retval EFI_SUCCESS           Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcTurboCtrlMode(
    UINT8    Enable,
    UINT8    ACBrickCap,
    UINT8    ECPollPeriod,
    UINT8    ECGuardBandValue,
    UINT8    ECAlgorithmSel
)
{
  EFI_STATUS  Status;

  if (Enable) {
    Status = EcWriteCmd (SMC_TURBOCTRL_TESTMODE_ENABLE);
    Status = EcWriteData (ACBrickCap);
    Status = EcWriteData (ECPollPeriod);
    Status = EcWriteData (ECGuardBandValue);
    Status = EcWriteData (ECAlgorithmSel);
  } else {
    Status = EcWriteCmd (SMC_TURBOCTRL_TESTMODE_DISABLE);
  }

  return Status;

}

/**
  Allow EC to control fan when OS is hung.

  @param[in] CpuTemp
  @param[in] CpuFanSpeed

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcFailSafeFanCtrl(
    UINT8    CpuTemp,
    UINT8    CpuFanSpeed
)
{
  EFI_STATUS  Status;
  Status = EcWriteCmd (SMC_FAIL_SAFE_FAN_CTRL);
  Status = EcWriteData (CpuTemp);
  Status = EcWriteData (CpuFanSpeed);

  return Status;
}

static
VOID
Stall (
  UINTN     Usec
  )
{
  UINTN   i;
  UINT32  Data32;
  UINT32  PrevData;
  UINTN   Counter;

  Counter = (UINTN) ((Usec * 10) / 3);
  //
  // Call WaitForTick for Counter + 1 ticks to try to guarantee Counter tick
  // periods, thus attempting to ensure Microseconds of stall time.
  //
  if (Counter != 0) {

    PrevData = IoRead32 (PcdGet16 (PcdAcpiBaseAddress) + 8);
    for (i = 0; i < Counter;) {
      Data32 = IoRead32 (PcdGet16 (PcdAcpiBaseAddress) + 8);
      if (Data32 < PrevData) {
        //
        // Reset if there is a overlap
        //
        PrevData = Data32;
        continue;
      }

      i += (Data32 - PrevData);
      PrevData = Data32;
    }
  }

  return ;
}

/**
  Description: Enable and Disable Port 66/64 IO trap When EC enters and exits low power mode

  @param[in] DispatchHandle  - Handle of this dispatch function
  @param[in] DispatchContext - Pointer to the dispatch function's context.
                                  The DispatchContext fields are filled in by the dispatching driver
                                  prior to invoke this dispatch function

**/
VOID EcCsSmiHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN EFI_SMM_GPI_REGISTER_CONTEXT  *DispatchContext
)
{
  EFI_STATUS  Status;
  UINT32      EcSmiGpiVal;
  UINT32      EcLowPowerExitGpioVal;

  EcSmiGpiVal = 0;

  //
  // Read config register for EC SMI# GPIO
  //
  GpioGetInputInversion (mGpioSmiPad, &EcSmiGpiVal);

  //
  // Change GPI_INV bit of EC SMI# so we enter handler on next edge.
  //
  EcSmiGpiVal ^= BIT0;
  GpioSetInputInversion (mGpioSmiPad, EcSmiGpiVal);

  //
  // Clear ALT_GPI_SMI_STS register for GPIO (EC SMI#)
  //
  GpioClearGpiSmiSts (mGpioSmiPad);

  //
  // Read config register for GPIO (EC Low Power Exit)
  //
  GpioGetInputValue (mEcSlpS0Pad, &EcLowPowerExitGpioVal);

  //
  // If GPIO (EC SMI#) is Low, then Enable IO trap, and Drive GPIO (EC Low Power Exit) Low
  //
  if(EcSmiGpiVal == 0) {
    //
    // Enable I/O Trap for Ports 64/66
    //
    Status = mPchSmmIoTrapControl->Resume (
                            mPchSmmIoTrapControl,
                            mIchIoTrapHandle64
                            );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "mPchSmmIoTrapControl->Resume returned %r\n", Status));
      return;
    }

    //
    // Drive GPIO (EC Low Power Exit) Low
    //
    EcLowPowerExitGpioVal = 0;

  //
  // Else GPIO (EC SMI#) is High, Diable IO trap, and Drive (EC Low Power Exit) High
  //
  } else {
    //
    // Disable IO Trap for Ports 64/66
    //
    Status = mPchSmmIoTrapControl->Pause (
                          mPchSmmIoTrapControl,
                          mIchIoTrapHandle64
                          );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "mPchSmmIoTrapControl->Pause returned %r\n", Status));
      return;
    }

    //
    // Drive GPIO (EC Low Power Exit)  High
    //
    EcLowPowerExitGpioVal = 1;
  }

  //
  // Write new value to GPIO (EC Low Power Exit)
  //
  GpioSetOutputValue (mEcSlpS0Pad, EcLowPowerExitGpioVal);

  return;
}

/**
  Traps on reads to IO ports 64/66 and when a read occurs, a wake signal is sent to the EC. The funtion will
    wait until the EC is awake then perform the read and stuff the EC return value in AL for the OS.

  @param[in] DispatchHandle       - Handle of this dispatch function
  @param[in] DispatchContext      - Pointer to the dispatch function's context.
                                       The DispatchContext fields are filled in by the dispatching driver
                                       prior to invoke this dispatch function
  @param[in] CommBuffer           - Point to the CommBuffer structure
  @param[in] CommBufferSize       - Point to the Size of CommBuffer structure
**/
VOID
EcCsIoTrapHandler (
  IN EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                  *DispatchContext,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
)
{
  UINT16                      CpuIndex;
  EFI_SMM_SAVE_STATE_IO_INFO  IoInfo;
  UINT8                       IoPort;
  UINT8                       IoPortValue;
  UINT32                      Count;
  UINT32                      EcSmiGpiVal;
  UINT32                      EAX;
  EFI_STATUS                  Status;

  IoPort      = 0;
  Count       = 0;
  IoPortValue = 0;
  EcSmiGpiVal = 0;
  //
  // Check EC Low Power Status. If EC is not in Low Power mode, we should pause IO Trap control.
  //
  Status = GpioGetInputValue (mGpioSmiPad, &EcSmiGpiVal);
  if (EcSmiGpiVal == 1) {
    //
    // Disable IO Trap for Ports 64/66 if EC is not in Low Power State
    //
    Status = mPchSmmIoTrapControl->Pause (
                          mPchSmmIoTrapControl,
                          mIchIoTrapHandle64
                          );
    DEBUG ((DEBUG_INFO, "mPchSmmIoTrapControl->Pause returned %r\n", Status));
    return;
  }

  //
  // Find which Thread caused the IO Trap
  //
  for (CpuIndex = 0; CpuIndex < gSmst->NumberOfCpus; CpuIndex++) {
    Status = mSmmCpu->ReadSaveState(
                        mSmmCpu,
                        sizeof(IoInfo),
                        EFI_SMM_SAVE_STATE_REGISTER_IO,
                        CpuIndex,
                        &IoInfo
                        );
    DEBUG ((DEBUG_INFO, "EC:EFI_SMM_CPU_PROTOCOL->ReadSaveState() returned %r\n", Status));
    if ((IoInfo.IoPort == 0x64) || (IoInfo.IoPort == 0x66)) {
      //
      // Get value of port that was trapped
      //
      IoPort = (UINT8) IoInfo.IoPort;
      break;
    }
  }

  //
  // If IoPort is not either 0x64 or 0x66 after all CPUs looped, then this is not the IO trap we should be dispatched.
  //
  if (!((IoPort == 0x64) || (IoPort == 0x66))) {
    DEBUG ((DEBUG_INFO, "Return as trapped port is not IO ports 64/66\n"));
    return;
  }

  //
  // Drive GPIO (EC Low Power Exit) Low->High Edge to wake EC
  //
  GpioSetOutputValue (mEcSlpS0Pad, V_PCH_GPIO_TX_STATE_LOW);
  Stall(10); //10us
  GpioSetOutputValue (mEcSlpS0Pad, V_PCH_GPIO_TX_STATE_HIGH);
  //
  // Wait until EC has exited low power mode
  //
  DEBUG ((DEBUG_INFO, "Wait For EC to exit from Low Power Mode\n"));
  do {
    Stall(10); //10us
    //
    // Read config register for GPIO
    //
    GpioGetInputValue (mGpioSmiPad, &EcSmiGpiVal);
    Count++;
  } while((EcSmiGpiVal != 1) && (Count < 800));

  if (EcSmiGpiVal != 1) {
    DEBUG ((DEBUG_INFO, "Timeout for EC doesn't exit from Low Power Mode in 8ms and no one can handle this IO trap\n"));
    return;
  }
  DEBUG ((DEBUG_INFO, "EC exited from Low Power Mode\n"));

  //
  // Read Port 66 or 64
  //
  gSmst->SmmIo.Io.Read( &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    IoPort,
                    1,
                    &IoPortValue);
  //second read for timing WA @todo
  gSmst->SmmIo.Io.Read( &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    IoPort,
                    1,
                    &IoPortValue);

  //
  // Return value read from EC to the OS
  //
  Status = mSmmCpu->ReadSaveState (
                      mSmmCpu,
                      sizeof (EAX),
                      EFI_SMM_SAVE_STATE_REGISTER_RAX,
                      CpuIndex,
                      &EAX
                      );
  DEBUG ((DEBUG_INFO, "EC:EFI_SMM_CPU_PROTOCOL->ReadSaveState() returned %r\n", Status));
  ASSERT_EFI_ERROR(Status);

  EAX &= 0xFFFFFF00;
  EAX |= (UINT32)IoPortValue;

  Status = mSmmCpu->WriteSaveState (
                      mSmmCpu,
                      sizeof (EAX),
                      EFI_SMM_SAVE_STATE_REGISTER_RAX,
                      CpuIndex,
                      &EAX
                      );
  DEBUG ((DEBUG_INFO, "EC:EFI_SMM_CPU_PROTOCOL->WriteSaveState() returned %r\n", Status));
  ASSERT_EFI_ERROR(Status);
  return;
}

/**
  Initialize and register for the IO Trap and EC SMI Handler to deal with the EC entering low power mode.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS SetupEcIoTrap (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    EcHandle;
  EFI_SMM_GPI_REGISTER_CONTEXT  EcContext;

  //
  // Enable EC SMI# for SMI
  //
  mGpioSmiPad = PcdGet32 (PcdEcSmiGpio);
  mEcSlpS0Pad = PcdGet32 (PcdEcLowPowerExitGpio);

  //
  // Get GpiNum according to GPIO Group+Pad
  //
  GpioGetGpiSmiNum (mGpioSmiPad, &EcContext.GpiNum);

  //
  // Register SMI handler EC SMI usage during Connected Standby
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmGpiDispatch2ProtocolGuid, NULL, (VOID **) &mSmmGpiDispatch);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Installing EcCsSmiHandler Handler \n"));
  Status = mSmmGpiDispatch->Register (
                              mSmmGpiDispatch,
                              (EFI_SMM_HANDLER_ENTRY_POINT2)EcCsSmiHandler,
                              &EcContext,
                              &EcHandle
                              );
  DEBUG ((DEBUG_INFO, "EcCsSmiHandler Install Status: %x\n", Status));

  //
  // Register EC Low Power IO Trap Handler
  //
  DEBUG ((DEBUG_INFO, "Installing EcCsIoTrapHandler Handler \n"));
  mIchIoTrapContext64.Type         = ReadTrap;//ReadTrap;
  mIchIoTrapContext64.Length       = 4;
  mIchIoTrapContext64.Address      = 0x64;

  Status = mSmmIoTrapDispatch->Register (
                                mSmmIoTrapDispatch,
                                (EFI_SMM_HANDLER_ENTRY_POINT2)EcCsIoTrapHandler,
                                &mIchIoTrapContext64,
                                &mIchIoTrapHandle64
                                );
  DEBUG ((DEBUG_INFO, "IoTrap register Status: %x\n", Status));

  //
  // Disable IO Trap Handler
  //
  Status = gSmst->SmmLocateProtocol (&gPchSmmIoTrapControlGuid, NULL, (VOID **) &mPchSmmIoTrapControl);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  Status = mPchSmmIoTrapControl->Pause (
                        mPchSmmIoTrapControl,
                        mIchIoTrapHandle64
                        );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "mPchSmmIoTrapControl->Pause returned %r\n", Status));
    return Status;
  }
  return EFI_SUCCESS;
}

// AMI_OVERRIDE_START >>> Support AMI acpi Enlink.
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ECAcpiEnableLink
//
// Description: This routine will be called when ACPI enabled.
//
// Parameters:  DispatchHandle - SMM Dispatch Handle
//
// Returns:     None
//
// Modified:
//
// Referrals:
//
// Notes:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID ECAcpiEnableLink (
  IN EFI_HANDLE   DispatchHandle )
{
    mEcAccess.SMINotifyDisable();
    mEcAccess.AcpiEnable();
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ECAcpiDisableLink
//
// Description: This routine will be called when ACPI disabled.
//
// Parameters:  DispatchHandle - SMM Dispatch Handle
//
// Returns:     None
//
// Modified:
//
// Referrals:
//
// Notes:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID ECAcpiDisableLink (
    IN EFI_HANDLE    DispatchHandle )
{
    mEcAccess.AcpiDisable();
}


//----------------------------------------------------------------------------

EFI_STATUS
EFIAPI
CreateECAcpiEnDisLink (
    IN CONST EFI_GUID  *Protocol,
    IN VOID            *Interface,
    IN EFI_HANDLE      Handle )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_ACPI_DISPATCH_PROTOCOL  *mAcpiEnDispatch;
    EFI_ACPI_DISPATCH_PROTOCOL  *mAcpiDisDispatch;
    EFI_HANDLE                  EcAHandle;
    
    Status = gSmst->SmmLocateProtocol (&gEfiEcAccessProtocolGuid, \
                                       NULL, \
                                       (VOID*)&mEcAccess );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = gSmst->SmmLocateProtocol (&gEfiAcpiEnDispatchProtocolGuid, \
                                       NULL, \
                                       &mAcpiEnDispatch );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = mAcpiEnDispatch->Register (mAcpiEnDispatch, \
                                        ECAcpiEnableLink, \
                                        &EcAHandle );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = gSmst->SmmLocateProtocol (&gEfiAcpiDisDispatchProtocolGuid, \
                                       NULL, \
                                       &mAcpiDisDispatch );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = mAcpiDisDispatch->Register( mAcpiDisDispatch, \
                                         ECAcpiDisableLink, \
                                         &EcAHandle );
    
    DEBUG((DEBUG_INFO, "EcSmi: CreateECAcpiEnDisLink Status = %r.\n", Status));    
    
    return Status;
}
// AMI_OVERRIDE_END <<<

/**
  EC initializes.

  @param[in] ImageHandle      Image handle of this driver.
  @param[in] SystemTable      Pointer to the System Table.

  @retval EFI_SUCCESS         Function successfully executed.
  @retval EFI_UNSUPPORTED     Install protocol failure.
**/
EFI_STATUS
EFIAPI
InitializeEcSmm (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_STATUS                                Status2;
  EFI_STATUS                                Status3;
  SA_SETUP                                  SaSetup;
  SETUP_DATA                                SetupData;
  UINTN                                     VariableSize;
  UINT64                                    TempMsr;
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL     *SaGlobalNvsAreaProtocol;
  PLATFORM_INFO                             *PlatformInfo;
  CPU_SETUP                                 CpuSetup;
  UINT8                                     EcPowerFeatures;

  Status = gSmst->SmmLocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, (VOID **) &mSmmCpu);
  ASSERT_EFI_ERROR(Status);

  Status = gSmst->SmmLocateProtocol (&gSmmThunkProtocolGuid, NULL, (VOID **) &mSmmThunk);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "SMM Thunk Protocol not located.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "SMM Thunk Protocol located.\n"));
  }

  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  if ((PlatformInfo->EcPresent == FALSE) ||
        (PlatformInfo->PlatformFlavor == FlavorWorkstation) ||
        (PlatformInfo->BoardType      == BoardTypeSv)
        ) {
    return EFI_SUCCESS;
  }

  mEcAccess.Handle            = NULL;
  mEcAccess.QuerryCmd         = EcQueryCommand;
  mEcAccess.WriteCmd          = EcWriteCmd;
  mEcAccess.WriteData         = EcWriteData;
  mEcAccess.ReadData          = EcReadData;
  mEcAccess.ReadMem           = EcReadMem;
  mEcAccess.WriteMem          = EcWriteMem;
  mEcAccess.AcpiEnable        = ECEnableACPIMode;
  mEcAccess.AcpiDisable       = ECDisableACPIMode;
  mEcAccess.SMINotifyEnable   = ECEnableSMINotify;
  mEcAccess.SMINotifyDisable  = ECDisableSMINotify;
  mEcAccess.ShutDownSystem    = ECShutDownSystem;
  mEcAccess.GetMotherBoardID  = ECGetMotherBoardID;
  mEcAccess.GetECVersion      = ECGetECVersion;
  mEcAccess.EnableLan         = ECEnableLan;
  mEcAccess.DisableLan        = ECDisableLan;
  mEcAccess.DeepSxConfig      = ECDeepSxConfig;
  mEcAccess.TurboCtrlMode     = EcTurboCtrlMode;
  mEcAccess.GetSwitchStatus   = EcGetSwitchStatus;
  mEcAccess.FailSafeFanCtrl   = EcFailSafeFanCtrl;

  Status = gSmst->SmmInstallProtocolInterface (
                  &mEcAccess.Handle,
                  &gEfiEcAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mEcAccess
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "InstallMultipleProtocolInterfaces returned %r\n", Status));
    return EFI_UNSUPPORTED;
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Couldn't register the EC SMI handler.  Status: %r\n", Status));
    return Status;
  }

  EcWriteCmd (SMC_DISABLE_ACPI_MODE);
  EcWriteCmd (SMC_ENABLE_POWER_SWITCH);
  EcWriteCmd (SMC_ENABLE_SMI_NOTIFY);
  EcWriteCmd (SMC_SMI_ENABLE);
  EcWriteCmd (SMC_SET_ETM_COMMAND);

  //
  // Register SMM IO Trap
  //
  Status = gSmst->SmmLocateProtocol(&gEfiSmmIoTrapDispatch2ProtocolGuid, NULL, (VOID **) &mSmmIoTrapDispatch);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate IoTrap Protocol returned %r\n", Status));
    return Status;
  }

  //
  // Locate our shared data area
  //
  Status = gBS->LocateProtocol (&gSaGlobalNvsAreaProtocolGuid, NULL, (VOID **) &SaGlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_START - Get mSaGlobalNvsAreaPtr from SaGlobalNvsAreaProtocol->Area.
  mSaGlobalNvsAreaPtr = SaGlobalNvsAreaProtocol->Area;
// AMI_OVERRIDE_END - Get mSaGlobalNvsAreaPtr from SaGlobalNvsAreaProtocol->Area.

  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  (CHAR16 *) L"Setup",
                  &gSetupVariableGuid,
                  (UINT32 *) NULL,
                  &VariableSize,
                  &SetupData
                  );

  VariableSize = sizeof (SA_SETUP);
  Status2 = gRT->GetVariable (
                  (CHAR16 *) L"SaSetup",
                  &gSaSetupVariableGuid,
                  (UINT32 *) NULL,
                  &VariableSize,
                  &SaSetup
                  );

  VariableSize = sizeof (CPU_SETUP);
  Status3 = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &CpuSetup
                  );

  if (!EFI_ERROR (Status) && !EFI_ERROR (Status2) && !EFI_ERROR (Status3)) {
    EcWriteCmd (SMC_SET_SHUTDOWN_TEMP);
    if (CpuSetup.AutoThermalReporting) {
      //
      // Use TjMax from MSR (0x1A2) for Automatic Thermal Reporting option
      //
      TempMsr = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
      TempMsr &= (UINT64)(B_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_MASK);
      EcWriteData ((UINT8)(TempMsr >> N_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_OFFSET));
    } else {
      //
      // Use Critical Trip Point from Setup
      //
      EcWriteData (SetupData.CriticalThermalTripPoint);
    }
    //
    //Send the command to EC to enable reporting memory temperatures to the processor via PECI.
    //
    EcWriteCmd (SMC_SET_PECI_INJECTED_TEMP);
    EcWriteData (SaSetup.PeciInjectedTemp);
    //
    // Send the command to put the EC into low power mode if it is enabled in setup
    //
    EcPowerFeatures = 0;
// AMI_OVERRIDE_START  >>> for ULT disable build error.
#if defined ULT_SUPPORT && ULT_SUPPORT == 1
    if (SetupData.LowPowerS0Idle && SetupData.EcLowPowerMode && SetupData.CSNotifyEC) {
      EcPowerFeatures |= EC_DEBUG_LOW_POWER_ENABLE;
    }
#endif
// AMI_OVERRIDE_END <<<
    if (SetupData.EcDebugLed){
      EcPowerFeatures |= EC_DEBUG_LED_DISABLE;
    }
    if (SetupData.ECBaseCsPwrPolicy){
      EcPowerFeatures |= EC_DEBUG_BASE_PCS_PWR_POLICY;
    }

    EcWriteCmd (EC_POWER_FEATURES_CMD);
    EcWriteData (EcPowerFeatures);

// AMI_OVERRIDE_START  >>> for ULT disable build error.
#if defined ULT_SUPPORT && ULT_SUPPORT == 1
    if (SetupData.LowPowerS0Idle && SetupData.EcLowPowerMode && SetupData.CSNotifyEC) {
    SetupEcIoTrap();
    }
#endif
// AMI_OVERRIDE_END <<<
  }
  
// AMI_OVERRIDE_START >>> Support AMI acpi Enlink. 
    Status = gSmst->SmmRegisterProtocolNotify (
                          &gEfiAcpiEnDispatchProtocolGuid,
                          CreateECAcpiEnDisLink,
                          &mAcpiReg
                          );

    CreateECAcpiEnDisLink(NULL,NULL,NULL);
// AMI_OVERRIDE_END
  return EFI_SUCCESS;

}

/**
  Read the EC Query Value

  @param[in] pQdata           Pointer to a UINT8 buffer.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcQueryCommand (
  UINT8   *pQdata
)
{
  EFI_STATUS  Status;
  Status = EcWriteCmd (SMC_QUERY_SMI);
  Status = EcReadData (pQdata);
  return EFI_SUCCESS;
}


/**
  Wait till EC I/P buffer is free.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EcIbFree (
  VOID
  )
{
  UINT8   Status;
  do {
    gSmst->SmmIo.Io.Read( &gSmst->SmmIo,
            SMM_IO_UINT8,
            EcCommandPort,
            1,
            &Status);
  } while (Status & 2);
  return EFI_SUCCESS;

}


/**
  Wait till EC O/P buffer is full

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EcObFull (
  VOID
  )
{
  UINT8    Status;
  do {
    gSmst->SmmIo.Io.Read( &gSmst->SmmIo, SMM_IO_UINT8, EcCommandPort, 1, &Status);
  } while (!(Status & 1));

  return EFI_SUCCESS;

}


/**
  Send EC command

  @param[in] cmd              Writing command to EC's command port.

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcWriteCmd (
  UINT8    cmd
)
{
  EcIbFree();
  gSmst->SmmIo.Io.Write( &gSmst->SmmIo,
           SMM_IO_UINT8,
           EcCommandPort,
           1,
           &cmd);
  return EFI_SUCCESS;
}


/**
   Write Data from EC data port

   @param[in] data             Writing data to EC's data port.

   @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcWriteData (
  UINT8    data
)
{
  EcIbFree();
  gSmst->SmmIo.Io.Write( &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    EcDataPort,
                    1,
                    &data);
  return EFI_SUCCESS;
}

/**
   Read Data from EC data Port.

   @param[in] pData            Pointer to a UINT8 buffer.

   @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcReadData (
  UINT8   *pData
)
{
  if (EFI_ERROR(EcObFull())) return EFI_DEVICE_ERROR;
  gSmst->SmmIo.Io.Read( &gSmst->SmmIo, SMM_IO_UINT8, EcDataPort, 1, pData);
  return EFI_SUCCESS;
}


/**
  Read Data from EC Memory from location pointed by Index.

  @param[in] Index            Index of the read data
  @param[in] Data             Data to be read

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcReadMem (
  UINT8  Index,
  UINT8  *Data
)
{
  UINT8  cmd = SMC_READ_EC;
  EcWriteCmd (cmd);
  EcWriteData(Index);
  EcReadData(Data);
  return EFI_SUCCESS;
}


/**
  Write Data to EC memory at location pointed by Index.

  @param[in] Index            Index of the written data
  @param[in] Data             Data to be written

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcWriteMem (
  UINT8  Index,
  UINT8  Data
)
{
  UINT8  cmd = SMC_WRITE_EC;
  EcWriteCmd (cmd);
  EcWriteData(Index);
  EcWriteData(Data);
  return EFI_SUCCESS;
}

/**
  Get the info from EC on Developer Mode / Recovery Mode switch.
   EC will return 0 in bit3 when the Recovery Mode switch is pressed and
   will return 0 in bit6 when Developer Mode switch is pressed.

  @param[in] Data             Read data from EC

  @retval EFI_SUCCESS         Function successfully executed.
**/
EFI_STATUS
EFIAPI
EcGetSwitchStatus (
  UINT8   *Data
)
{
  UINT8  cmd = SMC_GET_SWITCH_STATUS;
  EcWriteCmd (cmd);
  EcReadData(Data);
  return EFI_SUCCESS;
}
