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
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Chipset/NB SMI/NBSMI.C 5     10/14/12 12:17a Jeffch $
//
// $Revision: 5 $
//
// $Date: 10/14/12 12:17a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Chipset/NB SMI/NBSMI.C $
// 
//*************************************************************************
/** @file NbSmi.c
    This file contains code for all North Bridge SMI events

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <SaRegs.h>
#include <Protocol/NBPlatformData.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmCpu.h>
#ifdef AMI_CRB_EC_SUPPORT_FLAG
#include <PlatformInfo.h>
#include <PlatformBoardId.h>
#endif

//#include <Setup.h>
#if NB_ERROR_LOG_SUPPORT
#include <Protocol/PchTcoSmiDispatch.h>
#include <NBSMI.h>
#include <Pci.h>
#include <PciE.h>
#endif
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)
static BOOLEAN          gEccErrHandleEnable = FALSE;
static BOOLEAN          gPegErrHandleEnable = FALSE;
NB_ASL_BUFFER           *gNbAslBufPtr = NULL;
NB_SETUP_DATA           *gNbSetupData = NULL;
EFI_SMM_CPU_PROTOCOL    *gSmmCpu = NULL;

// GUID Definition(s)
#if NB_ERROR_LOG_SUPPORT
EFI_GUID gNbErrorLogDispatchProtocolGuid        = EFI_NB_ERROR_LOG_DISPATCH_PROTOCOL_GUID;
#endif
// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)
//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)
#if NB_ERROR_LOG_SUPPORT
static SETUP_DATA                   gSetupData = {0};

NB_ERROR_LOG_DISPATCH_LINK  *gNbErrorLogDispatchHead = 0, *gNbErrorLogDispatchTail = 0;

//UINT32 NBPcieBridge[] = 
//{
//  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, PCI_VID)},
//  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN1, PCI_VID)},
//  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN2, PCI_VID)},
//  {0xFFFFFFFF}
//};

typedef struct {
    UINT8  Bus;
    UINT8  Dev;
    UINT8  Fun;
    UINT8  Reg;
} DEVICES_PCI_LIST;

DEVICES_PCI_LIST gNBPciBridge[] = {
    { PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN,  PCI_VID },
    { PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN1, PCI_VID },
    { PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN2, PCI_VID },
    {        0xFF,        0xFF,         0xFF,       0 }
};

UINTN gNBPciBridgeCount = sizeof(gNBPciBridge) / sizeof(DEVICES_PCI_LIST);

EFI_STATUS NbErrorLogRegister (
    IN EFI_NB_ERROR_LOG_DISPATCH_PROTOCOL   *This,
    IN EFI_NB_ERROR_LOG_DISPATCH            Function,
    OUT EFI_HANDLE                          *Handle 
);

EFI_STATUS NbErrorLogUnregister (
    IN EFI_NB_ERROR_LOG_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                           Handle 
);

EFI_NB_ERROR_LOG_DISPATCH_PROTOCOL gEfiNbErrorLogDispatchProtocol = \
                                   {NbErrorLogRegister, NbErrorLogUnregister};

NB_ERROR_INFO NbErrorInfo;
UINT32        gDevBaseAddr = 0;

#if NB_ECC_ERROR_LOG_SUPPORT
EFI_STATUS NBEccErrLogHandle(VOID);
#endif

#endif

#if NB_ERROR_LOG_SUPPORT
/**
    Create and add link to specified list.

    @param Size 
    @param Head 
    @param Tail 

    @retval VOID Pointer

    @note  Here is the control flow of this function:
              1. Allocate Link in Smm Pool.
              2. Add Link to end.
              3. Return Link address.

**/

VOID * AddLink (
    IN UINT32       Size,
    IN VOID         **Head,
    IN VOID         **Tail )
{
    VOID *Link;

    if (pSmst->SmmAllocatePool(EfiRuntimeServicesData, Size, (VOID **)&Link) != EFI_SUCCESS) return 0;

    ((GENERIC_LINK*)Link)->Link = 0;
    if (!*Head) {
        *Head = *Tail = Link;
    } else {
        ((GENERIC_LINK*)*Tail)->Link = Link;
        *Tail = Link;
    }

    return Link;
}

/**
    Remove link from specified list.

    @param Handle EFI Handle
    @param Head 
    @param Tail 

    @retval BOOLEAN
        TRUE if link was removed. FALSE if link not in the list.

    @note  Here is the control flow of this function:
              1. Search link list for Link.
              2. Remove link from list.
              3. Free link.

**/

BOOLEAN RemoveLink (
    IN EFI_HANDLE   Handle,
    IN VOID         **Head,
    IN VOID         **Tail )
{
    GENERIC_LINK *PrevLink,*Link;

    PrevLink = *Head;

    // Is link first. Link address is the same as the Handle. 
    if (((GENERIC_LINK*)*Head) == Handle) {
        if (PrevLink == *Tail) *Tail = 0; // If Tail = Head, then 0.
        *Head = PrevLink->Link;
        pSmst->SmmFreePool(PrevLink);
        return TRUE;
    }

    // Find Link.
    for (Link=PrevLink->Link; Link; PrevLink=Link, Link=Link->Link) {
        if (Link == Handle) {   // Link address is the same as the Handle.
            if (Link == *Tail) *Tail = PrevLink;
            PrevLink->Link = Link->Link;
            pSmst->SmmFreePool(Link);
            return TRUE;
        }
    }

    return FALSE;
}

/**
    Register a Link on NbErrorLog enable SMI.

    @param This 
    @param Function 
    @param Context 
              

    @retval Handle -
        EFI_STATUS

    @note  Here is the control flow of this function:
              1. Verify if Context if valid. If invalid,
                 return EFI_INVALID_PARAMETER.
              2. Allocate structure and add to link list.
              3. Fill link.
              4. Enable Smi Source.

**/

EFI_STATUS NbErrorLogRegister (
    IN EFI_NB_ERROR_LOG_DISPATCH_PROTOCOL   *This,
    IN EFI_NB_ERROR_LOG_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle )
{
    NB_ERROR_LOG_DISPATCH_LINK *NewLink;

    NewLink = AddLink( sizeof(NB_ERROR_LOG_DISPATCH_LINK), \
                       &gNbErrorLogDispatchHead, \
                       &gNbErrorLogDispatchTail );
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function = Function;
    *Handle = NewLink;

#if NB_ECC_ERROR_LOG_SUPPORT
    if (gSetupData.RtSystemEnable)
    { 
      if(gSetupData.RtEccEnable)
      {
        if(((READ_PCI16_NB(0xC8) & (BIT00 | BIT01)) != 0) && gEccErrHandleEnable)
        {
             NBEccErrLogHandle();
        }
      }
    }
#endif

    return EFI_SUCCESS;
}

/**
    Unregister a Link on NbErrorLog enable SMI.

    @param This 
    @param Handle 

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
              2. Disable SMI Source if no other handlers using source.
              3. Return EFI_SUCCESS.

**/

EFI_STATUS NbErrorLogUnregister (
    IN EFI_NB_ERROR_LOG_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle )
{
    if (!RemoveLink(Handle, &gNbErrorLogDispatchHead, &gNbErrorLogDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}
#endif
/**
    This is a template NB SMI GetContext for Porting.

    @param VOID

    @retval BOOLEAN

    @note  Here is the control flow of this function:
              1. Check if NB Smi source.
              2. If yes, return TRUE.
              3. If not, return FALSE.
**/

BOOLEAN GetNbSmiContext (VOID)
{
        return FALSE;
}

#if NB_ERROR_LOG_SUPPORT
/**
    Init NB PCIE Error devices log.

    @param VOID

    @retval VOID

**/
VOID NBPcieErrLogHandle (VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  NB_ERROR_LOG_DISPATCH_LINK       *Link;
  UINT8      CapPtr;
  UINT16     PciStatus;
  UINT16     PcieStatus;
  UINT16     DeviceStatus;

  MemSet((VOID*)&NbErrorInfo, sizeof(NB_ERROR_INFO), 0);

  PciStatus = READ_MEM16(gDevBaseAddr + PCI_STS);
  PcieStatus = READ_MEM16(gDevBaseAddr + PCI_SECSTATUS);

  CapPtr = NbFindCapPtr(gDevBaseAddr, PCI_CAP_ID_PCIEXP);
  if (CapPtr != 0)
    DeviceStatus = READ_MEM16(gDevBaseAddr + CapPtr + PCIE_DEV_STA_OFFSET);
 
  if ((PciStatus & (BIT8 | BIT15)) || (PcieStatus & (BIT8 | BIT15)))
    NbErrorInfo.PcieErrorInfo.ParityError = TRUE;
  else
    NbErrorInfo.PcieErrorInfo.ParityError = FALSE;

   if ((PciStatus & BIT14) || (PcieStatus & BIT14) || ((CapPtr != 0) && (DeviceStatus & 0x7)))
    NbErrorInfo.PcieErrorInfo.SystemError = TRUE;
   else
    NbErrorInfo.PcieErrorInfo.SystemError = FALSE;

  if ((NbErrorInfo.PcieErrorInfo.ParityError) || (NbErrorInfo.PcieErrorInfo.SystemError)) 
  {
    NbErrorInfo.PcieErrorInfo.Bus = (gDevBaseAddr >> 20) & ((UINT8)((PCIEX_LENGTH >> 20) - 1));
    NbErrorInfo.PcieErrorInfo.Dev = (gDevBaseAddr >> 15) & 0x1F;
    NbErrorInfo.PcieErrorInfo.Fun = (gDevBaseAddr >> 12) & 0x07;
    NbErrorInfo.PcieErrorInfo.VendorId  = READ_MEM16(gDevBaseAddr + PCI_VID);
    NbErrorInfo.PcieErrorInfo.DeviceId  = READ_MEM16(gDevBaseAddr + PCI_DID);
    NbErrorInfo.PcieErrorInfo.PciCommand = READ_MEM16(gDevBaseAddr + PCI_CMD);
    NbErrorInfo.PcieErrorInfo.PciCCode  = READ_MEM16(gDevBaseAddr + PCI_SCC);
    NbErrorInfo.PcieErrorInfo.BridgeControl = READ_MEM16(gDevBaseAddr + PCI_BRIDGE_CNTL);
    NbErrorInfo.PcieErrorInfo.Version  = READ_MEM8(gDevBaseAddr + CapPtr + PCIE_CAP_OFFSET) & 0x0F;
    NbErrorInfo.PcieErrorInfo.PortType  = (UINT32)((READ_MEM8(gDevBaseAddr + CapPtr + PCIE_CAP_OFFSET) & 0xF0) >> 4);

    if (CapPtr != 0) {
      NbErrorInfo.PcieErrorInfo.Correctable = (DeviceStatus & BIT0)? TRUE : FALSE;
      NbErrorInfo.PcieErrorInfo.NonFatal = (DeviceStatus & BIT1)? TRUE : FALSE;
      NbErrorInfo.PcieErrorInfo.Fatal = (DeviceStatus & BIT2)? TRUE : FALSE;
    }

    NbErrorInfo.ErrorType = NbPcieError; // PCIE Error

    for (Link = gNbErrorLogDispatchHead; Link; Link = Link->Link) {
      Link->Function(Link, NbErrorInfo);
    }

    // Clear Error status
    WRITE_MEM16(gDevBaseAddr + PCI_STS, PciStatus);
    WRITE_MEM16(gDevBaseAddr + PCI_SECSTATUS, PcieStatus);

    if (CapPtr != 0)
      // Clear Error Status
      WRITE_MEM16(gDevBaseAddr + CapPtr + PCIE_DEV_STA_OFFSET, DeviceStatus);

    // Clear DMISERR
    WRITE_IO16((TCO_BASE_ADDRESS + 0x04), BIT12);

    // Clear SERR#_NMI_STS & NMI2SMI_STS by set Port 61h[2] = 1 then set it to 0.
    if(NbErrorInfo.PcieErrorInfo.SystemError) 
    {
      SET_IO8(NMI_SC_PORT, BIT02);
      RESET_IO8(NMI_SC_PORT, BIT02);
    }
  }
}

/**
    The function will check PCIE error event.

    @param N/A

    @retval EFI_SUCCESS ECC error event generated.
    @retval Other No ECC error event

**/

BOOLEAN GetNbPcieContext(
)
{
  UINT8      i;
  UINT8      PegBus;
  UINT8      PegDev;
  UINT8      PegFun;

//  if(!gPegErrHandleEnable) return FALSE;

  for ( i = 0; i < gNBPciBridgeCount; i++)
  {
    PegBus = gNBPciBridge[i].Bus;
    PegDev = gNBPciBridge[i].Dev;
    PegFun = gNBPciBridge[i].Fun;

    if (READ_PCI32(PegBus, PegDev, PegFun, 0) == 0xFFFFFFFF)
      continue;

    if(((READ_PCI16(PegBus, PegDev, PegFun, PCI_CMD) & (BIT6 | BIT8)) == (BIT6 | BIT8)) &&
      (((READ_PCI16(PegBus, PegDev, PegFun, PCI_STS) & (BIT08 | BIT14 | BIT15)) != 0) ||
       ((READ_PCI16(PegBus, PegDev, PegFun, PCI_SECSTATUS) & (BIT08 | BIT14 | BIT15)) != 0)))
    {
	  gDevBaseAddr = (UINT32)NB_PCIE_CFG_ADDRESS(PegBus, PegDev, PegFun, PCI_VID);
      return TRUE;
    }
  }
  return FALSE;
}
#endif

#if NB_ECC_ERROR_LOG_SUPPORT
/**
    The function will check what ECC feature is enable or disable.

    @param N/A

    @retval EFI_SUCCESS ECC feature is support.
    @retval EFI_UNSUPPORTED ECC feature is not support

**/
EFI_STATUS EccEnableFunction(VOID)
{
    if (((READ_MEM32_MCH(0x5004) & (BIT12 | BIT13)) != 0) || \
        ((READ_MEM32_MCH(0x5008) & (BIT12 | BIT13)) != 0))
    {
// [ EIP428812 ] Remove useless code.
//        RW_MEM32_MCH(0x4034, 0, (BIT14 | BIT16 | BIT17));
//        RW_MEM32_MCH(0x4434, 0, (BIT14 | BIT16 | BIT17));

        // Disable Error and SCI Commands
        RW_PCI16_NB(0xCA, 0, (BIT00 | BIT01));
        RW_PCI16_NB(0xCE, 0, (BIT00 | BIT01));

        // Enable SMI Command
//        RW_PCI16_NB(0xC8, (BIT00 | BIT01), 0);
        RW_PCI16_NB(0xCC, (BIT00 | BIT01), 0);

        return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}
/**
    The function will check ECC error event.

    @param N/A

    @retval EFI_SUCCESS ECC error event generated.
    @retval Other No ECC error event

**/

BOOLEAN GetNbEccContext(VOID)
{
    if (gEccErrHandleEnable)
    {
        if (((READ_PCI16_NB(0xCC) & (BIT00 | BIT01)) != 0) && 
            ((READ_PCI16_NB(0xC8) & (BIT00 | BIT01)) != 0))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/**
    This function handles ECC error.

    @param This NB SMI Context pointer

    @retval EFI_SUCCESS ECC error is handled.

**/
EFI_STATUS NBEccErrLogHandle(VOID)
{

    NB_ERROR_INFO                    NbErrorInfo;
    NB_ERROR_LOG_DISPATCH_LINK       *Link;

    MemSet((VOID*)&NbErrorInfo, sizeof(NB_ERROR_INFO), 0);

    if ((READ_MEM32_MCH(0x4C48) & (BIT00 | BIT01)) != 0)
    {
        NbErrorInfo.EccErrorInfo.EccErrLog0 = READ_MEM32_MCH(0x4C48);
        NbErrorInfo.EccErrorInfo.EccErrLog1 = READ_MEM32_MCH(0x4C4C);
    }

    // Channel 0
    if ((READ_MEM32_MCH(0x4048) & (BIT00 | BIT01)) != 0)
    {
        NbErrorInfo.EccErrorInfo.Ch0_EccErrLog0 = READ_MEM32_MCH(0x4048);
        NbErrorInfo.EccErrorInfo.Ch0_EccErrLog1 = READ_MEM32_MCH(0x404C);
        // [28:27] 00 or 01 = DimmNum 0, 10 or 11 = DimmNum 1
        NbErrorInfo.EccErrorInfo.EccErrDimmNum = (READ_MEM32_MCH(0x4048) & BIT28) ? 1 : 0;
    }

    // Channel 1
    if ((READ_MEM32_MCH(0x4448) & (BIT00 | BIT01)) != 0)
    {
        NbErrorInfo.EccErrorInfo.Ch1_EccErrLog0 = READ_MEM32_MCH(0x4448);
        NbErrorInfo.EccErrorInfo.Ch1_EccErrLog1 = READ_MEM32_MCH(0x444C);
        // [28:27] 00 or 01 = DimmNum 0, 10 or 11 = DimmNum 1
        NbErrorInfo.EccErrorInfo.EccErrDimmNum = (READ_MEM32_MCH(0x4448) & BIT28) ? 3 : 2;
    }

    if ((READ_PCI16_NB(0xC8) & BIT00) != 0)
        NbErrorInfo.EccErrorInfo.Correctable = 1;

    if ((READ_PCI16_NB(0xC8) & BIT01) != 0)
        NbErrorInfo.EccErrorInfo.UnCorrectable = 1;

    NbErrorInfo.ErrorType = NbEccError; // Ecc Error

    for(Link = gNbErrorLogDispatchHead; Link; Link = Link->Link) {
       Link->Function(Link, NbErrorInfo);
    }

    RW_PCI16_NB(0xC8, (BIT00 | BIT01), 0);

#if ECC_MULTI_BIT_TYPE_HANG == 1
    if (NbErrorInfo.EccErrorInfo.UnCorrectable == 1){
         EFI_DEADLOOP()
    }
#endif

    return EFI_SUCCESS;
}
#endif

#if NB_ERROR_LOG_SUPPORT
/**
    North Bridge error logging handler.

    @param DispatchHandle Handle of dispatch function, for when interfacing
        with the parent SMM driver, will be the address of linked
        list link in the call back record.
    @param DispatchContext Pointer to the dispatch function's context.

    @retval VOID

**/
VOID NBErrLogHandler (
    IN EFI_HANDLE                       DispatchHandle
)
{
//#if NB_PCIE_ERROR_LOG_SUPPORT
//    if (GetNbPcieContext()) NBPcieErrLogHandle();
//#endif
}

#endif

/**
    This is a template NB SMI Handler for Porting.

    @param VOID

    @retval VOID
**/

VOID NbSmiHandler (VOID)
{

}

/**
    This is a NB software SMI Handler for IGFX int15 get setup data.

    @param DispatchHandle EFI Handle
    @param DispatchContext Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval VOID
**/
EFI_STATUS NbSwSmiIgfxGetSetupHandler (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL)
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;    
    UINTN       Cpu = (UINTN)-1;
    UINT16      RegBx;    

    if (CommBuffer != NULL && CommBufferSize != NULL) {
      Cpu = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
    }
    
    if(Cpu == (UINTN)-1) return Status;
    
    Status = gSmmCpu->ReadSaveState (gSmmCpu,
                                     2,
                                     EFI_SMM_SAVE_STATE_REGISTER_RBX,
                                     Cpu,
                                     &RegBx);
    if(EFI_ERROR(Status)) return Status;

    switch (RegBx & 0xff) {
        case IGFX_LCD_PANEL_TYPE:  // 0x80
             RegBx = (UINT16)gNbSetupData->LcdPanelType;
             break;
        case IGFX_LCD_PANEL_SCALING: // 0x81
             RegBx = (UINT16)gNbSetupData->LcdPanelScaling;
             break;
        case IGFX_BOOT_TYPE: // 0x82
             RegBx = (UINT16)gNbSetupData->IgdBootType;
             break;
        case IGFX_BACKLIGHT_TYPE: // 0x83
             RegBx = (UINT16)gNbSetupData->IgdLcdBlc;
             break;
        case IGFX_LFP_PANEL_COLOR_DEPTH_TYPE: // 0x84
             RegBx = (UINT16)gNbSetupData->LfpColorDepth;
             break;
        case IGFX_EDP_ACTIVE_LFP_CONFIG_TYPE: // 0x85
             RegBx = (UINT16)gNbSetupData->ActiveLFP;
             break;
        case IGFX_PRIMARY_DISPLAY_TYPE: // 0x86
             RegBx = (UINT16)gNbSetupData->PrimaryDisplay;
             break;
        case IGFX_DISPLAY_PIPE_B_TYPE: // 0x87
             RegBx = (UINT16)gNbSetupData->DisplayPipeB;
             break;
//        case IGFX_SDVO_PANEL_TYPE: // 0x88
//             RegBx = (UINT16)gNbSetupData->SdvoPanelType;
//             break;
        case IGFX_DISP_DDI_TYPE: // 0x89
             RegBx = (UINT16)gNbSetupData->DDISelection;
             break;
        default:
             RegBx = 0;
             break;
    } // switch
    
    Status = gSmmCpu->WriteSaveState(gSmmCpu, 
                                     2,
                                     EFI_SMM_SAVE_STATE_REGISTER_RBX, 
                                     Cpu, 
                                     &RegBx);    

    return Status;    
}

/**
    This is a template NB Sx SMI Handler for Porting.

    @param DispatchHandle EFI Handle
    @param DispatchContext Pointer to the EFI_SMM_SX_DISPATCH_CONTEXT

    @retval VOID
**/
EFI_STATUS NbSxSmiHandler (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL )
{
/*
    // SMBAVUMA Workaround
    WRITE_IO8(0x3c4, 0x01);
    SET_IO8(0x3c5, 0x20);
*/
    return EFI_SUCCESS;
}

/**
    North Bridge SMM Child Dispatcher Handler.

    @param SmmImageHandle 
    @param CommunicationBuffer OPTIONAL
    @param SourceSize OPTIONAL

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Read SMI source status registers.
              2. If source, call handler.
              3. Repeat #2 for all sources registered.
**/

EFI_STATUS NbChildDispatcher(
    IN EFI_HANDLE           DispatchHandle,
    IN CONST VOID           *DispatchContext OPTIONAL,
    IN OUT VOID             *CommBuffer OPTIONAL,
    IN OUT UINTN            *CommBufferSize OPTIONAL)
{
    if (GetNbSmiContext()) NbSmiHandler();
#if NB_ECC_ERROR_LOG_SUPPORT
    if(gSetupData.RtEccEnable){
      if (GetNbEccContext()) NBEccErrLogHandle();
    }
#endif
#if NB_PCIE_ERROR_LOG_SUPPORT
    if(gSetupData.RtPciErrorEnable){
      if (GetNbPcieContext()) NBPcieErrLogHandle();
    }
#endif
    return EFI_SUCCESS;
}

//----------------------------------------------------------------------------

/**
    Installs North Bridge SMM Child Dispatcher Handler.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/

EFI_STATUS InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                        Status;
    EFI_STATUS                        PchTcoSmiDispatchStatus = EFI_SUCCESS;
#if NB_ERROR_LOG_SUPPORT
    EFI_HANDLE                        RootHandle = NULL;
    EFI_HANDLE                        DummyHandle = NULL;
#endif
    EFI_SMM_SW_DISPATCH2_PROTOCOL     *pSwDispatch;
    EFI_SMM_SX_DISPATCH2_PROTOCOL     *pSxDispatch;
    EFI_SMM_SW_REGISTER_CONTEXT       SwContext = {NB_SWSMI_IGFX_GET_SETUP};
    EFI_SMM_SX_REGISTER_CONTEXT       SxContext = {SxS3, SxEntry};
    EFI_HANDLE                        Handle;
    EFI_GUID                          NbAslBufPtrGuid = NB_ASL_BUFFER_PTR_GUID;
    CHAR16                            NbAslBufPtrVar[] = NB_ASL_BUFFER_PTR_VARIABLE;
    UINTN                             VarSize = sizeof(UINT32);
    UINT32                            NbAslBufPtr;
    UINTN                             VariableSize = sizeof(NB_SETUP_DATA);
#if NB_ERROR_LOG_SUPPORT
    PCH_TCO_SMI_DISPATCH_PROTOCOL    *PchTcoSmiDispatch;
    EFI_GUID    SetupGuid = SETUP_GUID;
    UINTN       Size = sizeof(SETUP_DATA);
#endif
    
    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;
    
    Status = pRS->GetVariable( NbAslBufPtrVar, \
                               &NbAslBufPtrGuid, \
                               NULL, \
                               &VarSize, \
                               &NbAslBufPtr );
    if (!EFI_ERROR(Status)) gNbAslBufPtr = (NB_ASL_BUFFER *)(UINTN)NbAslBufPtr;
    
    Status = pSmst->SmmAllocatePool(EfiRuntimeServicesData, VariableSize, (VOID **)&gNbSetupData);
    if (!EFI_ERROR (Status)){
        GetNbSetupData(pRS, gNbSetupData, FALSE); 
    } else {
        gNbSetupData->EccSupport = 0;
        gNbSetupData->LcdPanelType = 0;
//        gNbSetupData->SdvoPanelType = 0;
        gNbSetupData->LcdPanelScaling = 0;
        gNbSetupData->IgdBootType = 0;
        gNbSetupData->DisplayPipeB = 0;
        gNbSetupData->IgdLcdBlc = 0;
        gNbSetupData->ActiveLFP = 1;
        gNbSetupData->LfpColorDepth = 0;
        gNbSetupData->PrimaryDisplay = 3;
        gNbSetupData->DDISelection = 0;
    }
    
#ifdef AMI_CRB_EC_SUPPORT_FLAG
{
    PLATFORM_INFO             *PlatformInfo;
    //
    // Detect PlatformInfo for NB DXE use
    //
    Status = pSmst->SmmLocateProtocol(&gPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfo);
    if (!EFI_ERROR(Status)) {
        switch (PlatformInfo->BoardID) {
          case BoardIdSkylakeA0Lpddr3Crb:
          case BoardIdSkylakeA0Ddr4Rvp2:
          case BoardIdSkylakeA0Rvp3:
          case BoardIdSkylakeA0Rvp3Ppv:
          case BoardIdSkylakeULpddr3Rvp5:
          case BoardIdSkylakeURvp7:
          case BoardIdSkylakeURvp7Ppv:
          case BoardIdSkylakeYRvp13:
          case BoardIdSkylakeURvp15:
          case BoardIdSkylakeSds:
            gNbSetupData->DDISelection = 1;
            break;
        }
    }
}
#endif
    
    Status = pSmst->SmmLocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, (VOID **)&gSmmCpu);
    if (EFI_ERROR(Status)) return Status;

    Status = pSmst->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **)&pSwDispatch);    
    if (!EFI_ERROR(Status)) {
        if (READ_PCI32_IGD (R_SA_IGD_VID) != 0xFFFFFFFF) {
            Status = pSwDispatch->Register (pSwDispatch, \
                                            NbSwSmiIgfxGetSetupHandler, \
                                            &SwContext, \
                                            &Handle);
        }
    }

    Status = pSmst->SmmLocateProtocol(&gEfiSmmSxDispatch2ProtocolGuid, NULL, (VOID **)&pSxDispatch);
    if (!EFI_ERROR(Status)) {
        Status = pSxDispatch->Register (pSxDispatch, \
                                        NbSxSmiHandler, \
                                        &SxContext, \
                                        &Handle);     
    }

#if NB_ERROR_LOG_SUPPORT
    Status = pRS->GetVariable ( L"Setup", \
                                &SetupGuid, \
                                NULL,\
                                &Size, \
                                &gSetupData );
    
    if (gSetupData.RtSystemEnable)
    {
      if(gSetupData.RtPciErrorEnable) {

//     if (gNbSetupData->SmbiosLogging == 1)
//     {
       
	   Status = pSmst->SmmLocateProtocol (&gPchTcoSmiDispatchProtocolGuid, NULL, (VOID **) &PchTcoSmiDispatch);
	
#if NB_ECC_ERROR_LOG_SUPPORT
	   if(gSetupData.RtEccEnable) {
           
              if(gNbSetupData->EccSupport)
              {
                 if (!EFI_ERROR(Status))
                 {
                      PchTcoSmiDispatchStatus = PchTcoSmiDispatch->MchRegister( PchTcoSmiDispatch, \
                                                                                NBErrLogHandler, \
                                                                                &Handle );
                      ASSERT_EFI_ERROR (PchTcoSmiDispatchStatus);
              
                      if (!EFI_ERROR(PchTcoSmiDispatchStatus)) gEccErrHandleEnable = TRUE;
                      // Enable ECC error log function
                      //EccEnableFunction();
                      }
                  }
	       }

#endif

// AMI_DEBUG >>>
// Removed NmiRegister(), it could be invoked once only.
// AMI done that at InSmmFunction() in PcieErrorLog.c.
//#if NB_PCIE_ERROR_LOG_SUPPORT
//        if (!EFI_ERROR(Status))
//        {
//           PchTcoSmiDispatchStatus = PchTcoSmiDispatch->NmiRegister( PchTcoSmiDispatch, \
//                                                                     NBErrLogHandler, \
//                                                                     &Handle);
//           ASSERT_EFI_ERROR (PchTcoSmiDispatchStatus);
           //
           // Enable TCO_EN.
           //
//           SET_IO16_PM(R_PCH_SMI_EN, B_PCH_SMI_EN_TCO);
//           if (!EFI_ERROR(PchTcoSmiDispatchStatus)) gPegErrHandleEnable = TRUE;
//        }
//#endif 
// AMI_DEBUG <<<

       Status = pSmst->SmmInstallProtocolInterface( &DummyHandle, \
                                                    &gNbErrorLogDispatchProtocolGuid, \
                                                    EFI_NATIVE_INTERFACE, \
                                                    &gEfiNbErrorLogDispatchProtocol );
//    }
      }  
    }
    

    // Register Callbacks   
    if ((gSetupData.RtSystemEnable)){
      Status  = pSmst->SmiHandlerRegister (NbChildDispatcher, \
                                           NULL, \
                                           &RootHandle);    
    }
    
#endif
    
    return EFI_SUCCESS;
}

/**
    Installs North Bridge SMM Child Dispatcher Handler.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI InitializeNbSmm (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{   
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);    
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
