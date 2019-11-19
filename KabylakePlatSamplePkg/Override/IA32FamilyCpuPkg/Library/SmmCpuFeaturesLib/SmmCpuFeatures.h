/** @file
  The CPU specific programming for PiSmmCpuDxeSmm module.

@copyright
 Copyright (c) 2010 - 2017 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef __SMM_FEATURES_H__
#define __SMM_FEATURES_H__

#include <PiSmm.h>

#include <Library/SmmCpuFeaturesLib.h>

#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/LocalApicLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/MtrrLib.h>
#include <Library/ClientCpuSmmDefLib.h>      // RPPO-SKL-0018
#include <Library/SocketLga1156Lib.h>
#include <Library/SocketLga775Lib.h>
#include <Register/Cpuid.h>
#include <Library/IoLib.h>                   // RPPO-KBL-0043
#include <Library/HobLib.h>                  // RPPO-SKL-0023
#include <SaDataHob.h>                       // RPPO-SKL-0023


#include "SmramSaveStateMsr.h"

////////
// Below definition is from IA32 SDM
////////
#define EFI_MSR_IA32_MTRR_CAP                  0xFE
#define  IA32_MTRR_SMRR_SUPPORT_BIT            BIT11
#define  IA32_MTRR_EMRR_SUPPORT_BIT            BIT12
#define  IA32_MTRR_SMRR2_SUPPORT_BIT           BIT13

#define EFI_MSR_IA32_FEATURE_CONTROL           0x3A
#define  IA32_SMRR_ENABLE_BIT                  BIT3

#define  EFI_MSR_SMRR_PHYS_MASK_VALID          BIT11
#define  EFI_MSR_SMRR_MASK                     0xFFFFF000
#define  EFI_MSR_EMRR_PHYS_MASK_LOCK           BIT10
#define  EFI_MSR_EMRR_MASK                     0xFFFFF000

#define EFI_MSR_NEHALEM_SMRR_PHYS_BASE         0x1F2
#define EFI_MSR_NEHALEM_SMRR_PHYS_MASK         0x1F3

#define   CPUID1_EDX_XD_SUPPORT      0x100000

////////
// Below definition is from CPU BWG
////////
#define EFI_MSR_NEHALEM_EMRR_PHYS_BASE         0x1F4
#define EFI_MSR_NEHALEM_EMRR_PHYS_MASK         0x1F5

#define EFI_MSR_HASWELL_SMRR2_PHYS_BASE        0x1F6
#define EFI_MSR_HASWELL_SMRR2_PHYS_MASK        0x1F7

#define EFI_MSR_HASWELL_UNCORE_EMRR_BASE       0x2F4

#define EFI_MSR_HASWELL_UNCORE_EMRR_MASK       0x2F5
#define  MSR_UNCORE_EMRR_MASK_LOCK_BIT         BIT10
#define  MSR_UNCORE_EMRR_MASK_RANGE_ENABLE_BIT BIT11

#define EFI_MSR_HASWELL_SMM_MCA_CAP            0x17D
#define  SMM_PROT_MODE_BASE_BIT                BIT54
#define  TARGETED_SMI_BIT                      BIT56
#define  SMM_CPU_SVRSTR_BIT                    BIT57
#define  SMM_CODE_ACCESS_CHK_BIT               BIT58
#define  LONG_FLOW_INDICATION_BIT              BIT59

#define EFI_MSR_HASWELL_SMM_FEATURE_CONTROL    0x4E0
#define  SMM_FEATURE_CONTROL_LOCK_BIT          BIT0
#define  SMM_CPU_SAVE_EN_BIT                   BIT1
#define  SMM_CODE_CHK_EN_BIT                   BIT2

#define EFI_MSR_HASWELL_EVENT_CTL_HLT_IO       0xC1F
#define EFI_MSR_HASWELL_SMBASE                 0xC20
#define EFI_MSR_HASWELL_SMMREVID               0xC21
#define EFI_MSR_HASWELL_RIP                    0xC04
#define EFI_MSR_HASWELL_EFER                   0xC03

#define SMM_HASWELL_CLIENT_LOG_PROC_EN_BIT_LENGTH     12
#define SMM_HASWELL_SERVER_LOG_PROC_EN_BIT_LENGTH     64

#define EFI_MSR_HASWELL_SMM_ENABLE             0x4E1

#define EFI_MSR_HASWELL_SMM_DELAYED            0x4E2

#define EFI_MSR_HASWELL_SMM_BLOCKED            0x4E3

#define EFI_MSR_HASWELL_IEDBASE                0xC22

//#define EFI_MSR_NEHALEM_PCIEXBAR               0x300

#define EFI_MSR_SMM_SAVE_CONTROL               0x3e
#define SAVE_FLOATING_POINT_POINTERS           BIT0

#define EFI_MSR_THREAD_ID_INFO                 0x53

#define EFI_MSR_SMM_PROT_MODE_BASE             0x9D
#define  SMM_PROT_MODE_BASE_ENABLE_BIT         BIT0
#define  SMM_PROT_MODE_BASE_PADDR_MASK         0x00000000FFFFFF00ULL

#define PLATFORM_INFO_SMM_SAVE_CONTROL         BIT16

#define SMM_NEHALEM_IEDBASE_OFFSET             0xFEEC
  //
  // RPPO-SKL-0020: RoyalParkOverrideBegin
  //
#define MSR_IA32_RTIT_CTL                      0x00000570
#define B_RTIT_CTL_TRACE_ENABLE                BIT0
#define B_RTIT_CAPABLE                         BIT25
  //
  // RPPO-SKL-0020: RoyalParkOverrideEnd
  //
#define IED_STRING                             "INTEL RSVD"


#define EFI_MSR_CPU_BUS_NO                     0x300
#define HASWELL_NON_STICKY_SCRATCHPAD3(Bus)    PCI_LIB_ADDRESS (Bus, 16, 7, 0x6C)
#define SKYLAKE_NON_STICKY_SCRATCHPAD3(Bus)    PCI_LIB_ADDRESS (Bus, 8, 2, 0xE8)

//
// Server register defines
//
#define HASWELL_SMM_DELAYED0_REG(Bus)          PCI_LIB_ADDRESS (Bus, 16, 6, 0x40)
#define HASWELL_SMM_DELAYED1_REG(Bus)          PCI_LIB_ADDRESS (Bus, 16, 6, 0x44)
#define HASWELL_SMM_BLOCKED0_REG(Bus)          PCI_LIB_ADDRESS (Bus, 16, 6, 0x48)
#define HASWELL_SMM_BLOCKED1_REG(Bus)          PCI_LIB_ADDRESS (Bus, 16, 6, 0x4C)
#define HASWELL_SMM_ENABLE0_REG(Bus)           PCI_LIB_ADDRESS (Bus, 16, 6, 0x50)
#define HASWELL_SMM_ENABLE1_REG(Bus)           PCI_LIB_ADDRESS (Bus, 16, 6, 0x54)
#define HASWELL_SMM_FEATURE_CONTROL_REG(Bus)   PCI_LIB_ADDRESS (Bus, 16, 6, 0x58)

#define SKYLAKE_SMM_DELAYED0_REG(Bus)          PCI_LIB_ADDRESS (Bus, 8, 1, 0x40)
#define SKYLAKE_SMM_DELAYED1_REG(Bus)          PCI_LIB_ADDRESS (Bus, 8, 1, 0x44)
#define SKYLAKE_SMM_BLOCKED0_REG(Bus)          PCI_LIB_ADDRESS (Bus, 8, 1, 0x48)
#define SKYLAKE_SMM_BLOCKED1_REG(Bus)          PCI_LIB_ADDRESS (Bus, 8, 1, 0x4C)
#define SKYLAKE_SMM_ENABLE0_REG(Bus)           PCI_LIB_ADDRESS (Bus, 8, 1, 0x50)
#define SKYLAKE_SMM_ENABLE1_REG(Bus)           PCI_LIB_ADDRESS (Bus, 8, 1, 0x54)
#define SKYLAKE_SMM_FEATURE_CONTROL_REG(Bus)   PCI_LIB_ADDRESS (Bus, 8, 1, 0x7C)

//
//
//
#define INVALID_APIC_ID 0xFFFFFFFFFFFFFFFFULL

//
// Define for Protected Mode SMM Entry Template
//
#define SMMSEG_PROTECT_MODE_CODE_SEGMENT   0x10

////////
// Below section is definition for CPU SMM Feature context
////////

//
// Structure to describe CPU identification mapping
// if ((CPUID_EAX(1) & Mask) == (Signature & Mask)), it means matched.
//
typedef struct {
  UINT32  Signature;
  UINT32  Mask;
} CPUID_MAPPING;

//
// CPU SMM family
//
typedef enum {
  CpuHaswell,
  CpuSkylake,
  CpuQuark,
  CpuSmmFamilyMax
} CPU_SMM_FAMILY;

//
// Structure to describe CPU SMM class
//
typedef struct {
  CPU_SMM_FAMILY    Family;
  UINT32            MappingCount;
  CPUID_MAPPING     *MappingTable;
} CPU_SMM_CLASS;

//
// Structure to describe CPU_SMM_FEATURE_CONTEXT
//
typedef struct {
  BOOLEAN          SmrrEnabled;
  BOOLEAN          EmrrSupported;
  BOOLEAN          Smrr2Enabled;
} CPU_SMM_FEATURE_CONTEXT;

//
// SMMSEG_FEATURE_ENABLES bitmap
//
#define SMMSEG_FEATURE_ENABLE                  BIT0
#define SMMSEG_FEATURE_CR4_MCE_CTL_ENABLE      BIT1

//
// Structure to describe CPU SMM Protected Mode
//
typedef struct {
  UINT32  Reserved1;
  UINT32  SmmSegFeatureEnables;   // SMM_SEG_FEATURE_ENABLES
  UINT32  GDTRLimit;
  UINT32  GDTRBaseOffset;
  UINT32  CSSelector;
  UINT32  Reserved3;
  UINT32  ESPOffset;
  UINT32  Reserved4;
  UINT32  IDTRLimit;
  UINT32  IDTRBaseOffset;
} SMMSEG;

#define  SMM_PROT_MODE_GDT_ENTRY_COUNT    27

//
// SMM PROT MODE CONTEXT (total 0x100 bytes)
//
typedef struct {
  SMMSEG                    SmmProtectedModeSMMSEG;                       // 40 bytes
  IA32_SEGMENT_DESCRIPTOR   SmmProtectedModeGdt[SMM_PROT_MODE_GDT_ENTRY_COUNT];  // 27 * 8 = 216 bytes
} SMM_PROT_MODE_CONTEXT;

//
// Quark CPUID signatures
//
#define CPUID_SIGNATURE_QUARK                   0x00000590

//
// Haswell CPUID signatures
//
#define CPUID_SIGNATURE_HASWELL_CLIENT          0x000306C0
#define CPUID_SIGNATURE_HASWELL_SERVER          0x000306F0
#define CPUID_SIGNATURE_HASWELL_ULT_CLIENT      0x00040650   // RPPO-SKL-0017

//
// Broadwell CPUID signatures
//
#define CPUID_SIGNATURE_BROADWELL_ULT_CLIENT    0x000306D0
#define CPUID_SIGNATURE_BROADWELL_SERVER        0x000406F0
#define CPUID_SIGNATURE_BROADWELL_DE_SERVER     0x00050660

//
// Skylake CPUID signatures
//
#define CPUID_SIGNATURE_SKYLAKE_CLIENT          0x000406E0
#define CPUID_SIGNATURE_SKYLAKE_DT_CLIENT       0x000506E0  // RPPO-SKL-0021
#define CPUID_SIGNATURE_SKYLAKE_SERVER          0x00050650


//
// RPPO-KBL-0047: RoyalParkOverrideBegin
// Kabylake CPUID signatures
//
#define CPUID_SIGNATURE_KABYLAKE_CLIENT          0x000806E0
#define CPUID_SIGNATURE_KABYLAKE_DT_CLIENT       0x000906E0
//
// RPPO-KBL-0047: RoyalParkOverrideEnd
//

//
// CPUID masks
//
#define CPUID_MASK_NO_STEPPING                  0x0FFF0FF0
#define CPUID_MASK_NO_STEPPING_MODEL            0x0FFF0F00

//
// SMM CPU synchronization features available on a processor
//
typedef struct {
  BOOLEAN          TargetedSmiSupported;
  BOOLEAN          DelayIndicationSupported;
  BOOLEAN          BlockIndicationSupported;
  //
  // This processor's LOG_PROC_EN bit used in SMM_ENABLE, SMM_DELAYED, and SMM_BLOCKED MSRs
  // (introduced in Haswell processor).
  // Value of "-1" indicates this field is invalid (i.e. LOG_PROC_EN bit is not
  // supported)
  //
  UINT64           HaswellLogProcEnBit;
} SMM_CPU_SYNC_FEATURE;

///
///
///
extern BOOLEAN    mSaveStateInMsr;
extern UINT8      mSmmFeatureSaveStateRegisterLma;
extern SPIN_LOCK  mMsrSpinLock;
extern BOOLEAN    mSmmProcTraceEnable;    // RPPO-SKL-0020

/**
  Read MSR or CSR based on the CPU type Register to read.

  NOTE: Since platform may uses I/O ports 0xCF8 and 0xCFC to access
        CSR, we need to use SPIN_LOCK to avoid collision on MP System.

  @param[in]  CpuIndex  The processor index.
  @param[in]  RegName   Register name.

  @return 64-bit value read from register.

**/
UINT64
SmmReadReg64 (
  IN  UINTN           CpuIndex,
  IN  SMM_REG_NAME    RegName
  );

/**
  Write MSR or CSR based on the CPU type Register to write.

  NOTE: Since platform may uses I/O ports 0xCF8 and 0xCFC to access
        CSR, we need to use SPIN_LOCK to avoid collision on MP System.

  @param[in]  CpuIndex  The processor index.
  @param[in]  RegName   Register name.
  @param[in]  RegValue  64-bit Register value.

**/
VOID
SmmWriteReg64 (
  IN  UINTN           CpuIndex,
  IN  SMM_REG_NAME    RegName,
  IN  UINT64          RegValue
  );

/**
  Initialize SMM Protected Mode IDT table.
  
  @param[in]  Cr3      CR3 value.
**/
VOID
InitProtectedModeIdt (
  IN UINT32  Cr3
  );

#endif
