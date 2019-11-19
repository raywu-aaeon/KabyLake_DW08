//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file CrbDxe.c
    This file contains code for Chipset Reference Board Template
    initialization in the DXE stage

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------
#include <Efi.h>
#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <SaRegs.h>
#include <PchAccess.h>
#include <Protocol/CrbInfo.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#if (defined(SmbiosUpdateData_SUPPORT) && (SmbiosUpdateData_SUPPORT == 1))
#include <Protocol/AmiSmbios.h>
#include <Protocol/SmbiosUpdateDataProtocol.h>
#endif
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)
#define SMBIOS_NOT_FOUND        0xFFFF
// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
UINT8
CrbDxeGetChipsetVendorNo (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

UINT32
CrbDxeGetCpuId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

UINT32
CrbDxeGetNorthBridgePciId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

UINT32
CrbDxeGetSouthBridgePciId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

CHAR8*
CrbDxeGetProjectId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

UINT16
CrbDxeGetBiosRevision (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

#if defined CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT && CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT
EFI_STATUS  CrbOutPeiMemInfoDriverBindingSupported ( 
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
);

EFI_STATUS  CrbOutPeiMemInfoDriverBindingStart ( 
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
);

EFI_STATUS  CrbOutPeiMemInfoDriverBindingStop ( 
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   ControllerHandle,
    IN  UINTN                       NumberOfChildren,
    IN  EFI_HANDLE                  *ChildHandleBuffer
);

EFI_STATUS GetCrbOutPeiMemInfoDriverName (
    IN EFI_COMPONENT_NAME2_PROTOCOL *This,
    IN CHAR8                        *Language,
    OUT CHAR16                      **DriverName 
);
EFI_STATUS GetCrbOutPeiMemInfoControllerName (
    IN EFI_COMPONENT_NAME2_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_HANDLE                   ChildHandle OPTIONAL,
    IN CHAR8                        *Language,
    OUT CHAR16                      **ControllerName 
);
#endif
//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)
EFI_EVENT       gEvent;
VOID            *gNotifyReg = NULL;
CHAR8 CONST     ProjectTag[5] = CONVERT_TO_STRING(CRB_PROJECT_TAG);

// GUID Definition(s)

// Protocol Definition(s)
AMI_EFI_CRBINFO_PROTOCOL  gAmiCrbInforProtocol = {
    CrbDxeGetChipsetVendorNo,
    CrbDxeGetCpuId,
    CrbDxeGetNorthBridgePciId,
    CrbDxeGetSouthBridgePciId,
    CrbDxeGetProjectId,
    CrbDxeGetBiosRevision,
};

#if defined CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT && CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT
static BOOLEAN         gEntryToShell = FALSE;
EFI_HOB_HANDOFF_INFO_TABLE *pHit = NULL;


static CHAR16  *gGetPeiMemString[] = {
	L"=============== Pei Memory Information ==============\n\r",
    L"Pei Memory Base = ",
    L"Pei Memory Size = ",
    L"Used Pei Memory Size = ",
    L"Free Pei Memory Size = ",
    L"=====================================================\n\r"
};

CHAR16 *gCrbOutPeiMemInfoDriverName = L"AMI CRB Get Pei Memory information driver";
CHAR16 *gCrbOutPeiMemInfoControllerName = L"Get Pei Memory information";

EFI_COMPONENT_NAME2_PROTOCOL gCrbOutPeiMemInfoDriverNameProtocol = {
	GetCrbOutPeiMemInfoDriverName,
	GetCrbOutPeiMemInfoControllerName,
    "eng"
};


EFI_DRIVER_BINDING_PROTOCOL gCrbOutPeiMemInfoBindingProtocol = {
	CrbOutPeiMemInfoDriverBindingSupported,
	CrbOutPeiMemInfoDriverBindingStart,
	CrbOutPeiMemInfoDriverBindingStop,
    1,
    NULL,
    NULL
    };
#endif
// External Declaration(s)

// Function Definition(s)

//----------------------------------------------------------------------------
#if defined CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT && CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT

/**
    Retrieves a Unicode string that is the user readable name of
    the EFI Driver.

    @param This       - A pointer to the EFI_COMPONENT_NAME_PROTOCOL
        instance.
    @param Language   - A pointer to a three character ISO 639-2 language
        identifier. This is the language of the driver
        name that that the caller is requesting, and it
        must match one of the languages specified in
        SupportedLanguages. The number of languages
        supported by a driver is up to the driver writer.
    @param DriverName - A pointer to the Unicode string to return.
        This Unicode string is the name of the driver
        specified by This in the language specified by
        Language.

    @retval EFI_SUCCES The Unicode string for the Driver specified by
        This and the language specified by Language was
        returned in DriverName.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER DriverName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This does not
        support the language specified by 
        Language.

    @note  GENERALLY NO PORTING REQUIRED
**/
EFI_STATUS GetCrbOutPeiMemInfoDriverName (
    IN EFI_COMPONENT_NAME2_PROTOCOL *This,
    IN CHAR8                        *Language,
    OUT CHAR16                      **DriverName )
{
    if(!Language || !DriverName) return EFI_INVALID_PARAMETER;
    if (MemCmp(Language, "eng", 3)) return EFI_UNSUPPORTED;
    *DriverName = gCrbOutPeiMemInfoDriverName;
    return EFI_SUCCESS;
}

/**
    Retrieves a Unicode string that is the user readable name of
    the controller that is being managed by an EFI Driver.

    @param This           - A pointer to the EFI_COMPONENT_NAME_PROTOCOL
        instance. 
    @param Controller     - The handle of a controller that the driver
        specified by This is managing. This handle
        specifies the controller whose name is to
        be returned.
    @param ChildHandle    - The handle of the child controller to
        retrieve the name of. This is an optional
        parameter that  may be NULL. It will be NULL
        for device drivers. It will also be NULL for
        a bus drivers that wish to retrieve the name
        of the bus controller.
        It will not be NULL for a bus driver that
        wishes to retrieve the name of a child
        controller. 
    @param Language       - A pointer to a three character ISO 639-2
        language identifier. This is the language of
        the controller name that that the caller is
        requesting, and it must match one of the
        languages specified in SupportedLanguages.
        The number of languages supported by a
        driver is up to the driver writer. 
    @param ControllerName - A pointer to the Unicode string to return.
        This Unicode string is the name of the
        controller specified by ControllerHandle and
        ChildHandle in the language specified by
        Language from the point of view of the
        driver specified by This.

    @retval EFI_SUCCESS The Unicode string for the user readable name
        in the language specified by Language for the
        driver specified by This was returned in
        DriverName.
    @retval EFI_INVALID_PARAMETER ControllerHandle is not a valid
        EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is
        not a valid EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER ControllerName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This is not
        currently managing the controller
        specified by ControllerHandle and
        ChildHandle.
    @retval EFI_UNSUPPORTED The driver specified by This does not
        support the language specified by 
        Language.

    @note  GENERALLY NO PORTING REQUIRED
**/
EFI_STATUS GetCrbOutPeiMemInfoControllerName (
    IN EFI_COMPONENT_NAME2_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_HANDLE                   ChildHandle OPTIONAL,
    IN CHAR8                        *Language,
    OUT CHAR16                      **ControllerName )
{
    if(!Language || !ControllerName) return EFI_INVALID_PARAMETER;
    if (MemCmp(Language, "eng", 3)) return EFI_UNSUPPORTED;
    *ControllerName = gCrbOutPeiMemInfoControllerName;
    return EFI_SUCCESS;
}

/**
    CrbOutPeiMemInfo Driver binding protocol Supported function

    @param This pointer to EFI_DRIVER_BINDING_PROTOCOL structure
    @param ControllerHandle handle of controller to serve
    @param RemainingDevicePath pointer to EFI_DEVICE_PATH_PROTOCOL structure

    @retval EFI_SUCCESS driver supports given controller
    @retval EFI_UNSUPPORTED given controller not supported  

**/
EFI_STATUS  CrbOutPeiMemInfoDriverBindingSupported ( 
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
	
    EFI_STATUS          Status;
    
    if (!gEntryToShell) return EFI_UNSUPPORTED;
   
    Status = pBS->OpenProtocol (
                            ControllerHandle,
                            &gEfiGraphicsOutputProtocolGuid,
                            NULL,
                            This->DriverBindingHandle,
                            ControllerHandle,
                            EFI_OPEN_PROTOCOL_TEST_PROTOCOL );
    
    if (!EFI_ERROR (Status)) {
       Status = pBS->OpenProtocol (
                               ControllerHandle,
                               &gEfiSimpleTextOutProtocolGuid,
                               NULL,
                               This->DriverBindingHandle,
                               ControllerHandle,
                               EFI_OPEN_PROTOCOL_TEST_PROTOCOL );
       if (!EFI_ERROR (Status)) {
	      return EFI_SUCCESS;
       }   
    }
    
    return EFI_UNSUPPORTED;
}

/**
    CrbOutPeiMemInfo Driver binding protocol Start function

    @param This pointer to EFI_DRIVER_BINDING_PROTOCOL structure
    @param ControllerHandle handle of controller to serve
    @param RemainingDevicePath pointer to EFI_DEVICE_PATH_PROTOCOL structure

    @retval EFI_SUCCESS driver supports given controller
    @retval EFI_UNSUPPORTED given controller not supported  

**/
EFI_STATUS  CrbOutPeiMemInfoDriverBindingStart ( 
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *SimpleTextOut;
	EFI_STATUS          Status;
    EFI_PHYSICAL_ADDRESS    PeiMemLen;
	EFI_PHYSICAL_ADDRESS    HobMemoryFreeBottom;
	EFI_PHYSICAL_ADDRESS              HobMemoryFreeTop;
	EFI_PHYSICAL_ADDRESS              UsedPeiMemSize;
	EFI_PHYSICAL_ADDRESS              FreePeiMemSize;
	CHAR16                  BuffString[50];   
    Status = pBS->OpenProtocol (
                            ControllerHandle,
                            &gEfiSimpleTextOutProtocolGuid,
                            (VOID**)&SimpleTextOut,
                            NULL,
                            NULL,
                            EFI_OPEN_PROTOCOL_GET_PROTOCOL );
    if (EFI_ERROR (Status)) return EFI_UNSUPPORTED;

    if (pHit == NULL)return EFI_UNSUPPORTED;

    PeiMemLen = pHit->EfiMemoryTop -  pHit->EfiMemoryBottom;
    HobMemoryFreeTop = pHit->EfiFreeMemoryTop;
    HobMemoryFreeBottom = pHit->EfiFreeMemoryBottom;
    UsedPeiMemSize = (HobMemoryFreeBottom - pHit->EfiMemoryBottom) + (pHit->EfiMemoryTop - HobMemoryFreeTop);
    FreePeiMemSize = HobMemoryFreeTop - HobMemoryFreeBottom;
    
    SimpleTextOut->OutputString (SimpleTextOut, L"\n\r");
    SimpleTextOut->OutputString (SimpleTextOut, gGetPeiMemString[0]);
    SimpleTextOut->OutputString (SimpleTextOut, L"\n\r");
    
    SimpleTextOut->OutputString (SimpleTextOut, gGetPeiMemString[1]);
    Swprintf(BuffString, L"%x\n\r",pHit->EfiMemoryBottom);
    SimpleTextOut->OutputString (SimpleTextOut, BuffString);
    
    SimpleTextOut->OutputString (SimpleTextOut, gGetPeiMemString[2]);
    Swprintf(BuffString, L"%x (%d) MB.\n\r", PeiMemLen, PeiMemLen / 1024 / 1024);
    SimpleTextOut->OutputString (SimpleTextOut, BuffString);

    SimpleTextOut->OutputString (SimpleTextOut, gGetPeiMemString[3]);
    Swprintf(BuffString, L"%d MB (%d) Bytes.\n\r", UsedPeiMemSize / 1024 / 1024, UsedPeiMemSize);
    SimpleTextOut->OutputString (SimpleTextOut, BuffString);
    
    SimpleTextOut->OutputString (SimpleTextOut, gGetPeiMemString[4]);
    Swprintf(BuffString, L"%d MB (%d) Bytes.\n\r", FreePeiMemSize / 1024 / 1024, FreePeiMemSize);
    SimpleTextOut->OutputString (SimpleTextOut, BuffString);
    
    SimpleTextOut->OutputString (SimpleTextOut, L"\n\r");
    SimpleTextOut->OutputString (SimpleTextOut, gGetPeiMemString[5]);
	return EFI_SUCCESS;
}

/**
    CrbOutPeiMemInfo Driver binding protocol Stop function

    @param This pointer to EFI_DRIVER_BINDING_PROTOCOL structure
    @param ControllerHandle handle of controller to serve
    @param NumberOfChildren number of child devices of controller
    @param ChildHandleBuffer pointer to child devices handles array

    @retval EFI_SUCCESS driver was successfully uninstalled from controller

**/
EFI_STATUS  CrbOutPeiMemInfoDriverBindingStop ( 
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   ControllerHandle,
    IN  UINTN                       NumberOfChildren,
    IN  EFI_HANDLE                  *ChildHandleBuffer
)
{

	return EFI_SUCCESS;
}

/**
    This function will set this gEntryToShell flag.
    
    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID
**/
VOID
EFIAPI
CrbReadyToShell (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
	gEntryToShell = TRUE;

    pBS->CloseEvent (Event);
    return;
}
#endif

/**
    Provide the chipset vendor number.

    @param VOID

    @retval 0 Unknown.
    @retval 1 Intel.
    @retval 2 AMD.

    @note  .
**/
UINT8
CrbDxeGetChipsetVendorNo (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
    UINT32      Index;
    UINT32      RegisterEax;
    UINT32      RegisterEbx;
    UINT32      RegisterEcx;
    UINT32      RegisterEdx;
    // Intel
    // EBX 756E6547 "uneG"
    // ECX 6C65746E "letn"
    // EDX 49656e69 "Ieni"
    // AMD
    //EBX 68747541 "htuA"
    //ECX 444D4163 "DMAc"
    //EDX 69746E65 "itne"
    CHAR8 CONST Intel[5] = "letnI";
    CHAR8 CONST Amd[3] = "DMA";
    CHAR8 MyString[5];
    UINT8 ChipsetVendorNo = 0;

    Index = 0;
    Index = AsmCpuid(Index, &RegisterEax, &RegisterEbx, &RegisterEcx, &RegisterEdx);
    MyString[4] = ((CHAR8*)&RegisterEdx)[3];
    MyString[3] = ((CHAR8*)&RegisterEcx)[0];
    MyString[2] = ((CHAR8*)&RegisterEcx)[1];
    MyString[1] = ((CHAR8*)&RegisterEcx)[2];
    MyString[0] = ((CHAR8*)&RegisterEcx)[3];

    if (CompareMem(MyString, Intel, 5) == 0) {
        ChipsetVendorNo = 1;
    } else if (CompareMem(MyString, Amd, 3) == 0) {
        ChipsetVendorNo = 2;
    }

    return ChipsetVendorNo;
}

/**
    Provide the CPU ID.

    @param 
        AMI_EFI_CRBINFO_PROTOCOL      *This

    @retval EAX : Version Information (Type, Family, Model, and Stepping ID)

    @note  .
**/
UINT32
CrbDxeGetCpuId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL    *This
  )
{
    UINT32  Index = 1;
    UINT32  RegisterEax = -1;
    // EAX : Version Information (Type, Family, Model, and Stepping ID)
    Index = AsmCpuid(Index, &RegisterEax, NULL, NULL, NULL);
    return RegisterEax;
}

/**
    Provide the PCI DID/VID of the north bridge.

    @param VOID

    @retval -1 - Undefined.
    @retval others PCI DID/VID.

    @note  .
**/
UINT32
CrbDxeGetNorthBridgePciId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
    UINT32 PciId = -1;

    if (This->GetChipsetVendorNo(This) == 1) {
        PciId = PciRead32(PCI_LIB_ADDRESS(0, 0, 0, 0));
    } else if (This->GetChipsetVendorNo(This) == 2){
        PciId = PciRead32(PCI_LIB_ADDRESS(0, 0, 0, 0));
    }

    return PciId;
}

/**
    Provide the PCI DID/VID of the south bridge.

    @param VOID

    @retval -1 - Undefined.
    @retval others PCI DID/VID.

    @note  .
**/
UINT32
CrbDxeGetSouthBridgePciId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
    UINT32 PciId = -1;

    if (This->GetChipsetVendorNo(This) == 1) {
        PciId = PciRead32(PCI_LIB_ADDRESS(0, 31, 0, 0));
    } else if (This->GetChipsetVendorNo(This) == 2){
        PciId = PciRead32(PCI_LIB_ADDRESS(0, 20, 0, 0));
    }

    return PciId;
}

/**
    Provide the project ID.

    @param VOID

    @retval NULL Undefined / error.
    @retval others Project ID.

    @note  .
**/
CHAR8*
CrbDxeGetProjectId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
    return ProjectTag;
}

/**
    Provide the BIOS revision.

    @param VOID

    @retval -1 - Undefined / error.
    @retval others BIOS revision.

    @note  .
**/
UINT16
CrbDxeGetBiosRevision (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
    return (CRB_PROJECT_MAJOR_VERSION << 8) | CRB_PROJECT_MINOR_VERSION;
}

#if (defined(SmbiosUpdateData_SUPPORT) && (SmbiosUpdateData_SUPPORT == 1))
/**
    Override SMBIOS Type 26.27.28.29 Location And Status value.

    @param[in] EFI_EVENT
    @param[in] VOID

**/
VOID
EFIAPI
CrbSmbiosUpdateType (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
    EFI_STATUS                      Status;
    EFI_SMBIOS_UPDATE_DATA_PROTOCOL *SmbiosUpdateDataProtocol;
#if ELECTRICAL_PROBE_INFO
    SMBIOS_ELECT_CURRENT_PROBE_INFO *T29_p;
#endif
#if TEMPERATURE_PROBE_INFO
    SMBIOS_TEMPERATURE_PROBE_INFO   *T28_p;
#endif    
#if COOLING_DEVICE_INFO
    SMBIOS_COOLING_DEV_INFO         *T27_p;
#endif    
#if VOLTAGE_PROBE_INFO
    SMBIOS_VOLTAGE_PROBE_INFO       *T26_p;
#endif
    UINT16                          Handle;

    SmbiosUpdateDataProtocol = NULL;
    Handle = SMBIOS_NOT_FOUND;
    Status = pBS->LocateProtocol(   &gSmbiosUpdateDataProtocolGuid, \
                                    NULL, \
                                    &SmbiosUpdateDataProtocol 
                                 );
    if (EFI_ERROR(Status)) {
        return ;
    }

#if TEMPERATURE_PROBE_INFO
    Handle = SmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(28,1);
    if (Handle != SMBIOS_NOT_FOUND) {
        T28_p = (SMBIOS_TEMPERATURE_PROBE_INFO *)SmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(Handle);
        T28_p->LocationAndStatus = 0x67;
        Handle = SMBIOS_NOT_FOUND;
    }
#endif

#if COOLING_DEVICE_INFO
    Handle = SmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(27,1);
    if (Handle != SMBIOS_NOT_FOUND) {
        T27_p = (SMBIOS_COOLING_DEV_INFO *)SmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(Handle);
        T27_p->DevTypeAndStatus = 0x67;
        Handle = SMBIOS_NOT_FOUND;
    }

    Handle = SmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(27,2);
    if (Handle != SMBIOS_NOT_FOUND) {
            T27_p = (SMBIOS_COOLING_DEV_INFO *)SmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(Handle);
            T27_p->DevTypeAndStatus = 0x67;
            Handle = SMBIOS_NOT_FOUND;
    }
#endif

#if VOLTAGE_PROBE_INFO
    Handle = SmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(26,1);
    if (Handle != SMBIOS_NOT_FOUND) {
        T26_p = (SMBIOS_VOLTAGE_PROBE_INFO *)SmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(Handle);
        T26_p->LocationAndStatus = 0x67;
        Handle = SMBIOS_NOT_FOUND;
    }
#endif

#if ELECTRICAL_PROBE_INFO
    Handle = SmbiosUpdateDataProtocol->SMBIOS_FindStructureByType(29,1);
    if (Handle != SMBIOS_NOT_FOUND) {
        T29_p = (SMBIOS_ELECT_CURRENT_PROBE_INFO *)SmbiosUpdateDataProtocol->SMBIOS_GetStructureBase(Handle);
        T29_p->LocationAndStatus = 0x67;
    }
#endif

    pBS->CloseEvent (Event);
    return;
}
#endif

/**
    This function is the entry point for this DXE. This function
    initializes the CRB

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS EFIAPI CrbDxeInit (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS          Status = EFI_SUCCESS;
#if (defined(SmbiosUpdateData_SUPPORT) && (SmbiosUpdateData_SUPPORT == 1))
    VOID                *RegistrationSmBios;
#endif
#if defined CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT && CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT    
    VOID                *RegistrationShell20File;
    VOID                *RegistrationShellProtocol;
    static EFI_GUID Shell20FileGuid = { 0x6302d008, 0x7f9b, 0x4f30, 0x87, 0xac, 0x60, 0xc9, 0xfe, 0xf5, 0xda, 0x4e };
    static EFI_GUID ShellProtocolGuid = { 0x47C7B223, 0xC42A, 0x11D2, 0x8E, 0x57, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B };
    EFI_EVENT   Shell20FileEvent;
    EFI_EVENT   ShellProtocolEvent;
#endif
    
    InitAmiLib(ImageHandle, SystemTable);  
    
    Status = pBS->InstallMultipleProtocolInterfaces(
                    &ImageHandle,
                    &gAmiEfiCrbInfoProtocolGuid, &gAmiCrbInforProtocol,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);    

#if (defined(SmbiosUpdateData_SUPPORT) && (SmbiosUpdateData_SUPPORT == 1))
    EfiCreateProtocolNotifyEvent (
           &gSmbiosUpdateDataProtocolGuid,
           TPL_CALLBACK,
           CrbSmbiosUpdateType,
           NULL,
           &RegistrationSmBios
           );
#endif

#if defined CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT && CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT  
    
    pHit = GetEfiConfigurationTable(pST, &gEfiHobListGuid);
    
    gCrbOutPeiMemInfoBindingProtocol.ImageHandle = ImageHandle;
    gCrbOutPeiMemInfoBindingProtocol.DriverBindingHandle = ImageHandle;
    
    Status = pBS->InstallMultipleProtocolInterfaces( \
                                             &ImageHandle, \
                                             &gEfiDriverBindingProtocolGuid, \
                                             &gCrbOutPeiMemInfoBindingProtocol, \
                                             &gEfiComponentName2ProtocolGuid,
                                             &gCrbOutPeiMemInfoDriverNameProtocol,
                                             NULL );
    
    RegisterProtocolCallback(
        &Shell20FileGuid,
        CrbReadyToShell,
        NULL, &Shell20FileEvent, &RegistrationShell20File
    );

    RegisterProtocolCallback(
        &ShellProtocolGuid,
        CrbReadyToShell,
        NULL, &ShellProtocolEvent, &RegistrationShellProtocol
    );
#endif
    
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
