//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include "Efi.h"
#include "Token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmIoTrapDispatch2.h>
#include <Protocol/PchSmmIoTrapControl.h>
#include <Library/GpioLib.h>
#include <Library/MmPciLib.h>
#include <SaAccess.h>
#include <SaRegs.h>

GLOBAL_REMOVE_IF_UNREFERENCED   PCH_SMM_IO_TRAP_CONTROL_PROTOCOL       *mPchSmmIoTrapControl;
GLOBAL_REMOVE_IF_UNREFERENCED   EFI_HANDLE                             NvIoTrapHandle;
GLOBAL_REMOVE_IF_UNREFERENCED   EFI_SMM_IO_TRAP_REGISTER_CONTEXT       NvIoTrapContext;
GLOBAL_REMOVE_IF_UNREFERENCED   EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL     *NvIoTrap;

#define NB_PCIE_CFG_ADDR(bus,dev,func,reg) \
  ((UINTN) (PCIEX_BASE_ADDRESS + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

static
VOID
Stall (
  UINTN     Usec
  )
{
  UINTN   i;
  UINT32  Data32;
  UINT32  PrevData;
  UINTN   Counter;

  Counter = (UINTN) ((Usec * 10) / 3);
  //
  // Call WaitForTick for Counter + 1 ticks to try to guarantee Counter tick
  // periods, thus attempting to ensure Microseconds of stall time.
  //
  if (Counter != 0) {

    PrevData = IoRead32 (PM_TMR_BLK_ADDRESS);
    for (i = 0; i < Counter;) {
      Data32 = IoRead32 (PM_TMR_BLK_ADDRESS);
      if (Data32 < PrevData) {
        //
        // Reset if there is a overlap
        //
        PrevData = Data32;
        continue;
      }

      i += (Data32 - PrevData);
      PrevData = Data32;
    }
  }

  return ;
}

VOID
PcieGpioWrite(
  IN       UINT8                        Expander,
  IN       UINT32                       Gpio,
  IN       BOOLEAN                      Active,
  IN       BOOLEAN                      Level
)
{
  EFI_STATUS  Status;

  if (Active == 0) {
    Level = (~Level) & 0x1;
  }

  Status = GpioSetOutputValue(Gpio, (UINT32)Level);
  if (Status != EFI_SUCCESS) {
    return;
  }
}

VOID
EFIAPI
NvIoTrapCallback (
  IN EFI_HANDLE                             DispatchHandle,
  IN CONST VOID                             *CallbackContext,
  IN OUT VOID                               *CommBuffer,
  IN OUT UINTN                              *CommBufferSize
  )
{
  //UINT32 PegLinkControlAddress;
  UINTN PegBaseAddress;	
  //UINT8 k;
  DEBUG ((DEBUG_INFO, "===================NvIoTrapCallback Enter==============\n"));
  IoWrite8(0x80, 0xCA);

  //
  // 1.Enable PEX Link between root port and GPU 
  //   (It should be Link Disable bit in Link Control register of root port.)
  //
  //PegLinkControlAddress = NB_PCIE_CFG_ADDR(SA_PEG_BUS_NUM,SA_PEG10_DEV_NUM,SA_PEG10_FUN_NUM,R_SA_PEG_LCTL_OFFSET);
  PegBaseAddress = MmPciBase (SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM);
  DEBUG ((DEBUG_INFO, "NvIoTrapCallback:: PegBaseAddress = %x\n",PegBaseAddress));
  MmioAnd8 (PegBaseAddress + R_SA_PEG_LCTL_OFFSET, (UINT8) ~(BIT4));
  //k = (MmioRead8(PegLinkControlAddress)) & 0xEF;  
  //MmioWrite8(PegLinkControlAddress,k);

  //
  // 2.Toggle GPU_EVENT to LOW 
  //   GPU_EVENT is a PCH GPIO assigned by H/W. 
  //   It might be different from project to project.
  //   Please fill in your GPU_EVENT value in the Token "GPIO_GPU_EVENT".
  //   Token value "ACTIVE_GPU_EVENT" need to set to '0' which means low active.
  //    
  PcieGpioWrite (EXPANDER_GPU_EVENT,
		 GPIO_GPU_EVENT,
		 ACTIVE_GPU_EVENT,
                 1
                 );
  
  //  
  // 3.Wait 20ms
  //
  Stall(20*1000);
  
  //  
  // 4.Toggle GPU_EVENT to HIGH
  //  
  PcieGpioWrite (EXPANDER_GPU_EVENT,
		 GPIO_GPU_EVENT,
		 ACTIVE_GPU_EVENT,
		 0
                 );
  
  //
  // 5.Touch the following registers on root port to enable PEX Link Bundle power. 
  //   This is required for SKL-H and KBL-H for GC6 exit.
  //   a.Root port PCIE register 0xC20[5:4] = 00b
  //   b.Root port PCIE Register 0xC38[3] = 00b
  //
  PegBaseAddress = NB_PCIE_CFG_ADDR(SA_PEG_BUS_NUM,SA_PEG10_DEV_NUM,SA_PEG10_FUN_NUM,0);  
  MmioAnd8 (PegBaseAddress + R_SA_PEG_AFEOVR_OFFSET, (UINT8) ~(BIT5 | BIT4));
  MmioAnd8 (PegBaseAddress + R_SA_PEG_CMNSPARE_OFFSET, (UINT8) ~(BIT3));
  
  
  DEBUG ((DEBUG_INFO, "===================NvIoTrapCallback Exit==============\n"));
  return;
}

EFI_STATUS
JtEnableIoTrap (
  IN EFI_HANDLE       DispatchHandle,
  IN CONST VOID       *DispatchContext OPTIONAL,
  IN OUT VOID         *CommBuffer OPTIONAL,
  IN OUT UINTN        *CommBufferSize OPTIONAL)
{
  EFI_STATUS Status;
  IoWrite8(0x80, 0xAA);
  //
  // 1.Enable I/O trap.
  //
  Status = mPchSmmIoTrapControl->Resume (
                        mPchSmmIoTrapControl,
                        NvIoTrapHandle
                        );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "JtEnableIoTrap:: mPchSmmIoTrapControl->Resume returned %r\n", Status));
    return Status;
  }
  //
  // 2.Clear IO cycle of root port command register 
  //
  MmioWrite8(NB_PCIE_CFG_ADDR(SA_PEG_BUS_NUM,SA_PEG10_DEV_NUM,SA_PEG10_FUN_NUM,0x4),0x06);
  return EFI_SUCCESS;
}

EFI_STATUS
JtDisableIoTrap (
  IN EFI_HANDLE       DispatchHandle,
  IN CONST VOID       *DispatchContext OPTIONAL,
  IN OUT VOID         *CommBuffer OPTIONAL,
  IN OUT UINTN        *CommBufferSize OPTIONAL)
{
  EFI_STATUS Status;
  IoWrite8(0x80, 0xBB);
  //
  // 1.Disable I/O trap.
  //
  Status = mPchSmmIoTrapControl->Pause (
                        mPchSmmIoTrapControl,
                        NvIoTrapHandle
                        );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "JtDisableIoTrap:: mPchSmmIoTrapControl->Pause returned %r\n", Status));
    return Status;
  }
  //
  // 2.Enable IO cycle of root port command register
  //
  MmioWrite8(NB_PCIE_CFG_ADDR(SA_PEG_BUS_NUM,SA_PEG10_DEV_NUM,SA_PEG10_FUN_NUM,0x4),0x07);    
  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: InSmmFunction
//
// Description: InSmmFunction
//
// Input:      
//
// Output: 
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS InSmmFunction (EFI_HANDLE ImageHandle,EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status;
    EFI_HANDLE                          SwHandle;
    EFI_SMM_BASE2_PROTOCOL              *gSmmBase2;
    EFI_SMM_SW_DISPATCH2_PROTOCOL       *gSwDispatch = NULL;	
    EFI_SMM_SW_REGISTER_CONTEXT         JtSwSmiContext = {SWSMI_JT_IO_TRAP_EN};
    EFI_SMM_SW_REGISTER_CONTEXT         JtSwSmiContext1 = {SWSMI_JT_IO_TRAP_DIS};
    
    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &gSmmBase2);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmmBase->GetSmstLocation (gSmmBase2, &pSmst);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmst->SmmLocateProtocol( \
                        &gEfiSmmSwDispatch2ProtocolGuid, NULL, &gSwDispatch);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = gSwDispatch->Register( gSwDispatch, JtEnableIoTrap, &JtSwSmiContext, &SwHandle);
    DEBUG ((DEBUG_INFO, "Nv JtEnableIoTrap returned %r\n", Status));
    Status = gSwDispatch->Register( gSwDispatch, JtDisableIoTrap, &JtSwSmiContext1, &SwHandle);
    DEBUG ((DEBUG_INFO, "Nv JtDisableIoTrap returned %r\n", Status));

    //
    // Locate the protocol for pause and resume
    //
    Status = pSmst->SmmLocateProtocol (&gPchSmmIoTrapControlGuid, NULL, (VOID **) &mPchSmmIoTrapControl);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "JtDisableIoTrap:: LocateProtocol gPchSmmIoTrapControlGuid failure. \n", Status));
      ASSERT_EFI_ERROR (Status);
      return Status;
    }     
    ///
    /// Locate the PCH IO TRAP Dispatch protocol
    ///
    NvIoTrapHandle = NULL;
    Status = pSmst->SmmLocateProtocol(&gEfiSmmIoTrapDispatch2ProtocolGuid, NULL, &NvIoTrap);
    ASSERT_EFI_ERROR (Status);
    if (!EFI_ERROR(Status)) {
      ///
      /// Register IO Trap SMI handler
      ///
      NvIoTrapContext.Type         = ReadTrap;
      NvIoTrapContext.Length       = 4;
      NvIoTrapContext.Address      = 0x3C8;
      Status = NvIoTrap->Register (
                              NvIoTrap,
                              (EFI_SMM_HANDLER_ENTRY_POINT2)NvIoTrapCallback,
                              &NvIoTrapContext,
                              &NvIoTrapHandle );
      DEBUG ((DEBUG_INFO, "Nv Register IoTrap returned %r\n", Status));
      DEBUG ((DEBUG_INFO, "NvIoTrapContext.Address = %x\n", NvIoTrapContext.Address));
      if (EFI_ERROR(Status)) return Status;
    }
    Status = mPchSmmIoTrapControl->Pause (
                                     mPchSmmIoTrapControl,
                                     NvIoTrapHandle
                                     );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Jt:: mPchSmmIoTrapControl->Pause returned %r\n", Status));
      return Status;
    }  
    return Status;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: NvSwSmiEntry
//
// Description: NvSwSmiEntry
//
// Input:       EFI_HANDLE           ImageHandle,
//              EFI_SYSTEM_TABLE     *SystemTable
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NvSmmKernelPanicEntry (
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE      *SystemTable )
{
    InitAmiLib(ImageHandle,SystemTable);

    return InitSmmHandler( ImageHandle, SystemTable, InSmmFunction, NULL );
}

//**********************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
