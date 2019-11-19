//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
/** @file BootGuardTPMPei.c
    TPM Initialization Flow for Boot Guard

**/
//**********************************************************************

#include <Efi.h>
#include <Pei.h>
#include "AmiTcg/TcgCommon.h"
#include <AmiPeiLib.h>
#include "AmiTcg/TcgMisc.h"
#include <PPI/TcgTcmService.h>
#include <Ppi/TcgService.h>
#include <Ppi/TpmDevice.h>
#include "PPI/CpuIo.h"
#include "PPI/LoadFile.h"
#include <Ppi/TcgPlatformSetupPeiPolicy.h>
#include <AmiTcg/AmiTcgPlatformPei.h>
#include <Ppi/AmiTcgPlatformPpi.h>
#include <Token.h>
#include <CpuRegs.h>
#include <AmiCspLibInc.h>
#include <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>
#include "BootGuardTPMPei.h"

static
EFI_STATUS
__stdcall __FillCallbackContext(
    IN EFI_PEI_SERVICES          **PeiService,
    OUT TCG_PEI_CALLBACK_CONTEXT *CallbackContext )
{
    EFI_GUID _gPeiTpmPpiGuid = PEI_TPM_PPI_GUID;
    CallbackContext->PeiServices = PeiService;
    return (*PeiService)->LocatePpi(
               PeiService,
               &_gPeiTpmPpiGuid,
               0,
               NULL,
               &CallbackContext->TpmDevice
           );
}

#define TCGPASSTHROUGH( cb, in, out ) \
    TcgCommonPassThrough(  \
        cb, \
        sizeof (in) / sizeof (*(in)), \
        (in), \
        sizeof (out) / sizeof (*(out)), \
        (out) \
        )

EFI_STATUS TcgCommonSha1Complete(
    IN VOID         *CallbackContext,
    IN VOID         *Data,
    IN UINT32       DataLen,
    OUT TCG_DIGEST  *Digest )
{
    TPM_1_2_CMD_SHA1COMPLETE       cmdSHA1Complete;
    TPM_1_2_RET_HEADER             retSHA1Complete;
    TPM_TRANSMIT_BUFFER            InBuffer[2], OutBuffer[2];

    if ( DataLen >= 64 )
    {
        return EFI_INVALID_PARAMETER;
    }

    cmdSHA1Complete.Header.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdSHA1Complete.Header.ParamSize = TPM_H2NL(sizeof(cmdSHA1Complete) + DataLen);
    cmdSHA1Complete.Header.Ordinal = TPM_H2NL( TPM_ORD_SHA1Complete );
    cmdSHA1Complete.NumBytes       = TPM_H2NL( DataLen );

    InBuffer[0].Buffer = &cmdSHA1Complete;
    InBuffer[0].Size   = sizeof (cmdSHA1Complete);
    InBuffer[1].Buffer = Data;
    InBuffer[1].Size   = DataLen;

    OutBuffer[0].Buffer = &retSHA1Complete;
    OutBuffer[0].Size   = sizeof (retSHA1Complete);
    OutBuffer[1].Buffer = Digest;
    OutBuffer[1].Size   = sizeof (*Digest);

    return TCGPASSTHROUGH( CallbackContext, InBuffer, OutBuffer );
}

EFI_STATUS
SHA1HashFunc (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN UINT8                *HashData,
    IN UINT32               HashDataLen,
    OUT TCG_DIGEST          *Digest
)
{
    EFI_STATUS                  Status;
    UINT32                      Sha1MaxBytes;
    TCG_PEI_CALLBACK_CONTEXT    Context;

    Status = __FillCallbackContext( PeiServices, &Context );
    ASSERT_EFI_ERROR ( Status );

    Status = Context.TpmDevice->Init( Context.TpmDevice, PeiServices );

    if ( EFI_ERROR( Status )) {
        goto Exit;
    }

    Status = TcgCommonSha1Start( &Context, TCG_ALG_SHA, &Sha1MaxBytes );

    if ( EFI_ERROR( Status )) {
        goto Exit;
    }

    Status = TcgCommonSha1Update(
                 &Context,
                 HashData,
                 HashDataLen & ~63,
                 Sha1MaxBytes
             );

    if ( EFI_ERROR( Status )) {
        goto Exit;
    }

    HashData    += (HashDataLen & ~63);
    HashDataLen &= 63;

    Status = TcgCommonSha1Complete(
                 &Context,
                 (UINT8 *)HashData,
                 (UINT32)HashDataLen,
                 Digest
             );

    if ( EFI_ERROR( Status )) {
        goto Exit;
    }

Exit:
    Context.TpmDevice->Close( Context.TpmDevice, PeiServices );
    return Status;
}


EFI_STATUS ACM_PostSuccess(
)
{
    UINT32  MsrValue;

    MsrValue = (UINT32) ReadMsr (MSR_BOOT_GUARD_SACM_INFO);
    if ( ((MsrValue & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == B_BOOT_GUARD_SACM_INFO_NEM_ENABLED)  &&
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) == B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT)  &&         
         (((MsrValue & V_TPM_PRESENT_DTPM_12) == V_TPM_PRESENT_DTPM_12) || ((MsrValue & V_TPM_PRESENT_DTPM_20) == V_TPM_PRESENT_DTPM_20)) &&
         ((MsrValue & B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) == B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) ) {
    	DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] : ACM_PostSuccess : EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    }

    DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] : ACM_PostSuccess : EFI_UNSUPPORTED\n"));
    return EFI_UNSUPPORTED;
}

UINT8
GetBpRstrLow (
  VOID
  )
{
  BP_RSTR_LOW BpRstr;
  UINT32      AcmStatus;
  UINT64      SacmInfo;

  AcmStatus = MmioRead32 (MMIO_ACM_STATUS);
  SacmInfo  = ReadMsr (MSR_BOOT_GUARD_SACM_INFO);

  BpRstr.Bits.Facb = (UINT8)((SacmInfo  & BIT4)  >> 4);
  BpRstr.Bits.Dcd  = (UINT8)((AcmStatus & BIT21) >> 21);
  BpRstr.Bits.Dbi  = (UINT8)((AcmStatus & BIT22) >> 22);
  BpRstr.Bits.Pbe  = (UINT8)((AcmStatus & BIT23) >> 23);
  BpRstr.Bits.Bbp  = (UINT8)((AcmStatus & BIT24) >> 24);
  BpRstr.Bits.Reserved = 0;
  BpRstr.Bits.BpInvd   = 0;

  return BpRstr.Data;
}

/**
  Get the lower 8 bits of Boot Policy Type

  @return The lower 8 bits of BP.TYPE
**/
UINT8
GetBpTypeLow (
  VOID
  )
{
  BP_TYPE_LOW BpType;
  UINT32      AcmStatus;
  UINT64      SacmInfo;

  AcmStatus = MmioRead32 (MMIO_ACM_STATUS);
  SacmInfo  = ReadMsr (MSR_BOOT_GUARD_SACM_INFO);

  BpType.Bits.MeasuredBoot = (UINT8)((SacmInfo & BIT5) >> 5);
  BpType.Bits.VerifiedBoot = (UINT8)((SacmInfo & BIT6) >> 6);
  BpType.Bits.Hap      = (UINT8)((AcmStatus & BIT20) >> 20);
  BpType.Bits.Reserved = 0;

  return BpType.Data;
}

EFI_STATUS LogDetailPCREvent(
    IN EFI_PEI_SERVICES **PeiServices
)
{
    UINT8               FoundACM = 0, FoundKM = 0, FoundBPM = 0;
    UINT32              u32HashLen = 0;
    UINT8               byteEventBuf[0x200];
    TCG_PCR_EVENT       *TcgEvent = (TCG_PCR_EVENT*)&byteEventBuf[0];
    UINT32              EventNum = 0, FitEntryPointer = 0, FitEntryNumber = 0, i = 0;
    UINTN               Len;
    CHAR8               DetailPCRStr[] = "Boot Guard Debug Measured S-CRTM";
    PEI_TPM_PPI               *TpmPpi = NULL;
    PEI_TCG_PPI               *TcgPpi = NULL;
    EFI_STATUS          Status = EFI_SUCCESS;
    BOOT_POLICY         *BP = NULL;
    FIT_ENTRY           *FitEntry = NULL;
    KEY_MANIFEST_STRAUCTURE   *KmStructure = NULL;
    BpmStruct                 *BpmStructure = NULL;

    ZeroMem(
        byteEventBuf,
        sizeof(byteEventBuf)
    );

    Len = sizeof(DetailPCRStr) - 1;

    TcgEvent->PCRIndex = 0;
    TcgEvent->EventType = EV_S_CRTM_CONTENTS;
    TcgEvent->EventSize = (UINT32)Len;

    DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] : Enter LogDetailPCREvent(...)\n"));

    MemCpy(
        TcgEvent->Event,
        DetailPCRStr,
        Len
    );

    Status = LocateTcgPpi(PeiServices,&TpmPpi, &TcgPpi);
    if(EFI_ERROR(Status))return EFI_NOT_FOUND;

    // On page 44.
    // The pHashData must be the format .
    // SHA-1 {
    //          1) One byte containing the lower 8 bit of the BP.RSTR
    //          2) One byte contain the lower 8 bits of BP.TYPE
    //          ....
    //          7)  Digest of Hashed IBB Segments(s)

    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof (BOOT_POLICY),&BP);
    if ( EFI_ERROR( Status ) ) {
        return EFI_OUT_OF_RESOURCES;
    }

    DEBUG((DEBUG_INFO, "MSR[0x%x]:[%08x]\n", MSR_BOOT_GUARD_SACM_INFO, ReadMsr (MSR_BOOT_GUARD_SACM_INFO) ));
    DEBUG((DEBUG_INFO, "ACM_STATUS:[%08x]\n", ACM_STATUS ));

    BP->RSTR = GetBpRstrLow ();
    DEBUG((DEBUG_INFO, "BP->RSTR:[%x]\n", BP->RSTR ));

    BP->TYPE = GetBpTypeLow ();
    DEBUG((DEBUG_INFO, "BP->TYPE:[%x]\n", BP->TYPE ));

    BP->ACM_SVN = BOOT_GUARD_ACMSVN;
    DEBUG((DEBUG_INFO, "BP->ACM_SVN = [%x]\n", BP->ACM_SVN ));
    {
        FitEntryPointer = MmioRead32(IBB_ENTRYPOINT_M);
        if ( FitEntryPointer == 0xFFFFFFFF ) {
        	DEBUG((DEBUG_ERROR, "[BootGuardTPMPei.c] : FitEntryPointer(%lx) is empty\n", FitEntryPointer));
        	ASSERT_EFI_ERROR ( EFI_NOT_READY );
        }

        FitEntry = (FIT_ENTRY*)FitEntryPointer;
        if ( FitEntry->TblAddress != EFI_SIGNATURE_64 ('_', 'F', 'I', 'T', '_', ' ', ' ', ' ') ) {
        	DEBUG((DEBUG_ERROR, "[BootGuardTPMPei.c] : [Type 0] FitEntry->TblAddress(%lx) is error\n", FitEntry->TblAddress));
        	ASSERT_EFI_ERROR ( EFI_NOT_READY );
        }

        FitEntryNumber = FitEntry->TblSIZE;

        FoundACM = 0;
        for(i=1; i<FitEntryNumber; i++) {
            FitEntry = (FIT_ENTRY*)(FitEntryPointer + i*16);
            DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] : FitEntry->TblType = %x\n", FitEntry->TblType));
            if ( FitEntry->TblType == 0x02 ) { // FIT type 0x02 - Anc ACM location
                FoundACM =1;
                break;
            }
        }

        if ( FoundACM == 0 ) {
        	DEBUG((DEBUG_ERROR, "[BootGuardTPMPei.c] : Can't find the Boot Guard ACM"));
        	ASSERT_EFI_ERROR ( EFI_NOT_READY );
        }

        DEBUG((DEBUG_INFO, "BP->ACM_Signature \n"));

        for ( i=0; i<256; i++ ) {
            if (i % 16 == 0) DEBUG((DEBUG_INFO, "\n"));
            BP->ACM_Signature[i] = MmioRead8( (UINTN)FitEntry->TblAddress + 0x184 + i );
            DEBUG((DEBUG_INFO, "%02x ", BP->ACM_Signature[i]));
        }

        DEBUG((DEBUG_INFO, "\n"));

        FoundKM = 0;
        for(i=1; i<FitEntryNumber; i++) {
            FitEntry = (FIT_ENTRY*)(FitEntryPointer + i*16);
            DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] : FitEntry->TblType = %x\n", FitEntry->TblType));
            if ( FitEntry->TblType == 0x0B ) { // FIT Type 0x0B - Key Manifest
                FoundKM =1;
                break;
            }
        }

        if ( FoundKM == 0 ) {
        	DEBUG((DEBUG_ERROR, "[BootGuardTPMPei.c] : Can't find the Boot Guard KM"));
        	ASSERT_EFI_ERROR ( EFI_NOT_READY );
        }

        KmStructure = (KEY_MANIFEST_STRAUCTURE*)FitEntry->TblAddress;
        DEBUG((DEBUG_INFO, "\nKmStructure:\n"));

        for ( i=0; i<256; i++ ) {
            if (i % 16 == 0) DEBUG((DEBUG_INFO, "\n"));
            BP->Key_Manifest_Signature[i] = KmStructure->KeyManifestSignature.Signature.Signature[i];
            DEBUG((DEBUG_INFO, "%02x ", BP->Key_Manifest_Signature[i]));
        }

        DEBUG((DEBUG_INFO, "\n"));

        FoundBPM = 0;
        for(i=1; i<FitEntryNumber; i++) {
            FitEntry = (FIT_ENTRY*)(FitEntryPointer + i*16);
            DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] : FitEntry->TblType = %x\n", FitEntry->TblType));
            if ( FitEntry->TblType == 0x0C ) { // FIT type 0x0C - Boot Policy Manifest
                FoundBPM =1;
                break;
            }
        }

        if ( FoundBPM == 0 ) {
        	DEBUG((DEBUG_ERROR, "[BootGuardTPMPei.c] : Can't find the Boot Guard BPM"));
        	ASSERT_EFI_ERROR ( EFI_NOT_READY );
        }

        BpmStructure = (BpmStruct*)FitEntry->TblAddress;
        DEBUG((DEBUG_INFO, "\nBpmStructure:\n"));

        for ( i=0; i<256; i++ ) {
            if (i % 16 == 0) DEBUG((DEBUG_INFO, "\n"));
            BP->Boot_Policy_Manifest_Signature[i] = BpmStructure->Bpm_Signature_Element.KeySignature.Signature.Signature[i];
            DEBUG((DEBUG_INFO, "%02x ", BP->Boot_Policy_Manifest_Signature[i]));
        }

        DEBUG((DEBUG_INFO, "\n\nBpmStructure->Digest_of_Hashed_IBB_Segment:\n\n"));

        for ( i=0; i<32; i++ ) {
            if (i % 16 == 0) DEBUG((DEBUG_INFO, "\n"));
            BP->Digest_of_Hashed_IBB_Segment[i] = BpmStructure->Ibb_Element.Digest.HashBuffer[i];
            DEBUG((DEBUG_INFO, "%02x ", BP->Digest_of_Hashed_IBB_Segment[i]));
        }

        DEBUG((DEBUG_INFO, "\n"));
    }

    {
        TCG_DIGEST                       Digest;

        u32HashLen = sizeof (BOOT_POLICY);
        SHA1HashFunc( PeiServices, (UINT8*)BP, u32HashLen, &Digest );
        CopyMem(&TcgEvent->Digest, &Digest, SHA1_DIGEST_SIZE);
    }
    Status = TcgPpi->TCGLogEvent(TcgPpi, PeiServices, (VOID*)TcgEvent, &EventNum);
    return Status;
}

EFI_STATUS
BootGuardMeasureCRTMVersion(
    IN EFI_PEI_SERVICES **PeiServices )
{
    EFI_STATUS Status;
    DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] Start of BootGuardMeasureCRTMVersion\n"));

    Status = ACM_PostSuccess();
    if( !EFI_ERROR(Status) )
    {
    	DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] Start the LogDetailPCREvent\n"));
        Status = LogDetailPCREvent( PeiServices );
        ASSERT_EFI_ERROR ( Status );
    }
    DEBUG((DEBUG_INFO, "[BootGuardTPMPei.c] End of BootGuardMeasureCRTMVersion\n"));
    return MeasureCRTMVersion( PeiServices );
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
