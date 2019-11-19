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

/** @file AmiCmosLib.c
    South Bridge CMOS Library implementation

*/

#include <Token.h>
#include <AmiCspLib.h>
#include <Library/AmiCmosLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/AmiChipsetIoLib.h>
#include <Library/PchPcrLib.h>
#include <PchAccess.h>
#include <SbElinks.h>
#include <Guid/Rtc.h>
#include <Sb.h>

extern SB_RTC_PWR_VALID_CALLBACK SB_RTC_PWR_VALID_CALLBACK_LIST EndOfList;
SB_RTC_PWR_VALID_CALLBACK* SbRtcPwrValidCallbackList[] = { SB_RTC_PWR_VALID_CALLBACK_LIST NULL };


#if CMOS_MANAGER_SUPPORT
/**
    This function is used to access addresses in the CMOS
    register range (0x80-0xff), for PEI and DXE boot phases.

    @param PeiServices - PEI Services table pointer (NULL in DXE phase)
    @param AccessType - ReadType or WriteType to specify the
                          type of access
    @param CmosRegister - The CMOS register to access
    @param CmosParameterValue - Pointer to the data variable to be
                                accessed

    @retval EFI_STATUS (return value) 
            EFI_SUCCESS The access operation was successfully.
            Otherwise A valid EFI error code is returned.

    @note  This function is used when a translation from logical address
              to index port value is required.
**/

EFI_STATUS ReadWriteCmosBank2 (
    IN EFI_PEI_SERVICES         **PeiServices,  // NULL in DXE phase
    IN CMOS_ACCESS_TYPE         AccessType,
    IN UINT16                   CmosRegister,
    IN OUT UINT8                *CmosParameterValue )
{
    if ((CmosRegister < 0x80) || (CmosRegister > 0xff))
        return EFI_INVALID_PARAMETER;

    // Some chipsets require tranlation from the logical CMOS address to a
    // physical CMOS index port value. However, other chipsets do not require
    // a translation and the index/data port can be directly used for 
    // accessing the second bank.

    IoWrite8( CMOS_IO_EXT_INDEX, (UINT8)CmosRegister );

    if (AccessType == ReadType) {
        *CmosParameterValue = IoRead8( CMOS_IO_EXT_DATA );
    } else {
        IoWrite8( CMOS_IO_EXT_DATA, *CmosParameterValue );
    }

    return EFI_SUCCESS;
}

/**
    This function is checked CMOS battery is good or not.

    @param PeiServices - PEI Services table pointer (NULL in DXE phase)

    @retval BOOLEAN  
            TRUE The CMOS is battery is good.
            FALSE The CMOS is battery is bad.
**/
BOOLEAN SbGetRtcPowerStatus (
    IN EFI_PEI_SERVICES     **PeiServices )
{
    return (READ_PCI8(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B) & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) ? FALSE : TRUE; // 0xA4
}

#endif  // #if CMOS_MANAGER_SUPPORT


/**
    This function reads one byte from CMOS register addressed by Index

    @param Index - CMOS Index

    @return The value is read by the index.

**/
UINT8 ReadCmos(
    IN UINT8 Index
)
{
    // Read bit 7 (NMI setting).
    UINT8           NMI = 0;
    volatile UINT8  Value;
    BOOLEAN         IntState = CPULib_GetInterruptState();
    UINT8           RtcIndexPort;
    UINT8           RtcDataPort;

    CPULib_DisableInterrupt();

    if (Index < 0x80) {
      // Standard CMOS
      RtcIndexPort  = RTC_INDEX_REG;
      RtcDataPort   = RTC_DATA_REG;

                      									          //Improve alternate access mode >>
      // Save current NMI_EN.
      NMI = ReadPort70h()  & RTC_NMI_MASK;				//Improve alternate access mode <<

    } else {
      // Upper CMOS
      RtcIndexPort  = CMOS_IO_EXT_INDEX;
      RtcDataPort   = CMOS_IO_EXT_DATA;
    }

    Index &= ~RTC_NMI_MASK;

    IoWrite8(RtcIndexPort, Index | NMI);
    Value = IoRead8(RtcDataPort); // Read register.

    if (IntState) CPULib_EnableInterrupt();

    return (UINT8)Value;
}

/**
    This function writes value to CMOS register addressed by Index

    @param Index - CMOS register index
    @param Value - Value to write

    @retval VOID

**/
VOID WriteCmos(
    IN UINT8 Index, 
    IN UINT8 Value
)
{
    // Read bit 7 (NMI setting).
    UINT8   NMI = 0;
    BOOLEAN IntState = CPULib_GetInterruptState();
    UINT8   RtcIndexPort;
    UINT8   RtcDataPort;

    CPULib_DisableInterrupt();

    if (Index < 0x80) {
      // Standard CMOS
      RtcIndexPort  = RTC_INDEX_REG;
      RtcDataPort   = RTC_DATA_REG;

                                        //Improve alternate access mode >>
      // Save current NMI_EN.
      NMI = ReadPort70h()  & RTC_NMI_MASK;
                                        //Improve alternate access mode <<
    } else {
      // Upper CMOS
      RtcIndexPort  = CMOS_IO_EXT_INDEX;
      RtcDataPort   = CMOS_IO_EXT_DATA;
    }

    Index &= ~RTC_NMI_MASK; 

    IoWrite8(RtcIndexPort, Index | NMI);
    IoWrite8(RtcDataPort, Value); // Write Register.

    if (IntState) CPULib_EnableInterrupt();
}

/**
    Enable or disable Alternate Access Mode.

    @param Switch TRUE: Enable Alternate Access Mode
                  FALSE: Disable Alternate Access Mode

    @retval VOID
**/
VOID SwitchAlternateAccessMode (
    IN BOOLEAN      Switch
)
{
    UINT32  Data32Or;
    UINT32  Data32And;

    Data32Or  = 0;
    Data32And = 0xFFFFFFFF;

    if (Switch) {
      ///
      /// Enable Alternate Access Mode
      /// Note: The RTC Index field (including the NMI mask at bit7) is write-only
      /// for normal operation and can only be read in Alt Access Mode.
      ///
      Data32Or  = (UINT32) (B_PCH_PCR_ITSS_GIC_AME);
    } else {
      ///
      /// Disable Alternate Access Mode
      ///
      Data32And = (UINT32) ~(B_PCH_PCR_ITSS_GIC_AME);
    }

    ///
    /// Program Alternate Access Mode Enable bit
    ///
    PchPcrAndThenOr32 (
      PID_ITSS, R_PCH_PCR_ITSS_GIC,
      Data32And,
      Data32Or
      );
}

/**
    Read port 70h.

    @param VOID

    @return Data of port 70h.
**/
UINT8 ReadPort70h (
    VOID
)
{
    UINT8  Port70h;

    SwitchAlternateAccessMode (TRUE);

    Port70h = IoRead8(RTC_INDEX_REG);

    SwitchAlternateAccessMode (FALSE);

    return Port70h;
}

/**
    Check NMI bit

    @param VOID

    @retval TRUE  : NMI is enabled.
            FALSE : NMI is disabled.
**/
BOOLEAN SbCheckNmiStatus (
    VOID
)
{
    if (ReadPort70h() & RTC_NMI_MASK) {
        // NMI is disabled.
        return FALSE;
    } else {
        // NMI is Enabled.
        return TRUE;
    }
}

/**
    Enable / Disable NMI Bit

    @param Enable - TRUE  : Enable NMI
                    FALSE : Disable NMI

    @retval TRUE  : NMI is enabled
            FALSE : NMI is disabled
**/
BOOLEAN SbEnableDisableNmi (
    IN BOOLEAN      Enable
)
{
    BOOLEAN IntState = CPULib_GetInterruptState();
    static UINT8   Data8;

    CPULib_DisableInterrupt();
    
    Data8 = IoRead8 (CMOS_IO_STD_INDEX);
    
    if (Enable)
      Data8 &= ~RTC_NMI_MASK;
    else {
      Data8 |= RTC_NMI_MASK;
    }
    IoWrite8 (CMOS_ADDR_PORT, Data8);

    if (IntState) CPULib_EnableInterrupt();
    
    return SbCheckNmiStatus();
}

/**
    Save / Restore NMI bit

    @param Save TRUE  : Save NMI
                FALSE : Restore NMI

    @retval VOID

**/
VOID SbSaveRestoreNmi (
    IN BOOLEAN      Save
)
{
    static  UINT8   NmiMask;
            BOOLEAN IntState = CPULib_GetInterruptState();
            UINT8   Data8;

    CPULib_DisableInterrupt();

    if (Save)
      // Save current NMI_EN.
      NmiMask = ReadPort70h() & RTC_NMI_MASK;
    else {
      Data8 = IoRead8 (CMOS_IO_STD_INDEX) & ~(RTC_NMI_MASK);
      IoWrite8 (CMOS_ADDR_PORT, Data8 | NmiMask);    
    }
    if (IntState) CPULib_EnableInterrupt();
}

/**
    Check RTC power status

    @param VOID

    @retval TRUE:  Valid
            FALSE: Invalid

**/
BOOLEAN SbIsRtcPwrValid (
    VOID
)
{
    UINT32                  Index;
    BOOLEAN                 RtcPwrValid = TRUE;

#if !EXTERNAL_RTC_SUPPORT
    if (READ_PCI8(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B) & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS)
        RtcPwrValid = FALSE;
    else
        RtcPwrValid = TRUE;
#endif

    for (Index = 0; SbRtcPwrValidCallbackList[Index] != NULL; Index++) {
        RtcPwrValid = SbRtcPwrValidCallbackList[Index]();
    }
    return RtcPwrValid;
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
