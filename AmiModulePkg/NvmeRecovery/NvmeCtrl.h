//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeCtrl.c
    Header file for Nvme Controller/device definition

**/

#ifndef _AMI_NVME_CTRL_H
#define _AMI_NVME_CTRL_H

// Refer to NVM Spec second 3.1 NVMe 1.1 which describes the register map for the controller
#define Offset_Cap      0x0
#define Offset_Version  0x8
#define Offset_Intms    0xC
#define Offset_Intmc    0x10
#define Offset_CC       0x14
#define Offset_CSTS     0x1C
#define CSTS_RDY        0x1
#define CSTS_CFS        0x2
#define Offset_Nssrs    0x20
#define Offset_Aqa      0x24
#define Offset_Asq      0x28
#define Offset_Acq      0x30
#define Offset_Admin_Submission_Queue_Tail_DoorBell 0x1000

// Admin Cmd Set Figure 38 NVM Express 1.1
#define DELETE_IO_SUBMISSION_QUEUE  0x00
#define CREATE_IO_SUBMISSION_QUEUE  0x01
#define GET_LOG_PAGE                0x02
#define DELETE_IO_COMPLETION_QUEUE  0x04
#define CREATE_IO_COMPLETION_QUEUE  0x05
#define IDENTIFY                    0x06
#define ABORT                       0x08
#define SET_FEATURES                0x09
#define GET_FEATURES                0x0A
#define ASYNC_EVENT_REQUEST         0x0C
#define FIRMWARE_ACTIVATE           0x10
#define FIRMWARE_IMAGE_DOWNLOAD     0x11
#define FORMAT_NVM                  0x80
#define SECURITY_SEND               0x81
#define SECURITY_RECEIVE            0x82

//NVM Command Set Figure 121 NVM Express 1.1
#define NVME_FLUSH                  0x00
#define NVME_WRITE                  0x01
#define NVME_READ                   0x02
#define NVME_WRITE_UNCORRECTABLE    0x04
#define NVME_COMPARE                0x05
#define NVME_WRITE_ZEROES           0x08
#define NVME_DATASET_MANAGEMENT     0x09

#define QUEUE_DOORBELL_OFFSET(QUEUE_NUM, TAIL_HEAD, DoorBellStride) \
        (0x1000 + (((QUEUE_NUM * 2)  + TAIL_HEAD) *  (4 << DoorBellStride)))

#define ADMIN_QUEUE_SIZE          0x100
#define IO_QUEUE_SIZE             0x100

#pragma pack(1)

// Figure 10 NVM Express 1.1
typedef struct {
    UINT8   Opcode;
    UINT8   FusedOperation : 2;
    UINT8   RESERVED1 : 5;
    UINT8   PSDT : 1;
    UINT16  CommandIdentifier;
} STRUC_CMD0;

typedef struct {
    STRUC_CMD0  CMD0;
    UINT32      NSID;
    UINT64      RESERVED1;
    UINT64      MPTR;
    UINT64      PRP1;
    UINT64      PRP2;
    UINT32      CDW10;
    UINT32      CDW11;
    UINT32      CDW12;
    UINT32      CDW13;
    UINT32      CDW14;
    UINT32      CDW15;
} NVME_ADMIN_COMMAND;

typedef struct {
    NVME_ADMIN_COMMAND    NvmCmd;
    BOOLEAN               AdminOrNVMCmdSet;           // TRUE for Admin
    UINT16                SQIdentifier;               // Queue 0 for Admin cmds and 1 for NVM cmds
    UINT32                CmdTimeOut;                 // In msec.
    UINT8                 RetryCount;
} NVME_COMMAND_WRAPPER;

//Figure 25  NVM Express 1.1 Spec
typedef struct {
    UINT32      DW0;
    UINT32      DW1;
    UINT16      SQHeadPointer;
    UINT16      SQIdentifier;
    UINT16      CommandIdentifier;
    UINT16      PhaseTag :  1;
    UINT16      StatusCode :  8;
    UINT16      StatusCodeType : 3;
    UINT16      Reservered1 : 2;
    UINT16      More : 1;
    UINT16      DoNotRetry  : 1;
} COMPLETION_QUEUE_ENTRY;

// Figure 83 NVM Express 1.1 Spec
typedef struct {
    UINT16      MP;                 // Bits 15:0
    UINT8       Reserved1;          // Bits 23:16
    UINT8       MPS : 1;            // Bits 24
    UINT8       NOPS : 1;           // Bits 25
    UINT8       Reserved2 : 6;      // Bits 31:26
    UINT32      ENLAT;              // Bits 63:32
    UINT32      EXLAT;              // Bits 95:64
    UINT8       RRT : 5;            // Bits 100:96
    UINT8       Reserved3 : 3;      // Bits 103:101
    UINT8       RRL : 5;            // Bits 108:104
    UINT8       Reserved4 : 3;      // Bits 111:109
    UINT8       RWT : 5;            // Bits 116:112
    UINT8       Reserved5 : 3;      // Bits 119:117
    UINT8       RWL : 5;            // Bits 124:120
    UINT8       Reserved6 : 3;      // Bits 127:125
    UINT8       Reserved7[16];
} POWER_STATE_DESCRIPTOR;

// Figure 82 NVM Express 1.1 spec
typedef struct {
    UINT16                  VID;                    // Offset 01:00                
    UINT16                  SSVID;                  // Offset 03:02
    UINT8                   SerialNumber[20];       // Offset 23:04
    UINT8                   ModelNumber[40];        // Offset 63:24
    UINT8                   FirmwareRevision[8];    // Offset 71:64
    UINT8                   ArbitrationBurst;       // Offset 72
    UINT8                   IEEEOUIIdentifier[3];   // Offset 75:73
    UINT8                   CMIC;                   // Offset 76
    UINT8                   MDTS;                   // Offset 77
    UINT16                  ControllerID;           // Offset 79:78
    UINT8                   Reserved1[176];         // Offset 255:80
    UINT16                  OACS;                   // Offset 257:256
    UINT8                   ACL;                    // Offset 258
    UINT8                   AERL;                   // Offset 259
    UINT8                   FRMW;                   // Offset 260
    UINT8                   LPA;                    // Offset 261
    UINT8                   ELPE;                   // Offset 262
    UINT8                   NPSS;                   // Offset 263
    UINT8                   AVSCC;                  // Offset 264
    UINT8                   APSTA;                  // Offset 265
    UINT8                   Reserved2[246];          // Offset 511:266
    UINT8                   SQES;                   // Offset 512
    UINT8                   CQES;                   // Offset 513
    UINT16                  Reserved3;              // Offset 515:514
    UINT32                  NN;                     // Offset 519:516
    UINT16                  ONCS;                   // Offset 521:520
    UINT16                  FUSES;                  // Offset 523:522
    UINT8                   FNA;                    // Offset 524
    UINT8                   VWC;                    // Offset 525
    UINT16                  AWUN;                   // Offset 527:526
    UINT16                  AWUPF;                  // Offset 529:528
    UINT8                   NVSCC;                  // Offset 530
    UINT8                   Reserved4;              // Offset 531
    UINT16                  ACWU;                   // Offset 533:532
    UINT16                  Reserved5;              // Offset 535:536
    UINT32                  SGLS;                   // Offset 539:536
    UINT8                   Reserved6[164];         // Offset 703:540
    UINT8                   Reserved7[1344];        // Offset 2047:704
    POWER_STATE_DESCRIPTOR  psdd[32];   // Offset 3071:2048
    UINT8                   VS[1024];               // Offset 4095:3072
} IDENTIFY_CONTROLLER_DATA;

// Figure 85 NVM Express 1.1 Spec
typedef struct {
    UINT16      MS;                     // Bits 15:0
    UINT8       LBADS;                  // Bits 23:16
    UINT8       RP : 2;                 // Bits 25:24
    UINT8       Reserved1 :6;           // Bits 31:26
} LBA_FORMAT_DATA;

// Figure 84 NVM Express 1.1 Spec
typedef struct {
    
    UINT64             NSIZE;                  // Offset 7:0
    UINT64             NCAP;                   // Offset 15:8
    UINT64             NUSE;                   // Offset 23:16
    UINT8              NSFEAT;                 // Offset 24
    UINT8              NLBAF;                  // Offset 25
    UINT8              FLBAS;                  // Offset 26
    UINT8              MC;                     // Offset 27
    UINT8              DPC;                    // Offset 28
    UINT8              DPS;                    // Offset 29
    UINT8              NMIC;                   // Offset 30
    UINT8              RESCAP;                 // Offset 31
    UINT8              Reserved1[88];           // Offset 119:32
    UINT64             EUI64;                  // Offset 127:120
    LBA_FORMAT_DATA    LBAF[16];          // Offset 191:128
    UINT8              Reserved2[192];         // Offset 383:192
    UINT8              VS[3712];               // Offset 4095:384
} IDENTIFY_NAMESPACE_DATA;

typedef struct {
    UINT64            BarAddress;
    UINT8             BusNumber;
    UINT8             Device;
    UINT8             Function;
    UINT16            CommandIdentifierAdmin;
    UINT16            CommandIdentifierQueue1;
    UINT64            RawNvmeCapability;
    UINT32            MaxQueueEntrySupported;
    BOOLEAN           ContiguousQueueRequired;
    BOOLEAN           ArbitrationMechanismSupport;
    UINT8             TimeOut;
    UINT8             DoorBellStride;
    BOOLEAN           NVMResetSupport;
    UINT8             CmdSetsSupported;
    UINT32            MemoryPageSizeMin;
    UINT32            MemoryPageSizeMax;
    UINT32            MemoryPageSize;

    //----------------------------Admin Nvme Cmds------------------------------------------------------
    UINT64            AdminSubmissionUnAligned;         // Points to the start of the buffer
    UINTN             AdminSubmissionUnAlignedSize;
    UINT64            AdminSubmissionQueue;             // Points to the start of the buffer Aligned
    UINTN             AdminSubmissionQueueSize;
    UINT16            AdminSubmissionQueueHeadPtr;
    UINT16            AdminSubmissionQueueTailPtr;
    
    UINT64            AdminCompletionUnAligned;
    UINTN             AdminCompletionUnAlignedSize;
    UINT64            AdminCompletionQueue;             // Points to the start of the buffer Aligned
    UINTN             AdminCompletionQueueSize;
    UINT16            AdminCompletionQueueHeadPtr;
    UINT16            AdminCompletionQueueTailPtr;
    UINT8             AdminPhaseTag : 1;
    UINT8             RESERVED : 7;
    //------------------------------------------------------------------------------------------------

    
    //----------------------------Queue1 for Nvme Cmds------------------------------------------------------
    UINT16            NVMQueueNumber;
    UINT64            Queue1SubmissionUnAligned;        // Points to the start of the buffer
    UINTN             Queue1SubmissionUnAlignedSize;    // Original size
    UINT64            Queue1SubmissionQueue;            // Aligned address

    UINT64            Queue1CompletionUnAligned;
    UINTN             Queue1CompletionUnAlignedSize;
    UINT64            Queue1CompletionQueue;

    UINT32            Queue1SubmissionQueueSize;
    UINT32            Queue1CompletionQueueSize;
    UINT16            Queue1CompletionQueueHeadPtr;  
    UINT16            Queue1CompletionQueueTailPtr;  
    UINT16            Queue1SubmissionQueueHeadPtr;      // Updated from Completion queue entry
    UINT16            Queue1SubmissionQueueTailPtr;      // Updated when new cmds are issued
    UINT8             Queue1PhaseTag : 1;
    UINT8             RESERVED1 : 7;
    
    IDENTIFY_CONTROLLER_DATA    *IdentifyData;  
    NVME_COMMAND_WRAPPER        *NvmeCmdWrapper;                  // Will be used by internal routines
    BOOLEAN                     ReInitializingQueue1;             // Flag used to prevent initializing Queue1 in an infinite loop
} NVME_RECOVERY_CONTROLLER_INFO;

typedef struct {
    UINT32                        ActiveNameSpaceID;
    UINT32                        Signature;
    IDENTIFY_NAMESPACE_DATA       IdentifyNamespaceData;
    UINT64                        *PRP2List;                       // if the data transfer size is more than twice the max. page size 
} ACTIVE_NAMESPACE_DATA;

typedef struct {
    UINT32                        ActiveNameSpaceID;
    ACTIVE_NAMESPACE_DATA         ActiveNameSpaceData;
    NVME_RECOVERY_CONTROLLER_INFO *NvmeControllerInfo;
    EFI_PEI_BLOCK_IO_MEDIA        MediaInfo;
    BOOLEAN                       LookedForMedia;
} NVME_RECOVERY_DEVICE_INFO;

#pragma pack()

//MMIO Access
#define MmAddress( BaseAddr, Register ) \
        ((UINT64)(BaseAddr) + \
        (UINTN)(Register) \
         )
#define Mm32Ptr( BaseAddr, Register ) \
        ((volatile UINT32 *)MmAddress (BaseAddr, Register ))

#define Mm16Ptr( BaseAddr, Register ) \
        ((volatile UINT16 *)MmAddress (BaseAddr, Register ))

#define Mm8Ptr( BaseAddr, Register ) \
        ((volatile UINT8 *)MmAddress (BaseAddr, Register ))

//Controller Generic Registers

#define CONTROLLER_REG32( BaseAddr, Register ) \
        (*Mm32Ptr ((BaseAddr), (Register)))

#define CONTROLLER_REG16( BaseAddr, Register ) \
        (*Mm16Ptr ((BaseAddr), (Register)))

#define CONTROLLER_REG8( BaseAddr, Register ) \
        (*Mm8Ptr ((BaseAddr), (Register)))

#define CONTROLLER_WRITE_REG32( BaseAddr, Register, Data ) \
        (CONTROLLER_REG32 ((BaseAddr), (Register))) = ((UINT32) (Data))

#define CONTROLLER_WRITE_REG16( BaseAddr, Register, Data ) \
        (CONTROLLER_REG16 ((BaseAddr), (Register))) = ((UINT16) (Data))

#define CONTROLLER_WRITE_REG8( BaseAddr, Register, Data ) \
        (CONTROLLER_REG8 ((BaseAddr), (Register))) = ((UINT8) (Data))

#define CONTROLLER_REG8_OR( BaseAddr, Register, OrData) \
        (CONTROLLER_REG8 ((BaseAddr), (Register))) |= ((UINT8) (OrData))

#define CONTROLLER_REG16_OR( BaseAddr, Register, OrData) \
        (CONTROLLER_REG16 ((BaseAddr), (Register))) |= ((UINT16) (OrData))

#define CONTROLLER_REG32_OR( BaseAddr, Register, OrData) \
        (CONTROLLER_REG32 ((BaseAddr), (Register))) = (CONTROLLER_REG32 ((BaseAddr), (Register))) | ((UINT32) (OrData))

#define CONTROLLER_REG8_AND( BaseAddr, Register, AndData) \
        (CONTROLLER_REG8 ((BaseAddr), (Register))) = (CONTROLLER_REG8 ((BaseAddr), (Register))) & ((UINT8) (AndData))

#define CONTROLLER_REG16_AND( BaseAddr, Register, AndData) \
        (CONTROLLER_REG16 ((BaseAddr), (Register))) &= ((UINT16) (AndData))

#define CONTROLLER_REG32_AND( BaseAddr, Register, AndData) \
        (CONTROLLER_REG32 ((BaseAddr), (Register))) = (CONTROLLER_REG32 ((BaseAddr), (Register))) & ((UINT32) (AndData))

#define CONTROLLER_REG8_AND_OR( BaseAddr, Register, AndData, OrData) \
        (CONTROLLER_REG8 ((BaseAddr), (Register)) = \
        (((CONTROLLER_REG8 ((BaseAddr), (Register))) & ((UINT8) (AndData))) | ((UINT8) (OrData))))

#define CONTROLLER_REG16_AND_OR( BaseAddr, Register, AndData, OrData) \
        (CONTROLLER_REG16 ((BaseAddr), (Register)) = \
        (((CONTROLLER_REG16 ((BaseAddr), (Register))) & ((UINT16) AndData)) | ((UINT16) (OrData))))

#define CONTROLLER_REG32_AND_OR( BaseAddr, Register,AndData,  OrData) \
        (CONTROLLER_REG32 ((BaseAddr), (Register)) = \
        (((CONTROLLER_REG32 ((BaseAddr), (Register))) & ((UINT32) (AndData))) | ((UINT32) (OrData))))

EFI_STATUS
ExecuteNvmeCmd (
    IN  NVME_RECOVERY_CONTROLLER_INFO    *NvmeControllerInfo,
    IN  NVME_COMMAND_WRAPPER             *NvmeCmdWrapper,
    OUT COMPLETION_QUEUE_ENTRY           *CmdCompletionData
);

EFI_STATUS
RecreateQueue1 (
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo
);

EFI_STATUS
CreateAdditionalSubmissionCompletionQueue ( 
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo,
    IN  NVME_COMMAND_WRAPPER            *NvmeCmdWrapper,
    IN  UINT16                          QueueNumber,
    IN  UINT32                          QueueSize
);

EFI_STATUS
WaitForCompletionQueueUpdate (
    IN  NVME_RECOVERY_CONTROLLER_INFO       *NvmeControllerInfo,
    IN  NVME_COMMAND_WRAPPER                *NvmeCmdWrapper,
    OUT COMPLETION_QUEUE_ENTRY              *CmdCompletionData 
);

EFI_STATUS
ConfigureActiveNamespace(
    NVME_RECOVERY_DEVICE_INFO   *NvmeDeviceInfo
);

EFI_STATUS  
GetIdentifyData (
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo, 
    OUT UINT8                           *IdentifyData,
    IN  UINT8                           ControllerNameSpaceStructure, 
    IN  UINT32                          NameSpaceID
);

EFI_STATUS
SetNumberOfQueues (
    NVME_RECOVERY_CONTROLLER_INFO *NvmeControllerInfo
);

EFI_STATUS
NvmeReadBlocks (
    IN  NVME_RECOVERY_DEVICE_INFO *NvmeDeviceInfo,
    IN  EFI_LBA                   LBA,
    IN  UINTN                     BufferSize,
    OUT VOID                      *Buffer,
    IN  UINT8                     ReadWriteOpCode
);

EFI_STATUS
ProgramPRP2List (
    IN  UINT64  *PRP2List, 
    IN  UINT32  PageSize, 
    IN  UINTN   BufferAddress,
    IN  UINTN   BufferSize,
    IN  UINTN   *PRP2TransferSize
);

VOID
PrintExecuteNvmeCmdData (
     NVME_COMMAND_WRAPPER    *NvmeCmdWrapper
);

VOID
PrintIdentifyDataStructure (
    IN  UINT8   *IdentifyData, 
    IN  UINT8   ControllerNameSpaceStructure
);

#endif // _AMI_NVME_CTRL_H

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
