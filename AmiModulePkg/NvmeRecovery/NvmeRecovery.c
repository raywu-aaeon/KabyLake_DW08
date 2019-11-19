//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeRecovery.c
    This file contains the code to implement
    EFI_PEI_RECOVERY_BLOCK_IO_PPI for Nvme devices.

**/

//------------------------------------------------------------------------
#include "NvmeRecovery.h"
//------------------------------------------------------------------------

// PEI Recovery Block I/O PPI
EFI_PEI_PPI_DESCRIPTOR   Nvme_BlockIoPpiDescriptor = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiVirtualBlockIoPpiGuid,
    NULL
};

EFI_GUID    guidEndOfPei = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;
static EFI_PEI_NOTIFY_DESCRIPTOR gNotifyList = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &guidEndOfPei,
    NotifyOnNvmeRecoveryCapsuleLoaded
};
NVME_PCI_DEVICE_INFO             *gNvmePciDeviceInfo[MAX_NVME_RECOVERY_DEVICE]={NULL};
NVME_RECOVERY_CONTROLLER_INFO    *gNvmeControllerInfo[MAX_NVME_RECOVERY_DEVICE]={NULL};
EFI_PEI_SERVICES                 **gPeiServices = NULL;
EFI_PEI_STALL_PPI                *gStallPpi     = NULL;
EFI_PEI_PCI_CFG2_PPI             *gPciCfg = NULL;

#if defined(PeiPciEnumeration_SUPPORT) && (PeiPciEnumeration_SUPPORT == 1)
#define AMI_PEI_PCI_ENUMERATION_GUID \
 {0xe7b5b715,0x1183, 0x4533, {0xbe,0x76,0x56,0xa6,0xd7,0xce,0xb0,0x2e}}
EFI_GUID    gAmiPeiPciEnumerationPpiGuid = AMI_PEI_PCI_ENUMERATION_GUID;
AMI_PEI_PCI_ENUMERATION_PPI      *gPeiPciEnumerationPpi = NULL;
#else
NVME_PCI_BRIDGE_INFO             *gNvmePciBridgeInfo[NVME_MAXIMUM_PCI_ROOT_BRIDGE]={NULL};
NVME_PCI_PROGRAMMED_BRIDGE_INFO  *gNvmePciProgrammedBridgeInfo[NVME_MAXIMUM_PCI_ROOT_BRIDGE]={NULL};

UINT64   gNvmeMmioBaseAddress = 0;
UINT8    gPciBridgeCount = 0;
UINT8    gFreeBusNumber         = 0;
BOOLEAN  gAssignMmioTopDown     = FALSE;

UINT32    gNvmeBridgeOffsetAddr =
#ifdef   NVME_PCI_MEM_MAP_BRIDGE_ADDRESS_OFFSET
    NVME_PCI_MEM_MAP_BRIDGE_ADDRESS_OFFSET;
#else
    0;
#endif
#endif


/**
    Installs EFI_PEI_RECOVERY_BLOCK_IO_PPI for Nvme devices.

    @param  FileHandle 
    @param  PeiServices 

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
NvmeRecoveryPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS                Status         = EFI_SUCCESS;
    NVME_RECOVERY_BLK_IO_DEV  *NvmeBlkIoDev  = NULL;

    if ( !PeiServices || !FileHandle) {
        return EFI_INVALID_PARAMETER;
    }

    gPeiServices = PeiServices;
    gPciCfg = (**PeiServices).PciCfg;

    Status = (**PeiServices).LocatePpi( PeiServices,
                                        &gEfiPeiStallPpiGuid,
                                        0,
                                        NULL,
                                        (VOID**)&gStallPpi
                                        );
    if ( EFI_ERROR( Status )) {
        return Status;
    }

    Status = (**PeiServices).AllocatePool( PeiServices,
                                           sizeof(NVME_RECOVERY_BLK_IO_DEV),
                                           (VOID**)&NvmeBlkIoDev
                                           );
    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }

    (**PeiServices).SetMem ( (VOID*)NvmeBlkIoDev,
                             sizeof(NVME_RECOVERY_BLK_IO_DEV),
                             0
                             );

    NvmeBlkIoDev->HaveEnumeratedDevices = FALSE;
    NvmeBlkIoDev->DeviceCount           = 0;

    NvmeBlkIoDev->RecoveryBlkIo.GetNumberOfBlockDevices = Nvme_GetNumberOfBlockDevices;
    NvmeBlkIoDev->RecoveryBlkIo.GetBlockDeviceMediaInfo = Nvme_GetBlockDeviceMediaInfo;
    NvmeBlkIoDev->RecoveryBlkIo.ReadBlocks              = Nvme_ReadBlocks;
    Nvme_BlockIoPpiDescriptor.Ppi = &NvmeBlkIoDev->RecoveryBlkIo;

    // Install EFI_PEI_RECOVERY_BLOCK_IO_PPI
    Status = (**PeiServices).InstallPpi( PeiServices, &Nvme_BlockIoPpiDescriptor );
    return Status;
}

/**
    Nvme_GetNumberOfBlockDevices is API function of EFI_PEI_RECOVERY_BLOCK_IO_PPI. 
    This Returns the number of Nvme Block Devices present in the System.
   
    @param  PeiServices 
    @param  This 
    @param  NumberBlockDevices 

    @retval EFI_STATUS

    @note  
    The enumeration will be done to find Nvmedevices if not enumerated
    already
**/
EFI_STATUS
EFIAPI
Nvme_GetNumberOfBlockDevices(
    IN  EFI_PEI_SERVICES              **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    OUT UINTN                         *NumberBlockDevices
)
{
    NVME_RECOVERY_BLK_IO_DEV  *NvmeBlkIoDev = NULL;
    EFI_STATUS                Status;

    if ( !This || !NumberBlockDevices) {
        return EFI_INVALID_PARAMETER;
    }

    NvmeBlkIoDev = (NVME_RECOVERY_BLK_IO_DEV*)This;

    if ( !NvmeBlkIoDev->HaveEnumeratedDevices ) {
        // Enumerate PCI root bridges for Nvme Devices
        Status = EnumerateNvmeDevices( NvmeBlkIoDev );
        if(EFI_ERROR(Status)) {
            return Status;
        }
        NvmeBlkIoDev->HaveEnumeratedDevices = TRUE;
    }

    *NumberBlockDevices = NvmeBlkIoDev->DeviceCount;
    return EFI_SUCCESS;
}

/**
    Routine returns the Media information of the Nvme device respective to the 
    NvmeDevice index

    @param  PeiServices 
    @param  This 
    @param  DeviceIndex 
    @param  MediaInfo 

    @retval Status

    @note  
  The enumeration will be done to find Nvmedevices if not enumerated
  already.
**/
EFI_STATUS
EFIAPI
Nvme_GetBlockDeviceMediaInfo (
    IN  EFI_PEI_SERVICES              **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN  UINTN                         DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA        *MediaInfo
)
{
    NVME_RECOVERY_BLK_IO_DEV  *NvmeBlkIoDev = NULL;
    EFI_STATUS                Status;

    // Return error if input parameter is invalid
    if ( !This || !MediaInfo ) {
        return EFI_INVALID_PARAMETER;
    }

    NvmeBlkIoDev = (NVME_RECOVERY_BLK_IO_DEV*)This;

    // Enumerate to find the Nvme Devices if not enumerated.
    if ( !NvmeBlkIoDev->HaveEnumeratedDevices ) {
        Status = EnumerateNvmeDevices( NvmeBlkIoDev );
        if ( EFI_ERROR( Status )) {
            return Status;
        }
        NvmeBlkIoDev->HaveEnumeratedDevices = TRUE;
    }

    // Return error if input Nvme device index is invalid
    if( DeviceIndex > NvmeBlkIoDev->DeviceCount-1 ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !NvmeBlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia ) {
        NvmeBlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia = TRUE;
    }

    // Return the Media info of the requested NVME Device.
    *MediaInfo = NvmeBlkIoDev->DeviceInfo[DeviceIndex]->MediaInfo;
    return EFI_SUCCESS;
}

/**
    Read Nvme device LBA given as input and stores the data in Buffer
    Total number of LBA to read is calculated from input BufferSize

    @param  PeiServices 
    @param  This 
    @param  DeviceIndex 
    @param  StartLba 
    @param  BufferSize 
    @param  Buffer 

    @retval Status
       Data filled in *Buffer

    @note  
  The enumeration will be done to find Nvme devices if not enumerated
  already.
**/
EFI_STATUS
EFIAPI
Nvme_ReadBlocks (
    IN  EFI_PEI_SERVICES              **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN  UINTN                         DeviceIndex,
    IN  EFI_PEI_LBA                   StartLba,
    IN  UINTN                         BufferSize,
    OUT VOID                          *Buffer
)
{
    EFI_PEI_BLOCK_IO_MEDIA    MediaInfo;
    EFI_STATUS                Status;
    NVME_RECOVERY_BLK_IO_DEV  *NvmeBlkIoDev = NULL;

    // Return error if input parameter is invalid
    if ( !This || !Buffer ) {
        return EFI_INVALID_PARAMETER;
    }
    
    NvmeBlkIoDev = (NVME_RECOVERY_BLK_IO_DEV *)This;

    if ( BufferSize == 0 ) {
        return EFI_SUCCESS;
    }

    if ( !NvmeBlkIoDev->HaveEnumeratedDevices ) {
        Status = EnumerateNvmeDevices( NvmeBlkIoDev );
        if ( EFI_ERROR( Status )) {
            return Status;
        }
        NvmeBlkIoDev->HaveEnumeratedDevices = TRUE;
    }

    // Return error if input Nvme device index is invalid
    if( DeviceIndex > NvmeBlkIoDev->DeviceCount-1 ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !NvmeBlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia ) {
        Status = Nvme_GetBlockDeviceMediaInfo( PeiServices,
                                               This,
                                               DeviceIndex,
                                               &MediaInfo
                                               );

        if (EFI_ERROR(Status)) {
            return Status;
        }
    } else {
        MediaInfo = NvmeBlkIoDev->DeviceInfo[DeviceIndex]->MediaInfo;
    }

    if ( !MediaInfo.MediaPresent ) {
        // Return as No media found
        return EFI_NO_MEDIA;
    }

    Status = NvmeReadBlocks( NvmeBlkIoDev->DeviceInfo[DeviceIndex],
                             StartLba,
                             BufferSize,
                             Buffer,
                             NVME_READ
                             );

    return Status;
}
/**
    Enumerate PCI root bridges for Nvme Controller/Drive

    @param  NvmeBlkIoDev 

    @retval EFI_STATUS

**/
EFI_STATUS
EnumerateNvmeDevices(
    IN OUT NVME_RECOVERY_BLK_IO_DEV             *NvmeBlkIoDev
)
{
    EFI_STATUS                Status;
    UINT8                     Index;
	IDENTIFY_CONTROLLER_DATA  *Identifydata;
    NVME_RECOVERY_DEVICE_INFO *NvmeDeviceInfo;
	
#if defined(PeiPciEnumeration_SUPPORT) && (PeiPciEnumeration_SUPPORT == 1)
    UINT8                           DeviceInstance = 0;
    AMI_PEI_PCI_DEVICE_LOCATION     *PeiPciDevLocation;
    AMI_PEI_PCI_CLASSCODE           PciClassCode;
#else
    UINT8                     Index1;
    UINT8                     SecondaryBusNo;
    NVME_PCI_BRIDGE_INFO      BridgeInfo;
    BOOLEAN                   DeviceFound = FALSE;
    UINT16                    VendorId;
	UINT8                     SubBusNoForRootBridge = NVME_PCI_MAX_SUBORDINATE_NUMBER;
    NVME_PCI_ROOT_BRIDGE_LIST RootBridgeList[]      = { NVME_ROOT_BRIDGE_LIST
                                                       {0xff,0xff,0xff}
                                                      };
    UINT64                    BridgeBaseAddress = 0;
    UINT32                    BridgePreviousMemBase = 0;
    BOOLEAN                   BridgeAlreadyProgrammed = FALSE;
#endif
    
#if defined(PeiPciEnumeration_SUPPORT) && (PeiPciEnumeration_SUPPORT == 1)
    Status = (**gPeiServices).LocatePpi( gPeiServices, &gAmiPeiPciEnumerationPpiGuid, 0, NULL, &gPeiPciEnumerationPpi);

    if ( EFI_ERROR( Status )) {
        return Status;
    }
    Status = gPeiPciEnumerationPpi->InitDevices(gPeiPciEnumerationPpi);


    if ( EFI_ERROR( Status )) {
        return Status;
    }
    PciClassCode.BaseClassCode = NVME_CLASS_CODE;
    PciClassCode.SubClassCode =  NVME_SUBCLASS_CODE;
    PciClassCode.ProgrammingInterface = NVME_INTERFACE_CODE;
    
    Status = (**gPeiServices).AllocatePool((CONST EFI_PEI_SERVICES**)gPeiServices,
                                            sizeof(AMI_PEI_PCI_DEVICE_LOCATION),
                                            (VOID**)&PeiPciDevLocation);
    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }
    do {
        Status = gPeiPciEnumerationPpi->GetNextDeviceByClassCode( \
                                                        gPeiPciEnumerationPpi, 
                                                        PciClassCode,
                                                        DeviceInstance,
                                                        PeiPciDevLocation);
#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n GetNextDeviceByClassCode Status :%r ",Status));
#endif

        if ( EFI_ERROR( Status )) {
            break;
        }
        // Allocate memory for the Controller
        Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                 sizeof(NVME_PCI_DEVICE_INFO),
                                                 (VOID**)&gNvmePciDeviceInfo[DeviceInstance]);

        if ( EFI_ERROR( Status )) {
            return EFI_OUT_OF_RESOURCES;
        }
        (**gPeiServices).SetMem( (VOID*) gNvmePciDeviceInfo[DeviceInstance],
                                            sizeof(NVME_PCI_DEVICE_INFO),
                                            0 );
        gNvmePciDeviceInfo[DeviceInstance]->BusNumber = PeiPciDevLocation->BusNumber;
        gNvmePciDeviceInfo[DeviceInstance]->Device =  PeiPciDevLocation->DeviceNumber;
        gNvmePciDeviceInfo[DeviceInstance]->Function = PeiPciDevLocation->FunctionNumber;
        
        // We got the PCI location of the Nvme Controller now find the BAR address 
        // assigned by the Pei Pci Enumeration PEIM
        gPciCfg->Read(gPeiServices, \
                           gPciCfg, \
                           EfiPeiPciCfgWidthUint64,\
                           PEI_PCI_CFG_ADDRESS(
                               PeiPciDevLocation->BusNumber,
                               PeiPciDevLocation->DeviceNumber,
                               PeiPciDevLocation->FunctionNumber,
                               PCI_BAR0),\
                           &(gNvmePciDeviceInfo[DeviceInstance]->BaseAddress)); 
        
        gNvmePciDeviceInfo[DeviceInstance]->BaseAddress &= ~0x0F; // Mask don't care bits for Mmio Base Address
#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n Nvme Device Found with BaseAddress:%x ",gNvmePciDeviceInfo[DeviceInstance]->BaseAddress ));
#endif
        DeviceInstance++;
        
    } while(Status == EFI_SUCCESS);                                 
    
#else
    
    if( RootBridgeList[0].Bus == 0xff && RootBridgeList[0].Dev == 0xff &&
            RootBridgeList[0].Func == 0xff ) {

#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n No Root bridge added for Nvme device enumeration "));
#endif

        return EFI_NOT_FOUND;
    }
    
    
    // Other Recovery Driver's will do RootBridge Enumeration, so consider
    // from the Previously Programmed Bridge memory and Free Secondary Bus 
    // Number from PCD values.
        
    PcdNvmeRootBridgeResources(FALSE);
    for( Index=0; RootBridgeList[Index].Func != 0xff ; Index++) {
        
        if( (RootBridgeList[Index].Dev == 0) && 
                (RootBridgeList[Index].Func == 0) ) {

            // Align the MMIO address for the PCI 2 PCI Bridge in Top Down approach
            if(gAssignMmioTopDown) {
                gNvmeMmioBaseAddress -= SIZE_1MB;
            }
            BridgeBaseAddress = gNvmeMmioBaseAddress + gNvmeBridgeOffsetAddr;
            if(!gAssignMmioTopDown) {
                gNvmeMmioBaseAddress += SIZE_1MB;
            }
            // Enumerate Bus Zero.
            Status = EnumerateBus( 0, BridgeBaseAddress);
            if ( EFI_ERROR( Status )) {
                return Status;
            }
        } else {
            Status = gPciCfg->Read(gPeiServices, \
                                   gPciCfg, \
                                   EfiPeiPciCfgWidthUint16,\
                                   PEI_PCI_CFG_ADDRESS(
                                       RootBridgeList[Index].Bus,
                                       RootBridgeList[Index].Dev,
                                       RootBridgeList[Index].Func,
                                       PCI_VID),\
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
                // Store the already Programmed Secondary & Subordinate
                // Bus no. for the root Bridge.
                gPciCfg->Read ( gPeiServices, \
                               gPciCfg, \
                               EfiPeiPciCfgWidthUint8, \
                               PEI_PCI_CFG_ADDRESS(
                                   RootBridgeList[Index].Bus,
                                   RootBridgeList[Index].Dev, 
                                   RootBridgeList[Index].Func, 
                                   PCI_SBUS),\
                               &BridgeInfo.SecBusNumber);
                
                gPciCfg->Read ( gPeiServices, \
                                gPciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                    RootBridgeList[Index].Bus,
                                    RootBridgeList[Index].Dev,
                                    RootBridgeList[Index].Func, 
                                    PCI_SUBUS),\
                                &BridgeInfo.SubBusNumber );    
                                     
                gPciCfg->Read (gPeiServices, \
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
                BridgeBaseAddress  = ((BridgePreviousMemBase << 16) & 0xFFF00000) + gNvmeBridgeOffsetAddr;
            } else {
                // Program the Secondary & subordinate Bus no. for root Bridge.
                gPciCfg->Write ( gPeiServices, \
                             gPciCfg, \
                             EfiPeiPciCfgWidthUint8, \
                             PEI_PCI_CFG_ADDRESS (
                                 RootBridgeList[Index].Bus,
                                 RootBridgeList[Index].Dev, 
                                 RootBridgeList[Index].Func, 
                                 PCI_SBUS),\
                             &BridgeInfo.SecBusNumber);
               
                gPciCfg->Write ( gPeiServices, \
                             gPciCfg, \
                             EfiPeiPciCfgWidthUint8, \
                             PEI_PCI_CFG_ADDRESS (
                                 RootBridgeList[Index].Bus,
                                 RootBridgeList[Index].Dev,
                                 RootBridgeList[Index].Func, 
                                 PCI_SUBUS),\
                             &BridgeInfo.SubBusNumber );
                BridgeAlreadyProgrammed = FALSE;
                // Align the MMIO address for the PCI 2 PCI Bridge
                if(gAssignMmioTopDown) {
                    gNvmeMmioBaseAddress -= SIZE_1MB;
                }
                BridgeBaseAddress = gNvmeMmioBaseAddress + gNvmeBridgeOffsetAddr;
                if(!gAssignMmioTopDown) {
                    gNvmeMmioBaseAddress += SIZE_1MB;
                }
                gFreeBusNumber++;
            }
            BridgeInfo.BridgeAlreadyProgrammed = BridgeAlreadyProgrammed;
            // Store the Bridge info for programming the Bridge's
            // above this Bridge.
            Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                     sizeof(NVME_PCI_BRIDGE_INFO),
                                                     (VOID**)&gNvmePciBridgeInfo[gPciBridgeCount] );
            if ( EFI_ERROR( Status )) {
                return EFI_OUT_OF_RESOURCES;
            }

            *gNvmePciBridgeInfo[gPciBridgeCount] = BridgeInfo;

            SecondaryBusNo = BridgeInfo.SecBusNumber;
            gPciBridgeCount++;

            // Program Subordinate for the bridge above this Bridge.
            ProgramSubordinateBusForBridgeAbove(RootBridgeList[Index].Bus,SecondaryBusNo);
            // Enumerate the device's under this RootBrdige.
            Status = EnumerateBus(SecondaryBusNo, BridgeBaseAddress);
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
                                                       BridgeBaseAddress );
            if ( EFI_ERROR( Status )) {
                return Status;
            }
        }
    }
    // Set the PCD, with new NVMe Root Bridge Resources
    PcdNvmeRootBridgeResources(TRUE);
    // check and clear Primary, Secondary and Subordinate Bus number for the Bridges under 
    // which device is not found.
    SecondaryBusNo = 0;
    for( Index1 = 0; Index1< NVME_MAXIMUM_PCI_ROOT_BRIDGE;Index1++) {
        if(!gNvmePciBridgeInfo[Index1]) { 
            continue;
        }
        if(gNvmePciBridgeInfo[Index1]->BridgeAlreadyProgrammed) continue;
        for(Index=0,DeviceFound= FALSE; Index < MAX_NVME_RECOVERY_DEVICE ;Index++) {
            if( gNvmePciDeviceInfo[Index] ) {  
                if( gNvmePciDeviceInfo[Index]->BusNumber == \
                                  gNvmePciBridgeInfo[Index1]->SecBusNumber) {
                    // Device Found under this Bridge.
                    DeviceFound = TRUE;
                    break;
                }
            }
        }
        if(!DeviceFound) {
            // No device found under this Bridge reset Sec and Subordinate 
            // Bus Number registers of P2P.
            gPciCfg->Write ( gPeiServices, \
                              gPciCfg, \
                              EfiPeiPciCfgWidthUint8, \
                              PEI_PCI_CFG_ADDRESS (
                                  gNvmePciBridgeInfo[Index1]->PrimaryBusNumber,
                                  gNvmePciBridgeInfo[Index1]->Device, 
                                  gNvmePciBridgeInfo[Index1]->Function, 
                                  PCI_PBUS),\
                              &SecondaryBusNo);
            gPciCfg->Write ( gPeiServices, \
                                 gPciCfg, \
                                 EfiPeiPciCfgWidthUint8, \
                                 PEI_PCI_CFG_ADDRESS (
                                    gNvmePciBridgeInfo[Index1]->PrimaryBusNumber,
                                    gNvmePciBridgeInfo[Index1]->Device, 
                                    gNvmePciBridgeInfo[Index1]->Function, 
                                    PCI_SBUS),\
                                 &SecondaryBusNo);
                                         
            gPciCfg->Write ( gPeiServices, \
                                  gPciCfg, \
                                  EfiPeiPciCfgWidthUint8, \
                                  PEI_PCI_CFG_ADDRESS (
                                      gNvmePciBridgeInfo[Index1]->PrimaryBusNumber,
                                      gNvmePciBridgeInfo[Index1]->Device, 
                                      gNvmePciBridgeInfo[Index1]->Function, 
                                      PCI_SUBUS),\
                                  &SecondaryBusNo );
        }
    }
#endif
    
    Status = (**gPeiServices).NotifyPpi ((CONST EFI_PEI_SERVICES**)gPeiServices, &gNotifyList);
    if(EFI_ERROR(Status)) {
        ASSERT_PEI_ERROR ((CONST EFI_PEI_SERVICES**)gPeiServices, Status);
        return Status;
    }

    // Search for the Nvme Controller and Configure the Nvme Controller
    for(Index=0; Index < MAX_NVME_RECOVERY_DEVICE ;Index++) {

        if( gNvmePciDeviceInfo[Index] ) {

            // Allocate memory for Nvme Controller
            Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                     sizeof(NVME_RECOVERY_CONTROLLER_INFO),
                                                     (VOID**)&gNvmeControllerInfo[Index] );

            if(EFI_ERROR(Status)) {
                return Status;
            }

            (**gPeiServices).SetMem( gNvmeControllerInfo[Index],
                                     sizeof(NVME_RECOVERY_CONTROLLER_INFO),
                                     0 );

            // Initialize Nvme Controller
            Status = InitializeNvmeController( gNvmeControllerInfo[Index], gNvmePciDeviceInfo[Index] );
            if ( EFI_ERROR(Status) ) {
                continue;
            }

            // Allocate memory for Nvme Controller
            Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                     sizeof(IDENTIFY_CONTROLLER_DATA) + gNvmeControllerInfo[Index]->MemoryPageSize,
                                                     (VOID**)&Identifydata );
            if(EFI_ERROR(Status)) {
                return Status;
            }

            // Aligned Buffer address used for getting Identify Device data
            Identifydata = (VOID*) (((UINT64)Identifydata & ~((UINT64)gNvmeControllerInfo[Index]->MemoryPageSize-1)) + 
                                   gNvmeControllerInfo[Index]->MemoryPageSize);

            (**gPeiServices).SetMem( (VOID*)Identifydata,
                                     sizeof(IDENTIFY_CONTROLLER_DATA),
                                     0 );

            gNvmeControllerInfo[Index]->IdentifyData = Identifydata;

            // Get IdentifyController Data Structure
            Status = GetIdentifyData ( gNvmeControllerInfo[Index], (UINT8 *)Identifydata , 1, 0 );
            if (EFI_ERROR(Status)) {
                continue;
            }

            // Configure NSID=1 and define EFI_PEI_BLOCK_IO_MEDIA for NSID=1
            if (1 <= Identifydata->NN) {

                // Allocate memory for NVME_RECOVERY_DEVICE_INFO 
                Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                        sizeof(NVME_RECOVERY_DEVICE_INFO),
                                                        (VOID**)&NvmeBlkIoDev->DeviceInfo[NvmeBlkIoDev->DeviceCount] 
                                                        );
                if(EFI_ERROR(Status)) {
                    return Status;
                }

                NvmeDeviceInfo = NvmeBlkIoDev->DeviceInfo[NvmeBlkIoDev->DeviceCount];
                
                (**gPeiServices).SetMem( NvmeDeviceInfo,
                                         sizeof(NVME_RECOVERY_DEVICE_INFO),
                                         0 );

                NvmeDeviceInfo->ActiveNameSpaceID = 1;
                NvmeDeviceInfo->NvmeControllerInfo = gNvmeControllerInfo[Index];

                // Configure Namespace(NSID=1) of Nvme Controller
                Status = ConfigureActiveNamespace(NvmeDeviceInfo);
                if (EFI_ERROR(Status)) {
                    continue;
                }

                // Create Submission and Completion Queue1
                NvmeDeviceInfo->NvmeControllerInfo->NVMQueueNumber = 1;
                Status  = CreateAdditionalSubmissionCompletionQueue ( 
                              NvmeDeviceInfo->NvmeControllerInfo, 
                              NvmeDeviceInfo->NvmeControllerInfo->NvmeCmdWrapper, 
                              NvmeDeviceInfo->NvmeControllerInfo->NVMQueueNumber, 
                              (NvmeDeviceInfo->NvmeControllerInfo->MaxQueueEntrySupported >= IO_QUEUE_SIZE)?
                                      IO_QUEUE_SIZE : NvmeDeviceInfo->NvmeControllerInfo->MaxQueueEntrySupported
                              );

                if( EFI_ERROR(Status) ) {
                    continue;
                }

                NvmeBlkIoDev->DeviceCount++;

            } // if Loop for NSID=1
        } //Nvme Controller if loop
    } // Nvme Controller for loop

    return EFI_SUCCESS;
}
#if !defined(PeiPciEnumeration_SUPPORT) || (defined(PeiPciEnumeration_SUPPORT) && (PeiPciEnumeration_SUPPORT == 0))
/**
    Routine to Get or Set the Root Bridge Resources from the PCDs AmiPcdPeiPciMmioTopAddress
    and AmiPcdPeiPciEnumerationLastBusNumber.

    @param  Flag - TRUE Sets the PCD values.
                 - FALSE Gets the PCD Values.

    @retval NONE
**/

VOID
PcdNvmeRootBridgeResources(
    BOOLEAN Flag
) 
{
    UINT64    PciMmioStartAddress = 0;
    if(Flag) {
        PcdSet8(AmiPcdPeiPciEnumerationLastBusNumber, (gFreeBusNumber-1));
        PcdSet64(AmiPcdPeiPciMmioTopAddress,gNvmeMmioBaseAddress);
    } else {
	    // Check whether the Mmio Top Address Pcd holds a valid value or not
        if( !PcdGet64(AmiPcdPeiPciMmioTopAddress) ) {
            FindPciMmioAddress(&PciMmioStartAddress);
            PcdSet64( AmiPcdPeiPciMmioTopAddress, PciMmioStartAddress);
        }
        gNvmeMmioBaseAddress = PcdGet64(AmiPcdPeiPciMmioTopAddress);
        gFreeBusNumber = PcdGet8(AmiPcdPeiPciEnumerationLastBusNumber)+ 1 ;
        if( gNvmeMmioBaseAddress <=  PcdGet64(PcdPciExpressBaseAddress)) {
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
#ifdef ENABLE_NVME_RECOVERY_TRACES
            PEI_TRACE((-1,gPeiServices,"\n PcdPciExpressBaseAddress below region can't be used as MMIO, its reserved Memory!! "));
#endif
            *PciMmioStartAddress = PcdGet64(PcdPciExpressBaseAddress) + PCIEX_LENGTH;
            break;
	    }
        Hob.Raw = GET_NEXT_HOB (Hob);
        Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw);
    }
    return; 
}


/**
    Enumerates the Bus number given and search for the Nvme Controller/Drive
    and configures the Nvme device if found in the Bus
        
    @param  Bus 

    @retval Status
**/
EFI_STATUS
EnumerateBus (
    IN  UINT8   Bus,
    IN  UINT64  BridgeBaseAddress
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
    UINT32               MaskAddress = NVME_LDWORD_BAR_MASK;
    UINT64               DeviceDecodeRange0=0,DeviceDecodeRange1=0;
    UINT64               DeviceDecodeRange=0;
    BOOLEAN              MultiFunc=FALSE;
    static UINT8         DeviceCount=0;
    NVME_PCI_DEVICE_INFO PciDeviceInfo;
    NVME_PCI_BRIDGE_INFO PciBridgeInfo;
    UINT64               NvmeMmioDeviceAddress  = BridgeBaseAddress;
    UINT32               BridgePreviousMemBase = 0;
    BOOLEAN              BridgeAlreadyProgrammed = FALSE;
    for( Device = 0;  Device <= PCI_MAX_DEVICE; Device++ ) {

        for( Function = 0;  Function <= PCI_MAX_FUNC; Function++ ) {

#if ENABLE_NVME_RECOVERY_TRACES
    PEI_TRACE((-1, gPeiServices, "\n Enumerating Bus:%x Dev:%x Func:%x",Bus, Device, Function ));
#endif

            gPciCfg->Read(gPeiServices, \
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
                    
                    // PCI device
                    gPciCfg->Read(gPeiServices, \
                                  gPciCfg, \
                                  EfiPeiPciCfgWidthUint32,\
                                  PEI_PCI_CFG_ADDRESS(Bus,Device,
                                  Function,PCI_REV_ID_OFFSET),\
                                  &RevisionId);

                    // Check is it Nvme PCIE controller
                    if( RevisionId[3]== NVME_CLASS_CODE &&
                            RevisionId[2]== NVME_SUBCLASS_CODE &&
                            RevisionId[1]== NVME_INTERFACE_CODE ) {

                        PEI_TRACE(( -1, gPeiServices, "\n NVMe device found at Bus:%x Dev:%x Func:%x \n", Bus, Device, Function ));
                        
                        gPciCfg->Write (gPeiServices, \
                                        gPciCfg, \
                                        EfiPeiPciCfgWidthUint32, \
                                        PEI_PCI_CFG_ADDRESS (Bus,
                                        Device, Function, PCI_BAR0),\
                                        &MaskAddress);
                    
                        gPciCfg->Read(gPeiServices, \
                                      gPciCfg, \
                                      EfiPeiPciCfgWidthUint32,\
                                      PEI_PCI_CFG_ADDRESS(Bus,Device,
                                      Function,PCI_BAR0),\
                                      &DeviceDecodeRange0);
                        
                        MaskAddress = NVME_LDWORD_BAR_MASK;
                        
                        gPciCfg->Write (gPeiServices, \
                                        gPciCfg, \
                                        EfiPeiPciCfgWidthUint32, \
                                        PEI_PCI_CFG_ADDRESS (Bus,
                                        Device, Function, PCI_BAR1),\
                                        &MaskAddress);
                    
                        gPciCfg->Read(gPeiServices, \
                                      gPciCfg, \
                                      EfiPeiPciCfgWidthUint32,\
                                      PEI_PCI_CFG_ADDRESS(Bus,Device,
                                      Function,PCI_BAR1),\
                                      &DeviceDecodeRange1);

                        // Nvme IO mapped IO is not supported
                        if( DeviceDecodeRange0 & 1 ) {
                            break;
                        }

                        DeviceDecodeRange = DeviceDecodeRange0 + LShiftU64(DeviceDecodeRange1, 32);

                        // Find Length of the Nvme Bar
                        DeviceDecodeRange &= MASK_MEM_DECODE_RANGE;
                        DeviceDecodeRange = ~DeviceDecodeRange + 1;

                        // Nvme base address (61-14)
                        NvmeMmioDeviceAddress &= NVME_BAR_MASK;
                        PciDeviceInfo.BaseAddress = NvmeMmioDeviceAddress;

                        PciDeviceInfo.BusNumber = Bus;
                        PciDeviceInfo.Device = Device;
                        PciDeviceInfo.Function = Function;
                        PciDeviceInfo.IsMmioDevice = TRUE;

                        // Program BAR addr and cmd register for the device.
                        gPciCfg->Write (gPeiServices, \
                                       gPciCfg, \
                                       EfiPeiPciCfgWidthUint32, \
                                       PEI_PCI_CFG_ADDRESS (Bus,
                                       Device, Function, PCI_BAR0),\
                                       &NvmeMmioDeviceAddress);

                        NvmeMmioDeviceAddress = RShiftU64(NvmeMmioDeviceAddress, 32);

                        gPciCfg->Write (gPeiServices, \
                                       gPciCfg, \
                                       EfiPeiPciCfgWidthUint32, \
                                       PEI_PCI_CFG_ADDRESS (Bus,
                                       Device, Function, PCI_BAR1),\
                                       &NvmeMmioDeviceAddress);
                        
                        NvmeMmioDeviceAddress = PciDeviceInfo.BaseAddress;
                        NvmeMmioDeviceAddress += DeviceDecodeRange;
                        // Allign Nvme Base address to 16bytes
                        if((NvmeMmioDeviceAddress & 0xF) != 0 ) {
                            NvmeMmioDeviceAddress = (NvmeMmioDeviceAddress & (~0xF)) + 0x10;
                        }

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

                        // Store Nvme Controller info for programming bridges above this bridge
                        Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                                 sizeof(NVME_PCI_DEVICE_INFO),
                                                                 (VOID**)&gNvmePciDeviceInfo[DeviceCount] );
                        if ( EFI_ERROR( Status )) {
                            return EFI_OUT_OF_RESOURCES;
                        }
                        *gNvmePciDeviceInfo[DeviceCount] = PciDeviceInfo;
                        DeviceCount++;
                    } // if end
                    break;

                case HDR_TYPE_P2P_BRG:

                    // PCI bridge
                    if( gPciBridgeCount >= NVME_MAXIMUM_PCI_ROOT_BRIDGE ) {
                        return EFI_OUT_OF_RESOURCES;
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
                        // Store the already Programmed Secondary & Subordinate Bus no. for root Bridge.
                        gPciCfg->Read ( gPeiServices, \
                                        gPciCfg, \
                                        EfiPeiPciCfgWidthUint8, \
                                        PEI_PCI_CFG_ADDRESS (
                                            Bus,
                                            Device, 
                                            Function, PCI_SBUS),\
                                        &PciBridgeInfo.SecBusNumber);
                                                                                              
                        gPciCfg->Read ( gPeiServices, \
                                       gPciCfg, \
                                       EfiPeiPciCfgWidthUint8, \
                                       PEI_PCI_CFG_ADDRESS (
                                           Bus,
                                           Device,
                                           Function, PCI_SUBUS),\
                                       &PciBridgeInfo.SubBusNumber );    
                                                             
                        gPciCfg->Read (gPeiServices, \
                                      gPciCfg, \
                                      EfiPeiPciCfgWidthUint16, \
                                      PEI_PCI_CFG_ADDRESS (
                                          Bus,
                                          Device, 
                                          Function, PCI_MEMBASE),\
                                      &BridgePreviousMemBase);
                        BridgeBaseAddress  = ((BridgePreviousMemBase << 16) & 0xFFF00000) + gNvmeBridgeOffsetAddr;
                        BridgeAlreadyProgrammed = TRUE;
                       
                    } else {
                        // Program Primary, Secondary & subordinate Bus no. for Bridge.
                        // Program Primary, Secondary & subordinate Bus no. for Bridge.
                        gPciCfg->Write ( gPeiServices, \
                                     gPciCfg, \
                                     EfiPeiPciCfgWidthUint8, \
                                     PEI_PCI_CFG_ADDRESS ( 
                                         Bus, 
                                         Device, 
                                         Function, PCI_PBUS ),\
                                     &Bus );
                        gPciCfg->Write ( gPeiServices, \
                                     gPciCfg, \
                                     EfiPeiPciCfgWidthUint8, \
                                     PEI_PCI_CFG_ADDRESS ( 
                                         Bus, 
                                         Device, 
                                         Function, PCI_SBUS ),\
                                     &PciBridgeInfo.SecBusNumber );

                        gPciCfg->Write ( gPeiServices, \
                                     gPciCfg, \
                                     EfiPeiPciCfgWidthUint8, \
                                     PEI_PCI_CFG_ADDRESS (
                                         Bus,
                                         Device, 
                                         Function, PCI_SUBUS),\
                                     &PciBridgeInfo.SubBusNumber);
                        BridgeAlreadyProgrammed = FALSE;
                        // Align Mmio Base Address P2P bridge in Top Down approach
                        if(gAssignMmioTopDown) {
                            gNvmeMmioBaseAddress -= SIZE_1MB;
                        }
                        BridgeBaseAddress = gNvmeMmioBaseAddress + gNvmeBridgeOffsetAddr;
                        gFreeBusNumber++;
                        // Align the Mmio Base Address for the next P2P bridge
                        if(!gAssignMmioTopDown) {
                            gNvmeMmioBaseAddress += SIZE_1MB;
                        }
                    }
                    PciBridgeInfo.BridgeAlreadyProgrammed = BridgeAlreadyProgrammed;
                    // Store Bridge info for programming Bridge's above this Bridge.
                    Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                                             sizeof(NVME_PCI_BRIDGE_INFO),
                                                             (VOID**)&gNvmePciBridgeInfo[gPciBridgeCount]);
                    if ( EFI_ERROR( Status )) {
                        return EFI_OUT_OF_RESOURCES;
                    }

                    *gNvmePciBridgeInfo[gPciBridgeCount] = PciBridgeInfo;
                    SecBus = PciBridgeInfo.SecBusNumber;
                    gPciBridgeCount++;
                    
                    // Program Subordinate Bus number for the bridges which is above this Bridge.
                    ProgramSubordinateBusForBridgeAbove( Bus, SecBus );
                    
                    // Enumerate the Device's/Bridge's under this Bridge.
                    Status = EnumerateBus(SecBus, BridgeBaseAddress);
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
                                                               BridgeBaseAddress );
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

/**
    Routine to Check whether MEM base, Mem limit of the specific
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
    UINT32    BridgePreviousMemBase = 0x0;
    UINT8     SecBusNoForRootBridge = 0x0;
    UINT8     SubBusNoForRootBridge = 0x0;
    // Read the Secondary, Subordinate Bus no. and Mem Base of the root Bridge.
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
    NVME_PCI_PROGRAMMED_BRIDGE_INFO ProgrammedBridgeInfo;
    static UINT8                    ProgrammedBridgeCount=0;
    UINT8                           CmdReg;

    if( ProgrammedBridgeCount >= NVME_MAXIMUM_PCI_ROOT_BRIDGE ) {
        return EFI_OUT_OF_RESOURCES;
    }
    
    gPciCfg->Read( gPeiServices, \
                   gPciCfg, \
                   EfiPeiPciCfgWidthUint8,\
                   PEI_PCI_CFG_ADDRESS(Bus,Device,
                       Function,PCI_CMD),\
                   &CmdReg );
    CmdReg |= CMD_ENABLE_MEM;

    gPciCfg->Write (gPeiServices, \
                    gPciCfg, \
                    EfiPeiPciCfgWidthUint16, \
                    PEI_PCI_CFG_ADDRESS (Bus,
                    Device, Function, PCI_MEMBASE),\
                    &Address);

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
    // Nvme Programmed Bridge is Memory mapped
    ProgrammedBridgeInfo.IsMMIO = TRUE;

    Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                            sizeof(NVME_PCI_PROGRAMMED_BRIDGE_INFO),
                                            (VOID**)&gNvmePciProgrammedBridgeInfo[ProgrammedBridgeCount] );
    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }

    *gNvmePciProgrammedBridgeInfo[ProgrammedBridgeCount] = ProgrammedBridgeInfo;
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

    for(Index=0; Index < NVME_MAXIMUM_PCI_ROOT_BRIDGE ;Index++) {
        if( gNvmePciBridgeInfo[Index] ) {
            if(PrimaryBusNo == gNvmePciBridgeInfo[Index]->SecBusNumber) {
                gPciCfg->Write ( gPeiServices, \
                                 gPciCfg, \
                                 EfiPeiPciCfgWidthUint8, \
                                 PEI_PCI_CFG_ADDRESS (gNvmePciBridgeInfo[Index]->PrimaryBusNumber,
                                         gNvmePciBridgeInfo[Index]->Device, gNvmePciBridgeInfo[Index]->Function, PCI_SUBUS),\
                                 &SubordinateBusNo);
            }
        }
    }
    return;
}

/**
    Routine to initialize Bridge/Device detected and configured when enumerating the Specific
    PCI to PCI bridge

    @param  Bus 
    @param  Device 
    @param  Function 
    @param  SecondaryBusNo 

    @retval Status
**/
EFI_STATUS
CheckforProgrammedBridgeorDevice (
    IN  UINT8  Bus,
    IN  UINT8  Device,
    IN  UINT8  Function,
    IN  UINT8  SecondaryBusNo,
    IN  UINT8  BridgeAlreadyProgrammed,
    IN  UINT64 BridgeDevBaseAddress
)
{
    EFI_STATUS  Status;
    UINT8       Index=0,Index1;
    UINT16      Address;
    UINT8       Subordinate=0;
    BOOLEAN     DeviceFound = FALSE;
    
    // Check the stored info and Program the Bridge, if any
    // Device/Bridge is programmed behind this.
    if(!BridgeAlreadyProgrammed ) {
        for(Index=0; Index < MAX_NVME_RECOVERY_DEVICE ;Index++) {
            if( gNvmePciDeviceInfo[Index] ) { 
                if( gNvmePciDeviceInfo[Index]->BusNumber == SecondaryBusNo ) {
                    // Check whether Devices found behind this bridge
                    // If found program the bridge.
                    DeviceFound = TRUE;
                    if( gNvmePciDeviceInfo[Index]->IsMmioDevice ) {
                        // Programm Memlimit, Membase based on Base address exact width
                        Address = (UINT16)RShiftU64(BridgeDevBaseAddress, 16);
                    }
                    Status = ProgramPciBridge(Bus,Device,Function,Address,gNvmePciDeviceInfo[Index]->IsMmioDevice);
                    if ( EFI_ERROR( Status )) {
                        return Status;
                    }
                    break;
                }           
            }
        }
    }
    // Program the Bridge if any Bridge behind this programmed.
    for(Index=0; Index<NVME_MAXIMUM_PCI_ROOT_BRIDGE ;Index++) {
        if( gNvmePciProgrammedBridgeInfo[Index] ) {
		    // Check whether any Device found under the child bridge
            if(SecondaryBusNo == gNvmePciProgrammedBridgeInfo[Index]->PrimaryBusNumber) {
			    DeviceFound = TRUE;
                Status = ProgramPciBridge(Bus,Device,Function,gNvmePciProgrammedBridgeInfo[Index]->MemIOBaseLimit,
                        gNvmePciProgrammedBridgeInfo[Index]->IsMMIO);
                if ( EFI_ERROR( Status )) {
                    return Status;
                }
            }
        }
    }
    // No NVME device found under this bridge or child bridge so restore to the previous value.
    if(!DeviceFound && !BridgeAlreadyProgrammed ) {
        if(gAssignMmioTopDown) {
            gNvmeMmioBaseAddress += SIZE_1MB;
        } else {
            gNvmeMmioBaseAddress -= SIZE_1MB;
        }
    }
    // Program the Sub-ordinate no. for the bridge based on the bridge's programmed behind this.
    for(Index=0; Index<NVME_MAXIMUM_PCI_ROOT_BRIDGE ;Index++) {
        if( gNvmePciBridgeInfo[Index] ) {
            if(SecondaryBusNo == gNvmePciBridgeInfo[Index]->PrimaryBusNumber) {
                if(!Subordinate) {
                    Subordinate = gNvmePciBridgeInfo[Index]->SubBusNumber;
                } else {
                    if ( gNvmePciBridgeInfo[Index]->SubBusNumber > Subordinate) {
                        Subordinate = gNvmePciBridgeInfo[Index]->SubBusNumber;
                    }
                }
            }
        }
    }

    if(Subordinate != 0) {
        for(Index1=0; Index1 < NVME_MAXIMUM_PCI_ROOT_BRIDGE ;Index1++) {
            if( gNvmePciBridgeInfo[Index1] ) {
                if(SecondaryBusNo == gNvmePciBridgeInfo[Index1]->SecBusNumber) {
                    gNvmePciBridgeInfo[Index1]->SubBusNumber = Subordinate;
                }
            }
        }
     } else {     
     
         // If no bridge is programmed behind this bridge, then assign Sec. no as Sub no.
         for(Index1=0; Index1 < NVME_MAXIMUM_PCI_ROOT_BRIDGE ;Index1++) {
             if( gNvmePciBridgeInfo[Index1] ) {
                 if((gNvmePciBridgeInfo[Index1]->PrimaryBusNumber == Bus) && (gNvmePciBridgeInfo[Index1]->Device == Device)
                     && (gNvmePciBridgeInfo[Index1]->Function == Function)) {
                     Subordinate = gNvmePciBridgeInfo[Index1]->SecBusNumber;
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
    return EFI_SUCCESS;
}
#endif
/**
    It is a notification function after capsule is loaded to reprogram the PCI enumeration.

    @param  PeiServices 
    @param  NotifyDesc 
    @param  InvokePpi 

    @retval Status
**/
EFI_STATUS
EFIAPI
NotifyOnNvmeRecoveryCapsuleLoaded (
    IN  EFI_PEI_SERVICES          **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN  VOID                      *InvokePpi
)
{
#if defined(PeiPciEnumeration_SUPPORT) && (PeiPciEnumeration_SUPPORT == 1)
    EFI_STATUS      Status;
#else
    UINT8       Index;
    UINT8       Index1;
    UINT8       BusNumber = 0;
    UINT16      BaseAddrLimit = 0;
    UINT8       CmdReg;
    UINT8       Bus;
    UINT8       Device;
    UINT8       Function;
#endif

#if defined(PeiPciEnumeration_SUPPORT) && (PeiPciEnumeration_SUPPORT == 1)
    Status =   gPeiPciEnumerationPpi->ResetDeviceResources(gPeiPciEnumerationPpi);
    ASSERT_PEI_ERROR(PeiServices, Status);
    return Status;
#else

    // Program the bridge's Prim./Sec./Subordinate bus number to Zero.
    // and reprogram the membase/memlinit, Command reg.
    for(Index=0; Index<NVME_MAXIMUM_PCI_ROOT_BRIDGE ;Index++) {
        if( gNvmePciBridgeInfo[Index] ) {
            gPciCfg->Write (gPeiServices, \
                            gPciCfg, \
                            EfiPeiPciCfgWidthUint8, \
                            PEI_PCI_CFG_ADDRESS (gNvmePciBridgeInfo[Index]->PrimaryBusNumber,
                                    gNvmePciBridgeInfo[Index]->Device, gNvmePciBridgeInfo[Index]->Function, PCI_PBUS),\
                            &BusNumber);

            gPciCfg->Write (gPeiServices, \
                            gPciCfg, \
                            EfiPeiPciCfgWidthUint8, \
                            PEI_PCI_CFG_ADDRESS (gNvmePciBridgeInfo[Index]->PrimaryBusNumber,
                                    gNvmePciBridgeInfo[Index]->Device, gNvmePciBridgeInfo[Index]->Function, PCI_SBUS),\
                            &BusNumber);

            gPciCfg->Write (gPeiServices, \
                            gPciCfg, \
                            EfiPeiPciCfgWidthUint8, \
                            PEI_PCI_CFG_ADDRESS (gNvmePciBridgeInfo[Index]->PrimaryBusNumber,
                                    gNvmePciBridgeInfo[Index]->Device, gNvmePciBridgeInfo[Index]->Function, PCI_SUBUS),\
                            &BusNumber);

             for(Index1=0; Index1<NVME_MAXIMUM_PCI_ROOT_BRIDGE ;Index1++) {
                if( gNvmePciProgrammedBridgeInfo[Index1] ) {
                    if( (gNvmePciProgrammedBridgeInfo[Index1]->PrimaryBusNumber ==  gNvmePciBridgeInfo[Index]->PrimaryBusNumber) &&
                        (gNvmePciProgrammedBridgeInfo[Index1]->Device ==  gNvmePciBridgeInfo[Index]->Device) &&
                        (gNvmePciProgrammedBridgeInfo[Index1]->Function ==  gNvmePciBridgeInfo[Index]->Function) ) {
                        

                        Bus      = gNvmePciBridgeInfo[Index]->PrimaryBusNumber;
                        Device   = gNvmePciBridgeInfo[Index]->Device;
                        Function = gNvmePciBridgeInfo[Index]->Function;

                        gPciCfg->Read(gPeiServices, \
                                      gPciCfg, \
                                      EfiPeiPciCfgWidthUint8,\
                                      PEI_PCI_CFG_ADDRESS(Bus,Device,
                                      Function,PCI_CMD),\
                                      &CmdReg);

                        CmdReg &= NVME_MASK_MEM_BUS_MASTER;
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

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
