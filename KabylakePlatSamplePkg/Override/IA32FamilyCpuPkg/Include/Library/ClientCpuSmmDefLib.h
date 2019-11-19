/** @file
  Public include file for CPU definitions and CPU library functions.

@copyright
  Copyright (c) 2012 - 2014, Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the
  license agreement.
**/

#ifndef __CLIENT_CPU_SMM_DEF_H__
#define __CLIENT_CPU_SMM_DEF_H__

//
// Definition for CPUID Index
//
#define EFI_CPUID_FEATURE_FLAG                0x7

#pragma pack (1)

//
// Aligned to STM SAS Rev. 0.9
//
#define TXT_PROCESSOR_SMM_DESCRIPTOR_SIGNATURE \
  (('G' << 56) | ('I' << 48) | ('S' << 40) | ('S' << 32) |  \
   ('P' << 24) | ('T' << 16) | ('X' << 8) | ('T'))
#define TXT_PROCESSOR_SMM_DESCRIPTOR_VERSION_1 1
#define TXT_PROC_DESC_CURRENT_VERSION  \
  TXT_PROCESSOR_SMM_DESCRIPTOR_VERSION_1

typedef enum {
  TXT_SMM_PAGE_VIOLATION = 1,
  TXT_SMM_MSR_VIOLATION,
  TXT_SMM_REGISTER_VIOLATION,
  TXT_SMM_IO_VIOLATION,
  TXT_SMM_PCI_VIOLATION
} TXT_SMM_PROTECTION_EXCEPTION_TYPE;

typedef struct _STM_PROTECTION_EXCEPTION_HANDLER {
  UINT64                            SpeEip;
  UINT64                            SpeEsp;
  UINT16                            SpeSs;
  UINT16                            PageViolationException:1;
  UINT16                            MsrViolationException:1;
  UINT16                            RegisterViolationException:1;
  UINT16                            IoViolationException:1;
  UINT16                            PciViolationException:1;
  UINT16                            Reserved1:11;
  UINT32                            Reserved2;
} STM_PROTECTION_EXCEPTION_HANDLER;

typedef struct {
  UINT8                             ExecutionDisableOutsideSmrr:1;
  UINT8                             Intel64Mode:1;
  UINT8                             Cr4Pae : 1;
  UINT8                             Cr4Pse : 1;
  UINT8                             Reserved1 : 4;
} SMM_ENTRY_STATE;

typedef struct {
  UINT8                             SmramToVmcsRestoreRequired:1;
  UINT8                             ReinitializeVmcsRequired:1;
  UINT8                             Reserved2:6;
} SMM_RESUME_STATE;

typedef struct {
  UINT8                             DomainType:4;
  UINT8                             XStatePolicy:2;
  UINT8                             EptEnabled:1;
  UINT8                             Reserved3:1;
} SMM_STM_STATE;

typedef struct _TXT_PROCESSOR_SMM_DESCRIPTOR {
  UINT64                            Signature;
  UINT16                            Size;
  UINT16                            Version;
  UINT32                            LocalApicId;
  SMM_ENTRY_STATE                   SmmEntryState;
  SMM_RESUME_STATE                  SmmResumeState;
  SMM_STM_STATE                     SmmStmState;
  UINT8                             Reserved4;
  UINT16                            SmmCs;
  UINT16                            SmmDs;
  UINT16                            SmmSs;
  UINT16                            SmmOtherSegment;
  UINT16                            SmmTr;
  UINT16                            Reserved5;
  UINT64                            SmmCr3;
  UINT64                            SmmStmSetupRip;
  UINT64                            SmmStmTeardownRip;
  UINT64                            SmmSmiHandlerRip;
  UINT64                            SmmSmiHandlerRsp;
  UINT64                            SmmGdtPtr;
  UINT32                            SmmGdtSize;
  UINT32                            SmmRequiredStmRevId;
  STM_PROTECTION_EXCEPTION_HANDLER  StmProctectionExceptionHandler;
  UINT64                            MtrrBaseMaskPtr;
  UINT64                            BiosHwResourceRequirementsPtr;
} TXT_PROCESSOR_SMM_DESCRIPTOR;

#pragma pack ()

#endif
