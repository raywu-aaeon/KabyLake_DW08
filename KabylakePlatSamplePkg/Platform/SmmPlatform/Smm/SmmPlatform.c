/** @file
  Smm platform driver.

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

#include "SmmPlatform.h"
#include <OemSetup.h>
#include <CpuRegs.h>
#include <PlatformInfo.h>
#include <PchPolicyCommon.h>
#include "PcieDockSmi.h"
#include "CmosMap.h"
#include "PlatformBoardId.h"
#include "AcpiCommon.h"

#define PROGRESS_CODE_S3_SUSPEND_START _gPcd_FixedAtBuild_PcdProgressCodeS3SuspendStart
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Library/MmPciLib.h>
#include <Library/GpioLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <MeChipset.h>
#include <HeciRegs.h>
#include <Protocol/PchTcoSmiDispatch.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/SmmCpu.h>
//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_SYSTEM_TABLE2                     *mSmst;

GLOBAL_REMOVE_IF_UNREFERENCED UINT16                                    mAcpiBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA                       *mGlobalNvsAreaPtr  = 0;
GLOBAL_REMOVE_IF_UNREFERENCED CPU_GLOBAL_NVS_AREA                       *mCpuGlobalNvsAreaPtr = 0;
GLOBAL_REMOVE_IF_UNREFERENCED SYSTEM_AGENT_GLOBAL_NVS_AREA              *mSaGlobalNvsAreaPtr  = 0;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_SMM_DEV                          mAcpiSmm;
//
// Setup variables
//
GLOBAL_REMOVE_IF_UNREFERENCED SETUP_DATA                                mSystemConfiguration;
GLOBAL_REMOVE_IF_UNREFERENCED PCH_SETUP                                 mPchSetup;
//
// HMRFPO lock output parameters
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_EC_ACCESS_PROTOCOL                    *mEcAccess = NULL;

GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                     mPlatformFlavor;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                     mBoardType;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                     mEcPresent;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_CPU_PROTOCOL                      *mSmmCpu;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                     mSsdPwrOverrideInSx = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                     mEctoPdControllerNegotiationInSx = 0;

/**
  Converts Decimal to BCD
  Only for 2 digit BCD.

  @param[in] Dec       - Decimal value to be converted to BCD

  @retval BCD for Dec number
**/
UINT8
DecToBCD (
  UINT8 Dec
  )
{
  UINT8 FirstDigit;
  UINT8 SecondDigit;

  FirstDigit  = Dec % 10;
  SecondDigit = Dec / 10;

  return (SecondDigit << 4) + FirstDigit;
}

/**
  Converts BCD to Dec number
  Only for 2 digit BCD.

  @param[in] BCD       - BCD number which needs to be converted to Dec

  @retval Dec value for given BCD
**/
UINT8
BCDToDec (
  UINT8 BCD
  )
{
  UINT8 FirstDigit;
  UINT8 SecondDigit;
  FirstDigit  = BCD & 0xf;
  SecondDigit = BCD >> 4;

  return SecondDigit * 10 + FirstDigit;
}

/**
  Reads the RTC Index register

  @param[in] Index - Index register

  @retval Value in Index register
**/
UINT8
ReadRtcIndex (
  IN UINT8 Index
  )
{
  UINT8           Value;
  UINT8           Addr;

  //
  // Use port RTC alternative ports 74h/75h to prevent from breaking NMI setting
  //

  //
  // Check if Data Time is valid
  //
  if (Index <= 9) {
    do {
      Addr = 0x0A;
      mSmst->SmmIo.Io.Write (
                        &mSmst->SmmIo,
                        SMM_IO_UINT8,
                        R_PCH_RTC_INDEX_ALT,
                        1,
                        &Addr
                        );
      mSmst->SmmIo.Io.Read (
                        &mSmst->SmmIo,
                        SMM_IO_UINT8,
                        R_PCH_RTC_TARGET_ALT,
                        1,
                        &Value
                        );
    } while (Value & 0x80);
  }

  Addr = Index;
  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT8,
                    R_PCH_RTC_INDEX_ALT,
                    1,
                    &Addr
                    );
  //
  // Read register.
  //
  mSmst->SmmIo.Io.Read (
                    &mSmst->SmmIo,
                    SMM_IO_UINT8,
                    R_PCH_RTC_TARGET_ALT,
                    1,
                    &Value
                    );
  if (Index <= 9) {
    Value = BCDToDec (Value);
  }

  return (UINT8) Value;
}

/**
  Writes to an RTC Index register

  @param[in] Index   - Index to be written
  @param[in] Value   - Value to be written to Index register
**/
VOID
WriteRtcIndex (
  IN UINT8 Index,
  IN UINT8 Value
  )
{
  UINT8           Addr;

  //
  // Use port RTC alternative ports 74h/75h to prevent from breaking NMI setting
  //

  Addr = Index;
  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT8,
                    R_PCH_RTC_INDEX_ALT,
                    1,
                    &Addr
                    );
  if (Index <= 9) {
    Value = DecToBCD (Value);
  }
  //
  // Write Register.
  //
  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT8,
                    R_PCH_RTC_TARGET_ALT,
                    1,
                    &Value
                    );
}

/**
  Disable RTC alarm and clear RTC PM1 status.

**/
VOID
InitRtcWakeup (
  VOID
  )
{
  UINT8  Data8;
  UINT16 Data16;

  //
  // Clear Alarm Flag (AF) by reading RTC Reg C
  //
  Data8 = ReadRtcIndex (0xC);
  mSmst->SmmIo.Io.Read (
                    &mSmst->SmmIo,
                    SMM_IO_UINT16,
                    mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
                    1,
                    &Data16
                    );
  Data16 = Data16 | B_PCH_ACPI_PM1_STS_RTC;
  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT16,
                    mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
                    1,
                    &Data16
                    );
}

/**
  Turn off Fan

**/
VOID
TurnFanOff (
  VOID
  )
{
  // Platform specific @ TBD
  // use PCH lib for GPIO attribute programming
}

/**
  Clear unexpected Wakeup events from XHCI controller.
  This function must be called after TurnFanOff.

**/
VOID
XhciClearFalseWakeupEvent(
  VOID
  )
{
  UINT16      PmState;
  UINT16      CommandState;
  UINTN       XhciPciMmBase;
  UINTN       XhciMmBase;
  UINTN       Counter;
  UINTN       LoopCounter;
  UINT32      PortStatus;

  //
  // Check if XHCI controller is enabled
  //
  XhciPciMmBase = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI
                      );
  if (MmioRead16 (XhciPciMmBase + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return;
  }

  //
  // HSB PCH-LP XHCI workaround.
  // Check XHCI controller D-state.  If it's in D3 hot, bring back it to D0.
  //
  PmState = MmioRead16(XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS);
  if ((PmState & V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3) == V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3) {
    MmioAnd16(XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS, (UINT16) ~(V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3));
  }

  //
  // Set MMIO enable bit in Command register
  //
  CommandState = MmioRead16(XhciPciMmBase + PCI_COMMAND_OFFSET);
  MmioOr16(XhciPciMmBase + PCI_COMMAND_OFFSET, (UINT16)(EFI_PCI_COMMAND_MEMORY_SPACE));

  //
  // Clear all USB status change bits in all PORTSC registers.
  //
  XhciMmBase = (MmioRead32(XhciPciMmBase + R_PCH_XHCI_MEM_BASE) & ~(0x0F)) + ((UINT64)(MmioRead32(XhciPciMmBase + R_PCH_XHCI_MEM_BASE + 4)) << 32);

  //
  // USB2 ports loop
  //
  for (Counter = 0; Counter < PCH_LP_XHCI_MAX_USB2_PORTS ; Counter++) {
    PortStatus = MmioRead32(XhciMmBase + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Counter) & (B_PCH_XHCI_PORTSCXUSB2_CSC + B_PCH_XHCI_PORTSCXUSB2_PEC + B_PCH_XHCI_PORTSCXUSB2_WRC + B_PCH_XHCI_PORTSCXUSB2_OCC + B_PCH_XHCI_PORTSCXUSB2_PRC + B_PCH_XHCI_PORTSCXUSB2_PLC +B_PCH_XHCI_PORTSCXUSB2_CEC);
    for (LoopCounter = 0 ; LoopCounter < 100 ; LoopCounter++) {
      if (PortStatus) {
        MmioOr32((XhciMmBase + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Counter), PortStatus);
      } else {
        break;
      }
      PortStatus = MmioRead32(XhciMmBase + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Counter) & (B_PCH_XHCI_PORTSCXUSB2_CSC + B_PCH_XHCI_PORTSCXUSB2_PEC + B_PCH_XHCI_PORTSCXUSB2_WRC + B_PCH_XHCI_PORTSCXUSB2_OCC + B_PCH_XHCI_PORTSCXUSB2_PRC + B_PCH_XHCI_PORTSCXUSB2_PLC +B_PCH_XHCI_PORTSCXUSB2_CEC);
    }
  }
  //
  // USB3 ports loop
  //
  for (Counter = 0; Counter < PCH_LP_XHCI_MAX_USB3_PORTS ; Counter++) {
    PortStatus = MmioRead32(XhciMmBase + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * Counter) & (B_PCH_XHCI_PORTSCXUSB3_CSC + B_PCH_XHCI_PORTSCXUSB3_PEC + B_PCH_XHCI_PORTSCXUSB3_WRC + B_PCH_XHCI_PORTSCXUSB3_OCC + B_PCH_XHCI_PORTSCXUSB3_PRC + B_PCH_XHCI_PORTSCXUSB3_PLC + B_PCH_XHCI_PORTSCXUSB3_CEC);
    for (LoopCounter = 0 ; LoopCounter < 100 ; LoopCounter++) {
      if (PortStatus) {
        MmioOr32((XhciMmBase + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * Counter), PortStatus);
      } else {
        break;
      }
      PortStatus = MmioRead32(XhciMmBase + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * Counter) & (B_PCH_XHCI_PORTSCXUSB3_CSC + B_PCH_XHCI_PORTSCXUSB3_PEC + B_PCH_XHCI_PORTSCXUSB3_WRC + B_PCH_XHCI_PORTSCXUSB3_OCC + B_PCH_XHCI_PORTSCXUSB3_PRC + B_PCH_XHCI_PORTSCXUSB3_PLC + B_PCH_XHCI_PORTSCXUSB3_CEC);
    }
  }
  //
  // Clear GPE0_STS[127:96] PME_B0_STS Bit13 to prevent a wake up event happenning
  //
  IoOr32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0_STS_127_96), (UINT32) B_PCH_ACPI_GPE0_STS_127_96_PME_B0);

  //
  // DO NOT bring XHCI controller back to the original state.  That causes wake up events again.
  //
}

/**
  Turn off NGFF WWAN device when system is going to S5 state

**/
VOID
TurnNgffWwanOff (
  VOID
  )
{
  // Platform specific @ TBD
  // use PCH lib for GPIO attribute programming

}

/**
Turn off M.2 SSD1 and M.2 SSD2 in Sx State for SKL Halo boards RVP11, RVP11PPV and RVP16
//For SKL Halo RVP platform the SSD power rail isnt turned off by the platform power rails in Sx states
//Set the SSD power to Low when platform is going in Sx state.
//This is platform specific for SKL Halo RVP 11, RVP11PPV, RVP16.
//Revisit in future to verify its need in upcoming platforms.
**/
VOID
RemoveSsdPwr(
VOID
)
{
  if (mSsdPwrOverrideInSx == 1) {
    GpioSetOutputValue(GPIO_SKL_H_GPP_G8, 0);//M.2_SSD1_PWREN set to Low
    GpioSetOutputValue(GPIO_SKL_H_GPP_G9, 0);//M.2_SSD2_PWREN set to Low
  }

}

/**
  If PCIe native mode is disabled through BIOS setup options,
  Windows ACPI driver initializes the PM1 enable bits with PCIEXP_WAKE_DIS bit set to 1 provided the flag PCI_EXP_WAK is set in FADT.
  If the FADT flag is not set then Windows will not set the PCIEXP_WAKE_DIS bit.

  In summary OS is setting PCIEXP_WAKE_DIS if native PCIe support is disabled.

  To mitigate the wake issue bios will clear this bit in case of Sx entry which will allow system to detect any wake event from Add in card."

  @param[in] VOID

  @retval    VOID
**/
VOID
EnablePchPcieWakesupport(
  VOID
)
{
  UINT16  Data16;

  if (!(mSystemConfiguration.PciExpNative)) {
    mSmst->SmmIo.Io.Read(
                      &mSmst->SmmIo,
                      SMM_IO_UINT16,
                      mAcpiBaseAddr + R_PCH_ACPI_PM1_EN,
                      1,
                      &Data16
                      );
    Data16 = Data16 & (UINT16)~B_PCH_ACPI_PM1_EN_PCIEXP_WAKE_DIS;
    mSmst->SmmIo.Io.Write(
                      &mSmst->SmmIo,
                      SMM_IO_UINT16,
                      mAcpiBaseAddr + R_PCH_ACPI_PM1_EN,
                      1,
                      &Data16
                      );
  }
}

#define DDR3_STANDBY_PWR_GPIO_PIN                      0                    // Platform specific @ TBD

/**
  S3 Sleep Entry Call Back.

  @param[in] DispatchHandle     - The handle of this callback, obtained when registering
  @param[in] DispatchContext    - The predefined context which contained sleep type and phase
  @param[in] CommBuffer         - A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in] CommBufferSize     - The size of the CommBuffer.

  @retval EFI_SUCCESS           - Operation successfully performed
**/
EFI_STATUS
EFIAPI
S3SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS              Status;
//AMI_OVERRIDE_START - Use Flag to separte the code which use EC command
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  UINT8                   EcDataBuffer;
#endif
//AMI_OVERRIDE_END - Use Flag to separte the code which use EC command

  //
  // Report status code in S3 SMI handler entry
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PROGRESS_CODE_S3_SUSPEND_START);

//AMI_OVERRIDE_START - Use Flag to separte the code which use EC command
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  EcDataBuffer = 0x03; // Input: 0x03 S3, 0x04 S4, 0x05S5
  if (mEctoPdControllerNegotiationInSx) {
    //
    // Send this EC command from SMM to notify EC and wait for response (EC_C_USBC_SX_ENTRY_WAIT for Sx Entry)
    //
    Status = UsbcSxEntry (&EcDataBuffer);
  }
#endif
//AMI_OVERRIDE_END - Use Flag to separte the code which use EC command

  //
  // Enable Wakesupport for PCIE Add-on Cards when Native support is disabled
  //
  EnablePchPcieWakesupport();

  Status = GetPchAcpiBase (mSmst);

  Status = SaveRuntimeScriptTable (mSmst);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if ((mPlatformFlavor == FlavorMobile) || (mPlatformFlavor == FlavorEmbedded)) {
    //
    // This GPIO operation is designed to take care of DDR3 Standby Power Reduction
    //  PullDown GPIO
    //
    //GpioSetPadElectricalConfig (DDR3_STANDBY_PWR_GPIO_PIN, GpioTermNone); //@todo, uncomment when DDR3_STANDBY_PWR_GPIO_PIN has been assigned correct GpioPad
  }

  RemoveSsdPwr();

  //
  // WARNING!!! Please do not modify GP_LVL register after this point
  //

  return EFI_SUCCESS;
}

/**
  S4 Sleep Entry Call Back.

  @param[in] DispatchHandle     - The handle of this callback, obtained when registering
  @param[in] DispatchContext    - The predefined context which contained sleep type and phase
  @param[in] CommBuffer         - A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in] CommBufferSize     - The size of the CommBuffer.

  @retval EFI_SUCCESS           - Operation successfully performed
**/
EFI_STATUS
EFIAPI
S4SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
//AMI_OVERRIDE_START - Use Flag to separte the code which use EC command	
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  UINT8                   EcDataBuffer;
  //
  // Put EC in non-ACPI mode.  This ensures EC takes over FAN control from now on till actual shutdown.
  //
  EnableEcAcpiMode (FALSE);
  if (mEctoPdControllerNegotiationInSx) {

    EcDataBuffer = 0x04; // Input: 0x03 S3, 0x04 S4, 0x05 S5
    //
    // Send this EC command from SMM to notify EC and wait for response (EC_C_USBC_SX_ENTRY_WAIT for Sx Entry)
    //
    UsbcSxEntry (&EcDataBuffer);
  }
#endif
//AMI_OVERRIDE_END - Use Flag to separte the code which use EC command

  //
  // Enable Wakesupport for PCIE Add-on Cards when Native support is disabled
  //
  EnablePchPcieWakesupport();

  RemoveSsdPwr();

  //
  // WARNING!!! Please do not modify GP_LVL register after this point
  //

  return EFI_SUCCESS;
}

/**
  S5 Sleep Entry Call Back.

  @param[in] DispatchHandle     - The handle of this callback, obtained when registering
  @param[in] DispatchContext    - The predefined context which contained sleep type and phase
  @param[in] CommBuffer         - A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in] CommBufferSize     - The size of the CommBuffer.

  @retval EFI_SUCCESS           - Operation successfully performed
**/
EFI_STATUS
EFIAPI
S5SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  UINT32    Data32;
//AMI_OVERRIDE_START - Use TOKEN to disable RTCWAKEUP feature  
#if defined RTCWakeup_SUPPORT_FLAG && RTCWakeup_SUPPORT_FLAG == 1
  UINT16    Data16;
#endif
//AMI_OVERRIDE_END - Use TOKEN to disable RTCWAKEUP feature
  
//AMI_OVERRIDE_START - Use Flag to separte the code which use EC command  
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  UINT8     EcDataBuffer;

  //
  // Put EC in non-ACPI mode.  This ensures EC takes over FAN control from now on till actual shutdown.
  //
  EnableEcAcpiMode (FALSE);

  EcDataBuffer = 0x05; // Input: 0x03 S3, 0x04 S4, 0x05 S5
  if (mEctoPdControllerNegotiationInSx) {
    //
    // Send this EC command from SMM to notify EC and wait for response (EC_C_USBC_SX_ENTRY_WAIT for Sx Entry)
    //
    UsbcSxEntry (&EcDataBuffer);
  }
#endif
//AMI_OVERRIDE_END - Use Flag to separte the code which use EC command  

  //
  // Enable Wakesupport for PCIE Add-on Cards when Native support is disabled
  //
  EnablePchPcieWakesupport();

//AMI_OVERRIDE_START - Use TOKEN to disable RTCWAKEUP feature 
#if defined RTCWakeup_SUPPORT_FLAG && RTCWakeup_SUPPORT_FLAG == 1
  if (mSystemConfiguration.WakeOnRTCS5 == 1) {

    //
    // Clear RTC PM1 status
    //
    mSmst->SmmIo.Io.Read (
                      &mSmst->SmmIo,
                      SMM_IO_UINT16,
                      mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
                      1,
                      &Data16
                      );
    Data16 = Data16 | B_PCH_ACPI_PM1_STS_RTC;
    mSmst->SmmIo.Io.Write (
                      &mSmst->SmmIo,
                      SMM_IO_UINT16,
                      mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
                      1,
                      &Data16
                      );
    //
    // set RTC_EN bit in PM1_EN to wake up from the alarm
    //
    mSmst->SmmIo.Io.Read (
                      &mSmst->SmmIo,
                      SMM_IO_UINT16,
                      mAcpiBaseAddr + R_PCH_ACPI_PM1_EN,
                      1,
                      &Data16
                      );
    Data16 = Data16 | B_PCH_ACPI_PM1_EN_RTC;
    mSmst->SmmIo.Io.Write (
                      &mSmst->SmmIo,
                      SMM_IO_UINT16,
                      mAcpiBaseAddr + R_PCH_ACPI_PM1_EN,
                      1,
                      &Data16
                      );
  }
#endif 
//AMI_OVERRIDE_END - Use TOKEN to disable RTCWAKEUP feature 
  //
  // Read GPE0 enable register and set PME_B0 enable (bit 13) to enable
  // WOL from S5 state.
  //
  mSmst->SmmIo.Io.Read (
                        &mSmst->SmmIo,
                        SMM_IO_UINT32,
                        mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96,
                        1,
                        &Data32
                        );
  Data32 |= BIT13;
  mSmst->SmmIo.Io.Write (
                        &mSmst->SmmIo,
                        SMM_IO_UINT32,
                        mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96,
                        1,
                        &Data32
                        );

  // @todo Need to check if this is still needed for SKL SDS: This should be removed when WWAN RTD3 and CS function are functional.
  if (mBoardType == BoardTypeSds) {
    TurnNgffWwanOff();
  }

  RemoveSsdPwr();

  //
  // WARNING!!! Please do not modify GP_LVL register after this point
  //

  return EFI_SUCCESS;
}

/**
  When a power button event happens, it shuts off the machine

  @param[in] DispatchHandle    - Handle of this dispatch function
  @param[in] DispatchContext   - Pointer to the dispatch function's context
  @param[in] CommBuffer        - A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in] CommBufferSize    - The size of the CommBuffer.

  @retval EFI_SUCCESS          - Operation successfully performed
**/
EFI_STATUS
EFIAPI
PowerButtonCallback (
  IN  EFI_HANDLE                 DispatchHandle,
  IN  CONST VOID                 *DispatchContext,
  IN  OUT VOID                   *CommBuffer  OPTIONAL,
  IN  UINTN                      *CommBufferSize  OPTIONAL
  )
{
  UINT32  Buffer;
  UINT32  Data32;
  UINT16  Data16;

  //
  // Clear Sleep Type Enable
  //


  //
  // Clear Power Button Status
  //
  mSmst->SmmIo.Io.Read (
                    &mSmst->SmmIo,
                    SMM_IO_UINT16,
                    mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
                    1,
                    &Data16
                    );
  Data32 = Data16 | B_PCH_ACPI_PM1_STS_PWRBTN;
  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                     SMM_IO_UINT32,
                    mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
                    1,
                     &Data32
                     );

  //
  // Shut it off now
  //
    mSmst->SmmIo.Io.Read (
                    &mSmst->SmmIo,
                    SMM_IO_UINT32,
                    mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
                    1,
                    &Buffer
                    );
  Buffer &= ~(B_PCH_ACPI_PM1_CNT_SLP_EN | B_PCH_ACPI_PM1_CNT_SLP_TYP);

  Buffer |= V_PCH_ACPI_PM1_CNT_S5;

  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT32,
                     mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
                    1,
                     &Buffer
                     );


  Buffer |= B_PCH_ACPI_PM1_CNT_SLP_EN;

  mSmst->SmmIo.Io.Write (
                    &mSmst->SmmIo,
                    SMM_IO_UINT32,
                   mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
                    1,
                   &Buffer
                   );

  return EFI_SUCCESS;
}

/**
  Get PCH chipset Power Management I/O Base at runtime.

  @param[in] Smst        - The standard SMM system table.

  @retval EFI_SUCCESS    - Successfully init the device.
  @retval Other          - Error occured whening calling Dxe lib functions.

**/
EFI_STATUS
GetPchAcpiBase (
  IN EFI_SMM_SYSTEM_TABLE2      *Smst
  )
{
  PchAcpiBaseGet (&(mAcpiSmm.PchAcpiBase));
  return EFI_SUCCESS;
}

EFI_STATUS
SaveRuntimeScriptTable (
  IN EFI_SMM_SYSTEM_TABLE2      *Smst
  )
{
  EFI_SMM_SCRIPT_TABLE  ScriptTablePtr;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  PciAddress;
  UINT32                Data32;
  UINTN                 DeviceBaseAddress;
  // Create runtime script table
  //
  ScriptTablePtr = mAcpiSmm.RuntimeScriptTableBase;
  SmmBootScriptCreateTable (&ScriptTablePtr, EFI_ACPI_S3_RESUME_SCRIPT_TABLE);

  //
  // Check if GMM device is present or not
  //
  DeviceBaseAddress = MmPciBase (0x00, 0x08, 0x00);

  if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {

    //
    // Save Memory base register of D08:F4 R10-R13
    //
    PciAddress.Bus              = 0x00;
    PciAddress.Device           = 0x08;
    PciAddress.Function         = 0x00;
    PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    if(((Data32 & BIT0) == 0) && ((Data32 & BIT2) == 1)){

      //
      // Save upper 32bits of 64bits BAR if device implements this.
      //
      PciAddress.Bus              = 0x00;
      PciAddress.Device           = 0x08;
      PciAddress.Function         = 0x00;
      PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET + 0x04;
      PciAddress.ExtendedRegister = 0x00;

      Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

      SmmBootScriptWrite (
                  &ScriptTablePtr,
                  EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                  EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                  S3BootScriptWidthUint32,
                  PciAddress,
                  (UINTN)1,
                  &Data32
                  );
    }
  }

  DeviceBaseAddress = MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_ISH,
                           PCI_FUNCTION_NUMBER_PCH_ISH
                           );

  if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {

    //
    // Save PCI register of D19:F0 R04
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_ISH;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_ISH;
    PciAddress.Register         = PCI_COMMAND_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );
    //
    // Save Memory base register of D19:F0 R10-R13
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_ISH;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_ISH;
    PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    if(((Data32 & BIT0) == 0) && ((Data32 & BIT2) == 1)){

      //
      // Save upper 32bits of 64bits BAR if device implements this.
      //
      PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
      PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_ISH;
      PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_ISH;
      PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET + 0x04;
      PciAddress.ExtendedRegister = 0x00;

      Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

      SmmBootScriptWrite (
                  &ScriptTablePtr,
                  EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                  EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                  S3BootScriptWidthUint32,
                  PciAddress,
                  (UINTN)1,
                  &Data32
                  );
    }
  }

  DeviceBaseAddress = MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_THERMAL,
                           PCI_FUNCTION_NUMBER_PCH_THERMAL
                           );

  if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {

    //
    // Save PCI register of D20:F2 R04
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_THERMAL;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_THERMAL;
    PciAddress.Register         = PCI_COMMAND_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    //
    // Save Memory base register of D20:F2 R10-R13
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_THERMAL;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_THERMAL;
    PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET;
    PciAddress.ExtendedRegister = 0;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    if(((Data32 & BIT0) == 0) && ((Data32 & BIT2) == 1)){

      //
      // Save upper 32bits of 64bits BAR if device implements this.
      //
      PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
      PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_THERMAL;
      PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_THERMAL;
      PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET + 0x04;;
      PciAddress.ExtendedRegister = 0x00;

      Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

      SmmBootScriptWrite (
                  &ScriptTablePtr,
                  EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                  EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                  S3BootScriptWidthUint32,
                  PciAddress,
                  (UINTN)1,
                  &Data32
                  );
    }
  }

  DeviceBaseAddress = MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_PMC,
                           PCI_FUNCTION_NUMBER_PCH_PMC
                           );

  if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {

    //
    // Save PCI register of D31:F2 R04
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_PMC;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_PMC;
    PciAddress.Register         = PCI_COMMAND_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    //
    // Save Memory base register of D31:F2 R10-R13
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_PMC;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_PMC;
    PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    if(((Data32 & BIT0) == 0) && ((Data32 & BIT2) == 1)){

      //
      // Save upper 32bits of 64bits BAR if device implements this.
      //
      PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
      PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_PMC;
      PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_PMC;
      PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET + 0x04;
      PciAddress.ExtendedRegister = 0x00;

      Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

      SmmBootScriptWrite (
                  &ScriptTablePtr,
                  EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                  EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                  S3BootScriptWidthUint32,
                  PciAddress,
                  (UINTN)1,
                  &Data32
                  );
    }
  }

  DeviceBaseAddress = MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_SMBUS,
                           PCI_FUNCTION_NUMBER_PCH_SMBUS
                           );

  if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {

    //
    // Save PCI register of D31:F4 R04
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_SMBUS;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_SMBUS;
    PciAddress.Register         = PCI_COMMAND_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
              &ScriptTablePtr,
              EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
              EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
              S3BootScriptWidthUint32,
              PciAddress,
              (UINTN)1,
              &Data32
              );
    //
    // Save Memory base register of D31:F4 R10-R13
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_SMBUS;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_SMBUS;
    PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    if(((Data32 & BIT0) == 0) && ((Data32 & BIT2) == 1)){

      //
      // Save upper 32bits of 64bits BAR if device implements this.
      //
      PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
      PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_SMBUS;
      PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_SMBUS;
      PciAddress.Register         = PCI_BASE_ADDRESSREG_OFFSET + 0x04;
      PciAddress.ExtendedRegister = 0x00;

      Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

      SmmBootScriptWrite (
                  &ScriptTablePtr,
                  EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                  EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                  S3BootScriptWidthUint32,
                  PciAddress,
                  (UINTN)1,
                  &Data32
                  );
    }
  }

  DeviceBaseAddress = MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2,
                           PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2
                           );

  if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {

    //
    // Save Memory base register of D25:F0 R18-R1B
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2;
    PciAddress.Register         = R_PCH_SERIAL_IO_BAR1_LOW;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    if(((Data32 & BIT0) == 0) && ((Data32 & BIT2) == 1)){

      //
      // Save upper 32bits of 64bits BAR if device implements this.
      //
      PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
      PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2;
      PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2;
      PciAddress.Register         = R_PCH_SERIAL_IO_BAR1_HIGH;
      PciAddress.ExtendedRegister = 0x00;

      Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

      SmmBootScriptWrite (
                  &ScriptTablePtr,
                  EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                  EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                  S3BootScriptWidthUint32,
                  PciAddress,
                  (UINTN)1,
                  &Data32
                  );
    }

    //
    // Save PCI register of D25:F0 R3C
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2;
    PciAddress.Register         = R_PCH_SERIAL_IO_INTERRUPTREG;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
              &ScriptTablePtr,
              EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
              EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
              S3BootScriptWidthUint32,
              PciAddress,
              (UINTN)1,
              &Data32
              );

    //
    // Save PCI register of 25:F0 R04
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2;
    PciAddress.Register         = PCI_COMMAND_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
              &ScriptTablePtr,
              EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
              EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
              S3BootScriptWidthUint32,
              PciAddress,
              (UINTN)1,
              &Data32
              );

  }

  DeviceBaseAddress = MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_LPC,
                           PCI_FUNCTION_NUMBER_PCH_LPC
                           );

  if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {

    //
    // Save Memory base register of D31:F0 R10-R13
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_LPC;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_LPC;
    PciAddress.Register         = 0x60;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    //
    // Save Memory base register of D31:F0 R10-R13
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_LPC;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_LPC;
    PciAddress.Register         = 0x68;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );
  }


  DeviceBaseAddress = MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_HDA,
                           PCI_FUNCTION_NUMBER_PCH_HDA
                           );

  if (MmioRead16 (DeviceBaseAddress + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {

    //
    // Save Memory base register of D31:F3 R10-R13
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_HDA;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_HDA;
    PciAddress.Register         = R_PCH_HDA_HDALBA;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
                &ScriptTablePtr,
                EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                S3BootScriptWidthUint32,
                PciAddress,
                (UINTN)1,
                &Data32
                );

    if(((Data32 & BIT0) == 0) && ((Data32 & BIT2) == 1)){

      //
      // Save upper 32bits of 64bits BAR if device implements this.
      //
      PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
      PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_HDA;
      PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_HDA;
      PciAddress.Register         = R_PCH_HDA_HDAUBA;
      PciAddress.ExtendedRegister = 0x00;

      Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

      SmmBootScriptWrite (
                  &ScriptTablePtr,
                  EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
                  EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
                  S3BootScriptWidthUint32,
                  PciAddress,
                  (UINTN)1,
                  &Data32
                  );
    }

    //
    // Save PCI register of D31:F3 R3C
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_HDA;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_HDA;
    PciAddress.Register         = PCI_INT_LINE_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
              &ScriptTablePtr,
              EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
              EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
              S3BootScriptWidthUint32,
              PciAddress,
              (UINTN)1,
              &Data32
              );

    //
    // Save PCI register of D31:F3 R04
    //
    PciAddress.Bus              = DEFAULT_PCI_BUS_NUMBER_PCH;
    PciAddress.Device           = PCI_DEVICE_NUMBER_PCH_HDA;
    PciAddress.Function         = PCI_FUNCTION_NUMBER_PCH_HDA;
    PciAddress.Register         = PCI_COMMAND_OFFSET;
    PciAddress.ExtendedRegister = 0x00;

    Data32 = PciRead32 (PCI_LIB_ADDRESS(PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciAddress.Register));

    SmmBootScriptWrite (
              &ScriptTablePtr,
              EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
              EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
              S3BootScriptWidthUint32,
              PciAddress,
              (UINTN)1,
              &Data32
              );

  }


  //
  // Close table, ensure integrity
  //
  SmmBootScriptCloseTable (
    (mAcpiSmm.RuntimeScriptTableBase),
    ScriptTablePtr,
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE
    );

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ECAccessCallback (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                        Status;
//AMI_OVERRIDE_START - If SMRAM size is too small, it would locate SMRAM failed 
#if 0
  CPU_SETUP                         CpuSetup;
  PCH_SETUP                         PchSetup;
  UINTN                             VariableSize;
#endif 
//AMI_OVERRIDE_END - If SMRAM size is too small, it would locate SMRAM failed 

  Status = mSmst->SmmLocateProtocol(&gEfiEcAccessProtocolGuid, NULL, (VOID **) &mEcAccess);
  ASSERT_EFI_ERROR(Status);

//AMI_OVERRIDE_START - If SMRAM size is too small, it would locate SMRAM failed
#if 0
  VariableSize = sizeof(CPU_SETUP);
  Status = gRT->GetVariable(
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &CpuSetup
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VariableSize = sizeof (PCH_SETUP);
  Status = gRT->GetVariable (
                  L"PchSetup",
                  &gPchSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &PchSetup
                  );
#endif                  
//AMI_OVERRIDE_END - If SMRAM size is too small, it would locate SMRAM failed
 

  if (mEcAccess != NULL) {
//AMI_OVERRIDE_START - If SMRAM size is too small, it would locate SMRAM failed 
#if 0      
    DEBUG ((DEBUG_INFO, "DeepSx Mode: %x\n", PchSetup.DeepSxMode));
    switch(PchSetup.DeepSxMode) {
#else    
    DEBUG ((DEBUG_INFO, "DeepSx Mode: %x\n", mPchSetup.DeepSxMode));
    switch(mPchSetup.DeepSxMode) {     
#endif    
//AMI_OVERRIDE_END - If SMRAM size is too small, it would locate SMRAM failed     
      case PchDpS4S5BatteryEn:
      case PchDpS4S5AlwaysEn:
        mEcAccess->DeepSxConfig(0x03);
        break;

      case PchDpS3S4S5BatteryEn:
      case PchDpS3S4S5AlwaysEn:
        mEcAccess->DeepSxConfig(0x05);
        break;

      case PchDeepSxPolDisable:
      default:
        mEcAccess->DeepSxConfig(0x00);
    }
    
//AMI_OVERRIDE_START - If SMRAM size is too small, it would locate SMRAM failed
#if 0     
    if (CpuSetup.EcTurboControlMode) {
      mEcAccess->TurboCtrlMode(TRUE, CpuSetup.AcBrickCapacity, CpuSetup.EcPollingPeriod,
                                     CpuSetup.EcGuardBandValue, CpuSetup.EcAlgorithmSel);
#else
    if (mPchSetup.EcTurboControlMode) {
      mEcAccess->TurboCtrlMode(TRUE, mPchSetup.AcBrickCapacity, mPchSetup.EcPollingPeriod,
                                     mPchSetup.EcGuardBandValue, mPchSetup.EcAlgorithmSel);      
#endif 
//AMI_OVERRIDE_END - If SMRAM size is too small, it would locate SMRAM failedIf SMRAM size is too small, it would locate SMRAM failed      
    } else {
      mEcAccess->TurboCtrlMode(FALSE, 0x00, 0x00, 0x00, 0x00);
    }
  }
  return EFI_SUCCESS;
}


/**
  This hardware SMI handler will be run every time the BIOS Write Enable bit is set.

  @param[in] DispatchHandle       Not used

**/
VOID
EFIAPI
PchTcoTimeoutCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{

  DEBUG((DEBUG_INFO, "PchTcoTimeoutCallback entry\n"));


  //
  // Stall for 3 seconds to trigger the second timeout.
  //
  MicroSecondDelay (3000 * 1000);

  //
  // A catastrophic crash happened (causing TCO timeout) when we reach here
  // Directly trigger a warm reboot here
  //
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET);

  CpuDeadLoop ();

  return;
}

/**
  Register a SMI handler for Tco timeout event.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid pointer passed.
**/
EFI_STATUS
EFIAPI
RegisterTcoTimeoutHandler (
  VOID
  )
{
  EFI_STATUS                                Status;
  PCH_TCO_SMI_DISPATCH_PROTOCOL             *mPchTcoSmiDispatchProtocol;
  EFI_HANDLE                                Handle;

  Status = mSmst->SmmLocateProtocol (&gPchTcoSmiDispatchProtocolGuid, NULL, (VOID **) &mPchTcoSmiDispatchProtocol);

  Handle = NULL;
  Status = mPchTcoSmiDispatchProtocol->TcoTimeoutRegister (
                                         mPchTcoSmiDispatchProtocol,
                                         PchTcoTimeoutCallback,
                                         &Handle
                                         );
  DEBUG((DEBUG_INFO, "RegisterTcoTimeoutHandler status = %x\n", Status));

  return Status;
}

/**
  Initializes the SMM Platform Driver

  @param[in] ImageHandle   - Pointer to the loaded image protocol for this driver
  @param[in] SystemTable   - Pointer to the EFI System Table

  @retval Status           - EFI_SUCCESS
  @retval Assert, otherwise.

**/
EFI_STATUS
EFIAPI
InitializePlatformSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
// AMI_OVERRIDE_START - EIP349813 : Please use a Token/Flag to determine whether EspiSlaveSmi is Registered or not.
#ifdef AMI_CRB_EC_SUPPORT_FLAG  
  EFI_HANDLE                                Handle;
#endif
// AMI_OVERRIDE_EDN - EIP349813 : Please use a Token/Flag to determine whether EspiSlaveSmi is Registered or not.  
  EFI_HANDLE                                PowerButtonHandle;
//AMI_OVERRIDE_START - For support dock 
#ifdef DOCK_ENABLE  
  EFI_HANDLE                                SwHandle;
#endif 
//AMI_OVERRIDE_END - For support dock 
  EFI_HANDLE                                GpiHandle;
  EFI_HANDLE                                S4SleepEntryHandle;
  EFI_HANDLE                                S5SleepEntryHandle;
#ifdef EFI_S3_RESUME
  EFI_HANDLE                                S3SleepEntryHandle;
#endif

  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL   *PowerButtonDispatch;
//AMI_OVERRIDE_START - For support dock 
#ifdef DOCK_ENABLE  
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch;
#endif
//AMI_OVERRIDE_END - For support dock 
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT     PowerButtonContext;
//AMI_OVERRIDE_START - For support dock 
#ifdef DOCK_ENABLE   
  EFI_SMM_SW_REGISTER_CONTEXT               SwContext;
#endif
//AMI_OVERRIDE_END - For support dock 
  EFI_SMM_SX_DISPATCH2_PROTOCOL             *SxDispatch;
  EFI_SMM_SX_REGISTER_CONTEXT               EntryDispatchContext;
  VOID                                      *Registeration;
  EFI_GLOBAL_NVS_AREA_PROTOCOL              *GlobalNvsAreaProtocol;
  UINTN                                     DataSize;
  PLATFORM_INFO                             *PlatformInfo = NULL;
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL     *SaGlobalNvsAreaProtocol;
  CPU_GLOBAL_NVS_AREA_PROTOCOL              *CpuGlobalNvsAreaProtocol;
  SETUP_CPU_FEATURES                        SetupCpuFeatures;
  UINT32                                    VariableAttr;
  UINT64                                    SmmFeatureMsr;
// AMI_OVERRIDE_START - EIP349813 : Please use a Token/Flag to determine whether EspiSlaveSmi is Registered or not.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  PCH_ESPI_SMI_DISPATCH_PROTOCOL            *PchEspiSmiDispatchProtocol;
#endif
// AMI_OVERRIDE_EDN - EIP349813 : Please use a Token/Flag to determine whether EspiSlaveSmi is Registered or not.
  EFI_PEI_HOB_POINTERS                      HobList;
//AMI_OVERRIDE_START - Use Smm Variable Protocol to set variable in SMM  
  EFI_SMM_VARIABLE_PROTOCOL                 *SmmVariable;    
//AMI_OVERRIDE_END - Use Smm Variable Protocol to set variable in SMM  

// AMI_OVERRIDE_START - EIP349813 : Please use a Token/Flag to determine whether EspiSlaveSmi is Registered or not.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  Handle              = NULL;
#endif
// AMI_OVERRIDE_EDN - EIP349813 : Please use a Token/Flag to determine whether EspiSlaveSmi is Registered or not.
  PowerButtonHandle   = NULL;
//AMI_OVERRIDE_START - For support dock  
#ifdef DOCK_ENABLE   
  SwHandle            = NULL;
#endif
//AMI_OVERRIDE_END - For support dock 
  GpiHandle           = NULL;

  //
  // Great!  We're now in SMM!
  //
  mEctoPdControllerNegotiationInSx = PcdGetBool (PcdUsbcEcPdNegotiation) ? 1 : 0;
  //
  // Get the HOB list. If it is not present, then ASSERT.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Fail to get the HOB list.\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  HobList.Raw = GetNextGuidHob (&gPlatformInfoHobGuid, HobList.Raw);
  if (HobList.Raw == NULL) {
    return EFI_NOT_FOUND;
  } else {
    PlatformInfo    = (PLATFORM_INFO *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
    mPlatformFlavor = PlatformInfo->PlatformFlavor;
    mEcPresent      = PlatformInfo->EcPresent;
    mSsdPwrOverrideInSx = PcdGetBool (PcdSsdPwrOverrideInSx) ? 1 : 0;
  }

  //
  // Initialize global variables
  //
  mSmst = gSmst;

  //
  // Locate setup variable
  // We assert if it is not found because we have a dependency on AcpiPlatform which is
  // dependent on protocols the setup driver produces.
  //
  DataSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &DataSize,
                  &mSystemConfiguration
                  );
//  ASSERT_EFI_ERROR (Status);

  DataSize = sizeof (PCH_SETUP);
  Status = gRT->GetVariable (
                  L"PchSetup",
                  &gPchSetupVariableGuid,
                  NULL,
                  &DataSize,
                  &mPchSetup
                  );
  //
  // Get the ACPI Base Address
  //
  PchAcpiBaseGet (&mAcpiBaseAddr);

  //
  // Disable RTC alarm and clear RTC PM1 status
  //
  InitRtcWakeup();

  //
  // Locate the ICH SMM Power Button dispatch protocol
  //
  Status = mSmst->SmmLocateProtocol (&gEfiSmmPowerButtonDispatch2ProtocolGuid, NULL, (VOID**)&PowerButtonDispatch);
  ASSERT_EFI_ERROR (Status);

  //
  // Register the power button SMM event
  //
  PowerButtonContext.Phase = EfiPowerButtonEntry;
  Status = PowerButtonDispatch->Register (
                                  PowerButtonDispatch,
                                  PowerButtonCallback,
                                  &PowerButtonContext,
                                  &PowerButtonHandle
                                  );
  ASSERT_EFI_ERROR (Status);

//AMI_OVERRIDE_START - For support dock  
#ifdef DOCK_ENABLE
  //
  //  Locate the ICH SMM SW dispatch protocol
  //
  Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch);
  ASSERT_EFI_ERROR (Status);
#endif
//AMI_OVERRIDE_END - For support dock 

  //
  // Locate our shared data area
  //
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;

  //
  // CPU Global NVS
  //
  Status = gBS->LocateProtocol (&gCpuGlobalNvsAreaProtocolGuid, NULL, (VOID **) &CpuGlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mCpuGlobalNvsAreaPtr = CpuGlobalNvsAreaProtocol->Area;

  //
  // SA Global NVS area
  //
  Status = gBS->LocateProtocol (&gSaGlobalNvsAreaProtocolGuid, NULL, (VOID **) &SaGlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mSaGlobalNvsAreaPtr = SaGlobalNvsAreaProtocol->Area;

  Status = mSmst->SmmRegisterProtocolNotify (
                    &gEfiEcAccessProtocolGuid,
                    ECAccessCallback,
                    &Registeration
                    );
  ASSERT_EFI_ERROR(Status);

#ifdef DOCK_ENABLE
  //
  // Register PCIe Dock Bridges IO MEM Base Limit Patch Handler
  //
  SwContext.SwSmiInputValue = (UINTN) PcdGet8 (PcdPcieDockBridgeResourcePatchSmi);
  Status = SwDispatch->Register (
                         SwDispatch,
                         PcieDockSmiCallback,
                         &SwContext,
                         &SwHandle
                         );
  ASSERT_EFI_ERROR (Status);
#endif

  //
  // Locate the SmmSxDispatch protocol
  //
  Status = mSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid, NULL, (VOID**)&SxDispatch);
  if (EFI_ERROR(Status)) {
    return Status;
  }

#ifdef EFI_S3_RESUME
  //
  // Register S3 entry phase call back function
  //
  EntryDispatchContext.Type  = SxS3;
  EntryDispatchContext.Phase = SxEntry;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S3SleepEntryCallBack,
                         &EntryDispatchContext,
                         &S3SleepEntryHandle
                         );
#endif
  //
  // Register S4-Entry phase callback function
  //
  EntryDispatchContext.Type  = SxS4;
  EntryDispatchContext.Phase = SxEntry;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S4SleepEntryCallBack,
                         &EntryDispatchContext,
                         &S4SleepEntryHandle
                         );
  //
  // Register S5 entry phase call back function
  //
  EntryDispatchContext.Type  = SxS5;
  EntryDispatchContext.Phase = SxEntry;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S5SleepEntryCallBack,
                         &EntryDispatchContext,
                         &S5SleepEntryHandle
                         );

  //
  // Check MSR SMM_MCA_CAP(0x17D) whether supports CPU SMM Enhancement Feature
  //
  DataSize = sizeof (SETUP_CPU_FEATURES);
  Status = gRT->GetVariable (
                  L"SetupCpuFeatures",
                  &gSetupVariableGuid,
                  &VariableAttr,
                  &DataSize,
                  &SetupCpuFeatures
                  );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_WARN, "Get CPU Feature variable failure \n"));
  }

  SetupCpuFeatures.SmmEnhancementSupported = 0;
  SetupCpuFeatures.SmmTargetedSmiAvailable = 0;
  SetupCpuFeatures.SmmCpuSvrstrAvailable = 0;
  SetupCpuFeatures.SmmCodeAccessAvailable = 0;
  SetupCpuFeatures.SmmLongFlowAvailable = 0;
  SmmFeatureMsr = AsmReadMsr64 (MSR_SMM_MCA_CAP);
  if ((SmmFeatureMsr & (B_TARGETED_SMI | B_SMM_CPU_SVRSTR | B_SMM_CODE_ACCESS_CHK | B_LONG_FLOW_INDICATION)) != 0) {
    SetupCpuFeatures.SmmEnhancementSupported = 1;
    SetupCpuFeatures.SmmTargetedSmiAvailable = (UINT8) RShiftU64((SmmFeatureMsr & B_TARGETED_SMI), N_TARGETED_SMI);
    SetupCpuFeatures.SmmCpuSvrstrAvailable   = (UINT8) RShiftU64((SmmFeatureMsr & B_SMM_CPU_SVRSTR), N_SMM_CPU_SVRSTR);
    SetupCpuFeatures.SmmCodeAccessAvailable  = (UINT8) RShiftU64((SmmFeatureMsr & B_SMM_CODE_ACCESS_CHK), N_SMM_CODE_ACCESS_CHK);
    SetupCpuFeatures.SmmLongFlowAvailable    = (UINT8) RShiftU64((SmmFeatureMsr & B_LONG_FLOW_INDICATION), N_LONG_FLOW_INDICATION);
  }

//AMI_OVERRIDE_START - Use Smm Variable Protocol to set variable in SMM  
  Status = mSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID **) &SmmVariable);
  ASSERT_EFI_ERROR (Status);

#if 0  
  Status = gRT->SetVariable (
#else
  Status = SmmVariable->SmmSetVariable (
#endif
//AMI_OVERRIDE_END - Use Smm Variable Protocol to set variable in SMM         
                  L"SetupCpuFeatures",
                  &gSetupVariableGuid,
                  VariableAttr,
                  DataSize,
                  &SetupCpuFeatures
                  );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_WARN, "Set CPU Feature variable failure \n"));
  }

#ifdef EFI_S3_RESUME
  //
  // Initialize RuntimeScriptTable base address
  //
  Status = InitializeSmmScriptLib (SystemTable, 2, &(mAcpiSmm.RuntimeScriptTableBase));
  DEBUG((DEBUG_INFO, "RuntimeScriptTableBase is 0x%x\n", mAcpiSmm.RuntimeScriptTableBase));
  ASSERT_EFI_ERROR (Status);
#endif

  //
  // Lock HMRFPO
  // Output parameters are stored in global variables that can be used in subsequent calls in SMM
  // call back functions, e.g., HMRFPO Enable
  //

// AMI_OVERRIDE_START - EIP349813 : Please use a Token/Flag to determine whether EspiSlaveSmi is Registered or not.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  //
  //   Locate the PCH eSPI SMI dispatch protocol.
  //
  PchEspiSmiDispatchProtocol = NULL;
  Status = mSmst->SmmLocateProtocol (&gPchEspiSmiDispatchProtocolGuid, NULL, (VOID **) &PchEspiSmiDispatchProtocol);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_WARN, "Locate the PCH eSPI SMI dispatch protocol failure\n"));
  } else {

    //
    // Register an eSPI EC SMI callback function.
    //
    Handle = NULL;
    Status = PchEspiSmiDispatchProtocol->EspiSlaveSmiRegister (
                                       PchEspiSmiDispatchProtocol,
                                       eSpiEcSmiCallback,
                                       &Handle
                                       );
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_WARN, "Register eSPI EC SMI callback function failure\n"));
    }
  }
#endif
// AMI_OVERRIDE_EDN - EIP349813 : Please use a Token/Flag to determine whether EspiSlaveSmi is Registered or not.

  //
  // Register TCO timeout SMI handler
  //
  Status = RegisterTcoTimeoutHandler ();

  return EFI_SUCCESS;
}
