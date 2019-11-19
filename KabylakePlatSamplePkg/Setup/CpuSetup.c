/** @file
  CPU Setup Routines

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2019 Intel Corporation.

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

#include <Library/CmosAccessLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/RngLib.h>
#include <Library/CpuMailboxLib.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/MdeModuleHii.h>
#include <Library/BaseLib.h>
#include "Base.h"
#include <SetupPrivate.h>
#include "CmosMap.h"
#include "Platform.h"
#include <Register/Cpuid.h>
#include <Register/Msr.h>
#include <CpuAccess.h>
#include <Protocol/CpuInfo.h>
#include <Ppi/SiPolicy.h>
#include <Library/MmPciLib.h>
// AMI_OVERRIDE_START - For our Setup variable design.
#include "AmiSetupPrivate.h"
#include "SaSetup.h"
#include <CpuRegs.h>
// AMI_OVERRIDE_END - For our Setup variable design.

//
// More CPU definitions
// @todo remove CPU definitions
//
#define SGX_NO_CHANGE_IN_EPOCH              0
#define SGX_CHANGE_TO_NEW_RANDOM_EPOCH      1
#define SGX_USER_MANUAL_EPOCH               2
#define MAX_NON_TURBO_RATIO_OFFSET          8
#define MAX_NON_TURBO_RATIO_MASK            0xff
#define MAX_EFFICIENCY_RATIO_MASK           0xff
#define PACKAGE_MAX_POWER_OFFSET            32
#define PACKAGE_MIN_POWER_OFFSET            16
#define POWER_LIMIT_1_MASK                  (0x7FFF)  // Bits 14:0
#define MAX_EFFICIENCY_RATIO_OFFSET         40

static EFI_HII_HANDLE     gHiiHandle;

static UINT8              mCoreRatioFinal1     = 0;
static UINT8              mCoreRatioFinal2     = 0;
static UINT8              mCoreRatioFinal3     = 0;
static UINT8              mCoreRatioFinal4     = 0;
static UINT8              mCoreRatioFinal5     = 0;
static UINT8              mCoreRatioFinal6     = 0;
static UINT8              mCoreRatioFinal7     = 0;
static UINT8              mCoreRatioFinal8     = 0;
// AMI_OVERRIDE_START - For our Setup variable design.
static CPU_SETUP                mCpuSetup;
static SETUP_VOLATILE_DATA      mSetupVolatileData;
// AMI_OVERRIDE_END - For our Setup variable design.

typedef struct {
  UINT32  CPUID;
  CHAR8   String[16];
} CPU_REV;

GLOBAL_REMOVE_IF_UNREFERENCED CPU_REV  ProcessorRevisionTable[] = {
  {EnumCpuSklUltUlx    + EnumSklB0,    "B0"},
  {EnumCpuSklUltUlx    + EnumSklC0,    "C0/J0"},
  {EnumCpuSklUltUlx    + EnumSklD0,    "D0/K0"},
  {EnumCpuSklDtHalo    + EnumSklP0,    "P0"},
  {EnumCpuSklDtHalo    + EnumSklQ0,    "Q0"},
  {EnumCpuSklDtHalo    + EnumSklM0,    "M0"},
  {EnumCpuSklDtHalo    + EnumSklR0,    "R0/S0/N0"},
  {EnumCpuKblUltUlxG0  + EnumKblG0,    "G0"},
  {EnumCpuKblUltUlx    + EnumKblH0,    "H0/J0"},
  {EnumCpuKblUltUlx    + EnumKblY0,    "Y0"},
  {EnumCpuKblUltUlx    + EnumAmlW0,    "W0"},
  {EnumCpuKblUltUlx    + EnumAmlV0,    "V0"},
  {EnumCpuKblDtHaloA0  + EnumKblA0,    "A0"},
  {EnumCpuKblDtHalo    + EnumKblB0,    "B0/S0/M0"},
  {EnumCpuKblDtHalo    + EnumKblN0,    "N0/U0"},
  {EnumCpuKblDtHalo    + EnumCflB0,    "B0"},
  {EnumCpuKblDtHalo    + EnumCflP0,    "P0"},
  {EnumCpuKblDtHalo    + EnumCflR0,    "R0"}
};
#if 0  // AMI_OVERRIDE_START - We don't use this variable.
extern UINT8 AdvancedBin[];
#endif // AMI_OVERRIDE_END - We don't use this variable.


/**
  This function change the PRMRR size according to EnableSgx
  setup option.
  If EnableSgx was changed to Enabled, PRMRR size will be 128MB
  Otherwise it will be software controlled ( for
  EnableSgx=Disabled, it will be 0)

  @param[IN]       EFI_FORM_CALLBACK_PROTOCOL   *This
  @param[IN]       UINT16                       KeyValue
  @param[IN]       EFI_IFR_DATA_ARRAY           *Data,
  @param[IN]       EFI_HII_CALLBACK_PACKET      **Packet

  @retval        EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
SgxFormCallBackFunction(
#if 0 // AMI_OVERRIDE_START	- For our Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS			  *CallbackValues
#endif  // AMI_OVERRIDE_END	- For our Callback function design.    
  );

/**
  Update the min, max, and default values for CpuRatio.

  @param[in] CpuRatioDefault      The CPU MNTR default.

  @retval EFI_SUCCESS             Values updated successfully.
  @retval EFI_NOT_FOUND           Failed to update it.
**/
EFI_STATUS
InitCpuMntrDefaultVfr (
  UINT8      CpuRatioDefault
  );

/**
  Returns number of CPU Cores

  @retval UINT8 - Number of CPU Cores.
**/
UINT8 NumCpuCores (
  VOID
  )
{
  UINT64 MsrData = AsmReadMsr64(MSR_CORE_THREAD_COUNT);

  return (UINT8)((UINT32)MsrData >> 16);
}

/**
  Get number of supported threads per core.

  @retval  UINT8- Number of Threads per core.
**/
UINT8 NumSupportedThreadsPerCore (
  VOID
  )
{
  EFI_CPUID_REGISTER    CpuidRegs;
  AsmCpuidEx (0xb, 0, NULL, &CpuidRegs.RegEbx, NULL, NULL);
  return (UINT8)CpuidRegs.RegEbx;
}

/**
  Get number of supported Cpu Cores per package.

  @retval UINT8 - Number of supported Cpu Cores per package.
**/
UINT8 NumSupportedCpuCores (
  VOID
  )
{
  EFI_CPUID_REGISTER    CpuidRegs;
  AsmCpuidEx (0xb, 1, NULL, &CpuidRegs.RegEbx, NULL, NULL);
  return (UINT8)CpuidRegs.RegEbx/NumSupportedThreadsPerCore();
}

/**
  Read the Factory Configured Default Non Turbo Ratio.
  @retval Turbo Ratio
**/
UINT8
MaxNonTurboRatio_Factoryvalue (
  VOID
  )
{
  //
  // MSR 0CE Bit[8-15]
  //
  UINT8   MaxNonTurboRatio;
  UINT64  PlatformInfo;

  PlatformInfo = AsmReadMsr64 (MSR_PLATFORM_INFO);
  MaxNonTurboRatio = (UINT8) (((UINT32) (UINTN) RShiftU64 (PlatformInfo, MAX_NON_TURBO_RATIO_OFFSET)) & MAX_NON_TURBO_RATIO_MASK);
  return MaxNonTurboRatio;
}

/**
  Read the Factory Configured Default MaxEfficiencyRatio (Minimum Ratio)

  @retval Max Efficiency Ratio
**/
UINT8
MaxEfficiencyRatio_factory (
  VOID
  )
{
  //
  // MSR 0CE Bit[40-47]
  //
  UINT8   MaxEfficiencyRatio;
  UINT64  PlatformInfo;

  PlatformInfo        = AsmReadMsr64 (MSR_PLATFORM_INFO);
  MaxEfficiencyRatio  = (UINT8) RShiftU64 (PlatformInfo, MAX_EFFICIENCY_RATIO_OFFSET) & MAX_EFFICIENCY_RATIO_MASK;
  return MaxEfficiencyRatio;
}

/**
  Initialize Turbo Core Ratio defaults.

  @param[in] EFI_EVENT    Event
  @param[in] VOID         *Context
**/
VOID
InitTurboRatioDefault (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS            Status;
  UINT64                CoreRatioLimit;
  UINTN                 VariableSize;
  UINT32                Attributes;

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
          &Attributes,
                  &VariableSize,
                  &mCpuSetup
                  );
  ASSERT_EFI_ERROR (Status);

  if (mCpuSetup.IsTurboRatioDefaultsInitalized == 0) {

    mCpuSetup.IsTurboRatioDefaultsInitalized = 1;
    mCpuSetup.FlexRatioOverrideDefault = MaxNonTurboRatio_Factoryvalue();

    CoreRatioLimit = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);

    mCpuSetup.RatioLimit1Default = (UINT8) (CoreRatioLimit & B_MSR_TURBO_RATIO_LIMIT_1C);
    mCpuSetup.RatioLimit2Default = (UINT8) (RShiftU64(CoreRatioLimit, N_MSR_TURBO_RATIO_LIMIT_2C) & B_MSR_TURBO_RATIO_LIMIT_1C);
    mCpuSetup.RatioLimit3Default = (UINT8) (RShiftU64(CoreRatioLimit, N_MSR_TURBO_RATIO_LIMIT_3C) & B_MSR_TURBO_RATIO_LIMIT_1C);
    mCpuSetup.RatioLimit4Default = (UINT8) (RShiftU64(CoreRatioLimit, N_MSR_TURBO_RATIO_LIMIT_4C) & B_MSR_TURBO_RATIO_LIMIT_1C);
    mCpuSetup.RatioLimit5Default = (UINT8) (RShiftU64(CoreRatioLimit, N_MSR_TURBO_RATIO_LIMIT_5C) & B_MSR_TURBO_RATIO_LIMIT_1C);
    mCpuSetup.RatioLimit6Default = (UINT8) (RShiftU64(CoreRatioLimit, N_MSR_TURBO_RATIO_LIMIT_6C) & B_MSR_TURBO_RATIO_LIMIT_1C);
    mCpuSetup.RatioLimit7Default = (UINT8) (RShiftU64(CoreRatioLimit, N_MSR_TURBO_RATIO_LIMIT_7C) & B_MSR_TURBO_RATIO_LIMIT_1C);
    mCpuSetup.RatioLimit8Default = (UINT8) (RShiftU64(CoreRatioLimit, N_MSR_TURBO_RATIO_LIMIT_8C) & B_MSR_TURBO_RATIO_LIMIT_1C);
    mCpuSetup.RatioLimit1 = mCpuSetup.RatioLimit1Default;
    mCpuSetup.RatioLimit2 = mCpuSetup.RatioLimit2Default;
    mCpuSetup.RatioLimit3 = mCpuSetup.RatioLimit3Default;
    mCpuSetup.RatioLimit4 = mCpuSetup.RatioLimit4Default;
    mCpuSetup.RatioLimit5 = mCpuSetup.RatioLimit5Default;
    mCpuSetup.RatioLimit6 = mCpuSetup.RatioLimit6Default;
    mCpuSetup.RatioLimit7 = mCpuSetup.RatioLimit7Default;
    mCpuSetup.RatioLimit8 = mCpuSetup.RatioLimit8Default;

    Status = gRT->SetVariable (
                    L"CpuSetup",
                    &gCpuSetupVariableGuid,
          Attributes,
                    VariableSize,
                    &mCpuSetup
                    );
    ASSERT_EFI_ERROR (Status);
  }
}

/**
  Initialize AcheckRequest defaults. Always reset to Disable
**/
VOID
InitTxtAcheckDefault (
  VOID
  )
{
  EFI_STATUS            Status;
  UINT32                VariableAttr;
  UINTN                 VariableSize;

  VariableAttr = 0;
  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  &VariableAttr,
                  &VariableSize,
                  &mCpuSetup
                  );
  if (EFI_ERROR (Status)) {
    VariableAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  }
  ASSERT_EFI_ERROR (Status);

  mCpuSetup.AcheckRequest = 0;

  Status = gRT->SetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  VariableAttr,
                  VariableSize,
                  &mCpuSetup
                  );
  ASSERT_EFI_ERROR (Status);
}

/**
  Initialize BIOS Guard defaults.
  BIOS Guard must be disabled if SAF Mode is activated.
**/
VOID
InitBiosGuardDefault (
  VOID
  )
{
  EFI_STATUS            Status;
  UINTN                 VariableSize;
  UINT32                Attributes;
  UINTN                 PchSpiBar0;

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  &Attributes,
                  &VariableSize,
                  &mCpuSetup
                  );
  ASSERT_EFI_ERROR (Status);

  PchSpiBar0 = MmioRead32 (MmPciBase (
                             DEFAULT_PCI_BUS_NUMBER_PCH,
                             PCI_DEVICE_NUMBER_PCH_SPI,
                             PCI_FUNCTION_NUMBER_PCH_SPI)
                             + R_PCH_SPI_BAR0);
  PchSpiBar0 &= ~(B_PCH_SPI_BAR0_MASK);

  if (PchSpiBar0 == 0) {
    DEBUG ((DEBUG_ERROR, "ERROR : PchSpiBar0 is invalid!\n"));
    ASSERT (FALSE);
  }

  mCpuSetup.SafModeDisableBiosGuard = 0;
  if ((MmioRead32 (PchSpiBar0 + R_PCH_SPI_HSFSC) & B_PCH_SPI_HSFSC_SAF_MODE_ACTIVE)) {
    mCpuSetup.SafModeDisableBiosGuard = 1;
    mCpuSetup.BiosGuard = 0;
  }

  Status = gRT->SetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  Attributes,
                  VariableSize,
                  &mCpuSetup
                  );
  ASSERT_EFI_ERROR (Status);
}

/**
  Display the CPU VR menu programmed defaults.
**/
VOID
InitCpuVrConfig (
  VOID
  )
{

  EFI_STATUS                  Status;
  UINT32                      MailboxData;
  UINT32                      MailboxCmd;
  UINT32                      MailboxStatus;
  UINT32                      MailboxType;
  UINT64                      TempAcLoadline;
  UINT64                      TempDcLoadline;
  VR_TOPOLOGY_DATA            VrTopology;
  UINT8                       TempVrAddress;
  UINT8                       SvidEnabled;
  UINTN                       VrIndex;
  UINTN                       VariableSize;
  UINT32                      Attributes;

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  &Attributes,
                  &VariableSize,
                  &mCpuSetup
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "InitCpuVrConfig ()"));
  MailboxType = MAILBOX_TYPE_VR_MSR;

  ///
  /// Get CPU VR topology
  ///
  Status = MailboxRead (MailboxType, READ_VR_STRAP_CONFIG_CMD, (UINT32*)&VrTopology.Fields, &MailboxStatus);
  if (MailboxStatus != PCODE_MAILBOX_CC_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "VR: Error Reading VR topology. EFI_STATUS = %r, Mailbox Status = %X\n", Status, MailboxStatus));
    ///
    /// If VR topology fails, it is pointeless to continue. Return.
    ///
    return;
  }

  ///
  /// Set VR configuration parameters for all VR domains
  /// SA = 0, IA = 1, GTUS = 2, GTS = 3
  ///
  for (VrIndex = SKL_VR_DOMAIN_SA; VrIndex <= SKL_VR_DOMAIN_GTS; VrIndex++) {
    switch (VrIndex) {

      case SKL_VR_DOMAIN_SA:
        TempVrAddress = (UINT8) VrTopology.Fields.VrSaAddress;
        SvidEnabled = (UINT8) (~VrTopology.Fields.VrSaSvidType) & BIT0;
        break;

      case SKL_VR_DOMAIN_IA:
        TempVrAddress = (UINT8) VrTopology.Fields.VrIaAddress;
        SvidEnabled = (UINT8) (~VrTopology.Fields.VrIaSvidType) & BIT0;
        break;

      case SKL_VR_DOMAIN_GTUS:
        TempVrAddress = (UINT8) VrTopology.Fields.VrGtusAddress;
        SvidEnabled = (UINT8) (~VrTopology.Fields.VrGtusSvidType) & BIT0;
        break;

      case SKL_VR_DOMAIN_GTS:
        TempVrAddress = (UINT8) VrTopology.Fields.VrGtsAddress;
        SvidEnabled = (UINT8) (~VrTopology.Fields.VrGtsSvidType) & BIT0;
        break;

      default:
        TempVrAddress = 0;
        SvidEnabled = 0;
        break;
    }

    if (mCpuSetup.VrConfigEnable[VrIndex] == 1 && SvidEnabled == 1) {
      ///
      /// AC / DC Loadline
      ///
      MailboxCmd = READ_ACDC_LOADLINE_CMD | ((TempVrAddress & VR_ADDRESS_MASK) << VR_ADDRESS_OFFSET);
      Status = MailboxRead (MailboxType, MailboxCmd, &MailboxData, &MailboxStatus);
      if (MailboxStatus == PCODE_MAILBOX_CC_SUCCESS) {
        ///
        ///  Loadline is 1/100 mOhm units. Mailbox interface requires Loadline in U-4.20 Ohms format.
        ///  After multiplying by 2^20, we must divide the result by 100,000 to convert to Ohms.
        ///  Adding half of divisor to dividend to account for rounding errors in fixed point arithmetic.
        ///
        ///  Only update if the command was successful
        ///
        TempAcLoadline = (MailboxData & AC_LOADLINE_MASK);
        TempDcLoadline = RShiftU64((MailboxData & DC_LOADLINE_MASK), DC_LOADLINE_OFFSET);

        TempAcLoadline = MultU64x32 (TempAcLoadline, 100000);
        mCpuSetup.AcLoadline[VrIndex] = (UINT16) ((TempAcLoadline + (1 << 19)) >> 20);
        TempDcLoadline = MultU64x32 (TempDcLoadline, 100000);
        mCpuSetup.DcLoadline[VrIndex] = (UINT16) ((TempDcLoadline + (1 << 19)) >> 20);
      } else{
         DEBUG ((DEBUG_ERROR, "VR: Error Reading AC/DC Loadline. EFI_STATUS = %r, Mailbox Status = %X\n", Status, MailboxStatus));
      }

      ///
      /// Icc Max
      ///
      MailboxCmd = READ_VR_ICC_MAX_CMD | ((TempVrAddress & VR_ADDRESS_MASK) << VR_ADDRESS_OFFSET);
      Status = MailboxRead (MailboxType, MailboxCmd, &MailboxData, &MailboxStatus);
      if (MailboxStatus == PCODE_MAILBOX_CC_SUCCESS) {
        ///
        /// Only update if the command was successful
        ///
        mCpuSetup.IccMax[VrIndex] = (UINT16) (MailboxData & 0xFFFF);
      } else {
         DEBUG ((DEBUG_ERROR, "VR: Error Reading ICC Max. EFI_STATUS = %r, Mailbox Status = %X\n", Status, MailboxStatus));
      }

      ///
      /// VR Voltage Limit
      /// -Mailbox Voltage Limit defined as U16.3.13, Range 0-7.999V
      /// -Policy defined in mV, Range 0-7999mV
      /// -Adding half of divisor to dividend to account for rounding errors in fixed point arithmetic.
      ///
      MailboxCmd = READ_VR_VOLTAGE_LIMIT_CMD | ((TempVrAddress & VR_ADDRESS_MASK) << VR_ADDRESS_OFFSET);
      Status = MailboxRead (MailboxType, MailboxCmd, &MailboxData, &MailboxStatus);
      if (MailboxStatus == PCODE_MAILBOX_CC_SUCCESS) {
        ///
        /// Only update if the command was successful
        ///
        mCpuSetup.VrVoltageLimit[VrIndex] = ((MailboxData * 1000 + (1 << 12)) >> 13) & VR_VOLTAGE_LIMIT_MASK;

      } else {
         DEBUG ((DEBUG_ERROR, "VR: Error Reading Vr Voltage limit. EFI_STATUS = %r, Mailbox Status = %X\n", Status, MailboxStatus));
      }

      ///
      /// VR TDC Settings
      /// -Mailbox TDC Current Limit defined as U15.12.3, Range 0-4095A
      ///    -Policy defined in 1/8 A increments
      ///
      MailboxCmd = READ_VR_TDC_CONFIG_CMD | ((TempVrAddress & VR_ADDRESS_MASK) << VR_TDC_ADDRESS_OFFSET);

      Status = MailboxRead (MailboxType, MailboxCmd, &MailboxData, &MailboxStatus);
      if (MailboxStatus == PCODE_MAILBOX_CC_SUCCESS) {
        ///
        /// Only update if the command was successful
        ///
        mCpuSetup.TdcPowerLimit[VrIndex] = (UINT16) ((MailboxData & VR_TDC_CURRENT_LIMIT_MASK));
      } else {
        DEBUG ((DEBUG_ERROR, "VR: Error Reading VR TDC Config. EFI_STATUS = %r, Mailbox Status = %X\n", Status, MailboxStatus));
      }
    }
  }

  Status = gRT->SetVariable (
                L"CpuSetup",
                &gCpuSetupVariableGuid,
                Attributes,
                VariableSize,
                &mCpuSetup
                );

}

/**
  Initialize CPU strings and VR topology.

  @param[in] EFI_EVENT    Event
  @param[in] VOID         *Context
**/
VOID
InitCPUInfo (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS              Status;
  UINTN                   CpuNumber;
  UINTN                   VariableSize;
  UINT8                   Index;
  SETUP_CPU_FEATURES      SetupCpuFeatures;
  CHAR8                   String[15];
  MSR_REGISTER            MicroCodeVersion;
  UINT16                  PowerLimitInteger;
  UINT16                  PowerLimitFraction;
  UINT16                  PowerLimitInteger2;
  UINT16                  PowerLimitFraction2;
  UINT8                   TurboRatio;
  MSR_REGISTER            TempMsr;
  UINT8                   PowerUnit;
  UINT8                   CoreRatio;
  CPU_INFO                *GetCpuInfo;
  CPUID_VERSION_INFO_ECX  Ecx;
  UINT8                   CtdpLevels;
  UINT8                   CtdpRatio;
  UINT16                  CtdpTdp;
  UINT32                  CtdpTar;
  UINT32                  CtdpAddress;
  UINT8                   CpuSku;
  UINT64                  DebugInterfaceStatus;
  CPU_INFO_PROTOCOL       *DxeCpuInfo;
  CPU_STEPPING            CpuSteppingId;
  CPU_FAMILY              CpuFamily;
  CPU_GENERATION          CpuGeneration;
  UINT32                  MailboxStatus;
  UINT32                  CpuSetupVolVarAttr;
  VR_TOPOLOGY_DATA        VrTopology;
  CPU_SETUP_VOLATILE_DATA CpuSetupVolData;
// AMI_OVERRIDE_START - Update CPU setup variable
  EFI_CPUID_REGISTER      CpuidRegs;
// AMI_OVERRIDE_END - Update CPU setup variable


  PowerLimitInteger = PowerLimitFraction = 0;
  TurboRatio                             = 0;
  PowerUnit                              = 0;
  CpuNumber                              = 0;
  GetCpuInfo                             = NULL;
  DebugInterfaceStatus                   = 0;

  VariableSize  = sizeof (SETUP_VOLATILE_DATA);
  Status = gRT->GetVariable (
                  L"SetupVolatileData",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &mSetupVolatileData
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return ;
  }

  ///
  /// Locate DxeCpuInfo protocol instance and gather CPU information
  ///
  Status = gBS->LocateProtocol (&gCpuInfoProtocolGuid, NULL, (VOID **) &DxeCpuInfo);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Failed to locate DxeCpuInfo Protocol\n"));
    return;
  }

  GetCpuInfo               = DxeCpuInfo->CpuInfo;
  GetCpuInfo->BrandString  = DxeCpuInfo->CpuInfo->BrandString;
  GetCpuInfo->CacheInfo    = DxeCpuInfo->CpuInfo->CacheInfo;

  GetCpuInfo->IntendedFreq = (10000 * (((UINT32)AsmReadMsr64(MSR_PLATFORM_INFO) >> MAX_NON_TURBO_RATIO_OFFSET) & MAX_NON_TURBO_RATIO_MASK)) /100;
  GetCpuInfo->Voltage      = 0;    //Voltage is variable, and no information os available.

  ///
  /// Update current Debug interface MSR enable/disable state
  ///
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, &Ecx.Uint32, NULL);
  if (Ecx.Bits.SDBG == 1) {
    DebugInterfaceStatus = AsmReadMsr64(MSR_IA32_DEBUG_INTERFACE) & B_DEBUG_INTERFACE_DEBUG_STATUS;
  }

  if (DebugInterfaceStatus) {
    InitString (
      gHiiHandle,
      STRING_TOKEN (STR_DEBUG_MSR_INTERFACE_STATUS_VALUE),
      L"%a",
      "Enabled"
      );
  } else {
    InitString (
      gHiiHandle,
      STRING_TOKEN (STR_DEBUG_MSR_INTERFACE_STATUS_VALUE),
      L"%a",
      "Disabled"
      );
  }

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_PROCESSOR_VERSION_VALUE),
    L"%a",
    GetCpuInfo->BrandString
    );

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_PROCESSOR_SPEED_VALUE),
    L"%d MHz",
    GetCpuInfo->IntendedFreq
    );

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_PROCESSOR_ID_VALUE),
    L"0x%X",
    GetCpuInfo->CpuSignature
    );

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_PROCESSOR_PACKAGE_VALUE),
    L"%a",
    "Not Implemented Yet"
    );

  //Update the Microcode Revision
  MicroCodeVersion.Qword = AsmReadMsr64(0x8b);
  if(MicroCodeVersion.Dwords.High != 0) {
    InitString(
    gHiiHandle,
    STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE),
    L"%x",
    MicroCodeVersion.Dwords.High         //ignore Lower 32-bits.
    );
  }

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_PROCESSOR_COUNT_VALUE),
    L"%dCore(s) / %dThread(s)",
    GetCpuInfo->NumCores,
    GetCpuInfo->NumCores * GetCpuInfo->NumHts
    );

  for (Index = 0; Index <= GetCpuInfo->MaxCacheSupported; ++Index) {
    switch (GetCpuInfo->CacheInfo[Index].Level) {
    case 1:
      //
      // L1 cache size is per core. Display the size per core times number of cores.
      // If there's only 1 enabled core, simply display the size per core.
      //
      if (GetCpuInfo->CacheInfo[Index].Type == 1) {
        if (GetCpuInfo->NumCores == 1) {
          InitString (
            gHiiHandle,
            STRING_TOKEN (STR_PROCESSOR_L1_DATA_CACHE_VALUE),
            L"%d KB",
            GetCpuInfo->CacheInfo[Index].Size
            );
        } else {
          InitString (
            gHiiHandle,
            STRING_TOKEN (STR_PROCESSOR_L1_DATA_CACHE_VALUE),
            L"%d KB x %d",
            GetCpuInfo->CacheInfo[Index].Size,
            GetCpuInfo->NumCores
            );
        }
      } else if (GetCpuInfo->CacheInfo[Index].Type == 2) {
        if (GetCpuInfo->NumCores == 1) {
          InitString (
            gHiiHandle,
            STRING_TOKEN (STR_PROCESSOR_L1_INSTR_CACHE_VALUE),
            L"%d KB",
            GetCpuInfo->CacheInfo[Index].Size
            );
        } else {
          InitString (
            gHiiHandle,
            STRING_TOKEN (STR_PROCESSOR_L1_INSTR_CACHE_VALUE),
            L"%d KB x %d",
            GetCpuInfo->CacheInfo[Index].Size,
            GetCpuInfo->NumCores
            );
        }
      }
      break;

    case 2:
      //
      // L2 cache size is per core. Display the size per core times number of cores.
      // If there's only 1 enabled core, simply display the size per core.
      //
      if (GetCpuInfo->NumCores == 1) {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_L2_CACHE_VALUE),
          L"%d KB",
          GetCpuInfo->CacheInfo[Index].Size
          );
      } else {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_L2_CACHE_VALUE),
          L"%d KB x %d",
          GetCpuInfo->CacheInfo[Index].Size,
          GetCpuInfo->NumCores
          );
      }
      break;

    case 3:
      //
      // L3 cache size is not per core. It is shared between cores.
      //
      InitString (
        gHiiHandle,
        STRING_TOKEN (STR_PROCESSOR_L3_CACHE_VALUE),
        L"%d MB",
        GetCpuInfo->CacheInfo[Index].Size / 1024
        );
      break;

    case 4:
      InitString (
        gHiiHandle,
        STRING_TOKEN (STR_PROCESSOR_L4_CACHE_VALUE),
        L"%d MB",
        GetCpuInfo->CacheInfo[Index].Size / 1024
        );
      break;
    }
  }

  CpuFamily = GetCpuFamily();
  CpuSku = GetCpuSku();
  CpuSteppingId = GetCpuStepping();
  CpuGeneration = GetCpuGeneration ();

  switch (CpuSku) {
    case EnumCpuUlt:
      if (CpuGeneration == EnumKblCpu) {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_VALUE),
          L"%a",
          "Kabylake ULT"
          );
      } else {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_VALUE),
          L"%a",
          "Skylake ULT"
          );
      }
      break;
    case EnumCpuUlx:
      if (CpuGeneration == EnumKblCpu) {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_VALUE),
          L"%a",
          "Kabylake ULX"
          );
      } else {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_VALUE),
          L"%a",
          "Skylake ULX"
          );
      }
      break;
    case EnumCpuTrad:
      if (CpuGeneration == EnumKblCpu) {
        if (IsCflSCpu ()) {
          InitString (
            gHiiHandle,
            STRING_TOKEN (STR_PROCESSOR_VALUE),
            L"%a",
            "CoffeeLake DT"
            );
        } else {
          InitString (
            gHiiHandle,
            STRING_TOKEN (STR_PROCESSOR_VALUE),
            L"%a",
            "Kabylake DT"
            );
        }
      } else {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_VALUE),
          L"%a",
          "Skylake DT"
          );
      }
      break;
    case EnumCpuHalo:
      if (CpuGeneration == EnumKblCpu) {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_VALUE),
          L"%a",
          "Kabylake Halo"
          );
      } else {
        InitString (
          gHiiHandle,
          STRING_TOKEN (STR_PROCESSOR_VALUE),
          L"%a",
          "Skylake Halo"
          );
      }
      break;

    default:
      InitString (
        gHiiHandle,
        STRING_TOKEN (STR_PROCESSOR_VALUE),
        L"%a",
        "Unknown"
        );
  }

  VariableSize = sizeof (SETUP_CPU_FEATURES);
  Status = gRT->GetVariable (
                  L"SetupCpuFeatures",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupCpuFeatures
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return ;
  }

  if (SetupCpuFeatures.VTAvailable) {
    AsciiSPrint (String, 15, "Supported");
  } else {
    AsciiSPrint (String, 15, "Not Supported");
  }

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_PROCESSOR_VMX_VALUE),
    L"%a",
    String
    );

  if (SetupCpuFeatures.TXTAvailable) {
    AsciiSPrint (String, 15, "Supported");
  } else {
    AsciiSPrint (String, 15, "Not Supported");
  }

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_PROCESSOR_SMX_VALUE),
    L"%a",
    String
    );

  //
  // Package Power SKU Unit
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
  PowerUnit = (UINT8)(TempMsr.Dwords.Low & PACKAGE_POWER_UNIT_MASK);
  PowerUnit = (UINT8) (2 << (PowerUnit-1));

  if (PowerUnit == 0 ) {
    ASSERT (FALSE);
    return;
  }

  //
  // Min and Max Turbo Power Limit
  //
  TempMsr.Qword = AsmReadMsr64(MSR_PACKAGE_POWER_SKU);
  PowerLimitFraction = PowerLimitInteger= (UINT16)RShiftU64((TempMsr.Qword & PACKAGE_MAX_POWER_MASK), PACKAGE_MAX_POWER_OFFSET);

  //
  // 0 for Max Power Limit means no limit, so set the value to 0x7FFF (max value possible)
  //
  if(PowerLimitInteger == 0 ) {
    PowerLimitFraction = PowerLimitInteger= 0x7FFF;
  }

  PowerLimitInteger = PowerLimitInteger / PowerUnit;
  PowerLimitFraction = PowerLimitFraction % PowerUnit;

  // Now convert fractional part into 3 decimal place
  PowerLimitFraction = (PowerLimitFraction * 1000 ) / PowerUnit;

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_MAX_TURBO_POWER_LIMIT_VALUE),
    L"%d.%d",
    PowerLimitInteger,PowerLimitFraction
    );

  PowerLimitFraction = PowerLimitInteger= (UINT16)RShiftU64 ((TempMsr.Qword & PACKAGE_MIN_POWER_MASK), PACKAGE_MIN_POWER_OFFSET );

  PowerLimitInteger = PowerLimitInteger / PowerUnit;
  PowerLimitFraction = PowerLimitFraction % PowerUnit;

  // Now convert fractional part into 3 decimal place
  PowerLimitFraction = (PowerLimitFraction * 1000 ) / PowerUnit;

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_MIN_TURBO_POWER_LIMIT_VALUE),
    L"%d.%d",
    PowerLimitInteger,PowerLimitFraction
    );

  //
  // Package TDP Limit
  //
  TempMsr.Qword = AsmReadMsr64(MSR_PACKAGE_POWER_SKU);
  PowerLimitFraction = PowerLimitInteger= (UINT16)(TempMsr.Dwords.Low & PACKAGE_TDP_POWER_MASK);

  PowerLimitInteger = PowerLimitInteger / PowerUnit;
  PowerLimitFraction = PowerLimitFraction % PowerUnit;

  // Now convert fractional part into 3 decimal place
  PowerLimitFraction = (PowerLimitFraction * 1000 ) / PowerUnit;

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_TDP_LIMIT_VALUE),
    L"%d.%d",
    PowerLimitInteger,PowerLimitFraction
    );

  //
  // Turbo Power Limit
  //
  TempMsr.Qword = AsmReadMsr64(MSR_PACKAGE_POWER_LIMIT);

  //
  // Long duration power limit
  //
  PowerLimitFraction = PowerLimitInteger= (UINT16)(TempMsr.Dwords.Low & POWER_LIMIT_1_MASK);

  PowerLimitInteger = PowerLimitInteger / PowerUnit;
  PowerLimitFraction = PowerLimitFraction % PowerUnit;

  // Now convert fractional part into 3 decimal place
  PowerLimitFraction = (PowerLimitFraction * 1000 ) / PowerUnit;

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_LONG_DUR_PWR_LIMIT_VALUE),
    L"%d.%d",
    PowerLimitInteger,PowerLimitFraction
    );

  //
  // Short duration power limit
  //    Notice, we are using TempMsr.Dwords.High, so reusing Power Limit 1 Mask
  //
  PowerLimitFraction = PowerLimitInteger= (UINT16)(TempMsr.Dwords.High & POWER_LIMIT_1_MASK);

  PowerLimitInteger = PowerLimitInteger / PowerUnit;
  PowerLimitFraction = PowerLimitFraction % PowerUnit;

  // Now convert fractional part into 3 decimal place
  PowerLimitFraction = (PowerLimitFraction * 1000 ) / PowerUnit;

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_SHORT_DUR_PWR_LIMIT_VALUE),
    L"%d.%d",
    PowerLimitInteger,PowerLimitFraction
    );

  //
  // 1C, 2C, 3C, 4C, 5C, 6C, 7C, 8C ratio
  //
  TempMsr.Qword = AsmReadMsr64(MSR_TURBO_RATIO_LIMIT);
  CoreRatio = (UINT8)(TempMsr.Qword & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (mCoreRatioFinal1 == 0) {
    mCoreRatioFinal1 = CoreRatio;
  }

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_1C_RATIO_VALUE),
    L"%d",
    CoreRatio
    );

  CoreRatio = (UINT8)(RShiftU64(TempMsr.Qword, N_MSR_TURBO_RATIO_LIMIT_2C) & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (mCoreRatioFinal2 == 0) {
    mCoreRatioFinal2 = CoreRatio;
  }

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_2C_RATIO_VALUE),
    L"%d",
    CoreRatio
    );

  CoreRatio = (UINT8)(RShiftU64(TempMsr.Qword, N_MSR_TURBO_RATIO_LIMIT_3C) & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (mCoreRatioFinal3 == 0) {
    mCoreRatioFinal3 = CoreRatio;
  }

  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_3C_RATIO_VALUE),
    L"%d",
    CoreRatio
    );

  CoreRatio = (UINT8)(RShiftU64(TempMsr.Qword, N_MSR_TURBO_RATIO_LIMIT_4C) & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (mCoreRatioFinal4 == 0) {
    mCoreRatioFinal4 = CoreRatio;
  }
  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_4C_RATIO_VALUE),
    L"%d",
    CoreRatio
    );

  CoreRatio = (UINT8)(RShiftU64(TempMsr.Qword, N_MSR_TURBO_RATIO_LIMIT_5C) & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (mCoreRatioFinal5 == 0) {
    mCoreRatioFinal5 = CoreRatio;
  }
  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_5C_RATIO_VALUE),
    L"%d",
    CoreRatio
    );

  CoreRatio = (UINT8)(RShiftU64(TempMsr.Qword, N_MSR_TURBO_RATIO_LIMIT_6C) & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (mCoreRatioFinal6 == 0) {
    mCoreRatioFinal6 = CoreRatio;
  }
  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_6C_RATIO_VALUE),
    L"%d",
    CoreRatio
    );

  CoreRatio = (UINT8)(RShiftU64(TempMsr.Qword, N_MSR_TURBO_RATIO_LIMIT_7C) & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (mCoreRatioFinal7 == 0) {
    mCoreRatioFinal7 = CoreRatio;
  }
  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_7C_RATIO_VALUE),
    L"%d",
    CoreRatio
    );

  CoreRatio = (UINT8)(RShiftU64(TempMsr.Qword, N_MSR_TURBO_RATIO_LIMIT_8C) & B_MSR_TURBO_RATIO_LIMIT_1C);
  if (mCoreRatioFinal8 == 0) {
    mCoreRatioFinal8 = CoreRatio;
  }
  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_8C_RATIO_VALUE),
    L"%d",
    CoreRatio
    );

  //
  // ConfigTdp Configurations
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  CoreRatio     = (UINT8) (TempMsr.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_1C);

  //
  // Get the number of configurable TDP Levels supported
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  CtdpLevels  = (UINT8)(RShiftU64 (TempMsr.Qword, 33) & 0x3);
  InitString (
    gHiiHandle,
    STRING_TOKEN (STR_CTDP_LEVELS_VALUE),
    L"%d",
    CtdpLevels + 1
    );

  if (CtdpLevels != 0) {

    //
    // Get Nominal Ratio
    //
    TempMsr.Qword = AsmReadMsr64 (MSR_CONFIG_TDP_NOMINAL);
    CtdpRatio = (UINT8)(TempMsr.Dwords.Low & CONFIG_TDP_NOMINAL_RATIO_MASK);

    TempMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU);
    CtdpTdp   = (UINT16)(TempMsr.Dwords.Low & PACKAGE_TDP_POWER_MASK);

    // Package TDP Limit
    PowerLimitFraction                      = PowerLimitInteger = (CtdpTdp & PACKAGE_TDP_POWER_MASK);
    PowerLimitInteger                       = PowerLimitInteger / PowerUnit;
    PowerLimitFraction                      = PowerLimitFraction % PowerUnit;
    // Now convert fractional part into 3 decimal place
    PowerLimitFraction = (PowerLimitFraction * 1000) / PowerUnit;

    InitString (
      gHiiHandle,
      STRING_TOKEN (STR_CTDP_NOMINAL_VALUE),
      L"Ratio:%d TAR:%d PL1:%d.%dW",
      CtdpRatio,
      CtdpRatio - 1,
      PowerLimitInteger,
      PowerLimitFraction
      );

    //
    // Get Level1 Ratio and TDP
    //
    TempMsr.Qword   = AsmReadMsr64 (MSR_CONFIG_TDP_LVL1);
    CtdpRatio = (UINT8)(RShiftU64 (TempMsr.Qword , 16) & 0xFF);
    CtdpTdp   = (UINT16)(TempMsr.Dwords.Low & PACKAGE_TDP_POWER_MASK);

    PowerLimitFraction                      = PowerLimitInteger = (CtdpTdp & PACKAGE_TDP_POWER_MASK);
    PowerLimitInteger                       = PowerLimitInteger / PowerUnit;
    PowerLimitFraction                      = PowerLimitFraction % PowerUnit;
    // Now convert fractional part into 3 decimal place
    PowerLimitFraction = (PowerLimitFraction * 1000) / PowerUnit;

    InitString (
      gHiiHandle,
      STRING_TOKEN (STR_CTDP_LEVEL1_VALUE),
      L"Ratio:%d TAR:%d PL1:%d.%dW",
      CtdpRatio,
      CtdpRatio - 1,
      PowerLimitInteger,
      PowerLimitFraction
      );

    //
    // Get Level2 Ratio and TDP
    //
    TempMsr.Qword   = AsmReadMsr64 (MSR_CONFIG_TDP_LVL2);
    CtdpRatio = (UINT8)(RShiftU64 (TempMsr.Qword , 16) & 0xFF);
    CtdpTdp   = (UINT16)(TempMsr.Dwords.Low & PACKAGE_TDP_POWER_MASK);

    PowerLimitFraction                      = PowerLimitInteger = (CtdpTdp & PACKAGE_TDP_POWER_MASK);
    PowerLimitInteger                       = PowerLimitInteger / PowerUnit;
    PowerLimitFraction                      = PowerLimitFraction % PowerUnit;
    // Now convert fractional part into 3 decimal place
    PowerLimitFraction = (PowerLimitFraction * 1000) / PowerUnit;

    InitString (
      gHiiHandle,
      STRING_TOKEN (STR_CTDP_LEVEL2_VALUE),
      L"Ratio:%d TAR:%d PL1:%d.%dW",
      CtdpRatio,
      CtdpRatio - 1,
      PowerLimitInteger,
      PowerLimitFraction
      );

    //
    // Get MMIO Power Limit 1
    //
    CtdpAddress = (UINT32)((UINTN) PcdGet64 (PcdMchBaseAddress) + 0x59A0);
    CtdpTdp = (UINT16)((*((UINT32*) ((UINTN) CtdpAddress))) & PACKAGE_TDP_POWER_MASK);

    PowerLimitFraction                      = PowerLimitInteger = (CtdpTdp & PACKAGE_TDP_POWER_MASK);
    PowerLimitInteger                       = PowerLimitInteger / PowerUnit;
    PowerLimitFraction                      = PowerLimitFraction % PowerUnit;
    // Now convert fractional part into 3 decimal place
    PowerLimitFraction = (PowerLimitFraction * 1000) / PowerUnit;

    //
    // Get MSR Power Limit 1
    //
    TempMsr.Qword       = AsmReadMsr64 (0x610);
    CtdpTdp = (UINT16)(TempMsr.Dwords.Low & PACKAGE_TDP_POWER_MASK);
    PowerLimitFraction2                     = PowerLimitInteger2 = (CtdpTdp  & PACKAGE_TDP_POWER_MASK);
    PowerLimitInteger2                      = PowerLimitInteger2 / PowerUnit;
    PowerLimitFraction2                     = PowerLimitFraction2 % PowerUnit;
    // Now convert fractional part into 3 decimal place
    PowerLimitFraction2 = (PowerLimitFraction2 * 1000) / PowerUnit;

    InitString (
      gHiiHandle,
      STRING_TOKEN (STR_CTDP_PWR_LIMIT1_VALUE),
      L"%d.%dW (MSR:%d.%d)",
      PowerLimitInteger,
      PowerLimitFraction,
      PowerLimitInteger2,
      PowerLimitFraction2
      );

    //
    // Get MMIO Power Limit 2
    //
    CtdpAddress = (UINT32)((UINTN) PcdGet64 (PcdMchBaseAddress) + 0x59A4);
    CtdpTdp = (UINT16)((*((UINT32*) ((UINTN) CtdpAddress))) & PACKAGE_TDP_POWER_MASK);

    PowerLimitFraction                      = PowerLimitInteger = (CtdpTdp & PACKAGE_TDP_POWER_MASK);
    PowerLimitInteger                       = PowerLimitInteger / PowerUnit;
    PowerLimitFraction                      = PowerLimitFraction % PowerUnit;
    // Now convert fractional part into 3 decimal place
    PowerLimitFraction = (PowerLimitFraction * 1000) / PowerUnit;

    //
    // Get MSR Power Limit 1
    //
    TempMsr.Qword       = AsmReadMsr64 (0x610);
    CtdpTdp = (UINT16)(TempMsr.Dwords.High & PACKAGE_TDP_POWER_MASK);
    PowerLimitFraction2                     = PowerLimitInteger2 = (CtdpTdp  & PACKAGE_TDP_POWER_MASK);
    PowerLimitInteger2                      = PowerLimitInteger2 / PowerUnit;
    PowerLimitFraction2                     = PowerLimitFraction2 % PowerUnit;
    // Now convert fractional part into 3 decimal place
    PowerLimitFraction2 = (PowerLimitFraction2 * 1000) / PowerUnit;

    InitString (
      gHiiHandle,
      STRING_TOKEN (STR_CTDP_PWR_LIMIT2_VALUE),
      L"%d.%dW (MSR:%d.%d)",
      PowerLimitInteger,
      PowerLimitFraction,
      PowerLimitInteger2,
      PowerLimitFraction2
      );

    //
    // Get TAR value
    //
    TempMsr.Qword   = AsmReadMsr64 (0x64C);
    CtdpTar = (UINT32)(TempMsr.Dwords.Low);
    if (CtdpTar & 0x80000000) {
      InitString (
        gHiiHandle,
        STRING_TOKEN (STR_CTDP_TAR_VALUE),
        L"%d (Locked)",
        CtdpTar & 0xFF
        );
    } else {
      InitString (
        gHiiHandle,
        STRING_TOKEN (STR_CTDP_TAR_VALUE),
        L"%d (Unlocked)",
        CtdpTar & 0xFF
        );
    }
  }

  VariableSize = sizeof (CPU_SETUP_VOLATILE_DATA);
  Status = gRT->GetVariable (
                  L"CpuSetupVolatileData",
                  &gCpuSetupVariableGuid,
                  &CpuSetupVolVarAttr,
                  &VariableSize,
                  &CpuSetupVolData
                  );
  if (EFI_ERROR (Status)) {
    CpuSetupVolVarAttr = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  }
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
  ASSERT_EFI_ERROR (Status);
#endif // AMI_OVERRIDE_END - For our Setup variable design.

  //
  //  Initialize Vr Topology Data
  //
  Status = MailboxRead ( MAILBOX_TYPE_VR_MSR, READ_VR_STRAP_CONFIG_CMD, (UINT32*)&VrTopology.Fields, &MailboxStatus);
  if (MailboxStatus != PCODE_MAILBOX_CC_SUCCESS) {
    DEBUG ((DEBUG_WARN, "Cpu Setup: Error Reading VR topology. EFI_STATUS =%X, Mailbox Status = %X\n", Status, MailboxStatus));
    CpuSetupVolData.CoreVrLocked = 0;
    CpuSetupVolData.SaVrLocked   = 0;
    CpuSetupVolData.GtsVrLocked  = 0;
    CpuSetupVolData.GtusVrLocked = 0;
  } else {
    //
    // Lock the VR submenu if it does not use SVID.
    // VR Type. 1 - No SVID VR, 0 - SVID VR
    //
    CpuSetupVolData.CoreVrLocked = (UINT8)VrTopology.Fields.VrIaSvidType;
    CpuSetupVolData.SaVrLocked   = (UINT8)VrTopology.Fields.VrSaSvidType;
    CpuSetupVolData.GtsVrLocked  = (UINT8)VrTopology.Fields.VrGtsSvidType;
    CpuSetupVolData.GtusVrLocked = (UINT8)VrTopology.Fields.VrGtusSvidType;

    //
    // If Gts and Gtus share the same VR address, lock Gtus
    //
    if (VrTopology.Fields.VrGtsAddress == VrTopology.Fields.VrGtusAddress) {
        CpuSetupVolData.GtusVrLocked = TRUE;
    }
  }
// AMI_OVERRIDE_START - For our Setup variable design.
  AsmCpuid (1, &(CpuidRegs.RegEax), &(CpuidRegs.RegEbx), &(CpuidRegs.RegEcx), &(CpuidRegs.RegEdx));
  CpuSetupVolData.CpuFamilyModel = (UINT16) (CpuidRegs.RegEax) & 0x3FF0;
  CpuSetupVolData.CpuExtendedFamilyModel = (UINT16) (((CpuidRegs.RegEax) >> 16) & 0x0FFF);
  CpuSetupVolData.CpuStepping = (UINT8) (CpuidRegs.RegEax) & 0xF;
  
  CpuSetupVolData.EdramTestModeEnable = FALSE;

  if ((((CpuSetupVolData.CpuExtendedFamilyModel == CPU_EXT_FM_SKL_ULT_ULX) || (CpuSetupVolData.CpuExtendedFamilyModel == CPU_EXT_FM_KBL_ULT_ULX)) &&
      ((CpuSetupVolData.CpuStepping == EnumSklK0) || (CpuSetupVolData.CpuStepping == EnumKblJ0))) ||
      (((CpuSetupVolData.CpuExtendedFamilyModel == CPU_EXT_FM_SKL_DT_HALO) || (CpuSetupVolData.CpuExtendedFamilyModel == CPU_EXT_FM_KBL_DT_HALO)) &&
      ((CpuSetupVolData.CpuStepping == EnumSklN0)))){
    CpuSetupVolData.EdramTestModeEnable = TRUE;
  }

// AMI_OVERRIDE_END - For our Setup variable design.

  Status = gRT->SetVariable (
                  L"CpuSetupVolatileData",
                  &gCpuSetupVariableGuid,
                  CpuSetupVolVarAttr,
                  sizeof (CpuSetupVolData),
                  &CpuSetupVolData
                  );
  ASSERT_EFI_ERROR (Status);
  
// AMI_OVERRIDE_START - For our Setup variable design.
  InitCpuMntrDefault ();
  InitCpuSetupCallback ();
// AMI_OVERRIDE_END - For our Setup variable design.
  
}

/**
  Cpu setup callback

  @param[in] EFI_EVENT    Event
  @param[in] VOID         *Context
**/
#if 0 // AMI_OVERRIDE_START - We don't use this callback function.
VOID
EFIAPI
CpuSetupCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                Status;
  UINTN                     VariableSize;

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &mCpuSetup
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return ;
  }

  CmosWrite8 (CMOS_CPU_RATIO_OFFSET, mCpuSetup.CpuRatio);
  ///
  /// Save to special CMOS to use in early PEI phase
  ///
  CmosWrite8 (CMOS_TXT_REG, mCpuSetup.AcheckRequest);

}
#endif // AMI_OVERRIDE_END - We don't use this callback function.

/**
  Initialize CPU strings.

  @param[in] EFI_HII_HANDLE   HiiHandle
  @param[in] UINT16           Class
**/
VOID
InitCPUStrings (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  )
{
#if 0 // AMI_OVERRIDE_START - We don't use this event.
  EFI_EVENT             SetupNvramCallbackEvt;
  VOID                  *SetupNvramCallbackReg;
#endif // AMI_OVERRIDE_END - We don't use this event.
  EFI_CPUID_REGISTER    CpuidRegs;
  UINT32                CPUID;
  UINTN                 i;
  UINTN                 count;

  if ((Class == MAIN_FORM_SET_CLASS) || (Class == ADVANCED_FORM_SET_CLASS)) {
    DEBUG ((DEBUG_INFO, "<InitCPUStrings>"));
    gHiiHandle  = HiiHandle;

    InitTurboRatioDefault (NULL, NULL);
    InitTxtAcheckDefault ();
    InitCPUInfo (NULL, NULL);
    InitBiosGuardDefault ();

#if 0 // AMI_OVERRIDE_START - We don't use this event.
    SetupNvramCallbackEvt = EfiCreateProtocolNotifyEvent (
                              &gSetupNvramUpdateGuid,
                              TPL_CALLBACK,
                              CpuSetupCallback,
                              NULL,
                              &SetupNvramCallbackReg
                              );
    ASSERT (SetupNvramCallbackEvt != NULL);
#endif // AMI_OVERRIDE_END - We don't use this event.
  }

  if (Class == MAIN_FORM_SET_CLASS) {
    AsmCpuid (CPUID_VERSION_INFO, &(CpuidRegs.RegEax), &(CpuidRegs.RegEbx), &(CpuidRegs.RegEcx), &(CpuidRegs.RegEdx));
    CPUID = CpuidRegs.RegEax & CPUID_FULL_FAMILY_MODEL_STEPPING;

    count = sizeof (ProcessorRevisionTable) / sizeof (ProcessorRevisionTable[0]);
    for (i = 0; i < count; i++) {
      if (CPUID == ProcessorRevisionTable[i].CPUID) {
        InitString (
          HiiHandle,
          STRING_TOKEN (STR_PROCESSOR_STEPPING_VALUE),
          L"%a",
          ProcessorRevisionTable[i].String
          );
        break;
      }
    }
  }
  InitCpuVrConfig (); ///< Display CPU VR config default programmed values.
}

/**
  This function validates the Flex Ratio setup value

  @param[in] EFI_FORM_CALLBACK_PROTOCOL   *This
  @param[in] UINT16                       KeyValue
  @param[in] EFI_IFR_DATA_ARRAY           *Data,
  @param[in] EFI_HII_CALLBACK_PACKET      **Packet

  @retval EFI_SUCCESS        Call Back Function executed successfully
**/
EFI_STATUS
EFIAPI
CpuFormCallBackFunction(
#if 0 // AMI_OVERRIDE_START - For our Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif  // AMI_OVERRIDE_END - For our Callback function design.
  )
{
  CPU_SETUP               *CpuSetup;
  UINT8                   LfmRatio;
  UINT8                   HfmRatio;
  UINT8                   RatioLimitMin;
  UINT8                   OverclockingBins;
  MSR_REGISTER            MsrPlatformInfo;
  MSR_REGISTER            FlexRatioMsr;
  UINTN                   VarSize;
  UINTN                   VariableSize;
  EFI_STATUS              Status;
  EFI_STRING              RequestString = NULL;
// AMI_OVERRIDE_START - For our Callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
  
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
// AMI_OVERRIDE_END - For our Callback function design.

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }
  VarSize  = sizeof (CPU_SETUP);
  CpuSetup = AllocatePool(VarSize);
  ASSERT (CpuSetup != NULL);
  if (CpuSetup == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // GetBrowserData by VarStore Name (Setup)
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
  Status = HiiGetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup);
#else
  Status = HiiGetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CpuSetup);
#endif // AMI_OVERRIDE_END - For our Setup variable design.
  ASSERT_EFI_ERROR(Status);

  MsrPlatformInfo.Qword    = AsmReadMsr64 (MSR_PLATFORM_INFO);
  LfmRatio                 = (UINT8) (RShiftU64 (MsrPlatformInfo.Qword, MAX_EFFICIENCY_RATIO_OFFSET)) & MAX_EFFICIENCY_RATIO_MASK;
  HfmRatio                 = (UINT8) (RShiftU64 (MsrPlatformInfo.Qword, MAX_NON_TURBO_RATIO_OFFSET)) & MAX_NON_TURBO_RATIO_MASK;

  //
  // Save the Max Non Turbo Ratio
  //
  if (HfmRatio > CpuSetup->MaxNonTurboRatio) {
    CpuSetup->MaxNonTurboRatio = HfmRatio;
  } else {
    HfmRatio = CpuSetup->MaxNonTurboRatio;
  }
  FlexRatioMsr.Qword       = AsmReadMsr64 (MSR_FLEX_RATIO);
  OverclockingBins         = (UINT8) RShiftU64 ((FlexRatioMsr.Dwords.Low & B_OVERCLOCKING_BINS), 17);

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &mCpuSetup
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // If Flex Ratio Override is disabled, use the original fused value.
  // Otherwise, use the current Flex Ratio value.
  //
  if (mCpuSetup.CpuRatioOverride == 0) {
    RatioLimitMin = mCpuSetup.FlexRatioOverrideDefault;
  } else {
    RatioLimitMin = mCpuSetup.CpuRatio;
  }

  switch (KeyValue) {
    case KEY_CpuRatioLimit:
      break;

    case KEY_RatioLimit1:
      if (CpuSetup->RatioLimit1 < RatioLimitMin) {
        CpuSetup->RatioLimit1 = mCoreRatioFinal1;
      } else if (OverclockingBins != 0x07) {
        if (CpuSetup->RatioLimit1 > (mCpuSetup.RatioLimit1Default + OverclockingBins)) {
          CpuSetup->RatioLimit1 = mCoreRatioFinal1;
        }
      }
      mCoreRatioFinal1 = CpuSetup->RatioLimit1;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit1), sizeof (CpuSetup->RatioLimit1));
      break;

    case KEY_RatioLimit2:
      if (CpuSetup->RatioLimit2 < RatioLimitMin) {
        CpuSetup->RatioLimit2 = mCoreRatioFinal2;
      } else if (OverclockingBins != 0x07) {
        if (CpuSetup->RatioLimit2 > (mCpuSetup.RatioLimit2Default + OverclockingBins)) {
          CpuSetup->RatioLimit2 = mCoreRatioFinal2;
        }
      }
      mCoreRatioFinal2 = CpuSetup->RatioLimit2;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit2), sizeof (CpuSetup->RatioLimit1));
      break;

    case KEY_RatioLimit3:
      if (CpuSetup->RatioLimit3 < RatioLimitMin) {
        CpuSetup->RatioLimit3 = mCoreRatioFinal3;
      } else if (OverclockingBins != 0x07) {
        if (CpuSetup->RatioLimit3 > (mCpuSetup.RatioLimit3Default + OverclockingBins)) {
          CpuSetup->RatioLimit3 = mCoreRatioFinal3;
        }
      }
      mCoreRatioFinal3 = CpuSetup->RatioLimit3;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit3), sizeof (CpuSetup->RatioLimit1));
      break;

    case KEY_RatioLimit4:
      if (CpuSetup->RatioLimit4 < RatioLimitMin) {
        CpuSetup->RatioLimit4 = mCoreRatioFinal4;
      } else if (OverclockingBins != 0x07) {
        if (CpuSetup->RatioLimit4 > (mCpuSetup.RatioLimit4Default + OverclockingBins)) {
          CpuSetup->RatioLimit4 = mCoreRatioFinal4;
        }
      }
      mCoreRatioFinal4 = CpuSetup->RatioLimit4;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit4), sizeof (CpuSetup->RatioLimit1));
      break;

    case KEY_RatioLimit5:
      if (CpuSetup->RatioLimit5 < RatioLimitMin) {
        CpuSetup->RatioLimit5 = mCoreRatioFinal5;
      } else if (OverclockingBins != 0x07) {
        if (CpuSetup->RatioLimit5 > (mCpuSetup.RatioLimit5Default + OverclockingBins)) {
          CpuSetup->RatioLimit5 = mCoreRatioFinal5;
        }
      }
      mCoreRatioFinal5 = CpuSetup->RatioLimit5;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit5), sizeof (CpuSetup->RatioLimit1));
      break;

   case KEY_RatioLimit6:
      if (CpuSetup->RatioLimit6 < RatioLimitMin) {
        CpuSetup->RatioLimit6 = mCoreRatioFinal6;
      } else if (OverclockingBins != 0x07) {
        if (CpuSetup->RatioLimit6 > (mCpuSetup.RatioLimit6Default + OverclockingBins)) {
          CpuSetup->RatioLimit6 = mCoreRatioFinal6;
        }
      }
      mCoreRatioFinal6 = CpuSetup->RatioLimit6;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit6), sizeof (CpuSetup->RatioLimit1));
      break;

  case KEY_RatioLimit7:
      if (CpuSetup->RatioLimit7 < RatioLimitMin) {
        CpuSetup->RatioLimit7 = mCoreRatioFinal7;
      } else if (OverclockingBins != 0x07) {
        if (CpuSetup->RatioLimit7 > (mCpuSetup.RatioLimit7Default + OverclockingBins)) {
          CpuSetup->RatioLimit7 = mCoreRatioFinal7;
        }
      }
      mCoreRatioFinal7 = CpuSetup->RatioLimit7;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit7), sizeof (CpuSetup->RatioLimit1));
      break;

  case KEY_RatioLimit8:
      if (CpuSetup->RatioLimit8 < RatioLimitMin) {
        CpuSetup->RatioLimit8 = mCoreRatioFinal8;
      } else if (OverclockingBins != 0x07) {
        if (CpuSetup->RatioLimit8 > (mCpuSetup.RatioLimit8Default + OverclockingBins)) {
          CpuSetup->RatioLimit8 = mCoreRatioFinal8;
        }
      }
      mCoreRatioFinal8 = CpuSetup->RatioLimit8;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit8), sizeof (CpuSetup->RatioLimit1));
      break;

  case KEY_MonitorMwaitChange:
      if (CpuSetup->MonitorMwait == 0) {    //If mwait disable, ApIdleManner and ApHandoffManner should change to halt loop.
          CpuSetup->ApIdleManner = 1;   //Halt loop
          CpuSetup->ApHandoffManner = 1;  //Halt loop
      }
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, ApIdleManner), sizeof (CpuSetup->ApIdleManner));
      break;

    default:
      ASSERT(FALSE);
  }

  if (RequestString != NULL) {
    VarSize = sizeof(CPU_SETUP);
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
    Status = HiiSetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup, RequestString);
#else
    Status = HiiSetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CpuSetup, NULL);
#endif // AMI_OVERRIDE_END - For our Setup variable design.
    ASSERT_EFI_ERROR(Status);
    FreePool (RequestString);
  }

  FreePool(CpuSetup);

  return EFI_SUCCESS;
}

/**
  This function resets the dynamic VR data when the VrPowerDelivery is updated

  @param[in] EFI_FORM_CALLBACK_PROTOCOL   *This
  @param[in] UINT16                       KeyValue
  @param[in] EFI_IFR_DATA_ARRAY           *Data,
  @param[in] EFI_HII_CALLBACK_PACKET      **Packet

  @retval EFI_SUCCESS        Call Back Function executed successfully
**/
EFI_STATUS
EFIAPI
CpuVrConfigCallBackFunction(
#if 0 // AMI_OVERRIDE_START - For our Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif // AMI_OVERRIDE_END - For our Callback function design.  
  )
{
  CPU_SETUP               *CpuSetup;
  UINTN                   VarSize;
  UINT32                  Index;
  EFI_STATUS              Status;
  EFI_STRING              RequestString = NULL;
// AMI_OVERRIDE_START - For our Callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
    
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
// AMI_OVERRIDE_END - For our Callback function design.

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  VarSize  = sizeof (CPU_SETUP);
  CpuSetup = AllocatePool(VarSize);
  ASSERT (CpuSetup != NULL);
  if (CpuSetup == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // GetBrowserData by VarStore Name (Setup)
  Status = HiiGetBrowserData (&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup);
  ASSERT_EFI_ERROR(Status);

  switch (KeyValue) {
    case KEY_VrPowerDeliveryChange:
      //
      //  If VrPowerDeliveryDesign Changed, we must revert any dynamic VR initialization
      //  Dynamic settings are VR settings which are part of the VR override table
      //
      for (Index = 0; Index < MAX_NUM_VRS; Index++) {
        //
        //  Set loadline to AUTO
        //
        CpuSetup->AcLoadline[Index] = 0;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, AcLoadline[Index]), sizeof (CpuSetup->AcLoadline[Index]));
        VarSize = sizeof(CPU_SETUP);
        Status = HiiSetBrowserData (&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup, RequestString);
        ASSERT_EFI_ERROR(Status);

        CpuSetup->DcLoadline[Index] = 0;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, DcLoadline[Index]), sizeof (CpuSetup->DcLoadline[Index]));
        Status = HiiSetBrowserData (&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup, RequestString);
        ASSERT_EFI_ERROR(Status);

        //
        //  Set IccMax to AUTO
        //
        CpuSetup->IccMax[Index] = 0;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, IccMax[Index]), sizeof (CpuSetup->IccMax[Index]));
        Status = HiiSetBrowserData (&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup, RequestString);
        ASSERT_EFI_ERROR(Status);

        //
        //  Set VR TDC to AUTO
        //
        CpuSetup->TdcPowerLimit[Index]  = 0;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, TdcPowerLimit[Index]), sizeof (CpuSetup->TdcPowerLimit[Index]));
        Status = HiiSetBrowserData (&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup, RequestString);
        ASSERT_EFI_ERROR(Status);

        //
        //  Set VR Voltage Limit to AUTO
        //
        CpuSetup->VrVoltageLimit[Index] = 0;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, VrVoltageLimit[Index]), sizeof (CpuSetup->VrVoltageLimit[Index]));
        Status = HiiSetBrowserData (&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup, RequestString);
        ASSERT_EFI_ERROR(Status);
      }
      break;

    default:
      ASSERT(FALSE);
  }

  if (RequestString != NULL) {
    FreePool (RequestString);
  }

  FreePool (CpuSetup);

  return EFI_SUCCESS;
}

/**
  This function change the PRMRR size according to EnableSgx
  setup option.
  If EnableSgx was changed to Enabled, PRMRR size will be 128MB
  Otherwise it will be software controlled ( for
  EnableSgx=Disabled, it will be 0)

  @param[IN]       EFI_FORM_CALLBACK_PROTOCOL   *This
  @param[IN]       UINT16                       KeyValue
  @param[IN]       EFI_IFR_DATA_ARRAY           *Data,
  @param[IN]       EFI_HII_CALLBACK_PACKET      **Packet

  @retval        EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
SgxFormCallBackFunction(
#if 0 // AMI_OVERRIDE_START - For our Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 			  *CallbackValues
#endif // AMI_OVERRIDE_END - For our Callback function design.
  )
{
  CPU_SETUP              *CurrentUserSetupData;
  UINTN                   VarSize;
  EFI_STATUS              Status;
  EFI_STRING              RequestString = NULL;
// AMI_OVERRIDE_START - For our Callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
    
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
// AMI_OVERRIDE_END - For our Callback function design.

  ///
  /// No action to perform on Action Changing
  ///
  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  VarSize = sizeof (CPU_SETUP);
  CurrentUserSetupData  = AllocatePool(VarSize);
  ASSERT (CurrentUserSetupData != NULL);

  if (CurrentUserSetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ///
  /// GetBrowserData by VarStore Name (Setup)
  ///
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
  Status = HiiGetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CurrentUserSetupData);
#else
  Status = HiiGetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CurrentUserSetupData);  
#endif // AMI_OVERRIDE_END - For our Setup variable design.
  if (Status != TRUE) {
    return EFI_NOT_FOUND;
  }
  switch (KeyValue) {
    case KEY_SgxChange:

      ///
      /// If EnableSgx is changed to Enable, then PRMRR size will be set to 128MB
      ///
      if (CurrentUserSetupData->EnableSgx == CPU_FEATURE_ENABLE) {
        CurrentUserSetupData->PrmrrSize = SIZE_128MB;
      }

      ///
      /// If EnableSgx is changed to disable or to Software Controlled, PRMRR size is change to 0, which in Software Controlled option
      /// it means the the size is controlled by the software and not by BIOS
      ///
      if (CurrentUserSetupData->EnableSgx == CPU_FEATURE_DISABLE || CurrentUserSetupData->EnableSgx == SGX_SOFTWARE_CONTROL) {
        CurrentUserSetupData->PrmrrSize = CPU_FEATURE_DISABLE;
      }

      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, PrmrrSize), sizeof (CurrentUserSetupData->PrmrrSize));

      if (RequestString != NULL) {
        VarSize = sizeof(CPU_SETUP);
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
        Status = HiiSetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CurrentUserSetupData, RequestString);
#else
        Status = HiiSetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CurrentUserSetupData, NULL);
#endif // AMI_OVERRIDE_END - For our Setup variable design.
        FreePool (CurrentUserSetupData);
        if (Status != TRUE) {
          return EFI_NOT_FOUND;
        }
        FreePool (RequestString);
      }
      Status = EFI_SUCCESS;
      break;
    default:
      Status = EFI_UNSUPPORTED;
  }

  return Status;
}

EFI_STATUS
EFIAPI
SgxEpoch1CallBackFunction(
#if 0 // AMI_OVERRIDE_START - For our Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif // AMI_OVERRIDE_END - For our Callback function design.
  )
{
  CPU_SETUP               *CurrentUserSetupData;
  UINTN                   VarSize;
  EFI_STATUS              Status;
  EFI_STRING              RequestString = NULL;
// AMI_OVERRIDE_START - For our Callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
  
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
// AMI_OVERRIDE_END - For our Callback function design.

  ///
  /// No action to perform on Action Changing
  ///
  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  switch (KeyValue) {
    case KEY_SgxEpoch1:
      VarSize               = sizeof (CPU_SETUP);
      CurrentUserSetupData = AllocatePool (VarSize);
      if (CurrentUserSetupData == NULL) {
        return EFI_NOT_FOUND;
      }

      ///
      /// GetBrowserData by VarStore Name (Setup)
      ///
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
      Status = HiiGetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CurrentUserSetupData);
#else
      Status = HiiGetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CurrentUserSetupData);  
#endif // AMI_OVERRIDE_END - For our Setup variable design.
      if (Status != TRUE) {
        return EFI_NOT_FOUND;
      }

      if (CurrentUserSetupData->ShowEpoch == 2) {
        return EFI_SUCCESS;
      }
    CurrentUserSetupData->ShowEpoch = 2;

      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, ShowEpoch), sizeof (CurrentUserSetupData->ShowEpoch));

      if (RequestString != NULL) {
        VarSize = sizeof(CPU_SETUP);
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
        Status = HiiSetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CurrentUserSetupData, RequestString);
#else
        Status = HiiSetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CurrentUserSetupData, NULL);
#endif // AMI_OVERRIDE_END - For our Setup variable design.
        if (Status != TRUE) {
          FreePool (CurrentUserSetupData);
          return EFI_NOT_FOUND;
        }
        FreePool (RequestString);
      }
      FreePool (CurrentUserSetupData);
      Status = EFI_SUCCESS;
      break;

    default:
      Status = EFI_UNSUPPORTED;
  }
  return Status;
}

/**
  This function update the EPOCHs value

  @param[in] EFI_HII_CONFIG_ACCESS_PROTOCOL   *This
  @param[in] EFI_BROWSER_ACTION               Action
  @param[in] EFI_QUESTION_ID                  KeyValue
  @param[in] UINT8                            Type
  @param[in] EFI_IFR_TYPE_VALUE               *Value
  @param[in] OUT EFI_BROWSER_ACTION_REQUEST   *ActionRequest

  @retval     EFI_SUCCESS                  - EPOCH values successfully changed
  @exceptions Anything Except EFI_SUCCESS  - EPOCH values have not been changed
**/
EFI_STATUS
EFIAPI
EpochChangeCallBackFunction(
#if 0 // AMI_OVERRIDE_START - For our Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif // AMI_OVERRIDE_END - For our Callback function design.  
  )
{
  CPU_SETUP               *CurrentUserSetupData;
  UINTN                   VarSize;
  UINTN                   VariableSize;
  EFI_STATUS              Status;
  EFI_STRING              RequestString;
  CHAR16                  *StringBuffer1;
  CHAR16                  *StringBuffer2;
  CHAR16                  *StringBuffer3;
  EFI_INPUT_KEY           Key;
  UINTN                   StringBuffSize;
  BOOLEAN                 RngSuccess;
  UINT64                  RandomEpoch;
// AMI_OVERRIDE_START - For our Callback function design.
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
    
  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
// AMI_OVERRIDE_END - For our Callback function design.

  RequestString = NULL;
  RngSuccess    = FALSE;
  RandomEpoch   = 0;

  ///
  /// No action to perform on Action Changing
  ///
  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  switch (KeyValue) {
    case KEY_EpochChange:
      VarSize               = sizeof (CPU_SETUP);
      CurrentUserSetupData  = AllocatePool (VarSize);
      if (CurrentUserSetupData == NULL) {
        return EFI_NOT_FOUND;
      }

      ///
      /// GetBrowserData by VarStore Name (Setup)
      ///
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
      Status = HiiGetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CurrentUserSetupData);
#else
      Status = HiiGetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CurrentUserSetupData);  
#endif // AMI_OVERRIDE_END - For our Setup variable design.
      if (Status != TRUE) {
        return EFI_NOT_FOUND;
      }

      /**
        If callback was called with value that is not supported by setup options
        This may happen if EPOCH was changed -> setup change was not saved by f4
        and user selected Continue and 'N', so the changes would not take effect
        In this case the callback will be called again, but EpochUpdate value would be
        invalid
      **/
      if (CurrentUserSetupData->EpochUpdate != SGX_NO_CHANGE_IN_EPOCH &&
          CurrentUserSetupData->EpochUpdate != SGX_CHANGE_TO_NEW_RANDOM_EPOCH &&
          CurrentUserSetupData->EpochUpdate != SGX_USER_MANUAL_EPOCH) {
        return EFI_UNSUPPORTED;
      }

      StringBuffer1 = AllocateZeroPool (200 * sizeof (CHAR16));
      if (StringBuffer1 == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      StringBuffer2 = AllocateZeroPool (200 * sizeof (CHAR16));
      if (StringBuffer2 == NULL) {
        FreePool (StringBuffer1);
        return EFI_OUT_OF_RESOURCES;
      }
      StringBuffer3 = AllocateZeroPool (200 * sizeof (CHAR16));
      if (StringBuffer3 == NULL) {
        FreePool (StringBuffer1);
        FreePool (StringBuffer2);
        return EFI_OUT_OF_RESOURCES;
      }

      StringBuffSize = (200 * sizeof (CHAR16)) / sizeof (CHAR16);
      StrCpyS (StringBuffer1, StringBuffSize, L"Warning: EPOCH change!!!");
      StrCpyS (StringBuffer2, StringBuffSize, L"All persistent data protected by Intel(R) Software Guard Extensions Technology will be lost");
      StrCpyS (StringBuffer3, StringBuffSize, L"Press 'Y' if you wish to continue or press any other key otherwise");

      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_RED,&Key, StringBuffer1,StringBuffer2,StringBuffer3, NULL);

      ///
      /// If user selected to not change EPOCH value
      ///
      if (Key.UnicodeChar != L'Y' && Key.UnicodeChar != L'y') {
        FreePool (StringBuffer1);
        FreePool (StringBuffer2);
        FreePool (StringBuffer3);

        VariableSize = sizeof (CPU_SETUP);
        Status = gRT->GetVariable (
                        L"CpuSetup",
                        &gCpuSetupVariableGuid,
                        NULL,
                        &VariableSize,
                        &mCpuSetup
                        );
        if (Status != EFI_SUCCESS) {
          return Status;
        }

        ///
        /// If User changed the EPOCH type selection setup option but decided that he doesn't want to
        /// continue with changing the current EPOCH values
        ///
        if (CurrentUserSetupData->EpochUpdate != mCpuSetup.EpochUpdate) {
          ///
          /// Need to return the EPOCH type selection setup option to its previous state
          ///
          CurrentUserSetupData->EpochUpdate = mCpuSetup.EpochUpdate;
          VarSize = sizeof(CPU_SETUP);
          RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, EpochUpdate), sizeof (mCpuSetup.EpochUpdate));
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
          Status = HiiSetBrowserData (&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CurrentUserSetupData, RequestString);
#else
          Status = HiiSetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CurrentUserSetupData, NULL);
#endif // AMI_OVERRIDE_END - For our Setup variable design.
          if (Status != TRUE) {
            FreePool (CurrentUserSetupData);
            return EFI_NOT_FOUND;
          }
        }
        FreePool (CurrentUserSetupData);
        return EFI_UNSUPPORTED;
      }

      ///
      /// It reports that EPOCH key has been changed
      /// Make the EPOCHs available for SGX_CHANGE_TO_NEW_RANDOM_EPOCH and SGX_USER_MANUAL_EPOCH
      ///

      if (CurrentUserSetupData->EpochUpdate == SGX_CHANGE_TO_NEW_RANDOM_EPOCH) {
        ///
        /// Generate two new 64bit random numbers and replace the previous Epoch values.
        ///
        RngSuccess = GetRandomNumber64 (&RandomEpoch);
        if (RngSuccess) {
          CurrentUserSetupData->SgxEpoch0 = RandomEpoch;
          DEBUG ((DEBUG_INFO, "SgxEpoch0: %016llx\n", CurrentUserSetupData->SgxEpoch0));
        } else {
          DEBUG ((DEBUG_ERROR, "SgxEpoch0 - Failed to get a new 64-bit random number!\n"));
          ASSERT (!RngSuccess);
          return EFI_NOT_FOUND;
        }

        RngSuccess = GetRandomNumber64 (&RandomEpoch);
        if (RngSuccess) {
          CurrentUserSetupData->SgxEpoch1 = RandomEpoch;
          DEBUG ((DEBUG_INFO, "SgxEpoch1: %016llx\n", CurrentUserSetupData->SgxEpoch1));
        } else {
          DEBUG ((DEBUG_ERROR, "SgxEpoch1 - Failed to get a new 64-bit random number!\n"));
          ASSERT (!RngSuccess);
          return EFI_NOT_FOUND;
        }

        ///
        /// After generating two new Epoch values, set "Select EPOCH input type" back to 'No Change in Owner EPOCHs'
        ///
        CurrentUserSetupData->EpochUpdate = SGX_NO_CHANGE_IN_EPOCH;
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, EpochUpdate), sizeof (CurrentUserSetupData->EpochUpdate));
      }

      if (CurrentUserSetupData->EpochUpdate == SGX_USER_MANUAL_EPOCH) {
        CurrentUserSetupData->SgxEpoch0 = 0;
        CurrentUserSetupData->SgxEpoch1 = 0;
        CurrentUserSetupData->ShowEpoch = 1;
        StrCpyS (StringBuffer1, StringBuffSize, L"Please make sure you write down the EPOCH values");
        StrCpyS (StringBuffer2, StringBuffSize, L"They will not be visible after exiting setup options");
        StrCpyS (StringBuffer3, StringBuffSize, L"Press any key to continue");

        CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
                     &Key,
                     StringBuffer1,
                     StringBuffer2,
                     StringBuffer3,
                     NULL);

        FreePool (StringBuffer1);
        FreePool (StringBuffer2);
        FreePool (StringBuffer3);

      }

      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, SgxEpoch0), sizeof (CurrentUserSetupData->SgxEpoch0));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, SgxEpoch1), sizeof (CurrentUserSetupData->SgxEpoch1));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, ShowEpoch), sizeof (CurrentUserSetupData->ShowEpoch));

      if (RequestString != NULL) {
        VarSize = sizeof(CPU_SETUP);
#if 0  // AMI_OVERRIDE_START - For our Setup variable design.
        Status = HiiSetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CurrentUserSetupData, RequestString);
#else
        Status = HiiSetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CurrentUserSetupData, NULL);
#endif // AMI_OVERRIDE_END - For our Setup variable design.
        if (Status != TRUE) {
          FreePool (CurrentUserSetupData);
          return EFI_NOT_FOUND;
        }
        FreePool (RequestString);
      }
      FreePool (CurrentUserSetupData);
      Status = EFI_SUCCESS;
      break;

    default:
      Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Initial CPU MNTR default in SETUP variable and VFR.

  @retval EFI_SUCCESS             The initialization is done.
  @retval EFI_NOT_FOUND           Failed to initial CPU MNTR.
**/
EFI_STATUS
EFIAPI
InitCpuMntrDefault (
  VOID
  )
{
  EFI_STATUS    Status;
  UINTN         VariableSize;
  UINT32        Attributes;

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
          &Attributes,
                  &VariableSize,
                  &mCpuSetup
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (mCpuSetup.CpuRatioOverride == 0) {
    mCpuSetup.CpuDefaultRatio = MaxNonTurboRatio_Factoryvalue();
  }
  mCpuSetup.CpuRatio = MaxNonTurboRatio_Factoryvalue();

  Status = gRT->SetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
          Attributes,
                  VariableSize,
                  &mCpuSetup
                  );
#if 0  // AMI_OVERRIDE_START - We don't use this function.
  Status = InitCpuMntrDefaultVfr (mCpuSetup.CpuDefaultRatio);
#endif // AMI_OVERRIDE_END - We don't use this function.

  return EFI_SUCCESS;
}

#if 0 // AMI_OVERRIDE_START - We don't use this function.

/**
  Update the min, max, and default values for CpuRatio.

  @param[in] CpuRatioDefault      The CPU MNTR default.

  @retval EFI_SUCCESS             Values updated successfully.
  @retval EFI_NOT_FOUND           Failed to update it.
**/
EFI_STATUS
InitCpuMntrDefaultVfr (
  UINT8      CpuRatioDefault
  )
{
  EFI_HII_PACKAGE_HEADER *PackageHdr;
  CHAR8                  *BytePtr;
  EFI_IFR_OP_HEADER      *ParentIfr;
  EFI_IFR_GUID_LABEL     *LabelPtr;
  EFI_IFR_DEFAULT        *DefaultPtr;
  UINTN                  PackageSize;
  EFI_IFR_NUMERIC        *NumericPtr;

  //
  // add Arrary Length (UINT32) to piont to package header.
  //
  PackageHdr = (EFI_HII_PACKAGE_HEADER *)(AdvancedBin + sizeof(UINT32));
  PackageSize = PackageHdr->Length;
  PackageSize = PackageSize & 0x00ffffff;
  BytePtr = (CHAR8 *) (PackageHdr + 1);

  //
  // loop to find CPU RATIO lebel.
  //
  while (TRUE) {
    if (BytePtr >= (CHAR8 *)PackageHdr + PackageSize) {
      DEBUG ((DEBUG_ERROR, "Couldn't find the target node to patch.\n"));
      return EFI_NOT_FOUND;
    }

    ParentIfr = (EFI_IFR_OP_HEADER *)BytePtr;
    if (ParentIfr->OpCode == EFI_IFR_GUID_OP) {
      LabelPtr = (EFI_IFR_GUID_LABEL *)ParentIfr;
      if (CompareGuid ((EFI_GUID *)(VOID *)&LabelPtr->Guid, &gEfiIfrTianoGuid) && (LabelPtr->Number == LABEL_CPU_RATIO )) {
        BytePtr += sizeof (EFI_IFR_GUID_LABEL);
        break;
      }
    }
    BytePtr += ParentIfr->Length;
  }

  //
  // loop to find CPU ratio numberic OP code.
  //
  while (TRUE) {
    if (BytePtr >= (CHAR8 *)PackageHdr + PackageSize) {
      DEBUG ((DEBUG_ERROR, "Couldn't find the target node to patch.\n"));
      return EFI_NOT_FOUND;
    }

    ParentIfr = (EFI_IFR_OP_HEADER *)BytePtr;
    if (ParentIfr->OpCode == EFI_IFR_NUMERIC_OP) {
      NumericPtr = (EFI_IFR_NUMERIC *)ParentIfr;
      NumericPtr->data.u8.MinValue = MaxEfficiencyRatio_factory();
      NumericPtr->data.u8.MaxValue = CpuRatioDefault;
      break;
    }
    BytePtr += ParentIfr->Length;
  }

  //
  // advance to numberic default OP code.
  //
  while (TRUE) {
    if (BytePtr >= (CHAR8 *)PackageHdr + PackageSize) {
      DEBUG ((DEBUG_ERROR, "Couldn't find the target node to patch.\n"));
      return EFI_NOT_FOUND;
    }

    ParentIfr = (EFI_IFR_OP_HEADER *)BytePtr;
    if (ParentIfr->OpCode == EFI_IFR_DEFAULT_OP) {
      DefaultPtr = (EFI_IFR_DEFAULT *)ParentIfr;
      break;
    }
    BytePtr += ParentIfr->Length;
  }

  //
  // Now the Default value is found. Patch it!
  //
  DefaultPtr->Value.u8 = CpuRatioDefault;
  return EFI_SUCCESS;
}
#endif // AMI_OVERRIDE_END - We don't use this function.


// AMI_OVERRIDE_START - For our Setup callback function design.
VOID
InitCpuSetupCallback (
  VOID
  )
{
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit1, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit2, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit3, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit4, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit5, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit6, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit7, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit8, CpuFormCallBackFunction);  
  AmiSetupRegisterCallbackHandler (KEY_CpuRatioLimit, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_SgxChange, SgxFormCallBackFunction);  
  AmiSetupRegisterCallbackHandler (KEY_EpochChange, EpochChangeCallBackFunction); 
  AmiSetupRegisterCallbackHandler (KEY_SgxEpoch1, SgxEpoch1CallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_MonitorMwaitChange, CpuFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_VrPowerDeliveryChange, CpuVrConfigCallBackFunction);
}
// AMI_OVERRIDE_END - For our Setup callback function design.
