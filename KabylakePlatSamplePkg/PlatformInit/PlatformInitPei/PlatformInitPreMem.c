/** @file
  Source code file for Platform Init Pre-Memory PEI module

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2019 Intel Corporation.

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

#include <Base.h>
#ifndef MINTREE_FLAG
#include <CmosMap.h>
#endif //MINTREE_FLAG
#include <OemSetup.h>
#include <PchAccess.h>
#include <SetupVariable.h>
#include <PlatformBoardId.h>
#include <SaPolicyCommon.h>
#include <CpuAccess.h>
#include <Platform.h>
#include <IndustryStandard/Pci30.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Library/EcMiscLib.h>
#include <Library/EcCommands.h>
#include <Library/MmPciLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiPlatformHookLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchPmcLib.h>
#include <Library/MultiPlatSupportLib.h>
#include <Library/PeiPolicyInitLib.h>
#include <Library/MtrrLib.h>
#include <Ppi/Smbus2.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/Capsule.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/Reset.h>
#include <PlatformInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/TcoWdtHob.h>
#include <Guid/MemoryTypeInformation.h>
#include <Uefi/UefiInternalFormRepresentation.h>
#include <Library/PeiPlatformLib.h>
#include <Include/Library/ConfigBlockLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PchSerialIoLib.h>
#include <Library/GpioExpanderLib.h>
#include <Library/BoardConfigLib.h>
#include <IndustryStandard/SmBios.h>
#include <FirwmareConfigurations.h>
#include <Library/BoardConfigLib.h>
#include <PlatformBoardConfig.h>
#include <Board/BoardHook.h>
#include <Library/DebugPrintErrorLevelLib.h>
//AMI_OVERRIDE_START - Take care by AMI module
#include <SbPeiLib.h> 
//AMI_OVERRIDE_END - Take care by AMI module
//AMI_OVERRIDE_START - Install PPI after Gpio Config done
#include <SbGpioConfigDonePpi.h>
//AMI_OVERRIDE_END - Install PPI after Gpio Config done

#ifndef MINTREE_FLAG
#include <Library/CmosAccessLib.h>
#include <Library/PeiPlatformRecoveryLib.h>
#include <Ppi/BootInNonS3Mode.h>
#include <BootState.h>
#include <SimpleBootFlag.h>
#include <FastBootExceptionInfoHob.h>
#include <FastBootFunctionEnabledHob.h>
#include <Library/PeiWdtAppLib.h>
#include <Library/BiosIdLib.h>
#include <Library/SetupDataCacheLib.h>
/// If the debugger branch trace/data store debug feature is enabled
/// This is an optional debug feature as enabling it will introduce
/// significant changes in performance and system resources
///
#include "DsAllocation.h" //must be include for release build
#include <MeChipset.h>
#endif //MINTREE_FLAG

//ray_override / [TAG-SupportDVIEdidLess] Spec. Changed : Support DVI EDID Less Feature / Hot-plug Internal Pull High / Added >>
#include <Library/GpioLib.h>

static GPIO_INIT_CONFIG DVI_1_HotPlug_GpioTermWpu20K[] =
{
  {GPIO_SKL_H_GPP_I1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K }}, //DDSP_HPD_1
};

static GPIO_INIT_CONFIG DVI_1_HotPlug_GpioTermNone[] =
{
  {GPIO_SKL_H_GPP_I1,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //DDSP_HPD_1
};

static GPIO_INIT_CONFIG DVI_2_HotPlug_GpioTermWpu20K[] =
{
  {GPIO_SKL_H_GPP_I0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermWpu20K }}, //DDSP_HPD_0
};

static GPIO_INIT_CONFIG DVI_2_HotPlug_GpioTermNone[] =
{
  {GPIO_SKL_H_GPP_I0,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirIn      , GpioOutDefault, GpioIntDefault  , GpioPlatformReset , GpioTermNone }}, //DDSP_HPD_0
};
//ray_override / [TAG-SupportDVIEdidLess] Spec. Changed : Support DVI EDID Less Feature / Hot-plug Internal Pull High / Added <<

extern EFI_GUID   gOsProfileGuid;

typedef struct {
  EFI_PHYSICAL_ADDRESS    BaseAddress;
  UINT64                  Length;
} MEMORY_MAP;

typedef struct {
  UINT8 SerialDebug;
  UINT8 SerialDebugBaudRate;
  UINT8 RamDebugInterface;
  UINT8 UartDebugInterface;
  UINT8 Usb2DebugInterface;
  UINT8 Usb3DebugInterface;
  UINT8 SerialIoDebugInterface;
  UINT8 TraceHubDebugInterface;
} DEBUG_CONFIG_DATA;

#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND  1
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND  1000
#endif

#define FAST_BOOT_OR_MASK       0x01
#define BOOT_SPECIAL_MASK       0x80

#define FAST_BOOT_WDT_RESET_SIGNATURE   0xFB

///
/// Reset Generator I/O Port
///
#define RESET_GENERATOR_PORT           0xCF9

///
/// Wake Event Types
///
#define EC_GETWAKE_STATUS    0x35
#define EC_CLEARWAKE_STATUS  0x36
#define EC_EXTRA_IO_PORT_1   0x06A0

#define  MEPciD22F0RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (0 << 12))
#define  MEPciD22F1RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (1 << 12))
#define  MEPciD22F2RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (2 << 12))
#define  MEPciD22F3RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (3 << 12))
#define  MEPciD22F4RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (4 << 12))

///
/// Priority of our boot modes, highest priority first
///
GLOBAL_REMOVE_IF_UNREFERENCED EFI_BOOT_MODE mBootModePriority[] = {
  BOOT_IN_RECOVERY_MODE,
  BOOT_WITH_DEFAULT_SETTINGS,
  BOOT_ON_FLASH_UPDATE,
  BOOT_ON_S2_RESUME,
  BOOT_ON_S3_RESUME,
  BOOT_ON_S4_RESUME,
  BOOT_WITH_MINIMAL_CONFIGURATION,
  BOOT_ASSUMING_NO_CONFIGURATION_CHANGES,
  BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS,
  BOOT_WITH_FULL_CONFIGURATION,
  BOOT_ON_S5_RESUME,
  BOOT_SPECIAL_MASK
};

//AMI_OVERRIDE_START - Take care by AMI module
#if 0
GLOBAL_REMOVE_IF_UNREFERENCED MEMORY_MAP MmioMap [] = {
  {FixedPcdGet64 (PcdPciExpressBaseAddress), FixedPcdGet32 (PcdPciExpressRegionLength)},
  {FixedPcdGet64 (PcdApicLocalAddress), FixedPcdGet32 (PcdApicLocalMmioSize)},
  {FixedPcdGet64 (PcdMchBaseAddress), FixedPcdGet32 (PcdMchMmioSize)},
  {FixedPcdGet64 (PcdDmiBaseAddress), FixedPcdGet32 (PcdDmiMmioSize)},
  {FixedPcdGet64 (PcdEpBaseAddress), FixedPcdGet32 (PcdEpMmioSize)},
  {FixedPcdGet64 (PcdGdxcBaseAddress), FixedPcdGet32 (PcdGdxcMmioSize)}
};
#endif
//AMI_OVERRIDE_END - Take care by AMI module

EFI_STATUS
EFIAPI
PlatformInitPreMem (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN VOID                        *Ppi
  );

VOID
TimerInit (
  VOID
  );
#ifndef MINTREE_FLAG
EFI_STATUS
EFIAPI
Smbus2IsReadyCallback (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN VOID                        *Ppi
  );
#endif //MINTREE_FLAG

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN VOID                        *Ppi
  );

EFI_STATUS
EFIAPI
SiPreMemPolicyPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

#ifndef MINTREE_FLAG
EFI_STATUS
EFIAPI
WdtAppPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
PlatformVoltageInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

#endif // MINTREE_FLAG

EFI_STATUS
EFIAPI
PlatformInitPreMemUpdateBootMode (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  );

EFI_STATUS
EFIAPI
PchReset (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );

//AMI_OVERRIDE_START - For updating Board ID when EC disable
#ifndef AMI_CRB_EC_SUPPORT_FLAG
EFI_STATUS
CreateBoardPrivateData (
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  );
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - For updating Board ID when EC disable

static EFI_PEI_NOTIFY_DESCRIPTOR mPreMemNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) PlatformInitPreMem
};

static EFI_PEI_NOTIFY_DESCRIPTOR mMemDiscoveredNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) MemoryDiscoveredPpiNotifyCallback
};

static EFI_PEI_NOTIFY_DESCRIPTOR mSiPreMemPolicyNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gSiPreMemPolicyPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) SiPreMemPolicyPpiNotifyCallback
};

#ifndef MINTREE_FLAG
static EFI_PEI_NOTIFY_DESCRIPTOR mWdtPpiNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gWdtPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) WdtAppPpiNotifyCallback
};

static EFI_PEI_NOTIFY_DESCRIPTOR mEfiPeiSmbus2PpiNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiSmbus2PpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) PlatformVoltageInit
};

//AMI_OVERRIDE_START - Take care by AMI module
#if 0
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID mEfiPeiBootInNonS3ModePpiGuid = EFI_PEI_BOOT_IN_NON_S3_MODE_PPI;

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_PPI_DESCRIPTOR mPpiListNonS3BootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &mEfiPeiBootInNonS3ModePpiGuid,
  NULL
};
#endif 
//AMI_OVERRIDE_END - Take care by AMI module
#endif // MINTREE_FLAG

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_PPI_DESCRIPTOR mPpiListRecoveryBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};

//AMI_OVERRIDE_START - Take care by AMI module
#if 0
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_PPI_DESCRIPTOR mPpiBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  NULL
};

#ifdef CAPSULE_FLAG
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR mBootModeNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiCapsulePpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) PlatformInitPreMemUpdateBootMode
};
#endif // CAPSULE_FLAG
#endif
//AMI_OVERRIDE_END - Take care by AMI module

static EFI_PEI_RESET_PPI mResetPpi = {
  PchReset
};

static EFI_PEI_PPI_DESCRIPTOR mPreMemPpiList[] = {
//AMI_OVERRIDE_START - Install PPI after Gpio Config done
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gAmiSbGpioConfigDonePpiGuid,
    NULL
  },
//AMI_OVERRIDE_END - Install PPI after Gpio Config done
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI| EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiResetPpiGuid,
    &mResetPpi
  }
};

BOOLEAN
GetSleepTypeAfterWakeup (
  OUT UINT32 *SleepType
  );

EFI_STATUS
EFIAPI
PeiGuidForward (
  VOID
  );

#if 0 //AMI_OVERRIDE_START - Take care by AMI module
#ifndef MINTREE_FLAG
/**
  CMOS test to see if it is bad
**/
BOOLEAN
IsCmosBad (
  VOID
  )
{
  UINT8               Nmi;
  volatile UINT32     Data32;

  Nmi     = 0;
  Data32  = 0;

  ///
  /// Preserve NMI bet setting
  ///
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~0, B_PCH_PCR_ITSS_GIC_AME);
  ///
  /// GIC read back is done in PchPcr lib
  ///
  Nmi    = IoRead8 ((UINT64) CMOS_ADDR_PORT) & 0x80;
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~B_PCH_PCR_ITSS_GIC_AME, 0);

  if (CmosRead8 ((UINT8) (CMOS_BAD_REG | Nmi)) & (BIT6 + BIT7)) {
    return TRUE;
  } else {
    return FALSE;
  }
}


/**
  Clear CMOS 0x0E
**/
VOID
ClearCmos0E (
  VOID
  )
{
  UINT8               Value;
  UINT8               Nmi;
  volatile UINT32     Data32;

  Nmi     = 0;
  Data32  = 0;
  ///
  /// Preserve NMI bit setting
  ///
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~0, B_PCH_PCR_ITSS_GIC_AME);
  ///
  /// GIC read back is done in PchPcr lib
  ///
  Nmi    = IoRead8 ((UINT64) CMOS_ADDR_PORT) & 0x80;
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~B_PCH_PCR_ITSS_GIC_AME, 0);

  Value = CmosRead8 ((UINT8) (CMOS_BAD_REG | Nmi));
  CmosWrite8 ((UINT8) (CMOS_BAD_REG | Nmi), (Value & 0x3F));
}
#endif //MINTREE_FLAG
#endif //AMI_OVERRIDE_END - Take care by AMI module

/**
  Sync the debug variable into Cmos offset 0x4C to update the debug level.
**/
VOID
UpdateDebugLevel (
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices
  )
{
  EFI_STATUS         Status;
  DEBUG_CONFIG_DATA  DebugConfigData;
  UINT32             DebugPrintErrorLevel;
  UINTN              DebugVariableSize;

  DebugVariableSize = sizeof (DEBUG_CONFIG_DATA);
  Status = VariableServices->GetVariable (
             VariableServices,
             L"DebugConfigData",
             &gDebugConfigVariableGuid,
             NULL,
             &DebugVariableSize,
             &DebugConfigData
           );
  if (EFI_ERROR (Status)) {
    SetDebugPrintErrorLevel ( DEBUG_ERROR | DEBUG_WARN | DEBUG_INFO | DEBUG_LOAD );
    DEBUG ((DEBUG_ERROR, "Failed to get Debug Configuration data variable!\n"));
    DEBUG ((DEBUG_ERROR, "| Debug error level now updata to - Level : Default |\n"));
  } else {
    switch (DebugConfigData.SerialDebug) {
      case 0:
        DebugPrintErrorLevel = 0;
        DEBUG ((DEBUG_ERROR, "| Debug error level now updata to - Level : 0 |\n"));
        break;
      case 1:
        DebugPrintErrorLevel = DEBUG_ERROR;
        break;
      case 2:
        DebugPrintErrorLevel = DEBUG_ERROR | DEBUG_WARN;
        break;
      case 4:
        DebugPrintErrorLevel = DEBUG_ERROR | DEBUG_WARN | DEBUG_INFO | DEBUG_EVENT | DEBUG_LOAD;
        break;
      case 5:
        DebugPrintErrorLevel = DEBUG_ERROR | DEBUG_WARN | DEBUG_INFO | DEBUG_VERBOSE | DEBUG_LOAD;
        break;
      default:
        DebugPrintErrorLevel = DEBUG_ERROR | DEBUG_WARN | DEBUG_INFO | DEBUG_LOAD;
        break;
    }
    SetDebugPrintErrorLevel (DebugPrintErrorLevel);
    DebugPrintErrorLevel = DebugConfigData.SerialDebug + 0x30;
    DEBUG ((DEBUG_ERROR, "| Debug error level now updata to - Level : %s |\n",
           (DebugConfigData.SerialDebug  > 5) ? L"Default" :
           (DebugConfigData.SerialDebug == 3) ? L"Default" : (EFI_STRING)&DebugPrintErrorLevel));
  }
}

VOID
PrintVariableData (
  IN UINT8   *Data8,
  IN UINTN   DataSize
  )
{
  UINTN      Index;

  for (Index = 0; Index < DataSize; Index++) {
    if (Index % 0x10 == 0) {
      DEBUG ((DEBUG_INFO, "\n%08X:", Index));
    }
    DEBUG ((DEBUG_INFO, " %02X", *Data8++));
  }
  DEBUG ((DEBUG_INFO, "\n"));
}

/**
  Perform the default variable initializations after variable service is ready.

  @param[in]  PeiServices  Pointer to PEI Services Table.
  @param[in]  NotifyDesc   Pointer to the descriptor for the Notification event that
                           caused this function to execute.
  @param[in]  Interface    Pointer to the PPI data associated with this function.

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
EFIAPI
PlatformVariableInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  )
{
//AMI_OVERRIDE_START - Take care by AMI setup
#if 0
  EFI_STATUS                      Status;
  SETUP_DATA                      Setup;
  UINTN                           DataSize;
#ifndef MINTREE_FLAG
  SA_SETUP                        SaSetup;
  ME_SETUP                        MeSetup;
  CPU_SETUP                       CpuSetup;
  PCH_SETUP                       PchSetup;
  UINTN                           SaDataSize;
  UINTN                           MeDataSize;
  UINTN                           CpuDataSize;
  UINTN                           PchDataSize;
#endif // MINTREE_FLAG
#endif
//AMI_OVERRIDE_END - Take care by AMI setup
//AMI_OVERRIDE_START - AMI did not use
#if 0
  OS_PROFILE                      OsProfileData;
  UINTN                           OsProfileSize;
  UINT8                           OsProfile;
  UINT8                           PrevOs;
#endif
//AMI_OVERRIDE_END - AMI did not use
  UINT32                          Attributes = 0;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  EFI_PEI_HOB_POINTERS            HobList;
  PLATFORM_INFO                   *PlatformInfo;
//AMI_OVERRIDE_START - Take care by AMI module
#if 0
  EFI_BOOT_MODE                   BootMode;
#endif
//AMI_OVERRIDE_END - Take care by AMI module

  VariableServices = (EFI_PEI_READ_ONLY_VARIABLE2_PPI *) Interface;

  ///
  /// Find PlatformInfo HOB
  ///
  PeiServicesGetHobList ((VOID **) &HobList.Raw);
  PlatformInfo = NULL;
  HobList.Raw = GetNextGuidHob (&gPlatformInfoHobGuid, HobList.Raw);
  if (HobList.Raw == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = (PLATFORM_INFO *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
  if (NULL == PlatformInfo) {
    return EFI_NOT_FOUND;
  }

  ///
  /// Check whether OS defaults need to be loaded
  ///
  DEBUG ((DEBUG_INFO, "Check if OsDefaults need to be loaded \n"));

//AMI_OVERRIDE_START - AMI did not use
#if 0
  OsProfileSize = sizeof (OS_PROFILE);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"OsProfile",
                               &gOsProfileGuid,
                               &Attributes,
                               &OsProfileSize,
                               &OsProfileData
                               );

  DEBUG ((DEBUG_INFO, "Get OsProfile variable, Status = %r, Attributes = %X \n", Status, Attributes));

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "OS_PROFILE not available yet, do nothing \n" ));
    OsProfile = 0;
    PrevOs = 0;
  } else {
    OsProfile = OsProfileData.OsProfile;
    PrevOs = OsProfileData.PrevOs;
    DEBUG ((DEBUG_INFO, "OsProfile = %X, PrevOs = %X \n", OsProfile, PrevOs));
  }

#ifndef MINTREE_FLAG
  ///
  /// CMOS battery is ok. Check whether the CMOS content is corrupted.
  ///
  if (IsCmosBad ()) {
    DEBUG ((DEBUG_INFO, "CMOS battery is healthy but CMOS content is bad. Reset the SMOS 0Eh and the Setup variable.\n"));
    ClearCmos0E ();
    return CreateBoardDefaultVariableHob (EFI_HII_DEFAULT_CLASS_STANDARD, OsProfile);
  }
#endif //MINTREE_FLAG

  ///
  /// Check BootMode on Recovery boot or Boot with Default settings.
  ///
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
#ifndef MINTREE_FLAG
  if (BootMode == BOOT_IN_RECOVERY_MODE || BootMode == BOOT_WITH_DEFAULT_SETTINGS) {
    return CreateBoardDefaultVariableHob (EFI_HII_DEFAULT_CLASS_STANDARD, OsProfile);
  }
#endif //MINTREE_FLAG

  ///
  /// Check whether Setup Variable does exist to know the first boot or not.
  ///
  DataSize = sizeof (SETUP_DATA);
  Status  = VariableServices->GetVariable (VariableServices, L"Setup", &gSetupVariableGuid, NULL, &DataSize, &Setup);

#ifndef MINTREE_FLAG
  SaDataSize = sizeof (SA_SETUP);
  if (Status != EFI_NOT_FOUND) {
    Status = VariableServices->GetVariable (VariableServices, L"SaSetup", &gSaSetupVariableGuid, NULL, &SaDataSize, &SaSetup);
  }

  MeDataSize = sizeof (ME_SETUP);
  if (Status != EFI_NOT_FOUND) {
    Status = VariableServices->GetVariable (VariableServices, L"MeSetup", &gMeSetupVariableGuid, NULL, &MeDataSize, &MeSetup);
  }

  CpuDataSize = sizeof (CPU_SETUP);
  if (Status != EFI_NOT_FOUND) {
    Status = VariableServices->GetVariable (VariableServices, L"CpuSetup", &gCpuSetupVariableGuid, NULL, &CpuDataSize, &CpuSetup);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  PchDataSize = sizeof (PCH_SETUP);
  if (Status != EFI_NOT_FOUND) {
    Status = VariableServices->GetVariable (VariableServices, L"PchSetup", &gPchSetupVariableGuid, NULL, &PchDataSize, &PchSetup);
  }
#endif // MINTREE_FLAG

  ///
  /// Setup variable is not found. So, set the default setting.
  ///
  if (Status == EFI_NOT_FOUND || (OsProfile != PrevOs)) {
    Status = CreateBoardDefaultVariableHob (EFI_HII_DEFAULT_CLASS_STANDARD, OsProfile);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = VariableServices->GetVariable (VariableServices, L"Setup", &gSetupVariableGuid, NULL, &DataSize, &Setup);
    ASSERT_EFI_ERROR (Status);

#ifndef MINTREE_FLAG
    Status = VariableServices->GetVariable (VariableServices, L"SaSetup", &gSaSetupVariableGuid, NULL, &SaDataSize, &SaSetup);
    ASSERT_EFI_ERROR (Status);

    Status = VariableServices->GetVariable (VariableServices, L"MeSetup", &gMeSetupVariableGuid, NULL, &MeDataSize, &MeSetup);
    ASSERT_EFI_ERROR (Status);

    Status = VariableServices->GetVariable (VariableServices, L"CpuSetup", &gCpuSetupVariableGuid, NULL, &CpuDataSize, &CpuSetup);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = VariableServices->GetVariable (VariableServices, L"PchSetup", &gPchSetupVariableGuid, NULL, &PchDataSize, &PchSetup);
    ASSERT_EFI_ERROR (Status);
#endif // MINTREE_FLAG

    DEBUG_CODE_BEGIN();
      ///
      /// Print default Setup variable.
      ///
      DEBUG ((DEBUG_INFO, "\nDumping SETUP_DATA:"));
      PrintVariableData ((UINT8 *) &Setup, DataSize);

#ifndef MINTREE_FLAG
      DEBUG ((DEBUG_INFO, "\nDumping SA_SETUP:"));
      PrintVariableData ((UINT8 *) &SaSetup, SaDataSize);

      DEBUG ((DEBUG_INFO, "\nDumping CPU_SETUP:"));
      PrintVariableData ((UINT8 *) &CpuSetup, CpuDataSize);

      DEBUG ((DEBUG_INFO, "\nDumping ME_SETUP:"));
      PrintVariableData ((UINT8 *) &MeSetup, MeDataSize);

      DEBUG ((DEBUG_INFO, "\nDumping PCH_SETUP:"));
      PrintVariableData ((UINT8 *) &PchSetup, PchDataSize);
#endif // MINTREE_FLAG

    DEBUG_CODE_END();
  }
#endif
//AMI_OVERRIDE_END - AMI did not use

  return EFI_SUCCESS;
}


/**
  Program timer 1 as refresh timer
**/
VOID
TimerInit (
  VOID
  )
{
  IoWrite8 (0x43, 0x54);
  IoWrite8 (0x41, 0x12);
}


/**
  Set Cache Mtrr.
**/
VOID
SetCacheMtrr (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_HOB_POINTERS        Hob;
  MTRR_SETTINGS               MtrrSetting;
  UINT64                      MemoryBase;
  UINT64                      MemoryLength;
  UINT64                      LowMemoryLength;
  UINT64                      HighMemoryLength;
  EFI_BOOT_MODE               BootMode;
  EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttribute;
  UINT64                      CacheMemoryLength;
  UINTN                       FlashRegionAddress;
  UINTN                       FlashRegionSize;

  // change from 7M to 8M to reduce MTRR numbers
  FlashRegionSize = (UINTN) ((FixedPcdGet32 (PcdFlashAreaSize) + 0x7fffff) & 0xFF800000);
  FlashRegionAddress = (UINTN) (0x100000000ULL - FlashRegionSize);

  ///
  /// Reset all MTRR setting.
  ///
  ZeroMem(&MtrrSetting, sizeof(MTRR_SETTINGS));

  ///
  /// Cache the Flash area as WP to boost performance
  ///
  Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                FlashRegionAddress,
                FlashRegionSize,
                CacheWriteProtected
                );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Update MTRR setting from MTRR buffer for Flash Region to be WP to boost performance
  ///
  MtrrSetAllMtrrs (&MtrrSetting);

  ///
  /// Set low to 1 MB. Since 1MB cacheability will always be set
  /// until override by CSM.
  /// Initialize high memory to 0.
  ///
  LowMemoryLength   = 0x100000;
  HighMemoryLength  = 0;
  ResourceAttribute = (
                       EFI_RESOURCE_ATTRIBUTE_PRESENT |
                       EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                       EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                       EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                       EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                       EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
                       );

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode != BOOT_ON_S3_RESUME) {
    ResourceAttribute |= EFI_RESOURCE_ATTRIBUTE_TESTED;
  }

  Status = PeiServicesGetHobList ((VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) ||
          ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_RESERVED) &&
           (Hob.ResourceDescriptor->ResourceAttribute == ResourceAttribute))
         ) {
        if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000000ULL) {
          HighMemoryLength += Hob.ResourceDescriptor->ResourceLength;
        } else if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000) {
          LowMemoryLength += Hob.ResourceDescriptor->ResourceLength;
        }
      }
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  DEBUG ((DEBUG_INFO, "Memory Length (Below 4GB) = %lx.\n", LowMemoryLength));
  DEBUG ((DEBUG_INFO, "Memory Length (Above 4GB) = %lx.\n", HighMemoryLength));

  ///
  /// Assume size of main memory is multiple of 256MB
  ///
  MemoryLength = (LowMemoryLength + 0xFFFFFFF) & 0xF0000000;
  MemoryBase = 0;

  CacheMemoryLength = MemoryLength;

//AMI_OVERRIDE_START - Provide every MTRR setting as write back. This change is for preventing 
//                     MtrrSetMemoryAttributeInMtrrSettings function use uncacheable to describe memory. 
//                     If it occurred, flash area will be override as uncacheable memory type, then post time will increase. 
#if 0
  ///
  /// Programming MTRRs to avoid override SPI region with UC when MAX TOLUD Length >= 3.5GB
  ///
  if (MemoryLength > 0xDC000000) {
     CacheMemoryLength = 0xC0000000;
     Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                MemoryBase,
                CacheMemoryLength,
                CacheWriteBack
                );
     ASSERT_EFI_ERROR (Status);

     MemoryBase = 0xC0000000;
     CacheMemoryLength = MemoryLength - 0xC0000000;
     if (MemoryLength > 0xE0000000) {
        CacheMemoryLength = 0x20000000;
        Status = MtrrSetMemoryAttributeInMtrrSettings (
                 &MtrrSetting,
                 MemoryBase,
                 CacheMemoryLength,
                 CacheWriteBack
                 );
        ASSERT_EFI_ERROR (Status);

        MemoryBase = 0xE0000000;
        CacheMemoryLength = MemoryLength - 0xE0000000;
     }
  }

  Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                MemoryBase,
                CacheMemoryLength,
                CacheWriteBack
                );
  ASSERT_EFI_ERROR (Status);
#endif

  {
    UINT64 MemoryLength1;
    UINT64 PowerTwo;
    MemoryLength1 = MemoryLength;
    for ( PowerTwo = GetPowerOfTwo64(MemoryLength1); MemoryLength1 > 0; PowerTwo = GetPowerOfTwo64(MemoryLength1) ) {
      Status = MtrrSetMemoryAttributeInMtrrSettings (
                 &MtrrSetting,
                 MemoryBase,
                 PowerTwo,
                 CacheWriteBack
                 );
      ASSERT_EFI_ERROR (Status);
      MemoryBase += PowerTwo;
      MemoryLength1 -= PowerTwo;
    }
  }
//AMI_OVERRIDE_END - Provide every MTRR setting as write back. This change is for preventing 
//                   MtrrSetMemoryAttributeInMtrrSettings function use uncacheable to describe memory. 
//                   If it occurred, flash area will be override as uncacheable memory type, then post time will increase. 

  if (LowMemoryLength != MemoryLength) {
     MemoryBase = LowMemoryLength;
     MemoryLength -= LowMemoryLength;
     Status = MtrrSetMemoryAttributeInMtrrSettings (
                   &MtrrSetting,
                   MemoryBase,
                   MemoryLength,
                   CacheUncacheable
                   );
      ASSERT_EFI_ERROR (Status);
  }

  ///
  /// VGA-MMIO - 0xA0000 to 0xC0000 to be UC
  ///
  Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                0xA0000,
                0x20000,
                CacheUncacheable
                );
  ASSERT_EFI_ERROR (Status);
  
// AMI_OVERRIDE_START >>> EIP340801 : Set 0xC0000 to 0x100000 to be WP.
  Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                0xC0000,
                0x40000,
                CacheWriteProtected
                );
  ASSERT_EFI_ERROR ( Status);  
// AMI_OVERRIDE_END <<< EIP340801 : Set 0xC0000 to 0x100000 to be WP.

  ///
  /// Update MTRR setting from MTRR buffer
  ///
  MtrrSetAllMtrrs (&MtrrSetting);

  return ;
}


/**
  Install Firmware Volume Hob's once there is main memory

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor  Notify that this module published.
  @param[in]  Ppi               PPI that was installed.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                    Status;
  EFI_BOOT_MODE                 BootMode;
  UINTN                         Index;
  UINT8                         PhysicalAddressBits;
  UINT32                        RegEax;

  Index = 0;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  SetCacheMtrr ();

  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  } else {
    PhysicalAddressBits = 36;
  }

  ///
  /// Create a CPU hand-off information
  ///
  BuildCpuHob (PhysicalAddressBits, 16);

//AMI_OVERRIDE_START - Take care by AMI module  
#if 0
  ///
  /// If S3 resume, then we are done
  ///
  if (BootMode == BOOT_ON_S3_RESUME) {
    ///
    /// Install FvRecovery3 for S3Resume2Pei
    ///
    PeiServicesInstallFvInfo2Ppi (
      &(((EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FixedPcdGet32 (PcdFlashFvRecovery3Base))->FileSystemGuid),
      (VOID *) (UINTN) FixedPcdGet32 (PcdFlashFvRecovery3Base),
      FixedPcdGet32 (PcdFlashFvRecovery3Size),
      NULL,
      NULL,
      0
      );
    return EFI_SUCCESS;
  }
  ///
  /// Build HOB for DXE
  ///
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    ///
    /// Prepare the recovery service
    ///
#ifndef MINTREE_FLAG
    Status = InitializeRecovery ();
    ASSERT_EFI_ERROR (Status);
#endif // MINTREE_FLAG
  } else {

    PeiServicesInstallFvInfo2Ppi (
      &(((EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FixedPcdGet32 (PcdFlashFvRecovery3Base))->FileSystemGuid),
      (VOID *) (UINTN) FixedPcdGet32 (PcdFlashFvRecovery3Base),
      FixedPcdGet32 (PcdFlashFvRecovery3Size),
      NULL,
      NULL,
      0
      );

    PeiServicesInstallFvInfo2Ppi (
      &(((EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FixedPcdGet32 (PcdFlashFvMainBase))->FileSystemGuid),
      (VOID *) (UINTN) FixedPcdGet32 (PcdFlashFvMainBase),
      FixedPcdGet32 (PcdFlashFvMainSize),
      NULL,
      NULL,
      0
      );

  }

  ///
  /// Build Memory Mapped IO Resource which is used to build E820 Table in LegacyBios.
  ///
  for (Index = 0; Index < sizeof (MmioMap) / (sizeof (MEMORY_MAP)); Index++) {
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_MAPPED_IO,
      (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
      MmioMap[Index].BaseAddress,
      MmioMap[Index].Length
      );
    BuildMemoryAllocationHob (
      MmioMap[Index].BaseAddress,
      MmioMap[Index].Length,
      EfiMemoryMappedIO
      );
  }

  //
  // Report resource HOB for flash FV
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    (UINTN) FixedPcdGet32 (PcdFlashAreaBaseAddress),
    (UINTN) FixedPcdGet32 (PcdFlashAreaSize)
    );
  BuildMemoryAllocationHob (
    (UINTN) FixedPcdGet32 (PcdFlashAreaBaseAddress),
    (UINTN) FixedPcdGet32 (PcdFlashAreaSize),
    EfiMemoryMappedIO
    );

  BuildFvHob (
    (UINTN) FixedPcdGet32 (PcdFlashAreaBaseAddress),
    (UINTN) FixedPcdGet32 (PcdFlashAreaSize)
    );
#ifndef MINTREE_FLAG
  GetCsmControlStatus ();
#endif //MINTREE_FLAG
#endif
//AMI_OVERRIDE_END - Take care by AMI module
  return Status;
}

/**
  Get sleep type after wakeup

  @param[out] SleepType  Sleep type to be returned.

  @retval     TRUE       A wake event occured without power failure.
  @retval     FALSE      Power failure occured or not a wakeup.
**/
BOOLEAN
GetSleepTypeAfterWakeup (
  OUT UINT32            *SleepType
  )
{
  UINT16                Pm1Sts;
  UINT32                Pm1Cnt;
  UINTN                 PmcBaseAddress;

  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );

  ///
  /// Read the ACPI registers
  ///
  Pm1Sts  = IoRead16 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_STS);
  Pm1Cnt  = IoRead32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_CNT);

  ///
  /// Get sleep type if a wake event occurred and there is no power failure and reset
  ///
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0) {
    if ((MmioRead16 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B) & (B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS | B_PCH_PMC_GEN_PMCON_B_PWR_FLR)) == 0)
    {
      *SleepType = Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP;
      return  TRUE;
    }
    else {
      ///
      /// Clear Wake Status (WAK_STS) and Sleep Type (SLP_TYP)
      ///
      IoWrite16 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_WAK);
      Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SLP_TYP;
      IoWrite32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
      return  FALSE;
    }
  }
  return  FALSE;
}
#ifndef MINTREE_FLAG
/**
  Check fast boot is enabled or not

  @retval  FastBootEnabledStatus  TRUE means fast boot is enabled
                                  FALSE means fast boot is disabled
**/
//AMI_OVERRIDE_START - Take care by AMI module
#if 0
BOOLEAN
EFIAPI
IsFastBootEnabled (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiReadOnlyVarPpi;
  UINTN                           VarSize;
  SETUP_DATA                      SystemConfiguration;
  BOOLEAN                         FastBootEnabledStatus;

  FastBootEnabledStatus = FALSE;
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &PeiReadOnlyVarPpi
             );
  if (Status == EFI_SUCCESS) {
    VarSize = sizeof (SETUP_DATA);
    Status = PeiReadOnlyVarPpi->GetVariable (
                                  PeiReadOnlyVarPpi,
                                  L"Setup",
                                  &gSetupVariableGuid,
                                  NULL,
                                  &VarSize,
                                  &SystemConfiguration
                                  );
    if (Status == EFI_SUCCESS) {
      if (SystemConfiguration.FastBoot != 0) {
        FastBootEnabledStatus = TRUE;
      }
    }
  }

  return FastBootEnabledStatus;
}


/**
  Check Fast Boot 30-second WDT (TCO Timer) timeout has occurred

  @retval  TRUE   Timeout has occurred
  @retval  FALSE  Timeout didnt occur
**/
BOOLEAN
IsTimeOutOccurred (
  VOID
  )
{
  if (IoRead8 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO_WDCNT) == FAST_BOOT_WDT_RESET_SIGNATURE) {
    IoWrite8 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO_WDCNT, 0);
    DEBUG ((DEBUG_INFO, "WDT occurred in previous boot\n"));
    return TRUE;
  }

  DEBUG ((DEBUG_INFO, "WDT did not occur in previous boot\n"));
  return FALSE;
}


/**
  Check if this system boot is due to 4sec power button override has occurred

  @retval  TRUE   Power Button Override occurred in last system boot
  @retval  FALSE  Power Button Override didnt occur
**/
BOOLEAN
IsPowerButtonOverride (
  VOID
  )
{
  if ((IoRead16 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_PRBTNOR) != 0) {
    DEBUG ((DEBUG_INFO, "Power Button Override occurred in last system boot\n"));
    return TRUE;
  }

  DEBUG ((DEBUG_INFO, "Power Button Override did not occur\n"));
  return FALSE;
}
#endif
//AMI_OVERRIDE_END - Take care by AMI module
#endif

//AMI_OVERRIDE_START - AMI did not use
#if 0
#ifndef MINTREE_FLAG
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4090)
#endif

/**
  Check CMOS register bit to determine if previous boot was successful

  @retval  TRUE   Previous Boot was success
  @retval  FALSE  Previous Boot wasn't success
**/
BOOLEAN
IsPreviousBootSuccessful (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT8                           PreviousBootCompletionFlag;
  BOOLEAN                         BootState;
  UINTN                           DataSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVar;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &PeiVar
             );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Get last Boot State Variable to confirm that it is not a first boot.
  ///
  DataSize = sizeof (BOOLEAN);
  Status = PeiVar->GetVariable (
                     PeiVar,
                     BOOT_STATE_VARIABLE_NAME,
                     &gBootStateGuid,
                     NULL,
                     &DataSize,
                     &BootState
                     );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  PreviousBootCompletionFlag = 0;

  PreviousBootCompletionFlag = CmosRead8 (CMOS_FAST_BOOT_REG);
  PreviousBootCompletionFlag &= FAST_BOOT_OR_MASK;

  if(PreviousBootCompletionFlag){
    DEBUG ((DEBUG_INFO, "Previous Boot was not successful to boot to OS\n"));
    return FALSE;
  }
  DEBUG ((DEBUG_INFO, "Previous boot cycle successfully completed handover to OS\n"));
  return TRUE;
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
#endif //MINTREE_FLAG
#endif
//AMI_OVERRIDE_END - AMI did not use

/**
  Query PCH to determine if Intrusion Detection set

  @retval  TRUE   No Change
  @retavl  FALSE  Change
**/
BOOLEAN
IsBootWithNoChange (
  VOID
  )
{
//AMI_OVERRIDE_START - AMI did not use
#if 0
  EFI_STATUS                      Status;
#endif
//AMI_OVERRIDE_END - AMI did not use
  UINT16                          TcoBase;
  UINT16                          Tco2Status;
  BOOLEAN                         BoxOpen;
//AMI_OVERRIDE_START - AMI did not use
#if 0
  BOOLEAN                         BootState;
  UINTN                           DataSize; // AdvancedFeaturesContent
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVar;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &PeiVar
             );
  ASSERT_EFI_ERROR (Status);
//
// AdvancedFeaturesBegin
//
  ///
  /// Get last Boot State Variable From NVRAM
  ///
  DataSize = sizeof (BOOLEAN);
  Status = PeiVar->GetVariable (
                     PeiVar,
                     BOOT_STATE_VARIABLE_NAME,
                     &gBootStateGuid,
                     NULL,
                     &DataSize,
                     &BootState
                     );
  if (EFI_ERROR (Status)) {
//
// AdvancedFeaturesEnd
//
    //
    // If there's no variable, just report the state of the hardware
    //
    BootState = TRUE;
  }// AdvancedFeaturesContent
#endif
//AMI_OVERRIDE_END - AMI did not use

  ///
  /// Read the TCO registers
  ///
  TcoBase     = (UINT16)PcdGet16 (PcdTcoBaseAddress);
  Tco2Status  = IoRead16 (TcoBase + R_PCH_TCO2_STS);

  ///
  /// This is the state of the hardware
  ///
  BoxOpen = (BOOLEAN) (Tco2Status & B_PCH_TCO2_STS_INTRD_DET);
  if (BoxOpen) {
    ///
    /// Clear the bit for next boot.
    ///
    Tco2Status |= B_PCH_TCO2_STS_INTRD_DET;
    IoWrite16 (TcoBase + R_PCH_TCO2_STS, Tco2Status);

    ///
    /// Since it was OPEN, return that it cannot be in "no config. change boot"
    ///
    DEBUG ((DEBUG_INFO, "Boot with Full configuration\n"));
    return FALSE;
//AMI_OVERRIDE_START - AMI did not use
#if 0
  } else {
    ///
    /// Since it was CLOSED, return that can be "no config. change boot"
    /// Need variable to know that memory was tested AT LEAST one time!
    ///
    if (!BootState) {
      DEBUG ((DEBUG_INFO, "Boot assuming no configuration changes\n"));
      return TRUE;
    } else {
      DEBUG ((DEBUG_INFO, "Boot with Full configuration\n"));
      return FALSE;
    }
#endif
  }
  return TRUE;    
//AMI_OVERRIDE_END - AMI did not use  
}

//AMI_OVERRIDE_START - AMI did not use
#if 0

/**
  Given the current boot mode, and a proposed new boot mode, determine
  which has priority. If the new boot mode has higher priority, then
  make it the current boot mode.

  @param[in][out] CurrentBootMode pointer to current planned boot mode
  @param[in][out] NewBootMode     proposed boot mode

  @retval         EFI_NOT_FOUND   if either boot mode is not recognized
  @retval         EFI_SUCCESS     if both boot mode values were
                                  recognized and were processed.
**/
EFI_STATUS
PrioritizeBootMode (
  IN OUT EFI_BOOT_MODE    *CurrentBootMode,
  IN EFI_BOOT_MODE        NewBootMode
  )
{
  UINT32 CurrentIndex;
  UINT32 NewIndex;

  ///
  /// Find the position of the current boot mode in our priority array
  ///
  for ( CurrentIndex = 0;
        CurrentIndex < sizeof (mBootModePriority) / sizeof (mBootModePriority[0]);
        CurrentIndex++) {
    if (mBootModePriority[CurrentIndex] == *CurrentBootMode) {
      break;
    }
  }
  if (CurrentIndex >= sizeof (mBootModePriority) / sizeof (mBootModePriority[0])) {
    return EFI_NOT_FOUND;
  }
  ///
  /// Find the position of the new boot mode in our priority array
  ///
  for ( NewIndex = 0;
        NewIndex < sizeof (mBootModePriority) / sizeof (mBootModePriority[0]);
        NewIndex++) {
    if (mBootModePriority[NewIndex] == NewBootMode) {
      ///
      /// If this new boot mode occurs before the current boot mode in the
      /// priority array, then take it.
      ///
      if (NewIndex < CurrentIndex) {
        *CurrentBootMode = NewBootMode;
      }
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

#ifndef MINTREE_FLAG
/**
  Check if the value is of ODD parity.

  @param[in]  Value  the 8 bits value

  @retval     TRUE   the value is of ODD parity
  @retval     FALSE  the value is not of ODD parity
**/
BOOLEAN
CheckOddParity (
  IN  UINT8 Value
  )
{
  UINT8                           OddNum;
  UINT8                           Index;

  OddNum = 0;

  for (Index = 0; Index < 8; Index++) {
    if (Value & (1 << Index)) {
      OddNum++;
    }
  }

  return (BOOLEAN) (OddNum % 2 != 0);
}

/**
  Get simple boot flag

  @retval  LegacyValue
  @retval  EfiValue
**/
UINT8
GetSimpleBootFlag (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVar;
  UINTN                           BufferSize;
  UINT8                           EfiValue;
  UINT8                           LegacyValue;

  ///
  /// Read EFI variable
  ///
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &PeiVar
             );
  ASSERT_EFI_ERROR (Status);
  EfiValue = 0;
  BufferSize = sizeof (EfiValue);
  Status = PeiVar->GetVariable (
                     PeiVar,
                     SIMPLE_BOOT_FLAG_VARIABLE_NAME,
                     &gEfiGlobalVariableGuid,
                     NULL,
                     &BufferSize,
                     &EfiValue
                     );

  if (!EFI_ERROR (Status)) {
    if (!CheckOddParity (EfiValue)) {
      EfiValue = 0;
    }
  }

  ///
  /// Read Cmos value
  ///
  LegacyValue = CmosRead8 (0x4F);

  if (!CheckOddParity (LegacyValue)) {
    LegacyValue = 0;
  }

  return (LegacyValue != 0) ? LegacyValue : EfiValue;
}
#endif //MINTREE_FLAG

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4090)
#endif

EFI_STATUS
EFIAPI
PlatformInitPreMemUpdateBootMode (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  )
{
  EFI_STATUS                    Status;
  PEI_CAPSULE_PPI               *Capsule;
  EFI_BOOT_MODE                 BootMode;
#ifndef MINTREE_FLAG
  EFI_SIMPLE_BOOT_FLAG          SimpleBootFlag;
  UINT8                         FastBootRegData;
  FAST_BOOT_EXCEPTION_INFO_HOB  *FastBootExceptionInfoHob;
  FAST_BOOT_EXCEPTION_TYPE      ExceptionType;
  FAST_BOOT_EXCEPTION_CATEGORY  ExceptionCategory;
  BOOLEAN                       IsFastBootEnable;

  ExceptionType     = NoException;
  ExceptionCategory = NoExceptionCategory;
  FastBootRegData   = 0;
#endif //MINTREE_FLAG

  PeiServicesGetBootMode (&BootMode);
#ifndef MINTREE_FLAG
  IsFastBootEnable = IsFastBootEnabled ();
  ///
  /// Check Simple Boot Flag @ CMOS Index 0x4F
  ///
  SimpleBootFlag.Uint8 = GetSimpleBootFlag ();
  if ((SimpleBootFlag.Bits.Booting != 0) || (SimpleBootFlag.Bits.Diag != 0)) {
    ///
    /// Don't set Diagnostics boot it will significantly impact boot performance. e.g.: MRC extensive memory test
    /// Current platform boot flow is very complex and it may reset before booting to OS.
    /// Need to design a hook machanism: bios calls PreReset() before resetting system when the BOOTING bit gets cleared.
    ///
    /// PrioritizeBootMode (&BootMode, BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS);
  }
#endif //MINTREE_FLAG
  ///
  /// Determine if we're in capsule update mode
  ///
  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = PeiServicesLocatePpi (
               &gPeiCapsulePpiGuid,
               0,
               NULL,
               (VOID **)&Capsule
               );
    if (Status == EFI_SUCCESS) {
      Status = Capsule->CheckCapsuleUpdate ((EFI_PEI_SERVICES **) PeiServices);
      if (Status == EFI_SUCCESS) {
        PrioritizeBootMode (&BootMode, BOOT_ON_FLASH_UPDATE);
      }
    }
  }
  if ((BootMode != BOOT_ON_S5_RESUME) && IsBootWithNoChange ()) {
#ifndef MINTREE_FLAG
  if (IsFastBootEnable) { // Set BootMode correctly so that later priotization take effect properly.
    if(!IsPreviousBootSuccessful () && IsTimeOutOccurred () && IsPowerButtonOverride ()) {
      PrioritizeBootMode (&BootMode, BOOT_WITH_FULL_CONFIGURATION);
      }
    } else {
#endif //MINTREE_FLAG
    PrioritizeBootMode (&BootMode, BOOT_ASSUMING_NO_CONFIGURATION_CHANGES);
#ifndef MINTREE_FLAG
    }
#endif //MINTREE_FLAG
  } else {
    PrioritizeBootMode (&BootMode, BOOT_WITH_FULL_CONFIGURATION);
  }

#ifndef MINTREE_FLAG
  ///
  /// if Fast Boot is enabled, check other excption conditions before
  /// prioritizing BootMode to BOOT_WITH_MINIMAL_CONFIGURATION
  ///
  if (IsFastBootEnable) {
    if(IsPreviousBootSuccessful () && !IsTimeOutOccurred () && !IsPowerButtonOverride ()) {
      DEBUG ((DEBUG_INFO, "Prioritizing Boot mode to BOOT_WITH_MINIMAL_CONFIGURATION\n"));
      PrioritizeBootMode (&BootMode, BOOT_WITH_MINIMAL_CONFIGURATION);
    } else {
      Status = PeiServicesCreateHob (
                 EFI_HOB_TYPE_GUID_EXTENSION,
                 sizeof (FAST_BOOT_EXCEPTION_INFO_HOB),
                 (VOID **) &FastBootExceptionInfoHob
                 );
      if (!EFI_ERROR (Status)) {
        FastBootExceptionInfoHob->Header.Name = gFastBootExceptionInfoHobGuid;
        FastBootExceptionInfoHob->FbExceptionType = ExceptionType2;
        FastBootExceptionInfoHob->FbExceptionCategory = BootFailure;
      }

      DEBUG ((DEBUG_INFO, "Exception has occurred. Prioritizing Boot mode to BOOT_WITH_FULL_CONFIGURATION\n"));
      PrioritizeBootMode (&BootMode, BOOT_WITH_FULL_CONFIGURATION);
    }
  }
#endif //MINTREE_FLAG
  Status = PeiServicesSetBootMode (BootMode);
  ASSERT_EFI_ERROR (Status);
#ifndef MINTREE_FLAG
  /*
  if (*BootMode == BOOT_ON_S3_RESUME) {
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListSStateBootMode);
    ASSERT_EFI_ERROR (Status);
  }
  */

  ///
  /// If PLL or Bclk reset occured during S3, we need to do
  /// prioritize BootMode to BOOT_ON_S3_RESUME
  ///
  if(PmcIsBclkS3Boot ()) {
    DEBUG ((DEBUG_INFO, "Prioritizing Boot mode to BOOT_ON_S3_RESUME for PLL or BCLK S3 OC flow.\n"));
    PrioritizeBootMode (&BootMode, BOOT_ON_S3_RESUME);
  }

  ///
  /// FastBoot - set boot progress bit to indicate boot is in progress
  /// bit will be cleared before booting to OS
  ///
  if (IsFastBootEnable) {
    if (BootMode != BOOT_ON_S3_RESUME) {
      FastBootRegData = CmosRead8 (CMOS_FAST_BOOT_REG);
      FastBootRegData |= FAST_BOOT_OR_MASK;
      CmosWrite8 (CMOS_FAST_BOOT_REG, FastBootRegData);
    }
  }
#endif //MINTREE_FLAG
  ///
  /// Signal possible dependent modules that there has been a
  /// final boot mode determination, it is used to build BIST
  /// Hob for Dxe use.
  ///
  Status = PeiServicesInstallPpi (&mPpiBootMode);
  ASSERT_EFI_ERROR (Status);
#ifndef MINTREE_FLAG
  if (BootMode != BOOT_ON_S3_RESUME) {
    ///
    /// If not in S3 mode, signal it.
    ///
    Status = PeiServicesInstallPpi (&mPpiListNonS3BootMode);
    ASSERT_EFI_ERROR (Status);
  }
#endif //MINTREE_FLAG

  return EFI_SUCCESS;
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif
//AMI_OVERRIDE_END - AMI did not use

/**
  Detect boot mode

  @param[in] VariableServices     Variable Service Ppi
**/
VOID
DetectBootMode (
  IN EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices
  )
{
  EFI_STATUS                      Status;
  UINT32                          SleepType;
  EFI_BOOT_MODE                   BootMode;
  SETUP_DATA                      Setup;
  UINTN                           DataSize;
#ifndef MINTREE_FLAG
  SA_SETUP                        SaSetup;
  ME_SETUP                        MeSetup;
  CPU_SETUP                       CpuSetup;
  PCH_SETUP                       PchSetup;
#endif // MINTREE_FLAG

  PeiServicesGetBootMode (&BootMode);

  Status = IsRecoveryMode ();
  if (Status == EFI_SUCCESS) {
    BootMode = BOOT_IN_RECOVERY_MODE;
    Status = PeiServicesInstallPpi (&mPpiListRecoveryBootMode);
    ASSERT_EFI_ERROR (Status);
  } else {
    if (Status == EFI_UNSUPPORTED) {
      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }
    if (GetSleepTypeAfterWakeup (&SleepType)) {
      switch (SleepType) {
        case V_PCH_ACPI_PM1_CNT_S3:
          BootMode = BOOT_ON_S3_RESUME;
          break;
        case V_PCH_ACPI_PM1_CNT_S4:
          BootMode = BOOT_ON_S4_RESUME;
          break;
        case V_PCH_ACPI_PM1_CNT_S5:
          BootMode = BOOT_ON_S5_RESUME;
          break;
      }
    }

    DEBUG ((DEBUG_INFO, "Check Cmos Battery Status\n"));
    if (!PchIsRtcBatteryGood ()) {
      ///
      /// Report RTC battery failure
      ///
      DEBUG ((DEBUG_WARN, "RTC Power failure !! Could be due to a weak or missing battery.\n"));

      DEBUG ((DEBUG_WARN, "BootMode is set to BOOT_WITH_DEFAULT_SETTINGS\n"));

      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }
  }
  ///
  /// Check whether Setup Variable does exist to know the first boot or not.
  ///
  DataSize = sizeof (SETUP_DATA);
  Status  = VariableServices->GetVariable (VariableServices, L"Setup", &gSetupVariableGuid, NULL, &DataSize, &Setup);
  if (Status == EFI_NOT_FOUND) {
    BootMode = BOOT_WITH_DEFAULT_SETTINGS;
  }
#ifndef MINTREE_FLAG
  if (Status != EFI_NOT_FOUND) {
    DataSize = sizeof (SA_SETUP);
    Status = VariableServices->GetVariable (VariableServices, L"SaSetup", &gSaSetupVariableGuid, NULL, &DataSize, &SaSetup);
    if (Status == EFI_NOT_FOUND) {
      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }
  }
  if (Status != EFI_NOT_FOUND) {
    DataSize = sizeof (ME_SETUP);
    Status = VariableServices->GetVariable (VariableServices, L"MeSetup", &gMeSetupVariableGuid, NULL, &DataSize, &MeSetup);
    if (Status == EFI_NOT_FOUND) {
      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }
  }
  if (Status != EFI_NOT_FOUND) {
    DataSize = sizeof (CPU_SETUP);
    Status = VariableServices->GetVariable (VariableServices, L"CpuSetup", &gCpuSetupVariableGuid, NULL, &DataSize, &CpuSetup);
    if (Status == EFI_NOT_FOUND) {
      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }
  }
  if (Status != EFI_NOT_FOUND) {
    DataSize = sizeof (PCH_SETUP);
    Status = VariableServices->GetVariable (VariableServices, L"PchSetup", &gPchSetupVariableGuid, NULL, &DataSize, &PchSetup);
    if (Status == EFI_NOT_FOUND) {
      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }
  }
#endif // MINTREE_FLAG

  PeiServicesSetBootMode (BootMode);

//AMI_OVERRIDE_START - Take care by AMI module  
#if 0
#ifdef CAPSULE_FLAG
  ///
  /// Register notify to update BootMode later
  ///
  Status = PeiServicesNotifyPpi (&mBootModeNotifyList);
#else
  Status = PlatformInitPreMemUpdateBootMode (GetPeiServicesTablePointer (), NULL, NULL);
#endif
  ASSERT_EFI_ERROR (Status);
#endif
//AMI_OVERRIDE_END - Take care by AMI module

  return ;
}


/**
  Check PlatformInfo HOB existing or not

  @param[out] PlatformInfo

  @retval    Status
**/
EFI_STATUS
CheckPlatformInfoHob (
  OUT PLATFORM_INFO                     **PlatformInfo
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_HOB_POINTERS                  Hob;

  *PlatformInfo = NULL;
  ///
  /// Find the PlatformInfo HOB
  ///
  Status = PeiServicesGetHobList ((VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION && (CompareMem (&Hob.Guid->Name, &gPlatformInfoHobGuid, sizeof(gPlatformInfoHobGuid))==0)) {
      *PlatformInfo = (PLATFORM_INFO *) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  if (!(*PlatformInfo)) {
    Status = EFI_NOT_FOUND;
    return Status;
  }

  return Status;
}
//AMI_OVERRIDE_START - AMI did not use
#if 0
#ifndef MINTREE_FLAG
/**
  Check to see the overclocking is enabled or not

  @retval  OverclockingEnabledStatus  TRUE means overclocking is enabled
                                      FALSE means overclocking is disabled
**/
BOOLEAN
IsOverclockingEnabled (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiReadOnlyVarPpi;
  UINTN                           VarSize;
  CPU_SETUP                       CpuSetup;
  BOOLEAN                         OverclockingEnabledStatus;

  OverclockingEnabledStatus = FALSE;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &PeiReadOnlyVarPpi
             );
  if (Status == EFI_SUCCESS) {
    VarSize = sizeof (CPU_SETUP);
    Status = PeiReadOnlyVarPpi->GetVariable (
                                  PeiReadOnlyVarPpi,
                                  L"CpuSetup",
                                  &gCpuSetupVariableGuid,
                                  NULL,
                                  &VarSize,
                                  &CpuSetup
                                  );
    if (Status == EFI_SUCCESS) {
      if (CpuSetup.OverclockingSupport != 0) {
        OverclockingEnabledStatus = TRUE;
      }
    }
  }

  return OverclockingEnabledStatus;
}
#endif // MINTREE_FLAG

/**
  Create Fast Boot HOB

  @param[in]  VariableServices  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
EFIAPI
CreateFastBootHob (
  IN EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices
  )
{
#ifndef MINTREE_FLAG
  EFI_STATUS                      Status;
  UINTN                           VarSize;
  EFI_BOOT_MODE                   BootMode;
  FAST_BOOT_FUNCTION_ENABLED_HOB  *FastBootFunctionEnabledHob;
  FAST_BOOT_EXCEPTION_INFO_HOB    *FastBootExceptionInfoHob;

  if (IsFastBootEnabled ()) {
    ///
    /// If Fast Boot is enabled, create the FAST_BOOT_FUNCTION_ENABLED_HOB for other modules' reference.
    ///
    Status = PeiServicesCreateHob (
               EFI_HOB_TYPE_GUID_EXTENSION,
               sizeof (FAST_BOOT_FUNCTION_ENABLED_HOB),
               (VOID **) &FastBootFunctionEnabledHob
               );
    if (!EFI_ERROR (Status)) {
      FastBootFunctionEnabledHob->Header.Name = gFastBootFunctionEnabledHobGuid;
      FastBootFunctionEnabledHob->FastBootEnabled = TRUE;
    } else {
      return Status;
    }
    ///
    /// When RTC battery is drained (RTC power loss) or CMOS content is cleared (via jumper short), this bit will get set.
    /// This is the Fast Boot Exception Type 2.
    ///
    if (MmioRead16 (
          MmPciBase (
          DEFAULT_PCI_BUS_NUMBER_PCH,
          PCI_DEVICE_NUMBER_PCH_PMC,
          PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B) & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS)
    {
      Status = PeiServicesCreateHob (
                 EFI_HOB_TYPE_GUID_EXTENSION,
                 sizeof (FAST_BOOT_EXCEPTION_INFO_HOB),
                 (VOID **) &FastBootExceptionInfoHob
                 );
      if (!EFI_ERROR (Status)) {
        FastBootExceptionInfoHob->Header.Name = gFastBootExceptionInfoHobGuid;
        FastBootExceptionInfoHob->FbExceptionType = ExceptionType2;
        FastBootExceptionInfoHob->FbExceptionCategory = ContentLost;
      } else {
        return Status;
      }
    }
    ///
    /// Check the EFI Globally-Defined variable which is nonexistent right after updating BIOS, to determine if BIOS was just updated.
    ///
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"ConOut",
                                 &gEfiGlobalVariableGuid,
                                 NULL,
                                 &VarSize,
                                 NULL
                                 );
    if (Status == EFI_NOT_FOUND) {
      PeiServicesGetBootMode (&BootMode);
      Status = PrioritizeBootMode (&BootMode, BOOT_WITH_FULL_CONFIGURATION);
      if (!EFI_ERROR (Status)) {
        Status = PeiServicesSetBootMode (BootMode);
      }
      Status = PeiServicesCreateHob (
                 EFI_HOB_TYPE_GUID_EXTENSION,
                 sizeof (FAST_BOOT_EXCEPTION_INFO_HOB),
                 (VOID **) &FastBootExceptionInfoHob
                 );
      if (!EFI_ERROR (Status)) {
        FastBootExceptionInfoHob->Header.Name = gFastBootExceptionInfoHobGuid;
        FastBootExceptionInfoHob->FbExceptionType = ExceptionType2;
        FastBootExceptionInfoHob->FbExceptionCategory = FirmwareUpdate;
      } else {
        return Status;
      }
    }
    ///
    /// If overclocking is enabled, then BIOS shall switch back to Full Boot mode
    ///
    if (IsOverclockingEnabled ()) {
      Status = PeiServicesCreateHob (
                 EFI_HOB_TYPE_GUID_EXTENSION,
                 sizeof (FAST_BOOT_EXCEPTION_INFO_HOB),
                 (VOID **) &FastBootExceptionInfoHob
                 );
      if (!EFI_ERROR (Status)) {
        FastBootExceptionInfoHob->Header.Name = gFastBootExceptionInfoHobGuid;
        FastBootExceptionInfoHob->FbExceptionType = ExceptionType2;
        FastBootExceptionInfoHob->FbExceptionCategory = SpecialBoot;
      } else {
        return Status;
      }
    }
  }
#endif //MINTREE_FLAG
  return EFI_SUCCESS;
}

#endif
//AMI_OVERRIDE_END - AMI did not use

/**
  Parse the status registers for figuring out the wake-up event and save it into
  an GUID HOB which will be referenced later.

  @param[out]  WakeUpType       Updates the wakeuptype based on the status registers
**/
VOID
GetWakeupEventAndSaveToHob (
  OUT  UINT8   *WakeUpType
  )
{
  UINT16             Pm1Sts;
  UINTN              Gpe0Sts;
  UINTN              Gpe0PmeSts;
  UINTN              Gpe0RiSts;

  DEBUG ((DEBUG_INFO, "GetWakeupEventAndSaveToHob ()\n"));

  Gpe0Sts    = 0;
  Gpe0RiSts  = 0;
  Gpe0PmeSts = 0;

  ///
  /// Read the ACPI registers
  ///
  Pm1Sts     = IoRead16 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_STS);
  Gpe0Sts    = IoRead32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_GPE0_STS_127_96);
  Gpe0RiSts  = Gpe0Sts & B_PCH_ACPI_GPE0_STS_127_96_RI;
  Gpe0PmeSts = Gpe0Sts & B_PCH_ACPI_GPE0_STS_127_96_PME;

  DEBUG ((DEBUG_INFO, "Gpe0Sts is: %02x\n", Gpe0Sts));
  DEBUG ((DEBUG_INFO, "ACPI Wake Status Register: %04x\n", Pm1Sts));

  ///
  /// Figure out the wake-up event
  ///
  if ((Pm1Sts & B_PCH_ACPI_PM1_EN_PWRBTN) != 0 ) {
    *WakeUpType = SystemWakeupTypePowerSwitch;
  } else if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0) {
    if (Gpe0PmeSts != 0) {
      *WakeUpType = SystemWakeupTypePciPme;
    } else if (Gpe0RiSts != 0) {
      *WakeUpType = SystemWakeupTypeModemRing;
    } else if (Gpe0Sts != 0) {
      *WakeUpType = SystemWakeupTypeOther;
    } else {
      *WakeUpType = SystemWakeupTypeOther;
    }
  } else if ((Pm1Sts & B_PCH_ACPI_PM1_STS_RTC) != 0) {
    *WakeUpType = SystemWakeupTypeApmTimer;
  } else {
    *WakeUpType = SystemWakeupTypeOther;
  }

  DEBUG ((DEBUG_INFO, "GetWakeupEventAndSaveToHob : WakeUpType %x\n", *WakeUpType));
}


//@todo it should be performed in Si Pkg.
/**
  Provide hard reset PPI service.
  To generate full hard reset, write 0x0E to PCH RESET_GENERATOR_PORT (0xCF9).

  @param[in]  PeiServices       General purpose services available to every PEIM.

  @retval     Not return        System reset occured.
  @retval     EFI_DEVICE_ERROR  Device error, could not reset the system.
**/
EFI_STATUS
EFIAPI
PchReset(
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  DEBUG ((DEBUG_INFO, "Perform Cold Reset\n"));
  IoWrite8 (RESET_GENERATOR_PORT, 0x0E);

  CpuDeadLoop ();

  ///
  /// System reset occured, should never reach at this line.
  ///
  ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);

  return EFI_DEVICE_ERROR;
}

//@todo it should be moved to Si Pkg.
/**
  Early Platform PCH initialization
**/
VOID
EarlyPlatformPchInit (
  VOID
  )
{
  UINT16                          Data16;
  UINT8                           Data8;
  UINTN                           LpcBaseAddress;
  UINT8                           TcoRebootHappened;
  TCO_WDT_HOB                     *TcoWdtHobPtr;
  EFI_STATUS                      Status;
  UINTN                           P2sbBase;

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  ///
  /// Program bar
  ///
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );

  MmioWrite32 (P2sbBase + R_PCH_P2SB_SBREG_BAR, PCH_PCR_BASE_ADDRESS);
  MmioOr8 (P2sbBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// Program ACPI BASE
  ///
  PchAcpiBaseSet (PcdGet16 (PcdAcpiBaseAddress));

  ///
  /// Program PWRM BASE
  ///
  PchPwrmBaseSet (PCH_PWRM_BASE_ADDRESS);

  ///
  /// LPC I/O Configuration
  ///
//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
  PchLpcIoDecodeRangesSet (
    (V_PCH_LPC_IOD_LPT_378  << N_PCH_LPC_IOD_LPT)  |
    (V_PCH_LPC_IOD_COMB_3E8 << N_PCH_LPC_IOD_COMB) |
    (V_PCH_LPC_IOD_COMA_3F8 << N_PCH_LPC_IOD_COMA)
    );

  PchLpcIoEnableDecodingSet (
    B_PCH_LPC_IOE_ME2  |
    B_PCH_LPC_IOE_SE   |
    B_PCH_LPC_IOE_ME1  |
    B_PCH_LPC_IOE_KE   |
    B_PCH_LPC_IOE_HGE  |
    B_PCH_LPC_IOE_LGE  |
    B_PCH_LPC_IOE_FDE  |
    B_PCH_LPC_IOE_PPE  |
    B_PCH_LPC_IOE_CBE  |
    B_PCH_LPC_IOE_CAE
    );
//AMI_OVERRIDE_START - Only for CRB EC
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC


  ///
  /// Enable the upper 128-byte bank of RTC RAM
  ///
  PchPcrAndThenOr32 (PID_RTC, R_PCH_PCR_RTC_CONF, (UINT32)~0, B_PCH_PCR_RTC_CONF_UCMOS_EN);

  ///
  /// Disable the Watchdog timer expiration from causing a system reset
  ///
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~0, B_PCH_PCR_ITSS_GIC_AME);

  ///
  /// Halt the TCO timer
  ///
  Data16 = IoRead16 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO1_CNT);
  Data16 |= B_PCH_TCO_CNT_TMR_HLT;
  IoWrite16 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO1_CNT, Data16);

  ///
  /// Read the Second TO status bit
  ///
  Data8 = IoRead8 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO2_STS);
  if ((Data8 & B_PCH_TCO2_STS_SECOND_TO) == B_PCH_TCO2_STS_SECOND_TO) {
    TcoRebootHappened = 1;
    DEBUG ((DEBUG_INFO, "PlatformInitPreMem - TCO Second TO status bit is set. This might be a TCO reboot\n"));
  } else {
    TcoRebootHappened = 0;
  }

  ///
  /// Create HOB
  ///
  Status = PeiServicesCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, sizeof (TCO_WDT_HOB), (VOID **) &TcoWdtHobPtr);
  if (!EFI_ERROR (Status)) {
    TcoWdtHobPtr->Header.Name  = gTcoWdtHobGuid;
    TcoWdtHobPtr->TcoRebootHappened = TcoRebootHappened;
  }

  ///
  /// Clear the Second TO status bit
  ///
  IoWrite8 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO2_STS, B_PCH_TCO2_STS_SECOND_TO);

  ///
  /// Disable SERR NMI and IOCHK# NMI in port 61
  ///
  Data8 = IoRead8 (R_PCH_NMI_SC);
  Data8 |= (B_PCH_NMI_SC_PCI_SERR_EN | B_PCH_NMI_SC_IOCHK_NMI_EN);
  IoWrite8 (R_PCH_NMI_SC, Data8);

  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~B_PCH_PCR_ITSS_GIC_AME, 0);
}
#ifndef MINTREE_FLAG
// @todo: Move it to BaseEcMiscLib
//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG
/**
  EcForceResetAfterAcRemoval

  @param[in]  ForceResetAfterAcRemovalVar   0 : Clear AC Removal reset bit in EC
                                            1 : Set AC Removal reset bit in EC
**/
VOID
EcForceResetAfterAcRemoval (
  IN UINT8  ForceResetAfterAcRemovalVar
  )
{
  EFI_STATUS Status;
  UINT8      DataBuffer[2];

  DataBuffer[0] = EC_D_FORCE_RESET;
  Status = ReadEcRam (DataBuffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  if (ForceResetAfterAcRemovalVar == 1) {
    ///
    /// ForceResetAfterAcRemovalVar = 1
    /// Set EC RAM 0x78 BIT2.  This is Reset after AC removal.
    ///
    DataBuffer[0] |= BIT2;
  } else {
    ///
    /// ForceResetAfterAcRemovalVar = 0
    /// Clear EC RAM 0x78 BIT2
    ///
    DataBuffer[0] &= ~BIT2;
  }

  DataBuffer[1] = DataBuffer[0];
  DataBuffer[0] = EC_D_FORCE_RESET;
  Status = WriteEcRam (DataBuffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  Status = UpdateDevicePower ();
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
}

#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC

/**
  Set the state to go after G3

  @dot
    digraph G {
      subgraph cluster_c0 {
        node [shape = box];
          b1[label="EcForceResetAfterAcRemoval ()" fontsize=12 style=filled color=lightblue];
          b2[label="Force S5" fontsize=12 style=filled color=lightblue];
          b3[label="Force S0" fontsize=12 style=filled color=lightblue];

        node [shape = ellipse];
          e1[label="Start" fontsize=12 style=filled color=lightblue];
          e2[label="End" fontsize=12 style=filled color=lightblue];

        node [shape = diamond,style=filled,color=lightblue];
          d1[label="GetVariable\nFroceResetAfterAcRemoval" fontsize=12];
          d2[label="EcPresent" fontsize=12];
          d3[label="ForceResetAfterAcRemoval" fontsize=12];
          d4[label="GetVariable\nStateAfterG3" fontsize=12];
          d5[label="StateAfterG3" fontsize=12];

        label = "SetTheStateToGoAfterG3 Flow"; fontsize=15; fontcolor=black; color=lightblue;
        e1 -> d1
        d1 -> d2 [label="Success" fontsize=9]
        d1 -> d4 [label="Fail" fontsize=9]
        d2 -> b1 [label="Yes" fontsize=9]
        b1 -> d3
        d2 -> d3 [label="No" fontsize=9]
        d3 -> b3 [label="Yes" fontsize=9]
        d3 -> d4 [label="No" fontsize=9]
        d4 -> d5 [label="Success" fontsize=9]
        d4 -> b3 [label="Fail" fontsize=9]
        d5 -> b2 [label="Set" fontsize=9]
        d5 -> b3 [label="Clear" fontsize=9]
        b3 -> e2
      }
    }
  @enddot

  @param[in]  VariableServices  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
**/
VOID
SetTheStateToGoAfterG3 (
  IN EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices
  )
{
  EFI_STATUS            Status;
  UINTN                 VariableSize;
  UINT8                 ForceResetAfterAcRemovalVar;
  UINT8                 StateAfterG3;
  PCH_SETUP             PchSetup;
  UINT8                 DataUint8;
  UINTN                 PmcBaseAddress;

  ForceResetAfterAcRemovalVar = 0;
  StateAfterG3 = 0;


  VariableSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &PchSetup
                               );
  if (!EFI_ERROR(Status)) {
    StateAfterG3 = PchSetup.StateAfterG3;
  }
  ///
  /// @todo: It should move to Si Pkg
  ///
  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );

  DataUint8 = MmioRead8 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B);
  //
  // Don't clear B_PCH_PMC_GEN_PMCON_B_PWR_FLR by accident since it's RW/1C
  //
  DataUint8 &= (UINT8)~B_PCH_PMC_GEN_PMCON_B_PWR_FLR;

  if ((StateAfterG3 == 1) && (ForceResetAfterAcRemovalVar == 0)) {
    DataUint8 |= B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN; // AfterG3 = S5
  } else {
    DataUint8 &= (UINT8)~B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN; // AfterG3 = S0
  }
  MmioWrite8 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B, DataUint8);
}
#endif // MINTREE_FLAG

// @todo: It should be moved Policy Init.
/**
  Initialize the GPIO IO selection, GPIO USE selection, and GPIO signal inversion registers

  @param[in] VariableServices  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
**/
VOID
PlatformPchInit (
  IN EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices,
  IN UINT8                           *EcPresent
  )
{
  UINT16                          Data16;
  UINT8                           Data8;
  UINTN                           LpcBaseAddress;

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  ///
  /// LPC I/O Configuration
  ///
//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
  PchLpcIoDecodeRangesSet (
    (V_PCH_LPC_IOD_LPT_378  << N_PCH_LPC_IOD_LPT)  |
    (V_PCH_LPC_IOD_COMB_3E8 << N_PCH_LPC_IOD_COMB) |
    (V_PCH_LPC_IOD_COMA_3F8 << N_PCH_LPC_IOD_COMA)
    );
  PchLpcIoEnableDecodingSet (
    B_PCH_LPC_IOE_ME2  |
    B_PCH_LPC_IOE_SE   |
    B_PCH_LPC_IOE_ME1  |
    B_PCH_LPC_IOE_KE   |
    B_PCH_LPC_IOE_HGE  |
    B_PCH_LPC_IOE_LGE  |
    B_PCH_LPC_IOE_FDE  |
    B_PCH_LPC_IOE_PPE  |
    B_PCH_LPC_IOE_CBE  |
    B_PCH_LPC_IOE_CAE
    );
//AMI_OVERRIDE_START - Only for CRB EC
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
  ///
  /// Enable the upper 128-byte bank of RTC RAM
  ///
  PchPcrAndThenOr32 (PID_RTC, R_PCH_PCR_RTC_CONF, (UINT32)~0, B_PCH_PCR_RTC_CONF_UCMOS_EN);
  ///
  /// Disable the Watchdog timer expiration from causing a system reset
  ///
  PchPcrAndThenOr32 (PID_SMB, R_PCH_PCR_SMBUS_GC, (UINT32)~0, B_PCH_PCR_SMBUS_GC_NR);

  ///
  /// Halt the TCO timer
  ///
  Data16 = IoRead16 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO1_CNT);
  Data16 |= B_PCH_TCO_CNT_TMR_HLT;
  IoWrite16 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO1_CNT, Data16);
  ///
  /// Clear the Second TO status bit
  ///
  IoWrite8 (PcdGet16 (PcdTcoBaseAddress) + R_PCH_TCO2_STS, B_PCH_TCO2_STS_SECOND_TO);
  ///
  /// Enable the LPC I/O decoding for 0x6A0~0x6A7 as EC's extra I/O port, where 0x6A0 is the Data port
  /// and 0x6A4 is the Command/Status port.
  ///
//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  if (*EcPresent == TRUE) {
      PchLpcGenIoRangeSet (EC_EXTRA_IO_PORT_1, 0x10);
  }
#ifdef EMBEDDED_FLAG
  else {
    /// Added Nuvoton HW monitor IO address.
    PchLpcGenIoRangeSet (PcdGet16 (PcdNct6776fHwMonBase), 0x10);
  }
#endif // EMBEDDED_FLAG
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
  ///
  /// Disable SERR NMI and IOCHK# NMI in port 61
  ///
  Data8 = IoRead8 (R_PCH_NMI_SC);
  Data8 |= (B_PCH_NMI_SC_PCI_SERR_EN | B_PCH_NMI_SC_IOCHK_NMI_EN);
  IoWrite8 (R_PCH_NMI_SC, Data8);

}


/**
  Run this function after SaPolicy PPI to be installed.

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor  Notify that this module published.
  @param[in]  Ppi               PPI that was installed.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
EFIAPI
SiPreMemPolicyPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS Status;

  ///
  /// Platform specific programming. It may have Chipset, EC or some platform specific
  /// programming here.
  ///
  Status = PlatformSpecificInitPreMem ();
  ASSERT_EFI_ERROR (Status);

  return Status;
}

#ifndef MINTREE_FLAG

/**
  Run this function after Wdt PPI to be installed.

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor  Notify that this module published.
  @param[in]  Ppi               PPI that was installed.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
EFIAPI
WdtAppPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS Status;
  WDT_PPI    *WdtPei;

  WdtPei = (WDT_PPI *) Ppi;

  Status = PeiWdtApp (WdtPei);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Programs the overrides for VccCore voltage which are controlled by IR3570
  chip using SVID/PMBUS interface.

  Voltage rails VccCore use this routine.

  @param[in] Voltage        Voltage target in mV
  @param[in] SmbusPpi       Smbus PPI to perform SVID transactions
**/
EFI_STATUS
ProgramVccCoreOverride (
  IN  UINT16                Voltage,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddr;
  UINTN                       Length; 
  UINT8                       DeviceCommand;
  UINT8                       InputVID;

  //
  // Slave address
  //
  SlaveAddr.SmbusDeviceAddress = 0x3A;

  //
  // Device Command and set Byte write
  //
  DeviceCommand = 0xE3;  // VccCore
  Length = 1;
  //
  // Send the voltage in mV
  //
  InputVID = (UINT8) (Voltage / 10);
  DEBUG ((DEBUG_INFO, "InputVID = 0x%08X\n", InputVID));

  Status = SmbusPpi->Execute (SmbusPpi, 
                               SlaveAddr, // 7-bit slave address,
                               DeviceCommand,
                               EfiSmbusWriteByte,
                               FALSE,
                               &Length,
                               &InputVID
                             );

  DEBUG ((DEBUG_INFO, "SMBUS transaction result : Status=0x%08X\n", Status));

  return Status;
}

/**
  Programs the overrides for VccSA voltage which are controlled by IR3570
  chip using SVID/PMBUS interface.

  Voltage rails VccSA use this routine.

  @param[in] Voltage        Voltage target in mV
  @param[in] SmbusPpi       Smbus PPI to perform SVID transactions
**/
EFI_STATUS
ProgramVccSaOverride (
  IN  UINT16                Voltage,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddr;
  UINTN                       Length; 
  UINT8                       DeviceCommand;
  UINT8                       InputVID;

  //
  // Slave address
  //
  SlaveAddr.SmbusDeviceAddress = 0x3C;

  //
  // Device Command and set Byte write
  //
  DeviceCommand = 0x6A;  // VccSA
  Length = 1;
  //
  // Send the voltage in mV
  //
  InputVID = (UINT8) ((Voltage / 5) - 50);
  DEBUG ((DEBUG_INFO, "InputVID = 0x%08X\n", InputVID));

  Status = SmbusPpi->Execute (SmbusPpi, 
                               SlaveAddr, // 7-bit slave address,
                               DeviceCommand,
                               EfiSmbusWriteByte,
                               FALSE,
                               &Length,
                               &InputVID
                             );

  DEBUG ((DEBUG_INFO, "SMBUS transaction result : Status=0x%08X\n", Status));

  return Status;
}

/**
  Programs the overrides for VccCore DC Load Line which are controlled by IR3570
  chip using SVID/PMBUS interface.

  @param[in] VrLLOverrideEnable  VccCore VR Load Line Enabled or Disabled
  @param[in] LoadLinemOhms       DC Load Line target in mOhm
  @param[in] SmbusPpi            Smbus PPI to perform SVID transactions
**/
EFI_STATUS
ProgramVccCoreLLOverride (
  IN  UINT8                 VrLLOverrideEnable,
  IN  UINT8                 LoadLinemOhms,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddr;
  UINTN                       Length;
  UINT8                       DeviceCommand;
  UINT8                       Data;

  //
  // Slave address 0x74
  //
  SlaveAddr.SmbusDeviceAddress = 0x3A;

  //
  // Device Command for VccCore DC Load Line Enabled
  //
  DeviceCommand = 0xEC;
  Length = 1;

  Status = SmbusPpi->Execute (SmbusPpi,
                               SlaveAddr,
                               DeviceCommand,
                               EfiSmbusReadByte,
                               FALSE,
                               &Length,
                               &Data);
  DEBUG ((DEBUG_INFO, "SMBUS Read 0xEC: Status=0x%08X\n", Status));
  if (VrLLOverrideEnable == 0) {
    Data &= (UINT8) ~BIT6;
  } else {
    Data |= (UINT8) BIT6;
  }
  Status = SmbusPpi->Execute (SmbusPpi,
                               SlaveAddr,
                               DeviceCommand,
                               EfiSmbusWriteByte,
                               FALSE,
                               &Length,
                               &Data
                             );

  if (VrLLOverrideEnable != 0) {
    //
    // Send the DC Load Line override value
    //
    DeviceCommand = 0xEB;
    Status = SmbusPpi->Execute (SmbusPpi,
                                 SlaveAddr,
                                 DeviceCommand,
                                 EfiSmbusReadByte,
                                 FALSE,
                                 &Length,
                                 &Data);

    Data &= (UINT8)~0x1F;
    Data |= LoadLinemOhms;
    DEBUG ((DEBUG_INFO, "Input VR Load Line setting = 0x%08X\n", Data));
    Status = SmbusPpi->Execute (SmbusPpi,
                                 SlaveAddr,
                                 DeviceCommand,
                                 EfiSmbusWriteByte,
                                 FALSE,
                                 &Length,
                                 &Data
                               );
    DEBUG ((DEBUG_INFO, "SMBUS write 0xEB : Status=0x%08X\n", Status));
  }

  return Status;
}

/**
  Programs the overrides for VccIO voltage which are controlled by IR3570
  chip using SVID/PMBUS interface.

  Voltage rails VccIO use this routine.

  @param[in] Voltage        Voltage target in mV
  @param[in] SmbusPpi       Smbus PPI to perform SVID transactions
**/
EFI_STATUS
ProgramVccIoOverride (
  IN  UINT16                Voltage,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddr;
  UINTN                       Length; 
  UINT8                       DeviceCommand;
  UINT8                       InputVID;

  //
  // Slave address
  //
  SlaveAddr.SmbusDeviceAddress = 0x3C;

  //
  // Device Command and set Byte write
  //
  DeviceCommand = 0x6C;  // VccIO
  Length = 1;

  //
  // Send the voltage in mV
  //
  InputVID = (UINT8) ((Voltage / 5) - 50);
  DEBUG ((DEBUG_INFO, "InputVID = 0x%08X\n", InputVID));

  Status = SmbusPpi->Execute (SmbusPpi, 
                               SlaveAddr, // 7-bit slave address,
                               DeviceCommand,
                               EfiSmbusWriteByte,
                               FALSE,
                               &Length,
                               &InputVID
                             );

  DEBUG ((DEBUG_INFO, "SMBUS transaction result : Status=0x%08X\n", Status));

  return Status;
}


/**
  Programs the overrides for Memoey VDDQ voltage which are controlled by IR3570
  chip using SVID/PMBUS interface.

  Voltage rails VDDQ use this routine.

  @param[in] Voltage        Voltage target in mV
  @param[in] SmbusPpi       Smbus PPI to perform SVID transactions
**/
EFI_STATUS
ProgramVddqOverride (
  IN  UINT16                Voltage,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddr;
  UINTN                       Length; 
  UINT8                       DeviceCommand;
  UINT8                       InputVID;

  //
  // Slave address
  //
  SlaveAddr.SmbusDeviceAddress = 0x3E;

  //
  // Device Command and set Byte write
  //
  DeviceCommand = 0x6C;  // Voltage Out Command
  Length = 1;

  //
  //  Send the voltage in mV
  //
  InputVID = (UINT8) (((Voltage / 10) - 50) + 1);
  DEBUG ((DEBUG_INFO, "InputVID = 0x%08X\n", InputVID));

  Status = SmbusPpi->Execute (SmbusPpi, 
                               SlaveAddr, // 7-bit slave address,
                               DeviceCommand,
                               EfiSmbusWriteByte,
                               FALSE,
                               &Length,
                               &InputVID
                             );

  DEBUG ((DEBUG_INFO, "SMBUS transaction result : Status=0x%08X\n", Status));

  return Status;
}

/**
  Programs VccST volages using SVID/PMBUS interface.

  @param[in] Prefix         Sets the offset value as positive or negative
  @param[in] Voltage        VccSFR_OC voltage in mV
  @param[in] SmbusPpi       Smbus PPI to perform SVID transactions
**/
EFI_STATUS
ProgramVccStOverride (
  IN  UINT16                Voltage,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddr;
  UINTN                       Length; 
  UINT8                       Data;
  UINT8                       DeviceCommand;
  UINT8                       Prefix;

  //
  // Slave address
  //
  SlaveAddr.SmbusDeviceAddress = 0x10;

  //
  // Device Command and set Byte write
  // 
  DeviceCommand = 0x3;  
  Length = 1;

  //
  // Data = 0 is 1020mv 

  //
  Prefix = 0;
  if (Voltage >= 1020) {
  	Data = (UINT8) (((Voltage - 1020) / 10) | (Prefix << 7));
  } else {
    Prefix = 1;
  	Data = (UINT8) (((1020 - Voltage) / 10) | (Prefix << 7));    
  }

  DEBUG ((DEBUG_INFO, "SMBUS transaction request : Addr=0x%02X Command=0x%02X Length=0x%02X Data=0x%02X\n",
    SlaveAddr.SmbusDeviceAddress, DeviceCommand, Length, Data));
  Status = SmbusPpi->Execute (SmbusPpi,
                              SlaveAddr, // 7-bit slave address,
                              DeviceCommand,
                              EfiSmbusWriteByte,
                              FALSE,
                              &Length,
                              &Data
                             );
  DEBUG ((DEBUG_INFO, "SMBUS transaction result : Status=0x%08X\n", Status));

  return Status;
}


/**
  Programs VccSFR_OC volages using SVID/PMBUS interface.

  @param[in] Prefix         Sets the offset value as positive or negative
  @param[in] Voltage        VccSFR_OC voltage in mV
  @param[in] SmbusPpi       Smbus PPI to perform SVID transactions
**/
EFI_STATUS
ProgramVccSfrOcOverride (
  IN  UINT16                Voltage,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddr;
  UINTN                       Length; 
  UINT8                       Data;
  UINT8                       DeviceCommand;
  UINT8                       Prefix;

  //
  // Slave address
  //
  SlaveAddr.SmbusDeviceAddress = 0x13;

  //
  // Device Command and set Byte write
  // 
  DeviceCommand = 0x1;
  Length = 1;

  //
  // Data = 0 is 1250mv

  //
  Prefix = 0;
  if (Voltage >= 1250) {
  	Data = (UINT8) (((Voltage - 1250) / 10) | (Prefix << 7));
  } else {
    Prefix = 1;
  	Data = (UINT8) (((1250 - Voltage) / 10) | (Prefix << 7));    
  }

  DEBUG ((DEBUG_INFO, "SMBUS transaction request : Addr=0x%02X Command=0x%02X Length=0x%02X Data=0x%02X\n",
    SlaveAddr.SmbusDeviceAddress, DeviceCommand, Length, Data)); 
  Status = SmbusPpi->Execute (SmbusPpi, 
                              SlaveAddr, // 7-bit slave address,
                              DeviceCommand,
                              EfiSmbusWriteByte,
                              FALSE,
                              &Length,
                              &Data
                             );
  DEBUG ((DEBUG_INFO, "SMBUS transaction result : Status=0x%08X\n", Status));

  return Status;
}

/**
  Programs VccSFR volages using SVID/PMBUS interface.

  @param[in] Prefix         Sets the offset value as positive or negative
  @param[in] Voltage        VccSFR voltage in mV
  @param[in] SmbusPpi       Smbus PPI to perform SVID transactions
**/
EFI_STATUS
ProgramVccSfrOverride (
  IN  UINT16                Voltage,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS                  Status;
  EFI_SMBUS_DEVICE_ADDRESS    SlaveAddr;
  UINTN                       Length; 
  UINT8                       Data;
  UINT8                       DeviceCommand;
  UINT8                       Prefix;

  //
  // Slave address
  //
  SlaveAddr.SmbusDeviceAddress = 0x13;

  //
  // Device Command and set Byte write
  // 
  DeviceCommand = 0x2;
  Length = 1;

  //
  // Data = 0 is 1020mv

  //
  Prefix = 0;
  if (Voltage >= 1020) {
  	Data = (UINT8) (((Voltage - 1020) / 10) | (Prefix << 7));
  } else {
    Prefix = 1;
  	Data = (UINT8) (((1020 - Voltage) / 10) | (Prefix << 7));    
  }

  DEBUG ((DEBUG_INFO, "SMBUS transaction request : Addr=0x%02X Command=0x%02X Length=0x%02X Data=0x%02X\n",
    SlaveAddr.SmbusDeviceAddress, DeviceCommand, Length, Data)); 
  Status = SmbusPpi->Execute (SmbusPpi,
                              SlaveAddr, // 7-bit slave address,
                              DeviceCommand,
                              EfiSmbusWriteByte,
                              FALSE,
                              &Length,
                              &Data
                             );
  DEBUG ((DEBUG_INFO, "SMBUS transaction result : Status=0x%08X\n", Status));

  return Status;
}

/**
  Checks the override knobs for Vcc IN, Vcc SA, Vcc SFR, Vcc IO, Vddq AB and Vddq CD
  voltages and apply the VRM programming sequences when neccessary.

  Note : KBL-X has only Vddq CD; Vddq AB is simply provided for completeness sake here

  @param[in] CpuSetup       Setup variables under CPU Setup
  @param[in] SaSetup        Setup variables under SA Setup
  @param[in] SmbusPpi       Smbus PPI to perform SVID/PMBUS transactions
**/
VOID
ProgramVoltageOverrides (
  IN  CPU_SETUP             *CpuSetup,
  IN  SA_SETUP              *SaSetup,
  IN  EFI_PEI_SMBUS2_PPI    *SmbusPpi
  )
{
  EFI_STATUS Status = 0x0;

  //
  // Check for VccCore voltage override
  //
  if (CpuSetup->VccCoreOverrideEnable == 0) {
  	DEBUG ((DEBUG_INFO, "Default VccCore selected. No action required for voltage programming.\n"));
  } else {
  	DEBUG ((DEBUG_INFO, "(OCVR) Programming VccCore Override = %X\n", CpuSetup->VccCoreOverrideVoltage));
    Status = ProgramVccCoreOverride (CpuSetup->VccCoreOverrideVoltage, SmbusPpi);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "VccCore programming failed. Status = %r\n", Status));
    } else {
      DEBUG ((DEBUG_INFO, "VccCore programming successful. Status = %r\n", Status));
    }
  } // VccCore override

  //
  // Check for Vcc ST voltage override
  //
  if (CpuSetup->VccStOverrideEnable == 0) {
    DEBUG ((DEBUG_INFO, "Default VccST selected. No action required for voltage programming.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "(OCVR) Programming VccSt Override = %X\n", CpuSetup->VccStOverrideVoltage));
    Status = ProgramVccStOverride (CpuSetup->VccStOverrideVoltage, SmbusPpi);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "VccST programming failed. Status = %r\n", Status));
    } else {
      DEBUG ((DEBUG_INFO, "VccST programming successful. Status = %r\n", Status));
    }
  } // VccST override

  //
  // Check for VccSA voltage override
  //
  if (CpuSetup->VccSaOverrideEnable == 0) {
    DEBUG ((DEBUG_INFO, "Default VccSA selected. No action required for voltage programming.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "(OCVR) Programming VccSa Override = %X\n", CpuSetup->VccSaOverrideVoltage));
    Status = ProgramVccSaOverride (CpuSetup->VccSaOverrideVoltage, SmbusPpi);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "VccSA programming failed. Status = %r\n", Status));
    } else {
      DEBUG ((DEBUG_INFO, "VccSA programming successful. Status = %r\n", Status));
    }
  } // VccSA override

  //
  // Check for VccSFR and VccSFR_OC voltage override
  //
  if (CpuSetup->VccSfrOverrideEnable == 0) {
    DEBUG ((DEBUG_INFO, "Default VccSFR selected. No action required for voltage programming.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "(OCVR) Programming VccSfr Override = %X\n", CpuSetup->VccSfrOverrideVoltage));
    Status = ProgramVccSfrOverride (CpuSetup->VccSfrOverrideVoltage, SmbusPpi);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "VccSFR and VccSFR programming failed. Status = %r\n", Status));
    } else {
      DEBUG ((DEBUG_INFO, "VccSFR and VccSFR programming successful. Status = %r\n", Status)); 
    }
  } // VccSFR override

  
  if (CpuSetup->VccSfrOcOverrideEnable == 0) {
    DEBUG ((DEBUG_INFO, "Default VccSFR_OC selected. No action required for voltage programming.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "(OCVR) Programming VccSfrOc Override = %X\n", CpuSetup->VccSfrOcOverrideVoltage));
    Status = ProgramVccSfrOcOverride (CpuSetup->VccSfrOcOverrideVoltage, SmbusPpi);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "VccSFR and VccSFR_OC programming failed. Status = %r\n", Status));
    } else {
      DEBUG ((DEBUG_INFO, "VccSFR and VccSFR_OC programming successful. Status = %r\n", Status));
    }
  } // VccSFR_OC override

  //
  // Check for VccIO voltage override
  //
  if (CpuSetup->VccIoOverrideEnable == 0)
  {
    DEBUG ((DEBUG_INFO, "Default VccIO selected. No action required for voltage programming.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "(OCVR) Programming VccIo Override = %X\n", CpuSetup->VccIoOverrideVoltage));
    Status = ProgramVccIoOverride (CpuSetup->VccIoOverrideVoltage, SmbusPpi);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "VccIO Programming failed. Status = %r\n", Status));
    } else {
      DEBUG ((DEBUG_INFO, "VccIO Programming successful. Status = %r\n", Status));
    }
  } // Vcc io override

  //
  // Check for VccCore DC Load Line override
  //
  DEBUG ((DEBUG_INFO, "(OCVR) Programming VccCore DC Load Line Enable = %X\n", CpuSetup->CoreVrDcLLOverrideEnable));
  DEBUG ((DEBUG_INFO, "(OCVR) Programming VccCore DC Load Line Override = %X\n", CpuSetup->CoreDcLoadline));
  Status = ProgramVccCoreLLOverride (CpuSetup->CoreVrDcLLOverrideEnable, CpuSetup->CoreDcLoadline, SmbusPpi);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "VccCore DC Load Line Programming failed. Status = %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "VccCore DC Load Line Programming successful. Status = %r\n", Status));
  }

}

/**
  Platform Voltage Init - Programs platform voltages over SMBUS.
  Run this function after Smbus2Ppi is installed.

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor  Notify that this module published.
  @param[in]  Ppi               PPI that was installed.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
EFIAPI
PlatformVoltageInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                      Status;
  EFI_STATUS                      Status2;
  EFI_STATUS                      Status3;
  SA_SETUP                        SaSetup;
  CPU_SETUP                       CpuSetup;
  EFI_PEI_SMBUS2_PPI              *SmbusPpi;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
  UINTN                           VariableSize;
  UINT8                           WdtTimeOut;
  WDT_PPI                         *gWdtPei;
  UINTN                           SmbusPciBase;
  UINT16                          SmbusIoBar;

  Status          = EFI_SUCCESS;
  Status2         = EFI_SUCCESS;
  Status3         = EFI_SUCCESS;
  WdtTimeOut      = 0;
  DEBUG ((DEBUG_INFO, "(OC) Platform Voltage Init\n"));

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &ReadOnlyVariable
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Locate WDT_PPI (ICC WDT PPI)
  //
  Status = PeiServicesLocatePpi (
             &gWdtPpiGuid,
             0,
             NULL,
             (VOID **) &gWdtPei
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SA_SETUP);
  Status2 = ReadOnlyVariable->GetVariable (
                               ReadOnlyVariable,
                               L"SaSetup",
                               &gSaSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SaSetup
                               );

  VariableSize = sizeof (CPU_SETUP);
  Status3 = ReadOnlyVariable->GetVariable (
                               ReadOnlyVariable,
                               L"CpuSetup",
                               &gCpuSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &CpuSetup
                               );

  if (EFI_ERROR (Status2) || EFI_ERROR (Status3)) {
    //
    // If GetVariable fails, return EFI_SUCCESS asuming OC is Disabled.
    //
    DEBUG ((DEBUG_INFO, "(OC) Failed to read Setup Data\n"));
    return EFI_SUCCESS;
  }

  WdtTimeOut = gWdtPei->CheckStatus ();

  if (CpuSetup.OverclockingSupport == 0) {
    DEBUG ((DEBUG_INFO, "(OC) Skipping Platform Voltage Init, overclocking is not enabled in BIOS setup.\n"));
    return EFI_SUCCESS;
  }

  //
  // Locate SMBUS PPI
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbus2PpiGuid,
             0,
             NULL,
             (VOID **) &SmbusPpi
             );
  DEBUG ((DEBUG_INFO, "Locate Smbus2Ppi Status = %x\n", Status));
  ASSERT_EFI_ERROR (Status);

  //
  ///
  /// Voltage control for VccIo, VccST, VccSA, VccSFR, and VccSFR_OC.
  ///   - Check if we need to override the voltages of the different VRMs using the SVID or PMBUS interfaces
  ///
  if (CpuSetup.OverclockingSupport != 0) {
    //
    // Initial SMBUS
    //
    SmbusIoBar = 0xEFA0;

    //
    // Temporary initialize SMBUS
    //
    SmbusPciBase = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SMBUS,
                     PCI_FUNCTION_NUMBER_PCH_SMBUS
                     );
    
    DEBUG ((DEBUG_INFO, "SmbusPciBase = %x\n", SmbusPciBase));
    //
    // Initialize I/O BAR
    //
    MmioWrite16 (SmbusPciBase + R_PCH_SMBUS_BASE, SmbusIoBar);
    
    //
    // Enable the Smbus I/O Enable
    //
    MmioOr8 (SmbusPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_IO_SPACE);

  	DEBUG ((DEBUG_INFO, "SmbusBase Offset 0x4 = %x\n", MmioRead8 (SmbusPciBase + PCI_COMMAND_OFFSET)));
  	//
    // Enable the SMBUS Controller
    //
    MmioOr8 (SmbusPciBase + R_PCH_SMBUS_HOSTC, B_PCH_SMBUS_HOSTC_HST_EN);
    
    ProgramVoltageOverrides (&CpuSetup, &SaSetup, SmbusPpi);
  }

  return Status;
}

#endif //MINTREE_FLAG

/**
  Configues the IC2 Controller on which GPIO Expander Communicates.
  This Function is to enable the I2CGPIOExapanderLib to programm the Gpios
  Complete intilization will be done in later Stage

**/
VOID
EFIAPI
I2CGpioExpanderInitPreMem(
  VOID
  )
{
  ConfigureSerialIoController (PchSerialIoIndexI2C4, PchSerialIoAcpiHidden);
  SerialIoI2cGpioInit (PchSerialIoIndexI2C4, PchSerialIoAcpiHidden, PchSerialIoIs33V);
}



/**
  This function handles PlatformInit task after PeiReadOnlyVariable2 PPI produced

  @dot
    digraph G {
      subgraph cluster_c0 {
        node [shape = box];
          b1[label="FlashMapInit ()" fontsize=12 style=filled color=lightblue];
          b2[label="ConfigurePlatformClocks ()" fontsize=12 style=filled color=lightblue];
          b4[label="Notify Smbus2 PPI" fontsize=12 style=filled color=lightblue];
          b5[label="Enable LPC IO decode for EC access" fontsize=12 style=filled color=lightblue];
          b6[label="Get boot mode" fontsize=12 style=filled color=lightblue];
          b7[label="Get PlatformInfoVariable" fontsize=12 style=filled color=lightblue];
          b8[label="WakeUpEc ()" fontsize=12 style=filled color=lightblue];
          b9[label="WakeUpEc ()" fontsize=12 style=filled color=lightblue];
          b10[label="DetectEcRevision ()" fontsize=12 style=filled color=lightblue];
          b11[label="EcPresent = TRUE" fontsize=12 style=filled color=lightblue];
          b12[label="EcPresent = FALSE" fontsize=12 style=filled color=lightblue];
          b13[label="DetectBoard ()" fontsize=12 style=filled color=lightblue];
          b14[label="PchLpcIoEnableDecodingSet ()\n to restore LPC IO decode setting" fontsize=12 style=filled color=lightblue];
          b15[label="BuildGuidDataHob ()" fontsize=12 style=filled color=lightblue];
          b16[label="PlatformVariableInit ()" fontsize=12 style=filled color=lightblue];
          b17[label="DetectWakeReason ()" fontsize=12 style=filled color=lightblue];
          b18[label="DetectBootMode ()" fontsize=12 style=filled color=lightblue];
          b19[label="Get SystemConfiguration variable" fontsize=12 style=filled color=lightblue];
          b20[label="Set the system configuration\n to FirmwareConfiguration" fontsize=12 style=filled color=lightblue];
          b21[label="Set the system configuration\n to production mode" fontsize=12 style=filled color=lightblue];
          b22[label="PeiPolicyInitPreMem ()" fontsize=12 style=filled color=lightblue];
          b23[label="BoardInit ()\n to configure GPIO and SIO" fontsize=12 style=filled color=lightblue];
          b24[label="CreateFastBootHob ()" fontsize=12 style=filled color=lightblue];
          b25[label="GetWakeupEventAndSaveToHob ()" fontsize=12 style=filled color=lightblue];
          b26[label="CreateAttemptUsbFirstHotkeyInfoHob ()" fontsize=12 style=filled color=lightblue];
          b27[label="Notify Wdt PPI" fontsize=12 style=filled color=lightblue];
          b28[label="Notify gSiPreMemPolicyPpiGuid PPI" fontsize=12 style=filled color=lightblue];
          b29[label="PlatformPchInit ()" fontsize=12 style=filled color=lightblue];
          b30[label="SetTheStateToGoAfterG3 ()" fontsize=12 style=filled color=lightblue];
          b31[label="Clear all pending SMI" fontsize=12 style=filled color=lightblue];
          b32[label="Install Pre Memory PPIs" fontsize=12 style=filled color=lightblue];

        node [shape = ellipse];
          e1[label="Start" fontsize=12 style=filled color=lightblue];
          e2[label="End" fontsize=12 style=filled color=lightblue];

        node [shape = diamond,style=filled,color=lightblue];
          d1[label="Locate Smbus2 PPI" fontsize=12];
          d2[label="Get PlatformInfoVariable successfully\nand BootMode is not equal full configuration" fontsize=12];
          d3[label="Is EC present?" fontsize=12];
          d4[label="Is status success?" fontsize=12];
          d5[label="Is EC not present?" fontsize=12];
          d6[label="Is status success?" fontsize=12];

        label = "PlatformInitPreMem Flow"; fontsize=15; fontcolor=black; color=lightblue;
        e1 -> b1
        b1 -> d1
        d1 -> b2 [label="Success" fontsize=9]
        d1 -> b4 [label="Fail" fontsize=9]
        b2 -> b3
        b3 -> b5
        b4 -> b5
        b5 -> b6
        b6 -> b7
        b7 -> d2
        d2 -> d3 [label="Yes" fontsize=9]
        d2 -> b9 [label="No" fontsize=9]
        d3 -> b8 [label="Yes" fontsize=9]
        d3 -> d5 [label="No" fontsize=9]
        b8 -> d5
        b9 -> b10
        b10 -> d4
        d4 -> b11 [label="Yes" fontsize=9]
        d4 -> b12 [label="No" fontsize=9]
        b11 -> b13
        b12 -> b13
        b13 -> d5
        d5 -> b14 [label="Not present" fontsize=9]
        d5 -> b15 [label="Present" fontsize=9]
        b14 -> b15
        b15 -> b16
        b16 -> b17
        b17 -> b18
        b18 -> b19
        b19 -> d6
        d6 -> b20 [label="Yes" fontsize=9]
        d6 -> b21 [label="No" fontsize=9]
        b20 -> b22
        b21 -> b22
        b22 -> b23
        b23 -> b24
        b24 -> b25
        b25 -> b26
        b26 -> b27
        b27 -> b28
        b28 -> b29
        b29 -> b30
        b30 -> b31
        b31 -> b32
        b32 -> e2
      }
    }
  @enddot

  @param[in]  PeiServices  Pointer to PEI Services Table.
  @param[in]  NotifyDesc   Pointer to the descriptor for the Notification event that
                           caused this function to execute.
  @param[in]  Ppi          Pointer to the PPI data associated with this function.

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
EFIAPI
PlatformInitPreMem (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                        Status;
  PLATFORM_INFO                     PlatformInfo;
//AMI_OVERRIDE_START - Take care by AMI setup
#if 0
  PLATFORM_INFO                     *PlatformInfoPtr;
#endif
//AMI_OVERRIDE_END - Take care by AMI setup
  BOARD_CONFIG                      BoardConfig;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  VOID                              *Hob;
//AMI_OVERRIDE_START - Only for CRB EC
#if 0
  UINT16                            LpcIoe;
  UINT16                            LpcIoeOrg;
  UINTN                             LpcBaseAddress;
#endif
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  UINTN                             VarSize;
  PLATFORM_INFO                     PlatformInfoVariable;
  UINT32                            SleepType;
#else
  PLATFORM_INFO                     *PlatformInfoHob;
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
  EFI_BOOT_MODE                     BootMode;
  UINTN                             McD0BaseAddress;
  UINT16                            ABase;
  UINT16                            Pm1Sts;
  UINT32                            Pm1Cnt;
  SETUP_DATA                        SystemConfiguration;
  UINTN                             VariableSize;
  UINT8                             FwConfig;
#ifndef MINTREE_FLAG
#if 0 // AMI_OVERRIDE_START - Take care by AMI setup
  CHAR16                            BiosVersion[40];
  CHAR16                            ReleaseDate[20];
  CHAR16                            ReleaseTime[20];
#endif // AMI_OVERRIDE_END - Take care by AMI setup
#endif //MINTREE_FLAG
  UINT8                             SynchDelay;
//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  UINT8                             EcDataBuffer; // AdvancedFeaturesContent

#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
  VariableServices = (EFI_PEI_READ_ONLY_VARIABLE2_PPI *) Ppi;

//AMI_OVERRIDE_START - Take care by AMI setup
#if 0
  ZeroMem (&PlatformInfoPtr, sizeof (PlatformInfoPtr));
  if (CheckPlatformInfoHob (&PlatformInfoPtr) == EFI_SUCCESS) {
    ///
    /// If PlatformInfo HOB has been created, then exit the function to avoid function re-entry.
    ///
    return EFI_SUCCESS;
  }
#endif
//AMI_OVERRIDE_END - Take care by AMI setup

  ZeroMem (&PlatformInfo, sizeof (PlatformInfo));
  ZeroMem (&BoardConfig, sizeof (BoardConfig));

//AMI_OVERRIDE_START - Only for CRB EC
#if 0
  ///
  /// Enable LPC IO decode for EC access
  ///
  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );
  LpcIoeOrg = MmioRead16 (LpcBaseAddress + R_PCH_LPC_IOE);
  LpcIoe = LpcIoeOrg | B_PCH_LPC_IOE_ME1;
#ifdef EMBEDDED_FLAG
  // Enable LPC IO decode for SIO (0x2e) access.
  LpcIoe = LpcIoeOrg | B_PCH_LPC_IOE_SE;
  LpcIoeOrg |= B_PCH_LPC_IOE_SE;
#endif
  PchLpcIoEnableDecodingSet (LpcIoe);
#endif
//AMI_OVERRIDE_END - Only for CRB EC

//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  GetSleepTypeAfterWakeup(&SleepType);
  VarSize = sizeof (PLATFORM_INFO);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_INFO_VARIABLE_NAME,
                               &gPlatformInfoHobGuid,
                               NULL,
                               &VarSize,
                               &PlatformInfoVariable
                               );
  if ((Status == EFI_SUCCESS) && (SleepType == V_PCH_ACPI_PM1_CNT_S3)) {
    CopyMem (&PlatformInfo, &PlatformInfoVariable, sizeof (PLATFORM_INFO));
    CopyMem(&BoardConfig, &PlatformInfoVariable, sizeof(PLATFORM_INFO));
    if (PlatformInfo.EcPresent == TRUE) {
      Status = WakeUpEc ();
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_WARN, "Wake Up Ec Fail. Status = %r \n", Status));
      }
      PlatformInfo.UpdateData = FALSE;
    }
    // Init BoardId.GetBoardConfig() ignores the EC commands to reduce resume time and perform other functionalities
    GetBoardConfig (&BoardConfig, SleepType, sizeof (BoardConfig.BoardName) / sizeof (CHAR8));
  } else {
    Status = WakeUpEc ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_WARN, "Wake Up Ec Fail. Status = %r \n", Status));
    }
    GetBoardConfig (&BoardConfig, SleepType, sizeof (BoardConfig.BoardName) / sizeof (CHAR8));
    CopyMem (&PlatformInfo, &BoardConfig, sizeof (PLATFORM_INFO));
    PlatformInfo.UpdateData = TRUE;
  }
#else
  Status = GetPlatformInfo (&PlatformInfoHob);
  if (!EFI_ERROR(Status)) {
    CopyMem (&PlatformInfo, PlatformInfoHob, sizeof (PLATFORM_INFO));
    PlatformInfo.EcPresent = FALSE;
  }

#if (defined CRB_BOARD && (CRB_BOARD != 9)) // Normal Case
  CreateBoardPrivateData (PlatformInfo.BoardID, PlatformInfo.BoardID);
#elif CRB_BOARD == 9 // Kaby Lake S DDR4 uDIMM Crb Case
  CreateBoardPrivateData (PlatformInfo.BoardID, BoardIdKabyLakeSDdr4UdimmCrb);
#endif // End of CRB_BOARD
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
  
//AMI_OVERRIDE_START - Only for CRB EC
#if 0
  if (PlatformInfo.EcPresent == FALSE) {
    ///
    /// Restore LPC IO decode setting
    ///
    PchLpcIoEnableDecodingSet (LpcIoeOrg);
  }

#endif 
//AMI_OVERRIDE_END - Only for CRB EC

#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - Take care by AMI setup
#if 0
  Status = GetBiosVersionDateTime (BiosVersion, ReleaseDate, ReleaseTime);
  CopyMem (&PlatformInfo.BiosVersion, &BiosVersion, sizeof (BiosVersion));
  CopyMem (&PlatformInfo.ReleaseDate, &ReleaseDate, sizeof (ReleaseDate));
  CopyMem (&PlatformInfo.ReleaseTime, &ReleaseTime, sizeof (ReleaseTime));
#endif
//AMI_OVERRIDE_END - Take care by AMI setup 
#endif //MINTREE_FLAG
  //
  // Updates the wakeupType which will be used to update the same in Smbios table 01
  //
  GetWakeupEventAndSaveToHob (&PlatformInfo.WakeUpType);

  ///
  /// Build HOB for setup memory information
  ///
  Hob = BuildGuidDataHob (
          &gPlatformInfoHobGuid,
          &PlatformInfo,
          sizeof (PLATFORM_INFO)
          );
  if (Hob == NULL) {
    ///
    /// Cannot Build HOB
    ///
    DEBUG ((DEBUG_ERROR, "Can't create PlatformInfo Hob successfully\n"));
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
  }

  Status = InitBoardConfigBlockPreMem ();
  ASSERT_EFI_ERROR (Status);

  //
  // Configures the I2CGpioExpander
  //
//AMI_OVERRIDE_START - Add token to disable I2C initial since customer board may not have I/O expander
#if AMI_CRB_I2C_IO_EXPANDER_SUPPORT_FLAG
  if (PcdGetBool (PcdIoExpanderPresent)) {
    I2CGpioExpanderInitPreMem();
  }
#endif // End of AMI_CRB_I2C_IO_EXPANDER_SUPPORT_FLAG
//AMI_OVERRIDE_END - Add token to disable I2C initial since customer board may not have I/O expander

//AMI_OVERRIDE_START - Take care by AMI module
#if 0
  DetectBootMode (VariableServices);
#else
  AmiSbUpdateBootMode ((EFI_PEI_SERVICES **)PeiServices);
  DEBUG ((DEBUG_ERROR, "AmiSbUpdateBootMode end!\n"));
  IsBootWithNoChange ();  
#endif
//AMI_OVERRIDE_END - Take care by AMI module

  PeiServicesGetBootMode (&BootMode);
  //
  // Make sure the Variable "Setup" and "(RC)Setup" have been initialized in boot modes where it may not be.
  //
  if ((BootMode != BOOT_WITH_MINIMAL_CONFIGURATION) &&
      (BootMode != BOOT_ASSUMING_NO_CONFIGURATION_CHANGES) &&
      (BootMode != BOOT_ON_S5_RESUME) &&
      (BootMode != BOOT_ON_S4_RESUME) &&
      (BootMode != BOOT_ON_S3_RESUME)) {
    Status = PlatformVariableInit (PeiServices, NotifyDescriptor, Ppi);
    ASSERT_EFI_ERROR (Status);
  }

#ifndef MINTREE_FLAG
  //
  // After PlatformVariableInit, the Variable "Setup" has been initialized.
  // Cache the Variable "Setup" here to boost performance.
  //
  Status = SetupDataCacheInit ();
#endif

  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "Fail to get System Configuration and set the configuration to production mode!\n"));
//AMI_OVERRIDE_START - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token
#if 0
    FwConfig = 0;
#else
    FwConfig = 2;
#endif
//AMI_OVERRIDE_END - Fix EIP215494 System will hang on 0x55 if we disabled SecureMod_Support token
    SynchDelay = 0;
  } else {
    FwConfig = SystemConfiguration.FirmwareConfiguration;
    SynchDelay = SystemConfiguration.AlternateModeSynchDelay;
  }

  //
  // Sync variabel with Cmos.
  //
  UpdateDebugLevel (VariableServices);

//AMI_OVERRIDE_START - For AMI POST_TIME_TUNING_SUPPORT.
#if defined(POST_TIME_TUNING_SUPPORT) && (POST_TIME_TUNING_SUPPORT == 1)
  if (SystemConfiguration.FastBoot == 0) {
    if (SynchDelay != 0) {
      MicroSecondDelay (SynchDelay * STALL_ONE_MILLI_SECOND * 100); // SynchDelay * 0.1s
    }
  }
#else 
  if (SynchDelay != 0) {
    MicroSecondDelay (SynchDelay * STALL_ONE_MILLI_SECOND * 100); // SynchDelay * 0.1s
  }
#endif // End of POST_TIME_TUNING_SUPPORT
//AMI_OVERRIDE_END - For AMI POST_TIME_TUNING_SUPPORT.
#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - Only for CRB EC  
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  if ((PcdGetBool (PcdUsbcEcPdNegotiation)) && ((BootMode == BOOT_ON_S5_RESUME) ||
      (BootMode == BOOT_ON_S4_RESUME) ||
      (BootMode == BOOT_ON_S3_RESUME))) {
    //
    // Send this EC command during boot to notify EC and wait for response (EC_C_USBC_SX_EXIT_WAIT for Sx exit)
    //
    Status = UsbcSxExit (&EcDataBuffer);
  }
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
#endif // MINTREE_FLAG


  //
  // Initialize Intel PEI Platform Policy
  //
  PeiPolicyInitPreMem (FwConfig);


  ///
  /// Configure GPIO and SIO
  ///
//AMI_OVERRIDE_START - Only for CRB EC   
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  Status = BoardInitPreMem ();
#else
  Status = AmiBoardInit ((EFI_PEI_SERVICES **)PeiServices, PlatformInfo.BoardID);
//ray_override / [TAG-SupportDVIEdidLess] Spec. Changed : Support DVI EDID Less Feature / Hot-plug Internal Pull High / Added >>
{
  UINT16                           GpioTableCount ;
  
  if ( SystemConfiguration.DviEdidLessMode[0] )
  {
    GpioTableCount = sizeof (DVI_1_HotPlug_GpioTermWpu20K) / sizeof (GPIO_INIT_CONFIG) ;
    GpioConfigurePads (GpioTableCount, DVI_1_HotPlug_GpioTermWpu20K);
  }
  else
  {
    GpioTableCount = sizeof (DVI_1_HotPlug_GpioTermNone) / sizeof (GPIO_INIT_CONFIG) ;
    GpioConfigurePads (GpioTableCount, DVI_1_HotPlug_GpioTermNone);
  }

  if ( SystemConfiguration.DviEdidLessMode[1] )
  {
    GpioTableCount = sizeof (DVI_2_HotPlug_GpioTermWpu20K) / sizeof (GPIO_INIT_CONFIG) ;
    GpioConfigurePads (GpioTableCount, DVI_2_HotPlug_GpioTermWpu20K);
  }
  else
  {
    GpioTableCount = sizeof (DVI_2_HotPlug_GpioTermNone) / sizeof (GPIO_INIT_CONFIG) ;
    GpioConfigurePads (GpioTableCount, DVI_2_HotPlug_GpioTermNone);
  }
}
//ray_override / [TAG-SupportDVIEdidLess] Spec. Changed : Support DVI EDID Less Feature / Hot-plug Internal Pull High / Added <<
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
//AMI_OVERRIDE_END - Only for CRB EC
  ASSERT_EFI_ERROR (Status);

//AMI_OVERRIDE_START - Take care by AMI module
#if 0
  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = CreateFastBootHob (VariableServices);
    ASSERT_EFI_ERROR (Status);
  }
#endif
// AMI_OVERRIDE_END - Take care by AMI module
#ifndef MINTREE_FLAG
  Status = PeiServicesNotifyPpi (&mWdtPpiNotifyList);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesNotifyPpi (&mEfiPeiSmbus2PpiNotifyList);
  ASSERT_EFI_ERROR (Status);
#endif //MINTREE_FLAG

  Status = PeiServicesNotifyPpi (&mSiPreMemPolicyNotifyList);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Do basic PCH init
  ///
  PlatformPchInit (VariableServices, &PlatformInfo.EcPresent);

#ifndef MINTREE_FLAG
  ///
  /// Set what state (S0/S5) to go to when power is re-applied after a power failure (G3 state)
  ///
  SetTheStateToGoAfterG3 (VariableServices);
#endif // MINTREE_FLAG

  ///
  /// Clear all pending SMI. On S3 clear power button enable so it will not generate an SMI.
  ///
  IoWrite16 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_EN, 0);
  IoWrite32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_GPE0_EN_127_96, 0);
  ///----------------------------------------------------------------------------------
  ///
  /// BIOS should check the WAK_STS bit in PM1_STS[15] (PCH register ABASE+00h) before memory
  /// initialization to determine if ME has reset the system while the host was in a sleep state.
  /// If WAK_STS is not set, BIOS should ensure a non-sleep exit path is taken by overwriting
  /// PM1_CNT[12:10] (PCH register ABASE+04h) to 111b to force an s5 exit.
  ///
  PchAcpiBaseGet (&ABase);
  Pm1Sts = IoRead16 (ABase + R_PCH_ACPI_PM1_STS);
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) == 0) {
    Pm1Cnt = IoRead32 (ABase + R_PCH_ACPI_PM1_CNT);
    Pm1Cnt |= V_PCH_ACPI_PM1_CNT_S5;
    IoWrite32 (ABase + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
  }
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  if (BootMode == BOOT_ON_S3_RESUME) {
    //
    // A platform PEIM should enable R/W access to E/F segment in the S3 boot path
    // otherwise, this AP wakeup buffer can't be accessed during CPU S3 operation.
    //
    McD0BaseAddress = MmPciBase (SA_MC_BUS, 0, 0);
    MmioWrite8 (McD0BaseAddress + R_SA_PAM0, 0x30);
    MmioWrite8 (McD0BaseAddress + R_SA_PAM5, 0x33);
    MmioWrite8 (McD0BaseAddress + R_SA_PAM6, 0x33);
  }

  ///
  /// Install Pre Memory PPIs
  ///
  Status = PeiServicesInstallPpi (&mPreMemPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  Platform Init before memory PEI module entry point

  @param[in]  FileHandle           Not used.
  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_SUCCESS          The function completes successfully
  @retval     EFI_OUT_OF_RESOURCES Insufficient resources to create database
**/
EFI_STATUS
EFIAPI
PlatformInitPreMemEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;
#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - AMI did not use
#if 0
  BIOS_ID_IMAGE BiosIdImage;

#endif
//AMI_OVERRIDE_END - AMI did not use
  ///@todo this functionality should be moved to Si Pkg.
  ///
  /// Allocate an initial buffer from heap for debugger use
  ///
  DEBUG_CODE(DsAllocation (););

//AMI_OVERRIDE_START - AMI did not use
#if 0
  Status = GetBiosId (&BiosIdImage);

  if (Status == EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "+==================================================+\n"));
    DEBUG ((EFI_D_ERROR, "| BIOS version [%s]  |\n", &BiosIdImage.BiosIdString));
    DEBUG ((EFI_D_ERROR, "+==================================================+\n"));
  }
#endif
//AMI_OVERRIDE_END - AMI did not use
#endif //MINTREE_FLAG
  ///@todo it should be moved to Si Pkg.
  ///
  /// Do Early PCH init
  ///
  EarlyPlatformPchInit ();

  TimerInit ();

  ///
  /// Performing PlatformInitPreMem after PeiReadOnlyVariable2 PPI produced
  ///
  Status = PeiServicesNotifyPpi (&mPreMemNotifyList);

  ///
  /// After code reorangized, memorycallback will run because the PPI is already
  /// installed when code run to here, it is supposed that the InstallEfiMemory is
  /// done before.
  ///
  Status = PeiServicesNotifyPpi (&mMemDiscoveredNotifyList);

  return Status;
}
