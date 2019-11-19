//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CpuDxe.h
    Cpu Dxe header file.

**/

#ifndef __CPU_DXE_H__
#define __CPU_DXE_H__


#include <Token.h>
#include <Cpu.h>
#include <Dxe.h>
#include <Setup.h>
#include <CpuBoardLib.h>
#include <AmiDxeLib.h>
#include <Protocol/MpService.h>
#include <Protocol/AmiCpuInfo.h>
#include <Protocol/AmiCpuInfo2.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/Cpu.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/CpuCspLib.h>
#include <Library/CpuPolicy.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>

#define DELAY_CALCULATE_CPU_PERIOD  200     //uS

#ifdef __cplusplus
extern "C" {
#endif

// Define structures used and referenced in this file
typedef struct{
	UINT16 NumberOfPStates;
} P_STATES_DATA;

typedef struct{
	UINT32 MemAddress, MemLength;
} system_memory_struc;

typedef struct {
	BOOLEAN LimitCpuidSupport;
	BOOLEAN LimitCpuidEnable;
} CPU_FEATURES_STATUS;

EFI_STATUS EfiCpuFlushDataCache (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN EFI_PHYSICAL_ADDRESS     Start,
    IN UINT64                   Length,
    IN EFI_CPU_FLUSH_TYPE       FlushType);

EFI_STATUS EfiCpuSetMemoryAttributes (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN EFI_PHYSICAL_ADDRESS     BaseAddress,
    IN UINT64                   Length,
    IN UINT64                   Attributes);

UINT32 * GetPtrToPrivateAmiCpuInfo2Entry(
    IN UINT32 Package,
    IN UINT32 Core,
    IN UINT32 Thread
);

VOID CreatePrivateAmiCpuInfo2();

//APIC ID
//CPU NUM
#define PRIVATE_INFO_NUM_OF_CPU_DATA 2

typedef struct {
    AMI_CPU_INFO_2_PROTOCOL AmiCpuInfo2;
//??? Determine max structure size of UINT32s
    //  UINT32 # of populated Sockets 0
    //  ---------------------
    //  ---------------------
    //  ---Socket #0---
    //  UINT32 # of Cores
    //   ---Core #0---
    //  UINT32 # of Threads
    //   ---Thread #0---
    //  UINT32 APIC ID
    //  UINT32 CPU Num
    //   ---Thread #1---
    //  UINT32 APIC ID
    //  UINT32 CPU Num
    //  ---Core #1---
    //  ....
    //  ---------------------
    //  ---------------------
    //  ---Socket #1---
    //  UINT32 # of Cores
    //  ---Core # 0---
    //  UINT32 # of Threads
    //  ---Thread #0---
    //  ....
} PRIVATE_AMI_CPU_INFO_2_PROTOCOL;

extern PRIVATE_AMI_CPU_INFO_2_PROTOCOL *gPrivateAmiCpuInfo2;

typedef struct _AMI_BEFORE_CPU_RC_PROTOCOL AMI_BEFORE_CPU_RC_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *PLATFORM_CPU_DXE_POLICY_OVERWRITE) (
  IN AMI_BEFORE_CPU_RC_PROTOCOL    *This
  
  );

typedef struct _AMI_BEFORE_CPU_RC_PROTOCOL {
	PLATFORM_CPU_DXE_POLICY_OVERWRITE		PlatformCpuDxePolicyOverwrite;
};

VOID CpuDxeMiscFuncs(VOID);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
