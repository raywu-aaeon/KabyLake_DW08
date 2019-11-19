//*************************************************************************
//**                                                                     **
//**   Copyright (c) 1985-2019, American Megatrends International LLC.   **
//**                                                                     **
//**      All rights reserved. Subject to AMI licensing agreement.       **
//**                                                                     **
//*************************************************************************

//*************************************************************************
/** @file AmiBdsPlatformLib
    This file provides Elink functions of BDS_CONTROL_FLOW

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <BootOptions.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Protocol/AcpiS3Save.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadFile.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/FastBootProtocol2.h>
#include <Library/PciLib.h>
#include <Library/UefiLib.h>
#include "Register/PchRegsPmc.h"
#include <SetupVariable.h>

#if FAST_BOOT_SUPPORT
extern FAST_BOOT_POLICY2    *gFastBootPolicy;
#endif
#define READ_PCI16(Bx, Dx, Fx, Rx)          PciRead16 (PCI_LIB_ADDRESS(Bx, Dx, Fx, Rx))
#define DEFAULT_PCI_BUS_NUMBER_PCH  0

// Functions are defined in Bds.c
VOID SignalProtocolEvent(IN EFI_GUID *ProtocolGuid);

VOID 
EFIAPI 
InstallConsoleStartedProtocol(
    CHAR16*   ConDevVar, 
    EFI_GUID* ProtocolGuid
);

extern EFI_GUID BdsConnectDriversProtocolGuid;
extern EFI_GUID ConInStartedProtocolGuid;
extern EFI_GUID ConOutStartedProtocolGuid;

// Local functions
EFI_STATUS FwLoadFile2 (
    IN EFI_LOAD_FILE_PROTOCOL *This, IN EFI_DEVICE_PATH_PROTOCOL *FilePath,
    IN BOOLEAN BootPolicy, IN OUT UINTN *BufferSize,
    IN VOID *Buffer OPTIONAL
);

// Since Bds declares NumberOfFvHandles and FvHandles to static, using library can't use them.
// So replace InstallFwLoadFile here for static variables.
static UINTN NumberOfFvHandles2 = 0;
static EFI_HANDLE *FvHandles2 = NULL;

// Global Variables
EFI_LOAD_FILE_PROTOCOL FwLoadFileInterface2  = {FwLoadFile2};

struct {
    VENDOR_DEVICE_PATH Media;
    EFI_DEVICE_PATH_PROTOCOL End;
} FwLoadFileDp2 = {
    {
        {
            MEDIA_DEVICE_PATH, MEDIA_VENDOR_DP,
            sizeof(VENDOR_DEVICE_PATH)
        },
        AMI_MEDIA_DEVICE_PATH_GUID
    },
    {
        END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
        sizeof(EFI_DEVICE_PATH_PROTOCOL)
    }
};

//----------------------------------------------------------------------------
static VOID DummyEndOfDxeCallback(IN EFI_EVENT Event, IN VOID *Context){}

/**
 * Create and signal the event for the End of DXE event to allow items in the system to
 * perform the necessary security lock down of their interfaces
 */
VOID SignalEventsForIntelPlatform(VOID){
    EFI_EVENT Event;
    EFI_STATUS Status;
    static BOOLEAN SignalEndOfDxeEventIsSignal = FALSE;

    if (SignalEndOfDxeEventIsSignal){
      return;
    }    
 
    Status = gBS->CreateEventEx(
        EVT_NOTIFY_SIGNAL, TPL_CALLBACK, DummyEndOfDxeCallback,
        NULL, &gEfiEndOfDxeEventGroupGuid, &Event
    );
    if(EFI_ERROR(Status)) return;
    gBS->SignalEvent(Event);
    gBS->CloseEvent(Event);
    DEBUG((DEBUG_INFO,"All EndOfDxe callbacks have returned successfully\n"));
    
    SignalEndOfDxeEventIsSignal = TRUE;
    
    DEBUG((DEBUG_INFO,"Signal EfiDxeSmmReadyToLockProtocol\n"));
    SignalProtocolEvent(&gEfiDxeSmmReadyToLockProtocolGuid);
}

/**
 * Signal the Connect Drivers protocol event and the DXE SMM Ready to Lock protocol event
 */
VOID SignalConnectDriversEventForIntelPlatform(){
    EFI_STATUS Status;
    EFI_ACPI_S3_SAVE_PROTOCOL *AcpiS3Save;
    
    PROGRESS_CODE(DXE_BDS_CONNECT_DRIVERS);
    
    DEBUG((DEBUG_INFO,"SignalConnectDriversEvent for IntelPlatform\n")); 
    
    // ACPI module starting from label ACPI_10 is aligned with the EDKII S3 save/resume implementation,
    // which depends on this call prior to signaling EndOfDxe event.
    if (!EFI_ERROR(gBS->LocateProtocol(&gEfiAcpiS3SaveProtocolGuid, NULL, (VOID **) &AcpiS3Save))){
        AcpiS3Save->S3Save(AcpiS3Save,NULL);
    }
    
    SignalProtocolEvent(&BdsConnectDriversProtocolGuid);

#if 0 // For Intel platform, these events have to signal after ConOutStartedProtocol install.
    SignalEndOfDxeEvent();
    SignalProtocolEvent(&gEfiDxeSmmReadyToLockProtocolGuid);
#endif 

    // Initialize FvHandles array.
    // The array is used by the FwLoadFile function.
    Status = gBS->LocateHandleBuffer (
        ByProtocol, &gEfiFirmwareVolume2ProtocolGuid,
        NULL,  &NumberOfFvHandles2, &FvHandles2
    );
    if(EFI_ERROR(Status)){
        FvHandles2 = NULL;
        NumberOfFvHandles2 = 0;
    }
}

/**
 * Protocol implementation for the FwLoadFile instance that the system creates for
 * the Built In UEFI Shell.
 *
 * @param This Pointer to the EFI_LOAD_FILE_PROTOCOL
 * @param FilePath The device path
 * @param BootPolicy The boot Policy
 * @param BufferSize The size of the buffer to fill with the boot executable
 * @param Buffer The buffer to fill with the boot executable
 *
 * @retval EFI_INVALID_PARAMETER one or more of the parameters are bad
 * @retval EFI_UNSUPPORTED The passed device path is not supported
 * @retval EFI_NOT_FOUND The load image could not be found
 * @retval EFI_BUFFER_TOO_SMALL The buffer is too small to fix the load image
 * @retval EFI_SUCCESS The buffer was successfully filled with the load image
 */
EFI_STATUS FwLoadFile2 (
    IN EFI_LOAD_FILE_PROTOCOL *This, IN EFI_DEVICE_PATH_PROTOCOL *FilePath,
    IN BOOLEAN BootPolicy, IN OUT UINTN *BufferSize,
    IN VOID *Buffer OPTIONAL
){
    EFI_GUID *FileGuid;
    UINTN i;
    EFI_STATUS Status;
    UINTN TmpBuffer = 1;

    if (!BufferSize || (*BufferSize && !Buffer))
        return EFI_INVALID_PARAMETER;
    if (!FilePath || isEndNode(FilePath)){
        FileGuid = PcdGetPtr(PcdShellFile);
        if (FileGuid==NULL) return EFI_UNSUPPORTED;
    }else{
        if (   FilePath->Type!=MEDIA_DEVICE_PATH
            || FilePath->SubType!=MEDIA_FV_FILEPATH_DP
        ) return EFI_INVALID_PARAMETER;
        FileGuid = &((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)FilePath)->FvFileName;
    }
    //If Buffer is NULL, ReadSection will allocate the memory.
    //That's not what we need.
    //Initialize Buffer with some value.
    //We don't care what value is that because *BufferSize is 0 anyway,
    //so nothing will be copied into the buffer.
    //We know that *BufferSize is always 0 for NULL buffer because we checked that
    //at the beginning of the routine.
    if (!Buffer) Buffer = (VOID*)TmpBuffer;

    // Since this protocol is installed when we are past our trust boundary, don't try to process
    // all available FV instances. Just deal with the known FV.
    // FvHandles array is initialized right after signaling DxeSmmReadyToLock event (SignalConnectDriversEvent).
    Status=EFI_NOT_FOUND;
    for(i=0; i<NumberOfFvHandles2; i++)
    {
        EFI_FIRMWARE_VOLUME_PROTOCOL *Fv;
        UINT32 AuthStatus;
        Status = gBS->HandleProtocol(FvHandles2[i], &gEfiFirmwareVolume2ProtocolGuid, (VOID**)&Fv);
        if (EFI_ERROR(Status)) continue;
        Status = Fv->ReadSection(
            Fv, FileGuid, EFI_SECTION_PE32,
            0, &Buffer, BufferSize, &AuthStatus
        );
        if (!EFI_ERROR(Status)){
            if (Status==EFI_WARN_BUFFER_TOO_SMALL) Status=EFI_BUFFER_TOO_SMALL;
            break;
        }
    }
    return Status;
}

/**
 * Helper functions used by InstallFwLoadFile to install the LoadFile Protocol for the Built in EFI Shell.
 * InstallFwLoadFile skips protocol installation when SecureBoot is enabled.
 * If platform policy is to install the protocol on SecureBoot,
 * replace "InstallFwLoadFile," eLink with the "InstallFwLoadFileProtocol," eLink.
 */
VOID InstallFwLoadFileProtocol2(){
    EFI_HANDLE Handle=NULL;

    gBS->InstallMultipleProtocolInterfaces(
        &Handle,
        &gEfiLoadFileProtocolGuid, &FwLoadFileInterface2,
        &gEfiDevicePathProtocolGuid, &FwLoadFileDp2,
        NULL
    );
}

/**
 * Install the LoadFileProtocol for the Built in EFI Shell
 */
VOID InstallFwLoadFile2(){
    EFI_STATUS Status;
    UINT8 Data = 0;
    UINTN Size = sizeof(UINT8);
    Status = gRT->GetVariable(EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &Data);
    if(!EFI_ERROR(Status) && (Data == 1))
        return;
    InstallFwLoadFileProtocol2();
}

#if FAST_BOOT_SUPPORT
/**
 *  This function connects all PCI handles excpet PCI devices in 
 *  FAST_BOOT_PCI_SKIP_LIST.
 *    
 */
VOID 
FastbootConnectEveryPciHandles()
{
    EFI_STATUS  Status;
    UINTN Number;
    EFI_HANDLE *Handle;    
    UINTN HandleCounter, SkipTableCounter;  
    SKIP_PCI_LIST2 *SkipPciList = gFastBootPolicy->SkipPciList;

    DEBUG((DEBUG_INFO,"FastbootConnectEveryPciHandles enter\n")); 
    
    Status = gBS->LocateHandleBuffer(
        ByProtocol, &gEfiPciIoProtocolGuid, NULL, &Number, &Handle
    );
    if (EFI_ERROR(Status)) return;

     for ( HandleCounter = 0; HandleCounter < Number; HandleCounter++)
    {
        EFI_PCI_IO_PROTOCOL *PciIo;
        UINT8 PciClass[4];
        Status = gBS->HandleProtocol( Handle[HandleCounter], &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
        if (EFI_ERROR(Status)) continue;
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_REV_ID_OFFSET, 1, &PciClass);

        ///check skip table
        for (SkipTableCounter = 0; SkipTableCounter < (gFastBootPolicy->SkipPciListSize / (sizeof(SKIP_PCI_LIST2))); SkipTableCounter++) {

            if (SkipPciList[SkipTableCounter].SubClass == 0xFF && PciClass[3] == SkipPciList[SkipTableCounter].Class) 
                break;
                
            if (PciClass[3] == SkipPciList[SkipTableCounter].Class && PciClass[2] == SkipPciList[SkipTableCounter].SubClass)
                break;
        }

        if ( SkipTableCounter < (gFastBootPolicy->SkipPciListSize / (sizeof(SKIP_PCI_LIST2))))
            continue;
                
        ///check fastboot policy
        if (gFastBootPolicy->UsbSupport == 0 && PciClass[3] == PCI_CL_SER_BUS && PciClass[2] == PCI_CL_SER_BUS_SCL_USB)
            continue;    

        if (PciClass[3] == PCI_CL_NETWORK && gFastBootPolicy->NetworkStackSupport == 0) {
            gBS->ConnectController(Handle[HandleCounter], NULL, NULL, FALSE);
            continue;
        }
        
        ///all pass, then we connect this controller
        gBS->ConnectController(Handle[HandleCounter], NULL, NULL, TRUE);        
                
    }

    gBS->FreePool(Handle);   
    
    DEBUG((DEBUG_INFO,"FastbootConnectEveryPciHandles exit\n")); 
}

/**
 *  This function connects handles in the system.
 *    
 */
VOID 
EFIAPI 
FastbootConnectEverything()
{
	FastbootConnectEveryPciHandles();

/// Signal it anyway for Consplitter to take care the ConIn/ConOut 
/// after everything is connected
    InstallConsoleStartedProtocol(NULL, &ConOutStartedProtocolGuid);
    InstallConsoleStartedProtocol(NULL, &ConInStartedProtocolGuid);
    
}
#endif

#if OVER_CLOCK_SUPPORT
void DisableOCWdt(VOID)
{
	EFI_STATUS                Status;
	UINT32                    OcWdtCtlReg;
	UINT16                    PmBase;
	UINTN                     VariableSize;
	SETUP_DATA                SetupData;
	UINT32                    gSetupVariableAttributes;
	
	VariableSize = sizeof (SETUP_DATA);
	Status = gRT->GetVariable (
	                  L"Setup",
	                  &gSetupVariableGuid,
	                  &gSetupVariableAttributes,
	                  &VariableSize,
	                  &SetupData
	                  );
	
	if ((!EFI_ERROR (Status)) && (SetupData.OverclockingSupport == 0)) {
	    return;
	}
	PmBase = READ_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, 
			PCI_DEVICE_NUMBER_PCH_PMC, 
			PCI_FUNCTION_NUMBER_PCH_PMC, 
			R_PCH_PMC_ACPI_BASE) & B_PCH_PMC_ACPI_BASE_BAR;
	
	OcWdtCtlReg = IoRead32(PmBase + R_PCH_OC_WDT_CTL);
	OcWdtCtlReg &= ~(B_PCH_OC_WDT_CTL_UNXP_RESET_STS | B_PCH_OC_WDT_CTL_FORCE_ALL | B_PCH_OC_WDT_CTL_EN);
	IoWrite32(PmBase + R_PCH_OC_WDT_CTL, OcWdtCtlReg);
	DEBUG ((DEBUG_INFO, "Disable OC Wdt before connecteverything.\n"));
}
#endif

