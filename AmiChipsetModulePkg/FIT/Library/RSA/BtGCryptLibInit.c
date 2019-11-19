//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
#include <Token.h>
#include <AmiPeiLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include "includes.h"
#include <CryptLib.h>
#include <Library/PcdLib.h>

//
// Global variables
//

// Crypto Memory Manager heap address
// static EFI_PHYSICAL_ADDRESS gDstAddress = 0;

/**
  This constructor function allocates memory for PEI Crypto Memory manager,
 
  @param  FileHandle    The firmware allocated handle for the EFI image.
  @param  PeiServices   A pointer to the PEI Services.
  
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
PeiBtGCryptLibAllocate (
  // IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS           Status;
    EFI_PHYSICAL_ADDRESS DstAddress = 0;
    // UINTN       Npages;

    DEBUG((DEBUG_INFO, "PeiBtGCryptLibAllocate\n"));
    ///////////////////////////////////////////////////////////////////////////////
    //
    // Init Aux Memory Manager
    // Pre-allocates runtime space for possible cryptographic operations
    //
    ///////////////////////////////////////////////////////////////////////////////
    // Status = EFI_SUCCESS;
    // if(gDstAddress == 0) {
        // gHeapSize = PcdGet32(AmiPcdCpMaxHeapSize);
        // Npages = EFI_SIZE_TO_PAGES(gHeapSize);
        Status = (*PeiServices)->AllocatePages(PeiServices, EfiBootServicesData, EFI_SIZE_TO_PAGES(PcdGet32(PcdBtgHeapSize)), &DstAddress);
        // Status = (*PeiServices)->AllocatePages(PeiServices, EfiBootServicesData, Npages, &gDstAddress);
    // }
    // DEBUG ((DEBUG_INFO, "PEI Heap alloc %r (addr=%X, size=%x)\n", Status, (UINT32)gDstAddress, gHeapSize));
    if(!EFI_ERROR(Status) && DstAddress != 0 ) {
        DEBUG((DEBUG_INFO, "DstAddress = 0x%X\n", DstAddress));
        PcdSet32(PcdBtgHeap, (UINT32)DstAddress);
        DEBUG ((DEBUG_INFO, "PEI Heap alloc %r (addr=%X, size=%x)\n", Status, PcdGet32(PcdBtgHeap), PcdGet32(PcdBtgHeapSize)));
        InitCRmm( (void*)DstAddress, PcdGet32(PcdBtgHeapSize));
        
    }

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PeiBtGCryptLibFree (
  // IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
#if defined(MDE_PKG_VERSION) && (MDE_PKG_VERSION>=15)
    EFI_STATUS           Status;
    
    DEBUG((DEBUG_INFO, "PeiBtGCryptLibFree\n"));
    DEBUG((DEBUG_INFO, "Heap = 0x%X\n", PcdGet32(PcdBtgHeap)));
    Status = (*PeiServices)->FreePages(PeiServices, PcdGet32(PcdBtgHeap), EFI_SIZE_TO_PAGES(PcdGet32(PcdBtgHeapSize)));
    DEBUG((DEBUG_INFO, "Free Pages return %r\n", Status));
#endif
    return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
