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

/** @file SbCspIoDecodeLib.c
    This file contains code for South Bridge LPC IO Decode
    initialization in the Library stage

*/

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <Efi.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiSio.h>
#include <Library/PciCf8Lib.h>
#include <Sb.h>
#include <Library/IoLib.h>
#include <PchAccess.h>
#include <AmiCspLib.h>
//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

//TODO: Replace this with more generic code
#define ICH_REG_LPC_IBDF         0x6C
#define ICH_REG_LPC_IODEC        R_SB_LPC_IOD
#define ICH_REG_LPC_EN           R_SB_LPC_IOE
#define ICH_REG_LPC_GEN1_DEC     R_SB_LPC_GEN1_DEC
#define ICH_REG_LPC_GEN2_DEC     R_SB_LPC_GEN2_DEC
#define ICH_REG_LPC_GEN3_DEC     R_SB_LPC_GEN3_DEC
#define ICH_REG_LPC_GEN4_DEC     R_SB_LPC_GEN4_DEC
#define SIO_SB_BUS_NUMBER        SB_LPC_BUS
#define SIO_SB_DEV_NUMBER        SB_LPC_DEV
#define SIO_SB_FUNC_NUMBER       SB_LPC_FUN
/**
    This function set LPC Bridge Generic Decoding

    @param *LpcPciIo - Pointer to LPC PCI IO Protocol (This is not used but kept for backwards compatible)
    @param Base      - I/O base address
    @param Length    - I/O Length
    @param Enabled   - Enable/Disable the generic decode range register
    
    @retval EFI_SUCCESS - Set successfully.
    @retval EFI_UNSUPPORTED - This function is not implemented or the Length more than the maximum supported size of generic range decoding.
    @retval EFI_INVALID_PARAMETER - the Input parameter is invalid.
    @retval EFI_OUT_OF_RESOURCES - There is not available Generic Decoding Register.
    @retval EFI_NOT_FOUND - the generic decode range will be disabled is not found.

**/

EFI_STATUS
EFIAPI
AmiSioLibSetLpcGenericDecoding(
    IN VOID                     *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable
)
{
    // Porting Required
    UINT32                  IoGenDecode32, NumOfBdf;
    UINT16                  IoGenDecIndex;
    UINT16                  Buffer16;
    UINT8                   Bsf8 = 0;
    UINT8                   Bsr8 = 0;
    EFI_STATUS				Status = EFI_SUCCESS;
    EFI_PCI_IO_PROTOCOL     *LpcPciIoProtocol = (EFI_PCI_IO_PROTOCOL*)LpcPciIo;

    if (Length > 0x100) return EFI_UNSUPPORTED;

    if (Length == 0) return EFI_INVALID_PARAMETER;

    if (Length < 4) Length = 4;

    if (LpcPciIo==NULL) NumOfBdf = (UINT32)(PCI_CF8_LIB_ADDRESS(SIO_SB_BUS_NUMBER,SIO_SB_DEV_NUMBER, SIO_SB_FUNC_NUMBER, 0));

    // Read I/O Generic Decodes Register.
    for (IoGenDecIndex = 0; IoGenDecIndex < 4; IoGenDecIndex++) {
        //IoGenDecode32 = READ_PCI32_SB(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4);
        if (LpcPciIo==NULL){
            IoGenDecode32=PciCf8Read32(NumOfBdf+(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4));
        } else {
            Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint32,(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4),1,&IoGenDecode32);
            if (EFI_ERROR(Status)) return Status;
        }

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

//    MmPci32(LPC_BUS, LPC_DEVICE, LPC_FUNC, (ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4)) = IoGenDecode32; // 0x84
    if (LpcPciIo==NULL){
        PciCf8Write32(NumOfBdf+(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4),IoGenDecode32);
	} else {
        Status=LpcPciIoProtocol->Pci.Write(LpcPciIoProtocol, EfiPciIoWidthUint32,(ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4),1,&IoGenDecode32);
        if (EFI_ERROR(Status)) return Status;
    }
    //
    // Program LPC Generic IO Range #, PCR[DMI] + 2730h ~ 273Fh to the same value programmed in LPC/eSPI PCI Offset 84h~93h.
    //
    MMIO_WRITE32(SB_PCR_ADDRESS(PID_DMI, (UINT16) (R_PCH_PCR_DMI_LPCLGIR1 + IoGenDecIndex * 4)), IoGenDecode32);
    ///### Intel RC PchPcrWrite32 (PID_DMI, (UINT16) (R_PCH_PCR_DMI_LPCLGIR1 + Index * 4), Data32);
    
    // Porting End

    return EFI_SUCCESS;
}

/**
    This function sets LPC Bridge Device Decoding

    @param *LpcPciIo - Pointer to LPC PCI IO Protocol (This is not used but kept for backwards compatible)
    @param Base      - I/O base address, if Base is 0 means disabled the decode of the device
    @param DevUid    - The device Unique ID
    @param Type      - Device Type, please refer to AMISIO.h
    
    @retval EFI_SUCCESS - Set successfully.
    @retval EFI_UNSUPPORTED - There is not proper Device Decoding register for the device UID.

**/

EFI_STATUS
EFIAPI
AmiSioLibSetLpcDeviceDecoding(
    IN VOID                     *LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type
)
{
    EFI_STATUS              Status = EFI_UNSUPPORTED;
    UINT32                  NumOfBdf;
    // Porting Required
    UINT16                  ComRange[] = { 0x3f8, 0x2f8, 0x220, 0x228,\
                                           0x238, 0x2e8, 0x338, 0x3e8, 0x2f0, 0x2e0, 0};
    UINT16                  LptRange[] = { 0x378, 0x278, 0x3bc, 0};
    UINT16                  FpcRange[] = { 0x3f0, 0x370, 0};
    UINT16                  IoRangeMask16 = 0xffff;
    UINT16                  IoRangeSet16 = 0;
    UINT16                  IoEnMask16 = 0xffff;
    UINT16                  IoEnSet16 = 0;
    UINT8                   i;
    UINT16                  Tmp16;
    EFI_PCI_IO_PROTOCOL     *LpcPciIoProtocol = (EFI_PCI_IO_PROTOCOL*)LpcPciIo;

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
        case (dsUART) :
            if (Base == 0) {
                if (DevUid) IoEnMask16 &= ~BIT01;
                else IoEnMask16 &= ~BIT00;
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
        case (dsPS2M) :
        case (dsPS2CK) :
        case (dsPS2CM) :
            if (Base == 0) IoEnMask16 &= ~BIT10;
            else IoEnSet16 |= BIT10;
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
        default :
            if (Type != 0xFF)
                return EFI_UNSUPPORTED;

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
///                    if (Base == 0x62) IoEnSet16 |= BIT11;
///                    else {
///                    if (Base) AmiSioLibSetLpcGenericDecoding( LpcPciIo, \
///                                                           Base , \
///                                                           4, \
///                                                           TRUE );
///                        else return EFI_UNSUPPORTED;
///                    }
///                }
///            }
            break;
    }

    if (LpcPciIo==NULL) NumOfBdf = (UINT32)(PCI_CF8_LIB_ADDRESS(SIO_SB_BUS_NUMBER,SIO_SB_DEV_NUMBER, SIO_SB_FUNC_NUMBER, 0));

    //    RW_PCI16_SB(ICH_REG_LPC_IODEC, IoRangeSet16, ~IoRangeMask16);
    if (LpcPciIo==NULL){
        Tmp16 = PciCf8Read16((UINTN)NumOfBdf + ICH_REG_LPC_IODEC);
	} else {
        Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint16, ICH_REG_LPC_IODEC, 1, &Tmp16);
        if (EFI_ERROR(Status)) return Status;
    }
	
    Tmp16 = Tmp16&IoRangeMask16;
    Tmp16 = Tmp16|IoRangeSet16;
    
	if (LpcPciIo==NULL){    
        PciCf8Write16((UINTN)NumOfBdf + ICH_REG_LPC_IODEC, Tmp16);
    } else {
        Status=LpcPciIoProtocol->Pci.Write(LpcPciIoProtocol, EfiPciIoWidthUint16, ICH_REG_LPC_IODEC,1, &Tmp16);
        if (EFI_ERROR(Status)) return Status;
    }
    //
    // program LPC I/O Decode Ranges, PCR[DMI] + 2770h[15:0] to the same value programmed in LPC/eSPI PCI offset 80h.
    //
    MmioAndThenOr16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOD), IoRangeMask16, IoRangeSet16);
    
    //RW_PCI16_SB(ICH_REG_LPC_EN, IoEnSet16, ~IoEnMask16);
    if (LpcPciIo==NULL){
        Tmp16 = PciCf8Read16((UINTN)NumOfBdf + ICH_REG_LPC_EN);
	} else {
        Status=LpcPciIoProtocol->Pci.Read(LpcPciIoProtocol, EfiPciIoWidthUint16, ICH_REG_LPC_EN, 1, &Tmp16);
        if (EFI_ERROR(Status)) return Status;
    }
    Tmp16 = Tmp16&IoEnMask16;
    Tmp16 = Tmp16|IoEnSet16;
	
	if (LpcPciIo==NULL){    
        PciCf8Write16((UINTN)NumOfBdf + ICH_REG_LPC_EN,Tmp16);
    } else {
        Status=LpcPciIoProtocol->Pci.Write(LpcPciIoProtocol, EfiPciIoWidthUint16, ICH_REG_LPC_EN, 1, &Tmp16);
        if (EFI_ERROR(Status)) return Status;
    }

    //
    // program LPC I/O Decode Ranges, PCR[DMI] + 2774h[15:0] to the same value programmed in LPC/eSPI PCI offset 82h.
    //
    MmioAndThenOr16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOE), IoEnMask16, IoEnSet16);

    // Porting End

    return EFI_SUCCESS;
}

/**
    This function set LPC Bridge Generic Decoding
              
    @param LpcPciIo Pointer to LPC PCI IO Protocol
    @param Base I/O base address
    @param Length I/O Length
    @param Enabled Enable/Disable the generic decode range register

    @retval EFI_STATUS
            EFI_SUCCESS Set successfully.
            EFI_UNSUPPORTED This function is not implemented or the
                            Length more than the maximum supported
                            size of generic range decoding.
            EFI_INVALID_PARAMETER the Input parameter is invalid.
            EFI_OUT_OF_RESOURCES There is not available Generic
                                 Decoding Register.
            EFI_NOT_FOUND the generic decode range will be disabled
                          is not found.
**/
EFI_STATUS SbLibSetLpcGenericDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable )
{
    // Porting Required
    UINT32                  IoGenDecode32;
    UINT16                  IoGenDecIndex;
    UINT16                  Buffer16;
    UINT8                   Bsf8 = 0;
    UINT8                   Bsr8 = 0;

    if (Length > 0x100) return EFI_UNSUPPORTED;

    if (Length == 0) return EFI_INVALID_PARAMETER;

    if (Length < 4) Length = 4;

    // Read I/O Generic Decodes Register.
    for (IoGenDecIndex = 0; IoGenDecIndex < 4; IoGenDecIndex++) {
        IoGenDecode32 = READ_PCI32_SB(R_SB_LPC_GEN1_DEC + IoGenDecIndex * 4);
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

    WRITE_PCI32_SB(R_SB_LPC_GEN1_DEC + IoGenDecIndex * 4, IoGenDecode32);

    //
    // Program LPC Generic IO Range #, PCR[DMI] + 2730h ~ 273Fh to the same value programmed in LPC/eSPI PCI Offset 84h~93h.
    //
    MMIO_WRITE32(SB_PCR_ADDRESS(PID_DMI, (UINT16) (R_PCH_PCR_DMI_LPCLGIR1 + IoGenDecIndex * 4)), IoGenDecode32);
    ///### Intel RC PchPcrWrite32 (PID_DMI, (UINT16) (R_PCH_PCR_DMI_LPCLGIR1 + Index * 4), Data32);
    
    // Porting End

    return EFI_SUCCESS;
}

/**
    This function sets LPC Bridge Device Decoding
              
    @param LpcPciIo Pointer to LPC PCI IO Protocol
    @param Base I/O base address, if Base is 0 means disabled the
           decode of the device 
    @param DevUid The device Unique ID
    @param Type Device Type, please refer to AMISIO.h

    @retval EFI_STATUS
            EFI_SUCCESS Set successfully.
            EFI_UNSUPPORTED There is not proper Device Decoding 
                            register for the device UID.
**/
EFI_STATUS SbLibSetLpcDeviceDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type )
{
    EFI_STATUS              Status = EFI_UNSUPPORTED;

    // Porting Required
    UINT16                  ComRange[] = { 0x3f8, 0x2f8, 0x220, 0x228, \
                                         0x238, 0x2e8, 0x338, 0x3e8, 0};
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

        // COM x Address Range
        case (dsUART) :
            if (Base == 0) {
                switch (DevUid) {
                    case 0 :
                            IoEnMask16 &= ~BIT00;
                            break;
                    case 1 :
                            IoEnMask16 &= ~BIT01;
                            break;
                    case 2 :
                            break;
                    case 3 :
                            break;
                    default:
                            break;
                }
            } else {
                if ((Base == 0x3F8) || (Base == 0x3E8) || (Base == 0x2F8) || (Base == 0x2E8)) {
                    IoEnSet16 |= (BIT01 | BIT00);
                    IoRangeMask16 &= ~(BIT06 | BIT05 | BIT04 | BIT02 | BIT01 | BIT00);
                    IoRangeSet16 |= (BIT06 | BIT05 | BIT04);
                    SbLibSetLpcGenericDecoding(LpcPciIo, 0x200, 0x100, TRUE);
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
                    } else {
                            SbLibSetLpcGenericDecoding( LpcPciIo, \
                                                         Base , \
                                                         8, \
                                                         TRUE );
                    }
                }
            }
            break;

        // KBC Address Enable
        case (dsPS2K) :
        case (dsPS2M) :
        case (dsPS2CK) :
        case (dsPS2CM) :
            if (Base == 0) IoEnMask16 &= ~BIT10;
            else IoEnSet16 |= BIT10;
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

        // Other Address Enable
        default :
            if (Type != 0xFF)
                return EFI_UNSUPPORTED;

            switch (Base) {
                case (0x2e) :
                    IoEnSet16 |= BIT12;
                    break;
                case (0x4e) :
                    IoEnSet16 |= BIT13;
                    break;
                case (0x62) :
                case (0x63) :
                case (0x64) :
                case (0x65) :
                case (0x66) :
                    IoEnSet16 |= BIT11;
                    break;
                case (0) :
                    return EFI_UNSUPPORTED;
                    break;
                default :
                    SbLibSetLpcGenericDecoding( LpcPciIo, \
                                                 Base , \
                                                 4, \
                                                 TRUE );
                    break;
            }

//####            if (Base == 0x2e) IoEnSet16 |= BIT12;
//####            else {
//####                if (Base == 0x4e) IoEnSet16 |= BIT13;
//####                else {
//####                    if (Base) SbLib_SetLpcGenericDecoding( LpcPciIo, \
//####                                                           Base , \
//####                                                           4, \
//####                                                           TRUE );
//####                    else return EFI_UNSUPPORTED;
//####                }
//####            }

            break;
    }

    RW_PCI16_SB(R_PCH_LPC_IOD, IoRangeSet16, ~IoRangeMask16); // 0x80
    //
    // program LPC I/O Decode Ranges, PCR[DMI] + 2770h[15:0] to the same value programmed in LPC/eSPI PCI offset 80h.
    //
    MmioAndThenOr16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOD), IoRangeMask16, IoRangeSet16);
    //PchPcrWrite16 (PID_DMI, R_PCH_PCR_DMI_LPCIOD, LpcIoDecodeRanges);
    
    RW_PCI16_SB(R_PCH_LPC_IOE, IoEnSet16, ~IoEnMask16); // 0x82
    //
    // program LPC I/O Decode Ranges, PCR[DMI] + 2774h[15:0] to the same value programmed in LPC/eSPI PCI offset 82h.
    //
    MmioAndThenOr16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOE), IoEnMask16, IoEnSet16);
    //PchPcrWrite16 (PID_DMI, R_PCH_PCR_DMI_LPCIOE, LpcIoEnableDecoding);
    
    // Porting End

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

