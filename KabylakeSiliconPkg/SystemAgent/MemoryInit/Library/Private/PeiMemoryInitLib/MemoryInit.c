/** @file
  Memory Initialization PEIM.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Guid/SmramMemoryReserve.h>
#include <Guid/MemoryTypeInformation.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Guid/AcpiS3Context.h>
#include <Library/PeiSaPolicyLib.h>
#include <Private/Library/CpuCommonLib.h>
#include <Source/Include/MrcMemoryMap.h>
//
// Definition in EDK Foundation, used in this driver
//
#define BOOT_IN_RECOVERY_MODE_MASK                    0x40
#define BOOT_SPECIAL_MASK                             0x80

#include <Library/MmPciLib.h>
#include <Register/Cpuid.h>
#include <Register/Msr.h>
#include <CpuAccess.h>
#include <Library/CpuPlatformLib.h>
#include <SaPolicyCommon.h>
#include "MemoryInit.h"
#include "MrcGlobal.h"
#include "MrcDebugHook.h"
#include "MrcMalloc.h"
#include <Library/PcdLib.h>
#include <Library/PerformanceLib.h>
#include <Library/TxtLib.h>
#include <PttPtpRegs.h>
#include <MemoryTest.h>

#include <IndustryStandard/SmBios.h>

//
// Driver Dependent PPI Prototypes
//
#include <Ppi/Capsule.h>
#include <TxtInfoHob.h>
#include <Ppi/Wdt.h>
#include <Ppi/MasterBootMode.h>

#ifdef FSP_FLAG
#include <FspEas.h>
#include <Library/FspCommonLib.h>
#include <Library/FspSwitchStackLib.h>
#include <FspmUpd.h>
#endif // FSP_FLAG

//
// Private Library Definitions
//
#include <Private/Library/TxtPeiLib.h>
#include <Private/Library/MeInitLib.h>


#ifdef FSP_FLAG
//
// Support 64 K IO space
// Moving RES_IO_BASE due to new ACPI Base address 0x1800
//
#define RES_IO_BASE   0x2000
#define RES_IO_LIMIT  0xFFFF
//
// Support 4G address space
//
#define RES_MEM_LIMIT_1 ((UINTN) MmPciBase (0,0,0) - 1)
#endif // FSP_FLAG

// AMI_OVERRIDE_SA0008_START >>>
#include <Ppi/NbPpi.h>
// AMI_OVERRIDE_SA0008_END <<<
//
// Driver GUID Definitions
//
extern EFI_GUID  gEfiSmmPeiSmramMemoryReserveGuid;

#ifndef MDEPKG_NDEBUG
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringFc[]  = "WITH_FULL_CONFIGURATION";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringMc[]  = "WITH_MINIMAL_CONFIGURATION";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringNc[]  = "ASSUMING_NO_CONFIGURATION_CHANGES";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringFcd[] = "WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringDs[]  = "WITH_DEFAULT_SETTINGS";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringS4[]  = "ON_S4_RESUME";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringS5[]  = "ON_S5_RESUME";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringS2[]  = "ON_S2_RESUME";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringS3[]  = "ON_S3_RESUME";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringFu[]  = "ON_FLASH_UPDATE";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringRm[]  = "IN_RECOVERY_MODE";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringRmm[] = "IN_RECOVERY_MODE_MASK";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringSm[]  = "SPECIAL_MASK";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 BootStringUnk[] = "MODE_UNKNOWN";
#endif

// AMI_OVERRIDE_SA0008_START >>>
static EFI_PEI_PPI_DESCRIPTOR mAmiPeiBeforeMrcDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gAmiPeiBeforeMrcGuid, \
      NULL }
};

static EFI_PEI_PPI_DESCRIPTOR mAmiPeiCompelteMrcDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gAmiPeiAfterMrcGuid, \
      NULL }
};

static EFI_PEI_PPI_DESCRIPTOR mAmiPeiEndOfMrcDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gAmiPeiEndOfMemDetectGuid, \
      NULL }
};
// AMI_OVERRIDE_SA0008_END <<<

///
/// The following table drives the SPD fields that are read from the DIMM.
///
GLOBAL_REMOVE_IF_UNREFERENCED const SPD_OFFSET_TABLE SpdDdr3Table[] = {
  {   0,               1,             (1 << SpdCold),},
  {   2,               2,             (1 << SpdCold) | (1 << SpdFast),},
  {   3,              41,             (1 << SpdCold),},
  {  60,              63,             (1 << SpdCold),},
  { SPD3_MANUF_START, SPD3_MANUF_END, (1 << SpdCold) | (1 << SpdFast),},
  { 128,             145,             (1 << SpdCold),},
#ifdef SUPPORT_SPD_CRC
  {  39,              59,             (1 << SpdCold),},
  {  64,             125,             (1 << SpdCold),},
#endif
  { 176,             179,             (1 << SpdCold),},
  { 180,             184,             (1 << SpdCold),},
  { 185,             215,             (1 << SpdCold),},
  { 220,             250,             (1 << SpdCold),},
};
GLOBAL_REMOVE_IF_UNREFERENCED const SPD_OFFSET_TABLE SpdDdr4Table[] = {
  {   0,               1,             (1 << SpdCold),},
  {   2,               2,             (1 << SpdCold) | (1 << SpdFast),},
  {   3,              40,             (1 << SpdCold),},
  { 117,             131,             (1 << SpdCold),},
  { SPD4_MANUF_START, SPD4_MANUF_END, (1 << SpdCold) | (1 << SpdFast),},
  { 329,             348,             (1 << SpdCold),},
#ifdef SUPPORT_SPD_CRC
  {  32,             119,             (1 << SpdCold),},
  { 126,             255,             (1 << SpdCold),},
  { 349,             383,             (1 << SpdCold),},
#endif
  { 384,             387,             (1 << SpdCold),},
  { 388,             389,             (1 << SpdCold),},
  { 393,             431,             (1 << SpdCold),},
  { 440,             478,             (1 << SpdCold),},
};
GLOBAL_REMOVE_IF_UNREFERENCED const SPD_OFFSET_TABLE SpdLpddrTable[] = {
  {   0,               1,               (1 << SpdCold),},
  {   2,               2,               (1 << SpdCold) | (1 << SpdFast),},
  {   3,              32,               (1 << SpdCold),},
  { 120,             130,               (1 << SpdCold),},
  { SPDLP_MANUF_START, SPDLP_MANUF_END, (1 << SpdCold) | (1 << SpdFast),},
  { 329,             348,               (1 << SpdCold),},
#ifdef SUPPORT_SPD_CRC
  {  31,             121,               (1 << SpdCold),},
  { 126,             255,               (1 << SpdCold),},
  { 349,             383,               (1 << SpdCold),},
#endif
  { 384,             387,               (1 << SpdCold),},
  { 388,             389,               (1 << SpdCold),},
  { 393,             431,               (1 << SpdCold),},
  { 440,             478,               (1 << SpdCold),},
};
extern MrcDimmSts DimmPresence (IN MrcDebug *const Debug, IN const MrcSpd *const Spd);

GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 MrcDataStringConst[] = "MRCD";
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 MrcSpdStringConst[]  = "SPD ";

// AMI_OVERRIDE_SA0026_START >>>
#ifdef NB_SMBIOS_TYPE_17_CUSTOMIZATION_FLAG
#define NB_SMBIOS_TYPE17_EXTRA_INFO_DATA_GUID \
  { 0x46006e87, 0x0330, 0x44a3, 0xb8, 0x2b, 0xeb, 0xf9, 0x91, 0x9, 0x47, 0x2e }

static EFI_GUID gNbSmbiosType17ExtraInfoDataGuid   = NB_SMBIOS_TYPE17_EXTRA_INFO_DATA_GUID;

typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  NB_TYPE17_EXTRA_DATA NbSpdData[MAX_CONTROLLERS * MAX_CHANNEL * MAX_DIMMS_IN_CHANNEL];
} NB_SMBIOS_TYPE_17_DATA_HOB;

VOID
BuildNbSmbiosType17ExtraInfoDataHob (
  IN MrcParameters            *MrcData
  )
{
  EFI_STATUS        Status;
  MrcSaveData       *SaveData;
  MrcContSave       *ControllerSave;
  MrcChannelSave    *ChannelSave;
  MrcDimmSave       *DimmSave;

  NB_SMBIOS_TYPE_17_DATA_HOB *T17_DataHob;
  
  UINT8                Controller;
  UINT8                Channel;
  UINT8                Dimm;

  DEBUG ((DEBUG_INFO, "BuildNbSmbiosType17ExtraInfoDataHob - Start\n"));

  SaveData = &MrcData->Save.Data;
  
  //
  // Build HOB for T17_DataHob
  //
  Status = MrcGetHobForDataStorage (
             (VOID **) &T17_DataHob,
             sizeof (NB_SMBIOS_TYPE_17_DATA_HOB),
             &gNbSmbiosType17ExtraInfoDataGuid
           );
  ASSERT_EFI_ERROR (Status);

  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerSave               = &SaveData->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelSave                   = &ControllerSave->Channel[Channel];
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        DimmSave = &ChannelSave->DimmSave[Dimm];
        T17_DataHob->NbSpdData[(Controller << 2) + (Channel << 1) + Dimm].tCKmin = DimmSave->NbSpdData.tCKmin;
        T17_DataHob->NbSpdData[(Controller << 2) + (Channel << 1) + Dimm].VDD = DimmSave->NbSpdData.VDD;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "BuildNbSmbiosType17ExtraInfoDataHob - End\n"));

}
#endif
// AMI_OVERRIDE_SA0026_END <<<

/**
  Entry point of MemoryInit library
  This function will invoke main memory detection code after the following dependencies
  have been statisified at right timing.
  - gSiPreMemPolicyPpiGuid
  - gSiPolicyPpiGuid
  - gWdtPpiGuid
  - gSaPeiInitPpiGuid
  - gEfiPeiMasterBootModePpiGuid
  Note:\n
  If TXT is not implemented(_SA_MISC_PEI_PREMEM_CONFIG->TxtImplemented == 0), gEfiPeiMasterBootModePpiGuid will be the last dependency of the notify PPI registration to perform MemoryInit.\n
  If TXT is implemented(_SA_MISC_PEI_PREMEM_CONFIG->TxtImplemented == 1), gPeiTxtReadyToRunMemoryInitPpiGuid will be the PPI that invokes MemoryInit. (gEfiPeiMasterBootModePpiGuid should be installed earlier than gPeiTxtReadyToRunMemoryInitPpiGuid)\n

  @retval EFI_SUCCESS          - MRC callback has been registered successfully
  @retval EFI_OUT_OF_RESOURCES - Fail to allocate required buffer for MRC callback control.
**/
EFI_STATUS
EFIAPI
InstallMrcCallback (
  VOID
  )
{
  EFI_STATUS                    Status;
  MRC_INSTANCE                  *MrcInstance;
  SI_PREMEM_POLICY_PPI          *SiPreMemPolicyPpi;
  SA_MISC_PEI_PREMEM_CONFIG     *MiscPeiPreMemConfig;

  DEBUG ((DEBUG_INFO, "[InstallMrcCallback] MRC Entry...\n"));
  //
  // Obtain policy settings.
  //
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *) &MiscPeiPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize MRC Instance for callbacks
  //
  MrcInstance = AllocateZeroPool (sizeof (MRC_INSTANCE));
  ASSERT (MrcInstance != NULL);
  if (MrcInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  MrcInstance->Signature                = MRC_INSTANCE_SIGNATURE;
  MrcInstance->NotifyDescriptor.Flags   = (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  MrcInstance->NotifyDescriptor.Notify  = PrememoryInitCallBack;
  MrcInstance->FileHandle               = NULL;
  MrcInstance->MrcExecuted              = FALSE;
  if (MiscPeiPreMemConfig->TxtImplemented == 1) {
    MrcInstance->NotifyDescriptor.Guid    = &gPeiTxtReadyToRunMemoryInitPpiGuid;
    DEBUG ((DEBUG_INFO, "[InstallMrcCallback] Register gPeiTxtReadyToRunMemoryInitPpiGuid Notify Status = %r\n", Status));
  } else {
    MrcInstance->NotifyDescriptor.Guid    = &gEfiPeiMasterBootModePpiGuid;
    DEBUG ((DEBUG_INFO, "[InstallMrcCallback] Register PeiMasterBootModePpi Notify Status = %r\n", Status));
  }
  Status = PeiServicesNotifyPpi (&MrcInstance->NotifyDescriptor);
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  A notify callback function to determine if main memory detection function can be executed or not

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The notify callback PPI.  Not used.

  @retval EFI_SUCCESS          - MRC has been executed successfully
  @retval EFI_NOT_READY        - One of the dependency not ready yet for MRC execution.
**/
EFI_STATUS
EFIAPI
PrememoryInitCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                    Status;
  MRC_INSTANCE                  *MrcInstance;
#ifdef FSP_FLAG
  volatile BOOLEAN  LoopUntilReset;

  LoopUntilReset = TRUE;
#endif

  DEBUG ((DEBUG_INFO, "[PrememoryInitCallBack]\n"));
  MrcInstance = MRC_INSTANCE_FROM_NOTIFY_THIS (NotifyDescriptor);

  //
  // If MRC has been executed, rest of callbacks should return directly.
  //
  if (MrcInstance->MrcExecuted == TRUE) {
    return EFI_SUCCESS;
  }

  Status = PeiServicesLocatePpi (
             &gEfiPeiMasterBootModePpiGuid,
             0,
             NULL,
             NULL
             );
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "[PrememoryInitCallBack] PeiMasterBootModePpi is not installed yet.\n"));
    return EFI_NOT_READY;
  }

  //
  // Here we will execute MemoryInit and set MrcExecuted to TRUE
  //
  MrcInstance->MrcExecuted = TRUE;
  Status = PeimMemoryInit (MrcInstance->FileHandle, (CONST EFI_PEI_SERVICES **) PeiServices, MrcInstance);
#ifdef FSP_FLAG
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "FspMemoryInitApi() - Error Initialization Failure - 0x%x\n", Status));
    //
    // Return after converting to API return status as per FSP EAS
    //
    if ((Status != EFI_INVALID_PARAMETER) && (Status != EFI_UNSUPPORTED) &&
        (Status != EFI_DEVICE_ERROR) && (Status != EFI_OUT_OF_RESOURCES)) {
      Status = EFI_DEVICE_ERROR;  // Remap to API return status as per FSP EAS
    }
    ///
    /// Below code is not an infinite loop.The control will go back to API calling function in BootLoader each time BootLoader
    /// calls the FSP API as FSP can't proceed after a Memory Initialization Failure.
    ///
    do {
      SetFspApiReturnStatus (Status);
      Pei2LoaderSwitchStack ();
      DEBUG ((DEBUG_ERROR, "!!!ERROR: FspMemoryInitApi() - Initialization Failure - 0x%x\n", Status));
    } while (LoopUntilReset);
  }
#endif
  return Status;
}

/**
  Main starting point for system memory initialization.
    - 1. Get SysBootMode and MrcBootMode
    - 2. Locate SiPreMemPolicyPpi
    - 3. Locate S3DataPtr from MiscPeiPreMemConfig.
    - 4. SaveDataValid := TRUE if S3DataPtr is not NULL.
    - 5. If SysBootMode is BOOT_ON_S3_RESUME and S3Data is not valid:
      -> ASSERT.
    - 6. If MrcBootMode is Warm boot, but S3 data is not valid :
        -> change MrcBootMode to Cold boot.
    - 7. If MrcBootMode is Cold boot:
        -> Run MRC code
        -> Save S3 Restore Data
      Else
        -> Run MRC_S3Resume
    - 8. Run MRC_Done().
    - 9. Install EFI memory HOBs.

  @param[in] FfsHeader (or FileHandle) - Pointer to Firmware File System file header
  @param[in] PeiServices               - General purpose services available to every PEIM.
  @param[in] MrcInstance               - MRC instance structure for callback implementation

  @retval EFI_SUCCESS      - Memory initialization completed successfully.
  @retval EFI_NOT_READY    - Cannot locate SA Policy.
  @retval EFI_NOT_FOUND    - No S3 data in S3 Boot Mode.
  @retval EFI_DEVICE_ERROR - MemoryInit failed or IOSAV Memory test failed.
**/
EFI_STATUS
PeimMemoryInit (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  MRC_INSTANCE                 *MrcInstance
  )
{
  SI_PREMEM_POLICY_PPI        *SiPreMemPolicyPpi;
  MEMORY_CONFIGURATION        *MemConfig;
  SA_MISC_PEI_PREMEM_CONFIG   *MiscPeiPreMemConfig;
  GRAPHICS_PEI_PREMEM_CONFIG  *GtConfig;
  MEMORY_CONFIG_NO_CRC        *MemConfigNoCrc;
  MEMORY_PLATFORM_DATA_HOB    *Hob;
  MrcParameters               *MrcData;
  MrcInput                    *Inputs;
  MrcOutput                   *Outputs;
  MrcSave                     *SaveSys;
  MrcSave                     *Save;
  MRC_FUNCTION                CallTable;
  MRC_FUNCTION                *MrcCall;
  MrcIntOutput                *MrcIntData;
  EFI_STATUS                  Status;
  EFI_BOOT_MODE               SysBootMode;
  MrcStatus                   MrcStatus;
  MrcBootMode                 MrcBootMode;
  BOOLEAN                     SaveDataValid;
  UINT32                      Crc32;
  MrcDebug                    *Debug;
#ifndef MDEPKG_NDEBUG
  const UINT8                 *Str;
#endif
  UINT32                      MemoryClock;
  MrcClockRatio               Ratio;
  UINT8                       InitStat;
  UINT8                       ForceFullTraining;
  UINT8                       OrigMrcBootMode;
  UINT8                       TotalDprSizeMB;
  UINT16                      HobTotalSize;
  UINT16                      HobBufferSize;
  MrcIntOutput                MrcIntGlobalData;
  MrcParameters               MrcGlobalData;

  DEBUG ((DEBUG_INFO, "[PeimMemoryInit]\n"));
  ZeroMem (&MrcGlobalData, sizeof (MrcParameters));
  ZeroMem (&MrcIntGlobalData, sizeof (MrcIntOutput));
  MrcData                                  = &MrcGlobalData;
  Inputs                                   = &MrcData->Inputs;
  Inputs->Call.Func                        = &CallTable;
  MrcCall                                  = Inputs->Call.Func;
  Outputs                                  = &MrcData->Outputs;
  MrcIntData                               = &MrcIntGlobalData;
  MrcData->IntOutputs.Internal             = (void *) MrcIntData;
  MrcIntData->Size                         = sizeof (MrcIntOutput);
  MrcData->MrcDataSize                     = sizeof (MrcParameters);
  MrcData->Save.Size                       = sizeof (MrcSave);
  Inputs->Header.GuidHob.Header.HobLength  = sizeof (MrcInput);
  Outputs->Size                            = sizeof (MrcOutput);
  *((UINT32 *) &MrcData->MrcDataString[0]) = *((UINT32 *) MrcDataStringConst);
  MrcIntData->PeiServices                  = (UINT32) PeiServices;
  HobBufferSize                            = 0;
// AMI_OVERRIDE_SA0008_START >>>
  Status = (*PeiServices)->InstallPpi(PeiServices, &mAmiPeiBeforeMrcDesc[0]);
  ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_SA0008_END <<<  

  //
  // Obtain policy settings.
  //
  SiPreMemPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);
  if (SiPreMemPolicyPpi == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MemConfig = NULL;
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMemoryConfigGuid, (VOID *) &MemConfig);
  ASSERT_EFI_ERROR (Status);
  if (MemConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MiscPeiPreMemConfig = NULL;
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *) &MiscPeiPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  if (MiscPeiPreMemConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MemConfigNoCrc = NULL;
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMemoryConfigNoCrcGuid, (VOID *) &MemConfigNoCrc);
  ASSERT_EFI_ERROR (Status);
  if (MemConfigNoCrc == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GtConfig = NULL;
  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gGraphicsPeiPreMemConfigGuid, (VOID *) &GtConfig);
  ASSERT_EFI_ERROR (Status);
  if (GtConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SiPreMemPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (&gSiPreMemPolicyPpiGuid, 0, NULL, (VOID **) &SiPreMemPolicyPpi);
  ASSERT_EFI_ERROR (Status);
  if (SiPreMemPolicyPpi == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MrcSetupOem (MrcData, MemConfig, MemConfigNoCrc);

  //
  // Obtain boot mode.
  //
  Status = PeiServicesGetBootMode (&SysBootMode);
  ASSERT_EFI_ERROR (Status);

  if (SysBootMode != BOOT_ON_S3_RESUME) {
    //
    // Calculate total HOB size needed for MRC
    //
    switch (Inputs->HobBufferSize) {
      case 1:
        HobBufferSize = 1;
        break;

      case 2:
        HobBufferSize = 1 * 1024;
        break;

      case 3:
        HobBufferSize = (63 * 1024) - sizeof (EFI_HOB_GUID_TYPE) - sizeof (MEMORY_PLATFORM_DATA);
        break;

      case 0:
      default:
#ifdef SSA_FLAG
        HobBufferSize = (63 * 1024) - sizeof (EFI_HOB_GUID_TYPE) - sizeof (MEMORY_PLATFORM_DATA);
#else
  #ifdef MRC_DEBUG_PRINT
        HobBufferSize = 1 * 1024;
  #else
        HobBufferSize = 1;
  #endif
#endif
        break;
    }
    HobTotalSize = sizeof (EFI_HOB_GUID_TYPE) + sizeof (MEMORY_PLATFORM_DATA) + HobBufferSize;
    Status = MrcGetHobForDataStorage ((VOID **) &Hob, HobTotalSize, &gSiMemoryPlatformDataGuid);
    ASSERT_EFI_ERROR (Status);
    MrcHeapInitialize (MrcData, (void *) &Hob->Data, HobTotalSize - sizeof (EFI_HOB_GUID_TYPE));
    DEBUG ((DEBUG_INFO, "MRC heap. Base = %Xh, Size = %d\n", Inputs->HeapBase, Inputs->HeapSize));
#ifdef BDAT_SUPPORT
    Status = MrcGetHobForDataStorage ((VOID **) &Outputs->BdatMemoryHob[0], sizeof (BDAT_MEMORY_DATA_HOB), &gEfiMemorySchemaGuid);
    if (EFI_SUCCESS != Status) {
      Outputs->BdatMemoryHob[0].Pointer = NULL;
    }
    Status = MrcGetBdatSchemaListHob (&Outputs->BdatSchemasHob.Pointer);
    if (EFI_ERROR (Status)) {
      Outputs->BdatSchemasHob.Pointer = NULL;
    }
#endif
  } else {
    Hob = NULL;
#ifdef BDAT_SUPPORT
    Outputs->BdatMemoryHob[0].Pointer = NULL;
#endif
  }

  Debug = &Outputs->Debug;

#ifdef SSA_FLAG
  Status = PeiServicesLocatePpi (
             &gSsaBiosCallBacksPpiGuid,
             0,
             NULL,
             (VOID **) &Inputs->SsaCallbackPpi
             );
  if (EFI_SUCCESS != Status) {
    Inputs->SsaCallbackPpi = 0;
  }
  Debug->Stream       = (UINT32) PeiServices;
  DEBUG ((DEBUG_INFO, "SsaCallbackPpi = %Xh\n", Inputs->SsaCallbackPpi));
#endif // SSA_FLAG
  Inputs->DebugLevel  = (MemConfig->MrcTimeMeasure > 0) ? MSG_LEVEL_TIME : MemConfigNoCrc->SerialDebugLevel;
  Inputs->DebugStream = (UINT32) PeiServices;
#ifdef MRC_DEBUG_PRINT
#ifdef SSA_FLAG
  if (SysBootMode != BOOT_ON_S3_RESUME) {
    MrcSerialBufferMalloc (MrcData); // @todo - heap/stack collision issue with Power Training code
  }
#endif
#endif
  //
  // Top of stack at the beginning of MRC - for stack usage calculations.
  //
  Inputs->MrcStackTop = (UINT32) &FileHandle;

  MrcCall->MrcDebugHook (MrcData, MRC_INITIALIZATION_START);
  InitStat = 0;
  ForceFullTraining = 0;
  MrcStatus = mrcSuccess;

  //
  // Unlock memory if it is necessary.
  //
  UnlockMemory (MrcData);

  //
  // Get MRC BootMode
  //
  MrcBootMode = (SysBootMode == BOOT_ON_S3_RESUME) ? bmS3 : MrcGetBootMode (MrcData);

  DEBUG_CODE_BEGIN ();
  if ((SysBootMode == BOOT_ON_S3_RESUME) && (bmCold == MrcGetBootMode (MrcData))) {
    DEBUG ((
             DEBUG_ERROR,
             "SysBootMode = %Xh and MrcBootMode = %d - Check PCH SR bit\n",
             SysBootMode,
             MrcBootMode
             ));
  }
  DEBUG_CODE_END ();

  //
  // Get Cpu Information
  //
  if (MrcSetCpuInformation (MrcData, Inputs->CpuidModel, Inputs->CpuidStepping) != mrcSuccess) {
    DEBUG ((DEBUG_ERROR, "ERROR: CPU Family/Model/Step %Xh is not supported:\n", Inputs->CpuidModel | Inputs->CpuidStepping));
    ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
  }

#ifndef MDEPKG_NDEBUG
  switch (SysBootMode) {
    case BOOT_WITH_FULL_CONFIGURATION:                  Str = BootStringFc;   break;
    case BOOT_WITH_MINIMAL_CONFIGURATION:               Str = BootStringMc;   break;
    case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:        Str = BootStringNc;   break;
    case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS: Str = BootStringFcd;  break;
    case BOOT_WITH_DEFAULT_SETTINGS:                    Str = BootStringDs;   break;
    case BOOT_ON_S4_RESUME:                             Str = BootStringS4;   break;
    case BOOT_ON_S5_RESUME:                             Str = BootStringS5;   break;
    case BOOT_ON_S2_RESUME:                             Str = BootStringS2;   break;
    case BOOT_ON_S3_RESUME:                             Str = BootStringS3;   break;
    case BOOT_ON_FLASH_UPDATE:                          Str = BootStringFu;   break;
    case BOOT_IN_RECOVERY_MODE:                         Str = BootStringRm;   break;
    case BOOT_IN_RECOVERY_MODE_MASK:                    Str = BootStringRmm;  break;
    case BOOT_SPECIAL_MASK:                             Str = BootStringSm;   break;
    default:                                            Str = BootStringUnk;  break;
  }
  DEBUG ((DEBUG_INFO, "System boot mode: %Xh - BOOT_%a\n", SysBootMode, Str));
#endif

  //
  // Locate and determine if memory configuration save data is valid.
  //
  SaveDataValid = FALSE;
  DEBUG ((
           DEBUG_INFO,
           "Restore data of size %d from 0x%X\nGlobal data of size %d is at 0x%X\n",
           (MiscPeiPreMemConfig->S3DataPtr == NULL) ? 0 : ((MrcSave *) MiscPeiPreMemConfig->S3DataPtr)->Size,
           MiscPeiPreMemConfig->S3DataPtr,
           sizeof (MrcParameters),
           &MrcData
           ));

  if ((MiscPeiPreMemConfig->S3DataPtr != NULL) && (SysBootMode != BOOT_WITH_DEFAULT_SETTINGS)) {
    SaveSys = (MrcSave *) (MiscPeiPreMemConfig->S3DataPtr);
    Save    = SaveSys;
    Crc32 = MrcCalculateCrc32 ((UINT8 *) (&Save->Data), sizeof (MrcSaveData));
    DEBUG ((DEBUG_INFO, "Calc. crc = 0x%x, Header crc = 0x%x\n", Crc32, Save->Header.Crc));
    if (Crc32 == Save->Header.Crc) {
      DEBUG ((DEBUG_INFO, "Saved memory configuration data is valid\n"));
      ((*PeiServices)->CopyMem) ((VOID *) &MrcData->Save, (VOID *) SaveSys, sizeof (MrcSave));
      SaveDataValid = TRUE;
    }
  }

  //
  // We must have memory configuration save data in order to resume from S3.
  //
  if ((SysBootMode == BOOT_ON_S3_RESUME) && (!SaveDataValid)) {
    DEBUG ((DEBUG_ERROR, "Unable to resume from S3 without valid saved memory configuration data\n"));
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  if (!(MrcBootMode == bmS3 || MrcBootMode == bmWarm)) {
    //
    // Check CPU Replaced Status, if so a system non-power cycle reset will be required.
    //
    DEBUG ((DEBUG_INFO, "Calling CpuReplacementCheck\n"));
    Status = CpuReplacementCheck (FileHandle, &ForceFullTraining);

    if (ForceFullTraining == 0x1) {
      SaveDataValid = FALSE;
    }
  }

  // Keep track of the original MRC Boot mode before an alternate flow is determined below.
  OrigMrcBootMode = MrcBootMode;

  //
  // MrcBootMode can ONLY be bmCold, bmWarm or bmS3 at this point.
  //
  Inputs->MchBarBaseAddress = MiscPeiPreMemConfig->MchBar;
  switch (MrcBootMode) {
    case bmCold:
      //
      // Advance the MRC boot mode to fast boot if the following condition is met.
      // If sticky scratchpad is set, there was a failure during previous Fast flow, so use Cold.
      //
      if ((SaveDataValid == TRUE) &&
          (MemConfig->MrcFastBoot > 0) &&
          (MrcWmRegGet (MrcData) == 0) &&
          (ColdBootRequired (MrcData, MemConfig, MrcBootMode) == FALSE)
          ) {
        MrcBootMode = bmFast;
        DEBUG ((DEBUG_INFO, "Fast boot is possible, so forcing it\n"));
      } else {
        DEBUG ((DEBUG_INFO, "Cold boot\n"));
        SaveDataValid = FALSE;
      }
      break;

    case bmWarm:
    case bmS3:
      if (SaveDataValid == FALSE) {
        DEBUG ((DEBUG_WARN, "Saved memory configuration data is not valid, forcing a cold boot\n"));
        MrcBootMode = bmCold;
        break;
      } else {
        if (ColdBootRequired (MrcData, MemConfig, MrcBootMode) == TRUE) {
          DEBUG ((
                   DEBUG_WARN,
                   "Platform settings or configuration have changed, forcing a cold boot\n"
                   ));
          MrcBootMode = bmCold;
          SaveDataValid = FALSE;
          break;
        }
        if ((MemConfigNoCrc->CleanMemory == TRUE) && (MrcBootMode == bmWarm)) {
          DEBUG ((
                   DEBUG_WARN,
                   "TCG MOR request, forcing fast boot flow to scrub memory\n"
                   ));
          MrcBootMode = bmFast;
          break;
        }
        //
        // Do not force cold boot when BOOT_ON_FLASH_UPDATE since the memory content should be retained.
        //
        if (SysBootMode != BOOT_ON_FLASH_UPDATE) {
          /*
            Check SSKPD register to determine if Warm Reset occured before MRC was reached during a cold boot.
            If so, we need to choose Fast or Cold boot path.
          */
          if ((MrcBootMode == bmWarm) && (MrcWmRegGet (MrcData) == 0)) {
            DEBUG ((
                     DEBUG_WARN,
                     "Reset occured in the cold boot path before reaching MRC.\n"
                     ));
            if (MemConfig->MrcFastBoot > 0) {
              DEBUG ((DEBUG_INFO, "Fast boot is possible, so forcing it\n"));
              MrcBootMode = bmFast;
            } else {
              DEBUG ((DEBUG_INFO, "Cold boot\n"));
              MrcBootMode = bmCold;
              SaveDataValid = FALSE;
            }
            break;
          }
        }
      }
      DEBUG ((DEBUG_INFO, (MrcBootMode == bmS3) ? "Resume from S3\n" : "Warm reset\n"));
      break;

    default:
      DEBUG ((DEBUG_INFO, "Cold boot\n"));
      MrcBootMode = bmCold;
      SaveDataValid = FALSE;
      break;
  }

  //
  // Clear MrcSave if not valid saved data. We don't want to end up with Ghost DIMMs
  //
  if (SaveDataValid == FALSE) {
    ZeroMem (&MrcData->Save, sizeof (MrcSave));
  }

  //
  // Calculate Total DPR Size
  //
  CalculateTotalDprMemorySize (&TotalDprSizeMB);

  if (TotalDprSizeMB != 0) {
    Inputs->DprSize = (UINT32) TotalDprSizeMB;
  }

  //
  // Set up the MRC input data structure.
  //
  PERF_START_EX (FileHandle, NULL, NULL, AsmReadTsc (), 0x3060);
  Inputs->BootMode = MrcSetupMrcData (
                       SysBootMode,
                       MrcBootMode,
                       MrcData,
                       MiscPeiPreMemConfig,
                       GtConfig,
                       MemConfig,
                       MemConfigNoCrc,
                       SiPreMemPolicyPpi,
                       Inputs->CpuidModel
                       );
  PERF_END_EX (FileHandle, NULL, NULL, AsmReadTsc (), 0x3061);

  //
  // Initialize MeStolenSize to 0 before we retrieving from ME FW.
  //
  Inputs->MeStolenSize = 0;


  //
  // ME Stolen Size in MB units
  //
  DEBUG ((DEBUG_ERROR, "Calling MeSendUmaSize\n"));

  Inputs->MeStolenSize = MeSendUmaSize (FileHandle);
  DEBUG ((DEBUG_INFO, "ME UMA size = %d MB\n", Inputs->MeStolenSize));

  do {
    if (Inputs->BootMode == bmCold) {
      //
      // Clear DRAM Init Bit if we are doing a cold boot, to prevent hang if a warm reset occurs in the training flow
      // where an old memory config is saved.
      //
      MrcResetDISB (MrcData);
    }
    DEBUG ((DEBUG_INFO, "MRC Start Memory Configuration\n"));
    PERF_START_EX (FileHandle, NULL, NULL, AsmReadTsc (), 0x3080);
    MrcStatus = MrcStartMemoryConfiguration (MrcData, 0);
    PERF_END_EX (FileHandle, NULL, NULL, AsmReadTsc (), 0x3081);

    switch (MrcStatus) {
      case mrcSuccess:
        break;

      case mrcFrequencyError:
        MrcGetCurrentMemoryFrequency (MrcData, (UINT32 * const) &MemoryClock, &Ratio, NULL, NULL);
        if (Ratio >= Outputs->Ratio) {
          DEBUG ((DEBUG_ERROR, "Memory initialization has failed\n"));
          //
          // Get lower byte and set the error bit
          //
          (MrcCall->MrcIoWrite8) (0x80, (MrcCall->MrcIoRead8) (0x80) | MRC_FAILURE_INDICATION);
          ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
          return EFI_DEVICE_ERROR;
        } else {
          // Restart memory configuration, using the lower frequency.
          MrcStatus = mrcColdBootRequired;
        }
        // no break;

      case mrcColdBootRequired:
        if (Inputs->BootMode == bmFast) {
          // If XMP Profile is selected, switch to Default Memory Profile.
          if (Inputs->MemoryProfile != STD_PROFILE) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Switching from XMP profile to Default\n");
            MemConfig->SpdProfileSelected = Default;
            MemConfig->tCL                = 0;
            MemConfig->tRCDtRP            = 0;
            MemConfig->tRAS               = 0;
            MemConfig->tWR                = 0;
            MemConfig->tRFC               = 0;
            MemConfig->tRRD               = 0;
            MemConfig->tWTR               = 0;
            MemConfig->tRTP               = 0;
            MemConfig->tFAW               = 0;
            MemConfig->tCWL               = 0;
            MemConfig->tREFI              = 0;
            MemConfig->NModeSupport       = 0;
            MemConfig->Ratio              = 0;
            MemConfig->RefClk             = 0;
            MemConfig->OddRatioMode       = 0;
            MemConfig->VddVoltage         = VDD_INVALID;
          }
          // At this point, input structure has limited data.
          // We need to initialize the input structure for the cold boot.
          MrcBootMode = bmCold;
          Inputs->BootMode = MrcSetupMrcData (
                               SysBootMode,
                               MrcBootMode,
                               MrcData,
                               MiscPeiPreMemConfig,
                               GtConfig,
                               MemConfig,
                               MemConfigNoCrc,
                               SiPreMemPolicyPpi,
                               Inputs->CpuidModel
                               );
        } else {
          Inputs->BootMode = bmCold;
        }
        break;

      case mrcDimmNotExist:
        //
        // Set memory init status = 0x1 and send DRAM Init Done to ME FW,
        // indicating that no memory exists in the system.
        //
        InitStat = 0x1;
        Status = MeConfigDidReg (FileHandle, InitStat, Outputs->MemoryMapData.PttStolenBase, Inputs->MeStolenSize);
        DEBUG ((DEBUG_INFO, "PttStolenBase = 0x%08X \n", Outputs->MemoryMapData.PttStolenBase));

        MrcCall->MrcDebugHook (MrcData, MRC_NO_MEMORY_DETECTED);
        DEBUG ((DEBUG_ERROR, "There are no DIMMs present in the system\n"));
        //
        //Indicate to the caller that memory has not been detected.
        //
        (*PeiServices)->ReportStatusCode (
                           PeiServices,
                           EFI_ERROR_CODE,
                           EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED,
                           0,    // Instance
                           NULL, // *CallerId OPTIONAL
                           NULL  // *Data OPTIONAL
                           );
        // no break;

      default:
        DEBUG ((DEBUG_ERROR, "Memory initialization has failed\n"));
        //
        // Get lower byte and set the error bit
        //
        (MrcCall->MrcIoWrite8) (0x80, (MrcCall->MrcIoRead8) (0x80) | MRC_FAILURE_INDICATION);
        ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
        return EFI_DEVICE_ERROR;
    }
  } while (MrcStatus == mrcColdBootRequired);

  DEBUG ((
           DEBUG_INFO,
           "Lowest StackAddr: 0x%x\tTop StackAddr: 0x%x\tDepth: %d\n",
           Debug->LowestStackAddr,
           Debug->TopStackAddr,
           Debug->TopStackAddr - Debug->LowestStackAddr
           ));
  //
  // Intel Silicon View Technology (ISVT) IO Reading port 0x84 with EAX = 0x100 for End of MRC
  //
#if defined __GNUC__  // GCC compiler
  __asm__ __volatile__ (
    "\n\t mov $0x100, %eax"
    "\n\t in $0x84, %al"
    );
#else //MSFT compiler
  ASM {
    mov EAX, 100h
    in  AL, 84h
  }
#endif

  PERF_START_EX (FileHandle, NULL, NULL, AsmReadTsc (), 0x3090);
  if ((MrcBootMode == bmCold) || (MrcBootMode == bmFast)) {
    //
    // Perform simple memory test.
    //
    if (mrcFail == BasicMemoryTest (MrcData)) {
      if (MrcBootMode == bmFast) {
        if (Inputs->RetrainOnFastFail) {
          //
          // BasicMemoryTest failed in Fast flow - fall back to Cold boot flow.
          //
          DEBUG ((DEBUG_ERROR, "BasicMemoryTest failed in Fast flow - reset to run MRC in Cold flow !!\n\n\n"));
          IoWrite16 (0x80, 0);  // Clear 16-bit port80
          (*PeiServices)->ResetSystem2 (EfiResetCold, EFI_SUCCESS, 0, NULL);
          CpuDeadLoop ();
        }
      }
      MrcCall->MrcDebugHook (MrcData, MRC_MEM_INIT_DONE_WITH_ERRORS);
      ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
      return EFI_DEVICE_ERROR;
    }
  } else {
    //
    // Perform simple non-destructive memory test for Warm / S3 resume.
    // On S3 Resume run this test only for Debug builds, to avoid S3 exit latency increase for Release builds.
    //
#ifdef MDEPKG_NDEBUG
    if (MrcBootMode != bmS3) {
#endif // MDEPKG_NDEBUG
      if (mrcFail == BasicMemoryTestS3 (MrcData)) {
        MrcCall->MrcDebugHook (MrcData, MRC_MEM_INIT_DONE_WITH_ERRORS);
        ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
        return EFI_DEVICE_ERROR;
      }
#ifdef MDEPKG_NDEBUG
    }
#endif // MDEPKG_NDEBUG
  }
  PERF_END_EX (FileHandle, NULL, NULL, AsmReadTsc (), 0x3091);

  //
  // Configure "ME DRAM Init Done Register"
  // ME UMA Size outside of a 0MB-64MB range is not defined or if BDF 0:22:0 is not present, exit.
  //

  DEBUG ((DEBUG_INFO, "Check for Memory Retrain on warm reset -- MrcBootMode=0x%02X OrigBootMode=0x%02X\n", MrcBootMode, OrigMrcBootMode));
  // On warm reset if memory coherency was not maintained (forced Cold Reset flow), set the DID message
  //  to indicate that memory was not preserved across reset, so that ME will reload the FW from NV memory.
  if ((bmWarm == OrigMrcBootMode) && ((bmCold == MrcBootMode) || (MemConfigNoCrc->CleanMemory == TRUE))) {
    DEBUG ((DEBUG_INFO, "Memory retrain occurred during warm reset.  Force ME FW reload.\n"));
    // Set the flag to tell FW that memory was not maintained InitStat bits 3:0 = (3).
    InitStat = (InitStat & 0xF0) | 0x3;
  }

  DEBUG ((DEBUG_INFO, "ME UMA Size requested: %d MB\n", Inputs->MeStolenSize));
  if ((Inputs->MeStolenSize > 0x40) || (MmioRead32 (MmPciBase (0, 22, 0) + 0x10) == 0xffffffff)) {
    DEBUG ((DEBUG_ERROR, "Invalid ME UMA Size requested.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "Calling MeConfigDidReg\n"));
    Status = MeConfigDidReg ( FileHandle, InitStat, Outputs->MemoryMapData.PttStolenBase, Inputs->MeStolenSize);
    DEBUG ((DEBUG_INFO, "MeDramInitDone Complete.\n"));
  }

  //
  // If memory is not preserved across reset then proceed through normal boot instead of Boot on flash update
  //
  if (((MrcBootMode == bmCold) || (MrcBootMode == bmFast)) && (SysBootMode == BOOT_ON_FLASH_UPDATE)) {
    SysBootMode = BOOT_WITH_FULL_CONFIGURATION;
    Status = PeiServicesSetBootMode (SysBootMode);
  }

// AMI_OVERRIDE_SA0008_START >>>
  Status = (*PeiServices)->InstallPpi(PeiServices, &mAmiPeiCompelteMrcDesc[0]);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_ERROR, "Install Complete MRC Ppi.\n"));
// AMI_OVERRIDE_SA0008_END <<<
// AMI_OVERRIDE_SA0009_START >>> EIP 102852
  Status = (*PeiServices)->GetBootMode( PeiServices, &SysBootMode );
// AMI_OVERRIDE_SA0009_END <<<
  //
  // Install EFI memory HOBs
  //
  if (SysBootMode != BOOT_ON_S3_RESUME) {
#ifdef MRC_DEBUG_PRINT
    Debug->Current = 0;
#endif // MRC_DEBUG_PRINT
    BuildMemoryS3DataHob (MrcData);
    BuildMemoryInfoDataHob (MrcData);
    BuildMemoryPlatformDataHob (MrcData, Hob);
    ZeroMem ((VOID *) &Hob->Buffer, HobBufferSize);
// AMI_OVERRIDE_SA0026_START >>>
#ifdef NB_SMBIOS_TYPE_17_CUSTOMIZATION_FLAG
    BuildNbSmbiosType17ExtraInfoDataHob (MrcData);
#endif // NB_SMBIOS_TYPE_17_CUSTOMIZATION_FLAG
// AMI_OVERRIDE_SA0026_END <<<
  } else {
    // Make sure DISB is set during S3 Resume, must be done after MrcStartMemoryConfiguration() is completed.
    MrcSetDISB (MrcData);
  }
  Status = InstallEfiMemory(MrcData, PeiServices, SysBootMode, MiscPeiPreMemConfig
    );
  ASSERT_EFI_ERROR(Status);

  DEBUG ((DEBUG_INFO, "MemoryInit Complete.\n"));
  MrcCall->MrcDebugHook (MrcData, MRC_MEM_INIT_DONE);
// AMI_OVERRIDE_SA0008_START >>>
  Status = (*PeiServices)->InstallPpi(PeiServices, &mAmiPeiEndOfMrcDesc[0]);
  ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_SA0008_END <<<

  return Status;
}

/**
  Retrieves the data structure associated with the GUIDed HOB of type gEfiSmmPeiSmramMemoryReserveGuid

  @retval NULL   A HOB of type gEfiSmmPeiSmramMemoryReserveGuid could not be found.
  @retval !NULL  A pointer to the GUID data from a HIB of type gEfiSmmPeiSmramMemoryReserveGuid

**/
EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *
GetSrmamHobData (
  VOID
  )
{
  VOID  *GuidHob;

  //
  // Search SmramMemoryReserve HOB that describes SMRAM region
  //
  GuidHob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (GuidHob == NULL) {
    return NULL;
  }
  return (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) GET_GUID_HOB_DATA (GuidHob);
}

/**
  This routine will split SmramReserve hob to reserve 1 page for SMRAM content in S3 phase
  for R9 SMM core.

  @retval EFI_SUCCESS           The gEfiSmmPeiSmramMemoryReserveGuid is splited successfully.
  @retval EFI_NOT_FOUND         The gEfiSmmPeiSmramMemoryReserveGuid is not found.

**/
EFI_STATUS
EFIAPI
SplitSmramReserveHob (
  VOID
  )
{
  EFI_HOB_GUID_TYPE                *GuidHob;
  EFI_PEI_HOB_POINTERS             Hob;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK   *DescriptorBlock;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK   *NewDescriptorBlock;
  UINTN                            BufferSize;
  UINTN                            SmramRanges;
  UINTN                            Index;
  UINTN                            SubIndex;

  //
  // Retrieve the GUID HOB data that contains the set of SMRAM descriptyors
  //
  GuidHob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  DescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) GET_GUID_HOB_DATA (GuidHob);

  //
  // Allocate one extra EFI_SMRAM_DESCRIPTOR to describe a page of SMRAM memory that contains a pointer
  // to the SMM Services Table that is required on the S3 resume path
  //
  SmramRanges = DescriptorBlock->NumberOfSmmReservedRegions;
  BufferSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK) + (SmramRanges * sizeof (EFI_SMRAM_DESCRIPTOR));

  Hob.Raw = BuildGuidHob (
              &gEfiSmmPeiSmramMemoryReserveGuid,
              BufferSize
              );
  ASSERT (Hob.Raw);
  NewDescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) Hob.Raw;

  //
  // Copy old EFI_SMRAM_HOB_DESCRIPTOR_BLOCK to new allocated region
  //
  CopyMem ((VOID *) Hob.Raw, DescriptorBlock, BufferSize - sizeof (EFI_SMRAM_DESCRIPTOR));

  //
  // Increase the number of SMRAM descriptors by 1 to make room for the ALLOCATED descriptor of size EFI_PAGE_SIZE
  //
  NewDescriptorBlock->NumberOfSmmReservedRegions = (UINT32) (SmramRanges + 1);

  ASSERT (SmramRanges >= 1);
  //
  // Copy last entry to the end - we assume TSEG is last entry, which is same assumption as R8 CPU/SMM driver
  //
  CopyMem (&NewDescriptorBlock->Descriptor[SmramRanges], &NewDescriptorBlock->Descriptor[SmramRanges - 1], sizeof (EFI_SMRAM_DESCRIPTOR));

  //
  // Update the last but 1 entry in the array with a size of EFI_PAGE_SIZE and put into the ALLOCATED state
  //
  NewDescriptorBlock->Descriptor[SmramRanges - 1].PhysicalSize    = EFI_PAGE_SIZE;
  NewDescriptorBlock->Descriptor[SmramRanges - 1].RegionState    |= EFI_ALLOCATED;

  //
  // Reduce the size of the last SMRAM descriptor by EFI_PAGE_SIZE
  //
  NewDescriptorBlock->Descriptor[SmramRanges].PhysicalStart += EFI_PAGE_SIZE;
  NewDescriptorBlock->Descriptor[SmramRanges].CpuStart      += EFI_PAGE_SIZE;
  NewDescriptorBlock->Descriptor[SmramRanges].PhysicalSize  -= EFI_PAGE_SIZE;

  //
  // Now, we have created SmramReserve Hob for SmmAccess drive. But the issue is that, R8 SmmAccess will assume there is 2 SmramReserve region only.
  // Reporting 3 SmramReserve region will cause buffer overflow. Moreover, we would like to filter AB-SEG or H-SEG to avoid SMM cache-poisoning issue.
  // So we uses scan SmmReserve Hob to remove AB-SEG or H-SEG.
  //
  for (Index = 0; Index <= SmramRanges; Index++) {
    if (NewDescriptorBlock->Descriptor[Index].PhysicalSize == 0) {
      //
      // Skip zero entry
      //
      continue;
    }
    if (NewDescriptorBlock->Descriptor[Index].PhysicalStart < BASE_1MB) {
      //
      // Find AB-SEG or H-SEG
      // remove this region
      //
      for (SubIndex = Index; SubIndex < NewDescriptorBlock->NumberOfSmmReservedRegions - 1; SubIndex++) {
        CopyMem (&NewDescriptorBlock->Descriptor[SubIndex], &NewDescriptorBlock->Descriptor[SubIndex + 1], sizeof (EFI_SMRAM_DESCRIPTOR));
      }
      //
      // Zero last one
      //
      ZeroMem (&NewDescriptorBlock->Descriptor[SubIndex], sizeof (EFI_SMRAM_DESCRIPTOR));
      //
      // Decrease Number
      //
      NewDescriptorBlock->NumberOfSmmReservedRegions --;
      //
      // Decrease Index to let it test mew entry
      //
      Index --;
    }
  }

  //
  // Last step, we can scrub old one
  //
  ZeroMem (&GuidHob->Name, sizeof (GuidHob->Name));

  return EFI_SUCCESS;
}

/**
  This routine will create AcpiVariable hob to point the reserved smram in S3 phase
  for R9 SMM core.

  @retval EFI_SUCCESS           The variable hob with gEfiAcpiVariableGuid is created successfully.
  @retval EFI_NOT_FOUND         The gEfiSmmPeiSmramMemoryReserveGuid is not found.

**/
EFI_STATUS
CreateAcpiVariableHob (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS             Hob;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK   *DescriptorBlock;
  UINTN                            SmramRanges;

  //
  // Retrieve the GUID HOB data that contains the set of SMRAM descriptyors
  //
  DescriptorBlock = GetSrmamHobData ();
  if (DescriptorBlock == NULL) {
    return EFI_NOT_FOUND;
  }

  Hob.Raw = BuildGuidHob (
              &gEfiAcpiVariableGuid,
              sizeof (EFI_SMRAM_DESCRIPTOR)
              );
  ASSERT (Hob.Raw);

  //
  // It should be already patch, so just copy last but 1 region directly.
  //
  SmramRanges = DescriptorBlock->NumberOfSmmReservedRegions;
  ASSERT (SmramRanges >= 2);
  if (SmramRanges >= 2) {
    CopyMem ((VOID *) Hob.Raw, &DescriptorBlock->Descriptor[SmramRanges - 2], sizeof (EFI_SMRAM_DESCRIPTOR));
  }

  return EFI_SUCCESS;
}

/**
  Driver Entry for AcpiVariableHobOnSmramReservHob PEIM

  @param   FileHandle       Handle of the file being invoked.
  @param   PeiServices      Describes the list of possible PEI Services.

  @retval EFI_SUCCESS      Success create variable hob with gEfiAcpiVariableGuid and
                           split gEfiSmmPeiSmramMemoryReserveGuid.
  @retval EFI_NOT_FOUND    Can not get gEfiSmmPeiSmramMemoryReserveGuid hob

**/
EFI_STATUS
CreateS3AcpiVariableHob (
  VOID
  )
{
  EFI_STATUS              Status;

  //
  // Split SmramReserve hob, which is required for R9 SMM Core for S3.
  //
  Status = SplitSmramReserveHob ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Create AcpiVariable hob, which is required for R9 SMM Core for S3.
  //
  Status = CreateAcpiVariableHob ();

  return Status;
}

/**
  This function installs memory for all paths.

  @param[in] MrcData                - Mrc data structure
  @param[in] PeiServices            - PEI Services table.
  @param[in] SysBootMode            - The specific boot path that is being followed.
  @param[in] MiscPeiPreMemConfig    - SA_MISC_PEI_PREMEM_CONFIG structure.

  @retval EFI_SUCCESS          The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES Out of Resources.
**/
EFI_STATUS
InstallEfiMemory (
  IN MrcParameters *CONST      MrcData,
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN EFI_BOOT_MODE             SysBootMode,
  IN SA_MISC_PEI_PREMEM_CONFIG *MiscPeiPreMemConfig
  )
{
  IN CONST MrcInput                     *Inputs;
  EFI_STATUS                            Status;
  UINT8                                 Index;
  UINT8                                 NumRanges;
  UINT8                                 SmramIndex;
  UINT8                                 SmramRanges;
  UINT64                                PeiMemoryLength;
  UINT64                                InstalledPeiMemoryLength;
  UINT64                                RangeLength;
  UINTN                                 BufferSize;
  UINTN                                 PeiMemoryIndex;
  UINTN                                 McD0BaseAddress;
  EFI_PEI_HOB_POINTERS                  Hob;
  EFI_PHYSICAL_ADDRESS                  PeiMemoryBaseAddress;
  EFI_PHYSICAL_ADDRESS                  InstalledPeiMemoryBaseAddress;
  UINT32                                RequiredMemSize;
  EFI_PHYSICAL_ADDRESS                  TopUseableMemAddr;
  EFI_PHYSICAL_ADDRESS                  TopUseableMemSize;
  EFI_PHYSICAL_ADDRESS                  Tom;
  MEMORY_TEST_LEVEL                     MemoryTestLevel;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramHobDescriptorBlock;
  EFI_SMRAM_DESCRIPTOR                  *SmramDescriptor;
  PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE MemoryMap[MAX_RANGES];
  EFI_PHYSICAL_ADDRESS                  BadMemoryAddress;
  EFI_RESOURCE_TYPE                     ResourceType;
  EFI_RESOURCE_ATTRIBUTE_TYPE           ResourceAttribute;
  SI_PREMEM_POLICY_PPI                  *SiPreMemPolicyPpi;
  MEMORY_CONFIG_NO_CRC                  *MemConfigNoCrc;
  EFI_RESOURCE_ATTRIBUTE_TYPE           ResourceAttributeTested;
#ifndef FSP_FLAG
  PEI_CAPSULE_PPI                       *Capsule;
  VOID                                  *CapsuleBuffer;
  UINTN                                 CapsuleBufferLength;
  EFI_STATUS                            CapsulePpiStatus;
#else
  BOOLEAN                               IsTsegRegion;
  FSPM_UPD                              *FspmUpdDataPtr;
  UINT32                                BootLoaderTolumSize;
#endif

  Inputs = &MrcData->Inputs;
  DEBUG ((DEBUG_INFO, "Install EFI memory.\n"));

  //
  // Get the Memory Map
  //
  NumRanges = MAX_RANGES;
  ZeroMem (MemoryMap, sizeof (PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE) * NumRanges);
  Status = GetMemoryMap (MrcData, (PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE *) MemoryMap, &NumRanges);
  ASSERT_EFI_ERROR (Status);

  //
  // Find the highest memory range in processor native address space to give to
  // PEI. Then take the top.
  // If this algorithm changes, then we need to fix the capsule memory
  // selection algorithm below.
  //
  PeiMemoryBaseAddress = 0;
  InstalledPeiMemoryBaseAddress = 0;
  InstalledPeiMemoryLength = 0;
  ResourceAttributeTested = 0;

  //
  // Query the platform for the minimum memory size.
  //
  PeiMemoryLength = 0;
  SiPreMemPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  MemConfigNoCrc = NULL;
  if (SiPreMemPolicyPpi != NULL) {
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMemoryConfigNoCrcGuid, (VOID *) &MemConfigNoCrc);
    if (MemConfigNoCrc != NULL) {
      PeiMemoryLength = MemConfigNoCrc->PlatformMemorySize;
      DEBUG ((DEBUG_INFO, "PeiMemoryLength: %x, %llx\n",PeiMemoryLength,PeiMemoryLength));
    }
  }

#ifndef FSP_FLAG
  //
  // Get required memory size for ACPI use. This helps to put ACPI memory on the top.
  //
  RequiredMemSize = 0;
  RetrieveRequiredMemorySize (&RequiredMemSize);
  DEBUG ((DEBUG_INFO, "Required memory size = %Xh (%d) bytes\n", RequiredMemSize, RequiredMemSize));
#else
  RequiredMemSize = PcdGet32 (PcdFspReservedMemoryLength);

  FspmUpdDataPtr = (FSPM_UPD *) GetFspMemoryInitUpdDataPointer ();
  BootLoaderTolumSize = FspmUpdDataPtr->FspmArchUpd.BootLoaderTolumSize;

  if ((RequiredMemSize + BootLoaderTolumSize) > PeiMemoryLength) {
    DEBUG ((DEBUG_ERROR, "Error: BootLoaderTolumSize Greater than available memory.\n"));
    DEBUG ((DEBUG_ERROR, "Force BootLoaderTolumSize to zero.\n"));
    BootLoaderTolumSize = 0;
  }
#endif

  PeiMemoryIndex = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    DEBUG ((DEBUG_INFO, "Found %016Xh bytes at ", MemoryMap[Index].RangeLength));
    DEBUG ((DEBUG_INFO, "%016Xh\n", MemoryMap[Index].PhysicalAddress));
    if ((MemoryMap[Index].Type == DualChannelDdrMainMemory) &&
        (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength < MAX_ADDRESS) &&
        (MemoryMap[Index].PhysicalAddress >= PeiMemoryBaseAddress) &&
        (MemoryMap[Index].RangeLength >= PeiMemoryLength)) {
      PeiMemoryBaseAddress = MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength - PeiMemoryLength;
      PeiMemoryIndex       = Index;
    }
  }

  //
  // Test only the PEI memory if necessary. Some platforms do not require the
  // Base Memory PEIM to be present.
  //
  switch (SysBootMode) {

    case BOOT_WITH_FULL_CONFIGURATION:
      MemoryTestLevel = Quick;
      break;

    case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
      MemoryTestLevel = Extensive;
      break;

    default:
      MemoryTestLevel = Ignore;
      break;
  }
  if (SysBootMode != BOOT_ON_S3_RESUME) {
    ResourceAttributeTested = EFI_RESOURCE_ATTRIBUTE_TESTED;

   (*PeiServices)->ReportStatusCode (
             PeiServices,
             EFI_PROGRESS_CODE,                                  // Type
             EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_TEST,  // Value
             0,    // Instance
             NULL, // *CallerId OPTIONAL
             NULL  // *Data OPTIONAL
             );
  }


  if ((MrcGetBootMode (MrcData) == bmWarm) && (MemConfigNoCrc != NULL)) {
    if (MemConfigNoCrc->MemTestOnWarmBoot == 0) {
      MemoryTestLevel = Ignore;
    }
  }
  Status = BaseMemoryTest (
             MrcData,
             PeiMemoryBaseAddress,
             PeiMemoryLength,
             MemoryTestLevel,
             &BadMemoryAddress
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Memory test failure at %lXh.\n", BadMemoryAddress));
  }

  ASSERT_EFI_ERROR (Status);

  //@todo - Move EDKII non-FSP capsule outside of FSP code.
#ifndef FSP_FLAG
  Capsule             = NULL;
  CapsulePpiStatus    = EFI_NOT_FOUND; // Set Fail status as default.
  CapsuleBuffer       = NULL;
  CapsuleBufferLength = 0;

  if (SysBootMode == BOOT_ON_FLASH_UPDATE) {
    CapsulePpiStatus = PeiServicesLocatePpi (
               &gPeiCapsulePpiGuid,
               0,
               NULL,
               (VOID **) &Capsule
               );
    ASSERT_EFI_ERROR (CapsulePpiStatus);
  }
#endif

#ifndef FSP_FLAG
  if (SysBootMode != BOOT_ON_S3_RESUME) {
    InstalledPeiMemoryBaseAddress = PeiMemoryBaseAddress;
    //
    // Carve out the top memory reserved for ACPI.
    //
    InstalledPeiMemoryLength = PeiMemoryLength - RequiredMemSize;
  } else {
    InstalledPeiMemoryBaseAddress = MiscPeiPreMemConfig->AcpiReservedMemoryBase;
    InstalledPeiMemoryLength = MiscPeiPreMemConfig->AcpiReservedMemorySize;
  }
#else
  InstalledPeiMemoryBaseAddress = PeiMemoryBaseAddress + PeiMemoryLength - BootLoaderTolumSize - RequiredMemSize;
  InstalledPeiMemoryLength = RequiredMemSize;
#endif

  DEBUG ((DEBUG_INFO, "Building RESOURCE_SYSTEM_MEMORY Hob: \n"));
  DEBUG ((DEBUG_INFO, "PeiMemoryBaseAddress = %lXh, PeiMemoryLength = %lXh\n", InstalledPeiMemoryBaseAddress, InstalledPeiMemoryLength));

#ifndef FSP_FLAG
  if (SysBootMode != BOOT_ON_S3_RESUME) {
    BuildResourceDescriptorHob (
      EFI_RESOURCE_SYSTEM_MEMORY,
      (
        EFI_RESOURCE_ATTRIBUTE_PRESENT |
        EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
        ResourceAttributeTested |
        EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
        ),
      PeiMemoryBaseAddress,
      PeiMemoryLength
      );
  }
#else

  // Build HOB that describes at BootLoader TOLUM Resource reserved at top of main memory below 4GB.

  if (BootLoaderTolumSize > 0) {
    DEBUG ((DEBUG_INFO, "FSP_BOOT_LOADER_TOLUM_HOB\n"));
    DEBUG ((DEBUG_INFO, "FspBootLoaderTolum Base : %x\n", PeiMemoryBaseAddress + PeiMemoryLength - BootLoaderTolumSize));
    DEBUG ((DEBUG_INFO, "FspBootLoaderTolum Size : %x\n", BootLoaderTolumSize));
    BuildResourceDescriptorWithOwnerHob (
      EFI_RESOURCE_MEMORY_RESERVED,             // MemoryType,
      (
        EFI_RESOURCE_ATTRIBUTE_PRESENT |
        EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
        ResourceAttributeTested |
        EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
        ),
      PeiMemoryBaseAddress + PeiMemoryLength - BootLoaderTolumSize,
      BootLoaderTolumSize,
      &gFspBootLoaderTolumHobGuid
      );
  }

  // HOB that describes FSP reserved memory below BootLoader TOLUM.

  DEBUG ((DEBUG_INFO, "FSP_RESERVED_MEMORY_RESOURCE_HOB\n"));
  DEBUG ((DEBUG_INFO, "FspReservedMemoryResource Base : %x\n", PeiMemoryBaseAddress + PeiMemoryLength - BootLoaderTolumSize - RequiredMemSize));
  DEBUG ((DEBUG_INFO, "FspReservedMemoryResource Size : %x\n", RequiredMemSize));
  BuildResourceDescriptorWithOwnerHob (
    EFI_RESOURCE_MEMORY_RESERVED,             // MemoryType,
    (
      EFI_RESOURCE_ATTRIBUTE_PRESENT |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      ResourceAttributeTested |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
      ),
    InstalledPeiMemoryBaseAddress,
    InstalledPeiMemoryLength,
    &gFspReservedMemoryResourceHobGuid
    );

#endif

  //
  // Install physical memory descriptor hobs for each memory range.
  //
  SmramRanges = 0;
  for (Index = 0; Index < NumRanges; Index++) {
#ifdef FSP_FLAG
  IsTsegRegion = FALSE;
#endif
    if (MemoryMap[Index].Type == DualChannelDdrMainMemory) {
      if (Index == PeiMemoryIndex) {
        //
        // This is a partially tested Main Memory range, give it to EFI
        //
#ifndef FSP_FLAG
        if (SysBootMode != BOOT_ON_S3_RESUME){
          RangeLength = MemoryMap[Index].RangeLength - PeiMemoryLength;
        } else {
          RangeLength = MemoryMap[Index].RangeLength;
        }
#else
        //
        // In this RangeLength remove RequiredMemSize and BootLoaderTolumSize as it is described in
        // the gFspBootLoaderTolumHobGuid and gFspReservedMemoryResourceHobGuid Resource HOBs.
        //
        RangeLength = MemoryMap[Index].RangeLength - RequiredMemSize - BootLoaderTolumSize;
#endif
      } else {
        //
        // This is an untested Main Memory range, give it to EFI.
        //
        RangeLength = MemoryMap[Index].RangeLength;
      }
  //@todo - Move EDKII non-FSP capsule outside of FSP code.
#ifndef FSP_FLAG
      if (CapsulePpiStatus == EFI_SUCCESS) {
        //
        // Find the largest memory range excluding that given to PEI.
        // This must be the last thing done before calling BuildResourceDescriptorHob with EFI_RESOURCE_SYSTEM_MEMORY.
        // If any EFI_RESOURCE_SYSTEM_MEMORY node is changed after this, that will break Capsule handling.
        //
        if (MemoryMap[Index].PhysicalAddress + RangeLength < MAX_ADDRESS) {
          if (Index != PeiMemoryIndex) {
            if (RangeLength > CapsuleBufferLength) {
              CapsuleBuffer       = (VOID *) ((UINTN) MemoryMap[Index].PhysicalAddress);
              CapsuleBufferLength = (UINTN) RangeLength;
            }
          } else {
            if (RangeLength >= CapsuleBufferLength) {
              CapsuleBuffer       = (VOID *) ((UINTN) MemoryMap[Index].PhysicalAddress);
              CapsuleBufferLength = (UINTN) RangeLength;
            }
          }
        }
      }
#endif
      BuildResourceDescriptorHob (
        EFI_RESOURCE_SYSTEM_MEMORY,
        (
          EFI_RESOURCE_ATTRIBUTE_PRESENT |
          EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
          //
          // RC Override: mark <4G available memory as tested to give DXE enough memory space, so that default
          // memory allocations won't occupy the bins for specific memory types.
          //
          ResourceAttributeTested |
          EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
          EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
          ),
        MemoryMap[Index].PhysicalAddress,
        RangeLength
        );

      //
      // Test this memory range
      //
      Status = BaseMemoryTest (
                 MrcData,
                 MemoryMap[Index].PhysicalAddress,
                 RangeLength,
                 MemoryTestLevel,
                 &BadMemoryAddress
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Memory test failure at %lXh.\n", BadMemoryAddress));
        ASSERT_EFI_ERROR (Status);
      }
    } else {
      ResourceAttribute = 0;
      if ((
            (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
            (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
            ) &&
          (MemoryMap[Index].PhysicalAddress != MC_ABSEG_HSEG_PHYSICAL_START)) {
        //
        // Only count and report TSEG.
        //
#ifdef FSP_FLAG
        IsTsegRegion = TRUE;
#endif
        SmramRanges++;
      } else if (MemoryMap[Index].Type == DualChannelDdrReservedWriteBackMemory) {
        ResourceAttribute = EFI_RESOURCE_ATTRIBUTE_PRESENT |
                            EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                            ResourceAttributeTested |
                            EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;
      }
      //
      // Make sure non-system memory is marked as reserved.
      //
#ifdef FSP_FLAG
      if (IsTsegRegion) {
        //
        // Build an Tseg resource Hob for Fsp
        //
        BuildResourceDescriptorWithOwnerHob (
          EFI_RESOURCE_MEMORY_RESERVED,          //MemoryType,
          ResourceAttribute,                     //MemoryAttribute
          MemoryMap[Index].PhysicalAddress,      //MemoryBegin
          MemoryMap[Index].RangeLength,          //MemoryLength
          &gFspReservedMemoryResourceHobTsegGuid //OwnerGUID
        );
      } else {
#endif
        BuildResourceDescriptorHob (
          EFI_RESOURCE_MEMORY_RESERVED,     // MemoryType,
          ResourceAttribute,                // MemoryAttribute
          MemoryMap[Index].PhysicalAddress, // MemoryBegin
          MemoryMap[Index].RangeLength      // MemoryLength
        );
#ifdef FSP_FLAG
      }
#endif
    }
  }

  BufferSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK);
  if (SmramRanges > 0) {
    BufferSize += ((SmramRanges - 1) * sizeof (EFI_SMRAM_DESCRIPTOR));
  }

  Hob.Raw = BuildGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid, BufferSize);
  if (Hob.Raw == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmramHobDescriptorBlock = (void *) (Hob.Raw);
  SmramHobDescriptorBlock->NumberOfSmmReservedRegions = SmramRanges;

  SmramIndex = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    ResourceAttribute = 0;
    if ((
          (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
          (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
          ) &&
        (MemoryMap[Index].PhysicalAddress != MC_ABSEG_HSEG_PHYSICAL_START)) {
      //
      // This is an SMRAM range (not reporting AB_SEG or H_SEG), create an SMRAM descriptor.
      //
      SmramDescriptor = &SmramHobDescriptorBlock->Descriptor[SmramIndex];
      SmramDescriptor->PhysicalStart = MemoryMap[Index].PhysicalAddress;
      SmramDescriptor->CpuStart      = MemoryMap[Index].CpuAddress;

      //
      // RangeLength includes alignment adjustment.
      //
      if (SmramDescriptor->PhysicalStart < 0x100000) {
        SmramDescriptor->PhysicalSize  = MemoryMap[Index].RangeLength;
      } else {
        SmramDescriptor->PhysicalSize  = (Inputs->TsegSize - Inputs->IedSize )* 0x100000;
      }
      DEBUG ((DEBUG_INFO, "TSeg base is %Xh\n", SmramDescriptor->PhysicalStart));
      DEBUG ((DEBUG_INFO, "TSeg SMRAM size is %Xh\n", SmramDescriptor->PhysicalSize));

      if (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) {
        SmramDescriptor->RegionState = EFI_SMRAM_CLOSED | EFI_CACHEABLE;
      } else {
        SmramDescriptor->RegionState = EFI_SMRAM_CLOSED;
      }
      SmramIndex++;
    }
  }
  //
  // Get the current "Top of Upper Usable Memory" address from TOUUD.
  // If TOUUD > 4G, it means memory is re-mapped.
  //
  McD0BaseAddress = MmPciBase (SA_MC_BUS, 0, 0);
  TopUseableMemSize = (UINT64) (MmioRead32 (McD0BaseAddress + R_SA_TOUUD) + LShiftU64 (MmioRead32 (McD0BaseAddress + R_SA_TOUUD + 4), 32)) & B_SA_TOUUD_TOUUD_MASK;
  TopUseableMemAddr = MEM_EQU_4GB;
  Tom = (UINT64) (MmioRead32 (McD0BaseAddress + R_SA_TOM) + LShiftU64 (MmioRead32 (McD0BaseAddress + R_SA_TOM + 4), 32)) & B_SA_TOM_TOM_MASK;

  if (TopUseableMemSize > MEM_EQU_4GB) {
    //
    // This is above 4G memory address, give it to EFI.
    // If memory hob length is above 4G length, cut and separate it.
    //
    while ((TopUseableMemSize - MEM_EQU_4GB) > MEM_EQU_4GB) {
      DEBUG ((DEBUG_INFO, "Found 0x100000000 bytes at 0x%016lX\n", TopUseableMemAddr));

      if (Inputs->MemoryTrace && (TopUseableMemAddr + MEM_EQU_4GB > RShiftU64 (Tom, 1))) {
        //
        // Mark memory above 4GB as reserved if it's used for Memory Trace
        //
        ResourceType      = EFI_RESOURCE_MEMORY_RESERVED;
        ResourceAttribute = 0;
      } else {
        ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
        ResourceAttribute = EFI_RESOURCE_ATTRIBUTE_PRESENT |
                            EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                            EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;
      }
      BuildResourceDescriptorHob (
        ResourceType,       // MemoryType,
        ResourceAttribute,  // MemoryAttribute
        TopUseableMemAddr,  // MemoryBegin
        MEM_EQU_4GB         // MemoryLength
        );
      TopUseableMemSize = TopUseableMemSize - MEM_EQU_4GB;
      TopUseableMemAddr = TopUseableMemAddr + MEM_EQU_4GB;
    }
    //
    // Create hob for remaining memory which is above 4G memory address.
    //
    if (TopUseableMemSize > MEM_EQU_4GB) {
      DEBUG ((DEBUG_INFO, "Found 0x%016lX bytes at ", TopUseableMemSize - MEM_EQU_4GB));
      DEBUG ((DEBUG_INFO, "0x%016lX\n", TopUseableMemAddr));
    }
    if (Inputs->MemoryTrace) {
      //
      // Mark memory above 4GB as reserved if it's used for Memory Trace
      //
      ResourceType      = EFI_RESOURCE_MEMORY_RESERVED;
      ResourceAttribute = 0;
    } else {
      ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
      ResourceAttribute = EFI_RESOURCE_ATTRIBUTE_PRESENT |
                          EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                          EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;
    }
    BuildResourceDescriptorHob (
      ResourceType,                     // MemoryType,
      ResourceAttribute,                // MemoryAttribute
      TopUseableMemAddr,                // MemoryBegin
      (TopUseableMemSize - MEM_EQU_4GB) // MemoryLength
      );
  }

#ifdef FSP_FLAG
  // @todo: Is allocation needed.

  //
  // Allocate I/O space resource
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_IO,
    (
      EFI_RESOURCE_ATTRIBUTE_PRESENT     |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE
      ),
    RES_IO_BASE,
    RES_IO_LIMIT - RES_IO_BASE + 1
    );
  DEBUG ((DEBUG_INFO, " Providing I/O space resource from 0x%X to 0x%X\n", RES_IO_BASE, RES_IO_LIMIT - RES_IO_BASE + 1));
#endif

#ifdef FSP_FLAG
{
// @todo: Is allocation needed.

  //
  // Allocate PCI memory space.
  // First read memory map registers to calculate available MMIO resource
  //
  EFI_PHYSICAL_ADDRESS  RemapBase       = (UINT64) (MmioRead32 (McD0BaseAddress + R_SA_REMAPBASE) + LShiftU64 (MmioRead32 (McD0BaseAddress + R_SA_REMAPBASE + 4), 32)) & B_SA_REMAPBASE_REMAPBASE_MASK;
  EFI_PHYSICAL_ADDRESS  RemapLimit      = (UINT64) (MmioRead32 (McD0BaseAddress + R_SA_REMAPLIMIT) + LShiftU64 (MmioRead32 (McD0BaseAddress + R_SA_REMAPLIMIT + 4), 32)) & B_SA_REMAPLIMIT_REMAPLMT_MASK;
  EFI_PHYSICAL_ADDRESS  Tolud           = MmioRead32 (McD0BaseAddress + R_SA_TOLUD) & B_SA_TOLUD_TOLUD_MASK;
  EFI_PHYSICAL_ADDRESS  PciBaseAddress  = Tolud;
  EFI_PHYSICAL_ADDRESS  MeSegMask       = (UINT64) (MmioRead32 (McD0BaseAddress + R_SA_MESEG_MASK) + LShiftU64 (MmioRead32 (McD0BaseAddress + R_SA_MESEG_MASK + 4), 32));
  BOOLEAN               MeStolenEnable  = (BOOLEAN) ((MeSegMask & B_SA_MESEG_MASK_ME_STLEN_EN_MASK) != 0);
  UINT64                MeStolenSize;
  UINT64                Length;

  //
  // Check if memory remap is used
  //
  if ((RemapBase > RemapLimit) && (MeStolenEnable)) {
    MeStolenSize = MeSegMask & B_SA_MESEG_MASK_MEMASK_MASK;
    if (MeStolenSize != 0) {
      MeStolenSize = 0x8000000000L - MeStolenSize;
    }
    //
    // Remap is disabled -> PCI starts at TOLUD + ME Stolen size
    //
    PciBaseAddress += MeStolenSize;
  }

  Length = RES_MEM_LIMIT_1 - PciBaseAddress + 1;

  if (Length != 0) {
    DEBUG ((DEBUG_INFO, " Providing PCI space from 0x%X to 0x%X\n", (UINT32) PciBaseAddress, (UINT32)(PciBaseAddress + Length - 1)));
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_MAPPED_IO,
      (
        EFI_RESOURCE_ATTRIBUTE_PRESENT     |
        EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
        EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE
      ),
      PciBaseAddress,
      Length
    );
  }
}
#endif


#ifndef FSP_FLAG
  if(SysBootMode != BOOT_ON_S3_RESUME) {
    if ((Capsule != NULL) && (CapsuleBufferLength != 0)) {
      //
      // Call the Capsule PPI Coalesce function to coalesce the capsule data.
      //
      Status = Capsule->Coalesce ((EFI_PEI_SERVICES **)PeiServices, &CapsuleBuffer, &CapsuleBufferLength);
      //
      // If it failed, then NULL out our capsule PPI pointer so that the capsule
      // HOB does not get created below.
      //
      if (Status != EFI_SUCCESS) {
        Capsule = NULL;
      }
    }
  }
#endif

  DEBUG ((DEBUG_INFO, "Installed Memory = %llxh, PeiMemoryLength = %llxh\n", InstalledPeiMemoryBaseAddress, InstalledPeiMemoryLength));
  Status = PeiServicesInstallPeiMemory (InstalledPeiMemoryBaseAddress, InstalledPeiMemoryLength);
  ASSERT_EFI_ERROR (Status);

#ifndef FSP_FLAG
  if(SysBootMode != BOOT_ON_S3_RESUME) {
    //
    // If we found the capsule PPI (and we didn't have errors), then
    // call the capsule PEIM to allocate memory for the capsule.
    //
    if (Capsule != NULL) {
      Status = Capsule->CreateState ((EFI_PEI_SERVICES **) PeiServices, CapsuleBuffer, CapsuleBufferLength);
    }
  }
#endif


  //
  // Create SMRAM ACPI Variable Hob
  //
  CreateS3AcpiVariableHob ();
  return EFI_SUCCESS;
}

/**
  Determine the memory size desired based on HOB memory information.

  @param[in, out] Size        - The memory size to return.
**/
void
RetrieveRequiredMemorySize (
  IN OUT    UINTN             *Size
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_HOB_POINTERS        Hob;
  EFI_MEMORY_TYPE_INFORMATION *MemoryData;
  UINT8                       Index;
  UINTN                       TempPageNum;

  *Size      = 0;
  MemoryData = NULL;
  Status     = PeiServicesGetHobList ((VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
        CompareGuid (&Hob.Guid->Name, &gEfiMemoryTypeInformationGuid)) {
      MemoryData = (EFI_MEMORY_TYPE_INFORMATION *) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }
  //
  // Platform PEIM should supply the information. Generic PEIM doesn't assume any default value.
  //
  if (MemoryData == NULL) {
    return;
  }

  TempPageNum = 0;
  for (Index = 0; MemoryData[Index].Type != EfiMaxMemoryType; Index++) {
    //
    // Accumulate default memory size requirements
    //
    TempPageNum += MemoryData[Index].NumberOfPages;
  }

  if (TempPageNum == 0) {
    return;
  }
  //
  // 16 additional pages are used by DXE memory manager.
  //
  *Size = (TempPageNum + 16) * EFI_PAGE_SIZE;

  return;
}

/**
  Determine the Total DPR memory size needed based on the DPR directory in the SA Data HOB.

  @param[in, out] Size        - The memory size in MB to return.
**/
void
CalculateTotalDprMemorySize (
  IN OUT   UINT8             *Size
  )
{
  UINT8                DprEntryIndex;
  SA_CONFIG_HOB        *SaConfigHob;
  DPR_DIRECTORY_ENTRY  *DirectoryEntry;

  *Size           = 0;
  DprEntryIndex   = 0;
  DirectoryEntry  = NULL;
  SaConfigHob       = NULL;

  SaConfigHob = GetFirstGuidHob (&gSaConfigHobGuid);
  if (SaConfigHob != NULL) {
    DirectoryEntry = SaConfigHob->DprDirectory;
    while (DprEntryIndex < DPR_DIRECTORY_MAX) {
      *Size += DirectoryEntry->Size;
      DirectoryEntry++;
      DprEntryIndex++;
    }
  }
  return;
}

/**
  Calculates the bases for each technology consuming the DPR region
  and updates the SA Data HOB with the appropriate values in the Dpr
  directory

  @param[in] Base           - The memory base to return.
  @param[in] TotalDprSizeMB - The total DPR size in MB
**/
void
UpdateDprHobInfo (
  IN       EFI_PHYSICAL_ADDRESS  Base,
  IN       UINT8                 TotalDprSizeMB
  )
{
  UINT32               TopOfDpr;
  UINT8                DprEntryIndex;
  SA_CONFIG_HOB        *SaConfigHob;
  DPR_DIRECTORY_ENTRY  *DirectoryEntry;

  DprEntryIndex   = 0;
  DirectoryEntry  = NULL;
  SaConfigHob       = NULL;
  TopOfDpr        = (UINT32) Base + (UINT32) LShiftU64 (TotalDprSizeMB, 20);

  SaConfigHob = GetFirstGuidHob (&gSaConfigHobGuid);
  if (SaConfigHob != NULL) {
    DirectoryEntry = SaConfigHob->DprDirectory;
    while (DprEntryIndex < DPR_DIRECTORY_MAX) {
      switch (DirectoryEntry->Type) {
        case DPR_DIRECTORY_TYPE_TXT:
          DirectoryEntry->PhysBase = (UINT32) TopOfDpr - (UINT32) LShiftU64 (DirectoryEntry->Size, 20);
          break;
        case DPR_DIRECTORY_TYPE_BIOSGUARD:
          DirectoryEntry->PhysBase = (UINT32) Base;
          break;
        default:
          break;
      }
      DirectoryEntry++;
      DprEntryIndex++;
    }
  }
  return;
}

/**
  Determine the memory size desired by GDXC

  @param[in]      MrcData     - Mrc data structure
  @param[in, out] MotSize     - The MOT memory size
  @param[in, out] GdxcSize    - The GDXC memory size
**/
void
RetrieveGdxcMemorySize (
  IN MrcParameters  *CONST  MrcData,
  IN OUT   UINT64           *MotSize,
  IN OUT   UINT64           *GdxcSize
  )
{
  CONST MRC_FUNCTION                  *MrcCall;
  MrcOutput                           *Outputs;
  UINT32                              TempMotSize;
  UINT32                              Start;
  UINT32                              End;

  MrcCall = MrcData->Inputs.Call.Func;
  Outputs = &MrcData->Outputs;
  if (Outputs->Gdxc.GdxcEnable == 0) {
    *MotSize = 0;
    *GdxcSize = 0;
  } else {
    //
    // Determine Gdxc size: Includes MOT\PSMI\IOT (OCLA)
    //
    MrcGetGdxcMot (MrcData, (UINT32 *const) &Start, (UINT32 *const) &End);
    TempMotSize = End - Start;
    if (TempMotSize > 0) {
      *MotSize  = MrcCall->MrcLeftShift64 ((UINT64) (TempMotSize + 1), 23);
      *GdxcSize = *MotSize;
    }

    MrcGetGdxcOcla (MrcData, (UINT32 *const) &Start, (UINT32 *const) &End);
    *GdxcSize += MrcCall->MrcLeftShift64 ((UINT64) (End - Start), 23);

    // Add PSMI buffer area allocated between MOT and IOT if GDXC registers are already allocated space.
    if (*GdxcSize != 0) {
      *GdxcSize += (PSMI_SIZE_MB << 20);
    }
  }
  return;
}

/**
  This function returns the memory ranges to be enabled, along with information
  describing how the range should be used. The MemoryMap buffer will be filled in and
  NumRanges will contain the actual number of memory ranges that are to be enabled.

  @param[in]      MrcData     - Mrc data structure
  @param[in, out] MemoryMap   - Buffer to record details of the memory ranges to be enabled.
  @param[in, out] NumRanges   - On input, this contains the maximum number of memory ranges that
                                can be described in the MemoryMap buffer.

  @retval EFI_SUCCESS          - The function completed successfully.
  @retval EFI_BUFFER_TOO_SMALL - The specified number of ranges is too large.
**/
EFI_STATUS
GetMemoryMap (
  IN MrcParameters  *CONST                        MrcData,
  IN OUT   PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE  *MemoryMap,
  IN OUT   UINT8                                  *NumRanges
  )
{
  const MRC_FUNCTION      *MrcCall;
  const MrcInput          *Inputs;
  MrcOutput               *Outputs;
  MrcMemoryMap            *MemoryMapData;
  BOOLEAN                 EnableSmram;
  EFI_PHYSICAL_ADDRESS    MemorySize;
  EFI_STATUS              Status;
  UINT32                  SmramMask;
  UINT8                   EsmramcRegister;
  UINT8                   Index;
  EFI_PHYSICAL_ADDRESS    PrmrrEndAddress;
  BOOLEAN                 AvailableMemoryHole;
  EFI_PHYSICAL_ADDRESS    AvailableMemoryHoleEnd;
#ifdef PTT_FLAG
  UINT32                  TpmStsFtif;
#endif
  SI_PREMEM_POLICY_PPI    *SiPreMemPolicyPpi;
  MEMORY_CONFIGURATION    *MemConfig;

  if ((*NumRanges) < MAX_RANGES) {
    return EFI_BUFFER_TOO_SMALL;
  }

  MrcCall     = MrcData->Inputs.Call.Func;
  *NumRanges  = 0;
  AvailableMemoryHoleEnd = 0;

  Inputs    = &MrcData->Inputs;
  Outputs   = &MrcData->Outputs;
  MemoryMapData = &Outputs->MemoryMapData;

  //
  // Get platform memory range service
  //
  SmramMask = 0;
  SiPreMemPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  if (SiPreMemPolicyPpi != NULL) {
    MemConfig = NULL;
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMemoryConfigGuid, (VOID *) &MemConfig);
    if (MemConfig != NULL) {
      //
      // Find out which memory ranges to reserve on this platform
      //
      SmramMask = MemConfig->SmramMask;
    }
  }

  //
  // Generate memory ranges for the memory map.
  //
  EnableSmram     = FALSE;
  EsmramcRegister = 0;
  MemorySize      = 0;
  Index           = 0;

  //
  // Get the current "max usable memory" address from TOLUD because we will not
  // support any memory above 4Gig.  Will ignore the memory between 4G and TOUUD.
  //

  //
  // System is very unlikely to work with less than 32MB
  //
  ASSERT ((MemoryMapData->ToludBase << 20) >= (32 * 1024 * 1024));

  //
  // Add memory below 640KB to the memory map. Make sure memory between
  // 640KB and 1MB are reserved, even if not used for SMRAM
  //
  MemoryMap[*NumRanges].RowNumber       = Index;
  MemoryMap[*NumRanges].PhysicalAddress = ADDRESS_SPACE_START;
  MemoryMap[*NumRanges].CpuAddress      = ADDRESS_SPACE_START;
  MemoryMap[*NumRanges].RangeLength     = 0xA0000;
  MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
  (*NumRanges)++;

  //
  // Reserve ABSEG or HSEG SMRAM if needed
  //
  if (SmramMask & (PEI_MR_SMRAM_ABSEG_MASK | PEI_MR_SMRAM_HSEG_MASK)) {
    EnableSmram = TRUE;
    MemoryMap[*NumRanges].PhysicalAddress = MC_ABSEG_HSEG_PHYSICAL_START;
    MemoryMap[*NumRanges].RangeLength     = MC_ABSEG_HSEG_LENGTH;
    MemoryMap[*NumRanges].CpuAddress      = (SmramMask & PEI_MR_SMRAM_ABSEG_MASK) ?
      MC_ABSEG_CPU_START :
      MC_HSEG_CPU_START;
    //
    // Chipset only supports cacheable SMRAM.
    //
    MemoryMap[*NumRanges].Type = DualChannelDdrSmramNonCacheable;
  } else {
    //
    // Just mark this range reserved.
    //
    MemoryMap[*NumRanges].PhysicalAddress = MC_ABSEG_HSEG_PHYSICAL_START;
    MemoryMap[*NumRanges].CpuAddress      = MC_ABSEG_CPU_START;
    MemoryMap[*NumRanges].RangeLength     = 0x60000;
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
  }
  MemoryMap[*NumRanges].RowNumber = Index;
  (*NumRanges)++;

  //
  // Add remaining memory to the memory map.
  //
  MemoryMap[*NumRanges].RowNumber       = Index;
  MemoryMap[*NumRanges].PhysicalAddress = LOWER_MEMORY_START;
  MemoryMap[*NumRanges].CpuAddress      = LOWER_MEMORY_START;
  MemoryMap[*NumRanges].RangeLength     = MemoryMapData->LowestBase - LOWER_MEMORY_START;
  MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
  (*NumRanges)++;

  //
  // PTT
  //
#ifdef PTT_FLAG
  TpmStsFtif     = MrcCall->MrcMmioRead32 (R_PTT_TXT_STS_FTIF);
  if ((TpmStsFtif & V_FTIF_FTPM_PRESENT) == V_FTIF_FTPM_PRESENT) {
    MemoryMap[*NumRanges].RowNumber       = Index;
    MemoryMap[*NumRanges].PhysicalAddress = MemoryMapData->PttStolenBase;
    MemoryMap[*NumRanges].CpuAddress      = MemoryMapData->PttStolenBase;
    MemoryMap[*NumRanges].RangeLength     = 0x1000;  //4KB
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedWriteBackMemory;
    (*NumRanges) ++;
  }
#endif

  ///
  /// If GDXC supported and enabled
  ///
  if (Outputs->Gdxc.GdxcEnable) {

    //
    // GDXC IOT
    //
    if (MemoryMapData->GdxcIotBase < 0x1000) {  // Below the 4GB line.
      MemoryMap[*NumRanges].RowNumber       = Index;
      MemoryMap[*NumRanges].PhysicalAddress = (MemoryMapData->GdxcIotBase << 20);
      MemoryMap[*NumRanges].CpuAddress      = (MemoryMapData->GdxcIotBase << 20);
      MemoryMap[*NumRanges].RangeLength     = (MemoryMapData->GdxcIotSize << 20);
      MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
      (*NumRanges) ++;
    }

    //
    // GDXC MOT
    //
    if (MemoryMapData->GdxcMotBase < 0x1000) {  // Below the 4GB line.
      MemoryMap[*NumRanges].RowNumber       = Index;
      MemoryMap[*NumRanges].PhysicalAddress = (MemoryMapData->GdxcMotBase << 20);
      MemoryMap[*NumRanges].CpuAddress      = (MemoryMapData->GdxcMotBase << 20);
      MemoryMap[*NumRanges].RangeLength     = (MemoryMapData->GdxcMotSize << 20);
      MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
      (*NumRanges) ++;
    }
  }

  //
  // PRMRR
  //
  if (Inputs->PrmrrSize > 0) {
    MemoryMap[*NumRanges].RowNumber       = Index;
    MemoryMap[*NumRanges].PhysicalAddress = (MemoryMapData->PrmrrBase << 20);
    MemoryMap[*NumRanges].CpuAddress      = (MemoryMapData->PrmrrBase << 20);
    MemoryMap[*NumRanges].RangeLength     = (Inputs->PrmrrSize << 20);
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedWriteBackMemory;

    PrmrrEndAddress = MemoryMap[*NumRanges].CpuAddress + MemoryMap[*NumRanges].RangeLength;
    (*NumRanges) ++;

  } else {
    PrmrrEndAddress = 0;
  }

  //
  // DPR
  //
  if (Inputs->DprSize > 0) {
    MemoryMap[*NumRanges].RowNumber       = Index;
    MemoryMap[*NumRanges].PhysicalAddress = ((MemoryMapData->TsegBase - Inputs->DprSize) << 20);
    MemoryMap[*NumRanges].CpuAddress      = ((MemoryMapData->TsegBase - Inputs->DprSize) << 20);
    MemoryMap[*NumRanges].RangeLength     = (Inputs->DprSize << 20);
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;

    UpdateDprHobInfo (MemoryMap[*NumRanges].PhysicalAddress, (UINT8) Inputs->DprSize);

    AvailableMemoryHole = (PrmrrEndAddress != 0) && (PrmrrEndAddress != MemoryMap[*NumRanges].CpuAddress);
    AvailableMemoryHoleEnd = MemoryMap[*NumRanges].CpuAddress;
    (*NumRanges) ++;

    if (AvailableMemoryHole) {
      //
      // Add unused memory between PRMRR and DPR to the memory map.
      //
      MemoryMap[*NumRanges].RowNumber       = Index;
      MemoryMap[*NumRanges].PhysicalAddress = ((MemoryMapData->PrmrrBase + Inputs->PrmrrSize) << 20);
      MemoryMap[*NumRanges].CpuAddress      = ((MemoryMapData->PrmrrBase + Inputs->PrmrrSize) << 20);
      MemoryMap[*NumRanges].RangeLength     = AvailableMemoryHoleEnd - PrmrrEndAddress;
      MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
      (*NumRanges) ++;
    }

  }

  //
  // TSEG
  //
  if (Inputs->TsegSize > 0) {
    MemoryMap[*NumRanges].RowNumber       = Index;
    MemoryMap[*NumRanges].PhysicalAddress = (MemoryMapData->TsegBase << 20);
    MemoryMap[*NumRanges].CpuAddress      = (MemoryMapData->TsegBase << 20);
    MemoryMap[*NumRanges].RangeLength     = (Inputs->TsegSize << 20);
    MemoryMap[*NumRanges].Type            = DualChannelDdrSmramCacheable;
    (*NumRanges) ++;
    //
    // Enable SMRAM
    //
    MmioOr8 (MmPciBase (SA_MC_BUS, 0, 0) + R_SA_SMRAMC, B_SA_SMRAMC_G_SMRAME_MASK);
  }

  //
  // BGSM (GFX GTT)
  //
  if (Outputs->GraphicsGttSize > 0) {
    MemoryMap[*NumRanges].RowNumber       = Index;
    MemoryMap[*NumRanges].PhysicalAddress = (MemoryMapData->GttBase << 20);
    MemoryMap[*NumRanges].CpuAddress      = (MemoryMapData->GttBase << 20);
    MemoryMap[*NumRanges].RangeLength     = (Outputs->GraphicsGttSize << 20);
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
    (*NumRanges) ++;
  }

  //
  // BDSM (GFX UMA)
  //
  if (Outputs->GraphicsStolenSize > 0) {
    MemoryMap[*NumRanges].RowNumber       = Index;
    MemoryMap[*NumRanges].PhysicalAddress = (MemoryMapData->BdsmBase << 20);
    MemoryMap[*NumRanges].CpuAddress      = (MemoryMapData->BdsmBase << 20);
    MemoryMap[*NumRanges].RangeLength     = (Outputs->GraphicsStolenSize << 20);
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
    (*NumRanges) ++;
  }

  return EFI_SUCCESS;
}

/**
  This function returns a pointer to the allocated hand off buffer.

  @param[in, out] Hob         - A pointer to where to store the pointer to the allocated data buffer.
  @param[in]      BlockSize   - The size of the buffer to get.
  @param[in]      Guid        - The GUID to assign to the HOB.

  @retval EFI_SUCCESS - Hob is successfully built.
  @retval Others      - Error occured while creating the Hob.
**/
EFI_STATUS
MrcGetHobForDataStorage (
  IN OUT VOID                     **Hob,
  IN     UINT16                   BlockSize,
  IN     EFI_GUID                 *Guid
  )
{
  EFI_HOB_GUID_TYPE *Buffer;
  EFI_STATUS        Status;

  Status = PeiServicesCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, BlockSize, Hob);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Buffer = (EFI_HOB_GUID_TYPE *) *Hob;
  CopyMem (&(Buffer->Name), Guid, sizeof (EFI_GUID));
  ZeroMem ((VOID *) ((UINT32) Buffer + sizeof (EFI_HOB_GUID_TYPE)), BlockSize - sizeof (EFI_HOB_GUID_TYPE));
  return EFI_SUCCESS;
}

#ifdef BDAT_SUPPORT
/**
  Finds the BDAT Schema List HOB if it exists.  If it does not exist, creates it

  @param[in]  Inputs            - Pointer to the MRC Input data structure
  @param[out] BdatSchemaListHob - Pointer to Schema List Hob, by reference

  @retval EFI_SUCCESS - Hob was successfully found or built.
  @retval Others      - Error occured while creating or finding the Hob.
**/
EFI_STATUS
MrcGetBdatSchemaListHob (
  OUT      MRC_BDAT_SCHEMA_LIST_HOB  **BdatSchemaListHob
  )
{
  EFI_STATUS               Status;
  MRC_BDAT_SCHEMA_LIST_HOB *Buffer;

  Status = EFI_SUCCESS;
  Buffer = (MRC_BDAT_SCHEMA_LIST_HOB *) GetFirstGuidHob (&gMrcSchemaListHobGuid);

  if (Buffer != NULL) {
    DEBUG ((DEBUG_INFO, "BDAT Schema List HOB already exists\n"));
    *BdatSchemaListHob = Buffer;
  } else {
    DEBUG ((DEBUG_INFO, "Creating BDAT Schema List HOB: "));
    Status = MrcGetHobForDataStorage (
               (VOID **) &Buffer,
               sizeof (MRC_BDAT_SCHEMA_LIST_HOB),
               (EFI_GUID*)&gMrcSchemaListHobGuid
               );
    DEBUG ((DEBUG_INFO, "%r\n", Status));
    if (!EFI_ERROR (Status)) {
      *BdatSchemaListHob = Buffer;
    }
  }

  return Status;
}
#endif


/**
  Determines whether or not the platform has executed a TXT launch by
  examining the TPM Establishment bit.

  @param[in] VOID

  @retval TRUE        - If the TPM establishment bit is asserted.
  @retval FALSE       - If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsEstablishmentBitAsserted (
  VOID
  )
{
  UINT8             Access;
  UINT16            TimeOutCount;


  //
  // Set TPM.ACCESS polling timeout about 750ms.
  //
  TimeOutCount = TPM_TIME_OUT;
  do {
    //
    // Read TPM status register
    //

    Access = MmioRead8 (TPM_STATUS_REG_ADDRESS);

    //
    // if TPM.Access == 0xFF, TPM is not present.
    //
    if (Access == 0xFF) {
      return FALSE;
    }
    //
    // Check tpmRegValidSts bit before checking establishment bit.
    //
    if ((Access & 0x80) == 0x80) {
      //
      // tpmRegValidSts set, we can check establishment bit now.
      //
      break;
    } else {
      //
      // Delay 1ms
      //
      MicroSecondDelay (1000);
    }

    TimeOutCount--;
  } while (TimeOutCount != 0);

  //
  // ValidSts is not set.
  //
  if ((Access & 0x80) != 0x80) {
    return FALSE;
  }
  //
  // The bit we're interested in uses negative logic:
  // If bit 0 == 1 then return False,
  // Else return True.
  //
  return (BOOLEAN) ((Access & 0x1) ? FALSE : TRUE);
}

/**
  Unlock memory when security is set and TxT is not enabled.

  @param[in] MrcData     - Mrc global data.
**/
void
UnlockMemory (
  IN CONST MrcParameters    *CONST  MrcData
  )
{
  EFI_CPUID_REGISTER Reg;
  UINT32             Data32;
  const MrcInput     *Inputs;
  MRC_FUNCTION       *MrcCall;
  UINT64             LtLockSts;

  Inputs = &MrcData->Inputs;
  MrcCall = Inputs->Call.Func;

  Data32 = 0;

  AsmCpuid (
    CPUID_VERSION_INFO,
    &Reg.RegEax,
    &Reg.RegEbx,
    &Reg.RegEcx,
    &Reg.RegEdx
    );
  if ((Reg.RegEcx & BIT6)) {
    DEBUG ((DEBUG_INFO, "Processor supports TXT\n"));

    Data32 = CheckSmxCapabilities ();

    if (Data32 & BIT0) {
      DEBUG ((DEBUG_INFO, "Platform / PCH supports TXT\n"));
      LtLockSts = MrcCall->MrcReadMsr64 (MSR_LT_LOCK_MEMORY);
      if ((LtLockSts & (B_MSR_LT_LOCK_MEMORY_MEM_LOCK_DEV | B_MSR_LT_LOCK_MEMORY_MEM_LOCK_CPU)) == 0) {
        return;
      }

      if (!(IsEstablishmentBitAsserted ())) {
        DEBUG ((DEBUG_INFO, "Unlock memory\n"));
        MrcCall->MrcWriteMsr64 (MSR_LT_UNLOCK_MEMORY, 0);
      }
    } else {
      DEBUG ((DEBUG_INFO, "Platform / PCH does not support TxT\n"));
    }
  } else {
    DEBUG ((DEBUG_INFO, "Processor does not support TxT\n"));
  }
}

/**
  This function calls into TxT ACHECK if platform is TxT enabled.
**/
void
MrcTxtAcheck (
  VOID
  )
{
  EFI_STATUS              Status;

  Status = AliasCheck ();
}

/**
  Determine whether a cold reset of the platform is required.
  Note that the memory configuration saved data must be valid.

  @param[in] MrcData             - The MRC "global data" area.
  @param[in] MemConfig           - MEMORY_CONFIGURATION structure.
  @param[in] MrcBootMode         - Current MRC boot mode.

  @retval TRUE if cold reset is required, otherwise returns FALSE.
**/
BOOLEAN
ColdBootRequired (
  IN const MrcParameters *const MrcData,
  IN MEMORY_CONFIGURATION       *MemConfig,
  IN MrcBootMode                MrcBootMode
  )
{
  const MrcInput       *Inputs;
  const MrcSaveData    *SaveData;
  MrcVersion           Version;
  UINT32               CurrentCrc;

  Inputs    = &MrcData->Inputs;
  SaveData  = &MrcData->Save.Data;

  MrcVersionGet (MrcData, &Version);
  if ((Version.Major != SaveData->Version.Major) ||
      (Version.Minor != SaveData->Version.Minor) ||
      (Version.Rev   != SaveData->Version.Rev)   ||
      (Version.Build != SaveData->Version.Build)) {
    DEBUG ((
             DEBUG_INFO,
             "MRC change detected, prev. ver. %d.%d.%d.%d, curr. ver. %d.%d.%d.%d\n",
             SaveData->Version.Major,
             SaveData->Version.Minor,
             SaveData->Version.Rev,
             SaveData->Version.Build,
             Version.Major,
             Version.Minor,
             Version.Rev,
             Version.Build
             ));
    return TRUE;
  }
  if ((Inputs->CpuModel != SaveData->CpuModel) || (Inputs->CpuStepping != SaveData->CpuStepping)) {
    DEBUG ((
             DEBUG_INFO,
             "CPU change detected, prev. CPU %x.%x, curr. CPU %x.%x\n",
             SaveData->CpuModel,
             SaveData->CpuStepping,
             Inputs->CpuModel,
             Inputs->CpuStepping
             ));
    return TRUE;
  }
  if (MrcBootMode != bmS3) {
    CurrentCrc   = MrcCalculateCrc32 ((UINT8 *) MemConfig, sizeof (MEMORY_CONFIGURATION));
    if (CurrentCrc != SaveData->SaMemCfgCrc) {
      DEBUG ((
               DEBUG_INFO,
               "System Agent input parameter change detected, prev. CRC %xh, curr. CRC %xh.\n",
               SaveData->SaMemCfgCrc,
               CurrentCrc
               ));

      CurrentCrc = MrcCalculateCrc32 ((UINT8 *) MemConfig, sizeof (MEMORY_CONFIGURATION));
      if (CurrentCrc == SaveData->SaMemCfgCrc) {
        DEBUG ((DEBUG_INFO, "Only SerialDebug option changed, no need to retrain\n"));
        return FALSE;
      }
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Set up the MRC OEM data structure.

  @param[in, out] MrcData             - Pointer to the MRC global data structure.
  @param[in]      MemConfig           - MEMORY_CONFIGURATION structure.
  @param[in]      MemConfigNoCrc      - MEMORY_CONFIG_NO_CRC  structure.
**/
void
MrcSetupOem (
  IN OUT MrcParameters          *CONST MrcData,
  IN    MEMORY_CONFIGURATION    *MemConfig,
  IN    MEMORY_CONFIG_NO_CRC    *MemConfigNoCrc
  )
{
  SA_FUNCTION_CALLS    *SaCall;
  SA_MEMORY_FUNCTIONS  *MemCall;
  MrcInput             *Inputs;
  MRC_FUNCTION         *MrcCall;

  SaCall         = &MemConfigNoCrc->SaCall;
  MemCall        = &MemConfigNoCrc->MrcCall;
  Inputs         = &MrcData->Inputs;
  MrcCall        = Inputs->Call.Func;

  MrcGetCoreFunction (
    MrcData,
    (UINT32 *) &MemCall->GetMcAddressDecode,
    (UINT32 *) &MemCall->GetMcAddressEncode,
    (UINT32 *) &MemCall->MrcChannelExist,
    (UINT32 *) &MemCall->MrcPrintf,
    (UINT32 *) &MemCall->MrcChangeMargin,
    (UINT32 *) &MemCall->MrcSignExtend,
    (UINT32 *) &MemCall->ShiftPiCommandTrain,
    (UINT32 *) &MemCall->UpdateVrefWaitStable,
    (UINT32 *) &MemCall->MrcThermalOverrides
    );


  Inputs->PciIndex               = MemConfig->PciIndex;
  Inputs->PciData                = MemConfig->PciData;
  Inputs->CpuidModel             = GetCpuFamily ();
  Inputs->CpuidStepping          = GetCpuStepping ();

#if 1
  ((MRC_MEMORY_COPY) (SaCall->CopyMem)) ((UINT8 *) MrcCall, (UINT8 *) SaCall, sizeof (SA_FUNCTION_CALLS));
  MrcCall->MrcSetLockPrmrr         = (MRC_SET_LOCK_PRMRR) (&SetUncorePrmrr);
#else
  MrcCall->MrcIoRead8              = (MRC_IO_READ_8) (SaCall->IoRead8);
  MrcCall->MrcIoRead16             = (MRC_IO_READ_16) (SaCall->IoRead16);
  MrcCall->MrcIoRead32             = (MRC_IO_READ_32) (SaCall->IoRead32);
  MrcCall->MrcIoWrite8             = (MRC_IO_WRITE_8) (SaCall->IoWrite8);
  MrcCall->MrcIoWrite16            = (MRC_IO_WRITE_16) (SaCall->IoWrite16);
  MrcCall->MrcIoWrite32            = (MRC_IO_WRITE_32) (SaCall->IoWrite32);
  MrcCall->MrcMmioRead8            = (MRC_MMIO_READ_8) (SaCall->MmioRead8);
  MrcCall->MrcMmioRead16           = (MRC_MMIO_READ_16) (SaCall->MmioRead16);
  MrcCall->MrcMmioRead32           = (MRC_MMIO_READ_32) (SaCall->MmioRead32);
  MrcCall->MrcMmioRead64           = (MRC_MMIO_READ_64) (SaCall->MmioRead64);
  MrcCall->MrcMmioWrite8           = (MRC_MMIO_WRITE_8) (SaCall->MmioWrite8);
  MrcCall->MrcMmioWrite16          = (MRC_MMIO_WRITE_16) (SaCall->MmioWrite16);
  MrcCall->MrcMmioWrite32          = (MRC_MMIO_WRITE_32) (SaCall->MmioWrite32);
  MrcCall->MrcMmioWrite64          = (MRC_MMIO_WRITE_64) (SaCall->MmioWrite64);
  MrcCall->MrcSmbusRead8           = (MRC_SMBUS_READ_8) (SaCall->SmbusRead8);
  MrcCall->MrcSmbusRead16          = (MRC_SMBUS_READ_16) (SaCall->SmbusRead16);
  MrcCall->MrcSmbusWrite8          = (MRC_SMBUS_WRITE_8) (SaCall->SmbusWrite8);
  MrcCall->MrcSmbusWrite16         = (MRC_SMBUS_WRITE_16) (SaCall->SmbusWrite16);
  MrcCall->MrcGetPciDeviceAddress  = (MRC_GET_PCI_DEVICE_ADDRESS) (SaCall->GetPciDeviceAddress);
  MrcCall->MrcGetPcieDeviceAddress = (MRC_GET_PCIE_DEVICE_ADDRESS) (SaCall->GetPcieDeviceAddress);
  MrcCall->MrcGetRtcTime           = (MRC_GET_RTC_TIME) (SaCall->GetRtcTime);
  MrcCall->MrcGetCpuTime           = (MRC_GET_CPU_TIME) (SaCall->GetCpuTime);
  MrcCall->MrcCopyMem              = (MRC_MEMORY_COPY) (SaCall->CopyMem);
  MrcCall->MrcSetMem               = (MRC_MEMORY_SET_BYTE) (SaCall->SetMem);
  MrcCall->MrcSetMemWord           = (MRC_MEMORY_SET_WORD) (SaCall->SetMemWord);
  MrcCall->MrcSetMemDword          = (MRC_MEMORY_SET_DWORD) (SaCall->SetMemDword);
  MrcCall->MrcLeftShift64          = (MRC_LEFT_SHIFT_64) (SaCall->LeftShift64);
  MrcCall->MrcRightShift64         = (MRC_RIGHT_SHIFT_64) (SaCall->RightShift64);
  MrcCall->MrcMultU64x32           = (MRC_MULT_U64_U32) (SaCall->MultU64x32);
  MrcCall->MrcDivU64x64            = (MRC_DIV_U64_U64) (SaCall->DivU64x64);
  MrcCall->MrcGetSpdData           = (MRC_GET_SPD_DATA) (SaCall->GetSpdData);
  MrcCall->MrcGetRandomNumber      = (MRC_GET_RANDOM_NUMBER) (SaCall->GetRandomNumber);
  MrcCall->MrcCpuMailboxRead       = (MRC_CPU_MAILBOX_READ) (SaCall->CpuMailboxRead);
  MrcCall->MrcCpuMailboxWrite      = (MRC_CPU_MAILBOX_WRITE) (SaCall->CpuMailboxWrite);
  MrcCall->MrcGetMemoryVdd         = (MRC_GET_MEMORY_VDD) (SaCall->GetMemoryVdd);
  MrcCall->MrcSetMemoryVdd         = (MRC_SET_MEMORY_VDD) (SaCall->SetMemoryVdd);
  MrcCall->MrcCheckpoint           = (MRC_CHECKPOINT) (SaCall->CheckPoint);
  MrcCall->MrcDebugHook            = (MRC_DEBUG_HOOK) (SaCall->DebugHook);
  MrcCall->MrcPrintString          = (MRC_PRINT_STRING) (SaCall->DebugPrint);
  MrcCall->MrcMsrRead64            = (MRC_MSR_READ_64) (SaCall->ReadMsr64);
  MrcCall->MrcMsrWrite64           = (MRC_MSR_WRITE_64) (SaCall->WriteMsr64);
#endif

  MrcCall->MrcTxtAcheck            = (MRC_TXT_ACHECK) MrcTxtAcheck;

  return;
}

/**
  Set up the MRC input data structure.

  @param[in]  SysBootMode         - Boot mode of the system.
  @param[in]  BootMode            - Boot mode of the MRC.
  @param[in]  MrcData             - Pointer to the MRC global data structure
  @param[in]  MiscPeiPreMemConfig - SA_MISC_PEI_PREMEM_CONFIG structure.
  @param[in]  GtConfig            - GRAPHICS_PEI_PREMEM_CONFIG structure.
  @param[in]  MemConfig           - MEMORY_CONFIGURATION structure.
  @param[in]  MemConfigNoCrc      - MEMORY_CONFIG_NO_CRC structure.
  @param[in]  SiPreMemPolicyPpi   - The SI Pre-Mem Policy PPI instance.
  @param[in]  CpuModel            - CPU Type.

  @retval  MrcBootMode
**/
MrcBootMode
MrcSetupMrcData (
  IN CONST EFI_BOOT_MODE              SysBootMode,
  IN CONST MrcBootMode                BootMode,
  OUT      MrcParameters              *CONST MrcData,
  IN       SA_MISC_PEI_PREMEM_CONFIG  *MiscPeiPreMemConfig,
  IN       GRAPHICS_PEI_PREMEM_CONFIG *GtConfig,
  IN       MEMORY_CONFIGURATION       *CONST MemConfig,
  IN       MEMORY_CONFIG_NO_CRC       *CONST MemConfigNoCrc,
  IN       SI_PREMEM_POLICY_PPI        *CONST SiPreMemPolicyPpi,
  IN       CPU_FAMILY                 CpuModel
  )
{
  MrcInput                    *Inputs;
  MRC_FUNCTION                *MrcCall;
  MrcControllerIn             *ControllerIn;
  MrcChannelIn                *ChannelIn;
  MrcDimmSts                  Status;
  UINT8                       Dimm;
  MrcDimmIn                   *DimmIn;
  UINT8                       Index;
  BOOLEAN                     UlxUlt;
  UINT8                       Controller;
  UINT8                       Channel;
  UINT32                      DmiBar;
  PCIE_CR_OPIO_RX_DLL_GLOBAL3_STRUCT  RxDllGlobal3;
  CPU_SECURITY_PREMEM_CONFIG         *CpuSecurityPreMemConfig;
  OVERCLOCKING_PREMEM_CONFIG         *OcPreMemConfig;

  Inputs         = &MrcData->Inputs;
  MrcCall        = Inputs->Call.Func;

  Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gCpuSecurityPreMemConfigGuid, (VOID *) &CpuSecurityPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  Status  = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaOverclockingPreMemConfigGuid, (VOID *) &OcPreMemConfig);
  ASSERT_EFI_ERROR (Status);
  // Synchronize the MRC input policies to the SA MEMORY_CONFIGURATION structure.
  //
  MrcCall->MrcCopyMem ((UINT8 *) Inputs, (UINT8 *) MemConfig, sizeof (MEMORY_CONFIGURATION));

  Inputs->SaMemCfgAddress = (UINT32) MemConfig;
  Inputs->SaMemCfgSize    = sizeof (MEMORY_CONFIGURATION);
  Inputs->OcSupport       = OcPreMemConfig->OcSupport;
  Status                  = DIMM_NOT_PRESENT;
  UlxUlt                  = ((Inputs->CpuModel == cmSKL_ULX_ULT) || (Inputs->CpuModel == cmKBL_ULX_ULT));

  //
  // Force standard profile when system boot mode indicates. Usually means some has cleared CMOS.
  //
  if (SysBootMode == BOOT_WITH_DEFAULT_SETTINGS) {
    Inputs->MemoryProfile  = STD_PROFILE;
    Inputs->RefClk         = 0;
    Inputs->Ratio          = 0;
    Inputs->VddVoltage     = VDD_INVALID;
  }

  //
  // Setup the base addresses.
  //
  Inputs->MchBarBaseAddress = MiscPeiPreMemConfig->MchBar;
  Inputs->PciEBaseAddress   = (UINT32) PcdGet64 (PcdPciExpressBaseAddress);
  Inputs->SmbusBaseAddress  = MiscPeiPreMemConfig->SmbusBar;
  Inputs->GdxcBaseAddress   = MiscPeiPreMemConfig->GdxcBar;
  Inputs->HpetBaseAddress   = 0xFED00000;

  //
  // MMIO size in MB units (below 4GB)
  //
  Inputs->MmioSize = MiscPeiPreMemConfig->MmioSize;

  //
  // DDR maximum frequency
  //
  Inputs->FreqMax = MemConfig->DdrFreqLimit;

  /**
    TSEG Size in MB units
    Since IED memory is part of TSEG, re-calculate TSEG size with IED size.
    Also TSEG size must be power of 2.
    @note When re-calculating we do not enlarge TSEG to next power of 2 size because the minimal TSEG size is recommended.
    For example: original TsegSize = 16MB, IedSize = 4MB, we will still give 16MB to MRC after recalculation.
  **/
  Inputs->TsegSize = GetPowerOfTwo32 (MiscPeiPreMemConfig->TsegSize + MiscPeiPreMemConfig->IedSize) >> 20;

  //
  // Prmrr size is in MB units
  //
  Inputs->PrmrrSize = (CpuSecurityPreMemConfig->PrmrrSize) >> 20;

  //
  // Graphics Stolen Size
  // Graphics Stolen size above 64MB has a granularity of 32MB increments
  // GMS values below 240 correspond to Sizes 32 * GSMValue
  // Graphics Stolen size below 64MB has a higher granularity and can be set in 4MB increments
  // GMS values ranging from 240-254 correspond to sizes 4MB to 60MB (excluding 32MB) which is 4*(GSMValue-239)
  //
  Inputs->GraphicsGttSize            = (1 << GtConfig->GttSize);

  if (GtConfig->IgdDvmt50PreAlloc < 240) {
    Inputs->GraphicsStolenSize           = 32 * GtConfig->IgdDvmt50PreAlloc;
  } else {
    Inputs->GraphicsStolenSize           = 4 * (GtConfig->IgdDvmt50PreAlloc - 239);
  }
  Inputs->GfxIsVersatileAcceleration = FALSE;

  //
  //Get RTC time
  //
  MrcCall->MrcGetRtcTime (&(Inputs->BaseTime.Seconds),&(Inputs->BaseTime.Minutes),
             &(Inputs->BaseTime.Hours), &(Inputs->BaseTime.DayOfMonth),
             &(Inputs->BaseTime.Month), &(Inputs->BaseTime.Year));
  DEBUG ((
           DEBUG_INFO,
           "RTC %d/%d/%d %d:%d:%d\n",
           Inputs->BaseTime.Month,
           Inputs->BaseTime.DayOfMonth,
           Inputs->BaseTime.Year,
           Inputs->BaseTime.Hours,
           Inputs->BaseTime.Minutes,
           Inputs->BaseTime.Seconds
           ));

  //
  // Get BoardType (see MrcBoardType enum)
  //
  Inputs->BoardType = MiscPeiPreMemConfig->UserBd;

  //
  // Get VccIO value:
  // SKL ULT/ULX: 0.85v/0.95v, according to PCIE_CR_OPIO_RX_DLL_GLOBAL3[31:30]
  // SKL DT/Halo: 0.95v always
  //
  Inputs->VccIomV = 950;
  if (UlxUlt) {
    DmiBar = (MmioRead32 (Inputs->PciEBaseAddress + UNCORE_CR_DMIBAR_0_0_0_PCI_REG)) & ~BIT0;
    RxDllGlobal3.Data = MmioRead32 (DmiBar + PCIE_CR_OPIO_RX_DLL_GLOBAL3_REG);
    if (RxDllGlobal3.Bits.select_vccio_level == 0) {
      Inputs->VccIomV = 850;
    }
  }

  Inputs->SetRxDqs32        = FALSE;

  //
  // MemConfig->ProbelessTrace: 0 - Disabled, 1 - GDXC IOT/MOT, 2 - HD Port
  //
  Inputs->Gdxc.GdxcEnable          = (MemConfig->ProbelessTrace == 1) ? 1 : 0;

  //
  // Options for training steps
  //
  Inputs->TrainingEnables.WRDSEQT     = (UlxUlt) ? 0 : 1;
  Inputs->TrainingEnables.WRDSUDT     = 0;
  DEBUG_CODE_BEGIN ();
  Inputs->TrainingEnables.RMT         = 1;      // Enable RMT in debug build regardless of the setup option
  Inputs->RmtPerTask                  = FALSE;  // Run RMT after major training steps
  DEBUG_CODE_END ();

  Inputs->BClkFrequency = (MemConfig->BClkFrequency < (BCLK_DEFAULT - (10 * 1000 * 1000))) ?
    BCLK_DEFAULT : ((MemConfig->BClkFrequency / 1000000) * 1000000);

  Inputs->IedSize             = (MiscPeiPreMemConfig->IedSize) >> 20;

  //
  // Options for Thermal settings
  //
  Inputs->LockPTMregs                        = (MiscPeiPreMemConfig->LockPTMregs == 0) ? 0 : 1;

  Inputs->EnVttOdt        = UlxUlt;
  Inputs->VttCompForVsshi = 1;
  Inputs->MrcMode         = MrcModeFull;

  Inputs->PowerTrainingMode = (UlxUlt) ? 0 : 1; // 0 - power training; 1 - margin training.

  // RMT BDAT support.
  Inputs->BdatEnable = (BOOLEAN) MiscPeiPreMemConfig->BdatEnable;
  Inputs->BdatTestType      = MiscPeiPreMemConfig->BdatTestType;
  Inputs->BdatSchema        = MiscPeiPreMemConfig->BdatSchema;
  //
  // DRAM ODT
  //
  Inputs->LpddrDramOdt = (UlxUlt) ? 1 : 0;
  Inputs->Ddr3DramOdt  = 1;
  Inputs->Ddr4DramOdt  = 1;

#ifdef FSP_FLAG
#ifdef SSA_FLAG
  //
  // MMA Loader Test Config and Content pointers and size, used for FSP.
  //
  Inputs->MmaTestConfigPtr   = MemConfigNoCrc->MmaTestConfigPtr;
  Inputs->MmaTestConfigSize  = MemConfigNoCrc->MmaTestConfigSize;
  Inputs->MmaTestContentPtr  = MemConfigNoCrc->MmaTestContentPtr;
  Inputs->MmaTestContentSize = MemConfigNoCrc->MmaTestContentSize;
#endif // SSA_FLAG
#endif // FSP_FLAG

  Inputs->CleanMemory = MemConfigNoCrc->CleanMemory;
  Inputs->SerialDebugLevel = MemConfigNoCrc->SerialDebugLevel;

  //
  // SA GV is only used on ULX/ULT.
  //
  Inputs->SaGv = UlxUlt ? MemConfig->SaGv : 0;

  //
  // Initialize the board-specific RCOMP values
  //
  MrcCall->MrcCopyMem ((UINT8 *) Inputs->RcompResistor, (UINT8 *) MemConfigNoCrc->RcompData->RcompResistor, sizeof (Inputs->RcompResistor));
  MrcCall->MrcCopyMem ((UINT8 *) Inputs->RcompTarget,   (UINT8 *) MemConfigNoCrc->RcompData->RcompTarget,   sizeof (Inputs->RcompTarget));
  if (Inputs->RcompResistor[0] == 0) {
    DEBUG ((DEBUG_ERROR, "ERROR: RCOMP resistor input cannot be zero!\n"));
    ASSERT (FALSE);
  }
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn = &Inputs->Controller[Controller];
    ControllerIn->ChannelCount = 0;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelIn = &ControllerIn->Channel[Channel];
      {
        //
        // Decide which channels and DIMMs are enabled.
        //
        switch (MemConfig->DisableDimmChannel[Channel]) {
          case 1:
            ChannelIn->Dimm[0].Status = DIMM_DISABLED;
            ChannelIn->Dimm[1].Status = DIMM_ENABLED;
            ChannelIn->Status         = CHANNEL_PRESENT;
            ControllerIn->ChannelCount++;
            ChannelIn->DimmCount      = 1;
            break;
          case 2:
            ChannelIn->Dimm[0].Status = DIMM_ENABLED;
            ChannelIn->Dimm[1].Status = DIMM_DISABLED;
            ChannelIn->Status         = CHANNEL_PRESENT;
            ControllerIn->ChannelCount++;
            ChannelIn->DimmCount      = 1;
            break;
          case 3:
            ChannelIn->Dimm[0].Status = DIMM_DISABLED;
            ChannelIn->Dimm[1].Status = DIMM_DISABLED;
            ChannelIn->Status         = CHANNEL_DISABLED;
            ChannelIn->DimmCount      = 0;
            break;
          default:
            ChannelIn->Dimm[0].Status = DIMM_ENABLED;
            ChannelIn->Dimm[1].Status = DIMM_ENABLED;
            ChannelIn->Status         = CHANNEL_PRESENT;
            ControllerIn->ChannelCount++;
            ChannelIn->DimmCount      = 2;
            break;
        }
      }
      //
      // DQ byte mapping to CMD/CTL/CLK
      //
      MrcCall->MrcCopyMem ((UINT8 *) ChannelIn->DQByteMap, (UINT8 *) MemConfigNoCrc->DqByteMap->DqByteMap[Channel], sizeof (ChannelIn->DQByteMap));
      MrcCall->MrcCopyMem ((UINT8 *) ChannelIn->DqsMapCpu2Dram, (UINT8 *) MemConfigNoCrc->DqsMap->DqsMapCpu2Dram[Channel], sizeof (ChannelIn->DqsMapCpu2Dram));

      //
      // Get DIMM SpdBaseAddresses.
      //
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        DimmIn = &ChannelIn->Dimm[Dimm];
        Index  = (Channel * MAX_DIMMS_IN_CHANNEL) + Dimm;
        DimmIn->SpdAddress = MiscPeiPreMemConfig->SpdAddressTable[Index];
        MrcCall->MrcCopyMem ((UINT8 *) &DimmIn->Spd.MrcSpdString[0], (UINT8 *) MrcSpdStringConst, sizeof (DimmIn->Spd.MrcSpdString));
        ZeroMem (&DimmIn->Spd.Data, sizeof (MrcSpd));
        DimmIn->Spd.Flag.Bit.DimmNumber    = Dimm;
        DimmIn->Spd.Flag.Bit.ChannelNumber = Channel;
        DimmIn->Spd.Flag.Bit.MdSocket      = (DimmIn->SpdAddress > 0) ? 1 : 0;
        DEBUG ((DEBUG_INFO, "Spd Address for Channel %d Dimm %d: %x\n", Channel, Dimm, DimmIn->SpdAddress));
        if ((DIMM_ENABLED == DimmIn->Status) || (DIMM_DISABLED == DimmIn->Status)) {
          if (DimmIn->SpdAddress > 0) {
            if ((bmCold == BootMode) || (bmFast == BootMode)) {
              MrcCall->MrcGetSpdData (
                         BootMode,
                         DimmIn->SpdAddress,
                         (UINT8 *) &DimmIn->Spd.Data,
                         (UINT8 *) &SpdDdr3Table,
                         sizeof (SpdDdr3Table),
                         (UINT8 *) &SpdDdr4Table,
                         sizeof (SpdDdr4Table),
                         (UINT8 *) &SpdLpddrTable,
                         sizeof (SpdLpddrTable)
                         );
            }
          } else {
            MrcCall->MrcCopyMem ((UINT8 *) &DimmIn->Spd.Data, (UINT8 *) &MemConfigNoCrc->SpdData->SpdData[Channel][Dimm][0], sizeof (MrcSpd));
          }
        }
      } // for Dimm
    } // for Channel
  } // for Controller

  CheckForTimingOverride (Inputs, MemConfig);

#ifdef MINI_FLAG
  MrcMiniBiosOverrides (MrcData);
#endif // MINI_FLAG

  switch (BootMode) {
    case bmWarm:
    case bmS3:
    case bmFast:
      return BootMode;

    default:
      DEBUG ((DEBUG_ERROR, "ERROR: Invalid flow specified, defaulting to cold flow\n"));
      // No break. Note that the boot mode changes to bmCold.
    case bmCold:
      break;
  }
  return bmCold;
}

/**
  Check to see if user defined profile is selected and if it is, then copy the
  timing settings for this profile to the timing override structure. If user
  defined profile is not selected, then set the timing override structure to 0.

  @note Even though we set timings on a DIMM by DIMM basis, the controller
  may force DIMM timings to be the same for all DIMMs in a channel.

  @param[in, out] Inputs              - The MRC Input data structure.
  @param[in]      MemConfig           - MEMORY_CONFIGURATION structure.
**/
void
CheckForTimingOverride (
  IN OUT MrcInput *const          Inputs,
  IN     MEMORY_CONFIGURATION     *MemConfig
  )
{
  MrcControllerIn             *ControllerIn;
  MrcChannelIn                *ChannelIn;
  MrcTiming                   *Timing;
  UINT8                       Controller;
  UINT8                       Channel;
  UINT8                       Dimm;

  //
  // Override DIMM timing settings for customer profile setting.
  //
  if (Inputs->MemoryProfile == USER_PROFILE) {

    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn = &Inputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn = &ControllerIn->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          Timing = &ChannelIn->Dimm[Dimm].Timing;
          Timing->tCL     = MemConfig->tCL;
          Timing->tCWL    = MemConfig->tCWL;
          Timing->tFAW    = MemConfig->tFAW;
          Timing->tRAS    = MemConfig->tRAS;
          Timing->tRCDtRP = MemConfig->tRCDtRP;
          Timing->tREFI   = MemConfig->tREFI;
          Timing->tRFC    = MemConfig->tRFC;
          Timing->tRRD    = MemConfig->tRRD;
          Timing->tRRD_L  = MemConfig->tRRD_L;
          Timing->tRRD_S  = MemConfig->tRRD_S;
          Timing->tRTP    = MemConfig->tRTP;
          Timing->tWR     = MemConfig->tWR;
          Timing->tWTR    = MemConfig->tWTR;
          Timing->tWTR_L  = MemConfig->tWTR_L;
          Timing->tWTR_S  = MemConfig->tWTR_S;
        }
      }
    }
  }

  return;
}

/**
  Sets CpuModel and CpuStepping in MrcData based on CpuModelStep.

  @param[out] MrcData     - The Mrc Host data structure
  @param[in]  CpuModel    - The CPU Family Model.
  @param[in]  CpuStepping - The CPU Stepping.

  @retval mrcSuccess if the model and stepping is found.  Otherwise mrcFail
**/
MrcStatus
MrcSetCpuInformation (
  OUT MrcParameters  *MrcData,
  IN  MrcCpuModel    CpuModel,
  IN  MrcCpuStepping CpuStepping
  )
{
  MrcInput      *Inputs;
  MrcStatus     Status;
  MRC_FUNCTION  *MrcCall;
  CHAR8         *Step;
  UINT64        Revision;

  Inputs = &MrcData->Inputs;
  MrcCall = Inputs->Call.Func;
  Status = mrcFail;

  Inputs->CpuFamily = cfMax;
  Step = NULL;

  switch (CpuModel) {
    case cmKBL_ULX_ULT:
      Inputs->CpuModel = cmKBL_ULX_ULT;
      Inputs->CpuFamily = cfKbl;
      DEBUG ((DEBUG_INFO, "Kabylake ULT/ULX:"));

      switch (CpuStepping) {
        case csKblH0:
          Inputs->CpuStepping = csKblH0;
          Step = "H0/J0";
          break;

        case csKblY0:
          Inputs->CpuStepping = csKblY0;
          Step = "Y0";
          break;

        case csAmlW0:
          Inputs->CpuStepping = csAmlW0;
          Step = "W0";
          break;

        case csAmlV0:
          Inputs->CpuStepping = csAmlV0;
          Step = "V0";
          break;

        default:
          Inputs->CpuStepping = csKblUlxUltLast;
          break;
      }
      Status = mrcSuccess;
      break;

    case cmSKL_ULX_ULT:
      Inputs->CpuModel = cmSKL_ULX_ULT;
      if (CpuStepping > csSklUlxUltLast) {
        Inputs->CpuFamily = cfKbl;
        DEBUG ((DEBUG_INFO, "Kabylake ULT/ULX:"));

        switch (CpuStepping) {
          case csKblG0:
            Inputs->CpuStepping = csKblG0;
            Step = "G0";
            break;

          default:
            Inputs->CpuStepping = csKblUlxUltLast;
            break;
        }
      } else {
        Inputs->CpuFamily = cfSkl;
        DEBUG ((DEBUG_INFO, "Skylake ULT/ULX:"));

        switch (CpuStepping) {
          case csSklB0:
            Inputs->CpuStepping = csSklB0;
            Step = "B0";
            break;

          case csSklC0:
            Inputs->CpuStepping = csSklC0;
            Step = "C0/J0";
            break;

          case csSklD0:
            Inputs->CpuStepping = csSklD0;
            Step = "D0/K0";
            break;

          default:
            Inputs->CpuStepping = csSklUlxUltLast;
            break;
        }
      }
      Status = mrcSuccess;
      break;

    case cmKBL_DT_HALO:
      Inputs->CpuModel = cmKBL_DT_HALO;
      Inputs->CpuFamily = cfKbl;
      DEBUG ((DEBUG_INFO, "Kabylake DT/Halo:"));

      switch (CpuStepping) {
        case csKblB0:
          Inputs->CpuStepping = csKblB0;
          Step = "B0/S0/M0";
          break;

        case csKblN0:
          Inputs->CpuStepping = csKblN0;
          Step = "N0";
          break;

        case csCflB0:
          Inputs->CpuStepping = csCflB0;
          Step = "B0";
          break;

        case csCflP0:
          Inputs->CpuStepping = csCflP0;
          Step = "P0";
          break;

        case csCflR0:
          Inputs->CpuStepping = csCflR0;
          Step = "R0";
          break;

        default:
          Inputs->CpuStepping = csKblDtHaloLast;
          break;
      }
      Status = mrcSuccess;
      break;

    case cmSKL_DT_HALO:
      Inputs->CpuModel = cmSKL_DT_HALO;
      if (CpuStepping > csSklDtHaloLast) {
        Inputs->CpuFamily = cfKbl;
        DEBUG ((DEBUG_INFO, "Kabylake DT/Halo:"));

        switch (CpuStepping) {
          case csKblA0:
            Inputs->CpuStepping = csKblA0;
            Step = "A0";
            break;

          default:
            Inputs->CpuStepping = csKblDtHaloLast;
            break;
        }
      } else {
        Inputs->CpuFamily = cfSkl;
        DEBUG ((DEBUG_INFO, "Skylake DT/Halo:"));

        switch (CpuStepping) {
          case csSklP0:
            Inputs->CpuStepping = csSklP0;
            Step = "P0";
            break;

          case csSklQ0:
            Inputs->CpuStepping = csSklQ0;
            Step = "Q0";
            break;

          case csSklM0:
            Inputs->CpuStepping = csSklM0;
            Step = "M0";
            break;

          case csSklR0:
            Inputs->CpuStepping = csSklR0;
            Step = "R0/S0/N0";
            break;

          default:
            Inputs->CpuStepping = csSklDtHaloLast;
            break;
        }
      }
      Status = mrcSuccess;
      break;
  }

  if (Status == mrcSuccess) {
    if (Step != NULL) {
      DEBUG ((DEBUG_INFO, " Stepping %a\n", Step));
    } else {
      DEBUG ((DEBUG_WARN, "\nWARNING: Unknown CPU stepping, using MRC for last known step = %Xh\n", Inputs->CpuStepping));
    }
  }

  Revision = MrcCall->MrcReadMsr64 (MSR_IA32_BIOS_SIGN_ID);
  DEBUG ((DEBUG_INFO, "Microcode Update: 0x%08X\n", (UINT32) (MrcCall->MrcRightShift64 (Revision, 32))));

  return Status;
}


/**
  Build S3 memory data HOB

  @param[in] MrcData          Pointer to Mrc Parameters
**/
VOID
BuildMemoryS3DataHob (
  IN MrcParameters            *MrcData
  )
{
  VOID *HobPtr;

  HobPtr = NULL;

  DEBUG ((
          DEBUG_INFO,
          "BuildMemoryS3DataHob.\nMemoryS3DataHob Base : %x\nMemoryS3DataHob Size : %x\n",
          &MrcData->Save,
          (UINTN) MrcData->Save.Size
          ));

  HobPtr = BuildGuidDataHob (
             &gSiMemoryS3DataGuid,
             (VOID *)&(MrcData->Save),
             (UINTN) MrcData->Save.Size
           );
  ASSERT (HobPtr != NULL);
}

/**
  Build SMBIOS and OC memory data HOB

  @param[in] MrcData          Pointer to Mrc Parameters
**/
VOID
BuildMemoryInfoDataHob (
  IN MrcParameters            *MrcData
  )
{
  MEMORY_INFO_DATA_HOB *MemoryInfo;
  CONTROLLER_INFO      *ControllerInfo;
  CHANNEL_INFO         *ChannelInfo;
  DIMM_INFO            *DimmInfo;
  MRC_FUNCTION         *MrcCall;
  MrcInput             *Inputs;
  MrcControllerIn      *ControllerIn;
  MrcChannelIn         *ChannelIn;
  MrcSaveData          *SaveData;
  MrcContSave          *ControllerSave;
  MrcChannelSave       *ChannelSave;
  MrcDimmSave          *DimmSave;
  MrcOutput            *Outputs;
  MrcControllerOut     *ControllerOut;
  MrcChannelOut        *ChannelOut;
  MrcDimmOut           *DimmOut;
  MrcProfile           Profile;
  UINT8                Controller;
  UINT8                Channel;
  UINT8                Dimm;
  UINT8                PartNumberOffset;
  UINT8                ModulePartLength;

  SaveData = &MrcData->Save.Data;
  Outputs  = &MrcData->Outputs;
  Inputs   = &MrcData->Inputs;
  MrcCall  = Inputs->Call.Func;

  DEBUG ((DEBUG_INFO, "BuildMemoryInfoDataHob.\n"));

  //
  // Build HOB for MemoryInfoData
  //
  MemoryInfo = BuildGuidHob (
                 &gSiMemoryInfoDataGuid,
                 sizeof (MEMORY_INFO_DATA_HOB)
                 );
  ASSERT (MemoryInfo != NULL);
  if (MemoryInfo == NULL) {
    return;
  }
  ZeroMem ((VOID *) MemoryInfo, sizeof (MEMORY_INFO_DATA_HOB));

  MrcVersionGet (MrcData, (MrcVersion *) &MemoryInfo->Version);
  MemoryInfo->Revision = 0x01;
  MemoryInfo->MemoryType                 = Outputs->DdrType;
  MemoryInfo->ConfiguredMemoryClockSpeed = (UINT16) Outputs->Frequency;
  MemoryInfo->MaximumMemoryClockSpeed    = (UINT16) Outputs->FreqMax;
  if (Outputs->EccSupport) {
    MemoryInfo->ErrorCorrectionType = MemoryErrorCorrectionSingleBitEcc;
  } else {
    MemoryInfo->ErrorCorrectionType = MemoryErrorCorrectionNone;
  }
  MemoryInfo->EccSupport              = Outputs->EccSupport;
  MemoryInfo->TotalPhysicalMemorySize = Outputs->MemoryMapData.TotalPhysicalMemorySize;
  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    MemoryInfo->VddVoltage[Profile]   = Outputs->VddVoltage[Profile];
    if (Profile < MAX_XMP_PROFILE_NUM) {
      // Store the tCK value read from SPD XMP profiles.
      // Use Profile as the logical array indecies for XMP1 and XMP2.
      MemoryInfo->DefaultXmptCK[Profile] = Outputs->DefaultXmptCK[Profile];
    }
  }

  MemoryInfo->MemoryProfile    = Inputs->MemoryProfile;

  MemoryInfo->XmpProfileEnable = SaveData->XmpProfileEnable;
  MemoryInfo->Ratio            = SaveData->Ratio;
  MemoryInfo->RefClk           = SaveData->RefClk;

  PartNumberOffset = sizeof (SPD_MANUFACTURER_ID_CODE) + sizeof (SPD_MANUFACTURING_LOCATION) + sizeof (SPD_MANUFACTURING_DATE) + sizeof (SPD_MANUFACTURER_SERIAL_NUMBER);
  if (MrcData->Outputs.DdrType != MRC_DDR_TYPE_DDR4) {
    PartNumberOffset +=2; //Skip 2 bytes (CRC) to align the PartNumberOffset for DDR3
  }

  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn                 = &Inputs->Controller[Controller];
    ControllerSave               = &SaveData->Controller[Controller];
    ControllerOut                = &Outputs->Controller[Controller];
    ControllerInfo               = &MemoryInfo->Controller[Controller];
    ControllerInfo->Status       = ControllerSave->Status;
    ControllerInfo->DeviceId     = ControllerOut->DeviceId;
    ControllerInfo->RevisionId   = ControllerOut->RevisionId;
    ControllerInfo->ChannelCount = ControllerSave->ChannelCount;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelIn                     = &ControllerIn->Channel[Channel];
      ChannelSave                   = &ControllerSave->Channel[Channel];
      ChannelOut                    = &ControllerOut->Channel[Channel];
      ChannelInfo                   = &ControllerInfo->ChannelInfo[Channel];
      ChannelInfo->Status           = ChannelSave->Status;
      ChannelInfo->ChannelId        = Channel;
      ChannelInfo->DimmCount        = (UINT8) ChannelSave->DimmCount;
      ChannelInfo->tRd2Rd.SG        = ChannelOut->tRd2Rd.SG;
      ChannelInfo->tRd2Rd.DG        = ChannelOut->tRd2Rd.DG;
      ChannelInfo->tRd2Rd.DR        = ChannelOut->tRd2Rd.DR;
      ChannelInfo->tRd2Rd.DD        = ChannelOut->tRd2Rd.DD;
      ChannelInfo->tRd2Wr.SG        = ChannelOut->tRd2Wr.SG;
      ChannelInfo->tRd2Wr.DG        = ChannelOut->tRd2Wr.DG;
      ChannelInfo->tRd2Wr.DR        = ChannelOut->tRd2Wr.DR;
      ChannelInfo->tRd2Wr.DD        = ChannelOut->tRd2Wr.DD;
      ChannelInfo->tWr2Rd.SG        = ChannelOut->tWr2Rd.SG;
      ChannelInfo->tWr2Rd.DG        = ChannelOut->tWr2Rd.DG;
      ChannelInfo->tWr2Rd.DR        = ChannelOut->tWr2Rd.DR;
      ChannelInfo->tWr2Rd.DD        = ChannelOut->tWr2Rd.DD;
      ChannelInfo->tWr2Wr.SG        = ChannelOut->tWr2Wr.SG;
      ChannelInfo->tWr2Wr.DG        = ChannelOut->tWr2Wr.DG;
      ChannelInfo->tWr2Wr.DR        = ChannelOut->tWr2Wr.DR;
      ChannelInfo->tWr2Wr.DD        = ChannelOut->tWr2Wr.DD;
      for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
        MrcCall->MrcCopyMem ((UINT8 *) &ChannelInfo->Timing[Profile], (UINT8 *) &ChannelSave->Timing[Profile], sizeof (MrcTiming));
      }
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        DimmSave = &ChannelSave->DimmSave[Dimm];
        DimmOut  = &ChannelOut->Dimm[Dimm];
        DimmInfo = &ChannelInfo->DimmInfo[Dimm];
        DimmInfo->Status = DimmOut->Status;
        DimmInfo->DimmId = Dimm;
        DimmInfo->DimmCapacity = DimmOut->DimmCapacity;
        if ((DimmInfo->Status == DIMM_PRESENT) && (DimmInfo->DimmCapacity > 0)) {
          DimmInfo->RankInDimm = DimmOut->RankInDimm;
          DimmInfo->MfgId = ((DimmSave->SpdSave[1] << 8) |
                             (DimmSave->SpdSave[0]));
          MrcCall->MrcCopyMem ((UINT8 *) &DimmInfo->ModulePartNum[0],
                               (UINT8 *) &DimmSave->SpdSave[PartNumberOffset],
                               sizeof (SPD4_MODULE_PART_NUMBER));
          DimmInfo->SpdDramDeviceType       = DimmSave->SpdDramDeviceType;
          DimmInfo->SpdModuleType           = DimmSave->SpdModuleType;
          DimmInfo->SpdModuleMemoryBusWidth = DimmSave->SpdModuleMemoryBusWidth;
          ///
          /// Dimm is present in slot
          /// Get the Memory DataWidth info
          /// SPD Offset 8 Bits [2:0] DataWidth aka Primary Bus Width
          ///
          MemoryInfo->DataWidth = 8 * (1 << (DimmInfo->SpdModuleMemoryBusWidth & 0x07));
          MrcCall->MrcCopyMem ((UINT8 *) &DimmInfo->SpdSave[0], (UINT8 *) &DimmSave->SpdSave[0], sizeof (DimmInfo->SpdSave));
          DEBUG ((DEBUG_INFO, "Manufacturer Id of Channel %x Dimm %x is : 0x%x \n", Channel, Dimm, DimmInfo->MfgId));
          DEBUG ((DEBUG_INFO, "Module Part Number of Channel %x Dimm %x is : ", Channel, Dimm));
          for (ModulePartLength = 0; ModulePartLength < sizeof (SPD4_MODULE_PART_NUMBER); ModulePartLength++) {
            DEBUG ((DEBUG_INFO, "%x ", DimmInfo->ModulePartNum[ModulePartLength]));
          }
          DEBUG ((DEBUG_INFO, "\n"));
        }
      }
    }
  }
}

/**
  Build platform memory data HOB

  @param[in]      MrcData             Pointer to Mrc Parameters
  @param[in, out] MemoryPlatformData  Hob to pass memory data for platform code
**/
VOID
BuildMemoryPlatformDataHob (
  IN     MrcParameters            *MrcData,
  IN OUT MEMORY_PLATFORM_DATA_HOB *MemoryPlatformData
  )
{
  MrcInput      *Inputs;
  MrcOutput     *Outputs;
  MrcMemoryMap  *MemoryMapData;

  Outputs = &MrcData->Outputs;
  Inputs  = &MrcData->Inputs;
  MemoryMapData = &Outputs->MemoryMapData;

  DEBUG ((DEBUG_INFO, "BuildMemoryPlatformDataHob.\n"));
  MemoryPlatformData->Data.Revision        = 0x02;
  MemoryPlatformData->Data.BootMode        = Inputs->BootMode;
  MemoryPlatformData->Data.TsegSize        = Inputs->TsegSize;
  MemoryPlatformData->Data.TsegBase        = MemoryMapData->TsegBase;
  MemoryPlatformData->Data.PrmrrSize       = Inputs->PrmrrSize;
  MemoryPlatformData->Data.PrmrrBase       = MemoryMapData->PrmrrBase;
  MemoryPlatformData->Data.GttBase         = MemoryMapData->GttBase;
  MemoryPlatformData->Data.MmioSize        = Inputs->MmioSize;
  MemoryPlatformData->Data.PciEBaseAddress = Inputs->PciEBaseAddress;
}

