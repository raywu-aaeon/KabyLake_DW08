//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CpuPeiBeforeMem.c
    Main CpuPeiEarlyInit Driver File. 

**/

#include <Cpu.h>
#include <Token.h>
#include <AmiPeiLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/IoLib.h>
#include "Library/CpuCspLib.h"
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Setup.h>

/**
    CpuPei driver entry Before memory initialize

    @param FfsHeader pointer to image FFS file
    @param PeiServices pointer to PEI services

    @retval EFI_STATUS Return EFI status. 
**/

EFI_STATUS
EFIAPI
CpuPeiBeforeMemEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
  return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
