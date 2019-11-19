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
/** @file SbChipsetPpi.h
    This header file contains the PPI definition for the SB.
*/

#ifndef __AMI_SB_CHIPSET_PPI_H__
#define __AMI_SB_CHIPSET_PPI_H__

//
// About GpioTable, please refer to KabylakeSiliconPkg\Pch\Include\Library\GpioLib.h
//

typedef struct _AMI_SB_PCI_DEVICES_TABLE_STRUCT {
    UINT64                      PciAddr;
    UINT8                       PciSidReg;
} AMI_SB_PCI_DEVICES_TABLE_STRUCT;

#endif

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
