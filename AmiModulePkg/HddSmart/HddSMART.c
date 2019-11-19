//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file HddSMART.c
    HDD SMART Support for Ide, AHCI and Nvme Interface

**/

//---------------------------------------------------------------------------

#include "HddSMART.h"
#include <Setup.h>
#include <SmartfailureFunctionList.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include "Protocol/NvmExpressPassthru.h"
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0))
#include "Nvme/NvmeController.h"
#include "Protocol/AmiNvmeController.h"
#include "Nvme/NvmePassthru.h"
#endif


UINT8       gNvmeRaidController = FALSE;
UINT16      gUnicodeStr[41];
VOID        *gSmartFailureReg = NULL;
UINT32      gNameSpaceIdFromRaid = 0xFFFFFFFF;

//---------------------------------------------------------------------------

EFI_GUID gSetupGuid = SETUP_GUID;
extern EFI_GUID gBdsAllDriversConnectedProtocolGuid;

EFI_HII_HANDLE               HiiPostHandle;
AMI_POST_MANAGER_PROTOCOL    *pAmiPostMgr;
SETUP_DATA                   gSetupData;


// Generic function in this file that will call function specified via elink
VOID 
ReportSmartFailure (
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType,
    VOID                            *Interface
);

EFI_STATUS
GetNvmeSmartHealthData (
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL   *NvmePassThruProtocol,
    UINT8                                LID,
    UINT8                                *Buffer,
    UINT32                               BufferLength
);

//
// ELINK defined function that will be called.  Function name 'OemReportSmartFailureFunction'
//   is #defined via the makefile /D to be the ELINK function name
//
VOID 
OemReportSmartFailureFunction (
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType,
    VOID                            *Interface
);

VOID
EFIAPI
ReportSmartFailureCallBack (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

/**
    Save NVMe model number.

    @param NvmeModelNumber

    @retval EFI_SUCCESS

**/
EFI_STATUS
GetNvmeUnicodeStringName (
    UINT8  *NvmeModelNumber )
{
    UINTN    NvmeModelNumberSize=40,i;

    for(i=0; i<NvmeModelNumberSize; i++) {
        gUnicodeStr[i] = NvmeModelNumber[i];
    }
    gUnicodeStr[i] = 0;

    return EFI_SUCCESS;
}

/**
    Routine to send Identify command to NvmeController and Namespace of the Nvme Drive

    @param ControllerNamespace,
    @param NameSpaceID
    @param Buffer
    @param Length

    @retval EFI_STATUS
**/
EFI_STATUS
IdentifyDataCommand (
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *NvmePassthruProtocol,
  IN UINT8                              ControllerNamespace,
  IN UINT32                             NameSpaceID,
  IN OUT VOID                           *Buffer,
  IN OUT UINTN                          *Length )
{
    EFI_STATUS    Status;
    VOID          *TransferBuffer=NULL,*TempTransferBuffer;
    UINT32        TransferLength = NVME_IDENTIFY_COMMAND_SIZE;

    EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET    NvmeCommandPacket;
    EFI_NVM_EXPRESS_COMMAND                     NvmeCmd;
    EFI_NVM_EXPRESS_COMPLETION                  NvmeCompletion;
    
    // Clear the Global NvmeCommandPacket structure to Zero
    pBS->SetMem( &NvmeCommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET), 0 );
    
    // Clear Global Nvm Express Admin Command to Zero
    pBS->SetMem( &NvmeCmd, sizeof(EFI_NVM_EXPRESS_COMMAND), 0);

    // Clear Global Nvm Express completion structure to Zero
    pBS->SetMem( &NvmeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION), 0);

    // Allocate memory for Transfer Buffer to get Identify data structure
    Status = pBS->AllocatePool( EfiBootServicesData,
                                (UINTN)(TransferLength + LShiftU64(1, NvmePassthruProtocol->Mode->IoAlign)),  // Length + Extra Memory for memory alligning
                                (VOID**)&TransferBuffer);
    
    if( EFI_ERROR(Status) ) {
        return Status;
    }
    pBS->SetMem(TransferBuffer, (UINTN)(TransferLength + LShiftU64(1,NvmePassthruProtocol->Mode->IoAlign)) , 0);

    TempTransferBuffer = TransferBuffer;
    // Allign TransferBuffer base address based on NvmePassthruProtocol->Mode->IoAlign
    TransferBuffer = (VOID*)(((UINTN)TransferBuffer & ~(LShiftU64(1,NvmePassthruProtocol->Mode->IoAlign)-1)) +
                           LShiftU64(1,NvmePassthruProtocol->Mode->IoAlign) );

    
    // Fill NvmeCommandPacket to get Identify data
    NvmeCmd.Nsid = 0;    // NSID=0 for Nvme Controller identify data
    NvmeCmd.Cdw0.Opcode = IDENTIFY;
    NvmeCmd.Cdw0.FusedOperation = 0;
    NvmeCmd.Cdw10 = ControllerNamespace;
    NvmeCmd.Flags |= CDW10_VALID;
    
    NvmeCommandPacket.CommandTimeout = NVME_COMMAND_TIMEOUT * 10000000;    // 1Sec
    NvmeCommandPacket.NvmeCmd = &NvmeCmd;
    NvmeCommandPacket.QueueType = 0;            // 0 - Admin Command type, 1- I/O command type 
    NvmeCommandPacket.NvmeCompletion = &NvmeCompletion;
    NvmeCommandPacket.TransferBuffer = TransferBuffer;
    NvmeCommandPacket.TransferLength = TransferLength;

    // Send Command through Passthru API
    Status = NvmePassthruProtocol->PassThru( NvmePassthruProtocol,
                                             NameSpaceID,
                                             &NvmeCommandPacket,
                                             NULL
                                             );
    
    if( EFI_ERROR(Status) ) {
        return Status;
    }
    
    if( Buffer ) {
        if( *Length >= TransferLength ) {
            pBS->CopyMem( Buffer, NvmeCommandPacket.TransferBuffer, TransferLength );
            *Length = NvmeCommandPacket.TransferLength;
        } else {
            Status = EFI_BAD_BUFFER_SIZE;
        }
    }

    pBS->FreePool( TempTransferBuffer );
    return Status;
}

/**
    NVMe Pass Thru event to check Smart Error for NVMe in Raid Mode.

    @param  EFI_EVENT          Event     Image handle.
    @param  VOID               Context   Pointer to the EFI system table.

    @retval VOID

    @note

**/
VOID
ReportRaidModeSmartFailureCallBack (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context )
{
    EFI_STATUS                          Status;
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    AMI_NVME_CONTROLLER_PROTOCOL        *NvmeController = NULL; 
#endif
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  *NvmePassthruProtocol;
    NVME_SMART_HEALTH_INFO              NvmeSmartHealthInfo;
    UINTN                               SetupDataSize = sizeof(SETUP_DATA);
    UINTN                               HandleCount = 0;
    EFI_HANDLE                          *HandleBuffer  = NULL;
    UINT32                              Index;
    
    // Get NvmePassthruProtocol Handles
    Status = pBS->LocateHandleBuffer(ByProtocol,
                                    &gEfiNvmExpressPassThruProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &HandleBuffer);

    if (EFI_ERROR(Status)) {
        return ;
    }
    for(Index = 0; Index < HandleCount; Index++) {
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
        // Check gEfiNvmExpressPassThruProtocolGuid is produced by Raid or AMI NVMe Driver 
        Status = pBS->HandleProtocol ( HandleBuffer[Index], 
                                       &gAmiNvmeControllerProtocolGuid, 
                                      (VOID**)&NvmeController);
    
        if (!(EFI_ERROR(Status))) {
            continue;
        }
#endif
        Status = pBS->HandleProtocol( HandleBuffer[Index], 
                                      &gEfiNvmExpressPassThruProtocolGuid, 
                                      (VOID**)&NvmePassthruProtocol );
                                          
        if (EFI_ERROR(Status)) {
            continue;
        }
 
        pBS->SetMem(&NvmeSmartHealthInfo, sizeof(NVME_SMART_HEALTH_INFO),0);
        gNameSpaceIdFromRaid = 0xFFFFFFFF; 
        while (TRUE) {
            
            // Get gNameSpaceIdFromRaid for the usage of GetNvmeSmartHealthData()
            Status = NvmePassthruProtocol->GetNextNamespace (NvmePassthruProtocol, &gNameSpaceIdFromRaid);
            if (EFI_ERROR (Status)) {
                break;
            }

            // Get SMART Self test setup option
            Status = pRS->GetVariable ( L"Setup", 
                                        &gSetupGuid, 
                                        NULL,
                                        &SetupDataSize, 
                                        &gSetupData);
    
            if (EFI_ERROR(Status)) {
                // If Setup Variable is not found, set Default values
                gSetupData.SmartSelfTest = DEFAULT_SMART_SELFTEST;
            }
    
            if (gSetupData.SmartSelfTest) {
                gNvmeRaidController = TRUE;
                //  Send Smart Health command
                Status = GetNvmeSmartHealthData ( NvmePassthruProtocol,
                                                  0x2,
                                                  (UINT8*)&NvmeSmartHealthInfo,
                                                  sizeof(NVME_SMART_HEALTH_INFO)>>2 );// Send Number of DWORD as Bufferlength
    
                if(EFI_ERROR(Status)) {
                    continue;
                }
                gNvmeRaidController = FALSE;
                // Check for Nvme Smart Error
            if( NvmeSmartHealthInfo.CriticalWarning & NVME_SMART_HEALTH_IDENTIFY_MASK ) {
                    ERROR_CODE( DXE_IDE_SMART_ERROR, EFI_ERROR_MAJOR );
                    if(pST->ConIn != NULL) {
                        ReportSmartFailure(RaidStorageInterfaceNvme, NvmePassthruProtocol);
                    } 
                }
            }
        
        // clear NvmeSmartHealthInfo for next loop
        pBS->SetMem(&NvmeSmartHealthInfo, sizeof(NVME_SMART_HEALTH_INFO),0);
    
        }// end of while
    } // end of for
    return ;
}

/**
    This function is the entry point for IDE SMART Driver.

    @param  EFI_HANDLE          ImageHandle     Image handle.
    @param  EFI_SYSTEM_TABLE    *SystemTable    Pointer to the EFI system table.

    @retval EFI_STATUS
    @retval EFI_SUCCESS         When everything is going on fine!

    @note   Entry Points are used to locate or install protocol interfaces and
            notification events.

**/

EFI_STATUS 
EFIAPI HddSmartEntryPoint (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_HANDLE    Handle = NULL;
    EFI_STATUS    Status;
    EFI_EVENT     Event;
    
    InitAmiLib( ImageHandle, SystemTable );

    Status = LoadStrings( ImageHandle, &HiiPostHandle );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(AMI_HDD_SMART_INIT_PROTOCOL),
                                (VOID**)&gHddSmartInitProtocol );
    ASSERT_EFI_ERROR(Status);
    if ( EFI_ERROR( Status )) {
        return Status;
    }

    gHddSmartInitProtocol->InstallSmartInterface   = InstallSmartInterface;
    gHddSmartInitProtocol->UnInstallSmartInterface = UnInstallSmartInterface;
    gHddSmartInitProtocol->InitSmartSupport        = InitSmartSupport;
    gHddSmartInitProtocol->SmartDiagonasticFlag    = FALSE;
#if SMART_DIAGNOSTICS_SUPPORT
    gHddSmartInitProtocol->SmartDiagonasticFlag = TRUE;
#endif

    Status = pBS->InstallProtocolInterface(
                                &Handle,
                                &gAmiHddSmartInitProtocolGuid,
                                EFI_NATIVE_INTERFACE,
                                gHddSmartInitProtocol
                                );
    
    ASSERT_EFI_ERROR( Status );
    
    // Call back for gBdsAllDriversConnectedProtocolGuid to Handle the NVMe device 
    // in Raid mode to check Smart bad or not 
    Status = RegisterProtocolCallback( &gBdsAllDriversConnectedProtocolGuid,
                                       ReportRaidModeSmartFailureCallBack, 
                                       NULL, 
                                       &Event, 
                                       &gSmartFailureReg);
    
    ASSERT_EFI_ERROR( Status );

    return Status;
}

/**
    Routine to get Nvme Smart Health information

    @param  NvmePassThruProtocol
    @parem  LID
    @param  Buffer
    @param  BufferLength

    @retval EFI_STATUS
    @retval EFI_SUCCESS         When everything is going on fine!

    @note   As per Nvme Spec 1.2, Smart health inforamtion is idenitical for
            for the Nvme controller and each namespace, Hence Used Namespace value
            as 0xFFFFFFFF to get Nvme controller smart Health data.
            
            BufferLength - is the Number of DWORD length of Buffer
**/
EFI_STATUS
GetNvmeSmartHealthData (
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL   *NvmePassThruProtocol,
    UINT8                                LID,
    UINT8                                *Buffer,
    UINT32                               BufferLength
)
{
    EFI_STATUS    Status=EFI_SUCCESS;

    EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  NvmeCommandPacket;
    EFI_NVM_EXPRESS_COMMAND                   NvmeCmd;
    EFI_NVM_EXPRESS_COMPLETION                NvmeCompletion;
    UINT32                                    NamespaceId = 0;
    UINT8                                     *IdentifyControllerData = NULL;
    UINTN                                     Size = NVME_IDENTIFY_COMMAND_SIZE;
    
    // Clear NvmeCommandPacket structure to Zero
    pBS->SetMem( &NvmeCommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET), 0 );

    // Clear Nvm Express Admin Command to Zero
    pBS->SetMem( &NvmeCmd, sizeof(EFI_NVM_EXPRESS_COMMAND), 0);

    // Clear Nvm Express completion structure to Zero
    pBS->SetMem( &NvmeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION), 0);

    if ( gNvmeRaidController ) {
        NamespaceId = gNameSpaceIdFromRaid;
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    NVME_IDENTIFY_COMMAND_SIZE,
                                    &IdentifyControllerData);
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) {
            return Status;
        }
        // Get Identfy namespace data of the Namespace ID
        Status = IdentifyDataCommand ( NvmePassThruProtocol,
                                       1,                    // ControllerNamespace = 1
                                       NamespaceId,          
                                       (VOID*)IdentifyControllerData,
                                       &Size );
        if( EFI_ERROR(Status)) {
            pBS->FreePool(IdentifyControllerData);
            return Status;
        }
        GetNvmeUnicodeStringName(IdentifyControllerData+MODEL_NUM_OFFSET);
        pBS->FreePool(IdentifyControllerData);
        
    }
    // Fill NvmeCommandPacket to get Smart/Health Log info
    NvmeCmd.Nsid = 0xFFFFFFFF;
    NvmeCmd.Cdw0.Opcode = GET_LOG_PAGE;
    NvmeCmd.Cdw0.FusedOperation = 0;
    // Number of DWORD to get from Controller is stored at BIT16 to BIT27
    NvmeCmd.Cdw10 = BufferLength - 1;
    NvmeCmd.Cdw10 = NvmeCmd.Cdw10<<16 | LID;    // 0x02 for Smart/Health Log
    NvmeCmd.Flags |= (CDW10_VALID|CDW11_VALID|CDW12_VALID|CDW13_VALID);
    
    NvmeCommandPacket.CommandTimeout = NVME_COMMAND_TIMEOUT * 10000000;    // 1Sec
    NvmeCommandPacket.NvmeCmd = &NvmeCmd;   
    NvmeCommandPacket.QueueType = 0;
    NvmeCommandPacket.NvmeCompletion = &NvmeCompletion;
    NvmeCommandPacket.TransferBuffer = Buffer;
    NvmeCommandPacket.TransferLength = BufferLength << 2;  //Transfer length in terms of bytes

    // Send Command through Passthru API
    Status = NvmePassThruProtocol->PassThru( NvmePassThruProtocol,
                                             NamespaceId,
                                             &NvmeCommandPacket,
                                             NULL
                                             );
    return Status;
}

/**
    Initialize Smart functionality

        
    @param BusInterface 
    @param BusInterfaceType 

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
InitSmartSupport (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
)
{
    UINT8                   bData;
    EFI_STATUS              Status;
    IDENTIFY_DATA           IdentifyData = { 0 };
    DEVICE_TYPE             DeviceType = 0;
    COMMAND_STRUCTURE       CommandStructure;
    UINTN                   SetupDataSize = sizeof(SETUP_DATA);
    BOOLEAN                 SmartFailure= FALSE;
    SMART_ERROR_DEVINFO     *SmartErrorDevInfo = NULL;
    EFI_EVENT               ReportSmartFailureEvent = NULL;
    VOID                    *ReportSmartFailureRegistration = NULL;
    
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif
    
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        IdentifyData = IdeBusInterface->IdeDevice.IdentifyData;
        DeviceType   = IdeBusInterface->IdeDevice.DeviceType;
    }
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdentifyData = SataDevInterface->IdentifyData;
        DeviceType   = SataDevInterface->DeviceType;
    }
#endif

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        // Get NvmePassThruProtocol interface for Nvme Device
        BusInterface = ((AMI_NVME_CONTROLLER_PROTOCOL*)BusInterface)->EfiNvmExpressPassThru;
    }
#endif

    // Check for ATA device
    if  ( BusInterfaceType <= AmiStorageInterfaceAhci && DeviceType == ATA ) {
        // Check if SMART is supported, if no exit
        if ( IdentifyData.Command_Set_Supported_82 & 0x1 ) {
            // Whether to Enable or Disable SMART Support
            bData = ENABLE_SMART == 1 ? SMART_ENABLE_CMD : SMART_DISABLE_CMD;

            // Check if current SMART status is what is required
            Status = EFI_SUCCESS;

            if ( !((IdentifyData.Command_Set_Enabled_85 & 0x1) == ENABLE_SMART)) {
                ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
                CommandStructure.Features = bData;
                CommandStructure.LBAMid   = 0x4F;
                CommandStructure.LBAHigh  = 0xC2;
                CommandStructure.Command  = SMART_COMMAND;

                Status = CommonNonDataHook( BusInterface,
                                            CommandStructure,
                                            BusInterfaceType );

                if ( EFI_ERROR( Status )) {
                    return Status;
                }
            }

            // Enable Autosave feature
            if ((Status == EFI_SUCCESS) && (bData == SMART_ENABLE_CMD)) {
                ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
                CommandStructure.Features    = SMART_AUTOSAVE;
                CommandStructure.SectorCount = 0xF1;
                CommandStructure.LBAMid      = 0x4F;
                CommandStructure.LBAHigh     = 0xC2;
                CommandStructure.Command     = SMART_COMMAND;

                Status = CommonNonDataHook( BusInterface,
                                            CommandStructure,
                                            BusInterfaceType );

                if ( EFI_ERROR( Status )) {
                    return Status;
                }
            }
        }
    }

    if( (BusInterfaceType <= AmiStorageInterfaceAhci && DeviceType == ATA) 
            || (BusInterfaceType == AmiStorageInterfaceNvme )) {

        // Get SMART Self test setup option
        Status = pRS->GetVariable(L"Setup", &gSetupGuid, NULL,
                                    &SetupDataSize, &gSetupData);

        if (EFI_ERROR(Status)) {
            // If Setup Variable is not found, set Default values
            gSetupData.SmartSelfTest = DEFAULT_SMART_SELFTEST;
        }

        if (gSetupData.SmartSelfTest) {

            // If SMART threshold exceeded, report it to the user.
            Status = SmartReturnStatusWrapper( BusInterface, BusInterfaceType , &SmartFailure);
            if(EFI_ERROR(Status)) { 
                 return Status;
            }
            if (SmartFailure) {
                ERROR_CODE( DXE_IDE_SMART_ERROR, EFI_ERROR_MAJOR );
                
                if(pST->ConIn != NULL) {
                    ReportSmartFailure(BusInterfaceType, BusInterface);
                } else {
                
                    // Registering CallBack function to Display Smart Error Message
                    // latter Once System Table's ConIn is Updated.
                    Status = pBS->AllocatePool( EfiBootServicesData,
                                                sizeof(SMART_ERROR_DEVINFO),
                                                &SmartErrorDevInfo);
                    ASSERT_EFI_ERROR(Status);
                    if(EFI_ERROR(Status)) {
                        return Status;
                    }
                    
                    pBS->SetMem( SmartErrorDevInfo, sizeof(SMART_ERROR_DEVINFO), 0);
                    SmartErrorDevInfo->BusInterfaceType = BusInterfaceType;
                    SmartErrorDevInfo->BusInterface  = BusInterface; 

                    Status = RegisterProtocolCallback ( &gBdsAllDriversConnectedProtocolGuid,\
                                                        ReportSmartFailureCallBack,\
                                                        SmartErrorDevInfo,\
                                                        &ReportSmartFailureEvent,\
                                                        &ReportSmartFailureRegistration);
                    if (EFI_ERROR(Status)) {
                        return Status;
                    }
                }
            }
        }
    }
    return EFI_SUCCESS;
}

/**
    Return SMART Status

        
    @param BusInterface 
    @param BusInterfaceType 
    @param *SmartFailure
    
    @retval EFI_STATUS

    @note  Returns EFI_SUCCESS if threshold is not exceeded.

**/

EFI_STATUS 
SmartReturnStatusWrapper (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType,
    IN OUT BOOLEAN                  *SmartFailure
)
{
   
    UINT16                  wData = THRESHOLD_NOT_EXCEEDED;
    EFI_STATUS              Status;
    COMMAND_STRUCTURE       CommandStructure;
    NVME_SMART_HEALTH_INFO  NvmeSmartHealthInfo;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL    *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
    EFI_PCI_IO_PROTOCOL     *PciIo = NULL;
    UINT8                   bData=0;
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

    if( BusInterfaceType <= AmiStorageInterfaceAhci ) {

        // Read the SMART return Status
        ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
        CommandStructure.Features = SMART_RETURN_STATUS;
        CommandStructure.LBAMid   = 0x4F;
        CommandStructure.LBAHigh  = 0xC2;
        CommandStructure.Command  = SMART_COMMAND;

        Status = CommonNonDataHook( BusInterface, CommandStructure, BusInterfaceType );

        ASSERT_EFI_ERROR(Status);
        if ( EFI_ERROR( Status )) {
            return Status;
        }

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
        if ( BusInterfaceType == AmiStorageInterfaceIde ) {
            if ( !EFI_ERROR( Status )) {
                PciIo = IdeBusInterface->PciIO;

                PciIo->Io.Read(
                        IdeBusInterface->PciIO,
                        EfiPciIoWidthFifoUint8,
                        EFI_PCI_IO_PASS_THROUGH_BAR,
                        IdeBusInterface->IdeDevice.Regs.CommandBlock.
                        LBAHighReg,
                        1,
                        &bData
                        );

                // Get the LBA Mid/High register contends.
                wData = bData << 8;
                PciIo->Io.Read(
                        IdeBusInterface->PciIO,
                        EfiPciIoWidthFifoUint8,
                        EFI_PCI_IO_PASS_THROUGH_BAR,
                        IdeBusInterface->IdeDevice.Regs.CommandBlock.
                        LBAMidReg,
                        1,
                        &bData
                        );
                wData |= bData;
            }
        } 
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
        if( BusInterfaceType == AmiStorageInterfaceAhci ) {
            volatile AHCI_RECEIVED_FIS *FISReceiveAddress;
            FISReceiveAddress = (AHCI_RECEIVED_FIS*)SataDevInterface->PortFISBaseAddr;

            // Get the pointer to RFIS
            if ( !EFI_ERROR( Status ) && (FISReceiveAddress->Ahci_Rfis[0] == FIS_REGISTER_D2H))
                wData = (FISReceiveAddress->Ahci_Rfis[6] << 8) + FISReceiveAddress->Ahci_Rfis[5];
        }
#endif

        if ( wData == THRESHOLD_NOT_EXCEEDED ) {
            *SmartFailure = FALSE;
        } else {
            *SmartFailure = TRUE;
        }
    } else { // Here, BusInterfaceType = AmiStorageInterfaceNvme

        pBS->SetMem(&NvmeSmartHealthInfo, sizeof(NVME_SMART_HEALTH_INFO),0);

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
        Status = GetNvmeSmartHealthData (
                        BusInterface,
                        0x2,
                        (UINT8*)&NvmeSmartHealthInfo,
                        sizeof(NVME_SMART_HEALTH_INFO)>>2 // Send Number of DWORD as Bufferlength
                        );
        
#else
        Status= EFI_NOT_FOUND;
#endif        
        if(EFI_ERROR(Status)) {
            return EFI_DEVICE_ERROR;
        }
        // Check for Nvme Smart Error
        if( NvmeSmartHealthInfo.CriticalWarning & NVME_SMART_HEALTH_IDENTIFY_MASK ) {
            *SmartFailure = TRUE;
        } else {
            *SmartFailure = FALSE;
        }
    }

    return Status;
}

/**
    Default function to report SMART failures to the POST screen

    @param  BusInterfaceType BusInterfaceType - 0 - Device is in IDE mode
    @param  1 - Device is in AHCI mode
    @param  VOID *Interface - Interface.  Should be cast into SATA_DEVICE_INTERFACE
            or AMI_IDE_BUS_PROTOCOL depending on BusInterfaceType

    @retval VOID
    @retval VOID

**/

VOID 
SmartErrorThresholdExceeded (
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType,
    VOID                            *Interface
)
{
    EFI_STATUS      Status;
    UINTN           StringSize = 0;
    CHAR16          *StrBuffer = NULL;
    CHAR16          *Str1 = NULL;
    CHAR16          *Str2 = NULL;
    CHAR16          *DeviceName = NULL;
    UINT8           ChannelorPort = 0;
    UINT8           DeviceorPMPort = 0;
    STRING_REF      Token;
    EFI_INPUT_KEY   Key;
    EFI_TPL         OldTpl;
    
    // Reducing the curret TPL level to TPL_APPLICATION level,
    // inorder to allow the execution of USB Timer Event(UsbHcOnTimer),
    // which will monitor the state of USBConnect and disconnect.
    
    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL); //we store the current TPL
    pBS->RestoreTPL(TPL_APPLICATION);
    //
    // Get the Port/Channel information from the interface
    //
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if (BusInterfaceType == AmiStorageInterfaceAhci) {
        //
        // Sata Mode
        //
        DeviceName = ((SATA_DEVICE_INTERFACE*)Interface)->UDeviceName->UnicodeString;
        ChannelorPort = ((SATA_DEVICE_INTERFACE*)Interface)->PortNumber;
        DeviceorPMPort = ((SATA_DEVICE_INTERFACE*)Interface)->PMPortNumber;
    } 
#endif
    
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if (BusInterfaceType == AmiStorageInterfaceIde) {
        //
        // IDE Mode
        //
        DeviceName = ((AMI_IDE_BUS_PROTOCOL*)Interface)->IdeDevice.UDeviceName->UnicodeString;
        ChannelorPort = ((AMI_IDE_BUS_PROTOCOL*)Interface)->IdeDevice.Channel;
        DeviceorPMPort = ((AMI_IDE_BUS_PROTOCOL*)Interface)->IdeDevice.Device;
    }
#endif

    if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        // Nvme device
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
            DeviceName = \
                 ((NVM_EXPRESS_PASS_THRU_INSTANCE*)Interface)->NvmeControllerProtocol->UDeviceName->UnicodeString;
#endif
    }
    if ( BusInterfaceType == RaidStorageInterfaceNvme ) {
        DeviceName =  gUnicodeStr;
    }

    // Locate the AMI Post Manger protocol for displaying information on the post screen
    Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid,
                                 NULL,
                                 (VOID**)&pAmiPostMgr );

    // Check if we could locate the AMI post manager
    if (!EFI_ERROR(Status) && pST->ConOut != NULL) {
        // Depending on the mode, get different strings to identify the port the failing
        //  device is connected to
        if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
            // Sata Mode

            // Print the SATA port that the device is connected
            //  so get the "Port" string
            Str2 = HiiGetString(HiiPostHandle, STRING_TOKEN(STR_SMART_PORT));

            //
            // allocate enough space to print the "port %d" string 
            //
            Status = pBS->AllocatePool(EfiBootServicesData,
                                       sizeof(CHAR16)*(Wcslen(Str2)+4),
                                       (VOID**)&Str1);
            if ( EFI_ERROR( Status )) {
                pBS->RaiseTPL(TPL_HIGH_LEVEL);  // guarantees that RestoreTPL won't ASSERT
                pBS->RestoreTPL(OldTpl);
                return;
            }

            //
            // Create the "Port <0|1|2|3|4|5>" string in Str1
            //
            Swprintf(Str1, L"%s %01d", Str2, ChannelorPort);

            //
            // Fill string 2 with a null
            //
            Str2[0] = L'\0';
        } else if ( BusInterfaceType == AmiStorageInterfaceIde ) {
            // IDE Mode

            // Determine if we need to get the "Primary" or "Secondary" string
            if (ChannelorPort == PRIMARY_CHANNEL) Token = STRING_TOKEN(STR_PRIMARY);  
            else Token = STRING_TOKEN(STR_SECONDARY);  

            // Get the string out of the Hiidatabase into Str1
            Str1 = HiiGetString(HiiPostHandle, Token);

            // Determine if we need to get the "Master" or "Slave" string
            if (DeviceorPMPort == MASTER_DRIVE) Token = STRING_TOKEN(STR_MASTER);  
            else Token = STRING_TOKEN(STR_SLAVE);  

            // Get the string out of the Hiidatabase into Str1
            Str2 = HiiGetString(HiiPostHandle, Token);
        }

        if( BusInterfaceType == AmiStorageInterfaceNvme || BusInterfaceType == RaidStorageInterfaceNvme ) {
            // DeviceNameLength + 4 for Newline,Carriage return, Null termination
            if(DeviceName != NULL) {
                StringSize = Wcslen(DeviceName) + 4;
            }
            Status = pBS->AllocatePool( EfiBootServicesData,
                                        StringSize*sizeof(CHAR16),
                                        (VOID**)&StrBuffer);
            if ( EFI_ERROR( Status )) {
                pBS->RaiseTPL(TPL_HIGH_LEVEL);  // guarantees that RestoreTPL won't ASSERT
                pBS->RestoreTPL(OldTpl);
                return;
            }

            Swprintf(StrBuffer, L"\n\r%s", DeviceName);
        } else {
            // Determine the size of all the components we are trying to print
            //  and additional spaces for the newline, Carriage return, the two spaces
            //  the colon and the null termination
            if(DeviceName != NULL) {
                StringSize = Wcslen(Str1) + Wcslen(Str2) + Wcslen(DeviceName) + 7;
            }
              
            Status = pBS->AllocatePool( EfiBootServicesData,
                                        StringSize*sizeof(CHAR16),
                                        (VOID**)&StrBuffer);
            ASSERT_EFI_ERROR(Status);
            if ( EFI_ERROR( Status )) {
                pBS->RaiseTPL(TPL_HIGH_LEVEL);  // guarantees that RestoreTPL won't ASSERT
                pBS->RestoreTPL(OldTpl);
                return;
            }

            // Build the string for IDE or SATA: 
            // Ide string looks like - "<Primary|Secondary> <Master|Slave> : <Device Name String>"
            // sata string looks like - "Port <0|1|2|3|4|5> : <Device Name String>"
            Swprintf(StrBuffer, L"\n\r%s %s: %s", Str1, Str2, DeviceName);

            // free the helper buffers
            if(Str1 != NULL)
                pBS->FreePool(Str1);

            if(Str2 != NULL) {
                pBS->FreePool(Str2);
                Str2 = NULL;
            }
        }
        // Display the post message
        pAmiPostMgr->SwitchToPostScreen();
        pAmiPostMgr->DisplayPostMessage(StrBuffer);

        // and free the buffer that was used
        if(StrBuffer != NULL) {
            pBS->FreePool(StrBuffer);
            StrBuffer = NULL;
        }

        // get the smart failure string from the HiiDatabase
       Str1 = HiiGetString(HiiPostHandle, STRING_TOKEN(STR_SMART_FAILURE));

        // And display it below the drive information strings, then free the space allocated
        pAmiPostMgr->DisplayPostMessage(Str1);
        if(Str1 != NULL) {
            pBS->FreePool(Str1);
            Str1 = NULL;
        }

        // If keyboard input is available, pause for user input
        if (pST->ConIn != NULL) {

            // Get the pause string from the HiiDatabase
            Str1 = HiiGetString(HiiPostHandle, STRING_TOKEN(STR_SMART_FAILURE_PROMPT));

            // Display the pause message on the screen
            pAmiPostMgr->DisplayPostMessage( Str1 );


            // Free the space allocated by GetHiiString
            if(Str1 != NULL)
                pBS->FreePool(Str1);


            // And wait for the user to press the input key
            Key.UnicodeChar = 0;
            do {
                //
                // Pause for 1/2 a second between key press reads
                //
                pBS->Stall(50000);
                Status = pST->ConIn->ReadKeyStroke( pST->ConIn, &Key );
                if(Status == EFI_DEVICE_ERROR){
                    pBS->RaiseTPL(TPL_HIGH_LEVEL);  // guarantees that RestoreTPL won't ASSERT
                    pBS->RestoreTPL(OldTpl);
                    return;
                }
                
            } while ( Key.ScanCode != WAIT_FOR_ERROR_KEY );
        }//if(pST->ConIn != NULL)
    }//if(!EFI_ERROR(Status) && pST->ConOut != NULL)
    
    pBS->RaiseTPL(TPL_HIGH_LEVEL);  // guarantees that RestoreTPL won't ASSERT
    pBS->RestoreTPL(OldTpl);
    return;
}

/**
    Installs SmartInterface Protocol

    @param  BusInterface 
    @param  BusInterfaceType 

    @retval EFI_STATUS

    @note   Already SMART support capability has been established.

**/

EFI_STATUS
EFIAPI
InstallSmartInterface (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
)
{
    EFI_STATUS              Status;
    AMI_SMART_INTERFACE     *SmartInterface;
    UINT8                   *Buffer = NULL;
    IDENTIFY_DATA           IdentifyData = { 0 };
    EFI_HANDLE              DeviceHandle  = NULL;
    DEVICE_TYPE             DeviceType = 0;
    COMMAND_STRUCTURE       CommandStructure;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
   SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif
   
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeDevInterface = (AMI_NVME_CONTROLLER_PROTOCOL*)BusInterface;
    if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        DeviceHandle = NvmeDevInterface->ControllerHandle;
    }
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        IdentifyData    = IdeBusInterface->IdeDevice.IdentifyData;
        DeviceType      = IdeBusInterface->IdeDevice.DeviceType;
        DeviceHandle = IdeBusInterface->IdeDeviceHandle;
    }
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if( BusInterfaceType == AmiStorageInterfaceAhci ) {

        IdentifyData    = SataDevInterface->IdentifyData;
        DeviceType      = SataDevInterface->DeviceType;
        DeviceHandle = SataDevInterface->IdeDeviceHandle;
    }
#endif
    if(DeviceHandle == NULL) { 
	    return EFI_NOT_FOUND;
    }
    // Return error if gAmiHddSmartProtocolGuid is already installed on the Device Handle
    Status = pBS->OpenProtocol ( DeviceHandle, 
                                 &gAmiHddSmartProtocolGuid, 
                                 NULL, 
                                 NULL, 
                                 NULL, 
                                 EFI_OPEN_PROTOCOL_TEST_PROTOCOL );
    if(!EFI_ERROR(Status)) {
        return EFI_ALREADY_STARTED;
    }

    // Check for HDD and SMART support
    if  ( BusInterfaceType<=AmiStorageInterfaceAhci    // Hdd handled by AHCI or IDE
         && (DeviceType == ATA)          // It is HDD
         && (IdentifyData.Command_Set_Supported_82 & 0x1)   // SMART supported
         && (IdentifyData.Command_Set_Supported_84 & 0x2 )) // SMART Enabled

    {
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    512,
                                    (VOID**)&Buffer );
        ASSERT_EFI_ERROR(Status);
        if ( EFI_ERROR( Status )) {
            return EFI_OUT_OF_RESOURCES;
        }

        ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
        CommandStructure.Buffer    = Buffer;
        CommandStructure.ByteCount = 512;
        CommandStructure.Features  = SMART_READ_DATA;
        CommandStructure.LBAMid    = 0x4F;
        CommandStructure.LBAHigh   = 0xC2;
        CommandStructure.Command   = SMART_COMMAND;
        
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
        if ( BusInterfaceType == AmiStorageInterfaceIde ) {
            CommandStructure.Device = IdeBusInterface->IdeDevice.Device << 4;
            Status = IdeBusInterface->AtaPioDataIn(
                            IdeBusInterface,
                            CommandStructure.Buffer,
                            CommandStructure.ByteCount,
                            CommandStructure.Features,
                            (UINT8)CommandStructure.SectorCount,
                            CommandStructure.LBALow,
                            CommandStructure.LBAMid,
                            CommandStructure.LBAHigh,
                            CommandStructure.Device,
                            CommandStructure.Command,
                            FALSE);
        } 
#endif
        
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
        if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
            Status = SataDevInterface->AhciBusInterface->ExecutePioDataCommand(
                SataDevInterface,
                &CommandStructure,
                FALSE );
        }
#endif

        if ( EFI_ERROR( Status )) {
            goto Error;
        }
    }


    if ( (BusInterfaceType<=AmiStorageInterfaceAhci && DeviceType == ATA) 
            || (BusInterfaceType == AmiStorageInterfaceNvme ) ) {

        // Install the Interface
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    sizeof(AMI_SMART_INTERFACE),
                                    (VOID**)&SmartInterface );
        ASSERT_EFI_ERROR(Status);
        if ( EFI_ERROR( Status )) {
           goto Error;
        }

        SmartInterface->Revision            = SMART_PROTOCOL_REVISION;
        SmartInterface->Signature           = SMART_PROTOCOL_SIGNATURE;
        SmartInterface->SmartSelfTest       = SmartSelfTest;
        SmartInterface->SmartSelfTestStatus = SmartSelfTestStatus;
        SmartInterface->SmartReturnStatus   = SmartReturnStatus;
        SmartInterface->SendSmartCommand    = SendSmartCommand;
        SmartInterface->SmartReadData       = SmartReadData;
        SmartInterface->SmartWriteData      = SmartWriteData;
        SmartInterface->SmartAbortSelfTest  = SmartAbortSelfTest;
        SmartInterface->SmartGetSmartData   = SmartGetSmartData;

        if ( BusInterfaceType <= AmiStorageInterfaceAhci ) {
            SmartInterface->ShortPollingTime    = Buffer[372];
            if ( Buffer[373] == 0xFF )
                SmartInterface->ExtPollingTime  = (UINT16)(Buffer[376]<<8 | Buffer[375]);
            else
                SmartInterface->ExtPollingTime  = (UINT16)Buffer[373];
        } else { // Here, BusInterfaceType = AmiStorageInterfaceNvme
            SmartInterface->ShortPollingTime = 0;
            SmartInterface->ExtPollingTime  = 0;
        }

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
        if ( BusInterfaceType == AmiStorageInterfaceIde ) {
            IdeBusInterface->SmartInterface = (AMI_HDD_SMART_PROTOCOL*)SmartInterface;
            SmartInterface->BusInterface    = IdeBusInterface;
        } 
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
        if ( BusInterfaceType == AmiStorageInterfaceAhci ) {       
            SataDevInterface->SmartInterface = (AMI_HDD_SMART_PROTOCOL*)SmartInterface;
            SmartInterface->BusInterface     = SataDevInterface;
        }
#endif

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
        if( BusInterfaceType == AmiStorageInterfaceNvme ) {
            NvmeDevInterface->SmartInterface = SmartInterface;
            SmartInterface->BusInterface     = NvmeDevInterface->EfiNvmExpressPassThru;
        }
#endif
        // Set BusInterfaceType to FALSE if IdeBus else TRUE for Ahci Bus.
        SmartInterface->BusInterfaceType = BusInterfaceType;  // Ide Bus

        Status = pBS->InstallMultipleProtocolInterfaces( &DeviceHandle,
                                                         &gAmiHddSmartProtocolGuid,
                                                         SmartInterface,
                                                         NULL );
        if (EFI_ERROR(Status) && (SmartInterface !=NULL)) {
            pBS->FreePool( SmartInterface);
        }
    }
    
Error: 
    if((Buffer != NULL)) {
        pBS->FreePool( Buffer );
    }
    return Status;
}

/**
    Uninstall SmartInterface Protocol

    @param  BusInterface 
    @param  BusInterfaceType 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
UnInstallSmartInterface (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
)
{
    EFI_STATUS              Status;
    AMI_SMART_INTERFACE     *SmartInterface = NULL;
    EFI_HANDLE              DeviceHandle = NULL;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
   
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        SmartInterface  = (AMI_SMART_INTERFACE*)IdeBusInterface->SmartInterface;
        DeviceHandle = IdeBusInterface->IdeDeviceHandle;
    }
#endif
   
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        SmartInterface  = (AMI_SMART_INTERFACE*)SataDevInterface->SmartInterface;
        DeviceHandle = SataDevInterface->IdeDeviceHandle;
    }
#endif

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    if(  BusInterfaceType == AmiStorageInterfaceNvme ) {
        SmartInterface  = (AMI_SMART_INTERFACE*)(((AMI_NVME_CONTROLLER_PROTOCOL*)BusInterface)->SmartInterface);
        DeviceHandle = ((AMI_NVME_CONTROLLER_PROTOCOL*)BusInterface)->ControllerHandle;
    }
#endif


    if ( SmartInterface == NULL || DeviceHandle == NULL ) {
        return EFI_SUCCESS;
    }

    Status = pBS->UninstallMultipleProtocolInterfaces( DeviceHandle,
                                                       &gAmiHddSmartProtocolGuid,
                                                       SmartInterface,
                                                       NULL );

    if ((Status == EFI_SUCCESS) && (SmartInterface != NULL)) {
        pBS->FreePool( SmartInterface );
        SmartInterface = NULL;
    }
    return Status;
}

/**
    Starts SMART SelfTest. SelfTestType will indicate whether to run
    short or extended self test.

    @param This 
    @param SelfTestType ( 0 : short, 1 :extended)
    @param PollPeriod (Minutes)

    @retval EFI_STATUS
    @retval PollPeriod will indicate the recommended time interval between polling.
    @retval Frequent polling will either abort the self test or may prolong it.

**/

EFI_STATUS 
EFIAPI
SmartSelfTest (
    IN AMI_HDD_SMART_PROTOCOL   *This,
    IN UINT8        SelfTestType,
    OUT UINT16      *PollPeriod
)
{
    EFI_STATUS              Status;
    UINT8                   bData;
    AMI_SMART_INTERFACE     *SmartInterface = (AMI_SMART_INTERFACE*)This;    

    if( SmartInterface->BusInterfaceType == AmiStorageInterfaceNvme ) {
        return EFI_UNSUPPORTED;
    }

    if(PollPeriod == NULL)
        return EFI_INVALID_PARAMETER;

    // Check for Short or Extended
    bData = SelfTestType == 0 ? 1 : 2;
    Status = SendSmartCommand( This, SMART_EXECUTE_OFFLINE_IMMEDIATE, 0, bData);

    *PollPeriod = SelfTestType == 0 ? SmartInterface->ShortPollingTime : SmartInterface->ExtPollingTime;

    return Status;
}

/**
    Aborts SMART Self Test. 

    @param  This 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
SmartAbortSelfTest (
    IN  AMI_HDD_SMART_PROTOCOL  *This
)
{
    VOID                       *BusInterface = ((AMI_SMART_INTERFACE * )This)->BusInterface;    
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType = ((AMI_SMART_INTERFACE * )This)->BusInterfaceType;
    COMMAND_STRUCTURE   CommandStructure;

    if( BusInterfaceType == AmiStorageInterfaceNvme ) {
        return EFI_UNSUPPORTED;
    }

    ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Features = SMART_EXECUTE_OFFLINE_IMMEDIATE;
    CommandStructure.LBALow   = SMART_ABORT_SELF_TEST_SUBROUTINE;
    CommandStructure.LBAMid   = 0x4F;
    CommandStructure.LBAHigh  = 0xC2;
    CommandStructure.Command  = SMART_COMMAND;

    return CommonNonDataHook (BusInterface, CommandStructure, BusInterfaceType );

}

/**
    Get SMART data of the hard disk.

    @param  This 

    @retval OUT UINT8   **SmartData

**/

EFI_STATUS
EFIAPI
SmartGetSmartData (
    IN              AMI_HDD_SMART_PROTOCOL  *This,
    OUT UINT8       **SmartData
)
{
    EFI_STATUS                 Status;    
    VOID                       *BusInterface = ((AMI_SMART_INTERFACE * )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType = ((AMI_SMART_INTERFACE * )This)->BusInterfaceType;
    COMMAND_STRUCTURE          CommandStructure;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

    if( !SmartData ) {
        return EFI_INVALID_PARAMETER;
    }

    Status = pBS->AllocatePool ( EfiBootServicesData,
                                 512,
                                 (VOID**)SmartData );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    if( BusInterfaceType <= AmiStorageInterfaceAhci ) {
        ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
        CommandStructure.Buffer     = *SmartData; 
        CommandStructure.ByteCount  = 512;
        CommandStructure.Features   = SMART_READ_DATA;
        CommandStructure.LBAMid     = 0x4F;
        CommandStructure.LBAHigh    = 0xC2;   
        CommandStructure.Command    = SMART_COMMAND;
    
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
        if (BusInterfaceType == AmiStorageInterfaceIde) {
            CommandStructure.Device = IdeBusInterface->IdeDevice.Device << 4;
            Status = IdeBusInterface->AtaPioDataIn( IdeBusInterface, 
                                                    CommandStructure.Buffer,
                                                    CommandStructure.ByteCount,
                                                    CommandStructure.Features, 
                                                    (UINT8)CommandStructure.SectorCount,
                                                    CommandStructure.LBALow, 
                                                    CommandStructure.LBAMid, 
                                                    CommandStructure.LBAHigh,
                                                    CommandStructure.Device, 
                                                    CommandStructure.Command,
                                                    FALSE
                                                     );
        } 
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
        if (BusInterfaceType == AmiStorageInterfaceAhci) {
            Status = SataDevInterface->AhciBusInterface->ExecutePioDataCommand( 
                                                SataDevInterface, 
                                                &CommandStructure, 
                                                FALSE
                                                );
        }
#endif
    } else { // BusInterfaceType = AmiStorageInterfaceNvme 
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
        Status = GetNvmeSmartHealthData (
                        BusInterface,
                        0x2,
                        (UINT8*)*SmartData,
                        sizeof(NVME_SMART_HEALTH_INFO)>>2
                        );
#else 
        Status= EFI_NOT_FOUND;
#endif        
    }

    return Status;
}

/**
    Starts SMART SelfTest. SelfTestType will indicate whether to run
    short or extended self test.

        
    @param This 
    @param TestStatus 

    @retval 
        EFI_STATUS : TestStatus is valid only when EFI_STATUS is EFI_SUCCESS
        TestStatus : Will indicate the % of test completed.
        TestStatus = 0xFF Indicates Self Test Failed and EFI_STATUS = EFI_ERROR
  Example: When TestStatus = 10, 10% of the test has been done.

**/

EFI_STATUS 
EFIAPI
SmartSelfTestStatus (
    IN AMI_HDD_SMART_PROTOCOL   *This,
    OUT UINT8                   *TestStatus
)
{
    EFI_STATUS                 Status;
    UINT8                      *Buffer;    
    VOID                       *BusInterface = ((AMI_SMART_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType = ((AMI_SMART_INTERFACE* )This)->BusInterfaceType;
    COMMAND_STRUCTURE          CommandStructure;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

    if( BusInterfaceType == AmiStorageInterfaceNvme ) {
        return EFI_UNSUPPORTED;
    }

    if(TestStatus == NULL)
        return EFI_INVALID_PARAMETER;
    Status = pBS->AllocatePool( EfiBootServicesData,
                                512,
                                (VOID**)&Buffer );
    ASSERT_EFI_ERROR(Status);
    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }

    ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Buffer    = Buffer;
    CommandStructure.ByteCount = 512;
    CommandStructure.Features  = SMART_READ_DATA;
    CommandStructure.LBAMid    = 0x4F;
    CommandStructure.LBAHigh   = 0xC2;
    CommandStructure.Command   = SMART_COMMAND;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        CommandStructure.Device = IdeBusInterface->IdeDevice.Device << 4;
        Status = IdeBusInterface->AtaPioDataIn(IdeBusInterface,
                                                CommandStructure.Buffer,
                                                CommandStructure.ByteCount,
                                                CommandStructure.Features,
                                                (UINT8)CommandStructure.SectorCount,
                                                CommandStructure.LBALow,
                                                CommandStructure.LBAMid,
                                                CommandStructure.LBAHigh,
                                                CommandStructure.Device,
                                                CommandStructure.Command,
                                                FALSE);
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        Status = SataDevInterface->AhciBusInterface->ExecutePioDataCommand(
            SataDevInterface,
            &CommandStructure,
            FALSE );
    }
#endif
    

    if ( EFI_ERROR( Status )) {
        *TestStatus = 0xff;
        if(Buffer != NULL)
            pBS->FreePool( Buffer );
        return EFI_DEVICE_ERROR;
    }

    //
    // If Bits 7:4 is not equal to 0 or 0xf then there was an error
    // in the previous self test.
    //
    if (((Buffer[363] >> 4) != 0) && ((Buffer[363] >> 4) != 0xf)) {
        *TestStatus = 0xff;
        Status      = EFI_DEVICE_ERROR;
    } else {
        *TestStatus = Buffer[363] & 0xf;
        *TestStatus = 10 - (*TestStatus);
        *TestStatus = 10 * (*TestStatus);
    }
    if(Buffer != NULL)
        pBS->FreePool( Buffer );
    return Status;
}

/**
    Returns SMART status

    @param This 

    @retval EFI_STATUS

    @note  Returns EFI_SUCCESS if threshold is not exceeded.

**/

EFI_STATUS 
EFIAPI
SmartReturnStatus (
    IN AMI_HDD_SMART_PROTOCOL    *This,
    IN OUT BOOLEAN               *SmartFailure
)
{
    VOID *BusInterface        = ((AMI_SMART_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType  = ((AMI_SMART_INTERFACE* )This)->BusInterfaceType;

    return (SmartReturnStatusWrapper( BusInterface, BusInterfaceType, SmartFailure ));
}

/**
    Sends any Non-Data SMART Command. 

    @param This 
    @param SubCommand 
    @param AutoSaveEnable //OPTIONAL
    @param SelfTestType //OPTIONAL

    @retval EFI_STATUS

    @note  Returns EFI_UNSUPPORTED when SMART feature is not enabled.

**/

EFI_STATUS
EFIAPI
SendSmartCommand ( 
    IN AMI_HDD_SMART_PROTOCOL    *This,
    IN UINT8                     SubCommand,
    IN UINT8                     AutoSaveEnable,
    IN UINT8                     SelfTestType
)
{
    EFI_STATUS                 Status = EFI_UNSUPPORTED;    
    VOID                       *BusInterface = ((AMI_SMART_INTERFACE * )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType = ((AMI_SMART_INTERFACE * )This)->BusInterfaceType;
    
    COMMAND_STRUCTURE          CommandStructure;
    IDENTIFY_DATA              IdentifyData = { 0 };
    BOOLEAN                    EnableSmart;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
   
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

    if( BusInterfaceType == AmiStorageInterfaceNvme ) {
        return EFI_UNSUPPORTED;
    }

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        IdentifyData = IdeBusInterface->IdeDevice.IdentifyData;
    } 
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdentifyData = SataDevInterface->IdentifyData;
    }
#endif
    
    EnableSmart = SubCommand == SMART_ENABLE_CMD ? 1 : 0; 

    if (!((IdentifyData.Command_Set_Enabled_85 & 0x1) == EnableSmart)) {

        ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE));
        CommandStructure.Features = SubCommand;
        CommandStructure.LBAMid   = 0x4F;
        CommandStructure.LBAHigh  = 0xC2;
        CommandStructure.Command  = SMART_COMMAND;

        switch (SubCommand) {

        case SMART_AUTOSAVE:
          CommandStructure.SectorCount = AutoSaveEnable;
          break;

        case SMART_EXECUTE_OFFLINE_IMMEDIATE:
          CommandStructure.LBALow   = SelfTestType;
          break;

        default:
          break;
        }

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
        if ( BusInterfaceType == AmiStorageInterfaceIde ) {
             CommandStructure.Device = IdeBusInterface->IdeDevice.Device << 4;

            //
            // IdeBus API changed from Core 4.6.5.2. Added Core Version check for the OLD 
            // Core support.
            //
            Status = IdeBusInterface->IdeNonDataCommand(
                                         IdeBusInterface,
                                         CommandStructure.Features,
                                         (UINT8)CommandStructure.SectorCount,
                                         (UINT8)(CommandStructure.SectorCount >> 8),
                                         CommandStructure.LBALow,
                                         CommandStructure.LBALowExp,
                                         CommandStructure.LBAMid,
                                         CommandStructure.LBAMidExp,
                                         CommandStructure.LBAHigh,
                                         CommandStructure.LBAHighExp,
                                         CommandStructure.Device,
                                         CommandStructure.Command
                                         );
        } 
#endif
        
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
        if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
            Status = SataDevInterface->AhciBusInterface->ExecuteNonDataCommand(
                                                            SataDevInterface,
                                                            CommandStructure 
                                                            );
        }
#endif
        return Status;

    } else {
        return EFI_UNSUPPORTED;
    }
}

/**
    Sends any Data In SMART command. 

    @param This 
    @param SubCommand 
    @param Buffer 
    @param LogAddress //OPTIONAL    
    @param SectorCount //OPTIONAL 
 
    @retval EFI__STATUS

**/

EFI_STATUS 
EFIAPI
SmartReadData (
    IN  AMI_HDD_SMART_PROTOCOL    *This,
    IN  UINT8                     SubCommand,
    OUT VOID                      *Buffer,
    IN  UINT8                     LogAddress,
    IN  UINT8                     SectorCount
)
{
    EFI_STATUS                 Status = EFI_UNSUPPORTED; 
    VOID                       *BusInterface = ((AMI_SMART_INTERFACE * )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType = ((AMI_SMART_INTERFACE * )This)->BusInterfaceType;
    COMMAND_STRUCTURE          CommandStructure;
    BOOLEAN                    MultipleSectors = FALSE;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

    if(Buffer == NULL)
        return EFI_INVALID_PARAMETER;

    if( BusInterfaceType == AmiStorageInterfaceNvme ) {
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
        if( SubCommand != GET_LOG_PAGE ) {
            return EFI_UNSUPPORTED;
        }

        Status = GetNvmeSmartHealthData (
                        BusInterface,
                        LogAddress,
                        Buffer,
                        SectorCount
                        );
#else 
        Status = EFI_NOT_FOUND;
#endif        
        return Status;
    }

    //
    //If SubCommand = SMART_READ_DATA then set SectorCount to 1 
    //
    if (SectorCount == 0) {
        SectorCount = 1;
    }

    ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Buffer     = Buffer; 

    if ( SubCommand == SMART_READLOG ) {
        CommandStructure.LBALow       = LogAddress ;      // LOG Address to read
        CommandStructure.SectorCount  = SectorCount;      // No.of sectors to read
        CommandStructure.ByteCount    = SectorCount*ATA_SECTOR_BYTES;
        if ( SectorCount>1 ) {
            MultipleSectors = TRUE;
        }
    } else {
        //
        // SubCommand SMART_READ_DATA reads only 512 bytes.
        //
        CommandStructure.ByteCount    = 512;
    }
    CommandStructure.Features   = SubCommand;
    CommandStructure.LBAMid     = 0x4F;
    CommandStructure.LBAHigh    = 0xC2;
    CommandStructure.Command    = SMART_COMMAND;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if (BusInterfaceType == AmiStorageInterfaceIde) {

        Status = IdeBusInterface->AtaPioDataOut ( 
                                       IdeBusInterface,
                                       CommandStructure.Buffer,
                                       CommandStructure.ByteCount, 
                                       CommandStructure.Features,
                                       CommandStructure.SectorCount, 
                                       CommandStructure.LBALow, 
                                       CommandStructure.LBALowExp, 
                                       CommandStructure.LBAMid, 
                                       CommandStructure.LBAMidExp, 
                                       CommandStructure.LBAHigh,
                                       CommandStructure.LBAHighExp, 
                                       CommandStructure.Device, 
                                       CommandStructure.Command,
                                       FALSE, 
                                       MultipleSectors );
          
    } 
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if (BusInterfaceType == AmiStorageInterfaceAhci) {
        Status = SataDevInterface->AhciBusInterface->SataPioDataOut(
                                                        SataDevInterface,
                                                        CommandStructure,
                                                        FALSE );
    }
#endif
    return Status;
}

/**
    Sends any Data Out SMART command.

    @param This 
    @param SubCommand 
    @param Buffer 
    @param LogAddress 
    @param SectorCount 

    @retval EFI_STATUS

**/

EFI_STATUS 
EFIAPI
SmartWriteData (
    IN AMI_HDD_SMART_PROTOCOL *This,
    IN UINT8                  SubCommand,
    IN VOID                   *Buffer,
    IN UINT8                  LogAddress,
    IN UINT8                  SectorCount
)
{
    EFI_STATUS                  Status = EFI_UNSUPPORTED;
    COMMAND_STRUCTURE           CommandStructure;
    VOID                        *BusInterface = ((AMI_SMART_INTERFACE * )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE   BusInterfaceType = ((AMI_SMART_INTERFACE * )This)->BusInterfaceType;
     BOOLEAN                     MultipleSectors = FALSE;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = \
                                   (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = \
                                    (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

    if( BusInterfaceType == AmiStorageInterfaceNvme ) {
        return EFI_UNSUPPORTED;
    }

    if(Buffer== NULL)
        return EFI_INVALID_PARAMETER;
    ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Buffer      = Buffer;
    CommandStructure.Features    = SubCommand;
    CommandStructure.ByteCount   = SectorCount * ATA_SECTOR_BYTES;
    CommandStructure.SectorCount = SectorCount;
    CommandStructure.LBALow      = LogAddress;       //LOG address to write on
    CommandStructure.LBAMid      = 0x4F;
    CommandStructure.LBAHigh     = 0xC2;
    CommandStructure.Command     = SMART_COMMAND;

    if ( SectorCount>1 )
        MultipleSectors = TRUE;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        Status = IdeBusInterface->AtaPioDataOut ( 
                                       IdeBusInterface,
                                       CommandStructure.Buffer,
                                       CommandStructure.ByteCount, 
                                       CommandStructure.Features,
                                       CommandStructure.SectorCount, 
                                       CommandStructure.LBALow, 
                                       CommandStructure.LBALowExp, 
                                       CommandStructure.LBAMid, 
                                       CommandStructure.LBAMidExp, 
                                       CommandStructure.LBAHigh,
                                       CommandStructure.LBAHighExp, 
                                       CommandStructure.Device, 
                                       CommandStructure.Command,
                                       TRUE, 
                                       MultipleSectors ); 
    } 
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if (BusInterfaceType == AmiStorageInterfaceAhci) {
        Status = SataDevInterface->AhciBusInterface->SataPioDataOut (
                                                 SataDevInterface, 
                                                 CommandStructure,
                                                 TRUE ); 
    }
#endif
    
    return Status;
}

/**
    Common Hook which Calls Non Data Command.

    @param BusInterface 
    @param CommandStructure 
    @param BusInterfaceType 

    @retval EFI_STATUS

**/
EFI_STATUS 
CommonNonDataHook (
    IN VOID                         *BusInterface,
    IN COMMAND_STRUCTURE            CommandStructure,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
)
{
    EFI_STATUS               Status = EFI_UNSUPPORTED;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface =\
                                 (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
   
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = \
                                 (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        CommandStructure.Device = IdeBusInterface->IdeDevice.Device << 4;
        //
        // IdeBus API changed from Core 4.6.5.2. Added Core Version check for the OLD 
        // Core support.
        //
        Status = IdeBusInterface->IdeNonDataCommand(\
                                      IdeBusInterface,
                                      CommandStructure.Features,
                                      (UINT8)CommandStructure.SectorCount,
                                      (UINT8)(CommandStructure.SectorCount >> 8),
                                      CommandStructure.LBALow,
                                      CommandStructure.LBALowExp,
                                      CommandStructure.LBAMid,
                                      CommandStructure.LBAMidExp,
                                      CommandStructure.LBAHigh,
                                      CommandStructure.LBAHighExp,
                                      CommandStructure.Device,
                                      CommandStructure.Command
                                      );
    } 
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        Status = SataDevInterface->AhciBusInterface->ExecuteNonDataCommand(
                                                         SataDevInterface,
                                                         CommandStructure 
                                                         );
    }
#endif
    return Status;
}

/**
    Event's CallBack function which will call Wrapper Function to Report
    SMART failure Error Message.

    @param  EFI_EVENT Event 
    @param  VOID *Context - Interface.  
         
    @retval VOID
    
**/
VOID
EFIAPI
ReportSmartFailureCallBack (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    SMART_ERROR_DEVINFO   *SmartErrorDevInfo = (SMART_ERROR_DEVINFO *)Context;

    // Calls the Wrapper Function to display the SmartFailure Error Message.
    ReportSmartFailure(SmartErrorDevInfo->BusInterfaceType,
                       SmartErrorDevInfo->BusInterface);
    pBS->FreePool(SmartErrorDevInfo);
    pBS->CloseEvent(Event);
}
/**
    Wrapper Function to call a ELINKed function

    @param  AMI_STORAGE_BUS_INTERFACE BusInterfaceType 
            0 - Device is in IDE mode
            1 - Device is in AHCI mode
    @param  VOID *Interface - Interface.  
          Should be cast into SATA_DEVICE_INTERFACE
          or AMI_IDE_BUS_PROTOCOL depending on BusInterfaceType parameter

    @retval VOID
    @retval VOID

**/

VOID
ReportSmartFailure (
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType,
    VOID                            *Interface
)
{
    OemReportSmartFailureFunction(BusInterfaceType, Interface);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
