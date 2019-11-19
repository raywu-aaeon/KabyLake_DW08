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
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************************


/** @file SbPeiBoard.c
    This file contains PEI stage board component code for
    Template SB

**/


// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <AmiSbGpio.h>     // for GPIO table
#include <Library/DebugLib.h>

// Produced PPIs

// GUID Definitions

// Portable Constants

// Function Prototypes

// PPI interface definition

// Function Definition

// GPIO Elink Sample code >>>
EFI_STATUS SbGetOemGpioTable(
    IN EFI_PEI_SERVICES             **PeiServices,
    OUT GPIO_INIT_CONFIG             **GpioTable,
    OUT UINTN                       *TableSize
)
{
#if CRB_BOARD == 0 // RVP3
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: Before %x, %x in SbGetOemGpioTable\n", *GpioTable, &mGpioTableLpDdr3Rvp3, *TableSize));
    *GpioTable = mGpioTableLpDdr3Rvp3;
    *TableSize = sizeof(mGpioTableLpDdr3Rvp3);
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: %x, %x in SbGetOemGpioTable\n", *GpioTable, &mGpioTableLpDdr3Rvp3, *TableSize));
#elif (CRB_BOARD == 2 || CRB_BOARD == 8)// RVP7 or RVP15
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: Before %x, %x in SbGetOemGpioTable\n", *GpioTable, &mGpioTableDdr3LrRvp7, *TableSize));
    *GpioTable = mGpioTableDdr3LrRvp7;
    *TableSize = sizeof(mGpioTableDdr3LrRvp7);
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: %x, %x in SbGetOemGpioTable\n", *GpioTable, &mGpioTableDdr3LrRvp7, *TableSize));
#elif (CRB_BOARD == 12)// RVP17
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: Before %x, %x in SbGetOemGpioTable\n", *GpioTable, &mGpioTableKblRvp17, *TableSize));
    *GpioTable = mGpioTableKblRvp17;
    *TableSize = sizeof(mGpioTableKblRvp17);
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: %x, %x in SbGetOemGpioTable\n", *GpioTable, &mGpioTableKblRvp17, *TableSize));    
#endif
    
    return EFI_SUCCESS;
}
// GPIO Elink Sample code <<<


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

