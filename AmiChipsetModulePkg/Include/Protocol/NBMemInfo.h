//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
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
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/SandyBridge_A/Chipset/Protocols/NBMemInfo.h 1     2/24/10 10:39p Tonywu $
//
// $Revision: 1 $
//
// $Date: 2/24/10 10:39p $
//*************************************************************************
/** @file NBMemInfo.h
    The header file for NB Memory Information 

**/
//*************************************************************************

#ifndef _NB_MEMORY_INFO_
#define _NB_MEMORY_INFO_

#ifdef __cplusplus
extern "C" {
#endif

#include <Token.h>

#define EFI_NB_MEMORY_INFO_GUID \
  {0x490216c0, 0x76a, 0x44d3, 0xa5, 0x36, 0xac, 0xe0, 0x5c, 0x90, 0xe3, 0x86}

// {40312829-7891-4abd-B200-0C541B061939}
#define EFI_NB_MRC_S3_DATA_GUID \
  {0x40312829, 0x7891, 0x4abd, 0xb2, 0x00, 0x0c, 0x54, 0x1b, 0x06, 0x19, 0x39}

//	GUID_VARIABLE_DECLARATION(gEfiNBMemoryInfoGuid, EFI_NB_MEMORY_INFO_GUID);
extern EFI_GUID gEfiNBMemoryInfoGuid;

#define NB_MEMORY_INFO_VARIABLE L"NBMemoryInfo"

#pragma pack(push,1)

typedef enum {
    UnknownType,
    DDR1,
    DDR2,
    DDR3,
    OtherType
} MEMORY_TYPE_INFO;

typedef enum {
    UnknownMode,
    Single,
    Dual,
    Four,
    Eight,
    OtherMode
} MEMORY_CHANNEL_MODE_INFO;

typedef enum {
    UnknownCL,
    CL10,
    CL15,
    CL20,
    CL25,
    CL30,
    CL40,
    CL50,
    CL60,
    CL70,
    CL80,
    OtherCL
} CAS_LATENCY_INFO;


typedef struct {
    UINT32          Size;                  // MB
    BOOLEAN         DoubleSide;
    BOOLEAN         Ecc;
    UINT8           SpdAddr;
} MEMORY_SLOT_INFO;

typedef struct {
    MEMORY_TYPE_INFO         RamType;
    UINT32                   MaxCapacity;           // MB
    UINT32                   TotalMemory;           // MB
    UINT32                   Speed;                 // Mhz
    MEMORY_SLOT_INFO         Slot[DIMM_SLOT_NUM];
    CAS_LATENCY_INFO         CasLatency;
    MEMORY_CHANNEL_MODE_INFO ChannelMode;
} NB_MEMORY_ARRAY_INFO;

typedef struct {
    NB_MEMORY_ARRAY_INFO     MemoryArray[MEMORY_ARRAY_NUM];
} NB_MEMORY_INFO;

#pragma pack(pop)

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
