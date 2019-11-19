/** @file
  Source code file for the Platform Init DXE module

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor. This file may be modified by the user, subject to
  the additional terms of the license agreement.

@par Specification Reference:
**/
#include <Efi.h>
#include <Token.h>
#include <AmiCspLib.h>
#include <AmiHobs.h>
#include <Library\HobLib.h>
#include <Guid\HobList.h>
#include <Include\Private\CpuInitDataHob.h>
#include <Include\Private\SiPolicyHob.h>
#include <Include\Private\PchPolicyHob.h>

// {CD59A1C7-2DC4-44a3-A3AF-09DCA3C62E13}
#define AMI_INTEL_RC_POLICY_SIZE_HOB_GUID \
{ 0xcd59a1c7, 0x2dc4, 0x44a3, { 0xa3, 0xaf, 0x9, 0xdc, 0xa3, 0xc6, 0x2e, 0x13 } };

// {91D24E2E-EC3D-4d91-94FF-7E77857265A2}
#define AMI_INTEL_SI_RC_POLICY_DUMMY_HOB_GUID \
{ 0x91d24e2e, 0xec3d, 0x4d91, { 0x94, 0xff, 0x7e, 0x77, 0x85, 0x72, 0x65, 0xa2 } };

// {FBA30831-608A-475f-94BF-3356DC3DC218}
#define AMI_INTEL_PCH_RC_POLICY_DUMMY_HOB_GUID \
{ 0xfba30831, 0x608a, 0x475f, { 0x94, 0xbf, 0x33, 0x56, 0xdc, 0x3d, 0xc2, 0x18 } };

// {25AE7E55-A187-4005-B9B4-5712EA4A930E}
#define AMI_INTEL_CPU_RC_POLICY_DUMMY_HOB_GUID \
{ 0x25ae7e55, 0xa187, 0x4005, { 0xb9, 0xb4, 0x57, 0x12, 0xea, 0x4a, 0x93, 0xe } };


typedef struct _AMI_INTEL_RC_POLICY_SIZE_HOB {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    UINT16              SiPolicyHobSize;
    UINT16              PchPolicyHobSize;
    UINT16              CpuInitDataHobSize;
} AMI_INTEL_RC_POLICY_SIZE_HOB;

EFI_GUID            gAmiIntelRcPolicySizeHobGuid = AMI_INTEL_RC_POLICY_SIZE_HOB_GUID;
EFI_GUID            gAmiIntelSiRcPolicyDummyHobGuid = AMI_INTEL_SI_RC_POLICY_DUMMY_HOB_GUID;
EFI_GUID            gAmiIntelPchRcPolicyDummyHobGuid = AMI_INTEL_PCH_RC_POLICY_DUMMY_HOB_GUID;
EFI_GUID            gAmiIntelCpuRcPolicyDummyHobGuid = AMI_INTEL_CPU_RC_POLICY_DUMMY_HOB_GUID;

#include <Pei.h>
#include <AmiPeiLib.h>

/**
    This function is the entry point for this PEI.
    it must be ported to do AmiSaveRcHobSizeEntry specific programming needed
    at power-on, both in wakeup path as well as power-on path.

    @param FfsHeader   Pointer to the FFS file header
        PeiServices Pointer to the PEI services table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

    @note  This function should initialize North Bridge before memory
              detection.
              Install AMI_PEI_NBINIT_POLICY_PPI to indicate that NB Init
              PEIM is installed
**/

EFI_STATUS EFIAPI AmiPeiCreateDummyRcHobEntry (
    IN EFI_FFS_FILE_HEADER      *FfsHeader,
    IN EFI_PEI_SERVICES         **PeiServices )
{
    EFI_STATUS          Status;
    UINT16              HobDataSize;
    UINT8               *DummyPtr;
    EFI_BOOT_MODE       BootMode;
    
    AMI_INTEL_RC_POLICY_SIZE_HOB    *AmiIntelRcPolicySizeHob;
    
    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (BootMode != BOOT_IN_RECOVERY_MODE) return EFI_UNLOAD_IMAGE;
        
    Status = (*PeiServices)->CreateHob (PeiServices,
                                        EFI_HOB_TYPE_GUID_EXTENSION,
                                        sizeof(AMI_INTEL_RC_POLICY_SIZE_HOB),
                                        &AmiIntelRcPolicySizeHob);
    if(EFI_ERROR(Status)) return Status;
    
    AmiIntelRcPolicySizeHob->EfiHobGuidType.Name = gAmiIntelRcPolicySizeHobGuid;
    AmiIntelRcPolicySizeHob->SiPolicyHobSize = sizeof(SI_POLICY_HOB);
    AmiIntelRcPolicySizeHob->PchPolicyHobSize = sizeof(PCH_POLICY_HOB);
    AmiIntelRcPolicySizeHob->CpuInitDataHobSize = sizeof(CPU_INIT_DATA_HOB);

    //
    // Create the dummy HOB of Si policy.
    //
    DummyPtr = NULL;
    HobDataSize = (sizeof(EFI_HOB_GUID_TYPE) + (sizeof(SI_POLICY_HOB) * 2)); 
    Status = (*PeiServices)->CreateHob (PeiServices,
                                        EFI_HOB_TYPE_GUID_EXTENSION,
                                        HobDataSize,
                                        &DummyPtr);
    if(!EFI_ERROR(Status)) {
    	((EFI_HOB_GUID_TYPE *)DummyPtr)->Name = gAmiIntelSiRcPolicyDummyHobGuid;
    }
    
    //
    // Create the dummy HOB of Pch policy.
    //
    DummyPtr = NULL;
    HobDataSize = (sizeof(EFI_HOB_GUID_TYPE) + (sizeof(PCH_POLICY_HOB) * 2));
    Status = (*PeiServices)->CreateHob (PeiServices,
                                        EFI_HOB_TYPE_GUID_EXTENSION,
                                        HobDataSize,
                                        &DummyPtr);
    if(!EFI_ERROR(Status)) {
    	((EFI_HOB_GUID_TYPE *)DummyPtr)->Name = gAmiIntelPchRcPolicyDummyHobGuid;
    }
    
    //
    // Create the dummy HOB of Cpu policy.
    //
    DummyPtr = NULL;
    HobDataSize = (sizeof(EFI_HOB_GUID_TYPE) + (sizeof(CPU_INIT_DATA_HOB) * 2));
    Status = (*PeiServices)->CreateHob (PeiServices,
                                        EFI_HOB_TYPE_GUID_EXTENSION,
                                        HobDataSize,
                                        &DummyPtr);
    if(!EFI_ERROR(Status)) {
    	((EFI_HOB_GUID_TYPE *)DummyPtr)->Name = gAmiIntelCpuRcPolicyDummyHobGuid;
    }
    
    return EFI_UNLOAD_IMAGE;
}
