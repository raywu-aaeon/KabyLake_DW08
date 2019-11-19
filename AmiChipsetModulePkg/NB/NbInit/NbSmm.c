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
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Chipset/NBSmm.c 1     2/08/12 4:34a Yurenlai $
//
// $Revision: 1 $
//
// $Date: 2/08/12 4:34a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Chipset/NBSmm.c $
// 
//*************************************************************************
/** @file NbSmm.c
    This file contains code for SMM access and control (both the
    protocol defined by Framework

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <Dxe.h>
#include <PiDxe.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SmmAccess2.h>
#include <Guid/HobList.h>
#include <Guid/SmramMemoryReserve.h>
#include <SaRegs.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)
#define SMM_ACCESS_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('4', '5', 's', 'a')

// Macro Definition(s)
#define SMM_ACCESS_PRIVATE_DATA_FROM_THIS(a)   BASE_CR (a, SMM_ACCESS_PRIVATE_DATA, SmmAccess)

// Type Definition(s)
typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;
  EFI_SMM_ACCESS2_PROTOCOL        SmmAccess;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;

  ///
  /// Local Data for SMM Access interface goes here
  ///
  UINTN                           NumberRegions;
  EFI_SMRAM_DESCRIPTOR            *SmramDesc;
} SMM_ACCESS_PRIVATE_DATA;

// Function Prototype(s)

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)
static SMM_ACCESS_PRIVATE_DATA  mSmmAccess;

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//----------------------------------------------------------------------------

#if defined NB_SMM_ACCESS2_PROTOCOL_SUPPORT && NB_SMM_ACCESS2_PROTOCOL_SUPPORT == 1
/**
    This routine accepts a request to "open" a region of SMRAM.  The
    region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
    The use of "open" means that the memory is visible from all boot-service
    and SMM agents.
    @param This Pointer to the SMM Access Interface.

        Return Status based on errors
    @retval EFI_SUCCESS The region was successfully opened.
    @retval EFI_DEVICE_ERROR The region could not be opened because locked by chipset.
    @retval EFI_INVALID_PARAMETER The descriptor index was out of bounds.
**/
EFI_STATUS
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  )
{
    EFI_STATUS                Status;
    SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
    UINTN                     Index;
    UINT64                    Address;
    UINT8                     SmramControl;    
      
    SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);
    
    for (Index = 0; Index < SmmAccess->NumberRegions; Index ++) 
    {
      if (SmmAccess->SmramDesc[Index].RegionState & EFI_SMRAM_LOCKED) 
      {       
        DEBUG((DEBUG_INFO, "Cannot open a locked SMRAM region\n"));
        continue;
      }
      ///
      /// BEGIN CHIPSET SPECIFIC CODE
      ///
      ///
      /// SMRAM register is PCI 0:0:0:88, SMRAMC (8 bit)
      ///
      Address = NB_PCI_CFG_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC);
      
      Status = SmmAccess->PciRootBridgeIo->Pci.Read (
                                                SmmAccess->PciRootBridgeIo,
                                                EfiPciWidthUint8,
                                                Address,
                                                1,
                                                &SmramControl
                                                );
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Read returned %r\n", Status));
        return Status;
      }
      ///
      ///  Is SMRAM locked?
      ///
      if ((SmramControl & B_SA_SMRAMC_D_LCK_MASK) != 0) {
        ///
        /// Cannot Open a locked region
        ///
        SmmAccess->SmramDesc[Index].RegionState |= EFI_SMRAM_LOCKED;
        DEBUG((DEBUG_INFO, "Cannot open a locked SMRAM region\n"));
        return EFI_DEVICE_ERROR;
      }
      ///
      /// Open SMRAM region
      ///
      SmramControl |= B_SA_SMRAMC_D_OPEN_MASK;
      SmramControl &= ~(B_SA_SMRAMC_D_CLS_MASK);
      
      Status = SmmAccess->PciRootBridgeIo->Pci.Write (
                                                SmmAccess->PciRootBridgeIo,
                                                EfiPciWidthUint8,
                                                Address,
                                                1,
                                                &SmramControl
                                                );
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Write returned %r\n", Status));
        return Status;
      }
      ///
      /// END CHIPSET SPECIFIC CODE
      ///
      SmmAccess->SmramDesc[Index].RegionState &= ~(EFI_SMRAM_CLOSED |EFI_ALLOCATED);
      SmmAccess->SmramDesc[Index].RegionState |= EFI_SMRAM_OPEN;      
    }     
    SmmAccess->SmmAccess.OpenState = TRUE;
    
    return EFI_SUCCESS;
}

/**
    This routine accepts a request to "close" a region of SMRAM.  The
    region could be legacy AB or TSEG near top of physical memory.
    The use of "close" means that the memory is only visible from SMM agents,
    not from BS or RT code.
    @param This Pointer to the SMM Access Interface.

        Return Status based on errors
    @retval EFI_SUCCESS The region was successfully closed.
    @retval EFI_DEVICE_ERROR The region could not be closed because locked by chipset.
    @retval EFI_INVALID_PARAMETER The descriptor index was out of bounds.
**/
EFI_STATUS
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  )
{
    EFI_STATUS                Status;    
    SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
    UINTN                     Index;
    UINT64                    Address;
    UINT8                     SmramControl;    
    
    SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);
    
    for (Index = 0; Index < SmmAccess->NumberRegions; Index ++) 
    {
      if (SmmAccess->SmramDesc[Index].RegionState & EFI_SMRAM_LOCKED) 
      {       
        DEBUG((DEBUG_INFO, "Cannot close a locked SMRAM region\n"));
        continue;
      }
      if (SmmAccess->SmramDesc[Index].RegionState & EFI_SMRAM_CLOSED) 
      {
        continue;
      }
      ///
      /// BEGIN CHIPSET SPECIFIC CODE
      ///      
      ///
      /// SMRAM register is PCI 0:0:0:88, SMRAMC (8 bit)
      ///
      Address = NB_PCI_CFG_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC);
      
      Status = SmmAccess->PciRootBridgeIo->Pci.Read (
                                                SmmAccess->PciRootBridgeIo,
                                                EfiPciWidthUint8,
                                                Address,
                                                1,
                                                &SmramControl
                                                );
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Read returned %r\n", Status));
        return Status;
      }
      ///
      ///  Is SMRAM locked?
      ///
      if ((SmramControl & B_SA_SMRAMC_D_LCK_MASK) != 0) {
        ///
        /// Cannot Close a locked region
        ///
        SmmAccess->SmramDesc[Index].RegionState |= EFI_SMRAM_LOCKED;
        DEBUG((DEBUG_INFO, "Cannot close a locked SMRAM region\n"));
        return EFI_DEVICE_ERROR;
      }
      ///
      /// Close SMRAM region
      ///
      SmramControl &= ~(B_SA_SMRAMC_D_OPEN_MASK);
      
      Status = SmmAccess->PciRootBridgeIo->Pci.Write (
                                                SmmAccess->PciRootBridgeIo,
                                                EfiPciWidthUint8,
                                                Address,
                                                1,
                                                &SmramControl
                                                );
      
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Write returned %r\n", Status));
        return Status;
      }
      ///
      /// END CHIPSET SPECIFIC CODE
      ///
      SmmAccess->SmramDesc[Index].RegionState &= ~EFI_SMRAM_OPEN;
      SmmAccess->SmramDesc[Index].RegionState |= (EFI_SMRAM_CLOSED |EFI_ALLOCATED);;      
    }       
    SmmAccess->SmmAccess.OpenState = FALSE;
    return EFI_SUCCESS;
}

/**
    This routine accepts a request to "lock" SMRAM.  The
    region could be legacy AB or TSEG near top of physical memory.
    The use of "lock" means that the memory can no longer be opened
    to BS state..
    @param This Pointer to the SMM Access Interface.

        Return Status based on errors
    @retval EFI_SUCCESS The region was successfully locked.
    @retval EFI_DEVICE_ERROR The region could not be locked because locked by chipset.
    @retval EFI_INVALID_PARAMETER The descriptor index was out of bounds.
**/
EFI_STATUS
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  )
{
    EFI_STATUS                Status;    
    SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
    UINTN                     Index;
    UINT64                    Address;
    UINT8                     SmramControl;    
    
    SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);
    
    for (Index = 0; Index < SmmAccess->NumberRegions; Index ++) 
    {
      if (SmmAccess->SmmAccess.OpenState) 
      {
        DEBUG((DEBUG_INFO, "Cannot lock SMRAM when SMRAM regions are still open\n"));
        return EFI_DEVICE_ERROR;
      }
      ///
      /// BEGIN CHIPSET SPECIFIC CODE
      ///           
      ///
      /// SMRAM register is PCI 0:0:0:88, SMRAMC (8 bit)
      ///
      Address = NB_PCI_CFG_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC);
      
      Status = SmmAccess->PciRootBridgeIo->Pci.Read (
                                                SmmAccess->PciRootBridgeIo,
                                                EfiPciWidthUint8,
                                                Address,
                                                1,
                                                &SmramControl
                                                );
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Read returned %r\n", Status));
        return Status;
      }
      ///
      /// Lock the SMRAM
      ///
      SmramControl |= B_SA_SMRAMC_D_LCK_MASK;
      
      Status = SmmAccess->PciRootBridgeIo->Pci.Write (
                                                SmmAccess->PciRootBridgeIo,
                                                EfiPciWidthUint8,
                                                Address,
                                                1,
                                                &SmramControl
                                                );
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Write returned %r\n", Status));
        return Status;
      }
      ///
      /// END CHIPSET SPECIFIC CODE
      ///
      SmmAccess->SmramDesc[Index].RegionState |= EFI_SMRAM_LOCKED;
    }  
    SmmAccess->SmmAccess.LockState = TRUE; 
    
    return EFI_SUCCESS;
}

/**
    This routine services a user request to discover the SMRAM
    capabilities of this platform.  This will report the possible
    ranges that are possible for SMRAM access, based upon the 
    memory controller capabilities.
    @param This Pointer to the SMRAM Access Interface.
    @param SmramMapSize Pointer to the variable containing size of the
        buffer to contain the description information.
    @param SmramMap Buffer containing the data describing the Smram region 
        descriptors.
        Return Status based on errors
    @retval EFI_BUFFER_TOO_SMALL The user did not provide a sufficient buffer.
    @retval EFI_SUCCESS The user provided a sufficiently-sized buffer.
**/
EFI_STATUS
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL     *This,
  IN OUT UINTN                          *SmramMapSize, 
  IN OUT EFI_SMRAM_DESCRIPTOR           *SmramMap
  )
{
    EFI_STATUS                Status;
    SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
    UINTN                     NecessaryBufferSize;
    
    SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);
    
    NecessaryBufferSize = SmmAccess->NumberRegions * sizeof(EFI_SMRAM_DESCRIPTOR);
    
    if (*SmramMapSize < NecessaryBufferSize) {
      DEBUG((DEBUG_INFO, "SMRAM Map Buffer too small\n"));
      Status = EFI_BUFFER_TOO_SMALL;
    } else {
      pBS->CopyMem(SmramMap, SmmAccess->SmramDesc, NecessaryBufferSize);
      DEBUG((DEBUG_INFO, "SMRAM Map Buffer installed complete\n"));
      Status = EFI_SUCCESS;
    }
        
    *SmramMapSize = NecessaryBufferSize;
    
    return Status;
}

/**
    Install an SMM Access2 Protocol

    @param ImageHandle Image handle
        SystemTable Pointer to the system table

    @retval Return Status based on errors
**/

EFI_STATUS InstallSmmAccess2Protocol (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable)
{
    EFI_STATUS                          Status;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo;
    UINTN                               Index;
    EFI_SMRAM_HOB_DESCRIPTOR_BLOCK      *DescriptorBlock;
    EFI_PEI_HOB_POINTERS                *Hob;

    //
    // Initialize Global variables
    //
    pBS->SetMem (&mSmmAccess, sizeof (mSmmAccess), 0);      

    Status = pBS->LocateProtocol (
                    &gEfiPciRootBridgeIoProtocolGuid,
                    NULL,
                    (VOID **) &PciRootBridgeIo
                    );

    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "Could not locate PCI Root Bridge IO Protocol\n"));
      return EFI_NOT_FOUND;
    }
    mSmmAccess.Signature       = SMM_ACCESS_PRIVATE_DATA_SIGNATURE;
    mSmmAccess.Handle          = NULL;
    mSmmAccess.PciRootBridgeIo = PciRootBridgeIo;
    ///
    /// Get Hob list
    ///
    Hob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
    if (Hob == NULL) {
      DEBUG((DEBUG_ERROR, "SmramMemoryReserve HOB not found\n"));
      return EFI_NOT_FOUND;
    }
        
    DescriptorBlock = (VOID *) ((UINT8 *) Hob + sizeof (EFI_HOB_GUID_TYPE));

    ///
    /// Alloc space for mSmmAccess.SmramDesc
    ///
    mSmmAccess.SmramDesc = AllocateZeroPool ((DescriptorBlock->NumberOfSmmReservedRegions) * sizeof (EFI_SMRAM_DESCRIPTOR));
    if (mSmmAccess.SmramDesc == NULL) {
      DEBUG((DEBUG_ERROR, "Alloc mSmmAccess.SmramDesc fail.\n"));
      return EFI_OUT_OF_RESOURCES;
    }
    
    ///
    /// Use the HOB to publish SMRAM capabilities
    ///
    for (Index = 0; Index < DescriptorBlock->NumberOfSmmReservedRegions; Index++) {
      mSmmAccess.SmramDesc[Index].PhysicalStart = DescriptorBlock->Descriptor[Index].PhysicalStart;
      mSmmAccess.SmramDesc[Index].CpuStart      = DescriptorBlock->Descriptor[Index].CpuStart;
      mSmmAccess.SmramDesc[Index].PhysicalSize  = DescriptorBlock->Descriptor[Index].PhysicalSize;
      mSmmAccess.SmramDesc[Index].RegionState   = DescriptorBlock->Descriptor[Index].RegionState;
    }

    mSmmAccess.NumberRegions               = Index;
    mSmmAccess.SmmAccess.Open              = Open;
    mSmmAccess.SmmAccess.Close             = Close;
    mSmmAccess.SmmAccess.Lock              = Lock;
    mSmmAccess.SmmAccess.GetCapabilities   = GetCapabilities;
    mSmmAccess.SmmAccess.LockState         = FALSE;
    mSmmAccess.SmmAccess.OpenState         = FALSE;

    //
    // Install our protocol interfaces on the device's handle
    //
    Status = pBS->InstallMultipleProtocolInterfaces (
                    &mSmmAccess.Handle,
                    &gEfiSmmAccess2ProtocolGuid,
                    &mSmmAccess.SmmAccess,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "InstallMultipleProtocolInterfaces returned %r\n", Status));
      return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;    
}
#endif

/**
    This function is invoked from NB DXE to initialize SMM
    related stuff in NorthBridge and install appropriate
    SMM protocols such as SMM Access & SMM Control

    @param ImageHandle Image handle
        SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.
**/

EFI_STATUS NbSmmInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable)
{
    EFI_STATUS      Status = EFI_SUCCESS;

#if defined NB_SMM_ACCESS2_PROTOCOL_SUPPORT && NB_SMM_ACCESS2_PROTOCOL_SUPPORT == 1    
    Status = InstallSmmAccess2Protocol (ImageHandle, SystemTable);
#endif
    
    return Status;
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
