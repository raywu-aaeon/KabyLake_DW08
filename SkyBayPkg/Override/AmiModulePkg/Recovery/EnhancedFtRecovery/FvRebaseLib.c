//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file
  Fv Rebase Library file.
**/
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Pi\PiFirmwareVolume.h>
#include <Library\PeCoffLib.h>
#include <Guid\FirmwareFileSystem2.h>
#include <EfiImageFormat.h>
#include <Protocol\FlashProtocol.h>
#include <Token.h>
//-#include <..\ReFlash\ReFlash.h>
#if !defined(AMI_ROM_LAYOUT_FV_RECOVERY_ADDRESS)
#define AMI_ROM_LAYOUT_FV_RECOVERY_ADDRESS 0
#define AMI_ROM_LAYOUT_FV_RECOVERY_SIZE 0
#endif
#if !defined(AMI_ROM_LAYOUT_FV_BB_BACKUP_ADDRESS)
#define AMI_ROM_LAYOUT_FV_BB_BACKUP_ADDRESS (FV_BB_BASE - FV_BB_SIZE)
#endif
#if !defined(AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_ADDRESS)
#define AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_ADDRESS (FV_MAIN_BASE + (FV_MAIN_SIZE - FV_BB_SIZE))
#endif
//----------------------------------------------------------------------------
// Function Externs
EFI_STATUS
GetNextFile (
  IN EFI_FFS_FILE_HEADER          *CurrentFile,
  OUT EFI_FFS_FILE_HEADER         **NextFile
  );
EFI_STATUS
InitializeFvLib (
  IN VOID                         *Fv,
  IN UINT32                       FvLength
  );
EFI_STATUS
VerifyFfsFile (
  IN EFI_FFS_FILE_HEADER  *FfsHeader
  );
EFI_STATUS
GetSectionByType (
  IN EFI_FFS_FILE_HEADER          *File,
  IN EFI_SECTION_TYPE             SectionType,
  IN UINTN                        Instance,
  OUT EFI_FILE_SECTION_POINTER    *Section
  );
UINT32
GetLength (
  UINT8     *ThreeByteLength
  );
VOID *
EFIAPI
PeCoffLoaderGetPdbPointer (
  IN VOID  *Pe32Data
  );

UINT16
PeCoffLoaderGetPeHeaderMagicValue (
  IN  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION  Hdr
  );
VOID
EFIAPI
PeCoffLoaderRelocateImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  );
VOID *
PeCoffLoaderImageAddress (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT          *ImageContext,
  IN     UINTN                                 Address, 
  IN     UINTN                                 TeStrippedOffset
  );
RETURN_STATUS
PeCoffLoaderRelocateImageEx (
  IN UINT16      *Reloc,
  IN OUT CHAR8   *Fixup,
  IN OUT CHAR8   **FixupData,
  IN UINT64      Adjust
  );
  
//----------------------------------------------------------------------------
// Local prototypes
#define AMI_PORTING
#define PRINTED_GUID_BUFFER_SIZE  37  // including null-termination

typedef enum {
    FvTypeMain,
    FvTypeBootBlock,
    FvTypeNvRam,
    FvTypeCustom,
    FvTypeMax
} FLASH_FV_TYPE;

typedef struct _FLASH_AREA_EX FLASH_AREA_EX;

typedef EFI_STATUS (REFLASH_FUNCTION_EX)(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
);

struct _FLASH_AREA_EX
{
    UINT8 *BlockAddress;
    UINT8 *BackUpAddress;
    UINT32 RomFileOffset;
    UINTN Size;
    UINT32 BlockSize;
    FLASH_FV_TYPE Type;
    BOOLEAN Update;
    BOOLEAN TopSwapTrigger;
    REFLASH_FUNCTION_EX *BackUp;
    REFLASH_FUNCTION_EX *Program;
    EFI_STRING_ID BackUpStringId;
    EFI_STRING_ID ProgramStringId;
};

extern FLASH_AREA_EX *BlocksToUpdate;
extern VOID SetBackUpArea(VOID);
extern FLASH_PROTOCOL *Flash;
//----------------------------------------------------------------------------
// Local Variables

//----------------------------------------------------------------------------
// Function Definitions
/**
 * This function prints a GUID to a buffer
 *
 * @param Guid Pointer to a GUID to print.
 * @param Buffer Pointer to a user-provided buffer to print to
 * @param BufferLen Size of the Buffer
 * @param Uppercase If use upper case.
 *                      
 * @retval EFI_SUCCESS The GUID was printed.
 * @retval EFI_INVALID_PARAMETER The input was NULL.
 * @retval EFI_BUFFER_TOO_SMALL The input buffer was not big enough
 */
EFI_STATUS
PrintGuidToBuffer (
  IN EFI_GUID     *Guid,
  IN OUT UINT8    *Buffer,
  IN UINT32       BufferLen,
  IN BOOLEAN      Uppercase
  )
{
  if (Guid == NULL) {
    TRACE (( -1, "ERROR: PrintGuidToBuffer() called with a NULL value\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (BufferLen < PRINTED_GUID_BUFFER_SIZE) {
    TRACE (( -1, "ERORR: PrintGuidToBuffer() called with invalid buffer size\n"));
    return EFI_BUFFER_TOO_SMALL;
  }

  if (Uppercase) {
    Sprintf (Buffer, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", \
      Guid->Data1, Guid->Data2, Guid->Data3, \
      Guid->Data4[0], Guid->Data4[1], Guid->Data4[2], Guid->Data4[3], \
      Guid->Data4[4], Guid->Data4[5], Guid->Data4[6], Guid->Data4[7] );
  } else {
    Sprintf (Buffer, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
      Guid->Data1, Guid->Data2, Guid->Data3, \
      Guid->Data4[0], Guid->Data4[1], Guid->Data4[2], Guid->Data4[3], \
      Guid->Data4[4], Guid->Data4[5], Guid->Data4[6], Guid->Data4[7] );
  }

  return EFI_SUCCESS;
}

/**
 * This function return the Section Header Length.
 *
 * @param SectionHdr Pointer to Section Header.
 *                      
 * @retval UINT32 Section Header Length
 */
UINT32
GetSectionHeaderLength (
  IN EFI_COMMON_SECTION_HEADER *SectionHdr
  )
{
    if (GetLength(SectionHdr->Size) == 0xFFFFFF) return (sizeof(EFI_PE32_SECTION2));
    return sizeof(EFI_PE32_SECTION);
}

/**
 * This function update the FFS File Checksum.
 *
 * @param FfsHeader Pointer to FFS File Header.
 * @param FileSize Size of the File.
 *                      
 */
VOID 
UpdateFfsFileChecksum(
  IN    EFI_FFS_FILE_HEADER *FfsHeader, 
  IN    UINTN               FileSize
  )
{
	EFI_FFS_FILE_STATE State = FfsHeader->State;
	FfsHeader->IntegrityCheck.Checksum.Header = 0;
	FfsHeader->IntegrityCheck.Checksum.File = 0;
	FfsHeader->State = 0;
	FfsHeader->IntegrityCheck.Checksum.Header = CalculateCheckSum8 ((UINT8 *) FfsHeader, sizeof (EFI_FFS_FILE_HEADER));
	if (FfsHeader->Attributes & FFS_ATTRIB_CHECKSUM) {		
		FfsHeader->IntegrityCheck.Checksum.File = CalculateCheckSum8 ((UINT8 *) FfsHeader, FileSize);
	}else {           
		FfsHeader->IntegrityCheck.Checksum.File = FFS_FIXED_CHECKSUM;           
	}    
	FfsHeader->State = State ;
}

/**
 * This function Support routine for the PE/COFF Loader that reads a 
 *  buffer from a PE/COFF file
 *
 * @param FileHandle The handle to the PE/COFF file
 * @param FileOffset The offset, in bytes, into the file to read
 * @param ReadSize The number of bytes to read from the file starting at FileOffset
 * @param Buffer A pointer to the buffer to read the data into.
 *
 * @retval EFI_SUCCESS ReadSize bytes of data were read into Buffer from 
 *                      the PE/COFF file starting at FileOffset
 */
EFI_STATUS
FfsRebaseImageRead (
  IN     VOID    *FileHandle,
  IN     UINTN   FileOffset,
  IN OUT UINT32  *ReadSize,
  OUT    VOID    *Buffer
  )
{
  CHAR8   *Destination8;
  CHAR8   *Source8;
  UINT32  Length;

  Destination8  = Buffer;
  Source8       = (CHAR8 *) ((UINTN) FileHandle + FileOffset);
  Length        = *ReadSize;
  while (Length--) {
    *(Destination8++) = *(Source8++);
  }

  return EFI_SUCCESS;
}

/**
  Applies relocation fixups to a PE/COFF image that was loaded with PeCoffLoaderLoadImage().

  If the DestinationAddress field of ImageContext is 0, then use the ImageAddress field of
  ImageContext as the relocation base address.  Otherwise, use the DestinationAddress field
  of ImageContext as the relocation base address.  The caller must allocate the relocation
  fixup log buffer and fill in the FixupData field of ImageContext prior to calling this function.
  
  The ImageRead, Handle, PeCoffHeaderOffset,  IsTeImage, Machine, ImageType, ImageAddress, 
  ImageSize, DestinationAddress, RelocationsStripped, SectionAlignment, SizeOfHeaders, 
  DebugDirectoryEntryRva, EntryPoint, FixupDataSize, CodeView, PdbPointer, and FixupData of 
  the ImageContext structure must be valid prior to invoking this service.
    
  If ImageContext is NULL, then ASSERT().

  Note that if the platform does not maintain coherency between the instruction cache(s) and the data
  cache(s) in hardware, then the caller is responsible for performing cache maintenance operations
  prior to transferring control to a PE/COFF image that is loaded using this library.

  @param  ImageContext        The pointer to the image context structure that describes the PE/COFF
                              image that is being relocated.

  @retval RETURN_SUCCESS      The PE/COFF image was relocated.
                              Extended status information is in the ImageError field of ImageContext.
  @retval RETURN_LOAD_ERROR   The image in not a valid PE/COFF image.
                              Extended status information is in the ImageError field of ImageContext.
  @retval RETURN_UNSUPPORTED  A relocation record type is not supported.
                              Extended status information is in the ImageError field of ImageContext.

**/
RETURN_STATUS
EFIAPI
SimplePeCoffLoaderRelocateImage (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext, 
  IN     UINT32                        FixedOffset
  )
{
  RETURN_STATUS                         Status;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  EFI_IMAGE_DATA_DIRECTORY              *RelocDir;
  UINT64                                Adjust;
  EFI_IMAGE_BASE_RELOCATION             *RelocBase;
  EFI_IMAGE_BASE_RELOCATION             *RelocBaseEnd;
  UINT16                                *Reloc;
  // UINT16                                *RelocEnd;
  // CHAR8                                 *Fixup;
  CHAR8                                 *FixupBase;
  // UINT16                                *Fixup16;
  // UINT32                                *Fixup32;
  // UINT64                                *Fixup64;
  CHAR8                                 *FixupData;
  PHYSICAL_ADDRESS                      BaseAddress;
  // UINT32                                NumberOfRvaAndSizes;
  UINT16                                Magic;
  UINT32                                TeStrippedOffset;

  ASSERT (ImageContext != NULL);

  //
  // Assume success
  //
  ImageContext->ImageError = IMAGE_ERROR_SUCCESS;

  //
  // If there are no relocation entries, then we are done
  //
  if (ImageContext->RelocationsStripped) {
    // Applies additional environment specific actions to relocate fixups 
    // to a PE/COFF image if needed
    PeCoffLoaderRelocateImageExtraAction (ImageContext);  
    return RETURN_SUCCESS;
  }

  //
  // If the destination address is not 0, use that rather than the
  // image address as the relocation target.
  //
  if (ImageContext->DestinationAddress != 0) {
    BaseAddress = ImageContext->DestinationAddress;
  } else {
    BaseAddress = ImageContext->ImageAddress;
  }

  if (!(ImageContext->IsTeImage)) {
    UINT32                  NumberOfRvaAndSizes;
    Hdr.Pe32 = (EFI_IMAGE_NT_HEADERS32 *)((UINTN)ImageContext->ImageAddress + ImageContext->PeCoffHeaderOffset);
    TeStrippedOffset = 0;
    Magic = PeCoffLoaderGetPeHeaderMagicValue (Hdr);

    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset
      //
      Adjust = (UINT64)BaseAddress - Hdr.Pe32->OptionalHeader.ImageBase;
      if (Adjust != 0) {
        Hdr.Pe32->OptionalHeader.ImageBase = (UINT32)BaseAddress;
      }

      NumberOfRvaAndSizes = Hdr.Pe32->OptionalHeader.NumberOfRvaAndSizes;
      RelocDir  = &Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC];
    } else {
      //
      // Use PE32+ offset
      //
      Adjust = (UINT64) BaseAddress - Hdr.Pe32Plus->OptionalHeader.ImageBase;
      if (Adjust != 0) {
        Hdr.Pe32Plus->OptionalHeader.ImageBase = (UINT64)BaseAddress;
      }

      NumberOfRvaAndSizes = Hdr.Pe32Plus->OptionalHeader.NumberOfRvaAndSizes;
      RelocDir  = &Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC];
    }

    //
    // Find the relocation block
    // Per the PE/COFF spec, you can't assume that a given data directory
    // is present in the image. You have to check the NumberOfRvaAndSizes in
    // the optional header to verify a desired directory entry is there.
    //
    if ((NumberOfRvaAndSizes < EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC)) {
      RelocDir = NULL;
    }
  } else {
    Hdr.Te             = (EFI_TE_IMAGE_HEADER *)(UINTN)(ImageContext->ImageAddress);
    TeStrippedOffset   = (UINT32)Hdr.Te->StrippedSize - sizeof (EFI_TE_IMAGE_HEADER);
    Adjust             = (UINT64) (BaseAddress - (Hdr.Te->ImageBase + TeStrippedOffset));
    /* AMIPORTING BEGIN */
    if (FixedOffset)  Adjust = (UINT64)FixedOffset;
    /* AMIPORTING END */
    if (Adjust != 0) {
      Hdr.Te->ImageBase  = (UINT64) (BaseAddress - TeStrippedOffset);
    }
    //
    // Find the relocation block
    //
    RelocDir = &Hdr.Te->DataDirectory[0];
  }

  if ((RelocDir != NULL) && (RelocDir->Size > 0)) {
    RelocBase = (EFI_IMAGE_BASE_RELOCATION *) PeCoffLoaderImageAddress (ImageContext, RelocDir->VirtualAddress, TeStrippedOffset);
    RelocBaseEnd = (EFI_IMAGE_BASE_RELOCATION *) PeCoffLoaderImageAddress (ImageContext,
                                                                            RelocDir->VirtualAddress + RelocDir->Size - 1,
                                                                            TeStrippedOffset
                                                                            );
    if (RelocBase == NULL || RelocBaseEnd == NULL) {
      return RETURN_LOAD_ERROR;
    }
  } else {
    //
    // Set base and end to bypass processing below.
    //
    RelocBase = RelocBaseEnd = NULL;    
  }

  //
  // If Adjust is not zero, then apply fix ups to the image
  //
  if (Adjust != 0) {
    //
    // Run the relocation information and apply the fixups
    //
    FixupData = ImageContext->FixupData;
    while (RelocBase < RelocBaseEnd) {
      UINT16            *RelocEnd, *Fixup16;
      CHAR8             *Fixup;
      UINT32            *Fixup32;
      UINT64            *Fixup64;

      Reloc     = (UINT16 *) ((CHAR8 *) RelocBase + sizeof (EFI_IMAGE_BASE_RELOCATION));
      //
      // Add check for RelocBase->SizeOfBlock field.
      //
      if ((RelocBase->SizeOfBlock == 0) || (RelocBase->SizeOfBlock > RelocDir->Size)) {
        ImageContext->ImageError = IMAGE_ERROR_FAILED_RELOCATION;
        return RETURN_LOAD_ERROR;
      }

      RelocEnd  = (UINT16 *) ((CHAR8 *) RelocBase + RelocBase->SizeOfBlock);
      FixupBase = PeCoffLoaderImageAddress (ImageContext, RelocBase->VirtualAddress, TeStrippedOffset);
      if (FixupBase == NULL) {
        return RETURN_LOAD_ERROR;
      }  

      //
      // Run this relocation record
      //
      while (Reloc < RelocEnd) {

        Fixup = FixupBase + (*Reloc & 0xFFF);
        switch ((*Reloc) >> 12) {
        case EFI_IMAGE_REL_BASED_ABSOLUTE:
          break;

        case EFI_IMAGE_REL_BASED_HIGH:
          Fixup16   = (UINT16 *) Fixup;
          *Fixup16 = (UINT16) (*Fixup16 + ((UINT16) ((UINT32) Adjust >> 16)));
          if (FixupData != NULL) {
            *(UINT16 *) FixupData = *Fixup16;
            FixupData             = FixupData + sizeof (UINT16);
          }
          break;

        case EFI_IMAGE_REL_BASED_LOW:
          Fixup16   = (UINT16 *) Fixup;
          *Fixup16  = (UINT16) (*Fixup16 + (UINT16) Adjust);
          if (FixupData != NULL) {
            *(UINT16 *) FixupData = *Fixup16;
            FixupData             = FixupData + sizeof (UINT16);
          }
          break;

        case EFI_IMAGE_REL_BASED_HIGHLOW:
          Fixup32   = (UINT32 *) Fixup;
          *Fixup32  = *Fixup32 + (UINT32) Adjust;
          if (FixupData != NULL) {
            FixupData             = ALIGN_POINTER (FixupData, sizeof (UINT32));
            *(UINT32 *)FixupData  = *Fixup32;
            FixupData             = FixupData + sizeof (UINT32);
          }
          break;

        case EFI_IMAGE_REL_BASED_DIR64:
          Fixup64 = (UINT64 *) Fixup;
          *Fixup64 = *Fixup64 + (UINT64) Adjust;
          if (FixupData != NULL) {
            FixupData = ALIGN_POINTER (FixupData, sizeof(UINT64));
            *(UINT64 *)(FixupData) = *Fixup64;
            FixupData = FixupData + sizeof(UINT64);
          }
          break;

        default:
          //
          // The common code does not handle some of the stranger IPF relocations
          // PeCoffLoaderRelocateImageEx () adds support for these complex fixups
          // on IPF and is a No-Op on other architectures.
          //
          Status = PeCoffLoaderRelocateImageEx (Reloc, Fixup, &FixupData, Adjust);
          if (RETURN_ERROR (Status)) {
            ImageContext->ImageError = IMAGE_ERROR_FAILED_RELOCATION;
            return Status;
          }
        }

        //
        // Next relocation record
        //
        Reloc += 1;
      }

      //
      // Next reloc block
      //
      RelocBase = (EFI_IMAGE_BASE_RELOCATION *) RelocEnd;
    }

    //
    // Adjust the EntryPoint to match the linked-to address
    //
    if (ImageContext->DestinationAddress != 0) {
       ImageContext->EntryPoint -= (UINT64) ImageContext->ImageAddress;
       ImageContext->EntryPoint += (UINT64) ImageContext->DestinationAddress;
    }
  }
  
  // Applies additional environment specific actions to relocate fixups 
  // to a PE/COFF image if needed
  PeCoffLoaderRelocateImageExtraAction (ImageContext);
  
  return RETURN_SUCCESS;
}

/**
 * This function determines if a file is XIP and should be rebased.  It will
 * rebase any PE32 sections found in the file using the base address.
 *
 * @param FvInfo A pointer to FV_INFO struture.
 * @param FileName Ffs File PathName
 * @param FfsFile A pointer to Ffs file image.
 * @param XipOffset The offset address to use for rebasing the XIP file image.
 * @param FvMapFile FvMapFile to record the function address in one Fvimage
 *                      
 * @retval EFI_SUCCESS The image was properly rebased.
 * @retval EFI_INVALID_PARAMETER An input parameter is invalid.
 * @retval EFI_ABORTED An error occurred while rebasing the input file image.
 * @retval EFI_OUT_OF_RESOURCES Could not allocate a required resource.
 * @retval EFI_NOT_FOUND No compressed sections could be found.
 */
EFI_STATUS
FfsRebase ( 
  IN OUT EFI_FFS_FILE_HEADER    *FfsFile,
  IN EFI_PHYSICAL_ADDRESS       BaseAddress,
  IN UINT32                     FixedOffset
  )
{
  EFI_STATUS                            Status;
  PE_COFF_LOADER_IMAGE_CONTEXT          ImageContext;
  PE_COFF_LOADER_IMAGE_CONTEXT          OrigImageContext;  
  EFI_PHYSICAL_ADDRESS                  XipBase;
  EFI_PHYSICAL_ADDRESS                  NewPe32BaseAddress;
  UINTN                                 Index;
  EFI_FILE_SECTION_POINTER              CurrentPe32Section;
  EFI_FFS_FILE_STATE                    SavedState;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       *ImgHdr;
  EFI_TE_IMAGE_HEADER                   *TEImageHeader;
  UINT8                                 *MemoryImagePointer;
  EFI_IMAGE_SECTION_HEADER              *SectionHeader;
  CHAR8                                 FileName [80];
  UINT8                                 *PeFileBuffer;
  UINT32                                PeFileSize;
  // CHAR8                                 *PdbPointer;
  UINT32                                FfsHeaderSize;
  UINT32                                CurSecHdrSize;
  BOOLEAN                               RelocationsAreStripped = FALSE;

  Index              = 0;  
  MemoryImagePointer = NULL;
  TEImageHeader      = NULL;
  ImgHdr             = NULL;
  SectionHeader      = NULL;
  PeFileBuffer       = NULL;

  //
  // Verify input parameters
  //
  if (FfsFile == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Do some cursory checks on the FFS file contents
  //
  PrintGuidToBuffer (&FfsFile->Name, FileName, sizeof (FileName), TRUE);
  Status = VerifyFfsFile (FfsFile);
  if (EFI_ERROR (Status)) {
     TRACE (( -1, "file does not appear to be a valid FFS file, cannot be rebased.\n", FileName, Status));
    return EFI_INVALID_PARAMETER;
  }

  XipBase = BaseAddress;

  //
  // We only process files potentially containing PE32 sections.
  //
  switch (FfsFile->Type) {
    case EFI_FV_FILETYPE_SECURITY_CORE:
    case EFI_FV_FILETYPE_PEI_CORE:
    case EFI_FV_FILETYPE_PEIM:
    case EFI_FV_FILETYPE_COMBINED_PEIM_DRIVER:
    case EFI_FV_FILETYPE_DRIVER:
    case EFI_FV_FILETYPE_DXE_CORE:
      break;
    case EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE:
      //
      // Rebase the inside FvImage.
      //
      //GetChildFvFromFfs (FvInfo, FfsFile, XipOffset);

      //
      // Search PE/TE section in FV sectin.
      //
      break;
    default:
      return EFI_SUCCESS;
  }

  FfsHeaderSize = GetLength(FfsFile->Size);
  //
  // Rebase each PE32 section
  //
  Status      = EFI_SUCCESS;
  for (Index = 1;; Index++) {
#ifdef AMI_PORTING 
	// Add fixed attribute if file with the stripped relocations has been rebased.
    RelocationsAreStripped = FALSE;
#endif  
    //
    // Init Value
    //
    NewPe32BaseAddress = 0;
    
    //
    // Find Pe Image
    //
    Status = GetSectionByType (FfsFile, EFI_SECTION_PE32, Index, &CurrentPe32Section);
    if (EFI_ERROR (Status)) {
      break;
    }
    CurSecHdrSize = GetSectionHeaderLength(CurrentPe32Section.CommonHeader);

    //
    // Initialize context
    //
    MemSet (&ImageContext, sizeof (ImageContext), 0);
    ImageContext.Handle     = (VOID *) ((UINTN) CurrentPe32Section.Pe32Section + CurSecHdrSize);
    ImageContext.ImageRead  = (PE_COFF_LOADER_READ_FILE) FfsRebaseImageRead;
    Status                  = PeCoffLoaderGetImageInfo (&ImageContext);
    if (EFI_ERROR (Status)) {
      TRACE (( -1, "Invalid PeImage - The input file is %s and the return status is %x\n", FileName, Status));
      // SMDbgPrint ("Invalid PeImage - The input file is %g and the return status is %r\n", FfsFile->Name, Status);
      return Status;
    }

    // if ( (ImageContext.Machine == EFI_IMAGE_MACHINE_ARMT) ||
    //      (ImageContext.Machine == EFI_IMAGE_MACHINE_AARCH64) ) {
    //   mArm = TRUE;
    // }

    //
    // Keep Image Context for PE image in FV
    //
    MemCpy (&OrigImageContext, &ImageContext, sizeof (ImageContext));
    
    //
    // Get File PdbPointer
    //
    // PdbPointer = PeCoffLoaderGetPdbPointer (ImageContext.Handle);

    //
    // Get PeHeader pointer
    //
    ImgHdr = (EFI_IMAGE_OPTIONAL_HEADER_UNION *)((UINTN) CurrentPe32Section.Pe32Section + CurSecHdrSize + ImageContext.PeCoffHeaderOffset);

    //
    // Calculate the PE32 base address, based on file type
    //
    switch (FfsFile->Type) {
      case EFI_FV_FILETYPE_SECURITY_CORE:
      case EFI_FV_FILETYPE_PEI_CORE:
      case EFI_FV_FILETYPE_PEIM:
      case EFI_FV_FILETYPE_COMBINED_PEIM_DRIVER:
        //
        // Check if section-alignment and file-alignment match or not
        //
        if ((ImgHdr->Pe32.OptionalHeader.SectionAlignment != ImgHdr->Pe32.OptionalHeader.FileAlignment)) {
          //
          // Xip module has the same section alignment and file alignment.
          //
          TRACE (( -1, "Invalid - Section-Alignment and File-Alignment do not match : %s.\n", FileName));
          //  SMDbgPrint ("Invalid - Section-Alignment and File-Alignment do not match : %g.\n", FfsFile->Name);
          return EFI_ABORTED;
        }
        //
        // PeImage has no reloc section. It will try to get reloc data from the original EFI image. 
        //
        if (ImageContext.RelocationsStripped) {
#ifdef AMI_PORTING 
	// Add fixed attribute if file with the stripped relocations has been rebased.
          RelocationsAreStripped = TRUE;
#endif
          /* AMIPORTING BEGIN */
          PeFileSize = GetLength(FfsFile->Size);
          PeFileBuffer = (UINT8 *) Malloc (PeFileSize);
          MemCpy (PeFileBuffer, (UINT8*)FfsFile, PeFileSize);
          /* AMIPORTING END */
          //
          // Handle pointer to the original efi image.
          //
          ImageContext.Handle = PeFileBuffer;
          Status              = PeCoffLoaderGetImageInfo (&ImageContext);
          if (EFI_ERROR (Status)) {
            TRACE (( -1, "Invalid PeImage - The input file is %s and the return status is %x\n", FileName, Status));
            // SMDbgPrint( "Invalid PeImage - The input file is %g and the return status is %r\n", FfsFile->Name, Status);
            return Status;
          }
          ImageContext.RelocationsStripped = FALSE;
        }

        NewPe32BaseAddress = XipBase + (UINTN) CurrentPe32Section.Pe32Section + CurSecHdrSize - (UINTN)FfsFile;
        break;

      case EFI_FV_FILETYPE_DRIVER:
      case EFI_FV_FILETYPE_DXE_CORE:
        //
        // Check if section-alignment and file-alignment match or not
        //
        if ((ImgHdr->Pe32.OptionalHeader.SectionAlignment != ImgHdr->Pe32.OptionalHeader.FileAlignment)) {
          //
          // Xip module has the same section alignment and file alignment.
          //
          TRACE (( -1, "Invalid - Section-Alignment and File-Alignment do not match : %s.\n", FileName));
          // SMDbgPrint ( "Invalid - Section-Alignment and File-Alignment do not match : %g.\n", FfsFile->Name);
          return EFI_ABORTED;
        }
        NewPe32BaseAddress = XipBase + (UINTN) CurrentPe32Section.Pe32Section + CurSecHdrSize - (UINTN)FfsFile;
        break;

      default:
        //
        // Not supported file type
        //
        return EFI_SUCCESS;
    }
    
    //
    // Relocation doesn't exist
    //
    if (ImageContext.RelocationsStripped) {
      TRACE(( -1, "Warning - The file %s has no .reloc section.\n", FileName));
      continue;
    }

    //
    // Relocation exist and rebase
    //
    //
    // Load and Relocate Image Data
    //
    MemoryImagePointer = (UINT8 *) Malloc ((UINTN) ImageContext.ImageSize + ImageContext.SectionAlignment);
    if (MemoryImagePointer == NULL) {
      TRACE (( -1, "Resource - memory cannot be allocated on rebase of %s\n", FileName));
      // SMDbgPrint( "Resource - memory cannot be allocated on rebase of %g\n", FfsFile->Name);
      return EFI_OUT_OF_RESOURCES;
    }
    MemSet ((VOID *) MemoryImagePointer, (UINTN) ImageContext.ImageSize + ImageContext.SectionAlignment, 0);
    ImageContext.ImageAddress = ((UINTN) MemoryImagePointer + ImageContext.SectionAlignment - 1) & (~((UINTN) ImageContext.SectionAlignment - 1));
    
    Status =  PeCoffLoaderLoadImage (&ImageContext);
    if (EFI_ERROR (Status)) {
      TRACE (( -1, "Invalid - LocateImage() call failed on rebase of PE image %s.\n", FileName));
      // SMDbgPrint( "Invalid - LocateImage() call failed on rebase of PE image %g.\n", FfsFile->Name);
      pBS->FreePool ((VOID *) MemoryImagePointer);
      return Status;
    }
         
    ImageContext.DestinationAddress = NewPe32BaseAddress;
    Status                          = PeCoffLoaderRelocateImage (&ImageContext);
    if (EFI_ERROR (Status)) {
      TRACE (( -1, "Invalid - RelocateImage() call failed on rebase of PE image %s.\n", FileName));
      // SMDbgPrint( "Invalid - RelocateImage() call failed on rebase of PE image %g.\n", FfsFile->Name);
      pBS->FreePool ((VOID *) MemoryImagePointer);
      return Status;
    }

    //
    // Copy Relocated data to raw image file.
    //
    SectionHeader = (EFI_IMAGE_SECTION_HEADER *) (
                       (UINTN) ImgHdr +
                       sizeof (UINT32) + 
                       sizeof (EFI_IMAGE_FILE_HEADER) +  
                       ImgHdr->Pe32.FileHeader.SizeOfOptionalHeader
                       );
    
    for (Index = 0; Index < ImgHdr->Pe32.FileHeader.NumberOfSections; Index ++, SectionHeader ++) {
      MemCpy (
        (UINT8 *) CurrentPe32Section.Pe32Section + CurSecHdrSize + SectionHeader->PointerToRawData, 
        (VOID*) (UINTN) (ImageContext.ImageAddress + SectionHeader->VirtualAddress), 
        SectionHeader->SizeOfRawData
        );
    }

    pBS->FreePool ((VOID *) MemoryImagePointer);
    MemoryImagePointer = NULL;
    if (PeFileBuffer != NULL) {
      pBS->FreePool (PeFileBuffer);
      PeFileBuffer = NULL;
    }
    
    //
    // Update Image Base Address
    //
    if (ImgHdr->Pe32.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      ImgHdr->Pe32.OptionalHeader.ImageBase = (UINT32) NewPe32BaseAddress;
    } else if (ImgHdr->Pe32Plus.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      ImgHdr->Pe32Plus.OptionalHeader.ImageBase = NewPe32BaseAddress;
    } else {
      TRACE (( -1, "Invalid - unknown PE magic signature %X in PE32 image %s\n",
        ImgHdr->Pe32.OptionalHeader.Magic,
        FileName
        ));
      // SMDbgPrint( "Invalid - unknown PE magic signature %X in PE32 image %g\n",
      //   ImgHdr->Pe32.OptionalHeader.Magic,
      //   FfsFile->Name
      //   );
      return EFI_ABORTED;
    }

    //
    // Now update file checksum
    //
#ifdef AMI_PORTING
	  // Add fixed attribute if file with the stripped relocations has been rebased.
	if (RelocationsAreStripped){
	  FfsFile->Attributes |= FFS_ATTRIB_FIXED;
	  UpdateFfsFileChecksum(FfsFile, GetLength (FfsFile->Size));
    }else
#endif
    if (FfsFile->Attributes & FFS_ATTRIB_CHECKSUM) {
      SavedState  = FfsFile->State;
      FfsFile->IntegrityCheck.Checksum.File = 0;
      FfsFile->State                        = 0;
      FfsFile->IntegrityCheck.Checksum.File = CalculateCheckSum8 (
                                                (UINT8 *) ((UINT8 *)FfsFile + FfsHeaderSize),
                                                GetLength (FfsFile->Size) - FfsHeaderSize
                                                );
      FfsFile->State = SavedState;
    }
  }

  if (FfsFile->Type != EFI_FV_FILETYPE_SECURITY_CORE &&
      FfsFile->Type != EFI_FV_FILETYPE_PEI_CORE &&
      FfsFile->Type != EFI_FV_FILETYPE_PEIM &&
      FfsFile->Type != EFI_FV_FILETYPE_COMBINED_PEIM_DRIVER &&
      FfsFile->Type != EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE
      ) {
    //
    // Only Peim code may have a TE section
    //
    return EFI_SUCCESS;
  }
  
  //
  // Now process TE sections
  //
  for (Index = 1;; Index++) {
#ifdef AMI_PORTING 
	// Add fixed attribute if file with the stripped relocations has been rebased.
    RelocationsAreStripped = FALSE;
#endif  
    NewPe32BaseAddress = 0;
    
    //
    // Find Te Image
    //
    Status = GetSectionByType (FfsFile, EFI_SECTION_TE, Index, &CurrentPe32Section);
    if (EFI_ERROR (Status)) {
      break;
    }

    CurSecHdrSize = GetSectionHeaderLength(CurrentPe32Section.CommonHeader);
    //
    // Calculate the TE base address, the FFS file base plus the offset of the TE section less the size stripped off
    // by GenTEImage
    //
    TEImageHeader = (EFI_TE_IMAGE_HEADER *) ((UINT8 *) CurrentPe32Section.Pe32Section + CurSecHdrSize);

    //
    // Initialize context, load image info.
    //
    MemSet (&ImageContext, sizeof (ImageContext), 0);
    ImageContext.Handle     = (VOID *) TEImageHeader;
    ImageContext.ImageRead  = (PE_COFF_LOADER_READ_FILE) FfsRebaseImageRead;
    Status                  = PeCoffLoaderGetImageInfo (&ImageContext);
    if (EFI_ERROR (Status)) {
      TRACE (( -1, "Invalid TeImage - The input file is %s and the return status is %x.\n", FileName, Status));
      // SMDbgPrint( "Invalid TeImage - The input file is %g and the return status is %r.\n", FfsFile->Name, Status);
      return Status;
    }

    // if ( (ImageContext.Machine == EFI_IMAGE_MACHINE_ARMT) ||
    //      (ImageContext.Machine == EFI_IMAGE_MACHINE_AARCH64) ) {
    //   mArm = TRUE;
    // }

    //
    // Keep Image Context for TE image in FV
    //
    MemCpy (&OrigImageContext, &ImageContext, sizeof (ImageContext));

    //
    // Get File PdbPointer
    //
    // PdbPointer = PeCoffLoaderGetPdbPointer (ImageContext.Handle);

    //
    // Set new rebased address.
    //
    /* AMIPORTING BEGIN */
    NewPe32BaseAddress = XipBase + (UINTN) TEImageHeader + sizeof (EFI_TE_IMAGE_HEADER) \
                         - TEImageHeader->StrippedSize - (UINTN) FfsFile;
//-    NewPe32BaseAddress = XipBase + (UINTN) TEImageHeader - (UINTN) FfsFile;
    /* AMIPORTING END */
    //
    // if reloc is stripped, try to get the original efi image to get reloc info.
    //
    if (ImageContext.RelocationsStripped) {
#ifdef AMI_PORTING 
	// Add fixed attribute if file with the stripped relocations has been rebased.
          RelocationsAreStripped = TRUE;
#endif
        /* AMIPORTING BEGIN */
        PeFileSize = GetLength(FfsFile->Size);
        PeFileBuffer = (UINT8 *) Malloc (PeFileSize);
        MemCpy (PeFileBuffer, (UINT8*)FfsFile, PeFileSize);
        /* AMIPORTING END */
        //
        // Append reloc section into TeImage
        //
        ImageContext.Handle = PeFileBuffer;
        Status              = PeCoffLoaderGetImageInfo (&ImageContext);
        if (EFI_ERROR (Status)) {
          TRACE (( -1, "Invalid TeImage - The input file is %s and the return status is %x.\n", FileName, Status));
          // SMDbgPrint( "Invalid TeImage - The input file is %g and the return status is %r.\n", FfsFile->Name, Status);
          return Status;
        }
        ImageContext.RelocationsStripped = FALSE;
    }
    //
    // Relocation doesn't exist
    //
    if (ImageContext.RelocationsStripped) {
      TRACE(( -1, "Warning - The file %s has no .reloc section.\n", FileName));
      // SMDbgPrint("Warning - The file %g has no .reloc section.\n", FfsFile->Name);
      continue;
    }

    //
    // Relocation exist and rebase
    //
    //
    // Load and Relocate Image Data
    //
    MemoryImagePointer = (UINT8 *) Malloc ((UINTN) ImageContext.ImageSize + ImageContext.SectionAlignment);
    if (MemoryImagePointer == NULL) {
      TRACE (( -1, "Resource - memory cannot be allocated on rebase of TE image %s.\n", FileName));
      // SMDbgPrint( "Resource - memory cannot be allocated on rebase of TE image %g.\n", FfsFile->Name);
      return EFI_OUT_OF_RESOURCES;
    }
    MemSet ((VOID *) MemoryImagePointer, (UINTN) ImageContext.ImageSize + ImageContext.SectionAlignment, 0);
    // ImageContext.ImageAddress = ((UINTN) MemoryImagePointer + ImageContext.SectionAlignment - 1) & (~((UINTN) ImageContext.SectionAlignment - 1));
    ImageContext.ImageAddress = (PHYSICAL_ADDRESS)MemoryImagePointer;
    Status =  PeCoffLoaderLoadImage (&ImageContext);
    if (EFI_ERROR (Status)) {
      TRACE (( -1, "Invalid - LocateImage() call failed on rebase of TE image %s.\n", FileName));
      // SMDbgPrint( "[%r(%x)] LocateImage() call failed on rebase of TE image %g.\n", Status, ImageContext.ImageError ,FfsFile->Name);
      pBS->FreePool ((VOID *) MemoryImagePointer);
      return Status;
    }
    //
    // Reloacate TeImage
    // 
    ImageContext.DestinationAddress = NewPe32BaseAddress;
    Status                          = SimplePeCoffLoaderRelocateImage (&ImageContext, FixedOffset);
    if (EFI_ERROR (Status)) {
      TRACE (( -1, "Invalid - RelocateImage() call failed on rebase of TE image %s.\n", FileName));
      // SMDbgPrint( "[%r(%x)] RelocateImage() call failed on rebase of TE image %g.\n", Status, ImageContext.ImageError ,FfsFile->Name);
      pBS->FreePool ((VOID *) MemoryImagePointer);
      return Status;
    }
    
    //
    // Copy the relocated image into raw image file.
    //
    SectionHeader = (EFI_IMAGE_SECTION_HEADER *) (TEImageHeader + 1);
    for (Index = 0; Index < TEImageHeader->NumberOfSections; Index ++, SectionHeader ++) {
      if (!ImageContext.IsTeImage) {
        MemCpy (
          (UINT8 *) TEImageHeader + sizeof (EFI_TE_IMAGE_HEADER) - TEImageHeader->StrippedSize + SectionHeader->PointerToRawData, 
          (VOID*) (UINTN) (ImageContext.ImageAddress + SectionHeader->VirtualAddress), 
          SectionHeader->SizeOfRawData
          );
      } else {
        MemCpy (
          (UINT8 *) TEImageHeader + sizeof (EFI_TE_IMAGE_HEADER) - TEImageHeader->StrippedSize + SectionHeader->PointerToRawData, 
          (VOID*) (UINTN) (ImageContext.ImageAddress + sizeof (EFI_TE_IMAGE_HEADER) - TEImageHeader->StrippedSize + SectionHeader->VirtualAddress), 
          SectionHeader->SizeOfRawData
          );
      }
    }
    
    //
    // Free the allocated memory resource
    //
    pBS->FreePool ((VOID *) MemoryImagePointer);
    MemoryImagePointer = NULL;
    if (PeFileBuffer != NULL) {
      pBS->FreePool (PeFileBuffer);
      PeFileBuffer = NULL;
    }
    
    //
    // Update Image Base Address
    //
    TEImageHeader->ImageBase = NewPe32BaseAddress;

    //
    // Now update file checksum
    //
#ifdef AMI_PORTING
	  // Add fixed attribute if file with the stripped relocations has been rebased.
	if (RelocationsAreStripped){
	  FfsFile->Attributes |= FFS_ATTRIB_FIXED;
	  UpdateFfsFileChecksum(FfsFile, GetLength (FfsFile->Size));
    }else
#endif
    if (FfsFile->Attributes & FFS_ATTRIB_CHECKSUM) {
      SavedState  = FfsFile->State;
      FfsFile->IntegrityCheck.Checksum.File = 0;
      FfsFile->State                        = 0;
      FfsFile->IntegrityCheck.Checksum.File = CalculateCheckSum8 (
                                                (UINT8 *)((UINT8 *)FfsFile + FfsHeaderSize),
                                                GetLength (FfsFile->Size) - FfsHeaderSize
                                                );
      FfsFile->State = SavedState;
    }
  }
 
  return EFI_SUCCESS;
}

/**
 * This function Rebase the given ROM image
 *
 * @param InputBuff Pointer to Buffer containing the volume to be rebased
 * @param OutputBuff Pointer to Rebased Volume
 * @param BaseAddress Base address of the volume
 * @param FixedOffset Fixed Offset from the Base address
 *                      
 * @retval EFI_SUCCESS on Success 
 * @retval EFI_ABORTED on Faulure
 */
EFI_STATUS 
Rebase  (
    UINT8       *InputBuff,
    UINT8       **OutputBuff,
    UINT32      BaseAddress,
    UINT32      FixedOffset
)
{
	UINT32                      FvSize =0;
    // BOOLEAN                     ErasePolarity	= FALSE;
	EFI_FIRMWARE_VOLUME_HEADER  VolumeHeader;
	EFI_FV_BLOCK_MAP_ENTRY      BlockMap;
	// UINTN                       Signature[2], BytesRead;
	UINTN                       BytesRead;
	EFI_FIRMWARE_VOLUME_HEADER  *FvImage;
	EFI_FFS_FILE_HEADER         *CurrentFile;
	EFI_STATUS                  Status;
	EFI_PHYSICAL_ADDRESS        CurrentFileBaseAddress;
	EFI_GUID                    EfiFfsVolumeTopFileGuid = EFI_FFS_VOLUME_TOP_FILE_GUID;
    UINT8                       gMsgBuffer[80];

	FvImage = NULL;
	/* AMIPORTING BEGIN */
	// ReadHeader function is modified and reused, as it accepts the File pointer as i/p parameter

	MemCpy(&VolumeHeader,InputBuff, sizeof (EFI_FIRMWARE_VOLUME_HEADER) - sizeof (EFI_FV_BLOCK_MAP_ENTRY));
	BytesRead     = sizeof (EFI_FIRMWARE_VOLUME_HEADER) - sizeof (EFI_FV_BLOCK_MAP_ENTRY);

	// Signature[0]  = VolumeHeader.Signature;
	// Signature[1]  = 0;
	// if (VolumeHeader.Attributes & EFI_FVB2_ERASE_POLARITY) 
	// 	ErasePolarity = TRUE;

	//do {
	MemCpy(&BlockMap,InputBuff+BytesRead,sizeof (EFI_FV_BLOCK_MAP_ENTRY));

    if (BlockMap.NumBlocks != 0) {
      FvSize += BlockMap.NumBlocks * BlockMap.Length;
    }

  //} while (!(BlockMap.NumBlocks == 0 && BlockMap.BlockLength == 0));

    if (VolumeHeader.FvLength != FvSize) {
      TRACE ((-1, " volume size not consistant with block maps.\n"));
      // SMDbgPrint (" volume size not consistant with block maps.\n");
      return EFI_ABORTED;
    }
    
    /* AMIPORTING END */
    
    FvImage = Malloc (FvSize);
    if (FvImage == NULL) {
      TRACE ((-1, "application error - memory allocation failed.\n"));
      // SMDbgPrint ("application error - memory allocation failed.\n");
      return EFI_ABORTED;
    }
    
    MemCpy(FvImage, InputBuff, FvSize);
    
    InitializeFvLib (FvImage, FvSize);
    
    // Get the first file
    Status = GetNextFile (NULL, &CurrentFile);
    if (EFI_ERROR (Status)) {
      TRACE ((-1, "cannot find the first file in the FV image.\n"));
      // SMDbgPrint ("cannot find the first file in the FV image. :: %r\n", Status);
      return EFI_ABORTED;
    }
    
    
    // Check if each file should be rebased
    //
    while (CurrentFile != NULL) {
      //
      // Rebase this file
      //
      CurrentFileBaseAddress  = BaseAddress + ((UINTN) CurrentFile - (UINTN) FvImage);
      Status                  = FfsRebase (CurrentFile, CurrentFileBaseAddress, FixedOffset);
    
      if (EFI_ERROR (Status)) {
        switch (Status) {
    
        case EFI_INVALID_PARAMETER:
          TRACE ((-1, "invalid parameter passed to FfsRebase.\n"));
          // SMDbgPrint ("invalid parameter passed to FfsRebase.\n");
          break;
    
        case EFI_ABORTED:
          TRACE ((-1, "error detected while rebasing -- aborted.\n"));
          // SMDbgPrint ("error detected while rebasing -- aborted.\n");
          break;
    
        case EFI_OUT_OF_RESOURCES:
          TRACE ((-1, "FfsRebase could not allocate required resources.\n"));
          // SMDbgPrint ("FfsRebase could not allocate required resources.\n");
          break;
    
        case EFI_NOT_FOUND:
          TRACE ((-1, "FfsRebase could not locate a PE32 section.\n"));
          // SMDbgPrint ("FfsRebase could not locate a PE32 section.\n");
          break;
    
        default:
          Sprintf(gMsgBuffer, "FfsRebase returned unknown status = 0x%08X\n", Status);
          TRACE ((-1, "%s", gMsgBuffer));
          // SMDbgPrint ("%s", gMsgBuffer);
          break;
        }
    
        return EFI_ABORTED;
      }
      //
      // Get the next file
      //
      Status = GetNextFile (CurrentFile, &CurrentFile);
      if (EFI_ERROR (Status)) {
        TRACE ((-1, "cannot find the next file in the FV image.\n"));
        // SMDbgPrint ("cannot find the next file in the FV image.\n");
        return EFI_ABORTED;
      }
      //
      // Rebase Completed if Next file is the Volume Top File.
      //
      if (!guidcmp((UINT8*)&CurrentFile->Name, &EfiFfsVolumeTopFileGuid)) break;
    }
    
    *OutputBuff = (UINT8 *)Malloc(FvSize);
    MemCpy (*OutputBuff, FvImage, FvSize);
    pBS->FreePool(FvImage);	
    return EFI_SUCCESS;
}

/**
 * This function performs backup of certain flash area
 * 
 * @param Block          Pointer to the flash area 
 * @param BlockNumber    Block number within flash area
 * 
 * @retval EFI_SUCCESS   Execution went successfully
 * @retval other         error occured during execution
 * 
 **/
EFI_STATUS AmiFlashBackUpEx(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
)
{
    UINT8 *Source;
    UINT8 *Destination;
    VOID *Tmp;
    EFI_STATUS Status;
    EFI_TPL OldTpl;
    static UINT8 *Buffer; 

    Status = pBS->AllocatePool(EfiBootServicesData, Block->BlockSize, &Tmp);
    if(EFI_ERROR(Status))
        return Status;

    Source = Block->BlockAddress + BlockNumber * Block->BlockSize;
    Destination = Block->BackUpAddress + BlockNumber * Block->BlockSize;

    if(Block->Type == FvTypeBootBlock) SetBackUpArea();

    Status = Flash->Read(Source, Block->BlockSize, Tmp);
    if(EFI_ERROR(Status)) {
        pBS->FreePool(Tmp);
        return Status;
    }
    if (Block->BackUpAddress == (UINT8*)AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_ADDRESS) { 
        EFI_FIRMWARE_VOLUME_HEADER *FvHdr = (EFI_FIRMWARE_VOLUME_HEADER *)Block->BlockAddress;
        // Check whether the Header of FV_RECOVERY is valid, if not valid, 
        // the scenario could be the FV_RECOVERY upddate is terminated in previous boot, 
        // So, No need to backup it again.
        if (FvHdr->Signature != EFI_FVH_SIGNATURE) {
            pBS->FreePool(Tmp);
            return EFI_SUCCESS;
        }
#if 0   // [ EIP444358 ]
        // Header of FV_RECOVERY is valid, rebase the FV_RECOVERY to be with
        // FV_RECOVERY_BACKUP Address
        if (Destination == (UINT8*)AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_ADDRESS) {
            Status = Rebase((UINT8*)Block->BlockAddress, (UINT8**)&Buffer, \
                            (UINT32)Block->BackUpAddress, \
                            (UINT32)(Block->BackUpAddress - Block->BlockAddress));
            if(EFI_ERROR(Status)) Buffer = NULL;
        }
        if (Buffer != NULL) {
            MemCpy (Tmp, Buffer + BlockNumber * Block->BlockSize, Block->BlockSize);
        } 
#endif  // [ EIP444358 ]
    }
    //save to backup address
    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    Status = Flash->Update(Destination, Block->BlockSize, Tmp);
    pBS->RestoreTPL(OldTpl);
    pBS->FreePool(Tmp);
    
    // If end of FV_RECOVERY backup, invalidate the FV_RECOVERY header
    if (Destination == (UINT8*)AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_ADDRESS + \
                            (AMI_ROM_LAYOUT_FV_RECOVERY_SIZE - Block->BlockSize)) {
        static UINT32 FlashEraser = (FLASH_ERASE_POLARITY) ? 0 : 0xffffffff;
        EFI_FIRMWARE_VOLUME_HEADER *Fv =(EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)AMI_ROM_LAYOUT_FV_RECOVERY_ADDRESS;
        Flash->Write(&(Fv->Signature), sizeof(UINT32), &FlashEraser);
        if (Buffer != NULL) pBS->FreePool(Buffer);
    }                                
    // If end of FV_BB backup, swap the update priority back.
    if (Destination == (UINT8*)AMI_ROM_LAYOUT_FV_BB_BACKUP_ADDRESS + \
                            (AMI_ROM_LAYOUT_FV_BB_SIZE - Block->BlockSize)) {
        FLASH_AREA_EX       TempBlocksToUpdate;
        TempBlocksToUpdate = BlocksToUpdate[0];
        BlocksToUpdate[0] = BlocksToUpdate[1];
        BlocksToUpdate[1] = TempBlocksToUpdate;
        if (!EFI_ERROR(Status)) SetTopSwap(TRUE);
    }                                
    return Status;
}

/**
 * This function Display "BlocksToUpdate" List information
 *
 */
VOID DisplayBlockToUpdateList(VOID)
{
/*    UINT8           i;
    for (i = 0; BlocksToUpdate[i].Type != FvTypeMax; i++ ) {
        SMDbgPrint("[%x] BlocksToUpdate :: Addr %x, Offset %x, Size %x, Type %x, Ts %x, Update %x, Backup %x\n",
    	            i, (UINT32)BlocksToUpdate[i].BlockAddress,
    	            (UINT32)BlocksToUpdate[i].RomFileOffset,
    	            (UINT32)BlocksToUpdate[i].Size,
    	            (UINT8)BlocksToUpdate[i].Type,
    	            (UINT8)BlocksToUpdate[i].TopSwapTrigger,
                	(UINT8)BlocksToUpdate[i].Update,
    	            (UINT32)BlocksToUpdate[i].BackUpAddress);
    }
*/
}

/**
 * This function Modify the "BlocksToUpdate" List for meeting the Fault 
 *  Tolerant Flash Update Flow before FlashUpdate Starting.
 *
 */
VOID FtRecoveryBeforeFlashUpdate(VOID)
{
    FLASH_AREA_EX   TempBlocksToUpdate;
    
    if ((BlocksToUpdate[0].BlockAddress == (UINT8*)AMI_ROM_LAYOUT_FV_BB_ADDRESS) && \
        (BlocksToUpdate[1].BlockAddress == (UINT8*)AMI_ROM_LAYOUT_FV_RECOVERY_ADDRESS)) {
        if (IsTopSwapOn()) {
            // If TopSwap is on, the FV_BB update is terminated in previous boot, 
            // Only change the TopSwap Trigger when the FV_BB update is completed.
            BlocksToUpdate[0].TopSwapTrigger = TRUE;
            BlocksToUpdate[1].TopSwapTrigger = FALSE;
            return ;
        }
        // Start of backup process, 1) override the backup procedure and BackupAddress.
        if (BlocksToUpdate[0].BackUp != NULL) {
            BlocksToUpdate[0].BackUp = AmiFlashBackUpEx;
        }
        if (BlocksToUpdate[1].BackUp != NULL) {
            BlocksToUpdate[1].BackUp = AmiFlashBackUpEx;
            BlocksToUpdate[1].BackUpAddress = (UINT8*)AMI_ROM_LAYOUT_FV_RECOVERY_BACKUP_ADDRESS;
        }    
        // 2) Swap the priority of FV_BB and FV_RECOVERY for meeting the Fault 
        //    Tolerant Update Process.
        //    [1] Backup FV_RECOVERY, [2] Backup FV_BB, [3] TopSwap enabled, 
        //    [4] Update FV_BB, [5] TopSwap disabled, [6] Update FV_RECOVERY, 
        //    [7] Update other blocks.
        TempBlocksToUpdate = BlocksToUpdate[0];
        BlocksToUpdate[0] = BlocksToUpdate[1];
        BlocksToUpdate[1] = TempBlocksToUpdate;
        BlocksToUpdate[0].TopSwapTrigger = FALSE;
        BlocksToUpdate[1].TopSwapTrigger = TRUE;
    }
    DisplayBlockToUpdateList();
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
