//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **

/** @file BgtCheckMeStateLib.c
*/

#include <Token.h>
#include <Uefi.h>
#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <MeChipset.h>
#include <Library/PchInfoLib.h>
#include <Sb.h>
#include <Register/PchRegsPsf.h>

#ifdef PCIEX_BASE_ADDRESS 
#define ME_PCIE_CFG_ADDRESS(bus, dev, func, reg) \
            ((UINTN)(PCIEX_BASE_ADDRESS + ((UINT8)(bus) << 20) + \
            ((UINT8)(dev) << 15) + ((UINT8)(func) << 12) + (reg)))
#endif

/**
    Get ME Host FW Status.

    @param VOID

    @retval UINT32 Value of HFS

**/
UINT32
GetHfs(
    VOID
)
{
    UINT32      Data;
    UINT32      HfsData;
    BOOLEAN     DeviceHide = FALSE;
    UINT32      Address;

    Address = PCH_PCR_ADDRESS(PID_PSF1, (R_PCH_LP_PCR_PSF1_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN));

    if( !Address ) return 0xFFFFFFFF; 

    Data = MmioRead32( Address );

    if( Data & B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS )
    {
        MmioWrite32( Address, (UINT32)(Data & ~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS) );
        MmioRead32( Address ); // Take register effect.
        DeviceHide = TRUE;
    }

    HfsData = *(volatile UINT32*)ME_PCIE_CFG_ADDRESS( ME_BUS, ME_DEVICE_NUMBER, HECI_MIN_FUNC, R_ME_HFS);

    //If HDA_SDO jumper is asserted, the Heci device won't be able to access in ME 12.
    //Here add the temp solution if vendor and device ID is 0xFFFF, return data with SECOVR_JMPR
    if( HfsData == 0xFFFFFFFF )
    {
        UINT16  Vid;
        UINT16  Did;

        Vid = *(volatile UINT16*)ME_PCIE_CFG_ADDRESS( ME_BUS, ME_DEVICE_NUMBER, HECI_MIN_FUNC, 0x00);

        Did = *(volatile UINT16*)ME_PCIE_CFG_ADDRESS( ME_BUS, ME_DEVICE_NUMBER, HECI_MIN_FUNC, 0x02);

        if( (Vid == 0xFFFF) && (Did == 0xFFFF) )
        {
            HfsData = HfsData & ((UINT32)(0x04 << 0x10));
        }
    }

    if( DeviceHide )
    {
        MmioWrite32( Address, Data );
    }

    return HfsData;
}

/**    
    This function will check ME is disable or not

    @retval EFI_SUCCESS - ME is disable.
    @retval EFI_WRITE_PROTECTED - ME is enable.
**/
EFI_STATUS
EFIAPI
CheckMeStateDisable (
    VOID
)
{
    UINT32      Value32;
    
    //Get HFSTS
    Value32 = GetHfs();
    //bit [16:19]
    Value32 &= (BIT16 | BIT17 | BIT18 | BIT19);
    Value32 = Value32 >> 0x10; 
    switch( Value32 )
    {
        case 0x03: //Soft Temporary Disable
        case 0x04: //SECOVER_JMPR
        case 0x05: //SECOVER_MEI_MSG
            return EFI_SUCCESS;
        break;

        default:
            return EFI_WRITE_PROTECTED;
        break;
    }
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
