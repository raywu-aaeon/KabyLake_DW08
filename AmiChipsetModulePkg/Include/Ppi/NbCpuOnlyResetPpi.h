//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
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
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Chipset/NB PPI/NBPPI.h 6     4/08/13 8:13a Ireneyang $
//
// $Revision: 6 $
//
// $Date: 4/08/13 8:13a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Chipset/NB PPI/NBPPI.h $
// 
//*************************************************************************
/** @file NbCpuOnlyResetPpi.h
    This header file contains the PPI definition for the NB.

**/
//*************************************************************************

#ifndef _PEI_NB_CPU_ONLY_RESET_PPI_H_
#define _PEI_NB_CPU_ONLY_RESET_PPI_H_
 
// {1F0F049E-3A68-4c97-865A-BC5EED7920E7}
#define AMI_PEI_NB_CPU_ONLY_RESET_PPI_GUID \
 {0x1f0f049e, 0x3a68, 0x4c97, 0x86, 0x5a, 0xbc, 0x5e, 0xed, 0x79, 0x20, 0xe7}  

typedef struct _EFI_PEI_NB_CPU_ONLY_RESET_PPI  EFI_PEI_NB_CPU_ONLY_RESET_PPI;

typedef VOID (EFIAPI *EFI_PEI_NB_CPU_ONLY_RESET) (
    IN EFI_PEI_SERVICES         **PeiServices
);

struct _EFI_PEI_NB_CPU_ONLY_RESET_PPI {
    EFI_PEI_NB_CPU_ONLY_RESET   GenerateCpuOnlyReset;
};

extern EFI_GUID gAmiPeiNbCpuOnlyResetPpiGuid;

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
