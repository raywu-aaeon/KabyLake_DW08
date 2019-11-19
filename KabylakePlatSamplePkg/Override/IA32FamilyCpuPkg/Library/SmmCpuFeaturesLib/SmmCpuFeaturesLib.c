/** @file
  The CPU specific programming for PiSmmCpuDxeSmm module, such as
  SMRR, EMRR, IED.

@copyright
 Copyright (c) 2015 - 2019 Intel Corporation. All rights reserved
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

#include "SmmCpuFeatures.h"

//
// The CPUID mapping for Quark
//
CPUID_MAPPING  mQuarkMap[] = {
  {CPUID_SIGNATURE_QUARK, CPUID_MASK_NO_STEPPING},      // Quark
  };

//
// The CLASS for Quark
//
CPU_SMM_CLASS mQuarkClass = {
  CpuQuark,
  sizeof(mQuarkMap)/sizeof(mQuarkMap[0]),
  mQuarkMap,
  };

//
CPUID_MAPPING  mHaswellMap[] = {
  {CPUID_SIGNATURE_HASWELL_CLIENT, CPUID_MASK_NO_STEPPING},    // Haswell Client
  {CPUID_SIGNATURE_HASWELL_SERVER, CPUID_MASK_NO_STEPPING},    // Haswell Server
  {CPUID_SIGNATURE_HASWELL_ULT_CLIENT, CPUID_MASK_NO_STEPPING},// Haswell Ult Client, // RPPO-SKL-0017
  {CPUID_SIGNATURE_BROADWELL_ULT_CLIENT, CPUID_MASK_NO_STEPPING}, // Broadwell ULT Client
  {CPUID_SIGNATURE_BROADWELL_SERVER, CPUID_MASK_NO_STEPPING},     // Broadwell Server
  {CPUID_SIGNATURE_BROADWELL_DE_SERVER, CPUID_MASK_NO_STEPPING},  // Broadwell DE Server
  };

//
// The CLASS for Haswell
//
CPU_SMM_CLASS mHaswellClass = {
  CpuHaswell,
  sizeof(mHaswellMap)/sizeof(mHaswellMap[0]),
  mHaswellMap,
  };

//
//
// The CPUID mapping for SkyLake
//
CPUID_MAPPING  mSkylakeMap[] = {
  {CPUID_SIGNATURE_SKYLAKE_CLIENT, CPUID_MASK_NO_STEPPING},    // Skylake Client
  {CPUID_SIGNATURE_SKYLAKE_DT_CLIENT, CPUID_MASK_NO_STEPPING}, // Skylake DT Client, RPPO-SKL-0021
  {CPUID_SIGNATURE_SKYLAKE_SERVER, CPUID_MASK_NO_STEPPING},    // Skylake Server
  {CPUID_SIGNATURE_KABYLAKE_CLIENT, CPUID_MASK_NO_STEPPING},   // Kabylake Client , RPPO-KBL-0047
  {CPUID_SIGNATURE_KABYLAKE_DT_CLIENT, CPUID_MASK_NO_STEPPING} // Kabylake DT Client , RPPO-KBL-0047
  };

//
// The CLASS for SkyLake
//
CPU_SMM_CLASS mSkylakeClass = {
  CpuSkylake,
  sizeof(mSkylakeMap)/sizeof(mSkylakeMap[0]),
  mSkylakeMap,
  };

// This table defines supported CPU class
//
CPU_SMM_CLASS *mCpuClasstable[] = {
  &mQuarkClass,
  &mHaswellClass,
  &mSkylakeClass
  };

////////
// Below section is common definition
////////

//
// Assumes UP, or MP with identical feature set
//
CPU_SMM_FEATURE_CONTEXT  mFeatureContext;
CPU_SMM_CLASS            *mThisCpu;
BOOLEAN                  mSmmMsrSaveStateEnable = FALSE;
BOOLEAN                  mSaveStateInMsr = FALSE;
UINT32                   mSmrr2Base;
UINT32                   mSmrr2Size;
UINT8                    mSmrr2CacheType;
BOOLEAN                  mSmmUseDelayIndication;
BOOLEAN                  mSmmUseBlockIndication;
BOOLEAN                  mSmmEnableIndication;
SPIN_LOCK                mCsrAccessLock;
BOOLEAN                  mIEDEnabled;
BOOLEAN                  mIEDBaseInitialized = FALSE;
UINT32                   mIEDBase;
UINT32                   mIEDRamSize;
BOOLEAN                  mSmmProcTraceEnable;   // RPPO-SKL-0020
UINT8                    *mNonIioBusNum;
UINT64                   *mTstateMsr;
UINT32                   *mSmBase;
SMM_CPU_SYNC_FEATURE     *mSmmSyncFeature;
//
// Protected Mode Entrypoint
//
BOOLEAN                  mSmmProtectedModeEnable;
SMM_PROT_MODE_CONTEXT    *mSmmProtModeContext;

//
// Set default value to assume MSR_SMM_FEATURE_CONTROL is not supported
//
BOOLEAN                  mSmmFeatureControlSupported;

extern IA32_DESCRIPTOR  gSmmFeatureSmiHandlerIdtr;

//
// Variables from Protected Mode SMI Entry Template
//
extern volatile UINT32                     gSmmFeatureSmiStack;
extern UINT32                              gSmmFeatureSmbase;
extern UINT32                              gSmmUsableStackSize;
extern UINT32                              gSmmFeatureSmiCr3;
extern BOOLEAN                             gSmmFeatureXdSupported;
extern UINT32                              gProtModeSmbase;
extern volatile UINT8                      gcSmmFeatureSmiHandlerTemplate[];
extern CONST UINT16                        gcSmmFeatureSmiHandlerSize;
extern volatile UINT8                      gcSmiPMHandlerTemplate[];
extern CONST UINT16                        gcSmiPMHandlerSize;
extern UINT32                              gProtModeIdtr;
extern UINT32                              gPMStackDesc[2];

/**
  This function will return current CPU_SMM_CLASS accroding to CPUID mapping.

  @return The point to current CPU_SMM_CLASS

**/
CPU_SMM_CLASS *
GetCpuFamily (
  VOID
  )
{
  UINT32         ClassIndex;
  UINT32         Index;
  UINT32         Count;
  CPUID_MAPPING  *CpuMapping;
  UINT32         RegEax;

  AsmCpuid (CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  for (ClassIndex = 0; ClassIndex < sizeof(mCpuClasstable)/sizeof(mCpuClasstable[0]); ClassIndex++) {
    CpuMapping = mCpuClasstable[ClassIndex]->MappingTable;
    Count = mCpuClasstable[ClassIndex]->MappingCount;
    for (Index = 0; Index < Count; Index++) {
      if ((CpuMapping[Index].Signature & CpuMapping[Index].Mask) == (RegEax & CpuMapping[Index].Mask)) {
        return mCpuClasstable[ClassIndex];
      }
    }
  }
  return NULL;
}

/**
  The constructor function

  @param[in]  ImageHandle  The firmware allocated handle for the EFI image.
  @param[in]  SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS      The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmCpuFeaturesLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT32  RegEax;
  UINT32  RegEdx;
  UINTN   FamilyId;
  UINTN   ModelId;
  SA_DATA_HOB  *SaDataHob; // RPPO-SKL-0023

  //
  // Retrieve CPU Family and Model
  //
  AsmCpuid(CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  DEBUG ((EFI_D_INFO, "CPUID - 0x%08x\n", RegEax));
  FamilyId = (RegEax >> 8) & 0xf;
  ModelId  = (RegEax >> 4) & 0xf;
  if (FamilyId == 0x06 || FamilyId == 0x0f) {
    ModelId = ModelId | ((RegEax >> 12) & 0xf0);
  }
  DEBUG ((EFI_D_INFO, "FamilyId - 0x%08x\n", FamilyId));
  DEBUG ((EFI_D_INFO, "ModelId - 0x%08x\n", ModelId));

  RegEdx = 0;
  AsmCpuid (CPUID_EXTENDED_FUNCTION, &RegEax, NULL, NULL, NULL);
  if (RegEax >= CPUID_EXTENDED_CPU_SIG) {
    AsmCpuid (CPUID_EXTENDED_CPU_SIG, NULL, NULL, NULL, &RegEdx);
  }
  //
  // Determine the mode of the CPU at the time an SMI occurs
  //   Intel(R) 64 and IA-32 Architectures Software Developer's Manual
  //   Volume 3C, Section 34.4.1.1
  //
  mSmmFeatureSaveStateRegisterLma = EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT;
  if ((RegEdx & BIT29) != 0) {
    mSmmFeatureSaveStateRegisterLma = EFI_SMM_SAVE_STATE_REGISTER_LMA_64BIT;
  }
  if (FamilyId == 0x06) {
    if (ModelId == 0x17 || ModelId == 0x0f || ModelId == 0x1c) {
      mSmmFeatureSaveStateRegisterLma = EFI_SMM_SAVE_STATE_REGISTER_LMA_64BIT;
    }
  }

  gSmmFeatureXdSupported = FALSE;
  if ((RegEdx & CPUID1_EDX_XD_SUPPORT) != 0) {
    gSmmFeatureXdSupported = TRUE;
  }
  //
  // Initialize spin lock for MSR access
  //
  InitializeSpinLock (&mMsrSpinLock);

  //
  // Use CPUID to determine CPU Family
  //
  mThisCpu = GetCpuFamily ();
  ASSERT (mThisCpu != NULL);

  //
  // Initialize spin lock for CSR access
  //
  InitializeSpinLock(&mCsrAccessLock);

  mNonIioBusNum = (UINT8 *)AllocatePool(sizeof(UINT8) * PcdGet32(PcdCpuMaxLogicalProcessorNumber));
  ASSERT(mNonIioBusNum != NULL);

  mTstateMsr = (UINT64 *)AllocatePool(sizeof(UINT64) * PcdGet32(PcdCpuMaxLogicalProcessorNumber));
  ASSERT(mTstateMsr != NULL);

  mSmBase = (UINT32 *)AllocatePool(sizeof(UINT32) * PcdGet32 (PcdCpuMaxLogicalProcessorNumber));
  ASSERT(mSmBase != NULL);

  mSmmSyncFeature = (SMM_CPU_SYNC_FEATURE *)AllocatePool (sizeof (SMM_CPU_SYNC_FEATURE) * PcdGet32 (PcdCpuMaxLogicalProcessorNumber));
  ASSERT (mSmmSyncFeature != NULL);

  //
  // Related PCD values are retrieved into global variables.
  //
  mSmmMsrSaveStateEnable  = PcdGetBool (PcdCpuSmmMsrSaveStateEnable);
  mSmmProtectedModeEnable = PcdGetBool (PcdCpuSmmProtectedModeEnable);
  
  mSmmEnableIndication    = PcdGetBool (PcdCpuSmmUseSmmEnableIndication);
  mSmmUseDelayIndication  = PcdGetBool (PcdCpuSmmUseDelayIndication);
  mSmmUseBlockIndication  = PcdGetBool (PcdCpuSmmUseBlockIndication);
  mSmmProcTraceEnable     = PcdGetBool (PcdCpuSmmProcTraceEnable);   // RPPO-SKL-0020

  //
  // Dump some SMM feature PCD values
  //
  DEBUG ((EFI_D_INFO, "PcdCpuSmmMsrSaveStateEnable     = %d\n", mSmmMsrSaveStateEnable));
  DEBUG ((EFI_D_INFO, "PcdCpuSmmProtectedModeEnable    = %d\n", mSmmProtectedModeEnable));
  //
  // RPPO-KBL-0078: RoyalParkOverrideBegin
  //  
  DEBUG ((EFI_D_INFO, "PcdCpuSmmUseSmmEnableIndication = %d\n", mSmmEnableIndication));
  DEBUG ((EFI_D_INFO, "PcdCpuSmmUseDelayIndication     = %d\n", mSmmUseDelayIndication));
  DEBUG ((EFI_D_INFO, "PcdCpuSmmUseBlockIndication     = %d\n", mSmmUseBlockIndication));
  //
  // RPPO-KBL-0078: RoyalParkOverrideEnd
  //
  
  if (mSmmProtectedModeEnable) {
    mSmmProtModeContext = AllocatePages (EFI_SIZE_TO_PAGES(sizeof(SMM_PROT_MODE_CONTEXT) * PcdGet32(PcdCpuMaxLogicalProcessorNumber)));
    ASSERT(mSmmProtModeContext != NULL);
  }

  mSmrr2Base = PcdGet32(PcdCpuSmmSmrr2Base);
  mSmrr2Size = PcdGet32(PcdCpuSmmSmrr2Size);
  mSmrr2CacheType = PcdGet8(PcdCpuSmmSmrr2CacheType);

  //
  // Check the validation of SMRR2 base, size and cache type
  //
  ASSERT(mSmrr2Size == GetPowerOfTwo32(mSmrr2Size));
  ASSERT((mSmrr2Size & ~(SIZE_4KB - 1)) == mSmrr2Size);
  ASSERT((mSmrr2Base & ~(mSmrr2Size - 1)) == mSmrr2Base);
  ASSERT((mSmrr2CacheType == MTRR_CACHE_WRITE_PROTECTED) || (mSmrr2CacheType == MTRR_CACHE_WRITE_BACK));
  DEBUG((EFI_D_INFO, "SMRR2 Base: %x Size: %x Cache type: %x\n", mSmrr2Base, mSmrr2Size, mSmrr2CacheType));
  DEBUG_CODE(
    if (mSmrr2Size == 0) {
      DEBUG((EFI_D_INFO, "SMRR2 size is 0, SMRR2 is not enabled.\n"));
    }
  );

  //
  // RPPO-SKL-0023: RoyalParkOverrideBegin
  //
  //
  // Cache IED configuration settings into module global variables.
  //
  SaDataHob = NULL;
  mIEDEnabled = 0;
  mIEDRamSize = 0;
  SaDataHob = (SA_DATA_HOB *)GetFirstGuidHob (&gSaDataHobGuid);
  if (SaDataHob != NULL) {
    if (SaDataHob->IedSize != 0) {
      mIEDEnabled = TRUE;
      mIEDRamSize = SaDataHob->IedSize;
    }
  }
  //
  // RPPO-SKL-0023: RoyalParkOverrideEnd
  //
  DEBUG((EFI_D_INFO, "mIEDEnabled: %x\n", mIEDEnabled));
  DEBUG((EFI_D_INFO, "mIEDRamSize: %x\n", mIEDRamSize));

  return EFI_SUCCESS;
}

/**
  Check if it is Quark processor.

  @retval TRUE  It is Quark.
  @retval FALSE It is not Quark.
**/
BOOLEAN
IsQuark (
  VOID
  )
{
  UINT32 RegEax;
  UINTN  Index;

  AsmCpuid (CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  for (Index = 0; Index < sizeof(mQuarkMap)/sizeof(mQuarkMap[0]); Index++) {
    if ((RegEax & mQuarkMap[Index].Mask) == (mQuarkMap[Index].Signature & mQuarkMap[Index].Mask)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Return if SMRR is supported

  @retval TRUE  SMRR is supported.
  @retval FALSE SMRR is not supported.

**/
BOOLEAN
IsSmrrSupported (
  VOID
  )
{
  UINT64                            MtrrCap;
  BOOLEAN                           ReturnValue;

  //
  // Check for Quark SoC
  //
  if (IsQuark()) {
    //
    // CARP: Turn off for now since SMRR MSRs are not really MSRs and SMRR setting
    //       in Core2InitSmrr() for uses MSRs.
    //
    ReturnValue = FALSE;
  } else {
    //
    // Not Quark
    //
    MtrrCap = AsmReadMsr64(EFI_MSR_IA32_MTRR_CAP);
    if ((MtrrCap & IA32_MTRR_SMRR_SUPPORT_BIT) == 0) {
      ReturnValue = FALSE;
    } else {
      ReturnValue = TRUE;
    }
  }
  return ReturnValue;
}

/**
  Return if SMRR2 is supported

  @retval TRUE  SMRR2 is supported.
  @retval FALSE SMRR2 is not supported.

**/
BOOLEAN
IsSmrr2Supported (
  VOID
  )
{
  UINT64                            MtrrCap;
  BOOLEAN                           ReturnValue;

  //
  // Check for Quark SoC
  //
  if (IsQuark()) {
    ReturnValue = FALSE;
  } else {
    //
    // Not Quark
    //
    MtrrCap = AsmReadMsr64(EFI_MSR_IA32_MTRR_CAP);
    if ((MtrrCap & IA32_MTRR_SMRR2_SUPPORT_BIT) == 0) {
      ReturnValue = FALSE;
    } else {
      ReturnValue = TRUE;
   }
  }
  return ReturnValue;
}

/**
  Return if EMRR is supported

  @retval TRUE  EMRR is supported.
  @retval FALSE EMRR is not supported.

**/
BOOLEAN
IsEmrrSupported (
  VOID
  )
{
  UINT64                            MtrrCap;
  BOOLEAN                           ReturnValue;

  //
  // Check for Quark SoC
  //
  if (IsQuark()) {
    ReturnValue = FALSE;
  } else {
    //
    // Not Quark
    //
    MtrrCap = AsmReadMsr64(EFI_MSR_IA32_MTRR_CAP);
    if ((MtrrCap & IA32_MTRR_EMRR_SUPPORT_BIT) == 0) {
      ReturnValue = FALSE;
    } else {
      ReturnValue = TRUE;
    }
  }
  return ReturnValue;
}

/**
  Initialize SMRR in SMM relocate.

  @param  SmrrBase           The base address of SMRR.
  @param  SmrrSize           The size of SMRR.
**/
VOID
NehalemInitSmrr (
  IN UINT32                SmrrBase,
  IN UINT32                SmrrSize
  )
{
  AsmWriteMsr64 (EFI_MSR_NEHALEM_SMRR_PHYS_BASE, SmrrBase | MTRR_CACHE_WRITE_BACK);
  AsmWriteMsr64 (EFI_MSR_NEHALEM_SMRR_PHYS_MASK, (~(SmrrSize - 1) & EFI_MSR_SMRR_MASK)); // Valid bit will be set in ConfigSmrr() at first SMI
}

/**
  Configure SMRR register at each SMM entry.
**/
VOID
NehalemConfigSmrr (
  VOID
  )
{
  UINT64 SmrrMask;

  SmrrMask = AsmReadMsr64 (EFI_MSR_NEHALEM_SMRR_PHYS_MASK);
  if ((SmrrMask & EFI_MSR_SMRR_PHYS_MASK_VALID) == 0) {
    AsmWriteMsr64(EFI_MSR_NEHALEM_SMRR_PHYS_MASK, SmrrMask | EFI_MSR_SMRR_PHYS_MASK_VALID);
  }
}

/**
  Check if it is Haswell-EX processor.

  @retval TRUE  It is Haswell-EX.
  @retval FALSE It is not Haswell-EX.
**/
BOOLEAN
IsHaswellServer (
  VOID
  )
{
  UINT32 RegEax;

  AsmCpuid (CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  if (((RegEax & CPUID_MASK_NO_STEPPING) == CPUID_SIGNATURE_HASWELL_SERVER)   ||
      ((RegEax & CPUID_MASK_NO_STEPPING) == CPUID_SIGNATURE_BROADWELL_SERVER) ||
      ((RegEax & CPUID_MASK_NO_STEPPING) == CPUID_SIGNATURE_BROADWELL_DE_SERVER)) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check if it is Skylake server processor.

  @retval TRUE  It is Skylake server
  @retval FALSE It is not Skylake server.
**/
BOOLEAN
IsSkylakeServer (
  VOID
  )
{
  UINT32 RegEax;

  AsmCpuid (CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  if ((RegEax & CPUID_MASK_NO_STEPPING) == CPUID_SIGNATURE_SKYLAKE_SERVER) {
    return TRUE;
  }
  return FALSE;
}


/**
  Programming IED.

  @param  ProcessorNumber       The processor number.
  @param  IedBase               The base address of IED. -1 when IED is disabled.
  @param  IsMonarch             If this processor treated as monarch.
  @param  SmmInitSaveStateInMsr TRUE indicates SMM save state in MSR is supported.
                                FALSE indicates SMM save state in MSR is not supported.
**/
VOID
HaswellRelocateIedBase (
  IN UINTN                 ProcessorNumber,
  IN UINT32                IedBase,
  IN BOOLEAN               IsMonarch,
  IN BOOLEAN               SmmInitSaveStateInMsr
  )
{
  UINT32 StrSize;
  UINT32 *IedOffset;
  UINT32 ScratchPad;
  UINT64 Bus;

  ScratchPad = 0;
  //
  // IED must be enabled for Haswell processor.
  // A minimum of 4MB IDERAM is required for Haswell processors.
  //
  ASSERT (mIEDEnabled);
  ASSERT (mIEDRamSize >= 0x400000);

  if (SmmInitSaveStateInMsr) {
    AsmWriteMsr64 (EFI_MSR_HASWELL_IEDBASE, LShiftU64 (IedBase, 32));
  } else {
    IedOffset = (UINT32 *)(UINTN)(SMM_DEFAULT_SMBASE + SMM_NEHALEM_IEDBASE_OFFSET);
    *IedOffset = IedBase;
  }

  if ((IedBase != (UINT32)-1) && IsMonarch) {
    // 48 byte header
    ZeroMem ((UINT8 *)(UINTN)IedBase, 48);
    // signature
    StrSize = sizeof(IED_STRING) - 1;
    CopyMem ((UINT8 *)(UINTN)IedBase, IED_STRING, StrSize);
    // IED size
    *(UINT32 *)(UINTN)(IedBase + 10) = mIEDRamSize;
    // Set IED trace
    if (IsHaswellServer ()) {
      Bus = AsmReadMsr64 (EFI_MSR_CPU_BUS_NO);
      //if MSR has valid Bus#
      if((Bus & BIT31) != 0) {
        Bus = (RShiftU64 (Bus, 8)) & 0xff;
        //get base of IedTrace from BIOSNONSTICKYSCRATCHPAD3
        ScratchPad = PciRead32 (HASWELL_NON_STICKY_SCRATCHPAD3 ((UINT32)Bus));
      }
    } else if (IsSkylakeServer ()) {
       Bus = AsmReadMsr64 (EFI_MSR_CPU_BUS_NO);
      //if MSR has valid Bus#
      if((Bus & BIT63) != 0) {
        Bus &= 0xff;
        //get base of IedTrace from BIOSNONSTICKYSCRATCHPAD3
        ScratchPad = PciRead32 (SKYLAKE_NON_STICKY_SCRATCHPAD3 ((UINT32)Bus));
      }
    }

    if (ScratchPad == 0) {
      *(UINT64 *)(UINTN)(IedBase + 16) = (UINT64)-1;
    } else {
      // IED Trace Ptr = ScrtachPad * 1MB
      *(UINT64 *)(UINTN)(IedBase + 16) = LShiftU64 (ScratchPad, 20);
    }

    // patch
    ZeroMem ((UINT8 *)(UINTN)IedBase + 0x100000, 32 * 0x1024);
  }
}

/**
  Initialize EMRR in SMM relocation for Haswell processors.

  @param  IedBase           IEDRAM base address.

**/
VOID
HaswellInitEmrr (
  IN UINT32          IedBase
  )
{
  UINT32                ApicId;
  UINT32                IntraPackageIdBits;
  UINT64                McSegBase;
  UINT64                McSegMask;
  UINT8                 PhysicalAddressBits;
  UINT64                ValidMtrrAddressMask;
  UINT32                RegEax;

  ApicId = GetInitialApicId ();

  AsmCpuidEx (CPUID_EXTENDED_TOPOLOGY, 1, &IntraPackageIdBits, NULL, NULL, NULL);
  IntraPackageIdBits &= 0x1f;

  AsmCpuid (CPUID_EXTENDED_FUNCTION, &RegEax, NULL, NULL, NULL);
  PhysicalAddressBits = 36;
  if (RegEax >= CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (CPUID_VIR_PHY_ADDRESS_SIZE, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  }
  ValidMtrrAddressMask = (LShiftU64 (1, PhysicalAddressBits) - 1) & 0xfffffffffffff000ull;

  if ((ApicId >> IntraPackageIdBits) != 0) {
    //
    // IEDBASE + 3MB for socket 1
    //
    McSegBase = IedBase + 0x300000;
  } else {
    //
    // IEDBASE + 2MB for socket 0
    //
    McSegBase = IedBase + 0x200000;
  }
  McSegBase &= ValidMtrrAddressMask;

  McSegMask = (UINT64)(~(UINT64)(0x200000 - 1));
  McSegMask &= ValidMtrrAddressMask;

  if ((AsmReadMsr64 (EFI_MSR_HASWELL_UNCORE_EMRR_MASK) & MSR_UNCORE_EMRR_MASK_LOCK_BIT) == 0) {
    AsmWriteMsr64 (EFI_MSR_HASWELL_UNCORE_EMRR_BASE, McSegBase);
    AsmWriteMsr64 (EFI_MSR_HASWELL_UNCORE_EMRR_MASK, McSegMask | MSR_UNCORE_EMRR_MASK_LOCK_BIT | MSR_UNCORE_EMRR_MASK_RANGE_ENABLE_BIT);
  }

  if (!(AsmReadMsr64 (EFI_MSR_NEHALEM_EMRR_PHYS_MASK) & EFI_MSR_EMRR_PHYS_MASK_LOCK)) {
    AsmWriteMsr64 (EFI_MSR_NEHALEM_EMRR_PHYS_BASE, McSegBase | MTRR_CACHE_WRITE_BACK);
    AsmWriteMsr64 (EFI_MSR_NEHALEM_EMRR_PHYS_MASK, McSegMask | EFI_MSR_EMRR_PHYS_MASK_LOCK);
  }
}

/**
  Config MSR Save State feature for Haswell processors.

  @param[in]  ProcessorNumber        The processor number.

**/
VOID
HaswellConfigMsrSaveState (
  IN UINTN        ProcessorNumber
  )
{
  UINT64 SmmFeatureControlMsr;

  if (mSmmMsrSaveStateEnable && mSmmFeatureControlSupported) {
    if ((AsmReadMsr64 (EFI_MSR_HASWELL_SMM_MCA_CAP) & SMM_CPU_SVRSTR_BIT) != 0) {
      SmmFeatureControlMsr = SmmReadReg64 (ProcessorNumber, SmmRegFeatureControl);
      ASSERT ((SmmFeatureControlMsr & SMM_FEATURE_CONTROL_LOCK_BIT) == 0);

      if ((SmmFeatureControlMsr & SMM_CPU_SAVE_EN_BIT) == 0) {
        //
        // SMM_CPU_SAVE_EN_BIT is package scope, so when a thead in a package sets this bit,
        // MSR Save State feature will be enabled for all threads in the package in subsequent SMIs.
        //
        // Lock bit is not set here. It will be set when configuring SMM Code Access Check feature later.
        //
        SmmFeatureControlMsr |= SMM_CPU_SAVE_EN_BIT;
        SmmWriteReg64 (ProcessorNumber, SmmRegFeatureControl, SmmFeatureControlMsr);
        mSaveStateInMsr = TRUE;
      }
    }
  }
}

/**
  Initialize SMRR2 in SMM relocate.

  @param  Smrr2Base          The base address of SMRR2.
  @param  Smrr2Size          The size of SMRR2.
**/
VOID
HaswellInitSmrr2 (
  IN UINT32                Smrr2Base,
  IN UINT32                Smrr2Size
  )
{
  ASSERT (Smrr2Size != 0);

  if ((AsmReadMsr64 (EFI_MSR_HASWELL_SMRR2_PHYS_MASK) & EFI_MSR_SMRR_PHYS_MASK_VALID) == 0) {
    //
    // Program SMRR2 Base and Mask
    //
    AsmWriteMsr64 (EFI_MSR_HASWELL_SMRR2_PHYS_BASE, Smrr2Base | mSmrr2CacheType);
    AsmWriteMsr64 (EFI_MSR_HASWELL_SMRR2_PHYS_MASK, (~(Smrr2Size - 1) & EFI_MSR_SMRR_MASK) | EFI_MSR_SMRR_PHYS_MASK_VALID);
  }
}

//
// RPPO-SKL-0017: RoyalParkOverrideBegin
//
/**
  Configure SMRR2 register at Smm Ready To Lock event notification.
**/
VOID
HaswellConfigSmrr2 (
  VOID
  )
{
  UINT64 Smrr2Mask;

  if (mFeatureContext.Smrr2Enabled){
    Smrr2Mask = AsmReadMsr64 (EFI_MSR_HASWELL_SMRR2_PHYS_MASK);
    if (((Smrr2Mask & EFI_MSR_SMRR_PHYS_MASK_VALID) == 0)) {
      //
      // If SMMR2 is enabled and is not valid, then validate it
      //
      AsmWriteMsr64(EFI_MSR_HASWELL_SMRR2_PHYS_MASK, Smrr2Mask | EFI_MSR_SMRR_PHYS_MASK_VALID);
    }
  }
}
//
// RPPO-SKL-0017: RoyalParkOverrideEnd
//

/**
  Enable the Save Floating Point Pointers feature on every logical processor.
  Refer to SNB/IVB BWG "SMM Handler Considerations".

**/
VOID
EnableSmmSaveControl (
  VOID
)
{
  //
  // MSR_PLATFORM_INFO[16] = 1 indicates the Save Floating Point Pointers feature exists.
  //
  if ((AsmReadMsr64 (MSR_PLATFORM_INFO) & PLATFORM_INFO_SMM_SAVE_CONTROL) != 0) {
    AsmMsrOr64 (EFI_MSR_SMM_SAVE_CONTROL, SAVE_FLOATING_POINT_POINTERS);
  }
}

////////
// Below section is definition for the supported class
////////

////////
// Below section is external function
////////
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
  )
{
  UINT64      RetVal;

  RetVal = 0;
  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    if (IsHaswellServer()) {
      AcquireSpinLock (&mCsrAccessLock);
      switch (RegName) {
      //
      // Server uses CSR
      //
      case SmmRegFeatureControl:
        RetVal = PciRead32 (HASWELL_SMM_FEATURE_CONTROL_REG (mNonIioBusNum[CpuIndex]));
        break;
      case SmmRegSmmEnable:
        RetVal = PciRead32 (HASWELL_SMM_ENABLE1_REG (mNonIioBusNum[CpuIndex]));
        RetVal = LShiftU64 (RetVal, 32) | PciRead32 (HASWELL_SMM_ENABLE0_REG (mNonIioBusNum[CpuIndex]));
        break;
      case SmmRegSmmDelayed:
        RetVal = PciRead32 (HASWELL_SMM_DELAYED1_REG (mNonIioBusNum[CpuIndex]));
        RetVal = LShiftU64 (RetVal, 32) | PciRead32 (HASWELL_SMM_DELAYED0_REG (mNonIioBusNum[CpuIndex]));
        break;
      case SmmRegSmmBlocked:
        RetVal = PciRead32 (HASWELL_SMM_BLOCKED1_REG (mNonIioBusNum[CpuIndex]));
        RetVal = LShiftU64 (RetVal, 32) | PciRead32 (HASWELL_SMM_BLOCKED0_REG (mNonIioBusNum[CpuIndex]));
        break;
      default:
        ASSERT (FALSE);
      }
      ReleaseSpinLock (&mCsrAccessLock);
    } else if (IsSkylakeServer ()) {
      AcquireSpinLock (&mCsrAccessLock);
      switch (RegName) {
      //
      // Server uses CSR
      //
      case SmmRegFeatureControl:
        RetVal = PciRead32 (SKYLAKE_SMM_FEATURE_CONTROL_REG (mNonIioBusNum[CpuIndex]));
        break;
      case SmmRegSmmEnable:
        RetVal = PciRead32 (SKYLAKE_SMM_ENABLE1_REG (mNonIioBusNum[CpuIndex]));
        RetVal = LShiftU64 (RetVal, 32) | PciRead32 (SKYLAKE_SMM_ENABLE0_REG (mNonIioBusNum[CpuIndex]));
        break;
      case SmmRegSmmDelayed:
        RetVal = PciRead32 (SKYLAKE_SMM_DELAYED1_REG (mNonIioBusNum[CpuIndex]));
        RetVal = LShiftU64 (RetVal, 32) | PciRead32 (SKYLAKE_SMM_DELAYED0_REG (mNonIioBusNum[CpuIndex]));
        break;
      case SmmRegSmmBlocked:
        RetVal = PciRead32 (SKYLAKE_SMM_BLOCKED1_REG (mNonIioBusNum[CpuIndex]));
        RetVal = LShiftU64 (RetVal, 32) | PciRead32 (SKYLAKE_SMM_BLOCKED0_REG (mNonIioBusNum[CpuIndex]));
        break;
      default:
        ASSERT (FALSE);
      }
      ReleaseSpinLock (&mCsrAccessLock);
    } else {
      switch (RegName) {
      //
      // Client uses MSR
      //
      case  SmmRegFeatureControl:
        RetVal = AsmReadMsr64 (EFI_MSR_HASWELL_SMM_FEATURE_CONTROL);
        break;
      case  SmmRegSmmEnable:
        RetVal = AsmReadMsr64 (EFI_MSR_HASWELL_SMM_ENABLE);
        break;
      case  SmmRegSmmDelayed:
        RetVal = AsmReadMsr64 (EFI_MSR_HASWELL_SMM_DELAYED);
        break;
      case  SmmRegSmmBlocked:
        RetVal = AsmReadMsr64 (EFI_MSR_HASWELL_SMM_BLOCKED);
        break;
      default:
        ASSERT (FALSE);
      }
    }
    break;
  default:
    break;
  }
  return  RetVal;
}

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
  )
{
  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    if (IsHaswellServer()) {
      AcquireSpinLock (&mCsrAccessLock);
      switch (RegName) {
      //
      // Server uses CSR
      //
      case SmmRegFeatureControl:
        PciWrite32 (HASWELL_SMM_FEATURE_CONTROL_REG (mNonIioBusNum[CpuIndex]), (UINT32)RegValue);
        break;
      case SmmRegSmmEnable:
        PciWrite32 (HASWELL_SMM_ENABLE1_REG (mNonIioBusNum[CpuIndex]), (UINT32) RShiftU64 (RegValue, 32));
        PciWrite32 (HASWELL_SMM_ENABLE0_REG (mNonIioBusNum[CpuIndex]), (UINT32)RegValue);
        break;
      default:
        ASSERT (FALSE);
      }
      ReleaseSpinLock (&mCsrAccessLock);
    } else if (IsSkylakeServer ()) {
      AcquireSpinLock (&mCsrAccessLock);
      switch (RegName) {
      //
      // Server uses CSR
      //
      case SmmRegFeatureControl:
        PciWrite32 (SKYLAKE_SMM_FEATURE_CONTROL_REG (mNonIioBusNum[CpuIndex]), (UINT32)RegValue);
        break;
      case SmmRegSmmEnable:
        PciWrite32 (SKYLAKE_SMM_ENABLE1_REG (mNonIioBusNum[CpuIndex]), (UINT32) RShiftU64 (RegValue, 32));
        PciWrite32 (SKYLAKE_SMM_ENABLE0_REG (mNonIioBusNum[CpuIndex]), (UINT32)RegValue);
        break;
      default:
        ASSERT (FALSE);
      }
      ReleaseSpinLock (&mCsrAccessLock);
    } else {
      switch (RegName) {
      //
      // Client uses MSR
      //
      case  SmmRegFeatureControl:
        AsmWriteMsr64 (EFI_MSR_HASWELL_SMM_FEATURE_CONTROL, RegValue);
        break;
      case  SmmRegSmmEnable:
        AsmWriteMsr64 (EFI_MSR_HASWELL_SMM_ENABLE, RegValue);
        break;
      default:
        ASSERT (FALSE);
      }
    }
    break;
  default:
    break;
  }
}

/**
  Disable SMRR register if SMRR is supported and SmmCpuFeaturesNeedConfigureMtrrs()
  returns TRUE.
**/
VOID
EFIAPI
SmmCpuFeaturesDisableSmrr (
  VOID
  )
{
}

/**
  Enable SMRR register if SMRR is supported and SmmCpuFeaturesNeedConfigureMtrrs()
  returns TRUE.
**/
VOID
EFIAPI
SmmCpuFeaturesReenableSmrr (
  VOID
  )
{
}

/**
  Determines if MTRR registers must be configured to set SMRAM cache-ability
  when executing in System Management Mode.

  @retval TRUE   MTRR registers must be configured to set SMRAM cache-ability.
  @retval FALSE  MTRR registers do not need to be configured to set SMRAM
                 cache-ability.
**/
BOOLEAN
EFIAPI
SmmCpuFeaturesNeedConfigureMtrrs (
  VOID
  )
{
  ASSERT (mThisCpu != NULL);// RPPO-SKL-0022
  if (mThisCpu == NULL) {   // RPPO-SKL-0022
    return FALSE;           // RPPO-SKL-0022
  }                         // RPPO-SKL-0022

  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    return FALSE;
  default:
    return TRUE;
  }
}

/**
  This function sets DR6 & DR7 according to SMM save state, before running SMM C code.
  They are useful when you want to enable hardware breakpoints in SMM without entry SMM mode.

  NOTE: It might not be appreciated in runtime since it might
        conflict with OS debugging facilities. Turn them off in RELEASE.

  @param    CpuIndex              CPU Index

**/
VOID
EFIAPI
SmmFeatureCpuSmmDebugEntry (
  IN UINTN  CpuIndex
  )
{
  SMRAM_SAVE_STATE_MAP *CpuSaveState;

  if (FeaturePcdGet (PcdCpuSmmDebug)) {
    if (mSaveStateInMsr) {
      AsmWriteDr6 ((UINTN)AsmReadMsr64 (MSR_DR6));
      AsmWriteDr7 ((UINTN)AsmReadMsr64 (MSR_DR7));
    } else {
      CpuSaveState = (SMRAM_SAVE_STATE_MAP *)(UINTN)(mSmBase[CpuIndex] + SMRAM_SAVE_STATE_MAP_OFFSET);
      if (mSmmFeatureSaveStateRegisterLma == EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT) {
        AsmWriteDr6 (CpuSaveState->x86._DR6);
        AsmWriteDr7 (CpuSaveState->x86._DR7);
      } else {
        AsmWriteDr6 ((UINTN)CpuSaveState->x64._DR6);
        AsmWriteDr7 ((UINTN)CpuSaveState->x64._DR7);
      }
    }
  }
}

/**
  This function restores DR6 & DR7 to SMM save state.

  NOTE: It might not be appreciated in runtime since it might
        conflict with OS debugging facilities. Turn them off in RELEASE.

  @param    CpuIndex              CPU Index

**/
VOID
EFIAPI
SmmFeatureCpuSmmDebugExit (
  IN UINTN  CpuIndex
  )
{
  SMRAM_SAVE_STATE_MAP *CpuSaveState;

  if (FeaturePcdGet (PcdCpuSmmDebug)) {
    if (mSaveStateInMsr) {
      // DR6/DR7 MSR is RO
    } else {
      CpuSaveState = (SMRAM_SAVE_STATE_MAP *)(UINTN)(mSmBase[CpuIndex] + SMRAM_SAVE_STATE_MAP_OFFSET);
      if (mSmmFeatureSaveStateRegisterLma == EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT) {
        CpuSaveState->x86._DR7 = (UINT32)AsmReadDr7 ();
        CpuSaveState->x86._DR6 = (UINT32)AsmReadDr6 ();
      } else {
        CpuSaveState->x64._DR7 = AsmReadDr7 ();
        CpuSaveState->x64._DR6 = AsmReadDr6 ();
      }
    }
  }
}

/**
  Processor specific hook point each time a CPU enters System Management Mode.

  @param[in] CpuIndex  The index of the CPU that has entered SMM.  The value
                       must be between 0 and the NumberOfCpus field in the
                       System Management System Table (SMST).
**/
VOID
EFIAPI
SmmCpuFeaturesRendezvousEntry (
  IN UINTN  CpuIndex
  )
{
  SMRAM_SAVE_STATE_MAP         *CpuSaveState;
  UINT64                       EventCtlHaltIoMsr;
  UINT64                       MsrValue;    // RPPO-SKL-0020

  ASSERT (mThisCpu != NULL);// RPPO-SKL-0022
  if (mThisCpu == NULL) {   // RPPO-SKL-0022
    return;                 // RPPO-SKL-0022
  }                         // RPPO-SKL-0022

  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    //
    // Configure SMRR/EMRR register at each SMM entry.
    //
    if (mFeatureContext.SmrrEnabled) {
      NehalemConfigSmrr ();
    }
    if (mFeatureContext.Smrr2Enabled) {
      //
      // Note that SMRR2 registers can't be written in the first SMI because
      // MSR Save State feature is not enabled.
      //
      HaswellInitSmrr2 (mSmrr2Base, mSmrr2Size);
    }

    if (FeaturePcdGet (PcdFrameworkCompatibilitySupport) && mSaveStateInMsr) {
      //
      // Copy SMBASE, SMMRevId, IORestart, AutoHALTRestart values in MSR to CPU Save State in memory
      // to co-work with the SMM thunk driver.
      //
      CpuSaveState = (SMRAM_SAVE_STATE_MAP *)(UINTN)(mSmBase[CpuIndex] + SMRAM_SAVE_STATE_MAP_OFFSET);
      CpuSaveState->x86.SMBASE = (UINT32)mSmBase[CpuIndex];
      CpuSaveState->x86.SMMRevId = (UINT32)AsmReadMsr64 (EFI_MSR_HASWELL_SMMREVID);
      EventCtlHaltIoMsr = AsmReadMsr64 (EFI_MSR_HASWELL_EVENT_CTL_HLT_IO);
      CpuSaveState->x86.IORestart = (UINT16)EventCtlHaltIoMsr;
      CpuSaveState->x86.AutoHALTRestart = (UINT16)RShiftU64 (EventCtlHaltIoMsr, 16);
    }
  //
  // RPPO-SKL-0020: RoyalParkOverrideBegin
  //
    //
    // Configure ProcTrace feature at SMM entry.
    //
    if (mSmmProcTraceEnable) {
      MsrValue = AsmReadMsr64 (MSR_IA32_RTIT_CTL);
      MsrValue |= B_RTIT_CTL_TRACE_ENABLE;
      AsmWriteMsr64 (MSR_IA32_RTIT_CTL, MsrValue);
    }
  //
  // RPPO-SKL-0020: RoyalParkOverrideEnd
  //
    return ;
  default:
    return ;
  }
}

/**
  Processor specific hook point each time a CPU exits System Management Mode.

  @param[in] CpuIndex  The index of the CPU that is exiting SMM.  The value must
                       be between 0 and the NumberOfCpus field in the System
                       Management System Table (SMST).
**/
VOID
EFIAPI
SmmCpuFeaturesRendezvousExit (
  IN UINTN  CpuIndex
  )
{
  SMRAM_SAVE_STATE_MAP         *CpuSaveState;
  UINT64                       EventCtlHaltIoMsr;
  UINT64                       MsrValue;    // RPPO-SKL-0020

  ASSERT (mThisCpu != NULL);// RPPO-SKL-0022
  if (mThisCpu == NULL) {   // RPPO-SKL-0022
    return;                 // RPPO-SKL-0022
  }                         // RPPO-SKL-0022

  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    if (FeaturePcdGet (PcdFrameworkCompatibilitySupport) && mSaveStateInMsr) {
      //
      // Copy SMBASE, IORestart, AutoHALTRestart values in CPU Save State in memory to MSR
      // to co-work with the SMM thunk driver.
      //
      CpuSaveState = (SMRAM_SAVE_STATE_MAP *)(UINTN)(mSmBase[CpuIndex] + SMRAM_SAVE_STATE_MAP_OFFSET);
      if (CpuSaveState->x86.SMBASE != (UINT32)mSmBase[CpuIndex]) {
        //
        // Note that if SMBASE is changed, the SMI management is out of control of this SMM CPU driver.
        //
        AsmWriteMsr64 (EFI_MSR_HASWELL_SMBASE, CpuSaveState->x86.SMBASE);
      }
      EventCtlHaltIoMsr = AsmReadMsr64 (EFI_MSR_HASWELL_EVENT_CTL_HLT_IO);
      BitFieldWrite64 (EventCtlHaltIoMsr, 0, 15, CpuSaveState->x86.IORestart);
      BitFieldWrite64 (EventCtlHaltIoMsr, 16, 31, CpuSaveState->x86.AutoHALTRestart );
      AsmWriteMsr64 (EFI_MSR_HASWELL_EVENT_CTL_HLT_IO, EventCtlHaltIoMsr);
    }
  //
  // RPPO-SKL-0020: RoyalParkOverrideBegin
  //
    //
    // Configure ProcTrace feature at SMM Exit.
    //
    if (mSmmProcTraceEnable) {
      MsrValue = AsmReadMsr64 (MSR_IA32_RTIT_CTL);
      MsrValue &= ~((UINT64)B_RTIT_CTL_TRACE_ENABLE);
      AsmWriteMsr64 (MSR_IA32_RTIT_CTL, MsrValue);
    }
  //
  // RPPO-SKL-0020: RoyalParkOverrideEnd
  //
    return ;
  default:
    return ;
  }
}

/**
  Hook point in normal execution mode that allows the one CPU that was elected
  as monarch during System Management Mode initialization to perform additional
  initialization actions immediately after all of the CPUs have processed their
  first SMI and called SmmCpuFeaturesInitializeProcessor() relocating SMBASE
  into a buffer in SMRAM and called SmmCpuFeaturesHookReturnFromSmm().
**/
VOID
EFIAPI
SmmCpuFeaturesSmmRelocationComplete (
  VOID
  )
{
  //
  // Dump some SMM feature setting after SMM rellocation
  //
  DEBUG ((EFI_D_INFO, "CpuSmmFeatureControlSupported= %d\n", mSmmFeatureControlSupported));
  DEBUG ((EFI_D_INFO, "CpuSmmMsrSaveStateEnable     = %d\n", mSmmMsrSaveStateEnable));
  DEBUG ((EFI_D_INFO, "CpuSmmProtectedModeEnable    = %d\n", mSmmProtectedModeEnable));
  DEBUG ((EFI_D_INFO, "CpuSmmUseDelayIndication     = %d\n", mSmmUseDelayIndication));
  DEBUG ((EFI_D_INFO, "CpuSmmUseBlockIndication     = %d\n", mSmmUseBlockIndication));
  DEBUG ((EFI_D_INFO, "CpuSmmUseSmmEnableIndication = %d\n", mSmmEnableIndication));

  mThisCpu = GetCpuFamily (); // RPPO-SKL-0022
  ASSERT (mThisCpu != NULL);  // RPPO-SKL-0022
  if (mThisCpu == NULL) {     // RPPO-SKL-0022
    return;                   // RPPO-SKL-0022
  }                           // RPPO-SKL-0022

  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    if (!IsSmrrSupported ()) {
      return ;
    }
    mFeatureContext.SmrrEnabled = TRUE;
    mFeatureContext.EmrrSupported = IsEmrrSupported ();
    if (IsSmrr2Supported () && mSmrr2Size != 0 && mSaveStateInMsr) {
      mFeatureContext.Smrr2Enabled = TRUE;
    } else {
      mFeatureContext.Smrr2Enabled = FALSE;
    }
    return ;
  default:
    return ;
  }
}

/**
  Setup SMM Protected Mode context for processor.

  @param  ProcessorNumber    The processor number.
  @param  SmBase             The SMBASE value of the processor.
  @param  StackAddress       Stack address of the processor.
  @param  GdtBase            Gdt table base address of the processor.
  @param  GdtSize            Gdt table size of the processor.
  @param  CodeSegment        Code segment value.
  @param  ProtModeIdt        Pointer to protected-mode IDT descriptor.
**/
VOID
SetupSmmProtectedModeContext(
  IN UINTN                    ProcessorNumber,
  IN UINT32                   SmBase,
  IN UINT32                   StackAddress,
  IN UINTN                    GdtBase,
  IN UINTN                    GdtSize,
  IN UINT16                   CodeSegment,
  IN IA32_DESCRIPTOR          *ProtModeIdt
  )
{
  SMMSEG                      *ThreadSmmSeg;
  IA32_SEGMENT_DESCRIPTOR     *GdtDescriptor;
  UINTN                       GdtEntryIndex;

  if (!mSmmProtectedModeEnable) {
    return;
  }

  //
  // Fill SMMSEG structure fields
  //
  ThreadSmmSeg = &mSmmProtModeContext[ProcessorNumber].SmmProtectedModeSMMSEG;
  ThreadSmmSeg->GDTRLimit = (UINT32)(GdtSize - 1);
  ThreadSmmSeg->GDTRBaseOffset = (UINT32)GdtBase - SmBase;
  ThreadSmmSeg->CSSelector = CodeSegment;
  ThreadSmmSeg->ESPOffset = StackAddress - SmBase;
  ThreadSmmSeg->IDTRLimit = ProtModeIdt->Limit;
  ThreadSmmSeg->IDTRBaseOffset = (UINT32)ProtModeIdt->Base - SmBase;
  //
  // Patch 32bit CS/SS segment base to SMBASE, SS = CS + 8 decided by hardware
  //
  GdtEntryIndex = ThreadSmmSeg->CSSelector / sizeof(IA32_SEGMENT_DESCRIPTOR);
  GdtDescriptor = (IA32_SEGMENT_DESCRIPTOR *)GdtBase;
  GdtDescriptor[GdtEntryIndex].Bits.BaseLow = (UINT16)0x0000FFFF & SmBase;
  GdtDescriptor[GdtEntryIndex].Bits.BaseMid = (UINT8)0x000000FF & (SmBase >> 16);
  GdtDescriptor[GdtEntryIndex].Bits.BaseHigh = (UINT8)0x000000FF & (SmBase >> 24);
  GdtDescriptor[GdtEntryIndex + 1].Bits.BaseLow = (UINT16)0x0000FFFF & SmBase;
  GdtDescriptor[GdtEntryIndex + 1].Bits.BaseMid = (UINT8)0x000000FF & (SmBase >> 16);
  GdtDescriptor[GdtEntryIndex + 1].Bits.BaseHigh = (UINT8)0x000000FF & (SmBase >> 24);
}

/**
  Enable SMM Protected Mode.

  @param  ProcessorNumber    The processor number.

**/
VOID
EnableSmmProtectedMode (
  IN UINTN   ProcessorNumber
  )
{
  SMMSEG     *ThreadSmmSeg;

  //
  // Check if hardware support SMM PROT MODE feature
  //
  if ((AsmReadMsr64 (EFI_MSR_HASWELL_SMM_MCA_CAP) & SMM_PROT_MODE_BASE_BIT) == 0) {
    mSmmProtectedModeEnable = FALSE;
    FreePages (mSmmProtModeContext, EFI_SIZE_TO_PAGES(sizeof(SMM_PROT_MODE_CONTEXT) * PcdGet32(PcdCpuMaxLogicalProcessorNumber)));
    return ;
  }

  //
  // Enable the SMM PROT MODE feature in the SMMSEG structure
  //
  ThreadSmmSeg = &mSmmProtModeContext[ProcessorNumber].SmmProtectedModeSMMSEG;
  ThreadSmmSeg->SmmSegFeatureEnables = (SMMSEG_FEATURE_ENABLE | SMMSEG_FEATURE_CR4_MCE_CTL_ENABLE);

  //
  // SMMSEG should be 256-byte aligned 32-bit address
  //
  ASSERT (((UINT64)(UINTN)ThreadSmmSeg & ~SMM_PROT_MODE_BASE_PADDR_MASK) == 0);
  AsmWriteMsr64 (EFI_MSR_SMM_PROT_MODE_BASE, ((UINT32)(UINTN)ThreadSmmSeg | SMM_PROT_MODE_BASE_ENABLE_BIT));
}

/**
  Called during the very first SMI into System Management Mode to initialize
  CPU features, including SMBASE, for the currently executing CPU.  Since this
  is the first SMI, the SMRAM Save State Map is at the default address of
  SMM_DEFAULT_SMBASE + SMRAM_SAVE_STATE_MAP_OFFSET.  The currently executing
  CPU is specified by CpuIndex and CpuIndex can be used to access information
  about the currently executing CPU in the ProcessorInfo array and the
  HotPlugCpuData data structure.

  @param[in] CpuIndex        The index of the CPU to initialize.  The value
                             must be between 0 and the NumberOfCpus field in
                             the System Management System Table (SMST).
  @param[in] IsMonarch       TRUE if the CpuIndex is the index of the CPU that
                             was elected as monarch during System Management
                             Mode initialization.
                             FALSE if the CpuIndex is not the index of the CPU
                             that was elected as monarch during System
                             Management Mode initialization.
  @param[in] ProcessorInfo   Pointer to an array of EFI_PROCESSOR_INFORMATION
                             structures.  ProcessorInfo[CpuIndex] contains the
                             information for the currently executing CPU.
  @param[in] CpuHotPlugData  Pointer to the CPU_HOT_PLUG_DATA structure that
                             contains the ApidId and SmBase arrays.
**/
VOID
EFIAPI
SmmCpuFeaturesInitializeProcessor (
  IN UINTN                      CpuIndex,
  IN BOOLEAN                    IsMonarch,
  IN EFI_PROCESSOR_INFORMATION  *ProcessorInfo,
  IN CPU_HOT_PLUG_DATA          *CpuHotPlugData
  )
{
  SMRAM_SAVE_STATE_MAP              *CpuState;
  UINT64                            McaCap;
  UINT16                            LogProcIndexInPackage;
  UINT64                            CpuBusNum;
  UINT32                            RegEdx;     // RPPO-SKL-0020
  UINT32                            ApicId;
  UINT32                            IntraPackageIdBits;
  BOOLEAN                           SmmInitSaveStateInMsr;
  SMM_CPU_SYNC_FEATURE              *SyncFeature;

  //
  // Adjust size of SMRR if IED is enabled.
  //
  if (mIEDEnabled) {
    if (!mIEDBaseInitialized) {
      mIEDBase = CpuHotPlugData->SmrrBase + CpuHotPlugData->SmrrSize;
      DEBUG ((EFI_D_INFO, "IEDRAM Base: 0x%x\n", mIEDBase));
      CpuHotPlugData->SmrrSize += mIEDRamSize;
      DEBUG ((EFI_D_INFO, "SMRR Base: 0x%x, SMRR Size: 0x%x\n", CpuHotPlugData->SmrrBase, CpuHotPlugData->SmrrSize));
      mIEDBaseInitialized = TRUE;
    }
  } else {
    mIEDBase = (UINT32) -1;
  }

  mSmBase[CpuIndex] = (UINT32)CpuHotPlugData->SmBase[CpuIndex];

  SyncFeature = &(mSmmSyncFeature[CpuIndex]);
  SyncFeature->TargetedSmiSupported = FALSE;
  SyncFeature->DelayIndicationSupported = FALSE;
  SyncFeature->BlockIndicationSupported = FALSE;
  SyncFeature->HaswellLogProcEnBit = (UINT64)(INT64)(-1);

  mThisCpu = GetCpuFamily (); // RPPO-SKL-0022
  ASSERT (mThisCpu != NULL);  // RPPO-SKL-0022
  if (mThisCpu == NULL) {     // RPPO-SKL-0022
    return;                   // RPPO-SKL-0022
  }                           // RPPO-SKL-0022

  SmmInitSaveStateInMsr = FALSE;
  McaCap = 0;

  //
  // Configure SMBASE.
  //
  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    if (IsHaswellServer()) {
      //
      // Get the Non-IIO bus number assigned by BIOS
      //
      CpuBusNum = AsmReadMsr64 (EFI_MSR_CPU_BUS_NO);
      ASSERT ((CpuBusNum & BIT31) != 0);
      mNonIioBusNum[CpuIndex] = (UINT8) (RShiftU64 (CpuBusNum, 8) & 0xff);
    } else if (IsSkylakeServer ()) {
      CpuBusNum = AsmReadMsr64 (EFI_MSR_CPU_BUS_NO);
      ASSERT ((CpuBusNum & BIT63) != 0);
      mNonIioBusNum[CpuIndex] = (UINT8) (CpuBusNum & 0xff);
    }

    //
    // Check SMM Code Access Check bit before access SMM Feature Control MSR 
    //
    McaCap = AsmReadMsr64 (EFI_MSR_HASWELL_SMM_MCA_CAP);
    if ((McaCap & SMM_CODE_ACCESS_CHK_BIT) != 0) {
      mSmmFeatureControlSupported = TRUE;
      if ((McaCap & SMM_CPU_SVRSTR_BIT) != 0 &&
          (SmmReadReg64 (CpuIndex, SmmRegFeatureControl) & SMM_CPU_SAVE_EN_BIT) != 0) {
        AsmWriteMsr64 (EFI_MSR_HASWELL_SMBASE, CpuHotPlugData->SmBase[CpuIndex]);
        SmmInitSaveStateInMsr = TRUE;
        break;
      }
    }
    //
    // Fall back to legacy SMBASE setup.
    //
    CpuState = (SMRAM_SAVE_STATE_MAP *)(UINTN)(SMM_DEFAULT_SMBASE + SMRAM_SAVE_STATE_MAP_OFFSET);
    CpuState->x86.SMBASE = (UINT32)CpuHotPlugData->SmBase[CpuIndex];
    break ;
  default:
    return ;
  }

  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    //
    // mIEDBase = -1 means disabled
    //
    if (mIEDBase != (UINT32) -1) {
      HaswellRelocateIedBase (CpuIndex, mIEDBase, IsMonarch, SmmInitSaveStateInMsr);
    }
    if (IsSmrrSupported ()) {
      NehalemInitSmrr (CpuHotPlugData->SmrrBase, CpuHotPlugData->SmrrSize);
    }
    if (IsSmrr2Supported ()) {                                                      // RPPO-SKL-0017
      HaswellInitSmrr2 (mSmrr2Base, mSmrr2Size);                                    // RPPO-SKL-0017
    }                                                                               // RPPO-SKL-0017

    if(mThisCpu->Family != CpuSkylake )  {                                          // RPPO-SKL-0017
      IoWrite32 (0x80, 0xe1e1e1e1);                                                 // RPPO-SKL-0017, RPPO-KBL-0043
      if (IsEmrrSupported ()) {
        HaswellInitEmrr (mIEDBase);
      }
    }                                                                               // RPPO-SKL-0017

    HaswellConfigMsrSaveState (CpuIndex);

    EnableSmmSaveControl ();

    if (mSmmUseDelayIndication || mSmmUseBlockIndication || mSmmEnableIndication) {
      if (IsHaswellServer () || IsSkylakeServer ()) {
        //
        // Bit [7:0] of this MSR, CURRENT_THREAD_ID, holds an unique number of a thread within
        // the package
        //
        LogProcIndexInPackage = (UINT16) AsmMsrBitFieldRead64 (EFI_MSR_THREAD_ID_INFO, 0, 7);
        ASSERT (LogProcIndexInPackage < SMM_HASWELL_SERVER_LOG_PROC_EN_BIT_LENGTH);
      } else {
        //
        // Get local APIC ID of the executing processor for logical processor Bitmask
        //  C1T0 APIC ID = 2, bitmask = 0000 0100b
        //  C2T1 APIC ID = 5, bitmask = 0010 0000b
        //
        ApicId = GetInitialApicId ();
        AsmCpuidEx (CPUID_EXTENDED_TOPOLOGY, 1, &IntraPackageIdBits, NULL, NULL, NULL);
        IntraPackageIdBits &= 0x1f;
        ApicId = ApicId & ((1 << IntraPackageIdBits) - 1);
        LogProcIndexInPackage = (UINT16) ApicId;
      }
      SyncFeature->HaswellLogProcEnBit = LShiftU64 (1ull, LogProcIndexInPackage);
    }
    if (mSmmUseDelayIndication) {
      SyncFeature->DelayIndicationSupported = (BOOLEAN)((McaCap & LONG_FLOW_INDICATION_BIT) != 0);
    }
    if (mSmmUseBlockIndication) {
      SyncFeature->BlockIndicationSupported = TRUE;
    }
    if (mSmmEnableIndication) {
      SyncFeature->TargetedSmiSupported = (BOOLEAN)((McaCap & TARGETED_SMI_BIT) != 0);
    }
  //
  // RPPO-SKL-0020: RoyalParkOverrideBegin
  //
    //
    // Check ProcTrace Capability
    //
    AsmCpuid (EFI_CPUID_FEATURE_FLAG, NULL, NULL, NULL, &RegEdx);
    if (!(RegEdx & B_RTIT_CAPABLE)) {
      mSmmProcTraceEnable = FALSE;
    }
  //
  // RPPO-SKL-0020: RoyalParkOverrideEnd
  //
    if (mSmmProtectedModeEnable) {
      EnableSmmProtectedMode (CpuIndex);
    }
    return ;
  default:
    ASSERT (FALSE);
    return ;
  }
}

/**
  Return the size, in bytes, of a custom SMI Handler in bytes.  If 0 is
  returned, then a custom SMI handler is not provided by this library,
  and the default SMI handler must be used.

  @retval 0    Use the default SMI handler.
  @retval > 0  Use the SMI handler installed by SmmCpuFeaturesInstallSmiHandler()
               The caller is required to allocate enough SMRAM for each CPU to
               support the size of the custom SMI handler.
**/
UINTN
EFIAPI
SmmCpuFeaturesGetSmiHandlerSize (
  VOID
  )
{
  if (mSmmProtectedModeEnable) {
    return gcSmiPMHandlerSize;
  }
  return gcSmmFeatureSmiHandlerSize;
}

/**
  Install a custom SMI handler for the CPU specified by CpuIndex.  This function
  is only called if SmmCpuFeaturesGetSmiHandlerSize() returns a size is greater
  than zero and is called by the CPU that was elected as monarch during System
  Management Mode initialization.

  @param[in] CpuIndex   The index of the CPU to install the custom SMI handler.
                        The value must be between 0 and the NumberOfCpus field
                        in the System Management System Table (SMST).
  @param[in] SmBase     The SMBASE address for the CPU specified by CpuIndex.
  @param[in] SmiStack   The stack to use when an SMI is processed by the
                        the CPU specified by CpuIndex.
  @param[in] StackSize  The size, in bytes, if the stack used when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] GdtBase    The base address of the GDT to use when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] GdtSize    The size, in bytes, of the GDT used when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] IdtBase    The base address of the IDT to use when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] IdtSize    The size, in bytes, of the IDT used when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] Cr3        The base address of the page tables to use when an SMI
                        is processed by the CPU specified by CpuIndex.
**/
VOID
EFIAPI
SmmCpuFeaturesInstallSmiHandler (
  IN UINTN   CpuIndex,
  IN UINT32  SmBase,
  IN VOID    *SmiStack,
  IN UINTN   StackSize,
  IN UINTN   GdtBase,
  IN UINTN   GdtSize,
  IN UINTN   IdtBase,
  IN UINTN   IdtSize,
  IN UINT32  Cr3
  )
{
  if (mSmmProtectedModeEnable) {
    //
    // Initialize protected mode IDT
    //
    InitProtectedModeIdt (Cr3);
  }

  //
  // Initialize values in template before copy
  //
  gSmmFeatureSmiStack   = (UINT32)((UINTN)SmiStack + StackSize - sizeof (UINTN));
  gSmmUsableStackSize   = PcdGet32(PcdCpuSmmStackSize);
  gSmmFeatureSmiCr3     = Cr3;
  gSmmFeatureSmiHandlerIdtr.Base = IdtBase;
  gSmmFeatureSmiHandlerIdtr.Limit = (UINT16)(IdtSize - 1);

  //
  // Set the value at the top of the CPU stack to the CPU Index
  //
  *(UINTN*)(UINTN)gSmmFeatureSmiStack = CpuIndex;
  gPMStackDesc[0] = gSmmFeatureSmiStack;

  //
  // Copy template to CPU specific SMI handler location
  //
  if (mSmmProtectedModeEnable) {
    gProtModeSmbase = SmBase;
    CopyMem (
      (VOID*)(UINTN)(SmBase + SMM_HANDLER_OFFSET),
      (VOID*)gcSmiPMHandlerTemplate,
      gcSmiPMHandlerSize
      );
    //
    // Prepare for the SMMSEG structure
    //
    SetupSmmProtectedModeContext (
      CpuIndex,
      (UINT32)SmBase,
      gSmmFeatureSmiStack,
      GdtBase,
      GdtSize,
      SMMSEG_PROTECT_MODE_CODE_SEGMENT,
      (IA32_DESCRIPTOR *)(UINTN)gProtModeIdtr
      );
  } else {
    gSmmFeatureSmbase = SmBase;
    CopyMem (
      (VOID*)(UINTN)(SmBase + SMM_HANDLER_OFFSET),
      (VOID*)gcSmmFeatureSmiHandlerTemplate,
      gcSmmFeatureSmiHandlerSize
      );
  }
}

/**
  Check to see if an SMM register is supported by a specified CPU.

  @param[in] CpuIndex  The index of the CPU to check for SMM register support.
                       The value must be between 0 and the NumberOfCpus field
                       in the System Management System Table (SMST).
  @param[in] RegName   Identifies the SMM register to check for support.

  @retval TRUE   The SMM register specified by RegName is supported by the CPU
                 specified by CpuIndex.
  @retval FALSE  The SMM register specified by RegName is not supported by the
                 CPU specified by CpuIndex.
**/
BOOLEAN
EFIAPI
SmmCpuFeaturesIsSmmRegisterSupported (
  IN UINTN         CpuIndex,
  IN SMM_REG_NAME  RegName
  )
{
  switch (mThisCpu->Family) {
  case CpuHaswell:
  case CpuSkylake:
    switch (RegName) {
    case SmmRegFeatureControl:
      return mSmmFeatureControlSupported;
    case SmmRegSmmEnable:
      return mSmmSyncFeature[CpuIndex].TargetedSmiSupported;
    case SmmRegSmmDelayed:
      return mSmmSyncFeature[CpuIndex].DelayIndicationSupported;
    case SmmRegSmmBlocked:
      return mSmmSyncFeature[CpuIndex].BlockIndicationSupported;
    }
  default:
    break;
  }
  return FALSE;
}

/**
  Returns the current value of the SMM register for the specified CPU.
  If the SMM register is not supported, then 0 is returned.

  @param[in] CpuIndex  The index of the CPU to read the SMM register.  The
                       value must be between 0 and the NumberOfCpus field in
                       the System Management System Table (SMST).
  @param[in] RegName   Identifies the SMM register to read.

  @return  The value of the SMM register specified by RegName from the CPU
           specified by CpuIndex.
**/
UINT64
EFIAPI
SmmCpuFeaturesGetSmmRegister (
  IN UINTN         CpuIndex,
  IN SMM_REG_NAME  RegName
  )
{
  if (SmmCpuFeaturesIsSmmRegisterSupported (CpuIndex, RegName)) {
    if (RegName == SmmRegFeatureControl) {
      return SmmReadReg64 (CpuIndex, RegName);
    }
    if ((SmmReadReg64 (CpuIndex, RegName) & mSmmSyncFeature[CpuIndex].HaswellLogProcEnBit) != 0) {
      return 1;
    }
  }
  return 0;
}

/**
  Sets the value of an SMM register on a specified CPU.
  If the SMM register is not supported, then no action is performed.

  @param[in] CpuIndex  The index of the CPU to write the SMM register.  The
                       value must be between 0 and the NumberOfCpus field in
                       the System Management System Table (SMST).
  @param[in] RegName   Identifies the SMM register to write.
                       registers are read-only.
  @param[in] Value     The value to write to the SMM register.
**/
VOID
EFIAPI
SmmCpuFeaturesSetSmmRegister (
  IN UINTN         CpuIndex,
  IN SMM_REG_NAME  RegName,
  IN UINT64        Value
  )
{
  UINT64  NewValue;

  if (SmmCpuFeaturesIsSmmRegisterSupported (CpuIndex, RegName)) {
    if (RegName == SmmRegFeatureControl) {
      SmmWriteReg64 (CpuIndex, RegName, Value);
    } else {
      NewValue = SmmReadReg64 (CpuIndex, RegName);
      if (Value != 0) {
        NewValue = NewValue | mSmmSyncFeature[CpuIndex].HaswellLogProcEnBit;
      } else {
        NewValue = NewValue & (~mSmmSyncFeature[CpuIndex].HaswellLogProcEnBit);
      }
      SmmWriteReg64 (CpuIndex, RegName, NewValue);
    }
  }
}

/**
  This function is hook point called after the gEfiSmmReadyToLockProtocolGuid
  notification is completely processed.
**/
VOID
EFIAPI
SmmCpuFeaturesCompleteSmmReadyToLock (
  VOID
  )
{
  DEBUG ((EFI_D_INFO, "SmmCpuFeaturesCompleteSmmReadyToLock...\n"));
  

  DEBUG ((EFI_D_INFO, "SmmCpuFeaturesCompleteSmmReadyToLock Done\n"));
}

/**
  This API provides a method for a CPU to allocate a specific region for storing page tables.
  
  This API can be called more than once to allocate the page table memory.

  Allocates the number of 4KB pages of type EfiRuntimeServicesData and returns a pointer to the
  allocated buffer.  The buffer returned is aligned on a 4KB boundary.  If Pages is 0, then NULL
  is returned.  If there is not enough memory remaining to satisfy the request, then NULL is
  returned.
  
  This function can also return NULL if there is no preference on where the page tables are allocated in SMRAM.

  @param  Pages                 The number of 4 KB pages to allocate.

  @return A pointer to the allocated buffer for page tables.
  @retval NULL      Fail to allocate a specific region for storing page tables,
                    Or there is no preference on where the page tables are allocated in SMRAM.

**/
VOID *
EFIAPI
SmmCpuFeaturesAllocatePageTableMemory (
  IN UINTN           Pages
  )
{
  if (Pages == 0) {
    return NULL;
  }

  return NULL;
}

