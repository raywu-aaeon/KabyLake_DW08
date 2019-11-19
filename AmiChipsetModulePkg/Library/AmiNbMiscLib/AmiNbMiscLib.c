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

/** @file AmiNbMiscLib.c
    North Bridge Miscellaneous functions Library implementation

*/

#include <Hob.h>
#include <Nb.h>
#include <SaRegs.h>
#include <AmiCspLib.h>
#include <Library/AmiNbMiscLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAddressEncodeDecodeLib.h>
#include <Protocol/MemInfo.h>
#include <MemInfoHob.h>
#include <PciBus.h>

// Function Definition(s)
/**
    This function issues a CPU only reset.

    @param VOID

    @retval VOID
**/
VOID NbResetCpuOnly (
    VOID
)
{
    // Porting Required.
}

/**
    Returns the base address of TSEG.

    @param VOID

    @retval The Base Address of TSEG.
**/
UINT32 NBGetTsegBase (VOID)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to get
    base Address of TSEG
**** PORTING REQUIRED ****/
    // PCI Device 0 Offset B8h bits 31:20
    return (MmioRead32 ((UINTN) NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_TSEGMB)) & 0xFFF00000);
}

/**
    Check if Vtd is supported.

    @param VOID

    @retval TRUE  Vtd is supported
    @retval FALSE Vtd is not supported
**/
BOOLEAN NbCheckVtdSupport (
    VOID
)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to know
    if Vtd is supported or not
**** PORTING REQUIRED ****/
    // PCI Device 0 Offset E4h bits 23:23
    return !(MmioRead32 ((UINTN) NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_MC_CAPID0_A_OFFSET)) & BIT23);
}

/**
    Check if the system is Dual Channel or not

    @param MemoryChannelType  0:Symmetric, 1:Interleave, 2:Single

    @retval TRUE  It is Dual Channel
    @retval FALSE It is not Dual Channel
**/
BOOLEAN NbIsDualChannel (
    IN UINT8              MemoryChannelType 
)
{
/**** PORTING REQUIRED ****
    Please use registers or other way to check if 
    the system is Dual Channel or not
**** PORTING REQUIRED ****/
///(EIP190855)>
    EFI_HOB_GUID_TYPE       *GuidHob;
    VOID                    *HobList;
    VOID                    *HobData;
    MEMORY_INFO_DATA_HOB    *mMemInfo;
    UINT8                   Ch;
    UINT8                   Dimm;
    UINT32                  ChannelSize[CH_NUM];
    UINT32                  DimmCapacity[DIMM_NUM];
    UINT8                   ChannelType;

    //
    // Set default Memory Channel-Type is "Dual-Channel Symmetric Mode".
    //
    ChannelType = 0;

    //
    // Get the HOB list.  If it is not present, then ASSERT.
    //
    GuidHob = NULL;
    HobData = NULL;
    mMemInfo = NULL;
    HobList = GetHobList ();
    ASSERT (HobList != NULL);

    //
    // Search for the Memory Configuration GUID HOB.  If it is not present, then
    // there's nothing we can do.
    //
    GuidHob = GetNextGuidHob (&gSiMemoryInfoDataGuid, HobList);
    if (GuidHob != NULL) {
        HobData   = GET_GUID_HOB_DATA (GuidHob);
        mMemInfo  = (MEMORY_INFO_DATA_HOB *) HobData;
    } else { // MemInfoHob Data HOB not available
        return FALSE;
    }

    //
    // Getting the Dimm Capacity
    //
    for (Ch = 0; Ch < CH_NUM; Ch++) {
      for (Dimm = 0; Dimm < DIMM_NUM; Dimm++) {
          DimmCapacity[Dimm] = (UINT32) mMemInfo->Controller[0].ChannelInfo[Ch].DimmInfo[Dimm].DimmCapacity;
      }
      ChannelSize[Ch] = DimmCapacity[0] + DimmCapacity[1];
    }
    
    //
    // Detect System Memory Organization Modes
    //
    if((ChannelSize[0] != 0) && (ChannelSize[1] != 0)) { // is Dual-Channel Symmetric Mode
        if((ChannelSize[0] == ChannelSize[1])) {
            ChannelType = 0;
        } else if(ChannelSize[0] > ChannelSize[1]) { // is Dual-Channel Mode - Intel Flex Memory Technology Mode
            ChannelType = 1;
        } else {
            return FALSE;
        }
    } else if(ChannelSize[0] || ChannelSize[1]) { // is Single-Channel Mode
        return FALSE;
    } else {
        return FALSE;
    }

    //
    // Check the input Channel-Type and detected Channel-Type are same or not.
    //
    if(MemoryChannelType == ChannelType) return TRUE;

    return FALSE;
///(EIP190855)<
}

/**
    Lock and unlock Pavpc

    @param Mode               TRUE: Lock Pavpc; FALSE: Unlock Pavpc

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to lock/unlock Pavpc

**/
EFI_STATUS NbLockPavpc (
    IN BOOLEAN            Mode
)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to know
    how to lock and unlock Pavpc.
    If it is not supported by the chip, please return EFI_UNSUPPORTED.
**** PORTING REQUIRED ****/
    UINT32       Pavpc;

    if (MmioRead32 (IGD_REG (R_SA_IGD_VID)) != 0xFFFFFFFF) {

       Pavpc = MmioRead32 (NB_PCIE_CFG_ADDRESS (0, 0, 0, R_SA_PAVPC));

       if (Mode) {
          ///
          /// Lock PAVPC Register
          ///
          Pavpc |= B_SA_PAVPC_PAVPLCK_MASK;
       } else {
          ///
          /// UnLock PAVPC Register
          ///
          Pavpc &= ~B_SA_PAVPC_PAVPLCK_MASK; 
       }

       MmioWrite32 (NB_PCIE_CFG_ADDRESS (0, 0, 0, R_SA_PAVPC), Pavpc); // WRITE_PCI32_IGD(R_SA_PAVPC, Pavpc);

       return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}

/**
    Get the information of the DIMM location indicated by MemoryAddr

    @param MemoryAddr         The system address to convert
    @param NbAddressDecode    Pointer to the buffer used to store NB_ADDRESS_DECODE

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to get the information of DIMM location

**/
EFI_STATUS NbGetDimmLocInfo (
    IN CONST UINTN        MemoryAddr,
    OUT NB_ADDRESS_DECODE *NbAddressDecode
)
{
/**** PORTING REQUIRED ****
    Please check if MRC supports memory address decode.
    If it is not supported by MRC, please return EFI_UNSUPPORTED.
**** PORTING REQUIRED ****/
#ifndef _MEMORY_ADDRESS_ENCODE_DECODE_LIB_H_

  return EFI_UNSUPPORTED;

#else

  EFI_STATUS       Status;
  MRC_DRAM_ADDRESS DramAddress;

  Status = MrcMemoryAddressDecode (
             (UINT64) MemoryAddr, 
             &DramAddress
           );
  ASSERT_EFI_ERROR (Status);

  NbAddressDecode->ChannelNumber = (UINT8) DramAddress.ChannelNumber;
  NbAddressDecode->DimmNumber    = (UINT8) DramAddress.DimmNumber;
  NbAddressDecode->Rank          = (UINT8) DramAddress.Rank;
  NbAddressDecode->Bank          = (UINT8) DramAddress.Bank;
  NbAddressDecode->Ras           = (UINT16) DramAddress.Ras;
  NbAddressDecode->Cas           = (UINT16) DramAddress.Cas;

  return Status;

#endif // _MEMORY_ADDRESS_ENCODE_DECODE_LIB_H_
}

/**
    This function searches the PCI address space for the PCI
    device specified for a particular capability ID and returns
    the offset in the PCI address space if one found

    @param PciAddress 
    @param CapId 

    @retval Capability ID Address if one found
        Otherwise returns 0
**/
UINT32
NbFindCapPtr(
  IN UINT64  PciAddress,
  IN UINT8   CapId
)
{
    UINT8   Value;
    UINT32  Address = (UINT32)PciAddress;

    Address = (Address & 0xffffff00) | 6; //PCI Status Register.
    Value = READ_MEM8(Address + 0);

    if (Value == 0xff) return 0;          // No device.
    if (!(Value & (1 << 4))) return 0;    // Check if capabilities list.
   
    *(UINT8*)&Address = 0x34;             // Register to First capabilities pointer
                                        // if 0, then capabilities
    for(;;) {
        Value = READ_MEM8(Address + 0);
        if (Value == 0) return 0;

        *(UINT8*)&Address = Value;          // PciAddress = ptr to CapID
        Value = READ_MEM8(Address + 0);     // New cap ptr.

        //If capablity ID, return register that points to it.
        if (Value == CapId) return Address; 

        ++Address;                          // Equals to next capability pointer.
    }
}

/**
    This function is Retrain Link NB Pci Device.

    @param PciBus PCI Bus Number.
    @param PciDev PCI Device Number.
    @param PciFun PCI Function Number.
    @param PciCapPtr PCI CapPtr Number.

    @retval VOID

**/
VOID NbRetrainLinkPciDevice (
    IN UINT8                PciBus,
    IN UINT8                PciDev,
    IN UINT8                PciFun,
    IN UINT8                CapPtr )
{
    // Disable Link
    SET_PCI8(PciBus, PciDev, PciFun, CapPtr + 0x10, BIT04);

    // Retrain Link
    RW_PCI8(PciBus, PciDev, PciFun, CapPtr + 0x10 , BIT05, BIT04);

    // Wait Link States
    while (READ_PCI16(PciBus, PciDev, PciFun, CapPtr + 0x12) & BIT11);

    // if Retrain Link Anyway 1, Clear Retrain Link
    if (READ_PCI8(PciBus, PciDev, PciFun, CapPtr + 0x10) & BIT05) {
       RESET_PCI8(PciBus, PciDev, PciFun, CapPtr + 0x10 , BIT05);
    }

    // Wait Link States
    while (READ_PCI16(PciBus, PciDev, PciFun, CapPtr + 0x12) & BIT11);
}


/**
    This function is called by PCI Bus Driver before configuring
    or disabling any PCI device. This function should examine the
    Vendor/Device ID or PCI Bus, Device and Function numbers to
    make sure it is not a north bridge device or any other device
    which should no be configured by PCI Bus Driver.

    @param PciDevice Pointer to PCI Device Info structure.

        EFI_STATUS
    @retval EFI_SUCCESS SKIP this device, do not touch
        PCI Command register.
    @retval EFI_UNSUPPORTED DON'T SKIP this device do complete
        enumeration as usual.
**/

EFI_STATUS NBProtectedPciDevice (
    IN VOID     *PciDevice)
{
    PCI_DEV_INFO    *PciDev;
    
    PciDev = (PCI_DEV_INFO*)PciDevice;
    
    if ((PciDev->Address.Addr.Bus == 0) && (PciDev->Address.Addr.Device == 0x1F) && (PciDev->Address.Addr.Function == 0x05))
    {
        return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}

/**
    This function is called by PCI Bus Driver before installing
    Protocol Interface for the input device.

    @param PciDevice Pointer to PCI Device Info structure.

    @retval EFI_SUCCESS

    @note  All resource in the device had been assigned, but the command
              register is disabled.
**/

EFI_STATUS NBProgramPciDevice (
    IN VOID     *PciDevice )
{
//####    PCI_DEV_INFO    *PciDev;    
//####    PciDev = (PCI_DEV_INFO*)PciDevice;
    
//####if ((PciDev->Address.Addr.Bus == NB_BUS) && \
//####    (PciDev->Address.Addr.Device == NB_DEV) && \
//####    (PciDev->Address.Addr.Function == NB_FUN)) {
//####    // Do any porting if needed.
//####}

    return EFI_SUCCESS;
}

/**
    This function is called by PCI Bus Driver, can be used to
    the attributes of the PCI device.

    @param PciDevice Pointer to PCI Device Info structure.
    @param Attributes Attributes bitmask which caller whants to
        change.
    @param Capabilities The PCI device supports Capabilityes
    @param Set Specifies weathere to set or reset given
        "Attributes".

    @retval EFI_SUCCESS

    @note  This routine may be invoked twice depend on the device type,
              the first time is at BDS phase, the second is before
              legacy boot.
**/

EFI_STATUS NBUpdatePciDeviceAttributes (
    IN VOID                     *PciDevice,
    IN OUT UINT64               *Attributes,
    IN UINT64                   Capabilities,
    IN BOOLEAN                  Set )
{
//####    PCI_DEV_INFO    *PciDev;    
//####    PciDev = (PCI_DEV_INFO*)PciDevice;
    
    return EFI_SUCCESS;
}

EFI_STATUS NBRemovePciDevice (
    IN VOID     *PciDevice,
    IN UINTN            Count,
    IN UINTN            LowResType
    )
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINT32 i;
    PCI_DEV_INFO    *Device;
    
    Device = (PCI_DEV_INFO*)PciDevice;

    if (Device->Address.Addr.Bus > 0)
    {
        for(i=0; i <= PCI_MAX_BAR_NO; i++)
        {
            if((Device->Bar[i].Type>0) && (Device->Bar[i].Length>0))
            {           
                DEBUG((DEBUG_INFO,"BAR Index=%d;\tType=%d;\tGRA=0x%lX;\tLEN=0x%lX;\tOffset=0x%X;\n", 
                       i, Device->Bar[i].Type, Device->Bar[i].Gran, Device->Bar[i].Length, Device->Bar[i].Offset)); 
                          
                if (Device->Bar[i].Length >= 0xE0000000) // if 3.5G
                {
#if defined (AUTO_ENABLE_ABOVE_4G_PCI_DECODE_IN_OUT_OF_RESOURCES) && AUTO_ENABLE_ABOVE_4G_PCI_DECODE_IN_OUT_OF_RESOURCES == 1
{
                    SETUP_DATA             *SetupData = NULL;
                    EFI_GUID               SetupGuid  = SETUP_GUID;
                    UINTN                  VarSize = 0;
                    UINT32                 VarAttributes = 0;
                    EFI_STATUS             VarStatus;

                    VarStatus = GetEfiVariable(L"Setup", &SetupGuid, &VarAttributes, &VarSize, &SetupData);
                    if (!EFI_ERROR(VarStatus)) {
                        if (SetupData->EnableAbove4GBMmio == 0) {
                            SetupData->EnableAbove4GBMmio = 1;
                            VarStatus = pRS->SetVariable (L"Setup", &SetupGuid, VarAttributes, sizeof(SETUP_DATA), SetupData);
                            if (!EFI_ERROR(VarStatus))   pRS->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
                        }
                    }
}
#endif
                    Status = EFI_SUCCESS; 
                    break;
                }
            }
        }
    }
    return Status;
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
