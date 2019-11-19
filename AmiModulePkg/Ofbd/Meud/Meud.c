//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file Meud.c

**/
#include "Efi.h"
#include "Token.h"
#include <Library/DebugLib.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "Meud.h"
#include <Ofbd.h>
#include <Protocol/SmiFlash.h>
#include <Flash.h>
#include <Library/AmiBufferValidationLib.h>

extern FLASH_REGIONS_DESCRIPTOR FlashRegionsDescriptor[];

EFI_SMI_FLASH_PROTOCOL *gSmiFlash;
UINT32 gFlashCapacity;

/**
    Locate SmiFlash protocol callback

    @param Event
    @param Context

    @retval VOID

**/
#if defined(PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x0001000A)
EFI_STATUS
EFIAPI
MeudCallback(
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
#else
VOID
MeudCallback(
    EFI_EVENT   Event,
    VOID        *Context
)
#endif
{
    EFI_GUID    EfiSmiFlashProtocolGuid = EFI_SMI_FLASH_GUID;
    EFI_STATUS  Status;

#if (PI_SPECIFICATION_VERSION >= 0x1000A)
    Status = pSmst->SmmLocateProtocol( &EfiSmiFlashProtocolGuid, NULL, &gSmiFlash );
    return EFI_SUCCESS;
#else
    Status = pBS->LocateProtocol( &EfiSmiFlashProtocolGuid, NULL, &gSmiFlash );
    if( EFI_ERROR(Status) ) gSmiFlash = NULL:
#endif
}
/**
    OFBD ME Firmware Update InSmm Function

    @param VOID

    @retval VOID

**/
VOID
MeudInSmm(
    VOID
)
{
    EFI_STATUS              Status;
    EFI_GUID                EfiSmmBase2ProtocolGuid = EFI_SMM_BASE2_PROTOCOL_GUID;
    EFI_GUID                EfiSmiFlashProtocolGuid = EFI_SMI_FLASH_GUID;
    EFI_SMM_BASE2_PROTOCOL  *SmmBase2;
    EFI_SMM_SYSTEM_TABLE2   *SmmSystemTable2 = NULL;

    gFlashCapacity = GetFlashCapacity();

    Status = pBS->LocateProtocol( &EfiSmmBase2ProtocolGuid, NULL, &SmmBase2 );
    DEBUG((DEBUG_INFO,"The Status of locate protocol(EfiSmmBase2ProtocolGuid) is %r\n", Status));
    SmmBase2->GetSmstLocation( SmmBase2, &SmmSystemTable2 );

#if (PI_SPECIFICATION_VERSION >= 0x1000A)
    Status = pSmst->SmmLocateProtocol( &EfiSmiFlashProtocolGuid, NULL, &gSmiFlash );
#else
    Status = pBS->LocateProtocol( &EfiSmiFlashProtocolGuid, NULL, &gSmiFlash );
#endif

    if( EFI_ERROR(Status) )
    {
#if defined(PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x0001000A)
        VOID    *ProtocolNotifyRegistration;
        Status = pSmst->SmmRegisterProtocolNotify(
                            &EfiSmiFlashProtocolGuid,
                            MeudCallback,
                            &ProtocolNotifyRegistration );
#else
        EFI_EVENT   SmiFlashCallbackEvt;
        VOID        *MeudReg;
        RegisterProtocolCallback(
            &EfiSmiFlashProtocolGuid,
            MeudCallback,
            NULL,
            &SmiFlashCallbackEvt,
            &MeudReg );
#endif
    }
    CspMeudInSmm();
}
/**
    OFBD ME Firmware Update Entry point

    @param Buffer
    @param pOFBDDataHandled

    @retval VOID

**/
VOID
MeudEntry(
    IN      VOID    *Buffer,
    IN  OUT UINT8   *OfbdDataHandled
)
{
    OFBD_HDR                        *OfbdHdr;
    OFBD_EXT_HDR                    *OfbdExtHdr;
    OFBD_TC_55_MEUD_STRUCT          *MeudStructPtr;
    EFI_STATUS                      Status = EFI_SUCCESS;
    UINT32                          Buffer1, Buffer2;
    OFBD_TC_55_ME_INFO_STRUCT       *MeInfoStructPtr;
    OFBD_TC_55_ME_PROCESS_STRUCT    *MeProcessStructPtr;
    OFBD_TC_55_ME_VERSION_STRUCT    *MeVersionStructPtr;

    OfbdHdr = (OFBD_HDR*)Buffer;
    OfbdExtHdr = (OFBD_EXT_HDR*)((UINT8*)Buffer + (sizeof(OFBD_HDR)));
    if (EFI_ERROR(AmiValidateMemoryBuffer (OfbdExtHdr, sizeof(OFBD_EXT_HDR)))) return;
    MeudStructPtr = (OFBD_TC_55_MEUD_STRUCT*)((UINT8*)OfbdExtHdr + sizeof(OFBD_EXT_HDR));
    if (EFI_ERROR(AmiValidateMemoryBuffer (MeudStructPtr, sizeof(OFBD_TC_55_MEUD_STRUCT)))) return;
    MeInfoStructPtr = (OFBD_TC_55_ME_INFO_STRUCT*)MeudStructPtr;
    if (EFI_ERROR(AmiValidateMemoryBuffer (MeInfoStructPtr, sizeof(OFBD_TC_55_ME_INFO_STRUCT)))) return;
    MeProcessStructPtr = (OFBD_TC_55_ME_PROCESS_STRUCT*)MeudStructPtr;
    if (EFI_ERROR(AmiValidateMemoryBuffer (MeProcessStructPtr, sizeof(OFBD_TC_55_ME_PROCESS_STRUCT)))) return;

    if( *OfbdDataHandled == 0 )
    {
        if( OfbdHdr->OFBD_FS & OFBD_FS_MEUD )
        {
            //Check Type Code ID
            if( OfbdExtHdr->TypeCodeID == OFBD_EXT_TC_MEUD )
            {
                switch( MeudStructPtr->bSubFunction )
                {
                    //case 0, case 1 and case2 are for Me Ignition FW, It's won't support in A5 project
                    case 0 :
                    case 1 :
                    case 2 :
                        *OfbdDataHandled = 0xFE;
                        return;

                    case 3 :
                        Status = CspReportMeInfo( 3, \
                                    &(MeudStructPtr->dMERuntimeBase), \
                                    &(MeudStructPtr->dMERuntimeLength) );

                        MeudStructPtr->dMEBiosRegionBase = \
                                                    gFlashCapacity - FLASH_SIZE;

#if defined(OEM_REGION_ORDER) && (OEM_REGION_ORDER == 1)
                        MeudStructPtr->dMEBiosRegionBase = MeudStructPtr->dMEBiosRegionBase - OEM_REGION_SIZE;
#endif

                        MeudStructPtr->dMEBiosRegionLength = (UINT32)FLASH_SIZE;

                        // If FlashCapacity == 0, Not support ME Update
                        if( gFlashCapacity == 0 )
                        {
                            // Fail , Return
                            *OfbdDataHandled = 0xFE;
                            return;
                        }

                        *OfbdDataHandled = 0xFF;
                        MeudStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        return;

                    case 4 :
                        // Send this again to check is this suppoet.
                        Status = CspReportMeInfo( 3, \
                                    &Buffer1, \
                                    &Buffer2 );
                        if( EFI_ERROR(Status) )
                        {
                            // Fail , Return
                            *OfbdDataHandled = 0xFE;
                            return;
                        }
                        MeudStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        *OfbdDataHandled = 0xFF;
                        gSmiFlash->FlashCapacity = gFlashCapacity;

                        return;

                    case 5 :
                        gSmiFlash->FlashCapacity = FLASH_SIZE;
                        MeudStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        *OfbdDataHandled = 0xFF;
                        return;

                    default :
                        *OfbdDataHandled = 0xFE;
                        break;
// =============== OFBD Version 2.1 and AFU version 2.37 or newer ===============
#if (OFBD_VERSION >= 0x0210)

                    case 9 :
                    // Get Info
                    {
                        UINT8    Index = 0;
                        UINT16   TotalBlocks = 0;

                        //Search for all regions
                        for( Index = 0; Index < MAX_BLK; Index++ )
                        {
                            if( FlashRegionsDescriptor[Index].FlashRegion == MAX_BLK )
                                break;

                            Status = GetRegionOffset(
                                        FlashRegionsDescriptor[Index].FlashRegion,
                                        &MeInfoStructPtr->BlockInfo[TotalBlocks].StartAddress,
                                        &MeInfoStructPtr->BlockInfo[TotalBlocks].BlockSize );
                            if( !EFI_ERROR(Status) )
                            {
                                UINT8   String[32];

                                MemCpy(
                                    MeInfoStructPtr->BlockInfo[TotalBlocks].Command,
                                    FlashRegionsDescriptor[Index].Command,
                                    4 );

                                MemCpy( &String[0], "Flash ", 6 );
                                MemCpy( &String[6], FlashRegionsDescriptor[Index].Command, 4 );
                                MemCpy( &String[10], " Region", 8 );

                                MemCpy(
                                    MeInfoStructPtr->BlockInfo[TotalBlocks].Description,
                                    String,
                                    18 );

                                MeInfoStructPtr->BlockInfo[TotalBlocks].Type = FlashRegionsDescriptor[Index].FlashRegion;

                                // Status = 1 means Protect
                                MeInfoStructPtr->BlockInfo[TotalBlocks].Status = 0;

                                TotalBlocks += 1;
                            }
                        }

                        MeInfoStructPtr->TotalBlocks = TotalBlocks;
                        *OfbdDataHandled = 0xFF;
                        MeudStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        break;
                    }

                    case 10 :
                        // ME Process Handle
                        // In CSP_MEUD.c
                        MEProcessHandler( &MeProcessStructPtr );
                        *OfbdDataHandled = 0xFF;
                        MeudStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        break;

                    case 0x0B :
                        //New function to indicate AFU process image wiht ME 12
                        MeVersionStructPtr = (OFBD_TC_55_ME_VERSION_STRUCT*)MeudStructPtr;
                        CspMeudReportFirmwareType( MeVersionStructPtr );
                        *OfbdDataHandled = 0xFF;
                        break;

                    case 0x0C :
                        // ME Process Handle case 0x81, 0x82, 0x83 for NonBIOS_Validation feature
#if defined(NonBIOS_Validation) && (NonBIOS_Validation == 1)
                        MEProcessHandler( &MeProcessStructPtr );
                        *OfbdDataHandled = 0xFF;
                        MeudStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
#else
                        *OfbdDataHandled = 0xFE;
#endif
                        break;

#endif //#if (OFBD_VERSION >= 0x0210)
                }// End of Switch
            }
        }
    }
    return;
}
