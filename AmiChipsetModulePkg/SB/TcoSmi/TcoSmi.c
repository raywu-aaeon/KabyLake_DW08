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

/** @file TcoSmi.c
    This file register Tco Smi Handler to IchnDispatch

**/

//----------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/PchTcoSmiDispatch.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
//----------------------------------------------------------------------------

// TCO1 Status
// B_PCH_TCO1_STS_DMISMI                     0x0400
// B_PCH_TCO1_STS_BIOSWR                     0x0100
// B_PCH_TCO1_STS_NEWCENTURY                 0x0080
// B_PCH_TCO1_STS_TIMEOUT                    0x0008
// B_PCH_TCO1_STS_SW_TCO_SMI                 0x0002
// B_PCH_TCO1_STS_NMI2SMI                    0x0001

#ifndef TCO1_SMI_HANDLER_REGISTER
#define TCO1_SMI_HANDLER_REGISTER    B_PCH_TCO1_STS_NEWCENTURY    // Default register for NEWCENTURY_STS
#endif

/**
    This function converts data from DEC to BCD format

    @param Dec value to be converted

    @retval UINT8 result of conversion

**/
UINT8 DecToBCD(
    IN UINT8 Dec
)
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}

/**
    This function converts data from BCD to DEC format

    @param IN UINT8 BCD - value to be converted

    @retval UINT8 result of conversion

**/
UINT8 BCDToDec(IN UINT8 BCD)
{
    UINT8 FirstDigit = BCD & 0xf;
    UINT8 SecondDigit = BCD >> 4;;

    return SecondDigit * 10  + FirstDigit;
}

/**
    Handle TcoSmi generated

    @param  DispatchHandle,
**/
VOID
EFIAPI DummyTcoSmiHandler (
    IN  EFI_HANDLE                      DispatchHandle)
{
    UINT8   Century;
    UINT8   Value;
    
    //DEBUG((DEBUG_INFO, "### DummyTcoSmiHandler ###\n"));
    IoWrite8(CMOS_ADDR_PORT, ACPI_CENTURY_CMOS);
    Century = IoRead8(CMOS_DATA_PORT); // Read register.
    
    Value = BCDToDec(Century);
    Value++;
    Century = DecToBCD(Value);
    
    IoWrite8(CMOS_ADDR_PORT, ACPI_CENTURY_CMOS);
    IoWrite8(CMOS_DATA_PORT, Century);
}

/**
    In SMM Function for TcoSmi SMM driver.

    @param[in] ImageHandle  Image handle of this driver.
    @param[in] SystemTable  A Pointer to the EFI System Table.

    @return EFI_STATUS
    @retval EFI_SUCCESS
**/
EFI_STATUS
InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                              Status;
    PCH_TCO_SMI_DISPATCH_PROTOCOL           *mPchTcoSmiDispatchProtocol;
    EFI_HANDLE                              Handle;

    Status = InitAmiSmmLib (ImageHandle, SystemTable);
    if (EFI_ERROR(Status)) return Status;
    
    mPchTcoSmiDispatchProtocol = NULL;
    Status = gSmst->SmmLocateProtocol (&gPchTcoSmiDispatchProtocolGuid, NULL, (VOID **) &mPchTcoSmiDispatchProtocol);
    if (EFI_ERROR(Status)) return Status;
    
    Handle = NULL;
    Status = mPchTcoSmiDispatchProtocol->NewCenturyRegister(mPchTcoSmiDispatchProtocol, DummyTcoSmiHandler, &Handle);
            
    return Status;
}

/**
    Entry Point for TcoSmi SMM driver.

    @param[in] ImageHandle  Image handle of this driver.
    @param[in] SystemTable  A Pointer to the EFI System Table.

    @return EFI_STATUS
    @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
InitializeTcoSmm (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
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
