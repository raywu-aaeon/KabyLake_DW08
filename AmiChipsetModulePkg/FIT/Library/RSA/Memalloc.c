//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: CryptMemMgr.c - Light-weight memory manager for PEI and DXE crypto library
//
// Description:    Implements memory management routines malloc(), free() 
//                  and realloc() for PEI and DXE Crypto library
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>
#include "AmiLib.h"
#include <Library/PeiServicesTablePointerLib.h>
#include "includes.h"
#include "os.h"
#include <Library/PcdLib.h>
// #include "wpa_debug.h"

// RestCRmm
// BrGInitCRmm
// malloc
// realloc
// free

//#pragma pack(push, 4)
typedef struct {
    UINT8      *Addr;
    UINT8       Pages;      // Num of pages, Page = 256Byte
    UINT8       Attrib;     // 0-Free, 1-Used
    UINT16      Reserved;   // Align to 2 DWords
} CR_MEM_DESC;
//#pragma pack(pop)
                                           // min 15k decrypt
//#define CR_MAX_HEAP_SIZE 256*4*46        // max 46k or 54k with LTM_FAST=1

#define CR_PAGE_SIZE                256
//
// Attrib Flag Definitions
//
#define CR_BLOCK_FREE               0x00
#define CR_BLOCK_USED               0x01
//
// Global variables
//
// static CR_MEM_DESC    *gCurDesc = NULL;
// static CR_MEM_DESC    *gFirstDesc = NULL;
// static UINT8          *gMemHeap = NULL;
// static UINTN           gMaxHeapSize = 0;
// dbg
// static UINT8          *gMaxAddrReached = NULL;

/*
Optimization-combining free blocks:
	If found free descriptor with smaller page size, check if adjacent block(s) are free too:
	 - Last free descriptor: Clear Page value, leave address as it's no longer matter
	 - Next free descriptor with available combined Page size(can be more then 1):
		Combine Page numbers with Prev one.
		Set Attrib = USED
		Use recursive invocation
*/
/*
void CR_squeze(int **i, CR_MEM_DESC* pDesc, int *Pages )
{
// already found at least one free block pDesc[i]
// Pages reflect remaining number
	if(i>=0 && pDesc[i].Attrib==FREE)
		if(pDesc[i].Pages <= Pages)
	{
// upd Pages in adjacent blocks
		pDesc[i].Pages = 0
		pDesc[i-1].Pages+=pDesc[i].Pages;
		Pages-=pDesc[i].Pages;
		pDesc[i-1].Attrib = USED;
//if Pages != 0 -> call recursively
		if(Pages)
			CE_squeze(--i, pDesc, Pages);
	}
}
*/

void ResetCRmm();
void *BtGcrypt_malloc(unsigned int Size);

//<AMI_PHDR_START>
//----------------------------------------------------------------------
//
// Procedure:    amicrypt_malloc
//
//Description:    Theory of operation:
//    -Returns first available address on the Crypto Heap. 
//    -Traverses MemDesc indexes from last used to 0 for ones with Free attribute 
//      and enough Page space. If free block was not found - returns next available 
//      address and updates next available MemDesc and MaxDescIndex
//    -Updates the pointer within descriptors (index) for next mem chunk
//    -Set mem descriptor with new allocated address within CrHeap, Page size aligned
//    _______________________________________________
//   |                                               |
//gMemHeap                      gCurDesc        gFirstDesc
//   |                              ^                ^ 
//   |<--------- mem heap --------->|<-- mem_desc -->|
//   |______________________________|________________|
//
//    alternative implementation for optimization-combining free blocks:
//    If free descriptor with smaller page size is found, check if adjacent block(s) are free too:
//     - Last free descriptor: update the Addr to next available Page
//     - Next free descriptor with available combined Page size(can be more then 1:
//        increment Page num to match requested size
//---------------------------------------------------------------------- 
//<AMI_PHDR_END>
void *BtGcrypt_malloc(unsigned int Size)
{
    /*
    EFI_STATUS                  Status;
    CONST EFI_PEI_SERVICES      **PeiServices;
    VOID                        *Addr = NULL;

    PeiServices = GetPeiServicesTablePointer ();
    // Status = (*PeiServices)->AllocatePages(PeiServices, EfiBootServicesData, EFI_SIZE_TO_PAGES(Size), &Addr);
    Status = (*PeiServices)->AllocatePool( PeiServices, Size, &Addr );
    if(!EFI_ERROR(Status)){
        return (void *)Addr;
    }else{
        return NULL;
    }
    */
////////////////////////////////////////////////////////////////////////
    CR_MEM_DESC *pDesc, *FirstDesc, *CurDesc;
    UINT8       *NewAddr, *LastHeapAddr;
    UINT32       Pages;
    // UINT32       MaxAddrReached;
    int          i;

    FirstDesc = (CR_MEM_DESC*)PcdGet32(PcdBtgFirstDesc);
    CurDesc = (CR_MEM_DESC*)PcdGet32(PcdBtgCurDesc);
    // MaxAddrReached = PcdGet32(PcdBtgMaxAddrReached);
    
    if(CurDesc == NULL) ResetCRmm();
    
    pDesc = CurDesc;
    
//  Potential heap overflow condition check - 
//  max allocation mem size must be less then the max number 
//  of Pages can stored within UINT8 = 256*256=64kb
    Pages = Size/CR_PAGE_SIZE + (Size%CR_PAGE_SIZE == 0?0:1); // Align to the page size
    if(Pages > 0xff)
        return NULL;
    
// locate available desc backwards
// Test cases
// 3. Old Pages>=Pages  -? Override Pages : Desc use=47; Heap Use: Max
// 2. Old Pages>=Pages  -? Leave Old Pages: Desc use=25; Heap use: Min
// 1. Old Pages==Pages                      Desc use=28; Heap Use: Mid
    i = 0;
    while(&pDesc[i] <= FirstDesc)
    {
        if(pDesc[i].Attrib==CR_BLOCK_FREE && 
            (pDesc[i].Pages >= (UINT8)Pages))
//            (pDesc[i].Pages == Pages)) // Mode 1
        {
            pDesc[i].Attrib = CR_BLOCK_USED;
//            pDesc[i].Pages = Pages; //(Mode1, 3)
            // DEBUG((DEBUG_INFO, "BtgCrypt: ReUse mem(0x%X),page(%X) ; Set Descriptor USED(0x%X)\n", pDesc[i].Addr, pDesc[i].Pages, &pDesc[i]));
            return (void*)(pDesc[i].Addr);
        }
        i++;
    }
// allocate new one
    if(CurDesc > FirstDesc) {
        DEBUG((DEBUG_ERROR, "====\nBtgCrypt MMGR:Descriptor mem overflow %08X>%08X\n====", CurDesc, FirstDesc));
        return NULL;// out of Desc space
    }
    
    NewAddr = pDesc->Addr+(pDesc->Pages*CR_PAGE_SIZE);
    LastHeapAddr = NewAddr+(Pages*CR_PAGE_SIZE);
    
// reuse the descriptor if it has matching address
    if(NewAddr != pDesc->Addr) pDesc--;
// dbg    
    // if(MaxAddrReached < (UINT32)LastHeapAddr) {
        // PcdSet32(PcdBtgMaxAddrReached, (UINT32)LastHeapAddr);
    // }
// end dbg
    if(LastHeapAddr < (UINT8*)pDesc)
    {
        CurDesc = pDesc;
        pDesc->Addr = NewAddr;
        pDesc->Pages = (UINT8)Pages;
        pDesc->Attrib = CR_BLOCK_USED;
        PcdSet32(PcdBtgCurDesc, (UINT32)CurDesc);
        // DEBUG((DEBUG_INFO, "BtgCrypt: LastHeapAddr=0x%X, Available Space=0x%X\n", LastHeapAddr, (UINT8*)pDesc - LastHeapAddr));
        // DEBUG((DEBUG_INFO, "BtgCrypt: New mem(0x%X),page(%X) ; New Descriptor USED(0x%X)\n", pDesc->Addr, pDesc->Pages, pDesc));
        return (VOID*)(NewAddr);
    }
    DEBUG((DEBUG_ERROR, "====>\nBtgCrypt MMGR:Heap and Descriptor mem overlap %08X>%08X\n<====", LastHeapAddr, (UINT8*)pDesc));
    return NULL; // out of Heap space
}

// #ifndef os_zalloc    
// void *os_zalloc(size_t/*unsigned int*/ Size)
// {
    // return(BtGcrypt_malloc(Size));
// }
// #endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------
//
// Procedure:    amicrypt_free
//
// Description:    Crypto Memory Manager->Free
//  Traverses MemDescriptor indexes from last used to 0 to locate Address to be freed. 
//  Set Attrib to Free. May also Clear the stale memory
//---------------------------------------------------------------------- 
//<AMI_PHDR_END>
void BtGcrypt_free(void *ptr)
{
    int    i = 0;
    CR_MEM_DESC *pDesc, *FirstDesc, *CurDesc;
    
    FirstDesc = (CR_MEM_DESC*)PcdGet32(PcdBtgFirstDesc);
    CurDesc = (CR_MEM_DESC*)PcdGet32(PcdBtgCurDesc);
    
    if(CurDesc == NULL) ResetCRmm();
    
    pDesc = CurDesc;
    // locate any available backwards
    while(&pDesc[i] <= FirstDesc)
    {
        if(pDesc[i].Attrib==CR_BLOCK_USED && pDesc[i].Addr == ptr)
        {
            pDesc[i].Attrib = CR_BLOCK_FREE;
            // clear unused memory
            memset(pDesc[i].Addr, 0x0, pDesc[i].Pages*CR_PAGE_SIZE);
            // DEBUG((DEBUG_INFO, "BtgCrypt: free mem(0x%X),page(%X) ; Set Descriptor FREE(0x%X)\n", pDesc[i].Addr, pDesc[i].Pages, &pDesc[i]));
            ptr=NULL;
            return;
        }
        i++;
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
//
// Procedure:    realloc
//
// Description:    Crypto Memory Manager->realloc
//    Traverses memory descriptors back from last to 0 for matching or smaller free contiguous space
//    Alloc new desc and free the old one. Copy mem from old one to new one. Empty old memory
//---------------------------------------------------------------------- 
//<AMI_PHDR_END>
void *BtGcrypt_realloc(void *OldPtr, unsigned int NewSize)
{
    void * NewPtr;
    if ( !OldPtr ) {
        /*
         *  NULL pointer means just do malloc
        */
        return BtGcrypt_malloc( NewSize );
    } else if ( NewSize == 0 ) 
        {
        /*
         *  Non-NULL pointer and zero size means just do free
        */
    	BtGcrypt_free( OldPtr );
        } else {
            NewPtr = BtGcrypt_malloc(NewSize);
            if(NewPtr != NULL){
                memcpy(NewPtr, OldPtr, NewSize);
                BtGcrypt_free( OldPtr );
                return NewPtr;
            }
        }
    return NULL;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
//
// Procedure:    ResetCRmm
//
// Description:    Resets Crypto Memory Manager. Clears mem heap and descriptors
//---------------------------------------------------------------------- 
//<AMI_PHDR_END>
void ResetCRmm()
{
    
    CR_MEM_DESC     *FirstDesc, *CurDesc;
    UINT32          BtgHeap, BtgHeapSize;
    // UINT32          BtgMaxAddrReached;
    
    // 0. Get Pcd data
    BtgHeap = PcdGet32(PcdBtgHeap);
    BtgHeapSize = PcdGet32(PcdBtgHeapSize);
    FirstDesc = (CR_MEM_DESC*)PcdGet32(PcdBtgFirstDesc);
    // BtgMaxAddrReached = PcdGet32(PcdBtgMaxAddrReached);
    // 1. Reset whole Heap
    memset( (void *)BtgHeap, 0x0, (int)BtgHeapSize);
    // 2. Init 1st descriptor
    CurDesc = FirstDesc;
    CurDesc->Addr = (UINT8 *)BtgHeap;
    CurDesc->Pages = 0;
    CurDesc->Attrib = CR_BLOCK_FREE;
    PcdSet32(PcdBtgCurDesc, (UINT32)CurDesc);
    
    //Debug
    // DEBUG((DEBUG_INFO, "BtgCrypt: ResetCRmm()\n"));
    // DEBUG((DEBUG_INFO, "BtgCrypt: BtgHeap        = 0x%X\n", BtgHeap));
    // DEBUG((DEBUG_INFO, "BtgCrypt: BtgHeapSize    = 0x%X\n", BtgHeapSize));
    // DEBUG((DEBUG_INFO, "BtgCrypt: FirstDesc      = 0x%X\n", (UINT32)FirstDesc));
    // DEBUG((DEBUG_INFO, "BtgCrypt: CurDesc        = 0x%X\n", (UINT32)CurDesc));
    // DEBUG((DEBUG_INFO, "BtgCrypt: CurDesc.Addr   = 0x%X\n", CurDesc->Addr));
    // DEBUG((DEBUG_INFO, "BtgCrypt: CurDesc.Pages  = 0x%X\n", CurDesc->Pages));
    // DEBUG((DEBUG_INFO, "BtgCrypt: CurDesc.Attrib = 0x%X\n", CurDesc->Attrib));
    
    // PcdSet32(PcdBtgMaxAddrReached, 0);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
//
// Procedure:    InitCRmm
//
// Description:    Initializes Crypto Memory Manager
//---------------------------------------------------------------------- 
//<AMI_PHDR_END>
void InitCRmm(
    VOID*  pHeap,
    UINTN  HeapSize
)
{
    CR_MEM_DESC*    FirstDesc;

    // PcdSet32(PcdBtgHeap, pHeap);
    // PcdSet32(PcdBtgHeapSize, HeapSize);
    
    FirstDesc = (CR_MEM_DESC*)((UINT8*)pHeap + HeapSize);
    FirstDesc--;

    PcdSet32(PcdBtgFirstDesc, (UINT32)FirstDesc);
    PcdSet32(PcdBtgCurDesc, 0);
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
