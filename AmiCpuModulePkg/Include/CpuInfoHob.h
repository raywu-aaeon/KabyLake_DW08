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

/** @file CpuInfoHob.h
    CPU information hob header file, define CPU information hob
    structures in this file. 

**/

#ifndef _AMI_CPU_HOB_H // To Avoid this header get compiled twice
#define _AMI_CPU_HOB_H

#ifdef __cplusplus
extern "C" {
#endif

#define AMI_CPU_INTERNAL_INFO_HOB_GUID \
    {0xe255aa36, 0x3374, 0x4774, 0x81, 0x97, 0xc7, 0x4e, 0x2b, 0x06, 0x26, 0x5b}

#pragma pack(push,1)
typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    UINT8   Revision;
    UINT8   FactoryPowerConv;
    UINT8   FactoryTimeConv;
    UINT16  FactoryTdc;
    UINT16  FactoryTdp;                    
    UINT8   FactoryTdpLimitTime;
    UINT8   FactoryOneCoreRatioLimit;
    UINT8   FactoryTwoCoreRatioLimit;
    UINT8   FactoryThreeCoreRatioLimit;
    UINT8   FactoryFourCoreRatioLimit;
    UINT8   FactoryFiveCoreRatioLimit;
    UINT8   FactorySixCoreRatioLimit;
	UINT16	FactoryIaCoreCurrentMax;
	UINT8	FactoryOneCoreRatioMax;
	UINT8	FactoryTwoCoreRatioMax;
	UINT8	FactoryThreeCoreRatioMax;
	UINT8	FactoryFourCoreRatioMax;
	UINT8	FactoryFiveCoreRatioMax;
	UINT8	FactorySixCoreRatioMax;
	UINT8	AesAvailable;
} AMI_CPU_INTERNAL_INFO_HOB;
#pragma pack(pop)

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
