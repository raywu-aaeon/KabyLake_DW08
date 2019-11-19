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
// $Header:  $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log:  $
// 
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		NBCIEfi.c
//
// Description:	This file installs 
//              Nbci EFI protocol
//
// Notes:		MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
#include <AmiDxeLib.h>
#include <Dxe.h>
#include <Protocol/Nbci.h>

extern EFI_BOOT_SERVICES    *pBS;
extern EFI_RUNTIME_SERVICES *pRS;

//
// Defined in Mxm30Dxe.c
//
extern  VOID                *gMxmLegMemAddr;
extern  UINTN                gMxmDataSize;
/////////////////////////////////////////
//     Nbci EFI PROTOCOL PROTOTYPES    //
/////////////////////////////////////////

EFI_STATUS NbciReturnSpecLevel (
  IN NBCI_EFI_PROTOCOL *This,
  IN OUT CHAR8        *pucRevisionLevel,
  OUT UINTN           *puSupportFuncs
);

EFI_STATUS NbciReturnStructure (
  IN NBCI_EFI_PROTOCOL *This,
  IN EFI_HANDLE       ControllerHandle,
  IN OUT CHAR16       *usStructSize,
  IN CHAR16           usDataBlockID,
  OUT CHAR8           **pNbciStruct
);

EFI_STATUS NbciSelectOutputDevice (
  IN NBCI_EFI_PROTOCOL *This,
  IN EFI_HANDLE      ControllerHandle,
  IN OUT CHAR16      usStructSize,
  IN EFI_HANDLE      ChildHandle,
  IN CHAR8           ucOutputSetting
);

EFI_STATUS NbciCheckOutputDevice (
  IN NBCI_EFI_PROTOCOL *This,
  IN EFI_HANDLE       ControllerHandle,
  IN OUT CHAR16       usStructSize,
  IN EFI_HANDLE       ChildHandle,
  OUT CHAR8           ucDisplayAvailable
);

NBCI_EFI_PROTOCOL   NBCIProtocol = {
  NbciReturnSpecLevel,
  NbciReturnStructure,
  NbciSelectOutputDevice,
  NbciCheckOutputDevice
};

////////////////////////////////
// Nbci EFI PROTOCOL FUNCTIONS //
////////////////////////////////

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   NbciReturnSpecLevel
//
// Description: Caller provides pointer to location, which on call contains the Module requested Spec Revision
//              Level, and which on exit, will contain the Platform's preferred Revision Level and Bit Field of
//              supported functions.
//
// Input:       
//              CHAR8       *pucRevisionLevel
//              UINTN       *puSupportFuncs
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
//          Bit 1 = `1' (NbciReturnStucture is also required)
//          Bit 2 = `1' if NbciSelectOutputDevice is supported, `0' if not
//          Bit 3 = `1' if NbciCheckOutputDevice is supported, `0' if not
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NbciReturnSpecLevel (
  IN NBCI_EFI_PROTOCOL  *This,
  IN OUT CHAR8          *pucRevisionLevel,
  OUT UINTN             *puSupportFuncs
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  *pucRevisionLevel = EFI_NBCI_DataBlockID;
  *puSupportFuncs = EFI_NBCI_SupportFuncs;

  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   NbciReturnStructure
//
// Description: Caller provides expected Nbci Interface Revision Level and a pointer which, 
//              on return will contain the Nbci structure.
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
//       ControllerHandle - used to differentiate which Nbci module is being referenced. 
//          This is the EFI handle for the PCI device. ChildHandle where used, is the output device (the display).
//       usStructSize - set to the size of the supplied buffer by the caller, and on return should contain the
//          actual size of the structure. If the structure is larger than the buffer, EFI_BUFFER_TOO_SMALL
//          should be returned as the status of the call
//      usDataBlockID - Identifier for the data block to return. To obtain the Nbci information structure, use usDataBlockID[0..7] to specify the revision
//          of the Nbci software specification that is supported by the Nbci module.Format is binary coded decimal, for example: 0030h = 3.0, etc.
//          To obtain an additional vendor specific data block, use lower 8 bits to specify an identifier in the range 0x80-0x8F. 
//          If the system BIOS does not contain Nbci Graphics Module Software Specification Version 3.0 such a data block, it should return an error. 
//          Graphics vendors should not assume a specific assignment within this range - the SBIOS can store
//          the data for different vendors in any order in the 0x80-0x8F identifier range
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NbciReturnStructure (
  IN NBCI_EFI_PROTOCOL *This,
  IN EFI_HANDLE       ControllerHandle,
  IN OUT CHAR16       *usStructSize,
  IN CHAR16           usDataBlockID,
  OUT CHAR8          **pNbciStruct
  )
{
  // Only supports Rev 3.0
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
  if(usDataBlockID != EFI_NBCI_DataBlockID ||  !gMxmDataSize) {
    return EFI_UNSUPPORTED;
  }
  //
  // calculate the offset within *pNbciStruct by comparing the DevID in the Nbci Header with given controller handle DevID
  //
  *pNbciStruct = (CHAR8*)gMxmLegMemAddr;
  *usStructSize = (CHAR16)gMxmDataSize;
#else
  return EFI_UNSUPPORTED;
#endif
  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   NbciSelectOutputDevice
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
EFI_STATUS NbciSelectOutputDevice (
  IN NBCI_EFI_PROTOCOL *This,
  IN EFI_HANDLE      ControllerHandle,
  IN OUT CHAR16      usStructSize,
  IN EFI_HANDLE      ChildHandle,
  IN CHAR8           ucOutputSetting
  )
{
  return EFI_UNSUPPORTED;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   NbciCheckOutputDevice
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
EFI_STATUS NbciCheckOutputDevice (
  IN NBCI_EFI_PROTOCOL *This,
  IN EFI_HANDLE       ControllerHandle,
  IN OUT CHAR16       usStructSize,
  IN EFI_HANDLE       ChildHandle,
  OUT CHAR8           ucDisplayAvailable
  )
{
  return EFI_UNSUPPORTED;
}

///////////////////////////////////////
// END of Nbci EFI PROTOCOL FUNCTIONS //
///////////////////////////////////////

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   NBCIInterfaces
//
// Description: Common entry point to install:
// 1. Install Nbci protocol; 
//
// Input:       EFI_HANDLE           ImageHandle
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
NBCIInterfaces (
  IN EFI_HANDLE         ImageHandle
  )
{
  EFI_STATUS  Status;
  EFI_GUID    EfiNbciProtocolGuid = NBCI_EFI_GUID;

  //
  // Install NBCI_EFI_PROTOCOL
  //
  Status = pBS->InstallProtocolInterface (
                  &ImageHandle,
                  &EfiNbciProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &NBCIProtocol
                  );
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
