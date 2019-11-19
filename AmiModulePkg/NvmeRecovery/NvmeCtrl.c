//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeCtrl.c
    This file contains the code which provide access to the Nvme 
    Controller/Device

**/

//----------------------------------------------------------------------
#include "NvmeRecovery.h"
//----------------------------------------------------------------------

extern EFI_PEI_SERVICES       **gPeiServices;
extern EFI_PEI_STALL_PPI      *gStallPpi;
extern EFI_PEI_PCI_CFG2_PPI   *gPciCfg;

/**
    Function to recreate Admin,IO Submission/Completion queues

    @param  NvmeControllerInfo

    @retval EFI_STATUS
**/
EFI_STATUS
RecreateAllQueues (
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    UINT32          ProgramCC = 0;
    UINT32          Delay;

    ProgramCC = CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CC);

    // Check if the controller is already running. If yes stop it.
    Delay = NvmeControllerInfo->TimeOut * 500;
    if (CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CC) & 0x1) {
        
        //  Clear Control register
        CONTROLLER_REG32 (NvmeControllerInfo->BarAddress, Offset_CC) = 0;
        do {
            if (!(CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CSTS) & 0x1)) {
                break;
            }
            gStallPpi->Stall((CONST EFI_PEI_SERVICES**)gPeiServices, (CONST EFI_PEI_STALL_PPI*)gStallPpi, 1000 );

        } while (--Delay);
    }

    if (!Delay) {
        goto  RecreateAllQueues_Error;  
    }

    // Program Admin Queue Size and Base Address
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Aqa) =  
             ((NvmeControllerInfo->AdminCompletionQueueSize - 1) << 16) + (NvmeControllerInfo->AdminSubmissionQueueSize - 1);

    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Asq) = 
             (UINT32) NvmeControllerInfo->AdminSubmissionQueue;
     
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Asq + 4) =  
             (UINT32) RShiftU64(NvmeControllerInfo->AdminSubmissionQueue, 32);
     
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Acq) =  
             (UINT32)NvmeControllerInfo->AdminCompletionQueue;
     
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Acq + 4) =  
             (UINT32) RShiftU64(NvmeControllerInfo->AdminCompletionQueue, 32);

    NvmeControllerInfo->AdminPhaseTag = FALSE;

    // Enable Controller
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CC) = (ProgramCC | 1);

    // Wait for the controller to get ready
    // Check if the controller is already running. If yes stop it.
    Delay = NvmeControllerInfo->TimeOut * 500;
    do {
        if ((CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CSTS) & 0x1)) {
            break;
        }
        gStallPpi->Stall((CONST EFI_PEI_SERVICES**)gPeiServices, (CONST EFI_PEI_STALL_PPI*)gStallPpi, 1000 );
    } while (--Delay);

RecreateAllQueues_Error:
    if (!Delay) {
        Status = EFI_DEVICE_ERROR;
        return Status;
    }

    NvmeControllerInfo->AdminSubmissionQueueHeadPtr = 0;
    NvmeControllerInfo->AdminSubmissionQueueTailPtr = 0;
    NvmeControllerInfo->AdminCompletionQueueHeadPtr = 0;
    NvmeControllerInfo->AdminCompletionQueueTailPtr = 0;
    NvmeControllerInfo->AdminPhaseTag = FALSE;

    if (!NvmeControllerInfo->ReInitializingQueue1) {
        Status = RecreateQueue1 (NvmeControllerInfo);
    }
    return Status;
}

/**
    Function to add command to Admin submission queue

    @param  NvmeControllerInfo
    @param  NvmeCmdWrapper

    @retval EFI_STATUS
**/
EFI_STATUS
AddToAdminSubmissionQueue (
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo,
    IN  NVME_COMMAND_WRAPPER            *NvmeCmdWrapper
)
{
    EFI_STATUS          Status;
    UINT64              DestinationAddress;

    // Is Admin Queue full?
    if ( NvmeControllerInfo->AdminSubmissionQueueHeadPtr ) {  // Non-zero value
        if ((NvmeControllerInfo->AdminSubmissionQueueTailPtr - 1) == NvmeControllerInfo->AdminSubmissionQueueHeadPtr){
                    
            // In this design, queue should never get filled up. 
            // If it does something is wrong. Delete and start all over again.
          
            Status = RecreateAllQueues (NvmeControllerInfo);
            if (EFI_ERROR(Status)) {            
                return EFI_OUT_OF_RESOURCES;
            }
        }
    } else {
        // If Head is at the start and Tail is at the end, then queue is full
        if (NvmeControllerInfo->AdminSubmissionQueueTailPtr == (NvmeControllerInfo->AdminSubmissionQueueHeadPtr + 
              NvmeControllerInfo->AdminSubmissionQueueSize - 1)) {
            
            // In this design, queue should never get filled up. 
            // If it does something is wrong. Delete and start all over again.
              Status = RecreateAllQueues (NvmeControllerInfo);
              if (EFI_ERROR(Status)) {            
                  return EFI_OUT_OF_RESOURCES;
              }
        }
    }

    // Copy cmd to Admin Queue
    DestinationAddress = NvmeControllerInfo->AdminSubmissionQueue + (
                              NvmeControllerInfo->AdminSubmissionQueueTailPtr * sizeof(NVME_ADMIN_COMMAND));
#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n Destination address for Admin Cmd     : %lx\n",DestinationAddress)); 
#endif

    (**gPeiServices).CopyMem ( (VOID*)DestinationAddress, 
                               &(NvmeCmdWrapper->NvmCmd),
                               sizeof(NVME_ADMIN_COMMAND) );

    NvmeControllerInfo->AdminSubmissionQueueTailPtr++;
    
    // Check if there is a roller over
    if (NvmeControllerInfo->AdminSubmissionQueueTailPtr >= (NvmeControllerInfo->AdminSubmissionQueueSize)) {
        NvmeControllerInfo->AdminSubmissionQueueTailPtr = 0;
    }

    return EFI_SUCCESS;
}

/**
    Function to create IO Submission/Completion queues

    @param  NvmeControllerInfo
    @param  NvmeCmdWrapper
    @param  QueueNumber
    @param  QueueSize

    @retval EFI_STATUS
**/
EFI_STATUS
CreateAdditionalSubmissionCompletionQueue ( 
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo,
    IN  NVME_COMMAND_WRAPPER            *NvmeCmdWrapper,
    IN  UINT16                          QueueNumber,
    IN  UINT32                          QueueSize
)
{
    EFI_STATUS              Status;
    COMPLETION_QUEUE_ENTRY  CompletionData;
    UINTN                   AllocatePageSize;
    UINT64                  QueueAddress = 0;

     (**gPeiServices).SetMem ( NvmeCmdWrapper,
                               sizeof(NVME_COMMAND_WRAPPER),
                               0 );

    // Allocate memory only if the pointer is NULL
    if (NvmeControllerInfo->Queue1CompletionUnAligned == 0) {
        AllocatePageSize = NvmeControllerInfo->MemoryPageSize + QueueSize * sizeof(COMPLETION_QUEUE_ENTRY);

        Status = (*gPeiServices)->AllocatePages ( (const EFI_PEI_SERVICES **)gPeiServices, \
                                                  EfiRuntimeServicesData, \
                                                  EFI_SIZE_TO_PAGES(AllocatePageSize), \
                                                  (EFI_PHYSICAL_ADDRESS*)&(QueueAddress) );
        
        if (EFI_ERROR(Status)) {
            return Status;
        }

        NvmeControllerInfo->Queue1CompletionUnAligned = QueueAddress;
        NvmeControllerInfo->Queue1CompletionUnAlignedSize = EFI_SIZE_TO_PAGES(AllocatePageSize);
        NvmeControllerInfo->Queue1CompletionQueue = (QueueAddress & ~(NvmeControllerInfo->MemoryPageSize - 1)) + 
                                                      NvmeControllerInfo->MemoryPageSize;

        NvmeControllerInfo->Queue1CompletionQueueSize = QueueSize;

        (**gPeiServices).SetMem ( (VOID *)NvmeControllerInfo->Queue1CompletionQueue,
                                  NvmeControllerInfo->Queue1CompletionQueueSize * sizeof (COMPLETION_QUEUE_ENTRY),
                                  0 );
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }

    // Build NVME command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = CREATE_IO_COMPLETION_QUEUE;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeControllerInfo->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.NSID = 0;
    
    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)NvmeControllerInfo->Queue1CompletionQueue;
    NvmeCmdWrapper->NvmCmd.PRP2 = 0;
    NvmeCmdWrapper->NvmCmd.CDW10 = ((QueueSize - 1) << 16 )+ QueueNumber;
    NvmeCmdWrapper->NvmCmd.CDW11 = 1;           // Contiguous
        
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;           // Cmd issued in Queue0
    NvmeCmdWrapper->CmdTimeOut = 1000;

    Status = ExecuteNvmeCmd (NvmeControllerInfo, NvmeCmdWrapper, &CompletionData);
    if (EFI_ERROR(Status)) {
        goto CreateAdditionalSubmissionCompletionQueue_Exit;
    }

    // Allocate memory only if the pointer is NULL
    if (NvmeControllerInfo->Queue1SubmissionUnAligned == 0) {

        AllocatePageSize = NvmeControllerInfo->MemoryPageSize + 
                           QueueSize * sizeof(NVME_ADMIN_COMMAND);

        Status = (*gPeiServices)->AllocatePages ( (const EFI_PEI_SERVICES **)gPeiServices, \
                                                 EfiRuntimeServicesData, \
                                                 EFI_SIZE_TO_PAGES(AllocatePageSize), \
                                                 (EFI_PHYSICAL_ADDRESS*)&(QueueAddress) );

        if (EFI_ERROR(Status)) {
            return Status;
        }

        NvmeControllerInfo->Queue1SubmissionUnAligned = QueueAddress;
        NvmeControllerInfo->Queue1SubmissionUnAlignedSize = EFI_SIZE_TO_PAGES(AllocatePageSize);
        NvmeControllerInfo->Queue1SubmissionQueue = (QueueAddress & ~(NvmeControllerInfo->MemoryPageSize - 1)) + 
                                                            NvmeControllerInfo->MemoryPageSize;

        NvmeControllerInfo->Queue1SubmissionQueueSize = QueueSize;
        (**gPeiServices).SetMem ( (VOID *)NvmeControllerInfo->Queue1SubmissionQueue,
                                   NvmeControllerInfo->Queue1SubmissionQueueSize * sizeof (NVME_ADMIN_COMMAND),
                                   0 );
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }

    // Build NVME command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = CREATE_IO_SUBMISSION_QUEUE;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeControllerInfo->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.NSID = 0;
            
    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)NvmeControllerInfo->Queue1SubmissionQueue;
    NvmeCmdWrapper->NvmCmd.PRP2 = 0;
    NvmeCmdWrapper->NvmCmd.CDW10 = ((QueueSize - 1) << 16 )+ QueueNumber;
    NvmeCmdWrapper->NvmCmd.CDW11 = (QueueNumber << 16) + 1;           // Contiguous

    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;           // Cmd issued to admin queue
    NvmeCmdWrapper->CmdTimeOut = 1000;

    Status = ExecuteNvmeCmd (NvmeControllerInfo, NvmeCmdWrapper, &CompletionData);
    if (EFI_ERROR(Status)) {
        return Status;
    }

CreateAdditionalSubmissionCompletionQueue_Exit:
    NvmeControllerInfo->Queue1PhaseTag = FALSE;

    NvmeControllerInfo->Queue1SubmissionQueueTailPtr = 0;
    NvmeControllerInfo->Queue1SubmissionQueueHeadPtr = 0;

    NvmeControllerInfo->Queue1CompletionQueueTailPtr = 0;
    NvmeControllerInfo->Queue1CompletionQueueHeadPtr = 0;

    return Status;
}

/**
    Function to recreate IO Submission/Completion queues

    @param  NvmeControllerInfo

    @retval EFI_STATUS
**/
EFI_STATUS
RecreateQueue1 (
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo
)
{
    EFI_STATUS                Status;
    NVME_COMMAND_WRAPPER      *NvmeCmdWrapper = NvmeControllerInfo->NvmeCmdWrapper;
    COMPLETION_QUEUE_ENTRY    CompletionData;
    
    NvmeControllerInfo->ReInitializingQueue1 = TRUE;

    (**gPeiServices).SetMem ( NvmeCmdWrapper,
                              sizeof(NVME_COMMAND_WRAPPER),
                              0 );

    // Build NVME command to delete Submission queue
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = DELETE_IO_SUBMISSION_QUEUE;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeControllerInfo->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.CDW10 = NvmeControllerInfo->NVMQueueNumber;
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;           // Queue 0 for Admin cmds
    NvmeCmdWrapper->CmdTimeOut = 1000;
    
    Status = ExecuteNvmeCmd (NvmeControllerInfo, NvmeCmdWrapper, &CompletionData);
    if (EFI_ERROR(Status)) {
        goto RecreateQueue1_Error;
    }
    
    // Build NVME command to delete Completion queue
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = DELETE_IO_COMPLETION_QUEUE;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeControllerInfo->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.CDW10 = NvmeControllerInfo->NVMQueueNumber;    
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;           // Queue 0 for Admin cmds
    NvmeCmdWrapper->CmdTimeOut = 1000;
    
    Status = ExecuteNvmeCmd (NvmeControllerInfo, NvmeCmdWrapper, &CompletionData);
    if (EFI_ERROR(Status)) {
        goto RecreateQueue1_Error;
    }

    // Create Submission and Completion Queue1
    Status  = CreateAdditionalSubmissionCompletionQueue(
                                NvmeControllerInfo, 
                                NvmeCmdWrapper, 
                                NvmeControllerInfo->NVMQueueNumber, 
                                NvmeControllerInfo->Queue1SubmissionQueueSize
                                );
RecreateQueue1_Error:
    NvmeControllerInfo->ReInitializingQueue1 = FALSE;
    return Status;
}

/**
    Function to add Nvme command in IO submission queue

    @param  NvmeControllerInfo
    @param  NvmeCmdWrapper

    @retval EFI_STATUS
**/
EFI_STATUS
AddToQueue1SubmissionQueue (
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo,
    IN  NVME_COMMAND_WRAPPER            *NvmeCmdWrapper
)
{
    EFI_STATUS          Status;
    UINT64              DestinationAddress;

    // Is Queue full? If Tail is one less than the Head queue is full.
    if (NvmeControllerInfo->Queue1SubmissionQueueHeadPtr) {  // Non-zero value
        if ((NvmeControllerInfo->Queue1SubmissionQueueTailPtr - 1) == NvmeControllerInfo->Queue1SubmissionQueueHeadPtr){
            
            // In this design, queue should never get filled up. 
            // If it does something is wrong. Delete and start all over again.
            Status = RecreateQueue1 (NvmeControllerInfo);
            if (EFI_ERROR(Status)) {    
                return EFI_OUT_OF_RESOURCES;
            }
        }
    } else { 
        // If Head is at the start and Tail is at the end, then queue is full
        if (NvmeControllerInfo->Queue1SubmissionQueueTailPtr == (NvmeControllerInfo->Queue1SubmissionQueueHeadPtr + 
                                    NvmeControllerInfo->Queue1SubmissionQueueSize - 1)) {
            
            // In this design, queue should never get filled up. 
            // If it does something is wrong. Delete and start all over again.
            
            Status = RecreateQueue1 (NvmeControllerInfo);
            if (EFI_ERROR(Status)) {          
                return EFI_OUT_OF_RESOURCES;
            }
        }
    }

    // Copy cmd to Admin Queue
    DestinationAddress = NvmeControllerInfo->Queue1SubmissionQueue + 
                        (NvmeControllerInfo->Queue1SubmissionQueueTailPtr * sizeof(NVME_ADMIN_COMMAND));

#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n DestinationAddress for IO command :%lx\n",DestinationAddress)); 
#endif

    (**gPeiServices).CopyMem ( (VOID*)DestinationAddress, 
                               &(NvmeCmdWrapper->NvmCmd),
                               sizeof(NVME_ADMIN_COMMAND) );

    NvmeControllerInfo->Queue1SubmissionQueueTailPtr++;

    // Check if there is a roller over
    if (NvmeControllerInfo->Queue1SubmissionQueueTailPtr >= NvmeControllerInfo->Queue1SubmissionQueueSize) {
        NvmeControllerInfo->Queue1SubmissionQueueTailPtr = 0;
    }
    return EFI_SUCCESS;
}

/**
    Function used to update the Door bell register to process 
    Nvme command

    @param  NvmeControllerInfo
    @param  QueueNumber
    @param  Value

    @retval EFI_STATUS
**/
EFI_STATUS
UpdateDoorBellRegister (
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo,
    IN  UINT16                          QueueNumber,
    IN  UINT32                          Value
)
{
    UINT32      Offset;

    // Update Door Bell Register
    Offset = QUEUE_DOORBELL_OFFSET(QueueNumber, 0, NvmeControllerInfo->DoorBellStride);
    
#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n DoorBell Offset %lx Value %x\n", NvmeControllerInfo->BarAddress + Offset, Value)); 
#endif

    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset) = Value;
    return EFI_SUCCESS;
}

/**
    Function used to send the Nvme command to the Nvme Controler/Drive

    @param  NvmeControllerInfo
    @param  NvmeCmdWrapper
    @param  CmdCompletionData

    @retval EFI_STATUS
**/
EFI_STATUS
ExecuteNvmeCmd (
    IN  NVME_RECOVERY_CONTROLLER_INFO    *NvmeControllerInfo,
    IN  NVME_COMMAND_WRAPPER             *NvmeCmdWrapper,
    OUT COMPLETION_QUEUE_ENTRY           *CmdCompletionData
)

{
    EFI_STATUS      Status;

    PrintExecuteNvmeCmdData(NvmeCmdWrapper);

    do {
        // Because of recursive nature and retry mechanism, cmd identifier needs to be updated just before giving the call.
        NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeCmdWrapper->AdminOrNVMCmdSet ? NvmeControllerInfo->CommandIdentifierAdmin : \
                                                                           NvmeControllerInfo->CommandIdentifierQueue1;
        
        NvmeCmdWrapper->AdminOrNVMCmdSet ? (Status = AddToAdminSubmissionQueue(NvmeControllerInfo, NvmeCmdWrapper)) : \
                (Status = AddToQueue1SubmissionQueue(NvmeControllerInfo, NvmeCmdWrapper));
    
        if (EFI_ERROR(Status)) {
            continue;
        }
    
        Status = UpdateDoorBellRegister(NvmeControllerInfo, 
                                        NvmeCmdWrapper->SQIdentifier,
                                        NvmeCmdWrapper->AdminOrNVMCmdSet ? NvmeControllerInfo->AdminSubmissionQueueTailPtr :
                                                NvmeControllerInfo->Queue1SubmissionQueueTailPtr 
                                        );

        if (EFI_ERROR(Status)) {
            continue;
        }

        //Wait for cmd to complete
        if (NvmeCmdWrapper->NvmCmd.CMD0.Opcode != ASYNC_EVENT_REQUEST) {
            Status = WaitForCompletionQueueUpdate(NvmeControllerInfo, NvmeCmdWrapper, CmdCompletionData);
        }
    
    } while (EFI_ERROR(Status) && NvmeCmdWrapper->RetryCount--);

#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n Status: %x ", Status )); 
#endif
    return  Status;
}

/**
    Function checks Nvme controller for the completion of the command sent

    @param  NvmeControllerInfo
    @param  NvmeCmdWrapper
    @param  CmdCompletionData

    @retval EFI_STATUS
**/
EFI_STATUS
WaitForCompletionQueueUpdate (
    IN  NVME_RECOVERY_CONTROLLER_INFO       *NvmeControllerInfo,
    IN  NVME_COMMAND_WRAPPER                *NvmeCmdWrapper,
    OUT COMPLETION_QUEUE_ENTRY              *CmdCompletionData 
)
{
    EFI_STATUS              Status;
    UINT32                  TimeOut = NvmeCmdWrapper->CmdTimeOut;
    UINT32                  Offset;
    COMPLETION_QUEUE_ENTRY  *pCmdCompletionData;
    UINT16                  CommandIdentifier = NvmeCmdWrapper->AdminOrNVMCmdSet ? NvmeControllerInfo->CommandIdentifierAdmin : \
                                                NvmeControllerInfo->CommandIdentifierQueue1; 
    
    UINT64                  CompletionQueueStart = NvmeCmdWrapper->AdminOrNVMCmdSet ? NvmeControllerInfo->AdminCompletionQueue : \
                                                   NvmeControllerInfo->Queue1CompletionQueue; 
    
    UINT16                  CompletionQueueHeadPtr = NvmeCmdWrapper->AdminOrNVMCmdSet ? NvmeControllerInfo->AdminCompletionQueueHeadPtr :\
                                                     NvmeControllerInfo->Queue1CompletionQueueHeadPtr;

    // Toggle expected phase tag on every roll over
    if (CompletionQueueHeadPtr == 0) {
            NvmeCmdWrapper->AdminOrNVMCmdSet ? (NvmeControllerInfo->AdminPhaseTag = ~NvmeControllerInfo->AdminPhaseTag) : \
                        (NvmeControllerInfo->Queue1PhaseTag = ~NvmeControllerInfo->Queue1PhaseTag);
    }

    // Get the offset to the Command Completion Queue Head Pointer
    pCmdCompletionData = (COMPLETION_QUEUE_ENTRY *)(CompletionQueueStart + CompletionQueueHeadPtr * sizeof(COMPLETION_QUEUE_ENTRY));

    do {
        // Check whether Command Identifier, SQ ID matches and Phase Tag matches with the cmd issued.
        if ((pCmdCompletionData->CommandIdentifier == CommandIdentifier) && \
            (pCmdCompletionData->SQIdentifier == NvmeCmdWrapper->SQIdentifier) && \
            (pCmdCompletionData->PhaseTag == (NvmeCmdWrapper->AdminOrNVMCmdSet ? NvmeControllerInfo->AdminPhaseTag : NvmeControllerInfo->Queue1PhaseTag)))
        {
            break;
        }
        
        // check if there are any fatal errors
        if (CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CSTS) & CSTS_CFS) {
            return EFI_DEVICE_ERROR;
        }
        // 1msec delay
        gStallPpi->Stall((CONST EFI_PEI_SERVICES**)gPeiServices, (CONST EFI_PEI_STALL_PPI*)gStallPpi, 1000 );
        
    } while (--TimeOut);
    
    if (!TimeOut) {
        
        // This is a fatal condition. We should expect some kind of response from the controller. 
        // If not we have to either wait for more time or delete and recreate the queue
        
        NvmeCmdWrapper->AdminOrNVMCmdSet ? NvmeControllerInfo->CommandIdentifierAdmin++ : \
                                           NvmeControllerInfo->CommandIdentifierQueue1++;
        
        if (NvmeCmdWrapper->AdminOrNVMCmdSet) {
            Status = RecreateAllQueues(NvmeControllerInfo);
        } else {
            Status = RecreateQueue1(NvmeControllerInfo);
        }
        
        if (EFI_ERROR(Status)) {
            // If recreating the queue gets an error. nothing can be done.
            NvmeCmdWrapper->RetryCount = 0;
        } else {
            // Retry the cmd one more time
            NvmeCmdWrapper->RetryCount++;
        }
        return EFI_DEVICE_ERROR;
    }

    // Update HeadPtr from Completion Queue. 
    // Check what Queue was cmd posted to and then update the corresponding Head/Tail pointer
    if (NvmeCmdWrapper->AdminOrNVMCmdSet) {
        NvmeControllerInfo->AdminSubmissionQueueHeadPtr = pCmdCompletionData->SQHeadPointer;
        NvmeControllerInfo->AdminCompletionQueueHeadPtr = NvmeControllerInfo->AdminSubmissionQueueHeadPtr;
        Offset = QUEUE_DOORBELL_OFFSET(NvmeCmdWrapper->SQIdentifier, 1, NvmeControllerInfo->DoorBellStride);
        CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset) = NvmeControllerInfo->AdminCompletionQueueHeadPtr;
    } else { 
        NvmeControllerInfo->Queue1SubmissionQueueHeadPtr = pCmdCompletionData->SQHeadPointer;
        NvmeControllerInfo->Queue1CompletionQueueHeadPtr = NvmeControllerInfo->Queue1SubmissionQueueHeadPtr;
        Offset = QUEUE_DOORBELL_OFFSET(NvmeCmdWrapper->SQIdentifier, 1, NvmeControllerInfo->DoorBellStride);
        CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset) = NvmeControllerInfo->Queue1CompletionQueueHeadPtr;   
    }
    
    // Check whether all cmds submitted has been completed. CompletionQueue Head pointer should give a clue on
    // how many cmds where executed.
    if ( pCmdCompletionData->StatusCode || pCmdCompletionData->StatusCodeType ) {
        Status = EFI_DEVICE_ERROR;
    } else {
        Status = EFI_SUCCESS;
    }

#if ENABLE_NVME_RECOVERY_TRACES
    if (NvmeCmdWrapper->AdminOrNVMCmdSet) {
        PEI_TRACE((-1, gPeiServices, "\nAdminSubmissionQueueHeadPtr %08X\n", NvmeControllerInfo->AdminSubmissionQueueHeadPtr)); 
        PEI_TRACE((-1, gPeiServices, "AdminSubmissionQueueTailPtr %08X\n", NvmeControllerInfo->AdminSubmissionQueueTailPtr)); 
        PEI_TRACE((-1, gPeiServices, "AdminCompletionQueueHeadPtr %08X\n", NvmeControllerInfo->AdminCompletionQueueHeadPtr)); 
        PEI_TRACE((-1, gPeiServices, "AdminCompletionQueueTailPtr %08X\n", NvmeControllerInfo->AdminCompletionQueueTailPtr));
    } else {
        PEI_TRACE((-1, gPeiServices, "\nQueue1SubmissionQueueHeadPtr %08X\n", NvmeControllerInfo->Queue1SubmissionQueueHeadPtr));
        PEI_TRACE((-1, gPeiServices, "Queue1SubmissionQueueTailPtr %08X\n", NvmeControllerInfo->Queue1SubmissionQueueTailPtr));
        PEI_TRACE((-1, gPeiServices, "Queue1CompletionQueueHeadPtr %08X\n", NvmeControllerInfo->Queue1CompletionQueueHeadPtr));
        PEI_TRACE((-1, gPeiServices, "Queue1CompletionQueueTailPtr %08X\n", NvmeControllerInfo->Queue1CompletionQueueTailPtr));
    }
#endif

    // Update o/p buffer
    (**gPeiServices).CopyMem ( (VOID *)CmdCompletionData, 
                               pCmdCompletionData,
                               sizeof(COMPLETION_QUEUE_ENTRY) );

    NvmeCmdWrapper->AdminOrNVMCmdSet ? NvmeControllerInfo->CommandIdentifierAdmin++ : NvmeControllerInfo->CommandIdentifierQueue1++;
    return Status;
}

/**
    Functin to get the Identify data of the Nvme controller or Namespace

    @param  NvmeControllerInfo
    @param  IdentifyData
    @param  ControllerNameSpaceStructure
    @param  NameSpaceID

    @retval EFI_STATUS
**/
EFI_STATUS
GetIdentifyData (
    IN  NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo, 
    OUT UINT8                           *IdentifyData,
    IN  UINT8                           ControllerNameSpaceStructure, 
    IN  UINT32                          NameSpaceID
)
{
    EFI_STATUS              Status;
    NVME_COMMAND_WRAPPER    *NvmeCmdWrapper = NvmeControllerInfo->NvmeCmdWrapper;
    COMPLETION_QUEUE_ENTRY  CompletionData;
    
    (**gPeiServices).SetMem ( NvmeCmdWrapper,
                              sizeof(NVME_COMMAND_WRAPPER),
                              0 );
    
    // Build NVME command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = IDENTIFY;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeControllerInfo->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.NSID = NameSpaceID;
            
    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)(UINTN)IdentifyData;
    
    if( ((UINT64)(UINTN)IdentifyData & ~((UINT64)NvmeControllerInfo->MemoryPageSize-1)) ) {
         // If PRP1 isn't aligned on MemoryPageSize, then PRP2 will also be used.
         NvmeCmdWrapper->NvmCmd.PRP2 = ((UINT64)(UINTN)IdentifyData & ~((UINT64)NvmeControllerInfo->MemoryPageSize-1)) + 
                                        NvmeControllerInfo->MemoryPageSize; 
    } else {
         NvmeCmdWrapper->NvmCmd.PRP2 = 0;
    }

    NvmeCmdWrapper->NvmCmd.CDW10 = ControllerNameSpaceStructure;
    
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;           // Queue 0 for Admin cmds
    NvmeCmdWrapper->CmdTimeOut = 1000;

    Status = ExecuteNvmeCmd (NvmeControllerInfo, NvmeCmdWrapper, &CompletionData);
    
    if (!EFI_ERROR(Status)) {
#if ENABLE_NVME_RECOVERY_TRACES
        PrintIdentifyDataStructure (IdentifyData, ControllerNameSpaceStructure);
#endif
    }
    return Status;
}


/**
    Configures Specific Active Namespace of the Nvme Contoller

    @param  NvmeDeviceInfo

    @retval EFI_STATUS
**/
EFI_STATUS
ConfigureActiveNamespace(
    NVME_RECOVERY_DEVICE_INFO   *NvmeDeviceInfo
)
{
    EFI_STATUS                    Status;
    ACTIVE_NAMESPACE_DATA         *ActiveNameSpaceData = &NvmeDeviceInfo->ActiveNameSpaceData;
    NVME_RECOVERY_CONTROLLER_INFO *NvmeControllerInfo = NvmeDeviceInfo->NvmeControllerInfo;
    UINTN                         Size;
    UINT64                        Prp2BaseAddr=0;

    (**gPeiServices).SetMem ( ActiveNameSpaceData,
                              sizeof(ACTIVE_NAMESPACE_DATA),
                              0 );
    
    ActiveNameSpaceData->ActiveNameSpaceID = NvmeDeviceInfo->ActiveNameSpaceID;

    // Get list of Active NameSpace Data
    Status = GetIdentifyData ( NvmeControllerInfo, 
                               (UINT8*)&(ActiveNameSpaceData->IdentifyNamespaceData),
                               0, 
                               ActiveNameSpaceData->ActiveNameSpaceID
                               );
    
    if( EFI_ERROR(Status) ) {
        return Status;
    }
    
    (**gPeiServices).SetMem ( &NvmeDeviceInfo->MediaInfo,
                              sizeof(EFI_PEI_BLOCK_IO_MEDIA),
                              0 );

    // PRP2List will be cleared when used
    Size = NvmeControllerInfo->MemoryPageSize;

    Status = (*gPeiServices)->AllocatePages ( (const EFI_PEI_SERVICES **)gPeiServices,
                                              EfiRuntimeServicesData,
                                              EFI_SIZE_TO_PAGES(Size),
                                              (EFI_PHYSICAL_ADDRESS*)&Prp2BaseAddr );

    if (EFI_ERROR(Status)) {
        return Status;
    }

    ActiveNameSpaceData->PRP2List = (UINT64*)Prp2BaseAddr;

    (**gPeiServices).SetMem ( ActiveNameSpaceData->PRP2List,
                              EFI_SIZE_TO_PAGES(Size),
                              0 );
    
    NvmeDeviceInfo->MediaInfo.MediaPresent = TRUE;
    NvmeDeviceInfo->MediaInfo.DeviceType = MaxDeviceType;
    NvmeDeviceInfo->MediaInfo.LastBlock = (UINTN)ActiveNameSpaceData->IdentifyNamespaceData.NSIZE - 1;      // Name space Size
    
    Size = ActiveNameSpaceData->IdentifyNamespaceData.LBAF[ActiveNameSpaceData->IdentifyNamespaceData.FLBAS & 0xF].LBADS;
    NvmeDeviceInfo->MediaInfo.BlockSize = (UINT32) LShiftU64(1, Size);
    
    return EFI_SUCCESS;
}

/**
    Function to read buffer from Nvme controller

    @param  NvmeDeviceInfo
    @param  LBA
    @param  BufferSize
    @param  Buffer
    @param  ReadWriteOpCode

    @retval EFI_STATUS
**/
EFI_STATUS
NvmeReadBlocks (
    IN  NVME_RECOVERY_DEVICE_INFO *NvmeDeviceInfo,
    IN  EFI_LBA                   LBA,
    IN  UINTN                     BufferSize,
    OUT VOID                      *Buffer,
    IN  UINT8                     ReadWriteOpCode
)
{
    EFI_STATUS                      Status;
    UINTN                           DataN;
    EFI_PEI_BLOCK_IO_MEDIA          MediaInfo = NvmeDeviceInfo->MediaInfo;
    UINTN                           MaxTransferBufferSize = BufferSize;
    EFI_PHYSICAL_ADDRESS            TransferBuffer;
    NVME_RECOVERY_CONTROLLER_INFO   *NvmeControllerInfo = NvmeDeviceInfo->NvmeControllerInfo;
    NVME_COMMAND_WRAPPER            *NvmeCmdWrapper = NvmeControllerInfo->NvmeCmdWrapper;
    UINT32                          LBACountInOneTransfer;
    UINTN                           PRP1TransferSize;
    UINTN                           PRP2TransferSize;
    COMPLETION_QUEUE_ENTRY          CompletionData;
    
    if (BufferSize == 0) {
        return EFI_SUCCESS;
    }
    
    // Check whether the block size is multiple of BlkMedia->BlockSize
    DataN = BufferSize % MediaInfo.BlockSize;
    if (DataN){
        return EFI_BAD_BUFFER_SIZE;
    }
    
    // Check for Valid start LBA #
    if ( LBA > MediaInfo.LastBlock ) {
        return EFI_INVALID_PARAMETER;
    }
    
    // Check for Valid End LBA #
    DataN = BufferSize / MediaInfo.BlockSize;
    if (LBA + DataN > MediaInfo.LastBlock + 1) {
        return EFI_INVALID_PARAMETER;
    }
    
    do {
        // Limit the transfer size to MDTS value
        if (NvmeControllerInfo->IdentifyData->MDTS && (MaxTransferBufferSize > ((UINTN)LShiftU64(1, NvmeControllerInfo->IdentifyData->MDTS) * NvmeControllerInfo->MemoryPageSizeMin))){
            MaxTransferBufferSize = (UINTN)LShiftU64(1, NvmeControllerInfo->IdentifyData->MDTS) * NvmeControllerInfo->MemoryPageSizeMin;
        }
        
        TransferBuffer = (EFI_PHYSICAL_ADDRESS) (UINTN) Buffer;
        
        (**gPeiServices).SetMem ( NvmeCmdWrapper,
                                  sizeof(NVME_COMMAND_WRAPPER),
                                  0 );

        LBACountInOneTransfer = 0;
        PRP2TransferSize = 0;
        
        NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64) TransferBuffer;
        PRP1TransferSize = NvmeControllerInfo->MemoryPageSize - 
                            (TransferBuffer & ((UINTN)(NvmeControllerInfo->MemoryPageSize) - 1));
        
        // If all data can be transferred using only PRP1 then do that.
        if (PRP1TransferSize >= MaxTransferBufferSize) {
            PRP1TransferSize = MaxTransferBufferSize;
        }
        
        // Check Transfer size and use PRP2 if needed
        if (MaxTransferBufferSize - PRP1TransferSize) {

            // Do we need either a PRP2 pointer or a List
            if (MaxTransferBufferSize - PRP1TransferSize <= NvmeControllerInfo->MemoryPageSize) {
                NvmeCmdWrapper->NvmCmd.PRP2 = NvmeCmdWrapper->NvmCmd.PRP1 + PRP1TransferSize;
                PRP2TransferSize = MaxTransferBufferSize - PRP1TransferSize;
            } else {
                  // We need PRP2 List
                  Status = ProgramPRP2List (NvmeDeviceInfo->ActiveNameSpaceData.PRP2List, NvmeControllerInfo->MemoryPageSize, \
                                        (UINTN)TransferBuffer + PRP1TransferSize, \
                                        MaxTransferBufferSize -  PRP1TransferSize, &PRP2TransferSize
                                        );
                
                  if (EFI_ERROR(Status)) {
                    break;
                }
                
                NvmeCmdWrapper->NvmCmd.PRP2 = (UINT64) (UINTN) NvmeDeviceInfo->ActiveNameSpaceData.PRP2List;
            }
        }
        
        LBACountInOneTransfer = (UINT32)((PRP1TransferSize + PRP2TransferSize) / MediaInfo.BlockSize);
        
        // Build NVME command
        NvmeCmdWrapper->NvmCmd.CMD0.Opcode = ReadWriteOpCode;
        NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
        NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
        NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeControllerInfo->CommandIdentifierQueue1;
        NvmeCmdWrapper->NvmCmd.NSID = NvmeDeviceInfo->ActiveNameSpaceID;
        
        NvmeCmdWrapper->NvmCmd.MPTR = 0;
        NvmeCmdWrapper->NvmCmd.CDW10 = (UINT32)LBA;
        NvmeCmdWrapper->NvmCmd.CDW11 = (UINT32) RShiftU64 (LBA, 32);
        NvmeCmdWrapper->NvmCmd.CDW12 = 0x80000000 + (LBACountInOneTransfer - 1);
        NvmeCmdWrapper->NvmCmd.CDW13 = 0;
        NvmeCmdWrapper->NvmCmd.CDW14 = 0;
            
        NvmeCmdWrapper->AdminOrNVMCmdSet = FALSE;
        NvmeCmdWrapper->SQIdentifier = NvmeControllerInfo->NVMQueueNumber;
        NvmeCmdWrapper->CmdTimeOut = 1000;

        Status = ExecuteNvmeCmd (NvmeControllerInfo, NvmeCmdWrapper, &CompletionData);

        if (EFI_ERROR(Status)) {
            break;
        }

        // Remaining Bytes to be transferred
        MaxTransferBufferSize -= (LBACountInOneTransfer * MediaInfo.BlockSize);

        // Update LBA # for next transfer if needed        
        LBA += LBACountInOneTransfer;
        
        // Adjust the Buffer address
        Buffer =(VOID*) ((UINTN) Buffer + (LBACountInOneTransfer * MediaInfo.BlockSize));
        BufferSize -= (LBACountInOneTransfer * MediaInfo.BlockSize);

        // if Buffer read for MaxTransferBufferSize is not completed fully, read for the remaining size
        if (MaxTransferBufferSize == 0) {
            MaxTransferBufferSize = BufferSize;
        }
    } while(MaxTransferBufferSize);

    return Status;
}

/**
    Programms PRP2 list

    @param  PRP2List
    @param  PageSize
    @param  BufferAddress
    @param  BufferSize
    @param  PRP2TransferSize

    @retval EFI_STATUS
**/
EFI_STATUS
ProgramPRP2List (
    IN  UINT64  *PRP2List, 
    IN  UINT32  PageSize, 
    IN  UINTN   BufferAddress,
    IN  UINTN   BufferSize,
    IN  UINTN   *PRP2TransferSize
)
{
    
    UINTN    TotalNumberOfEntries = PageSize / 8;        // Each entry 64 bytes long
            
    *PRP2TransferSize = 0;
    
    do {
        
        *PRP2List++ = BufferAddress;
        if (BufferSize >= PageSize) {
            *PRP2TransferSize += PageSize;
            BufferAddress += PageSize;
            BufferSize -= PageSize;
        } else {
              *PRP2TransferSize = *PRP2TransferSize + (UINT32)BufferSize;
              BufferAddress += BufferSize;
              BufferSize = 0;
        }
        
    } while (--TotalNumberOfEntries && (BufferSize > 0));
    
    return  EFI_SUCCESS;
}

/**
    Function initializes the Number of queues for the Nvme controller

    @param  NvmeControllerInfo

    @retval VOID
**/
EFI_STATUS
SetNumberOfQueues (
    NVME_RECOVERY_CONTROLLER_INFO *NvmeControllerInfo
)
{
    EFI_STATUS              Status;
    NVME_COMMAND_WRAPPER    *NvmeCmdWrapper = NvmeControllerInfo->NvmeCmdWrapper;
    COMPLETION_QUEUE_ENTRY  CompletionData;
    
    (**gPeiServices).SetMem ( NvmeCmdWrapper,
                              sizeof(NVME_COMMAND_WRAPPER),
                              0 );

    // Build NVME command for Set Number of queue to 1
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = SET_FEATURES;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeControllerInfo->CommandIdentifierAdmin;

    NvmeCmdWrapper->NvmCmd.CDW10 = 0x7;
    NvmeCmdWrapper->NvmCmd.CDW11 = 0x10001;
    
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;           // Queue 0 for Admin cmds
    NvmeCmdWrapper->CmdTimeOut = 1000;

    Status = ExecuteNvmeCmd ( NvmeControllerInfo, NvmeCmdWrapper, &CompletionData );
    return Status;
}

/**
    Prints NvmeCmdWrapper info passed to ExecuteNvmeCmd function

    @param NvmeCmdWrapper

    @retval VOID
**/
VOID
PrintExecuteNvmeCmdData (
     NVME_COMMAND_WRAPPER    *NvmeCmdWrapper
)
{
#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n *****  ExecuteNvmeCmd  ***** \n")); 
    PEI_TRACE((-1, gPeiServices, "\n Opcode                                  : %16lx", NvmeCmdWrapper->NvmCmd.CMD0.Opcode)); 
    PEI_TRACE((-1, gPeiServices, "\n CommandIdentifier                       : %16lx",NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier ));
    PEI_TRACE((-1, gPeiServices, "\n NSID                                    : %08X",NvmeCmdWrapper->NvmCmd.NSID));
    PEI_TRACE((-1, gPeiServices, "\n CmdTimeOut                              : %08X", NvmeCmdWrapper->CmdTimeOut));
    PEI_TRACE((-1, gPeiServices, "\n AdminOrNVMCmdSet                        : %08X",NvmeCmdWrapper->AdminOrNVMCmdSet));
    PEI_TRACE((-1, gPeiServices, "\n PRP1                                    : %016lX",NvmeCmdWrapper->NvmCmd.PRP1));
    PEI_TRACE((-1, gPeiServices, "\n PRP2                                    : %016lX",NvmeCmdWrapper->NvmCmd.PRP2));
    PEI_TRACE((-1, gPeiServices, "\n CDW10                                   : %08X",NvmeCmdWrapper->NvmCmd.CDW10));
    PEI_TRACE((-1, gPeiServices, "\n CDW11                                   : %08X",NvmeCmdWrapper->NvmCmd.CDW11));
    PEI_TRACE((-1, gPeiServices, "\n CDW12                                   : %08X",NvmeCmdWrapper->NvmCmd.CDW12));
    PEI_TRACE((-1, gPeiServices, "\n CDW13                                   : %08X",NvmeCmdWrapper->NvmCmd.CDW13));
    PEI_TRACE((-1, gPeiServices, "\n CDW14                                   : %08X",NvmeCmdWrapper->NvmCmd.CDW14));
    PEI_TRACE((-1, gPeiServices, "\n CDW15                                   : %08X",NvmeCmdWrapper->NvmCmd.CDW15));
#endif
}

/**
    Prints Controller capabilities

    @param NvmeControllerInfo

    @retval VOID
**/
VOID
PrintNvmeCapability (
    NVME_RECOVERY_CONTROLLER_INFO *NvmeControllerInfo
)
{

#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n NvmeBarAddress: %x\n",NvmeControllerInfo->BarAddress));
    PEI_TRACE((-1, gPeiServices, "\n Controller Capabilities Reg Value       : %16lx\n",NvmeControllerInfo->RawNvmeCapability));
    PEI_TRACE((-1, gPeiServices, "\n Max. Queue Entrys Supported             : %08X\n",NvmeControllerInfo->MaxQueueEntrySupported));
    PEI_TRACE((-1, gPeiServices, "\n Contiguous Queue Required               : %08X\n",NvmeControllerInfo->ContiguousQueueRequired));
    PEI_TRACE((-1, gPeiServices, "\n Arbitration Mode Supported              : %08X\n",NvmeControllerInfo->ArbitrationMechanismSupport));
    PEI_TRACE((-1, gPeiServices, "\n TimeOut in 500msec unit                 : %08X\n",NvmeControllerInfo->TimeOut));
    PEI_TRACE((-1, gPeiServices, "\n Doorbell Stride                         : %08X\n",NvmeControllerInfo->DoorBellStride));
    PEI_TRACE((-1, gPeiServices, "\n NVM Subsystem Reset Support             : %08X\n",NvmeControllerInfo->NVMResetSupport));
    PEI_TRACE((-1, gPeiServices, "\n Command Sets Supported                  : %08X\n",NvmeControllerInfo->CmdSetsSupported));
    PEI_TRACE((-1, gPeiServices, "\n Memory Page Size Min.in Bytes           : %08X\n",NvmeControllerInfo->MemoryPageSizeMin));
    PEI_TRACE((-1, gPeiServices, "\n Memory Page Size Max.in Bytes           : %08X\n",NvmeControllerInfo->MemoryPageSizeMax));
#endif
    return;
}

/**
    Initialize Nvme controller and ADMIN submission and Completion 
    queues

    @param  NvmeControllerInfo 
    @param  PciDeviceInfo 

    @retval EFI_STATUS

**/
EFI_STATUS
InitializeNvmeController (
    NVME_RECOVERY_CONTROLLER_INFO *NvmeControllerInfo,
    NVME_PCI_DEVICE_INFO          *PciDeviceInfo
)
{
    EFI_STATUS                    Status;
    UINT64                        ControllerCapabilities;
    UINTN                         Delay;
    UINTN                         AllocatePageSize;
    UINT32                        ProgramCC;
    EFI_PHYSICAL_ADDRESS          AdminQueueAddress = 0;
    UINT16                        CmdReg;
    
    NvmeControllerInfo->BarAddress = PciDeviceInfo->BaseAddress;
    NvmeControllerInfo->BusNumber = PciDeviceInfo->BusNumber;
    NvmeControllerInfo->Device = PciDeviceInfo->Device;
    NvmeControllerInfo->Function = PciDeviceInfo->Function;

    // Enable Nvme Device
    gPciCfg->Read(gPeiServices, \
                  gPciCfg, \
                  EfiPeiPciCfgWidthUint16,\
                  PEI_PCI_CFG_ADDRESS(NvmeControllerInfo->BusNumber, NvmeControllerInfo->Device,
                          NvmeControllerInfo->Function,PCI_CMD),\
                  &CmdReg);
    CmdReg |= CMD_ENABLE_MEM;

    gPciCfg->Write( gPeiServices, \
                    gPciCfg, \
                    EfiPeiPciCfgWidthUint16,\
                    PEI_PCI_CFG_ADDRESS(NvmeControllerInfo->BusNumber, NvmeControllerInfo->Device,
                           NvmeControllerInfo->Function,PCI_CMD),\
                    &CmdReg);

    ControllerCapabilities = LShiftU64(CONTROLLER_REG32(NvmeControllerInfo->BarAddress, 4), 32) + 
                                        CONTROLLER_REG32(NvmeControllerInfo->BarAddress, 0);
    NvmeControllerInfo->RawNvmeCapability = ControllerCapabilities;
    
    NvmeControllerInfo->MaxQueueEntrySupported = (ControllerCapabilities & 0xFFFF) + 1;
    NvmeControllerInfo->ContiguousQueueRequired = (BOOLEAN)(RShiftU64((ControllerCapabilities & 0x10000), 16));
    NvmeControllerInfo->ArbitrationMechanismSupport = (BOOLEAN)(RShiftU64((ControllerCapabilities & 0x60000), 17));
    NvmeControllerInfo->TimeOut = (UINT8)RShiftU64((ControllerCapabilities & 0xFF000000), 24);  // 500msec units
    NvmeControllerInfo->DoorBellStride = (UINT8)(RShiftU64((ControllerCapabilities & 0xF00000000), 32));
    NvmeControllerInfo->NVMResetSupport = (BOOLEAN) (RShiftU64((ControllerCapabilities & 0x1000000000), 36));
    NvmeControllerInfo->CmdSetsSupported = (UINT8)(RShiftU64((ControllerCapabilities & 0x1FE000000000), 37));
    NvmeControllerInfo->MemoryPageSizeMin = (UINT32) LShiftU64(1, (UINTN)(RShiftU64((ControllerCapabilities & 0xF000000000000), 48) + 12));  // In Bytes
    NvmeControllerInfo->MemoryPageSizeMax = (UINT32) LShiftU64(1, (UINTN)(RShiftU64((ControllerCapabilities & 0xF0000000000000), 52) + 12)); // In Bytes

    PrintNvmeCapability(NvmeControllerInfo);
    
    // Is NVM command set supported
    if (!(NvmeControllerInfo->CmdSetsSupported & 0x1)) {
        return EFI_UNSUPPORTED;
    }
    
    // Check if the controller is already running. If yes stop it.
    Delay = NvmeControllerInfo->TimeOut * 500;

    // Check if the controller is still in shutdown process occurring state
    do {
     
        if ((CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CSTS) & 0xC) != 4) {
            break;
        }
        gStallPpi->Stall((CONST EFI_PEI_SERVICES**)gPeiServices, (CONST EFI_PEI_STALL_PPI*)gStallPpi, 1000 ); // 1msec delay
        
    }while (--Delay);

    if (!Delay) {
        return EFI_DEVICE_ERROR;
    }
    
    Delay = NvmeControllerInfo->TimeOut * 500;
    if (CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CC) & 0x1) {
        
        //  Disable Enable bit
        CONTROLLER_REG32_AND (NvmeControllerInfo->BarAddress, Offset_CC, ~0x01);
        do {
            if (!(CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CSTS) & 0x1)) {
                break;
            }
            gStallPpi->Stall((CONST EFI_PEI_SERVICES**)gPeiServices, (CONST EFI_PEI_STALL_PPI*)gStallPpi, 1000 ); // 1msec delay
        } while (--Delay);
    }
    
    if (!Delay) {
        return EFI_DEVICE_ERROR;
    }
    
    // Initialize Controller configuration register. 
    // Select Round Robin and NVM Command Set (both values are zero)
    NvmeControllerInfo->MemoryPageSize = NvmeControllerInfo->MemoryPageSizeMin;
    
    ProgramCC = (UINT32) LShiftU64((UINTN)RShiftU64(NvmeControllerInfo->MemoryPageSize, 13), 7);

    // Initialize with default value. Later it can be modified
    ProgramCC |= (6 << 16);     // I/O Submission Queue Entry Size
    ProgramCC |= (4 << 20);     // I/O Completion Queue Entry Size
    
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CC) =  ProgramCC;

    // Allocate memory for Admin Queue. Each entry is 64bytes long and queue should be aligned on MemoryPageSize
    AllocatePageSize = NvmeControllerInfo->MemoryPageSize + 
                        ADMIN_QUEUE_SIZE * sizeof (NVME_ADMIN_COMMAND);

    Status = (*gPeiServices)->AllocatePages ( (const EFI_PEI_SERVICES **)gPeiServices, \
                                              EfiRuntimeServicesData, \
                                              EFI_SIZE_TO_PAGES(AllocatePageSize), \
                                              (EFI_PHYSICAL_ADDRESS*)&(AdminQueueAddress) );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    NvmeControllerInfo->AdminSubmissionUnAligned = AdminQueueAddress;
    NvmeControllerInfo->AdminSubmissionUnAlignedSize = EFI_SIZE_TO_PAGES(AllocatePageSize);
    NvmeControllerInfo->AdminSubmissionQueue = (AdminQueueAddress & ~(NvmeControllerInfo->MemoryPageSize - 1)) + 
                                                    NvmeControllerInfo->MemoryPageSize;

    Status = (*gPeiServices)->AllocatePages ( (const EFI_PEI_SERVICES **)gPeiServices, \
                                              EfiRuntimeServicesData, \
                                              EFI_SIZE_TO_PAGES(AllocatePageSize), \
                                              (EFI_PHYSICAL_ADDRESS*)&(AdminQueueAddress) );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    NvmeControllerInfo->AdminCompletionUnAligned = AdminQueueAddress;
    NvmeControllerInfo->AdminCompletionUnAlignedSize = EFI_SIZE_TO_PAGES(AllocatePageSize);
    NvmeControllerInfo->AdminCompletionQueue = (AdminQueueAddress & ~(NvmeControllerInfo->MemoryPageSize - 1)) + 
                                                    NvmeControllerInfo->MemoryPageSize;
    
#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n AdminSubmissionQueue Base %x",NvmeControllerInfo->AdminSubmissionQueue));
    PEI_TRACE((-1, gPeiServices, "\n AdminCompletionQueue Base %x",NvmeControllerInfo->AdminCompletionQueue));
#endif
    
    NvmeControllerInfo->AdminSubmissionQueueSize = ADMIN_QUEUE_SIZE;
    NvmeControllerInfo->AdminCompletionQueueSize = ADMIN_QUEUE_SIZE;
    
    (**gPeiServices).SetMem ( (VOID*)NvmeControllerInfo->AdminSubmissionQueue,
                              NvmeControllerInfo->AdminSubmissionQueueSize*sizeof(NVME_ADMIN_COMMAND),
                              0 );
    
    (**gPeiServices).SetMem ( (VOID*)NvmeControllerInfo->AdminCompletionQueue,
                              NvmeControllerInfo->AdminCompletionQueueSize*sizeof (NVME_ADMIN_COMMAND),
                              0 );
    
    // Program Admin Queue Size and Base Address
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Aqa) =  
                                                ((NvmeControllerInfo->AdminCompletionQueueSize - 1) << 16) + 
                                                (NvmeControllerInfo->AdminSubmissionQueueSize - 1);
    
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Asq) = 
                                                (UINT32) NvmeControllerInfo->AdminSubmissionQueue;
    
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Asq + 4) =  
                                                (UINT32) RShiftU64(NvmeControllerInfo->AdminSubmissionQueue, 32);
    
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Acq) =  
                                                (UINT32)NvmeControllerInfo->AdminCompletionQueue;
    
    CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_Acq + 4) =  
                                                (UINT32) RShiftU64(NvmeControllerInfo->AdminCompletionQueue, 32);

    NvmeControllerInfo->AdminPhaseTag = FALSE;

    Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                             sizeof(NVME_COMMAND_WRAPPER),
                                             (VOID**)&(NvmeControllerInfo->NvmeCmdWrapper) );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    // Enable Controller
    CONTROLLER_REG32_OR(NvmeControllerInfo->BarAddress, Offset_CC, 0x1);
    
    // Wait for the controller to get ready
    // Check if the controller is already running. If yes stop it.
    Delay = NvmeControllerInfo->TimeOut * 500;
    do {
        if ((CONTROLLER_REG32(NvmeControllerInfo->BarAddress, Offset_CSTS) & 0x1)) {
            break;
        }
        gStallPpi->Stall((CONST EFI_PEI_SERVICES**)gPeiServices, (CONST EFI_PEI_STALL_PPI*)gStallPpi, 1000 ); // 1msec delay
    } while (--Delay);
        
    if (!Delay) {
        return EFI_DEVICE_ERROR;
    }
    Status = SetNumberOfQueues (NvmeControllerInfo);
    return Status;
}

/**
    Prints Identfiy data information for the Nvme controller/Namespace based on
    the ControllerNameSpaceStructure input value.

    @param  NvmeControllerInfo 
    @param  PciDeviceInfo 

    @retval EFI_STATUS

**/
VOID
PrintIdentifyDataStructure (
    IN  UINT8   *IdentifyData, 
    IN  UINT8   ControllerNameSpaceStructure
)
{
    IDENTIFY_CONTROLLER_DATA    *IdentifyControllerData = (IDENTIFY_CONTROLLER_DATA *)IdentifyData;
    IDENTIFY_NAMESPACE_DATA     *IdentifyNameSpaceData = (IDENTIFY_NAMESPACE_DATA *)IdentifyData;
    UINT32                      *ActiveNameSapceID = (UINT32 *)IdentifyData;
    UINT32                      i;
    UINT64                      FirmwareVersion;
    
    switch (ControllerNameSpaceStructure) {
    
        case 1:
            FirmwareVersion = *(UINT64 *)(IdentifyControllerData->FirmwareRevision);
            
            PEI_TRACE((-1, gPeiServices, "\n Identify Controller Data Structure\n " ));

            PEI_TRACE((-1,gPeiServices,"Vendor ID            : %x\n", IdentifyControllerData->VID));
            PEI_TRACE((-1,gPeiServices,"SubSystem Vendor ID                       : %x\n", IdentifyControllerData->SSVID));
            PEI_TRACE((-1,gPeiServices,"Firmware Version                          : %x\n", FirmwareVersion));
            PEI_TRACE((-1,gPeiServices,"NameSpace Sharing Capability              : %x\n", IdentifyControllerData->CMIC));
            PEI_TRACE((-1,gPeiServices,"Max. Data Transfer Size                   : %x\n", IdentifyControllerData->MDTS));
            PEI_TRACE((-1,gPeiServices,"Controller ID                             : %08X\n", IdentifyControllerData->ControllerID));
            PEI_TRACE((-1,gPeiServices,"Optional Admin Cmd Support                : %08X\n", IdentifyControllerData->OACS));
            PEI_TRACE((-1,gPeiServices,"Abort Command Limit                       : %08X\n", IdentifyControllerData->ACL));
            PEI_TRACE((-1,gPeiServices,"Asyn. Event Request Limit                 : %08X\n", IdentifyControllerData->AERL));
            PEI_TRACE((-1,gPeiServices,"Firmware Updates                          : %08X\n", IdentifyControllerData->FRMW));
            PEI_TRACE((-1,gPeiServices,"Log Page Attribute                        : %08X\n", IdentifyControllerData->LPA));
            PEI_TRACE((-1,gPeiServices,"# of Power state supported                : %08X\n", IdentifyControllerData->NPSS));
            PEI_TRACE((-1,gPeiServices,"Admin Vendor Specific cmd                 : %08X\n", IdentifyControllerData->AVSCC));
            PEI_TRACE((-1,gPeiServices,"Autonomous Power state attrib             : %08X\n", IdentifyControllerData->APSTA));
            PEI_TRACE((-1,gPeiServices,"Submission queue Entry Size               : %08X\n", IdentifyControllerData->SQES));
            PEI_TRACE((-1,gPeiServices,"Completion queue Entry Size               : %08X\n", IdentifyControllerData->CQES));
            PEI_TRACE((-1,gPeiServices,"Number of NameSpace                       : %08X\n", IdentifyControllerData->NN));
            PEI_TRACE((-1,gPeiServices,"Optional NVM Command Support              : %08X\n", IdentifyControllerData->ONCS));
            PEI_TRACE((-1,gPeiServices,"Fused Operation Support                   : %08X\n", IdentifyControllerData->FUSES));
            PEI_TRACE((-1,gPeiServices,"Format NVM Attribute                      : %08X\n", IdentifyControllerData->FNA));
            PEI_TRACE((-1,gPeiServices,"Volatile Write Cache                      : %08X\n", IdentifyControllerData->VWC));
            PEI_TRACE((-1,gPeiServices,"Atomic Write Unit Normal                  : %08X\n", IdentifyControllerData->AWUN));
            PEI_TRACE((-1,gPeiServices,"Atomic Write Unit Power Fail              : %08X\n", IdentifyControllerData->AWUPF));
            PEI_TRACE((-1,gPeiServices,"NVM VS CMD Config                         : %08X\n", IdentifyControllerData->NVSCC));
            PEI_TRACE((-1,gPeiServices,"Atomic Compare & Write Unit               : %08X\n", IdentifyControllerData->ACWU));
            PEI_TRACE((-1,gPeiServices,"SGL Support                               : %08X\n", IdentifyControllerData->SGLS));
            break;
            
        case 0:
            PEI_TRACE((-1,gPeiServices,"\nIdentify NameSpace Data Structure\n"));
            PEI_TRACE((-1,gPeiServices, "NameSpace Size                             : %08X\n", IdentifyNameSpaceData->NSIZE));
            PEI_TRACE((-1,gPeiServices, "NameSpace Capacity                         : %08X\n", IdentifyNameSpaceData->NCAP));
            PEI_TRACE((-1,gPeiServices, "NameSpace Utilization                      : %08X\n", IdentifyNameSpaceData->NUSE));
            PEI_TRACE((-1,gPeiServices,"NameSpace Features                         : %08X\n", IdentifyNameSpaceData->NSFEAT));
            PEI_TRACE((-1,gPeiServices,"No. of LBA Formats (0 Based)               : %08X\n", IdentifyNameSpaceData->NLBAF));
            PEI_TRACE((-1,gPeiServices, "Formatted LBA Size                         : %08X\n", IdentifyNameSpaceData->FLBAS));
            PEI_TRACE((-1,gPeiServices, "MetaData Capabilities                      : %08X\n", IdentifyNameSpaceData->MC));
            PEI_TRACE((-1,gPeiServices, "End-to-end data Protection cap             : %08X\n", IdentifyNameSpaceData->DPC));
            PEI_TRACE((-1,gPeiServices, "End-to-end data Protection settings        : %08X\n", IdentifyNameSpaceData->DPS));
            PEI_TRACE((-1,gPeiServices,"NameSpace Sharing                          : %08X\n", IdentifyNameSpaceData->NMIC));
            PEI_TRACE((-1,gPeiServices,"Reservation Capabilities                   : %08X\n", IdentifyNameSpaceData->RESCAP));
            PEI_TRACE((-1,gPeiServices, "IEEE Extended Unique Identifier            : %016lx\n", IdentifyNameSpaceData->EUI64));
            for (i=0; i<16; i++){
                PEI_TRACE((-1,gPeiServices,"LBA Format %02X Support                  : %08X\n", i, *(UINT32 *)&(IdentifyNameSpaceData->LBAF[i])));
            }
            break;
            
        case 2:
            PEI_TRACE((-1,gPeiServices,"\nActive NameSpace IDs\n"));
            for (i=0; i<1024 && ActiveNameSapceID[i]; i++) {
                PEI_TRACE((-1,gPeiServices,"    %08X\n", i, ActiveNameSapceID[i]));
            }
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
