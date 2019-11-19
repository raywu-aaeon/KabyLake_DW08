//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file TbtSetupWrapper.c
    Provide functions for Thunderbolt setup page.

**/

#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Guid/HobList.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <SetupVariable.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TbtCommonLib.h>
#include <Library/MmPciLib.h>
#include <Library/GpioExpanderLib.h>
#include <TbtBoardInfo.h>
#include <Pci.h>

STATIC SETUP_DATA            SetupData;

/**
  This function will Set Security Level.

  @param  Data                  Data being sent to the original driver
  @param  TbtSelector           Security Mode Selection option in BIOS Setup

  @retval TRUE                  If TBT device present
**/
BOOLEAN
SetSecLevel (
  IN   UINT8                    Data,
  IN   UINT8                    TbtSelector
  )
{
  UINTN                         TbtBaseAddress;
  UINT32                        RegisterValue;
  UINT32                        MaxWaitIter;
  UINT8                         RetCode;
  UINT8                         TbtUsBus;
  BOOLEAN                       ReturnValue;
  UINTN                         RpDev;
  UINTN                         RpFunc;
  EFI_STATUS                    Status;

  MaxWaitIter   = 50;   ///< Wait 5 sec
  RetCode       = 0x72;
  TbtUsBus      = 3;
  ReturnValue   = FALSE;

  DEBUG ((DEBUG_INFO, "SetSecLevel() \n"));

  Status = GetTbtRpDevFun (TbtSelector - 1, &RpDev, &RpFunc);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }
  TbtUsBus = MmioRead8 (MmPciBase (0x00, (UINT32)RpDev, (UINT32)RpFunc) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  DEBUG ((DEBUG_INFO, "TBT_US_BUS = 0x%x \n", TbtUsBus));

  TbtBaseAddress = MmPciBase (TbtUsBus, 0x00, 0x00);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress = 0x%lx \n", TbtBaseAddress));
  MmioWrite32 (TbtBaseAddress + PCIE2TBT_R, (Data << 8) | PCIE2TBT_SET_SECURITY_LEVEL | PCIE2TBT_VLD_B);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress + PCIE2TBT_R = 0x%lx \n", TbtBaseAddress + PCIE2TBT_R));
  DEBUG ((DEBUG_INFO, "Data to be written = 0x%lx \n", (Data << 8) | PCIE2TBT_SET_SECURITY_LEVEL | PCIE2TBT_VLD_B));

  IoWrite8 (0x80, 0x70);

  while (MaxWaitIter-- > 0) {
    RegisterValue = MmioRead32 (TbtBaseAddress + TBT2PCIE_R);
    DEBUG ((DEBUG_INFO, "TbtBaseAddress + TBT2PCIE_R = 0x%lx \n", TbtBaseAddress + TBT2PCIE_R));
    DEBUG ((DEBUG_INFO, "Data read = 0x%lx \n", RegisterValue));

    if (0xFFFFFFFF == RegisterValue) {
      //
      // Device is not here return now
      //
      DEBUG ((DEBUG_INFO, "TBT device is not present \n"));
      RetCode = 0x7F;
      break;
    }

    if (RegisterValue & TBT2PCIE_DON_R) {
      RetCode = 0x71;
      ReturnValue     = TRUE;
      break;
    }

    gBS->Stall (100 * 1000);
  }

  MaxWaitIter = 50;  // Wait 5 sec
  MmioWrite32 (TbtBaseAddress + PCIE2TBT_R, 0);
  DEBUG ((DEBUG_INFO, "TbtBaseAddress + PCIE2TBT_R = 0x%lx \n", TbtBaseAddress + PCIE2TBT_R));
  DEBUG ((DEBUG_INFO, "Data to be written = 0x%lx \n", 0));

  IoWrite8 (0x80, 0x72);

  while (MaxWaitIter-- > 0) {
    RegisterValue = MmioRead32 (TbtBaseAddress + TBT2PCIE_R);
    DEBUG ((DEBUG_INFO, "TbtBaseAddress + TBT2PCIE_R = 0x%lx \n", TbtBaseAddress + TBT2PCIE_R));
    DEBUG ((DEBUG_INFO, "Data read = 0x%lx \n", RegisterValue));

    if (0xFFFFFFFF == RegisterValue) {
      //
      // Device is not here return now
      //
      DEBUG ((DEBUG_INFO, "TBT device is not present \n"));
      RetCode = 0x7F;
      break;
    }

    if (!(RegisterValue & TBT2PCIE_DON_R)) {
      RetCode = 0x71;
      break;
    }

    gBS->Stall (100 * 1000);
  }

  IoWrite8 (0x80, RetCode);

  return ReturnValue;
}

/**
  This function is a hook called when TSE determines
  that it has to load the boot options in the boot
  order. This function is available as ELINK.

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
TbtSetupEnterHook (
  VOID
  )
{
  UINTN                         VariableSize;

//
// EIP322151 - Set Thunderbolt_SUPPORT and disable OVER_CLOCK_SUPPORT cause build code error
//
  EFI_GUID                      SetupVariableGuid = SETUP_GUID;

  VariableSize = sizeof (SETUP_DATA);
  pRS->GetVariable (
          L"Setup",
          &SetupVariableGuid,
          NULL,
          &VariableSize,
          &SetupData
          );
}

/**
  This function is a hook called after some control
  modified in the setup utility by user. This
  function is available as ELINK.

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
TbtSetupResetHook (
  VOID
  )
{
	// Porting if needed.
}

/**
  Force Power function.

  @param  GpioAccessType        Type01: call GpioSetOutputValue;                    Type02: call GpioExpSetOutput
  @param  Expander              Type01: don't care;                                 Type02: Expander Value with in the Contoller
  @param  GpioNumber            Type01: GPIO pad;                                   Type02: Pin with in the Expnader Value
  @param  Value                 Type01: Output value: 0: OutputLow, 1: OutputHigh;  Type02: none

  @return                       No return value
**/
VOID
ForcePower (
  IN  UINT8                     GpioAccessType,
  IN  UINT8                     Expander,
  IN  UINT32                    GpioNumber,
  IN  BOOLEAN                   Value
  )
{

  if (GpioAccessType == 0x01) {
    // PCH
    GpioSetOutputValue (GpioNumber, (UINT32) Value);
  } else if (GpioAccessType == 0x02) {
    // IoExpander {TCA6424A}
    GpioExpSetOutput(Expander, (UINT8) GpioNumber, (UINT8) Value);
  }
}

/**
  Callback function to load default config

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
TbtSetupLoadDefaultHook (
  VOID
  )
{
  TBT_INFO_HOB                  *TbtInfoHob = NULL;
  EFI_STATUS                    Status;

  if (SetupData.SecurityMode != DEFAULT_TBT_SECURITY_MODE) {
    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &TbtInfoHob);
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Searching Thunderbolt(TM) information Hob... \n"));
      TbtInfoHob = GetNextGuidHob (&gTbtInfoHobGuid, TbtInfoHob);
      if (TbtInfoHob == NULL) {
        return;
      }
    }

    ForcePower (TbtInfoHob->ForcePwrGpio.GpioAccessType, TbtInfoHob->ForcePwrGpio.Expander,
                TbtInfoHob->ForcePwrGpio.GpioNumber, TbtInfoHob->ForcePwrGpio.GpioLevel);
    pBS->Stall (SetupData.Gpio3ForcePwrDly * 1000);
    SetSecLevel (DEFAULT_TBT_SECURITY_MODE, SetupData.TbtSelector);
    pBS->Stall (SetupData.Gpio3ForcePwrDly * 1000);
    ForcePower (TbtInfoHob->ForcePwrGpio.GpioAccessType, TbtInfoHob->ForcePwrGpio.Expander,
                TbtInfoHob->ForcePwrGpio.GpioNumber, !TbtInfoHob->ForcePwrGpio.GpioLevel);
  }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
