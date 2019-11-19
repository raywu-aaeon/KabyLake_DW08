/** @file
  Source code file for Platform Init PEI module

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

#include <CpuRegs.h>
#include <PchAccess.h>
#include <SetupVariable.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PchInfoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/GpioLib.h>
#include <Library/PeiPolicyInitLib.h>
#ifndef MINTREE_FLAG
#include <Library/PeiTbtInitLib.h>
#include <Library/PeiReadWriteVariableLib.h>
#include <Library/PcdValueUpdateLib.h>
#endif // MINTREE_FLAG
#include <Ppi/EndOfPeiPhase.h>
#include <Library/MtrrLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/SmramMemoryReserve.h>
#include <Library/ConfigBlockLib.h>
#include <Ppi/SiPolicy.h>
#include <SaPolicyCommon.h>
#include <Library/PeiPlatformHookLib.h>

#include <Guid/FirmwareFileSystem2.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Guid/SystemNvDataGuid.h>
#include <FlashArea.h> // AdvancedFeaturesContent

#include <AttemptUsbFirst.h> // AdvancedFeaturesContent
#include <Library/BoardConfigLib.h>
#include <Ppi/I2cMaster.h>
#include <GpioPinsSklLp.h>
#include <Library/TimerLib.h>
#include <Library/PeiPlatformHookLib.h>
#include "PlatformBoardId.h"
#include <Library/I2cAccessLib.h>
#ifndef MINTREE_FLAG
#include <MipiBridgeConfig.h>
#endif // MINTREE_FLAG
#include <Library/EcLib.h>
#include <Library/CpuPlatformLib.h>
//AMI_OVERRIDE_START - Program SSID by AMI parameter 
#ifndef INSTALL_INTEL_SSID 
#include <SbPeiLib.h>
#endif
//AMI_OVERRIDE_END - Program SSID by AMI parameter

#define  MEPciD22F0RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (0 << 12))
#define  MEPciD22F1RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (1 << 12))
#define  MEPciD22F2RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (2 << 12))
#define  MEPciD22F3RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (3 << 12))
#define  MEPciD22F4RegBase       (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (22 << 15) + (UINT32) (4 << 12))

#define  I2C_SAR_ADDRESS_SDS     0x28
#define  I2C_WRITE               0

#define GET_OCCUPIED_SIZE(ActualSize, Alignment) \
  ((ActualSize) + (((Alignment) - ((ActualSize) & ((Alignment) - 1))) & ((Alignment) - 1)))

EFI_STATUS
EFIAPI
PlatformInitEndOfPei (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR  mEndOfPeiNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) PlatformInitEndOfPei
};

/**
  Update MTRR setting and set write back as default memory attribute.

  @retval  EFI_SUCCESS  The function completes successfully.
  @retval  Others       Some error occurs.
**/
EFI_STATUS
EFIAPI
SetCacheMtrrAfterEndOfPei (
  VOID
  )
{
  EFI_STATUS                            Status;
  MTRR_SETTINGS                         MtrrSetting;
  EFI_PEI_HOB_POINTERS                  Hob;
  UINT64                                MemoryBase;
  UINT64                                MemoryLength;
  UINT64                                Power2Length;
  EFI_BOOT_MODE                         BootMode;
  UINTN                                 Index;
  UINT64                                SmramSize;
  UINT64                                SmramBase;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramHobDescriptorBlock;
#ifndef MINTREE_FLAG
  SI_PREMEM_POLICY_PPI                  *SiPreMemPolicyPpi;
  SA_MISC_PEI_PREMEM_CONFIG             *MiscPeiPreMemConfig;
  UINT32                                IedSize;
#endif //MINTREE_FLAG
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }
#ifndef MINTREE_FLAG
  IedSize = 0;
  SiPreMemPolicyPpi = NULL;
  MiscPeiPreMemConfig = NULL;
  //
  // Obtain SA policy settings.
  //
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);
  if (SiPreMemPolicyPpi != NULL) {
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *) &MiscPeiPreMemConfig);
    ASSERT_EFI_ERROR (Status);
    if (MiscPeiPreMemConfig != NULL) {
      IedSize = MiscPeiPreMemConfig->IedSize;
    }
  }
#endif //MINTREE_FLAG
  //
  // Clear the CAR Settings
  //
  ZeroMem(&MtrrSetting, sizeof(MTRR_SETTINGS));

  //
  // Default Cachable attribute will be set to WB to support large memory size/hot plug memory
  //
  MtrrSetting.MtrrDefType &= ~((UINT64)(0xFF));
  MtrrSetting.MtrrDefType |= (UINT64) CacheWriteBack;

  //
  // Set fixed cache for memory range below 1MB
  //
  Status = MtrrSetMemoryAttributeInMtrrSettings (
                         &MtrrSetting,
                         0x0,
                         0xA0000,
                         CacheWriteBack
                         );
  ASSERT_EFI_ERROR (Status);

  Status = MtrrSetMemoryAttributeInMtrrSettings (
                         &MtrrSetting,
                         0xA0000,
                         0x20000,
                         CacheUncacheable
                         );
  ASSERT_EFI_ERROR (Status);

  Status = MtrrSetMemoryAttributeInMtrrSettings (
                         &MtrrSetting,
                         0xC0000,
                         0x40000,
                         CacheWriteProtected
                         );
  ASSERT_EFI_ERROR ( Status);

  //
  // PI SMM IPL can't set SMRAM to WB because at that time CPU ARCH protocol is not available.
  // Set cacheability of SMRAM to WB here to improve SMRAM initialization performance.
  //
  SmramSize = 0;
  SmramBase = 0;
  Status = PeiServicesGetHobList ((VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
      if (CompareGuid (&Hob.Guid->Name, &gEfiSmmPeiSmramMemoryReserveGuid)) {
        SmramHobDescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) (Hob.Guid + 1);
        for (Index = 0; Index < SmramHobDescriptorBlock->NumberOfSmmReservedRegions; Index++) {
          if (SmramHobDescriptorBlock->Descriptor[Index].PhysicalStart > 0x100000) {
            SmramSize += SmramHobDescriptorBlock->Descriptor[Index].PhysicalSize;
            if (SmramBase == 0 || SmramBase > SmramHobDescriptorBlock->Descriptor[Index].CpuStart) {
              SmramBase = SmramHobDescriptorBlock->Descriptor[Index].CpuStart;
            }
          }
        }
        break;
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  //
  // Set non system memory as UC
  //
  MemoryBase   = 0x100000000;

  //
  // Add IED size to set whole SMRAM as WB to save MTRR count
  //
  SmramSize += IedSize;  // AdvancedFeaturesContent
  MemoryLength = MemoryBase - (SmramBase + SmramSize);
  while (MemoryLength != 0) {
    Power2Length = GetPowerOfTwo64 (MemoryLength);
    MemoryBase -= Power2Length;
    Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                MemoryBase,
                Power2Length,
                CacheUncacheable
                );
    ASSERT_EFI_ERROR (Status);
    MemoryLength -= Power2Length;
  }

  //
  // Update MTRR setting from MTRR buffer
  //
  MtrrSetAllMtrrs (&MtrrSetting);

  return Status;
}
#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - Only for Skylake SDS
#if 0  
/**
  Reset the SAR Sensor to ensure the SAR sensor can trigger an edge to the WWAN module on SKL SDS for FCC compliance.

  @retval  EFI_SUCCESS  The function completes successfully.
  @retval  Others       Some error occurs.
**/
EFI_STATUS
EFIAPI
ResetSarSensorSklSds (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINTN                            Speed;
  EFI_I2C_REQUEST_PACKET           Packet;
  UINT8                            Buffer[2];
  UINTN                            Instance;
  EFI_PEI_I2C_MASTER_PPI           *MasterPpi;
  UINT32                           SimDetectGpio;
  UINT64                           Ticker;
  UINT64                           TimeNanoSeconds;
  UINT64                           DelayFromReset;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  SETUP_DATA                       SystemConfiguration;
  UINTN                            VariableSize;

  GpioGetInputValue(GPIO_SKL_LP_GPP_F23, &SimDetectGpio);

  //@ToDo Optimize for no SIM card
  //if (SimDetectGpio == 1) { //No Sim Detected
  //  //GpioSetOutputValue (GPIO_SKL_LP_GPP_G5, 0);
  //  return EFI_SUCCESS;
  //}

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,  // GUID
             0,                                 // INSTANCE
             NULL,                              // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices        // PPI
             );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "ResetSarSensorSklSds: Failed to find PEI Read Variable support. Status = %r\n", Status));
  }
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );

  //
  // If WWAN is disabled skip SAR Reset Delay, else get the reset delay from the BIOS menu option
  //
  if (EFI_ERROR(Status) || !SystemConfiguration.WWANEnable) {
    DEBUG ((DEBUG_ERROR, "ResetSarSensorSklSds: Either GetVariable failed or WWAN is disabled. Status = %r\n", Status));
    return (Status);
  }
  DelayFromReset = SystemConfiguration.I2cSarResetDelay;
  DEBUG ((DEBUG_INFO, "ResetSarSensorSklSds: Specified delay is %d seconds.\n", DelayFromReset));

  //
  // Wait for the reset delay (number of seconds after reset until SAR Sensor reset can be issued)
  //
  Ticker = GetPerformanceCounter();
  TimeNanoSeconds = GetTimeInNanoSecond (Ticker);

  while (TimeNanoSeconds < MultU64x32(DelayFromReset, 1000000000)) {
    MicroSecondDelay(200);
    Ticker = GetPerformanceCounter();
    TimeNanoSeconds = GetTimeInNanoSecond (Ticker);
  }

  //
  // Issue SAR Sensor Reset to trigger edge to WWAN module
  //
  Speed = 0;
  Instance = 0;
  Status = EFI_SUCCESS;
  while(TRUE) {
    Status = PeiServicesLocatePpi (&gEfiPeiI2cMasterPpiGuid, Instance, NULL, (VOID **) &MasterPpi);
    if (EFI_ERROR (Status)) {
      break;
    }
    if (CompareGuid (&(MasterPpi->Identifier), &gI2c0MasterGuid)) { // We know that the I2C device is I2C0 for SKL SDS
      break;
    }
    Instance++;
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ResetSarSensorSklSds: Fail to Locate I2C PPI! Status = %r\n", Status));
  } else {
    Speed = (UINTN)(100000);
    Status = MasterPpi->SetBusFrequency(MasterPpi,&Speed);

    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "ResetSarSensorSklSds: SetBusFrequency Failed. Status = %r\n", Status));
      return (Status);
    }

    Packet.OperationCount = 1;
    Packet.Operation[0].Flags = I2C_WRITE;
    Packet.Operation[0].LengthInBytes = 2;
    Packet.Operation[0].Buffer = Buffer;
    Buffer[0] = 0x7f;
    Buffer[1] = 0xde;
    Status = MasterPpi->StartRequest(MasterPpi, I2C_SAR_ADDRESS_SDS, &Packet); //SAR Sensor I2C address is 0x28 on SKL SDS
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "ResetSarSensorSklSds: StartRequest Failed. Status = %r\n", Status));
      return (Status);
    }

    DEBUG ((DEBUG_INFO, "Finished I2C 0x28 write to SAR sensor\n"));
  }

  return (Status);
}
#endif
//AMI_OVERRIDE_END - Only for Skylake SDS
#endif // MINTREE_FLAG

/**
  This function handles PlatformInit task at the end of PEI

  @param[in]  PeiServices  Pointer to PEI Services Table.
  @param[in]  NotifyDesc   Pointer to the descriptor for the Notification event that
                           caused this function to execute.
  @param[in]  Ppi          Pointer to the PPI data associated with this function.

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
EFIAPI
PlatformInitEndOfPei (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                    Status;
  PLATFORM_INFO                 *PlatformInfo;
#ifndef MINTREE_FLAG
  UINTN                         PciCfgBase;
  UINT8                         Value;
  PCH_SERIAL_IO_CONTROLLER      I2CPort;
#endif // MINTREE_FLAG

  Status = SetCacheMtrrAfterEndOfPei ();
  ASSERT_EFI_ERROR (Status);

#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - Skip TBT function when disabling Thunderbolt_SUPPORT
#if defined(Thunderbolt_SUPPORT) && Thunderbolt_SUPPORT
  Status = TbtInit ();
#endif
//AMI_OVERRIDE_END - Skip TBT function when disabling Thunderbolt_SUPPORT
#endif // MINTREE_FLAG
  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR (Status);
  if (PlatformInfo == NULL) {
    return Status;
  }

#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - Only for Skylake SDS
#if 0  
  if (PcdGetBool (PcdSarSensorResetWA)) {
    Status = ResetSarSensorSklSds();
    if (Status != EFI_SUCCESS) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }
#endif
//AMI_OVERRIDE_END - Only for Skylake SDS

  if (PcdGetBool(PcdMipiBridgeConfigInit)) {
    Value = PcdGet8(PcdMipiBridgeI2CPort);
    DEBUG((DEBUG_INFO, "Configure MIPI Bridge on I2C%d - Start\n", Value));
    ///
    /// Ensure that the I2C is in in D0 power state
    ///
    I2CPort = PchSerialIoIndexI2C0;
    if (Value == 4) {
      I2CPort = PchSerialIoIndexI2C4;
    }
    DEBUG((DEBUG_INFO, "  Ensure I2C controller is in D0 state\n"));
    PciCfgBase = MmPciBase(0, GetSerialIoDeviceNumber(I2CPort), GetSerialIoFunctionNumber(I2CPort));
    MmioOr32(PciCfgBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
    MmioAnd32(PciCfgBase + R_PCH_SERIAL_IO_PME_CTRL_STS, (UINT32)~B_PCH_SERIAL_IO_PME_CTRL_STS_PWR_ST);

    ///
    /// Send the command block (script) to the MIPI bridge
    ///
    Status = I2cBlockWriteRead(I2CPort, GMT_MIPI_BRIDGE_I2C_ADDRESS, (UINT8 *)GmtMipiBridgeI2cScript_Config, sizeof(GmtMipiBridgeI2cScript_Config));
    if (Status != EFI_SUCCESS) {
      DEBUG((DEBUG_ERROR, "Configure MIPI Bridge - Failed to configure MIPI bridge\n"));
    } else {
      DEBUG((DEBUG_INFO, "Configure MIPI Bridge - End\n"));
    }
  }
#endif // MINTREE_FLAG
  return Status;
}


/**
  Clear any SMI status or wake status left from boot.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
ClearIchSmiAndWake (
  VOID
  )
{
  EFI_STATUS          Status;
  EFI_BOOT_MODE       BootMode;
  UINT16              ABase;
  UINT16              Pm1Sts;
  UINT32              Pm1Cnt;
  UINT32              Data32;
  UINT32              Gpe0Sts;
  UINT32              SmiSts;
  UINT16              DevActSts;
  UINT16              TcoBase;
  UINT16              Tco1Sts;
  UINT16              LpcDeviceId;
  UINTN               LanBaseAddress;
  UINT16              LanPmcs;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  PchAcpiBaseGet (&ABase);

  PchTcoBaseGet (&TcoBase);

  //
  // Read the ACPI registers
  //
  Pm1Sts  = IoRead16 (ABase + R_PCH_ACPI_PM1_STS);
  Pm1Cnt  = IoRead32 (ABase + R_PCH_ACPI_PM1_CNT);

  Gpe0Sts = IoRead32 (ABase + R_PCH_ACPI_GPE0_STS_127_96);
  SmiSts  = IoRead32 (ABase + R_PCH_SMI_STS);

  Data32 = 0;
  //
  // Before clear B_PCH_ACPI_GPE0_STS_127_96_PME_B0, PMES bit of internal device must be cleared at first.
  //
  Data32 = (Gpe0Sts & B_PCH_ACPI_GPE0_STS_127_96_PME_B0);
  if (Data32 != 0) {
    //
    // Check if Internal LAN waked up system
    //
    LanBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_LAN,
                       PCI_FUNCTION_NUMBER_PCH_LAN
                       );
    LanPmcs = MmioRead16 (LanBaseAddress + R_PCH_LAN_PMCS);
    if ((LanPmcs & B_PCH_LAN_PMCS_PMES) != 0) {
      //
      // Clear PMES bit
      //
      MmioOr16 (LanBaseAddress + R_PCH_LAN_PMCS, B_PCH_LAN_PMCS_PMES);
      DEBUG ((DEBUG_INFO, "Clear LAN PMES bit!\n"));
    }
  }

  LpcDeviceId = GetLpcDid ();
  //
  // Clear any SMI or wake state from the boot
  //
  Pm1Sts |=
    (
      B_PCH_ACPI_PM1_STS_WAK |
      B_PCH_ACPI_PM1_STS_PRBTNOR |
      B_PCH_ACPI_PM1_STS_RTC |
      B_PCH_ACPI_PM1_STS_PWRBTN |
      B_PCH_ACPI_PM1_STS_GBL |
      B_PCH_ACPI_PM1_STS_TMROF
    );

  Gpe0Sts |=
    (
      B_PCH_ACPI_GPE0_STS_127_96_PME_B0 |
      B_PCH_ACPI_GPE0_STS_127_96_PME |
      B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP |
      B_PCH_ACPI_GPE0_STS_127_96_RI |
      B_PCH_ACPI_GPE0_STS_127_96_SMB_WAK |
      B_PCH_ACPI_GPE0_STS_127_96_TC0SCI |
      B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG |
      B_PCH_ACPI_GPE0_STS_127_96_LAN_WAKE
    );

  //
  // PCH Desktop
  //
  if (IS_PCH_LPC_DEVICE_ID_MOBILE (LpcDeviceId)) {
    Gpe0Sts |= B_PCH_ACPI_GPE0_STS_127_96_BATLOW;
  }

  SmiSts |=
    (
      B_PCH_SMI_STS_SMBUS |
      B_PCH_SMI_STS_PERIODIC |
      B_PCH_SMI_STS_TCO |
      B_PCH_SMI_STS_MCSMI |
      B_PCH_SMI_STS_SWSMI_TMR |
      B_PCH_SMI_STS_APM |
      B_PCH_SMI_STS_ON_SLP_EN |
      B_PCH_SMI_STS_BIOS
    );
  //
  // Write them back
  //
  IoWrite16 (ABase + R_PCH_ACPI_PM1_STS,  Pm1Sts);
  IoWrite32 (ABase + R_PCH_ACPI_GPE0_STS_127_96, Gpe0Sts);
  IoWrite32 (ABase + R_PCH_SMI_STS,  SmiSts);

  DevActSts = IoRead16 (ABase + R_PCH_DEVACT_STS);
  Tco1Sts = IoRead16 (TcoBase + R_PCH_TCO1_STS);

  DevActSts |=
    (
      B_PCH_DEVACT_STS_KBC |
      B_PCH_DEVACT_STS_PIRQDH |
      B_PCH_DEVACT_STS_PIRQCG |
      B_PCH_DEVACT_STS_PIRQBF |
      B_PCH_DEVACT_STS_PIRQAE
    );
  Tco1Sts |=
    (
      B_PCH_TCO1_STS_DMISERR |
      B_PCH_TCO1_STS_DMISMI |
      B_PCH_TCO1_STS_DMISCI |
      B_PCH_TCO1_STS_BIOSWR |
      B_PCH_TCO1_STS_NEWCENTURY |
      B_PCH_TCO1_STS_TIMEOUT |
      B_PCH_TCO1_STS_TCO_INT |
      B_PCH_TCO1_STS_SW_TCO_SMI
    );

  //
  // clear GPI SMI STS
  //
  GpioClearAllGpiSmiSts ();

  IoWrite16 (TcoBase + R_PCH_TCO1_STS, Tco1Sts);

  //
  // We do not want to write 1 to clear INTRD_DET bit.
  //
  IoWrite16 ((UINTN) (TcoBase + R_PCH_TCO2_STS), (UINT16) ~B_PCH_TCO2_STS_INTRD_DET);

  IoWrite16 (ABase + R_PCH_DEVACT_STS, DevActSts);

  //
  // Clear Power Failure (PWR_FLR), It was used to check S3_Resume State.
  //
  // MmioOr8 (
  //   LpcBaseAddress + R_PCH_PMC_GEN_PMCON_B,
  //   B_PCH_PMC_GEN_PMCON_B_PWR_FLR
  //   );

  return EFI_SUCCESS;
}

//
// AdvancedFeaturesBegin
//
//@todo Review this functionality and if it is required for SKL SDS
/**
  Create the HOB for hotkey status for 'Attempt USB First' feature

  @retval  EFI_SUCCESS  HOB Creating successful.
  @retval  Others       HOB Creating failed.
**/
//AMI_OVERRIDE_START - Take care by AMI Flash module
#if 0
EFI_STATUS
CreateAttemptUsbFirstHotkeyInfoHob (
  VOID
  )
{
  EFI_STATUS                     Status;
  ATTEMPT_USB_FIRST_HOTKEY_INFO  AttemptUsbFirstHotkeyInfo;
  VOID                           *Hob;

  Status = EFI_SUCCESS;

  ZeroMem (
    &AttemptUsbFirstHotkeyInfo,
    sizeof (AttemptUsbFirstHotkeyInfo)
    );

  AttemptUsbFirstHotkeyInfo.RevisonId = 0;
  AttemptUsbFirstHotkeyInfo.HotkeyTriggered = FALSE;

  ///
  /// Build HOB for Attempt USB First feature
  ///
  Hob = BuildGuidDataHob (
          &gAttemptUsbFirstHotkeyInfoHobGuid,
          &(AttemptUsbFirstHotkeyInfo),
          sizeof (ATTEMPT_USB_FIRST_HOTKEY_INFO)
          );

  return Status;
}
#endif
//AMI_OVERRIDE_END - Take care by AMI Flash module
//
// AdvancedFeaturesEnd
//

/**
  Program ME PCI Device Sub-system VID & SID.

**/
VOID
ProgramMePciSubSystemVidDid (
  VOID
  )
{
  ///@todo it should be moved to Si Pkg.
  ///
  /// Program SSVID/SSID on various devices
  ///
  if (MmioRead32 (MEPciD22F0RegBase + 0x0) != 0xFFFFFFFF) {
    MmioWrite32 (MEPciD22F0RegBase + 0x2C, PcdGet32 (PcdPciD22SubsystemVidDid));
  }

  if (MmioRead32 (MEPciD22F1RegBase + 0x0) != 0xFFFFFFFF) {
    MmioWrite32 (MEPciD22F1RegBase + 0x2C, PcdGet32 (PcdPciD22SubsystemVidDid));
  }

  if (MmioRead32 (MEPciD22F2RegBase + 0x0) != 0xFFFFFFFF) {
    MmioWrite32 (MEPciD22F2RegBase + 0x2C, PcdGet32 (PcdPciD22SubsystemVidDid));
  }

  if (MmioRead32 (MEPciD22F3RegBase + 0x0) != 0xFFFFFFFF) {
    MmioWrite32 (MEPciD22F3RegBase + 0x2C, PcdGet32 (PcdPciD22SubsystemVidDid));
  }

  if (MmioRead32 (MEPciD22F4RegBase + 0x0) != 0xFFFFFFFF) {
    MmioWrite32 (MEPciD22F4RegBase + 0x2C, PcdGet32 (PcdPciD22SubsystemVidDid));
  }
}

#ifndef MINTREE_FLAG
/**
Search and identify the physical address of a
file module inside the BIOSGUARD_MODULE_FV_SIGNED

@retval  EFI_SUCCESS  If address has been found
@retval  Others       If address has not been found
**/
//AMI_OVERRIDE_START >>> fix - BIOS GUARD has no function when enable the BIOS GUARD setup item.
/*
static EFI_STATUS
FindBiosGuardModuleInFlash(
IN EFI_FIRMWARE_VOLUME_HEADER *FvHeader,
IN EFI_GUID                   *GuidPtr,
IN OUT UINT32                 *ModulePtr,
IN OUT UINT32                 *ModuleSize
)
{
  EFI_FFS_FILE_HEADER        *FfsHeader;
  EFI_COMMON_SECTION_HEADER  *SectionHeader;
  UINT32                     FileLength;
  UINT32                     FileOccupiedSize;
  EFI_GUID                   *SectionDefinitionGuid;
  UINT32                     DataOffset;
// AMI_OVERRIDE_START >>> fix build failed issue
  BOOLEAN                    WasFound = FALSE;
// AMI_OVERRIDE_END <<< fix build failed issue
  ///
  /// Locate Firmware File System file within Firmware Volume
  ///
  FfsHeader = (EFI_FFS_FILE_HEADER*)((UINT8*)FvHeader + FvHeader->HeaderLength);

  ///
  /// Locate GUIDED section header
  ///
  if (IS_FFS_FILE2(FfsHeader)) {
    SectionHeader = (EFI_COMMON_SECTION_HEADER *)((UINTN)FfsHeader + sizeof(EFI_FFS_FILE_HEADER2));
  } else {
    SectionHeader = (EFI_COMMON_SECTION_HEADER *)((UINTN)FfsHeader + sizeof(EFI_FFS_FILE_HEADER));
  }

  ///
  /// the BIOS Guard module is encapsulated in the FDF as such:
  ///   FILE_FV  - BIOSGUARD_MODULE_FV_SIGNED
  ///     FFS File Header
  ///       EFI_GUID_DEFINED_SECTION
  ///         WIN_CERTIFICATE_UEFI_GUID
  ///         EFI_CERT_BLOCK_RSA_2048_SHA256
  ///         BIOSGUARD_MODULE_FV
  ///           FFS File Header
  ///             BIOS Guard module binary
  ///           FFS File Header
  ///             BIOS Guard module binary
  ///

  ///
  /// check if this is guided section
  ///
  if (SectionHeader->Type != EFI_SECTION_GUID_DEFINED) {
    return EFI_NOT_FOUND;
  }
  if (IS_SECTION2(SectionHeader)) {
    SectionDefinitionGuid = &((EFI_GUID_DEFINED_SECTION2 *)SectionHeader)->SectionDefinitionGuid;
    DataOffset = ((EFI_GUID_DEFINED_SECTION2 *)SectionHeader)->DataOffset;
  }
  else {
    SectionDefinitionGuid = &((EFI_GUID_DEFINED_SECTION *)SectionHeader)->SectionDefinitionGuid;
    DataOffset = ((EFI_GUID_DEFINED_SECTION *)SectionHeader)->DataOffset;
  }
  ///
  /// check if guided section is signed
  ///
  if (!CompareGuid (SectionDefinitionGuid, &gEfiFirmwareContentsSignedGuid)) {
    return EFI_NOT_FOUND;
  }

  //
  // Advance to next FV (BIOSGUARD_MODULE_FV)
  //
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)((UINT8*)SectionHeader + \
    DataOffset + \
    sizeof(WIN_CERTIFICATE_UEFI_GUID) + \
    sizeof(EFI_CERT_BLOCK_RSA_2048_SHA256));
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) ALIGN_POINTER ((UINTN)FvHeader, 16);

  //
  // Traverse through the FFS of the BIOSGUARD_MODULE_FV
  //
// AMI_OVERRIDE_START >>> fix build failed issue
//  BOOLEAN WasFound = FALSE;
// AMI_OVERRIDE_END <<< fix build failed issue
  FfsHeader = (EFI_FFS_FILE_HEADER*)((UINT8*)FvHeader + FvHeader->HeaderLength);
  do {
    if (IS_FFS_FILE2(FfsHeader)) {
      FileLength = FFS_FILE2_SIZE(FfsHeader);
      ASSERT(FileLength > 0x00FFFFFF);
    }
    else {
      FileLength = FFS_FILE_SIZE(FfsHeader);
    }
    //
    // FileLength is adjusted to FileOccupiedSize as it is 8 byte aligned.
    //
    FileOccupiedSize = (UINT32)GET_OCCUPIED_SIZE(FileLength, 8);

    //
    // check for matching FFS guid
    //
    if (CompareGuid(GuidPtr, &FfsHeader->Name)) {
      WasFound = TRUE;
      break;
    }

    //
    // advance to next FFS
    //
    FfsHeader = (EFI_FFS_FILE_HEADER *)(((UINT32)(UINT8 *)FfsHeader) + FileOccupiedSize);

  } while ((UINTN)FfsHeader < (UINTN)((UINT8 *)FvHeader + FvHeader->FvLength));

  if (WasFound) {
    ///
    /// BiosGuard binary is after FFS file header
    ///
    if (IS_FFS_FILE2(FfsHeader)) {
      *ModulePtr = (UINT32)((UINTN)FfsHeader + sizeof(EFI_FFS_FILE_HEADER2));
      *ModuleSize = FileLength - sizeof(EFI_FFS_FILE_HEADER);
    }
    else {
      *ModulePtr = (UINT32)((UINTN)FfsHeader + sizeof(EFI_FFS_FILE_HEADER));
      *ModuleSize = FileLength - sizeof(EFI_FFS_FILE_HEADER);
    }

    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}
*/
//AMI_OVERRIDE_END <<< fix - BIOS GUARD has no function when enable the BIOS GUARD setup item.

/**
  Get the BIOS Guard Module pointer.

  @param[in, out] ModulePtr  - Input is a NULL pointer,
                               and output points BIOS Guard module address if found.
  @param[out]     ModuleSize - UINT32 Input Output the BIOS Guard module size

  @retval EFI_SUCCESS       - BIOS Guard Module found.
  @retval EFI_NOT_FOUND     - BIOS Guard Module size and/or Address equal to 0.
  @retval Others            - BIOS Guard Module not found.
**/
EFI_STATUS
UpdateBiosGuardModulePtr (
  IN OUT EFI_PHYSICAL_ADDRESS   *ModulePtr,
  OUT    UINT32                 *ModuleSize
  )
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME_HEADER    *FvHeader;
  EFI_GUID                      *BiosGuardModuleGuidPtr;
  UINT32                        BiosGuardModuleAddr;
  UINT32                        BiosGuardModuleSize;
//AMI_OVERRIDE_START >>> fix - BIOS GUARD has no function when enable the BIOS GUARD setup item.
  EFI_FFS_FILE_HEADER           *FfsFile = NULL;
//AMI_OVERRIDE_END <<< fix - BIOS GUARD has no function when enable the BIOS GUARD setup item.
  Status     = EFI_SUCCESS;
  BiosGuardModuleAddr = 0;
  BiosGuardModuleSize = 0;

  if (IsCflSCpu () || ((GetCpuFamily () == EnumCpuKblUltUlx) && (GetCpuStepping () >= EnumAmlW0))) {
    BiosGuardModuleGuidPtr      = &gBiosGuardModuleCflGuid;
    DEBUG ((DEBUG_INFO, "Searching for CFL BIOS Guard Module binary\n"));
  } else {
    BiosGuardModuleGuidPtr      = &gBiosGuardModuleGuid;
    DEBUG ((DEBUG_INFO, "Searching for KBL BIOS Guard Module binary\n"));
  }

  ///
  /// Locate Firmware Volume header
  ///
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FixedPcdGet32 (PcdFlashBiosGuardModuleFvBase);

//AMI_OVERRIDE_START >>> fix - BIOS GUARD has no function when enable the BIOS GUARD setup item.
/*
  Status = FindBiosGuardModuleInFlash (FvHeader, BiosGuardModuleGuidPtr, &BiosGuardModuleAddr, &BiosGuardModuleSize);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "BIOS Guard Module not found\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  if ((BiosGuardModuleAddr == 0) || (BiosGuardModuleSize == 0)) {
    DEBUG ((DEBUG_ERROR, "BiosGuardModuleSize = 0x%X and/or BiosGuardModule = 0x%X equal to zero\n", BiosGuardModuleAddr, BiosGuardModuleSize));
    ASSERT_EFI_ERROR (Status);
    return EFI_NOT_FOUND;
  }
*/
  ///
  /// Locate Firmware File System file within Firmware Volume
  ///
  Status = PeiServicesFfsFindFileByName (BiosGuardModuleGuidPtr, FvHeader, (VOID **)&FfsFile);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  BiosGuardModuleAddr = (UINT32)((UINT8 *) FfsFile + sizeof (EFI_FFS_FILE_HEADER));
  BiosGuardModuleSize = FFS_FILE_SIZE(FfsFile) - sizeof(EFI_FFS_FILE_HEADER);
  DEBUG (( DEBUG_INFO, "BIOS Guard Module Location: %x, BIOS Guard Module Size: %x\n", BiosGuardModuleAddr, BiosGuardModuleSize));
  *ModulePtr  = (EFI_PHYSICAL_ADDRESS) BiosGuardModuleAddr;
  *ModuleSize = BiosGuardModuleSize;
//AMI_OVERRIDE_END <<< fix - BIOS GUARD has no function when enable the BIOS GUARD setup item.
  return Status;
}


/**
  Create and initialize BIOS Guard HOB
  @param[in] BootMode   Boot mode of this boot.
  @retval  EFI_SUCCESS  HOB Creating successful.
  @retval  Others       HOB Creating failed.
**/
EFI_STATUS
BiosGuardHobInit (
  IN EFI_BOOT_MODE                    BootMode
  )
{
  EFI_STATUS                       Status;
  PLATFORM_INFO                    *PlatformInfo;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  UINTN                            VariableSize;
  CPU_SETUP                        CpuSetupBuffer;
  CPU_SETUP                        *CpuSetup;
  BIOSGUARD_HOB                    *BiosGuardHobPtr;
  UINTN                            SpiBaseAddress;
  UINTN                            PchSpiBase;
  UINT32                           FlashBase = 0;
  UINT8                            BgpdtPkeySlot0[] = { 0x6a, 0x94, 0x81, 0x87, 0xfd, 0x8c, 0x51, 0x0a, 0x2a, 0x74, 0x9c, 0x7b, 0xe3, 0xf7, 0x5c, 0x3a, 0xbf, 0xce, 0x60, 0x4f, 0x68, 0xeb, 0xc6, 0x8f, 0x69, 0x98, 0x8c, 0xc5, 0xdb, 0xe3, 0x29, 0x70 };
  UINT8                            BgpdtPkeySlot1[] = { 0x1c, 0x51, 0x3a, 0xc5, 0x1c, 0x3d, 0x63, 0x10, 0xef, 0xed, 0xfe, 0xc5, 0xf3, 0x4b, 0xf2, 0xe0, 0x9b, 0x22, 0xe8, 0x06, 0xab, 0xd3, 0x19, 0x2e, 0xfa, 0xfb, 0x6e, 0xd9, 0x36, 0x0d, 0x68, 0x18 };
  UINT8                            BgpdtPkeySlot2[] = { 0x19, 0x0b, 0x33, 0xf8, 0xde, 0x3a, 0xa7, 0x9b, 0x57, 0xad, 0xb2, 0x45, 0x86, 0x0e, 0x7f, 0x0e, 0x40, 0x62, 0x80, 0x22, 0x8f, 0x04, 0x92, 0xec, 0x87, 0x44, 0x81, 0xd9, 0xef, 0xed, 0x9f, 0xa3 };
  UINT8                            EcStatus = 0;
  UINT8                            PlatIdStr[] = "KABYLAKE";
  UINT32                           Index = 0;

  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR ( Status);
  if (PlatformInfo == NULL) {
    return Status;
  }

  //
  // Make sure ReadOnlyVariablePpi is available
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (CPU_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"CpuSetup",
                               &gCpuSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &CpuSetupBuffer
                               );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  CpuSetup  = &CpuSetupBuffer;

  // @todo. Move below code to find flash size to PCH library
  SpiBaseAddress = MmPciBase (
      DEFAULT_PCI_BUS_NUMBER_PCH,
      PCI_DEVICE_NUMBER_PCH_SPI,
      PCI_FUNCTION_NUMBER_PCH_SPI
      );

  PchSpiBase = MmioRead32(SpiBaseAddress + R_PCH_SPI_BAR0) & ~B_PCH_SPI_BAR0_MASK;
  if (PchSpiBase != 0) {
    FlashBase = (UINT32) LShiftU64 ((*(UINT16*)(UINTN) PchSpiBase), 12);
  }

  Status = PeiServicesCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, sizeof (BIOSGUARD_HOB), (VOID **)&BiosGuardHobPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to create BIOS Guard Hob Pointer\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  ZeroMem (&(BiosGuardHobPtr->Bgpdt), (sizeof (BIOSGUARD_HOB) - sizeof (EFI_HOB_GUID_TYPE)));
  BiosGuardHobPtr->EfiHobGuidType.Name = gBiosGuardHobGuid;

  BiosGuardHobPtr->Bgpdt.BgpdtMajVer = BGPDT_MAJOR_VERSION;
  BiosGuardHobPtr->Bgpdt.BgpdtMinVer = BGPDT_MINOR_VERSION;
  if (IsCflSCpu () || ((GetCpuFamily () == EnumCpuKblUltUlx) && (GetCpuStepping () >= EnumAmlW0))) {
    BiosGuardHobPtr->Bgpdt.BgModSvn    = BIOSGUARD_SVN_CFL;
  } else {
    BiosGuardHobPtr->Bgpdt.BgModSvn    = BIOSGUARD_SVN;
  }
  BiosGuardHobPtr->Bgpdt.BiosSvn     = 0x00510000;

  BiosGuardHobPtr->Bgpdt.LastSfam = 0;
  BiosGuardHobPtr->Bgpdt.SfamData[0].FirstByte = FlashBase + FixedPcdGet32 (PcdFlashNvStorageSize);
  BiosGuardHobPtr->Bgpdt.SfamData[0].LastByte  = FlashBase + FixedPcdGet32 (PcdFlashFvRecoveryOffset) + FixedPcdGet32 (PcdFlashFvRecoverySize) - 1;
  CopyMem (&BiosGuardHobPtr->Bgpdt.PkeySlot0[0], &BgpdtPkeySlot0[0], sizeof (BgpdtPkeySlot0));
  CopyMem (&BiosGuardHobPtr->Bgpdt.PkeySlot1[0], &BgpdtPkeySlot1[0], sizeof (BgpdtPkeySlot1));
  CopyMem (&BiosGuardHobPtr->Bgpdt.PkeySlot2[0], &BgpdtPkeySlot2[0], sizeof (BgpdtPkeySlot2));
  BiosGuardHobPtr->BiosGuardLog.LastPage       = MAX_BIOSGUARD_LOG_PAGE - 1;
  BiosGuardHobPtr->BiosGuardLog.LoggingOptions = BIOSGUARD_LOG_OPT_DEBUG | BIOSGUARD_LOG_OPT_FLASH_ERROR | BIOSGUARD_LOG_OPT_FLASH_ERASE | BIOSGUARD_LOG_OPT_FLASH_WRITE | BIOSGUARD_LOG_OPT_BRANCH_TRACE | BIOSGUARD_LOG_OPT_STEP_TRACE;
  BiosGuardHobPtr->BiosGuardLog.Version        = BIOSGUARD_LOG_VERSION;

  if (PlatformInfo->EcPresent == TRUE) {
    DEBUG ((DEBUG_INFO, "EC is Present\n"));
    BiosGuardHobPtr->Bgpdt.BiosGuardAttr      |= EnumEcPresent;
    BiosGuardHobPtr->Bgpdt.EcCmd               = EC_C_PORT;
    BiosGuardHobPtr->Bgpdt.EcData              = EC_D_PORT;
    if (BootMode != BOOT_ON_S3_RESUME) {
      Status = SendEcCommand (EC_C_BIOSGUARD_DISCOVERY);
      if (Status == EFI_SUCCESS) {
        Status = ReceiveEcData (&EcStatus);
        PlatformInfo->BiosGuardEcStatus = EcStatus;
      } else {
        DEBUG ((DEBUG_ERROR, "SendEcCommand(EcCmdDiscovery) Failed\n"));
      }
    } else {
      EcStatus = PlatformInfo->BiosGuardEcStatus;
    }
    if (((EcStatus & 0xF8) == 0) && (EcStatus & BIT0)) {
      if (((EcStatus & (BIT2 | BIT1)) == (BIT2 | BIT1)) || (((EcStatus & (BIT2 | BIT1)) == 0))) {
        DEBUG ((DEBUG_INFO, "EC FW supports BiosGuard\n"));
        BiosGuardHobPtr->Bgpdt.BiosGuardAttr |= EnumEcBiosGuardProtected;
        BiosGuardHobPtr->Bgpdt.EcCmdGetSvn   = 0xB3;
        BiosGuardHobPtr->Bgpdt.EcCmdOpen     = 0xB4;
        BiosGuardHobPtr->Bgpdt.EcCmdClose    = 0xB5;
        BiosGuardHobPtr->Bgpdt.EcCmdPortTest = 0xB6;
        BiosGuardHobPtr->BgupHeader.EcSvn    = 0x00010000;
      } else {
        DEBUG ((DEBUG_WARN, "EC FW Error\n"));
      }
    } else {
      DEBUG ((DEBUG_WARN, "EC FW do not support BiosGuard\n"));
    }
  }
  if (CpuSetup->FlashWearOutProtection) {
    BiosGuardHobPtr->Bgpdt.BiosGuardAttr |= EnumFlashwearoutProtection;
  }
  BiosGuardHobPtr->Bgpdt.BiosGuardAttr |= EnumFtuEnable;

  BiosGuardHobPtr->BiosGuardModulePtr  = 0;
  BiosGuardHobPtr->BiosGuardModuleSize = 0;

  Status = UpdateBiosGuardModulePtr (&BiosGuardHobPtr->BiosGuardModulePtr, &BiosGuardHobPtr->BiosGuardModuleSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UpdateBiosGuardModulePtr not found\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  DEBUG ((DEBUG_INFO, "------------------ BIOS GUARD Module ------------------\n"));
  DEBUG ((DEBUG_INFO, " BIOS GUARD Module:: Product ID : 0x%X\n", *(UINT16*)(UINTN)(BiosGuardHobPtr->BiosGuardModulePtr + BIOSGUARD_PRODUCT_ID_OFFSET)));
  DEBUG ((DEBUG_INFO, " BIOS GUARD Module:: Build Number : 0x%X\n", *(UINT16*)(UINTN)(BiosGuardHobPtr->BiosGuardModulePtr + BIOSGUARD_BUILD_NUMBER_OFFSET)));

  BiosGuardHobPtr->Bgpdt.BgpdtSize = (sizeof (BGPDT) - sizeof (BiosGuardHobPtr->Bgpdt.SfamData) + ((BiosGuardHobPtr->Bgpdt.LastSfam + 1) * sizeof (SFAM_DATA)));
  CopyMem (&BiosGuardHobPtr->Bgpdt.PlatId[0], &PlatIdStr[0], sizeof (PlatIdStr));

  BiosGuardHobPtr->BgupHeader.Version = BGUP_HDR_VERSION;
  CopyMem(&BiosGuardHobPtr->BgupHeader.PlatId[0], &PlatIdStr[0], sizeof (PlatIdStr));
  BiosGuardHobPtr->BgupHeader.PkgAttributes = 0;
  BiosGuardHobPtr->BgupHeader.PslMajorVer = PSL_MAJOR_VERSION;
  BiosGuardHobPtr->BgupHeader.PslMinorVer = PSL_MINOR_VERSION;
  BiosGuardHobPtr->BgupHeader.BiosSvn = BiosGuardHobPtr->Bgpdt.BiosSvn;
  BiosGuardHobPtr->BgupHeader.VendorSpecific = 0x808655AA;

  DEBUG ((DEBUG_INFO, "------------------ BIOS GUARD HOB ------------------\n"));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : Version : 0x%X\n", BiosGuardHobPtr->BgupHeader.Version));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : PlatId[16] :\n"));
  for (Index = 0; Index < 16; Index++) {
      DEBUG ((DEBUG_INFO, " 0x%X", BiosGuardHobPtr->BgupHeader.PlatId[Index]));
  }

  DEBUG ((DEBUG_INFO, " \n"));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : PkgAttributes : 0x%X\n", BiosGuardHobPtr->BgupHeader.PkgAttributes));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : PslMajorVer : 0x%X\n", BiosGuardHobPtr->BgupHeader.PslMajorVer));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : PslMinorVer : 0x%X\n", BiosGuardHobPtr->BgupHeader.PslMinorVer));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : ScriptSectionSize : 0x%X\n", BiosGuardHobPtr->BgupHeader.ScriptSectionSize));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : DataSectionSize : 0x%X\n", BiosGuardHobPtr->BgupHeader.DataSectionSize));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : BiosSvn : 0x%X\n", BiosGuardHobPtr->BgupHeader.BiosSvn));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : EcSvn : 0x%X\n", BiosGuardHobPtr->BgupHeader.EcSvn));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGUP_HEADER : VendorSpecific : 0x%X\n", BiosGuardHobPtr->BgupHeader.VendorSpecific));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : BgpdtSize : 0x%X\n", BiosGuardHobPtr->Bgpdt.BgpdtSize));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : BgpdtMajVer : 0x%X\n", BiosGuardHobPtr->Bgpdt.BgpdtMajVer));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : BgpdtMinVer : 0x%X\n", BiosGuardHobPtr->Bgpdt.BgpdtMinVer));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : PlatId[16] :\n"));
  for (Index = 0; Index < 16; Index++) {
      DEBUG ((DEBUG_INFO, " 0x%X", BiosGuardHobPtr->Bgpdt.PlatId[Index]));
  }

  DEBUG ((DEBUG_INFO, " \n"));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : PkeySlot0[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15) || (Index == 31)) {
      DEBUG ((DEBUG_INFO, " 0x%X\n", BiosGuardHobPtr->Bgpdt.PkeySlot0[Index]));
    } else {
      DEBUG ((DEBUG_INFO, " 0x%X", BiosGuardHobPtr->Bgpdt.PkeySlot0[Index]));
    }
  }

  DEBUG ((DEBUG_INFO, " \n"));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : PkeySlot1[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15) || (Index == 31)) {
      DEBUG ((DEBUG_INFO, " 0x%X\n", BiosGuardHobPtr->Bgpdt.PkeySlot1[Index]));
    } else {
      DEBUG ((DEBUG_INFO, " 0x%X", BiosGuardHobPtr->Bgpdt.PkeySlot1[Index]));
    }
  }

  DEBUG ((DEBUG_INFO, " \n"));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : PkeySlot2[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15) || (Index == 31)) {
      DEBUG ((DEBUG_INFO, " 0x%X\n", BiosGuardHobPtr->Bgpdt.PkeySlot2[Index]));
    } else {
      DEBUG ((DEBUG_INFO, " 0x%X", BiosGuardHobPtr->Bgpdt.PkeySlot2[Index]));
    }
  }

  DEBUG ((DEBUG_INFO, " \n"));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : BgModSvn : 0x%X\n", BiosGuardHobPtr->Bgpdt.BgModSvn));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : BiosSvn : 0x%X\n", BiosGuardHobPtr->Bgpdt.BiosSvn));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : ExecLim : 0x%X\n", BiosGuardHobPtr->Bgpdt.ExecLim));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : BiosGuardAttr : 0x%X\n", BiosGuardHobPtr->Bgpdt.BiosGuardAttr));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : EcCmd : 0x%X\n", BiosGuardHobPtr->Bgpdt.EcCmd));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : EcData : 0x%X\n", BiosGuardHobPtr->Bgpdt.EcData));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : EcCmdGetSvn : 0x%X\n", BiosGuardHobPtr->Bgpdt.EcCmdGetSvn));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : EcCmdOpen : 0x%X\n", BiosGuardHobPtr->Bgpdt.EcCmdOpen));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : EcCmdClose : 0x%X\n", BiosGuardHobPtr->Bgpdt.EcCmdClose));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : EcCmdPortTest : 0x%X\n", BiosGuardHobPtr->Bgpdt.EcCmdPortTest));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : LastSfam : 0x%X\n", BiosGuardHobPtr->Bgpdt.LastSfam));
  DEBUG ((DEBUG_INFO, " BIOS GUARD HOB:: BGPDT : SfamData[64] :\n"));

  //
  // Change the array size according to MAX_SFAM_COUNT
  //
  for (Index = 0; Index <= BiosGuardHobPtr->Bgpdt.LastSfam; Index++) {
    if ((Index == 15) || (Index == 31) || (Index == 47) || (Index == 63) ) {
      DEBUG ((DEBUG_INFO, " 0x%X 0x%X\n", BiosGuardHobPtr->Bgpdt.SfamData[Index].FirstByte,
                                          BiosGuardHobPtr->Bgpdt.SfamData[Index].LastByte));
    } else {
      DEBUG ((DEBUG_INFO, " 0x%X 0x%X,", BiosGuardHobPtr->Bgpdt.SfamData[Index].FirstByte,
                                         BiosGuardHobPtr->Bgpdt.SfamData[Index].LastByte));
    }
  }
  DEBUG ((DEBUG_INFO, " \n"));

  return EFI_SUCCESS;
}
#endif // MINTREE_FLAG

/**
  Platform Init PEI module entry point

  @param[in]  FileHandle           Not used.
  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_SUCCESS          The function completes successfully
  @retval     EFI_OUT_OF_RESOURCES Insufficient resources to create database
**/
EFI_STATUS
EFIAPI
PlatformInitEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  SETUP_DATA                       SystemConfiguration;
  UINTN                            VariableSize;
  UINT8                            FwConfig;
#ifndef MINTREE_FLAG
  SI_PREMEM_POLICY_PPI             *SiPreMemPolicyPpi;
  CPU_SECURITY_PREMEM_CONFIG       *CpuSecurityPreMemConfig;
  EFI_BOOT_MODE                    BootMode;

  PeiServicesGetBootMode (&BootMode);
  Status = PeiServicesLocatePpi (
              &gSiPreMemPolicyPpiGuid,
              0,
              NULL,
              (VOID **) &SiPreMemPolicyPpi
              );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *)SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *)&CpuSecurityPreMemConfig);
  ASSERT_EFI_ERROR(Status);
#endif // MINTREE_FLAG

  Status = InitBoardConfigBlockPostMem ();
  ASSERT_EFI_ERROR (Status);

  ///
  /// Configure GPIO and SIO
  ///
//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG  
  Status = BoardInit ();
  ASSERT_EFI_ERROR (Status);
#endif
//AMI_OVERRIDE_END - Only for CRB EC

#if 0 //AMI_OVERRIDE_START - Take care by AMI module
#ifndef MINTREE_FLAG
  Status = PeiReadWriteVariableInit ();
#endif // MINTREE_FLAG

#endif //AMI_OVERRIDE_END - Take care by AMI module
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,  // GUID
             0,                                 // INSTANCE
             NULL,                              // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices        // PPI
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "Fail to get System Configuration and set the configuration to production mode!\n"));
    FwConfig = 0;
  } else {
    FwConfig = SystemConfiguration.FirmwareConfiguration;
  }

#ifndef MINTREE_FLAG
  Status = PcdValueUpdate ();
  ASSERT_EFI_ERROR (Status);

  if (CpuSecurityPreMemConfig->BiosGuard == TRUE) {
    Status = BiosGuardHobInit (BootMode);
    ASSERT_EFI_ERROR (Status);
  }
#endif // MINTREE_FLAG

  PeiPolicyInit (PeiServices, FwConfig);

  Status = ClearIchSmiAndWake ();
  ASSERT_EFI_ERROR (Status);

//AMI_OVERRIDE_START - Program SSID by AMI parameter
#ifdef INSTALL_INTEL_SSID  
  ProgramMePciSubSystemVidDid ();
#else    
  // Program ME Devices' Subsystem Vendor ID & Subsystem ID
  ProgramMeSubId( (EFI_PEI_SERVICES **)PeiServices, (*PeiServices)->PciCfg );
#endif
//AMI_OVERRIDE_END - Program SSID by AMI parameter

#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - Take care by AMI Flash module
#if 0   
  //
  // Create USB Boot First hotkey information HOB
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    CreateAttemptUsbFirstHotkeyInfoHob ();
  }
#endif
//AMI_OVERRIDE_END - Take care by AMI Flash module

  //
  // Initializing Platform Specific Programming
  //
  Status = PlatformSpecificInit();
  ASSERT_EFI_ERROR (Status);
#endif // MINTREE_FLAG
  //
  // Performing PlatformInitEndOfPei after EndOfPei PPI produced
  //
  Status = PeiServicesNotifyPpi (&mEndOfPeiNotifyList);

  return Status;
}
