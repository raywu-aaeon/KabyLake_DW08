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

/** @file SbCspFlashLib.c
    South Bridge Chipset Flash Library implementation

*/

#include "Efi.h"
#include "Token.h"


#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AmiChipsetIoLib.h>
#include <PchAccess.h>
#include <CpuRegs.h>
#include <Library/PchPcrLib.h>

#include <Protocol/SmmBase2.h>
#include <Library/UefiBootServicesTableLib.h>
static BOOLEAN gAmiCspInSmm = FALSE;
static EFI_SMM_BASE2_PROTOCOL  *gAmiCspSmmBase2 = NULL;
//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------

// GUID Definition(s)

// Protocol/Ppi Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

//Generic Flash part porting hooks

/**
    This function is invoked to do any chipset specific operations 
    that are necessary when enabling the Flash Part for writing

    @param VOID

    @retval VOID

**/
VOID ChipsetFlashDeviceWriteEnable(
    VOID
)
{
    if (READ_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC) & B_PCH_SPI_BC_LE) {
        if ( (AsmReadMsr64 (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) && 
             (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) {
            return;
        }
    }  
    
    ///
    /// Write clear BC_SYNC_SS prior to change WPD from 0 to 1.
    ///
    SET_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC + 1, (B_PCH_SPI_BC_SYNC_SS >> 8));
    
    ///
    /// Set BIOSWE bit (SPI PCI Offset DCh [0]) = 1b
    /// Enable the access to the BIOS space for both read and write cycles
    ///
    SET_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC, B_PCH_SPI_BC_WPD);
    
    if ((READ_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC) & B_PCH_SPI_BC_EISS) == 0) return;
    if (!gAmiCspInSmm) return;
    if ( !((AsmReadMsr64 (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
           (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN))) {
         UINT32 Data32;
         Data32 = MmioRead32 ((UINTN) (0xFED30880)) | (UINT32) (BIT0);
         AsmWriteMsr64(MSR_SPCL_CHIPSET_USAGE_ADDR, Data32);
    }
}

/**
    This function is invoked to do any chipset specific operations 
    that are necessary when disabling the Flash Part for writing

    @param VOID

    @retval VOID

**/
VOID ChipsetFlashDeviceWriteDisable(
    VOID
)
{
    if (READ_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC) & B_PCH_SPI_BC_LE) {
        if ( (AsmReadMsr64 (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) && 
             (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) {
            return;
        }
    }  
    
    ///
    /// Clear BIOSWE bit (SPI PCI Offset DCh [0]) = 0b
    /// Disable the access to the BIOS space for write cycles  
    RESET_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC, B_PCH_SPI_BC_WPD);
    
    if ((READ_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC) & B_PCH_SPI_BC_EISS) == 0) return;
    if (!gAmiCspInSmm) return;
    if ( !((AsmReadMsr64 (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
           (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN))) {
          UINT32 Data32;
          Data32 = MmioRead32 ((UINTN) (0xFED30880)) & (UINT32) (~BIT0);
          AsmWriteMsr64 (MSR_SPCL_CHIPSET_USAGE_ADDR, Data32);
    }
}

#if BIOS_LOCK_ENABLE
/**
    The procedure re-enable the BIOS Write Enable bit before
    Write/Erase procedures if BIOS_LOCK_ENABLE is set. 

    @param SwSmiNum - A S/W SMI number, this value must be 0x20.
    @param Buffer   - A 64bit buffer uses for communicating between
                      caller and this routine.

    @retval VOID

**/

VOID BiosLockEnableSmiFlashHook (
    IN UINT8            SwSmiNum,
    IN OUT UINT64       Buffer
)
{
    if ((SwSmiNum != 0x20) || \
        ((READ_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC) & B_PCH_SPI_BC_LE) == 0))
      return;

    // Clear B_PCH_SPI_BC_SYNC_SS
    SET_PCI8(0, 0x1F, 0x05, R_PCH_SPI_BC + 1, (B_PCH_SPI_BC_SYNC_SS >> 8));   

    // Clear TCO_STS
    WRITE_IO16(PM_BASE_ADDRESS + R_PCH_SMI_STS, B_PCH_SMI_STS_TCO);

}    
#endif

/**
    This function checks if TopSwap (A16, A17, A18 address lines
    inversion) is on
              
    @param VOID

    @retval TRUE TopSwap is ON
    @retval FALSE TopSwap is OFF

    @note  Intel Chipsets are porting required
**/
BOOLEAN IsTopSwapOn( VOID )
{
    BOOLEAN LpcTopSwapState = (READ_PCI8(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_BC) & B_PCH_LPC_BC_TS) ? TRUE : FALSE;
    BOOLEAN SpiTopSwapState = (READ_PCI8(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SPI, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC) & B_PCH_SPI_BC_TSS) ? TRUE : FALSE;
    // 
    // Check TopSwap bit of LPC, eSPI and SPI.
    //
    return (LpcTopSwapState & SpiTopSwapState) ? TRUE : FALSE; 
}

/**
    This function sets TopSwap (A16, A17, A18 address lines 
    inversion) to ON or OFF.

    @param BOOLEAN On - if TRUE, set TopSwap to ON, if FALSE - set to OFF

    @retval VOID

    @note  Intel Chipsets are porting required
**/
VOID
SetTopSwap(
        IN BOOLEAN      On
)
{
    // 
    // Enable or disable TopSwap bit.
    //
    if (On) {
        PchPcrAndThenOr8(PID_RTC, R_PCH_PCR_RTC_BUC, 0xff, B_PCH_PCR_RTC_BUC_TS); // Enable.
    } else {
        PchPcrAndThenOr8(PID_RTC, R_PCH_PCR_RTC_BUC, 0xfe, 0);  // Disable. 
    }
      
    return; 
}

/**
    Read SPI Descriptor

    @param  SectionSelect     Data written to Flash Descriptor Section Select bits
    @param  SectionIndex      Data written to Flash Descriptor Section Index bits
    @param  *SectionData      Pointer to Flash Descriptor Section Data

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to read SPI Descriptor
    @retval EFI_DEVICE_ERROR  Failed to read SPI Descriptor
    @retval EFI_NOT_FOUND     Invalid Flash Valid Signature

**/
EFI_STATUS ReadSpiDescriptor (
    IN UINT8                SectionSelect,
    IN UINT8                SectionIndex,
    OUT UINT32              *SectionData
)
{
    //UINT32 Buffer32;

    //WRITE_MEM32_SPI(R_RCRB_SPI_FDOC, 0);
    
    //Buffer32 = READ_MEM32_SPI(R_RCRB_SPI_FDOD);

    //if (Buffer32 != 0x0ff0a55a)                       //Improve alternate access mode 
    //    return EFI_NOT_FOUND;

    //Buffer32 = ((UINT32)SectionSelect << 12) | ((UINT32)SectionIndex << 2);
    //WRITE_MEM32_SPI(R_RCRB_SPI_FDOC, Buffer32);
    //*FDOD = READ_MEM32_SPI(R_RCRB_SPI_FDOD);

    return EFI_UNSUPPORTED;
}

/**
    Get the SPI region base, based on the enum type.
        
    @param RegionType - Region type to query for the base address
    @param LimitAddress - This address is left shifted by 12 bits to 
                          represent bits 26:12 for the Region 'n' Limit.
    @param BaseAddress - This address is left shifted by 12 bits to 
                         represent bits 26:12 for the Region 'n' Base.

    @retval EFI_SUCCESS - Read success
            EFI_INVALID_PARAMETER - Invalid region type given

**/
/*
EFI_STATUS
EFIAPI
GetSpiRegionAddresses (
  IN   AMI_PCH_SPI_REGION_TYPE  RegionType,
  OUT  UINT32                   *LimitAddress,
  OUT  UINT32                   *BaseAddress
  )
{
    BOOLEAN               AdjustRange;
    UINTN                 RegionTypeReg;
    UINT32                ReadValue;
    UINT32                RegionRangeBase;

    if (!((RegionType >= AmiUndefinedType) && (RegionType <= AmiTxeType))) {
      return EFI_INVALID_PARAMETER;
    }

    AdjustRange = FALSE;

    if (RegionType == AmiUndefinedType) {
      return EFI_SUCCESS;
    } else if (RegionType == AmiDescriptorType) {
      RegionTypeReg = R_SB_SPI_FREG0_FLASHD;
    } else if (RegionType == AmiBiosType) {
      AdjustRange = TRUE;
      RegionTypeReg = R_SB_SPI_FREG1_BIOS;
    } else if (RegionType == AmiTxeType) {
      RegionTypeReg = R_SB_SPI_FREG2_SEC;
    }

    ReadValue = MmioRead32 (SPI_BASE_ADDRESS + RegionTypeReg);
    RegionRangeBase  = (ReadValue & B_SB_SPI_FREGX_BASE_MASK) << 12;

    if (AdjustRange == TRUE) {
      *LimitAddress += RegionRangeBase;
      *BaseAddress  += RegionRangeBase;
    } else {
      *LimitAddress = (ReadValue & B_SB_SPI_FREGX_LIMIT_MASK) >> 4;
      *BaseAddress  = RegionRangeBase;
    }
    
    return EFI_SUCCESS;
}
*/

/**
    Get total Flash ROM size by reading SPI Descriptor.
        
    @param VOID

    @return Flash ROM Size

**/
/*
UINT32 GetTotalFlashRomSize ( VOID )
{
    UINT32  TotalRomSize = 0;
    UINT32  CtrlData;
    UINT8   NumSpi = 0;

    //
    // Select to Flash Map 0 Register to get the number of flash Component
    //
    CtrlData = MmioRead32((SPI_BASE_ADDRESS + R_SB_SPI_FDOC));
    CtrlData &= (UINT32)(~(B_SB_SPI_FDOC_FDSS_MASK | B_SB_SPI_FDOC_FDSI_MASK));
    CtrlData |= (UINT32)(V_SB_SPI_FDOC_FDSS_FSDM | R_SB_SPI_FDBAR_FLASH_MAP0);
    MmioWrite32((SPI_BASE_ADDRESS + R_SB_SPI_FDOC), CtrlData);

    switch ( MmioRead16 (SPI_BASE_ADDRESS + R_SB_SPI_FDOD) & B_SB_SPI_FDBAR_NC ) {
        case V_SB_SPI_FDBAR_NC_1:
            NumSpi = 1;
            break;
        case V_SB_SPI_FDBAR_NC_2:
            NumSpi = 2;
            break;
        default:
            break;
    }

    if (NumSpi == 0) return TotalRomSize;

    //
    // Select to Flash Components Register to get the Component 1 Density
    //
    CtrlData = MmioRead32((SPI_BASE_ADDRESS + R_SB_SPI_FDOC));
    CtrlData &= (UINT32)(~(B_SB_SPI_FDOC_FDSS_MASK | B_SB_SPI_FDOC_FDSI_MASK));
    CtrlData |= (UINT32)(V_SB_SPI_FDOC_FDSS_COMP | R_SB_SPI_FCBA_FLCOMP);
    MmioWrite32((SPI_BASE_ADDRESS + R_SB_SPI_FDOC), CtrlData);

    //
    //  Get Component 1 Density
    //
    switch ( (UINT8) MmioRead32 (SPI_BASE_ADDRESS + R_SB_SPI_FDOD) & B_SB_SPI_FLCOMP_COMP1_MASK ) {
        case V_SB_SPI_FLCOMP_COMP1_512KB:
            TotalRomSize += (UINT32) (512 << KBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_1MB:
            TotalRomSize += (UINT32) (1 << MBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_2MB:
            TotalRomSize += (UINT32) (2 << MBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_4MB:
            TotalRomSize += (UINT32) (4 << MBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_8MB:
            TotalRomSize += (UINT32) (8 << MBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_16MB:
            TotalRomSize += (UINT32) (16 << MBShift);
            break;
        default:
            break;
    } // end of switch

    //
    // Get Component 2 Density
    //
    if (NumSpi == 2) {
        switch ( (UINT8) MmioRead32 (SPI_BASE_ADDRESS + R_SB_SPI_FDOD) & B_SB_SPI_FLCOMP_COMP2_MASK ) {
            case V_SB_SPI_FLCOMP_COMP2_512KB:
                TotalRomSize += (UINT32) (512 << KBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_1MB:
                TotalRomSize += (UINT32) (1 << MBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_2MB:
                TotalRomSize += (UINT32) (2 << MBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_4MB:
                TotalRomSize += (UINT32) (4 << MBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_8MB:
                TotalRomSize += (UINT32) (8 << MBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_16MB:
                TotalRomSize += (UINT32) (16 << MBShift);
                break;
            default:
                break;
        } // end of switch
    }// end of if

    return TotalRomSize;
}
*/

/**
    This routine provides H/W read/write-protection of the BIOS region.
    If chipset supports "SPI Flash Protected Range Registers", then program
    them to protect BIOS region in SPI Flash.
        
    @param VOID

    @retval EFI_STATUS
            EFI_SUCCESS - Set successfully.
            EFI_INVALID_PARAMETER - The parameter of input is invalid
            EFI_UNSUPPORTED - Chipset or H/W is not supported.

    Note:   Porting required

**/
EFI_STATUS SbFlashProtectedRange ( VOID )
{
    return EFI_UNSUPPORTED;
/*
    AMI_SPI_PROTECTED_RANGE_CONIFG  SpiProtectedRange[] = { SPI_PROTECTED_RANGE_0,
                                                            SPI_PROTECTED_RANGE_1,
                                                            SPI_PROTECTED_RANGE_2,
                                                            SPI_PROTECTED_RANGE_3,
                                                            SPI_PROTECTED_RANGE_4 };
    AMI_PCH_SPI_REGION_TYPE         AmiSpiRegionType;
    BOOLEAN                         WriteProtectionEnable;
    BOOLEAN                         ReadProtectionEnable;
    UINT32                          ProtectedRangeBase;
    UINT32                          ProtectedRangeLimit;
    UINT32                          RegIndex;
    UINT32                          TotalRomSize;
    volatile UINT32                 Value32;

    //
    // Get Total ROM size first
    //
    TotalRomSize = GetTotalFlashRomSize();

    if (TotalRomSize == 0) return EFI_INVALID_PARAMETER;

    //
    // Program Protected Range Registers per SPI_PROTECTED_RANGE_X tokens
    //
    for (RegIndex = 0; RegIndex < 5; RegIndex++) {
      AmiSpiRegionType      = SpiProtectedRange[RegIndex].AmiPchSpiRegionType;
      WriteProtectionEnable = SpiProtectedRange[RegIndex].WriteProtectionEnable;
      ReadProtectionEnable  = SpiProtectedRange[RegIndex].ReadProtectionEnable;
      ProtectedRangeBase    = SpiProtectedRange[RegIndex].ProtectedRangeBase;
      ProtectedRangeLimit   = SpiProtectedRange[RegIndex].ProtectedRangeBase + SpiProtectedRange[RegIndex].ProtectedRangeLength - 1;
      
      if ((WriteProtectionEnable == 0) && (ReadProtectionEnable == 0)) continue;
      
      //
      // Get RangeBase and RangeLimit per SpiRegion
      //
      GetSpiRegionAddresses (AmiSpiRegionType, &ProtectedRangeBase, &ProtectedRangeLimit);
      //
      // Exceed the address of protected range base.
      //
      if (ProtectedRangeBase >= TotalRomSize) continue;
      //
      // Exceed the address of protected range limit.
      //
      if (ProtectedRangeLimit >= TotalRomSize) continue;

      Value32 = *(UINT32 *)(SPI_BASE_ADDRESS + R_SB_SPI_PR0 + (RegIndex * 4));
      if (Value32 == 0) {
        Value32 = (ProtectedRangeBase & 0x1FFF000) >> 12;
        Value32 += ((ProtectedRangeLimit & 0x1FFF000) << 4);
        if (ReadProtectionEnable) Value32 |= B_SB_SPI_PRx_RPE;
        if (WriteProtectionEnable) Value32 |= B_SB_SPI_PRx_WPE;
        *(UINT32 *)(SPI_BASE_ADDRESS + R_SB_SPI_PR0 + (RegIndex * 4)) = Value32;
      } // if register is not use yet
    } // for loop

    return EFI_SUCCESS;
*/    
}

EFI_STATUS
EFIAPI
AmiCspLibDxeConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS         Status;

    
    if( gAmiCspSmmBase2 == NULL ) {
        Status = gBS->LocateProtocol( &gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &gAmiCspSmmBase2 );
        if ( EFI_ERROR(Status) ) return EFI_SUCCESS;            
    }
    gAmiCspSmmBase2->InSmm (gAmiCspSmmBase2, &gAmiCspInSmm);

    return EFI_SUCCESS;
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
