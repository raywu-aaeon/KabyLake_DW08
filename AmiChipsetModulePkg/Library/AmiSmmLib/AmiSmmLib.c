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

/** @file AmiSmmLib.c
    South Bridge SMM Library implementation

*/

#include <Token.h>
#include <Library/AmiSmmLib.h>
#include <Library/IoLib.h>
#include <Guid/Rtc.h>
#include <AmiCspLib.h>
#include <PchAccess.h>
#include <Sb.h>
#if defined HPET_PROTOCOL_SUPPORT && HPET_PROTOCOL_SUPPORT == 1
#include <Library/AmiHpetApicLib.h>
#endif
#include "CpuRegs.h"
#include <Library/PciSegmentLib.h>


//---------------------------------------------------------------------------
#if SMM_SUPPORT
//---------------------------------------------------------------------------

/**
    This hook is called in the very SMI entry and exit.
    Save/Restore chipset data if needed.

    @param Save - TRUE = Save / FALSE = Restore

    @retval EFI_SUCCESS
**/

EFI_STATUS SbSmmSaveRestoreStates (
    IN BOOLEAN                      Save )
{

    static volatile UINT8   StoreCMOS;
    static volatile UINT8   StoreExtCMOS;
    static volatile UINT32  StoreCF8;
    static volatile UINT8   RtcRegA;
    static volatile UINT32  StorePm2Cnt;

    #if defined HPET_PROTOCOL_SUPPORT && HPET_PROTOCOL_SUPPORT == 1
    static volatile BOOLEAN IsHpetApicEn = FALSE;
    static volatile UINT8   StoreIoApicIndex;
    volatile UINT8  Irq;
    #endif

    static volatile UINT32  StoreSpiBar0 = 0;
    UINT32                  SpiBar0;

    if (Save) {
       // 
       //  Before SmmDispatch 
       //
       StoreCF8 = IoRead32(0xcf8);    // Store CF8 (PCI index)                
    
#if defined SAVE_RESTORE_NMI_IN_SMM && SAVE_RESTORE_NMI_IN_SMM        
       // Save Alternate access bit.
       AltAcc = MMIO_READ32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC)) & B_SB_PCR_ITSS_GIC_AME;
    
       // Enable Alternate access mode, let port 70h/72h can be readable.
       SET_MEM32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC), B_SB_PCR_ITSS_GIC_AME);
       while ((MMIO_READ32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC)) & B_SB_PCR_ITSS_GIC_AME) == 0);
    
       StoreCMOS = IoRead8(RTC_IO_INDEX); // Save 0x70
       StoreExtCMOS = IoRead8(CMOS_IO_EXT_INDEX);  // Save 0x72
       
       // Disable Alternate access mode.
       RESET_MEM32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC), B_SB_PCR_ITSS_GIC_AME);
       while (MMIO_READ32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC)) & B_SB_PCR_ITSS_GIC_AME);
#else
       StoreCMOS = IoRead8(CMOS_IO_INDEX_BACKDOOR); // Save 0x74
       StoreExtCMOS = IoRead8(CMOS_IO_INDEX_BACKDOOR+2);  // Save 0x76 
#endif
       
       StorePm2Cnt = READ_IO8_PM(R_SB_ACPI_PM2_CNT); // Store PM_BASE+0x50
    
#if defined CMOS_MANAGER_SUPPORT && CMOS_MANAGER_SUPPORT
#if defined SAVE_RESTORE_NMI_IN_SMM && SAVE_RESTORE_NMI_IN_SMM 
      IoWrite8(CMOS_IO_EXT_INDEX, SB_CMOS_MISC_FLAG_REG);
      IoWrite8(CMOS_IO_EXT_DATA, 0);
#endif     
#endif

#if defined HPET_PROTOCOL_SUPPORT && HPET_PROTOCOL_SUPPORT == 1
      StoreIoApicIndex = MMIO_READ8(APCB);
      IsHpetApicEn = IsHpetApicEnable();
      if (IsHpetApicEn){
          Irq = GetHpetApicPin();
          IoApicMaskIrq(Irq);
      }
#endif
      SpiBar0 = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (
                                    0,
                                    SB_SPI_BUS,
                                    SB_SPI_DEV,
                                    SB_SPI_FUN,
                                    R_SB_SPI_BAR0)) & ~SPI_BAR0_MASK;

      if (SpiBar0 != SPI_BASE) {
        //
        // Confirm if SPI_BASE is used
        //
        if (MmioRead32 (SPI_BASE) == 0xFFFFFFFF) {
          StoreSpiBar0  = SpiBar0;
          SpiBar0       = SPI_BASE;
          PciSegmentWrite32 (PCI_SEGMENT_LIB_ADDRESS (
                                0,
                                SB_SPI_BUS,
                                SB_SPI_DEV,
                                SB_SPI_FUN,
                                R_SB_SPI_BAR0),
                                SpiBar0);        
        } else {
          StoreSpiBar0 = 0;
        }
      } else {
        StoreSpiBar0 = 0;
      }
    } else {
       if (StoreSpiBar0) {
           PciSegmentWrite32 (PCI_SEGMENT_LIB_ADDRESS (
                                 0,
                                 SB_SPI_BUS,
                                 SB_SPI_DEV,
                                 SB_SPI_FUN,
                                 R_SB_SPI_BAR0),
                                 StoreSpiBar0);
       }
       
       IoWrite32(0xcf8, StoreCF8);    // Restore 0xCF8 (PCI index)
       
       // 
       //  After SmmDispatch 
       //
       if ((StoreCMOS >= 0) && (StoreCMOS <= 9))   // EIP467760
       {
           do
           {
               RtcRegA = READ_IO8_RTC(0x8a);
           } while (RtcRegA & 0x80);
       } // if ((StoreCMOS >= 0) && (StoreCMOS <= 9))
    
#if defined CMOS_MANAGER_SUPPORT && CMOS_MANAGER_SUPPORT
#if defined SAVE_RESTORE_NMI_IN_SMM && SAVE_RESTORE_NMI_IN_SMM
       IoWrite8(CMOS_IO_EXT_INDEX, SB_CMOS_MISC_FLAG_REG);
       SbCmosMiscFlag = IoRead8(CMOS_IO_EXT_DATA);
    
       if (SbCmosMiscFlag & ENABLE_NMI_BEFORE_SMI_EXIT)
           StoreCMOS &= ~BIT07;          // Enable NMI_EN

       if (SbCmosMiscFlag & DISABLE_NMI_BEFORE_SMI_EXIT)
           StoreCMOS |= BIT07;           // Disable NMI_EN
#endif
#endif

#if defined SAVE_RESTORE_NMI_IN_SMM && SAVE_RESTORE_NMI_IN_SMM
       IoWrite8(RTC_IO_INDEX, StoreCMOS);     // Restore 0x70
       IoWrite8(CMOS_IO_EXT_INDEX, StoreExtCMOS); // Restore 0x72
       do
       {
           //Improve alternate access mode >>
           MmioAndThenOr32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC), ~B_SB_PCR_ITSS_GIC_AME, AltAcc);
       } while ((MMIO_READ32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC)) & B_SB_PCR_ITSS_GIC_AME) != AltAcc);     //Improve alternate access mode <<
#else
       IoWrite8(CMOS_IO_INDEX_BACKDOOR, StoreCMOS);     // Restore 0x74
       IoWrite8(CMOS_IO_INDEX_BACKDOOR+2, StoreExtCMOS); // Restore 0x76
#endif       
    
       WRITE_IO8_PM (R_SB_ACPI_PM2_CNT, StorePm2Cnt); // Restore PM_BASE+0x50
       IoWrite32(0xcf8, StoreCF8);    // Restore 0xCF8 (PCI index)

#if defined HPET_PROTOCOL_SUPPORT && HPET_PROTOCOL_SUPPORT == 1
       if (IsHpetApicEn) {
           Irq = GetHpetApicPin();
           IoApicUnmaskIrq(Irq);
           IoApicEoi(Irq);
       }

       MMIO_WRITE8(APCB, StoreIoApicIndex);
       StoreIoApicIndex = MMIO_READ8(APCB);
#endif
    }
    return EFI_SUCCESS;
}

//---------------------------------------------------------------------------
#endif  // END OF SMM Related Porting Hooks
//---------------------------------------------------------------------------

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
