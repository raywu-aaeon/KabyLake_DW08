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

/** @file CpuPolicy.h
    CPU setup data header file, define all the CPU
    setup items and a structures in this file. 

    @note  The context of the CPU_SETUP_DATA may be able to copy from
              CPU.SD directly 

**/

#include <Setup.h>

#ifndef _AMI_CPU_SETUP_POLICY_H // To Avoid this header get compiled twice
#define _AMI_CPU_SETUP_POLICY_H

#ifdef __cplusplus
extern "C" {
#endif

#define AMI_CPU_PLATFORM_INFO_HOB_GUID \
    {0x65ed5ad8, 0xd5f0, 0x4012, 0xb9, 0x5c, 0xc5, 0x42, 0xe4, 0x7a, 0x32, 0xe6}

#ifndef VFRCOMPILE

#pragma pack(push, 1)

typedef struct _CPU_SETUP_DATA  CPU_SETUP_DATA;

typedef struct _CPU_SETUP_DATA {
    // CPU Setup header
    UINT32  CpuPolicyVersion;

//    UINT8       Clpm; // Current low power mode

    UINT8       ProcessorVmxEnable;
    UINT8       ProcessorHtMode;
    UINT8       ExecuteDisableBit;
    UINT8       ProcessorCcxEnable;
    UINT8       ProcessorEistEnable;
    UINT8       CpuidMaxValue;
    UINT8       MlcStreamerPrefetcherEnable;
    UINT8       MlcSpatialPrefetcherEnable;
    UINT8       DCUStreamerPrefetcherEnable;
    UINT8       DCUIPPrefetcherEnable;
    UINT8       TurboModeEnable;
    UINT8       ProcessorXEEnable;
    UINT8       ProcessorXapic;
    UINT8       ProcessorTDCLimitOverrideEnable;
    UINT8       ProcessorTDCLimit;
    UINT8       ProcessorTDPLimitOverrideEnable;
    UINT8       ProcessorTDPLimit;
    UINT8       RatioLimit1C;
    UINT8       RatioLimit2C;
    UINT8       RatioLimit3C;
    UINT8       RatioLimit4C;
    UINT8       ProcessorVirtualWireMode;
    UINT8       ActiveProcessorCores;	
} CPU_SETUP_DATA;


typedef struct _AMI_CPU_PLATFORM_INFO_HOB
{
    EFI_HOB_GUID_TYPE EfiHobGuidType;
    CPU_SETUP_DATA CpuPolicyData;
} AMI_CPU_PLATFORM_INFO_HOB;

#pragma pack(pop)

typedef VOID (CPU_OEM_SETUP_CALLBACK) (
    IN VOID                 *Services,
    IN OUT CPU_SETUP_DATA    *CpuSetupData,
    IN SETUP_DATA           *SetupData,
    IN BOOLEAN              Pei
);


VOID GetCpuSetupData (
    IN VOID                 *Service,
    IN OUT CPU_SETUP_DATA    *CpuSetupData,
    IN BOOLEAN              Pei
);
#endif
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
