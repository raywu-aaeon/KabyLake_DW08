//***********************************************************************
//*                                                                     *
//*                      Copyright (c) 1985-2021, AMI.                  *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

//*************************************************************************
/** @file RestorePartition.c
    This file will look for the specific label of partition and
    then boot from it when hotkey is pressed.

**/
//*************************************************************************


//---------------------------------------------------------------------------

#ifdef TSE_FOR_APTIO_4_50

#include "Token.h"
#include <Efi.h>

#else

#include "minisetup.h"

#endif

#include "bootflow.h"

#include <Protocol/LegacyBiosExt.h>
#include <Protocol/BlockIo.h>

#include <AmiDxeLib.h>

//---------------------------------------------------------------------------

#if RESTORE_PARTITION_FAT_SUPPORT
#define VOL_LEN sizeof(RESTORE_PARTITION_VOLUME_NAME)-1 // excludes zero of string
#endif

#if RESTORE_PARTITION_NTFS_SUPPORT
#define UNICODE_VOL_LEN sizeof(UNICODE_RESTORE_PARTITION_VOLUME_NAME)-2 // excludes zero of string
#endif

#ifndef EFI_AMI_LEGACYBOOT_PROTOCOL_GUID
#define EFI_AMI_LEGACYBOOT_PROTOCOL_GUID            \
  {0x120d28aa, 0x6630, 0x46f0, {0x81, 0x57, 0xc0, 0xad, 0xc2, 0x38, 0x3b, 0xf5}}
#endif

typedef struct _HD_PAR_STRUC
{
    UINT8   bActive;
    UINT8   bStartHd;
    UINT16  wStartCylSec;
    UINT8   bType;
    UINT8   bEndHd;
    UINT16  wEndCylSec;
    UINT32  dNumSecMbrToFirst;
    UINT32  dNumSec;
} HD_PAR_STRUC;

//---------------------------------------------------------------------------


EFI_GUID guidEfiVar = EFI_GLOBAL_VARIABLE;
EFI_GUID gEfiLegacyBiosExtProtocolGuid  = EFI_LEGACY_BIOS_EXT_PROTOCOL_GUID;
BBS_TABLE               *gRpBbsTable;           // found BBS_TABLE for this restore partition
UINT16                  gRpBbsIndex;            // Index to this BBS_TABLE
UINT16                  gRpBbsCount;            // GetBbsInfo.BbsCount
UINT8                   gRpFound;               // 1/2 = non-NTFS / NTFS restore partition found
UINT16                  gRpEntryOfs;            // Offset to MBR of restore partition
EFI_BLOCK_IO_PROTOCOL   *gRpBlkIo;              // EFI_BLOCK_IO_PROTOCOL of found restore partition
EFI_HANDLE  gBlkIoHandle = NULL;

extern EFI_BOOT_SERVICES    *gBS;
extern EFI_STATUS _BootLaunchDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath, VOID *Options, UINTN OptionSize, BOOLEAN ValidBootOption );

extern VOID * EfiLibAllocatePool(IN  UINTN   AllocationSize);
extern VOID MemFreePointer(VOID **ptr);
extern INTN EfiCompareMem(IN VOID     *MemOne,  IN VOID     *MemTwo,  IN UINTN    Length);
extern EFI_RUNTIME_SERVICES *gRT;

extern BOOLEAN OemCheckRestorePartition();

//extern BOOT_DATA    *gBootData;
extern UINTN        gPostStatus;
extern UINTN        gBootOptionCount;
extern UINT32       gBootFlow;
extern BOOLEAN      gEnterSetup;
extern BOOLEAN gSpecificPartitionFind;
extern GLOBAL_GPT_HARDDRIVE_DEVICE_PATH   gGlobalGptHdDP;

typedef struct
{
    EFI_GUID    PartitionTypeGuid;
    EFI_GUID    UniquePartitionGuid;
    EFI_LBA     StartingLba;
    EFI_LBA     EndingLba;
    UINT64      Attributes;
    CHAR16      PartitionName[36];
} PARTITION_ENTRY;

typedef struct
{
    EFI_DEVICE_PATH_PROTOCOL    Header;
    UINT32          PartitionNumber;
    UINT64          PartitionStart;
    UINT64          PartitionSize;
    UINT8           Signature[16];
    UINT8           MBRType;
    UINT8           SignatureType;
    UINT8           FileType;
    UINT8           FileSubType;
    UINT16          FileLength;
    CHAR16          FileName[sizeof(BOOTFILEPATH)/2];
    UINT8           EndType;
    UINT8           EndSubType;
    UINT16          EndLength;
} GPT_HARDDRIVE_DEVICE_PATH;

EFI_GUID OemRestorePartitionGuid = OEM_RESTORE_PARTITION_GUID;
UINT8                   gGptRpFound;
UINT32                  gAddr16;  

GPT_HARDDRIVE_DEVICE_PATH   gGptHdDP =
{
    {0, 0, {0, 0}},
    (UINT32)0,
    (UINT64)0,
    (UINT64)0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    (UINT8)0,
    (UINT8)0,
    (UINT8)0,
    (UINT8)0,
    (UINT16)0,
    {BOOTFILEPATH},
    (UINT8)0,
    (UINT8)0,
    (UINT16)0
};

VOID	SaveINT13BeforeINT19 ( IN EFI_EVENT Event, IN VOID *Context);
VOID    BootRestorePartition();

UINT8   gMbrRpFound = FALSE;
UINT8*  gMbrEfiBootDp = NULL;

#if RESPAR_EFI_BOOT_FROM_MBR || EFI_BOOTFILEPATH_CHECK
#include <Protocol/SimpleFileSystem.h>
#endif 

#if RESPAR_EFI_BOOT_FROM_MBR
typedef struct
{
    EFI_DEVICE_PATH_PROTOCOL    Header;
    CHAR16          FileName[sizeof(BOOTFILEPATH)/2];
} RESPAR_HARDDRIVE_DEVICE_PATH;
#endif

#if EFI_BOOTFILEPATH_CHECK
/**
    This function is to check if the file resides on input device(by Handle).
    @param VOID

    @retval TRUE means the boot file is avaiable.
    @retval FALSE means the boot file is not avaiable.

**/
BOOLEAN
CheckFilePathByHandle(EFI_HANDLE FsHandle, CHAR16 *FilePath){
    EFI_STATUS  Status;
    BOOLEAN     IsExisted = FALSE;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FsInterface;
    EFI_FILE_PROTOCOL *Root, *FileInterface;
    
    if(FsHandle){
        Status = pBS->HandleProtocol( FsHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**)&FsInterface );
        if(!EFI_ERROR(Status)){
            Status = FsInterface->OpenVolume(FsInterface,&Root);
            if(!EFI_ERROR(Status)){
                Status = Root->Open(Root, &FileInterface, FilePath, EFI_FILE_MODE_READ, 0);
                if(!EFI_ERROR(Status)){
                  IsExisted = TRUE;
                }
            }
        }
    }
    
    return IsExisted;
}

/**
    This function is to check if the file resides on input device(by DevicePath).
    @param VOID

    @retval TRUE means the boot file is avaiable.
    @retval FALSE means the boot file is not avaiable.

**/
BOOLEAN
CheckFilePathByDevicePath(EFI_DEVICE_PATH_PROTOCOL *BootFileOnFsDp, CHAR16 *FilePath){
    EFI_HANDLE  DpHandle = NULL;
    EFI_STATUS  Status;
    EFI_DEVICE_PATH_PROTOCOL   *DpBuffer = NULL;
	BOOLEAN IsExisted = FALSE;
    UINTN       HandleCount = 0;
    UINTN       i = 0;
    EFI_HANDLE  *HandleBuffer = NULL;
    EFI_DEVICE_PATH_PROTOCOL *SearchDp = NULL;


    DpBuffer = BootFileOnFsDp;
    
    // Compare device path with all FS protocol Handle.
    Status = gBS->LocateHandleBuffer (  ByProtocol,
                                        &gEfiSimpleFileSystemProtocolGuid,
                                        NULL,
                                        &HandleCount,
                                        &HandleBuffer  );
    for(i=0;i<HandleCount;i++){
        Status = pBS->HandleProtocol( HandleBuffer[i], &gEfiDevicePathProtocolGuid, (VOID**)&SearchDp );
        if(EFI_ERROR(Status)){
            continue;
        }
        if(DPLength(DpBuffer) != DPLength(SearchDp)){
            continue;
        }
        if(!MemCmp((VOID*)DpBuffer,(VOID*)SearchDp, DPLength(DpBuffer))){
            DpHandle = HandleBuffer[i];
            break;
        }
    }
    
    if(DpHandle){
        IsExisted = CheckFilePathByHandle(DpHandle, FilePath);
    }
    
    MemFreePointer((VOID **)&HandleBuffer);        

    return  IsExisted;
}
#endif

#if RESPAR_EFI_BOOT_FROM_MBR
BOOLEAN CheckValidMbr(IN UINT8 *MbrBuffer);
UINT8   CheckRestorePbr(IN EFI_BLOCK_IO_PROTOCOL    *blkIo, IN UINT8 *PbrBuffer, IN UINT64 PbrLba);

/**
    This function checks if partition header is a valid.

    @param PhBuffer points to partition header that triggered this call

    @retval TRUE Valid partiton header
    @retval FALSE invalid partition header

**/
BOOLEAN CheckValidPartHeader(IN UINT8* PhBuffer){
    if(((HD_PAR_STRUC*)PhBuffer)->bType == 0 ||
        ((HD_PAR_STRUC*)PhBuffer)->dNumSecMbrToFirst == 0 ||
        ((HD_PAR_STRUC*)PhBuffer)->dNumSec == 0)
        return FALSE;
    return TRUE;
}

/**
    This function checks the volume label name of partition to see
    if matches RESTORE_PARTITION_VOLUME_NAME. This is for EFI Restore-
    Partition image on MBR partition.

    @param VOID 

    @retval 0 It is NOT restore partition or ERROR return;
    @retval 1 restore partition found

    @note  gMbrRpFound and gMbrEfiBootDp modified

**/
UINT8 CheckMbrRestorePartitionForEfiBoot(){
    UINT64  PbrLba = 0;
    UINT64  ExtPbrTableBase = 0;
    UINT64  ExtPbrTableOffset = 0;
    UINT16  RpEntryOfs = 0;
    UINT8   *MbrBuffer = NULL;
    UINT8   *PbrBuffer = NULL;
    UINT8   *ExtPbrTableBuffer = NULL;
    EFI_STATUS  Status;
    UINT32  PartStart = 0;
    UINT32  PartSize = 0;
    UINTN   HandleCount = 0 ;
    EFI_HANDLE  *HandleBuffer = NULL;
    UINTN   FsHandleIdx = 0;
    BOOLEAN RpFound = FALSE;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePathNode = NULL;

    // The first buffer with the BlockSize of gRpBlkIo is for MbrBuffer, 
    // the second is for PbrBuffer and the last is for ExtPbrTableBuffer.
    MbrBuffer = EfiLibAllocatePool(gRpBlkIo->Media->BlockSize * 3); 
    if (MbrBuffer == NULL){
        return  FALSE;
    }
    PbrBuffer = MbrBuffer + gRpBlkIo->Media->BlockSize;
    ExtPbrTableBuffer = PbrBuffer + gRpBlkIo->Media->BlockSize;
    
    // Read MBR
    Status = gRpBlkIo->ReadBlocks(
                 gRpBlkIo,
                 gRpBlkIo->Media->MediaId,
                 0,                          // LBA#0 = MBR
                 gRpBlkIo->Media->BlockSize,
                 (VOID *)MbrBuffer  );
    
    if(EFI_ERROR(Status)){
        MemFreePointer((VOID **)&MbrBuffer);
    }
    
    // check 4 partition entries
    for (RpEntryOfs = 0x1be; RpEntryOfs < 0x1fe; RpEntryOfs += 0x10)
    {
        if(!CheckValidPartHeader(MbrBuffer+RpEntryOfs)) continue;

        // Is it the extended partition?
        if (((HD_PAR_STRUC*)(MbrBuffer + RpEntryOfs))->bType == 0x5 ||
                ((HD_PAR_STRUC*)(MbrBuffer + RpEntryOfs))->bType == 0xf){
            DEBUG((DEBUG_INFO, "[ResPar] Extended Partition found(0x%X) by CheckMbrRestorePartitionForEfiBoot()\n", RpEntryOfs));
            ExtPbrTableBase = ((HD_PAR_STRUC*)(MbrBuffer + RpEntryOfs))->dNumSecMbrToFirst;
            ExtPbrTableOffset = 0;
            while(1){
                // read Extended Partition table
                DEBUG((DEBUG_INFO, "[ResPar] Read Extended Partition Table at 0x%X\n", ExtPbrTableBase + ExtPbrTableOffset));
                Status = gRpBlkIo->ReadBlocks(
                                gRpBlkIo,
                                gRpBlkIo->Media->MediaId,
                                (UINTN)(ExtPbrTableBase + ExtPbrTableOffset),          // start LBA of each partition
                                gRpBlkIo->Media->BlockSize,
                                (VOID *)ExtPbrTableBuffer
                );
                if(EFI_ERROR(Status)) break;

                DEBUG((DEBUG_INFO, "[ResPar] (Offset 0x1be)partition type: 0x%x, Lba of partition starting: 0x%x, size of partition: 0x%x\n",
                                                                ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1be))->bType,
                                                                ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1be))->dNumSecMbrToFirst,
                                                                ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1be))->dNumSec));
                DEBUG((DEBUG_INFO, "[ResPar] (Offset 0x1ce)partition type: 0x%x, Lba of partition starting: 0x%x, size of partition: 0x%x\n",
                                                                ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1ce))->bType,
                                                                ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1ce))->dNumSecMbrToFirst,
                                                                ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1ce))->dNumSec));
                
                // check first partition header(0x1be)
                if (!CheckValidPartHeader(ExtPbrTableBuffer+0x1be) || !CheckValidMbr(ExtPbrTableBuffer)) break;

                // check the Pbr of first partition
                PbrLba = ExtPbrTableBase + ExtPbrTableOffset + ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1be))->dNumSecMbrToFirst;
                
                // read Partition Boot Record (PBR)
                Status = gRpBlkIo->ReadBlocks(
                                gRpBlkIo,
                                gRpBlkIo->Media->MediaId,
                                (UINTN)PbrLba,          // PBR: LBA start number
                                gRpBlkIo->Media->BlockSize,
                                (VOID *)PbrBuffer
                );
                if (EFI_ERROR(Status))
                    break;
                
                // search restore partition.
                RpFound = CheckRestorePbr(gRpBlkIo, PbrBuffer, PbrLba);
                if (RpFound){
                    PartStart = (UINT32)ExtPbrTableBase + (UINT32)ExtPbrTableOffset + ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1be))->dNumSecMbrToFirst;
                    PartSize = ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1be))->dNumSec;
                    break;
                }
                
                // check second partition header(0x1ce)
                if (!CheckValidPartHeader(ExtPbrTableBuffer+0x1ce) || !CheckValidMbr(ExtPbrTableBuffer)) break;
                
                // go to next extended partition table
                ExtPbrTableOffset = ((HD_PAR_STRUC*)(ExtPbrTableBuffer + 0x1ce))->dNumSecMbrToFirst;
            }
        }
        else{
            DEBUG((DEBUG_INFO, "[ResPar] PBR found(0x%X) by CheckMbrRestorePartitionForEfiBoot()\n", RpEntryOfs));
            // check the Pbr of Mbr
            PbrLba = ((HD_PAR_STRUC*)(MbrBuffer + RpEntryOfs))->dNumSecMbrToFirst;
            // read Partition Boot Record (PBR)
            Status = gRpBlkIo->ReadBlocks(
                            gRpBlkIo,
                            gRpBlkIo->Media->MediaId,
                            (UINTN)PbrLba,
                            gRpBlkIo->Media->BlockSize,
                            (VOID *)PbrBuffer
            );
            if (!EFI_ERROR(Status)){
                // search restore partition.
            	RpFound = CheckRestorePbr(gRpBlkIo, PbrBuffer, PbrLba);
                if (RpFound){
                    PartStart = ((HD_PAR_STRUC*)(MbrBuffer + RpEntryOfs))->dNumSecMbrToFirst;
                    PartSize = ((HD_PAR_STRUC*)(MbrBuffer + RpEntryOfs))->dNumSec;
                }
            }
        }
        
        // search the device path of restore partition
        if(RpFound){
            EFI_DEVICE_PATH_PROTOCOL    *FsDevicePath = NULL;
            EFI_HANDLE                  *FsHandle = NULL;

            DEBUG((DEBUG_INFO, "[ResPar] RpFound is True, PartInfo check, starting lba: 0x%X, size: 0x%X\n", PartStart, PartSize));            
            // search a FS handle matches with the ipnut(RestorePartition) by comparing device path.
            Status = pBS->LocateHandleBuffer(
                            ByProtocol,
                            &gEfiSimpleFileSystemProtocolGuid, 
                            NULL, 
                            &HandleCount, 
                            &HandleBuffer);
            for(FsHandleIdx=0;FsHandleIdx<HandleCount && !FsDevicePath && !FsHandle;FsHandleIdx++){
                EFI_DEVICE_PATH_PROTOCOL *BlkDp = NULL;
                Status = gBS->HandleProtocol (
                                HandleBuffer[FsHandleIdx], 
                                &gEfiDevicePathProtocolGuid, 
                                (VOID *) &DevicePath);
                if (EFI_ERROR(Status))
                    continue;
                Status = gBS->HandleProtocol (
                                gBlkIoHandle, 
                                &gEfiDevicePathProtocolGuid, 
                                (VOID *) &BlkDp);
                if (EFI_ERROR(Status))
                    continue;
                
                // Is it a child handle of gBlkIoHandle.
                if(MemCmp(BlkDp, DevicePath, DPLength(BlkDp) - sizeof(EFI_DEVICE_PATH_PROTOCOL))) 
                    continue;
                
                DevicePathNode = DevicePath;
                while (!isEndNode (DevicePathNode) && !FsDevicePath)
                {
                    if ( DevicePathNode->Type == MEDIA_DEVICE_PATH &&
                         DevicePathNode->SubType == MEDIA_HARDDRIVE_DP ){
                        HARDDRIVE_DEVICE_PATH   *HdDp = (HARDDRIVE_DEVICE_PATH *) DevicePathNode;
                        // compare the starting LBA of partition and the size of partition.
                        if( HdDp->PartitionStart == PartStart &&
                            HdDp->PartitionSize == PartSize ){
                            FsDevicePath = DevicePath;
                            FsHandle = HandleBuffer[FsHandleIdx];
                            break;
                        }
                    }
                    DevicePathNode = NEXT_NODE (DevicePathNode);
                }
            }
            MemFreePointer((VOID **)&HandleBuffer);
            
            // create efi file boot device path.
            if(FsDevicePath){
                EFI_DEVICE_PATH_PROTOCOL    *BootFileDp = NULL;
                RESPAR_HARDDRIVE_DEVICE_PATH    FileDp = {  
                                                            {
                                                                MEDIA_DEVICE_PATH,      // type
                                                                MEDIA_FILEPATH_DP,      // subtype
                                                                {0,0}
                                                            },                    // length
                                                            BOOTFILEPATH            // FilePath
                                                         };
#if EFI_BOOTFILEPATH_CHECK
                gMbrRpFound = CheckFilePathByHandle(FsHandle, BOOTFILEPATH);
                if (!gMbrRpFound){
                    // boot file not found.
                    DEBUG((DEBUG_INFO, "[ResPar] Boot file(%S) not found\n",BOOTFILEPATH));
                    continue;
                }
#else
                gMbrRpFound = TRUE;
#endif
                
                SET_NODE_LENGTH((EFI_DEVICE_PATH_PROTOCOL*)&FileDp, sizeof(EFI_DEVICE_PATH_PROTOCOL) + sizeof(BOOTFILEPATH));
                BootFileDp = AppendDevicePathNode(FsDevicePath, (EFI_DEVICE_PATH_PROTOCOL*)&FileDp);
                if(!BootFileDp){
                    gMbrRpFound = FALSE;
                    continue;
                }
                gMbrEfiBootDp = (UINT8*)BootFileDp;
                
                break;
            } //if(FsDevicePath){
        } //if (RpFound)
    } //for (RpEntryOfs = 0x1be; RpEntryOfs < 0x1fe; RpEntryOfs += 0x10)
    
    MemFreePointer((VOID **)&MbrBuffer);

    return (gMbrRpFound && gMbrEfiBootDp);
}
#endif

/**
    This function checks the volume label name of partition to see
    if matches RESTORE_PARTITION_VOLUME_NAME.

    @param gBlkIo 
    @param PbrBuffer Partition Boot Record

    @retval 0 It is NOT restore partition or ERROR return;
    @retval 1 non-NTFS restore partition found
    @retval 2 NTFS restore partition found

**/

UINT8
CheckRestorePbr(
    IN EFI_BLOCK_IO_PROTOCOL    *blkIo,
    IN UINT8 *PbrBuffer,
    IN UINT64 PbrLba
)
{
    BOOLEAN found = FALSE;
    UINT16 bps;

    bps = *((UINT16*)(PbrBuffer + 0x0b));
    DEBUG((DEBUG_INFO, "[ResPar] [0x0] JMP = %X\n", PbrBuffer[0x0]));
    DEBUG((DEBUG_INFO, "[ResPar] [0x0b] bytes per sector = %X\n", bps));
    DEBUG((DEBUG_INFO, "[ResPar] [0x15] Media = %X\n", PbrBuffer[0x15]));


    if (PbrBuffer[0x00] != 0xeb &&      // JMP op code
            PbrBuffer[0x00] != 0xe9)        // JMP op code
        return FALSE;

    if (bps != 0x200 ||                 // bytes per sector
            PbrBuffer[0x15] < 0xf0)         // media descriptor
        return FALSE;


    DEBUG((DEBUG_INFO, "[ResPar] File system = %s\n", PbrBuffer + 0x3));

    if (!EfiCompareMem(PbrBuffer + 0x3, "NTFS", 4))       // NTFS?
    {

#if RESTORE_PARTITION_NTFS_SUPPORT
        UINT64  lba;
        UINT8   spf;
        EFI_STATUS Status;
        UINT8 *VolBuffer;
        UINT16 FileRecordSize, i;

        lba = *((UINT32*)(PbrBuffer + 0x30)) * (UINT32)PbrBuffer[0xd];    // Logical Cluster Number for the file $MFT * sectors per cluster
        lba += (UINT64)PbrLba;

        DEBUG((DEBUG_INFO, "[ResPar] $MFT LBA=%X\n", lba));
        DEBUG((DEBUG_INFO, "[ResPar] PbrBuffer[0x40] = %X\n", PbrBuffer[0x40]));

        // $Volume is at the 3rd file record
        if (PbrBuffer[0x40] >= 0x80)    // Negative number? Clusters Per File Record Segment
        {
            // negative number, it is power of 2
            spf = (UINT8)((1 << (0x100 - PbrBuffer[0x40])) / bps);   // bytes per file record / bytes per sector = sectors per file record
        }
        else
        {
            spf = (UINT8)*((UINT32*)(PbrBuffer + 0x40)) * (UINT32)PbrBuffer[0xd];    // Clusters Per File Record Segment * sectors per cluster = sectors per file record
        }

        lba += (UINT64)(3 * spf);

        FileRecordSize = (UINT16)spf * bps;
        DEBUG((DEBUG_INFO, "[ResPar] FileRecordSize=%X\n", FileRecordSize));
        DEBUG((DEBUG_INFO, "[ResPar] $Volume LBA=%X\n", lba));

        VolBuffer = EfiLibAllocatePool(FileRecordSize);
        if (VolBuffer == NULL)
        {
            DEBUG((DEBUG_ERROR, "[ResPar] Allocate VolBuffer Failed!!!\n"));
            return 0;
        }
        // Read $Volume = $MFT + the 3rd file record
        Status = blkIo->ReadBlocks(
                     blkIo,
                     blkIo->Media->MediaId,
                     lba,
                     FileRecordSize,
                     (VOID *)VolBuffer
                 );
        if(!EFI_ERROR(Status)){
            for (i = 0 ; i < FileRecordSize && !found ; i++)
            {
                if (!EfiCompareMem(
                            VolBuffer + i,
                            UNICODE_RESTORE_PARTITION_VOLUME_NAME,
                            UNICODE_VOL_LEN))
                {
                    DEBUG((DEBUG_INFO, "[ResPar] '%S' NTFS Recovery partition found!!!!\n", (CHAR16*)(VolBuffer + i)));
                    found = TRUE;
                }
            }
        }

        MemFreePointer((VOID **)&VolBuffer);

#endif  //#if RESTORE_PARTITION_NTFS_SUPPORT

    }
    else    // Not NTFS
    {
#if RESTORE_PARTITION_FAT_SUPPORT
        UINT64  lba;
        EFI_STATUS Status;
        UINT8 *VolBuffer;
        UINT8   *FatBuf;
        UINT16  i;
        UINT8   Attribute;
        BOOLEAN IsEnd = FALSE;
        UINT64  SecPtr;
		UINT64	FatLba;
        UINT32  FatNum;
        UINT32  RootFatNum;
		UINT8	SecPerClu;

        DEBUG((DEBUG_INFO, "[ResPar] found Volume = %s\n", PbrBuffer + 0x47));
        DEBUG((DEBUG_INFO, "[ResPar] LEN = %X\n", VOL_LEN));

        if (!EfiCompareMem(
                    PbrBuffer + 0x47,
                    RESTORE_PARTITION_VOLUME_NAME,
                    VOL_LEN))
        {
            DEBUG((DEBUG_INFO, "[ResPar] Recovery partition found!!!!\n"));
            found = TRUE;
        }

        FatLba = *((UINT16 *)(PbrBuffer + 0x0e)) + PbrLba;      // FATs follows reserve secotrs.
		SecPerClu = PbrBuffer[0xd];
        RootFatNum = *((UINT32 *)(PbrBuffer + 0x2c));           // Frist Fat entire in root directory.
        FatNum = RootFatNum;
        lba = (UINT64)*((UINT16 *)(PbrBuffer + 0x0e)) + (UINT64)FatNum * (UINT64)(*((UINT32 *)(PbrBuffer + 0x24)));   //root directory entry (reserve secotr + ( number of FAT * sectors per FAT ))

        FatBuf = EfiLibAllocatePool(512);
        if (FatBuf == NULL)
        {
            DEBUG((DEBUG_ERROR, "[ResPar] Allocate FatBuf Failed!!!\n"));
            return 0;
        }		
        VolBuffer = EfiLibAllocatePool(512);
		if (VolBuffer == NULL)
        {
            MemFreePointer((VOID **)&FatBuf);
            DEBUG((DEBUG_ERROR, "[ResPar] Allocate VolBuffer Failed!!!\n"));
            return 0;
        }

        // Search all FDB(File Description Block) in root directory.
        // It will end if find out the FDB with Volume Name type or end of root directory.
        while(1){		
            Status = blkIo->ReadBlocks(
        	    			blkIo,
                            blkIo->Media->MediaId,
                            FatLba + (FatNum / 128),	// There are 128 FAT entires for each sector.
                            512,
                            (VOID *)FatBuf );
            if(EFI_ERROR(Status)) break;

            if(*(UINT32*)(FatBuf + ((FatNum % 128) * 4)) != 0xFFFFFF7 && *(UINT32*)(FatBuf + ((FatNum % 128) * 4)) != 0x0){	//When FAT entire value equals to 0xFFFFFF7, means borken cluster.
                                                                                                                            //When FAT entire value equals to 0x0, this cluster does not be use.
                //Each Fat entire has 1 cluster space for storage. So serach the all sectors in each cluster.
        		SecPtr = 0;
                do{
                    Status = blkIo->ReadBlocks(
                                    blkIo,
                                    blkIo->Media->MediaId,
                                    PbrLba + lba + ((FatNum - RootFatNum) * SecPerClu) + SecPtr,	// exclude system reserve(2 cluster)
                                    512,
                                    (VOID *)VolBuffer );
                    if(EFI_ERROR(Status)) break;
                    //There are 16 entire for each sector. (each entire size is 32 bytes)
                    for(i=0;i<16;++i){      
                        if( !(*(UINT8*)(VolBuffer+(i*0x20))) ){
                            //First byte = 0 means the end of Fat entire.
                            IsEnd = TRUE;
                            break;
                        }
                        Attribute = *(UINT8*)(VolBuffer+(i*0x20)+11);
                        //Attribute - bitvector. 
                        //Bit 0: read only. 
                        //Bit 1: hidden.
                        //Bit 2: system file. 
                        //Bit 3: volume label. 
                        //Bit 4: subdirectory.
                        //Bit 5: archive. 
                        //Bits 6-7: unused.
                        //Notice: 0x0F represents long file name type
                        if(( Attribute & BIT3) && Attribute != 0x0F){
                            //DEBUG((DEBUG_INFO, ",[ResPar] Label name= %s ", VolBuffer+(i*0x20)));
                            //compare the RESTORE_PARTITION_VOLUME_NAME with volume label
                            if (!EfiCompareMem(
                                        VolBuffer+(i*0x20),
                                        RESTORE_PARTITION_VOLUME_NAME,
                                        VOL_LEN))
                            {
                                DEBUG((DEBUG_INFO, "[ResPar] Recovery partition found!!!!\n"));
                                found = TRUE;
                                break;
                           }
                        }
                    }
                    SecPtr++;  //next sector
                }while(!found && !IsEnd && SecPtr < SecPerClu);
            }	//end of "if(*(UINT32*)(FatBuf + ((FatNum % 128) * 4)) != 0xFFFFFF7 && *(UINT32*)(FatBuf + ((FatNum % 128) * 4)) != 0x0)"
            
            if(found || *(UINT32*)(FatBuf + ((FatNum % 128) * 4)) >= 0xFFFFFF7){
                // = 0xFFFFFF7: cluster is borken
                // > 0xFFFFF8: end of FAT entrie's cluster(or end of root directory)
                break;
            }

        	if(*(UINT32*)(FatBuf + ((FatNum % 128) * 4)) >= 0x2 && *(UINT32*)(FatBuf + ((FatNum % 128) * 4)) <= 0xFFFFFF6){
            	// Move to next Fat entire no.
        		FatNum = *(UINT32*)(FatBuf + ((FatNum % 128) * 4));
            }
            else{
                //Fat entire 0 and 1 are system reserver.
                break;
            }
        }

        MemFreePointer((VOID **)&FatBuf);
        MemFreePointer((VOID **)&VolBuffer);
#endif  //#if RESTORE_PARTITION_FAT_SUPPORT
    }

    return found;

}



/**
    This function checks if this is a valid partition.

    @param bootFlowPtr Boot flow entry that triggered this call

    @retval TRUE Valid MBR with partition signature present
    @retval FALSE no partition signature

**/
BOOLEAN
CheckValidMbr(IN UINT8 *MbrBuffer)
{
    return (*((UINT16*)(MbrBuffer + 0x1fe)) == 0xaa55); // signature
}

/**
    Check if have a specific restore partition in the storage
    with GPT partition format.

    @param VOID

    @retval VOID

    @note  gGptRpFound and gGptHdDP modified

**/
VOID
CheckGptRestorePartition(VOID)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       *GptHeaderBuffer = NULL;
    UINT8       *GptEntryBuffer = NULL;

    UINT64      PartitionEntryLBA = 0;
    UINT64      GptPbrLba;
    UINT32      NumGptEntry = 0, EfiSystemPartNum = 0;
    UINT8       *PbrBuffer = NULL;
    UINT32      j = 0;

    PARTITION_ENTRY     *PartEntry = NULL, EfiPartEntry;

    DEBUG((-1, "[ResPar] DW08 - CheckGptRestorePartition Start\n"));
    PbrBuffer = EfiLibAllocatePool(gRpBlkIo->Media->BlockSize);
    if (PbrBuffer == NULL)
    {
        DEBUG((DEBUG_ERROR, "[ResPar] Allocate PbrBuffer Failed!!!\n"));
        return;
    }

    GptHeaderBuffer = EfiLibAllocatePool(gRpBlkIo->Media->BlockSize);
    if (GptHeaderBuffer == NULL)
    {
        DEBUG((DEBUG_ERROR, "[ResPar] Allocate GptHeaderBuffer Failed!!!\n"));
        MemFreePointer((VOID **)&PbrBuffer);
        return;
    }
    GptEntryBuffer = EfiLibAllocatePool(gRpBlkIo->Media->BlockSize);
    if (GptEntryBuffer == NULL)
    {
        DEBUG((DEBUG_ERROR, "[ResPar] Allocate GptEntryBuffer Failed!!!\n"));
        MemFreePointer((VOID **)&PbrBuffer);
        MemFreePointer((VOID **)&GptHeaderBuffer);
        return;
    }

    //Read GPT Header Table
    gRpBlkIo->ReadBlocks(
        gRpBlkIo,
        gRpBlkIo->Media->MediaId,
        1,                          // LBA#1 = GPT Header Table
        gRpBlkIo->Media->BlockSize,
        (VOID *)GptHeaderBuffer );

    //To see if it is EFI-compatible partition table header
    if (EfiCompareMem(
                GptHeaderBuffer,
                "EFI PART",
                8))
    {
        DEBUG((DEBUG_INFO, "[ResPar] It is NOT EFI-compatible partition table header\n"));

        MemFreePointer((VOID **)&GptHeaderBuffer);
        MemFreePointer((VOID **)&GptEntryBuffer);
        MemFreePointer((VOID **)&PbrBuffer);
        return;
    }
    DEBUG((DEBUG_INFO, "[ResPar] It is EFI-compatible partition table header\n"));

    //The starting LBA of the GUID Partition Entry array.
    PartitionEntryLBA = *((UINT64 *)(GptHeaderBuffer + 0x48));

    //The number of Partition Entries in the GUID Partition Entry array.
    NumGptEntry = *((UINT32*)(GptHeaderBuffer + 0x50));
    DEBUG((-1, "[ResPar] DW08 - NumGptEntry = 0x%X\n", NumGptEntry));

    //Check all partition entries unless it finds recovery partition with proper volume label
    for (j = 0; j < NumGptEntry ; j++)
    {
        //There are four entries in one LBA. Read the next LBA if all 4 entries have been read
        if ( j%4 == 0)
        {
            gRpBlkIo->ReadBlocks(
                gRpBlkIo,
                gRpBlkIo->Media->MediaId,
                (PartitionEntryLBA+j/4),
                gRpBlkIo->Media->BlockSize,
                (VOID *)GptEntryBuffer  );
        }

        PartEntry = (PARTITION_ENTRY*)(GptEntryBuffer+((j%4)*0x80));

        //Use GUID to decide if the partition which restore tool's efi boot loader resides in exists or not
        if (!EfiCompareMem(&(PartEntry->PartitionTypeGuid),&OemRestorePartitionGuid,sizeof(EFI_GUID)))
        {
            EfiPartEntry = *PartEntry; //Partition which restore tool's efi boot loader resides in
            //Partition Number of partition which restore tool's efi boot loader resides in
            EfiSystemPartNum = j+1;

            DEBUG((DEBUG_INFO, "\n[ResPar]Partition which restore tool's efi boot loader resides in found!!!\n"));
            DEBUG((DEBUG_INFO, "[ResPar]EfiSystemPartNum = %d\n", EfiSystemPartNum));
            DEBUG((DEBUG_INFO, "[ResPar]PartitionStart = %x\n", EfiPartEntry.StartingLba));
        }
        else
            continue;

        //Starting LBA of the partition defined by this entry.
        GptPbrLba = PartEntry->StartingLba;

        //Read Partition Boot Record (PBR)
        Status = gRpBlkIo->ReadBlocks(
                     gRpBlkIo,
                     gRpBlkIo->Media->MediaId,
                     (UINTN)GptPbrLba,          // PBR: Partition LBA start number
                     gRpBlkIo->Media->BlockSize,
                     (VOID *)PbrBuffer  );

        if (EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "[ResPar] Read Pbr failed!!!\n"));
            continue;
        }

        //To check if there is a partition with proper label name
#if GPT_BOOT_LOADER_PARTITON_LABEL_CHECK
        if (CheckRestorePbr(
                    gRpBlkIo,
                    PbrBuffer,
                    GptPbrLba   ))
#endif
        {
            gGptRpFound = TRUE;

            // set up the device path for the EFI system partition
            gGptHdDP.Header.Type = MEDIA_DEVICE_PATH;
            gGptHdDP.Header.SubType = MEDIA_HARDDRIVE_DP;
            SET_NODE_LENGTH(&(gGptHdDP.Header), sizeof(HARDDRIVE_DEVICE_PATH));

            //gGptHdDP.PartitionNumber = (j/128) + 1;
            gGptHdDP.PartitionNumber = EfiSystemPartNum;//Partition Number of EFI system partition
            gGptHdDP.PartitionStart = EfiPartEntry.StartingLba;
            gGptHdDP.PartitionSize = EfiPartEntry.EndingLba - EfiPartEntry.StartingLba + 1;
            gBS->CopyMem(&(gGptHdDP.Signature), &(EfiPartEntry.UniquePartitionGuid), \
                         sizeof(EFI_GUID));
            gGptHdDP.MBRType = MBR_TYPE_EFI_PARTITION_TABLE_HEADER;
            gGptHdDP.SignatureType = SIGNATURE_TYPE_GUID;

            //File Path
            gGptHdDP.FileType = MEDIA_DEVICE_PATH;
            gGptHdDP.FileSubType = MEDIA_FILEPATH_DP;
            gGptHdDP.FileLength = sizeof(BOOTFILEPATH) + 4;

            //End of device path
            gGptHdDP.EndType = END_DEVICE_PATH;
            gGptHdDP.EndSubType = END_ENTIRE_SUBTYPE;
            gGptHdDP.EndLength = 0x04;

#if EFI_BOOTFILEPATH_CHECK
            {            
                EFI_DEVICE_PATH_PROTOCOL    *Dp = NULL;
                EFI_DEVICE_PATH_PROTOCOL    *BlkDp = NULL;
                Status = pBS->HandleProtocol( gBlkIoHandle, &gEfiDevicePathProtocolGuid, (VOID**)&BlkDp );
                if(EFI_ERROR(Status))
                    continue ;
                Dp = AppendDevicePathNode(BlkDp, &(gGptHdDP.Header));
                if (Dp != NULL){
                    gGptRpFound = CheckFilePathByDevicePath(Dp,BOOTFILEPATH);
                    MemFreePointer((VOID **)&Dp);
                }
                if(!gGptRpFound)
                    continue;
            }
#endif  
            
            gGlobalGptHdDP = gGptHdDP;
            
            DEBUG((DEBUG_INFO, "\n[ResPar]PartitionNumber = %d\n", gGptHdDP.PartitionNumber));
            DEBUG((DEBUG_INFO, "[ResPar]PartitionStart = %x\n", gGptHdDP.PartitionStart));
            DEBUG((DEBUG_INFO, "[ResPar]PartitionStart = %x\n", EfiPartEntry.StartingLba));
            break;
        }//end of if CheckRestorePbr

    }//end of for (j = 0; j < NumGptEntry ; j++)
    MemFreePointer((VOID **)&GptHeaderBuffer);
    MemFreePointer((VOID **)&GptEntryBuffer);
    MemFreePointer((VOID **)&PbrBuffer);
}

/**
    For no CSM support, use BlockIO to search if uEFI restore
    partition exists.

    @param VOID

    @retval VOID

**/
VOID
BlockIOSearchRestorePartition(VOID)
{
    EFI_STATUS  Status;
    UINTN       HandleCount = 0;
    EFI_HANDLE  *HandleBuffer = 0;
    UINTN       i = 0;
    UINT8       *MbrBuffer = NULL;



    Status = gBS->LocateHandleBuffer (  ByProtocol, \
                                        &gEfiBlockIoProtocolGuid, \
                                        NULL, \
                                        &HandleCount, \
                                        &HandleBuffer   );

    if (EFI_ERROR(Status))
    {
        DEBUG((DEBUG_ERROR, "[ResPar] Can't locate BlockIO!!!\n"));
        return;
    }

    for (i=0; i<HandleCount; i++ )
    {

        //if (EFI_ERROR(Status))
        //{
        //    DEBUG((DEBUG_ERROR, "[ResPar] Find device path protocol failed!!!\n"));
        //    continue;
        //}

        Status = gBS->HandleProtocol(   HandleBuffer[i],
                                        &gEfiBlockIoProtocolGuid,
                                        (VOID**)&gRpBlkIo );
        if (EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "[ResPar] Find BlockIo protocol failed!!!\n"));
            continue;
        }

        gBlkIoHandle = HandleBuffer[i];

        if(!OemCheckRestorePartition()){
            continue;
        }
       
        //Only need a BlockIo which is presented for whole HDD
        if (    gRpBlkIo->Media->MediaPresent == 1 &&
                gRpBlkIo->Media->LogicalPartition == 0 &&
                gRpBlkIo->Media->ReadOnly == 0  )
        {

            MbrBuffer = EfiLibAllocatePool(gRpBlkIo->Media->BlockSize);
            if (MbrBuffer == NULL)
            {
                DEBUG((DEBUG_ERROR, "[ResPar] Allocate MbrBuffer Failed!!!\n"));
                continue;
            }

            // Read MBR
            Status = gRpBlkIo->ReadBlocks(
                         gRpBlkIo,
                         gRpBlkIo->Media->MediaId,
                         0,                          // LBA#0 = MBR
                         gRpBlkIo->Media->BlockSize,
                         (VOID *)MbrBuffer  );

            //Check if it is a valid MBR
            if (!CheckValidMbr(MbrBuffer) || EFI_ERROR(Status))
            {
                DEBUG((DEBUG_INFO, "[ResPar] It is not valid MBR or read blocks failed!!!\n"));
                MemFreePointer((VOID **)&MbrBuffer);
                continue;
            }

            DEBUG((DEBUG_INFO, "[ResPar] found MBR signature = %X\n", *((UINT16*)(MbrBuffer + 0x1fe))));

            //It is GPT partition so it has PMBR
            if (((HD_PAR_STRUC*)(MbrBuffer + 0x1be))->bType == 0XEE)
            {
                DEBUG((DEBUG_INFO, "[ResPar] It is a PMBR!!!!\n"));
                CheckGptRestorePartition();

                if (gGptRpFound == TRUE)
                {
                    DEBUG((DEBUG_INFO, "[ResPar] Found GPT restore partition!!!\n"));
                    MemFreePointer((VOID **)&MbrBuffer);
                    gBS->FreePool( HandleBuffer );
                    return;
                }
            }
#if RESPAR_EFI_BOOT_FROM_MBR
            else{   // this is MBR
                CheckMbrRestorePartitionForEfiBoot();
                
                if (gMbrRpFound  == TRUE)
                {
                    MemFreePointer((VOID **)&MbrBuffer);
                    gBS->FreePool( HandleBuffer );
                    return;
                }
            }
#endif

            MemFreePointer((VOID **)&MbrBuffer);
        }//end of if(gRpBlkIo->Media->MediaPresent == 1)
        else continue;

    }//end of for(i=0; i<HandleCount; i++ )

    gBS->FreePool( HandleBuffer );

    return;
}


/**
    This function search the restore partition at boot time.

    @param VOID

    @retval VOID

    @note  gRpBbsTable ,gRpBbsIndex, gRpBbsCount, gRpFound, 
				gRpEntryOfs and gRpBlkIo modified

**/

VOID
SearchRestorePartition(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;

    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios;
    UINT16      HddCount;
    HDD_INFO    *HddInfo;
    UINT16      i;

    UINT8       *MbrBuffer = NULL, *PbrBuffer = NULL;
    UINT32      PbrLba;

    EFI_HANDLE  Handle;

    EFI_HANDLE                      *HandleBuffer;
    UINTN                           HandleCount = 0;
    EFI_DEVICE_PATH_PROTOCOL        *DevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL        *DevicePathNode = NULL;
    PCI_DEVICE_PATH                 *PciDevicePath = NULL;

    BOOLEAN     FirstTime = TRUE;
    UINT32      RaidBlockIO = 0;

    gGptRpFound = FALSE;

    gRpBlkIo = NULL;
    gRpFound = FALSE;

    // Locate CSM
    if (EFI_ERROR(Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios)))
    {
        DEBUG((DEBUG_INFO, "[ResPar] ERROR=%X: Locate CSM\n", Status));
        //Since there is no CSM support, try to use BlockIO to search if uEFI restore partition exists.
        DEBUG((DEBUG_INFO, "[ResPar] No CSM support!!! Try to search partition by BlockIO only!!!\n"));
        BlockIOSearchRestorePartition();
        return;
    }

    // Get BBS_TABLE
    if (EFI_ERROR(Status = LegacyBios->GetBbsInfo(
                               LegacyBios,
                               &HddCount,
                               &HddInfo,
                               &gRpBbsCount,
                               &gRpBbsTable)))
    {
        DEBUG((DEBUG_ERROR, "[ResPar] ERROR=%X: Get BBS table\n", Status));
        return; //error
    }


    // Search for specific device
    for (i = 0; i < gRpBbsCount && !gRpFound; i++)
    {
        if (gRpBbsTable[i].DeviceType == BBS_HARDDISK)      // Hard disk?
        {
            //Handle = (EFI_HANDLE)((gRpBbsTable[i].IBV2 << 32)+ gRpBbsTable[i].IBV1);
            //There is no need to use shift to get handle
            Handle = *(VOID**)(&gRpBbsTable[i].IBV1);

            //If device is using OpRom, IBV will be 0
            //Using information of BBsTable to find the proper handle of device
            if ( gRpBbsTable[i].IBV1 == 0 )
            {
                //Find all handle using block IO protocol
                //Want to make sure we can test all of blockIo handler; not start over again whenever test
                //next BBS device.
                Status = gBS->LocateHandleBuffer (  ByProtocol, \
                                                    &gEfiBlockIoProtocolGuid, \
                                                    NULL, \
                                                    &HandleCount, \
                                                    &HandleBuffer   );
                if (EFI_ERROR(Status))
                {
                    DEBUG((DEBUG_INFO,"[ResPar] Locate Handle Buffer Failed!!!\n"));
                    continue;
                }

                if (FirstTime)
                {
                    FirstTime = FALSE;
                    RaidBlockIO = 0;
                }

                for (; RaidBlockIO <  HandleCount; RaidBlockIO++ )
                {
                    //Find handle has Device Path Protocol and point to interface
                    Status = gBS->HandleProtocol (  HandleBuffer[RaidBlockIO], \
                                                    &gEfiDevicePathProtocolGuid, \
                                                    (VOID *) &DevicePath    );

                    if (EFI_ERROR(Status)) continue;

                    DEBUG((DEBUG_INFO,"[ResPar] RaidBlockIO=%d\n",RaidBlockIO));

                    DevicePathNode = DevicePath;

                    //Find PCI device path
                    PciDevicePath = NULL;
                    while (!isEndNode (DevicePathNode))
                    {
                        if ((DevicePathNode->Type == HARDWARE_DEVICE_PATH) && \
                                (DevicePathNode->SubType == HW_PCI_DP))
                        {
                            PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;

                            break;
                        }
                        DevicePathNode = NEXT_NODE (DevicePathNode);
                    }
                    
                    //skip this device handle check when no PCI device path node found
                    if ( PciDevicePath == NULL )
                    {
                        continue;
                    }
                    
                    //Compare pci dvice path
                    if ( ( PciDevicePath->Device == gRpBbsTable[i].Device ) && \
                            ( PciDevicePath->Function == gRpBbsTable[i].Function ) )
                    {
                        DEBUG((DEBUG_INFO,"[ResPar] Find Handle!!!\n"));
                        Handle = HandleBuffer[RaidBlockIO];
                        DEBUG((DEBUG_INFO,"[ResPar] Handle = %X\n", Handle));

                        gBS->HandleProtocol(
                            Handle,
                            &gEfiBlockIoProtocolGuid,
                            (VOID**)&gRpBlkIo);
                        DEBUG((DEBUG_INFO,"[ResPar] MediaId = %X\n", gRpBlkIo->Media->MediaId));
                        DEBUG((DEBUG_INFO,"[ResPar] RemovableMedia = %X\n", gRpBlkIo->Media->RemovableMedia));
                        DEBUG((DEBUG_INFO,"[ResPar] MediaPresent = %X\n", gRpBlkIo->Media->MediaPresent));
                        DEBUG((DEBUG_INFO,"[ResPar] LogicalPartition = %X\n", gRpBlkIo->Media->LogicalPartition));
                        DEBUG((DEBUG_INFO,"[ResPar] ReadOnly = %X\n", gRpBlkIo->Media->ReadOnly));
                        DEBUG((DEBUG_INFO,"[ResPar] BlockSize = %X\n", gRpBlkIo->Media->BlockSize));
                        DEBUG((DEBUG_INFO,"[ResPar] LastBlock = %X\n", gRpBlkIo->Media->LastBlock));

                        //Check Media info to see if this device is the right one we want to seek.
                        //There should be media presented, not read only and since we are seeking the
                        //whole disk not just one particular partititon, logical partition should be 0.
                        //This can avoid we set CD-ROM's handle as the HDD's handle.
                        if (gRpBlkIo->Media->MediaPresent == 1 &&
                                gRpBlkIo->Media->LogicalPartition == 0 &&
                                gRpBlkIo->Media->ReadOnly == 0)
                        {
                            RaidBlockIO++;
                            break;
                        }
                        else
                            continue;
                    }
                }
                gBS->FreePool( HandleBuffer );
            }
            else
                Handle = (EFI_HANDLE)(UINTN)(gRpBbsTable[i].IBV1);

            DEBUG((DEBUG_INFO, "[ResPar] Handle=%X\n", Handle));

            if (!EFI_ERROR(Status = gBS->HandleProtocol(
                                        Handle,
                                        &gEfiBlockIoProtocolGuid,
                                        (VOID**)&gRpBlkIo)
                          ))
            {
                gBlkIoHandle = Handle;

                if(!OemCheckRestorePartition()){
                    continue;
                }

                MbrBuffer = EfiLibAllocatePool(gRpBlkIo->Media->BlockSize);
                if (MbrBuffer == NULL)
                    break;

                // Read MBR
                Status = gRpBlkIo->ReadBlocks(
                             gRpBlkIo,
                             gRpBlkIo->Media->MediaId,
                             0,                          // LBA#0 = MBR
                             gRpBlkIo->Media->BlockSize,
                             (VOID *)MbrBuffer
                         );

                if (!CheckValidMbr(MbrBuffer) || EFI_ERROR(Status)) {
                    MemFreePointer((VOID **)&MbrBuffer);
                    continue;
                }

                DEBUG((DEBUG_INFO, "[ResPar] found MBR signature = %X\n", *((UINT16*)(MbrBuffer + 0x1fe))));

                //It is GPT partition so it has PMBR
                if (((HD_PAR_STRUC*)(MbrBuffer + 0x1be))->bType == 0XEE)
                {
                    DEBUG((DEBUG_INFO, "***** It is PMBR\n"));

                    CheckGptRestorePartition();

                    MemFreePointer((VOID **)&MbrBuffer);

                    if (gGptRpFound == TRUE)
                    {
                        DEBUG((DEBUG_INFO, "[ResPar] Found GPT restore partition!!!\n"));
                        break;
                    }

                    continue;
                }//end of if it is PMBR
#if RESPAR_EFI_BOOT_FROM_MBR
                else{   // this is MBR
                    CheckMbrRestorePartitionForEfiBoot();
                
                    if (gMbrRpFound  == TRUE)
                    {
                        DEBUG((DEBUG_INFO, "[ResPar] Found boot file in restore partition on Mbr partition!!!\n"));
                        MemFreePointer((VOID **)&MbrBuffer);
                        break;
                    }
                }
#endif

                PbrBuffer = EfiLibAllocatePool(gRpBlkIo->Media->BlockSize);
                if (PbrBuffer == NULL)
                    break;

                // check 4 partition entries
                for (gRpEntryOfs = 0x1be; gRpEntryOfs < 0x1fe; gRpEntryOfs += 0x10)
                {

                    PbrLba = ((HD_PAR_STRUC*)(MbrBuffer + gRpEntryOfs))->dNumSecMbrToFirst;
                    DEBUG((DEBUG_INFO, "[ResPar] PBR (%X) LBA# = %X\n", gRpEntryOfs, PbrLba));

                    // read Partition Boot Record (PBR)
                    Status = gRpBlkIo->ReadBlocks(
                                 gRpBlkIo,
                                 gRpBlkIo->Media->MediaId,
                                 (UINTN)PbrLba,          // PBR: LBA start number
                                 gRpBlkIo->Media->BlockSize,
                                 (VOID *)PbrBuffer
                             );
                    if (EFI_ERROR(Status))
                        continue;

                    // PbrBuffer = 512 bytes of PBR

                    gRpBbsIndex = i;
                    if ((gRpFound = CheckRestorePbr(
                                       gRpBlkIo,
                                       PbrBuffer,
                                       PbrLba)) != 0)
                        break;
                }

                MemFreePointer((VOID **)&MbrBuffer);
                MemFreePointer((VOID **)&PbrBuffer);
            }
        }
    }
}

/**
    This function hooks CheckForKeyHook.
    @param bootFlowPtr

    @retval VOID

**/

VOID
RestorePartitionCallBack(IN BOOT_FLOW *bootFlowPtr)
{
    DEBUG((DEBUG_INFO, "[ResPar] F9 PRESSED.....\n"));
    
    SearchRestorePartition();
    BootRestorePartition();
}

/**
    This function set restore partition to the first boot priority.

    @param VOID

    @retval VOID

**/

VOID
BootRestorePartition()
{
    EFI_STATUS Status = EFI_SUCCESS;

    UINT16      j;

    UINT8 DevicePathRaw[] =             // BBS_BBS_DEVICE_PATH raw data initialization without type
    {
        BBS_DEVICE_PATH,                // BBS_BBS_DEVICE_PATH.EFI_DEVICE_PATH_PROTOCOL.Type
        BBS_BBS_DP,                     // BBS_BBS_DEVICE_PATH.EFI_DEVICE_PATH_PROTOCOL.SubType
        sizeof(BBS_BBS_DEVICE_PATH), 0, // BBS_BBS_DEVICE_PATH.EFI_DEVICE_PATH_PROTOCOL.Length[2]
        BBS_FLOPPY, 0,                  // BBS_BBS_DEVICE_PATH.DeviceType   (to be filled later)
        0, 0,                           // BBS_BBS_DEVICE_PATH.StatusFlag
        0,                              // BBS_BBS_DEVICE_PATH.String
    };

    BBS_BBS_DEVICE_PATH *BootDevicePath = (BBS_BBS_DEVICE_PATH *)DevicePathRaw;

    UINT8       *MbrBuffer = NULL;
    UINT32      Addr16;
    UINT32      LockUnlockAddr, LockUnlockSize;
    EFI_LEGACY_BIOS_EXT_PROTOCOL    *BiosExtensions = NULL;

    UINT8       *ShadowAdr;
    UINT8       *BDA_4f6 = (UINT8*)0x4f6;

    EFI_EVENT   BootEvent;
    VOID        *pRegistration = NULL;
    EFI_GUID    AmiLegacyBootProtocolGuid = EFI_AMI_LEGACYBOOT_PROTOCOL_GUID;

    //DEBUG((DEBUG_INFO, "[ResPar] F9 PRESSED.....\n"));

	//SearchRestorePartition();
    if (gGptRpFound || gMbrRpFound)
    {
        gSpecificPartitionFind = TRUE;
        DEBUG((DEBUG_INFO, "************[ResPar] F9 PRESSED.....*************\n"));
        Status = _BootLaunchDevicePath((gGptRpFound)? (EFI_DEVICE_PATH_PROTOCOL *) & gGptHdDP : (EFI_DEVICE_PATH_PROTOCOL *) gMbrEfiBootDp, NULL, 0, TRUE);

        if (EFI_ERROR(Status))
        {
            DEBUG((DEBUG_INFO, "****** Can't find device path!!!\n"));
            return;
        }
        return;
    }
    else
    {
        gEnterSetup = TRUE;
        gSpecificPartitionFind = FALSE;
    }
    
    if (!gRpFound)
        return;

    DEBUG((DEBUG_INFO, "[ResPar] MBR+%X = restore partition\n", gRpEntryOfs));


    // NTFS (gRpFound==2) should boot from PBR without any partition change.
    // NTFS - Must be used with CSM_61

    MbrBuffer = EfiLibAllocatePool(gRpBlkIo->Media->BlockSize);
    if (MbrBuffer != NULL)
    {

        // Read MBR
        Status = gRpBlkIo->ReadBlocks(
                     gRpBlkIo,
                     gRpBlkIo->Media->MediaId,
                     0,                          // LBA#0 = MBR
                     gRpBlkIo->Media->BlockSize,
                     (VOID *)MbrBuffer
                 );

        if (CheckValidMbr(MbrBuffer) && !EFI_ERROR(Status))
        {
            // Set as first boot
            ((BBS_BBS_DEVICE_PATH *)BootDevicePath)->DeviceType = gRpBbsTable[gRpBbsIndex].DeviceType;  // set DeviceType for LegacyBoot()

            // descend other's priority
            for (j = 0; j < gRpBbsCount; j++)
            {
                switch (gRpBbsTable[j].BootPriority)
                {
                case BBS_IGNORE_ENTRY:
                case BBS_LOWEST_PRIORITY:
                    continue;
                default:
                    gRpBbsTable[j].BootPriority++;      // descend
                    break;
                }
            }

            // Set desired device as first boot
            gRpBbsTable[gRpBbsIndex].BootPriority = 0;  // set found device as highest priority

            MemFreePointer((VOID **)&MbrBuffer);

            // Locate OEM16 function and update it with gRpEntryOfs.
            Status = gBS->LocateProtocol(
                         &gEfiLegacyBiosExtProtocolGuid, NULL, (VOID**)&BiosExtensions);
            if (EFI_ERROR(Status)) return;

            // The following call makes CSM16 shadow writable
            ShadowAdr = (UINT8*)0xE0000;
            BiosExtensions->UnlockShadow(ShadowAdr, 0, &LockUnlockAddr, &LockUnlockSize);

            Status = BiosExtensions->Get16BitFuncAddress(
                         CSM16_OEM_BEFORE_CALL_BOOT_VECTOR,
                         &Addr16);
            if (EFI_ERROR(Status)) return;
            // Addr16 points to RestorePartitionModuleStart routine implemented in
            // RESPAR.ASM

            if (!EFI_ERROR(Status))
            {
                UINT8 RpSig[] = {"$RPV"};

                while (Addr16 < 0xf0000)
                {
                    if (!MemCmp((VOID*)(UINTN)Addr16, (VOID*)RpSig, 4))  // "$RPV" found?
                        break;                              // yes.. break
                    Addr16++;
                }
                if (Addr16 < 0xf0000)   // "$RPV" found
                {
                    *(UINT16*)((UINTN)(Addr16 + 4)) = gRpEntryOfs;  // set ResPar.asm:RpEntryOfs = gRpEntryOfs
                }
                else
                    DEBUG((DEBUG_INFO, "[ResPar] Can not find $RPV\n"));//4 //debug

                gAddr16 = Addr16;	// save $PRV address
                if(Addr16 < 0xf0000){
                    Status = pBS->CreateEvent(
                                    EVT_NOTIFY_SIGNAL,
                                    TPL_NOTIFY, 
                                    SaveINT13BeforeINT19, 
                                    NULL, 
                                    &BootEvent
                                    );
                    if (!EFI_ERROR(Status)){
                        Status = pBS->RegisterProtocolNotify(
                                        &AmiLegacyBootProtocolGuid, 
                                        BootEvent, 
                                        &pRegistration
                                        );
                    }
                }

                if (EFI_ERROR(Status))
                    DEBUG((DEBUG_ERROR, "[ResPar] Register AmiLegacyBoot callback fail!!\n"));
            }


#if RESTORE_PARTITION_SET_ACTIVE
            *BDA_4f6 &= ~ 0x8;  // bit3 = 0 = no PBR, boot from MBR if RESTORE_PARTITION_SET_ACTIVE=1
#else
            *BDA_4f6 |= 0x8;    // bit3 = 1 = PBR boot, not tried
#endif

            BiosExtensions->LockShadow(LockUnlockAddr, LockUnlockSize);

            // Boot it here for found specific device
            _BootLaunchDevicePath((EFI_DEVICE_PATH_PROTOCOL *)BootDevicePath, NULL, 0, TRUE);
        }

        MemFreePointer((VOID **)&MbrBuffer);
    }

}

/**
    This function check the pressed-Hotkey is the Restore Partition 
    key or not.

    @param UnicodeChar 
    @param ScanCode 
    @param OemKeyCallbackFn 

    @retval TRUE Hotkey matched
    @retval FLASE Hotkey unmatched

**/
BOOLEAN RestorePartitionCheckKey(  CHAR16 UnicodeChar, UINT16 ScanCode, UINT32 ShiftState, OUT VOID **OemKeyCallbackFn )
{
/**
 * @brief Original
 * 
	if(ScanCode == RESTORE_PARTITION_KEY_SCANCODE
	&& UnicodeChar == RESTORE_PARTITION_KEY_UNICODE)
	{
		*OemKeyCallbackFn = (VOID*)(UINTN)RestorePartitionCallBack;
		return TRUE; // key consumed
	}
	return FALSE;
 */
    *OemKeyCallbackFn = (VOID*)(UINTN)RestorePartitionCallBack;
    return TRUE; // key consumed
}

/**
    This function save old INT13 vector 
              

    @param Event 
    @param Context 

    @retval VOID

**/
VOID SaveINT13BeforeINT19 (
    IN EFI_EVENT Event, 
    IN VOID *Context
){
	EFI_STATUS			Status;
    UINT8*  ShadowAdr   = (UINT8*)0xE0000;
    UINT32  Addr16      = gAddr16;	// $PRV address in shadowram.
    UINT32  LockUnlockAddr;
	UINT32  LockUnlockSize;
    UINT8	RpSig[] = {"$RPV"};
    EFI_LEGACY_BIOS_EXT_PROTOCOL    *BiosExtensions = NULL;

    pBS->CloseEvent(Event);

    Status = gBS->LocateProtocol(&gEfiLegacyBiosExtProtocolGuid, NULL, (VOID**)&BiosExtensions);
    if (EFI_ERROR(Status)){
		DEBUG((DEBUG_ERROR,"[ResPar] Locate LegacyBiosExtProtocol Failed!!!\n"));
		return;
	}

    BiosExtensions->UnlockShadow(ShadowAdr, 0, &LockUnlockAddr, &LockUnlockSize);

	if (!MemCmp((VOID*)(UINTN)Addr16, (VOID*)RpSig, 4))
	    *(UINT32*)((UINTN)(Addr16 + 4 + 2)) = *(UINT32*)(0x4c);
	else 
		DEBUG((DEBUG_INFO,"[ResPar] CsmOem Function has Changed!!!\n"));
    
    BiosExtensions->LockShadow(LockUnlockAddr, LockUnlockSize);
} 

