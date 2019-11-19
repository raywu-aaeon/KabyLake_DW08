//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CpuDxe.c
    Installs CPU Info Protocol and initializes the
    processor interrupt vector table. The CPU Architectural
    Protocol enables/disables/get state of interrupts, set
    memory range cache type, and installs/uninstalls
    interrupt handlers.

**/

#include "CpuDxe.h"

EFI_GUID gHobListGuid               = HOB_LIST_GUID;
EFI_GUID gEfiMpServicesProtocolGuid = EFI_MP_SERVICES_PROTOCOL_GUID;
EFI_GUID gAmiCpuinfoHobGuid         = AMI_CPUINFO_HOB_GUID;
EFI_GUID gAmiCpuInfoProtocolGuid    = AMI_CPU_INFO_PROTOCOL_GUID;
EFI_GUID gAmiCpuInfo2ProtocolGuid   = AMI_CPU_INFO_2_PROTOCOL_GUID;

EFI_MP_SERVICES_PROTOCOL *gEfiMpServicesProtocol;
EFI_PROCESSOR_INFORMATION *gEfiMpProcContext;

UINTN                     gNumOfCpus;  
UINT32                    gNumOfCpuCores;
UINT32                    gNumOfThreads;

EFI_EVENT 	gAmiMpEvent;
VOID		*gAmiMpEventRegistration = 0;

static VOID   *gAcpiData;
static UINT32 gAcpiDataNumEntries;

static CHAR8 gProcObjPath = '\0';

CPUINFO_HOB             *gCpuInfoHob;
AMI_CPU_INFO            *gAmiCpuInfo;
VOID 			        *gSetupHandle;

/**
    Calculate CPU time period.

    @param VOID

    @retval UINT64 Timer Period
**/

UINT64 CalculateTimerPeriod()
{
    UINT64  Timer1;
    UINT64  Timer2;

    EFI_TPL OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);

    Timer1 = ReadRtdsc();

    pBS->Stall(DELAY_CALCULATE_CPU_PERIOD);

    Timer2 = ReadRtdsc();

    pBS->RestoreTPL(OldTpl);

    return Div64(DELAY_CALCULATE_CPU_PERIOD * (UINT64)1000000000, (UINT32)(Timer2-Timer1),NULL);    //50us * 10E15  femtosec;
}

/**
    Protocol function to get Cpu Info.

    @param This A pointer to the AMI_CPU_INFO_PROTOCOL
        instance.
    @param Cpu Cpu number
    @param Info A pointer to AMI_CPU_INFO data.

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetCpuInfo(
    IN AMI_CPU_INFO_PROTOCOL    *This,
    IN UINTN                    Cpu,
    OUT AMI_CPU_INFO            **Info
)
{
    if (Cpu >= gNumOfCpus) return EFI_INVALID_PARAMETER;
    *Info = &gAmiCpuInfo[Cpu];
    return EFI_SUCCESS;
}

AMI_CPU_INFO_PROTOCOL gAmiCpuInfoProtocol = {GetCpuInfo};

static CACHE_DESCRIPTOR_INFO gZeroCacheDesc = {0, 0, 0, 0, 0};

typedef struct {
    UINT32 NumCores;    //Number of cores in Package.
    UINT32 NumEntries;  //Each cache descriptor should have same number of entries, max of 4.
    AMI_CPU_INFO_2_CACHE_DESCR PkgCacheDesc[5];   //Total of the cores // EIP106753
    AMI_CPU_INFO_2_CACHE_DESCR CoreCacheDesc[5];  //Internal implementation assumes symmetry among cores.
                                                  //If this every changes, internal implementation will be updated.
} PKG_CACHE_DESCR;

static PKG_CACHE_DESCR *gPkgCacheDesc; //Array of Package Cache Descriptions.


typedef struct {
    UINT32 CacheType:5;
    UINT32 CacheLevel:3;
    UINT32 NotUsed:24;
} CPUID4_EAX_CACHE_INFO;

typedef struct {
    UINT32  LineSize:12;
    UINT32  Partitions:10;
    UINT32  Ways:10;
} CPUID4_EBX_CACHE_INFO;

#define MAX_NUM_CACHE_DESC  8

/**
    Pointer to internal Package information.

    @param PkgPtr Internal package information.

    @retval UINT32 Internal Core information.
**/

UINT32 *FindPtrToPrivCpuInfoPkg(
    IN UINT32 Package
)
{
    UINT32 *p = (UINT32*)(gPrivateAmiCpuInfo2 + 1);
    UINT32 i;

    if (Package >= *p) return (UINT32*)-1;   //Package does not exist.
    p++; //now p = Num cores of package 0.

    //Skip entries for previous packages.
    for (i = 0; i < Package; ++i) {
        UINT32 NumCores = *p++;    //p = now number of threads
        UINT32 j;
        for (j = 0; j < NumCores; ++j) {
            UINT32 NumThreads = *p++;
            p += NumThreads * PRIVATE_INFO_NUM_OF_CPU_DATA;
        }
    }
    return p;
}

/**
    Pointer to internal Core information.
   
    @param PkgPtr Internal package information.
    @param Core 

    @retval UINT32 Internal Core information.
**/

UINT32 *FindPtrToPrivCpuInfoCore(
    IN UINT32 *PkgPtr,
    IN UINT32 Core
)
{
    UINT32 *p = PkgPtr;
    UINT32 NumCores = *p++;
    UINT32 i;
    if (Core >= NumCores) return (UINT32*)-1;   //Core does not exist.

    //Skip previous cores.
    for (i = 0; i < Core; ++i) {
        UINT32 NumThreads = *p++;
        p += NumThreads * PRIVATE_INFO_NUM_OF_CPU_DATA;
    }
    return p;
}

/**
    Pointer to internal Core information.
  
    @param CorePtr Internal core information.
    @param Thread 

    @retval UINT32 Internal thread information.
**/

UINT32 *FindPtrToPrivCpuInfoThread(
    IN UINT32 *CorePtr,
    IN UINT32 Thread
)
{
    UINT32 *p = CorePtr;
    UINT32 NumThreads = *p++;
    if (Thread >= NumThreads) return (UINT32*)-1;   //Thread does not exist.
    p += Thread * PRIVATE_INFO_NUM_OF_CPU_DATA;
    return p;
}

/**
    Get socket number from Apic ID.
 
    @param ApicId Apic id

    @retval UINT32 Physical Socket Id
**/

UINT32 GetBoardSocketNumber(IN UINT32 ApicId)
{
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT8  MaxThreadsPackage;

    CPULib_CpuID(1, &RegEax, &RegEbx, &RegEcx, &RegEdx);

    MaxThreadsPackage = (UINT8)(RegEbx >> 16);
    
    return ApicId / MaxThreadsPackage;
}

/**
    Get the number of packages populated and sockets.
  
    @param This A pointer to the AMI_CPU_INFO_2_PROTOCOL
        instance.
    @param NumPopulatedPackages Number of populated packages
    @param NumBoardSockets OPTIONAL - Number of board sockets

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetNumPackages(
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    OUT UINT32                  *NumPopulatedPackages,
    OUT UINT32                  *NumBoardSockets OPTIONAL
)
{
    //First entry after protocol functions is number of discovered packages.
    /*UINT32 *p = (UINT32*)(gPrivateAmiCpuInfo2 + 1);

    *NumPopulatedPackages = *p;
    //NUMBER_CPU_SOCKETS must be ported if more than 1 socket.
    ASSERT(*NumPopulatedPackages <= NUMBER_CPU_SOCKETS);
    if (NumBoardSockets) {
        *NumBoardSockets = NUMBER_CPU_SOCKETS;

        //In case of porting error, Board sockets can never be less than Populated packages.
        if (*NumPopulatedPackages > *NumBoardSockets) *NumBoardSockets = *NumPopulatedPackages;
    }*/
    
    if(NumPopulatedPackages == NULL)
        return EFI_INVALID_PARAMETER;
        
    *NumPopulatedPackages = 1;

   if (NumBoardSockets)
        *NumBoardSockets = 1;
    
    return EFI_SUCCESS;
}

/**
    Get the number of Cores

    @param This A pointer to the AMI_CPU_INFO_2_PROTOCOL
        instance.
    @param Package 
    @param NumEnabledCores Number of enabled cores.
    @param NumEnabledThreads This value is total for package.
    @param NumDisabledCores OPTIONAL - Flag must be set in Protocol.
    @param NumEnabledThreads OPTIONAL - Flag must be set in Protocol.

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetNumCoresThreads(
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    IN UINT32                   Package,
    OUT UINT32                  *NumEnabledCores,
    OUT UINT32                  *NumEnabledThreads,
    OUT UINT32                  *NumDisabledCores OPTIONAL,
    OUT UINT32                  *NumDisabledThreads OPTIONAL
)
{
    UINT32 *p = FindPtrToPrivCpuInfoPkg(Package);
    UINT32 TotNumCores;
    UINT32 TotNumThreads = 0;
    UINT32 i;

    if (NumEnabledCores == NULL || NumEnabledThreads == NULL)
        return EFI_INVALID_PARAMETER;
        
    if (p == (UINT32*) -1) return EFI_INVALID_PARAMETER;

    //After package is number of cores.
    TotNumCores = *p++;
    for (i = 0; i < TotNumCores; ++i) {
        UINT32 NumThreads = *p++; //After core is Number of Threads
        TotNumThreads += NumThreads;
        p += NumThreads * PRIVATE_INFO_NUM_OF_CPU_DATA;    //APIC ID and CPU NUM;
    }

    *NumEnabledCores = TotNumCores;
    *NumEnabledThreads = TotNumThreads;

    if (NumDisabledCores) *NumDisabledCores = NumSupportedCpuCores() - TotNumCores;        
    if (NumDisabledThreads) *NumDisabledThreads = NumSupportedCpuThreads() - TotNumThreads;    

    return EFI_SUCCESS;
}

/**
    Get the number of Threads
    
    @param This A pointer to the AMI_CPU_INFO_2_PROTOCOL
        instance.
    @param Package 
    @param Core 
    @param NumEnabledThreads Number of enabled threads.
    @param NumDisabledThreads OPTIONAL - Flag must be set if valid

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetNumThreads(
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    IN UINT32                   Package,
    IN UINT32                   Core,
    OUT UINT32                  *NumEnabledThreads,
    OUT UINT32                  *NumDisabledThreads OPTIONAL
)
{
    UINT32 *p = FindPtrToPrivCpuInfoPkg(Package);
    
    if (NumEnabledThreads == NULL) return EFI_INVALID_PARAMETER;
    
    if (p == (UINT32*) -1) return EFI_INVALID_PARAMETER;

    p = FindPtrToPrivCpuInfoCore(p, Core);
    if (p == (UINT32*) -1) return EFI_INVALID_PARAMETER;

    *NumEnabledThreads = *p;
    if (NumDisabledThreads) *NumDisabledThreads = 0;

    return EFI_SUCCESS;
}

/**
    Get SBSP
  
    @param This A pointer to the AMI_CPU_INFO_2_PROTOCOL
        instance.
    @param Package Socket number information
    @param Core Core number information
    @param Thread Thread number information

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetSbsp(
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    OUT UINT32                  *Package,
    OUT UINT32                  *Core,
    OUT UINT32                  *Thread
)
{
    //Desktop system, it must be package0, core0, thread0
    if (Package == NULL || Core == NULL || Thread == NULL)
        return EFI_INVALID_PARAMETER;
        
    *Package = 0;
    *Core = 0;
    *Thread = 0;
    
    return EFI_SUCCESS;
}

/**
    Get Apic Number and Version
  
    @param This A pointer to the AMI_CPU_INFO_2_PROTOCOL
        instance.
    @param Package Socket number.
    @param Core Core number
    @param Thread Thread number
    @param ApicId Apic id
    @param ApicVer OPTIONAL

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetApicInfo(
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    IN UINT32                   Package,
    IN UINT32                   Core,
    IN UINT32                   Thread,
    OUT UINT32                  *ApicId,
    OUT UINT32                  *ApicVer OPTIONAL
)
{
    UINT32 *p = GetPtrToPrivateAmiCpuInfo2Entry(
        Package, Core, Thread);
    
    if (ApicId == NULL) return EFI_INVALID_PARAMETER;
    
    if (p == (UINT32*) -1) return EFI_INVALID_PARAMETER;

    //p points to 32-bit APIC ID and 32-bit CPU Num for internal structures.

    *ApicId = *p++;

    if (ApicVer) {
        *ApicVer = (UINT8)MemRead32((UINT32*)(UINTN)(LOCAL_APIC_BASE + APIC_VERSION_REGISTER));
    }

    return EFI_SUCCESS;
}

/**
    Get Cpu ACPI information.
  
    @param This A pointer to the AMI_CPU_INFO_2_PROTOCOL
        instance.
    @param AcpiData ACPI Data
    @param NumEntries Number of Entries in data.

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetAcpiInfo(
	IN AMI_CPU_INFO_2_PROTOCOL  *This,
    OUT VOID                    **AcpiData,
    OUT UINT32                  *NumEntries
)
{   
    if (AcpiData == NULL || NumEntries == NULL)
        return EFI_INVALID_PARAMETER;
        
    *AcpiData = gAcpiData;
    *NumEntries = gAcpiDataNumEntries;
    return EFI_SUCCESS;
}

/**
    Get the package Cache Information
  
    @param This A pointer to the AMI_CPU_INFO_2_PROTOCOL
        instance.
    @param Package Socket number. Intenal socket number (continous)
    @param Description Updates pointer to pointer with pointer to Cache information. 
    @param NumEntries Number of AMI_CPU_INFO_2_CACHE_DESCR Entries.

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetPackageCacheDescr(
    IN AMI_CPU_INFO_2_PROTOCOL      *This,
    IN UINT32                       Package,
    OUT AMI_CPU_INFO_2_CACHE_DESCR  **Description,
    OUT UINT32                      *NumEntries
)
{
    PKG_CACHE_DESCR *PkgCacheDesc;

    if (Package >= NumberOfCpuSocketsPopulated()) return EFI_INVALID_PARAMETER;
    
    if (Description == NULL || NumEntries == NULL)
        return EFI_INVALID_PARAMETER;

    PkgCacheDesc = &gPkgCacheDesc[Package];
    *Description = &PkgCacheDesc->PkgCacheDesc[0];
    *NumEntries = PkgCacheDesc->NumEntries;
    return EFI_SUCCESS;
}

/**
    Get the Core Cache Information
   
    @param This A pointer to the AMI_CPU_INFO_2_PROTOCOL
        instance.
    @param Package Socket number. Internal socket number (continous)
    @param Core Core number. Internal core number (continous)
    @param Description Updates pointer to pointer with pointer to Cache information. 
    @param NumEntries Number of AMI_CPU_INFO_2_CACHE_DESCR Entries.

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GetCoreCacheDescr(
    IN AMI_CPU_INFO_2_PROTOCOL      *This,
    IN UINT32                       Package,
    IN UINT32                       Core,
    OUT AMI_CPU_INFO_2_CACHE_DESCR  **Description,
    OUT UINT32                      *NumEntries
)
{
    PKG_CACHE_DESCR *PkgCacheDesc;    

    if (Package >= NumberOfCpuSocketsPopulated()) return EFI_INVALID_PARAMETER;
    
    if (Description == NULL || NumEntries == NULL)
        return EFI_INVALID_PARAMETER;
    
    PkgCacheDesc = &gPkgCacheDesc[Package];
    
    if (Core >= PkgCacheDesc->NumCores) return EFI_INVALID_PARAMETER;

    *Description = &PkgCacheDesc->CoreCacheDesc[0];
    *NumEntries = PkgCacheDesc->NumEntries;
    return EFI_SUCCESS;
}

PRIVATE_AMI_CPU_INFO_2_PROTOCOL PrivateAmiCpuInfo2Init = {
    {
        AMI_CPU_INFO_2_PROTOCOL_VERSION,          //ProtocolVer
        0,          //Flags
        GetNumPackages,
        GetNumCoresThreads,
        GetNumThreads,
        GetSbsp,
        GetApicInfo,
        GetAcpiInfo,
        GetPackageCacheDescr,
        GetCoreCacheDescr
    }
    //Additional information will allocated.
};

PRIVATE_AMI_CPU_INFO_2_PROTOCOL *gPrivateAmiCpuInfo2;

/**
    Fill CPU information in Private Ami Cpu Info structure.

    @param VOID

    @retval VOID
**/

VOID FillPrivateAmiCpuInfo2()
{
    //See PRIVATE_AMI_CPU_INFO_2_PROTOCOL defintion for comments on internal CPU information.

    UINT32 *p = (UINT32*)(gPrivateAmiCpuInfo2 + 1);
    UINT32 NumSockets = 1;
    UINT32 CpuNum = 0;
    UINT32 i;
    UINT32 j;
    UINT32 k;

    *gPrivateAmiCpuInfo2 = PrivateAmiCpuInfo2Init;
    *p++ = NumSockets;
    for (i = 0; i < NumSockets; ++i) {
        *p++ = gNumOfCpuCores;        
        for (j = 0; j < gNumOfCpuCores; ++j) {            
            *p++ = gNumOfThreads;
            for (k = 0; k < gNumOfThreads; ++k) {                
                *p++ = (UINT32)(gEfiMpProcContext[CpuNum].ProcessorId);
                *p++ = CpuNum;
                ++CpuNum;
                ASSERT(CpuNum <= gNumOfCpus);
            }
        }
    }
}

/**
    Get pointer to APIC/Cpu Num
  
    @param Package Socket number. Internal socket number (continous) 
    @param Core Core number. Internal core number (continous)
    @param Thread Thread number.

    @retval UINT32 Pointer to APIC/Cpu Num data structure.
**/

UINT32 * GetPtrToPrivateAmiCpuInfo2Entry(
    IN UINT32 Package,
    IN UINT32 Core,
    IN UINT32 Thread
)
{
    UINT32 *p;

    p = FindPtrToPrivCpuInfoPkg(Package);
    if (p == (UINT32*) -1) return (UINT32*)-1;   //Package does not exist.

    p = FindPtrToPrivCpuInfoCore(p, Core);
    if (p == (UINT32*) -1) return (UINT32*)-1;   //Core does not exist.

    p = FindPtrToPrivCpuInfoThread(p, Thread);
    return p;
}

/**
    Get CPU Package/Core/Thread by CPU Number. Number sequencial to APIC ID.
   
    @param CpuNum Cpu number
    @param Package Socket number. Internal socket number (continous) 
    @param Core Core number. Internal core number (continous)
    @param Thread Thread number.

    @retval BOOLEAN If found, return TRUE.
**/

BOOLEAN GetCpuPkgCoreThrdByNum(
    IN UINT32 CpuNum,
    OUT UINT32 *Package,
    OUT UINT32 *Core,
    OUT UINT32 *Thread
)
{
    UINT32 *p = (UINT32*)(gPrivateAmiCpuInfo2 + 1);
    UINT32 NumPkgs = *p++;
    UINT32 Pkg;

    for (Pkg = 0; Pkg < NumPkgs; ++Pkg) {
        UINT32 NumCores = *p++;
        UINT32 Cor;
        for (Cor = 0; Cor < NumCores; ++Cor) {
            UINT32 NumThrds = *p++;
            UINT32 Thrd;
            for (Thrd = 0; Thrd < NumThrds; ++Thrd) {
                ++p;    //Skip ApicId;
                if (*p++ == CpuNum) {
                    *Package = Pkg;
                    *Core = Cor;
                    *Thread = Thrd;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/**
    Fill ACPI Data structure
  
    @param AcpiProcData Acpi processor data.
    @param Package Socket number.
    @param Core Core number.
    @param Thread Thread number.
    @param Bsp True is Bsp.

    @retval BOOLEAN If filled, return TRUE.
**/

BOOLEAN FillAcpiData(
    IN OUT ACPI_PROCESSOR_INFO *AcpiProcData,
    IN UINT32 Package,
    IN UINT32 Core,
    IN UINT32 Thread,
    IN BOOLEAN Bsp
)
{
    UINT32 ApicId;
//    UINT32 CpuNum;
    UINT32 CpuSignature, CpuIdEBX, CpuIdECX, FeatureFlags;

    static UINT32 ProcId = 1;

    UINT32 *ptr = GetPtrToPrivateAmiCpuInfo2Entry(Package, Core, Thread);
    if (ptr == (UINT32*) -1) return FALSE;
    //ptr points to 32-bit APIC ID and 32-bit CPU Num for internal structures.

    ApicId = *ptr++;
//    CpuNum = *ptr;
    CPULib_CpuID(1, &CpuSignature, &CpuIdEBX, &CpuIdECX, &FeatureFlags);

    AcpiProcData->Type = ACPI_PROCESSOR_INFO_TYPE;    //0
    AcpiProcData->Length = sizeof(ACPI_PROCESSOR_INFO);
    AcpiProcData->Enable = 1;
    AcpiProcData->Bsp = Bsp;
    AcpiProcData->Package = Package;
    AcpiProcData->Core = Core;
    AcpiProcData->Thread = Thread;
    AcpiProcData->ApicId = ApicId;      //LAPIC number for processor.
    //AcpiProcData->ApicVer = gCpuInfoHob->Cpuinfo[CpuNum].ApicVer;
    AcpiProcData->ApicVer = (UINT8)MemRead32((UINT32*)(UINTN)(LOCAL_APIC_BASE + APIC_VERSION_REGISTER));
    
    AcpiProcData->CpuSignature = CpuSignature;
    AcpiProcData->FeatureFlags = FeatureFlags & (BIT0 | BIT7 | BIT8 | BIT9);
    AcpiProcData->ProcId = ProcId;     //ASL processor object ID.
    //AcpiProcData->ProcObjPath = (EFI_PHYSICAL_ADDRESS)(UINTN)&gProcObjPath;  //ASL processor object ID.
    AcpiProcData->LocalApicType = FALSE;            //All processors will either be xAPIC or x2APIC Mode not mixed.   

#if (LAPIC_QUANTITY > 0)
    if (LAPIC_0_INT_TYPE == 1)
    {
        AcpiProcData->NmiFlags = (LAPIC_0_TRIGGER_MODE << 2) | LAPIC_0_POLARITY;
        AcpiProcData->LintnPin = LAPIC_0_DEST_LINTIN;
    }
#endif
#if (LAPIC_QUANTITY > 1)
    if (LAPIC_1_INT_TYPE == 1)
    {
        AcpiProcData->NmiFlags = (LAPIC_1_TRIGGER_MODE << 2) | LAPIC_1_POLARITY;
        AcpiProcData->LintnPin = LAPIC_1_DEST_LINTIN;
    }
#endif
#if (LAPIC_QUANTITY > 2)
    if (LAPIC_2_INT_TYPE == 1)
    {
        AcpiProcData->NmiFlags = (LAPIC_2_TRIGGER_MODE << 2) | LAPIC_2_POLARITY;
        AcpiProcData->LintnPin = LAPIC_2_DEST_LINTIN;
    }
#endif
    
    ++ProcId;
    return TRUE;
}

/**
    Create Private Ami Cpu Info2 Acpi Data.

    @param Package Processor package
    @param Thread Processor thread (usually either 0 or 1 for HT)
    @param Bsp True is Bsp.
    @param BspCore Bsp core number.

    @retval BOOLEAN TRUE if any entries added.
**/

BOOLEAN AddLocalApicCoreEntries(
    IN UINT32 Package,
    IN UINT32 Thread,
    IN BOOLEAN Bsp,
    IN UINT32 BspCore
){
    UINT32 NumEnabledCores;
    UINT32 NumEnabledThreads;
    UINT32 Core;
    BOOLEAN ValidEntry;
    AMI_CPU_INFO_2_PROTOCOL *AmiCpu2Info = (AMI_CPU_INFO_2_PROTOCOL*)gPrivateAmiCpuInfo2;
    ACPI_PROCESSOR_INFO *AcpiProcData = (ACPI_PROCESSOR_INFO *)gAcpiData;
    static UINT32 Entry = 0;

    AmiCpu2Info->GetNumCoresThreads(AmiCpu2Info, Package, &NumEnabledCores, &NumEnabledThreads, NULL, NULL);
    NumEnabledThreads = NumEnabledThreads / NumEnabledCores;

    if (Thread >= NumEnabledThreads) return FALSE;    //Different packages could have different numbers of threads;

    ValidEntry = FillAcpiData(
        &AcpiProcData[Entry],
        Package,
        BspCore,
        Thread,
        Bsp && Thread == 0
    );
    if (ValidEntry) ++Entry;

    for (Core = 0; Core < NumEnabledCores; ++Core) {
        if (Core == BspCore) continue;
        ValidEntry = FillAcpiData(
            &AcpiProcData[Entry],
            Package,
            Core,
            Thread,
            FALSE
        );
        if (ValidEntry) ++Entry;
    }

    return TRUE;
}

/**
    Create Private Ami Cpu Info2 Acpi Data.

    @param VOID

    @retval VOID
**/

VOID CreateAcpiData()
{
	EFI_STATUS  Status;
	UINT32      Package;
	UINT32      Thread;
    UINT32      BspPackage;
    UINT32      BspCore;
    UINT32      BspThread;
	UINT32      MaxPackages;
    BOOLEAN     ProcessedEntries;
    AMI_CPU_INFO_2_PROTOCOL *AmiCpu2Info = (AMI_CPU_INFO_2_PROTOCOL*)gPrivateAmiCpuInfo2;

    gAcpiDataNumEntries = (UINT32)gNumOfCpus;
    
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(ACPI_PROCESSOR_INFO) * gNumOfCpus, &gAcpiData);
    ASSERT_EFI_ERROR(Status);

    pBS->SetMem(gAcpiData, sizeof(ACPI_PROCESSOR_INFO) * gNumOfCpus, 0);
    
    Status = AmiCpu2Info->GetSbsp(
	    AmiCpu2Info,
        &BspPackage,
        &BspCore,
        &BspThread
    );
    ASSERT_EFI_ERROR(Status);

    Status = AmiCpu2Info->GetNumPackages(AmiCpu2Info, &MaxPackages, NULL);
    ASSERT_EFI_ERROR(Status);
    Thread = 0;     //Thread count for a core.

    do {            //Thread
        ProcessedEntries = FALSE;

        //Bsp is always first entry.
        if (AddLocalApicCoreEntries(BspPackage, Thread, TRUE, BspCore))
            ProcessedEntries = TRUE;

        for (Package = 0; Package < MaxPackages; ++Package) {
            if (Package == BspPackage) continue;
            if (AddLocalApicCoreEntries(Package, Thread, FALSE, BspCore))
                ProcessedEntries = TRUE;
        }
        ++Thread;
    } while (ProcessedEntries);     //No more threads
}

/**
    Get Cache information.

    @param VOID

    @retval VOID
**/

VOID CreateCacheData()
{
    AMI_CPU_INFO_2_PROTOCOL *AmiCpu2Info = (AMI_CPU_INFO_2_PROTOCOL*)gPrivateAmiCpuInfo2;
    UINT32  NumPackages;
    UINT32  Package;
    UINT32  CpuNum;
    EFI_STATUS Status;

    BOOLEAN IsLxSharedByPackage[5];
                                   
    IsLxSharedByPackage[1] = GetCacheSharedThreads(1) > 2 ? TRUE: FALSE;
    IsLxSharedByPackage[2] = GetCacheSharedThreads(2) > 2 ? TRUE: FALSE;
    IsLxSharedByPackage[3] = GetCacheSharedThreads(3) > 2 ? TRUE: FALSE;
    IsLxSharedByPackage[4] = GetCacheSharedThreads(4) > 2 ? TRUE: FALSE;
    
    Status = AmiCpu2Info->GetNumPackages(AmiCpu2Info, &NumPackages, NULL);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(PKG_CACHE_DESCR) * NumPackages, &gPkgCacheDesc);
    ASSERT_EFI_ERROR(Status);

    MemSet(gPkgCacheDesc, sizeof(PKG_CACHE_DESCR) * NumPackages, 0);

    //Assume symmetry between the cores in a package. This never likely to change.
    for(Package = 0, CpuNum = 0; CpuNum < gNumOfCpus; ++Package) {
        AMI_CPU_INFO *AmiCpuInfo    = &gAmiCpuInfo[CpuNum];
        PKG_CACHE_DESCR *PkgDesc    = &gPkgCacheDesc[Package];
        CACHE_DESCRIPTOR_INFO *CacheInfo    = AmiCpuInfo->CacheInfo;
        UINT32 NumCores             = AmiCpuInfo->NumCores;
        UINT32 Entry = 0;

        ASSERT (Package < NumPackages);

        PkgDesc->NumCores = NumCores;

        while(CacheInfo->Desc != 0) {
            AMI_CPU_INFO_2_CACHE_DESCR *PkgCacheDesc = &PkgDesc->PkgCacheDesc[Entry];
            AMI_CPU_INFO_2_CACHE_DESCR *CoreCacheDesc = &PkgDesc->CoreCacheDesc[Entry];

            ASSERT(Entry < MAX_NUM_CACHE_DESC);
            ASSERT(CacheInfo->Level < 5);

            CoreCacheDesc->LengthDesc = sizeof(AMI_CPU_INFO_2_CACHE_DESCR);
            CoreCacheDesc->Level = CacheInfo->Level;
            CoreCacheDesc->Type = CacheInfo->Type;
            CoreCacheDesc->Size = CacheInfo->Size;
            CoreCacheDesc->Associativity = CacheInfo->Associativity;
            CoreCacheDesc->Shared = IsLxSharedByPackage[CacheInfo->Level] + 1;

            if (CoreCacheDesc->Type == 3) {
                CoreCacheDesc->Type = 2;   //Translate type from AMI CPU INFO 1 to AMI CPU INFO 2.
            }

            MemCpy(PkgCacheDesc, CoreCacheDesc, sizeof(AMI_CPU_INFO_2_CACHE_DESCR));

            PkgCacheDesc->Size *= !IsLxSharedByPackage[CacheInfo->Level] ? NumCores : 1;

            ++Entry;
            ++CacheInfo;
        }
        CpuNum += NumCores * (AmiCpuInfo->NumHts ? 2 : 1);
        PkgDesc->NumEntries = Entry;
    }
}

/**
    Create Private Ami Cpu Info2 structure.

    @param VOID

    @retval VOID
**/

VOID CreatePrivateAmiCpuInfo2()
{
    EFI_STATUS Status;

    
    Status = pBS->AllocatePool(
        EfiReservedMemoryType,
        0x2000 * gNumOfCpus,
        &gPrivateAmiCpuInfo2
    );
    ASSERT_EFI_ERROR(Status);
    
    FillPrivateAmiCpuInfo2();
    
    CreateAcpiData();
    
    CreateCacheData();
}

/**
    Update cache information with CPUID 4.
  
    @param CacheInfo Array to be filled of cache info 
        structures.

    @retval VOID
**/

VOID FillCacheDesc(IN OUT CACHE_DESCRIPTOR_INFO * CacheInfo)
{
    CPUID4_EAX_CACHE_INFO EaxInfo;
    CPUID4_EBX_CACHE_INFO EbxInfo;
    UINT32 RegEcx;
    UINT32 RegEdx;
    UINT32 CacheCount;

    for (CacheCount  = 0; CacheCount < (MAX_NUM_CACHE_DESC - 1); ++CacheCount) {
        RegEcx = CacheCount;
        CPULib_CpuID(4, (UINT32*)&EaxInfo, (UINT32*)&EbxInfo, &RegEcx, &RegEdx);
        if (EaxInfo.CacheType == 0) break; //No more cache.

        CacheInfo[CacheCount].Desc = 0xff;  //Unused.
        CacheInfo[CacheCount].Level = EaxInfo.CacheLevel;
        switch (EaxInfo.CacheType) {
        case 1: CacheInfo[CacheCount].Type = 0; break;
        case 2: CacheInfo[CacheCount].Type = 1; break;
        case 3: CacheInfo[CacheCount].Type = 3; break;
        }

        CacheInfo[CacheCount].Size =
            (EbxInfo.Ways + 1) * (EbxInfo.Partitions + 1) * (EbxInfo.LineSize + 1) * (RegEcx + 1) /
             1024;
        CacheInfo[CacheCount].Associativity = EbxInfo.Ways + 1;
    }
    CacheInfo[CacheCount] = gZeroCacheDesc;
}

/**
    Create Private Ami Cpu Info1 structure.

    @param VOID

    @retval VOID
**/

VOID CreatePrivateAmiCpuInfo1()
{
    EFI_STATUS      Status;
    AMI_CPU_INFO    *AmiCpuInfo;
    UINT64          MicroCodeVersion;
    UINT32          CpuSignature;
    UINT32          i;
    UINT32          Bclk;
    UINT32          RegEAX, RegEBX, RegECX, RegEDX;
    UINT32          FeatureEcx, FeatureEdx;
    UINT32          ExtFeatureEdx;
    CHAR8           *BrandString;    
    CHAR8           *BrandString1;    
    UINT64          TimerPeriod;
    UINT8           *pos1;
    BOOLEAN         Ht0 = IsHt0();   //True, if not hyper-threaded CPU.
    //AMI_CPU_INFO    *AmiCpuInfo = &gAmiCpuInfo[Cpu];

    
      //Allocate memory for AMI_CPU_INFO. This will be filled by CPU initialization.
    Status = pBS->AllocatePool(
        EfiBootServicesData,
        sizeof(AMI_CPU_INFO) * gNumOfCpus,
        &gAmiCpuInfo
    );
    ASSERT_EFI_ERROR(Status);
    
    //Get Cpu Signature
    CpuSignature = GetCpuSignature();
    
    //Use 100 for bclk for Sandy Bridge and Ivy Bridge
    Bclk = 100;
    
    //Allocate memory for Brand string
    Status = pBS->AllocatePool(EfiBootServicesData, 49, &BrandString);
    ASSERT_EFI_ERROR(Status);
    BrandString1 = BrandString;

    //Get Brand string
    CPULib_CpuID(0x80000002, &RegEAX, &RegEBX, &RegECX, &RegEDX);
    *(UINT32*)BrandString = RegEAX; BrandString +=4;
    *(UINT32*)BrandString = RegEBX; BrandString +=4;
    *(UINT32*)BrandString = RegECX; BrandString +=4;
    *(UINT32*)BrandString = RegEDX; BrandString +=4;

    CPULib_CpuID(0x80000003, &RegEAX, &RegEBX, &RegECX, &RegEDX);
    *(UINT32*)BrandString = RegEAX; BrandString +=4;
    *(UINT32*)BrandString = RegEBX; BrandString +=4;
    *(UINT32*)BrandString = RegECX; BrandString +=4;
    *(UINT32*)BrandString = RegEDX; BrandString +=4;

    CPULib_CpuID(0x80000004, &RegEAX, &RegEBX, &RegECX, &RegEDX);
    *(UINT32*)BrandString = RegEAX; BrandString +=4;
    *(UINT32*)BrandString = RegEBX; BrandString +=4;
    *(UINT32*)BrandString = RegECX; BrandString +=4;
    *(UINT32*)BrandString = RegEDX; BrandString +=4;
    *BrandString = '\0';
    
    BrandString = BrandString1;    
        
    //Using CPUID to get related feature
    CPULib_CpuID(1, &RegEAX, &RegEBX, &FeatureEcx, &FeatureEdx);
    CPULib_CpuID(0x80000001, &RegEAX, &RegEBX, &RegECX, &ExtFeatureEdx);
    
    //Get loaded Microcode version, MSR 0x8b [EDX] = Microcode version   
    MicroCodeVersion = ReadMsr(0x8b);
    MicroCodeVersion = *((UINT32*)&MicroCodeVersion + 1); //ignore upper 32-bits.

    TimerPeriod = CalculateTimerPeriod();   //10^-15 s.
    
    for(i = 0; i < gNumOfCpus; i++ )
    {    
    
        AmiCpuInfo = &gAmiCpuInfo[i];            
        Status = pBS->AllocatePool(EfiBootServicesData, MAX_NUM_CACHE_DESC * sizeof(CACHE_DESCRIPTOR_INFO), &AmiCpuInfo->CacheInfo);
        ASSERT_EFI_ERROR(Status);
            
        FillCacheDesc(AmiCpuInfo->CacheInfo);   //Get Cache Information.
        //Remove leading spaces. After removing leading spaces, the Brand String can not be
        //freed. However, it should never be freed.
        
        AmiCpuInfo->BrandString = BrandString;
        
        while (*AmiCpuInfo->BrandString == ' ') ++AmiCpuInfo->BrandString;
        
        //Remove extra spaces in middle.
        pos1 = AmiCpuInfo->BrandString;
        
        for(;;) {
            UINT8 *pos2;
            UINT8 *pos3;
            while (*pos1 != ' ' && *pos1 != '\0') ++pos1;   //Find next space.
            if (*pos1 == '\0') break;                       //If found terminator, break.
            if (*++pos1 != ' ') continue;                   //If not second space, continue scanning.
            pos2 = pos1;                                    //Found 2 spaces.
            while(*++pos2 == ' ');                          //Skip spaces.
            pos3 = pos1;
            while(*pos2 != '\0') *pos3++ = *pos2++;         //copy string
            *pos3++ = '\0';                                 //Add terminator.
        }       
        
        AmiCpuInfo->Version      = CpuSignature;
        AmiCpuInfo->X64Supported = (ExtFeatureEdx >> 29) & 1;
        AmiCpuInfo->Ht0          = Ht0;
        AmiCpuInfo->Features     = Shl64(FeatureEcx, 32) + FeatureEdx;
        AmiCpuInfo->NumCores     = gNumOfCpuCores;
        AmiCpuInfo->NumHts       = AmiIsHtEnabled() * 2;   //Either 2 or 0.
        AmiCpuInfo->FSBFreq      = Bclk;
        AmiCpuInfo->Voltage         = 0;    //Voltage is variable, and no information os available.       
        AmiCpuInfo->MicroCodeVers   = (UINT32)MicroCodeVersion;
        AmiCpuInfo->IntendedFreq = ((UINT32)ReadMsr(0x198) >> 8) * Bclk;
        AmiCpuInfo->ActualFreq      = 1000000000/(UINT32)TimerPeriod;            
    }    
}

/**
    Cpu Dxe global data initialize.
   
    @param VOID

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS GlobalDataInitialize(VOID)
{

    EFI_STATUS  Status;
    UINT64 MsrData = ReadMsr(MSR_CORE_THREAD_COUNT);    
    EFI_PROCESSOR_INFORMATION *ptr;
    UINT32 i;
    UINTN  NumberOfEnabledProcessors;
        
    gNumOfCpuCores = ((UINT32)(MsrData >> 16 & 0xff));
    //Locate MP services protocol provided by CPU RC
    Status = pBS->LocateProtocol(&gEfiMpServicesProtocolGuid, NULL, &gEfiMpServicesProtocol);    
    ASSERT_EFI_ERROR(Status);
    
    if (!EFI_ERROR(Status)){
        //Get number of Cpus on system
        Status = gEfiMpServicesProtocol->GetNumberOfProcessors( gEfiMpServicesProtocol, &gNumOfCpus, &NumberOfEnabledProcessors );

    }else{
        return Status;
    }
    
    gNumOfThreads = (UINT32)(gNumOfCpus / gNumOfCpuCores);    
    
    DEBUG ((DEBUG_INFO, "Cpu MP service cpus = %x cores %x threads %x\n", gNumOfCpus, gNumOfCpuCores, gNumOfThreads));        
    
    //Get MP processor context of each CPU
    Status = pBS->AllocatePool(
        EfiBootServicesData,
        sizeof(EFI_PROCESSOR_INFORMATION) * gNumOfCpus,
        &gEfiMpProcContext
    );
    
    ptr = gEfiMpProcContext;
    for(i = 0; i < gNumOfCpus; i++ , ptr++)
    {
        gEfiMpServicesProtocol->GetProcessorInfo(
                                    gEfiMpServicesProtocol,
                                    i,
                                    ptr
                                ); 
    }
    
    return Status;
}    

/**
    Handler executed before OS.
 
    @param Event Event that was triggered
    @param Context Point to calling context

    @retval VOID
**/

VOID PrepareToBoot(IN EFI_EVENT Event, IN VOID *Context)
{
    //Trigger SWSMI to save CPU fixed & varible MTRRs
    IoWrite8(SW_SMI_IO_ADDRESS, SW_SMI_SAVE_MSR);
}

#if CPU_SETUP_SET_BOOT_RATIO
/**
    Adjust CPU boot ratio base on setup item

    @param VOID

    @retval VOID
**/

VOID CpuSetBootRatio(VOID)
{
    UINT8 MaxRatio = Shr64(ReadMsr(MSR_PLATFORM_INFO),8) & 0xff;
    UINT8 MinRatio = Shr64(ReadMsr(MSR_PLATFORM_INFO),40) & 0xff;
    UINT8 DesireRatio = (UINT8)GetPlatformCpuBootRatio(gSetupHandle);  
    
    if (DesireRatio == 0 || DesireRatio == 0xff)
        return;
        
    if (DesireRatio < MinRatio || DesireRatio > MaxRatio)
        DesireRatio = MaxRatio;
        
    ReadWriteMsr(MSR_IA32_MISC_ENABLE, BIT16, (UINT64)-1);    
    WriteMsr(0x199, DesireRatio << 8);
}

#endif
/**
    Ami cpu mpService callback

    @param Event Event that was triggered
    @param Context Point to calling context

    @retval EFI_STATUS Return EFI status
**/

VOID AmiCpuMpServiceCallback(IN EFI_EVENT Event, IN VOID *Context)
{
    EFI_STATUS  Status;
    EFI_EVENT   BootEvent;
    VOID        *FirstHob;
    UINT32      i;

    //Init global data for later usage
    Status = GlobalDataInitialize();
	if ( EFI_ERROR(Status) )
		DEBUG ((DEBUG_ERROR, "CpuDxe GlobalDataInitialize fail\n"));
       
    //Create AMI private CpuInfo1 and CpuInfo2 for AMI other module usage   
    CreatePrivateAmiCpuInfo1();       
    CreatePrivateAmiCpuInfo2();    
   
    Status = pBS->InstallProtocolInterface(
                    &TheImageHandle,
                    &gAmiCpuInfo2ProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    gPrivateAmiCpuInfo2
                );
	if ( EFI_ERROR(Status) )
		DEBUG ((DEBUG_ERROR, "CpuDxe Install Protocol PrivateAmiCpuInfo2 fail\n"));
                
    Status = pBS->InstallProtocolInterface(
                    &TheImageHandle,
                    &gAmiCpuInfoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gAmiCpuInfoProtocol
                ); 
	if ( EFI_ERROR(Status) )
		DEBUG ((DEBUG_ERROR, "CpuDxe Install Protocol AmiCpuInfoProtocol fail\n"));
        
    // Get CPU feature & set SetupCpuFeatures variable to hide unsupported setup items
    DxeInitPlatformCpuLib(pBS, pRS, &gSetupHandle);

    FirstHob = GetEfiConfigurationTable(pST,&gHobListGuid);
    if (!FirstHob) return;    

    //Fill CpuInfoHob
    gCpuInfoHob = (CPUINFO_HOB*)FirstHob;

    //Find CpuInfo Hob & Update it
    while (!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION,&gCpuInfoHob))) {
        if (guidcmp(&gCpuInfoHob->EfiHobGuidType.Name,&gAmiCpuinfoHobGuid) == 0) break;
    }
    if (EFI_ERROR(Status)) return;

    for (i = 0; i < gNumOfCpuCores; ++i) {                
        gCpuInfoHob->Cpuinfo[i].ApicId = (UINT8) gEfiMpProcContext[i].ProcessorId;
    }

#if CPU_SETUP_SET_BOOT_RATIO
    CpuSetBootRatio();
#endif
	
	Status = pBS->CreateEvent(
        EVT_SIGNAL_EXIT_BOOT_SERVICES,
        TPL_CALLBACK,
        PrepareToBoot,
        (VOID*)TRUE,
        &BootEvent
    );
    ASSERT_EFI_ERROR(Status);

    Status = CreateLegacyBootEvent(
        TPL_CALLBACK,
        PrepareToBoot,
        (VOID*)FALSE,
        &BootEvent
    );
    ASSERT_EFI_ERROR(Status);
	
    return;
}

/**
    Enable or disable Limit Cpuid feature.

    @param Buffer Point to the input variable.

    @retval VOID
**/

VOID
EFIAPI
ProgramCpuidLimitbit (
  IN VOID *Buffer
  )
{
	BOOLEAN *LimitCpuidEnable;
	UINT64 	MsrData;
	
	LimitCpuidEnable = (BOOLEAN *)Buffer;
	MsrData = ReadMsr(MSR_IA32_MISC_ENABLE); 
	
	if (*LimitCpuidEnable == ((BOOLEAN)(Shr64(MsrData, 22) & 0x1)))
		return;

	if (*LimitCpuidEnable == 1)
		WriteMsr(MSR_IA32_MISC_ENABLE, (MsrData | 0x400000));
	else
		WriteMsr(MSR_IA32_MISC_ENABLE, (MsrData & ~0x400000));
}

/**
    Enable or disable CPU features on Aps.

    @param Buffer Point to the input variable.

    @retval VOID
**/

VOID
EFIAPI
ProgramCpuFeatureOnAps (
  IN VOID *Buffer
  )
{
	CPU_FEATURES_STATUS *CpuFeaturesStatus;
	CpuFeaturesStatus = (CPU_FEATURES_STATUS *)Buffer;

	if (CpuFeaturesStatus->LimitCpuidSupport == TRUE)
		ProgramCpuidLimitbit(&CpuFeaturesStatus->LimitCpuidEnable);
}

/**
    Cpu callback event for program Intel processor features that are not programmed in RC.

    @param Event Event that was triggered
    @param Context Point to calling context

    @retval EFI_STATUS Return EFI status
**/

VOID ProgramFeatureNotifyEvent(IN EFI_EVENT Event, IN VOID *Context) 
{
	EFI_STATUS  Status;
	UINTN       DataSize;
	EFI_GUID	gSetupGuid = SETUP_GUID;
	UINT32      Attributes = 0;
	SETUP_DATA  SetupData;
	CPU_FEATURES_STATUS CpuFeaturesStatus = {0};
	
	pBS->CloseEvent (Event);
	
	DataSize = sizeof(SETUP_DATA);
	
	Status = pRS->GetVariable(
						L"Setup",
						&gSetupGuid,
						&Attributes,
						&DataSize,
						&SetupData
					 );
	if (EFI_ERROR(Status)) return;
	
	if (IsLimitCpuidSupported() == TRUE) {
		CpuFeaturesStatus.LimitCpuidSupport = TRUE;
		CpuFeaturesStatus.LimitCpuidEnable = SetupData.LimitCpuid;
	}
	//BSP	
	if (CpuFeaturesStatus.LimitCpuidSupport == 1)
		ProgramCpuidLimitbit(&CpuFeaturesStatus.LimitCpuidEnable);

	//APs
	Status = gEfiMpServicesProtocol->StartupAllAPs (
						  gEfiMpServicesProtocol,
						  ProgramCpuFeatureOnAps,
						  FALSE,
						  NULL,
						  0,
						  &CpuFeaturesStatus,
						  NULL
						  );
	if ( EFI_ERROR(Status) )
		DEBUG ((DEBUG_ERROR, "CpuDxe StartupAllAPs Program Cpu Feature fail\n"));
}

/**
    Cpu Dxe Entrypoint.

    @param ImageHandle - Handle assigned to this driver.
    @param SystemTable - Efi System table.

    @retval EFI_STATUS Return EFI status
**/

EFI_STATUS EFIAPI DxeInitializeCpu(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS Status;
    InitAmiLib(ImageHandle,SystemTable);
    
    Status = RegisterProtocolCallback(
    	&gEfiMpServicesProtocolGuid,
        AmiCpuMpServiceCallback,
        NULL,
        &gAmiMpEvent,
        &gAmiMpEventRegistration
    );
	
	if ( EFI_ERROR(Status) )
		DEBUG ((DEBUG_ERROR, "CpuDxe register fail\n"));

    if (IsLimitCpuidSupported()) {
    	EFI_EVENT	EndOfDxeEvent;
		pBS->CreateEventEx(
				   EVT_NOTIFY_SIGNAL, 
				   TPL_CALLBACK, 
				   ProgramFeatureNotifyEvent,
				   NULL, 
				   &gEfiEndOfDxeEventGroupGuid,
				   &EndOfDxeEvent);
	}
    
    if (AmiIsSgxSupported()) {
    	EFI_GCD_MEMORY_SPACE_DESCRIPTOR PrmrrMemorySpaceDescriptor;
    	EFI_PHYSICAL_ADDRESS 	mPrmrrBase;
    	EFI_PHYSICAL_ADDRESS 	mPrmrrMask;
    	UINT64					MemoryAttributes;
    	EFI_STATUS  			Status;
		UINTN       			DataSize;
		EFI_GUID				gSetupGuid = SETUP_GUID;
		UINT32      			Attributes = 0;
		SETUP_DATA  			SetupData;

    	mPrmrrBase = ReadMsr(MSR_UNCORE_PRMRR_PHYS_BASE);
    	mPrmrrMask = ((~(ReadMsr(MSR_UNCORE_PRMRR_PHYS_MASK) & ~(B_MSR_PRMRR_PHYS_MASK_LOCK | B_MSR_PRMRR_VALID_BIT))) + 1) & (Shl64 (AsmReadMsr64 (MSR_PRMRR_VALID_CONFIG),20));
//    	DEBUG ((DEBUG_ERROR, "CpuDxe mPrmrrBase = = %8lx, mPrmrrMask = %8lx\n", mPrmrrBase, mPrmrrMask));
    	if (mPrmrrBase != 0) {
			Status = gDS->GetMemorySpaceDescriptor (mPrmrrBase, &PrmrrMemorySpaceDescriptor);
			ASSERT_EFI_ERROR (Status);
	
			MemoryAttributes = PrmrrMemorySpaceDescriptor.Capabilities;
	
			Status = gDS->SetMemorySpaceAttributes (mPrmrrBase, mPrmrrMask, MemoryAttributes);
			if ( EFI_ERROR(Status) ) {
				DEBUG ((DEBUG_ERROR, "CpuDxe Install Set SGX Memory Space Attributes fail\n"));
//				ASSERT_EFI_ERROR(Status);
			}
    	}
		
    	//Clear PrmrrSize if SGX is disabled.
		DataSize = sizeof(SETUP_DATA);
		Status = pRS->GetVariable(
							L"Setup",
							&gSetupGuid,
							&Attributes,
							&DataSize,
							&SetupData
						 );
		
		if ((!EFI_ERROR(Status)) && (SetupData.EnableSgx == 0)) {
			SetupData.PrmrrSize = 0;
			
			Status = pRS->SetVariable (
							  L"Setup",
			                  &gSetupGuid,
			                  Attributes,
			                  sizeof (SETUP_DATA),
			                  &SetupData
			                  );
			ASSERT_EFI_ERROR(Status);
		}
    }
    
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

