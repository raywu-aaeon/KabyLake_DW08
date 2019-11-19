//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
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
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/CougarPoint_A/Chipset/SmmChildDispatcher/SmmChildDispatch.h 4     2/09/11 4:52a Yurenlai $
//
// $Revision: 4 $
//
// $Date: 2/09/11 4:52a $
//*************************************************************************
/** @file SmmChildDispatch.h
    SMM Child dispatcher functions and data structures
    definition.

**/
//*************************************************************************

                                        // (F010411A)>
#ifndef __SMM_CHILD_DISPATCH__H__
#define __SMM_CHILD_DISPATCH__H__
#ifdef __cplusplus
extern "C" {
#endif

#include "SmiObjectDefine.h"

#include <AmiDxeLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/SmmGpiDispatch2.h>
//-#include <Protocol\SmmStandbyButtonDispatch2.h>
//-#include <Protocol\SmmPowerButtonDispatch2.h>
#include <Protocol/SmmIoTrapDispatch2.h>
#include <Protocol/SmmSwDispatch.h>
#include <Protocol/SmmSxDispatch.h>
#include <Protocol/SmmPeriodicTimerDispatch.h>
#include <Protocol/SmmUsbDispatch.h>
#include <Protocol/SmmGpiDispatch.h>
//-#include <Protocol\SmmStandbyButtonDispatch.h>
//-#include <Protocol\SmmPowerButtonDispatch.h>
#include <Protocol/SmmIoTrapDispatch.h>
#include <Protocol/SmmTcoDispatch.h>
#include <Protocol/SmmBiosWriteDispatch.h>


#define SMI_PI_0_9_SIGNATURE 'PI09'
#define SMI_PI_1_1_SIGNATURE 'PI11'

#pragma pack(push, 1)

typedef enum {
    EfiSmmSwSmi,
    EfiSmmSxSmi,
    EfiSmmPeriodicTimerSmi,
    EfiSmmSwSmiTimerSmi,
    EfiSmmUsbSmi,
    EfiSmmGpiSmi,
    EfiSmmStandbyButtonSmi,
    EfiSmmPowerButtonSmi,
    EfiSmmTcoSmi,
    EfiSmmIoTrapSmi,
    EfiSmmMaxSmi
} EFI_SMM_SMI;

typedef enum {
    SmmPeriodicTimerSmi,
    SmmSwSmiTimerSmi,
    SmmMaxTimerSmi
} EFI_SMM_TIMER_SMI;

typedef enum {
    SbSmiStandbyButtonEntry,
    SbSmiStandbyButtonExit,
    SbSmiStandbyButtonMax
} SBSMI_STANDBY_BUTTON_PHASE;

typedef struct {
    SBSMI_STANDBY_BUTTON_PHASE Phase;
} SBSMI_STANDBY_BUTTON_REGISTER_CONTEXT;

typedef enum {
    SbSmiPowerButtonEntry,
    SbSmiPowerButtonExit,
    SbSmiPowerButtonMax
} SBSMI_POWER_BUTTON_PHASE;

typedef struct {
    SBSMI_POWER_BUTTON_PHASE Phase;
} SBSMI_POWER_BUTTON_REGISTER_CONTEXT;

typedef union {
    EFI_SMM_SW_DISPATCH_CONTEXT             SwContext;
    EFI_SMM_SX_DISPATCH_CONTEXT             SxContext;
    EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT TimerContext;
    EFI_SMM_USB_DISPATCH_CONTEXT            UsbContext;
    EFI_SMM_GPI_DISPATCH_CONTEXT            GpiContext;
    EFI_SMM_TCO_DISPATCH_CONTEXT            TcoContext;
    EFI_SMM_IO_TRAP_DISPATCH_CONTEXT        IoTrapContext;
//#if PI_SPECIFICATION_VERSION >= 0x0001000A    // [ EIP427935 ]
    EFI_SMM_SW_REGISTER_CONTEXT             SwRegisterContext;
    EFI_SMM_SX_REGISTER_CONTEXT             SxRegisterContext;
    EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT TimerRegisterContext;
    EFI_SMM_USB_REGISTER_CONTEXT            UsbRegisterContext;
    EFI_SMM_GPI_REGISTER_CONTEXT            GpiRegisterContext;
    EFI_SMM_IO_TRAP_REGISTER_CONTEXT        IoTrapRegisterContext;
//#endif
    SBSMI_STANDBY_BUTTON_REGISTER_CONTEXT   SBtnContext;
    SBSMI_POWER_BUTTON_REGISTER_CONTEXT     PBtnContext;
} EFI_SMM_SMI_CONTEXT;

typedef struct
{
    UINT32 Signature;
    EFI_SMM_SMI SmiType;
    SMI_CONTEXT_TYPE ContextType;
    EFI_SMM_SMI_CONTEXT Context;
    VOID* Function;
    VOID* CommBuffer;
    UINTN CommBufferSize;
    VOID* HandleObj;
} SB_SMI_CONTEXT;
//already defined in SmmPeriodicTimerDipatch2.h(EDKII)
/*typedef struct _EFI_SMM_PERIODIC_TIMER_CONTEXT
{
    UINT64 Period;
    UINT64 SmiTickInterval;
    UINT64 ElapsedTime;
} EFI_SMM_PERIODIC_TIMER_CONTEXT;*/

#pragma pack(pop)


//---------------------- SW SMI Handler Porting hooks -----------------------

VOID        SwSmiEnable( VOID );
VOID        SwSmiDisable( VOID );
VOID        SwSmiClear( VOID );
BOOLEAN     SwSmiDetect( OUT UINT16 *Type );
UINTN       GetEAX( VOID* Smst );
EFI_STATUS GetTriggerCpu(VOID* Smst, UINTN *CpuIndex);

//---------------------- SX SMI Handler Porting hooks -----------------------

VOID        SxSmiEnable( VOID );
VOID        SxSmiDisable( VOID );
VOID        SxSmiClear( VOID );
VOID        PutToSleep( VOID );
BOOLEAN     SxSmiDetect( OUT UINT16 *Type );

//---------------- Periodic timer SMI Handler Porting hooks -----------------

EFI_STATUS TimerSmiEnable( IN EFI_SMM_TIMER_SMI Type );
EFI_STATUS TimerSmiDisable( IN EFI_SMM_TIMER_SMI Type );
EFI_STATUS TimerSmiClear( IN EFI_SMM_TIMER_SMI Type );
BOOLEAN TimerSmiDetect( IN EFI_SMM_TIMER_SMI Type );
EFI_STATUS TimerSetInterval( IN EFI_SMM_TIMER_SMI Type, IN UINT64 Interval);

//---------------------- USB SMI Handler Porting hooks ----------------------

VOID UsbSmiSet( IN UINT16 ControllerType );
VOID UsbSmiClear( IN UINT16 ControllerType );
BOOLEAN UsbSmiDetect( OUT UINT16 *Type );
UINT16 GetControllerType( OUT EFI_DEVICE_PATH_PROTOCOL *Device );
EFI_STATUS UsbVerifyRegisterContext(IN VOID *Context);
VOID UsbGetDevicePath(EFI_DEVICE_PATH_PROTOCOL **DevPath);
BOOLEAN CompareUsbDevice(VOID *Device1, VOID *Device2);

//---------------------- GPI SMI Handler Porting hooks ----------------------

VOID        GpiSmiSet( IN UINT32 GpiEnableBit );
VOID        GpiSmiReset( IN UINT32 GpiDisableBit );
VOID        GpiSmiClear( IN UINT32 GpiClearBit );
BOOLEAN     GpiSmiDetect( OUT UINT32 *Gpi );

//---------------- Standby button SMI Handler Porting hooks -----------------

VOID        SButtonSmiEnable( VOID );
VOID        SButtonSmiDisable( VOID );
VOID        SButtonSmiClear( VOID );
BOOLEAN     SButtonSmiDetect( OUT UINT16 *Type );

//---------------- Power button SMI Handler Porting hooks -------------------

VOID        PButtonSmiEnable( VOID );
VOID        PButtonSmiDisable( VOID );
VOID        PButtonSmiClear( VOID );
BOOLEAN     PButtonSmiDetect( OUT UINT16 *Type );

//--------------------- TCO SMI Handler Porting hooks -----------------------

VOID        TcoSmiSet( IN UINT32 TcoBitOffset );
VOID        TcoSmiReset( IN UINT32 TcoBitOffset );
VOID        TcoSmiEnable( VOID );
VOID        TcoSmiDisable( VOID );
VOID        TcoSmiClear( VOID );
BOOLEAN     TcoSmiDetect( OUT UINT32 *TcoStatus );

//------------------ I/O Trap SMI Handler Porting hooks ---------------------

VOID IoTrapSmiSet(IN OUT UINT32 *DtatPointer);
VOID IoTrapSmiReset(IN UINT32 TrapRegIndex);
VOID IoTrapSmiClear( VOID );
BOOLEAN IoTrapSmiDetect(OUT UINT32 *IoTrapStatus, OUT UINT32 *IoTrapAddress,
                        OUT UINT32 *IoTrapOpType, OUT UINT32 *IoTrapData,
                        OUT UINT32 *IoTrapByteEnables);
VOID SelfIoTrapSmiClear(IN EFI_SMM_IO_TRAP_DISPATCH_CONTEXT *SmiIoTrapContext,
                        IN EFI_SMM_IO_TRAP_DISPATCH_CONTEXT *IoTrapContext );


//---------------------------------------------------------------------------
//                      All purpose SMI Porting hooks
//---------------------------------------------------------------------------
VOID        ClearAllSmi( VOID );

extern VOID SB_Shutdown(VOID);


#define ACPI_PATH_MACRO \
 {{ACPI_DEVICE_PATH,ACPI_DP,ACPI_DEVICE_PATH_LENGTH}, EISA_PNP_ID(0x0A03),0}

#define PCI_PATH_MACRO(Device,Function) \
 {{HARDWARE_DEVICE_PATH, HW_PCI_DP, HW_PCI_DEVICE_PATH_LENGTH}, (Function),(Device)}

#define END_PATH_MACRO \
 {END_DEVICE_PATH,END_ENTIRE_SUBTYPE,END_DEVICE_PATH_LENGTH}

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
