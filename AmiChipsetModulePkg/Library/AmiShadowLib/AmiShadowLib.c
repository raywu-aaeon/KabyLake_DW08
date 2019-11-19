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

/** @file AmiShadowLib.c
    North Bridge Shadow Library implementation

*/

#include <Library/AmiShadowLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Nb.h>
#include <SaRegs.h>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#if     CSM_SUPPORT

#define ATTR_DISABLED           0       // Shadow RAM Disabled
#define ATTR_READ               1       // Shadow RAM Read Enabled
#define ATTR_WRITE              2       // Shadow RAM Write Enabled
#define ATTR_READ_WRITE         3       // Shadow RAM Read/Write Enabled

EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo;
//----------------------------------------------------------------------------

typedef struct
{
    UINT8   Register;
    UINT8   Mask;
    UINT32  StartAddress;
    UINT32  Length;
} PAM_STRUCT;

/** Porting required for the following structure **/
PAM_STRUCT gPamStruct[] =
{
/*  // [ EIP381157- ]
    {0x91,0xfc,0xc0000,0x4000}, 
    {0x91,0xcf,0xc4000,0x4000}, 
    {0x92,0xfc,0xc8000,0x4000}, 
    {0x92,0xcf,0xcc000,0x4000}, 
    {0x93,0xfc,0xd0000,0x4000}, 
    {0x93,0xcf,0xd4000,0x4000}, 
    {0x94,0xfc,0xd8000,0x4000}, 
    {0x94,0xcf,0xdc000,0x4000}, 
    {0x95,0xfc,0xe0000,0x4000},
    {0x95,0xcf,0xe4000,0x4000},
    {0x96,0xcf,0xec000,0x4000},
    {0x96,0xfc,0xe8000,0x4000},
    {0x90,0xcf,0xf0000,0x10000}
*/
    {R_SA_PAM1,0xfc,0xc0000,0x4000}, 
    {R_SA_PAM1,0xcf,0xc4000,0x4000}, 
    {R_SA_PAM2,0xfc,0xc8000,0x4000}, 
    {R_SA_PAM2,0xcf,0xcc000,0x4000}, 
    {R_SA_PAM3,0xfc,0xd0000,0x4000}, 
    {R_SA_PAM3,0xcf,0xd4000,0x4000}, 
    {R_SA_PAM4,0xfc,0xd8000,0x4000}, 
    {R_SA_PAM4,0xcf,0xdc000,0x4000}, 
    {R_SA_PAM5,0xfc,0xe0000,0x4000},
    {R_SA_PAM5,0xcf,0xe4000,0x4000},
    {R_SA_PAM6,0xcf,0xec000,0x4000},
    {R_SA_PAM6,0xfc,0xe8000,0x4000},
    {R_SA_PAM0,0xcf,0xf0000,0x10000}
};

#define NUM_PAM_ENTRIES (sizeof(gPamStruct) / sizeof(PAM_STRUCT))

//----------------------------------------------------------------------------
// Start OF CSM Related Porting Hooks
//----------------------------------------------------------------------------

//
// The following data structure specifies the PCI device/function number of the root
// bridge(s). Number of entries in this table defined by ROOT_BRIDGE_COUNT
// This table is a missing link between RootBridgeIo and PciIo, which allows to update
// BusNumXlat table with actual bus numbers.
// Each entry in the table is a pair of RootBridge UID (UINT32), provided in RootBridge
// device path, and PCI Dev/Func number (UINT8) that can be used to access Root Bridge on
// PCI bus.
ROOT_BRIDGE_MAPPING_ENTRY   RbMap[ROOT_BRIDGE_COUNT] = {
//  RB ID           Device function number
    {0x00,          (UINT8)((NB_DEV << 3) + NB_FUN)}
    };
UINTN                       RbCount = ROOT_BRIDGE_COUNT;

/**
    Helper function to get the Start and End Index for
    PAM register table.

    @param StartAddress       Start address of the PAM area
    @param Length             Length of the PAM area
    @param StartIndex         Start Index of the PAM (OUT)
    @param EndIndex           End Index of the PAM (OUT)

    @retval EFI_STATUS

**/
EFI_STATUS NbGetPamStartEndIndex(
    IN  UINT32   StartAddress,
    IN  UINT32   Length,
    OUT UINT32  *StartIndex,
    OUT UINT32  *EndIndex
)
{
    UINT32                  StartIdx;
    UINT32                  EndIdx;
    UINT32                  TotalLength = 0;

    if (StartAddress < gPamStruct[0].StartAddress)
        return EFI_INVALID_PARAMETER;

    for(StartIdx = 0; StartIdx < NUM_PAM_ENTRIES; ++StartIdx) {
        if (StartAddress <= gPamStruct[StartIdx].StartAddress) break;
    }
    if (StartAddress < gPamStruct[StartIdx].StartAddress) StartIdx--;

    if (StartIdx == NUM_PAM_ENTRIES) return EFI_INVALID_PARAMETER;

    // Adjust the length of the requested region if starting address is
    // out of bounds.
    Length += (StartAddress - gPamStruct[StartIdx].StartAddress);

    for(EndIdx = StartIdx; EndIdx < NUM_PAM_ENTRIES; ++EndIdx) {
        TotalLength += gPamStruct[EndIdx].Length;
        if (TotalLength >= Length) break;
    }
    if (EndIdx == NUM_PAM_ENTRIES) return EFI_INVALID_PARAMETER;

    *StartIndex = StartIdx;
    *EndIndex = EndIdx;

    return EFI_SUCCESS;
}


/**
    Program 0xc0000 - 0xfffff regions to Lock/Unlock.

    @param gBS            Pointer to the boot services table
    @param gRS            Pointer to the runtime services table
    @param StartAddress   Start address of the PAM area
    @param Length         Length of the PAM area
    @param Setting        Settings to be set for the above area
    @param Granularity    Granularity of the above area (return value)

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Search the structure for the first entry matching 
                 the StartAddress.
              2. If not found, return EFI_INVALID_PARAMETER.
              3. Find the last entry in structure for the region to program,
                 by adding the lengths of the entries.
              4. If not found, return EFI_INVALID_PARAMETER.
              5. Read/Write each register for the entry to set region.
              6. Return the Granularity for the region.
**/
EFI_STATUS
NbProgramPamRegisters(
    IN EFI_BOOT_SERVICES    *gBS,
    IN EFI_RUNTIME_SERVICES *gRS,
    IN UINT32               StartAddress,
    IN UINT32               Length,
    IN UINT8                Setting,
    IN OUT UINT32           *Granularity
)
{
    //NB shadow programming.
    //Note: For this routine to work, the gPamStruct regions must be continuous.

    EFI_STATUS          Status = EFI_SUCCESS;
    UINT32              StartIndex;
    UINT32              EndIndex;
    UINTN               i;
    UINT8               Data;
    UINT8               Shift;

    Status = NbGetPamStartEndIndex( StartAddress, \
                                    Length, \
                                    &StartIndex, \
                                    &EndIndex );
    if (EFI_ERROR(Status)) return Status;


    for (i = StartIndex; i <= EndIndex; ++i) {
        //Bus 0, Device 0, Function 0
        Data = (UINT8) MmioRead32 (NB_PCIE_CFG_ADDRESS(0, 0, 0, gPamStruct[i].Register)); // READ_PCI8_NB(gPamStruct[i].Register)
        Data &= gPamStruct[i].Mask;
        Shift = (gPamStruct[i].Mask == 0xfc) ? 0 : 4;
        switch (Setting) {
            case 0 :            // Read Only
            case 1 :            // Read Only (Permanently)
                    Data |= (ATTR_READ << Shift);
                    break;
            case 2 :            // Read/Write
                    Data |= (ATTR_READ_WRITE << Shift);
                    break;
            case 3 :            // Disabled
            default:
                    break;
        }

        MmioWrite32 (NB_PCIE_CFG_ADDRESS(0, 0, 0, gPamStruct[i].Register), Data); // WRITE_PCI8_NB(gPamStruct[i].Register, Data);

    }

    if (Granularity)
        *Granularity = ( (StartAddress+Length) < 0xf0000 ) ? 0x4000 : 0x10000;

    return EFI_SUCCESS;
}

/**
    Program 0xc0000 - 0xfffff regions to Lock/Unlock during PEI stage

    @param PeiServices   Pointer to the PEI services table
    @param StartAddress  Start address of the PAM area
    @param Length        Length of the PAM area
    @param Setting       Settings to be set for the above area
    @param Granularity   Granularity of the above area (return value)

    @retval EFI_STATUS

**/
EFI_STATUS NbPeiProgramPamRegisters(
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT32           StartAddress,
    IN UINT32           Length,
    IN UINT8            Setting,
    IN OUT UINT32       *Granularity OPTIONAL
)
{
    // NB shadow programming.
    // Note: For this routine to work, the gPamStruct regions must
    // be continuous.

    EFI_STATUS          Status = EFI_SUCCESS;
    UINT32              StartIndex;
    UINT32              EndIndex;
    UINTN               i;
    UINT8               Data;
    UINT8               Shift;

    Status = NbGetPamStartEndIndex( StartAddress, \
                                    Length, \
                                    &StartIndex, \
                                    &EndIndex );
    if (EFI_ERROR(Status)) return Status;


    for (i = StartIndex; i <= EndIndex; ++i) {
        //Bus 0, Device 0, Function 0
        Data = (UINT8) MmioRead32 (NB_PCIE_CFG_ADDRESS(0, 0, 0, gPamStruct[i].Register)); // READ_PCI8_NB(gPamStruct[i].Register);
        Data &= gPamStruct[i].Mask;
        Shift = (gPamStruct[i].Mask == 0xfc) ? 0 : 4;
        switch (Setting) {
            case 0 :            // Read Only
            case 1 :            // Read Only (Permanently)
                    Data |= (ATTR_READ << Shift);
                    break;
            case 2 :            // Read/Write
                    Data |= (ATTR_READ_WRITE << Shift);
                    break;
            case 3 :            // Disabled
            default:
                    break;
        }
        MmioWrite32 (NB_PCIE_CFG_ADDRESS(0, 0, 0, gPamStruct[i].Register), Data); // WRITE_PCI8_NB(gPamStruct[i].Register, Data);
    }

    if (Granularity)
        *Granularity = ( (StartAddress+Length) < 0xf0000 ) ? 0x4000 : 0x10000;

    return EFI_SUCCESS;
}

//----------------------------------------------------------------------------
#endif          // END OF CSM Related Porting Hooks
//----------------------------------------------------------------------------

/**
    This function provides runtime interface to enable/disable
    writing in E000-F000 segment

    @param Enable if TRUE - enable writing, if FALSE - disable

    @retval VOID
**/
VOID NbRuntimeShadowRamWrite(
    IN BOOLEAN Enable
)
{
    // Porting Required.
    static UINT8   F000Reg = 0xff; // 0x80
    static UINT8   E000Reg = 0xff; // 0x85
    static UINT8   E800Reg = 0xff; // 0x86

    if (Enable) {
        F000Reg = MmioRead8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM0)); // READ_PCI8_NB(R_SA_PAM0); // 0x80
        MmioOr8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM0), 0x30); // SET_PCI8_NB(R_SA_PAM0, 0x30);
        
        E000Reg = MmioRead8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM5)); // READ_PCI8_NB(R_SA_PAM5); // 0x85
        E800Reg = MmioRead8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM6)); // READ_PCI8_NB(R_SA_PAM6); // 0x86
        MmioOr8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM5), 0x33); // SET_PCI8_NB(R_SA_PAM5, 0x33);
        MmioOr8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM6), 0x33); // SET_PCI8_NB(R_SA_PAM6, 0x33);
    } else {
        if (F000Reg != 0xff) {
            MmioWrite8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM0), F000Reg); // WRITE_PCI8_NB(R_SA_PAM0, F000Reg); // 0x80
            MmioWrite8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM5), E000Reg); // WRITE_PCI8_NB(R_SA_PAM5, E000Reg); // 0x85
            MmioWrite8 (NB_PCIE_CFG_ADDRESS(0, 0, 0, R_SA_PAM6), E800Reg); // WRITE_PCI8_NB(R_SA_PAM6, E800Reg); // 0x86
        }
    }

}

/**
    This function provides runtime interface to enable/disable
    writing in C000-F000 segment

    @param The shadowing attributes of the BIOS range
    @param The BIOS range which is going to be shadowed

    @retval VOID
**/
EFI_STATUS
NbRuntimeShadowRamWriteExt (
    IN SHADOW_ARRTIBUTE   Attribute,
    IN SHADOW_BIOS_RANGE  Range
)
{
    /**** PORTING REQUIRED ****
    UINT8   PamData=0;
    UINT8   PamIndex=0;

    switch (Attribute) {
    case Disabled:
      PamData = 0x0;
      break;
    case ReadOnly:
      PamData = 0x1;
      break;
    case WriteOnly:
      PamData = 0x2;
      break;
    case ReadWrite:
      PamData = 0x3;
      break;
    default:
      return EFI_INVALID_PARAMETER;
    }

    switch (Range) {
    case C4000_16K:
      PamData <<= 4;
    case C0000_16K:
      PamIndex = PAM1;
      break;
    case CC000_16K:
      PamData <<= 4;
    case C8000_16K:
      PamIndex = PAM2;
      break;
    case D4000_16K:
      PamData <<= 4;
    case D0000_16K:
      PamIndex = PAM3;
      break;
    case DC000_16K:
      PamData <<= 4;
    case D8000_16K:
      PamIndex = PAM4;
      break;
    case E4000_16K:
      PamData <<= 4;
    case E0000_16K:
      PamIndex = PAM5;
      break;
    case EC000_16K:
      PamData <<= 4;
    case E8000_16K:
      PamIndex = PAM6;
      break;
    case F0000_64K:
      PamData <<= 4;
      PamIndex = PAM0;
      break;
    default:
      return EFI_INVALID_PARAMETER;
    }

    WRITE_PCI8_NB(PamIndex, PamData);
    **** PORTING REQUIRED ****/
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
