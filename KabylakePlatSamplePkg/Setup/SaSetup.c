/** @file
  The file contains SA related setup options

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2019 Intel Corporation.

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

#include <SetupPrivate.h>
#include "OemSetup.h"
#include "SaSetup.h"
#include "PlatformBoardId.h"
#include "CpuRegs.h"
#include "DimmInfo.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/CpuPlatformLib.h>
#include <Protocol/SaPolicy.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/MemInfo.h>
#include <Protocol/GopPolicy.h>
// AMI_OVERRIDE_START - For our Setup variable design.
#include "AmiSetupPrivate.h"
// AMI_OVERRIDE_END - For our Setup variable design.

GLOBAL_REMOVE_IF_UNREFERENCED EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios = NULL;
static EFI_HII_HANDLE     gHiiHandle;
// AMI_OVERRIDE_START - For our Setup variable design.
static SA_SETUP                 mSaSetup;
static SETUP_VOLATILE_DATA      mSetupVolatileData;
UINT64  gMemorySize;  // Used for SA setup callback function.
// AMI_OVERRIDE_END - For our Setup variable design.

#define  HbPciD0F0RegBase    (UINTN) PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (0 << 15) + (UINT32) (0 << 12)
#define  IgdPciD2F0RegBase   (UINTN) PcdGet64 (PcdPciExpressBaseAddress) + (UINT32) (2 << 15) + (UINT32) (0 << 12)


VOID
UpdatePegInfo (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  );

VOID
UpdateDmiInfo (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  );

#define NB_MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define PCI_CFG_ADDR(bus, dev, func, reg) \
    ((VOID *) (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

EFI_STRING_ID  DimmSizeString[SLOT_NUM] = {
  STRING_TOKEN(STR_DIMM_SIZE_CH0_SLT0_VALUE),
  STRING_TOKEN(STR_DIMM_SIZE_CH0_SLT1_VALUE),
  STRING_TOKEN(STR_DIMM_SIZE_CH1_SLT0_VALUE),
  STRING_TOKEN(STR_DIMM_SIZE_CH1_SLT1_VALUE)
};

EFI_STRING_ID  RankInDimmString[SLOT_NUM] = {
  STRING_TOKEN(STR_DIMM_CH0_SLT0_RANK_VALUE),
  STRING_TOKEN(STR_DIMM_CH0_SLT1_RANK_VALUE),
  STRING_TOKEN(STR_DIMM_CH1_SLT0_RANK_VALUE),
  STRING_TOKEN(STR_DIMM_CH1_SLT1_RANK_VALUE)
};

EFI_STRING_ID  DimmMfgString[SLOT_NUM] = {
  STRING_TOKEN(STR_DIMM_CH0_SLT0_MFG_VALUE),
  STRING_TOKEN(STR_DIMM_CH0_SLT1_MFG_VALUE),
  STRING_TOKEN(STR_DIMM_CH1_SLT0_MFG_VALUE),
  STRING_TOKEN(STR_DIMM_CH1_SLT1_MFG_VALUE)
};

EFI_STRING_ID  DimmStatusString[SLOT_NUM] = {
  STRING_TOKEN(STR_DIMM_CH0_SLT0_STATUS),
  STRING_TOKEN(STR_DIMM_CH0_SLT1_STATUS),
  STRING_TOKEN(STR_DIMM_CH1_SLT0_STATUS),
  STRING_TOKEN(STR_DIMM_CH1_SLT1_STATUS)
};

EFI_STRING_ID  PegInfo[3] = {
  STRING_TOKEN(STR_PEG0_INFO_VALUE),
  STRING_TOKEN(STR_PEG1_INFO_VALUE),
  STRING_TOKEN(STR_PEG2_INFO_VALUE)
};

// AMI_OVERRIDE_START - Created SA setup callback function for some SA features and mistake-proofing.
EFI_STATUS
EFIAPI
NbSetupCallback (
  IN AMI_HII_CALLBACK_PARAMETERS  *CallbackValues
)
{
  EFI_STATUS              Status;
  EFI_GUID                SetupGuid = SETUP_GUID;
  SETUP_DATA              *SetupData = NULL;
  UINTN                   BufferSize;
  UINT32                  IgdMemSize;
  EFI_BROWSER_ACTION      Action;
  EFI_QUESTION_ID         KeyValue;
  EFI_IFR_TYPE_VALUE      *Value;

  Action = CallbackValues->Action;
  KeyValue = CallbackValues->QuestionId;
  Value = CallbackValues->Value;

  ///
  /// No action to perform on Action Changing
  ///
  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  BufferSize= sizeof(SETUP_DATA);
  SetupData = AllocatePool(BufferSize);
  ASSERT (SetupData != NULL);
  if (SetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = HiiGetBrowserData(
             &SetupGuid, L"Setup",
             BufferSize, (UINT8*) SetupData
             );
  if (EFI_ERROR(Status)) return Status;

  switch (KeyValue) {
      case NB_CRID_KEY:
        // Sync up value of CridEnable to PchCrid.
         SetupData->PchCrid = Value->u8;
        break;

      case NB_APERTURE_SIZE_KEY:
        // Above 4GB MMIO BIOS assignment is automatically enabled when selecting 2048 aperture. (SystemAgent\SaInit\Dxe\GraphicsInit.c)
        // To use this feature, please disable CSM Support.
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
        if (Value->u8 >= 15) {
          if (SetupData->CsmSupport == 1) {
            SetupData->ApertureSize = 7;  // APERTURE_SIZE_1024MB
          }
        }
#endif
         break;

      case NB_IGD_DVMT50_PREALLOC_KEY:
        // IgdDvmt50PreAlloc <- Value->u8
        if (Value->u8 < 240) {
          IgdMemSize = 32 * Value->u8;
        } else {
          IgdMemSize = 4 * (Value->u8 - 239);
        }
        // Make sure gMemorySize is more than IgdDvmt50PreAlloc + 1024MB.
        if ((IgdMemSize + 1024) > gMemorySize) {
          SetupData->IgdDvmt50PreAlloc = 1; // STR_DVMT50_PRE_ALLOC_32M
        }
        // Note: enlarging pre-allocated memory for iGFX may need to reduce MmioSize because of 4GB boundary limitation. (Reference: GraphicsConfig.h)
        if ( (Value->u8 == 64) && (SetupData->MaxTolud < MAX_TOLUD_3G) ) {
          SetupData->IgdDvmt50PreAlloc = 1; // STR_DVMT50_PRE_ALLOC_32M
        }
        break;
  }

  if (!EFI_ERROR(Status)) {
    BufferSize= sizeof(SETUP_DATA);
    Status = HiiSetBrowserData(
               &SetupGuid, L"Setup", 
               BufferSize, (UINT8 *) SetupData, NULL
               );
    ASSERT_EFI_ERROR(Status);
  }

  FreePool(SetupData);
  return Status;
}

VOID
InitSaSetupCallback (
  VOID
  )
{
  AmiSetupRegisterCallbackHandler (NB_CRID_KEY, NbSetupCallback);
  AmiSetupRegisterCallbackHandler (NB_APERTURE_SIZE_KEY, NbSetupCallback);
  AmiSetupRegisterCallbackHandler (NB_IGD_DVMT50_PREALLOC_KEY, NbSetupCallback);
}
// AMI_OVERRIDE_END - Created SA setup callback function for some SA features and mistake-proofing.

VOID
EFIAPI
SaSetupCallback (
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
// AMI_OVERRIDE_START - Use token to replace CSM_FLAG because we don't definet it.
//#ifdef CSM_FLAG
#if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
// AMI_OVERRIDE_END - Use token to replace CSM_FLAG because we don't definet it.
  UINT8                           DataHigh;
  UINT8                           DataLow;
  EFI_STATUS                      Status;
  EFI_IA32_REGISTER_SET           RegSet;

  //
  // Check IGFX device
  //
  Status  = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (void **) &LegacyBios);
  if (Status != EFI_SUCCESS) {
    return;
  }
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  if (mSaSetup.VideoOpRom != CSMSETUP_LEGACY_ONLY_OPROMS) {
      return;
  }
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  if (mSaSetup.VideoOpRom != CSMSETUP_LEGACY_ONLY_OPROMS) { 
    return;
  }
  RegSet.X.AX = 0x5f01;
  Status  = LegacyBios->Int86 (LegacyBios, 0x15, &RegSet);

  //
  // HEX2Ascii
  //
  DataHigh  = (UINT8) (((RegSet.X.DX & 0x0F00) >> 4) | (RegSet.X.DX & 0x000F));
  DataLow   = (UINT8) (((RegSet.X.BX & 0x0F00) >> 4) | (RegSet.X.BX & 0x000F));

  InitString (
     gHiiHandle,
     STRING_TOKEN (STR_CHIP_IGFX_VBIOS_REV_VALUE),
     L"%02X%02X",
     DataHigh,
     DataLow
     );
#else
  return;
#endif
}

VOID
InitSaStrings (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  )
{
  UINT8                           MemoryType;
  UINT8                           Profile;
  UINT64                          MemorySize;
  UINT16                          DdrFrequency;
  UINT16                          VendorId;
  EFI_STATUS                      Status;
  UINTN                           VariableSize;
  UINT32                          Value32;
  UINTN                           Slot;
  UINT8                           w;
  UINT8                           x;
  UINT8                           y;
  UINT8                           z;
  UINT8                           Rp0;
  UINT8                           Rpn;
  UINT16                          Data16;
  UINT8                           DimmStatus[SLOT_NUM];
  UINT8                           RankInDimm[SLOT_NUM];
  EFI_EVENT                       EndOfDxeEvent;
  PLATFORM_INFO                   *PlatformInfo = NULL;
  MEM_INFO_PROTOCOL               *MemInfoProtocol;
  MEMORY_INFO_DATA                *MemInfoData;
  MEMORY_TIMING                   *Timing;
  SA_POLICY_PROTOCOL              *SaPolicy;
  UINT8                           SecureBoot;
  CHAR8                           *GtStr;
  CHAR8                           *DimmStatusStr;
  UINT32                          VariableAttributes;
  GRAPHICS_DXE_CONFIG             *GraphicsDxeConfig;
  UINT8                           CpuSku;
  UINT32                          EdramSize;
  UINT32                          SetupVolAttributes;
  UINT32                          CapId0_A;
  UINT32                          CapId0_B;
  CPU_OVERRIDE_IDENTIFIER         CpuIdentifier;

  //
  // Get CpuIdentifier to identify Amberlake system
  //
  CpuIdentifier = GetCpuIdentifier ();

// AMI_OVERRIDE_START - For our Setup menu design.
#if 0
  if ((Class != MAIN_FORM_SET_CLASS) && (Class != ADVANCED_FORM_SET_CLASS)) {
#else
  if ((Class != MAIN_FORM_SET_CLASS) && (Class != CHIPSET_FORM_SET_CLASS)) {
#endif
// AMI_OVERRIDE_END - For our Setup menu design.
    return;
  }

  DEBUG ((DEBUG_INFO, "<InitSaStrings>"));

  // Get CapId0_A
  CapId0_A = *(UINT32 *) PCI_CFG_ADDR (0, 0, 0, 0xE4);
  // Get CapId0_B
  CapId0_B = *(UINT32 *) PCI_CFG_ADDR (0, 0, 0, 0xE8);


  CpuSku = GetCpuSku();
  Status = gBS->LocateProtocol (&gSaPolicyProtocolGuid, NULL, (VOID **) &SaPolicy);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *)SaPolicy, &gGraphicsDxeConfigGuid, (VOID *)&GraphicsDxeConfig);
  ASSERT_EFI_ERROR (Status);

#if 0 // AMI_OVERRIDE_START - For our Setup menu design.
  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &mSetupData
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
#endif // AMI_OVERRIDE_END - For our Setup menu design.


  //
  // Get the Memory Info HOB Protocol if it exists.
  //
  Status = gBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **) &MemInfoProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }

  MemInfoData = &MemInfoProtocol->MemInfoData;

  MemoryType    = MemInfoData->DdrType;
  DdrFrequency  = MemInfoData->ddrFreq;
  Profile       = MemInfoData->Profile;
  Timing        = &MemInfoData->Timing[Profile];

  VariableSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &mSaSetup
                  );
  if (!EFI_ERROR (Status)) {
    mSaSetup.XmpProfileEnable = MemInfoData->XmpProfileEnable;
    Status = gRT->SetVariable (
                    L"SaSetup",
                    &gSaSetupVariableGuid,
                    VariableAttributes,
                    VariableSize,
                    &mSaSetup
                    );
  }

  for (Slot = 0; Slot < SLOT_NUM; Slot++) {
    DimmStatus[Slot] = MemInfoData->DimmStatus[Slot];
    RankInDimm[Slot] = MemInfoData->RankInDimm[Slot];
  }

  //
  // Update the memory size string for main page and chipset page
  //
// AMI_OVERRIDE_START - For our Setup menu design.
#if 0
  if ((Class == MAIN_FORM_SET_CLASS) || (Class == ADVANCED_FORM_SET_CLASS)) {
#else
  if ((Class == MAIN_FORM_SET_CLASS) || (Class == CHIPSET_FORM_SET_CLASS)) {
    gMemorySize = MemInfoData->memSize; // Used for SA setup callback function.
#endif
// AMI_OVERRIDE_END - For our Setup menu design.
    MemorySize = MemInfoData->memSize;
    InitString (HiiHandle, STRING_TOKEN (STR_MEMORY_SIZE_VALUE), L"%5ld MB", MemorySize);
    InitString (HiiHandle, STRING_TOKEN (STR_MEMORY_FREQ_VALUE), L"%5ld MHz", DdrFrequency);

    //
    // MCDECS_CR_MRC_REVISION - MRC version
    //
    Value32 = MmioRead32 ((UINTN) PcdGet64 (PcdMchBaseAddress) + 0x5034);
    x       = (UINT8) ((Value32 & 0xFF000000) >> 24);
    y       = (UINT8) ((Value32 & 0xFF0000) >> 16);
    z       = (UINT8) ((Value32 & 0xFF00) >> 8);
    w       = (UINT8) (Value32 & 0xFF);

    InitString (HiiHandle, STRING_TOKEN (STR_MRC_REV_VALUE), L"%d.%d.%d.%d", x, y, z, w);
  }

  VariableSize = sizeof (SecureBoot);
  Status = gRT->GetVariable (
                  L"SecureBoot",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &VariableSize,
                  &SecureBoot
                  );
  if (EFI_ERROR (Status)) {
// AMI_OVERRIDE_START - We still need GOP/VBIOS version display on setup menu when SecureBoot_SUPPORT disable.
#if 0
    return;
#else
    SecureBoot = 0;
#endif
// AMI_OVERRIDE_END - We still need GOP/VBIOS version display on setup menu when SecureBoot_SUPPORT disable.
  }

// AMI_OVERRIDE_START - CSM_SUPPORT turn off will build errors.
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1) 
// AMI_OVERRIDE_START - For our CSM feature, detect GOP version.
#if 0
  if (((mSetupData.CsmControl == 1) || (mSetupData.CsmControl == 2) || (SecureBoot == 1)) && (MmioRead16 (IgdPciD2F0RegBase + 0x2) != 0xFFFF)) {
#else
  if (((mSaSetup.VideoOpRom == CSMSETUP_UEFI_ONLY_OPROMS) || (SecureBoot == 1)) && (MmioRead16 (IgdPciD2F0RegBase + 0x2) != 0xFFFF)) {
#endif
// AMI_OVERRIDE_END - For our CSM feature, detect GOP version.
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_CHIP_IGFX_GOP_REV_VALUE),
      L"%s",
      &GraphicsDxeConfig->GopVersion
    );
  }
#endif
// AMI_OVERRIDE_END - CSM_SUPPORT turn off will build errors.

  //
  // GT frequency setting
  //
  if ((MmioRead16 (IgdPciD2F0RegBase + 0x2) != 0xFFFF)) {
    Value32 = MmioRead32 ((UINTN) PcdGet64 (PcdMchBaseAddress) + 0x5998);
    Rpn = (UINT8) ((Value32 & 0xFF0000) >> 16);
    Rp0 = (UINT8) (Value32 & 0xFF);
    InitString (HiiHandle, STRING_TOKEN (STR_GT_FREQ_HELP), L"Maximum GT frequency limited by the user. Choose between %dMHz (RPN) and %dMHz (RP0). Value beyond the range will be clipped to min/max supported by SKU", Rpn*50, Rp0*50);
  }

  if (Class == MAIN_FORM_SET_CLASS) {
    gHiiHandle = HiiHandle;
    Data16 = MmioRead16 (IgdPciD2F0RegBase + 0x2);
    if (Data16 != 0xFFFF) {
      if ((CpuIdentifier == EnumAmlY5Watt22CpuId) || (CpuIdentifier == EnumAmlY7Watt22CpuId) || (CpuIdentifier == EnumAmlY7Watt42CpuId)) {
        switch (Data16 & 0x70) {
          case 0x0:
            GtStr = "GT0";
            break;
          case 0x10:
            GtStr = "GT0.5";
            break;
          case 0x20:
            GtStr = "GT1";
            break;
          case 0x30:
            GtStr = "GT1.5";
            break;
          case 0x40:
            GtStr = "GT2";
            break;
          case 0x50:
            GtStr = "GT2.5";
            break;
          case 0x60:
            GtStr = "GT3";
            break;
          case 0x70:
            GtStr = "GT4";
            break;
          default:
            GtStr = "Unknown";
            break;
        }
      } else {
        //
        // DID to GT Sku mapping : X0Y = GT1, X1Y = GT2, X2Y = GT3, X3Y = GT4
        //
        switch (Data16 & 0x30) {
          case 0x0:
            GtStr = "GT1";
            break;

          case 0x10:
            GtStr = "GT2";
            break;

          case 0x20:
            GtStr = "GT3";
            break;

          case 0x30:
            GtStr = "GT4";
            break;

          default:
            GtStr = "Unknown";
            break;
        }
      }
      InitString (gHiiHandle, STRING_TOKEN (STR_PROCESSOR_GT_VALUE), L"%a (0x%X)", GtStr, Data16);
    }

    //
    // EDRAM Size display
    //
    if (AsmReadMsr64 (MSR_PLATFORM_INFO) & B_PLATFORM_INFO_EDRAM_EN) {
      EdramSize = 0;
      if (CpuSku == EnumCpuUlt || EnumCpuUlx) {
        EdramSize = 64;
      } else if (CpuSku == EnumCpuTrad || EnumCpuHalo) {
        EdramSize = 128;
        if ((Data16 != 0xFFFF) && ((Data16 == V_SA_PCI_DEV_2_GT4_SDT_ID) || (Data16 == V_SA_PCI_DEV_2_GT4_KDT_ID))) {
          //
          // Exception for GT4e LGA sku due to LGA packaging limitation
          //
          EdramSize = 64;
        }
      }
      InitString (
        HiiHandle,
        STRING_TOKEN (STR_EDRAM_SIZE_VALUE),
        L"%d MB",
        EdramSize
      );
    }

    //
    // Check IGFX device
    //
    if ((MmioRead8 (HbPciD0F0RegBase + R_SA_DEVEN) & B_SA_DEVEN_D2EN_MASK)) {
      //
      // IGFX enable
      //
      if (!LegacyBios) {
        Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (void **) &LegacyBios);
        if (EFI_ERROR (Status)) {
          Status = gBS->CreateEventEx (
                         EVT_NOTIFY_SIGNAL,
                         TPL_CALLBACK,
                         SaSetupCallback,
                         NULL,
                         &gEfiEndOfDxeEventGroupGuid,
                         &EndOfDxeEvent
                         );
          ASSERT_EFI_ERROR (Status);
        } else {
          SaSetupCallback (NULL, NULL);
        }
      }
    }
  }

// AMI_OVERRIDE_START - For our Setup menu design.
#if 0
  if (Class == ADVANCED_FORM_SET_CLASS) {
#else
  if (Class == CHIPSET_FORM_SET_CLASS) {
#endif
// AMI_OVERRIDE_END - For our Setup menu design.

    VariableSize = sizeof (SETUP_VOLATILE_DATA);
    Status = gRT->GetVariable (
                    L"SetupVolatileData",
                    &gSetupVariableGuid,
                    &SetupVolAttributes,
                    &VariableSize,
                    &mSetupVolatileData
                    );
    ASSERT_EFI_ERROR (Status);
    if (!EFI_ERROR (Status)) {
      for (Slot = 0; Slot < (CH_NUM * DIMM_NUM); Slot++) {
        InitString (
          HiiHandle,
          DimmSizeString[Slot],
          L"%4ld MB (%a)",
          MemInfoData->dimmSize[Slot],
          (MemoryType < 3) ? MemoryTypeStr[MemoryType] : "Unknown"
          );
      }

      for (Slot = 0; Slot < SLOT_NUM; Slot++) {
        if (DimmStatus[Slot] < DIMM_NOT_PRESENT) {
          mSetupVolatileData.DimmPresent[Slot] = 1;
          InitString (
            HiiHandle,
            RankInDimmString[Slot],
            L"%1d",
            RankInDimm[Slot]
            );

          DimmStatusStr = (DimmStatus[Slot] == DIMM_DISABLED) ? "Populated & Disabled" : "Populated & Enabled";

          /**
            Get the Memory Module Vendor JEDEC ID
            Byte 117-118 for DDR3/LPDDR3 and byte 320-321 for DDR4
            It's from first byte of SPD buffer.
          **/
          VendorId = *(UINT16 *) (UINTN) (MemInfoData->DimmsSpdData[Slot]);
          VendorId &= ~(BIT7);  // Clear the parity bit
          for (y = 0; y < MemoryModuleManufacturerListSize; y++) {
            if (VendorId == MemoryModuleManufacturerList[y].MfgId) {
              InitString (
                HiiHandle,
                DimmMfgString[Slot],
                L"%a",
                MemoryModuleManufacturerList[y].String
                );
              break;
            }
          }
        } else {
          mSetupVolatileData.DimmPresent[Slot] = 0;
          DimmStatusStr = "Not Populated / Disabled";
        }
        InitString (
          HiiHandle,
          DimmStatusString[Slot],
          L"%a",
          DimmStatusStr
          );
      } // for Slot

      //
      // SA PCIe code version
      //
      Value32 = MmioRead32 ((UINTN) PcdGet64 (PcdDmiBaseAddress) + R_SA_DMIBAR_SCRATCHPAD1_OFFSET);
      w       = (UINT8) ((Value32 & 0xFF000000) >> 24);
      x       = (UINT8) ((Value32 & 0x00FF0000) >> 16);
      y       = (UINT8) ((Value32 & 0x0000FF00) >> 8);
      z       = (UINT8)  (Value32 & 0x000000FF);

      InitString (
        HiiHandle,
        STRING_TOKEN (STR_SA_PCIE_CODE_VERSION_VALUE),
        L"%d.%d.%d.%d",
        w,
        x,
        y,
        z
        );
      //
      // VT-d status report
      //
      InitString (
        HiiHandle,
        STRING_TOKEN (STR_SA_VTD_VALUE),
        mSetupVolatileData.VTdAvailable ? L"Supported" : L"Unsupported"
        );

      if ((CapId0_A & BIT2) == BIT2) {
        mSetupVolatileData.DdrOverclockSupported = 1;
      }
      
      mSetupVolatileData.DdrType = MemoryType;
      
      //
      // Check if IMGU is supported 
      //
      if ((CapId0_B & BIT31) == BIT31) {
        mSetupVolatileData.ImguSupported = 0;
      } else {
        mSetupVolatileData.ImguSupported = 1;
      }
            
      Status = gRT->SetVariable (
                      L"SetupVolatileData",
                      &gSetupVariableGuid,
                      SetupVolAttributes,
                      VariableSize,
                      &mSetupVolatileData
                      );
      ASSERT_EFI_ERROR (Status);
    } // if SetupVolatileData found
    //
    // Update the tCL, tRCD, tRP and tRAS string with data obtained from MemInfo protocol
    //
// AMI_OVERRIDE_START - For our OC feature design.
#if defined(OVER_CLOCK_SUPPORT) && (OVER_CLOCK_SUPPORT == 1)
// AMI_OVERRIDE_END - For our OC feature design.
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_MEMORY_VDD_VALUE),
      L"%d",
      MemInfoData->VddVoltage[Profile]
      );
// AMI_OVERRIDE_START - For our OC feature design.
#endif
// AMI_OVERRIDE_END - For our OC feature design.

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_MEMORY_TIMINGS_VALUE),
      L"%d-%d-%d-%d",
      Timing->tCL,
      Timing->tRCDtRP,
      Timing->tRCDtRP,
      Timing->tRAS
      );

// AMI_OVERRIDE_START - Created SA setup callback function for some SA features and mistake-proofing.
    InitSaSetupCallback();
// AMI_OVERRIDE_END - Created SA setup callback function for some SA features and mistake-proofing.

  } // if ADVANCED_FORM_SET_CLASS

  //
  // Locate Platform Info Protocol.
  //
  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, (VOID **) &PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR(Status) && (PlatformInfo->PlatformType == TypeTrad)) {
    UpdatePegInfo (HiiHandle, Class);
    UpdateDmiInfo (HiiHandle, Class);
  }

}

VOID
UpdatePegInfo (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  )
{
  UINT16              Data16;
  UINT8               Index;
  UINT8               PegDeviceNumber;
  UINT8               PegFunctionNumber;
  UINTN               VariableSize;
  EFI_STATUS          Status;
  UINT32              SetupVolAttributes;
  Data16 = 0;

// AMI_OVERRIDE_START - For our Setup menu design.
#if 0
  if (Class == ADVANCED_FORM_SET_CLASS) {
#else
  if (Class == CHIPSET_FORM_SET_CLASS) {
#endif
// AMI_OVERRIDE_END - For our Setup menu design.

    VariableSize = sizeof (SETUP_VOLATILE_DATA);
    Status = gRT->GetVariable (
                    L"SetupVolatileData",
                    &gSetupVariableGuid,
                    &SetupVolAttributes,
                    &VariableSize,
                    &mSetupVolatileData
                    );
    ASSERT_EFI_ERROR (Status);

    for (Index = 0; Index < 3; Index++) {

      PegDeviceNumber   = SA_PEG10_DEV_NUM;
      PegFunctionNumber = Index;
      //
      // Initialize PegPresent = 0 (Not Present)
      //
      mSetupVolatileData.PegPresent[Index] = 0;

      //
      // Check for DID VID to check Root Port is present
      //
      Data16 = *(UINT16 *) PCI_CFG_ADDR (SA_PEG_BUS_NUM, PegDeviceNumber, PegFunctionNumber, 0x0);

      if (Data16 != 0xFFFF) {
        //
        // Read Slot Status register
        //
        Data16 = *(UINT16 *) PCI_CFG_ADDR (SA_PEG_BUS_NUM, PegDeviceNumber, PegFunctionNumber, R_SA_PEG_SLOTSTS_OFFSET);

        //
        // Check for Presence Detect State SlotStatus 0xba[6]
        //
        if (Data16 & 0x40) {
          //
          // Read LinkStatus 0xb2[3-0]-Current Link Speed, [9-4]-Negotiated Link Width.
          //
          Data16 = *(UINT16 *) PCI_CFG_ADDR (SA_PEG_BUS_NUM, PegDeviceNumber, PegFunctionNumber, R_SA_PEG_LSTS_OFFSET);

          InitString (
            HiiHandle,
            PegInfo[Index],
            L"x%d  Gen%1d",
            ((Data16 & 0x3f0) >> 4),
            (Data16 & 0xf)
            );

          //
          // Initialize PegPresent = 1 (Present)
          //
          mSetupVolatileData.PegPresent[Index] = 1;

        }
      }
    }
    //
    // for loop
    //
    Status = gRT->SetVariable (
                    L"SetupVolatileData",
                    &gSetupVariableGuid,
                    SetupVolAttributes,
                    VariableSize,
                    &mSetupVolatileData
                    );
    ASSERT_EFI_ERROR (Status);

  }
  //
  // ADVANCED_FORM_SET_CLASS
  //
}

VOID
UpdateDmiInfo (
  EFI_HII_HANDLE HiiHandle,
  UINT16         Class
  )
{
  UINT16  Data16;
  UINT64  DmiBar;

  //
  // Get DMIBAR
  //
  DmiBar = *(UINT64 *) PCI_CFG_ADDR (0, 0, 0, R_SA_DMIBAR) &~BIT0;

  //
  // LSTS 9:4 - DMI Link Negotiated Width, 3:0 - Max Link Speed, Gen2/Gen1 Infomation
  //
  Data16 = *(UINT16 *) (UINTN) (DmiBar + 0x8a);

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_DMI_INFO_VALUE),
    L"X%d  Gen%1d",
    ((Data16 >> 4) & 0xf),
    (Data16 & 0xf)
    );
}
