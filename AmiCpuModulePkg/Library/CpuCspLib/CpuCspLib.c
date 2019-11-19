//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CpuCspLib.c
    Contains the CPU library related functions. These functions can be linked
    with various components in the project.

**/

//#include <Efi.h>
#include <Token.h>
#include <AmiDxeLib.h>
#include <Smm.h>
#include <PciE.h>

#include "Cpu.h"
#include "Library/CpuCspLib.h"

//AptioV
//This is a workaournd.
//This Aptio4 code is using x64_BUILD macro, which is wrong.
//The macro is not defined by the Core build flags in Aptio4.
//EFIx64 should be used instead.
#ifdef EFIx64
#ifndef x64_BUILD
#define x64_BUILD 1
#endif
#endif

#ifndef FV_MICROCODE_BASE
#define FV_MICROCODE_BASE FV_MAIN_BASE
#endif

#define MAX_NR_BUS ((PCIEX_LENGTH/0x100000)-1)

static EFI_GUID gMicrocodeFfsGuid = 
    {0x17088572, 0x377F, 0x44ef, 0x8F, 0x4E, 0xB0, 0x9F, 0xFF, 0x46, 0xA0, 0x70};

/**
    Get the cpu signature.

    @param VOID

    @retval UINT32 Cpu Signature
**/

UINT32 GetCpuSignature()
{
    UINT32 CpuSignature, CpuIdEBX, CpuIdECX, CpuIdEDX;
    CPULib_CpuID(1, &CpuSignature, &CpuIdEBX, &CpuIdECX, &CpuIdEDX);
    return CpuSignature;
}

typedef struct {
    UINT32 Stepping:4;
    UINT32 Model:4;
    UINT32 Family:4;
    UINT32 Type:2;
    UINT32 RV:2;
    UINT32 ExtModel:4;
    UINT32 ExtFamily:8;
} CPU_SIGNATURE;

/**
    Get the cpu family from signature.

    @param CpuSignature Cpu Signature

    @retval UINT32 Cpu Family
**/

UINT32 AmiGetCpuFamily(IN UINT32 CpuSignature)
{
    CPU_SIGNATURE *Signature = (CPU_SIGNATURE*)&CpuSignature;
    return Signature->ExtFamily + Signature->Family;
}

/**
    Get the cpu model from signature.

    @param CpuSignature Cpu Signature

    @retval UINT32 Cpu Model
**/

UINT32 GetCpuModel(IN UINT32 CpuSignature)
{
    CPU_SIGNATURE *Signature = (CPU_SIGNATURE*)&CpuSignature;
    return (Signature->ExtModel << 4) + Signature->Model;
}

/**
    Get the cpu platform Id.

    @param VOID

    @retval UINT32 Cpu Platform Id
**/

UINT32  GetCpuPlatformId(VOID)
{
    return (UINT32)Shr64(ReadMsr(0x17), 50) & 7;
}

/**
    Get the CPU group for the platform policy.

    @param VOID

    @retval UINT32 Cpu Group
**/

UINT32 GetCpuGroup() {
    
   UINT32 NumCores = NumSupportedCpuCores();
   
   switch (NumCores) {
      case 8: return 1;
      case 4: return 2;
      case 2: return 5;
      default: return 2;
   }
}

/**
    Return the Smrr Base Msr

    @param VOID

    @retval UINT32 SMRR Base
**/

UINT32  GetSmrrBaseMsr()
{
    return 0x1f2;
}

/**
    Return number of shared threads for a Information.

    @param Level Cache level

    @retval UINT8 Number of shared threads.
**/

UINT8 GetCacheSharedThreads(IN UINT8 Level)
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT32 i = 0;

    for(;;) {
        RegEcx = i;
        CPULib_CpuID(4, &RegEax, &RegEbx, &RegEcx, &RegEdx);
        if ((RegEax & 0x1f) == 0) break;
        if (((RegEax >> 5) & 7) == Level) return 1 + ((RegEax >> 14)& 0xfff);
        ++i;
    }
    return 0;
}

/**
    This function writes the CPU MSR with the value provided.
  
    @param Msr MSR index
    @param Value OR Value
    @param Mask AND Mask Value

    @retval VOID
**/

VOID ReadWriteMsr(IN UINT32 Msr, IN UINT64 Value, IN UINT64 Mask)
{
    UINT64 OrigData = ReadMsr(Msr);
    UINT64 WriteData = (OrigData & Mask) | Value;
    WriteMsr(Msr, WriteData);
}

/**
    Get number of supported threads per core.

    @param VOID

    @retval UINT8 Number of Threads per core.
**/

UINT8 NumSupportedThreadsPerCore()
{
	UINT32	RegEax, RegEbx, RegEcx, RegEdx;

    RegEcx = 0;		
    CPULib_CpuID(0xb, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    return (UINT8)RegEbx;
}

/**
    Get number of supported Cpu Cores per package.

    @param VOID

    @retval UINT8 Number of supported Cpu Cores per package.
**/

UINT8 NumSupportedCpuCores()
{
	UINT32	RegEax, RegEbx, RegEcx, RegEdx;
    UINT8  TotLogicalCpus;
    UINT8  LogicalCpusPerCore;

    RegEcx = 1;		
    CPULib_CpuID(0xb, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    TotLogicalCpus  = (UINT8)RegEbx;

    RegEcx = 0;		
    CPULib_CpuID(0xb, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    LogicalCpusPerCore  = (UINT8)RegEbx;

    return TotLogicalCpus / LogicalCpusPerCore;
}

/**
    Get number of supported Cpu Threads per package.

    @param VOID

    @retval UINT8 Number of supported Cpu Threads per package.
**/

UINT8 NumSupportedCpuThreads()
{
	UINT32	RegEax, RegEbx, RegEcx, RegEdx;    
    
    RegEcx = 1;		
    CPULib_CpuID(0xb, &RegEax, &RegEbx, &RegEcx, &RegEdx);    
    
    return (UINT8)RegEbx;
}

/**
    Get number of logical CPUs.

    @param VOID

    @retval UINT8 Number of logical CPUs.
**/

UINT8 NumLogicalCpus()
{
    UINT64  MsrData = ReadMsr(MSR_CORE_THREAD_COUNT);
    return (UINT8)MsrData;
}

/**
    Determine if CPU is HT.

    @param VOID

    @retval BOOLEAN True if HT CPU.
**/

BOOLEAN AmiIsHtEnabled()
{
    UINT8   NumLogCPUs, NumCpuCores;
    UINT64  MsrData = ReadMsr(MSR_CORE_THREAD_COUNT);
    UINT32  CpuSignature = GetCpuSignature() & 0xfffffff0;

    NumCpuCores = (UINT8)((UINT32)MsrData >> 16);

    // Westmere work around
    if (CpuSignature == WESTMERE) NumCpuCores &= 0xf;

    NumLogCPUs = (UINT8)MsrData;

    if ((NumLogCPUs / NumCpuCores) <= 1) return FALSE;
    return TRUE;
}

/**
    Returns number of CPU Cores

    @param VOID

    @retval UINT8 Number of CPU Cores.
**/

UINT8 NumCpuCores()
{
    UINT32  CpuSignature = GetCpuSignature() & 0xfffffff0;
    UINT64 MsrData = ReadMsr(MSR_CORE_THREAD_COUNT);
    UINT8 NumCpuCores = (UINT8)((UINT32)MsrData >> 16);

    // Westmere work around
    if (CpuSignature == WESTMERE) NumCpuCores &= 0xf;

    return NumCpuCores;
}

/**
    Determine if CPU thread is logical CPU 0 executing.

    @param VOID

    @retval BOOLEAN True if logical CPU 0.
**/

BOOLEAN IsHt0()
{
    UINT32 ApicMask;
    UINT32 ApicId;
    UINT8 ThreadsPerCore = NumSupportedThreadsPerCore();
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;

    if (ThreadsPerCore < 2) return TRUE;    //Check if Ht Capable.
    ApicMask = ThreadsPerCore - 1;

    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    ApicId = RegEbx >> 24; 

    //Use APIC ID to determine if logical CPU.
    if ((ApicId & ApicMask) == 0) return TRUE;  //All logical CPU0 will have bit 0 clear.
    return FALSE;
}

/**
    Determine if CPU thread is CPU Core 0 executing.

    @param VOID

    @retval BOOLEAN True if logical CPU 0.
**/

BOOLEAN IsCore0()
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT8  MaxThreadsPackage;
    UINT32 ApicMask;
    UINT32 ApicId;
    //UINT8 ThreadsPerCore = NumSupportedThreadsPerCore();// not referenced

    ApicMask = ~(NumSupportedThreadsPerCore() - 1);

    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);

    MaxThreadsPackage = (UINT8)(RegEbx >> 16);
    
    ApicMask &= MaxThreadsPackage - 1;
    ApicId = RegEbx >> 24; 

    //Use APIC ID to determine if logical CPU.
    if ((ApicId & ApicMask) == 0) return TRUE;
    return FALSE;
}

/**
    Determine if CPU supports X64.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsX64Supported()
{    
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    
    CPULib_CpuID(0x80000001, &RegEax, &RegEbx, &RegEcx, &RegEdx);	
	return (RegEdx >> 29) & 1;
}

/**
    Determine if CPU supports Execute Disable.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsXDSupported()
{   	
	UINT32  RegEax, RegEbx, RegEcx, RegEdx;

	CPULib_CpuID(0x80000001, &RegEax, &RegEbx, &RegEcx, &RegEdx);	
	return (RegEdx >> 20) & 1;
}

/**
    Determine if CPU supports EIST.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsEistSupported()
{	
	UINT32  RegEax, RegEbx, RegEcx, RegEdx;
    	
	CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);
	return (RegEcx >> 7) & 1;

}

/**
    Determine if CPU supports Turbo mode.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsTurboModeSupported()
{
	BOOLEAN ret;
	UINT32  RegEax, RegEbx, RegEcx, RegEdx;
    UINT64 MsrData = ReadMsr(MSR_IA32_MISC_ENABLE);
  
	CPULib_CpuID(6, &RegEax, &RegEbx, &RegEcx, &RegEdx);
	ret = ((RegEax >> 1) & 1) | (UINT32) (Shr64(MsrData,TURBO_MODE_DISABLE_BIT) & 1); 
	return ret;
}

/**
    Determine if CPU supports Full unlock.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsFullUnlockCpuSuuported()
{
	UINT64	MsrData = ReadMsr(MSR_FLEX_RATIO);
				
	if((UINT32)(Shr64(MsrData, 17)) == 0x7) return TRUE;
	return FALSE;
}

/**
    Determine if CPU supports Programmable TDC/TDP Limit for the Turbo mode.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsXeTdpLimitSupported()
{
	return (((UINT32)ReadMsr(MSR_PLATFORM_INFO)) >> XE_TDP_PROGRAMMABLE_BIT & 1);
}

/**
    Determine if CPU supports Programmable Core Ratio Limit for the Turbo mode.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsXECoreRatioLimitSupported()
{	
	return (((UINT32)ReadMsr(MSR_PLATFORM_INFO)) >> XE_CORE_RATIO_PROGRAMMABLE_BIT & 1);
}

/**
    Determine if CPU supports Programmable TDC/TDP Limit for the Turbo mode.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsCtdpSupported()
{
	BOOLEAN ret;    
	ret = Shr64(ReadMsr(MSR_PLATFORM_INFO), CTDP_OFFSET) & CTDP_MASK ? 1:0;
	return ret;
}

/**
    Determine if CPU supports TCC activation offset programmable

    @param VOID

    @retval BOOLEAN True if Smx supported.
**/

BOOLEAN IsTccActProgSupported()
{    
	return (((UINT32)ReadMsr(MSR_PLATFORM_INFO)) >> TCC_ACTIVE_OFFSET_PROGRAMMABLE_BIT & 1);
}

/**
    Determine if CPU supports limiting CpuId to 3.

    @param VOID

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsLimitCpuidSupported()
{
    UINT32 RegEbx, RegEcx, RegEdx;
    UINT32 LargestCPUIDFunc;
    CPULib_CpuID(0, &LargestCPUIDFunc, &RegEbx, &RegEcx, &RegEdx);
    return LargestCPUIDFunc > 3;
}

/**
    Determine if CPU supports machine check.

    @param Features Cpu featuures.

    @retval BOOLEAN True if supported.
**/

BOOLEAN IsMachineCheckSupported(IN CPU_FEATURES *Features)
{
    //Check if MCE and MCA supported.
    return ((Features->FeatureEdx & ((1 << 7) + (1 << 14))) == ((1 << 7) + (1 << 14)));
}

/**
    Determine if CPU supports VT extensions Vmx.

    @param Features Cpu featuures.

    @retval BOOLEAN True if Vmx supported.
**/

BOOLEAN IsVmxSupported(IN CPU_FEATURES *Features)
{    
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);	 
	return (RegEcx >> 5) & 1;
}

/**
    Determine if CPU supports VT extensions Smx.

    @param Features Cpu featuures.

    @retval BOOLEAN True if Smx supported.
**/

BOOLEAN IsSmxSupported(IN CPU_FEATURES *Features)
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    
    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);	
	return (RegEcx >> 6) & 1;
}

/**
    Determine if CPU supports Smrr.

    @param VOID

    @retval BOOLEAN True if Smx supported.
**/

BOOLEAN IsSmrrSupported()
{ 
     return (((UINT32)ReadMsr(MSR_IA32_MTRRCAP)) >> SMRR_SUPPORTED_BIT) & 1;    
}

/**
    Determine if Energy Performance Bias supported.

    @param VOID

    @retval BOOLEAN True if Energy Performance Bias supported.
**/

BOOLEAN IsEnergyPerfBiasSupported()
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    
    ReadWriteMsr(MSR_MISC_PWR_MGMT, (1 << ENG_PERF_BIAS_EN_BIT), (UINT64)-1); //Energy Performance Bias Enable
    CPULib_CpuID(6, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    return !!(RegEcx & BIT3);
}

/**
    Determine if C-state interrupting state supported.

    @param CstateIndex C state indxe.

    @retval BOOLEAN True if C-state interrupting supported.
**/

BOOLEAN IsCxStateSupported(IN UINT8 CstateIndex)
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT8  ret;
    
    CPULib_CpuID(5, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    switch (CstateIndex){
    
    case C3_STATE:
        ret = !!(RegEdx & C3_SUB_STATES_MASK);
        break;
    
    case C6_STATE:
        ret = !!(RegEdx & C6_SUB_STATES_MASK);
        break;
    
    case C7_STATE:
        ret = !!(RegEdx & C7_SUB_STATES_MASK);
        break;                

    case C8_STATE:
        ret = !!(RegEdx & C8_SUB_STATES_MASK);     
        break;        
    
    case C9_STATE:
        ret = !!(RegEdx & C9_SUB_STATES_MASK);
        break;        

    case C10_STATE:
        ret = !!(RegEdx & C10_SUB_STATES_MASK);
        break;        
    
    }
    return ret;
}

/**
    Determine if CPU supports AES instuctions.

    @param VOID

    @retval BOOLEAN True if Smx supported.
**/

BOOLEAN IsAesSupported()
{ 
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    
    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);	
	return (RegEcx >> 25) & 1;  
}

/**
    Determine if Vmx is enabled.

    @param VOID

    @retval BOOLEAN True if Vmx enabled.
**/

BOOLEAN CPULib_IsVmxEnabled()
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT8  Msr;
    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    if (!(RegEcx & (1 << 5))) return FALSE;

    Msr = (UINT8)ReadMsr(MSR_IA32_FEATURE_CONTROL);
    return !!(Msr & (1 << 2));
}

/**
    Determine if Smx is enabled.

    @param VOID

    @retval BOOLEAN True if Smx enabled.
**/

BOOLEAN CPULib_IsSmxEnabled()
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT8  Msr;
    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    if (!(RegEcx & BIT6)) return FALSE;

    Msr = (UINT8)ReadMsr(MSR_IA32_FEATURE_CONTROL);
    return !!(Msr & BIT1);
}

/**
    Determine if Smrr is enabled.

    @param VOID

    @retval BOOLEAN True if Smrr is enabled.
**/

BOOLEAN CPULib_IsSmrrEnabled()
{
    //Once SMRR is enabled, the opened SMM Area can't be read outside of SMM.
#if SMM_CACHE_SUPPORT == 0
    return FALSE;
#else
    //Some CPUs, SMRR has an enable bit. Nehalem only has a capability bit.
    UINT32 MtrrCap = (UINT32)ReadMsr(MSR_IA32_MTRR_CAP);
    return !!(MtrrCap & SMRR_SUPPORT_MASK);
#endif
}

/**
    Is APIC enabled, xAPIC or x2APIC

    @param VOID

    @retval BOOLEAN True if enabled
**/

BOOLEAN CPULib_IsLocalApicEnabled()
{
    UINT32 Msr = (UINT32)ReadMsr(MSR_XAPIC_BASE);
    return !!(Msr & (1 << XAPIC_GLOBAL_ENABLE_BIT));
}

/**
    Get C-state latency.

    @param VOID

    @retval BOOLEAN True if enabled
**/

BOOLEAN CPULib_IsLocalX2ApicEnabled()
{
    UINT32 Msr = (UINT32)ReadMsr(MSR_XAPIC_BASE);
    return !!(Msr & (1 << XAPIC_X2APIC_ENABLE_BIT));
}

/**
    Check to see if the MSR_IA32_FEATURE_CONTROL is locked.

    @param VOID

    @retval BOOLEAN True if MSR_IA32_FEATURE_CONTROL is locked.
**/

BOOLEAN CPULib_IsFeatureControlLocked() {
    UINT8 Ia32FeatureCntrl = (UINT8)ReadMsr(MSR_IA32_FEATURE_CONTROL);
    return Ia32FeatureCntrl & 1;
}

/**
    Returns number of CPU sockets are populated.

    @param VOID

    @retval UINT32 Number of CPU sockets populated.
**/

UINT32  NumberOfCpuSocketsPopulated()
{
    return 1;
}

/**
    Get C-state latency.

    @param Cstate C state indxe.

    @retval UINT32 C-state latentcy in uS.
**/

UINT32 CPULIB_GetCstateLatency(IN UINT8 Cstate)
{
    UINT32 CpuSigNoVer = GetCpuSignature() & 0xfffffff0;
    BOOLEAN IsSandyBridge = CpuSigNoVer == SANDY_BRIDGE || CpuSigNoVer == JAKETOWN || CpuSigNoVer == IVY_BRIDGE;

    if (IsSandyBridge) {
        switch(Cstate) {
        case 1: return 1;
        case 3: return 80;
        case 6: return 104;
        case 7: return 109;
        }
    }

    switch(Cstate) {
    case 1: return 3;
    case 3: return 205;
    case 6: return 245;
    case 7: return 245;
    }
    return 0;
}

/**
    Get C-state power.

    @param Cstate C state indxe.

    @retval UINT32 C-state power in mW.
**/

UINT32 CPULIB_GetCstatePower(IN UINT8 Cstate)
{
    switch(Cstate) {
    case 1: return 1000;
    case 3: return 500;
    case 6: return 350;
    case 7: return 200;
    }
    return 0;
}

/**
    Find the microcode address for the CPU calling this in specific firmware volume.

    @param Fv - Firmware Volume

    @retval VOID 0 if not found or the Microcode Address.
**/

VOID * CPULib_FindMicrocodeInFv(IN VOID *Fv)
{
    UINT8  *pEndOfFv = (UINT8*)Fv + ((EFI_FIRMWARE_VOLUME_HEADER*)Fv)->FvLength - 1;
    UINT8  *pFfs = (UINT8*)Fv + ((EFI_FIRMWARE_VOLUME_HEADER*)Fv)->HeaderLength;
    UINT8  *pEndOfFfs;
    UINT32 FfsSize;

    MICROCODE_HEADER  *pUc;
    UINT32 UcSize;
    UINT32 CpuSignature = GetCpuSignature();
    UINT8  CpuFlags = (UINT8)(Shr64(ReadMsr(0x17), 50)) & 7;
    UINT8  UcFlag = 1 << CpuFlags;

    //Check for corrupt firmware volume.
    if (((EFI_FIRMWARE_VOLUME_HEADER*)Fv)->Signature != 'HVF_') return 0;
    if (pEndOfFv <= pFfs) return 0;

    while(TRUE) {
    	  // ...AMI_OVERRIDE...  >>>
          // if (*(UINT32*)pFfs == 0xffffffff) return 0;
    	  // ...AMI_OVERRIDE...  <<<
        if (pFfs >= pEndOfFv) return 0;

        FfsSize = *(UINT32*)&((EFI_FFS_FILE_HEADER*)pFfs)->Size & 0xffffff;

        //Find Microcode file
        if (guidcmp(&((EFI_FFS_FILE_HEADER*)pFfs)->Name, &gMicrocodeFfsGuid) == 0)
            break;
        if ((pFfs + FfsSize) <= pFfs) return 0; //Corruption?
        pFfs +=  FfsSize;  //Next file.
        pFfs = (UINT8*)(((UINT32)pFfs + 7) & ~7);  //Align on 8 byte boundary.
    }

    pEndOfFfs = pFfs + FfsSize;

    //Find Microcode
    pUc = (MICROCODE_HEADER*)(pFfs + sizeof(EFI_FFS_FILE_HEADER));

    while(TRUE) {
        if (pUc->HeaderVersion != 1) return 0;  //End of microcode or corrupt.
        UcSize = pUc->DataSize ? pUc->TotalSize : 2048;

        if (pUc->CpuSignature == CpuSignature && (pUc->Flags & UcFlag))
            break;

        if (pUc->TotalSize > (pUc->DataSize + 48)) {        //Extended signature count.
            MICROCODE_EXT_PROC_SIG_TABLE *SigTable = (MICROCODE_EXT_PROC_SIG_TABLE*)((UINT8*)pUc + pUc->DataSize + 48);
            UINT32 ExtSigCount = SigTable->Count;
            UINT8 i;

            if (ExtSigCount >= 20) return 0;    //Corrupt microcode.

            for (i = 0; i < ExtSigCount; ++i) {
                if (SigTable->ProcSig[i].CpuSignature == CpuSignature && (SigTable->ProcSig[i].Flags & UcFlag)) return pUc;
            }
        }

#if PACK_MICROCODE
        UcSize = (UcSize + (16 - 1)) & ~(16 - 1);
#else
        UcSize = (UcSize + (MICROCODE_BLOCK_SIZE - 1)) & ~(MICROCODE_BLOCK_SIZE - 1);
#endif
        if (((UINT64)(UINTN)pUc + UcSize) >= (UINT64)(UINTN)pEndOfFfs) return 0;   //End of uc or corrupt

        pUc = (MICROCODE_HEADER*)((UINT8*)pUc + UcSize);
    }
    return pUc;
}

/**
    Find the microcode address for the CPU calling this.

    @param VOID

    @retval VOID 0 if not found or the Microcode Address.
**/

VOID * CPULib_FindMicrocode()
{
    VOID *Microcode;
#if MICROCODE_SPLIT_BB_UPDATE
    Microcode =  CPULib_FindMicrocodeInFv((VOID*)FV_MICROCODE_UPDATE_BASE);
    if (Microcode == NULL) Microcode = CPULib_FindMicrocodeInFv((VOID*)FV_MICROCODE_BASE);
#else
    Microcode =  CPULib_FindMicrocodeInFv((VOID*)FV_MICROCODE_BASE);
#endif
    return Microcode;
}

/**
    Get Microcode Version

    @param VOID

    @retval UINT32 Microcode Version
**/

UINT32  CPULib_GetMicrocodeVer()
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;

    //Clear IA32_BIOS_SIGN_ID of microcode loaded.
    WriteMsr(MSR_IA32_BIOS_SIGN_ID, 0);

    //Reading CPU ID 1, updates the MSR to the microcode revision.
    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);

    return (UINT32)Shr64(ReadMsr(MSR_IA32_BIOS_SIGN_ID), 32);
}

/**
    Get CPU frequency.

    @param VOID

    @retval UINT32 Frequency in MHz
**/

UINT32 CPULib_GetFreq()
{
    UINT32  CpuSignature = GetCpuSignature();
    UINT32  CpuSigNoVer = CpuSignature & 0xfffffff0;
    BOOLEAN IsSandyBridge = CpuSigNoVer == SANDY_BRIDGE || CpuSigNoVer == JAKETOWN || CpuSigNoVer == IVY_BRIDGE;
    UINT32  Bclk = IsSandyBridge ? 100 : 133;   //Base clock frequency in MHz
    UINT32  Freq;

    Freq = (UINT32)ReadMsr(0x198);  //Get Clock multiplier which is embedded in this value.
    if (IsSandyBridge) Freq >>= 8;  //Sandy Bridge [15:8], Otherwise [7:0]
    Freq = (Freq & 0xff) * Bclk;    //Frequency = Multiplier * Bclk

    return Freq;
}

/**
    Returns the CPU voltage in mV.

    @param VOID

    @retval UINT32 CPU Voltage (mV)
**/

#define SHARKBAY 0x306C0
UINT32 CpuLib_GetCpuVoltage()
{
    UINT32  Voltage;
    UINT32  CpuSignature = GetCpuSignature();
    UINT32  CpuSigNoVer = CpuSignature & 0xfffffff0;
    BOOLEAN IsSandyBridge = CpuSigNoVer == SANDY_BRIDGE || CpuSigNoVer == JAKETOWN \
    						|| CpuSigNoVer == IVY_BRIDGE || CpuSigNoVer == SHARKBAY;
    if (!IsSandyBridge) return 0;

    Voltage = (UINT32)(Shr64(ReadMsr(MSR_IA32_PERF_STATUS), 32)) & 0xffff;

    //Convert MSR to mV.
    Voltage = (Voltage * 1000) >> 13;

    return Voltage;
}

/**
    Program 0xc0000 - 0xfffff regions to Lock/Unlock. Not used.
 
    @param StartAddress .
    @param Length .
    @param Setting .
    @param Granularity .

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS CPUProgramPAMRegisters(
    IN EFI_BOOT_SERVICES       *gBS,
    IN EFI_RUNTIME_SERVICES    *gRS,
    IN UINT32  StartAddress,
    IN UINT32  Length,
    IN UINT8   Setting,
    IN UINT32  *Granularity
)
{
    return EFI_SUCCESS;
}

/**
    Determine if CPU supports Timed Mwait.
 
    @param VOID

    @retval True if Timed Mwait supported.
**/

UINT8 IsTimedMwaitSupported ()
{
  UINT8 MwaitSupportFlag;
  MwaitSupportFlag = (UINT8)(RShiftU64(AsmReadMsr64(MSR_PLATFORM_INFO),37) & 0x01);	
  return MwaitSupportFlag;
}

/**
    Determine if CPU supports Hardware P-States.
 
    @param VOID

    @retval True if Hardware P-States supported.
**/


BOOLEAN IsHwpSupported ()
{
  return (BOOLEAN)(RShiftU64(AsmReadMsr64(MSR_MISC_PWR_MGMT),6) & 0x01);
}

/**
    Determine if CPU supports Fivr SSC.
 
    @param VOID

    @retval TRUE  if SGX supported
  	@retval FALSE if SGX is not supported
**/

BOOLEAN AmiIsSgxSupported()
{
	UINT32 RegEax, RegEbx, RegEcx, RegEdx;
	//
	// Proccessor support SGX feature by reading CPUID.(EAX=7,ECX=0):EBX[2]
	//
	AsmCpuidEx (0x7, 0, &RegEax, &RegEbx, &RegEcx, &RegEdx);
	///
	/// SGX feature is supported only on SKL and later,
	/// with CPUID.(EAX=7,ECX=0):EBX[2]=1
	/// PRMRR configuration enabled, MSR IA32_MTRRCAP (FEh) [12] == 1
	///
	if (((RegEbx & BIT2)) && (AsmReadMsr64 (MSR_IA32_MTRRCAP) & BIT12)) {
		return TRUE;
	}
	//
	// Continue patch load without PRMRR initialization.
	// Patch load will be successful only if this is not SGX patch
	//
	return FALSE;
}

/**
    Get the PRMRR support size for the platform policy.

    @param VOID

    @retval UINT32 Support size
**/

UINT32 GetPrmrrSupportSize() {
    
   UINT32 PrmrrSupportSize = (Shl64(AsmReadMsr64(MSR_PRMRR_VALID_CONFIG), 20)) & 0xE000000;
   
   switch (PrmrrSupportSize) {
      case 0xE000000: return 0xE;
      case 0x6000000: return 0x6;
      case 0x2000000: return 0x2;
      default: return 2;
   }
}

/**
    Get maximum turbo ratio in unit MHz.
 
    @param VOID

    @retval UINT32 maximum turbo ratio in MHz.
**/

UINT32 GetMaximumTurboFrequency ()
{
  UINT32  Bclk = 100; //Base clock frequency in MHz
  UINT32  Freq;
  
  if (IsTurboModeSupported()) //if CPU supports Turbo mode, then read MSR_TURBO_RATIO_LIMIT_1C , 
      Freq = (UINT32)(AsmReadMsr64(MSR_TURBO_RATIO_LIMIT) & MAX_RATIO_LIMIT_MASK);
  else                        //otherwise read MAX_NON_TURBO_RATIO instead
      Freq = (UINT32)RShiftU64(AsmReadMsr64(MSR_PLATFORM_INFO), MAX_NON_TURBO_RATIO_OFFSET) & MAX_NON_TURBO_RATIO_MASK;
  
  Freq = Freq * Bclk;
  return Freq;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************


