//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
// $Header: 	$
//
// $Revision: 	$
//
// $Date:		$
//**********************************************************************
// Revision History
// ----------------
// $Log:		$
// 
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		Mxm30Efi.c
//
// Description:	This file installs 
//              1)Mxm EFI protocol
//              2)Mxm Legacy Int15 and 
//              3)Acpi interfaces for Switchable Graphics
//
// Notes:		MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
#include <AmiDxeLib.h>
#include <Dxe.h>
#include <Protocol/Mxm30.h>


extern EFI_BOOT_SERVICES    *pBS;
extern EFI_RUNTIME_SERVICES *pRS;

// Defined in Mxm30Dxe.c
extern VOID                 *gMxmData;
extern UINTN                gMxmDataSize;
/////////////////////////////////////////
//     MXM EFI PROTOCOL PROTOTYPES     //
/////////////////////////////////////////

EFI_STATUS MxmReturnSpecLevel (
    IN MXM3_EFI_PROTOCOL *This,
    IN EFI_HANDLE       ControllerHandle,
    IN OUT CHAR16       usStructSize,
    IN OUT CHAR8        *pucRevisionLevel,
    OUT UINTN           *puSupportFuncs
);

EFI_STATUS MxmReturnStructure (
    IN MXM3_EFI_PROTOCOL *This,
    IN EFI_HANDLE       ControllerHandle,
    IN OUT CHAR16       *usStructSize,
    IN CHAR16           usDataBlockID,
    OUT CHAR8           **pMxmStruct
);

EFI_STATUS MxmSelectOutputDevice (
    IN MXM3_EFI_PROTOCOL *This,
    IN EFI_HANDLE      ControllerHandle,
    IN OUT CHAR16      usStructSize,
    IN EFI_HANDLE      ChildHandle,
    IN CHAR8           ucOutputSetting
);

EFI_STATUS MxmCheckOutputDevice (
    IN MXM3_EFI_PROTOCOL *This,
    IN EFI_HANDLE       ControllerHandle,
    IN OUT CHAR16       usStructSize,
    IN EFI_HANDLE       ChildHandle,
    OUT CHAR8           ucDisplayAvailable
);

MXM3_EFI_PROTOCOL   Mxm30Protocol = {
    MxmReturnSpecLevel,
    MxmReturnStructure,
    MxmSelectOutputDevice,
    MxmCheckOutputDevice
};

////////////////////////////////
// MXM EFI PROTOCOL FUNCTIONS //
////////////////////////////////

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   MxmReturnSpecLevel
//
// Description: Caller provides pointer to location, which on call contains the Module requested Spec Revision
//              Level, and which on exit, will contain the Platform's preferred Revision Level and Bit Field of
//              supported functions.
//
// Input:       UINT8       *This
//              EFI_HANDLE   ControllerHandle
//              UCHAR16     usStructSize
//
// Output:      EFI_STATUS =
//              EFI_SUCCESS, EFI_UNSUPPORTED, EFI_INVALID_PARAMETER, EFI_BUFFER_TOO_SMALL
//
// Modified:
//
// Referrals:
//
// Notes:
//          Where the integer value passed in uSupportFuncs indicates supported functions:
//          Bit 0 = `1' (this method is required)
//          Bit 1 = `1' (MxmReturnStucture is also required)
//          Bit 2 = `1' if MxmSelectOutputDevice is supported, `0' if not
//          Bit 3 = `1' if MxmCheckOutputDevice is supported, `0' if not
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS MxmReturnSpecLevel (
    IN MXM3_EFI_PROTOCOL *This,
    IN EFI_HANDLE       ControllerHandle,
    IN OUT CHAR16      usStructSize,
    IN OUT CHAR8       *pucRevisionLevel,
    OUT UINTN           *puSupportFuncs
){
    EFI_STATUS  Status = EFI_SUCCESS;

    *pucRevisionLevel = EFI30_DataBlockID;
    *puSupportFuncs = EFI30_SupportFuncs;

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   MxmReturnStructure
//
// Description: Caller provides expected MXM Interface Revision Level and a pointer which, 
//              on return will contain the MXM structure.
//
// Input:       UINT8       *This
//              EFI_HANDLE   ControllerHandle
//              UCHAR16     usStructSize
//
// Output:      EFI_STATUS =
//              EFI_SUCCESS, EFI_UNSUPPORTED, EFI_INVALID_PARAMETER, EFI_BUFFER_TOO_SMALL
//
// Modified:
//
// Referrals:
//
// Notes:
//       ControllerHandle - used to differentiate which MXM module is being referenced. 
//          This is the EFI handle for the PCI device. ChildHandle where used, is the output device (the display).
//       usStructSize - set to the size of the supplied buffer by the caller, and on return should contain the
//          actual size of the structure. If the structure is larger than the buffer, EFI_BUFFER_TOO_SMALL
//          should be returned as the status of the call
//      usDataBlockID - Identifier for the data block to return. To obtain the MXM information structure, use usDataBlockID[0..7] to specify the revision
//          of the MXM software specification that is supported by the MXM module.Format is binary coded decimal, for example: 0030h = 3.0, etc.
//          To obtain an additional vendor specific data block, use lower 8 bits to specify an identifier in the range 0x80-0x8F. 
//          If the system BIOS does not contain MXM Graphics Module Software Specification Version 3.0 such a data block, it should return an error. 
//          Graphics vendors should not assume a specific assignment within this range - the SBIOS can store
//          the data for different vendors in any order in the 0x80-0x8F identifier range
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS MxmReturnStructure (
    IN MXM3_EFI_PROTOCOL *This,
    IN EFI_HANDLE       ControllerHandle,
    IN OUT CHAR16       *usStructSize,
    IN CHAR16           usDataBlockID,
    OUT CHAR8          **pMxmStruct
){
    EFI_STATUS  Status = EFI_SUCCESS;
// Only supports Rev 3.0
    if(usDataBlockID != EFI30_DataBlockID)
        return EFI_UNSUPPORTED;

// calculate the offset within *pMxmStruct by comparing the DevID in the MXM Header with given controller handle DevID
    *pMxmStruct = gMxmData;
    *usStructSize = (CHAR16)gMxmDataSize;

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   MxmSelectOutputDevice
//
// Description: Select Output Device Channel. 
//              Caller provides the handle of the target adapter, the handle of the display and the output setting
//
// Input:       UINT8       *This
//              EFI_HANDLE   ControllerHandle
//              UCHAR16     usStructSize
//
// Output:      EFI_STATUS =
//              EFI_SUCCESS, EFI_UNSUPPORTED, EFI_INVALID_PARAMETER, EFI_BUFFER_TOO_SMALL
//
// Modified:
//
// Referrals:
//
// Notes:
//              Where the value passed in ucOutputSetting may indicate one of the following actions:
//              0 - Acquire shared Display DDC
//              1 - Enable this Display Output
//              3 - Enable this Display - Both Output and DDC
//              4 - Release shared Display DDC
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS MxmSelectOutputDevice (
    IN MXM3_EFI_PROTOCOL *This,
    IN EFI_HANDLE      ControllerHandle,
    IN OUT CHAR16      usStructSize,
    IN EFI_HANDLE      ChildHandle,
    IN CHAR8           ucOutputSetting
){

    return EFI_UNSUPPORTED;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   MxmCheckOutputDevice
//
// Description: Check Availability of Output Device. Caller provides the handle of 
//              the target adapter and the handle of the display
//
// Input:       UINT8       *This
//              EFI_HANDLE   ControllerHandle
//              UCHAR16     usStructSize
//
// Output:      EFI_STATUS =
//              EFI_SUCCESS, EFI_UNSUPPORTED, EFI_INVALID_PARAMETER, EFI_BUFFER_TOO_SMALL
//              UCHAR8 ucDisplayAvailable
//
// Modified:
//
// Referrals:
//
// Notes:   The returned value ucDisplayAvailable will be one of the following values:
//          0 - Connector is currently available
//          1 - Connector is not currently available
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS MxmCheckOutputDevice (
    IN MXM3_EFI_PROTOCOL *This,
    IN EFI_HANDLE       ControllerHandle,
    IN OUT CHAR16       usStructSize,
    IN EFI_HANDLE       ChildHandle,
    OUT CHAR8           ucDisplayAvailable
){

    return EFI_UNSUPPORTED;
}

///////////////////////////////////////
// END of MXM EFI PROTOCOL FUNCTIONS //
///////////////////////////////////////

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   MXM30Interfaces
//
// Description: Entry point of EFI MXM Support driver. The function istalls 
// 1. Install MXM protocol; 
// 2. Update data structure to be passed to Int15 binary
//      must support multiple MXM data modules. Dependent on controller's handle
//  Load MXM data block <1Mb area.
//      a. Copy MXM Structure below 1MB
//          1. Allocate Legacy mem space for MXM dat <1MB
//          OR          
//          2. Update the MXM data by the pointer returned by Int15 5F80 Func01(default)
//      b. Update the pointer to MXM data in EFI Protocol API
//      c. Create an ACPI SSDT table and copy MXM block there (ASL MXM logic must handle different MXM modules...)
//
// Input:       EFI_HANDLE           ImageHandle,
//              EFI_SYSTEM_TABLE     *SystemTable
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
MXM30Interfaces (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS  Status;
    EFI_GUID  gEfiMxm3ProtocolGuid = MXM3_EFI_GUID;

    Status = pBS->InstallProtocolInterface (
                &ImageHandle,
                &gEfiMxm3ProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &Mxm30Protocol
                );
    DEBUG ((DEBUG_INFO, "==Install Mxm30Protocol  %r====\n", Status));           
    ASSERT_EFI_ERROR (Status);

    return Status;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
