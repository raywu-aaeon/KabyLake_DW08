/** @file
  Thunderbolt(TM) Setup Rountines.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2018 Intel Corporation.

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

#include <Library/HiiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/CpuPlatformLib.h>
#include "Base.h"
#include <Library/MmPciLib.h>
#include <SetupPrivate.h>
#include "SaAccess.h"
#include <PcieRegs.h>
#include <TbtBoardInfo.h>
#include <Library/TbtCommonLib.h>
#include <Library/GpioExpanderLib.h>
#include <GpioPinsSklLp.h>
#include <Guid/HobList.h>
// AMI_OVERRIDE_START - Fix building error for RC.
#include <AmiSetupPrivate.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
// AMI_OVERRIDE_END - Fix building error for RC.
// AMI_OVERRIDE_START - Add for updating the PCIe ClkReqNum.
#include <Library/PchInfoLib.h>
// AMI_OVERRIDE_END - Add for updating the PCIe ClkReqNum.
// AMI_OVERRIDE_START - follow AMI Usb
#include <Protocol/UsbPolicy.h>
// AMI_OVERRIDE_END - follow AMI Usb


TBT_INFO_HOB                      *gTbtInfoHob = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN    mTbtOsFormEntry = FALSE;
// AMI_OVERRIDE_START - EIP279338 The resource for PEG Thunderbolt host is incorrect
#define PCH_PCIE_NUM  (24)
#define PEG_NUM   (3)
#define PCIE_NUM  (PCH_PCIE_NUM + PEG_NUM)
// AMI_OVERRIDE_END - EIP279338 The resource for PEG Thunderbolt host is incorrect

static
VOID
TbtSetPCIe2TbtCommand (
  IN    UINT8        command,
  IN    UINT8        TbtUsBus
  )
{
  volatile UINT32 RegVal;
  volatile UINT32 MaxWaitIter;
  volatile UINT8  RetCode;
  UINTN           DeviceBaseAddress;
  //
  // Wait 5 sec
  //
  MaxWaitIter = 50;
  RetCode     = 0x52;
  DEBUG ((DEBUG_INFO, "TbtSetPCIe2TBTCommand: command= %x \n", command));
  DeviceBaseAddress = MmPciBase (TbtUsBus, 0x00, 0x00);
  MmioWrite32 (DeviceBaseAddress + PCIE2TBT_R, command | PCIE2TBT_VLD_B);

  IoWrite8 (0x80, 0x50);

  while (MaxWaitIter-- > 0) {
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

    gBS->Stall (1 * 1000);
  }

  MmioWrite32 (DeviceBaseAddress + PCIE2TBT_R, 0);
  IoWrite8 (0x80, RetCode);
}

/**
  GPIO write function

  @param  GpioAccessType        Type01: call GpioSetOutputValue;                    Type02: call GpioExpSetOutput
  @param  Expander              Type01: don't care;                                 Type02: Expander Value with in the Contoller
  @param  GpioNumber            Type01: GPIO pad;                                   Type02: Pin with in the Expnader Value
  @param  Value                 Type01: Output value: 0: OutputLow, 1: OutputHigh;  Type02: none

  @return                       No return value
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
    GpioExpSetOutput(Expander, (UINT8)GpioNumber, (UINT8)Value);
  }
}

/**
  GPIO read function

  @param  GpioAccessType        Type01: call GpioSetOutputValue;                    Type02: call GpioExpSetOutput
  @param  Expander              Type01: don't care;                                 Type02: Expander Value with in the Contoller
  @param  GpioNumber            Type01: GPIO pad;                                   Type02: Pin with in the Expnader Value
  @param  PadState              {0: GPO [GPIO TX State], 1: GPI [GPIO RX State]}

  @return                       Return data
**/
UINT8
GpioRead (
  IN  UINT8                     GpioAccessType,
  IN  UINT8                     Expander,
  IN  UINT32                    GpioNumber,
  IN  UINT8                     PadState
  )
{
  UINT8                         Data8;
  UINT32                        Data32;

  Data8 = 0;
  Data32 = 0;

  if (GpioAccessType == 0x01) {
    // PCH
    if (PadState == 0x01) {
      // GPIO RX State
      GpioGetInputValue (GpioNumber, &Data32);
      Data8 = (UINT8) Data32;
    } else {
      // GPIO TX State
      GpioGetOutputValue (GpioNumber, &Data32);
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

  Force Power function.

  @param  GpioAccessType        Type01: call GpioSetOutputValue;                    Type02: call GpioExpSetOutput
  @param  Expander              Type01: don't care;                                 Type02: Expander Value with in the Contoller
  @param  GpioNumber            Type01: GPIO pad;                                   Type02: Pin with in the Expnader Value
  @param  Value                 Type01: Output value: 0: OutputLow, 1: OutputHigh;  Type02: none

  @return                       No return value
**/
VOID
ForcePower(
  IN  UINT8                     GpioAccessType,
  IN  UINT8                     Expander,
  IN  UINT32                    GpioNumber,
  IN  BOOLEAN                   Value
  )
{
  GpioWrite(GpioAccessType, Expander, GpioNumber, Value);
}

/**
  Read/Write the GPE_STS for CioPlugEventGpio.AcpiGpeSts

  @param  AcpiGpeStsAddress     ACPI GPE status address
  @param  AcpiGpeStsBit         ACPI GPE status BIT
  @param  GpioPad               GPIO PAD
  @param  Operation             {0: Clear the bit, 1:Read the bit}

  @retval 0                     If Operation = 0
  @retval others                If Operation = 1, return Data
**/
UINT8
AcpiGpeStsRW (
  IN  UINT16                    AcpiGpeStsAddress,
  IN  UINT8                     AcpiGpeStsBit,
  IN  GPIO_PAD                  GpioPad,
  IN  UINT8                     Operation
  )
{
  UINT16                        Address;
  UINT8                         BitPosition;
  UINT8                         Mask;
  UINT8                         Data8;
  BOOLEAN                       TwoTierState;
  UINT32                        Data32;

  TwoTierState = GpioCheckFor2Tier (GpioPad);

  Address = AcpiGpeStsAddress;
  BitPosition = AcpiGpeStsBit;

  while (BitPosition > 8) {
    Address += 1;
    BitPosition -= 8;
  }

  Mask = 1 << BitPosition;
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
      Data8 >>= BitPosition;
      Data8 &= 1;
    }
  }

  return (Data8);
}

typedef struct _TITAN_RIDGE_ID_TABLE {
  UINT32 DevID;
} TITAN_RIDGE_DevID_Table;

TITAN_RIDGE_DevID_Table Titan_Ridge_DevID_Tbl[] =
{
  {0x15EA8086},  // TR DP
  {0x15E78086},  // TR SP
};

BOOLEAN
Check_SL4_Supported(
  IN    UINTN        TbtBaseAddress
)
{
  UINT32       REG_VAL;
  UINT8        Index;
  UINTN        Titan_Ridge_DevID_Table_Size;

  REG_VAL = MmioRead32(TbtBaseAddress);
  if (0xFFFFFFFF == REG_VAL)
    return TRUE;

  Titan_Ridge_DevID_Table_Size = sizeof(Titan_Ridge_DevID_Tbl) / sizeof(TITAN_RIDGE_DevID_Table);
  for (Index = 0; Index < Titan_Ridge_DevID_Table_Size; Index++) {
    if (Titan_Ridge_DevID_Tbl[Index].DevID == REG_VAL) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  This function will Get Security Level.

  @param  TbtSelector           Security Mode Selection option in BIOS Setup

  @reture                       Error Code or Success
**/
UINT8
GetSecLevel (
  IN    UINT8 TbtSelector,
  IN    UINT8 Command
  )
{
  UINTN        TbtBaseAddress;
  UINT32       REG_VAL;
  UINT32       MaxWaitIter;
  UINT8        RetCode;
  UINT8        TbtUsBus;
  UINT8        ret;
  UINT8        Index;
  UINTN        RpDev;
  UINTN        RpFunc;
  EFI_STATUS   Status;

  MaxWaitIter = 50;  // Wait 5 sec
  RetCode       = 0x82;
  TbtUsBus    = 3;
  ret           = 0xFF;

  DEBUG ((DEBUG_INFO, "GetSecLevel() \n"));

  Index = TbtSelector;
  if (Index == 0) {
    Index = gTbtInfoHob->TbtDefaultSelectorInfo;
  }

  Status = GetTbtRpDevFun (Index - 1, &RpDev, &RpFunc);
  ASSERT_EFI_ERROR (Status);
  TbtUsBus = MmioRead8 (MmPciBase (0x00, (UINT32)RpDev, (UINT32)RpFunc) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  DEBUG ((DEBUG_INFO, "TbtUsBus = 0x%x \n", TbtUsBus));

  TbtBaseAddress = MmPciBase (TbtUsBus, 0x00, 0x00);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress = 0x%lx \n", TbtBaseAddress));
  MmioWrite32 (TbtBaseAddress + PCIE2TBT_R, Command | PCIE2TBT_VLD_B);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress + PCIE2TBT_R = 0x%lx \n", TbtBaseAddress + PCIE2TBT_R));
  DEBUG ((DEBUG_INFO, "Data to be written = 0x%lx \n", Command | PCIE2TBT_VLD_B));

  IoWrite8 (0x80, 0x80);

  while (MaxWaitIter-- > 0) {
    REG_VAL = MmioRead32 (TbtBaseAddress + TBT2PCIE_R);
    DEBUG ((DEBUG_INFO, "TbtBaseAddress + TBT2PCIE_R = 0x%lx \n", TbtBaseAddress + TBT2PCIE_R));
    DEBUG ((DEBUG_INFO, "Data read = 0x%lx \n", REG_VAL));

    if (0xFFFFFFFF == REG_VAL) {
      //
      // Device is not here return now
      //
      DEBUG ((DEBUG_INFO, "TBT device is not present \n"));
      RetCode = 0x8F;
      break;
    }

    if (REG_VAL & TBT2PCIE_DON_R) {
      RetCode = 0x81;
      ret     = (UINT8) (0xFF & (REG_VAL >> 8));
      break;
    }

    gBS->Stall (100 * 1000);
  }

  MaxWaitIter = 50;   // Wait 5 sec

  MmioWrite32 (TbtBaseAddress + PCIE2TBT_R, 0);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress + PCIE2TBT_R = 0x%lx \n", TbtBaseAddress + PCIE2TBT_R));
  DEBUG ((DEBUG_INFO, "Data to be written = 0x%lx \n", 0));

  IoWrite8 (0x80, 0x82);

  while (MaxWaitIter-- > 0) {
    REG_VAL = MmioRead32 (TbtBaseAddress + TBT2PCIE_R);
    DEBUG ((DEBUG_INFO, "TbtBaseAddress + TBT2PCIE_R = 0x%lx \n", TbtBaseAddress + TBT2PCIE_R));
    DEBUG ((DEBUG_INFO, "Data read = 0x%lx \n", REG_VAL));

    if (0xFFFFFFFF == REG_VAL) {
      //
      // Device is not here return now
      //
      DEBUG ((DEBUG_INFO, "TBT device is not present \n"));
      RetCode = 0x8F;
      break;
    }

    if (!(REG_VAL & TBT2PCIE_DON_R)) {
      RetCode = 0x81;
      break;
    }

    gBS->Stall (100 * 1000);
  }

  IoWrite8 (0x80, RetCode);

  return ret;
}

/**
  This function will Set Security Level.

  @param  Data                  Data being sent to the original driver
  @param  TbtSelector           Security Mode Selection option in BIOS Setup

  @retval TRUE                  If TBT Device present
**/
BOOLEAN
SetSecLevel (
  IN    UINT8 Data,
  IN    UINT8 TbtSelector,
  IN    UINT8 Command
  )
{
  UINTN        TbtBaseAddress;
  UINT32       REG_VAL;
  UINT32       MaxWaitIter;
  UINT8        RetCode;
  UINT8        TbtUsBus;
  BOOLEAN      ret;
  UINT8        Index;
  UINTN        RpDev;
  UINTN        RpFunc;
  EFI_STATUS   Status;


  if (Command == PCIE2TBT_SET_ENUMERATION_METHOD) {
    MaxWaitIter = 150;   // Wait 15 sec
  } else {
    MaxWaitIter = 50;   // Wait 5 sec
  }
  RetCode  = 0x72;
  TbtUsBus = 3;
  ret      = FALSE;

  DEBUG ((DEBUG_INFO, "SetSecLevel() \n"));

  Index = TbtSelector;
  if (Index == 0) {
    Index = gTbtInfoHob->TbtDefaultSelectorInfo;
  }

  Status = GetTbtRpDevFun (Index - 1, &RpDev, &RpFunc);
  ASSERT_EFI_ERROR (Status);
  TbtUsBus = MmioRead8 (MmPciBase (0x00, (UINT32)RpDev, (UINT32)RpFunc) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  DEBUG ((DEBUG_INFO, "TbtUsBus = 0x%x \n", TbtUsBus));

  TbtBaseAddress = MmPciBase (TbtUsBus, 0x00, 0x00);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress = 0x%lx \n", TbtBaseAddress));

  if ((Command == PCIE2TBT_SET_SECURITY_LEVEL) && (Data == 4)) {
    if (!Check_SL4_Supported(TbtBaseAddress))
      return FALSE;
  }

  MmioWrite32 (TbtBaseAddress + PCIE2TBT_R, (Data << 8) | Command | PCIE2TBT_VLD_B);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress + PCIE2TBT_R = 0x%lx \n", TbtBaseAddress + PCIE2TBT_R));
  DEBUG ((DEBUG_INFO, "Data to be written = 0x%lx \n", (Data << 8) | Command | PCIE2TBT_VLD_B));

  IoWrite8 (0x80, 0x70);

  while (MaxWaitIter-- > 0) {
    REG_VAL = MmioRead32 (TbtBaseAddress + TBT2PCIE_R);
    DEBUG ((DEBUG_INFO, "TbtBaseAddress + TBT2PCIE_R = 0x%lx \n", TbtBaseAddress + TBT2PCIE_R));
    DEBUG ((DEBUG_INFO, "Data read = 0x%lx \n", REG_VAL));

    if (0xFFFFFFFF == REG_VAL) {
      //
      // Device is not here return now
      //
      DEBUG ((DEBUG_INFO, "TBT device is not present \n"));
      RetCode = 0x7F;
      break;
    }

    if (REG_VAL & TBT2PCIE_DON_R) {
      RetCode = 0x71;
      ret     = TRUE;
      break;
    }

    gBS->Stall (100 * 1000);
  }

  if (Command == PCIE2TBT_SET_ENUMERATION_METHOD) {
    MaxWaitIter = 150;   // Wait 15 sec
  } else {
    MaxWaitIter = 50;   // Wait 5 sec
  }
  MmioWrite32 (TbtBaseAddress + PCIE2TBT_R, 0);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress + PCIE2TBT_R = 0x%lx \n", TbtBaseAddress + PCIE2TBT_R));
  DEBUG ((DEBUG_INFO, "Data to be written = 0x%lx \n", 0));

  IoWrite8 (0x80, 0x72);

  while (MaxWaitIter-- > 0) {
    REG_VAL = MmioRead32 (TbtBaseAddress + TBT2PCIE_R);
    DEBUG ((DEBUG_INFO, "TbtBaseAddress + TBT2PCIE_R = 0x%lx \n", TbtBaseAddress + TBT2PCIE_R));
    DEBUG ((DEBUG_INFO, "Data read = 0x%lx \n", REG_VAL));

    if (0xFFFFFFFF == REG_VAL) {
      //
      // Device is not here return now
      //
      DEBUG ((DEBUG_INFO, "TBT device is not present \n"));
      RetCode = 0x7F;
      break;
    }

    if (!(REG_VAL & TBT2PCIE_DON_R)) {
      RetCode = 0x71;
      break;
    }

    gBS->Stall (100 * 1000);
  }

  IoWrite8 (0x80, RetCode);

  return ret;
}

// AMI_OVERRIDE_START - Add for updating the PCIe ClkReqNum.
/**
  This function will update the right TBT clkreq number by Token, TBT_CLKREQ_NUM_UPDATE.

  @param  SetupData             Pointer to SETUP_DATA
  @param  IndexPort             TBT in which PCIe index port
  @param  MaxRootPortNum        PCH max PCIe root ports

  @return                       No return value
**/
VOID UpdatePchPCIeClkReqNum (
  IN  SETUP_DATA                *SetupData,
  IN  UINT8                     IndexPort,
  IN  UINT8                     MaxRootPortNum
  )
{
  UINT8                         TbtClkReqNum[20] = TBT_CLKREQ_NUM_UPDATE;
  UINT8                         Index;

  // Check PCIe Index port is PCH, or not.
  if (IndexPort > 19)
    return;

  // Setting ClkReqNum if customer need to update.
  if (TbtClkReqNum[IndexPort] != 0xFF) {
    SetupData->PchPcieClkReqOverride[IndexPort] = 2;
    SetupData->PchPcieClkReqNumber[IndexPort] = TbtClkReqNum[IndexPort];
  }

  // Reset other SB PCIE root port ClkReqNum setting
  for (Index=0 ; Index < MaxRootPortNum ; Index++) {
    if ((TbtClkReqNum[Index] != 0xFF) && (SetupData->PchPcieClkReqOverride[Index] == 1) && (Index != IndexPort)) {
      SetupData->PchPcieClkReqOverride[Index] = 0;
      SetupData->PchPcieClkReqNumber[Index] = 0;
    }
  }
}
// AMI_OVERRIDE_END - Add for updating the PCIe ClkReqNum.

/**
  Thunderbolt setup page callback function.

  @param  CallbackValues        Poniter to the structure of AMI_HII_CALLBACK_PARAMETERS

  @retval EFI_SUCCESS
  @retval others
**/
EFI_STATUS
EFIAPI
TbtFormCallBackFunction (
// AMI_OVERRIDE_START - For AMI callback function
#if 0
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else
  AMI_HII_CALLBACK_PARAMETERS   *CallbackValues
#endif
// AMI_OVERRIDE_END - For AMI callback function
  )
{
  SETUP_DATA                    *SetupData;
  UINTN                         VarSize;
  EFI_STATUS                    Status;
  volatile EFI_IFR_TYPE_VALUE   lValue;
  BOOLEAN                       SaveOnExit;
  EFI_STRING                    RequestString;
  EFI_STRING                    SaRequestString;
  EFI_STRING                    PchRequestString;
  UINT8                         RootPort;
  UINT8                         Rootportselected;
// AMI_OVERRIDE_STARRT - For AMI callback function
  EFI_BROWSER_ACTION            Action;
  EFI_QUESTION_ID               KeyValue;
  EFI_IFR_TYPE_VALUE            *Value;
  EFI_BROWSER_ACTION_REQUEST    *ActionRequest;
// AMI_OVERRIDE_END - For AMI callback function
// AMI_OVERRIDE_START - Add for updating the PCIe ClkReqNum.
  PCH_SERIES                    PchSeries = GetPchSeries ();
  // AMI_OVERRIDE_END - Add for updating the PCIe ClkReqNum.
// AMI_OVERRIDE_START - For AMI callback function
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
  Value = CallbackValues->Value;
  ActionRequest = CallbackValues->ActionRequest;
// AMI_OVERRIDE_END - For AMI callback function

  Status                  = EFI_SUCCESS;
  SaveOnExit              = FALSE;
  RequestString           = NULL;
  SaRequestString         = NULL;
  PchRequestString        = NULL;
  Rootportselected        = 0;

  DEBUG ((DEBUG_INFO, "TbtFormCallBackFunction \n"));

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &gTbtInfoHob);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Searching Thunderbolt(TM) information Hob... \n"));
    gTbtInfoHob = GetNextGuidHob (&gTbtInfoHobGuid, gTbtInfoHob);
  }

  if (gTbtInfoHob == NULL) {
    DEBUG ((DEBUG_INFO, "Thunderbolt(TM) information Hob is NULL \n"));
    return EFI_NOT_FOUND;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED)
    return EFI_UNSUPPORTED;

  if (Action == EFI_BROWSER_ACTION_CHANGING)
    return EFI_SUCCESS;

  if ((Value == NULL) || (ActionRequest == NULL))
    return EFI_INVALID_PARAMETER;

  VarSize = sizeof (SETUP_DATA);
  SetupData = AllocatePool (VarSize);
  ASSERT (SetupData != NULL);
  if (SetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = HiiGetBrowserData (&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) SetupData);
  ASSERT_EFI_ERROR(Status);



  IoWrite16(0x80, (UINT16)KeyValue);

  lValue = *Value;

  DEBUG ((DEBUG_INFO, "Thunderbolt(TM) call back actions begin \n"));

  switch (KeyValue) {
    case KEY_TBT_SUPPORT:
      DEBUG ((DEBUG_INFO, "KEY_TBT_SUPPORT CallBack \n"));
      if(SetupData->TbtSupport == 1) {
        if (SetupData->Win10Support == 0) { // Don't Change PciExpNative Value if TBT Win10 support is enable.
          SetupData->PciExpNative = 0;
        }
        if (SetupData->Rtd3Tbt == 0) { // Don't Change RTD3 Support Value if TBT Rtd3 support is enable.
          SetupData->Rtd3Support = 0;
        }

// AMI_OVERRIDE_START - Set the MMIO resource default for OEM when enabling TbtSupport.
#if 0
        SaSetup->MaxTolud = 1;
#else
        if(SetupData->TBThostRouter == 2) {
          SetupData->MaxTolud = TBT_DEFAULT_TOLUD_TWO_PORT;
        } else {
          SetupData->MaxTolud = TBT_DEFAULT_TOLUD_ONE_PORT;
        }
#endif
// AMI_OVERRIDE_END - Set the MMIO resource default for OEM when enabling TbtSupport.
        SetupData->TbtPcieSupport = 0;
//
// KblGBegin
//
        // If KblG Package and TBT is enabled in Legacy enum mode (NO_OS_NATIVE_SUPPORT, Win10Support == 0),
        // Then disable Peg0 LTR, so that the package is not stuck in C2 state
        if (IsKblGPackage()) {
          if (SetupData->Win10Support == 0) {
//AMI_OVERRIDE_START - follow AMI setup style
            SetupData->Peg0LtrEnable = 0;
//AMI_OVERRIDE_END - follow AMI setup style
          }
        }
//
// KblGEnd
//
// AMI_OVERRIDE_END - Control by CSM module.
#if 0

        if (SetupData->PciExpNative != 0 && SetupData->Win10Support != 0) { // Disable and Grayout CSM if Native PCIE support is not Disable.
          //
          // Disable CSM if CSM is enable
          //
          if (SetupData->CsmControl != EFI_CSM_CONTROL_ALWAYS_OFF) {
            SetupData->CsmControl = EFI_CSM_CONTROL_ALWAYS_OFF;
            RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, CsmControl), sizeof (SetupData->CsmControl));
          }
        }
#endif
// AMI_OVERRIDE_END - Control by CSM module.
      } else {
        SetupData->PciExpNative = 1;
        SetupData->MaxTolud = 0;
        SetupData->Rtd3Support = 1;
        SetupData->TbtPcieSupport = 1;
//
// KblGBegin
//
        // If KblG Package and TBT is disabled, enable Peg0 LTR
        if (IsKblGPackage()) {
//AMI_OVERRIDE_START - follow AMI setup style
          SetupData->Peg0LtrEnable = 1;
//AMI_OVERRIDE_END - follow AMI setup style
        }
//
// KblGEnd
//
      }
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PciExpNative), sizeof (SetupData->PciExpNative));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, MaxTolud), sizeof (SetupData->MaxTolud));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Rtd3Support), sizeof (SetupData->Rtd3Support));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtPcieSupport), sizeof (SetupData->TbtPcieSupport));
//AMI_OVERRIDE_START - follow AMI setup style
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg0LtrEnable), sizeof (SetupData->Peg0LtrEnable));
//AMI_OVERRIDE_END - follow AMI setup style
    case KEY_TBT_BOOT_ON:
    case KEY_TBT_USB_ON:
      if ((SetupData->TbtBootOn == 1) || (SetupData->TbtUsbOn == 1)) {
        SetupData->CallSmiBeforeBoot = 0;
//AMI_OVERRIDE_START - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
//        SetupData->TbtSkipPciOprom = 0;
//AMI_OVERRIDE_END - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
        SetupData->AssignIo = 1;
// AMI_OVERRIDE_END - Support Legacy TBT device boot.

//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
        SetupData->TbtPcieIoRsvd = TBT_PCIE_IO_RESERVED_WHEN_BOOT_ON_SUPPORT;
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
      } else {
        SetupData->CallSmiBeforeBoot = 3;
//AMI_OVERRIDE_START - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
//        SetupData->TbtSkipPciOprom = 1;
//AMI_OVERRIDE_END - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
// AMI_OVERRIDE_START - Support Legacy TBT device boot.
        SetupData->AssignIo = 0;
// AMI_OVERRIDE_END - Support Legacy TBT device boot.
// AMI_OVERRIDE_START - Following CTS guide, Io resource should not be presented when TBT / USB boot are disabled.
        SetupData->TbtPcieIoRsvd = TBT_DEFAULT_PCIE_IO_RESERVED;
// AMI_OVERRIDE_END - Following CTS guide, Io resource should not be presented when TBT / USB boot are disabled.
      }
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, CallSmiBeforeBoot), sizeof (SetupData->CallSmiBeforeBoot));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtSkipPciOprom), sizeof (SetupData->TbtSkipPciOprom));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, AssignIo), sizeof (SetupData->AssignIo));  // AMI_OVERRIDE_TBT0007 - Support Legacy TBT device boot.
// AMI_OVERRIDE_START - Setting SetupData->TbtSelector by AMI setup option.
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtPcieIoRsvd), sizeof (SetupData->TbtPcieIoRsvd));
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
#if 0
    case KEY_TBT_SELECTOR_RVP3:
    case KEY_TBT_SELECTOR_RVP7:
    case KEY_TBT_SELECTOR_SDS:
    case KEY_TBT_SELECTOR_DT_HALO:
      DEBUG ((DEBUG_INFO, " KEY_TBT_SELECTOR_RVP3 / KEY_TBT_SELECTOR_RVP7 / KEY_TBT_SELECTOR_SDS CallBack / KEY_TBT_SELECTOR_DT / KEY_TBT_SELECTOR_HALO \n"));
#endif
    case KEY_TBT_LOCATION_GROUP:
    case KEY_TBT_LOCATION_SB:
    case KEY_TBT_LOCATION_NB:
      switch(SetupData->TbtHostLocationGroup){
        case 0x00:
          DEBUG ((DEBUG_INFO, "TBT Browser action : Change TBT host to SB PCIE slot\n"));
          SetupData->TbtSelector = SetupData->TbtHostTempLocationSB;
          break;
        case 0x01:
          DEBUG ((DEBUG_INFO, "TBT Browser action : Change TBT host to NB PCIE slot\n"));
          SetupData->TbtSelector = SetupData->TbtHostTempLocationNB;
          break;
      }
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtSelector), sizeof (SetupData->TbtSelector));
      DEBUG ((DEBUG_INFO, "TBT Browser action : New Location:%x of Location Group %x\n", SetupData->TbtSelector, SetupData->TbtHostLocationGroup));
// AMI_OVERRIDE_END - Setting SetupData->TbtSelector by AMI setup option.
// AMI_OVERRIDE_START - Add for updating the PCIe ClkReqNum.
      if (PchSeries == PchH)
        UpdatePchPCIeClkReqNum(SetupData, (SetupData->TbtSelector - 1), PCH_MAX_PCIE_ROOT_PORTS);
      else if (PchSeries == PchLp)
        UpdatePchPCIeClkReqNum(SetupData, (SetupData->TbtSelector - 1), PCH_LP_PCIE_MAX_ROOT_PORTS);
// AMI_OVERRIDE_END - Add for updating the PCIe ClkReqNum.
      //
      // Check all the possible conditions for TBT Root port Selector [AUTO, PCH, PEG etc] to enable Hot Plug Capability for the TBT Port
      // Most of the existing boards are using PCH Slots only for TBT implementation
      // Care needs to be taken while adding PEG Slot capability or other capability for TBT implementation
      //
      if(SetupData->TbtSelector == AUTO) {
        Rootportselected = (gTbtInfoHob->TbtDefaultSelectorInfo - 1);
      } else {
        Rootportselected = (SetupData->TbtSelector - 1);
      }
// AMI_OVERRIDE_START - The resource for PEG Thunderbolt host is incorrect
      for(RootPort= 0; RootPort < PCH_PCIE_NUM; ++RootPort ) {
        if(gTbtInfoHob->TbtSupportedPortsInfo & (1<<RootPort)) {
          //
          //if PCH-Rootport is supported for TBT, check for the Port which is selected
          //
          if(SetupData->TbtSupport == 1) {
            SetupData->PcieRootPortHPE[RootPort] =  ((Rootportselected == RootPort)? 1:0);
          } else {
            SetupData->PcieRootPortHPE[RootPort] =  0;
          }
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PcieRootPortHPE[RootPort]), sizeof (SetupData->PcieRootPortHPE[RootPort]));
        }
      }

//AMI_OVERRIDE_START - EIP300653 Set PEG port to Auto as the default
      if (SetupData->TbtSupport == 1) {
        if ((SetupData->TbtSelector == PCH_PCIE_NUM+1) || (SetupData->TbtSelector == 0 && gTbtInfoHob->TbtDefaultSelectorInfo == PCH_PCIE_NUM+1)) {
          SetupData->Peg0Enable = 1;
          SetupData->Peg1Enable = 2;
          SetupData->Peg2Enable = 2;
          SetupData->PegMaxPayload[0] = 0;
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PegMaxPayload[0]), sizeof (SetupData->PegMaxPayload[0]));
        } else if ((SetupData->TbtSelector == PCH_PCIE_NUM+2) || (SetupData->TbtSelector == 0 && gTbtInfoHob->TbtDefaultSelectorInfo == PCH_PCIE_NUM+2)) {
          SetupData->Peg0Enable = 2;
          SetupData->Peg1Enable = 1;
          SetupData->Peg2Enable = 2;
          SetupData->PegMaxPayload[1] = 0;
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PegMaxPayload[1]), sizeof (SetupData->PegMaxPayload[1]));
        } else if ((SetupData->TbtSelector == PCH_PCIE_NUM+3) || (SetupData->TbtSelector == 0 && gTbtInfoHob->TbtDefaultSelectorInfo == PCH_PCIE_NUM+3)) {
          SetupData->Peg0Enable = 2;
          SetupData->Peg1Enable = 2;
          SetupData->Peg2Enable = 1;
          SetupData->PegMaxPayload[2] = 0;
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PegMaxPayload[2]), sizeof (SetupData->PegMaxPayload[2]));
        }
      } else {
          SetupData->Peg0Enable = 2;
          SetupData->Peg1Enable = 2;
          SetupData->Peg2Enable = 2;
      }
//AMI_OVERRIDE_END - EIP300653 Set PEG port to Auto as the default
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg0Enable), sizeof (SetupData->Peg0Enable));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg1Enable), sizeof (SetupData->Peg1Enable));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg2Enable), sizeof (SetupData->Peg2Enable));

      for (RootPort = PCH_PCIE_NUM; RootPort < (PCH_PCIE_NUM + PEG_NUM); ++RootPort) {
        if(gTbtInfoHob->TbtSupportedPortsInfo & (1<<RootPort)) {
          //
          //if PEG-Rootport is supported for TBT, check for the Port which is selected
          //
          if(SetupData->TbtSupport == 1) {
            SetupData->PegRootPortHPE[RootPort-PCH_PCIE_NUM] =  ((Rootportselected == RootPort) ? 1:0);
          } else {
            SetupData->PegRootPortHPE[RootPort-PCH_PCIE_NUM] =  0;
          }
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PegRootPortHPE[RootPort-PCH_PCIE_NUM]), sizeof (SetupData->PegRootPortHPE[RootPort-PCH_PCIE_NUM]));
        }
      }

      if (SetupData->TbtSupport == 1) {
        if ((SetupData->TbtSelector == PCH_PCIE_NUM+1) || (SetupData->TbtSelector == 0 && gTbtInfoHob->TbtDefaultSelectorInfo == PCH_PCIE_NUM+1)) {
          SetupData->Peg0PowerDownUnusedLanes = 0;
        } else {
          SetupData->Peg0PowerDownUnusedLanes = 1;
        }
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg0PowerDownUnusedLanes), sizeof (SetupData->Peg0PowerDownUnusedLanes));

        if ((SetupData->TbtSelector == PCH_PCIE_NUM+2) || (SetupData->TbtSelector == 0 && gTbtInfoHob->TbtDefaultSelectorInfo == PCH_PCIE_NUM+2)) {
          SetupData->Peg1PowerDownUnusedLanes = 0;
        } else {
          SetupData->Peg1PowerDownUnusedLanes = 1;
        }
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg1PowerDownUnusedLanes), sizeof (SetupData->Peg1PowerDownUnusedLanes));

        if ((SetupData->TbtSelector == PCH_PCIE_NUM+3) || (SetupData->TbtSelector == 0 && gTbtInfoHob->TbtDefaultSelectorInfo == PCH_PCIE_NUM+3)) {
          SetupData->Peg2PowerDownUnusedLanes = 0;
        } else {
          SetupData->Peg2PowerDownUnusedLanes = 1;
        }
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg2PowerDownUnusedLanes), sizeof (SetupData->Peg2PowerDownUnusedLanes));
      }
    break;
// AMI_OVERRIDE_END - The resource for PEG Thunderbolt host is incorrect
    case KEY_TBT_HOSTROUTER:
      DEBUG ((DEBUG_INFO, "KEY_TBT_HOSTROUTER CallBack \n"));
      //
      //As per the Bios implementation Guide
      //(Table 2.3: Host Router resource allocation/reservation scheme) ,
      //we need to allocate enough  resources based on Host router type
      //one port or two port
      //
      if(SetupData->TBThostRouter == 2) {
        SetupData->TbtPcieExtraBusRsvd   = 106;
        SetupData->TbtPcieMemRsvd        = 737;
        SetupData->TbtPcieMemAddrRngMax  = 26;
        SetupData->TbtPciePMemRsvd       = 1184;
        SetupData->TbtPciePMemAddrRngMax = 28;
// AMI_OVERRIDE_START - Following CTS guide, Io resource should not be presented when TBT / USB boot are disabled.
        SetupData->TbtPcieIoRsvd         = TBT_DEFAULT_PCIE_IO_RESERVED;
// AMI_OVERRIDE_END - Following CTS guide, Io resource should not be presented when TBT / USB boot are disabled.
      } else {
        SetupData->TbtPcieExtraBusRsvd   = 56;
        SetupData->TbtPcieMemRsvd        = 353;
        SetupData->TbtPcieMemAddrRngMax  = 26;
        SetupData->TbtPciePMemRsvd       = 544;
        SetupData->TbtPciePMemAddrRngMax = 28;
// AMI_OVERRIDE_START - Following CTS guide, Io resource should not be presented when TBT / USB boot are disabled.
        SetupData->TbtPcieIoRsvd         = TBT_DEFAULT_PCIE_IO_RESERVED;
// AMI_OVERRIDE_END - Following CTS guide, Io resource should not be presented when TBT / USB boot are disabled.
      }
// AMI_OVERRIDE_START - Following CTS guide, Io resource should not be presented when TBT / USB boot are disabled.
      if ((SetupData->TbtBootOn == 1) || (SetupData->TbtUsbOn == 1)) {
//AMI_OVERRIDE_START - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
        SetupData->TbtPcieIoRsvd = TBT_PCIE_IO_RESERVED_WHEN_BOOT_ON_SUPPORT;
//AMI_OVERRIDE_END - EIP319788 - Set IO Reserved to 0, there's still IO resource presence if USB_ON or BOOT_ON is enabled
      }
// AMI_OVERRIDE_END - Following CTS guide, Io resource should not be presented when TBT / USB boot are disabled.
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtPcieExtraBusRsvd), sizeof (SetupData->TbtPcieExtraBusRsvd));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtPcieMemRsvd), sizeof (SetupData->TbtPcieMemRsvd));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtPcieMemAddrRngMax), sizeof (SetupData->TbtPcieMemAddrRngMax));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtPciePMemRsvd), sizeof (SetupData->TbtPciePMemRsvd));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtPciePMemAddrRngMax), sizeof (SetupData->TbtPciePMemAddrRngMax));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtPcieIoRsvd), sizeof (SetupData->TbtPcieIoRsvd));
    break;

    case TBT_SECURITY_LEVEL_KEY:
      DEBUG ((EFI_D_INFO, "TBT_SECURITY_LEVEL_KEY CallBack \n"));
      ForcePower(gTbtInfoHob->ForcePwrGpio.GpioAccessType, gTbtInfoHob->ForcePwrGpio.Expander, gTbtInfoHob->ForcePwrGpio.GpioNumber, gTbtInfoHob->ForcePwrGpio.GpioLevel);
      gBS->Stall (SetupData->Gpio3ForcePwrDly * 1000);
      if (SetSecLevel (lValue.u8, SetupData->TbtSelector, PCIE2TBT_SET_SECURITY_LEVEL) && (lValue.u8 == GetSecLevel (SetupData->TbtSelector, PCIE2TBT_GET_SECURITY_LEVEL))) {
        SetupData->SecurityMode = lValue.u8;
        SaveOnExit = TRUE;
        //If selected security level is SL3 (Display Port and USB), BIOS should disable "Boot from  Thunderbolt" and "Boot from USB/USB at boot"
        if (SetupData->SecurityMode == 3) {
          SetupData->TbtUsbOn = 0;
          SetupData->TbtBootOn = 0;
          SetupData->CallSmiBeforeBoot = 3;
//AMI_OVERRIDE_START - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
//          SetupData->TbtSkipPciOprom = 1;
//AMI_OVERRIDE_END - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtUsbOn), sizeof (SetupData->TbtUsbOn));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtBootOn), sizeof (SetupData->TbtBootOn));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, CallSmiBeforeBoot), sizeof (SetupData->CallSmiBeforeBoot));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TbtSkipPciOprom), sizeof (SetupData->TbtSkipPciOprom));
        }
      } else {
        SetupData->SecurityMode = 0;
      }

      if (SetupData->Gpio3ForcePwr == 0) {
        ForcePower(gTbtInfoHob->ForcePwrGpio.GpioAccessType, gTbtInfoHob->ForcePwrGpio.Expander, gTbtInfoHob->ForcePwrGpio.GpioNumber, !(gTbtInfoHob->ForcePwrGpio.GpioLevel));
        gBS->Stall (SetupData->Gpio3ForcePwrDly * 1000);
      }
    break;

//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
    case KEY_TBT_X_CONNECT_SUPPORT:
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
// AMI_OVERRIDE_START - Use AMI style setup data
/*
      if(SetupData->TbtDetachableGfxSupport == 1) {
        SaSetup->PrimaryDisplay = 0; // IGFX
      } else {
        SaSetup->PrimaryDisplay = 3; // Auto
      }
      SaRequestString = HiiConstructRequestString (SaRequestString, OFFSET_OF (SA_SETUP, PrimaryDisplay), sizeof (SaSetup->PrimaryDisplay));
*/
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
     if(SetupData->TbtXConnectSupport == 1) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
    	 SetupData->PrimaryDisplay = 0; // IGFX
      } else {
    	  SetupData->PrimaryDisplay = 3; // Auto
      }
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PrimaryDisplay), sizeof (SetupData->PrimaryDisplay));
// AMI_OVERRIDE_END - Use AMI style setup data

      break;

    default:
      return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "Thunderbolt(TM) call back - HiiSetBrowserData begin \n"));
  if (RequestString != NULL) {
    VarSize = sizeof(SETUP_DATA);
// AMI_OVERRIDE_START - For AMI callback function
    Status = HiiSetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) SetupData, NULL);
// AMI_OVERRIDE_END - For AMI callback function
    ASSERT_EFI_ERROR(Status);
    FreePool (RequestString);
  }

  DEBUG ((DEBUG_INFO, "Thunderbolt(TM) call back - HiiSetBrowserData end \n"));
  FreePool(SetupData);

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  if(SaveOnExit)
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_SUBMIT;

  DEBUG ((DEBUG_INFO, "Thunderbolt(TM) call back actions end \n"));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
TbtOsSelectorFormCallBackFunction (
// AMI_OVERRIDE_START >>> For AMI callback function
#if 0
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else
  AMI_HII_CALLBACK_PARAMETERS   *CallbackValues
#endif
// AMI_OVERRIDE_END <<< For AMI callback function
  )
{
// AMI_OVERRIDE_START >>> EIP358265
/*  SA_SETUP                   *SaSetup;
  PCH_SETUP                  *PchSetup;*/
// AMI_OVERRIDE_END <<< EIP358265
  SETUP_DATA                 *SetupData;
  EFI_STRING                  RequestString;
  EFI_STRING                  SaRequestString;
  EFI_STRING                  PchRequestString;
  UINTN                       VarSize;
  EFI_STATUS                  Status;
  BOOLEAN                     SaveOnExit;
  UINT16                      DeviceId;
  UINT32                      Timeout;
  UINTN                       RpDev;
  UINTN                       RpFunc;
  UINT8                       HoustRouteBus;
  UINT8                       TbtSelector;
//AMI_OVERRIDE_START >>> EIP362438: System delays 3~5 seconds before entering into UEFI Setup
//  UINT32                      RegVal;
//AMI_OVERRIDE_END <<< EIP362438: System delays 3~5 seconds before entering into UEFI Setup
  UINTN                       DeviceBaseAddress;
  volatile EFI_IFR_TYPE_VALUE lValue;
//AMI_OVERRIDE_START >>> EIP362438: System delays 3~5 seconds before entering into UEFI Setup
//  SETUP_VOLATILE_DATA        *SetupVolatileData;
//AMI_OVERRIDE_END <<< EIP362438: System delays 3~5 seconds before entering into UEFI Setup
  UINTN                       Index;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       HandleCount;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  UINT8                       SecBus;
  UINT8                       SubBus;
  UINTN                       BusNumber;
  UINTN                       DeviceNumber;
  UINTN                       FunctionNumber;
  UINTN                       SegmentNumber;
// AMI_OVERRIDE_STARRT - For AMI callback function
  EFI_BROWSER_ACTION            Action;
  EFI_QUESTION_ID               KeyValue;
  EFI_IFR_TYPE_VALUE            *Value;
  EFI_BROWSER_ACTION_REQUEST    *ActionRequest;
// AMI_OVERRIDE_END - For AMI callback function


// AMI_OVERRIDE_START - For AMI callback function
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
  Value = CallbackValues->Value;
  ActionRequest = CallbackValues->ActionRequest;
// AMI_OVERRIDE_END - For AMI callback function

  Timeout = 50;
  SaveOnExit = FALSE;
  Status = EFI_SUCCESS;
  RequestString = NULL;
  SaRequestString = NULL;
  PchRequestString = NULL;

  DEBUG ((DEBUG_INFO, "TbtOsSelectorFormCallBackFunction \n"));

//AMI_OVERRIDE_START >>> EIP405697 : System assert in debug mode when TBT is not fully disabled.
  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
//AMI_OVERRIDE_END <<< EIP405697 : System assert in debug mode when TBT is not fully disabled.

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &gTbtInfoHob);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Searching Thunderbolt(TM) information Hob... \n"));
    gTbtInfoHob = GetNextGuidHob (&gTbtInfoHobGuid, gTbtInfoHob);
  }

  if (gTbtInfoHob == NULL) {
    DEBUG ((DEBUG_ERROR, "Thunderbolt(TM) information Hob is NULL \n"));
    return EFI_NOT_FOUND;
  }

  VarSize = sizeof (SETUP_DATA);
  SetupData = AllocatePool (VarSize);
  ASSERT (SetupData != NULL);
  if (SetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = HiiGetBrowserData (&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) SetupData);
  ASSERT_EFI_ERROR(Status);

// AMI_OVERRIDE_START >>> EIP358265
/*  VarSize = sizeof (SA_SETUP);
  SaSetup = AllocatePool (VarSize);
  ASSERT (SaSetup != NULL);
  if (SaSetup == NULL) {
    FreePool (SetupData);
    return EFI_OUT_OF_RESOURCES;
  }
  Status = HiiGetBrowserData (&gSaSetupVariableGuid, L"SaSetup", VarSize, (UINT8 *) SaSetup);
  ASSERT_EFI_ERROR(Status);

  VarSize = sizeof (PCH_SETUP);
  PchSetup = AllocatePool (VarSize);
  ASSERT (PchSetup != NULL);
  if (PchSetup == NULL) {
    FreePool (SaSetup);
    FreePool (SetupData);
    return EFI_OUT_OF_RESOURCES;
  }
  Status = HiiGetBrowserData (&gPchSetupVariableGuid, L"PchSetup", VarSize, (UINT8 *) PchSetup);
  ASSERT_EFI_ERROR(Status);*/
// AMI_OVERRIDE_END <<< EIP358265

//AMI_OVERRIDE_START >>> EIP362438: System delays 3~5 seconds before entering into UEFI Setup
//  VarSize = sizeof (SETUP_VOLATILE_DATA);
//  SetupVolatileData = AllocatePool (VarSize);
//  ASSERT (SetupVolatileData != NULL);
//  if ( SetupVolatileData == NULL ) {
// AMI_OVERRIDE_START >>> EIP358265
/*    FreePool (SaSetup);
    FreePool (PchSetup);*/
// AMI_OVERRIDE_END <<< EIP358265
//    FreePool (SetupData);
//    return EFI_OUT_OF_RESOURCES;
//  }
//  Status = HiiGetBrowserData (&gSetupVariableGuid, L"SetupVolatileData", VarSize, (UINT8 *) SetupVolatileData);
//  ASSERT_EFI_ERROR(Status);
//AMI_OVERRIDE_END <<< EIP362438: System delays 3~5 seconds before entering into UEFI Setup
  lValue.u8 = 0;
  if (Value != NULL) {
    lValue = *Value;
  }

  if (SetupData->TbtSelector == AUTO) {
    TbtSelector = gTbtInfoHob->TbtDefaultSelectorInfo;
  } else {
    TbtSelector = SetupData->TbtSelector;
  }

  Status = GetTbtRpDevFun (TbtSelector - 1, &RpDev, &RpFunc);
  ASSERT_EFI_ERROR (Status);
  DeviceBaseAddress = MmPciBase (0x00, (UINT32)RpDev, (UINT32)RpFunc);
  HoustRouteBus = MmioRead8 (DeviceBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  SecBus = MmioRead8 (DeviceBaseAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  SubBus = MmioRead8 (DeviceBaseAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
//AMI_OVERRIDE_START >>> EIP362438: System delays 3~5 seconds before entering into UEFI Setup
/*
  if (!mTbtOsFormEntry) {
    DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: First time initialization. \n"));
    ForcePower(gTbtInfoHob->ForcePwrGpio.GpioAccessType, gTbtInfoHob->ForcePwrGpio.Expander, gTbtInfoHob->ForcePwrGpio.GpioNumber, gTbtInfoHob->ForcePwrGpio.GpioLevel);
    gBS->Stall (SetupData->Gpio3ForcePwrDly * 1000);

    DeviceId= MmioRead16 (MmPciBase (HoustRouteBus, 0, 0) + PCI_DEVICE_ID_OFFSET);
    for (Timeout = 0 ; Timeout < 500 ; Timeout++) {
      if ( MmioRead16 (MmPciBase (HoustRouteBus, 0, 0) + PCI_COMMAND_OFFSET) != 0xFFFF) {
        break;
      }
      gBS->Stall (1000);
    }
    TbtSetPCIe2TbtCommand (PCIE2TBT_GET_ENUMERATION_METHOD, HoustRouteBus);

    RegVal = (MmioRead32(MmPciBase(HoustRouteBus, 0x00, 0x00) + TBT2PCIE_R) & 0xFFFFFF00) >> 8; // TBT2PCI_R Bit 8-31 are valide for status.
    DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: PCIE2TBT_GET_ENUMERATION_METHOD result is %x.\n", RegVal));

    switch (RegVal) {

      case (NO_OS_NATIVE_SUPPORT) :  // No Windows Native support
        SetupData->Win10Support = 0;
        SetupData->Rtd3Tbt = 0;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Win10Support), sizeof (SetupData->Win10Support));
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Rtd3Tbt), sizeof (SetupData->Rtd3Tbt));
        SetupVolatileData->TbtFwSupportedFeature = 0;
        DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: No OS Native Support\n"));
        DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: Tbt FW Supported Feature = %x \n", SetupVolatileData->TbtFwSupportedFeature));
//
// KblGBegin
//
        // If KblG Package and TBT is enabled in Legacy enum mode (NO_OS_NATIVE_SUPPORT),
        // Then disable Peg0 LTR, so that the package is not stuck in C2 state
        if (IsKblGPackage()) {
          SaSetup->Peg0LtrEnable = 0;
          SaRequestString = HiiConstructRequestString (SaRequestString, OFFSET_OF (SA_SETUP, Peg0LtrEnable), sizeof (SaSetup->Peg0LtrEnable));
        }
//
// KblGEnd
//
        break;

      case (OS_NATIVE_SUPPORT_ONLY) :  // Windows Native support
        SetupData->Win10Support = 1;
        SetupData->Rtd3Tbt = 0;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Win10Support), sizeof (SetupData->Win10Support));
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Rtd3Tbt), sizeof (SetupData->Rtd3Tbt));
        SetupVolatileData->TbtFwSupportedFeature = 1;
        DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: OS Native Support Enable\n"));
        DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: Tbt FW Supported Feature = %x \n", SetupVolatileData->TbtFwSupportedFeature));
//
// KblGBegin
//
        // If KblG Package and TBT is enabled in Native enum mode (OS_NATIVE_SUPPORT_ONLY),
        // Then enable Peg0 LTR
        if (IsKblGPackage()) {
          SaSetup->Peg0LtrEnable = 1;
          SaRequestString = HiiConstructRequestString (SaRequestString, OFFSET_OF (SA_SETUP, Peg0LtrEnable), sizeof (SaSetup->Peg0LtrEnable));
        }
//
// KblGEnd
//
        break;

      case (OS_NATIVE_SUPPORT_RTD3) :  // Windows Native support + TBT RTD3
        SetupData->Win10Support = 2;
        SetupData->Rtd3Tbt = 1;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Win10Support), sizeof (SetupData->Win10Support));
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Rtd3Tbt), sizeof (SetupData->Rtd3Tbt));
        SetupVolatileData->TbtFwSupportedFeature = 2;
        DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: OS Native Support + RTD3 Enable\n"));
        DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: Tbt FW Supported Feature = %x \n", SetupVolatileData->TbtFwSupportedFeature));
//
// KblGBegin
//
        // If KblG Package and TBT is enabled in Native enum with RTD3 support (OS_NATIVE_SUPPORT_RTD3),
        // Then enable Peg0 LTR
        if (IsKblGPackage()) {
          SaSetup->Peg0LtrEnable = 1;
          SaRequestString = HiiConstructRequestString (SaRequestString, OFFSET_OF (SA_SETUP, Peg0LtrEnable), sizeof (SaSetup->Peg0LtrEnable));
        }
//
// KblGEnd
//
        break;

      default :
        SetupData->Win10Support = 0;
        SetupData->Rtd3Tbt = 0;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Win10Support), sizeof (SetupData->Win10Support));
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Rtd3Tbt), sizeof (SetupData->Rtd3Tbt));
        SetupVolatileData->TbtFwSupportedFeature = 0;
        DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: Unknown\n"));
        DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: Tbt FW Supported Feature = %x \n", SetupVolatileData->TbtFwSupportedFeature));
    } // End of switch (RegVal)
    mTbtOsFormEntry = TRUE;

    DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: Saving Volatile Setup variable\n"));
    DEBUG ((DEBUG_INFO, "TBT_OSSELECT_FORM_ID CallBack: Tbt FW Supported Feature before saving = %x \n", SetupVolatileData->TbtFwSupportedFeature));
    VarSize = sizeof(SETUP_VOLATILE_DATA);
    Status = HiiSetBrowserData (&gSetupVariableGuid, L"SetupVolatileData", VarSize, (UINT8 *) SetupVolatileData, NULL);
    ASSERT_EFI_ERROR(Status);
    VarSize = sizeof(SA_SETUP);
    Status = HiiSetBrowserData (&gSaSetupVariableGuid, L"SaSetup", VarSize, (UINT8 *) SaSetup, SaRequestString);
    ASSERT_EFI_ERROR(Status);
  }
  FreePool (SetupVolatileData);
  if (SetupData->Gpio3ForcePwr == 0) {
    ForcePower(gTbtInfoHob->ForcePwrGpio.GpioAccessType, gTbtInfoHob->ForcePwrGpio.Expander, gTbtInfoHob->ForcePwrGpio.GpioNumber, !(gTbtInfoHob->ForcePwrGpio.GpioLevel));
    gBS->Stall (SetupData->Gpio3ForcePwrDly * 1000);
  }
*/
//AMI_OVERRIDE_END <<< EIP362438: System delays 3~5 seconds before entering into UEFI Setup

//AMI_OVERRIDE_START >>> EIP405697 : System assert in debug mode when TBT is not fully disabled.
//  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
//// AMI_OVERRIDE_START >>> EIP358265
///*    FreePool (SaSetup);
//    FreePool (PchSetup);*/
//// AMI_OVERRIDE_END <<< EIP358265
//    FreePool (SetupData);
//    return EFI_UNSUPPORTED;
//  }
//
//  if (Action == EFI_BROWSER_ACTION_CHANGING) {
//// AMI_OVERRIDE_START >>> EIP358265
///*    FreePool (SaSetup);
//    FreePool (PchSetup);*/
//// AMI_OVERRIDE_END <<< EIP358265
//    FreePool (SetupData);
//    return EFI_SUCCESS;
//  }
//
//  if ((Value == NULL) || (ActionRequest == NULL)) {
//// AMI_OVERRIDE_START >>> EIP358265
///*    FreePool (SaSetup);
//    FreePool (PchSetup);*/
//// AMI_OVERRIDE_END <<< EIP358265
//    FreePool (SetupData);
//    return EFI_INVALID_PARAMETER;
//  }
//AMI_OVERRIDE_END <<< EIP405697 : System assert in debug mode when TBT is not fully disabled.

  switch (KeyValue) {
    case KEY_TBT_OS_SELECTOR:
      DEBUG ((DEBUG_INFO, "KEY_TBT_OS_SELECTOR CallBack: OS selector key handling. \n"));
      ForcePower (gTbtInfoHob->ForcePwrGpio.GpioAccessType, gTbtInfoHob->ForcePwrGpio.Expander, gTbtInfoHob->ForcePwrGpio.GpioNumber, gTbtInfoHob->ForcePwrGpio.GpioLevel);
      gBS->Stall (SetupData->Gpio3ForcePwrDly * 1000);
      for (Timeout = 0 ; Timeout < 500 ; Timeout++) {
        if (0xFFFF != MmioRead16 (MmPciBase (HoustRouteBus, 0, 0) + PCI_COMMAND_OFFSET)) {
          break;
        }
        gBS->Stall (1000);
      }
      //
      // Disconnect all
      //
       gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiPciIoProtocolGuid,
         NULL,
         &HandleCount,
         &HandleBuffer
         );

       for (Index = 0; Index < HandleCount; Index++) {
         Status = gBS->HandleProtocol( HandleBuffer[Index],
                                       &gEfiPciIoProtocolGuid,
                                       (VOID **) &PciIo );
         if (EFI_ERROR(Status)) {
           DEBUG ((DEBUG_INFO, "DisconnectTBTControllers: Cannot HandleProtocol %X %d\n", HandleBuffer[Index], Index));
           continue;
         }
         Status = PciIo->GetLocation( PciIo, &SegmentNumber, &BusNumber, &DeviceNumber, &FunctionNumber );
         DEBUG ((DEBUG_INFO,"Handle[%x]->B:%x|D:%x|F:%x\n", Index, BusNumber, DeviceNumber, FunctionNumber));
         if(SecBus <= BusNumber && BusNumber <= SubBus) {
           DEBUG ((DEBUG_INFO, "DisconnectTBTControllers: Handle[%x]->B:%x|D:%x|F:%x\n", Index, BusNumber, DeviceNumber, FunctionNumber));
           gBS->DisconnectController (HandleBuffer[Index], NULL, NULL);
         }
       }
      if (SetSecLevel (lValue.u8, SetupData->TbtSelector, PCIE2TBT_SET_ENUMERATION_METHOD)) {
        SetupData->Win10Support = lValue.u8;
        TbtSetPCIe2TbtCommand (PCIE2TBT_POWER_CYCLE, HoustRouteBus);
        for (Timeout = 0 ; Timeout < 500 ; Timeout++) {
          if (0xFFFF != MmioRead16 (MmPciBase (HoustRouteBus, 0, 0) + PCI_COMMAND_OFFSET)) {
            //
            // Device is here, exit the timeout loop.
            //
            DEBUG ((DEBUG_INFO, "KEY_TBT_OS_SELECTOR CallBack. Found TBT HR before timeout. \n"));
            break;
          }
          gBS->Stall (1000);
        }
      }
      if ( Timeout == 500) {
        DEBUG ((DEBUG_ERROR, "KEY_TBT_OS_SELECTOR CallBack. Cannot find TBT HR. \n"));
      }
      DeviceId= MmioRead16 (MmPciBase (HoustRouteBus, 0, 0) + PCI_DEVICE_ID_OFFSET);

      SetupData->Rtd3Tbt = 0;
      switch (SetupData->Win10Support) {

        case (OS_NATIVE_SUPPORT_RTD3) :
          SetupData->Rtd3Tbt = 1;
          // Fall through to OS_NATIVE_SUPPORT_ONLY. Don't put break.

        case (OS_NATIVE_SUPPORT_ONLY) :
          SetupData->TBTHotSMI = 0;
          SetupData->PciExpNative = 1;
          SetupData->Gpio5Filter = 0;
          SetupData->TBTSetClkReq = 1;
          SetupData->NativeAspmEnable = 2;
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TBTHotSMI), sizeof (SetupData->TBTHotSMI));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PciExpNative), sizeof (SetupData->PciExpNative));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Gpio5Filter), sizeof (SetupData->Gpio5Filter));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TBTSetClkReq), sizeof (SetupData->TBTSetClkReq));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, NativeAspmEnable), sizeof (SetupData->NativeAspmEnable));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Rtd3Tbt), sizeof (SetupData->Rtd3Tbt));
          SaveOnExit = TRUE;
//
// KblGBegin
//
        // If KblG Package and TBT is enabled in Native enum (with/without RTD3 Support)
        // (OS_NATIVE_SUPPORT_RTD3/OS_NATIVE_SUPPORT_ONLY), then enable Peg0 LTR
        if (IsKblGPackage()) {
//AMI_OVERRIDE_START - follow AMI setup style
          SetupData->Peg0LtrEnable = 1;
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg0LtrEnable), sizeof (SetupData->Peg0LtrEnable));
//AMI_OVERRIDE_END - follow AMI setup style
        }
//
// KblGEnd
//
          break;

        case (NO_OS_NATIVE_SUPPORT) :
          SaveOnExit = TRUE;
          SetupData->TBTHotSMI = 1;
          SetupData->PciExpNative = 0;
          SetupData->Gpio5Filter = 1;
          SetupData->TBTSetClkReq = 0;
          SetupData->NativeAspmEnable = 0;
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TBTHotSMI), sizeof (SetupData->TBTHotSMI));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, PciExpNative), sizeof (SetupData->PciExpNative));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Gpio5Filter), sizeof (SetupData->Gpio5Filter));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, TBTSetClkReq), sizeof (SetupData->TBTSetClkReq));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, NativeAspmEnable), sizeof (SetupData->NativeAspmEnable));
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Rtd3Tbt), sizeof (SetupData->Rtd3Tbt));
//
// KblGBegin
//
        // If KblG Package and TBT is enabled in Legacy enum mode (NO_OS_NATIVE_SUPPORT),
        // Then disable Peg0 LTR, so that the package is not stuck in C2 state
        if (IsKblGPackage()) {
//AMI_OVERRIDE_START - follow AMI setup style
          SetupData->Peg0LtrEnable = 0;
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, Peg0LtrEnable), sizeof (SetupData->Peg0LtrEnable));
//AMI_OVERRIDE_END - follow AMI setup style
        }
//
// KblGEnd
//
          break;

       default:
          SetupData->Win10Support = 0;
          SaveOnExit = FALSE;
      }
      break;

    default:
      if (SetupData->Gpio3ForcePwr == 0) {
        ForcePower(gTbtInfoHob->ForcePwrGpio.GpioAccessType, gTbtInfoHob->ForcePwrGpio.Expander, gTbtInfoHob->ForcePwrGpio.GpioNumber, !(gTbtInfoHob->ForcePwrGpio.GpioLevel));
        gBS->Stall (SetupData->Gpio3ForcePwrDly * 1000);
      }
// AMI_OVERRIDE_START >>> EIP358265
/*      FreePool (SaSetup);
      FreePool (PchSetup);*/
// AMI_OVERRIDE_END <<< EIP358265
      FreePool (SetupData);
      return EFI_SUCCESS;
  }

  if (SetupData->Gpio3ForcePwr == 0) {
    ForcePower(gTbtInfoHob->ForcePwrGpio.GpioAccessType, gTbtInfoHob->ForcePwrGpio.Expander, gTbtInfoHob->ForcePwrGpio.GpioNumber, !(gTbtInfoHob->ForcePwrGpio.GpioLevel));
    gBS->Stall (SetupData->Gpio3ForcePwrDly * 1000);
  }

// AMI_OVERRIDE_START - Control by CSM module.
#if 0
  if (SetupData->PciExpNative != 0 && SetupData->Win10Support != 0) { // Disable and Grayout CSM if Native PCIE support is not Disable.
    //
    // Disable CSM if CSM is enable
    //
    if (SetupData->CsmControl != EFI_CSM_CONTROL_ALWAYS_OFF) {
      SetupData->CsmControl = EFI_CSM_CONTROL_ALWAYS_OFF;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, CsmControl), sizeof (SetupData->CsmControl));
    }
  }

#endif
// AMI_OVERRIDE_END - Control by CSM module.
  if (RequestString != NULL) {
    VarSize = sizeof(SETUP_DATA);
    Status = HiiSetBrowserData (&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) SetupData, NULL); // AMI_OVERRIDE_START >>> EIP358265
    ASSERT_EFI_ERROR(Status);
    FreePool (RequestString);
  }
// AMI_OVERRIDE_START >>> EIP358265
/*  if (SaRequestString != NULL) {
    VarSize = sizeof(SA_SETUP);
    Status = HiiSetBrowserData (&gSaSetupVariableGuid, L"SaSetup", VarSize, (UINT8 *) SaSetup, SaRequestString);
    ASSERT_EFI_ERROR(Status);
    FreePool (SaRequestString);
  }
  if (PchRequestString != NULL) {
    VarSize = sizeof(PCH_SETUP);
    Status = HiiSetBrowserData (&gPchSetupVariableGuid, L"PchSetup", VarSize, (UINT8 *) PchSetup, PchRequestString);
    ASSERT_EFI_ERROR (Status);
    FreePool (PchRequestString);
  }*/
// AMI_OVERRIDE_END <<< EIP358265
  DEBUG ((DEBUG_INFO, "Thunderbolt(TM) call back - HiiSetBrowserData end \n"));
// AMI_OVERRIDE_START >>> EIP358265
/*  FreePool (SaSetup);
  FreePool (PchSetup);*/
// AMI_OVERRIDE_END <<< EIP358265
  FreePool (SetupData);

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  if(SaveOnExit)
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_SUBMIT;

  DEBUG ((DEBUG_INFO, "Thunderbolt(TM) call back actions end \n"));

  return EFI_SUCCESS;
}

/**
  Get uncommitted Vtd  Browser status from BIOS setup menu to check Vtd availability

  @retval EFI_SUCCESS           Vt-d is available.
  @retval EFI_UNSUPPORTED       Vt-d is not available.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to get buffer.
**/
EFI_STATUS
GetVtdStatusfromBrowser (
  VOID
  )
{
  SETUP_VOLATILE_DATA         *SetupVolatileData;
  UINTN                       VarSize;
  EFI_STATUS                  Status;

  //
  // Initialization
  //
  SetupVolatileData           = NULL;
  Status                      = EFI_SUCCESS;

  //
  // Allocate memory for SetupVolatileData
  //
  VarSize = sizeof (SETUP_VOLATILE_DATA);
  SetupVolatileData = AllocateZeroPool (VarSize);

  if (SetupVolatileData == NULL) {
    DEBUG ((DEBUG_ERROR, "Allocate SetupVolatileData size fail\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Retrive uncommitted SetupVolatileData data from Browser
  //
  Status = HiiGetBrowserData (&gSetupVariableGuid, L"SetupVolatileData", VarSize, (UINT8 *) SetupVolatileData);
  if (Status == FALSE) {
    DEBUG ((DEBUG_ERROR, "Not able to retrive SetupVolatileData\n"));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Return VTd Support
  //
  if (SetupVolatileData->VTdAvailable == TRUE) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_UNSUPPORTED;
  }

Exit:
  if (SetupVolatileData != NULL) {
    FreePool (SetupVolatileData);
  }
  return Status;
}


/**
  Get uncommitted Vtd TBT Base Security Browser value from BIOS setup menu

  @retval EFI_SUCCESS           TBT Base Security enable.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to get buffer.
  @retval EFI_UNSUPPORTED       TBT Base Security diable.
**/
EFI_STATUS
BaseSecurityGetBrowserValue (
  VOID
  )
{
  SETUP_DATA                  *SetupData;
  UINTN                       VarSize;
  EFI_STATUS                  Status;

  //
  // Initialization
  //
  SetupData = NULL;
  Status = EFI_SUCCESS;

  //
  // Allocate memory for SetupData
  //
  VarSize = sizeof (SETUP_DATA);
  SetupData = AllocateZeroPool (VarSize);
  if (SetupData == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate SetupData size\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Retrive uncommitted SetupData data from Browser
  //
  Status = HiiGetBrowserData (&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) SetupData);
  if (Status == FALSE) {
    DEBUG ((DEBUG_ERROR, "Failed to retrive SetupData\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  if (SetupData->TbtVtdBaseSecurity == FALSE) {
    Status = EFI_UNSUPPORTED;
  } else {
    Status = EFI_SUCCESS;
  }

Exit:
  if (SetupData != NULL) {
    FreePool (SetupData);
  }
  return Status;
}

/**
  Disable CSM from browser setup option from browser setup option

  @retval EFI_SUCCESS           Successfully disable CSM.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to get buffer.
  @retval EFI_INVALID_PARAMETER Failed to disable CSM
**/
EFI_STATUS
DisableCsm (
  VOID
  )
{
  SETUP_DATA                  *SetupData;
  UINTN                       VarSize;
  EFI_STATUS                  Status;
  EFI_STRING                  RequestString;

  //
  // Initialization
  //
  SetupData = NULL;
  Status = EFI_SUCCESS;
  RequestString = NULL;

  //
  // Allocate memory for SetupData
  //
  VarSize = sizeof (SETUP_DATA);
  SetupData = AllocateZeroPool (VarSize);
  if (SetupData == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate SetupData size\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Retrive uncommitted SetupData data from Browser
  //
  Status = HiiGetBrowserData (&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) SetupData);
  if (Status == FALSE) {
    DEBUG ((DEBUG_ERROR, "Failed to retrive SetupData\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Disable CSM if CSM is enable
  //
// AMI_OVERRIDE_START - CSM controlled by CSM module
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
  if (SetupData->CsmSupport != 0) {

    SetupData->CsmSupport = 0;

    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SETUP_DATA, CsmSupport), sizeof (SetupData->CsmSupport));
    if (RequestString != NULL) {
      VarSize = sizeof (SETUP_DATA);
      Status = HiiSetBrowserData (&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) SetupData, RequestString);
      if (Status != TRUE) {
        DEBUG ((DEBUG_ERROR, "Failed to enable CSM\n"));
        Status = EFI_INVALID_PARAMETER;
        goto Exit;
      }

      DEBUG ((DEBUG_INFO, "Successfully disable CSM\n"));
    }
  }
#endif
// AMI_OVERRIDE_STOP - CSM controlled by CSM module

Exit:
  if (RequestString != NULL) {
    FreePool (RequestString);
  }
  if (SetupData != NULL) {
    FreePool (SetupData);
  }

  return Status;
}
// AMI_OVERRIDE_START - follow AMI Usb
//#ifdef LEGACY_USB_FLAG
// AMI_OVERRIDE_END - follow AMI Usb
/**
  Disable Legacy USB from browser setup option

  @retval EFI_SUCCESS           Successfully disable Legacy USB.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to get buffer.
  @retval EFI_INVALID_PARAMETER Failed to disable Legacy USB.
**/
EFI_STATUS
DisableLegacyUSB (
  VOID
  )
{
  UINTN                       VarSize;
  EFI_STATUS                  Status;
  EFI_STRING                  RequestString;
  USB_SUPPORT_SETUP           *UsbSetupData;

  //
  // Initialization
  //
  Status = EFI_SUCCESS;
  RequestString = NULL;
  UsbSetupData = NULL;

  //
  // Allocate memory for UsbSetupData
  //
  VarSize = sizeof (USB_SUPPORT_SETUP);
  UsbSetupData = AllocatePool (VarSize);
  if (UsbSetupData == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate UsbSetupData size\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Retrive uncommitted UsbSetupData data from Browser
  //
  Status = HiiGetBrowserData(&gSetupVariableGuid, L"UsbSupport", VarSize, (UINT8 *) UsbSetupData);
  if (Status == FALSE) {
    DEBUG ((DEBUG_ERROR, "Failed to retrive UsbSetupData\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
// AMI_OVERRIDE_START - follow AMI Usb
  //
  //  Refer to Uhcd.c (AmiModulePkg\Usb)
  //  0: Enabled
  //  2: Auto
  //  1: Disabled
  //
  if (UsbSetupData->UsbLegacySupport != 1) {
    UsbSetupData->UsbLegacySupport = 1;
// AMI_OVERRIDE_END - follow AMI Usb
    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (USB_SUPPORT_SETUP, UsbLegacySupport), sizeof (UsbSetupData->UsbLegacySupport));
    if (RequestString != NULL) {
      //
      // Pass changed uncommitted data back to Form Browser
      //
      VarSize = sizeof (USB_SUPPORT_SETUP);
      Status = HiiSetBrowserData (&gSetupVariableGuid, L"UsbSupport", VarSize, (UINT8 *) UsbSetupData, RequestString);
      if (Status != TRUE) {
        DEBUG ((DEBUG_ERROR, "Failed to disable Legacy USB\n"));
        Status = EFI_INVALID_PARAMETER;
        goto  Exit;
      }

      DEBUG ((DEBUG_INFO, "Successfully disable Legacy USB\n"));
    }
  }

Exit:
  if (RequestString != NULL) {
    FreePool (RequestString);
  }
  if (UsbSetupData != NULL) {
    FreePool (UsbSetupData);
  }
  return Status;
}
// AMI_OVERRIDE_START - follow AMI Usb
//#endif
// AMI_OVERRIDE_END - follow AMI Usb

/**
  Setup VTD Setup option value
  @param[in]  BOOLEAN Config

  @retval EFI_SUCCESS           Successfully disable Legacy USB.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to get buffer.
  @retval EFI_INVALID_PARAMETER Failed to disable Legacy USB.
  @retval EFI_UNSUPPORTED       Vt-d is not available.
**/
EFI_STATUS
SetVtdFromBrowser (
  IN BOOLEAN Config
  )
{
  SA_SETUP                    *SaSetup;
  UINTN                       VarSize;
  EFI_STATUS                  Status;
  EFI_STRING                  RequestString;

  //
  // Initialization
  //
  SaSetup = NULL;
  Status = EFI_SUCCESS;
  RequestString = NULL;

  if (GetVtdStatusfromBrowser() == EFI_UNSUPPORTED) {
    DEBUG ((DEBUG_ERROR, "Vtd Support is unavailable\n"));
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  //
  // Allocate memory for SaSetup
  //
  VarSize = sizeof (SA_SETUP);
  SaSetup = AllocateZeroPool (VarSize);
  if (SaSetup == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate SaSetup size\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Retrive uncommitted SaSetup data from Browser
  //
  Status = HiiGetBrowserData (&gSaSetupVariableGuid, L"SaSetup", VarSize, (UINT8 *) SaSetup);
  if (Status == FALSE) {
    DEBUG ((DEBUG_ERROR, "Failed to retrive SaSetup\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Config Vtd Browser setup option
  //
  if (Config == TRUE) {
    SaSetup->EnableVtd = TRUE;
  } else {
    SaSetup->EnableVtd = FALSE;
  }

  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, EnableVtd), sizeof (SaSetup->EnableVtd));
  if (RequestString != NULL) {
    //
    // Pass changed uncommitted data back to Form Browser
    //
    VarSize = sizeof (SA_SETUP);
    Status = HiiSetBrowserData (&gSaSetupVariableGuid, L"SaSetup", VarSize, (UINT8 *) SaSetup, RequestString);
    if (Status != TRUE) {
      DEBUG ((DEBUG_INFO, "Failed to config Vtd\n"));
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }

    DEBUG ((DEBUG_INFO, "Successfully config Vtd\n"));
  }

Exit:
  if (RequestString != NULL) {
    FreePool (RequestString);
  }
  if (SaSetup != NULL) {
    FreePool (SaSetup);
  }

  return Status;
}

/**
  TBTSecurityCallBackFunction
  @param[in] This             Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in] Action           Specifies the type of action taken by the browser.
  @param[in] KeyValue         A unique value which is sent to the original exporting driver
                              so that it can identify the type of data to expect of data to expect.
  @param[in] Type             The type of value for the question.
  @param[in] Value            A pointer to the data being sent to the original exporting driver.

  @retval EFI_SUCCESS         The callback successfully handled the action
  @retval EFI_UNSUPPORTED     The specified action is not supported by the callback
  @retval EFI_INVALID_PARAMETER Failed to disable Legacy USB.
**/
EFI_STATUS
EFIAPI
TBTSecurityCallBackFunction (
// AMI_OVERRIDE_START >>> For AMI callback function
#if 0
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else
  AMI_HII_CALLBACK_PARAMETERS   *CallbackValues
#endif
// AMI_OVERRIDE_END <<< For AMI callback function
  )
{
  SETUP_VOLATILE_DATA         *SetupVolatileData;
  UINTN                       VarSize;
  EFI_STATUS                  Status;
  UINT32                      SetupVolAttributes;
// AMI_OVERRIDE_STARRT - For AMI callback function
  EFI_BROWSER_ACTION            Action;
  EFI_QUESTION_ID               KeyValue;
  EFI_IFR_TYPE_VALUE            *Value;
  EFI_BROWSER_ACTION_REQUEST    *ActionRequest;
// AMI_OVERRIDE_END - For AMI callback function



  //
  // Initialization
  //
  VarSize                     = 0;
  Status                      = EFI_SUCCESS;
  SetupVolatileData           = NULL;
  SetupVolAttributes          = 0;
// AMI_OVERRIDE_START - For AMI callback function
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
  Value = CallbackValues->Value;
  ActionRequest = CallbackValues->ActionRequest;
// AMI_OVERRIDE_END - For AMI callback function

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "[TBT] TBTSecurityCallBackFunction START\n"));

  //
  // Allocate memory for SetupVolatileData
  //
  VarSize = sizeof (SETUP_VOLATILE_DATA);
  SetupVolatileData = AllocateZeroPool (VarSize);

  if (SetupVolatileData == NULL) {
    DEBUG ((DEBUG_ERROR, "Allocate SetupVolatileData size fail\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Retrive SetupVolatileData data from memory
  //
  Status = gRT->GetVariable (
                    L"SetupVolatileData",
                    &gSetupVariableGuid,
                    &SetupVolAttributes,
                    &VarSize,
                    SetupVolatileData
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Not able to retrive SetupVolatileData from Nvram\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Retrive uncommitted BaseSecurity data from Browser
  //
  if (BaseSecurityGetBrowserValue() == EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "User enable TbtVtdBaseSecurity. Set and grayout options\n"));
    SetupVolatileData->TbtVtdBaseSecurityGrayOut = TRUE;
    //
    // Enable VTd from browser
    //
    SetVtdFromBrowser (TRUE);

    //
    // Disable CSM from browser
    //
    DisableCsm();

    //
    // Disable Legacy USB from browser
    //
// AMI_OVERRIDE_START - follow AMI Usb
//#ifdef LEGACY_USB_FLAG
    DisableLegacyUSB();
//#endif
// AMI_OVERRIDE_END - follow AMI Usb
  } else {
    DEBUG ((DEBUG_INFO, "User disable TbtVtdBaseSecurity\n"));
    SetupVolatileData->TbtVtdBaseSecurityGrayOut = FALSE;
  }

  //
  // Save SetupVolatileData data back to memory
  //
  Status = gRT->SetVariable (
                    L"SetupVolatileData",
                    &gSetupVariableGuid,
                    SetupVolAttributes,
                    VarSize,
                    SetupVolatileData
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Not able to save SetupVolatileData\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

Exit:
  if (SetupVolatileData != NULL) {
    FreePool (SetupVolatileData);
  }

  DEBUG ((DEBUG_INFO, "[TBT] TBTSecurityCallBackFunction END\n"));
  return Status;
}

// AMI_OVERRIDE_START - For AMI callback function
/**
  Init. Thunderbolt setup page callback function.

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
InitTbtSetupCallback (
  VOID
  )
{
  AmiSetupRegisterCallbackHandler (KEY_TBT_SUPPORT, TbtFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_TBT_BOOT_ON, TbtFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_TBT_USB_ON, TbtFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_TBT_LOCATION_GROUP, TbtFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_TBT_LOCATION_SB, TbtFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_TBT_LOCATION_NB, TbtFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_TBT_HOSTROUTER, TbtFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (TBT_SECURITY_LEVEL_KEY, TbtFormCallBackFunction);
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
  AmiSetupRegisterCallbackHandler (KEY_TBT_X_CONNECT_SUPPORT, TbtFormCallBackFunction);//Debug
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
  AmiSetupRegisterCallbackHandler (KEY_TBT_OS_SELECTOR, TbtOsSelectorFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_TBT_VTD_SECURITY_ID, TBTSecurityCallBackFunction);
}

/**
  Thunderbolt setup page string init. function.

  @param  HiiHandle             HiiHandle of FormData
  @param  Class                 SETUP_FORM_SET_CLASS

  @return                       No return value
**/
VOID
InitTbtStrings (
  EFI_HII_HANDLE                HiiHandle,
  UINT16                        Class
  )
{
  //Created TBT setup callback function for some TBT features
  InitTbtSetupCallback();

}
// AMI_OVERRIDE_END - For AMI callback function

