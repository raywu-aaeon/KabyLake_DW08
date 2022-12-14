//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/IntelGopDriver/IntelSaGopPolicy.c 4     10/31/12 6:33a Jeffch $
//
// $Revision: 4 $
//
// $Date: 10/31/12 6:33a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/IntelGopDriver/IntelSaGopPolicy.c $
// 
//*************************************************************************
/** @file LegacyRegionDxe.c
    This file initializes and Installs LegacyRegion2 Protocol.

**/
//**********************************************************************

#include "LegacyRegionDxe.h"

//
// Uncore SAD (System Address Decode) device register map.
//
// PAM Range       Offset  Bits  Operation
// =============== ======  ====  ===============================================================
// 0xC0000-0xC3FFF  0x81   1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xC4000-0xC7FFF  0x81   5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xC8000-0xCBFFF  0x82   1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xCC000-0xCFFFF  0x82   5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD0000-0xD3FFF  0x83   1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD4000-0xD7FFF  0x83   5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xD8000-0xDBFFF  0x84   1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xDC000-0xDFFFF  0x84   5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE0000-0xE3FFF  0x85   1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE4000-0xE7FFF  0x85   5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xE8000-0xEBFFF  0x86   1:0   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xEC000-0xEFFFF  0x86   5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
// 0xF0000-0xFFFFF  0x80   5:4   00 = DRAM Disabled, 01= Read Only, 10 = Write Only, 11 = Normal
//
STATIC LEGACY_MEMORY_SECTION_INFO   mSectionArray[] = {
  {0xC0000, 0x4000,  FALSE, FALSE},
  {0xC4000, 0x4000,  FALSE, FALSE},
  {0xC8000, 0x4000,  FALSE, FALSE},
  {0xCC000, 0x4000,  FALSE, FALSE},
  {0xD0000, 0x4000,  FALSE, FALSE},
  {0xD4000, 0x4000,  FALSE, FALSE},
  {0xD8000, 0x4000,  FALSE, FALSE},
  {0xDC000, 0x4000,  FALSE, FALSE},
  {0xE0000, 0x4000,  FALSE, FALSE},
  {0xE4000, 0x4000,  FALSE, FALSE},
  {0xE8000, 0x4000,  FALSE, FALSE},
  {0xEC000, 0x4000,  FALSE, FALSE},
  {0xF0000, 0x10000, FALSE, FALSE}
};

STATIC PAM_REGISTER_VALUE  mRegisterValues[] = {
  {QP_REG_PAM1_OFFSET, 0x01, 0x02},
  {QP_REG_PAM1_OFFSET, 0x10, 0x20},
  {QP_REG_PAM2_OFFSET, 0x01, 0x02},
  {QP_REG_PAM2_OFFSET, 0x10, 0x20},
  {QP_REG_PAM3_OFFSET, 0x01, 0x02},
  {QP_REG_PAM3_OFFSET, 0x10, 0x20},
  {QP_REG_PAM4_OFFSET, 0x01, 0x02},
  {QP_REG_PAM4_OFFSET, 0x10, 0x20},
  {QP_REG_PAM5_OFFSET, 0x01, 0x02},
  {QP_REG_PAM5_OFFSET, 0x10, 0x20},
  {QP_REG_PAM6_OFFSET, 0x01, 0x02},
  {QP_REG_PAM6_OFFSET, 0x10, 0x20},
  {QP_REG_PAM0_OFFSET, 0x10, 0x20}
};

//
// Handle used to install the Legacy Region Protocol
//
STATIC EFI_HANDLE  mHandle = NULL;

//
// Instance of the Legacy Region Protocol to install into the handle database
//
STATIC EFI_LEGACY_REGION2_PROTOCOL  mLegacyRegion2 = {
  LegacyRegion2Decode,
  LegacyRegion2Lock,
  LegacyRegion2BootLock,
  LegacyRegion2Unlock,
  LegacyRegionGetInfo
};

STATIC
EFI_STATUS
LegacyRegionManipulationInternal (
  IN  UINT32                  Start,
  IN  UINT32                  Length,
  IN  BOOLEAN                 *ReadEnable,
  IN  BOOLEAN                 *WriteEnable,
  OUT UINT32                  *Granularity
  )
{
  UINT32                        EndAddress;
  UINTN                         Index;
  UINTN                         StartIndex;

  //
  // Validate input parameters.
  //
  if (Length == 0 || Granularity == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  EndAddress = Start + Length - 1;
  if ((Start < QP_PAM_BASE_ADDRESS) || EndAddress > QP_PAM_LIMIT_ADDRESS) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Loop to find the start PAM.
  //
  StartIndex = 0;
  for (Index = 0; Index < (sizeof(mSectionArray) / sizeof (mSectionArray[0])); Index++) {
    if ((Start >= mSectionArray[Index].Start) && (Start < (mSectionArray[Index].Start + mSectionArray[Index].Length))) {
      StartIndex = Index;
      break;
    }
  }
  ASSERT (Index < (sizeof(mSectionArray) / sizeof (mSectionArray[0])));

  //
  // Program PAM until end PAM is encountered
  //
  for (Index = StartIndex; Index < (sizeof(mSectionArray) / sizeof (mSectionArray[0])); Index++) {
    if (ReadEnable != NULL) {
      if (*ReadEnable) {
        PciOr8 (
          PCI_LIB_ADDRESS(MAX_BUS_NUMBER, QP_ARCHITECTURE_DEV, QP_SAD_FUNC, mRegisterValues[Index].PAMRegOffset),
          mRegisterValues[Index].ReadEnableData
          );
      } else {
        PciAnd8 (
          PCI_LIB_ADDRESS(MAX_BUS_NUMBER, QP_ARCHITECTURE_DEV, QP_SAD_FUNC, mRegisterValues[Index].PAMRegOffset),
          (UINT8) (~mRegisterValues[Index].ReadEnableData)
          );
      }
    }
    if (WriteEnable != NULL) {
      if (*WriteEnable) {
        PciOr8 (
          PCI_LIB_ADDRESS(MAX_BUS_NUMBER, QP_ARCHITECTURE_DEV, QP_SAD_FUNC, mRegisterValues[Index].PAMRegOffset),
          mRegisterValues[Index].WriteEnableData
          );
      } else {
        PciAnd8 (
          PCI_LIB_ADDRESS(MAX_BUS_NUMBER, QP_ARCHITECTURE_DEV, QP_SAD_FUNC, mRegisterValues[Index].PAMRegOffset),
          (UINT8) (~mRegisterValues[Index].WriteEnableData)
          );
      }
    }

    //
    // If the end PAM is encountered, record its length as granularity and jump out.
    //
    if ((EndAddress >= mSectionArray[Index].Start) && (EndAddress < (mSectionArray[Index].Start + mSectionArray[Index].Length))) {
      *Granularity = mSectionArray[Index].Length;
      break;
    }
  }
  ASSERT (Index < (sizeof(mSectionArray) / sizeof (mSectionArray[0])));

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
LegacyRegionGetInfoInternal (
  OUT UINT32                        *DescriptorCount,
  OUT LEGACY_MEMORY_SECTION_INFO    **Descriptor
  )
{
  UINTN    Index;
  UINT8    PamValue;

  //
  // Check input parameters
  //
  if (DescriptorCount == NULL || Descriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Fill in current status of legacy region.
  //
  *DescriptorCount = sizeof(mSectionArray) / sizeof (mSectionArray[0]);
  for (Index = 0; Index < *DescriptorCount; Index++) {
    PamValue = PciRead8 (PCI_LIB_ADDRESS(MAX_BUS_NUMBER, QP_ARCHITECTURE_DEV, QP_SAD_FUNC, mRegisterValues[Index].PAMRegOffset));
    mSectionArray[Index].ReadEnabled = FALSE;
    if ((PamValue & mRegisterValues[Index].ReadEnableData) != 0) {
      mSectionArray[Index].ReadEnabled = TRUE;
    }
    mSectionArray[Index].WriteEnabled = FALSE;
    if ((PamValue & mRegisterValues[Index].WriteEnableData) != 0) {
      mSectionArray[Index].WriteEnabled = TRUE;
    }
  }

  *Descriptor = mSectionArray;
  return EFI_SUCCESS;
}

/**
  Modify the hardware to allow (decode) or disallow (not decode) memory reads in a region.

  If the On parameter evaluates to TRUE, this function enables memory reads in the address range
  Start to (Start + Length - 1).
  If the On parameter evaluates to FALSE, this function disables memory reads in the address range
  Start to (Start + Length - 1).

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose attributes
                                should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address
                                was not aligned to a region's starting address or if the length
                                was greater than the number of bytes in the first region.
  @param  On[in]                Decode / Non-Decode flag.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Decode (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity,
  IN  BOOLEAN                      *On
  )
{
  return LegacyRegionManipulationInternal (Start, Length, On, NULL, Granularity);
}


/**
  Modify the hardware to disallow memory attribute changes in a region.

  This function makes the attributes of a region read only. Once a region is boot-locked with this
  function, the read and write attributes of that region cannot be changed until a power cycle has
  reset the boot-lock attribute. Calls to Decode(), Lock() and Unlock() will have no effect.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.
  @retval EFI_UNSUPPORTED       The chipset does not support locking the configuration registers in
                                a way that will not affect memory regions outside the legacy memory
                                region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2BootLock (
  IN  EFI_LEGACY_REGION2_PROTOCOL         *This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_UNSUPPORTED;
}


/**
  Modify the hardware to disallow memory writes in a region.

  This function changes the attributes of a memory range to not allow writes.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Lock (
  IN  EFI_LEGACY_REGION2_PROTOCOL *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity
  )
{
  BOOLEAN  WriteEnable;

  WriteEnable = FALSE;
  return LegacyRegionManipulationInternal (Start, Length, NULL, &WriteEnable, Granularity);
}


/**
  Modify the hardware to allow memory writes in a region.

  This function changes the attributes of a memory range to allow writes.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Unlock (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity
  )
{
  BOOLEAN  WriteEnable;

  WriteEnable = TRUE;
  return LegacyRegionManipulationInternal (Start, Length, NULL, &WriteEnable, Granularity);
}

/**
  Get region information for the attributes of the Legacy Region.

  This function is used to discover the granularity of the attributes for the memory in the legacy
  region. Each attribute may have a different granularity and the granularity may not be the same
  for all memory ranges in the legacy region.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  DescriptorCount[out]  The number of region descriptor entries returned in the Descriptor
                                buffer.
  @param  Descriptor[out]       A pointer to a pointer used to return a buffer where the legacy
                                region information is deposited. This buffer will contain a list of
                                DescriptorCount number of region descriptors.  This function will
                                provide the memory for the buffer.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegionGetInfo (
  IN  EFI_LEGACY_REGION2_PROTOCOL   *This,
  OUT UINT32                        *DescriptorCount,
  OUT EFI_LEGACY_REGION_DESCRIPTOR  **Descriptor
  )
{
  LEGACY_MEMORY_SECTION_INFO   *SectionInfo;
  UINT32                       SectionCount;
  EFI_LEGACY_REGION_DESCRIPTOR *DescriptorArray;
  UINTN                        Index;
  UINTN                        DescriptorIndex;

  //
  // Get section numbers and information
  //
  LegacyRegionGetInfoInternal (&SectionCount, &SectionInfo);

  //
  // Each section has 3 descriptors, corresponding to readability, writeability, and lock status.
  //
  DescriptorArray = AllocatePool (sizeof (EFI_LEGACY_REGION_DESCRIPTOR) * SectionCount * 3);
  if (DescriptorArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DescriptorIndex = 0;
  for (Index = 0; Index < SectionCount; Index++) {
    DescriptorArray[DescriptorIndex].Start       = SectionInfo[Index].Start;
    DescriptorArray[DescriptorIndex].Length      = SectionInfo[Index].Length;
    DescriptorArray[DescriptorIndex].Granularity = SectionInfo[Index].Length;
    if (SectionInfo[Index].ReadEnabled) {
      DescriptorArray[DescriptorIndex].Attribute   = LegacyRegionDecoded;
    } else {
      DescriptorArray[DescriptorIndex].Attribute   = LegacyRegionNotDecoded;
    }
    DescriptorIndex++;

    //
    // Create descriptor for writeability, according to lock status
    //
    DescriptorArray[DescriptorIndex].Start       = SectionInfo[Index].Start;
    DescriptorArray[DescriptorIndex].Length      = SectionInfo[Index].Length;
    DescriptorArray[DescriptorIndex].Granularity = SectionInfo[Index].Length;
    if (SectionInfo[Index].WriteEnabled) {
      DescriptorArray[DescriptorIndex].Attribute = LegacyRegionWriteEnabled;
    } else {
      DescriptorArray[DescriptorIndex].Attribute = LegacyRegionWriteDisabled;
    }
    DescriptorIndex++;

    //
    // Chipset does not support bootlock.
    //
    DescriptorArray[DescriptorIndex].Start       = SectionInfo[Index].Start;
    DescriptorArray[DescriptorIndex].Length      = SectionInfo[Index].Length;
    DescriptorArray[DescriptorIndex].Granularity = SectionInfo[Index].Length;
    DescriptorArray[DescriptorIndex].Attribute   = LegacyRegionNotLocked;
    DescriptorIndex++;
  }

  *DescriptorCount = (UINT32) DescriptorIndex;
  *Descriptor      = DescriptorArray;

  return EFI_SUCCESS;
}

/**
  Entry point to the DXE Driver that produces the Legacy Region Protocol.

  @retval  EFI_SUCCESS One or more of the drivers returned a success code.
  @retval  !EFI_SUCESS The return status from the last driver entry point in the list.

**/
EFI_STATUS
EFIAPI
LegacyRegionInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Install the Legacy Region Protocol on a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gEfiLegacyRegion2ProtocolGuid, &mLegacyRegion2,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
