//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
/** @file SbDxe.c
    This file contains code for Template Southbridge initialization
    in the DXE stage

**/

// Module specific Includes
#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Dxe.h>
#include <AmiCspLib.h>
#include <AmiHobs.h>
#include <Library/AmiHpetApicLib.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/PlatformDriverOverride.h>        // [ EIP405844 ]
#include <Protocol/LoadedImage.h>
#include <Protocol/SBPlatformData.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ConfigBlockLib.h>
#include <Library/PchGbeLib.h>
#include <Library/AmiSbMiscLib.h>
#include <SbSetupData.h>
#include <Guid/Acpi.h>
#include <Protocol/AcpiSystemDescriptionTable.h>     // [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.

// Consumed Protocols
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#if ACPI_SUPPORT
#include <Protocol/AcpiSystemDescriptionTable.h>
#endif
#include <Protocol/AcpiTable.h>
#include <Acpi50.h>
#include <AmiCspGlobalNvsAreaProtocol.h>

// Produced Protocols
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/RealTimeClock.h>
#include <Protocol/WatchdogTimer.h>
#include <Protocol/Legacy8259.h>
#include <Protocol/AmiCsmOpromPolicy.h>
#include <SbDxeInitElink.h>
#include <PchAccess.h>

#if SecureMod_SUPPORT
#include <Flash.h>
#endif
#if defined (SPI_INITIALIZE_WITH_VSCC) && (SPI_INITIALIZE_WITH_VSCC == 1)
#include <Protocol/FlashProtocol.h>
#endif

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#include <Protocol/AmiUsbController.h>
#include <Protocol/UsbPolicy.h>
#endif

#include <Protocol/GlobalNvsArea.h>
#include <Private/Library/PchHdaLib.h>
#include <Private/Protocol/PchNvs.h>
#include <Private/SiConfigHob.h>
#include <PchPolicyCommon.h>

#if RaidDriver_SUPPORT
#include <IndustryStandard/AmiAtaAtapi.h>
#include <Protocol/AmiAhciBus.h>
//#include "protocol/BlockIo.h"
//#include "Protocol/PDiskInfo.h"
//#include "Protocol/PIDEController.h"
//#include "Protocol/PIDEBus.h"
//#include "Protocol/PAhciBus.h"
#endif
#if (defined(SMBIOS_SUPPORT) && (SMBIOS_SUPPORT == 1))
#include <Protocol/AmiSmbios.h>
#endif
#include <Library/PchInfoLib.h>
#include <Library/PchPcieRpLib.h>
#if HOTPLUG_SUPPORT
#include <Library/AmiPciHotPlugLib.h>
#else
#include <Pci.h>
#endif
//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Build flag adjustments
#define PCH_DEVICE_ENABLE  1
#define PCH_DEVICE_DISABLE 0

#ifndef SMM_SUPPORT
#define SMM_SUPPORT 0
#endif

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
EFI_GUID gAmiLSIPathGuid = {0xC206DDF8, 0xB4D6, 0x4A5D, 0xB1, 0xE3, 0x74, 0x56, 0xEF, 0xB0, 0xB1, 0xF8};
#endif

// Timer Constants
#define SYSTEM_TIMER_IRQ 0

// Timer Period
#define TIMER_TICK 838 // ns

//8259 PIC defines
#define ICW1            0x11    // Slave exists and ICW4 required.
#define ICW3_M          1 << 2  // IRQ 2 connects to slave
#define ICW3_S          2       // IRQ 2 connects to master
#define ICW4            1       // Bit 4 Normal Nested Mode
                                // Bit 3 Non-buffered Mode
                                // Bit 2 Unused with non-buffered mode
                                // Bit 1 Set manual EOI instead of automatic
                                // Bit 0 8086/8088 mode

#define OCW1_M          0xff    // Master Mask
#define OCW1_S          0xff    // Slave Mask

#define EOI_COMMAND     0x20    // EOI Command

#define FLASH_DEVICE_BASE_ADDRESS (0xffffffff - FLASH_SIZE + 1)

#define PCI_CLASS_NETWORK             0x02
#define PCI_CLASS_NETWORK_ETHERNET    0x00
#define PCI_CLASS_NETWORK_OTHER       0x80

#define SPI_OPCODE_READ_ID_INDEX      0x4

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

EFI_STATUS WatchdogInit (
    IN EFI_HANDLE        ImageHandle, 
    IN EFI_SYSTEM_TABLE  *SystemTable
);

EFI_STATUS Initialize8259(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
);

EFI_STATUS
SBDXE_BoardInit (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
);

VOID EFIAPI SbInitAfterDeviceInstall (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

VOID EFIAPI WatchdogHandler (
    IN EFI_EVENT        Event, 
    IN VOID             *Context
);

EFI_STATUS EFIAPI RegisterHandler (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN EFI_WATCHDOG_TIMER_NOTIFY         NotifyFunction
);

EFI_STATUS EFIAPI WatchdogSetTimerPeriod (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN UINT64                            TimerPeriod
);

EFI_STATUS EFIAPI WatchdogGetTimerPeriod (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN UINT64                            *TimerPeriod
);

#if defined HPET_PROTOCOL_SUPPORT && HPET_PROTOCOL_SUPPORT == 1
VOID TimerInit (VOID);
#endif

VOID CreateSbAcpiComponent (VOID);

//EFI_STATUS ReserveSbResources (
//    IN EFI_HANDLE      ImgHandle,
//    IN EFI_HANDLE      CntrHandle
//);

VOID
EFIAPI
SbInitEndOfDxe (
    IN EFI_EVENT    Event,
    IN VOID         *Context
);

VOID 
EFIAPI
InitSbRegsBeforeBoot (
    IN EFI_EVENT    Event,
    IN VOID         *Context
);

VOID 
EFIAPI
InitSbRegsBeforeLagecyBoot (
    IN EFI_EVENT    Event,
    IN VOID         *Context
);

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
VOID USBPerPortDisableCallback (
    IN EFI_EVENT    Event,
    IN VOID *Context
);
#endif

VOID ConfigPciDevices (
    IN EFI_SYSTEM_TABLE     *SystemTable
);

VOID ReportSBDxeError (
    IN EFI_STATUS       Status
);

VOID 
EFIAPI
SbCheckOprom (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

VOID 
EFIAPI
SbSetupNvramUpdatedCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

VOID SBSpiProgramVSCC(VOID);

VOID SBClearRTC_AlarmFlag(VOID);

#if ACPI_SUPPORT
//EFI_STATUS UpdateDsdtAcpiTable (VOID);            // [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
EFI_STATUS EFIAPI UpdateDsdtAcpiTable(
    IN EFI_ACPI_SDT_HEADER    *Table,
    IN EFI_ACPI_TABLE_VERSION Version,
    IN UINTN                  TableKey
);

EFI_STATUS LocatePublishIdeSataAcpiTables(VOID);

VOID 
EFIAPI
UpdateRcNhltAcpiData (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

VOID TradUltDsdtTableUpdate (
//  IN ACPI_HDR   *DsdtPtr,                         // [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
  IN EFI_ACPI_SDT_HEADER   *DsdtPtr,
  IN PCH_SERIES            PchSeries
);
#endif

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

typedef struct {
    UINT8 Bus;
    UINT8 Dev;
    UINT8 Fun;
    VOID *Process;
} DEVICES_AFTER_PCIIO;

DEVICES_AFTER_PCIIO gPchLpDevicesTable[] = {
#if defined(SB_I2C_HID_SUPPORT) && (SB_I2C_HID_SUPPORT == 1)        
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C0, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C0,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C1, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C1,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C2, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C2,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C3, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C3,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C4, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C4,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C5, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C5,  NULL },                
#endif      
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9,  NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10, NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11, NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12, NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_HDA,           PCI_FUNCTION_NUMBER_PCH_HDA,               NULL },       //(EIP101204)
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SMBUS,         PCI_FUNCTION_NUMBER_PCH_SMBUS,             NULL },
    { DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA,          PCI_FUNCTION_NUMBER_PCH_SATA,              NULL },
    { 0xFF,         0xFF,         0xFF,         NULL }
};

UINTN gEventCount = sizeof(gPchLpDevicesTable) / sizeof(DEVICES_AFTER_PCIIO);

#if SB_PCIE_ERROR_LOG_SUPPORT
DEVICES_AFTER_PCIIO gDevicesTable[] = {
    { 0, 0x1B,  0, NULL },
    { 0, 0x1B,  1, NULL },
    { 0, 0x1B,  2, NULL },
    { 0, 0x1C,  0, NULL },
    { 0, 0x1C,  1, NULL },
    { 0, 0x1C,  2, NULL },
    { 0, 0x1C,  3, NULL },
    { 0, 0x1C,  4, NULL },
    { 0, 0x1C,  5, NULL },
    { 0, 0x1C,  6, NULL },
    { 0, 0x1C,  7, NULL },
    { 0, 0x1D,  0, NULL },
    { 0, 0x1D,  1, NULL },
    { 0, 0x1D,  2, NULL },
    { 0, 0x1D,  3, NULL },
    { 0, 0x1D,  4, NULL },
    { 0, 0x1D,  5, NULL },
    { 0, 0x1D,  6, NULL },
    { 0, 0x1D,  7, NULL }
};

UINTN gDevicesTableCount = sizeof(gDevicesTable) / sizeof(DEVICES_AFTER_PCIIO);

BOOLEAN gErrorLoggingFlag = FALSE;
#endif

typedef struct {
    UINT32                      VidDid;
    UINT32                      Vscc;
} SPI_VSCC_TABLE;

#if (defined(SMBIOS_SUPPORT) && (SMBIOS_SUPPORT == 1))
#pragma pack (1)
typedef struct {
  SMBIOS_STRUCTURE_HEADER        Header;
  UINT16                         OemInfo;
  UINT16                         Zero;   //terminator
} EFI_MISC_OEM_TYPE_0x88;
#pragma pack ()
#endif

EFI_HANDLE  mWatchdogHandle         = NULL;
EFI_EVENT   mWatchdogEvent;

UINT8       gMasterBase;
UINT8       gSlaveBase;

// Initially in protected mode. (0 = Real, 1 = 32 bit protected) 
UINT8       gMode                   = 1;

// Initially all Real IRQs masked, protected masked 
UINT16      gIrqMask[2]             = {0xffff, 0xffff}; 

// Initially all Real IRQs Edge, protected Edge. 
UINT16      gIrqTrigger[2]          = {0, 0};
UINT64      mWatchdogPeriod         = 0;

EFI_WATCHDOG_TIMER_NOTIFY       mWatchdogNotifyFunction = NULL;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo = NULL;
EFI_S3_SAVE_STATE_PROTOCOL      *gBootScript = NULL;
VOID                            *gSbInitNotifyReg = NULL;
VOID                            *gCsmOpromReg = NULL;
VOID                            *gSbSetupNvramUpdatedReg = NULL;
VOID                            *gSbSataIdeProtocolReg = NULL;  
SB_SETUP_DATA                   *gSbSetupData = NULL;
BOOLEAN                         gDisableAllUsbControllers = FALSE;
PCH_NVS_AREA_PROTOCOL           *gPchNvsAreaProtocol = NULL;
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
EFI_EVENT                       gEvtUsbProtocol  = NULL;
VOID                            *gRegUsbProtocol = NULL;
EFI_USB_PROTOCOL                *gUsbProtocol = NULL;

#define ACPI_PATH_MACRO \
 {{ACPI_DEVICE_PATH,ACPI_DP,ACPI_DEVICE_PATH_LENGTH}, EISA_PNP_ID(0x0A03),0}

#define PCI_PATH_MACRO(Device,Function) \
 {{HARDWARE_DEVICE_PATH, HW_PCI_DP, HW_PCI_DEVICE_PATH_LENGTH}, (Function),(Device)}

#define END_PATH_MACRO \
 {END_DEVICE_PATH,END_ENTIRE_SUBTYPE,END_DEVICE_PATH_LENGTH}

typedef struct
{
 ACPI_HID_DEVICE_PATH      Acpi;
 PCI_DEVICE_PATH           Pci;
 EFI_DEVICE_PATH_PROTOCOL  End;
} SMM_USB_DEVICE_PATH;

SMM_USB_DEVICE_PATH gXhciDevicePath[] = 
{
    {
        ACPI_PATH_MACRO,
        PCI_PATH_MACRO(0x14, 0),
        END_PATH_MACRO
    }
};

AMI_USB_HW_SMI_HC_CONTEXT gXhciHwSmiContext[] =
{
    USB_HC_XHCI,
    (EFI_DEVICE_PATH_PROTOCOL*)gXhciDevicePath
};
#endif
#if SB_PCIE_ERROR_LOG_SUPPORT
static SETUP_DATA                   gSetupData = {0};
#endif

#if (ACPI_SUPPORT)
#define IDE_SATA_ACPI_TABLE_STORAGE_GUID \
  { 0x22046D50,0xF390,0x498c,0x92,0xE5,0x5B,0xA4,0xF8,0xE7,0xF8,0xB6}
EFI_GUID IdeSataAcpiTableStorageGuid = IDE_SATA_ACPI_TABLE_STORAGE_GUID;
#endif

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
VOID                            *gLSINotifyReg = NULL;
EFI_EVENT                       LSIDeviceEvent;
UINT32                          *PciPcieAddr = NULL;
UINT32                          LSIDeviceLength = 0;
#define LSI_VENDOR_ID  0x1000
#endif

// AMI_OVERRIDE_NB0000_START >>>
CSP_RES_ITEM gSbResTable[] = {
#if 0 
//----------------------------------------------------------------------------
// ResBase      ResLength       ResType                         Attributes  
//----------------------------------------------------------------------------
// Always reserve 16MB for ROM image.       
{ RESERVED_ROM_BASE,   RESERVED_ROM_LENGTH,         EfiGcdMemoryTypeMemoryMappedIo ,\
                                          (EFI_MEMORY_UC | EFI_MEMORY_RUNTIME)},
// IOAPICs dedicated Space
{ APCB,         APCL,              EfiGcdMemoryTypeMemoryMappedIo , \
                                          (EFI_MEMORY_UC|EFI_MEMORY_RUNTIME)},
#if FEC00000_APIC_AUTODETECT
{ APCB + APCL,  0x00100000 - APCL, EfiGcdMemoryTypeMemoryMappedIo , \
                                           EFI_MEMORY_UC},
#endif

#if HPET_SUPPORT
// HPET dedicated Space
{ HPET_BASE_ADDRESS, 0x00004000,   EfiGcdMemoryTypeMemoryMappedIo , \
                                          (EFI_MEMORY_UC|EFI_MEMORY_RUNTIME)},
#endif
{ PCR_BASE_ADDRESS,  PCR_MMIO_SIZE, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
{ PWRM_BASE_ADDRESS, PWRM_MMIO_SIZE, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
{ SPI_BASE, 0x00001000, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC|EFI_MEMORY_RUNTIME) },
{ 0xFE020000, 0x00016000, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
{ 0xFE1C0000, 0x00040000, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
{ 0xFE200000, 0x00040000, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
// SB Temp Memory Mapped Space
{ SB_TEMP_MMIO_BASE, SB_TEMP_MMIO_BASE_LENGTH, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
// HECI Memory Mapped Space
{ HECI_BASE_ADDRESS, 0x10000,  EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
// HECI2 Memory Mapped Space
{ HECI2_BASE_ADDRESS, 0x10000, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
// HECI5 Memory Mapped Space
{ HECI5_BASE_ADDRESS, 0x10000, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC) },
// IO Used by PM register block
{ PM_BASE_ADDRESS,    PMLN         ,EfiGcdIoTypeIo      ,       -1  },
{ TCO_BASE_ADDRESS, 0x80, EfiGcdIoTypeIo, -1},
// IO Used by SMBus register block
{ SMBUS_BASE_ADDRESS, SMBL         ,EfiGcdIoTypeIo      ,       -1  },
// IO Used by GPIO register block
{ GPIO_BASE_ADDRESS,  GPLN         ,EfiGcdIoTypeIo      ,       -1  },
#else
  0
#endif
};

UINTN   gSbResTableCount = sizeof(gSbResTable) / sizeof(CSP_RES_ITEM);
// AMI_OVERRIDE_NB0000_END <<<

#define ONBOARD_RAID_GUID \
    { 0x5d206dd3, 0x516a, 0x47dc, 0xa1, 0xbc, 0x6d, 0xa2, 0x4, 0xaa, 0xbe, 0x8 };

#if RaidDriver_SUPPORT
#define PCH_EFI_RAID_DRIVER_EXECUTION_GUID \
    { 0x99D5757C, 0xD906, 0x11E0, 0x8D, 0x78, 0x8D, 0xE4, 0x48, 0x24, 0x01, 0x9B };
#endif

// GUID Definition(s)
EFI_GUID gOnboardRaidGuid                  = ONBOARD_RAID_GUID;
EFI_GUID gWatchdogGuid                     = EFI_WATCHDOG_TIMER_ARCH_PROTOCOL_GUID;
EFI_GUID gAmiBeforeBootProtocolGuid        = AMITSE_EVENT_BEFORE_BOOT_GUID;
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
EFI_GUID gEfiUsbProtocolGuid               = EFI_USB_PROTOCOL_GUID;
EFI_GUID gEfiUsbPolicyProtocolGuid         = EFI_USB_POLICY_PROTOCOL_GUID;
#endif

EFI_GUID   GuidListCheckForRaid[] = { BUS_OVERRIDE_GUIDS_FOR_RAID {0} };
EFI_GUID   gEfiBusSpecificDriverOverrideProtocolGuid = EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL_GUID;

EFI_HANDLE ImageHandleArray[5] = {NULL};
BOOLEAN    LoadedImageDone = FALSE;

// [ EIP405844 ] +>>>
//EFI_STATUS RaidGetDriver(
//	IN EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL *This,
//	IN OUT EFI_HANDLE *DriverImageHandle
//);
//
//static EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL RaidBusSpecificDriverOverride = {
//    RaidGetDriver
//};

EFI_STATUS
EFIAPI
RaidGetDriver(
    IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
    IN EFI_HANDLE                            ControllerHandle,
    IN OUT EFI_HANDLE                        *DriverImageHandle
);

EFI_STATUS
EFIAPI
RaidGetDriverPath(
    IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
    IN EFI_HANDLE                            ControllerHandle,
    IN OUT EFI_DEVICE_PATH_PROTOCOL          **DriverImagePath
);

EFI_STATUS
EFIAPI
RaidDriverLoaded(
    IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
    IN EFI_HANDLE                            ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL              *DriverImagePath,
    IN EFI_HANDLE                            DriverImageHandle
);

static EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL RaidPlatformDriverOverride = {
    RaidGetDriver,
    RaidGetDriverPath,
    RaidDriverLoaded
};
// [ EIP405844 ] +<<<

#if RaidDriver_SUPPORT
static AHCI_PLATFORM_POLICY_PROTOCOL AhciPlatformPolicy= {
    FALSE,
    TRUE
};
#endif

// Protocol Definition(s)

// Architectural Protocol Definitions
EFI_WATCHDOG_TIMER_ARCH_PROTOCOL mWatchdog = {
    RegisterHandler,
    WatchdogSetTimerPeriod,
    WatchdogGetTimerPeriod
};

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
VOID 
EFIAPI
SbUsbProtocolCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);
#endif

VOID 
EFIAPI
UpdateGlobalNvsAreaCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
VOID GetStorageLSIAllDevicePath (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);
#endif

// External Declaration(s)

extern EFI_STATUS CountTime (
    IN UINTN            DelayTime, 
    IN UINT16           BaseAddr
);

VOID ClearWarmResetFlag (VOID);

//---------------------------------------------------------------------------
#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
/**
    StoreLSI All Device Path event when system is in ready-to-boot phase.

    @param [in]  EFI_EVENT
                      [in]  VOID
    @retval None.

**/
VOID
EFIAPI
StoreLSIDevicePath (
    IN EFI_EVENT    Event,
    IN VOID         *Context
)
{
	EFI_STATUS               Status;
	UINT32                 	 Attributes;
	UINT32                 	 PathData = 0;
	UINTN                  	 DataSize = sizeof(UINT32);
	UINTN                  	 AddrTotalSize;
	UINT32                   *AddrPointer = NULL;
	UINT32 i;
	if(*(PciPcieAddr) != 0x0){
		AddrTotalSize = ((LSIDeviceLength + 1) * sizeof(UINT32));
		Status = pBS->AllocatePool (EfiReservedMemoryType, AddrTotalSize, (VOID**)&AddrPointer);
		ASSERT_EFI_ERROR(Status);

		MemSet(AddrPointer, AddrTotalSize, 0);
		MemCpy(AddrPointer, PciPcieAddr, AddrTotalSize);

		*(AddrPointer + LSIDeviceLength) = 0xFF; // Use to judge if this is final information.
		
		PathData = (UINT32) AddrPointer;
		Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
		Status = pRS->SetVariable(L"LSIDevice", &gAmiLSIPathGuid, Attributes, DataSize, &PathData);
		ASSERT_EFI_ERROR(Status);
        	for(i = 0x0; i < LSIDeviceLength ; i++){
        		DEBUG((-1, "AddrPointer[%x] : %x\n",i, *(AddrPointer + i)));
        	}
	}
	// Free memory used
	pBS->FreePool(PciPcieAddr);

	// Kill the Event
	pBS->CloseEvent(LSIDeviceEvent);
	pBS->CloseEvent(Event);
}
#endif

/**
    Retrain PCIE Device Link

    @param Address 

    @retval 

**/
VOID RetrainLink(IN UINT32 Address) 
{
   SET_MEM8(Address + R_PCH_PCIE_LCTL, BIT05); // 0x50
   CountTime(10, PM_BASE_ADDRESS);
   while (READ_MEM16(Address + R_PCH_PCIE_LSTS) & BIT11); // 0x52
}

#if RaidDriver_SUPPORT
/**
    Install SATA UEFI RAID driver GUID for PCH SataDriver. 

    @param VOID

    @retval VOID
**/
VOID InstallPchSataUefiDriver ( VOID )
{
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
  SETUP_DATA  SetupData;
  EFI_GUID    SetupGuid = SETUP_GUID;
  UINTN       Size;
  UINT8       SecData = 0;
#endif // CSM_SUPPORT
  BOOLEAN     InstallRaidDriver = TRUE;
  EFI_HANDLE  Handle = NULL;
  EFI_STATUS  Status;
  EFI_HANDLE  RaidDriverHandle=NULL;
  EFI_GUID    PchEfiRaidDriverExecutionGuid = PCH_EFI_RAID_DRIVER_EXECUTION_GUID;

  if (gSbSetupData->SataInterfaceMode == SATA_MODE_RAID) {
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    Size = sizeof(UINT8);
    Status = pRS->GetVariable (L"SecureBoot", &gEfiGlobalVariableGuid, NULL, &Size, &SecData);
    
    if (!EFI_ERROR(Status) && (SecData != 0)) {
      // No CSM in secure boot mode
    }
    else {
      Size = sizeof(SETUP_DATA);
      Status = pRS->GetVariable (L"Setup", &SetupGuid, NULL, &Size, &SetupData);
      ASSERT_EFI_ERROR(Status);
      
      // If CsmSupport is 0, InstallRaidDriver is always TRUE.
      if ((SetupData.CsmSupport == 1) && (SetupData.SataLegacyOrom == 1)) {
        InstallRaidDriver = FALSE;
      }
    }
#endif // CSM_SUPPORT

    //
    // By default Legacy Raid option settings initilized
    //
    AhciPlatformPolicy.RaidDriverMode = FALSE;
    AhciPlatformPolicy.AhciBusAtapiSupport = TRUE;

    DEBUG((DEBUG_INFO, "SB: InstallRaidDriver = %x\n", InstallRaidDriver));    
    if (InstallRaidDriver) {
      Status = pBS->InstallProtocolInterface( \
                                   &Handle, \
                                   &PchEfiRaidDriverExecutionGuid, \
                                   EFI_NATIVE_INTERFACE, \
                                   NULL);
      ASSERT_EFI_ERROR(Status);

      //
      // UEFI Raid driver enabled
      //
      AhciPlatformPolicy.RaidDriverMode = TRUE;
      //
      // Uefi Raid driver supports the ATAPI device. So Ahcibus doesn't need to handle the ATAPI devices
      //
      AhciPlatformPolicy.AhciBusAtapiSupport = FALSE;
    }

    Status = pBS->InstallProtocolInterface( \
		    &RaidDriverHandle, \
		    &gAmiAhciPlatformPolicyProtocolGuid, \
		    EFI_NATIVE_INTERFACE, \
		    &AhciPlatformPolicy \
		    );

    // [ EIP405844 ] +>>>
    //
    // Install the  platform override protocol on the Raid Controller Handle
    //
    //
    RaidDriverHandle = NULL;
    Status = pBS->InstallMultipleProtocolInterfaces(
                    &RaidDriverHandle,
                    &gEfiPlatformDriverOverrideProtocolGuid,
                    &RaidPlatformDriverOverride,
                    NULL
                    );
    // [ EIP405844 ] +<<<
  }
}
#endif

#if FORCE_USER_TO_SETUP_IF_CMOS_BAD                                  // [EIP88358] >>
/**
    BIOS force to enter setup if CMOS was bad.

    @param VOID

    @retval VOID
**/
VOID ForceSetupIfCmosBad (VOID)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    CMOS_BAD_HOB            *CmosBadHob;
    UINT32                  BootFlow = BOOT_FLOW_CONDITION_FIRST_BOOT;
    EFI_GUID                guidBootFlow = BOOT_FLOW_VARIABLE_GUID;
    EFI_GUID                ghoblistguid = HOB_LIST_GUID;   
    EFI_GUID                CmosBadHobGuid = CMOS_BAD_HOB_GUID;
 
    CmosBadHob = GetEfiConfigurationTable(    //get hob list
                    pST,
                    &ghoblistguid);  
 
    if(CmosBadHob == NULL) return;

    Status = FindNextHobByGuid(    //the hob exit when cmos is bad and creat hob success. 
                &CmosBadHobGuid,            
                &CmosBadHob);
                     
    if(!EFI_ERROR(Status)) {
        DEBUG((DEBUG_INFO, "SB: Force to Setup.\n"));
        pRS->SetVariable(
                L"BootFlow",
                &guidBootFlow,
                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof(BootFlow),
                &BootFlow);
    }
}
#endif                                                               // [EIP88358] <<

/**
    Create SetupPlatformData Information.

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS CreateSbPlatformInfo (VOID)
{
    EFI_STATUS              Status;    
    EFI_GUID                SetupGuid = SETUP_GUID;    
    SETUP_PLATFORM_DATA     SetupPlatformData = {0};
    PCH_SERIES              PchSeries = GetPchSeries();
    
    SetupPlatformData.GbePciePortNum = (UINT8) PchGetGbePortNumber ();
    if (SetupPlatformData.GbePciePortNum == 0) {
        SetupPlatformData.GbePciePortNum = 0xFF;
    }
    
    if (PchSeries == PchLp) SetupPlatformData.LPTType = 1;
       
    SetupPlatformData.PlatformFlavor = PlatformInfo_PlatformFlavor;
    SetupPlatformData.PchRid = READ_PCI8_SB(PCI_REVISION_ID_OFFSET);

    Status = pRS->SetVariable (L"SetupPlatformData",
                               &SetupGuid,
                               EFI_VARIABLE_BOOTSERVICE_ACCESS,
                               sizeof (SETUP_PLATFORM_DATA),
                               &SetupPlatformData);
    ASSERT_EFI_ERROR (Status);
    
    return Status;
}

/**
    This function can initialize any SB registers before Exit
    Boot Services.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/
VOID
EFIAPI
SbInExitBootServices (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
#if defined TRACE_HUB_STATUS_CODE_SUPPORT && TRACE_HUB_STATUS_CODE_SUPPORT == 1  
    WriteCmos(AMI_TRACE_HUB_ENABLE_REG, 0);
#endif

    UpdateRcNhltAcpiData (NULL, NULL);
    
    pBS->CloseEvent (Event);
}

VOID InitVariable (VOID)
{
    EFI_STATUS                  Status;
    EFI_GUID                    SbWarmResetGuid = SB_WARM_RESET_GUID;
    CHAR16                      SbWarmResetVar[] = SB_WARM_RESET_VARIABLE;
    UINT32                      SbWarmResetFlag = 0;
    UINTN                       VarSize = sizeof(SbWarmResetFlag);
    static EFI_GUID             guidHob = HOB_LIST_GUID;
    EFI_HOB_HANDOFF_INFO_TABLE  *pHit;

    Status = pRS->GetVariable( SbWarmResetVar, \
                               &SbWarmResetGuid, \
                               NULL, \
                               &VarSize, \
                               &SbWarmResetFlag );
    if (EFI_ERROR(Status)) {
        SbWarmResetFlag = 0;
        Status = pRS->SetVariable( SbWarmResetVar, \
                                   &SbWarmResetGuid, \
                                   EFI_VARIABLE_NON_VOLATILE | \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                                   EFI_VARIABLE_RUNTIME_ACCESS, \
                                   VarSize, \
                                   &SbWarmResetFlag );
        
    } else if (SbWarmResetFlag == SB_WARM_RESET_TAG) {
        SbWarmResetFlag = 0;
        Status = pRS->SetVariable( SbWarmResetVar, \
                                   &SbWarmResetGuid, \
                                   EFI_VARIABLE_NON_VOLATILE | \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                                   EFI_VARIABLE_RUNTIME_ACCESS, \
                                   VarSize, \
                                   &SbWarmResetFlag );
        
        //Get Boot Mode
        pHit = GetEfiConfigurationTable(pST, &guidHob);
        if (pHit && (pHit->BootMode == BOOT_WITH_FULL_CONFIGURATION)) {
            // Update Boot mode for ME.
            pHit->BootMode = BOOT_ASSUMING_NO_CONFIGURATION_CHANGES;
        }         
    }    
}

/**
    This function is the entry point for this DXE. This function
    initializes the chipset SB

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS EFIAPI SbDxeInit(
    IN EFI_HANDLE  ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_EVENT               Event;
    EFI_EVENT               SbExitBootServicesEvent;
#if CSM_SUPPORT    
    EFI_EVENT               UpdateRcNhltAcpiInLegacyEvent;
#endif    
    CPUINFO_HOB             *CpuInfoHob;
    EFI_GUID                HobListGuid = HOB_LIST_GUID;
    EFI_GUID                AmiCpuinfoHobGuid = AMI_CPUINFO_HOB_GUID;
    UINTN                   VariableSize = sizeof(SB_SETUP_DATA);
    EFI_EVENT               EndOfDxeEvent;    
    EFI_EVENT               BeforeBootEvent;
    EFI_EVENT               LegacyBootEvent;
    VOID                    *Registration = NULL;
#if HOTPLUG_SUPPORT
    PCI_HOST_BRG_DATA       *hb = NULL;
#endif
#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
    UINTN                   PciPcieAddrSize;
#endif
    AMI_CSP_GLOBALNVS_AREA_PROTOCOL     *AmiCspGlobalNvsAreaProtocol;
    
    InitAmiLib(ImageHandle, SystemTable);

    PROGRESS_CODE (DXE_SB_INIT);
    DEBUG_RAYDEBUG((-1, "DXE_SB_INIT\n"));

#if defined RESERVE_SBWARMRESETVAR && RESERVE_SBWARMRESETVAR == 1  
    InitVariable();
#else
    ClearWarmResetFlag();
#endif
    
    Status = pBS->LocateProtocol( &gEfiPciRootBridgeIoProtocolGuid, \
                                  NULL, \
                                  (VOID **)&gPciRootBridgeIo );
    ReportSBDxeError(Status);

    Status = pBS->LocateProtocol( &gEfiS3SaveStateProtocolGuid, \
                                  NULL, \
                                  (VOID **)&gBootScript );
    ReportSBDxeError(Status);

    //Find APIC ID Hob.
    CpuInfoHob = (CPUINFO_HOB*)GetEfiConfigurationTable( SystemTable, \
                                                         &HobListGuid );
    if (CpuInfoHob == NULL) Status = EFI_UNSUPPORTED;
    else Status = FindNextHobByGuid( &AmiCpuinfoHobGuid, (VOID**)&CpuInfoHob);
    ASSERT_EFI_ERROR(Status);

    // Install 8259 services
    Initialize8259(ImageHandle, SystemTable);

    // Install watchdog timer services
    WatchdogInit(ImageHandle, SystemTable);
    
    CreateSbPlatformInfo ();

#if defined HPET_PROTOCOL_SUPPORT && HPET_PROTOCOL_SUPPORT == 1
    TimerInit();
#endif

    Status = pBS->AllocatePool( EfiBootServicesData, \
                                VariableSize, \
                                (VOID **)&gSbSetupData );
    ASSERT_EFI_ERROR(Status);

    GetSbSetupData( pRS, gSbSetupData, FALSE );

    //
    // Enable or Disable the Devices depending upon the Setup.
    //
    PROGRESS_CODE (DXE_SB_DEVICES_INIT);

    ConfigPciDevices(SystemTable);

    CreateSbAcpiComponent();

#ifdef AMI_ONSITE_DEBUG
    // Clear RTC Reg C Alarm Flag
    SBClearRTC_AlarmFlag();
#endif

    Status = SBDXE_BoardInit(ImageHandle, SystemTable);

    Status = RegisterProtocolCallback( &gEfiPciIoProtocolGuid, \
                                       SbInitAfterDeviceInstall, \
                                       NULL, \
                                       &Event, \
                                       &gSbInitNotifyReg );
    ReportSBDxeError( Status );

    Status = RegisterProtocolCallback( &gOpromStartEndProtocolGuid, \
                                       SbCheckOprom, \
                                       NULL, \
                                       &Event, \
                                       &gCsmOpromReg );

    ReportSBDxeError( Status );

    Status = RegisterProtocolCallback( &gAmiTseNVRAMUpdateGuid, \
                                       SbSetupNvramUpdatedCallback, \
                                       NULL, \
                                       &Event, \
                                       &gSbSetupNvramUpdatedReg );
    ReportSBDxeError( Status );

// AMI_OVERRIDE_NB0000_START >>>
    //Reserve SB resources. All None Existant Memory Space Must be converted to MMIO now
    //Status = LibAllocCspResource(gSbResTable, gSbResTableCount, ImageHandle, NULL);
    //ASSERT_EFI_ERROR(Status);
// AMI_OVERRIDE_NB0000_END <<<

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
    Status = RegisterProtocolCallback( &gEfiUsbProtocolGuid,\
                                       SbUsbProtocolCallback,\
                                       NULL,\
                                       &gEvtUsbProtocol,\
                                       &gRegUsbProtocol );
    
    pBS->SignalEvent(gEvtUsbProtocol);
#endif
    
    Status = pBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    SbInitEndOfDxe,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    ReportSBDxeError(Status);

    Status = CreateReadyToBootEvent( TPL_NOTIFY, InitSbRegsBeforeBoot, \
                                     NULL, &BeforeBootEvent );
    ReportSBDxeError(Status);

    Status = CreateLegacyBootEvent( TPL_CALLBACK, \
                                    InitSbRegsBeforeLagecyBoot, \
                                    NULL, \
                                    &LegacyBootEvent );
    ReportSBDxeError(Status);

    Status = pBS->CreateEvent (
                    EVT_SIGNAL_EXIT_BOOT_SERVICES,
                    TPL_CALLBACK,
                    SbInExitBootServices,
                    NULL,
                    &SbExitBootServicesEvent
                    );
    ASSERT_EFI_ERROR (Status);
    
#if CSM_SUPPORT
    CreateLegacyBootEvent(TPL_CALLBACK, &UpdateRcNhltAcpiData, NULL, &UpdateRcNhltAcpiInLegacyEvent);
#endif 
    
#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1 
    RegisterProtocolCallback(
        &gAmiBeforeBootProtocolGuid,
        USBPerPortDisableCallback,
        NULL, &Event, &Registration
    );
	
#if CSM_SUPPORT
    CreateLegacyBootEvent(TPL_CALLBACK, &USBPerPortDisableCallback, NULL, &LegacyBootEvent);
#endif
#endif
   
    RegisterProtocolCallback(
        &gEfiGlobalNvsAreaProtocolGuid,
        UpdateGlobalNvsAreaCallback,
        NULL, &Event, &Registration
    );

    //Program SPI base VSCC 
    SBSpiProgramVSCC();

#if RaidDriver_SUPPORT
    InstallPchSataUefiDriver();
#endif

#if FORCE_USER_TO_SETUP_IF_CMOS_BAD                                  // [EIP88358] >>
    ForceSetupIfCmosBad();
#endif                                                               // [EIP88358] <<

#if HOTPLUG_SUPPORT
    Status = HpcInstallHpProtocol (hb);
    ASSERT_EFI_ERROR(Status);
#endif
    
#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
    PciPcieAddrSize = 100 * sizeof(UINT32);
    Status = pBS->AllocatePool (EfiReservedMemoryType, PciPcieAddrSize,  (VOID **)&PciPcieAddr);
    ASSERT_EFI_ERROR(Status);
    MemSet(PciPcieAddr, PciPcieAddrSize, 0);
	
    RegisterProtocolCallback(
		    &gEfiPciIoProtocolGuid,
		    GetStorageLSIAllDevicePath,
		    NULL, &LSIDeviceEvent, &gLSINotifyReg);
    
    Status = EfiCreateEventReadyToBootEx (
	        TPL_NOTIFY,
	        (EFI_EVENT_NOTIFY) StoreLSIDevicePath,
	        NULL,
	        &Event);
#endif    

    Status = pBS->LocateProtocol (
                    &gAmiCspGlobalNvsAreaProtocolGuid,
                    NULL,
                    (VOID **) &AmiCspGlobalNvsAreaProtocol
                    );
    if (!EFI_ERROR (Status)) {
        AmiCspGlobalNvsAreaProtocol->Area->NotifyPowerBtn = PcdGet8(AmiPcdCspNvsNtpb);
    }

    return EFI_SUCCESS;
}

/**
    Clear RTC Reg C Alarm Flag

    @param VOID

    @retval VOID

**/

VOID SBClearRTC_AlarmFlag(VOID)
{

    // Clear all SMI status here.
    // Must read RTC Reg C to be able to clear SMM RTC alarm flag.
    ReadCmos(0x0C);

    WRITE_IO16_PM(R_SB_ACPI_PM1_STS, 0xcd31);  // 0x00
    WRITE_IO32_PM(R_SB_ACPI_GPE0_STS_127_96, 0xffffffff);  // 0x8C
    WRITE_IO16_PM(R_SB_DEVACT_STS, 0xffff);    // 0x44
    
    WRITE_IO16_TCO(R_PCH_TCO1_STS, 0xffff);     // 0x04
    WRITE_IO16_TCO(R_PCH_TCO2_STS, 0xfffe);     // 0x06, Skip Intrusion [EIP73607]
    WRITE_IO32_PM(R_PCH_SMI_STS, 0xffffffff);   // 0x34
}

/**
    Program SPI VSCC.

    @param VOID

    @retval VOID

**/

VOID SBSpiProgramVSCC(VOID)
{
/*
    EFI_STATUS Status;
    UINT32 Data32 = 0;
    UINT32 dUVSCC = 0;
    UINT32 Mask = 0xFFFFFFFF;
    UINT8  Data8 = 0;
#if defined (SPI_INITIALIZE_WITH_VSCC) && (SPI_INITIALIZE_WITH_VSCC == 1)
    static FLASH_PROTOCOL *Flash = NULL;
#endif
#if defined (OEM_UPDATE_VSCC_TABLE_SUPPORT) && (OEM_UPDATE_VSCC_TABLE_SUPPORT == 1)
    SPI_VSCC_TABLE                  SpiVsccTbl[] = { OEM_SPI_VSCC_TABLE };
    UINTN  VsccTableCount = sizeof (SpiVsccTbl) / sizeof (SPI_VSCC_TABLE);
    UINT32 SpiCmd =0;
    UINT32 Timeout;
    UINTN  i;
#endif

  DEBUG((DEBUG_INFO, "[[ SBSpiProgramVSCC() Start. ]]\n"));

  // If Flash module support VSCC updated, skip programming VSCC.
  // If enable OEM_UPDATE_VSCC_TABLE_SUPPORT, override VSCC value by ELINK.
  if (((0 == READ_MEM32_SPI(R_SB_SPI_SFDP0_LVSCC)) &&
       (0 == READ_MEM32_SPI(R_SB_SPI_SFDP1_UVSCC))) || 
       (OEM_UPDATE_VSCC_TABLE_SUPPORT == 1)) {
#if defined (SPI_INITIALIZE_WITH_VSCC) && (SPI_INITIALIZE_WITH_VSCC == 1)
    Status = pBS->LocateProtocol(&gFlashProtocolGuid, NULL, &Flash);
    if (!EFI_ERROR(Status)) {
        // Identify Flash through FlashWriteEnable Hook.
        Flash->DeviceWriteEnable();
        Flash->DeviceWriteDisable();
    }
    if (((0 == READ_MEM32_SPI(R_SB_SPI_SFDP0_LVSCC)) &&
         (0 == READ_MEM32_SPI(R_SB_SPI_SFDP1_UVSCC))) || 
         (OEM_UPDATE_VSCC_TABLE_SUPPORT == 1))
#endif
    {
    dUVSCC = ((UINT32)(READ_MEM8_SPI(R_RCRB_SPI_OPMENU + 2)) << 8);

    Data8 = READ_MEM8_SPI(R_RCRB_SPI_PREOP + 1);
    if ((Data8 == 0x50) || (Data8 == 0x39))
        dUVSCC |= BIT03;

    if (Data8 == 0x39 ) {
        dUVSCC |= (BIT04 + BIT02);
    } else if(Data8 != 0x50) {
        dUVSCC |= BIT02;
    }

    if (READ_MEM8_SPI(R_RCRB_SPI_OPMENU + 2) == 0xD8)
        dUVSCC |= (BIT00 + BIT01);
    else
        dUVSCC |= (BIT00);
    
#if defined (OEM_UPDATE_VSCC_TABLE_SUPPORT) && (OEM_UPDATE_VSCC_TABLE_SUPPORT == 1)
    // Set SPI read-address = 0
    WRITE_MEM32_SPI(R_RCRB_SPI_FADDR, 0);
    SpiCmd = SPI_OPCODE_READ_ID_INDEX << 12;
    SpiCmd += ( ( 3 - 1 ) << 16 );
    SpiCmd += ( 1 << 22 );
    // Go (BIT1)
    WRITE_MEM32_SPI(R_RCRB_SPI_SSFSTS, SpiCmd | BIT09);

    // WaitForSpiCycleDone
    for ( Timeout = 0, i = 0; Timeout < 0x4000000; Timeout++ ) {
        i = READ_MEM8_SPI(R_RCRB_SPI_SSFSTS);
        if ( i & BIT02 ) break;
    }
    // IoDelay
    for ( Timeout = 0; Timeout < 33; Timeout++ ) {
        IoWrite8( 0xEB, 0x55 );
        IoWrite8( 0xEB, 0xAA );
    }
    // write BIT2 to clear CycleDone status
    WRITE_MEM8_SPI(R_RCRB_SPI_SSFSTS, BIT02);
    // Get Flash ID
    Data32 = READ_MEM32_SPI(R_RCRB_SPI_FDATA0) & 0x00FFFFFF;
    // Swap MSB/LSB
    Data32 = (((Data32 & 0xff) << 16) | ((Data32 & 0xff0000) >>16) | ((Data32 & 0xff00)));

    for ( i =0; i < VsccTableCount; i++ ) {
        if (SpiVsccTbl[i].VidDid == Data32) {
            dUVSCC = SpiVsccTbl[i].Vscc;
        }
    }
#endif
    WRITE_MEM32_SPI(R_RCRB_SPI_UVSCC, dUVSCC);
    WRITE_MEM32_SPI(R_RCRB_SPI_LVSCC, dUVSCC);
    }
  }
//####    }

    Status = ReadSPIDescriptor(0x01, 0x00, &Data32);
    if (!EFI_ERROR(Status))
    {
        Data8 = (UINT8)((Data32 & (BIT27 | BIT28 | BIT29)) >> 27);
        SET_MEM8_SPI(R_RCRB_SPI_SSFCTL + 2, Data8);
    }
    DEBUG((DEBUG_INFO, "[[ SBSpiProgramVSCC() Done. ]]\n"));*/
}


#if (ACPI_SUPPORT)

#include <Acpi20.h>

EFI_EVENT       mAcpiEvent;
VOID            *mAcpiReg;
EFI_EVENT       mAcpiSdtEvent;                      // [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
VOID            *mAcpiSdtReg;                       // [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
UINT64          gHpetBase = HPET_BASE_ADDRESS;
UINTN           mHpetTblHandle;
SB_ASL_BUFFER   *gSbAslBufPtr = NULL;
EFI_ACPI_TABLE_PROTOCOL     *gAt;

/**
    This function will create all ACPI components for SB when
    ACPI table protocol is available.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID 
EFIAPI
CreateSbAcpiCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    EFI_STATUS                  Status = EFI_SUCCESS;

    // It must be only one instance of such protocol
    Status = pBS->LocateProtocol( &gEfiAcpiTableProtocolGuid, NULL, (VOID **)&gAt );
    DEBUG((DEBUG_INFO, "PciHostCSHooks: LocateProtocol(ACPITableProtocol) = %r\n", Status));
    if(EFI_ERROR(Status)) return;
    
    Status = LocatePublishIdeSataAcpiTables();
    ASSERT_EFI_ERROR(Status);    
    
// [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
//    Status = UpdateDsdtAcpiTable();
//    ASSERT_EFI_ERROR(Status);

    // Kill the Event
    pBS->CloseEvent(Event);
}

/**
  Retrieves address of NHLT table from XSDT/RSDT.

  @retval NHLT_ACPI_TABLE*  Pointer to NHLT table if found
  @retval NULL              NHLT could not be found
**/

NHLT_ACPI_TABLE *
LocateNhltAcpiTable (
  VOID
  )
{
  EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  NHLT_ACPI_TABLE                               *Nhlt;
  UINTN                                         Index;
  UINT32                                        Data32;
  EFI_STATUS                                    Status;
  Rsdp  = NULL;
  Xsdt  = NULL;
  Nhlt  = NULL;

  ///
  /// Find the AcpiSupport protocol returns RSDP (or RSD PTR) address.
  ///
  DEBUG((DEBUG_INFO, "LocateNhltAcpiTable() Start\n"));

  Status = EfiGetSystemConfigurationTable (&gEfiAcpiTableGuid, (VOID *) &Rsdp);
  if (EFI_ERROR (Status) || (Rsdp == NULL)) {
    DEBUG((DEBUG_ERROR, "EFI_ERROR or Rsdp == NULL\n"));
    return NULL;
  }

  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN) Rsdp->XsdtAddress;
  if (Xsdt == NULL || Xsdt->Signature != EFI_ACPI_5_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    // If XSDT has not been found, check RSDT
    Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN) Rsdp->RsdtAddress;
    if (Xsdt == NULL || Xsdt->Signature != EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
      DEBUG((DEBUG_ERROR, "XSDT/RSDT == NULL or wrong signature\n"));
      return NULL;
    }
  }

  for (Index = sizeof (EFI_ACPI_DESCRIPTION_HEADER); Index < Xsdt->Length; Index = Index + sizeof (UINT32)) {
    Data32  = *(UINT32 *) ((UINT8 *) Xsdt + Index);
    Nhlt    = (NHLT_ACPI_TABLE *) (UINT32 *) (UINTN) Data32;
    if (Nhlt->Header.Signature == NHLT_ACPI_TABLE_SIGNATURE) {
      break;
    }
  }

  if (Nhlt == NULL || Nhlt->Header.Signature != NHLT_ACPI_TABLE_SIGNATURE) {
    DEBUG((DEBUG_ERROR, "Nhlt == NULL or wrong signature\n"));
    return NULL;
  }

  DEBUG((DEBUG_INFO, "Found NhltTable, Address = 0x%x\n", Nhlt));

  return Nhlt;
}

/**
  Sets NVS ACPI variables for _DSM accordingly to policy.

  @param[in]                NhltAcpiTableAddress
  @param[in]                NhltAcpiTableLength
  @param[in]                DspFeatureMask
  @param[in]                DspPpModuleMask
**/
VOID
EFIAPI
UpdateRcNhltAcpiData (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  NHLT_ACPI_TABLE     *NhltTable = NULL;
  EFI_STATUS          Status;
  PCH_NVS_AREA                              *mPchNvsArea;

  DEBUG((DEBUG_INFO, "UpdateRcNhltAcpiData() Start\n"));  
  
  if (Event != NULL) {
    pBS->CloseEvent(Event);
  }
  
  NhltTable = LocateNhltAcpiTable ();
  
  if(NhltTable != NULL) {
      if(gPchNvsAreaProtocol == NULL) {
         Status = pBS->LocateProtocol( &gPchNvsAreaProtocolGuid,
                                         NULL,
                                         (VOID **)&gPchNvsAreaProtocol );
         if (EFI_ERROR(Status)) return;
      }
      
      mPchNvsArea = gPchNvsAreaProtocol->Area;
      
      DEBUG((DEBUG_INFO, "UpdateRcNhltAcpiData():\n NHLT Address = 0x%08x\n", (UINT32)(UINTN)NhltTable));

      mPchNvsArea->NHLA = (UINT32)(UINTN)NhltTable;
//    mPchNvsAreaProtocol->Area->NHLL = (UINT32)(NhltTable->Header.Length);
  }
  DEBUG((DEBUG_INFO, "UpdateRcNhltAcpiData() End\n"));
}

/**
    Update DSDT ACPI table.

    @param VOID

    @retval EFI_STATUS
**/

// [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
/*
EFI_STATUS UpdateDsdtAcpiTable ( VOID )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  ACPI_HDR              *DsdtPtr = NULL;
  EFI_PHYSICAL_ADDRESS  DsdtAddr;
  PCH_SERIES            PchSeries = GetPchSeries();

  // Find DSDT ACPI Table
  Status = LibGetDsdt(&DsdtAddr, EFI_ACPI_TABLE_VERSION_ALL);
  if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "SBDXE: Fail to Get DSDT - returned %r\n", Status));
      ASSERT_EFI_ERROR(Status);
  } 
  DsdtPtr = (ACPI_HDR*)(UINTN)DsdtAddr;
  DEBUG((DEBUG_INFO, "SBDXE: Found DSDT Table at 0x%08X\n", DsdtPtr));

  // Update TRAD/ULT DSDT Table.
  TradUltDsdtTableUpdate ( DsdtPtr, PchSeries );

  return Status;
}
*/
EFI_STATUS EFIAPI UpdateDsdtAcpiTable(
    IN EFI_ACPI_SDT_HEADER    *Table,
    IN EFI_ACPI_TABLE_VERSION Version,
    IN UINTN                  TableKey
)
{
    PCH_SERIES            PchSeries = GetPchSeries();

    DEBUG((DEBUG_INFO, "SBDXE: Table->Signature = %x\n", Table->Signature));
    if (Table->Signature == DSDT_SIG) {
        DEBUG((DEBUG_INFO, "SBDXE: DSDT_SIG FOUND!!\n"));
        // Update TRAD/ULT DSDT Table.
        TradUltDsdtTableUpdate ( Table, PchSeries );
        return EFI_SUCCESS;
    }
    return EFI_UNSUPPORTED;
}
        
VOID 
EFIAPI
CreateSbAcpiSdtCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_ACPI_SDT_PROTOCOL       *AcpiSdt;
    EFI_ACPI_SDT_HEADER         *DsdtPtr = NULL;
#if !((ACPI_MODULE_VER >= 170) && (AMI_MODULE_PKG_VERSION >= 46))
    EFI_PHYSICAL_ADDRESS        DsdtAddr;
#endif
    PCH_SERIES                  PchSeries = GetPchSeries();
	
    Status = pBS->LocateProtocol( &gEfiAcpiSdtProtocolGuid, NULL, (VOID **)&AcpiSdt );
    DEBUG((DEBUG_INFO, "SBDXE: LocateProtocol(gEfiAcpiSdtProtocolGuid) = %r\n", Status));
    if(EFI_ERROR(Status)) return;

#if ((ACPI_MODULE_VER >= 170) && (AMI_MODULE_PKG_VERSION >= 46))
    Status = AmiGetDsdt(&DsdtPtr, EFI_ACPI_TABLE_VERSION_ALL);
#else
    Status = LibGetDsdt(&DsdtAddr, EFI_ACPI_TABLE_VERSION_ALL);
#endif
    if (!EFI_ERROR(Status)) {
#if !((ACPI_MODULE_VER >= 170) && (AMI_MODULE_PKG_VERSION >= 46))      
      DsdtPtr = (EFI_ACPI_SDT_HEADER*)(UINTN)DsdtAddr;
#endif
      DEBUG((DEBUG_INFO, "SBDXE: Found DSDT Table at 0x%08X\n", DsdtPtr));
      // Update TRAD/ULT DSDT Table.
      TradUltDsdtTableUpdate ( DsdtPtr, PchSeries );
    } else {
      DEBUG((DEBUG_ERROR, "SBDXE: Fail to Get DSDT, register a call back to monitor it.\n"));
      Status = AcpiSdt->RegisterNotify (TRUE, UpdateDsdtAcpiTable);
    }
    pBS->CloseEvent(Event);
}

/**
        This function  will locate and publish the IDE.asl or SATA.asl 
        depending upon the mode IDE/AHCI. 

        @param VOID
    
        @retval VOID

**/
EFI_STATUS LocatePublishIdeSataAcpiTables(VOID)
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol = NULL;

  INTN                          Instance;
  UINTN                         AcpiTableKey;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  EFI_ACPI_DESCRIPTION_HEADER   *SataAcpiTable = NULL;

  UINT8                         Data;
  BOOLEAN                       AhciFlag = FALSE;

  //Read the Sub class code register to check for ide/Ahci mode.
  Data = READ_PCI8_SATA(0x0A);

  if (Data == V_PCH_SATA_SUB_CLASS_CODE_AHCI || \
      Data == V_PCH_SATA_SUB_CLASS_CODE_RAID)
    AhciFlag = TRUE;
  //
  // Locate protocol.
  // There is little chance we can't find an FV protocol
  //
  DEBUG((DEBUG_INFO, "Locate FirmwareVolume2\n"));
  Status = pBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Looking for FV with ACPI storage file
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = pBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiFirmwareVolume2ProtocolGuid,
                  (VOID **)&FwVol
                  );
    ASSERT_EFI_ERROR (Status);

    //
    // See if it has the ACPI storage file
    //
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                    FwVol,
                    &IdeSataAcpiTableStorageGuid,
                    NULL,
                    &Size,
                    &FileType,
                    &Attributes,
                    &FvStatus
                    ); 
    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //
  //
  // Free any allocated buffers
  //
  pBS->FreePool (HandleBuffer);

  //
  // Sanity check that we found our data file
  //
  ASSERT (FwVol);

  //
  // Read tables from the storage file.
  //
  Instance = 0;
  CurrentTable = NULL;

  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &IdeSataAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **)&CurrentTable,
                      (UINTN *)&Size,
                      (UINT32 *)&FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      SataAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
      if(AhciFlag) {
        //AHCI mode is Enabled
        if (MemCmp (&SataAcpiTable->OemTableId, "SataTabl", 8) == 0) {
          DEBUG((DEBUG_INFO, "SataTabl.asl found!!\n"));
          break;
        }
      } 
      Instance++;
      CurrentTable = NULL;
    }
  }
  if (SataAcpiTable == NULL) return EFI_NOT_FOUND;

  //
  // Update the SSDT table in the ACPI tables.
  //
  DEBUG((DEBUG_INFO, "InstallAcpiTable Sata.asl\n"));
  AcpiTableKey = 0;
  Status = gAt->InstallAcpiTable (gAt, SataAcpiTable, SataAcpiTable->Length, &AcpiTableKey);

  DEBUG((DEBUG_INFO, "InstallAcpiTable() = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
#if (ACPI_MODULE_VER < 170)                         // [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
  pBS->FreePool (SataAcpiTable);
#endif  // #if (ACPI_MODULE_VER < 170)   

  return Status;
}

#endif 

/**
    This function will Enable/Disable onchip PCI device in SB 
    depend on SETUP questions.

    @param SystemTable - Pointer to the system table 

    @retval VOID

**/

VOID ConfigPciDevices (
    IN EFI_SYSTEM_TABLE *SystemTable )
{

}

/**
    This function creates all ACPI components supported by SB.

    @param VOID

    @retval VOID
**/

VOID CreateSbAcpiComponent (VOID)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    VOID            *ProtocolPointer;
	VOID            *SdtProtocolPointer;            // [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.

#if (ACPI_SUPPORT)
    Status = RegisterProtocolCallback( &gEfiAcpiTableProtocolGuid, \
                                       CreateSbAcpiCallback, \
                                       NULL, \
                                       &mAcpiEvent, \
                                       &mAcpiReg );
    // If AcpiSupport protocol has been installed we can use it right on
    // the way
    Status = pBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, \
                                  NULL, \
                                  (VOID **)&ProtocolPointer);
    if (!EFI_ERROR(Status)) pBS->SignalEvent(mAcpiEvent);

// [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
    Status = RegisterProtocolCallback( &gEfiAcpiSdtProtocolGuid, \
                                       CreateSbAcpiSdtCallback, \
                                       NULL, \
                                       &mAcpiSdtEvent, \
                                       &mAcpiSdtReg );
									   
    // If AcpiSdtSupport protocol has been installed we can use it right on
    // the way
    Status = pBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, \
                                  NULL, \
                                  (VOID **)&SdtProtocolPointer);
    if (!EFI_ERROR(Status)) {
        pBS->SignalEvent(mAcpiSdtEvent);
    }
#endif

}

/**
    This function tries to find the specific PCI Express extended
    capabilities ID structure address.

    @param Bus       - The PCI Bus number.
    @param Dev       - The PCI Device number.
    @param Fun       - The PCI Function number.
    @param FindCapId - the specific extended capabilities ID will be
        found.

        
    @retval EFI_SUCCESS Found the extended capabilities structure
        successfully, the input CapPtr16 will
        have the structure address.
    @retval EFI_NOT_FOUND Not found the extended capabilities
        structure. 
**/

EFI_STATUS GetExtCapStrucAddr (
    IN UINT8                Bus,
    IN UINT8                Dev,
    IN UINT8                Fun,
    IN UINT16               FindCapId,
    IN UINT16               *CapPtr16 )
{
    UINT32          Buffer32;

    *CapPtr16 = 0x100;

    Buffer32 = READ_PCI32(Bus, Dev, Fun, *CapPtr16);
    while (Buffer32 != 0xffffffff) {
        if ((UINT16)Buffer32 == FindCapId) return EFI_SUCCESS;
        *CapPtr16 = (UINT16)((Buffer32 >> 20) & 0xfffc);
        if (*CapPtr16 == 0) break;
        Buffer32 = READ_PCI32(Bus, Dev, Fun, *CapPtr16);
    }
    return EFI_NOT_FOUND;
}

/**
    This function tries to find the specific capabilities
    ID structure address.

    @param Bus       - The PCI Bus number.
    @param Dev       - The PCI Device number.
    @param Fun       - The PCI Function number.
    @param FindCapId - the specific legacy capabilities ID will be
        found.

        
    @retval EFI_SUCCESS Found the legacy capabilities structure
        successfully, the input CapPtr16 will
        have the structure address.
    @retval EFI_NOT_FOUND Not found the extended capabilities
        structure. 
**/

EFI_STATUS GetLegCapStrucAddr (
    IN UINT8                Bus,
    IN UINT8                Dev,
    IN UINT8                Fun,
    IN UINT8                FindCapId,
    IN UINT16               *CapPtr16 )
{
    UINT8           Buffer8;

    if (READ_PCI32(Bus, Dev, Fun, PCI_VID) != 0xffffffff) {
        if (READ_PCI16(Bus, Dev, Fun, PCI_STS) & 0x10) {
            *CapPtr16 = ((READ_PCI8(Bus, Dev, Fun, PCI_HDR) & 0x7f) == 2) ? \
                                                                    0x14:0x34;
            *CapPtr16 = (UINT16)READ_PCI8(Bus, Dev, Fun, *CapPtr16);
            if (*CapPtr16 == 0) return EFI_NOT_FOUND;
            Buffer8 = READ_PCI8(Bus, Dev, Fun, *CapPtr16);
            while (Buffer8 != 0) {
                if (Buffer8 == FindCapId) return EFI_SUCCESS;
                Buffer8 = (UINT8)(*CapPtr16) + 1;
                *CapPtr16 = (UINT16)(READ_PCI8(Bus, Dev, Fun, Buffer8));
                if (*CapPtr16 == 0) break;
                Buffer8 = READ_PCI8(Bus, Dev, Fun, *CapPtr16);
            }
        }
    }

    return EFI_NOT_FOUND;
}

#if SB_PCIE_ERROR_LOG_SUPPORT
/**
    Enable the error register of PCI-Express Device.

    @param Address -  PCI Express Config MMIO of device.

    @retval VOID

**/
VOID SbEnablePciDevErr(IN UINT64 Address)
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
      DEBUG((DEBUG_INFO, "READ PciCmdData (Enable SERR, BIT8)  = %x \n", READ_MEM16 (Address+0x04)));
  }      
  if (gSetupData.RtPerrEnable){
      SET_MEM16_S3(gBootScript, DevBaseAddr + PCI_CMD, BIT6);
      DEBUG((DEBUG_INFO, "READ PciCmdData (Enable PERR, BIT6)  = %x \n", READ_MEM16 (Address+0x04)));
  }
  
  CapPtr = SbFindCapPtr(DevBaseAddr, PCI_CAP_ID_PCIEXP);
  DEBUG((DEBUG_INFO, "CapPtr = %x \n", CapPtr));
        
  // Enable CEE/NFE/FEE
  // Root?    
  //if ((READ_MEM8(DevBaseAddr + CapPtr + CAP_XCAP_PORT_TYPE) & 0xF0) == 0x40){
  if(CapPtr != 0)
  {
     // Clear Device Error Status
     WRITE_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_DSTS, (BIT0 | BIT1 | BIT2));
     DEBUG((DEBUG_INFO, "Clear Error Status (DevBaseAddr + CapPtr + 0x0A) = %x\n", READ_MEM8(DevBaseAddr + CapPtr + 0x0A)));
     
     if (gSetupData.RtFatalErrorEnable){
         SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_RCTL, BIT2);     
         DEBUG((DEBUG_INFO, "Root? Read Capbility 0x1C = %x\n", READ_MEM8(DevBaseAddr + CapPtr + 0x1C)));      
     }
      
     if (gSetupData.RtUnCorrectedEnable){
         SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_RCTL, BIT1);     
         DEBUG((DEBUG_INFO, "Root? Read Capbility 0x1C = %x\n", READ_MEM8(DevBaseAddr + CapPtr + 0x1C)));      
     }
     
     if (gSetupData.RtCorrectedEnable){
         SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_RCTL, BIT0);     
         DEBUG((DEBUG_INFO, "Root? Read Capbility 0x1C = %x\n", READ_MEM8(DevBaseAddr + CapPtr + 0x1C)));      
     }      
      
      // End-Device?
   } //else if ((READ_MEM8(DevBaseAddr + CapPtr + CAP_XCAP_PORT_TYPE) & 0xF0) == 0x00){
      
   if (READ_MEM16(DevBaseAddr + PCI_SCC) == 0x0604)
   {	      
       if (gSetupData.RtFatalErrorEnable){
           SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_DCTL, BIT2); 
          DEBUG((DEBUG_INFO, "End-Device? Read Capbility 0x08 = %x\n", READ_MEM8(DevBaseAddr + CapPtr + 0x08)));  
       }
	   
       if (gSetupData.RtUnCorrectedEnable){  
           SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_DCTL, BIT1);   
           DEBUG((DEBUG_INFO, "End-Device? Read Capbility 0x08 = %x\n", READ_MEM8(DevBaseAddr + CapPtr + 0x08)));  
       } 
      
       if (gSetupData.RtCorrectedEnable){  
           SET_MEM8_S3(gBootScript, DevBaseAddr + CapPtr + CAP_DCTL, BIT0);     
           DEBUG((DEBUG_INFO, "End-Device? Read Capbility 0x08 = %x\n", READ_MEM8(DevBaseAddr + CapPtr + 0x08)));  
       }
    } 
}

/**
    Initialize PCI Express Device Error Handle.

    @param Address -  PCI Express Config MMIO of device.
	
	@retval VOID

**/
VOID SbPciExpressDeviceInitialize(IN UINT64 Address)
{
  UINT8   Dev;
  UINT8   Func;
  UINT8   CurrentBus;
  UINT16  Buffer16;
  UINT64  DevAddress;
  UINT8   PciFun = (Address >> 12) & 0x07;
  UINT8   CapPtr;
  UINT8   Buffer8;

  CapPtr = SbFindCapPtr(Address, PCI_CAP_ID_PCIEXP);
  Buffer8 = READ_MEM8(Address + CapPtr + CAP_DCTL);
  Buffer8 &= 0xF0;

  if (gSbSetupData->PcieRootPortURE[PciFun]){
      Buffer8 |= BIT03;
      DEBUG((DEBUG_INFO, "Bi03 Buffer8 = %x\n", Buffer8));
  }
  
  if (gSbSetupData->PcieRootPortFEE[PciFun] || (gSetupData.RtFatalErrorEnable == 0x01)){
      gSbSetupData->PcieRootPortFEE[PciFun] = gSetupData.RtFatalErrorEnable;  
      Buffer8 |= BIT02;
      DEBUG((DEBUG_INFO, "Bi02 Buffer8 = %x\n", Buffer8));
  }
  
  if (gSbSetupData->PcieRootPortNFE[PciFun] || (gSetupData.RtUnCorrectedEnable == 0x01)){
      gSbSetupData->PcieRootPortNFE[PciFun] = gSetupData.RtUnCorrectedEnable ;	  
      Buffer8 |= BIT01;
      DEBUG((DEBUG_INFO, "Bi01 Buffer8 = %x\n", Buffer8));
  }
  
  if (gSbSetupData->PcieRootPortCEE[PciFun] || (gSetupData.RtCorrectedEnable)){
      gSbSetupData->PcieRootPortCEE[PciFun] = gSetupData.RtCorrectedEnable;
      Buffer8 |= BIT00;
      DEBUG((DEBUG_INFO, "Bi00 Buffer8 = %x\n", Buffer8));
  }  

  // Clear Status
  WRITE_MEM8_S3(gBootScript, Address + CAP_DSTS, 0xff);
  
  // Set Status
  WRITE_MEM8_S3( gBootScript, \
                 Address + CapPtr + CAP_DCTL, \
                 Buffer8 );
 
  CurrentBus = READ_MEM8((UINT32)Address + PCI_SBUS);

  for (Dev = 0; Dev < 32; Dev++)
  {
    for (Func = 0; Func < 8; Func++)
    {    	
      DevAddress = (UINT64)SB_PCIE_CFG_ADDRESS(CurrentBus, Dev, Func, 0);

      if (READ_MEM16(DevAddress) == 0xFFFF)
          continue;
      
      SbEnablePciDevErr(DevAddress);      
      
      Buffer16 = READ_MEM16((UINT32)SB_PCIE_CFG_ADDRESS(CurrentBus, Dev, 0, 0) + PCI_SCC);
      
      if (Buffer16 == 0x0604)
      {
          DevAddress = (UINT64)SB_PCIE_CFG_ADDRESS(CurrentBus, Dev, 0, 0);
        
          if (gSetupData.RtSerrEnable){
              SET_MEM8_S3(gBootScript, DevAddress + 0x3E, BIT1);                  
              DEBUG((DEBUG_INFO, "READ BCTRL (Enable SERR, BIT1)  = %x \n", READ_MEM16 (DevAddress+0x0E))); 
          }
          
          if (gSetupData.RtPerrEnable){
              SET_MEM8_S3(gBootScript, DevAddress + 0x3E, BIT0);  
              DEBUG((DEBUG_INFO, "READ BCTRL (Enable SERR, BIT0)  = %x \n", READ_MEM16 (DevAddress+0x0E))); 
          }
      
          SbPciExpressDeviceInitialize(DevAddress);
      
      }
    }
  }
}
#endif

/**
    This callback function is called when a PCI I/O Protocol is
    installed.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval EFI_SUCCESS
**/

VOID EFIAPI SbInitAfterDeviceInstall (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    EFI_STATUS              Status;
    UINTN                   BufferSize = 20 * sizeof(EFI_HANDLE);
    EFI_HANDLE              Handle;
    EFI_PCI_IO_PROTOCOL     *PciIo;
    PCI_STD_DEVICE          Pci;
    UINTN                   PciSeg;
    UINTN                   PciBus;
    UINTN                   PciDev;
    UINTN                   PciFun;
#if SB_PCIE_ERROR_LOG_SUPPORT
    UINT64                  Address = 0;
#endif

    Status = pBS->LocateHandle( ByRegisterNotify, \
                                NULL, \
                                gSbInitNotifyReg, \
                                &BufferSize, \
                                &Handle );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;
    //
    // Locate PciIo protocol installed on Handle
    //

    Status = pBS->HandleProtocol( Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;

    // Get PCI Device Bus/Device/Function Numbers
    Status = PciIo->GetLocation( PciIo, &PciSeg, &PciBus, &PciDev, &PciFun );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;

    DEBUG((DEBUG_INFO, "PCI Bus, Device, function = %X, %X,%X\n", PciBus, PciDev, PciFun ));

    if (((UINT8)PciBus == SB_SMBUS_BUS) && \
        ((UINT8)PciDev == SB_SMBUS_DEV) && \
        ((UINT8)PciFun == SB_SMBUS_FUN)) {
      if ((READ_PCI32_SMBUS(0x10) & 0xFFFFFFF0) &&        // PCI_BASE_ADDRESSREG_OFFSET
          ((READ_PCI8_SMBUS(PCI_COMMAND_OFFSET) & BIT1) == 0))
        SET_PCI8_SMBUS(PCI_COMMAND_OFFSET, BIT1);       // EFI_PCI_COMMAND_MEMORY_SPACE

      if ((READ_PCI16_SMBUS(R_PCH_SMBUS_BASE) & 0xFFC0) && \
          ((READ_PCI8_SMBUS(PCI_COMMAND_OFFSET) & BIT0) == 0))
        SET_PCI8_SMBUS(PCI_COMMAND_OFFSET, BIT0);       // EFI_PCI_COMMAND_IO_SPACE

      gEventCount -= 1;
    }
    if (((UINT8)PciBus == SB_HDA_BUS) && \
        ((UINT8)PciDev == SB_HDA_DEV) && \
        ((UINT8)PciFun == SB_HDA_FUN)) {
      if(gSbSetupData->PchHdAudioPme){
          SET_PCI16_HDA(R_PCH_HDA_PCS, B_PCH_HDA_PCS_PMEE);
      }
      gEventCount -= 1;
    }

#if defined(SB_I2C_HID_SUPPORT) && (SB_I2C_HID_SUPPORT == 1)    
    if (((UINT8)PciBus == SB_BUS) && \
        (((UINT8)PciDev == SB_SIO_I2C0_DEV) || ((UINT8)PciDev == SB_SIO_I2C1_DEV) || \
         ((UINT8)PciDev == SB_SIO_I2C2_DEV) || ((UINT8)PciDev == SB_SIO_I2C3_DEV)) && \
        (((UINT8)PciFun == SB_SIO_I2C0_FUN) || ((UINT8)PciFun == SB_SIO_I2C1_FUN) || \
         ((UINT8)PciFun == SB_SIO_I2C2_FUN) || ((UINT8)PciFun == SB_SIO_I2C3_FUN))) {
      WRITE_PCI8(PciBus, PciDev, PciFun, PCI_COMMAND_OFFSET, BIT2|BIT1); //EFI_PCI_COMMAND_BUS_MASTER, EFI_PCI_COMMAND_MEMORY_SPACE
      gEventCount -= 1;
    }
 
    if (((UINT8)PciBus == SB_BUS) && \
        (((UINT8)PciDev == SB_SIO_I2C4_DEV) || ((UINT8)PciDev == SB_SIO_I2C5_DEV)) && \
        (((UINT8)PciFun == SB_SIO_I2C4_FUN) || ((UINT8)PciFun == SB_SIO_I2C5_FUN))) {
      WRITE_PCI8(PciBus, PciDev, PciFun, PCI_COMMAND_OFFSET, BIT2|BIT1); //EFI_PCI_COMMAND_BUS_MASTER, EFI_PCI_COMMAND_MEMORY_SPACE
      gEventCount -= 1;
    } 
#endif    

    if (((UINT8)PciBus == SB_SATA_BUS) && \
        ((UINT8)PciDev == SB_SATA_DEV) && \
        ((UINT8)PciFun == SB_SATA_FUN)) {
      Status = PciIo->Pci.Read (PciIo,
                                EfiPciIoWidthUint32,
                                0,
                                sizeof (Pci) / sizeof (UINT32),
                                &Pci);

      //Check for Onboard Raid controller and if's it's onboard install the Guid on that Handle. 
      if (!EFI_ERROR (Status))
      {
        if ( Pci.Header.ClassCode[1] == PCI_CL_MASS_STOR_SCL_RAID && \
             Pci.Header.VendorId == 0x8086 && \
             (Pci.Header.DeviceId == 0x282A || Pci.Header.DeviceId == 0x2822 || Pci.Header.DeviceId == 0x2826) ) { // for mobile, Desktop, WS and Server

           pBS->InstallProtocolInterface(&Handle, \
                                         &gOnboardRaidGuid, \
                                         EFI_NATIVE_INTERFACE, \
                                         NULL);

// [ EIP405844 ] ->>>
//            //
//            // Install the Bus Specific Override Protocol on the Raid Controller Handle
//            //
//            Status = pBS->InstallMultipleProtocolInterfaces(&Handle,
//                        &gEfiBusSpecificDriverOverrideProtocolGuid,
//                        &RaidBusSpecificDriverOverride,
//                        NULL);
// [ EIP405844 ] ->>>

           ASSERT_EFI_ERROR(Status);
        }
      }
      gEventCount -= 1;
    }

    if (((UINT8)PciBus == DEFAULT_PCI_BUS_NUMBER_PCH) && \
        (((UINT8)PciDev == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1) || \
         ((UINT8)PciDev == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2) || \
         ((UINT8)PciDev == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3) )) {
        
      gEventCount -= 1;
    }
    
    // Kill the Event
    if (gEventCount == 1)
        pBS->CloseEvent(Event);
}

/**
    This callback function is called before/after processing all
    PCI optonal ROM.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID 
EFIAPI
SbCheckOprom (
    IN EFI_EVENT   Event,
    IN VOID        *Context)
{
    EFI_STATUS                      Status;
    UINTN                           BufferSize = sizeof(EFI_HANDLE);
    EFI_HANDLE                      Handle;
    AMI_CSM_PLATFORM_POLICY_DATA    *CsmPolicyData;
    UINTN                           Seg;
    UINTN                           Bus;
    UINTN                           Dev;
    UINTN                           Fun;
       
    Status = pBS->LocateHandle (ByRegisterNotify, NULL, gCsmOpromReg, &BufferSize, &Handle);
    if (EFI_ERROR(Status)) return;

    Status = pBS->HandleProtocol(Handle, &gOpromStartEndProtocolGuid, (VOID **)&CsmPolicyData);
    if (EFI_ERROR(Status)) return;
   
    if (CsmPolicyData == NULL) return;  // PostProcessOpRom callback
	if (CsmPolicyData->PciIo == NULL) return;

    if (gSbSetupData->SataInterfaceMode == SATA_MODE_RAID) {	
      Status = CsmPolicyData->PciIo->GetLocation (CsmPolicyData->PciIo, &Seg, &Bus, &Dev, &Fun);
      if (EFI_ERROR(Status)) return;
      
      if ((Bus == SB_SATA_BUS) && (Dev == SB_SATA_DEV) && (Fun == SB_SATA_FUN)) {
        if (gSbSetupData->SataLegacyOrom == 1)
          CsmPolicyData->ExecuteThisRom = TRUE;
        else
          CsmPolicyData->ExecuteThisRom = FALSE;
        
        DEBUG((DEBUG_INFO, "SbCheckOprom: ExecuteThisRom = %x\n", CsmPolicyData->ExecuteThisRom));
      }
    }
    else {
      //Close the event if needed.
      pBS->CloseEvent(Event);
    }
}

//**********************************************************************

BOOT_SCRIPT_SB_PCI_REG_SAVE gPciRegistersSave[] = {
    SB_LPC_REG(R_SB_LPC_GEN1_DEC)         , EfiBootScriptWidthUint32, 0xffffffff, // 0x84
    SB_LPC_REG(R_SB_LPC_GEN2_DEC)         , EfiBootScriptWidthUint32, 0xffffffff, // 0x88
    SB_LPC_REG(R_SB_LPC_GEN3_DEC)         , EfiBootScriptWidthUint32, 0xffffffff, // 0x8C
    SB_LPC_REG(R_SB_LPC_GEN4_DEC)         , EfiBootScriptWidthUint32, 0xffffffff, // 0x90       
    
    SB_SMBUS_REG(R_SB_SMBUS_MBASE0_ADDR) , EfiBootScriptWidthUint32, 0xffffffff, // 0x10
    SB_SMBUS_REG(R_SB_SMBUS_MBASE1_ADDR) , EfiBootScriptWidthUint32, 0xffffffff, // 0x14
    SB_SMBUS_REG(R_SB_SMBUS_BASE_ADDR)   , EfiBootScriptWidthUint32, 0xffffffff, // 0x20
    SB_SMBUS_REG(R_SB_SMBUS_INTR_LN)     , EfiBootScriptWidthUint16, 0xffffffff, // 0x3C
    SB_SMBUS_REG(R_SB_SMBUS_PCICMD)      , EfiBootScriptWidthUint16, 0xffffffff, // 0x04
};

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
VOID CheckDisableUsbControllers(VOID)
{
    EFI_STATUS Status;
    EFI_GUID   EfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE;
    UINTN      BootOrderSize = 0;
    UINT16     *BootOrder = NULL;
    
    Status = pBS->LocateProtocol( &gEfiUsbProtocolGuid,
                                  NULL,
                                  (VOID **)&gUsbProtocol );
    if (EFI_ERROR(Status)) return;
    
    if (gDisableAllUsbControllers){
        Status = pRS->GetVariable( L"BootOrder", \
                                   &EfiGlobalVariableGuid, \
                                   NULL, \
                                   &BootOrderSize, \
                                   &BootOrder );
        if (Status == EFI_NOT_FOUND) return;
        
        // Shutdown legacy
        gUsbProtocol->UsbRtShutDownLegacy();
        
        WRITE_PCI16(SB_XHCI_BUS, SB_XHCI_DEV, SB_XHCI_FUN, R_SB_XHCI_PCICMD, 0);
        
        MmioOr32((PWRM_BASE_ADDRESS + R_PCH_PWRM_NST_PG_FDIS_1), B_PCH_PWRM_NST_PG_FDIS_1_XHCI_FDIS_PMC);
    }
}
#endif

#if (defined(SMBIOS_SUPPORT) && (SMBIOS_SUPPORT == 1))
/**
    Build and fill SmBios type 0x88 for CRID.

    @retval EFI_STATUS EFI_SUCCESS.

**/
VOID
UpdateSmbios136Table (VOID)
{
    EFI_STATUS                        Status;
    EFI_MISC_OEM_TYPE_0x88            Data88;
    AMI_SMBIOS_PROTOCOL               *AmiSmbiosProtocol = NULL;
    SB_SETUP_DATA                     *gSbSetupData = NULL;
    UINTN                             VariableSize = sizeof(SB_SETUP_DATA);

    Status = pBS->LocateProtocol(&gAmiSmbiosProtocolGuid, NULL, (VOID **)&AmiSmbiosProtocol);
    if (EFI_ERROR(Status)) return;
    
    // Clear all data
    pBS->SetMem (&Data88, sizeof(EFI_MISC_OEM_TYPE_0x88), 0);

    Data88.Header.Type = 0x88;
    Data88.Header.Length = 6;
    Data88.Header.Handle = 0;

    //
    // Get PCH Setup Data.
    //    
    Status = pBS->AllocatePool( EfiBootServicesData, \
                                VariableSize, \
                                (VOID **)&gSbSetupData );
    ASSERT_EFI_ERROR(Status);    
    
    GetSbSetupData( pRS, gSbSetupData, FALSE );
    
    DEBUG((DEBUG_INFO, "SbDxe.c :: gSbSetupData->PchCrid = %x\n",gSbSetupData->PchCrid));
    if (gSbSetupData->PchCrid == 1){
        Data88.OemInfo = 0x5a5a;
    }
    
    // Free memory used for setup data
    pBS->FreePool( gSbSetupData );

    Status = AmiSmbiosProtocol->SmbiosAddStructure((UINT8 *)&Data88, sizeof(EFI_MISC_OEM_TYPE_0x88));
    ASSERT_EFI_ERROR(Status);
}
#endif

/**

    @param VOID

    @retval VOID
**/
VOID SbMiscProgramBootScript ( VOID )
{
    UINT32      i, Data32;   
#if SB_PCIE_ERROR_LOG_SUPPORT
    UINT8       CapPtr = 0;
    UINT8       PchBus;
    UINT8       PchDev;
    UINT8       PchFun;
    UINTN       PchAddress;
    EFI_GUID    SetupGuid = SETUP_GUID;
    UINTN       Size = sizeof(SETUP_DATA);
    EFI_STATUS  Status;
    UINT16      IsBridge;
    UINT32      Attributes = 0;
#endif
    
    DEBUG((DEBUG_INFO, "SbMiscProgramBootScript - Start\n"));    
    
    for (i = 0; i < (sizeof(gPciRegistersSave)/sizeof(BOOT_SCRIPT_SB_PCI_REG_SAVE)); ++i) {
        gPciRootBridgeIo->Pci.Read (gPciRootBridgeIo,
                                    EfiBootScriptWidthUint32,
                                    gPciRegistersSave[i].Address,
                                    1, &Data32);
        if (Data32 == 0xFFFFFFFF) continue;
        
        gPciRootBridgeIo->Pci.Read (gPciRootBridgeIo,
                                    gPciRegistersSave[i].Width,
                                    gPciRegistersSave[i].Address,
                                    1, &Data32);
        
        Data32 &= gPciRegistersSave[i].Mask;
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO (gBootScript, 
                                               gPciRegistersSave[i].Width,
                                               gPciRegistersSave[i].Address,
                                               1, &Data32);
    } //  end for loop
  
#if SB_PCIE_ERROR_LOG_SUPPORT
    
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
             PchBus = gDevicesTable[i].Bus;
             PchDev = gDevicesTable[i].Dev;
             PchFun = gDevicesTable[i].Fun;
             Data32 = READ_PCI32 (PchBus, PchDev, PchFun, PCI_VID); 
             if (Data32 == 0xFFFFFFFF) continue;
              
             PchAddress = SB_PCIE_CFG_ADDRESS(PchBus, PchDev, PchFun, 0);
             IsBridge = READ_MEM16(PchAddress + PCI_SCC);
               
             //Clear PCI Error status of PCI status and Secondary status
             WRITE_MEM8_S3(gBootScript, PchAddress + PCI_STS_ERROR, 0xff);
             WRITE_MEM8_S3(gBootScript, PchAddress + PCI_SECSTATUS_ERROR, 0xff);
              
             //
             //Set SERR/PERR Enable for Root Port.
             //
             if (gSetupData.RtSerrEnable){
                 SET_MEM16_S3(gBootScript, PchAddress + PCI_CMD, BIT8);                  
                 DEBUG((DEBUG_INFO, "READ Primary PciCmdData (Enable PERR, BIT6)  = %x \n", READ_MEM16 (PchAddress+0x04))); 
             }
         
             if (gSetupData.RtPerrEnable){
                 SET_MEM16_S3(gBootScript, PchAddress + PCI_CMD, BIT6);                  
                 DEBUG((DEBUG_INFO, "READ Primary PciCmdData (Enable PERR, BIT6)  = %x \n", READ_MEM16 (PchAddress+0x04)));
             } 
              
             //Check if it's bridge. If it's bridge, set 0x3E Bit0 and Bit 1 for SERR and PERR.
             if (IsBridge == 0x0604){
                if (gSetupData.RtSerrEnable){
                    SET_MEM8_S3(gBootScript, PchAddress + PCI_BRIDGE_CNTL, BIT1);                  
                    DEBUG((DEBUG_INFO, "READ Primary PciCmdData (Enable SERR, BIT1)  = %x \n", READ_MEM16 (PchAddress+0x3E))); 	       
	            }
                
                if (gSetupData.RtPerrEnable){
                    SET_MEM8_S3(gBootScript, PchAddress + PCI_BRIDGE_CNTL, BIT0);  
                    DEBUG((DEBUG_INFO, "READ Primary PciCmdData (Enable SERR, BIT0)  = %x \n", READ_MEM16 (PchAddress+0x3E))); 
                }
             }
	          
             // if enable PCI SERR and PERR
             gErrorLoggingFlag = TRUE;
    
             SbPciExpressDeviceInitialize(PchAddress);
           
             CapPtr = SbFindCapPtr(PchAddress, PCI_CAP_ID_PCIEXP);
    
             if (CapPtr != 0) 
             {            	 
                 // Clear Device Error status
                 WRITE_MEM8_S3(gBootScript, PchAddress + CapPtr + CAP_DSTS, (BIT0 | BIT1 | BIT2));
                              
                 // Enable the error bits of Device Control
                 if (gSetupData.RtCorrectedEnable){
                    SET_MEM8_S3(gBootScript, PchAddress + CapPtr + CAP_DCTL, (BIT0));
                    DEBUG((DEBUG_INFO, "PchAddress + CapPtr  + 0x08 (BIT0)= %x \n", READ_MEM8(PchAddress + CapPtr + 0x08) ));
                 }
                  
                 if (gSetupData.RtUnCorrectedEnable){
                     SET_MEM8_S3(gBootScript, PchAddress + CapPtr + CAP_DCTL, (BIT1));
                     DEBUG((DEBUG_INFO, "PchAddress + CapPtr  + 0x08 (BIT1)= %x \n", READ_MEM8(PchAddress + CapPtr + 0x08) ));
                 }
             
                 if (gSetupData.RtFatalErrorEnable){
                     SET_MEM8_S3(gBootScript, PchAddress + CapPtr + CAP_DCTL, (BIT2));
                     DEBUG((DEBUG_INFO, "PchAddress + CapPtr  + 0x08 (BIT2)= %x \n", READ_MEM8(PchAddress + CapPtr + 0x08) ));
                 }
             
                 //
                 // Enable the error bits of Root Control
                 //
                 if (gSetupData.RtCorrectedEnable){
                     SET_MEM8_S3(gBootScript, PchAddress + CapPtr + CAP_RCTL, (BIT0));
                     DEBUG((DEBUG_INFO, "PchAddress + CapPtr  + 0x1C (BIT0)= %x \n", READ_MEM8(PchAddress + CapPtr + 0x1C) ));
                 }
                  
                 if (gSetupData.RtUnCorrectedEnable){
                     SET_MEM8_S3(gBootScript, PchAddress + CapPtr + CAP_RCTL, (BIT1));
                     DEBUG((DEBUG_INFO, "PchAddress + CapPtr  + 0x1C (BIT1)= %x \n", READ_MEM8(PchAddress + CapPtr + 0x1C) ));
                 }
             
                 if (gSetupData.RtFatalErrorEnable){
                     SET_MEM8_S3(gBootScript, PchAddress + CapPtr + CAP_RCTL, (BIT2));
                     DEBUG((DEBUG_INFO, "PchAddress + CapPtr  + 0x1C (BIT2)= %x \n", READ_MEM8(PchAddress + CapPtr + 0x1C) ));
                 }
              }
          }
      }
    }//if (gSetupData.RtPciErrorEnable == 1)
#endif
  
    DEBUG((DEBUG_INFO, "SbMiscProgramBootScript - End\n"));
}

/**
  This function handles SbDxeInit task at the end of DXE

  @param[in]  Event
  @param[in]  *Context
**/
VOID
EFIAPI
SbInitEndOfDxe (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
    DEBUG((DEBUG_INFO, "SBDXE: SbInitEndOfDxe\n"));
    
    SbMiscProgramBootScript ();
       
    pBS->CloseEvent (Event);

    return;
}

/**
    This function can initialize any SB registers before DXE
    stage exiting.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID 
EFIAPI
InitSbRegsBeforeBoot (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{  
    DEBUG((DEBUG_INFO, "SBDXE: InitSbRegsBeforeBoot\n"));

#if (defined(SMBIOS_SUPPORT) && (SMBIOS_SUPPORT == 1))
    UpdateSmbios136Table ();
#endif

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
    CheckDisableUsbControllers();
#endif
    
    // Trigger SWSMI to save bootscript.
    IoWrite8 (SW_SMI_IO_ADDRESS, SW_SMI_SCRIPT_TABLE_SAVE);
   
    pBS->CloseEvent(Event);
}

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
/**
    This callback function is called after USB Protocol is 
    installed.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID 
EFIAPI 
SbUsbProtocolCallback (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context )
{
    EFI_STATUS    Status = EFI_SUCCESS;
    USB_SKIP_LIST SkipMassTable[] = { {1, 0, 0xff, 0, 0, 0x8},
                                      {0, 0, 0,    0, 0, 0  }
                                    };
    EFI_USB_POLICY_PROTOCOL *EfiUsbPolicyProtocol;	
#if USB_DRIVER_MAJOR_VER >= 22  // EIP426082
    UINT8          SkipTableSize;
#endif  //USB_DRIVER_MAJOR_VER >= 22


#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1	
    PCH_SERIES 		PchSeries = GetPchSeries();
    UINT8           Port = 0, IndexPort = 0, SsPortIndex;
    USB_SKIP_LIST   *SkipOemMassTable; 
    USB_SKIP_LIST   *SklSkipMassTable;
    
    USB_SKIP_LIST SkipPchHUSBMassTable[] = {
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 1, 0, 0x8},  // USB_P1
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 2, 0, 0x8},  // USB_P2
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 3, 0, 0x8},  // USB_P3
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 4, 0, 0x8},  // USB_P4
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 5, 0, 0x8},  // USB_P5
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 6, 0, 0x8},  // USB_P6
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 7, 0, 0x8},  // USB_P7
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 8, 0, 0x8},  // USB_P8             
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 9, 0, 0x8},  // USB_P9
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 10, 0, 0x8}, // USB_P10
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 11, 0, 0x8}, // USB_P11
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 12, 0, 0x8}, // USB_P12
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 13, 0, 0x8}, // USB_P13
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 14, 0, 0x8}, // USB_P14
            
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x11, 0, 0x8}, // USB3_P1
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x12, 0, 0x8}, // USB3_P2
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x13, 0, 0x8}, // USB3_P3
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x14, 0, 0x8}, // USB3_P4
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x15, 0, 0x8}, // USB3_P5
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x16, 0, 0x8}, // USB3_P6
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x17, 0, 0x8}, // USB3_P7
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x18, 0, 0x8}, // USB3_P8
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x19, 0, 0x8}, // USB3_P9
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 0x1A, 0, 0x8}  // USB3_P10               
    };
    
    USB_SKIP_LIST SkipPchLpUSBMassTable[] = {
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 1, 0, 0x8},  // USB_P1
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 2, 0, 0x8},  // USB_P2
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 3, 0, 0x8},  // USB_P3
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 4, 0, 0x8},  // USB_P4
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 5, 0, 0x8},  // USB_P5
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 6, 0, 0x8},  // USB_P6
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 7, 0, 0x8},  // USB_P7
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 8, 0, 0x8},  // USB_P8             
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 9, 0, 0x8},  // USB_P9
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 10, 0, 0x8}, // USB_P10
            
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 13, 0, 0x8}, // USB3_P1
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 14, 0, 0x8}, // USB3_P2
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 15, 0, 0x8}, // USB3_P3
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 16, 0, 0x8}, // USB3_P4
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 17, 0, 0x8}, // USB3_P5
            {0, SKIP_FLAG_SKIP_MULTI_LEVEL, 0xA0, 18, 0, 0x8}  // USB3_P6             
    };
#endif
    Status = pBS->LocateProtocol(&gEfiUsbPolicyProtocolGuid,
                                NULL,
                                (VOID **)&EfiUsbPolicyProtocol);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }

   
    Status = pBS->AllocatePool (EfiBootServicesData, 1 * sizeof(AMI_USB_HW_SMI_HC_CONTEXT*), 
        (VOID**)&EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcContext);

    EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcContext[0] = gXhciHwSmiContext;

    EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcCount = 1;

    Status = pBS->LocateProtocol( &gEfiUsbProtocolGuid,
                                  NULL,
                                  (VOID **)&gUsbProtocol );
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }
    
    if ( gDisableAllUsbControllers ) {
#if USB_DRIVER_MAJOR_VER >= 22  // EIP426082
        SkipTableSize = sizeof(SkipMassTable)/sizeof (USB_SKIP_LIST);
        Status = gUsbProtocol->UsbCopySkipTable(SkipMassTable, \
                                                &SkipTableSize, \
                                                FALSE);
#else
        gUsbProtocol->UsbCopySkipTable(SkipMassTable, \
                                       sizeof(SkipMassTable)/sizeof (USB_SKIP_LIST));
#endif  //USB_DRIVER_MAJOR_VER >= 22
        pBS->CloseEvent(Event);
        return;
    }

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1   
    //Report the SKIP USB port table for USB per-port disable
    
    if(gSbSetupData->PchUsbPortDisable == 1){	   
        if (PchSeries == PchH) {
            SkipOemMassTable = MallocZ(sizeof(SkipPchHUSBMassTable) + sizeof (USB_SKIP_LIST));
            SklSkipMassTable = SkipPchHUSBMassTable;
        } else {
            SkipOemMassTable = MallocZ(sizeof(SkipPchLpUSBMassTable) + sizeof (USB_SKIP_LIST));
            SklSkipMassTable = SkipPchLpUSBMassTable;
        }
        ASSERT(SkipOemMassTable);

        // For USB per-port disable
        for (Port = 0; Port < GetPchUsbMaxPhysicalPortNum(); Port++) {
            if (gSbSetupData->PchUsbHsPort[Port] == PCH_DEVICE_DISABLE) {
                SkipOemMassTable[IndexPort++] = SklSkipMassTable[Port];
                DEBUG((DEBUG_INFO, "Disable USB Port = %x\n", Port));               
            }
        }
        
        SsPortIndex = GetPchUsbMaxPhysicalPortNum();
        
        // For USB3 per-port disable
        for (Port = 0; Port < GetPchXhciMaxUsb3PortNum(); Port++) {
            if (gSbSetupData->PchUsbSsPort[Port] == PCH_DEVICE_DISABLE) {
                SkipOemMassTable[IndexPort++] = SklSkipMassTable[Port + SsPortIndex];
                DEBUG((DEBUG_INFO, "Disable USB3 Port = %x\n", Port));               
            }
        }        

        if ( IndexPort != 0 ) {
#if USB_DRIVER_MAJOR_VER >= 22  // EIP426082
            SkipTableSize = IndexPort +1;
            gUsbProtocol->UsbCopySkipTable(SkipOemMassTable, \
                                           &SkipTableSize, \
                                           FALSE);
#else            
            gUsbProtocol->UsbCopySkipTable(SkipOemMassTable, \
                                           IndexPort +1);
#endif  //USB_DRIVER_MAJOR_VER >= 22
        }

        pBS->FreePool( SkipOemMassTable );
    }
#endif

    pBS->CloseEvent(Event);
}
#endif

/**
    This callback function is update global NVS area.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/
VOID 
EFIAPI
UpdateGlobalNvsAreaCallback (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context )
{
    EFI_STATUS    Status = EFI_SUCCESS;
    EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsArea;

    //
    // Locate the Global NVS Protocol.
    //
    Status = pBS->LocateProtocol (
                    &gEfiGlobalNvsAreaProtocolGuid,
                    NULL,
                    (VOID **)&GlobalNvsArea
                    );
    ASSERT_EFI_ERROR (Status);
    
    if (!EFI_ERROR(Status)) {
      //
      // Update GlobalNvsArea.
      //
    }
    pBS->CloseEvent(Event);
}

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
/**
    This callback function is called when a PCI I/O Protocol is
    installed.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval EFI_SUCCESS
**/
VOID GetStorageLSIAllDevicePath (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context )
{
	EFI_STATUS                  Status;
	EFI_HANDLE                  Handle;
	UINTN                       BufferSize = 20 * sizeof(EFI_HANDLE);
	UINT32                      Count = 0x1;
	UINTN                       PciSeg, PciBus, PciDev, PciFun;
	EFI_PCI_IO_PROTOCOL         *PciIo;
	UINT16					    VendorId;
	EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
	EFI_DEVICE_PATH_PROTOCOL    *CurrentDevPath;
	UINTN                       RootbridgeDevPathLength;

	Status = pBS->LocateHandle( ByRegisterNotify, \
			NULL, \
			gLSINotifyReg, \
			&BufferSize, \
			&Handle );
	ASSERT_EFI_ERROR(Status);
	if (EFI_ERROR(Status)) return;
	//
	// Locate PciIo protocol installed on Handle
	//
	Status = pBS->HandleProtocol( Handle, &gEfiPciIoProtocolGuid, &PciIo );
	ASSERT_EFI_ERROR(Status);
	if (EFI_ERROR(Status)) return;
	
	// Get PCI Device Bus/Device/Function Numbers
	Status = PciIo->GetLocation( PciIo, &PciSeg, &PciBus, &PciDev, &PciFun );
	ASSERT_EFI_ERROR(Status);
	if (EFI_ERROR(Status)) return;
	
	if (PciBus != 0){
		DEBUG((-1, "Current Device SegNum :  %x, BusNum :  %x, DevNum :  %x, FunNum :  %x\n",PciSeg, PciBus, PciDev, PciFun));
		
		//Check if it is LSI's VendorID 0x1000
		Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 0, 1, &VendorId);
		if ((!EFI_ERROR(Status)) && (VendorId == LSI_VENDOR_ID)){
			
			// PciPcieAddr Structure for one LSI Device :  (_1_) (_2_) ... (_N_) 
			// (1) Device Path Length
			// (2) LSI Device (Bus/Dev/Fun)
			//   :
			// (N) Parent Bridge (Bus/Dev/Fun)
			*(PciPcieAddr + (LSIDeviceLength +Count)) =(UINT32) ((PciBus << 16) + (PciDev << 8) + PciFun);
			
			Status = pBS->HandleProtocol (Handle, &gEfiDevicePathProtocolGuid, &DevicePath);
		
			//Find handle of the second bridge or more bridge from Device Path and Get its path (Bus Dev Fun)
			RootbridgeDevPathLength = sizeof(ACPI_HID_DEVICE_PATH) + sizeof(PCI_DEVICE_PATH) + sizeof(EFI_DEVICE_PATH_PROTOCOL);
			while (DPLength(DevicePath) > RootbridgeDevPathLength)
			{
				DevicePath = DPCut(DevicePath);
				CurrentDevPath = DevicePath;
			
				Status = pBS->LocateDevicePath (
					&gEfiPciIoProtocolGuid,
					&CurrentDevPath,
					&Handle );
				if (!EFI_ERROR (Status)){
					Count++;
					Status = pBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
					ASSERT_EFI_ERROR(Status);
					Status = PciIo->GetLocation(PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
					ASSERT_EFI_ERROR(Status);
					DEBUG((-1, "%xth BusNum : %x DevNum : %x FunNum : %x\n", (Count-1), PciBus, PciDev, PciFun));
					*(PciPcieAddr + (LSIDeviceLength + Count)) =(UINT32) ((PciBus << 16) + (PciDev << 8) + PciFun);
					DEBUG((-1, "PciPcieAddr[%x] : %x\n", Count, *(PciPcieAddr + (LSIDeviceLength + Count))));
				}
			}
			*(PciPcieAddr + LSIDeviceLength) = ++Count;
			DEBUG((-1, "LSIDeviceLength : %x Count : %x\n", LSIDeviceLength, Count));
			LSIDeviceLength = LSIDeviceLength + *(PciPcieAddr + LSIDeviceLength);
			DEBUG((-1, "Total LSIDeviceLength : %x\n", LSIDeviceLength));
		}
	}
}
#endif

/**
    This function can initialize any SB registers before legacy
    OS booting.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID 
EFIAPI
InitSbRegsBeforeLagecyBoot (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    pBS->CloseEvent(Event);
}

/**
    This function can disable USB PerPort before OS booting.             

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
VOID USBPerPortDisableCallback (
    IN EFI_EVENT    Event,
    IN VOID *Context
)
{
    PCH_SERIES                  PchSeries = GetPchSeries();
    UINTN                       PchMaxUsbPorts = PCH_MAX_USB2_PORTS + PCH_MAX_USB3_PORTS;
    UINTN                       PchLpMaxUsbPorts = PCH_LP_XHCI_MAX_USB3_PORTS + PCH_LP_XHCI_MAX_USB2_PORTS;
    UINT8                       XhciPortDisableFlag[PCH_MAX_USB2_PORTS + PCH_MAX_USB3_PORTS];
    UINT32                      Index;
    UINT64                      XhciBar;
    static BOOLEAN              USBPerPortDisableDone = FALSE;

    //Make sure the processing is performed only once. 
    if (USBPerPortDisableDone){
        pBS->CloseEvent(Event);
        return;
    }

    DEBUG((DEBUG_INFO, "OEM_USB_PER_PORT_DISABLE_SUPPORT - Start\n"));
    GetSbSetupData( pRS, gSbSetupData, FALSE );
    
    // Read back Xhci MMIO addrss
    if ((MmioRead32(SB_XHCI_MMIO_REG(R_SB_XHCI_MEM_BASE)) & 0x6) == 0x4){
        XhciBar = (((UINT64) MmioRead32(SB_XHCI_MMIO_REG((R_SB_XHCI_MEM_BASE + 4))) << 32) | 
                   ((UINT64) MmioRead32(SB_XHCI_MMIO_REG(R_SB_XHCI_MEM_BASE)) & (~0x0F)));
    } else {
        XhciBar = MMIO_READ32(SB_XHCI_MMIO_REG(R_SB_XHCI_MEM_BASE)) & (~0x0F);
    }
    DEBUG((DEBUG_INFO, "Xhci Bar = %x\n", XhciBar));
    
    // Initial Xhci Port Disable Flag
    // for PCH-LP
    // Index: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
    // SS/HS: HS HS HS HS HS HS HS HS HS HS HS HS SS SS SS SS SS SS
    // for PCH-H
    // SS/HS: HS HS HS HS HS HS HS HS HS HS HS HS HS HS HS HS SS SS SS SS SS SS SS SS SS SS
    for (Index=0;Index<PchMaxUsbPorts;Index++) XhciPortDisableFlag[Index] = 0;

    for (Index = 0; Index < GetSbUsbMaxPhysicalPortNum (); Index++) {
        if (gSbSetupData->PchUsbHsPort[Index] != 1) {
            XhciPortDisableFlag[Index] |= 1;
        }
    }

// Disable usb port under Xhci controller >>>
{
    UINT32                      counter;
    UINT32                      RegVal;
    
    // Disable Xhci port which are disconnected
    for(Index=0; Index<PchMaxUsbPorts; Index++){
        if (XhciPortDisableFlag[Index] != 0){
            DEBUG((DEBUG_INFO, "Disable port%d under Xhci controller(start number:1)\n", (Index + 1)));
            if (PchSeries == PchLp){
                // for PCH-LP
                // Index: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
                // SS/HS: HS HS HS HS HS HS HS HS HS HS HS HS SS SS SS SS SS SS
                DEBUG((DEBUG_INFO, "this PCH is ULT sku\n"));
                if (Index > PchLpMaxUsbPorts) break;
                
                if (Index < PCH_LP_XHCI_MAX_USB2_PORTS){
                    //HS port
                    RegVal = MmioRead32(XhciBar + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Index);
                    if (RegVal & B_PCH_XHCI_PORTSCXUSB2_PED){
                        //RegVal = (B_PCH_XHCI_PORTSCXUSB2_PED | B_PCH_XHCI_PORTSCXUSB2_PP);    //(EIP242242-)
                        RegVal = (B_PCH_XHCI_PORTSCXUSB2_PR | B_PCH_XHCI_PORTSCXUSB2_PP);       //(EIP242242+)
                        MmioWrite32((XhciBar + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Index), RegVal);
                        for(counter=0;counter<200;counter++){
                            RegVal = MmioRead32(XhciBar + R_PCH_LP_XHCI_PORTSC01USB2 + 0x10 * Index);
                            if(~(RegVal & BIT0)) break;  // Connect status bit 0 = 0
                            //if(!(RegVal & B_PCH_XHCI_PORTSCXUSB2_PED)) break; //(EIP242242-)
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB2_PRC) break;   //(EIP242242+)
                            pBS->Stall(100);
                        } // for loop
                    } // PED bit is enable
                } else {
                    //SS port
                    RegVal = MmioRead32(XhciBar + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * (Index - 12));
                    if (RegVal & B_PCH_XHCI_PORTSCXUSB3_PED){
                        RegVal = (B_PCH_XHCI_PORTSCXUSB3_WPR | B_PCH_XHCI_PORTSCXUSB3_PP);
                        MmioWrite32((XhciBar + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * (Index - 12)), RegVal);
                        for(counter=0;counter<3000;counter++){
                            RegVal = MmioRead32(XhciBar + R_PCH_LP_XHCI_PORTSC01USB3 + 0x10 * (Index - 12));
                            if(~(RegVal & BIT0)) break;  // Connect status bit 0 = 0
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB3_WRC) break;
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB3_PRC) break;
                            pBS->Stall(100);
                        } // for loop
                        
                        // Clear Warm Port Reset Change and Port Reset Change bits
                        //RegVal = (B_PCH_XHCI_PORTSCXUSB3_WRC | B_PCH_XHCI_PORTSCXUSB3_PRC | B_PCH_XHCI_PORTSCXUSB3_PP);
                        //MmioWrite32((XhciBar + R_PCH_LP_XHCI_PORTSC1USB3 + 0x10 * Index), RegVal);
                    } // PED bit is enable
                }// SS/HS port
            } else {
                // for PCH-H
                // Index: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
                // SS/HS: HS HS HS HS HS HS HS HS HS HS HS HS HS HS HS HS SS SS SS SS SS SS SS SS SS SS
                DEBUG((DEBUG_INFO, "this PCH is MB/DT sku\n"));
                if (Index < PCH_H_XHCI_MAX_USB2_PORTS){
                    //HS port
                    RegVal = MmioRead32(XhciBar + R_PCH_H_XHCI_PORTSC01USB2 + 0x10 * Index);
                    if (RegVal & B_PCH_XHCI_PORTSCXUSB2_PED){
                        //RegVal = (B_PCH_XHCI_PORTSCXUSB2_PED | B_PCH_XHCI_PORTSCXUSB2_PP);    //(EIP242242-)
                        RegVal = (B_PCH_XHCI_PORTSCXUSB2_PR | B_PCH_XHCI_PORTSCXUSB2_PP);       //(EIP242242+)
                        MmioWrite32((XhciBar + R_PCH_H_XHCI_PORTSC01USB2 + 0x10 * Index), RegVal);
                        for(counter=0;counter<200;counter++){
                            RegVal = MmioRead32(XhciBar + R_PCH_H_XHCI_PORTSC01USB2 + 0x10 * Index);
                            if(~(RegVal & BIT0)) break;  // Connect status bit 0 = 0
                            //if(!(RegVal & B_PCH_XHCI_PORTSCXUSB2_PED)) break; //(EIP242242-)
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB2_PRC) break;   //(EIP242242+)
                            pBS->Stall(100);
                        } // for loop
                    } // PED bit is enable
                } else {
                    //SS port
                    RegVal = MmioRead32(XhciBar + R_PCH_H_XHCI_PORTSC01USB3 + 0x10 * (Index - 16));
                    if (RegVal & B_PCH_XHCI_PORTSCXUSB3_PED){
                        RegVal = (B_PCH_XHCI_PORTSCXUSB3_WPR | B_PCH_XHCI_PORTSCXUSB3_PP);
                        MmioWrite32((XhciBar + R_PCH_H_XHCI_PORTSC01USB3 + 0x10 * (Index - 16)), RegVal);
                        for(counter=0;counter<3000;counter++){
                            RegVal = MmioRead32(XhciBar + R_PCH_H_XHCI_PORTSC01USB3 + 0x10 * (Index - 16));
                            if(~(RegVal & BIT0)) break;  // Connect status bit 0 = 0
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB3_WRC) break;
                            if(RegVal & B_PCH_XHCI_PORTSCXUSB3_PRC) break;
                            pBS->Stall(100);
                        } // for loop
                        
                        // Clear Warm Port Reset Change and Port Reset Change bits
                        //RegVal = (B_PCH_XHCI_PORTSCXUSB3_WRC | B_PCH_XHCI_PORTSCXUSB3_PRC | B_PCH_XHCI_PORTSCXUSB3_PP);
                        //MmioWrite32((XhciBar + R_PCH_H_XHCI_PORTSC1USB3 + 0x10 * Index), RegVal);
                    } // PED bit is enable
                } // SS/HS port
            } // PCH sku
        } // XhciPortDisableFlag[counter] != 0
    } // for loop
}
// Disable usb port under Xhci controller <<<

    USBPerPortDisableDone = TRUE;
    DEBUG((DEBUG_INFO, "OEM_USB_PER_PORT_DISABLE_SUPPORT - End\n"));
    pBS->CloseEvent(Event);
}
#endif

/**
    This callback function is called after Setup NVRAM variable
    being updated.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID 
EFIAPI
SbSetupNvramUpdatedCallback (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context )
{
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)    
    EFI_STATUS          Status;
    SETUP_DATA          *SetupData = NULL;
    EFI_GUID            SetupGuid  = SETUP_GUID;
    UINT32              Attributes = 0;
    UINTN               VarSize = 0;
    
    Status = GetEfiVariable(L"Setup", &SetupGuid, &Attributes, &VarSize, (VOID **)&SetupData);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }

    if ((SetupData->CsmSupport == 0) && (SetupData->SataLegacyOrom == 1)) {
      DEBUG((DEBUG_INFO, "SB: Set SataLegacyOrom to 0 if CsmSupport is 0.\n"));        
      SetupData->SataLegacyOrom = 0;
      Status = pRS->SetVariable (L"Setup", &SetupGuid, Attributes, sizeof(SETUP_DATA), SetupData);
      if (EFI_ERROR(Status))
      {
          ASSERT_EFI_ERROR(Status);
      }
    }
#endif    
    
    pBS->CloseEvent(Event);
}

//----------------------------------------------------------------------------
// USUALLY NO PORTING REQUIRED FOR THE FOLLOWING ROUTINES
//----------------------------------------------------------------------------

/**
    This function resets warm reset variable.

    @param VOID

    @retval VOID
**/

VOID ClearWarmResetFlag (VOID)
{
    EFI_STATUS          Status;
    EFI_GUID            SbWarmResetGuid = SB_WARM_RESET_GUID;
    CHAR16              SbWarmResetVar[] = SB_WARM_RESET_VARIABLE;
    UINT32              SbWarmResetFlag = 0;
    UINTN               VarSize = sizeof(SbWarmResetFlag);
    static EFI_GUID     guidHob = HOB_LIST_GUID;
    EFI_HOB_HANDOFF_INFO_TABLE *pHit;

    Status = pRS->GetVariable( SbWarmResetVar, \
                               &SbWarmResetGuid, \
                               NULL, \
                               &VarSize, \
                               &SbWarmResetFlag );
    if ((!EFI_ERROR(Status)) && (SbWarmResetFlag == SB_WARM_RESET_TAG)) {
        SbWarmResetFlag ^= 0xffffffff;
        Status = pRS->SetVariable( SbWarmResetVar, \
                                   &SbWarmResetGuid, \
                                   EFI_VARIABLE_NON_VOLATILE | \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                                   EFI_VARIABLE_RUNTIME_ACCESS, \
                                   0, \
                                   &SbWarmResetFlag );

        //Get Boot Mode
        pHit = GetEfiConfigurationTable(pST, &guidHob);
        if (pHit && (pHit->BootMode == BOOT_WITH_FULL_CONFIGURATION)) {
            // Update Boot mode for ME.
            pHit->BootMode = BOOT_ASSUMING_NO_CONFIGURATION_CHANGES;
        }    
    }
}

/**
    This function reports DXE_SB_ERROR code to system during SB
    DXE initialzation if needed.

    @param Status - EFI status.

    @retval VOID
**/

VOID ReportSBDxeError (
    IN EFI_STATUS       Status )
{
    if (Status != EFI_SUCCESS) {
        // Report Error code
        ERROR_CODE (DXE_SB_ERROR, EFI_ERROR_MAJOR);
        ASSERT_EFI_ERROR(Status);
    }
}

/**
    This function is called when the watchdog timer event is 
    signalled.  It calls the registered handler and then
    resets the system

 	@param Event   - Watchdog event
    @param Context - Context pointer

    @retval VOID

**/

VOID EFIAPI WatchdogHandler (
    IN EFI_EVENT            Event,
    IN VOID                 *Context )
{
    // Call the registered handler if there is one
    if (mWatchdogNotifyFunction != NULL) {
        mWatchdogNotifyFunction (mWatchdogPeriod);
    }

    // Reset the system
    pRS->ResetSystem( EfiResetCold, EFI_TIMEOUT, 0, NULL );
}


/**
    This function registers a handler that is called when the
    Timer event has been signalled

    @param *This          - Pointer to the instance of the Architectural
        Protocol
    @param NotifyFunction - The function to call when the interrupt fires

        
    @retval EFI_SUCCESS When new handle is registered
    @retval EFI_ALREADY_STARTED If notify function is already
        defined
    @retval EFI_INVALID_PARAMETER If notify function is NULL
**/

EFI_STATUS EFIAPI RegisterHandler (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN EFI_WATCHDOG_TIMER_NOTIFY         NotifyFunction )
{
    // Only register the handler if it is still NULL
    if (NotifyFunction && mWatchdogNotifyFunction)
        return EFI_ALREADY_STARTED;
    if (!NotifyFunction && !mWatchdogNotifyFunction)
        return EFI_INVALID_PARAMETER;

    mWatchdogNotifyFunction = NotifyFunction;

    return EFI_SUCCESS;
}

/**
    This function sets the timer period before the watchdog goes
    off every TimerPeriod number of 100ns intervals, if the 
    period is set to 0 the timer event is cancelled

    @param *This       - Pointer to the instance of the Architectural
        Protocol
    @param TimerPeriod - The number of 100ns intervals to which the
        watchdog will be programmed.

        
    @retval EFI_SUCCESS The event has been set to be
        signaled at the requested time.
    @retval EFI_INVALID_PARAMETER WatchdogEvent or TimerDelayType
        is not valid.
**/

EFI_STATUS EFIAPI WatchdogSetTimerPeriod (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN UINT64                            TimerPeriod )
{
    EFI_TIMER_DELAY     TimerDelayType;

    // Store new timer length
    mWatchdogPeriod = TimerPeriod;

    // Cancel timer event if Timer Period is 0
    TimerDelayType = (TimerPeriod) ? TimerRelative : TimerCancel;

    // Set the timer for the event
    return pBS->SetTimer( mWatchdogEvent, TimerDelayType, mWatchdogPeriod );
}

/**
    This function returns the current watchdog timer period

    @param *This        - Pointer to the instance of the Architectural
        Protocol
    @param *TimerPeriod - Pointer to a memory location to load the
        current Timer period into

    @retval *TimerPeriod - Current Timer Period if function returns
        EFI_SUCCESS
**/

EFI_STATUS EFIAPI WatchdogGetTimerPeriod (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN UINT64                            *TimerPeriod )
{
    // return the current Watchdog period
    *TimerPeriod = mWatchdogPeriod;

    return EFI_SUCCESS;
}

/**
    This function installs the the Watchdog Timer protocol on its
    handle, and initializes the Watchdog timer.

    @param ImageHandle - ImageHandle of the loaded driver
    @param SystemTable - Pointer to the System Table

        
    @retval EFI_SUCCESS The Watchdog Timer protocol was
        installed.
    @retval EFI_OUT_OF_RESOURCES Space for a new handle could not
        be allocated.
    @retval EFI_INVALID_PARAMETER One of the parameters has an 
        invalid value.
**/

EFI_STATUS WatchdogInit (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable )
{
    EFI_STATUS  Status;

    // Use the Timer event to trigger the Watchdog.  No specific hardware
    // exists for this
    Status = pBS->CreateEvent( EVT_TIMER | EVT_NOTIFY_SIGNAL, \
                               TPL_NOTIFY, \
                               WatchdogHandler, \
                               NULL, \
                               &mWatchdogEvent );

    // Create a handle for the ArchProtocol and install Watchdog Arch
    // Protocol on the handle
    Status = pBS->InstallProtocolInterface( &mWatchdogHandle, \
                                            &gWatchdogGuid, \
                                            EFI_NATIVE_INTERFACE, \
                                            &mWatchdog );

    return Status;
}

#if defined HPET_PROTOCOL_SUPPORT && HPET_PROTOCOL_SUPPORT == 1
VOID TimerInit (VOID)
{
    UINT8       Value;
    
    // Init default for Timer 1
	IoWrite8 (LEGACY_TIMER_CTRL, 0x36);
	IoWrite8 (LEGACY_TIMER_0_COUNT, 0);
	IoWrite8 (LEGACY_TIMER_0_COUNT, 0);
    // Add boot script programming
    Value = 0x36;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO (gBootScript, EfiBootScriptWidthUint8, LEGACY_TIMER_CTRL, 1, &Value);
    Value = 0x0;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO (gBootScript, EfiBootScriptWidthUint8, LEGACY_TIMER_0_COUNT, 1, &Value);
    BOOT_SCRIPT_S3_IO_WRITE_MACRO (gBootScript, EfiBootScriptWidthUint8, LEGACY_TIMER_0_COUNT, 1, &Value);
    
    //Program Timer1 to pass certain customer's test
    IoWrite8 (LEGACY_TIMER_CTRL, 0x54);
    IoWrite8 (LEGACY_TIMER_1_COUNT, 0x12);
    //add boot script programming
    Value = 0x54;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO (gBootScript, EfiBootScriptWidthUint8, LEGACY_TIMER_CTRL, 1, &Value);
    Value = 0x12;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO (gBootScript, EfiBootScriptWidthUint8, LEGACY_TIMER_1_COUNT, 1, &Value);
}
#endif

/**
    Program the Irq Mask and Trigger.

    @param VOID

    @retval VOID

    @note  Here is the control flow of this function:
              1. Program Master Irq Mask.
              2. Program Slave Irq Mask.
              3. Program Trigger Level.
**/

VOID ProgramIrqMaskTrigger (VOID)
{
    IoWrite8(LEGACY_8259_MASK_REGISTER_MASTER, (UINT8)gIrqMask[gMode]);
    IoWrite8(LEGACY_8259_MASK_REGISTER_SLAVE, (UINT8)(gIrqMask[gMode] >> 8));

    // 4d0 can not be accessed as by IoWrite16, we have to split
    IoWrite8(LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_MASTER, \
                                            (UINT8)gIrqTrigger[gMode]);
    IoWrite8(LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_SLAVE, \
                                            (UINT8)(gIrqTrigger[gMode] >> 8));
}

/**
    Initializes the interrupt controller and program the Irq 
    Master and Slave Vector Base.

    @param *This      - Pointer to this object
    @param MasterBase - IRQ base for the master 8259 controller
    @param SlaveBase  - IRQ base for the slave 8259 controller

    @retval EFI_SUCCESS Interrupt on the interrupt controllers was
        enabled.

    @note  Here is the control flow of this function:
              1. If Master base is changed, initialize master 8259 setting 
                  the interrupt offset.
              2. If Slave base is changed, initialize slave 8259 setting 
                  the interrupt offset.
              3. Return EFI_SUCCESS.
**/

EFI_STATUS SetVectorBase (
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN UINT8                    MasterBase,
    IN UINT8                    SlaveBase )
{
    // 8259 Master
    if (MasterBase != gMasterBase) {
        // Start 8259 Master Initialization. 
        IoWrite8(LEGACY_8259_CONTROL_REGISTER_MASTER, ICW1); // 0x20
        // Set Interrupt Offset 
        IoWrite8(LEGACY_8259_MASK_REGISTER_MASTER, MasterBase); // 0x21
        // Set Slave IRQ. 
        IoWrite8(LEGACY_8259_MASK_REGISTER_MASTER, ICW3_M); // 0x21
        // Set 8259 mode. See ICW4 comments with #define. 
        IoWrite8(LEGACY_8259_MASK_REGISTER_MASTER, ICW4); // 0x21
        gMasterBase = MasterBase;
    }

    // 8259 Slave
    if (SlaveBase != gSlaveBase) {
        // Start 8259 Slave  Initialization. 
        IoWrite8(LEGACY_8259_CONTROL_REGISTER_SLAVE, ICW1); // 0xA0
        // Set Interrupt Offset 
        IoWrite8(LEGACY_8259_MASK_REGISTER_SLAVE, SlaveBase); // 0xA1
        // Set Slave IRQ. 
        IoWrite8(LEGACY_8259_MASK_REGISTER_SLAVE, ICW3_S); // 0xA1
        // Set 8259 mode. See ICW4 comments with #define. 
        IoWrite8(LEGACY_8259_MASK_REGISTER_SLAVE, ICW4); // 0xA1
        gSlaveBase = SlaveBase;
    }

    return EFI_SUCCESS;
}

/**
    Get the Master/Slave Irq Mask, Irq Level for Legacy real 
    mode and protected mode.

    @param *This               - Pointer to this object
    @param *LegacyMask         - Legacy mode interrupt mask
    @param *LegacyEdgeLevel    - Legacy mode edge/level trigger value
    @param *ProtectedMask      - Protected mode interrupt mask
    @param *ProtectedEdgeLevel - Protected mode edge/level trigger value

    @retval EFI_SUCCESS Returned irq mask/level.

    @note  Here is the control flow of this function:
              1. If *LegacyMask not NULL, get legacy Mask.
              2. If *LegacyEdgeLevel not NULL, get legacy Trigger Level.
              3. If *ProtectedMask not NULL, get protected Mask.
              4. If *ProtectedEdgeLevel not NULL, get protected trigger 
                 level.
              5. Return EFI_SUCCESS.
**/

EFI_STATUS GetMask (
    IN EFI_LEGACY_8259_PROTOCOL *This,
    OUT UINT16                  *LegacyMask         OPTIONAL,
    OUT UINT16                  *LegacyEdgeLevel    OPTIONAL,
    OUT UINT16                  *ProtectedMask      OPTIONAL,
    OUT UINT16                  *ProtectedEdgeLevel OPTIONAL )
{
    if (LegacyMask)         *LegacyMask         = gIrqMask[0];
    if (LegacyEdgeLevel)    *LegacyEdgeLevel    = gIrqTrigger[0];
    if (ProtectedMask)      *ProtectedMask      = gIrqMask[1];
    if (ProtectedEdgeLevel) *ProtectedEdgeLevel = gIrqTrigger[1];

    return EFI_SUCCESS;
}

/**
    Set the Master/Slave Irq Mask, Irq Level for Legacy real mode
    and protected mode.

    @param *This               - Pointer to this object
    @param *LegacyMask         - Legacy mode interrupt mask
    @param *LegacyEdgeLevel    - Legacy mode edge/level trigger value
    @param *ProtectedMask      - Protected mode interrupt mask
    @param *ProtectedEdgeLevel - Protected mode edge/level trigger value

    @retval EFI_SUCCESS Set irq mask/level.

    @note  Here is the control flow of this function:
              1. If *LegacyMask not NULL, set legacy mask variable.
              2. If *LegacyEdgeLevel not NULL, set legacy Trigger Level
                 variable.
              3. If *ProtectedMask not NULL, set protected mask variable.
              4. If *ProtectedEdgeLevel not NULL, set protected trigger 
                 level variable.
              5. Call function to program 8259 with mask/trigger of
                 current mode.
              6. Return EFI_SUCCESS.
**/

EFI_STATUS SetMask (
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN UINT16                   *LegacyMask         OPTIONAL,
    IN UINT16                   *LegacyEdgeLevel    OPTIONAL,
    IN UINT16                   *ProtectedMask      OPTIONAL,
    IN UINT16                   *ProtectedEdgeLevel OPTIONAL )
{
    if (LegacyMask)         gIrqMask[0]    = *LegacyMask;
    if (LegacyEdgeLevel)    gIrqTrigger[0] = *LegacyEdgeLevel;
    if (ProtectedMask)      gIrqMask[1]    = *ProtectedMask;
    if (ProtectedEdgeLevel) gIrqTrigger[1] = *ProtectedEdgeLevel;

    ProgramIrqMaskTrigger();

    return EFI_SUCCESS;
}

/**
    Sets the interrupt mode operation to legacy or protected. 
    New mask and edge/level status can be provided as input

    @param *This      - Pointer to this object
    @param Mode       - Interrupt mode setting
    @param *Mask      - New interrupt mask for this mode
    @param *EdgeLevel - New edge/level trigger value for this mode

    @retval EFI_SUCCESS Set mode was successful

    @note  Here is the control flow of this function:
              1. If invalid mode, return EFI_INVALID_PARAMETER.
              2. If *Mask not NULL, set mode mask variable.
              3. If *EdgeLevel not NULL, set mode trigger level variable.
              4. Call function to program 8259 with mask/trigger of
                 current mode.
              5. Return EFI_SUCCESS.
**/

EFI_STATUS SetMode (
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_MODE            Mode,
    IN UINT16                   *Mask       OPTIONAL,
    IN UINT16                   *EdgeLevel  OPTIONAL )
{
    if (Mode >= Efi8259MaxMode) return EFI_INVALID_PARAMETER;

    gMode = Mode;
    if (Mask) gIrqMask[Mode] = *Mask;
    if (EdgeLevel) gIrqTrigger[Mode] = *EdgeLevel;

    ProgramIrqMaskTrigger();

    return EFI_SUCCESS;
}

/**
    Returns the vector number for the requested IRQ

    @param This   - Legacy8259 protocol object
    @param Irq    - IRQ number for which vector is needed
    @param Vector - Vector value is returned in this pointer

        
    @retval EFI_INVALID_PARAMETER Invalid IRQ.
    @retval EFI_SUCCESS Get Irq Vector for IRQ.

    @note  Here is the control flow of this function:
              1. If invalid IRQ, return EFI_INVALID_PARAMETER.
              2. If Set *Vector to Irq base + interrupt offset.
              3. Return EFI_SUCCESS.
**/

EFI_STATUS GetVector (
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_IRQ             Irq,
    OUT UINT8                   *Vector )
{
    if ((UINT8)Irq >= (UINT8)Efi8259IrqMax) return EFI_INVALID_PARAMETER;
    *Vector = (Irq <= Efi8259Irq7) ? gMasterBase + Irq : gSlaveBase + Irq - 8;

    return EFI_SUCCESS;
}

/**
    Enable the Interrupt controllers to respond in a specific IRQ.

    @param This           - Legacy8259 protocol object
    @param Irq            - IRQ that has to be enabled
    @param LevelTriggered - Trigger mechanism (level or edge) for this
        IRQ

        
    @retval EFI_SUCCESS Interrupt on the interrupt 
        controllers was enabled.
    @retval EFI_INVALID_PARAMETER Interrupt not existent. 
        The 8259 master/slave supports
        IRQ 0-15.

    @note  Here is the control flow of this function:
              1. Check if IRQ is valid. If not, return EFI_INVALID_PARAMETER
              2. Clear interrupt mask bit in variable of current mode.
              3. Set/Clear bit of trigger level variable of current mode.
              4. Program mask/trigger.
              5. Return EFI_SUCCESS.
**/

EFI_STATUS EnableIrq (
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_IRQ             Irq,
    IN BOOLEAN                  LevelTriggered )
{
    if ((UINT8)Irq > (UINT8)15) return EFI_INVALID_PARAMETER;

    gIrqMask[gMode] &= (UINT16)(~(1 << Irq)); // Clear mask for the Irq.

    gIrqTrigger[gMode] &= (UINT16)(~(1 << Irq)); // Mask Irq to change.

    // Change irq bit, 0 = edge, 1 = level. 
    if (LevelTriggered) gIrqTrigger[gMode] |= (1 << Irq);

    ProgramIrqMaskTrigger();

    return EFI_SUCCESS;
}

/**
    Disable the Interrupt controllers to stop responding to 
    a specific IRQ.

    @param This - Legacy8259 protocol object
    @param Irq  - IRQ that has to be disabled

        
    @retval EFI_SUCCESS Interrupt on the interrupt
        controllers was enabled.
    @retval EFI_INVALID_PARAMETER Interrupt not existent. 
        The 8259 master/slave supports
        IRQ 0-15.

    @note  Here is the control flow of this function:
              1. Check if IRQ is valid. If not, return EFI_INVALID_PARAMETER
              2. Set interrupt mask bit in variable of current mode.
              3. Program mask/trigger.
              4. Return EFI_SUCCESS.
**/

EFI_STATUS DisableIrq (
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_IRQ             Irq )
{
    if ((UINT8)Irq > (UINT8)15) return EFI_INVALID_PARAMETER;

    gIrqMask[gMode] |= (1 << Irq);  // Set mask for the IRQ.

    ProgramIrqMaskTrigger();

    return EFI_SUCCESS;
}

/**
    Get IRQ vector asigned to PCI card.

    @param This      - Pointer to this object
    @param PciHandle - PCI handle for this device
    @param Vector    - Interrupt vector this device

        
    @retval EFI_SUCCESS Vector returned.
    @retval EFI_INVALID_PARAMETER Invalid PciHandle.

    @note  Here is the control flow of this function:
              1. Get Handle of PciIo installed on PciHandle.
              2. If PciIo not installed, return EFI_INVALID_DEVICE.
              3. Set *vector to Irq vector programmed into card.
              4. Return EFI_SUCCESS.
**/

EFI_STATUS GetInterruptLine (
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_HANDLE               PciHandle,
    OUT UINT8                   *Vector )
{
    EFI_STATUS              Status;
    EFI_PCI_IO_PROTOCOL     *PciIo;

    Status = pBS->HandleProtocol( PciHandle, \
                                  &gEfiPciIoProtocolGuid, \
                                  (VOID **)&PciIo );
    if (EFI_ERROR(Status)) return EFI_INVALID_PARAMETER;

    // Read vector from card. 
    PciIo->Pci.Read( PciIo, \
                     EfiPciIoWidthUint8, \
                     PCI_INTLINE, \
                     1, \
                     Vector );

    return EFI_SUCCESS;
}

/**
    Send end of interrupt command to interrupt controller(s).

    @param This - Pointer to this object
    @param Irq  - IRQ number for this EOI has to be sent

        
    @retval EFI_SUCCESS EOI command sent to controller(s)
    @retval EFI_INVALID_PARAMETER Interrupt not existent. The 8259
        master/slave supports IRQ 0-15.

    @note  Here is the control flow of this function:
              1. Check if IRQ is valid. If not, return EFI_INVALID_PARAMETER
              2. If Irq >= 8, then Send EOI to slave controller.
              3. Send EOI to master controller. (This is for both master /
                 slave IRQs)
              4. Return EFI_SUCCESS.
**/

EFI_STATUS EndOfInterrupt (
    IN EFI_LEGACY_8259_PROTOCOL     *This,
    IN EFI_8259_IRQ                 Irq )
{
    if (Irq > 15) return EFI_INVALID_PARAMETER;

    if (Irq >= 8) {                     // If Slave, send EOI to slave first.
        // Send Slave EOI 
        IoWrite8 (LEGACY_8259_CONTROL_REGISTER_SLAVE, EOI_COMMAND);
    }
    // Send Master EOI 
    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_MASTER, EOI_COMMAND);

    return EFI_SUCCESS;
}

EFI_LEGACY_8259_PROTOCOL gLegacy8259Protocol = {
    SetVectorBase,
    GetMask, SetMask,
    SetMode,
    GetVector,
    EnableIrq, DisableIrq,
    GetInterruptLine,
    EndOfInterrupt
};

/**
    Initialize 8259 Interrupt Controller.

    @param ImageHandle - Image handle for this driver
    @param SystemTable - Pointer to the sytem table

        
    @retval EFI_SUCCESS The legacy 8259 Protocols were
        installed.  
    @retval EFI_ALREADY_STARTED The legacy 8259 Protocol was passed
        in that is already present in the
        handle database.
    @retval EFI_OUT_OF_RESOURCES There was not enought memory in
        pool to install all the protocols.

    @note  Here is the control flow of this function:
              1. Initialize the Cpu setting vector bases.
              2. Set Cpu Mode, mask, and trigger level.
              3. Install Legacy 8259 Interface.
**/

EFI_STATUS Initialize8259 (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable )
{
    EFI_STATUS  Status;
    UINT16      Mask = 0xffff;      // Mask all interrupts.
    UINT16      EdgeLevel = 0x00;   // Set all edge.
    BOOLEAN IntState = CPULib_GetInterruptState();
 
    CPULib_DisableInterrupt();

    // Set the protected mode vectors for MASTER and SLAVE PICs
    SetVectorBase(0, MASTER_INTERRUPT_BASE, SLAVE_INTERRUPT_BASE);
    SetMode(0, Efi8259ProtectedMode, &Mask, &EdgeLevel);

    // Install the Legacy8259Protocol
    Status = pBS->InstallMultipleProtocolInterfaces( \
                                                &ImageHandle, \
                                                &gEfiLegacy8259ProtocolGuid, \
                                                &gLegacy8259Protocol, \
                                                NULL );
    if (EFI_ERROR(Status)) return Status;

    if(IntState)CPULib_EnableInterrupt();

    return EFI_SUCCESS;
}

/**
    Get the the DriverImage Handle order to Start the 
    Raid Controller handle 

    @param This -  EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL 
    @param DriverImageHandle -  Driver Image Handle 

    @retval DriverImageHandle Returns the Driver Image handle 

**/

// [ EIP405844 ] +>>>
//EFI_STATUS RaidGetDriver(
//	IN EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL *This,
//	IN OUT EFI_HANDLE *DriverImageHandle
//)
/**
  Retrieves the image handle of the platform override driver for onboard Raid controller in the system.

  @param  This                  A pointer to the EFI_PLATFORM_DRIVER_OVERRIDE_
                                PROTOCOL instance.
  @param  ControllerHandle      The device handle of the controller to check if a driver override
                                exists.
  @param  DriverImageHandle     On input, a pointer to the previous driver image handle returned
                                by GetDriver(). On output, a pointer to the next driver
                                image handle.

  @retval EFI_SUCCESS           The driver override for ControllerHandle was returned in
                                DriverImageHandle.
  @retval EFI_NOT_FOUND         A driver override for ControllerHandle was not found.
  @retval EFI_INVALID_PARAMETER The handle specified by ControllerHandle is NULL.
  @retval EFI_INVALID_PARAMETER DriverImageHandle is not a handle that was returned on a
                                previous call to GetDriver().

**/

EFI_STATUS
EFIAPI
RaidGetDriver(
    IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
    IN EFI_HANDLE                            ControllerHandle,
    IN OUT EFI_HANDLE                        *DriverImageHandle
)
{
    UINTN                       HandleCount;
    EFI_HANDLE                  *HandleBuffer = NULL;
    UINTN                       Index;
    EFI_LOADED_IMAGE_PROTOCOL   *LoadedImage;
    EFI_DRIVER_BINDING_PROTOCOL *DriverBindingProtocol=NULL;
    static UINT8                GuidCount = 0;
    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIo;
    UINTN                       PciSeg;
    UINTN                       PciBus;
    UINTN                       PciDev;
    UINTN                       PciFun;
    PCI_STD_DEVICE              Pci;

    //
    // Validate the the Input parameters
    //
    if (This == NULL || DriverImageHandle == NULL || ControllerHandle == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    Status = pBS->HandleProtocol(ControllerHandle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    
    if (EFI_ERROR(Status)) {
        return EFI_NOT_FOUND;
    }
    
    // Get PCI Device Bus/Device/Function Numbers
    Status = PciIo->GetLocation(PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return EFI_NOT_FOUND;
    }
    
    if ((PciBus != SB_SATA_BUS) || (PciDev != SB_SATA_DEV) || (PciFun != SB_SATA_FUN)) {
        return EFI_NOT_FOUND;
    }
    Status = PciIo->Pci.Read(
                            PciIo,
                            EfiPciIoWidthUint32,
                            0,
                            sizeof(Pci) / sizeof(UINT32),
                            &Pci
                            );
    // Check for Onboard Raid controller and if's it's onboard install the Guid on that Handle.
    if (EFI_ERROR(Status)) {
        return EFI_NOT_FOUND;
    }
    if (Pci.Header.ClassCode[1] != PCI_CL_MASS_STOR_SCL_RAID) {
        return EFI_NOT_FOUND;
    }
    if (Pci.Header.VendorId != V_PCH_INTEL_VENDOR_ID) {
        return EFI_NOT_FOUND;
    }

    //
    // If already Found all the images,proceed to return the data
    //
    if (!LoadedImageDone) {

        LoadedImageDone = TRUE;

        //
        // Locate all the driver binding protocols
        //
        Status = pBS->LocateHandleBuffer (
                                      ByProtocol,
                                      &gEfiDriverBindingProtocolGuid,
                                      NULL,
                                      &HandleCount,
                                      &HandleBuffer
                                      );

        if (EFI_ERROR(Status)) {
            return Status;
        }

        for (Index = 0; Index < HandleCount; Index++) {

            //
            // Get the Driver Binding Protocol Interface
            //
            Status = pBS->HandleProtocol( HandleBuffer[Index],
                                          &gEfiDriverBindingProtocolGuid,
                                          (VOID **)&DriverBindingProtocol );

            if (EFI_ERROR(Status) || DriverBindingProtocol == NULL) {
                continue;
            }

            //
            // Get the LoadedImage Protocol from ImageHandle
            //
            Status = pBS->HandleProtocol(DriverBindingProtocol->ImageHandle,
                                          &gEfiLoadedImageProtocolGuid,
                                          (VOID **)&LoadedImage);

            if (EFI_ERROR(Status)){
                continue;
            }

            //
            //Compare the File guid with driver's needs to launched first
            //
            if (guidcmp(&(((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)(LoadedImage->FilePath))->FvFileName), \
                    &GuidListCheckForRaid[GuidCount]) != 0) {
                continue;
            }

            //
            // Driver Image handle found. Add it in the Array
            //
            ImageHandleArray[GuidCount] = DriverBindingProtocol->ImageHandle;
            GuidCount++;

            //
            // Start from the begining
            //
            Index = -1;

            //
            // Check All the Guid's are found. If found break the loop
            //
            if (GuidCount >= (sizeof(GuidListCheckForRaid) / sizeof(EFI_GUID) -1)) {
                break;
            }

        }

        //
        // Free the HandleBuffer Memory.
        //
        if (HandleBuffer) {
            pBS->FreePool(HandleBuffer);
        }

    }

    if (GuidCount == 0) {
        //
        // Image handle not found
        //
        return EFI_NOT_FOUND;
    }

    //
    //If the *DriverImageHandle is NULL , return the first Imagehandle
    //
    if (*DriverImageHandle == NULL) {
        if (ImageHandleArray[0] != NULL) {
            *DriverImageHandle = ImageHandleArray[0];
            return EFI_SUCCESS;
        }
    } else {
        //
        // If *DriverImageHandle not NULL , return the next Imagehandle
        // from the available image handle list
        //
        for (Index = 0; Index < 4; Index++) {
            if (*DriverImageHandle == ImageHandleArray[Index] && (ImageHandleArray[Index + 1] != NULL)) {
                *DriverImageHandle = ImageHandleArray[Index + 1];
                return EFI_SUCCESS;
            }
// AMI_OVERRIDE_SCT002_START >>>
            if (*DriverImageHandle == ImageHandleArray[Index] && (ImageHandleArray[Index + 1] == NULL)) {
                return EFI_NOT_FOUND;
            }
// AMI_OVERRIDE_SCT002_END <<<
        }
    }

    //
    // No more Image handle found to handle the controller.
    //
// AMI_OVERRIDE_SCT002_START >>>
//    return EFI_NOT_FOUND;
    return EFI_INVALID_PARAMETER;
// AMI_OVERRIDE_SCT002_END <<<
}

/**
  Retrieves the device path of the platform override driver for Onboard Raid controller in the system.

  @param  This                  A pointer to the EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL instance.
  @param  ControllerHandle      The device handle of the controller to check if a driver override
                                exists.
  @param  DriverImagePath       On input, a pointer to the previous driver device path returned by
                                GetDriverPath(). On output, a pointer to the next driver
                                device path. Passing in a pointer to NULL will return the first
                                driver device path for ControllerHandle.

  @retval EFI_SUCCESS           The driver override for ControllerHandle was returned in
                                DriverImageHandle.
  @retval EFI_UNSUPPORTED       The operation is not supported.
  @retval EFI_NOT_FOUND         A driver override for ControllerHandle was not found.
  @retval EFI_INVALID_PARAMETER The handle specified by ControllerHandle is NULL.
  @retval EFI_INVALID_PARAMETER DriverImagePath is not a device path that was returned on a
                                previous call to GetDriverPath().

**/

EFI_STATUS
EFIAPI
RaidGetDriverPath(
    IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
    IN EFI_HANDLE                            ControllerHandle,
    IN OUT EFI_DEVICE_PATH_PROTOCOL          **DriverImagePath
)
{
    return EFI_UNSUPPORTED;
}

/**
  Used to associate a driver image handle with a device path that was returned on a prior call to the
  GetDriverPath() service. This driver image handle will then be available through the
  GetDriver() service.

  @param  This                  A pointer to the EFI_PLATFORM_DRIVER_OVERRIDE_
                                PROTOCOL instance.
  @param  ControllerHandle      The device handle of the controller.
  @param  DriverImagePath       A pointer to the driver device path that was returned in a prior
                                call to GetDriverPath().
  @param  DriverImageHandle     The driver image handle that was returned by LoadImage()
                                when the driver specified by DriverImagePath was loaded
                                into memory.

  @retval EFI_SUCCESS           The association between DriverImagePath and
                                DriverImageHandle was established for the controller specified
                                by ControllerHandle.
  @retval EFI_UNSUPPORTED       The operation is not supported.
  @retval EFI_NOT_FOUND         DriverImagePath is not a device path that was returned on a prior
                                call to GetDriverPath() for the controller specified by
                                ControllerHandle.
  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.
  @retval EFI_INVALID_PARAMETER DriverImagePath is not a valid device path.
  @retval EFI_INVALID_PARAMETER DriverImageHandle is not a valid image handle.

**/

EFI_STATUS
EFIAPI
RaidDriverLoaded(
    IN EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL *This,
    IN EFI_HANDLE                            ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL              *DriverImagePath,
    IN EFI_HANDLE                            DriverImageHandle
)
{
    return EFI_UNSUPPORTED;
}
// [ EIP405844 ] +<<<

BOOLEAN
IsDeviceSignature (
  IN UINT8 *DsdtPointer
)
{
  UINT8  *TmpPointer;
  UINT16 *HalfSignature;

  for (TmpPointer = DsdtPointer-8; TmpPointer<DsdtPointer; TmpPointer++) {
    HalfSignature = (UINT16 *) TmpPointer;
    if (*HalfSignature == 0x825B) { //AML_DEVICE_OP
      return TRUE;
    }
  }
  return FALSE;
}
/**
    Update the TRAD(PCH-H) ot ULT(PCH-LP) DSDT table

    @param DsdtTable   - The table points to DSDT table.
    @param PchSeries   - LPT-H or LPT-LP

    @retval VOID

**/
VOID
TradUltDsdtTableUpdate (
//  IN ACPI_HDR   *DsdtPtr,                         // [ EIP418295 ] Improve the DSDT table updating logic for compatible with (INT)ACPI 017.
  IN EFI_ACPI_SDT_HEADER   *DsdtPtr,
  IN PCH_SERIES            PchSeries
  )
{
  AMI_SB_SERIES  SbSeries = AmiGetSbSeries();
  UINT8       *CurrPtr;
  UINT8       *DsdtPointer;
  UINT32      *Signature;
  UINT8       HexStr[36] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H',\
                            'I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
  UINT8       ReturnVaule = 0;
  UINT8       RPFN[PCH_MAX_PCIE_ROOT_PORTS];
  UINT8       Index;
  UINTN       RpDev;
  UINTN       RpFun;
  EFI_STATUS  Status;
#if defined UPDATE_SERIAL_IO_DEVICE_ASL_OBJECT && UPDATE_SERIAL_IO_DEVICE_ASL_OBJECT
  UINT8       *TmpDsdtPointer;
  UINT16      *HalfSignature;
#endif


  for (Index = 0; Index < PCH_MAX_PCIE_ROOT_PORTS; Index++){
      RPFN[Index] = Index % 8;
      if(Index < GetPchMaxPciePortNum()){
          Status = GetPchPcieRpDevFun (Index, &RpDev, &RpFun);
          if(!EFI_ERROR(Status)){
              RPFN[Index] = (UINT8)RpFun;
              DEBUG((DEBUG_INFO, "SB: Rp%d function number: %d\n", Index, RPFN[Index]));
          }
      }
  } //  end for loop
  
  CurrPtr = (UINT8 *) DsdtPtr;
  for (DsdtPointer = CurrPtr;
       DsdtPointer < (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);
       DsdtPointer++ )
  {
      Signature = (UINT32 *) DsdtPointer;
      switch(*Signature){
      //************** PCIE Adress update for Port Swapping ****************
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '1')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if (*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[0];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '2')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[1];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '3')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[2];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '4')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[3];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '5')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[4];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '6')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[5];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '7')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[6];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '8')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[7];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '0', '9')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[8];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '0')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[9];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '1')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[10];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '2')):
              DsdtPointer = DsdtPointer + 5;
              Signature = (UINT32 *) DsdtPointer;
              if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                  DsdtPointer = DsdtPointer + 5;
                  *DsdtPointer = RPFN[11];
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '3')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[12];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '4')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[13];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '5')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[14];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '6')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[15];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '7')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[16];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '8')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[17];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '1', '9')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[18];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '2', '0')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[19];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '2', '1')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[20];
                  }
              }
              break;
          case (EFI_SIGNATURE_32 ('R', 'P', '2', '2')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[21];
                  }
              }
              break;   
          case (EFI_SIGNATURE_32 ('R', 'P', '2', '3')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[22];
                  }
              }
              break;   
          case (EFI_SIGNATURE_32 ('R', 'P', '2', '4')):
              if (SbSeries == SbH){
                  DsdtPointer = DsdtPointer + 5;
                  Signature = (UINT32 *) DsdtPointer;
                  if(*Signature == EFI_SIGNATURE_32 ('_', 'A', 'D', 'R')){
                      DsdtPointer = DsdtPointer + 5;
                      *DsdtPointer = RPFN[23];
                  }
              }
              break;   
      
#if defined UPDATE_SERIAL_IO_DEVICE_ASL_OBJECT && UPDATE_SERIAL_IO_DEVICE_ASL_OBJECT
      //************** Update Serial I/O devices ASL object ****************
          case (SIGNATURE_32('S','D','M','A')):
          case (SIGNATURE_32('I','2','C','0')):
          case (SIGNATURE_32('I','2','C','1')):
          case (SIGNATURE_32('S','P','I','0')):
          case (SIGNATURE_32('S','P','I','1')):
          case (SIGNATURE_32('U','A','0','0')):
          case (SIGNATURE_32('U','A','0','1')):
          case (SIGNATURE_32('S','D','H','C')):
               if(!IsDeviceSignature(DsdtPointer)) {
                 break;
               }

               for ( TmpDsdtPointer = DsdtPointer; \
                     TRUE; \
                     TmpDsdtPointer++){
                 HalfSignature = (UINT16 *) TmpDsdtPointer;
                 Signature = (UINT32 *) TmpDsdtPointer;
                 if (*HalfSignature == 0x825B) { //AML_DEVICE_OP
                   break;
                 }
                 if (gSbSetupData->LpssMode == 2) { // PCI mode
                   if (*Signature == SIGNATURE_32('_','H','I','D')){
                     *Signature = SIGNATURE_32('X','H','I','D');
                   }
                   if (*Signature == SIGNATURE_32('_','C','I','D')){
                     *Signature = SIGNATURE_32('X','C','I','D');
                   }
                   if (*Signature == SIGNATURE_32('_','U','I','D')){
                     *Signature = SIGNATURE_32('X','U','I','D');
                   }
                   if (*Signature == SIGNATURE_32('_','C','R','S')){
                     *Signature = SIGNATURE_32('X','C','R','S');
                   }
                 } else if (gSbSetupData->LpssMode == 1) { // ACPI mode
                   if (*Signature == SIGNATURE_32('_','A','D','R')){
                     *Signature = SIGNATURE_32('X','A','D','R');
                   }
                 }
               }
               break;
#endif
          default:
               break;
      }// end switch      
  }// end of for loop
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
