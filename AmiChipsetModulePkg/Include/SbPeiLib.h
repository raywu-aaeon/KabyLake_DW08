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

//*************************************************************************
// $Header:
//
// $Revision:
//
// $Date:
//*************************************************************************
/** @file SbPeiLib.h
    South Bridge PEI Library header file, define all the South
    Bridge Function prototype in this file. 

**/
//*************************************************************************

#ifndef __SB_PEI_LIB_H__ // To Avoid this header get compiled twice
#define __SB_PEI_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

BOOLEAN IsS3 (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo
);

BOOLEAN IsS4 (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo
);

BOOLEAN IsCmosBad (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo 
);

EFI_STATUS AmiSbUpdateBootMode (
    IN EFI_PEI_SERVICES         **PeiServices
//    IN EFI_PEI_CPU_IO_PPI       *CpuIo,
//    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
);

EFI_STATUS AmiBoardInit (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN UINT16               BoardId
);

VOID ProgramSbSubId(
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
);

VOID ProgramMeSubId (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN EFI_PEI_PCI_CFG2_PPI     *PciCfg        
);

EFI_STATUS AmiAdjustOemGpioConfig (
    IN EFI_PEI_SERVICES     **PeiServices
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
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
