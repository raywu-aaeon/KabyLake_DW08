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

#include <DXE.h>
#include <AmiDxeLib.h>
#include <Protocol\NBPlatformData.h>
#include <Guid\NbChipsetGuid.h>
#include <Library\AmiCpuInitDataHobLib.h>
#include <Library\AmiSiPolicyHobLib.h>
#include <Library\AmiPchPolicyHobLib.h>

EFI_STATUS
CheckHobSizeAndDelete (
  IN EFI_GUID      *Guid,
  IN     UINT16    PeiPolcyDataSize,
  IN     UINT16    DxePolcyDataSize,
  OUT     VOID      *PeiRcPolicyHobPtr
  )
{
	  EFI_HOB_HANDOFF_INFO_TABLE           *pHit;
//	  UINT16                               HobDataSize;
   
	  pHit = GetEfiConfigurationTable(pST, &gEfiHobListGuid);

      if(!EFI_ERROR(FindNextHobByGuid(Guid, &pHit))) {
  	    DEBUG((-1, " found.... \n"));
    	 // HobDataSize = ((EFI_HOB_GENERIC_HEADER *)pHit)->HobLength - sizeof (EFI_HOB_GUID_TYPE);
    	  DEBUG((-1, " PeiPolcyDataSize = %X DxePolcyDataSize = %X\n" , PeiPolcyDataSize, DxePolcyDataSize));
    	 if ( PeiPolcyDataSize != DxePolcyDataSize) {
  		  //
  		  // delete incorrect Hob
  		  //
  		  ((EFI_HOB_GENERIC_HEADER *)pHit)->HobType = EFI_HOB_TYPE_UNUSED;
  		    PeiRcPolicyHobPtr = pHit;
    	    DEBUG((-1, " Del.... \n"));
  	      return EFI_SUCCESS;
  	     }
 	    DEBUG((-1, " No Del.... \n"));
	  }
      return EFI_NOT_FOUND;
}
/**
  Entry point for the driver.

  @param[in]  ImageHandle  Image Handle.
  @param[in]  SystemTable  EFI System Table.

  @retval     EFI_SUCCESS  Function has completed successfully.
  @retval     Others       All other error conditions encountered result in an ASSERT.
**/

EFI_STATUS
AmiDxeRecoveryRcHobResumeEntry (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HOB_HANDOFF_INFO_TABLE *pHit;
  BOOLEAN                    CreateNewHob;
  AMI_INTEL_RC_POLICY_SIZE_HOB    *mAmiIntelRcPolicySizeHob = NULL;
  UINT16              SiPolicyHobSize;
  UINT16              PchPolicyHobSize;
  UINT16              CpuInitDataHobSize;
  UINT8               *AmiRcHobPtr;
  VOID                *OrgSiPolicyHobPtr = NULL;
  VOID                *OrgPchPolicyHobPtr = NULL;
  VOID                *CpuInitDataHobPtr = NULL;
  
  InitAmiLib(ImageHandle,SystemTable);
  
  Status = EFI_SUCCESS;
  
  pHit = GetEfiConfigurationTable(pST, &gEfiHobListGuid);
  if (pHit != NULL) { 
     if(pHit->BootMode == BOOT_IN_RECOVERY_MODE) {
    	mAmiIntelRcPolicySizeHob = GetFirstGuidHob (&gAmiIntelRcPolicySizeHobGuid);
    	if(mAmiIntelRcPolicySizeHob == NULL) return EFI_UNLOAD_IMAGE;
    	SiPolicyHobSize =  mAmiIntelRcPolicySizeHob->SiPolicyHobSize;
    	PchPolicyHobSize =  mAmiIntelRcPolicySizeHob->PchPolicyHobSize;
    	CpuInitDataHobSize = mAmiIntelRcPolicySizeHob->CpuInitDataHobSize;
    	  
    	CreateNewHob = FALSE;
    	while(!EFI_ERROR(CheckHobSizeAndDelete(&gSiPolicyHobGuid, SiPolicyHobSize, sizeof(SI_POLICY_HOB), OrgSiPolicyHobPtr))) {
    		CreateNewHob = TRUE;
    	}
    	if (CreateNewHob) {
    		AmiRcHobPtr = NULL;
    		AmiRcHobPtr = GetFirstGuidHob (&gAmiIntelSiRcPolicyDummyHobGuid);
    		if(!EFI_ERROR(Status)){
    	 	    DEBUG((-1, "Dummy Hob update to Si RC policy hob.... \n"));
    		   ((EFI_HOB_GUID_TYPE *)AmiRcHobPtr)->Name = gSiPolicyHobGuid;
    		   AmiUpdateSiPolicyHob ((UINT8*)(AmiRcHobPtr + sizeof (EFI_HOB_GUID_TYPE)),\
    				   sizeof(SI_POLICY_HOB),\
    				   OrgSiPolicyHobPtr);
    		}		   
    	}
    	
    	CreateNewHob = FALSE;
    	while(!EFI_ERROR(CheckHobSizeAndDelete(&gPchPolicyHobGuid, PchPolicyHobSize, sizeof(PCH_POLICY_HOB), OrgPchPolicyHobPtr))) {
    		CreateNewHob = TRUE;
    	}
    	if (CreateNewHob) {
    		AmiRcHobPtr = NULL;
    		AmiRcHobPtr = GetFirstGuidHob (&gAmiIntelPchRcPolicyDummyHobGuid);
    		if(!EFI_ERROR(Status)){
    	 	    DEBUG((-1, "Dummy Hob update to Pch RC policy hob.... \n"));
    		   ((EFI_HOB_GUID_TYPE *)AmiRcHobPtr)->Name = gPchPolicyHobGuid;
    		   AmiUpdatePchPolicyHob ((UINT8*)(AmiRcHobPtr + sizeof (EFI_HOB_GUID_TYPE)),\
    				   sizeof(PCH_POLICY_HOB),\
    				   OrgPchPolicyHobPtr);
    		}	
    	}

    	CreateNewHob = FALSE;
    	while(!EFI_ERROR(CheckHobSizeAndDelete(&gCpuInitDataHobGuid, CpuInitDataHobSize, sizeof(CPU_INIT_DATA_HOB), CpuInitDataHobPtr))) {
    		CreateNewHob = TRUE;
    	}
    	if (CreateNewHob) {
    		AmiRcHobPtr = NULL;
    		AmiRcHobPtr = GetFirstGuidHob (&gAmiIntelCpuRcPolicyDummyHobGuid);
    		if(!EFI_ERROR(Status)){
    	 	    DEBUG((-1, "Dummy Hob update to Cpu RC policy hob.... \n"));
    		   ((EFI_HOB_GUID_TYPE *)AmiRcHobPtr)->Name = gCpuInitDataHobGuid;
    		   AmiUpdateCpuInitDataHob ((UINT8*)(AmiRcHobPtr + sizeof (EFI_HOB_GUID_TYPE)),\
    				   sizeof(CPU_INIT_DATA_HOB),\
    				   CpuInitDataHobPtr);
    		}	
    	}
     }
  }

  return EFI_UNLOAD_IMAGE;
}
