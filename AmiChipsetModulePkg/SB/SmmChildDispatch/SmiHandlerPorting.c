//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date: $
//**********************************************************************

/** @file SmiHandlerPorting.c
    This file contains SMM Child Dispatcher porting functions

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiSmm.h>
#include <Protocol/SmmCpu.h>
#include "SmmChildDispatch.h"

//#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A) // [ EIP427935 ]
extern EFI_SMM_CPU_PROTOCOL  *gEfiSmmCpuProtocol;
//#endif

//*************** All purpose SMI Porting hooks *************************

/**
    This function clears all SMI's

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID ClearAllSmi(VOID)
{
//Porting required
}

/**
    This function disables all SMI's

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID DisableAllSmi(VOID)
{
//Porting required
//Disable All smi, which can be caused by SouthBridge
}

//*************** SW SMI Handler Porting hooks ***************************

/**
    This function enables SW SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SwSmiEnable(VOID)
{
//Porting required
}

/**
    This function disables SW SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SwSmiDisable(VOID)
{
//Porting required
}

/**
    This function clears SW SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SwSmiClear(VOID)
{
//Porting required
}

/**
    This function detects SW SMI event

    @param UINT16 *Type - pointer to store SW SMI number

    @retval TRUE SW SMI occured, FALSE otherwise

    @note  Porting required

**/
BOOLEAN SwSmiDetect(UINT16 *Type)
{
//Porting required
//If Software SMI occured return TRUE, and value of SMI store in Type variable
    return FALSE;
}

/**
    This function returns EAX saved value from CPU that caused SW SMI

    @param VOID

    @retval EAX saved value

    @note  Porting required

**/
UINTN GetEAX(VOID)
{
//#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A) // [ EIP427935 ]
//Porting required for different CPU
    EFI_STATUS              Status;
    EFI_GUID                SwSmiCpuTriggerGuid = SW_SMI_CPU_TRIGGER_GUID;
    SW_SMI_CPU_TRIGGER      *SwSmiCpuTrigger;
    UINTN                   Cpu = pSmst->CurrentlyExecutingCpu - 1; // CPU #
    UINT16                  i;
    UINT32                  RegEAX;
    
    for (i = 0; i < pSmst->NumberOfTableEntries; i++) {
        if (guidcmp(&(pSmst->SmmConfigurationTable[i].VendorGuid), \
                                                  &SwSmiCpuTriggerGuid) == 0)
            break;
    }

    // If found table, check for the CPU that caused the software Smi.
    if (i != pSmst->NumberOfTableEntries) {
        SwSmiCpuTrigger = pSmst->SmmConfigurationTable[i].VendorTable;
        Cpu = SwSmiCpuTrigger->Cpu;
    }

    Status = gEfiSmmCpuProtocol->ReadSaveState ( \
                                            gEfiSmmCpuProtocol, \
                                            4, \
                                            EFI_SMM_SAVE_STATE_REGISTER_RAX, \
                                            Cpu, \
                                            &RegEAX );
    return RegEAX;
//#else
////Porting required for different CPU
//    UINT16                  i;
//    EFI_GUID                SwSmiCpuTriggerGuid = SW_SMI_CPU_TRIGGER_GUID;
//    SW_SMI_CPU_TRIGGER      *SwSmiCpuTrigger;
//    UINTN                   Cpu = pSmst->CurrentlyExecutingCpu - 1; //default cpu #
//    EFI_SMM_CPU_SAVE_STATE  *CpuSaveState;
//    
//    for (i = 0; i < pSmst->NumberOfTableEntries; i++) 
//    {
//        if (guidcmp(&(pSmst->SmmConfigurationTable[i].VendorGuid), &SwSmiCpuTriggerGuid) == 0)
//            break;
//    }
//
//    //If found table, check for the CPU that caused the software Smi.
//    if (i != pSmst->NumberOfTableEntries) 
//    {
//        SwSmiCpuTrigger = pSmst->SmmConfigurationTable[i].VendorTable;
//        Cpu = SwSmiCpuTrigger->Cpu;
//    }
//
//    CpuSaveState = pSmst->CpuSaveState;
//    return CpuSaveState[Cpu].Ia32SaveState.EAX;
//#endif
}

//*************** SX SMI Handler Porting hooks ***************************

/**
    This function enables SX SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SxSmiEnable(VOID)
{
//Porting required
}

/**
    This function disables SX SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SxSmiDisable(VOID)
{
//Porting required
}

/**
    This function clears SX SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SxSmiClear(VOID)
{
//Porting required
}

/**
    This function detects SX SMI event

    @param UINT16 *Type - pointer to store value of Sleep type

    @retval TRUE SX SMI occured, FALSE otherwise

    @note  Porting required

**/
BOOLEAN SxSmiDetect(UINT16 *Type)
{
//Porting required
//If Sleep SMI occured return TRUE, and value of Sleep type store in Type variable
    return FALSE;
}

/**
    Disable Smi sleep and put to sleep.

    @param *Context - Pointer to Sleep SMI context

    @retval VOID

    @note  Here is the control flow of this function:
              1. Disable Smi sleep.
              2. Set to go to sleep if you want to sleep in SMI. otherwise
                 set IORestart to 0xFF in CPU SMM dump area.

**/

VOID PutToSleep (
    IN AMI_SMM_SX_CONTEXT  *SxContext )
{
    SxSmiDisable(); // Disable sleep SMI.

    // Set to sleep. 
    IoWrite16(PM_BASE_ADDRESS + 4, IoRead16(PM_BASE_ADDRESS + 4) | 0x2000);
}

//*************** Periodic timer SMI Handler Porting hooks ***************************
UINT64 SupportedIntervals[] = {
//Porting required - put all available intervals here (in nanoseconds)

    0       //terminator record
};

/**
    This function enables Periodic timer SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID TimerSmiEnable(VOID)
{
//Porting required
}

/**
    This function disables Periodic timer SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID TimerSmiDisable(VOID)
{
//Porting required
}

/**
    This function clears Periodic timer SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID TimerSmiClear(VOID)
{
//Porting required
}

/**
    This function detects Periodic timer SMI event

    @param UINT16 *Type - added for compatibility, not used

    @retval TRUE Periodic timer SMI occured, FALSE otherwise

    @note  Porting required

**/
BOOLEAN TimerSmiDetect(UINT16 *Type)
{
//Porting required - return TRUE if Timer SMI detected, Type ignored
    return FALSE;
}

/**
    This function programs Periodic timer to given interval

    @param UINT64 Interval - interval to program

    @retval VOID

    @note  Porting required

**/
VOID TimerSetInterval(UINT64 Interval)
{
//Porting required
}

//*************** Usb SMI Handler Porting hooks ***************************

/**
    This function enables/disables USB SMI based on given Controller type

    @param UINT16 ControllerType - USB controller type variable

    @retval VOID

    @note  Porting required

**/
VOID UsbSmiSet(UINT16 ControllerType)
{
//Porting required
//This function implements logic as follows:
//Two lowest bits of ControllerType:
// 00 - both USB controllers smi are disabled
// 01 - UHCI/OHCI enabled, EHCI - disabled
// 10 - UHCI/OHCI disabled, EHCI - enabled
// 11 - both USB controllers smi are enabled
}

/**
    This function clears USB SMI based on given Controller type

    @param UINT16 ControllerType - USB controller type variable

    @retval VOID

    @note  Porting required

**/
VOID UsbSmiClear(UINT16 ControllerType)
{
//Porting required
//clear UHCI/OHCI SMI if 1, EHCI - if 2 or both if 3
}

/**
    This function detects USB SMI event

    @param UINT16 *Type - pointer to store USB controller type, source of event

    @retval TRUE USB SMI occured, FALSE otherwise

    @note  Porting required

**/
BOOLEAN UsbSmiDetect(UINT16 *Type)
{
//Porting required
//If interrupt occured Set *Type to 1 for UHCI/OHCI, 2 for EHCI or 3 for both and return TRUE
//Otherwise return FALSE and set *Type to zero
    return FALSE;
}

/**
    This function returns USB controller type, based on given device path

    @param EFI_DEVICE_PATH_PROTOCOL *Device - pointer USB device path protocol

    @retval UINT16 USB controller type

    @note  Porting required

**/
UINT16 GetControllerType(EFI_DEVICE_PATH_PROTOCOL *Device)
{
//Porting required
//return 1 for USB1.1 (UHCI or OHCI controllers) and 2 for USB2.0 (EHCI controller)
//return 0 if there are no matches - it will indicate an error
    return 0;
}

//*************** GPI SMI Handler Porting hooks ***************************

/**
    This function enables/disables GPI SMI based on given bit field

    @param UINT16 Type - GPI enabled bit field

    @retval VOID

    @note  Porting required

**/
VOID GpiSmiSet(UINT16 Type)
{
//Porting required
//All Gpis which correspondent bit in Type set to 1 should be enabled, all others - disabled
}

/**
    This function clears GPI SMI based on given bit field

    @param UINT16 Type - GPI enabled bit field

    @retval VOID

    @note  Porting required

**/
VOID GpiSmiClear(UINT16 Type)
{
//Porting required
//All Gpis which correspondent bit in Type set to 1 should be cleared
}

/**
    This function detects GPI SMI event

    @param UINT16 *Type - pointer to store source of GPI SMI

    @retval TRUE GPI SMI occured, FALSE otherwise

    @note  Porting required

**/
BOOLEAN GpiSmiDetect(UINT16 *Type)
{
//Porting required
    return FALSE;
}

//*************** Standby button SMI Handler Porting hooks *****************

/**
    This function enables Standby button SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SButtonSmiEnable(VOID)
{
//Porting required
}

/**
    This function disables Standby button SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SButtonSmiDisable(VOID)
{
//Porting required
}

/**
    This function clears Standby button SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID SButtonSmiClear(VOID)
{
//Porting required
}

/**
    This function detects Standby button SMI event

    @param UINT16 *Type - pointer to store value of Standby button phase

    @retval TRUE Standby button SMI occured, FALSE otherwise

    @note  Porting required

**/
BOOLEAN SButtonSmiDetect(UINT16 *Type)
{
//Porting required
    return FALSE;
}

//*************** Power button SMI Handler Porting hooks *****************

/**
    This function enables Power button SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID PButtonSmiEnable(VOID)
{
//Porting required
}

/**
    This function disables Power button SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID PButtonSmiDisable(VOID)
{
//Porting required
}

/**
    This function clears Power button SMI

    @param VOID

    @retval VOID

    @note  Porting required

**/
VOID PButtonSmiClear(VOID)
{
//Porting required
}

/**
    This function detects Power button SMI event

    @param UINT16 *Type - pointer to store value of Power button phase

    @retval TRUE Standby button SMI occured, FALSE otherwise

    @note  Porting required

**/
BOOLEAN PButtonSmiDetect(UINT16 *Type)
{
//Porting required
    return FALSE;
}

//---------------------------------------------------------------------------
//                     I/O Trap SMI Handler Porting hooks
//---------------------------------------------------------------------------

/**
    This function sets I/O Trap functon based on given the
    context

    @param IoTrapContext - Pointer to the context that I/O trap register
        will be enabled.

    @retval VOID
**/

VOID IoTrapSmiSet (
    IN EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *IoTrapContext )
{
/*
    // Porting required if needed.
    UINT32 IoTrapAddr = RCRB_MMIO_IO_TRAP_0; // 0x1E80
    UINT32 i;
    UINT32 Buffer32 = 0;

    // Find an available I/O trap register
    for (i = 0; i < MAX_SUPPORTED_IOTRAP_REGS; i++) {
        if ((READ_MEM32_RCRB(IoTrapAddr) & 1) == 0) break;
        IoTrapAddr += 8;
    }

    IoTrapContext->TrapRegIndex = i;

    if (IoTrapContext->IoTrapContext.Length < 4) IoTrapContext->IoTrapContext.Length = 4;
    Buffer32 = IoTrapContext->IoTrapContext.Length;
    for (i = 0; Buffer32 != 1; Buffer32 >>= 1, i++);
    if (IoTrapContext->IoTrapContext.Length > (1 << i)) i++;

    IoTrapContext->IoTrapContext.Length = 1 << i; // Length is always 2^n
    
    Buffer32 = IoTrapContext->IoTrapContext.Address & 0xfffc;
    Buffer32 |= ((IoTrapContext->IoTrapContext.Length - 1) & 0xfffc) << 16;
    WRITE_MEM32_RCRB(IoTrapAddr, Buffer32);

    Buffer32 = 0xf0;
    if (IoTrapContext->TrapWidth == AccessWord) Buffer32 = 0x03;
    if (IoTrapContext->TrapWidth == AccessDWord) Buffer32 = 0x0f;

    if (IoTrapContext->IoTrapContext.TrapOpType == ReadWriteIoCycle) {
        Buffer32 |= (1 << 17); // Both Read/Write Cycles.
    } else {
        if (IoTrapContext->IoTrapContext.TrapOpType == ReadIoCycle)
            Buffer32 |= (1 << 16); // Read Cycle Only
    }

    WRITE_MEM32_RCRB(IoTrapAddr + 4, Buffer32);
    SET_MEM32_RCRB(IoTrapAddr, 1); // Enable Trap and SMI.
*/
}

/**
    This function resets I/O Trap functon based on given the
    context

    @param IoTrapContext - Pointer to the context that I/O trap register
        will be disabled.

    @retval VOID
**/

VOID IoTrapSmiReset (
    IN EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *IoTrapContext )
{
    // Porting required if needed.
 //   UINT32 IoTrapAddr = RCRB_MMIO_IO_TRAP_0 + IoTrapContext->TrapRegIndex * 8;

//    WRITE_MEM32_RCRB(IoTrapAddr, 0);
//    WRITE_MEM32_RCRB(IoTrapAddr + 4, 0);
}

/**
    This function enables I/O Trap SMI

    @param VOID

    @retval VOID

    @note  Porting required
**/

VOID IoTrapSmiEnable (VOID)
{
    // Porting required if needed.
}

/**
    This function disables I/O Trap SMI

    @param VOID

    @retval VOID

    @note  Porting required
**/

VOID IoTrapSmiDisable (VOID)
{
    // Porting required if needed.
}

/**
    This function clears all I/O Trap SMI status.

    @param VOID

    @retval VOID

    @note  Porting required
**/

VOID IoTrapSmiClear (VOID)
{
    // Porting required
//    SET_MEM32_RCRB(RCRB_MMIO_TRSR, 0); // 0x1E00
}

/**
    This function detects I/O Trap SMI event.

    @param *IoTrapContext - Pointer to EFI_SMM_IO_TRAP_DISPATCH_CONTEXT

    @retval TRUE I/O Trap SMI occured, the SMI context IoTrapContext
        should be updated according to the traped H/W
        information.

    @note  Porting required
**/

BOOLEAN IoTrapSmiDetect ( 
    OUT EFI_SMM_IO_TRAP_REGISTER_CONTEXT    *IoTrapContext )
{
/*
    UINT32      IoTrapStatus;
    UINT32      Buffer32;

    // Porting required
    IoTrapStatus = READ_MEM32_RCRB(RCRB_MMIO_TRSR) & 15; // 0x1E00

    if (IoTrapStatus) {

//        IoTrapContext->TrapRegIndex = 0;

//        while (IoTrapStatus != 1) {
//            IoTrapStatus >>= 1;
//            IoTrapContext->TrapRegIndex++;
//        }

        Buffer32 = READ_MEM32_RCRB(RCRB_MMIO_TRCR); // 0x1E10
//        IoTrapContext->TrapAddress = Buffer32 & 0xfffc;
        IoTrapContext->TrapOpType = (Buffer32 & 0x1000000) ? WriteIoCycle : ReadIoCycle;

//        if (IoTrapContext->TrapOpType == WriteIoCycle)
//            IoTrapContext->TrapData = READ_MEM32_RCRB(RCRB_MMIO_TWDR);

//        return TRUE;
//    }
*/
    return FALSE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
