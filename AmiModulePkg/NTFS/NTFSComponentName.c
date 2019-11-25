//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/NTFS Driver/NTFSComponentName.c 1     6/09/11 3:06p Pats $
//
// $Revision: 1 $
//
// $Date: 6/09/11 3:06p $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: NTFSComponentName.c
//
// Description: NTFS controller name functions
//
//<AMI_FHDR_END>
//**********************************************************************

#include "NTFS.h"

#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID //old Core
#ifndef LANGUAGE_CODE_ENGLISH
#define LANGUAGE_CODE_ENGLISH "eng"
#endif
static BOOLEAN LanguageCodesEqual(
    CONST CHAR8* LangCode1,
    CONST CHAR8* LangCode2
    )
{
    return LangCode1[0]==LangCode2[0]
           && LangCode1[1]==LangCode2[1]
           && LangCode1[2]==LangCode2[2];
}
#endif
//----------------------------------------------------------------------

EFI_STATUS
NTFSCtlDriverName(
    IN EFI_COMPONENT_NAME_PROTOCOL *This,
    IN CHAR8                       *Language,
    OUT CHAR16                     **DriverName
    );

EFI_STATUS
NTFSCtlGetControllerName(
    IN EFI_COMPONENT_NAME_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_HANDLE ChildHandle      OPTIONAL,
    IN CHAR8                       *Language,
    OUT CHAR16                     **ControllerName
    );

//----------------------------------------------------------------------

CHAR16                             *gNTFSDriverName = L"AMI NTFS Driver";
CHAR16                             *gNTFSControllerName = L"NTFS File System";
extern EFI_GUID                    guidFS;
extern EFI_DRIVER_BINDING_PROTOCOL gNTFSDriverBinding;
//==================================================================================
EFI_COMPONENT_NAME_PROTOCOL        gNTFSDriverNames = {
    NTFSCtlDriverName,
    NTFSCtlGetControllerName,
    LANGUAGE_CODE_ENGLISH
};


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure:   NTFSCtlDriverName
//
// Description: Retrieves a Unicode string that is the user readable name of
//              the EFI Driver.
//
//
// Parameters:
//   EFI_COMPONENT_NAME_PROTOCOL
//    This       - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
//   CHAR8
//    Language   - A pointer to a three character ISO 639-2 language identifier.
//                 This is the language of the driver name that that the caller
//                 is requesting, and it must match one of the languages specified
//                 in SupportedLanguages.  The number of languages supported by a
//                 driver is up to the driver writer.
//   CHAR16
//    DriverName - A pointer to the Unicode string to return.  This Unicode string
//                 is the name of the driver specified by This in the language
//                 specified by Language.
//
// Return value:
//    EFI_SUCCES            - The Unicode string for the Driver specified by This
//                            and the language specified by Language was returned
//                            in DriverName.
//    EFI_INVALID_PARAMETER - Language is NULL.
//    EFI_INVALID_PARAMETER - DriverName is NULL.
//    EFI_UNSUPPORTED       - The driver specified by This does not support the
//                            language specified by Language.
//
// Modified:
//
// Referral(s):
//
// NOTE(S):
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
NTFSCtlDriverName(
    IN EFI_COMPONENT_NAME_PROTOCOL *This,
    IN CHAR8                       *Language,
    OUT CHAR16                     **DriverName
    )
{
    if (!Language || !DriverName) return EFI_INVALID_PARAMETER;

    if (!LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH))
        return EFI_UNSUPPORTED;

    *DriverName = gNTFSDriverName;
    return EFI_SUCCESS;
}



//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure:   NTFSCtlGetControllerName
//
// Description: Retrieves a Unicode string that is the user readable name of
//              the controller that is being managed by an EFI Driver.
//
// Parameters:
//   EFI_COMPONENT_NAME_PROTOCOL
//    This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
//   EFI_HANDLE
//    ControllerHandle - The handle of a controller that the driver specified by
//                       This is managing.  This handle specifies the controller
//                       whose name is to be returned.
//   EFI_HANDLE
//    ChildHandle      - The handle of the child controller to retrieve the name
//                       of.  This is an optional parameter that may be NULL.  It
//                       will be NULL for device drivers.  It will also be NULL
//                       for a bus drivers that wish to retrieve the name of the
//                       bus controller.  It will not be NULL for a bus driver
//                       that wishes to retrieve the name of a child controller.
//   CHAR8
//    Language         - A pointer to a three character ISO 639-2 language
//                       identifier.  This is the language of the controller name
//                       that that the caller is requesting, and it must match one
//                       of the languages specified in SupportedLanguages.  The
//                       number of languages supported by a driver is up to the
//                       driver writer.
//   CHAR16
//    ControllerName   - A pointer to the Unicode string to return.  This Unicode
//                       string is the name of the controller specified by
//                       ControllerHandle and ChildHandle in the language
//                       specified by Language from the point of view of the
//                       driver specified by This.
//
// Return value:
//    EFI_SUCCESS           - The Unicode string for the user readable name in the
//                            language specified by Language for the driver
//                            specified by This was returned in DriverName.
//    EFI_INVALID_PARAMETER - ControllerHandle is not a valid EFI_HANDLE.
//    EFI_INVALID_PARAMETER - ChildHandle is not NULL and it is not a valid
//                            EFI_HANDLE.
//    EFI_INVALID_PARAMETER - Language is NULL.
//    EFI_INVALID_PARAMETER - ControllerName is NULL.
//    EFI_UNSUPPORTED       - The driver specified by This is not currently
//                            managing the controller specified by
//                            ControllerHandle and ChildHandle.
//    EFI_UNSUPPORTED       - The driver specified by This does not support the
//                            language specified by Language.
//
// Modified:
//
// Referral(s):
//
// NOTE(S):
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
NTFSCtlGetControllerName(
    IN EFI_COMPONENT_NAME_PROTOCOL *This,
    IN EFI_HANDLE                  Controller,
    IN EFI_HANDLE ChildHandle      OPTIONAL,
    IN CHAR8                       *Language,
    OUT CHAR16                     **ControllerName
    )
{

    EFI_STATUS                      Status;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *NTFS;
    VOLUME_INFO                     *vi;

    if (!Language || !ControllerName) return EFI_INVALID_PARAMETER;

    if (ChildHandle != NULL) return EFI_UNSUPPORTED;

    Status = pBS->OpenProtocol ( Controller,
                                 &guidFS,
                                 &NTFS,
                                 gNTFSDriverBinding.DriverBindingHandle,
                                 Controller,
                                 EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;

    vi = (VOLUME_INFO *)NTFS;

    //  Compare the Language
    if (!LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH))
        return EFI_UNSUPPORTED;

    if (vi->NTFSName) *ControllerName = vi->NTFSName;
    else *ControllerName = gNTFSControllerName;

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
