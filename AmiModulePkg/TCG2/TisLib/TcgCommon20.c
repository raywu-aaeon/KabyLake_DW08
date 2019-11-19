//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Libraries/TisLib/TcgCommon.c 1     10/08/13 11:58a Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 11:58a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Libraries/TisLib/TcgCommon.c $
//
// 1     10/08/13 11:58a Fredericko
// Initial Check-In for Tpm-Next module
//
// 2     10/03/13 12:34p Fredericko
// Sha256 support policy update
//
// 1     7/10/13 5:50p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
// [Files]          TisLib.cif
// TisLib.mak
// TcgTpm12.h
// TpmLib.h
// TcgCommon.h
// ZTEICTcmOrdinals.h
// TpmLib.c
// TcgCommon.c
// TisLib.sdl
// sha1.h
// INTTcgAcpi.h
// TcgPc.h
// TcmPc.h
// TcgEfiTpm.h
// TcgEFI12.h
//
// 9     3/19/12 6:13p Fredericko
//
// 8     3/29/11 12:24p Fredericko
//
// 7     3/28/11 12:12p Fredericko
// [TAG]        EIP 54642
// [Category] Improvement
// [Description] 1. Checkin Files related to TCG function override
// 2. Include TCM and TPM auto detection
// [Files] Affects all TCG files
//
// 6     5/19/10 5:07p Fredericko
// Included File Header
// Included File Revision History
// Include \Updated AMI Function Headers
// Code Beautification
// EIP 37653
//
//*************************************************************************
/*++

   Copyright (c) 2005 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.


   Module Name:

   TcgCommon.c

   Abstract:

   TCG Commands implemented for both PEI and DXE

   --*/
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TcgCommon.c
//
// Description:
//  common TCG functions can be found here
//
//<AMI_FHDR_END>
//*************************************************************************
#include "AmiTcg/TcgCommon20.h"
#include "Token.h"
#include "AmiTcg/sha.h"
#include <AmiTcg/Tpm20.h>
#include <AmiTcg/TCGMisc.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/AmiTcgGuidIncludes.h>
#include <Guid/HobList.h>

UINT16
EFIAPI Tpm20TcgCommonH2NS(
    IN UINT16 Val )
{
    return TPM_H2NS( Val );
}

UINT32
EFIAPI Tpm20TcgCommonH2NL(
    IN UINT32 Val )
{
    return TPM_H2NL( Val );
}

VOID
EFIAPI Tpm20TcgCommonCopyMem(
    IN VOID  *CallbackContext,
    OUT VOID *Dest,
    IN VOID  *Src,
    IN UINTN Size )
{
    CHAR8 *Destination8;
    CHAR8 *Source8;

    if ( Src < Dest )
    {
        Destination8 = (CHAR8*) Dest + Size - 1;
        Source8      = (CHAR8*) Src + Size - 1;
        while ( Size-- )
        {
            *(Destination8--) = *(Source8--);
        }
    }
    else
    {
        Destination8 = (CHAR8*) Dest;
        Source8      = (CHAR8*) Src;
        while ( Size-- )
        {
            *(Destination8++) = *(Source8++);
        }
    }
}

// The patch is for Legacy back to UEFI, we need to re-calucate TcgEventCount and LastEventEntry
UINTN Tpm20GetLogEventCount(TCG_PCR_EVENT_HDR   *TcgLog)
{
    TCG_PCR_EVENT_HDR *TcgLogNext = TcgLog;
    UINTN   NextLoc =0;
    UINTN   Count=0;

    while( 1 )
    {
        if(TcgLogNext == NULL)break;
        if(TcgLogNext->PCRIndex > 24)
            break;
        if( TcgLogNext->EventType == 0 )
        {
            break;
        }
        NextLoc =  (UINTN)&TcgLogNext->EventSize;
        NextLoc =  (UINTN)(NextLoc)+ TcgLogNext->EventSize + sizeof(TcgLogNext->EventSize);
        TcgLogNext = (TCG_PCR_EVENT_HDR  *)NextLoc;
        ++Count;
    }
    return Count;
}



UINTN Tpm20FindNextLogLocation(TCG_PCR_EVENT_HDR   *TcgLog, UINTN EventNum)
{
    TCG_PCR_EVENT_HDR *TcgLogNext = TcgLog;
    UINTN   NextLoc =0;
    UINTN i=0;

    if(EventNum == 0) return ((UINTN) TcgLogNext);;
    //return the lcation of the next log
    for(i=0; i<EventNum; i++)
    {
        if(TcgLogNext == NULL)break;
        if(TcgLogNext->PCRIndex > 24)
            break;
        NextLoc =  (UINTN)&TcgLogNext->EventSize;
        NextLoc =  (UINTN)(NextLoc)+ TcgLogNext->EventSize + sizeof(TcgLogNext->EventSize);
        TcgLogNext = (TCG_PCR_EVENT_HDR  *)NextLoc;
    }
    return ((UINTN) TcgLogNext);
}



EFI_STATUS
EFIAPI Tpm20TcgCommonLogEvent(
    IN VOID          *CallbackContext,
    IN TCG_PCR_EVENT *EvtLog,
    IN OUT UINT32    *TableSize,
    IN UINT32        MaxSize,
    IN TCG_PCR_EVENT *NewEntry,
    IN UINT8         HashAlgorithm )
{
    UINT32    TempSize;

    //protect Tcglog from possible overflow log operation
    if(NewEntry->EventSize > ((UINT32)(~0) - sizeof(EFI_TCG_PCR_EVENT_HEADER)))
    {
        return EFI_OUT_OF_RESOURCES;
    }

    TempSize = sizeof(TCG_PCR_EVENT)-sizeof(NewEntry->Digest) - sizeof(UINT32)-1;
    Tpm20TcgCommonCopyMem( CallbackContext, EvtLog, NewEntry, TempSize );

    if(HashAlgorithm == 0)
    {
        Tpm20TcgCommonCopyMem( CallbackContext, (((UINT8 *)EvtLog) + TempSize), (UINT8 *)&NewEntry->Digest.digest, sizeof(NewEntry->Digest.digest) );
        TempSize+=sizeof(NewEntry->Digest.digest);
        Tpm20TcgCommonCopyMem( CallbackContext, (((UINT8 *)EvtLog) + TempSize), (UINT8 *)&NewEntry->EventSize, sizeof(UINT32));
        TempSize+=sizeof(UINT32);
        Tpm20TcgCommonCopyMem( CallbackContext, (((UINT8 *)EvtLog) + TempSize), NewEntry->Event, NewEntry->EventSize);
    }

    *TableSize += (TempSize + NewEntry->EventSize);
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI Tpm20SHA1HashAll(
    IN VOID            *CallbackContext,
    IN VOID            *HashData,
    IN UINTN           HashDataLen,
    OUT UINT8         *Digest
)
{
    SHA1_CTX       Sha1Ctx;
    unsigned char  DigestArray[20];

    SHA1Init( &Sha1Ctx );

    SHA1Update( &Sha1Ctx, HashData, (u32)HashDataLen );

    SHA1Final( DigestArray, &Sha1Ctx );

    Tpm20TcgCommonCopyMem(
        CallbackContext,
        Digest,
        DigestArray,
        sizeof (DigestArray));

    return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI SHA2HashAll(
    IN  VOID            *CallbackContext,
    IN  VOID            *HashData,
    IN  UINTN           HashDataLen,
    OUT UINT8           *Digest
)
{
    SHA2_CTX       Sha2Ctx;
    unsigned char  DigestArray[32];

    sha256_init( &Sha2Ctx );

    sha256_process( &Sha2Ctx, HashData, (u32)HashDataLen );

    sha256_done( &Sha2Ctx, DigestArray );

    Tpm20TcgCommonCopyMem(
        CallbackContext,
        Digest,
        DigestArray,
        sizeof (DigestArray));

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI SHA384HashAll(
    IN  VOID            *HashData,
    IN  UINTN           HashDataLen,
    OUT UINT8           *Digest
)
{
    unsigned char    DigestArray[48];
    VOID            *CallbackContext=NULL;
    const unsigned char   *Addr[1];
    UINTN           Len[1];

    Addr[0] = (unsigned char*)HashData;
    Len[0]  = HashDataLen;
    sha384_vector(1, Addr, Len, DigestArray);
    Tpm20TcgCommonCopyMem(CallbackContext, Digest, DigestArray, sizeof (DigestArray));
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI SHA512HashAll(
    IN  VOID            *HashData,
    IN  UINTN           HashDataLen,
    OUT UINT8           *Digest
)
{
    unsigned char   DigestArray[64];
    VOID            *CallbackContext=NULL;
    const unsigned char   *Addr[1];
    UINTN           Len[1];

    Addr[0] = (unsigned char*)HashData;
    Len[0]  = HashDataLen;

    sha384_vector(1, Addr, Len, DigestArray);
    Tpm20TcgCommonCopyMem(CallbackContext, Digest, DigestArray, sizeof (DigestArray));

    return EFI_SUCCESS;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetHob
//
// Description: Find instance of a HOB type in a HOB list
//
// Input:
//      Type          The HOB type to return.
//      HobStart      The first HOB in the HOB list.
//
// Output:
//      Pointer to the Hob matching the type or NULL
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
VOID* Tpm20GetHob(
    IN UINT16 Type,
    IN VOID   *HobStart  )
{
    EFI_PEI_HOB_POINTERS Hob;

    Hob.Raw = HobStart;

    //
    // Return input if not found
    //
    if ( HobStart == NULL )
    {
        return HobStart;
    }

    //
    // Parse the HOB list, stop if end of list or matching type found.
    //
    while ( !TCGEND_OF_HOB_LIST( Hob ))
    {
        if ( Hob.Header->HobType == Type )
        {
            break;
        }

        Hob.Raw = TCGGET_NEXT_HOB( Hob );
    }

    //
    // Return input if not found
    //
    if ( TCGEND_OF_HOB_LIST( Hob ))
    {
        return HobStart;
    }

    return (VOID*)(Hob.Raw);
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgGetNextGuidHob
//
// Description: Find GUID HOB
//
// Input:       HobStart    A pointer to the start hob.
//              Guid        A pointer to a guid.
// Output:
//              Buffer          A pointer to the buffer.
//              BufferSize      Buffer size.
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS Tpm20TcgGetNextGuidHob(
    IN OUT VOID          **HobStart,
    IN EFI_GUID          * Guid,
    OUT VOID             **Buffer,
    OUT UINTN            *BufferSize OPTIONAL )
{
    EFI_STATUS           Status;
    EFI_PEI_HOB_POINTERS GuidHob;

    if ( Buffer == NULL )
    {
        return EFI_INVALID_PARAMETER;
    }

    for ( Status = EFI_NOT_FOUND; EFI_ERROR( Status );)
    {
        GuidHob.Raw = *HobStart;

        if ( TCGEND_OF_HOB_LIST( GuidHob ))
        {
            return EFI_NOT_FOUND;
        }

        GuidHob.Raw = Tpm20GetHob( EFI_HOB_TYPE_GUID_EXTENSION, *HobStart );

        if ( GuidHob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION )
        {
            if (!CompareMem( Guid, &GuidHob.Guid->Name, sizeof(EFI_GUID) ))
            {
                Status  = EFI_SUCCESS;
                *Buffer = (VOID*)((UINT8*)(&GuidHob.Guid->Name)
                                  + sizeof (EFI_GUID));

                if ( BufferSize != NULL )
                {
                    *BufferSize = GuidHob.Header->HobLength
                                  - sizeof (EFI_HOB_GUID_TYPE);
                }
            }
        }

        *HobStart = TCGGET_NEXT_HOB( GuidHob );
    }

    return Status;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   CompareGuid
//
// Description: Compares two input GUIDs
//
// Input:       Comparision status
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
/*BOOLEAN CompareGuid(
    EFI_GUID *G1,
    EFI_GUID *G2 )
{
    UINT32 *p1 = (UINT32*)G1, *p2 = (UINT32*)G2;
    UINTN  i;

    for ( i = 0; i < 4; ++i )
    {
        if ( p1[i] != p2[i] )
        {
            return FALSE;
        }
    }
    return TRUE;
    ;
}*/

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   LocateATcgHob
//
// Description:
//
// Input:
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
VOID* Tpm20LocateATcgHob(
    UINTN                   NoTableEntries,
    EFI_CONFIGURATION_TABLE *ConfigTable,
    EFI_GUID                *HOB_guid )
{
    VOID *HobStart;
    VOID *PtrHob;

    while ( NoTableEntries > 0 )
    {
        NoTableEntries--;

        if ((!CompareMem(
                    &ConfigTable[NoTableEntries].VendorGuid,
                    &gEfiHobListGuid, sizeof(EFI_GUID)
                )))
        {
            HobStart = ConfigTable[NoTableEntries].VendorTable;

            if ( !EFI_ERROR(
                    Tpm20TcgGetNextGuidHob( &HobStart, HOB_guid, &PtrHob, NULL )
                    ))
            {
                return PtrHob;
            }
        }
    }
    return NULL;
}

