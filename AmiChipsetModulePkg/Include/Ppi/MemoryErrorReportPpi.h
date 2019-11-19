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
/** @file MemoryErrorReportPpi.h
    This header file contains the PPI definition for the NB.

**/
//*************************************************************************

#ifndef _PEI_MEMORY_ERROR_REPORT_PPI_H_
#define _PEI_MEMORY_ERROR_REPORT_PPI_H_

//85226559-0def-48d8-a8c9-b746d6a4df01
#define AMI_MEMORY_ERROR_REPORT_PPI_GUID \
  { 0x85226559, 0x0def, 0x48d8, 0xa8, 0xc9, 0xb7, 0x46, 0xd6, 0xa4, 0xdf, 0x01 }

//9831BFF1-D7F8-4c92-8DBB-56CA2488E189
#define AMI_CHECK_DIMM_VOLTAGE_FAIL_PPI_GUID \
  { 0x9831bff1, 0xd7f8, 0x4c92, 0x8d, 0xbb, 0x56, 0xca, 0x24, 0x88, 0xe1, 0x89 }

typedef struct _PEI_MEMORY_ERROR_REPORT_PPI  PEI_MEMORY_ERROR_REPORT_PPI;

typedef
EFI_STATUS
(EFIAPI *PEI_AMI_MEMORY_ERROR_READ) (
  IN      EFI_PEI_SERVICES              **PeiServices,  ///< Pointer to PeiServices
  IN      PEI_MEMORY_ERROR_REPORT_PPI   *This,          ///< Pointer to the PPI structure
  IN OUT  UINT32                        MemErrData      ///< Memory Error Data Buffer
);

struct _PEI_MEMORY_ERROR_REPORT_PPI {
    PEI_AMI_MEMORY_ERROR_READ           AmiMemoryErrorRead;
};

extern EFI_GUID gAmiMemoryErrorReportPpiGuid;
extern EFI_GUID gAmiCheckDimmVoltageFailPpiGuid;

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
