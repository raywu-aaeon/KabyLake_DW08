/** @file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2017 Intel Corporation.

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
  - OpRegion / Software SCI SPEC
**/
///
/// Include files
///
#include "IgdOpRegion.h"
#include "CpuRegs.h"
#include "Library/CpuPlatformLib.h"
#include <Library/HobLib.h>
#include <Library/ConfigBlockLib.h>
#include <Private/SiConfigHob.h>

///
/// Global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED IGD_OPREGION_PROTOCOL                  mIgdOpRegion;
//AMI_OVERRIDE_START >>> EIP426038 - Fix build failed with new GCC flags
//GLOBAL_REMOVE_IF_UNREFERENCED SA_POLICY_PROTOCOL                     *mSaPolicy;
extern SA_POLICY_PROTOCOL                     *mSaPolicy;
//AMI_OVERRIDE_END <<< EIP426038 - Fix build failed with new GCC flags

///
/// Function implementations.
///
/**
  Get VBT data using SaPlaformPolicy

  @param[in] VbtFileBuffer    Pointer to VBT data buffer.

  @retval EFI_SUCCESS      VBT data was returned.
  @retval EFI_NOT_FOUND    VBT data not found.
  @exception EFI_UNSUPPORTED  Invalid signature in VBT data.
**/
EFI_STATUS
GetIntegratedIntelVbtPtr (
  OUT VBIOS_VBT_STRUCTURE **VbtFileBuffer
  )
{
  EFI_STATUS                    Status;
  EFI_PHYSICAL_ADDRESS          VbtAddress;
  UINT32                        Size;
  GRAPHICS_DXE_CONFIG           *GraphicsDxeConfig;

  ///
  /// Get the SA policy.
  ///
  Status = gBS->LocateProtocol (
                  &gSaPolicyProtocolGuid,
                  NULL,
                  (VOID **) &mSaPolicy
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetConfigBlock ((VOID *) mSaPolicy, &gGraphicsDxeConfigGuid, (VOID *)&GraphicsDxeConfig);
  ASSERT_EFI_ERROR (Status);

  VbtAddress = GraphicsDxeConfig->VbtAddress;
  Size       = GraphicsDxeConfig->Size;

  if (VbtAddress == 0x00000000) {
    return EFI_NOT_FOUND;
  } else {
    ///
    /// Check VBT signature
    ///
    *VbtFileBuffer  = NULL;
    *VbtFileBuffer = (VBIOS_VBT_STRUCTURE *) (UINTN) VbtAddress;
    if ((*((UINT32 *) ((*VbtFileBuffer)->HeaderSignature))) != VBT_SIGNATURE) {
      FreePool (*VbtFileBuffer);
      *VbtFileBuffer = NULL;
      return EFI_UNSUPPORTED;
    }
  }
  if (Size == 0) {
    return EFI_NOT_FOUND;
  } else {
    ///
    /// Check VBT size
    ///
    if ((*VbtFileBuffer)->HeaderVbtSize > Size) {
      (*VbtFileBuffer)->HeaderVbtSize = (UINT16) Size;
    }
  }
  return EFI_SUCCESS;
}

/**
  Get a pointer to an uncompressed image of the Intel video BIOS.

  Note: This function would only be called if the video BIOS at 0xC000 is
        missing or not an Intel video BIOS.  It may not be an Intel video BIOS
        if the Intel graphic contoller is considered a secondary adapter.

  @param[in] VBiosImage   - Pointer to an uncompressed Intel video BIOS.  This pointer must
                            be set to NULL if an uncompressed image of the Intel Video BIOS
                            is not obtainable.

  @retval EFI_SUCCESS        - VBiosPtr is updated.
  @exception EFI_UNSUPPORTED - No Intel video BIOS found.
**/
EFI_STATUS
GetIntegratedIntelVBiosPtr (
  OUT INTEL_VBIOS_OPTION_ROM_HEADER **VBiosImage
  )
{
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  INTEL_VBIOS_PCIR_STRUCTURE    *PcirBlockPtr;
  EFI_STATUS                    Status;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  INTEL_VBIOS_OPTION_ROM_HEADER *VBiosRomImage;
  ///
  /// Set as if an umcompressed Intel video BIOS image was not obtainable.
  ///
  VBiosRomImage = NULL;

  ///
  /// Get all PCI IO protocols
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Find the video BIOS by checking each PCI IO handle for an Intel video
  /// BIOS OPROM.
  ///
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ASSERT_EFI_ERROR (Status);

    VBiosRomImage = PciIo->RomImage;

    ///
    /// If this PCI device doesn't have a ROM image, skip to the next device.
    ///
    if (!VBiosRomImage) {
      continue;
    }
    ///
    /// Get pointer to PCIR structure
    ///
    PcirBlockPtr = (INTEL_VBIOS_PCIR_STRUCTURE *) ((UINT8 *) VBiosRomImage + VBiosRomImage->PcirOffset);

    ///
    /// Check if we have an Intel video BIOS OPROM.
    ///
    if ((VBiosRomImage->Signature == OPTION_ROM_SIGNATURE) &&
        (PcirBlockPtr->VendorId == V_SA_MC_VID) &&
        (PcirBlockPtr->ClassCode[0] == 0x00) &&
        (PcirBlockPtr->ClassCode[1] == 0x00) &&
        (PcirBlockPtr->ClassCode[2] == 0x03)
        ) {
      ///
      /// Found Intel video BIOS.
      ///
      *VBiosImage = VBiosRomImage;
      return EFI_SUCCESS;
    }
  }
  ///
  /// No Intel video BIOS found.
  ///
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);
  return EFI_UNSUPPORTED;
}

/**
  Get Intel video BIOS VBT information (i.e. Pointer to VBT and VBT size).
  The VBT (Video BIOS Table) is a block of customizable data that is built
  within the video BIOS and edited by customers.

  @retval EFI_SUCCESS            - Video BIOS VBT information returned.
  @exception EFI_UNSUPPORTED     - Could not find VBT information (*VBiosVbtPtr = NULL).
**/
EFI_STATUS
GetVBiosVbtEndOfDxe (
  VOID
  )
{
  INTEL_VBIOS_PCIR_STRUCTURE    *PcirBlockPtr;
  UINT32                        PcirBlockAddress;
  UINT16                        PciVenderId;
  INTEL_VBIOS_OPTION_ROM_HEADER *VBiosPtr;
  VBIOS_VBT_STRUCTURE           *VBiosVbtPtr;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  EFI_STATUS                    Status;
  VBIOS_VBT_STRUCTURE           *VbtFileBuffer;
  UINTN                         Index;
  UINT8                         LegacyVbtFound;
  GRAPHICS_DXE_CONFIG           *GraphicsDxeConfig;
  EFI_PEI_HOB_POINTERS          HobPtr;
  SI_CONFIG_HOB                 *SiConfigHob;

  VbtFileBuffer = NULL;
  LegacyVbtFound = 1;

  ///
  /// Get the SA policy.
  ///
  Status = gBS->LocateProtocol (
                  &gSaPolicyProtocolGuid,
                  NULL,
                  (VOID **) &mSaPolicy
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetConfigBlock ((VOID *) mSaPolicy, &gGraphicsDxeConfigGuid, (VOID *)&GraphicsDxeConfig);
  ASSERT_EFI_ERROR (Status);

  LegacyBios = NULL;
  VBiosPtr = NULL;
  //
  // Get Silicon Config data HOB
  //
  HobPtr.Guid = GetFirstGuidHob (&gSiConfigHobGuid);
  SiConfigHob = (SI_CONFIG_HOB *)GET_GUID_HOB_DATA (HobPtr.Guid);
  if (SiConfigHob->CsmFlag == 1) {
    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);

    if (LegacyBios) {
      VBiosPtr      = (INTEL_VBIOS_OPTION_ROM_HEADER *) (UINTN) (VBIOS_LOCATION_PRIMARY);
      PcirBlockAddress = VBIOS_LOCATION_PRIMARY + VBiosPtr->PcirOffset;
      PcirBlockPtr  = (INTEL_VBIOS_PCIR_STRUCTURE *) (UINTN) (PcirBlockAddress);
      PciVenderId   = PcirBlockPtr->VendorId;
      ///
      /// If the video BIOS is not at 0xC0000 or it is not an Intel video BIOS get
      /// the integrated Intel video BIOS (must be uncompressed).
      ///
      if ((VBiosPtr->Signature != OPTION_ROM_SIGNATURE) || (PciVenderId != V_SA_MC_VID)) {
        GetIntegratedIntelVBiosPtr (&VBiosPtr);
        if (VBiosPtr != NULL) {
          ///
          /// Video BIOS found.
          ///
          PcirBlockPtr  = (INTEL_VBIOS_PCIR_STRUCTURE *) ((UINT8 *) VBiosPtr + VBiosPtr->PcirOffset);
          PciVenderId   = PcirBlockPtr->VendorId;

          if ((VBiosPtr->Signature != OPTION_ROM_SIGNATURE) || (PciVenderId != V_SA_MC_VID)) {
            ///
            /// Intel video BIOS not found.
            ///
            VBiosVbtPtr = NULL;
            LegacyVbtFound = 0;
          }
        }
      }
    }
  }
  if ((LegacyBios == NULL) || (LegacyVbtFound == 0)) {
    ///
    /// No Video BIOS found, try to get VBT from FV.
    ///
    GetIntegratedIntelVbtPtr (&VbtFileBuffer);
    if (VbtFileBuffer != NULL) {
      ///
      /// Video BIOS not found, use VBT from SaPolicy
      ///
      DEBUG ((DEBUG_INFO, "VBT data found\n"));
      for (Index = 0; (GraphicsDxeConfig->GopVersion[Index] != '\0'); Index++) {
      }
      Index = (Index+1)*2;
      CopyMem (mIgdOpRegion.OpRegion->Header.DVER, GraphicsDxeConfig->GopVersion, Index);
      CopyMem (mIgdOpRegion.OpRegion->VBT.GVD1, VbtFileBuffer, VbtFileBuffer->HeaderVbtSize);
      return EFI_SUCCESS;
    }
  }

  if (VBiosPtr == NULL) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "VBIOS found at 0x%X\n", VBiosPtr));
  VBiosVbtPtr = (VBIOS_VBT_STRUCTURE *) ((UINT8 *) VBiosPtr + VBiosPtr->VbtOffset);

  if ((*((UINT32 *) (VBiosVbtPtr->HeaderSignature))) != VBT_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Initialize Video BIOS version with its build number.
  ///
  mIgdOpRegion.OpRegion->Header.VVER[0] = VBiosVbtPtr->CoreBlockBiosBuild[0];
  mIgdOpRegion.OpRegion->Header.VVER[1] = VBiosVbtPtr->CoreBlockBiosBuild[1];
  mIgdOpRegion.OpRegion->Header.VVER[2] = VBiosVbtPtr->CoreBlockBiosBuild[2];
  mIgdOpRegion.OpRegion->Header.VVER[3] = VBiosVbtPtr->CoreBlockBiosBuild[3];
  CopyMem (mIgdOpRegion.OpRegion->VBT.GVD1, VBiosVbtPtr, VBiosVbtPtr->HeaderVbtSize);

  ///
  /// Return final status
  ///
  return EFI_SUCCESS;
}

/**
  Graphics OpRegion / Software SCI driver installation function.

  @param[in] void         - None
  @retval EFI_SUCCESS     - The driver installed without error.
  @retval EFI_ABORTED     - The driver encountered an error and could not complete
                            installation of the ACPI tables.
**/
EFI_STATUS
IgdOpRegionInit (
  VOID
  )
{
  EFI_HANDLE                    Handle;
  EFI_STATUS                    Status;
  UINT32                        DwordData;
  UINTN                         IgdBaseAddress;
  UINT32                        Data32;
  UINT16                        PchAcpiBaseAddress;
  UINT16                        PchTcoBaseAddress;
  SA_POLICY_PROTOCOL            *SaPolicy;
  CPU_SKU                       CpuSku;
  GRAPHICS_DXE_CONFIG           *GraphicsDxeConfig;
  UINT8                         Index;
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  *SaGlobalNvsArea;

  CpuSku        = GetCpuSku ();

  ///
  /// Get the SA policy.
  ///
  Status = gBS->LocateProtocol (&gSaPolicyProtocolGuid, NULL, (VOID **)&SaPolicy);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetConfigBlock ((VOID *) SaPolicy, &gGraphicsDxeConfigGuid, (VOID *)&GraphicsDxeConfig);
  ASSERT_EFI_ERROR (Status);
  ///
  ///  Locate the SA Global NVS Protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gSaGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &SaGlobalNvsArea
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Allocate an ACPI NVS memory buffer as the IGD OpRegion, zero initialize
  /// the first 1K, and set the IGD OpRegion pointer in the Global NVS
  /// area structure.
  ///
  Status = (gBS->AllocatePool) (EfiACPIMemoryNVS, sizeof (IGD_OPREGION_STRUC), (VOID **) &mIgdOpRegion.OpRegion);
  ASSERT_EFI_ERROR (Status);

  SetMem (mIgdOpRegion.OpRegion, 0x2000, 0);
  SaGlobalNvsArea->Area->IgdOpRegionAddress = (UINT32) (UINTN) (mIgdOpRegion.OpRegion);

  ///
  /// If IGD is disabled return
  ///
  IgdBaseAddress = MmPciBase (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0);
  if (MmioRead32 (IgdBaseAddress + 0) == 0xFFFFFFFF) {
    return EFI_SUCCESS;
  }
  ///
  /// Initialize OpRegion Header
  ///
  CopyMem (mIgdOpRegion.OpRegion->Header.SIGN, HEADER_SIGNATURE, sizeof (HEADER_SIGNATURE));
  ///
  /// Set OpRegion Size in KBs
  ///
  mIgdOpRegion.OpRegion->Header.SIZE = HEADER_SIZE / 1024;
  mIgdOpRegion.OpRegion->Header.OVER = (UINT32) (LShiftU64 (HEADER_OPREGION_VER, 16) + LShiftU64 (HEADER_OPREGION_REV, 8));

  ///
  /// All Mailboxes are supported.
  ///
  mIgdOpRegion.OpRegion->Header.MBOX = HEADER_MBOX_SUPPORT;

  ///
  /// Initialize OpRegion Mailbox 1 (Public ACPI Methods).
  ///
  /// Note - The initial setting of mailbox 1 fields is implementation specific.
  /// Adjust them as needed many even coming from user setting in setup.
  ///
  mIgdOpRegion.OpRegion->MBox1.CLID = LID_OPEN;

  ///
  /// Initialize OpRegion Mailbox 3 (ASLE Interrupt and Power Conservation).
  ///
  /// Note - The initial setting of mailbox 3 fields is implementation specific.
  /// Adjust them as needed many even coming from user setting in setup.
  ///
  ///
  /// Do not initialize TCHE. This field is written by the graphics driver only.
  ///
  ///
  /// The ALSI field is generally initialized by ASL code by reading the embedded controller.
  ///
  mIgdOpRegion.OpRegion->Header.PCON = GraphicsDxeConfig->PlatformConfig;
  if (CpuSku != EnumCpuTrad) {
    mIgdOpRegion.OpRegion->Header.PCON = mIgdOpRegion.OpRegion->Header.PCON | 0x2;
  }

  mIgdOpRegion.OpRegion->MBox3.BCLP = BACKLIGHT_BRIGHTNESS;

  mIgdOpRegion.OpRegion->MBox3.PFIT = (FIELD_VALID_BIT | PFIT_STRETCH);

  ///
  /// Reporting to driver for VR IMON Calibration. Bits [5-1] values supported 14A to 31A.
  ///
  mIgdOpRegion.OpRegion->MBox3.PCFT = (SaGlobalNvsArea->Area->GfxTurboIMON << 1) & 0x003E;

  ///
  /// Set Initial current Brightness
  ///
  mIgdOpRegion.OpRegion->MBox3.CBLV = (INIT_BRIGHT_LEVEL | FIELD_VALID_BIT);

  ///
  /// Static Backlight Brightness Level Duty cycle Mapping Table
  ///
  for (Index = 0; Index < MAX_BCLM_ENTRIES; Index++) {
    mIgdOpRegion.OpRegion->MBox3.BCLM[Index] = GraphicsDxeConfig->BCLM[Index];
  }

  mIgdOpRegion.OpRegion->MBox3.IUER = 0x00;

  if (!EFI_ERROR (Status)) {
    mIgdOpRegion.OpRegion->MBox3.IUER =  GraphicsDxeConfig->IuerStatusVal;
  }

  ///
  /// Initialize hardware state:
  ///   Set ASLS Register to the OpRegion physical memory address.
  ///   Set SWSCI register bit 15 to a "1" to activate SCI interrupts.
  ///
  MmioWrite32 (IgdBaseAddress + R_SA_IGD_ASLS_OFFSET, (UINT32) (UINTN) (mIgdOpRegion.OpRegion));
  MmioAndThenOr16 (IgdBaseAddress + R_SA_IGD_SWSCI_OFFSET, (UINT16) ~(BIT0), BIT15);

  DwordData = MmioRead32 (IgdBaseAddress + R_SA_IGD_ASLS_OFFSET);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (IgdBaseAddress + R_SA_IGD_ASLS_OFFSET),
    1,
    &DwordData
    );
  DwordData = MmioRead32 (IgdBaseAddress + R_SA_IGD_SWSCI_OFFSET);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (IgdBaseAddress + R_SA_IGD_SWSCI_OFFSET),
    1,
    &DwordData
    );

  PchAcpiBaseGet (&PchAcpiBaseAddress);
  PchTcoBaseGet (&PchTcoBaseAddress);
  ///
  /// Clear the B_DMISCI_STS bit in R_TCO1_STS by writing a '1'.
  ///
  IoWrite16 ((UINTN) (PchTcoBaseAddress + R_PCH_TCO1_STS), B_PCH_TCO1_STS_DMISCI);

  ///
  /// Clear the ACPI TCO status.
  ///
  Data32 = B_PCH_ACPI_GPE0_STS_127_96_TC0SCI;
  IoWrite32 ((UINTN) (PchAcpiBaseAddress + R_PCH_ACPI_GPE0_STS_127_96), Data32);

  ///
  /// Enable ACPI TCO SCI's.
  ///
  IoOr16 ((UINTN) (PchAcpiBaseAddress + R_PCH_ACPI_GPE0_EN_127_96), B_PCH_ACPI_GPE0_EN_127_96_TC0SCI);

  ///
  /// Install OpRegion / Software SCI protocol
  ///
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gIgdOpRegionProtocolGuid,
                  &mIgdOpRegion,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Return final status
  ///
  return EFI_SUCCESS;
}

#ifndef MINTREE_FLAG
/**
  Update Graphics OpRegion after PCI enumeration.

  @param[in] void         - None
  @retval EFI_SUCCESS     - The function completed successfully.
**/
EFI_STATUS
UpdateIgdOpRegionEndOfDxe (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         Index;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  PCI_TYPE00                    Pci;
  UINTN                         Segment;
  UINTN                         Bus;
  UINTN                         Device;
  UINTN                         Function;

  Bus      = 0;
  Device   = 0;
  Function = 0;

  DEBUG ((DEBUG_INFO, "UpdateIgdOpRegionEndOfDxe\n"));

  mIgdOpRegion.OpRegion->Header.PCON |= BIT8; //Set External Gfx Adapter field is valid
  mIgdOpRegion.OpRegion->Header.PCON &= (UINT32) (~BIT7); //Assume No External Gfx Adapter

  ///
  /// Get all PCI IO protocols handles
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < HandleCount; Index++) {
      ///
      /// Get the PCI IO Protocol Interface corresponding to each handle
      ///
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiPciIoProtocolGuid,
                      (VOID **) &PciIo
                      );

      if (!EFI_ERROR (Status)) {
        ///
        /// Read the PCI configuration space
        ///
        Status = PciIo->Pci.Read (
                              PciIo,
                              EfiPciIoWidthUint32,
                              0,
                              sizeof (Pci) / sizeof (UINT32),
                              &Pci
                              );

        ///
        /// Find the display controllers devices
        ///
        if (!EFI_ERROR (Status) && IS_PCI_DISPLAY (&Pci)) {
          Status = PciIo->GetLocation (
                            PciIo,
                            &Segment,
                            &Bus,
                            &Device,
                            &Function
                            );

          //
          // Assumption: Onboard devices will be sits on Bus no 0, while external devices will be sits on Bus no > 0
          //
          if (!EFI_ERROR (Status) && (Bus > 0)) {
            //External Gfx Adapter Detected and Available
            DEBUG ((DEBUG_INFO, "PCON - External Gfx Adapter Detected and Available\n"));
            mIgdOpRegion.OpRegion->Header.PCON |= BIT7;
            break;
          }
        }
      }
    }
  }

  ///
  /// Free any allocated buffers
  ///
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  ///
  /// Return final status
  ///
  return Status;
}
#endif //MINTREE_FLAG
