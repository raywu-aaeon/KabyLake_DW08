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
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************************
/** @file CrbPei.c
    This file contains code for Chipset Reference Board Template
    initialization in the PEI stage

**/
//*****************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiPeiLib.h>
#include <AmiCspLib.h>
#include <Ppi/CrbInfo.h>
#include <Library/BaseMemoryLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
UINT8
CrbPeiGetChipsetVendorNo (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

UINT32
CrbPeiGetCpuId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

UINT32
CrbPeiGetNorthBridgePciId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

UINT32
CrbPeiGetSouthBridgePciId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

CHAR8*
CrbPeiGetProjectId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

UINT16
CrbPeiGetBiosRevision (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)
CHAR8 CONST ProjectTag[5] = CONVERT_TO_STRING(CRB_PROJECT_TAG);

// GUID Definition(s)

// PPI Definition(s)
AMI_PEI_CRBINFO_PPI gAmiCrbInforPpi = {
    CrbPeiGetChipsetVendorNo,
    CrbPeiGetCpuId,
    CrbPeiGetNorthBridgePciId,
    CrbPeiGetSouthBridgePciId,
    CrbPeiGetProjectId,
    CrbPeiGetBiosRevision,
};

// Function Definition(s)

// PPI that are installed
static EFI_PEI_PPI_DESCRIPTOR gCrbPpiList[] =  
{
    {
        (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
        &gAmiPeiCrbInfoPpiGuid,
        &gAmiCrbInforPpi
    }
};

// PPI that are notified

//----------------------------------------------------------------------------

/**
    Provide the chipset vendor number.

    @param VOID

    @retval 0 Unknown.
    @retval 1 Intel.
    @retval 2 AMD.

    @note  .
**/
UINT8
CrbPeiGetChipsetVendorNo (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
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
        AMI_PEI_CRBINFO_PPI   *This

    @retval EAX : Version Information (Type, Family, Model, and Stepping ID)

    @note  .
**/
UINT32
CrbPeiGetCpuId (
  IN  CONST AMI_PEI_CRBINFO_PPI     *This
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
CrbPeiGetNorthBridgePciId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
  )
{
    UINT32 PciId = -1;

    if (This->GetChipsetVendorNo(This) == 1) {
        PciId = PciRead32(PCI_LIB_ADDRESS(0, 0, 0, 0));
    } else if (This->GetChipsetVendorNo(This) == 2) {
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
CrbPeiGetSouthBridgePciId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
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
CrbPeiGetProjectId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
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
CrbPeiGetBiosRevision (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
  )
{
    return (CRB_PROJECT_MAJOR_VERSION << 8) | CRB_PROJECT_MINOR_VERSION;
}

/**
    This function is the entry point for this PEI. This function
    initializes the chipset CRB

    @param FfsHeader   Pointer to the FFS file header
    @param PeiServices Pointer to the PEI services table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

    @note  This function could initialize CRB for anything.

**/

EFI_STATUS EFIAPI CrbPeiInit (
    IN       EFI_PEI_FILE_HANDLE   FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices )
{
    EFI_STATUS              Status = EFI_SUCCESS;
        
    Status = (*PeiServices)->InstallPpi (PeiServices, &gCrbPpiList[0]);
    ASSERT_PEI_ERROR (PeiServices, Status); 
       
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
