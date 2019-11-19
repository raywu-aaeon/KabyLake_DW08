/** @file
  Implementation of Fsp SA Policy Initialization.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2019 Intel Corporation.

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

#include <PeiFspPolicyInitLib.h>

#include <Ppi/SiPolicy.h>
#include <ConfigBlock/MemoryConfig.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmbusLib.h>
#include <Library/MmPciLib.h>
#include <Library/ConfigBlockLib.h>
#include <Library/PcdLib.h>

#include <IndustryStandard/Pci.h>
#include <MrcSpdData.h>
#include <PchAccess.h>
#include <Ppi/FirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiPeiCis.h>
#include <Core/Pei/PeiMain.h>

#define MAX_SPD_PAGE_COUNT           (2)
#define MAX_SPD_PAGE_SIZE            (256)
#define MAX_SPD_SIZE                 (MAX_SPD_PAGE_SIZE * MAX_SPD_PAGE_COUNT)
#define SPD_PAGE_ADDRESS_0           (0x6C)
#define SPD_PAGE_ADDRESS_1           (0x6E)
#define SPD_DDR3_SDRAM_TYPE_OFFSET   (0x02)
#define SPD_DDR3_SDRAM_TYPE_NUMBER   (0x0B)
#define SPD_DDR4_SDRAM_TYPE_NUMBER   (0x0C)
#define SPD_LPDDR3_SDRAM_TYPE_NUMBER (0xF1)
#define SPD_JEDEC_LPDDR3_SDRAM_TYPE_NUMBER (0x0F)
#define XMP_ID_STRING                (0x4A0C)
#define SPD3_MANUF_START             (117)
#define SPD3_MANUF_END               (127)
#define SPD4_MANUF_START             (320)
#define SPD4_MANUF_END               (328)
#define SPDLP_MANUF_START            (320)
#define SPDLP_MANUF_END              (328)

GLOBAL_REMOVE_IF_UNREFERENCED const SPD_OFFSET_TABLE mSpdDdr3Table[] = {
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

GLOBAL_REMOVE_IF_UNREFERENCED const SPD_OFFSET_TABLE mSpdDdr4Table[] = {
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

GLOBAL_REMOVE_IF_UNREFERENCED const SPD_OFFSET_TABLE mSpdLpddrTable[] = {
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


/**
  Read the SPD data over the SMBus, at the specified SPD address, starting at
  the specified starting offset and read the given amount of data.

  @param[in] SpdAddress  - SPD SMBUS address
  @param[in, out] Buffer - Buffer to store the data.
  @param[in] Start       - Starting SPD offset
  @param[in] Size        - The number of bytes of data to read and also the size of the buffer.
  @param[in, out] Page   - The final page that is being pointed to.

  @retval EFI_SUCCESS if the read is successful, otherwise error status.
**/
static
EFI_STATUS
InternalDoSpdRead (
  IN     const UINT8  SpdAddress,
  IN OUT UINT8        *const Buffer,
  IN     const UINT16 Start,
  IN           UINT16 Size,
  IN OUT UINT8        *const Page
  )
{
  EFI_STATUS    EfiStatus;
  BOOLEAN       PageUpdate;
  UINT16        Count;
  UINT16        Index;

  EfiStatus = EFI_DEVICE_ERROR;
  if ((Buffer != NULL) && (Start < MAX_SPD_SIZE) && ((Start + Size) < MAX_SPD_SIZE)) {
    Count = 0;
    PageUpdate = FALSE;
    while (Size--) {
      Index = Start + Count;
      if ((Index / MAX_SPD_PAGE_SIZE) != *Page) {
        *Page = (UINT8) (Index / MAX_SPD_PAGE_SIZE);
        PageUpdate = TRUE;
      }
      Index %= MAX_SPD_PAGE_SIZE;
      if (PageUpdate == TRUE) {
        PageUpdate = FALSE;
        SmBusWriteDataByte ((*Page == 0) ? SPD_PAGE_ADDRESS_0 : SPD_PAGE_ADDRESS_1, 0, &EfiStatus);
      }
      Buffer[Count] = SmBusReadDataByte (SpdAddress | ((UINT32) Index << 8), &EfiStatus);
      if (EFI_SUCCESS != EfiStatus) {
        Buffer[Count] = 0;
        break;
      }
      Count++;
    }
    EfiStatus = EFI_SUCCESS;
  }
  return (EfiStatus);
}

/**
  See if there is valid XMP SPD data.

  @param[in] Debug    - Mrc debug structure.
  @param[in, out] Spd - Mrc SPD structure.
  @param[in] XmpStart - The current offset in the SPD.

  @retval TRUE if valid, FALSE in not.
**/
static
BOOLEAN
InternalVerifyXmp (
  IN OUT MrcSpd *const Spd,
  IN const UINT16 XmpStart
  )
{
  SPD_EXTREME_MEMORY_PROFILE_HEADER      *Header1;
  SPD_EXTREME_MEMORY_PROFILE_HEADER_2_0  *Header2;
  BOOLEAN                                 Xmp;

  Xmp = FALSE;

  switch (((UINT8 *)Spd)[2]) {
    case SPD_DDR3_SDRAM_TYPE_NUMBER:
      Header1 = &Spd->Ddr3.Xmp.Header;
      if (XmpStart == ((UINT32) (Header1) - (UINT32) Spd)) {
        Xmp = TRUE;
        if ((Header1->XmpRevision.Data & 0xFE) == 0x12) {
          return (TRUE);
        } else {
          Header1->XmpId            = 0;
          Header1->XmpOrgConf.Data  = 0;
          Header1->XmpRevision.Data = 0;
        }
      }
      break;
    case SPD_DDR4_SDRAM_TYPE_NUMBER:
      Header2 = &Spd->Ddr4.EndUser.Xmp.Header;
      if (XmpStart == ((UINT32) (Header2) - (UINT32) Spd)) {
        Xmp = TRUE;
        if ((Header2->XmpRevision.Data) == 0x20) {
          return (TRUE);
        } else {
          Header2->XmpId            = 0;
          Header2->XmpOrgConf.Data  = 0;
          Header2->XmpRevision.Data = 0;
        }
      }
      break;
    case SPD_JEDEC_LPDDR3_SDRAM_TYPE_NUMBER:
    case SPD_LPDDR3_SDRAM_TYPE_NUMBER:
      return (TRUE);
    default:
      return (FALSE);
  }
  if (!Xmp) {
    return (TRUE);
  }
  return (FALSE);
}

/**
  Read the SPD data over the SMBus, at the given SmBus SPD address and copy the data to the data structure.
  The SPD data locations read is controlled by the current boot mode.

  @param[in] BootMode          - The current MRC boot mode.
  @param[in] Address           - SPD SmBus address offset.
  @param[in] Buffer            - Buffer that contains the data read from the SPD.
  @param[in] SpdDdr3Table      - Indicates which SPD bytes to read.
  @param[in] SpdDdr3TableSize  - Size of SpdDdr3Table in bytes.
  @param[in] SpdDdr4Table      - Indicates which SPD bytes to read.
  @param[in] SpdDdr4TableSize  - Size of SpdDdr4Table in bytes.
  @param[in] SpdLpddrTable     - Indicates which SPD bytes to read.
  @param[in] SpdLpddrTableSize - Size of SpdLpddrTable in bytes.

  @retval TRUE if the read is successful, otherwise FALSE on error.
**/
BOOLEAN
InternalGetSpdData (
  IN SPD_BOOT_MODE BootMode,
  IN UINT8         Address,
  IN OUT   UINT8   *Buffer,
  IN UINT8         *SpdDdr3Table,
  IN UINT32        SpdDdr3TableSize,
  IN UINT8         *SpdDdr4Table,
  IN UINT32        SpdDdr4TableSize,
  IN UINT8         *SpdLpddrTable,
  IN UINT32        SpdLpddrTableSize
  )
{
  const SPD_OFFSET_TABLE *Tbl;
  const SPD_OFFSET_TABLE *TableSelect;
  EFI_STATUS             Status;
  UINT32                 Byte;
  UINT32                 Stop;
  UINT8                  Page;

  Page   = (UINT8) (~0);
  Status = InternalDoSpdRead (Address, &Buffer[SPD_DDR3_SDRAM_TYPE_OFFSET], 2, 1, &Page);
  if (EFI_SUCCESS == Status) {
    switch (Buffer[SPD_DDR3_SDRAM_TYPE_OFFSET]) {
      case SPD_DDR3_SDRAM_TYPE_NUMBER:
      case SPD_LPDDR3_SDRAM_TYPE_NUMBER:
      default:
        TableSelect = (SPD_OFFSET_TABLE *) SpdDdr3Table;
        Stop = (SpdDdr3TableSize / sizeof (SPD_OFFSET_TABLE));
        break;
      case SPD_DDR4_SDRAM_TYPE_NUMBER:
        TableSelect = (SPD_OFFSET_TABLE *) SpdDdr4Table;
        Stop = (SpdDdr4TableSize / sizeof (SPD_OFFSET_TABLE));
        break;
      case SPD_JEDEC_LPDDR3_SDRAM_TYPE_NUMBER:
        TableSelect = (SPD_OFFSET_TABLE *) SpdLpddrTable;
        Stop = (SpdLpddrTableSize / sizeof (SPD_OFFSET_TABLE));
        break;
    }
    for (Byte = 0; (EFI_SUCCESS == Status) && (Byte < Stop); Byte++) {
      Tbl = &TableSelect[Byte];
      if ((1 << BootMode) & Tbl->BootMode) {
        Status = InternalDoSpdRead (Address, &Buffer[Tbl->Start], Tbl->Start, Tbl->End - Tbl->Start + 1, &Page);
        if (Status == EFI_SUCCESS) {
          if (SpdCold == BootMode) {
            if (FALSE == InternalVerifyXmp ((MrcSpd *) Buffer, Tbl->Start)) {
              break;
            }
          }
        } else {
          break;
        }
      }
    }
  }

  return ((EFI_SUCCESS == Status) ? TRUE : FALSE);
}

/**
  Initialize the Smbus PPI and program the Smbus BAR

  @retval EFI_SUCCESS             The function completes successfully
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database
**/
EFI_STATUS
InternalInitializePchSmbus (
  VOID
  )
{
  UINTN          SmbusRegBase;
  SmbusRegBase = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_SMBUS,
                   PCI_FUNCTION_NUMBER_PCH_SMBUS
                   );

  ///
  /// Since PEI has no PCI enumerator, set the BAR & I/O space enable ourselves
  ///
  MmioAndThenOr32 (SmbusRegBase + R_PCH_SMBUS_BASE, B_PCH_SMBUS_BASE_BAR, PcdGet16 (PcdSmbusBaseAddress));

  MmioOr8 (SmbusRegBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_IO_SPACE);

  ///
  /// Reset the SMBus host controller
  ///
  MmioOr8 (SmbusRegBase + R_PCH_SMBUS_HOSTC, B_PCH_SMBUS_HOSTC_SSRESET);

  ///
  /// Enable the SMBus host controller
  ///
  MmioAndThenOr8 (
    SmbusRegBase + R_PCH_SMBUS_HOSTC,
    (UINT8)(~(B_PCH_SMBUS_HOSTC_SMI_EN | B_PCH_SMBUS_HOSTC_I2C_EN)),
    B_PCH_SMBUS_HOSTC_HST_EN
    );

  ///
  /// Clear Status Register before anyone uses the interfaces
  ///
  IoWrite8 (PcdGet16(PcdSmbusBaseAddress) + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);

  return EFI_SUCCESS;
}

/**
  Update Spd Data

  @param[in][out] FspmUpd              Pointer to FSP UPD Data.
  @param[in]      MemConfigNoCrc       Pointer to Mem Config No Crc.
  @param[in]      MiscPeiPreMemConfig  Pointer to Misc Config.

  @retval         EFI_SUCCESS          The function completes successfully
  @retval         Other                The function fail
**/
VOID
EFIAPI
InternalUpdateSpdData (
  IN OUT FSPM_UPD               *FspmUpd,
  IN MEMORY_CONFIG_NO_CRC       *MemConfigNoCrc,
  IN SA_MISC_PEI_PREMEM_CONFIG  *MiscPeiPreMemConfig
  )
{
  UINT8                     Socket;
  UINT8                     *SpdData;

  InternalInitializePchSmbus ();

  DEBUG ((DEBUG_INFO, "Updating UPD:Memory Spd Pointers...\n"));
  if (FspmUpd == NULL || MemConfigNoCrc == NULL) {
    DEBUG ((DEBUG_ERROR, "EFI_INVALID_PARAMETER.\n"));
    DEBUG ((DEBUG_ERROR, "Fail to access SPD from SiPolicyPpi\n"));
    return;
  }

  if (*((UINT32 *)MiscPeiPreMemConfig->SpdAddressTable) != 0x0) {
    //
    // Update MemConfigNoCrc->SpdData->SpdData
    //
    for (Socket = 0; Socket < SA_MC_MAX_SOCKETS; Socket++) {
      SpdData = (UINT8 *)((UINT32)MemConfigNoCrc->SpdData->SpdData + (Socket * SA_MC_MAX_SPD_SIZE));
      InternalGetSpdData (
        0,
        MiscPeiPreMemConfig->SpdAddressTable[Socket],
        (UINT8 *)SpdData,
        (UINT8 *)&mSpdDdr3Table,
        sizeof(mSpdDdr3Table),
        (UINT8 *)&mSpdDdr4Table,
        sizeof(mSpdDdr4Table),
        (UINT8 *)&mSpdLpddrTable,
        sizeof(mSpdLpddrTable)
        );
    }
  }

  FspmUpd->FspmConfig.MemorySpdPtr00 = (UINT32)MemConfigNoCrc->SpdData->SpdData;
  FspmUpd->FspmConfig.MemorySpdPtr01 = (UINT32)MemConfigNoCrc->SpdData->SpdData + (1 * SA_MC_MAX_SPD_SIZE);
  FspmUpd->FspmConfig.MemorySpdPtr10 = (UINT32)MemConfigNoCrc->SpdData->SpdData + (2 * SA_MC_MAX_SPD_SIZE);
  FspmUpd->FspmConfig.MemorySpdPtr11 = (UINT32)MemConfigNoCrc->SpdData->SpdData + (3 * SA_MC_MAX_SPD_SIZE);

  DEBUG ((DEBUG_INFO, "UPD:MemorySpdPtr Updated\n"));
}

/**
  PeiGetSectionFromFv finds the file in FV and gets file Address and Size

  @param[in] NameGuid              - File GUID
  @param[out] Address              - Pointer to the File Address
  @param[out] Size                 - Pointer to File Size

  @retval EFI_SUCCESS                Successfull in reading the section from FV
**/
EFI_STATUS
EFIAPI
PeiGetSectionFromFv (
  IN CONST  EFI_GUID        NameGuid,
  OUT VOID                  **Address,
  OUT UINT32               *Size
  )
{
  EFI_STATUS  Status;
  EFI_PEI_FIRMWARE_VOLUME_PPI          *FvPpi;
  EFI_FV_FILE_INFO                     FvFileInfo;
  PEI_CORE_INSTANCE                    *PrivateData;
  UINTN                                CurrentFv;
  PEI_CORE_FV_HANDLE                   *CoreFvHandle;
  EFI_PEI_FILE_HANDLE                  VbtFileHandle;
  EFI_GUID                             *VbtGuid;
  EFI_COMMON_SECTION_HEADER            *Section;
  CONST EFI_PEI_SERVICES               **PeiServices;

  PeiServices = GetPeiServicesTablePointer ();

  PrivateData = PEI_CORE_INSTANCE_FROM_PS_THIS(PeiServices);

  Status = PeiServicesLocatePpi (
            &gEfiFirmwareFileSystem2Guid,
            0,
            NULL,
            (VOID **) &FvPpi
            );
  ASSERT_EFI_ERROR (Status);

  CurrentFv = PrivateData->CurrentPeimFvCount;
  CoreFvHandle = &(PrivateData->Fv[CurrentFv]);

  Status = FvPpi->FindFileByName (FvPpi, &NameGuid, &CoreFvHandle->FvHandle, &VbtFileHandle);
  if (!EFI_ERROR(Status) && VbtFileHandle != NULL) {

  DEBUG ((DEBUG_INFO, "Find SectionByType \n"));

    Status = FvPpi->FindSectionByType (FvPpi, EFI_SECTION_RAW, VbtFileHandle, (VOID **) &VbtGuid);
    if (!EFI_ERROR (Status)) {

    DEBUG ((DEBUG_INFO, "GetFileInfo \n"));

      Status = FvPpi->GetFileInfo (FvPpi, VbtFileHandle, &FvFileInfo);
      Section = (EFI_COMMON_SECTION_HEADER *)FvFileInfo.Buffer;

      if (IS_SECTION2 (Section)) {
        ASSERT (SECTION2_SIZE (Section) > 0x00FFFFFF);
        *Size = SECTION2_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER2);
        *Address = ((UINT8 *)Section + sizeof (EFI_COMMON_SECTION_HEADER2));
      } else {
        *Size = SECTION_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER);
        *Address = ((UINT8 *)Section + sizeof (EFI_COMMON_SECTION_HEADER));
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Performs FSP SA PEI Policy initialization in pre-memory.

  @param[in][out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSaPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  EFI_STATUS                  Status;
  SA_MISC_PEI_PREMEM_CONFIG   *MiscPeiPreMemConfig;
  MEMORY_CONFIGURATION        *MemConfig;
  MEMORY_CONFIG_NO_CRC        *MemConfigNoCrc;
  SI_PREMEM_POLICY_PPI        *SiPreMemPolicyPpi;
#ifndef MINTREE_FLAG
  PCIE_PEI_PREMEM_CONFIG      *PciePeiPreMemConfig;
  SWITCHABLE_GRAPHICS_CONFIG  *SgGpioData;
  GRAPHICS_PEI_PREMEM_CONFIG  *GtPreMemConfig;
  OVERCLOCKING_PREMEM_CONFIG  *OcPreMemConfig;
  UINT8                       Index;
#endif //MINTREE_FLAG
  VOID                        *Buffer;

  //
  // Locate SiPreMemPolicyPpi
  //
  SiPreMemPolicyPpi = NULL;
  MiscPeiPreMemConfig = NULL;
  MemConfig = NULL;
  MemConfigNoCrc = NULL;
#ifndef MINTREE_FLAG
  PciePeiPreMemConfig = NULL;
  SgGpioData = NULL;
  GtPreMemConfig = NULL;
  OcPreMemConfig = NULL;
#endif //MINTREE_FLAG


  Status = PeiServicesLocatePpi(
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status) == FALSE) {
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *) &MiscPeiPreMemConfig);
    ASSERT_EFI_ERROR (Status);
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMemoryConfigNoCrcGuid, (VOID *) &MemConfigNoCrc);
    ASSERT_EFI_ERROR (Status);
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gMemoryConfigGuid, (VOID *) &MemConfig);
    ASSERT_EFI_ERROR (Status);
#ifndef MINTREE_FLAG
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gGraphicsPeiPreMemConfigGuid, (VOID *) &GtPreMemConfig);
    ASSERT_EFI_ERROR (Status);
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaPciePeiPreMemConfigGuid, (VOID *) &PciePeiPreMemConfig);
    ASSERT_EFI_ERROR (Status);
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSwitchableGraphicsConfigGuid, (VOID *) &SgGpioData);
    ASSERT_EFI_ERROR (Status);
    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaOverclockingPreMemConfigGuid, (VOID *) &OcPreMemConfig);
    ASSERT_EFI_ERROR (Status);
#endif //MINTREE_FLAG
 }

  DEBUG((DEBUG_INFO, "Updating Dq Byte Map and DQS Byte Swizzling Settings...\n"));
  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcDqByteMap);
  if (Buffer) {
    CopyMem ((VOID *)FspmUpd->FspmConfig.DqByteMapCh0, Buffer, 12);
    CopyMem ((VOID *)FspmUpd->FspmConfig.DqByteMapCh1, (UINT8*) Buffer + 12, 12);
  }
  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcDqsMapCpu2Dram);
  if (Buffer) {
    CopyMem ((VOID *)FspmUpd->FspmConfig.DqsMapCpu2DramCh0, Buffer, 8);
    CopyMem ((VOID *)FspmUpd->FspmConfig.DqsMapCpu2DramCh1, (UINT8*) Buffer + 8, 8);
  }

  DEBUG((DEBUG_INFO, "Updating Dq Pins Interleaved,Rcomp Resistor & Rcomp Target Settings...\n"));
  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcRcompResistor);
  if (Buffer) {
    CopyMem ((VOID *)FspmUpd->FspmConfig.RcompResistor, Buffer, 6);
  }
  Buffer = (VOID *) (UINTN) PcdGet32 (PcdMrcRcompTarget);
  if (Buffer) {
    CopyMem ((VOID *)FspmUpd->FspmConfig.RcompTarget, Buffer, 10);
  }

  //
  // Update UPD:DqPinsInterleaved
  //
  FspmUpd->FspmConfig.DqPinsInterleaved = (UINT8)MemConfig->DqPinsInterleaved;

  //
  // Update UPD:DqPinsInterleaved
  //
  FspmUpd->FspmConfig.CaVrefConfig  = MemConfig->CaVrefConfig;

  //
  // Update UPD:MemorySpdPtrXX
  //
  InternalUpdateSpdData (FspmUpd, MemConfigNoCrc, MiscPeiPreMemConfig);

  //
  // Update UPD:MemorySpdDataLen
  //
  FspmUpd->FspmConfig.MemorySpdDataLen = SA_MC_MAX_SPD_SIZE;

  //
  // Update UPD:PlatformMemorySize
  //
  //
  // @todo: This value is used since #183932. Revisit.
  //
  FspmUpd->FspmConfig.PlatformMemorySize = MemConfigNoCrc->PlatformMemorySize;
  FspmUpd->FspmConfig.CleanMemory        = (UINT8) MemConfigNoCrc->CleanMemory;
  FspmUpd->FspmConfig.SaGv               = MemConfig->SaGv;
  FspmUpd->FspmConfig.RMT                = (UINT8) MemConfig->RMT;
  FspmUpd->FspmConfig.DdrFreqLimit       = MemConfig->DdrFreqLimit;
#ifndef MINTREE_FLAG
  FspmUpd->FspmConfig.IedSize            = MiscPeiPreMemConfig->IedSize;
  FspmUpd->FspmConfig.UserBd             = MiscPeiPreMemConfig->UserBd;
  FspmUpd->FspmConfig.SgDelayAfterPwrEn  = MiscPeiPreMemConfig->SgDelayAfterPwrEn;
  FspmUpd->FspmConfig.SgDelayAfterHoldReset = MiscPeiPreMemConfig->SgDelayAfterHoldReset;
  FspmUpd->FspmConfig.MmioSize           = MiscPeiPreMemConfig->MmioSize;
  FspmUpd->FspmConfig.MmioSizeAdjustment = MiscPeiPreMemConfig->MmioSizeAdjustment;
  FspmUpd->FspmConfig.TxtImplemented     = (UINT8) MiscPeiPreMemConfig->TxtImplemented;
  FspmUpd->FspmConfig.TsegSize           = MiscPeiPreMemConfig->TsegSize;
#endif //MINTREE_FLAG

  FspmUpd->FspmConfig.SpdProfileSelected = MemConfig->SpdProfileSelected;
  FspmUpd->FspmConfig.VddVoltage         = MemConfig->VddVoltage;
  FspmUpd->FspmConfig.RefClk             = MemConfig->RefClk;
  FspmUpd->FspmConfig.Ratio              = MemConfig->Ratio;
  FspmUpd->FspmConfig.OddRatioMode       = (UINT8) MemConfig->OddRatioMode;
  FspmUpd->FspmConfig.tCL                = (UINT8) MemConfig->tCL;
  FspmUpd->FspmConfig.tCWL               = (UINT8) MemConfig->tCWL;
  FspmUpd->FspmConfig.tFAW               = MemConfig->tFAW;
  FspmUpd->FspmConfig.tRAS               = MemConfig->tRAS;
  FspmUpd->FspmConfig.tRCDtRP            = (UINT8) MemConfig->tRCDtRP;
  FspmUpd->FspmConfig.tREFI              = MemConfig->tREFI;
  FspmUpd->FspmConfig.tRFC               = MemConfig->tRFC;
  FspmUpd->FspmConfig.tRRD               = (UINT8) MemConfig->tRRD;
  FspmUpd->FspmConfig.tRTP               = (UINT8) MemConfig->tRTP;
  FspmUpd->FspmConfig.tWR                = (UINT8) MemConfig->tWR;
  FspmUpd->FspmConfig.tWTR               = (UINT8) MemConfig->tWTR;
  FspmUpd->FspmConfig.NModeSupport       = MemConfig->NModeSupport;
  FspmUpd->FspmConfig.DllBwEn0           = MemConfig->DllBwEn0;
  FspmUpd->FspmConfig.DllBwEn1           = MemConfig->DllBwEn1;
  FspmUpd->FspmConfig.DllBwEn2           = MemConfig->DllBwEn2;
  FspmUpd->FspmConfig.DllBwEn3           = MemConfig->DllBwEn3;
  FspmUpd->FspmConfig.EvLoader           = (UINT8) MemConfig->EvLoader;

#ifndef MINTREE_FLAG
  FspmUpd->FspmTestConfig.SkipExtGfxScan= (UINT8) MiscPeiPreMemConfig->SkipExtGfxScan;
  FspmUpd->FspmTestConfig.BdatEnable= (UINT8) MiscPeiPreMemConfig->BdatEnable;
  FspmUpd->FspmTestConfig.ScanExtGfxForLegacyOpRom= (UINT8) MiscPeiPreMemConfig->ScanExtGfxForLegacyOpRom;
  FspmUpd->FspmTestConfig.LockPTMregs= (UINT8) MiscPeiPreMemConfig->LockPTMregs;
#endif //MINTREE_FLAG
#ifndef MINTREE_FLAG
  if (PciePeiPreMemConfig != NULL) {
    FspmUpd->FspmConfig.DmiGen3ProgramStaticEq = (UINT8) PciePeiPreMemConfig->DmiGen3ProgramStaticEq;
    FspmUpd->FspmConfig.Peg0Enable = (UINT8) PciePeiPreMemConfig->Peg0Enable;
    FspmUpd->FspmConfig.Peg1Enable = (UINT8) PciePeiPreMemConfig->Peg1Enable;
    FspmUpd->FspmConfig.Peg2Enable = (UINT8) PciePeiPreMemConfig->Peg2Enable;
    FspmUpd->FspmConfig.Peg0MaxLinkSpeed = (UINT8) PciePeiPreMemConfig->Peg0MaxLinkSpeed;
    FspmUpd->FspmConfig.Peg1MaxLinkSpeed = (UINT8) PciePeiPreMemConfig->Peg1MaxLinkSpeed;
    FspmUpd->FspmConfig.Peg2MaxLinkSpeed = (UINT8) PciePeiPreMemConfig->Peg2MaxLinkSpeed;
    FspmUpd->FspmConfig.Peg0MaxLinkWidth = (UINT8) PciePeiPreMemConfig->Peg0MaxLinkWidth;
    FspmUpd->FspmConfig.Peg1MaxLinkWidth = (UINT8) PciePeiPreMemConfig->Peg1MaxLinkWidth;
    FspmUpd->FspmConfig.Peg2MaxLinkWidth = (UINT8) PciePeiPreMemConfig->Peg2MaxLinkWidth;
    FspmUpd->FspmConfig.Peg0PowerDownUnusedLanes = (UINT8) PciePeiPreMemConfig->Peg0PowerDownUnusedLanes;
    FspmUpd->FspmConfig.Peg1PowerDownUnusedLanes = (UINT8) PciePeiPreMemConfig->Peg1PowerDownUnusedLanes;
    FspmUpd->FspmConfig.Peg2PowerDownUnusedLanes = (UINT8) PciePeiPreMemConfig->Peg2PowerDownUnusedLanes;
    FspmUpd->FspmConfig.InitPcieAspmAfterOprom = (UINT8) PciePeiPreMemConfig->InitPcieAspmAfterOprom;
    FspmUpd->FspmConfig.PegDisableSpreadSpectrumClocking = (UINT8) PciePeiPreMemConfig->PegDisableSpreadSpectrumClocking;
    for (Index = 0; Index < SA_DMI_MAX_LANE; Index++) {
      FspmUpd->FspmConfig.DmiGen3RootPortPreset[Index] = PciePeiPreMemConfig->DmiGen3RootPortPreset[Index];
      FspmUpd->FspmConfig.DmiGen3EndPointPreset[Index] = PciePeiPreMemConfig->DmiGen3EndPointPreset[Index];
      FspmUpd->FspmConfig.DmiGen3EndPointHint[Index] = PciePeiPreMemConfig->DmiGen3EndPointHint[Index];
    }
    for (Index = 0; Index < SA_DMI_MAX_BUNDLE; Index++) {
      FspmUpd->FspmConfig.DmiGen3RxCtlePeaking[Index] = PciePeiPreMemConfig->DmiGen3RxCtlePeaking[Index];
    }
    for (Index = 0; Index < SA_DMI_MAX_BUNDLE; Index++) {
      FspmUpd->FspmConfig.PegGen3RxCtlePeaking[Index] = PciePeiPreMemConfig->PegGen3RxCtlePeaking[Index];
    }
    FspmUpd->FspmConfig.PegDataPtr = (UINT32) PciePeiPreMemConfig->PegDataPtr;
    CopyMem((VOID *)FspmUpd->FspmConfig.PegGpioData, &PciePeiPreMemConfig->PegGpioData, sizeof (PEG_GPIO_DATA));
    FspmUpd->FspmConfig.DmiDeEmphasis = PciePeiPreMemConfig->DmiDeEmphasis;

    for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
      FspmUpd->FspmConfig.PegRootPortHPE[Index] = PciePeiPreMemConfig->PegRootPortHPE[Index];
    }

    FspmUpd->FspmTestConfig.DmiVc1 = (UINT8) PciePeiPreMemConfig->DmiVc1;
    FspmUpd->FspmTestConfig.DmiVcm = (UINT8) PciePeiPreMemConfig->DmiVcm;
    FspmUpd->FspmTestConfig.DmiMaxLinkSpeed = (UINT8) PciePeiPreMemConfig->DmiMaxLinkSpeed;
    FspmUpd->FspmTestConfig.DmiGen3EqPh2Enable = (UINT8) PciePeiPreMemConfig->DmiGen3EqPh2Enable;
    FspmUpd->FspmTestConfig.DmiGen3EqPh3Method = (UINT8) PciePeiPreMemConfig->DmiGen3EqPh3Method;
    FspmUpd->FspmTestConfig.Peg0Gen3EqPh2Enable = (UINT8) PciePeiPreMemConfig->Peg0Gen3EqPh2Enable;
    FspmUpd->FspmTestConfig.Peg1Gen3EqPh2Enable = (UINT8) PciePeiPreMemConfig->Peg1Gen3EqPh2Enable;
    FspmUpd->FspmTestConfig.Peg2Gen3EqPh2Enable = (UINT8) PciePeiPreMemConfig->Peg2Gen3EqPh2Enable;
    FspmUpd->FspmTestConfig.Peg0Gen3EqPh3Method = (UINT8) PciePeiPreMemConfig->Peg0Gen3EqPh3Method;
    FspmUpd->FspmTestConfig.Peg1Gen3EqPh3Method = (UINT8) PciePeiPreMemConfig->Peg1Gen3EqPh3Method;
    FspmUpd->FspmTestConfig.Peg2Gen3EqPh3Method = (UINT8) PciePeiPreMemConfig->Peg2Gen3EqPh3Method;
    FspmUpd->FspmTestConfig.PegGen3ProgramStaticEq = (UINT8) PciePeiPreMemConfig->PegGen3ProgramStaticEq;
    FspmUpd->FspmTestConfig.Gen3SwEqAlwaysAttempt = (UINT8) PciePeiPreMemConfig->Gen3SwEqAlwaysAttempt;
    FspmUpd->FspmTestConfig.Gen3SwEqNumberOfPresets = (UINT8) PciePeiPreMemConfig->Gen3SwEqNumberOfPresets;
    FspmUpd->FspmTestConfig.Gen3SwEqEnableVocTest = (UINT8) PciePeiPreMemConfig->Gen3SwEqEnableVocTest;
    FspmUpd->FspmTestConfig.PegRxCemTestingMode = (UINT8) PciePeiPreMemConfig->PegRxCemTestingMode;
    FspmUpd->FspmTestConfig.PegRxCemLoopbackLane = (UINT8) PciePeiPreMemConfig->PegRxCemLoopbackLane;
    FspmUpd->FspmTestConfig.PegGenerateBdatMarginTable = (UINT8) PciePeiPreMemConfig->PegGenerateBdatMarginTable;
    FspmUpd->FspmTestConfig.PegRxCemNonProtocolAwareness = (UINT8) PciePeiPreMemConfig->PegRxCemNonProtocolAwareness;
    FspmUpd->FspmTestConfig.PegGen3RxCtleOverride = (UINT8) PciePeiPreMemConfig->PegGen3RxCtleOverride;
    for (Index = 0; Index < SA_PEG_MAX_LANE; Index++) {
      FspmUpd->FspmTestConfig.PegGen3RootPortPreset[Index] = PciePeiPreMemConfig->PegGen3RootPortPreset[Index];
      FspmUpd->FspmTestConfig.PegGen3EndPointPreset[Index] = PciePeiPreMemConfig->PegGen3EndPointPreset[Index];
      FspmUpd->FspmTestConfig.PegGen3EndPointHint[Index] = PciePeiPreMemConfig->PegGen3EndPointHint[Index];
    }
    FspmUpd->FspmTestConfig.Gen3SwEqJitterDwellTime = PciePeiPreMemConfig->Gen3SwEqJitterDwellTime;
    FspmUpd->FspmTestConfig.Gen3SwEqJitterErrorTarget = PciePeiPreMemConfig->Gen3SwEqJitterErrorTarget;
    FspmUpd->FspmTestConfig.Gen3SwEqVocDwellTime = PciePeiPreMemConfig->Gen3SwEqVocDwellTime;
    FspmUpd->FspmTestConfig.Gen3SwEqVocErrorTarget = PciePeiPreMemConfig->Gen3SwEqVocErrorTarget;
  }

  if (GtPreMemConfig != NULL) {
    FspmUpd->FspmConfig.PrimaryDisplay = (UINT8) GtPreMemConfig->PrimaryDisplay;
    FspmUpd->FspmConfig.InternalGfx = (UINT8) GtPreMemConfig->InternalGraphics;
    FspmUpd->FspmConfig.IgdDvmt50PreAlloc = (UINT8) GtPreMemConfig->IgdDvmt50PreAlloc;
    FspmUpd->FspmConfig.ApertureSize = (UINT8) GtPreMemConfig->ApertureSize;
    FspmUpd->FspmConfig.GttMmAdr = GtPreMemConfig->GttMmAdr;
    FspmUpd->FspmConfig.GttSize = GtPreMemConfig->GttSize;
    FspmUpd->FspmTestConfig.PanelPowerEnable = (UINT8) GtPreMemConfig->PanelPowerEnable;
  }
#endif //MINTREE_FLAG
  //
  // Update UPD:SmramMask
  //
  if (MemConfig != NULL) {
    FspmUpd->FspmConfig.SmramMask = MemConfig->SmramMask;
  }
#ifndef MINTREE_FLAG
  if (SgGpioData != NULL) {
    CopyMem((VOID *) FspmUpd->FspmConfig.SaRtd3Pcie0Gpio, &SgGpioData->SaRtd3Pcie0Gpio, sizeof (SA_PCIE_RTD3_GPIO));
    CopyMem((VOID *) FspmUpd->FspmConfig.SaRtd3Pcie1Gpio, &SgGpioData->SaRtd3Pcie1Gpio, sizeof (SA_PCIE_RTD3_GPIO));
    CopyMem((VOID *) FspmUpd->FspmConfig.SaRtd3Pcie2Gpio, &SgGpioData->SaRtd3Pcie2Gpio, sizeof (SA_PCIE_RTD3_GPIO));
    FspmUpd->FspmConfig.RootPortDev = SgGpioData->RootPortDev;
    FspmUpd->FspmConfig.RootPortFun = SgGpioData->RootPortFun;
  }
#endif

#ifndef MINTREE_FLAG
  if (OcPreMemConfig != NULL) {
    FspmUpd->FspmConfig.SaOcSupport = (UINT8) OcPreMemConfig->OcSupport;
    FspmUpd->FspmConfig.GtsVoltageMode = (UINT8) OcPreMemConfig->GtsVoltageMode;
    FspmUpd->FspmConfig.GtusVoltageMode = (UINT8) OcPreMemConfig->GtusVoltageMode;
    FspmUpd->FspmConfig.GtsMaxOcRatio = OcPreMemConfig->GtsMaxOcRatio;
    FspmUpd->FspmConfig.GtsVoltageOffset = OcPreMemConfig->GtsVoltageOffset;
    FspmUpd->FspmConfig.GtsVoltageOverride = OcPreMemConfig->GtsVoltageOverride;
    FspmUpd->FspmConfig.GtsExtraTurboVoltage = OcPreMemConfig->GtsExtraTurboVoltage;
    FspmUpd->FspmConfig.GtusVoltageOffset = OcPreMemConfig->GtusVoltageOffset;
    FspmUpd->FspmConfig.GtusVoltageOverride = OcPreMemConfig->GtusVoltageOverride;
    FspmUpd->FspmConfig.GtusExtraTurboVoltage = OcPreMemConfig->GtusExtraTurboVoltage;
    FspmUpd->FspmConfig.SaVoltageOffset = OcPreMemConfig->SaVoltageOffset;
    FspmUpd->FspmConfig.GtusMaxOcRatio = OcPreMemConfig->GtusMaxOcRatio;
  }
#endif //MINTREE_FLAG
  return EFI_SUCCESS;
}

/**
  Performs FSP SA PEI Policy initialization.

  @param[in][out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSaPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  EFI_STATUS                Status;
  SI_POLICY_PPI             *SiPolicyPpi;
  GRAPHICS_PEI_CONFIG       *GtConfig;
#ifndef MINTREE_FLAG
  SA_MISC_PEI_CONFIG        *MiscPeiConfig;
  PCIE_PEI_CONFIG           *PciePeiConfig;
  SKYCAM_CONFIG             *SkyCamPolicy;
  VTD_CONFIG                *Vtd;
  GMM_CONFIG                *GmmConfig;
  UINT8                     Index;
  EFI_BOOT_MODE             BootMode;
// AMI_OVERRIDE_START - Get the CsmFlag from SiConfig
  SI_CONFIG                 *SiConfig = NULL;
// AMI_OVERRIDE_END - Get the CsmFlag from SiConfig
// AMI_OVERRIDE_START - Change to AMI Logo
  EFI_GUID                  PeiVbtGuid  = {0x7e175642, 0xf3ad, 0x490a, 0x9f, 0x8a, 0x2e, 0x9f, 0xc6, 0x93, 0x3d, 0xdd};
  EFI_GUID                  PeiLogoGuid = {0x52bf7d53, 0x79cf, 0x4555, 0xa5, 0x17, 0x1e, 0x8b, 0xb3, 0x04, 0x23, 0x32};
// AMI_OVERRIDE_END - Change to AMI Logo

  //
  // @todo This could be cleared up after FSP provides ExitBootServices NotifyPhase.
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
#endif //MINTREE_FLAG
  //
  // Locate SiPolicyPpi
  //
  SiPolicyPpi = NULL;
  Status = PeiServicesLocatePpi(
             &gSiPolicyPpiGuid,
             0,
             NULL,
             (VOID **)&SiPolicyPpi
             );
  if ((Status == EFI_SUCCESS) && (SiPolicyPpi != NULL)) {
    GtConfig = NULL;
    Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gGraphicsPeiConfigGuid, (VOID *) &GtConfig);
    ASSERT_EFI_ERROR (Status);
#ifndef MINTREE_FLAG
    MiscPeiConfig = NULL;
    Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSaMiscPeiConfigGuid, (VOID *) &MiscPeiConfig);
    ASSERT_EFI_ERROR (Status);

    SkyCamPolicy = NULL;
    Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSkyCamConfigGuid, (VOID *) &SkyCamPolicy);
    ASSERT_EFI_ERROR (Status);

    Vtd = NULL;
    Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gVtdConfigGuid, (VOID *) &Vtd);
    ASSERT_EFI_ERROR (Status);

    GmmConfig = NULL;
    Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gGmmConfigGuid, (VOID *) &GmmConfig);
    ASSERT_EFI_ERROR (Status);

    PciePeiConfig = NULL;
    Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSaPciePeiConfigGuid, (VOID *) &PciePeiConfig);
    ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_START - Get the CsmFlag from SiConfig
    Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSiConfigGuid, (VOID *) &SiConfig);
    ASSERT_EFI_ERROR (Status);
// AMI_OVERRIDE_END - Get the CsmFlag from SiConfig
#endif //MINTREE
  }

  DEBUG ((DEBUG_INFO | DEBUG_INIT, "FSP UpdatePeiSaPolicy\n"));
#ifndef MINTREE_FLAG
  if (MiscPeiConfig != NULL) {
    FspsUpd->FspsConfig.Device4Enable = (UINT8) MiscPeiConfig->Device4Enable;
    FspsUpd->FspsConfig.CridEnable = (UINT8) MiscPeiConfig->CridEnable;
    FspsUpd->FspsConfig.DefaultSvid = MiscPeiConfig->DefaultSvidSid.SubSystemVendorId;
    FspsUpd->FspsConfig.DefaultSid = MiscPeiConfig->DefaultSvidSid.SubSystemId;
    FspsUpd->FspsTestConfig.ChapDeviceEnable = (UINT8) MiscPeiConfig->ChapDeviceEnable;
    FspsUpd->FspsTestConfig.SkipPamLock = (UINT8) MiscPeiConfig->SkipPamLock;
    FspsUpd->FspsTestConfig.EdramTestMode = (UINT8) MiscPeiConfig->EdramTestMode;
  }
  if (PciePeiConfig != NULL) {
    FspsUpd->FspsConfig.DmiAspm = (UINT8) PciePeiConfig->DmiAspm;
    FspsUpd->FspsTestConfig.DmiExtSync = (UINT8) PciePeiConfig->DmiExtSync;
    FspsUpd->FspsTestConfig.DmiIot = (UINT8) PciePeiConfig->DmiIot;
    for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
      FspsUpd->FspsConfig.PegDeEmphasis[Index] = PciePeiConfig->PegDeEmphasis[Index];
      FspsUpd->FspsConfig.PegSlotPowerLimitValue[Index] = PciePeiConfig->PegSlotPowerLimitValue[Index];
      FspsUpd->FspsConfig.PegSlotPowerLimitScale[Index] = PciePeiConfig->PegSlotPowerLimitScale[Index];
      FspsUpd->FspsConfig.PegPhysicalSlotNumber[Index] = PciePeiConfig->PegPhysicalSlotNumber[Index];
      FspsUpd->FspsTestConfig.PegMaxPayload[Index] = PciePeiConfig->PegMaxPayload[Index];
    }
  }
#endif //MINTREE_FLAG
  if (GtConfig != NULL) {
#ifndef MINTREE_FLAG
    FspsUpd->FspsConfig.PavpEnable = (UINT8) GtConfig->PavpEnable;
    FspsUpd->FspsConfig.CdClock = (UINT8) GtConfig->CdClock;
    FspsUpd->FspsTestConfig.RenderStandby = (UINT8) GtConfig->RenderStandby;
    FspsUpd->FspsTestConfig.PmSupport = (UINT8) GtConfig->PmSupport;
    FspsUpd->FspsTestConfig.CdynmaxClampEnable = (UINT8) GtConfig->CdynmaxClampEnable;
    FspsUpd->FspsTestConfig.GtFreqMax = (UINT8) GtConfig->GtFreqMax;
    FspsUpd->FspsConfig.ProgramGtChickenBits = (UINT8) GtConfig->ProgramGtChickenBits;
#endif //MINTREE_FLAG
    //
    // For FSP, FspsUpd->FspsConfig.PeiGraphicsPeimInit is always enabled as default.
    //
    FspsUpd->FspsConfig.PeiGraphicsPeimInit = (UINT8) GtConfig->PeiGraphicsPeimInit; // SA: InternalOnly: For Internal validation we still need to enable both Enable/Disable Cases

    //
    // Update UPD: VBT & LogoPtr
    //
//
// AdvancedFeaturesBegin
//
#ifdef EFI_S3_RESUME
// AMI_OVERRIDE_START - Get the CsmFlag from SiConfig 	
#if 0
    if ((BootMode == BOOT_ON_S3_RESUME) || (PcdGetBool (PcdCsmEnable))) {
#else
    if ((BootMode == BOOT_ON_S3_RESUME) || (((UINT8) SiConfig->CsmFlag)) == 1) {
#endif
// AMI_OVERRIDE_END - Get the CsmFlag from SiConfig 
      FspsUpd->FspsConfig.GraphicsConfigPtr = (UINT32) NULL;
    } else {
      FspsUpd->FspsConfig.GraphicsConfigPtr = (UINT32) GtConfig->GraphicsConfigPtr;
    }
#else
//
// AdvancedFeaturesEnd
//
    FspsUpd->FspsConfig.GraphicsConfigPtr = (UINT32) GtConfig->GraphicsConfigPtr;
#endif //EFI_S3_RESUME // AdvancedFeaturesContent
    DEBUG(( DEBUG_INFO, "VbtPtr from GraphicsPeiConfig is 0x%x\n", FspsUpd->FspsConfig.GraphicsConfigPtr));

    FspsUpd->FspsConfig.LogoPtr  = (UINT32) GtConfig->LogoPtr;
    FspsUpd->FspsConfig.LogoSize = GtConfig->LogoSize;
    DEBUG(( DEBUG_INFO, "LogoPtr from PeiFspSaPolicyInit GraphicsPeiConfig is 0x%x\n", FspsUpd->FspsConfig.LogoPtr));
    DEBUG(( DEBUG_INFO, "LogoSize from PeiFspSaPolicyInit GraphicsPeiConfig is 0x%x\n", FspsUpd->FspsConfig.LogoSize));



  }
#ifndef MINTREE_FLAG
  if (SkyCamPolicy != NULL) {
    FspsUpd->FspsConfig.SaImguEnable = (UINT8) SkyCamPolicy->SaImguEnable;
  }
  if (Vtd != NULL) {
    FspsUpd->FspsConfig.X2ApicOptOut = (UINT8) Vtd->X2ApicOptOut;
    FspsUpd->FspsConfig.VtdBaseAddress[0] = Vtd->BaseAddress[0];
    FspsUpd->FspsConfig.VtdBaseAddress[1] = Vtd->BaseAddress[1];
    FspsUpd->FspsTestConfig.VtdDisable = (UINT8) Vtd->VtdDisable;
  }
  if (GmmConfig != NULL) {
    FspsUpd->FspsConfig.GmmEnable = (UINT8) GmmConfig->GmmEnable;
  }
#endif //MINTREE_FLAG
  return EFI_SUCCESS;
}
