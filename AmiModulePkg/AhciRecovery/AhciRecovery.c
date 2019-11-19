//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AhciRecovery.c
    Implements EFI_PEI_RECOVERY_BLOCK_IO_PPI for Ahci devices.

**/

//---------------------------------------------------------------------------

#include    "AhciRecovery.h"

//---------------------------------------------------------------------------

EFI_GUID    gPeiAhciController2PpiGuid      = PEI_AHCI_CONTROLLER_2_PPI_GUID;
EFI_GUID    guidEndOfPei                    = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;
EFI_GUID    gPeiVirtualBlockIoInstallPpiGuid = PEI_VIRTUAL_BLOCKIO_INSTALL_PPI_GUID;
#if OFFBOARD_AHCI_RECOVERY_SUPPORT
AHCI_PCI_BRIDGE_INFO             *gAhciPciBridgeInfo[AHCI_MAXIMUM_PCI_ROOT_BRIDGE]={NULL};
AHCI_PCI_PROGRAMMED_BRIDGE_INFO  *gAhciPciProgrammedBridgeInfo[AHCI_MAXIMUM_PCI_ROOT_BRIDGE]={NULL};
AHCI_PCI_DEVICE_INFO             *gAhciPciDeviceInfo[MAX_AHCI_CONTROLLER_DEVICE]={NULL};
#endif
AHCI_RECOVERY_CONTROLLER_INFO    *gAhciControllerInfo[MAX_AHCI_CONTROLLER_DEVICE]={NULL};
EFI_DEVICE_PATH                  *gAcpiDp;
PCI_DEVICE_PATH                  gPeiPciPath = {
                                              { HW_PCI_DP,
                                                HARDWARE_DEVICE_PATH,
                                                sizeof( PCI_DEVICE_PATH )
                                              },
                                              0,
                                              0};

SATA_DEVICE_PATH                 gPeiSataPath = {
                                               { MESSAGING_DEVICE_PATH,
                                                 MSG_SATA_DP,
                                                 sizeof( SATA_DEVICE_PATH )             
                                               },
                                               0,
                                               0,
                                               0};

// PEI Recovery Block I/O PPI
EFI_PEI_PPI_DESCRIPTOR   Ahci_PpiDescriptor = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiVirtualBlockIoPpiGuid,
    NULL
};

// PEI Recovery Block I/O PPI2
EFI_PEI_PPI_DESCRIPTOR   Ahci_PpiDescriptor2 = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiVirtualBlockIo2PpiGuid,
    NULL
};

static
EFI_PEI_NOTIFY_DESCRIPTOR ANotifyList = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &guidEndOfPei,
    NotifyOnAhciRecoveryCapsuleLoaded
};

static 
EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK, \
      &gEfiPeiBootInRecoveryModePpiGuid, InstallPeiRecoveryBlkIoPpi },
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gPeiVirtualBlockIoInstallPpiGuid, InstallPeiRecoveryBlkIoPpi },
};

static EFI_DEVICE_PATH EndOfDevicePathNode = {
       END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
    {sizeof(EFI_DEVICE_PATH),0}
};
EFI_PEI_SERVICES     **gPeiServices = NULL;
EFI_PEI_STALL_PPI    *gStallPpi     = NULL;
EFI_PEI_PCI_CFG2_PPI *gPciCfg = NULL;

UINT8    gAhciControllerCount   = 0;
UINT32   gAhciMmioBaseAddress   = 0;
UINT32   gAbarLength            = 0;
UINT8    gFreeBusNumber         = 0;
UINT64   gMaskLastNibble        = 0;
UINT64   gMaskAddress           = 0;
BOOLEAN  gAssignMmioTopDown     = FALSE;
BOOLEAN  gAbarType64Bit         = FALSE;

#if OFFBOARD_AHCI_RECOVERY_SUPPORT
UINT8    gExtAhciPciDeviceCount = 0;
UINT8    gPciBridgeCount        = 0;
#endif

/**
    GetNumberOfBlockDevices function of
    EFI_PEI_RECOVERY_BLOCK_IO2_PPI.This Returns the number of
    Block Devices present in the System.

    @param PeiServices 
    @param This 

    @retval NumberBlockDevices

**/
EFI_STATUS
EFIAPI
Ahci_GetNumberOfBlockDevices2 (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO2_PPI   *This,
    OUT UINTN                           *NumberBlockDevices
)
{
    AHCI_RECOVERY_BLK_IO_DEV    *Ahci_BlkIoDev = NULL;
    EFI_STATUS                  Status;
    
    if ( This == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    Ahci_BlkIoDev = BASE_CR(This ,AHCI_RECOVERY_BLK_IO_DEV, RecoveryBlkIo2);

    Status = Ahci_GetNumberOfBlockDevices( PeiServices, 
                                           &Ahci_BlkIoDev->RecoveryBlkIo, 
                                           NumberBlockDevices );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    DEBUG ((DEBUG_BLKIO, "\n Ahci_GetNumberOfBlockDevices2 - Number of Ahci Devices found :%x\n",Ahci_BlkIoDev->DeviceCount ));
    *NumberBlockDevices = Ahci_BlkIoDev->DeviceCount;

    return EFI_SUCCESS;
}

/**
    GetNumberOfBlockDevices function of
    EFI_PEI_RECOVERY_BLOCK_IO_PPI.This Returns the number of
    Block Devices present in the System.

    @param PeiServices 
    @param This 

    @retval NumberBlockDevices

**/
EFI_STATUS
EFIAPI
Ahci_GetNumberOfBlockDevices (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    OUT UINTN                           *NumberBlockDevices
)
{
    AHCI_RECOVERY_BLK_IO_DEV    *Ahci_BlkIoDev = NULL;
    EFI_STATUS                  Status;
    
    if ( This == NULL ) {
        return EFI_INVALID_PARAMETER;
    }
    Ahci_BlkIoDev = (AHCI_RECOVERY_BLK_IO_DEV*)This;

    if ( !Ahci_BlkIoDev->HaveEnumeratedDevices ) {
        Status =  AhciEnumerateDevices( PeiServices, Ahci_BlkIoDev);
        if(EFI_ERROR(Status)) {
            return Status;
        }
        Ahci_BlkIoDev->HaveEnumeratedDevices = TRUE;
    }

    DEBUG ((DEBUG_BLKIO, "\n Ahci_GetNumberOfBlockDevices - Number of Ahci Devices found :%x\n",Ahci_BlkIoDev->DeviceCount ));
    *NumberBlockDevices = Ahci_BlkIoDev->DeviceCount;

    return EFI_SUCCESS;
}

/**
    GetBlockDeviceMediaInfo function of
    EFI_PEI_RECOVERY_BLOCK_IO2_PPI.This returns the Block IO
    Media Information.

    @param PeiServices 
    @param This 
    @param DeviceIndex 

    @retval MediaInfo

**/
EFI_STATUS
EFIAPI
Ahci_GetBlockDeviceMediaInfo2 (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO2_PPI   *This,
    IN  UINTN                           DeviceIndex,
    OUT EFI_PEI_BLOCK_IO2_MEDIA          *MediaInfo
)
{
    AHCI_RECOVERY_BLK_IO_DEV    *Ahci_BlkIoDev = NULL;
    AHCI_RECOVERY_DEVICE_INFO   *AhciRecDeviceInfo = NULL;
    EFI_STATUS                  Status;
    
    if ((This == NULL) || (MediaInfo == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    Ahci_BlkIoDev = BASE_CR(This ,AHCI_RECOVERY_BLK_IO_DEV, RecoveryBlkIo2);
    AhciRecDeviceInfo = Ahci_BlkIoDev->DeviceInfo[DeviceIndex];

    if ( !Ahci_BlkIoDev->HaveEnumeratedDevices ) {
        Status = Ahci_GetBlockDeviceMediaInfo(PeiServices, 
                                &Ahci_BlkIoDev->RecoveryBlkIo,
                                DeviceIndex, 
                                &AhciRecDeviceInfo->MediaInfo);
        if (EFI_ERROR(Status)) {
                return Status;
        }
    }
    
    *MediaInfo = AhciRecDeviceInfo->MediaInfo2;
    return EFI_SUCCESS;
}

/**
    GetBlockDeviceMediaInfo function of
    EFI_PEI_RECOVERY_BLOCK_IO_PPI.This returns the Block IO
    Media Information.

    @param PeiServices 
    @param This 
    @param DeviceIndex 

    @retval MediaInfo

**/
EFI_STATUS
EFIAPI
Ahci_GetBlockDeviceMediaInfo (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    IN  UINTN                           DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA          *MediaInfo
)
{
    AHCI_RECOVERY_BLK_IO_DEV    *Ahci_BlkIoDev = NULL;
    AHCI_RECOVERY_DEVICE_INFO   *AhciRecDeviceInfo;
    EFI_STATUS                  Status;
    
    if ((This == NULL) || (MediaInfo == NULL)) {
        return EFI_INVALID_PARAMETER;
    }
    Ahci_BlkIoDev = (AHCI_RECOVERY_BLK_IO_DEV*)This;

    if ( !Ahci_BlkIoDev->HaveEnumeratedDevices ) {
        Status = AhciEnumerateDevices( PeiServices, Ahci_BlkIoDev);
        if ( EFI_ERROR( Status )) {
            return Status;                 
        }
        Ahci_BlkIoDev->HaveEnumeratedDevices= TRUE;
    }

    if ( DeviceIndex > (Ahci_BlkIoDev->DeviceCount - 1)) {
        return EFI_INVALID_PARAMETER;
    }
    AhciRecDeviceInfo = Ahci_BlkIoDev->DeviceInfo[DeviceIndex];
    if ( !AhciRecDeviceInfo->LookedForMedia ) {
        if (AhciRecDeviceInfo->RecoveryDeviceType  == ATAPI) {
            AhciAtapiDetectMedia( PeiServices,AhciRecDeviceInfo);
        }
        AhciRecDeviceInfo->LookedForMedia = TRUE;
    }
    *MediaInfo = AhciRecDeviceInfo->MediaInfo;
    return EFI_SUCCESS;
}

/**
    ReadBlocks function of EFI_PEI_RECOVERY_BLOCK_IO2_PPI.

    @param  PeiServices
    @param  This
    @param  DeviceIndex
    @param  StartLba
    @param  BufferSize

    @retval Buffer
**/
EFI_STATUS
EFIAPI
Ahci_ReadBlocks2 (
    IN  EFI_PEI_SERVICES                 **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO2_PPI    *This,
    IN  UINTN                            DeviceIndex,
    IN  EFI_PEI_LBA                      StartLba,
    IN  UINTN                            BufferSize,
    OUT VOID                             *Buffer
)
{
    EFI_STATUS                  Status         = EFI_SUCCESS;
    AHCI_RECOVERY_BLK_IO_DEV    *Ahci_BlkIoDev  = NULL;
    AHCI_RECOVERY_DEVICE_INFO   *AhciRecDeviceInfo = NULL;

    if ( This == NULL ) {
        return EFI_INVALID_PARAMETER;
    }
   
    Ahci_BlkIoDev = BASE_CR(This ,AHCI_RECOVERY_BLK_IO_DEV, RecoveryBlkIo2);
    AhciRecDeviceInfo = Ahci_BlkIoDev->DeviceInfo[DeviceIndex];
    
    Status = Ahci_ReadBlocks( PeiServices, 
                              &Ahci_BlkIoDev->RecoveryBlkIo, 
                              DeviceIndex,
                              StartLba, 
                              BufferSize, 
                              Buffer);

    DEBUG (( DEBUG_BLKIO, "Ahci_ReadBlocks2 Status :%x", Status ));
    return Status;
}

/**
    ReadBlocks function of EFI_PEI_RECOVERY_BLOCK_IO_PPI.

    @param  PeiServices
    @param  This
    @param  DeviceIndex
    @param  StartLba
    @param  BufferSize

    @retval Buffer
**/
EFI_STATUS
EFIAPI
Ahci_ReadBlocks (
    IN  EFI_PEI_SERVICES                 **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI    *This,
    IN  UINTN                            DeviceIndex,
    IN  EFI_PEI_LBA                      StartLba,
    IN  UINTN                            BufferSize,
    OUT VOID                             *Buffer
)
{
    EFI_PEI_BLOCK_IO_MEDIA      MediaInfo = {0,};
    EFI_STATUS                  Status         = EFI_SUCCESS;
    UINTN                       NumberOfBlocks = 0;
    UINTN                       BlockSize      = 0;
    AHCI_RECOVERY_BLK_IO_DEV    *Ahci_BlkIoDev  = NULL;
    AHCI_RECOVERY_DEVICE_INFO   *AhciRecDeviceInfo = NULL;
    if ( This == NULL ) {
        return EFI_INVALID_PARAMETER;
    }
    Ahci_BlkIoDev = (AHCI_RECOVERY_BLK_IO_DEV*)This;
    if ( Buffer == NULL ) {
        return EFI_INVALID_PARAMETER;
    }
    if ( BufferSize == 0 ) {
        return EFI_SUCCESS;
    }
    if ( !Ahci_BlkIoDev->HaveEnumeratedDevices ) {
        Status = AhciEnumerateDevices( PeiServices, Ahci_BlkIoDev);
        if ( EFI_ERROR( Status )) {
            return Status;
        }
        Ahci_BlkIoDev->HaveEnumeratedDevices = TRUE;
    }                                                                                                                                                                                                           
    AhciRecDeviceInfo = Ahci_BlkIoDev->DeviceInfo[DeviceIndex];
    if ( !AhciRecDeviceInfo->LookedForMedia ) {
        Status = Ahci_GetBlockDeviceMediaInfo(
                                    PeiServices,
                                    This,
                                    DeviceIndex,
                                    &MediaInfo
                                    );

        if ( Status != EFI_SUCCESS ) {
            return Status;
        }
    }  else {
        CopyMemPei(&MediaInfo, &(AhciRecDeviceInfo->MediaInfo), sizeof(EFI_PEI_BLOCK_IO_MEDIA));
    }

    BlockSize = MediaInfo.BlockSize;
    if ( BufferSize % BlockSize != 0 ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !MediaInfo.MediaPresent ) {
        return EFI_NO_MEDIA;
    }
    
    // Pass the Parameters
    if(  AhciRecDeviceInfo->RecoveryDeviceType == ATA ) {
        NumberOfBlocks = BufferSize / BlockSize;
        if ( StartLba > MediaInfo.LastBlock ) {
            return EFI_INVALID_PARAMETER;
        }

        if ((StartLba + NumberOfBlocks) > (MediaInfo.LastBlock + 1)) {
            return EFI_INVALID_PARAMETER;
        }
        // Read the Data from Ata devices.     
        Status = Ahci_AtaReadSectors(
                        AhciRecDeviceInfo,
                        Buffer,
                        BufferSize,
                        StartLba,
                        BlockSize
                        );
    } else {
        Status = Ahci_AtapiReadSectors(
                                AhciRecDeviceInfo,
                                Buffer,
                                BufferSize,
                                StartLba,
                                BlockSize
                                );
    }
    
    DEBUG (( DEBUG_BLKIO, "Ahci_ReadBlocks Status :%x", Status ));
    return Status;
}


/**
    Detects the ATAPI Media type.

    @param  PeiServices
    @param  AhciRecInterface

    @retval None
**/
VOID
AhciAtapiDetectMedia (
    IN  EFI_PEI_SERVICES             **PeiServices,
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo
)
{
    DetectAtapiMedia(PeiServices,AhciRecDeviceInfo);

    return;
}


/**
    Description : Initialize the SB AHCI Controller

    @param  PeiServices
    @param  AhciRecInterface

    @retval None
**/
EFI_STATUS
InitSBAhci (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV    *AhciRecInterface
)
{
    PEI_AHCI_CONTROLLER_2_PPI       *AhciControllerPpi = NULL;
    EFI_STATUS                      Status=EFI_SUCCESS;
    EFI_STATUS                      Status1=EFI_SUCCESS;    
    UINT64                          AhciBaseAddr =0;
    UINTN                           SbAhciBarAddr;
    UINT8                           SbSataBusNo =0;
    UINT8                           SbSataDeviceNo =0;
    UINT8                           SbSataFunctionNo =0;
    UINT8                           Index = 0; 
   
    // Align the MMIO address for the PCH SATA controller in Top Down approach 
    if(gAssignMmioTopDown) {
        gAhciMmioBaseAddress -= SIZE_1MB;
    }
    SbAhciBarAddr = (UINTN)gAhciMmioBaseAddress;
    DEBUG((DEBUG_BLKIO, "\n InitSBAhci :gAhciMmioBaseAddress is %x ",SbAhciBarAddr));
    
    // Initializing the OnBoard SATA Controller mode to AHCI
    do
    {
        if(gAhciControllerCount >= MAX_AHCI_CONTROLLER_DEVICE) {
            Status = EFI_OUT_OF_RESOURCES;
            ASSERT_PEI_ERROR(PeiServices, Status);
            return Status;
        }        
        Status = (**PeiServices).LocatePpi( PeiServices, 
                                            &gPeiAhciController2PpiGuid, 
                                            Index, 
                                            NULL, 
                                            &AhciControllerPpi );
        if(EFI_ERROR(Status)) {
           break;
        }
        
        // Initialize the SB SATA controller to AHCI mode
        Status1 = AhciControllerPpi->GetSbSataLocation(PeiServices,
                                                        AhciControllerPpi,
                                                        &SbSataBusNo,
                                                        &SbSataDeviceNo,
                                                        &SbSataFunctionNo);
        if(EFI_ERROR(Status1)) {
            continue; 
        }
        //Before Programming ABAR. Reading ABAR value to check below, 
        //1. ABAR type is 64 bit or 32 bit
        //2. ABAR is Zero or Non-Zero value.
        Status1 = gPciCfg->Read(PeiServices, \
                                gPciCfg, \
                                EfiPeiPciCfgWidthUint64, \
                                PEI_PCI_CFG_ADDRESS (
                                    SbSataBusNo,
                                    SbSataDeviceNo,
                                    SbSataFunctionNo, 
                                    ABAR_OFFSET),\
                                &AhciBaseAddr);
        if(EFI_ERROR(Status1)) {
            continue; 
        }

        DEBUG ((DEBUG_BLKIO, "\nInitSBAhci :Initial ABAR value is :%lx \n",AhciBaseAddr));
        
        // Checking Whether ABAR is 64Bit Address type        
        if (AhciBaseAddr & BIT2 )  {   
            gMaskLastNibble = MASK_MEM_DECODE_RANGE_64BIT;
            gMaskAddress    = AHCI_64BIT_BAR_MASK;
            gAbarType64Bit  = TRUE;
            
            // Enter Else Part if ABAR is 32Bit Address type
        } else {
            gMaskLastNibble = MASK_MEM_DECODE_RANGE;
            gMaskAddress    = AHCI_DWORD_BAR_MASK;
            gAbarType64Bit  = FALSE;
        }  
        
        //Before Programming ABAR, Check Whether ABAR is Zero.
        if (!(AhciBaseAddr & gMaskLastNibble )) {                       
                        
            // Find the Device Decode Range of the PCH SATA controller
            gAbarLength= FindDeviceDecodeRange(SbSataBusNo,SbSataDeviceNo,SbSataFunctionNo);
           
            // Programming ABAR address for the obtained PCH SATA
            AhciBaseAddr  = (UINT64)SbAhciBarAddr ;
            Status1 = gPciCfg->Write (PeiServices, \
                                      gPciCfg, \
                                      gAbarType64Bit ? EfiPeiPciCfgWidthUint64 : EfiPeiPciCfgWidthUint32, \
                                      PEI_PCI_CFG_ADDRESS (
                                              SbSataBusNo,
                                              SbSataDeviceNo,
                                              SbSataFunctionNo, 
                                              ABAR_OFFSET),\
                                       &AhciBaseAddr);
           
         if(EFI_ERROR(Status1)) {
                continue; 
            }
        } else {
            // If ABAR is having value Proceeding with it.
            AhciBaseAddr  = (AhciBaseAddr & gMaskLastNibble);
            gAbarLength   = 0;
        }    
        
        DEBUG ((DEBUG_BLKIO, "\nInitSBAhci :After Programming ABAR is: %lx ",AhciBaseAddr));

        // Initialize the SB SATA controller to AHCI mode
        Status1 = AhciControllerPpi->RecInitSbSataController(PeiServices,
                                                              AhciControllerPpi,
                                                              (UINTN)AhciBaseAddr);
        if(EFI_ERROR(Status1)) {
            continue; 
        }
        
        // Allocate memory for the Controller
        Status1 = (**PeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)PeiServices,
                                                  sizeof(AHCI_RECOVERY_CONTROLLER_INFO),
                                                  (VOID**)&gAhciControllerInfo[gAhciControllerCount]);
        ASSERT_PEI_ERROR (PeiServices, Status1);
        if(EFI_ERROR(Status1)) {
            return Status1;
        }
        (**PeiServices).SetMem( gAhciControllerInfo[gAhciControllerCount],
                                 sizeof(AHCI_RECOVERY_CONTROLLER_INFO),
                                 0 );
                     
        gAhciControllerInfo[gAhciControllerCount]->BusNumber = SbSataBusNo;  
        gAhciControllerInfo[gAhciControllerCount]->Device = SbSataDeviceNo;
        gAhciControllerInfo[gAhciControllerCount]->Function = SbSataFunctionNo;
        gAhciControllerInfo[gAhciControllerCount]->AhciBaseAddress = (UINTN)AhciBaseAddr;        
        
        //Creates hardware PCI Path for valid SATA device and appends with 
        //previously created Device Path           
        gPeiPciPath.Function = SbSataFunctionNo;
        gPeiPciPath.Device = SbSataDeviceNo;       
        gAhciControllerInfo[gAhciControllerCount]->DevPath = PeiDPAddNode(gPeiServices,gAcpiDp,&(gPeiPciPath.Header));         
        
        SbAhciBarAddr += (UINTN)gAbarLength;
        gAhciControllerCount++;
        Index++;
        
    } while(Status == EFI_SUCCESS);
    // If no PCH SATA Controller is found, reset to Initial MMIO Address value
    if(gAhciControllerInfo[0] == NULL) {
        if ( gAssignMmioTopDown )
            gAhciMmioBaseAddress += SIZE_1MB;
    } else {
        if(!gAssignMmioTopDown)
            gAhciMmioBaseAddress += SIZE_1MB;
    }
    return EFI_SUCCESS;
}

/**
    This function will calculate and return the device decode range of the SATA controller

    @param    Bus
    @param    Device
    @param    Function

    @retval    UINT32
**/
UINT32
FindDeviceDecodeRange( 
    UINT8 Bus, 
    UINT8 Device, 
    UINT8 Function
) {
    
    UINT64         DeviceDecodeRange = 0;
    
    gPciCfg->Write(gPeiServices, \
                    gPciCfg, \
                    gAbarType64Bit ? EfiPeiPciCfgWidthUint64 : EfiPeiPciCfgWidthUint32, \
                    PEI_PCI_CFG_ADDRESS (Bus,
                    Device, Function,ABAR_OFFSET),\
                    &gMaskAddress);
                                  
    gPciCfg->Read(gPeiServices, \
                   gPciCfg, \
                   gAbarType64Bit ? EfiPeiPciCfgWidthUint64 : EfiPeiPciCfgWidthUint32, \
                   PEI_PCI_CFG_ADDRESS(Bus,Device,
                   Function,ABAR_OFFSET),\
                   &DeviceDecodeRange);
          
    // Find Length of the Ahci Bar   
    DeviceDecodeRange &= gMaskLastNibble;
    DeviceDecodeRange = ~DeviceDecodeRange + 1;
    
    DEBUG ((DEBUG_BLKIO, "\n Length of ABAR is %x ",DeviceDecodeRange));
    return (UINT32)DeviceDecodeRange;
}
 
/**
    Description : Enumerates and Initializes the Offboard AHCI Controller

    @param  PeiServices
    @param  AhciRecInterface

    @retval None
**/
#if OFFBOARD_AHCI_RECOVERY_SUPPORT
EFI_STATUS
InitExtAhci (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV    *AhciRecInterface
)
{
    EFI_STATUS                      Status;
    UINT8                           SubBusNoForRootBridge = AHCI_PCI_MAX_SUBORDINATE_NUMBER;
    UINT8                           SecondaryBusNo;
    UINT8                           Index;
    UINT16                          VendorId=0;
    UINT8                           Index1;
    UINT32                          BridgeMmioBaseAddress = 0x0;
    UINT32                          BridgePreviousMemBase = 0x0;
    AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo;
    AHCI_PCI_BRIDGE_INFO            BridgeInfo;
    AHCI_PCI_ROOT_BRIDGE_INFO       RootBridgeList[] = { AHCI_ROOT_BRIDGE_LIST
                                                        {0xff,0xff,0xff}
                                                        };
    PCIE_SATA_CONTROLLER_INFO        PcieSataControllerInfo[] = { PCIE_SATA_CONTROLLER_INIT_FUNC_LIST
                                                                 {0xFFFF,0xFFFF, 0xFFFF, 0xFFFF,  NULL}
                                                              };
    EFI_DEVICE_PATH                 *ParentDp;                                                              
    UINT8                           PcieControllerInitHookCount = \
                                      sizeof( PcieSataControllerInfo) / sizeof(PCIE_SATA_CONTROLLER_INFO);
    
    BOOLEAN                         BridgeAlreadyProgrammed = FALSE;
    BOOLEAN                         DeviceFound = FALSE;
    
    if( RootBridgeList[0].Bus == 0xff && RootBridgeList[0].Dev == 0xff &&
               RootBridgeList[0].Func == 0xff ) {
        DEBUG ((DEBUG_INFO, "\n No Root bridge Entry added for Ahci device Enumeration "));
        return EFI_SUCCESS;
    }
    for( Index=0; RootBridgeList[Index].Func != 0xff ; Index++) {
        if((gPciBridgeCount >= AHCI_MAXIMUM_PCI_ROOT_BRIDGE) || (gAhciControllerCount+gExtAhciPciDeviceCount) >= MAX_AHCI_CONTROLLER_DEVICE) {
            Status = EFI_OUT_OF_RESOURCES;
            ASSERT_PEI_ERROR( gPeiServices, Status);
            return Status;
        }
        // First Check whether this bridge is already Enumerated.
        for( Index1 = 0; Index1 < gPciBridgeCount; Index1++ ) {
            if( gAhciPciBridgeInfo[Index1]->PrimaryBusNumber == RootBridgeList[Index].Bus &&\
                gAhciPciBridgeInfo[Index1]->Device == RootBridgeList[Index].Dev && \
                gAhciPciBridgeInfo[Index1]->Function == RootBridgeList[Index].Func ) {
                break;
             }
        }
        if(Index1 != gPciBridgeCount) continue;
        if((RootBridgeList[Index].Bus == 0) && \
	               (RootBridgeList[Index].Dev == 0) && \
                        (RootBridgeList[Index].Func == 0)) {
            // Enumerate Bus Zero.  
            Status = EnumerateBus ( gAcpiDp, 0, gAhciMmioBaseAddress );
            ASSERT_PEI_ERROR (PeiServices, Status);
            if ( EFI_ERROR( Status )) {
                return Status;
            }
            break;
         } else {

            DEBUG ((DEBUG_INFO, "\n Enumerating Bridge Bus:%x Dev:%x Func:%x",RootBridgeList[Index].Bus,
                    RootBridgeList[Index].Dev, RootBridgeList[Index].Func ));
            Status = gPciCfg->Read(gPeiServices, \
                                      gPciCfg, \
                                      EfiPeiPciCfgWidthUint16,\
                                      PEI_PCI_CFG_ADDRESS(RootBridgeList[Index].Bus,RootBridgeList[Index].Dev,
                                          RootBridgeList[Index].Func,PCI_VID),\
                                      &VendorId);
            if(VendorId == INVALID_VENDOR_ID ) continue;
            BridgeInfo.PrimaryBusNumber = RootBridgeList[Index].Bus;
            BridgeInfo.SecBusNumber = gFreeBusNumber;
            BridgeInfo.SubBusNumber = SubBusNoForRootBridge;
            BridgeInfo.Device = RootBridgeList[Index].Dev;
            BridgeInfo.Function = RootBridgeList[Index].Func;
            // Check whether the PCI/PCI Bridge is already Programmed
            if( CheckIsBridgeProgrammed(RootBridgeList[Index].Bus,
                                          RootBridgeList[Index].Dev,
                                          RootBridgeList[Index].Func)) {

                DEBUG ((DEBUG_BLKIO, "\n Bridge @B[%x]D[%x]F[%x] is Already Programmed ",RootBridgeList[Index].Bus,\
                                                                                           RootBridgeList[Index].Dev,
                                                                                           RootBridgeList[Index].Func));

                // Store the already Programmed Secondary & Subordinate
                // Bus no. for the root Bridge.
                gPciCfg->Read ( PeiServices, \
                                gPciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS(
                                   RootBridgeList[Index].Bus,
                                   RootBridgeList[Index].Dev, 
                                   RootBridgeList[Index].Func, 
                                   PCI_SBUS),\
                                &BridgeInfo.SecBusNumber);
                                                      
                gPciCfg->Read ( PeiServices, \
                                 gPciCfg, \
                                 EfiPeiPciCfgWidthUint8, \
                                 PEI_PCI_CFG_ADDRESS (
                                     RootBridgeList[Index].Bus,
                                     RootBridgeList[Index].Dev,
                                     RootBridgeList[Index].Func, 
                                     PCI_SUBUS),\
                                 &BridgeInfo.SubBusNumber );    
                     
                gPciCfg->Read (PeiServices, \
                                gPciCfg, \
                                EfiPeiPciCfgWidthUint16, \
                                PEI_PCI_CFG_ADDRESS (
                                     RootBridgeList[Index].Bus,
                                     RootBridgeList[Index].Dev, 
                                     RootBridgeList[Index].Func, 
                                     PCI_MEMBASE),\
                                &BridgePreviousMemBase);
                BridgeAlreadyProgrammed = TRUE;
                // Only the Upper 12 bit will be the valid Bridge MmioBase  
                BridgeMmioBaseAddress  = (BridgePreviousMemBase << 16) & 0xFFF00000;
                DEBUG ((DEBUG_BLKIO, "\n Previously Programmed Secondary Bus Number:%x, Subordinate Bus Number :%x",BridgeInfo.SecBusNumber,BridgeInfo.SubBusNumber ));
                DEBUG ((DEBUG_BLKIO, "\n BridgePreviousMemBase: %x", BridgePreviousMemBase));

             } else {
                 DEBUG ((DEBUG_BLKIO, "\n Bridge @B[%x]D[%x]F[%x] is Newly Programmed ",RootBridgeList[Index].Bus,\
                                                                                          RootBridgeList[Index].Dev,
                                                                                          RootBridgeList[Index].Func));
                 // Bridge is not Programmed before, Program the Secondary &
                 // Subordinate Bus no. for root Bridge.
                 gPciCfg->Write ( PeiServices, \
                                   gPciCfg, \
                                   EfiPeiPciCfgWidthUint8, \
                                   PEI_PCI_CFG_ADDRESS (
                                      RootBridgeList[Index].Bus,
                                      RootBridgeList[Index].Dev, 
                                      RootBridgeList[Index].Func, 
                                      PCI_SBUS),\
                                   &BridgeInfo.SecBusNumber);
                     
                 gPciCfg->Write ( PeiServices, \
                                   gPciCfg, \
                                   EfiPeiPciCfgWidthUint8, \
                                   PEI_PCI_CFG_ADDRESS (
                                      RootBridgeList[Index].Bus,
                                      RootBridgeList[Index].Dev,
                                      RootBridgeList[Index].Func, 
                                      PCI_SUBUS),\
                                   &BridgeInfo.SubBusNumber );
                 BridgeAlreadyProgrammed = FALSE;
                 if(gAssignMmioTopDown) {
                     gAhciMmioBaseAddress -= SIZE_1MB;
                 } else {
                     gAhciMmioBaseAddress += SIZE_1MB;
                 }
                 BridgeMmioBaseAddress = gAhciMmioBaseAddress;
                 gFreeBusNumber++; 
                 DEBUG ((DEBUG_BLKIO, "\n Newly Programmed Secondary Bus Number:%x, Subordinate Bus Number :%x",BridgeInfo.SecBusNumber,BridgeInfo.SubBusNumber ));
                 DEBUG ((DEBUG_BLKIO, "\n BridgePreviousMemBase: %x", BridgePreviousMemBase));
             }
             BridgeInfo.BridgeAlreadyProgrammed = BridgeAlreadyProgrammed;
             
             // Store the Bridge info for programming the Bridge's
             // above this Bridge.
             Status = (**PeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)PeiServices,
                                                     sizeof(AHCI_PCI_BRIDGE_INFO),
                                                     (VOID**)&gAhciPciBridgeInfo[gPciBridgeCount] );
             if ( EFI_ERROR( Status )) {
                 return EFI_OUT_OF_RESOURCES;
             }

             *gAhciPciBridgeInfo[gPciBridgeCount] = BridgeInfo;
             SecondaryBusNo = BridgeInfo.SecBusNumber;
             
             //Creates hardware PCI Path for PCI-PCI bridge and appends with ACPI Device Path
             gPeiPciPath.Function = BridgeInfo.Function;
             gPeiPciPath.Device = BridgeInfo.Device;
             ParentDp = PeiDPAddNode(gPeiServices,gAcpiDp,&(gPeiPciPath.Header));
             gPciBridgeCount++;
             
             // Program Subordinate for the bridge above this Bridge.
             ProgramSubordinateBusForBridgeAbove(RootBridgeList[Index].Bus,\
                                                 SecondaryBusNo);
                                
             // Enumerate the device's under this RootBrdige.
             Status = EnumerateBus( ParentDp, SecondaryBusNo, BridgeMmioBaseAddress );
             if ( EFI_ERROR( Status )) {
                 return Status;
             }

             // Program Root Bridge based on the bridge or device programmed
             // under this Bridge.
             Status = CheckforProgrammedBridgeorDevice( RootBridgeList[Index].Bus,
                                                        RootBridgeList[Index].Dev,
                                                        RootBridgeList[Index].Func,
                                                        SecondaryBusNo,
                                                        BridgeAlreadyProgrammed,
                                                        BridgeMmioBaseAddress
                                                        );
             if ( EFI_ERROR( Status )) {
                 return Status;
             }
        }
    }
    
    // check and clear Secondary and Subordinate Bus number for the Bridges under 
    // which device is not found.
    SecondaryBusNo = 0;
    for( Index1 = 0; Index1< AHCI_MAXIMUM_PCI_ROOT_BRIDGE;Index1++) {
        if(!gAhciPciBridgeInfo[Index1]){ 
            continue;
        }
        if(gAhciPciBridgeInfo[Index1]->BridgeAlreadyProgrammed) continue;
        for(Index=0,DeviceFound= FALSE; Index < MAX_AHCI_CONTROLLER_DEVICE ;Index++) {
            if( gAhciPciDeviceInfo[Index] ) {  
                if( gAhciPciDeviceInfo[Index]->BusNumber == \
                              gAhciPciBridgeInfo[Index1]->SecBusNumber ) {
                    DeviceFound = TRUE;
                    break;
                }
            }
        }
        if(!DeviceFound) {
            // No device found under this Bridge reset Sec and Subordinate 
            // Bus Number registers of P2P.
            gPciCfg->Write ( PeiServices, \
                             gPciCfg, \
                             EfiPeiPciCfgWidthUint8, \
                             PEI_PCI_CFG_ADDRESS (
                                 gAhciPciBridgeInfo[Index1]->PrimaryBusNumber,
                                 gAhciPciBridgeInfo[Index1]->Device, 
                                 gAhciPciBridgeInfo[Index1]->Function, 
                                 PCI_SBUS),\
                             &SecondaryBusNo);
                                     
             gPciCfg->Write ( PeiServices, \
                              gPciCfg, \
                              EfiPeiPciCfgWidthUint8, \
                              PEI_PCI_CFG_ADDRESS (
                                  gAhciPciBridgeInfo[Index1]->PrimaryBusNumber,
                                  gAhciPciBridgeInfo[Index1]->Device, 
                                  gAhciPciBridgeInfo[Index1]->Function, 
                                  PCI_SUBUS),\
                              &SecondaryBusNo );
        }
    }
    for ( Index = 0; Index <= gExtAhciPciDeviceCount ; Index++) {
        if(gAhciPciDeviceInfo[Index]) {
            
            if(gAhciControllerCount >= MAX_AHCI_CONTROLLER_DEVICE) {
                Status = EFI_OUT_OF_RESOURCES;
                ASSERT_PEI_ERROR( gPeiServices, Status);
                return Status;
            }
            // Allocate memory for the Controller
            Status = (**PeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)PeiServices,
                                                    sizeof(AHCI_RECOVERY_CONTROLLER_INFO),
                                                    (VOID**)&gAhciControllerInfo[gAhciControllerCount]);
            ASSERT_PEI_ERROR(PeiServices,Status);
            if(EFI_ERROR(Status)) {
                return Status;
            }

            (**PeiServices).SetMem( gAhciControllerInfo[gAhciControllerCount],
                                     sizeof(AHCI_RECOVERY_CONTROLLER_INFO),
                                     0 );
            AhciControllerInfo =  gAhciControllerInfo[gAhciControllerCount];
            AhciControllerInfo->BusNumber = gAhciPciDeviceInfo[Index]->BusNumber;
            AhciControllerInfo->Device = gAhciPciDeviceInfo[Index]->Device;
            AhciControllerInfo->Function = gAhciPciDeviceInfo[Index]->Function;
            AhciControllerInfo->AhciBaseAddress = gAhciPciDeviceInfo[Index]->BaseAddress;
            AhciControllerInfo->DevPath = gAhciPciDeviceInfo[Index]->DevPath;
            
            gAhciControllerCount++;
            
            // Check for ELink Hook to do controller Specific Initialization
            gPciCfg->Read(PeiServices, \
                          gPciCfg, \
                          EfiPeiPciCfgWidthUint16,\
                          PEI_PCI_CFG_ADDRESS(AhciControllerInfo->BusNumber,
                                  AhciControllerInfo->Device,
                                  AhciControllerInfo->Function,PCI_VID),\
                          &VendorId);
            
            for(Index1=0; Index1 < PcieControllerInitHookCount; Index1++) {
                if(PcieSataControllerInfo[Index1].VendorID  == 0xFFFF) continue;
                if(VendorId == PcieSataControllerInfo[Index1].VendorID) {  
                    PcieSataControllerInfo[Index1].AhciModeInitFunction (
                        (UINTN)(PcdGet64(PcdPciExpressBaseAddress) + \
                        PCI_LIB_ADDRESS( AhciControllerInfo->BusNumber,
                                         AhciControllerInfo->Device,
                                         AhciControllerInfo->Function,
                                         0 )));               
                }
            }
           
        }
    }
    
    return EFI_SUCCESS;
}
#endif
/**
    Description : Enumerates Ata or Atapi devices.  Called by
    Ata_GetNumberOfBlockDevices or
    
    @param  PeiServices
    @param  AhciRecInterface

    @retval None
**/

EFI_STATUS
AhciEnumerateDevices (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV    *AhciRecInterface
)
{
    UINT8                           PortEnumeration = 0xFF;     // Bit Map
    UINT8                           CurrentPort = 0, CurrentPMPort = 0xFF;
    EFI_STATUS                      Status;
    UINT8                           AtaDeviceCount = 0;
    UINT8                           Index = 0;
    AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo;
    
    //ACPI Device Path filled with Default values.  
    ACPI_HID_DEVICE_PATH            PeiAcpiPath = {
                                                   { ACPI_DEVICE_PATH,
                                                     ACPI_DP,
                                                     sizeof(ACPI_HID_DEVICE_PATH)
                                                   },
                                                   EISA_PNP_ID(0x0A03),
                                                   0};
    DEBUG ((DEBUG_INFO, "\nDevice Detection and Configuration starts\n"));

    //Creates ACPI Device Node, it is the First Node of the Device Path and 
    //Storing its Address to the Global Pointer.    
    gAcpiDp = PeiDPAddNode( gPeiServices, NULL, (EFI_DEVICE_PATH*)&PeiAcpiPath);
    // Other Recovery Driver's will do RootBridge Enumeration, so consider
    // from the Previously Programmed Bridge MMIO Address and Free Secondary Bus 
    // Number from PCD values.
    
    PcdAhciRootBridgeResources(FALSE);
    
    // Initialize the PCH SATA Controller
    Status = InitSBAhci(PeiServices, AhciRecInterface);
    if ( EFI_ERROR( Status )) {
        return Status;
    }
    // Enumerate and Initialize the Offboard SATA Controller
#if OFFBOARD_AHCI_RECOVERY_SUPPORT   
    Status= InitExtAhci(PeiServices, AhciRecInterface);
    if ( EFI_ERROR( Status )) {
        return Status;
    }
#endif
    Status = (**PeiServices).LocatePpi( PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, &gStallPpi );
    if ( EFI_ERROR( Status )) {
        return EFI_DEVICE_ERROR;
    }   
    // Setting the PCD value so as to avoid the Root Bridge Resource Conflict in other Recovery Driver's
    PcdAhciRootBridgeResources(TRUE);
    
    // Initialize the SATA Controller
    for(Index=0; Index < MAX_AHCI_CONTROLLER_DEVICE ;Index++) {
        if( gAhciControllerInfo[Index] ) {
            
            // Initialize the SATA Controller
            Status = AhciInitController(PeiServices, gAhciControllerInfo[Index]);

            DEBUG ((DEBUG_INFO, "\n AhciInitController Status for the SATA Controller at Bus:%x, Device :%x, Function :%x is :%r",\
                    gAhciControllerInfo[Index]->BusNumber,\
                    gAhciControllerInfo[Index]->Device,\
                    gAhciControllerInfo[Index]->Function,
                    Status));

            if(EFI_ERROR(Status)) {
                 continue;
            }
            gAhciControllerInfo[Index]->HBAPortImplemented = HBA_REG32(gAhciControllerInfo[Index]->AhciBaseAddress, HBA_PI);
            if (!gAhciControllerInfo[Index]->HBAPortImplemented) {
                continue;
            }
            PortEnumeration = gAhciControllerInfo[Index]->HBAPortImplemented;

           //--------------------------------------------------------------------------
           //  Detection and Configuration starts
           //--------------------------------------------------------------------------
           for ( CurrentPort = 0 ; PortEnumeration != 0 ; PortEnumeration >>= 1, CurrentPort++, CurrentPMPort = 0xFF) {

               if(!(PortEnumeration & 1)) {
                    continue;
               }

               // Check if the current port is implemented or not?
               Status = CheckPortImplemented(gAhciControllerInfo[Index], CurrentPort);
               if (EFI_ERROR(Status)) {
                   continue;
               }

               // Detect and Configure the ATA and ATAPI devices
               Status = DetectAndConfigureDevice(PeiServices, AhciRecInterface, gAhciControllerInfo[Index], CurrentPort, CurrentPMPort);
               DEBUG ((DEBUG_INFO, "\nDetectAndConfigureDevice Status at Port:%x and PMPort:%x is %r\n", CurrentPort,CurrentPMPort, Status));
            }
        }  
    }
    // Register a Notification function that free Up the resources allocated for Controller at the End of PEI phase
    (**PeiServices).NotifyPpi (PeiServices, &ANotifyList);
    
    for ( Index = 0; Index < AhciRecInterface->DeviceCount; Index++  )
    {
        AhciRecDeviceInfo = AhciRecInterface->DeviceInfo[Index];
        
        //Creates Sata Path for the valid Device Detected and 
        //appends with previously created Device Path.
        gPeiSataPath.HBAPortNumber = AhciRecDeviceInfo->PortNo;
        gPeiSataPath.PortMultiplierPortNumber = AhciRecDeviceInfo->PMPortNo == 0xFF ? 0xFFFF : AhciRecDeviceInfo->PMPortNo;
        gPeiSataPath.Lun = 0;
        AhciRecDeviceInfo->DevicePath = PeiDPAddNode (gPeiServices,AhciRecDeviceInfo->AhciControllerInfo->DevPath,&(gPeiSataPath.Header));      

        if(AhciRecDeviceInfo->RecoveryDeviceType == ATA) { 
            AtaDeviceCount++;
        }
        
    }

    DEBUG ((DEBUG_INFO, "\nNumber of AHCI ATA Device Found:%d \n", AtaDeviceCount));
    DEBUG ((DEBUG_INFO, "\nNumber of AHCI ATAPI Device Found: %d \n", (AhciRecInterface->DeviceCount - AtaDeviceCount)));

    return EFI_SUCCESS;
}

/**
    Routine to Get or Set the Root Bridge Resources from the PCD's
    AmiPcdPeiPciMmioTopAddress and AmiPcdPeiPciEnumerationLastBusNumber

    @param  Flag

    @retval NONE
**/

VOID
PcdAhciRootBridgeResources(
    BOOLEAN Flag
) 
{
    UINT64          PciMmioStartAddress = 0;
    if(Flag) {
        PcdSet8(AmiPcdPeiPciEnumerationLastBusNumber,(gFreeBusNumber-1));
        PcdSet64(AmiPcdPeiPciMmioTopAddress,gAhciMmioBaseAddress);
    } else {
        if( !PcdGet64(AmiPcdPeiPciMmioTopAddress) ) {
            FindPciMmioAddress(&PciMmioStartAddress);
            PcdSet64( AmiPcdPeiPciMmioTopAddress, PciMmioStartAddress);
        }
        gAhciMmioBaseAddress = (UINT32) PcdGet64(AmiPcdPeiPciMmioTopAddress);
        gFreeBusNumber = PcdGet8(AmiPcdPeiPciEnumerationLastBusNumber)+ 1;
        if(gAhciMmioBaseAddress <=  PcdGet64(PcdPciExpressBaseAddress)) {
            gAssignMmioTopDown = TRUE;
        }
    }
}

/**
    Routine to find the start of the MMIO address that can be assigned for the 
    PCI device in Pei Phase.

    @param  PciMmioStartAddress

    @retval NONE
    
    @notes 1) Initially consider the MMIO region below the PCIE_BASE_ADDRESS.
    This routine will check Resource descriptor HOB and finds 
    if the selected MMIO region falls in Resource descriptor Hob's memory range.
    2) If the selected region is in Hob's memory range, means its reserved memory
    and can't be configured used as MMIO so select MMIO from PCIE_BASE_ADDRESS + PCIE_LENGTH. 
**/
VOID
FindPciMmioAddress(
    UINT64              *PciMmioStartAddress
)
{
    EFI_PEI_HOB_POINTERS                          Hob;
    EFI_HOB_RESOURCE_DESCRIPTOR                   *ResourceHob;
    EFI_PHYSICAL_ADDRESS                          MmioBaseAddress;
    
    *PciMmioStartAddress = PcdGet64(PcdPciExpressBaseAddress);
    // Try to find the Memory region that can be used as MMIO for PCI Devices. 
    // Check Resource descriptor HOB to check whether the MMIO selected
    // is being used as Reserved memory or System memory.
    MmioBaseAddress = PcdGet64(PcdPciExpressBaseAddress) - SIZE_1MB;
    Hob.Raw = GetFirstHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR);
    ASSERT_PEI_ERROR (gPeiServices, Hob.Raw != NULL);
    while ((Hob.Raw != NULL) && (!END_OF_HOB_LIST (Hob))) {
        ResourceHob = Hob.ResourceDescriptor;
        // check whether the selected MMIO falls within Resource descriptor memory range.
        if ( ResourceHob->PhysicalStart <= MmioBaseAddress &&
                ResourceHob->PhysicalStart + ResourceHob->ResourceLength >= MmioBaseAddress ) {

            DEBUG ((DEBUG_BLKIO, "\n PcdPciExpressBaseAddress below region can't be used as MMIO, its reserved Memory!! "));
#ifdef PCIEX_LENGTH

            *PciMmioStartAddress = PcdGet64(PcdPciExpressBaseAddress) + PCIEX_LENGTH;
#else
            DEBUG ((DEBUG_ERROR, "\nPCIEX_LENGTH is not defined"));
            ASSERT (FALSE);
#endif
            break;
        }
        Hob.Raw = GET_NEXT_HOB (Hob);
        Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw);
    }
    DEBUG ((DEBUG_BLKIO, "\n Selected Mmio Address :%lx ",*PciMmioStartAddress ));
    return; 
}

/**
    Routine to initialize Bridge/Device detected and configured when enumerating the Specific
    PCI to PCI bridge

    @param  Bus 
    @param  Device 
    @param  Function 
    @param  SecondaryBusNo 
    @param  BridgeAlreadyProgrammed
    @param  BridgeMmioBaseAddress
    
    @retval Status
**/
#if OFFBOARD_AHCI_RECOVERY_SUPPORT
EFI_STATUS
CheckforProgrammedBridgeorDevice (
    IN  UINT8  Bus,
    IN  UINT8  Device,
    IN  UINT8  Function,
    IN  UINT8  SecondaryBusNo,
    IN  UINT8  BridgeAlreadyProgrammed,
    IN  UINT32 BridgeMmioBaseAddress
)
{
    EFI_STATUS  Status;
    UINT8       Index=0,Index1;
    UINT8       Subordinate=0;
    UINT16      Address;
    BOOLEAN     DeviceFound = FALSE;

    if( !BridgeAlreadyProgrammed ) {
        // Check the stored info and Program the Bridge, if any
        // Device/Bridge is programmed behind this.
        for(Index=0; Index < MAX_AHCI_CONTROLLER_DEVICE ;Index++) {
            if( gAhciPciDeviceInfo[Index] ) {  
                if( gAhciPciDeviceInfo[Index]->BusNumber == SecondaryBusNo ) {
                
                    // Check whether Devices found behind this bridge
                    // If found program the bridge.
                    if( gAhciPciDeviceInfo[Index]->IsMmioDevice ) {
                        // Programm Memlimit, Membase based on Base address exact width
                        Address = (UINT16)(BridgeMmioBaseAddress >>16);
                    }
                    Status = ProgramPciBridge(Bus,Device,Function,Address,gAhciPciDeviceInfo[Index]->IsMmioDevice);
                    DEBUG ((DEBUG_BLKIO, "\n MemBase Value Programmed for Bridge @B[%x]D[%x]F[%x] is :%x ",Bus, Device, Function, Address ));
                    if ( EFI_ERROR( Status )) {
                        return Status;
                    }
                    DeviceFound = TRUE;
                }
                break;
            }
        }
    } 
    // Program the Bridge if any Bridge behind this programmed.
    for(Index=0; Index < AHCI_MAXIMUM_PCI_ROOT_BRIDGE ;Index++) {
        if( gAhciPciProgrammedBridgeInfo[Index] ) {
            if(SecondaryBusNo == gAhciPciProgrammedBridgeInfo[Index]->PrimaryBusNumber) {
                DeviceFound = TRUE;
                Status = ProgramPciBridge(Bus,Device,Function,gAhciPciProgrammedBridgeInfo[Index]->MemIOBaseLimit,
                        gAhciPciProgrammedBridgeInfo[Index]->IsMMIO);
                if ( EFI_ERROR( Status )) {
                    return Status;
                }
            }
        }
    }
    // No SATA device found under this bridge or child bridge so restore to the previous value.
    if(!DeviceFound  && !BridgeAlreadyProgrammed ) {
        if(gAssignMmioTopDown) {
            gAhciMmioBaseAddress += SIZE_1MB;
        } else {
            gAhciMmioBaseAddress -= SIZE_1MB;
        }
    }
    // Program the Sub-ordinate no. for the bridge based on the bridge's programmed behind this.
    for(Index=0; Index<AHCI_MAXIMUM_PCI_ROOT_BRIDGE ;Index++) {
        if( gAhciPciBridgeInfo[Index] ) {
            if(SecondaryBusNo == gAhciPciBridgeInfo[Index]->PrimaryBusNumber) {
            
                if(!Subordinate) {
                    Subordinate = gAhciPciBridgeInfo[Index]->SubBusNumber;
                } else {
                    if ( gAhciPciBridgeInfo[Index]->SubBusNumber > Subordinate) {
                        Subordinate = gAhciPciBridgeInfo[Index]->SubBusNumber;
                    }
                }
            }
        }
    }

    if(Subordinate != 0) {
        for(Index1=0; Index1 < AHCI_MAXIMUM_PCI_ROOT_BRIDGE ;Index1++) {
            if( gAhciPciBridgeInfo[Index1] ) {
                if(SecondaryBusNo == gAhciPciBridgeInfo[Index1]->SecBusNumber) {
                    gAhciPciBridgeInfo[Index1]->SubBusNumber = Subordinate;
                }
            }
        }
     } else {
         // If no bridge is programmed behind this bridge, then assign Sec. no as Sub no.
         for(Index1=0; Index1 < AHCI_MAXIMUM_PCI_ROOT_BRIDGE ;Index1++) {
             if( gAhciPciBridgeInfo[Index1] ) {
                 if((gAhciPciBridgeInfo[Index1]->PrimaryBusNumber == Bus) && (gAhciPciBridgeInfo[Index1]->Device == Device)
                     && (gAhciPciBridgeInfo[Index1]->Function == Function)) {
                     Subordinate = gAhciPciBridgeInfo[Index1]->SecBusNumber;
                 }
             }
         }
     }

    gPciCfg->Write ( gPeiServices, \
                     gPciCfg, \
                     EfiPeiPciCfgWidthUint8, \
                     PEI_PCI_CFG_ADDRESS (Bus,
                     Device, Function, PCI_SUBUS),\
                     &Subordinate);
    DEBUG ((DEBUG_BLKIO, "\n Subordinate Bus Number Programmed for Bridge @B[%x]D[%x]F[%x] is :%x ",Bus, Device, Function, Subordinate ));
    return EFI_SUCCESS;
}

/**
    Routine to Check whether MEM base, MEM limit of the specific
    PCI to PCI bridge is programmed already

    @param  Bus 
    @param  Device 
    @param  Function 
    @param  Address 
    @param  IsMmio

    @retval Status
**/
BOOLEAN
CheckIsBridgeProgrammed(
    IN  UINT8   Bus,
    IN  UINT8   Device,
    IN  UINT8   Function)
{
    UINT8     SecBusNoForRootBridge = 0x0;
    UINT8     SubBusNoForRootBridge = 0x0;
    UINT16    BridgePreviousMemBase = 0X0;
    // Program the Secondary & subordinate Bus no. for root Bridge.
    gPciCfg->Read ( gPeiServices, \
                     gPciCfg, \
                     EfiPeiPciCfgWidthUint8, \
                     PEI_PCI_CFG_ADDRESS (Bus,
                     Device, 
                     Function, PCI_SBUS),\
                     &SecBusNoForRootBridge);
                    
    gPciCfg->Read ( gPeiServices, \
                     gPciCfg, \
                     EfiPeiPciCfgWidthUint8, \
                     PEI_PCI_CFG_ADDRESS (Bus,
                     Device,
                     Function, PCI_SUBUS),\
                     &SubBusNoForRootBridge );
    gPciCfg->Read (gPeiServices, \
                    gPciCfg, \
                    EfiPeiPciCfgWidthUint16, \
                    PEI_PCI_CFG_ADDRESS (Bus,
                    Device, 
                    Function, 
                    PCI_MEMBASE),\
                    &BridgePreviousMemBase);
    if(SecBusNoForRootBridge && SubBusNoForRootBridge && BridgePreviousMemBase ) return TRUE;
    return FALSE;
}

/**
    Routine to initialize MEM base, Mem limit of the specific
    PCI to PCI bridge

    @param  Bus 
    @param  Device 
    @param  Function 
    @param  Address 
    @param  IsMmio

    @retval Status
**/
EFI_STATUS
ProgramPciBridge (
    IN  UINT8   Bus,
    IN  UINT8   Device,
    IN  UINT8   Function,
    IN  UINT16  Address,
    IN  BOOLEAN IsMmio
)
{
    EFI_STATUS                      Status;
    AHCI_PCI_PROGRAMMED_BRIDGE_INFO ProgrammedBridgeInfo;
    static UINT8                    ProgrammedBridgeCount=0;
    UINT8                           CmdReg;
    UINT16                          ProgrammedAddress =0;

    if( ProgrammedBridgeCount >= AHCI_MAXIMUM_PCI_ROOT_BRIDGE ) {
        Status = EFI_OUT_OF_RESOURCES;
        ASSERT_PEI_ERROR( gPeiServices, Status);
        return Status;
    }
    
    gPciCfg->Read( gPeiServices, \
                   gPciCfg, \
                   EfiPeiPciCfgWidthUint8,\
                   PEI_PCI_CFG_ADDRESS(Bus,Device,
                       Function,PCI_CMD),\
                   &CmdReg );
    CmdReg |= CMD_ENABLE_MEM;
    
    gPciCfg->Read (gPeiServices, \
                   gPciCfg, \
                   EfiPeiPciCfgWidthUint16, \
                   PEI_PCI_CFG_ADDRESS (Bus,
                   Device, Function, PCI_MEMBASE),\
                   &ProgrammedAddress);
    
    if( ( ProgrammedAddress == 0 ) || ( ProgrammedAddress > Address )) {
        gPciCfg->Write (gPeiServices, \
                    gPciCfg, \
                    EfiPeiPciCfgWidthUint16, \
                    PEI_PCI_CFG_ADDRESS (Bus,
                    Device, Function, PCI_MEMBASE),\
                    &Address);
    }
           
    gPciCfg->Write (gPeiServices, \
                    gPciCfg, \
                    EfiPeiPciCfgWidthUint16, \
                    PEI_PCI_CFG_ADDRESS (Bus,
                    Device, Function, PCI_MEMLIMIT),\
                    &Address);

    gPciCfg->Write (gPeiServices, \
                    gPciCfg, \
                    EfiPeiPciCfgWidthUint8, \
                    PEI_PCI_CFG_ADDRESS (Bus,
                    Device, Function, PCI_CMD),\
                    &CmdReg);
    
    // Store the Programmed Bridge info.
    ProgrammedBridgeInfo.PrimaryBusNumber = Bus;
    ProgrammedBridgeInfo.Device = Device;
    ProgrammedBridgeInfo.Function = Function;
    ProgrammedBridgeInfo.MemIOBaseLimit = Address;
    // Ahci Programmed Bridge is Memory mapped
    ProgrammedBridgeInfo.IsMMIO = TRUE;
    
    Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                            sizeof(AHCI_PCI_PROGRAMMED_BRIDGE_INFO),
                                            (VOID**)&gAhciPciProgrammedBridgeInfo[ProgrammedBridgeCount]);
    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }

    *gAhciPciProgrammedBridgeInfo[ProgrammedBridgeCount] = ProgrammedBridgeInfo;
    ProgrammedBridgeCount++;

    return EFI_SUCCESS;
}

/**
    Routine to initialize SubordinateBusNo for the bridge which is detected and 
    configured before the Specific PCI to PCI bridge

    @param  PrimaryBusNo 
    @param  SubordinateBusNo 

    @retval VOID
**/
VOID
ProgramSubordinateBusForBridgeAbove (
    IN  UINT8   PrimaryBusNo,
    IN  UINT8   SubordinateBusNo
)
{
    UINT8   Index;    
    DEBUG ((DEBUG_BLKIO, "\n In ProgramSubordinateBusForBridgeAbove....")); 
    for(Index=0; Index < AHCI_MAXIMUM_PCI_ROOT_BRIDGE ;Index++) {
        if( gAhciPciBridgeInfo[Index] ) {
            if(PrimaryBusNo == gAhciPciBridgeInfo[Index]->SecBusNumber) {
                gPciCfg->Write ( gPeiServices, \
                                 gPciCfg, \
                                 EfiPeiPciCfgWidthUint8, \
                                 PEI_PCI_CFG_ADDRESS (gAhciPciBridgeInfo[Index]->PrimaryBusNumber,
                                         gAhciPciBridgeInfo[Index]->Device, gAhciPciBridgeInfo[Index]->Function, PCI_SUBUS),\
                                 &SubordinateBusNo);
        DEBUG ((DEBUG_BLKIO, "\n Subordinate Bus Number Programmed for Bridge @B[%x]D[%x]F[%x] is :%x ",\
                                                        gAhciPciBridgeInfo[Index]->PrimaryBusNumber, 
                                                        gAhciPciBridgeInfo[Index]->Device, 
                                                        gAhciPciBridgeInfo[Index]->Function, 
                                                        SubordinateBusNo ));
            }
        }
    }
    return;
}

/**
    Enumerates the Bus number given and search for the Sata Controller/Drive
    and configures the Ahci device if found in the Bus
        
    @param  ParentDp 
    @param  Bus
    @param  BridgeMmioBaseAddress
    
    @retval Status
    
    @note
    1.ParentDp used to keep the track of Previous Device Path.
    2.ParentDp initially holds the Acpi Device Path and it will be appended 
    further with newly found PCI-PCI Bridge until Detecting Valid SATA Device.
**/
EFI_STATUS
EnumerateBus (
    IN  EFI_DEVICE_PATH *ParentDp,
    IN  UINT8  Bus,
    IN  UINT32 BridgeMmioBaseAddress
)
{
    EFI_STATUS           Status;
    UINT8                SecBus;
    UINT8                Device;
    UINT8                Function;
    UINT16               VendorId;
    UINT8                HeaderType;
    UINT8                RevisionId[4];
    UINT8                CmdReg;
    UINT32               MaskAddress = AHCI_DWORD_BAR_MASK;
    UINT32               DeviceDecodeRange=0;
    BOOLEAN              MultiFunc=FALSE;
    AHCI_PCI_DEVICE_INFO PciDeviceInfo;
    AHCI_PCI_BRIDGE_INFO PciBridgeInfo;
    UINT32               AhciMmioDeviceAddress = BridgeMmioBaseAddress;
    UINT32               BridgePreviousMemBase  = 0x0;
    BOOLEAN              BridgeAlreadyProgrammed = FALSE;
    EFI_DEVICE_PATH      *ChildBridgeDp;
            
    for( Device = 0;  Device <= PCI_MAX_DEVICE; Device++ ) {

        for( Function = 0;  Function <= PCI_MAX_FUNC; Function++ ) {

            DEBUG ((DEBUG_INFO, "\n AhciRecovery: Enumerating Bus:%x Dev:%x Func:%x",Bus, Device, Function));
            if((gAhciControllerCount + gExtAhciPciDeviceCount) >= MAX_AHCI_CONTROLLER_DEVICE) {
                Status = EFI_OUT_OF_RESOURCES;
                ASSERT_PEI_ERROR( gPeiServices, Status);
                return Status;
            }
            Status = gPciCfg->Read(gPeiServices, \
                          gPciCfg, \
                          EfiPeiPciCfgWidthUint16,\
                          PEI_PCI_CFG_ADDRESS(Bus,Device,
                          Function,PCI_VID),\
                          &VendorId);
            if( Function==0 && VendorId == INVALID_VENDOR_ID ) {
                break;
            } else if ( VendorId == INVALID_VENDOR_ID ) {
                continue;
            }
            gPciCfg->Read(gPeiServices, \
                          gPciCfg, \
                          EfiPeiPciCfgWidthUint8,\
                          PEI_PCI_CFG_ADDRESS(Bus,Device,
                          Function,PCI_HDR),\
                          &HeaderType);

            // Check HeaderType to find its a Multi function device
            if( Function == 0) {
                if(HeaderType & HDR_TYPE_MULTIFUNC) {
                    MultiFunc = TRUE;
                }
            }
            
            HeaderType &= 3;
            
            switch(HeaderType) {

                case HDR_TYPE_DEVICE :   
                    // Skipping Onboard Device as its handled in InitSbAhci()
                    if( Bus == 0 ) continue;
                    // PCI Target device Found
                    gPciCfg->Read(gPeiServices, \
                                  gPciCfg, \
                                  EfiPeiPciCfgWidthUint32,\
                                  PEI_PCI_CFG_ADDRESS(Bus,Device,
                                  Function,PCI_REV_ID_OFFSET),\
                                  &RevisionId);

                    // Check is it SATA PCI controller
                    if((RevisionId[3]== AHCI_CLASS_CODE) &&         // Check for MassStorage Device
                        (RevisionId[2]== AHCI_SUBCLASS_CODE)) {    // Check for Serial ATA Device
                            
                        gPciCfg->Write (gPeiServices, \
                                        gPciCfg, \
                                        EfiPeiPciCfgWidthUint32, \
                                        PEI_PCI_CFG_ADDRESS (Bus,
                                        Device, Function, PCI_BAR5),\
                                        &MaskAddress);
                        
                        gPciCfg->Read(gPeiServices, \
                                      gPciCfg, \
                                      EfiPeiPciCfgWidthUint32,\
                                      PEI_PCI_CFG_ADDRESS(Bus,Device,
                                      Function,PCI_BAR5),\
                                      &DeviceDecodeRange);
                        

                        // Ahci IO mapped IO is not supported
                        if( DeviceDecodeRange & 1 ) {
                            break;
                        }

                        // Find Length of the Ahci Bar
                        DeviceDecodeRange &= MASK_MEM_DECODE_RANGE;
                        DeviceDecodeRange = ~DeviceDecodeRange + 1;

                        // Ahci base address (31-13)
                        AhciMmioDeviceAddress &= AHCI_BAR_MASK;
                        PciDeviceInfo.BaseAddress = AhciMmioDeviceAddress;
                        PciDeviceInfo.BusNumber = Bus;
                        PciDeviceInfo.Device = Device;
                        PciDeviceInfo.Function = Function;
                        PciDeviceInfo.IsMmioDevice = TRUE;

                        //Creates hardware PCI Path for valid SATA device and 
                        //appends with previously created Device Path                   
                        gPeiPciPath.Function = Function;
                        gPeiPciPath.Device = Device;
                        PciDeviceInfo.DevPath = PeiDPAddNode(gPeiServices,ParentDp,&(gPeiPciPath.Header));
                        
                        // Program ABAR address 
                        gPciCfg->Write (gPeiServices, \
                                       gPciCfg, \
                                       EfiPeiPciCfgWidthUint32, \
                                       PEI_PCI_CFG_ADDRESS (Bus,
                                       Device, Function, PCI_BAR5),\
                                       &AhciMmioDeviceAddress);
                        
                        AhciMmioDeviceAddress = PciDeviceInfo.BaseAddress;
                        AhciMmioDeviceAddress += DeviceDecodeRange;

                        DEBUG ((DEBUG_BLKIO, "\n Found PCI Target Device with DeviceDecodeRange:%x and ABAR programmed:%x \n",DeviceDecodeRange, \
                            PciDeviceInfo.BaseAddress));

                        
                        // Allign Ahci Base address to 16bytes
                        if((AhciMmioDeviceAddress & 0xF) != 0 ) {
                            AhciMmioDeviceAddress = (AhciMmioDeviceAddress & (~0xF)) + 0x10;
                        }
                        // Set Bus Master Enable and Memory Space Enable Bit of the Command Register
                        gPciCfg->Read (gPeiServices, \
                                       gPciCfg, \
                                       EfiPeiPciCfgWidthUint8, \
                                       PEI_PCI_CFG_ADDRESS (Bus,
                                       Device, Function, PCI_CMD),\
                                       &CmdReg);
                        
                        CmdReg |= CMD_ENABLE_MEM;

                        gPciCfg->Write (gPeiServices, \
                                       gPciCfg, \
                                       EfiPeiPciCfgWidthUint8, \
                                       PEI_PCI_CFG_ADDRESS (Bus,
                                       Device, Function, PCI_CMD),\
                                       &CmdReg);
                                       
                        // Store Ahci Controller info for programming bridges above this bridge
                        Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                                 sizeof(AHCI_PCI_DEVICE_INFO),
                                                                 (VOID**)&gAhciPciDeviceInfo[gExtAhciPciDeviceCount] );
                        if ( EFI_ERROR( Status )) {
                            return EFI_OUT_OF_RESOURCES;
                        }
                        
                        *gAhciPciDeviceInfo[gExtAhciPciDeviceCount] = PciDeviceInfo;
                        gExtAhciPciDeviceCount++;
                        
                    } // if end
                    break;

                case HDR_TYPE_P2P_BRG:

                    // PCI bridge
                    if( gPciBridgeCount >= AHCI_MAXIMUM_PCI_ROOT_BRIDGE ) {
                        Status = EFI_OUT_OF_RESOURCES;
                        ASSERT_PEI_ERROR(gPeiServices, Status);
                        return Status;
                    }
                    PciBridgeInfo.PrimaryBusNumber = Bus;
                    PciBridgeInfo.Device = Device;
                    PciBridgeInfo.Function = Function;
                    PciBridgeInfo.SecBusNumber = gFreeBusNumber;
                    PciBridgeInfo.SubBusNumber = gFreeBusNumber;
                    // Check whether the PCI/PCI Bridge is already Programmed
                    if( CheckIsBridgeProgrammed(Bus,
                                                Device,
                                                Function)) {
                        DEBUG ((DEBUG_BLKIO, "\n Bridge @B[%x]D[%x]F[%x] is Already Programmed ",Bus,\
                                                                                                  Device,
                                                                                                  Function));
                        // Store the already Programmed Secondary & Subordinate Bus no.for root Bridge.
                        gPciCfg->Read ( gPeiServices, \
                                        gPciCfg, \
                                        EfiPeiPciCfgWidthUint8, \
                                        PEI_PCI_CFG_ADDRESS (Bus,
                                                Device, 
                                                Function, 
                                                PCI_SBUS),\
                                       &PciBridgeInfo.SecBusNumber);
                                                                          
                        gPciCfg->Read ( gPeiServices, \
                                        gPciCfg, \
                                        EfiPeiPciCfgWidthUint8, \
                                        PEI_PCI_CFG_ADDRESS (Bus,
                                                Device,
                                                Function, 
                                                PCI_SUBUS),\
                                        &PciBridgeInfo.SubBusNumber );    
                                         
                        gPciCfg->Read (gPeiServices, \
                                       gPciCfg, \
                                       EfiPeiPciCfgWidthUint16, \
                                       PEI_PCI_CFG_ADDRESS (Bus,
                                                Device, 
                                                Function, 
                                                PCI_MEMBASE),\
                                        &BridgePreviousMemBase);
                        BridgeAlreadyProgrammed = TRUE;
                        //Only the Upper 12 bit will be the valid P2P Bridge MemBase
                        BridgeMmioBaseAddress  = (BridgePreviousMemBase << 16) & 0xFFF00000;
                        DEBUG ((DEBUG_BLKIO, "\n Previously Programmed Secondary Bus Number:%x, Subordinate Bus Number :%x",PciBridgeInfo.SecBusNumber,PciBridgeInfo.SubBusNumber ));
                        DEBUG ((DEBUG_BLKIO, "\n BridgePreviousMemBase: %x", BridgePreviousMemBase));
                                    
                    } else {
                        // Bridge is not Programmed before, Program the Secondary & Subordinate Bus no. for root Bridge.
                       DEBUG ((DEBUG_BLKIO, "\n Bridge @B[%x]D[%x]F[%x] is Newly Programmed ",Bus,\
                                                                                                Device,
                                                                                                Function));
                        gPciCfg->Write ( gPeiServices, \
                                          gPciCfg, \
                                          EfiPeiPciCfgWidthUint8, \
                                          PEI_PCI_CFG_ADDRESS (Bus,
                                                  Device, 
                                                  Function, PCI_SBUS),\
                                          &PciBridgeInfo.SecBusNumber);
                                         
                        gPciCfg->Write ( gPeiServices, \
                                         gPciCfg, \
                                         EfiPeiPciCfgWidthUint8, \
                                         PEI_PCI_CFG_ADDRESS (Bus,
                                                 Device,
                                                 Function, PCI_SUBUS),\
                                         &PciBridgeInfo.SubBusNumber);
                        BridgeAlreadyProgrammed = FALSE;
                        if(gAssignMmioTopDown) {
                            gAhciMmioBaseAddress -= SIZE_1MB;
                        } else {
                            gAhciMmioBaseAddress += SIZE_1MB;
                        }
                        BridgeMmioBaseAddress = gAhciMmioBaseAddress;
                        gFreeBusNumber++;
                        DEBUG ((DEBUG_BLKIO, "\n Newly Programmed Secondary Bus Number:%x, Subordinate Bus Number :%x",PciBridgeInfo.SecBusNumber,PciBridgeInfo.SubBusNumber ));
                        DEBUG ((DEBUG_BLKIO, "\n BridgePreviousMemBase: %x", BridgePreviousMemBase));
                    }
                    PciBridgeInfo.BridgeAlreadyProgrammed = BridgeAlreadyProgrammed;
                    // Store Bridge info for programming Bridge's above this Bridge.
                    Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                             sizeof(AHCI_PCI_BRIDGE_INFO),
                                                             (VOID**)&gAhciPciBridgeInfo[gPciBridgeCount]);
                    if ( EFI_ERROR( Status )) {
                        return EFI_OUT_OF_RESOURCES;
                    }
                      
                    //Creates hardware PCI Path for PCI-PCI bridge and 
                    //appends with previously created Device Path                                          
                    gPeiPciPath.Function =  PciBridgeInfo.Function;
                    gPeiPciPath.Device = PciBridgeInfo.Device;                                        
                    ChildBridgeDp = PeiDPAddNode(gPeiServices,ParentDp,&(gPeiPciPath.Header)); 
                    
                    *gAhciPciBridgeInfo[gPciBridgeCount] = PciBridgeInfo;
                    SecBus = PciBridgeInfo.SecBusNumber;
                    gPciBridgeCount++;

                    // Program Subordinate Bus number for the bridges which is above this Bridge.
                    ProgramSubordinateBusForBridgeAbove( Bus, SecBus );
                                        
                    Status = EnumerateBus ( ChildBridgeDp, SecBus, BridgeMmioBaseAddress );
                     
                     if ( EFI_ERROR( Status )) {
                        return Status;
                     }
                    
                    // Check for any Device/Bridge programmed under this 
                    // bridge and Program this bridge based on that.
                    Status = CheckforProgrammedBridgeorDevice( Bus,    // TODO
                                                               Device,
                                                               Function,
                                                               SecBus,
                                                               BridgeAlreadyProgrammed,
                                                               BridgeMmioBaseAddress );
                    if ( EFI_ERROR( Status )) {
                        return Status;
                    }
                    break;
                default:
                    break;
            } // Switch
            
            // Continue if PCI is multi functional device
            if((Function == 0) && !MultiFunc) {
                break;
            } else if(MultiFunc) {
                MultiFunc=FALSE;
            }
        } // Function For Loop
    } // Device For Loop
    
    return EFI_SUCCESS;
}
#endif

/**
    This routine resets the Ahci Controller on end of Pei when
    Ahci Recovery Capsule Loaded.

    @param PeiServices 
    @param NotifyDesc 
    @param InvokePpi 

    @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
NotifyOnAhciRecoveryCapsuleLoaded (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
    IN  VOID                        *InvokePpi
)
{
    UINT32                          GlobalHostControl;
    UINTN                           AhciBaseAddress;
    UINT8                           Port = 0;
    UINT8                           Index;
    UINT8                           BusNumber = 0;
    UINT16                          BaseAddrLimit = 0;
    EFI_STATUS                      Status;
#if OFFBOARD_AHCI_RECOVERY_SUPPORT   
    UINT8                           CmdReg;
    UINT8                           Bus;
    UINT8                           Device;
    UINT8                           Function;
    UINT8                           Index1;
#endif     
    //    1. Disable AHCI_ENABLE in the GHC.
    //    2. Issue Controller Reset.
    //    3. Wait for HBA Reset to Complete.

    for(Index=0; Index < MAX_AHCI_CONTROLLER_DEVICE ;Index++) {
        if( gAhciControllerInfo[Index] ) {
            AhciBaseAddress = gAhciControllerInfo[Index]->AhciBaseAddress;
            
            //  Clear AE bit in Global Host Control Register
            HBA_WRITE_REG32(AhciBaseAddress, HBA_GHC,0x00000000);
            
            // HBA Reset
            GlobalHostControl = 0x01;
            HBA_WRITE_REG32(AhciBaseAddress, HBA_GHC, GlobalHostControl);
            // 5 milliSec Delay
            gStallPpi->Stall( gPeiServices, gStallPpi, 5000 );
            for ( Port = 0; Port < gAhciControllerInfo[Index]->NumberofPortsImplemented; Port++) {
                Status = CheckPortImplemented(gAhciControllerInfo[Index], Port);
                if (EFI_ERROR(Status)) {
                    continue;
                }
                // Clear Start
                HBA_PORT_REG32_AND (AhciBaseAddress,  Port, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_ST));

                // Make sure CR is 0 with in 500msec
                WaitForMemClear( AhciBaseAddress,
                                 Port,
                                 HBA_PORTS_CMD,
                                 HBA_PORTS_CMD_CR,
                                 HBA_CR_CLEAR_TIMEOUT );

                //  Clear FIS receive enable.
                HBA_PORT_REG32_AND (AhciBaseAddress, Port,
                                    HBA_PORTS_CMD, ~(HBA_PORTS_CMD_FRE));

                //  Make sure FR is 0 with in 500msec
                WaitForMemClear( AhciBaseAddress,
                                  Port,
                                  HBA_PORTS_CMD,
                                  HBA_PORTS_CMD_FR,
                                  HBA_FR_CLEAR_TIMEOUT );

           }
            
        }     
    }
#if OFFBOARD_AHCI_RECOVERY_SUPPORT
    // Program the bridge's Prim./Sec./Subordinate bus number to Zero.
    // and reprogram the membase/memlinit, Command reg.
    for(Index=0; Index<AHCI_MAXIMUM_PCI_ROOT_BRIDGE ;Index++) {
        if( gAhciPciBridgeInfo[Index] ) {
            gPciCfg->Write (gPeiServices, \
                                gPciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                        gAhciPciBridgeInfo[Index]->PrimaryBusNumber,
                                        gAhciPciBridgeInfo[Index]->Device, 
                                        gAhciPciBridgeInfo[Index]->Function, 
                                        PCI_PBUS),\
                                &BusNumber);

            gPciCfg->Write (gPeiServices, \
                                gPciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                        gAhciPciBridgeInfo[Index]->PrimaryBusNumber,
                                        gAhciPciBridgeInfo[Index]->Device, 
                                        gAhciPciBridgeInfo[Index]->Function, 
                                        PCI_SBUS),\
                                &BusNumber);

            gPciCfg->Write (gPeiServices, \
                                gPciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                        gAhciPciBridgeInfo[Index]->PrimaryBusNumber,
                                        gAhciPciBridgeInfo[Index]->Device, 
                                        gAhciPciBridgeInfo[Index]->Function, 
                                        PCI_SUBUS),\
                                &BusNumber);

            for(Index1=0; Index1<AHCI_MAXIMUM_PCI_ROOT_BRIDGE ;Index1++) {
                if( gAhciPciProgrammedBridgeInfo[Index1] ) {
                    if( (gAhciPciProgrammedBridgeInfo[Index1]->PrimaryBusNumber ==  gAhciPciBridgeInfo[Index]->PrimaryBusNumber) &&
                         (gAhciPciProgrammedBridgeInfo[Index1]->Device ==  gAhciPciBridgeInfo[Index]->Device) &&
                         (gAhciPciProgrammedBridgeInfo[Index1]->Function ==  gAhciPciBridgeInfo[Index]->Function) ) {
                            

                         Bus      = gAhciPciBridgeInfo[Index]->PrimaryBusNumber;
                         Device   = gAhciPciBridgeInfo[Index]->Device;
                         Function = gAhciPciBridgeInfo[Index]->Function;

                         gPciCfg->Read(gPeiServices, \
                                          gPciCfg, \
                                          EfiPeiPciCfgWidthUint8,\
                                          PEI_PCI_CFG_ADDRESS(Bus,Device,
                                          Function,PCI_CMD),\
                                          &CmdReg);

                         CmdReg &= AHCI_MASK_MEM_BUS_MASTER;
                         gPciCfg->Write ( gPeiServices, \
                                             gPciCfg, \
                                             EfiPeiPciCfgWidthUint16, \
                                             PEI_PCI_CFG_ADDRESS (Bus,
                                                Device, Function, PCI_MEMBASE),\
                                             &BaseAddrLimit);

                          gPciCfg->Write ( gPeiServices, \
                                             gPciCfg, \
                                             EfiPeiPciCfgWidthUint16, \
                                             PEI_PCI_CFG_ADDRESS (Bus,
                                             Device, Function, PCI_MEMLIMIT),\
                                             &BaseAddrLimit);

                          gPciCfg->Write (gPeiServices, \
                                            gPciCfg, \
                                            EfiPeiPciCfgWidthUint8, \
                                            PEI_PCI_CFG_ADDRESS (Bus,
                                            Device, Function, PCI_CMD),\
                                            &CmdReg);
                        }
                    } else {
                        break;
                    }
                } // Programmed Bridge For loop
            }
        } // PCI Root Bridge For loop
#endif    
    return EFI_SUCCESS;
}

/**
    PeiMalloc allocates bytes of Memory based on Size and 
    returns a pointer to the allocated space.
    
    @param  PeiServices
    @param  Size
    
    @retval Address of the allocated memory
**/
VOID* 
PeiMalloc ( 
    EFI_PEI_SERVICES **PeiServices,
    UINTN Size
)
{
    VOID *p=NULL;   
    (**PeiServices).AllocatePool( PeiServices,Size,&p );
    return p;
}

/**
    PeiDPCopy copies the user provided device path into a new buffer and
    returns its address.
    
    @param  PeiServices
    @param  pDp
    
    @retval Address of the new copy of pDp
**/
VOID* 
PeiDPCopy (
    EFI_PEI_SERVICES **PeiServices,
    EFI_DEVICE_PATH *pDp
)
{
    UINTN l;
    UINT8 *p = NULL;
    
    if (!pDp) return NULL;
    
    l = PeiDPLength(pDp);
    p = PeiMalloc(PeiServices,l);   
    (*PeiServices)->CopyMem ( p, pDp, l);
    return p;
}

/**    
    PeiDPLength Finds length of the provided device path, 
    (including the size of EndOfDevicePath).
    
     @param  pDp
     
     @retval Size of the device path, pDp, including EndOfDevicePath.
 **/
UINTN 
PeiDPLength (
    EFI_DEVICE_PATH *pDp
)
{
    UINTN Size;
    if (!pDp) return 0;
    Size = 0;
    for( 
        ; !(isEndNode(pDp) && pDp->SubType==END_ENTIRE_SUBTYPE)
        ; pDp = NEXT_NODE(pDp)
    ){
        UINTN Length = NODE_LENGTH(pDp);
        //Protection from the junk data.
        //Zero type and zero length are illegal.
        //If we encountered them, return
        if (!pDp->Type || !Length) return Size;
        Size += Length;
    }
    return Size + sizeof(EFI_DEVICE_PATH); // add size of END_DEVICE_PATH node
}

/**
    PeiDPAddNode adds the very first element of pDp2 to pDp1 just 
    before its device path terminator. 

    @param  PeiServices
    @param  pDp1
    @param  pDp2
    
    @retval Address of the newly allocated modified device path
    
    @note
    1.pDp1 is the device path to be appended to pDp2
    2.Device Path read from pDp2, and its very first element will be 
      appended to pDp1.
**/
VOID* 
PeiDPAddNode (
    EFI_PEI_SERVICES **PeiServices,
    EFI_DEVICE_PATH *pDp1, 
    EFI_DEVICE_PATH *pDp2
)
{
    UINTN l1;
    UINT8 *NewDp, *p;
        
    if (!pDp2) return (pDp1) ? PeiDPCopy(PeiServices,pDp1) : PeiDPCopy(PeiServices,&EndOfDevicePathNode);
    
    l1 = pDp1 ? PeiDPLength(pDp1)-sizeof(EFI_DEVICE_PATH) : 0;
    
    NewDp = PeiMalloc(PeiServices,(l1+NODE_LENGTH(pDp2)+sizeof(EFI_DEVICE_PATH)));
    p = NewDp;

    if (l1) { 
        (*PeiServices)->CopyMem ( p, pDp1, l1); 
        p+=l1; 
    }
    (*PeiServices)->CopyMem ( p, pDp2, NODE_LENGTH(pDp2)); 
    p+=NODE_LENGTH(pDp2);
    *((EFI_DEVICE_PATH*)p) = EndOfDevicePathNode;
    return NewDp;
}

/**
    This routine resets the Ahci Controller on end of Pei when
    Ahci Recovery Capsule Loaded.

    @param PeiServices 
    @param NotifyDesc 
    @param InvokePpi 

    @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
InstallPeiRecoveryBlkIoPpi (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
    IN  VOID                        *InvokePpi
)
{
    AHCI_RECOVERY_BLK_IO_DEV    *Ahci_BlkIoDev  = NULL;
    static   BOOLEAN            RecoveryBlockIoPpiInstalled = FALSE;
    EFI_STATUS                  Status;
    
    if(RecoveryBlockIoPpiInstalled) {
        return EFI_SUCCESS;
    }
    
    Status = (**PeiServices).AllocatePool( PeiServices,
                                           sizeof(AHCI_RECOVERY_BLK_IO_DEV),
                                           (VOID**)&Ahci_BlkIoDev );
    if ( EFI_ERROR( Status )) {
        ASSERT_PEI_ERROR ( PeiServices, Status );  
        return EFI_OUT_OF_RESOURCES;
    }

    (*PeiServices)->SetMem ((VOID*)Ahci_BlkIoDev,
                             sizeof(AHCI_RECOVERY_BLK_IO_DEV),
                             0 );
    
    Ahci_BlkIoDev->HaveEnumeratedDevices = FALSE;
    Ahci_BlkIoDev->DeviceCount           = 0;
    Ahci_BlkIoDev->AmiExtSig             = AMI_EXTENSION_SIGNATURE;
     
    Ahci_BlkIoDev->RecoveryBlkIo.GetNumberOfBlockDevices = Ahci_GetNumberOfBlockDevices;
    Ahci_BlkIoDev->RecoveryBlkIo.GetBlockDeviceMediaInfo = Ahci_GetBlockDeviceMediaInfo;
    Ahci_BlkIoDev->RecoveryBlkIo.ReadBlocks              = Ahci_ReadBlocks;
    Ahci_PpiDescriptor.Ppi = &Ahci_BlkIoDev->RecoveryBlkIo;
    Status = (**PeiServices).InstallPpi( PeiServices, &Ahci_PpiDescriptor );
    ASSERT_PEI_ERROR ( PeiServices, Status );

    // Install Virtual Block Io PPI for Ahci Recovery
    Ahci_BlkIoDev->RecoveryBlkIo2.GetNumberOfBlockDevices = Ahci_GetNumberOfBlockDevices2;
    Ahci_BlkIoDev->RecoveryBlkIo2.GetBlockDeviceMediaInfo = Ahci_GetBlockDeviceMediaInfo2;
    Ahci_BlkIoDev->RecoveryBlkIo2.ReadBlocks              = Ahci_ReadBlocks2;
    Ahci_PpiDescriptor2.Ppi = &Ahci_BlkIoDev->RecoveryBlkIo2;

    Status = (**PeiServices).InstallPpi( PeiServices, &Ahci_PpiDescriptor2 );
    ASSERT_PEI_ERROR ( PeiServices, Status );
    RecoveryBlockIoPpiInstalled = TRUE;
    
    return Status;
}
/**
    Entry Point for AhciRecovery,Installs EFI_PEI_RECOVERY_BLOCK_IO_PPI for AHCI devices.

    @param  FileHandle
    @param  PeiServices

    @retval EFI_STATUS

**/
EFI_STATUS 
EFIAPI
AhciRecoveryPeimEntry (
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
)
{
  
    EFI_STATUS      Status  = EFI_SUCCESS;

    if ( !gPeiServices ) {
        gPeiServices = PeiServices;
        gPciCfg = (**PeiServices).PciCfg;
    }

    // Install Virtual Block Io PPI for Ahci Recovery
    Status = (*PeiServices)->NotifyPpi( PeiServices, mNotifyList );
    ASSERT_PEI_ERROR ( PeiServices, Status);    
    return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
