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

//**********************************************************************

/** @file SbGpio.h
    South Bridge GPIO header file.

**/
//*************************************************************************
#ifndef __AMI_SB_GPIO_H__
#define __AMI_SB_GPIO_H__

//
// Generic GPIO definition.
// Output from GpioConfig.h,GpioLib.hGpioPinsSklLp.h,and GpioPinsSklh.h.
//
#include <AmiGpioConfig.h>
#include <AmiGpioLib.h>
#include <AmiGpioPinsSklLp.h>
#include <AmiGpioPinsSklH.h>

//
// AMI PPI definition for the SB.
//
#include <Ppi/SbPpi.h>

// GPIO Table Terminator
#ifndef END_OF_GPIO_TABLE
#define END_OF_GPIO_TABLE 0xFFFFFFFF
#endif

//
// Output from sample Gpio Table header files.
// If you want to modify for your platform's GPIO setting,
// please follow the sample and replace the OUTPUTREGISTER "AmiGpioTable" to your header file's path.
//
#include <AmiGpioTable.h>

//
//Below is the sample code of GPIO INIT PPI.
//
/*
#if defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE == 2
#ifdef mGpioTableDdr3LrRvp7
AMI_GPIO_INIT_PPI Rvp7GpioInitPpi = { 
    GPIO_BASE_ADDRESS,
    mGpioTableDdr3LrRvp7,
    sizeof(mGpioTableDdr3LrRvp7),
    TRUE
};
#endif
AMI_SB_PCI_SSID_TABLE_STRUCT Rvp7SbSsidTable[] = { 
    SB_PCI_DEVICES_SSID_TABLE
};
#ifdef mGpioTableDdr3LrRvp7
static AMI_PEI_SB_CUSTOM_PPI Rvp7SbCustomPpi = {
    &Rvp7GpioInitPpi, 
    Rvp7SbSsidTable
};
#endif
//
// Should be defined in C code.
//
static EFI_PEI_PPI_DESCRIPTOR Rvp7CustomPpi[] = {
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gAmiPeiSBCustomPpiGuid, &Rvp7SbCustomPpi },
};
#endif
*/

#endif // __AMI_SB_GPIO_H__
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
