/** @file
  EFI SMM Communication Protocol as defined in the PI 1.2 specification.

  This protocol provides a means of communicating between drivers outside of SMM and SMI 
  handlers inside of SMM.  

  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef _SMM_COMMUNICATION_H_
#define _SMM_COMMUNICATION_H_

#include <Protocol/MmCommunication.h>


typedef EFI_MM_COMMUNICATE_HEADER EFI_SMM_COMMUNICATE_HEADER;

#define EFI_SMM_COMMUNICATION_PROTOCOL_GUID EFI_MM_COMMUNICATION_PROTOCOL_GUID

typedef EFI_MM_COMMUNICATION_PROTOCOL EFI_SMM_COMMUNICATION_PROTOCOL;

extern EFI_GUID gEfiSmmCommunicationProtocolGuid;

//*** AMI PORTING BEGIN ***//
// Backward compatibility.
// SMM Communication ACPI Table was deprecated in UEFI 2.6B.
// Corresponding headers/definitions have been removed from EDKII.
// We're keeping them in Aptio until next Core label to ensure compatibility with older versions of UefiCpuPkg and RC.
#include <Uefi/UefiAcpiDataTable.h>
#pragma pack(1)
typedef struct {
  EFI_ACPI_DATA_TABLE  UefiAcpiDataTable;
  UINT32               SwSmiNumber;
  UINT64               BufferPtrAddress;
} EFI_SMM_COMMUNICATION_ACPI_TABLE;
#pragma pack()
//*** AMI PORTING END *****//
#endif

