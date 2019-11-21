//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file NbDxe.c
    This file contains code for North Bridge initialization
    in the DXE stage

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------
#include <Efi.h>
#include <Token.h>
#include <Dxe.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/NBPlatformData.h>
#include <Protocol/Wdt.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/AmiCsmOpromPolicy.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/VariableLock.h>
#include <Library/PchInfoLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <SaDataHob.h>
#include <MrcInterface.h>
#include <Protocol/MemInfo.h>
#include <MemInfoHob.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/SmmAccess2.h>
#include <Guid/NbChipsetGuid.h>
#include <Guid/HobList.h>
#include <SaRegs.h>
#include <PchAccess.h>
#include <PlatformBoardId.h>
#include <Setup/SaSetup.h>
#include <AcpiOemElinks.h>
#include <Register/Cpuid.h>
#include <Protocol/EdidActive.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/SaPolicy.h>
#if ACPI_SUPPORT
 #include <Protocol/AcpiSystemDescriptionTable.h>
#endif
#if NB_ERROR_LOG_SUPPORT
#include <Pci.h>
#include <PciE.h>
#endif
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#if USB_DRIVER_MAJOR_VER > 19
#include <Protocol/AmiUsbHcMemRecordProtocol.h>
#endif
#endif
#if defined (VMM_RESERVED_MEMORY_MTRR_UC) && (VMM_RESERVED_MEMORY_MTRR_UC == 1) // EIP405946
#include <Protocol/Cpu.h>
#include <Base.h>
#endif  //#if defined (VMM_RESERVED_MEMORY_MTRR_UC) && (VMM_RESERVED_MEMORY_MTRR_UC == 1)

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)
#define MAX_TOLUD_0_5G       20 
#define MAX_TOLUD_0_75G      21 

#ifndef SMM_SUPPORT
  #define SMM_SUPPORT 0
#endif

#if defined(ENABLE_CLEAR_IGFX_FRAME_BUFFER) && (ENABLE_CLEAR_IGFX_FRAME_BUFFER == 1)
  #define MIN_EDID_BUFFER_SIZE           128
  #define HORIZONTAL_ACTIVE_OFFSET_3A    0x3A
  #define HORIZONTAL_ACTIVE_OFFSET_38    0x38
  #define HORIZONTAL_ACTIVE_OFFSET_3D    0x3D
  #define HORIZONTAL_ACTIVE_OFFSET_3B    0x3B
  #define RGB_32_BIT                     4
#endif

// Macro Definition(s)

// Type Definition(s)
typedef struct {
    UINT8 Bus;
    UINT8 Dev;
    UINT8 Fun;
    VOID *Process;
} DEVICES_AFTER_PCIIO;

// Function Prototype(s)
EFI_STATUS 
NbDxeBoardInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
);

VOID NbInitAfterDeviceInstall (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

VOID
EFIAPI
NbInitEndOfDxe (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

VOID InitNbRegsBeforeBoot (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

EFI_STATUS InstallNbMemoryInfo (
    IN EFI_SYSTEM_TABLE *SystemTable
);

VOID CreateNbAcpiComponent ( VOID );

EFI_STATUS NbSmmInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
);

VOID NbSetupNvramUpdatedCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

VOID NbAmiConOutStartedProtocolCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

EFI_STATUS NbDxeShadowRam (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
);

EFI_STATUS CreateNbPlatformInfo (
    IN NB_SETUP_DATA    *NbSetupData
);

VOID NbVariableToLock (
    IN EFI_EVENT        Event,
    IN VOID             *Context 
);

#if NB_PCIE_ERROR_LOG_SUPPORT

static SETUP_DATA                   gSetupData = {0};

VOID NbPciExpressDeviceInitialize (
    IN UINT64 Address
);
#endif

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

EFI_S3_SAVE_STATE_PROTOCOL      *gBootScript = NULL;
EFI_EVENT                       gEvent;
VOID                            *gNBInitNotifyReg = NULL;
NB_SETUP_DATA                   *gNbSetupData = NULL;
VOID                            *gNbSetupNvramUpdatedReg = NULL;
VOID                            *gAmiConOutStartedProtocolReg = NULL;
VOID                            *gInterface = NULL;
VOID                            *gNbVariableToLockReg = NULL;
MEM_INFO_PROTOCOL               gMemInfoHobProtocol;
NB_PLATFORM_DATA                NbPlatformData = {0};
UINT32                          gBClkFrequency = 0;
MrcProfile                      MemoryProfile = 0;
MRC_CH_TIMING                   *NBMrcTimingData;
EDKII_VARIABLE_LOCK_PROTOCOL    *gVariableLockProtocol = NULL;
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#if USB_DRIVER_MAJOR_VER > 19
EFI_EVENT                       gEvtUsbHcMemRecordProtocol  = NULL;
VOID                            *gRegUsbHcMemRecordProtocol = NULL;
EFI_GUID 						gAmiUsbHcMemRecordProtocolGuid = AMI_USB_HC_MEM_RECORD_PROTOCOL_GUID;
#endif
#endif
// GUID Definition(s)

EFI_GUID gEfiPciIoProtocolGuid           = EFI_PCI_IO_PROTOCOL_GUID;
EFI_GUID gSetupNvramUpdatedGuid          = AMITSE_NVRAM_UPDATE_GUID;
EFI_GUID gSetupGuid                      = SETUP_GUID;
EFI_GUID gEfiLegacyBiosPlatformProtocolGuid = EFI_LEGACY_BIOS_PLATFORM_PROTOCOL_GUID;
EFI_GUID gConOutStartedCheckGuid         = CONSOLE_OUT_DEVICES_STARTED_PROTOCOL_GUID;
EFI_GUID gAmiBeforeBootProtocolGuid      = AMITSE_EVENT_BEFORE_BOOT_GUID;

extern EFI_GUID  gMemoryConfigVariableGuid;
extern EFI_GUID  gPegConfigVariableGuid;
// AMI_OVERRIDE_NB0000_START >>>
//----------------------------------------------------------------------------
//THIS TABLE MUST BE FILLED WITH RESOURCE REGIONS DECODED BY THE NB FOR ITSELF
//!!! NOTE :Use Attributes field == -1 to mark an IO Space Resource. 
//          you must provide valid attributes for Memory Mapped IO regions. 
//----------------------------------------------------------------------------

CSP_RES_ITEM gNbCspResTable[] = {
//----------------------------------------------------------------------------
// UINT64 ResBase  UINTN ResLength  GCD_MEMORY_TYPE ResType  UINT64 Attributes
//----------------------------------------------------------------------------
#ifdef AMI_PCIE_DYNAMIC_SUPPORT_FLAG
//Add PCI Express region
{ 0xE0000000, 0x10000000, EfiGcdMemoryTypeMemoryMappedIo , \
                                         (EFI_MEMORY_UC|EFI_MEMORY_RUNTIME)},
#else
0
#endif
};

UINTN   gNbCspResCount = sizeof(gNbCspResTable) / sizeof(CSP_RES_ITEM);
// AMI_OVERRIDE_NB0000_END <<<

DEVICES_AFTER_PCIIO gDevicesTable[] = {
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN,  NULL },
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN1, NULL },
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN2, NULL }
};

UINTN gDevicesTableCount = sizeof(gDevicesTable) / sizeof(DEVICES_AFTER_PCIIO);

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//----------------------------------------------------------------------------
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#if USB_DRIVER_MAJOR_VER > 19
/**
    This callback function is called after USB HcMemRecord Protocol is
    installed.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID NbUsbHcMemRecordCallback (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context)
{
    EFI_STATUS                          Status;
    UINTN                               BufferSize = sizeof(EFI_HANDLE);
    EFI_HANDLE                          Handle;
    EFI_PCI_IO_PROTOCOL                 *PciIo;
    UINTN                               PciSeg, PciBus, PciDev, PciFun;    
    AMI_USB_HC_MEM_RECORD_PROTOCOL      *AmiUsbHcMemRecordProtocol;
    SA_POLICY_PROTOCOL                  *SaPolicy;
    MISC_DXE_CONFIG                     *MiscDxeConfig;

    DEBUG((DEBUG_INFO, "NBDXE: NbUsbHcMemRecordCallback\n"));

    Status = pBS->LocateHandle (ByRegisterNotify,
                                NULL,
                                gRegUsbHcMemRecordProtocol,
                                &BufferSize,
                                &Handle);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_WARN, "NBDXE: No handle for Ami UsbHcMemRecord Protocol\n"));
      return;
    }

    Status = pBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    ASSERT_EFI_ERROR(Status);

    Status = PciIo->GetLocation (PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
    ASSERT_EFI_ERROR(Status);

    if ((PciBus == PCI_BUS_NUMBER_PCH_XHCI) && (PciDev == PCI_DEVICE_NUMBER_PCH_XHCI) && (PciFun == PCI_FUNCTION_NUMBER_PCH_XHCI)) {
      Status = pBS->HandleProtocol (Handle, &gAmiUsbHcMemRecordProtocolGuid, (VOID **)&AmiUsbHcMemRecordProtocol);
      ASSERT_EFI_ERROR(Status);
      
      Status = pBS->LocateProtocol (&gSaPolicyProtocolGuid, NULL, (VOID **)&SaPolicy);
      ASSERT_EFI_ERROR(Status);
      
      Status = GetConfigBlock ((VOID *)SaPolicy, &gMiscDxeConfigGuid, (VOID *)&MiscDxeConfig);
      ASSERT_EFI_ERROR (Status);
      
      MiscDxeConfig->RmrrUsbBaseAddress[0] = AmiUsbHcMemRecordProtocol->HcMemStart;
      MiscDxeConfig->RmrrUsbBaseAddress[1] = AmiUsbHcMemRecordProtocol->HcMemStart + EFI_PAGES_TO_SIZE (AmiUsbHcMemRecordProtocol->MemTotalPages) - 1;
      
      DEBUG ((DEBUG_INFO, "RMRR Base  address of Intel XHCI %016lX\n", MiscDxeConfig->RmrrUsbBaseAddress[0]));
      DEBUG ((DEBUG_INFO, "RMRR Limit address of Intel XHCI %016lX\n", MiscDxeConfig->RmrrUsbBaseAddress[1]));
      
      pBS->CloseEvent(Event);
    }
}
#endif
#endif

/**
    This function is the entry point for this DXE. This function
    initializes the chipset NB before PCI Bus enumeration.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.
**/

EFI_STATUS EFIAPI NbDxeInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable)
{
    EFI_STATUS          Status = EFI_SUCCESS;    
    UINT8               i = 0;
    UINTN               VariableSize;
    EFI_EVENT           Event;
    EFI_EVENT           EndOfDxeEvent;
    CHAR16              MemoryCeilingVariable[] = L"MemCeil.";
    UINT32              MemoryCeiling = 0; // Init to 0  
    UINTN               DataSize = sizeof(UINT32);
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;    

    InitAmiLib(ImageHandle, SystemTable);

    PROGRESS_CODE (DXE_NB_INIT);

    Status = pBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, \
                                  NULL, \
                                  (VOID **)&PciRootBridgeIo);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->LocateProtocol (&gEfiS3SaveStateProtocolGuid, \
                                  NULL, \
                                  (VOID **)&gBootScript);
    ASSERT_EFI_ERROR(Status);

    // Read the NB Setup Data
    VariableSize = sizeof (NB_SETUP_DATA);
    Status = pBS->AllocatePool (EfiBootServicesData, VariableSize, (VOID **)&gNbSetupData);
    ASSERT_EFI_ERROR(Status);

    GetNbSetupData (pRS, gNbSetupData, FALSE);

    if (gNbSetupData->MaxTolud == 0) {
        // Delete Variable of org memory ceiling variable.
        Status = pRS->SetVariable( MemoryCeilingVariable,
                                   &gAmiChipsetVariableGuid,
                                   0,
                                   0,
                                   NULL );
    }
    
    // Set memory ceiling variable.
    if(gNbSetupData->MaxTolud) {
        UINT32 MmioSize = 0x400;        

        switch (gNbSetupData->MaxTolud) {
          case MAX_TOLUD_0_5G:
            MmioSize = 0xE00;
            break;
          case MAX_TOLUD_0_75G:
            MmioSize = 0xD00;
            break;
          case MAX_TOLUD_1G:
            MmioSize = 0xC00;
            break;
          case MAX_TOLUD_1_25G:
            MmioSize = 0xB00;
            break;
          case MAX_TOLUD_1_5G:
            MmioSize = 0xA00;
            break;
          case MAX_TOLUD_1_75G:
            MmioSize = 0x900;
            break;
          case MAX_TOLUD_2G:
            MmioSize = 0x800;
            break;
          case MAX_TOLUD_2_25G:
            MmioSize = 0x700;
            break;
          case MAX_TOLUD_2_5G:
            MmioSize = 0x600;
            break;
          case MAX_TOLUD_2_75G:
            MmioSize = 0x500;
            break;
          case MAX_TOLUD_3G:
            MmioSize = 0x400;
            break;
          case MAX_TOLUD_3_25G:
            MmioSize = 0x300;
            break;
          case MAX_TOLUD_3_5G:
            MmioSize = 0x200;
            break;
        }
        
        //AMI_OVERRIDE_START >>>
        if((MmioSize <= 0x200) && (PCIEX_BASE_ADDRESS == 0xE0000000)){
            MmioSize += 0x100;
        }
        //AMI_OVERRIDE_END <<<
          
        Status = pRS->GetVariable ( MemoryCeilingVariable,
                                   &gAmiChipsetVariableGuid,
                                   NULL,
                                   &DataSize,
                                   &MemoryCeiling );
        if (EFI_ERROR(Status)) {
            MemoryCeiling = 0xffffffff - ((UINT32)Shl64(MmioSize, 20)) + 1;
            if (MemoryCeiling > PCIEX_BASE_ADDRESS) MemoryCeiling = PCIEX_BASE_ADDRESS;
            Status = pRS->SetVariable( MemoryCeilingVariable,
                                       &gAmiChipsetVariableGuid,
                                       EFI_VARIABLE_NON_VOLATILE |
                                       EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                       sizeof (UINT32),
                                       &MemoryCeiling );
        }
    }
    
    Status = NbDxeBoardInit(ImageHandle, SystemTable);
    
    //
    // Detect if CSM is exist. If it's not, the shawdow ram can be set. 
    //
    Status = pBS->LocateProtocol (&gEfiLegacyBiosPlatformProtocolGuid, NULL, (VOID **)&gInterface);
    if(EFI_ERROR(Status)) {
      Status = NbDxeShadowRam(ImageHandle, SystemTable);
    }
    
    CreateNbAcpiComponent();

    Status = RegisterProtocolCallback (&gEfiPciIoProtocolGuid,
                                       NbInitAfterDeviceInstall,
                                       NULL, &gEvent, &gNBInitNotifyReg);
    ASSERT_EFI_ERROR(Status);

    Status = RegisterProtocolCallback (&gAmiConOutStartedProtocolGuid,
                                       NbAmiConOutStartedProtocolCallback,
                                       NULL, &gEvent, &gAmiConOutStartedProtocolReg);
    ASSERT_EFI_ERROR(Status);
    
    Status = pBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    NbInitEndOfDxe,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );    
    ASSERT_EFI_ERROR(Status);
    
    Status = CreateReadyToBootEvent (TPL_NOTIFY, InitNbRegsBeforeBoot,
                                     NULL, &gEvent);
    ASSERT_EFI_ERROR(Status);

    Status = InstallNbMemoryInfo (SystemTable);
    ASSERT_EFI_ERROR(Status);

    CreateNbPlatformInfo (gNbSetupData);    // Please let it after InstallNbMemoryInfo.

    Status = RegisterProtocolCallback( &gSetupNvramUpdatedGuid,
                                       NbSetupNvramUpdatedCallback,
                                       NULL,
                                       &Event,
                                       &gNbSetupNvramUpdatedReg );
#ifdef AMI_PCIE_DYNAMIC_SUPPORT_FLAG
    gNbCspResTable->ResBase = PCIEX_BASE_ADDRESS;
    gNbCspResTable->ResLength = PCIEX_LENGTH;
    Status = LibAllocCspResource( gNbCspResTable, \
                                  gNbCspResCount, \
                                  ImageHandle, \
                                  SystemTable );
#endif
    
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#if USB_DRIVER_MAJOR_VER > 19
    Status = RegisterProtocolCallback (&gAmiUsbHcMemRecordProtocolGuid,
                                       NbUsbHcMemRecordCallback,
                                       NULL,
                                       &gEvtUsbHcMemRecordProtocol,
                                       &gRegUsbHcMemRecordProtocol);
    ASSERT_EFI_ERROR(Status);
#endif	
#endif
    
    //
    // Nb Variable to locks.
    //
    Status = pBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &gVariableLockProtocol);
    if (!EFI_ERROR (Status)) {
        NbVariableToLock(NULL, NULL);
    } else {
        Status = RegisterProtocolCallback( &gEdkiiVariableLockProtocolGuid,
                                           NbVariableToLock,
                                           NULL,
                                           &Event,
                                           &gNbVariableToLockReg );
    }
    
#if SMM_SUPPORT
    Status = NbSmmInit(ImageHandle, SystemTable);
#endif

#if defined (VMM_RESERVED_MEMORY_MTRR_UC) && (VMM_RESERVED_MEMORY_MTRR_UC == 1) // EIP405946
    {
        EFI_PHYSICAL_ADDRESS    VmmReservedMemory = 0x40000000;
        UINT64                  VmmReservedMemorySize = 0x400;
        EFI_CPU_ARCH_PROTOCOL   *CpuArchProtocol;

        Status = pBS->AllocatePages (AllocateAddress, \
                                     EfiReservedMemoryType, \
                                    VmmReservedMemorySize, \
                                    &VmmReservedMemory);
        DEBUG((DEBUG_INFO, "Virtual Machine Monitor (VMM) reserve the host physical address\n"));
        DEBUG((DEBUG_INFO, "Base: 0x%lX, Size: 0x%lX, Status: %r\n", VmmReservedMemory, (VmmReservedMemorySize * SIZE_4KB), Status));

        if (!EFI_ERROR (Status))
        {        
            Status = pBS->LocateProtocol (&gEfiCpuArchProtocolGuid, \
                                          NULL, \
                                          (VOID **) &CpuArchProtocol);
            DEBUG((DEBUG_INFO, "LocateProtocol[CpuArchProtocol] Status: %r\n"));

            if (!EFI_ERROR (Status))
            {
                CpuArchProtocol->SetMemoryAttributes(CpuArchProtocol, \
                                                     VmmReservedMemory, \
                                                     (VmmReservedMemorySize * SIZE_4KB), \
                                                     EFI_MEMORY_UC);
                DEBUG((DEBUG_INFO, "CpuArchProtocol->SetMemoryAttributes[Base: 0x%lX, Size: 0x%lX] Status: %r\n", VmmReservedMemory, (VmmReservedMemorySize * SIZE_4KB), Status));
            }
        }
        
    }
#endif  //#if defined (VMM_RESERVED_MEMORY_MTRR_UC) && (VMM_RESERVED_MEMORY_MTRR_UC == 1)

    return EFI_SUCCESS;
}


/**
    This function will lock this NB Variable.
    
    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID
**/
VOID NbVariableToLock (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
#if !defined(MDEPKG_NDEBUG)
    EFI_STATUS                    Status;
#endif


    //Kill the Event
    if(Event != NULL)
        pBS->CloseEvent(Event);

    if(gVariableLockProtocol == NULL)
        pBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, 
                             NULL, 
                             (VOID **)&gVariableLockProtocol);
    
    if (gVariableLockProtocol != NULL) {
#if defined(MDEPKG_NDEBUG)
        gVariableLockProtocol->RequestToLock (gVariableLockProtocol, 
                                                               L"MemoryConfig", 
                                                               &gMemoryConfigVariableGuid);
#elif !defined(MDEPKG_NDEBUG)
        Status = gVariableLockProtocol->RequestToLock (gVariableLockProtocol, 
                                                       L"MemoryConfig", 
                                                       &gMemoryConfigVariableGuid);
        DEBUG((DEBUG_INFO, "NbDxe: MemoryConfig Variable to Lock. Status = %r.\n", Status));
        ASSERT_EFI_ERROR (Status);

        Status = gVariableLockProtocol->RequestToLock (gVariableLockProtocol, 
                                                     L"SaPegData", 
                                                     &gPegConfigVariableGuid);
        DEBUG((DEBUG_INFO, "NbDxe: SaPegData Variable to Lock. Status = %r.\n", Status));
        ASSERT_EFI_ERROR (Status);
#endif
    }
}

/**
  This function handles NbDxeInit task at the end of DXE

  @param[in]  Event
  @param[in]  *Context
**/
VOID
EFIAPI
NbInitEndOfDxe (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
#if NB_ERROR_LOG_SUPPORT
#if NB_PCIE_ERROR_LOG_SUPPORT    
    UINTN       i;    
    UINT8       PegBus;
    UINT8       PegDev;
    UINT8       PegFun;
    UINTN       PegAddress;
    UINT8       CapPtr = 0;
    UINT32      Data32;
    EFI_GUID    SetupGuid = SETUP_GUID;
    UINTN       Size = sizeof(SETUP_DATA);
    UINT16      IsBridge;
    EFI_STATUS  Status;
#endif
    
#if NB_ERROR_LOG_SUPPORT && NB_PCIE_ERROR_LOG_SUPPORT

    Status = pRS->GetVariable ( L"Setup", \
                                &SetupGuid, \
                                NULL,\
                                &Size, \
                                &gSetupData );
    if (gSetupData.RtSystemEnable)
    {	    
      if (gSetupData.RtPciErrorEnable) 
      {	
        for (i = 0; i < gDevicesTableCount; i++) 
        {       
             PegBus = gDevicesTable[i].Bus;
             PegDev = gDevicesTable[i].Dev;
             PegFun = gDevicesTable[i].Fun;
             Data32 = READ_PCI32 (PegBus, PegDev, PegFun, PCI_VID);  
    
             // Only if this device exists and the corresponding slot status is set!
             if ((Data32 != 0xFFFFFFFF) && (READ_PCI8 (PegBus, PegDev, PegFun, R_SA_PEG_SLOTSTS_OFFSET) & BIT06))
             {
                  PegAddress = NB_PCIE_CFG_ADDRESS(PegBus, PegDev, PegFun, 0);
                  IsBridge = READ_MEM16(PegAddress + PCI_SCC);
             
                  //Set SERR/PERR for Root Bridge.
                  if (gSetupData.RtSerrEnable){
                      SET_MEM16_S3(gBootScript, PegAddress + PCI_CMD, BIT8);                  
                      DEBUG((DEBUG_INFO, "READ Primary PciCmdData (Enable PERR, BIT6)  = %x \n", READ_MEM16 (PegAddress+0x04))); 
                  }
                  if (gSetupData.RtPerrEnable){
                      SET_MEM16_S3(gBootScript, PegAddress + PCI_CMD, BIT6);                  
                      DEBUG((DEBUG_INFO, "READ Primary PciCmdData (Enable PERR, BIT6)  = %x \n", READ_MEM16 (PegAddress+0x04)));
                  }
             
                  //Check if it's bridge. If it's bridge, set 0x3E Bit0 and Bit 1 for SERR and PERR.
                  if (IsBridge == 0x0604){
                     if (gSetupData.RtSerrEnable){
                         SET_MEM8_S3(gBootScript, PegAddress + PCI_BRIDGE_CNTL, BIT1);                  
                         DEBUG((DEBUG_INFO, "READ Primary PciCmdData (Enable SERR, BIT1)  = %x \n", READ_MEM16 (PegAddress+0x0E)));
                  }
                 
                     if (gSetupData.RtPerrEnable){
                         SET_MEM8_S3(gBootScript, PegAddress + PCI_BRIDGE_CNTL, BIT0);  
                         DEBUG((DEBUG_INFO, "READ Primary PciCmdData (Enable SERR, BIT0)  = %x \n", READ_MEM16 (PegAddress+0x0E)));
                     }
                  } 
                              
                                                     
                  // if enable PCI SERR and PERR
                  if((READ_MEM16(PegAddress + PCI_CMD) & (BIT6 | BIT8)) == (BIT6 | BIT8))
                  {
                      NbPciExpressDeviceInitialize(PegAddress);
                      
                      // Clear Error status
                      WRITE_MEM8_S3(gBootScript, PegAddress + PCI_STS_ERROR, 0xff);
                      WRITE_MEM8_S3(gBootScript, PegAddress + PCI_SECSTATUS_ERROR, 0xff);
                      
                      CapPtr = NbFindCapPtr(PegAddress, PCI_CAP_ID_PCIEXP);
                  
                      if (CapPtr != 0) {
                          // Clear Device Error status
                          WRITE_MEM8_S3(gBootScript, PegAddress + CapPtr + CAP_DSTS, (BIT0 | BIT1 | BIT2));
                          // Enable the error bits of Device Control
                          if (gSetupData.RtCorrectedEnable)
                              SET_MEM8_S3(gBootScript, PegAddress + CapPtr + CAP_DCTL, BIT0);
                          if (gSetupData.RtUnCorrectedEnable)
                              SET_MEM8_S3(gBootScript, PegAddress + CapPtr + CAP_DCTL, BIT1);
                          if (gSetupData.RtFatalErrorEnable)
                              SET_MEM8_S3(gBootScript, PegAddress + CapPtr + CAP_DCTL, BIT2);
                           
                          // Enable the error bits of Root Control
                          if (gSetupData.RtCorrectedEnable)
                              SET_MEM8_S3(gBootScript, PegAddress + CapPtr + CAP_RCTL, BIT0);
                          if (gSetupData.RtUnCorrectedEnable)
                              SET_MEM8_S3(gBootScript, PegAddress + CapPtr + CAP_RCTL, BIT1);
                          if (gSetupData.RtFatalErrorEnable)
                              SET_MEM8_S3(gBootScript, PegAddress + CapPtr + CAP_RCTL, BIT2);
                       }
                  }     
              }         
          } // (i = 0; gDevicesTable[i].Bus != 0xff; i++)
        }
    }
#endif

#if NB_ERROR_LOG_SUPPORT && NB_ECC_ERROR_LOG_SUPPORT

    if (gSetupData.RtSystemEnable)
    {
      if((gNbSetupData->EccSupport) && (gSetupData.RtEccEnable)) 
      {
        if (((READ_MEM32_MCH(0x5004) & (BIT12 | BIT13)) != 0) ||
           ((READ_MEM32_MCH(0x5008) & (BIT12 | BIT13)) != 0))
        {
// [ EIP428812 ] Remove useless code.
//             RW_MEM32_MCH(0x4034, 0, (BIT14 | BIT16 | BIT17));
//             RW_MEM32_MCH(0x4434, 0, (BIT14 | BIT16 | BIT17));

             // Disable Error and SCI Commands
             RW_PCI16_NB(0xCA, 0, (BIT00 | BIT01));
             RW_PCI16_NB(0xCE, 0, (BIT00 | BIT01));
             
             // Enable SMI Command
             // SET_PCI8_NB(0xC8, BIT00);
             RW_PCI16_NB(0xCC, (BIT00 | BIT01), 0);
        }
      }
    }
#endif
#endif    
        
    pBS->CloseEvent (Event);

    return;
}

/**
    This function can initialize any NB registers before DXE
    stage exiting.

    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID
**/
VOID InitNbRegsBeforeBoot (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{   
    DEBUG((DEBUG_INFO, "NBDXE: InitNbRegsBeforeBoot.\n"));
    
    //Kill the Event
    pBS->CloseEvent(Event);
    
#if (defined ENABLE_ABOVE_4G_MEM_DURING_BIOS_POST && (ENABLE_ABOVE_4G_MEM_DURING_BIOS_POST == 0))  
{
  	EFI_HOB_RESOURCE_DESCRIPTOR        *ResourceHob = NULL;
 	DXE_SERVICES			           *DxeServices = NULL;
    EFI_STATUS                         Status;
    
    //
    // Get the HOB list for processing
    ResourceHob = GetEfiConfigurationTable(pST,&gEfiHobListGuid);
    if (!ResourceHob) return;
   	
    Status = LibGetDxeSvcTbl(&DxeServices);
    if (EFI_ERROR(Status)) return;
    
    while (!EFI_ERROR (Status = FindNextHobByType (EFI_HOB_TYPE_UNUSED, &ResourceHob))) {
        if ((ResourceHob->PhysicalStart >= BASE_4GB) &&
          (ResourceHob->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)) {
        	ResourceHob->Header.HobType = EFI_HOB_TYPE_RESOURCE_DESCRIPTOR;
			Status = DxeServices->AddMemorySpace( EfiGcdMemoryTypeSystemMemory, 
					                              ResourceHob->PhysicalStart,
					                              ResourceHob->ResourceLength, 
		                              			  (EFI_MEMORY_UC | EFI_MEMORY_WC | EFI_MEMORY_WT | EFI_MEMORY_WB));
        }
    }
    
    DEBUG((DEBUG_INFO, "NBDXE: Added memory above 4G!.\n"));
}    
#endif    
}

/**
    This routine is called to get the 16-bit offset of a 
    structure which can be located using the PCI Extended 
    Capabilities Pointer mechanism. 

    @param Bus The PCI bus number of the PCI device.
    @param Dev The PCI device number of the PCI device.
    @param Fun The PCI function number of the PCI device.
    @param FindCapNo The Extended Capability ID to be found.
    @param CapPtr16 The offset address of desired structure

        EFI_STATUS
    @retval EFI_NOT_FOUND The desired structure is not found.
**/

EFI_STATUS GetExtCapStrucAddr (
    IN UINT8                Bus,
    IN UINT8                Dev,
    IN UINT8                Fun,
    IN UINT16               FindCapNo,
    OUT UINT16              *CapPtr16 )
{
    UINT32          Buffer32;

    *CapPtr16 = 0x100;

    Buffer32 = READ_PCI32(Bus, Dev, Fun, *CapPtr16);
    while (Buffer32 != 0xffffffff) {
        if ((UINT16)Buffer32 == FindCapNo) return EFI_SUCCESS;
        *CapPtr16 = (UINT16)((Buffer32 >> 20) & 0xfffc);
        if (*CapPtr16 == 0) break;
        Buffer32 = READ_PCI32(Bus, Dev, Fun, *CapPtr16);
    }
    return EFI_NOT_FOUND;
}

/**
    This routine is called to get the 16-bit offset of a 
    structure which can be located using the PCI Legacy 
    Capabilities Pointer mechanism. 

    @param Bus The PCI bus number of the PCI device.
    @param Dev The PCI device number of the PCI device.
    @param Fun The PCI function number of the PCI device.
    @param FindCapNo The Legacy Capability ID to be found.
    @param CapPtr16 The offset address of desired structure

        EFI_STATUS
    @retval EFI_NOT_FOUND The desired structure is not found.
**/

EFI_STATUS GetLegCapStrucAddr (
    IN UINT8                Bus,
    IN UINT8                Dev,
    IN UINT8                Fun,
    IN UINT8                FindCapNo,
    OUT UINT16              *CapPtr16 )
{
    UINT8           Buffer8;

    if (READ_PCI32(Bus, Dev, Fun, PCI_VENDOR_ID_OFFSET) != 0xffffffff) {
        if (READ_PCI16(Bus, Dev, Fun, 6) & 0x10) {
            *CapPtr16 = ((READ_PCI8(Bus, Dev, Fun, 14) & 0x7f) == 2) ? \
                                                                    0x14:0x34;
            *CapPtr16 = (UINT16)READ_PCI8(Bus, Dev, Fun, *CapPtr16);
            if (*CapPtr16 == 0) return EFI_NOT_FOUND;
            Buffer8 = READ_PCI8(Bus, Dev, Fun, *CapPtr16);
            while (Buffer8 != 0) {
                if (Buffer8 == FindCapNo) return EFI_SUCCESS;
                Buffer8 = (UINT8)(*CapPtr16) + 1;
                *CapPtr16 = (UINT16)(READ_PCI8(Bus, Dev, Fun, Buffer8));
                if (*CapPtr16 == 0) break;
                Buffer8 = READ_PCI8(Bus, Dev, Fun, *CapPtr16);
            }
        }
    }
    return EFI_NOT_FOUND;
}
/*
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   FindPciGraphicAdapter
//
// Description: This routine tries to find any PCI graphic adapter on the
//              PCI bus.
//
// Input:       None
//
// Output:      EFI_STATUS
//              EFI_NOT_FOUND - No PCI Graphic Adapter to be found.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS FindPciGraphicAdapter (VOID)
{
    UINT8                   StartBus;
    UINT8                   EndBus;
    UINT8                   Bus;
    UINT8                   Dev;
    UINT8                   Fun;
    UINT8                   EndFun;
    UINT8                   Buffer8;
    UINT32                  Vid32;

    StartBus = READ_PCI8(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, PCIBR_REG_SBUSN);
    EndBus = READ_PCI8(PCIBR_BUS, PCIBR_DEV, PCIBR_FUN, PCIBR_REG_SUBUSN);

    for (Bus = StartBus; Bus <= EndBus; Bus++) {
        for (Dev = 0; Dev <= PCI_MAX_DEVICE; Dev++) {
            Vid32 = READ_PCI32(Bus, Dev, 0, PCI_VID);
            if (Vid32 != 0xffffffff) {
                Buffer8 = READ_PCI8(Bus, Dev, 0, PCI_HDR);
                EndFun = (Buffer8 & 0x80) ? 8 : 1;
                for (Fun = 0; Fun < EndFun; Fun++) {
                    Vid32 = READ_PCI32(Bus, Dev, Fun, PCI_VID);
                    if (Vid32 != 0xffffffff) {
                        Buffer8=READ_PCI8(Bus, Dev, Fun, PCI_BCC);
                        if (Buffer8 == 3) return EFI_SUCCESS;
                    }
                }
            }
        }
    }

    return EFI_NOT_FOUND;
}
*/

#if NB_PCIE_ERROR_LOG_SUPPORT
/**
    Enable the error register of PCI-Express Device.

    @param Address PCIE devices Address

    @retval VOID

**/
VOID NbEnablePciDevErr(
     IN UINT64 Address
)
{
  UINT32 DevBaseAddr = (UINT32)Address;
  UINT8  CapPtr;
  UINT8  Data8;
 
  //
  //Enable SERR# NMI and IOCHK# in port 61.
  //
  Data8 = IoRead8 (R_PCH_NMI_SC);
  Data8 &= 0xF3;
  IoWrite8 (R_PCH_NMI_SC, Data8);
  
  
  if (gSetupData.RtSerrEnable){
      SET_MEM16_S3(gBootScript, DevBaseAddr + PCI_CMD, BIT8);
      DEBUG((DEBUG_INFO, "READ PciCmdData (Enable SERR, BIT8)  = %x \n", READ_MEM16 (Address+0x08)));
  }      
  if (gSetupData.RtPerrEnable){
      SET_MEM16_S3(gBootScript, DevBaseAddr + PCI_CMD, BIT6);
      DEBUG((DEBUG_INFO, "READ PciCmdData (Enable PERR, BIT6)  = %x \n", READ_MEM16 (Address+0x04)));
  }
  
  // Clear Error Status
  WRITE_MEM8_S3(gBootScript, DevBaseAddr + PCI_STS_ERROR, 0xff);//(BIT0 | BIT1 | BIT2 | BIT3));

  CapPtr = NbFindCapPtr(DevBaseAddr, PCI_CAP_ID_PCIEXP);
  if(CapPtr != 0)
  {
     // Clear Device Error Status
     WRITE_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_DSTS, (BIT0 | BIT1 | BIT2));
     // Enable the error bits of Device Control
     if (gSetupData.RtCorrectedEnable)
         SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_DCTL, BIT0);
     if (gSetupData.RtUnCorrectedEnable)
         SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_DCTL, BIT1);
     if (gSetupData.RtFatalErrorEnable)
         SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_DCTL, BIT2);
  }

  // Root?
  //  if ((READ_MEM8(DevBaseAddr + CapPtr + 0x02) & 0xF0) == 0x40)
  // if device is bridge
  if (READ_MEM16(DevBaseAddr + PCI_SCC) == 0x0604)
  {
      WRITE_MEM8_S3(gBootScript, DevBaseAddr + PCI_SECSTATUS_ERROR, 0xff);//(BIT0 | BIT1 | BIT2 | BIT3));
      if(CapPtr != 0){
         if (gSetupData.RtCorrectedEnable)
             SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_RCTL, BIT0);
         if (gSetupData.RtUnCorrectedEnable)
             SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_RCTL, BIT1);
         if (gSetupData.RtFatalErrorEnable)
             SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_RCTL, BIT2);	
      }
  }
}

/**
    Init NB Pcie devices error register

    @param Address PCIE devices Address

    @retval VOID

**/
VOID NbPciExpressDeviceInitialize(
     IN UINT64 Address
)
{
  UINT8   Dev;
  UINT8   Func;
  UINT8   CurrentBus;
  UINT16  Buffer16;
  UINT64  DevAddress;
  UINT8   CapPtr;
  UINT8   Buffer8;

  CapPtr = NbFindCapPtr(Address, PCI_CAP_ID_PCIEXP);
  Buffer8 = READ_MEM8(Address + CapPtr + CAP_DCTL);
  Buffer8 &= 0xF0;
  
  //Buffer8 |= BIT03;
  
  if (gSetupData.RtCorrectedEnable)
      Buffer8 |= BIT00;
  if (gSetupData.RtUnCorrectedEnable)
      Buffer8 |= BIT01;
  if (gSetupData.RtFatalErrorEnable)
      Buffer8 |= BIT02;

  WRITE_MEM8_S3( gBootScript, \
                 Address + CapPtr + CAP_DCTL, \
                 Buffer8 );

  CurrentBus = READ_MEM8((UINT32)Address + PCIEBRN_REG_SBUSN);

  for (Dev = 0; Dev < 32; Dev++)
  {
    for (Func = 0; Func < 8; Func++)
    {
      DevAddress = (UINT64)NB_PCIE_CFG_ADDRESS(CurrentBus, Dev, Func, 0);

      if (READ_MEM16(DevAddress) == 0xFFFF)
          continue;
      
      DEBUG((DEBUG_INFO, "Bus = %x Dev = %x Func = %x\n", CurrentBus, Dev, Func));
      NbEnablePciDevErr(DevAddress);

      Buffer16 = READ_MEM16((UINT32)NB_PCIE_CFG_ADDRESS(CurrentBus, Dev, 0, 0) + PCI_SCC);
      if (Buffer16 == 0x0604)
      {
          DEBUG((DEBUG_INFO, "Bus = %x Dev = %x Func = %x\n", CurrentBus, Dev, Func));
          DevAddress = (UINT64)NB_PCIE_CFG_ADDRESS(CurrentBus, Dev, 0, 0);
          
          if (gSetupData.RtSerrEnable){
              SET_MEM8_S3(gBootScript, DevAddress + PCI_BRIDGE_CNTL, BIT1);
              DEBUG((DEBUG_INFO, "READ BCTRL (Enable SERR, BIT1)  = %x \n", READ_MEM16 (DevAddress+0x3E))); 
          }
          
          if (gSetupData.RtPerrEnable){
              SET_MEM8_S3(gBootScript, DevAddress + PCI_BRIDGE_CNTL, BIT0);
              DEBUG((DEBUG_INFO, "READ BCTRL (Enable SERR, BIT0)  = %x \n", READ_MEM16 (DevAddress+0x3E))); 
          }
          NbPciExpressDeviceInitialize(DevAddress);
      }
    }
  }
}
#endif
/**
    This callback function is called when a PCI I/O Protocol is
    installed.

    @param Event Event of callback
    @param Context Context of callback.

    @retval EFI_SUCCESS
**/

VOID NbInitAfterDeviceInstall (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    EFI_STATUS              Status;
    EFI_HANDLE              Handle;
    EFI_PCI_IO_PROTOCOL     *PciIo;
    UINTN                   BufferSize = 20 * sizeof(EFI_HANDLE);
    UINTN                   PciSeg;
    UINTN                   PciBus;
    UINTN                   PciDev;
    UINTN                   PciFun;

    Status = pBS->LocateHandle( ByRegisterNotify, NULL, gNBInitNotifyReg, \
                                &BufferSize, &Handle );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;
    
    // Locate PciIo protocol installed on Handle
    Status = pBS->HandleProtocol( Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;

    // Get PCI Device Bus/Device/Function Numbers
    Status = PciIo->GetLocation(PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;


    if (NB_PCI_CFG_ADDRESS(PciBus, PciDev, PciFun, 0) == NB_PCIEBRN_BUS_DEV_FUN)
    {

    }

    if (NB_PCI_CFG_ADDRESS(PciBus, PciDev, PciFun, 0) == NB_IGD_BUS_DEV_FUN) {

    }
    

    if (((UINT8)PciBus == PCIEBRN_BUS) && \
        ((UINT8)PciDev == PCIEBRN_DEV) && \
        (((UINT8)PciFun >= PCIEBRN_FUN) || ((UINT8)PciFun <= PCIEBRN_FUN2))) {

    }

    pBS->CloseEvent(Event); 
}

/**
    Notification function on SimpleIn protocol Installation

    @param SystemTable Pointer to the System Table
              

    @retval EFI_SUCCESS
**/

EFI_STATUS
EFIAPI
RegisterDisplayDIMMPopulationErrMsg (
  IN    EFI_SYSTEM_TABLE            *SystemTable
  )
{

  return EFI_SUCCESS;
}

/**
    This function collects all memory information and creates a
    structure use for other DXE drivers.

    @param Event Event of callback
    @param Context Context of callback.

    @retval EFI_SUCCESS
**/

EFI_STATUS InstallNbMemoryInfo (
    IN EFI_SYSTEM_TABLE *SystemTable )
{
  MEMORY_INFO_DATA_HOB    *mMemInfo;
  UINT8                   node;
  UINT8                   Ch;
  UINT8                   Dimm;
  UINT8                   Slot0;
  UINT8                   Slot1;
  UINT8                   Slot2;
  UINT8                   Slot3;
#ifndef OC_SUPPORT
  BOOLEAN                 MemoryTimingValuesInitialized;
#endif  // #ifndef OC_SUPPORT
  EFI_HOB_GUID_TYPE       *GuidHob;
  
  //
  // Get the HOB list and install MemInfo protocol
  //
  GuidHob = GetFirstGuidHob (&gSiMemoryInfoDataGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  mMemInfo = (MEMORY_INFO_DATA_HOB *) GET_GUID_HOB_DATA (GuidHob);

#ifndef OC_SUPPORT
  MemoryTimingValuesInitialized = FALSE;
#endif  // #ifndef OC_SUPPORT

  gMemInfoHobProtocol.MemInfoData.memSize = (UINT32)mMemInfo->TotalPhysicalMemorySize;
  gMemInfoHobProtocol.MemInfoData.ddrFreq = (UINT16)mMemInfo->ConfiguredMemoryClockSpeed;
  gMemInfoHobProtocol.MemInfoData.VddVoltage[0] = (UINT16)mMemInfo->VddVoltage[0];

  gMemInfoHobProtocol.MemInfoData.EccSupport = mMemInfo->EccSupport;
  gMemInfoHobProtocol.MemInfoData.RefClk = mMemInfo->RefClk;
  gMemInfoHobProtocol.MemInfoData.Ratio = mMemInfo->Ratio;
  NbPlatformData.DDR3Type = mMemInfo->MemoryType;
  // removed BClkFrequency because this vaule is in the "MrcData.Inputs"s
//    gBClkFrequency = MemInfoHob->MrcData.Inputs.BClkFrequency;
  MemoryProfile = gMemInfoHobProtocol.MemInfoData.Profile = mMemInfo->MemoryProfile;

  //
  // Getting the tRAS
  // See MRC_TimingConfiguration.c for tRAS algorithm
  //
  for (node = 0; node < NODE_NUM; node++) {
    for (Ch = 0; Ch < CH_NUM; Ch++) {
      for (Dimm = 0; Dimm < DIMM_NUM; Dimm++) {
        gMemInfoHobProtocol.MemInfoData.dimmSize[(node << 2) + (Ch << 1) + Dimm] = (UINT16) mMemInfo->Controller[0].ChannelInfo[Ch].DimmInfo[Dimm].DimmCapacity;
        DEBUG (
          (DEBUG_INFO,
          "Node %d Ch %d Dimm %d Size: %d\n",
          node,
          Ch,
          Dimm,
          gMemInfoHobProtocol.MemInfoData.dimmSize[(node << 2) + (Ch << 1) + Dimm])
          );
        gMemInfoHobProtocol.MemInfoData.DimmStatus[(node << 2) + (Ch << 1) + Dimm] = mMemInfo->Controller[0].ChannelInfo[Ch].DimmInfo[Dimm].Status;
        gMemInfoHobProtocol.MemInfoData.RankInDimm[(node << 2) + (Ch << 1) + Dimm] = mMemInfo->Controller[0].ChannelInfo[Ch].DimmInfo[Dimm].RankInDimm;
        //
        // Updating tCL, tRCD and tRP to MemInfoHobProtocol from MemInfoHob
        //
        if (mMemInfo->Controller[0].ChannelInfo[Ch].DimmInfo[Dimm].Status == DIMM_PRESENT) {
          gMemInfoHobProtocol.MemInfoData.Timing[0].tCL   = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tCL;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tCWL  = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tCWL;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tFAW  = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tFAW;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tRAS  = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tRAS;
//          gMemInfoHobProtocol.MemInfoData.Timing[0].tRC   = MemInfoHob->MrcData.Outputs.Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tRC;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tRCDtRP  = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tRCDtRP;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tREFI = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tREFI;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tRFC  = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tRFC;
//          gMemInfoHobProtocol.MemInfoData.Timing[0].tRP   = MemInfoHob->MrcData.Outputs.Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tRP;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tRRD  = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tRRD;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tRTP  = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tRTP;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tWR   = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tWR;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tWTR  = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tWTR;
          gMemInfoHobProtocol.MemInfoData.Timing[0].tRPab = mMemInfo->Controller[0].ChannelInfo[Ch].Timing[MemoryProfile].tRPab;
          NBMrcTimingData = mMemInfo->Controller[0].ChannelInfo[Ch].Timing;

          NbPlatformData.XmpProfile1 = (NBMrcTimingData[2].tCK != 0) ? TRUE : FALSE;
          NbPlatformData.XmpProfile2 = (NBMrcTimingData[3].tCK != 0) ? TRUE : FALSE;

#if (defined MRC_MEMORY_DOWN_SUPPORT && (MRC_MEMORY_DOWN_SUPPORT == 1))  
          //
          // Input MemoryDown Spd data for SmBiosMemory driver.
          //
          /**
            Get pointer of SPD data read from MRC for SMBIOS driver to report w/o reading SMBus again.
            MRC only provides SMBIOS data and Manufacturing data after first boot is saved to NVRAM.
          **/
          gMemInfoHobProtocol.MemInfoData.DimmsSpdData[(node << 2) + (Ch << 1) + Dimm] = (UINT8*)&mMemInfo->Controller[0].ChannelInfo[Ch].DimmInfo[Dimm].SpdSave;
#endif            
            
//        gMemInfoHobProtocol.MemInfoData.NMode = MemInfoHob->MrcData.Outputs.Controller[0].ChannelInfo[Ch].Timing.NMode;            
          //
          // Since we have only one setup varaiable to keep track of each memory timing, We inititialize the Memory Info HOB with the first occurence of data
          //
#ifndef OC_SUPPORT
          MemoryTimingValuesInitialized = TRUE;
#endif  // #ifndef OC_SUPPORT
        }
      }
    }
  }

#ifndef OC_SUPPORT
{
    EFI_STATUS             Status;
    SETUP_DATA             *SetupData = NULL;
    EFI_GUID               SetupGuid  = SETUP_GUID;
    UINT32                 Attributes = 0;
    UINTN                  VarSize = 0;
    WDT_PROTOCOL           *WdtProtocol = NULL;

    if (MemoryTimingValuesInitialized) {
        Status = GetEfiVariable(L"Setup", &SetupGuid, &Attributes, &VarSize, &SetupData);
        if (!EFI_ERROR(Status)) {
            Status = pBS->LocateProtocol(&gWdtProtocolGuid, NULL, (VOID **)&WdtProtocol);
            if (!EFI_ERROR(Status)) {
                if (WdtProtocol->CheckStatus() == V_PCH_OC_WDT_CTL_STATUS_FAILURE)
                    SetupData->SpdProfileSelected = 0; // Auto 
            }

            SetupData->tCL   = (UINT8)NBMrcTimingData[MemoryProfile].tCL;
            SetupData->tCWL  = (UINT8)NBMrcTimingData[MemoryProfile].tCWL;
            SetupData->tFAW  = NBMrcTimingData[MemoryProfile].tFAW;
            SetupData->tRAS  = NBMrcTimingData[MemoryProfile].tRAS;
            SetupData->tRCDtRP = (UINT8)NBMrcTimingData[MemoryProfile].tRCDtRP;
            SetupData->tREFI = NBMrcTimingData[MemoryProfile].tREFI;
            SetupData->tRFC  = NBMrcTimingData[MemoryProfile].tRFC;
            SetupData->tRRD  = (UINT8)NBMrcTimingData[MemoryProfile].tRRD;
            SetupData->tRTP  = (UINT8)NBMrcTimingData[MemoryProfile].tRTP;
            SetupData->tWR   = (UINT8)NBMrcTimingData[MemoryProfile].tWR;
            SetupData->tWTR  = (UINT8)NBMrcTimingData[MemoryProfile].tWTR;

            Status = pRS->SetVariable (
                              L"Setup",
                              &SetupGuid,
                              Attributes,
                              sizeof(SETUP_DATA),
                              SetupData);
            ASSERT_EFI_ERROR (Status);
        }
    }
}
#endif

    // Mobile - 0; Desktop - 2; UpServer - 3; FlavorWorkStation - 4;
    if (!(IS_SA_DEVICE_ID_MOBILE(READ_PCI16_NB(R_SA_MC_DEVICE_ID)))) { // If platform is not FlavorMobile

      Slot0 = (mMemInfo->Controller[0].ChannelInfo[0].DimmInfo[0].Status == DIMM_PRESENT) ? TRUE : FALSE;
      Slot1 = (mMemInfo->Controller[0].ChannelInfo[0].DimmInfo[1].Status == DIMM_PRESENT) ? TRUE : FALSE;
      Slot2 = (mMemInfo->Controller[0].ChannelInfo[1].DimmInfo[0].Status == DIMM_PRESENT) ? TRUE : FALSE;
      Slot3 = (mMemInfo->Controller[0].ChannelInfo[1].DimmInfo[1].Status == DIMM_PRESENT) ? TRUE : FALSE;

      //
      // Channel 0          Channel 1
      // Slot0   Slot1      Slot0   Slot1      - Population            AIO board
      // 0          0          0          0          - Invalid        - Invalid
      // 0          0          0          1          - Valid          - Invalid
      // 0          0          1          0          - Invalid        - Valid
      // 0          0          1          1          - Valid          - Valid
      // 0          1          0          0          - Valid          - Invalid
      // 0          1          0          1          - Valid          - Invalid
      // 0          1          1          0          - Invalid        - Invalid
      // 0          1          1          1          - Valid          - Invalid
      // 1          0          0          0          - Invalid        - Valid
      // 1          0          0          1          - Invalid        - Invalid
      // 1          0          1          0          - Invalid        - Valid
      // 1          0          1          1          - Invalid        - Valid
      // 1          1          0          0          - Valid          - Valid
      // 1          1          0          1          - Valid          - Invalid
      // 1          1          1          0          - Invalid        - Valid
      // 1          1          1          1          - Valid          - Valid
      //
      if ((Slot0 && (Slot1 == 0)) || (Slot2 && (Slot3 == 0))) {
        RegisterDisplayDIMMPopulationErrMsg (SystemTable);
      }
    }
    
//    Status = pBS->InstallMultipleProtocolInterfaces (
//                &Handle,
//                &gMemInfoProtocolGuid,
//                &gMemInfoHobProtocol,
//                NULL
//                );

    return EFI_SUCCESS;
}

/**
    This callback function is called after Setup NVRAM variable
    being updated.

    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID
**/

VOID NbSetupNvramUpdatedCallback (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context )
{
#if !defined(MDEPKG_NDEBUG)
    EFI_STATUS              Status;
#endif    
    NB_SETUP_DATA           *NBSetupData = NULL;
    UINTN                   VariableSize = sizeof(NB_SETUP_DATA);

#if defined(MDEPKG_NDEBUG)
    pBS->AllocatePool (EfiBootServicesData, \
                                    VariableSize, \
                                    (VOID **)&NBSetupData);
#elif !defined(MDEPKG_NDEBUG)
    Status = pBS->AllocatePool (EfiBootServicesData, \
                                VariableSize, \
                                (VOID **)&NBSetupData);
    ASSERT_EFI_ERROR(Status);
#endif

    GetNbSetupData (pRS, NBSetupData, FALSE);

    // Free memory used for setup data
    pBS->FreePool( NBSetupData );

    pBS->CloseEvent(Event);
}

BOOLEAN NbExecuteUefiRom(
    IN UINT8        BaseClassCode
)
{
    EFI_STATUS Status;
    AMI_OPROM_POLICY_PROTOCOL *AmiOpromPolicyProtocol;

    Status = pBS->LocateProtocol(&gAmiOpromPolicyProtocolGuid, NULL, (VOID**)&AmiOpromPolicyProtocol); 
    if(EFI_ERROR(Status))   //if CSM OptOut is disabled we should always launch UEFI OpROM
        return TRUE;

    Status = AmiOpromPolicyProtocol->CheckUefiOpromPolicy(AmiOpromPolicyProtocol, BaseClassCode);
    return (EFI_ERROR(Status)) ? FALSE : TRUE;
}

#if defined(ENABLE_CLEAR_IGFX_FRAME_BUFFER) && (ENABLE_CLEAR_IGFX_FRAME_BUFFER == 1)
VOID NbClearFrameBufferForIGFX(
    VOID
)
{
    volatile UINT64      *FrameBufferPtr;
    UINT64       i;
    UINT32       HiGTBaseAddress;
    UINT32       LoGTBaseAddress;
    UINT64       FrameBufferAddress;
    EFI_EDID_ACTIVE_PROTOCOL      *EdidActiveInterface;
    EFI_STATUS                    Status;
    UINT8                         *EdidDataPtr = NULL;
    UINT16                        HorizontalActive = 0;
    UINT16                        VerticalActive = 0;
    UINT8                         PixelsData = 0;
    UINTN                         ClearedFrameBufferSize = 0;

    
    Status = pBS->LocateProtocol (&gEfiEdidActiveProtocolGuid, NULL, (VOID**)&EdidActiveInterface);
    DEBUG((DEBUG_INFO, "Locate gEfiEdidActiveProtocolGuid Status = %r.\n", Status));
    if (EFI_ERROR (Status)) {
      return;
    }

    EdidDataPtr = EdidActiveInterface->Edid;
    DEBUG((DEBUG_INFO, "EdidDataPtr = 0x%x.\n", EdidDataPtr));
    DEBUG((DEBUG_INFO, "Edid Size = %d.\n", EdidActiveInterface->SizeOfEdid));
    //
    // This pointer is NULL if no EDID information is available for the video output
    // device. The minimum size of a valid Edid buffer is 128 bytes.
    //
    if ((EdidDataPtr == NULL) || (EdidActiveInterface->SizeOfEdid < MIN_EDID_BUFFER_SIZE)) {
      return;
    }

    //
    // The purpose is to get the horizontal and the verical of the monitor, based on EDID spec, 
    // the horizontal of the monitor is defined at offset 0x38 and the upper 4 bits of offset 0x3A and
    // the vertical of the monitor is defined at offset 0x3B and the upper 4 bits of offset 0x3D.
    //
    PixelsData = *(EdidDataPtr + HORIZONTAL_ACTIVE_OFFSET_3A);
    HorizontalActive = (PixelsData >> 4) << 8;
    HorizontalActive |= *(EdidDataPtr + HORIZONTAL_ACTIVE_OFFSET_38);

    PixelsData = *(EdidDataPtr + HORIZONTAL_ACTIVE_OFFSET_3D);
    VerticalActive = (PixelsData >> 4) << 8;
    VerticalActive |= *(EdidDataPtr + HORIZONTAL_ACTIVE_OFFSET_3B);

    //
    // The total size of frame buffer shall be cleaned. 
    //
    ClearedFrameBufferSize = HorizontalActive * VerticalActive * RGB_32_BIT;

    DEBUG((DEBUG_INFO, "HorizontalActive = %d.\n", HorizontalActive));
    DEBUG((DEBUG_INFO, "VerticalActive = %d.\n", VerticalActive));
    DEBUG((DEBUG_INFO, "ClearedFrameBufferSize = 0x%x.\n", ClearedFrameBufferSize));

    //
    // The garbage of the screen only display on 1080p above monitor so if the resolution of the monitor
    // is less than or equal to 1080p, the frame buffer will not be cleaned.
    //
    if (VerticalActive <= 1080) {
      DEBUG((DEBUG_INFO, "The resolution of the monitor is less than or equal to 1080p so there is no action\n"));
      return;
    }
                     
    if(READ_PCI16_IGD (R_PCI_SCC_OFFSET) == 0x0300) {
      if (!NbExecuteUefiRom(0x03)) {
            
         LoGTBaseAddress = READ_PCI32_IGD (R_SA_IGD_GMADR) & (~0x000000FF);
         HiGTBaseAddress = READ_PCI32_IGD (R_SA_IGD_GMADR + 4);
         FrameBufferAddress = LShiftU64(HiGTBaseAddress, 32);
         FrameBufferAddress |= LoGTBaseAddress;
         if(FrameBufferAddress != 0 && (READ_PCI8_IGD (R_SA_IGD_CMD) != 0)) {
            FrameBufferPtr = (volatile UINT64*)FrameBufferAddress;
            for (i = 0; i < ClearedFrameBufferSize; i += sizeof(UINT64)) {
               *FrameBufferPtr = 0;
               FrameBufferPtr++;
            }
         }
      }
    }
}
#endif

/**
    Install gEfiPciEnumerationCompleteProtocolGuid Protocol.

    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID
**/

VOID NbAmiConOutStartedProtocolCallback (
  IN EFI_EVENT                    Event,
  IN VOID                         *Context)
{

#if defined(ENABLE_CLEAR_IGFX_FRAME_BUFFER) && (ENABLE_CLEAR_IGFX_FRAME_BUFFER == 1)
    NbClearFrameBufferForIGFX ();
#endif

    pBS->CloseEvent(Event);

}

//----------------------------------------------------------------------------
#if (ACPI_SUPPORT)

//ray_override / [XI-BringUp] Bring Up Porting / Modified >>
//UINT8    ACPI_OEM_ID[6]     = ACPI_OEM_ID_MAK;
//UINT8    ACPI_OEM_TBL_ID[8] = ACPI_OEM_TBL_ID_MAK;
UINT8       ACPI_OEM_ID[6]     = T_ACPI_OEM_ID;
UINT8       ACPI_OEM_TBL_ID[8] = T_ACPI_OEM_TBL_ID;
//ray_override / [XI-BringUp] Bring Up Porting / Modified <<

UINTN           mMcfgTblHandle=0;
EFI_EVENT       mAcpiEvent;
VOID            *mAcpiReg;

/**
    This function will create all ACPI components for NB when
    ACPI support protocol is available.

    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID
**/

VOID CreateNbAcpiCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    MCFG_20                     *mcfg;
    EFI_ACPI_TABLE_PROTOCOL     *At;
    UINT8                       i;  
    UINTN                       TableKey;

    // It must be only one instance of such protocol
    Status = pBS->LocateProtocol( &gEfiAcpiTableProtocolGuid, NULL, (VOID **)&At );
    DEBUG((DEBUG_INFO, "PciHostCSHooks: LocateProtocol(ACPITableProtocol) = %r\n", Status));
    if(EFI_ERROR(Status)) return;

//--------------------------------
  //it must be only one instance of such protocol
    mcfg=MallocZ(sizeof(MCFG_20));
    ASSERT(mcfg);
    if(!mcfg) return;

    //Fill Table header;
    mcfg->Header.Signature=MCFG_SIG;
    mcfg->Header.Length=sizeof(MCFG_20);
    mcfg->Header.Revision=1;
    mcfg->Header.Checksum=0;
    for (i=0;i<6;i++) {
      mcfg->Header.OemId[i]=ACPI_OEM_ID[i];
    }

    for (i=0;i<8;i++) {
      mcfg->Header.OemTblId[i]=ACPI_OEM_TBL_ID[i];
    }  
    mcfg->Header.OemRev=ACPI_OEM_REV;
    mcfg->Header.CreatorId=0x5446534d;  // "MSFT" 4D 53 46 54
    mcfg->Header.CreatorRev=0x97;
    
    //fill MCFG Fields  
    mcfg->BaseAddr=PCIEX_BASE_ADDRESS;  // Base address of 256MB extended config space
    mcfg->PciSeg=0;                     // Segment # of PCI Bus
    mcfg->StartBus=0;                   // Start bus number of PCI segment
    mcfg->EndBus=(UINT8)((PCIEX_LENGTH >> 20) - 1);// End bus number of PCI segment

    // Add table 
    Status = At->InstallAcpiTable( At, mcfg, sizeof(MCFG_20), &TableKey );
    DEBUG((DEBUG_INFO, "PciHostCSHooks: ACPITable->InstallAcpiTable(MCFG)=%r\n",Status));
    ASSERT_EFI_ERROR(Status);

    //free memory used for table image
    pBS->FreePool(mcfg);

    //Kill the Event
    pBS->CloseEvent(Event);

}

#endif

/**
    This function creates all ACPI components supported by NB.

    @param VOID

    @retval VOID
**/

VOID CreateNbAcpiComponent (VOID)
{
#if (ACPI_SUPPORT)
    RegisterProtocolCallback(&gEfiAcpiTableProtocolGuid, \
                                      CreateNbAcpiCallback, \
                                      NULL, \
                                      &mAcpiEvent, \
                                      &mAcpiReg );
    // If System Description Table Protocol has been installed we can use
    // it rigth on the way
    pBS->SignalEvent(mAcpiEvent);
#endif

}

/**
    For setting PAM0\PAM5\PAM6 to "Disable", "Read Only", "Write Only", or "R/W Enable"
    under not using legacy.

    @param EFI_HANDLE             
        EFI_SYSTEM_TABL  

    @retval VOID
**/

EFI_STATUS NbDxeShadowRam (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
) 
{
    EFI_STATUS         Status = EFI_SUCCESS;
    UINT8              Default_PAM0 = (UINT8)(NB_F0000_PAM0 << 4);
    UINT8              Default_PAM5 = (UINT8)((NB_E0000_PAM5 << 4) | NB_E0000_PAM5);
    UINT8              Default_PAM6 = (UINT8)((NB_E8000_PAM6 << 4) | NB_E8000_PAM6);

    //
    // Set "Disable", "Read Only", "Write Only", or "R/W Enable" as Token value into PAM0\PAM5\PAM6.
    //
    WRITE_PCI8_NB (0x80, Default_PAM0);      
    WRITE_PCI8_NB (0x85, Default_PAM5); 
    WRITE_PCI8_NB (0x86, Default_PAM6); 

    return Status;
}

/**
    This function collects information and creates a structure 
    use for other DXE drivers.

    @param Event Event of callback
    @param Context Context of callback.

    @retval EFI_SUCCESS
**/

EFI_STATUS CreateNbPlatformInfo (
    IN NB_SETUP_DATA    *NbSetupData)
{
    EFI_STATUS        Status;
    UINT16            McDeviceId;
    UINT16            LpcDeviceId;
    UINT32            RegEax, RegEbx, RegEcx, RegEdx;
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    SETUP_DATA        SetupData;
    UINTN             Size;
    UINT8             SecData = 0;
#endif   

    //
    // Initialize NbPlatformData variable
    //
    NbPlatformData.PegAvailable = (GetPchSeries() == PchLp) ? FALSE : TRUE;
    NbPlatformData.IGFXCapability = (READ_PCI32_NB (R_SA_MC_CAPID0_A_OFFSET) & BIT11) ? FALSE : TRUE;     
    NbPlatformData.IGFXAvailable = (READ_PCI32_IGD (R_SA_IGD_VID) != 0xFFFFFFFF) ? TRUE : FALSE;    
    NbPlatformData.VTdAvailable = (READ_PCI32_NB(0xe4) & BIT23) ? 0 : 1;
    // Read MC device ID
    McDeviceId = READ_PCI16_NB(R_SA_MC_DEVICE_ID);
    // Read PCH device ID
    LpcDeviceId = READ_PCI16_SB(PCI_DEVICE_ID_OFFSET);
    // Mobile - 0; Desktop - 2; UpServer - 3; FlavorWorkStation - 4;
    if(IS_SA_DEVICE_ID_MOBILE(McDeviceId)) {
        NbPlatformData.UserBoard = FlavorMobile;
    }
    else if(IS_SA_DEVICE_ID_DESKTOP (McDeviceId) | IS_SA_DEVICE_ID_SERVER(McDeviceId)) {
        if(IS_PCH_LPC_DEVICE_ID_SERVER (LpcDeviceId)) {
            NbPlatformData.UserBoard = FlavorUpServer;
        } else {
            NbPlatformData.UserBoard = FlavorDesktop;
        }
    }
    else {
         NbPlatformData.UserBoard = FlavorDesktop;
    }
    
    if (NbPlatformData.IGFXAvailable == TRUE) {
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
      Size = sizeof(UINT8);
      Status = pRS->GetVariable (L"SecureBoot", &gEfiGlobalVariableGuid, NULL, &Size, &SecData);
    
      if (!EFI_ERROR(Status) && (SecData != 0)) {
        // No CSM in secure boot mode
        NbPlatformData.IGFXGopAvailable = 1;
      }
      else {
        Size = sizeof(SETUP_DATA);
        Status = pRS->GetVariable (L"Setup", &gSetupGuid, NULL, &Size, &SetupData);
        if (!EFI_ERROR(Status)) {
          if ((SetupData.CsmSupport == 0) || (SetupData.VideoOpRom == 1))
            NbPlatformData.IGFXGopAvailable = 1;
        }
      }
#else
      NbPlatformData.IGFXGopAvailable = 1;
#endif // CSM_SUPPORT
    }

    //
    // Set CpuFamilyModel, CpuExtendedFamilyModel and CpuStepping
    // 
    CPULib_CpuID(CPUID_VERSION_INFO, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    NbPlatformData.CpuFamilyModel = (UINT16) RegEax & 0x3FF0; // CPU_FM_SKL
    NbPlatformData.CpuExtendedFamilyModel = (UINT16) ((RegEax >> 16) & 0x0FFF);
    NbPlatformData.CpuStepping = (UINT8) (RegEax & CPUID_FULL_STEPPING);

    Status = pRS->SetVariable (
                L"NBPlatformData",
                &gSetupGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof (NB_PLATFORM_DATA),
                &NbPlatformData
                );
    ASSERT_EFI_ERROR (Status);
    
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
