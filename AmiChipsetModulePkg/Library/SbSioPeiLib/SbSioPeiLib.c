//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  <SbSioPeiLib.c>
//
// Description: Porting for PEI phase.Just for necessary devices porting.
//
//
//<AMI_FHDR_END>
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <Sb.h>
#include <AmiPeiLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiSio.h>
#include <Library/AmiSioPeiLib.h>
#include <Library/PciCf8Lib.h>
#include <Register/PchRegsDmi.h>

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------
#define ICH_LPC_IO_DECODE_OFFSET    R_SB_LPC_IOD
#define ICH_LPC_IO_ENABLE_OFFSET    R_SB_LPC_IOE
#define ICH_LPC_REG_GEN1_DEC_OFFSET R_SB_LPC_GEN1_DEC
#define SIO_SB_BUS_NUMBER           SB_BUS
#define SIO_SB_DEV_NUMBER           SB_LPC_DEV
#define SIO_SB_FUNC_NUMBER          SB_LPC_FUN


//<AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:   CSP_SetLpcGenericDecoding
//
// Description:
//  This function set LPC Bridge Generic Decoding
//
// Input:
//  *LpcPciIo - Pointer to LPC PCI IO Protocol
//  Base      - I/O base address
//  Length    - I/O Length
//  Enabled   - Enable/Disable the generic decode range register
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_UNSUPPORTED - This function is not implemented or the
//                    Length more than the maximum supported
//                    size of generic range decoding.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//  EFI_OUT_OF_RESOURCES - There is not available Generic
//                         Decoding Register.
//  EFI_NOT_FOUND - the generic decode range will be disabled
//                  is not found.
//
// Notes:
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AmiSioLibSetLpcGenericDecoding (
    IN VOID				       *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable )
{
    // Porting Required
    UINT32                  IoGenDecode32,NumOfBdf;
    UINT16                  IoGenDecIndex;
    UINT16                  Buffer16;
    UINT8                   Bsf8 = 0;
    UINT8                   Bsr8 = 0;

    if (Length > 0x100) return EFI_UNSUPPORTED;

    if (Length == 0) return EFI_INVALID_PARAMETER;

    if (Length < 4) Length = 4;

    //NumOfBdf = (UINT32) (SIO_SB_BUS_NUMBER << 16)|(SIO_SB_DEV_NUMBER << 11) |(SIO_SB_FUNC_NUMBER << 8);
    NumOfBdf = (UINT32)(PCI_CF8_LIB_ADDRESS(SIO_SB_BUS_NUMBER,SIO_SB_DEV_NUMBER, SIO_SB_FUNC_NUMBER, 0));

    // Read I/O Generic Decodes Register.
    for (IoGenDecIndex = 0; IoGenDecIndex < 4; IoGenDecIndex++) {
        //IoGenDecode32 = ReadPciReg32(NumOfBdf,ICH_LPC_REG_GEN1_DEC_OFFSET + IoGenDecIndex * 4);
    	IoGenDecode32=PciCf8Read32(NumOfBdf+(ICH_LPC_REG_GEN1_DEC_OFFSET + IoGenDecIndex * 4));

        if (Enable) {
            if ((IoGenDecode32 & 1) == 0) break;
            else if ((IoGenDecode32 & 0xfffc) == Base) break;
        } else {
            if (((IoGenDecode32 & 0xfffc) == Base) && (IoGenDecode32 & 1)) {
                IoGenDecode32 = 0; // Disable & clear the base/mask fields
                break;
            }
        }
    }

    if (Enable) {
        if (IoGenDecIndex == 4) return EFI_OUT_OF_RESOURCES;

        Buffer16 = Length;
        while ((Buffer16 % 2) == 0) {
            Buffer16 /= 2;
            Bsf8++;
        }

        while (Length) {
            Length >>= 1;
            Bsr8++;
        }

        if (Bsf8 == (Bsr8 - 1)) Bsr8--;

        Length = (1 << Bsr8) - 1 ;

        Base &= (~Length);

        IoGenDecode32 = Base | (UINT32)((Length >> 2) << 18) | 1;

    } else {
        if (IoGenDecIndex == 4) return EFI_NOT_FOUND;
    }

    //WritePciReg32(NumOfBdf,ICH_LPC_REG_GEN1_DEC_OFFSET + IoGenDecIndex * 4, IoGenDecode32);
    PciCf8Write32(NumOfBdf+(ICH_LPC_REG_GEN1_DEC_OFFSET + IoGenDecIndex * 4),IoGenDecode32);

    //
    // Program LPC Generic IO Range #, PCR[DMI] + 2730h ~ 273Fh to the same value programmed in LPC/eSPI PCI Offset 84h~93h.
    //
    MMIO_WRITE32(SB_PCR_ADDRESS(PID_DMI, (UINT16) (R_PCH_PCR_DMI_LPCLGIR1 + IoGenDecIndex * 4)), IoGenDecode32);
    ///### Intel RC PchPcrWrite32 (PID_DMI, (UINT16) (R_PCH_PCR_DMI_LPCLGIR1 + Index * 4), Data32);
    
    // Porting End

    return EFI_SUCCESS;

}


//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:  CSP_SetLpcDeviceDecoding
//
// Description:
//  This function goes through the elinked list of identify functions
//  giving control when the token "IODECODETYPE == 1".
//
// Input:
//  Base    - I/O base address
//            Base=0 means disable the decode of the device
//  DevUid  - The device Unique ID
//            If type is 0xFF, DevUid contain the IO length
//  Type    - Device type
//            If type is 0xFF, DevUid contain the IO length
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//
// Notes:
//  Chipset porting should provide the Io Ranage decode function.
//  If chipset porting provide this function, set IODECODETYPE = 0.
//  If chipset porting doesn't provide this function, you can eLink your
//  function to IoRangeDecodeList or replace CSP_SetLpcDeviceDecoding elink
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AmiSioLibSetLpcDeviceDecoding(
    IN    VOID           *LpcPciIo,
    IN    UINT16         Base,
    IN    UINT8          DevUid,
    IN    SIO_DEV_TYPE   Type)
{
    UINT32                  NumOfBdf;
//	    UINT16                  ComRange[] = { 0x3f8, 0x2f8, 0x220, 0x228,\
//	                                           0x238, 0x2e8, 0x338, 0x3e8, 0};
    UINT16                  ComRange[] = { 0x3f8, 0x2f8, 0x220, 0x228,\
                                           0x238, 0x2e8, 0x338, 0x3e8, 0x2f0, 0x2e0, 0};
    UINT16                  LptRange[] = { 0x378, 0x278, 0x3bc, 0};
    UINT16                  FpcRange[] = { 0x3f0, 0x370, 0};
    UINT16                  IoRangeMask16 = 0xffff;
    UINT16                  IoRangeSet16 = 0;
    UINT16                  IoEnMask16 = 0xffff;
    UINT16                  IoEnSet16 = 0;
    UINT8                   i;

    switch (Type) {
        // FDC Address Range
        case (dsFDC) :
            if (Base == 0) IoEnMask16 &= ~BIT03;
            else {
                for (i = 0; (FpcRange[i] != 0) && (FpcRange[i] != Base); i++);
                if (FpcRange[i]) {
                    IoEnSet16 |= BIT03;
                    IoRangeMask16 &= ~BIT12;
                    IoRangeSet16 |= (i << 12);
                }
                else return EFI_UNSUPPORTED;
            }
            break;

        // LPT Address Range
        case (dsLPT) :
            if (Base == 0) IoEnMask16 &= ~BIT02;
            else {
                for (i = 0; (LptRange[i] != 0) && (LptRange[i] != Base); i++);
                if (LptRange[i]) {
                    IoEnSet16 |= BIT02;
                    IoRangeMask16 &= ~(BIT09 | BIT08);
                    IoRangeSet16 |= (i << 8);
                } else return EFI_UNSUPPORTED;
            }
            break;

        // ComA Address Range
        case (dsCIR) :
        case (dsUART) :
            if (Base == 0) {//Dxe Phase to Close PEI phase opened decode
                if (DevUid==2) IoEnMask16 &= ~BIT01;//close COMB decode
                else if (DevUid==1) IoEnMask16 &= ~BIT00; //close COMA decode
            } else {
                if ((Base == 0x3F8) || (Base == 0x3E8) || (Base == 0x2F8) || (Base == 0x2E8)) {
                    IoEnSet16 |= (BIT01 | BIT00);
                    IoRangeMask16 &= ~(BIT06 | BIT05 | BIT04 | BIT02 | BIT01 | BIT00);
                    IoRangeSet16 |= (BIT06 | BIT05 | BIT04);
                    AmiSioLibSetLpcGenericDecoding(LpcPciIo, 0x200, 0x100, TRUE);
                } else {
                    if (DevUid < 2) {
                        for (i = 0; (ComRange[i] != 0) && (ComRange[i] != Base); i++);
                        if (ComRange[i]) {
                            if (DevUid) {
                                IoEnSet16 |= BIT01;
                                IoRangeMask16 &= ~(BIT06 | BIT05 | BIT04);
                                IoRangeSet16 |= (i << 4);
                            } else {
                                IoEnSet16 |= BIT00;
                                IoRangeMask16 &= ~(BIT02 | BIT01 | BIT00);
                                IoRangeSet16 |= i;
                            }
                        } else return EFI_UNSUPPORTED;
                    } 
                }
            }
            break;

        // KBC Address Enable
        case (dsPS2K) :
        case (dsPS2CK) :
            if (Base == 0) IoEnMask16 &= ~BIT10;
            else IoEnSet16 |= BIT10;
            break;

        case (dsPS2M) :
        case (dsPS2CM) :
            break;
        // Game Port Address Enable
        case (dsGAME) :
            if (Base == 0) IoEnMask16 &= ~(BIT09 | BIT08);
            else {
                if (Base == 0x200) {
                    IoEnSet16 |= BIT08;
                } else {
                    if (Base == 0x208) IoEnSet16 |= BIT09;
                    else return EFI_UNSUPPORTED;
                }
            }
            break;

        // LPC CFG Address Enable
        case (0xff) :
            switch(Base)
            {
                case 0:
                    return EFI_UNSUPPORTED;
                    break;
                case 0x2e:
                    IoEnSet16 |= BIT12;
                    break;
                case 0x4e:
                    IoEnSet16 |= BIT13;
                    break;
                case 0x62:
                case 0x63:
                case 0x64:
                case 0x65:
                case 0x66:
                    IoEnSet16 |= BIT11;
                    break;
                default:
                    AmiSioLibSetLpcGenericDecoding( LpcPciIo, \
                                                    Base , \
                                                    DevUid, \
                                                    TRUE );
                    return EFI_SUCCESS;
            } // end of switch
///            if (Base == 0x2e) IoEnSet16 |= BIT12;
///            else {
///                if (Base == 0x4e) IoEnSet16 |= BIT13;
///                else {
///                    //!!!Attention!!!If type is 0xFF, DevUid contain the IO length
///                    if (Base) {
///                        AmiSioLibSetLpcGenericDecoding( LpcPciIo, \
///                                                      Base , \
///                                                     DevUid, \
///                                                       TRUE );
///                        return EFI_SUCCESS;
///                    } else return EFI_UNSUPPORTED;
///                }
///            }
            break;

        default :
            return EFI_UNSUPPORTED;
    }

    NumOfBdf = (UINT32)(PCI_CF8_LIB_ADDRESS(SIO_SB_BUS_NUMBER,SIO_SB_DEV_NUMBER, SIO_SB_FUNC_NUMBER, 0));
    //NumOfBdf=(UINT32)(SIO_SB_BUS_NUMBER << 16) | (SIO_SB_DEV_NUMBER << 11) | (SIO_SB_FUNC_NUMBER << 8);


    //  RWPciReg16(NumOfBdf,ICH_LPC_IO_DECODE_OFFSET, IoRangeSet16, ~IoRangeMask16);	//0X80
    PciCf8AndThenOr16((UINTN)NumOfBdf+ICH_LPC_IO_DECODE_OFFSET,IoRangeMask16, IoRangeSet16);

    //
    // program LPC I/O Decode Ranges, PCR[DMI] + 2770h[15:0] to the same value programmed in LPC/eSPI PCI offset 80h.
    //
    MmioAndThenOr16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOD), IoRangeMask16, IoRangeSet16);
    
    //RWPciReg16(NumOfBdf,ICH_LPC_IO_ENABLE_OFFSET, IoEnSet16, ~IoEnMask16);		//0X82
    PciCf8AndThenOr16(NumOfBdf+ICH_LPC_IO_ENABLE_OFFSET,IoEnMask16,IoEnSet16);
    
    //
    // program LPC I/O Decode Ranges, PCR[DMI] + 2774h[15:0] to the same value programmed in LPC/eSPI PCI offset 82h.
    //
    MmioAndThenOr16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOE), IoEnMask16, IoEnSet16);
    // Porting End

    return EFI_SUCCESS;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: ProgramRtRegisterTable
//
// Description:
//  This function will program the runtime register.
//
// Input:
//  IN  UINT16  Base - Runtime register IO base
//  IN  SIO_DEVICE_INIT_DATA  *Table - initial table
//  IN  UINT8   Count - table data count
// Output:
//  NONE
//
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
void ProgramRtRegisterTable(
    IN  UINT16  Base,
    IN  SIO_DEVICE_INIT_DATA  *Table,
    IN  UINT8   Count
)
{
    UINT8   i;
    UINT8   Value8;

    for(i=0;i<Count;i++) {
        //Some registers may not allow to read, and also we can write some registers without any read operations. 
        if(Table[i].AndData8 == 0x00)  Value8 = Table[i].OrData8;
        else Value8 = (IoRead8(Base+Table[i].Reg16) & Table[i].AndData8) | Table[i].OrData8;
        IoWrite8(Base+Table[i].Reg16, Value8 );
    }
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

