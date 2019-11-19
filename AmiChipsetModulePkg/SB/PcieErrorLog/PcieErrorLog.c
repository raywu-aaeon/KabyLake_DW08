/** @file PowerButton.c
    Provide functions to register and handle Powerbutton
    functionality.  This code is generic and as long as PM
    base address SDL token is defined properly this should
    work fine.

**/
#define ELOG_PROTOCOL   0

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Sb.h>
#include <PchAccess.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmbiosElogSupport.h>
#include <Protocol/SbSmiProtocol.h>
#include <Protocol/PchTcoSmiDispatch.h>
#include <Library/PchCycleDecodingLib.h>
#include <Pci.h>
#include <PciE.h>

EFI_SMM_BASE2_PROTOCOL                  *pSmmBase2;
EFI_SMM_SYSTEM_TABLE2                   *pSmst2;
PCH_TCO_SMI_DISPATCH_PROTOCOL           *mPchTcoSmiDispatchProtocol;
EFI_SMM_SW_DISPATCH2_PROTOCOL           *SwDispatch;
#if ELOG_PROTOCOL
EFI_SM_ELOG_PROTOCOL                    *GenericElogProtocol = NULL;
#endif

#if SB_PCIE_ERROR_LOG_SUPPORT
static SETUP_DATA                   gSetupData = {0};
#endif

EFI_SMM_SW_REGISTER_CONTEXT             SbErrorLogS3PatchContext = {SW_SMI_SB_EL_S3};

SB_PCIE_ERROR_LOG_DISPATCH_LINK         *gSbPcieErrorLogDispatchHead = 0, *gSbPcieErrorLogDispatchTail = 0;

EFI_GUID gSbPcieErrorLogDispatchProtocolGuid = EFI_SB_PCIE_ERROR_LOG_DISPATCH_PROTOCOL_GUID;

UINT32 SBPcieBridge[] = 
{
  {(UINT32)SB_PCIERP00_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP01_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP02_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP03_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP04_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP05_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP06_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP07_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP08_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP09_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP10_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP11_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP12_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP13_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP14_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP15_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP16_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP17_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP18_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP19_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP20_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP21_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP22_MMIO_REG(PCI_VID)},
  {(UINT32)SB_PCIERP23_MMIO_REG(PCI_VID)},
  {0xFFFFFFFF}
};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AddLink
//
// Description: Create and add link to specified list.
//
// Parameters:  Size -
//              Head - 
//              Tail -
//
// Returns:     VOID Pointer
//
// Modified:
//
// Referrals:   SmmAllocatePool
//
// Notes:       Here is the control flow of this function:
//              1. Allocate Link in Smm Pool.
//              2. Add Link to end.
//              3. Return Link address.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID * AddLink (
    IN UINT32       Size,
    IN VOID         **Head,
    IN VOID         **Tail )
{
    VOID *Link;

    if (pSmst2->SmmAllocatePool(EfiRuntimeServicesData, Size, (VOID **)&Link) != EFI_SUCCESS) return 0;

    ((GENERIC_LINK*)Link)->Link = 0;
    if (!*Head) {
        *Head = *Tail = Link;
    } else {
        ((GENERIC_LINK*)*Tail)->Link = Link;
        *Tail = Link;
    }

    return Link;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RemoveLink
//
// Description: Remove link from specified list.
//
// Parameters:  Handle - EFI Handle
//              Head -
//              Tail -
//
// Returns:     BOOLEAN
//                  TRUE if link was removed. FALSE if link not in the list.
//
// Modified:
//
// Referrals:   SmmFreePool
//
// Notes:       Here is the control flow of this function:
//              1. Search link list for Link.
//              2. Remove link from list.
//              3. Free link.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

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
        pSmst2->SmmFreePool(PrevLink);
        return TRUE;
    }

    // Find Link.
    for (Link=PrevLink->Link; Link; PrevLink=Link, Link=Link->Link) {
        if (Link == Handle) {   // Link address is the same as the Handle.
            if (Link == *Tail) *Tail = PrevLink;
            PrevLink->Link = Link->Link;
            pSmst2->SmmFreePool(Link);
            return TRUE;
        }
    }

    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   EfiSbPcieErrorLogEnRegister
//
// Description: Register a Link on SbPcieErrorLog enable SMI.
//
// Parameters:  This - 
//              Function -
//              Context - 
//              
//
// Returns:     Handle -
//              EFI_STATUS
//
// Modified:    gSbPcieErrorLogDispatchHead, gSbPcieErrorLogDispatchTail
//
// Referrals:   AddLink
//
// Notes:       Here is the control flow of this function:
//              1. Verify if Context if valid. If invalid,
//                 return EFI_INVALID_PARAMETER.
//              2. Allocate structure and add to link list.
//              3. Fill link.
//              4. Enable Smi Source.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS SbPcieErrorLogRegister (
    IN EFI_SB_PCIE_ERROR_LOG_DISPATCH_PROTOCOL   *This,
    IN EFI_SB_PCIE_ERROR_LOG_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle )
{
    SB_PCIE_ERROR_LOG_DISPATCH_LINK *NewLink;

    NewLink = AddLink( sizeof(SB_PCIE_ERROR_LOG_DISPATCH_LINK), \
                       &gSbPcieErrorLogDispatchHead, \
                       &gSbPcieErrorLogDispatchTail );
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SbPcieErrorLogUnregister
//
// Description: Unregister a Link on SbPcieErrorLog enable SMI.
//
// Parameters:  This -
//              Handle -
//
// Returns:     EFI_STATUS
//
// Modified:    gSbPcieErrorLogDispatchHead, gSbPcieErrorLogDispatchTail
//
// Referrals:   RemoveLink
//
// Notes:       Here is the control flow of this function:
//              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
//              2. Disable SMI Source if no other handlers using source.
//              3. Return EFI_SUCCESS.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS SbPcieErrorLogUnregister (
    IN EFI_SB_PCIE_ERROR_LOG_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle )
{
    if (!RemoveLink(Handle, &gSbPcieErrorLogDispatchHead, &gSbPcieErrorLogDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure: ElogGenerateNmiNow
//
// Description: Generate NmiNow.
//
// Input: None
//
// Output: None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID ElogGenerateNmiNow (VOID)
{
  UINT16          SaveNmi2SmiEn;
  UINT8           SavePort70;

  // Read the NMI2SMI_EN bit, save it for future restore
  SaveNmi2SmiEn = READ_IO16_TCO (R_PCH_TCO1_CNT);

  // Set the NMI2SMI_EN bit to 0
  RESET_IO16_TCO (R_PCH_TCO1_CNT, B_PCH_TCO_CNT_NMI2SMI_EN);

  SavePort70 = ReadPort70h();

  // Enable NMI_EN
  WRITE_IO8(CMOS_ADDR_PORT, (SavePort70 & ~B_PCH_NMI_EN_NMI_EN));

  // Set NMI_NOW = 1
  SET_IO16_TCO (R_PCH_TCO1_CNT, B_PCH_TCO_CNT_NMI_NOW);

  // Clear NMI_NOW = 0 by writing 1 to NMI_NOW bit
  SET_IO16_TCO (R_PCH_TCO1_CNT, B_PCH_TCO_CNT_NMI_NOW);

  // Restore NMI2SMI_EN
  WRITE_IO16_TCO (R_PCH_TCO1_CNT, SaveNmi2SmiEn);

  // Clear the DMISERR_STS bit, bit 12
  WRITE_IO16_TCO (R_PCH_TCO1_STS, B_PCH_TCO1_STS_DMISERR);

  // Clear the NMI2SMI_STS bit if set
  if ((READ_IO16_TCO(R_PCH_TCO1_STS)) & B_PCH_TCO1_STS_NMI2SMI) {
    // check port 0x61
    if (READ_IO8(R_PCH_NMI_SC) & B_PCH_NMI_SC_SERR_NMI_STS) {
        SET_IO8(R_PCH_NMI_SC, B_PCH_NMI_SC_PCI_SERR_EN);
        RESET_IO8(R_PCH_NMI_SC, B_PCH_NMI_SC_PCI_SERR_EN);
    }
  }

  // Restore NMI_EN
  WRITE_IO8(CMOS_ADDR_PORT, SavePort70);

  return;
  
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure: SBErrLogHandler
//
// Description: South bridge error logging handler.
//
// Input: DispatchHandle  - Handle of dispatch function, for when interfacing
//                          with the parent SMM driver, will be the address of linked
//                          list link in the call back record.
//        DispatchContext - Pointer to the dispatch function's context.
//
// Output: None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SBErrLogHandler (
    IN EFI_HANDLE                       DispatchHandle
)
{

  EFI_STATUS Status = EFI_SUCCESS;
  SB_PCIE_ERROR_LOG_DISPATCH_LINK       *Link;
  SB_PCIE_ERR ErrorInfo;
  UINT8      i;
  UINT8      CapPtr;
  UINT16     PciStatus;
  UINT16     PcieStatus;
  UINT16     DeviceStatus;
  UINT32     DevBaseAddr;

  // Clear SERR#_NMI_STS & NMI2SMI_STS by set Port 61h[2] = 1 then set it to 0.
  SET_IO8(R_PCH_NMI_SC, B_PCH_NMI_SC_PCI_SERR_EN);
  RESET_IO8(R_PCH_NMI_SC, B_PCH_NMI_SC_PCI_SERR_EN);

  for (i = 0; SBPcieBridge[i] != 0xFFFFFFFF; i++)
  {
    DevBaseAddr = SBPcieBridge[i];
    if (READ_MEM32(DevBaseAddr) == 0xFFFFFFFF)
      continue;

    PciStatus  = READ_MEM16(DevBaseAddr + PCI_STS);
    PcieStatus = READ_MEM16(DevBaseAddr + PCI_SECSTATUS);

    CapPtr = SbFindCapPtr(DevBaseAddr, PCI_CAP_ID_PCIEXP);
    if (CapPtr != 0)
      DeviceStatus = READ_MEM16(DevBaseAddr + CapPtr + PCIE_DEV_STA_OFFSET);

    if ((PciStatus & (BIT8 | BIT15)) || (PcieStatus & (BIT8 | BIT15)))
      ErrorInfo.ParityError = TRUE;
    else
      ErrorInfo.ParityError = FALSE;

    if ((PciStatus & BIT14) || (PcieStatus & BIT14))
      ErrorInfo.SystemError = TRUE;
    else
      ErrorInfo.SystemError = FALSE;

    if ((ErrorInfo.ParityError) || (ErrorInfo.SystemError)) {
      ErrorInfo.PcieAddress = DevBaseAddr;
      ErrorInfo.Bus = (DevBaseAddr >> 20) & ((UINT8)((PCIEX_LENGTH >> 20) - 1));
      ErrorInfo.Dev = (DevBaseAddr >> 15) & 0x1F;
      ErrorInfo.Fun = (DevBaseAddr >> 12) & 0x07;
      ErrorInfo.VendorId   = READ_MEM16(DevBaseAddr + PCI_VID);
      ErrorInfo.DeviceId   = READ_MEM16(DevBaseAddr + PCI_DID);
      ErrorInfo.PciCommand = READ_MEM16(DevBaseAddr + PCI_CMD);
      ErrorInfo.PciCCode   = READ_MEM16(DevBaseAddr + PCI_SCC);
      ErrorInfo.BridgeControl = READ_MEM16(DevBaseAddr + PCI_BRIDGE_CNTL);
      ErrorInfo.Version  = READ_MEM8(DevBaseAddr + CapPtr + PCIE_CAP_OFFSET) & 0x0F;
      ErrorInfo.PortType   = (UINT32)((READ_MEM8(DevBaseAddr + CapPtr + PCIE_CAP_OFFSET) & 0xF0) >> 4);

    if (CapPtr != 0) {
      ErrorInfo.Correctable = (DeviceStatus & BIT0)? TRUE : FALSE;
      ErrorInfo.NonFatal = (DeviceStatus & BIT1)? TRUE : FALSE;
      ErrorInfo.Fatal = (DeviceStatus & BIT2)? TRUE : FALSE;
    }

    for ( Link = gSbPcieErrorLogDispatchHead; Link; Link = Link->Link ) {
      Link->Function(Link, ErrorInfo);
    }

    // Clear Error status
    WRITE_MEM16(DevBaseAddr + PCI_STS, PciStatus);
    WRITE_MEM16(DevBaseAddr + PCI_SECSTATUS, PcieStatus);

    if (CapPtr != 0)
      // Clear Error Status
      WRITE_MEM16(DevBaseAddr + CapPtr + PCIE_DEV_STA_OFFSET, DeviceStatus);

#if defined SB_ELOG_GENERATE_NMI_NOW_SUPPORT && SB_ELOG_GENERATE_NMI_NOW_SUPPORT == 1
    if (!ErrorInfo.Correctable)
      ElogGenerateNmiNow();
#endif 

    } // if ((ErrorInfo.ParityError) || (ErrorInfo.SystemError))
  }
}


//----------------------------------------------------------------------------
VOID InitSbSmiLogic(VOID)
{
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure: InitSbSmiLogicHandler
//
// Description: Initialize SB SMI Logic Handler.
//              This handler should be performed in ready to boot
//              and S3 resume.
//
// Input: DispatchHandle  - Handle of dispatch function, for when interfacing
//                          with the parent SMM driver, will be the address of linked
//                          list link in the call back record.
//        DispatchContext - Pointer to the dispatch function's context.
//
// Output: None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS InitSbSmiLogicHandler (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL )
{
  //UINT32        PmBase;
  UINT32        Buffer32;
  UINT16        TcoBase;

  //PmBase = READ_PCI16_SB (SB_REG_PMBASE) & 0xFF80;
  PchTcoBaseGet (&TcoBase);
  
  //
  //Every SERR (System Erors) will generate NMI. So route NMI as a SMI to handle Errors
  //Steps to route NMI as a SMI
  //Enable NMI2SMI_EN bit in TCO1 Control Register
  //UnMask NMI Enable bit in NMI Enable (and Real Time Clock Index) Register
  //

  // Set NMI2SMI_EN = '1b', TCO_BASE + 08h[9]
  //SET_IO16_PM(TCO1_CNT_OFFSET, BIT09);
  SET_IO16(TcoBase + R_PCH_TCO1_CNT, BIT09);

  // Enable NMI by set Port 70h[7] = '0b'
  SwitchAlternateAccessMode (TRUE);             //Improve alternate access mode
  RESET_IO8(CMOS_ADDR_PORT, BIT07);
  SwitchAlternateAccessMode (FALSE);            //Improve alternate access mode

  //
  //Clear all Spurious Sources of the SMI.
  //  
  Buffer32 = READ_IO32_PM(R_PCH_SMI_STS);
  WRITE_IO32_PM(R_PCH_SMI_STS, Buffer32);

  //Buffer32 = READ_IO32_PM(TCO1_STS_OFFSET);
  Buffer32 = READ_IO32(TcoBase + R_PCH_TCO1_STS);
  //WRITE_IO32_PM(TCO1_STS_OFFSET, Buffer32);
  WRITE_IO32(TcoBase + R_PCH_TCO1_STS, Buffer32);

  // Disable and clear SERR# NMIs
  SET_IO8(R_PCH_NMI_SC, BIT02);

  // Enable SERR# NMIs
  RESET_IO8(R_PCH_NMI_SC, BIT02);

  return EFI_SUCCESS;
}

EFI_SB_PCIE_ERROR_LOG_DISPATCH_PROTOCOL gEfiSbPcieErrorLogDispatchProtocol = \
                                   {SbPcieErrorLogRegister, SbPcieErrorLogUnregister};



/**
    This function is called from SMM during SMM registration.

        
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS

**/
EFI_STATUS InSmmFunction(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS     Status;
    EFI_HANDLE     Handle = NULL;
    EFI_HANDLE     DummyHandle = NULL;
#if SB_PCIE_ERROR_LOG_SUPPORT
    UINTN          Size = sizeof(SETUP_DATA);
    EFI_GUID       SetupGuid = SETUP_GUID;
#endif
    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;
    
    Status = pSmmBase2->GetSmstLocation( pSmmBase2, &pSmst2);
    if (EFI_ERROR(Status)) return Status;
    
    Status = pSmst2->SmmLocateProtocol( &gEfiSmmSwDispatch2ProtocolGuid, \
                                         NULL, \
                                         (VOID **)&SwDispatch );
    if (EFI_ERROR(Status)) return Status;
    
    Status = pSmst2->SmmLocateProtocol ( &gPchTcoSmiDispatchProtocolGuid, \
                                         NULL, \
                                         (VOID **) &mPchTcoSmiDispatchProtocol);
    if (EFI_ERROR(Status)) return Status;    

#if SB_PCIE_ERROR_LOG_SUPPORT
    
    Status = pRS->GetVariable ( L"Setup", \
                                &SetupGuid, \
                                NULL,\
                                &Size, \
                                &gSetupData );
#endif
    
#if ELOG_PROTOCOL
    Status = pBS->LocateProtocol( &gElogProtocolGuid,
                                  NULL,
                                  (VOID **)&GenericElogProtocol );
#endif    
    if (!EFI_ERROR (Status)) {

      Status = SwDispatch->Register( SwDispatch, \
                                     InitSbSmiLogicHandler, \
                                     &SbErrorLogS3PatchContext, \
                                     &Handle );
      if (EFI_ERROR(Status)) return Status;
#if SB_PCIE_ERROR_LOG_SUPPORT      
      if (gSetupData.RtSystemEnable){

      Status = mPchTcoSmiDispatchProtocol->NmiRegister( mPchTcoSmiDispatchProtocol, \
                                                        SBErrLogHandler, \
                                                        &Handle );
      ASSERT_EFI_ERROR (Status);
#endif      
      
      //
      // Enable TCO_EN.
      //
      SET_IO16_PM(R_PCH_SMI_EN, B_PCH_SMI_EN_TCO);
      
      Status = pSmst2->SmmInstallProtocolInterface( &DummyHandle, \
                                                    &gSbPcieErrorLogDispatchProtocolGuid, \
                                                    EFI_NATIVE_INTERFACE, \
                                                    &gEfiSbPcieErrorLogDispatchProtocol );
#if SB_PCIE_ERROR_LOG_SUPPORT
      }
#endif	  
    }

    return Status;
}

/**
    This function is called from outside of SMM during SMM registration.

        
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS

**/
EFI_STATUS NotInSmmFunction(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{

    return EFI_SUCCESS;
}

/**
    This is the entrypoint of the Power button driver.

        
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS

**/
EFI_STATUS PcieErrorLogEntry (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS    Status;

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&pSmmBase2);
    if (EFI_ERROR(Status)) return Status;

    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}
