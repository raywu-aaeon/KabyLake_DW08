//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  AmiSecureBootLib.c: helper library with hooks for Secure Boot configuration services
**/
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/AmiDigitalSignature.h>
#include <Library/DxeServicesLib.h>
#include <Library/DebugLib.h>

#include <Library/MemoryAllocationLib.h>

#include <Setup.h>
#include "TimeStamp.h"
#include <SecureBoot.h>
#include <AmiSecureBootLib.h>
#include <AmiSecureBootLibHooks.h>

//============================================================================

EFI_GUID *SecureVariableFileGuid [] = {
    &gAmiDbxFileGuid,
    &gAmiDbtFileGuid,
    &gAmiDbrFileGuid,
    &gAmiDbFileGuid,
    &gAmiKekFileGuid,
    &gAmiPkFileGuid,
    NULL
};

CHAR16* SecureVariableFileName[] = {
    EFI_IMAGE_SECURITY_DATABASE1,
    EFI_IMAGE_SECURITY_DATABASE2,
    EFI_IMAGE_SECURITY_DATABASE3,
    EFI_IMAGE_SECURITY_DATABASE,
    EFI_KEY_EXCHANGE_KEY_NAME,
    EFI_PLATFORM_KEY_NAME,
    NULL
};

CHAR16* SecureVariableFileNameDefault[] = {
    EFI_DBX_DEFAULT_VARIABLE_NAME,
    EFI_DBT_DEFAULT_VARIABLE_NAME,
    EFI_DBR_DEFAULT_VARIABLE_NAME,
    EFI_DB_DEFAULT_VARIABLE_NAME, 
    EFI_KEK_DEFAULT_VARIABLE_NAME,
    EFI_PK_DEFAULT_VARIABLE_NAME,
    NULL
};

// Build time array of certificates to be removed form the signature list.
SDL_X509_SIG_ENTRY SecureBootLibBadCertificateList[] = {
    SECUREBOOT_LIB_BAD_CERT_LIST {NULL, {0,0,0,0,0,0,0,0,0,0,0}}
};

static AMI_DIGITAL_SIGNATURE_PROTOCOL    *mAmiSig = NULL;

// ----------------------------------------------------------------------------
// Auxiliary functions for Secure Boot Key Management
// ----------------------------------------------------------------------------

/**
  Set/Delete Secure Boot policy Variables: PK, KEK, db, dbx, ...

  @param[in]  Action - Supported Types:
                SET_SECURE_VARIABLE_DEL 1
                SET_SECURE_VARIABLE_SET 2
                SET_SECURE_VARIABLE_APPEND 8
                SET_SECURE_VARIABLE_SET_RT 4

  @param[in]  SecureVariableName - EFI Variable name
  @param[in]  SecureVariableNameDefault - EFI Variable name for Default version
  @param[in]  SecureVariableGuid - Efi Variable GUID
  @param[in]  pVarBuffer - Variable data buffer
  @param[in]  VarSize - Variable data size
  @param[in]  OPTIONAL - indicator that the parameter is optional

  @retval     Status
**/
EFI_STATUS
EFIAPI
AmiSetSecureBootVariable(
    IN UINT16   Action,
    IN CHAR16   *SecureVariableName,
    IN CHAR16   *SecureVariableNameDefault OPTIONAL,
    IN EFI_GUID *SecureVariableGuid,
    IN UINT8    *pVarBuffer,
    IN UINTN    VarSize
){
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT32      Attributes;
    UINTN       Size, Offset;
    AMI_EFI_VARIABLE_AUTHENTICATION_2 *AuthHdr2;

    if(SecureVariableName == NULL || SecureVariableGuid == NULL )
        return EFI_INVALID_PARAMETER;

    if(Action != SET_SECURE_VARIABLE_DEL) {

        if(pVarBuffer == NULL || VarSize == 0 )
            return EFI_INVALID_PARAMETER;

        if(((Action & SET_SECURE_VARIABLE_SET_RT) == SET_SECURE_VARIABLE_SET_RT) &&
           (SecureVariableNameDefault == NULL) )
            return EFI_INVALID_PARAMETER;
    }

    Size = 0;
    Status = pRS->GetVariable( SecureVariableName, SecureVariableGuid, NULL, &Size, NULL);
    // Var exists
    if(Status == EFI_BUFFER_TOO_SMALL &&
       ((Action & SET_SECURE_VARIABLE_DEL) == SET_SECURE_VARIABLE_DEL)) {
        // try to erase. should succeed if system in pre-boot and Admin mode
        Status = pRS->SetVariable(SecureVariableName,SecureVariableGuid,0,0,NULL);
        DEBUG((SecureBoot_DEBUG_LEVEL,"Clear NV Var %s(%r)\n",SecureVariableName, Status));
        if(EFI_ERROR(Status))
            return Status;
    }
    if(Status == EFI_NOT_FOUND)
        Status = EFI_SUCCESS;

    Attributes = (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS);

    if((Action & (SET_SECURE_VARIABLE_SET_RT)) == SET_SECURE_VARIABLE_SET_RT) {

        AuthHdr2 = (AMI_EFI_VARIABLE_AUTHENTICATION_2*)pVarBuffer;
        Offset = sizeof(EFI_TIME) + AuthHdr2->AuthInfo.Hdr.dwLength;
        Status = pRS->SetVariable(SecureVariableNameDefault,
                &gEfiGlobalVariableGuid,
                Attributes,
                VarSize - Offset,
                (UINT8*)pVarBuffer + Offset
        );
        DEBUG((SecureBoot_DEBUG_LEVEL,"Set RT Var %s(%r)\n",SecureVariableNameDefault, Status));
    }

    if((Action & (SET_SECURE_VARIABLE_SET)) == SET_SECURE_VARIABLE_SET) {

        Attributes |= (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS);

        if((Action & (SET_SECURE_VARIABLE_APPEND)) == SET_SECURE_VARIABLE_APPEND)
            Attributes |= EFI_VARIABLE_APPEND_WRITE;

        Status = pRS->SetVariable(SecureVariableName,
                SecureVariableGuid,
                Attributes,
                VarSize,
                pVarBuffer
        );
        DEBUG((SecureBoot_DEBUG_LEVEL,"Set NV Var %s(%r)\n",SecureVariableName, Status));
        if(Status == EFI_NOT_FOUND) // Set of a zero-length variable
            return EFI_SUCCESS;

        if(EFI_ERROR(Status))
            return Status;

        VarSize = 0;
        pRS->GetVariable( SecureVariableName, SecureVariableGuid, NULL, &VarSize, NULL);
        DEBUG((SecureBoot_DEBUG_LEVEL,"Old Var Size %Xh\nNew Var size %Xh\n", Size, VarSize));
        if(((Action & SET_SECURE_VARIABLE_APPEND)== SET_SECURE_VARIABLE_APPEND) && Size == VarSize)
            Status = EFI_ALREADY_STARTED;
    }

    return Status;
}

/**
  Install a list of Secure Boot policy Variables

  @param[in]  Action Supported Types:
                SET_SECURE_VARIABLE_DEL 1
                SET_SECURE_VARIABLE_SET 2
                SET_SECURE_VARIABLE_APPEND 8
                SET_SECURE_VARIABLE_SET_RT 4
                SET_RT_DEFAULT_KEYS 4

  @retval     Status
**/
EFI_STATUS
EFIAPI
AmiInstallSecureBootVariables (
    IN UINT16    Action,
    IN UINT16    SecureVariableSigDatabaseCount,
    IN CHAR16   *SecureVariableNameList[],
    IN CHAR16   *SecureVariableNameListDefault[] OPTIONAL,
    IN CONST EFI_GUID *SecureVariableFileGuid[]
){
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8      *pVarBuffer = NULL;
    UINTN       FileSize;
    UINT8       Index;
    EFI_GUID    *EfiVarGuid;

    if(SecureVariableNameList == NULL || SecureVariableFileGuid == NULL )
        return EFI_INVALID_PARAMETER;

    if(((Action & (SET_RT_DEFAULT_KEYS)) == SET_RT_DEFAULT_KEYS) &&
       (SecureVariableNameListDefault == NULL))
        return EFI_INVALID_PARAMETER;

///////////////////////////////////////////////////////////////////////////////
// Initial provisioning of Authenticated non-volatile EFI Variables 
///////////////////////////////////////////////////////////////////////////////
    Index = 0;
    while(/*!EFI_ERROR(Status) &&*/ SecureVariableNameList[Index] != NULL)
    {
        if(Index < SecureVariableSigDatabaseCount)
            EfiVarGuid = &gEfiImageSecurityDatabaseGuid;
        else
            EfiVarGuid = &gEfiGlobalVariableGuid;

        pVarBuffer = NULL;
        FileSize = 0 ; 
        if(Action & (SET_NV_DEFAULT_KEYS | SET_RT_DEFAULT_KEYS)) {
            Status = GetSectionFromAnyFv (SecureVariableFileGuid[Index], EFI_SECTION_RAW, 0, &pVarBuffer, (UINTN*)&FileSize);
            DEBUG((SecureBoot_DEBUG_LEVEL,"Get Raw image %s(%r), sz = %x\n",SecureVariableNameList[Index], Status, FileSize));
            if(EFI_ERROR(Status)) {
                Index++;
                continue;
            }
        }
        Status = AmiSetSecureBootVariable(Action, SecureVariableNameList[Index], SecureVariableNameListDefault[Index], EfiVarGuid, pVarBuffer, FileSize );

        if(pVarBuffer != NULL)
            pBS->FreePool(pVarBuffer);

        Index++;
    }

    return Status;
}

/**
  Provisioning of all factory default Secure Boot policy Variables

  @param[in]  Action Supported Types:
                SET_NV_DEFAULT_KEYS
                SET_RT_DEFAULT_KEYS

  @retval     Status
**/
EFI_STATUS
EFIAPI
AmiInstallSecureBootDefaults (
    IN UINT16    Action
){
    return AmiInstallSecureBootVariables (Action, SecureVariableSigDatabaseCount, SecureVariableFileName, SecureVariableFileNameDefault, SecureVariableFileGuid);
}

/**
  Fill Authenticated_Header2
  @param[in]   UINT8 *pVar - pointer to the Variable data buffer
  @param[in]   UINT8  VarSetMode - Append flag

  @retval     None
**/
VOID
EFIAPI
AmiFillAuthenticatedHdr2(
    UINT8*  pVar,
    UINT8   VarSetMode
){
    AMI_EFI_VARIABLE_AUTHENTICATION_2 *AuthHdr2;
    EFI_TIME EfiTime = {
        FOUR_DIGIT_YEAR_INT,
        TWO_DIGIT_MONTH_INT,
        TWO_DIGIT_DAY_INT,
        TWO_DIGIT_HOUR_INT,
        TWO_DIGIT_MINUTE_INT,
        TWO_DIGIT_SECOND_INT,0,0,0,0,0
    };

    if(pRS) {
        pRS->GetTime(&EfiTime, NULL);
        EfiTime.Pad1 = 0;
        EfiTime.Nanosecond = 0;
        EfiTime.TimeZone = 0;
        EfiTime.Daylight = 0;
        EfiTime.Pad2 = 0;
    }
    DEBUG((SecureBoot_DEBUG_LEVEL, "EfiTime Now : (Year=%d Month=%d Day=%d Hour=%d Min=%d Sec=%d)\n",
           EfiTime.Year, EfiTime.Month, EfiTime.Day, EfiTime.Hour, EfiTime.Minute, EfiTime.Second));

    AuthHdr2 = (AMI_EFI_VARIABLE_AUTHENTICATION_2*)pVar;
    MemCpy (&AuthHdr2->TimeStamp, &EfiTime, sizeof (EFI_TIME));
    if((VarSetMode & SET_SECURE_VARIABLE_APPEND)== SET_SECURE_VARIABLE_APPEND)
        AuthHdr2->TimeStamp.Year = 2000; // append should not change the original date the var was created
    AuthHdr2->AuthInfo.Hdr.dwLength = sizeof(WIN_CERTIFICATE_UEFI_GUID_1);
    AuthHdr2->AuthInfo.Hdr.wRevision = 0x200;
    AuthHdr2->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
    AuthHdr2->AuthInfo.CertType = gEfiCertPkcs7Guid;

    return;
}

/**
  Fill Authenticated Variable Hdr2 followed by Signature List Hdr

  @param[in]   UINT8 *pVar - pointer to the Variable data buffer
  @param[in]   UINT8 *pCert - Pointer to a Certificate structure
  @param[in]   UINTN CertSize - Size of input Certificate data
  @param[in]   UINT8  VarSetMode - Append flag

  @retval     Status
**/
EFI_STATUS
EFIAPI
AmiFillAuthenticatedVariableHdr(
    UINT8 *pVar,
    UINT8 *pCert,
    UINTN CertSize,
    UINT8 VarSetMode
){
    EFI_STATUS Status;
    EFI_VARIABLE_SIG_HDR_2 *AuthHdr2;    
    static EFI_GUID    AmiSigOwner = AMI_APTIO_SIG_OWNER_GUID;
    EFI_GUID          *pGuid;
    UINT8 *subject;
    UINTN  str_len = 256;

    AuthHdr2 = (EFI_VARIABLE_SIG_HDR_2*)pVar;

    AmiFillAuthenticatedHdr2(pVar, VarSetMode); // Append AuthHdr to Var data.

    if(CertSize == EFI_CERT_TYPE_SHA256_SIZE)
        pGuid = &gEfiCertSha256Guid;
    if(CertSize == EFI_CERT_TYPE_CERT_X509_SHA256_GUID_SIZE)
        pGuid = &gEfiCertX509Sha256Guid;
    if(CertSize == EFI_CERT_TYPE_CERT_X509_SHA384_GUID_SIZE)
        pGuid = &gEfiCertX509Sha384Guid;
    if(CertSize == EFI_CERT_TYPE_CERT_X509_SHA512_GUID_SIZE)
        pGuid = &gEfiCertX509Sha512Guid;
    if(CertSize == EFI_CERT_TYPE_RSA2048_SIZE)
        pGuid = &gEfiCertRsa2048Guid;
    if(CertSize > EFI_CERT_TYPE_RSA2048_SIZE) {
        Status = EFI_LOAD_ERROR;
        if(mAmiSig == NULL)
            Status = pBS->LocateProtocol(&gAmiDigitalSignatureProtocolGuid, NULL, &mAmiSig);
        if(mAmiSig == NULL)
            return Status;
        subject = (void*) AllocateZeroPool (str_len);
        if(subject == NULL)
            return Status;
        Status = mAmiSig->Pkcs7Verify (mAmiSig,
                pCert, CertSize,
                NULL, 0,
                &subject, &str_len,
                x509GetSubjectsCommonNameStr,
                RESET // Flags 
        );
        DEBUG((SecureBoot_DEBUG_LEVEL, "Cert parse: %r\nSubject Name: %a\n", Status, subject));
        if(subject != NULL)
            pBS->FreePool(subject);
        if(EFI_ERROR(Status))
            return Status;

        pGuid = &gEfiCertX509Guid;
    }
//      CopyMem (&AuthHdr2->SigList.SignatureType, *pGuid, sizeof (EFI_GUID));
    AuthHdr2->SigList.SignatureType = *pGuid;
    AuthHdr2->SigList.SignatureSize = (UINT32)(sizeof(EFI_GUID)+CertSize);
    AuthHdr2->SigList.SignatureListSize = AuthHdr2->SigList.SignatureSize+sizeof(EFI_SIGNATURE_LIST);
    AuthHdr2->SigList.SignatureHeaderSize = 0;
    AuthHdr2->SigData.SignatureOwner = AmiSigOwner;

DEBUG((SecureBoot_DEBUG_LEVEL,"SigList Type=%g\nSigSize=%x\nListSize=%x\n", &(AuthHdr2->SigList.SignatureType), AuthHdr2->SigList.SignatureSize, AuthHdr2->SigList.SignatureListSize));

    return EFI_SUCCESS;
}

/**
  Validate the data payload begins with valid Signature List header
  and based on the results returns Status.
        
  @param[in]   *Data - pointer to the Var data
  @param[in]   DataSize - size of Var data
  @param[out]  SigCertCount OPTIONAL - Number of Certificates encountered inside the Data
  @param[out]  SigListCount OPTIONAL - Number of Signature Lists encountered inside the Data
  
  @return EFI_SECURITY_VIOLATION          The Signature does NOT pass the validation
                                          check carried out by the firmware.
  @return EFI_SUCCESS                     Data passed validation successfully.

**/
EFI_STATUS
EFIAPI
AmiValidateSignatureList (
    IN VOID         *Data,
    IN UINTN        DataSize,
    IN OUT UINTN    *SigCertCount OPTIONAL,
    IN OUT UINTN    *SigListCount OPTIONAL
){
    EFI_STATUS          Status;
    EFI_SIGNATURE_LIST *SigList;
    UINTN               Index, certCount, listCount, SigSize;
    static EFI_GUID mSignatureSupport[SIGSUPPORT_NUM] = {SIGSUPPORT_LIST};

    if(DataSize == 0 || DataSize > NVRAM_SIZE || Data == NULL)
        return EFI_SECURITY_VIOLATION; // Sig not found

// loop till end of DataSize for all available SigLists
// Verify signature is one from SigDatabase list mSignatureSupport / sizeof(EFI_GUID)
// SigData begins with SigOwner GUID
// SignatureHdrSize = 0 for known Sig Types

    SigList  = (EFI_SIGNATURE_LIST *)Data;
    Status = EFI_SECURITY_VIOLATION;
    certCount = 0;
    listCount = 0;
    while ((DataSize > 0) && (DataSize >= SigList->SignatureListSize)) {

DEBUG((SecureBoot_DEBUG_LEVEL,"DataSize (%X)\n", DataSize));

        for (Index = 0; Index < SIGSUPPORT_NUM; Index++) {
            if (!guidcmp ((EFI_GUID*) &(SigList->SignatureType), &mSignatureSupport[Index]))
                break;
        }
DEBUG((SecureBoot_DEBUG_LEVEL,"SigList.Type(%g)\n", &(SigList->SignatureType)));
        if(Index >= SIGSUPPORT_NUM)
            return EFI_SECURITY_VIOLATION; // Sig not found

DEBUG((SecureBoot_DEBUG_LEVEL,"OK\nSigList.Size(2D < %X < %X)\n",SigList->SignatureListSize, NVRAM_SIZE));
        if(SigList->SignatureListSize < 0x2D || // Min size for a dummy SigList with 1 byte in SigList.SigData.Data
           SigList->SignatureListSize > NVRAM_SIZE)
            return EFI_SECURITY_VIOLATION; 

DEBUG((SecureBoot_DEBUG_LEVEL,"OK\nSigList.HdrSize(%X)\n",SigList->SignatureHeaderSize));
        if(SigList->SignatureHeaderSize != 0)
            return EFI_SECURITY_VIOLATION; // Sig not found

DEBUG((SecureBoot_DEBUG_LEVEL,"OK\nSigList.SigSize(%X)\n",SigList->SignatureSize));
        listCount ++;
        for(SigSize=sizeof(EFI_SIGNATURE_LIST); SigSize < SigList->SignatureListSize; SigSize+=SigList->SignatureSize)
            certCount++;
DEBUG((SecureBoot_DEBUG_LEVEL,"Sig.ListCount(%d), CertCount(%d)\n", listCount, certCount));

        DataSize -= SigList->SignatureListSize;
        SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);

        Status = EFI_SUCCESS;
    }
    if(SigCertCount)
        *SigCertCount = certCount;
    if(SigListCount)
        *SigListCount = listCount;

    return Status;
}
/**
  Locate and return a pointer to Certificate inside of a Signature List Database

  @param[in]   LookUpCertEntry - pointer to Certificate entry
  @param[in]   VOID *bufIn - pointer to the Var data
  @param[in]   UINTN bufSize - size of Var data
  @param[out]  *SigListOut - Pointer to a signature list
  
  @return Efi Status

**/
EFI_STATUS EFIAPI
AmiFindCertificateInSignatureList( 
    IN SDL_X509_SIG_ENTRY *LookUpCertEntry,
    IN VOID      *bufIn,
    IN UINTN     bufSize,
    IN OUT EFI_SIGNATURE_LIST **SigListOut OPTIONAL
    )
{
    EFI_STATUS Status;
    UINTN Count, i;
    EFI_SIGNATURE_DATA *SigData;
    EFI_SIGNATURE_LIST *SigList;
    UINT8* pCert;
    UINTN  CertSize;
    CHAR8  *Subject;
    UINTN  str_len;
    EFI_GUID *OwnerGuid;

    // To launch the files from the selected file system
    if(bufSize == 0 || bufIn == NULL)
        return EFI_INVALID_PARAMETER; // Sig not found

    if(LookUpCertEntry == NULL || LookUpCertEntry->SubjectName == NULL)
        return EFI_INVALID_PARAMETER; // Sig not found

    Status = AmiValidateSignatureList (bufIn, bufSize, NULL, &Count);
    if(EFI_ERROR(Status))
        goto exit;

    if( mAmiSig == NULL)
         Status = pBS->LocateProtocol(&gAmiDigitalSignatureProtocolGuid, NULL, &mAmiSig);
    if(EFI_ERROR(Status) || mAmiSig == NULL)
         goto exit;

    str_len = AsciiStrLen(LookUpCertEntry->SubjectName)+1;    // max Subject string len to compare
    DEBUG((SecureBoot_DEBUG_LEVEL,"Rid of '%a', Len %Xh, CertCount %d\n",LookUpCertEntry->SubjectName, str_len, Count));
    Subject = (void*) AllocateZeroPool (str_len);
    if(Subject == NULL) {
         DEBUG((SecureBoot_DEBUG_LEVEL,"Allocate buffer fail\n"));
         goto exit;
    }

    SigList  = (EFI_SIGNATURE_LIST *)bufIn;
    Status   =  EFI_NOT_FOUND;
    for(i = 0; i < Count; i++) {
        SigData   = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigList + sizeof (EFI_SIGNATURE_LIST) + SigList->SignatureHeaderSize);
        pCert = (UINT8*)SigData->SignatureData;
        CertSize = (UINTN)SigList->SignatureSize-sizeof(EFI_GUID);
        OwnerGuid = (EFI_GUID*)&SigData->SignatureOwner;
        DEBUG((SecureBoot_DEBUG_LEVEL,"SigList %x\nOwner Guid %g\nLookup Guid %g\n", SigList, OwnerGuid, (EFI_GUID*)&LookUpCertEntry->OwnerGuid));
        if(!guidcmp ((EFI_GUID*) &(SigList->SignatureType), &gEfiCertX509Guid) &&
           !guidcmp (OwnerGuid, (EFI_GUID*)&LookUpCertEntry->OwnerGuid)
        ) {
            MemSet(Subject, str_len, 0);
            Status = mAmiSig->Pkcs7Verify (
                 mAmiSig,
                 pCert, CertSize,
                 NULL, 0,
                 &Subject, &str_len,
                 x509GetSubjectsCommonNameStr,
                 RESET // Flags 
            );
DEBUG((SecureBoot_DEBUG_LEVEL, "Cert parse: %r\nSubject Name: '%a'\n", Status, Subject));
            // continue to next list if Status=Error. string size may be too small - not our Cert anyway
            if(!EFI_ERROR(Status) && 
              (MemCmp(Subject, LookUpCertEntry->SubjectName, str_len) == 0) ) {
                if(SigListOut != NULL)
                    *SigListOut = SigList;
                break;
            }
            Status   =  EFI_NOT_FOUND;
        }
        SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
    } // for
exit:
    // Free the allocated menu list space
    if(Subject != NULL) 
        pBS->FreePool(Subject);

    return Status;
}

/**
  Scan through Signature List data and remove the Certificate from the list.
          
  @param[in]   LookUpCertEntry - Subject name string to look up in Certificate list
  @param[in]   bufIn - pointer to the Var data
  @param[in]   bufSize - size of Var data
  @param[out]  bufSize - modified size of data
  
  @return EFI_SUCCESS           Data was successfully updated

**/
EFI_STATUS 
EFIAPI 
AmiRemoveCertificateFromSignatureList(
    SDL_X509_SIG_ENTRY *LookUpCertEntry,
    IN VOID         *bufIn,
    IN OUT UINTN    *bufSize
){
    EFI_STATUS Status;
    EFI_SIGNATURE_LIST *SigList;
    UINTN    SigSize, CopySize;

    // To launch the files from the selected file system
    if(bufSize == NULL || *bufSize == 0 || bufIn == NULL)
        return EFI_INVALID_PARAMETER; // Sig not found

    if(LookUpCertEntry == NULL || LookUpCertEntry->SubjectName == NULL)
        return EFI_INVALID_PARAMETER; // Sig not found

    Status = AmiFindCertificateInSignatureList( LookUpCertEntry, bufIn, *bufSize, (EFI_SIGNATURE_LIST**)&SigList);
    DEBUG((SecureBoot_DEBUG_LEVEL,"Find Cert in a SigList (%r)\nSig %x\n", Status, SigList));
    if(!EFI_ERROR(Status)) {
        // A valid item found inside SigList
        // delete selected SignatureList
        // move cert list up at the address. Upd size etc...
        SigSize = SigList->SignatureListSize;
        CopySize = ((UINTN)bufIn+*bufSize) - ((UINTN)SigList+SigSize);
        // replace this SigData block with data following it
        MemCpy (SigList, (void*)((UINTN)SigList+SigSize), CopySize);
        *bufSize = *bufSize - SigSize;
    }

    return Status;
}

/**
  SecureBootLibBadCertificates hook removes blacklisted certificates from the SignatureList database
  
  @param[in]   BadCertificateList - Optional, pointer to the bad certificate list to be removed form the Signature List 
  @param[in]   VarBuf - pointer to the Signature List to be processed
  @param[in]   VarSize - size of the Signature List data
  @param[out]  VarSize - modified size of data
  
  @retval EFI_SUCCESS       The hook completes successfully.
  @retval Other values      The hook cannot complete due to some error.
**/
EFI_STATUS
EFIAPI
AmiRemoveBadCertificates(
    IN SDL_X509_SIG_ENTRY *BadCertificateList OPTIONAL,
    IN VOID         *VarBuf,
    IN OUT UINTN    *VarSize
){
    EFI_STATUS Status;
    SDL_X509_SIG_ENTRY *List;
    UINTN   Index;

    if (VarBuf == NULL || VarSize == NULL)
        return EFI_INVALID_PARAMETER; 

    if(BadCertificateList == NULL)
        List = SecureBootLibBadCertificateList;
    else 
        List = BadCertificateList;

    Status = EFI_SUCCESS;
    // Scan list 
    for (Index = 0; 
         List->SubjectName != NULL && !EFI_ERROR(Status); 
         List = &List[++Index] 
    ){
        Status = AmiRemoveCertificateFromSignatureList(List, VarBuf, VarSize);
    }

    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
