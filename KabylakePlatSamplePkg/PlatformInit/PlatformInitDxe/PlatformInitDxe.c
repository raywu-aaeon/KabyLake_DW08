/** @file
  Source code file for the Platform Init DXE module

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
#include "PlatformInitDxe.h"
#include <CpuRegs.h> // AdvancedFeaturesContent
#include <Guid/EventGroup.h>
#ifndef MINTREE_FLAG
#include <Library/BootGuardLib.h>
#include <Protocol/CpuInfo.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/VariableLock.h>
#endif //MINTREE_FLAG
#include <Protocol/S3SaveState.h>
#ifndef MINTREE_FLAG
#include <SoftwareGuardSetupData.h>
#include <MeBiosPayloadHob.h>
#include <Library/PeiPolicyBoardConfigLib/PeiPolicyBoardConfig.h>
#include <Library/CpuPlatformLib.h>
#endif //MINTREE_FLAG
#include <Library/GpioLib.h>
#include <GpioPinsSklLp.h> // AdvancedFeaturesContent
#include <Library/BoardConfigLib.h> // AdvancedFeaturesContent
#include <Library/PchPcrLib.h>
#include <Library/I2cAccessLib.h> // AdvancedFeaturesContent
#include <MipiBridgeConfig.h> // AdvancedFeaturesContent
#include <PlatformBoardId.h>
#ifndef MINTREE_FLAG
#include <Board/BoardHook.h>
#include <Library/EcLib.h>
#include <Library/SmbiosProcessorLib.h>
//AMI_OVERRIDE_START - Use AMI TCG2 module
#if 0  
#include <Protocol/PchReset.h>
#include <Guid/TpmInstance.h>
#include <Library/Tpm2CommandLib.h>
#endif 
//AMI_OVERRIDE_END - Use AMI TCG2 module
#include <Protocol/PciIo.h>
#endif //MINTREE_FLAG

// AMI_OVERRIDE_START - Because we disable the token ALWAYS_PUBLISH_HII_RESOURCES, move BiosIshDataPass from PchSetup.c to PlatformInitDxe.c
#include <Library/DxeMeLib.h>
// AMI_OVERRIDE_END - Because we disable the token ALWAYS_PUBLISH_HII_RESOURCES, move BiosIshDataPass from PchSetup.c to PlatformInitDxe.c

GLOBAL_REMOVE_IF_UNREFERENCED EFI_EVENT                         EndOfDxeEvent;
// AMI_OVERRIDE_START - SGX status can't change from disable to enable after reboot by SGX BIOS info tool
GLOBAL_REMOVE_IF_UNREFERENCED VOID                              *mRegistration;
// AMI_OVERRIDE_END - SGX status can't change from disable to enable after reboot by SGX BIOS info tool
#ifndef MINTREE_FLAG
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA_PROTOCOL      mGlobalNvsArea;

///
/// SGX EPC definitions
///
#define EPC_MULTIPLIER            75
#define EPC_SUBTRACT_MB           SIZE_2MB
#define PRM_BOTTOM_LIMIT          4
#define EPC_32_MB                 32
#define EPC_64_MB                 64
#define EPC_128_MB                128
#define EPC_1024_MB               1024
#define EPC_2048_MB               2048
//AMI_OVERRIDE_START - Because we disable the token ALWAYS_PUBLISH_HII_RESOURCES, move BiosIshDataPass from PchSetup.c to PlatformInitDxe.c
VOID
BiosIshDataPass (
  VOID
  )
{
  EFI_STATUS              Status;    
// AMI_OVERRIDE_START - Fix string end character
  static CONST CHAR8      FileName[] = "bios2ish";
// AMI_OVERRIDE_END - Fix string end character

  Status = HeciPdtUnlockMsg (FileName);
  DEBUG ((DEBUG_INFO, "Sending PDT Unlock Message, Status =%r.\n", Status));
}
//AMI_OVERRIDE_END - Because we disable the token ALWAYS_PUBLISH_HII_RESOURCES, move BiosIshDataPass from PchSetup.c to PlatformInitDxe.c

//AMI_OVERRIDE_START - Use AMI TCG2 module
#if 0
EFI_STATUS
EFIAPI
PlatformPchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  );

GLOBAL_REMOVE_IF_UNREFERENCED PCH_RESET_CALLBACK_PROTOCOL mPchResetCallbackProtocol = { PlatformPchResetCallback };
#endif
//AMI_OVERRIDE_END - Use AMI TCG2 module

/**
  Program RTD3 GPIO and DFU Enable GPIO to enable IVCAM before OS boot. (3D Camera Bell Criff)

  @param[in] PowerGpio - Specify GPIO number to control camera's power.
  @param[in] DFUEnable - Specify GPIO number to control FW update mode.

**/
VOID
UsbIvCamInit(
  IN UINT32 PowerGpio,
  IN UINT32 DFUEnable
  )
{
  SETUP_DATA SetupData;
  UINTN      VariableSize;
  EFI_STATUS Status;

  // Check if Ivycam is disabled. If disabled, don't power on the cam.
  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupData
                  );
  if (!EFI_ERROR(Status)) {
    if (SetupData.CameraType != 1) {
      return;
    }
  }
  //
  // Turn on IVCAM.  To turn on, we need to drive both Power and DFU Enable GPIO to high.
  //

  // Drive DFU Enable to high.
  GpioSetOutputValue (DFUEnable, V_PCH_GPIO_TX_STATE_HIGH);

  // Drive Camera Power to high.
  GpioSetOutputValue (PowerGpio, V_PCH_GPIO_TX_STATE_HIGH);
}
#endif //MINTREE_FLAG

#ifndef MINTREE_FLAG
VOID
SaveWwanRegisters (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  PCI_TYPE00                    PciData;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINTN                         Segment;
  UINTN                         Bus;
  UINTN                         Device;
  UINTN                         Function;
  UINTN                         DeviceBaseAddress;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol;

  DEBUG ((DEBUG_INFO, "SaveWwanRegisters() - Start\n"));

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "SaveWwanRegisters: Error. No Pci IO protocol.\n"));
    HandleCount = 0;
  }
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID *) &PciIo
                    );
    ASSERT_EFI_ERROR(Status);
    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    if (Bus != 0) {
      Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint8,
                    0,
                    sizeof (PciData),
                    &PciData
                    );
      //
      // Find XMM 7360/7560 WWAN card. This is specific to 7360/7560 card. Change this if we need to support other cards.
      //
      if ((PciData.Hdr.VendorId == 0x8086) &&
          ((PciData.Hdr.DeviceId == 0x7360) ||
           (PciData.Hdr.DeviceId == 0x7560))) {
        DEBUG ((DEBUG_INFO, "SaveWwanRegisters: Found XMM 7360/7560. Save key registers.\n"));
        //
        // Save the following registers in ACPI NVS area.
        // Device Control Register   - Offset 78h Word
        // Link Control Register     - Offset 80h Word
        // Device Control 2 Register - Offset 98h Word
        // LTR Max Snoop Latency     - Offset 14Ch Word
        // LTR Max No Snoop Latency  - Offset 14Eh Word
        //
        DeviceBaseAddress = MmPciBase ((UINT32)Bus, (UINT32)0x0, (UINT32)0x0);
        GlobalNvsAreaProtocol->Area->WwanDeviceControlReg = MmioRead16 (DeviceBaseAddress + 0x78);
        DEBUG ((DEBUG_INFO, "SaveWwanRegisters: Device Control Register = %x\n", GlobalNvsAreaProtocol->Area->WwanDeviceControlReg));
        GlobalNvsAreaProtocol->Area->WwanLinkControlReg = MmioRead16 (DeviceBaseAddress + 0x80);
        DEBUG ((DEBUG_INFO, "SaveWwanRegisters: Link Control Register = %x\n", GlobalNvsAreaProtocol->Area->WwanLinkControlReg));
        GlobalNvsAreaProtocol->Area->WwanDeviceControlReg2 = MmioRead16 (DeviceBaseAddress + 0x98);
        DEBUG ((DEBUG_INFO, "SaveWwanRegisters: Device Control Register2 = %x\n", GlobalNvsAreaProtocol->Area->WwanDeviceControlReg2));
        GlobalNvsAreaProtocol->Area->WwanMaxSnoopLatency = MmioRead16 (DeviceBaseAddress + 0x14C);
        DEBUG ((DEBUG_INFO, "SaveWwanRegisters: LTR Max Snoop Latency = %x\n", GlobalNvsAreaProtocol->Area->WwanMaxSnoopLatency));
        GlobalNvsAreaProtocol->Area->WwanMaxNoSnoopLatency = MmioRead16 (DeviceBaseAddress + 0x14E);
        DEBUG ((DEBUG_INFO, "SaveWwanRegisters: LTR Max No Snoop Latency = %x\n", GlobalNvsAreaProtocol->Area->WwanMaxNoSnoopLatency));
      }
    }
  }
  if (HandleBuffer) {
    FreePool (HandleBuffer);
  }
  DEBUG ((DEBUG_INFO, "SaveWwanRegisters() - End\n"));
}
#endif //MINTREE_FLAG

/**
  This function handles PlatformInit task at the end of DXE

  @param[in]  Event     The Event this notify function registered to.
  @param[in]  Context   Pointer to the context data registered to the
                        Event.
**/
VOID
EFIAPI
EndofDxeCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                    Status;
  static BOOLEAN                S3DataSaved = FALSE; // AdvancedFeaturesContent
  UINT16                        ABase;
  UINT32                        SmiEn;
  UINT16                        Pm1Sts;
  PLATFORM_INFO                 *PlatformInfo;
#ifndef MINTREE_FLAG
  UINTN                         PciCfgBase;
//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  UINT64                        OsIndication;
  UINTN                         DataSize;
#endif  
//AMI_OVERRIDE_END - Only for CRB EC
  UINT8                         Value;
  PCH_SERIAL_IO_CONTROLLER      I2CPort;
#endif //MINTREE_FLAG

  gBS->CloseEvent (Event);
#ifndef MINTREE_FLAG
  //
  // @todo: It should be moved to other place.
  //
  CheckLidBaseEcProtocolVersion ();

  if (S3DataSaved) {
    return ;
  }
#endif //MINTREE_FLAG

  PchAcpiBaseGet (&ABase);
  SmiEn = IoRead32 (ABase + R_PCH_SMI_EN);
  //
  // Disabled Legacy USB Logic that generates SMI, during S3 Resume.
  //
  SmiEn &= ~(B_PCH_SMI_EN_LEGACY_USB | B_PCH_SMI_EN_LEGACY_USB2 | B_PCH_SMI_EN_PERIODIC | B_PCH_SMI_EN_SWSMI_TMR);

  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint32,
    (UINTN) (ABase + R_PCH_SMI_EN),
    1,
    &SmiEn
    );
  //
  // Clear bust master status bit on S3 resume
  //
  Pm1Sts = B_PCH_ACPI_PM1_STS_BM;

  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint16,
    (UINTN) (ABase + R_PCH_ACPI_PM1_STS),
    1,
    &Pm1Sts
    );
  //
  // Get Platform Info from Protocol
  //
  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
  if (EFI_ERROR (Status)) {
    return ;
  }

#ifndef MINTREE_FLAG
  // Program 1394 OHCI host controller GUID at offset 0x24 and 0x28 in MMIO
  // This is to resolve yellow bang issue on 1394 net adapter in OS on LosLunas platfom.
  //
  if ((PlatformInfo->PlatformFlavor == FlavorDesktop) ||
      (PlatformInfo->PlatformFlavor == FlavorWorkstation)) {
    Callback1394BootScript ();
  }

  S3DataSaved = TRUE;

  if (PcdGetBool (PcdUsbIvCamSupport)){
    UsbIvCamInit(GPIO_SKL_LP_GPP_B2, GPIO_SKL_LP_GPP_E0);
  }

  if (PcdGetBool (PcdMipiDisplayPresent)) {
    Value = PcdGet8(PcdMipiBridgeI2CPort);
    DEBUG((DEBUG_INFO, "Turn On MIPI Display on I2C%d - Start\n",Value));
    ///
    /// Ensure that the I2C is in in D0 power state
    ///
    I2CPort = PchSerialIoIndexI2C0;
    if (Value == 4) {
      I2CPort = PchSerialIoIndexI2C4;
    }
    DEBUG((DEBUG_INFO, "  Ensure I2C controller is in D0 state\n"));
    PciCfgBase = MmPciBase(0, GetSerialIoDeviceNumber(I2CPort), GetSerialIoFunctionNumber(I2CPort));
    MmioOr32(PciCfgBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    MmioAnd32(PciCfgBase + R_PCH_SERIAL_IO_PME_CTRL_STS, (UINT32)~B_PCH_SERIAL_IO_PME_CTRL_STS_PWR_ST);
    ///
    /// Send the command block (script) to the MIPI bridge
    ///
    Status = I2cBlockWriteRead(I2CPort, GMT_MIPI_BRIDGE_I2C_ADDRESS, (UINT8 *)GmtMipiBridgeI2cScript_DispOn, sizeof(GmtMipiBridgeI2cScript_DispOn));
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_WARN, "Turn On MIPI Display - Failed to turn on MIPI display\n"));
    } else {
      DEBUG ((DEBUG_INFO, "Turn On MIPI Display - End\n"));
    }
  }

//AMI_OVERRIDE_START - Only for CRB EC
#ifdef AMI_CRB_EC_SUPPORT_FLAG
  //
  // Read the home button value to determine if we need to override and boot straight to BIOS setup menu
  //
  Value = 0;
  if (SendEcCommand(0xA) == EFI_SUCCESS) {
    if (ReceiveEcData(&Value) != EFI_SUCCESS) {
      Value = 0;
    }
  }

  if (((Value >> 3) & 0x1) == 0x1) {
    //
    // Read OsIndication register and set EFI_OS_INDICATIONS_BOOT_TO_FW_UI to boot to BIOS setup menu
    //
    OsIndication = 0;
    DataSize = sizeof(UINT64);
    Status = gRT->GetVariable(
      L"OsIndications",
      &gEfiGlobalVariableGuid,
      NULL,
      &DataSize,
      &OsIndication
      );

    OsIndication |= ((UINT64)EFI_OS_INDICATIONS_BOOT_TO_FW_UI);
    Status = gRT->SetVariable(
      L"OsIndications",
      &gEfiGlobalVariableGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
      sizeof(UINT64),
      &OsIndication
      );
    DEBUG((DEBUG_INFO, "Home Button pressed, setting indicator to boot to Setup Menu\n"));
  }

  if (PcdGetBool (PcdPcieWwanEnable)) {
    SaveWwanRegisters();
  }
#endif  
//AMI_OVERRIDE_END - Only for CRB EC
#endif //MINTREE_FLAG

#ifndef MINTREE_FLAG
  ///
  /// Verify if Boot Guard is supported
  ///
  if (IsBootGuardSupported()){
    ///
    /// Identify if Revocation is requested by Boot Guard ACM
    ///
    if (AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & BIT7) {
      BootGuardOemRevocationHook();
    }
  }
#endif //MINTREE_FLAG
//AMI_OVERRIDE_START - Because we disable the token ALWAYS_PUBLISH_HII_RESOURCES, move BiosIshDataPass from PchSetup.c to PlatformInitDxe.c
{
    SETUP_DATA      SetupData;
    UINT32          Attributes = 0;
    UINTN           VariableSize;    
    
    VariableSize = sizeof (SETUP_DATA);
    Status = gRT->GetVariable (
                    L"Setup",
                    &gSetupVariableGuid,
                    &Attributes,
                    &VariableSize,
                    &SetupData
                    );    
    ASSERT_EFI_ERROR (Status);   
    
    // Send PDT Unlock Message to ISH 
    if (SetupData.PchIshPdtUnlock == 1) {
      BiosIshDataPass ();
      //Set the value back to 0 so the user needs to manually enable the option to send the next PDT unlock message
      SetupData.PchIshPdtUnlock = 0;

      Status = gRT->SetVariable (
                      L"Setup",
                      &gSetupVariableGuid,
                      Attributes,
                      sizeof (SETUP_DATA),
                      &SetupData
                      );      
      ASSERT_EFI_ERROR (Status);
    }    
}
//AMI_OVERRIDE_END - Because we disable the token ALWAYS_PUBLISH_HII_RESOURCES, move BiosIshDataPass from PchSetup.c to PlatformInitDxe.c

//AMI_OVERRIDE_START - Fixed ASSERT in MdePkg\Library\BaseLib\LinkedList.c on 109: Count < _gPcd_FixedAtBuild_PcdMaximumLinkedListLength
#if 0  
  gBS->CloseEvent (Event);
#endif
//AMI_OVERRIDE_END - Fixed ASSERT in MdePkg\Library\BaseLib\LinkedList.c on 109: Count < _gPcd_FixedAtBuild_PcdMaximumLinkedListLength  

}

/**
  Registers callback for PlatformInitEndOfDxe.

**/
VOID
RegisterEndOfDxeCallbacks(
  VOID
  )
{
  EFI_STATUS                     Status;

  ///
  /// Performing PlatformInitEndOfDxe after the gEfiEndOfDxeEventGroup is signaled.
  ///
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EndofDxeCallback,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

}

/**
  This function handles PlatformInitDxe task at the ready to boot

  @param[in]  Event     The Event this notify function registered to.
  @param[in]  Context   Pointer to the context data registered to the
                        Event.
**/
VOID
EFIAPI
PlatformDxeReadyToBootCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  UINT32                        Data32;

  gBS->CloseEvent (Event);

  //
  // Clear No Reboot (NR) for TCO 2nd timeout reboot.
  //
  PchPcrRead32 (PID_SMB, R_PCH_PCR_SMBUS_GC, &Data32);
  Data32 &= ~B_PCH_PCR_SMBUS_GC_NR;
  PchPcrWrite32 (PID_SMB, R_PCH_PCR_SMBUS_GC, Data32);

  return;
}

/**
  Registers callback for PlatformDxeReadyToBoot

**/
VOID
RegisterReadyToBootCallback (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_EVENT                      Event;

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             PlatformDxeReadyToBootCallback,
             NULL,
             &Event
             );

  ASSERT_EFI_ERROR (Status);
}

#ifndef MINTREE_FLAG
/**
  Call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
//AMI_OVERRIDE_START - Use AMI TCG2 module 
#if 0
EFI_STATUS
EFIAPI
PlatformPchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  EFI_STATUS                  Status;

  DEBUG ((DEBUG_INFO, "PlatformPchResetCallback\n"));

  Status = Tpm2Shutdown (TPM_SU_CLEAR);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "Tpm2Shutdown (TPM_SU_CLEAR) failed: %r\n", Status));
  }

  return EFI_SUCCESS;
}
#endif 
//AMI_OVERRIDE_END - Use AMI TCG2 module 

/**
  Creates a UEFI variable for OS with all data regarding SGX EPC

  @retval EPC_BIOS_CONFIG   Pointer to BIOS EPC variable structure
**/
EPC_BIOS_CONFIG *
CreateSgxInfoForOs (
  VOID
  )
{
  UINT64             SupportedPrmBins;
  UINT64             PrmSize;
  UINT64             MaxPrmSize;
  UINT64             MaxPlatformPrm;
  UINT8              Index;
  UINT8              BinVal;
  UINT64             C6DramSub;
  MEM_INFO_PROTOCOL  *mMemInfoHob;
  EFI_STATUS         Status;
  EFI_CPUID_REGISTER Cpuid;
  EPC_BIOS_CONFIG    *EpcBios;

  EpcBios = (EPC_BIOS_CONFIG*) AllocateRuntimePool (sizeof (EPC_BIOS_CONFIG));

  if (EpcBios == NULL) {
    DEBUG ((DEBUG_ERROR, "CreateSgxInfoForOs: Memory allocation for EPC structure failed"));
    return NULL;
  }

  ///
  /// Presence of SGX feature is detectable by reading CPUID.(EAX=0x12, ECX=0):EAX[0]
  ///
  AsmCpuidEx (CPUID_SGX_ENABLED, 0, &Cpuid.RegEax, 0, 0, 0);

  ///
  /// If SGX was not activated
  ///
  if (Cpuid.RegEax == 0) {
    DEBUG ((DEBUG_INFO, "CreateSgxInfoForOs: SGX feature is not present, setting AllocatedEpcSize = 0\n"));
    EpcBios->AllocatedEpcSize = 0;
  } else {

    //
    // Read CPUID(12,0x2) to get EPC Base and size
    //
    AsmCpuidEx (CPUID_SGX_ENABLED, 0x2, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);

    //
    // Check if the first sub-leaf is a valid EPC section
    //
    if ((Cpuid.RegEax & 0xF) != 0x1) {
      EpcBios->AllocatedEpcSize = 0;
    } else {

      ///
      /// If SGX was enabled, this variable will hold the EPC length as it was read from CPUID SGX sub-leaf
      /// Else this variable will hold the value 0
      ///
      EpcBios->AllocatedEpcSize = (UINT32) (UINTN) (LShiftU64 ((UINT64) (Cpuid.RegEdx & 0xFFFFF), 32) + (UINT64) (Cpuid.RegEcx & 0xFFFFF000));
    }
  }
  SupportedPrmBins = AsmReadMsr64 (MSR_PRMRR_VALID_CONFIG);

  C6DramSub = 0;

  ///
  /// If C6DRAM is enabled then PRMRR size of 1MB will be supported, reduce 0.5 MB from reported EPC size
  ///
  if ((SupportedPrmBins & BIT0) != 0) {
    C6DramSub = SIZE_512KB;
  }

  EpcBios->SupportedPrmBins = (UINT32) (UINTN) SupportedPrmBins;
  DEBUG ((DEBUG_INFO, "CreateSgxInfoForOs: SupportedPrmBins = 0x%ll016X\n", SupportedPrmBins));

  MaxPrmSize = 0;
  ///
  /// Traverse over all SupportedPrmBins, skip the ones, that are lower or equal to 2 MB
  ///
  for (Index = 0, PrmSize = 1; Index < sizeof(EpcBios->EpcMap) / sizeof(EpcBios->EpcMap[0]); Index++) {

    EpcBios->EpcMap[Index] = 0;

    ///
    /// No More supported PRM size
    ///
    if (SupportedPrmBins == 0) {
      continue;
    }
    BinVal = SupportedPrmBins & BIT0;
    if (Index != 0) {

      ///
      /// Current PRM size in MB
      ///
      PrmSize = LShiftU64(PrmSize,1);
    }
    SupportedPrmBins = RShiftU64(SupportedPrmBins, 1);

    ///
    /// Don't include PRM sizes that are lower than 4MB
    ///
    if (BinVal == 0 || PrmSize < PRM_BOTTOM_LIMIT) {
      continue;
    } else {

      ///
      /// Calculate the supported EPC size, EPC_SIZE = (PRM_SIZE * 0.75) - 2 - 0.5 * IsC6DramEnable()
      ///
      DEBUG ((DEBUG_INFO, "CreateSgxInfoForOs: Supported PRM size = %d MB\n", PrmSize));
      EpcBios->EpcMap[Index] = (UINT32)(DivU64x32(MultU64x32(PrmSize * SIZE_1MB, EPC_MULTIPLIER), 100) - EPC_SUBTRACT_MB - C6DramSub);
      DEBUG ((DEBUG_INFO, "CreateSgxInfoForOs: EpcBios->EpcMap[%d] = 0x%ll016X MB\n", Index, EpcBios->EpcMap[Index]));
      EpcBios->MaxEpcSize = EpcBios->EpcMap[Index];
      MaxPrmSize = PrmSize;
    }
  }

  ///
  /// Reading size of board memory
  ///
  Status = gBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **) &mMemInfoHob);

  ///
  /// If unable to find the memory size, set Max EPC to 32MB
  ///
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "CreateSgxInfoForOs: MemInfoHobProtocol not installed, unable to find system memory size, EPC variable creation failed"));
    FreePool(EpcBios);
    return NULL;
  } else {
    DEBUG ((DEBUG_INFO, "CreateSgxInfoForOs: Size of memory on board = %d MB\n", mMemInfoHob->MemInfoData.memSize));
    /**
     Need to evaluate Max Epc size supported
     ------------------------------------
     |On board memory   |MaxPlatformPrm  |
     ------------------------------------|
     |  1 GB            |32 MB           |
     |  2 GB            |64 MB           |
     |  4 GB or greater |128 MB          |
     ------------------------------------
    **/

    switch(mMemInfoHob->MemInfoData.memSize) {
    case EPC_1024_MB:
      MaxPlatformPrm = EPC_32_MB;
      break;
    case EPC_2048_MB:
      MaxPlatformPrm = EPC_64_MB;
      break;
    default:
      MaxPlatformPrm = EPC_128_MB;
    }
  }
  ///
  /// MaxEpcSize = EPC(min (MaxPlatformPRM, max(PRMSize)))
  ///
  if (MaxPlatformPrm < MaxPrmSize) {
    DEBUG ((DEBUG_INFO, "CreateSgxInfoForOs: Memory on board is smaller than required to support 0x%ll016XMB PRM\n", MaxPrmSize));
    EpcBios->MaxEpcSize = (UINT32) (DivU64x32(MultU64x32(MaxPlatformPrm * SIZE_1MB,EPC_MULTIPLIER), 100) - EPC_SUBTRACT_MB - C6DramSub);
  }
  DEBUG ((DEBUG_INFO, "CreateSgxInfoForOs: Final MaxEpcSize = 0x%ll016X MB\n", EpcBios->MaxEpcSize));
  DEBUG ((DEBUG_INFO, "CreateSgxInfoForOs end\n"));

  return EpcBios;
}

//AMI_OVERRIDE_START - SGX status can't change from disable to enable after reboot by SGX BIOS info tool
/**
  This function will lock Sgx variable.

  @param[in]  Event     - A pointer to the Event that triggered the callback.
  @param[in]  Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
SgxVariableLockCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                    Status;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;

  //Kill the Event
  if(Event != NULL)
	gBS->CloseEvent(Event);

  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, 
								NULL, 
								(VOID **) &VariableLock);
	
  if (VariableLock != NULL) {
	Status = VariableLock->RequestToLock (VariableLock, EPC_BIOS, &gEpcBiosDataGuid);
	if (EFI_ERROR (Status)) {
	  DEBUG ((DEBUG_INFO, "SgxVariableLockCallback:BIOS was unable to lock EpcBios Variable\n"));
	}
	ASSERT_EFI_ERROR (Status);

	Status = VariableLock->RequestToLock (VariableLock, SINIT_SVN_VARIABLE_NAME, &gSinitSvnGuid);
	if (EFI_ERROR (Status)) {
	  DEBUG ((DEBUG_INFO, "SgxVariableLockCallback: BIOS was unable to lock SinitSvn Variable\n"));
	}
	ASSERT_EFI_ERROR (Status);

	Status = VariableLock->RequestToLock (VariableLock, SGX_POLICY, &gSgxPolicyStatusGuid);
	if (EFI_ERROR (Status)) {
	  DEBUG ((DEBUG_INFO, "SgxVariableLockCallback:BIOS was unable to lock SgxPolicyStatus variable.\n"));
	}
	ASSERT_EFI_ERROR (Status);
  }
  DEBUG ((DEBUG_INFO, "SgxVariableLockCallback:Sgx Variable Lock Finish.\n"));
}
//AMI_OVERRIDE_END - SGX status can't change from disable to enable after reboot by SGX BIOS info tool

/**
 Update BIOS and OS EPC UEFI variables
**/
VOID
UpdateEpcVariables (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    EpcBiosStatus;
  EFI_STATUS                    EpcOsStatus;
  UINTN                         EpcBiosConfigurationSize;
  EPC_BIOS_CONFIG               EpcBiosConfiguration;
  UINTN                         EpcOsConfigurationSize;
  EPC_OS_CONFIG                 EpcOsConfiguration;
  CPU_INFO_PROTOCOL             *CpuInfo;
  EPC_BIOS_CONFIG               *EpcBios;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;
  UINTN                         VariableSize;
  UINT32                        VariableAttr;
  CPU_SETUP                     CpuSetup;

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  &VariableAttr,
                  &VariableSize,
                  &CpuSetup
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }

  ///
  /// Reading EPC_BIOS UEFI variable
  ///
  EpcBiosConfigurationSize = sizeof (EPC_BIOS_CONFIG);
  EpcBiosStatus = gRT->GetVariable (
                         EPC_BIOS,
                         &gEpcBiosDataGuid,
                         NULL,
                         &EpcBiosConfigurationSize,
                         &EpcBiosConfiguration
                         );
  ///
  /// Reading EPC_OS_CTRL UEFI variable
  ///
  EpcOsConfigurationSize = sizeof (EPC_OS_CONFIG);
  EpcOsStatus = gRT->GetVariable (
                         EPC_OS_CTRL,
                         &gEpcOsDataGuid,
                         NULL,
                         &EpcOsConfigurationSize,
                         &EpcOsConfiguration
                         );

  if (!EFI_ERROR(EpcBiosStatus)) {
    DEBUG ((DEBUG_INFO, "Old MaxEpcSize from GetVariable = 0x%X\n", EpcBiosConfiguration.MaxEpcSize));
    DEBUG ((DEBUG_INFO, "Old SupportedPrmBins from GetVariable = 0x%X\n", EpcBiosConfiguration.SupportedPrmBins));
    DEBUG ((DEBUG_INFO, "Old EpcSize from GetVariable = 0x%X\n", EpcBiosConfiguration.AllocatedEpcSize));
  }

  if (!EFI_ERROR(EpcOsStatus)) {
    DEBUG ((DEBUG_INFO, "Old RequestedEpcSize from GetVariable = 0x%X\n", EpcOsConfiguration.RequestedEpcSize));
  }

  ///
  /// If SGX is not in 'Software Control' mode, delete EPC_BIOS (if found) & delete EPC_OS_CTRL variable (if found).
  ///
  if (CpuSetup.EnableSgx != SGX_SOFTWARE_CONTROL) {

    if (EpcBiosStatus != EFI_NOT_FOUND) {
      ///
      /// SGX is not in 'Software Control' mode and EPC_BIOS variable is found. Hence, deleting EPC_BIOS variable.
      ///
      DEBUG ((DEBUG_INFO, "SGX is not in 'Software Control' mode and EPC_BIOS variable is found, deleting EPC_BIOS variable \n"));
      ZeroMem (&EpcBiosConfiguration, EpcBiosConfigurationSize);

      Status = gRT->SetVariable (
                    EPC_BIOS,
                    &gEpcBiosDataGuid,
                    0,
                    0,
                    NULL
                    );
    }

    if (EpcOsStatus != EFI_NOT_FOUND) {
      ///
      /// SGX is not in 'Software Control' mode and EPC_OS_CTRL variable is found. Hence, deleting EPC_OS_CTRL variable.
      ///
      DEBUG ((DEBUG_INFO, "SGX is not in 'Software Control' mode and EPC_OS_CTRL variable is found, deleting EPC_OS_CTRL variable \n"));
      ZeroMem (&EpcOsConfiguration, EpcOsConfigurationSize);

      Status = gRT->SetVariable (
                    EPC_OS_CTRL,
                    &gEpcOsDataGuid,
                    0,
                    0,
                    NULL
                    );
    }
    return;
  }

  ///
  /// Get the CPU platform setup policy.
  ///
  Status = gBS->LocateProtocol (&gCpuInfoProtocolGuid, NULL, (VOID **) &CpuInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Located CpuInfo Protocol Status = %r\n", Status));
    return;
  }
  EpcBios = CreateSgxInfoForOs ();

  ///
  /// If BIOS EPC UEFI variable initialization fails
  ///
  if (EpcBios == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO, "Saving UEFI variable EPC_BIOS\n"));
  DEBUG ((DEBUG_INFO, "New MaxEpcSize from HOB = 0x%X\n", EpcBios->MaxEpcSize));
  DEBUG ((DEBUG_INFO, "New SupportedPrmBins from HOB = 0x%X\n", EpcBios->SupportedPrmBins));
  DEBUG ((DEBUG_INFO, "New AllocatedEpcSize from HOB = 0x%X\n", EpcBios->AllocatedEpcSize));

  ///
  /// SGX is in 'Software Control' mode, need to update EPC_BIOS variable
  ///
  DEBUG ((DEBUG_INFO, "SGX is in 'Software Control' mode, updating EPC_BIOS variable \n"));
  Status = gRT->SetVariable (
                  EPC_BIOS,
                  &gEpcBiosDataGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (EPC_BIOS_CONFIG),
                  EpcBios
                  );
  ASSERT_EFI_ERROR(Status);

  ///
  /// Locking EPC_BIOS variable, setting it to be read only
  ///
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (!EFI_ERROR (Status)) {
    Status = VariableLock->RequestToLock (VariableLock, EPC_BIOS, &gEpcBiosDataGuid);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "BIOS was unable to lock EpcBios Variable\n"));
    }
  }

  ///
  /// If SGX was enabled and SGX setup option is set to auto
  ///
  if ((EpcBios->AllocatedEpcSize != 0) && (CpuSetup.EnableSgx == SGX_SOFTWARE_CONTROL)) {

    ///
    /// Updating PRMRR size setup option, to be compatible with current PRMRR size
    ///
    CpuSetup.PrmrrSize = SIZE_32MB;
    if (EpcBios->AllocatedEpcSize > SIZE_32MB) {
      CpuSetup.PrmrrSize = SIZE_64MB;
    }
    if (EpcBios->AllocatedEpcSize > SIZE_64MB) {
      CpuSetup.PrmrrSize = SIZE_128MB;
    }
    DEBUG ((DEBUG_INFO, "PRMRR size in setup options was changed to 0x%ll016X \n", CpuSetup.PrmrrSize));
  }

  Status = gRT->SetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  VariableAttr,
                  sizeof (CPU_SETUP),
                  &CpuSetup
                  );
  ASSERT_EFI_ERROR(Status);
}

VOID
UpdateSgxSvnVariable(
  VOID
  )
{
  EFI_STATUS                    Status;
  CPU_INFO_PROTOCOL             *CpuInfo;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;
  UINT64                        MaskValue;
  UINT64                        Svn;
  MaskValue = BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16;
  ///
  /// Locate CPU Info Protocol.
  ///
  Status = gBS->LocateProtocol (&gCpuInfoProtocolGuid, NULL, (VOID **) &CpuInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Located CpuInfo Protocol Status = %r\n", Status));
    return;
  }

  Svn = CpuInfo->SgxInfo->SgxSinitNvsData;
  Svn &= MaskValue;
  Svn >>= 16;
  DEBUG ((DEBUG_INFO, "SINIT SE SVN:: Read the SgxSinitNvsData value =  0x%X \n", CpuInfo->SgxInfo->SgxSinitNvsData));
  DEBUG ((DEBUG_INFO, "SINIT SE SVN:: Programming NV Variable with the Svn value =  0x%X \n", Svn));
  Status = gRT->SetVariable (
                  SINIT_SVN_VARIABLE_NAME,
                  &gSinitSvnGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof (SINIT_SVN),
                  &Svn
                  );

  ASSERT_EFI_ERROR(Status);

  ///
  /// Locking SINIT_SVN_VARIABLE_NAME variable, setting it to be read only
  ///
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (!EFI_ERROR (Status)) {
    Status = VariableLock->RequestToLock (VariableLock, SINIT_SVN_VARIABLE_NAME, &gSinitSvnGuid);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "BIOS was unable to lock SinitSvn Variable\n"));
    }
  }
}

/**
  Creates a UEFI variable for reporting platform SGX status information
**/
VOID
UpdateSgxPolicyStatusVariable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;
  UINTN                         VariableSize;
  UINT32                        VariableAttr;
  CPU_SETUP                     CpuSetup;
  SOFTWARE_GUARD_STATUS         SgxPolicyStatus;

  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  &VariableAttr,
                  &VariableSize,
                  &CpuSetup
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Update SGX Status variables
  /// Initialize variables if CPU Setup data not found
  ///
  if (EFI_ERROR(Status)){
    DEBUG ((DEBUG_WARN, "CPU Setup data was not found. Initalizing SGX variables.\n"));
    SgxPolicyStatus.SgxStatus = 0;
  } else {
    SgxPolicyStatus.SgxStatus = CpuSetup.EnableSgx;
  }

  ///
  /// Set SGX Status UEFI variable with updated values
  ///
  Status = gRT->SetVariable (
                  SGX_POLICY,
                  &gSgxPolicyStatusGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (SOFTWARE_GUARD_STATUS),
                  &SgxPolicyStatus
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Locking SOFTWARE_GUARD_STATUS variable, setting it to be read only
  ///
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (!EFI_ERROR (Status)) {
    Status = VariableLock->RequestToLock (VariableLock, SGX_POLICY, &gSgxPolicyStatusGuid);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "BIOS was unable to lock SgxPolicyStatus variable.\n"));
    } else {
      DEBUG ((DEBUG_INFO, "BIOS locked SgxPolicyStatus variable.\n"));
    }
  }
//AMI_OVERRIDE_START - SGX status can't change from disable to enable after reboot by SGX BIOS info tool
#if 0
  ASSERT_EFI_ERROR (Status);
#endif
//AMI_OVERRIDE_END - SGX status can't change from disable to enable after reboot by SGX BIOS info tool
}

/**
  Updates Platform information to the setup data

  @param[in] PlatformInfo     Pointer to Platform Info

**/
VOID
UpdatePlatformInfo (
  PLATFORM_INFO                *PlatformInfo
  )
{
  EFI_STATUS              Status;
  SETUP_VOLATILE_DATA     SetupVolatileData;
  UINTN                   VariableSize;
  UINT32                  SetupVolAttributes;
  BOARD_HOOK_PLATFORM_SETUP_OVERRIDE BoardHookPlatformSetupOverride;

  SetMem (&SetupVolatileData, sizeof(SETUP_VOLATILE_DATA), 0);

  VariableSize = sizeof (SETUP_VOLATILE_DATA);
  Status = gRT->GetVariable (
                  L"SetupVolatileData",
                  &gSetupVariableGuid,
                  &SetupVolAttributes,
                  &VariableSize,
                  &SetupVolatileData
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Get VTd status
  //
  SetupVolatileData.VTdAvailable = (MmioRead32(HbPciD0F0RegBase + 0xe4) & BIT23) ? 0 : 1;

  SetupVolatileData.PlatformFlavor = PlatformInfo->PlatformFlavor;
  SetupVolatileData.FabId          = PlatformInfo->BoardRev;
  SetupVolatileData.BoardBomId     = PlatformInfo->BoardBomId;
  SetupVolatileData.PlatformType   = PlatformInfo->PlatformType;
  SetupVolatileData.BoardType      = PlatformInfo->BoardType;


  BoardHookPlatformSetupOverride = (BOARD_HOOK_PLATFORM_SETUP_OVERRIDE) (UINTN) PcdGet64 (PcdFuncBoardHookPlatformSetupOverride);
  if (BoardHookPlatformSetupOverride) {
    Status = BoardHookPlatformSetupOverride ((VOID *) &SetupVolatileData);
  }

  Status = gRT->SetVariable (
                  L"SetupVolatileData",
                  &gSetupVariableGuid,
                  SetupVolAttributes,
                  sizeof (SETUP_VOLATILE_DATA),
                  &SetupVolatileData
                  );
  ASSERT_EFI_ERROR (Status);
}
#endif //MINTREE_FLAG

/**
  Entry point for the driver.

  @param[in]  ImageHandle  Image Handle.
  @param[in]  SystemTable  EFI System Table.

  @retval     EFI_SUCCESS  Function has completed successfully.
  @retval     Others       All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
PlatformInitDxe (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                  Status;
#ifndef MINTREE_FLAG
  PLATFORM_INFO               *PlatformInfo;
  SA_SETUP                    SaSetup;
  UINT16                      ModuleName[] = L"13P2BAD33";  //SKYCAM Device
  ME_BIOS_PAYLOAD_HOB         *MbpHob = NULL;
  SETUP_DATA                  SetupData;
  UINTN                       VariableSize;
  UINT32                      VariableAttr;
//AMI_OVERRIDE_START - Take care by AMI SecureBoot module
#if 0
  UINT8                       *SecureBootState;
#endif 
//AMI_OVERRIDE_END - Take care by AMI SecureBoot module
#endif // MINTREE_FLAG
//AMI_OVERRIDE_START - If VariableLock protocol didn't install, ragister notify to lock the SGX variable
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;	
//AMI_OVERRIDE_END - If VariableLock protocol didn't install, ragister notify to lock the SGX variable

  Status = EFI_SUCCESS;

#ifdef SINITBIN_FLAG
  Status = TxtSinitAcmLoad ();
  if (Status == EFI_SUCCESS) {
    DEBUG((DEBUG_INFO, "TXTDXE: Found SINIT ACM In FV\n"));
  } else {
      DEBUG((DEBUG_INFO, "TXTDXE: Error finding SINIT binary in FVs\n"));
  }
#endif // SINITBIN_FLAG
#ifndef MINTREE_FLAG
  if (IsSgxSupported () && PcdGetBool (PcdSoftwareGuardEnable)) {
    UpdateEpcVariables ();
    UpdateSgxSvnVariable ();
    UpdateSgxPolicyStatusVariable ();
//AMI_OVERRIDE_START - If VariableLock protocol didn't install, ragister notify to lock the SGX variable
    Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
    if (Status == EFI_NOT_FOUND) {
      EFI_EVENT	CallbackEvent;
	  Status = gBS->CreateEvent (
					EVT_NOTIFY_SIGNAL,
					TPL_CALLBACK,
					SgxVariableLockCallback,
					NULL,
					&CallbackEvent
					);
	  ASSERT_EFI_ERROR (Status);

	  Status = gBS->RegisterProtocolNotify (
					&gEdkiiVariableLockProtocolGuid,
					CallbackEvent,
					&mRegistration
					);
	  ASSERT_EFI_ERROR (Status);
	  DEBUG ((DEBUG_INFO, "Register BIOS locked SgxPolicyStatus ProtocolNotify\n"));
    }
//AMI_OVERRIDE_END - If VariableLock protocol didn't install, ragister notify to lock the SGX variable
  }

//AMI_OVERRIDE_START - Take care by AMI SecureBoot module  
#if 0
  SecureBootState = NULL;
  GetVariable2 (L"SecureBootEnable", &gEfiSecureBootEnableDisableGuid, (VOID **) &SecureBootState, NULL);

  //
  // Set CsmControl Setup variable reflecting Secure boot status. Whenever Secure boot is enabled, CSM must be turned off.
  //
  if ((SecureBootState != NULL) && (*SecureBootState)) {
    VariableSize = sizeof (SETUP_DATA);
    Status = gRT->GetVariable (
                    L"Setup",
                    &gSetupVariableGuid,
                    &VariableAttr,
                    &VariableSize,
                    &SetupData
                    );
    ASSERT_EFI_ERROR (Status);

    if (!EFI_ERROR (Status)) {
      SetupData.CsmControl = EFI_CSM_CONTROL_ALWAYS_OFF;
      SetupData.OpRomPost  = OPROM_EFI;
      Status = gRT->SetVariable (
                      L"Setup",
                      &gSetupVariableGuid,
                      VariableAttr,
                      VariableSize,
                      &SetupData
                      );
      ASSERT_EFI_ERROR (Status);
    }
  }

  if (SecureBootState != NULL) {
    FreePool (SecureBootState);
  }
#endif
//AMI_OVERRIDE_END - Take care by AMI SecureBoot module 
#endif // MINTREE_FLAG

#ifdef CSM_FLAG
  if (PcdGetBool (PcdCsmEnable)) {
    InitializeGfxInt15();
  }
#endif // CSM_FLAG

  //
  // Initialize PCH Platform settings
  //
  PchPlatformInitDxe();

  //
  // Initialize System Agent platform settings
  //
  SaPlatformInitDxe();
#ifndef MINTREE_FLAG
  PlatformInfo = NULL;
  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
  if (PlatformInfo == NULL) {
    DEBUG ((DEBUG_ERROR, " Locate PlatformInfoProtocol Status = %r\n", Status));
    return EFI_NOT_FOUND;
  }

  //
  // Need to modify SETUP OPTION values on the fly based on MCP version for SDS systems
  //
  if (PcdGetBool (PcdSdsSetupOptionOverride)) {
    VariableSize = sizeof (SETUP_DATA);
    VariableAttr = 0;
    Status = gRT->GetVariable (
                    L"Setup",
                    &gSetupVariableGuid,
                    &VariableAttr,
                    &VariableSize,
                    &SetupData
                    );
    CopyMem (SetupData.SkyCam_Link0_ModuleName, ModuleName, sizeof(UINT16) * SKYCAM_MODULE_NAME_LENGTH);
    Status = gRT->SetVariable (
                    L"Setup",
                    &gSetupVariableGuid,
                    VariableAttr,
                    VariableSize,
                    &SetupData
                    );

    VariableSize = sizeof (SA_SETUP);
    VariableAttr = 0;
    Status = gRT->GetVariable (
                    L"SaSetup",
                    &gSaSetupVariableGuid,
                    &VariableAttr,
                    &VariableSize,
                    &SaSetup
                    );
    if (!EFI_ERROR (Status)) {
      SaSetup.SlateIndicatorRT = 0;
      SaSetup.DockIndicatorRT  = 0;
      if ((PlatformInfo->DockAttached == 1) && (PlatformInfo->EcPresent == 1)) {
        SaSetup.SlateIndicatorRT = 1;
        SaSetup.DockIndicatorRT  = 1;
      }
      Status = gRT->SetVariable (
                      L"SaSetup",
                      &gSaSetupVariableGuid,
                      VariableAttr,
                      VariableSize,
                      &SaSetup
                      );
    }

    //
    // Get Mbp Hob
    //
    MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
    VariableSize = sizeof (SETUP_DATA);
    VariableAttr = 0;
    Status = gRT->GetVariable (
                      L"Setup",
                      &gSetupVariableGuid,
                      &VariableAttr,
                      &VariableSize,
                      &SetupData
                      );
    if (((MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType != IntelMeCorporateFw) && (SetupData.PchFpsEnable == 2)) ||
        (SetupData.PchFpsEnable == 0)) {
      SetupData.PchSpi1SensorDevice = 0;
      Status = gRT->SetVariable (
                      L"Setup",
                      &gSetupVariableGuid,
                      VariableAttr,
                      VariableSize,
                      &SetupData
                      );
    }
  }

  //
  // Update Platform information to the setup data
  //
  UpdatePlatformInfo(PlatformInfo);
#endif //MINTREE_FLAG

#ifdef CSM_FLAG
  //
  // Update Video related informations in the CMOS reigister
  //
  if (PcdGetBool (PcdCsmEnable)) {
    UpdateCmosDisplayRegisters(PlatformInfo);
  }
#endif //CSM_FLAG
#ifndef MINTREE_FLAG
  //
  // Update DIMM funtionalities for desktop and server boards
  //
  UpdateDimmPopulation(PlatformInfo);
#endif //MINTREE_FLAG

  RegisterEndOfDxeCallbacks();
  RegisterReadyToBootCallback ();
#ifndef MINTREE_FLAG
  //
  // Add Smbios type 4 and type 7 table.
  //
  AddSmbiosProcessorAndCacheTables ();

//AMI_OVERRIDE_START - Use AMI TCG2 module
#if 0
  //
  // Check if TPM2.0 exist.
  //
  if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceNoneGuid)) {
  } else if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)) {
  } else {
    //
    // Register Pch reset callback to shutdown TPM
    //
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gPchResetCallbackProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mPchResetCallbackProtocol
                    );
    ASSERT_EFI_ERROR (Status);
  }
#endif 
//AMI_OVERRIDE_END - Use AMI TCG2 module
#endif // MINTREE_FLAG
  return Status;
}

#ifdef SINITBIN_FLAG
/**

  This function looks for SINIT ACM in FVs and updates TXT HOB
  with SINIT ACM Base and Size.

  @retval EFI_SUCCESS     - SINIT ACM found and copied.
  @retval EFI_NOT_FOUND   - If TxtInfoHob is not found
  @retval EFI_UNSUPPORTED - TXT Device memory not available.

**/
EFI_STATUS
TxtSinitAcmLoad (
  VOID
  )
{
  EFI_STATUS                    Status;
  TXT_INFO_HOB                  *HobList;
  TXT_INFO_DATA                 *TxtInfoData;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  UINTN                         Size;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT64                        TxtHeapMemoryBase;
  UINT64                        TxtSinitMemoryBase;
  UINT64                        *Ptr64;
  EFI_PHYSICAL_ADDRESS          TopAddr;
  BIOS_OS_DATA_REGION           *BiosOsDataRegion;

  HandleBuffer = NULL;
  NumberOfHandles = 0;
  FwVol = NULL;
  FileType = 0;
  Attributes = 0;
  BiosOsDataRegion = NULL;

  ///
  /// Find TXT HOB
  ///
  HobList = (TXT_INFO_HOB *) GetFirstGuidHob (&gTxtInfoHobGuid);
  if (HobList == NULL) {
    return EFI_NOT_FOUND;
  }

  TxtInfoData = &HobList->Data;
  ///
  /// Check TXT mode
  ///
  if (TxtInfoData->TxtMode == 0){
    return EFI_UNSUPPORTED;
  }

  if ((TxtInfoData == 0) ||
      (TxtInfoData->TxtDprMemoryBase == 0) ||
      (TxtInfoData->TxtDprMemorySize == 0) ||
      (TxtInfoData->TxtHeapMemorySize == 0) ||
      (TxtInfoData->SinitMemorySize == 0)
      ) {
    return EFI_UNSUPPORTED;
  } else {
    ///
    /// Use address passed from PEI
    ///
    TopAddr             = TxtInfoData->TxtDprMemoryBase + TxtInfoData->TxtDprMemorySize;

    TxtHeapMemoryBase   = (UINT64) (TopAddr - TxtInfoData->TxtHeapMemorySize);

    TxtSinitMemoryBase  = TxtHeapMemoryBase - TxtInfoData->SinitMemorySize;
  }
  ///
  /// Start looking for SINIT ACM in FVs
  ///
  DEBUG((DEBUG_INFO, "TXTDXE: Looking for SINIT ACM in FVs\n"));

  Status = gBS->LocateHandleBuffer(
    ByProtocol,
    &gEfiFirmwareVolume2ProtocolGuid,
    NULL,
    &NumberOfHandles,
    &HandleBuffer
    );
  ASSERT_EFI_ERROR(Status);
  ///
  /// Looking for FV with SinitAC binary
  ///
  for (UINTN Index = 0; Index < NumberOfHandles; Index++) {
    ///
    /// Get the protocol on this handle
    ///
    Status = gBS->HandleProtocol(
      HandleBuffer[Index],
      &gEfiFirmwareVolume2ProtocolGuid,
      &FwVol
      );
    ASSERT_EFI_ERROR(Status);
    ///
    /// See if it has the SinitACM file
    ///
    Size = 0;
    FvStatus = 0;
    Status = FwVol->ReadFile(
      FwVol,
      &gSinitModuleGuid,
      NULL,
      &Size,
      &FileType,
      &Attributes,
      &FvStatus
      );
    ///
    /// If the binary was located, then break
    ///
    if (Status == EFI_SUCCESS) {
      DEBUG((DEBUG_INFO, "TXTDXE: Found SINIT ACM In FV\n"));
      break;
    }
  }
  FreePool(HandleBuffer);
  ///
  /// Sanity check
  ///
  if ( !(FwVol == NULL || Status != EFI_SUCCESS || Size >= TxtInfoData->SinitMemorySize ) ) {
    ///
    /// Read Sinit ACM from the storage file.
    ///
    Ptr64 = (UINT64 *)TxtSinitMemoryBase;
    Status = FwVol->ReadFile(
      FwVol,
      &gSinitModuleGuid,
      &Ptr64,
      &Size,
      &FileType,
      &Attributes,
      &FvStatus
      );
    ASSERT_EFI_ERROR(Status);

    TxtInfoData->SinitAcmSize = (UINT64) Size;
    Ptr64 = (UINT64 *) TxtHeapMemoryBase;
    ///
    /// BiosOsDataSize plus size of data size field itself
    ///
    BiosOsDataRegion                = (BIOS_OS_DATA_REGION *) (Ptr64 + 1);
    BiosOsDataRegion->BiosSinitSize = (UINT32) TxtInfoData->SinitAcmSize;
    DEBUG((DEBUG_INFO, "TXTDXE: Copy SINIT ACM Done\n"));
  }
  return Status;
}
#endif //SINITBIN_FLAG
