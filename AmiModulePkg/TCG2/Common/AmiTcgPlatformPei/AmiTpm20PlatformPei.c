//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgPlatform/AmiTcgPlatformPeiBeforeMem.c 1     10/08/13 12:04p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:04p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgPlatform/AmiTcgPlatformPeiBeforeMem.c $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    AmiTcgPlatformPeiBeforeMem.c
//
// Description: Function file for AmiTcgPlatformPeiBeforeMem
//
//<AMI_FHDR_END>
//*************************************************************************
#include <efi.h>
#include <AmiTcg\TcgCommon12.h>
#include "AmiTcg\Sha.h"
#include <AmiTcg\TcgMisc.h>
#include <AmiTcg\TpmLib.h>
#include <token.h>
#include "PPI\TcgService.h"
#include "PPI\TpmDevice.h"
#include <Library/DebugLib.h>
#include <Ppi\ReadOnlyVariable2.h>
#include <Ppi\AmiTcgPlatformPpi.h>
#include <AmiTcg\AmiTcgPlatformPei.h>
#include <AmiTcg\Tpm20Pei.h>
#include <Library/BaseMemoryLib.h>
#include "PPI\TcgPlatformSetupPeiPolicy.h"
#include <Library/AmiRomLayoutLib.h>
#include <Ppi\EndOfPeiPhase.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <ImageAuthentication.h>
#include <Library/IoLib.h>
#include <ImageAuthentication.h>
#include <Library/UefiLib.h>
#include <AmiTcg\AmiTpmStatusCodes.h>

// NIST 800-155
#include <Guid/HobList.h>
#include <AmiHobs.h>
#include <RomLayout.h>
// NIST 800-155

#define ___INTERNAL_CONVERT_TO_STRING___(a) #a
#define CONVERT_TO_STRING(a) ___INTERNAL_CONVERT_TO_STRING___(a)
#define ___INTERNAL_CONVERT_TO_WSTRING___(a) L###a
#define CONVERT_TO_WSTRING(a) ___INTERNAL_CONVERT_TO_WSTRING___(a)

#define TCG2_PROTOCOL_SPEC_TCG_1_2  0x01
#define TCG2_PROTOCOL_SPEC_TCG_2    0x02

BOOLEAN CrbSupported();
UINT8 GetCurrentInterfaceType();
EFI_STATUS FindAndMeasureDxeCore(PeiServices);

#pragma pack(push,1)

typedef struct
{
    TPM2_COMMAND_HEADER       Header;
    TPM_CAP                   Capability;
    UINT32                    Property;
    UINT32                    PropertyCount;
} TPM2_GET_CAPABILITY_COMMAND;

typedef struct
{
    TPM2_RESPONSE_HEADER      Header;
    TPMI_YES_NO               MoreData;
    TPMS_CAPABILITY_DATA      CapabilityData;
} TPM2_GET_CAPABILITY_RESPONSE;

#pragma pack(pop)

EFI_STATUS MeasureTCGPcClientSpecID(
    IN CONST EFI_PEI_SERVICES **PeiServices);

UINT8
EFIAPI FirstBootCheck(
    IN EFI_PEI_SERVICES **PeiServices);

EFI_STATUS
Tpm20MeasureCRTMVersion(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    AMI_TREE_PPI *TrEEPeiPpi);

EFI_STATUS 
EFIAPI
TpmPeiReportStatusCode(  IN EFI_STATUS_CODE_TYPE   Type,
                         IN EFI_STATUS_CODE_VALUE  Value);

#define AMI_GLOBAL_VARIABLE_GUID \
    {0x1368881,0xc4ad,0x4b1d,0xb6,0x31,0xd5,0x7a,0x8e,0xc8,0xdb,0x6b}


#if FTpmPlatformProfile == 1
#define AMD_FTPM_PPI_GUID \
            {0x91774185, 0xf72d, 0x467e, {0x93, 0x39, 0xe0, 0x8, 0xdb, 0xae, 0xe, 0x14}}
EFI_GUID giTpmPpiGuid    = AMD_FTPM_PPI_GUID;
EFI_STATUS iTpmGetInfo (
    IN OUT UINTN  *iTPMStatus
);
#endif

EFI_FFS_FILE_HEADER *gFfsHeader;

extern TPM20_MEASURE_CRTM_VERSION_PEI_FUNC_PTR  TPM20_MEASURE_CRTM_VERSION_PEI_FUNCTION;
TPM20_MEASURE_CRTM_VERSION_PEI_FUNC_PTR *Tpm20MeasureCRTMVersionFuncPtr = TPM20_MEASURE_CRTM_VERSION_PEI_FUNCTION;


#if defined (MDE_PKG_VERSION) && (MDE_PKG_VERSION > 9)
#ifndef TPM2_ALG_SHA1
#define TPM2_ALG_SHA1 TPM_ALG_SHA1
#endif
#ifndef TPM2_ALG_SHA256
#define TPM2_ALG_SHA256 TPM_ALG_SHA256
#endif
#ifndef TPM2_ALG_SHA384
#define TPM2_ALG_SHA384 TPM_ALG_SHA384
#endif
#ifndef TPM2_ALG_SHA512
#define TPM2_ALG_SHA512 TPM_ALG_SHA512
#endif
#ifndef TPM2_ALG_SM3_256
#define TPM2_ALG_SM3_256 TPM_ALG_SM3_256
#endif
#ifndef TPM2_ALG_ID
#define TPM2_ALG_ID TPM_ALG_ID
#endif
#endif

#define SHA1_DIGEST_SIZE        20
#define SHA256_DIGEST_SIZE      32
#define SM3_256_DIGEST_SIZE     32
#define SHA384_DIGEST_SIZE      48
#define SHA512_DIGEST_SIZE      64


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   InternalPeiBuildHobGuid
//
// Description: Internal abstracted function to create a Hob
//
// Input:       IN  EFI_PEI_SERVICES  **PeiServices,
//              IN  EFI_GUID          *Guid,
//              IN  UINTN             DataLength,
//              OUT VOID              **Hob
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS InternalPeiBuildHobGuid(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_GUID         *Guid,
    IN UINTN            DataLength,
    OUT VOID            **Hob )
{
    EFI_STATUS Status;

    Status = (*PeiServices)->CreateHob(
                 PeiServices,
                 EFI_HOB_TYPE_GUID_EXTENSION,
                 (UINT16) ( sizeof (EFI_HOB_GUID_TYPE) + DataLength ),
                 Hob
             );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    DEBUG((-1, "Hob created \n"));
    ((EFI_HOB_GUID_TYPE*)(*Hob))->Name = *Guid;

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI AmdPspAvailable(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *Ppi );



#if FTpmPlatformProfile == 1
EFI_PEI_NOTIFY_DESCRIPTOR   AmdMemDiscCallback =
{
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &giTpmPpiGuid,
    AmdPspAvailable
};
#endif


#define TREE_BOOT_HASH_ALG_SHA1   0x00000001
#define TREE_BOOT_HASH_ALG_SHA256 0x00000002
#define TREE_BOOT_HASH_ALG_SHA384 0x00000004
#define TREE_BOOT_HASH_ALG_SHA512 0x00000008
#define TREE_BOOT_HASH_ALG_SM3    0x00000010

UINT8      Sha1Digest[SHA1_DIGEST_SIZE];
TPM2_HALG  TpmDigest;

EFI_STATUS MeasureLogDxeFwVol(
    IN  CONST EFI_PEI_SERVICES  **PeiServices,
    IN  FwVolHob                *TpmFwVolHob,
    IN  AMI_TREE_PPI            *TrEEPeiPpi,
    IN  AMI_INTERNAL_HLXE_PPI   *HashLogExtendEx
)
{
    EFI_STATUS                        Status = EFI_SUCCESS;
    TCG_PCR_EVENT2_HDR                Tcg20Event;
    EFI_PLATFORM_FIRMWARE_BLOB        EventData;
    SHA1_CTX                          Sha1Ctx;
    SHA2_CTX                          Sha2Ctx;
    SHA384_CTX                        Sha384Ctx;
    SHA512_CTX                        Sha512Ctx;
    UINTN                             Count = 0;
    UINT32                            i = 0;
    UINT32                            FwVolHobCount;
    UINT8                             gTcg2SpecVersion = 0;  
    UINT32                            gPcrBanks = 0;

    DEBUG(( -1, "[%d] Enter MeasureLogDxeFwVol\n", __LINE__));

    FwVolHobCount = TpmFwVolHob->Count;
    gTcg2SpecVersion = TpmFwVolHob->Tcg2SpecVersion;
    gPcrBanks = TpmFwVolHob->PcrBanks;
    
    Tcg20Event.PCRIndex = PCRi_CRTM_AND_POST_BIOS;
    Tcg20Event.EventType = EV_POST_CODE;
    Tcg20Event.EventSize = sizeof(EventData);

    EventData.BlobBase = TpmFwVolHob[0].baseAddress;
    EventData.BlobLength = (UINT64)TpmFwVolHob[0].Size;

    if(gTcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2 )
    {
        SHA1Init( &Sha1Ctx );
    }
    else if(gTcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Init( &Sha1Ctx );
        }
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_init(&Sha2Ctx);
        }
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_init(&Sha384Ctx);
        }
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_init(&Sha512Ctx);
        }
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SM3)
        {
            sha256_init(&Sha2Ctx);
        }
    }
    
    DEBUG ((-1, "FwVolHobCount = %x \n", FwVolHobCount));
    for(i=0; i< FwVolHobCount; i++)
    {
        DEBUG ((-1, "TpmFwVolHob[i].Size = %x \n", TpmFwVolHob[i].Size));
        DEBUG ((-1, "TpmFwVolHob[i].baseAddress = %lx \n", TpmFwVolHob[i].baseAddress));
        if(gTcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update( &Sha1Ctx, (unsigned char *)TpmFwVolHob[i].baseAddress, (u32)TpmFwVolHob[i].Size );
        }
        else if(gTcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha2Ctx, (unsigned char *)TpmFwVolHob[i].baseAddress, (u32)TpmFwVolHob[i].Size );
            }
            if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update( &Sha1Ctx, (unsigned char *)TpmFwVolHob[i].baseAddress, (u32)TpmFwVolHob[i].Size );
            }
            if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process(&Sha384Ctx, (unsigned char *)TpmFwVolHob[i].baseAddress, (u32)TpmFwVolHob[i].Size);
            }
            if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process(&Sha512Ctx, (unsigned char *)TpmFwVolHob[i].baseAddress, (u32)TpmFwVolHob[i].Size);
            }
            if( gPcrBanks & TREE_BOOT_HASH_ALG_SM3)
            {
                //until sm3 is available in official OpenSSL, Use SM3[SHA-256[FwVol]]
                sha256_process( &Sha2Ctx, (unsigned char *)TpmFwVolHob[i].baseAddress, (u32)TpmFwVolHob[i].Size );
            }
        }
    }
    
    if(gTcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Final((unsigned char *)&Sha1Digest, &Sha1Ctx);
        //printbuffer(Sha1Digest,SHA1_DIGEST_SIZE );
        CopyMem(Tcg20Event.Digests.digests[0].digest.sha1, Sha1Digest, SHA1_DIGEST_SIZE);
        Tcg20Event.Digests.digests[0].hashAlg = TPM2_ALG_SHA1;
    }
    else if(gTcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Final((unsigned char *)&TpmDigest.sha1, &Sha1Ctx);
            CopyMem(&Tcg20Event.Digests.digests[Count].digest.sha1, &TpmDigest.sha1, SHA1_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SHA1;
            Count+=1;
        }
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_done(&Sha2Ctx, (unsigned char *)&TpmDigest.sha256);
            CopyMem(&Tcg20Event.Digests.digests[Count].digest.sha256, &TpmDigest.sha256, SHA256_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SHA256;
            Count+=1;
        }
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_done(&Sha384Ctx, (unsigned char *)&TpmDigest.sha384 );
            CopyMem(&Tcg20Event.Digests.digests[Count].digest.sha384,  &TpmDigest.sha384, SHA384_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SHA384;
            Count+=1;
        }
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_done(&Sha512Ctx, (unsigned char *)&TpmDigest.sha512);
            CopyMem(&Tcg20Event.Digests.digests[Count].digest.sha512,  &TpmDigest.sha512, SHA512_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SHA512;
            Count+=1;
        }
        if( gPcrBanks & TREE_BOOT_HASH_ALG_SM3)
        {
            sha256_done(&Sha2Ctx, (unsigned char *)&TpmDigest.sm3_256);
            CopyMem(&Tcg20Event.Digests.digests[Count].digest.sm3_256,  &TpmDigest.sm3_256, SM3_256_DIGEST_SIZE);
            Tcg20Event.Digests.digests[Count].hashAlg = TPM2_ALG_SM3_256;
            Count+=1;
        }
        
        Tcg20Event.Digests.count = (UINT32)Count;
    }
    
    Status = HashLogExtendEx->AmiHashLogExtendEx(PeiServices, TrEEPeiPpi, NULL, 0, 0, &Tcg20Event, (UINT8*)&EventData);
    
    DEBUG(( -1, "MeasureLogDxeFwVol - %r\n", Status));
    return Status;

}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureCRTMVersion
//
// Description: Measures EFI CRTM Version
//              Demo Version[546BFB1E1D0C4055A4AD4EF4BF17B83A]
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
Tpm20MeasureCRTMVersion(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    AMI_TREE_PPI *TrEEPeiPpi)
{
    AMI_TREE_EVENT          *TrEEEventData;
    UINT32              EventSize = 0;
    EFI_GUID            CrtmVersion = CRTM_GUID;
    EFI_STATUS          Status;
    UINT8               *EventDataPtr;
    UINTN               Len;

    EventSize  = sizeof(EFI_GUID);

    Len =  sizeof(AMI_TREE_EVENT_HEADER) +  sizeof(UINT32) + EventSize;

    Status = (*PeiServices)->AllocatePool(PeiServices, Len, &TrEEEventData);

    TrEEEventData->Size = Len;

    if(EFI_ERROR(Status))return Status;
    TrEEEventData->Header.EventType  = EV_S_CRTM_VERSION;
    TrEEEventData->Header.HeaderSize = sizeof(AMI_TREE_EVENT_HEADER);
    TrEEEventData->Header.HeaderVersion = 1;
    TrEEEventData->Header.PcrIndex      = PCRi_CRTM_AND_POST_BIOS;

    EventDataPtr = (UINT8 *)TrEEEventData;

    EventDataPtr += sizeof(AMI_TREE_EVENT_HEADER) + sizeof(UINT32);

    CopyMem(
        EventDataPtr,
        &CrtmVersion,
        EventSize
    );

    Status = TrEEPeiPpi->HashLogExtendEvent(TrEEPeiPpi,
                                          0, (EFI_PHYSICAL_ADDRESS)EventDataPtr, EventSize, TrEEEventData);

    TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_CRTM_VERSION_MEASURED | EFI_SOFTWARE_PEI_MODULE);
    
    return Status;
}


EFI_STATUS
EFIAPI
Tpm2PeiGetCapability (
    IN      AMI_TREE_PPI            *TrEEPeiPpi,
    IN      TPM_CAP                   Capability,
    IN      UINT32                    Property,
    IN      UINT32                    PropertyCount,
    OUT     TPMI_YES_NO               *MoreData,
    OUT     TPMS_CAPABILITY_DATA      *CapabilityData
)
{
    EFI_STATUS                        Status;
    TPM2_GET_CAPABILITY_COMMAND       SendBuffer;
    TPM2_GET_CAPABILITY_RESPONSE      RecvBuffer;
    UINT32                            SendBufferSize;
    UINT32                            RecvBufferSize;
 
    if(TrEEPeiPpi == NULL)return EFI_INVALID_PARAMETER;

    //
    // Construct command
    //
    SendBuffer.Header.tag = SwapBytes16(TPM_ST_NO_SESSIONS);
    SendBuffer.Header.commandCode = SwapBytes32(TPM_CC_GetCapability);

    SendBuffer.Capability = SwapBytes32 (Capability);
    SendBuffer.Property = SwapBytes32 (Property);
    SendBuffer.PropertyCount = SwapBytes32 (PropertyCount);

    SendBufferSize = (UINT32) sizeof (SendBuffer);
    SendBuffer.Header.paramSize = SwapBytes32 (SendBufferSize);

    //
    // send Tpm command
    //
    RecvBufferSize = sizeof (RecvBuffer);
    //Status = TreeSubmitCommand (pTreeProtocol, SendBufferSize, (UINT8 *)&SendBuffer, RecvBufferSize, (UINT8 *)&RecvBuffer );
    Status  = TrEEPeiPpi->SubmitCommand(TrEEPeiPpi, SendBufferSize, (UINT8 *)&SendBuffer, RecvBufferSize, (UINT8 *)&RecvBuffer );
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    
    // Check the device return Tag, if it is belong the TPM20 RSP_TAG
    if( RecvBuffer.Header.tag != (TPMI_ST_COMMAND_TAG)TPM_H2NS(TPM_ST_NO_SESSIONS) )
    {
        return EFI_INCOMPATIBLE_VERSION;
    }
    

    if (RecvBufferSize <= sizeof (TPM2_RESPONSE_HEADER) + sizeof (UINT8))
    {
        return EFI_DEVICE_ERROR;
    }

    //
    if (SwapBytes32(RecvBuffer.Header.responseCode) != TPM_RC_SUCCESS)
    {
        DEBUG((-1, "Tpm2GetCapability: Response Code error! 0x%08x\r\n", SwapBytes32(RecvBuffer.Header.responseCode)));
        return EFI_DEVICE_ERROR;
    }
    // Return the response
    //
    *MoreData = RecvBuffer.MoreData;
    //
    // Does not unpack all possiable property here, the caller should unpack it and note the byte order.
    //
    CopyMem (CapabilityData, &RecvBuffer.CapabilityData, RecvBufferSize - sizeof (TPM2_RESPONSE_HEADER) - sizeof (UINT8));

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm2GetPeiCapabilityFwVersion (
    IN      AMI_TREE_PPI            *TrEEPeiPpi,
    OUT     UINT32                  *TpmFwVersion
)
{
    TPMS_CAPABILITY_DATA    TpmCap;
    TPMI_YES_NO             MoreData;
    EFI_STATUS              Status;
    UINT32                  VersionH=0;
    UINT32                  VersionL=0;

    if(TpmFwVersion == NULL) return EFI_INVALID_PARAMETER;
    
    *TpmFwVersion = 0;
    
    Status = Tpm2PeiGetCapability (
                 TrEEPeiPpi,
                 TPM_CAP_TPM_PROPERTIES,
                 TPM_PT_FIRMWARE_VERSION_1,
                 1,
                 &MoreData,
                 &TpmCap
             );
    
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    
    *TpmFwVersion = SwapBytes32 (TpmCap.data.tpmProperties.tpmProperty->value);
    
    DEBUG((-1, "\n Firmware = %x \n", *TpmFwVersion));
    
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
Tpm2PeiGetCapabilityManufactureID (
    IN      AMI_TREE_PPI            *TrEEPeiPpi,
    OUT     UINT32                    *ManufactureId
)
{
    TPMS_CAPABILITY_DATA    TpmCap;
    TPMI_YES_NO             MoreData;
    EFI_STATUS              Status;

    Status = Tpm2PeiGetCapability (
                 TrEEPeiPpi,
                 TPM_CAP_TPM_PROPERTIES,
                 TPM_PT_MANUFACTURER,
                 1,
                 &MoreData,
                 &TpmCap
             );
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    *ManufactureId = SwapBytes32 (TpmCap.data.tpmProperties.tpmProperty->value);
    
    DEBUG((-1, "\n ManufactureId = %x \n", *ManufactureId));

    return EFI_SUCCESS;
}





//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   Tpm20PeiSelfTest
//
// Description: Send TPM 20 SelfTest Command
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS Tpm20PeiSelfTest(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    AMI_TREE_PPI *TrEEPeiPpi)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    TPM2_SelfTest           SelfTestCmd;
    TPM2_Common_RespHdr     SelfTestReponse;
    UINT32                  ReturnSize = 0;
    UINT32                  TpmFwVersion;
    UINT32                  TpmManufacturer;
    
    if(TrEEPeiPpi == NULL) return EFI_INVALID_PARAMETER;
    
    SelfTestCmd.tag = (TPMI_ST_COMMAND_TAG)TPM_H2NS(TPM_ST_NO_SESSIONS);
    SelfTestCmd.CommandSize = TPM_H2NL((sizeof(TPM2_SelfTest)));
    SelfTestCmd.CommandCode = TPM_H2NL(TPM_CC_SelfTest);
    
    Status = Tpm2GetPeiCapabilityFwVersion(TrEEPeiPpi, &TpmFwVersion);
    if(Status == EFI_INCOMPATIBLE_VERSION){
        return EFI_DEVICE_ERROR;
    }
    
    Status = Tpm2PeiGetCapabilityManufactureID(TrEEPeiPpi, &TpmManufacturer);
        
    if(!EFI_ERROR(Status) && TpmManufacturer == 0x49465800){
        if(TpmFwVersion == 0x07003C || TpmFwVersion == 0x050006){ //workaround for Infineon FW 5.6 and 7.60 failure
            SelfTestCmd.SelfTestType = 1;
        }else{
            SelfTestCmd.SelfTestType = 0;
        }
    }
    
    ReturnSize = (UINT32)sizeof(SelfTestReponse);

    SetMem((UINT8 *)&SelfTestReponse,(UINTN)sizeof(SelfTestReponse), 0);

    DEBUG((-1, "\nsending TPM20 SelfTest To Tpm \n"));
    Status = TrEEPeiPpi->SubmitCommand(TrEEPeiPpi,
                                       sizeof(SelfTestCmd),
                                       (UINT8*)&SelfTestCmd,
                                       ReturnSize,
                                       (UINT8*)&SelfTestReponse);

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    // Check the device return Tag, if it is belong the TPM20 RSP_TAG
    if( SelfTestReponse.tag != (TPMI_ST_COMMAND_TAG)TPM_H2NS(TPM_ST_NO_SESSIONS) )
    {
        Status = EFI_DEVICE_ERROR;
    }
    else if( SelfTestReponse.ResponseCode != TPM_RC_SUCCESS)
    {
        Status = EFI_NOT_READY;
    }

    DEBUG((-1, "SelfTestReponse.Tag = %x \n", TPM_H2NS(SelfTestReponse.tag)));
    DEBUG((-1, "SelfTestReponse.ResponseCode = %x \n", TPM_H2NL(SelfTestReponse.ResponseCode)));
    DEBUG((-1, "SelfTestReponse.Status = %r \n", Status));


    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   Tpm20PeiSendStartup
//
// Description: Send TPM 20 Startup Command
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS Tpm20PeiSendStartup(IN CONST EFI_PEI_SERVICES **PeiServices,
                               AMI_TREE_PPI *TrEEPeiPpi, EFI_BOOT_MODE BootMode)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    EFI_STATUS          Status2;
    TPM2_Startup_Cmd    StartupCmd;
    TPM2_Common_RespHdr StartupReponse;
    UINT32              ReturnSize = 0;
    AMI_TCG_PEI_FUNCTION_OVERRIDE_PPI       *PpiOverride;
    EFI_GUID            SkipTpmStartupGuid = AMI_SKIP_TPM_STARTUP_GUID;
    BOOLEAN             SkipTpmStartup = FALSE;
    EFI_HOB_GUID_TYPE       *Hob;
    Tpm20DeviceHob          *TrEEDeviceHob;
    EFI_GUID gTpm20Supporthobguid = TPM20_HOB_GUID;
    TCG_CONFIGURATION       ConfigFlags;
    TCG_PLATFORM_SETUP_INTERFACE *TcgPeiPolicy;
    EFI_GUID                        gTcgPeiPolicyGuid =\
            TCG_PLATFORM_SETUP_PEI_POLICY_GUID;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &SkipTpmStartupGuid,
                 0, NULL,
                 &PpiOverride);

    if(!EFI_ERROR(Status))
    {
        SkipTpmStartup = TRUE;
    }

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gTcgPeiPolicyGuid,
                 0, NULL,
                 &TcgPeiPolicy);

    if(EFI_ERROR(Status) || TcgPeiPolicy == NULL )return Status;

    Status = TcgPeiPolicy->getTcgPeiPolicy((EFI_PEI_SERVICES **)PeiServices, &ConfigFlags);

    if(EFI_ERROR(Status))return Status;

    if(TrEEPeiPpi == NULL)return EFI_INVALID_PARAMETER;

    StartupCmd.tag = (TPMI_ST_COMMAND_TAG)TPM_H2NS(TPM_ST_NO_SESSIONS);
    StartupCmd.CommandSize = TPM_H2NL((sizeof(TPM2_Startup_Cmd)));
    StartupCmd.CommandCode = TPM_H2NL(TPM_CC_Startup);



    if(BootMode == BOOT_ON_S3_RESUME)
    {
        StartupCmd.StartupType = TPM_H2NS(TPM_SU_STATE);
    }
    else
    {
        StartupCmd.StartupType = TPM_H2NS(TPM_SU_CLEAR);
    }

    ReturnSize = (UINT32)sizeof(StartupReponse);

    SetMem((UINT8 *)&StartupReponse,(UINTN)sizeof(StartupReponse), 0);
    if( !SkipTpmStartup )
    {
        DEBUG((-1, "sending TPM20 b4 TCGPassThroughToTpm \n"));
        Status = TrEEPeiPpi->SubmitCommand(TrEEPeiPpi,
                                           sizeof(TPM2_Startup_Cmd),
                                           (UINT8*)&StartupCmd,
                                           ReturnSize,
                                           (UINT8*)&StartupReponse);

        if(EFI_ERROR(Status))return Status;

        if((StartupReponse.ResponseCode) != TPM_RC_SUCCESS)
        {
            DEBUG((-1, "StartupReponse.ResponseCode = %x \n", TPM_H2NL(StartupReponse.ResponseCode)));
            DEBUG((-1, "StartupReponse.Status = %r \n", Status));
            TpmPeiReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_2_0_STARTUP_ERROR | EFI_SOFTWARE_PEI_MODULE);   
            Status = EFI_DEVICE_ERROR;
        }else{
            TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_2_0_STARTUP_CMD_SENT | EFI_SOFTWARE_PEI_MODULE);
        }
        
    }
    
    if(BootMode == BOOT_ON_S3_RESUME)
    {
#if defined (SetReadyStateOnExitBootServices) && (SetReadyStateOnExitBootServices == 1)
        // When customer have enable the TOKEN,
        // We still need to sync on S3 resume

        // For Intel PTT Sighting, we avoid the TPM_BASE addr.
        if( 0xFFFFFFFF != *(UINT32*)(0xFED40000 + 0xC) )
        {
            // The requirement is suitable for Intel PTT or D-TPM
            EFI_STATUS dTPMCrbSetReqReadyState( VOID* );
            BOOLEAN isTpm20CrbPresent();
            if( TRUE == isTpm20CrbPresent() )
            {
                dTPMCrbSetReqReadyState( (VOID*)0xFED40000 );
            }
        }
#endif
        return Status;
    }
    // Since the BootGuard will skip startup command, Check here if the TPM 2.0 is present
    if( !EFI_ERROR(Status) )
    {
        Status = Tpm20PeiSelfTest( PeiServices, TrEEPeiPpi );
        if( Status == EFI_NOT_READY )
        {
            Status = EFI_SUCCESS;
        }
        if(!EFI_ERROR(Status)){
            TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_2_0_SELFTEST_CMD_SENT | EFI_SOFTWARE_PEI_MODULE);
        }
        
    }

    Status2 = InternalPeiBuildHobGuid((EFI_PEI_SERVICES **)PeiServices, &gTpm20Supporthobguid,
                                     (sizeof(Tpm20DeviceHob)),  &Hob);
    ASSERT_EFI_ERROR( Status2 );
    
    if(Status != EFI_DEVICE_ERROR && !EFI_ERROR(Status))
    {
        TrEEDeviceHob = (Tpm20DeviceHob*)(Hob + 1);
        TrEEDeviceHob->Tpm20DeviceState = 1;
        if(!GetCurrentInterfaceType())
        {
            TrEEDeviceHob->InterfaceType = 1;
        }
        else
        {
            TrEEDeviceHob->InterfaceType = 0;
        }
    }
    else
    {
        TrEEDeviceHob = (Tpm20DeviceHob*)(Hob + 1);
        TrEEDeviceHob->Tpm20DeviceState = 0;
        TrEEDeviceHob->InterfaceType = ConfigFlags.InterfaceSel;
    }
    

    TrEEDeviceHob->Reserved = FirstBootCheck((EFI_PEI_SERVICES **)PeiServices);

    if( TrEEDeviceHob->Tpm20DeviceState )
    {
        DEBUG((-1, "StartupReponse.Tag = %x \n", StartupReponse.tag));
        DEBUG((-1, "StartupReponse.Size = %x \n", StartupReponse.ResponsSize));
        DEBUG((-1, "StartupReponse.ResponseCode = %x \n", StartupReponse.ResponseCode));
    }
    
    return Status;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   FirstBootCheck
//
// Description: Check for first boot
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT8
EFIAPI FirstBootCheck(
    IN EFI_PEI_SERVICES **PeiServices)
{

    EFI_STATUS  Status;
    CHAR16      Monotonic[] = L"MonotonicCounter";
#if defined(CORE_COMBINED_VERSION) && (CORE_COMBINED_VERSION > 262797)
    EFI_GUID    Guid        = AMI_GLOBAL_VARIABLE_GUID;
#else
    EFI_GUID    Guid        = EFI_GLOBAL_VARIABLE;
#endif
    UINTN                          Size        = sizeof(UINT32);
    UINT32                         Counter;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gEfiPeiReadOnlyVariable2PpiGuid,
                 0, NULL,
                 &ReadOnlyVariable
             );

    if(EFI_ERROR(Status) || ReadOnlyVariable == NULL )return Status;

    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, Monotonic, &Guid,
                                           NULL, &Size, &Counter );


    if ( EFI_ERROR( Status ))
    {
        DEBUG((-1, "TPM20::First boot Scenario determined \n"));
        return 01;
    }

    DEBUG((-1, "TPM20::Not First boot Scenario\n"));
    return 0xFF;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   AmdPspAvailable
//
// Description: Call back for PSP fTPM
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI AmdPspAvailable(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *Ppi )
{

    EFI_STATUS Status;
    AMI_TREE_PPI *TrEEPeiPpi = NULL;
    TCG_PLATFORM_SETUP_INTERFACE    *TcgPeiPolicy;
    TCG_CONFIGURATION               ConfigFlags;
    EFI_GUID                        gTcgPeiPolicyGuid =\
            TCG_PLATFORM_SETUP_PEI_POLICY_GUID;
#if defined(TcgNistSp800_155_SUPPORT) && (TcgNistSp800_155_SUPPORT == 1)
    BOOLEAN                         measureSpecIdEvent = TRUE;
#else
    BOOLEAN                         measureSpecIdEvent = FALSE;
#endif
    EFI_BOOT_MODE       BootMode;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gAmiTreePpiGuid,
                 0, NULL,
                 &TrEEPeiPpi);

    if(EFI_ERROR(Status) || TrEEPeiPpi == NULL )return Status;

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if(EFI_ERROR(Status))return Status;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gTcgPeiPolicyGuid,
                 0, NULL,
                 &TcgPeiPolicy);

    if(TcgPeiPolicy == NULL)
    {
        return EFI_NOT_FOUND;
    }

    Status = TcgPeiPolicy->getTcgPeiPolicy((EFI_PEI_SERVICES **)PeiServices, &ConfigFlags);
    if(EFI_ERROR(Status))return Status;


    //send TPM 2.0 Startup
    Status = Tpm20PeiSendStartup(PeiServices, TrEEPeiPpi, BootMode);
    if( EFI_ERROR(Status) )
    {
        return EFI_SUCCESS;
    }

    if(BootMode == BOOT_ON_S3_RESUME)
    {
        return EFI_SUCCESS;
    }

    if(measureSpecIdEvent == TRUE || ConfigFlags.Tcg2SpecVersion ==  TCG2_PROTOCOL_SPEC_TCG_2)
    {
        Status =  MeasureTCGPcClientSpecID(PeiServices);
    }
    Status = Tpm20MeasureCRTMVersionFuncPtr( PeiServices, TrEEPeiPpi);
    Status = FindAndMeasureDxeCore(PeiServices);
    return Status;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   printbuffer
//
// Description: Printbuffer for debug purposes
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
void printbuffer(UINT8 *Buffer, UINTN BufferSize)
{
    UINTN i=0;
    UINTN j=0;

    DEBUG((-1,"\n**********PrintBuffer Entry********"));
    for(i=0; i<BufferSize; i++)
    {

        if(i%16 == 0)
        {
            DEBUG((-1,"\n"));
            DEBUG((-1,"%04x :", j));
            j+=1;
        }
        DEBUG((-1,"%02x ", Buffer[i]));
    }
    DEBUG((-1,"\n"));
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureNISTManifest
//
// Description: Measures NIST spec
//
//
// Input:       IN  PEI_TCG_PPI* tcg,
//              IN  EFI_PEI_SERVICES      **ps
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
#if defined(TcgNistSp800_155_SUPPORT) && (TcgNistSp800_155_SUPPORT == 1)
EFI_STATUS MeasureNISTManifest(
    IN CONST EFI_PEI_SERVICES **PeiServices)
{
    AMI_TREE_EVENT                    *TrEEEventData;
    EFI_STATUS                    Status;
    EFI_GUID                      ManifestGuid = ReferenceManifestGuid_TcgLog;
    AMI_TREE_PPI                    *TrEEPeiPpi = NULL;
    UINTN                           Size=sizeof(EFI_GUID) + sizeof(UINT32) + sizeof(AMI_TREE_EVENT_HEADER) + sizeof(UINT32);
    UINT8                           *EventDataPtr;

    DEBUG((-1,  "TCG Pei: MeasureNISTManifest\n"));
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gAmiTreePpiGuid,
                 0, NULL,
                 &TrEEPeiPpi);

    if(TrEEPeiPpi == NULL)
    {
        return EFI_NOT_FOUND;
    }

    Status = (*PeiServices)->AllocatePool(PeiServices, Size, &TrEEEventData);
    if(EFI_ERROR(Status))return Status;

    TrEEEventData->Header.EventType = EV_NO_ACTION;
    TrEEEventData->Header.HeaderSize =  sizeof(AMI_TREE_EVENT_HEADER);
    TrEEEventData->Header.HeaderVersion = 1;
    TrEEEventData->Header.PcrIndex = PCRi_CRTM_AND_POST_BIOS;
    TrEEEventData->Size = Size;

    EventDataPtr = (UINT8 *)TrEEEventData;
    EventDataPtr += sizeof(AMI_TREE_EVENT_HEADER) + sizeof(UINT32);

    *(UINT32 *)EventDataPtr = TcgOemVenderID;
    EventDataPtr += sizeof(UINT32);

    CopyMem(EventDataPtr,
            &ManifestGuid,
            sizeof(EFI_GUID));

    Status = TrEEPeiPpi->HashLogExtendEvent(TrEEPeiPpi,
                                            0,
                                            (EFI_PHYSICAL_ADDRESS)EventDataPtr,  //not used
                                            0,
                                            TrEEEventData);

    return Status;
}
#endif

// ->>>>>>>>>>>>>>>> NIST 800-155


#define STRUCT_FIELD_OFFSET( type, field )  \
    ((UINTN)&(((type*)0)->field))
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureTCGPcClientSpecID
//
// Description: Includes a measurement of the TcgSpecID into PCR[0]
//
//
// Input:       IN  PEI_TCG_PPI* tcg,
//              IN  EFI_PEI_SERVICES      **ps
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS MeasureTCGPcClientSpecID(
    IN CONST EFI_PEI_SERVICES **PeiServices)
{
    AMI_TREE_EVENT                    *TrEEEventData;
    EFI_STATUS                    Status;
    CHAR8                         Signature[] = "Spec ID Event03";
    UINT32                        NumOfAlg=0;
    AMI_TREE_PPI                    *TrEEPeiPpi = NULL;
    UINTN                           Size=sizeof(TCG_PCClientSpecIDEventStructEx) + sizeof(AMI_TREE_EVENT_HEADER) + sizeof(UINT32);
    UINT8                           *EventDataPtr;
    TCG_PLATFORM_SETUP_INTERFACE    *TcgPeiPolicy;
    TCG_CONFIGURATION               ConfigFlags;
    EFI_GUID                        gTcgPeiPolicyGuid =\
            TCG_PLATFORM_SETUP_PEI_POLICY_GUID;
    UINT32                          PcrBanks;


    DEBUG((-1,  "TCG Pei: MeasureTCGPcClientSpecID\n"));
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gAmiTreePpiGuid,
                 0, NULL,
                 &TrEEPeiPpi);

    if(TrEEPeiPpi == NULL)
    {
        return EFI_NOT_FOUND;
    }

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gTcgPeiPolicyGuid,
                 0, NULL,
                 &TcgPeiPolicy);

    if(TcgPeiPolicy == NULL)
    {
        return EFI_NOT_FOUND;
    }

    Status = TcgPeiPolicy->getTcgPeiPolicy((EFI_PEI_SERVICES **)PeiServices, &ConfigFlags);
    if(EFI_ERROR(Status))return Status;

    PcrBanks = ConfigFlags.PcrBanks;   

    DEBUG((-1, "TCG Pei: TCG_PcClientSpecID\n"));

    Status = (*PeiServices)->AllocatePool(PeiServices,Size, &TrEEEventData);
    if(EFI_ERROR(Status))return Status;

    EventDataPtr = (UINT8 *)TrEEEventData;
    SetMem(EventDataPtr, Size, 0);

    TrEEEventData->Header.PcrIndex = PCRi_CRTM_AND_POST_BIOS;
    TrEEEventData->Header.EventType = EV_NO_ACTION;
    TrEEEventData->Header.HeaderSize = sizeof(AMI_TREE_EVENT_HEADER);
    TrEEEventData->Header.HeaderVersion = 1;


    EventDataPtr +=  sizeof(AMI_TREE_EVENT_HEADER) + sizeof(UINT32);

    DEBUG((-1, "TrEEEventData->Size = %x \n", TrEEEventData->Size ));
    DEBUG((-1, "TPML_DIGEST_VALUES Size = %x \n",sizeof(TPML_DIGEST_VALUES)));

    CopyMem(
        EventDataPtr,
        Signature,
        AsciiStrLen(Signature));

    ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->PlatformClass = TCG_PLATFORM_CLASS;
    ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->SpecVersionMinor = 0x00;
    ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->SpecVersionMajor = 0x02;
    ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->SpecErrata = 0x00;
#if defined(x64_TCG) && (x64_TCG == 1)
    ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->uintnSize = 2;
#else
    ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->uintnSize = 1;
#endif

    if(ConfigFlags.Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1 )
        {
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].algortihgmId = TPM2_ALG_SHA1;
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].digestSize = SHA1_DIGEST_SIZE;
            ++NumOfAlg;
        }
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256 )
        {
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].algortihgmId = TPM2_ALG_SHA256;
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].digestSize = SHA256_DIGEST_SIZE;
            ++NumOfAlg;
        }
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384 )
        {
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].algortihgmId = TPM2_ALG_SHA384;
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].digestSize = SHA384_DIGEST_SIZE;
            ++NumOfAlg;
        }
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
        {
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].algortihgmId = TPM2_ALG_SHA512;
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].digestSize = SHA512_DIGEST_SIZE;
            ++NumOfAlg;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3 )
        {
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].algortihgmId = TPM2_ALG_SM3_256;
            ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[NumOfAlg].digestSize = SM3_256_DIGEST_SIZE;
            ++NumOfAlg;
        }
        ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->numberOfAlgorithms = NumOfAlg;
    }
    else
    {
        ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[0].algortihgmId = TPM2_ALG_SHA1;
        ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->digestSizes[0].digestSize = SHA1_DIGEST_SIZE;
        ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->numberOfAlgorithms = 1;
    }

    Size=STRUCT_FIELD_OFFSET(TCG_PCClientSpecIDEventStructEx, digestSizes)
         + ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->numberOfAlgorithms * sizeof(TCG_EFISpecIdEventAlgorithmSize) + sizeof(UINT8)
         + sizeof(AMI_TREE_EVENT_HEADER) + sizeof(UINT32);

    TrEEEventData->Size = Size;


    Size = STRUCT_FIELD_OFFSET(TCG_PCClientSpecIDEventStructEx, digestSizes);
    Size += ((TCG_PCClientSpecIDEventStructEx *)EventDataPtr)->numberOfAlgorithms * sizeof(TCG_EFISpecIdEventAlgorithmSize);
    *(UINT8*)(EventDataPtr + Size) = 0;

    EventDataPtr +=  sizeof(TCG_PCClientSpecIDEventStructEx);

    Status = TrEEPeiPpi->HashLogExtendEvent(TrEEPeiPpi,
                                            0,
                                            (EFI_PHYSICAL_ADDRESS)EventDataPtr,  //not used
                                            0,
                                            TrEEEventData);

    return Status;
}


TCG_PEI_MEMORY_CALLBACK    *FvMemCallback;



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureFVOnMemAvail
//
// Description: On Memory available callback for DxeCore measurement
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI MeasureFVOnMemAvail(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *Ppi )
{
    EFI_STATUS Status=EFI_SUCCESS;
    Status = FindAndMeasureDxeCore(PeiServices);
    return Status;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureTeImage
//
// Description: Measure Te Images (if DxeCore is a TE image)
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
MeasureTeImage (
    IN  CONST EFI_PEI_SERVICES **PeiServices,
    IN  EFI_IMAGE_LOAD_EVENT     *ImageLoad,
    IN  UINTN                     Tcg2SpecVersion,
    IN  UINT32                    PcrBanks,
    IN  TPM2_HALG                *Tpm2Digest)
{
    EFI_TE_IMAGE_HEADER               *ptrToTEHdr;
    EFI_IMAGE_SECTION_HEADER          *Section;
    UINT8                             *HashBase;
    UINT64                            HashSize;
    UINT64                             SumOfBytesHashed;
    UINTN                             SectionHeaderOffset;
    UINTN                             numOfSectionHeaders;
    UINTN                             Index;
    AMI_PEI_HASH_INTERFACE_PPI        *HashInterface;
    AMI_TREE_PPI                      *TrEEPeiPpi = NULL;
    EFI_GUID                          Intfceguid = AMI_PEI_HASH_INTERFACE_PROTOCOL_GUID;
    SHA1_CTX                          Sha1Ctx;
    SHA2_CTX                          Sha2Ctx;
    SHA384_CTX                        Sha384Ctx;
    SHA512_CTX                        Sha512Ctx;
    VOID                              *HashInterfaceContext;
    UINTN                             HashInterfaceContextSize = 0x100;
    UINT8                             HashSxBuffer[0x100];
    TPM2B_DIGEST                      Result;
    EFI_STATUS                        Status;

    HashInterfaceContext = &HashSxBuffer[0];

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Init(&Sha1Ctx);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
            SHA1Init(&Sha1Ctx);
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
            sha256_init( &Sha2Ctx );
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
            sha384_init( &Sha384Ctx );
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
            sha512_init(&Sha512Ctx);
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {

            //locate the PeiTree Protocol
            Status = (*PeiServices)->LocatePpi(PeiServices,
                                               &Intfceguid,
                                               0, NULL,
                                               &HashInterface);

            if(EFI_ERROR(Status) || HashInterface == NULL) return Status;

            //locate the PeiTree Protocol
            Status = (*PeiServices)->LocatePpi(
                         PeiServices,
                         &gAmiTreePpiGuid,
                         0, NULL,
                         &TrEEPeiPpi);

            if(EFI_ERROR(Status) || TrEEPeiPpi == NULL) return Status;

            HashInterface->Init(TrEEPeiPpi, AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext, &HashInterfaceContextSize);
        }
    }

    DEBUG ((-1, "Printing TE Image Buffer in Memory Image Location = %x, Image size = %x\n",
            (UINTN)ImageLoad->ImageLocationInMemory, ImageLoad->ImageLengthInMemory));

    //printbuffer((UINT8 *)(UINTN)ImageLoad->ImageLocationInMemory, 1024);

    ptrToTEHdr = (EFI_TE_IMAGE_HEADER *)((UINT8 *)(UINTN)ImageLoad->ImageLocationInMemory);
    //Hash TE Image header and section headers

    HashBase = (UINT8 *)(UINTN)ptrToTEHdr;
    HashSize = sizeof(EFI_TE_IMAGE_HEADER) + ( EFI_IMAGE_SIZEOF_SECTION_HEADER * ptrToTEHdr->NumberOfSections);

    DEBUG ((-1, "Printing Hashed TE Hdr and Section Header\n"));
    DEBUG ((-1, "Base = %x Len = %x \n", HashBase, HashSize));

    //printbuffer(HashBase, HashSize);

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);
    }


    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
        {
            sha512_process((SHA384_CTX *)HashInterfaceContext, HashBase, (unsigned long)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_process((SHA512_CTX *)HashInterfaceContext, HashBase, (unsigned long)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize, AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
        }
    }


    SectionHeaderOffset = sizeof(EFI_TE_IMAGE_HEADER) + (UINTN)ptrToTEHdr;
    numOfSectionHeaders = ptrToTEHdr->NumberOfSections;

    SumOfBytesHashed = HashSize;

    //check for alignment
    //hash Alignment buffer
    HashSize = (((EFI_IMAGE_SECTION_HEADER *)((UINT8 *)SectionHeaderOffset))->PointerToRawData
                - ptrToTEHdr->StrippedSize + sizeof(EFI_TE_IMAGE_HEADER)+ (UINT64)ImageLoad->ImageLocationInMemory - ((UINT64)ImageLoad->ImageLocationInMemory + HashSize));

    if(HashSize !=0)
    {
        HashBase += sizeof(EFI_TE_IMAGE_HEADER) + ( EFI_IMAGE_SIZEOF_SECTION_HEADER * ptrToTEHdr->NumberOfSections);

        DEBUG ((-1, "Printing Hashed TE Alignment Buffer\n"));
        DEBUG ((-1, "Base = %x Len = %x \n", HashBase, HashSize));
//        printbuffer(HashBase, HashSize);

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process((SHA384_CTX *)HashInterfaceContext, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process((SHA512_CTX *)HashInterfaceContext, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
            {

                HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize, AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
            }

            SumOfBytesHashed+=HashSize;
        }
    }


    //
    // TE Images do not have a CertDirector
    //

    //process hash section by section
    //hash in order sections. We do not sort TE sections
    for (Index = 0; Index < numOfSectionHeaders; Index++)
    {

        Section = (EFI_IMAGE_SECTION_HEADER *)((UINT8 *)SectionHeaderOffset);

        if (Section->SizeOfRawData == 0)
        {
            continue;
        }

        //Hash raw data

        HashBase = (UINT8 *)(((UINTN)ImageLoad->ImageLocationInMemory)
                             + (UINTN)Section->PointerToRawData - ptrToTEHdr->StrippedSize + sizeof(EFI_TE_IMAGE_HEADER));

        HashSize = (UINTN) Section->SizeOfRawData;

        DEBUG ((-1, "Section Base = %x Section Len = %x \n", HashBase, HashSize));
        //printbuffer(HashBase, 1024);

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process((SHA384_CTX *)HashInterfaceContext, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process((SHA512_CTX *)HashInterfaceContext, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
            {
                Status = HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize,\
                                                                           AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
            }
        }

        SumOfBytesHashed += HashSize;
        SectionHeaderOffset += EFI_IMAGE_SIZEOF_SECTION_HEADER;

    }

    DEBUG ((-1, "SumOfBytesHashed = %x \n", SumOfBytesHashed));

    //verify size
    if ( ImageLoad->ImageLengthInMemory > SumOfBytesHashed)
    {

        DEBUG ((-1, "Hash rest of Data if true \n"));

        HashBase = (UINT8 *)(UINTN)ImageLoad->ImageLocationInMemory + SumOfBytesHashed;
        HashSize = (UINTN)(ImageLoad->ImageLengthInMemory - SumOfBytesHashed);

        DEBUG ((-1, "Base = %x Len = %x \n", HashBase, HashSize));
        //printbuffer(HashBase, 106);

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha2Ctx, HashBase, (u32)HashSize );
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process((SHA384_CTX *)HashInterfaceContext, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process((SHA512_CTX *)HashInterfaceContext, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
            {
                Status = HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize,\
                                                                           AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
            }
        }

    }

    //
    // Finalize the SHA hash.
    //

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Final(Tpm2Digest->sha1, &Sha1Ctx);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Final(Tpm2Digest->sha1, &Sha1Ctx);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_done( &Sha2Ctx, Tpm2Digest->sha256 );
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_done(&Sha384Ctx, (unsigned char *)Tpm2Digest->sha384);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_done(&Sha512Ctx, (unsigned char *)Tpm2Digest->sha512);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            UINTN   TempVal = 64;

            HashInterface->GetHashResults(TrEEPeiPpi,
                                          HashInterfaceContext,AMI_TREE_BOOT_HASH_ALG_SM3,
                                          (UINT8 *)&Result.buffer,  &TempVal);
            Result.size = (UINT16)TempVal;

            if (EFI_ERROR(Status))
            {
                return EFI_DEVICE_ERROR;
            }

            (*PeiServices)->CopyMem(Tpm2Digest->sm3_256 , Result.buffer, Result.size);
        }

    }
    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureDxeCorePEImage
//
// Description: Measure Dxe Core as a PE Image
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI
MeasureDxeCorePEImage (
    IN  CONST EFI_PEI_SERVICES **PeiServices,
    IN  EFI_PHYSICAL_ADDRESS      ImageAddress,
    IN  UINTN                     ImageSize,
    IN  AMI_TREE_PPI             *TrEEPeiPpi,
    IN  AMI_INTERNAL_HLXE_PPI    *HashLogExtendEx,
    IN  TCG_CONFIGURATION        *ConfigFlags
)
{
    EFI_STATUS                        Status;
    TCG_PCR_EVENT2_HDR                TcgEvent;
    UINT8                             *EventData = NULL;
    EFI_IMAGE_LOAD_EVENT              *ImageLoad;
    EFI_DEVICE_PATH_PROTOCOL          *FullPath;
    UINT32                            FullPathSize;
    EFI_IMAGE_DOS_HEADER              *DosHdr;
    UINT32                            PeCoffHeaderOffset;
    EFI_IMAGE_NT_HEADERS64            *Hdr;
    EFI_IMAGE_SECTION_HEADER          *Section;
    UINT8                             *HashBase;
    UINTN                             HashSize;
    UINTN                             SumOfBytesHashed;
    EFI_IMAGE_SECTION_HEADER          *SectionHeader;
    UINTN                             Index, iPos;
    AMI_INTERNAL_HLXE_PPI             *InternalHLXE = NULL;
    EFI_GUID                          gEfiAmiHLXEGuid =  AMI_PROTOCOL_INTERNAL_HLXE_GUID;
    AMI_PEI_HASH_INTERFACE_PPI        *HashInterface;
    EFI_GUID                          Intfceguid = AMI_PEI_HASH_INTERFACE_PROTOCOL_GUID;
    VOID                              *HashInterfaceContext;
    UINTN                             HashInterfaceContextSize = 0x100;
    UINT8                             HashSxBuffer[0x100];
    UINT32                            Tcg2SpecVersion;
    UINT32                            PcrBanks;
    TPM2B_DIGEST                      *Result;
    TPM2_HALG                         TpmDigest;
    SHA1_CTX                          Sha1Ctx;
    SHA2_CTX                          Sha256Ctxt;
    SHA384_CTX                        Sha384Ctxt;
    SHA512_CTX                        Sha512Ctxt;
    TPM2_HALG                         *Tpm2Digest;
    UINT8                             *Buffer;
    TPM2_ALG_ID                       Algo;
    UINTN                             i=0;

    Tcg2SpecVersion = ConfigFlags->Tcg2SpecVersion;
    PcrBanks = ConfigFlags->PcrBanks;

    HashInterfaceContext = &HashSxBuffer[0];

    ImageLoad     = NULL;
    FullPath      = NULL;
    SectionHeader = NULL;
    FullPathSize  = 0;

    DEBUG ((-1, "PeiMeasurePeImage Entry\n"));

    //Allocate Event log memory
    Status = (*PeiServices)->AllocatePool(PeiServices, ((sizeof (*ImageLoad)
                                          - sizeof (ImageLoad->DevicePath)) + FullPathSize), &EventData);

    if(EFI_ERROR(Status))return Status;
    //
    // Determine destination PCR by BootPolicy
    //
    TcgEvent.EventSize  = sizeof (*ImageLoad) - sizeof (ImageLoad->DevicePath);
    TcgEvent.EventSize += FullPathSize;

    Status = (*PeiServices)->AllocatePool(PeiServices,TcgEvent.EventSize, &ImageLoad);

    if (ImageLoad == NULL)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
    }

    ImageLoad->ImageLocationInMemory = ImageAddress;
    ImageLoad->ImageLengthInMemory   = ImageSize;
    ImageLoad->ImageLinkTimeAddress  = 0;
    ImageLoad->LengthOfDevicePath    = 0;

    DEBUG ((-1, "ImageLoad->ImageLocationInMemory = %lx,  ImageLoad->ImageLengthInMemory = %lx \n",
            ImageLoad->ImageLocationInMemory, ImageLoad->ImageLengthInMemory));

    //
    // Check PE/COFF image
    //
    DosHdr = (EFI_IMAGE_DOS_HEADER *)(UINTN)ImageAddress;
    PeCoffHeaderOffset = 0;
    if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE)
    {
        PeCoffHeaderOffset = DosHdr->e_lfanew;
    }

    if (((EFI_TE_IMAGE_HEADER *)((UINT8 *)(UINTN)ImageAddress + PeCoffHeaderOffset))->Signature
            == EFI_TE_IMAGE_HEADER_SIGNATURE)
    {

        //Measure TE Image
        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            MeasureTeImage(PeiServices, ImageLoad, Tcg2SpecVersion, PcrBanks, &TpmDigest);
            (*PeiServices)->CopyMem(&TcgEvent.Digests.digests[0].digest.sha1, TpmDigest.sha1, SHA1_DIGEST_SIZE);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            MeasureTeImage(PeiServices, ImageLoad, Tcg2SpecVersion, PcrBanks, &TpmDigest);
            (*PeiServices)->CopyMem(&TcgEvent.Digests.digests, &TpmDigest, sizeof(TPM2_HALG));
        }

        goto TEImageDone;
    }

    DEBUG ((-1, "Image is a PE image\n"));

    //
    // PE/COFF Image Measurement
    //
    //    NOTE: The following codes/steps are based upon the authenticode image hashing in
    //      PE/COFF Specification 8.0 Appendix A.
    //
    //

    // 1. Load the image header into memory.

    // 2. Initialize a SHA hash context.
    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Init(&Sha1Ctx);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
            SHA1Init(&Sha1Ctx);
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
            sha256_init( &Sha256Ctxt );
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
            sha384_init( &Sha384Ctxt );
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
            sha512_init(&Sha512Ctxt);
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {

            //locate the PeiTree Protocol
            Status = (*PeiServices)->LocatePpi(PeiServices,
                                               &Intfceguid,
                                               0, NULL,
                                               &HashInterface);

            if(EFI_ERROR(Status) || HashInterface == NULL) return Status;

            //locate the PeiTree Protocol
            Status = (*PeiServices)->LocatePpi(
                         PeiServices,
                         &gAmiTreePpiGuid,
                         0, NULL,
                         &TrEEPeiPpi);

            if(EFI_ERROR(Status) || TrEEPeiPpi == NULL) return Status;

            HashInterface->Init(TrEEPeiPpi, AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext, &HashInterfaceContextSize);
        }
    }


    //
    // Measuring PE/COFF Image Header;
    // But CheckSum field and SECURITY data directory (certificate) are excluded
    //
    Hdr   = (EFI_IMAGE_NT_HEADERS64 *)((UINT8 *)(UINTN)ImageAddress + PeCoffHeaderOffset);

    //
    // 3. Calculate the distance from the base of the image header to the image checksum address.
    // 4. Hash the image header from its base to beginning of the image checksum.
    //
    HashBase = (UINT8 *)(UINTN)ImageAddress;
    HashSize = (UINTN) ((UINT8 *)(&Hdr->OptionalHeader.CheckSum) - HashBase);
    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_process( &Sha256Ctxt, HashBase, (u32)HashSize );
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
        {
            sha512_process(&Sha384Ctxt, HashBase, (unsigned long)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_process(&Sha512Ctxt, HashBase, (unsigned long)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize, AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
        }
    }

    DEBUG ((-1, "CheckPoint 1\n"));
    //
    // 5. Skip over the image checksum (it occupies a single ULONG).
    // 6. Get the address of the beginning of the Cert Directory.
    // 7. Hash everything from the end of the checksum to the start of the Cert Directory.
    //

    HashBase = (UINT8 *) &Hdr->OptionalHeader.CheckSum + sizeof (UINT32);
    HashSize = (UINTN) ((UINT8 *)(&Hdr->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_process( &Sha256Ctxt, HashBase, (u32)HashSize );
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
        {
            sha512_process(&Sha384Ctxt, HashBase, (unsigned long)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_process(&Sha512Ctxt, HashBase, (unsigned long)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            Status = HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize,\
                                                                   AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
        }
    }
    //
    // 8. Skip over the Cert Directory. (It is sizeof(IMAGE_DATA_DIRECTORY) bytes.)
    // 9. Hash everything from the end of the Cert Directory to the end of image header.
    //
    HashBase = (UINT8 *) &Hdr->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
    HashSize = Hdr->OptionalHeader.SizeOfHeaders -
               (UINTN) ((UINT8 *)(&Hdr->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - (UINT8 *)(UINTN)ImageAddress);

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Update(&Sha1Ctx,
                   HashBase,
                   (u32)HashSize);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_process( &Sha256Ctxt, HashBase, (u32)HashSize );
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
        {
            sha512_process(&Sha384Ctxt, HashBase, (unsigned long)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_process(&Sha512Ctxt, HashBase, (unsigned long)HashSize);
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            Status = HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize,\
                                                                   AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
        }
    }
    //
    // 10. Set the SUM_OF_BYTES_HASHED to the size of the header
    //
    SumOfBytesHashed = Hdr->OptionalHeader.SizeOfHeaders;

    //
    // 11. Build a temporary table of pointers to all the IMAGE_SECTION_HEADER
    //     structures in the image. The 'NumberOfSections' field of the image
    //     header indicates how big the table should be. Do not include any
    //     IMAGE_SECTION_HEADERs in the table whose 'SizeOfRawData' field is zero.
    //
    (*PeiServices)->AllocatePool(PeiServices,sizeof (EFI_IMAGE_SECTION_HEADER) * Hdr->FileHeader.NumberOfSections, &SectionHeader);

    if(SectionHeader==NULL)return EFI_OUT_OF_RESOURCES;
    (*PeiServices)->SetMem(SectionHeader, (sizeof (EFI_IMAGE_SECTION_HEADER) * Hdr->FileHeader.NumberOfSections), 0);

    //
    // 12.    Using the 'PointerToRawData' in the referenced section headers as
    //      a key, arrange the elements in the table in ascending order. In other
    //      words, sort the section headers according to the disk-file offset of
    //      the section.
    //
    Section = (EFI_IMAGE_SECTION_HEADER *) (
                  (UINT8 *)(UINTN)ImageAddress +
                  PeCoffHeaderOffset +
                  sizeof(UINT32) +
                  sizeof(EFI_IMAGE_FILE_HEADER) +
                  Hdr->FileHeader.SizeOfOptionalHeader
              );

    for (Index = 0; Index < Hdr->FileHeader.NumberOfSections; Index++)
    {
        iPos = Index;
        while ((iPos > 0) && (Section->PointerToRawData < SectionHeader[iPos - 1].PointerToRawData))
        {
            (*PeiServices)->CopyMem (&SectionHeader[iPos], &SectionHeader[iPos - 1], sizeof(EFI_IMAGE_SECTION_HEADER));
            iPos--;
        }

        (*PeiServices)->CopyMem( &SectionHeader[iPos], Section,
                                 sizeof(EFI_IMAGE_SECTION_HEADER));
        Section += 1;
    }

    //
    // 13.    Walk through the sorted table, bring the corresponding section
    //      into memory, and hash the entire section (using the 'SizeOfRawData'
    //      field in the section header to determine the amount of data to hash).
    // 14.    Add the section's 'SizeOfRawData' to SUM_OF_BYTES_HASHED .
    // 15.    Repeat steps 13 and 14 for all the sections in the sorted table.
    //
    for (Index = 0; Index < Hdr->FileHeader.NumberOfSections; Index++)
    {
        Section  = (EFI_IMAGE_SECTION_HEADER *) &SectionHeader[Index];
        if (Section->SizeOfRawData == 0)
        {
            continue;
        }
        HashBase = (UINT8 *)(UINTN)ImageAddress + Section->PointerToRawData;
        HashSize = (UINTN) Section->SizeOfRawData;

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha256Ctxt, HashBase, (u32)HashSize );
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process(&Sha384Ctxt, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process(&Sha512Ctxt, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
            {
                Status = HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize,\
                                               AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
            }
        }


        SumOfBytesHashed += HashSize;
    }

    //
    // 16.    If the file size is greater than SUM_OF_BYTES_HASHED, there is extra
    //      data in the file that needs to be added to the hash. This data begins
    //      at file offset SUM_OF_BYTES_HASHED and its length is:
    //             FileSize  -  (CertDirectory->Size)
    //
    if (ImageSize > SumOfBytesHashed)
    {
        HashBase = (UINT8 *)(UINTN)ImageAddress + SumOfBytesHashed;
        HashSize = (UINTN)(ImageSize -
                           Hdr->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size -
                           SumOfBytesHashed);

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
        {
            SHA1Update(&Sha1Ctx,
                       HashBase,
                       (u32)HashSize);
        }

        if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
        {
            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
            {
                SHA1Update(&Sha1Ctx,
                           HashBase,
                           (u32)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
            {
                sha256_process( &Sha256Ctxt, HashBase, (u32)HashSize );
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
            {
                sha512_process(&Sha384Ctxt, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
            {
                sha512_process(&Sha512Ctxt, HashBase, (unsigned long)HashSize);
            }

            if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
            {
                Status = HashInterface->Update(TrEEPeiPpi, HashBase, (UINTN)HashSize,\
                                                                           AMI_TREE_BOOT_HASH_ALG_SM3, HashInterfaceContext);
            }
        }
    }

    //
    // 17.    Finalize the SHA hash.
    //
    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_1_2)
    {
        SHA1Final(TpmDigest.sha1, &Sha1Ctx);
        (*PeiServices)->CopyMem(&TcgEvent.Digests.digests[0].digest.sha1, TpmDigest.sha1, SHA1_DIGEST_SIZE);
    }

    if(Tcg2SpecVersion == TCG2_PROTOCOL_SPEC_TCG_2)
    {
        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
        {
            SHA1Final(Tpm2Digest->sha1, &Sha1Ctx);
            i+=1;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
        {
            sha256_done( &Sha256Ctxt, Tpm2Digest->sha256 );
            i+=1;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
        {
            sha384_done(&Sha384Ctxt, (unsigned char *)Tpm2Digest->sha384);
            i+=1;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
        {
            sha512_done(&Sha512Ctxt, (unsigned char *)Tpm2Digest->sha512);
            i+=1;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            UINTN   TempVal = 64;

            HashInterface->GetHashResults(TrEEPeiPpi,
                                          HashInterfaceContext,AMI_TREE_BOOT_HASH_ALG_SM3,
                                          (UINT8 *)&Result->buffer,  &TempVal );
            Result->size = (UINT16)TempVal;

            if (EFI_ERROR(Status))
            {
                return EFI_DEVICE_ERROR;
            }

            i+=1;

            (*PeiServices)->CopyMem(Tpm2Digest->sm3_256 , Result->buffer, Result->size);
        }

        TcgEvent.Digests.count = i;

        Buffer = (UINT8 *)&TcgEvent.Digests.digests[0];

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA1)
        {
            Algo = TPM2_ALG_SHA1;
            (*PeiServices)->CopyMem( Buffer , &Algo , sizeof(UINT16));
            Buffer+=sizeof(UINT16);
            (*PeiServices)->CopyMem( Buffer , Tpm2Digest->sha1, SHA1_DIGEST_SIZE);
            Buffer+=SHA1_DIGEST_SIZE;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA256)
        {
            Algo = TPM2_ALG_SHA256;
            (*PeiServices)->CopyMem( Buffer , &Algo , sizeof(UINT16));
            Buffer+=sizeof(UINT16);
            (*PeiServices)->CopyMem( Buffer , Tpm2Digest->sha256, SHA256_DIGEST_SIZE);
            Buffer+=SHA256_DIGEST_SIZE;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA384)
        {
            Algo = TPM2_ALG_SHA384;
            (*PeiServices)->CopyMem( Buffer , &Algo , sizeof(UINT16));
            Buffer+=sizeof(UINT16);
            (*PeiServices)->CopyMem( Buffer , Tpm2Digest->sha384, SHA384_DIGEST_SIZE);
            Buffer+=SHA384_DIGEST_SIZE;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SHA512)
        {
            Algo = TPM2_ALG_SHA512;
            (*PeiServices)->CopyMem( Buffer , &Algo , sizeof(UINT16));
            Buffer+=sizeof(UINT16);
            (*PeiServices)->CopyMem( Buffer , Tpm2Digest->sha512, SHA512_DIGEST_SIZE);
            Buffer+=SHA512_DIGEST_SIZE;
        }

        if( PcrBanks & AMI_TREE_BOOT_HASH_ALG_SM3)
        {
            Algo = TPM2_ALG_SM3_256;
            (*PeiServices)->CopyMem( Buffer , &Algo , sizeof(UINT16));
            Buffer+=sizeof(UINT16);
            (*PeiServices)->CopyMem( Buffer , Tpm2Digest->sm3_256, SM3_256_DIGEST_SIZE);
            Buffer+=SM3_256_DIGEST_SIZE;
        }

    }

TEImageDone:

    //
    // HashLogExtendEvent
    //
    TcgEvent.PCRIndex = PCRi_CRTM_AND_POST_BIOS;
    TcgEvent.EventType = EV_POST_CODE;
    (*PeiServices)->CopyMem(EventData, ImageLoad, TcgEvent.EventSize);

    HashLogExtendEx->AmiHashLogExtendEx(PeiServices, TrEEPeiPpi, NULL, 0, 0, &TcgEvent, EventData);
Done:

    DEBUG ((-1, "Pei Measure PE Image Done\n"));
    return Status;
}

#if defined (TCGRomLayout_SUPPORT) && (TCGRomLayout_SUPPORT!=0)
// <AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Name: GetRomLayout
//
// Description:
//
// Input:
//  OUT UINT8 **pRomLayout
//  OUT UINTN *Size
//
// Output:      EFI_STATUS
//
//--------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS GetRomArea(
    IN OUT AMI_ROM_AREA **pRomLayout,
    IN OUT UINTN *Size,
    IN  AMI_ROM_AREA *First
)
{
    if(pRomLayout == NULL)return EFI_INVALID_PARAMETER;
    if(First == NULL)
    {
        *pRomLayout = AmiGetFirstRomArea();
        if(*pRomLayout == NULL)return EFI_NOT_FOUND;
    }
    else
    {
        *pRomLayout = AmiGetNextRomArea(First);
        if(*pRomLayout == NULL)return EFI_NOT_FOUND;
    }

    *Size = (*pRomLayout)->Size;
    return EFI_SUCCESS;
}
#endif

#ifndef ROM_AREA_TCG_MEASURED
// Platform Firmware Volume that either contains or
// measures the EFI Boot Services and EFI Run Time Services; (code measured into TPM PCR[0])
#define ROM_AREA_TCG_MEASURED     0x000008000
#endif


//*******************************************************************************
//<AMI_PHDR_START>
//
// Procedure:   FindAndMeasureDxeCore
//
// Description:
//
// Input:
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//******************************************************************************
EFI_STATUS FindAndMeasureDxeCore(IN CONST EFI_PEI_SERVICES **PeiServices)
{

    EFI_STATUS              Status;
    AMI_TREE_PPI                    *TrEEPeiPpi = NULL;
    AMI_INTERNAL_HLXE_PPI           *HashLogExtendEx;
    EFI_GUID                        gAmiHashLogExtendEx = AMI_HASH_LOG_EXTEX_GUID;
    TCG_PLATFORM_SETUP_INTERFACE    *TcgPeiPolicy;
    TCG_CONFIGURATION               ConfigFlags;
    EFI_GUID                        gTcgPeiPolicyGuid =\
            TCG_PLATFORM_SETUP_PEI_POLICY_GUID;

#if defined (TCGRomLayout_SUPPORT) && (TCGRomLayout_SUPPORT!=0)
    AMI_ROM_AREA                    *RomArea = NULL;
    UINTN                           RomArea_size;
#endif
    FwVolHob                        *TpmFwVolHob;
    FwVolHob                        FwVolHobArray[10];
    UINTN                           i=0;
    UINTN                            Count=0;
    EFI_HOB_GUID_TYPE               *VolHob;
    EFI_GUID                        amiFVhoblistguid = AMI_FV_HOB_LIST_GUID;

    
    
    //locate the PeiTree Protocol
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gAmiTreePpiGuid,
                 0, NULL,
                 &TrEEPeiPpi);

    if(TrEEPeiPpi == NULL)
    {
        return EFI_NOT_FOUND;
    }

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gAmiHashLogExtendEx,
                 0, NULL,
                 &HashLogExtendEx);

    if(HashLogExtendEx == NULL)
    {
        return EFI_NOT_FOUND;
    }

    //create RomAreaOHob for Measurement of FV in Dxemain
#if defined (TCGRomLayout_SUPPORT) && (TCGRomLayout_SUPPORT!=0)
    Status = GetRomArea(&RomArea,&RomArea_size, NULL);
    if( (!EFI_ERROR(Status))&&(RomArea != NULL) )
    {
        for(i=0; i<10; i++)
        {
            if(RomArea == NULL)break;
            
            if((RomArea->Attributes & ROM_AREA_FV_SIGNED) && (RomArea->Attributes & ROM_AREA_TCG_MEASURED) ||
                    ((RomArea->Attributes & (ROM_AREA_FV_PEI+ROM_AREA_FV_DXE)) &&
                     !(RomArea->Attributes & ROM_AREA_VITAL) ) )  // Attributes)
            {
                
                FwVolHobArray[Count].baseAddress = RomArea->Address;
                FwVolHobArray[Count].Size        = RomArea->Size;
            DEBUG ((-1, "RomArea->Address = %x \n", RomArea->Address));
            DEBUG ((-1, "RomArea->Size = %x \n",RomArea->Size));
                Count +=1;
            }
            
            GetRomArea(&RomArea,&RomArea_size, RomArea);
        }
    }
    else
    {
        FwVolHobArray[Count].baseAddress = FV_MAIN_BASE;
        FwVolHobArray[Count].Size = FV_MAIN_SIZE;
        Count+=1;
    }
#else
    FwVolHobArray[Count].baseAddress = FV_MAIN_BASE;
    FwVolHobArray[Count].Size = FV_MAIN_SIZE;
    Count+=1;
#endif

    Status = InternalPeiBuildHobGuid((EFI_PEI_SERVICES **)PeiServices, &amiFVhoblistguid,
                                     (sizeof(FwVolHob)*Count),  &VolHob);

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gTcgPeiPolicyGuid,
                 0, NULL,
                 &TcgPeiPolicy);

    if(TcgPeiPolicy == NULL)
    {
        return EFI_NOT_FOUND;
    }

    Status = TcgPeiPolicy->getTcgPeiPolicy((EFI_PEI_SERVICES **)PeiServices, &ConfigFlags);
    if(EFI_ERROR(Status))return Status;

    TpmFwVolHob = (FwVolHob*)(VolHob + 1);
    for(i=0; i<Count; i++)
    {

        if(i==0)
        {
            TpmFwVolHob->Size = FwVolHobArray[i].Size;
            TpmFwVolHob->baseAddress = FwVolHobArray[i].baseAddress;
            TpmFwVolHob->Count = Count;
            TpmFwVolHob->Tcg2SpecVersion = (UINTN)ConfigFlags.Tcg2SpecVersion;
            TpmFwVolHob->PcrBanks = ConfigFlags.PcrBanks;
        }
        else
        {
            TpmFwVolHob->Size = FwVolHobArray[i].Size;
            TpmFwVolHob->baseAddress = FwVolHobArray[i].baseAddress;
            TpmFwVolHob->Count = 0;
            TpmFwVolHob->Tcg2SpecVersion = 0;
            TpmFwVolHob->PcrBanks = 0;
        }

        DEBUG ((-1, "TpmFwVolHob->Size = %x \n", TpmFwVolHob->Size));
        DEBUG ((-1, "TpmFwVolHob->baseAddress = %x \n",TpmFwVolHob->baseAddress));
        DEBUG ((-1, "TpmFwVolHob->Tcg2SpecVersion = %x \n",TpmFwVolHob->Tcg2SpecVersion));
        DEBUG ((-1, "TpmFwVolHob address = %x \n", TpmFwVolHob));
        
        TpmFwVolHob+=1;
    }
    
    //For meet NIST golden measurement verification
#if defined(SAVE_ENTIRE_FV_IN_MEM) && (SAVE_ENTIRE_FV_IN_MEM == 0)
    MeasureLogDxeFwVol(
            PeiServices,
            (TpmFwVolHob-Count),
            TrEEPeiPpi,
            HashLogExtendEx
            );
#endif

    return Status;
}






//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   AmiTcgPlatformPEI_EntryBeforeMem
//
// Description: Installs AMIplatform PPI for initialization in PEI before
//              memory is installed
//
// Input:        IN EFI_FFS_FILE_HEADER *FfsHeader,
//               IN EFI_PEI_SERVICES    **PeiServices
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI AmiTpm20PlatformPeiEntry(
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN CONST EFI_PEI_SERVICES    **PeiServices
)
{
    EFI_STATUS Status;
    AMI_TREE_PPI *TrEEPeiPpi = NULL;
#if FTpmPlatformProfile == 1
    UINTN  Info;
#endif
    TCG_PLATFORM_SETUP_INTERFACE    *TcgPeiPolicy;
    TCG_CONFIGURATION               ConfigFlags;
    EFI_GUID                        gTcgPeiPolicyGuid =\
            TCG_PLATFORM_SETUP_PEI_POLICY_GUID;
#if defined(TcgNistSp800_155_SUPPORT) && (TcgNistSp800_155_SUPPORT == 1)
    BOOLEAN                         measureSpecIdEvent = TRUE;
#else
    BOOLEAN                         measureSpecIdEvent = FALSE;
#endif
    EFI_BOOT_MODE       BootMode;
    gFfsHeader = FfsHeader;


#if FTpmPlatformProfile == 1
    Status = EFI_SUCCESS;
    if(!EFI_ERROR(iTpmGetInfo(&Info)))
    {
        Status = (**PeiServices).NotifyPpi (PeiServices, &AmdMemDiscCallback);
        return EFI_SUCCESS;
    }

#endif

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gAmiTreePpiGuid,
                 0, NULL,
                 &TrEEPeiPpi);

    if(EFI_ERROR(Status) || TrEEPeiPpi == NULL )return Status;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gTcgPeiPolicyGuid,
                 0, NULL,
                 &TcgPeiPolicy);

    if(TcgPeiPolicy == NULL)
    {
        return EFI_NOT_FOUND;
    }

    Status = TcgPeiPolicy->getTcgPeiPolicy((EFI_PEI_SERVICES **)PeiServices, &ConfigFlags);
    if(EFI_ERROR(Status))return Status;

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if(EFI_ERROR(Status))return Status;

    //send TPM 2.0 Startup
    Status = Tpm20PeiSendStartup(PeiServices, TrEEPeiPpi, BootMode);
    if( EFI_ERROR(Status) )
    {
        return EFI_SUCCESS;
    }

    if(BootMode == BOOT_ON_S3_RESUME)
    {
        return EFI_SUCCESS;
    }

    if(measureSpecIdEvent == TRUE || ConfigFlags.Tcg2SpecVersion ==  TCG2_PROTOCOL_SPEC_TCG_2)
    {
        Status =  MeasureTCGPcClientSpecID(PeiServices);
    }
#if defined(TcgNistSp800_155_SUPPORT) && (TcgNistSp800_155_SUPPORT == 1)
    Status =  MeasureNISTManifest(PeiServices);
#endif
    Status = Tpm20MeasureCRTMVersionFuncPtr( PeiServices, TrEEPeiPpi);

    if ( !EFI_ERROR( Status ))
    {
#if defined(SAVE_ENTIRE_FV_IN_MEM) && SAVE_ENTIRE_FV_IN_MEM == 0
        Status = MeasureFVOnMemAvail( (EFI_PEI_SERVICES**)PeiServices, NULL, NULL );
#else
    Status = FindAndMeasureDxeCore(PeiServices);
#endif
    }
    return Status;

}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
