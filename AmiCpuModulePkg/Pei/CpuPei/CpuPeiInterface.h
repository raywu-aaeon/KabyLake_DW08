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

/** @file CpuPeiInterface.h
    Cpu Pei Interface header file

**/

#ifndef __CPU_PEI_BOARD_H__
#define __CPU_PEI_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif


#define IED_SIZE	0x400000
#define VariableMtrrCount 10

#define SIZE_1M        0x100000
#define SIZE_64M      0x4000000
#define SIZE_128M     0x8000000
#define SIZE_256M    0x10000000
#define SIZE_512M    0x20000000
#define SIZE_1G      0x40000000
#define SIZE_2G      0x80000000
#define SIZE_4G     0x100000000
#define SIZE_8G     0x200000000
#define SIZE_16G    0x400000000

//Chagned in IvyBridge RC 0.8
//#define PEI_CACHE_PPI_GUID \
//  {0xc153205a, 0xe898, 0x4c24, 0x86, 0x89, 0xa4, 0xb4, 0xbc, 0xc5, 0xc8, 0xa2}
#define PEI_CACHE_PPI_GUID \
  {0x9be4bc2, 0x790e, 0x4dea, 0x8b, 0xdc, 0x38, 0x5, 0x16, 0x98, 0x39, 0x44}

#define AMI_INTERNAL_UCODE_HOB_GUID \
    {0x94567c6f, 0xf7a9, 0x4229, 0x13, 0x30, 0xfe, 0x11, 0xcc, 0xab, 0x3a, 0x11}

typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;    
	UINT32  uCodeAddr;
} AMI_INTERNAL_UCODE_HOB;

typedef enum _EFI_MEMORY_CACHE_TYPE
{
  EfiCacheTypeUncacheable   = 0,
  EfiCacheTypeWriteCombining= 1,
  EfiCacheTypeReserved2     = 2,
  EfiCacheTypeReserved3     = 3,
  EfiCacheTypeWriteThrough  = 4,
  EfiCacheTypeWriteProtected= 5,
  EfiCacheTypeWriteBack     = 6,
  EfiCacheTypeMaximumType   = 7
} EFI_MEMORY_CACHE_TYPE;


typedef struct _PEI_CACHE_PPI PEI_CACHE_PPI;

//
// *******************************************************
// PEI_SET_CACHE_PPI
// *******************************************************
//
typedef
EFI_STATUS
(EFIAPI *PEI_SET_CACHE_PPI) (
  IN  PEI_CACHE_PPI                     * This,
  IN  EFI_PHYSICAL_ADDRESS              MemoryAddress,
  IN  UINT64                            MemoryLength,
  IN  EFI_MEMORY_CACHE_TYPE             MemoryCacheType
  );

//
// *******************************************************
// PEI_RESET_CACHE_PPI
// *******************************************************
//
typedef
EFI_STATUS
(EFIAPI *PEI_RESET_CACHE_PPI) (
  IN PEI_CACHE_PPI                      * This
  );

//
// *******************************************************
// PEI_ACTIVATE_CACHE_PPI
// *******************************************************
//  
typedef
EFI_STATUS
(EFIAPI *PEI_ACTIVATE_CACHE_PPI) (
  IN PEI_CACHE_PPI                      * This
  );
  
//
// *******************************************************
// PEI_CACHE_PPI
// *******************************************************
//
typedef struct _PEI_CACHE_PPI {
  PEI_SET_CACHE_PPI   SetCache;
  PEI_RESET_CACHE_PPI ResetCache;
  PEI_ACTIVATE_CACHE_PPI ActivateCache;
} PEI_CACHE_PPI;
  
typedef struct {
    UINT64  Base;
    UINT64  Len;
} MEMORY_MAP;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

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

