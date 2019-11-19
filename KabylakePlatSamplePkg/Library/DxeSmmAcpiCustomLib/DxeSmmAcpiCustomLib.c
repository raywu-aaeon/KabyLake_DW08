/** @file
  SMM ACPI Custom library implementation.

  These functions need to be SMM safe.

  These functions require the SMM IO library (SmmIoLib) and EC library (BaseEcLib) to be present.
  Caller must link those libraries and have the proper include path.

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

@par Specification Reference:
**/

// AMI_OVERRIDE_START >>> Support MdePkg >= 15
//#include <Library/SmmIoLib.h>
#include <KabylakePlatSamplePkg/Include/Library/SmmIoLib.h>
// AMI_OVERRIDE_END <<<
#include <Library/DxeSmmAcpiCustomLib.h>
#include <PchAccess.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <Library/DxeSmmInternalGraphicDeviceLib.h>

//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED UINT8 BrightnessLevel = 100;
extern SYSTEM_AGENT_GLOBAL_NVS_AREA *mSaGlobalNvsAreaPtr;

//
// Function implemenations
//


/**
  Set up CRB specific ACPI Values/Registers before booting to ACPI OS
    1) Clear SMBus Semaphore
    2) Set IGD NVS values
    3) Enable fan control by CPU temperature  if thermal offset enabled

**/
VOID
PlatformSpecificAcpiEnableHook (
  VOID
  )
{
  UINT64  SmBusPciAddress;
  UINT16  SmBusBaseAddress;
  UINT64  IgdPciAddress;
  UINT16  IgdRegValue;
  EFI_IA32_REGISTER_SET   Registers;

  //
  // CLear SMBus Semaphore
  //
  SmBusPciAddress = SMM_PCI_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SMBUS, PCI_FUNCTION_NUMBER_PCH_SMBUS, R_PCH_SMBUS_BASE);

  SmmPciCfgRead(
      SmmPciWidthUint16,
      (SMM_PCI_IO_ADDRESS*)&SmBusPciAddress,
      &SmBusBaseAddress
      );

  SmBusBaseAddress &= ~1 ;
  SmmIoWrite8 (SmBusBaseAddress, 0xFF);

  // 1) Init Brightness Percentage.
  // 2) Init CADL/PADL.
  // 3) Init DID Values and number of valid DIDs.
  IgdPciAddress = SMM_PCI_ADDRESS(0, 2, 0, 0);
  SmmPciCfgRead(
      SmmPciWidthUint16,
      (SMM_PCI_IO_ADDRESS*)&IgdPciAddress,
      &IgdRegValue
      );


  // Initialize the IGD Registers default values in case one of the
  // following conditions is met:
  //       1) IGD is a Secondary Display Device, making it not
  //          possible for BIOS to make INT10h calls
  //       2) An INT10h call fails
  //       3) A maximum of 5 encoders are present at any given time,
  //          so not all DIDs will get updated with a valid
  //          value.  Per the ACPI Spec, a '0' is valid.  Per WHQL,
  //          addresses must be unique.  Therefore, invalid DIDs
  //          must be initialized to unique values.
  //       4) A Static solution is implemented in VBT (Display must
  //          be attached for Hot-Key).
  //
  //       DID1 (080000100h) = CRT,
  //       DID2 (080000240h) = TV,
  //       DID3 (080000320h) = EFP,
  //       DID4 (080000410h) = LFP.
  //       DID5 = Invalid DIDs
  //       will set BIT31 = 0 to clearly define "another scheme" when it is
  //       reported to the Miniport Driver.
  if (IgdRegValue != 0xFFFF) {
     mSaGlobalNvsAreaPtr->BrightnessPercentage = BrightnessLevel ;
     mSaGlobalNvsAreaPtr->DeviceId1 = 0x80000100;
     mSaGlobalNvsAreaPtr->DeviceId2 = 0x80000400;
     mSaGlobalNvsAreaPtr->DeviceId3 = 0x80000200;
     mSaGlobalNvsAreaPtr->DeviceId4 = 0x04;
     mSaGlobalNvsAreaPtr->DeviceId5 = 0x05;
     mSaGlobalNvsAreaPtr->NumberOfValidDeviceId = 4;
     mSaGlobalNvsAreaPtr->CurrentDeviceList = 0x0F;

     //
     // --cr--: dynamically check mobile vs. dtop
     //
     if (IsPrimaryDisplayDevice((V_SA_DEVICE_ID_SKL_MB_ULT_1 << 16) | V_SA_MC_VID) ||
         IsPrimaryDisplayDevice((V_SA_DEVICE_ID_SKL_MB_ULX_2 << 16) | V_SA_MC_VID) ||
         IsPrimaryDisplayDevice((V_SA_DEVICE_ID_SKL_MB_ULX_3 << 16) | V_SA_MC_VID) ||
         IsPrimaryDisplayDevice((V_SA_DEVICE_ID_KBL_MB_ULT_1 << 16) | V_SA_MC_VID) ||
         IsPrimaryDisplayDevice((V_SA_DEVICE_ID_KBL_MB_ULX_1 << 16) | V_SA_MC_VID) ||
         IsPrimaryDisplayDevice((V_SA_DEVICE_ID_AML_MB_ULX_1 << 16) | V_SA_MC_VID)
        ) {
         mSaGlobalNvsAreaPtr->IgdState = 1 ;
         GetCadl (&Registers) ;
         if (Registers.X.Flags.ZF == 0) {
         Registers.H.CH |= BIT3;
         mSaGlobalNvsAreaPtr->CurrentDeviceList = Registers.H.CH;
         }
     }
  }
}


/**


**/
VOID
PlatformSpecificAcpiDisableHook (
  VOID
  )
{

}
