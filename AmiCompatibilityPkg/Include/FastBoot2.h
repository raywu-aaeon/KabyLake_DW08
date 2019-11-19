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

/** @file FastBoot2.h
 *  Fast boot functions definitions.
 */

#ifndef __FASTBOOT2__H__
#define __FASTBOOT2__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/DevicePath.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>
#include <Hob.h>
#include <Token.h>
#include <IndustryStandard/PciCodeId.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>


#define FAST_BOOT_TYPE_EFI         0x00000001    ///last boot was from UEFI device
#define FAST_BOOT_TYPE_LEGACY      0x00000000    ///last boot was from Legacy device

#define MAX_SERIAL_COUNT           20
#define BOOT_OPTION_NAME_MAX       9
#define BOOT_OPTION_LENGTH         200
#define BOOT_VAR_NAME_MAX          15
#define BOOT_ORDER_LENGTH          50
#define DEVICE_STRING_MAX          50
#define NAME_STRING_LENGTH         1024
#define MAX_COUNTER                0x10
#define IDE_CONTROLLER             0x0101
#define RAID_CONTROLLER            0x0104
#define AHCI_CONTROLLER            0x0106

#define BBS_POPUP_BOOT             0x55aa55aa
#define PORT_80                    0x80

#define COM_PORT_EISA_ID           0x50141d0
#define EFI_FILE_NAME_LENGTH       22
#define BOOT_IA32                  44     ///size in bytes of string L"EFI\\BOOT\\BOOTIA32.EFI"
#define BOOT_X64                   42     ///size in bytes of string L"EFI\\BOOT\\BOOTx64.EFI"

#define BOOT_FLOW_CONDITION_FAST_BOOT   7
#define BOOT_FLOW_CONDITION_NORMAL      0

#define PCI_VID_OFFSET             0x00
#define PCI_CLASS_CODE_OFFSET      0xB
#define PCI_SUBCLASS_CODE_OFFSET   0x0A



#if FASTBOOT_NEED_RESTART
#define FAST_BOOT_RESTART L"FastbootRestart" 
#define FAST_BOOT_RESTART_COUNT L"FastbootRestartCount"

///{39ac7cb1-789c-45f6-94f0-68873c5c1ff2}
#define FAST_BOOT_RESTART_COUNT_GUID \
    { 0x39ac7cb1, 0x789c, 0x45f6, {0x94, 0xf0, 0x68, 0x87, 0x3c, 0x5c, 0x1f, 0xf2 }}

///{800dabd7-ca8d-4611-bd77-3b200ebc275a}
#define FAST_BOOT_RESTART_GUID \
    { 0x800dabd7, 0xca8d, 0x4611, {0xbd, 0x77, 0x3b, 0x20, 0x0e, 0xbc, 0x27, 0x5a }}
#endif

/// {B540A530-6978-4da7-91CB-7207D764D262}
#define FAST_BOOT_VARIABLE_GUID \
    { 0xb540a530, 0x6978, 0x4da7, {0x91, 0xcb, 0x72, 0x7, 0xd7, 0x64, 0xd2, 0x62 }}

GUID_VARIABLE_DECLARATION(FastBootVariableGuid, FAST_BOOT_VARIABLE_GUID);

/// {50535FFD-1555-4329-A2C5-F3F44011A449}
#define AMI_SATA_PRESENT_HOB_GUID \
    {0x50535ffd, 0x1555, 0x4329, {0xa2, 0xc5, 0xf3, 0xf4, 0x40, 0x11, 0xa4, 0x49}}

#ifndef EFI_SHELL_PROTOCOL_GUID
#define EFI_SHELL_PROTOCOL_GUID \
  {0x47C7B223, 0xC42A, 0x11D2, {0x8E, 0x57, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#endif

#ifndef EFI_SHELL2_0_FILE_GUID
#define EFI_SHELL2_0_FILE_GUID  \
    { 0x6302d008, 0x7f9b, 0x4f30, 0x87, 0xac, 0x60, 0xc9, 0xfe, 0xf5, 0xda, 0x4e }
#endif

#ifndef AMI_MEDIA_DEVICE_PATH_GUID
#define AMI_MEDIA_DEVICE_PATH_GUID \
    { 0x5023b95c, 0xdb26, 0x429b, {0xa6, 0x48, 0xbd, 0x47, 0x66, 0x4c, 0x80, 0x12 }}
#endif

#if FASTBOOT_NEED_RESTART
#ifndef AMITSE_EVENT_BEFORE_BOOT_GUID
#define AMITSE_EVENT_BEFORE_BOOT_GUID \
  { 0x3677770f, 0xefb2, 0x43b2, {0xb8, 0xae, 0xb3, 0x2, 0xe9, 0x60, 0x48, 0x82 }}
#endif

#define EFI_AMI_LEGACYBOOT_PROTOCOL_GUID \
  {0x120d28aa, 0x6630, 0x46f0, {0x81, 0x57, 0xc0, 0xad, 0xc2, 0x38, 0x3b, 0xf5}};
#endif


#pragma    pack(1)
typedef struct _SATA_INFO {
    UINT16              ClassCode;      ///Base class and Sub class code
    UINT32              PresentPortBitMap;    //Upper word is BDF, Lower word is port bitmap
                                              /** Bit map description    
                                                    IDE Mode:
                                                    Bit0: Primary Master
                                                    Bit1: Secondary Master
                                                    Bit2: Primary Slave
                                                    Bit3: Secondary Slave
                                                
                                                    AHCI or RAID mode:
                                                    Bit0: Port0
                                                    Bit1: Port1
                                                    Bit2: Port2
                                                    Bit3: Port3
                                                    Bit4: Port4
                                                    Bit5: Port5
                                              **/      
} SATA_INFO;

#if SUPPORT_RAID_DRIVER
typedef struct _SATA_PRESENT_HOB {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    UINT8               ControllerCount;
    SATA_INFO           SataInfo[4];
} SATA_PRESENT_HOB;
#endif

#if SUPPORT_RAID_DRIVER
typedef struct _SATA_DEVICE_DATA{ 
    UINT16  Bdf;
    UINT16  ClassCode;
    UINT8   DevType;
    UINT8   PortNum; ///IDE MODE: bit0:channel, bit1:device
    UINT8    SerialNum[20];
} SATA_DEVICE_DATA;
#endif

typedef struct _FAST_BOOT2 {
    UINT32 BootOptionNumber : 16;
    UINT32 BootType         : 2;
    UINT32 Reserved         : 14;
    UINT32 BootCount;
    UINT16 DevStrCheckSum; 
///SATA device information
#if SUPPORT_RAID_DRIVER
    SATA_DEVICE_DATA    SataDevice[MAX_SATA_DEVICE_COUNT] ;
#endif
///SATA controller information   
    UINT8  ControllerCount;
    SATA_INFO SataInfo[4];
} FAST_BOOT2;
#pragma    pack()

///FastBoot.c

VOID 
EFIAPI 
FastBootWorker(
    VOID
);

VOID 
EFIAPI 
ResetFastBootVariable(
    VOID
);

BOOLEAN 
EFIAPI 
OemConfigurationChanged(
    VOID
);

EFI_STATUS 
EFIAPI 
CreateLegacyFastBootOption(
    FAST_BOOT2 *Var
);

EFI_STATUS 
EFIAPI 
CreateEfiFastBootOption(
    IN EFI_LOAD_OPTION *BootOption,
    IN UINTN           OptionSize,
    IN UINT16          OptionNumber
);

BOOLEAN 
EFIAPI 
IsSupportedDevice(
    IN  EFI_DEVICE_PATH_PROTOCOL *Dp,
    OUT BOOLEAN                  *UsbMassStorage,
    OUT BOOLEAN                  *HddFilePath
);

VOID 
EFIAPI 
FastBootClearBootFailFlag(
    IN EFI_EVENT Event, 
    IN VOID      *Context
);

BOOLEAN 
EFIAPI 
IsBootToShell(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp
);

VOID 
EFIAPI 
FastBootReadyToBootNotify(
    IN EFI_EVENT Event, 
    IN VOID      *Context
);

EFI_STATUS 
EFIAPI 
FastLegacyBoot(
    VOID
);

EFI_STATUS 
EFIAPI 
FastEfiBoot(
    VOID
);

EFI_STATUS 
EFIAPI 
TseHandshake();

VOID 
EFIAPI 
ReturnToNormalBoot(
    VOID
);

BOOLEAN 
EFIAPI 
IsRuntime(
    VOID
);

VOID 
EFIAPI 
FbConnectInputDevices(
    VOID
);


///FastBootTseHook.c
                                        ///(EIP63924+)>
EFI_STATUS 
EFIAPI 
FastBootCheckForKey(         
    IN  BOOLEAN *EnterSetup,
    IN  UINT32  *BootFlow);

EFI_STATUS 
EFIAPI 
FastBootStopCheckForKeyTimer();

                                        ///<(EIP63924+)
///Hook
typedef BOOLEAN (IS_VALID_FASTBOOT_BOOT_OPTION_FUNC_PTR)(
    IN  UINTN BootOrderIndex,
    IN  EFI_LOAD_OPTION *BootOption
);
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
