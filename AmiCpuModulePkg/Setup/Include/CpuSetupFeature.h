//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CpuSetupFeature.h
    Ami cpu Setup header file.

**/


#ifndef __CPURC_SETUP_H__
#define __CPURC_SETUP_H__



#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////
//This is a private structure that is used for setup by Cpu.Sd
//This structure can change any time. No drivers should refer
// directly to this structure.
////////////////////////////////////////////////////////////////
typedef struct {
	UINT8	XDBitAvailable;
	UINT8	HTAvailable;
    UINT8   MultiCoreAvailable;
	UINT8	VTAvailable;
	UINT8	LimitCpuidAvailable;
    UINT8   MultiSocketAvailable;
    UINT8   MultiSocketPopulated;
    UINT8   LocalX2ApicAvailable;
    UINT8   NumCores;
    UINT8   CpuGroup;   //Arbitrary number
    UINT8   IsSandyBridge;
    UINT8   IsHasWell;
    UINT8   Skt0Pop;
    UINT8   Skt1Pop;
    UINT8   Skt2Pop;
    UINT8   Skt3Pop;
	UINT8	SmxAvailable;
	UINT8   CpuMismatch;
	UINT8   XECoreRatioLimitAvailable;
	UINT8   CurrentLimitAvailable;
	UINT8   PwrLimitAvailable;
	UINT8   TccActivationAvailable;
	UINT8   EistAvailable;
	UINT8   TurboModeAvailable;
	UINT8   C3Available;
	UINT8   C6Available;
	UINT8   C7Available;
	UINT8   AesAvailable;
	UINT8   cTDPAvailable;
	UINT8   C8Available;
	UINT8   C9Available;
	UINT8   C10Available;
    UINT8   PkgCStateDemotionAvailable;
    UINT8   DebugInterfaceSelection;
    UINT8	FivrAvailable;
    UINT8	SgxSupported;
    UINT8	PrmrrSupportSize;
    UINT8   TjMax;
    UINT16  CpuTdp;
    UINT8	HwpAvailable;
    UINT32  MaximumTurboFrequency;
} AMICPU_SETUP_FEATURES;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
