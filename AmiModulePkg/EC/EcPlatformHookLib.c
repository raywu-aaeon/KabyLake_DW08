//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: EcPlatformHookLib.c
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Base.h>
#include <Token.h>
#include <AmiCspLib.h>
#include <Register/PchRegsLpc.h>

EFI_STATUS
EFIAPI
PchLpcIoDecodeRangesSet (
  UINT16                                LpcIoDecodeRanges
  );

EFI_STATUS
EFIAPI
PchLpcIoEnableDecodingSet (
  UINT16                                LpcIoEnableDecoding
  );


//<AMI_PHDR_START>
//**********************************************************************
// Procedure:   PlatformHookSerialPortInitialize
//
// Description: Performs platform specific initialization required.
//
// Input:       VOID
//
// Output:      RETURN_SUCCESS       The platform specific initialization succeeded.
//              RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.
//
//**********************************************************************
//<AMI_PHDR_END>

RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{

  PchLpcIoDecodeRangesSet (0x0070);
  PchLpcIoEnableDecodingSet (0x3c03);

  return  RETURN_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

